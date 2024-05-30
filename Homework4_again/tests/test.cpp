// // test.cpp

// #include "BehaviorTreeNode.hpp"
// // SteeringData character, monster, goal, monstergoal;

// CharacterBehaviorTree cbt;
// MonsterBehaviorTree mbt;

// int main()
// {
//     character.position.x = 0.f;
//     character.position.y = 0.f;
//     monster.position.x = 9.f;
//     monster.position.y = 9.f;
//     Node* croot = cbt.constructBehaviorTree();
//     Node* mroot = mbt.constructBehaviorTree();

//     // root->evaluate();
//     croot->Evaluate();
//     mroot->Evaluate();

//     return 0;
// }

#include <iostream>
#include <unordered_map>
#include <vector>

struct TreeNode {
    int attribute;
    std::unordered_map<int, TreeNode*> branches;
    int classification;
};

TreeNode* createNode() {
    TreeNode* newNode = new TreeNode;
    newNode->attribute = -1; // Default value
    newNode->classification = -1; // Default value
    return newNode;
}

// Function to build the decision tree based on the graph representation
TreeNode* buildDecisionTree() {
    // Root node
    TreeNode* root = createNode();
    root->attribute = 0;

    // Node 1
    TreeNode* node1 = createNode();
    node1->attribute = 3;
    root->branches[0] = node1;

    // Node 2
    TreeNode* node2 = createNode();
    node2->attribute = 2;
    node1->branches[0] = node2;

    // Node 3
    TreeNode* node3 = createNode();
    node3->attribute = 1;
    node2->branches[0] = node3;
    node3->classification = 1;

    // Node 4
    TreeNode* node4 = createNode();
    node4->attribute = 1;
    node2->branches[1] = node4;

    // Node 5
    TreeNode* node5 = createNode();
    node5->classification = 1;
    node4->branches[0] = node5;

    // Node 6
    TreeNode* node6 = createNode();
    node6->classification = 4;
    node4->branches[3] = node6;

    // Node 7
    TreeNode* node7 = createNode();
    node7->attribute = 2;
    node1->branches[1] = node7;

    // Node 8
    TreeNode* node8 = createNode();
    node8->attribute = 1;
    node7->branches[0] = node8;

    // Node 9
    TreeNode* node9 = createNode();
    node9->classification = 1;
    node8->branches[0] = node9;

    // Node 10
    TreeNode* node10 = createNode();
    node10->classification = 1;
    node8->branches[1] = node10;

    // Node 11
    TreeNode* node11 = createNode();
    node11->attribute = 1;
    node7->branches[1] = node11;

    // Node 12
    TreeNode* node12 = createNode();
    node12->classification = 1;
    node11->branches[0] = node12;

    // Node 13
    TreeNode* node13 = createNode();
    node13->attribute = 1;
    node11->branches[1] = node13;

    // Node 14
    TreeNode* node14 = createNode();
    node14->classification = 1;
    node13->branches[0] = node14;

    // Node 15
    TreeNode* node15 = createNode();
    node15->attribute = 1;
    root->branches[1] = node15;

    // Node 16
    TreeNode* node16 = createNode();
    node16->classification = 0;
    node15->branches[0] = node16;

    // Node 17
    TreeNode* node17 = createNode();
    node17->classification = 4;
    node15->branches[2] = node17;

    // Node 18
    TreeNode* node18 = createNode();
    node18->attribute = 2;
    root->branches[2] = node18;

    // Node 19
    TreeNode* node19 = createNode();
    node19->classification = 5;
    node18->branches[0] = node19;

    // Node 20
    TreeNode* node20 = createNode();
    node20->classification = 4;
    node18->branches[1] = node20;

    // Node 21
    TreeNode* node21 = createNode();
    node21->attribute = 2;
    root->branches[2] = node21;

    // Node 22
    TreeNode* node22 = createNode();
    node22->classification = 5;
    node21->branches[0] = node22;

    // Node 23
    TreeNode* node23 = createNode();
    node23->classification = 4;
    node21->branches[1] = node23;

    // Node 24
    TreeNode* node24 = createNode();
    node24->attribute = 2;
    node15->branches[3] = node24;

    TreeNode* node25 = createNode();
    node25->classification=5;
    node24->branches[0] = node25;

    TreeNode* node26 = createNode();
    node26->classification = 4;
    node24->branches[1] = node26;

    return root;
}

// Function to classify an item using the decision tree
int classifyItem(TreeNode* tree, std::vector<int> item) {
    if (tree->classification != -1)
        return tree->classification;

    int attributeValue = item[tree->attribute];
    if (tree->branches.find(attributeValue) != tree->branches.end()) {
        TreeNode* nextNode = tree->branches[attributeValue];
        return classifyItem(nextNode, item);
    } else {
        std::cerr << "Error: Branch not found for attribute value " << attributeValue << std::endl;
        return -1; // or return a default classification
    }
}

int main() {
    // Build the decision tree
    TreeNode* decisionTree = buildDecisionTree();

    // Example item
    std::vector<int> newItem = {2, 3, 0, 0}; // Example attribute values

    // Classify the item using the decision tree
    int classification = classifyItem(decisionTree, newItem);
    if (classification != -1)
        std::cout << "Classification: " << classification << std::endl;

    // Cleanup: Implement logic to delete the decision tree nodes to avoid memory leaks

    return 0;
}
