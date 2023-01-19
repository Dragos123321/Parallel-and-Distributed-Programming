#define _CRT_SECURE_NO_WARNINGS
#include "Multiplications.h"

#include <algorithm>
#include <iostream>
#include <future>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <mpi.h>

const int MAX_DEPTH = 4;

Polynomial simple_sequential(const Polynomial& lhs, const Polynomial& rhs)
{
    Polynomial result;

    auto degree = lhs.m_degree + rhs.m_degree;

    result.m_degree = degree;
    result.m_coefficients.resize(degree + 1, 0);

    for (size_t i = 0; i <= lhs.m_degree; ++i) {
        for (size_t j = 0; j <= rhs.m_degree; ++j) {
            result.m_coefficients[i + j] += lhs.m_coefficients[i] * rhs.m_coefficients[j];
        }
    }

    return result;
}

Polynomial karatsuba_sequential(const Polynomial& lhs, const Polynomial& rhs)
{
    if (lhs.m_degree < 2 || rhs.m_degree < 2) {
        return simple_sequential(lhs, rhs);
    }

    auto mid = std::max(lhs.m_degree, rhs.m_degree) / 2;

    auto lhsLow = lhs.subPolynomial(0, mid);
    auto lhsHigh = lhs.subPolynomial(mid, lhs.m_degree + 1);
    auto rhsLow = rhs.subPolynomial(0, mid);
    auto rhsHigh = rhs.subPolynomial(mid, rhs.m_degree + 1);

    auto z1 = karatsuba_sequential(lhsLow, rhsLow);
    auto z2 = karatsuba_sequential(lhsLow + lhsHigh, rhsLow + rhsHigh);
    auto z3 = karatsuba_sequential(lhsHigh, rhsHigh);

    auto r1 = z3.shift(2 * mid);
    auto r2 = ((z2 - z3) - z1).shift(mid);
    return (r1 + r2) + z1;
}

Polynomial simple_parallel(const Polynomial& lhs, const Polynomial& rhs)
{
    boost::asio::thread_pool threadPool(std::thread::hardware_concurrency());

    Polynomial result;

    auto degree = lhs.m_degree + rhs.m_degree;

    result.m_degree = degree;
    result.m_coefficients.resize(degree + 1, 0);

    for (size_t i = 0; i <= lhs.m_degree; ++i) {
        boost::asio::post(threadPool, [&lhs, &rhs, &result, i]() {
            for (size_t j = 0; j <= rhs.degree(); ++j) {
                result.coefficient(i + j, result[i + j] + lhs[i] * rhs[j]);
            }
        });
    }

    threadPool.join();

    return result;
}

Polynomial karatsuba_parallel(const Polynomial& lhs, const Polynomial& rhs) {
    return karatsuba_parallel_util(lhs, rhs, 0);
}

Polynomial karatsuba_parallel_util(const Polynomial& lhs, const Polynomial& rhs, int depth)
{
    if (depth >= MAX_DEPTH) {
        return karatsuba_sequential(lhs, rhs);
    }

    if (lhs.m_degree < 2 || rhs.m_degree < 2) {
        return karatsuba_sequential(lhs, rhs);
    }

    auto mid = std::max(lhs.m_degree, rhs.m_degree) / 2;

    auto lhsLow = lhs.subPolynomial(0, mid);
    auto lhsHigh = lhs.subPolynomial(mid, lhs.m_degree + 1);
    auto rhsLow = rhs.subPolynomial(0, mid);
    auto rhsHigh = rhs.subPolynomial(mid, rhs.m_degree + 1);

    std::promise<Polynomial> promise1;
    std::promise<Polynomial> promise2;
    std::promise<Polynomial> promise3;

    auto future1 = promise1.get_future();
    auto future2 = promise2.get_future();
    auto future3 = promise3.get_future();

    std::thread thread1([&lhsLow, &rhsLow](std::promise<Polynomial>&& promise, int depth) {
        promise.set_value(karatsuba_parallel_util(lhsLow, rhsLow, depth + 1));
    }, std::move(promise1), depth);

    std::thread thread2([&lhsLow, &lhsHigh, &rhsLow, &rhsHigh](std::promise<Polynomial>&& promise, int depth) {
        promise.set_value(karatsuba_parallel_util(lhsLow + lhsHigh, rhsLow + rhsHigh, depth + 1));
    }, std::move(promise2), depth);

    std::thread thread3([&lhsHigh, &rhsHigh](std::promise<Polynomial>&& promise, int depth) {
        promise.set_value(karatsuba_parallel_util(lhsHigh, rhsHigh, depth + 1));
    }, std::move(promise3), depth);

    thread1.join();
    thread2.join();
    thread3.join();

    auto z1 = future1.get();
    auto z2 = future2.get();
    auto z3 = future3.get();

    auto r1 = z3.shift(2 * mid);
    auto r2 = ((z2 - z3) - z1).shift(mid);
    return (r1 + r2) + z1;
}

void worker_simple_mpi(int me)
{
    MPI_Status status;
    int poly1_size;
    int poly2_size;
    std::vector<int> coeff_poly1;
    std::vector<int> coeff_poly2;

    MPI_Recv(&poly1_size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&poly2_size, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

    coeff_poly1.resize(poly1_size, 0);
    coeff_poly2.resize(poly2_size + 1, 0);

    MPI_Recv(coeff_poly1.data(), poly1_size, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
    MPI_Recv(coeff_poly2.data(), poly2_size + 1, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);

    Polynomial result;

    auto degree = poly1_size + poly2_size - 1;

    result.m_degree = degree;
    result.m_coefficients.resize(degree + 1, 0);

    for (int i = 0; i < poly1_size; ++i) {
        for (int j = 0; j <= poly2_size; ++j) {
            result.m_coefficients[i + j] += coeff_poly1[i] * coeff_poly2[j];
        }
    }

    MPI_Ssend(&degree, 1, MPI_INT, 0, 5, MPI_COMM_WORLD);
    MPI_Ssend(result.m_coefficients.data(), degree + 1, MPI_INT, 0, 6, MPI_COMM_WORLD);
}

Polynomial master_simple_mpi(const Polynomial& poly1, const Polynomial& poly2, int nrNodes)
{
    int nrElems = (poly1.degree() + 1) / nrNodes;
    
    int poly1_size = static_cast<int>(poly1.degree());
    int poly2_size = static_cast<int>(poly2.degree());

    std::vector<int> coeffs1 = poly1.m_coefficients;

    for (int i = 1; i < nrNodes; ++i) {
        MPI_Ssend(&nrElems, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        MPI_Ssend(&poly2_size, 1, MPI_INT, i, 2, MPI_COMM_WORLD);

        MPI_Ssend(poly1.m_coefficients.data() + i * nrElems, nrElems, MPI_INT, i, 3, MPI_COMM_WORLD);
        MPI_Ssend(poly2.m_coefficients.data(), poly2_size + 1, MPI_INT, i, 4, MPI_COMM_WORLD);
    }

    auto degree = poly1_size + poly2_size;
    std::vector<int> res_coeffs(degree + 1, 0);

    for (int i = 0; i < nrElems; ++i) {
        for (int j = 0; j <= poly2_size; ++j) {
            res_coeffs[i + j] += poly1.m_coefficients[i] * poly2.m_coefficients[j];
        }
    }

    for (int i = nrElems * nrNodes; i <= poly1_size; ++i) {
        for (int j = 0; j <= poly2_size; ++j) {
            res_coeffs[i + j] += poly1.m_coefficients[i] * poly2.m_coefficients[j];
        }
    }

    for (int i = 1; i < nrNodes; ++i) {
        MPI_Status status;
        int size = 0;

        MPI_Recv(&size, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &status);

        std::vector<int> coeffs;
        coeffs.resize(size + 1, 0);

        MPI_Recv(coeffs.data(), size + 1, MPI_INT, i, 6, MPI_COMM_WORLD, &status);

        for (int k = 0; k < coeffs.size(); ++k) {
            res_coeffs[k + i * nrElems] += coeffs[k];
        }
    }

    return Polynomial(res_coeffs);
}

void send_karatsuba(const Polynomial& poly1, const Polynomial& poly2, int to)
{
    int poly1_degree = (int)poly1.degree();
    int poly2_degree = (int)poly2.degree();

    MPI_Ssend(&poly1_degree, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
    MPI_Ssend(poly1.coefficients().data(), poly1_degree + 1, MPI_INT, to, 2, MPI_COMM_WORLD);

    MPI_Ssend(&poly2_degree, 1, MPI_INT, to, 3, MPI_COMM_WORLD);
    MPI_Ssend(poly2.coefficients().data(), poly2_degree + 1, MPI_INT, to, 4, MPI_COMM_WORLD);
}

Polynomial recv_karatsuba(int from)
{
    int degree;
    MPI_Status status;

    MPI_Recv(&degree, 1, MPI_INT, from, 5, MPI_COMM_WORLD, &status);

    std::vector<int> coeffs(degree + 1, 0);

    MPI_Recv(coeffs.data(), degree + 1, MPI_INT, from, 6, MPI_COMM_WORLD, &status);

    return Polynomial(coeffs);
}

void karatsuba_send_res(const Polynomial& poly, int parent_id)
{
    int degree = poly.degree();

    MPI_Ssend(&degree, 1, MPI_INT, parent_id, 5, MPI_COMM_WORLD);
    MPI_Ssend(poly.coefficients().data(), degree + 1, MPI_INT, parent_id, 6, MPI_COMM_WORLD);
}

void karatsuba_recv_polys(Polynomial& poly1, Polynomial& poly2, int& parent_id, int me) 
{
    MPI_Status status;
    int poly1_degree;
    int poly2_degree;

    int me_min_1 = me - 1;
    parent_id = me_min_1 / 3;

    MPI_Recv(&poly1_degree, 1, MPI_INT, parent_id, 1, MPI_COMM_WORLD, &status);

    std::vector<int> coeff1(poly1_degree + 1, 0);

    MPI_Recv(coeff1.data(), poly1_degree + 1, MPI_INT, parent_id, 2, MPI_COMM_WORLD, &status);

    poly1 = Polynomial(coeff1);

    MPI_Recv(&poly2_degree, 1, MPI_INT, parent_id, 3, MPI_COMM_WORLD, &status);

    std::vector<int> coeff2(poly2_degree + 1, 0);

    MPI_Recv(coeff2.data(), poly2_degree + 1, MPI_INT, parent_id, 4, MPI_COMM_WORLD, &status);

    poly2 = Polynomial(coeff2);
}

Polynomial karatsuba_mpi(const Polynomial& poly1, const Polynomial& poly2, int me, int nrProcs)
{
    if (poly1.degree() <= 2 && poly2.degree() <= 2) {
        return simple_sequential(poly1, poly2);
    }

    auto mid = std::max(poly1.m_degree, poly2.m_degree) / 2;

    auto lhsLow = poly1.subPolynomial(0, mid);
    auto lhsHigh = poly1.subPolynomial(mid, poly1.m_degree + 1);
    auto rhsLow = poly2.subPolynomial(0, mid);
    auto rhsHigh = poly2.subPolynomial(mid, poly2.m_degree + 1);

    Polynomial z1;
    Polynomial z2;
    Polynomial z3;

    if (me * 3 + 1 < nrProcs)
        send_karatsuba(lhsLow, rhsLow, me * 3 + 1);
    else
        z1 = karatsuba_sequential(lhsLow, rhsLow);

    if (me * 3 + 2 < nrProcs)
        send_karatsuba(lhsLow + lhsHigh, rhsLow + rhsHigh, me * 3 + 2);
    else 
        z2 = karatsuba_sequential(lhsLow + lhsHigh, rhsLow + rhsHigh);

    if (me * 3 + 3 < nrProcs)
        send_karatsuba(lhsHigh, rhsHigh, me * 3 + 3);
    else 
        z3 = karatsuba_sequential(lhsHigh, rhsHigh);

    if (me * 3 + 1 < nrProcs)
        z1 = recv_karatsuba(me * 3 + 1);

    if (me * 3 + 2 < nrProcs)
        z2 = recv_karatsuba(me * 3 + 2);

    if (me * 3 + 3 < nrProcs)
        z3 = recv_karatsuba(me * 3 + 3);

    auto r1 = z3.shift(2 * mid);
    auto r2 = ((z2 - z3) - z1).shift(mid);
    return (r1 + r2) + z1;
}