/**
 * @file movements.c
 * 
 * @author Tiago Firmino - ist1103590
 * 
 * File containing functions used to manage
 * the movements such as entries and exits
 * used in the program, as well as park
 * facturations.
 * 
*/

#include "project.h"
#include "prototypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


/**
 * Creates a new entry initializing its values
 * and inserts it into the park's entries list
 * sorted by the entry date.
 * and the number of vehicles in its park.
 * If the given vehicle is new, add it to the system's
 * vehicle hash table and the park's vehicle list,
 * incrementing the number of vehicles of that park.
 * Prints the park where the entry was made and the
 * available park slots.
*/
void register_entry(park_t* park,
                char* license_plate, 
                timestamp_t entry_d,
                system_t* sys) {

    entry_t* new_entry = (entry_t*)safe_malloc(sizeof(entry_t));

    vehicle_t* vhc = search_ht(sys->vhc_ht, license_plate);
    if (vhc == NULL)
        vhc = add_vehicle(license_plate, new_entry, entry_d, sys);
    else {
        vhc->last_entry = entry_d;
        vhc->current_entry = new_entry;
    }
    new_entry->vehicle = vhc;
    strcpy(new_entry->vehicle->license_plate, license_plate);
    new_entry->park_name = park->park_name;
    new_entry->entry_date_time = entry_d;

    sys->date_registry = entry_d;
    
    park->num_vehicles++;
    insert_list(park->park_vehicles, vhc);
    
    sorted_insert_list(park->park_entries, new_entry, ENTRY_COMMAND);

    printf("%s %d\n", park->park_name,
     park->park_capacity - park->num_vehicles);
    
}

/**
 * Creates a new exit initializing its values
 * and inserts it into the park's exits list
 * sorted by exit date.
 * Sets the vehicle's current entry to NULL
 * and decreases the number of vehicles in that park,
 * deleting that vehicle node from the park's vehicle list.
 * Calculates the total facturation for the
 * period in which the vehicle stayed inside the park.
*/
void register_exit(park_t* park,
                char* license_plate,
                timestamp_t exit_d,
                system_t* sys) {
    
    exit_t* new_exit = (exit_t*)safe_malloc(sizeof(exit_t));

    vehicle_t* vhc = search_ht(sys->vhc_ht, license_plate);
    vhc->current_entry = NULL;

    new_exit->park_name = park->park_name;
    new_exit->vehicle = vhc;
    strcpy(new_exit->vehicle->license_plate, license_plate);
    new_exit->exit_date_time = exit_d;

    sys->date_registry = exit_d;

    float paid_value = calculate_facturation(vhc->last_entry,
     exit_d, park->park_tariff);
    new_exit->paid_value = paid_value;

    park->num_vehicles--;
    delete_node(park->park_vehicles, vhc);

    sorted_insert_list(park->park_exits, new_exit, EXIT_COMMAND);

    printf("%s %02d-%02d-%4d %02d:%02d %02d-%02d-%4d %02d:%02d %.2f\n",
        license_plate, vhc->last_entry.d,
         vhc->last_entry.mth, vhc->last_entry.y,
         vhc->last_entry.h, vhc->last_entry.min, 
         exit_d.d, exit_d.mth, exit_d.y,
         exit_d.h, exit_d.min, new_exit->paid_value);
}


/**
 * Checks for invalid arguments of the commands 'e' and 's'.
*/
int invalid_movement_args(park_t* park, char* license_plate,
 timestamp_t date, system_t* sys, int is_entry) {
    vehicle_t* vhc = search_ht(sys->vhc_ht, license_plate);

    if (validate_movement_date(date, sys)) return TRUE;
    if (validate_entry_park_capacity(park, is_entry)) return TRUE;
    if (validate_license_plate(license_plate)) return TRUE;
    if (validate_vehicle_entry(vhc, is_entry)) return TRUE;
    if (validate_vehicle_exit(vhc, is_entry, park, license_plate)) return TRUE;

    return FALSE;
}

/**
 * Checks if the given date is invalid 
 * or if the day of date is the 29th of february.
*/
int validate_movement_date(timestamp_t date, system_t* sys) {
    if (invalid_date(date, sys, FALSE) ||
        (date.d == 29 && date.mth == 2)) {
        printf(INVALID_DATE);
        return TRUE;
    }
    return FALSE;
}

/**
 * Checks in case of an entry if the park capacity
 * has already been exceeded.
*/
int validate_entry_park_capacity(park_t* park, int is_entry) {
    if (is_entry && park->num_vehicles == park->park_capacity) {
        printf(PARK_CAPACITY_EXCEEDED, park->park_name);
        return TRUE;
    }
    return FALSE;
}

/**
 * Checks if the given license plate is valid.
*/
int validate_license_plate(char* license_plate) {
    if (!is_license_plate(license_plate)) {
        printf(VEHICLE_INVALID_LICENSE, license_plate);
        return TRUE;
    }
    return FALSE;
}

/**
 * Checks in case of an entry, if the given vehicle
 * has already made an entry.
*/
int validate_vehicle_entry(vehicle_t* vhc, int is_entry) {
    if (is_entry && vhc && vhc->current_entry) {
        printf(VEHICLE_INVALID_ENTRY, vhc->license_plate);
        return TRUE;
    }
    return FALSE;
}

/**
 * Checks in case of an exit, if the vehicle exists,
 * if the vehicle made an entry on the given park that it is
 * trying to exit and if the vehicle is not currently
 * in a park.
*/
int validate_vehicle_exit(vehicle_t* vhc, int is_entry,
 park_t* park, char* license_plate) {
    if ((!is_entry && !vhc) || 
        (!is_entry && vhc && vhc->current_entry && 
        strcmp(vhc->current_entry->park_name,
         park->park_name) != 0) || 
        (!is_entry && vhc && !vhc->current_entry)) {
        printf(VEHICLE_INVALID_EXIT, license_plate);
        return TRUE;
    }
    return FALSE;
}


/**
 * Calculates the total facturation for the
 * period in which a vehicle stayed inside a park by using
 * the park's tariffs and the system's date and time logic.
*/
float calculate_facturation(timestamp_t entry, 
                            timestamp_t exit, tariff_t tariff) {

    int entry_min = get_time_in_mins(entry);
    int exit_min = get_time_in_mins(exit);
    
    // Total parking duration in minutes.
    int total_duration = exit_min - entry_min;
    if(total_duration == 0) return 0;

    // Adjust for February 29th if needed.
    if (check_feb29(entry, exit))
        total_duration -= 1440; // Subtract 24 hours in minutes for February 29.

    // Calculate total full days and remaining minutes.
    int full_days = total_duration / (24 * 60);
    int remaining_minutes = total_duration % (24 * 60);

    // Calculate charges for full days.
    float total_charge = full_days * tariff.max_daily_price;

    if (remaining_minutes == 0) return total_charge;
    
    float remaining_charge = 0;
    // Calculate charges for remaining minutes.
    // Charges for the first hour (up to 4 intervals of 15 minutes each).
    int intervals = (remaining_minutes >= 60) ? 4 :
                    (remaining_minutes - 1) / 15 + 1;

    remaining_minutes -= 
     (remaining_minutes - intervals * 15 < 0) ? remaining_minutes :
                                                intervals * 15;

    remaining_charge += intervals * tariff.first_hour_price;

    // Charges after the first hour.
    intervals = 
     (remaining_minutes % 15) == 0 ? (remaining_minutes / 15) : 
                                     (remaining_minutes / 15) + 1;

    remaining_charge += intervals * tariff.hour_price;

    total_charge += (remaining_charge > tariff.max_daily_price) ?
      tariff.max_daily_price : remaining_charge;
    
    return total_charge;
}

/**
 * Checks for invalid arguments of the command 'f' with two arguments.
*/
int invalid_factdate_args(timestamp_t facturation_date,
            system_t* sys) {
                
    if (invalid_date(facturation_date, sys, TRUE)) {
        printf(INVALID_DATE);
    } else {
        return FALSE;
    }
    return TRUE;
}

/**
 * Shows the facturation of a given park on a given day,
 * sorted by the exit date and time.
*/
void print_facturation_by_day(park_t* park,
            timestamp_t facturation_date) {
    
    int started = FALSE;
    node_t* current_node = park->park_exits->head;
    while (current_node) {
        exit_t* exit = (exit_t*)current_node->val;
        int compare = compare_date(exit->exit_date_time,
                               facturation_date);
        if (started && compare) {
            break;
        }
        if (exit && !compare) {
            started = TRUE;
            printf("%s %02d:%02d %.2f\n",
                exit->vehicle->license_plate,
                exit->exit_date_time.h, 
                exit->exit_date_time.min,
                exit->paid_value);
        }
        current_node = current_node->next;
    }
}


/**
 * Shows the daily facturation of a given park since its creation,
 * sorted by date.
*/
void print_facturation(park_t* park) {

    node_t* current_node = park->park_exits->head;
    if (current_node == NULL) return;

    exit_t* exit = (exit_t*)current_node->val;
    timestamp_t previous_date = exit->exit_date_time;
    float daily_value = 0;

    while (current_node) {
        exit = current_node->val;
        timestamp_t temp_date = exit->exit_date_time;
        if (compare_date(previous_date, temp_date)) {
            // Different date then previous
            printf("%02d-%02d-%4d %.2f\n",
               previous_date.d, previous_date.mth,
               previous_date.y, daily_value);
            daily_value = 0;
        }
        daily_value += exit->paid_value;
        previous_date = exit->exit_date_time;
        
        current_node = current_node->next;  
    }
    printf("%02d-%02d-%4d %.2f\n",
               previous_date.d, previous_date.mth,
               previous_date.y, daily_value);
}
