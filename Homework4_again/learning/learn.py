import numpy as np
from graphviz import Digraph

class Node:
    def __init__(self, data):
        self.data = data
        self.children = {}
        self.attribute = None
        self.prediction = None

def entropy(class_labels):
    _, counts = np.unique(class_labels, return_counts=True)
    probabilities = counts / len(class_labels)
    return -np.sum(probabilities * np.log2(probabilities + 1e-10))

def information_gain(data, labels, attribute):
    total_entropy = entropy(labels)
    attribute_values, counts = np.unique(data[:, attribute], return_counts=True)
    weighted_entropy = np.sum([(counts[i] / np.sum(counts)) * entropy(labels[data[:, attribute] == attribute_value]) 
                               for i, attribute_value in enumerate(attribute_values)])
    return total_entropy - weighted_entropy

def build_tree(data, labels, attributes):
    if len(np.unique(labels)) == 1:
        node = Node(data)
        node.prediction = int(labels[0])
        return node
    if len(attributes) == 0:
        node = Node(data)
        node.prediction = int(np.argmax(np.bincount(labels.astype(int))))
        return node
    best_attribute = max(attributes, key=lambda attr: information_gain(data, labels, attr))
    remaining_attributes = [attr for attr in attributes if attr != best_attribute]
    node = Node(data)
    node.attribute = best_attribute
    attribute_values = np.unique(data[:, best_attribute])
    for value in attribute_values:
        subset_indices = np.where(data[:, best_attribute] == value)[0]
        subset_data = data[subset_indices]
        subset_labels = labels[subset_indices]
        if len(subset_data) == 0:
            child = Node(subset_data)
            child.prediction = int(np.argmax(np.bincount(labels)))
        else:
            child = build_tree(subset_data, subset_labels, remaining_attributes)
        node.children[value] = child
    return node

def predict(node, instance): #authored with chatgpt
    if node.prediction is not None:
        return node.prediction
    attribute_value = instance[node.attribute]
    if attribute_value not in node.children:
        return int(np.argmax(np.bincount(instance[:, -1].astype(int))))
    return predict(node.children[attribute_value], instance)

def visualize_tree(node, dot=None):
    if dot is None:
        dot = Digraph()
    if node.prediction is not None:
        dot.node(str(id(node)), label=f"Class: {node.prediction}")
    else:
        dot.node(str(id(node)), label=f"Attribute: {node.attribute}")
        for value, child in node.children.items():
            child_label = f"{value}"
            dot.edge(str(id(node)), str(id(child)), label=child_label)
            visualize_tree(child, dot)
    return dot

def tree_to_text(node, indent=0): #authored with chatgpt
    text = ""
    if node.prediction is not None:
        text += "  " * indent + f"Class: {node.prediction}\n"
    else:
        text += "  " * indent + f"Attribute: {node.attribute}\n"
        for value, child in node.children.items():
            text += "  " * indent + f"Value {value}:\n"
            text += tree_to_text(child, indent + 1)
    return text

# Read data from CSV
data = np.genfromtxt('processed_data.csv', delimiter=',')
X = data[:, :-1]
y = data[:, -1]

# Define attributes
attributes = list(range(X.shape[1]))

# Build decision tree
tree = build_tree(X, y, attributes)

# Visualize tree
dot = visualize_tree(tree)
print(dot)
dot.render('decision_tree', format='png', cleanup=True)

with open("decision_tree.txt", "w") as text_file:
    text_file.write(tree_to_text(tree))
