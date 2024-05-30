import pandas as pd
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score

# Read the CSV files without header
actual_data = pd.read_csv("processed_data.csv", header=None)
predicted_data = pd.read_csv("processed_predicted_data.csv", header=None)

# Provide column names manually
column_names = ['Distance feature', 'Character Action', 'IsDanceComplete feature', 'obstacleProximity feature', 'Monster Action']

# Assign column names to DataFrames
actual_data.columns = column_names
predicted_data.columns = column_names

# Extract actual and predicted outputs
actual_outputs = actual_data['Monster Action'].values
predicted_outputs = predicted_data['Monster Action'].values

# Calculate accuracy
accuracy = accuracy_score(actual_outputs, predicted_outputs)

# Calculate precision
precision = precision_score(actual_outputs, predicted_outputs, average='weighted')

# Calculate recall
recall = recall_score(actual_outputs, predicted_outputs, average='weighted')

# Calculate F1-score
f1 = f1_score(actual_outputs, predicted_outputs, average='weighted')

print("Accuracy:", accuracy)
print("Precision:", precision)
print("Recall:", recall)
print("F1-score:", f1)

# Accuracy: 0.4106793639996599
# Precision: 0.38924742809938556
# Recall: 0.4106793639996599
# F1-score: 0.2836202886209185