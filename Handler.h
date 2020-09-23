

#include <stdbool.h>
#include "StringArray.h"

void handleBatchMode(FILE *file, StringArray *paths);
bool handleInteractiveMode(StringArray *paths);
#define wish "wish> "