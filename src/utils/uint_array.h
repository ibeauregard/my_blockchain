#ifndef UINT_ARRAY_H
#define UINT_ARRAY_H

typedef struct s_uint_array {
    unsigned int *array;
    size_t length;
} UintArray;

UintArray *new_uint_array(unsigned int *array, size_t length);
void free_uint_array(UintArray *array);

#endif
