#include<easy2d/easy2d.h>
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include <climits>
#include <sstream>
#include <cmath>
using namespace easy2d;
using namespace std;

struct INSTRUCTIONS {
    string name;//否则后面初始化不好初始化
    int number;
    bool is_num_int = 1;
};

//全局变量设定：
bool user_levels[5] = { 1 };//存储用户关卡通关信息，关卡号就是下标！
bool user_levels_lock[5] = { 1,1 };//是否可以游玩；为了方便，从1开始记录！
bool has_file[4] = { 0,1 };
ifstream fusers_in, flevel_in;//读取用户数据文件、关卡信息
ofstream fusers_out, fusers_lstins;//更新电脑中的存档信息
bool faults = 0;//程序是否出错（也许可以记录一下是什么错？）
vector<INSTRUCTIONS> last_instructions;

vector<int> input;//输入
vector<int> target;//目标输出
vector<int> output;
vector<string> availables;//可用命令
vector<string> instruction;//仅用在GUI上
vector<INSTRUCTIONS> instructions;
vector<int> space;//空间数
string fusers;//用户数据文件
string levels;//关卡名
int lvnum = 0;
int instruction_count = 0;
int current_block = INT_MIN;
int level= 0;
int spacenumber = 3;//空间数
bool have_file[5] = { 0,1 };
bool succeed = 0, quit = 0;
bool error_flag = false;
bool inbox_done = false;
bool to_menu = false, to_level = false;
int position = 0;
string errortext;


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

/*Tips
C++ 中的字符串有很多种类型，因为 Easy2D 使用 Unicode 字符集，
所以必须在字符串前加一个前缀 ‘L’ 来标志这个字符串是 Unicode 编码的。//但这里加L会报错，不知道为什么
*/




static void enter_level(Scene* scene);
int Load_menu();
static void create_menu(easy2d::Scene* scene);
static void create_scene(Scene* scene);
// 定义一个自定义类，继承自Node
class RectWithText : public Node {
public:
    // 构造函数
    RectWithText(int i, string type, Scene* scene)
        : Node(), level_button(nullptr), level_text(nullptr) {
        // 创建实心矩形
        string text;
        Point point1,point2;
        Size size;
        Font font;

        if (type == "level")
        {
            text = "Level ";
            point1 = Point(300.0, 200.0 + 100.0 * i);
            point2 = Point(310, 208 + 100.0 * i);
            size = Size(112.0, 50.0);
            font = Font("Arial", 30);
            auto button = new Shape(Shape::Rect, Rect(point1, size));
            level_button = new ShapeNode(button);
            if (user_levels_lock[i])
                level_button->setFillColor(Color::ForestGreen);
            else
                level_button->setFillColor(Color::Gray);
            level_button->setStrokeColor(Color::DarkGreen);
            level_button->setStrokeWidth(6.0);
            this->addChild(level_button);
        }
        else if (type == "file")
        {
            point1 = Point(-400.0 + 500 * i, 300.0);
            point2 = Point(-300 + 500 * i, 408.0);
            size = Size(400.0, 300.0);
            text = "F i l e ";
            font = Font("Palatino Linotype", 50);
            auto button = new Shape(Shape::Rect, Rect(point1, size));
            level_button = new ShapeNode(button);
            if (have_file[i])
                level_button->setFillColor(Color::ForestGreen);
            else
                level_button->setFillColor(Color::Gray);
            level_button->setStrokeColor(Color::DarkGreen);
            level_button->setStrokeWidth(6.0);
            this->addChild(level_button);
            /**/
        }
        
         // 将矩形节点添加到当前节点

        // 创建文字标签
        string level_m = std::to_string(i); // 将整数level转换为字符
        std::string levelStr = text + level_m;
        level_text = new Text(levelStr, font); // 使用默认字体Arial
        level_text->setFillColor(Color::White);
        level_text->setPos(point2); // 将文字节点放置在矩形上方
        this->addChild(level_text); // 将文字节点添加到当前节点

        auto callback = [scene, i, level_m,type](ButtonEvent evt)
            {
                if (evt == ButtonEvent::Clicked)
                {
                    if (type == "level")
                    {
                        levels = level_m;
                        //打开新场景
                        //SceneManager::back();
                        auto level_scene = gcnew Scene;
                        create_scene(level_scene);
                        //SceneManager::back();
                        //SceneManager::enter(level_scene);
                        enter_level(level_scene);
                    }
                    else if (type == "file")
                    {

                        fusers = "C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\text\\user" + std::to_string(i) + ".txt";
                        Load_menu();
                        SceneManager::back();
                        auto mnscene = new Scene;
                        create_menu(mnscene);

                        //新建存档与开始游戏按钮功能待开发
                    }

                    //fusers = "text/user.txt";
                }
            };
        // 给 sprite 添加监听器
        auto clk = gcnew ButtonListener(callback);
        level_button->addListener(clk);
        //light_node->startAllListeners();
        clk->ignoreGamePaused();
    }

    // 析构函数
    virtual ~RectWithText() {
        // 清理资源
        delete level_button;
        delete level_text;
    }

private:
    Text* level_text;  // 用于显示文字的节点
    ShapeNode* level_button;  // 用于显示实心矩形的节点
};

//innerlogic
void Load_menu_datas()
{
    int lvname;//关卡号
    bool lv_is_pass;//此关卡之前是否通关
    lvnum = 0;
    while (fusers_in >> lvname >> lv_is_pass)
    {
        user_levels[lvname] = lv_is_pass;//存储用户通关数据 用户数据格式：关卡号+是否通关
        lvnum++;
    }

    for (int i = 1; i <=lvnum; i++)//用户可选关数据，关卡号就是下标
    {
        if (user_levels[i - 1])
            user_levels_lock[i] = 1;
        else
            user_levels_lock[i] = 0;
    }
    fusers_in.close();
}

int Load_menu()//载入目录数据
{
    /*cout << "请输入文件路径" << endl;
    getline(cin, fusers);*/
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
            Load_menu_datas();
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
    for (int i = 1; i < 4; i++)
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
    inputs.clear();

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
    for (int i = 0; i < count; i++)
        inputn[i] = stoi(inputs[i]);
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
            for (; lvname <= lvnum; lvname++)
                fusers_out << lvname << " " << user_levels[lvname] << endl;
            //存储用户通关数据 用户数据格式：关卡号+ +是否通关

            fusers_lstins.open("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\user_level" + lvplayed + ".txt");
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

void onUpdate(Scene* scene)
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
        display[i].setAnchor(0, 0.5f);
        display[i].setPos(1405, 250 + i * 30);
        scene->addChild(display + i);
    }
}



void load_instructions_from_keyboard_2() {//cmd读取
    try {
        //cin >> level;
        cout << "Input the number of instructions" << endl;
        cin >> instruction_count;
        if (instruction_count == 0)
        {
            cout << "Invalid Input!" << endl;
            return;
        }
        cin.ignore();
        instructions.resize(instruction_count);
        instruction.resize(instruction_count);
        cout << "Input  your instructions one by one" << endl;
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

void load_instructions_from_keyboard() {//GUI
    try {
        auto scene = gcnew Scene;
        SceneManager::enter(scene);

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

        SceneManager::back();
    }
    catch (...) {
        cout << "Invalid input." << endl;
    }
}

void load_instructions_from_file(const string& file_path, vector<INSTRUCTIONS>instr) {
    int tmp;
    try {
        ifstream file;
        file.open(file_path);
        if (!file) {
            cout << "File not found: " << file_path << endl;
            return;
        }
        //file >> tmp;
        if (file.eof())//可能用户第一次玩，没有数据
            return;

        instruction_count = 0;//加一个归零防止崩溃

        string ins_num;
        file >> instruction_count;
        //instruction_count = std::stoi(ins_num);
        file.ignore();
        instructions.resize(instruction_count);
        instruction.resize(instruction_count);
        getline(file, ins_num);
        for (int i = 0; i < instruction_count; i++) {

            getline(file,instruction[i]);

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
        file.close();
    }
    catch (...) {
        cout << "Invalid file format." << endl;
    }
}

int initial(/*vector<int> &input,vector<int> &target,vector<string> &availables*/)//初始化函数，关卡数据存储格式：输入函数、目标、可用指令；
{
    string instr;//存储指令

    /*cout << "Please enter the number of the level you want to play.\nEnter Quit to quit the game" << endl;
    cin >> levels;

    if (levels == "Quit")//退出游戏
    {
        quit = 1;
        return -1;
    }*/

    try {
        string path = "C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\text\\level" + levels + ".txt";
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
    for (int i = 0; i < input.size(); i++)
    {
        cout << input[i] << " ";
    }
    cout << "\ntargets\n";
    for (int i = 0; i < target.size(); i++)
    {
        cout << target[i] << " ";
    }
    cout << "\navailables:" << endl;
    for (int i = 0; i < availables.size(); i++)
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

void user_input_cons()
{
    string mode;
    cout << "Choose input mode (keyboard/file): ";
    cin >> mode;
    cin.ignore();
    if (mode == "keyboard")
        load_instructions_from_keyboard_2();
    else if (mode == "file") {
        string file_path;
        cout << "Enter the file path: ";
        getline(cin,file_path);
        load_instructions_from_file(file_path, instructions);
    }
    else {
        cout << "Invalid mode." << endl;
        return;
    }
}

void in_level(Scene* scene);

/*Load_menu();
SceneManager::back();
auto mnscene = new Scene;
create_menu(mnscene);*/

void cleanchild(Scene* scene);

static void refresh_menu();
static void create_choosefile(Scene* scene);

static void create_scene(easy2d::Scene* scene)
{
    // 进入 scene 场景
    SceneManager::enter(scene);
    Window::setTitle("HumanRescorceMachine");
    Window::setSize(1600, 900.5);
}

static void create_enter(easy2d::Scene* scene)
{
    //创建退出按钮
    auto quit = gcnew Sprite("rescorce/enter2.png");//必须在同一路径下！
    quit->setPos(650, 500);
    scene->addChild(quit);
    auto callback = [](ButtonEvent evt)
        {
            if (evt == ButtonEvent::Clicked)
            {
                SceneManager::back();
                auto filescene = new Scene;
                create_choosefile(filescene);
            }
        };
    // 给 sprite 添加监听器
    auto lis = gcnew ButtonListener(callback);
    quit->addListener(lis);
    // 游戏暂停时，让这个按钮继续工作
    lis->ignoreGamePaused();
}


void clear_lis(ShapeNode* newfilenode)
{
    newfilenode->removeAllListeners();
}

static void create_back(easy2d::Scene* scene)
{
    //创建退出按钮
    auto back = gcnew Sprite("rescorce/back2.png");//必须在同一路径下！
    scene->addChild(back);
    auto callback = [scene](ButtonEvent evt)
        {
            if (evt == ButtonEvent::Clicked)
            {
                //removeAllListeners();
                //clear_lis(newfilenode);
                cleanchild(scene);
                //SceneManager::back();
                upload_data(fusers);
                refresh_menu();
                //create_menu(menu_scene);
            }
        };
    // 给 sprite 添加监听器
    auto go_back = gcnew ButtonListener(callback);
    back->addListener(go_back);
    // 游戏暂停时，让这个按钮继续工作
    go_back->ignoreGamePaused();
}

static void create_quit(easy2d::Scene* scene)
{
    //创建退出按钮
    auto quits = gcnew Sprite("rescorce/quit2.png");//必须在同一路径下！
    quits->setAnchor(1.0f, 0.0f);//需要是小数！
    quits->setPos(1600, 0);
    scene->addChild(quits);
    auto callback = [](ButtonEvent evt)
        {
            if (evt == ButtonEvent::Clicked)
            {
                upload_data(fusers);
                // 点击了按钮，退出游戏
                Game::quit();
            }
        };
    // 给 sprite 添加监听器
    auto lis = gcnew ButtonListener(callback);
    quits->addListener(lis);
    // 游戏暂停时，让这个按钮继续工作
    lis->ignoreGamePaused();
}

static void create_Menutext(easy2d::Scene* scene)
{
    auto Menu = gcnew Text("M e n u", Font("Light", 50, Font::Weight::Thin, true));
    Menu->setFillColor(Color::OrangeRed);
    Menu->setStrokeColor(Color::OrangeRed);
    Menu->setStrokeWidth(1.5);
    Menu->setPos(750, 30);
    scene->addChild(Menu);

    /*auto newFtext = new Text("Please enter the cmd to see informations and instructions\nafter clicking the level", Font("Arial", 30)); // 使用默认字体Arial
    newFtext->setFillColor(Color::Black);
    newFtext->setAnchor(0.5f, 0.5f);
    newFtext->setPos(800, 800); // 将文字节点放置在矩形上方
    scene->addChild(newFtext);*/
}

static void create_Starttext(easy2d::Scene* scene)
{
    auto Menu = gcnew Text("Human Resource Machine", Font("Light", 80, Font::Weight::Thin, true));
    Menu->setFillColor(Color::Green);
    Menu->setStrokeColor(Color::Green);
    Menu->setStrokeWidth(3);
    Menu->setPos(400, 50);
    scene->addChild(Menu);
}

static void create_filetext(easy2d::Scene* scene)
{
    auto Menu = gcnew Text("Choose your file.", Font("Light", 50, Font::Weight::Thin, true));
    Menu->setFillColor(Color::Black);
    Menu->setStrokeColor(Color::Black);
    Menu->setStrokeWidth(1);
    Menu->setPos(400, 50);
    scene->addChild(Menu);
}

//新建存档按钮，待开发
static void create_newfile(int chosefile, easy2d::Scene* scene)
{
    auto newfile = gcnew Shape(Shape::Rect, Rect(Point(500, 700), Size(100, 40)));
    auto newfilenode = gcnew ShapeNode(newfile);
    newfilenode->setAnchor(0.5f, 0.5f);//需要是小数！
    newfilenode->setFillColor(Color::Yellow);
    scene->addChild(newfilenode);

    auto newFtext = gcnew Text("create file", Font("Arial",30)); // 使用默认字体Arial
    newFtext->setFillColor(Color::White);
    newFtext->setAnchor(0.5f, 0.5f);
    newFtext->setPos(500, 700); // 将文字节点放置在矩形上方
    scene->addChild(newFtext); // 将文字节点添加到当前节点


    auto callback = [chosefile](ButtonEvent evt)
        {
            if (evt == ButtonEvent::Clicked)
            {
                if (!has_file[chosefile])
                    has_file[chosefile] = 1;
                else
                    cout << "This file has already been created!" << endl;
            }
        };

    // 给 sprite 添加监听器
    auto lis = gcnew ButtonListener(callback);
    newfilenode->addListener(lis);
    // 游戏暂停时，让这个按钮继续工作
    lis->ignoreGamePaused();
}


bool run_ins(Scene* scene, Sprite* robot, Sprite* pointer0, Text* rb_text);
void run_2(Scene* scene);

//用户输入指令的按钮，因为游戏引擎缺陷，必须使用一个按钮来使关卡界面能够再用户输入指令前展示,内部指令已经放入run函数中
static void ready_input_cons(easy2d::Scene* scene, Sprite* robot, Sprite* pointer0, Text* rb_text)
{
    auto newfile = gcnew Shape(Shape::Rect, Rect(Point(800, 700), Size(250, 100)));
    auto newfilenode = gcnew ShapeNode(newfile);
    newfilenode->setAnchor(0.5f, 0.5f);//需要是小数！
    newfilenode->setFillColor(RGB(22, 81, 141));
    newfilenode->setStrokeColor(Color::Brown);
    newfilenode->setStrokeWidth(2.5);
    scene->addChild(newfilenode);

    auto newFtext = gcnew Text("Input your\ninstructions", Font("Arial", 30)); // 使用默认字体Arial
    newFtext->setFillColor(Color::White);
    newFtext->setAnchor(0.5f, 0.5f);
    newFtext->setPos(800, 690); // 将文字节点放置在矩形上方
    scene->addChild(newFtext); // 将文字节点添加到当前节点

    auto ntext = gcnew Text("Please input in the cmd.", Font("Arial", 15)); // 使用默认字体Arial
    ntext->setFillColor(Color::YellowGreen);
    ntext->setAnchor(0.5f, 0.5f);
    ntext->setPos(800, 735); 
    scene->addChild(ntext); // 将文字节点添加到当前节点

    auto callback = [newfilenode, scene, robot, pointer0, rb_text](ButtonEvent evt)
        {
            if (evt == ButtonEvent::Clicked)
            {
                user_input_cons();
                SceneManager::clear();
                SceneManager::back();
                auto level_run = new Scene;
                run_2(level_run);
            }
        };

    // 给 sprite 添加监听器
    auto lis = gcnew ButtonListener(callback);
    newfilenode->addListener(lis);
    // 游戏暂停时，让这个按钮继续工作
    //lis->autorelease();
    lis->ignoreGamePaused();
}

static void create_background(easy2d::Scene* scene, string str)
{
    auto background = gcnew Sprite(str);//必须在同一路径下！
    scene->addChild(background);
}

static void create_lights(int i, easy2d::Scene* scene)
{
    if (user_levels_lock[i])
    {
        auto light = new Shape(Shape::Circle, Point(500.0, 222.0 + 100 * i), 12);
        auto light_node = new ShapeNode(light);
        light_node->setStrokeColor(Color::Brown);
        light_node->setStrokeWidth(5.0);
        if (user_levels[i])
            light_node->setFillColor(Color::Yellow);
        else
            light_node->setFillColor(Color::Gray);
        scene->addChild(light_node);
    }
}

static void create_levels(easy2d::Scene* scene)
{
    for (int i = 1; i <= lvnum; i++)
    {
        auto rect1 = gcnew RectWithText(i,"level", scene);
        scene->addChild(rect1);
        create_lights(i, scene);
    }
}

static void create_files(easy2d::Scene* scene)//画files图
{
    for (int i = 1; i <=3; i++)
    {
        auto rect1 = gcnew RectWithText(i, "file", scene);
        scene->addChild(rect1);
    }
}

static void create_stmenu(easy2d::Scene* scene)
{
    create_scene(scene);
    create_background(scene, "rescorce/start1.jpg");
    create_quit(scene);
    create_Starttext(scene);
    create_enter(scene);
}

static void create_choosefile(easy2d::Scene* scene)
{
    create_scene(scene);
    create_background(scene, "rescorce/start1.jpg");
    create_quit(scene);
    create_filetext(scene);
    create_files(scene);
}//创建选择存档界面

static void create_menu(easy2d::Scene* scene)
{
    create_scene(scene);
    create_background(scene, "rescorce/background.jpg");
    create_quit(scene);
    create_Menutext(scene);
    create_levels(scene);

}

static void refresh_menu()
{
    fusers_in.open(fusers);//打开文件
    if (!fusers_in)
    {
        cout << "File cannot be open!" << endl;
        Game::quit;
    }
    Load_menu_datas();
    auto menu_scene = gcnew Scene;
    create_menu(menu_scene);
}

static void enter_level(Scene* level_scene)///
{
    spacenumber = initial(/*input,target,availables*/);//数据初始化
    space.resize(spacenumber);
    //auto level_scene = gcnew Scene;
    //create_scene(level_scene);
    //SceneManager::back();
    //SceneManager::enter(level_scene);
    in_level(level_scene);

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

//GUI
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
        for (int j = position + 1; j < input.size(); j++) {
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
        out_square[out_index]->open("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
        out_brick[out_index]->addChild(out_square[out_index]);
        out_num[out_index]->setText(num0);
        out_num[out_index]->setAnchor(0.5f, 0.5f);
        out_num[out_index]->setPos(25, 25);
        out_brick[out_index]->addChild(out_num[out_index]);
        out_brick[out_index]->setAnchor(0.5f, 0.5f);
        out_brick[out_index]->setPos(1220, 200);
        out_brick[out_index]->setVisible(false);
        scene->addChild(out_brick[out_index]);

        auto create_out = gcnew CallFunc([out_index]() {out_brick[out_index]->setVisible(true); });
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
        errortext = "Error on instruction " + std::to_string(i + 1);
        error_flag = true;
    }
}

void add(int i, int x, int out_index, Sequence* rb_se, Text* rb_text, Delay* delay) {
    if ((space[x] != INT_MIN) && (x >= 0) && (x < spacenumber)) {
        current_block += space[x];

        //GUI
        auto moveTo = gcnew MoveTo(2.5f, Point(500 + 120 * x, 350));
        string s = std::to_string(current_block);
        auto changeText = gcnew CallFunc([rb_text, s]() {rb_text->setText(s); });
        //auto inbox_act = gcnew Spawn({ moveTo, changeText });
        rb_se->add(moveTo);
        rb_se->add(changeText);
        wait_for_move(out_index, delay);

    }
    else {
        errortext = "Error on instruction " + std::to_string(i + 1);
        error_flag = true;
    }
}

void sub(int i, int x, int out_index, Sequence* rb_se, Text* rb_text, Delay* delay) {
    if ((space[x] != INT_MIN) && (x >= 0) && (x < spacenumber)) {
        current_block -= space[x];

        //GUI
        auto moveTo = gcnew MoveTo(2.5f, Point(500 + 120 * x, 350));
        string s = std::to_string(current_block);
        auto changeText = gcnew CallFunc([rb_text, s]() {rb_text->setText(s); });
        rb_se->add(moveTo);
        rb_se->add(changeText);
        wait_for_move(out_index, delay);
    }
    else {
        errortext = "Error on instruction " + std::to_string(i + 1);
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

        auto moveTo = gcnew MoveTo(2.5f, Point(500 + 120 * x, 350));
        string s = std::to_string(current_block);
        auto changeText = gcnew CallFunc([blk_text, s]() {blk_text->setText(s); });
        //auto inbox_act = gcnew Spawn({ moveTow, changeText });
        rb_se->add(moveTo);
        rb_se->add(changeText);
        wait_for_move(out_index, delay);
    }
    else {
        errortext = "Error on instruction " + std::to_string(i + 1);
        error_flag = true;
    }
}

void copyfrom(int i, int x, int out_index, Sequence* rb_se, Text* rb_text, Delay* delay) {
    if (space[x] != INT_MIN && x < spacenumber && x >= 0) {
        current_block = space[x];

        //GUI
        auto moveTo = gcnew MoveTo(2.5f, Point(500 + 120 * x, 350));
        string s = std::to_string(current_block);
        auto changeText = gcnew CallFunc([rb_text, s]() {rb_text->setText(s); });
        //auto inbox_act = gcnew Spawn({ moveTo, changeText });
        rb_se->add(moveTo);
        rb_se->add(changeText);
        wait_for_move(out_index, delay);
        //需要更改数字
    }

    else {
        errortext = "Error on instruction " + std::to_string(i + 1);
        error_flag = true;
    }
}

void jump(int& i, int x, easy2d::Sprite* robot) {//缺少指针动画
    if (x > 0 && x <= instruction_count) {
        i = x - 2;
    }
    else {
        errortext = "Error on instruction " + std::to_string(i + 1);
        error_flag = true;
    }
}

void jumpifzero(int& i, int x, easy2d::Sprite* robot) {//缺少指针动画
    if (current_block == 0) {
        if (x > 0 && x <= instruction_count)
            i = x - 2;
        else {
            errortext = "Error on instruction " + std::to_string(i + 1);
            error_flag = true;
        }
    }
    else {
        if (current_block == INT_MIN) {
            errortext = "Error on instruction " + std::to_string(i + 1);
            error_flag = true;
        }
    }
}

void jumpifnegative(int& i, int x, easy2d::Sprite* robot) {
    if (current_block < 0) {
        if (x > 0 && x <= instruction_count)
            i = x - 2;
        else {
            errortext = "Error on instruction " + std::to_string(i + 1);
            error_flag = true;
        }
    }
    else {
        if (current_block == INT_MIN) {
            errortext = "Error on instruction " + std::to_string(i + 1);
            error_flag = true;
        }
    }
}

void addstop(int i, Sequence* rb_se, Sequence* point_se, int pos)
{
    auto ani_pause = gcnew Delay(0.5);
    rb_se->add(ani_pause->clone());
    for (int j = pos; j < input.size(); j++) {
        bricks_ani[j]->add(ani_pause->clone());//保留传送带动画
    }
    for (int j = 0; j < out_bricks_ani.size(); j++) {
        out_bricks_ani[j]->add(ani_pause->clone());//保留传送带动画
    }
    auto movePointer = gcnew MoveTo(0.5f, Point(1405, 250 + i * 30));
    point_se->add(movePointer);
    wait_time += 0.5;
}/////
void addstop2(Sequence* rb_se, Sequence* point_se, int pos)
{
    auto ani_pause = gcnew Delay(0.5);
    rb_se->add(ani_pause->clone());
    //point_se->add(ani_pause->clone());
    for (int j = pos; j < input.size(); j++) {
        bricks_ani[j]->add(ani_pause->clone());//保留传送带动画
    }
    for (int j = 0; j < out_bricks_ani.size(); j++) {
        out_bricks_ani[j]->add(ani_pause->clone());//保留传送带动画
    }
    point_se->add(ani_pause->clone());
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

void check()
{
    if (output == target)
    {
        succeed = true;
        user_levels[level] = 1;//直接退出动画也能存档
    }

    else
    {
        succeed = false;
    }
}

void check1( Scene* scene)
{
    auto Judge = gcnew Text;
    Judge->setFont(Font("Palatino Linotype", 130));
    Judge->setAnchor(0.5f, 0.5f);
    Judge->setPos(800, 450);
    Judge->setFillColor(Color::OrangeRed);
    Judge->setStrokeColor(Color::OrangeRed);
    Judge->setStrokeWidth(5);
    if (succeed)
    {
        Judge->setText("Success");
    }

    else if(!succeed && !error_flag)
    {
        Judge->setText("Fail");
    }
    else if (error_flag)
    {
        Judge->setText(errortext);
    }
    scene->addChild(Judge);
}

//判断函数，重新开始游戏功能待开发
void after_ops(Scene* scene)
{
    if (succeed)
    {
        user_levels[level] = 1;
        /*SceneManager::back();
        cleanchild(scene);
        Game::reset();

        auto menu_scene = gcnew Scene;
        refresh_menu();
        create_menu(menu_scene);*/
        //break;
    }

    else//失败重新尝试,待开发
    {
        /*string retry;
        cout << "Enter R to retry." << endl;//文件输入暂时无法实现retry功能
        cin >> retry;
        if (retry == "R")//重新开始游戏功能待开发
        {
            SceneManager::back();
            cleanchild(scene);
            Game::reset();
            enter_level();
            /*auto menu_scene = gcnew Scene;
            refresh_menu();
            create_menu(menu_scene);
        }
        else
        {
            SceneManager::back();
            cleanchild(scene);
            Game::reset();
            auto menu_scene = gcnew Scene;
            refresh_menu();
            create_menu(menu_scene);
            //break;
        }*/

    }
}

//inlevel
bool run_ins(Scene* scene, Sprite* robot, Sprite* pointer0, Text* rb_text)
{
    onUpdate(scene);

    int i = 0, out_index = 0;

    auto rb_se = gcnew Sequence();
    auto point_se = gcnew Sequence();
    auto stopp = gcnew Delay(2.5);//每一步的延迟2.5s
    auto del2 = gcnew Delay(0.5);//每一步的延迟2.5s

    out_brick[0] = new Node();
    out_square[0] = new Sprite;
    out_num[0] = new Text;


    while (i < instruction_count) {
        addstop(i, rb_se, point_se, position);//指针移动

        if (error_flag)
            break;

        if (instructions[i].name == "inbox") {
            inbox(i, out_index, rb_se, rb_text, stopp);//3s
            point_se->add(del2->clone());
            point_se->add(stopp->clone());
        }
        else if (instructions[i].name == "outbox") {
            outbox(i, out_index, rb_se, scene, rb_text, stopp);//3s
            point_se->add(del2->clone());
            point_se->add(stopp->clone());
            out_index++;//画图用
            destination_count++;
        }
        else if (instructions[i].name == "add" && level != 1 && instructions[i].is_num_int) {
            add(i, instructions[i].number, out_index, rb_se, rb_text, stopp);
            point_se->add(stopp->clone());
        }
        else if (instructions[i].name == "sub" && level != 1 && instructions[i].is_num_int) {
            sub(i, instructions[i].number, out_index, rb_se, rb_text, stopp);
            point_se->add(stopp->clone());
        }
        else if (instructions[i].name == "copyto" && level != 1 && instructions[i].is_num_int) {
            copyto(i, instructions[i].number, out_index, rb_se, space_num[instructions[i].number], stopp);
            point_se->add(stopp->clone());
        }
        else if (instructions[i].name == "copyfrom" && level != 1 && instructions[i].is_num_int) {
            copyfrom(i, instructions[i].number, out_index, rb_se, rb_text, stopp);
            point_se->add(stopp->clone());
        }
        else if (instructions[i].name == "jump" && level != 1 && instructions[i].is_num_int) {
            jump(i, instructions[i].number, robot);
        }
        else if (instructions[i].name == "jumpifzero" && level != 1 && instructions[i].is_num_int) {
            jumpifzero(i, instructions[i].number, robot);
        }
        else if (instructions[i].name == "jumpifnegative" && level == 4 && instructions[i].is_num_int) {
            jumpifnegative(i, instructions[i].number, robot);
        }
        else {
            errortext = "Error on instruction " + std::to_string(i + 1);
            return true;
        }

        if (inbox_done)
            break;
        i++;
    }

    if (error_flag == true)
    {
        //rb_se->release();
        succeed = false;
        //auto rb_se = gcnew Sequence;
    }
    addstop2(rb_se, point_se, position);

    check();

    auto checks = gcnew CallFunc([scene]() {check1(scene); });
    auto checkn = gcnew CallFunc([scene]() {after_ops(scene); });
    rb_se->add(checks);
    rb_se->add(stopp);//停留2.5秒
    rb_se->add(checkn);
    robot->runAction(rb_se->clone());
    pointer0->runAction(point_se->clone());
    for (int i = 0; i < bricks_ani.size(); i++)
    {
        brick[i]->runAction(bricks_ani[i]->clone());
    }
    for (int i = 0; i < out_bricks_ani.size(); i++)
    {
        out_brick[i]->runAction(out_bricks_ani[i]->clone());
    }

    rb_se->release();
    //Game::pause();

    if (error_flag == false)
        return true;
}

void run(Scene* scene) {

    //Game::pause();
    auto bg = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\levelbg.png");
    scene->addChild(bg);

    create_quit(scene);
    //create_back(scene);

    auto belt1 = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\csdd.png");
    belt1->setAnchor(0, 1.0f);
    belt1->setPos(200, 750);
    auto belt2 = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\csd.png");
    belt2->setAnchor(1.0f, 1.0f);
    belt2->setPos(1300, 750);
    scene->addChild(belt1);
    scene->addChild(belt2);

    auto robot = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\robot3.png");
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
    auto pointer0 = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\pointer0.png");
    scene->addChild(pointer0);
    pointer0->setAnchor(1, 0.5f);
    pointer0->setPos(1405, 250);

    string INPUT, TARGET;//input和target
    INPUT = arrayToString(input);
    TARGET = arrayToString(target);

    auto newFtext = new Text("Please enter the cmd to see informations and instructions after clicking the level.\nAfter entering, go back to GUI to enjoy animation.", Font("Arial", 30)); // 使用默认字体Arial
    newFtext->setFillColor(Color::Black);
    newFtext->setAnchor(0.5f, 0.5f);
    newFtext->setPos(800, 800); // 将文字节点放置在矩形上方
    scene->addChild(newFtext);

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
        prepare_inbricks(2);

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




        for (int i = 0; i < 2; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
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

        //onUpdate(scene);

        /*auto display = gcnew Text[instruction_count];
        for (int i = 0; i < instruction_count; i++) {
            display[i].setText(instruction[i]);
            display[i].setAnchor(0, 0.5f);
            display[i].setPos(1405, 250 + i * 30);
            scene->addChild(display + i);
        }*/

        /*scene->addChild(pointer0);
        pointer0->setAnchor(1, 0.5f);
        pointer0->setPos(1405, 250);*/

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
            auto a = gcnew Shape(Shape::Rect, Rect(Point(500 + 120 * i, 500), Size(70, 70)));
            SPAces[i] = gcnew ShapeNode(a);
            SPAces[i]->setFillColor(Color::Chocolate);
            space_num[i] = gcnew Text("111");
            space_num[i]->setFont(Font("Arial", 35));
            space_num[i]->setFillColor(Color::White);
            space_num[i]->setAnchor(0.5f, 0.5f);
            space_num[i]->setPos(525 + 120 * i, 525);
            SPAces[i]->addChild(space_num[i]);
            scene->addChild(SPAces[i]);
        }

        for (int i = 0; i < 8; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
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
        }

        scene->addChild(pointer0);
        pointer0->setAnchor(1, 0.5f);
        pointer0->setPos(1405, 250);*/

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
            auto a = gcnew Shape(Shape::Rect, Rect(Point(500 + 120 * i, 500), Size(70, 70)));
            SPAces[i] = gcnew ShapeNode(a);
            SPAces[i]->setFillColor(Color::Chocolate);
            space_num[i] = gcnew Text("111");
            space_num[i]->setFont(Font("Arial", 35));
            space_num[i]->setFillColor(Color::White);
            space_num[i]->setAnchor(0.5f, 0.5f);
            space_num[i]->setPos(525 + 120 * i, 525);
            SPAces[i]->addChild(space_num[i]);
            scene->addChild(SPAces[i]);
        }

        for (int i = 0; i < 8; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
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
        }

        scene->addChild(pointer0);
        pointer0->setAnchor(1, 0.5f);
        pointer0->setPos(640, 150);*/
        //end GUI
    }
    else if (level == 4) {
        prepare_inbricks(8);
        space.resize(spacenumber, INT_MIN);

        //begin GUI
        auto text = new Text("第四关：计算两个数的最大公约数。（提示：使用辗转相减法）");
        scene->addChild(text);
        text->setAnchor(0.5f, 0.5f);
        text->setPos(800, 70);

        for (int i = 0; i < 3; i++)
        {
            auto a = gcnew Shape(Shape::Rect, Rect(Point(500 + 120 * i, 500), Size(70, 70)));
            SPAces[i] = gcnew ShapeNode(a);
            SPAces[i]->setFillColor(Color::Chocolate);
            space_num[i] = gcnew Text("111");
            space_num[i]->setFont(Font("Arial", 35));
            space_num[i]->setFillColor(Color::White);
            space_num[i]->setAnchor(0.5f, 0.5f);
            space_num[i]->setPos(525 + 120 * i, 525);
            SPAces[i]->addChild(space_num[i]);
            scene->addChild(SPAces[i]);
        }

        for (int i = 0; i < 2; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
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

        /*scene->addChild(robot);
        robot->setAnchor(0.5f, 0.5f);
        robot->setPos(180, 200);


        scene->addChild(pointer0);
        pointer0->setAnchor(1, 0.5f);
        pointer0->setPos(640, 150);*/


        //end GUI
    }
    SceneManager::enter(scene);
    show_init();//显示界面

    ready_input_cons(scene, robot, pointer0, rb_text);
    //user_input_cons();
    //run_ins(scene, robot, pointer0, rb_text);
    /*onUpdate(scene);

    int i = 0, out_index = 0;

    auto rb_se = gcnew Sequence();
    auto point_se = gcnew Sequence();
    auto stopp = gcnew Delay(2.5);//每一步的延迟2.5s
    auto del2 = gcnew Delay(0.5);//每一步的延迟2.5s

    out_brick[0] = new Node();
    out_square[0] = new Sprite;
    out_num[0] = new Text;


    while (i < instruction_count) {
        addstop(i, rb_se, point_se, position);//指针移动

        if (error_flag)
            break;

        if (instructions[i].name == "inbox") {
            inbox(i, out_index, rb_se, rb_text, stopp);//3s
            point_se->add(del2->clone());
        }
        else if (instructions[i].name == "outbox") {
            outbox(i, out_index, rb_se, scene, rb_text, stopp);//3s
            point_se->add(del2->clone());
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
            errortext =  "Error on instruction "+std::to_string(i+1);
            return true;
        }

        point_se->add(stopp->clone());

        if (inbox_done)
            break;
        i++;
        //if (error_flag == false) {
            //auto movePointer = gcnew MoveTo(0.5f,Point(640, 150 + i * 15));
            //point_se->add(movePointer);
        }
    }

    if (error_flag == true)
    {
        //rb_se->release();
        succeed = false;
        //auto rb_se = gcnew Sequence;
    }
    addstop2(rb_se, point_se, position);

    check();

    auto checks = gcnew CallFunc([scene]() {check1(scene); });
    auto checkn = gcnew CallFunc([scene]() {after_ops(scene); });
    rb_se->add(checks);
    rb_se -> add(stopp);//停留2.5秒
    rb_se->add(checkn);
    robot->runAction(rb_se->clone());
    pointer0->runAction(point_se->clone());
    for (int i = 0; i < bricks_ani.size(); i++)
    {
        brick[i]->runAction(bricks_ani[i]->clone());
    }
    for (int i = 0; i < out_bricks_ani.size(); i++)
    {
        out_brick[i]->runAction(out_bricks_ani[i]->clone());
    }

    rb_se->release();
    //Game::pause();

    if (error_flag == false)
        return true;*/

}
void run_2(Scene* scene) {

    //Game::pause();
    auto bg = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\levelbg.png");
    scene->addChild(bg);

    create_quit(scene);
    create_back(scene);

    auto belt1 = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\csdd.png");
    belt1->setAnchor(0, 1.0f);
    belt1->setPos(200, 750);
    auto belt2 = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\csd.png");
    belt2->setAnchor(1.0f, 1.0f);
    belt2->setPos(1300, 750);
    scene->addChild(belt1);
    scene->addChild(belt2);

    auto robot = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\robot3.png");
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
    auto pointer0 = gcnew Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\pointer0.png");
    scene->addChild(pointer0);
    pointer0->setAnchor(1, 0.5f);
    pointer0->setPos(1405, 250);

    string INPUT, TARGET;//input和target
    INPUT = arrayToString(input);
    TARGET = arrayToString(target);



    if (level == 1) {
        prepare_inbricks(2);

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




        for (int i = 0; i < 2; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
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
            auto a = gcnew Shape(Shape::Rect, Rect(Point(500 + 120 * i, 500), Size(70, 70)));
            SPAces[i] = gcnew ShapeNode(a);
            SPAces[i]->setFillColor(Color::Chocolate);
            space_num[i] = gcnew Text("111");
            space_num[i]->setFont(Font("Arial", 35));
            space_num[i]->setFillColor(Color::White);
            space_num[i]->setAnchor(0.5f, 0.5f);
            space_num[i]->setPos(525 + 120 * i, 525);
            SPAces[i]->addChild(space_num[i]);
            scene->addChild(SPAces[i]);
        }

        for (int i = 0; i < 8; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
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
            auto a = gcnew Shape(Shape::Rect, Rect(Point(500 + 120 * i, 500), Size(70, 70)));
            SPAces[i] = gcnew ShapeNode(a);
            SPAces[i]->setFillColor(Color::Chocolate);
            space_num[i] = gcnew Text("111");
            space_num[i]->setFont(Font("Arial", 35));
            space_num[i]->setFillColor(Color::White);
            space_num[i]->setAnchor(0.5f, 0.5f);
            space_num[i]->setPos(525 + 120 * i, 525);
            SPAces[i]->addChild(space_num[i]);
            scene->addChild(SPAces[i]);
        }

        for (int i = 0; i < 8; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
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

    }
    else if (level == 4) {
        prepare_inbricks(8);
        space.resize(spacenumber, INT_MIN);

        //begin GUI
        auto text = new Text("第四关：计算两个数的最大公约数。（提示：使用辗转相减法）");
        scene->addChild(text);
        text->setAnchor(0.5f, 0.5f);
        text->setPos(800, 70);

        for (int i = 0; i < 3; i++)
        {
            auto a = gcnew Shape(Shape::Rect, Rect(Point(500 + 120 * i, 500), Size(70, 70)));
            SPAces[i] = gcnew ShapeNode(a);
            SPAces[i]->setFillColor(Color::Chocolate);
            space_num[i] = gcnew Text("111");
            space_num[i]->setFont(Font("Arial", 35));
            space_num[i]->setFillColor(Color::White);
            space_num[i]->setAnchor(0.5f, 0.5f);
            space_num[i]->setPos(525 + 120 * i, 525);
            SPAces[i]->addChild(space_num[i]);
            scene->addChild(SPAces[i]);
        }

        for (int i = 0; i < 2; i++) {
            brick[i] = new Node();
            square[i] = new Sprite("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\rescorce\\block4.png");
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

        //end GUI
    }
    SceneManager::enter(scene);
    show_init();//显示界面

    run_ins(scene, robot, pointer0, rb_text);

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

void in_level(Scene* scene)
{
    succeed = 0;
    error_flag = false;
    const string last_inp = "C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\text\\user_level" + levels + ".txt";
    //load_instructions_from_file(last_inp, last_instructions);//按行存储上一次的指令

    /*show_init();//显示界面

    user_input_cons();*/

    succeed = 0;
    position = 0;
    output.clear();
    output.resize(0);
    run(scene);
    //SceneManager::enter(scene);



    //Game::start();
    //if (quit)
        //Game::quit();
        //以下别碰，我也不知道怎么跑起来的……

        
}

/*void in_level(Scene* scene)
{
    succeed = 0;
    draw_level(scene);
    /*while (!succeed)//游戏进行中
    {

        //Game::init();
        error_flag = false;
        const string last_inp = "C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\text\\user_level" + levels + ".txt";
        load_instructions_from_file(last_inp, last_instructions);//按行存储上一次的指令

        show_init();//显示界面

        user_input_cons();

        /*Window::setTitle("111");
        Window::setSize(1600, 900);
        auto scene = new Scene;
        SceneManager::enter(scene);

        succeed = 0;
        position = 0;
        output.clear();
        output.resize(0);
        bool no_error = run(scene);

        if (!no_error)
            succeed = false;


        //以下别碰，我也不知道怎么跑起来的……
        //Game::pause();
        string retry;
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
    //delete(pointer0);
}*/



int main()
{
	//以下用于在游戏开始前配置游戏（比如窗口什么的）
    if (Game::init())
    {
        /* 设计游戏内容 */
        // 创建一个空场景

        //Load_menu();
        auto start = gcnew Scene;
        create_stmenu(start);
        //create_choosefile(start);
        Game::start();
        //Game::quit();

        //正式开始运行关卡
        

    // 释放内存
    }
	Game::destroy();//回收游戏资源
	return 0;
}
