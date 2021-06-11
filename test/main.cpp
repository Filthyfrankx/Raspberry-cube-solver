#include <iostream>
#include <QDebug>
#include <wiringPi.h>
#include "hardwarecontrol.h"
//#include "imageprocess.h"
#include "solution.h"
#include "machine_state.h"
using namespace std;
#include <QObject>
#include <QTimer>
#include <QFile>
#include <QTextStream>    //文本流类，用于对文本文件的读写。

int main()
{

    cout << "Hello World!" << endl;
    wiringPiSetup();
    HardwareControl My_Control;



    QThread Solve_Thread;
    Solution solve;
    solve.moveToThread(&Solve_Thread);
    QObject::connect(&Solve_Thread,&QThread::started,&solve,&Solution::work);
    Solve_Thread.start();


    QThread Machine_state_Thread;
    machine_state Machine_state;
    Machine_state.moveToThread(&Machine_state_Thread);
    QObject::connect(&Machine_state_Thread,&QThread::started,&Machine_state,&machine_state::machine_state_show);
    Machine_state_Thread.start();

    QFile *temp = new QFile("/proc/cpuinfo");
    string cpuid = "000000000dba8fe8";
    pinMode(But_pin,INPUT);
    while(1)
    {
        if (!temp->open(QIODevice::ReadOnly | QIODevice::Text))  //打开CPU温度文件。
        {
            qDebug()<<"读取文件失败";   //需要引入<QDebug>头文件
            Machine_state.set_machine_state("system_error");
        }
        else
        {
            QTextStream stream(temp);
            QString cpuinfo = stream.readAll();  //读取温度。
            temp->close();                      //关闭文件。
            string st_cpuinfo = cpuinfo.toStdString();
            if(st_cpuinfo.find(cpuid)!= string::npos)
            {
                cout<<"okkk"<<cpuid<<endl;
                Machine_state.set_machine_state("run_normal");
                delete temp;
                break;
            }
            else
            {
                cout<<" no   okkk"<<cpuid<<endl;
                Machine_state.set_machine_state("system_error");
            }

        }
        delay(1000);
    }

    int mytime = 0;
    int run_flag = 0; //1 start run   0 ready  -1 error
    int but_down_count = 0;//ji su
    My_Control.Motor_T->ReciveRotationAngle(T_bar_up);
    My_Control.Motor_O->ReciveRotationAngle(O_bar_down);
    delay(260);
    My_Control.PositiongReset();
    /*************     **************/


    if(digitalRead(But_pin) == but_down)  //init threshold
    {
        temp = new QFile("./threshold");
        delay(10);
        while(digitalRead(But_pin) == but_down)
        {
            delay(1000);
        }//deng dai anjian tan qi
        //jiance yan se
        Machine_state.set_machine_state("FlipOverAndTakePhotos");
        string st = My_Control.get_all_threshold();
        if(st == "no_cam" || st == "photo_error" || st == "locked_rotor")
        {
            run_flag = -1;
            Machine_state.set_machine_state(st);
            My_Control.Motor_T->ReciveRotationAngle(Angle_disable);
            My_Control.Motor_O->ReciveRotationAngle(Angle_disable);
        }
        else //photo ok
        {
            std::cout<<st<<std::endl;
            while(1)
            {
                if (!temp->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))  //打开CPU温度文件。
                {
                    qDebug()<<"读取文件失败";
                    Machine_state.set_machine_state("system_error");
                }
                else
                {
                    string str_err = My_Control.My_imageprocess.string_to_hsv_thres(st);
                    if(str_err != "data_error")
                    {
                        QTextStream stream(temp);
                        QString w_qstr_threshold = QString::fromStdString(st);
                        stream << w_qstr_threshold << endl;
                        cout<<"write"<< w_qstr_threshold.toStdString() << endl;
                        temp->close();
                        Machine_state.set_machine_state("run_normal");
                        delete temp;
                        break;
                    }
                    else {
                        temp->close();
                        Machine_state.set_machine_state("system_error");
                    }

                  }
                delay(1000);
            }
        }
    }
    else  //du wen jian
    {
        temp = new QFile("./threshold");
        while(1)
        {
            if (!temp->open(QIODevice::ReadOnly | QIODevice::Text))  //打开CPU温度文件。
            {
                qDebug()<<"读取文件失败";
                Machine_state.set_machine_state("system_error");
            }
            else
            {
                QTextStream stream(temp);
                QString r_qstr_shreshold = stream.readAll();
                temp->close();
                string r_str_threshold = r_qstr_shreshold.toStdString();
                cout <<"read:"<< r_str_threshold << endl;
                string str_err = My_Control.My_imageprocess.string_to_hsv_thres(r_str_threshold);
                if(str_err != "data_error")
                {
                    Machine_state.set_machine_state("run_normal");
                    delete temp;
                    break;
                }
                else {

                    Machine_state.set_machine_state("system_error");
                }
            }
            delay(1000);
        }
    }

    My_Control.Motor_T->ReciveRotationAngle(Angle_disable);
    My_Control.Motor_O->ReciveRotationAngle(Angle_disable);
    while(1)
    {
        delay(100);
        if(digitalRead(But_pin) == but_down && run_flag == 0)
        {
            delay(10);
            if(digitalRead(But_pin) == but_down)
            {
                run_flag = 1;
                Machine_state.set_machine_state("run_normal");
                My_Control.PositiongReset();
//                string st = My_Control.get_all_threshold();
//                string str_err = My_Control.My_imageprocess.string_to_hsv_thres(st);
            }
        }
        while(run_flag == -1 && digitalRead(But_pin) == but_down)
        {
            but_down_count++;
            delay(1000);
            if(but_down_count > 4)
            {
                Machine_state.set_machine_state("run_normal");
                My_Control.PositiongReset();
                run_flag = 0;
                My_Control.Motor_T->ReciveRotationAngle(Angle_disable);
                My_Control.Motor_O->ReciveRotationAngle(Angle_disable);
            }
        }
        but_down_count = 0;
        if(run_flag == 1)
        {
            Machine_state.set_machine_state("FlipOverAndTakePhotos");
            string st = My_Control.FlipOverAndTakePhotos();
            if(st == "no_cam" || st == "photo_error" || st == "locked_rotor")
            {
                run_flag = -1;
                Machine_state.set_machine_state(st);
                My_Control.Motor_T->ReciveRotationAngle(Angle_disable);
                My_Control.Motor_O->ReciveRotationAngle(Angle_disable);
            }
            else //photo ok
            {
                Machine_state.set_machine_state("run_normal");
                cout<< st<<endl;
                Machine_state.set_machine_state("start_solve");

                solve.start_solve(st);
                for(mytime = 0; mytime < 100;mytime++)
                {
                    if(solve.solve_flag == 1)  //solve complete
                        break;
                    delay(100);

                }
                if(mytime == 100)  //over time
                {
                    solve.stop_solve();
                    Machine_state.set_machine_state("solve_error");
                    My_Control.Motor_T->ReciveRotationAngle(Angle_disable);
                    My_Control.Motor_O->ReciveRotationAngle(Angle_disable);
                    run_flag = -1;
                }
                else   //solve success
                {
                    Machine_state.set_machine_state("run_normal");
                    vector<int> step = solve.Step_Transformation();
                    string step_run = My_Control.Step_run(step);
                    if(step_run == "locked_rotor")
                    {
                        Machine_state.set_machine_state("locked_rotor");
                        My_Control.Motor_T->ReciveRotationAngle(Angle_disable);
                        My_Control.Motor_O->ReciveRotationAngle(Angle_disable);
                        run_flag = -1;

                    }
                    else{ //step ok
                        Machine_state.set_machine_state("run_normal");
                        My_Control.Motor_T->ReciveRotationAngle(Angle_disable);
                        My_Control.Motor_O->ReciveRotationAngle(Angle_disable);
                        run_flag = 0;
                    }
                }

            }

        }

    }
//        cout << digitalRead(But_pin) << endl;
//        if(digitalRead(But_pin) == but_down)
//        {
//            pinMode(R_led_pin,OUTPUT);
//            digitalWrite(R_led_pin,led_off);
//            pinMode(G_led_pin,OUTPUT);
//            digitalWrite(G_led_pin,led_off);
//        pinMode(B_led_pin,OUTPUT);
//        digitalWrite(B_led_pin,led_off);
//    }
//        else
//        {
//        pinMode(R_led_pin,OUTPUT);
//        digitalWrite(R_led_pin,led_on);
//        pinMode(G_led_pin,OUTPUT);
//        digitalWrite(R_led_pin,led_on);
//        pinMode(B_led_pin,OUTPUT);
//        digitalWrite(R_led_pin,led_on);
//        }


    //ImageProcess My_imageprocess;
    //My_imageprocess.get_side_color(0);
    //string st = My_Control.My_imageprocess.get_side_color();
    //My_Control.PositiongReset();
//   string st = My_Control.FlipOverAndTakePhotos();
//   cout<< st<<endl;
//    //string start = "FBBRRDFLLRUDFDBDBBBURRLDUFULLFFURUDRUBRLBFFLDDULDFULRB" ;
//    Solution solve;
//    solve.work(st);
//    vector<int> step = solve.Step_Transformation();
//    cout<< "STEP_SIZE"<<step.size()<<endl;
//    for( int j = 0;j < step.size();j++)
//    {
//        if(j>0)
//        {
//            My_Control.The_LAST_STEP = step[j-1];
//        }
//        else
//            My_Control.The_LAST_STEP = 0;
//        if(j+1 < step.size())
//        {
//            My_Control.The_NEXT_STEP = step[j+1];
//        }
//        else
//            My_Control.The_NEXT_STEP = 0;

//        switch(step[j])
//        {
//        case 1:   //顺时针90
//        {
//            My_Control.TurnTheCube_degrees(Clockwise,90);
//            cout<< "顺时针90"<<endl;
//            break;
//        }
//        case 2:   //顺时针180
//        {
//            My_Control.TurnTheCube_degrees(Clockwise,180);
//cout<< "顺时针180"<<endl;
//            break;
//        }
//        case 3:   //逆时针90
//        {
//            My_Control.TurnTheCube_degrees(Anticlockwise,90);
//cout<< "逆时针90"<<endl;
//            break;
//        }
//        case 4:   //整体顺时针90
//        {
//            My_Control.OverAllTurn(Clockwise,90)  ;      //整体转动rotation_angle度
//cout<< "整体顺时针90"<<endl;
//            break;
//        }
//        case 5:   //整体顺时针180
//        {
//            My_Control.OverAllTurn(Clockwise,180)  ;      //整体转动rotation_angle度
//cout<< "整体顺时针180"<<endl;
//            break;
//        }
//        case 6:   //整体逆时针90
//        {
//            My_Control.OverAllTurn(Anticlockwise,90)  ;      //整体转动rotation_angle度
//cout<< "整体逆时针90"<<endl;
//            break;
//        }
//        case 7:   //上翻1
//        {

//            My_Control.Cube_Flip(1) ;
//cout<< "上翻1"<<endl;
//            break;
//        }
//        case 8:    //上翻2
//        {

//            My_Control.Cube_Flip(2) ;
//cout<< "上翻2"<<endl;
//            break;
//        }
//        default:
//            break;
//        }
//    }
//    step.clear();
//    cv::waitKey(0);
//    return 0;
}


/*
 *
 * My_Control.OverAllTurn(Clockwise,90);
      My_Control.Cube_Flip(1);
      My_Control.TurnTheCube_degrees(Anticlockwise,90);
      My_Control.OverAllTurn(Clockwise,180);
      My_Control.Cube_Flip(3);
      My_Control.TurnTheCube_degrees(Clockwise,90);
 *      My_Control.Motor_O->ReciveRotationAngle(O_bar_up);   //弧形摆杆电机    //上扬
      delay(bar_swing_time);
      My_Control.Motor_T->ReciveRotationAngle(T_bar_pro_move);  //T型杆分阶段下降
      delay(T_bar_time_o_bar);
      My_Control.Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
      //delay(80);
      My_Control.Motor_T->ReciveRotationAngle(T_bar_down);
      delay(bar_swing_time);
      My_Control.Motor_T->ReciveRotationAngle(T_bar_up);
      delay(bar_swing_time);
 *


delayMicroseconds(10);
cv:: VideoCapture cap(0);
if(!cap.isOpened())
{
    return  -1;
}
cv::Mat frame;
while(1){
    cap>>frame;
   cv::imshow("Lena",frame);
   if(cv::waitKey(22)>=0)
       break;
}
*/
//cv::Mat image = cv::imread("/home/pi/Pictures/lena.png", cv::IMREAD_COLOR);
//image);cv::imshow("Lena",
//cv::waitKey(0);




/*
void on_MouseHandle(int event, int x , int y , int flags, void* param)
{
    cv::Mat& image = *(cv::Mat*) param;
    switch(event)
    {
    case cv::EVENT_LBUTTONDOWN:
    {
        cout << "(" << x <<","<<y<<")"<<"__";
        cout << "(" << static_cast<int>(image.at<cv::Vec3b>(y,x)[0]) << ",";
        cout << static_cast<int>(image.at<cv::Vec3b>(y,x)[1]) << ",";
        cout << static_cast<int>(image.at<cv::Vec3b>(y,x)[2]) << ")" << endl;
    }
        break;
    }
}
    cv:: VideoCapture cap(0);
    if(!cap.isOpened())
    {
        return  -1;
    }
    cv::Mat image;
    cv::Mat src_image;
    cv::Mat gray_image;
    cv::Mat canny_image;
    cv::Mat hsv;
    cv::Mat frame_threshold;
    while(1){

        cap>>src_image;
        image = src_image(cv::Range(40,400),cv::Range(140,500));// 80 y y x x
        cv::Rect rect(170,170,20,20);//x y w w

        //cv::rectangle(image,rect,cv::Scalar(255,0,0),1,cv::LINE_8,0);

        hsv = image.clone();
        frame_threshold =  image.clone();
        cvtColor(image,hsv,cv::COLOR_BGR2HSV);

        cv::inRange(hsv,cv::Scalar(0,0,100),cv::Scalar(255,255,255),gray_image);//er zhi hua


        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(8,8));
        cv::morphologyEx(gray_image,gray_image,cv::MORPH_CLOSE,element);
        cv::Canny(gray_image, canny_image, 50, 255, 5);
        double low_H = 0;
        double low_S = 43;
        double low_V = 46;
        double high_H = 10;
        double high_S = 255;
        double high_V = 255;

        cv::inRange(hsv,cv::Scalar(low_H,low_S,low_V),cv::Scalar(high_H,high_S,high_V),frame_threshold);

        //cvtColor(image,hsv,cv::COLOR_BGR2HSV);

        vector<cv::Vec4f> plines;
        cv::HoughLinesP(canny_image,plines,1,CV_PI / 180.0,30,60,30);
        cv::Scalar color2 = cv::Scalar(0,0,255);
        struct EDGE_LINE
        {
            int count = 0;
            double a = 0;
            double b = 0;
        };
        struct EDGE_LINE edge_lines[4];
        cv::Point edge_point[4];
        cv::Point centre_point[9];
         //cout << plines.size() << endl;
        for(unsigned int i = 0;i < plines.size();i++)
        {
            cv::Vec4f hine = plines[i];
            double deta_y,deta_x;
            deta_y = double(hine[3] -hine[1]);
            deta_x = double(hine[2] -hine[0]);
            if(fabs(deta_y)<fabs(deta_x))
            {
                double theta = deta_y/deta_x;
                if(hine[1] < 90 && hine[3] < 90 && fabs(theta) < my_threshod1 )
                {
                    edge_lines[0].count++;
                    edge_lines[0].a += theta;
                    edge_lines[0].b += hine[1] - theta*hine[0];

                }
                else if(hine[1] > 260 && hine[3] > 260 && fabs(theta) < my_threshod1 )
                {
                    edge_lines[2].count++;
                    edge_lines[2].a += theta;
                    edge_lines[2].b += hine[1] - theta*hine[0];

                }
            }
            else {
                double theta = deta_x/deta_y;
                if(hine[0] < 90 && hine[2] < 90 && fabs(theta) < my_threshod1)
                {
                    edge_lines[3].count++;
                    edge_lines[3].a += theta;
                    edge_lines[3].b += hine[0] - theta*hine[1];
                }
                else if(hine[0] > 260 && hine[2] > 260 && fabs(theta) < my_threshod1)
                {
                    edge_lines[1].count++;
                    edge_lines[1].a += theta;
                    edge_lines[1].b += hine[0] - theta*hine[1];
                }
            }
        }
        for(int i = 0;i<4;i++)
        {
            if(edge_lines[i].count != 0)
            {
                edge_lines[i].a = edge_lines[i].a/edge_lines[i].count;
                edge_lines[i].b = edge_lines[i].b/edge_lines[i].count;

            }
            else {
                cout << " NO CANNY *************************************"<< endl;
            }
        }

        edge_point[0].x = int((edge_lines[3].a*edge_lines[0].b+edge_lines[3].b)/(1-edge_lines[3].a*edge_lines[0].a));
        edge_point[0].y = int(edge_lines[0].a*edge_point[0].x+edge_lines[0].b);
        edge_point[1].x = int((edge_lines[1].a*edge_lines[0].b+edge_lines[1].b)/(1-edge_lines[1].a*edge_lines[0].a));
        edge_point[1].y = int(edge_lines[0].a*edge_point[1].x+edge_lines[0].b);
        edge_point[2].x = (edge_lines[1].a*edge_lines[2].b+edge_lines[1].b)/(1-edge_lines[1].a*edge_lines[2].a);
        edge_point[2].y = edge_lines[2].a*edge_point[2].x+edge_lines[2].b;
        edge_point[3].x = (edge_lines[3].a*edge_lines[2].b+edge_lines[3].b)/(1-edge_lines[3].a*edge_lines[2].a);
        edge_point[3].y = edge_lines[2].a*edge_point[3].x+edge_lines[2].b;

        double len = sqrt(pow((edge_point[1].x-edge_point[0].x),2)+pow((edge_point[1].y-edge_point[0].y),2)) +
                sqrt(pow((edge_point[2].x-edge_point[1].x),2)+pow((edge_point[2].y-edge_point[1].y),2)) +
                sqrt(pow((edge_point[3].x-edge_point[2].x),2)+pow((edge_point[3].y-edge_point[2].y),2)) +
                sqrt(pow((edge_point[0].x-edge_point[3].x),2)+pow((edge_point[0].y-edge_point[3].y),2)) ;
        len = len/4.0;
        // x   ,  y

        double edge_4point_theta1= (edge_point[3].x-edge_point[0].x)/3;    // y /x
        double edge_4point_theta2= (edge_point[3].y-edge_point[0].y)/3;
        centre_point[4].x = (edge_point[0].x +edge_point[1].x+edge_point[2].x+edge_point[3].x)/4;
        centre_point[4].y = (edge_point[0].y +edge_point[1].y+edge_point[2].y+edge_point[3].y)/4;
        centre_point[1].x = centre_point[4].x - edge_4point_theta1;
        centre_point[1].y = centre_point[4].y - edge_4point_theta2;
        centre_point[7].x = centre_point[4].x + edge_4point_theta1;
        centre_point[7].y = centre_point[4].y + edge_4point_theta2;
        centre_point[5].x = centre_point[4].x + edge_4point_theta2;
        centre_point[5].y = centre_point[4].y - edge_4point_theta1;
        centre_point[3].x = centre_point[4].x - edge_4point_theta2;
        centre_point[3].y = centre_point[4].y + edge_4point_theta1;
        centre_point[0].x = centre_point[3].x - edge_4point_theta1;
        centre_point[0].y = centre_point[3].y - edge_4point_theta2;
        centre_point[2].x = centre_point[5].x - edge_4point_theta1;
        centre_point[2].y = centre_point[5].y - edge_4point_theta2;
        centre_point[6].x = centre_point[3].x + edge_4point_theta1;
        centre_point[6].y = centre_point[3].y + edge_4point_theta2;
        centre_point[8].x = centre_point[5].x + edge_4point_theta1;
        centre_point[8].y = centre_point[5].y + edge_4point_theta2;

        cv::circle(image,centre_point[0],20,color2);
        cv::circle(image,centre_point[1],20,color2);
        cv::circle(image,centre_point[2],20,color2);
        cv::circle(image,centre_point[3],20,color2);
        cv::circle(image,centre_point[4],20,color2);
        cv::circle(image,centre_point[5],20,color2);
        cv::circle(image,centre_point[6],20,color2);
        cv::circle(image,centre_point[7],20,color2);
        cv::circle(image,centre_point[8],20,color2);

        cv::line(image,edge_point[0],edge_point[1],color2,3,cv::LINE_AA);
        cv::line(image,edge_point[1],edge_point[2],color2,3,cv::LINE_AA);
        cv::line(image,edge_point[2],edge_point[3],color2,3,cv::LINE_AA);
        cv::line(image,edge_point[3],edge_point[0],color2,3,cv::LINE_AA);
        cv::imshow("init",image);
        cv::imshow("BINARY",gray_image);

        cv::namedWindow("hsv");
        cv::setMouseCallback("hsv",on_MouseHandle,(void*)&hsv);
        cv::imshow("hsv",hsv);

        cv::imshow("canny_image",canny_image);

        cv::imshow("out2",frame_threshold);
       if(cv::waitKey(22)>=0)
           break;
    }
*/
