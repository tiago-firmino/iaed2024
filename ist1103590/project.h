/**
 * @file project.h
 * 
 * @author Tiago Firmino - ist1103590
 * 
 * Header file for the program constant values,
 * defines and structs.
 * 
*/

#ifndef PROJECT_H
#define PROJECT_H

/* constant values */

#define TRUE 1
#define FALSE 0
#define INVALID -1
#define MAX_LINE_SIZE BUFSIZ

#define V_LICENSE_PLT_LENGTH 9

#define MAX_P 20
#define MAX_CMD_LENGTH 65536
#define MINS_IN_YEAR 525600
#define MINS_IN_DAY 1440

/* command constant values */

#define QUIT_COMMAND 'q'
#define PARK_COMMAND 'p'
#define ENTRY_COMMAND 'e'
#define EXIT_COMMAND 's'
#define VEHICLE_COMMAND 'v'
#define FACT_COMMAND 'f'
#define REMOVE_COMMAND 'r'
#define PAID_COMAMND 'u'

/* struct calls to use in other structs */

typedef struct entry_t entry_t;

typedef struct vehicle_t vehicle_t;

/* timestamps and tariffs */

typedef struct {
	int y, d, mth, h, min;
} timestamp_t;

typedef struct {
	float first_hour_price;
	float hour_price;
	float max_daily_price;
} tariff_t;

/* linked list */

typedef struct node {
	struct node* next;
	struct node* prev;
	void* val;
} node_t;

typedef struct list {
	node_t* head;
	node_t* tail;
} list_t;

/* hashtable */

#define HASH_TABLE_SIZE 293

typedef struct node_h {
    vehicle_t* vehicle;
    struct node_h* next;
} node_h;

typedef struct hash_table {
    node_h* table[HASH_TABLE_SIZE];
} hash_table;


/* vehicles, entries, exits */

#define PARK_DOESNT_EXIST "%s: no such parking.\n"
#define PARK_CAPACITY_EXCEEDED "%s: parking is full.\n"
#define VEHICLE_INVALID_LICENSE "%s: invalid licence plate.\n"
#define VEHICLE_INVALID_ENTRY "%s: invalid vehicle entry.\n"
#define VEHICLE_INVALID_EXIT "%s: invalid vehicle exit.\n"
#define VEHICLE_NO_REGISTRY "%s: no entries found in any parking.\n"
#define INVALID_DATE "invalid date.\n"

struct vehicle_t {
	char license_plate[V_LICENSE_PLT_LENGTH];
	timestamp_t last_entry;
	entry_t* current_entry;
};

struct entry_t {
	char *park_name;
	vehicle_t* vehicle;
	timestamp_t entry_date_time;
};

typedef struct {
	char *park_name;
	vehicle_t* vehicle;
	timestamp_t exit_date_time;
	float paid_value;
} exit_t;

/* car parks */

#define PARK_DUPLICATE "%s: parking already exists.\n"
#define PARK_INVALID_NAME "invalid parking name.\n"
#define PARK_CAPACITY_INVALID "%d: invalid capacity.\n"
#define PARK_INVALID_TARIFARY "invalid cost.\n"
#define PARK_MAX_EXCEEDED "too many parks.\n"

typedef struct {
	char *park_name;
	int park_capacity;
	int num_vehicles;
	tariff_t park_tariff;
	list_t *park_entries;
	list_t *park_exits;
	list_t *park_vehicles;
} park_t;

/* system */

typedef struct {
	list_t *parks;
	list_t *srtd_parks;
	int num_parks;
	hash_table* vhc_ht;
	timestamp_t date_registry;
} system_t;

#endif
