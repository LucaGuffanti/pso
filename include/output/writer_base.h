#pragma once

#include <cstddef>
#include <vector>
#include <functional>

#include "point.h"

namespace pso
{
    template <std::size_t dim, typename T>
    class WriterBase
    {
        protected:
        std::size_t counter = 0;
        public:

        virtual void write_square_domain(const Point<dim, T>& min, const Point<dim, T>& max, std::function<T(Point<dim, T>&)>& func) const { std::exit(1);};
        virtual void write_particle_position(const std::vector<Point<dim, T>>& particles, std::function<T(Point<dim, T>&)>& func) {std::exit(1);}; 
        ~WriterBase() = default;
    };
}