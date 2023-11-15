import sys
import os

# check if the number of arguments is correct
if len(sys.argv) != 3:
    print("Usage: python grdconvert.py <path file> <time stamp>")
    exit()

path_file = sys.argv[1]
time_stamp = sys.argv[2]

# check if the file exists
if not os.path.isfile(path_file):
    print("File not found")
    exit()

# read the time_stamp file
with open(time_stamp, 'r') as f:
    lines = f.readlines()

# create a dictionary to map image_number to time_stamp
time_stamp_dict = {}
for line in lines:
    #跳过第一行
    if line.startswith('#timestamp'):
        continue
    time_stamp, image_number = line.strip().split(',')
    image_number = image_number.replace('frame', '').replace('.png', '')
    #将有效数字前的0去掉
    image_number = str(int(image_number))
    image_number+=".0"
    time_stamp_dict[image_number] = time_stamp

# read the path_file
with open(path_file, 'r') as f:
    path_lines = f.readlines()

# write the new path_file
with open(path_file, 'w') as f:
    for line in path_lines:
        # get the image number
        image_number, *rest = line.strip().split(' ')
        # find the corresponding time_stamp
        if image_number in time_stamp_dict:
            # replace the image number with the time stamp
            new_line = ' '.join([time_stamp_dict[image_number]] + rest)
            f.write(new_line + '\n')
        else:
            print(f"Image number {image_number} not found in time_stamp file.")