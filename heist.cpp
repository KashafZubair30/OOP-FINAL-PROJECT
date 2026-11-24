#include "heist.h"

// removes spaces, tabs and line endings from both ends of string
string trimString(const string& s) {
    int start = 0;
    int end = (int)s.length() - 1;
    while (start <= end && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n'))
        start++;
    while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n'))
        end--;
    return s.substr(start, end - start + 1);
}

//converts every lowercase letter to uppercase
string toUpperString(const string& s) {
    string result = s;
    for (int i = 0; i < (int)result.length(); i++) {
        if (result[i] >= 'a' && result[i] <= 'z')
            result[i] = result[i] - 'a' + 'A';
    }
    return result;
}

//converts integer to text
string intToString(int value) {
    if (value == 0)
        return "0";
    bool negative = value < 0;
    if (negative)
        value = -value;
    string digits = "";
    while (value > 0) {
        digits = char('0' + value % 10) + digits;
        value /= 10;
    }
    if (negative)
        digits = "-" + digits;
    return digits;
}

//converts text to an integer, ok becomes false if the text is not a valid number
int stringToInt(const string& s, bool& ok) {
    string t = trimString(s);
    ok = false;
    if (t.length() == 0 || t.length() > 9)
        return 0;
    int i = 0;
    bool negative = false;
    if (t[0] == '-') {
        negative = true;
        i = 1;
        if (t.length() == 1)
            return 0;
    }
    int value = 0;
    for (; i < (int)t.length(); i++) {
        if (t[i] < '0' || t[i] > '9')
            return 0;
        value = value * 10 + (t[i] - '0');
    }
    ok = true;
    return negative ? -value : value;
}

//splits a line like "a|b|c" into fields; returns how many fields the line really has
int splitLine(const string& line, string fields[], int maxFields) {
    string current = "";
    int count = 0;
    string text = trimString(line);
    // ignore a leading or trailing pipe, e.g. "| a | b |"
    if (text.length() > 0 && text[0] == '|')
        text = text.substr(1);
    if (text.length() > 0 && text[text.length() - 1] == '|')
        text = text.substr(0, text.length() - 1);
    for (int i = 0; i <= (int)text.length(); i++) {
        if (i == (int)text.length() || text[i] == '|') {
            if (count < maxFields)
                fields[count] = trimString(current);
            count++;
            current = "";
        }
        else {
            current += text[i];
        }
    }
    return count;
}

//makes roles comparable: "Partner 1" and "partner_1" become "PARTNER_1"
string normalizeRole(const string& role) {
    string r = toUpperString(trimString(role));
    for (int i = 0; i < (int)r.length(); i++) {
        if (r[i] == ' ')
            r[i] = '_';
    }
    return r;
}

//returns date as dd-mm-yy
string getTodayDate() {
    time_t now = time(0);
    tm* t = localtime(&now);
    int d = t->tm_mday;
    int m = t->tm_mon + 1;
    int y = (t->tm_year + 1900) % 100;
    string result = "";
    result += (d < 10 ? "0" : "") + intToString(d) + "-";
    result += (m < 10 ? "0" : "") + intToString(m) + "-";
    result += (y < 10 ? "0" : "") + intToString(y);
    return result;
}

//clears the console
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

//creates an empty rack that can hold cap tiles
TileRack::TileRack(int cap) : tiles(cap), positions(cap), count(0), capacity(cap) {
    // earlier raw dma logic, replaced by DynamicArray<T> (module 8):
    // tiles = new Tile[capacity];
    // positions = new int[capacity];
}

//deep copy constructor
TileRack::TileRack(const TileRack& other) : tiles(other.tiles), positions(other.positions), count(other.count), capacity(other.capacity) {
    // earlier raw dma logic:
    // tiles = new Tile[capacity];
    // for (int i = 0; i < capacity; i++) tiles[i] = other.tiles[i];
}

//deep copy assignment
TileRack& TileRack::operator=(const TileRack& other) {
    if (this != &other) {
        tiles = other.tiles;
        positions = other.positions;
        count = other.count;
        capacity = other.capacity;
    }
    return *this;
}

//destructor 
TileRack::~TileRack() {
    // earlier raw dma logic:
    // delete[] tiles;
    // delete[] positions;
}

//adds a letter and its board position to the rack
void TileRack::addTile(char letter, int position) {
    if (count >= capacity)
        throw RackFullException("The rack is full");
    tiles[count].tileLetter = letter;
    tiles[count].filled = true;
    positions[count] = position;
    count++;
}

//removes the most recently added letter and returns it
char TileRack::removeTile() {
    if (count == 0)
        throw RackEmptyException("The rack is empty");
    count--;
    char letter = tiles[count].tileLetter;
    tiles[count] = Tile();
    positions[count] = 0;
    return letter;
}

//shows every letter with its position
void TileRack::display() const {
    if (count == 0) {
        cout << " empty ";
        return;
    }
    for (int i = 0; i < count; i++)
        cout << tiles[i].tileLetter << " - " << positions[i] << "  ";
}

//how many letters are stored
int TileRack::getCount() const {
    return count;
}

//true when no more tiles fit
bool TileRack::isFull() const {
    return count >= capacity;
}

//true when nothing is stored
bool TileRack::isEmpty() const {
    return count == 0;
}

//builds the letters
string TileRack::orderedLetters() const {
    string result = "";
    for (int p = 1; p <= capacity; p++) {
        char ch = '_';
        for (int i = 0; i < count; i++) {
            if (positions[i] == p)
                ch = tiles[i].tileLetter;
        }
        result += ch;
    }
    return result;
}

//merges two racks into a single clue
string TileRack::operator+(const TileRack& other) const {
    return orderedLetters() + " " + other.orderedLetters();
}

//creates the board with row 1 having 7 slots, row 2 having 8 slots
Scrabble::Scrabble() : row1(7), row2(8), targetWord1(""), targetWord2(""), solved1(false), solved2(false) {
    // earlier raw dma logic (jagged rows):
    // Tile* row1 = new Tile[7];
    // Tile* row2 = new Tile[8];
}

//deep copy constructor
Scrabble::Scrabble(const Scrabble& other) : row1(other.row1), row2(other.row2), targetWord1(other.targetWord1),
targetWord2(other.targetWord2), solved1(other.solved1), solved2(other.solved2) {
}

//deep copy assignment
Scrabble& Scrabble::operator=(const Scrabble& other) {
    if (this != &other) {
        row1 = other.row1;
        row2 = other.row2;
        targetWord1 = other.targetWord1;
        targetWord2 = other.targetWord2;
        solved1 = other.solved1;
        solved2 = other.solved2;
    }
    return *this;
}

//destructor for scrabble class
Scrabble::~Scrabble() {
    // earlier raw dma logic:
    // delete[] row1;
    // delete[] row2;
}

//stores the two target words
void Scrabble::loadWords(const string& w1, const string& w2) {
    string a = toUpperString(trimString(w1));
    string b = toUpperString(trimString(w2));
    if ((int)a.length() != row1.getSize())
        throw InvalidWordException("Word 1 must have exactly 7 letters");
    if ((int)b.length() != row2.getSize())
        throw InvalidWordException("Word 2 must have exactly 8 letters");
    targetWord1 = a;
    targetWord2 = b;
}

//places a letter on its board position after checking position, slot and letter
void Scrabble::placeLetter(const Clue& clue) {
    // pick the row and its target word with a plain if/else
    DynamicArray<Tile>* row;
    string target;
    if (clue.clueRow == WORD_1) {
        row = &row1;
        target = targetWord1;
    }
    else {
        row = &row2;
        target = targetWord2;
    }
    int index = clue.cluePosition - 1;
    if (index < 0 || index >= row->getSize())
        throw InvalidPositionException("Board position " + intToString(clue.cluePosition) + " is not valid");
    if ((*row)[index].filled)
        throw SlotFilledException("Board position " + intToString(clue.cluePosition) + " is already filled");
    // the clue must match the expected letter of the target word
    Clue expected;
    expected.clueLetter = target[index];
    expected.cluePosition = clue.cluePosition;
    expected.clueRow = clue.clueRow;
    if (!(clue == expected))
        throw InvalidWordException("Letter " + string(1, clue.clueLetter) + " does not belong at this position");
    (*row)[index].tileLetter = clue.clueLetter;
    (*row)[index].filled = true;
}

//true when every slot of the row has a letter
bool Scrabble::isRowComplete(WordRow row) const {
    const DynamicArray<Tile>* r;
    if (row == WORD_1)
        r = &row1;
    else
        r = &row2;
    for (int i = 0; i < r->getSize(); i++) {
        if (!(*r)[i].filled)
            return false;
    }
    return true;
}

//returns the letters currently on a row 
string Scrabble::getRowWord(WordRow row) const {
    const DynamicArray<Tile>* r;
    if (row == WORD_1)
        r = &row1;
    else
        r = &row2;
    string word = "";
    for (int i = 0; i < r->getSize(); i++) {
        if ((*r)[i].filled)
            word += (*r)[i].tileLetter;
        else
            word += '_';
    }
    return word;
}

//returns the secret target word of a row
string Scrabble::getTargetWord(WordRow row) const {
    return (row == WORD_1) ? targetWord1 : targetWord2;
}

//checks the typed word; marks the row solved if it is correct
bool Scrabble::checkWord(WordRow row, const string& guess) {
    string g = toUpperString(trimString(guess));
    if (g == getTargetWord(row)) {
        if (row == WORD_1)
            solved1 = true;
        else
            solved2 = true;
        return true;
    }
    return false;
}

//true when the given row was solved by the detective
bool Scrabble::isSolved(WordRow row) const {
    return (row == WORD_1) ? solved1 : solved2;
}

//true when both words are solved
bool Scrabble::bothSolved() const {
    return solved1 && solved2;
}

//prints the board
void Scrabble::display() const {
    cout << "Scrabble:" << endl;
    cout << "  Word 1: " << getRowWord(WORD_1) << endl;
    cout << "  Word 2: " << getRowWord(WORD_2) << endl;
}

//creates a cottage with 7 empty locations
Cottage::Cottage() : locationList(7), locationCount(7) {
    // earlier raw dma logic:
    // Location* locationList = new Location[7];
}

//deep copy constructor
Cottage::Cottage(const Cottage& other) : locationList(other.locationList), locationCount(other.locationCount) {
}

// Deep copy assignment
Cottage& Cottage::operator=(const Cottage& other) {
    if (this != &other) {
        locationList = other.locationList;
        locationCount = other.locationCount;
    }
    return *this;
}

//destructor for cottage
Cottage::~Cottage() {
    // earlier raw dma logic:
    // delete[] locationList;
}

//reads locations.txt and picks the letter column of the chosen culprit
void Cottage::loadFromFile(const string& fileName, int culpritColumn) {
    ifstream file(fileName.c_str());
    if (!file)
        throw FileException(fileName + " was not found");
    string line;
    getline(file, line);
    int index = 0;
    while (getline(file, line)) {
        line = trimString(line);
        if (line.length() == 0)
            continue;
        string f[12];
        int n = splitLine(line, f, 12);
        bool ok;
        int pos = stringToInt(f[1], ok);
        if (n != 9 || !ok || pos < 1 || pos > 7 || f[2 + culpritColumn].length() != 1 || index >= 7)
            throw FileException(fileName + " is corrupted");
        locationList[index].locName = f[0];
        locationList[index].boardPosLoc = pos;
        locationList[index].letterClueLoc = f[2 + culpritColumn][0];
        locationList[index].locRow = WORD_1;
        locationList[index].locSearchFlag = false;
        index++;
    }
    if (index != 7)
        throw FileException(fileName + " must contain 7 locations");
}

//number of locations in the cottage
int Cottage::getLocationCount() const {
    return locationCount;
}

//true if the location was searched before
bool Cottage::isSearched(int index) const {
    return locationList[index].locSearchFlag;
}

//name of a location
string Cottage::getLocationName(int index) const {
    return locationList[index].locName;
}

//builds a clue from a location 
Clue Cottage::getClue(int index) const {
    Clue c;
    c.clueLetter = locationList[index].letterClueLoc;
    c.cluePosition = locationList[index].boardPosLoc;
    c.clueRow = locationList[index].locRow;
    return c;
}

//marks a location as searched
void Cottage::markSearched(int index) { locationList[index].locSearchFlag = true; }

//prints the numbered list of locations
void Cottage::displayLocations() const {
    cout << "====== Location Menu ======" << endl;
    for (int i = 0; i < locationCount; i++)
        cout << "[" << (i + 1) << "] " << locationList[i].locName << " - "
            << (locationList[i].locSearchFlag ? "Searched" : "NOT Searched") << endl;
    cout << "[0] Back" << endl;
    cout << "===========================" << endl;
}

//sets the common data of every character
Character::Character(const string& name, const string& role, const string& nat) : characterName(name), characterRole(role), nationality(nat), isCulprit(false), interviewed(false) {
}

//virtual destructor 
Character::~Character() {}

//by default anyone can be interviewed
int Character::getMinCredibility() const {
    return 0;
}

//default clue 
Clue Character::getClue() const {
    return Clue();
}

//getters
string Character::getName() const {
    return characterName;
}
string Character::getRole() const {
    return characterRole;
}
string Character::getNationality() const {
    return nationality;
}
bool Character::getIsCulprit() const {
    return isCulprit;
}
bool Character::isInterviewed() const {
    return interviewed;
}
void Character::setInterviewed(bool value) {
    interviewed = value;
}

//prints name, role and nationality
void Character::showProfile() const {
    cout << "  Name: " << characterName << " | Role: " << characterRole
        << " | Nationality: " << nationality << endl;
}

//creates the player with starting credibility and zero turns
Detective::Detective(const string& name, int startCredibility)
    : Character(name, "Detective", "Malay"), credibility(startCredibility), turns(0) {
}

//shows the detective's status
void Detective::display() const {
    showProfile();
    cout << "  Credibility: " << credibility << " | Turns used: " << turns << endl;
}

//reduces credibility but never below zero
void Detective::loseCredibility(int points) {
    credibility -= points;
    if (credibility < 0)
        credibility = 0;
}

int Detective::getCredibility() const {
    return credibility;
}
int Detective::getTurns() const {
    return turns;
}

// engine calls this to count turn
void Detective::useTurn() {
    turns++;
}

//victim stores its own Word 2 letter data
Victim::Victim(const string& name, const string& role, const string& nat, char letter, int pos)
    : Character(name, role, nat), letterClueChar(letter), boardPosChar(pos), clueRowChar(WORD_2) {
}

//victim speaks about the robbery
void Victim::display() const {
    showProfile();
    cout << "  \"The locker behind the bookshelf was emptied during my party. Find the thief\"" << endl;
}

// The victim can be interviewed only if credibility >= 80
int Victim::getMinCredibility() const {
    return 80;
}

//letter and position of the victim
Clue Victim::getClue() const {
    Clue c;
    c.clueLetter = letterClueChar;
    c.cluePosition = boardPosChar;
    c.clueRow = clueRowChar;
    return c;
}

//suspect keeps the letter data of Word 2
Suspect::Suspect(const string& name, const string& role, const string& nat, char letter, int pos)
    : Character(name, role, nat), letterClueChar(letter), boardPosChar(pos), clueRowChar(WORD_2) {
}

Suspect::~Suspect() {}

//letter and position of this suspect
Clue Suspect::getClue() const {
    Clue c;
    c.clueLetter = letterClueChar;
    c.cluePosition = boardPosChar;
    c.clueRow = clueRowChar;
    return c;
}

BusinessRelations::BusinessRelations(const string& name, const string& role, const string& nat, char letter, int pos)
    : Suspect(name, role, nat, letter, pos) {
}
BusinessRelations::~BusinessRelations() {}
string BusinessRelations::getCategory() const { return "Business Relations"; }

CloseRelations::CloseRelations(const string& name, const string& role, const string& nat, char letter, int pos)
    : Suspect(name, role, nat, letter, pos) {
}
CloseRelations::~CloseRelations() {}
string CloseRelations::getCategory() const { return "Close Relations"; }

HouseHelp::HouseHelp(const string& name, const string& role, const string& nat, char letter, int pos)
    : Suspect(name, role, nat, letter, pos) {
}
HouseHelp::~HouseHelp() {}
string HouseHelp::getCategory() const { return "House Help"; }

Partner::Partner(const string& name, const string& role, const string& nat, char letter, int pos)
    : Suspect(name, role, nat, letter, pos), BusinessRelations(name, role, nat, letter, pos) {
}
void Partner::display() const {
    showProfile();
    cout << "  Category: " << getCategory() << endl;
    cout << "  \"I was discussing a deal downstairs. Ask the others.\"" << endl;
}
int Partner::getMinCredibility() const { return 50; }

PA::PA(const string& name, const string& role, const string& nat, char letter, int pos)
    : Suspect(name, role, nat, letter, pos),
    BusinessRelations(name, role, nat, letter, pos),
    CloseRelations(name, role, nat, letter, pos) {
}
void PA::display() const {
    showProfile();
    cout << "  Category: " << getCategory() << endl;
    cout << "  \"I am the owner's right hand. I know all his secrets.\"" << endl;
}
int PA::getMinCredibility() const {
    return 35;
}
// PA belongs to two groups, so it resolves the ambiguity itself
string PA::getCategory() const {
    return "Business Relations & Close Relations";
}

Cook::Cook(const string& name, const string& role, const string& nat, char letter, int pos)
    : HouseHelp(name, role, nat, letter, pos) {
}
void Cook::display() const {
    showProfile();
    cout << "  Category: " << getCategory() << endl;
    cout << "  \"I was in the kitchen preparing dinner all evening.\"" << endl;
}

Guard::Guard(const string& name, const string& role, const string& nat, char letter, int pos)
    : HouseHelp(name, role, nat, letter, pos) {
}
void Guard::display() const {
    showProfile();
    cout << "  Category: " << getCategory() << endl;
    cout << "  \"I watched the gate and the garden. Nobody passed me.\"" << endl;
}

Servant::Servant(const string& name, const string& role, const string& nat, char letter, int pos)
    : HouseHelp(name, role, nat, letter, pos) {
}
void Servant::display() const {
    showProfile();
    cout << "  Category: " << getCategory() << endl;
    cout << "  \"I served drinks in the living room and cleaned the bedrooms.\"" << endl;
}

OwnersChild::OwnersChild(const string& name, const string& role, const string& nat, char letter, int pos, int childAge)
    : Suspect(name, role, nat, letter, pos), CloseRelations(name, role, nat, letter, pos), age(childAge) {
}
void OwnersChild::display() const {
    showProfile();
    cout << "  Age: " << age << " | Category: " << getCategory() << endl;
    cout << "  \"I was so scared and I stayed in my room\"" << endl;
}
//child talks only if credibility >= 75
int OwnersChild::getMinCredibility() const {
    return 75;
}

