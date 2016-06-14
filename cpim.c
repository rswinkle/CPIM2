#include "cpim.h"
#include "cxml.h"
#include "c_utils.h"
/*
 * included by cxml.h
 * c_utils is included only in cxml.c
#include "clist.h"
*/


CVEC_NEW_DEFS(contact, RESIZE)
CVEC_NEW_DEFS(attribute, RESIZE)

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
	for (int i=0; i<c->attribs.size; ++i)
		free_attribute(&c->attribs.a[i]);

	cvec_free_attribute(&c->attribs);
}





void add_contact(cvector_contact* contacts)
{
	contact tmp_c;
	attribute tmp_attrib;
	char choice;

	puts("Enter first name:");
	tmp_c.first = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

	puts("Enter last name:");
	tmp_c.last = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

	puts("Enter phone number:");
	tmp_c.phone = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

	cvec_attribute(&tmp_c.attribs, 0, 10);

	puts("Do you want to add any attributes? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);

	while (choice == 'Y' || choice == 'y') {
		puts("Enter attribute name:");
		tmp_attrib.name = read_string(stdin, SPACE_SET, '\n', 0);

		puts("Enter attribute value:");
		tmp_attrib.value = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

		cvec_push_attribute(&tmp_c.attribs, tmp_attrib);

		puts("Do you want to add another attribute? (y/N)");
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	}

	cvec_push_contact(contacts, tmp_c);
	saved = 0;
}

void save_contacts(cvector_contact* contacts)
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
	for (int i=0; i<contacts->size; ++i) {
		c = &contacts->a[i];
		fprintf(file, "\t<contact>\n");
		fprintf(file, "\t\t<lastname>%s</lastname>\n", c->last);
		fprintf(file, "\t\t<firstname>%s</firstname>\n", c->first);
		fprintf(file, "\t\t<phone>%s</phone>\n", c->phone);
		for (int j=0; j<c->attribs.size; ++j) {
			a = &c->attribs.a[j];
			fprintf(file, "\t\t<%s>%s</%s>\n", a->name, a->value, a->name);
		}
		fprintf(file, "\t</contact>\n");
	}
	fprintf(file, "</contacts>\n");
	fclose(file);
	saved = 1;

	puts("Contacts saved successfully.");
}

void load_contacts(cvector_contact* contacts)
{
	contact tmp_c;
	attribute tmp_attrib;
	char* tmp_str = NULL;
	char choice;

	int overwritten = 1;
	if (contacts->size) {
		overwritten = 0;
		puts("Do you want to overwrite current contacts? (y/N)");
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
		if (choice == 'y' || choice == 'Y') {
			for (int i=0; i<contacts->size; ++i)
				free_contact(&contacts->a[i]);
			cvec_clear_contact(contacts);
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
		cvec_attribute(&tmp_c.attribs, 0, 3);

		list_for_each_entry(xml_contact_members, xml_tree, &xml_contact->child_list, list) {
			if (!strcmp(xml_contact_members->key, "firstname"))
				tmp_c.first = mystrdup(xml_contact_members->value);
			else if (!strcmp(xml_contact_members->key, "lastname"))
				tmp_c.last = mystrdup(xml_contact_members->value);
			else if (!strcmp(xml_contact_members->key, "phone"))
				tmp_c.phone = mystrdup(xml_contact_members->value);
			else {
				tmp_attrib.name = mystrdup(xml_contact_members->key);
				tmp_attrib.value = mystrdup(xml_contact_members->value);

				cvec_push_attribute(&tmp_c.attribs, tmp_attrib);
			}
		}
		cvec_push_contact(contacts, tmp_c);
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

	for (int j=0; j<c->attribs.size; ++j) {
		a = &c->attribs.a[j];
		printf("%s: %s\n", a->name, a->value);
	}
}

void display_contacts(cvector_contact* contacts)
{
	for (int i=0; i<contacts->size; ++i) {
		print_contact(&contacts->a[i]);
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

void sort_contacts(cvector_contact* contacts)
{
	char choice;

	puts("Do you want to sort by last name, first name or both? (l/f/B)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'L' || choice == 'l')
		qsort(contacts->a, contacts->size, sizeof(contact), compare_last);
	else if (choice == 'F' || choice == 'f') 
		qsort(contacts->a, contacts->size, sizeof(contact), compare_first);
	else
		qsort(contacts->a, contacts->size, sizeof(contact), compare_contact);
	
	puts("\nSorted successfully");
}

void find_contacts(cvector_contact* contacts, cvector_i* results_out, int print_results)
{
	contact *tmp_c;
	char* last = NULL, *first = NULL;
	char choice;
	cvector_i results;
	cvec_i(&results, 0, contacts->size);

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

	int i;
	for (i=0; i<contacts->size; ++i) {
		tmp_c = &contacts->a[i];
		if (both) {
			if (!strcmp(last, tmp_c->last) && !strcmp(first, tmp_c->first))
				cvec_push_i(&results, i);
		} else if (last) {
			if (!strcmp(last, tmp_c->last))
				cvec_push_i(&results, i);
		} else if (!strcmp(first, tmp_c->first)) {
			cvec_push_i(&results, i);
		}
	}

	if (print_results) {
		printf("\n%lu Results Found:\n=================\n", (unsigned long)results.size);
		for (i=0; i<results.size; ++i) {
			print_contact(&contacts->a[results.a[i]]);
			putchar('\n');
		}
	}

	free(first);
	free(last);

	if (results_out)
		memcpy(results_out, &results, sizeof(cvector_i));
	else
		cvec_free_i(&results);
}


void remove_contact(cvector_contact* contacts)
{
	char choice;
	cvector_i results;

	puts("First search for a contact to remove.");
	find_contacts(contacts, &results, 0);
	printf("There were %lu results found\n", (unsigned long)results.size);
	if (!results.size)
		goto exit;

	qsort(results.a, results.size, sizeof(int), compare_int);

	puts("Do you want to remove them all? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		for (int i=results.size-1; i >= 0; --i) {
			free_contact(&contacts->a[results.a[i]]);
			cvec_erase_contact(contacts, results.a[i], results.a[i]);
		}
		saved = 0;
	} else {
		for (int i=results.size-1; i >= 0; --i) {
			putchar('\n');
			print_contact(&contacts->a[results.a[i]]);
			puts("\nDo you want to remove the above contact? (y/N)");
			choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
			if (choice == 'Y' || choice == 'y') {
				free_contact(&contacts->a[results.a[i]]);
				cvec_erase_contact(contacts, results.a[i], results.a[i]);
				saved = 0;
			}
		}
	}

exit:
	cvec_free_i(&results);
}

void edit_contact(contact* c, int print_first)
{
	attribute* a;
	attribute tmp_attrib;
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

	for (int j=0; j < c->attribs.size; ++j) {
		a = &c->attribs.a[j];
		printf("Do you want to remove or edit the %s attribute? (r/e/N)\n", a->name);
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
		if (choice == 'R' || choice == 'r') {
			free_attribute(&c->attribs.a[j]);
			cvec_erase_attribute(&c->attribs, j, j);
			--j;
		} else if (choice == 'E' || choice == 'e') {
			free(a->value);
			puts("Enter the new value:");
			a->value = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
		}
	}

	puts("Do you want to add any attributes? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);

	while (choice == 'Y' || choice == 'y') {
		puts("Enter attribute name:");
		tmp_attrib.name = read_string(stdin, SPACE_SET, '\n', 0);

		puts("Enter attribute value:");
		tmp_attrib.value = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

		cvec_push_attribute(&c->attribs, tmp_attrib);

		puts("Do you want to add another attribute? (y/N)");
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	}
}

void edit_contacts(cvector_contact* contacts)
{
	contact* tmp_c;
	char choice;
	cvector_i results;

	puts("First search for a contact to edit.");
	find_contacts(contacts, &results, 0);
	printf("There were %lu results found\n", (unsigned long)results.size);
	if (!results.size)
		goto exit;

	puts("Do you want to edit them all? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		for (int i=0; i < results.size; ++i) {
			putchar('\n');
			edit_contact(&contacts->a[results.a[i]], 1);
		}
		saved = 0;
	} else {
		for (int i=0; i < results.size; ++i) {
			tmp_c = &contacts->a[results.a[i]];
			putchar('\n');
			print_contact(tmp_c);
			putchar('\n');
			puts("Do you want to edit the above contact? (y/N)");
			choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
			if (choice == 'Y' || choice == 'y') {
				edit_contact(tmp_c, 0);
				saved = 0;
			}
		}
	}

exit:
	cvec_free_i(&results);
}
