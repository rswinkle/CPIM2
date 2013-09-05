#ifndef CPIM_H
#define CPIM_H

#include "cvector.h"

#include <stdlib.h>
#include <ctype.h>


#define MAX_STR_LENGTH 30

#define GET_CONTACT(VEC, I) ((contact*)&(VEC)->a[(I)*(VEC)->elem_size])
#define GET_ATTRIBUTE(VEC, I) ((attribute*)&(VEC)->a[(I)*(VEC)->elem_size])


typedef struct attribute
{
	char* name;
	char* value;
} attribute;

typedef struct contact
{
	char* first;
	char* last;
	char* phone;
	vector_void attribs;
} contact;


extern int saved;

/* puts adds a newline ... change all to fputs for consistency? */
void print_menu();

void free_attribute(void* tmp);
void free_contact(void* tmp);





void add_contact(vector_void* contacts);
void save_contacts(vector_void* contacts);
void load_contacts(vector_void* contacts);


void print_contact(contact* c);

void display_contacts(vector_void* contacts);


int compare_first(const void* contact1, const void* contact2);
int compare_last(const void* contact1, const void* contact2);
int compare_contact(const void* contact1, const void* contact2);

void sort_contacts(vector_void* contacts);
void find_contacts(vector_void* contacts, vector_i* results_out, int print_results);
void remove_contact(vector_void* contacts);
void edit_contact(contact* c, int print_first);
void edit_contacts(vector_void* contacts);

#endif
