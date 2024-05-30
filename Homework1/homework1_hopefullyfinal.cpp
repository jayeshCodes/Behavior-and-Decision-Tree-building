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
    bool xcomplete;
    bool ycomplete;
    bool negxcomplete;
    bool negycomplete;
    // bool spawn;
};

bool moveX(CustomSprite &s)
{
    if (s.sprite.getPosition().x < window.getSize().x - 40)
    {
        s.sprite.setPosition(s.sprite.getPosition().x + xVel, s.sprite.getPosition().y);
    }

    if (s.sprite.getPosition().x >= window.getSize().x - 40)
    {
        s.sprite.setRotation(90.f);
        s.xcomplete = true;
        return false;
    }
    return true;
}

bool moveY(CustomSprite &s)
{
    if (s.sprite.getPosition().y < window.getSize().y - 75)
        s.sprite.setPosition(s.sprite.getPosition().x, s.sprite.getPosition().y + yVel);

    if (s.sprite.getPosition().y >= window.getSize().y - 75)
    {
        s.sprite.setRotation(180.f);
        s.ycomplete = true;
        return false;
    }
    return true;
}

bool moveNegX(CustomSprite &s)
{
    if (s.sprite.getPosition().x > 0)
        s.sprite.setPosition(s.sprite.getPosition().x - xVel, s.sprite.getPosition().y);

    if (s.sprite.getPosition().x <= 0)
    {
        s.sprite.setRotation(270.f);
        s.negxcomplete = true;
        return false;
    }
    return true;
}

bool moveNegY(CustomSprite &s)
{
    if (s.sprite.getPosition().y > 0 + 50)
        s.sprite.setPosition(s.sprite.getPosition().x, s.sprite.getPosition().y - yVel);

    if (s.sprite.getPosition().y <= 0 + 50)
    {
        s.sprite.setRotation(0.f);
        s.negycomplete=true;
        if(s.negxcomplete && s.negycomplete && s.xcomplete && s.ycomplete){
            s.cycleComplete = true;
        }
        // s.cycleComplete=true;
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

    CustomSprite sprites[4];

    for (CustomSprite &sprite : sprites)
    {
        sprite.sprite.setTexture(texture);
        sprite.sprite.setPosition(0.0f, 0.0f);
        sprite.sprite.setScale(0.1f, 0.1f);
        sprite.cycleComplete = false;
    }

    int frame = 0, currentSpriteIndex = 0, count = -1;
    bool x = true, y = false, negx = false, negy = false;

    while (window.isOpen())
    {

        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();
        }

        if (frame % 30 == 0)
        {
            if (currentSpriteIndex % 4 == 0)
            {
                if (currentSpriteIndex < 4)
                {
                    x = moveX(sprites[currentSpriteIndex % 4]);
                }
                else
                {
                    x = moveX(sprites[0]);
                    y = moveY(sprites[1]);
                    negx = moveNegX(sprites[2]);
                    negy = moveNegY(sprites[3]);
                }
                if (!x)
                {
                    y = true;
                    currentSpriteIndex++;
                }
            }
            if (currentSpriteIndex % 4 == 1)
            {
                if (currentSpriteIndex < 4)
                {
                    x = moveX(sprites[(currentSpriteIndex % 4)]);
                    y = moveY(sprites[(currentSpriteIndex % 4) - 1]);
                }
                else
                {
                    x = moveX(sprites[0]);
                    y = moveY(sprites[1]);
                    negx = moveNegX(sprites[2]);
                    negy = moveNegY(sprites[3]);
                }
                if (!y)
                {
                    negx = true;
                    currentSpriteIndex++;
                }
            }
            if (currentSpriteIndex % 4 == 2)
            {
                if (currentSpriteIndex < 4)
                {
                    x = moveX(sprites[(currentSpriteIndex % 4)]);
                    y = moveY(sprites[(currentSpriteIndex % 4) - 1]);
                    negx = moveNegX(sprites[(currentSpriteIndex % 4) - 2]);
                }
                else
                {
                    x = moveX(sprites[3]);
                    y = moveY(sprites[0]);
                    negx = moveNegX(sprites[1]);
                    negy = moveNegY(sprites[2]);
                }
                if (!negx)
                {
                    negy = true;
                    currentSpriteIndex++;
                }
            }
            if (currentSpriteIndex % 4 == 3)
            {
                if (currentSpriteIndex < 4)
                {
                    x = moveX(sprites[(currentSpriteIndex % 4)]);
                    y = moveY(sprites[(currentSpriteIndex % 4) - 1]);
                    negx = moveNegX(sprites[(currentSpriteIndex % 4) - 2]);
                    negy = moveNegY(sprites[(currentSpriteIndex % 4) - 3]);
                }
                else
                {
                    x = moveX(sprites[2]);
                    y = moveY(sprites[3]);
                    negx = moveNegX(sprites[0]);
                    negy = moveNegY(sprites[1]);
                }
                if (!negy)
                {
                    x = true;
                    currentSpriteIndex++;
                }
            }
        }

        window.clear(sf::Color::White);
        if (currentSpriteIndex < 4)
        {

            for (int i = 0; i <= currentSpriteIndex; i++)
            {
                if (!sprites[i].cycleComplete)
                    window.draw(sprites[i].sprite);
            }
        }
        else
        {
            if (!sprites[0].cycleComplete)
            {
                window.draw(sprites[0].sprite);
            }
            if (!sprites[1].cycleComplete)
            {
                window.draw(sprites[1].sprite);
            }
            if (!sprites[2].cycleComplete)
            {
                window.draw(sprites[2].sprite);
            }
            if (!sprites[3].cycleComplete)
            {
                window.draw(sprites[3].sprite);
            }
        }
        window.display();

        frame++;
    }
}