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

