// BehaviorTreeNode.hpp
#pragma once
#include <vector>
#include "SteeringData.hpp"
#include "ActionType.hpp"
#include <iostream>
#include <SFML/System/Vector2.hpp>

SteeringData character, monster;

int characteraction;
int monsteraction;

bool isDanceComplete;


enum NodeState
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

    virtual ~Node() = default;
};

class SequenceNode : public Node
{
protected:
    std::vector<Node *> nodes;

public:
    SequenceNode(const std::vector<Node *> &n) : nodes(n) {}

    ~SequenceNode()
    {
        for (auto &node : nodes)
            delete node;
    }

    NodeState Evaluate() override
    {

        bool isAnyNodeRunning = false;
        for (auto &node : nodes)
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

class SelectorNode : public Node
{
protected:
    std::vector<Node *> nodes;

public:
    SelectorNode(const std::vector<Node *> &n) : nodes(n) {}

    ~SelectorNode()
    {
        for (auto &node : nodes)
            delete node;
    }

    NodeState Evaluate() override
    {
        for (auto &node : nodes)
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

class InverterNode : public Node
{
protected:
    Node *node;

public:
    InverterNode(Node *n) : node(n) {}

    ~InverterNode()
    {
        delete node;
    }

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

class DanceNode : public Node
{
public:
    NodeState Evaluate() override
    {
        if (!isDanceComplete)
        {
            // monsteraction = DANCE;
            return RUNNING;
        }
        if (isDanceComplete)
        {
            return SUCCESS;
        }
    }
};
class EvadeNode : public Node
{
public:
    NodeState Evaluate() override
    {
        float distance = calculateDistanceBetween(character.position, monster.position);
        if (distance <= 20 && distance > 10)
        {
            characteraction = EVADE;
            return RUNNING;
        }
        else if (distance <= 10)
        {

            return FAILURE;
        }
        else
        {

            return SUCCESS;
        }
    }

private:
    float calculateDistanceBetween(const sf::Vector2f &p1, const sf::Vector2f &p2)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};
class PursueNode : public Node
{
public:
    NodeState Evaluate() override
    {
        float distance = calculateDistanceBetween(character.position, monster.position);
        if (distance < 20.f && distance > 2.f)
        {
            // monsteraction = PURSUE;
            return RUNNING;
        }
        else if (distance <= 2.f)
            return SUCCESS;
        else
            return FAILURE;
    }

private:
    float calculateDistanceBetween(const sf::Vector2f &p1, const sf::Vector2f &p2)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};
class RangeNode : public Node
{
public:
    NodeState Evaluate() override
    {
        float distance = calculateDistanceBetween(character.position, monster.position);
        if (distance < 50.f && distance > 10.f)
        {
            return SUCCESS;
        }
        else
            return FAILURE;
    }

private:
    float calculateDistanceBetween(const sf::Vector2f &p1, const sf::Vector2f &p2)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};
class RangeNode2 : public Node
{
public:
    NodeState Evaluate() override
    {
        float distance = calculateDistanceBetween(character.position, monster.position);
        return distance <= 10 ? SUCCESS : FAILURE;
    }

private:
    float calculateDistanceBetween(const sf::Vector2f &p1, const sf::Vector2f &p2)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};
class WanderNode : public Node
{
public:
    NodeState Evaluate() override
    {
        if (calculateDistanceBetween(character.position, monster.position) > 50.f && calculateDistanceBetween(character.position, monster.position) <= 60)
        {
            characteraction = WANDER;
            return RUNNING;
        }
        else
            return FAILURE;
    }

private:
    float calculateDistanceBetween(const sf::Vector2f &p1, const sf::Vector2f &p2)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

class ZigZagNode : public Node
{
public:
    NodeState Evaluate() override
    {
        float distance = calculateDistanceBetween(character.position, monster.position);
        if (distance <= 10 && distance > 5)
        {
            characteraction = ZIGZAG;
            return RUNNING;
        }
        else if (distance <= 5)
        {
            return FAILURE;
        }
        else
        {
            return SUCCESS;
        }
    }

private:
    float calculateDistanceBetween(const sf::Vector2f &p1, const sf::Vector2f &p2)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

class PathFindNode : public Node
{
public:
    NodeState Evaluate() override
    {
        float distance = calculateDistanceBetween(monster.position, character.position);
        if (distance <= 60.f && distance>50.f)
        {
            return SUCCESS;
        }
        else if (distance > 60.f)
        {
            // monsteraction = PATHFIND_TO_CENTER;

            return RUNNING;
        }
        else
            return FAILURE;
    }

private:
    float calculateDistanceBetween(const sf::Vector2f &p1, const sf::Vector2f &p2)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

class CharacterBehaviorTree
{
public:
    Node *constructBehaviorTree()
    {
        // Construct the behavior tree for the character
        Node *wander = new WanderNode();
        Node *evade = new EvadeNode();
        Node *dance = new DanceNode();
        Node *range1 = new RangeNode();
        Node *range2 = new RangeNode2();
        Node *pathfind = new PathFindNode();

        Node *invert1 = new InverterNode(range1);
        Node *invert2 = new InverterNode(range2);

        std::vector<Node *> sequence1Nodes = {invert1, pathfind, wander};
        Node *sequence1 = new SequenceNode(sequence1Nodes);

        std::vector<Node *> sequence2Nodes = {range1, invert2, evade};
        Node *sequence2 = new SequenceNode(sequence2Nodes);

        std::vector<Node *> sequence3Nodes = {range2, new ZigZagNode()};
        Node *sequence3 = new SequenceNode(sequence3Nodes);

        std::vector<Node *> selectorNodes = {sequence1, sequence2, sequence3};
        Node *selector = new SelectorNode(selectorNodes);

        return selector;
    }
};

