/**
 * @file structures.c
 * 
 * @author Tiago Firmino - ist1103590
 * 
 * File containing functions used to manage
 * the data structures used in the program.
 * 
*/

#include "project.h"
#include "prototypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Linked list */

/**
 * Create a new empty double linked list.
 * Returns the newly created list.
*/
list_t* init_list() {
    list_t* new_list = (list_t*)safe_malloc(sizeof(list_t));
    new_list->head = NULL;
    new_list->tail = NULL;
    return new_list;
}


/**
 * Inserts a given (already allocated) value into the 
 * given double linked list, as the last element.
 */
void insert_list(list_t* list, void* elem) {
    node_t* node = (node_t*)safe_malloc(sizeof(node_t));

    node->val = elem;

    node->next = NULL;
    node->prev = list->tail;

    if(list->head == NULL) {
        list->head = node;
    } else {
        list->tail->next = node;
    }
    list->tail = node;
}


/**
 * Inserts a given (already allocated) value into the 
 * given double linked list, in a position sorted
 * based on comparison criteria.
 */
void sorted_insert_list(list_t* list, void* elem, char type) {
    node_t* new_node = (node_t*)safe_malloc(sizeof(node_t));
    new_node->val = elem;
    new_node->next = NULL;
    new_node->prev = NULL;

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        return;
    }

    node_t* current = list->head;
    while (current != NULL) {
        // Use the compare_elements function to determine ordering
        int compare_value = compare_elements(elem, current->val, type);
        if (compare_value < 0) {
            insert_before_node(new_node, current, list);
            return;
        }
        current = current->next;
    }

    // If we reach here, insert at the end of the list
    insert_at_tail(new_node, list);
}


/**
 * Compares two list elements based on their type.
 * Returns INVALID (-1) if elem1 should come before elem2,
 * TRUE (1) if elem1 should come after elem2,
 * or FALSE (0) if they are equal.
 */
int compare_elements(void* elem1, void* elem2, char type) {
    switch (type) {
        case PARK_COMMAND: {
            park_t* park1 = (park_t*)elem1;
            park_t* park2 = (park_t*)elem2;
            return strcmp(park1->park_name, park2->park_name);
        }
        case ENTRY_COMMAND: {
            entry_t* entry1 = (entry_t*)elem1;
            entry_t* entry2 = (entry_t*)elem2;
            return compare_date_time(entry1->entry_date_time,
             entry2->entry_date_time);
        }
        case EXIT_COMMAND: {
            exit_t* exit1 = (exit_t*)elem1;
            exit_t* exit2 = (exit_t*)elem2;
            return compare_date_time(exit1->exit_date_time,
             exit2->exit_date_time);
        }
        default:
            return FALSE;
    }
}

/**
 * Inserts a new node before the current one. 
 */
void insert_before_node(node_t* new_node,
     node_t* current, list_t* list) {
    new_node->next = current;
    new_node->prev = current->prev;
    if (current->prev != NULL) {
        current->prev->next = new_node;
    } else {
        list->head = new_node;
    }
    current->prev = new_node;
}

/**
 * Inserts a new node at the end of the list.
 */
void insert_at_tail(node_t* new_node, list_t* list) {
    list->tail->next = new_node;
    new_node->prev = list->tail;
    list->tail = new_node;
}


/**
 * Removes all items from the list, frees the list nodes
 * and values.
 */
void delete_list(list_t* list) {
    node_t* next = list->head;

    while(next != NULL) {
        node_t* aux = next;
        next = aux->next;
        
        free(aux->val);
        free(aux);
    }

    free(list);
}

/**
 * Deletes a single node from the list that has the given value,
 * freeing the node itself.
 */
void delete_node(list_t* list, void* val) {
    node_t* curr = list->head;
    while(curr != NULL && curr->val != val) {
        curr = curr->next;
    }

    if(curr != NULL) {
        if(curr->prev != NULL) {
            curr->prev->next = curr->next;
        } else {
            list->head = curr->next;
        }
        if(curr->next != NULL) {
            curr->next->prev = curr->prev;
        } else {
            list->tail = curr->prev;
        }
        free(curr);
    }
}

/* Hash table */

/**
 * Calculates the hash value for a given vehicle license plate.
 * Sums the ascii values of the license plate using that sum
 * to ensure the hash value fits within the bounds of the
 * hashtable array.
*/
int hash(char* plate) {
    int sum = 0;
    int len = strlen(plate);
    for (int i = 0; i < len; i++) {
        sum += plate[i];
    }
    return sum % HASH_TABLE_SIZE;
}

/**
 * Initializes a new hash table for storing vehicles
 * and sets all pointers in the hash table's array to NULL.
*/
hash_table* init_ht() {
    hash_table* hashtable = (hash_table*)safe_malloc(sizeof(hash_table));
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        hashtable->table[i] = NULL;
    }
    return hashtable;
}

/**
 * Inserts a vehicle into the hash table.
 * Calculates the hash value for the vehicle's license plate
 * to determine the appropriate slot in the hash table.
*/
void insert_ht(hash_table* hashtable, vehicle_t* vhc) {
    int index = hash(vhc->license_plate);
    node_h* newNode = (node_h*)safe_malloc(sizeof(node_h));
    newNode->vehicle = vhc;
    newNode->next = hashtable->table[index];
    hashtable->table[index] = newNode;
}

/**
 * Searches for a vehicle in the hash table by its license plate.
 * Calculates the hash value for the given license plate to locate
 * the correct slot in the hash table and traverses the linked list
 * at that slot to find a vehicle witha  matching license plate.
*/
vehicle_t* search_ht(hash_table* hashtable, char* plate) {
    int index = hash(plate);
    node_h* current = hashtable->table[index];
    while (current != NULL) {
        if (!strcmp(current->vehicle->license_plate, plate)) {
            return current->vehicle;
        }
        current = current->next;
    }
    return NULL;
}
