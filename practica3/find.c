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

short find(Array *ind_arr, int book_id, FILE *pfile_db)
{
    char *buffer;
    char isbn_buff[ISBN + 1];
    char title_buff[MAX_STRING + 1];
    char printed_buff[MAX_STRING + 1];
    char *ptr_title; 
    char *ptr_pipe;
    long data_len;
    size_t rec_size;
    long header_skip; 
    int result_bsc;
    long int offset;
    int k;
    char *ptr_editorial;
    long editorial_len;
    
    result_bsc = binary_search(ind_arr, book_id);

    if (result_bsc == NOT_FOUNDED || result_bsc == ERR)
    {
        fprintf(stdout, "Record with bookId=%d does not exist\n", book_id);
        fflush(stdout);
        return OK;
    }

    offset = ind_arr->array[result_bsc]->offset;
    rec_size = ind_arr->array[result_bsc]->size;

    /* CORRECCIÓN MATEMÁTICA CRÍTICA */
    /* 1. Cuánto saltamos en el archivo: El size_t (cabecera) + el int (ID) */
    header_skip = sizeof(size_t) + sizeof(int);

    /* 2. Cuánto leemos: El tamaño total del registro MENOS el ID (que ya saltamos) */
    /* NO restamos sizeof(size_t) porque rec_size no lo incluía */
    data_len = rec_size - sizeof(int);

    if (data_len > 0)
    {
        buffer = (char *)malloc(data_len + 1); 
        if (buffer != NULL)
        {
            /* Saltamos size_t e ID */
            fseek(pfile_db, offset + header_skip, SEEK_SET);

            if ((long)fread(buffer, 1, data_len, pfile_db) == data_len)
            {
                buffer[data_len] = '\0'; 

                /* A. ISBN */
                memcpy(isbn_buff, buffer, ISBN);
                isbn_buff[ISBN] = '\0';

                /* B. Buscar Pipe '|' */
                ptr_title = buffer + ISBN;
                ptr_pipe = NULL;
                
                for (k = 0; k < (data_len - ISBN); k++) {
                    if (ptr_title[k] == '|') {
                        ptr_pipe = ptr_title + k;
                        break;
                    }
                }

                if (ptr_pipe != NULL)
                {
                    /* C. Título */
                    long title_len = ptr_pipe - ptr_title;
                    if (title_len > MAX_STRING) title_len = MAX_STRING;
                    
                    memcpy(title_buff, ptr_title, title_len);
                    title_buff[title_len] = '\0';

                    /* D. Editorial */
                    ptr_editorial = ptr_pipe + 1;
                    /* Calculamos longitud restante hasta el final del buffer leído */
                    editorial_len = (buffer + data_len) - ptr_editorial;
                    
                    if (editorial_len > MAX_STRING) editorial_len = MAX_STRING;
                    if (editorial_len < 0) editorial_len = 0;

                    memcpy(printed_buff, ptr_editorial, editorial_len);
                    printed_buff[editorial_len] = '\0';
                }
                else
                {
                    strncpy(title_buff, ptr_title, MAX_STRING);
                    title_buff[MAX_STRING] = '\0';
                    strcpy(printed_buff, "Unknown");
                }

                fprintf(stdout, "%d|%s|%s|%s\n", book_id, isbn_buff, title_buff, printed_buff);
                fflush(stdout); 
            }
            free(buffer);
        }
        else return ERR;

        return OK;
    }
    return ERR;
}