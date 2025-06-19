#include <iostream>
#include "pso_gpu.cuh"
#include "pso.h"


using namespace pso;

__global__ void setup_kernel(curandState* state, uint64_t seed)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    curand_init(seed, tid, 0, &state[tid]);
}

template<std::size_t dim, typename T>
__global__ void generate_randoms(curandState* globalState, Point<dim, T>* randoms, T domain_min, T domain_max)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    curandState localState = globalState[tid];

    Point<dim, T> tmp;

    T delta = domain_max-domain_min;
    for(int d=0; d<dim; d++){
        randoms[tid][d] = curand_uniform(&localState) * delta + domain_min;
    }
}

template<std::size_t dim, typename T>
void Pso<dim, T, Modality::GPU>::run_algorithm(std::function<T(Point<dim, T>&)> functional){
    std::cout << "RUN ALGORITHM ON GPU" << std::endl;



    initialize_random_pos();

    initialize_random_vel();

    cpy_vec_to_host(d_pos);

    auto pos = this->get_pos();
    for (size_t i = 0; i < pos.size(); ++i) {
        std::cout << "Particle " << i << ": ";
        for (size_t d = 0; d < dim; ++d) {
            std::cout << pos[i][d] << " ";
        }
        std::cout << std::endl;
    }

    // initialize_random_vel();
    // create_random_vector(m_point_number);
    // m_functional = functional;
    
    // if(m_ptr != nullptr){
    //     m_ptr->write_square_domain(m_domain_min, m_domain_max, m_functional);
    // }
    // m_personal_best = m_pos;
    // m_global_best = m_personal_best[0];

    // for(size_t i=0; i<m_point_number; i++){
    //     if (functional(m_global_best) > functional(m_pos[i])){
    //         m_global_best = m_pos[i];
    //     }
    // }

    // double tolerance = m_tol_min + 1;
    // Point<dim, T> last_global_best; 
    // size_t iter = 0;
    // while(tolerance > m_tol_min && iter < m_max_iter){
    //     std::cout << "Iter: " << iter << std::endl;
    //     std::cout << "Tolerance norm: " << tolerance << std::endl;
    //     for(size_t p = 0; p<m_pos.size(); p++){
    //         for(size_t d = 0; d<dim; d++){
    //             // random r_personal, r_global
    //             m_vel[p][d] =   m_old_vel_weight*m_vel[p][d] + 
    //                             m_r_personal[p]*m_local_tendency*(m_personal_best[p][d]-m_pos[p][d]) + 
    //                             m_r_global[p]*m_global_tendency*(m_global_best[d]-m_pos[p][d]);
    //         }
    //         m_pos[p] = m_pos[p] + m_vel[p];
    //         if(functional(m_pos[p]) < functional(m_personal_best[p])){
    //             m_personal_best[p] = m_pos[p];
    //             if(functional(m_personal_best[p]) < functional(m_global_best)){
    //                 last_global_best = m_global_best;
    //                 m_global_best = m_personal_best[p];
    //             }
    //         } 
    //     }
    //     tolerance = std::abs(m_global_best.norm() - last_global_best.norm());
    //     iter ++;
    //     if(m_ptr != nullptr){
    //         m_ptr->write_particle_position(m_pos, m_functional);
    //     }
    // }
}

template<std::size_t dim, typename T>
void Pso<dim, T, Modality::GPU>::initialize_random_pos(){

    std::cout << "Initializing random pos on GPU" << std::endl;

    int n_elements = this->get_point_number();
    check_cuda_error(cudaMalloc(&d_pos, sizeof(Point<dim, T>) * n_elements), "error in malloc pos");
    check_cuda_error(cudaMemset(d_pos, 0, sizeof(Point<dim, T>) * n_elements), "error in pos mem set");

    // Allocate memory for curandState array
    curandState* d_state;
    check_cuda_error(cudaMalloc(&d_state, sizeof(curandState) * n_elements), "error in malloc curandState");

    // KERNEL to set the random state
    int n_threads = 1024;
    int n_blocks = (n_elements + n_threads - 1) / n_threads;
    setup_kernel<<<n_blocks, n_threads>>>(d_state, time(NULL));
    check_cuda_error(cudaDeviceSynchronize(), "synch error on the setup");

    check_cuda_error(cudaGetLastError(), "error after setup_kernel");

    T dom_min = this->get_domain_min();
    T dom_max = this->get_domain_max();

    generate_randoms<dim, T><<<n_blocks, n_threads>>>(d_state, d_pos, dom_min, dom_max);
    check_cuda_error(cudaDeviceSynchronize(), "Synch error on rand set");

    check_cuda_error(cudaGetLastError(), "error after setup_kernel");
}

template<std::size_t dim, typename T>
void Pso<dim, T, Modality::GPU>::initialize_random_vel(){
    std::cout << "Initializing random vel on GPU" << std::endl;

    int n_elements = this->get_point_number();
    check_cuda_error(cudaMalloc(&d_vel, sizeof(Point<dim, T>) * n_elements), "error in malloc pos");
    check_cuda_error(cudaMemset(d_vel, 0, sizeof(Point<dim, T>) * n_elements), "error in pos mem set");

    // Allocate memory for curandState array
    curandState* d_state;
    check_cuda_error(cudaMalloc(&d_state, sizeof(curandState) * n_elements), "error in malloc curandState");

    // KERNEL to set the random state
    int n_threads = 1024;
    int n_blocks = (n_elements + n_threads - 1) / n_threads;
    setup_kernel<<<n_blocks, n_threads>>>(d_state, time(NULL));
    check_cuda_error(cudaDeviceSynchronize(), "synch error on the setup");

    check_cuda_error(cudaGetLastError(), "error after setup_kernel");

    T dom_min = this->get_domain_min();
    T dom_max = this->get_domain_max();

    generate_randoms<dim, T><<<n_blocks, n_threads>>>(d_state, d_vel, dom_min, dom_max);
    check_cuda_error(cudaDeviceSynchronize(), "Synch error on rand set");

    check_cuda_error(cudaGetLastError(), "error after setup_kernel");
}

template<std::size_t dim, typename T>
void Pso<dim, T, Modality::GPU>::check_cuda_error(cudaError_t err, const char *msg){
    if (err != cudaSuccess) {
        std::cerr << "CUDA Error: " << msg << " - " << cudaGetErrorString(err) << std::endl;
        exit(EXIT_FAILURE);
    }
}


template<std::size_t dim, typename T>
void Pso<dim, T, Modality::GPU>::cpy_vec_to_host(Point<dim, T> *d_pos){
    Point<dim, T> *h_pos;

    int n_elements = this->get_point_number();
    h_pos = new Point<dim, T>[n_elements];
    check_cuda_error(cudaMemcpy(h_pos, d_pos, sizeof(Point<dim, T>) * n_elements, cudaMemcpyDeviceToHost), "error copying d_pos to h_pos");
    
    this->set_m_pos_from_d_pos(h_pos);
    delete[] h_pos;
}
// Explicit instantiation of generate_randoms kernel
template __global__ void generate_randoms<3, double>(curandState*, Point<3, double>*, double, double);
template __global__ void generate_randoms<3, float>(curandState*, Point<3, float>*, float, float);

template __global__ void generate_randoms<2, double>(curandState*, Point<2, double>*, double, double);
template __global__ void generate_randoms<2, float>(curandState*, Point<2, float>*, float, float);

template __global__ void generate_randoms<1, double>(curandState*, Point<1, double>*, double, double);
template __global__ void generate_randoms<1, float>(curandState*, Point<1, float>*, float, float);

// Explicit instantiation
template class Pso<3, double, Modality::GPU>;
template class Pso<3, float, Modality::GPU>;

template class Pso<2, double, Modality::GPU>;
template class Pso<2, float, Modality::GPU>;

template class Pso<1, double, Modality::GPU>;
template class Pso<1, float, Modality::GPU>;

