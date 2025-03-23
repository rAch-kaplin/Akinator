#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"

TreeErrors CreateNode(BTree** Node, elem_t data)
{
    *Node = (BTree*)calloc(1, sizeof(BTree));
    if (*Node == nullptr)
    {
        return ALLOC_ERR;
    }

    (*Node)->data = strdup(data);
    if ((*Node)->data == nullptr)
    {
        free(*Node);
        return ALLOC_ERR;
    }

    (*Node)->left = nullptr;
    (*Node)->right = nullptr;

    return OK;
}

TreeErrors FreeTree(BTree** Node)
{
    if (*Node == nullptr) return OK;

    FreeTree(&((*Node)->left));
    FreeTree(&((*Node)->right));

    free((*Node)->data);
    free(*Node);
    *Node = nullptr;

    return OK;
}

TreeErrors InsertNode(BTree** Root, elem_t data)
{
    if (*Root == nullptr)
    {
        return CreateNode(Root, data);
    }

    int cmp = strcmp(data, (*Root)->data);

    if (cmp < 0)
    {
        return InsertNode(&((*Root)->left), data);
    }
    else if (cmp > 0)
    {
        return InsertNode(&((*Root)->right), data);
    }
    else
    {
        return REPEAT_ELEM;
    }

    return OK;
}

TreeErrors InsertNodeLoop(BTree **Root, elem_t data)
{
    if (*Root == nullptr)
    {
        return CreateNode(Root, data);
    }

    BTree *cur = *Root;
    while (cur != nullptr)
    {
        if (data < cur->data)
        {
            if (cur->left != nullptr)
            {
                cur = cur->left;
            }
            else
            {
                return CreateNode(&(cur->left), data);
            }
        }
        else
        {
            if (cur->right != nullptr)
            {
                cur = cur->right;
            }
            else
            {
                return CreateNode(&(cur->right), data);
            }
        }
    }

    return OK;
}

TreeErrors InsertNodeLoop2(BTree **Root, elem_t data)
{
    if (! *Root)
    {
        return CreateNode(Root, data);
    }

    BTree *cur = *Root;

    while (cur)
    {
        if (data < cur->data)
        {
            if (cur->left) cur = cur->left;
            else           return CreateNode(&(cur->left), data);
        }

        else
        {
            if (cur->right) cur = cur->right;
            else            return CreateNode(&(cur->right), data);
        }
    }

    return OK;
}

BTree* NodeFind(BTree* Root, elem_t data)
{
    if (Root == nullptr)
    {
        return nullptr;
    }

    int cmp = strcmp(data, Root->data);

    if (cmp == 0) return Root;
    if (cmp  < 0) return NodeFind(Root->left, data);
    else          return NodeFind(Root->right, data);
}

TreeErrors TreeTraversal(BTree *Node)
{
    if (Node == nullptr)
    {
        return NODE_NULLPTR;
    }

    TreeTraversal(Node->left);
    printf(FORMAT_ELEM " ", Node->data);
    TreeTraversal(Node->right);

    return OK;
}

TreeErrors DeleteNode(BTree** Root, elem_t data)
{
    if (*Root == nullptr) return NODE_NULLPTR;

    int cmp = strcmp(data, (*Root)->data);

    if      (cmp < 0) return DeleteNode(&((*Root)->left), data);
    else if (cmp > 0) return DeleteNode(&((*Root)->right), data);

    else
    {
        if ((*Root)->left == nullptr)
        {
            BTree* temp = *Root;
            *Root = (*Root)->right;
            free(temp->data);
            free(temp);
            temp = nullptr;
        }
        else if ((*Root)->right == nullptr)
        {
            BTree* temp = *Root;
            *Root = (*Root)->left;
            free(temp->data);
            free(temp);
            temp = nullptr;
        }
        else
        {
            BTree* minNode = (*Root)->right;
            while (minNode->left != nullptr)
            {
                minNode = minNode->left;
            }

            free((*Root)->data);
            (*Root)->data = strdup(minNode->data);
            return DeleteNode(&((*Root)->right), minNode->data);
        }
    }

    return OK;
}
