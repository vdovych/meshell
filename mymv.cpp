//
// Created by stanislav on 08.06.18.
//

#include "mymv.h"

int main(int argc, char **argv) {
    vector<string> from;
    string des;
    bool ask = true, f = false;
    bool recursive = true;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            cout << "Stuff" << endl;
            return 0;
        }
        if (strcmp(argv[i], "-f") == 0)
            ask = false;
        else
            from.emplace_back(argv[i]);
    }
    des = from.back();
    from.pop_back();
    if (from.size() == 1) {
        cpd(from[0], des, ask);
        rmd(from[0], f);
    } else {
        for (string &name:from) {
            cpd(name, des, ask);
            rmd(name, f);
        }
    }


    return 0;
}