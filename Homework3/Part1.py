import networkx as nx
import matplotlib.pyplot as plt

# Generate a random graph with 100,000 nodes and around 200,000 edges
n = 100000
m = 200000
G = nx.gnm_random_graph(n, m)

# Plot the graph
nx.draw(G, node_size=10)
plt.show()
