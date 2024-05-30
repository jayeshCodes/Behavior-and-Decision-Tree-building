// Part2.cpp

#include "DecisionTree.hpp"
#include "SteeringData.hpp"
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

// Grid dimensions
const int GridSizeX = 50;
const int GridSizeY = 50;
std::vector<std::vector<int>> grid(GridSizeX, std::vector<int>(GridSizeY, 0)); // creating a 2D grid with GridSizeX rows and GridSizeY columns

// Dimensions of the rooms
const float RoomSize = 10.0f;


void createObstacles()
{
    for (int i = 0; i < 25; i++)
    {
        grid[i][20] = 1;
    }
    for (int i = 0; i < 15; i++)
    {
        grid[i][10] = 1;
    }
    for (int i = 0; i < 15; i++)
    {
        grid[24][i] = 1;
    }
    for (int i = 49; i > 30; i--)
    {
        grid[i][24] = 1;
    }
    for (int i = 0; i < 15; i++)
    {
        grid[40][i] = 1;
    }
    for (int i = 0; i <= 5; i++)
    {
        grid[40 + i][15 + i] = 1;
    }
    for (int i = 0; i <= 5; i++)
    {
        grid[40 - i][15 + i] = 1;
    }

    for (int i = 0; i < 22; i++)
    {
        grid[30][i] = 1;
    }
    for (int i = 0; i < 49; i++)
    {
        grid[i][30] = 1;
    }
    for (int i = 0; i < 49; i++)
    {
        grid[i][30] = 1;
    }
    for (int i = 49; i > 33; i--)
    {
        grid[29][i] = 1;
    }
    for (int i = 29; i > 0; i--)
    {
        grid[i][42] = 1;
    }
    std::cout << "Obstacles created" << std::endl;
}

struct Node
{
    int x, y;
    double f, g, h;
    Node *parent;

    Node(int x, int y, double g, double h, Node *parent) : x(x), y(y), g(g), h(h), parent(parent)
    {
        f = g + h;
    }
};

class Astar
{
private:
    bool isValid(int x, int y)
    {
        return x >= 0 && x < GridSizeX && y < GridSizeY && y >= 0 && grid[x][y] != 1;
    }

    double heuristic(sf::Vector2i a, sf::Vector2i b)
    {
        // Euclidean distance heuristic
        return std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2));
    }

    std::vector<sf::Vector2i> generateSuccessors(int x, int y)
    {
        std::vector<sf::Vector2i> successors;
        // Add neighboring cells (up, down, left, right)
        if (isValid(x - 1, y))
            successors.push_back(sf::Vector2i(x - 1, y));
        if (isValid(x, y - 1))
            successors.push_back(sf::Vector2i(x, y - 1));
        if (isValid(x + 1, y))
            successors.push_back(sf::Vector2i(x + 1, y));
        if (isValid(x, y + 1))
            successors.push_back(sf::Vector2i(x, y + 1));
        return successors;
    }

public:
    std::vector<sf::Vector2i> findPath(sf::Vector2i start, sf::Vector2i end)
    {
        std::vector<sf::Vector2i> path;

        // Open and closed lists
        std::vector<Node *> openList;
        std::unordered_set<Node *> closedList;

        // Create the start node
        Node *startNode = new Node(start.x, start.y, 0.0, heuristic(start, end), nullptr);
        openList.push_back(startNode);

        // A* algorithm
        while (!openList.empty())
        {
            // Get the node with the lowest f score from the open list
            auto currentIt = std::min_element(openList.begin(), openList.end(), [](Node *a, Node *b)
                                              { return a->f < b->f; });
            Node *current = *currentIt;
            openList.erase(currentIt);

            // Add current node to closed list
            closedList.insert(current);

            // Check if current node is the goal
            if (current->x == end.x && current->y == end.y)
            {
                // Reconstruct path
                while (current != nullptr)
                {
                    path.push_back(sf::Vector2i(current->x, current->y));
                    current = current->parent;
                }
                std::reverse(path.begin(), path.end());
                break;
            }

            // Generate successors
            std::vector<sf::Vector2i> successors = generateSuccessors(current->x, current->y);
            for (const auto &successor : successors)
            {
                // Check if successor is in the closed list
                if (closedList.find(new Node(successor.x, successor.y, 0.0, 0.0, nullptr)) != closedList.end())
                {
                    continue;
                }

                // Calculate tentative g score
                double tentativeG = current->g + 1.0;

                // Check if successor is in the open list or not
                bool inOpenList = false;
                for (Node *node : openList)
                {
                    if (node->x == successor.x && node->y == successor.y)
                    {
                        inOpenList = true;
                        break;
                    }
                }

                // If successor is not in the open list or new g score is lower
                if (!inOpenList || tentativeG < current->g)
                {
                    Node *successorNode = new Node(successor.x, successor.y, tentativeG, heuristic(successor, end), current);
                    if (!inOpenList)
                    {
                        openList.push_back(successorNode);
                    }
                }
            }
        }

        // Cleanup
        for (Node *node : openList)
        {
            delete node;
        }
        for (Node *node : closedList)
        {
            delete node;
        }

        return path;
    }
};

DecisionTreePart1 dtree;
Astar a;

int action;

sf::Vector2i start;
sf::Vector2i end;

sf::RenderWindow window(sf::VideoMode(GridSizeX *RoomSize, GridSizeY *RoomSize), "DTrees");
sf::Vector2u windowSize = window.getSize();

std::vector<sf::Vector2i> path;

class GameAI
{
public:
    void update(SteeringData &character, SteeringData &goal)
    {
        std::vector<int> features = extractFeatures(character);
        action = dtree.takeAction(features, character, goal, windowSize);
    }

    void evaluateAction(SteeringData &character, SteeringData &goal)
    {
        switch (action)
        {
        case WANDER:
            std::cout << WANDER << std::endl;
            break;

        case PATHFIND:
            start = sf::Vector2i(0, 0);
            end = sf::Vector2i(10, 10);
            path = a.findPath(start, end);
            break;

        default:
            std::cout << "No action" << std::endl;
            break;
        }
    }

private:
    std::vector<int> extractFeatures(SteeringData &character)
    {
        std::vector<int> features;

        // Extract features based on character's position
        if (character.position.x < 25.f || character.position.x > 675.f)
            features.push_back(1);
        else
            features.push_back(0);

        if (character.position.y < 25.f || character.position.y > 675.f)
            features.push_back(1);
        else
            features.push_back(0);

        // Extract features based on character's velocity
        float absoluteVelocity = std::sqrt(character.velocity.x * character.velocity.x + character.velocity.y * character.velocity.y);
        if (absoluteVelocity > 50.f)
            features.push_back(1);
        else
            features.push_back(0);

        return features;
    }
};

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
    window.setFramerateLimit(60);

    // init steeringdata
    SteeringData character, goal;

    // init clock
    sf::Clock clock;

    // init sprite
    CustomSprite sprite;
    sprite.sprite.setPosition(character.position);

    // init obstacles
    createObstacles();

    // init path (empty)
    std::vector<sf::Vector2i> path;

    // init AI
    GameAI ai;

    end = sf::Vector2i(250, 250);

    // crumbs
    std::vector<Crumb> breadcrumbs;
    sf::CircleShape circle;
    circle.setRadius(5); // Set the radius of the circle
    circle.setFillColor(sf::Color::Red);

    while (window.isOpen())
    {
        sf::Event event;
        sf::Time dt = clock.restart();
        while (window.pollEvent(event))
        {

            if (event.type == sf::Event::Closed)
                window.close();
        }

        // updation of character happens only here in the main loop
        ai.update(character, goal);
        ai.evaluateAction(character, goal);
        
        // path = a.findPath(start, end);
        if (!path.empty())
        {
            std::cout<<"Path found"<<std::endl;
            goal.position = sf::Vector2f(path[0].x * RoomSize + RoomSize / 2, path[0].y * RoomSize + RoomSize / 2);
            path.erase(path.begin());
            // std::cout << "First waypoint:: " << goal.position.x << "," << goal.position.y << std::endl;
        }

        // drawing logic
        window.clear(sf::Color::White);

        // Rooms and obstacles
        drawObstacles();
        if (path.size()>0)
            drawPath(path);

        // sf::RectangleShape endRect(sf::Vector2f(RoomSize, RoomSize));
        // endRect.setPosition(end.x * RoomSize, end.y * RoomSize);
        // endRect.setFillColor(sf::Color::Red);
        // window.draw(endRect);

        window.draw(sprite.sprite);

        for (const auto &crumb : breadcrumbs)
        {
            window.draw(crumb);
        }

        window.display();
    }

    return 0;
}
