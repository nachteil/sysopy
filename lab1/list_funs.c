#include <stdlib.h>
#include <stdio.h>
#include "list_funs.h"



/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

list_t * create_list() {
    list_t * result = (list_t *) malloc(sizeof(list_t));
    if(result == NULL) {
        printf("Memort allocation error. Exiting now.\n");
        exit(EXIT_ERROR_CODE);
    }
    result -> head = NULL;
    return result;
}

/*  *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   */

void delete_list(list_t * list)
{
    list_node_t * node = list -> head;

    while(node != NULL)
    {
        free(node -> name);
        free(node -> surname);
        free(node -> email_address);
        free(node -> phone_number);

        free(node -> birth_date);

        free(node -> address -> street);
        free(node -> address -> flat_num);
        free(node -> address -> building_num);
        free(node -> address -> zip_code);
        free(node -> address -> city);
        free(node -> address);

        list_node_t * next = node -> next;
        free(node);
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
