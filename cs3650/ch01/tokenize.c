// tokenize.c
// Author: Nat Tuck
// 3650F2017, Challenge01 Hints

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "tokenize.h"

typedef int (*char_pred)(char);

int
is_op_char(char cc)
{
    return cc == '<' || cc == '>'
        || cc == '|' || cc == '&'
        || cc == ';';
}

int
is_nop_char(char cc)
{
    return cc != 0 && !is_op_char(cc) && !isspace(cc);
}

char*
get_tok(char* text, char_pred cpred)
{
    char* tt = malloc(256);
    int ii = 0;
    for (; cpred(text[ii]); ++ii) {
        tt[ii] = text[ii];
    }
    tt[ii] = 0;
    return tt;
}

svec*
tokenize(char* text)
{
    svec* xs = make_svec();

    while (text[0]) {
        if (isspace(text[0])) {
            text++;
            continue;
        }

        char* tt;

        if (is_op_char(text[0])) {
            tt = get_tok(text, is_op_char);
        }
        else {
            tt = get_tok(text, is_nop_char);
        }

        svec_push_back(xs, tt);
        text += strlen(tt);
        free(tt);
    }

    return xs;
}

char*
detokenize(svec* sv)
{
    int len = 0;
    int ii;
    for (ii = 0; ii < sv->size; ii++) {
        len += 1 + strlen(svec_get(sv, ii));
    }

    int z = len * sizeof(char);
    assert(z > 0);
    char* str = malloc(z);
    int count = 0;
    int jj, kk;
    for (jj = 0; jj < sv->size; jj++) {
        char* word = svec_get(sv, jj);
        for (kk = 0; kk < strlen(word); kk++) {
            str[count] = word[kk];
            count++;
        }
        if (jj < sv->size - 1) {
            str[count] = ' ';
            count++;
        }    
    }
    str[count] = '\0';
    return str;
}
