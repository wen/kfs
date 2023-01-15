#include "vec.h"
#include "string.h"
#include "kheap.h"
#include "panic.h"

vec_t create_vec(void *addr, size_t max_size, cmp_t cmp)
{
	vec_t ret;

	ret.array = addr;
	bzero(ret.array, max_size * sizeof(void*));
	ret.size = 0;
	ret.max_size = max_size;
	ret.cmp = cmp;

	return ret;
}

void destroy_vec(vec_t *vec)
{
	kfree(vec->array);
}

void insert_vec(void *item, vec_t *vec)
{
	size_t i;
	cmp_t cmp = vec->cmp;

	for (i = 0; i < vec->size && cmp(vec->array[i], item); ++i)
		{} // no operation

	if (i == vec->size) {
		vec->array[vec->size++] = item;
	} else {
		void *tmp = vec->array[i];
		vec->array[i] = item;
		while (i < vec->size) {
			++i;
			void *tmp2 = vec->array[i];
			vec->array[i] = tmp;
			tmp = tmp2;
		}
		++vec->size;
	}
}

void *lookup_vec(size_t i, vec_t *vec)
{
	if (i >= vec->size)
		panic("Heap error");
	return vec->array[i];
}

void remove_vec(size_t i, vec_t *vec)
{
	for (; i < vec->size; ++i)
		vec->array[i] = vec->array[i+1];
	--vec->size;
}
