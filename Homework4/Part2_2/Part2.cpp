// Part2.cpp
#include "SteeringData.hpp"
#include "SteeringBehavior.hpp"
#include "PathFinder.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <vector>
#include <queue>
#include <cmath>
#include <unordered_set>
#include <memory>
#include <numeric>
#include <queue>
#include <ranges>
#include <set>

// init window
sf::RenderWindow window(sf::VideoMode(GridSizeX *RoomSize, GridSizeY *RoomSize), "PathFinding");
sf::Vector2u windowSize = window.getSize();

SteeringData character, monster, goal, monstergoal;
PositionMatchingBehavior positionMatch;
OrientationMatchingBehavior orientationMatch;
SeekBehavior seek;

sf::Vector2f endpoint;

std::vector<sf::Vector2i> characterPath;
std::vector<sf::Vector2i> monsterPath;

// A* class init
PathFinder::Astar astar;

// customSprite class
class CustomSprite
{
public:
    sf::Sprite sprite;
    sf::Texture texture;

    CustomSprite()
    {
        if (!texture.loadFromFile("boid.png"))
        {
            std::cout << "texture not found";
        }

        sprite.setTexture(texture);
        sprite.setScale(0.02f, 0.02f);
        sprite.setOrigin(sprite.getLocalBounds().width / 2.f, sprite.getLocalBounds().height / 2.f);
        // sprite.setPosition(320.f, 240.f);
        sprite.setRotation(0.f);
    }
};

class CustomSpriteMonster
{
public:
    sf::Sprite sprite;
    sf::Texture texture;

    CustomSpriteMonster()
    {
        if (!texture.loadFromFile("monster.png"))
        {
            std::cout << "texture not found";
        }

        sprite.setTexture(texture);
        sprite.setScale(0.07f, 0.07f);
        sprite.setOrigin(sprite.getLocalBounds().width / 2.f, sprite.getLocalBounds().height / 2.f);
        // sprite.setPosition(320.f, 240.f);
        sprite.setRotation(0.f);
    }
};

void drawObstacles()
{
    for (int i = 0; i < GridSizeX; ++i)
    {
        for (int j = 0; j < GridSizeY; ++j)
        {
            if (grid[i][j] == 1)
            {
                sf::RectangleShape obstacleRect(sf::Vector2f(RoomSize, RoomSize));
                obstacleRect.setPosition(i * RoomSize, j * RoomSize);
                obstacleRect.setFillColor(sf::Color::Black);
                window.draw(obstacleRect);
            }
        }
    }
}

float calculateDistanceFromEdges(const sf::Vector2f &characterPosition, const sf::Vector2u &windowSize)
{
    float distanceLeft = characterPosition.x;
    float distanceRight = windowSize.x - characterPosition.x;
    float distanceTop = characterPosition.y;
    float distanceBottom = windowSize.y - characterPosition.y;

    // Find the minimum distance among the four edges
    float minDistance = std::min({distanceLeft, distanceRight, distanceTop, distanceBottom});

    return minDistance;
}

void drawPath(const std::vector<sf::Vector2i> &path)
{
    for (size_t i = 0; i < path.size() - 1; ++i)
    {
        sf::Vector2i current = path[i];
        sf::Vector2i next = path[i + 1];

        sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(current.x * RoomSize + RoomSize / 2, current.y * RoomSize + RoomSize / 2), sf::Color::Blue),
                sf::Vertex(sf::Vector2f(next.x * RoomSize + RoomSize / 2, next.y * RoomSize + RoomSize / 2), sf::Color::Blue)};

        window.draw(line, 2, sf::Lines);
    }
}

void dance()
{
    character.velocity = sf::Vector2f(100, 100);
    character.rotation = 100.f;
}

sf::Vector2f selectRandom(sf::Vector2f currentPosition)
{
    // Define the maximum offset from the current position
    float maxOffset = 30.f;

    // Generate random offsets within the range [-maxOffset, maxOffset]
    float offsetX = static_cast<float>(std::rand() % static_cast<int>(2 * maxOffset) - maxOffset);
    float offsetY = static_cast<float>(std::rand() % static_cast<int>(2 * maxOffset) - maxOffset);

    // Calculate the new position
    sf::Vector2f newPosition = currentPosition + sf::Vector2f(offsetX, offsetY);

    // Clamp the new position within the game window
    newPosition.x = std::max(RoomSize / 2.f, std::min(windowSize.x - RoomSize / 2.f, newPosition.x));
    newPosition.y = std::max(RoomSize / 2.f, std::min(windowSize.y - RoomSize / 2.f, newPosition.y));

    if (grid[(int)newPosition.x / RoomSize][(int)newPosition.y / RoomSize] == -1)
        return selectRandom(character.position);

    return newPosition;
}

class characterAI
{
private:
    Node *behaviorTreeRoot;

public:
    characterAI()
    {
        behaviorTreeRoot = constructCharacterTree(); // Construct the character behavior tree
    }

    ~characterAI()
    {
        delete behaviorTreeRoot; // Clean up the behavior tree
    }

    ActionType getDecision(const SteeringData &character, const SteeringData &monster)
    {
        return decide(behaviorTreeRoot, character, monster);
    }

    void evaluateDecision(ActionType action)
    {
        // Implement logic to handle the evaluated action
        // This function could involve updating the character's state or behavior
        // based on the action determined by the behavior tree
        // For simplicity, you can implement each action separately here
        switch (action)
        {
        case WANDER:
            // Implement logic for the WANDER action
            std::cout << "character : " << action << std::endl;
            endpoint = selectRandom(character.position);
            characterPath = astar.findPath(character.position, endpoint);
            break;
        case EVADE:
            // Implement logic for the EVADE action
            std::cout << "character : " << action << std::endl;
            break;
        case ZIGZAG:
            // Implement logic for the ZIGZAG action
            std::cout << "character : " << action << std::endl;
            break;
        default:
            // Handle unknown action
            break;
        }
    }
};

class monsterAI
{
private:
    Node *behaviorTreeRoot;

public:
    monsterAI()
    {
        behaviorTreeRoot = constructMonsterTree(); // Construct the monster behavior tree
    }

    ~monsterAI()
    {
        delete behaviorTreeRoot; // Clean up the behavior tree
    }

    ActionType getDecision(monster, character)
    {
        // Implement decision-making logic based on behavior tree evaluation
        // Here you would traverse the behavior tree and determine the action to take
        // For simplicity, you can hardcode the decision for now
        // return a default action (e.g., PURSUE) for now
        return PURSUE;
    }

    void evaluateDecision(ActionType action)
    {
        // Implement logic to handle the evaluated action
        // This function could involve updating the monster's state or behavior
        // based on the action determined by the behavior tree
        // For simplicity, you can implement each action separately here
        switch (action)
        {
        case PATHFIND_TO_CENTER:
            // Implement logic for the PATHFIND_TO_CENTER action
            std::cout<<"monster: "<<action<<std::endl;
            break;
        case PURSUE:
            // Implement logic for the PURSUE action
            std::cout<<"monster: "<<action<<std::endl;
            break;
        case DANCE:
            // Implement logic for the DANCE action
            std::cout<<"monster: "<<action<<std::endl;
            break;
        default:
            // Handle unknown action
            break;
        }
    }
};

int main()
{
    createObstacles();
    window.setFramerateLimit(60);

    CustomSprite c;
    CustomSpriteMonster m;
    c.sprite.setPosition(character.position);

    monster.position.x = 100.f;
    monster.position.y = 90.f;
    m.sprite.setPosition(monster.position);
    // m.sprite.texture.loadFromFile("monster.png");

    // init AIs
    characterAI cai;
    // monsterAI mai;

    // init clocks
    sf::Clock clock;
    sf::Clock wanderTimer;

    while (window.isOpen())
    {
        sf::Event event;
        sf::Time dt = clock.restart();

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        auto characterAction = cai.getDecision(character, monster); // implement this function
        // auto monsterAction = mai.getDecision();   // implement this function

        if (characterAction == WANDER && wanderTimer.getElapsedTime().asSeconds() >= 2.f)
        {
            // Evaluate the decision
            cai.evaluateDecision(characterAction); // implement this function

            // Restart the timer
            wanderTimer.restart();
        }
        else if (characterAction != WANDER)
        {
            // Evaluate other actions immediately
            cai.evaluateDecision(characterAction); // implement this function
        }

        // mai.evaluateDecision(monsterAction); // implement this function

        // the evaluateDecision functions in both cases only calculate the goal positions and therefore, calculate the paths for character and path using the A* algorithm

        // pathfollow for character
        if (characterPath.size() > 0)
        {
            float dx = goal.position.x - character.position.x;
            float dy = goal.position.y - character.position.y;
            // Check if the character has reached the current waypoint
            float distanceToWaypoint = std::sqrt(dx * dx + dy * dy);
            if (distanceToWaypoint < RoomSize / 2)
            {
                // Remove the current waypoint from the path
                characterPath.erase(characterPath.begin());

                // If there are remaining waypoints, set the next waypoint as the goal
                if (!characterPath.empty())
                {
                    goal.position = sf::Vector2f(characterPath[0].x * RoomSize + RoomSize / 2, characterPath[0].y * RoomSize + RoomSize / 2);
                }
            }

            dx = goal.position.x - character.position.x;
            dy = goal.position.y - character.position.y;

            goal.orientation = atan2(dy, dx);

            character.velocity += seek.calculateSteering(character, goal) * dt.asSeconds();

            character.rotation = orientationMatch.calculateSteering(character, goal).y * dt.asSeconds();
            character.orientation += character.rotation;
        }

        character.position += character.velocity * dt.asSeconds();
        character.orientation += character.rotation;

        // pathfollow for monster
        if (monsterPath.size() > 0)
        {
            float dx = monstergoal.position.x - monster.position.x;
            float dy = monstergoal.position.y - monster.position.y;
            // Check if the character has reached the current waypoint
            float distanceToWaypoint = std::sqrt(dx * dx + dy * dy);
            if (distanceToWaypoint < RoomSize / 2)
            {
                // Remove the current waypoint from the path
                monsterPath.erase(monsterPath.begin());

                // If there are remaining waypoints, set the next waypoint as the goal
                if (!monsterPath.empty())
                {
                    monstergoal.position = sf::Vector2f(monsterPath[0].x * RoomSize + RoomSize / 2, monsterPath[0].y * RoomSize + RoomSize / 2);
                }
            }

            dx = monstergoal.position.x - monster.position.x;
            dy = monstergoal.position.y - monster.position.y;

            monstergoal.orientation = atan2(dy, dx);

            monster.velocity += seek.calculateSteering(monster, monstergoal) * dt.asSeconds();

            monster.rotation = orientationMatch.calculateSteering(monster, monstergoal).y * dt.asSeconds();
        }

        monster.position += monster.velocity * dt.asSeconds();
        monster.orientation += monster.rotation;

        // updating sprites
        c.sprite.setPosition(character.position);
        c.sprite.setRotation(character.rotation * (180.f / M_PI));

        m.sprite.setPosition(monster.position);
        m.sprite.setRotation(monster.rotation * (180.f / M_PI));

        // drawing
        window.clear(sf::Color::White);

        if (characterPath.size() > 0)
            drawPath(characterPath);

        if (monsterPath.size() > 0)
            drawPath(monsterPath);

        window.draw(c.sprite);
        window.draw(m.sprite);

        drawObstacles();

        // displaying window
        window.display();
    }
}