#include "gyoza.h"

#define CHECK(eval) if(!eval){std::cerr<<"Exception\n";}

//#define MECAB_ARGS "-d /usr/local/lib/mecab/dic/unidic/"
#define MECAB_ARGS "-d /var/lib/mecab/dic/unidic"
#define FEATURE_LEN 17
#define FEATURE_STR_LEN 128
#define FEATURESTR_LEN 256
#define INPUT_MAX_LEN 1024
#define DICTSTR_MAX_LEN 256
#define FW_DIG_SIZE 3   // Number of bytes in a fullwidth digit

#define IS_SMALL_TSU(s) ((s[strlen(s)-1]&0x3F)|(s[strlen(s)-2]&0x3)<<6)==(L'ッ'&0xFF)
#define ASCII_TO_UTF8(c) ((!isupper(c)<<8)|(((c-0x20)&0x3f)|0x80))|0xefbc00

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

bool isfullalpha(const char* str) {
    char tempStr[3];
    uint32_t c = 0;
    memcpy(tempStr, str, FW_DIG_SIZE);
    std::reverse(tempStr, tempStr+FW_DIG_SIZE);
    memcpy(&c, tempStr, FW_DIG_SIZE);

    return (c >= 0xefbca1 && c <= 0xefbcba) || (c >= 0xefbd81 && c <= 0xefbd9a);
}

// Halfwidth string characters to fullwidth characters
void halfToFull(char* str, char* retStr) {
    for (size_t i = 0; i < strlen(str); i++) {
        uint32_t c = ASCII_TO_UTF8(str[i]);
        memcpy(retStr+i*FW_DIG_SIZE, &c, FW_DIG_SIZE);
        std::reverse(retStr+i*FW_DIG_SIZE, retStr+(i+1)*FW_DIG_SIZE);
    }
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

void normalizeNums(char* str) {
    int tempStrCounter = 0;
    char tempStr[INPUT_MAX_LEN] = "";

    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] >= '0' && str[i] <= '9') {           // If is halfwidth digit,
            uint64_t c = 0x90BCEF|((str[i]&0xF)<<16);   // Turn into fullwidth digit
            memcpy(tempStr+tempStrCounter, &c, FW_DIG_SIZE);    // Append fullwidth digit to temp string
            tempStrCounter += FW_DIG_SIZE;
        }
        else {
            tempStr[tempStrCounter++] = str[i];
        }
    }

    strcpy(str, tempStr);
}

std::string Gyoza::romaji(char* jpText) {
    normalizeNums(jpText);

    // Parse input as nodes
    node = tagger->parseToNode(jpText);
    CHECK(node);

//    while (node->next != nullptr) {
//        std::cout << node->feature << std::endl;
//        node = node->next;
//    }
//    exit(1);

    std::string romaji;
    std::string romajiFinal;
    char* pch;
    char currKana[FEATURE_STR_LEN] = "";

    // Print words (nodes)
    for (; node; node = node->next) {
        char prevKana[FEATURE_STR_LEN] = "";
        char featureStr[FEATURESTR_LEN] = "";
        char features[FEATURE_LEN][FEATURE_STR_LEN] = {""};
        strcpy(featureStr, node->feature);  // get non-const version

        if (strlen(featureStr))

        // store first few node features in array
//        pch = strtoke(featureStr, ",");
//        for (int i = 0; i < FEATURE_LEN; i++) {
//            strcpy(features[i], pch);
//            pch = strtoke(NULL, ",");   
//        }
        pch = strtoke(featureStr, ",");
        for (int i = 0; i < FEATURE_LEN; i++) {
            if (pch == NULL) {
                strcpy(features[i], "*");
            }
            else {
                strcpy(features[i], pch);
                pch = strtoke(NULL, ",");   
            }
        }

        strcpy(prevKana, currKana);
        memset(currKana, 0, sizeof(currKana));
        strcpy(currKana, features[9]);

#ifdef DEBUG
        printNode(node, features);
#endif

        // Romajify node
        //if (node->surface[0] != ',' && strcmp(node->surface, "") != 0) {
        //if (features[9][0] != '*' && node->surface[0] != ',') {
        if (strcmp(features[0], "BOS/EOS") != 0 && node->surface[0] != ',') {
            if (strcmp(features[9], "*") == 0 && isalpha(node->surface[0])) {
                char hwStr[FEATURE_STR_LEN] = "";
                char fwStr[FEATURE_STR_LEN] = "";
                memcpy(hwStr, node->surface, node->length);
                halfToFull(hwStr, fwStr);
                japanese::utf8_kana_to_romaji(fwStr, romaji);
            }
            else if (strcmp(features[9], "*") == 0 && isfullalpha(node->surface)) {
                char tempStr[FEATURE_STR_LEN] = "";
                memcpy(tempStr, node->surface, node->length);
                japanese::utf8_kana_to_romaji(tempStr, romaji);
            }
            else {
                japanese::utf8_kana_to_romaji(features[9], romaji);
            }
            
            //if (prevKana[strlen(prevKana)-1] == (char)'ッ' && romaji != "") { 
            //if (((prevKana[strlen(prevKana)-1]&0x3F) | (prevKana[strlen(prevKana)-2]&0x3)<<6) == (L'ッ'&0xFF)) {
            if (IS_SMALL_TSU(prevKana)) {
                romajiFinal += romaji.front() + romaji;
            }
            else if (strcmp(features[1], "読点") == 0) {
                romajiFinal += ",";
                //romajiFinal += (node->next->length == 0) ? "," : ", ";
            }
            else if (strcmp(features[1], "句点") == 0) {
                romajiFinal += ".";
                //romajiFinal += (node->next->length == 0) ? "." : ". ";
            }
//            else if (strcmp(features[1], "括弧開") || strcmp(features[1], "括弧閉")) {
//            }
//            else if (features[9][0] == '*') {
//                romajiFinal += ();
//            }
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
