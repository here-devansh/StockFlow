 #pragma once
#include <climits>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "Product.cpp"
#include "Utils.cpp"

namespace ui {

struct InputClosed {};   

inline void rule(char c = '=') { std::cout << std::string(78, c) << "\n"; }
inline void heading(const std::string& title) {
    std::cout << "\n+" << std::string(76, '=') << "+\n| " << std::left << std::setw(75) << title << "|\n+" << std::string(76, '=') << "+\n";
}
inline void ok(const std::string& m) { std::cout << "[OK]    " << m << "\n"; }
inline void info(const std::string& m) { std::cout << "[INFO]  " << m << "\n"; }
inline void warn(const std::string& m) { std::cout << "[WARN]  " << m << "\n"; }
inline void error(const std::string& m) { std::cout << "[ERROR] " << m << "\n"; }

inline std::string readLine(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string s;
    if (!std::getline(std::cin, s)) throw InputClosed();
    return util::trim(s);   
}

inline int readInt(const std::string& prompt, int lo, int hi) {
    for (;;) {
        int v;
        if (!util::parseInt(readLine(prompt), v)) { error("Please enter a whole number."); continue; }
        if (v < lo || v > hi) { error("Value must be between " + std::to_string(lo) + " and " + std::to_string(hi) + "."); continue; }
        return v;
    }
}