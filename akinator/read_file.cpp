#include <stdlib.h>
#include "read_file.h"

void fileSize(FILE *file, size_t *size_of_file)
{

    long current_position = ftell(file);
    fseek(file, 0, SEEK_END);
    *size_of_file = (size_t)ftell(file);
    fseek(file, current_position, SEEK_SET);
}

codeError readFile(char **file_content, FileData *file_data)
{
    FILE * file = fopen("akinator/onegin.txt", "r");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file\n");
        return FAILED_OPEN_FILE;
    }

    size_t size_of_file = 0;
    fileSize(file, &size_of_file);

    printf("---SIZE_OF_FILE = %zu\n", size_of_file);

    *file_content = (char*)calloc(size_of_file + 1, sizeof(char));
    if (*file_content == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        return MEMORY_ALLOCATION_ERROR;
    }

    if (fread(*file_content, sizeof(char), size_of_file, file) != size_of_file)
    {
        fprintf(stderr, "File read error\n");
        return FILE_READ_ERROR;
    }
    fclose(file);

    for (size_t i = 0; i < size_of_file; i++)
    {
        if ((*file_content)[i] == '\n')
        {
            (*file_content)[i] = '\0';
            file_data->size++;
        }
    }


    printf("nStrings = %zu\n", file_data->size);

    file_data->lines = (StringElement*)calloc(file_data->size + 1, sizeof(StringElement));
    if (file_data->lines == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        return MEMORY_ALLOCATION_ERROR;
    }

    size_t line_index = 0;
    size_t start_new_line = 0;

    (file_data->lines)[line_index++].str = *file_content;
    for (size_t i = 0; i < size_of_file; i++)
    {
        if ((*file_content)[i] == '\0' && i + 1 < size_of_file)
        {
            (file_data->lines)[line_index].str = &(*file_content)[i + 1];
            (file_data->lines)[line_index].len = i - 1 - start_new_line;

            //fprintf(stderr, COLOR_RED "len = %zu\n" COLOR_RESET, (file_data->lines)[line_index].len);

            start_new_line = i + 1;
            line_index++;
        }
    }
    return NO_ERROR;
}

void resultOfReadFile(const FileData *file_data)
{
    puts("Содержимое считанного файла:\n");
    for (size_t j = 0; j < file_data->size; j++)
    {
        printf("---%s\n", file_data->lines[j].str);
    }
    putchar('\n');
}
