/**
 * @file prototypes.h
 * 
 * @author Tiago Firmino - ist1103590
 * 
 * Header file for the prototypes of the functions of the program.
 * 
*/

#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#include "project.h"

/* Prototypes for all functions of the project. */

/*************/
/* project.c */
/*************/

system_t* init_system();

int command_processor(char command, system_t* sys, char* buffer);

void exec_show_val(system_t* sys);

void exec_create_parking(system_t* sys, char* buffer);

void exec_register_entry(system_t* sys, char* buffer);

void exec_register_exit(system_t* sys, char* buffer);

void exec_log_vehicle_activity(system_t* sys);

void exec_park_facturation(system_t* sys, char* buffer);

void exec_remove_park(system_t* sys, char* buffer);

char* parse_allocate_name(char* buffer);

void read_until_end(char* s);

int read_name(char* s);

int read_spaces();

int isupper_char(char c);

int is_digit(char c);

char *duplicate_string(const char* str);

void *safe_malloc(unsigned size);

void free_parks(list_t* parks);

void free_hashtable(hash_table* hashtable);

void free_mem(system_t* sys, char* buffer);

/***********/
/* parks.c */
/***********/

void create_parking(char* name, int capacity,
 tariff_t tariff, system_t* sys);

void list_parks(system_t* sys);

void remove_parks(park_t* park, system_t* sys);

int invalid_park_args(char* park_name, int capacity, 
 tariff_t tariff, system_t* sys);

park_t* lookup_park(char* park_name, system_t* sys);

/***************/
/* movements.c */
/***************/

void register_entry(park_t* park, char* license_plate,
 timestamp_t entry_d, system_t* sys);

void register_exit(park_t* park, char* license_plate,
 timestamp_t exit_d, system_t* sys);

int invalid_movement_args(park_t* park, char* license_plate,
 timestamp_t date, system_t* sys, int is_entry);

int validate_movement_date(timestamp_t date, system_t* sys);

int validate_entry_park_capacity(park_t* park, int is_entry);

int validate_license_plate(char* license_plate);

int validate_vehicle_entry(vehicle_t* vhc, int is_entry);

int validate_vehicle_exit(vehicle_t* vhc, int is_entry,
 park_t* park, char* license_plate);

float calculate_facturation(timestamp_t entry,
 timestamp_t exit, tariff_t tariff);

int invalid_factdate_args(timestamp_t facturation_date, 
 system_t* sys);

void print_facturation_by_day(park_t* park,
    timestamp_t facturation_date);

void print_facturation(park_t* park);

/**************/
/* vehicles.c */
/**************/

vehicle_t* add_vehicle(char* license_plate, entry_t* entry,
 timestamp_t entry_d, system_t* sys);

int is_license_plate(char* s);

int invalid_vehicle_args(char* license_plate);

void vehicle_activity_logs(char* license_plate, system_t* sys);

int log_vehicle_activities_in_park(char* license_plate,
 park_t* park);

void print_corresponding_exit_if_exists(char* license_plate,
 entry_t* entry, park_t* park);

void print_entries(entry_t* entry);

void print_corresponding_exits(exit_t* corresponding_exit);

/****************/
/* structures.c */
/****************/

list_t* init_list();

void insert_list(list_t* list, void* elem);

void sorted_insert_list(list_t* list, void* elem, char type);

int compare_elements(void* elem1, void* elem2, char type);

void insert_before_node(node_t* new_node, node_t* current,
 list_t* list);

void insert_at_tail(node_t* new_node, list_t* list);

void delete_list(list_t* list);

void delete_node(list_t* list, void* val);

int hash(char* plate);

hash_table* init_ht();

void insert_ht(hash_table* hashtable, vehicle_t* vehicle);

vehicle_t* search_ht(hash_table* hashtable, char* plate);

/***********/
/* dates.c */
/***********/

int get_time_in_mins(timestamp_t ts);

int get_month_mins(int month, int year);

int compare_date_time(timestamp_t d1, timestamp_t d2);

int compare_date(timestamp_t d1, timestamp_t d2);

int invalid_date(timestamp_t ts, system_t* sys,
 int is_facturation);

int is_leap_year(int year);

int check_feb29(timestamp_t entry, timestamp_t exit);

#endif
