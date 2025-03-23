#include <assert.h>

#include "tree.h"
#include "akinator.h"

const size_t SIZE_QUESTION = 100;

bool CheckYesAnswer(char *answer);
bool CheckNoAnswer(char *answer);

void Akinator(BTree* Node)
{
    if (Node == nullptr) return;

    char *answer = (char*)calloc(SIZE_QUESTION, sizeof(char));
    assert(answer);

    printf("It's %s? (yes/no): ", Node->data);
    scanf("%99s", answer);

    if (CheckYesAnswer(answer) && Node->left)
    {
        Akinator(Node->left);
    }

    else if (CheckNoAnswer(answer) && Node->right)
    {
        Akinator(Node->right);
    }

    else
    {
        if (CheckYesAnswer(answer))
        {
            printf("I FIND THE ELEMENT\n");
            free(answer);
            return;
        }
        else if (CheckNoAnswer(answer))
        {
            printf("I DO NOT FIND, LETS WRITE IT\n");
            free(answer);
            return;
        }
        else
        {
            printf("INCORRECT INPUT, PLEASE, TRY AGAIN\n");
            Akinator(Node);
            return;
        }
    }

    free(answer);
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
