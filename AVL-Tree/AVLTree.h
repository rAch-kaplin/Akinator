#ifndef _HAVLTREE
#define _HAVLTREE

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "read_file.h"

//typedef int elem_t;
typedef char* elem_t;
#define FORMAT_STR "%s"


typedef struct AVLTree
{
    elem_t value;
    struct AVLTree* left;
    struct AVLTree* right;
    int height;
} AVLTree;

typedef enum
{
    OK                  = 0x000000,
    ALLOC_ERR           = 0x000001,
    NODE_NULLPTR        = 0x000002,
    REPEAT_ELEM         = 0x000004,
    FILE_NOT_OPEN       = 0x000008,
} TreeErrors;

TreeErrors InsertLinesIntoTree(AVLTree **Root, FileData *file_data);

TreeErrors CreateNode(AVLTree** Node, elem_t value);
TreeErrors FreeTree(AVLTree** Node);
TreeErrors InsertNode(AVLTree **Root, elem_t value);
AVLTree* NodeFind(AVLTree* Root, elem_t value);
TreeErrors TreeTraversal(AVLTree* Node);
int GetHeight(AVLTree* Node);
int GetBalanceFactor(AVLTree* Node);
TreeErrors UpdateHeight(AVLTree* Node);
TreeErrors RightRotate(AVLTree **Root);
TreeErrors LeftRotate(AVLTree **Root);
TreeErrors DeleteNode(AVLTree **Root, elem_t value);

TreeErrors TreeDumpDot(AVLTree* Root);
int GenerateGraph(AVLTree *Node, char* buffer, int* buffer_len, const size_t BUFFER_SIZE);

#endif // _HAVLTREE
