from datetime import datetime
import csv
import sys

# 读取txt文件路径
txt_file = sys.argv[1]

# 指定csv文件路径
csv_file = './tttt.csv'

# 打开txt文件并读取数据
with open(txt_file, 'r') as f:
    data = f.readlines()

# 将每一行数据按空格分割并存储为列表
data = [line.strip().split() for line in data]

# 将前7列时间数据转换到unix时间戳格式，精确到纳秒
for i in range(len(data)):
    # 将前7列数据拼接成时间字符串
    time_copy  = data[i][:7]
    # 将时间字符串转换为datetime对象
    year = int(time_copy[0])
    month = int(time_copy[1])
    day = int(time_copy[2])
    hour = int(time_copy[3])
    minute = int(time_copy[4])
    second = int(time_copy[5])
    microsecond = int(time_copy[6])*1000

    time_obj = datetime(year, month, day, hour, minute, second, microsecond)
    # 将datetime对象转换为unix时间戳格式，精确到纳秒
    timestamp = int(time_obj.timestamp() * 1e9)
    # 将转换后的时间戳替换原来的时间数据
    data[i][:7] = [timestamp]

# 将数据写入csv文件
with open(csv_file, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerows(data)


