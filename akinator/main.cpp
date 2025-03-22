#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "AVLTree.h"
#include "read_file.h"

int main()
{
    AVLTree *Root = nullptr;

    char *file_content = NULL;
    FileData file_data = {};

    codeError error = readFile(&file_content, &file_data);

    if (error != NO_ERROR)
    {
        switch (error)
        {
            case NO_ERROR:
                break;
            case FAILED_OPEN_FILE: fprintf(stderr, "Failed open file\n");
                break;
            case MEMORY_ALLOCATION_ERROR: fprintf(stderr, "MEMORY_ALLOCATION_ERROR\n");
                break;
            case FILE_READ_ERROR: fprintf(stderr, "FILE_READ_ERROR\n");
                break;
            default:
                fprintf(stderr, "Unknow error occurred\n");
        }
    }

    resultOfReadFile(&file_data);

    TreeErrors err = InsertLinesIntoTree(&Root, &file_data);
    if (err != OK)
    {
        fprintf(stderr, "Ошибка при вставке строк в AVL-дерево\n");
    }

    TreeDumpDot(Root);

    FreeTree(&Root);
    free(file_content);
    free(file_data.lines);
}

