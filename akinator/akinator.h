#ifndef _HAKINATOR
#define _HAKINATOR

#include "tree.h"

typedef struct
{
    BTree **Root;
    const char *name_base;
    const char *new_name_base;
} Akinat;

// CodeError Akinator(BTree **Node, Akinat *akn);
// CodeError ParseTree(BTree **Node, char **buffer, BTree *parent);
// size_t GetBaseSizeFile(FILE *name_base);
// int GetMode();
// CodeError CreateTree(BTree **Node, Akinat *akn);
// char *ReadBaseToBuffer(const char *name_base, size_t *file_size);
void MenuGuessing(BTree **Node, const char *name_base);

const size_t MAX_QUESTION = 150;

enum Mode
{
    key_guessing   = 103,
    key_restart    = 114,
    key_exit       = 113,
    key_definition = 100,
    key_difference =  68,
};

enum AnswerType
{
    ANSWER_UNKNOW = -1,
    ANSWER_NO      = 0,
    ANSWER_YES    =  1,
};



#endif //_HAKINATOR
