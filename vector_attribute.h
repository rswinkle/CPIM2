#ifndef VECTOR_attribute_H
#define VECTOR_attribute_H

#include "attribute.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



/** Data structure for attribute vector. */
typedef struct vector_attribute
{
	attribute* a;           /**< Array. */
	size_t size;       /**< Current size (amount you use when manipulating array directly). */
	size_t capacity;   /**< Allocated size of array; always >= size. */
} vector_attribute;



extern size_t VECTOR_attribute_SZ;

int vec_attribute(vector_attribute* vec, size_t size, size_t capacity);
int init_vec_attribute(vector_attribute* vec, attribute* vals, size_t num);

vector_attribute* vec_attribute_heap(size_t size, size_t capacity);
vector_attribute* init_vec_attribute_heap(attribute* vals, size_t num);

void vec_attribute_copy(void* dest, void* src);

int push_attribute(vector_attribute* vec, attribute a);
attribute pop_attribute(vector_attribute* vec);

int extend_attribute(vector_attribute* vec, size_t num);
int insert_attribute(vector_attribute* vec, size_t i, attribute a);
int insert_array_attribute(vector_attribute* vec, size_t i, attribute* a, size_t num);
void erase_attribute(vector_attribute* vec, size_t start, size_t end);
int reserve_attribute(vector_attribute* vec, size_t size);
int set_capacity_attribute(vector_attribute* vec, size_t size);
void set_val_sz_attribute(vector_attribute* vec, attribute val);
void set_val_cap_attribute(vector_attribute* vec, attribute val);

attribute* back_attribute(vector_attribute* vec);

void clear_attribute(vector_attribute* vec);
void free_vec_attribute_heap(void* vec);
void free_vec_attribute(void* vec);



#endif
