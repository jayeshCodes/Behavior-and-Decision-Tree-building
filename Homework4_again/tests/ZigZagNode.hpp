#include "BehaviorTreeNode.hpp"


#include <iostream>

SeekBehavior seek;

class ZigZagNode : Node
{
public:
    NodeState evaluate() override
    {
        float distance = calculateDistanceBetween(character.position, monster.position);
        if (distance <= 10 && distance > 5)
        {
            action = ZIGZAG;
            return RUNNING;
        }
        else if (distance <= 5)
            return FAILURE;
        else if (distance > 10)
            return SUCCESS;
    }

private:
    float calculateDistanceBetween(const sf::Vector2f &p1, const sf::Vector2f &p2)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};
