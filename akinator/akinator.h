#ifndef _HAKINATOR
#define _HAKINATOR

#include "tree.h"

void Akinator(BTree **Node, const char *name_base);
CodeError ParseTree(BTree **Node, char **buffer);
size_t GetBaseSizeFile(FILE *name_base);
char *ReadBaseToBuffer(const char *name_base, size_t *file_size);

const size_t MAX_QUESTION = 150;

enum mode
{
    key_guessing = 103,

    key_exit     = 113,

};



#endif //_HAKINATOR
