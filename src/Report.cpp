#include "Reports.h"

#include <cstdio>
#include <iomanip>
#include <iostream>

#include "Utils.h"

using namespace std;

Reports::Reports(const Bill* bills, int billCount) {
    this->bills = bills;
    this->billCount = billCount;
}

void Reports::sortByUnitsSold(ProductSales list[], int size) const {
    for (int i = 1; i < size; i++) {
        ProductSales key = list[i];
        int j = i - 1;
        while (j >= 0 && list[j].unitsSold < key.unitsSold) {
            list[j + 1] = list[j];
            j--;
        }
        list[j + 1] = key;
    }
}

void Reports::monthlyReport(int month, int year) const {
    string monthNames[12] = {"January", "February", "March", "April", "May", "June",
                             "July", "August", "September", "October", "November", "December"};

    char prefix[10];
    snprintf(prefix, sizeof(prefix), "%04d-%02d", year, month);

    int billsInMonth = 0;
    int lineCount = 0;
    for (int i = 0; i < billCount; i++) {
        if (bills[i].getDateTime().substr(0, 7) == prefix) {
            billsInMonth++;
            lineCount = lineCount + bills[i].getItemCount();
        }
    }

    printLine('=', 62);
    cout << "  MONTHLY SALES REPORT - " << monthNames[month - 1] << " " << year << "\n";
    printLine('=', 62);

    if (billsInMonth == 0) {
        cout << "  No sales were recorded in this month.\n";
        printLine('=', 62);
        return;
    }

    ProductSales* sales = new ProductSales[lineCount];
    int productCount = 0;
    int totalUnits = 0;
    double revenue = 0;

    for (int i = 0; i < billCount; i++) {
        if (bills[i].getDateTime().substr(0, 7) != prefix) continue;

        revenue = revenue + bills[i].getTotalAmount();

        for (int k = 0; k < bills[i].getItemCount(); k++) {
            const BillItem& item = bills[i].getItem(k);
            totalUnits = totalUnits + item.quantity;

            int found = -1;
            for (int j = 0; j < productCount; j++) {
                if (sales[j].productId == item.productId) found = j;
            }

            if (found == -1) {
                sales[productCount].productId = item.productId;
                sales[productCount].name = item.productName;
                sales[productCount].unitsSold = item.quantity;
                sales[productCount].revenue = item.quantity * item.unitPrice;
                productCount++;
            } else {
                sales[found].unitsSold = sales[found].unitsSold + item.quantity;
                sales[found].revenue = sales[found].revenue + item.quantity * item.unitPrice;
            }
        }
    }

    sortByUnitsSold(sales, productCount);

    cout << "  Number of bills    : " << billsInMonth << "\n";
    cout << "  Units sold         : " << totalUnits << "\n";
    cout << "  Total revenue      : Rs. " << formatMoney(revenue) << "\n";
    cout << "  Average bill value : Rs. " << formatMoney(revenue / billsInMonth) << "\n";
    printLine('-', 62);
    cout << "  Best-selling products (by units sold)\n";
    cout << "  " << left << setw(6) << "Rank" << setw(28) << "Product" << right << setw(8) << "Units" << setw(16)
         << "Revenue (Rs.)" << "\n";

    for (int i = 0; i < productCount && i < 5; i++) {
        cout << "  " << left << setw(6) << (i + 1) << setw(28) << sales[i].name << right << setw(8)
             << sales[i].unitsSold << setw(16) << formatMoney(sales[i].revenue) << "\n";
    }
    printLine('=', 62);
    cout << left;

    delete[] sales;
}
