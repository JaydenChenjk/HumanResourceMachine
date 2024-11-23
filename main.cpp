#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

vector<int> input;
vector<int> target;
int current_block;
vector<int> output;
vector<string> availables;
vector<int> spaces;
int instruction_count;
int index = 0;

void load_availables_from_keyboard() {
    try {
        cout << "Enter the number of availables: ";
        cin >> instruction_count;
        cin.ignore();
        cout << "Enter your availables one by one (e.g., inbox, outbox):" << endl;
        for (int i = 0; i < instruction_count; i++) {
            string instruction;
            getline(cin, instruction);
            availables.push_back(instruction);
        }
    }
    catch (...) {
        cout << "Invalid input." << endl;
    }
}

void load_availables_from_file(const string& file_path) {
    try {
        ifstream file(file_path);
        if (!file) {
            cout << "File not found: " << file_path << endl;
            return;
        }
        file >> instruction_count;
        file.ignore();
        availables.resize(instruction_count);
        for (int i = 0; i < instruction_count; i++) {
            getline(file, availables[i]);
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
    for (int i = 0; i < availables.size(); i++) {
        cout << (i + 1) << " " << availables[i] << endl;

        if (availables[i] == "inbox")
            inbox(i);
        else if (availables[i] == "outbox")
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

}

int main() {
    vector<int> input = {1, 2};
    vector<int> target = {1, 2};

    string mode;
    cout << "Choose input mode (keyboard/file): ";
    cin >> mode;
    cin.ignore();
    if (mode == "keyboard")
        load_availables_from_keyboard();
    else if (mode == "file") {
        string file_path;
        cout << "Enter the file path: ";
        cin >> file_path;
        load_availables_from_file(file_path);
    }
    else {
        cout << "Invalid mode." << endl;
        return 0;
    }

    run1();

    return 0;
}
