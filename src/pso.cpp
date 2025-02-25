#include "pso.h"

using namespace pso;

template<std::size_t dim, typename T>
void Pso<dim, T>::initialize_random_pos(){
    for(size_t i=0; i<m_point_number; i++){
        Point<dim, T> point;
        point.randomize(m_domain_min, m_domain_max);
        m_pos.push_back(point);
    }
}

template<std::size_t dim, typename T>
void Pso<dim, T>::initialize_random_vel(){
    for(size_t i=0; i<m_point_number; i++){
        Point<dim, T> point;
        point.randomize(m_domain_min, m_domain_max);
        m_vel.push_back(point);
    }
}

template<std::size_t dim, typename T>
void Pso<dim, T>::create_random_vector(size_t length){
    std::uniform_real_distribution<T> dist(0.0, 1.0);
    std::vector<T> rand;
    for(size_t i=0; i<length; i++){
        m_r_personal.push_back(dist(gen));
    }

    for(size_t i=0; i<length; i++){
        m_r_global.push_back(dist(gen));
    }
}

template<std::size_t dim, typename T>
void Pso<dim,T>::use_writer(std::unique_ptr<WriterBase<dim, T>> ptr){
    m_ptr = std::move(ptr);
}

template<std::size_t dim, typename T>
void Pso<dim, T>::run_algorithm(std::function<T(Point<dim, T>&)> functional){
    initialize_random_pos();
    std::cout << "Cacca" << std::endl;
    initialize_random_vel();
    create_random_vector(m_point_number);
    m_functional = functional;
    
    if(m_ptr != nullptr){
        m_ptr->write_square_domain(m_domain_min, m_domain_max, m_functional);
    }
    m_personal_best = m_pos;
    m_global_best = m_personal_best[0];

    for(size_t i=0; i<m_point_number; i++){
        if (functional(m_global_best) > functional(m_pos[i])){
            m_global_best = m_pos[i];
        }
    }

    double tolerance = m_tol_min + 1;
    Point<dim, T> last_global_best; 
    size_t iter = 0;
    while(tolerance > m_tol_min && iter < m_max_iter){
        std::cout << "Iter: " << iter << std::endl;
        std::cout << "Tolerance norm: " << tolerance << std::endl;
        for(size_t p = 0; p<m_pos.size(); p++){
            for(size_t d = 0; d<dim; d++){
                // random r_personal, r_global
                m_vel[p][d] =   m_old_vel_weight*m_vel[p][d] + 
                                m_r_personal[p]*m_local_tendency*(m_personal_best[p][d]-m_pos[p][d]) + 
                                m_r_global[p]*m_global_tendency*(m_global_best[d]-m_pos[p][d]);
            }
            m_pos[p] = m_pos[p] + m_vel[p];
            if(functional(m_pos[p]) < functional(m_personal_best[p])){
                m_personal_best[p] = m_pos[p];
                if(functional(m_personal_best[p]) < functional(m_global_best)){
                    last_global_best = m_global_best;
                    m_global_best = m_personal_best[p];
                }
            } 
        }
        tolerance = std::abs(m_global_best.norm() - last_global_best.norm());
        iter ++;
        if(m_ptr != nullptr){
            m_ptr->write_particle_position(m_pos, m_functional);
        }
    }
}

template<std::size_t dim, typename T>
void Pso<dim, T>::print_global_best(){
    std::cout << "Printing best pos: " << std::endl;
    m_global_best.print();
}

template class Pso<3, double>;
template class Pso<3, float>;

template class Pso<2, double>;
template class Pso<2, float>;

template class Pso<1, double>;
template class Pso<1, float>;