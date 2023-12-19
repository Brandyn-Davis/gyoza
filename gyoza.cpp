#include <iostream>
#include <cstdio>
#include <cstring>
#include <mecab.h>
#include "romaji.h"

#define CHECK(eval) if (! eval) { \
    const char *e = tagger ? tagger->what() : MeCab::getTaggerError(); \
    std::cerr << "Exception:" << e << std::endl; \
    delete tagger; \
    return -1; }
//#define MECAB_ARGS "-d /usr/local/lib/mecab/dic/unidic/"
#define MECAB_ARGS "-d /var/lib/mecab/dic/unidic"
#define FEATURE_LEN 17
#define FEATURE_STR_LEN 64
#define FEATURESTR_LEN 256
#define INPUT_MAX_LEN 1024


// strtok but with consecutive delim handling
char* strtoke(char* str, const char* delim) {
    static char *start = NULL;
    char *token = NULL;
    if (str) start = str;
    if (!start) return NULL;
    token = start;
    start = strpbrk(start, delim);
    if (start) *start++ = '\0';
    return token;
}

void printNode(const MeCab::Node* node, char features[][FEATURE_STR_LEN]) {
    std::cout.write(node->surface, node->length) << "\t";
    for (int j = 0; j < FEATURE_LEN; j++) {
        printf("[%s]", features[j]);
    }
    std::cout << std::endl;
}

int main (int argc, char **argv) {
    //if (argc != 2) {
    //    std::cout << "need an arg with japanese text" << std::endl;
    //    return 1;
    //}
    //char input[1024];
    //strcpy(input, argv[1]);

    //char input[INPUT_MAX_LEN] = "太郎は次郎が、持っている本を花子に渡した。";
    char input[INPUT_MAX_LEN] = "これは３つのテストのうちの１つです。";
    std::string romaji;
    std::string romajiFinal;
    char* pch;
  
    // Create tagger with dictionary specified
    MeCab::Tagger *tagger = MeCab::createTagger(MECAB_ARGS);
    CHECK(tagger);
  
    // Parse input as nodes
    const MeCab::Node* node = tagger->parseToNode(input);
    CHECK(node);

    // Print words (nodes)
    for (; node; node = node->next) {
        int i = 0;
        char currKana[FEATURE_STR_LEN];
        char prevKana[FEATURE_STR_LEN];
        char features[FEATURE_LEN][FEATURE_STR_LEN];
        char featureStr[FEATURESTR_LEN] = "";
        strcpy(featureStr, node->feature);

        // Store node features in array
        pch = strtoke(featureStr, ",");
        while (pch != NULL) {
            //printf("[%s]", pch);
            strcpy(features[i++], pch);
            pch = strtoke(NULL, ",");
        }
        strcpy(prevKana, currKana);
        strcpy(currKana, features[9]);

        //printNode(node, features);

        // Romajify node
        if (features[9][0] != '*' && node->surface[0] != ',') {
            japanese::utf8_kana_to_romaji(features[9], romaji);

//            if (node->prev != nullptr) {
                // If prev node ends w/ little tsu, have 2 of next chr
                //((prevKana[strlen(prevKana)-1]&0x3F) | (prevKana[strlen(prevKana)-2]&0x3)<<6) == L'ッ'&0xFF
                // can be mistaken for a few other characters that we don't expect to see
            if (prevKana[strlen(prevKana)-1] == (char)'ッ' && romaji != "") { 
                romajiFinal += romaji.front() + romaji;
            }
            else if (strcmp(features[1], "読点") == 0) {
                if (node->next->length == 0)
                    romajiFinal += ",";
                else
                    romajiFinal += ", ";
            }
            else if (strcmp(features[1], "句点") == 0) {
                if (node->next->length == 0)
                    romajiFinal += ".";
                else
                    romajiFinal += ". ";
            }
            else {
                if (node->prev->length == 0 || strlen(prevKana) == 0)
                    romajiFinal += romaji;
                else
                    romajiFinal += " " + romaji;
            }
            
            //printf("[%ld]", strlen(currKana));

            romaji = "";

            //std::cout << currKana;
        }
        else {
            //std::cout << '<' << currKana << '>';
        }
    }
    std::cout << romajiFinal << std::endl;
  
    delete tagger;
  
    return 0;
}
