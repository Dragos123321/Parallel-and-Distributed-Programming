#pragma once

#include <functional>
#include <vector>
#include <iostream>

class Polynomial
{
public:
    Polynomial();
    Polynomial(const std::vector<int>& coefficients, std::function<Polynomial(const Polynomial& lhs, const Polynomial& rhs)> multiply);
    Polynomial(std::vector<int>&& coefficients, std::function<Polynomial(const Polynomial& lhs, const Polynomial& rhs)> multiply);
    Polynomial(size_t degree, std::function<Polynomial(const Polynomial& lhs, const Polynomial& rhs)> multiply);

    Polynomial subPolynomial(size_t start, size_t end) const;
    size_t degree() const;

    void multiply(std::function<Polynomial(const Polynomial& lhs, const Polynomial& rhs)> multiply);

    friend Polynomial operator+(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial operator-(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial operator*(const Polynomial& lhs, const Polynomial& rhs);

    friend std::ostream& operator<<(std::ostream& stream, const Polynomial& polynomial);
    Polynomial shift(int shift);

    friend Polynomial simple_sequential(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial karatsuba_sequential(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial simple_parallel(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial karatsuba_parallel(const Polynomial& lhs, const Polynomial& rhs);
    friend Polynomial karatsuba_parallel_util(const Polynomial& lhs, const Polynomial& rhs, int depth);

    void coefficient(size_t index, int value);

    const int& operator[](size_t index) const;
private:
    void generate_random_coefficients();

    size_t m_degree;
    std::vector<int> m_coefficients;
    std::function<Polynomial(const Polynomial& lhs, const Polynomial& rhs)> m_multiply;
};

