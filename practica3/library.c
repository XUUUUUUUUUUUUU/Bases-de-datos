#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ISBN 16          /* Max length of ISBN code of book */
#define MAX_STRING 128   /* Max length of strings */
#define MAX_COMMAND 1024 /* Max length of commands */
#define OK 0             /* Ok code, if all things is ok */
#define ERR -1           /* Error code */
#define NOT_FOUNDED -2   /* No se ha encontrado key en busqueda binaria */
#define BESTFIT 0
#define WORSTFIT 1
#define FIRSTFIT 2

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

void initArray(Array *a, size_t initialSize)
{
    a->array = (Indexbook **)malloc(initialSize * sizeof(Indexbook *));
    a->used = 0;
    a->size = initialSize;
}

void insertArray(Array *a, Indexbook *ind)
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
    if (fwrite(registro->isbn, strlen(registro->isbn), 1, pfile) != 1)
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

int main(int argc, char *argv[])
{
    FILE *pfile = NULL, *pfile_del = NULL;
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

    /*Check if exists file which index information and store them before run programm*/
    pfile = fopen(ind_filename, "rb");
    if (pfile != NULL)
    {
        /*reloading the index*/
        if (reload_index(pfile, ind_arr) == ERR)
        {
            freeArray(ind_arr);
            freeDelArray(ind_del_arr);
            return ERR;
        }

        /*close the index file*/
        fclose(pfile);
    }

    fprintf(stdout, "Type command and arguments/s.\n");
    fprintf(stdout, "exit\n");

    /* Open the file that we are going to store book information*/
    pfile = fopen(db_filename, "wb");
    if (pfile == NULL)
    {
        freeArray(ind_arr);
        freeDelArray(ind_del_arr);

        return ERR;
    }

    /* Open the file that we are going to store deleted_index information */
    pfile_del = fopen(ind_del_filename, "wb");
    if (pfile_del == NULL)
    {
        freeArray(ind_arr);
        freeDelArray(ind_del_arr);
        fclose(pfile);

        return ERR;
    }
    registro = malloc(sizeof(Record));
    if (registro == NULL)
    {
        fclose(pfile);
        fclose(pfile_del);
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
                fclose(pfile);
                fclose(pfile_del);
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
            registro->size = sizeof(registro->book_id) + strlen(registro->isbn) + strlen(registro->title) + strlen(registro->printedBy) + sizeof(char);

            book_to_file(pfile, registro);

            /* values for ind */
            ind->key = registro->book_id;
            ind->offset = offset;
            ind->size = registro->size;
            offset = ftell(pfile);

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
                fclose(pfile);
                fclose(pfile_del);
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
    }

    /* Start print in the index file*/
    fclose(pfile);
    pfile = fopen(ind_filename, "wb");
    if (pfile == NULL)
    {
        fclose(pfile_del);
        free(registro);
        freeArray(ind_arr);
        freeDelArray(ind_del_arr);
        return ERR;
    }
    if (index_to_file(pfile, ind_arr) == ERR)
    {
        fclose(pfile);
        free(registro);
        freeArray(ind_arr);
        freeDelArray(ind_del_arr);
        return ERR;
    }
    /* finished */
    fprintf(stdout, "all done\n");
    index_del_to_file(pfile_del, ind_del_arr, strategy);
    fclose(pfile);
    fclose(pfile_del);
    free(registro);
    freeArray(ind_arr);
    freeDelArray(ind_del_arr);
    return 0;
}