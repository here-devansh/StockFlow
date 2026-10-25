#pragma once
#include <string>

#include "User.h"

class Manager : public User {
private:
    std::string username;
    std::string password;

public:
    Manager();

    bool loadLogin(const std::string& filePath);
    bool checkLogin(const std::string& enteredUsername, const std::string& enteredPassword) const;

    void displayRole() const;
};
