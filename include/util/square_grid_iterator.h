#pragma once

/**
 * @brief This file contains a metafunction used to iterate on a square grid of dimensions dim, evaluating a function at each point of the grid
 */

#include <cstddef>
#include <functional>
#include <array>
#include <vector>

#include "point.h"

namespace pso
{
    template<std::size_t dim, std::size_t current_dim, typename T> 
    class SquareGridIterator
    {

        public:
        static void iterate(const std::function<T(Point<dim, T>&)>& func, Point<dim, T>& current_point, const Point<dim, T>& min, const Point<dim, T>& max, const T delta, std::vector<Point<dim + 1, T>>& image)
        {
            for (T i = min[current_dim]; i <= max[current_dim]; i += delta)
            {
                current_point[current_dim] = i;
                SquareGridIterator<dim, current_dim + 1, T>::iterate(func, current_point, min, max, delta, image);
            }
        }
        
    };

    template<std::size_t dim, typename T>
    class SquareGridIterator<dim, dim, T>
    {
        public:
        static void iterate(const std::function<T(Point<dim, T>&)>& func, Point<dim, T>& current_point, const Point<dim, T>& min, const Point<dim, T>& max, const T delta, std::vector<Point<dim + 1, T>>& image)
        {
            Point<dim + 1, T> point;
            for (std::size_t i = 0; i < dim; i++)
            {
                point[i] = current_point[i];
            }
            point[dim] = func(current_point);
            image.push_back(point);
        }
    };
}