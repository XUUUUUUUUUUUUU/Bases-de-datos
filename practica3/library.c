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

/* auxiliar functions */

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
    if (fwrite(&registro->size, sizeof(size_t), 1, pfile) != 1) return ERR;

    /* write book_id */
    if (fwrite(&registro->book_id, sizeof(registro->book_id), 1, pfile) != 1) return ERR;
    
    /* write isbn*/
    if (fwrite(registro->isbn, strlen(registro->isbn), 1, pfile) != 1) return ERR;
    
    /* write title */
    if (fwrite(registro->title, strlen(registro->title), 1, pfile) != 1) return ERR;
    if (fputc('|', pfile) == EOF) return ERR; 

    /* write printedBy*/
    if (fwrite(registro->printedBy, strlen(registro->printedBy), 1, pfile) != 1) return ERR;

    return OK;
}

int main(int argc, char *argv[])
{
    FILE *pfile = NULL;
    Record *registro;
    int i;
    char command[MAX_COMMAND + 1];
    char *token;
    char mem_mode[MAX_STRING + 1]; /* memory allocation strategy */
    char filename[MAX_STRING + 1]; /* filename of information to stored */

    if (argc < 3)
    {
        fprintf(stderr, "Error parameters\n");
        return ERR;
    }
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "first_fit") == 0 || strcmp(argv[i], "last_fit") == 0 || strcmp(argv[i], "best_fit") == 0)
        {
            strcpy(mem_mode, argv[i]);
        }
        else
        {
            strcpy(filename, argv[i]);
        }
    }

    fprintf(stdout, "Type command and arguments/s.\n");
    fprintf(stdout, "exit\n");

    /* Open the file with we are going to store book information*/
    pfile = fopen(filename, "wb");
    registro = malloc(sizeof(Record));

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
    while (fgets(command, MAX_COMMAND, stdin) && (strcmp(command, "exit\n") != 0))
    {
        token = strtok(command, " ");
        if (strcmp(token, "add") == 0)
        {
            /* read the book_id */
            token = strtok(NULL, "|");
            registro->book_id = atoi(token);

            /* read the isbn */
            token = strtok(NULL, "|");
            strncpy(registro->isbn, token, ISBN);
            registro->printedBy[ISBN]='\0';

            /* read the title */
            token = strtok(NULL, "|");
            strncpy(registro->title, token, MAX_STRING);
            registro->title[MAX_STRING]='\0';

            /* read the printedBy (editorial) */
            token = strtok(NULL, "\n");
            strncpy(registro->printedBy, token, MAX_STRING);
            registro->printedBy[MAX_STRING]='\0';

            /* total size of register */
            registro->size=sizeof(registro->book_id)
                           +sizeof(registro->isbn)
                           +strlen(registro->title)+1
                           +strlen(registro->printedBy);

            book_to_file(pfile, registro);

            fprintf(stdout, "Record with BookID=%d has been added to the database\n", registro->book_id);
            fprintf(stdout, "exit\n");
        } else if(strcmp(token, "printInd") == 0)
        {

        }
    }
    free(registro);
    fclose(pfile);
    return 0;
}