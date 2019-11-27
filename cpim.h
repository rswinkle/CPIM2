#ifndef CPIM_H
#define CPIM_H

#define CVEC_ONLY_INT
#define CVEC_ONLY_VOID
#include "cvector.h"

#include <stdlib.h>
#include <ctype.h>

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
	cvector_void attribs;
} contact;


extern int saved;

/* puts adds a newline ... change all to fputs for consistency? */
void print_menu();

void free_attribute(void* tmp);
void free_contact(void* tmp);





void add_contact(cvector_void* contacts);
void save_contacts(cvector_void* contacts);
void load_contacts(cvector_void* contacts);


void print_contact(contact* c);

void display_contacts(cvector_void* contacts);


int compare_first(const void* contact1, const void* contact2);
int compare_last(const void* contact1, const void* contact2);
int compare_contact(const void* contact1, const void* contact2);

void sort_contacts(cvector_void* contacts);
void find_contacts(cvector_void* contacts, cvector_i* results_out, int print_results);
void remove_contact(cvector_void* contacts);
void edit_contact(contact* c, int print_first);
void edit_contacts(cvector_void* contacts);

#endif
