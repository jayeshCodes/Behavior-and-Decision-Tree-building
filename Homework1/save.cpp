#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

float xVel = 100.0f, yVel = (480.0f / 640.0f) * 100.0f;
sf::RenderWindow window(sf::VideoMode(640, 480), "Homework1");

bool moveX(sf::Sprite &sprite)
{
    if (sprite.getPosition().x < window.getSize().x - 40)
        sprite.setPosition(sprite.getPosition().x + xVel, sprite.getPosition().y);

    if (sprite.getPosition().x >= window.getSize().x - 40)
    {
        sprite.setRotation(90.f);
        return false;
    }
    return true;
}

bool moveY(sf::Sprite &sprite)
{
    if (sprite.getPosition().y < window.getSize().y - 75)
        sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y + yVel);

    if (sprite.getPosition().y >= window.getSize().y - 75)
    {
        sprite.setRotation(180.f);
        return false;
    }
    return true;
}

bool moveNegX(sf::Sprite &sprite)
{
    if (sprite.getPosition().x > 0)
        sprite.setPosition(sprite.getPosition().x - xVel, sprite.getPosition().y);

    if (sprite.getPosition().x <= 0)
    {
        sprite.setRotation(270.f);
        return false;
    }
    return true;
}

bool moveNegY(sf::Sprite &sprite)
{
    if (sprite.getPosition().y > 0 + 50)
        sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y - yVel);

    if (sprite.getPosition().y <= 0 + 50)
    {
        sprite.setRotation(0.f);
        return false;
    }
    return true;
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
    sf::Sprite sprites[4];

    for (sf::Sprite &sprite : sprites)
    {
        sprite.setTexture(texture);
        sprite.setPosition(0.0f, 0.0f);
        sprite.setScale(0.1f, 0.1f);
    }

    int moveIndices[4];

    for (int i = 0; i < 4; i++)
    {
        moveIndices[i] = i;
    }

    int frame = 0, currentSpriteIndex = 0, rotate = 0;

    float xVel = 100.0f, yVel = (480.0f / 640.0f) * 100.0f;

    bool movingx = true, movingy = false, movingnegx = false, movingnegy = false;

    while (window.isOpen())
    {

        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();
        }

        if (frame % 10 == 0 && currentSpriteIndex < 4)
        {
            if (currentSpriteIndex % 4 == 0)
            {
                // if (currentSpriteIndex < 4)
                {
                    movingx = moveX(sprites[currentSpriteIndex % 4]);
                }
                if (!movingx)
                {
                    // movingy = true;
                    currentSpriteIndex++;
                }
            }

            if (currentSpriteIndex % 4 == 1)
            {
                // if (currentSpriteIndex < 4)
                {
                    movingx = moveX(sprites[(currentSpriteIndex % 4)]);
                    movingy = moveY(sprites[(currentSpriteIndex % 4) - 1]);
                }
                if (!movingy)
                {
                    // movingnegx = true;
                    currentSpriteIndex++;
                }
            }

            if (currentSpriteIndex % 4 == 2)
            {
                // if (currentSpriteIndex < 4)
                {
                    movingx = moveX(sprites[(currentSpriteIndex % 4)]);
                    movingy = moveY(sprites[(currentSpriteIndex % 4) - 1]);
                    movingnegx = moveNegX(sprites[(currentSpriteIndex % 4) - 2]);
                }

                if (!movingnegx)
                {
                    // movingnegy = true;
                    currentSpriteIndex++;
                }
            }

            if (currentSpriteIndex % 4 == 3)
            {
                movingx = moveX(sprites[(currentSpriteIndex % 4)]);
                movingy = moveY(sprites[(currentSpriteIndex % 4) - 1]);
                movingnegx = moveNegX(sprites[(currentSpriteIndex % 4) - 2]);
                movingnegy = moveNegY(sprites[(currentSpriteIndex % 4) - 3]);
                if (!movingnegy)
                {
                    currentSpriteIndex++;
                    movingx = true;
                }
            }
        }

        // if (frame % 10 == 0 && currentSpriteIndex > 3)
        // {

        //     std::cout << "second if";

        //     switch (rotate)
        //     {
        //     case 0:
        //         movingx = moveX(sprites[0]);
        //         movingy = moveY(sprites[1]);
        //         movingnegx = moveNegX(sprites[2]);
        //         movingnegy = moveNegY(sprites[3]);
        //         rotate = 1;
        //         break;

        //     case 1:
        //         movingx = moveX(sprites[3]);
        //         movingy = moveY(sprites[0]);
        //         movingnegx = moveNegX(sprites[1]);
        //         movingnegy = moveNegY(sprites[2]);
        //         rotate = 2;
        //         break;

        //     case 2:
        //         movingx = moveX(sprites[2]);
        //         movingy = moveY(sprites[3]);
        //         movingnegx = moveNegX(sprites[0]);
        //         movingnegy = moveNegY(sprites[1]);
        //         rotate = 3;
        //         break;

        //     case 3:
        //         movingx = moveX(sprites[1]);
        //         movingy = moveY(sprites[2]);
        //         movingnegx = moveNegX(sprites[3]);
        //         movingnegy = moveNegY(sprites[0]);
        //         rotate = 0;
        //         break;
        //     }
        //     // if (movingx)
        //     // {
        //     //     movingx = moveX(sprites[0]);
        //     //     movingy = moveY(sprites[1]);
        //     //     movingnegx = moveNegX(sprites[2]);
        //     //     movingnegy = moveNegY(sprites[3]);
        //     //     if (!movingx)
        //     //         movingy = true;
        //     // }
        //     // if (movingy)
        //     // {
        //     //     movingx = moveX(sprites[3]);
        //     //     movingy = moveY(sprites[0]);
        //     //     movingnegx = moveNegX(sprites[1]);
        //     //     movingnegy = moveNegY(sprites[2]);
        //     //     if (!movingy)
        //     //         movingnegx = true;
        //     // }
        //     // if (movingnegx)
        //     // {
        //     //     movingx = moveX(sprites[2]);
        //     //     movingy = moveY(sprites[3]);
        //     //     movingnegx = moveNegX(sprites[0]);
        //     //     movingnegy = moveNegY(sprites[1]);
        //     //     if (!movingnegx)
        //     //         movingnegy = true;
        //     // }
        //     // if (movingnegy)
        //     // {
        //     //     movingx = moveX(sprites[1]);
        //     //     movingy = moveY(sprites[2]);
        //     //     movingnegx = moveNegX(sprites[3]);
        //     //     movingnegy = moveNegY(sprites[0]);
        //     //     if (!movingnegy)
        //     //         movingx = true;
        //     // }
        // }

        window.clear(sf::Color::White);
        if (currentSpriteIndex < 4)
        {

            for (int i = 0; i <= currentSpriteIndex; i++)
            {
                window.draw(sprites[i]);
            }
        }
        else
        {
            for (int i = 0; i <= 3; i++)
            {
                window.draw(sprites[i]);
            }
        }

        // else
        // {
        //     window.draw(sprites[0]);
        //     window.draw(sprites[1]);
        //     window.draw(sprites[2]);
        //     window.draw(sprites[3]);
        // }

        window.display();

        frame++;
    }

    return 0;
}
