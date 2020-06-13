#include "cpim.h"

#include "cxml.h"
#include "c_utils.h"

#include <assert.h>
#include <string.h>
/*
 * included by cxml.h
 * c_utils is included only in cxml.c
#include "clist.h"
*/

int saved;

/* puts adds a newline ... change all to fputs for consistency? */
void print_menu()
{
	puts("Actions\n-------");
	puts("Add Contact (A)");
	puts("Display Contacts (D)");
	puts("Edit Contacts (E)");
	puts("Save Contacts (V)");
	puts("Load Contacts (L)");
	puts("Remove Contacts (R)");
	puts("Sort (S)");
	puts("Find (F)");
	puts("Quit (Q)");
	puts("Help (?)");

}



void free_contact_list(list_head* contacts)
{
	list_head* p, *n;
	contact* tmp_contact;

	if (!contacts)
		return;

	list_for_each_safe(p, n, contacts) {
		tmp_contact = list_entry(p, contact, list);
		list_del(p);
		free_contact(tmp_contact);
	}
}

void free_attrib_list(list_head* attribs)
{
	list_head* p, *n;
	attribute* tmp_attrib;

	if (!attribs)
		return;

	list_for_each_safe(p, n, attribs) {
		tmp_attrib = list_entry(p, attribute, list);
		list_del(p);
		free_attribute(tmp_attrib);
	}
}



void free_attribute(void* tmp)
{
	attribute* a = (attribute*)tmp;
	free(a->name);
	free(a->value);
	free(a);
}

void free_contact(void* tmp)
{
	contact* c = (contact*)tmp;
	free(c->first);
	free(c->last);
	free(c->phone);
	free_attrib_list(&c->attribs);
	free(c);
}








void add_contact(list_head* contacts)
{
	contact* tmp_c;
	attribute* tmp_attrib;
	list_head* attribs;
	char choice;

	tmp_c = malloc(sizeof(contact)); assert(tmp_c);

	puts("Enter first name:");
	tmp_c->first = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

	puts("Enter last name:");
	tmp_c->last = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

	puts("Enter phone number:");
	tmp_c->phone = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

	list_add_tail(&tmp_c->list, contacts);

	INIT_LIST_HEAD(&tmp_c->attribs);
	attribs = &tmp_c->attribs;

	puts("Do you want to add any attributes? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);

	while (choice == 'Y' || choice == 'y') {
		tmp_attrib = malloc(sizeof(attribute)); assert(tmp_attrib);

		puts("Enter attribute name:");
		tmp_attrib->name = read_string(stdin, SPACE_SET, '\n', 0);

		puts("Enter attribute value:");
		tmp_attrib->value = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

		list_add_tail(&tmp_attrib->list, attribs);

		puts("Do you want to add another attribute? (y/N)");
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	}
	saved = 0;
}

void save_contacts(list_head* contacts)
{
	char* tmp_str = NULL;
	contact* c;
	attribute* a;
	
	puts("Enter the name of a file to save to:");
	tmp_str = read_string(stdin, SPACE_SET, '\n', 0);

	FILE* file = fopen(tmp_str, "w");
	free(tmp_str);
	if (!file) {
		perror("Error opening file");
		return;
	}

	fprintf(file, "<contacts>\n");

	list_for_each_entry(c, contact, contacts, list) {
		fprintf(file, "\t<contact>\n");
		fprintf(file, "\t\t<lastname>%s</lastname>\n", c->last);
		fprintf(file, "\t\t<firstname>%s</firstname>\n", c->first);
		fprintf(file, "\t\t<phone>%s</phone>\n", c->phone);
		list_for_each_entry(a, attribute, &c->attribs, list) {
			fprintf(file, "\t\t<%s>%s</%s>\n", a->name, a->value, a->name);
		}
		fprintf(file, "\t</contact>\n");
	}
	fprintf(file, "</contacts>\n");
	fclose(file);
	saved = 1;

	puts("Contacts saved successfully.");
}

void load_contacts(list_head* contacts)
{
	contact* tmp_c;
	attribute* tmp_attrib;
	char* tmp_str = NULL;
	char choice;

	int overwritten = 1;
	if (!list_empty(contacts)) {
		overwritten = 0;
		puts("Do you want to overwrite current contacts? (y/N)");
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
		if (choice == 'y' || choice == 'Y') {
			free_contact_list(contacts);
			overwritten = 1;
		}
	}
	
	puts("Enter the name of a file to load:");
	tmp_str = read_string(stdin, SPACE_SET, '\n', 0);

	FILE* file = fopen(tmp_str, "r");
	free(tmp_str);
	if (!file) {
		perror("Failed to open file");
		return;
	}
	
	xml_tree* tree = new_xml_tree();
	if (!tree || !parse_xml_file(file, tree))
		goto exit;

	xml_tree* xml_contact, *xml_contact_members;

	list_for_each_entry(xml_contact, xml_tree, &tree->child_list, list) {
		tmp_c = malloc(sizeof(contact)); assert(tmp_c);
		INIT_LIST_HEAD(&tmp_c->attribs);

		list_add_tail(&tmp_c->list, contacts);

		list_for_each_entry(xml_contact_members, xml_tree, &xml_contact->child_list, list) {
			if (!strcmp(xml_contact_members->key, "firstname"))
				tmp_c->first = mystrdup(xml_contact_members->value);
			else if (!strcmp(xml_contact_members->key, "lastname"))
				tmp_c->last = mystrdup(xml_contact_members->value);
			else if (!strcmp(xml_contact_members->key, "phone"))
				tmp_c->phone = mystrdup(xml_contact_members->value);
			else {
				tmp_attrib = malloc(sizeof(attribute)); assert(tmp_attrib);
				list_add_tail(&tmp_attrib->list, &tmp_c->attribs);
				tmp_attrib->name = mystrdup(xml_contact_members->key);
				tmp_attrib->value = mystrdup(xml_contact_members->value);
			}
		}
	}

	puts("Contacts loaded successfully.");
	saved = overwritten;

exit:
	free_xml_tree(tree);
	fclose(file);

}


void print_contact(contact* c)
{
	attribute* a;
	printf("Last Name: %s\n", c->last);
	printf("First Name: %s\n", c->first);
	printf("Phone Number: %s\n", c->phone);

	list_for_each_entry(a, attribute, &c->attribs, list) {
		printf("%s: %s\n", a->name, a->value);
	}
}

void display_contacts(list_head* contacts)
{
	contact* c;
	list_for_each_entry(c, contact, contacts, list) {
		print_contact(c);
		putchar('\n');
	}
}


int compare_first(list_head* contact1, list_head* contact2)
{
	contact* c1 = list_entry(contact1, contact, list);
	contact* c2 = list_entry(contact2, contact, list);
	return strcmp(c1->first, c2->first);
}

int compare_last(list_head* contact1, list_head* contact2)
{
	contact* c1 = list_entry(contact1, contact, list);
	contact* c2 = list_entry(contact2, contact, list);
	return strcmp(c1->last, c2->last);
}

int compare_contact(list_head* contact1, list_head* contact2)
{
	contact* c1 = list_entry(contact1, contact, list);
	contact* c2 = list_entry(contact2, contact, list);
	int ret = strcmp(c1->last, c2->last);
	if (!ret)
		return strcmp(c1->first, c2->first);
	return ret;
}

void sort_contacts(list_head* contacts)
{
	char choice;

	puts("Do you want to sort by last name, first name or both? (l/f/B)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'L' || choice == 'l')
		sort_list(contacts, compare_last);
	else if (choice == 'F' || choice == 'f')
		sort_list(contacts, compare_first);
	else
		sort_list(contacts, compare_contact);

	
	puts("\nSorted successfully");
}

typedef struct contact_ptr_list
{
	contact* c;
	list_head list;
} contact_ptr_list;


void find_contacts(list_head* contacts, list_head* results_out, int* size_out, int print_results)
{
	contact *tmp_c;
	char* last = NULL, *first = NULL;
	char choice;
	contact_ptr_list* contact_ptr, *contact_ptr2;
	LIST_HEAD(results);

	int both = 0;

	puts("Do you want to search by last name, first name, or both? (l/f/B)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice != 'F' && choice != 'f') {
		puts("Enter the last name:");
		last = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
	}
	if (choice != 'L' && choice != 'l') {
		puts("Enter the first name:");
		first = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
		if (last)
			both = 1;
	}

	int size =0;
	list_for_each_entry(tmp_c, contact, contacts, list) {
		if (both) {
			if (!strcmp(last, tmp_c->last) && !strcmp(first, tmp_c->first)) {
				contact_ptr = malloc(sizeof(contact_ptr_list)); assert(contact_ptr);
				contact_ptr->c = tmp_c;
				list_add_tail(&contact_ptr->list, &results);
				++size;
			}
		} else if (last) {
			if (!strcmp(last, tmp_c->last)) {
				contact_ptr = malloc(sizeof(contact_ptr_list)); assert(contact_ptr);
				contact_ptr->c = tmp_c;
				list_add_tail(&contact_ptr->list, &results);
				++size;
			}
		} else if (!strcmp(first, tmp_c->first)) {
			contact_ptr = malloc(sizeof(contact_ptr_list)); assert(contact_ptr);
			contact_ptr->c = tmp_c;
			list_add_tail(&contact_ptr->list, &results);
			++size;
		}
	}

	if (print_results) {
		printf("\n%d Results Found:\n=================\n", size);
		list_for_each_entry(contact_ptr, contact_ptr_list, &results, list) {
			print_contact(contact_ptr->c);
			putchar('\n');
		}
	}

	free(first);
	free(last);

	if (results_out && !list_empty(&results)) {
		list_replace(&results, results_out);
	} else {
		list_for_each_entry_safe(contact_ptr, contact_ptr2, contact_ptr_list, &results, list) {
 		   free(contact_ptr);
	   	}
	}
	if (size_out)
		*size_out = size;
}


void remove_contact(list_head* contacts)
{
	char choice;
	LIST_HEAD(results);
	list_head* p,*n;
	contact_ptr_list* contact_ptr;
	int size;

	puts("First search for a contact to remove.");
	find_contacts(contacts, &results, &size, 0);
	printf("There were %d results found\n", size);
	if (list_empty(&results))
		goto exit;

	puts("Do you want to remove them all? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		list_for_each(p, &results) {
			contact_ptr = list_entry(p, contact_ptr_list, list);
			list_del(&contact_ptr->c->list);
			free_contact(contact_ptr->c);
		}
		saved = 0;
	} else {
		list_for_each_entry(contact_ptr, contact_ptr_list, &results, list) {
			putchar('\n');
			print_contact(contact_ptr->c);
			puts("\nDo you want to remove the above contact? (y/N)");
			choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
			if (choice == 'Y' || choice == 'y') {
				list_del(&contact_ptr->c->list);
				free_contact(contact_ptr->c);
				saved = 0;
			}
		}
	}

exit:
	list_for_each_safe(p, n, &results) {
		free(list_entry(p, contact_ptr_list, list));
	}
}

void edit_contact(contact* c, int print_first)
{
	attribute* a;
	char choice;

	if (print_first) {
		print_contact(c);
		putchar('\n');
	}

	puts("Do you want to edit the first name? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		puts("Enter first name:");
		free(c->first);
		c->first = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
	}

	puts("Do you want to edit the last name? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		puts("Enter last name:");
		free(c->last);
		c->last = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
	}

	puts("Do you want to edit the phone number? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		puts("Enter phone number:");
		free(c->phone);
		c->phone = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
	}

	list_head* p, *n;
	list_for_each_safe(p, n, &c->attribs) {
		a = list_entry(p, attribute, list);
		
		printf("Do you want to remove or edit the %s attribute? (r/e/N)\n", a->name);
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
		if (choice == 'R' || choice == 'r') {
			list_del(p);
			free_attribute(a);
		} else if (choice == 'E' || choice == 'e') {
			free(a->value);
			puts("Enter the new value:");
			a->value = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
		}
	}

	puts("Do you want to add any attributes? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);

	while (choice == 'Y' || choice == 'y') {
		a = malloc(sizeof(attribute)); assert(a);
		puts("Enter attribute name:");
		a->name = read_string(stdin, SPACE_SET, '\n', 0);

		puts("Enter attribute value:");
		a->value = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

		list_add_tail(&a->list, &c->attribs);

		puts("Do you want to add another attribute? (y/N)");
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	}
}

void edit_contacts(list_head* contacts)
{
	char choice;
	LIST_HEAD(results);
	list_head* p, *n;
	contact_ptr_list* contact_ptr;
	int size;

	puts("First search for a contact to edit.");
	find_contacts(contacts, &results, &size, 0);
	printf("There were %d results found\n", size);
	if (list_empty(&results))
		return;

	puts("Do you want to edit them all? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		list_for_each_entry(contact_ptr, contact_ptr_list, &results, list) { 
			putchar('\n');
			edit_contact(contact_ptr->c, 1);
		}
		saved = 0;
	} else {
		list_for_each_entry(contact_ptr, contact_ptr_list, &results, list) { 
			putchar('\n');
			print_contact(contact_ptr->c);
			putchar('\n');
			puts("Do you want to edit the above contact? (y/N)");
			choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
			if (choice == 'Y' || choice == 'y') {
				edit_contact(contact_ptr->c, 0);
				saved = 0;
			}
		}
	}

	list_for_each_safe(p, n, &results) {
		free(list_entry(p, contact_ptr_list, list));
	}
}
