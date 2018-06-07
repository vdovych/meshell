#include <fcntl.h>
#include "meshell.h"
#define BUFFERSIZE 4096

//TODO: work with errors
using namespace std;


char curpath[BUFFERSIZE];
int d_env_size=0;
char* d_env[BUFFERSIZE];
const map<string,builtin> builtins={{"mcd",mcd},{"merrno",merrno},{"mpwd",mpwd},{"mexit",mexit},
                                    {"mecho",mecho},{".",mdot},{"mexport",mexport}};




void Command::forkexec() {
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
        if(!background)
            waitpid(pid, &status, 0);
    }
    else
    {
        if(background){
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
        }
        if(!cin_ch.empty()||cin_fd!=-1){
            close(STDIN_FILENO);
            if(!cin_ch.empty())
                cin_fd = open(cin_ch.c_str(),O_RDONLY);
            dup2(cin_fd,STDIN_FILENO);
        }
        if(!cout_ch.empty()||cout_fd!=-1){
            close(STDOUT_FILENO);
            if(!cout_ch.empty())
                if(cout_ch=="&2")
                    cout_fd = STDERR_FILENO;
                else
                    cout_fd = open(cout_ch.c_str(),O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                                                                        S_IROTH | S_IWOTH);
            dup2(cout_fd,STDOUT_FILENO);
        }
        if(!cerr_ch.empty()||cerr_fd!=-1){
            close(STDERR_FILENO);
            if(!cerr_ch.empty())
                if(cerr_ch=="&1")
                    cerr_fd = STDOUT_FILENO;
                else
                    cerr_fd = open(cout_ch.c_str(),O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                                                                    S_IROTH | S_IWOTH);
            dup2(cerr_fd,STDERR_FILENO);
        }

        execvpe(command, argv,d_env);
        perror(command);
        exit(EXIT_FAILURE);
    }
}
Command::Command(string raw_command) {
    raw_command+=" ";
    string curr_comm;
    bool onelapka= false;
    bool twolapka= false;
    bool replace = true;
    bool escape  = false;
    bool assign = false;
    bool out_to = false;
    bool err_to = false;
    bool in_from = false;
    for(auto ch:raw_command) {
        if(ch=='#'&&!onelapka&&!twolapka){
            if(!curr_comm.empty())
                args.push_back(curr_comm);
            break;


        }else if (escape){
            curr_comm+=ch;
            escape=false;
        } else if(ch=='='&&args.empty()){
            assign = true;
            curr_comm+=ch;
        }else if (ch == '\''&&!twolapka) {
            onelapka = !onelapka;
            replace = false;
        } else if (ch == '\"'&&!onelapka) {
            twolapka = !twolapka;
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
                    if(curr_comm[0]=='$'){
                        char* var=getenv(curr_comm.c_str()+1);
                        if(var!=NULL){
                            curr_comm=var;
                        }
                    }
                    if(curr_comm=="2>&1"){
                        cerr_ch="&1";
                    } else if(curr_comm==">&2"){
                        cout_ch="&2";
                    }else if(curr_comm==">"){
                        out_to= true;
                    }else if(curr_comm=="2>"){
                        err_to= true;
                    }else if(curr_comm=="<"){
                        in_from= true;
                    }else if(out_to){
                        cout_ch=curr_comm;
                    }else if(err_to){
                        cerr_ch=curr_comm;
                    }else if(in_from){
                        cin_ch=curr_comm;
                    }else {
                        vector<string> matched;
                        char *dir = new char[BUFFERSIZE];
                        char *path = new char[BUFFERSIZE];
                        realpath(curr_comm.c_str(), dir);
                        realpath(curr_comm.c_str(), path);
                        dirent **files;
                        int k = scandir(dirname(dir), &files, 0, alphasort);
                        for (int i = 0; i < k; i++) {
                            if (strcmp(files[i]->d_name, ".") != 0 && strcmp(files[i]->d_name, "..") != 0) {
                                if (!fnmatch(basename(path), files[i]->d_name, 0)) {
                                    char *tmp = new char[BUFFERSIZE];
                                    strcpy(tmp, dir);
                                    strcat(tmp, "/");
                                    strcat(tmp, files[i]->d_name);
                                    matched.emplace_back(tmp);
                                }
                            }
                        }
                        if (matched.empty()) {
                            if (!curr_comm.empty())
                                args.push_back(curr_comm);
                        } else {
                            for (auto &e:matched) {
                                args.push_back(e);
                            }
                        }
                    }


                }else if(args.empty()&&assign){
                    putenv(strdup(curr_comm.c_str()));
                } else {
                    if(!curr_comm.empty())
                        args.push_back(curr_comm);
                }


                curr_comm = "";
                onelapka= false;
                twolapka= false;
                replace = true;
                escape  = false;
                assign  = false;
            }
        }else{
            curr_comm+=ch;
        }
    }
}
Command::Command(vector<string> args,int cin_fd,int cout_fd,int cerr_fd, bool background){
    this->args=args;
    this->cin_fd=cin_fd;
    this->cout_fd=cout_fd;
    this->cerr_fd=cerr_fd;
    this->background=background;
}

void Command::exec() {
    if(args.empty()){
        return;
    }
    if(args.back()=="&"){
        background= true;
    }
    bool conv =false;
    for (auto& arg:args){
        if(arg=="|"){
            conv= true;
            break;
        }
    }
    if(conv){
        int i=0;
        Command *prev= nullptr,*curr= nullptr;
        int pip[2];
        while(i<args.size()){
            bool conv_next=false;
            vector<string> args_curr;
            for(;i<args.size();i++){
                if(args[i]=="|"){
                    i++;
                    conv_next= true;
                    break;
                }
                args_curr.push_back(args[i]);
            }
            if(prev == nullptr){
                pipe(pip);
                curr=new Command(args_curr,-1,pip[1],-1,true);
                curr->exec();
            }else {
                if(conv_next){
                    int p=pip[0];
                    pipe(pip);
                    curr=new Command(args_curr,p,pip[1],-1, true);
                    curr->exec();
                } else{
                    if(!cout_ch.empty())
                        cout_fd = open(cout_ch.c_str(),O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                                                                                S_IROTH | S_IWOTH);
                    if(!cerr_ch.empty())
                        cerr_fd = open(cout_ch.c_str(),O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                                                                                S_IROTH | S_IWOTH);
                    curr=new Command(args_curr,pip[0],cout_fd,cerr_fd,background);
                    curr->exec();
                }
            }
            prev=curr;
        }
    }else {
        bool b = true;
        for (auto &buil:builtins) {
            if (args[0] == buil.first) {
                buil.second(args);//TODO:redirect
                b = false;
                break;
            }
        }
        if (b)
            forkexec();
    }
    close(cin_fd);
    close(cout_fd);
    close(cerr_fd);
}

void mexport(const vector<string>& args){
    if(args.size()==2) {
        if(args[1]=="-h"||args[1]=="--help"){
            cout<<"HEEELP!!!"<<endl;
            return;
        }
        char *var = strtok(strdup(args[1].c_str()), "=");
        if (strtok(NULL, "=") != NULL) {
            putenv(strdup(args[1].c_str()));
            strcpy(d_env[d_env_size++], args[1].c_str());
        } else {
            setenv(var, "", 0);
            strcpy(d_env[d_env_size++], var);
            strcat(d_env[d_env_size - 1], "=");
        }
    }else{
        errno=1;
        cerr<<"Bad arguments"<<endl;
    }
}
void mecho(const vector<string>& args){
    if(args[1]=="-h"||args[1]=="--help"){
        cout<<"HEEEELP!!!!"<<endl;
        return;
    }
    for(int i=1;i<args.size();++i){
        cout<<args[i]<<endl;
    }
}
void mdot(const vector<string>& args){
    if(args.size()>1) {
        if (args[1] == "-h" || args[1] == "--help") {
            cout << "HEEEELP!!!!" << endl;
            return;
        }
        Command *command;
        char in[BUFFERSIZE];
        FILE *inp = fopen(args[1].c_str(), "r");
        while (true) {
            if (fgets(in, BUFFERSIZE, inp) == nullptr) {
                break;
            }
            command = new Command(in);
            command->exec();
        }
    }else{
        errno=1;
        cerr<<"Bad arguments"<<endl;
    }
}
void mcd(const vector<string>& args){
    if(args.size()>1) {
        if (args[1] == "-h" || args[1] == "--help") {
            cout << "HEEEELP!!!!" << endl;
            return;
        }
        char *tmp = realpath(args[1].c_str(), NULL);
        if (tmp != NULL)
            chdir(tmp);
        else
            errno = 2;
    } else{
        errno=1;
        cerr<<"Bad arguments"<<endl;
    }
}
void merrno(const vector<string>& args){
    if(args.size()==2&&(args[1]=="-h"||args[1]=="--help")){
        cout<<"HEEEELP!!!!"<<endl;
        return;
    }
    cout<<errno<<endl;
}
void mpwd(const vector<string>& args){
    if(args.size()==2&&(args[1]=="-h"||args[1]=="--help")){
        cout<<"HEEEELP!!!!"<<endl;
        return;
    }
    cout<<curpath<<endl;

}
void mexit(const vector<string>& args){
    if(args.size()==2) {
        if (args[1] == "-h" || args[1] == "--help") {
            cout << "HEEEELP!!!!" << endl;
            return;
        }
        exit(atoi(args[1].c_str()));
        return;
    } else if(args.size()==1)
        exit(0);
    errno=1;
    cerr<<"Bad arguments"<<endl;
}



int Init(){
    for(int i=0;environ[i];i++) {
        d_env[i]=new char[BUFFERSIZE];
        strcpy(d_env[i],environ[i]);
        ++d_env_size;
    }
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
    string in;
    if(argc==1) {
        while (true) {
            getcwd(curpath, BUFFERSIZE);
            in=readline(((string)curpath+" $ ").c_str());
            if(in.size()>0) {
                add_history(in.c_str());
                command = new Command(in);
                command->exec();
            }
        }
    } else{
        char in[BUFFERSIZE];
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