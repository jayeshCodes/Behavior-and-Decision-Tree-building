// BehaviorTree.hpp
#pragma once
#include <iostream>
#include <memory>
#include <cmath>
#include "SteeringData.hpp"

SteeringData character, monster, goal, monstergoal;

void getSteeringData(SteeringData &character, SteeringData &monster)
{
    this.character = character;
    this.monster = monster;
}

enum ActionType
{
    WANDER,
    EVADE,
    ZIGZAG,
    PATHFIND_TO_CENTER,
    PURSUE,
    DANCE
};

// Forward declarations
class BehaviorTreeNode;
class ConditionNode;
class ActionNode;
class SequenceNode;
class NotDecoratorNode;

// Base class for all behavior tree nodes
class BehaviorTreeNode
{
public:
    virtual bool run() = 0;
};

// Condition node class
class ConditionNode : public BehaviorTreeNode
{
private:
    std::string conditionName;

public:
    ConditionNode(const std::string &name) : conditionName(name) {}

    bool checkCondition()
    {
        // Implement condition check logic here
        bool isMonsterWithin50Units = (calculateDistance(character.position, monster.position) < 50.f);
        bool isMonsterWithin10Units = (calculateDistance(character.position, monster.position) < 10.f);
        if (conditionName == "MonsterWithin50Units")
        {
            // Simulated condition check, always returns true for demonstration
            std::cout << conditionName << std::endl;
            return true;
        }
        else if (conditionName == "MonsterWithin10Units")
        {
            // Simulated condition check, always returns true for demonstration
            std::cout << conditionName << std::endl;
            return true;
        }
        else if (conditionName == "CharacterWithin50Units")
        {
            // Simulated condition check, always returns true for demonstration
            std::cout << conditionName << std::endl;
            return true;
        }
        return false;
    }

    bool run() override
    {
        return checkCondition();
    }
};

// Action node class
class ActionNode : public BehaviorTreeNode
{
private:
    std::string actionName;

public:
    ActionNode(const std::string &name) : actionName(name) {}

    bool performAction()
    {
        // Implement action logic here
        std::cout << "Performing action: " << actionName << std::endl;
        // Simulated action, always returns true for demonstration
        return true;
    }

    bool run() override
    {
        return performAction();
    }
};

// Sequence node class
class SequenceNode : public BehaviorTreeNode
{
private:
    std::vector<std::unique_ptr<BehaviorTreeNode>> children;

public:
    void addChild(std::unique_ptr<BehaviorTreeNode> child)
    {
        children.push_back(std::move(child));
    }

    bool run() override
    {
        for (auto &child : children)
        {
            if (!child->run())
            {
                return false; // Failure if any child fails
            }
        }
        return true; // Success if all children succeed
    }
};

// Not decorator node class
class NotDecoratorNode : public BehaviorTreeNode
{
private:
    std::unique_ptr<ConditionNode> condition;

public:
    NotDecoratorNode(std::unique_ptr<ConditionNode> cond) : condition(std::move(cond)) {}

    bool run() override
    {
        return !condition->run(); // Inverts the result of the condition
    }
};

BehaviorTreeNode *constructCharacterTree()
{
    // Character Behavior Tree
    std::unique_ptr<ConditionNode> monsterNotWithin50Units = std::make_unique<ConditionNode>("MonsterWithin50Units");
    std::unique_ptr<ConditionNode> monsterWithin10Units = std::make_unique<ConditionNode>("MonsterWithin10Units");

    std::unique_ptr<ActionNode> wanderAction = std::make_unique<ActionNode>("Wander");
    std::unique_ptr<ActionNode> evadeAction = std::make_unique<ActionNode>("Evade");
    std::unique_ptr<ActionNode> zigzagAction = std::make_unique<ActionNode>("Zigzag");

    std::unique_ptr<SequenceNode> sequence1 = std::make_unique<SequenceNode>();
    sequence1->addChild(std::make_unique<NotDecoratorNode>(std::move(monsterNotWithin50Units)));
    sequence1->addChild(std::move(wanderAction));

    std::unique_ptr<SequenceNode> sequence2 = std::make_unique<SequenceNode>();
    sequence2->addChild(std::make_unique<ConditionNode>("MonsterWithin50Units"));
    sequence2->addChild(std::make_unique<NotDecoratorNode>(std::move(monsterWithin10Units)));
    sequence2->addChild(std::move(evadeAction));

    std::unique_ptr<SequenceNode> sequence3 = std::make_unique<SequenceNode>();
    sequence3->addChild(std::make_unique<ConditionNode>("MonsterWithin10Units"));
    sequence3->addChild(std::move(zigzagAction));

    std::unique_ptr<SequenceNode> root = std::make_unique<SequenceNode>();
    root->addChild(std::move(sequence1));
    root->addChild(std::move(sequence2));
    root->addChild(std::move(sequence3));

    return root.release();
}

BehaviorTreeNode *constructMonsterTree()
{
    // Monster Behavior Tree
    std::unique_ptr<ConditionNode> characterNotWithin50Units = std::make_unique<ConditionNode>("CharacterWithin50Units");
    std::unique_ptr<ConditionNode> collisionDetected = std::make_unique<ConditionNode>("CollisionDetected");

    std::unique_ptr<ActionNode> pathfindToCenterAction = std::make_unique<ActionNode>("PathfindToCenter");
    std::unique_ptr<ActionNode> pursueAction = std::make_unique<ActionNode>("Pursue");
    std::unique_ptr<ActionNode> danceAction = std::make_unique<ActionNode>("Dance");

    std::unique_ptr<SequenceNode> sequence4 = std::make_unique<SequenceNode>();
    sequence4->addChild(std::make_unique<NotDecoratorNode>(std::move(characterNotWithin50Units)));
    sequence4->addChild(std::move(pathfindToCenterAction));

    std::unique_ptr<SequenceNode> sequence5 = std::make_unique<SequenceNode>();
    sequence5->addChild(std::make_unique<ConditionNode>("CharacterWithin50Units"));
    sequence5->addChild(std::move(pursueAction));

    std::unique_ptr<SequenceNode> sequence6 = std::make_unique<SequenceNode>();
    sequence6->addChild(std::make_unique<ConditionNode>("CollisionDetected"));
    sequence6->addChild(std::move(danceAction));

    std::unique_ptr<SequenceNode> rootMonster = std::make_unique<SequenceNode>();
    rootMonster->addChild(std::move(sequence4));
    rootMonster->addChild(std::move(sequence5));
    rootMonster->addChild(std::move(sequence6));

    return rootMonster.release();
}
