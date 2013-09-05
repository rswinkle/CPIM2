#ifndef VECTOR_contact_H
#define VECTOR_contact_H

#include "contact.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



/** Data structure for contact vector. */
typedef struct vector_contact
{
	contact* a;           /**< Array. */
	size_t size;       /**< Current size (amount you use when manipulating array directly). */
	size_t capacity;   /**< Allocated size of array; always >= size. */
} vector_contact;



extern size_t VECTOR_contact_SZ;

int vec_contact(vector_contact* vec, size_t size, size_t capacity);
int init_vec_contact(vector_contact* vec, contact* vals, size_t num);

vector_contact* vec_contact_heap(size_t size, size_t capacity);
vector_contact* init_vec_contact_heap(contact* vals, size_t num);

void vec_contact_copy(void* dest, void* src);

int push_contact(vector_contact* vec, contact a);
contact pop_contact(vector_contact* vec);

int extend_contact(vector_contact* vec, size_t num);
int insert_contact(vector_contact* vec, size_t i, contact a);
int insert_array_contact(vector_contact* vec, size_t i, contact* a, size_t num);
void erase_contact(vector_contact* vec, size_t start, size_t end);
int reserve_contact(vector_contact* vec, size_t size);
int set_capacity_contact(vector_contact* vec, size_t size);
void set_val_sz_contact(vector_contact* vec, contact val);
void set_val_cap_contact(vector_contact* vec, contact val);

contact* back_contact(vector_contact* vec);

void clear_contact(vector_contact* vec);
void free_vec_contact_heap(void* vec);
void free_vec_contact(void* vec);



#endif
