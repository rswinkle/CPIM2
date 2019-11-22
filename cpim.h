#ifndef CPIM_H
#define CPIM_H

#include "cvector.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#include <sqlite3.h>


#define MAX_STR_LENGTH 30

#define GET_CONTACT(VEC, I) ((contact*)&(VEC)->a[(I)*(VEC)->elem_size])
#define GET_ATTRIBUTE(VEC, I) ((attribute*)&(VEC)->a[(I)*(VEC)->elem_size])

enum {
	NONE,
	LINE,
	BLOCK
};


typedef int64_t i64;

typedef struct attribute
{
	char* name;
	char* value;
} attribute;

typedef struct contact
{
	i64 id;
	char* first;
	char* middle;
	char* last;
	char* phone;
	cvector_void attribs;
} contact;


extern int saved;

/* puts adds a newline ... change all to fputs for consistency? */
void print_menu();

void free_attribute(void* tmp);
void free_contact(void* tmp);


// combine with open/close in init_db/shutdown_db
void create_table(sqlite3* db);
void prepare_stmts(sqlite3* db);
void finalize_stmts();



void add_contact(sqlite3* db);

void print_contact(contact* c);

void display_contacts();

int find_contacts(sqlite3* db, cvector_i* results, int print);
void remove_contact(sqlite3* db);
void edit_contact(int id, int print_first);
void edit_contacts(sqlite3* db);

#endif
