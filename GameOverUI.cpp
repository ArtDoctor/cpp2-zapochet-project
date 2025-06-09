#include "GameOverUI.h"
#include <iostream>

GameOverUI::GameOverUI() {
    // Attempt to load the font for UI text
    if (!font.loadFromFile("DejaVuSans.ttf")) {
        // Fallback font loading or error handling
        // For now, print an error and exit
        std::cerr << "Error: Could not load font DejaVuSans.ttf" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    gameOverText.setFont(font);
    gameOverText.setString("Game Over!");
    gameOverText.setCharacterSize(30);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
    gameOverText.setPosition(200.0f, 100.0f);

    restartButton.setSize(sf::Vector2f(150.0f, 50.0f));
    restartButton.setPosition(125.0f, 150.0f);
    restartButton.setFillColor(sf::Color::Blue);
    restartButton.setOutlineThickness(2.0f);
    restartButton.setOutlineColor(sf::Color::White);

    restartText.setFont(font);
    restartText.setString("Restart");
    restartText.setCharacterSize(20);
    restartText.setFillColor(sf::Color::White);
    sf::FloatRect restartTextBounds = restartText.getLocalBounds();
    restartText.setOrigin(restartTextBounds.width / 2.0f, restartTextBounds.height / 2.0f);
    restartText.setPosition(200.0f, 175.0f);
}

void GameOverUI::openWindow() {
    gameOverWindow.create(sf::VideoMode(400, 300), "Game Over");
    gameOverWindow.setFramerateLimit(60);
}

void GameOverUI::closeWindow() {
    if (gameOverWindow.isOpen()) {
        gameOverWindow.close();
    }
}

bool GameOverUI::isWindowOpen() const {
    return gameOverWindow.isOpen();
}

bool GameOverUI::pollEvent(sf::Event& event) {
    return gameOverWindow.pollEvent(event);
}

bool GameOverUI::isRestartClicked(float mouseX, float mouseY) {
    sf::FloatRect buttonBounds = restartButton.getGlobalBounds();
    return buttonBounds.contains(mouseX, mouseY);
}

void GameOverUI::render() {
    if (gameOverWindow.isOpen()) {
        gameOverWindow.clear(sf::Color::Black);
        gameOverWindow.draw(gameOverText);
        gameOverWindow.draw(restartButton);
        gameOverWindow.draw(restartText);
        gameOverWindow.display();
    }
}