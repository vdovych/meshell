//
// Created by stanislav on 08.06.18.
//

#ifndef MESHELL_UTILITY_H
#define MESHELL_UTILITY_H


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

bool is_dir(string file);

void cp(string from, string to, bool &ask);

void cpd(string dir, string to, bool &ask);

void ask_rem(string dir, bool &ask);

void rmd(string dir, bool &ask);

void recmkdir(string dir);

#endif //MESHELL_UTILITY_H
