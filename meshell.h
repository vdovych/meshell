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

#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

typedef void (*builtin)(const std::vector<std::string> &);

void mexport(const vector<string> &args);
void mecho(const std::vector<std::string> &args);
void mdot(const std::vector<std::string> &args);
void mcd(const std::vector<std::string> &args);
void mpwd(const std::vector<std::string> &args);
void mexit(const std::vector<std::string> &args);
void merrno(const std::vector<std::string> &args);

class Command {
    vector<string> args;
    string cin_ch;
    string cout_ch;
    string cerr_ch;
    int cin_fd=-1,cout_fd=-1,cerr_fd=-1;
    bool background= false;
    void parse_redirection();
    void forkexec();

public:
    Command(string raw_command);
    Command(vector<string>,int cin_fd,int cout_fd,int cerr_fd, bool background);

    void exec();
};

int Init();

#endif //MESHELL_MESHELL_H
