#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include <climits>
#include <sstream>
#include <cmath>
#include <easy2d/easy2d.h>

using namespace easy2d;
using namespace std;


struct INSTRUCTIONS {
    string name;//否则后面初始化不好初始化
    int number;
    bool is_num_int = 1;
};
//全局变量设定：
bool user_levels[5] = { 1 };//存储用户关卡通关信息，关卡号就是下标！
bool user_levels_lock[5] = { 1 };//为了方便，从1开始记录！
ifstream fusers_in, flevel_in;//读取用户数据文件、关卡信息
ofstream fusers_out, fusers_lstins;//更新电脑中的存档信息
bool faults = 0;//程序是否出错（也许可以记录一下是什么错？）
vector<INSTRUCTIONS> last_instructions;

vector<int> input;//输入
vector<int> target;//目标输出
vector<int> output;
vector<string> availables;//可用命令
vector<INSTRUCTIONS> instructions;
vector<int> space;//空间数
string fusers;//用户数据文件
string levels;//关卡名
int instruction_count = 0;
int current_block = INT_MIN;
int level;
int spacenumber = 3;//空间数
bool succeed = 0, quit = 0;
bool error_flag = false;
bool inbox_done = false;
int position = 0;
vector<string> instruction;

//GUI
Node* brick[8];
Node* out_brick[20];/////
Sprite* square[8];
Sprite* out_square[20];/////
Text* number[8];
Text* out_num[20];/////
vector<Sequence*> bricks_ani;//1到8是输入输出的block
vector<Sequence*> out_bricks_ani;
int destination_count = 0;
int space_brick[3];
auto pointer0 = new Sprite();

int Load_menu()//载入目录数据
{
    cout << "请输入文件路径" << endl;
    getline(cin, fusers);
    fusers_in.open(fusers);//打开文件
    try
    {
        if (!fusers_in)
        {
            cout << "无法打开文件！请检查文件是否存在以及路径是否正确。" << endl;
            return -1;
        }
        else
        {
            int lvname;//关卡号
            bool lv_is_pass;//此关卡之前是否通关
            while (fusers_in >> lvname >> lv_is_pass)
                user_levels[lvname] = lv_is_pass;//存储用户通关数据 用户数据格式：关卡号+是否通关

            for (int i = 1;i < 4;i++)//用户可选关数据，关卡号就是下标
            {
                if (user_levels[i - 1])
                    user_levels_lock[i] = 1;
                else
                    user_levels_lock[i] = 0;
            }
            fusers_in.close();
            return 0;
        }
    }
    catch (...)//处理任何异常
    {
        cout << "Invalid file format." << endl;
        fusers_in.close();
        return -1;
    }

}

void Menu()//展示目录
{
    cout << endl << "Menu" << endl;
    for (int i = 1;i < 4;i++)
    {
        cout << "level " << i;
        if (!user_levels_lock[i])
            cout << "------locked";
        else
        {
            if (user_levels[i])
                cout << "------ ●";
            else
                cout << "------ ○ ";
        }
        cout << endl;
    }
}

int Load_init_data(vector<string>& inputs)//以string形式存储所有数据
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

void Load_init_int(vector<int>& inputn)
{
    vector<string> inputs;
    int count;
    count = Load_init_data(inputs);
    inputn.resize(count);
    for (int i = 0;i < count;i++)
        inputn[i] = stoi(inputs[i]);
}



void load_instructions_from_keyboard() {
    try {
        //cin >> level;
        cin >> instruction_count;
        if (instruction_count == 0)
        {
            cout << "Invalid Input!" << endl;
            return;
        }
        cin.ignore();
        instructions.resize(instruction_count);
        instruction.resize(instruction_count);
        for (int i = 0; i < instruction_count; i++) {
            
            getline(cin, instruction[i]);

            stringstream ss(instruction[i]);
            string name;
            double number = INT_MIN;

            ss >> name;
            if (ss >> number) {
                if (name != "inbox" && name != "outbox") {
                    instructions[i].number = int(number);
                    if (floor(number) != number || floor(number) < 0)
                        instructions[i].is_num_int = 0;
                }
                else
                    instructions[i].is_num_int = 0;//指令数不符
            }
            else
            {
                if (name != "inbox" && name != "outbox")
                    instructions[i].is_num_int = 0;//指令数过少
            }

            if (ss >> number) {
                instructions[i].is_num_int = 0;//指令数过多
            }

            instructions[i].name = name;
        }
    }
    catch (...) {
        cout << "Invalid input." << endl;
    }
}


void load_instructions_from_file(const string& file_path, vector<INSTRUCTIONS>instr) {
    int tmp;
    try {
        ifstream file(file_path);
        if (!file) {
            cout << "File not found: " << file_path << endl;
            return;
        }
        file >> tmp;
        if (file.eof())//可能用户第一次玩，没有数据
            return;

        instruction_count = 0;//加一个归零防止崩溃
        file >> instruction_count;
        file.ignore();
        instructions.resize(instruction_count);
        instruction.resize(instruction_count);
        for (int i = 0; i < instruction_count; i++) {

            getline(cin, instruction[i]);

            stringstream ss(instruction[i]);
            string name;
            double number = INT_MIN;

            ss >> name;
            if (ss >> number) {
                if (name != "inbox" && name != "outbox") {
                    instructions[i].number = int(number);
                    if (floor(number) != number || floor(number) < 0)
                        instructions[i].is_num_int = 0;
                }
                else
                    instructions[i].is_num_int = 0;//指令数不符
            }
            else
            {
                if (name != "inbox" && name != "outbox")
                    instructions[i].is_num_int = 0;//指令数过少
            }

            if (ss >> number) {
                instructions[i].is_num_int = 0;//指令数过多
            }

            instructions[i].name = name;
            file.close();
        }
    }
    catch (...) {
        cout << "Invalid file format." << endl;
    }
}

int initial(/*vector<int> &input,vector<int> &target,vector<string> &availables*/)//初始化函数，关卡数据存储格式：输入函数、目标、可用指令；
{
    string instr;//存储指令

    cout << "Please enter the number of the level you want to play.\nEnter Quit to quit the game" << endl;
    cin >> levels;

    if (levels == "Quit")//退出游戏
    {
        quit = 1;
        return -1;
    }

    try {
        string path = "C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\text\\level"+levels+".txt";
        //flevel_in.open("C:\\Users\\jayde\\source\\repos\\easy2d\\level0.txt");
        //flevel_in.close();
        flevel_in.open(path);//打开关卡存储文件
        if (!flevel_in)//判断能否打开;但如果打开路径和原本的路径一样，没有“打开”操作，所以会认为“打开失败”
        {
            cout << "Unable to find this level! Please check if there's any spelling mistake." << endl;
            return -1;
        }

        else
        {
            level = stoi(levels);//转化关卡号为数字
            if (levels == "1")
                level = 1;

            if (level < 1 || level>5)//输入关卡号错误情况
            {
                cout << "关卡号输入错误！请重新输入。";
                return -1;//如果为-1则一直在循环里！（do_while)
            }
            else if (!user_levels_lock[level])
            {
                cout << "This level is locked! You need to pass all the previous levels to unlock." << endl;
                flevel_in.close();
                return -1;
            }

            else
            {
                string tmp;
                Load_init_int(input);
                getline(flevel_in, tmp);//中间有空行，所以消除一下

                Load_init_int(target);
                getline(flevel_in, tmp);//中间有空行，所以消除一下

                Load_init_data(availables);
                getline(flevel_in, tmp);

                flevel_in >> spacenumber;//获取空格数
                flevel_in >> tmp;

                if (!flevel_in.eof())
                    cout << "Please don't change this file!" << endl;
                flevel_in.close();
                return spacenumber;
            }
        }

    }
    catch (...) {
        cout << "Invalid file format." << endl;
        flevel_in.close();
        return -1;
    }

}

void show_init()
{
    cout << "\ninputs:" << endl;
    for (int i = 0;i < input.size();i++)
    {
        cout << input[i] << " ";
    }
    cout << "\ntargets\n";
    for (int i = 0;i < target.size();i++)
    {
        cout << target[i] << " ";
    }
    cout << "\navailables:" << endl;
    for (int i = 0;i < availables.size();i++)
    {
        cout << availables[i] << " ";
    }
    cout << "\navailable spaces:" << spacenumber << endl;

    cout << "Last instructions:" << endl;//此时还是记录上一次退出时的ins数量！
    /*if (instruction_count == 0)
        cout << "None" << endl;
    else {
        for (int i = 0;i < instruction_count;i++)
        {
            cout << last_instructions[i].name << " " << last_instructions[i].number << endl;
        }
    }*/
}

void upload_data(string lvplayed)//每一次退出关卡都运行一遍！
{
    fusers_out.open(fusers);//这个就是新建文件了，在打开的一瞬间就会覆盖（？）
    try
    {
        if (!fusers_out)
        {
            cout << "无法打开文件！请检查文件是否存在以及路径是否正确。" << endl;
        }
        else
        {
            int lvname = 1;//关卡号
            for (;lvname < 4;lvname++)
                fusers_out << lvname << " " << user_levels[lvname] << endl;
            //存储用户通关数据 用户数据格式：关卡号+ +是否通关

            fusers_lstins.open("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\text\\user_level" + lvplayed + ".txt");
            fusers_lstins << instruction_count << endl;
            for (int i = 0; i < instruction_count; i++) //记录指令
                fusers_lstins << instructions[i].name << " " << instructions[i].number << endl;
            fusers_lstins.close();
        }
    }
    catch (...)//处理任何异常
    {
        cout << "Invalid file format." << endl;
    }
    fusers_out.close();

}

void check()
{
    if (output == target)
        cout << "Success" << endl;
    else
        cout << "Fail" << endl;
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
        load_instructions_from_file(file_path, instructions);
    }
    else {
        cout << "Invalid mode." << endl;
        return;
    }
}


void prepare_inbricks(int count)
{
    for (int i = 0; i < count; i++)
    {
        auto seq = new Sequence;
        bricks_ani.push_back(seq);//新的outbox
    }
    return;
}

void inbox(int i, easy2d::Sprite* robot, Sequence* rb_se, Text* rb_text, Delay* delay) {
    if (position < input.size()) {
        current_block = input[position];

        //GUI        
        auto moveBy_inbox = gcnew MoveBy(2.5f, Vector2(150,0));
        string s = std::to_string(current_block);
        int pos = position;
        auto changeText = gcnew CallFunc([rb_text,s]() {rb_text->setText(s); });
        auto block_vis = gcnew CallFunc([pos]() {brick[pos]->setVisible(false); });//注意position的调用？
        auto inbox_act = gcnew Spawn({ moveBy_inbox, changeText});
        rb_se->add(inbox_act->clone());
        //bricks_ani[position]->add(moveBy_inbox);
        //brick[position]->setVisible(false);
       
        bricks_ani[position]->add(block_vis->clone());
        auto moveBy = gcnew MoveBy(2.5f, Vector2(0, -40));
        for (int j = position+1;j < input.size();j++) {
            bricks_ani[j]->add(moveBy->clone());//保留传送带动画
            //可以加一个暂停动画

        }
        if (destination_count != 0) {//传送带动画
            for (int j = 0; j < destination_count; j++) {
                out_bricks_ani[j]->add(delay->clone());
                //可以加一个暂停动画
            }
        }

        position++;      
    }
    else {
        inbox_done = true;
    }
}

void outbox(int i, easy2d::Sprite* robot, Sequence* rb_se, Text* rb_text, CallFunc* create_out, Delay* delay) {
    if (current_block != INT_MIN) {
        output.push_back(current_block);
        current_block = INT_MIN;

        //GUI
        if (destination_count != 0) {//传送带动画
            auto moveBy = gcnew MoveBy(2.5f, Vector2(0, 40));
            for (int j = 0;j < destination_count;j++) {
                out_bricks_ani[j]->add(moveBy->clone());
                //可以加一个暂停动画
            }
        }

        
        auto moveBy_outbox = gcnew MoveBy(2.5f, Vector2(-150, 0));
        auto changeText = gcnew CallFunc([rb_text]() {rb_text->setText(" "); });        //更改数字动画
        
        auto outbox_act = gcnew Spawn({ moveBy_outbox, changeText, create_out});
        rb_se->add(outbox_act->clone());

        auto seq = new Sequence;
        out_bricks_ani.push_back(seq);//新的outbox

        //bricks_ani[position]->add(moveBy_outbox);（物块动画，现已删除）
        for (int j = position; j < input.size(); j++) {
            bricks_ani[j]->add(delay->clone());//保留传送带动画
            //可以加一个暂停动画
        }


    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void add(int i, int x, easy2d::Sprite* robot) {
    if ((space[x] != INT_MIN) && (x >= 0) && (x < spacenumber)) {
        current_block += space[x];

        //GUI
        string CB = to_string(current_block);
        number[position]->setText(CB);
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void sub(int i, int x, easy2d::Sprite* robot) {
    if ((space[x] != INT_MIN) && (x >= 0) && (x < spacenumber)) {
        current_block -= space[x];

        //GUI
        string CB = to_string(current_block);
        number[position]->setText(CB);
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void copyto(int i, int x, easy2d::Sprite* robot) {
    if (current_block != INT_MIN && x < spacenumber && x >= 0) {
        space[x] = current_block;

        //GUI
        auto moveTo = gcnew MoveTo(2.5f, Point(300+50*x, 400));
        //brick[position]->runAction(moveTo);（物块动画，现已删除）
        robot->runAction(moveTo->clone());
        space_brick[x] = position;//这句话啥意思
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void copyfrom(int i, int x, easy2d::Sprite* robot){
    if (space[x] != INT_MIN && x < spacenumber && x >= 0) {
        current_block = space[x];
    
        //GUI
        
        auto moveFrom = gcnew MoveTo(2.5f, Point(300, 200));
        int ss=space_brick[x];
        //brick[ss]->runAction(moveFrom);（物块动画，现已删除）
        robot->runAction(moveFrom->clone());
        //需要更改数字
    }

    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void jump(int& i, int x, easy2d::Sprite* robot) {//缺少指针动画
    if (x > 0 && x <= instruction_count) {
        i = x - 2;
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void jumpifzero(int& i, int x, easy2d::Sprite* robot) {//缺少指针动画
    if (current_block == 0) {
        if (x > 0 && x <= instruction_count)
            i = x - 2;
        else {
            cout << "Error on instruction " << (i + 1) << endl;
            error_flag = true;
        }
    }
    else {
        if (current_block == INT_MIN) {
            cout << "Error on instruction " << (i + 1) << endl;
            error_flag = true;
        }
    }
}





bool run() {
    if (Game::init()) {

        //GUI
        // 创建一个空场景
        Window::setTitle("111");
        Window::setSize(1600, 900);
        auto scene = new Scene;
        SceneManager::enter(scene);
        
        auto bg = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\levelbg.png");
        scene->addChild(bg);
        auto robot = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\robot3.png");
        robot->setPos(180, 200);
        auto rb_text = new Text("0");
        robot->addChild(rb_text);
        rb_text->setPos(50, 10);
        scene->addChild(robot);

        if (level == 1) {
            prepare_inbricks(2);

            //begin GUI
            auto bgrec = gcnew Shape(Shape::Rect, Rect(Point(176, 30), Size(1230,80)));
            auto bg = gcnew ShapeNode(bgrec);
            bg->setFillColor(RGB(90, 185, 136));//
            bg->setStrokeColor(RGB(26, 64, 137));//
            bg->setStrokeWidth(5);
            scene->addChild(bg);
            auto text = new Text("第一关：让机器人取出输入序列上的每个积木，放入输出序列中",Font("Palatino Linotype", 30));
            scene->addChild(text);
            text->setAnchor(0.5f, 0.5f);
            text->setPos(800, 70);

            
            
            
            for (int i = 0;i < 2;i++) {
                brick[i] = new Node();
                square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
                string num0 = to_string(input[i]);
                number[i] = new Text(num0);
                brick[i]->addChild(square[i]);
                brick[i]->addChild(number[i]);
                scene->addChild(brick[i]);
                brick[i]->setAnchor(0.5f, 0.5f);
                brick[i]->setPos(150, 200 + 50 * i);
            }
           
            auto display = gcnew Text[instruction_count];
            for (int i = 0;i < instruction_count;i++) {
                display[i].setText(instruction[i]);
                display[i].setAnchor(0, 0.5f);
                display[i].setPos(650, 150 + i * 30);
                scene->addChild(display+i);
            }

            scene->addChild(pointer0);
            pointer0->setAnchor(1, 0.5f);
            pointer0->setPos(640, 150);

            //end GUI

        }
        else if (level == 2) {
            prepare_inbricks(8);
            space.resize(spacenumber, INT_MIN);

            //begin GUI
            auto text = new Text("第二关：对于输入序列中的每两个东西，先把第1个减去第2个，并把结果放在输出序列中，然后把第2个减去第1个，再把结果放在输出序列中，重复。");
            scene->addChild(text);
            text->setAnchor(0.5f, 0.5f);
            text->setPos(400, 100);

           
            
            for (int i = 0;i < 8;i++) {
                brick[i] = new Node();
                square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
                string num0 = to_string(input[i]);
                number[i] = new Text(num0);
                brick[i]->addChild(square[i]);
                brick[i]->addChild(number[i]);
                scene->addChild(brick[i]);
                brick[i]->setAnchor(0.5f, 0.5f);
                brick[i]->setPos(150, 200 + 40 * i);
            }
            
            scene->addChild(robot);
            robot->setAnchor(0.5f, 0.5f);
            robot->setPos(180, 200);

            Text* display = new Text[instruction_count];
            for (int i = 0;i < instruction_count;i++) {
                display[i].setText(instruction[i]);
                display[i].setAnchor(0, 0.5f);
                display[i].setPos(650, 150 + i * 15);
            }

            scene->addChild(pointer0);
            pointer0->setAnchor(1, 0.5f);
            pointer0->setPos(640, 150);

            //end GUI

        }
        else if (level == 3) {
            prepare_inbricks(8);
            space.resize(spacenumber, INT_MIN);

            //begin GUI
            auto text = new Text("第三关：从输入序列中依次取2个数字，如果相等则将其中一个输出，否则扔掉。重复这个过程直到输入传送带为空。");
            scene->addChild(text);
            text->setAnchor(0.5f, 0.5f);
            text->setPos(400, 100);

    
            for (int i = 0;i < 8;i++) {
                brick[i] = new Node();
                square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
                string num0 = to_string(input[i]);
                number[i] = new Text(num0);
                brick[i]->addChild(square[i]);
                brick[i]->addChild(number[i]);
                scene->addChild(brick[i]);
                brick[i]->setAnchor(0.5f, 0.5f);
                brick[i]->setPos(150, 200 + 40 * i);
            }
            
            scene->addChild(robot);
            robot->setAnchor(0.5f, 0.5f);
            robot->setPos(180, 200);

            Text* display[100];
            for (int i = 0;i < instruction_count;i++) {
                display[i] = new Text(instruction[i]);
                scene->addChild(display[i]);
                display[i]->setAnchor(0, 0.5f);
                display[i]->setPos(650, 150 + i * 15);
            }

            scene->addChild(pointer0);
            pointer0->setAnchor(1, 0.5f);
            pointer0->setPos(640, 150);


            //end GUI
        }

        //Game::start();


        int i = 0, out_index = 0;

        auto rb_se = gcnew Sequence();
        auto stopp = gcnew Delay(2.5);//每一步的延迟2.5s
        auto create_out = gcnew CallFunc([out_index, scene]() {
            out_brick[out_index] = new Node();
            out_square[out_index] = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
            
            string num0 = to_string(current_block);//currentblock的调用
            out_num[out_index] = gcnew Text(num0);
            out_brick[out_index]->addChild(out_square[out_index]);
            out_brick[out_index]->addChild(out_num[out_index]);
            scene->addChild(brick[out_index]);
            brick[out_index]->setAnchor(0.5f, 0.5f);
            brick[out_index]->setPos(750, 200 + 50);
            });


        while (i < instruction_count) {
            if (error_flag)
                break;
            if (instructions[i].name == "inbox") {
                inbox(i,robot,rb_se,rb_text,stopp);
            }
            else if (instructions[i].name == "outbox") {
                outbox(i,robot,rb_se,rb_text,create_out, stopp);
                out_index++;//画图用
                destination_count++;
            }
            else if (instructions[i].name == "add" && level != 1 && instructions[i].is_num_int) {
                add(i, instructions[i].number,robot);
            }
            else if (instructions[i].name == "sub" && level != 1 && instructions[i].is_num_int) {
                sub(i, instructions[i].number, robot);
            }
            else if (instructions[i].name == "copyto" && level != 1 && instructions[i].is_num_int) {
                copyto(i, instructions[i].number, robot);
            }
            else if (instructions[i].name == "copyfrom" && level != 1 && instructions[i].is_num_int) {
                copyfrom(i, instructions[i].number, robot);
            }
            else if (instructions[i].name == "jump" && level != 1 && instructions[i].is_num_int) {
                jump(i, instructions[i].number, robot);
            }
            else if (instructions[i].name == "jumpifzero" && level != 1 && instructions[i].is_num_int) {
                jumpifzero(i, instructions[i].number, robot);
            }
            else {
                cout << "Error on instruction " << (i + 1) << endl;
                return true;
            }

            if (inbox_done)
                break;

            i++;

            if (error_flag == false) {
                auto movePointer = gcnew MoveTo(2.5f, Point(640, 150 + i * 15));
                pointer0->runAction(movePointer);
            }

            
        }

        robot->runAction(rb_se);
        for (int i = 0; i <2; i++)
        {
            brick[i]->runAction(bricks_ani[i]);
        }
        for (int i = 0; i < 1; i++)
        {
            out_brick[i]->runAction(out_bricks_ani[i]);
        }
        check();
        Game::start();
        //Game::pause();
        if (error_flag == false)
            return true;


    }
    /* 销毁资源 */

    

}

int main()
{
    //原本input、target、availables、spaces在这里定义
    int a;
    string retry;
    //初始化
    do
    {
        a = Load_menu();//加载目录
    } while (!(a + 1));
    while (!quit)
    {
        upload_data(levels);

        Menu();//显示目录

        int spacenumber;//初始化;spacenumber:space number
        do {
            spacenumber = initial(/*input,target,availables*/);
        } while (!(spacenumber + 1) && !quit);


        if (quit)//退出游戏
            break;

        space.resize(spacenumber);

        //正式开始运行关卡
        succeed = 0;
        while (!succeed)//游戏进行中
        {

            const string last_inp = "C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\text\\user_level" + levels + ".txt";
            load_instructions_from_file(last_inp, last_instructions);//按行存储上一次的指令

            show_init();//显示界面

            user_input_cons();
            succeed = 0;
            succeed = run();

            Game::pause();
            if (succeed)
            {
                user_levels[level] = 1;
                break;
            }

            else//失败重新尝试
            {
                cout << "Enter R to retry." << endl;//文件输入暂时无法实现retry功能
                cin >> retry;
                if (retry == "R")
                    continue;
                else
                    break;
            }
        }
    }


    delete(pointer0);
    Game::destroy();
    return 0;
}
//之前：读取数据；
//输入关卡-》通过关卡号初始化待输入队列、目标输出、可用指令集-》转接进入关卡；
//之后：用文件形式保存数据，这个还没做

//invalidinput 之后的程序运行中止
