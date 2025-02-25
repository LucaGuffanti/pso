#include "writer_txt.h"
#include "standard_functions.h"

int main()
{
    std::function<double(Point<2, double>&)> functional = sphere<2, double>;
    WriterTXT<2, double> writer;
    writer.write_square_domain({-1.0, -1.0}, {1.0, 1.0}, functional);
    return 0;
}