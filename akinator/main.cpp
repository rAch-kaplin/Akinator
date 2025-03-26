#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "read_file.h"
#include "akinator.h"
#include "color.h"
#include "logger.h"

int main()
{
    LoggerInit(LOGL_DEBUG, "logger/logfile.log", DEFAULT_MODE);

    BTree *Root = nullptr;
    const char *name_base = "akinator/base.txt";

    MenuGuessing(&Root, name_base);
    FreeTree(&Root);

    LoggerDeinit();
    printf(GREEN "End main\n" RESET);
}

