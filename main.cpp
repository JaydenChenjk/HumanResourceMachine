#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

int level;
vector<int> input;
vector<int> target;
int current_block = -65536;
vector<int> output;
vector<int> space;
int instruction_count;
int position = 0;
bool error_flag = false;
struct INSTRUCTION {
    string name;
    int number;
};

vector<INSTRUCTION> instructions;

void load_instructions_from_keyboard();
void inbox(int i);
void outbox(int i);
void add_op(int i, int x);
void sub_op(int i, int x);
void copyto(int i, int x);
void copyfrom(int i, int x);
void jump(int& i, int x);
void jumpifzero(int& i, int x);
void run();

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
        int number = -65536;

        ss >> name;
        if (ss >> number)
            instructions[i].number = number;

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
    if (current_block != -65536) {
        output.push_back(current_block);
        current_block = -65536;
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void add_op(int i, int x) {
    if (x >= 0 && x < space.size() && space[x] != -65536)
        current_block += space[x];
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void sub_op(int i, int x) {
    if (x >= 0 && x < space.size() && space[x] != -65536)
        current_block -= space[x];
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void copyto(int i, int x) {
    if (x >= 0 && x < space.size()) {
        if (current_block != -65536)
            space[x] = current_block;
        else {
            cout << "Error on instruction " << (i + 1) << endl;
            error_flag = true;
        }
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void copyfrom(int i, int x) {
    if (x >= 0 && x < space.size()) {
        if (space[x] != -65536)
            current_block = space[x];
        else {
            cout << "Error on instruction " << (i + 1) << endl;
            error_flag = true;
        }
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void jump(int& i, int x) {
    if (x >= 0 && x < instruction_count) {
        i = x - 1;
    }
    else {
        cout << "Error on instruction " << (i + 1) << endl;
        error_flag = true;
    }
}

void jumpifzero(int& i, int x) {
    if (current_block == 0) {
        if (x >= 0 && x < instruction_count)
            i = x - 1;
        else {
            cout << "Error on instruction " << (i + 1) << endl;
            error_flag = true;
        }
    }
    else {
        if (current_block == -65536) {
            cout << "Error on instruction " << (i + 1) << endl;
            error_flag = true;
        }
    }
}

void run() {
    if (level == 1) {
        input = {1, 2};
        target = {1, 2};
    }
    else if (level == 2) {
        input = {3, 9, 5, 1, -2, -2, 9, -9};
        target = {-6, 6, 4, -4, 0, 0, 18, -18};
    }
    else if (level == 3) {
        input = {6, 2, 7, 7, -9, 3, -3, -3};
        target = {7, -3};
    }

    int i = 0;
    while (i < instruction_count) {
        if (error_flag)
            break;

        if (instructions[i].name == "inbox") {
            inbox(i);
        }
        else if (instructions[i].name == "outbox") {
            outbox(i);
        }
        else if (instructions[i].name == "add") {
            add_op(i, instructions[i].number);
        }
        else if (instructions[i].name == "sub") {
            sub_op(i, instructions[i].number);
        }
        else if (instructions[i].name == "copyto") {
            copyto(i, instructions[i].number);
        }
        else if (instructions[i].name == "copyfrom") {
            copyfrom(i, instructions[i].number);
        }
        else if (instructions[i].name == "jump") {
            jump(i, instructions[i].number);
        }
        else if (instructions[i].name == "jumpifzero") {
            jumpifzero(i, instructions[i].number);
        }
        else {
            cout << "Error on instruction " << (i + 1) << endl;
            error_flag = true;
        }

        i++;
    }

    if (error_flag)
        return;

    if (output == target)
        cout << "Success" << endl;
    else
        cout << "Fail" << endl;
}

int main(){
    load_instructions_from_keyboard();

    space.assign(3, -65536);

    run();

    return 0;
}