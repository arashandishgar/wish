//
// Created by arashandishgar on 9/18/20.
//
typedef struct StringArray{
    char  **array;
    int size;
}StringArray;
void init_array(StringArray *stringArray);
void addParaMeter(StringArray  *stringArray, const char* string);
char* getParameter(StringArray *stringArray, int index);
void removeAll(StringArray *stringArray);
void showTotalArray(StringArray* array);
void forEachParameter(StringArray *array,void(*starRoutine)(char*));
void replaceOldValueWithNewOne(StringArray *stringArray, char *new, int index);
int containValue(StringArray *stringArray,char *value);
StringArray *getCopyRangeOfArray( StringArray *stringArray,  int baseIndex,  int endIndex);