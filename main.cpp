#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <wait.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFFERSIZE 512

using namespace std;

//#define MDEBUG

int merrno=0;
char basepath[BUFFERSIZE];
char curpath[BUFFERSIZE];
vector<string> MYPATH;
vector<string> PATH;

class Command {
    char **args;
    int argc;
    static void forkexec(char* command, char** args) {
        pid_t pid = fork();

        if (pid == -1)
        {
            std::cerr << "Failed to fork()" << std::endl;
            exit(EXIT_FAILURE);
        }
        else if (pid > 0)
        {
            int status;
            waitpid(pid, &status, 0);
        }
        else
        {
            execvp(command, args);
            cerr << "Parent: Failed to execute " << command << " \n\tCode: " << errno << endl;
            exit(EXIT_FAILURE);   // exec never returns
        }
    }
public:
    Command(char **args,const int& argc) {
        if(argc<1){
            throw exception();
        }
        this->args=new char*[BUFFERSIZE];
        for (int i = 0; i < argc; ++i) {
            this->args[i]=new char[BUFFERSIZE];
            strcpy(this->args[i], args[i]);
        }
        this->argc=argc;
    }

    void print(){
        for (int i = 0; i < argc; ++i) {
            cout<<args[i]<<endl;
        }
        cout<<argc<<endl;
    }
    void exec() {
        if(strcmp(args[0],"merrno")==0){
            if(argc==2&&(strcmp(args[1],"-h")==0||strcmp(args[1],"--help")==0)){
                cout<<"Return code of curr error\n";
                return;
            }
            if(argc>1){
                merrno=1;
                return;
            }
            cout<<merrno<<endl;
            return;

        } else if(strcmp(args[0],"mpwd")==0){
            if(argc==2&&(strcmp(args[1],"-h")==0||strcmp(args[1],"--help")==0)){
                cout<<"Print working directory\n";
                return;
            }
            if(argc>1){
                merrno=1;
                return;
            }
            cout<<curpath<<endl;
            return;
        } else if(strcmp(args[0],"mcd")==0){
            if(argc!=2){
                merrno=1;
                return;
            }
            if(strcmp(args[1],"-h")==0||strcmp(args[1],"--help")==0){
                cout<<"Change working directory\n";
                return;
            }
            char*tmp=realpath(args[1],NULL);
            if(tmp!=NULL)
                chdir(tmp);
            else
                merrno=1;
            return;
        } else if(strcmp(args[0],"mexit")==0){
            int excode=0;
            if(argc==2&&(strcmp(args[1],"-h")==0||strcmp(args[1],"--help")==0)){
                cout<<"Exit\n";
                return;
            }
            if(argc>2){
                merrno=1;
                return;
            }
            if(argc==2)
                excode=atoi(args[1]);
            exit(excode);
        }else{
            char* victim= new char[BUFFERSIZE];
//            if(args[0][0]=='m'&&args[0][1]=='y')
//                strcpy(victim,basepath);
//            else
//                strcpy(victim,curpath);
//            strcat(victim,"/");
            strcpy(victim,args[0]);
            forkexec(victim,args);
        }
    }
};

Command read_command(char *command) {
    char commbuff[BUFFERSIZE]={0};
    int i = 0;
    bool onelapka = 0;
    bool twolapka = 0;
    char **args=new char*[BUFFERSIZE];
    int argi=0;
    char tmp;
    while (i < BUFFERSIZE) {
        if (command[i] == '\'') {
            onelapka = !onelapka;
        } else if (command[i] == '\"') {
            twolapka = !twolapka;
        } else if (command[i] == '\\') {
            strcat(commbuff, &command[i + 1]);
            i++;
        } else if (isspace(command[i])) {
            if (onelapka || twolapka) {
                strcat(commbuff, " ");
            } else {
                args[argi]=new char[BUFFERSIZE];
                strcpy(args[argi], commbuff);
                argi++;
                strcpy(commbuff,"");
            }
        } else if (command[i] == '\0') {
#ifdef MDEBUG
            cout<<i<<endl;
#endif
            break;
        } else {
#ifdef MDEBUG
            cout<<command[i]<<endl;
#endif
            tmp=command[i];
            strcat(commbuff, &tmp);
        }
        i++;
#ifdef MDEBUG
        cout<<"commbuff:"<<commbuff<<endl<<"command:"<<command;
#endif
    }
#ifdef MDEBUG
    cout<<"Succ"<<endl;
#endif
    return Command(args,argi);
}

int shell_init(){
    getcwd(basepath,BUFFERSIZE);

    char path[BUFFERSIZE];
    getcwd(path,BUFFERSIZE);
    string Path = path;
    setenv("PATH",(Path+":"+getenv("PATH")).c_str(), 1);

    //TODO: Init more stuff (^-^ )
}
int main() {
    char in[BUFFERSIZE];
    shell_init();
    cout<<getenv("PATH");
    while (true) {
        getcwd(curpath,BUFFERSIZE);
        cout << curpath << " $ ";
        fgets(in,BUFFERSIZE,stdin);
        Command command = read_command(in);
        command.print();
        command.exec();
    }

    return 0;
}