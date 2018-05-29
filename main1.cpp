#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <wait.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFFERSIZE 4096


using namespace std;


class Command{
    vector<string> args;

public:
    Command(string raw_command){
        string curr_comm;
        bool onelapka= false;
        bool twolapka= false;
        bool replace = true;
        bool escape  = false;
        for(auto ch:raw_command) {
            if (escape){
                curr_comm+=ch;
                escape=false;
            } else if (ch == '\'') {
                onelapka = !onelapka;
                replace = false;
            } else if (ch == '\"') {
                twolapka = !twolapka;
            } else if (ch == '\\') {
                escape = true;
            } else
        }
        //TODO: interpret command
        ///I need to read words
        ///Work with lapkas
        ///put correct pathes
    }
    void exec(){
        //TODO: execute command
    }
};

int Init(){
    char path[BUFFERSIZE];
    getcwd(path,BUFFERSIZE);
    string Path = path;
    setenv("PATH",(Path+":"+getenv("PATH")).c_str(), 1);
    return 0;
}

int main(int argc, char** argv){
    if(Init()<0)
        return -1;
    char curpath[BUFFERSIZE];
    Command* command;
    while (true) {
        getcwd(curpath,BUFFERSIZE);
        cout << curpath << " $ ";
        fgets(in,BUFFERSIZE,stdin);
        command=new Command(in);
        command->exec();
    }
}