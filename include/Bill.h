#pragma once
#include <string>

struct BillItem {
    int productId;
    std::string productName;
    int quantity;
    double unitPrice;
};

class Bill {
public:
    static const int MAX_ITEMS = 15;

private:
    int billId;
    int customerId;
    std::string customerName;
    std::string dateTime;
    BillItem items[MAX_ITEMS];
    int itemCount;

    static int nextBillId;

public:
    static int getNextBillId();
    static void setNextBillId(int value);

    Bill();
    Bill(int billId, int customerId, const std::string& customerName, const std::string& dateTime);

    bool addItem(int productId, const std::string& productName, int quantity, double unitPrice);

    int getBillId() const;
    int getCustomerId() const;
    std::string getCustomerName() const;
    std::string getDateTime() const;
    int getItemCount() const;
    const BillItem& getItem(int index) const;
    int getTotalUnits() const;
    double getTotalAmount() const;

    void displayReceipt() const;
};
