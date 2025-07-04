#include "point.h"
#include "pso.h"


int main()
{
    constexpr std::size_t dim = 2;
    double tol = 1e-3;
    double max = 1;
    double min = 1;
    Pso<dim, double, CPUModality> pso_test(dim*10, tol, 1000, max, min);
    pso_test.esempio();

    Pso<dim, double, GPUModality> pso_test2(dim*10, tol, 1000, max, min);
    pso_test2.esempio();

    return 0;
}