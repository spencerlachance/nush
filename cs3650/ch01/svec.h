// svec.h
// Author: Nat Tuck
// 3650F2017, Challenge01 Hints

#ifndef SVEC_H
#define SVEC_H

typedef struct svec {
    int size;
    int cap;
    char** data;
} svec;

svec* make_svec();
void  free_svec(svec* sv);

char* svec_get(svec* sv, int ii);
void  svec_put(svec* sv, int ii, char* item);

void svec_push_back(svec* sv, char* item);

void svec_sort(svec* sv);

int svec_contains(svec* sv, char* s);
svec* svec_split(svec* sv1, int ii);
int index_of(svec* sv, char* s);
void print_svec(svec* sv);

#endif
