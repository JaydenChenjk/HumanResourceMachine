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
int lvnum = 0;
int instruction_count = 0;
int current_block = INT_MIN;
int level= 0;
int spacenumber = 3;//空间数
bool have_file[5] = { 0 };
bool succeed = 0, quit = 0;
bool error_flag = false;
bool inbox_done = false;
bool to_menu = false, to_level = false;
int position = 0;

/*Tips
C++ 中的字符串有很多种类型，因为 Easy2D 使用 Unicode 字符集，
所以必须在字符串前加一个前缀 ‘L’ 来标志这个字符串是 Unicode 编码的。//但这里加L会报错，不知道为什么
*/




static bool enter_level();
// 定义一个自定义类，继承自Node
class RectWithText : public Node {
public:
    // 构造函数
    RectWithText(int i, string type)
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
        }
        else if (type == "file")
        {
            point1 = Point(-400.0 + 500 * i, 300.0);
            point2 = Point(-300 + 500 * i, 408.0);
            size = Size(400.0, 300.0);
            text = "F i l e ";
            font = Font("Palatino Linotype", 50);

        }


        auto button = new Shape(Shape::Rect, Rect(point1,size ));
        level_button = new ShapeNode(button);
        if (user_levels_lock[i])
            level_button->setFillColor(Color::ForestGreen);
        else
            level_button->setFillColor(Color::Gray);
        level_button->setStrokeColor(Color::DarkGreen);
        level_button->setStrokeWidth(6.0);
        this->addChild(level_button); // 将矩形节点添加到当前节点

        // 创建文字标签
        string level_m = std::to_string(i); // 将整数level转换为字符
        std::string levelStr = text + level_m;
        level_text = new Text(levelStr, font); // 使用默认字体Arial
        level_text->setFillColor(Color::White);
        level_text->setPos(point2); // 将文字节点放置在矩形上方
        this->addChild(level_text); // 将文字节点添加到当前节点

        auto callback = [level_m,type](ButtonEvent evt)
            {
                if (evt == ButtonEvent::Clicked)
                {
                    if (type == "level")
                    {
                        levels = level_m;
                        //打开新场景
                        enter_level();
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

//innerlogic
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
            for (; lvname < 4; lvname++)
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

static void create_menu(easy2d::Scene* scene);
static void refresh_menu();


/*Load_menu();
SceneManager::back();
auto mnscene = new Scene;
create_menu(mnscene);*/


static void create_scene(easy2d::Scene* scene)
{
    // 进入 scene 场景
    SceneManager::enter(scene);
    Window::setTitle("HumanRescorceMachine");
    Window::setSize(1690, 900);
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
                create_menu(filescene);
            }
        };
    // 给 sprite 添加监听器
    auto lis = gcnew ButtonListener(callback);
    quit->addListener(lis);
    // 游戏暂停时，让这个按钮继续工作
    lis->ignoreGamePaused();
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
                SceneManager::back();
                auto menu_scene = gcnew Scene;
                refresh_menu();
                create_menu(menu_scene);
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
    auto quit = gcnew Sprite("rescorce/quit2.png");//必须在同一路径下！
    quit->setAnchor(1.0f, 0.0f);//需要是小数！
    quit->setPos(1690, 0);
    scene->addChild(quit);
    auto callback = [](ButtonEvent evt)
        {
            if (evt == ButtonEvent::Clicked)
            {
                // 点击了按钮，退出游戏
                Game::quit();
            }
        };
    // 给 sprite 添加监听器
    auto lis = gcnew ButtonListener(callback);
    quit->addListener(lis);
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
        auto rect1 = gcnew RectWithText(i,"level");
        scene->addChild(rect1);
        create_lights(i, scene);
    }
}

static void create_files(easy2d::Scene* scene)//画files图
{
    for (int i = 1; i <=3; i++)
    {
        auto rect1 = gcnew RectWithText(i, "file");
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
    Load_menu_datas();
    auto menu_scene = gcnew Scene;
    create_menu(menu_scene);
}

static bool enter_level()
{
    auto level_scene = new Scene;
    create_scene(level_scene);
    create_quit(level_scene);
    create_back(level_scene);

    SceneManager::back();
    SceneManager::enter(level_scene);
    return 1;
}



int main()
{
	//以下用于在游戏开始前配置游戏（比如窗口什么的）
    if (Game::init())
    {
        /* 设计游戏内容 */
        // 创建一个空场景

        Load_menu();
        auto start = gcnew Scene;
        create_stmenu(start);
        //create_choosefile(start);
        Game::start();

    // 释放内存
    }
	Game::destroy();//回收游戏资源
	return 0;
}