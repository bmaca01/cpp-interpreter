#include<iostream>
#include<string>
#include<cctype>
#include<sstream>
#include<fstream>
#include<map>

#include "lex.h"
#include "parse.h"
#include "pt.h"

using namespace std;

namespace arg_info {
    bool is_file=0, is_flag=0, is_valid=0;
    bool flag_state[1] = {0};
    string flags[1] = {"-v"};
    int file_cnt=0;

    int index=0, max=0, err_code=0;

    ifstream file;

    int find(int size, string* arr, string str);
    void verify_arg(string arg);
    void reset_state();

    int find(int size, string* arr, string str) {
        // returns index of matched string, 
        // returns -1 if not found.
        for (int i=0; i<size; i++) {
            string currStr = arr[i];
            if (currStr.compare(str) == 0) 
                return i;
        }
        return -1;
    }

    void verify_arg(string arg) {
        if (arg[0] == '-') {
            is_flag=1;
            int tmp = find(4, flags, arg);
            if (tmp != -1) {
                is_valid=1;
                flag_state[tmp] = 1;
            }
            else if (tmp == -1) {
                // unrecognized flag
                is_valid=0;
                err_code=1;
            }
        }
        else {
            is_file=1;
            file_cnt++;
            if (file_cnt > 1)
                // more than one file
                err_code=2;
            else if (file_cnt == 1) {
                // cant open file
                file.open(arg);
                if (file.is_open() == false)
                    err_code=3;
            }
        }
    }
    void reset_state() {
        is_file=0, is_flag=0, is_valid=0, err_code=0;
    }
}

int Pt::NodeCount() const {
    int cnt = 0;
    if (left) {
        cnt += left->NodeCount();
    }
    if (right) {
        cnt += right->NodeCount();
    }
    return cnt+1;
}

int Pt::MaxDepth() const {
    int max=0, l=0, r=0;
    if (left)
        l += left->MaxDepth();
    if (right) {
        r += right->MaxDepth();
    }
    if (l > r)
        return max + (l+1);
    else
        return max + (r+1);
}

int Pt::CountPlus() const {
    int cnt=0;
    if (left) {
        cnt += left->CountPlus();
    }
    if (right) {
        cnt += right->CountPlus();
    }
    if (this->isPlus())
        return cnt+1;
    else
        return cnt;
}

int Pt::CountEq() const {
    int cnt=0;
    if (left) {
        cnt += left->CountEq();
    }
    if (right) {
        cnt += right->CountEq();
    }
    if (this->isEq())
        return cnt+1;
    else
        return cnt;
}

int main(int argc, char* argv[]) {
    using namespace arg_info;
    int ln_num = 0;
    Tok tok;
    istream* in = &cin;

    arg_info::max = argc-1;
    for (int i=1; i<argc; i++) {
        arg_info::index = i;
        string curr_arg(argv[i]);
        arg_info::verify_arg(curr_arg);

        switch (arg_info::err_code) {
            case 1:
                cout << "UNRECOGNIZED FLAG " << curr_arg << endl;
                return 0;
            case 2:
                cout << "ONLY ONE FILE NAME ALLOWED" << endl;
                return 0;
            case 3:
                cout << "CANNOT OPEN " << curr_arg << endl;
                return 0;
        }
    }
    arg_info::reset_state();

    // set input stream to file if its open
    if (arg_info::file.is_open() == true)
        in = &file;

    // make the parse tree
    Pt *head = Prog(*in, ln_num);
        
    if (head != 0) {

        int p = head->CountPlus();
        int e = head->CountEq();
        int d = head->MaxDepth();

        cout << "PLUS COUNT: " << p << endl;
        cout << "EQ COUNT: " << e << endl;
        cout << "MAX DEPTH: " << d << endl;
    }

    file.close();
    return 0;
}

