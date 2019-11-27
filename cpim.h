#ifndef CPIM_H
#define CPIM_H

#define CVEC_ONLY_INT
#include "cvector.h"

#include <stdlib.h>
#include <ctype.h>


#define MAX_STR_LENGTH 30


typedef struct attribute
{
	char* name;
	char* value;
} attribute;

CVEC_NEW_DECLS2(attribute)

typedef struct contact
{
	char* first;
	char* last;
	char* phone;
	cvector_attribute attribs;
} contact;


#define RESIZE(a) ((a+1)*2)

CVEC_NEW_DECLS2(contact)





extern int saved;

/* puts adds a newline ... change all to fputs for consistency? */
void print_menu();

void free_attribute(void* tmp);
void free_contact(void* tmp);





void add_contact(cvector_contact* contacts);
void save_contacts(cvector_contact* contacts);
void load_contacts(cvector_contact* contacts);


void print_contact(contact* c);

void display_contacts(cvector_contact* contacts);


int compare_first(const void* contact1, const void* contact2);
int compare_last(const void* contact1, const void* contact2);
int compare_contact(const void* contact1, const void* contact2);

void sort_contacts(cvector_contact* contacts);
void find_contacts(cvector_contact* contacts, cvector_i* results_out, int print_results);
void remove_contact(cvector_contact* contacts);
void edit_contact(contact* c, int print_first);
void edit_contacts(cvector_contact* contacts);

#endif
