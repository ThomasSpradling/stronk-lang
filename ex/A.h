#ifndef A_H
#define A_H

class B; // Forward declaration of class B

class A {
public:
    void f();

private:
    B* b; // Pointer to class B
};

#endif // A_H
