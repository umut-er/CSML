#ifndef DYN_ARR
#define DYN_ARR

#include <assert.h>
#include <stdlib.h>

#define DYN_ARR_DECL(type) \
typedef struct{ \
    type* array; \
    size_t size; \
    size_t capacity; \
} dyn_arr_##type; \
\
dyn_arr_##type* get_array_##type(size_t initial_cap); \
void array_add_##type(dyn_arr_##type* array, type item); \
void array_remove_##type(dyn_arr_##type* array, int idx); \
void free_array_##type(dyn_arr_##type* array);

#define DYN_ARR_IMPL(type) \
dyn_arr_##type* get_array_##type(size_t initial_cap){ \
    assert(initial_cap > 0); \
\
    dyn_arr_##type* array = (dyn_arr_##type*)malloc(sizeof (dyn_arr_##type)); \
    array->size = 0; \
    array->capacity = initial_cap; \
    array->array = malloc(array->capacity * sizeof (type)); \
    return array; \
} \
\
void array_add_##type(dyn_arr_##type* array, type item){ \
    if(array->size == array->capacity){ \
        array->capacity *= 2; \
        array->array = (type*)realloc(array->array, array->capacity * sizeof (type)); \
    } \
    \
    array->array[array->size] = item; \
    array->size++; \
} \
\
void array_remove_##type(dyn_arr_##type* array, int idx){ \
    assert(idx < array->size && idx >= 0); \
\
    for(size_t i = idx; i < array->size-1; i++){ \
        array->array[i] = array->array[i+1]; \
    } \
    array->size--; \
\
    if(array->size < array->capacity / 2){ \
        array->capacity /= 2; \
        array->array = realloc(array->array, array->capacity * sizeof (type)); \
    } \
} \
\
void free_array_##type(dyn_arr_##type* array){ \
    free(array->array); \
    free(array); \
}

#endif
