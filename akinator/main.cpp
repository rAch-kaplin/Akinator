#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "read_file.h"
#include "akinator.h"
#include "color.h"


int main()
{
    system("clear");

        printf(WHTB"# Akinator     \n"
           " (c) rAch, 2025\n\n" RESET
            YELB "Выберите соотвествующий режим работы\n\n"
            MAGB "Guessing game \n"

           "Exit        \n\n" RESET);

    BTree *Root = nullptr;

    CreateNode(&Root, "Is it a living thing?");
    CreateNode(&Root->left, "Is it a human?");
    CreateNode(&Root->right, "Is it a car?");

    Akinator(Root);
    TreeDumpDot(Root);
    FreeTree(&Root);
}

