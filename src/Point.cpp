#include <Point.h>

using namespace pso;

template <std::size_t dim, typename T>
Point<dim, T>::Point(const T& scalar)
{
    std::fill(data, data + dim, scalar);
}

template <std::size_t dim, typename T>
Point<dim, T>::Point(const std::initializer_list<T>& list)
{
    if (list.size() != dim)
    {
        throw std::invalid_argument("Invalid initializer list size");
    }

    std::copy(list.begin(), list.end(), data);
}

template <std::size_t dim, typename T>
T& Point<dim, T>::at(std::size_t index)
{
    if (index >= dim)
    {
        throw std::out_of_range("Index out of range");
    }

    return data[index];
}

template <std::size_t dim, typename T>
const T Point<dim, T>::at(std::size_t index) const
{
    if (index >= dim)
    {
        throw std::out_of_range("Index out of range");
    }

    return data[index];
}

template <std::size_t dim, typename T>
Point<dim, T> Point<dim, T>::operator+(Point<dim, T>& other) const
{
    Point<dim, T> result;
    for (std::size_t i = 0; i < dim; i++)
    {
        result[i] = data[i] + other[i];
    }

    return result;
}

template <std::size_t dim, typename T>
Point<dim, T> Point<dim, T>::operator+(const T& scalar) const
{
    Point<dim, T> result;
    for (std::size_t i = 0; i < dim; i++)
    {
        result[i] = data[i] + scalar;
    }

    return result;
}

template <std::size_t dim, typename T>
Point<dim, T> Point<dim, T>::operator-(Point<dim, T>& other) const
{
    Point<dim, T> result;
    for (std::size_t i = 0; i < dim; i++)
    {
        result[i] = data[i] - other[i];
    }

    return result;
}

template <std::size_t dim, typename T>
Point<dim, T> Point<dim, T>::operator-(const T& scalar) const
{
    Point<dim, T> result;
    for (std::size_t i = 0; i < dim; i++)
    {
        result[i] = data[i] - scalar;
    }

    return result;
}

template <std::size_t dim, typename T>
Point<dim, T> Point<dim, T>::operator*(T& scalar) const
{
    Point<dim, T> result;
    for (std::size_t i = 0; i < dim; i++)
    {
        result[i] = data[i] * scalar;
    }

    return result;
}

template <std::size_t dim, typename T>
Point<dim, T> Point<dim, T>::operator/(T& scalar) const
{

    if (scalar == static_cast<T>(0))
    {
        throw std::invalid_argument("Division by zero");
    }

    Point<dim, T> result;
    for (std::size_t i = 0; i < dim; i++)
    {
        result[i] = data[i] / scalar;
    }

    return result;
}


template <std::size_t dim, typename T>
T Point<dim, T>::dot(const Point<dim, T>& other) const
{
    T result = static_cast<T>(0);
    for (std::size_t i = 0; i < dim; i++)
    {
        result += data[i] * other[i];
    }

    return result;
}

template <std::size_t dim, typename T>
void Point<dim, T>::print() const
{
    for (std::size_t i = 0; i < dim; i++)
    {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;
}



// Explicit instantiation
template class Point<3, double>;
template class Point<3, float>;
template class Point<3, int>;

template class Point<2, double>;
template class Point<2, float>;
template class Point<2, int>;

template class Point<1, double>;
template class Point<1, float>;
template class Point<1, int>;
