/**
 * @brief It implements the main function of program
 *
 * @file library.c
 * @author Shaofan Xu y Alejandro Zheng
 * @version 1.0
 * @date 10-12-2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "find.h"
#include "types.h"
#include "del.h"
#include "add.h"

/* auxiliar functions */

/**
 * @brief freeAllMemory free all memory need in the function main
 * @author Shaofan Xu
 * @date 12/12/2025
 *
 * @param pfile1 pointer to the file to close
 * @param pfile2 pointer to the file to close
 * @param pfile3 pointer to the file to close
 * @param registro pointer to the record to free
 * @param ind_arr pointer to the array of index to free
 * @param ind_del arr pointer to the array of deleted book's index to free
 *
 * @return NONE
 */
void freeAllMemory(FILE *pfile1, FILE *pfile2, FILE *pfile3, Record *registro, Array *ind_arr, Del_Array *ind_del_arr)
{
    if (pfile1 != NULL)
        fclose(pfile1);
    if (pfile2 != NULL)
        fclose(pfile2);
    if (pfile3 != NULL)
        fclose(pfile3);
    if (registro != NULL)
        free(registro);
    if (ind_arr != NULL)
        freeArray(ind_arr);
    if (ind_del_arr != NULL)
        freeDelArray(ind_del_arr);
}

int main(int argc, char *argv[])
{
    FILE *pfile_db = NULL, *pfile_del = NULL, *pfile_ind = NULL;
    Record *registro;
    Array *ind_arr = NULL;
    Del_Array *ind_del_arr = NULL;
    int book_id;
    long int offset;
    char command[MAX_COMMAND + 1];
    char *token;
    int strategy;                          /* strategy to reuse the space of deleted book */
    char filename[MAX_STRING + 1];         /* filename of book's information to stored */
    char db_filename[MAX_STRING + 4];      /* filename of book's information to stored end with .db*/
    char ind_filename[MAX_STRING + 5];     /* filename of index information to stored end with .ind */
    char ind_del_filename[MAX_STRING + 5]; /* filename of deleted index information to stored end with .lst */
    size_t i;
    char *buffer;
    char isbn_buff[ISBN + 1];
    char title_buff[MAX_STRING + 1];
    char printed_buff[MAX_STRING + 1];
    char *ptr_title;
    char *ptr_pipe;
    long data_len;
    size_t rec_size;
    int current_id;
    int k;
    long header_skip;
    char *ptr_editorial;
    long editorial_len;

    if (argc < 3)
    {
        fprintf(stdout, "Missing argument\n");
        return 0;
    }

    if (strcmp(argv[1], "first_fit") == 0)
    {
        strategy = FIRSTFIT;
    }
    else if (strcmp(argv[1], "worst_fit") == 0)
    {
        strategy = WORSTFIT;
    }
    else if (strcmp(argv[1], "best_fit") == 0)
    {
        strategy = BESTFIT;
    }
    else
    {
        fprintf(stdout, "Unknown search strategy unknown_search_strategy\n");
        return 0;
    }

    strcpy(filename, argv[2]);
    sprintf(db_filename, "%s.db", filename);
    sprintf(ind_filename, "%s.ind", filename);
    sprintf(ind_del_filename, "%s.lst", filename);

    /*Iniciate the arry of indexbook*/
    ind_arr = malloc(sizeof(Array));
    if (ind_arr == NULL)
    {
        return ERR;
    }
    ind_del_arr = malloc(sizeof(Del_Array));
    if (ind_del_arr == NULL)
    {
        freeAllMemory(NULL, NULL, NULL, NULL, ind_arr, NULL);
        return ERR;
    }
    initArray(ind_arr, 5);
    initDelArray(ind_del_arr, 5);
    if (ind_arr->array == NULL || ind_del_arr->array == NULL)
    {
        freeAllMemory(NULL, NULL, NULL, NULL, ind_arr, ind_del_arr);
        return ERR;
    }

    offset = 0;

    /*Check if exists file with index information and store them before run programm*/
    pfile_ind = fopen(ind_filename, "rb");
    if (pfile_ind != NULL)
    {
        /*reloading the index*/
        if (reload_index(pfile_ind, ind_arr) == ERR)
        {
            freeAllMemory(pfile_ind, NULL, NULL, NULL, ind_arr, ind_del_arr);
            return ERR;
        }
        fclose(pfile_ind);
        pfile_ind = NULL;
    }

    /*Check if exists file with deleted index information and store them before run programm*/
    pfile_del = fopen(ind_del_filename, "rb");
    if (pfile_del != NULL)
    {
        /*reloading the index*/
        if (reload_del_index(pfile_del, ind_del_arr) == ERR)
        {
            printf("problem\n");
            freeAllMemory(pfile_ind, pfile_del, NULL, NULL, ind_arr, ind_del_arr);
            return ERR;
        }

        /*close the index file*/
        fclose(pfile_del);
        pfile_del = NULL;
    }
    fprintf(stdout, "Type command and arguments/s.\n");
    fprintf(stdout, "exit\n");

    /* Open the file that we are going to store book information*/
    pfile_db = fopen(db_filename, "w+b");
    if (pfile_db == NULL)
    {
        freeAllMemory(pfile_ind, pfile_del, NULL, NULL, ind_arr, ind_del_arr);
        return ERR;
    }

    /* Open the file that we are going to store deleted_index information */
    registro = malloc(sizeof(Record));
    if (registro == NULL)
    {
        freeAllMemory(pfile_ind, pfile_del, pfile_db, NULL, ind_arr, ind_del_arr);
        return ERR;
    }

    while (fgets(command, MAX_COMMAND, stdin) && (strcmp(command, "exit\n") != 0))
    {
        command[strcspn(command, "\r\n")] = 0;
        token = strtok(command, " ");
        if (strcmp(token, "add") == 0)
        {
            /* read the book_id */
            token = strtok(NULL, "|");
            if (token == NULL)
                continue;
            registro->book_id = atoi(token);

            /* read the isbn */
            token = strtok(NULL, "|");
            if (token == NULL)
                continue;
            strncpy(registro->isbn, token, ISBN);
            registro->isbn[ISBN] = '\0';

            /* read title */
            token = strtok(NULL, "|");
            if (token == NULL)
                continue;
            strncpy(registro->title, token, MAX_STRING);
            registro->title[MAX_STRING] = '\0';

            /* read printedBy. */
            token = strtok(NULL, "\r\n");
            if (token == NULL)
                continue;
            strncpy(registro->printedBy, token, MAX_STRING);
            registro->printedBy[MAX_STRING] = '\0';

            if (add_book(ind_arr, ind_del_arr, pfile_db, registro, strategy) == ERR)
            {
                freeAllMemory(pfile_ind, pfile_del, pfile_db, registro, ind_arr, ind_del_arr);

                return ERR;
            }
            fprintf(stdout, "exit\n");
        }
        else if (strcmp(token, "printInd") == 0)
        {
            for (i = 0; i < ind_arr->used; i++)
            {
                fprintf(stdout, "Entry #%ld\n", i);
                fprintf(stdout, "    key: #%d\n", ind_arr->array[i]->key);
                fprintf(stdout, "    offset: #%ld\n", ind_arr->array[i]->offset);
                fprintf(stdout, "    size: #%ld\n", ind_arr->array[i]->size);
            }
            fprintf(stdout, "exit\n");
        }
        else if (strcmp(token, "del") == 0)
        {
            /* read the book_id to delete*/
            token = strtok(NULL, "\r\n");
            book_id = atoi(token);

            if (delete_book(ind_arr, ind_del_arr, book_id, strategy) == ERR)
            {
                printf("2");
                freeAllMemory(pfile_ind, pfile_del, pfile_db, registro, ind_arr, ind_del_arr);
                return ERR;
            }
        }
        else if (strcmp(token, "printLst") == 0)
        {
            for (i = 0; i < ind_del_arr->used; i++)
            {
                fprintf(stdout, "Entry #%ld\n", i);
                fprintf(stdout, "    offset: #%ld\n", ind_del_arr->array[i]->offset);
                fprintf(stdout, "    size: #%ld\n", ind_del_arr->array[i]->register_size);
            }
            fprintf(stdout, "exit\n");
        }
        else if (strcmp(token, "find") == 0)
        {
            /* 1. Obtener el book_id del comando */
            token = strtok(NULL, "\r\n");
            if (token == NULL)
                continue;
            book_id = atoi(token);

            if (find(ind_arr, book_id, pfile_db) == ERR)
            {
                freeAllMemory(pfile_ind, pfile_del, pfile_db, registro, ind_arr, ind_del_arr);
                return ERR;
            }
            fflush(stdout);           
            fprintf(stdout, "exit\n"); 
        }
       else if (strcmp(token, "printRec") == 0)
        {
            
            for (i = 0; i < ind_arr->used; i++)
            {
                offset = ind_arr->array[i]->offset;
                rec_size = ind_arr->array[i]->size;
                current_id = ind_arr->array[i]->key;

                /* --- CORRECCIÓN AQUÍ --- */
                /* data_len es el tamaño del registro SIN el ID */
                data_len = rec_size - sizeof(int);
                /* header_skip es size_t + int */
                header_skip = sizeof(size_t) + sizeof(int);

                if (data_len > 0)
                {
                    buffer = (char *)malloc(data_len + 1);
                    if (buffer != NULL)
                    {
                        /* Usamos header_skip para movernos */
                        fseek(pfile_db, offset + header_skip, SEEK_SET);

                        if ((long)fread(buffer, 1, data_len, pfile_db) == data_len)
                        {
                            
                            buffer[data_len] = '\0'; 
                            /* ... ISBN ... */
                            memcpy(isbn_buff, buffer, ISBN);
                            isbn_buff[ISBN] = '\0';
                            
                            /* ... Busqueda del Pipe ... */
                            ptr_title = buffer + ISBN;
                            ptr_pipe = NULL;
                            for (k = 0; k < (data_len - ISBN); k++) {
                                if (ptr_title[k] == '|') { ptr_pipe = ptr_title + k; break; }
                            }
                            
                             if (ptr_pipe != NULL) {
                                long title_len = ptr_pipe - ptr_title;
                                if (title_len > MAX_STRING) title_len = MAX_STRING;
                                memcpy(title_buff, ptr_title, title_len);
                                title_buff[title_len] = '\0';

                                ptr_editorial = ptr_pipe + 1;
                                editorial_len = (buffer + data_len) - ptr_editorial;
                                if (editorial_len > MAX_STRING) editorial_len = MAX_STRING;
                                if (editorial_len < 0) editorial_len = 0;
                                memcpy(printed_buff, ptr_editorial, editorial_len);
                                printed_buff[editorial_len] = '\0';
                            } else {
                                strncpy(title_buff, ptr_title, MAX_STRING);
                                title_buff[MAX_STRING] = '\0';
                                strcpy(printed_buff, "N/A");
                            }

                            fprintf(stdout, "%d|%s|%s|%s\n", current_id, isbn_buff, title_buff, printed_buff);
                        }
                        free(buffer);
                    }
                }
            }
            fflush(stdout);
            fprintf(stdout, "exit\n");
        }
    }

    /* Start print in the index file*/
    fclose(pfile_db);
    pfile_db = NULL;

    pfile_ind = fopen(ind_filename, "wb");
    if (pfile_ind == NULL)
    {
        printf("hola");
        freeAllMemory(pfile_ind, pfile_del, pfile_db, registro, ind_arr, ind_del_arr);
        return ERR;
    }

    if (index_to_file(pfile_ind, ind_arr) == ERR)
    {

        freeAllMemory(pfile_ind, pfile_del, pfile_db, registro, ind_arr, ind_del_arr);
        return ERR;
    }
    fclose(pfile_ind);
    pfile_ind = NULL;
    /* finished */
    fprintf(stdout, "all done\n");

    pfile_del = fopen(ind_del_filename, "wb");
    if (pfile_del == NULL)
    {
        freeAllMemory(pfile_ind, pfile_del, pfile_db, registro, ind_arr, ind_del_arr);
        return ERR;
    }
    if (index_del_to_file(pfile_del, ind_del_arr, strategy) == ERR)
    {
        freeAllMemory(pfile_ind, pfile_del, pfile_db, registro, ind_arr, ind_del_arr);
        return ERR;
    }
    fclose(pfile_del);
    pfile_del = NULL;
    freeAllMemory(pfile_ind, pfile_del, pfile_db, registro, ind_arr, ind_del_arr);
    return 0;
}
