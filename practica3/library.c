#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ISBN 16
#define MAX_STRING 128

typedef struct {
    int book_id;
    char isbn[ISBN+1];
    char title[MAX_STRING+1];
    char printedBy[MAX_STRING+1];
    int size;

}record;
int main()
{  
    FILE *pfile=NULL;
    printf("Type command and arguments/s.");
    printf("exit");
    return 0;
}