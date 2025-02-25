#include "writer_txt.h"
#include "standard_functions.h"
#include <memory>

int main()
{
    std::function<double(Point<2, double>&)> functional = sphere<2, double>;
    std::unique_ptr<WriterBase<2, double>> writer = std::make_unique<WriterTXT<2, double>>();
    writer->write_square_domain({-1.0, -1.0}, {1.0, 1.0}, functional);

    std::vector<Point<2, double>> particles(100);
    for (auto& p : particles)
    {
        p.randomize(-1.0, 1.0);
    }

    writer->write_particle_position(particles, functional);
    for (auto& p : particles)
    {
        p.randomize(-1.0, 1.0);
    }
    writer->write_particle_position(particles, functional);


    return 0;
}