
#pragma once
#include <cstdio>
#include <iomanip>
#include <map>
#include <vector>
#include "Bill.cpp"
#include "ReportGenerator.cpp"


template <typename T, typename Less>
void mergeSortRange(std::vector<T>& a, std::vector<T>& tmp, size_t lo, size_t hi, Less less) { 
    if (hi - lo < 2) return;
    size_t mid = lo + (hi - lo) / 2;
    mergeSortRange(a, tmp, lo, mid, less);
    mergeSortRange(a, tmp, mid, hi, less);
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) tmp[k++] = less(a[j], a[i]) ? a[j++] : a[i++];  
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi) tmp[k++] = a[j++];
    for (size_t x = lo; x < hi; ++x) a[x] = tmp[x];
}
template <typename T, typename Less>
void mergeSort(std::vector<T>& v, Less less) {
    std::vector<T> tmp(v);
    mergeSortRange(v, tmp, 0, v.size(), less);
}

struct ProductSales {
    int productId;
    std::string name;
    int quantity;
    double revenue;
};

class MonthlyReport : public ReportGenerator {
    const std::vector<Bill>& bills_;
    int month_;
    int year_;

public:
    MonthlyReport(const std::vector<Bill>& bills, int month, int year) : bills_(bills), month_(month), year_(year) {}

    static const char* monthName(int m) {
        static const char* names[] = {"January", "February", "March", "April", "May", "June", "July",
                                      "August", "September", "October", "November", "December"};
        return (m >= 1 && m <= 12) ? names[m - 1] : "?";
    }

    std::string title() const override { return std::string("Monthly Sales Report - ") + monthName(month_) + " " + std::to_string(year_); }

    
    std::vector<ProductSales> ranking(int& billCount, double& revenue, int& units) const {
        char prefix[16];
        std::snprintf(prefix, sizeof prefix, "%04d-%02d", year_, month_);  
        std::map<int, ProductSales> byProduct;
        billCount = 0; revenue = 0; units = 0;
        for (const Bill& b : bills_) {
            if (b.timestamp().compare(0, 7, prefix) != 0) continue;
            ++billCount;
            revenue += b.grandTotal();
            for (const BillItem& i : b.items()) {
                ProductSales& ps = byProduct[i.productId];
                ps.productId = i.productId; ps.name = i.productName;
                ps.quantity += i.quantity; ps.revenue += i.lineTotal();
                units += i.quantity;
            }
        }
        std::vector<ProductSales> v;
        for (const auto& kv : byProduct) v.push_back(kv.second);
        mergeSort(v, [](const ProductSales& a, const ProductSales& b) {
            if (a.quantity != b.quantity) return a.quantity > b.quantity;
            return a.name < b.name;
        });
        return v;
    }

    void generate(std::ostream& os) const override {
        int bills = 0, units = 0;
        double revenue = 0;
        std::vector<ProductSales> rank = ranking(bills, revenue, units);
        os << std::string(62, '=') << "\n  " << title() << "\n" << std::string(62, '=') << "\n";
        if (bills == 0) { os << "  No sales were recorded in this month.\n" << std::string(62, '=') << "\n"; return; }
        os << "  Completed bills   : " << bills << "\n"
           << "  Units sold        : " << units << "\n"
           << "  Total revenue     : Rs. " << util::money(revenue) << "\n"
           << "  Average bill value: Rs. " << util::money(revenue / bills) << "\n" << std::string(62, '-') << "\n"
           << "  Best-selling products (by units sold)\n"
           << "  " << std::left << std::setw(6) << "Rank" << std::setw(28) << "Product" << std::right << std::setw(8) << "Units"
           << std::setw(16) << "Revenue (Rs.)" << "\n";
        for (size_t i = 0; i < rank.size() && i < 5; ++i)
            os << "  " << std::left << std::setw(6) << (i + 1) << std::setw(28) << util::fit(rank[i].name, 27) << std::right << std::setw(8)
               << rank[i].quantity << std::setw(16) << util::money(rank[i].revenue) << "\n";
        os << std::left << std::string(62, '=') << "\n";
    }
};

