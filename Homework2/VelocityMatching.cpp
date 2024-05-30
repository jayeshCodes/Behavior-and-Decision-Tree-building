// VelocityMatching.cpp

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <cmath>
#include <SteeringBehavior.hpp>

sf::RenderWindow window(sf::VideoMode(640, 480), "Homework2 Velocity Matching");

class CustomSprite
{
public:
    sf::Sprite sprite;
    sf::Texture texture;

    // SteeringData steering;

    CustomSprite()
    {
        if (!texture.loadFromFile("boid.png"))
        {
            std::cout << "texture not found";
        }

        sprite.setTexture(texture);
        sprite.setScale(0.05f, 0.05f);
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

    sf::Clock clock;
    sf::Vector2f mousePrevPos;
    sf::Vector2f mouseVelocity;

    CustomSprite sprite;
    sprite.sprite.setScale(0.05f, 0.05f);
    sprite.sprite.setOrigin(sprite.sprite.getLocalBounds().width / 2.f, sprite.sprite.getLocalBounds().height / 2.f);
    sprite.sprite.setPosition(320.f, 240.f);

    SteeringData character, goal;
    character.position = sprite.sprite.getPosition();
    character.velocity = sf::Vector2f(0.f, 0.f);
    // goal.position = sf::Vector2f(0.f, 50.f);
    // goal.velocity = sf::Vector2f(0.01f, 0.f);
    // sf::Vector2i windowCenter(window.getSize().x / 2, window.getSize().y / 2);

    // sf::Mouse::setPosition(windowCenter, window);

    VelocityMatchingBehavior velocityMatch;
    std::vector<Crumb> breadcrumbs;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

        sf::Time elapsed = clock.restart();
        mouseVelocity = (mousePos - mousePrevPos) / elapsed.asSeconds();
        mousePrevPos = mousePos;
        goal.velocity = mouseVelocity;

        character.velocity += velocityMatch.calculateSteering(character, goal) * elapsed.asSeconds();

        character.position += character.velocity;
        sprite.sprite.setPosition(character.position);

        Crumb crumb;
        crumb.setPosition(sprite.sprite.getPosition());
        breadcrumbs.push_back(crumb);

        // Remove old breadcrumbs if necessary
        if (breadcrumbs.size() > 1000) // Limiting the number of breadcrumbs
        {
            breadcrumbs.erase(breadcrumbs.begin());
        }

        window.clear(sf::Color::White);
        window.draw(sprite.sprite);
        for (const auto &crumb : breadcrumbs)
        {
            window.draw(crumb);
        }
        window.display();
    }

    return 0;
}