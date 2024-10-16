#ifndef RESURVIVOR__DYNAMIC_ARRAY_H
#define RESURVIVOR__DYNAMIC_ARRAY_H

#include <malloc.h>

/**
 * @brief
 */
typedef struct {

    void** data;
    unsigned size;
    unsigned capacity;
    unsigned minimum_capacity;
    double minimum_percentage_allowed; // minimum percentage of capacity filled that is allowed, if lower space is consumed then underlying array shrinking occurs

    // callbacks
    void (*datapoint_destroyer)(void*);
    int (*datapoint_equals)(void*, void*);
    void* (*datapoint_copy)(void*);

} DynamicArray;

/**
 * @brief Constructor for DynamicArray
 */
DynamicArray* dynamic_array_init(unsigned minimum_capacity, double minimum_percentage_allowed);

/**
 * @brief Destructor for DynamicArray
 */
void dynamic_array_destroy(DynamicArray* dym_arr);

/**
 * @brief Copy Constructor for DynamicArray. It performs a deep copy of the array, meaning every "object" stored in the array is also replicated (other than passing referencing to the objects to the new array)
 */
DynamicArray* dynamic_array_copy(DynamicArray* a_dym_arr);

/**
 * @brief Append an element to the end of the array. If array's capacity is surpassed, new and more memory will be allocated
 */
void dynamic_array_append(DynamicArray* dym_arr, void* element);

/**
 * @brief Deletes an element from the array. If number of elements drops significantly low with respect to underlying array capacity, new memory is allocated and the underlying array is shrunk. All appearances of the element will be deleted.
 */
int dynamic_array_delete(DynamicArray* dym_arr, void* element);

/**
 * @brief Deletes element at a particular position of the array
 * @param pos can be the 0-indexed position where the deletion needs to happen or it can be a negative integer number in [-size, -1] where size is the number of elements in the array, requesting the -n (n > 0) element of the array means that you want the nth element starting from the end of the array
 */
int dynamic_array_delete_at_pos(DynamicArray* dym_arr, int pos);

/**
 * @brief Deletes all elements of the array
 */
void dynamic_array_clear(DynamicArray* dym_arr);

/**
 * @brief Checks whether an element is inside the array.
 * @return int 0 if element is not found, int 1 if element is found
 */
int dynamic_array_find(DynamicArray* dym_arr, void* element);

/**
 * @brief Checks whether an element is inside the array and returns the indices of all the positions of the array where the element is found
 */
DynamicArray* dynamic_array_findall(DynamicArray* dym_arr, void* element);

/**
 * @brief Returns the element in specified position
 * @param pos can be the 0-indexed position where the deletion needs to happen or it can be a negative integer number in [-size, -1] where size is the number of elements in the array, requesting the -n (n > 0) element of the array means that you want the nth element starting from the end of the array
 * @return void* reference to the element that sits in specified position, NULL if any error occurs like position out of bounds or NULL provided as the dym_arr argument
 */
void* dynamic_array_get(DynamicArray* dym_arr, int pos);

/**
 * @brief Returns all elements of the underlying array in a built-in C array. All elements returned are COPIES of the elements of the dynamic array
 * @return void** array that holds copies of all the elements inside the dynamic array, NULL if dynamic array has no elements inside or error occurs (NULL dynamic array provided as argument)
 */
void** dynamic_array_get_all(DynamicArray* dym_arr);

/**
 * @brief Changes the capacity of the underlying array and copies all elements inside the array to the newly allocated array
 */
void _dynamic_array_change_capacity(DynamicArray* arr, unsigned capacity_new);

#endif /* RESURVIVOR__DYNAMIC_ARRAY_H */