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
    int current_id;
    long header_size;
    int result_bsc;
    long int offset;
    
    /* 2. Buscar en el índice en memoria */
    result_bsc = binary_search(ind_arr, book_id);

    if (result_bsc == NOT_FOUNDED || result_bsc == ERR)
    {
        fprintf(stdout, "Record with bookId=%d does not exist\n", book_id);
        return OK;
    }
    /* 3. Recuperar offset y tamaño del registro desde el índice */
    offset = ind_arr->array[result_bsc]->offset;
    rec_size = ind_arr->array[result_bsc]->size;

    /* Calculamos el tamaño de los datos de texto (Total - size_t - int) */
    /* Estructura en disco: [size_t][book_id][ISBN][Title][|][PrintedBy] */
    header_size = sizeof(size_t) + sizeof(int);
    data_len = rec_size - header_size;

    if (data_len > 0)
    {
        buffer = (char *)malloc(data_len + 1); /* +1 para seguridad del '\0' */
        if (buffer != NULL)
        {
            /* 4. Nos posicionamos en el archivo saltando size y book_id */
            fseek(pfile_db, offset + header_size, SEEK_SET);

            /* Leemos todos los datos de texto de una vez */
            if ((long)fread(buffer, 1, data_len, pfile_db) == data_len)
            {
                buffer[data_len] = '\0'; /* Aseguramos terminación nula */

                /* A. Extraer ISBN */
                strncpy(isbn_buff, buffer, ISBN);
                isbn_buff[ISBN] = '\0';

                /* B. Extraer Título (desde el fin del ISBN hasta el pipe '|') */
                ptr_title = buffer + ISBN;
                ptr_pipe = strchr(ptr_title, '|');

                if (ptr_pipe != NULL)
                {
                    /* Calculamos longitud del título */
                    int title_len = ptr_pipe - ptr_title;
                    if (title_len > MAX_STRING)
                        title_len = MAX_STRING;

                    strncpy(title_buff, ptr_title, title_len);
                    title_buff[title_len] = '\0';

                    /* C. Extraer PrintedBy (lo que queda después del pipe) */
                    strncpy(printed_buff, ptr_pipe + 1, MAX_STRING);
                    printed_buff[MAX_STRING] = '\0';
                }
                else
                {
                    /* Caso de error en formato de archivo */
                    strcpy(title_buff, ptr_title);
                    strcpy(printed_buff, "Unknown");
                }

                /* 5. Imprimir resultado con el formato solicitado */
                fprintf(stdout, "%d|%s|%s|%s\n", book_id, isbn_buff, title_buff, printed_buff);
            }
            free(buffer);
        }
        return OK;
    }

    return ERR;
}