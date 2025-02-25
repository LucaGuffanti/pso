#include "functions/standard_functions.h"
#include "pso.h"
#include "writer_txt.h"
#include <functional>
#include <cassert>

int main()
{
    double tol = 1e-10;
    double max = 10.0;
    double min = -10.0;
    const size_t dim = 2;

    Point<dim, double> p1 = {1.0, 2.0};

    // Test for the norm
    std::function<double(Point<dim, double>&)> f = sphere<dim, double>;
    std::cout << f(p1) << std::endl;
    assert(sphere(p1) == f(p1));

    // Real PSO example\
    std::cout << sphere(p1) << std::endl;
    std::function<double(Point<dim, double>&)> functional = sphere<dim, double>;
    Pso<dim, double> pso_test(dim*10, tol, 1000, max, min);
    std::unique_ptr<WriterBase<dim, double>> p = std::make_unique<WriterTXT<dim, double>>();
    pso_test.use_writer(std::move(p));
    pso_test.run_algorithm(functional);
    pso_test.print_global_best();

}