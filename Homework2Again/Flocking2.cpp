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
    SteeringData steering;

    CustomSprite()
    {
        if (!texture.loadFromFile("boid-sm.png"))
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

int main()
{
    CustomSprite sprites[20];
    SteeringData character;
    SteeringData goal;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);
        for (CustomSprite &sprite : sprites)
            window.draw(sprite.sprite);
        window.display();
    }
}