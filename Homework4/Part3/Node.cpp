// Node.cpp
// For node structure of behavior tree

#include "SteeringData.hpp"
#include "SteeringBehavior.hpp"
// #include "PathFinder.hpp"

#include <iostream>
#include <SFML/System/Vector2.hpp>

#include <vector>
#include <cmath>
#include <memory>
#include <numeric>
#include <set>

enum ActionType
{
    WANDER,
    PATHFIND, // to center of screen
    ZIGZAG,
    EVADE,
    PURSUE,
    DANCE
};

enum class NodeState
{
    RUNNING,
    SUCCESS,
    FAILURE
};

class Node
{
protected:
    NodeState _nodeState;

public:
    NodeState nodeState() const
    {
        return _nodeState;
    }

    virtual NodeState Evaluate() = 0;
};

class Sequence : public Node
{
protected:
    std::vector<Node *> nodes;

public:
    Sequence(const std::vector<Node *> &nodes) : nodes(nodes) {}

    NodeState Evaluate() override
    {
        bool isAnyNodeRunning = false;
        for (auto node : nodes)
        {
            switch (node->Evaluate())
            {
            case NodeState::RUNNING:
                isAnyNodeRunning = true;
                break;

            case NodeState::SUCCESS:
                break;

            case NodeState::FAILURE:
                _nodeState = NodeState::FAILURE;
                return _nodeState;

            default:
                break;
            }
        }
        _nodeState = isAnyNodeRunning ? NodeState::RUNNING : NodeState::SUCCESS;
        return _nodeState;
    }
};

class Selector : public Node
{
protected:
    std::vector<Node *> nodes;

public:
    Selector(const std::vector<Node *> &nodes) : nodes(nodes) {}

    NodeState Evaluate() override
    {
        for (auto node : nodes)
        {
            switch (node->Evaluate())
            {
            case NodeState::RUNNING:
                _nodeState = NodeState::RUNNING;
                return _nodeState;

            case NodeState::SUCCESS:
                _nodeState = NodeState::SUCCESS;
                return _nodeState;

            case NodeState::FAILURE:
                break;

            default:
                break;
            }
        }
        _nodeState = NodeState::FAILURE;
        return _nodeState;
    }
};

class Inverter : public Node
{
protected:
    Node *node;

public:
    Inverter(Node *node) : node(node) {}

    NodeState Evaluate() override
    {
        switch (node->Evaluate())
        {
        case NodeState::RUNNING:
            _nodeState = NodeState::RUNNING;
            break;

        case NodeState::SUCCESS:
            _nodeState = NodeState::FAILURE;
            break;

        case NodeState::FAILURE:
            _nodeState = NodeState::SUCCESS;
            break;

        default:
            break;
        }
        return _nodeState;
    }
};

// class BehaviorTreeNode : public Node
// {
// public:
//     virtual ActionType runBehavior(SteeringData &character, SteeringData &goal) = 0;
// };

// class Sequence : public BehaviorTreeNode
// {
// protected:
//     std::vector<BehaviorTreeNode *> nodes;

// public:
//     Sequence(const std::vector<BehaviorTreeNode *> &nodes) : nodes(nodes) {}

//     NodeState Evaluate() override
//     {
//         for (auto node : nodes)
//         {
//             NodeState result = node->Evaluate();
//             if (result != NodeState::SUCCESS)
//             {
//                 return result;
//             }
//         }
//         return NodeState::SUCCESS;
//     }

//     ActionType runBehavior(SteeringData &character, SteeringData &goal) override
//     {
//         NodeState state = Evaluate();
//         if (state == NodeState::SUCCESS)
//         {
//             return ActionType::SUCCESS;
//         }
//         else if (state == NodeState::RUNNING)
//         {
//             return ActionType::WANDER; // Or any appropriate action
//         }
//         else
//         {
//             return ActionType::FAILURE;
//         }
//     }
// };

// class Selector : public BehaviorTreeNode
// {
// protected:
//     std::vector<BehaviorTreeNode *> nodes;

// public:
//     Selector(const std::vector<BehaviorTreeNode *> &nodes) : nodes(nodes) {}

//     NodeState Evaluate() override
//     {
//         for (auto node : nodes)
//         {
//             NodeState result = node->Evaluate();
//             if (result != NodeState::FAILURE)
//             {
//                 return result;
//             }
//         }
//         return NodeState::FAILURE;
//     }

//     ActionType runBehavior(SteeringData &character, SteeringData &goal) override
//     {
//         NodeState state = Evaluate();
//         if (state == NodeState::SUCCESS)
//         {
//             return ActionType::SUCCESS;
//         }
//         else if (state == NodeState::RUNNING)
//         {
//             return ActionType::WANDER; // Or any appropriate action
//         }
//         else
//         {
//             return ActionType::FAILURE;
//         }
//     }
// };

// class MonsterBehavior : public BehaviorTreeNode
// {
// private:
//     SteeringData &monster;
//     SteeringData &player;

// public:
//     MonsterBehavior(SteeringData &monster, SteeringData &player) : monster(monster), player(player) {}

//     ActionType runBehavior(SteeringData &character, SteeringData &goal) override
//     {
//         // Calculate distance between monster and player
//         float distanceToPlayer = calculateDistance(monster.position, player.position);

//         if (distanceToPlayer < 100.0f)
//         { // If player is within a certain range, chase
//             goal.position = player.position;
//             return ActionType::ATTACK; // Or any appropriate action
//         }
//         else
//         {
//             // Wander randomly
//             goal.position = selectRandom(monster.position);
//             return ActionType::WANDER; // Or any appropriate action
//         }
//     }

// private:
//     float calculateDistance(const sf::Vector2f &pos1, const sf::Vector2f &pos2)
//     {
//         float dx = pos1.x - pos2.x;
//         float dy = pos1.y - pos2.y;
//         return std::sqrt(dx * dx + dy * dy);
//     }

//     sf::Vector2f selectRandom(sf::Vector2f currentPosition)
//     {
//         // Define the maximum offset from the current position
//         float maxOffset = 50.f;

//         // Generate random offsets within the range [-maxOffset, maxOffset]
//         float offsetX = static_cast<float>(std::rand() % static_cast<int>(2 * maxOffset) - maxOffset);
//         float offsetY = static_cast<float>(std::rand() % static_cast<int>(2 * maxOffset) - maxOffset);

//         // Calculate the new position
//         sf::Vector2f newPosition = currentPosition + sf::Vector2f(offsetX, offsetY);

//         return newPosition;
//     }
// };
