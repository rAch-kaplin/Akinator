#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "akinator.h"
#include "color.h"
#include "logger.h"
#include "arg_parser.h"

int main(int argc, const char* argv[])
{
    const char *log_file = "lib/logger/logfile.log";
    LoggerInit(LOGL_DEBUG, log_file, DEFAULT_MODE);

    ArgOption options[] = { {"-i",  "--input", true, nullptr, false},
                            {"-o", "--output", true, nullptr, false} };

    if (ParseArguments(argc, argv, options, sizeof(options) / sizeof(options[0])) != PARSE_OK)
    {
        fprintf(stderr, "ParseArg error\n");
        return 1;
    }

    BTree *Root = nullptr;
    const char *name_base = options[INPUT].argument;

    MenuGuessing(&Root, name_base);
    TreeDumpDot(Root);
    FreeTree(&Root);

    LoggerDeinit();
    printf(GREEN "End main\n" RESET);
}

