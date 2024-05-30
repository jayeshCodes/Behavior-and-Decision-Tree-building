#include "SteeringData.hpp"
#include "SteeringBehavior.hpp"
#include "PathFinder.hpp"
#include "Node.cpp"

#include <iostream>

// init window
sf::RenderWindow window(sf::VideoMode(GridSizeX *RoomSize, GridSizeY *RoomSize), "PathFinding");
sf::Vector2u windowSize = window.getSize();

SteeringData character, goal;
PositionMatchingBehavior positionMatch;
OrientationMatchingBehavior orientationMatch;
SeekBehavior seek;

// init start and end points for path finding
sf::Vector2i start, end;

std::vector<sf::Vector2i> path;

// A* class init
Astar astar;


class CustomSprite
{
public:
    sf::Sprite sprite;
    sf::Texture texture;

    CustomSprite()
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

void pathFollow(const SteeringData &character)
{
    // logic for path following path provided by decisiontree
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


// class AI
// {
// public:
//     int getDecision(SteeringData &character, SteeringData &goal)
//     {
//         std::vector<int> features = extractFeatures(character);
//         // return dtree.takeAction(features, character, goal);
//         int action = dtree.takeAction(character);
//         std::cout << "Action: " << ActionType(action) << std::endl;
//         return action;
//     }

//     void evaluateDecision(int &action)
//     {
//         int randomX = std::rand() % GridSizeX;
//         int randomY = std::rand() % GridSizeY;
//         sf::Vector2f endpoint;
//         float distance;
//         float distanceToEndpoint;

//         switch (action)
//         {
//         case WANDER:
//             // wander logic
//             // std::cout << "wander" << std::endl;
//             // Generate random coordinates within the grid bounds
//             endpoint = selectRandom(character.position);
//             path = astar.findPath(character.position, endpoint);
//             break;

//         case PATHFIND: // to center
//             // std::cout << "pathfind" << std::endl;
//             endpoint = sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f);
//             path = astar.findPath(character.position, endpoint);
//             break;

//         case DANCE:
//             path.clear();
//             dance();
//             break;

//             // default:
//             //     break;
//         }
//         if (!path.empty())
//             goal.position = sf::Vector2f(path[0].x * RoomSize + RoomSize / 2, path[0].y * RoomSize + RoomSize / 2);
//     }

// private:
//     std::vector<int> extractFeatures(SteeringData &character)
//     {

//         std::vector<int> features;

//         // Extract features based on character's position
//         if (character.position.x < 25.f || character.position.x > 675.f)
//             features.push_back(1);
//         else
//             features.push_back(0);

//         if (character.position.y < 25.f || character.position.y > 675.f)
//             features.push_back(1);
//         else
//             features.push_back(0);

//         // Extract features based on character's velocity
//         float absoluteVelocity = std::sqrt(character.velocity.x * character.velocity.x + character.velocity.y * character.velocity.y);
//         if (absoluteVelocity > 50.f)
//             features.push_back(1);
//         else
//             features.push_back(0);

//         return features;
//     }
// };

int main()
{
    window.setFramerateLimit(60);
    sf::Clock clock;

    // sprite init
    CustomSprite sprite;
    character.position = sf::Vector2f(10.f,10.f);
    sprite.sprite.setPosition(character.position);

    // AI init
    // AI ai;

    int action;

    // Construct the decision tree
    // auto root = dtree.constructDecisionTree();

    // Visualize the decision tree
    // dtree.visualizeTree(root);

    createObstacles();
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

        // getDecision
        // action = ai.getDecision(character, goal);
        // if (action == WANDER && wanderTimer.getElapsedTime().asSeconds() >= 2.f)
        // {
        //     // Evaluate the decision
        //     ai.evaluateDecision(action);

        //     // Restart the timer
        //     wanderTimer.restart();
        // }
        // else if (action != WANDER)
        // {
        //     // Evaluate other actions immediately
        //     ai.evaluateDecision(action);
        // }

        if (path.size() > 0)
        {
            float dx = goal.position.x - character.position.x;
            float dy = goal.position.y - character.position.y;
            // Check if the character has reached the current waypoint
            float distanceToWaypoint = std::sqrt(dx * dx + dy * dy);
            if (distanceToWaypoint < RoomSize / 2)
            {
                // Remove the current waypoint from the path
                path.erase(path.begin());

                // If there are remaining waypoints, set the next waypoint as the goal
                if (!path.empty())
                {
                    goal.position = sf::Vector2f(path[0].x * RoomSize + RoomSize / 2, path[0].y * RoomSize + RoomSize / 2);
                }
            }

            dx = goal.position.x - character.position.x;
            dy = goal.position.y - character.position.y;

            goal.orientation = atan2(dy, dx);

            // updating character's position and orientation using ArriveAndAlign behavior
            // if (path.size() > 1)
            // {
            character.velocity += seek.calculateSteering(character, goal) * dt.asSeconds();
            // }
            // else
            // {
            //     character.velocity += positionMatch.calculateSteering(character, goal) * dt.asSeconds();
            // }

            character.rotation = orientationMatch.calculateSteering(character, goal).y * dt.asSeconds();
            character.orientation += character.rotation;
        }

        character.position += character.velocity * dt.asSeconds();
        character.orientation += character.rotation;

        // updating sprite
        sprite.sprite.setPosition(character.position);
        sprite.sprite.setRotation(character.orientation * (180.f / M_PI));

        // draw logic
        window.clear(sf::Color::White);

        if (path.size() > 0)
            drawPath(path);

        window.draw(sprite.sprite);
        drawObstacles();

        // displaying window
        window.display();
    }
}