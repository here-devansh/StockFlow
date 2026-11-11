 
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Product.h"
#include "Utils.h"

using namespace std;

namespace ui {

struct InputClosed {};


inline void rule(char c = '=') {
    cout << string(50, c) << endl;
}


inline void heading(const string& title) {
    rule();
    cout << title << endl;
    rule();
}


inline void ok(const string& message) {
    cout << "[OK] " << message << endl;
}

inline void info(const string& message) {
    cout << "[INFO] " << message << endl;
}

inline void warn(const string& message) {
    cout << "[WARNING] " << message << endl;
}

inline void error(const string& message) {
    cout << "[ERROR] " << message << endl;
}


inline string readLine(const string& prompt) {
    string value;

    cout << prompt;

    if (!getline(cin, value))
        throw InputClosed();

    return util::trim(value);
}


inline int readInt(const string& prompt, int min, int max) {
    int value;

    while (true) {
        if (!util::parseInt(readLine(prompt), value)) {
            error("Enter a valid whole number.");
            continue;
        }

        if (value < min || value > max) {
            error("Number is out of range.");
            continue;
        }

        return value;
    }
}


inline double readPrice(const string& prompt) {
    double price;

    while (true) {
        if (!util::parseDouble(readLine(prompt), price)) {
            error("Enter a valid price.");
            continue;
        }

        if (!Product::isValidPrice(price)) {
            error("Invalid price.");
            continue;
        }

        return price;
    }
}


inline bool confirm(const string& prompt) {
    string answer = util::toLower(readLine(prompt + " (y/n): "));

    return answer == "y" || answer == "yes";
}

 
inline void productTable(const vector<const Product*>& list) {

    cout << "\nID\tName\tCategory\tPrice\tQuantity\tStatus\n";

    rule('-');

    for (const Product* p : list) {

        cout << p->id() << "\t"
             << p->name() << "\t"
             << p->category() << "\t"
             << util::money(p->price()) << "\t"
             << p->quantity() << "\t";

        if (p->quantity() == 0)
            cout << "OUT OF STOCK";

        else if (p->isLowStock())
            cout << "LOW STOCK";

        else
            cout << "OK";

        cout << endl;
    }

    rule('-');
}

}
