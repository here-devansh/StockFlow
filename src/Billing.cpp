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


string Billing::askPhone() const {
    while (true) {
        string phone = readLine("Phone (optional, press Enter to skip): ");
        if (Customer::isValidPhone(phone)) return phone;
        printError("The phone number must have 7 to 15 digits.");
    }
}

void Billing::showLowStockWarning(const Product& p) const {
    if (p.getQuantity() == 0) {
        printWarning("OUT OF STOCK: " + p.getName());
    } else if (p.isLowStock()) {
        printWarning("LOW STOCK: " + p.getName() + " has only " + to_string(p.getQuantity()) + " left (alert level " +
                     to_string(p.getThreshold()) + ")");
    }
}

void Billing::removeBillAt(int index) {
    for (int i = index; i < billCount - 1; i++) {
        bills[i] = bills[i + 1];
    }
    billCount--;
}

void Billing::insertBillAt(int index, const Bill& bill) {
    for (int i = billCount; i > index; i--) {
        bills[i] = bills[i - 1];
    }
    bills[index] = bill;
    billCount++;
}

bool Billing::collectItems() {
    queue.clear();
    cout << "Add items. Enter product ID 0 to finish, or -1 to list all products.\n";

    while (true) {
        int id = readInt("Product ID: ", -1, INT_MAX);
        if (id == 0) break;
        if (id == -1) {
            inventory->displayAll();
            continue;
        }

        if (queue.size() >= Bill::MAX_ITEMS) {
            printError("A bill can have at most " + to_string(Bill::MAX_ITEMS) + " lines. Enter 0 to finish.");
            continue;
        }

        Product* p = inventory->findProduct(id);
        if (p == nullptr) {
            printError("No product with ID " + to_string(id) + ".");
            continue;
        }

        cout << "  " << p->getName() << "  Rs. " << formatMoney(p->getPrice()) << "  (in stock: " << p->getQuantity()
             << ")\n";
        int quantity = readInt("  Quantity: ", INT_MIN, INT_MAX);
        if (quantity <= 0) {
            printError("The quantity must be at least 1.");
            continue;
        }

        int alreadyQueued = queue.getRequestedQuantity(id);
        if (quantity > p->getQuantity() - alreadyQueued) {
            printError("Not enough stock: " + to_string(p->getQuantity()) + " available, " + to_string(alreadyQueued) +
                       " already on this bill.");
            continue;
        }

        BillRequest request;
        request.productId = id;
        request.quantity = quantity;
        queue.enqueue(request);
        printOk("Added " + to_string(quantity) + " x " + p->getName());
    }

    if (queue.empty()) {
        cout << "No items entered. Bill cancelled.\n";
        return false;
    }

    cout << "\nItems on this bill:\n";
    for (int i = 0; i < queue.size(); i++) {
        BillRequest request;
        queue.getItem(i, request);
        Product* p = inventory->findProduct(request.productId);
        cout << "  " << request.quantity << " x " << p->getName() << "\n";
    }
    return true;
}

void Billing::createSale(const string& name, const string& phone) {
    int customerIndex = findCustomerIndex(name, phone);
    bool newCustomer = (customerIndex == -1);

    if (newCustomer && customerCount >= MAX_CUSTOMERS) {
        queue.clear();
        printError("The customer list is full. Bill rejected.");
        return;
    }

    int customerId;
    string customerName;
    string customerPhone;
    if (newCustomer) {
        customerId = Customer::getNextCustomerId();
        customerName = name;
        customerPhone = phone;
    } else {
        customerId = customers[customerIndex].getId();
        customerName = customers[customerIndex].getName();
        customerPhone = customers[customerIndex].getPhone();
    }

    Bill bill(Bill::getNextBillId(), customerId, customerName, currentDateTime());
    bool valid = true;

    BillRequest request;
    while (queue.dequeue(request)) {
        Product* p = inventory->findProduct(request.productId);
        if (p == nullptr) {
            printError("Product " + to_string(request.productId) + " no longer exists.");
            valid = false;
        } else if (!bill.addItem(p->getId(), p->getName(), request.quantity, p->getPrice())) {
            printError("Could not add " + p->getName() + " to the bill.");
            valid = false;
        }
    }

    for (int i = 0; i < bill.getItemCount() && valid; i++) {
        const BillItem& item = bill.getItem(i);
        Product* p = inventory->findProduct(item.productId);
        if (p == nullptr || item.quantity > p->getQuantity()) {
            printError("Not enough stock for " + item.productName + ".");
            valid = false;
        }
    }

    if (!valid) {
        printError("Bill rejected. Stock was NOT changed.");
        return;
    }

    int reduced = 0;
    bool stockOk = true;
    for (int i = 0; i < bill.getItemCount(); i++) {
        const BillItem& item = bill.getItem(i);
        if (!inventory->reduceStock(item.productId, item.quantity)) {
            stockOk = false;
            break;
        }
        reduced++;
    }

    if (stockOk) {
        bills[billCount] = bill;
        billCount++;
        if (newCustomer) {
            customers[customerCount] = Customer(customerId, customerName, customerPhone);
            customerCount++;
        }
    }

    if (!stockOk || !saveAll()) {
        string reason = "internal stock error";
        if (stockOk) {
            reason = files->getErrorMessage();
            billCount--;
            if (newCustomer) customerCount--;
        }
        for (int i = 0; i < reduced; i++) {
            inventory->restock(bill.getItem(i).productId, bill.getItem(i).quantity);
        }
        if (!saveAll()) {
            printWarning("The data files may be out of date: " + files->getErrorMessage());
        }
        printError("SALE FAILED: " + reason + ". Stock was restored and no sale was recorded.");
        return;
    }

    Bill::setNextBillId(bill.getBillId() + 1);
    if (newCustomer) Customer::setNextCustomerId(customerId + 1);
    undoStack.push(bill);

    cout << "\n";
    bill.displayReceipt();
    if (newCustomer) {
        printOk("New customer saved with ID C" + to_string(customerId) + ".");
    } else {
        printOk("Existing customer C" + to_string(customerId) + " used.");
    }
    printOk("Sale completed and saved.");

    for (int i = 0; i < bill.getItemCount(); i++) {
        Product* p = inventory->findProduct(bill.getItem(i).productId);
        if (p != nullptr) showLowStockWarning(*p);
    }
}

void Billing::generateBill() {
    printHeading("GENERATE CUSTOMER BILL");

    if (billCount >= MAX_BILLS) {
        printError("The bill list is full.");
        return;
    }

    string name = readText("Customer name (max 20 characters): ");
    string phone = askPhone();

    if (!collectItems()) return;

    if (!askYesNo("Generate the bill?")) {
        queue.clear();
        cout << "Bill cancelled. Stock was not changed.\n";
        return;
    }

    createSale(name, phone);
}

void Billing::undoLastSale() {
    printHeading("UNDO LAST SALE");

    const Bill* top = undoStack.top();
    if (top == nullptr) {
        cout << "Nothing to undo. Only sales made in this session can be undone.\n";
        return;
    }

    Bill sale = *top;
    cout << "Last sale: BILL-" << sale.getBillId() << " for " << sale.getCustomerName() << ", Rs. "
         << formatMoney(sale.getTotalAmount()) << "\n";
    for (int i = 0; i < sale.getItemCount(); i++) {
        cout << "  " << sale.getItem(i).quantity << " x " << sale.getItem(i).productName << "\n";
    }

    if (!askYesNo("Undo this sale?")) {
        cout << "Undo cancelled.\n";
        return;
    }

    int index = -1;
    for (int i = 0; i < billCount; i++) {
        if (bills[i].getBillId() == sale.getBillId()) index = i;
    }
    if (index == -1) {
        undoStack.pop();
        printError("The bill was not found. The undo entry was removed.");
        return;
    }

    for (int i = 0; i < sale.getItemCount(); i++) {
        Product* p = inventory->findProduct(sale.getItem(i).productId);
        if (p == nullptr || sale.getItem(i).quantity > Product::MAX_QUANTITY - p->getQuantity()) {
            printError("Cannot restore the stock of " + sale.getItem(i).productName + ". Undo stopped.");
            return;
        }
    }

    for (int i = 0; i < sale.getItemCount(); i++) {
        inventory->restock(sale.getItem(i).productId, sale.getItem(i).quantity);
    }
    Bill removed = bills[index];
    removeBillAt(index);

    if (!saveAll()) {
        insertBillAt(index, removed);
        for (int i = 0; i < sale.getItemCount(); i++) {
            inventory->reduceStock(sale.getItem(i).productId, sale.getItem(i).quantity);
        }
        printError("Undo failed: " + files->getErrorMessage() + ". The sale was left as it was.");
        return;
    }

    undoStack.pop();
    printOk("BILL-" + to_string(sale.getBillId()) + " was undone. Stock restored:");
    for (int i = 0; i < sale.getItemCount(); i++) {
        Product* p = inventory->findProduct(sale.getItem(i).productId);
        cout << "  " << sale.getItem(i).productName << " is now " << p->getQuantity() << "\n";
    }
}

void Billing::viewSalesHistory() {
    printHeading("SALES HISTORY");

    if (billCount == 0) {
        cout << "No bills recorded yet.\n";
        return;
    }

    cout << left << setw(11) << "Bill" << setw(21) << "Date/Time" << setw(22) << "Customer" << right << setw(7)
         << "Units" << setw(14) << "Total (Rs.)" << "\n";
    printLine('-', 75);

    double sum = 0;
    for (int i = 0; i < billCount; i++) {
        cout << left << setw(11) << ("BILL-" + to_string(bills[i].getBillId())) << setw(21) << bills[i].getDateTime()
             << setw(22) << bills[i].getCustomerName() << right << setw(7) << bills[i].getTotalUnits() << setw(14)
             << formatMoney(bills[i].getTotalAmount()) << "\n";
        sum = sum + bills[i].getTotalAmount();
    }
    printLine('-', 75);
    cout << left << billCount << " bill(s), total Rs. " << formatMoney(sum) << "\n";

    int id = readInt("Enter a bill number to see its receipt (0 = back): ", 0, INT_MAX);
    if (id == 0) return;

    for (int i = 0; i < billCount; i++) {
        if (bills[i].getBillId() == id) {
            bills[i].displayReceipt();
            return;
        }
    }
    printError("No bill with ID " + to_string(id) + ".");
}

void Billing::monthlyReport() {
    printHeading("MONTHLY REPORT");
    int month = readInt("Month (1-12): ", 1, 12);
    int year = readInt("Year (example 2026): ", 1970, 9999);

    Reports reports(bills, billCount);
    cout << "\n";
    reports.monthlyReport(month, year);
}
