#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>
#include <cmath>
#include <random>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


const float SCALE = 30.f;
const float PI = 3.14159265f;
const float MAX_SPEED = 14.f;
const float ACCELERATION_FORCE = 80.f;
const float FRICTION = 0.3f;
const float ROTATION_TORQUE = 10.f; // Torque for rotation when in air
const float MAX_ANGULAR_VELOCITY = 7.f; // Maximum rotation speed (radians per second)
const float ANGULAR_FRICTION = 0.95f; // Angular friction factor (0-1, lower = more friction)
const float SCREEN_HEIGHT = 600.f; // Screen height for game over condition
const float SEGMENT_LENGTH = 1000.f; // Length of each terrain segment
const float GENERATE_THRESHOLD = 500.f; // Generate new segment when bike is this far from end

// Function to generate different path types
std::vector<b2Vec2> generatePath(float startX, float endX, float step, int pathType, float startY) {
    std::vector<b2Vec2> points;
    for (float x = startX; x <= endX; x += step) {
        float y;
        switch (pathType) {
            case 0: // Wavy path
                y = startY + 50.f * std::sin((x - startX) * 0.006f);
                break;
            case 1: // Hilly path
                y = startY + 80.f * std::cos((x - startX) * 0.004f);
                break;
            case 2: // Steep waves
                y = startY + 100.f * std::sin((x - startX) * 0.008f);
                break;
            default:
                y = startY;
                break;
        }
        points.emplace_back(x / SCALE, y / SCALE);
    }
    return points;
}

// Function to reset the bike's state
void resetBike(b2Body* bike) {
    bike->SetTransform(b2Vec2(100.f / SCALE, 300.f / SCALE), 0.f);
    bike->SetLinearVelocity(b2Vec2(0.f, 0.f));
    bike->SetAngularVelocity(0.f);
}

// Function to extend terrain
void extendTerrain(b2Body* ground, std::vector<b2Vec2>& points, std::vector<sf::Vertex>& terrainVisual, float& endX, float baseY, std::mt19937& gen) {
    std::uniform_int_distribution<> dis(0, 2);
    int pathType = dis(gen);
    float startX = endX;
    endX += SEGMENT_LENGTH;
    
    // Get the Y position of the last point to ensure smooth connection
    float lastY = points.empty() ? baseY : points.back().y * SCALE;
    
    auto segmentPoints = generatePath(startX, endX, 30.f, pathType, lastY);
    for (const auto& p : segmentPoints) {
        points.push_back(p);
        terrainVisual.emplace_back(sf::Vector2f(p.x * SCALE, p.y * SCALE), sf::Color::Green);
    }
    // Update ground body
    ground->DestroyFixture(ground->GetFixtureList());
    if (!points.empty()) {
        b2ChainShape chain;
        chain.CreateChain(
            points.data(),
            static_cast<int32>(points.size()),
            b2Vec2(0.f, 0.f),
            b2Vec2(0.f, 0.f)
        );
        ground->CreateFixture(&chain, 0.f);
    }
}

int main() {
    // Main game window
    sf::RenderWindow window(sf::VideoMode(1200, 600), "Bicycle on Wavy Terrain");
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(60);

    sf::View view(sf::FloatRect(0, 0, 1200, 600));
    b2World world(b2Vec2(0.f, 9.8f)); // gravity

    // Random number generator for path selection
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 2); // Three path types

    // --- Wavy Ground using ChainShape ---
    std::vector<b2Vec2> points;
    std::vector<sf::Vertex> terrainVisual;

    float startX = 0.f;
    float endX = 3000.f;
    float step = 30.f;
    float baseY = 350.f;

    // Initial terrain generation
    for (float x = startX; x < endX; x += SEGMENT_LENGTH) {
        int pathType = dis(gen);
        auto segmentPoints = generatePath(x, x + SEGMENT_LENGTH, step, pathType, baseY);
        for (const auto& p : segmentPoints) {
            points.push_back(p);
            terrainVisual.emplace_back(sf::Vector2f(p.x * SCALE, p.y * SCALE), sf::Color::Green);
        }
    }

    // Create ground body
    b2BodyDef groundDef;
    b2Body* ground = world.CreateBody(&groundDef);

    if (!points.empty()) {
        b2ChainShape chain;
        chain.CreateChain(
            points.data(),
            static_cast<int32>(points.size()),
            b2Vec2(0.f, 0.f),
            b2Vec2(0.f, 0.f)
        );
        ground->CreateFixture(&chain, 0.f);
    }

    // --- Bicycle ---
    b2BodyDef bikeDef;
    bikeDef.type = b2_dynamicBody;
    bikeDef.position.Set(100.f / SCALE, 300.f / SCALE);
    b2Body* bike = world.CreateBody(&bikeDef);

    // Bike frame (main body)
    b2PolygonShape bikeFrameShape;
    bikeFrameShape.SetAsBox(30.f / SCALE, 8.f / SCALE);
    b2FixtureDef bikeFrameFixture;
    bikeFrameFixture.shape = &bikeFrameShape;
    bikeFrameFixture.density = 0.5f;
    bikeFrameFixture.friction = FRICTION;
    bikeFrameFixture.userData.pointer = 1; // Mark as frame
    bike->CreateFixture(&bikeFrameFixture);

    // Front wheel
    b2CircleShape frontWheelShape;
    frontWheelShape.m_radius = 15.f / SCALE;
    frontWheelShape.m_p.Set(20.f / SCALE, 8.f / SCALE);
    b2FixtureDef frontWheelFixture;
    frontWheelFixture.shape = &frontWheelShape;
    frontWheelFixture.density = 0.3f;
    frontWheelFixture.friction = FRICTION * 2;
    frontWheelFixture.userData.pointer = 2; // Mark as wheel
    bike->CreateFixture(&frontWheelFixture);

    // Rear wheel
    b2CircleShape rearWheelShape;
    rearWheelShape.m_radius = 15.f / SCALE;
    rearWheelShape.m_p.Set(-20.f / SCALE, 8.f / SCALE);
    b2FixtureDef rearWheelFixture;
    rearWheelFixture.shape = &rearWheelShape;
    rearWheelFixture.density = 0.3f;
    rearWheelFixture.friction = FRICTION * 2;
    rearWheelFixture.userData.pointer = 2; // Mark as wheel
    bike->CreateFixture(&rearWheelFixture);

    // Bike visuals: two circles (wheels) and a rectangle (frame)
    sf::CircleShape frontWheel(15.f);
    frontWheel.setOrigin(15.f, 10.f);
    frontWheel.setFillColor(sf::Color::Red); // Changed wheel color to red
    sf::CircleShape rearWheel(15.f);
    rearWheel.setOrigin(15.f, 10.f);
    rearWheel.setFillColor(sf::Color::Red); // Changed wheel color to red
    sf::RectangleShape bikeFrame(sf::Vector2f(60.f, 20.f));
    bikeFrame.setOrigin(30.f, 15.f);
    bikeFrame.setFillColor(sf::Color::Blue);

    // --- Game Over Window Setup ---
    sf::RenderWindow gameOverWindow;
    sf::Font font;
    if (!font.loadFromFile("DejaVuSans.ttf")) {
        // Fallback font loading (provide a valid font file path)
    }
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over!");
    gameOverText.setCharacterSize(30);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
    gameOverText.setPosition(200.f, 100.f);
    
    // Restart button
    sf::RectangleShape restartButton(sf::Vector2f(150.f, 50.f));
    restartButton.setPosition(125.f, 150.f);
    restartButton.setFillColor(sf::Color::Blue);
    restartButton.setOutlineThickness(2.f);
    restartButton.setOutlineColor(sf::Color::White);
    
    sf::Text restartText;
    restartText.setFont(font);
    restartText.setString("Restart");
    restartText.setCharacterSize(20);
    restartText.setFillColor(sf::Color::White);
    sf::FloatRect restartTextBounds = restartText.getLocalBounds();
    restartText.setOrigin(restartTextBounds.width / 2.f, restartTextBounds.height / 2.f);
    restartText.setPosition(200.f, 175.f);

    bool isGameOver = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                if (gameOverWindow.isOpen()) {
                    gameOverWindow.close();
                }
            }
        }

        // Handle events for game over window
        if (gameOverWindow.isOpen()) {
            while (gameOverWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    gameOverWindow.close();
                    isGameOver = false;
                    resetBike(bike);
                }
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(gameOverWindow);
                    sf::FloatRect buttonBounds = restartButton.getGlobalBounds();
                    
                    if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        gameOverWindow.close();
                        isGameOver = false;
                        resetBike(bike);
                    }
                }
            }
        }

        if (!isGameOver) {
            b2Vec2 velocity = bike->GetLinearVelocity();
            bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
            
            if (spacePressed) {
                // Check if bike is in air using vertical velocity
                if (std::abs(velocity.y) > 0.5f) {
                    // Apply counter-clockwise rotation when in air
                    bike->ApplyTorque(-ROTATION_TORQUE, true);
                    // Cap angular velocity
                    float angularVelocity = bike->GetAngularVelocity();
                    if (angularVelocity < -MAX_ANGULAR_VELOCITY) {
                        bike->SetAngularVelocity(-MAX_ANGULAR_VELOCITY);
                    }
                }
                // Always apply acceleration when space is pressed
                if (velocity.x < MAX_SPEED) {
                    bike->ApplyForceToCenter(b2Vec2(ACCELERATION_FORCE, 0.f), true);
                }
            }
            
            // Apply rotation friction when no input is pressed
            if (!spacePressed) {
                float currentAngularVel = bike->GetAngularVelocity();
                bike->SetAngularVelocity(currentAngularVel * ANGULAR_FRICTION);
            }

            world.Step(1.f / 60.f, 8, 3);

            b2Vec2 pos = bike->GetPosition();
            float angle = bike->GetAngle();

            // Update bike visuals - wheels properly attached to frame
            bikeFrame.setPosition(pos.x * SCALE, pos.y * SCALE);
            bikeFrame.setRotation(angle * 180 / PI);
            
            // Calculate wheel positions relative to the rotated frame
            float cosAngle = cos(angle);
            float sinAngle = sin(angle);
            
            // Front wheel position (relative to frame center)
            float frontWheelOffsetX = 20.f * cosAngle;
            float frontWheelOffsetY = 20.f * sinAngle;
            frontWheel.setPosition(pos.x * SCALE + frontWheelOffsetX, pos.y * SCALE + frontWheelOffsetY);
            frontWheel.setRotation(angle * 180 / PI);
            
            // Rear wheel position (relative to frame center)
            float rearWheelOffsetX = -20.f * cosAngle;
            float rearWheelOffsetY = -20.f * sinAngle;
            rearWheel.setPosition(pos.x * SCALE + rearWheelOffsetX, pos.y * SCALE + rearWheelOffsetY);
            rearWheel.setRotation(angle * 180 / PI);

            // Check for game over (bike frame touches ground or bike falls off screen)
            bool frameHitGround = false;
            
            // Check if bike frame specifically is colliding with ground
            for (b2ContactEdge* edge = bike->GetContactList(); edge; edge = edge->next) {
                b2Contact* contact = edge->contact;
                if (contact->IsTouching()) {
                    b2Fixture* bikeFixture = nullptr;
                    b2Fixture* groundFixture = nullptr;
                    
                    // Determine which fixture belongs to bike and which to ground
                    if (contact->GetFixtureA()->GetBody() == bike) {
                        bikeFixture = contact->GetFixtureA();
                        groundFixture = contact->GetFixtureB();
                    } else if (contact->GetFixtureB()->GetBody() == bike) {
                        bikeFixture = contact->GetFixtureB();
                        groundFixture = contact->GetFixtureA();
                    }
                    
                    // Check if it's the frame (userData == 1) hitting the ground
                    if (bikeFixture && groundFixture && 
                        groundFixture->GetBody() == ground &&
                        bikeFixture->GetUserData().pointer == 1) {
                        frameHitGround = true;
                        break;
                    }
                }
            }
            
            if (pos.y * SCALE > SCREEN_HEIGHT || frameHitGround) {
                isGameOver = true;
                gameOverWindow.create(sf::VideoMode(400, 300), "Game Over");
                gameOverWindow.setFramerateLimit(60);
            }

            // Extend terrain if bike is close to the end
            if (pos.x * SCALE > endX - GENERATE_THRESHOLD) {
                extendTerrain(ground, points, terrainVisual, endX, baseY, gen);
            }

            // Smooth camera follow
            sf::Vector2f target(pos.x * SCALE, 300.f);
            sf::Vector2f current = view.getCenter();
            float smoothing = 0.1f;
            view.setCenter(current + (target - current) * smoothing);
        }

        window.setView(view);
        window.clear(sf::Color::Black);

        // Draw thicker terrain using rectangles
        float lineThickness = 5.f;
        for (size_t i = 0; i < terrainVisual.size() - 1; ++i) {
            sf::Vector2f p1 = terrainVisual[i].position;
            sf::Vector2f p2 = terrainVisual[i+1].position;

            sf::Vector2f direction_check = p2 - p1;
            float distance_check = std::sqrt(direction_check.x * direction_check.x + direction_check.y * direction_check.y);
            if (distance_check <= step * 1.5f) {
                sf::Vector2f direction = p2 - p1;
                float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                float angle = std::atan2(direction.y, direction.x) * 180 / PI;

                sf::RectangleShape segment(sf::Vector2f(distance, lineThickness));
                segment.setOrigin(0, lineThickness / 2.f);
                segment.setPosition(p1);
                segment.setRotation(angle);
                segment.setFillColor(sf::Color::Green);
                window.draw(segment);
            }
        }

        // Draw bike components
        window.draw(rearWheel);
        window.draw(frontWheel);
        window.draw(bikeFrame);

        window.display();

        // Render game over window if open
        if (gameOverWindow.isOpen()) {
            gameOverWindow.clear(sf::Color::Black);
            gameOverWindow.draw(gameOverText);
            gameOverWindow.draw(restartButton);
            gameOverWindow.draw(restartText);
            gameOverWindow.display();
        }
    }

    return 0;
}