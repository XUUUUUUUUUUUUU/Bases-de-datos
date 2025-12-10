/**
 * @brief It writes the principal binary_search function
 *
 * @file find.c
 * @author Alejandro Zheng
 * @version 1.0
 * @date 10-12-2025
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "find.h"
#include "types.h"

int binary_search(Array *arr, int book_id)
{
    int right, left, mid;
    if (arr == NULL || arr->array == NULL || book_id < 0)
        return ERR;

    left = 0;
    right = arr->used - 1;
    while (left <= right)
    {
        mid = (right + left) / 2;
        if (arr->array[mid]->key == book_id)
        {
            return mid;
        }
        else if (arr->array[mid]->key > book_id)
        {
            right = mid - 1;
        }
        else
        {
            left = mid + 1;
        }
    }
    return NOT_FOUNDED;
}