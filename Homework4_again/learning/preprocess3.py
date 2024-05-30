import csv

def read_data_from_csv(input_filename, output_filename):
    with open(input_filename, 'r') as infile, open(output_filename, 'w') as outfile:
        writer = csv.writer(outfile)
        for line in infile:
            values = line.strip().split(',')
            distance_to_character = float(values[0])
            # Convert distanceToCharacter to categorical values
            if distance_to_character > 50:
                distance_to_character = 0
            elif distance_to_character <= 50 and distance_to_character > 10:
                distance_to_character = 1
            else:
                distance_to_character = 2
            
            character_action = int(values[1])
            monster_action = int(values[2])
            is_dance_complete = int(values[3])
            obstacle_proximity = int(values[5])
            
            # Write modified data to output file
            writer.writerow([distance_to_character, character_action, monster_action, is_dance_complete, obstacle_proximity])

# Read data from CSV file and write modified data to new CSV file
read_data_from_csv('data.csv', 'modified_data_3.csv')
