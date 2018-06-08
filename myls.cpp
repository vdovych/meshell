//
// Created by stanislav on 07.06.18.
//

#include "myls.h"
#include "utility.h"

#define BUFFSIZE 4096

struct details{
    string name;
    int size;
    string lastmod_s;
    timespec lastmod_t;
    char t;
    string ext;
    string to_str(){
        char siz[BUFFSIZE];
        sprintf(siz,"\t%d\t",size);
        return name+siz+lastmod_s;
    }
};
vector<string> get_filenames(vector<string>& dirs, bool recursive){
    vector<string> filenames;
    string dir;
    struct dirent** names;
    int n_names;
    while (!dirs.empty()){
        dir=dirs.back();
        dirs.pop_back();
        if(!is_dir(dir))
            filenames.emplace_back(dir);
        else {
            n_names = scandir(dir.c_str(), &names, NULL, alphasort);
            for (int i = 0; i < n_names; i++) {
                if(strcmp(names[i]->d_name,".")==0||strcmp(names[i]->d_name,"..")==0)
                    continue;
                if (recursive)
                    dirs.push_back(dir + "/" + names[i]->d_name);
                else
                    filenames.push_back(dir + "/" + names[i]->d_name);
            }
        }
    }
    return filenames;
}
details get_details(string file){
    struct stat buff;
    if (stat(file.c_str(), &buff) < 0) {
        perror(basename(strdup(file.c_str())));
        return details();
    }
    details dets;
    dets.name = basename(strdup(file.c_str()));
    dets.size=buff.st_size;
    dets.lastmod_t=buff.st_mtim;
    char buf[BUFFSIZE];
    strftime(buf, BUFFSIZE, "%D %T", gmtime(&buff.st_mtim.tv_sec));
    dets.lastmod_s=buf;
    dets.ext = dets.name.substr(dets.name.find_last_of('.')+1);
    if(dets.ext==dets.name)
        dets.ext.clear();
    if(S_ISDIR(buff.st_mode))
        dets.t='/';
    else if(S_ISLNK(buff.st_mode))
        dets.t='@';
    else if(S_ISFIFO(buff.st_mode))
        dets.t='|';
    else if(S_ISSOCK(buff.st_mode))
        dets.t='=';
    else if(buff.st_mode&S_IEXEC!=0)
        dets.t='*';
    else if(S_ISREG(buff.st_mode))
        dets.t=NULL;
    else
        dets.t='?';
    return dets;
}
enum sort_t{U,S,t,X,N};
int main(int argc, char** argv){
    //execvp("ls",argv);// :=)
    vector<string> dirs;
    char tmp[BUFFSIZE];
    char sort_s[BUFFSIZE]="N";
    sort_t sort_type;
    bool dir_first = false;
    bool spec_file = false;
    bool recursive = false;
    bool detailed = false;
    bool revers = false;
    bool type = false;
    bool read_all = false;
    for(int i=1;i<argc;i++){
        if(read_all){
            realpath(argv[i],tmp);
            dirs.emplace_back(tmp);
        }else if(argv[i][0]=='-') {
            if (strcmp(argv[i], "-h") == 0) {
                cout << "HEEELP" << endl;
                return 0;
            }
            else if (strcmp(argv[i], "-l") == 0)
                detailed = true;
            else if (strcmp(argv[i], "-r") == 0)
                revers = true;
            else if (strcmp(argv[i], "-R") == 0)
                recursive = true;
            else if (strcmp(argv[i], "-F") == 0)
                type = true;
            else if (strcmp(argv[i], "--") == 0)
                read_all = true;
            else if (strncmp(argv[i], "--sort=", 7) == 0)
                strcpy(sort_s, argv[i] + 7);
            else
                return 1;
        }
        else {
            realpath(argv[i], tmp);
            dirs.emplace_back(tmp);
        }
    }
    for(auto ch:sort_s){
        if(ch=='\0')
            break;
        switch (ch){
            case 'U': sort_type=U;break;
            case 'S': sort_type=S;break;
            case 't': sort_type=t;break;
            case 'X': sort_type=X;break;
            case 'N': sort_type=N;break;
            case 'D': dir_first=true;break;
            case 's': spec_file=true;break;
            default : return 1;
        }
    }
    if(dirs.empty())
        dirs.emplace_back(".");
    vector<string> files = get_filenames(dirs,recursive);
    vector<details> dets;
    for (auto &f:files) {
        dets.push_back(get_details(f));
    }
    if(sort_type!=U){
        if(sort_type==N){
            sort(dets.begin(),dets.end(),[](details a1,details a2)->int{ return a1.name<a2.name;});
        }
        if(sort_type==X){
            sort(dets.begin(),dets.end(),[](details a1,details a2)->int{ return a1.ext<a2.ext;});
        }
        if(sort_type==t){
            sort(dets.begin(),dets.end(),[](details a1,details a2)->int{ return a1.lastmod_t.tv_sec>a2.lastmod_t.tv_sec;});
        }
        if(sort_type==S){
            sort(dets.begin(),dets.end(),[](details a1,details a2)->int{ return a1.size>a2.size;});
        }
    }
    if(revers){
        reverse(dets.begin(),dets.end());
    }
    if(spec_file){
        vector<details> new_dets;
        for(auto& d:dets){
            if(d.t==NULL)
                new_dets.push_back(d);
        }
        for(auto& d:dets){
            if(d.t=='/')
                new_dets.push_back(d);
        }
        for(auto& d:dets){
            if(d.t=='*')
                new_dets.push_back(d);
        }
        for(auto& d:dets){
            if(d.t=='@')
                new_dets.push_back(d);
        }
        for(auto& d:dets){
            if(d.t=='|')
                new_dets.push_back(d);
        }
        for(auto& d:dets){
            if(d.t=='=')
                new_dets.push_back(d);
        }
        for(auto& d:dets){
            if(d.t=='?')
                new_dets.push_back(d);
        }
        dets.clear();
        dets=new_dets;
    }
    if(dir_first){
        vector<details> new_dets;
        for(auto& d:dets){
            if(d.t=='/')
                new_dets.push_back(d);
        }
        for(auto& d:dets){
            if(d.t!='/')
                new_dets.push_back(d);
        }
        dets.clear();
        dets=new_dets;
    }
    string ret;
    for(auto& det:dets){
        ret="";
        if(type||det.t=='/')
            ret+=det.t;
        if(detailed)
            ret+=det.to_str();
        else
            ret+=det.name;
        cout<<ret<<" ";
        if(detailed)
            cout<<endl;
    }
    cout<<endl;
    return 0;
}