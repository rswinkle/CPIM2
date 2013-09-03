#include "cxml.h"
#include "c_utils.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*
 
*/


xml_tree* new_xml_tree()
{
	xml_tree* tree = calloc(1, sizeof(xml_tree));
	INIT_LIST_HEAD(&tree->child_list);
	INIT_LIST_HEAD(&tree->list);
	return tree;
}

#define STDERR(STR) fprintf(stderr, STR)

void free_xml_tree(xml_tree* tree)
{
	list_head* p, *n;
	xml_tree* tmp_tree;

	printf("key = %s\n", tree->key);
	free(tree->key);
	if (tree->is_tree) {
		puts("tree");
//		getchar();
		list_for_each_safe(p, n, &tree->child_list) {
			tmp_tree = list_entry(p, xml_tree, list);
			printf("deleting %s\n", tmp_tree->key);
			free_xml_tree(tmp_tree);
		}
	} else {
		printf("value = %s\n", tree->value);
		free(tree->value);
	}

	free(tree);
}

int parse_xml_file(FILE* file, xml_tree* tree)
{
	char temp, temp2;
	char* tmp_str;
	xml_tree* tmp_tree;

	temp = getc(file);
	if (temp != '<') {
		STDERR("Parsing Error");
		return 0;
	}

	tmp_str = freadstring(file, '>', 0);
	if (!tmp_str) {
		fclose(file);
		perror("2");
		return 0;
	}

	tree->key = tmp_str;
	printf("key = %s %p\n", tmp_str, tree->key);
	while (1) {
		temp = getc(file);
		while (isspace(temp)) temp = getc(file);
		temp2 = getc(file);
		fseek(file, -2, SEEK_CUR);

		//if it's a string value
		if (temp != '<') {
			tmp_str = freadstring(file, '<', 0);
			if (!tmp_str) {
				perror("3");
				return 0;
			}
			tree->is_tree = 0;
			tree->value = tmp_str;
			
			temp = getc(file);
			if (temp != '/') {
				perror("3.5");
				return 0;
			}
			break;
			
		} else if (temp2 != '/') {      //it's a subtree
			tree->is_tree = 1;
			if (!(tmp_tree = new_xml_tree())) {
				fclose(file);
				perror("4");
				return 0;
			}
			if (!parse_xml_file(file, tmp_tree)) {
				perror("5");
				return 0;
			}
			list_add_tail(&tmp_tree->list, &tree->child_list);
		} else {
			getc(file);
			getc(file);
			break;
		}
	}
			
	printf("closing %s\n", tree->key);

	tmp_str = freadstring(file, '>', 0);
	if (!tmp_str) {
		perror("7");
		fclose(file);
		return 0;
	}
	if (strcmp(tmp_str, tree->key)) {
		perror("8");
		fclose(file);
		return 0;
	}
	
	//free closing tag
	free(tmp_str);

	return 1;
}





/*
int parse_xml(c_array* xml, xml_tree* tree)
{
	char* p = xml->data;
	int line = 1;

	if (*p != '<') {
		// parsing error at line
		return 0;
	}

	readstring(

*/	
