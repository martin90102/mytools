#include <pangolin/pangolin.h>
#include <Eigen/Core>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
using namespace std;
using namespace Eigen;

#define NAME(variable) #variable

void DrawTrajectory(vector<Isometry3d, Eigen::aligned_allocator<Isometry3d>> poses);
Vector3d RANSAC(vector<Vector3d> temp);
double distance(Vector3d &vec1, Vector3d &vec2);
void myplotter(vector<Vector3d> data, string name, Vector3d scalar = Vector3d(100, 100, 100));
signed int sign(double x)
{
    return x > 0 ? 1 : -1;
};

int main(int argc, char **argv)
{
    string file_path = argv[1];
    vector<Matrix3d> Trw;
    ifstream fin(file_path);
    if (!fin)
    {
        cout << "cannot find trajectory file at " << file_path << endl;
        return 1;
    }

    vector<Vector3d> pre_velocity;
    vector<Vector3d> pre_t;
    vector<Vector3d> pre_acc;
    vector<Vector3d> pre_gyro;
    vector<Vector3d> pre_rpy;
    vector<Quaterniond> pre_q;
    if (pre_velocity.empty())
    {
        pre_velocity.push_back(Vector3d::Zero());
    }
    if (pre_acc.empty())
    {
        pre_acc.push_back(Vector3d::Zero());
    }
    if (pre_t.empty())
    {
        pre_t.push_back(Vector3d::Zero());
    }
    if (pre_gyro.empty())
    {
        pre_gyro.push_back(Vector3d::Zero());
    }
    if (pre_rpy.empty())
    {
        pre_rpy.push_back(Vector3d::Zero());
    }
    if (pre_q.empty())
    {
        pre_q.push_back(Quaterniond::Identity());
    }
    if(Trw.empty())
    {
        Trw.push_back(Matrix3d::Identity());
    }
    bool first_line = true;
    double start_time = 0;
    // imu calibration(RANSAC)
    int count = 50;
    vector<Vector3d> calib_data;
    // vector<Vector3d> calib_gyro;
    while (count--)
    {
        double time, acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z,yaw,pitch,roll;
        fin >> time >> acc_x >> acc_y >> acc_z >> gyro_x >> gyro_y >> gyro_z>>yaw>>pitch>>roll;
        if (count == 99)
        {
            pre_acc.push_back(Vector3d(acc_x, acc_y, acc_z));
            pre_gyro.push_back(Vector3d(gyro_x, gyro_y, gyro_z));
        }
        calib_data.push_back(Vector3d(acc_x, acc_y, acc_z));
        // calib_gyro.push_back(Vector3d(gyro_x, gyro_y, gyro_z));
    }

    Vector3d gravity = RANSAC(calib_data);
    // Vector3d gyro_bias = RANSAC(calib_gyro);
    // printf("gravity: %lf %lf %lf\n", gravity.x(), gravity.y(), gravity.z());
    // 重新开始读取数据
    fin.close();
    fin.open(file_path);

    double dt = 0.1;
    bool first = true;
    // 读取第一行数据初始化状态
    while (!fin.eof() && first)
    {
        double time, acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z,yaw,pitch,roll;
        fin >> time >> acc_x >> acc_y >> acc_z >> gyro_x >> gyro_y >> gyro_z>>yaw>>pitch>>roll;
        Vector3d acc(acc_x, acc_y, acc_z);
        acc -= gravity;
        gyro_x = gyro_x / 180 * M_PI;
        gyro_y = gyro_y / 180 * M_PI;
        gyro_z = gyro_z / 180 * M_PI;
        pre_acc.push_back(acc);
        pre_gyro.push_back(Vector3d(gyro_x, gyro_y, gyro_z));
        first = false;
    }
    pre_acc.erase(pre_acc.begin());
    pre_gyro.erase(pre_gyro.begin());
    fin.close();
    fin.open(file_path);
    while (!fin.eof())
    {

        double time, acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z,yaw,pitch,roll;
        fin >> time >> acc_x >> acc_y >> acc_z >> gyro_x >> gyro_y >> gyro_z>>yaw>>pitch>>roll;
        Vector3d acc(acc_x, acc_y, acc_z);
        acc -= gravity;
        // acc -= Vector3d(0, 0, -9.7985);

        gyro_x = gyro_x / 180 * M_PI;
        gyro_y = gyro_y / 180 * M_PI;
        gyro_z = gyro_z / 180 * M_PI;
        Vector3d gyro;
        gyro << gyro_x, gyro_y, gyro_z;
        // rpy
        Vector3d rpy = pre_rpy.back() + dt * (gyro + pre_gyro.back()) / 2;
        // 旋转矩阵
        Matrix3d R_z;
        Matrix3d R_y;
        Matrix3d R_x;
        R_z << cos(rpy.z()), -sin(rpy.z()), 0,
            sin(rpy.z()), cos(rpy.z()), 0,
            0, 0, 1;
        R_y << cos(rpy.y()), 0, sin(rpy.y()),
            0, 1, 0,
            -sin(rpy.y()), 0, cos(rpy.y());
        R_x << 1, 0, 0,
            0, cos(rpy.x()), -sin(rpy.x()),
            0, sin(rpy.x()), cos(rpy.x());
        Matrix3d R = R_z * R_y * R_x;
        

        Vector3d velocity =pre_velocity.back() +R*0.5*dt*((pre_acc.back()+acc)) ; // v = v0 + a * dt(机器人坐标系下)
        printf("velocity: %lf %lf %lf\n", velocity.x(), velocity.y(), velocity.z());
        // velocity = R.inverse()*velocity;
        Vector3d t =  pre_t.back() + R*0.5*dt *(pre_velocity.back()+velocity);
        // (velocity + pre_velocity.back())  / 2; // t = t0 + v * dt(世界坐标系下)
        
        Trw.push_back(R);
        pre_t.push_back(t);
        pre_acc.push_back(acc);
        pre_rpy.push_back(rpy);
        // printf("rpy: %lf %lf %lf\n", rpy.x(), rpy.y(), rpy.z());
        pre_velocity.push_back(velocity);
        pre_gyro.push_back(gyro);
    }
    cout << "read total " << pre_acc.size() << " pose entries" << endl;
    // DrawTrajectory(poses);
    myplotter(pre_rpy, NAME(rpy), Vector3d(180 / M_PI, 180 / M_PI, 180 / M_PI)); // correct!
    myplotter(pre_velocity, NAME(velocity), Vector3d(10, 10, 30));
    // myplotter(pre_acc, NAME(acc));

    // 画出轨迹在xoy平面的投影
    cv::Mat xy_plot = cv::Mat::ones(1000, 1000, CV_8UC3);
    // 将画布设置为白色
    xy_plot = cv::Scalar::all(255);
    // 设置坐标轴和网格
    cv::line(xy_plot, cv::Point(0, 500), cv::Point(1000, 500), cv::Scalar(0, 0, 0));
    cv::line(xy_plot, cv::Point(500, 0), cv::Point(500, 1000), cv::Scalar(0, 0, 0));
    cv::putText(xy_plot, "x", cv::Point(950, 450), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0));
    cv::putText(xy_plot, "y", cv::Point(450, 950), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0));

    // 画出网格，每隔100个像素画一条线
    for (int i = 0; i < 1000; i += 100)
    {
        cv::line(xy_plot, cv::Point(i, 0), cv::Point(i, 1000), cv::Scalar(0, 0, 0));
        cv::line(xy_plot, cv::Point(0, i), cv::Point(1000, i), cv::Scalar(0, 0, 0));
    }
    for (int i = 0; i < pre_t.size(); i++)
    {

        cv::Point2d p(pre_t[i].y() / 10 + 500, pre_t[i].x() /10 + 500);
        cv::circle(xy_plot, p, 1, cv::Scalar(0, 0, 255));
    }
    cv::imshow("xy_plot", xy_plot);
    cv::waitKey(0);

    return 0;
}

void myplotter(vector<Vector3d> data, string name, Vector3d scale)
{
    // 根据输入变量名字，创建对应的画布
    cv::Mat data_plot = cv::Mat::ones(1000, 800, CV_8UC3);
    // 将画布设置为白色
    data_plot = cv::Scalar::all(255);

    // 设置坐标轴和网格
    cv::line(data_plot, cv::Point(0, 500), cv::Point(1000, 500), cv::Scalar(0, 0, 0));
    cv::line(data_plot, cv::Point(0, 0), cv::Point(0, 800), cv::Scalar(0, 0, 0));

    string x_name = name + "_x";
    cv::putText(data_plot, x_name, cv::Point(500, 200), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0));

    string y_name = name + "_y";
    cv::putText(data_plot, y_name, cv::Point(500, 500), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0));

    string z_name = name + "_z";
    cv::putText(data_plot, z_name, cv::Point(500, 800), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0));

    // 画出网格，每隔100个像素画一条线
    for (int i = 0; i < 1000; i += 100)
    {
        cv::line(data_plot, cv::Point(i, 0), cv::Point(i, 1000), cv::Scalar(0, 0, 0));
        cv::line(data_plot, cv::Point(0, i), cv::Point(1000, i), cv::Scalar(0, 0, 0));
    }

    // 画出数据
    for (int i = 0; i < data.size(); i++)
    {
        // 画出向上为正的坐标系
        cv::Point2d p0(i * 0.1, -data[i].x() * scale.x() + 200);
        cv::circle(data_plot, p0, 1, cv::Scalar(0, 0, 255));

        cv::Point2d p1(i * 0.1, -data[i].y() * scale.y() + 500);
        cv::circle(data_plot, p1, 1, cv::Scalar(0, 255, 0));

        cv::Point2d p2(i * 0.1, -data[i].z() * scale.z() + 800);
        cv::circle(data_plot, p2, 1, cv::Scalar(255, 0, 0));
    }
    // 根据输入data的名字，转换为string，命名窗口

    cv::imshow(name, data_plot);
}

void DrawTrajectory(vector<Isometry3d, Eigen::aligned_allocator<Isometry3d>> poses)
{
    // create pangolin window and plot the trajectory
    pangolin::CreateWindowAndBind("Trajectory Viewer", 1024, 768);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
        pangolin::ModelViewLookAt(-2, -2, -100, 0, 0, 0, pangolin::AxisZ));

    pangolin::View &d_cam = pangolin::CreateDisplay()
                                .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -1024.0f / 768.0f)
                                .SetHandler(new pangolin::Handler3D(s_cam));

    while (pangolin::ShouldQuit() == false)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        d_cam.Activate(s_cam);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glLineWidth(2);
        for (size_t i = 0; i < poses.size() - 1; i++)
        {
            Vector3d Ow = poses[i].translation();               // world origin
            Vector3d Xw = poses[i] * (0.1 * Vector3d(1, 0, 0)); // world x axis
            Vector3d Yw = poses[i] * (0.1 * Vector3d(0, 1, 0)); // world y axis
            Vector3d Zw = poses[i] * (0.1 * Vector3d(0, 0, 1)); // world z axis
            glBegin(GL_LINES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3d(Ow[0], Ow[1], Ow[2]);
            glVertex3d(Xw[0], Xw[1], Xw[2]);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3d(Ow[0], Ow[1], Ow[2]);
            glVertex3d(Yw[0], Yw[1], Yw[2]);
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3d(Ow[0], Ow[1], Ow[2]);
            glVertex3d(Zw[0], Zw[1], Zw[2]);
            glEnd();
        }

        for (size_t i = 0; i < poses.size() - 1; i++)
        {
            glColor3f(0.0, 0.0, 0.0);
            glBegin(GL_LINES);
            auto p1 = poses[i];
            auto p2 = poses[i + 1];
            glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
            glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
            // 两点之间距离
            // double dis = sqrt(pow(p1.translation()[0] - p2.translation()[0], 2) + pow(p1.translation()[1] - p2.translation()[1], 2) + pow(p1.translation()[2] - p2.translation()[2], 2));
            // printf("dis: %f\n", dis);
            glEnd();
        }
        // draw the axis
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        auto p1 = poses[0];
        glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
        glVertex3d(p1.translation()[0] + 1000, p1.translation()[1], p1.translation()[2]);
        glEnd();

        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINES);
        glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
        glVertex3d(p1.translation()[0], p1.translation()[1] + 1000, p1.translation()[2]);
        glEnd();

        glColor3f(0.0, 0.0, 1.0);
        glBegin(GL_LINES);
        glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
        glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2] + 1000);
        glEnd();
        pangolin::FinishFrame();
        usleep(5000);
    }
}

Vector3d RANSAC(vector<Vector3d> temp)
{
    int num_vecs = temp.size();
    int maxIterations = 40;

    double distanceV = 0.00137; // 0.99863: 3 in degree
    vector<int> destination_set;

    destination_set.push_back(3);
    while (maxIterations--)
    {
        int _inliner; // index of inliner
        _inliner = (rand() % num_vecs);

        vector<int> iner_set;
        if (iner_set.size() < 1)
        {
            iner_set.push_back(_inliner);
        }
        sort(iner_set.begin(), iner_set.end());

        for (size_t i = 0; i < num_vecs; i++)
        {
            if (binary_search(iner_set.begin(), iner_set.end(), i))
                continue;
            if (distance(temp[i], temp[_inliner]) < distanceV)
            {
                iner_set.push_back(i);
            }
        }
        if (destination_set.size() < iner_set.size())
        {
            destination_set.assign(iner_set.begin(), iner_set.end());
        }
    }

    Vector3d res(0, 0, 0);
    for (size_t i = 0; i < destination_set.size(); i++)
    {
        res += temp[destination_set[i]];
    }

    res /= ((double)destination_set.size());
    return res;
}

double distance(Vector3d &vec1, Vector3d &vec2)
{

    double product = 0;
    double squ01, squ02;
    for (size_t i = 0; i < 3; i++)
    {
        product += vec1[i] * vec2[i];
        squ01 += pow(vec1[i], 2);
        squ02 += pow(vec2[i], 2);
    }
    double correlation = product / (sqrt(squ01) * sqrt(squ02));
    return (1. - correlation);
}
