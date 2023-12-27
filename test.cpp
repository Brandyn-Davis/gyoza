#include <iostream>
#include <string>
#include <cassert>
#include "gyoza.h"

#define DICT_PATH "/var/lib/mecab/dic/unidic"
//#define DICT_PATH "/var/lib/mecab/"

int main(int argc, char* argv[]) {
    if (argc == 1) return 1;                // No input text

//    char testStr[50] = "テストです テストです";
//    const char path[] = "/var/lib/mecab/dic/unidic";

    Gyoza gyoza;
    if (gyoza.setDict(DICT_PATH) == -1) {
        std::cerr << "Dictionary was invalid!" << std::endl;
    }
    gyoza.setQuotes(true);

    if (argc == 2) {
        std::cout << gyoza.romaji(argv[1]) << std::endl;
    }
    else {
        std::cout << "wrong # of args!" << std::endl;
    }

    return 0;
}
