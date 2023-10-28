#include <iostream>
#include <iomanip>
#include "value.h"

// Prints a single value with precision 6.
void PrintValue(Value val) {
    std::cout << std::setprecision(6) << val;
}
