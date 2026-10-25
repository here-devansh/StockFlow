#pragma once
#include <string>

void printLine(char ch = '=', int count = 70);
void printHeading(const std::string& title);
void printOk(const std::string& message);
void printError(const std::string& message);
void printWarning(const std::string& message);

std::string trimText(const std::string& text);
std::string toLower(std::string text);
std::string formatMoney(double value);

bool textToInt(const std::string& text, int& number);
bool textToDouble(const std::string& text, double& number);

std::string readLine(const std::string& prompt);
int readInt(const std::string& prompt, int minValue, int maxValue);
double readDouble(const std::string& prompt, double minValue, double maxValue);
std::string readText(const std::string& prompt);
