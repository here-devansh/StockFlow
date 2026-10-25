#include "User.h"

#include <iostream>

using namespace std;

User::User(const string& name) {
    this->name = name;
}

string User::getName() const {
    return name;
}

void User::displayRole() const {
    cout << "Role: " << name << " (Guest) - can only view and search the inventory.\n";
}
