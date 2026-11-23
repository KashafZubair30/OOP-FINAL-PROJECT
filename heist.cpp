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
TileRack::TileRack(int cap) : tiles(0), positions(0), count(0), capacity(cap) {
    tiles = new Tile[capacity];
    positions = new int[capacity];
    for (int i = 0; i < capacity; i++)
        positions[i] = 0;
}

//deep copy constructor
TileRack::TileRack(const TileRack& other) : tiles(0), positions(0), count(other.count), capacity(other.capacity) {
    tiles = new Tile[capacity];
    positions = new int[capacity];
    for (int i = 0; i < capacity; i++) {
        tiles[i] = other.tiles[i];
        positions[i] = other.positions[i];
    }
}

//deep copy assignment
TileRack& TileRack::operator=(const TileRack& other) {
    if (this != &other) {
        delete[] tiles;
        delete[] positions;
        count = other.count;
        capacity = other.capacity;
        tiles = new Tile[capacity];
        positions = new int[capacity];
        for (int i = 0; i < capacity; i++) {
            tiles[i] = other.tiles[i];
            positions[i] = other.positions[i];
        }
    }
    return *this;
}

//destructor 
TileRack::~TileRack() {
    delete[] tiles;
    delete[] positions;
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

//creates a cottage with 7 empty locations
Cottage::Cottage() : locationList(0), locationCount(7) {
    locationList = new Location[7];
}

//deep copy constructor
Cottage::Cottage(const Cottage& other) : locationList(0), locationCount(other.locationCount) {
    locationList = new Location[locationCount];
    for (int i = 0; i < locationCount; i++)
        locationList[i] = other.locationList[i];
}

// Deep copy assignment
Cottage& Cottage::operator=(const Cottage& other) {
    if (this != &other) {
        delete[] locationList;
        locationCount = other.locationCount;
        locationList = new Location[locationCount];
        for (int i = 0; i < locationCount; i++)
            locationList[i] = other.locationList[i];
    }
    return *this;
}

//destructor for cottage
Cottage::~Cottage() {
    delete[] locationList;
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

