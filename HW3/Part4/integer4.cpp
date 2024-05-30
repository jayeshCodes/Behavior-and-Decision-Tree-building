#include <SFML/Graphics.hpp>
#include <iostream>
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
const int GridSizeX = 100;
const int GridSizeY = 100;
std::vector<std::vector<int>> grid(GridSizeX, std::vector<int>(GridSizeY, 0)); // creating a 2D grid with GridSizeX rows and GridSizeY columns

// Dimensions of the rooms and obstacles
const float RoomSize = 10.0f;
const float ObstacleSize = 20.0f;
const int NumRooms = 3;
const int NumObstacles = 3;

sf::Vector2i start;
sf::Vector2i end;

sf::RenderWindow window(sf::VideoMode(GridSizeX *RoomSize, GridSizeY *RoomSize), "PathFinding");

class CustomSprite
{
public:
    sf::Sprite sprite;
    sf::Texture texture;

    CustomSprite()
    {
        if (!texture.loadFromFile("boid-sm.png"))
        {
            std::cout << "texture not found";
        }

        sprite.setTexture(texture);
        // sprite.setScale(0.05f, 0.05f);
        sprite.setOrigin(sprite.getLocalBounds().width / 2.f, sprite.getLocalBounds().height / 2.f);
        sprite.setPosition(320.f, 240.f);
        sprite.setRotation(0.f);
    }
};

void createObstacles()
{
    for (int i = 0; i < 35; i++)
    {
        grid[i][49] = 1;
    }

    for (int i = 0; i < 50; i++)
    {
        grid[40][i] = 1;
    }

    for (int i = 0; i < 10; i++)
    {
        grid[15 + i][21 - i] = 1;
    }

    for (int i = 0; i < 50; i++)
    {
        grid[99 - i][55] = 1;
    }

    for (int i = 0; i < 50; i++)
    {
        grid[50][55 - i] = 1;
    }
    for (int i = 0; i < 45; i++)
    {
        grid[i][69] = 1;
    }
    for (int i = 0; i < 25; i++)
    {
        grid[0 + i][99 - i] = 1;
    }

    for (int i = 99; i > 51; i--)
    {
        grid[i][61] = 1;
    }

    for (int i = 99; i > 66; i--)
    {
        grid[51][i] = 1;
    }
    for (int i = 99; i > 58; i--)
    {
        grid[i][31] = 1;
    }
}

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

int main()
{
    // SteeringData character;
    // SteeringData goal;
    // PositionMatchingBehavior positionMatch;
    // OrientationMatchingBehavior orientationMatch;
    // sf::Clock clock;

    CustomSprite sprite;

    window.setFramerateLimit(60);

    createObstacles();

    std::vector<sf::Vector2i> path;

    bool drawpath = false;

    while (window.isOpen())
    {
        sf::Event event;
        // sf::Time dt = clock.restart();
        while (window.pollEvent(event))
        {

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    // get mouse position and quantize it to grid
                    int x = event.mouseButton.x / RoomSize;
                    int y = event.mouseButton.y / RoomSize;

                    if (grid[x][y] != 1)
                    {
                        start = {x, y};
                        std::cout << "Start position(node): " << start.x << "," << start.y << std::endl;
                    }
                }

                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    int x = event.mouseButton.x / RoomSize;
                    int y = event.mouseButton.y / RoomSize;

                    if (grid[x][y] != 1)
                    {
                        end = {x, y};
                        std::cout << "End position(node): " << end.x << "," << end.y << std::endl;

                        // Find path
                        path = findPath(start, end);

                        // // print path
                        std::cout << "Path: ";
                        for (const auto &node : path)
                        {
                            std::cout << "(" << node.x << ", " << node.y << ") -> ";
                        }
                        std::cout << std::endl;

                        drawpath = true;
                    }
                }
            }
        }

        // if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        // {
        //     // Set the goal position to the mouse click position
        //     goal.position = sf::Vector2f(sf::Mouse::getPosition(window));

        //     // Find the path using A*
        //     path = findPath(character.position, goal.position);

        //     // If path is not empty, set the first waypoint as the goal
        //     if (!path.empty())
        //     {
        //         // Set the goal position to the first waypoint in the path
        //         goal.position = sf::Vector2f(path[0].x * RoomSize + RoomSize / 2, path[0].y * RoomSize + RoomSize / 2);

        //         // Remove the current position (first waypoint) from the path
        //         path.erase(path.begin());
        //     }
        // }

        // if (!path.empty())
        // {
        //     // Calculate orientation towards the next waypoint
        //     float dx = goal.position.x - character.position.x;
        //     float dy = goal.position.y - character.position.y;
        //     goal.orientation = atan2(dy, dx);

        //     // Update character position and orientation using ArriveAndAlign
        //     character.position = sprite.sprite.getPosition();
        //     character.velocity = positionMatch.calculateSteering(character, goal);
        //     character.orientation += orientationMatch.calculateSteering(character, goal).y * dt.asSeconds();
        //     sprite.sprite.setPosition(character.position);
        //     sprite.sprite.setRotation(character.orientation * (180.f / M_PI));

        //     // Check if the character has reached the current waypoint
        //     float distanceToWaypoint = std::sqrt(dx * dx + dy * dy);
        //     if (distanceToWaypoint < RoomSize / 2)
        //     {
        //         // Remove the current waypoint from the path
        //         path.erase(path.begin());

        //         // If there are remaining waypoints, set the next waypoint as the goal
        //         if (!path.empty())
        //         {
        //             goal.position = sf::Vector2f(path[0].x * RoomSize + RoomSize / 2, path[0].y * RoomSize + RoomSize / 2);
        //         }
        //     }
        // }

        window.clear(sf::Color::White);

        // Rooms and obstacles
        drawObstacles();

        if (drawpath)
            drawPath(path);

        // Start and end points
        sf::RectangleShape startRect(sf::Vector2f(RoomSize, RoomSize));
        startRect.setPosition(start.x * RoomSize, start.y * RoomSize);
        startRect.setFillColor(sf::Color::Green);
        window.draw(startRect);

        sf::RectangleShape endRect(sf::Vector2f(RoomSize, RoomSize));
        endRect.setPosition(end.x * RoomSize, end.y * RoomSize);
        endRect.setFillColor(sf::Color::Red);
        window.draw(endRect);

        window.display();
    }

    return 0;
}
