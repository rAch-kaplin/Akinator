#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "tree.h"

TreeErrors TreeDumpDot(BTree* Root)
{
    static int dump_counter = 0;
    const size_t PNG_NAME_SIZE = 30;
    const size_t BUFFER_SIZE = 16384;
    char* buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
    if (buffer == nullptr)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return ALLOC_ERR;
    }

    int buffer_len = 0;

    buffer_len += snprintf(buffer + buffer_len, BUFFER_SIZE - (size_t)buffer_len,
                           "digraph G {\n"
                           "\trankdir = HR;\n"
                           "\tbgcolor=\"#fcf0d2\";\n");

    buffer_len += GenerateGraph(Root, buffer, &buffer_len, BUFFER_SIZE);

    buffer_len += snprintf(buffer + buffer_len, BUFFER_SIZE - (size_t)buffer_len, "}\n");

    FILE* dump_file = fopen("GraphDump/dump.dot", "w+");
    if (!dump_file)
    {
        fprintf(stderr, "Failed to open dump.dot\n");
        free(buffer);
        return FILE_NOT_OPEN;
    }

    fprintf(dump_file, "%s", buffer);
    fclose(dump_file);
    free(buffer);

    char png_filename[PNG_NAME_SIZE];
    snprintf(png_filename, PNG_NAME_SIZE, "GraphDump/img/dump_%d.png", dump_counter++);

    char command[256];
    snprintf(command, sizeof(command), "dot -Tpng GraphDump/dump.dot -o %s", png_filename);
    system(command);

    return OK;
}

int GenerateGraph(BTree *Node, char* buffer, int* buffer_len, const size_t BUFFER_SIZE)
{
    if (!Node) return 0;

    *buffer_len += snprintf(buffer + *buffer_len, BUFFER_SIZE - (size_t)*buffer_len,
                            "\tnode%p [shape=plaintext; style=filled; color=\"#fcf0d2\"; label = <\n"
                            "\t\t         <table BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"6\" BGCOLOR=\"#bfb58f\" COLOR=\"#4d3d03\">\n"
                            "\t\t\t           <tr><td align='center' colspan='2'><FONT COLOR='#3a3a3a'><b>Node: %p</b></FONT></td></tr>\n"
                            "\t\t\t           <tr><td align='center' colspan='2'><FONT COLOR='#ba6504'>Data: <b>"FORMAT_ELEM"</b></FONT></td></tr>\n"
                            "\t\t\t           <tr>\n"
                            "\t\t\t               <td WIDTH='150' PORT='left'  align='center'><FONT COLOR='#006400'><b>Left: %p</b></FONT></td>\n"
                            "\t\t\t               <td WIDTH='150' PORT='right' align='center'><FONT COLOR='#8b0000'><b>Right: %p</b></FONT></td>\n"
                            "\t\t\t           </tr>\n"
                            "\t\t         </table> >];\n",
                            Node, Node, Node->data, Node->left, Node->right);

    if (Node->left)
    {
        *buffer_len += GenerateGraph(Node->left, buffer, buffer_len, BUFFER_SIZE);
        *buffer_len += snprintf(buffer + *buffer_len, BUFFER_SIZE - (size_t)*buffer_len,
                                 "\t node%p:left -> node%p [color=\"#006400\"style=bold; weight=1000; ];\n", Node, Node->left);
    }

    if (Node->right)
    {
        *buffer_len += GenerateGraph(Node->right, buffer, buffer_len, BUFFER_SIZE);
        *buffer_len += snprintf(buffer + *buffer_len, BUFFER_SIZE - (size_t)*buffer_len,
                                "\t node%p:right -> node%p [color=\"#8b0000\"style=bold; weight=1000; ];\n", Node, Node->right);
    }

    return 0;
}
