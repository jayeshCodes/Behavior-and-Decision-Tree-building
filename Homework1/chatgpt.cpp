#include <SFML/Graphics.hpp>

const int windowWidth = 640;
const int windowHeight = 480;
const int spriteSize = 50;
const int movementSpeed = 5;

class AnimatedSprite {
public:
    sf::Sprite sprite;
    sf::Vector2f position;
    int direction; // 0: right, 1: down, 2: left, 3: up

    AnimatedSprite(sf::Texture& texture, const sf::Vector2f& initialPosition)
        : position(initialPosition), direction(0) {
        sprite.setTexture(texture);
        sprite.setPosition(initialPosition);
    }

    void move() {
        switch (direction) {
        case 0: // right
            position.x += movementSpeed;
            break;
        case 1: // down
            position.y += movementSpeed;
            break;
        case 2: // left
            position.x -= movementSpeed;
            break;
        case 3: // up
            position.y -= movementSpeed;
            break;
        }
        sprite.setPosition(position);
    }

    void rotateAndMove() {
        sprite.setRotation(direction * 90); // Rotate sprite based on direction
        move();
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML Animation");
    
    sf::Texture texture;
    if (!texture.loadFromFile("boid-sm.png")) {
        // Handle loading error
        return -1;
    }

    AnimatedSprite sprite1(texture, sf::Vector2f(50, 50));
    AnimatedSprite sprite2(texture, sf::Vector2f(windowWidth - spriteSize - 50, 50));
    AnimatedSprite sprite3(texture, sf::Vector2f(windowWidth - spriteSize - 50, windowHeight - spriteSize - 50));
    AnimatedSprite sprite4(texture, sf::Vector2f(50, windowHeight - spriteSize - 50));

    sf::Clock clock;
    int frameCounter = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (clock.getElapsedTime().asMilliseconds() >= 100) {
            // Update sprite positions and rotations every 100 milliseconds
            sprite1.move();
            sprite2.rotateAndMove();
            sprite3.rotateAndMove();
            sprite4.rotateAndMove();

            frameCounter++;

            if (frameCounter == 25) {
                // Change direction and reset frame counter when a quarter of the loop is completed
                sprite1.direction = 1; // move down
                sprite2.direction = 2; // move left
                sprite3.direction = 3; // move up
                sprite4.direction = 0; // move right

                frameCounter = 0;
            }

            clock.restart();
        }

        window.clear(sf::Color::White);
        window.draw(sprite1.sprite);
        window.draw(sprite2.sprite);
        window.draw(sprite3.sprite);
        window.draw(sprite4.sprite);
        window.display();
    }

    return 0;
}
