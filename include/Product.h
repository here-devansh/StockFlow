#pragma once
#include <string>

class Product {
private:
    int id;
    std::string name;
    std::string category;
    double price;
    int quantity;
    int threshold;

public:
    static const int MAX_QUANTITY = 1000000;
    static const int MAX_PRICE = 1000000;

    static bool isValidText(const std::string& text);
    static bool isValidPrice(double price);
    static bool isValidQuantity(int quantity);

    Product(int id, const std::string& name, const std::string& category, double price, int quantity, int threshold);

    int getId() const;
    std::string getName() const;
    std::string getCategory() const;
    double getPrice() const;
    int getQuantity() const;
    int getThreshold() const;
    bool isLowStock() const;

    bool setName(const std::string& newName);
    bool setCategory(const std::string& newCategory);
    bool setPrice(double newPrice);
    bool setThreshold(int newThreshold);
    bool addStock(int amount);
    bool removeStock(int amount);

    static void printHeader();
    void display() const;
};
