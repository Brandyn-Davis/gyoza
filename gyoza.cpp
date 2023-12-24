#include "gyoza.h"

#define CHECK(eval) if(!eval){std::cerr<<"Exception\n";}

//#define MECAB_ARGS "-d /usr/local/lib/mecab/dic/unidic/"
#define MECAB_ARGS "-d /var/lib/mecab/dic/unidic"
#define FEATURE_LEN 10
#define FEATURE_STR_LEN 64
#define FEATURESTR_LEN 256
#define INPUT_MAX_LEN 1024
#define DICTSTR_MAX_LEN 256


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

Gyoza::Gyoza(const char* dictStr) {
    char dictArg[DICTSTR_MAX_LEN];

    // Create tagger with correct dictionary
    if (strlen(dictStr) < DICTSTR_MAX_LEN-3) {
        strcpy(dictArg+3, dictStr);
        memcpy(dictArg, "-d ", 3);
        //std::cout << "Using argument \"" << dictArg << "\"" << std::endl;
        tagger = MeCab::createTagger(dictArg);
    }
    else if (strlen(dictStr) == 0) {
        std::cout << "No MeCab dictionary path provided. Using default..." << std::endl;
        tagger = MeCab::createTagger("");
    }
    else {
        std::cout << "MeCab dictionary path string is too long! Using default..." << std::endl;
        tagger = MeCab::createTagger("");
    }
    CHECK(tagger);
}

std::string Gyoza::romaji(char* jpText) {
    // Parse input as nodes
    node = tagger->parseToNode(jpText);
    CHECK(node);

//    std::cout << node->feature << std::endl;

    std::string romaji;
    std::string romajiFinal;
    char* pch;
    char currKana[FEATURE_STR_LEN] = "";
    char prevKana[FEATURE_STR_LEN] = "";
    char featureStr[FEATURESTR_LEN] = "";
    //char features[FEATURE_LEN][FEATURE_STR_LEN] = {""};
    char features[FEATURE_LEN][FEATURE_STR_LEN];

    // Print words (nodes)
    for (; node; node = node->next) {
        strcpy(featureStr, node->feature);  // get non-const version

        // store first few node features in array
        pch = strtoke(featureStr, ",");
        for (int i = 0; i < FEATURE_LEN; i++) {
            strcpy(features[i], pch);
            pch = strtoke(NULL, ",");   
        }

        strcpy(prevKana, currKana);
        strcpy(currKana, features[9]);

#ifdef DEBUG
        printNode(node, features);
#endif

        // Romajify node
        if (features[9][0] != '*' && node->surface[0] != ',') {
            japanese::utf8_kana_to_romaji(features[9], romaji);
            
            //if (prevKana[strlen(prevKana)-1] == (char)'ッ' && romaji != "") { 
            if (((prevKana[strlen(prevKana)-1]&0x3F) | (prevKana[strlen(prevKana)-2]&0x3)<<6) == (L'ッ'&0xFF)) {
                romajiFinal += romaji.front() + romaji;
            }
            else if (strcmp(features[1], "読点") == 0) {
                romajiFinal += (node->next->length == 0) ? "," : ", ";
            }
            else if (strcmp(features[1], "句点") == 0) {
                romajiFinal += (node->next->length == 0) ? "." : ". ";
            }
            else {
                romajiFinal += (node->prev->length == 0 || strlen(prevKana) == 0) ? romaji : " "+romaji;
            }
            
            romaji = "";
        }
        else {
        }
    }
  
    delete tagger;
    return romajiFinal;
}
