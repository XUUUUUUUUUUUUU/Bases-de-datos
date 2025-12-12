/**
 * @brief It implemets all functions to delete a book
 *
 * @file del.c
 * @author Shaofan Xu
 * @version 1.0
 * @date 30-11-2025
 */

#include <stdio.h>
#include "del.h"
#include "add.h"
#include "find.h"
#include "types.h"

void initDelArray(Del_Array *arr, size_t initialSize)
{
    if(arr==NULL)return;
    /* reserve memory for del arr*/
    arr->array = (Index_deleted_book **)malloc(initialSize * sizeof(Index_deleted_book *));
    if (arr->array == NULL)
    {
        return;
    }

    /* initialize the array */
    arr->used = 0;
    arr->size = initialSize;
}

void insertDelArray(Del_Array *arr, Index_deleted_book *ind_del, int strategy)
{
    int i;

    /* if the array is full, the realloc memory */
    if (arr->used == arr->size)
    {
        arr->size *= 2;
        arr->array = (Index_deleted_book **)realloc(arr->array, arr->size * sizeof(Index_deleted_book *));
    }

    /* order the array depent on the strategy */
    i = arr->used - 1;
    if (strategy == BESTFIT)
    {
        while (i >= 0 && arr->array[i]->register_size > ind_del->register_size)
        {
            arr->array[i + 1] = arr->array[i];
            i--;
        }
    }
    else if (strategy == WORSTFIT)
    {
        while (i >= 0 && arr->array[i]->register_size < ind_del->register_size)
        {
            arr->array[i + 1] = arr->array[i];
            i--;
        }
    }
    arr->array[i + 1] = ind_del;
    arr->used++;
}

void freeDelArray(Del_Array *arr)
{
    size_t i;

    /* free all memory */
    if (arr != NULL)
    {
        if (arr->array != NULL)
        {
            for (i = 0; i < arr->used; i++)
            {
                if (arr->array[i] != NULL)
                {
                    free(arr->array[i]);
                }
            }
            free(arr->array);
        }
    }
    arr->array = NULL;
    arr->used = arr->size = 0;
    free(arr);
}

short index_del_to_file(FILE *pfile_del, Del_Array *ind_del_arr, int strategy)
{
    Index_deleted_book *ind_del;
    size_t i;
    if (pfile_del == NULL || ind_del_arr == NULL)
    {
        return ERR;
    }

    /* write the strategy */
    if (fwrite(&(strategy), sizeof(strategy), 1, pfile_del) != 1)
        return ERR;

    /* run all array, and print all offset and size */
    for (i = 0; i < ind_del_arr->used; i++)
    {
        ind_del = ind_del_arr->array[i];
        if (fwrite(&(ind_del->offset), sizeof(ind_del->offset), 1, pfile_del) != 1)
            return ERR;
        if (fwrite(&(ind_del->register_size), sizeof(ind_del->register_size), 1, pfile_del) != 1)
            return ERR;
    }
    return OK;
}

short reload_del_index(FILE *pfile, Del_Array *del_arr)
{
    Index_deleted_book *del_temp;
    int strategy;

    /*the file or index structure is NULL*/
    if (pfile == NULL || del_arr == NULL)
    {
        return ERR;
    }

    /* read the strategy used*/
    if (fread(&(strategy), sizeof(strategy), 1, pfile) != 1)
    {
        /* nothing in the file */
        return OK;
    }
    /*process of loading index*/

    while (1)
    {
        /*Allocate memory for index*/
        del_temp = malloc(sizeof(Index_deleted_book));
        if (del_temp == NULL)
        {
            return ERR;
        }

        /*loading data*/
        if (fread(&(del_temp->offset), sizeof(del_temp->offset), 1, pfile) != 1)
        {
            free(del_temp);
            /*the loop end when the file have not index to load*/
            if (feof(pfile))
            {
                return OK;
            }
            else
            {
                return ERR;
            }
        }

        if (fread(&(del_temp->register_size), sizeof(del_temp->register_size), 1, pfile) != 1)
        {
            free(del_temp);
            return ERR;
        }

        /*Add index to the array of Indexbook*/
        insertDelArray(del_arr, del_temp, strategy);
    }

    return OK;
}

long find_and_use_hole(Del_Array *del_arr, size_t required_size, int strategy)
{
    int i, k, best_index = -1;
    size_t hole_size;
    size_t remaining;
    long use_offset;
    Index_deleted_book *hole, *new_hole;

    if (del_arr == NULL || del_arr->used == 0)
        return NOT_FOUNDED;

    /* 1. Find the index based on strategy */
    if (strategy == WORSTFIT)
    {
        if (del_arr->array[0]->register_size >= required_size)
        {
            best_index = 0;
        }
    }
    else if (strategy == BESTFIT || strategy == FIRSTFIT)
    {
        for (i = 0; (size_t)i < del_arr->used; i++)
        {
            if (del_arr->array[i]->register_size >= required_size)
            {
                best_index = i;
                break;
            }
        }
    }
    if (best_index == -1)
        return NOT_FOUNDED; /*No suitable hole found*/

    /* 2. Extract info */
    hole = del_arr->array[best_index];
    use_offset = hole->offset;
    hole_size = hole->register_size;

    /* 3. Handle remaining space (Fragmentation) */
    remaining = hole_size - required_size;

    /* Remove the used hole from the array (Shift left) */
    free(hole);
    for (k = best_index; (size_t)k < del_arr->used - 1; k++)
    {
        del_arr->array[k] = del_arr->array[k + 1];
    }
    del_arr->used--;

    /* If there is remaining space, add it back as a new hole */
    if (remaining > 0)
    {
        new_hole = malloc(sizeof(Index_deleted_book));
        if (new_hole == NULL)
        {
            return ERR;
        }
        new_hole->offset = use_offset + required_size;
        new_hole->register_size = remaining;
        insertDelArray(del_arr, new_hole, strategy);
    }

    return use_offset;
}

short delete_book(Array *ind_arr, Del_Array *ind_del_arr, int book_id, int strategy)
{
    int result_bsc;
    Index_deleted_book *ind_del=NULL; 
    /* Check if the book_id is in the ind or not*/
    result_bsc = binary_search(ind_arr, book_id);
    if (result_bsc == ERR)
    {
        return ERR;
    }
    else if (result_bsc == NOT_FOUNDED)
    {
        fprintf(stdout, "Record with bookId=%d does not exist\n", book_id);
    }
    else
    {
        ind_del=malloc(sizeof(Index_deleted_book));
        if(ind_del==NULL)
        {
            return ERR;
        }
        ind_del->register_size = ind_arr->array[result_bsc]->size;
        ind_del->offset = ind_arr->array[result_bsc]->offset;
        insertDelArray(ind_del_arr, ind_del, strategy);
        remove_from_index(ind_arr, result_bsc);
        fprintf(stdout, "Record with BookID=%d has been deleted\n", book_id);
    }
    return OK;
}