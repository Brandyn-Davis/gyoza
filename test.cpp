#include <iostream>
#include <string>
#include <cassert>
#include "gyoza.h"

#define DICT_PATH "/var/lib/mecab/dic/unidic"

int main(int argc, char* argv[]) {
    if (argc == 1) return 1;                // No input text

//    char testStr[50] = "テストです テストです";
//    const char path[] = "/var/lib/mecab/dic/unidic";

    Gyoza gyoza(DICT_PATH);

    if (argc == 2) {
        std::cout << gyoza.romaji(argv[1]) << std::endl;
    }
    else {
        std::cout << "wrong # of args!" << std::endl;
    }

    return 0;
}
