#pragma once
#include <string>

#include "Bill.h"

struct ProductSales {
    int productId;
    std::string name;
    int unitsSold;
    double revenue;
};

class Reports {
private:
    const Bill* bills;
    int billCount;

    void sortByUnitsSold(ProductSales list[], int size) const;

public:
    Reports(const Bill* bills, int billCount);

    void monthlyReport(int month, int year) const;
};
