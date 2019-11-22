
#include "cpim.h"



//include this before cvector.h for mystrdup
#include "c_utils.h"

#define CVECTOR_IMPLEMENTATION
#include "cvector.h"

#define CLIST_IMPLEMENTATION
#include "clist.h"

#include <sqlite3.h>

#include <stdio.h>


int main(int argc, char** argv)
{
	char choice;
	int quit = 0;

	sqlite3* db;

	char default_db[] = "contacts.db";
	char* db_file = default_db;

	if (argc > 2) {
		printf("usage: %s [sqlite_database]\n", argv[0]);
		puts("The provided database will be opened (created if it doesn't exist).)");
		puts("If no database is provided contacts.db will opened/created");
		return 0;
	} else if (argc == 2) {
		db_file = argv[1];
	}

	init_db(db_file, &db);

	print_menu();
	while (!quit) {
		
		puts("What action would you like to perform?");
		choice = read_char(stdin, SPACE_SET, 0, 1);

		switch (choice) {
		case 'A':
		case 'a':
			add_contact(db);
			break;

		case 'D':
		case 'd':
			display_contacts();
			break;

		case 'E':
		case 'e':
			edit_contacts(db);
			break;

			/*
		case 'V':
		case 'v':
			save_contacts(&contacts);
			break;

		case 'L':
		case 'l':
			load_contacts(&contacts);
			break;
			*/

		case 'R':
		case 'r':
			remove_contact(db);
			break;

		case 'F':
		case 'f':
			find_contacts(db, NULL, LINE);
			break;

		case 'Q':
		case 'q':
			quit = 1;
			break;

		case '?':
			print_menu();
			break;
		}
		putchar('\n');
	}

	shutdown_db(db);


	return 0;
}
