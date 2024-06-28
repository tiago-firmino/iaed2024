/**
 * @file project.c
 * 
 * @author Tiago Firmino - ist1103590 
 * 
 * File containing main() and other primary functions for the program.
 * 
*/

#include "project.h"
#include "prototypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * The main function of the program.
 * Creates the global system struct.
 * Creates a buffer for input reading.
 * Repeatedly waits for a new command.
 * Ends the program by freeing all the used memory.
 */
int main() {
	char* buffer;
	system_t* sys = init_system();
	buffer = safe_malloc(MAX_LINE_SIZE * sizeof(char));
	while (command_processor(getchar(), sys, buffer));
	free_mem(sys, buffer);
	return 0;
}

/**
 * Initializes the system struct.
 * Creates a new system and initializes the
 * park entries and park exits linked lists,
 * as well as the vehicle hash table.
 * Sets the park counter value to 0 and defines
 * the first date of the program as 01-01-2024.
 * Returns newly allocated system.
*/
system_t* init_system() {
    system_t* new_system = (system_t*)safe_malloc(sizeof(system_t));

    new_system->parks = init_list();
	new_system->srtd_parks = init_list();

	new_system->vhc_ht = init_ht();

    new_system->num_parks = 0;

    new_system->date_registry.y = 2024;
	new_system->date_registry.mth = 1;
	new_system->date_registry.d = 1;
	new_system->date_registry.h = 0;
	new_system->date_registry.min = 0;

    return new_system;
}

/**
 * Handles command input.
 * A string is passed as an argument to store the command
 * from stdin, a buffer is also passed as an argument,
 * which should be reused across commands to read.
 * If the program should continue after the command, returns 1.
 * Otherwise returns 0 exiting the program successfully.
*/
int command_processor(char command, system_t* sys, char* buffer) {
	switch (command) {
		case QUIT_COMMAND:
			return EXIT_SUCCESS;
		
		case PARK_COMMAND:
			exec_create_parking(sys, buffer);
			return 1;

		case ENTRY_COMMAND:
			exec_register_entry(sys, buffer);
			return 1;
		
		case EXIT_COMMAND:
			exec_register_exit(sys, buffer);
			return 1;
		
		case VEHICLE_COMMAND:
			exec_log_vehicle_activity(sys);
			return 1;
		
		case FACT_COMMAND:
			exec_park_facturation(sys, buffer);
			return 1;

		case REMOVE_COMMAND:
			exec_remove_park(sys, buffer);
			return 1;

		case PAID_COMAMND:
			exec_show_val(sys);
			return 1;
		default:
	        if (command == ' ' || command == '\t' || command == '\n') break;
	}
	return 1;
}

void exec_show_val(system_t* sys) {
	char license_plate[V_LICENSE_PLT_LENGTH];
	float total_paid = 0;

	read_spaces();
	scanf("%s", license_plate);

	if (validate_license_plate(license_plate)) {
		return;
	}
 
	vehicle_t* vhc = search_ht(sys->vhc_ht, license_plate);
	
	//if (vhc) {
	node_t* current_park = sys->parks->head;
	while (current_park){
		park_t* park = (park_t*)current_park->val;
		node_t* current_exit = park->park_exits->head;
		exit_t* exit = (exit_t*)current_exit->val;
		while (current_exit) {
			if (vhc == exit->vehicle) {
				total_paid += exit->paid_value;
			}
			current_exit =  current_exit->next;

		}

		current_park = current_park->next;
	}

	//}
	printf("%.2f\n",total_paid);
}


/**
 * Handles the 'p' command.
 * Adds a park to the system, or lists every park
 * if no arguments are given.
 */
void exec_create_parking(system_t* sys, char* buffer) {
	char c = read_spaces();
	int capacity;
	float first_hour_price, hour_price, max_daily_price;
	tariff_t tariff;

	if (!c) {
		list_parks(sys);
		return;
	}
	char* park_name = parse_allocate_name(buffer);
	if (!strcmp(park_name, "invalid")) {
		printf(PARK_INVALID_NAME);
		return;
	}
	c = read_spaces();
	if (c) {
		scanf("%d %f %f %f", &capacity,
				&first_hour_price, &hour_price, &max_daily_price);
		tariff.first_hour_price = first_hour_price;
		tariff.hour_price = hour_price;
		tariff.max_daily_price = max_daily_price;
		if (invalid_park_args(park_name, capacity, tariff, sys)) {
			free(park_name);
			return;
		} else {
			char* park_name_dup = duplicate_string(park_name);
			create_parking(park_name_dup, capacity, tariff, sys);
		}
	}
	free(park_name);
	read_until_end(buffer);
}

/**
 * Handles the 'e' command.
 * Registers the entry of a vehicle into a park to the system.
 */
void exec_register_entry(system_t* sys, char* buffer) {
	char license_plate[V_LICENSE_PLT_LENGTH];
	int is_entry = TRUE;
	timestamp_t entry_date;
	read_spaces();
	char* park_name = parse_allocate_name(buffer);
	read_spaces();

	scanf("%s", license_plate);
	if (scanf("%02d-%02d-%4d %02d:%02d", 
			&entry_date.d, &entry_date.mth,
			&entry_date.y, &entry_date.h, &entry_date.min) != 5) {
		printf(INVALID_DATE);
		free(park_name);
		return;
	}
	park_t* park = lookup_park(park_name, sys);
	if (!park) {
        printf(PARK_DOESNT_EXIST, park_name);
		free(park_name);
		return;
	} else if (invalid_movement_args(park, license_plate,
		 entry_date, sys, is_entry)) {

		free(park_name);
		return;
	}
	register_entry(park, license_plate, entry_date, sys);
	free(park_name);
	read_until_end(buffer);
}

/**
 * Handles the 's' command.
 * Regists the exit of a vehicle from a park to the system.
 */
void exec_register_exit(system_t* sys, char* buffer) {
	char license_plate[V_LICENSE_PLT_LENGTH];
	timestamp_t exit_date;
	int is_entry = FALSE;
	read_spaces();
	char* park_name = parse_allocate_name(buffer);
	read_spaces();

	scanf("%s", license_plate);
	if (scanf("%02d-%02d-%4d %02d:%02d", 
			&exit_date.d, &exit_date.mth,
			&exit_date.y, &exit_date.h, &exit_date.min) != 5) {
		printf(INVALID_DATE);
		free(park_name);
		return;
	}
	park_t* park = lookup_park(park_name, sys);
	if (!park) {
        printf(PARK_DOESNT_EXIST, park_name);
		free(park_name);
		return;
	} else if (invalid_movement_args(park, license_plate,
		 exit_date, sys, is_entry)) {
			
		free(park_name);
		return;
	}
	register_exit(park, license_plate, exit_date, sys);
	free(park_name);
	read_until_end(buffer);
}

/**
 * Handles the 'v' command.
 * Lists the entries and exits of a vehicle
 * sorted firstly by park name and then by
 * entry date and hour. If the vehicle is not
 * in a park, the exit date and hour is not shown.
 */
void exec_log_vehicle_activity(system_t* sys) {
	char license_plate[V_LICENSE_PLT_LENGTH];

	read_spaces();
	scanf("%s", license_plate);
	
	if (invalid_vehicle_args(license_plate)) return;

	vehicle_activity_logs(license_plate, sys);
}

/**
 * Handles the 'f' command.
 * Presents the facturation of a park sorted by date
 * if one argument is given or sorted by hour of exit
 * of the given day if two arguments are given.
 */
void exec_park_facturation(system_t* sys, char* buffer) {
	char c = ' ';
	timestamp_t facturation_date;
	read_spaces();
	char* park_name = parse_allocate_name(buffer);
	c = read_spaces();

	park_t* park = lookup_park(park_name, sys);
	if (!park) {
		printf(PARK_DOESNT_EXIST, park_name);
		free(park_name);
		return;
	}
	if (c) {
		if (scanf("%02d-%02d-%4d",
			&facturation_date.d, 
			&facturation_date.mth,
			&facturation_date.y) != 3) {
			printf(INVALID_DATE);
			free(park_name);
			return;
		}
		facturation_date.h = 0;
		facturation_date.min = 0;
		if (compare_date(facturation_date,
			 sys->date_registry) > 0) {
			printf(INVALID_DATE);
			free(park_name);
			return;
		}
		if (invalid_factdate_args(facturation_date, sys)) {
			free(park_name);
			return;
		}
		print_facturation_by_day(park, facturation_date);
	} else {
		print_facturation(park);
	}
	free(park_name);
}

/**
 * Handles the 'r' command.
 * Removes a park from the system
 * removing all entries and exits of that park
 * and listing the remaining parks sorted by park name.
 */
void exec_remove_park(system_t* sys, char* buffer) {
	char* park_name; 
	int name_len = 0;
	read_spaces();
	name_len = read_name(buffer);
	park_name = safe_malloc(name_len * (sizeof(char)));
	memcpy(park_name, buffer, name_len);
	read_spaces();
	
	park_t* park = lookup_park(park_name, sys); 
	if (!park) {
		printf(PARK_DOESNT_EXIST, park_name);
		free(park_name);
		return;
	}
	remove_parks(park, sys);
	free(park_name);
}


/*********/
/* Utils */
/*********/

/**
 * Parses a name from the provided buffer, allocates memory for it, 
 * and copies it into the newly allocated string.
*/
char* parse_allocate_name(char* buffer) {
    int name_len = read_name(buffer);
	char* invalid = "invalid";
	if (name_len == -1) {
		return invalid;
	}
    char* name = safe_malloc(name_len * sizeof(char));
    memcpy(name, buffer, name_len);
    
    return name; 
}

/**
 * Reads the text until the end of the line.
*/
void read_until_end(char* s) {
	char c;
	int i = 0;
	while ((c = getchar()) != '\n') {
		s[i++] = c;
	}
	s[i] = '\0';
}

/**
 * Reads the name from the received string, returns the number of read chars.
*/
int read_name(char* s) {
	int i = 0, c;
	s[0] = getchar();
	if (s[0] >= '0' && s[0] <= '9') {
		return -1;
	}
	if (s[0] != '"') {
		i = 1;
		while ((c = getchar()) != ' ' && c != '\t' && c != '\n') {
			if (c >= '0' && c <= '9') {
				s[i++] = '\0';
				return -1;
			}
			s[i++] = c;
		}
		ungetc(c, stdin);
	} else {
		while ((c = getchar()) != '"') {
			if (c >= '0' && c <= '9') {
				s[i++] = '\0';
				return -1;
			}
			s[i++] = c;
		}
	}
	s[i++] = '\0';
	return i;
}

/**
 * Reads spaces. Returns 0 if it has reached the end of line, 1 otherwise.
*/
int read_spaces() {
	int c;
	while ((c = getchar()) == ' ' || c == '\t');
	if (c == '\n') {
		return 0;
	}
	ungetc(c, stdin);
	return 1;
}

/**
 * Checks if a given char is upper cased.
*/
int isupper_char(char c) {
	if (c < 'A' || c > 'Z')
		return FALSE;
	return TRUE;
}

/**
 * Checks if a given char is a digit.
*/
int is_digit(char c) {
	return c >= '0' && c <= '9' ? TRUE : FALSE;
}

/**
 * Duplicates a string allocating 
 * new memory for the duplicate.
*/
char* duplicate_string(const char* str) {
    if (str == NULL) return NULL;

    size_t len = strlen(str);
    char* new_str = (char*)safe_malloc(len + 1);

    strcpy(new_str, str);

    return new_str;
}


/**
 * A safe version of malloc that stops the program if no memory
 * is available.
 */
void *safe_malloc(unsigned size) {
	void *ptr = malloc(size);
	if(!ptr) {
		printf("No memory.");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

/**
 * Frees the parks list of the system struct, 
 * freeing the park name
 * and both park entries and exits of each park node.
*/
void free_parks(list_t* parks) {
    node_t *current = parks->head, *temp;
    while (current != NULL) {
        park_t* park = (park_t*)current->val;
        delete_list(park->park_entries);
        delete_list(park->park_exits);

		node_t* next = park->park_vehicles->head;
    	while(next != NULL) {
			node_t* aux = next;
			next = aux->next;
			
			free(aux);
		}
    	free(park->park_vehicles);
		free(park->park_name);

        temp = current->next;

        free(current); 
        current = temp;
    }
    free(parks);
}

/**
 * 
*/
void free_hashtable(hash_table* hashtable) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        node_h* current = hashtable->table[i];
        while (current != NULL) {
            node_h* temp = current;
			vehicle_t* vhc = (vehicle_t*)temp->vehicle;
            current = current->next;
			free(vhc);
            free(temp);
        }
    }
    free(hashtable);
}


/**
 * Frees the all the memory of the program,
 * all allocated memory that has not been freed yet
 * is freed here, including the auxiliar buffer.
*/
void free_mem(system_t* sys, char* buffer) {
    free_parks(sys->parks);
	delete_list(sys->srtd_parks);
	free_hashtable(sys->vhc_ht);
	free(buffer);
    free(sys);
}