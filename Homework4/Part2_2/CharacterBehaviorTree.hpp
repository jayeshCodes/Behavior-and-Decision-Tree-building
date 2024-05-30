// CharacterBehaviorTree.hpp
#pragma once

#include <iostream>
#include <vector>

#include "SteeringData.hpp"


std::vector<float> characterfeatures;



enum ActionType
{
    WANDER,
    PATHFIND_TO_CENTER, // to center of screen
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
    virtual NodeState Evaluate(const SteeringData &character, const SteeringData &monster) = 0;
    NodeState nodeState() const
    {
        return _nodeState;
    }
};

class Sequence : public Node {
protected:
    std::vector<Node*> nodes;

public:
    Sequence(const std::vector<Node*>& nodes) : nodes(nodes) {}

    const std::vector<Node*>& getNodes() const {
        return nodes;
    }

    NodeState Evaluate(const SteeringData& character, const SteeringData& monster)  override {
        bool isAnyNodeRunning = false;
        for (auto node : nodes) {
            NodeState result = node->Evaluate(character, monster); // Evaluate child node with character and monster data
            switch (result) {
                case NodeState::RUNNING:
                    isAnyNodeRunning = true;
                    break;
                case NodeState::FAILURE:
                    _nodeState = NodeState::FAILURE;
                    return _nodeState;
                case NodeState::SUCCESS:
                    break;
            }
        }
        _nodeState = isAnyNodeRunning ? NodeState::RUNNING : NodeState::SUCCESS;
        return _nodeState;
    }
};

class Selector : public Node {
protected:
    std::vector<Node*> nodes;

public:
    Selector(const std::vector<Node*>& nodes) : nodes(nodes) {}

     const std::vector<Node*>& getNodes() const {
        return nodes;
    }

    NodeState Evaluate(const SteeringData& character, const SteeringData& monster) override {
        for (auto node : nodes) {
            NodeState result = node->Evaluate(character, monster); // Evaluate child node with character and monster data
            switch (result) {
                case NodeState::RUNNING:
                    _nodeState = NodeState::RUNNING;
                    return _nodeState;
                case NodeState::SUCCESS:
                    _nodeState = NodeState::SUCCESS;
                    return _nodeState;
                case NodeState::FAILURE:
                    break;
            }
        }
        _nodeState = NodeState::FAILURE;
        return _nodeState;
    }
};

class Inverter : public Node {
protected:
    Node* node;

public:
    Inverter(Node* node) : node(node) {}

    NodeState Evaluate(const SteeringData& character, const SteeringData& monster) override {
        NodeState result = node->Evaluate(character, monster); // Evaluate child node with character and monster data
        switch (result) {
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


class ActionNode : public Node {
protected:
    ActionType action;

public:
    ActionNode(ActionType action) : action(action) {}

      ActionType getAction() const {
        return action;
    }

    NodeState Evaluate(const SteeringData& character, const SteeringData& monster) override {
        switch (action) {
            case EVADE:
                // Determine if character should pursue the monster
                if (calculateDistance(character.position, monster.position) <= 100.f) { // Assuming 100 units as the pursuit range
                    std::cout << "Character evading the monster.\n";
                    _nodeState = NodeState::SUCCESS;
                } else {
                    std::cout << "Character wandering around.\n";
                    _nodeState = NodeState::FAILURE;
                }
                break;

            case WANDER:
                // Character always wanders
                std::cout << "Character wandering around.\n";
                _nodeState = NodeState::SUCCESS;
                break;

            default:
                // Handle unknown action
                _nodeState = NodeState::FAILURE;
                break;
        }

        return _nodeState;
    }

private:
    float calculateDistance(const sf::Vector2f& position1, const sf::Vector2f& position2) {
        return std::sqrt(std::pow(position1.x - position2.x, 2) + std::pow(position1.y - position2.y, 2));
    }
};

Node* constructCharacterTree()
{
    // Define actions
    ActionNode *danceAction = new ActionNode(DANCE);
    ActionNode *wanderAction = new ActionNode(WANDER);
    ActionNode *zigzagAction = new ActionNode(ZIGZAG);
    ActionNode *evadeAction = new ActionNode(EVADE);

    // Create nodes
    Sequence *sequence1 = new Sequence({danceAction, wanderAction});
    Inverter *inverter = new Inverter(new Selector({zigzagAction, evadeAction}));
    Sequence *rootSequence = new Sequence({new Selector({inverter, sequence1})});

    // Note: You may want to delete danceAction, wanderAction, zigzagAction,
    // evadeAction, sequence1, and inverter here if they are no longer needed.

    return rootSequence;
}

ActionType decide(Node* node, const SteeringData& character, const SteeringData& monster)
{
    // Evaluate the current node
    NodeState result = node->Evaluate(character, monster);

    // Check the result of the current node
    switch (result)
    {
    case NodeState::SUCCESS:
        // If the current node succeeds, return the action associated with it
        if (ActionNode* actionNode = dynamic_cast<ActionNode*>(node))
        {
            return actionNode->getAction(); // Assuming there's a method to get the action from ActionNode
        }
        break;

    case NodeState::FAILURE:
        // If the current node fails, continue traversal
        break;

    case NodeState::RUNNING:
        // If the current node is running, continue traversal
        break;

    default:
        // Handle other cases if necessary
        break;
    }

    // If the current node is a composite node (Sequence or Selector), recursively traverse its children
    if (Sequence* sequenceNode = dynamic_cast<Sequence*>(node))
    {
        for (Node* child : sequenceNode->getNodes())
        {
            // Recursively call decide for each child
            ActionType action = decide(child, character, monster);
            // If any child returns an action, return it
            if (action != WANDER)
            {
                return action;
            }
        }
    }
    else if (Selector* selectorNode = dynamic_cast<Selector*>(node))
    {
        for (Node* child : selectorNode->getNodes())
        {
            // Recursively call decide for each child
            ActionType action = decide(child, character, monster);
            // If any child returns an action, return it
            if (action != WANDER)
            {
                return action;
            }
        }
    }
    // If no action is returned by any node, default to WANDER
    return WANDER;
}
