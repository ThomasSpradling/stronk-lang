#include <iostream>
#include <iomanip>
#include "value.h"

void print_value(Value val) {
    std::cout << std::setprecision(6) << val;
}