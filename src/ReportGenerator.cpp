#pragma once
#include <ostream>
#include <string>

class ReportGenerator {
public:
    virtual ~ReportGenerator() = default;
    virtual std::string title() const = 0;               
    virtual void generate(std::ostream& os) const = 0;   
};
