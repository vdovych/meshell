//
// Created by stanislav on 08.06.18.
//

#include "mymkdir.h"


int main(int argc, char **argv) {
    string dest;
    bool promi = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            cout << "Stuff" << endl;
            return 0;
        }
        if (strcmp(argv[i], "-p") == 0)
            promi = true;
        else
            dest = argv[i];
    }
    if (!promi) {
        if (mkdir(dest.c_str(), S_IRWXG | S_IRWXO | S_IRWXU) < 0) {
            perror(dest.c_str());
            return 1;
        }
    } else {
        recmkdir(dest);
    }
    return 0;
}