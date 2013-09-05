#include "vector_contact.h"

#include <assert.h>

#define STDERR(X) fprintf(stderr, X)

#define VEC_contact_ALLOCATOR(x) ((x) * 2)


size_t VECTOR_contact_SZ = 50;



vector_contact* vec_contact_heap(size_t size, size_t capacity)
{
	vector_contact* vec;
	if (!(vec = malloc(sizeof(vector_contact)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VECTOR_contact_SZ;

	if (!(vec->a = malloc(vec->capacity*sizeof(contact)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	return vec;
}


vector_contact* init_vec_contact_heap(contact* vals, size_t num)
{
	vector_contact* vec;
	
	if (!(vec = malloc(sizeof(vector_contact)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->capacity = num + VECTOR_contact_SZ;
	vec->size = num;
	if (!(vec->a = malloc(vec->capacity*sizeof(contact)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	memcpy(vec->a, vals, sizeof(contact)*num);

	return vec;
}


int vec_contact(vector_contact* vec, size_t size, size_t capacity)
{
	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VECTOR_contact_SZ;

	if (!(vec->a = malloc(vec->capacity*sizeof(contact)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	return 1;
}


int init_vec_contact(vector_contact* vec, contact* vals, size_t num)
{
	vec->capacity = num + VECTOR_contact_SZ;
	vec->size = num;
	if (!(vec->a = malloc(vec->capacity*sizeof(contact)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	memcpy(vec->a, vals, sizeof(contact)*num);

	return 1;
}

void vec_contact_copy(void* dest, void* src)
{
	vector_contact* vec1 = dest;
	vector_contact* vec2 = src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	/*not much else we can do here*/
	if (!(vec1->a = malloc(vec2->capacity*sizeof(int)))) {
		assert(vec1->a != NULL);
		return;
	}
	
	memcpy(vec1->a, vec2->a, vec2->size*sizeof(contact));
	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
}





int push_contact(vector_contact* vec, contact a)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		vec->a[vec->size++] = a;
	} else {
		tmp_sz = VEC_contact_ALLOCATOR(vec->capacity);
		if (!(tmp = realloc(vec->a, sizeof(contact)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->a[vec->size++] = a;
		vec->capacity = tmp_sz;
	}
	return 1;
}



contact pop_contact(vector_contact* vec)
{
	return vec->a[--vec->size];
}

contact* back_contact(vector_contact* vec)
{
	return &vec->a[vec->size-1];
}


int extend_contact(vector_contact* vec, size_t num)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VECTOR_contact_SZ;
		if (!(tmp = realloc(vec->a, sizeof(contact)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	vec->size += num;
	return 1;
}


int insert_contact(vector_contact* vec, size_t i, contact a)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(contact));
		vec->a[i] = a;
	} else {
		tmp_sz = VEC_contact_ALLOCATOR(vec->capacity);
		if (!(tmp = realloc(vec->a, sizeof(contact)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(contact));
		vec->a[i] = a;
		vec->capacity = tmp_sz;
	}

	vec->size++;
	return 1;
}


int insert_array_contact(vector_contact* vec, size_t i, contact* a, size_t num)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VECTOR_contact_SZ;
		if (!(tmp = realloc(vec->a, sizeof(contact)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[i+num], &vec->a[i], (vec->size-i)*sizeof(contact));
	memcpy(&vec->a[i], a, num*sizeof(contact));
	vec->size += num;
	return 1;
}


void erase_contact(vector_contact* vec, size_t start, size_t end)
{
	size_t d = end - start + 1;
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(contact));
	vec->size -= d;
}


int reserve_contact(vector_contact* vec, size_t size)
{
	void* tmp;
	if (vec->capacity < size) {
		if (!(tmp = realloc(vec->a, sizeof(contact)*(size+VECTOR_contact_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = size + VECTOR_contact_SZ;
	}
	return 1;
}




int set_capacity_contact(vector_contact* vec, size_t size)
{
	void* tmp;
	if (size < vec->size) {
		vec->size = size;
	}

	if (!(tmp = realloc(vec->a, sizeof(contact)*size))) {
		assert(tmp != NULL);
		return 0;
	}
	vec->a = tmp;
	vec->capacity = size;
	return 1;
}



void set_val_sz_contact(vector_contact* vec, contact val)
{
	size_t i;
	for (i=0; i<vec->size; i++) {
		vec->a[i] = val;
	}
}


void set_val_cap_contact(vector_contact* vec, contact val)
{
	size_t i;
	for (i=0; i<vec->capacity; i++) {
		vec->a[i] = val;
	}
}


void clear_contact(vector_contact* vec) { vec->size = 0; }

void free_vec_contact_heap(void* vec)
{
	vector_contact* tmp = vec;
	free(tmp->a);
	free(tmp);
}

void free_vec_contact(void* vec)
{
	vector_contact* tmp = vec;
	free(tmp->a);
	tmp->size = 0;
	tmp->capacity = 0;
}
