#include "mg955.h"
#include <QDebug>
#include <wiringPi.h>
MG955::MG955(unsigned char pin,QObject *parent) : QObject(parent)
{
 MotorPin = pin;
 pinMode(MotorPin,OUTPUT);
 digitalWrite(MotorPin,LOW);
}

void MG955::ReciveRotationAngle(unsigned int RA)
{
  if(RA == 180)
    MG955_able_flag = MG955_Disable ;  //MG955_Disable
  else
  {
    RotationAngle = RA;
    MG955_able_flag = MG955_Enable;
    //qDebug()<<"ReciveRotationAngle"<<MotorPin;
  }
}
void MG955::MG955Work()
{
    unsigned int compare = 0;

    while(1)
    {

        while(MG955_able_flag == MG955_Enable)
        {
            compare =20000- ((RotationAngle*100/9) +500);
        // 0.5ms   0度 ;  1.5ms   90度  一个脉冲周期20ms对应计数次数20000
            digitalWrite(MotorPin,LOW);
            delayMicroseconds(compare);
            digitalWrite(MotorPin,HIGH);
            delayMicroseconds(CYCLE_TIME-compare);

        }

        digitalWrite(MotorPin,HIGH);

        while(MG955_able_flag == MG955_Disable)
        {
            delay(100);
            //qDebug()<<"MG955_Disable"<<MotorPin;
        }

    }
}
