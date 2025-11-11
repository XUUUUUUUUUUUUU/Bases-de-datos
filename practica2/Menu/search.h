/*
 Created by Alejandro Zheng on 09/11/2025.
*/

#ifndef NCOURSES_SEARCH_H
#define NCOURSES_SEARCH_H
#include "windows.h"
#include <string.h>
/*#include <unistd.h>*/
void results_search(char * from, char *to, char *date, int * n_choices,
                    char *** choices, int max_length, int max_rows);

#endif /*NCOURSES_SEARCH_H*/
