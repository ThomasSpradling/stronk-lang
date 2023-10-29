#include "A.h"
#include "B.h" // Include the header for class B

void A::f() {
    b->g(this); // Pass the pointer to this instance of A
}
