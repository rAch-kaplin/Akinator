#ifndef _HAKINATOR
#define _HAKINATOR

#include "tree.h"

CodeError Akinator(BTree **Node, const char *name_base);
//CodeError ParseTree(BTree **Node, char **buffer);
CodeError ParseTree(BTree **Node, char **buffer, BTree *parent);
size_t GetBaseSizeFile(FILE *name_base);
int GetMode();
CodeError CreateTree(BTree **Node, const char *name_base);
char *ReadBaseToBuffer(const char *name_base, size_t *file_size);
void MenuGuessing(BTree **Node, const char *name_base);

const size_t MAX_QUESTION = 150;

enum mode
{
    key_guessing = 103,
    key_restart  = 114,
    key_exit     = 113,
};



#endif //_HAKINATOR
