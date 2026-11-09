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
inline double readPrice(const std::string& prompt) {
    for (;;) {
        double v;
        if (!util::parseDouble(readLine(prompt), v)) { error("Please enter a valid number (e.g. 25.50)."); continue; }
        if (!Product::isValidPrice(v)) { error("Price must be greater than 0 and at most " + util::money(Product::MAX_PRICE) + "."); continue; }
        return v;
    }
}

inline bool confirm(const std::string& prompt) {
    std::string a = util::toLower(readLine(prompt + " (y/n): "));
    return a == "y" || a == "yes";
}

inline void productTable(const std::vector<const Product*>& list) {
    std::cout << std::left << std::setw(6) << "ID" << std::setw(27) << "Name" << std::setw(15) << "Category" << std::right
              << std::setw(10) << "Price" << std::setw(8) << "Qty" << std::setw(6) << "Min" << "  " << std::left << "Status\n";
    rule('-');
    for (const Product* p : list) {
        std::cout << std::left << std::setw(6) << p->id() << std::setw(27) << util::fit(p->name(), 26) << std::setw(15) << util::fit(p->category(), 14)
                  << std::right << std::setw(10) << util::money(p->price()) << std::setw(8) << p->quantity() << std::setw(6) << p->threshold()
                  << "  " << std::left << (p->quantity() == 0 ? "OUT OF STOCK" : p->isLowStock() ? "LOW STOCK" : "OK") << "\n";
    }
    rule('-');
}

}   



