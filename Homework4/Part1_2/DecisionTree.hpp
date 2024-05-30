// DecisionTree.hpp
#pragma once
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

struct TreeNode
{
    int featureIndex; // Index of the feature to split on
    double threshold; // Threshold for the split
    std::unique_ptr<TreeNode> left;   // Pointer to the left child node
    std::unique_ptr<TreeNode> right;  // Pointer to the right child node
    int label;        // Label or action associated with this node

    // Constructor to initialize the node
    TreeNode(int featureIndex, double threshold, std::unique_ptr<TreeNode> left, std::unique_ptr<TreeNode> right, int label = -1)
        : featureIndex(featureIndex), threshold(threshold), left(std::move(left)), right(std::move(right)), label(label) {}
};

enum ActionType
{
    WANDER,
    PATHFIND, // to center of screen
    DODGE,
    ATTACK,
    DANCE
};

class DecisionTreePart1
{
public:
    std::unique_ptr<TreeNode> constructDecisionTree()
    {
        auto root = std::make_unique<TreeNode>(0, 0, nullptr, nullptr, -1);
        root->left = std::make_unique<TreeNode>(1, 1, nullptr, nullptr, WANDER);
        root->right = std::make_unique<TreeNode>(1, 1, nullptr, nullptr, PATHFIND);

        return root;
    }

    int takeAction(std::vector<int> &features, SteeringData &character, SteeringData &goal)
    {
        auto action = makeDecision(features, constructDecisionTree());
        return action;
    }

     void visualizeTree(std::unique_ptr<TreeNode>& root, int depth = 0)
    {
        if (!root)
            return;

        // Print indentation based on the depth of the node
        std::string indentation(depth * 4, ' ');

        // Print information about the current node
        std::cout << indentation << "Feature Index: " << root->featureIndex
                  << ", Threshold: " << root->threshold
                  << ", Label: ";
        
        if (root->label == -1)
            std::cout << "Split on feature " << root->featureIndex << std::endl;
        else
            std::cout << root->label << std::endl;

        // Recursively visualize the left and right subtrees
        visualizeTree(root->left, depth + 1);
        visualizeTree(root->right, depth + 1);
    }


private:
    int makeDecision(std::vector<int> &features, std::unique_ptr<TreeNode> root)
    {
        auto currentNode = root.get();
        while (currentNode->label == -1)
        {
            if (features[currentNode->featureIndex] <= currentNode->threshold)
                currentNode = currentNode->left.get();
            else
                currentNode = currentNode->right.get();
        }
        return currentNode->label;
    }
};
