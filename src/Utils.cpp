#include "Utils.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

void printLine(char ch, int count) {
    cout << string(count, ch) << "\n";
}

void printHeading(const string& title) {
    cout << "\n";
    printLine('=');
    cout << "  " << title << "\n";
    printLine('=');
}

void printOk(const string& message) { cout << "[OK]    " << message << "\n"; }
void printError(const string& message) { cout << "[ERROR] " << message << "\n"; }
void printWarning(const string& message) { cout << "[WARN]  " << message << "\n"; }

string trimText(const string& text) {
    int start = 0;
    int end = text.size() - 1;
    while (start <= end && (text[start] == ' ' || text[start] == '\t' || text[start] == '\r')) start++;
    while (end >= start && (text[end] == ' ' || text[end] == '\t' || text[end] == '\r')) end--;
    return text.substr(start, end - start + 1);
}

string toLower(string text) {
    int length = text.size();
    for (int i = 0; i < length; i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') text[i] = text[i] + ('a' - 'A');
    }
    return text;
}

string formatMoney(double value) {
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%.2f", value);
    return string(buffer);
}

bool textToInt(const string& text, int& number) {
    try {
        size_t used = 0;
        int value = stoi(text, &used);
        if (used != text.size()) return false;
        number = value;
        return true;
    } catch (const exception&) {
        return false;
    }
}

bool textToDouble(const string& text, double& number) {
    try {
        size_t used = 0;
        double value = stod(text, &used);
        if (used != text.size()) return false;
        number = value;
        return true;
    } catch (const exception&) {
        return false;
    }
}

string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        cout << "\nInput ended. Goodbye!\n";
        exit(0);
    }
    return trimText(line);
}

int readInt(const string& prompt, int minValue, int maxValue) {
    while (true) {
        int number;
        string text = readLine(prompt);
        if (!textToInt(text, number)) {
            printError("Please enter a whole number.");
        } else if (number < minValue || number > maxValue) {
            printError("The number must be between " + to_string(minValue) + " and " + to_string(maxValue) + ".");
        } else {
            return number;
        }
    }
}

double readDouble(const string& prompt, double minValue, double maxValue) {
    while (true) {
        double number;
        string text = readLine(prompt);
        if (!textToDouble(text, number)) {
            printError("Please enter a number (example 25.50).");
        } else if (!(number >= minValue && number <= maxValue)) {
            printError("The number must be between " + formatMoney(minValue) + " and " + formatMoney(maxValue) + ".");
        } else {
            return number;
        }
    }
}

string readText(const string& prompt) {
    while (true) {
        string text = readLine(prompt);
        if (text.empty() || text.size() > 20 || text.find(',') != string::npos) {
            printError("Enter 1 to 20 characters. A comma (,) is not allowed.");
        } else {
            return text;
        }
    }
}
