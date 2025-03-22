#ifndef _HREAD_FILE
#define _HREAD_FILE
#include <stdio.h>

#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_RESET   "\x1b[0m"

typedef struct
{
    char *str;
    size_t len;
} StringElement;

typedef struct
{
    StringElement *lines;
    size_t size;
} FileData;

enum codeError
{
    FAILED_OPEN_FILE         =    0x000000,
    MEMORY_ALLOCATION_ERROR  =    0x000001,
    FILE_READ_ERROR          =    0x000002,
    NO_ERROR                 =    0x000004,
};

void fileSize(FILE *file, size_t *size_of_file);
codeError readFile(char **file_content, FileData *file_data);
void resultOfReadFile(const FileData *file_data);

#endif
