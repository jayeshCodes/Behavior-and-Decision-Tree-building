#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

float xVel = 100.0f, yVel = (480.0f / 640.0f) * 100.0f;
int cycleCount = 0;
sf::RenderWindow window(sf::VideoMode(640, 480), "Homework1");

class CustomSprite
{
public:
    sf::Sprite sprite;
    bool cycleComplete;
    bool spawn;
    bool xcomplete;
    bool ycomplete;
    bool negxcomplete;
    bool negycomplete;
};
void moveNegY(CustomSprite &s)
{
    if (s.sprite.getPosition().y > 0 + 50)
        s.sprite.setPosition(s.sprite.getPosition().x, s.sprite.getPosition().y - yVel);

    if (s.sprite.getPosition().y <= 0 + 50)
    {
        s.sprite.setRotation(0.f);
        s.sprite.setRotation(180.f);
        s.xcomplete = false;
        s.ycomplete = false;
        s.negxcomplete = false;
        s.negycomplete = true;
        // moveX(s);
        s.cycleComplete = true;
    }
}

void moveNegX(CustomSprite &s)
{
    if (s.sprite.getPosition().x > 0)
        s.sprite.setPosition(s.sprite.getPosition().x - xVel, s.sprite.getPosition().y);

    if (s.sprite.getPosition().x <= 0)
    {
        s.sprite.setRotation(270.f);
        s.xcomplete = false;
        s.ycomplete = false;
        s.negxcomplete = true;
        s.negycomplete = false;
        // moveNegY(s);
    }
}

void moveY(CustomSprite &s)
{
    if (s.sprite.getPosition().y < window.getSize().y - 75)
        s.sprite.setPosition(s.sprite.getPosition().x, s.sprite.getPosition().y + yVel);

    if (s.sprite.getPosition().y >= window.getSize().y - 75)
    {
        s.sprite.setRotation(180.f);
        s.xcomplete = false;
        s.ycomplete = true;
        s.negxcomplete = false;
        s.negycomplete = false;

        // moveNegX(s);
    }
}

void moveX(CustomSprite &s)
{
    if (s.sprite.getPosition().x < window.getSize().x - 40)
    {
        s.sprite.setPosition(s.sprite.getPosition().x + xVel, s.sprite.getPosition().y);
    }

    if (s.sprite.getPosition().x >= window.getSize().x - 40)
    {
        s.sprite.setRotation(90.f);
        s.xcomplete = true;
        s.ycomplete = false;
        s.negxcomplete = false;
        s.negycomplete = false;

        // moveY(s);
    }
}

int main()
{
    window.setFramerateLimit(60);
    sf::Texture texture;

    if (!texture.loadFromFile("boid.png"))
    {
        // error handling
        std::cout << "texture not found";
        return -1;
    }

    CustomSprite sprites[4];
    for (CustomSprite &sprite : sprites)
    {
        sprite.sprite.setTexture(texture);
        sprite.sprite.setScale(0.1f, 0.1f);
        sprite.sprite.setPosition(sf::Vector2f(0.f, 0.f));
        sprite.spawn = false;
        sprite.cycleComplete = false;
        sprite.xcomplete = false;
        sprite.ycomplete = false;
        sprite.negxcomplete = false;
        sprite.negycomplete = false;
    }

    sprites[0].spawn = true;

    int frame = 0, currentSpriteIndex = 0;
    bool x = false, y = false, negx = false, negy = false;
    bool spawnSprite = false;
    int spriteIndex = 1;

    while (window.isOpen())
    {
        // pollevent
        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();
        }

        if (frame % 10 == 0)
        {
            moveX(sprites[0]);
            if (sprites[0].xcomplete)
            {
                sprites[1].spawn = true;
                moveX(sprites[1]);
                moveY(sprites[0]);
            }
            if (sprites[1].xcomplete)
            {
                sprites[2].spawn = true;
                moveX(sprites[2]);
                moveY(sprites[1]);
                moveNegX(sprites[0]);
            }
            if (sprites[2].xcomplete)
            {
                sprites[3].spawn = true;
                moveX(sprites[3]);
                moveY(sprites[2]);
                moveNegX(sprites[1]);
                moveNegY(sprites[0]);
            }
        }

        // drawing logic
        window.clear(sf::Color::White);
        if (sprites[0].spawn && !sprites[0].cycleComplete)
        {
            window.draw(sprites[0].sprite);
        }
        if (sprites[1].spawn && !sprites[1].cycleComplete)
        {
            window.draw(sprites[1].sprite);
        }
        if (sprites[2].spawn && !sprites[2].cycleComplete)
        {
            window.draw(sprites[2].sprite);
        }
        if (sprites[3].spawn && !sprites[3].cycleComplete)
        {
            window.draw(sprites[3].sprite);
        }

        window.display();
        frame++;
    }
}