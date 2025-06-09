#include "Bicycle.h"
#include <cmath>

Bicycle::Bicycle(b2World* world) {
    // Define the bike body in Box2D
    b2BodyDef bikeDef;
    bikeDef.type = b2_dynamicBody;
    bikeDef.position.Set(100.0f / SCALE, 300.0f / SCALE);
    bike = world->CreateBody(&bikeDef);

    // Create the bike frame as a rectangle
    b2PolygonShape bikeFrameShape;
    bikeFrameShape.SetAsBox(30.0f / SCALE, 8.0f / SCALE);
    b2FixtureDef bikeFrameFixture;
    bikeFrameFixture.shape = &bikeFrameShape;
    bikeFrameFixture.density = 0.5f;
    bikeFrameFixture.friction = FRICTION;
    bikeFrameFixture.userData.pointer = 1; // 1 = frame
    bike->CreateFixture(&bikeFrameFixture);

    // Create the front wheel as a circle
    b2CircleShape frontWheelShape;
    frontWheelShape.m_radius = 15.0f / SCALE;
    frontWheelShape.m_p.Set(20.0f / SCALE, 8.0f / SCALE);
    b2FixtureDef frontWheelFixture;
    frontWheelFixture.shape = &frontWheelShape;
    frontWheelFixture.density = 0.3f;
    frontWheelFixture.friction = FRICTION * 2;
    frontWheelFixture.userData.pointer = 2; // 2 = wheel
    bike->CreateFixture(&frontWheelFixture);

    // Create the rear wheel as a circle
    b2CircleShape rearWheelShape;
    rearWheelShape.m_radius = 15.0f / SCALE;
    rearWheelShape.m_p.Set(-20.0f / SCALE, 8.0f / SCALE);
    b2FixtureDef rearWheelFixture;
    rearWheelFixture.shape = &rearWheelShape;
    rearWheelFixture.density = 0.3f;
    rearWheelFixture.friction = FRICTION * 2;
    rearWheelFixture.userData.pointer = 2; // 2 = wheel
    bike->CreateFixture(&rearWheelFixture);

    // Set up SFML shapes for rendering
    frontWheel = sf::CircleShape(15.0f);
    frontWheel.setOrigin(15.0f, 10.0f);
    frontWheel.setFillColor(sf::Color::Red);

    rearWheel = sf::CircleShape(15.0f);
    rearWheel.setOrigin(15.0f, 10.0f);
    rearWheel.setFillColor(sf::Color::Red);

    bikeFrame = sf::RectangleShape(sf::Vector2f(60.0f, 20.0f));
    bikeFrame.setOrigin(30.0f, 15.0f);
    bikeFrame.setFillColor(sf::Color::Blue);

    accumulatedAngle = 0.0f;
    lastAngle = 0.0f;
}

void Bicycle::reset() {
    bike->SetTransform(b2Vec2(100.0f / SCALE, 300.0f / SCALE), 0.0f);
    bike->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    bike->SetAngularVelocity(0.0f);
    accumulatedAngle = 0.0f;
    lastAngle = 0.0f;
}

bool Bicycle::updatePhysics(bool spacePressed) {
    // Update the bike's physics based on user input
    b2Vec2 velocity = bike->GetLinearVelocity();
    if (spacePressed) {
        // If the bike is moving vertically, apply torque to rotate it
        if (std::abs(velocity.y) > 0.5f) {
            bike->ApplyTorque(-ROTATION_TORQUE, true);
            float angularVelocity = bike->GetAngularVelocity();
            if (angularVelocity < -MAX_ANGULAR_VELOCITY) {
                bike->SetAngularVelocity(-MAX_ANGULAR_VELOCITY);
            }
        }
        // Apply forward force if not at max speed
        if (velocity.x < MAX_SPEED) {
            bike->ApplyForceToCenter(b2Vec2(ACCELERATION_FORCE, 0.0f), true);
        }
    }

    if (!spacePressed) {
        // Apply angular friction to slow down rotation
        float currentAngularVel = bike->GetAngularVelocity();
        bike->SetAngularVelocity(currentAngularVel * ANGULAR_FRICTION);
    }

    // --- Full rotation detection ---
    float currentAngle = bike->GetAngle();
    float delta = currentAngle - lastAngle;
    // Normalize delta to [-PI, PI]
    if (delta > PI) delta -= 2 * PI;
    if (delta < -PI) delta += 2 * PI;
    accumulatedAngle += delta;
    lastAngle = currentAngle;
    if (std::abs(accumulatedAngle) >= 2 * PI) {
        accumulatedAngle = 0.0f;
        return true;
    }
    return false;
}

void Bicycle::updateVisuals() {
    // Update the SFML shapes to match the Box2D body
    b2Vec2 pos = bike->GetPosition();
    float angle = bike->GetAngle();

    bikeFrame.setPosition(pos.x * SCALE, pos.y * SCALE);
    bikeFrame.setRotation(angle * 180 / PI);

    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    // Calculate wheel positions based on bike angle
    float frontWheelOffsetX = 20.0f * cosAngle;
    float frontWheelOffsetY = 20.0f * sinAngle;
    frontWheel.setPosition(pos.x * SCALE + frontWheelOffsetX, pos.y * SCALE + frontWheelOffsetY);
    frontWheel.setRotation(angle * 180 / PI);

    float rearWheelOffsetX = -20.0f * cosAngle;
    float rearWheelOffsetY = -20.0f * sinAngle;
    rearWheel.setPosition(pos.x * SCALE + rearWheelOffsetX, pos.y * SCALE + rearWheelOffsetY);
    rearWheel.setRotation(angle * 180 / PI);
}

void Bicycle::render(sf::RenderWindow& window) {
    window.draw(rearWheel);
    window.draw(frontWheel);
    window.draw(bikeFrame);
}