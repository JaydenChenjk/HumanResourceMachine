#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

vector<int> input;
vector<int> target;
int current_block;
vector<int> output;
vector<int> spaces;
int instruction_count;
int index = 0;

struct INSTRUCTIONS {
    string name;
    int number;
};
vector<INSTRUCTIONS> instructions(instruction_count);

void load_instructions_from_keyboard() {
    try {
        cout << "Enter the number of instructions: ";
        cin >> instruction_count;
        cin.ignore();
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

void load_instructions_from_file(const string& file_path) {
    try {
        ifstream file(file_path);
        if (!file) {
            cout << "File not found: " << file_path << endl;
            return;
        }
        file >> instruction_count;
        file.ignore();
        instructions.resize(instruction_count);
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
        cout << "Invalid file format." << endl;
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



void run1() {
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
        cout << "Success" << endl;
    else
        cout << "Fail" << endl;
}

void run2() {
    for (int i = 0; i < instructions.size(); i++) {
        cout << (i + 1) << " " << instructions[i].name;
        if(instructions[i].number!=-1)
            cout << " " << instructions[i].number;
        cout << endl;

        if (instructions[i].name == "inbox")
            inbox(i);
        else if (instructions[i].name == "outbox")
            outbox(i);
        else if (instructions[i].name == "add")
            add(i);
        else
            cout << "Error on instruction " << (i + 1) << endl;
    }

    if (output == target)
        cout << "Success" << endl;
    else
        cout << "Fail" << endl;
}

int main() {
    vector<int> input = {1, 2};
    vector<int> target = {1, 2};
    for(int i=0;i<instruction_count;i++) {
        instructions[i].name=-1;
    }

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
        load_instructions_from_file(file_path);
    }
    else {
        cout << "Invalid mode." << endl;
        return 0;
    }

    run1();

    return 0;
}
