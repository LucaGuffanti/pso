#include "functions/standard_functions.h"
#include "pso.h"
#include "pso_gpu.cuh"
#include "writer_txt.h"
#include <functional>
#include <cassert>
#include <vector>
#include <random>
#include <chrono>

int main()
{
    double tol = 1e-10;
    double max = 10.0;
    double min = -10.0;
    const size_t dim = 3;

    // Point<dim, double> p1 = {1.0, 2.0};

    // std::cout << "debug\n";
    size_t n = 10000;

    using T = Point<dim, double>;
    T* vec1 = new T[n];
    T* vec2 = new T[n];
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);

    for (size_t i = 0; i < n; ++i) {
        for (size_t d = 0; d < dim; ++d) {
            vec1[i][d] = dis(gen);
        }
    }
    for (size_t i = 0; i < n; ++i) {
        for (size_t d = 0; d < dim; ++d) {
            vec2[i][d] = dis(gen);
        }
    }



    // GPU
    Pso<dim, double, Modality::GPU> pso_test_gpu(dim*n, tol, 100, max, min); 
    pso_test_gpu.set_d_vec(vec1, true);
    pso_test_gpu.set_d_vec(vec2, false);

    auto start = std::chrono::high_resolution_clock::now();
    pso_test_gpu.run_algorithm();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "GPU run_algorithm() time: " << elapsed.count() << " seconds\n";
    
    pso_test_gpu.print_global_best();


    // CPU

    std::vector<T> vec1_vec(dim * n);
    std::vector<T> vec2_vec(dim * n);

    for (size_t i = 0; i < n; ++i) {
        for (size_t d = 0; d < dim; ++d) {
            vec1_vec[i][d] = vec1[i][d];
        }
    }
    for (size_t i = 0; i < n; ++i) {
        for (size_t d = 0; d < dim; ++d) {
            vec2_vec[i][d] = vec2[i][d];
        }
    }

    std::function<double(Point<dim, double>&)> functional = sphere<dim, double>;

    Pso<dim, double, Modality::CPU> pso_test_cpu(dim*n, tol, 1000, max, min);
    pso_test_gpu.set_m_vec(&vec1_vec, true);
    pso_test_gpu.set_m_vec(&vec2_vec, false);
    auto start_cpu = std::chrono::high_resolution_clock::now();
    pso_test_cpu.run_algorithm(functional);
    auto end_cpu = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_cpu = end_cpu - start_cpu;
    std::cout << "CPU run_algorithm() time: " << elapsed_cpu.count() << " seconds\n";
    pso_test_cpu.print_global_best();

}