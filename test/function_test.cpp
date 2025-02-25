#include "functions/standard_functions.h"
#include "pso.h"
#include <functional>
#include <cassert>

int main()
{
    double tol = 1e-10;

    double max = 10.0;
    double min = 0.0;
    const size_t dim = 3;


    Point<3, double> p1 = {1.0, 2.0, 3.0};

    std::cout << sphere(p1) << std::endl;

    std::function<double(Point<dim, double>&)> functional = sphere<3, double>;

    std::function<double(Point<3, double>&)> f = sphere<3, double>;
    std::cout << f(p1) << std::endl;
    assert(sphere(p1) == f(p1));

    Pso<dim, double> pso_test(dim*10, tol, 1000, max, min);
    pso_test.run_algorithm(functional);

    pso_test.print_global_best();

}