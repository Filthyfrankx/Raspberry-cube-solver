#ifndef MG955_H
#define MG955_H

#include <QObject>
#define    CYCLE_TIME     20000
#define  MG955_Disable 0
#define  MG955_Enable 1
class MG955 : public QObject
{
    Q_OBJECT
public:
    explicit MG955(unsigned char pin,QObject *parent = nullptr);
private:
    unsigned char MotorPin= 0;
    unsigned int  RotationAngle = 0;
    char MG955_able_flag = MG955_Disable;
signals:

public slots:
    void ReciveRotationAngle(unsigned int RA);
    void MG955Work();
};

#endif // MG955_H
