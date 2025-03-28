#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <color.h>

#include "tree.h"
#include "akinator.h"
#include "logger.h"


const size_t SIZE_QUESTION = 100;

bool CheckYesAnswer(char *answer);
bool CheckNoAnswer(char *answer);

void ProcessingModeGame(BTree **Node, const char *name_base);
CodeError AddNewObject(BTree** Node, const char* name_base);

void MenuGuessing(BTree **Node, const char *name_base)
{
    system("clear");

        printf(WHTB"# Akinator     \n"
           " (c) rAch, 2025\n\n" RESET
            YELB "Select the appropriate operating mode\n\n"
            MAGB "[g]: Guessing game \n"

           "[q]: Exit        \n\n" RESET);

    ProcessingModeGame(Node, name_base);
}

int GetMode()
{
    system("stty raw -echo");
    int c = getchar();
    system("stty sane");
    return c;
}

CodeError CreateTree(BTree **Node, const char *name_base)
{
    assert(Node != nullptr);
    //BTree *Root = *Node;

    size_t file_size = 0;
    char *base_buffer = ReadBaseToBuffer(name_base, &file_size);
    char *pars_buffer = base_buffer;
    CodeError err = ParseTree(Node, &pars_buffer);
    if (err != OK)
    {
        LOG(LOGL_ERROR, "ParsTree error: %d", err);
        free(base_buffer);
        return err;
    }
    free(base_buffer);
    TreeDumpDot(*Node);
    return OK;
}

void ProcessingModeGame(BTree **Node, const char *name_base)
{
    int mode_game = GetMode();

    switch (mode_game)
    {
        case key_guessing:
        {
            CreateTree(Node, name_base);
            Akinator(Node, name_base);
            break;
        }
        case key_exit:
        {
            return;
        }
        default:
        {
            break;
        }
    }
}

CodeError Akinator(BTree **Node, const char *name_base)
{
    assert(Node != nullptr);

    char *answer = (char*)calloc(SIZE_QUESTION, sizeof(char));
    if (answer == nullptr)
    {
        LOG(LOGL_ERROR, "ALLOC_FAILD");
        return ALLOC_ERR;
    }

    printf(CYAN "It's %s? (yes/no/q - quit/r - restart): " RESET, (*Node)->data);
    scanf("%99s", answer);

    if (strcmp(answer, "q") == 0)
    {
        free(answer);
        return OK;
    }
    else if (strcmp(answer, "r") == 0)
    {
        free(answer);
        return OK;
    }

    if (CheckYesAnswer(answer) && (*Node)->left)
    {
        Akinator(&(*Node)->left, name_base);
    }

    else if (CheckNoAnswer(answer) && (*Node)->right)
    {
        Akinator(&(*Node)->right, name_base);
    }

    else
    {
        if (CheckYesAnswer(answer))
        {
            printf(GREEN "I FIND THE ELEMENT\n" RESET);
            free(answer);
            return OK;
        }
        else if (CheckNoAnswer(answer))
        {
            printf(MAGENTA "I DO NOT FIND, LETS WRITE IT\n" RESET);
            CodeError err = AddNewObject(Node, name_base);
            if (err != OK)
            {
                LOG(LOGL_ERROR, "Failed to add a new object");
                return err;
            }
            free(answer);
            return OK;
        }
        else
        {
            printf(RED "INCORRECT INPUT, PLEASE, TRY AGAIN\n" RESET);
            free(answer);
            Akinator(Node, name_base);
            return INVALID_FORMAT;
        }
    }

    free(answer);
    return OK;
}

CodeError AddNewObject(BTree** Node, const char* name_base)
{
    assert(Node != nullptr);

    char* new_object = (char*)calloc(SIZE_QUESTION, sizeof(char));
    char* question =   (char*)calloc(SIZE_QUESTION, sizeof(char));

    if (!new_object || !question)
    {
        free(new_object);
        free(question);
        return ALLOC_ERR;
    }

    printf(MAGENTA "Who did you guess? " RESET);
    scanf(" %99[^\n]", new_object);

    printf(MAGENTA "What's the difference between %s and %s? " RESET, new_object, (*Node)->data);
    scanf(" %99[^\n]", question);

    BTree* NewNodeQuestion = nullptr;
    CodeError err = CreateNode(&NewNodeQuestion, question);
    if (err != OK)
    {
        free(new_object);
        free(question);
        return err;
    }

    BTree* NewNode = nullptr;
    err = CreateNode(&NewNode, new_object);
    if (err != OK)
    {
        free(new_object);
        free(question);
        return err;
    }

    BTree* OldNode = *Node;

    *Node = NewNodeQuestion;

    NewNodeQuestion->left = NewNode;

    NewNodeQuestion->right = OldNode;

    free(new_object);
    free(question);

    return OK;
}

bool CheckYesAnswer(char *answer)
{
    assert(answer);
    if (strncmp(answer, "yes", 3) == 0)
        return true;
    else
        return false;
}

bool CheckNoAnswer(char *answer)
{
    assert(answer);
    if (strncmp(answer, "no", 2) == 0)
        return true;
    else
        return false;
}

size_t GetBaseSizeFile(FILE *name_base)
{
    fseek(name_base, 0, SEEK_END);
    size_t file_size = (size_t)ftell(name_base);
    rewind(name_base);

    return file_size;
}

char *ReadBaseToBuffer(const char *name_base, size_t *file_size)
{
    FILE *base_file = fopen(name_base, "r");
    if (base_file == nullptr)
    {
        LOG(LOGL_ERROR, "Can't open file: %s", name_base);
        return nullptr;
    }

    *file_size = GetBaseSizeFile(base_file);
    char *buffer = (char*)calloc(*file_size + 1, sizeof(char));
    if (buffer == nullptr)
    {
        LOG(LOGL_ERROR, "Memory allocation failed");
        fclose(base_file);
        return nullptr;
    }

    size_t read = fread(buffer, sizeof(char), *file_size, base_file);
    if (read != *file_size)
    {
        free(buffer);
        fclose(base_file);
        return nullptr;
    }

    fclose(base_file);

    return buffer;
}

CodeError ParseTree(BTree **Node, char **buffer)
{
    LOG(LOGL_DEBUG, "Start ParseTree");

    if (buffer == nullptr || *buffer == nullptr)
    {
        LOG(LOGL_ERROR, "INVALID_ARGUMENT");
        return INVALID_ARGUMENT;
    }

    while (isspace(**buffer)) (*buffer)++;
    if (**buffer == '\0')
    {
        LOG(LOGL_ERROR, "UNEXPECTED_END_OF_INPUT");
        return UNEXPECTED_END_OF_INPUT;
    }

    if (**buffer == '{')
    {
        (*buffer)++;
        LOG(LOGL_DEBUG, "Found {");

        while (isspace(**buffer)) (*buffer)++;

        CodeError err = ParseTree(Node, buffer);
        if (err != OK) {
            FreeTree(Node);
            return err;
        }

        while (isspace(**buffer)) (*buffer)++;

        if (**buffer != '}')
        {
            LOG(LOGL_ERROR, "MISSING_CLOSING_BRACE");
            FreeTree(Node);
            return INVALID_FORMAT;
        }
        (*buffer)++;
        LOG(LOGL_DEBUG, "Found }");

        return OK;
    }

    if (**buffer == '<')
    {
        (*buffer)++;

        char name[MAX_QUESTION] = {};
        if (sscanf(*buffer, "%[^>]>", name) != 1)
        {
            LOG(LOGL_ERROR, "INVALID_LEAF_FORMAT");
            return INVALID_FORMAT;
        }
        (*buffer) += strlen(name) + 1;

        LOG(LOGL_DEBUG, "Leaf node: %s", name);
        return CreateNode(Node, name);
    }

    if (**buffer == '?')
    {
        (*buffer)++;

        char question[MAX_QUESTION] = {};
        if (sscanf(*buffer, "%[^?]?", question) != 1)
        {
            LOG(LOGL_ERROR, "INVALID_QUESTION_FORMAT");
            return INVALID_FORMAT;
        }
        (*buffer) += strlen(question) + 1;

        LOG(LOGL_DEBUG, "Question node: %s", question);
        CodeError err = CreateNode(Node, question);
        if (err != OK) return err;

        err = ParseTree(&((*Node)->left), buffer);
        if (err != OK)
        {
            FreeTree(Node);
            return err;
        }

        err = ParseTree(&((*Node)->right), buffer);
        if (err != OK) {
            FreeTree(Node);
            return err;
        }

        return OK;
    }

    LOG(LOGL_ERROR, "UNKNOWN_SYMBOL: %c", **buffer);
    return INVALID_FORMAT;
}
