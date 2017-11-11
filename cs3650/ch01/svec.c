// svec.c
// Author: Nat Tuck
// 3650F2017, Challenge01 Hints

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "svec.h"

svec*
make_svec()
{
    svec* sv = malloc(sizeof(svec));
    sv->size = 0;
    sv->cap  = 4;
    sv->data = malloc(4 * sizeof(char*));
    memset(sv->data, 0, 4 * sizeof(char*));
    return sv;
}

void
free_svec(svec* sv)
{
    int ii;
    for (ii = 0; ii < sv->size; ++ii) {
        if (sv->data[ii] != 0) {
            free(sv->data[ii]);
        }
    }
    free(sv->data);
    free(sv);
}

char*
svec_get(svec* sv, int ii)
{
    assert(ii >= 0 && ii < sv->size);
    return sv->data[ii];
}

void
svec_put(svec* sv, int ii, char* item)
{
    assert(ii >= 0 && ii < sv->size);
    sv->data[ii] = strdup(item);

}

void svec_push_back(svec* sv, char* item)
{
    int ii = sv->size;

    if (ii >= sv->cap) {
        sv->cap *= 2;
        sv->data = (char**) realloc(sv->data, sv->cap * sizeof(char*));
    }

    sv->size = ii + 1;
    svec_put(sv, ii, item);
}

// does the given svec contain the given element?
int
svec_contains(svec* sv, char* s)
{
    int ii;
    for (ii = 0; ii < sv->size; ii++) {
        if (strcmp(s, svec_get(sv, ii)) == 0) {
            return 1;
        }
    }
    return 0;
}

// splits the given svec on the given index.
// mutates the given svec to be the first section
// and returns the second section
svec*
svec_split(svec* sv1, int ii)
{
    svec* sv2 = make_svec();
    int jj;
    for (jj = ii + 1; jj < sv1->size; jj++) {
        svec_push_back(sv2, svec_get(sv1, jj));
    }
    for (jj = ii; jj < sv1->size; jj++) {
        if (sv1->data[jj] != 0) {
            free(sv1->data[jj]);
        }
    }
    sv1->size = ii;
    return sv2;
}

int
index_of(svec* sv, char* s)
{
    int ii;
    for (ii = 0; ii < sv->size; ii++) {
        if (strcmp(s, svec_get(sv, ii)) == 0) {
            return ii;
        }
    }
    return -1;
}

void
print_svec(svec* sv) {
    printf("Printing svec: ");
    int ii;
    for (ii = 0; ii < sv->size; ii++) {
        printf("%s ", svec_get(sv, ii));
    }
    printf("\n");
}
