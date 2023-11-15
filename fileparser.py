import sys
import csv

file_path = sys.argv[1]

def parse_log(file_path):
    with open(file_path, 'r') as file:
        reader = csv.reader(file, delimiter=',')
        with open('DVL.csv', 'w', newline='') as dvl_file, open('IMU.csv', 'w', newline='') as imu_file, open('DG.csv', 'w', newline='') as dg_file, open('GPS.txt', 'w') as gps_file:
            dvl_writer = csv.writer(dvl_file)
            imu_writer = csv.writer(imu_file)
            dg_writer = csv.writer(dg_file)
            for row in reader:
                timestamp = float(row[0])
                # DVL data
                m_dfDVL_wVel_forward, m_dfDVL_wVel_right, m_dfDVL_wVel_down = float(row[33]), float(row[34]), float(row[35])
                dvl_writer.writerow([timestamp, m_dfDVL_wVel_forward, m_dfDVL_wVel_right, m_dfDVL_wVel_down])
                # IMU data
                m_dfINS_Gyro_forward, m_dfINS_Gyro_right, m_dfINS_Gyro_down, m_dfINS_Acce_forward, m_dfINS_Acce_right, m_dfINS_Acce_down = float(row[26]), float(row[27]), float(row[28]), float(row[23]), float(row[24]), float(row[25])
                imu_writer.writerow([timestamp, m_dfINS_Gyro_forward, m_dfINS_Gyro_right, m_dfINS_Gyro_down, m_dfINS_Acce_forward, m_dfINS_Acce_right, m_dfINS_Acce_down])
                # DG data
                m_dfDepth = float(row[29])
                dg_writer.writerow([timestamp, m_dfDepth])
                # GPS data
                m_dfGPS_lon, m_dfGPS_lat = float(row[14]), float(row[15])
                gps_file.write(f"{timestamp} {m_dfGPS_lon} {m_dfGPS_lat}\n")

parse_log(file_path)
