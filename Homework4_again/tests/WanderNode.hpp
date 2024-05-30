#include "BehaviorTreeNode.hpp"


class WanderNode : Node
{
public:
    NodeState evaluate() override
    {
        action = WANDER;
        return RUNNING;
    }
};