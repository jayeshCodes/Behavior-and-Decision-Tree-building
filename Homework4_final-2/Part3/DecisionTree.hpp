#include <iostream>
#include <unordered_map>
#include <vector>
#include "BehaviorTreeNode.hpp"

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
    // Root node
    TreeNode *root = createNode();
    root->attribute = 0;

    TreeNode *Node1 = createNode();
    TreeNode *Node2 = createNode();
    TreeNode *Node3 = createNode();

    Node1->attribute = 3;
    Node2->attribute = 1;
    Node3->attribute = 2;

    root->branches[0] = Node1;
    root->branches[1] = Node2;
    root->branches[2] = Node3;

    TreeNode *Node4 = createNode();
    TreeNode *Node5 = createNode();
    TreeNode *Node6 = createNode();
    TreeNode *Node7 = createNode();
    TreeNode *Node8 = createNode();
    TreeNode *Node9 = createNode();
    TreeNode *Node10 = createNode();

    Node4->attribute = 2;
    Node5->attribute = 2;
    Node6->classification = 0;
    Node7->classification = 4;
    Node8->attribute = 2;
    Node9->classification = 5;
    Node10->classification = 4;

    Node1->branches[0] = Node4;
    Node1->branches[1] = Node5;

    Node2->branches[0] = Node6;
    Node2->branches[2] = Node7;
    Node2->branches[3] = Node8;

    Node3->branches[0] = Node9;
    Node3->branches[1] = Node10;

    TreeNode *Node11 = createNode();
    TreeNode *Node12 = createNode();
    TreeNode *Node13 = createNode();
    TreeNode *Node14 = createNode();
    TreeNode *Node15 = createNode();
    TreeNode *Node16 = createNode();

    Node11->attribute = 1;
    Node12->attribute = 1;
    Node13->attribute = 1;
    Node14->attribute = 1;
    Node15->classification = 5;
    Node16->classification = 4;

    Node4->branches[0] = Node11;
    Node4->branches[1] = Node12;

    Node5->branches[0] = Node13;
    Node5->branches[1] = Node14;

    Node8->branches[0] = Node15;
    Node8->branches[1] = Node16;

    TreeNode *Node17 = createNode();
    TreeNode *Node18 = createNode();
    TreeNode *Node19 = createNode();
    TreeNode *Node20 = createNode();
    TreeNode *Node21 = createNode();

    Node17->classification = 1;
    Node18->classification = 1;
    Node19->classification = 4;
    Node20->classification = 1;
    Node21->classification = 1;

    Node11->branches[0] = Node17;
    Node12->branches[0] = Node18;
    Node12->branches[3] = Node19;

    Node13->branches[0] = Node20;
    Node14->branches[0] = Node21;

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
