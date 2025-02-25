#include "functions/standard_functions.h"
#include <functional>
#include <cassert>

int main()
{
    Point<3, double> p1 = {1.0, 2.0, 3.0};

    std::cout << sphere(p1) << std::endl;
    std::function<double(Point<3, double>&)> f = sphere<3, double>;

    std::cout << f(p1) << std::endl;
    assert(sphere(p1) == f(p1));
}