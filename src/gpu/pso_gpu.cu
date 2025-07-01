#include <iostream>
#include "pso_gpu.cuh"
#include "pso.h"


using namespace pso;

struct Sphere {
    template <size_t dim, typename T>
    __host__ __device__ T operator()(Point<dim, T> pos) const {
        T result = 0;
        for(int d=0; d<dim; d++){
            result += pos[d] * pos[d];
        }
        return result;
    }
};

template <size_t dim, typename T, typename Func>
__global__ void eval_function(Func f, int size, T *d_eval_f, Point<dim, T> *d_pos) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx >= size) return;

    d_eval_f[idx] = f.template operator()<dim, T>(d_pos[idx]);  // Call templated functor
}

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

template<typename T>
__global__ void generate_randoms(curandState* globalState, T* randoms)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    curandState localState = globalState[tid];

    randoms[tid] = curand_uniform(&localState);
}

template<std::size_t dim, typename T>
__global__ void update_velocity(
            int size, 
            Point<dim, T> *d_vel_new,
            Point<dim, T> *d_pos,
            double m_old_vel_weight,
            Point<dim, T> *d_vel,
            T *d_r_personal,
            double m_local_tendency,
            Point<dim, T> *d_personal_best,
            T *d_r_global,
            double m_global_tendency,
            Point<dim, T> h_global_best){
    // ALGO
    // m_vel[p][d] =   m_old_vel_weight*m_vel[p][d] + 
    //                             m_r_personal[p]*m_local_tendency*(m_personal_best[p][d]-m_pos[p][d]) + 
    //                             m_r_global[p]*m_global_tendency*(m_global_best[d]-m_pos[p][d]);
    //         }

    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= size) return;

    Point<dim, T> interm0;
    Point<dim, T> interm1;
    Point<dim, T> interm2;
    
    interm0 = d_vel[tid] * m_old_vel_weight;
    interm1 = (d_personal_best[tid] - d_pos[tid]) * (d_r_personal[tid] * m_local_tendency);
    interm2 = (h_global_best - d_pos[tid]) * (d_r_global[tid] * m_global_tendency);
    
    d_vel_new[tid] = interm0 + interm1 + interm2;
}

template<std::size_t dim, typename T>
__global__ void update_velocity(
            int n_elements,
            Point<dim, T> *d_pos_new,
            Point<dim, T> *d_vel_new,
            Point<dim, T> *d_pos
    ){
    // Update vol
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= n_elements) return;

    d_pos_new[tid] = d_pos[tid] + d_vel_new[tid];
}

template<std::size_t dim, typename T, typename Func>
__global__ void update_personal_best(
            int n_elements,
            Func f,
            Point<dim, T> *d_personal_best,
            Point<dim, T> *d_pos_new
        ){
    // 
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= n_elements) return;
    
    Point<dim, T> max = (f(d_personal_best[tid]) < f(d_pos_new[tid])) ? d_personal_best[tid] : d_pos_new[tid];
    d_personal_best[tid] = max;
}

template<std::size_t dim, typename T>
void Pso<dim, T, Modality::GPU>::run_algorithm(std::function<T(Point<dim, T>&)> functional){
    std::cout << "RUN ALGORITHM ON GPU" << std::endl;

    initialize_random_vec(&d_pos);
    initialize_random_vec(&d_vel);

    initialize_random_vec(&d_r_personal);
    initialize_random_vec(&d_r_global);

    // cpy_vec_to_host(d_pos);

    // Allocate memory for d_personal_best vector on the device
    int n_elements = this->get_point_number();
    check_cuda_error(cudaMalloc(&d_personal_best, sizeof(Point<dim, T>) * n_elements), "error in malloc d_personal_best");
    check_cuda_error(cudaMemcpy(d_personal_best, d_pos, sizeof(Point<dim, T>) * n_elements, cudaMemcpyDeviceToDevice), "error copying d_pos to d_personal_best");
    
    // Allocate memory for d_eval_f vector on the device
    check_cuda_error(cudaMalloc(&d_eval_function, sizeof(T) * n_elements), "error in malloc d_eval_function");
    check_cuda_error(cudaMemset(d_eval_function, 0, sizeof(T) * n_elements), "error in memset d_eval_function");

    // eval f and find the best
    int n_threads = 1024;
    int n_blocks = (n_elements + n_threads - 1) / n_threads;

    eval_function<dim, T><<<n_blocks, n_threads>>>(Sphere{}, n_elements, d_eval_function, d_pos);
    cudaDeviceSynchronize();

    // Take the iterators for the eval vector and find the min idx without sorting
    auto eval_begin = thrust::device_pointer_cast(d_eval_function);
    auto eval_end   = eval_begin + n_elements;
    
    auto min_eval_ptr = thrust::min_element(
        eval_begin,
        eval_end
    );   
    int min_index = min_eval_ptr - eval_begin;
    
    // Global best
    check_cuda_error(cudaMemcpy(&h_global_best, d_pos + min_index, sizeof(Point<dim, T>), cudaMemcpyDeviceToHost), "error copying d_pos[min_index] to d_global_best");
    
    double tol_min = this->get_tol_min();
    double tolerance = tol_min + 1000;
    
    size_t iter = 0;
    size_t max_iter = this->get_max_iter();
    
    Point<dim, T> last_global_best = h_global_best; 

    Point<dim, T> *d_vel_new;
    check_cuda_error(cudaMalloc(&d_vel_new, sizeof(Point<dim, T>) * n_elements), "error in malloc d_vel_new");

    Point<dim, T> *d_pos_new;
    check_cuda_error(cudaMalloc(&d_pos_new, sizeof(Point<dim, T>) * n_elements), "error in malloc d_vel_new");


    while(tolerance > tol_min && iter < max_iter){
        std::cout << "Iter: " << iter << std::endl;
        std::cout << "Tolerance norm: " << tolerance << std::endl;

//      m_vel[p][d] =   m_old_vel_weight*m_vel[p][d] + 
//                      m_r_personal[p]*m_local_tendency*(m_personal_best[p][d]-m_pos[p][d]) + 
//                      m_r_global[p]*m_global_tendency*(m_global_best[d]-m_pos[p][d]);

        // COMPUTE THE VELOCITY UPDATE
        update_velocity<dim, T><<<n_blocks, n_threads>>>(
            n_elements,
            d_vel_new,
            d_pos,
            m_old_vel_weight,
            d_vel,
            d_r_personal,
            m_local_tendency,
            d_personal_best,
            d_r_global,
            m_global_tendency,
            h_global_best
        );
        cudaDeviceSynchronize();

//       m_pos[p] = m_pos[p] + m_vel[p];
        update_velocity<dim, T><<<n_blocks, n_threads>>>(
            n_elements,
            d_pos_new,
            d_vel_new,
            d_pos
        );
        cudaDeviceSynchronize();

        update_personal_best<dim, T><<<n_blocks, n_threads>>>(
            n_elements,
            Sphere{},
            d_personal_best,
            d_pos_new
        );
        cudaDeviceSynchronize();

        eval_function<dim, T><<<n_blocks, n_threads>>>(Sphere{}, n_elements, d_eval_function, d_pos_new);
        cudaDeviceSynchronize();

        // Take the iterators for the eval vector and find the min idx without sorting
        auto eval_begin = thrust::device_pointer_cast(d_eval_function);
        auto eval_end   = eval_begin + n_elements;

        auto min_eval_ptr = thrust::min_element(
            eval_begin,
            eval_end
        );   
        int min_index = min_eval_ptr - eval_begin;
        
        // Global best
        
        last_global_best = h_global_best;
        check_cuda_error(cudaMemcpy(&h_global_best, d_pos_new+ min_index, sizeof(Point<dim, T>), cudaMemcpyDeviceToHost), "error copying d_pos[min_index] to d_global_best");
        

        tolerance = std::abs(h_global_best.norm() - last_global_best.norm());

        std::cout << "Last global best: ";
        for (size_t d = 0; d < dim; ++d) {
            std::cout << last_global_best[d] << " ";
        }
        std::cout << std::endl;
        std::cout << "Current global best: ";
        for (size_t d = 0; d < dim; ++d) {
            std::cout << h_global_best[d] << " ";
        }
        std::cout << std::endl;

        iter ++;
        d_pos = d_pos_new;
        d_vel = d_vel_new;
    }

    std::cout << "Best position found: ";
    for (size_t d = 0; d < dim; ++d) {
        std::cout << h_global_best[d] << " ";
    }
    std::cout << std::endl;
    std::cout << "Best value: " << Sphere{}(h_global_best) << std::endl;
}

template<std::size_t dim, typename T>
void Pso<dim, T, Modality::GPU>::initialize_random_vec(Point<dim, T> **d_vec){

    std::cout << "Initializing random Point vec on GPU" << std::endl;

    int n_elements = this->get_point_number();
    check_cuda_error(cudaMalloc(d_vec, sizeof(Point<dim, T>) * n_elements), "error in malloc pos");
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

    generate_randoms<dim, T><<<n_blocks, n_threads>>>(d_state, *d_vec, dom_min, dom_max);
    check_cuda_error(cudaDeviceSynchronize(), "Synch error on rand set");

    check_cuda_error(cudaGetLastError(), "error after setup_kernel");
}

template<std::size_t dim, typename T>
void Pso<dim, T, Modality::GPU>::initialize_random_vec(T **d_vec){

    std::cout << "Initializing random T vec on GPU" << std::endl;

    int n_elements = this->get_point_number();
    check_cuda_error(cudaMalloc(d_vec, sizeof(T) * n_elements), "error in malloc pos");
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

    generate_randoms<T><<<n_blocks, n_threads>>>(d_state, *d_vec);
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

