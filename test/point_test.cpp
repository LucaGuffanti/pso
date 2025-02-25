#include <algorithm>
#include <iostream>
#include "point.h"
#include "pso.h"

using namespace pso;

double f(Point<3, double> p){
    return std::accumulate(p.begin(), p.end(), 0);
}

int main()
{
    Point<3, double> p1 = 4.0;
    Point<3, double> p2 = {1.0, 2.0, 3.0};
    Point<3, double> p3 = p1 + p2;

    p3 = p3 + 2;

    std::cout << p3 << std::endl;
    std::cout << p2 << std::endl;
    std::cout << p3.dot(p2) << std::endl;

    for (Point<3, double>::Iterator it = p3.begin(); it != p3.end(); it++)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    for (auto x : p3)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    p3.randomize(-10, 10);
    std::cout << p3 << std::endl;

    std::cout << p3.norm() << std::endl;
    std::cout << p3.norm(NormType::ONE_NORM) << std::endl;
    std::cout << p3.norm(NormType::INF_NORM) << std::endl;    

    double tol = 1e-4;

    double max = 10.0;
    double min = 0.0;
    const size_t dim = 3;

    std::function<double(Point<dim, double>)> functional = f;

    
    Pso<dim, double> pso_test(dim*10, tol, 1000, max, min);
    pso_test.run_algorithm(functional);
    pso_test.print_global_best();
    return 0;
}