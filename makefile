gyoza: test.cpp gyoza.cpp romaji-cpp/romaji.cpp romaji-cpp/romaji.h romaji-cpp/utf8.cpp romaji-cpp/utf8.h
	g++ -g -Wall test.cpp gyoza.cpp romaji-cpp/romaji.cpp romaji-cpp/utf8.cpp -o gyoza -lmecab
debug: test.cpp gyoza.cpp romaji-cpp/romaji.cpp romaji-cpp/romaji.h romaji-cpp/utf8.cpp romaji-cpp/utf8.h
	g++ -g -Wall test.cpp gyoza.cpp romaji-cpp/romaji.cpp romaji-cpp/utf8.cpp -o gyoza -lmecab -DDEBUG
