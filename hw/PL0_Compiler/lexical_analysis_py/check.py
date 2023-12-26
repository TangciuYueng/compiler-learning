import pickle
import csv

with open('lexical.pkl', 'rb') as file:
    tokens = pickle.load(file)
    state_trans_table = pickle.load(file)
    end_list = pickle.load(file)
    end_map = pickle.load(file)


csv_file_path = '../DFA.csv'

with open(csv_file_path, 'r') as csvfile:
    reader = csv.reader(csvfile)

    print(f'header {header}')

    rows = []

    for row in reader:
        processed_row = []

        for item in row:
            if item.isdigit():
                processed_row.append(int(item))
            elif item == '':
                processed_row.append(-2)
            else:
                processed_row.append(item)

        rows.append(processed_row)

print(len(rows))
print(len(state_trans_table))
    
# 假设 state_trans_table 和 rows 的维度相同
for i in range(len(state_trans_table)):
    for j in range(len(state_trans_table[i])):
        if state_trans_table[i][j] != rows[i][j]:
            print(f'Difference found at position ({i}, {j}):')
            print(f'state_trans_table[{i}][{j}] = {state_trans_table[i][j]}')
            print(f'rows[{i}][{j}] = {rows[i][j]}')

print("compare finished")
