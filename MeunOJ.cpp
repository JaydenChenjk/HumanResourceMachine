#include<iostream>
#include<fstream>
#include<vector>
#include<string>
using namespace std;

int initial(int level, vector<int> &input,vector<int> &target,vector<string> &availables)//初始化函数
{
    string avai[] = {"inbox","outbox","copyfrom","copyto","add","sub","jump","jumpifzero"};
    if(level<1||level>3)//输入关卡错误情况
        return -1;
    else if(level == 1)
    {
        int inp[] = {1,2};
        input.insert(input.begin(),inp,inp+2);
        target.insert(target.begin(),inp,inp+2);
        availables.insert(availables.begin(),avai,avai+2);
        return 0;
    }
    else if(level == 2)
    {
        int inp[] ={3,9,5,1,-2,-2,9,-9};
        int oup[] ={-6,6,4,-4,0,0,18,-18};
        input.insert(input.begin(),inp,inp+8);
        target.insert(target.begin(),oup,oup+8);
        availables.insert(availables.begin(),avai,avai+8);
        return 3;
    }
    else if(level == 3)
    {
        int inp[] ={6,2,7,7,-9,3,-3,-3};
        int oup[] ={7,-3};
        input.insert(input.begin(),inp,inp+8);
        target.insert(target.begin(),inp,inp+2);
        availables.insert(availables.begin(),avai,avai+8);
        return 3;
    }
    return 0;
}

int main()
{
    vector<int> input;//输入
    vector<int> target;//目标输出
    vector<string> available;//可用命令
    vector<int> spaces;

//初始化
    int level;
    cin>>level;//关卡序号
    const int spn = initial(level,input,target,available);//初始化;spn:space number
    spaces.resize(3);
    return 0;


}
//之前：读取数据；
//输入关卡-》通过关卡号初始化待输入队列、目标输出、可用指令集-》转接进入关卡；
//之后：用文件形式保存数据
