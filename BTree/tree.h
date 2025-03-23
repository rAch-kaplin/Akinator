#ifndef _HTREE
#define _HTREE

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "read_file.h"

typedef char* elem_t;
#define FORMAT_ELEM "%s"

typedef struct BTree
{
    elem_t data;
    struct BTree *left;
    struct BTree *right;
} BTree;

enum TreeErrors
{
    OK                  = 0x000000,
    ALLOC_ERR           = 0x000001,
    NODE_NULLPTR        = 0x000002,
    REPEAT_ELEM         = 0x000004,
    FILE_NOT_OPEN       = 0x000008,
};

TreeErrors InsertLinesIntoTree(BTree **Root, FileData *file_data);

TreeErrors CreateNode(BTree **Node, elem_t value);
TreeErrors FreeTree(BTree **Node);
TreeErrors InsertNode(BTree **Root, elem_t value);
BTree* NodeFind(BTree *Root, elem_t value);
BTree* NodeFind2(BTree *Root, elem_t value);
TreeErrors TreeTraversal(BTree *Node);
TreeErrors InsertNodeLoop(BTree **Root, elem_t value);
TreeErrors InsertNodeLoop2(BTree **Root, elem_t value);
TreeErrors DeleteNode(BTree **Root, elem_t value);

TreeErrors TreeDumpDot(BTree* Root);
int GenerateGraph(BTree *Node, char* buffer, int* buffer_len, const size_t BUFFER_SIZE);

#endif // _HTREE
