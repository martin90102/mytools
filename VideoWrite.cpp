#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <utime.h>
#include <string>
#include <chrono>

using namespace cv;
using namespace std;

int main()
{
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Failed to open camera." << endl;
        return -1;
    }

    namedWindow("Camera", WINDOW_NORMAL);

    Mat frame;
    // generate timestamp of the video，unix style timestamp 
    auto start_time = chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count();
    string filename = to_string(start_time) + ".avi";
    VideoWriter writer(filename, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)));

    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            cout << "Failed to capture frame." << endl;
            break;
        }
        auto time_now = chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now().time_since_epoch()).count();
        string time_str = to_string(time_now);
        putText(frame, time_str, Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        imshow("Camera", frame);

        writer.write(frame);

        if (waitKey(1) == 27)
        {
            break;
        }
    }

    cap.release();
    destroyAllWindows();
    cout<<"Video saved as "<<filename<<".\n";
    
    return 0;
}
