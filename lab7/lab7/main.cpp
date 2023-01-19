#include "Polynomial.h"
#include "Multiplications.h"
#include <mpi.h>

const std::string APPROACH = "Simple";
const int POLYNOM_SIZE = 100;

int main() {
    MPI_Init(0, 0);
    int me;
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    Polynomial poly1(100);
    Polynomial poly2(100);

    if (me == 0) {
        Polynomial res = master_simple_mpi(poly1, poly2, size);
        std::cout << res << '\n' << std::boolalpha << (res == simple_sequential(poly1, poly2)) << "\n\n";
    }
    else {
        worker_simple_mpi(me);
    }

    MPI_Finalize();
}