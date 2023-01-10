#pragma once

#include "Polynomial.h"

Polynomial simple_sequential(const Polynomial& lhs, const Polynomial& rhs);
Polynomial karatsuba_sequential(const Polynomial& lhs, const Polynomial& rhs);
Polynomial simple_parallel(const Polynomial& lhs, const Polynomial& rhs);
Polynomial karatsuba_parallel(const Polynomial& lhs, const Polynomial& rhs);
Polynomial karatsuba_parallel_util(const Polynomial& lhs, const Polynomial& rhs, int depth);

void worker_simple_mpi(int me);
Polynomial master_simple_mpi(const Polynomial& poly1, const Polynomial& poly2, int nrNodes);

Polynomial karatsuba_mpi(const Polynomial& poly1, const Polynomial& poly2, int me, int nrProcs);

void send_karatsuba(const Polynomial& poly1, const Polynomial& poly2, int to);
Polynomial recv_karatsuba(int from);
void karatsuba_send_res(const Polynomial& poly, int parent_id);
void karatsuba_recv_polys(Polynomial& poly1, Polynomial& poly2, int& parent_id, int me);