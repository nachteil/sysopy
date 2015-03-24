#include <stdlib.h>
#include <stdio.h>
#include "list_funs.h"
#include <string.h>

#define NULL ((void *)0)
/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

list_t * create_list() {
    list_t * result = (list_t *) malloc(sizeof(list_t));
    verify_not_null(result);
    result -> head = NULL;
    return result;
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void delete_node(list_node_t *node)
{
    if(node == NULL)
    {
        printf("Passed NULL to remove_node(). Returning now.");
        return;
    }


    free((void *) node -> birth_date);
    free((void *) node -> address);

    printf("Dependecies freed\n");

    free(node);

    return;
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void delete_list(list_t * list)
{
    if(list == NULL)
    {
        return;
    }

    list_node_t * node = list -> head;

    while(node != NULL)
    {
        list_node_t * next = node -> next;
        delete_node(node);
        node = next;
    }

    return;
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void add_elem(list_t * list, list_node_t * node_to_add)
{
    if(list == NULL)
    {
        printf("Cannot add element: 'list' is NULL.\n");
        return;
    }

    if(node_to_add == NULL)
    {
        printf("Cannot add element: 'node_to_add' is null\n");
        return;
    }

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
        printf("Found node to remove\n");
        if(iteration_node -> prev == NULL)
        {
            list -> head = iteration_node -> next;
            if(list -> head != NULL)
            {
                list -> head -> prev = NULL;
            }
        } else {
            iteration_node -> prev -> next = iteration_node -> next;
            if(iteration_node -> next != NULL)
            {
                iteration_node -> next -> prev = iteration_node -> prev;
            }
        }
        printf("Pointers ready\n");
        delete_node(iteration_node);
    }

    return;
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
    printf("\t| \t%s %s st., %s, %s\n", node -> address -> building_num, node -> address -> street, node -> address -> city, node -> address -> zip_code);
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

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void list_concat(list_t *first, list_t * second)
{
    if(first == NULL || second == NULL)
    {
        printf("Cannot perform concatenation of NULL lists.\n");
        return;
    }

    if(first -> head == NULL)
    {
        first -> head = second -> head;
        return;
    }

    if(second -> head == NULL)
    {
        second -> head = first -> head;
        return;
    }

    list_node_t * iteration_node = first -> head;
    while(iteration_node -> next != NULL)
    {
        iteration_node = iteration_node -> next;
    }
    iteration_node -> next = second -> head;
    second -> head -> prev = iteration_node;

    second -> head = first -> head;

    return;
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

list_t * get_equals(list_node_t * node, list_t * list)
{
    list_t * equals = create_list();
    list_node_t * iteration_node = list -> head;

    while(iteration_node != NULL)
    {
        if(strcmp(iteration_node -> name, node -> name) == 0 && strcmp(iteration_node -> surname, node -> surname) == 0)
        {
            add_elem(equals, iteration_node);
        }
        iteration_node = iteration_node -> next;
    }

    return equals;
}
/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

list_t * get_greater(list_node_t * node, list_t * list)
{
    list_t *greater = create_list();
    list_node_t * iteration_node = list -> head;

    while(iteration_node != NULL)
    {
        char * i_name = iteration_node -> name;
        char * i_surn = iteration_node -> surname;
        char * name = node -> name;
        char * surn = node -> surname;

        if(strcmp(i_surn, surn) > 0 || (strcmp(i_surn, surn) == 0 && strcmp(i_name, name) > 0))
        {
            add_elem(greater, iteration_node);
        }
        iteration_node = iteration_node -> next;
    }

    return greater;
}
/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

list_t * get_lesser(list_node_t * node, list_t * list)
{
    list_t *lesser = create_list();
    list_node_t * iteration_node = list -> head;

    while(iteration_node != NULL)
    {
        char * i_name = iteration_node -> name;
        char * i_surn = iteration_node -> surname;
        char * name = node -> name;
        char * surn = node -> surname;

        if(strcmp(i_surn, surn) < 0 || (strcmp(i_surn, surn) == 0 && strcmp(i_name, name) < 0))
        {
            add_elem(lesser, iteration_node);
        }
        iteration_node = iteration_node -> next;
    }

    return lesser;
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

// this is inner ("private") procedure, thus no null-check
void perform_quick_list_sort(list_t * list)
{

    // one-element list
    if(list -> head == NULL || list -> head -> next == NULL)
    {
        return;
    }

    list_node_t * pivot = list -> head;

    list_t * lesser  = create_list();
    list_t * equal   = create_list();
    list_t * greater = create_list();

    list_t * result = create_list();

    list_node_t * iteration_node = list -> head;

    while(iteration_node != NULL)
    {
        char * i_name = iteration_node -> name;
        char * i_surn = iteration_node -> surname;
        char * name = pivot -> name;
        char * surn = pivot -> surname;

        list_node_t * next = iteration_node -> next;

//        printf("Comparing %s and %s: %s\n", i_surn, surn, strcmp(i_surn, surn) == 0 ? "equal" : (strcmp(i_surn, surn) > 0 ? "first greater" : "fisrt lesser") );

        if(strcmp(i_name, name) == 0 && strcmp(i_surn, surn) == 0)
        {
            add_elem(equal, iteration_node);
        }

        if(strcmp(i_surn, surn) < 0 || (strcmp(i_surn, surn) == 0 && strcmp(i_name, name) < 0))
        {
            add_elem(lesser, iteration_node);
        }

        if(strcmp(i_surn, surn) > 0 || (strcmp(i_surn, surn) == 0 && strcmp(i_name, name) > 0))
        {
            add_elem(greater, iteration_node);
        }

        iteration_node = next;
    }

//    printf("\nEQUALS:\n");
//    print_list(equal);
//    printf("\nGREATER:\n");
//    print_list(greater);
//    printf("\nLESSER:\n");
//    print_list(lesser);

    perform_quick_list_sort(lesser);
    perform_quick_list_sort(equal);
    perform_quick_list_sort(greater);

    if(lesser -> head != NULL)
    {
        list_concat(result, lesser);
    }

    if(equal -> head != NULL)
    {
        list_concat(result, equal);
    }

    if(greater -> head != NULL)
    {
        list_concat(result, greater);
    }

    list -> head = result -> head;
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void list_sort(list_t * list)
{
    if(list == NULL || list -> head == NULL)
    {
        printf("Cannot sort NULL or empty list.\n");
        return;
    }

    list_node_t * iteration_node = list -> head;
    while(iteration_node != NULL)
    {
        if(iteration_node -> name == NULL || iteration_node -> surname == NULL)
        {
            printf("Sort operation impossible - there are keys (name and surname) missing in some elements.\n");
        }
        iteration_node = iteration_node -> next;
    }

    perform_quick_list_sort(list);
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */


list_node_t *find_elem_by_full_name(char * name, char * surname)
{
    if(name == NULL || surname == NULL || ! strcmp(name, "") || ! strcmp(surname(surname, "")))
    {
        printf("Cannot perform find on NULL or empty name\n");
        return;
    }

    
}
