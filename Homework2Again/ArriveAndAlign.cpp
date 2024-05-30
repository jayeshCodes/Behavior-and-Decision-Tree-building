// ArriveAndAlign.cpp

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <cmath>
#include <SteeringBehavior.hpp>

sf::RenderWindow window(sf::VideoMode(640, 480), "Homework2 Arrive and Align");

class CustomSprite
{
public:
    sf::Sprite sprite;
    sf::Texture texture;

    CustomSprite()
    {
        if (!texture.loadFromFile("boid.png"))
        {
            std::cout << "texture not found";
        }

        sprite.setTexture(texture);
        sprite.setScale(0.05f, 0.05f);
        sprite.setOrigin(sprite.getLocalBounds().width / 2.f, sprite.getLocalBounds().height / 2.f);
        sprite.setPosition(320.f, 240.f);
        sprite.setRotation(0.f);
    }
};

class Crumb : public sf::CircleShape
{
public:
    Crumb()
    {
        this->setRadius(2.f);
        this->setFillColor(sf::Color::Blue);
    }
};

int main()
{
    CustomSprite sprite;
    SteeringData character;
    SteeringData goal;
    // float ANGLE_THRESHOLD = 5.f;
    // float POSITION_MATCH_THRESHOLD = 10.f;

    goal.position = character.position; // setting initial goal position as the sprite's position itself
    character.orientation = 0.f;

    sf::Clock clock;
    PositionMatchingBehavior positionMatch;
    OrientationMatchingBehavior orientationMatch;

    // bool orientationMatchComplete = false;
    // bool positionMatchComplete = false;
    std::vector<Crumb> breadcrumbs;

    sf::CircleShape circle;
    circle.setRadius(5); // Set the radius of the circle
    circle.setFillColor(sf::Color::Red);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            goal.position = sf::Vector2f(sf::Mouse::getPosition(window));
            float dx = goal.position.x - character.position.x;
            float dy = goal.position.y - character.position.y;
            goal.orientation = atan2(dy, dx);
        }

        sf::Time dt = clock.restart();

        character.position = sprite.sprite.getPosition();                     // Update character position
        character.orientation = sprite.sprite.getRotation() * (M_PI / 180.f); // Update character orientation

        character.velocity += positionMatch.calculateSteering(character, goal) * dt.asSeconds();
        character.position += character.velocity * dt.asSeconds();
        sprite.sprite.setPosition(character.position);

        character.rotation = orientationMatch.calculateSteering(character, goal).y * dt.asSeconds();
        character.orientation += character.rotation;
        sprite.sprite.setRotation(character.orientation * (180.f / M_PI));

        Crumb crumb;
        crumb.setPosition(sprite.sprite.getPosition());
        breadcrumbs.push_back(crumb);

        // Remove old breadcrumbs if necessary
        if (breadcrumbs.size() > 2000) // Limiting the number of breadcrumbs
        {
            breadcrumbs.erase(breadcrumbs.begin());
        }

        circle.setPosition(goal.position);

        window.clear(sf::Color::White);
        window.draw(sprite.sprite);
        window.draw(circle);
        for (const auto &crumb : breadcrumbs)
        {
            window.draw(crumb);
        }

        window.display();
    }

    return 0;
}
