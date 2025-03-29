#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "akinator.h"
#include "color.h"
#include "logger.h"

int main() // TODO const
{
    LoggerInit(LOGL_DEBUG, "lib/logger/logfile.log", DEFAULT_MODE); //TODO logget(output)

    BTree *Root = nullptr;
    const char *name_base = "akinator/base.txt"; //TODO arg

    MenuGuessing(&Root, name_base);
    TreeDumpDot(Root);
    FreeTree(&Root);

    LoggerDeinit();
    printf(GREEN "End main\n" RESET);
}

