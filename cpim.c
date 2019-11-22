#include "cpim.h"

#include "cxml.h"
#include "c_utils.h"



enum {
	CREATE_TABLE,

	INSERT,

	SELECT_ID,
	SELECT_ALL,
	SELECT_LAST,
	SELECT_FIRST,
	SELECT_LAST_FIRST,

	UPDATE_ALL_ID,

	DEL_ID,
	DEL_LAST,
	DEL_FIRST_LAST,

	NUM_STMTS
};

const char* sql[] = {
	"CREATE TABLE IF NOT EXISTS contacts "
		"(id INTEGER PRIMARY KEY, "
		"first TEXT NOT NULL, "
		"middle TEXT NOT NULL, "
		"last TEXT NOT NULL, "
		"phone TEXT NOT NULL, "
		"attribs TEXT NOT NULL);",

	"INSERT INTO contacts (first, middle, last, phone, attribs) "
		"VALUES (?, ?, ?, ?, ?);",

	"SELECT * FROM contacts where id = ?;",
	"SELECT * FROM contacts order by last, first;",
	"SELECT * FROM contacts WHERE last = ? order by last, first;",
	"SELECT * FROM contacts WHERE first = ? order by last, first;",
	"SELECT * FROM contacts WHERE last = ? and first = ? order by last, first;",

	"UPDATE contacts SET first = ?, middle = ?, last = ?, phone = ?, attribs = ? where id = ?;",

	"DELETE FROM contacts WHERE id = ?;",
	"DELETE FROM contacts WHERE last = ?;",
	"DELETE FROM contacts WHERE first = ? and last = ?;"

};

sqlite3_stmt* sqlstmts[NUM_STMTS];

// TODO make db a static global in here?
// so main doesn't have to know about it and it doesn't have
// to be passed as a parameter

void init_db(const char* db_file, sqlite3** db)
{
	if (sqlite3_open(db_file, db)) {
		printf("Failed to open %s: %s\n", db_file, sqlite3_errmsg(*db));
		exit(0);
	}
	create_table(*db);
	prepare_stmts(*db);
}

void shutdown_db(sqlite3* db)
{
	finalize_stmts();
	sqlite3_close(db);
}

void create_table(sqlite3* db)
{
	char* errmsg = NULL;
	if (sqlite3_exec(db, sql[CREATE_TABLE], NULL, NULL, &errmsg)) {
		printf("Failed to create table: %s\n", errmsg);
		sqlite3_free(errmsg);
		exit(0);
	}
}

void prepare_stmts(sqlite3* db)
{
	for (int i=INSERT; i<NUM_STMTS; ++i) {
		if (sqlite3_prepare_v2(db, sql[i], -1, &sqlstmts[i], NULL)) {
			printf("Failed to prep following statement:\n\"%s\"\nproduced errormsg: %s\n", sql[i], sqlite3_errmsg(db));
			exit(0);
		}
	}
}

void finalize_stmts()
{
	for (int i=INSERT; i<NUM_STMTS; ++i) {
		if (sqlite3_finalize(sqlstmts[i])) {
			puts("failed to finalize");
			//printf("Failed to finalize following statement:\n\"%s\"\nproduced errormsg: %s\n", sql[i], sqlite3_errmsg(db));
			exit(0);
		}
	}
}



/* puts adds a newline ... change all to fputs for consistency? */
void print_menu()
{
	puts("Actions\n-------");
	puts("Add Contact (A)");
	puts("Display Contacts (D)");
	puts("Edit Contacts (E)");
	//puts("Save Contacts (V)");
	//puts("Load Contacts (L)");
	puts("Remove Contacts (R)");
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
	free(c->middle);
	free(c->last);
	free(c->phone);
	cvec_free_void(&c->attribs);
}








void add_contact(sqlite3* db)
{
	char* first;
	char* last;
	char* middle;
	char* phone;

	attribute tmp_attrib;
	char choice;
	
	puts("You may leave any field blank, or put only an initial etc.\n");

	puts("Enter first name:");
	first = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

	puts("Enter middle name:");
	middle = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

	puts("Enter last name:");
	last = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

	puts("Enter phone number:");
	phone = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

	cvector_void attribs;
	cvec_void(&attribs, 0, 10, sizeof(attribute), free_attribute, NULL);

	puts("Do you want to add any attributes? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);

	while (choice == 'Y' || choice == 'y') {
		puts("Enter attribute name:");
		tmp_attrib.name = read_string(stdin, SPACE_SET, '\n', 0);

		puts("Enter attribute value:");
		tmp_attrib.value = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);

		cvec_push_void(&attribs, &tmp_attrib);

		puts("Do you want to add another attribute? (y/N)");
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	}
	/*
	 *  TODO
	 *  // convert to snprintf so I can insert string into attribs column
		for (int j=0; j<c->attribs.size; ++j) {
			a = GET_ATTRIBUTE(&c->attribs, j);
			fprintf(file, "\t\t<%s>%s</%s>\n", a->name, a->value, a->name);
		}
	*/

	sqlite3_stmt* s = sqlstmts[INSERT];
	sqlite3_bind_text(s, 1, first, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 2, middle, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 3, last, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 4, phone, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 5, "", -1, SQLITE_STATIC);
	int rc;
	if ((rc = sqlite3_step(sqlstmts[INSERT])) != SQLITE_DONE) {
		printf("Failed to insert (%d): %s\n", rc, sqlite3_errmsg(db));
		return;
	}
	sqlite3_reset(s);
	printf("inserted %s %s\n", first, last);

	free(first);
	free(last);
	free(middle);
	free(phone);

	cvec_free_void(&attribs);


}


/*
void load_contacts(cvector_void* contacts)
{
	contact tmp_c;
	contact* tmp_c_ptr;
	attribute tmp_attrib;
	cvector_void* attribs;
	char* tmp_str = NULL;
	char choice;

	int overwritten = 1;
	if (contacts->size) {
		overwritten = 0;
		puts("Do you want to overwrite current contacts? (y/N)");
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
		if (choice == 'y' || choice == 'Y') {
			cvec_clear_void(contacts);
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
		cvec_void(&tmp_c.attribs, 0, 3, sizeof(attribute), free_attribute, NULL);
		cvec_push_void(contacts, &tmp_c);
		tmp_c_ptr = (contact*)cvec_back_void(contacts);
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

				cvec_push_void(attribs, &tmp_attrib);
			}
		}
	}

	puts("Contacts loaded successfully.");
	saved = overwritten;

exit:
	free_xml_tree(tree);
	fclose(file);

}
*/


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

void print_contact_lineid(contact* c)
{
	const char fmt[] = "%-10ld%-15s%-15s%-15s%-15s\n";

	printf(fmt, c->id, c->last, c->first, c->middle, c->phone);
}

// any variable bindings for stmt must be done before the call
// results must be correctly initialized if non-NULL (clearing to 0 is valid)
int select_rows(sqlite3_stmt* stmt, cvector_i* results, int print)
{
	int count = 0, rc;

	contact c = { 0 };
	const char fmt_hd[] = "%-10s%-15s%-15s%-15s%-15s\n";
	if (print) {
		printf(fmt_hd, "id", "last", "first", "middle", "phone");
		puts("===================================================================");
	}
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		c.id = sqlite3_column_int64(stmt, 0);
		c.first  = (char*)sqlite3_column_text(stmt, 1);
		c.middle = (char*)sqlite3_column_text(stmt, 2);
		c.last   = (char*)sqlite3_column_text(stmt, 3);
		c.phone  = (char*)sqlite3_column_text(stmt, 4);

		if (print == LINE)
			print_contact_lineid(&c);
		// no need to free strings, they're handled by sqlite3

		if (results)
			cvec_push_i(results, c.id);

		count++;
	}

	// put this at the top?  first time it would be redundant
	sqlite3_reset(stmt);

	return count;
}

int get_contact_by_id(int id, contact* c)
{
	sqlite3_stmt* s = sqlstmts[SELECT_ID];
	sqlite3_bind_int(s, 1, id);
	int rc;
	if ((rc = sqlite3_step(s)) != SQLITE_ROW) {
		puts("no such id!");
		//printf("sqlite error: %s\n", sqlite3_errstr(rc));
		return 0;
	}

	c->id = sqlite3_column_int64(s, 0);
	c->first = mystrdup((char*)sqlite3_column_text(s, 1));
	c->middle = mystrdup((char*)sqlite3_column_text(s, 2));
	c->last = mystrdup((char*)sqlite3_column_text(s, 3));
	c->phone = mystrdup((char*)sqlite3_column_text(s, 4));

	// TODO attributes
	
	sqlite3_reset(s);

	return 1;
}



void display_contacts()
{
	select_rows(sqlstmts[SELECT_ALL], NULL, LINE);
}


int find_contacts(sqlite3* db, cvector_i* results, int print)
{
	char* last = NULL, *first = NULL;
	char choice;
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

	sqlite3_stmt* stmt;
	if (last) {
		if (first) {
			stmt = sqlstmts[SELECT_LAST_FIRST];
			sqlite3_bind_text(stmt, 1, last, -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 2, first, -1, SQLITE_STATIC);
		} else {
			stmt = sqlstmts[SELECT_LAST];
			sqlite3_bind_text(stmt, 1, last, -1, SQLITE_STATIC);
		}
	} else {
		stmt = sqlstmts[SELECT_FIRST];
		sqlite3_bind_text(stmt, 1, first, -1, SQLITE_STATIC);
	}

	return select_rows(stmt, results, print);
}


void remove_contact(sqlite3* db)
{
	char choice;
	cvector_i results;

	cvec_i(&results, 0, 20);

	puts("First search for a contact to remove.");
	int n_results = find_contacts(db, &results, LINE);
	printf("There were %d results found\n", n_results);
	if (!n_results)
		return;

	sqlite3_stmt* s = sqlstmts[DEL_ID];
	puts("Do you want to remove them all? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		for (int i=0; i<n_results; ++i) {
			// do I have to use int64 for the parameter because id is rowid is 64-bit?
			// or do I have to use int because I'm using an int? or does it not matter
			sqlite3_bind_int(s, 1, results.a[i]);
			sqlite3_step(s);
			sqlite3_reset(s);
		}
	} else {
		// TODO if the result set is large, it'd scroll off the page, so they'd have to scroll up
		// to keep checking the id's, so I could/should print the whole row or at least last, first
		// in addition to the id
		for (int i=0; i<n_results; ++i) {
			printf("Do you want to remove the above contact with id %d? (y/N)\n", results.a[i]);
			choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
			if (choice == 'Y' || choice == 'y') {
				sqlite3_bind_int(s, 1, results.a[i]);
				sqlite3_step(s);
				sqlite3_reset(s);
			}
		}
	}

	cvec_free_i(&results);
}

void edit_contact(int id, int print_first)
{
	attribute* a;
	attribute tmp_attrib;
	char choice;
	contact c = { 0 };

	if (!get_contact_by_id(id, &c))
		return;

	if (print_first) {
		//print line
		print_contact(&c);
		putchar('\n');
	}

	puts("Do you want to edit the first name? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		puts("Enter first name:");
		free(c.first);
		c.first = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
	}

	puts("Do you want to edit the middle name? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		puts("Enter middle name:");
		free(c.middle);
		c.middle = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
	}

	puts("Do you want to edit the last name? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		puts("Enter last name:");
		free(c.last);
		c.last = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
	}

	puts("Do you want to edit the phone number? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		puts("Enter phone number:");
		free(c.phone);
		c.phone = read_string(stdin, SPACE_SET_NO_NEWLINE, '\n', 0);
	}

	/*
	for (int j=0; j < c->attribs.size; ++j) {
		a = GET_ATTRIBUTE(&c->attribs, j);
		printf("Do you want to remove or edit the %s attribute? (r/e/N)\n", a->name);
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
		if (choice == 'R' || choice == 'r') {
			cvec_erase_void(&c->attribs, j, j);
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

		cvec_push_void(&c->attribs, &tmp_attrib);

		puts("Do you want to add another attribute? (y/N)");
		choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	}
	*/

	sqlite3_stmt* s = sqlstmts[UPDATE_ALL_ID];
	sqlite3_bind_text(s, 1, c.first, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 2, c.middle, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 3, c.last, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 4, c.phone, -1, SQLITE_STATIC);
	sqlite3_bind_text(s, 5, "", -1, SQLITE_STATIC);
	sqlite3_bind_int64(s, 6, c.id);
	if (sqlite3_step(s) != SQLITE_DONE) {
		puts("failed to update");
		//printf("Failed to delete (%d): %s\n", rc, sqlite3_errmsg(db));
		return;
	}
	sqlite3_reset(s);

	free_contact(&c);
}

void edit_contacts(sqlite3* db)
{
	char choice;
	cvector_i results;
	cvec_i(&results, 0, 20);

	puts("First search for a contact to edit.");
	int n_results = find_contacts(db, &results, LINE);
	printf("There were %d results found\n", n_results);
	if (!n_results)
		return;

	puts("Do you want to edit them all? (y/N)");
	choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
	if (choice == 'Y' || choice == 'y') {
		for (int i=0; i < results.size; ++i) {
			edit_contact(results.a[i], 1);
		}
	} else {
		/*
		for (int i=0; i < results.size; ++i) {
			tmp_c = GET_CONTACT(contacts, results.a[i]);
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
		*/
	}

	cvec_free_i(&results);
}
