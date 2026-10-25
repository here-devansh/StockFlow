#pragma once
#include <string>

#include "Product.h"

struct ProductNode {
    Product data;
    ProductNode* prev;
    ProductNode* next;
    ProductNode(const Product& p) : data(p), prev(nullptr), next(nullptr) {}
};

struct HashNode {
    int id;
    ProductNode* node;
    HashNode* next;
};

class Inventory {
private:
    ProductNode* head;
    ProductNode* tail;
    int count;

    static const int TABLE_SIZE = 13;
    HashNode* table[TABLE_SIZE];

    int hashFunction(int id) const;
    ProductNode* findNode(int id) const;
    void hashInsert(int id, ProductNode* node);

public:
    Inventory();
    void clear();

    bool addProduct(const Product& p);
    Product* findProduct(int id);
    bool updateProduct(int id, const std::string& name, const std::string& category, double price, int threshold);
    bool restock(int id, int amount);

    int getCount() const;
    int nextFreeId() const;
    ProductNode* getHead() const;

    int searchByName(const std::string& keyword) const;
    void displayAll() const;
    int displayLowStock() const;

    bool isConsistent() const;
};
