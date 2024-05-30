import random

# Open the original edge list file for reading
with open("graph.txt", "r") as original_file:
    # Open a new file to write the modified edge list with weights
    with open("graph_with_weights.txt", "w") as new_file:
        # Iterate over each line in the original file
        for line in original_file:
            # Split the line into source and target nodes
            source, target = map(int, line.strip().split())
            # Generate a random whole number between 1 and 20
            weight = random.randint(1, 20)
            # Write the line with the weight included
            new_file.write(f"{source} {target} {weight}\n")
