#ifndef CXML_H
#define CXML_H
#include <stdio.h>
#include "clist.h"


typedef struct xml_tree
{
	char* key;
	int is_tree;
	char* value;
	list_head list;
	list_head child_list;
} xml_tree;

xml_tree* new_xml_tree();
void free_xml_tree(xml_tree* tree);


int parse_xml(const char* str, xml_tree* tree);


int parse_xml_file(FILE* file, xml_tree* tree);


#endif
