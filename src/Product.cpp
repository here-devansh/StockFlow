#include "Product.h"

#include <iomanip>
#include <iostream>

#include "Utils.h"

using namespace std;

bool Product::isValidText(const string& text) {
    return !text.empty() && text.size() <= 20 && text.find(',') == string::npos;
}

bool Product::isValidPrice(double price) {
    return price > 0 && price <= MAX_PRICE;
}

bool Product::isValidQuantity(int quantity) {
    return quantity >= 0 && quantity <= MAX_QUANTITY;
}

Product::Product(int id, const string& name, const string& category, double price, int quantity, int threshold) {
    this->id = id;
    this->name = name;
    this->category = category;
    this->price = price;
    this->quantity = quantity;
    this->threshold = threshold;
}

int Product::getId() const { return id; }
string Product::getName() const { return name; }
string Product::getCategory() const { return category; }
double Product::getPrice() const { return price; }
int Product::getQuantity() const { return quantity; }
int Product::getThreshold() const { return threshold; }

bool Product::isLowStock() const {
    return quantity <= threshold;
}

bool Product::setName(const string& newName) {
    if (!isValidText(newName)) return false;
    name = newName;
    return true;
}

bool Product::setCategory(const string& newCategory) {
    if (!isValidText(newCategory)) return false;
    category = newCategory;
    return true;
}

bool Product::setPrice(double newPrice) {
    if (!isValidPrice(newPrice)) return false;
    price = newPrice;
    return true;
}

bool Product::setThreshold(int newThreshold) {
    if (!isValidQuantity(newThreshold)) return false;
    threshold = newThreshold;
    return true;
}

bool Product::addStock(int amount) {
    if (amount <= 0) return false;
    if (quantity + amount > MAX_QUANTITY) return false;
    quantity = quantity + amount;
    return true;
}

bool Product::removeStock(int amount) {
    if (amount <= 0) return false;
    if (amount > quantity) return false;
    quantity = quantity - amount;
    return true;
}

void Product::printHeader() {
    cout << left << setw(6) << "ID" << setw(21) << "Name" << setw(21) << "Category" << right << setw(9) << "Price"
         << setw(7) << "Qty" << setw(6) << "Min" << "  " << left << "Status" << "\n";
    printLine('-', 80);
}

void Product::display() const {
    string status = "OK";
    if (quantity == 0) status = "OUT OF STOCK";
    else if (isLowStock()) status = "LOW STOCK";

    cout << left << setw(6) << id << setw(21) << name << setw(21) << category << right << setw(9) << formatMoney(price)
         << setw(7) << quantity << setw(6) << threshold << "  " << left << status << "\n";
}
