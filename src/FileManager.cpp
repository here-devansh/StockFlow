#include "FileManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Utils.h"

using namespace std;

const string HEADER = "id,name,category,price,quantity,threshold";

FileManager::FileManager(const string& folder) {
    this->folder = folder;
}

string FileManager::getErrorMessage() const {
    return errorMessage;
}

bool FileManager::createMissingFiles() {
    string productsPath = folder + "/products.csv";
    ifstream productsFile(productsPath);
    if (!productsFile) {
        ofstream newFile(productsPath);
        if (!newFile) {
            errorMessage = "cannot create " + productsPath + " (does the '" + folder + "' folder exist?)";
            return false;
        }
        newFile << HEADER << "\n";
        printWarning("products.csv was missing and has been created.");
    }

    string managerPath = folder + "/manager.txt";
    ifstream managerFile(managerPath);
    if (!managerFile) {
        ofstream newFile(managerPath);
        if (!newFile) {
            errorMessage = "cannot create " + managerPath;
            return false;
        }
        newFile << "admin admin123\n";
        printWarning("manager.txt was missing and has been created (admin / admin123).");
    }
    return true;
}

bool FileManager::addProductFromLine(const string& line, Inventory& inventory) {

    stringstream stream(line);
    string part[6];
    int partCount = 0;
    string oneField;
    while (getline(stream, oneField, ',')) {
        if (partCount == 6) return false;
        part[partCount] = oneField;
        partCount++;
    }
    if (partCount != 6) return false;

    int id, quantity, threshold;
    double price;
    if (!textToInt(part[0], id)) return false;
    if (!textToDouble(part[3], price)) return false;
    if (!textToInt(part[4], quantity)) return false;
    if (!textToInt(part[5], threshold)) return false;

    if (id <= 0) return false;
    if (!Product::isValidText(part[1]) || !Product::isValidText(part[2])) return false;
    if (!Product::isValidPrice(price)) return false;
    if (!Product::isValidQuantity(quantity) || !Product::isValidQuantity(threshold)) return false;

    Product p(id, part[1], part[2], price, quantity, threshold);
    return inventory.addProduct(p);
}

void FileManager::makeBackup(const string& path) {
    ifstream in(path);
    ofstream out(path + ".bak");
    string line;
    while (getline(in, line)) {
        out << line << "\n";
    }
    printWarning("A copy of the original file was saved as " + path + ".bak");
}

bool FileManager::loadProducts(Inventory& inventory) {
    string path = folder + "/products.csv";
    ifstream file(path);
    if (!file) {
        errorMessage = "cannot open " + path;
        return false;
    }

    int loaded = 0;
    int skipped = 0;
    string line;

    getline(file, line);
    if (line != HEADER) skipped++;

    while (getline(file, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);
        if (line.empty()) continue;
        if (addProductFromLine(line, inventory)) loaded++;
        else skipped++;
    }
    file.close();

    cout << "Loaded " << loaded << " product(s).\n";
    if (skipped > 0) {
        printWarning(to_string(skipped) + " bad line(s) were skipped in products.csv.");
        makeBackup(path);
    }
    return true;
}

bool FileManager::saveProducts(const Inventory& inventory) {
    string path = folder + "/products.csv";
    ofstream file(path);
    if (!file) {
        errorMessage = "cannot open " + path + " for writing";
        return false;
    }

    file << HEADER << "\n";
    for (ProductNode* p = inventory.getHead(); p != nullptr; p = p->next) {
        file << p->data.getId() << ","
             << p->data.getName() << ","
             << p->data.getCategory() << ","
             << formatMoney(p->data.getPrice()) << ","
             << p->data.getQuantity() << ","
             << p->data.getThreshold() << "\n";
    }

    if (!file) {
        errorMessage = "writing to " + path + " failed";
        return false;
    }
    return true;
}
