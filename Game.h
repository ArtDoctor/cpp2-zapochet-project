#ifndef GAME_H
#define GAME_H

#include "Bicycle.h"
#include "Terrain.h"
#include "GameOverUI.h"
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <random>

const int SCREEN_HEIGHT = 700;
const int SCREEN_WIDTH = 1500;

class Game {
public:
    Game();
    void run();

private:
    // Core game components
    sf::RenderWindow window;
    sf::View view;
    b2World world;
    std::mt19937 gen;
    bool isGameOver;

    // Game objects
    Bicycle bike;
    Terrain terrain;
    GameOverUI gameOverUI;

    // Helper functions
    void handleInput();
    void updatePhysics();
    void updateVisuals();
    void checkGameOver();
    void render();
};

#endif // GAME_H