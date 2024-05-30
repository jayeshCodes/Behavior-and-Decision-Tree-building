// // Part2.cpp
#include "SteeringBehavior.hpp"
#include "SteeringData.hpp"
#include "PathFinder.hpp"
#include "BehaviorTreeNode.hpp"
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
#include <set>
#include <fstream>

// init window
sf::RenderWindow window(sf::VideoMode(GridSizeX *RoomSize, GridSizeY *RoomSize), "Decision Tree Learning");
sf::Vector2u windowSize = window.getSize();

SteeringData goal, monstergoal;
PositionMatchingBehavior positionMatch;
OrientationMatchingBehavior orientationMatch;
SeekBehavior seek;

sf::Vector2f endpoint;
std::vector<sf::Vector2i> obstaclePositions;

std::vector<sf::Vector2i>
    characterPath;
std::vector<sf::Vector2i> monsterPath;
std::vector<sf::RectangleShape> obstacles;

// init behavior trees
CharacterBehaviorTree cbt;

// // A* class init
Astar astar;

bool isDanceSequenceOn = false;

// get data
struct State
{
    float distanceToCharacter;
    int characterAction;
    int monsterAction;
    bool isDanceComplete;
    float distanceToCenter;
    bool obstacleProximity;
    // float monsterVelocity;
};

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

// Function to record state information to a file
void recordDataToFile(const State &state)
{
    std::ofstream file("data.csv", std::ios::app); // Open the file in append mode
    if (file.is_open())
    {
        // Write attribute values to the file
        file << state.distanceToCharacter << ","
             << state.characterAction << ","
             << state.monsterAction << ","
             << state.isDanceComplete << ","
             << state.distanceToCenter << ","
             << state.obstacleProximity << "\n";
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file!" << std::endl;
    }
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
float calculateDistance(const sf::Vector2f &p1, const sf::Vector2f &p2)
{
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

void resetCharacters()
{
    do
    {
        character.position.x = static_cast<float>(std::rand() % windowSize.x);
        character.position.y = static_cast<float>(std::rand() % windowSize.y);
    } while (grid[(int)(character.position.x / RoomSize)][(int)(character.position.y / RoomSize)] == 1);

    // Initialize monster position randomly within the window and ensure it's a valid grid position
    do
    {
        monster.position.x = static_cast<float>(std::rand() % windowSize.x);
        monster.position.y = static_cast<float>(std::rand() % windowSize.y);
    } while (grid[(int)(monster.position.x / RoomSize)][(int)(monster.position.y / RoomSize)] == 1);

    // Ensure that the character and monster are at least 100 units away from each other
    float distance = calculateDistance(character.position, monster.position);
    while (distance > 100.0f)
    {
        // Move the monster position to a new random location
        monster.position.x = static_cast<float>(std::rand() % windowSize.x);
        monster.position.y = static_cast<float>(std::rand() % windowSize.y);

        // Recalculate the distance between character and monster
        distance = calculateDistance(character.position, monster.position);
    }
    character.orientation = 0;
    monster.orientation = 0;

    // Clear character and monster paths
    characterPath.clear();
    monsterPath.clear();
    isDanceComplete = false;
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

std::vector<int> extractFeatures()
{
    std::vector<int> features;
    float dc = calculateDistance(character.position, monster.position);
    if (dc > 50)
        features.push_back(0);
    else if (dc <= 50 && dc > 10)
        features.push_back(1);
    else if (dc <= 10 && dc > 5)
        features.push_back(2);

    std::cout << "Character action: " << characteraction << std::endl;
    int ca = characteraction;
    std::cout << "Value of ca: " << ca << std::endl;
    features.push_back(ca);

    bool value = isDanceComplete;

    // start commenting below
    // if (isDanceComplete)
    //     features.push_back(1);
    // else
    //     features.push_back(0);
    // end commenting here

    // comment the above lines and uncomment the below lines for second implementation and vice versa for first implementation
    // start commenting below
    if (isDanceComplete)
        features.push_back(1);
    else
        features.push_back(0);
    // end commenting here

    value = checkObstacleProximity(monster.position, obstaclePositions, 10.f);
    if (value)
        features.push_back(1);
    else
        features.push_back(0);

    return features;
}

Node *characterRoot;
TreeNode *monsterRoot;

class characterAI
{
public:
    characterAI()
    {
        characterRoot = cbt.constructBehaviorTree();
    }
    void getAction()
    {
        characterRoot->Evaluate();
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
                endpoint = selectRandom(character.position);
                characterPath = astar.findPath(character.position, endpoint);
                // Set the new goal position
            }
            break;
        case PATHFIND_TO_CENTER: // to center
                                 // Move towards the center of the grid
            std::cout << "character pathfind" << std::endl;

            endpoint = sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f);
            characterPath = astar.findPath(character.position, endpoint);
            break;

        case DANCE:
            // characterPath.clear();
            // dance();
            break;

        case EVADE:
            std::cout << "character Evading" << std::endl;
            estimatedFutureMonsterPosition = monster.position + monster.velocity * 5.0f; // Predict 2 seconds into the future

            // Calculate a point along the path from character's current position to a position opposite to the estimated future position of the monster
            evasionEndpoint = character.position + (character.position - estimatedFutureMonsterPosition) * 0.5f;

            // Find path to the evasion endpoint
            characterPath = astar.findPath(character.position, evasionEndpoint);

            break;

        case ZIGZAG:
            std::cout << "character Zigzagging" << std::endl;
            // Generate zigzagging path
            characterPath.clear();
            // Move diagonally, alternating between left-up and right-down directions
            for (int i = 0; i < 10; ++i)
            {
                // Alternate between left-up and right-down directions
                if (i % 2 == 0)
                    characterPath.push_back(sf::Vector2i(character.position.x - i, character.position.y - i));
                else
                    characterPath.push_back(sf::Vector2i(character.position.x + i, character.position.y + i));
            }
            break;

            // default:
            //     break;
        }
        if (!characterPath.empty())
            goal.position = sf::Vector2f(characterPath[0].x * RoomSize + RoomSize / 2, characterPath[0].y * RoomSize + RoomSize / 2);
    }
};
class monsterAI
{
public:
    monsterAI()
    {
        monsterRoot = buildDecisionTree();
    }
    void getAction()
    {
        std::vector<int> features = extractFeatures();
        std::cout << "Distance feature: " << features[0] << std::endl;
        std::cout << "Character Action: " << features[1] << std::endl;
        std::cout << "IsDanceComplete feature: " << features[2] << std::endl;
        std::cout << "obstacleProximity feature: " << features[3] << std::endl;
        monsteraction = classifyItem(monsterRoot, features);
        // comment the line below for first implementation
        if (calculateDistance(monster.position, character.position) <= 2.f)
            monsteraction = DANCE;
        std::cout << "Monster Action from tree: " << monsteraction << std::endl;
    }

    void performAction()
    {
        int randomX = std::rand() % GridSizeX;
        int randomY = std::rand() % GridSizeY;
        sf::Vector2f endpoint;
        float distance;
        float distanceToEndpoint;
        sf::Vector2f estimatedFuturePosition;
        sf::Vector2f pursuitEndpoint;
        float tolerance = 5.0f; // Adjust as needed

        switch (monsteraction)
        {
        case WANDER:
            // if (std::abs(monster.position.x - monstergoal.position.x) < tolerance && std::abs(monster.position.y - monstergoal.position.y) < tolerance)
            {
                // Generate random coordinates within the grid bounds

                endpoint = selectRandom(monster.position);
                monsterPath = astar.findPath(monster.position, endpoint);
                // Set the new goal position
            }
            break;
        case PATHFIND_TO_CENTER: // to center
            std::cout << "monster pathfind" << std::endl;
            endpoint = sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f);
            monsterPath = astar.findPath(monster.position, selectRandom(character.position));
            break;

        case DANCE:
            std::cout << "monster dance" << std::endl;

            monsterPath.clear();

            // dance();
            // if (isDanceComplete)
            //     return;
            break;

        case PURSUE:
            std::cout << "monster pursue" << std::endl;

            estimatedFuturePosition = character.position + character.velocity * 5.0f; // Predict 2 seconds into the future

            // Calculate a point along the path from monster's current position to the estimated future position of the character
            pursuitEndpoint = monster.position + (estimatedFuturePosition - monster.position) * 0.5f;

            // Find path to the pursuit endpoint
            monsterPath = astar.findPath(monster.position, pursuitEndpoint);
            // monsterPath.clear();
            monstergoal.position = pursuitEndpoint;
            break;

            // default:
            //     break;
        }
        if (!monsterPath.empty())
            monstergoal.position = sf::Vector2f(monsterPath[0].x * RoomSize + RoomSize / 2, monsterPath[0].y * RoomSize + RoomSize / 2);
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

class MonsterCrumb : public sf::CircleShape
{
public:
    MonsterCrumb()
    {
        this->setRadius(2.f);
        this->setFillColor(sf::Color::Red);
    }
};

int main()
{
    std::cout << "window size:" << (int)windowSize.x << "," << (int)windowSize.y;
    createObstacles();
    window.setFramerateLimit(60);

    CustomSprite c;
    CustomSpriteMonster m;

    resetCharacters();
    // Initialize character position randomly within the window and ensure it's a valid grid position
    c.sprite.setPosition(character.position);
    m.sprite.setPosition(monster.position);

    // init AIs
    characterAI cai;
    monsterAI mai;
    // init clocks
    sf::Clock clock;
    sf::Clock cwanderTimer;
    sf::Clock mwanderTimer;

    int frameCount = 0;
    std::vector<sf::Vector2i> obstaclePositions = extractObstacles(grid);

    std::vector<Crumb> breadcrumbs;
    std::vector<MonsterCrumb> monsterbreadcrumbs;

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

        State currentState;
        currentState.distanceToCharacter = calculateDistance(character.position, monster.position);
        currentState.characterAction = characteraction;
        currentState.monsterAction = monsteraction;
        currentState.isDanceComplete = isDanceComplete;
        currentState.distanceToCenter = calculateDistance(monster.position, sf::Vector2f(250.f, 250.f));
        currentState.obstacleProximity = checkObstacleProximity(monster.position, obstaclePositions, 10.f);

        // Record state information to file
        recordDataToFile(currentState);

        if (monsteraction != DANCE)
        {

            cai.getAction(); // implement this function
            mai.getAction(); // implement this function
        }

        // monsteraction = WANDER;

        if (cwanderTimer.getElapsedTime().asSeconds() >= 1.f)
        {
            // Evaluate the decision
            if (monsteraction != DANCE)
                cai.performAction(); // implement this function

            // Restart the timer
            mai.performAction(); // implement this function
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

        float dx;
        float dy;
        // pathfollow for monster
        if (monsterPath.size() > 0)
        {
            dx = monstergoal.position.x - monster.position.x;
            dy = monstergoal.position.y - monster.position.y;
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

            monster.velocity += seek.calculateSteering(monster, monstergoal) * dt.asSeconds() * 1.5f;

            monster.rotation = orientationMatch.calculateSteering(monster, monstergoal).y * dt.asSeconds();
        }

        if (monsteraction == DANCE)
        {
            character.velocity = sf::Vector2f(0.f, 0.f);
            character.rotation = 0;
        }
        // Update character position and ensure it stays within the grid
        character.position += character.velocity * dt.asSeconds();
        character.position.x = std::max(RoomSize / 2.f, std::min(windowSize.x - RoomSize / 2.f, character.position.x));
        character.position.y = std::max(RoomSize / 2.f, std::min(windowSize.y - RoomSize / 2.f, character.position.y));
        character.orientation += character.rotation;

        if (monsteraction == PATHFIND_TO_CENTER)
        {
            // If yes, vibrate the monster rapidly
            monster.rotation = std::sin(frameCount * 0.2f) * 5.f * dt.asSeconds(); // Adjust the frequency and magnitude as needed
        }

        if (monsteraction == DANCE && !isDanceComplete)
        {
            monster.orientation = 0;
            monster.rotation = std::sin(frameCount * 0.5f) * 5.f * dt.asSeconds(); // Adjust the frequency and magnitude as needed
            isDanceSequenceOn = true;
            static int danceStep = 0;
            static sf::Clock danceTimer;
            isDanceComplete = false;

            // Check if it's time to change dance step
            if (danceTimer.getElapsedTime().asSeconds() >= 1.f)
            {
                danceTimer.restart();
                danceStep++;
                if (danceStep == 9)
                {
                    isDanceComplete = true;
                    isDanceSequenceOn = false;
                    danceStep = 0;
                    resetCharacters();
                    monsteraction = 0;
                    characteraction = 0;
                }
            }

            // Perform dance steps based on the current dance step
            if (!isDanceComplete)
            {
                // Adjust the dance steps to keep the monster within the window boundaries
                switch (danceStep)
                {
                case 0: // Move left
                    monster.velocity.x = std::max(-30.f, -monster.position.x);
                    monster.velocity.y = 0.f;
                    break;
                case 1: // Move right
                    monster.velocity.x = std::min(30.f, windowSize.x - monster.position.x);
                    monster.velocity.y = 0.f;
                    break;
                case 2: // Move left
                    monster.velocity.x = std::max(-30.f, -monster.position.x);
                    monster.velocity.y = 0.f;
                    break;
                case 3: // Move right
                    monster.velocity.x = std::min(30.f, windowSize.x - monster.position.x);
                    monster.velocity.y = 0.f;
                    break;
                case 4: // Move up
                    monster.velocity.x = 0.f;
                    monster.velocity.y = std::max(-30.f, -monster.position.y);
                    break;
                case 5: // Move down
                    monster.velocity.x = 0.f;
                    monster.velocity.y = std::min(30.f, windowSize.y - monster.position.y);
                    break;
                case 6: // Move up
                    monster.velocity.x = 0.f;
                    monster.velocity.y = std::max(-30.f, -monster.position.y);
                    break;
                case 7: // Move down
                    monster.velocity.x = 0.f;
                    monster.velocity.y = std::min(30.f, windowSize.y - monster.position.y);
                    break;
                default:
                    std::cout << "Dance completed";
                    break;
                }
            }
        }
        // Update monster position and ensure it stays within the grid
        monster.position += monster.velocity * dt.asSeconds();
        monster.position.x = std::max(RoomSize / 2.f, std::min(windowSize.x - RoomSize / 2.f, monster.position.x));
        monster.position.y = std::max(RoomSize / 2.f, std::min(windowSize.y - RoomSize / 2.f, monster.position.y));
        monster.orientation += monster.rotation;

        monster.orientation += monster.rotation;

        // updating sprites
        c.sprite.setPosition(character.position);
        c.sprite.setRotation(character.orientation * (180.f / M_PI));

        m.sprite.setPosition(monster.position);
        m.sprite.setRotation(monster.orientation * (180.f / M_PI));

        // drawing
        window.clear(sf::Color::White);

        window.draw(m.sprite);

        if (!isDanceSequenceOn)
        {
            Crumb crumb;
            crumb.setPosition(c.sprite.getPosition());
            breadcrumbs.push_back(crumb);

            if (breadcrumbs.size() > 200) // Limiting the number of breadcrumbs
            {
                breadcrumbs.erase(breadcrumbs.begin());
            }

            for (const auto &crumb : breadcrumbs)
            {
                window.draw(crumb);
            }

            MonsterCrumb mcrumb;
            mcrumb.setPosition(m.sprite.getPosition());
            monsterbreadcrumbs.push_back(mcrumb);

            if (monsterbreadcrumbs.size() > 200) // Limiting the number of breadcrumbs
            {
                monsterbreadcrumbs.erase(monsterbreadcrumbs.begin());
            }

            for (const auto &mcrumb : monsterbreadcrumbs)
            {
                window.draw(mcrumb);
            }

            window.draw(m.sprite);
            if (characterPath.size() > 0)
                drawPath(characterPath);

            if (monsterPath.size() > 0)
                drawPath(monsterPath);

            window.draw(c.sprite);
            drawObstacles();
        }

        // displaying window
        window.display();
    }
}