#include <math.h>

#include "dynamic_array.h"

/**
 * @brief Constructor for DynamicArray
 */
DynamicArray* dynamic_array_init(unsigned given_minimum_capacity, double given_minimum_percentage_allowed)
{
    DynamicArray* arr = (DynamicArray*) malloc(sizeof(DynamicArray));
    arr -> minimum_capacity = given_minimum_capacity;
    arr -> minimum_percentage_allowed = given_minimum_percentage_allowed;
    arr -> capacity = given_minimum_capacity;
    arr -> size = 0;
    arr -> data = (void**) calloc(given_minimum_capacity, sizeof(void*));
    return arr;
}

void dynamic_array_destroy(DynamicArray* dym_arr)
{
    if (!dym_arr)
    {
        return;
    }

    for (int i = 0; i < dym_arr -> size; ++i)
    {
        free(dym_arr -> data[i]);
    }
    free(dym_arr -> data);
    free(dym_arr);

    return;
}


/**
 * @brief Copy Constructor for DynamicArray. It performs a deep copy of the array, meaning every "object" stored in the array is also replicated (other than passing referencing to the objects to the new array)
 */
DynamicArray* dynamic_array_copy(DynamicArray* a_dym_arr)
{
    if (!a_dym_arr)
    {
        return NULL;
    }

    DynamicArray* dym_arr_copy = (DynamicArray*) malloc(sizeof(DynamicArray));

    dym_arr_copy -> capacity = a_dym_arr -> capacity;
    dym_arr_copy -> size = a_dym_arr -> size;

    dym_arr_copy -> data = (void**) calloc(a_dym_arr -> capacity, sizeof(void*));
    for (int i = 0; i < a_dym_arr -> size; ++i)
    {
        dym_arr_copy -> data[i] = a_dym_arr -> datapoint_copy(a_dym_arr -> data[i]);
    }

    dym_arr_copy -> datapoint_destroyer = a_dym_arr -> datapoint_destroyer;
    dym_arr_copy -> datapoint_equals = a_dym_arr -> datapoint_equals;
    dym_arr_copy -> datapoint_copy = a_dym_arr -> datapoint_copy;

    return dym_arr_copy;
}


/**
 * @brief Append an element to the end of the array. If array's capacity is surpassed, new and more memory will be allocated
 */
void dynamic_array_append(DynamicArray* dym_arr, void* element)
{
    if (dym_arr -> size == dym_arr -> capacity)
    {
        _dynamic_array_change_capacity(dym_arr, 2U * (dym_arr -> capacity));
    }

    dym_arr -> data[dym_arr -> size] = dym_arr -> datapoint_copy(element);
    dym_arr -> size += 1;
}


/**
 * @brief Deletes an element from the array. If number of elements drops significantly low with respect to underlying array capacity, new memory is allocated and the underlying array is shrunk. All appearances of the element will be deleted.
 */
int dynamic_array_delete(DynamicArray* dym_arr, void* element)
{
    if (!dym_arr)
    {
        return 0;
    }

    unsigned deletions_occured = 0; // holds number of deletions that occured so far, it is used when a reposition of an element that will not be deleted is needed so no bubbles (empty positions) appear in the array
    for (int i = 0; i < dym_arr -> size; ++i)
    {
        if (dym_arr -> datapoint_equals(dym_arr -> data[i], element))
        {
            dym_arr -> datapoint_destroyer(dym_arr -> data[i]);
            dym_arr -> data[i] = NULL;
            deletions_occured += 1;
        }
        else
        {
            if (deletions_occured)
            {
                dym_arr -> data[i - deletions_occured] = dym_arr -> data[i];
                dym_arr -> data[i] = NULL; // this is not necessary
            }
        }
    }

    if (!deletions_occured)
    {
        return 0;
    }

    dym_arr -> size -= deletions_occured;
    
    if (dym_arr -> capacity > dym_arr -> minimum_capacity && dym_arr -> size < (unsigned)(dym_arr -> minimum_percentage_allowed * dym_arr -> capacity))
    {
        // due to multiple deletions it is possible that the shrinking of the underlying array isn't just by 2 but it may be a power of 2 so that the minimum capacity allowed is fullfilled
        unsigned new_capacity = dym_arr -> capacity / 2U;

        // if a single halfing of the underlying array doesn't satisfy the condition, keep dividing with a factor of two the underlying array's capacity until minimum capacity is reached or condition is met
        if (new_capacity > dym_arr -> minimum_capacity && dym_arr -> size < (unsigned)(dym_arr -> minimum_percentage_allowed * new_capacity))
        {
            new_capacity /= 2U;
        }

        _dynamic_array_change_capacity(dym_arr, new_capacity);
    }

    return 1;
}


/**
 * @brief Deletes element at a particular position of the array
 */
int dynamic_array_delete_at_pos(DynamicArray* dym_arr, int pos)
{
    if (!dym_arr)
    {
        return 0;
    }

    // check if given pos is within acceptable values
    if (pos < -((int)(dym_arr -> size)) || pos > ((int)(dym_arr -> size) - 1))
    {
        return 0; // wrong input given, pos is out of bounds
    }

    // if pos is negative, change the indexing to 0-indexing
    if (pos < 0)
    {
        pos = pos + (int)(dym_arr -> size);
    }

    dym_arr -> datapoint_destroyer(dym_arr -> data[pos]);
    dym_arr -> data[pos] = NULL;

    for (int i = pos + 1; i < dym_arr -> size; ++i)
    {
        dym_arr -> data[i - 1] = dym_arr -> data[i];
    }

    dym_arr -> data[dym_arr -> size - 1] = NULL;

    dym_arr -> size -= 1;

    if (dym_arr -> capacity > dym_arr -> minimum_capacity && dym_arr -> size < (unsigned)(dym_arr -> minimum_percentage_allowed * dym_arr -> capacity))
    {
        _dynamic_array_change_capacity(dym_arr, dym_arr -> capacity / 2U);
    }

    return 1;
}


/**
 * @brief Deletes all elements of the array
 */
void dynamic_array_clear(DynamicArray* dym_arr)
{
    for (int i = 0; i < dym_arr -> size; ++i)
    {
        dym_arr -> datapoint_destroyer(dym_arr -> data[i]);
        dym_arr -> data[i] = NULL;
    }

    dym_arr -> size = 0;

    if (dym_arr -> capacity > dym_arr -> minimum_capacity)
    {
        _dynamic_array_change_capacity(dym_arr, dym_arr -> minimum_capacity);
    }

    return;
}


/**
 * @brief Checks whether an element is inside the array.
 * @return int 0 if element is not found, int 1 if element is found
 */
int dynamic_array_find(DynamicArray* dym_arr, void* element)
{
    if (!dym_arr)
    {
        return 0;
    }

    for (int i = 0; i < dym_arr -> size; ++i)
    {
        if (dym_arr -> datapoint_equals(dym_arr -> data[i], element))
        {
            return 1;
        }
    }

    return 0;
}


/**
 * @brief Returns A COPY of the element in specified position
 * @param pos can be the 0-indexed position where the deletion needs to happen or it can be a negative integer number in [-size, -1] where size is the number of elements in the array, requesting the -n (n > 0) element of the array means that you want the nth element starting from the end of the array
 * @return void* reference to the element that sits in specified position, NULL if any error occurs like position out of bounds or NULL provided as the dym_arr argument
 */
void* dynamic_array_get(DynamicArray* dym_arr, int pos)
{
    if (!dym_arr)
    {
        return NULL;
    }

    // check if given pos is within acceptable values
    if (pos < -((int)(dym_arr -> size)) || pos > ((int)(dym_arr -> size) - 1))
    {
        return NULL; // wrong input given, pos is out of bounds
    }

    // if pos is negative, change the indexing to 0-indexing
    if (pos < 0)
    {
        pos = pos + (int)(dym_arr -> size);
    }

    return dym_arr -> datapoint_copy(dym_arr -> data[pos]);
}


/**
 * @brief Returns all elements of the underlying array in a built-in C array. All elements returned are COPIES of the elements of the dynamic array
 * @return void** array that holds copies of all the elements inside the dynamic array, NULL if dynamic array has no elements inside or error occurs (NULL dynamic array provided as argument)
 */
void** dynamic_array_get_all(DynamicArray* dym_arr)
{
    if (!dym_arr)
    {
        return NULL;
    }

    if (!(dym_arr -> size))
    {
        return NULL;
    }

    void** copy_array = (void**) calloc(dym_arr -> size, sizeof(void*));

    for (int i = 0; i < dym_arr -> size; ++i)
    {
        copy_array[i] = dym_arr -> datapoint_copy(dym_arr -> data[i]);
    }

    return copy_array;
}


/**
 * @brief Changes the capacity of the underlying array and copies all elements inside the array to the newly allocated array
 */
void _dynamic_array_change_capacity(DynamicArray* dym_arr, unsigned new_capacity)
{
    if (!dym_arr)
    {
        return;
    }

    if (dym_arr -> size > new_capacity)
    {
        return;
    }

    void** new_array = (void**) calloc(new_capacity, sizeof(void*));

    for (int i = 0; i < dym_arr -> size; ++i)
    {
        new_array[i] = dym_arr -> data[i];
    }

    free(dym_arr -> data);

    dym_arr -> data = new_array;
    dym_arr -> capacity = new_capacity;

    return;
}