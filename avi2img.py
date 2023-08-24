import cv2
import os
import time
import datetime

frequency = 22 # given frequency
timestamps = []

# Get the current working directory
current_dir = os.getcwd()

# Loop through all files in the current directory
for file in os.listdir(current_dir):
    # Check if the file is an AVI video file and starts with a timestamp
    if file.endswith(".avi") and file[0:14].isdigit():
        # Extract the timestamp from the file name
        timestamp = datetime.datetime(int(file[0:4]),int(file[4:6]),int(file[6:8]),int(file[8:10]), int(file[10:12]),int(file[12:14]),int(file[14:16])) 

        # Create a directory with the same name as the video file (without extension)
        directory_name = os.path.splitext(file)[0]
        os.makedirs(directory_name, exist_ok=True)

        # Open the video file
        video = cv2.VideoCapture(file)

        # Loop through all frames in the video
        frame_count = 0
        while True:
            # Read the next frame
            ret, frame = video.read()

            # If there are no more frames, break out of the loop
            if not ret:
                break

            # Construct the file name for the current frame
            temp = timestamp + datetime.timedelta(milliseconds=frame_count*1000/frequency)
            timestamps.append(temp)
            image_name = os.path.join(directory_name, f"{str(int(temp.timestamp()*1e9))}.jpg")
            print(str(int(temp.timestamp()*1e9)))

            # Save the current frame as an image file
            cv2.imwrite(image_name, frame)

            # Increment the frame count
            frame_count += 1

        # Release the video file
        video.release()

with open('timestamps.txt', 'w') as f:
    for timestamp in timestamps:
        f.write(str(int(timestamp.timestamp()*1e9)) + '\n')
