/**
 * @file vehicles.c
 * 
 * @author Tiago Firmino - ist1103590
 * 
 * File containing functions used to manage
 * vehicles used in the program.
 * 
*/

#include "project.h"
#include "prototypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Creates a new vehicle initializing its values correctly
 * and inserts it into the system's vehicle hash table.
 * Returns the newly created vehicle.
*/
vehicle_t* add_vehicle(char* license_plate, entry_t* entry,
     timestamp_t entry_d, system_t* sys) {
    vehicle_t* new_vehicle = (vehicle_t*)safe_malloc(sizeof(vehicle_t));
    new_vehicle->last_entry = entry_d;
    new_vehicle->current_entry = entry;

    strcpy(new_vehicle->license_plate, license_plate);
    insert_ht(sys->vhc_ht, new_vehicle);
    return new_vehicle;
    
}

/**
 * Checks whether the given string is a valid license plate.
 * returns TRUE only if the string has 
 * the correct license plate format.
*/
int is_license_plate(char* s) {
    int letterPair = FALSE,
        digitPair = FALSE,
        digit3Pair = FALSE;
    int counter = 0;
    if (strlen(s) != 8) {
        return FALSE;
    }

    for (int i = 0; i < 8; i++) {

        if ((i == 2 || i == 5) && s[i] != '-') {
            return FALSE;
        } else if (i == 2 || i == 5) {
            continue;
        }

        
        if (i % 3 == 0) {
            if (isupper_char(s[i]) && isupper_char(s[i+1])) { // Letter pair
                letterPair = TRUE;
            } else if (is_digit(s[i]) && is_digit(s[i+1])) { // Digit pair
                counter++;
                digitPair = TRUE;
            } else {
                return FALSE;
            }
            i++;
        }
    }
    if (counter == 3) {
        digit3Pair = TRUE;
        return digit3Pair;
    }
    return letterPair && digitPair;
}

/**
 * Checks for invalid arguments of the command 'v'.
*/
int invalid_vehicle_args(char* license_plate) {
    if (!is_license_plate(license_plate)) {
        printf(VEHICLE_INVALID_LICENSE, license_plate);
    } else {
        return FALSE;
    }
    return TRUE;
}

/**
 * Lists the given license plate's vehicle entries and exits
 * recorded in the system, which are sorted
 * firstly by the park name
 * and subsequently by the entry date and time.
*/
void vehicle_activity_logs(char* license_plate, system_t* sys) {
    int count = 0;
    
    node_t* park_node = sys->srtd_parks->head;
    while (park_node) {
        park_t* park = (park_t*)park_node->val;
        count += log_vehicle_activities_in_park(license_plate, park);
        park_node = park_node->next;
    }

    // No entries found
    if (count == 0)
        printf(VEHICLE_NO_REGISTRY, license_plate);
}


int log_vehicle_activities_in_park(char* license_plate, park_t* park) {
    int activity_count = 0;
    node_t* entry_node = park->park_entries->head;

    while (entry_node) {
        entry_t* entry = (entry_t*)entry_node->val;
        if (!strcmp(entry->vehicle->license_plate, license_plate)) {
            activity_count++;
            print_entries(entry);
            print_corresponding_exit_if_exists(license_plate, entry, park);
        }
        entry_node = entry_node->next;
    }
    return activity_count;
}


void print_corresponding_exit_if_exists(char* license_plate, entry_t* entry, park_t* park) {
    node_t* exit_node = park->park_exits->head;
    while(exit_node) {
        exit_t* exit = (exit_t*)exit_node->val;
        if (!strcmp(exit->vehicle->license_plate, license_plate) &&
            compare_date_time(exit->exit_date_time, entry->entry_date_time) >= 0) {
            print_corresponding_exits(exit);
            return;
        }
        exit_node = exit_node->next;
    }
    printf("\n");
}


/**
 * Prints vehicle sorted entries.
*/
void print_entries(entry_t* entry) {
    printf("%s %02d-%02d-%4d %02d:%02d", 
        entry->park_name,
        entry->entry_date_time.d, 
        entry->entry_date_time.mth,
        entry->entry_date_time.y, 
        entry->entry_date_time.h,
        entry->entry_date_time.min);
}

/**
 * Prints vehicle corresponding exits.
*/
void print_corresponding_exits(exit_t* corresponding_exit) {
    printf(" %02d-%02d-%4d %02d:%02d\n",
        corresponding_exit->exit_date_time.d,
        corresponding_exit->exit_date_time.mth,
        corresponding_exit->exit_date_time.y,
        corresponding_exit->exit_date_time.h,
        corresponding_exit->exit_date_time.min);
}