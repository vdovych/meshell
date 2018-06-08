//
// Created by stanislav on 08.06.18.
//

#include "mycp.h"


int main(int argc, char **argv) {
    vector<string> from;
    string des = "";
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
        else
            from.push_back(argv[i]);
    }
    des = from.back();
    from.pop_back();
    if (!recursive && from.size() == 1) {
        cp(from[0], des, ask);
    } else if (recursive) {
        for (string &name:from) {
            cpd(name, des, ask);
        }
    } else {
        for (string &name:from) {
            if (!is_dir(name)) {
                cp(name, des + "/" + basename(strdup(name.c_str())), ask);
            }
        }
    }


    return 0;
}