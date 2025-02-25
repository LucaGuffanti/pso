#pragma once

/**
 * @brief Standard functions used for benchmarking purposes.
 */

#include <cstddef>
#include <cmath>
#include <algorithm>
#include <numeric>

#include "point.h"

namespace pso
{

    /**
     * @brief Sphere function. 
     * @note Minimum : 0
     * @note Preferred domain: [-5.12, 5.12]
     */
    template<std::size_t dim, typename T> 
    T sphere(Point<dim, T>& point)
    {
        return std::accumulate(point.begin(), point.end(), static_cast<T>(0), [](T acc, T x){ return acc + x * x; });
    }


}