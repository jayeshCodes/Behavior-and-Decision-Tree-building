#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <cmath>
#include "SteeringBehavior.hpp" // Include your SteeringBehavior.hpp file

sf::RenderWindow window(sf::VideoMode(640, 480), "Homework2 Flocking");

sf::Vector2f getRandomPosition(float minX, float maxX, float minY, float maxY, const std::vector<sf::Vector2f> &occupiedPositions)
{
    sf::Vector2f randomPos;
    do
    {
        randomPos.x = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
        randomPos.y = minY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY - minY)));
    } while (std::find(occupiedPositions.begin(), occupiedPositions.end(), randomPos) != occupiedPositions.end());
    return randomPos;
}

sf::Vector2f getRandomVelocity(float minMagnitude, float maxMagnitude)
{
    float randomAngle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 * M_PI;
    float randomMagnitude = minMagnitude + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (maxMagnitude - minMagnitude));
    return sf::Vector2f(std::cos(randomAngle) * randomMagnitude, std::sin(randomAngle) * randomMagnitude);
}

float getOrientationFromVelocity(const sf::Vector2f &velocity)
{
    return std::atan2(velocity.y, velocity.x) * 180.f / M_PI;
}

float getRandomOffset(float minOffset, float maxOffset)
{
    return minOffset + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (maxOffset - minOffset));
}

class Crumb : public sf::CircleShape
{
public:
    Crumb()
    {
        this->setRadius(0.5f);
        this->setFillColor(sf::Color::Blue);
    }
};

class CustomSprite
{
public:
    sf::Sprite sprite;
    sf::Texture texture;
    SteeringData steering;

    CustomSprite()
    {
        std::vector<sf::Vector2f> occupiedPositions;
        sf::Vector2f randomPos = getRandomPosition(0.f, window.getSize().x, 0.f, window.getSize().y, occupiedPositions);
        for (int i = 0; i < 30; ++i)
        {
            occupiedPositions.push_back(randomPos);
        }

        if (!texture.loadFromFile("boid.png"))
        {
            std::cout << "texture not found";
        }

        sprite.setTexture(texture);
        sprite.setOrigin(sprite.getLocalBounds().width / 2.f, sprite.getLocalBounds().height / 2.f);
        sprite.setPosition(randomPos);
        sprite.setScale(0.01f, 0.01f);
        steering.position = randomPos;
        sprite.setRotation(0.f);
    }
};

int main()
{
    sf::Clock clock;
    CustomSprite boids[30];
    SteeringData steeringData[30];
    // CustomSprite sprite;
    for (int i = 0; i < 30; ++i)
    {
        steeringData[i].position = boids[i].sprite.getPosition();
        // steeringData[i].velocity = sf::Vector2f(100.f, 0.f);
        steeringData[i].velocity = getRandomVelocity(50.f, 150.f);
        steeringData[i].orientation = getOrientationFromVelocity(steeringData[i].velocity);
    }

    Align align;
    Cohesion cohesion;
    Separation separation;

    float maxSpeed = 200.f;
    std::vector<Crumb> breadcrumbs;

    while (window.isOpen())
    {
        sf::Event event;
        sf::Vector2f alignmentForce;
        sf::Vector2f cohesionForce;
        sf::Vector2f separationForce;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Time dt = clock.restart();

        // Calculate flocking behavior for each boid
        for (int i = 0; i < 30; i++)
        {
            SteeringData others[29]; // Array to hold other boids' data
            int index = 0;
            for (int j = 0; j < 30; j++)
            {
                if (i != j) // Exclude the current boid
                {
                    others[index] = steeringData[j];
                    index++;
                }
            }
            alignmentForce = align.alignForce(steeringData[i], others);
            cohesionForce = cohesion.cohesionForce(steeringData[i], others);
            separationForce = separation.separationForce(steeringData[i], others);

            if (steeringData[i].position.x < 0 + 10.f)
            {
                steeringData[i].position.x = window.getSize().x - 10.f; // Move to the right edge
                // steeringData[i].velocity.x = (-steeringData[i].velocity.x) + 100.f;
            }
            else if (steeringData[i].position.x > window.getSize().x - 10.f)
            {
                steeringData[i].position.x = 10.f; // Move to the left edge
                // steeringData[i].velocity.x = (-steeringData[i].velocity.x - 100.f);
            }

            if (steeringData[i].position.y < 0 + 10.f)
            {
                steeringData[i].position.y = window.getSize().y - 10.f; // Move to the bottom edge
                // steeringData[i].velocity.y = (-steeringData[i].velocity.y) + 100.f;
            }
            else if (steeringData[i].position.y > window.getSize().y - 10.f)
            {
                steeringData[i].position.y = 10.f; // Move to the top edge
                // steeringData[i].velocity.y = (-steeringData[i].velocity.y - 100.f);
            }
            // steeringData[i].velocity += 100.f * (0.0f * cohesionForce * dt.asSeconds() + 0.0f * alignmentForce * dt.asSeconds() + 1000.f * separationForce * dt.asSeconds());
            steeringData[i].velocity += 100.f * (0.8f * cohesionForce * dt.asSeconds() + 1.0f * alignmentForce * dt.asSeconds() + 1.5f * separationForce * dt.asSeconds()); //multiplying the final by 100 because the force is negligible otherwise
            // steeringData[i].velocity += 100.f * (100.0f * cohesionForce * dt.asSeconds() + 0.0f * alignmentForce * dt.asSeconds() + 0.f * separationForce * dt.asSeconds());

            float speed = std::sqrt(steeringData[i].velocity.x * steeringData[i].velocity.x + steeringData[i].velocity.y * steeringData[i].velocity.y);
            if (speed > maxSpeed)
            {
                steeringData[i].velocity = (steeringData[i].velocity / speed) * maxSpeed;
            }
            else if (speed < 100.f)
            {
                steeringData[i].velocity = (steeringData[i].velocity / speed) * 100.f;
            }

            steeringData[i].position += steeringData[i].velocity * dt.asSeconds();

            // If boid moves outside the window, adjust its position
            // if (steeringData[i].position.x < 0)
            // {
            //     steeringData[i].position.x = window.getSize().x + getRandomOffset(0.f, 100.f); // Adjust the range as needed
            // }
            // else if (steeringData[i].position.x > window.getSize().x)
            // {
            //     steeringData[i].position.x = -getRandomOffset(0.f, 100.f); // Adjust the range as needed
            // }

            // if (steeringData[i].position.y < 0)
            // {
            //     steeringData[i].position.y = window.getSize().y + getRandomOffset(0.f, 100.f); // Adjust the range as needed
            // }
            // else if (steeringData[i].position.y > window.getSize().y)
            // {
            //     steeringData[i].position.y = -getRandomOffset(0.f, 100.f); // Adjust the range as needed
            // }

            boids[i].sprite.setPosition(steeringData[i].position);
            steeringData[i].orientation = getOrientationFromVelocity(steeringData[i].velocity);
            boids[i].sprite.setRotation(steeringData[i].orientation);

            Crumb crumb;
            crumb.setPosition(boids[i].sprite.getPosition());
            breadcrumbs.push_back(crumb);

            // Remove old breadcrumbs if necessary
            if (breadcrumbs.size() > 1000) // Limiting the number of breadcrumbs
            {
                breadcrumbs.erase(breadcrumbs.begin());
            }
        }

        window.clear(sf::Color::White);
        for (int i = 0; i < 30; i++)
            window.draw(boids[i].sprite);

        for (const auto &crumb : breadcrumbs)
        {
            window.draw(crumb);
        }

        // window.draw(sprite.sprite);
        window.display();
    }
}
