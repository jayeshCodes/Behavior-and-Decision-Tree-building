// // Part2.cpp
#include "SteeringBehavior.hpp"
#include "SteeringData.hpp"
#include "PathFinder.hpp"
#include "DecisionTree.hpp"

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
#include <fstream>

// init window
sf::RenderWindow window(sf::VideoMode(GridSizeX *RoomSize, GridSizeY *RoomSize), "Decision Trees");
sf::Vector2u windowSize = window.getSize();

SteeringData character, goal;
PositionMatchingBehavior positionMatch;
OrientationMatchingBehavior orientationMatch;
SeekBehavior seek;

sf::Vector2f endpoint;

std::vector<sf::Vector2i> characterPath;
std::vector<sf::RectangleShape> obstacles;

// // A* class init
Astar astar;

std::vector<sf::Vector2i> obstaclePositions;

// get data

std::vector<sf::Vector2i> extractObstacles(const std::vector<std::vector<int>> &grid)
{
    std::vector<sf::Vector2i> obstaclePositions;

    // Iterate over the grid
    for (int x = 0; x < GridSizeX; ++x)
    {
        for (int y = 0; y < GridSizeY; ++y)
        {
            // Check if the current position is an obstacle
            if (grid[x][y] == 1)
            {
                // Add the obstacle position to the vector
                obstaclePositions.push_back(sf::Vector2i(x, y));
            }
        }
    }

    return obstaclePositions;
}

bool checkObstacleProximity(const sf::Vector2f &monsterPosition, const std::vector<sf::Vector2i> &obstaclePositions, float proximityThreshold)
{
    for (const auto &obstaclePos : obstaclePositions)
    {
        // Calculate the distance between the monster and the center of each obstacle
        sf::Vector2f obstacleCenter = sf::Vector2f((obstaclePos.x + 0.5f) * RoomSize, (obstaclePos.y + 0.5f) * RoomSize);
        float distance = std::sqrt(std::pow(monsterPosition.x - obstacleCenter.x, 2) +
                                   std::pow(monsterPosition.y - obstacleCenter.y, 2));
        // Check if the distance is within the proximity threshold
        if (distance <= proximityThreshold)
        {
            return true; // Obstacle is too close
        }
    }
    return false; // No obstacle is too close
}

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
float calculateDistance(const sf::Vector2f &p1, const sf::Vector2f &p2)
{
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

void resetCharacters()
{
    // do
    // {
    character.position.x = static_cast<float>(std::rand() % windowSize.x);
    character.position.y = static_cast<float>(std::rand() % windowSize.y);
    // } while (grid[(int)(character.position.x / RoomSize)][(int)(character.position.y / RoomSize)] == 1);

    character.orientation = 0;

    // Clear character and monster paths
    characterPath.clear();
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

    if (grid[(int)newPosition.x / RoomSize][(int)newPosition.y / RoomSize] == 1)
        return selectRandom(character.position);

    return newPosition;
}

sf::Vector2f selectRandom2(sf::Vector2f currentPosition)
{
    // Define the desired distance
    float distance = 60.f;

    // Generate a random angle in radians
    float randomAngle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2 * M_PI;

    // Calculate the new position based on the random angle and desired distance
    float offsetX = std::cos(randomAngle) * distance;
    float offsetY = std::sin(randomAngle) * distance;

    // Calculate the new position
    sf::Vector2f newPosition = currentPosition + sf::Vector2f(offsetX, offsetY);

    // Clamp the new position within the game window
    newPosition.x = std::max(RoomSize / 2.f, std::min(windowSize.x - RoomSize / 2.f, newPosition.x));
    newPosition.y = std::max(RoomSize / 2.f, std::min(windowSize.y - RoomSize / 2.f, newPosition.y));

    if (grid[(int)newPosition.x / RoomSize][(int)newPosition.y / RoomSize] == 1)
        return selectRandom2(currentPosition); // Recursively try again if the new position is blocked

    return newPosition;
}

bool isMonsterCaught(const sf::Vector2f &characterPos, const sf::Vector2f &monsterPos)
{
    float distance = calculateDistance(characterPos, monsterPos);
    return distance < 2;
}

// Function to reset all characters
std::vector<int> extractFeatures()
{
    std::vector<int> features;
    float dw = calculateDistanceFromEdges(character.position, windowSize);
    if (dw < 20.f)
        features.push_back(1);
    else
        features.push_back(0);

    bool po = checkObstacleProximity(character.position, obstaclePositions, 20.f);
    if (po)
        features.push_back(1);
    else
        features.push_back(0);

    float dc = calculateDistance(character.position, sf::Vector2f(250.f, 250.f));
    if (dc <= 0.f)
        features.push_back(1);
    else
        features.push_back(0);

    return features;
}

TreeNode *characterRoot;

class characterAI
{
public:
    int characteraction;
    characterAI()
    {
        characterRoot = buildDecisionTree();
        obstaclePositions = extractObstacles(grid);
    }
    void getAction()
    {
        std::vector<int> features;
        features = extractFeatures();
        characteraction = classifyItem(characterRoot, features);
    }

    void performAction()
    {
        int randomX = std::rand() % GridSizeX;
        int randomY = std::rand() % GridSizeY;
        sf::Vector2f endpoint;
        float distance;
        float distanceToEndpoint;
        sf::Vector2f estimatedFutureMonsterPosition;
        sf::Vector2f evasionEndpoint;
        float tolerance = 5.0f; // Adjust as needed

        switch (characteraction)
        {
        case WANDER:
            // Check if the character has reached its goal within the tolerance distance
            // if (std::abs(character.position.x - goal.position.x) < tolerance && std::abs(character.position.y - goal.position.y) < tolerance)
            {
                // Generate random coordinates within the grid bounds
                std::cout << "character wander";
                endpoint = selectRandom(character.position);
                characterPath = astar.findPath(character.position, endpoint);
                // Set the new goal position
            }
            break;
        case PATHFIND_TO_CENTER: // to center
                                 // Move towards the center of the grid
            std::cout << "character pathfind to center" << std::endl;

            endpoint = sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f);
            characterPath = astar.findPath(character.position, endpoint);
            break;

        case PATHFIND_AWAY_FROM_OBSTACLE:
        {
            // Find the nearest obstacle to the character
            std::cout << "character pathfind away from obstacle" << std::endl;
            sf::Vector2f nearestObstacle;
            float minDistanceToObstacle = std::numeric_limits<float>::max();
            for (const auto &obstaclePos : obstaclePositions)
            {
                sf::Vector2f obstacleCenter = sf::Vector2f((obstaclePos.x + 0.5f) * RoomSize, (obstaclePos.y + 0.5f) * RoomSize);
                float distanceToObstacle = calculateDistance(character.position, obstacleCenter);
                if (distanceToObstacle < minDistanceToObstacle)
                {
                    minDistanceToObstacle = distanceToObstacle;
                    nearestObstacle = obstacleCenter;
                }
            }

            // Calculate a new endpoint away from the nearest obstacle
            sf::Vector2f directionFromObstacle = character.position - nearestObstacle;
            directionFromObstacle /= minDistanceToObstacle; // Normalize the direction vector
            endpoint = character.position + directionFromObstacle * 20.f;

            // Ensure the new endpoint is within the window bounds
            endpoint.x = std::max(RoomSize / 2.f, std::min(windowSize.x - RoomSize / 2.f, endpoint.x));
            endpoint.y = std::max(RoomSize / 2.f, std::min(windowSize.y - RoomSize / 2.f, endpoint.y));

            characterPath = astar.findPath(character.position, endpoint);
        }
        break;
        }
        if (!characterPath.empty())
            goal.position = sf::Vector2f(characterPath[0].x * RoomSize + RoomSize / 2, characterPath[0].y * RoomSize + RoomSize / 2);
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

    createObstacles();
    window.setFramerateLimit(60);

    CustomSprite c;

    resetCharacters();
    character.position = sf::Vector2f(0.f, 0.f);
    // Initialize character position randomly within the window and ensure it's a valid grid position
    c.sprite.setPosition(character.position);

    // init AIs
    characterAI cai;
    // init clocks
    sf::Clock clock;
    sf::Clock cwanderTimer;

    int frameCount = 0;

    std::vector<Crumb> breadcrumbs;

    while (window.isOpen())
    {
        sf::Event event;
        sf::Time dt = clock.restart();
        frameCount++;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        cai.getAction(); // implement this function

        if (cwanderTimer.getElapsedTime().asSeconds() >= 1.f)
        {
            // Evaluate the decision
            cai.performAction(); // implement this function

            // Restart the timer
            cwanderTimer.restart();
        }

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

            character.velocity += seek.calculateSteering(character, goal) * dt.asSeconds() * 1.5f;

            character.rotation = orientationMatch.calculateSteering(character, goal).y * dt.asSeconds();
            character.orientation += character.rotation;
        }

        // Update character position and ensure it stays within the grid
        character.position += character.velocity * dt.asSeconds();
        character.position.x = std::max(RoomSize / 2.f, std::min(windowSize.x - RoomSize / 2.f, character.position.x));
        character.position.y = std::max(RoomSize / 2.f, std::min(windowSize.y - RoomSize / 2.f, character.position.y));
        character.orientation += character.rotation;

        // updating sprites
        c.sprite.setPosition(character.position);
        c.sprite.setRotation(character.orientation * (180.f / M_PI));

        // drawing
        window.clear(sf::Color::White);

        Crumb crumb;
        crumb.setPosition(c.sprite.getPosition());
        breadcrumbs.push_back(crumb);

        if (breadcrumbs.size() > 2000) // Limiting the number of breadcrumbs
        {
            breadcrumbs.erase(breadcrumbs.begin());
        }

        for (const auto &crumb : breadcrumbs)
        {
            window.draw(crumb);
        }

        window.draw(c.sprite);

        if (characterPath.size() > 0)
            drawPath(characterPath);

        window.draw(c.sprite);
        drawObstacles();

        // displaying window
        window.display();
    }
}