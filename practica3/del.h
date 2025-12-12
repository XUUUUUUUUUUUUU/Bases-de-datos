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
#include "types.h"

/**
 * @brief initDelArray creat an array of deleted books
 * @author Shaofan Xu
 * @date 30-11-2025
 *
 * @param arr pointer to array of deleted books
 * @param initialSize the initial size of array
 *
 * @return NONE
 */
void initDelArray(Del_Array *arr, size_t initialSize);

/**
 * @brief insertDelArray insert an index of deleted book in the deleted books array
 * @author Shaofan Xu
 * @date 30-11-2025
 *
 * @param arr pointer to array of deleted books
 * @param ind_del index of deleted book to insert
 * @param strategy used to order the array: BEST_FIT,WORST_FIT,FIRST_FIT
 *
 * @return NONE
 */
void insertDelArray(Del_Array *arr, Index_deleted_book *ind_del, int strategy);

/**
 * @brief freeDelArray free all memory reserved for array of deleted books
 * @author Shaofan Xu
 * @date 30-11-2025
 *
 * @param arr pointer to array of deleted books
 *
 * @return NONE
 */
void freeDelArray(Del_Array *arr);

/**
 * @brief index_del_to_file print deleted book's index register in the given file using binary mode
 * @author Shaofan Xu
 * @date 30/11/2025
 *
 * @param pfile_del pointer to the file to print: NOT NULL
 * @param ind_del_arr pointer to the arr of deleted book's index: NOT NULL
 * @param strategy strategy used to reuse the deleted book space
 *
 * @return OK if everythings is ok, and error in other case
 */
short index_del_to_file(FILE *pfile_del, Del_Array *ind_del_arr, int strategy);

/**
 * @brief reload_del_index reloads deleted book'index in the given file using binary mode
 * @author Shaofan Xu
 * @date 7/12/2025
 *
 * @param pfile pointer to the file to load index: NOT NULL
 * @param ind pointer to the arry of DelIndexbook to save index from file
 * @param strategy order of the array of deleted books
 *
 * @return OK if everythings is ok, and error in other case
 */
short reload_del_index(FILE *pfile, Del_Array *del_arr,int strategy);

/**
 * @brief Finds a hole based on strategy, updates del_arr, and returns the offset to use.
 * @date 30/11/2025
 * @author Shaofan Xu
 *
 * @param del_arr pointer to register of deleted books
 * @param required_size size need to write
 * @param strategy strategy to use to find the hole
 *
 * @return valid offset if found, -1 if no hole found.
 */
long find_and_use_hole(Del_Array *del_arr, size_t required_size, int strategy);

/**
 * @brief delete_book if remove a book from data base
 * @author Shaofan Xu
 * @date 12/12/2025
 *
 * @param ind_arr pointer to array of index
 * @param ind_del_arr pointer to array of deleted book
 * @param book_id the book_id of book to remove
 * @param strategy used to order the array: BEST_FIT,WORST_FIT,FIRST_FIT
 *
 * @return OK, if everthings Ok, and ERR in otherwise
 */
short delete_book(Array *ind_arr, Del_Array *ind_del_arr, int book_id, int strategy);
#endif