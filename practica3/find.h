/**
 * @brief It defines the principal binary_search function
 *
 * @file find.h
 * @author Alejandro Zheng
 * @version 1.0
 * @date 10-12-2025
 */

#ifndef FIND_H
#define FIND_H

#include "types.h"
/**
 * @brief binary_search print a book in the given file using binary mode
 * @author Shaofan Xu
 * @date 29/11/2025
 *
 * @param arr pointer to the Array which store all index : NOT NULL;
 * @param book_id  key we want to search
 *
 * @return Position if the key its founded; NOT_FOUNDED when there is no element match to the key; ERR in other case
 */
int binary_search(Array *arr, int book_id);

/**
 * @brief binary_search print a book in the given file using binary mode
 * @author Alejandro Zheng
 * @date 11/12/2025
 *
 * @param ind_arr pointer to the Array which store all index : NOT NULL;
 * @param book_id  key we want to search
 * @param pfile_db the file where we save the register
 *
 * @return Position if the key its founded; NOT_FOUNDED when there is no element match to the key; ERR in other case
 */
short find(Array *ind_arr, int book_id, FILE *pfile_db);

#endif