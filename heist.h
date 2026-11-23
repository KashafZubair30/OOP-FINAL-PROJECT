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

//base class of game exceptions
class GameException {
protected:
    string message;
    bool wrongStep;
public:
    GameException(const string& msg, bool penalty = false)
        : message("Error: " + msg), wrongStep(penalty) {
    }
    virtual ~GameException() {}
    string getMessage() const {
        return message;
    }
    bool isWrongStep() const {
        return wrongStep;
    }
};

class InvalidInputException : public GameException {
public:
    InvalidInputException(const string& msg) : GameException(msg, true) {}
};
class InvalidWordException : public GameException {
public:
    InvalidWordException(const string& msg) : GameException(msg, true) {}
};
class InvalidPositionException : public GameException {
public:
    InvalidPositionException(const string& msg) : GameException(msg, true) {}
};
class SlotFilledException : public GameException {
public:
    SlotFilledException(const string& msg) : GameException(msg, true) {}
};
class RackFullException : public GameException {
public:
    RackFullException(const string& msg) : GameException(msg, false) {}
};
class RackEmptyException : public GameException {
public:
    RackEmptyException(const string& msg) : GameException(msg, false) {}
};
class CredibilityException : public GameException {
public:
    CredibilityException(const string& msg) : GameException(msg, true) {}
};
class FileException : public GameException {
public:
    FileException(const string& msg) : GameException(msg, false) {}
};

//stores the collected letters of one word
class TileRack {
private:
    Tile* tiles;
    int* positions;
    int count;
    int capacity;
    string orderedLetters() const;
public:
    TileRack(int cap = 8);
    TileRack(const TileRack& other);
    TileRack& operator=(const TileRack& other);
    ~TileRack();

    void addTile(char letter, int position);
    char removeTile();
    void display() const;
    int getCount() const;
    bool isFull() const;
    bool isEmpty() const;
    //merges two racks into one clue string
    string operator+(const TileRack& other) const;
};
//word 1 has 7 tiles and 2 has 8 tiles in scrabble
class Scrabble {
private:
    Tile* row1;
    Tile* row2;
    string targetWord1;
    string targetWord2;
    bool solved1;
    bool solved2;
public:
    Scrabble();
    Scrabble(const Scrabble& other);
    Scrabble& operator=(const Scrabble& other);
    ~Scrabble();

    void loadWords(const string& w1, const string& w2);
    void placeLetter(const Clue& clue);
    bool isRowComplete(WordRow row) const;
    string getRowWord(WordRow row) const;
    string getTargetWord(WordRow row) const;
    bool checkWord(WordRow row, const string& guess);
    bool isSolved(WordRow row) const;
    bool bothSolved() const;
    void display() const;

    friend ostream& operator<<(ostream& os, const Scrabble& s);
    friend istream& operator>>(istream& is, Scrabble& s);
};

//cottage holds the 7 searchable location
class Cottage {
private:
    Location* locationList;
    int locationCount;
public:
    Cottage();
    Cottage(const Cottage& other);
    Cottage& operator=(const Cottage& other);
    ~Cottage();

    void loadFromFile(const string& fileName, int culpritColumn);
    int getLocationCount() const;
    bool isSearched(int index) const;
    string getLocationName(int index) const;
    Clue getClue(int index) const;
    void markSearched(int index);
    void displayLocations() const;
};

class GameEngine;

//abstract base class of everyone in the game
class Character {
protected:
    string characterName;
    string characterRole;
    string nationality;
    bool isCulprit;
    bool interviewed;
    void showProfile() const;
public:
    Character(const string& name = "", const string& role = "", const string& nat = "");
    virtual ~Character();

    virtual void display() const = 0;             // pure virtual: every child must override
    virtual int getMinCredibility() const;        // credibility needed to interview
    virtual Clue getClue() const;                 // letter + position for Word 2

    string getName() const;
    string getRole() const;
    string getNationality() const;
    bool getIsCulprit() const;
    bool isInterviewed() const;
    void setInterviewed(bool value);
};

//player
class Detective : public Character {
private:
    int credibility;
    int turns;
public:
    Detective(const string& name = "Detective", int startCredibility = 100);
    void display() const;
    void loseCredibility(int points);
    int getCredibility() const;
    int getTurns() const;
    void useTurn();
};

//the victim interviewed but not a suspect
class Victim : public Character {
private:
    char letterClueChar;
    int boardPosChar;
    WordRow clueRowChar;
public:
    Victim(const string& name, const string& role, const string& nat, char letter, int pos);
    void display() const;
    int getMinCredibility() const;
    Clue getClue() const;
};

//abstract suspect, stores the word 2 letter data shared by all suspects
class Suspect : public Character {
protected:
    char letterClueChar;
    int boardPosChar;
    WordRow clueRowChar;
public:
    Suspect(const string& name, const string& role, const string& nat, char letter, int pos);
    virtual ~Suspect();
    Clue getClue() const;
    virtual string getCategory() const = 0;
};

//virtual inheritance from suspect
class BusinessRelations : virtual public Suspect {
public:
    BusinessRelations(const string& name, const string& role, const string& nat, char letter, int pos);
    virtual ~BusinessRelations();
    string getCategory() const;
};

class CloseRelations : virtual public Suspect {
public:
    CloseRelations(const string& name, const string& role, const string& nat, char letter, int pos);
    virtual ~CloseRelations();
    string getCategory() const;
};

class HouseHelp : public Suspect {
public:
    HouseHelp(const string& name, const string& role, const string& nat, char letter, int pos);
    virtual ~HouseHelp();
    string getCategory() const;
};

class Partner : public BusinessRelations {
public:
    Partner(const string& name, const string& role, const string& nat, char letter, int pos);
    void display() const;
    int getMinCredibility() const;
};

// PA inherits from both BusinessRelations and CloseRelations
class PA : public BusinessRelations, public CloseRelations {
public:
    PA(const string& name, const string& role, const string& nat, char letter, int pos);
    void display() const;
    int getMinCredibility() const;
    string getCategory() const;
};

class Cook : public HouseHelp {
public:
    Cook(const string& name, const string& role, const string& nat, char letter, int pos);
    void display() const;
};

class Guard : public HouseHelp {
public:
    Guard(const string& name, const string& role, const string& nat, char letter, int pos);
    void display() const;
};

class Servant : public HouseHelp {
public:
    Servant(const string& name, const string& role, const string& nat, char letter, int pos);
    void display() const;
};

class OwnersChild : public CloseRelations {
private:
    int age;
public:
    OwnersChild(const string& name, const string& role, const string& nat, char letter, int pos, int childAge = 17);
    void display() const;
    int getMinCredibility() const;
};
#endif
