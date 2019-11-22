
#include <stdio.h>
#include <stdint.h>


#include <sqlite3.h>


typedef int64_t i64;

enum {
	CREATE_TABLE,
	INSERT,
	SELECT_ALL,
	SELECT_LAST,
	SELECT_FIRST_LAST,
	DEL_ID,
	DEL_LAST,
	DEL_FIRST_LAST
};



int main()
{

	const char* sql[] = {
		"CREATE TABLE contacts "
			"(id INTEGER PRIMARY KEY, "
			"first TEXT NOT NULL, "
			"middle TEXT NOT NULL, "
			"last TEXT NOT NULL, "
			"phone TEXT NOT NULL, "
			"attribs TEXT NOT NULL);",

		"INSERT INTO contacts (first, middle, last, phone, attribs) "
			"VALUES (?, ?, ?, ?, ?);",

		"SELECT * FROM contacts order by last;",
		"SELECT * FROM contacts WHERE last = ? order by last;",
		"SELECT * FROM contacts WHERE first = ? and last = ?;",

		"DELETE FROM contacts WHERE id = ?;",
		"DELETE FROM contacts WHERE last = ?;",
		"DELETE FROM contacts WHERE first = ? and last = ?;"

	};

	const char* values[] = {
		"Robert", "", "Winkler", "480-276-7978", "",
		"Garett", "R", "Winkler", "480-123-4567", "",
		"Robert", "Jordan", "Porter", "5555555555", ""
	};

	int rc;
	sqlite3* db;
	if (sqlite3_open("contacts.db", &db)) {
		printf("Failed to open db: %s\n", sqlite3_errmsg(db));
		return 0;
	}

	char* errmsg = NULL;

	if (sqlite3_exec(db, sql[CREATE_TABLE], NULL, NULL, &errmsg)) {
		printf("Failed to create table: %s\n", errmsg);
		sqlite3_free(errmsg);
		return 0;
	}

	sqlite3_stmt* insert;

	if (sqlite3_prepare_v2(db, sql[INSERT], -1, &insert, NULL)) {
		printf("Failed to prep insert: %s\n", sqlite3_errmsg(db));
		return 0;
	}

	//printf("%d\n", sizeof(values)/sizeof(char*)/4);
	for (int i=0; i<sizeof(values)/sizeof(char*)/5; ++i) {
		sqlite3_bind_text(insert, 1, values[i*5], -1, SQLITE_STATIC);
		sqlite3_bind_text(insert, 2, values[i*5+1], -1, SQLITE_STATIC);
		sqlite3_bind_text(insert, 3, values[i*5+2], -1, SQLITE_STATIC);
		sqlite3_bind_text(insert, 4, values[i*5+3], -1, SQLITE_STATIC);
		sqlite3_bind_text(insert, 5, values[i*5+4], -1, SQLITE_STATIC);
		if (sqlite3_step(insert) != SQLITE_DONE) {
			printf("Failed to insert (%d): %s\n", rc, sqlite3_errmsg(db));
			return 0;
		}
		sqlite3_reset(insert);
		printf("inserted %s %s\n", values[i*5], values[i*5+2]);
	}


	sqlite3_stmt* sel_all;
	if (sqlite3_prepare_v2(db, sql[SELECT_ALL], -1, &sel_all, NULL)) {
		printf("Failed to prep sel_last: %s\n", sqlite3_errmsg(db));
		return 0;
	}

	//sqlite3_bind_text(sel_last, 1, "Winkler", -1, SQLITE_STATIC);

	i64 id;
	char* first, *middle, *last, *phone;
	const char fmt[] = "%-10ld%-15s%-15s%-15s%-15s\n";
	const char fmt_hd[] = "%-10s%-15s%-15s%-15s%-15s\n";
	printf(fmt_hd, "id", "last", "first", "middle", "phone");
	puts("===================================================================");
	while ((rc = sqlite3_step(sel_all)) == SQLITE_ROW) {
			id = sqlite3_column_int64(sel_last, 0);
			first = sqlite3_column_text(sel_last, 1);
			middle = sqlite3_column_text(sel_last, 2);
			last = sqlite3_column_text(sel_last, 3);
			phone = sqlite3_column_text(sel_last, 4);

			printf(fmt, id, last, first, middle, phone);
	}






	sqlite3_finalize(insert);

	puts("success!");

	sqlite3_close(db);


	return 0;
}
