#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "tree.h"
#include "akinator.h"
#include "logger.h"
#include "stack.h"
#include "debug.h"
#include "color.h"

const size_t SIZE_QUESTION = 100;
#define MAX_SIZE_BUFFER "99"

void ProcessingModeGame(BTree **Node, const char *name_base);
CodeError AddNewObject(BTree** Node);
CodeError SaveDatabase(Akinat *akn);
CodeError SaveTreeToFile(BTree *node, char *base_buf, int depth, int *cur_len, const size_t SIZE_BUFFER);

bool FindWordNode(stack *stk, BTree *Node, const char *word);
CodeError DefinitionObject(BTree *Node);
CodeError FindDifference(BTree* Node);
void ReverseStack(struct stack* stk);
void PrintDefinition(stack *stk, char *word);

CodeError HandleUnsolvedLeaf(BTree **Node, Akinat *akn);
void HandleAnswer(BTree **Node, AnswerType type_answ, Akinat *akn);

AnswerType CheckAnswer(char *answer);

void MenuGuessing(BTree **Node, const char *name_base)
{
    system("clear");

        printf(WHTB"# Akinator     \n"
           " (c) rAch, 2025\n\n" RESET
            YELB "Select the appropriate operating mode\n\n"
            MAGB "[g]: Guessing  \n"
            MAGB "[d]: Defenition\n"
            MAGB "[D]: Diference \n"

           "[q]: Exit        \n\n" RESET);

    ProcessingModeGame(Node, name_base);
}

int GetMode()
{
    system("stty raw -echo");
    int c = getchar();
    system("stty sane");
    return c; //TODO Termios
}

CodeError CreateTree(BTree **Node, Akinat *akn)
{
    assert(Node != nullptr);

    size_t file_size = 0;
    char *base_buffer = ReadBaseToBuffer(akn->name_base, &file_size);
    char *pars_buffer = base_buffer;
    CodeError err = ParseTree(Node, &pars_buffer, nullptr);
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

    BTree **Root = Node;
    Akinat akn = {Node, name_base, "akinator/new_base.txt"};

    switch (mode_game)
    {
        case key_guessing:
        {
            CreateTree(Node, &akn);
            Akinator(Node, &akn);
            break;
        }
        case key_definition:
        {
            CreateTree(Node, &akn);
            DefinitionObject(*Root);
            break;
        }
        case key_difference:
        {
            CreateTree(Node, &akn);
            FindDifference(*Node);
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

CodeError Akinator(BTree **Node, Akinat *akn)
{
    assert(Node != nullptr);

    char *answer = (char*)calloc(SIZE_QUESTION, sizeof(char));
    if (answer == nullptr)
    {
        LOG(LOGL_ERROR, "ALLOC_FAILD");
        return ALLOC_ERR;
    }

    printf(CYAN "It's %s? (yes/no/q - quit/r - restart): " RESET, (*Node)->data);
    scanf("%" MAX_SIZE_BUFFER "s", answer);

    if (strcmp(answer, "q") == 0) //TODO q Q
    {
        free(answer);
        return OK;
    }
    else if (strcmp(answer, "r") == 0)
    {
        free(answer);
        Akinator(akn->Root, akn);
    }

    AnswerType type_answ = CheckAnswer(answer);
    free(answer);

    if (type_answ != ANSWER_UNKNOW)
    {
        HandleAnswer(Node, type_answ, akn);
    }
    else
    {
        printf(RED "INCORRECT INPUT, PLEASE, TRY AGAIN\n" RESET);
        Akinator(Node, akn);
    }

    return OK;
}

CodeError HandleUnsolvedLeaf(BTree **Node, Akinat *akn)
{
    assert(Node != nullptr);
    assert(akn->Root != nullptr);

    printf(MAGENTA "I DO NOT FIND, LETS WRITE IT\n" RESET);
    CodeError err = AddNewObject(Node);
    if (err != OK)
    {
        LOG(LOGL_ERROR, "Failed to add a new object");
        return err;
    }

    printf(YELLOW "Do you want to save changes? (yes/no): " RESET);
    char save_answer[MAX_QUESTION] = "";
    scanf("%" MAX_SIZE_BUFFER "s", save_answer);

    AnswerType type_answ = CheckAnswer(save_answer);

    if (type_answ == ANSWER_YES)
    {
        if (SaveDatabase(akn) == OK)
            printf(GREEN "Changes saved successfully!\n" RESET);
        else
            printf(RED "Error saving the database!\n" RESET);
    }
    else
    {
        printf(CYAN "Changes were not saved.\n" RESET);
    }

    return OK;
}

void HandleAnswer(BTree **Node, AnswerType type_answ, Akinat *akn)
{
    assert(Node != nullptr);

    switch (type_answ)
    {
        case ANSWER_YES:
        {
            if ((*Node)->left != nullptr)  Akinator(&(*Node)->left, akn);
            else                           printf(GREEN "I FIND THE ELEMENT\n" RESET);

            break;
        }

        case ANSWER_NO:
        {
            if ((*Node)->right != nullptr) Akinator(&(*Node)->right, akn);
            else                           HandleUnsolvedLeaf(Node, akn);

            break;
        }

        case ANSWER_UNKNOW:
        default:
            break;
    }
}

CodeError AddNewObject(BTree** Node)
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
    scanf(" %" MAX_SIZE_BUFFER "[^\n]", new_object);

    printf(MAGENTA "What's the difference between %s and %s? " RESET, new_object, (*Node)->data);
    scanf(" %" MAX_SIZE_BUFFER "[^\n]", question);

    BTree* NewNodeQuestion = nullptr;
    CodeError err = CreateNode(&NewNodeQuestion, question, (*Node)->parent);
    if (err != OK)
    {
        free(new_object);
        free(question);
        return err;
    } //TODO cpy question

    BTree* NewNode = nullptr;
    err = CreateNode(&NewNode, new_object, NewNodeQuestion);
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

    OldNode->parent = NewNodeQuestion;

    free(new_object);
    free(question);

    return OK;
}

CodeError SaveTreeToFile(BTree *Node, char *base_buf, int depth, int *cur_len, const size_t buffer_size)
{
    assert(base_buf != nullptr);
    if (Node == nullptr) return NODE_NULLPTR;

    *cur_len += snprintf(base_buf + *cur_len, buffer_size - (size_t)*cur_len, "%*s{\n", depth * 4, "");

    if (Node->left && Node->right)
    {
        *cur_len += snprintf(base_buf + *cur_len, buffer_size - (size_t)*cur_len, "%*s?%s?\n", (depth + 1) * 4, "", Node->data);

        SaveTreeToFile(Node->left, base_buf, depth + 1, cur_len, buffer_size);

        SaveTreeToFile(Node->right, base_buf, depth + 1, cur_len, buffer_size);
    }
    else
    {
        *cur_len += snprintf(base_buf + *cur_len, buffer_size - (size_t)*cur_len, "%*s<%s>\n", (depth + 1) * 4, "", Node->data);
    }

    *cur_len += snprintf(base_buf + *cur_len, buffer_size - (size_t)*cur_len, "%*s}\n", depth * 4, "");

    return OK;
}

CodeError SaveDatabase(Akinat *akn)
{
    if (akn->name_base == nullptr || akn->Root == nullptr)
    {
        LOG(LOGL_ERROR, "INVALID_ARGUMENT");
        return INVALID_ARGUMENT;
    }

    const size_t BASE_BUF_SIZE = 2048;
    char *base_buf = (char*)calloc(BASE_BUF_SIZE, sizeof(char));
    if (base_buf == nullptr)
    {
        LOG(LOGL_ERROR, "base_buf alloc error base buffer");
        return ALLOC_ERR;
    }

    int current_len = 0;
    int depth = 0;
    CodeError err = SaveTreeToFile(*(akn->Root), base_buf, depth, &current_len, BASE_BUF_SIZE);
    if (err != OK)
    {
        LOG(LOGL_ERROR, "FAILED_TO_SAVE_TREE");
        free(base_buf);
        return err;
    }

    FILE *file = fopen(akn->new_name_base, "w+");
    if (!file)
    {
        LOG(LOGL_ERROR, "FAILED_TO_OPEN_FILE");
        return FILE_NOT_OPEN;
    }

    fprintf(file, "%s", base_buf);
    fclose(file);

    free(base_buf);
    LOG(LOGL_INFO, "Database successfully saved to %s", akn->new_name_base);
    return OK;
}

AnswerType CheckAnswer(char *answer)
{
    assert(answer);

    if (strncmp(answer, "yes", 3) == 0)
        return ANSWER_YES;
    else if (strncmp(answer, "no", 2) == 0)
        return ANSWER_NO;
    else
        return ANSWER_UNKNOW;
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

CodeError ParseTree(BTree **Node, char **buffer, BTree *parent)
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

        CodeError err = ParseTree(Node, buffer, parent);
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

        char name[MAX_QUESTION] = "";
        if (sscanf(*buffer, "%[^>]>", name) != 1)
        {
            LOG(LOGL_ERROR, "INVALID_LEAF_FORMAT");
            return INVALID_FORMAT;
        }
        (*buffer) += strlen(name) + 1;

        LOG(LOGL_DEBUG, "Leaf node: %s", name);
        return CreateNode(Node, name, parent);
    }

    if (**buffer == '?')
    {
        (*buffer)++;

        char question[MAX_QUESTION] = "";
        if (sscanf(*buffer, "%[^?]?", question) != 1)
        {
            LOG(LOGL_ERROR, "INVALID_QUESTION_FORMAT");
            return INVALID_FORMAT;
        }
        (*buffer) += strlen(question) + 1;

        LOG(LOGL_DEBUG, "Question node: %s", question);
        CodeError err = CreateNode(Node, question, parent);
        if (err != OK) return err;

        err = ParseTree(&((*Node)->left), buffer, *Node);
        if (err != OK)
        {
            FreeTree(Node);
            return err;
        }

        err = ParseTree(&((*Node)->right), buffer, *Node);
        if (err != OK) {
            FreeTree(Node);
            return err;
        }

        return OK;
    } //TODO maybe make switch

    LOG(LOGL_ERROR, "UNKNOWN_SYMBOL: %c", **buffer);
    return INVALID_FORMAT;
}

void PrintDefinition(stack *stk, char *word)
{
    stackElem popped_elem_w = 0;
    BTree* current_node = nullptr;
    BTree* child_node = nullptr;

    printf(MAGENTA "%s: " RESET, word);

    stackPop(stk, &popped_elem_w);
    current_node = (BTree*)popped_elem_w;

    while (stk->size > 0)
    {
        child_node = current_node;
        stackPop(stk, &popped_elem_w);
        current_node = (BTree*)popped_elem_w;

        if (current_node->left == child_node)
        {
            printf(CYAN "%s " RESET, current_node->data);
        }
        else if (current_node->right == child_node)
        {
            printf(RED "not " CYAN "%s " RESET, current_node->data);
        }
        else
        {
            printf(RED "[invalid path] "RESET);
        }
    }
    printf("\n");
}

CodeError DefinitionObject(BTree *Node)
{
    assert(Node);

    struct stack stk = {};
    const size_t stack_size_default = 10;
    errorCode err = stackCtor(&stk, stack_size_default);
    if (err != STK_OK)
    {
        LOG(LOGL_ERROR, "StackCtor error");
        return ANOTHER_ERR;
    }

    char *word = (char*)calloc(SIZE_QUESTION + 1, sizeof(char));
    if (word == nullptr)
    {
        LOG(LOGL_ERROR, "Failed to allocate memory for a word\n");
        stackDtor(&stk);
        return ALLOC_ERR;
    }

    printf(YELLOW "Enter the word, you want define: " RESET);
    scanf("%" MAX_SIZE_BUFFER "s", word);
    printf("\n");


    if (FindWordNode(&stk, Node, word))
    {
        PrintDefinition(&stk, word);
    }
    else
    {
        printf(RED "Word not found in the tree.\n" RESET);
    }

    free(word);
    stackDtor(&stk);

    return OK;
}

bool FindWordNode(stack *stk, BTree *Node, const char *word)
{
    assert(Node);
    assert(stk);
    assert(word);

    stackPush(stk, (stackElem)Node);

    if (strcmp(Node->data, word) == 0)
    {
        printf(GREEN "Definition found: %s\n" RESET, Node->data);
        return true;
    }

    const bool is_found =
        ((Node->left  != nullptr) && FindWordNode(stk, Node->left, word)) ||
        ((Node->right != nullptr) && FindWordNode(stk, Node->right, word));

    if (is_found)
    {
        return true;
    }

    stackElem popped_elem = 0;
    stackPop(stk, &popped_elem);

    return false;
}

void ReverseStack(stack* stk)
{
    LOG(LOGL_INFO, "Start reverse stack");
    if (!stk || stk->size == 0) return;

    struct stack temp_stk = {};
    if (stackCtor(&temp_stk, (size_t)stk->size) != STK_OK)
    {
        LOG(LOGL_ERROR, "Failed to construct temporary stack");
        return;
    }

    stackElem elem = 0;

    while (stackPop(stk, &elem) == STK_OK)
    {
        stackPush(&temp_stk, elem);
    }

    free(stk->data);

    *stk = temp_stk;
    temp_stk.data = nullptr; //destroy stack
    //FIXME

    stackDtor(&temp_stk);
    LOG(LOGL_INFO, "End reverse stack");
}

CodeError FindDifference(BTree* Node)
{
    assert(Node);

    char* word1 = (char*)calloc(SIZE_QUESTION, sizeof(char));
    char* word2 = (char*)calloc(SIZE_QUESTION, sizeof(char));
    if (!word1 || !word2)
    {
        free(word1);
        free(word2);
        return ALLOC_ERR;
    }

    printf(YELLOW "Enter first word to compare: " RESET);
    scanf("%" MAX_SIZE_BUFFER "s", word1);
    printf(YELLOW "Enter second word to compare: " RESET);
    scanf("%" MAX_SIZE_BUFFER "s", word2);

    struct stack stk1 = {}, stk2 = {};
    if (stackCtor(&stk1, 10) != STK_OK || stackCtor(&stk2, 10) != STK_OK)
    {
        free(word1);
        free(word2);
        return ANOTHER_ERR;
    }

    if (!FindWordNode(&stk1, Node, word1) || !FindWordNode(&stk2, Node, word2))
    {
        printf(RED "One or both words not found in the tree.\n" RESET);
        free(word1);
        free(word2);
        stackDtor(&stk1);
        stackDtor(&stk2);
        return NODE_NULLPTR;
    }

    LOG(LOGL_INFO, "\n\n\n");
    ReverseStack(&stk1);
    LOG(LOGL_INFO, "\n\n\n");
    ReverseStack(&stk2);

    printf(GREEN "Comparison of '%s' and '%s':\n" RESET, word1, word2);

    stackElem elem1 = 0, elem2 = 0;
    BTree *Node1 = NULL, *Node2 = NULL;

    while (stackPop(&stk1, &elem1) == STK_OK && stackPop(&stk2, &elem2) == STK_OK)
    {
        Node1 = (BTree*)elem1;
        Node2 = (BTree*)elem2;

        if (Node1 != Node2)
        {
            printf(MAGENTA "Difference found:\n" RESET);
            printf(CYAN"- %s is %s%s\n"RESET, word1,
                   (Node1 == Node1->parent->left) ? "" : RED "not " CYAN,
                   Node1->parent->data);
            printf(CYAN"- %s is %s%s\n"RESET, word2,
                   (Node2 == Node2->parent->left) ? "" : RED "not " CYAN,
                   Node2->parent->data);
            break;
        }

    }

    free(word1);
    free(word2);
    stackDtor(&stk1);
    stackDtor(&stk2);
    return OK;
}


