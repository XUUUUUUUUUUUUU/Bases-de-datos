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

typedef struct
{
    int book_id;
    char isbn[ISBN + 1];
    char title[MAX_STRING + 1];
    char printedBy[MAX_STRING + 1];
    size_t size;

} Record;

typedef struct
{
    int key;
    long int offset;
    size_t size;
} Indexbook;

typedef struct
{
    size_t register_size;
    long int offset;
} Index_deleted_book;

typedef struct
{
    Indexbook **array;
    size_t used;
    size_t size;
} Array;

typedef struct
{
    Index_deleted_book **array;
    size_t used;
    size_t size;
} Del_Array;

/* auxiliar functions */

void initArray(Array *a, size_t initialSize)
{
    a->array = (Indexbook **)malloc(initialSize * sizeof(Indexbook *));
    a->used = 0;
    a->size = initialSize;
}

short insertArray(Array *a, Indexbook *ind)
{
    int i;
    if (a->used == a->size)
    {
        a->size *= 2;
        a->array = (Indexbook **)realloc(a->array, a->size * sizeof(Indexbook *));
    }
    i = a->used - 1;
    while (i >= 0 && a->array[i]->key > ind->key)
    {
        a->array[i + 1] = a->array[i];
        i--;
    }

    a->array[i + 1] = ind;
    a->used++;
    return OK;
}

void initDelArray(Del_Array *a, size_t initialSize)
{
    a->array = (Index_deleted_book **)malloc(initialSize * sizeof(Index_deleted_book *));
    a->used = 0;
    a->size = initialSize;
}

void insertDelArray(Del_Array *a, Index_deleted_book *ind_del, int strategy)
{
    int i;
    if (a->used == a->size)
    {
        a->size *= 2;
        a->array = (Index_deleted_book **)realloc(a->array, a->size * sizeof(Index_deleted_book *));
    }
    i = a->used - 1;
    if (strategy == BESTFIT)
    {
        while (i >= 0 && a->array[i]->register_size > ind_del->register_size)
        {
            a->array[i + 1] = a->array[i];
            i--;
        }
    }
    else if (strategy == WORSTFIT)
    {
        while (i >= 0 && a->array[i]->register_size < ind_del->register_size)
        {
            a->array[i + 1] = a->array[i];
            i--;
        }
    }
    a->array[i + 1] = ind_del;
    a->used++;
}

void freeArray(Array *a)
{
    size_t i;
    if (a != NULL)
    {
        if (a->array != NULL)
        {
            for (i = 0; i < a->used; i++)
            {
                if (a->array[i] != NULL)
                {
                    free(a->array[i]);
                }
            }
            free(a->array);
        }
    }
    a->array = NULL;
    a->used = a->size = 0;
    free(a);
}

void freeDelArray(Del_Array *a)
{
    size_t i;
    if (a != NULL)
    {
        if (a->array != NULL)
        {
            for (i = 0; i < a->used; i++)
            {
                if (a->array[i] != NULL)
                {
                    free(a->array[i]);
                }
            }
            free(a->array);
        }
    }
    a->array = NULL;
    a->used = a->size = 0;
    free(a);
}

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
short index_to_file(FILE *pfile, Array *ind_arr)
{
    Indexbook *ind;
    size_t i;
    if (pfile == NULL || ind_arr == NULL)
    {
        return ERR;
    }
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
short index_del_to_file(FILE *pfile_del, Del_Array *ind_del_arr, int strategy)
{
    Index_deleted_book *ind_del;
    size_t i;
    if (pfile_del == NULL || ind_del_arr == NULL)
    {
        return ERR;
    }
    if (fwrite(&(strategy), sizeof(strategy), 1, pfile_del) != 1)
        return ERR;
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

/**
 * @brief remove_from_index delete an index from the index array
 * @date 30/11/2025
 * @author Shaofan Xu
 *
 * @param arr pointer to the array of Indexbook: NOT NULL
 * @param pos position of the Indexbook to delete in the array: NOT NEGATIVE
 *
 * @return NULL
 */
void remove_from_index(Array *arr, int pos)
{
    size_t i;

    if (arr == NULL || arr->array == NULL || pos < 0)
        return;

    free(arr->array[pos]);

    for (i = pos; i < arr->used - 1; i++)
    {
        arr->array[i] = arr->array[i + 1];
    }
    arr->used--;
}

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
long find_and_use_hole(Del_Array *del_arr, size_t required_size, int strategy)
{
    int i, k, best_index = -1;
    size_t hole_size;
    size_t remaining;
    long use_offset;
    Index_deleted_book *hole, *new_hole;

    if (del_arr == NULL || del_arr->used == 0)
        return ERR;

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

/**
 * @brief reload_del_index reloads deleted book'index in the given file using binary mode
 * @author Shaofan Xu
 * @date 7/12/2025
 *
 * @param pfile pointer to the file to load index: NOT NULL
 * @param ind pointer to the arry of DelIndexbook to save index from file
 *
 * @return OK if everythings is ok, and error in other case
 */
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

int main(int argc, char *argv[])
{
    FILE *pfile_db = NULL, *pfile_del = NULL, *pfile_ind = NULL;
    Record *registro;
    Array *ind_arr = NULL;
    Del_Array *ind_del_arr = NULL;
    Indexbook *ind = NULL;
    Index_deleted_book *ind_del = NULL;
    int result_bsc; /*result of binary search */
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
    long hole_offset; /* offset of hole that can be used to insert register when we are adding new book */
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

    if (argc < 3)
    {
        fprintf(stdout, "Missing argument");
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
        fprintf(stdout, "Unknown search strategy unknown_search_strategy");
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
        freeArray(ind_arr);
        return ERR;
    }

    initArray(ind_arr, 5);
    initDelArray(ind_del_arr, 5);
    offset = 0; /*Esto hay que cambiar ya que los indice debe esta ordenado*/

    /*Check if exists file with index information and store them before run programm*/
    pfile_ind = fopen(ind_filename, "rb");
    if (pfile_ind != NULL)
    {
        /*reloading the index*/
        if (reload_index(pfile_ind, ind_arr) == ERR)
        {
            freeArray(ind_arr);
            freeDelArray(ind_del_arr);
            return ERR;
        }
        fclose(pfile_ind);
    }
    else
    {
        pfile_ind = fopen(ind_filename, "wb");
        if (pfile_ind == NULL)
        {
            /* pendiente de clen up*/
        }
    }

    /*Check if exists file with deleted index information and store them before run programm*/
    pfile_del = fopen(ind_del_filename, "rb");
    if (pfile_del != NULL)
    {
        /*reloading the index*/
        if (reload_del_index(pfile_del, ind_del_arr) == ERR)
        {
            printf("problem\n");
            freeArray(ind_arr);
            freeDelArray(ind_del_arr);
            fclose(pfile_ind);
            return ERR;
        }

        /*close the index file*/
        fclose(pfile_del);
    }
    else
    {
        pfile_del = fopen(ind_del_filename, "wb");
        if (pfile_del == NULL)
        {
            /* pendiente de clen up*/
        }
    }
    fprintf(stdout, "Type command and arguments/s.\n");
    fprintf(stdout, "exit\n");

    /* Open the file that we are going to store book information*/
    pfile_db = fopen(db_filename, "wb");
    if (pfile_db == NULL)
    {
        freeArray(ind_arr);
        freeDelArray(ind_del_arr);
        fclose(pfile_del);
        fclose(pfile_ind);
        return ERR;
    }

    /* Open the file that we are going to store deleted_index information */
    registro = malloc(sizeof(Record));
    if (registro == NULL)
    {
        fclose(pfile_del);
        fclose(pfile_db);
        fclose(pfile_ind);
        freeArray(ind_arr);
        freeDelArray(ind_del_arr);

        return ERR;
    }

    while (fgets(command, MAX_COMMAND, stdin) && (strcmp(command, "exit\n") != 0))
    {
        command[strcspn(command, "\r\n")] = 0;
        token = strtok(command, " ");
        if (strcmp(token, "add") == 0)
        {
            /* allocate memory of indexbook*/
            ind = malloc(sizeof(Indexbook));
            if (ind == NULL)
            {
                fclose(pfile_del);
                fclose(pfile_db);
                fclose(pfile_ind);
                freeArray(ind_arr);
                freeDelArray(ind_del_arr);
                free(registro);
                return ERR;
            }

            /* read the book_id */
            token = strtok(NULL, "|");
            registro->book_id = atoi(token);
            if (ind_arr->used > 0)
            {
                if (binary_search(ind_arr, registro->book_id) >= 0)
                {
                    fprintf(stdout, "Record with Book_ID==%d exists\n", registro->book_id);
                    free(ind);
                    continue;
                }
            }
            /* read the isbn */
            token = strtok(NULL, "|");
            strncpy(registro->isbn, token, ISBN);
            registro->isbn[ISBN] = '\0';

            /* read the title */
            token = strtok(NULL, "|");
            strncpy(registro->title, token, MAX_STRING);
            registro->title[MAX_STRING] = '\0';

            /* read the printedBy (editorial) */
            token = strtok(NULL, "\r\n");
            strncpy(registro->printedBy, token, MAX_STRING);
            registro->printedBy[MAX_STRING] = '\0';

            /* total size of register */
            registro->size = sizeof(registro->book_id) + ISBN + strlen(registro->title) + strlen(registro->printedBy) + sizeof(char);

            hole_offset = find_and_use_hole(ind_del_arr, registro->size, strategy);
            if (hole_offset >= 0)
            {
                fseek(pfile_db, hole_offset, SEEK_SET);
            }
            else if (hole_offset == NOT_FOUNDED)
            {
                /* values for ind */
                fseek(pfile_db, 0, SEEK_END);
            }
            else if (hole_offset == ERR)
            {
                /* pediente de clean up*/
            }
            offset = ftell(pfile_db);
            ind->key = registro->book_id;
            ind->offset = offset;
            ind->size = registro->size;

            /* add book to the file */
            book_to_file(pfile_db, registro);
            /*insert ind to arr*/
            insertArray(ind_arr, ind);

            fprintf(stdout, "Record with BookID=%d has been added to the database\n", registro->book_id);
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

            /* Check if the book_id is in the ind or not*/
            result_bsc = binary_search(ind_arr, book_id);
            if (result_bsc == ERR)
            {
                fclose(pfile_del);
                fclose(pfile_db);
                fclose(pfile_ind);
                freeArray(ind_arr);
                freeDelArray(ind_del_arr);
                free(registro);
            }
            else if (result_bsc == NOT_FOUNDED)
            {
                fprintf(stdout, "Record with bookId=%d does not exist\n", book_id);
            }
            else
            {
                ind_del = malloc(sizeof(Index_deleted_book));
                ind_del->register_size = ind_arr->array[result_bsc]->size;
                ind_del->offset = ind_arr->array[result_bsc]->offset;
                insertDelArray(ind_del_arr, ind_del, strategy);
                remove_from_index(ind_arr, result_bsc);
                fprintf(stdout, "Record with BookID=%d has been deleted\n", book_id);
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

            if (find(ind_arr,book_id,pfile_db) == ERR)
            {
                /* pendiente de clen up*/
            }
        }
        else if (strcmp(token, "printRec") == 0)
        {

            /* Tamaño de los metadatos al inicio del registro: size_t + int */
            header_size = sizeof(size_t) + sizeof(int);

            /* Iteramos por el índice, que garantiza el orden por BookID y que no hay borrados */
            for (i = 0; i < ind_arr->used; i++)
            {
                /* Recuperamos datos del índice */
                offset = ind_arr->array[i]->offset;
                rec_size = ind_arr->array[i]->size;
                current_id = ind_arr->array[i]->key;

                /* Calculamos cuánto ocupan las cadenas de texto */
                data_len = rec_size - header_size;

                if (data_len > 0)
                {
                    buffer = (char *)malloc(data_len + 1);
                    if (buffer != NULL)
                    {
                        /* Nos posicionamos saltando el size y el ID, directos al texto */
                        fseek(pfile_db, offset + header_size, SEEK_SET);

                        if ((long)fread(buffer, 1, data_len, pfile_db) == data_len)
                        {
                            buffer[data_len] = '\0'; /* Null termination de seguridad */

                            /* 1. Extraer ISBN (Primeros 16 bytes fijos) */
                            strncpy(isbn_buff, buffer, ISBN);
                            isbn_buff[ISBN] = '\0';

                            /* 2. Localizar separador '|' entre Título y Editorial */
                            ptr_title = buffer + ISBN;
                            ptr_pipe = strchr(ptr_title, '|');

                            if (ptr_pipe != NULL)
                            {
                                /* Extraer Título */
                                int title_len = ptr_pipe - ptr_title;
                                if (title_len > MAX_STRING)
                                    title_len = MAX_STRING;

                                strncpy(title_buff, ptr_title, title_len);
                                title_buff[title_len] = '\0';

                                /* Extraer Editorial (lo que sigue al pipe) */
                                strncpy(printed_buff, ptr_pipe + 1, MAX_STRING);
                                printed_buff[MAX_STRING] = '\0';
                            }
                            else
                            {
                                /* Fallback por si el archivo está corrupto */
                                strcpy(title_buff, ptr_title);
                                strcpy(printed_buff, "N/A");
                            }

                            /* Imprimir en el formato solicitado */
                            fprintf(stdout, "%d|%s|%s|%s\n", current_id, isbn_buff, title_buff, printed_buff);
                        }
                        free(buffer);
                    }
                }
            }
        }
    }

    /* Start print in the index file*/
    fclose(pfile_db);

    pfile_ind = fopen(ind_filename, "wb");
    if (pfile_ind == NULL)
    {
        /* pendiente de clen up*/
    }

    if (index_to_file(pfile_ind, ind_arr) == ERR)
    {
        fclose(pfile_del);
        fclose(pfile_ind);
        free(registro);
        freeArray(ind_arr);
        freeDelArray(ind_del_arr);
        return ERR;
    }
    /* finished */
    fprintf(stdout, "all done\n");

    pfile_del = fopen(ind_del_filename, "wb");
    if (pfile_del == NULL)
    {
        /* pendiente de clen up*/
    }
    index_del_to_file(pfile_del, ind_del_arr, strategy);
    fclose(pfile_ind);
    fclose(pfile_del);
    free(registro);
    freeArray(ind_arr);
    freeDelArray(ind_del_arr);
    return 0;
}