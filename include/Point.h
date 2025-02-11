#pragma once

#include <cstddef>
#include <stdexcept>
#include <concepts>
#include <functional>
#include <type_traits>
#include <limits>
#include <iostream>


namespace pso
{   
    /**
     * @brief Class that represents a point in a dim-dimensional space.
     * 
     * @tparam dim dimension of the space
     * @tparam T scalar
     */
    template <std::size_t dim, typename T>
    requires std::is_arithmetic_v<T>
    class Point
    {
    private:
        T data[dim];
    public:

        Point() { std::fill(data, data + dim, static_cast<T>(0)); };
        Point(const Point<dim, T>&) = default;
        Point(Point<dim, T>&&) = default;
        Point(const T& scalar);
        Point(const std::initializer_list<T>& list);

        Point<dim, T>& operator=(const Point<dim, T>&) = default;
        Point<dim, T>& operator=(Point<dim, T>&&) = default;

        ~Point() = default;


        /**
         * @brief Accesses the element at the specified index, without control over the bounds
         * 
         * @param index index
         * @return T& reference to the element
         */
        T& operator[](std::size_t index){ return data[index];}

        /**
         * @brief Accesses the element at the specified index, without control over the bounds
         * 
         * @param index index
         * @return const T reference to the element
         */
        const T operator[](std::size_t index) const { return data[index];}

        /**
         * @brief Accesses the element at the specified index, with control over the bounds.
         * 
         * @param index index
         * @throws std::out_of_range if index is out of range
         * @return T& reference to the element
         */
        T& at(std::size_t index);

        /**
         * @brief Accesses the element at the specified index, with control over the bounds.
         * 
         * @param index index
         * @throws std::out_of_range if index is out of range
         * @return const T reference to the element
         */
        const T at(std::size_t index) const;
    
        /**
         * @brief Performs element-wise addition of two points.
         * 
         * @param other other point
         * @return Point a new point containing the sum
         */
        Point operator+(Point<dim, T>& other) const;


        /**
         * @brief Performs element-wise addition of a point and a scalar.
         * 
         * @param scalar scalar
         * @return Point a new point containing the sum
         */
        Point operator+(const T& scalar) const;

        /**
         * @brief Performs element-wise subtraction of two points.
         * 
         * @param other other point
         * @return Point a new point containing the difference
         */
        Point operator-(Point<dim, T>& other) const;

        /**
         * @brief Performs element-wise subtraction of a point and a scalar.
         * 
         * @param scalar scalar
         * @return Point a new point containing the difference
         */
        Point operator-(const T& scalar) const;

        /**
         * @brief Performs element-wise multiplication of a point by a scalar.
         * 
         * @param scalar scalar
         * @return Point a new point containing the product
         */
        Point operator*(T& scalar) const;

        /**
         * @brief Performs element-wise division of a point by a scalar.
         * 
         * @param scalar scalar
         * @throws std::invalid_argument if scalar is zero
         * @return Point a new point containing the quotient
         */
        Point operator/(T& scalar) const;

        /**
         * @brief Computes the dot product of two points.
         * 
         * @param other other point
         * @return T dot product
         */
        T dot(const Point<dim, T>& other) const;


        void print() const;
    };
}

using namespace pso;

template<std::size_t dim, typename T>
std::ostream& operator<<(std::ostream& os, const Point<dim, T>& point)
{
    for (std::size_t i = 0; i < dim; i++)
    {
        os << point[i] << " ";
    }
    os << std::endl;
    return os;
}