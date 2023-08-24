import os
import sys

# Set the directory path
directory = sys.argv[1]
# Open a new text file to write the file names to
with open('file_names.txt', 'w') as file:
    # Loop through all files in the directory
    for filename in os.listdir(directory):
        # Write the file name to the text file
        filename_without_extension = os.path.splitext(filename)[0]
        # Write the modified file name to the text file
        file.write(filename_without_extension + '\n')