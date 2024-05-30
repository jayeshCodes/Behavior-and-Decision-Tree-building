#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

sf::RenderWindow window(sf::VideoMode(640, 480), "Homework1");

void moveX(sf::Sprite &s)
{
    if (s.getPosition().x <= window.getSize().x)
    {
        s.move(10, 0);
    }
    if (s.getPosition().x > window.getSize().x)
    {
        s.setPosition(0, 0);
    }
}

int main()
{
    sf::Texture texture;

    if (!texture.loadFromFile("boid.png"))
    {
        // Handle the error, if necessary
    }

    sf::Sprite sprite;
    sprite.setTexture(texture);

    // setscale to change size as sprite is too big
    sprite.setScale(0.1f, 0.1f);

    // set Sprite pos
    sprite.setPosition(0.f, 0.f);
    int frameCount = 0;
    // sprite.rotate(10.f);

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (frameCount % 100 == 0)
        {
            moveX(sprite);
        }

        // draw the sprite outside the event loop
        window.clear(sf::Color::White);
        window.draw(sprite);
        window.display();
        if (frameCount == 1000)
            frameCount = 0;
        frameCount++;
    }

    return 0;
}
