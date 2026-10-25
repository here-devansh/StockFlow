#include "Inventory.h"

#include <iostream>

#include "Utils.h"

using namespace std;

Inventory::Inventory() {
    head = nullptr;
    tail = nullptr;
    count = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = nullptr;
    }
}

void Inventory::clear() {

    ProductNode* current = head;
    while (current != nullptr) {
        ProductNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* entry = table[i];
        while (entry != nullptr) {
            HashNode* nextEntry = entry->next;
            delete entry;
            entry = nextEntry;
        }
        table[i] = nullptr;
    }
    head = nullptr;
    tail = nullptr;
    count = 0;
}

int Inventory::hashFunction(int id) const {
    return id % TABLE_SIZE;
}

void Inventory::hashInsert(int id, ProductNode* node) {
    int bucket = hashFunction(id);
    HashNode* entry = new HashNode;
    entry->id = id;
    entry->node = node;
    entry->next = table[bucket];
    table[bucket] = entry;
}

ProductNode* Inventory::findNode(int id) const {
    HashNode* entry = table[hashFunction(id)];
    while (entry != nullptr) {
        if (entry->id == id) return entry->node;
        entry = entry->next;
    }
    return nullptr;
}

bool Inventory::addProduct(const Product& p) {
    if (findNode(p.getId()) != nullptr) return false;

    ProductNode* node = new ProductNode(p);

    node->prev = tail;
    if (tail != nullptr) {
        tail->next = node;
    } else {
        head = node;
    }
    tail = node;

    count++;
    hashInsert(p.getId(), node);
    return true;
}

Product* Inventory::findProduct(int id) {
    ProductNode* node = findNode(id);
    if (node == nullptr) return nullptr;
    return &node->data;
}

bool Inventory::updateProduct(int id, const string& name, const string& category, double price, int threshold) {
    ProductNode* node = findNode(id);
    if (node == nullptr) return false;

    if (!Product::isValidText(name) || !Product::isValidText(category)) return false;
    if (!Product::isValidPrice(price) || !Product::isValidQuantity(threshold)) return false;

    node->data.setName(name);
    node->data.setCategory(category);
    node->data.setPrice(price);
    node->data.setThreshold(threshold);
    return true;
}

bool Inventory::restock(int id, int amount) {
    ProductNode* node = findNode(id);
    if (node == nullptr) return false;
    return node->data.addStock(amount);
}

int Inventory::getCount() const {
    return count;
}

int Inventory::nextFreeId() const {
    int highest = 0;
    for (ProductNode* p = head; p != nullptr; p = p->next) {
        if (p->data.getId() > highest) highest = p->data.getId();
    }
    return highest + 1;
}

ProductNode* Inventory::getHead() const {
    return head;
}

int Inventory::searchByName(const string& keyword) const {
    string key = toLower(keyword);
    int found = 0;
    for (ProductNode* p = head; p != nullptr; p = p->next) {
        bool inName = toLower(p->data.getName()).find(key) != string::npos;
        bool inCategory = toLower(p->data.getCategory()).find(key) != string::npos;
        if (inName || inCategory) {
            if (found == 0) Product::printHeader();
            p->data.display();
            found++;
        }
    }
    return found;
}

void Inventory::displayAll() const {
    if (count == 0) {
        cout << "Inventory is empty.\n";
        return;
    }
    Product::printHeader();
    for (ProductNode* p = head; p != nullptr; p = p->next) {
        p->data.display();
    }
    printLine('-', 80);
    cout << count << " product(s).\n";
}

int Inventory::displayLowStock() const {
    int found = 0;
    for (ProductNode* p = head; p != nullptr; p = p->next) {
        if (p->data.isLowStock()) {
            if (found == 0) Product::printHeader();
            p->data.display();
            found++;
        }
    }
    return found;
}

bool Inventory::isConsistent() const {
    int nodesInList = 0;
    for (ProductNode* p = head; p != nullptr; p = p->next) {
        if (findNode(p->data.getId()) != p) return false;
        nodesInList++;
    }
    int entriesInTable = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        for (HashNode* entry = table[i]; entry != nullptr; entry = entry->next) {
            entriesInTable++;
        }
    }
    return nodesInList == count && entriesInTable == count;
}
