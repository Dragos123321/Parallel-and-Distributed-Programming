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

    Polynomial poly1(10);
    Polynomial poly2(10);

    if (me == 0) {
        Polynomial res = karatsuba_mpi(poly1, poly2, me, size);
        std::cout << res << '\n' << std::boolalpha << (res == simple_sequential(poly1, poly2)) << "\n\n";
    }
    else {
        Polynomial p1(0), p2(0);
        int parent_id;
        karatsuba_recv_polys(p1, p2, parent_id, me);
        auto res = karatsuba_mpi(p1, p2, me, size);
        karatsuba_send_res(res, parent_id);
    }

    MPI_Finalize();
}