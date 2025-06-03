#include "Game.h"
#include <cmath>
#include <iostream>

Game::Game()
    : window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Bicycle on Wavy Terrain")
    , view(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT))
    , world(b2Vec2(0.0f, 9.8f))
    , bike(&world)
    , terrain(&world)
    , gameOverUI()
    , isGameOver(false)
{
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(60);
    
    std::random_device rd;
    gen = std::mt19937(rd());
}

void Game::run() {
    while (window.isOpen()) {
        handleInput();
        
        if (!isGameOver) {
            updatePhysics();
            updateVisuals();
            checkGameOver();
        }

        render();
    }
}

void Game::handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            gameOverUI.closeWindow();
        }
    }

    if (gameOverUI.isWindowOpen()) {
        while (gameOverUI.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                gameOverUI.closeWindow();
                isGameOver = false;
                bike.reset();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (gameOverUI.isRestartClicked(event.mouseButton.x, event.mouseButton.y)) {
                    gameOverUI.closeWindow();
                    isGameOver = false;
                    bike.reset();
                }
            }
        }
    }
}

void Game::updatePhysics() {
    bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    bike.updatePhysics(spacePressed);
    world.Step(1.0f / 60.0f, 8, 3);
}

void Game::updateVisuals() {
    b2Vec2 pos = bike.getPosition();
    bike.updateVisuals();
    terrain.extendIfNeeded(pos.x * SCALE, gen);
    
    sf::Vector2f target(pos.x * SCALE, 300.0f);
    sf::Vector2f current = view.getCenter();
    float smoothing = 0.1f;
    view.setCenter(current + (target - current) * smoothing);
}

void Game::checkGameOver() {
    b2Vec2 pos = bike.getPosition();
    bool frameHitGround = false;

    for (b2ContactEdge* edge = bike.getBody()->GetContactList(); edge; edge = edge->next) {
        b2Contact* contact = edge->contact;
        if (contact->IsTouching()) {
            b2Fixture* bikeFixture = nullptr;
            b2Fixture* groundFixture = nullptr;

            if (contact->GetFixtureA()->GetBody() == bike.getBody()) {
                bikeFixture = contact->GetFixtureA();
                groundFixture = contact->GetFixtureB();
            } else if (contact->GetFixtureB()->GetBody() == bike.getBody()) {
                bikeFixture = contact->GetFixtureB();
                groundFixture = contact->GetFixtureA();
            }

            if (bikeFixture && groundFixture && groundFixture->GetBody() == terrain.getBody()) {
                uintptr_t userData = bikeFixture->GetUserData().pointer;
                if (userData == 1) {
                    frameHitGround = true;
                    std::cout << "Game Over: Bike frame hit ground" << std::endl;
                    break;
                } else if (userData == 2) {
                }
            }
        }
    }

    if (pos.y * SCALE > SCREEN_HEIGHT || frameHitGround) {
        if (pos.y * SCALE > SCREEN_HEIGHT) {
            std::cout << "Game Over: Bike fell off screen (y = " << pos.y * SCALE << ")" << std::endl;
        }
        isGameOver = true;
        gameOverUI.openWindow();
    }
}

void Game::render() {
    window.setView(view);
    window.clear(sf::Color::Black);
    terrain.render(window);
    bike.render(window);
    window.display();
    gameOverUI.render();
}