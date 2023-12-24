#include <iostream>
#include <string>
#include "gyoza.h"

int main() {
    char testStr[50] = "テストです";
    const char path[] = "/var/lib/mecab/dic/unidic";

    Gyoza gyoza(path);
    std::cout << gyoza.romaji(testStr) << std::endl;

    return 0;
}
