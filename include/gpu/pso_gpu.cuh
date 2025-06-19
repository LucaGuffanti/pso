#pragma once

#include <cuda.h>
#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <iostream>
#include "modality.h"
#include "point.cuh"
#include "pso.h"

namespace pso {
    template <std::size_t dim, typename T>
    class Pso<dim, T, Modality::GPU> : public PsoBase<dim, T, Modality::GPU>{
        public:
            Pso(size_t point_number, double tolerance, size_t max_iter, T domain_max, T domain_min)
                : PsoBase<dim, T, Modality::GPU>(point_number, tolerance, max_iter, domain_max, domain_min) {
                    static_assert(dim == dim, "Pso<GPU> specialization being instantiated");
                    // exit(-1);
                }
        
        public:
            void initialize_random_pos();
            void initialize_random_vel();
            void run_algorithm(std::function<T(Point<dim, T>&)> functional);
            void cpy_vec_to_host(Point<dim, T> *d_pos);

        private:    // methods
            void check_cuda_error(cudaError_t err, const char *msg);


        private:    // vars
            Point<dim, T> *d_pos;
            Point<dim, T> *d_vel;
    };
}