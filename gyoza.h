#ifndef GYOZA_H
#define GYOZA_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <mecab.h>
#include "romaji-cpp/romaji.h"

class Gyoza {
    public:
        Gyoza(const char* dictStr);
        std::string romaji(char* jpText);
        void setQuotes(bool enableQuotes);
    private:
        MeCab::Tagger* tagger;
        const MeCab::Node* node;
        bool quotesEnabled = true;
};

#endif
