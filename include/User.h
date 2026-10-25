#pragma once
#include <string>

class User {
protected:
    std::string name;

public:
    User(const std::string& name);

    std::string getName() const;
    void displayRole() const;
};
