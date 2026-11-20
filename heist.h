#ifndef HEIST_H
#define HEIST_H

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

//word row (word 1 = locations, word 2 = characters) a letter belongs to
enum WordRow { WORD_1 = 1, WORD_2 = 2 };

//state of the player in game
enum GameState { STATE_MENU, STATE_PLAYING, STATE_WON, STATE_LOST, STATE_EXIT };

//options of the menu
enum MenuChoice { MENU_NEW_GAME = 1, MENU_LEADERBOARD, MENU_ABOUT, MENU_RULES, MENU_EXIT };


//one slot of scrabble
struct Tile {
    char tileLetter;
    bool filled;
    Tile() : tileLetter(' '), filled(false) {}
};

//clue carries a letter and its board position from a location or character to the board
struct Clue {
    char clueLetter;
    int cluePosition;
    WordRow clueRow;
    Clue() : clueLetter(' '), cluePosition(0), clueRow(WORD_1) {}
    bool operator==(const Clue& other) const {
        return clueLetter == other.clueLetter &&
            cluePosition == other.cluePosition &&
            clueRow == other.clueRow;
    }
};

//searchable place, gives 1 letter of word 1
struct Location {
    string locName;
    char letterClueLoc;
    int boardPosLoc;
    WordRow locRow;
    bool locSearchFlag;
    Location() : locName(""), letterClueLoc(' '), boardPosLoc(0), locRow(WORD_1), locSearchFlag(false) {}
};

//all helper functions
string trimString(const string& s);
string toUpperString(const string& s);
string intToString(int value);
int stringToInt(const string& s, bool& ok);
int splitLine(const string& line, string fields[], int maxFields);
string normalizeRole(const string& role);
string getTodayDate();
void clearScreen();

#endif
