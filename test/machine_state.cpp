#include "machine_state.h"

machine_state::machine_state(QObject *parent) : QObject(parent)
{
    pinMode(R_led_pin,OUTPUT);
    digitalWrite(R_led_pin,led_off);
    pinMode(G_led_pin,OUTPUT);
    digitalWrite(G_led_pin,led_off);
    pinMode(B_led_pin,OUTPUT);
    digitalWrite(B_led_pin,led_on);
}
void machine_state::machine_state_show()
{
    while(1)
    {
        delay(led_time);
        switch(machine_state_num)
        {
        case run_normal_num:
            digitalWrite(B_led_pin,led_on);
            digitalWrite(G_led_pin,led_off);
            digitalWrite(R_led_pin,led_off);
            break;
        case run_photo_num:
            digitalWrite(B_led_pin,led_on);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            digitalWrite(B_led_pin,led_on);
            digitalWrite(G_led_pin,led_off);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            break;
        case run_solve_num:
            digitalWrite(B_led_pin,led_on);
            digitalWrite(G_led_pin,led_off);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            break;
        case worning_no_cam_num:
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_off);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            break;
        case err_locked_rotor_num:
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_on);
            delay(led_time);
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_off);
            digitalWrite(R_led_pin,led_on);
            delay(led_time);
            break;
        case err_photo_num:
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_on);
            delay(led_time);
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_off);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            break;
        case err_solve_num:
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_on);
            delay(led_time);
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            break;
        case err_system_num:
            digitalWrite(B_led_pin,led_on);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_on);
            delay(led_time);
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_off);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            break;
        case threshold_init:
            digitalWrite(B_led_pin,led_on);
            digitalWrite(G_led_pin,led_off);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            digitalWrite(B_led_pin,led_on);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            digitalWrite(B_led_pin,led_on);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_on);
            delay(led_time);
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_on);
            digitalWrite(R_led_pin,led_on);
            delay(led_time);
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_off);
            digitalWrite(R_led_pin,led_on);
            delay(led_time);
            digitalWrite(B_led_pin,led_off);
            digitalWrite(G_led_pin,led_off);
            digitalWrite(R_led_pin,led_off);
            delay(led_time);
            break;
        default:
            break;
        }
    }

}
void machine_state::set_machine_state(std::string state_str)
{
    if(state_str == "run_normal")
        machine_state_num = run_normal_num;
    if(state_str == "FlipOverAndTakePhotos")
        machine_state_num = run_photo_num;
    else if(state_str == "start_solve")
            machine_state_num = run_solve_num;
    else if(state_str == "no_cam")
        machine_state_num = worning_no_cam_num;
    else if(state_str == "locked_rotor")
        machine_state_num = err_locked_rotor_num;
    else if(state_str == "photo_error")
        machine_state_num = err_photo_num;
    else if(state_str == "solve_error")
        machine_state_num = err_solve_num;
    else if(state_str == "system_error")
        machine_state_num = err_system_num;

    std::cout << "state_num" << machine_state_num << std::endl;
}
