#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H
#include <iostream>
#include <QObject>
#include <wiringPi.h>
//led
#define R_led_pin  23
#define G_led_pin  24
#define B_led_pin  25
#define led_on 0
#define led_off 1
#define run_normal_num '0'   //B_on
#define run_photo_num '1'   //B_on G_ ON_OFF
#define run_solve_num '2'   //B_on ON_OFF G_ ON_OFF
#define worning_no_cam_num '3'   //B_OFF G_ ON_OFF
#define err_locked_rotor_num '4'   //R_on G_ ON_OFF
#define err_photo_num '5'   //R_ ON_OFF G_ ON_OFF
#define err_solve_num '6'   //R_on_OFF G_ ON
#define err_system_num '7'   // B_onOFF G_onOFF R_ ONOFF
#define threshold_init '8'   //流水灯
class machine_state: public QObject
{
public:
    machine_state(QObject *parent = nullptr);
    void machine_state_show();
    void set_machine_state(std::string state_str);
private:
    char led_time = 100;
    char machine_state_num = run_normal_num;
public slots:

    /*
0
1
2
3
4
5
    */
};

#endif // MACHINE_STATE_H
