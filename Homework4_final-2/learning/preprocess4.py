import csv

def convert_distance(distance):
    if distance > 50:
        return 0
    elif 10 <= distance <= 50:
        return 1
    else:
        return 2

def main():
    with open('data.csv', 'r') as file:
        reader = csv.reader(file)
        data = list(reader)
        
    processed_data = []
    for row in data:
        distance_to_character = float(row[0])
        character_action = int(row[1])
        monster_action = int(row[2])
        is_dance_complete = int(row[3])
        distance_to_center = float(row[4])
        obstacle_proximity = int(row[5])
        
        distance_to_character = convert_distance(distance_to_character)
        
        processed_row = [distance_to_character, character_action, is_dance_complete, obstacle_proximity, monster_action]
        processed_data.append(processed_row)
        
    with open('processed_data.csv', 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerows(processed_data)

if __name__ == "__main__":
    main()
