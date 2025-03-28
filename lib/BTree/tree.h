#ifndef _HTREE
#define _HTREE

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include "read_file.h"

typedef char* elem_t;
#define FORMAT_ELEM "%s"

typedef struct BTree
{
    elem_t data;
    struct BTree *left;
    struct BTree *right;
} BTree;

enum CodeError
{
    OK                      = 0x0000,
    ALLOC_ERR               = 0x0001,
    NODE_NULLPTR            = 0x0002,
    REPEAT_ELEM             = 0x0004,
    FILE_NOT_OPEN           = 0x0008,
    INVALID_ARGUMENT        = 0x0010,
    UNEXPECTED_END_OF_INPUT = 0x0020,
    INVALID_FORMAT          = 0x0040,
    EMPTY_VALUE             = 0x0080,
    BUFFER_OVERFLOW         = 0x0100,
};


//CodeError InsertLinesIntoTree(BTree **Root, FileData *file_data);

CodeError CreateNode(BTree **Node, elem_t value);
CodeError FreeTree(BTree **Node);
CodeError InsertNode(BTree **Root, elem_t value);
BTree* NodeFind(BTree *Root, elem_t value);
BTree* NodeFind2(BTree *Root, elem_t value);
CodeError TreeTraversal(BTree *Node);
CodeError InsertNodeLoop(BTree **Root, elem_t value);
CodeError InsertNodeLoop2(BTree **Root, elem_t value);
CodeError DeleteNode(BTree **Root, elem_t value);

CodeError TreeDumpDot(BTree* Root);
int GenerateGraph(BTree *Node, char* buffer, int* buffer_len, const size_t BUFFER_SIZE);

#endif // _HTREE
