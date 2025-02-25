#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <random>
#include "point.h"

namespace pso{
    /**
     * @brief Header file containing all the necessary includes for the PSO library.
     * 
     * @tparam dim-dimentions for every point
     * @tparam T type for every point coordinate
     */
    template <std::size_t dim, typename T>
    class Pso{

        public:
        Pso(size_t point_number, double tolerance, size_t max_iter, T domain_max, T domain_min) : m_point_number(point_number), m_tol_min(tolerance),
        m_max_iter(max_iter), m_domain_max(domain_max), m_domain_min(domain_min){}
        ~Pso() = default;

        public:
            void run_algorithm(std::function<T(Point<dim, T>&)> functional);
            void print_global_best();

        private: // methods
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
    };
}