//
// Created by stanislav on 08.06.18.
//
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <regex>
#include <string>
#include "mygrep.h"

#define BUFFSIZE 4096

using namespace std;

int main(int argc, char **argv) {
    bool invert = false;
    bool ignore = false;
    bool reg = false;
    string strin;
    string fname;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                cout << "Stuff" << endl;
                return 0;
            }
            if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--invert-match") == 0) {
                invert = !invert;//for double inversion =)
            } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--ignore-case") == 0) {
                ignore = true;
            } else if (strncmp(argv[i], "--regexp=", 9) == 0) {
                reg = true;
                strin = argv[i] + 9;
            } else if (strncmp(argv[i], "--file=", 7) == 0) {
                fname = argv[i] + 7;
            } else {
                cerr << "bad args" << endl;
                return 1;
            }
        } else if (!reg) {
            strin = argv[i];
        } else {
            cerr << "bad args" << endl;
            return 1;
        }
    }
    if (strin.empty()) {
        cerr << "bad args" << endl;
        return 1;
    }
    if (!fname.empty()) {
        int fd;
        if ((fd = open(fname.c_str(), O_RDONLY)) < 0) {
            perror(fname.c_str());
            return 1;
        }
        close(STDIN_FILENO);
        dup2(fd, STDIN_FILENO);
    }
    string buf;
    char buff[BUFFSIZE];
    while (fgets(buff, BUFFSIZE, stdin)) {
        buf = buff;
        if (ignore)
            std::transform(buf.begin(), buf.end(), buf.begin(), ::tolower);
        if (!reg) {
            if (((buf.find(strin.c_str()) != string::npos) + invert) % 2)
                cout << buff;
        } else {
            if ((regex_match(buf, regex(strin)) + invert) % 2)
                cout << buff;
        }
    }

    return 0;
}