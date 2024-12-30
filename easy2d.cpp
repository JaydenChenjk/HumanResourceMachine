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
ShapeNode* SPAces[5];
Text* space_num[5];
vector<Sequence*> bricks_ani;//1到8是输入输出的block
vector<Sequence*> out_bricks_ani;
int destination_count = 0;
int space_brick[3];
float wait_time = 0;
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

void Press1(string& instruction000) {
    bool isPressed_a = Input::isPressed(KeyCode::A);
    bool isPressed_b = Input::isPressed(KeyCode::B);
    bool isPressed_c = Input::isPressed(KeyCode::C);
    bool isPressed_d = Input::isPressed(KeyCode::D);
    bool isPressed_e = Input::isPressed(KeyCode::E);
    bool isPressed_f = Input::isPressed(KeyCode::F);
    bool isPressed_g = Input::isPressed(KeyCode::G);
    bool isPressed_h = Input::isPressed(KeyCode::H);
    bool isPressed_i = Input::isPressed(KeyCode::I);
    bool isPressed_j = Input::isPressed(KeyCode::J);
    bool isPressed_k = Input::isPressed(KeyCode::K);
    bool isPressed_l = Input::isPressed(KeyCode::L);
    bool isPressed_m = Input::isPressed(KeyCode::M);
    bool isPressed_n = Input::isPressed(KeyCode::N);
    bool isPressed_o = Input::isPressed(KeyCode::O);
    bool isPressed_p = Input::isPressed(KeyCode::P);
    bool isPressed_q = Input::isPressed(KeyCode::Q);
    bool isPressed_r = Input::isPressed(KeyCode::R);
    bool isPressed_s = Input::isPressed(KeyCode::S);
    bool isPressed_t = Input::isPressed(KeyCode::T);
    bool isPressed_u = Input::isPressed(KeyCode::U);
    bool isPressed_v = Input::isPressed(KeyCode::V);
    bool isPressed_w = Input::isPressed(KeyCode::W);
    bool isPressed_x = Input::isPressed(KeyCode::X);
    bool isPressed_y = Input::isPressed(KeyCode::Y);
    bool isPressed_z = Input::isPressed(KeyCode::Z);
    bool isPressed_0 = Input::isPressed(KeyCode::Num0);
    bool isPressed_1 = Input::isPressed(KeyCode::Num1);
    bool isPressed_2 = Input::isPressed(KeyCode::Num2);
    bool isPressed_3 = Input::isPressed(KeyCode::Num3);
    bool isPressed_4 = Input::isPressed(KeyCode::Num4);
    bool isPressed_5 = Input::isPressed(KeyCode::Num5);
    bool isPressed_6 = Input::isPressed(KeyCode::Num6);
    bool isPressed_7 = Input::isPressed(KeyCode::Num7);
    bool isPressed_8 = Input::isPressed(KeyCode::Num8);
    bool isPressed_9 = Input::isPressed(KeyCode::Num9);
    bool isPressed_Space = Input::isPressed(KeyCode::Space);

    if (isPressed_a) {
        instruction000 += 'a';  // a -> a

    }
    else if (isPressed_b) {
        instruction000 += 'b';  // b -> b
    }
    else if (isPressed_c) {
        instruction000 += 'c';  // c -> c
    }
    else if (isPressed_d) {
        instruction000 += 'd';  // d -> d
    }
    else if (isPressed_e) {
        instruction000 += 'e';  // e -> e
    }
    else if (isPressed_f) {
        instruction000 += 'f';  // f -> f
    }
    else if (isPressed_g) {
        instruction000 += 'g';  // g -> g
    }
    else if (isPressed_h) {
        instruction000 += 'h';  // h -> h
    }
    else if (isPressed_i) {
        instruction000 += 'i';  // i -> i
    }
    else if (isPressed_j) {
        instruction000 += 'j';  // j -> j
    }
    else if (isPressed_k) {
        instruction000 += 'k';  // k -> k
    }
    else if (isPressed_l) {
        instruction000 += 'l';  // l -> l
    }
    else if (isPressed_m) {
        instruction000 += 'm';  // m -> m
    }
    else if (isPressed_n) {
        instruction000 += 'n';  // n -> n
    }
    else if (isPressed_o) {
        instruction000 += 'o';  // o -> o
    }
    else if (isPressed_p) {
        instruction000 += 'p';  // p -> p
    }
    else if (isPressed_q) {
        instruction000 += 'q';  // q -> q
    }
    else if (isPressed_r) {
        instruction000 += 'r';  // r -> r
    }
    else if (isPressed_s) {
        instruction000 += 's';  // s -> s
    }
    else if (isPressed_t) {
        instruction000 += 't';  // t -> t
    }
    else if (isPressed_u) {
        instruction000 += 'u';  // u -> u
    }
    else if (isPressed_v) {
        instruction000 += 'v';  // v -> v
    }
    else if (isPressed_w) {
        instruction000 += 'w';  // w -> w
    }
    else if (isPressed_x) {
        instruction000 += 'x';  // x -> x
    }
    else if (isPressed_y) {
        instruction000 += 'y';  // y -> y
    }
    else if (isPressed_z) {
        instruction000 += 'z';  // z -> z
    }
    else if (isPressed_0) {
        instruction000 += '0';  // 0 -> 0
    }
    else if (isPressed_1) {
        instruction000 += '1';  // 1 -> 1
    }
    else if (isPressed_2) {
        instruction000 += '2';  // 2 -> 2
    }
    else if (isPressed_3) {
        instruction000 += '3';  // 3 -> 3
    }
    else if (isPressed_4) {
        instruction000 += '4';  // 4 -> 4
    }
    else if (isPressed_5) {
        instruction000 += '5';  // 5 -> 5
    }
    else if (isPressed_6) {
        instruction000 += '6';  // 6 -> 6
    }
    else if (isPressed_7) {
        instruction000 += '7';  // 7 -> 7
    }
    else if (isPressed_8) {
        instruction000 += '8';  // 8 -> 8
    }
    else if (isPressed_9) {
        instruction000 += '9';  // 9 -> 9
    }
    else if (isPressed_Space) {
        instruction000 += ' ';
    }

    return;
}

void onUpdate(Scene *scene)
{
    vector<string> label(instruction_count);
    for (int i = 0; i < instruction_count; i++) {
        label[i] = "";
        if (instruction[i] != "")
            label[i] = instruction[i];
    }
    Text* display = new Text[instruction_count];
    for (int i = 0; i < instruction_count; i++) {
        display[i].setText(label[i]);
        display[i].addChild(scene);
        display[i].setAnchor(0, 0.5f);
        display[i].setPos(650, 150 + i * 15);
    }
}


void load_instructions_from_keyboard_2() {
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

void load_instructions_from_keyboard(Scene *scene) {
    try {
        string insta;
        bool isPressed_Enter = Input::isPressed(KeyCode::Enter);
        while (isPressed_Enter == false)
            Press1(insta);
        instruction_count = stoi(insta);
        if (instruction_count == 0)
        {
            cout << "Invalid Input!" << endl;
            return;
        }
        onUpdate(scene);

        instructions.resize(instruction_count);
        instruction.resize(instruction_count);
        for (int i = 0; i < instruction_count; i++) {
            while (isPressed_Enter == false)
                Press1(instruction[i]);

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
        string path = "C:\\Users\\jayde\\source\\repos\\easy2d\\level" + levels + ".txt";
        flevel_in.open("C:\\Users\\jayde\\source\\repos\\easy2d\\level0.txt");
        flevel_in.close();
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

            fusers_lstins.open("C:\\Users\\jayde\\source\\repos\\easy2d\\user_level" + lvplayed + ".txt");
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

void check(Scene* scene)
{
    auto Judge = gcnew Text;
    Judge->setFont(Font("Palatino Linotype", 130));
    Judge->setAnchor(0.5f, 0.5f);
    Judge->setPos(800, 450);
    Judge->setFillColor(Color::OrangeRed);
    Judge->setStrokeColor(Color::OrangeRed);
    Judge->setStrokeWidth(5);
    if (output == target)
    {
        succeed = true;
        Judge->setText("Success");
    }

    else
    {
        succeed = false;
        Judge->setText("Fail");
    }
    scene->addChild(Judge);
}

void user_input_cons(Scene *scene)
{
    string mode;
    cout << "Choose input mode (keyboard/file): ";
    cin >> mode;
    cin.ignore();
    if (mode == "keyboard")
        load_instructions_from_keyboard(scene);
    else if (mode == "file") {
        string file_path;
        cout << "Enter the file path: ";
        cin >> file_path;
        load_instructions_from_file(file_path, instructions);
    }
    else {
        cout << "Invalid mode." << endl;
        error_flag = true;
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

void wait_for_move(int out_index, Delay* delay)
{
    for (int j = 0; j < input.size(); j++) {
        bricks_ani[j]->add(delay->clone());//保留传送带动画
    }
    for (int j = 0; j < out_bricks_ani.size(); j++) {
        out_bricks_ani[j]->add(delay->clone());
        //可以加一个暂停动画
    }
    wait_time += 2.5;
    return;
}
void wait_for_move2(int out_index, Delay* delay)
{
    auto delay2 = gcnew Delay(2.0);
    for (int j = 0; j < input.size(); j++) {
        bricks_ani[j]->add(delay->clone());//保留传送带动画
    }
    for (int j = 0; j < out_bricks_ani.size(); j++) {
        out_bricks_ani[j]->add(delay2->clone());
        //可以加一个暂停动画
    }
    wait_time += 2.5;
    return;
}

void inbox(int i, int out_index, Sequence* rb_se, Text* rb_text, Delay* delay) {
    if (position < input.size()) {
        current_block = input[position];

        //GUI        
        auto moveBy_inbox = gcnew MoveTo(2.5f, Vector2(300, 200));
        string s = std::to_string(current_block);
        int pos = position;

        auto changeText = gcnew CallFunc([rb_text, s]() {rb_text->setText(s); });
        auto block_vis = gcnew CallFunc([pos]() {brick[pos]->setVisible(false); });//注意position的调用？
        //auto inbox_act = gcnew Spawn({ moveBy_inbox, changeText});

        rb_se->add(moveBy_inbox->clone());
        rb_se->add(changeText->clone());

        wait_for_move(out_index, delay);
        bricks_ani[position]->add(block_vis->clone());

        auto moveBy = gcnew MoveBy(0.5f, Vector2(0, -60));
        auto delay2 = gcnew Delay(0.5);
        for (int j = position + 1;j < input.size();j++) {
            bricks_ani[j]->add(moveBy->clone());//保留传送带动画
        }
        for (int j = 0; j < out_index; j++) {
            out_bricks_ani[j]->add(delay2->clone());
        }
        wait_time += 0.5;
        rb_se->add(delay2->clone());

        position++;
    }
    else {
        inbox_done = true;
    }
}

void outbox(int i, int out_index, Sequence* rb_se, Scene* scene, Text* rb_text, Delay* delay) {
    if (current_block != INT_MIN) {
        output.push_back(current_block);


        //GUI  
        auto moveBy_outbox = gcnew MoveTo(2.5f, Vector2(1100, 200));
        auto changeText = gcnew CallFunc([rb_text]() {rb_text->setText(" "); });

        int a = current_block;
        string num0 = to_string(a);
        out_brick[out_index + 1] = new Node();//创建新输出块
        out_square[out_index + 1] = new Sprite();
        out_num[out_index + 1] = new Text;
        out_square[out_index]->open("C:\\Users\\jayde\\source\\repos\\easy2d\\block4.png");
        out_brick[out_index]->addChild(out_square[out_index]);
        out_num[out_index]->setText(num0);
        out_num[i]->setAnchor(0.5f, 0.5f);
        out_num[i]->setPos(25, 25);
        out_brick[out_index]->addChild(out_num[out_index]);
        out_brick[out_index]->setAnchor(0.5f, 0.5f);
        out_brick[out_index]->setPos(1220, 200);
        out_brick[out_index]->setVisible(false);
        scene->addChild(out_brick[out_index]);

        auto create_out = gcnew CallFunc([out_index]() {out_brick[out_index]->setVisible(true);});
        auto outbox_act = gcnew Spawn({ changeText, create_out });

        rb_se->add(moveBy_outbox->clone());
        rb_se->add(outbox_act->clone());
        auto seq = new Sequence;
        out_bricks_ani.push_back(seq);//在下一个来的时候再创建新的sequence.
        //cout << out_bricks_ani.size();

        //wait_time -= 0.5;
        float tf = wait_time;
        auto dl = gcnew Delay(tf);
        out_bricks_ani[out_index]->add(dl->clone());

        wait_for_move2(out_index, delay);

        auto moveBy = gcnew MoveBy(0.5f, Vector2(0, 60));
        for (int j = 0; j < out_bricks_ani.size() - 1; j++) {
            out_bricks_ani[j]->add(moveBy->clone());
        }

        auto delay2 = gcnew Delay(0.5);
        out_bricks_ani[out_index]->add(delay2->clone());
        for (int j = position; j < input.size(); j++) {
            bricks_ani[j]->add(delay2->clone());//保留传送带动画
            //可以加一个暂停动画
        }
        for (int j = 0; j <= out_index; j++) {
            out_bricks_ani[j]->add(delay2->clone());
        }
        wait_time += 0.5;
        rb_se->add(delay2->clone());

        current_block = INT_MIN;
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void add(int i, int x, int out_index, Sequence* rb_se, Text* rb_text, Delay* delay) {
    if ((space[x] != INT_MIN) && (x >= 0) && (x < spacenumber)) {
        current_block += space[x];

        //GUI
        auto moveTo = gcnew MoveTo(2.5f, Point(500 + 120 * x, 500));
        string s = std::to_string(current_block);
        auto changeText = gcnew CallFunc([rb_text, s]() {rb_text->setText(s); });
        //auto inbox_act = gcnew Spawn({ moveTo, changeText });
        rb_se->add(moveTo);
        rb_se->add(changeText);
        wait_for_move(out_index, delay);

    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void sub(int i, int x, int out_index, Sequence* rb_se, Text* rb_text, Delay* delay) {
    if ((space[x] != INT_MIN) && (x >= 0) && (x < spacenumber)) {
        current_block -= space[x];

        //GUI
        auto moveTo = gcnew MoveTo(2.5f, Point(500 + 120 * x, 500));
        string s = std::to_string(current_block);
        auto changeText = gcnew CallFunc([rb_text, s]() {rb_text->setText(s); });
        rb_se->add(moveTo);
        rb_se->add(changeText);
        wait_for_move(out_index, delay);
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void copyto(int i, int x, int out_index, Sequence* rb_se, Text* blk_text, Delay* delay) {
    if (current_block != INT_MIN && x < spacenumber && x >= 0) {
        space[x] = current_block;

        //GUI
        //auto moveTo = gcnew MoveTo(2.5f, Point(300+50*x, 400));
        //brick[position]->runAction(moveTo);（物块动画，现已删除）
        //robot->runAction(moveTo->clone());
        space_brick[x] = position;//这句话啥意思

        auto moveTo = gcnew MoveTo(2.5f, Point(500 + 120 * x, 500));
        string s = std::to_string(current_block);
        auto changeText = gcnew CallFunc([blk_text, s]() {blk_text->setText(s); });
        //auto inbox_act = gcnew Spawn({ moveTow, changeText });
        rb_se->add(moveTo);
        rb_se->add(changeText);
        wait_for_move(out_index, delay);
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void copyfrom(int i, int x, int out_index, Sequence* rb_se, Text* rb_text, Delay* delay) {
    if (space[x] != INT_MIN && x < spacenumber && x >= 0) {
        current_block = space[x];

        //GUI
        auto moveTo = gcnew MoveTo(2.5f, Point(500 + 120 * x, 500));
        string s = std::to_string(current_block);
        auto changeText = gcnew CallFunc([rb_text, s]() {rb_text->setText(s); });
        //auto inbox_act = gcnew Spawn({ moveTo, changeText });
        rb_se->add(moveTo);
        rb_se->add(changeText);
        wait_for_move(out_index, delay);
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

void addstop(Sequence* rb_se, int pos)
{
    auto ani_pause = gcnew Delay(0.5);
    rb_se->add(ani_pause->clone());
    for (int j = pos; j < input.size(); j++) {
        bricks_ani[j]->add(ani_pause->clone());//保留传送带动画
    }
    for (int j = 0; j < out_bricks_ani.size(); j++) {
        out_bricks_ani[j]->add(ani_pause->clone());//保留传送带动画
    }
    wait_time += 0.5;
}/////


template <typename T>
string arrayToString(const vector<T>& vec) {
    stringstream ss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i != 0) {
            ss << ",";
        }
        ss << vec[i];
    }
    return ss.str();
}


bool run(Scene* scene) {
    //GUI
        // 创建一个空场景


    auto bg = gcnew Sprite("C:\\Users\\jayde\\source\\repos\\easy2d\\levelbg.png");
    scene->addChild(bg);
    auto belt1 = gcnew Sprite("C:\\Users\\jayde\\source\\repos\\easy2d\\csdd.png");
    belt1->setAnchor(0, 1.0f);
    belt1->setPos(200, 750);
    auto belt2 = gcnew Sprite("C:\\Users\\jayde\\source\\repos\\easy2d\\csd.png");
    belt2->setAnchor(1.0f, 1.0f);
    belt2->setPos(1300, 750);
    scene->addChild(belt1);
    scene->addChild(belt2);

    auto robot = gcnew Sprite("C:\\Users\\jayde\\source\\repos\\easy2d\\robot3.png");
    robot->setPos(480, 200);
    auto rb_text = new Text(" ");
    robot->addChild(rb_text);
    rb_text->setPos(50, 10);
    scene->addChild(robot);

    auto bgrec = gcnew Shape(Shape::Rect, Rect(Point(176, 30), Size(1230, 80)));
    auto bgrec1 = gcnew Shape(Shape::Rect, Rect(Point(1400, 200), Size(80, 300)));
    //auto bgrec2 = gcnew Shape(Shape::Rect, Rect(Point(1400, 200), Size(80, 300)));
    auto bg0 = gcnew ShapeNode(bgrec);
    auto bg1 = gcnew ShapeNode(bgrec1);
    bg0->setFillColor(RGB(90, 185, 136));//
    bg0->setStrokeColor(RGB(26, 64, 137));//
    bg0->setStrokeWidth(5);
    scene->addChild(bg0);
    bg1->setFillColor(RGB(90, 185, 136));//
    bg1->setStrokeColor(RGB(26, 64, 137));//
    bg1->setStrokeWidth(5);
    scene->addChild(bg1);

    string INPUT, TARGET;//input和target
    INPUT = arrayToString(input);
    TARGET = arrayToString(target);

    /*
    auto input_display = gcnew Text(INPUT);
    auto target_display = gcnew Text(TARGET);
    scene->addChild(input_display);
    input_display->setAnchor(0, 0.5f);
    input_display->setPos(200, 450);
    scene->addChild(target_display);
    target_display->setAnchor(0, 0.5f);
    target_display->setPos(200, 500);*/



    if (level == 1) {
        prepare_inbricks(3);

        //begin GUI
        auto bgrec = gcnew Shape(Shape::Rect, Rect(Point(176, 30), Size(1230, 80)));
        auto bg3 = gcnew ShapeNode(bgrec);
        bg3->setFillColor(RGB(90, 185, 136));//
        bg3->setStrokeColor(RGB(26, 64, 137));//
        bg3->setStrokeWidth(5);
        scene->addChild(bg3);

        auto text = new Text("第一关：让机器人取出输入序列上的每个积木，放入输出序列中", Font("Palatino Linotype", 30));
        scene->addChild(text);
        text->setAnchor(0.5f, 0.5f);
        text->setPos(800, 70);




        for (int i = 0; i < 3; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\jayde\\source\\repos\\easy2d\\block4.png");
            string num0 = to_string(input[i]);
            number[i] = new Text(num0);
            number[i]->setAnchor(0.5f, 0.5f);
            number[i]->setPos(25, 25);
            brick[i]->addChild(square[i]);
            brick[i]->addChild(number[i]);
            scene->addChild(brick[i]);
            brick[i]->setAnchor(0.5f, 0.5f);
            brick[i]->setPos(220, 200 + 60 * i);
        }

        auto display = gcnew Text[instruction_count];
        for (int i = 0; i < instruction_count; i++) {
            display[i].setText(instruction[i]);
            display[i].setAnchor(0, 0.5f);
            display[i].setPos(1405, 250 + i * 30);
            scene->addChild(display + i);
        }

        scene->addChild(pointer0);
        pointer0->setAnchor(1, 0.5f);
        pointer0->setPos(1380, 150);

        //end GUI

    }
    else if (level == 2) {
        prepare_inbricks(8);
        space.resize(spacenumber, INT_MIN);

        //begin GUI
        auto text = new Text("第二关：对于输入序列中的每两个东西，\n先把第1个减去第2个，并把结果放在输出序列中，然后把第2个减去第1个，再把结果放在输出序列中，重复。");
        scene->addChild(text);
        text->setAnchor(0.5f, 0.5f);
        text->setPos(800, 70);

        for (int i = 0; i < 3; i++)
        {
            auto a = gcnew Shape(Shape::Rect, Rect(Point(500 + 120 * i, 700), Size(70, 70)));
            SPAces[i] = gcnew ShapeNode(a);
            SPAces[i]->setFillColor(Color::Chocolate);
            space_num[i] = gcnew Text("111");
            space_num[i]->setFont(Font("Arial", 35));
            space_num[i]->setFillColor(Color::White);
            space_num[i]->setAnchor(0.5f, 0.5f);
            space_num[i]->setPos(525 + 120 * i, 725);
            SPAces[i]->addChild(space_num[i]);
            scene->addChild(SPAces[i]);
        }

        for (int i = 0; i < 8; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\jayde\\source\\repos\\easy2d\\block4.png");
            string num0 = to_string(input[i]);
            number[i] = new Text(num0);
            number[i]->setAnchor(0.5f, 0.5f);
            number[i]->setPos(25, 25);
            brick[i]->addChild(square[i]);
            brick[i]->addChild(number[i]);
            scene->addChild(brick[i]);
            brick[i]->setAnchor(0.5f, 0.5f);
            brick[i]->setPos(220, 200 + 60 * i);
        }

        /*Text* display = new Text[instruction_count];
        for (int i = 0; i < instruction_count; i++) {
            display[i].setText(instruction[i]);
            display[i].setAnchor(0, 0.5f);
            display[i].setPos(650, 150 + i * 15);
        }*/

        scene->addChild(pointer0);
        pointer0->setAnchor(1, 0.5f);
        pointer0->setPos(640, 150);

        //end GUI
        //Game::start();
    }
    else if (level == 3) {
        prepare_inbricks(8);
        space.resize(spacenumber, INT_MIN);

        //begin GUI
        auto text = new Text("第三关：从输入序列中依次取2个数字，如果相等则将其中一个输出，\n否则扔掉。重复这个过程直到输入传送带为空。");
        scene->addChild(text);
        text->setAnchor(0.5f, 0.5f);
        text->setPos(800, 70);

        for (int i = 0; i < 3; i++)
        {
            auto a = gcnew Shape(Shape::Rect, Rect(Point(500 + 120 * i, 700), Size(70, 70)));
            SPAces[i] = gcnew ShapeNode(a);
            SPAces[i]->setFillColor(Color::Chocolate);
            space_num[i] = gcnew Text("111");
            space_num[i]->setFont(Font("Arial", 35));
            space_num[i]->setFillColor(Color::White);
            space_num[i]->setAnchor(0.5f, 0.5f);
            space_num[i]->setPos(525 + 120 * i, 725);
            SPAces[i]->addChild(space_num[i]);
            scene->addChild(SPAces[i]);
        }

        for (int i = 0; i < 8; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\jayde\\source\\repos\\easy2d\\block4.png");
            string num0 = to_string(input[i]);
            number[i] = new Text(num0);
            number[i]->setAnchor(0.5f, 0.5f);
            number[i]->setPos(25, 25);
            brick[i]->addChild(square[i]);
            brick[i]->addChild(number[i]);
            scene->addChild(brick[i]);
            brick[i]->setAnchor(0.5f, 0.5f);
            brick[i]->setPos(220, 200 + 60 * i);
        }

        /*Text* display[100];
        for (int i = 0; i < instruction_count; i++) {
            display[i] = new Text(instruction[i]);
            scene->addChild(display[i]);
            display[i]->setAnchor(0, 0.5f);
            display[i]->setPos(650, 150 + i * 15);
        }*/

        scene->addChild(pointer0);
        pointer0->setAnchor(1, 0.5f);
        pointer0->setPos(640, 150);
        //end GUI
    }
    else if (level == 4) {
        prepare_inbricks(8);
        space.resize(spacenumber, INT_MIN);

        //begin GUI
        auto text = new Text("第四关：将输入序列上每个积木的数都除以2，并放在输出序列上。（保证所有数都是整数，不用考虑浮点数的情况）");
        scene->addChild(text);
        text->setAnchor(0.5f, 0.5f);
        text->setPos(800, 70);

        for (int i = 0; i < 3; i++)
        {
            auto a = gcnew Shape(Shape::Rect, Rect(Point(500 + 120 * i, 700), Size(70, 70)));
            SPAces[i] = gcnew ShapeNode(a);
            SPAces[i]->setFillColor(Color::Chocolate);
            space_num[i] = gcnew Text("111");
            space_num[i]->setFont(Font("Arial", 35));
            space_num[i]->setFillColor(Color::White);
            space_num[i]->setAnchor(0.5f, 0.5f);
            space_num[i]->setPos(525 + 120 * i, 725);
            SPAces[i]->addChild(space_num[i]);
            scene->addChild(SPAces[i]);
        }

        for (int i = 0; i < 8; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\jayde\\source\\repos\\easy2d\\block4.png");
            string num0 = to_string(input[i]);
            number[i] = new Text(num0);
            number[i]->setAnchor(0.5f, 0.5f);
            number[i]->setPos(25, 25);
            brick[i]->addChild(square[i]);
            brick[i]->addChild(number[i]);
            scene->addChild(brick[i]);
            brick[i]->setAnchor(0.5f, 0.5f);
            brick[i]->setPos(220, 200 + 60 * i);
        }

        scene->addChild(robot);
        robot->setAnchor(0.5f, 0.5f);
        robot->setPos(180, 200);


        scene->addChild(pointer0);
        pointer0->setAnchor(1, 0.5f);
        pointer0->setPos(640, 150);


        //end GUI
    }


    int i = 0, out_index = 0;

    auto rb_se = gcnew Sequence();
    auto point_se = gcnew Sequence();
    auto stopp = gcnew Delay(2.5);//每一步的延迟2.5s
    auto del2 = gcnew Delay(0.5);//每一步的延迟2.5s

    out_brick[0] = new Node();
    out_square[0] = new Sprite;
    out_num[0] = new Text;



    while (i < instruction_count) {
        if (error_flag)
            break;

        addstop(rb_se, position);
        if (instructions[i].name == "inbox") {
            inbox(i, out_index, rb_se, rb_text, stopp);//3s
        }
        else if (instructions[i].name == "outbox") {
            outbox(i, out_index, rb_se, scene, rb_text, stopp);//3s
            out_index++;//画图用
            destination_count++;
        }
        else if (instructions[i].name == "add" && level != 1 && instructions[i].is_num_int) {
            add(i, instructions[i].number, out_index, rb_se, rb_text, stopp);
        }
        else if (instructions[i].name == "sub" && level != 1 && instructions[i].is_num_int) {
            sub(i, instructions[i].number, out_index, rb_se, rb_text, stopp);
        }
        else if (instructions[i].name == "copyto" && level != 1 && instructions[i].is_num_int) {
            copyto(i, instructions[i].number, out_index, rb_se, space_num[instructions[i].number], stopp);
        }
        else if (instructions[i].name == "copyfrom" && level != 1 && instructions[i].is_num_int) {
            copyfrom(i, instructions[i].number, out_index, rb_se, rb_text, stopp);
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
            point_se->add(movePointer);
            point_se->add(del2);
        }


    }

    if (error_flag == true)
        return false;

    addstop(rb_se, position);

    auto check1 = gcnew CallFunc([scene]() {check(scene); });
    rb_se->add(check1);
    robot->runAction(rb_se->clone());
    for (int i = 0; i < bricks_ani.size(); i++)
    {
        brick[i]->runAction(bricks_ani[i]->clone());
    }
    for (int i = 0; i < out_bricks_ani.size(); i++)
    {
        out_brick[i]->runAction(out_bricks_ani[i]->clone());
    }

    Game::start();

    rb_se->release();
    //Game::pause();

    if (error_flag == false)
        return true;

}

void cleanchild(Scene* scene)//不要动它
{
    for (int i = 0; i < bricks_ani.size(); i++)
    {
        bricks_ani[i]->release();
    }
    for (int i = 0; i < out_bricks_ani.size(); i++)
    {
        out_bricks_ani[i]->release();
    }
    out_bricks_ani.clear();
    bricks_ani.clear();
    SceneManager::clear();
    //scene->release();
}

int main()
{
    if (Game::init())
    {
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

                //Game::init();
                error_flag = false;
                const string last_inp = "C:\\Users\\jayde\\source\\repos\\easy2d\\user_level" + levels + ".txt";
                load_instructions_from_file(last_inp, last_instructions);//按行存储上一次的指令

                show_init();//显示界面

                Window::setTitle("111");
                Window::setSize(1600, 900);
                auto scene = new Scene;
                SceneManager::enter(scene);

                user_input_cons(scene);


                succeed = 0;
                position = 0;
                output.clear();
                output.resize(0);
                bool no_error = run(scene);

                if (!no_error)
                    succeed = false;


                //以下别碰，我也不知道怎么跑起来的……
                //Game::pause();
                if (succeed)
                {
                    user_levels[level] = 1;
                    SceneManager::back();
                    cleanchild(scene);
                    Game::reset();
                    break;
                }

                else//失败重新尝试
                {
                    cout << "Enter R to retry." << endl;//文件输入暂时无法实现retry功能
                    cin >> retry;
                    if (retry == "R")
                    {
                        cleanchild(scene);
                        Game::reset();
                    }
                    else
                    {
                        cleanchild(scene);
                        Game::reset();
                        break;
                    }

                }
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
