// SteeringBehavior.hpp
#pragma once

#include <SFML/System/Vector2.hpp>

class SteeringData
{
public:
    sf::Vector2f position;
    float orientation;
    sf::Vector2f velocity;
    float rotation;
    sf::Vector2f linearAcceleration;
    float angularAcceleration;

    SteeringData()
    {
        position = sf::Vector2f();
        orientation = 0.f;
        velocity = sf::Vector2f();
        rotation = 0.f;
        linearAcceleration = sf::Vector2f();
        angularAcceleration = 0.f;
    }

    SteeringData(sf::Vector2f p, float o, sf::Vector2f v, float r, sf::Vector2f la, float anga)
    {
        position = p;
        orientation = o;
        velocity = v;
        rotation = r;
        linearAcceleration = la;
        angularAcceleration = anga;
    }
};
