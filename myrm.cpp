//
// Created by stanislav on 08.06.18.
//

#include "myrm.h"
#include "utility.h"

int main(int argc, char **argv) {
    vector<string> from;
    char tmp[BUFFSIZE];
    bool ask = true;
    bool recursive = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            cout << "Stuff" << endl;
            return 0;
        }
        if (strcmp(argv[i], "-f") == 0)
            ask = false;
        else if (strcmp(argv[i], "-R") == 0)
            recursive = true;
        else {
            realpath(argv[i], tmp);
            from.emplace_back(tmp);
        }
    }
    for (string dir:from) {
        if (recursive)
            rmd(dir, ask);
        else if (is_dir(dir)) {
            errno = 21;
            perror(dir.c_str());
        } else
            ask_rem(dir, ask);
    }
    return 0;
}