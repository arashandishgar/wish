
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "StringArray.h"
#include <stdarg.h>

void init_array(StringArray *stringArray) {
    if (stringArray == NULL) {
        perror("the pointer must not be null ");
    }
    stringArray->size = 0;
    stringArray->array = NULL;
}

void addParaMeter(StringArray *stringArray, const char *string) {
    if (string == NULL) {
        perror("string cannot be Null");
        return;
    }
    stringArray->size++;
    stringArray->array = realloc(stringArray->array, sizeof(char *) * stringArray->size);
    stringArray->array[stringArray->size - 1] = strdup(string);
}

char *getParameter(StringArray *stringArray, int index) {
    int validRage = stringArray->size - 1;
    if (index > validRage || index < 0) {
        printf("invalid range\n");
    }
    return stringArray->array[index];
}

void removeAll(StringArray *stringArray) {
    if (stringArray->size == 0) {
        return;
    }
    int *size = &stringArray->size;
    for (int i = 0; i < *size; i++) {
        free(stringArray->array[i]);
    }
    free(stringArray->array);
    *size = 0;
    stringArray->array = NULL;
}

void printNewLineOfString(char *format) {
    printf("%s\n", format);
}

void showTotalArray(StringArray *array) {
    if (array->size == 0||array==NULL) {
        printf("nothing\n");
        return;
    } else {
        forEachParameter(array, printNewLineOfString);
    }

}

void replaceOldValueWithNewOne(StringArray *stringArray, char *new, int index) {
    if (index >= stringArray->size || index < 0) {
        printf("invalid index\n");
        return;
    }
    char *old = stringArray->array[index];
    stringArray->array[index] = strdup(new);
    free(old);
}

void forEachParameter(StringArray *array, void(*starRoutine)(char *)) {
    if (array->size != 0) {
        for (int i = 0; i < array->size; i++) {
            starRoutine(getParameter(array, i));
        }
    }
}
int containValue(StringArray *stringArray,char *value){
    for(int i=0;i<stringArray->size;i++){
        if(strcmp(getParameter(stringArray,i),value)==0){
            return i;
        }
    }
    return -1;
}
//return StringArray* and must be free by malloc library
StringArray *getCopyRangeOfArray( StringArray *stringArray,  int baseIndex,  int endIndex) {
    if (endIndex > stringArray->size - 1) {
        printf("invalid array range");
        return NULL;
    } else if(endIndex<0|| baseIndex<0){
        printf(" Index cannot negative");
        return NULL;
    } else if (endIndex < baseIndex) {
        printf("end index must be grater than base index");
        return NULL;
    } else if (stringArray == NULL) {
        printf("stringArray cannot be null");
        return NULL;
    } else {
        int size = endIndex - baseIndex + 1;
        StringArray *result=malloc(sizeof(StringArray));
        init_array(result);
        for (int i = 0; i < size; i++) {
            addParaMeter(result,getParameter(stringArray,baseIndex+i));
        }
        return result;
    }

}
//TODO(Implement remove not neccessary)
