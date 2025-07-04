#include <point.cuh>

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
Point<dim, T> Point<dim, T>::operator+(const Point<dim, T>& other) const
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
Point<dim, T> Point<dim, T>::operator-(const Point<dim, T>& other) const
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
Point<dim, T> Point<dim, T>::operator*(const T& scalar) const
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
void Point<dim, T>::randomize(const T& min, const T& max)
{
    std::uniform_real_distribution<T> dist(min, max);
    for (auto& x: data)
    {
        x = dist(Point<dim,T>::gen);
    }
}

template <std::size_t dim, typename T>
T Point<dim, T>::norm(const NormType type) const
{
    if (type == NormType::ONE_NORM)
    {
        T sum = static_cast<T>(0);
        for (std::size_t i = 0; i < dim; ++i) {
            sum += data[i] < static_cast<T>(0) ? -data[i] : data[i];
        }
        return sum;
    }
    else if (type == NormType::TWO_NORM)
    {
        T sum = static_cast<T>(0);
        for (std::size_t i = 0; i < dim; ++i) {
            sum += data[i] * data[i];
        }
        return (T)sqrt(sum);
    }
    else
    {
        #ifndef __CUDA_ARCH__
            return *std::max_element(data, data + dim);
        #else
            // GPU-friendly max element (manual loop, no exceptions)
            T max_val = data[0];
            for (std::size_t i = 1; i < dim; ++i) {
                if (data[i] > max_val) max_val = data[i];
            }
            return max_val;
        #endif
    }
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

template class Point<2, double>;
template class Point<2, float>;

template class Point<1, double>;
template class Point<1, float>;
