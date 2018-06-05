//
// Created by stanislav on 01.06.18.
//

#ifndef MESHELL_MESHELL_H
#define MESHELL_MESHELL_H

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>
#include <map>
#include <fnmatch.h>
using namespace std;
typedef void (*builtin)(const std::vector<std::string>&);
void mexport(const vector<string>& args);
void mecho(const std::vector<std::string>& args);
void mdot(const std::vector<std::string>& args);
void mcd(const std::vector<std::string>& args);
void mpwd(const std::vector<std::string>& args);
void mexit(const std::vector<std::string>& args);
void merrno(const std::vector<std::string>& args);
class Command{
    vector<string> args;
    void forkexec();
public:
    Command(string raw_command);
    void exec();
};
int Init();

#endif //MESHELL_MESHELL_H
