#ifndef _HAKINATOR
#define _HAKINATOR

#include "tree.h"
#include "stack.h"

typedef struct
{
    BTree **Root;
    const char *name_base;
    const char *new_name_base;
    struct stack stk;
} Akinat; //FIXME Akinator

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
    UNKNOW        = -1,
    ANSWER_NO      = 0,
    ANSWER_YES     = 1,
    ANSWER_UNKNOW  = 2,

};



#endif //_HAKINATOR
