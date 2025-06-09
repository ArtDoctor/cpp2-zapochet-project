#include "Terrain.h"
#include "Bicycle.h"
#include <cmath>

Terrain::Terrain(b2World* world) : endX(3000.0f), baseY(350.0f) {
    float startX = 0.0f;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 2);

    // Generate initial terrain segments
    for (float x = startX; x < endX; x += SEGMENT_LENGTH) {
        int pathType = dis(gen);
        auto segmentPoints = generatePath(x, x + SEGMENT_LENGTH, 30.0f, pathType, baseY);
        for (const auto& p : segmentPoints) {
            points.push_back(p);
            terrainVisual.emplace_back(sf::Vector2f(p.x * SCALE, p.y * SCALE), sf::Color::Green);
        }
    }

    // Create the Box2D ground body and attach the terrain shape
    b2BodyDef groundDef;
    ground = world->CreateBody(&groundDef);
    if (!points.empty()) {
        b2ChainShape chain;
        chain.CreateChain(points.data(), static_cast<int32>(points.size()), b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 0.0f));
        ground->CreateFixture(&chain, 0.0f);
    }
}

std::vector<b2Vec2> Terrain::generatePath(float startX, float endX, float step, int pathType, float startY) {
    // Generate a vector of points for a terrain segment based on the path type
    std::vector<b2Vec2> points;
    for (float x = startX; x <= endX; x += step) {
        float y;
        switch (pathType) {
            case 0: // Wavy path
                y = startY + 50.0f * std::sin((x - startX) * 0.006f);
                break;
            case 1: // Hilly path
                y = startY + 80.0f * std::cos((x - startX) * 0.004f);
                break;
            case 2: // Steep waves
                y = startY + 100.0f * std::sin((x - startX) * 0.008f);
                break;
            default:
                y = startY;
                break;
        }
        points.emplace_back(x / SCALE, y / SCALE);
    }
    return points;
}

void Terrain::extendIfNeeded(float bikeX, std::mt19937& gen) {
    // If the bike is near the end of the current terrain, generate more terrain
    if (bikeX > endX - GENERATE_THRESHOLD) {
        std::uniform_int_distribution<> dis(0, 2);
        int pathType = dis(gen);
        float startX = endX;
        endX += SEGMENT_LENGTH;

        float lastY = points.empty() ? baseY : points.back().y * SCALE;
        auto segmentPoints = generatePath(startX, endX, 30.0f, pathType, lastY);

        for (const auto& p : segmentPoints) {
            points.push_back(p);
            terrainVisual.emplace_back(sf::Vector2f(p.x * SCALE, p.y * SCALE), sf::Color::Green);
        }

        // Properly destroy all fixtures attached to the ground body
        b2Fixture* fixture = ground->GetFixtureList();
        while (fixture) {
            b2Fixture* next = fixture->GetNext();
            ground->DestroyFixture(fixture);
            fixture = next;
        }
        // Recreate the chain shape with the new points
        if (!points.empty()) {
            b2ChainShape chain;
            chain.CreateChain(points.data(), static_cast<int32>(points.size()), b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 0.0f));
            ground->CreateFixture(&chain, 0.0f);
        }
    }
}

void Terrain::render(sf::RenderWindow& window) {
    // Render the terrain as a series of thick green lines
    float lineThickness = 5.0f;
    float step = 30.0f;
    for (size_t i = 0; i < terrainVisual.size() - 1; ++i) {
        sf::Vector2f p1 = terrainVisual[i].position;
        sf::Vector2f p2 = terrainVisual[i+1].position;

        sf::Vector2f direction_check = p2 - p1;
        float distance_check = std::sqrt(direction_check.x * direction_check.x + direction_check.y * direction_check.y);
        if (distance_check <= step * 1.5f) {
            sf::Vector2f direction = p2 - p1;
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159265f;

            sf::RectangleShape segment(sf::Vector2f(distance, lineThickness));
            segment.setOrigin(0, lineThickness / 2.0f);
            segment.setPosition(p1);
            segment.setRotation(angle);
            segment.setFillColor(sf::Color::Green);
            window.draw(segment);
        }
    }
}