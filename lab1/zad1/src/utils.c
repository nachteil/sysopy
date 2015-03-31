#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#define NULL ((void *)0)


void verify_not_null(void * v)
{
    if(v == NULL)
    {
        printf("Memory allocation error. Exiting now.\n");
        exit(1);
    }
}


list_t *create_sample_list()
{
    list_node_t * node_1 = create_node();
    list_node_t * node_2 = create_node();
    list_node_t * node_3 = create_node();
    list_node_t * node_4 = create_node();
    list_node_t * node_5 = create_node();
    list_node_t * node_6 = create_node();
    list_node_t * node_7 = create_node();

    //

    node_1 -> address -> building_num = "12";
    node_1 -> address -> city = "Los Angeles";
    node_1 -> address -> flat_num = "123";
    node_1 -> address -> street = "Groove";
    node_1 -> address -> zip_code = "90-210";

    node_1 -> birth_date -> day = 2;
    node_1 -> birth_date -> month = 11;
    node_1 -> birth_date -> year = 1921;

    node_1 -> email_address = "max.payne@gmail.com";
    node_1 -> phone_number = "23423423";
    node_1 -> name = "Max";
    node_1 -> surname = "Payne";

    //

    node_2 -> address -> building_num = "3423";
    node_2 -> address -> city = "Warsaw";
    node_2 -> address -> flat_num = "7";
    node_2 -> address -> street = "Wspolna";
    node_2 -> address -> zip_code = "32-300";

    node_2 -> birth_date -> day = 14;
    node_2 -> birth_date -> month = 2;
    node_2 -> birth_date -> year = 1966;

    node_2 -> email_address = "buizaczek@motylek.pl";
    node_2 -> phone_number = "52345276";
    node_2 -> name = "Alfonso";
    node_2 -> surname = "Pl";

    //

    node_3 -> address -> building_num = "4A";
    node_3 -> address -> city = "Krakau";
    node_3 -> address -> flat_num = "41";
    node_3 -> address -> street = "Sliczna";
    node_3 -> address -> zip_code = "31-412";

    node_3 -> birth_date -> day = 24;
    node_3 -> birth_date -> month = 7;
    node_3 -> birth_date -> year = 1987;

    node_3 -> email_address = "fire1991@yahoo.com";
    node_3 -> phone_number = "99342923";
    node_3 -> name = "Kuba";
    node_3 -> surname = "Kop";

    //

    node_4 -> address -> building_num = "65";
    node_4 -> address -> city = "New York";
    node_4 -> address -> flat_num = "99";
    node_4 -> address -> street = "Main";
    node_4 -> address -> zip_code = "54-423";

    node_4 -> birth_date -> day = 30;
    node_4 -> birth_date -> month = 1;
    node_4 -> birth_date -> year = 1999;

    node_4 -> email_address = "elo@no.us";
    node_4 -> phone_number = "6-5465-465463";
    node_4 -> name = "Nick";
    node_4 -> surname = "Rock";

    //

    node_5 -> address -> building_num = "22A/5";
    node_5 -> address -> city = "Lodz";
    node_5 -> address -> flat_num = "73";
    node_5 -> address -> street = "Brzydka";
    node_5 -> address -> zip_code = "00-010";

    node_5 -> birth_date -> day = 27;
    node_5 -> birth_date -> month = 10;
    node_5 -> birth_date -> year = 2001;

    node_5 -> email_address = "ll@dd.pl";
    node_5 -> phone_number = "36346376";
    node_5 -> name = "Kora";
    node_5 -> surname = "Rako";

    //

    node_6 -> address -> building_num = "22A/5";
    node_6 -> address -> city = "Lodz";
    node_6 -> address -> flat_num = "73";
    node_6 -> address -> street = "Brzydka";
    node_6 -> address -> zip_code = "00-010";

    node_6 -> birth_date -> day = 27;
    node_6 -> birth_date -> month = 10;
    node_6 -> birth_date -> year = 2001;

    node_6 -> email_address = "ll@dd.pl";
    node_6 -> phone_number = "36346376";
    node_6 -> name = "Jakub";
    node_6 -> surname = "Kmiec";

    //

    node_7 -> address -> building_num = "22A/5";
    node_7 -> address -> city = "Lodz";
    node_7 -> address -> flat_num = "73";
    node_7 -> address -> street = "Brzydka";
    node_7 -> address -> zip_code = "00-010";

    node_7 -> birth_date -> day = 27;
    node_7 -> birth_date -> month = 10;
    node_7 -> birth_date -> year = 2001;

    node_7 -> email_address = "ll@dd.pl";
    node_7 -> phone_number = "36346376";
    node_7 -> name = "Amadeusz";
    node_7 -> surname = "Rako";

    list_t * list = create_list();

    add_elem(list, node_1);
    add_elem(list, node_2);
    add_elem(list, node_3);
    add_elem(list, node_4);
    add_elem(list, node_5);
    add_elem(list, node_6);
    add_elem(list, node_7);

    return list;
}