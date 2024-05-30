#include <iostream>
#include <unordered_map>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include "ActionType.hpp"
#include "SteeringData.hpp"

struct TreeNode
{
    int attribute;
    std::unordered_map<int, TreeNode *> branches;
    int classification;
};

TreeNode *createNode()
{
    TreeNode *newNode = new TreeNode;
    newNode->attribute = -1;      // Default value
    newNode->classification = -1; // Default value
    return newNode;
}

// Function to build the decision tree based on the graph representation
TreeNode *buildDecisionTree()
{
    TreeNode *root = createNode();
    root->attribute = 0;

    TreeNode *ProximityToObstacle = createNode();
    ProximityToObstacle->attribute = 1;

    TreeNode *Pathfind_to_center_1 = createNode();
    Pathfind_to_center_1->classification = 1;

    root->branches[1] = Pathfind_to_center_1;
    root->branches[0] = ProximityToObstacle;

    TreeNode *Pathfind_away = createNode();
    Pathfind_away->classification = 2;

    TreeNode *Check_if_centered = createNode();
    Check_if_centered->attribute = 2;

    ProximityToObstacle->branches[1] = Pathfind_away;
    ProximityToObstacle->branches[0] = Check_if_centered;

    TreeNode *Pathfind_to_center_2 = createNode();
    Pathfind_to_center_2->classification = 1;

    TreeNode *Wander = createNode();
    Wander->classification = 0;

    Check_if_centered->branches[1] = Wander;
    Check_if_centered->branches[0] = Pathfind_to_center_2;

    return root;
}

// Function to classify an item using the decision tree
int classifyItem(TreeNode *tree, std::vector<int> item)
{
    if (tree->classification != -1)
        return tree->classification;

    int attributeValue = item[tree->attribute];
    if (tree->branches.find(attributeValue) != tree->branches.end())
    {
        TreeNode *nextNode = tree->branches[attributeValue];
        return classifyItem(nextNode, item);
    }
    else
    {
        std::cerr << "Error: Branch not found for attribute value " << attributeValue << std::endl;
        return -1; // or return a default classification
    }
}
