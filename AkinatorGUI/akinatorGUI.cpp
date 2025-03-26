#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "tree.h"
#include "akinator.h"
#include "color.h"
#include "logger.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

const size_t SIZE_QUESTION = 100;

bool CheckYesAnswer(char *answer);
bool CheckNoAnswer(char *answer);

void ProcessingModeGame(BTree **Node, const char *name_base);

void RunGuessingMode(sf::RenderWindow& window, sf::Font& font, BTree** Node);

void AkinatorInit(BTree **Node, const char *name_base)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Akinator");

    sf::Font font;
    if (!font.loadFromFile("font.ttf"))
    {
        LOG(LOGL_ERROR, "font.ttf cant open");
        return;
    }

    sf::Text menuText;
    menuText.setFont(font);
    menuText.setString("# Akinator\n"
                      "(c) rAch, 2025\n\n"
                      "Select the appropriate operating mode\n\n"
                      "[g]: Guessing game\n"
                      "[q]: Exit\n\n");
    menuText.setCharacterSize(24);
    menuText.setFillColor(sf::Color::Red);
    menuText.setStyle(sf::Text::Bold);

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
                        RunGuessingMode(window, font, Node);
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

void RunGuessingMode(sf::RenderWindow& window, sf::Font& font, BTree** Node)
{
    sf::Text questionText;
    questionText.setFont(font);
    questionText.setString("Is it " + std::string((*Node)->data) + "?");
    questionText.setCharacterSize(30);
    questionText.setFillColor(sf::Color::Black);
    questionText.setPosition(50, 50);

    sf::Text promptText;
    promptText.setFont(font);
    promptText.setString("(Y - Yes, N - No, Q - Quit)");
    promptText.setCharacterSize(20);
    promptText.setFillColor(sf::Color::Black);
    promptText.setPosition(50, 100);

    bool answered = false;
    while (window.isOpen() && !answered)
    {
        window.clear(sf::Color::White);
        window.draw(questionText);
        window.draw(promptText);
        window.display();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return;
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Y)
                {
                    if ((*Node)->left)
                    {
                        RunGuessingMode(window, font, &(*Node)->left);
                    }
                    else
                    {
                        sf::Text resultText;
                        resultText.setFont(font);
                        resultText.setString("I guessed it! It's " + std::string((*Node)->data));
                        resultText.setCharacterSize(30);
                        resultText.setFillColor(sf::Color::Green);
                        resultText.setPosition(100, 150);

                        window.clear(sf::Color::White);
                        window.draw(resultText);
                        window.display();
                        sf::sleep(sf::seconds(3));
                    }
                    answered = true;
                }
                else if (event.key.code == sf::Keyboard::N)
                {
                    if ((*Node)->right)
                    {
                        RunGuessingMode(window, font, &(*Node)->right);
                    }
                    else
                    {
                        sf::Text resultText;
                        resultText.setFont(font);
                        resultText.setString("I don't know what it is :(");
                        resultText.setCharacterSize(30);
                        resultText.setFillColor(sf::Color::Red);
                        resultText.setPosition(100, 150);

                        window.clear(sf::Color::White);
                        window.draw(resultText);
                        window.display();
                        sf::sleep(sf::seconds(3));
                    }
                    answered = true;
                }
                else if (event.key.code == sf::Keyboard::Q)
                {
                    return;
                }
            }
        }
    }
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
