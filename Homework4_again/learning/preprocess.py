def convert_distance_to_binary(distance):
    return 1 if distance <= 100 else 0

def read_data_from_csv(input_filename, output_filename):
    with open(input_filename, 'r') as input_file, open(output_filename, 'w') as output_file:
        for line in input_file:
            values = line.strip().split(',')
            distance_to_character = float(values[0])
            # Convert distanceToCharacter to binary attribute
            binary_distance = convert_distance_to_binary(distance_to_character)
            # Replace the original distanceToCharacter value with the binary one
            values[0] = str(binary_distance)
            # Write the modified line to the output file
            output_file.write(','.join(values) + '\n')

# Input and output filenames
input_filename = 'modified_data.csv'
output_filename = 'modified_data_2.csv'

# Call the function to read data from the CSV file, modify it, and write it to a new file
read_data_from_csv(input_filename, output_filename)
