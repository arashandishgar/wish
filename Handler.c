#define _GNU_SOURCE

#include<stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define wish "wish"
#define exitWorld "exit"
#define cd "cd"
#define redirectionSign ">"
#define MAX_PARALLELISM 10
#define PARALLELISM_SYMBOL "&"

#include "StringArray.h"

_Bool readLine(FILE *file, char **buffer, size_t *size) {
    //Todo(check if you can do with *buffer instead of **buffer) in TODO(check)
    ssize_t nRead = getline(buffer, size, file);
    if (nRead != -1) {
        char *dest = malloc(sizeof(char) * nRead);
        strncpy(dest, *buffer, nRead - 1);
        dest[nRead - 1] = '\0';
        //TODO(Check)
        free(*buffer);
        *buffer = dest;
    }
    return nRead != -1;
}

_Bool handleBuiltinCommand(StringArray *parameter, char *buffer, StringArray *paths) {
    if (strcmp(getParameter(parameter, 0), exitWorld) == 0) {
        free(buffer);
        removeAll(parameter);
        removeAll(paths);
        exit(EXIT_SUCCESS);
    } else if (strcmp(getParameter(parameter, 0), cd) == 0) {
        if (parameter->size != 2) {
            printf("to many argument\n");
            return true;
        }
        if (chdir(getParameter(parameter, 1)) != 0) {
            printf("invalid director\n");
        }
        return true;
    } else if (strcmp(getParameter(parameter, 0), "path") == 0) {
        if (parameter->size == 1) {
            removeAll(paths);
        } else {
            for (int i = 1; i < parameter->size; i++) {
                if (access(getParameter(parameter, i), X_OK) == 0) {
                    addParaMeter(paths, getParameter(parameter, i));
                } else {
                    printf("this parameter cannot be use as path : %s\n", getParameter(parameter, i));
                }
            }
        }
        return true;
    }
    return false;
}

char *strCatAndReturnNewPointerToDynamicMemory(char *first, char *second) {
    size_t firstLength = strlen(first);
    size_t secondLength = strlen(second);
    char *result = malloc(sizeof(char) * (firstLength + secondLength + 1));
    strcpy(result, first);
    strcat(result, second);
    return result;
}

void handleBuiltOutCommand(StringArray *parameters, StringArray *paths, char *buffer) {
    int index;
    if ((index = containValue(parameters, redirectionSign)) > -1) {
        if (index == parameters->size - 1 || index == 0) {
            printf("the correct use of >: command > file\n");
            removeAll(paths);
            removeAll(parameters);
            free(buffer);
            exit(EXIT_FAILURE);
        } else {
            FILE *i = fopen(getParameter(parameters, index + 1), "w+");
            if (i == NULL) {
                fwrite("invalid dir\n", 1, strlen("invalid dir"), stdout);
                exit(EXIT_FAILURE);
            } else {
                fflush(i);
                fclose(i);
                close(STDOUT_FILENO);
                open(getParameter(parameters, index + 1), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
            }
        }
    }
    if (access(getParameter(parameters, 0), X_OK) == 0) {
        if (index == -1) {
            index = parameters->size;
        }
        StringArray *tmpArray = NULL;
        tmpArray = getCopyRangeOfArray(parameters, 0, index - 1);
        tmpArray->array = realloc(tmpArray->array, (tmpArray->size + 1) * sizeof(char *));
        tmpArray->array[tmpArray->size] = NULL;
        execv(getParameter(parameters, 0), tmpArray->array);
        removeAll(tmpArray);
        free(tmpArray);
    }

    char *oldParameter = strdup(getParameter(parameters, 0));
    for (int i = 0; i < paths->size; i++) {
        char *tmp = strCatAndReturnNewPointerToDynamicMemory(getParameter(paths, i), "/");
        char *result = result = strCatAndReturnNewPointerToDynamicMemory(tmp, oldParameter);
        replaceOldValueWithNewOne(parameters, result, 0);
        free(result);
        free(tmp);
        if (access(getParameter(parameters, 0), X_OK) == 0) {
            if (index == -1) {
                index = parameters->size;
            }
            StringArray *tmpArray = NULL;
            tmpArray = getCopyRangeOfArray(parameters, 0, index - 1);
            tmpArray->array = realloc(tmpArray->array, (tmpArray->size + 1) * sizeof(char *));
            tmpArray->array[tmpArray->size] = NULL;
            execv(getParameter(parameters, 0), tmpArray->array);
            removeAll(tmpArray);
            free(tmpArray);
        }
    }
    free(oldParameter);
    removeAll(paths);
    removeAll(parameters);
    free(buffer);
    printf("invalid command\n");
    exit(EXIT_FAILURE);
}

void handleInput(char *buffer, StringArray *paths) {
    char *delim = " ";
    char *param = NULL;
    StringArray parameters;
    init_array(&parameters);
    while ((param = strsep(&buffer, delim)) != NULL) {
        if (strcmp(param, "") != 0) {
            addParaMeter(&parameters, param);
        }
    }
    if (parameters.size == 0) {
        return;
    }

    if (!handleBuiltinCommand(&parameters, buffer, paths)) {

        //TODO(Built-out Command)  paralisem
        int indices[MAX_PARALLELISM];
        for (int i = 0; i < MAX_PARALLELISM; i++) {
            indices[i] = -1;
        }
        int j = -1;
        if (containValue(&parameters, PARALLELISM_SYMBOL) > -1) {
            for (int i = 0; i < parameters.size; i++) {
                if (strcmp(getParameter(&parameters, i), PARALLELISM_SYMBOL) == 0) {
                    j++;
                    if (j > 9) {
                        printf("they can have just ten parallelism");
                        removeAll(&parameters);
                        return;
                    }
                    indices[j] = i;
                    if (i > 1) {
                        if (indices[j] == (indices[j - 1] + 1) || i == 0 || i == parameters.size - 1) {
                            printf("invalid token\n");
                            removeAll(&parameters);
                            return;
                        }
                    }
                }
            }
        }

        int size = 0;
        if (j == -1) {
            pid_t pid;
            pid = fork();

            if (pid == 0) {
                handleBuiltOutCommand(&parameters, paths, buffer);
            } else {
                int i = -1;
                wait(&i);
                removeAll(&parameters);
                return;
            }
        } else {
            size = j + 1;
        }
        StringArray *newParameters;
        pid_t pid=0;
        for (int i = 0; i < size; i++) {
            if (i == 0) {
                newParameters = getCopyRangeOfArray(&parameters, 0, indices[i] - 1);
                if((pid=fork())==0){
                    handleBuiltOutCommand(newParameters,paths,buffer);
                    break;
                }
            } else {
                newParameters = getCopyRangeOfArray(&parameters, indices[i - 1] + 1, indices[i] - 1);
                if((pid=fork())==0){
                    handleBuiltOutCommand(newParameters,paths,buffer);
                    break;
                }
            }
            if (i == size - 1) {
                removeAll(newParameters);
                free(newParameters);
                newParameters = getCopyRangeOfArray(&parameters, indices[i] + 1, parameters.size - 1);
                if((pid=fork())==0){
                    handleBuiltOutCommand(newParameters,paths,buffer);
                    removeAll(newParameters);
                    free(newParameters);
                    break;
                }
            }
        }
        if(pid!=0){
            pid_t wpid;
            while ((wpid = wait(NULL)) > 0);
        }
        removeAll(newParameters);
        free(newParameters);
        removeAll(&parameters);
    }

}

void handleBatchMode(FILE *file, struct StringArray *paths) {
    char *buffer = NULL;
    size_t size;
    while (readLine(file, &buffer, &size) == true) {
        handleInput(buffer, paths);
    }
    removeAll(paths);
    free(buffer);
    fflush(file);
    fclose(file);
}

void handleInteractiveMode(StringArray *paths) {
    while (true) {
        char *buffer = NULL;
        size_t size;
        char *currentDirectory = get_current_dir_name();
        printf("%s:%s >", wish, currentDirectory);
        free(currentDirectory);
        if (readLine(stdin, &buffer, &size) == false) {
            printf("fail to read line\n");
            free(buffer);
            exit(EXIT_FAILURE);
        } else {
            handleInput(buffer, paths);
        }
        free(buffer);
    }
}