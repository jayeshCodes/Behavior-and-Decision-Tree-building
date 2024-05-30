import random

def generate_unique_coordinates(num_nodes):
    # Generate all possible coordinates within the range
    all_coordinates = [(x, y) for x in range(1000) for y in range(1000)]
    # Shuffle the list of coordinates randomly
    random.shuffle(all_coordinates)
    # Yield unique coordinates for the required number of nodes
    for i in range(num_nodes):
        yield all_coordinates[i]

def add_random_unique_coordinates(input_file, output_file):
    num_nodes = 0
    with open(input_file, 'r') as f_in:
        # Count the number of nodes in the input file
        for _ in f_in:
            num_nodes += 1
    
    print(num_nodes)

    # Ensure that there are enough unique coordinates for each node
    if num_nodes * 2 > 1000 * 1000:
        print("Error: Number of nodes exceeds available unique coordinates.")
        return

    # Generate unique random coordinates for all nodes
    unique_coordinates = generate_unique_coordinates(num_nodes * 2)

    with open(input_file, 'r') as f_in, open(output_file, 'w') as f_out:
        for line in f_in:
            fields = line.strip().split()
            node1 = fields[0]
            node2 = fields[1]
            distance = fields[2]
            # Get unique random coordinates for each node
            coord1_x, coord1_y = next(unique_coordinates)
            coord2_x, coord2_y = next(unique_coordinates)
            f_out.write(f"{node1} {coord1_x} {coord1_y} {node2} {coord2_x} {coord2_y} {distance}\n")


# Example usage:
input_file = "large_graph.txt"  # Replace with your input file name
output_file = "large_graph_with_coordinates.txt"  # Replace with desired output file name

add_random_unique_coordinates(input_file, output_file)
