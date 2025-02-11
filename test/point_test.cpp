#include "Point.h"
#include <iostream>

using namespace pso;
int main()
{
    Point<3, double> p1 = 4.0;
    Point<3, double> p2 = {1.0, 2.0, 3.0};
    Point<3, double> p3 = p1 + p2;

    p1.print();
    p2.print();
    p3.print();
    return 0;
}