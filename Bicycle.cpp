#include "Bicycle.h"
#include <cmath>

Bicycle::Bicycle(b2World* world) {
    b2BodyDef bikeDef;
    bikeDef.type = b2_dynamicBody;
    bikeDef.position.Set(100.0f / SCALE, 300.0f / SCALE);
    bike = world->CreateBody(&bikeDef);

    b2PolygonShape bikeFrameShape;
    bikeFrameShape.SetAsBox(30.0f / SCALE, 8.0f / SCALE);
    b2FixtureDef bikeFrameFixture;
    bikeFrameFixture.shape = &bikeFrameShape;
    bikeFrameFixture.density = 0.5f;
    bikeFrameFixture.friction = FRICTION;
    bikeFrameFixture.userData.pointer = 1;
    bike->CreateFixture(&bikeFrameFixture);

    b2CircleShape frontWheelShape;
    frontWheelShape.m_radius = 15.0f / SCALE;
    frontWheelShape.m_p.Set(20.0f / SCALE, 8.0f / SCALE);
    b2FixtureDef frontWheelFixture;
    frontWheelFixture.shape = &frontWheelShape;
    frontWheelFixture.density = 0.3f;
    frontWheelFixture.friction = FRICTION * 2;
    frontWheelFixture.userData.pointer = 2;
    bike->CreateFixture(&frontWheelFixture);

    b2CircleShape rearWheelShape;
    rearWheelShape.m_radius = 15.0f / SCALE;
    rearWheelShape.m_p.Set(-20.0f / SCALE, 8.0f / SCALE);
    b2FixtureDef rearWheelFixture;
    rearWheelFixture.shape = &rearWheelShape;
    rearWheelFixture.density = 0.3f;
    rearWheelFixture.friction = FRICTION * 2;
    rearWheelFixture.userData.pointer = 2;
    bike->CreateFixture(&rearWheelFixture);

    frontWheel = sf::CircleShape(15.0f);
    frontWheel.setOrigin(15.0f, 10.0f);
    frontWheel.setFillColor(sf::Color::Red);

    rearWheel = sf::CircleShape(15.0f);
    rearWheel.setOrigin(15.0f, 10.0f);
    rearWheel.setFillColor(sf::Color::Red);

    bikeFrame = sf::RectangleShape(sf::Vector2f(60.0f, 20.0f));
    bikeFrame.setOrigin(30.0f, 15.0f);
    bikeFrame.setFillColor(sf::Color::Blue);
}

void Bicycle::reset() {
    bike->SetTransform(b2Vec2(100.0f / SCALE, 300.0f / SCALE), 0.0f);
    bike->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    bike->SetAngularVelocity(0.0f);
}

void Bicycle::updatePhysics(bool spacePressed) {
    b2Vec2 velocity = bike->GetLinearVelocity();
    if (spacePressed) {
        if (std::abs(velocity.y) > 0.5f) {
            bike->ApplyTorque(-ROTATION_TORQUE, true);
            float angularVelocity = bike->GetAngularVelocity();
            if (angularVelocity < -MAX_ANGULAR_VELOCITY) {
                bike->SetAngularVelocity(-MAX_ANGULAR_VELOCITY);
            }
        }
        if (velocity.x < MAX_SPEED) {
            bike->ApplyForceToCenter(b2Vec2(ACCELERATION_FORCE, 0.0f), true);
        }
    }

    if (!spacePressed) {
        float currentAngularVel = bike->GetAngularVelocity();
        bike->SetAngularVelocity(currentAngularVel * ANGULAR_FRICTION);
    }
}

void Bicycle::updateVisuals() {
    b2Vec2 pos = bike->GetPosition();
    float angle = bike->GetAngle();

    bikeFrame.setPosition(pos.x * SCALE, pos.y * SCALE);
    bikeFrame.setRotation(angle * 180 / PI);

    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

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