#include <stdio.h>
#include <assert.h>
#include "Handler.h"
#include <stdbool.h>
int main(int argc, char **argv) {
    StringArray  path;
    init_array(&path);
    addParaMeter(&path,"/bin");
    addParaMeter(&path,"/usr/bin");
    assert(argc <= 2);
    FILE *file;
    bool isBatchMode = false;
    if (argc == 2) {
        isBatchMode = true;

        file = fopen(argv[1], "a+");
    }
    if (isBatchMode) {
        handleBatchMode(file, &path);
    } else {
        handleInteractiveMode(&path);
    }
    return 0;
}
