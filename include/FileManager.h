#pragma once
#include <string>

#include "Inventory.h"

class FileManager {
private:
    std::string folder;
    std::string errorMessage;

    bool addProductFromLine(const std::string& line, Inventory& inventory);
    void makeBackup(const std::string& path);

public:
    FileManager(const std::string& folder);

    std::string getErrorMessage() const;

    bool createMissingFiles();
    bool loadProducts(Inventory& inventory);
    bool saveProducts(const Inventory& inventory);
};
