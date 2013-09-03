
#include "c_utils.h"

#define CVECTOR_IMPLEMENTATION
#include "cvector.h"


#include <stdio.h>
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

/* puts adds a newline ... change all to fputs for consistency? */
void print_menu()
{
	puts("Actions\n-------");
	puts("Add Contact (A)");
	puts("Display Contacts (D)");
	puts("Save Contacts (V)");
	puts("Load Contacts (L)");
	puts("Remove Contacts (R)");
	puts("Search (S)");
	puts("Quit (Q)");
	puts("Help (?)");

}


void free_attribute(void* tmp)
{
	attribute* a = (attribute*)tmp;
	free(a->name);
	free(a->value);
}

void free_contact(void* tmp)
{
	contact* c = (contact*)tmp;
	free(c->first);
	free(c->last);
	free(c->phone);
	free_vec_void(&c->attribs);
}

char read_char(FILE* input)
{
	char c, ret;
	do { ret = getc(input); } while (isspace(ret));
	do { c = getc(input); } while (c != '\n');
	return ret;
}

/* define GET_STRING macro */
#define READ_STRING(STR, CHAR) \
do { \
	do { CHAR = getchar(); } while (isspace(CHAR)); \
	ungetc(CHAR, stdin); \
	do { \
		STR = freadline(stdin); \
	} while (!STR); \
} while(0)





void add_contact(vector_void* contacts)
{
	contact tmp_c;
	attribute tmp_attrib;
	vector_void* attribs;
	char* tmp_str = NULL;
	char choice, clean;

	puts("Enter first name:");
	READ_STRING(tmp_str, clean);
	/*
	do { clean = getchar(); } while (isspace(clean));
	ungetc(clean, stdin);
	do {
		tmp_str = freadline(stdin);
	} while (!tmp_str);
	*/
	tmp_c.first = tmp_str;

	puts("Enter last name:");
	do {
		tmp_str = freadline(stdin);
	} while (!tmp_str);
	tmp_c.last = tmp_str;

	puts("Enter phone number:");
	do {
		tmp_str = freadline(stdin);
	} while (!tmp_str);
	tmp_c.phone = tmp_str;

	vec_void(&tmp_c.attribs, 0, 10, sizeof(attribute), free_attribute, NULL);
	push_void(contacts, &tmp_c);
	attribs = &((contact*)back_void(contacts))->attribs;

	puts("Do you want to add any attributes? (Y/N)");
	choice = read_char(stdin);

	while (choice == 'Y' || choice == 'y') {
		puts("Enter attribute name:");
		do {
			tmp_str = freadline(stdin);
		} while (!tmp_str);
		tmp_attrib.name = tmp_str;

		puts("Enter attribute value:");
		do {
			tmp_str = freadline(stdin);
		} while (!tmp_str);
		tmp_attrib.value = tmp_str;

		push_void(attribs, &tmp_attrib);

		puts("Do you want to add another attribute? (Y/N)\n");
		choice = read_char(stdin);
	}
}

void save_contacts(vector_void* contacts)
{
	char* tmp_str = NULL;
	char choice, clean;
	c_array out;
	SET_C_ARRAY(out, contacts->a, contacts->elem_size, contacts->size); 	
	
	puts("Enter the name of a file to save to:");
	READ_STRING(tmp_str, clean);

	printf("%d, %d\n", out.elem_size, out.len);
	if (!file_open_write(tmp_str, "wb", &out)) {
		perror("Error saving to file");
		return;
	}
	puts("Contacts saved successfully.");
}

void load_contacts(vector_void* contacts)
{
	char* tmp_str = NULL;
	char choice, clean;
	c_array in;

	puts("Enter the name of a file to load:");
	READ_STRING(tmp_str, clean);

	if (!file_open_read(tmp_str, "rb", &in)) {
		perror("Error reading file");
		return;
	}

	int sz = in.len/sizeof(contact);
	reserve_void(contacts, sz);
	contacts->size = sz;
	memcpy(contacts->a, in.data, in.len);

	puts("Contacts loaded successfully.");
}


void print_contact(contact* c)
{
	attribute* a;
	printf("Last Name: %s\n", c->last);
	printf("First Name: %s\n", c->first);
	printf("Phone Number: %s\n", c->phone);

	for (int j=0; j<c->attribs.size; ++j) {
		a = GET_ATTRIBUTE(&c->attribs, j);
		printf("%s: %s\n", a->name, a->value);
	}
}

void display_contacts(vector_void* contacts)
{
	contact* c;
	for (int i=0; i<contacts->size; ++i) {
		c = GET_CONTACT(contacts, i);
		print_contact(c);
		putchar('\n');
	}
}

void remove_contact(vector_void* contacts)
{
	contact *tmp_c;
	char* tmp_str = NULL;
	char choice, clean;

	puts("Enter the last name of the contact you wish to remove:");
	READ_STRING(tmp_str, clean);

	for (int i=0; i<contacts->size; ++i) {
		tmp_c = GET_CONTACT(contacts, i);
		if (!strcmp(tmp_str, tmp_c->last)) {
			erase_void(contacts, i, i);
			break;
		}
	}
}

void search_contacts(vector_void* contacts)
{
	contact *tmp_c;
	char* tmp_str = NULL;
	char choice, clean;

	puts("Enter the last name of the contact you wish to find:");
	READ_STRING(tmp_str, clean);
	
	int i;
	for (i=0; i<contacts->size; ++i) {
		tmp_c = GET_CONTACT(contacts, i);
		if (!strcmp(tmp_str, tmp_c->last)) {
			print_contact(tmp_c);
			break;
		}
	}
	if (i == contacts->size)
		puts("No results found");
}




int main(int argc, char** argv)
{
	vector_void contacts;
	char choice;
	int quit = 0;	
	vec_void(&contacts, 0, 10, sizeof(contact), free_contact, NULL);

	print_menu();
	while (!quit) {
		
		puts("What action would you like to perform?");
		choice = read_char(stdin);

		switch (choice) {
		case 'A':
		case 'a':
			add_contact(&contacts);
			break;
		case 'D':
		case 'd':
			display_contacts(&contacts);
			break;

		case 'V':
		case 'v':
			save_contacts(&contacts);
			break;

		case 'L':
		case 'l':
			load_contacts(&contacts);
			break;

		case 'R':
		case 'r':
			remove_contact(&contacts);
			break;

		case 'S':
		case 's':
			search_contacts(&contacts);
			break;

		case 'Q':
		case 'q':
			//add check for unsaved changes
			quit = 1;
			break;

		case '?':
			print_menu();
			break;
		}
		putchar('\n');
	}




	return 0;
}
