#ifndef GYOZA_H
#define GYOZA_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <mecab.h>
#include "romaji.h"

class Gyoza {
    public:
        Gyoza(const char* dictStr);
        std::string romaji(char* jpText);
    private:
        MeCab::Tagger* tagger;
        const MeCab::Node* node;
};

#endif
