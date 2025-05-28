#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>

class Button {
    private:
        sf::RectangleShape buttonShape;
        sf::Text buttonText;
        sf::Font font;

    public:
        Button(float x, float y, float width, float height, const std::string& text);
        void draw(sf::RenderWindow& window);
        bool isClicked(sf::Vector2i mousePosition);
};

#endif // BUTTON_H
