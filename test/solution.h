#ifndef SOLUTION_H
#define SOLUTION_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <queue>
#include <algorithm>
#include <sstream>
#include <QObject>


#include <QVariant>


#include <cstring>
#include <wiringPi.h>
using namespace std;
typedef vector<int> vi;

typedef struct CUBEDATA
{
    char start_cube[20][4];
    string goal_cube[20];

}Cubedta;

Q_DECLARE_METATYPE(CUBEDATA)
//char Start_cube[20][4];
//char Start_cube[];


class Solution : public QObject
{
    Q_OBJECT
public:
    explicit Solution(QObject *parent = nullptr);
    ~Solution();
    //string solve(char Start_cube[20][4]);//解魔方算法
    //char cube[20][4]={ "RB", "BL", "FU", "DL", "UR", "FD", "FL", "LU", "RF", "UB", "DR", "BD",
    //                                  "LBD", "RUF", "BRD", "URB", "ULF", "DRF", "LUB", "DFL" };
    char cube[20][4]; //设置的状态
    bool solve_flag = 0; //  0 wei jie suan cheng gong   //1jiesuanchenggong
    string outcome; //解算结果（线程里的）

    void work(void); //ThreadWork

    vector<int> Step_Transformation(void);//转化为机械步骤 输入str 输出结果直接在step里
    int Color_Block_Storage(char (*jiaokuai)[4]); //1 ok  0 error
    int Color_Sides_Transformation(string s); //把颜色填充进状态，结果直接存在全局变量Start_cube里

private:
    vi id(vi state);
    vi applyMove(int move, vi state);
    int inverse(int move);
    void Status_Update(int actions_performed);
    int stop_solve_flag = 0;  //0,不结束解算  ，1结束解算
    int start_solve_flag = 0;  //0,不解算  ，1开始解算
    char cube_current_status[6];
    char Start_cube[20][4];
    string start_str_temp;
public slots:
    string solve(char Start_cube[20][4], string goal[20]);//解魔方算法
    void start_solve(string start_str);
    void stop_solve();
signals:
    void caculatefinish(); //计算完成
};
#endif // 1_H
