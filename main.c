#include "cxml.h"
#include "c_utils.h"
#include "clist.h"

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

int saved;


/* puts adds a newline ... change all to fputs for consistency? */
void print_menu()
{
	puts("Actions\n-------");
	puts("Add Contact (A)");
	puts("Display Contacts (D)");
	puts("Save Contacts (V)");
	puts("Load Contacts (L)");
	puts("Remove Contacts (R)");
	puts("Sort (S)");
	puts("Find (F)");
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

char* read_string(FILE* file, int allow_empty)
{
	char tmp;
	char* str = NULL;

	do { tmp = getc(file); } while (isspace(tmp));
	ungetc(tmp, file);
	do {
		str = freadline(file);
		if (!allow_empty && str && !str[0]) {
			free(str);
			str = NULL;
		}
	} while (!str);

	return str;
}




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

		puts("Do you want to add another attribute? (Y/N)");
		choice = read_char(stdin);
	}
	saved = 0;
}

void save_contacts(vector_void* contacts)
{
	char* tmp_str = NULL;
	char clean;
	contact* c;
	attribute* a;
	
	puts("Enter the name of a file to save to:");
	READ_STRING(tmp_str, clean);

	FILE* file = fopen(tmp_str, "w");
	if (!file) {
		perror("Error opening file");
		return;
	}

	fprintf(file, "<contacts>\n");
	for (int i=0; i<contacts->size; ++i) {
		c = GET_CONTACT(contacts, i);
		fprintf(file, "\t<contact>\n");
		fprintf(file, "\t\t<lastname>%s</lastname>\n", c->last);
		fprintf(file, "\t\t<firstname>%s</firstname>\n", c->first);
		fprintf(file, "\t\t<phone>%s</phone>\n", c->phone);
		for (int j=0; j<c->attribs.size; ++j) {
			a = GET_ATTRIBUTE(&c->attribs, j);
			fprintf(file, "\t\t<%s>%s</%s>\n", a->name, a->value, a->name);
		}
		fprintf(file, "\t</contact>\n");
	}
	fprintf(file, "</contacts>\n");
	fclose(file);
	saved = 1;

	puts("Contacts saved successfully.");
}

void load_contacts(vector_void* contacts)
{
	contact tmp_c;
	contact* tmp_c_ptr;
	attribute tmp_attrib;
	vector_void* attribs;
	char* tmp_str = NULL;
	char clean, choice;

	int overwritten = 1;
	if (contacts->size) {
		overwritten = 0;
		puts("Do you want to overwrite current contacts? (Y/N)");
		choice = read_char(stdin);
		if (choice == 'y' || choice == 'Y') {
			clear_void(contacts);
			overwritten = 1;
		}
	}

	
	puts("Enter the name of a file to load:");
	READ_STRING(tmp_str, clean);

	FILE* file = fopen(tmp_str, "r");
	if (!file) {
		perror("Failed to open file");
		return;
	}
	
	xml_tree* tree = new_xml_tree();
	if (!tree || !parse_xml_file(file, tree))
		return;

	xml_tree* xml_contact, *xml_contact_members;

	list_for_each_entry(xml_contact, xml_tree, &tree->child_list, list) {
		vec_void(&tmp_c.attribs, 0, 3, sizeof(attribute), free_attribute, NULL);
		push_void(contacts, &tmp_c);
		tmp_c_ptr = (contact*)back_void(contacts);
		attribs = &tmp_c_ptr->attribs;

		list_for_each_entry(xml_contact_members, xml_tree, &xml_contact->child_list, list) {
			if (!strcmp(xml_contact_members->key, "firstname"))
				tmp_c_ptr->first = mystrdup(xml_contact_members->value);
			else if (!strcmp(xml_contact_members->key, "lastname"))
				tmp_c_ptr->last = mystrdup(xml_contact_members->value);
			else if (!strcmp(xml_contact_members->key, "phone"))
				tmp_c_ptr->phone = mystrdup(xml_contact_members->value);
			else {
				tmp_attrib.name = mystrdup(xml_contact_members->key);
				tmp_attrib.value = mystrdup(xml_contact_members->value);

				push_void(attribs, &tmp_attrib);
			}
		}
	}

	free_xml_tree(tree);

	saved = overwritten;
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


int compare_first(const void* contact1, const void* contact2)
{
	contact* c1 = (contact*)contact1;
	contact* c2 = (contact*)contact2;
	return strcmp(c1->first, c2->first);
}

int compare_last(const void* contact1, const void* contact2)
{
	contact* c1 = (contact*)contact1;
	contact* c2 = (contact*)contact2;
	return strcmp(c1->last, c2->last);
}

int compare_contact(const void* contact1, const void* contact2)
{
	contact* c1 = (contact*)contact1;
	contact* c2 = (contact*)contact2;
	int ret = strcmp(c1->last, c2->last);
	if (!ret)
		return strcmp(c1->first, c2->first);
	return ret;
}

void sort_contacts(vector_void* contacts)
{
	char choice;
	vector_i results;
	vec_i(&results, 0, contacts->size);

	puts("Do you want to sort by last name, first name or both? (L/F/B)");
	choice = read_char(stdin);
	if (choice == 'L' || choice == 'l')
		qsort(contacts->a, contacts->size, contacts->elem_size, compare_last);
	else if (choice == 'F' || choice == 'f') 
		qsort(contacts->a, contacts->size, contacts->elem_size, compare_first);
	else
		qsort(contacts->a, contacts->size, contacts->elem_size, compare_contact);

	
	puts("\nSorted successfully");
}

void find_contacts(vector_void* contacts, vector_i* results_out, int print_results)
{
	contact *tmp_c;
	char* last = NULL, *first = NULL;
	char clean, choice;
	vector_i results;
	vec_i(&results, 0, contacts->size);

	int both = 0;

	puts("Do you want to search by last name, first name, or both? (L/F/B)");
	choice = read_char(stdin);
	if (choice != 'F' && choice != 'f') {
		puts("Enter the last name:");
		READ_STRING(last, clean);
	}
	if (choice != 'L' && choice != 'l') {
		puts("Enter the first name:");
		READ_STRING(first, clean);
		if (last)
			both = 1;
	}

	int i;
	for (i=0; i<contacts->size; ++i) {
		tmp_c = GET_CONTACT(contacts, i);
		if (both && !strcmp(last, tmp_c->last) && !strcmp(first, tmp_c->first)) {
			push_i(&results, i);
		} else if (last && !strcmp(last, tmp_c->last)) {
			push_i(&results, i);
		} else if (first && !strcmp(first, tmp_c->first)) {
			push_i(&results, i);
		}
	}

	if (print_results) {
		printf("\n%d Results Found:\n=================\n", results.size);
		for (i=0; i<results.size; ++i) {
			tmp_c = GET_CONTACT(contacts, results.a[i]);
			print_contact(tmp_c);
			putchar('\n');
		}
	}

	if (results_out)
		memcpy(results_out, &results, sizeof(vector_i));
	else
		free_vec_i(&results);
}


void remove_contact(vector_void* contacts)
{
	contact *tmp_c;
	char choice;
	vector_i results;

	find_contacts(contacts, &results, 0);
	printf("There were  %d results found\n", results.size);
	if (!results.size)
		return;

	qsort(results.a, results.size, sizeof(int), compare_int);

	puts("Do you want to remove them all? (Y/N)");
	choice = read_char(stdin);
	if (choice == 'Y' || choice == 'y') {
		for (int i=results.size-1; i >= 0; --i) {
			erase_void(contacts, results.a[i], results.a[i]);
		}
		saved = 0;
	} else {
		for (int i=results.size-1; i >= 0; --i) {
			tmp_c = GET_CONTACT(contacts, results.a[i]);
			print_contact(tmp_c);
			puts("Do you want to remove the above contact? (Y/N)\n");
			choice = read_char(stdin);
			if (choice == 'Y' || choice == 'y') {
				erase_void(contacts, results.a[i], results.a[i]);
				saved = 0;
			}
		}
	}
}


int main(int argc, char** argv)
{
	vector_void contacts;
	char choice;
	int quit = 0;	
	vec_void(&contacts, 0, 10, sizeof(contact), free_contact, NULL);
	saved = 0;

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
			sort_contacts(&contacts);
			break;

		case 'F':
		case 'f':
			find_contacts(&contacts, NULL, 1);
			break;

		case 'Q':
		case 'q':
			//TODO
			if (!saved) {
				puts("You have unsaved changes! Are you sure you want to quit? (Y/N)");
				choice = read_char(stdin);
				if (choice == 'y' || choice == 'y')
					quit = 1;
				else
					quit = 0;
			} else {
				quit = 1;
			}
			break;

		case '?':
			print_menu();
			break;
		}
		putchar('\n');
	}




	return 0;
}
