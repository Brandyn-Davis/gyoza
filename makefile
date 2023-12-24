gyoza: gyoza.cpp romaji.cpp romaji.h utf8.cpp utf8.h
	g++ -g -Wall test.cpp gyoza.cpp romaji.cpp utf8.cpp -o gyoza -lmecab
