/**
 * @brief It defines all macro magnitud
 *
 * @file types.h
 * @author Alejandro Zheng
 * @version 1.0
 * @date 10-12-2025
 */

#ifndef TYPES_H
#define TYPES_H

#define ISBN 16          /* Max length of ISBN code of book */
#define MAX_STRING 128   /* Max length of strings */
#define MAX_COMMAND 1024 /* Max length of commands */
#define OK 0             /* Ok code, if all things is ok */
#define ERR -1           /* Error code */
#define NOT_FOUNDED -2   /* No se ha encontrado key en busqueda binaria */
#define BESTFIT 0        /* Used strategy to order the array of deleted books */
#define WORSTFIT 1       /* Used strategy to order the array of deleted books */
#define FIRSTFIT 2       /* Used strategy to order the array of deleted books */

/**
 * @brief Indexbook Structure
 * This structure store the index of book: key, offset and size
 */
typedef struct
{
    int key;         /* book_id */
    long int offset; /* offset of book in the file */
    size_t size;     /* size of book */
} Indexbook;

typedef struct
{
    Indexbook **array; /* the array of index  */
    size_t used;       /* used size of index array */
    size_t size;       /* total size of index array */
} Array;

/**
 * @brief Index_deleted_book structure
 * This structure defines a index of deleted book with size of deleted book register size, offset.
 */

typedef struct
{
    size_t register_size; /* size of register of deleted book*/
    long int offset;      /* offset of deleted book */
} Index_deleted_book;

/**
 * @brief Del_Array structure
 * This structure defines a Array of Index_deleted_book, size of array and used size.
 */
typedef struct
{
    Index_deleted_book **array; /* array of deleted books */
    size_t used;                /* used size of deleted array */
    size_t size;                /* total size of deleted array */
} Del_Array;
#endif