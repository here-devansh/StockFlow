#pragma once
#include <string>

#include "Bill.h"
#include "BillingQueue.h"
#include "Customer.h"
#include "FileManager.h"
#include "Inventory.h"
#include "UndoStack.h"

class Billing {
public:
    static const int MAX_CUSTOMERS = 1000;
    static const int MAX_BILLS = 2000;

private:
    Inventory* inventory;
    FileManager* files;

    Customer* customers;
    int customerCount;
    Bill* bills;
    int billCount;

    BillingQueue queue;
    UndoStack undoStack;

    int findCustomerIndex(const std::string& name, const std::string& phone) const;
    std::string askPhone() const;
    bool collectItems();
    void createSale(const std::string& name, const std::string& phone);
    void showLowStockWarning(const Product& p) const;
    void removeBillAt(int index);
    void insertBillAt(int index, const Bill& bill);

public:
    Billing(Inventory* inventory, FileManager* files);

    bool loadData();
    bool saveAll();

    void generateBill();
    void undoLastSale();
    void viewSalesHistory();
    void monthlyReport();

    void clearUndoHistory();
    void cleanup();
};
