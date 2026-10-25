#include <cstdio>
#include <fstream>
#include <iostream>

#include "FileManager.h"
#include "Inventory.h"
#include "Manager.h"
#include "User.h"
#include "Utils.h"

using namespace std;

int passed = 0;
int failed = 0;

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (condition) passed++;                                                \
        else { failed++; cout << "FAILED line " << __LINE__ << ": " #condition "\n"; } \
    } while (0)

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
}

void testProduct() {
    CHECK(Product::isValidText("Rice") && !Product::isValidText("") && !Product::isValidText("a,b"));
    CHECK(!Product::isValidText("this name is much too long for the table"));
    CHECK(Product::isValidPrice(0.5) && !Product::isValidPrice(0) && !Product::isValidPrice(-3));
    CHECK(Product::isValidQuantity(0) && !Product::isValidQuantity(-1) && !Product::isValidQuantity(Product::MAX_QUANTITY + 1));

    Product p(1, "Biscuit", "Snacks", 10, 3, 2);
    CHECK(!p.removeStock(4) && p.getQuantity() == 3);
    CHECK(!p.removeStock(0) && !p.removeStock(-1));
    CHECK(p.removeStock(3) && p.getQuantity() == 0);
    CHECK(p.isLowStock());
    CHECK(!p.addStock(0) && !p.addStock(Product::MAX_QUANTITY + 1) && p.addStock(10) && p.getQuantity() == 10);
    CHECK(!p.isLowStock());
    CHECK(!p.setPrice(-1) && p.getPrice() == 10 && p.setPrice(12.5) && p.getPrice() == 12.5);
    CHECK(!p.setName("") && p.getName() == "Biscuit" && p.setName("Cookie") && p.getName() == "Cookie");
}

void testInventory() {
    Inventory inv;
    CHECK(inv.addProduct(Product(1, "A", "c", 10, 5, 2)));
    CHECK(inv.addProduct(Product(14, "B", "c", 30, 5, 2)));
    CHECK(inv.addProduct(Product(27, "C", "c", 20, 9, 2)));
    CHECK(!inv.addProduct(Product(14, "Duplicate", "c", 10, 5, 2)));
    CHECK(inv.getCount() == 3 && inv.isConsistent());

    CHECK(inv.findProduct(14) != nullptr && inv.findProduct(14)->getName() == "B");
    CHECK(inv.findProduct(27) != nullptr && inv.findProduct(1) != nullptr);
    CHECK(inv.findProduct(99) == nullptr);
    CHECK(inv.nextFreeId() == 28);

    ProductNode* node = inv.getHead();
    CHECK(node->data.getId() == 1 && node->next->data.getId() == 14 && node->next->next->data.getId() == 27);
    CHECK(node->prev == nullptr && node->next->prev == node);

    CHECK(!inv.restock(1, 0) && !inv.restock(1, -3) && !inv.restock(99, 5));
    CHECK(inv.restock(1, 10) && inv.findProduct(1)->getQuantity() == 15);
    CHECK(!inv.updateProduct(1, "", "c", 5, 1) && inv.findProduct(1)->getName() == "A");
    CHECK(!inv.updateProduct(1, "X", "c", -5, 1) && inv.findProduct(1)->getName() == "A");
    CHECK(inv.updateProduct(1, "X", "cat", 5, 1) && inv.findProduct(1)->getName() == "X");
    CHECK(inv.displayLowStock() == 0);
    CHECK(inv.updateProduct(1, "X", "cat", 5, 20) && inv.displayLowStock() == 1);
    CHECK(inv.searchByName("cat") == 1 && inv.searchByName("zzz") == 0);

    inv.clear();
    CHECK(inv.getCount() == 0 && inv.getHead() == nullptr && inv.findProduct(1) == nullptr);
    CHECK(inv.addProduct(Product(5, "Again", "c", 10, 5, 2)) && inv.isConsistent());
    inv.clear();
}

void testFiles() {

    string folder = ".";
    FileManager files(folder);

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
    CHECK(files.loadProducts(c));
    CHECK(c.getCount() == 3 && c.isConsistent());
    CHECK(ifstream("products.csv.bak").good());

    a.clear();
    b.clear();
    c.clear();
    remove("products.csv");
    remove("products.csv.bak");
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
    testFiles();
    testManager();
    cout << "\n" << passed << " checks passed, " << failed << " failed\n";
    return failed;
}
