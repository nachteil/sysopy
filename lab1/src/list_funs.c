#include <stdlib.h>
#include <stdio.h>
#include "list_funs.h"

#define NULL ((void *)0)
/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

list_t * create_list() {
    list_t * result = (list_t *) malloc(sizeof(list_t));
    verify_not_null(result);
    result -> head = NULL;
    return result;
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void remove_node(list_node_t * node)
{
    if(node == NULL)
    {
        printf("Passed NULL to remove_node(). Returning now.");
        return;
    }

    free((void *) node -> name);
    free((void *) node -> surname);
    free((void *) node -> email_address);
    free((void *) node -> phone_number);

    free((void *) node -> birth_date);

    free((void *) node -> address -> street);
    free((void *) node -> address -> flat_num);
    free((void *) node -> address -> building_num);
    free((void *) node -> address -> zip_code);
    free((void *) node -> address -> city);
    free((void *) node -> address);

    free(node);
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void delete_list(list_t * list)
{
    list_node_t * node = list -> head;

    while(node != NULL)
    {
        list_node_t * next = node -> next;
        remove_node(node);
        node = next;
    }

    return;
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void add_elem(list_t * list, list_node_t * node_to_add)
{
    list_node_t * iteration_node = list -> head;
    if(list -> head != NULL)
    {
        while(iteration_node -> next != NULL)
        {
            iteration_node = iteration_node -> next;
        }
        iteration_node -> next = node_to_add;
        node_to_add -> prev = iteration_node;
        node_to_add -> next = NULL;

    } else {
        list -> head = node_to_add;
        node_to_add -> next = NULL;
        node_to_add -> prev = NULL;
    }
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void remove_elem(list_t * list, list_node_t * node_to_remove)
{
    if(list == NULL) {
        printf("Passed null as an argument to remove_elem(), returning now");
        return;
    }

    if(node_to_remove == NULL)
    {
        printf("Passed null as an argument to remove_elem(), returning now");
        return;
    }

    list_node_t * iteration_node = list -> head;

    while(iteration_node != NULL && iteration_node != node_to_remove)
    {
        iteration_node = iteration_node -> next;
    }

    if(iteration_node == node_to_remove)
    {
        if(iteration_node -> prev == NULL)
        {
            list -> head = iteration_node -> next;
            list -> head -> prev = NULL;
        } else {
            iteration_node -> prev -> next = iteration_node -> next;
        }

        if(iteration_node -> next == NULL)
        {
            iteration_node -> prev -> next = NULL;
        } else {

        }

        remove_node(iteration_node);
    }
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

list_node_t * create_node()
{
    list_node_t * node = (list_node_t *) malloc(sizeof(list_node_t));
    verify_not_null(node);

    address_t * address = (address_t *) malloc(sizeof(address_t));
    verify_not_null(address);

    date_t * date = (date_t *) malloc(sizeof(date));
    verify_not_null(date);

    address -> building_num = NULL;
    address -> city = NULL;
    address -> flat_num = NULL;
    address -> street = NULL;
    address -> zip_code = NULL;

    date -> day = 0;
    date -> month = 0;
    date -> year = 0;

    node -> address = address;
    node -> birth_date = date;

    node -> email_address = NULL;
    node -> phone_number = NULL;
    node -> name = NULL;
    node -> surname = NULL;

    node -> next = NULL;
    node -> prev = NULL;

    return node;
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

// this function is not published via .h, so I skip null-check
void print_node(list_node_t * node)
{
    printf("\t_______________________________________________________________\n");
    printf("\t| Name:            %s %s\n", node -> name, node -> surname);
    printf("\t| E-mail address:  %s\n", node -> email_address);
    printf("\t| Phone number:    %s \n", node -> phone_number);
    printf("\t| Address: \n");
    printf("\t| \t: %s %s st., %s, %s\n", node -> address -> building_num, node -> address -> street, node -> address -> city, node -> address -> zip_code);
    printf("\t| Date of birth:   %d.%d.%d\n\n", node -> birth_date -> day, node -> birth_date -> month, node -> birth_date -> year);
    printf("\t---------------------------------------------------------------\n");
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void print_list(list_t *list)
{
    if(list != NULL)
    {
        if(list -> head == NULL)
        {
            printf("Empty list\n");
            return;
        }
    } else {
        printf("Null list\n");
        return;
    }

    list_node_t * iteration_node = list -> head;

    while(iteration_node != NULL)
    {
        print_node(iteration_node);
        iteration_node = iteration_node -> next;
    }
}