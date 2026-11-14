#pragma once
#include <string>

class Customer {
private:
    int id;
    std::string name;
    std::string phone;

    static int nextCustomerId;

public:
    static bool isValidName(const std::string& name);
    static bool isValidPhone(const std::string& phone);
    static int getNextCustomerId();
    static void setNextCustomerId(int value);

    Customer();
    Customer(int id, const std::string& name, const std::string& phone);

    int getId() const;
    std::string getName() const;
    std::string getPhone() const;
};
