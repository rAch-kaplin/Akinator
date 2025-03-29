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

void RunGuessingMode(sf::RenderWindow& window, sf::Font& font, BTree** Node);
void AkinatorInit(BTree **Node, const char *name_base);
sf::Text CreateText(const sf::Font &font, const std::string &str, unsigned int size, sf::Color color, sf::Vector2f position);
void ShowResult(sf::RenderWindow &window, sf::Font &font, const std::string &message, sf::Color color);
sf::RectangleShape CreateButton(sf::Vector2f size, sf::Color color, sf::Vector2f position);

void AkinatorInit(BTree **Node, const char *name_base)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Akinator");
    window.setPosition(sf::Vector2i(600, 50));

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


void RunGuessingMode(sf::RenderWindow &window, sf::Font &font, BTree **Node)
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
                        RunGuessingMode(window, font, &(*Node)->left);
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
                        RunGuessingMode(window, font, &(*Node)->right);
                    }
                    else
                    {
                        ShowResult(window, font, "I don't know what it is :(", sf::Color::Red);
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
    }

    LOG(LOGL_ERROR, "UNKNOWN_SYMBOL: %c", **buffer);
    return INVALID_FORMAT;
}
