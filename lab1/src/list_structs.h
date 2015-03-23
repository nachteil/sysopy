//
// Created by yorg on 23.03.15.
//

#ifndef _SYSOPY_LIST_STRUCTS_H_
#define _SYSOPY_LIST_STRUCTS_H_

// date definition

typedef struct date {
    const unsigned char month;
    const unsigned char day;
    const unsigned short year;
} date_t;

// address definition

typedef struct address {
    const char * street;
    const char * flat_num;
    const char * building_num;
    const char * zip_code;
    const char * city;
} address_t;

// list_node definition

typedef struct list_node_t {
    const char * name;
    const char * surname;
    const date_t * birth_date;
    const char * email_address;
    const char * phone_number;
    const address_t * address;

    struct list_node_t * next;
    struct list_node_t * prev;

} list_node_t;

// list type definition

typedef struct list {
    list_node_t * head;
} list_t;

#endif //_SYSOPY_LIST_STRUCTS_H_
