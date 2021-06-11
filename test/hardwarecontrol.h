#ifndef HARDWARECONTROL_H //if not define
#define HARDWARECONTROL_H
#include <wiringPi.h>
#include <QObject>
#include <QThread>
#include "mg955.h"
#include "imageprocess.h"
#include <string>
//MG955
#define T_pin 2
#define O_pin 3
#define Angle_disable 180 //禁止舵机旋转超过180
#define T_bar_up 123//130
#define T_bar_down 30//30   //33
#define T_bar_pro_move 75
#define O_bar_up 130
#define O_bar_down 28//30
#define bar_swing_time 350//350//300
#define T_bar_time_o_bar 160//160//130
#define O_bar_time_T_bar 110//110//80
//SteppingMotor
#define SM_en_pin 12
#define SM_dir_pin  13
#define SM_pul_pin 14
#define StandartSpeed 80
#define PositiongResetSpeed 95
#define SM_Enable 0
#define SM_Disable 1
#define Clockwise  1 //顺时针
#define Anticlockwise 0//逆时针
//photo electric light
#define PE_pin 22
#define NoShelter  0
#define Shelter   1
//buttun
#define But_pin 21
#define but_up 0
#define but_down 1
//led
#define R_led_pin  23
#define G_led_pin  24
#define B_led_pin  25
#define led_on 0
#define led_off 1
class HardwareControl:public QObject
{
    Q_OBJECT
public:
    explicit HardwareControl(QObject *parent = nullptr);
    std::string SteppingMotor_Control(unsigned char dir,unsigned int rotation_angle,unsigned char en,unsigned char speed);
    void PositiongReset(void);
    std::string TurnTheCube_degrees(unsigned char dir,unsigned int rotation_angle );//扭转魔方底层90度
    void Cube_Flip(unsigned char count )  ; //魔方上翻 count 次
    std::string OverAllTurn(unsigned char dir,unsigned int rotation_angle ) ;       //整体转动rotation_angle度
    std::string FlipOverAndTakePhotos(void)   ;//翻转魔方依次拍照6面
    std::string Step_run(std::vector<int> step);
    std::string get_all_threshold(void);
    MG955 *Motor_T;
    MG955 *Motor_O;
    ImageProcess My_imageprocess;
    QThread Motor_T_Thread;
    QThread Motor_O_Thread;
    unsigned char The_LAST_STEP = 0;
    unsigned char The_NEXT_STEP = 0;   //存储上下步骤，用于优化步骤
private:
    void SteppingMotor_init();

    std::string side_colors;

/*
 * '1':   //顺时针90
 * '2':   //顺时针180
 * '3':   //逆时针90
 * '4':   //整体顺时针90
 * '5':   //整体顺时针180
 * '6':   //整体逆时针90
 * '7':   //上翻1
 * '8':    //上翻2
 */

    };

#endif // HARDWARECONTROL_H
