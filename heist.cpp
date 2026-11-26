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

//prints a tile as [A] or [ ]
ostream& operator<<(ostream& os, const Tile& t) {
    os << "[" << (t.filled ? t.tileLetter : ' ') << "]";
    return os;
}

//reads a tile letter from the stream
istream& operator>>(istream& is, Tile& t) {
    is >> t.tileLetter;
    t.filled = true;
    return is;
}

//prints a location with its search status
ostream& operator<<(ostream& os, const Location& loc) {
    os << loc.locName << " - " << (loc.locSearchFlag ? "Searched" : "NOT Searched");
    return os;
}

//reads location name, letter and board position
istream& operator>>(istream& is, Location& loc) {
    is >> loc.locName >> loc.letterClueLoc >> loc.boardPosLoc;
    return is;
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
    cout << *this;
}

//prints both rows with their status
ostream& operator<<(ostream& os, const Scrabble& s) {
    os << "Scrabble:" << endl;
    os << "  Word 1: ";
    for (int i = 0; i < s.row1.getSize(); i++)
        os << s.row1[i] << " ";
    if (s.solved1)
        os << " -> SOLVED (" << s.targetWord1 << ")";
    else if (s.isRowComplete(WORD_1))
        os << " -> COMPLETE";
    os << endl << "  Word 2: ";
    for (int i = 0; i < s.row2.getSize(); i++)
        os << s.row2[i] << " ";
    if (s.solved2)
        os << " -> SOLVED (" << s.targetWord2 << ")";
    else if (s.isRowComplete(WORD_2))
        os << " -> COMPLETE";
    os << endl;
    return os;
}

//reads both target words 
istream& operator>>(istream& is, Scrabble& s) {
    string a, b;
    is >> a >> b;
    s.loadWords(a, b);
    return is;
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
        cout << "[" << (i + 1) << "] " << locationList[i] << endl;
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

//short description of a character
ostream& operator<<(ostream& os, const Character& c) {
    os << c.characterName << " (" << c.characterRole << ")";
    return os;
}

//reads name, role and nationality
istream& operator>>(istream& is, Character& c) {
    is >> c.characterName >> c.characterRole >> c.nationality;
    return is;
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

//stores the score file name
LeaderBoard::LeaderBoard(const string& file) : fileName(file) {}

void LeaderBoard::addScore(const string& detective, const string& result, int credibility,
    int turnsLeft, const string& culprit) {
    ifstream check(fileName.c_str());
    bool exists = check.is_open();       // does the file already exist?
    check.close();
    ofstream out(fileName.c_str(), ios::app);
    if (!out)
        throw FileException("Could not write to " + fileName);
    if (!exists)
        out << "Detective|Result|Credibility|Turns_Left|Culprit|Date" << endl;
    out << detective << "|" << result << "|" << credibility << "|"
        << (turnsLeft < 10 ? "0" : "") << turnsLeft << "|" << culprit << "|" << getTodayDate() << endl;
}

//reads the file and prints all previous scores
void LeaderBoard::displayScores() const {
    ifstream file(fileName.c_str());
    if (!file)
        throw FileException(fileName + " was not found");
    string line;
    string header[6];
    if (!getline(file, line) || splitLine(line, header, 6) != 6)
        throw FileException(fileName + " is corrupted");
    cout << "=========== LEADER / SCORE BOARD ===========" << endl;
    cout << "Detective | Result | Credibility | Turns Left | Culprit | Date" << endl;
    cout << "--------------------------------------------" << endl;
    int rows = 0;
    while (getline(file, line)) {
        line = trimString(line);
        if (line.length() == 0)
            continue;
        string f[6];
        if (splitLine(line, f, 6) != 6)
            throw FileException(fileName + " is corrupted");
        cout << f[0] << " | " << f[1] << " | " << f[2] << " | " << f[3] << " | " << f[4] << " | " << f[5] << endl;
        rows++;
    }
    if (rows == 0)
        cout << "No games played yet." << endl;
    cout << "============================================" << endl;
}


GameEngine* GameEngine::instance = 0;

//sets default rules and empty pointers
GameEngine::GameEngine()
    : state(STATE_MENU), maxTurns(25), startCredibility(100), wrongTurnPenalty(5), wrongWordPenalty(10),
    winCondition("Accuse_Correct_Culprit"), loseCondition("Credibility_Zero_Or_Turns_Exhausted"),
    detective(0), cottage(0), scrabble(0), rackWord1(0), rackWord2(0), leaderBoard(0),
    culpritColumn(0), culpritRole("") {
    for (int i = 0; i < TOTAL_PEOPLE; i++)
        characters[i] = 0;
}

GameEngine::~GameEngine() {
    resetGame();
}

GameEngine* GameEngine::getInstance() {
    if (instance == 0)
        instance = new GameEngine();
    return instance;
}

//deletes the singleton object
void GameEngine::destroyInstance() {
    delete instance;
    instance = 0;
}

//connects the leader board
void GameEngine::setLeaderBoard(LeaderBoard* board) {
    leaderBoard = board;
}

GameState GameEngine::getState() const {
    return state;
}

void GameEngine::resetGame() {
    delete detective;  detective = 0;
    delete cottage;    cottage = 0;
    delete scrabble;   scrabble = 0;
    delete rackWord1;  rackWord1 = 0;
    delete rackWord2;  rackWord2 = 0;
    for (int i = 0; i < TOTAL_PEOPLE; i++) {
        delete characters[i];      // virtual destructor deletes the right type
        characters[i] = 0;
    }
}

string GameEngine::readLine() {
    string line;
    if (!getline(cin, line)) {
        cin.clear();
        state = STATE_EXIT;
        throw InvalidInputException("Input stream closed");
    }
    return line;
}

//reads a whole number between minValue and maxValue
int GameEngine::readNumber(const string& prompt, int minValue, int maxValue) {
    cout << prompt;
    string line = readLine();
    bool ok;
    int value = stringToInt(line, ok);
    if (!ok)
        throw InvalidInputException("Please enter a number");
    if (value < minValue || value > maxValue)
        throw InvalidInputException("Choice must be between " + intToString(minValue) + " and " + intToString(maxValue));
    return value;
}

bool GameEngine::readYesNo(const string& prompt) {
    cout << prompt;
    string answer = toUpperString(trimString(readLine()));
    if (answer == "Y" || answer == "YES")
        return true;
    if (answer == "N" || answer == "NO")
        return false;
    throw InvalidInputException("Please answer Y or N");
}

void GameEngine::pauseScreen() {
    cout << endl << "Press Enter to continue";
    string dummy;
    getline(cin, dummy);
    if (!cin)
        cin.clear();
}

//asks for name
string GameEngine::askDetectiveName() {
    while (true) {
        cout << "Enter detective name: ";
        string name = trimString(readLine());
        if (name.length() == 0) {
            cout << "Name cannot be empty" << endl;
        }
        else if (name.find(' ') != string::npos) {
            cout << "Spaces are not allowed, use . or _ " << endl;
        }
        else {
            return name;
        }
    }
}

//loads rules.txt; 
bool GameEngine::loadRules() {
    int mt = 25, sc = 100, wt = 5, ww = 10;
    string win = "Accuse_Correct_Culprit", lose = "Credibility_Zero_Or_Turns_Exhausted";
    bool good = true;
    int found = 0;
    ifstream file("rules.txt");
    if (!file)
        good = false;
    string line;
    while (good && getline(file, line)) {
        line = trimString(line);
        if (line.length() == 0)
            continue;
        int eq = (int)line.find('=');
        if (eq == (int)string::npos) {
            good = false;
            break;
        }
        string key = trimString(line.substr(0, eq));
        string val = trimString(line.substr(eq + 1));
        bool ok = true;
        int number = 0;
        if (key == "Max_Turns") {
            number = stringToInt(val, ok);
            mt = number; found++;
        }
        else if (key == "Starting_Credibility") {
            number = stringToInt(val, ok);
            sc = number; found++;
        }
        else if (key == "Every_Wrong_Turn") {
            number = stringToInt(val, ok);
            wt = number < 0 ? -number : number;
            found++;
        }
        else if (key == "Every_Wrong_Word") {
            number = stringToInt(val, ok);
            ww = number < 0 ? -number : number;
            found++;
        }
        else if (key == "Win_Condition") {
            win = val;
            found++;
        }
        else if (key == "Lose_Condition") {
            lose = val;
            found++;
        }
        else ok = false;
        if (!ok)
            good = false;
    }
    if (good && (found != 6 || mt <= 0 || sc <= 0))
        good = false;
    if (good) {
        maxTurns = mt;
        startCredibility = sc;
        wrongTurnPenalty = wt;
        wrongWordPenalty = ww;
        winCondition = win;
        loseCondition = lose;
    }
    else {
        maxTurns = 25;
        startCredibility = 100;
        wrongTurnPenalty = 5;
        wrongWordPenalty = 10;
        winCondition = "Accuse_Correct_Culprit";
        loseCondition = "Credibility_Zero_Or_Turns_Exhausted";
    }
    return good;
}

//creates the right Character object 
Character* GameEngine::createCharacter(const string& name, const string& role, const string& nat,
    char letter, int pos) {
    string r = normalizeRole(role);
    if (r == "VICTIM")
        return new Victim(name, role, nat, letter, pos);
    if (r == "COOK")
        return new Cook(name, role, nat, letter, pos);
    if (r == "GUARD")
        return new Guard(name, role, nat, letter, pos);
    if (r == "SERVANT")
        return new Servant(name, role, nat, letter, pos);
    if (r == "CHILD")
        return new OwnersChild(name, role, nat, letter, pos);
    if (r == "PARTNER_1" || r == "PARTNER_2")
        return new Partner(name, role, nat, letter, pos);
    if (r == "PA")
        return new PA(name, role, nat, letter, pos);
    throw FileException("Unknown role '" + role + "' in characters.txt");
}

//reads characters.txt and marks chosen culprit 
void GameEngine::loadCharacters(const string& fileName) {
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
        string f[14];
        int n = splitLine(line, f, 14);
        bool ok;
        int pos = stringToInt(f[3], ok);
        if (n != 12 || !ok || pos < 1 || pos > 8 || f[4 + culpritColumn].length() != 1 || index >= TOTAL_PEOPLE)
            throw FileException(fileName + " is corrupted");
        characters[index] = createCharacter(f[0], f[1], f[2], f[4 + culpritColumn][0], pos);
        index++;
    }
    if (index != TOTAL_PEOPLE)
        throw FileException(fileName + " must contain 8 people");
    if (normalizeRole(characters[0]->getRole()) != "VICTIM")
        throw FileException("First line of " + fileName + " must be the Victim");
    bool found = false;
    for (int i = 1; i < TOTAL_PEOPLE; i++) {
        if (normalizeRole(characters[i]->getRole()) == culpritRole) {
            characters[i]->isCulprit = true;
            found = true;
        }
    }
    if (!found)
        throw FileException("Culprit role " + culpritRole + " not found in " + fileName);
}

//reads clue.txt and loads the two words of the chosen culprit
void GameEngine::loadClueWords(const string& fileName) {
    ifstream file(fileName.c_str());
    if (!file)
        throw FileException(fileName + " was not found");
    string line;
    getline(file, line);
    int lines = 0;
    bool found = false;
    while (getline(file, line)) {
        line = trimString(line);
        if (line.length() == 0)
            continue;
        string f[5];
        if (splitLine(line, f, 5) != 3)
            throw FileException(fileName + " is corrupted");
        lines++;
        if (normalizeRole(f[0]) == culpritRole) {
            scrabble->loadWords(f[1], f[2]);
            found = true;
        }
    }
    if (!found || lines != 7)
        throw FileException(fileName + " is corrupted or missing the culprit role");
}

//finds which role owns two solved words
string GameEngine::findCulpritRole(const string& w1, const string& w2) const {
    ifstream file("clue.txt");
    if (!file)
        throw FileException("clue.txt was not found");
    string line;
    getline(file, line);
    while (getline(file, line)) {
        string f[5];
        if (splitLine(line, f, 5) == 3 && toUpperString(f[1]) == toUpperString(w1) &&
            toUpperString(f[2]) == toUpperString(w2))
            return f[0];
    }
    throw FileException("No culprit matches these words in clue.txt");
}

//prints the game title
void GameEngine::showTitle() const {
    cout << "+====================================================+" << endl;
    cout << "|                                                    |" << endl;
    cout << "|                  THE LOCKER HEIST                  |" << endl;
    cout << "|                                                    |" << endl;
    cout << "+====================================================+" << endl;
    cout << "A locker hidden behind a bookshelf was robbed during a party" << endl;
    cout << "at a wealthy owner's cottage. One of the guests is the culprit." << endl;
    cout << "You are the detective hired to find out who did it." << endl << endl;
}

//prints the main menu
void GameEngine::showMainMenu() const {
    cout << endl;
    cout << "+----------- THE LOCKER HEIST - MAIN MENU -----------+" << endl;
    cout << "  [1] New Game" << endl;
    cout << "  [2] View Leader/Score Board" << endl;
    cout << "  [3] About" << endl;
    cout << "  [4] Rules" << endl;
    cout << "  [5] Exit" << endl;
    cout << "+----------------------------------------------------+" << endl;
}

//prints the in game menu with the current turn number
void GameEngine::showGameMenu() const {
    cout << endl << "==== Game Menu ====" << endl;
    cout << "[1] Visit a Location" << endl;
    cout << "[2] Interview a Person" << endl;
    cout << "[3] View Scrabble Board (free - no turn)" << endl;
    cout << "[4] Form a Word" << endl;
    cout << "[5] Accuse a Suspect" << endl;
    cout << "[6] Quit" << endl;
    cout << endl << "--- TURN " << (detective->getTurns() + 1) << " ---" << endl;
}

//prints credibility, turns and the racks
void GameEngine::showStatus() const {
    cout << endl << "Credibility: " << detective->getCredibility()
        << " | Turns used: " << detective->getTurns() << "/" << maxTurns << endl;
    cout << "Rack (Word 1): ";
    rackWord1->display();
    cout << endl << "Rack (Word 2): ";
    rackWord2->display();
    cout << endl;
}

//shows about.txt
void GameEngine::showAbout() {
    ifstream file("about.txt");
    if (!file)
        throw FileException("about.txt was not found");
    string line;
    cout << endl;
    while (getline(file, line))
        cout << line << endl;
    pauseScreen();
}

//shows the rules
void GameEngine::showRules() {
    bool fromFile = loadRules();
    cout << endl << "=============== RULES ===============" << endl;
    if (!fromFile)
        cout << "Rules.txt missing or corrupt so default rules shown" << endl;
    cout << "Maximum turns        : " << maxTurns << endl;
    cout << "Starting credibility : " << startCredibility << endl;
    cout << "Every wrong turn     : -" << wrongTurnPenalty << endl;
    cout << "Every wrong word     : -" << wrongWordPenalty << endl;
    cout << "Win condition        : " << winCondition << endl;
    cout << "Lose condition       : " << loseCondition << endl;
    cout << "Viewing the board does not use a turn." << endl;
    cout << "=====================================" << endl;
    pauseScreen();
}

//prepares a fresh game
void GameEngine::startNewGame() {
    resetGame();
    clearScreen();
    cout << "+----------- THE LOCKER HEIST - NEW GAME -----------+" << endl;
    bool rulesOk = loadRules();
    if (!rulesOk)
        cout << "rules.txt is missing or corrupt, default rules are used." << endl;
    string name = askDetectiveName();

    culpritColumn = rand() % TOTAL_SUSPECTS;
    string columnRoles[TOTAL_SUSPECTS] = { "COOK", "GUARD", "SERVANT", "PARTNER_1", "PARTNER_2", "PA", "CHILD" };
    culpritRole = columnRoles[culpritColumn];

    detective = new Detective(name, startCredibility);
    cottage = new Cottage();
    scrabble = new Scrabble();
    rackWord1 = new TileRack(7);
    rackWord2 = new TileRack(8);
    cottage->loadFromFile("locations.txt", culpritColumn);
    loadCharacters("characters.txt");
    loadClueWords("clue.txt");

    state = STATE_PLAYING;
    cout << "A culprit has been secretly chosen. The game begins" << endl;
    cout << "You have " << maxTurns << " turns and " << startCredibility << " credibility." << endl;
    cout << "Wrong step: -" << wrongTurnPenalty << " credibility, wrong word: -" << wrongWordPenalty << "." << endl;
    cout << *scrabble;
}

//one step of the main menu
void GameEngine::mainMenuStep() {
    showMainMenu();
    int choice = readNumber("Choice: ", 1, 5);
    switch (choice) {
    case MENU_NEW_GAME:
        startNewGame();
        break;
    case MENU_LEADERBOARD:
        if (leaderBoard == 0)
            throw GameException("Leader board is not connected");
        leaderBoard->displayScores();
        pauseScreen();
        break;
    case MENU_ABOUT:
        showAbout();
        break;
    case MENU_RULES:
        showRules();
        break;
    case MENU_EXIT:
        cout << "Goodbye, Detective" << endl;
        state = STATE_EXIT;
        break;
    }
}

//runs one step of whatever state the game is in
void GameEngine::runStep() {
    if (state == STATE_MENU)
        mainMenuStep();
    else if (state == STATE_PLAYING)
        playTurn();
    else if (state == STATE_WON || state == STATE_LOST)
        finishGame();
}

//one turn of the game
void GameEngine::playTurn() {
    showStatus();
    showGameMenu();
    int choice = readNumber("Choice: ", 1, 6);
    bool turnUsed = false;
    switch (choice) {
    case 1:
        turnUsed = visitLocation();
        break;
    case 2:
        turnUsed = interviewPerson();
        break;
    case 3:
        cout << endl << *scrabble;
        break;
    case 4:
        turnUsed = formWord();
        break;
    case 5:
        turnUsed = accuseSuspect();
        break;
    case 6:
        turnUsed = quitGame();
        break;
    }
    if (turnUsed)
        detective->useTurn();
    if (state == STATE_PLAYING)
        checkEndConditions();
}

//visit a location: first visit collects the letter, repeat visit asks first
bool GameEngine::visitLocation() {
    cottage->displayLocations();
    int pick = readNumber("Pick: ", 0, cottage->getLocationCount());
    if (pick == 0)
        return false;
    int idx = pick - 1;
    string name = cottage->getLocationName(idx);
    Clue clue = cottage->getClue(idx);
    if (cottage->isSearched(idx)) {
        if (!readYesNo("You have already searched " + name + ". Search again? (Y/N): "))
            return false;
        cout << "Letter " << clue.clueLetter << " is at position " << clue.cluePosition << " of Word 1." << endl;
        return true;
    }
    if (!readYesNo("You have NOT searched " + name + " yet. Search? (Y/N): "))
        return false;
    rackWord1->addTile(clue.clueLetter, clue.cluePosition);
    scrabble->placeLetter(clue);
    cottage->markSearched(idx);
    cout << "Letter " << clue.clueLetter << " collected at position " << clue.cluePosition << " of Word 1." << endl;
    cout << "Letter has been placed." << endl;
    return true;
}

//interview a person: credibility rules are handled polymorphically
bool GameEngine::interviewPerson() {
    cout << endl << "====== Character Menu ======" << endl;
    for (int i = 0; i < TOTAL_PEOPLE; i++)
        cout << "[" << (i + 1) << "] " << *characters[i] << " - "
        << (characters[i]->isInterviewed() ? "Interviewed" : "NOT Interviewed") << endl;
    cout << "[0] Back" << endl << "===========================" << endl;
    int pick = readNumber("Pick: ", 0, TOTAL_PEOPLE);
    if (pick == 0)
        return false;
    Character* person = characters[pick - 1];
    Clue clue = person->getClue();
    if (person->isInterviewed()) {
        if (!readYesNo("You have already interviewed " + person->getName() + ". Interview again? (Y/N): "))
            return false;
        cout << "Letter " << clue.clueLetter << " is at position " << clue.cluePosition << " of Word 2." << endl;
        return true;
    }
    if (!readYesNo("You have NOT interviewed " + person->getName() + " yet. Interview? (Y/N): "))
        return false;
    if (detective->getCredibility() < person->getMinCredibility())
        throw CredibilityException(person->getName() + " refuses to talk. Credibility " +
            intToString(person->getMinCredibility()) + " is required");
    rackWord2->addTile(clue.clueLetter, clue.cluePosition);
    scrabble->placeLetter(clue);
    person->setInterviewed(true);
    person->display();                                  // polymorphic call
    cout << "Letter " << clue.clueLetter << " collected at position " << clue.cluePosition << " of Word 2." << endl;
    cout << "Placed." << endl;
    return true;
}

//type a completed word; wrong words cost credibility
bool GameEngine::formWord() {
    cout << "[1] Word 1   [2] Word 2   [0] Back" << endl;
    int pick = readNumber("Which word? ", 0, 2);
    if (pick == 0)
        return false;
    WordRow row = (pick == 1) ? WORD_1 : WORD_2;
    if (scrabble->isSolved(row)) {
        cout << "That word is already solved." << endl;
        return false;
    }
    if (!scrabble->isRowComplete(row))
        throw InvalidWordException("That row is not completely filled yet");
    cout << "Row: " << scrabble->getRowWord(row) << endl;
    while (detective->getCredibility() > 0) {
        cout << "Type the word: ";
        string guess = readLine();
        if (scrabble->checkWord(row, guess)) {
            cout << "Correct! The word is " << scrabble->getTargetWord(row) << endl;
            if (scrabble->bothSolved())
                revealCulprit();
            return true;
        }
        detective->loseCredibility(wrongWordPenalty);
        cout << "Wrong word -" << wrongWordPenalty << " credibility. Credibility left: "
            << detective->getCredibility() << endl;
    }
    return true;
}

//both words solved: read clue.txt and tell the detective the culprit's role
void GameEngine::revealCulprit() {
    string w1 = scrabble->getTargetWord(WORD_1);
    string w2 = scrabble->getTargetWord(WORD_2);
    string role = findCulpritRole(w1, w2);
    string nat = "";
    for (int i = 1; i < TOTAL_PEOPLE; i++) {
        if (normalizeRole(characters[i]->getRole()) == normalizeRole(role))
            nat = characters[i]->getNationality();
    }
    cout << endl << "Combined clue: " << (*rackWord1 + *rackWord2) << endl;
    cout << "Both words solved:" << endl;
    cout << "  " << w1 << " + " << w2 << " = " << nat << " -> " << role << " is the culprit" << endl;
}

// Accuse one of the seven suspects
bool GameEngine::accuseSuspect() {
    cout << endl << "====== Accuse ======" << endl;
    for (int i = 1; i < TOTAL_PEOPLE; i++)
        cout << "[" << i << "] " << *characters[i] << endl;
    cout << "[0] Back" << endl;
    int pick = readNumber("Accuse: ", 0, TOTAL_SUSPECTS);
    if (pick == 0)
        return false;
    cout << "Accuse: " << *characters[pick] << endl;
    if (characters[pick]->isCulprit) {
        cout << characters[pick]->getName() << " is the culprit" << endl;
        state = STATE_WON;
    }
    else {
        detective->loseCredibility(wrongTurnPenalty);
        cout << characters[pick]->getName() << " is innocent -" << wrongTurnPenalty << " credibility." << endl;
    }
    return true;
}

//leave the current game and return to the main menu
bool GameEngine::quitGame() {
    if (readYesNo("Quit this game? No score will be saved (Y/N): ")) {
        state = STATE_MENU;
        cout << "Case abandoned." << endl;
    }
    return false;
}

//decides if the game is lost
void GameEngine::checkEndConditions() {
    if (detective->getCredibility() <= 0) {
        cout << endl << "Your credibility is gone" << endl;
        state = STATE_LOST;
    }
    else if (detective->getTurns() >= maxTurns) {
        cout << endl << "You ran out of turns" << endl;
        state = STATE_LOST;
    }
}

//shows the result, saves it to the leader board and returns to the main menu
void GameEngine::finishGame() {
    bool won = (state == STATE_WON);
    string culpritName = "";
    string role = "";
    for (int i = 1; i < TOTAL_PEOPLE; i++) {
        if (characters[i]->isCulprit) {
            culpritName = characters[i]->getName();
            role = characters[i]->getRole();
        }
    }
    int turnsLeft = maxTurns - detective->getTurns();
    if (turnsLeft < 0)
        turnsLeft = 0;
    cout << endl << "+====================================================+" << endl;
    cout << "   " << detective->getName() << (won ? " WIN!" : " LOSE!") << endl;
    cout << "   Final credibility: " << detective->getCredibility()
        << " | Turns used: " << detective->getTurns() << " | Culprit: " << role << " " << culpritName << endl;
    cout << "+====================================================+" << endl;

    string name = detective->getName();
    int cred = detective->getCredibility();
    state = STATE_MENU;
    if (leaderBoard != 0)
        leaderBoard->addScore(name, won ? "WIN" : "LOSE", cred, turnsLeft, role);
    pauseScreen();
    clearScreen();
    cout << "Screen Cleared" << endl;
}

//prints the error and applies the wrong step penalty if needed
void GameEngine::handleException(const GameException& e) {
    cout << endl << e.getMessage() << endl;
    if (state == STATE_PLAYING && e.isWrongStep() && detective != 0) {
        detective->loseCredibility(wrongTurnPenalty);
        detective->useTurn();
        cout << "Wrong step -" << wrongTurnPenalty << " credibility and the turn is used." << endl;
        checkEndConditions();
    }
    else if (state == STATE_MENU) {
        resetGame();                                    // drop any half-built game
    }
}
