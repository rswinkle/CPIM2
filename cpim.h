#ifndef CPIM_H
#define CPIM_H

#include "clist.h"

#include <stdlib.h>
#include <ctype.h>


typedef struct attribute
{
	char* name;
	char* value;
	list_head list;
} attribute;

typedef struct contact
{
	char* first;
	char* last;
	char* phone;
	list_head list;
	list_head attribs;
} contact;


extern int saved;

/* puts adds a newline ... change all to fputs for consistency? */
void print_menu();

void free_attribute(void* tmp);
void free_contact(void* tmp);


void free_contact_list(list_head* contacts);
void free_attrib_list(list_head* attribs);



void add_contact(list_head* contacts);
void save_contacts(list_head* contacts);
void load_contacts(list_head* contacts);


void print_contact(contact* c);

void display_contacts(list_head* contacts);


int compare_first(list_head* contact1, list_head* contact2);
int compare_last(list_head* contact1, list_head* contact2);
int compare_contact(list_head* contact1, list_head* contact2);

void sort_contacts(list_head* contacts);
void find_contacts(list_head* contacts, list_head* results_out, int* size, int print_results);
void remove_contact(list_head* contacts);
void edit_contact(contact* c, int print_first);
void edit_contacts(list_head* contacts);

#endif
