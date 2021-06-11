#include "imageprocess.h"

struct EDGE_LINE
{
    int count = 0;
    double a = 0;
    double b = 0;
};

void on_MouseHandle(int event, int x , int y , int flags, void* param)
{
    cv::Mat& image = *(cv::Mat*) param;
    switch(event)
    {
    case cv::EVENT_LBUTTONDOWN:
    {
        std::cout << "(" << x <<","<<y<<")"<<"__";
        std::cout << "(" << static_cast<int>(image.at<cv::Vec3b>(y,x)[0]) << ",";
        std::cout << static_cast<int>(image.at<cv::Vec3b>(y,x)[1]) << ",";
        std::cout << static_cast<int>(image.at<cv::Vec3b>(y,x)[2]) << ")" << std::endl;
    }
        break;
    }
}

ImageProcess::ImageProcess()
{
 cap = new VideoCapture(0);
 if(!cap->isOpened())
 {
     std::cout << "no camera" << std::endl;
 }
 else std::cout << "open camera success" << std::endl;
 delete cap;

}


std::string ImageProcess::get_side_color(void)
{

    cap = new VideoCapture(0);
    while(!cap->isOpened())
    {
        delete cap;
        cap = new VideoCapture(0);
        photo_err_count++;
        if(photo_err_count>err_sum)
        {
            if(cap->isOpened())
            {
                delete  cap;
            }
            photo_err_count = 0;
            std::cout << "no camera" << std::endl;
            return "no_cam";
        }
    }
    photo_err_count = 0;
    std::cout << "open camera success" << std::endl;
    Mat image;
    Mat src_image;
    Mat gray_image;
    Mat canny_image;
    Mat hsv;
   // std::map<char,int> one_side_color;
   // cv::Mat frame_threshold;
    while(1){

        *cap >> src_image;
        image = src_image(Range(40,400),Range(140,500));// 80 y y x x
        hsv = image.clone();
       // frame_threshold =  image.clone();


        cvtColor(image,hsv,COLOR_BGR2HSV);
        inRange(hsv,Scalar(0,0,100),cv::Scalar(255,255,255),gray_image);//er zhi hua
        Mat element = getStructuringElement(MORPH_RECT,Size(8,8));
        morphologyEx(gray_image,gray_image,MORPH_CLOSE,element);
        Canny(gray_image, canny_image, 50, 255, 5);
//        double low_H = 0;
//        double low_S = 43;
//        double low_V = 46;
//        double high_H = 10;
//        double high_S = 255;
//        double high_V = 255;
//        cv::inRange(hsv,cv::Scalar(low_H,low_S,low_V),cv::Scalar(high_H,high_S,high_V),frame_threshold);

        std::vector<cv::Vec4f> plines;
        cv::HoughLinesP(canny_image,plines,1,CV_PI / 180.0,30,60,30);
        cv::Scalar color2 = cv::Scalar(0,0,255);

        struct EDGE_LINE edge_lines[4];
        cv::Point edge_point[4];
        cv::Point centre_point[9];

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
        if(edge_lines[0].count * edge_lines[1].count * edge_lines[2].count * edge_lines[3].count != 0)
        {

            for(int i = 0;i<4;i++)
            {
                edge_lines[i].a = edge_lines[i].a/edge_lines[i].count;
                edge_lines[i].b = edge_lines[i].b/edge_lines[i].count;

            }
//            std::cout <<"1"<< edge_lines[0].a-edge_lines[2].a << std::endl;
//            std::cout <<"2"<< edge_lines[1].a-edge_lines[3].a << std::endl;
//            std::cout <<"3"<< edge_lines[0].a/edge_lines[1].a << std::endl;
//            std::cout <<"4"<< edge_lines[0].a/edge_lines[3].a << std::endl;
            edge_point[0].x = int((edge_lines[3].a*edge_lines[0].b+edge_lines[3].b)/(1-edge_lines[3].a*edge_lines[0].a));
            edge_point[0].y = int(edge_lines[0].a*edge_point[0].x+edge_lines[0].b);
            edge_point[1].x = int((edge_lines[1].a*edge_lines[0].b+edge_lines[1].b)/(1-edge_lines[1].a*edge_lines[0].a));
            edge_point[1].y = int(edge_lines[0].a*edge_point[1].x+edge_lines[0].b);
            edge_point[2].x = (edge_lines[1].a*edge_lines[2].b+edge_lines[1].b)/(1-edge_lines[1].a*edge_lines[2].a);
            edge_point[2].y = edge_lines[2].a*edge_point[2].x+edge_lines[2].b;
            edge_point[3].x = (edge_lines[3].a*edge_lines[2].b+edge_lines[3].b)/(1-edge_lines[3].a*edge_lines[2].a);
            edge_point[3].y = edge_lines[2].a*edge_point[3].x+edge_lines[2].b;


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
            color_side.clear();
            for(int i = 0 ; i< 9 ; i++)
            {
                for (int cnt = 0;cnt<6;cnt++) {
                    color_count[cnt] = 0;
                }
                for(int x_deta = 0; x_deta<20; x_deta++)
                {
                    int y_deta = int(sqrt(400 - x_deta * x_deta));
                    Point hsv_ROI[4];
                    hsv_ROI[0].x = centre_point[i].x + x_deta;
                    hsv_ROI[0].y = centre_point[i].y + y_deta;
                    hsv_ROI[1].x = centre_point[i].x - x_deta;
                    hsv_ROI[1].y = centre_point[i].y + y_deta;
                    hsv_ROI[2].x = centre_point[i].x - x_deta;
                    hsv_ROI[2].y = centre_point[i].y - y_deta;
                    hsv_ROI[3].x = centre_point[i].x + x_deta;
                    hsv_ROI[3].y = centre_point[i].y - y_deta;

                    for(int j= 0;j<4;j++)
                    {
                        int H = static_cast<int>(hsv.at<cv::Vec3b>(hsv_ROI[j].y,hsv_ROI[j].x)[0]);
                        int S = static_cast<int>(hsv.at<cv::Vec3b>(hsv_ROI[j].y,hsv_ROI[j].x)[1]);
                        int V = static_cast<int>(hsv.at<cv::Vec3b>(hsv_ROI[j].y,hsv_ROI[j].x)[2]);
                        if(j%4 == 0 && x_deta%6 == 0)
                            std::cout << i<<"(" << H <<","<<S<<","<<V<<std::endl;
//                        if(H <= 50 && H >=28 ) //uH < 50 && H > 25    30 31 34  35   42  43 44 45 46
//                        {
//                           color_count[0] += 1;
//                        }
//                        else if(H < 104 && H > 76 && S+30 < V) //d        80 89 90 91 92 94 95 96 97   98 99 101  97  s<v
//                        {
//                           color_count[1] += 1;
//                        }
//                        else if(H <= 11 && H > 0) //l       4  5 6 7  9
//                        {
//                           color_count[2] += 1;
//                        }
//                        else if( H < 190 && H > 160) //r        169 170 171
//                        {
//                           color_count[3] += 1;
//                        }
//                        else if(H < 115 && H >=100 ) //f    106 107 n  104 105 103 102  108  s>v
//                        {
//                           color_count[4] += 1;
//                        }
//                        else if(H <= 84 && H >= 58 ) //b         63 65 66 74 73 75  80  81  82
//                        {
//                           color_count[5] += 1;
//                        }
                        //zhu yi liang he sun xu bu yi yang
                        if(H > hsv_threshold[0][0]&& H <= hsv_threshold[0][1])  //l
                            color_count[2] += 1;
                        else if(H > hsv_threshold[1][0]&& H <= hsv_threshold[1][1])//u
                            color_count[0] += 1;
                        else if((H > hsv_threshold[2][0]&& H <= hsv_threshold[2][1]&& S > 160) ||
                                (H > hsv_threshold[2][0]&& H <= (hsv_threshold[2][1]+2)+2 && S > 200 && V >140 && S+10>V)||
                                (H > hsv_threshold[2][0]&& H <= (hsv_threshold[2][1]+2)+5 && S > 240 && V >140 && S+10>V))//b
                            color_count[5] += 1;
                        else if((H > hsv_threshold[4][0]&& H <= hsv_threshold[4][1]&& S > 160)||
                                (H > hsv_threshold[4][0]-2 && H <= (hsv_threshold[4][1]+2) && S > 200 && V > 140 && S+10>V)
                                ||(H > hsv_threshold[4][0]-5 && H <= (hsv_threshold[4][1]+2) && S > 240 && V > 140 && S+10>V))//f
                            color_count[4] += 1;
                        else if((H > hsv_threshold[3][0]&& H <= hsv_threshold[3][1]) ||
                                (H > (hsv_threshold[3][0] - 10)&& H <= (hsv_threshold[3][1]+10) &&S+30 < V)||
                                (H > (hsv_threshold[2][0]+5)&& H <= (hsv_threshold[4][1]-5) && S < 40)||
                                (  S < 11))//d
                            color_count[1] += 1;
                        else if((H > hsv_threshold[5][0]&& H <= hsv_threshold[5][1]) || H == 0)//r
                            color_count[3] += 1;

                    }

                }
                int max = 0;

                for (int cnt = 0;cnt<6;cnt++)
                {

                    if (color_count[max] < color_count[cnt])
                        max  = cnt ;

                }
                if(color_count[max] < 50)
                {
                     std::cout <<  "data error"  << i << ":" << color_count[max]<< ":" <<color_temp[max]<< std::endl;
                     photo_err_count++;
                     if(photo_err_count>err_sum)
                     {
                         if(cap->isOpened())
                         {
                             delete  cap;
                         }
                         photo_err_count = 0;
                         return "photo_error";
                     }
                }
                else
                {
                    //std::cout << i << ":" << color_count[max] << std::endl;
                    std::cout << i << ":" <<color_temp[max] << ":" << color_count[max]<< std::endl;
                    color_side +=color_temp[max];
                }

            }
            //WUCUO........
            if(color_side.size() == 9)
            {
                if(cap->isOpened())
                {
                    delete  cap;
                }
                photo_err_count = 0;
                std::cout << color_side << std::endl;
                return color_side;
            }


//            std::cout << "(" << static_cast<int>(hsv.at<cv::Vec3b>(centre_point[4].y,centre_point[4].x)[0]) << ",";
//            std::cout << static_cast<int>(hsv.at<cv::Vec3b>(centre_point[4].y,centre_point[4].x)[1]) << ",";
//            std::cout << static_cast<int>(hsv.at<cv::Vec3b>(centre_point[4].y,centre_point[4].x)[2]) << ")" << std::endl;
//            cv::circle(image,centre_point[0],20,color2);
//            cv::circle(image,centre_point[1],20,color2);
//            cv::circle(image,centre_point[2],20,color2);
//            cv::circle(image,centre_point[3],20,color2);
//            cv::circle(image,centre_point[4],20,color2);
//            cv::circle(image,centre_point[5],20,color2);
//            cv::circle(image,centre_point[6],20,color2);
//            cv::circle(image,centre_point[7],20,color2);
//            cv::circle(image,centre_point[8],20,color2);

//            cv::line(image,edge_point[0],edge_point[1],color2,3,cv::LINE_AA);
//            cv::line(image,edge_point[1],edge_point[2],color2,3,cv::LINE_AA);
//            cv::line(image,edge_point[2],edge_point[3],color2,3,cv::LINE_AA);
//            cv::line(image,edge_point[3],edge_point[0],color2,3,cv::LINE_AA);
//            cv::imshow("init",image);
//            cv::imshow("BINARY",gray_image);
//            cv::namedWindow("hsv");
//            cv::setMouseCallback("hsv",on_MouseHandle,(void*)&hsv);
//            cv::imshow("hsv",hsv);
//            cv::imshow("canny_image",canny_image);

//            if(cv::waitKey(22)>=0)
//               break;
        }
        else {
            std::cout << " NO CANNY ************"<<std::endl;
            photo_err_count++;
            if(photo_err_count>err_sum)
            {
                if(cap->isOpened())
                {
                    delete  cap;
                }
                photo_err_count = 0;
                return "photo_error";
            }

        }


    }

}

void ImageProcess::hsv_thrshold_put(int num,int HL,int HH,int SL,int SH,int VL,int VH)
{
    hsv_threshold[num][0] = HL;
    hsv_threshold[num][1] = HH;
    hsv_threshold[num][2] = SL;
    hsv_threshold[num][3] = SH;
    hsv_threshold[num][4] = VL;
    hsv_threshold[num][5] = VH;
}
std::string ImageProcess::init_color_threshold(void)
{

    cap = new VideoCapture(0);
    while(!cap->isOpened())
    {
        delete cap;
        cap = new VideoCapture(0);
        photo_err_count++;
        if(photo_err_count>err_sum)
        {
            if(cap->isOpened())
            {
                delete  cap;
            }
            photo_err_count = 0;
            std::cout << "no camera" << std::endl;
            return "no_cam";
        }
    }
    photo_err_count = 0;
    std::cout << "open camera success" << std::endl;
    Mat image;
    Mat src_image;
    Mat gray_image;
    Mat canny_image;
    Mat hsv;
    while(1){
std::cout << "***************************" << std::endl;
        *cap >> src_image;
        image = src_image(Range(40,400),Range(140,500));// 80 y y x x
        hsv = image.clone();
       // frame_threshold =  image.clone();
        cvtColor(image,hsv,COLOR_BGR2HSV);
        inRange(hsv,Scalar(0,0,100),cv::Scalar(255,255,255),gray_image);//er zhi hua
        Mat element = getStructuringElement(MORPH_RECT,Size(8,8));
        morphologyEx(gray_image,gray_image,MORPH_CLOSE,element);
        Canny(gray_image, canny_image, 50, 255, 5);
        std::vector<cv::Vec4f> plines;
        cv::HoughLinesP(canny_image,plines,1,CV_PI / 180.0,30,60,30);
        cv::Scalar color2 = cv::Scalar(0,0,255);

        struct EDGE_LINE edge_lines[4];
        cv::Point edge_point[4];
        cv::Point centre_point[9];

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
        if(edge_lines[0].count * edge_lines[1].count * edge_lines[2].count * edge_lines[3].count != 0)
        {

            for(int i = 0;i<4;i++)
            {
                edge_lines[i].a = edge_lines[i].a/edge_lines[i].count;
                edge_lines[i].b = edge_lines[i].b/edge_lines[i].count;

            }
//            std::cout <<"1"<< edge_lines[0].a-edge_lines[2].a << std::endl;
//            std::cout <<"2"<< edge_lines[1].a-edge_lines[3].a << std::endl;
//            std::cout <<"3"<< edge_lines[0].a/edge_lines[1].a << std::endl;
//            std::cout <<"4"<< edge_lines[0].a/edge_lines[3].a << std::endl;
            edge_point[0].x = int((edge_lines[3].a*edge_lines[0].b+edge_lines[3].b)/(1-edge_lines[3].a*edge_lines[0].a));
            edge_point[0].y = int(edge_lines[0].a*edge_point[0].x+edge_lines[0].b);
            edge_point[1].x = int((edge_lines[1].a*edge_lines[0].b+edge_lines[1].b)/(1-edge_lines[1].a*edge_lines[0].a));
            edge_point[1].y = int(edge_lines[0].a*edge_point[1].x+edge_lines[0].b);
            edge_point[2].x = (edge_lines[1].a*edge_lines[2].b+edge_lines[1].b)/(1-edge_lines[1].a*edge_lines[2].a);
            edge_point[2].y = edge_lines[2].a*edge_point[2].x+edge_lines[2].b;
            edge_point[3].x = (edge_lines[3].a*edge_lines[2].b+edge_lines[3].b)/(1-edge_lines[3].a*edge_lines[2].a);
            edge_point[3].y = edge_lines[2].a*edge_point[3].x+edge_lines[2].b;


            double edge_4point_theta1= (edge_point[3].x-edge_point[0].x)/3;    // y /x
            double edge_4point_theta2= (edge_point[3].y-edge_point[0].y)/3;
            centre_point[4].x = (edge_point[0].x +edge_point[1].x+edge_point[2].x+edge_point[3].x)/4;
            centre_point[4].y = (edge_point[0].y +edge_point[1].y+edge_point[2].y+edge_point[3].y)/4;

            std::map<int,int> statistics_color[3];

            for(int x_deta = 0; x_deta<20; x_deta++)
            {
                int y_deta = int(sqrt(400 - x_deta * x_deta));
                Point hsv_ROI[4];
                hsv_ROI[0].x = centre_point[4].x + x_deta;
                hsv_ROI[0].y = centre_point[4].y + y_deta;
                hsv_ROI[1].x = centre_point[4].x - x_deta;
                hsv_ROI[1].y = centre_point[4].y + y_deta;
                hsv_ROI[2].x = centre_point[4].x - x_deta;
                hsv_ROI[2].y = centre_point[4].y - y_deta;
                hsv_ROI[3].x = centre_point[4].x + x_deta;
                hsv_ROI[3].y = centre_point[4].y - y_deta;

                for(int j= 0;j<4;j++)
                {
                    int H = static_cast<int>(hsv.at<cv::Vec3b>(hsv_ROI[j].y,hsv_ROI[j].x)[0]);
                    int S = static_cast<int>(hsv.at<cv::Vec3b>(hsv_ROI[j].y,hsv_ROI[j].x)[1]);
                    int V = static_cast<int>(hsv.at<cv::Vec3b>(hsv_ROI[j].y,hsv_ROI[j].x)[2]);
                    if(statistics_color[0].empty()||statistics_color[0].find(H) == statistics_color[0].end())
                    {
                        statistics_color[0][H] = 1;
                    }
                    else
                        statistics_color[0][H] = statistics_color[0][H] + 1;
                    if(statistics_color[1].empty()||statistics_color[1].find(S) == statistics_color[1].end())
                    {
                        statistics_color[1][S] = 1;
                    }
                    else
                        statistics_color[1][S] = statistics_color[1][S] + 1;
                    if(statistics_color[2].empty()||statistics_color[2].find(V) == statistics_color[2].end())
                    {
                        statistics_color[2][V] = 1;
                    }
                    else
                        statistics_color[2][V] = statistics_color[2][V] + 1;
                }

            }
            std::map<int,int>::iterator i;
            for(i = statistics_color[0].begin();i != statistics_color[0].end();i++)
            {
                std::cout<< i->first <<"-"<< i->second<< " ";
            }
            std::cout<<std::endl;
//            std::cout << "  ////  " ;
//            for(i = statistics_color[1].begin();i != statistics_color[1].end();i++)
//            {
//                std::cout<< i->first <<"-"<< i->second<< " ";
//            }
//            std::cout << "  ////  " ;
//            for(i = statistics_color[2].begin();i != statistics_color[2].end();i++)
//            {
//                std::cout<< i->first <<"-"<< i->second<< " ";
//            }
            int H_L =0 ;
            int H_H =0 ;
            for(i = statistics_color[0].begin();i != statistics_color[0].end();i++)
            {
                if(i->second > 6 )
                {
                  H_L =  i->first;
                  break;
                }
            }
            for(i = statistics_color[0].end();i != statistics_color[0].begin();)
            {
                i--;
                if(i->second > 6 )
                {
                  H_H =  i->first;
                  break;
                }
            }
            std::cout<<H_L <<"-"<< H_H<< std::endl;  //zui xiao fan wei
            std::string str_threshold = "_"+std::to_string(H_L)+"_"+std::to_string(H_H);
            if(cap->isOpened())
            {
                delete  cap;
            }
            photo_err_count = 0;
            return str_threshold;
//            int S_L = ;
//            int S_H = ;
//            int V_L = ;
//            int V_H = ;

        }
        else {
            std::cout << " NO CANNY ************"<<std::endl;
            photo_err_count++;
            if(photo_err_count>err_sum)
            {
                if(cap->isOpened())
                {
                    delete  cap;
                }
                photo_err_count = 0;
                return "photo_error";
            }
        }
    }
}

std::string ImageProcess::string_to_hsv_thres(std::string str_threshold)
{//low zai qian high zai hou
    std::map<int,int> low_thr;
    std::map<int,int> high_thr;
    std::vector<int> pos_cord;
    size_t pos = 0;
    while(1){
        size_t pos_temp = str_threshold.find('_',pos);
        if(pos_temp!=std::string::npos)
        {
            pos_cord.push_back(int(pos_temp));
            std::cout<<"pos_temp: "<<pos_temp<<"_"<<str_threshold[pos_temp+1]<<std::endl;
            pos = pos_temp+1;
        }
        else {
            break;
        }
    }
    std::cout<<pos_cord.size()<<std::endl;
    if(pos_cord.size()==13)
    {

      for(int i = 0 ;i<12;i=i+2) // i  i+1
      {
          std::string str_dec_l(str_threshold,pos_cord[i]+1,pos_cord[i+1] - pos_cord[i]-1) ;
          low_thr[std::stoi(str_dec_l)] = i/2;
          std::string str_dec_h(str_threshold,pos_cord[i+1]+1,pos_cord[i+2] - pos_cord[i+1]-1) ;
          high_thr[std::stoi(str_dec_h)] = i/2;
         //  key  paixu  xiao dao da
      }
      if(low_thr.size() != high_thr.size())
          return "data_error";
      std::map<int,int>::iterator low_thr_iter = low_thr.begin();
      std::map<int,int>::iterator high_thr_iter = high_thr.begin();
      while(low_thr_iter != low_thr.end() && high_thr_iter != high_thr.end())//pan duan sunxu
      {
          if(low_thr_iter->second == high_thr_iter->second)//GENGJU key pai xu
          {
              low_thr_iter++;
              high_thr_iter++;
          }
          else
              return "data_error";
      }
      low_thr_iter = low_thr.begin();
      high_thr_iter = high_thr.begin();

      hsv_threshold[0][0] = 0;//hl
      low_thr_iter++;
      hsv_threshold[0][1] = (high_thr_iter->first + low_thr_iter->first)/2;//hh

      hsv_threshold[1][0] = (high_thr_iter->first + low_thr_iter->first)/2;//hl
      low_thr_iter++;
      high_thr_iter++;
      hsv_threshold[1][1] = (high_thr_iter->first + low_thr_iter->first)/2;//hh

      hsv_threshold[2][0] = (high_thr_iter->first + low_thr_iter->first)/2;//hl
      low_thr_iter++;
      high_thr_iter++;
      hsv_threshold[2][1] = (high_thr_iter->first + low_thr_iter->first)/2;//hh

      hsv_threshold[3][0] = (high_thr_iter->first + low_thr_iter->first)/2;//hl
      low_thr_iter++;
      high_thr_iter++;
      hsv_threshold[3][1] = (high_thr_iter->first + low_thr_iter->first)/2;//hh

      hsv_threshold[4][0] = (high_thr_iter->first + low_thr_iter->first)/2;//hl
      low_thr_iter++;
      high_thr_iter++;
      hsv_threshold[4][1] = (high_thr_iter->first + low_thr_iter->first)/2;//hh

      hsv_threshold[5][0] = (high_thr_iter->first + low_thr_iter->first)/2;//hl
      high_thr_iter++;
      hsv_threshold[5][1] = high_thr_iter->first + 20;//hh

      //fen kong jian L U B D F R
      for(int i = 0;i<6;i++)
      {
          std::cout<<hsv_threshold[i][0]<<" ~ "<<hsv_threshold[i][1]<<std::endl;
      }
      return "data_seccess";
    }
    else
        return "data_error";
}


