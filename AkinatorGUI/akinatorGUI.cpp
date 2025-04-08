#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "tree.h"
#include "akinator.h"
#include "color.h"
#include "logger.h"
#include "stack.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

const size_t SIZE_QUESTION = 100;

bool CheckYesAnswer(char *answer);
bool CheckNoAnswer(char *answer);

void ProcessingModeGame(BTree **Node, const char *name_base);

void RunGuessingMode(sf::RenderWindow& window, sf::Font& font, BTree** Node, BTree **Root);
void AkinatorInit(BTree **Node, const char *name_base);
sf::Text CreateText(const sf::Font &font, const std::string &str, unsigned int size, sf::Color color, sf::Vector2f position);
void ShowResult(sf::RenderWindow &window, sf::Font &font, const std::string &message, sf::Color color);
sf::RectangleShape CreateButton(sf::Vector2f size, sf::Color color, sf::Vector2f position);
CodeError SaveDatabase(BTree **Root);
CodeError SaveTreeToFile(BTree *Node, char *base_buf, int depth, int *cur_len, const size_t buffer_size);
CodeError HandleNewNode(BTree **Node, char **buffer, BTree *parent);
CodeError HandleLeafNode(BTree **Node, char **buffer, BTree *parent);
CodeError HandleQuestionNode(BTree **Node, char **buffer, BTree *parent);
char *GetTextInput(sf::RenderWindow *window, sf::Font *font, const char *prompt);

void AkinatorInit(BTree **Node, const char *name_base)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Akinator");
    window.setPosition(sf::Vector2i(600, 50));

    BTree **Root = Node;

    sf::Font font;
    if (!font.loadFromFile("font.ttf"))
    {
        LOG(LOGL_ERROR, "font.ttf cant open");
        return;
    }

    sf::Text menuText = CreateText(font, "# Akinator\n"
                                             "(c) rAch, 2025\n\n"
                                             "Select the appropriate operating mode\n\n"
                                             "[g]: Guessing game\n"
                                             "[q]: Exit\n\n",
                                   24, sf::Color::Red, sf::Vector2f(200, 50));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::G)
                {
                    if (CreateTree(Node, name_base) == OK)
                    {
                        RunGuessingMode(window, font, Node, Root);
                    }
                }
                else if (event.key.code == sf::Keyboard::Q)
                {
                    window.close();
                }
            }
        }

        window.clear(sf::Color::White);
        window.draw(menuText);
        window.display();
    }
}

sf::Text CreateText(const sf::Font &font, const std::string &str, unsigned int size, sf::Color color, sf::Vector2f position)
{
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(position);
    return text;
}

sf::RectangleShape CreateButton(sf::Vector2f size, sf::Color color, sf::Vector2f position)
{
    sf::RectangleShape button(size);
    button.setFillColor(color);
    button.setPosition(position);
    return button;
}

char *GetTextInput(sf::RenderWindow *window, sf::Font *font, const char *prompt)
{
    assert(window != NULL);
    assert(font != NULL);

    static char inputBuffer[MAX_QUESTION] = "";
    memset(inputBuffer, 0, sizeof(inputBuffer));
    size_t inputLength = 0;

    sf::Text promptText = CreateText(*font, prompt, 24, sf::Color::Black, sf::Vector2f(50, 100));
    sf::Text inputText  = CreateText(*font, "", 24, sf::Color::Blue, sf::Vector2f(50, 200));

    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window->close();
                return NULL;
            }

            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode == '\b')
                {
                    if (inputLength > 0)
                    {
                        inputLength--;
                        inputBuffer[inputLength] = '\0';
                    }
                }
                else if (event.text.unicode == '\r')
                {
                    return inputBuffer; //FIXME
                }
                else if (inputLength < MAX_QUESTION - 1 &&
                         event.text.unicode >= 32 && event.text.unicode < 127)
                {
                    inputBuffer[inputLength++] = (char)event.text.unicode;
                    inputBuffer[inputLength] = '\0';
                }
                inputText.setString(inputBuffer);
            }
        }

        window->clear(sf::Color::White);
        window->draw(promptText);
        window->draw(inputText);
        window->display();
    }

    return NULL;
}

void RunGuessingMode(sf::RenderWindow &window, sf::Font &font, BTree **Node, BTree **Root)
{
    assert(Node != nullptr);

    sf::Text questionText = CreateText(font, "It's " + std::string((*Node)->data) + "?", 30, sf::Color::Black, sf::Vector2f(200, 50));

    sf::Texture texture;
    if (!texture.loadFromFile("images/" + std::string((*Node)->data) + ".jpg"))
    {
        if (!texture.loadFromFile("images/upload.png"))
        {
            LOG(LOGL_ERROR, "Failed to load image: %s", (*Node)->data);
        }

    }

    sf::Sprite sprite(texture);
    sprite.setPosition(200, 100);

    sf::RectangleShape yesButton = CreateButton(sf::Vector2f(150, 50), sf::Color::Green, sf::Vector2f(200, 400));
    sf::Text yesText = CreateText(font, "Yes", 24, sf::Color::White, sf::Vector2f(230, 410));

    sf::RectangleShape noButton = CreateButton(sf::Vector2f(150, 50), sf::Color::Red, sf::Vector2f(400, 400));
    sf::Text noText = CreateText(font, "No", 24, sf::Color::White, sf::Vector2f(460, 410));

    bool answered = false;
    while (window.isOpen() && !answered)
    {
        window.clear(sf::Color::White);
        window.draw(questionText);
        window.draw(sprite);
        window.draw(yesButton);
        window.draw(yesText);
        window.draw(noButton);
        window.draw(noText);
        window.display();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return;
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (yesButton.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y))
                {
                    answered = true;
                    if ((*Node)->left)
                    {
                        RunGuessingMode(window, font, &(*Node)->left, Root);
                    }
                    else
                    {
                        ShowResult(window, font, "I guessed right! It's " + std::string((*Node)->data), sf::Color::Green);
                    }
                }
                else if (noButton.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y))
                {
                    answered = true;
                    if ((*Node)->right)
                    {
                        RunGuessingMode(window, font, &(*Node)->right, Root);
                    }
                    else
                    {
                        ShowResult(window, font, "I don't know what it is :(", sf::Color::Red);

                        #if 1
                        char *new_object_input = GetTextInput(&window, &font, "What was it?");
                        if (new_object_input == NULL)
                        {
                            LOG(LOGL_ERROR, "new_object error read");
                            return;
                        }

                        char *new_object = strdup(new_object_input);
                        if (new_object == NULL)
                        {
                            LOG(LOGL_ERROR, "strdup failed");
                            return;
                        }

                        char prompt[MAX_QUESTION] = "";
                        snprintf(prompt, MAX_QUESTION, "What distinguishes %s from %s?", new_object, (*Node)->data);

                        char *newQuestion_input = GetTextInput(&window, &font, prompt);
                        if (newQuestion_input == NULL)
                        {
                            LOG(LOGL_ERROR, "newQuestion error read");
                            free(new_object);
                            return;
                        }

                        char *newQuestion = strdup(newQuestion_input);
                        if (newQuestion == NULL)
                        {
                            LOG(LOGL_ERROR, "strdup failed");
                            free(new_object);
                            return;
                        }

                        //printf(CYAN "%s-%s\n" RESET, new_object, newQuestion);

                        BTree *NewNodeQuestion = nullptr;
                        if (CreateNode(&NewNodeQuestion, newQuestion, (*Node)->parent) != OK)
                        {
                            LOG(LOGL_ERROR, "Failed to create old object node");

                            return;
                        }

                        BTree *NewNode = nullptr;
                        if (CreateNode(&NewNode, new_object, NewNodeQuestion) != OK)
                        {
                            LOG(LOGL_ERROR, "Failed to create new object node");

                            return;
                        }

                        BTree* OldNode = *Node;

                        *Node = NewNodeQuestion;
                        NewNodeQuestion->left = NewNode;
                        NewNodeQuestion->right = OldNode;

                        OldNode->parent = NewNodeQuestion;

                        printf("<%p>\n", *Root);
                        TreeDumpDot(*Root);
                        SaveDatabase(Root);

                        free(new_object);
                        free(newQuestion);

                        ShowResult(window, font, "Thanks! I've learned something new!", sf::Color::Blue);
                        #endif
                    }
                }
            }
        }
    }
}

void ShowResult(sf::RenderWindow &window, sf::Font &font, const std::string &message, sf::Color color)
{
    sf::Text resultText = CreateText(font, message, 30, color, sf::Vector2f(100, 250));

    window.clear(sf::Color::White);
    window.draw(resultText);
    window.display();
    sf::sleep(sf::seconds(2));
}

CodeError CreateTree(BTree **Node, const char *name_base)
{
    assert(Node != nullptr);

    size_t file_size = 0;
    char *base_buffer = ReadBaseToBuffer(name_base, &file_size);
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

CodeError HandleNewNode(BTree **Node, char **buffer, BTree *parent)
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
        LOG(LOGL_ERROR, "MISSING_CLOSING_BRACE, got '%c'", **buffer);
        FreeTree(Node);
        return INVALID_FORMAT;
    }

    (*buffer)++;
    LOG(LOGL_DEBUG, "Found }");

    return OK;
}

CodeError HandleLeafNode(BTree **Node, char **buffer, BTree *parent)
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

CodeError HandleQuestionNode(BTree **Node, char **buffer, BTree *parent)
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
    if (err != OK)
    {
        FreeTree(Node);
        return err;
    }

    return OK;
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

    switch (**buffer)
    {
        case '{':
            return HandleNewNode(Node, buffer, parent);

        case '<':
            return HandleLeafNode(Node, buffer, parent);

        case '?':
            return HandleQuestionNode(Node, buffer, parent);

        default:
        {
            LOG(LOGL_ERROR, "UNKNOWN_SYMBOL: %c", **buffer);
            return INVALID_FORMAT;
        }
    }

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

CodeError SaveDatabase(BTree **Root)
{
    if (Root == nullptr)
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
    CodeError err = SaveTreeToFile(*Root, base_buf, depth, &current_len, BASE_BUF_SIZE);
    if (err != OK)
    {
        LOG(LOGL_ERROR, "FAILED_TO_SAVE_TREE");
        free(base_buf);
        return err;
    }

    FILE *file = fopen("base.txt", "w+");
    if (!file)
    {
        LOG(LOGL_ERROR, "FAILED_TO_OPEN_FILE");
        return FILE_NOT_OPEN;
    }

    fprintf(file, "%s", base_buf);
    fclose(file);

    free(base_buf);
    LOG(LOGL_INFO, "Database successfully saved");
    return OK;
}
