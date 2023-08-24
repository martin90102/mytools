import os
import sys

def txt_csv_convertor(file_path):
    # Check if file exists
    if not os.path.exists(file_path):
        print("File does not exist")
        return
    
    # Check file extension
    file_ext = os.path.splitext(file_path)[1]
    if file_ext == '.txt':
        # Convert txt to csv
        with open(file_path, 'r') as txt_file:
            csv_data = []
            for line in txt_file:
                # Split line by space or comma
                line_data = line.strip().split(' ')
                if len(line_data) == 1:
                    line_data = line.strip().split(',')
                # Append line data to csv data
                csv_data.append(line_data)
        # Write csv data to file
        csv_file_path = os.path.splitext(file_path)[0] + '.csv'
        with open(csv_file_path, 'w') as csv_file:
            for line_data in csv_data:
                csv_file.write(','.join(line_data) + '\n')
    elif file_ext == '.csv':
        # Convert csv to txt
        with open(file_path, 'r') as csv_file:
            txt_data = []
            for line in csv_file:
                # Split line by comma
                line_data = line.strip().split(',')
                # Append line data to txt data
                txt_data.append(line_data)
        # Write txt data to file
        txt_file_path = os.path.splitext(file_path)[0] + '.txt'
        with open(txt_file_path, 'w') as txt_file:
            for line_data in txt_data:
                txt_file.write(' '.join(line_data) + '\n')
    else:
        # Invalid file format
        print("Invalid file format")

def main():
    # Check if file path is provided
    if len(sys.argv) < 2:
        print("File path not provided")
        return
    
    # Get file path
    file_path = sys.argv[1]
    # Convert file
    txt_csv_convertor(file_path)

if __name__ == '__main__':
    main()
    
