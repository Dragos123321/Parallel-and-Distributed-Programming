#pragma once

#include <functional>
#include <vector>
#include <iostream>

class Polynomial
{
public:
    Polynomial();
    Polynomial(const std::vector<int>& coefficients);
    Polynomial(std::vector<int>&& coefficients);
    Polynomial(size_t degree);

    Polynomial subPolynomial(size_t start, size_t end) const;
    size_t degree() const;
    const std::vector<int> coefficients() const;

    friend Polynomial operator+(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial operator-(const Polynomial& lhs, const Polynomial& rhs);
    friend bool operator==(const Polynomial& lhs, const Polynomial& rhs);

    friend std::ostream& operator<<(std::ostream& stream, const Polynomial& polynomial);
    Polynomial shift(int shift);

    friend Polynomial simple_sequential(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial karatsuba_sequential(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial simple_parallel(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial karatsuba_parallel(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial karatsuba_parallel_util(const Polynomial& lhs, const Polynomial& rhs, int depth);
    friend Polynomial karatsuba_mpi(const Polynomial& poly1, const Polynomial& poly2, int me, int nrProcs);
    friend void karatsuba_recv_polys(Polynomial& poly1, Polynomial& poly2, int& parent_id, int me);

    friend void worker_simple_mpi(int me);
    friend Polynomial master_simple_mpi(const Polynomial& poly1, const Polynomial& poly2, int nrNodes);

    void coefficient(size_t index, int value);

    const int& operator[](size_t index) const;
private:
    void generate_random_coefficients();

    size_t m_degree;
    std::vector<int> m_coefficients;
};

