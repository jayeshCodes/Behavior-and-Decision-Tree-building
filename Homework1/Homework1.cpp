#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

float xVel = 100.0f, yVel = (480.0f / 640.0f) * 100.0f;
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

void moveX(CustomSprite &sprite)
{
    if (sprite.sprite.getPosition().x < window.getSize().x - 40)
        sprite.sprite.setPosition(sprite.sprite.getPosition().x + xVel, sprite.sprite.getPosition().y);

    if (sprite.sprite.getPosition().x >= window.getSize().x - 40)
    {
        sprite.xcomplete = true;
    }
}

void moveY(CustomSprite &sprite)
{
    sprite.sprite.setRotation(90.f);

    if (sprite.sprite.getPosition().y < window.getSize().y - 75)
        sprite.sprite.setPosition(sprite.sprite.getPosition().x, sprite.sprite.getPosition().y + yVel);

    if (sprite.sprite.getPosition().y >= window.getSize().y - 75)
    {
        sprite.sprite.setRotation(180.f);
        sprite.ycomplete = true;
    }
}

void moveNegX(CustomSprite &sprite)
{
    if (sprite.sprite.getPosition().x > 0)
        sprite.sprite.setPosition(sprite.sprite.getPosition().x - xVel, sprite.sprite.getPosition().y);

    if (sprite.sprite.getPosition().x <= 0)
    {
        sprite.negxcomplete = true;
    }
}

void moveNegY(CustomSprite &sprite)
{
    sprite.sprite.setRotation(270.f);

    if (sprite.sprite.getPosition().y > 0 + 50)
        sprite.sprite.setPosition(sprite.sprite.getPosition().x, sprite.sprite.getPosition().y - yVel);

    if (sprite.sprite.getPosition().y <= 0 + 50)
    {
        sprite.sprite.setRotation(0.f);
        sprite.negycomplete = true;
    }
    if (sprite.xcomplete && sprite.ycomplete && sprite.negxcomplete && sprite.negycomplete)
    {
        sprite.cycleComplete = true;
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
        sprite.sprite.setPosition(0.0f, 0.0f);
        sprite.sprite.setScale(0.1f, 0.1f);
        sprite.cycleComplete = false;
        sprite.spawn = false;
        sprite.xcomplete = false;
        sprite.ycomplete = false;
        sprite.negxcomplete = false;
        sprite.negycomplete = false;
    }

    sprites[0].spawn = true;

    int frame = 0, currentSpriteIndex = 0, rotate = 0;

    bool movingx = true, movingy = false, movingnegx = false, movingnegy = false;

    while (window.isOpen())
    {

        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();
        }

        if (frame % 10 == 0)
        {
            if (currentSpriteIndex % 4 == 0)
            {
                moveX(sprites[0]);
                if (sprites[0].xcomplete)
                {
                    sprites[3].spawn = true;
                    currentSpriteIndex++;
                }
            }
            if (currentSpriteIndex % 4 == 1)
            {

                moveX(sprites[3]);
                moveY(sprites[0]);
                moveNegX(sprites[1]);
                moveNegY(sprites[2]);
                if (sprites[3].xcomplete)
                {
                    currentSpriteIndex++;
                    sprites[2].spawn = true;
                }
            }
            if (currentSpriteIndex % 4 == 2)
            {

                moveX(sprites[2]);
                moveY(sprites[3]);
                moveNegX(sprites[0]);
                moveNegY(sprites[1]);
                if (sprites[2].negxcomplete)
                {
                    currentSpriteIndex++;
                    sprites[1].spawn = true;
                }
            }
            if (currentSpriteIndex % 4 == 3)
            {

                moveX(sprites[1]);
                moveY(sprites[2]);
                moveNegX(sprites[3]);
                moveNegY(sprites[0]);
                if (sprites[0].negycomplete)
                {
                    currentSpriteIndex++;
                }
            }
        }

        window.clear(sf::Color::White);
        {
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
        }
        window.display();

        frame++;
    }

    return 0;
}