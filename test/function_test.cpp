#include "functions/standard_functions.h"

int main()
{
    Point<3, double> p1 = {1.0, 2.0, 3.0};

    std::cout << sphere(p1) << std::endl;
}