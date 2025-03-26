#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "tree.h"
#include "akinator.h"
#include "color.h"
#include "logger.h"

void AkinatorInit(BTree **Node, const char *name_base);

int main()
{
    LoggerInit(LOGL_DEBUG, "logger/logfile.log", DEFAULT_MODE);

    BTree *Root = nullptr;
    const char *name_base = "base.txt";

    AkinatorInit(&Root, name_base);

    FreeTree(&Root);

    LoggerDeinit();
    printf(GREEN "End main\n" RESET);
}

