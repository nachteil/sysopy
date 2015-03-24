#ifndef _SYSOPY_LIST_STRUCTS_H_
#define _SYSOPY_LIST_STRUCTS_H_

// date definition

typedef struct date {
    unsigned char month;
    unsigned char day;
    unsigned short year;
} date_t;

// address definition

typedef struct address {
    char * street;
    char * flat_num;
    char * building_num;
    char * zip_code;
    char * city;
} address_t;

// list_node definition

typedef struct list_node_t {
    char * name;
    char * surname;
    date_t * birth_date;
    char * email_address;
    char * phone_number;
    address_t * address;

    struct list_node_t * next;
    struct list_node_t * prev;

} list_node_t;

// list type definition

typedef struct list {
    list_node_t * head;
} list_t;

#endif //_SYSOPY_LIST_STRUCTS_H_
