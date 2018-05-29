//
// Created by stanislav on 09.05.18.
//

#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#include <dirent.h>

#define BUFFSIZE 4096

using namespace std;


void cp(string from, string to){
    int in,out,size_check;
    char buff[BUFFSIZE];
    try_again:
    in = open(from.c_str(),O_RDONLY);
    out = open(to.c_str(),O_WRONLY);
    if (errno == EINTR&&(in==-1||out==-1))
        goto try_again;//Exceptional situations require exceptional measures =)
    else if (in == -1||out==-1)
        cerr << errno<<endl;
    else {
        while (true) {
            size_check = read(in, buff, BUFFSIZE);
            if(size_check==0)
                break;
            if(size_check==-1&&errno==EINTR)
                continue;
            if(size_check==-1) {
                cerr << errno << endl;
                break;
            }
            size_check = write(out,buff,size_check);
            if(size_check==-1) {
                cerr << errno << endl;
                break;
            }
        }
    }
    if(close(in)==-1||close(out)==-1)
        cerr<<errno<<endl;
}

bool is_dir(string file) {
    stat buff;
    if (stat(file.c_str(), &buff) != 0)
        return 0;
    return S_ISDIR(buff.st_mode);
}

void cpd(string dir, string to){
    if(!is_dir(dir))
        cp(dir,to);
    else {
        char* newto = new char(dir.c_str());
        newto = new char((to+basename(newto)).c_str());
        DIR* dir1 = opendir(dir.c_str());
        dirent* dirent1 = readdir(dir1);
        cp(dirent1->d_name, newto);
        while(true){
            errno=0;
            dirent1=readdir(dir1);
            if(dirent1==NULL) {
                if (errno == 0)
                    break;
                else
                    cerr<<errno;
            }
            else{
                cpd(dirent1->d_name, newto);
            }

        }
    }
}

void rmdir(string dir){
    if(!is_dir(dir))
        remove(dir.c_str());
    else{
        DIR* dir1 = opendir(dir.c_str());
        dirent* dirent1 = readdir(dir1);
        rmdir(dirent1->d_name);
        while(true){
            errno=0;
            dirent1=readdir(dir1);
            if(dirent1==NULL) {
                if (errno == 0)
                    break;
                else
                    cerr<<errno;
            }
            else{
                rmdir(dirent1->d_name);
            }

        }
        remove(dir.c_str());
    }
}

void recmkdir(string dir){
    errno=0;
    if(mkdir(dir.c_str(),S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH |  S_IWOTH)!=0){
        if(errno==2){
            char* path;
            strcpy(path,dir.c_str());
            recmkdir(dirname(path));
            mkdir(dir.c_str(),S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH |  S_IWOTH);
        }else
            cerr<<errno;
    }
}