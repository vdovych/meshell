//
// Created by stanislav on 09.05.18.
//

#include "utility.h"
using namespace std;

inline bool if_exist(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

void cp(string from, string to, bool &ask) {
    int in,out,size_check;
    char buff[BUFFSIZE];
    try_again:
    in = open(from.c_str(),O_RDONLY);
    if (if_exist(to.c_str()) && ask) {
        char answ[50];
        char a;
        cout << "Overwrite " << to << "? [Y]es/[N]o/[A]ll/[C]ancel" << endl;
        cin >> answ;
        a = tolower(answ[0]);
        switch (a) {
            case 'a':
                ask = false;
            case 'n':
                return;
            case 'c':
                exit(0);
            default:
                break;
        }
    }
    out = open(to.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                                               S_IROTH | S_IWOTH);
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
    struct stat buff;
    if (stat(file.c_str(), &buff) != 0)
        return 0;
    return S_ISDIR(buff.st_mode);
}

void cpd(string dir, string to, bool &ask) {
    if(!is_dir(dir))
        cp(dir, to, ask);
    else {
        to += "/";
        to += basename(strdup(dir.c_str()));
        recmkdir(to);
        DIR* dir1 = opendir(dir.c_str());
        dirent* dirent1 = readdir(dir1);
        if (!(strcmp(dirent1->d_name, ".") == 0 || strcmp(dirent1->d_name, "..") == 0)) {
            char *newto = strdup((to + "/" + basename(dirent1->d_name)).c_str());
            cpd(dir + "/" + dirent1->d_name, newto, ask);
        }
        while (dirent1 = readdir(dir1)) {
            if (!(strcmp(dirent1->d_name, ".") == 0 || strcmp(dirent1->d_name, "..") == 0)) {
                char *newto = strdup((to + "/" + basename(dirent1->d_name)).c_str());
                cpd(dir + "/" + dirent1->d_name, newto, ask);
            }
        }
    }
}

void ask_rem(string dir, bool &ask) {
    if (ask) {
        char answ[50];
        char a;
        cout << "Remove " << dir << "? [Y]es/[N]o/[A]ll/[C]ancel" << endl;
        cin >> answ;
        a = tolower(answ[0]);
        switch (a) {
            case 'a':
                ask = false;
            case 'y':
                remove(dir.c_str());
                break;
            case 'c':
                exit(0);
            default:
                break;
        }
    } else
        remove(dir.c_str());
}

void rmd(string dir, bool &ask) {
    if (!is_dir(dir))
        ask_rem(dir, ask);
    else{
        DIR* dir1 = opendir(dir.c_str());
        dirent* dirent1 = readdir(dir1);
        if (!(strcmp(dirent1->d_name, ".") == 0 || strcmp(dirent1->d_name, "..") == 0))
            rmd(dir + "/" + dirent1->d_name, ask);
        while (dirent1 = readdir(dir1)) {
            if (!(strcmp(dirent1->d_name, ".") == 0 || strcmp(dirent1->d_name, "..") == 0))
                rmd(dir + "/" + dirent1->d_name, ask);
        }
        ask_rem(dir, ask);
    }
}

void recmkdir(string dir){
    errno=0;
    if (mkdir(dir.c_str(), S_IRWXG | S_IRWXO | S_IRWXU) < 0) {
        if(errno==2){
            char path[BUFFSIZE];
            strcpy(path,dir.c_str());
            recmkdir(dirname(path));
            mkdir(dir.c_str(), S_IRWXG | S_IRWXO | S_IRWXU);
        } else if (errno != 17)
            cerr<<errno;
    }
}