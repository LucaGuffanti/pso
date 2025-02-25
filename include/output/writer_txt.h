#pragma once

#include "writer_base.h"
#include <fstream>

namespace pso
{
    template <std::size_t dim, typename T>
    class WriterTXT : public WriterBase<dim, T>
    {
        public:
        void write_square_domain(const Point<dim, T>& min, const Point<dim, T>& max, std::function<T(Point<dim, T>&)>& func) const override;
        void write_particle_position(const std::vector<Point<dim, T>>& particles, std::function<T(Point<dim, T>&)>& func) override;
    };
}
