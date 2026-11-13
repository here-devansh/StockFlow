#include "Bill.h"

#include <iomanip>
#include <iostream>

#include "Utils.h"

using namespace std;

int Bill::nextBillId = 1001;

int Bill::getNextBillId() {
    return nextBillId;
}

void Bill::setNextBillId(int value) {
    nextBillId = value;
}

Bill::Bill() {
    billId = 0;
    customerId = 0;
    itemCount = 0;
}

Bill::Bill(int billId, int customerId, const string& customerName, const string& dateTime) {
    this->billId = billId;
    this->customerId = customerId;
    this->customerName = customerName;
    this->dateTime = dateTime;
    itemCount = 0;
}

bool Bill::addItem(int productId, const string& productName, int quantity, double unitPrice) {
    if (productId <= 0 || quantity <= 0 || unitPrice <= 0) return false;

    for (int i = 0; i < itemCount; i++) {
        if (items[i].productId == productId) {
            if (items[i].unitPrice != unitPrice) return false;
            items[i].quantity = items[i].quantity + quantity;
            return true;
        }
    }

    if (itemCount == MAX_ITEMS) return false;

    items[itemCount].productId = productId;
    items[itemCount].productName = productName;
    items[itemCount].quantity = quantity;
    items[itemCount].unitPrice = unitPrice;
    itemCount++;
    return true;
}

int Bill::getBillId() const { return billId; }
int Bill::getCustomerId() const { return customerId; }
string Bill::getCustomerName() const { return customerName; }
string Bill::getDateTime() const { return dateTime; }
int Bill::getItemCount() const { return itemCount; }

const BillItem& Bill::getItem(int index) const {
    return items[index];
}

int Bill::getTotalUnits() const {
    int units = 0;
    for (int i = 0; i < itemCount; i++) {
        units = units + items[i].quantity;
    }
    return units;
}

double Bill::getTotalAmount() const {
    double total = 0;
    for (int i = 0; i < itemCount; i++) {
        total = total + items[i].unitPrice * items[i].quantity;
    }
    return total;
}

void Bill::displayReceipt() const {
    printLine('=', 62);
    cout << "                 STOCKFLOW - CUSTOMER RECEIPT\n";
    printLine('=', 62);
    cout << " Bill ID     : BILL-" << billId << "\n";
    cout << " Date/Time   : " << dateTime << "\n";
    cout << " Customer ID : C" << customerId << "\n";
    cout << " Customer    : " << customerName << "\n";
    printLine('-', 62);

    cout << left << setw(4) << " #" << setw(26) << "Item" << right << setw(6) << "Qty" << setw(12) << "Unit Price"
         << setw(14) << "Line Total" << "\n";
    printLine('-', 62);

    for (int i = 0; i < itemCount; i++) {
        double lineTotal = items[i].unitPrice * items[i].quantity;
        cout << " " << left << setw(3) << (i + 1) << setw(26) << items[i].productName << right << setw(6)
             << items[i].quantity << setw(12) << formatMoney(items[i].unitPrice) << setw(14) << formatMoney(lineTotal)
             << "\n";
    }

    printLine('-', 62);
    cout << left << setw(36) << " Total units" << right << setw(26) << getTotalUnits() << "\n";
    cout << left << setw(36) << " GRAND TOTAL (Rs.)" << right << setw(26) << formatMoney(getTotalAmount()) << "\n";
    printLine('=', 62);
    cout << "               Thank you for shopping with us!\n";
    printLine('=', 62);
    cout << left;
}
