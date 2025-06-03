#ifndef GAMEOVERUI_H
#define GAMEOVERUI_H

#include <SFML/Graphics.hpp>

class GameOverUI {
public:
    GameOverUI();
    void openWindow();
    void closeWindow();
    bool isWindowOpen() const;
    bool pollEvent(sf::Event& event);
    bool isRestartClicked(float mouseX, float mouseY);
    void render();

private:
    sf::RenderWindow gameOverWindow;
    sf::Font font;
    sf::Text gameOverText;
    sf::RectangleShape restartButton;
    sf::Text restartText;
};

#endif // GAMEOVERUI_H