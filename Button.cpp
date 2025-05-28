#include <SFML/Graphics.hpp>
#include "Button.h"
#include <iostream>

Button::Button(float x, float y, float width, float height, const std::string& text) {
    // Load font
    if (!font.loadFromFile("DejaVuSans.ttf")) { // Replace with your font file
        std::cerr << "Error loading font" << std::endl;
    }

    // Setup button shape
    buttonShape.setPosition(x, y);
    buttonShape.setSize(sf::Vector2f(width, height));
    buttonShape.setFillColor(sf::Color::Blue);

    // Setup button text
    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(24);
    buttonText.setFillColor(sf::Color::White);

    // Center the text on the button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(textBounds.left + textBounds.width / 2.0f, 
                            textBounds.top + textBounds.height / 2.0f);
    buttonText.setPosition(x + width / 2.0f, y + height / 2.0f);
}

// Draw the button
void Button::draw(sf::RenderWindow& window) {
    window.draw(buttonShape);
    window.draw(buttonText);
}

// Check if button is clicked
bool Button::isClicked(sf::Vector2i mousePosition) {
    return buttonShape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition));
}