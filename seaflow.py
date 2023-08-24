# use to generate sea flow data
import sys
import numpy as np
import cv2 as cv

if len(sys.argv) != 7:
    print("Error input!\n")
    print("Usage: python3 seaflow.py grid_height grid_width grid_resolution flow_type velocity dir_velocity!\n")
    print("*Tips1: flow type == 1: one direction flow, flow type == 2: convection flow, flow type == 3:radial flow ,flow type == 4:circular flow!\n")
    print("*Tips2: grid_height, grid_width and grid_resolution are under the same unit of measurement(default is m)!\n")
    print("*Tips3: direction of velocity is 0~360 degree in XoY plane,neglect the velocity of z direction!\n")
    print("*Tips4: the unit of velocity is m/s!\n")
    exit(1)

grid_height = int(sys.argv[1])
grid_width = int(sys.argv[2])
grid_resolution = int(sys.argv[3])
flow_type = int(sys.argv[4])
velocity = float(sys.argv[5])
dir_velocity = float(sys.argv[6])

# check the grid
if grid_height < 0 or grid_width < 0 or grid_resolution < 0:
    print("Error input!\n")
    print("The grid_height, grid_width and grid_resolution must be positive!\n")
    exit(1)
# check the flow type
if flow_type != 1 and flow_type != 2 and flow_type != 3 and flow_type != 4:
    print("Error input!\n")
    print("The flow_type must be 1 or 2!\n")
    exit(1)

while dir_velocity > 360 or dir_velocity < 0:
    if dir_velocity > 360:
        dir_velocity %= 360
    if dir_velocity < 0:
        dir_velocity += 360

dir_velocity = dir_velocity / 180 * np.pi

# according the resolution to generate the grid
rows = int(grid_height / grid_resolution)
col = int(grid_width / grid_resolution)
grid = np.zeros((rows, col, 2))
for i in range(rows):
    for j in range(col):
        grid[i][j] = (0, 0)

# generate the flow
if flow_type == 1:
    velocity_x = velocity * np.cos(dir_velocity)
    velocity_y = velocity * np.sin(dir_velocity)
    for i in range(rows):
        for j in range(col):
            grid[i][j] = (velocity_x, velocity_y)

if flow_type == 2:
    # default: the center of the ellipse is the center of the grid
    # default: the rotation direction is clockwise
    # the direction of the velocity is the direction of the tangent line of the ellipse

    # the center of the ellipse
    center_x = grid_height / 2
    center_y = grid_width / 2

    # the radius of the ellipse
    a = grid_height / 2
    b = grid_width / 2

    # the rotation angle of the ellipse
    theta = dir_velocity

    # calculate the velocity of each point
    for i in range(rows):
        for j in range(col):
            # the distance between the point and the center of the ellipse
            r = np.sqrt((i*grid_resolution - center_x) ** 2 +
                        (j*grid_resolution - center_y) ** 2)
            # the angle between the point and the center of the ellipse
            alpha = np.arctan2((j*grid_resolution - center_y),
                               (i*grid_resolution - center_x))
            # the angle on the ellipse
            phi = np.arctan2((j*grid_resolution - center_y) / b,
                             (i*grid_resolution - center_x) / a)
            # the angle between the velocity and the tangent line of the ellipse
            beta = phi + np.pi / 2  # add 90 degrees
            # the velocity of the point
            velocity_x = velocity * np.cos(beta + theta)
            velocity_y = velocity * np.sin(beta + theta)
            grid[i][j] = (velocity_x, velocity_y)

# radial flow
if flow_type == 3:
    center_x = grid_height / 2
    center_y = grid_width / 2

    # calculate the velocity of each point
    for i in range(rows):
        for j in range(col):
            # the distance between the point and the center of the grid
            dx = (i*grid_resolution - center_x)
            dy = (center_y - j*grid_resolution)  # reverse the direction of y
            r = np.hypot(dx, dy)

            # the angle between the point and the center of the grid
            alpha = np.arctan2(dy, dx)

            # the velocity of the point
            velocity_x = velocity * -np.sin(alpha)
            velocity_y = velocity * np.cos(alpha)
            grid[i][j] = (velocity_x, velocity_y)

#circular flow
if flow_type == 4:
    center_x = grid_height / 2
    center_y = grid_width / 2

    # calculate the velocity of each point
    for i in range(rows):
        for j in range(col):
            # the distance between the point and the center of the grid
            dx = (i*grid_resolution - center_x)
            dy = (center_y - j*grid_resolution)  # reverse the direction of y
            r = np.hypot(dx, dy)

            # the angle between the point and the center of the grid
            alpha = np.arctan2(dy, dx)+np.pi/2

            # the velocity of the point
            velocity_x = velocity * -np.sin(alpha)
            velocity_y = velocity * np.cos(alpha)
            grid[i][j] = (velocity_x, velocity_y)

# output the grid
for i in range(rows):
    for j in range(col):
        print(grid[i][j], end="")
    print("\n")

# visualize the grid, use hsv color space
# use color to represent the direction of the velocity
grid_ = np.zeros((grid_height, grid_width, 3), dtype=np.uint8)
for i in range(0, rows):
    for j in range(0, col):
        start_point = (j*grid_resolution, i*grid_resolution)
        # map velocity magnitude to arrow length
        arrow_length = grid_resolution/2

        end_point = (int(start_point[0] + arrow_length * np.cos(np.arctan2(grid[i][j][1], grid[i][j][0]))),
                     int(start_point[1] + arrow_length * np.sin(np.arctan2(grid[i][j][1], grid[i][j][0]))))
        cv.arrowedLine(grid_, start_point, end_point, (255, 255, 255), 1)

# convert HSV to BGR for display
cv.imshow("grid", grid_)
cv.waitKey(0)

# save the grid
# 1. save the grid as img
cv.imwrite("grid.png", grid_)
# 2. save the grid as txt "grid_x grid_y velocity_x velocity_y" 
f = open("grid.txt", "w")
for i in range(rows):
    for j in range(col):
        f.write(str(i*grid_resolution)+" "+str(j*grid_resolution)+" "+str(grid[i][j][0])+" "+str(grid[i][j][1])+"\n")

