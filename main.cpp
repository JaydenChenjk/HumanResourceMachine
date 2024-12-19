#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <climits>

using namespace std;

int level;
vector<int> input;
vector<int> target;
vector<string> availables;
int current_block=INT_MIN;
vector<int> output;
vector<int> space;
int instruction_count;
int position = 0;
int spacenumber = 0;
bool error_flag = false;
int rotatetimes = 0;

struct INSTRUCTIONS {
    string name;
    int number;
    bool is_num_int = 1;
};
vector<INSTRUCTIONS> instructions;

void load_instructions_from_keyboard() {
    cin >> level;
    cin >> instruction_count;
    cin.ignore();
    instructions.resize(instruction_count);
    for (int i = 0; i < instruction_count; i++) {
        string instruction;
        getline(cin, instruction);

        stringstream ss(instruction);
        string name;
        double number = INT_MIN;

        ss >> name;
        if (ss >> number){
            if(name != "inbox"&& name != "outbox"){
                instructions[i].number = int(number);
                if(floor(number) != number || floor(number)<0)
                    instructions[i].is_num_int = 0;
            }
            else
                instructions[i].is_num_int = 0;//指令数不符
        }
        if (ss >> number){
            instructions[i].is_num_int = 0;//指令数不符
        }

        instructions[i].name = name;
    }
}


void inbox(int i) {
    if (position < input.size()) {
        current_block = input[position];
        position++;
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void outbox(int i) {
    if (current_block != INT_MIN) {
        output.push_back(current_block);
        current_block = INT_MIN;
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void add(int i, int x) {
    if ((space[x]!=INT_MIN)&&(x>=0)&&(x<spacenumber))
        current_block+=space[x];
    else{
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void sub(int i, int x) {
    if ((space[x]!=INT_MIN)&&(x>=0)&&(x<spacenumber))
        current_block-=space[x];
    else{
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void copyto(int i, int x) {
    if (current_block != INT_MIN && x<spacenumber && x>=0)
        space[x]=current_block;
    else{
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void copyfrom(int i, int x) {
    if (space[x] != INT_MIN && x<spacenumber && x>=0)
        current_block=space[x];
    else{
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void jump(int& i, int x) {
    if (x > 0 && x <= instruction_count) {
        i = x - 2;
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void jumpifzero(int& i, int x) {
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


void run() {
    if (level == 1) {
        input = {1, 2};
        target = {1, 2};
        availables = {"inbox","outbox"};
        spacenumber= 0;
        space.resize(spacenumber);
    } else if (level == 2) {
        input = {3, 9, 5, 1, -2, -2, 9, -9};
        target = {-6, 6, 4, -4, 0, 0, 18, -18};
        availables = {"inbox","outbox","add","sub","copyto","copyfrom","jump","jumpifzero"};
        spacenumber= 3;
        space.resize(spacenumber,INT_MIN);
    } else if (level == 3) {
        input = {6, 2, 7, 7, -9, 3, -3, -3};
        target = {7, -3};
        availables = {"inbox","outbox","add","sub","copyto","copyfrom","jump","jumpifzero"};
        spacenumber= 3;
        space.resize(spacenumber,INT_MIN);
    }

    int i = 0;
    while (i < instruction_count) {
        if (error_flag)
            break;
        if (instructions[i].name == "inbox") {
            inbox(i);
        } else if (instructions[i].name == "outbox") {
            outbox(i);
        } else if (instructions[i].name == "add"&&level != 1&&instructions[i].is_num_int) {
            add(i, instructions[i].number);
        } else if (instructions[i].name == "sub"&&level != 1&&instructions[i].is_num_int) {
            sub(i, instructions[i].number);
        } else if (instructions[i].name == "copyto"&&level != 1&&instructions[i].is_num_int) {
            copyto(i, instructions[i].number);
        } else if (instructions[i].name == "copyfrom"&&level != 1&&instructions[i].is_num_int) {
            copyfrom(i, instructions[i].number);
        } else if (instructions[i].name == "jump"&&level != 1&&instructions[i].is_num_int) {
            jump(i, instructions[i].number);
        } else if (instructions[i].name == "jumpifzero"&&level != 1&&instructions[i].is_num_int) {
            jumpifzero(i, instructions[i].number);
        } else {
            cout << "Error on instruction " << (i + 1) << endl;
            return;
        }

        i++;
    }

    if (error_flag==true)
        return;

    if (output == target)
        cout << "Success" << endl;
    else
        cout << "Fail" << endl;
}

int main()
{
    load_instructions_from_keyboard();

    run();

    return 0;
}