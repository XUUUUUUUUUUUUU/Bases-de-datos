#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ISBN 16          /* Max length of ISBN code of book */
#define MAX_STRING 128   /* Max length of strings */
#define MAX_COMMAND 1024 /* Max length of commands */
#define OK 0             /* Ok code, if all things is ok */
#define ERR -1           /* Error code */

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
    Indexbook **array;
    size_t used;
    size_t size;
} Array;

/* auxiliar functions */

void initArray(Array *a, size_t initialSize)
{
    a->array = (Indexbook **)malloc(initialSize * sizeof(Indexbook *));
    a->used = 0;
    a->size = initialSize;
}

void insertArray(Array *a, Indexbook *element)
{
    if (a->used == a->size)
    {
        a->size *= 2;
        a->array = (Indexbook **)realloc(a->array, a->size * sizeof(Indexbook *));
    }
    a->array[a->used++] = element;
}

void freeArray(Array *a)
{
    size_t i;
    if (a)
    {
        if (a->array)
        {
            for (i = 0; i < a->size; i++)
            {
                if (a->array[i])
                {
                    free(a->array[i]);
                }
            }
            free(a->array);
        }
    }
    a->array = NULL;
    a->used = a->size = 0;
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

int main(int argc, char *argv[])
{
    FILE *pfile = NULL;
    Record *registro;
    Array *arr = NULL;
    Indexbook *ind = NULL;
    long int offset;
    char command[MAX_COMMAND + 1];
    char *token;
    char mem_mode[MAX_STRING + 1]; /* memory allocation strategy */
    char filename[MAX_STRING + 1]; /* filename of information to stored */
    size_t i;

    if (argc < 3)
    {
        fprintf(stderr, "Missing argument\n");
        return ERR;
    }

    if (strcmp(argv[1], "first_fit") == 0 || strcmp(argv[1], "last_fit") == 0 || strcmp(argv[1], "best_fit") == 0)
    {
        strcpy(mem_mode, argv[1]);
    }
    else
    {
        fprintf(stderr, "Unknown search strategy unknown_search_strategy\n");
        return ERR;
    }

    strcpy(filename, argv[2]);

    fprintf(stdout, "Type command and arguments/s.\n");
    fprintf(stdout, "exit\n");

    /* Open the file with we are going to store book information*/
    pfile = fopen(filename, "wb");
    registro = malloc(sizeof(Record));
    arr=malloc(sizeof(Array));
    initArray(arr, 5);
    offset=0;
    if (pfile == NULL)
    {
        free(registro);
        return ERR;
    }
    if (registro == NULL)
    {
        fclose(pfile);
        return ERR;
    }
    if (arr == NULL)
    {
        fclose(pfile);
        free(registro);
        return ERR;
    }

    while (fgets(command, MAX_COMMAND, stdin) && (strcmp(command, "exit\n") != 0))
    {
        command[strlen(command)-1]=' ';
        token = strtok(command, " ");
        if (strcmp(token, "add") == 0)
        {
            /* allocate memory*/
            ind = malloc(sizeof(Indexbook));
            if (ind == NULL)
            {
                fclose(pfile);
                free(registro);
                freeArray(arr);
                return ERR;
            }

            /* read the book_id */
            token = strtok(NULL, "|");
            registro->book_id = atoi(token);

            /* read the isbn */
            token = strtok(NULL, "|");
            strncpy(registro->isbn, token, ISBN);
            registro->printedBy[ISBN] = '\0';

            /* read the title */
            token = strtok(NULL, "|");
            strncpy(registro->title, token, MAX_STRING);
            registro->title[MAX_STRING] = '\0';

            /* read the printedBy (editorial) */
            token = strtok(NULL, "\r");
            strncpy(registro->printedBy, token, MAX_STRING);
            registro->printedBy[MAX_STRING] = '\0';

            /* total size of register */
            registro->size = sizeof(registro->book_id) + sizeof(registro->isbn) + strlen(registro->title) + strlen(registro->printedBy)-1;

            book_to_file(pfile, registro);

            /* values for ind */
            ind->key = registro->book_id;
            ind->offset =offset;
            ind->size = registro->size;
            offset=ftell(pfile);

            /*insert ind to arr*/
            insertArray(arr,ind);

            fprintf(stdout, "Record with BookID=%d has been added to the database\n", registro->book_id);
            fprintf(stdout, "exit\n");
        }
        else if (strcmp(token, "printInd") == 0)
        {
            for (i = 0; i < arr->used; i++)
            {
                fprintf(stdout, "Entry #%ld\n", i);
                fprintf(stdout, "    key: #%d\n", arr->array[i]->key);
                fprintf(stdout, "    offset: #%ld\n", arr->array[i]->offset);
            }

            fprintf(stdout, "exit\n");
        }
    }

    free(registro);
    fclose(pfile);
    freeArray(arr);
    free(arr);
    return 0;
}