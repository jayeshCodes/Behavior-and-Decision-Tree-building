def read_data_from_csv(input_filename, output_filename):
    with open(input_filename, 'r') as input_file, open(output_filename, 'w') as output_file:
        for line in input_file:
            values = line.strip().split(',')
            # Remove the distanceToCenter column (index 4)
            values.pop(4)
            # Convert distanceToCharacter to binary attribute
            distance_to_character = float(values[0])
            binary_distance = 1 if distance_to_character <= 100 else 0
            values[0] = str(binary_distance)
            # Write the modified line to the output file
            output_file.write(','.join(values) + '\n')

# Input and output filenames
input_filename = 'modified_data.csv'
output_filename = 'modified_data_2.csv'

# Call the function to read data from the CSV file, modify it, and write it to a new file
read_data_from_csv(input_filename, output_filename)
