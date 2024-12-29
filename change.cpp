void load_level4()
{
    std::default_random_engine e;
    std::uniform_int_distribution<int> u(-9,9); // 左闭右闭区间
    e.seed(time(0));
    input.resize(8);
    target.resize(4);
    for (int i=0; i<8; i++) {
        do
        {
            input[i] = u(e);
        }while(!input[i]);
    }

    for(int i = 0;i<=6;i+=2)
    {
        if(input[i]*input[i+1]>0)
            target[i/2] = 0;
        else if(input[i]*input[i+1] <0)
            target[i/2] = 1;
    }

    availables = {"inbox","outbox","add","sub","copyto","copyfrom","jump","jumpifzero"};

    spacenumber = 6;
}

int initial(/*vector<int> &input,vector<int> &target,vector<string> &availables*/)//初始化函数，关卡数据存储格式：输入函数、目标、可用指令；
{
    string instr;//存储指令

    cout<<"Please enter the number of the level you want to play.\nEnter Quit to quit the game"<<endl;
    cin>>levels;
    if(levels == "Quit")//退出游戏
    {
        quit = 1;
        return -1;
    }

    try {
        string path = "C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\text\\level"+levels+".txt";
        //flevel_in.open("C:\\Users\\fyh_1\\Desktop\\programme\\cxsjjc\\Ming's robot\\VSaccestories\\Project1\\text\\level0.txt");
        //flevel_in.close();
        flevel_in.open(path);//打开关卡存储文件
        if(!flevel_in)//判断能否打开;但如果打开路径和原本的路径一样，没有“打开”操作，所以会认为“打开失败”
        {
            cout<<"Unable to find this level! Please check if there's any spelling mistake."<<endl;
            return -1;
        }

        else
        {
            level = stoi(levels);//转化关卡号为数字
            if(levels == "1")
                level = 1;

            if(level<1||level>5)//输入关卡号错误情况
            {
                cout<<"关卡号输入错误！请重新输入。";
                return -1;//如果为-1则一直在循环里！（do_while)
            }
            else if(!user_levels_lock[level])
            {
                cout<<"This level is locked! You need to pass all the previous levels to unlock."<<endl;
                flevel_in.close();
                return -1;
            }

            else if (level == 4)
            {
                load_level4();
                flevel_in.close();
                return spacenumber;
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

                cin>>spacenumber;
                flevel_in>>tmp;

                if(!flevel_in.eof())
                    cout<<"Please don't change this file!"<<endl;
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
