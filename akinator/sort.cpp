#include "read_file.h"
#include "tree.h"

TreeErrors InsertLinesIntoTree(BTree **Root, FileData *file_data)
{
    if (file_data == nullptr || file_data->lines == nullptr) return NODE_NULLPTR;

    for (size_t i = 0; i < file_data->size; i++)
    {
        TreeErrors err = InsertNode(Root, file_data->lines[i].str);
        if (err != OK)
        {
            fprintf(stderr, "String insertion error: %s\n", file_data->lines[i].str);
            return err;
        }
    }

    return OK;
}
