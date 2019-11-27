
#include "cpim.h"

//include this before cvector.h for mystrdup
#include "c_utils.h"

#define CVEC_ONLY_INT
#define CVECTOR_IMPLEMENTATION
#include "cvector.h"


#define CLIST_IMPLEMENTATION
#include "clist.h"

#include <stdio.h>


int main(int argc, char** argv)
{
	cvector_contact contacts;
	char choice;
	int quit = 0;
	cvec_contact(&contacts, 0, 10, free_contact, NULL);
	saved = 1;

	print_menu();
	while (!quit) {
		
		puts("What action would you like to perform?");
		choice = read_char(stdin, SPACE_SET, 0, 1);

		switch (choice) {
		case 'A':
		case 'a':
			add_contact(&contacts);
			break;

		case 'D':
		case 'd':
			display_contacts(&contacts);
			break;

		case 'E':
		case 'e':
			edit_contacts(&contacts);
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
				puts("You have unsaved changes! Are you sure you want to quit? (y/N)");
				choice = read_char(stdin, SPACE_SET_NO_NEWLINE, 0, 1);
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

		default:
			printf("'%c' is not a valid choice!\n", choice);
			print_menu();
		}
		putchar('\n');
	}

	cvec_free_contact(&contacts);


	return 0;
}
