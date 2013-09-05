#include "vector_attribute.h"

#include <assert.h>

#define STDERR(X) fprintf(stderr, X)

#define VEC_attribute_ALLOCATOR(x) ((x) * 2)


size_t VECTOR_attribute_SZ = 50;



vector_attribute* vec_attribute_heap(size_t size, size_t capacity)
{
	vector_attribute* vec;
	if (!(vec = malloc(sizeof(vector_attribute)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VECTOR_attribute_SZ;

	if (!(vec->a = malloc(vec->capacity*sizeof(attribute)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	return vec;
}


vector_attribute* init_vec_attribute_heap(attribute* vals, size_t num)
{
	vector_attribute* vec;
	
	if (!(vec = malloc(sizeof(vector_attribute)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->capacity = num + VECTOR_attribute_SZ;
	vec->size = num;
	if (!(vec->a = malloc(vec->capacity*sizeof(attribute)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	memcpy(vec->a, vals, sizeof(attribute)*num);

	return vec;
}


int vec_attribute(vector_attribute* vec, size_t size, size_t capacity)
{
	vec->size = (size > 0) ? size : 0;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + VECTOR_attribute_SZ;

	if (!(vec->a = malloc(vec->capacity*sizeof(attribute)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	return 1;
}


int init_vec_attribute(vector_attribute* vec, attribute* vals, size_t num)
{
	vec->capacity = num + VECTOR_attribute_SZ;
	vec->size = num;
	if (!(vec->a = malloc(vec->capacity*sizeof(attribute)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	memcpy(vec->a, vals, sizeof(attribute)*num);

	return 1;
}

void vec_attribute_copy(void* dest, void* src)
{
	vector_attribute* vec1 = dest;
	vector_attribute* vec2 = src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	/*not much else we can do here*/
	if (!(vec1->a = malloc(vec2->capacity*sizeof(int)))) {
		assert(vec1->a != NULL);
		return;
	}
	
	memcpy(vec1->a, vec2->a, vec2->size*sizeof(attribute));
	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
}





int push_attribute(vector_attribute* vec, attribute a)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		vec->a[vec->size++] = a;
	} else {
		tmp_sz = VEC_attribute_ALLOCATOR(vec->capacity);
		if (!(tmp = realloc(vec->a, sizeof(attribute)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->a[vec->size++] = a;
		vec->capacity = tmp_sz;
	}
	return 1;
}



attribute pop_attribute(vector_attribute* vec)
{
	return vec->a[--vec->size];
}

attribute* back_attribute(vector_attribute* vec)
{
	return &vec->a[vec->size-1];
}


int extend_attribute(vector_attribute* vec, size_t num)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VECTOR_attribute_SZ;
		if (!(tmp = realloc(vec->a, sizeof(attribute)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	vec->size += num;
	return 1;
}


int insert_attribute(vector_attribute* vec, size_t i, attribute a)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(attribute));
		vec->a[i] = a;
	} else {
		tmp_sz = VEC_attribute_ALLOCATOR(vec->capacity);
		if (!(tmp = realloc(vec->a, sizeof(attribute)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(attribute));
		vec->a[i] = a;
		vec->capacity = tmp_sz;
	}

	vec->size++;
	return 1;
}


int insert_array_attribute(vector_attribute* vec, size_t i, attribute* a, size_t num)
{
	void* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + VECTOR_attribute_SZ;
		if (!(tmp = realloc(vec->a, sizeof(attribute)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[i+num], &vec->a[i], (vec->size-i)*sizeof(attribute));
	memcpy(&vec->a[i], a, num*sizeof(attribute));
	vec->size += num;
	return 1;
}


void erase_attribute(vector_attribute* vec, size_t start, size_t end)
{
	size_t d = end - start + 1;
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(attribute));
	vec->size -= d;
}


int reserve_attribute(vector_attribute* vec, size_t size)
{
	void* tmp;
	if (vec->capacity < size) {
		if (!(tmp = realloc(vec->a, sizeof(attribute)*(size+VECTOR_attribute_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = size + VECTOR_attribute_SZ;
	}
	return 1;
}




int set_capacity_attribute(vector_attribute* vec, size_t size)
{
	void* tmp;
	if (size < vec->size) {
		vec->size = size;
	}

	if (!(tmp = realloc(vec->a, sizeof(attribute)*size))) {
		assert(tmp != NULL);
		return 0;
	}
	vec->a = tmp;
	vec->capacity = size;
	return 1;
}



void set_val_sz_attribute(vector_attribute* vec, attribute val)
{
	size_t i;
	for (i=0; i<vec->size; i++) {
		vec->a[i] = val;
	}
}


void set_val_cap_attribute(vector_attribute* vec, attribute val)
{
	size_t i;
	for (i=0; i<vec->capacity; i++) {
		vec->a[i] = val;
	}
}


void clear_attribute(vector_attribute* vec) { vec->size = 0; }

void free_vec_attribute_heap(void* vec)
{
	vector_attribute* tmp = vec;
	free(tmp->a);
	free(tmp);
}

void free_vec_attribute(void* vec)
{
	vector_attribute* tmp = vec;
	free(tmp->a);
	tmp->size = 0;
	tmp->capacity = 0;
}
