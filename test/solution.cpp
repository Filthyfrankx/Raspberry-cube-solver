#include <solution.h>

using namespace std;
typedef vector<int> vi;

Solution::Solution(QObject *parent) : QObject(parent)
{

}

Solution::~Solution()
{

}

//string s = "";

int phase1;

//=====================================================标志每一层搜索后的魔方状态
//   //step //state //L_0_R_0  //x//y//z
//int book[40][2][3][3][2][3][2][3][2];

vi Solution::applyMove(int move, vi state) {

    int affectedCubies[][8] = {
        { 0,  1,  2,  3,  0,  1,  2,  3 },   // U
        { 4,  7,  6,  5,  4,  5,  6,  7 },   // D
        { 0,  9,  4,  8,  0,  3,  5,  4 },   // F
        { 2, 10,  6, 11,  2,  1,  7,  6 },   // B
        { 3, 11,  7,  9,  3,  2,  6,  5 },   // L
        { 1,  8,  5, 10,  1,  0,  4,  7 },   // R
    };

    int turns = move % 3 + 1;//角度
    int face = move / 3;//面取整
    while (turns--) {
        vi oldState = state;
        for (int i = 0; i < 8; i++) {
            int isCorner = i > 3;
            int target = affectedCubies[face][i] + isCorner * 12;
            int killer = affectedCubies[face][(i & 3) == 3 ? i - 3 : i + 1] + isCorner * 12;;
            int orientationDelta = (i < 4) ? (face > 1 && face < 4) : (face < 2) ? 0 : 2 - (i & 1);
            state[target] = oldState[killer];
            //state[target+20] = (oldState[killer+20] + orientationDelta) % (2 + isCorner);
            state[target + 20] = oldState[killer + 20] + orientationDelta;
            if (!turns)
                state[target + 20] %= 2 + isCorner;
        }
    }
    return state;
}

int Solution::inverse(int move) {
    return move + 2 - 2 * (move % 3);
}

vi Solution::id(vi state) {

    //--- Phase 1: Edge orientations.
    if (phase1 < 2)
        return vi(state.begin() + 20, state.begin() + 32);

    //-- Phase 2: Corner orientations, E slice edges.
    if (phase1 < 3) {
        vi result(state.begin() + 31, state.begin() + 40);
        for (int e = 0; e < 12; e++)
            result[0] |= (state[e] / 8) << e;
        return result;
    }

    //--- Phase 3: Edge slices M and S, corner tetrads, overall parity.
    if (phase1 < 4) {
        vi result(3);
        for (int e = 0; e < 12; e++)
            result[0] |= ((state[e] > 7) ? 2 : (state[e] & 1)) << (2 * e);
        for (int c = 0; c < 8; c++)
            result[1] |= ((state[c + 12] - 12) & 5) << (3 * c);
        for (int i = 12; i < 20; i++)
            for (int j = i + 1; j < 20; j++)
                result[2] ^= state[i] > state[j];
        return result;
    }

    //--- Phase 4: The rest.
    return state;
}

string Solution::solve(char Start_cube[20][4],string goal[20])
{
    int num=0;
    stop_solve_flag = 0;  //开始解算
    /*当前状态
    U黄色，D白色，L橙色，R红色，F蓝色，B绿色
    上前 上右 上后 上左
    下前 下右 下后 下左
    前右 前左
    后右 后左
    上前右 上右后 上后左 上左前 下右前 下前左 下左后 下后右
    1->顺时针90    2->顺时针180  3->逆时针90
    */

    //    char Start_cube[20][4] = { "RB", "BL", "FU", "DL", "UR", "FD", "FL", "LU", "RF", "UB", "DR", "BD",
    //        "LBD", "RUF", "BRD", "URB", "ULF", "DRF", "LUB", "DFL" };

//    char Start_cube[20][4] = { "UF", "UR", "FL", "UL", "BL", "DR", "DB", "BR", "DL", "UB", "FR", "DF",
//        "LUB", "UFR", "RBU", "BRD", "ULF", "DFL", "DLB", "FDR" };

    string s = "";
    int applicableMoves[] = { 0, 262143, 259263, 74943, 74898 };

    //cout<<"     current thread ID:"<<QThread::currentThreadId();

    cout << "魔方状态：";
    for (int i = 0; i < 20; i++) {
        cout << Start_cube[i];
    }
    cout << endl << "破解步骤:";

    //--- Define the goal.
    //string goal[] = { "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL",
    //	"UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR" };

    //--- Prepare current (start) and goal state.
    vi currentState(40), goalState(40);
    for (int i = 0; i < 20; i++) {

        //--- Goal state.
        goalState[i] = i;

        //--- Current (start) state.
        ///这是我改动的！！！
        //string cubie = argv[i+1];
        string cubie = Start_cube[i];

        while ((currentState[i] = find(goal, goal + 20, cubie) - goal) == 20) {
            cubie = cubie.substr(1) + cubie[0];
            currentState[i + 20]++;
            if (stop_solve_flag == 1)  //强制停止，直接返回
            {
                phase1 = 0;
                return s;  //不发送

            }
        }
    }

    //--- Dance the funky Thistlethwaite...
    while (++phase1 < 5) {
        if (stop_solve_flag == 1)  //强制停止，直接返回
        {
            phase1 = 0;
            return s;  //不发送
        }
        //--- Compute ids for current and goal state, skip phase if equal.
        vi currentId = id(currentState), goalId = id(goalState);
        if (currentId == goalId)
            continue;

        //--- Initialize the BFS queue.
        queue<vi> q;
        q.push(currentState);
        q.push(goalState);

        //--- Initialize the BFS tables.
        map<vi, vi> predecessor;
        map<vi, int> direction, lastMove;
        direction[currentId] = 1;
        direction[goalId] = 2;

        //--- Dance the funky bidirectional BFS...
        while (!stop_solve_flag) {   //0解算   1 停止解算
            //--- Get state from queue, compute its ID and get its direction.
            vi oldState = q.front();
            q.pop();
            vi oldId = id(oldState);
            int& oldDir = direction[oldId];

            //--- Apply all applicable moves to it and handle the new state.
            for (int move = 0; move < 18; move++) {
                if (stop_solve_flag == 1) //强制停止，直接返回
                {
                    phase1 = 0;
                    return s;  //不发送
                }
                if (applicableMoves[phase1] & (1 << move)) {

                    //--- Apply the move.
                    vi newState = applyMove(move, oldState);
                    vi newId = id(newState);
                    int& newDir = direction[newId];

                    //--- Have we seen this state (id) from the other direction already?
                    //--- I.e. have we found a connection?
                    if (newDir  &&  newDir != oldDir) {

                        //--- Make oldId represent the forwards and newId the backwards search state.
                        if (oldDir > 1) {
                            swap(newId, oldId);
                            move = inverse(move);
                        }

                        //--- Reconstruct the connecting algorithm.
                        vi algorithm(1, move);
                        while (oldId != currentId) {
                            if (stop_solve_flag == 1) //强制停止，直接返回
                            {
                                phase1 = 0;
                                return s;  //不发送
                            }
                            algorithm.insert(algorithm.begin(), lastMove[oldId]);
                            oldId = predecessor[oldId];
                        }
                        while (newId != goalId) {
                            if (stop_solve_flag == 1) //强制停止，直接返回
                            {
                                phase1 = 0;
                                return s;  //不发送
                            }
                            algorithm.push_back(inverse(lastMove[newId]));
                            newId = predecessor[newId];
                        }
                        //--- Print and apply the algorithm.
                        stringstream ss;
                        for (int i = 0; i < (int)algorithm.size(); i++) {
                            if (stop_solve_flag == 1) //强制停止，直接返回
                            {
                                phase1 = 0;
                                return s;  //不发送
                            }

                            ss.str("");
                            ss << (algorithm[i] % 3 + 1);
                            s = s + "UDFBLR"[algorithm[i] / 3] + ss.str() + " ";
                            currentState = applyMove(algorithm[i], currentState);
                        }
                        num++;
                        //cout << s << endl;
                        //--- Jump to the next phase.
                        goto nextPhasePlease;
                    }

                    //--- If we've never seen this state (id) before, visit it.
                    if (!newDir) {
                        q.push(newState);
                        newDir = oldDir;
                        lastMove[newId] = move;
                        predecessor[newId] = oldId;
                    }
                }
            }
        }
        if (stop_solve_flag == 1) //强制停止，直接返回
        {
            phase1 = 0;
            return s;  //不发送

        }
        nextPhasePlease:
        ;
    }
    if (stop_solve_flag == 1) //强制停止，直接返回
    {
        phase1 = 0;
        return s;  //不发送

    }
    cout << endl << "耗时间:" << endl;
    if(num >3)
    {
        phase1 = 0;
        return s;
    }
    else {
        phase1 = 0;
        return s;
    }

}

void Solution::work(void)//data_of_cube包括起始状态，终止状态，
{
    while(1)
    {

        if(start_solve_flag == 1)
        {
            int transformation_right = 0;
            string goal[] = { "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL",
                              "UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR" };
            transformation_right = Color_Sides_Transformation(start_str_temp);
            cout << "zhuan huan cheng gong: "<< transformation_right<<endl;
            if(transformation_right == 1)
            {

                stop_solve_flag == 0;
                outcome = solve(Start_cube, goal);
                if (stop_solve_flag != 1)  //若强制停止，不发信号
                {
                    cout << "outcome: "<< outcome<<endl;
                    start_solve_flag = 0;
                    solve_flag = 1;     //solve success
                    //emit caculatefinish();
                }
                else {
                    start_solve_flag = 0;
                    solve_flag = 0;
                }
            }
            else {
                start_solve_flag = 0;
                solve_flag = 0;
                //........
            }
        }
        delay(100);
    }


}
vector<int> Solution::Step_Transformation(void)
{
    string str = outcome;
    unsigned int len = str.length();
    int step_count = 0;
    cout<<"strlenth"<<len<<endl;
    vector<int> step;
    step.clear();

        for (unsigned i = 0; i < str.length();i = i + 3)  //²½Öè×ª»»
        { //str  中每个动作间有空格
            cout<<"i::"<<i<<endl;
            char ch[2] = {};
            ch[0] = str[i];     //需转动的面
            ch[1] = str[i+1];   //  1:顺时针  2顺时针180  3 逆90
            int t_ch = ch[1]-'0';// 1 yu 3 xiang fan
            if(t_ch==1)
                t_ch = 3;
            else if(t_ch==3)
                t_ch = 1;
            for (int j = 0; j < 6; j++)
            {
                if (ch[0] == cube_current_status[j])//找到魔方需转动面当前在什么位置
                {
                    switch (j)
                    {
                    case 0: //ÔÚÉÏÃæ
                    {
                        step.push_back(8);//上翻2
                        Status_Update(8);
                        step_count++;

                        step.push_back(t_ch); //转动底面
                        Status_Update(t_ch);
                        step_count++;
                        break;
                    }
                    case 1: //在下面
                    {
                        step.push_back(t_ch);//转动底面
                        Status_Update(t_ch);
                        step_count++;
                        break;
                    }
                    case 2://ÔÚ×óÃæ
                    {
                        step.push_back(4);
                        Status_Update(4);
                        step_count++;

                        step.push_back(7);
                        Status_Update(7);
                        step_count++;

                        step.push_back(t_ch);
                        Status_Update(t_ch);
                        step_count++;
                        break;
                    }
                    case 3://ÔÚÓÒÃæ
                    {
                        step.push_back(6);
                        Status_Update(6);
                        step_count++;

                        step.push_back(7);
                        Status_Update(7);
                        step_count++;

                        step.push_back(t_ch);
                        Status_Update(t_ch);
                        step_count++;
                        break;
                    }
                    case 4://ÔÚÇ°Ãæ
                    {
                        step.push_back(5);
                        Status_Update(5);
                        step_count++;

                        step.push_back(7);
                        Status_Update(7);
                        step_count++;

                        step.push_back(t_ch);
                        Status_Update(t_ch);
                        step_count++;
                        break;
                    }
                    case 5://ÔÚºóÃæ
                    {
                        step.push_back(7);
                        Status_Update(7);
                        step_count++;

                        step.push_back(t_ch);
                        Status_Update(t_ch);
                        step_count++;
                        break;
                    }
                    }
                    break;
                }
            }
        }
        outcome.clear();
        return step;
}
const char Start_cube_temp[20][4] = { "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL",
"UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR" };
void Solution::Status_Update(int actions_performed)//每执行一个动作更新一次魔方状态
  {
      char cube_status_temp[6];
      for (int i = 0; i < 6; i++) //将魔方当前状态备份，用于转换
      {
          cube_status_temp[i] = cube_current_status[i];
      }

      //   1 ;//顺时针90    2;//顺时针180    3;//逆时针90
      //   4;//整体顺时针90       5;//整体顺时针180         6;//整体逆时针90
      //    7;//上翻1     8;//上翻2

      //cube_current_statu中
      //0   1    2     3     4     5
      //上   下   左   右     前    后
      switch (actions_performed)
      {
      case 1: //顺时针90
      {
          //各面方位不变
          break;
      }
      case 2: //顺时针180
      {   //各面方位不变
          break;
      }
      case 3://逆时针90
      {   //各面方位不变
          break;
      }
      case 4://整体顺时针90
      {
          //上下面不变
          cube_current_status[0] = cube_status_temp[0];
          cube_current_status[1] = cube_status_temp[1];
          cube_current_status[2] = cube_status_temp[4];
          cube_current_status[3] = cube_status_temp[5];
          cube_current_status[4] = cube_status_temp[3];
          cube_current_status[5] = cube_status_temp[2];
          break;
      }
      case 5://整体顺时针180
      {
          //上下面不变
          cube_current_status[0] = cube_status_temp[0];
          cube_current_status[1] = cube_status_temp[1];
          cube_current_status[2] = cube_status_temp[3];
          cube_current_status[3] = cube_status_temp[2];
          cube_current_status[4] = cube_status_temp[5];
          cube_current_status[5] = cube_status_temp[4];
          break;
      }
      case 6://整体逆时针90
      {
          //上下面不变
          cube_current_status[0] = cube_status_temp[0];
          cube_current_status[1] = cube_status_temp[1];
          cube_current_status[2] = cube_status_temp[5];
          cube_current_status[3] = cube_status_temp[4];
          cube_current_status[4] = cube_status_temp[2];
          cube_current_status[5] = cube_status_temp[3];
          break;
      }

      case 7://上翻1
      {    //左右面不变
          cube_current_status[0] = cube_status_temp[4];
          cube_current_status[1] = cube_status_temp[5];
          cube_current_status[2] = cube_status_temp[2];
          cube_current_status[3] = cube_status_temp[3];
          cube_current_status[4] = cube_status_temp[1];
          cube_current_status[5] = cube_status_temp[0];
          break;
      }
      case 8://上翻2
      {
          //左右面不变
          cube_current_status[0] = cube_status_temp[1];
          cube_current_status[1] = cube_status_temp[0];
          cube_current_status[2] = cube_status_temp[2];
          cube_current_status[3] = cube_status_temp[3];
          cube_current_status[4] = cube_status_temp[5];
          cube_current_status[5] = cube_status_temp[4];
          break;
      }
      }
  }
int Solution::Color_Block_Storage(char (*jiaokuai)[4])
{
        char jiaokuai_temp[2][4] = { '\n' };
        int cmp_right_count = 0;
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                jiaokuai_temp[i][j] = jiaokuai[i][j];
            }
        }

       for (int i=0; i < 20; i++)
        {
          if(strlen(jiaokuai[0]) == 2)
            {
              for (int j=0; j < 2; j++)
              {
                  for (int n=0; n < 2; n++)
                  {
                      if (j != n)
                      {
                          jiaokuai[0][0] = jiaokuai_temp[0][j];
                          jiaokuai[0][1] = jiaokuai_temp[0][n];
                          jiaokuai[1][0] = jiaokuai_temp[1][j];
                          jiaokuai[1][1] = jiaokuai_temp[1][n];
                          if (strcmp(jiaokuai[0], Start_cube_temp[i]) == 0)
                          {
                              for(int z = 0 ;z<4;z++)
                              {
                                  Start_cube[i][z] = jiaokuai[1][z];
                              }
                              cmp_right_count++;
                          }

                      }
                  }
              }
            }
          if (strlen(jiaokuai[0]) == 3)
            {
              for (int j=0; j < 3; j++)
              {
                  for (int n=0; n < 3; n++)
                  {
                      for (int m=0; m < 3; m++)
                      {
                          if (j != n && j != m && m != n)
                          {
                              jiaokuai[0][0] = jiaokuai_temp[0][j];
                              jiaokuai[0][1] = jiaokuai_temp[0][n];
                              jiaokuai[0][2] = jiaokuai_temp[0][m];
                              jiaokuai[1][0] = jiaokuai_temp[1][j];
                              jiaokuai[1][1] = jiaokuai_temp[1][n];
                              jiaokuai[1][2] = jiaokuai_temp[1][m];
                              if (strcmp(jiaokuai[0], Start_cube_temp[i]) == 0)//用中心块与Start_cube对应，找到位置(存在前后翻转)
                              {
                                  for(int z= 0 ;z<4;z++)
                                  {
                                      Start_cube[i][z] = jiaokuai[1][z];
                                  }
                                  cmp_right_count++;
                              }
                          }
                      }
                  }
              }
            }

        }
       if(cmp_right_count == 1)
           return  1;
       else {
           cout << "cmp_count: "<< cmp_right_count<<endl;
           return 0;
       }
}
int  Solution::Color_Sides_Transformation(string s)
{
    char color_sides[6][9];
    int cmp_right_count = 0;
    for(int i =0;i<6;i++)
    {
        for(int j =0;j<9;j++)
        {
            color_sides[i][j] = s.at(9 * i+j);
        }
    }
    cube_current_status[0] = color_sides[5][4];
    cube_current_status[1] = color_sides[4][4];
    cube_current_status[2] = color_sides[0][4];
    cube_current_status[3] = color_sides[2][4];
    cube_current_status[4] = color_sides[3][4];
    cube_current_status[5] = color_sides[1][4];
    //
    std::cout<<cube_current_status[0]<<cube_current_status[1]<<cube_current_status[2]
            <<cube_current_status[3]<<cube_current_status[4]<<cube_current_status[5]
            <<endl;

    char jiaokuai[2][4] = {'\n'};

    jiaokuai[0][0] = color_sides[0][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[3][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[0][1];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[3][7];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[0][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[4][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[0][5];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[4][3];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[0][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[1][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[0][7];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[1][1];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[0][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[5][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[0][3];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[5][3];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    ///

    jiaokuai[0][0] = color_sides[2][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[4][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[2][5];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[4][5];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[2][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[3][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[2][7];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[3][1];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[2][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[5][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[2][3];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[5][5];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[2][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[1][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[2][1];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[1][7];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    /////
    jiaokuai[0][0] = color_sides[1][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[5][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[1][3];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[5][1];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[1][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[4][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[1][5];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[4][7];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[3][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[4][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[3][5];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[4][1];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[3][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[5][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[3][3];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[5][7];//
    cmp_right_count += Color_Block_Storage(jiaokuai);
    /////

    jiaokuai[0][0] = color_sides[0][4];//0ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[3][4];//3ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][2] = color_sides[5][4];//5ÃæÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[0][0];//0Ãæ0¿éÑÕÉ«
    jiaokuai[1][1] = color_sides[3][6];//
    jiaokuai[1][2] = color_sides[5][6];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[0][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[3][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][2] = color_sides[4][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[0][2];//
    jiaokuai[1][1] = color_sides[3][8];//
    jiaokuai[1][2] = color_sides[4][0];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[0][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[4][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][2] = color_sides[1][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[0][8];//
    jiaokuai[1][1] = color_sides[4][6];//
    jiaokuai[1][2] = color_sides[1][2];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[0][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[5][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][2] = color_sides[1][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[0][6];//
    jiaokuai[1][1] = color_sides[5][0];//
    jiaokuai[1][2] = color_sides[1][0];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[2][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[5][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][2] = color_sides[1][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[2][0];//
    jiaokuai[1][1] = color_sides[5][2];//
    jiaokuai[1][2] = color_sides[1][6];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[2][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[1][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][2] = color_sides[4][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[2][2];//
    jiaokuai[1][1] = color_sides[1][8];//
    jiaokuai[1][2] = color_sides[4][8];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[2][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[4][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][2] = color_sides[3][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[2][8];//
    jiaokuai[1][1] = color_sides[4][2];//
    jiaokuai[1][2] = color_sides[3][2];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    jiaokuai[0][0] = color_sides[2][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][1] = color_sides[5][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[0][2] = color_sides[3][4];//ÖÐÐÄ¿éÑÕÉ«
    jiaokuai[1][0] = color_sides[2][6];//
    jiaokuai[1][1] = color_sides[5][8];//
    jiaokuai[1][2] = color_sides[3][0];//
    cmp_right_count += Color_Block_Storage(jiaokuai);

    if(cmp_right_count == 20)
        return 1;
    else return 0;
}

void Solution::stop_solve()//data_of_cube包括起始状态，终止状态，
{

    start_solve_flag = 0;
    solve_flag = 0;
    stop_solve_flag = 1;//解算结束
}
void Solution::start_solve(string start_str)
{

    start_str_temp = start_str;
    solve_flag = 0;
    stop_solve_flag = 0;
    start_solve_flag = 1;

}

