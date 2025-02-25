#include "writer_txt.h"
#include "square_grid_iterator.h"

using namespace pso;

template <std::size_t dim, typename T>
void WriterTXT<dim, T>::write_square_domain(const Point<dim, T>& min, const Point<dim, T>& max, std::function<T(Point<dim, T>&)>& func) const
{
    std::ofstream file("domain.txt");
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file domain.txt");
    }
    
    // Print the dimensions 
    file << dim << std::endl;

    // Create a vector to store the image of the function
    std::vector<Point<dim + 1, T>> image;

    Point<dim, T> current_point = min;

    SquareGridIterator<dim, 0, T>::iterate(func, current_point, min, max, 0.01, image);
    for (auto& p : image)
    {
        file << p;
    }

    file.close();


}

template <std::size_t dim, typename T>
void WriterTXT<dim, T>::write_particle_position(const std::vector<Point<dim, T>>& particles, std::function<T(Point<dim, T>&)>& func)
{
    std::ofstream file;
    file.open("particles"+std::to_string(this->counter)+".txt");
    (this->counter)++;


    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file particles.txt");
    }

    for (Point<dim, T> p : particles)
    {
        for (std::size_t i = 0; i < dim; i++)
        {
            file << p[i] << " ";
        }
        file << func(p) << std::endl;

    }

    file.close();
}

template class WriterTXT<3, double>;
template class WriterTXT<3, float>;

template class WriterTXT<2, double>;
template class WriterTXT<2, float>;

template class WriterTXT<1, double>;
template class WriterTXT<1, float>;