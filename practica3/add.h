/**
 * @brief It defines all functions to add a book
 *
 * @file add.h
 * @author Shaofan Xu
 * @version 1.0
 * @date 15-11-2025
 */
#ifndef ADD_H
#define ADD_H

#include <stdlib.h>
#include "types.h"

/**
 * @brief Record Structure
 * This structure store all information of a book: book_id, isbn , title, printedBy and size
 */
typedef struct
{
    int book_id;                    /* book_id primary key of a book */
    char isbn[ISBN + 1];            /* isbn code of book */
    char title[MAX_STRING + 1];     /* the title of book */
    char printedBy[MAX_STRING + 1]; /* the publisher of the book */
    size_t size;                    /* the size of record */

} Record;

/**
 * @brief initArray create the structure of index array
 * @author Shaofan Xu
 * @date 15/11/2025
 *
 * @param arr pointer to the array of indexbook
 * @param initialSize the initial size of the array
 *
 * @return NONE
 */
void initArray(Array *arr, size_t initialSize);

/**
 * @brief insertArray insert an index in the array of index
 * @author Shaofan Xu
 * @date 15/11/2025
 *
 * @param arr pointer to the array of index
 * @param ind pointer to the index to insert
 *
 * @return Ok if everythings ok, and ERR in otherwise
 */
short insertArray(Array *arr, Indexbook *ind);

/**
 * @brief freeArray free all allocated memory for the array of index
 * @author Shaofan Xu
 * @date 15/11/2025
 *
 * @param arr pointer to the array of index to free
 *
 * @return NONE
 */
void freeArray(Array *arr);

/**
 * @brief book_to_file print a book in the given file using binary mode
 * @author Shaofan Xu
 * @date 15/11/2025
 *
 * @param pfile pointer to the file to print: NOT NULL;
 * @param registro pointer to the register of book: NOT NULL
 *
 * @return OK if everythings is ok, and error in other case
 */
short book_to_file(FILE *pfile, Record *registro);

/**
 * @brief index_to_file print book'index in the given file using binary mode
 * @author Shaofan Xu
 * @date 23/11/2025
 *
 * @param pfile pointer to the file to print: NOT NULL
 * @param ind_arr pointer to the arr of index of book: NOT NULL
 *
 * @return OK if everythings is ok, and error in other case
 */
short index_to_file(FILE *pfile, Array *ind_arr);

/**
 * @brief reload_index reloads book'index in the given file using binary mode
 * @author Alejandro Zheng
 * @date 24/11/2025
 *
 * @param pfile pointer to the file to load index: NOT NULL
 * @param ind pointer to the arry of Indexbook to save index from file
 *
 * @return OK if everythings is ok, and error in other case
 */
short reload_index(FILE *pfile, Array *ind_arr);

/**
 * @brief remove_from_index delete an index from the index array
 * @date 30/11/2025
 * @author Shaofan Xu
 *
 * @param arr pointer to the array of Indexbook: NOT NULL
 * @param pos position of the Indexbook to delete in the array: NOT NEGATIVE
 *
 * @return NONE
 */
void remove_from_index(Array *arr, int pos);

/**
 * @brief add_book add a book to data base
 * @author Shaofan Xu
 * @date 12/12/2025
 *
 * @param ind_arr pointer to array of index
 * @param ind_del_arr pointer to array of deleted book
 * @param pfile_db pointer to file to print the data
 * @param registro pointer to record of book to print
 * @param strategy used to order the array: BEST_FIT,WORST_FIT,FIRST_FIT
 *
 * @return OK if everythings is ok, and error in other case
 */
short add_book(Array *ind_arr, Del_Array *ind_del_arr, FILE *pfile_db, Record *registro, int strategy);
#endif