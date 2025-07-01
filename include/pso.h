#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <random>
#include <memory>
#include "point.cuh"
#include "writer_base.h"
#include <modality.h>

namespace pso{
    /**
     * @brief Header file containing all the necessary includes for the PSO library.
     * 
     * @tparam dim-dimentions for every point
     * @tparam T type for every point coordinate
     */
    template <std::size_t dim, typename T, Modality mod>
    class PsoBase{

        public:
        PsoBase(size_t point_number, double tolerance, size_t max_iter, T domain_max, T domain_min) : m_point_number(point_number), m_tol_min(tolerance),
        m_max_iter(max_iter), m_domain_max(domain_max), m_domain_min(domain_min){}
        ~PsoBase() = default;

        public:
            void run_algorithm(std::function<T(Point<dim, T>&)> functional);
            void print_global_best();
            void use_writer(std::unique_ptr<WriterBase<dim, T>> ptr);

        public: // getter and setter
            size_t get_point_number() {return m_point_number;};
            T get_domain_max() {return m_domain_max;};
            T get_domain_min() {return m_domain_min;};
            const std::vector<Point<dim, T>>& get_pos() const { return m_pos;};
            double get_tol_min(){return m_tol_min;};
            size_t get_max_iter() {return m_max_iter;};

            void set_m_pos_from_d_pos(Point<dim, T> *h_pos) {
                m_pos.assign(h_pos, h_pos + m_point_number);
            };
            
        protected: // methods
            void initialize_random_pos();
            void initialize_random_vel();
            void create_random_vector(size_t length);

        private: // variables
            std::function<T(Point<dim, T>&)> m_functional;
            std::vector<Point<dim, T>> m_pos;
            std::vector<Point<dim, T>> m_vel;
            std::vector<Point<dim, T>> m_personal_best;
            Point<dim, T> m_global_best;
            const T m_domain_max;
            const T m_domain_min;
            double m_tol_min;
            size_t m_max_iter;
            size_t m_point_number;
            std::vector<T> m_r_personal;
            std::vector<T> m_r_global;
            double m_old_vel_weight = 0.5;
            double m_local_tendency = 0.4;
            double m_global_tendency = 0.6; // NB: Check if they need to sum to one
            std::mt19937 gen;
            std::unique_ptr<WriterBase<dim, T>> m_ptr;
    };

    template <std::size_t dim, typename T, Modality mod>
    class Pso : public PsoBase<dim, T, mod>{};

    template <std::size_t dim, typename T>
    class Pso<dim, T, Modality::CPU> : public PsoBase<dim, T, Modality::CPU>{
        public:
            Pso(size_t point_number, double tolerance, size_t max_iter, T domain_max, T domain_min)
                : PsoBase<dim, T, Modality::CPU>(point_number, tolerance, max_iter, domain_max, domain_min) {}
        
        public:
            void initialize_random_pos();
    };
}