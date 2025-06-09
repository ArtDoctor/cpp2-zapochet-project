#ifndef BICYCLE_H
#define BICYCLE_H

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

const float SCALE = 30.0f;
const float PI = 3.14159265f;
const float MAX_SPEED = 14.0f;
const float ACCELERATION_FORCE = 80.0f;
const float FRICTION = 0.3f;
const float ROTATION_TORQUE = 10.0f;
const float MAX_ANGULAR_VELOCITY = 7.0f;
const float ANGULAR_FRICTION = 0.95f;

class Bicycle {
public:
    Bicycle(b2World* world);
    void reset();
    // Returns true if a full rotation is completed this frame
    bool updatePhysics(bool spacePressed);
    void updateVisuals();
    void render(sf::RenderWindow& window);
    b2Body* getBody() const { return bike; }
    b2Vec2 getPosition() const { return bike->GetPosition(); }

private:
    b2Body* bike;
    sf::CircleShape frontWheel;
    sf::CircleShape rearWheel;
    sf::RectangleShape bikeFrame;
    float accumulatedAngle = 0.0f;
    float lastAngle = 0.0f;
};

#endif // BICYCLE_H