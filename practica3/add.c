/**
 * @brief It implements all functions to add a book
 *
 * @file add.c
 * @author Shaofan Xu
 * @version 1.0
 * @date 15-11-2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "add.h"
#include "find.h"
#include "del.h"
#include "types.h"

void initArray(Array *arr, size_t initialSize)
{
    if (arr == NULL)
        return;
    /* reserve memory for array */
    arr->array = (Indexbook **)malloc(initialSize * sizeof(Indexbook *));
    if (arr->array == NULL)
    {
        return;
    }
    /* initialize the array */
    arr->used = 0;
    arr->size = initialSize;
}

short insertArray(Array *arr, Indexbook *ind)
{
    int i;
    /* if the array is full, then reserve more memory */
    if (arr->used == arr->size)
    {
        arr->size *= 2;
        arr->array = (Indexbook **)realloc(arr->array, arr->size * sizeof(Indexbook *));
    }
    /* order the array by book_id ASC*/
    i = arr->used - 1;
    while (i >= 0 && arr->array[i]->key > ind->key)
    {
        arr->array[i + 1] = arr->array[i];
        i--;
    }

    arr->array[i + 1] = ind;
    arr->used++;
    return OK;
}

void freeArray(Array *arr)
{
    size_t i;
    /* free all allocated memory */
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

short book_to_file(FILE *pfile, Record *registro)
{
    if (pfile == NULL || registro == NULL)
    {
        return ERR;
    }

    /* size of all cadena */
    if (fwrite(&registro->size, sizeof(size_t), 1, pfile) != 1)
        return ERR;

    /* write book_id */
    if (fwrite(&registro->book_id, sizeof(registro->book_id), 1, pfile) != 1)
        return ERR;

    /* write isbn*/
    if (fwrite(registro->isbn, ISBN, 1, pfile) != 1)
        return ERR;

    /* write title */
    if (fwrite(registro->title, strlen(registro->title), 1, pfile) != 1)
        return ERR;
    if (fputc('|', pfile) == EOF)
        return ERR;

    /* write printedBy*/
    if (fwrite(registro->printedBy, strlen(registro->printedBy), 1, pfile) != 1)
        return ERR;

    return OK;
}

short index_to_file(FILE *pfile, Array *ind_arr)
{
    Indexbook *ind;
    size_t i;
    if (pfile == NULL || ind_arr == NULL || ind_arr->array == NULL)
    {
        return ERR;
    }

    /* print all register of index in the file */
    for (i = 0; i < ind_arr->used; i++)
    {
        ind = ind_arr->array[i];
        if (fwrite(&(ind->key), sizeof(ind->key), 1, pfile) != 1)
            return ERR;
        if (fwrite(&(ind->offset), sizeof(ind->offset), 1, pfile) != 1)
            return ERR;
        if (fwrite(&(ind->size), sizeof(ind->size), 1, pfile) != 1)
            return ERR;
    }
    return OK;
}

short reload_index(FILE *pfile, Array *ind_arr)
{
    Indexbook *ind_temp;

    /*the file or index structure is NULL*/
    if (pfile == NULL || ind_arr == NULL)
    {
        return ERR;
    }

    /*process of loading index*/
    while (1)
    {
        /*Allocate memory for index*/
        ind_temp = malloc(sizeof(Indexbook));
        if (ind_temp == NULL)
        {
            return ERR;
        }

        /*loading data*/
        if (fread(&(ind_temp->key), sizeof(ind_temp->key), 1, pfile) != 1)
        {
            free(ind_temp);
            /*the loop end when the file have not index to load*/
            break;
        }

        if (fread(&(ind_temp->offset), sizeof(ind_temp->offset), 1, pfile) != 1)
        {
            free(ind_temp);
            return ERR;
        }

        if (fread(&(ind_temp->size), sizeof(ind_temp->size), 1, pfile) != 1)
        {
            free(ind_temp);
            return ERR;
        }

        /*Add index to the array of Indexbook*/
        insertArray(ind_arr, ind_temp);
    }

    return OK;
}

void remove_from_index(Array *arr, int pos)
{
    size_t i;

    if (arr == NULL || arr->array == NULL || pos < 0)
        return;

    free(arr->array[pos]);

    /* remove the index from the array */
    for (i = pos; i < arr->used - 1; i++)
    {
        arr->array[i] = arr->array[i + 1];
    }
    arr->used--;
}

short add_book(Array *ind_arr, Del_Array *ind_del_arr, FILE *pfile_db, Record *registro, int strategy)
{
    Indexbook *ind = NULL;
    long hole_offset;
    long offset;

    /* check if the book is already in the data base */
    if (ind_arr->used > 0)
    {
        if (binary_search(ind_arr, registro->book_id) >= 0)
        {
            fprintf(stdout, "Record with Book_ID==%d exists\n", registro->book_id);
            return OK;
        }
    }

    ind = malloc(sizeof(Indexbook));
    if (ind == NULL)
    {
        return ERR;
    }

    /* record size */
    registro->size = sizeof(registro->book_id) + ISBN + strlen(registro->title) + strlen(registro->printedBy) + sizeof(char);

    /* check deleted book list if there is available space */
    hole_offset = find_and_use_hole(ind_del_arr, registro->size, strategy);

    if (hole_offset >= 0)
    {
        fseek(pfile_db, hole_offset, SEEK_SET);
    }
    else if (hole_offset == NOT_FOUNDED)
    {
        fseek(pfile_db, 0, SEEK_END);
    }
    else
    {

        free(ind);
        return ERR;
    }

    offset = ftell(pfile_db);

    ind->key = registro->book_id;
    ind->offset = offset;
    ind->size = registro->size;

    /* print in the file */
    if (book_to_file(pfile_db, registro) == ERR)
    {
        free(ind);
        return ERR;
    }

    /* insert in the array */
    if (insertArray(ind_arr, ind) == ERR)
    {
        free(ind);
        return ERR;
    }

    fprintf(stdout, "Record with BookID=%d has been added to the database\n", registro->book_id);
    return OK;
}