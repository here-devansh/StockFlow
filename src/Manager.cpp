#include "Manager.h"

#include <fstream>
#include <iostream>

using namespace std;

Manager::Manager() : User("Store Manager") {
}

bool Manager::loadLogin(const string& filePath) {
    ifstream file(filePath);
    if (!file) return false;
    file >> username >> password;
    return !username.empty() && !password.empty();
}

bool Manager::checkLogin(const string& enteredUsername, const string& enteredPassword) const {
    return enteredUsername == username && enteredPassword == password;
}

void Manager::displayRole() const {
    cout << "Role: " << name << " (Manager) - can manage products (billing and reports: phase 2).\n";
}
