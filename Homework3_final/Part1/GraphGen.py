import networkx as nx

# Generate a random graph with NetworkX
large_graph = nx.gnm_random_graph(100000, 500000)

# Export the graph to a text file
nx.write_edgelist(large_graph, "graph2.txt", data=False)
