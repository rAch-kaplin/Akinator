#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "read_file.h"
#include "akinator.h"
#include "color.h"
#include "logger.h"

void ProcessingModeGame(BTree **Node, const char *name_base);
void MenuGuessing(BTree **Node, const char *name_base);
void MenuGuessing(BTree **Node, const char *name_base)
{
    system("clear");

        printf(WHTB"# Akinator     \n"
           " (c) rAch, 2025\n\n" RESET
            YELB "Выберите соотвествующий режим работы\n\n"
            MAGB "[g]: Guessing game \n"

           "[q]: Exit        \n\n" RESET);

    ProcessingModeGame(Node, name_base);
}

int GetMode()
{
    system("stty raw -echo");
    int c = getchar();
    system("stty sane");
    return c;
}

CodeError CreateTree(BTree **Node, const char *name_base)
{
    size_t file_size = 0;
    char *base_buffer = ReadBaseToBuffer(name_base, &file_size);
    char *pars_buffer = base_buffer;
    CodeError err = ParseTree(Node, &pars_buffer);
    if (err != OK)
    {
        LOG(LOGL_ERROR, "ParsTree error: %d", err);
        free(base_buffer);
        return err;
    }
    free(base_buffer);
    TreeDumpDot(*Node);
    return OK;
}

void ProcessingModeGame(BTree **Node, const char *name_base)
{
    int mode_game = GetMode();

    switch (mode_game)
    {
        case key_guessing:
        {
            CreateTree(Node, name_base);
            Akinator(Node, name_base);
            break;
        }
        case key_exit:
        {
            return;
        }
        default:
        {
            break;
        }
    }
}

int main()
{
    LoggerInit(LOGL_DEBUG, "logger/logfile.log", DEFAULT_MODE);

    BTree *Root = nullptr;
    const char *name_base = "akinator/base.txt";

    MenuGuessing(&Root, name_base);
    // TreeDumpDot(Root);
    FreeTree(&Root);

    LoggerDeinit();
    printf(GREEN "End main\n" RESET);
}

