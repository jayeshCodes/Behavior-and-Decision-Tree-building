// Flocking.cpp

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <cmath>
#include <SteeringBehavior.hpp>
#include <vector>

sf::RenderWindow window(sf::VideoMode(640, 480), "Homework2 Wander");

class CustomSprite
{
public:
    sf::Sprite sprite;
    sf::Texture texture;
    SteeringData steering;

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

sf::Vector2f getRandomPosition(float minX, float maxX, float minY, float maxY)
{
    float randomX = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
    float randomY = minY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxY - minY)));
    return sf::Vector2f(randomX, randomY);
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr))); // Seed the random number generator

    // Create a window and initialize boids
    sf::RenderWindow window(sf::VideoMode(800, 600), "Flocking Simulation");
    std::vector<CustomSprite> boids; // Assuming CustomSprite is your boid class
    for (int i = 0; i < 20; ++i)
    {
        boids.push_back(CustomSprite()); // Initialize boids
    }

    sf::Clock clock;

    // Create flocking behavior instance
    FlockingBehavior flockingBehavior;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Time dt = clock.restart();

        // Iterate through each boid and calculate steering force using flocking behavior
        for (auto &boid : boids)
        {
            // Create SteeringData for the boid (you need to populate this with current boid information)
            SteeringData boidData;
            // Assuming you set position, orientation, velocity, etc. for each boid
            boidData.position = boid.sprite.getPosition();
            boidData.orientation = boid.sprite.getRotation() * (M_PI / 180.f);
            boidData.velocity = boid.steering.velocity; // Assuming velocity is a member of CustomSprite

            // Create a dummy goal (you need to set this based on your simulation)
            SteeringData goal;
            goal.position = getRandomPosition(0.f, window.getSize().x, 0.f, window.getSize().y); // Implement getRandomGoalPosition according to your requirements
            goal.orientation = 0.f;                                  // Set orientation to a suitable value

            // Calculate steering force using flocking behavior
            sf::Vector2f steering = flockingBehavior.calculateSteering(boidData, goal);

            // Update boid's velocity based on steering force (you need to integrate this with your existing code)
            boid.steering.velocity += steering * dt.asSeconds();
            // Limit velocity if needed
            // Update boid's position based on velocity
            boid.sprite.setPosition(boidData.position);
        }

        window.clear(sf::Color::White);
        // Draw boids
        for (const auto &boid : boids)
        {
            window.draw(boid.sprite);
        }
        window.display();
    }

    return 0;
}
