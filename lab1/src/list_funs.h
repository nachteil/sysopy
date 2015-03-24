
#ifndef _SYSOPY_LISTS_H_
#define _SYSOPY_LISTS_H_

#define NO_APPARTAMENT_ADDRESS -1
#define EXIT_ERROR_CODE 1
// struct forward declarations

#include "list_structs.h"
#include "utils.h"

// function declarations

list_t * create_list();
void delete_list(list_t *);

void add_elem(list_t *, list_node_t *);
void remove_elem(list_t *, list_node_t *);

list_node_t *find_elem_by_full_name(const char *const, const char *const);

void sort_list(list_t *);

void delete_node(list_node_t *);

void print_list(list_t *);
list_node_t * create_node();

void list_sort(list_t *);

#endif //_SYSOPY_LISTS_H_
