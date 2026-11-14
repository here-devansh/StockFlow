#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Bill.h"
#include "BillingQueue.h"
#include "Customer.h"
#include "FileManager.h"
#include "Inventory.h"
#include "Manager.h"
#include "Product.h"
#include "Reports.h"
#include "UndoStack.h"
#include "User.h"
#include "Utils.h"

using namespace std;

int passed = 0;
int failed = 0;

#define CHECK(condition)                                                              \
    do {                                                                              \
        if (condition) passed++;                                                      \
        else { failed++; cout << "FAILED line " << __LINE__ << ": " #condition "\n"; } \
    } while (0)

string capture(void (*action)(Inventory&), Inventory& inventory) {
    stringstream captured;
    streambuf* old = cout.rdbuf(captured.rdbuf());
    action(inventory);
    cout.rdbuf(old);
    return captured.str();
}

void sortByPriceAscending(Inventory& inv) { inv.displaySorted(true, true); }
void sortByPriceDescending(Inventory& inv) { inv.displaySorted(true, false); }
void sortByQuantityAscending(Inventory& inv) { inv.displaySorted(false, true); }
void sortByQuantityDescending(Inventory& inv) { inv.displaySorted(false, false); }

void testUtils() {
    int i;
    double d;
    CHECK(textToInt("42", i) && i == 42);
    CHECK(!textToInt("12abc", i) && !textToInt("abc", i) && !textToInt("", i) && !textToInt("99999999999", i));
    CHECK(textToDouble("25.50", d) && d == 25.5);
    CHECK(!textToDouble("2.5x", d) && !textToDouble("", d));
    CHECK(trimText("  hello  ") == "hello" && trimText("   ") == "" && trimText("") == "");
    CHECK(toLower("BiScUiT") == "biscuit");
    CHECK(formatMoney(12.5) == "12.50" && formatMoney(3) == "3.00");
    CHECK(isValidDateTime("2026-09-03 10:15:22") && !isValidDateTime("2026-13-03 10:15:22"));
    CHECK(!isValidDateTime("2026/09/03 10:15:22") && !isValidDateTime("hello") && !isValidDateTime(""));
    CHECK(isValidDateTime(currentDateTime()));
}

void testProduct() {
    CHECK(Product::isValidText("Rice") && !Product::isValidText("") && !Product::isValidText("a,b"));
    CHECK(!Product::isValidText("this name is much too long"));
    CHECK(Product::isValidPrice(0.5) && !Product::isValidPrice(0) && !Product::isValidPrice(-3));
    CHECK(Product::isValidQuantity(0) && !Product::isValidQuantity(-1) && !Product::isValidQuantity(Product::MAX_QUANTITY + 1));

    Product p(1, "Biscuit", "Snacks", 10, 3, 2);
    CHECK(!p.removeStock(4) && p.getQuantity() == 3);
    CHECK(!p.removeStock(0) && !p.removeStock(-1));
    CHECK(p.removeStock(3) && p.getQuantity() == 0 && p.isLowStock());
    CHECK(!p.addStock(0) && !p.addStock(Product::MAX_QUANTITY + 1) && p.addStock(10) && p.getQuantity() == 10);
    CHECK(!p.setPrice(-1) && p.getPrice() == 10 && p.setPrice(12.5) && p.getPrice() == 12.5);
    CHECK(!p.setName("") && p.getName() == "Biscuit" && p.setName("Cookie") && p.getName() == "Cookie");
}

void testInventory() {
    Inventory inv;
    CHECK(inv.addProduct(Product(1, "A", "c", 30, 5, 2)));
    CHECK(inv.addProduct(Product(14, "B", "c", 10, 5, 2)));
    CHECK(inv.addProduct(Product(27, "C", "c", 20, 9, 2)));
    CHECK(!inv.addProduct(Product(14, "Duplicate", "c", 10, 5, 2)));
    CHECK(inv.getCount() == 3 && inv.isConsistent());
    CHECK(inv.findProduct(14) != nullptr && inv.findProduct(14)->getName() == "B");
    CHECK(inv.findProduct(99) == nullptr && inv.nextFreeId() == 28);

    CHECK(inv.linearSearchById(27) == inv.findProduct(27));
    CHECK(inv.binarySearchById(27) == inv.findProduct(27) && inv.binarySearchById(1) == inv.findProduct(1));
    CHECK(inv.linearSearchById(99) == nullptr && inv.binarySearchById(99) == nullptr);

    CHECK(!inv.reduceStock(1, 6) && inv.findProduct(1)->getQuantity() == 5);
    CHECK(!inv.reduceStock(1, 0) && !inv.reduceStock(99, 1));
    CHECK(inv.reduceStock(1, 5) && inv.findProduct(1)->getQuantity() == 0);
    CHECK(!inv.restock(1, 0) && !inv.restock(1, -3) && !inv.restock(99, 5));
    CHECK(inv.restock(1, 10) && inv.findProduct(1)->getQuantity() == 10);
    CHECK(!inv.updateProduct(1, "", "c", 5, 1) && inv.findProduct(1)->getName() == "A");
    CHECK(!inv.updateProduct(1, "X", "c", -5, 1) && inv.findProduct(1)->getName() == "A");
    CHECK(inv.updateProduct(1, "X", "cat", 30, 1) && inv.findProduct(1)->getName() == "X");
    CHECK(inv.searchByName("cat") == 1 && inv.searchByName("zzz") == 0);

    string byPriceUp = capture(sortByPriceAscending, inv);
    CHECK(byPriceUp.find("B ") < byPriceUp.find("C ") && byPriceUp.find("C ") < byPriceUp.find("X "));
    string byPriceDown = capture(sortByPriceDescending, inv);
    CHECK(byPriceDown.find("X ") < byPriceDown.find("C ") && byPriceDown.find("C ") < byPriceDown.find("B "));
    string byQtyUp = capture(sortByQuantityAscending, inv);
    CHECK(byQtyUp.find("B ") < byQtyUp.find("C ") && byQtyUp.find("C ") < byQtyUp.find("X "));
    string byQtyDown = capture(sortByQuantityDescending, inv);
    CHECK(byQtyDown.find("X ") < byQtyDown.find("C ") && byQtyDown.find("C ") < byQtyDown.find("B "));
    CHECK(inv.getHead()->data.getId() == 1 && inv.isConsistent());

    CHECK(inv.deleteProduct(14) && inv.findProduct(14) == nullptr && inv.isConsistent() && inv.getCount() == 2);
    CHECK(inv.findProduct(27) != nullptr && inv.findProduct(1) != nullptr);
    CHECK(inv.deleteProduct(1) && inv.isConsistent() && inv.getHead()->data.getId() == 27);
    CHECK(inv.deleteProduct(27) && inv.isConsistent() && inv.getCount() == 0 && inv.getHead() == nullptr);
    CHECK(!inv.deleteProduct(27));
    CHECK(inv.addProduct(Product(5, "Again", "c", 10, 5, 2)) && inv.isConsistent());
    inv.clear();
    CHECK(inv.getCount() == 0 && inv.findProduct(5) == nullptr);
}

void testQueueAndStack() {
    BillingQueue q;
    BillRequest r;
    CHECK(q.empty() && !q.dequeue(r) && !q.front(r));
    BillRequest a = {1, 10};
    BillRequest b = {2, 20};
    BillRequest c = {1, 5};
    q.enqueue(a);
    q.enqueue(b);
    q.enqueue(c);
    CHECK(q.size() == 3 && q.front(r) && r.productId == 1 && r.quantity == 10);
    CHECK(q.getRequestedQuantity(1) == 15 && q.getRequestedQuantity(9) == 0);
    CHECK(q.getItem(1, r) && r.productId == 2 && !q.getItem(3, r) && !q.getItem(-1, r));
    CHECK(q.dequeue(r) && r.productId == 1 && r.quantity == 10);
    CHECK(q.dequeue(r) && r.productId == 2);
    CHECK(q.dequeue(r) && r.productId == 1 && r.quantity == 5 && q.empty());
    q.enqueue(a);
    q.clear();
    CHECK(q.empty() && q.size() == 0);

    UndoStack st;
    CHECK(st.empty() && st.top() == nullptr && !st.pop());
    Bill one(1, 1, "A", "2026-01-01 00:00:00");
    one.addItem(1, "X", 2, 5.0);
    Bill two(2, 1, "A", "2026-01-01 00:00:00");
    st.push(one);
    st.push(two);
    CHECK(st.top()->getBillId() == 2 && st.size() == 2);
    CHECK(st.pop() && st.top()->getBillId() == 1 && st.top()->getItemCount() == 1);
    CHECK(st.pop() && st.empty());
}

void testBillAndCustomer() {
    Bill b(1, 1, "A", "2026-09-05 10:00:00");
    CHECK(b.addItem(1, "Biscuit", 5, 10.0));
    CHECK(b.addItem(1, "Biscuit", 3, 10.0));
    CHECK(b.addItem(2, "Rice", 9, 20.0));
    CHECK(!b.addItem(3, "Bad", 0, 5.0) && !b.addItem(3, "Bad", 1, -5.0) && !b.addItem(1, "Biscuit", 1, 11.0));
    CHECK(b.getItemCount() == 2 && b.getItem(0).quantity == 8 && b.getTotalAmount() == 260.0 && b.getTotalUnits() == 17);
    for (int i = 0; i < Bill::MAX_ITEMS - 2; i++) {
        CHECK(b.addItem(100 + i, "Item", 1, 1.0));
    }
    CHECK(b.getItemCount() == Bill::MAX_ITEMS && !b.addItem(999, "TooMany", 1, 1.0));
    CHECK(b.addItem(1, "Biscuit", 1, 10.0) && b.getItem(0).quantity == 9);

    Bill copy = b;
    copy.addItem(1, "Biscuit", 100, 10.0);
    CHECK(b.getItem(0).quantity == 9 && copy.getItem(0).quantity == 109);

    CHECK(Customer::isValidPhone("") && Customer::isValidPhone("9876543210"));
    CHECK(!Customer::isValidPhone("12345") && !Customer::isValidPhone("98765abcde"));
    CHECK(Customer::isValidName("Aarav") && !Customer::isValidName("") && !Customer::isValidName("A,B"));
    Customer c(3, "Meera", "9123456780");
    CHECK(c.getId() == 3 && c.getName() == "Meera" && c.getPhone() == "9123456780");
}

void testReports() {
    Bill* bills = new Bill[3];
    bills[0] = Bill(1, 1, "A", "2026-09-05 10:00:00");
    bills[0].addItem(1, "Biscuit", 8, 10.0);
    bills[0].addItem(2, "Rice", 2, 20.0);
    bills[1] = Bill(2, 1, "A", "2026-09-20 10:00:00");
    bills[1].addItem(1, "Biscuit", 4, 10.0);
    bills[2] = Bill(3, 1, "A", "2026-10-01 10:00:00");
    bills[2].addItem(3, "OctoberItem", 1, 7.0);

    Reports reports(bills, 3);
    stringstream captured;
    streambuf* old = cout.rdbuf(captured.rdbuf());
    reports.monthlyReport(9, 2026);
    reports.monthlyReport(1, 2020);
    cout.rdbuf(old);
    string text = captured.str();

    CHECK(text.find("Number of bills    : 2") != string::npos);
    CHECK(text.find("Units sold         : 14") != string::npos);
    CHECK(text.find("Total revenue      : Rs. 160.00") != string::npos);
    CHECK(text.find("Average bill value : Rs. 80.00") != string::npos);
    CHECK(text.find("Biscuit") < text.find("Rice"));
    CHECK(text.find("OctoberItem") == string::npos);
    CHECK(text.find("No sales were recorded in this month.") != string::npos);
    delete[] bills;
}

void testFiles() {
    FileManager files(".");
    CHECK(files.createMissingFiles());

    Inventory a;
    a.addProduct(Product(1, "Rice", "Grocery", 95.5, 40, 10));
    a.addProduct(Product(2, "Salt", "Grocery", 28, 0, 5));
    CHECK(files.saveProducts(a));
    Inventory b;
    CHECK(files.loadProducts(b) && b.getCount() == 2);
    CHECK(b.findProduct(1)->getPrice() == 95.5 && b.findProduct(2)->getQuantity() == 0);

    {
        ofstream out("products.csv", ios::app);
        out << "abc,bad,row\n3,Neg,X,-5,1,1\n1,DupId,X,5,1,1\n4,Good,X,5,1,1\n5,Text,X,notnumber,1,1\n6,TooFew,X\n7,A,B,1,1,1,extra\n";
    }
    Inventory c;
    CHECK(files.loadProducts(c) && c.getCount() == 3 && c.isConsistent());
    CHECK(ifstream("products.csv.bak").good());

    Customer* customers = new Customer[5];
    customers[0] = Customer(1, "Meera", "9123456780");
    customers[1] = Customer(2, "Aarav", "");
    CHECK(files.saveCustomers(customers, 2));
    Customer* loadedCustomers = new Customer[5];
    int customerCount = 0;
    CHECK(files.loadCustomers(loadedCustomers, customerCount, 5) && customerCount == 2);
    CHECK(loadedCustomers[1].getName() == "Aarav" && loadedCustomers[1].getPhone() == "" && Customer::getNextCustomerId() == 3);

    Bill* bills = new Bill[5];
    bills[0] = Bill(1001, 1, "Meera", "2026-09-03 10:15:22");
    bills[0].addItem(1, "Rice", 2, 95.5);
    bills[0].addItem(2, "Salt", 1, 28);
    bills[1] = Bill(1002, 2, "Aarav", "2026-09-04 11:00:00");
    bills[1].addItem(1, "Rice", 1, 95.5);
    CHECK(files.saveBills(bills, 2));
    Bill* loadedBills = new Bill[5];
    int billCount = 0;
    CHECK(files.loadBills(loadedBills, billCount, 5) && billCount == 2);
    CHECK(loadedBills[0].getTotalAmount() == 219.0 && loadedBills[0].getItemCount() == 2 && loadedBills[1].getCustomerName() == "Aarav");
    CHECK(Bill::getNextBillId() == 1003);
    CHECK(files.loadBills(loadedBills, billCount, 1) && billCount == 1);

    delete[] customers;
    delete[] loadedCustomers;
    delete[] bills;
    delete[] loadedBills;
    a.clear();
    b.clear();
    c.clear();
    remove("products.csv");
    remove("customers.csv");
    remove("bills.csv");
    remove("manager.txt");
    remove("products.csv.bak");

    Inventory d;
    ofstream("products.csv") << "wrong header\n";
    CHECK(files.createMissingFiles() && files.loadProducts(d) && d.getCount() == 0 && ifstream("products.csv.bak").good());
    remove("products.csv");
    remove("products.csv.bak");
    remove("customers.csv");
    remove("bills.csv");
    remove("manager.txt");
}

void testManager() {
    User guest("Guest");
    Manager manager;
    CHECK(guest.getName() == "Guest" && manager.getName() == "Store Manager");
    CHECK(!manager.checkLogin("admin", "admin123"));
    {
        ofstream out("test_manager.txt");
        out << "admin admin123\n";
    }
    CHECK(manager.loadLogin("test_manager.txt"));
    CHECK(manager.checkLogin("admin", "admin123"));
    CHECK(!manager.checkLogin("admin", "wrong") && !manager.checkLogin("other", "admin123"));
    remove("test_manager.txt");
    CHECK(!manager.loadLogin("no_such_file.txt"));
}

int main() {
    testUtils();
    testProduct();
    testInventory();
    testQueueAndStack();
    testBillAndCustomer();
    testReports();
    testFiles();
    testManager();
    cout << "\n" << passed << " checks passed, " << failed << " failed\n";
    return failed;
}
