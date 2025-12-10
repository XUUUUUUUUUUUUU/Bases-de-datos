/**
 * @brief It defines all functions to delete a book
 *
 * @file del.h
 * @author Shaofan Xu
 * @version 1.0
 * @date 30-11-2025
 */
#ifndef DEL_H
#define DEL_H

#include <stdlib.h>

/**
 * @brief Index_deleted_book structure
 * This structure defines a index of deleted book with size of deleted book register size, offset.
 */

typedef struct
{
    size_t register_size;
    long int offset;
} Index_deleted_book;


/**
 * @brief Del_Array structure
 * This structure defines a Array of Index_deleted_book, size of array and used size.
 */
typedef struct _Del_Array Del_Array;


#endif