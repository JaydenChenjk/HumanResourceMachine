#include<iostream>
#include<fstream>
#include<vector>
#include<string>
using namespace std;


struct INSTRUCTIONS {
    string name;
    int number;
};
//全局变量设定：
bool user_levels[5]={0};//存储用户关卡通关信息
bool user_levels_lock[5] = {1};
ifstream fusers_in,flevel_in;//读取用户数据文件、关卡信息
ofstream fusers_out ;//更新电脑中的存档信息
bool faults = 0;//程序是否出错（也许可以记录一下是什么错？）
vector<INSTRUCTIONS> last_instructions;

vector<int> input;//输入
vector<int> target;//目标输出
vector<int> output;
vector<string> availables;//可用命令
vector<INSTRUCTIONS> instructions;
vector<int> spaces;//空间数
int instruction_count;
int current_block;
int index = 0;
bool succeed = 0 ,quit = 0;


int Load_menu()//载入目录数据
{
    string fusers;//用户数据文件
    cout<<"请输入文件路径"<<endl;
    getline(cin, fusers);

    fusers_in.open(fusers);//打开文件
    fusers_out.open(fusers);
    try 
    {
       if(!fusers_in)
        {
            cout<<"无法打开文件！请检查文件是否存在以及路径是否正确。"<<endl;
            return -1;
        }
        else
        {
            int lvname;//关卡号
            bool lv_is_pass;//此关卡之前是否通关
            while(fusers_in>>lvname>>lv_is_pass)
                user_levels[lvname-1]=lv_is_pass;//存储用户通关数据 用户数据格式：关卡号+是否通关
            
            for(int i = 1;i<5;i++)//用户可选关数据
            {
                if(user_levels[i-1])
                    user_levels_lock[i] = 1;
                else
                    user_levels_lock[i] = 0;
            }
            return 0;
        }
    }
    catch (...)//处理任何异常
    {
        cout << "Invalid file format." << endl;
        return -1;
    }
}

void Menu()//展示目录
{
    cout<<endl<<"Menu"<<endl;
    for(int i = 0;i<5;i++)
    {
        cout<<"level "<<i+1;
        if(!user_levels_lock[i])
            cout<<"------locked";
        else
        {
            if(user_levels[i])
                cout<<"------◉";
            else
                cout<<"------○";
        }
        cout<<endl;
    }
}

int Load_init_data(vector<string> &inputs)//以string形式存储所有数据
{
    string counts;//总共数据数（行数！所以input和output需要多行存储）
    int count;//转化为int
    getline(flevel_in, counts);//可能出错！用全局变量接着读可能会出错，后续排查！//需要先输入有几个值！
    count = stoi(counts);
    string in;
    for (int i = 0; i < count; i++) 
    {
        getline(flevel_in, in);//存储的时候一行存多个数值！
        inputs.push_back(in);
    }
    return count;
}

void Load_init_int(vector<int> &inputn)
{
    vector<string> inputs;
    int count;
    count = Load_init_data(inputs);
    inputn.resize(count);
    for(int i = 0;i<count;i++)
        inputn[i]=stoi(inputs[i]);
}

void load_instructions_from_keyboard() {
    try {
        cout << "Enter the number of instructions: ";
        cin >> instruction_count;
        cin.ignore();
        instructions.resize(instruction_count);

        cout << "Enter your instructions one by one (e.g., inbox, outbox):" << endl;
        for (int i = 0; i < instruction_count; i++) {
            string instruction;
            getline(cin, instruction);
            for(int j=0;j<instruction.length();j++) {
                if(instruction[j]==' ') {
                    instructions[i].number=instruction[j+1];
                    break;
                }
                instructions[i].name+=instruction[j];
            }
        }
    }
    catch (...) {
        cout << "Invalid input." << endl;
    }
}


void load_instructions_from_file(const string& file_path, vector<INSTRUCTIONS>instr) {
    int instruction_count,tmp;
    try {
        ifstream file(file_path);
        if (!file) {
            cout << "File not found: " << file_path << endl;
            return;
        }
        file>>tmp;
        if(file.eof())//可能用户第一次玩，没有数据
            return;
        
        file >> instruction_count;
        file.ignore();
        instr.resize(instruction_count);
        for (int i = 0; i < instruction_count; i++) {
            string instruction;
            getline(cin, instruction);
            for(int j=0;j<instruction.length();j++) {
                if(instruction[j]==' ') {
                   instr[i].number=instruction[j+1];
                    break;
                }
               instr[i].name+=instruction[j];
            }
        }
    }
    catch (...) {
        cout << "Invalid file format." << endl;
    }
}


int initial(/*vector<int> &input,vector<int> &target,vector<string> &availables*/)//初始化函数，关卡数据存储格式：输入函数、目标、可用指令；
{
    string levels;//关卡名
    string instr;//存储指令
    int itr_num,itr_pos = 0;//用户上一次输入的指令以及指令操作数
    
    cout<<"Please enter the number of the level you want to play.\nEnter Quit to quit the game"<<endl;
    cin>>levels;

    if(levels == "Quit")//退出游戏
    {
        quit = 1;
        return -1;
    }

    try {
        string path = "C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\level"+levels+".txt";
        flevel_in.open(path);//打开关卡存储文件
        if(!flevel_in)//判断能否打开
        {
            cout<<"Unable to find this level! Please check if there's any spelling mistake."<<endl;
            return -1;
        }

        else
        {
            int level = stoi(levels);//转化关卡号为数字 
            if(levels == "1")
                level = 1;
            
            if(level<1||level>5)//输入关卡号错误情况
            {
                cout<<"关卡号输入错误！请重新输入。";
                return -1;//如果为-1则一直在循环里！（do_while)  
            }
            else if(!user_levels_lock[level-1])
            {
                cout<<"This level is locked! You need to pass all the previous levels to unlock."<<endl;
                return -1;
            }

            else
            {
                int spnum = 3;//空间数
                string tmp;
                Load_init_int(input);
                getline(flevel_in, tmp);//中间有空行，所以消除一下
                Load_init_int(target);
                getline(flevel_in, tmp);//中间有空行，所以消除一下
                Load_init_data(availables);
                getline(flevel_in, tmp);
                flevel_in>>spnum;//获取空格数
                flevel_in>>tmp;

                const string last_inp = "C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\user_level"+levels+".txt";
                load_instructions_from_file(last_inp,last_instructions);//按行存储上一次的指令
                if(!flevel_in.eof())
                    cout<<"Please don't change this file!"<<endl;
                return spnum;
            }
        }
    }
    catch (...) {
        cout << "Invalid file format." << endl;
        return -1;
    }
}

void user_input_cons()
{
    string mode;
    cout << "Choose input mode (keyboard/file): ";
    cin >> mode;
    cin.ignore();
    if (mode == "keyboard")
        load_instructions_from_keyboard();
    else if (mode == "file") {
        string file_path;
        cout << "Enter the file path: ";
        cin >> file_path;
        load_instructions_from_file(file_path,instructions);
    }
    else {
        cout << "Invalid mode." << endl;
        return ;
    }
}

void inbox(int i) {
    if (index < input.size()) {
        current_block = input[index];
        index++;
    }
    else
        cout << "Error on instruction " << (i + 1) << endl;
}

void outbox(int i) {
    if (current_block != -1) {
        output.push_back(current_block);
        current_block = -1;
    }
    else
        cout << "Error on instruction " << (i + 1) << endl;
}



bool run1() {
    for (int i = 0; i < instructions.size(); i++) {
        cout << (i + 1) << " " << instructions[i].name;
        if(instructions[i].number!=-1)
            cout << " " << instructions[i].number;
        cout << endl;

        if (instructions[i].name == "inbox")
            inbox(i);
        else if (instructions[i].name == "outbox")
            outbox(i);
        else
            cout << "Error on instruction " << (i + 1) << endl;
    }

    if (output == target)
    {
        cout << "Success" << endl;
        return 1;
    }

    else
    {
        cout << "Fail" << endl;
        return 0;
    }

}

bool run2() {
    for (int i = 0; i < instructions.size(); i++) {
        cout << (i + 1) << " " << instructions[i].name;
        if(instructions[i].number!=-1)
            cout << " " << instructions[i].number;
        cout << endl;

        if (instructions[i].name == "inbox")
            inbox(i);
        else if (instructions[i].name == "outbox")
            outbox(i);
   //     else if (instructions[i].name == "add")
 //           add(i);
        else
            cout << "Error on instruction " << (i + 1) << endl;
    }

    if (output == target)
        cout << "Success" << endl;
    else
        cout << "Fail" << endl;
}






int main()
{
    //原本input、target、availables、spaces在这里定义
    int a;
    string retry;
    //初始化
    do
    {
        a =Load_menu();//加载目录
    } while (!(a+1));
    while(!quit)
    {
        Menu();//显示目录

        int spnum;//初始化;spnum:space number
        do{
            spnum = initial(/*input,target,availables*/);
        } while(!(spnum+1)&& !quit);
        

        if(quit)//退出游戏
            break;
        

        spaces.resize(spnum);

        //正式开始运行关卡
        
        while(!succeed)//游戏进行中
        {
            user_input_cons();
            succeed = 0;
            succeed = run1();
            if(succeed)
                break;
            
            else//失败重新尝试
            {
                cout<<"Enter R to retry."<<endl;
                cin>>retry;
                if(retry == "R")
                    continue;
                else
                    break;
            }

        }


    }
    

    return 0;
}
//之前：读取数据；
//输入关卡-》通过关卡号初始化待输入队列、目标输出、可用指令集-》转接进入关卡；
//之后：用文件形式保存数据，这个还没做
