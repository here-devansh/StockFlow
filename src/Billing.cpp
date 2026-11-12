#include "Billing.h"

#include <climits>
#include <iomanip>
#include <iostream>

#include "Reports.h"
#include "Utils.h"

using namespace std;

Billing::Billing(Inventory* inventory, FileManager* files) {
    this->inventory = inventory;
    this->files = files;
    customers = new Customer[MAX_CUSTOMERS];
    customerCount = 0;
    bills = new Bill[MAX_BILLS];
    billCount = 0;
}

bool Billing::loadData() {
    if (!files->loadCustomers(customers, customerCount, MAX_CUSTOMERS)) return false;
    if (!files->loadBills(bills, billCount, MAX_BILLS)) return false;
    return true;
}

bool Billing::saveAll() {
    if (!files->saveProducts(*inventory)) return false;
    if (!files->saveCustomers(customers, customerCount)) return false;
    if (!files->saveBills(bills, billCount)) return false;
    return true;
}

void Billing::cleanup() {
    queue.clear();
    undoStack.clear();
    delete[] customers;
    delete[] bills;
    customers = nullptr;
    bills = nullptr;
}

void Billing::clearUndoHistory() {
    if (!undoStack.empty()) {
        undoStack.clear();
        printWarning("Undo history cleared because the product list changed.");
    }
}

int Billing::findCustomerIndex(const string& name, const string& phone) const {
    for (int i = 0; i < customerCount; i++) {
        if (phone != "") {
            if (customers[i].getPhone() == phone) return i;
        } else {
            if (customers[i].getPhone() == "" && toLower(customers[i].getName()) == toLower(name)) return i;
        }
    }
    return -1;
}