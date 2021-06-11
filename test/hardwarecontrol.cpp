#include "hardwarecontrol.h"
#include <QDebug>
HardwareControl::HardwareControl(QObject *parent) :  QObject(parent)
{   //INIT
    SteppingMotor_init();

    pinMode(PE_pin,INPUT);

    qDebug()<<"main thread:"<<QThread::currentThreadId();
    Motor_T = new MG955(T_pin);
    Motor_O = new MG955(O_pin);
    Motor_T->moveToThread(&Motor_T_Thread);
    Motor_O->moveToThread(&Motor_O_Thread);
    connect(&Motor_T_Thread,&QThread::started,Motor_T,&MG955::MG955Work);
    connect(&Motor_O_Thread,&QThread::started,Motor_O,&MG955::MG955Work);
    //FINISH.....

    Motor_T_Thread.start();
    Motor_O_Thread.start();
}
 void HardwareControl::SteppingMotor_init()
 {
     pinMode(SM_en_pin,OUTPUT);
     digitalWrite(SM_en_pin,SM_Disable);
     pinMode(SM_dir_pin,OUTPUT);
     digitalWrite(SM_dir_pin,LOW);
     pinMode(SM_pul_pin,OUTPUT);
     digitalWrite(SM_pul_pin,LOW);
 }
std::string HardwareControl::SteppingMotor_Control(unsigned char dir,unsigned int rotation_angle,unsigned char en,unsigned char speed)
{
    unsigned int i = 0;
    unsigned int count = 0;
    unsigned int my_speed = speed;
    count = ((rotation_angle)*16/1.8);

    digitalWrite(SM_en_pin,SM_Enable);
    digitalWrite(SM_dir_pin,dir);
    delayMicroseconds(10);

    for(i=0;i < count;i++)
    {
        if(rotation_angle == 90 || rotation_angle == 180)
        {
            if(i == count/5)
                my_speed = speed+20;
            else if (i == count*1/6)
                my_speed = speed+10;
            else if (i == count*2/6)
                my_speed = speed;
            else if (i == count*3/6)
                my_speed = speed;
            else if (i == count*4/6)
                my_speed = speed;
            else if (i == count*5/6)
                my_speed = speed+15;
            else
                my_speed = speed+30;

        }
        else
            my_speed = speed;
        digitalWrite(SM_pul_pin,LOW);
        delayMicroseconds(my_speed);
        digitalWrite(SM_pul_pin,HIGH);
        delayMicroseconds(my_speed);
        digitalWrite(SM_pul_pin,LOW);
    }
    //JIANCE .....

    digitalWrite(SM_en_pin,SM_Enable);
    if(digitalRead(PE_pin) == Shelter)
        return "locked_rotor";
    else {
        return "motor_ok";
    }

}
void HardwareControl::PositiongReset(void)
{
    int reset_flag = 0;
    int count = 0;
    unsigned int speed = PositiongResetSpeed;
    Motor_T->ReciveRotationAngle(T_bar_up);
    Motor_O->ReciveRotationAngle(O_bar_down);
    digitalWrite(SM_en_pin,SM_Enable);
    digitalWrite(SM_dir_pin,Clockwise);

    delay(bar_swing_time);
    //Motor_T->ReciveRotationAngle(Angle_disable);
    //Motor_O->ReciveRotationAngle(Angle_disable);
    while(1)
    {
        if(reset_flag == 0 && digitalRead(PE_pin) == Shelter)
            reset_flag = 1;
        if(reset_flag == 1 && digitalRead(PE_pin) == NoShelter)
        {
            //delay(10);
            if(digitalRead(PE_pin) == NoShelter)
            {
                reset_flag = 0;
                count++;
                speed = speed + 7;
                if(count>2)
                    break;
            }
        }
        digitalWrite(SM_pul_pin,LOW);
        delayMicroseconds(speed);
        digitalWrite(SM_pul_pin,HIGH);
        delayMicroseconds(speed);
        digitalWrite(SM_pul_pin,LOW);
    }
    SteppingMotor_Control(Clockwise,4,SM_Enable,130);
    digitalWrite(SM_en_pin,SM_Enable);
}
std::string HardwareControl::TurnTheCube_degrees(unsigned char dir,unsigned int rotation_angle )//扭转魔方底层90度
{
    if(The_LAST_STEP == 0 || The_LAST_STEP == 4 || The_LAST_STEP == 5 || The_LAST_STEP == 6)
        //拍照后第一动作需下压,整体转动后需下压      其他不需要下压
        {
            Motor_T->ReciveRotationAngle(T_bar_down);      //T型摆杆        //下压
            delay(bar_swing_time);
            //Motor_T->ReciveRotationAngle(Angle_disable);
        }
    std::string temp_state = SteppingMotor_Control(dir,rotation_angle,SM_Enable,StandartSpeed);
    if(temp_state == "locked_rotor")
        return "locked_rotor";
    if(The_NEXT_STEP != 1 && The_NEXT_STEP != 2 && The_NEXT_STEP != 3)
        //后一动作为转动底盘    不需要T杆上扬
    {
      Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
      delay(bar_swing_time);
      //Motor_T->ReciveRotationAngle(Angle_disable);
    }
    return "motor_ok";
}
void HardwareControl::Cube_Flip(unsigned char count )
{
    int i;
    //以上扬了一次
    Motor_O->ReciveRotationAngle(O_bar_up);   //弧形摆杆电机    //上扬
    delay(bar_swing_time);
    //Motor_O->ReciveRotationAngle(Angle_disable);

    Motor_T->ReciveRotationAngle(T_bar_pro_move);  //T型杆分阶段下降
    delay(T_bar_time_o_bar);
    //Motor_T->ReciveRotationAngle(Angle_disable);

    Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
    delay(O_bar_time_T_bar);
    Motor_T->ReciveRotationAngle(T_bar_down);
    delay(bar_swing_time);
    //Motor_T->ReciveRotationAngle(Angle_disable);
    //Motor_O->ReciveRotationAngle(Angle_disable);


    //以上扬了一次
    for(i=0;i<count-1;i++)
    {
        Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
        delay(140);  //分段动作，需先上扬

        Motor_O->ReciveRotationAngle(O_bar_up);   //弧形摆杆电机    //上扬
        delay(bar_swing_time);
        //Motor_T->ReciveRotationAngle(Angle_disable);
        //Motor_O->ReciveRotationAngle(Angle_disable);

        Motor_T->ReciveRotationAngle(T_bar_pro_move);  //T型杆分阶段下降
        delay(T_bar_time_o_bar);
        //Motor_T->ReciveRotationAngle(Angle_disable);

        Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
        delay(O_bar_time_T_bar);
        Motor_T->ReciveRotationAngle(T_bar_down);
        delay(bar_swing_time);
        //Motor_T->ReciveRotationAngle(Angle_disable);
        //Motor_O->ReciveRotationAngle(Angle_disable);
    }
    if(The_NEXT_STEP != 1 && The_NEXT_STEP != 2 && The_NEXT_STEP != 3)
        //后一动作为转动底盘    不需要T杆上扬
        {
            Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
            delay(bar_swing_time);
            //Motor_T->ReciveRotationAngle(Angle_disable);
        }
}
std::string HardwareControl::OverAllTurn(unsigned char dir,unsigned int rotation_angle )        //整体转动rotation_angle度
{
    std::string temp_state = SteppingMotor_Control(dir,rotation_angle,SM_Enable,StandartSpeed);
    if(temp_state == "locked_rotor")
        return "locked_rotor";
    return "motor_ok";
}
std::string HardwareControl::FlipOverAndTakePhotos(void)
{
    side_colors.clear();
    std::string temp_return ;
    std::string temp_motor_state;
    for(int i = 0;i<4;i++)
    {
  /********/
    //拍照1 2 3 4
    temp_return = My_imageprocess.get_side_color();
    if(temp_return == "no_cam")
        return "no_cam";
    if(temp_return == "photo_error")
        return "photo_error";
    side_colors += temp_return;
    /********/
    if(i == 3)
        break;
    Motor_O->ReciveRotationAngle(O_bar_up);      //弧形摆杆电机    //上扬
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_pro_move); //77     //T型杆分阶段下降
    delay(T_bar_time_o_bar);
    Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
    delay(O_bar_time_T_bar);
    Motor_T->ReciveRotationAngle(T_bar_down);
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
    delay(bar_swing_time);
    }

    temp_motor_state = SteppingMotor_Control(Clockwise,90,SM_Enable,StandartSpeed);  //顺时针转90度
    if(temp_motor_state == "locked_rotor")
        return "locked_rotor";

    Motor_O->ReciveRotationAngle(O_bar_up);   //弧形摆杆电机    //上扬
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_pro_move); //77     //T型杆分阶段下降
    delay(T_bar_time_o_bar);
    Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
    delay(O_bar_time_T_bar);
    Motor_T->ReciveRotationAngle(T_bar_down);
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
    delay(bar_swing_time);
  /********/
    //拍照5
    temp_return = My_imageprocess.get_side_color();
    if(temp_return == "no_cam")
        return "no_cam";
    if(temp_return == "photo_error")
        return "photo_error";
    side_colors += temp_return;


    /********/
    Motor_O->ReciveRotationAngle(O_bar_up);   //弧形摆杆电机    //上扬
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_pro_move); //77     //T型杆分阶段下降
    delay(T_bar_time_o_bar);
    Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
    delay(O_bar_time_T_bar);
    Motor_T->ReciveRotationAngle(T_bar_down);
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
    delay(T_bar_time_o_bar);  //分段动作，需先上扬
    Motor_O->ReciveRotationAngle(O_bar_up);      //弧形摆杆电机    //上扬
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_pro_move); //77     //T型杆分阶段下降
    delay(T_bar_time_o_bar);
    Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
    delay(O_bar_time_T_bar);
    Motor_T->ReciveRotationAngle(T_bar_down);
    delay(bar_swing_time);
    Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
    delay(bar_swing_time);
    /********/
    //拍照6
    temp_return = My_imageprocess.get_side_color();
    if(temp_return == "no_cam")
        return "no_cam";
    if(temp_return == "photo_error")
        return "photo_error";
    side_colors += temp_return;
    return side_colors;
}

std::string HardwareControl::get_all_threshold(void)
{
    std::string my_check_threshold;
    std::string temp_return ;
    std::string temp_motor_state;
    for(int i = 0;i<4;i++)
    {
  /********/
    //拍照1 2 3 4
    temp_return = My_imageprocess.init_color_threshold();
    if(temp_return == "no_cam")
        return "no_cam";
    if(temp_return == "photo_error")
        return "photo_error";
    my_check_threshold += temp_return;
    /********/
    if(i == 3)
        break;
    Motor_O->ReciveRotationAngle(O_bar_up);      //弧形摆杆电机    //上扬
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_pro_move); //77     //T型杆分阶段下降
    delay(T_bar_time_o_bar);
    Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
    delay(O_bar_time_T_bar);
    Motor_T->ReciveRotationAngle(T_bar_down);
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
    delay(bar_swing_time);
    }

    temp_motor_state = SteppingMotor_Control(Clockwise,90,SM_Enable,StandartSpeed);  //顺时针转90度
    if(temp_motor_state == "locked_rotor")
        return "locked_rotor";

    Motor_O->ReciveRotationAngle(O_bar_up);   //弧形摆杆电机    //上扬
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_pro_move); //77     //T型杆分阶段下降
    delay(T_bar_time_o_bar);
    Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
    delay(O_bar_time_T_bar);
    Motor_T->ReciveRotationAngle(T_bar_down);
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
    delay(bar_swing_time);

  /********/
    //拍照5
    temp_return = My_imageprocess.init_color_threshold();
    if(temp_return == "no_cam")
        return "no_cam";
    if(temp_return == "photo_error")
        return "photo_error";
    my_check_threshold += temp_return;
    /********/
    Motor_O->ReciveRotationAngle(O_bar_up);   //弧形摆杆电机    //上扬
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_pro_move); //77     //T型杆分阶段下降
    delay(T_bar_time_o_bar);
    Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
    delay(O_bar_time_T_bar);
    Motor_T->ReciveRotationAngle(T_bar_down);
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
    delay(T_bar_time_o_bar);  //分段动作，需先上扬
    Motor_O->ReciveRotationAngle(O_bar_up);      //弧形摆杆电机    //上扬
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_pro_move); //77     //T型杆分阶段下降
    delay(T_bar_time_o_bar);
    Motor_O->ReciveRotationAngle(O_bar_down);   //弧形摆杆电机
    delay(O_bar_time_T_bar);
    Motor_T->ReciveRotationAngle(T_bar_down);
    delay(bar_swing_time);

    Motor_T->ReciveRotationAngle(T_bar_up);    //T型摆杆       //上扬
    delay(bar_swing_time);


    /********/
    //拍照6
    temp_return = My_imageprocess.init_color_threshold();
    if(temp_return == "no_cam")
        return "no_cam";
    if(temp_return == "photo_error")
        return "photo_error";
    my_check_threshold = my_check_threshold+temp_return+"_";//_170_171_27_28_10_11_80_83_103_103_63_64_
    return my_check_threshold;
}

std::string HardwareControl::Step_run(std::vector<int> step)
{
        The_LAST_STEP = 0;
        The_NEXT_STEP = 0;
        std::string temp_state ;
        std::cout<< "STEP_SIZE"<<step.size()<<endl;
        for( int j = 0;j < step.size();j++)
        {
            if(j>0)
            {
                The_LAST_STEP = step[j-1];
            }
            else
                The_LAST_STEP = 0;
            if(j+1 < step.size())
            {
                The_NEXT_STEP = step[j+1];
            }
            else
                The_NEXT_STEP = 0;

            switch(step[j])
            {
            case 1:   //顺时针90
            {
                temp_state =TurnTheCube_degrees(Clockwise,90);
                if(temp_state == "locked_rotor")
                    return "locked_rotor";
                std::cout<< "顺时针90"<<std::endl;
                break;
            }
            case 2:   //顺时针180
            {
                temp_state =TurnTheCube_degrees(Clockwise,180);
                if(temp_state == "locked_rotor")
                    return "locked_rotor";
                std::cout<< "顺时针180"<<std::endl;
                break;
            }
            case 3:   //逆时针90
            {
                temp_state = TurnTheCube_degrees(Anticlockwise,90);
                if(temp_state == "locked_rotor")
                    return "locked_rotor";
                std::cout<< "逆时针90"<<std::endl;
                break;
            }
            case 4:   //整体顺时针90
            {
                temp_state = OverAllTurn(Clockwise,90)  ;      //整体转动rotation_angle度
                if(temp_state == "locked_rotor")
                    return "locked_rotor";
                std::cout<< "整体顺时针90"<<std::endl;
                break;
            }
            case 5:   //整体顺时针180
            {
                temp_state = OverAllTurn(Clockwise,180)  ;      //整体转动rotation_angle度
                if(temp_state == "locked_rotor")
                    return "locked_rotor";
                std::cout<< "整体顺时针180"<<std::endl;
                break;
            }
            case 6:   //整体逆时针90
            {
                temp_state = OverAllTurn(Anticlockwise,90)  ;
                //整体转动rotation_angle度
                if(temp_state == "locked_rotor")
                    return "locked_rotor";
                std::cout<< "整体逆时针90"<<std::endl;
                break;
            }
            case 7:   //上翻1
            {

                Cube_Flip(1) ;
                std::cout<< "上翻1"<<std::endl;
                break;
            }
            case 8:    //上翻2
            {

                Cube_Flip(2) ;
                std::cout<< "上翻2"<<std::endl;
                break;
            }
            default:
                break;
            }
        }
        return "run_step_ok";

}
