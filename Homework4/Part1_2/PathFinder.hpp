// PathFinder.hpp
#pragma once
#include <SFML/System/Vector2.hpp>

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

void getGrid(std::vector<std::vector<int>> &gridRef)
{
    grid = gridRef;
}

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

        // Add diagonal neighbors
        if (isValid(x - 1, y - 1))
            successors.push_back(sf::Vector2i(x - 1, y - 1));
        if (isValid(x - 1, y + 1))
            successors.push_back(sf::Vector2i(x - 1, y + 1));
        if (isValid(x + 1, y - 1))
            successors.push_back(sf::Vector2i(x + 1, y - 1));
        if (isValid(x + 1, y + 1))
            successors.push_back(sf::Vector2i(x + 1, y + 1));

        return successors;
    }

    double heuristic(sf::Vector2i a, sf::Vector2i b)
    {
        // Euclidean distance heuristic
        return std::sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
    }

public:
    std::vector<sf::Vector2i> findPath(sf::Vector2f character, sf::Vector2f goal)
    {
        sf::Vector2i start = sf::Vector2i((int)character.x / RoomSize, (int)character.y / RoomSize);
        sf::Vector2i end = sf::Vector2i((int)goal.x / RoomSize, (int)goal.y / RoomSize);

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
