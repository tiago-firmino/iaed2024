/**
 * @file parks.c
 * 
 * @author Tiago Firmino - ist1103590 
 * 
 * File containing functions used in order to manage parks.
 *
*/

#include "project.h"
#include "prototypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Creates a new park initializing its values
 * and inserts it into the system's park list
 * and inserts it into the system's sorted park list (by park name),
 * incrementing the number of parks in the system.
*/
void create_parking(char* park_name, int capacity,
                     tariff_t tariff, system_t* sys) {

    park_t* new_park = (park_t*)safe_malloc(sizeof(park_t));
    new_park->park_name = park_name;
    new_park->park_capacity = capacity;

    new_park->park_tariff.first_hour_price =
     tariff.first_hour_price;
    new_park->park_tariff.hour_price =
     tariff.hour_price;
    new_park->park_tariff.max_daily_price =
     tariff.max_daily_price;

    new_park->num_vehicles = 0;
    
    new_park->park_entries = init_list();
    new_park->park_exits = init_list();
    new_park->park_vehicles = init_list();
    
    sys->num_parks++;
    insert_list(sys->parks, new_park);
    sorted_insert_list(sys->srtd_parks, new_park, PARK_COMMAND);
}

/**
 * Lists all parks that are in the system in order of creation.
*/
void list_parks(system_t* sys) {
    node_t* current = sys->parks->head;
    while (current != NULL) {
        park_t* p = (park_t*)current->val;
        printf("%s %d %d\n", p->park_name, p->park_capacity,
         p->park_capacity - p->num_vehicles);
        current = current->next;
    }
}

/**
 * Removes a park node and its dependencies from the system,
 * notably, deletes the park's entries and exits lists as well
 * as the park's vehicle list and frees the park name.
 * Then lists the remaining parks sorted by park name.
*/
void remove_parks(park_t* park, system_t* sys) {    
    delete_list(park->park_entries);
    delete_list(park->park_exits);
    
    sys->num_parks--;
    delete_node(sys->parks, park);

    node_t* current_vehicle = park->park_vehicles->head;
    while (current_vehicle) {
        vehicle_t* vehicle = 
         (vehicle_t*)current_vehicle->val;
        vehicle->current_entry = NULL;
        current_vehicle = current_vehicle->next;
        delete_node(park->park_vehicles, vehicle);
    }
    free(park->park_vehicles);
    
    node_t* current_park = sys->srtd_parks->head;
    while (current_park != NULL) {
        park_t* temp_park = (park_t*)current_park->val;
        if (park == temp_park) {
            node_t* temp = current_park->next;
            delete_node(sys->srtd_parks, temp_park);
            current_park = temp;
            continue;
        }
        printf("%s\n", temp_park->park_name);

        current_park = current_park->next;
    }
    free(park->park_name);
    free(park);
}

/**
 * Checks for invalid arguments of the command 'p'.
*/
int invalid_park_args(char* park_name, int capacity, 
                            tariff_t f, system_t* sys) {

    park_t* park = lookup_park(park_name, sys);

    if (sys->num_parks >= MAX_P) {
        printf(PARK_MAX_EXCEEDED);

    } else if (park) {
        printf(PARK_DUPLICATE, park_name);

    } else if (capacity <= 0) {
        printf(PARK_CAPACITY_INVALID, capacity);

    } else if (f.first_hour_price <= 0 ||
            f.hour_price <= 0 ||
            f.max_daily_price <= 0 ||
            !(f.first_hour_price < f.hour_price 
            && f.hour_price < f.max_daily_price)) {
        printf(PARK_INVALID_TARIFARY);

    } else {
        return FALSE;
    }
    return TRUE;
}

/**
 * Performs a lookup for the given park name in the 
 * sytem's park list.
*/
park_t* lookup_park(char* park_name, system_t* sys) {
    node_t* current = sys->parks->head;
    while (current != NULL) {
        park_t* park = (park_t*)current->val;

        if (!strcmp(park->park_name, park_name))
            return park;

        current = current->next;
    }
    return NULL;
}
