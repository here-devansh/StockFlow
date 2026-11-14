#include <climits>
#include <iostream>
#include <string>

#include "Billing.h"
#include "FileManager.h"
#include "Inventory.h"
#include "Manager.h"
#include "User.h"
#include "Utils.h"

using namespace std;

void saveInventory(FileManager& files, Inventory& inventory) {
    if (files.saveProducts(inventory)) {
        printOk("Saved to file.");
    } else {
        printError("Could NOT save to file: " + files.getErrorMessage());
    }
}

void viewAllProducts(Inventory& inventory) {
    printHeading("ALL PRODUCTS");
    inventory.displayAll();
    if (inventory.isConsistent()) {
        cout << "List and hash table check: OK\n";
    } else {
        printError("List and hash table do not match!");
    }
}

void searchProduct(Inventory& inventory) {
    printHeading("SEARCH PRODUCT");
    cout << "  1. By ID (hash table)\n";
    cout << "  2. By ID (linear search)\n";
    cout << "  3. By ID (binary search)\n";
    cout << "  4. By name or category\n";
    int choice = readInt("Choice (1-4): ", 1, 4);

    if (choice == 4) {
        string keyword = readLine("Type part of the name or category: ");
        if (keyword.empty()) {
            printError("You did not type anything.");
        } else if (inventory.searchByName(keyword) == 0) {
            cout << "No product matches '" << keyword << "'.\n";
        }
        return;
    }

    int id = readInt("Product ID: ", 1, 2000000000);
    Product* p;
    if (choice == 1) {
        p = inventory.findProduct(id);
    } else if (choice == 2) {
        p = inventory.linearSearchById(id);
    } else {
        p = inventory.binarySearchById(id);
    }

    if (p == nullptr) {
        printError("No product with ID " + to_string(id) + ".");
    } else {
        Product::printHeader();
        p->display();
    }
}

void addProduct(Inventory& inventory, FileManager& files) {
    printHeading("ADD PRODUCT");

    int id = readInt("Product ID (0 = choose automatically): ", 0, 2000000000);
    if (id == 0) {
        id = inventory.nextFreeId();
    }
    if (inventory.findProduct(id) != nullptr) {
        printError("Product ID " + to_string(id) + " already exists. Nothing was added.");
        return;
    }

    string name = readText("Name (max 20 characters): ");
    string category = readText("Category (max 20 characters): ");
    double price = readDouble("Price in Rs.: ", 0.01, Product::MAX_PRICE);
    int quantity = readInt("Quantity in stock: ", 0, Product::MAX_QUANTITY);
    int threshold = readInt("Low-stock alert level: ", 0, Product::MAX_QUANTITY);

    Product p(id, name, category, price, quantity, threshold);
    inventory.addProduct(p);
    printOk("Product '" + name + "' added with ID " + to_string(id) + ".");
    saveInventory(files, inventory);
}

string askTextOrKeep(const string& label, const string& oldValue) {
    while (true) {
        string answer = readLine("  " + label + " [" + oldValue + "]: ");
        if (answer.empty()) return oldValue;
        if (Product::isValidText(answer)) return answer;
        printError("Enter 1 to 20 characters. A comma (,) is not allowed.");
    }
}

double askPriceOrKeep(double oldValue) {
    while (true) {
        string answer = readLine("  Price [" + formatMoney(oldValue) + "]: ");
        double newValue;
        if (answer.empty()) return oldValue;
        if (textToDouble(answer, newValue) && Product::isValidPrice(newValue)) return newValue;
        printError("The price must be a number more than 0.");
    }
}

int askThresholdOrKeep(int oldValue) {
    while (true) {
        string answer = readLine("  Alert level [" + to_string(oldValue) + "]: ");
        int newValue;
        if (answer.empty()) return oldValue;
        if (textToInt(answer, newValue) && Product::isValidQuantity(newValue)) return newValue;
        printError("The alert level must be a whole number, 0 or more.");
    }
}

void updateProduct(Inventory& inventory, FileManager& files) {
    printHeading("UPDATE PRODUCT");

    int id = readInt("Product ID to update (0 = cancel): ", 0, 2000000000);
    if (id == 0) return;
    Product* p = inventory.findProduct(id);
    if (p == nullptr) {
        printError("No product with ID " + to_string(id) + ".");
        return;
    }

    Product::printHeader();
    p->display();
    cout << "Press Enter to keep the old value.\n";

    string name = askTextOrKeep("Name", p->getName());
    string category = askTextOrKeep("Category", p->getCategory());
    double price = askPriceOrKeep(p->getPrice());
    int threshold = askThresholdOrKeep(p->getThreshold());

    inventory.updateProduct(id, name, category, price, threshold);
    printOk("Product updated.");
    saveInventory(files, inventory);
}

void restockProduct(Inventory& inventory, FileManager& files) {
    printHeading("RESTOCK PRODUCT");

    int id = readInt("Product ID to restock (0 = cancel): ", 0, 2000000000);
    if (id == 0) return;
    Product* p = inventory.findProduct(id);
    if (p == nullptr) {
        printError("No product with ID " + to_string(id) + ".");
        return;
    }

    cout << p->getName() << " - stock now: " << p->getQuantity() << "\n";
    int amount = readInt("How many pieces to add? ", 1, Product::MAX_QUANTITY);

    if (inventory.restock(id, amount)) {
        printOk("New stock of " + p->getName() + ": " + to_string(p->getQuantity()));
        saveInventory(files, inventory);
    } else {
        printError("Cannot add that many: the stock would be more than " + to_string(Product::MAX_QUANTITY) + ".");
    }
}

void deleteProduct(Inventory& inventory, FileManager& files, Billing& billing) {
    printHeading("DELETE PRODUCT");

    int id = readInt("Product ID to delete (0 = cancel): ", 0, 2000000000);
    if (id == 0) return;
    Product* p = inventory.findProduct(id);
    if (p == nullptr) {
        printError("No product with ID " + to_string(id) + ".");
        return;
    }

    Product::printHeader();
    p->display();
    if (!askYesNo("Really delete '" + p->getName() + "'?")) {
        cout << "Deletion cancelled.\n";
        return;
    }

    string name = p->getName();
    inventory.deleteProduct(id);
    billing.clearUndoHistory();
    printOk("Product '" + name + "' deleted. Old bills keep the product name.");
    saveInventory(files, inventory);
}

void lowStockProducts(Inventory& inventory) {
    printHeading("LOW-STOCK PRODUCTS");
    int count = inventory.displayLowStock();
    if (count == 0) {
        printOk("All products have enough stock.");
    } else {
        printWarning(to_string(count) + " product(s) need restocking.");
    }
}

void sortProducts(Inventory& inventory) {
    printHeading("SORT PRODUCTS");
    cout << "  1. By price (Bubble Sort)\n";
    cout << "  2. By quantity (Insertion Sort)\n";
    int field = readInt("Choice (1-2): ", 1, 2);
    cout << "  1. Low to high\n";
    cout << "  2. High to low\n";
    int order = readInt("Order (1-2): ", 1, 2);

    inventory.displaySorted(field == 1, order == 1);
}

bool login(Manager& manager) {
    printHeading("MANAGER LOGIN");
    for (int attempt = 1; attempt <= 3; attempt++) {
        string username = readLine("Username: ");
        string password = readLine("Password: ");
        if (manager.checkLogin(username, password)) {
            printOk("Login successful.");
            return true;
        }
        printError("Wrong username or password (try " + to_string(attempt) + " of 3).");
    }
    printWarning("Too many wrong tries. Going back to the main menu.");
    return false;
}

void showMainMenu() {
    printHeading("MAIN MENU");
    cout << "  1. Manager Login\n";
    cout << "  2. View Inventory\n";
    cout << "  3. Search Product\n";
    cout << "  4. Exit\n";
    printLine();
}

void showManagerMenu() {
    printHeading("MANAGER MENU");
    cout << "   1. Add Product                  8. Generate Customer Bill\n";
    cout << "   2. View All Products            9. View Sales History\n";
    cout << "   3. Search Product              10. Generate Monthly Report\n";
    cout << "   4. Update Product              11. Sort Products\n";
    cout << "   5. Restock Product             12. Undo Last Sale\n";
    cout << "   6. Delete Product              13. Logout\n";
    cout << "   7. Display Low-Stock Products\n";
    printLine();
}

int main(int argc, char* argv[]) {
    string folder = "data";
    if (argc > 1) folder = argv[1];

    FileManager files(folder);
    Inventory inventory;
    Manager manager;
    User guest("Guest");
    Billing billing(&inventory, &files);
    int exitCode = 0;

    cout << "\nStockFlow - Smart Inventory & Billing Management System\n";

    if (!files.createMissingFiles()) {
        printError("Cannot prepare the data files: " + files.getErrorMessage());
        exitCode = 1;
    } else if (!manager.loadLogin(folder + "/manager.txt")) {
        printError("manager.txt must contain a username and a password (example: admin admin123).");
        exitCode = 1;
    } else if (!files.loadProducts(inventory) || !billing.loadData()) {
        printError("Cannot load the data: " + files.getErrorMessage());
        exitCode = 1;
    } else {
        guest.displayRole();
        bool running = true;
        bool loggedIn = false;

        while (running) {
            if (loggedIn == false) {
                showMainMenu();
                int choice = readInt("Enter your choice (1-4): ", 1, 4);

                if (choice == 1) {
                    if (login(manager)) {
                        loggedIn = true;
                        manager.displayRole();
                    }
                } else if (choice == 2) {
                    viewAllProducts(inventory);
                } else if (choice == 3) {
                    searchProduct(inventory);
                } else {
                    running = false;
                }
            } else {
                showManagerMenu();
                int choice = readInt("Enter your choice (1-13): ", 1, 13);

                if (choice == 1) addProduct(inventory, files);
                else if (choice == 2) viewAllProducts(inventory);
                else if (choice == 3) searchProduct(inventory);
                else if (choice == 4) updateProduct(inventory, files);
                else if (choice == 5) restockProduct(inventory, files);
                else if (choice == 6) deleteProduct(inventory, files, billing);
                else if (choice == 7) lowStockProducts(inventory);
                else if (choice == 8) billing.generateBill();
                else if (choice == 9) billing.viewSalesHistory();
                else if (choice == 10) billing.monthlyReport();
                else if (choice == 11) sortProducts(inventory);
                else if (choice == 12) billing.undoLastSale();
                else {
                    loggedIn = false;
                    printOk("Logged out.");
                    guest.displayRole();
                }
            }
        }

        if (billing.saveAll()) {
            printOk("All data saved. Goodbye!");
        } else {
            printError("Final save failed: " + files.getErrorMessage());
        }
    }

    billing.cleanup();
    inventory.clear();
    return exitCode;
}
