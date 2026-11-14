#include "Customer.h"

using namespace std;

int Customer::nextCustomerId = 1;

bool Customer::isValidName(const string& name) {
    return !name.empty() && name.size() <= 20 && name.find(',') == string::npos;
}

bool Customer::isValidPhone(const string& phone) {
    if (phone.empty()) return true;
    int length = phone.size();
    if (length < 7 || length > 15) return false;
    for (int i = 0; i < length; i++) {
        if (phone[i] < '0' || phone[i] > '9') return false;
    }
    return true;
}

int Customer::getNextCustomerId() {
    return nextCustomerId;
}

void Customer::setNextCustomerId(int value) {
    nextCustomerId = value;
}

Customer::Customer() {
    id = 0;
}

Customer::Customer(int id, const string& name, const string& phone) {
    this->id = id;
    this->name = name;
    this->phone = phone;
}

int Customer::getId() const { return id; }
string Customer::getName() const { return name; }
string Customer::getPhone() const { return phone; }
