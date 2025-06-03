#ifndef TERRAIN_H
#define TERRAIN_H

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>
#include <random>

const float SEGMENT_LENGTH = 1000.0f;
const float GENERATE_THRESHOLD = 500.0f;

class Terrain {
public:
    Terrain(b2World* world);
    void extendIfNeeded(float bikeX, std::mt19937& gen);
    void render(sf::RenderWindow& window);
    b2Body* getBody() const { return ground; }

private:
    b2Body* ground;
    std::vector<b2Vec2> points;
    std::vector<sf::Vertex> terrainVisual;
    float endX;
    float baseY;

    std::vector<b2Vec2> generatePath(float startX, float endX, float step, int pathType, float startY);
};

#endif // TERRAIN_H