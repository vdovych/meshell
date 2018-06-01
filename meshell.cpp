#include "meshell.h"
#define BUFFERSIZE 4096


using namespace std;


char curpath[BUFFERSIZE];
const map<string,builtin> builtins={{"mcd",mcd},{"merrno",merrno},{"mpwd",mpwd},{"mexit",mexit},{"margs",margs},{".",mdot}};

class Command{
    vector<string> args;
    void forkexec() {
        const char* command = args[0].c_str();
        char* argv[BUFFERSIZE]={0};
        for (int i = 0; i < args.size(); ++i) {
            argv[i]=new char[BUFFERSIZE];
            strcpy(argv[i],args[i].c_str());
        }
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
            execvp(command, argv);
            cerr << "Parent: Failed to execute " << command << " \n\tCode: " << errno << endl;
            exit(EXIT_FAILURE);   // exec never returns
        }
    }
public:
    Command(string raw_command){
        raw_command+=" ";
        string curr_comm;
        bool onelapka= false;
        bool twolapka= false;
        bool replace = true;
        bool escape  = false;
        for(auto ch:raw_command) {
            if(ch=='#'&&!onelapka&&!twolapka){
                if(!curr_comm.empty())
                    args.push_back(curr_comm);
                break;
            }else if (escape){
                curr_comm+=ch;
                escape=false;
            } else if (ch == '\'') {
                if(!twolapka) {
                    onelapka = !onelapka;
                    replace = false;
                } else
                    ch+='\'';
            } else if (ch == '\"') {
                if(!onelapka)
                    twolapka = !twolapka;
                else
                    ch+='\"';
            } else if (ch == '\\') {
                escape = true;
            } else if (ch == '-'){
                curr_comm+=ch;
                replace= false;
            } else if (isspace(ch)){
                if(onelapka||twolapka){
                    curr_comm+=ch;
                }else{
                    if(replace&&!args.empty()){
                        vector<string> matched;
                        char* dir =new char[BUFFERSIZE];
                        char* path =new char[BUFFERSIZE];
                        realpath(curr_comm.c_str(),dir);
                        realpath(curr_comm.c_str(),path);
                        dirent** files;
                        int k=scandir(dirname(dir), &files, 0, alphasort);
                        for(int i=0;i<k;i++){
                            if(strcmp(files[i]->d_name,".")!=0&&strcmp(files[i]->d_name,"..")!=0){
                                if(!fnmatch(basename(path),files[i]->d_name,0)){
                                    char *tmp = new char[BUFFERSIZE];
                                    strcpy(tmp,dir);
                                    strcat(tmp,"/");
                                    strcat(tmp,files[i]->d_name);
                                    matched.emplace_back(tmp);
                                }
                            }
                        }
                        if(matched.empty()){
                            if(!curr_comm.empty())
                                args.push_back(curr_comm);
                        } else{
                            for(auto& e:matched){
                                args.push_back(e);
                            }
                        }
                    }else {
                        if(!curr_comm.empty())
                            args.push_back(curr_comm);
                    }
                    curr_comm = "";
                    onelapka= false;
                    twolapka= false;
                    replace = true;
                    escape  = false;
                }
            }else{
                curr_comm+=ch;
            }
        }
    }
    void exec(){
        if(args.empty()){
            return;
        }
        bool b= true;
        for(auto& buil:builtins){
            if(args[0]==buil.first){
                buil.second(args);
                b= false;
                break;
            }
        }
        if(b)
            forkexec();

    }
};

void margs(const vector<string>& args){
    if(args[1]=="-h"||args[1]=="--help"){
        cout<<"HEEEELP!!!!"<<endl;
        return;
    }
    for(auto& e:args){
        cout<<e<<endl;
    }
}
void mdot(const vector<string>& args){
    if(args[1]=="-h"||args[1]=="--help"){
        cout<<"HEEEELP!!!!"<<endl;
        return;
    }
    Command* command;
    char in[BUFFERSIZE];
    FILE* inp = fopen(args[1].c_str(),"r");
    while (true){
        if(fgets(in, BUFFERSIZE, inp)== nullptr){
            break;
        }
        command = new Command(in);
        command->exec();
    }

}
void mcd(const vector<string>& args){
    if(args[1]=="-h"||args[1]=="--help"){
        cout<<"HEEEELP!!!!"<<endl;
        return;
    }
    char*tmp=realpath(args[1].c_str(),NULL);
    if(tmp!=NULL)
        chdir(tmp);
    else
        errno=2;
}
void merrno(const vector<string>& args){
    if(args[1]=="-h"||args[1]=="--help"){
        cout<<"HEEEELP!!!!"<<endl;
        return;
    }
    cout<<errno;
}
void mpwd(const vector<string>& args){
    if(args[1]=="-h"||args[1]=="--help"){
        cout<<"HEEEELP!!!!"<<endl;
        return;
    }
    cout<<curpath<<endl;

}
void mexit(const vector<string>& args){
    if(args[1]=="-h"||args[1]=="--help"){
        cout<<"HEEEELP!!!!"<<endl;
        return;
    }
    exit(atoi(args[1].c_str()));
}



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

    Command* command;
    char in[BUFFERSIZE];
    if(argc==1) {
        while (true) {
            getcwd(curpath, BUFFERSIZE);
            cout << curpath << " $ ";
            fgets(in, BUFFERSIZE, stdin);
            command = new Command(in);
            command->exec();
        }
    } else{
        FILE* inp = fopen(argv[1],"r");
        while (true){
            if(fgets(in, BUFFERSIZE, inp)== nullptr){
                break;
            }
            command = new Command(in);
            command->exec();
        }
    }
}