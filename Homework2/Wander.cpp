// Wander.cpp

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <cmath>
#include <SteeringBehavior.hpp>

sf::RenderWindow window(sf::VideoMode(640, 480), "Homework2 Wander");

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

// Function to generate a random position within a specified range (authored with chatgpt)
sf::Vector2f getRandomPosition(float minX, float maxX, float minY, float maxY)
{
    float randomX = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
    float randomY = minY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY - minY)));
    return sf::Vector2f(randomX, randomY);
}

float getDistance(sf::Vector2f &character, sf::Vector2f &goal)
{

    float distance_to_goal = std::sqrt((goal.x - character.x) * (goal.x - character.x) +
                                       (goal.y - character.y) * (goal.y - character.y));

    return distance_to_goal;
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr))); // Seed the random number generator
    float POSITION_MATCH_THRESHOLD = 150.f;
    // float POSITION_MATCH_THRESHOLD = 20.f;


    CustomSprite sprite;
    SteeringData character;
    SteeringData goal;

    goal.position = character.position; // setting initial goal position as the sprite's position itself
    character.orientation = 0.f;

    sf::Clock clock;
    PositionMatchingBehavior positionMatch;
    OrientationMatchingBehavior orientationMatch;
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

        sf::Time dt = clock.restart();

        // if (goal.position == sf::Vector2f() || getDistance(character.position, goal.position) < POSITION_MATCH_THRESHOLD || dt.asSeconds() > 0.0040)
        if (goal.position == sf::Vector2f() || getDistance(character.position, goal.position) < POSITION_MATCH_THRESHOLD)

        {
            goal.position = getRandomPosition(0.f, window.getSize().x, 0.f, window.getSize().y);
            float dx = goal.position.x - character.position.x;
            float dy = goal.position.y - character.position.y;
            goal.orientation = atan2(dy, dx);
        }

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
}