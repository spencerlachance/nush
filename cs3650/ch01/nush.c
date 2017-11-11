#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tokenize.h"
#include "svec.h"

// I got this method off of Stack Overflow
// Author: hmjd
char** str_split(char* a_str, const char a_delim)
{
    char** result = 0;
    size_t count = 0;
    char* tmp = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    while (*tmp) {
        if (a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    count += last_comma < (a_str + strlen(a_str) - 1);
    count++;
    result = malloc(sizeof(char*) * count);
    
    if (result) {
        size_t idx = 0;
        char* token = strtok(a_str, delim);
        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    return result;
}

// adapted from Nat Tuck's starter code
int
execute(char* cmd)
{
    int cpid;
    if ((cpid = fork())) {
        // parent
        int status;
        waitpid(cpid, &status, 0);
        return WEXITSTATUS(status);
    }
    else {
        // child
        char** args = str_split(cmd, ' ');
        int ii;
        for (ii = 0; ii < strlen(cmd); ii++) {
            if (cmd[ii] == ' ') {
                cmd[ii] = 0;
                break;
            }
        }
        execvp(cmd, args);
        free(args);
    }    
}

// Adapted from sort-pipe.c from Nat Tuck's lecture notes
int
execute_pipe(svec* sv)
{
    int cpid;
    svec* right = svec_split(sv, index_of(sv, "|"));
    char* cmd = detokenize(sv);     

    if ((cpid = fork())) {
        // parent 1
        int status;
        waitpid(cpid, &status, 0);
        free_svec(right);
        free(cmd);
        return WEXITSTATUS(status);
    }
    else {
        //child 1
        int rv;
        int pipes[2];
        rv = pipe(pipes);
        assert(rv != -1); 
       
        //pipes[0] is for reading
        //pipes[1] is for writing

        int cpid2;
        if ((cpid2 = fork())) {
            // parent 2
            close(pipes[1]);

            int status2;
            waitpid(cpid2, &status2, 0);

            close(0);
            rv = dup(pipes[0]);
            assert(rv != -1);

            evaluate_toks(right);
        }
        else {
            // child 2
            close(pipes[0]);
            close(1);

            rv = dup(pipes[1]);
            assert(rv != -1);
            
            char** args = str_split(cmd, ' ');
            int ii;
            for (ii = 0; ii < strlen(cmd); ii++) {
                if (cmd[ii] == ' ') {
                    cmd[ii] = 0;
                    break;
                }
            }   
            execvp(cmd, args);
            free(args);
        }
    }
}

int
redirect_input(svec* tokens)
{
    svec* right = svec_split(tokens, index_of(tokens, "<"));
    char* file_name = detokenize(right);
    char* cmd = detokenize(tokens);
    
    int fd = open(file_name, O_RDONLY);
    
    int si = dup(0);
    close(0);
    int rv = dup(fd);
    assert(rv == 0);
    close(fd);

    execute(cmd);

    dup2(si, 0);
}

int
redirect_output(svec* tokens)
{
    svec* right = svec_split(tokens, index_of(tokens, ">"));
    char* file_name = detokenize(right);
    char* cmd = detokenize(tokens);
    
    int fd = open(file_name, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    
    int so = dup(1);
    close(1);
    int rv = dup(fd);
    assert(rv == 1);
    close(fd);

    execute(cmd);

    dup2(so, 1);
}

void
execute_bkgrd(char* cmd)
{
    int cpid;
    if ((cpid = fork())) {
        // parent
    }
    else {
        // child
        char** args = str_split(cmd, ' ');
        int ii;
        for (ii = 0; ii < strlen(cmd); ii++) {
            if (cmd[ii] == ' ') {
                cmd[ii] = 0;
                break;
            }
        }
        execvp(cmd, args);
        free(args);
    }    
}

int
evaluate_toks(svec* tokens)
{
    int rv;
    char* cmd;
    svec* right;
    if (svec_contains(tokens, "<")) {
        return redirect_input(tokens);
    }    
    else if (svec_contains(tokens, ">")) {
        return redirect_output(tokens);
    }    
    else if (svec_contains(tokens, "|")) {
        return execute_pipe(tokens);
    }
    else if (svec_contains(tokens, "&")) {
        right = svec_split(tokens, index_of(tokens, "&"));
        cmd = detokenize(tokens);
        execute_bkgrd(cmd);
        free(cmd);
        free_svec(right);
    }
    else if (svec_contains(tokens, "&&")) {
        right = svec_split(tokens, index_of(tokens, "&&"));
        rv = evaluate_toks(tokens);
        if (!rv) {
            rv = evaluate_toks(right);
            free_svec(right);
            return rv;
        }
        else {
            return rv;
        }
    }        
    else if (svec_contains(tokens, "||")) {
        right = svec_split(tokens, index_of(tokens, "||"));
        rv = evaluate_toks(tokens);
        if (rv) {
            rv = evaluate_toks(right);
            free_svec(right);
            return rv;
        }
        else {
            return rv;
        }
    }        
    else if (svec_contains(tokens, ";")) {
        right = svec_split(tokens, index_of(tokens, ";"));  
        evaluate_toks(tokens);
        rv = evaluate_toks(right);
        free_svec(right);
        return rv;
    }
    else {
        if (strcmp("cd", svec_get(tokens, 0)) == 0) {
            return chdir(svec_get(tokens, 1));
        }
        if (strcmp("exit", svec_get(tokens, 0)) == 0) {
            exit(0); 
        }
        cmd = detokenize(tokens);
        rv = execute(cmd);
        free(cmd);
        return rv;
    }
}

// adapted from nat tuck's starter code
int
main(int argc, char* argv[])
{
    char cmd[256];
    char* rv;
    FILE* script;
    if (argc > 1) {
        script = fopen(argv[1], "r");
    }
    while(1) {
        if (argc == 1) {
            printf("nush$ ");
            fflush(stdout);
            rv = fgets(cmd, 256, stdin); 
        }
        else {
            rv = fgets(cmd, 256, script);
        }
        if (!rv) {
            break;
        }
        svec* tokens = tokenize(cmd);
        evaluate_toks(tokens);
        free_svec(tokens); 
    }
    return 0;
}
