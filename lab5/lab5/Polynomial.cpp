#include "Polynomial.h"

#include "Multiplications.h"

#include <algorithm>
#include <chrono>
#include <random>

Polynomial::Polynomial() :
    m_degree{ 0 }
{
    m_multiply = simple_sequential;
}

Polynomial::Polynomial(const std::vector<int>& coefficients, std::function<Polynomial(const Polynomial& lhs, const Polynomial& rhs)> multiply) :
    m_coefficients{ coefficients },
    m_multiply{ multiply }
{
    m_degree = m_coefficients.size() - 1;
}

Polynomial::Polynomial(std::vector<int>&& coefficients, std::function<Polynomial(const Polynomial& lhs, const Polynomial& rhs)> multiply) :
    m_coefficients{ std::move(coefficients) },
    m_multiply{ multiply }
{
    m_degree = m_coefficients.size() - 1;
}

Polynomial::Polynomial(size_t degree, std::function<Polynomial(const Polynomial& lhs, const Polynomial& rhs)> multiply) :
    m_multiply{ multiply },
    m_degree{ degree }
{
    generate_random_coefficients();
}

Polynomial Polynomial::subPolynomial(size_t start, size_t end) const
{
    return Polynomial(std::vector<int>(m_coefficients.begin() + start, m_coefficients.begin() + end), m_multiply);
}

size_t Polynomial::degree() const
{
    return m_degree;
}

Polynomial operator+(const Polynomial& lhs, const Polynomial& rhs)
{
    Polynomial result;

    size_t max_degree = std::max(lhs.m_degree, rhs.m_degree);
    size_t min_degree = std::min(lhs.m_degree, rhs.m_degree);

    result.m_degree = max_degree;
    result.m_coefficients.reserve(max_degree);

    for (size_t i = 0; i <= min_degree; ++i) {
        result.m_coefficients.push_back(lhs.m_coefficients[i] + rhs.m_coefficients[i]);
    }

    for (size_t i = min_degree + 1; i <= lhs.m_degree; ++i) {
        result.m_coefficients.push_back(lhs.m_coefficients[i]);
    }

    for (size_t i = min_degree + 1; i <= rhs.m_degree; ++i) {
        result.m_coefficients.push_back(rhs.m_coefficients[i]);
    }

    result.m_multiply = lhs.m_multiply;

    return result;
}

Polynomial operator-(const Polynomial& lhs, const Polynomial& rhs)
{
    Polynomial result;

    size_t max_degree = std::max(lhs.m_degree, rhs.m_degree);
    size_t min_degree = std::min(lhs.m_degree, rhs.m_degree);

    result.m_degree = max_degree;

    result.m_coefficients.reserve(max_degree);

    for (size_t i = 0; i <= min_degree; ++i) {
        result.m_coefficients.push_back(lhs.m_coefficients[i] - rhs.m_coefficients[i]);
    }

    for (size_t i = min_degree + 1; i <= lhs.m_degree; ++i) {
        result.m_coefficients.push_back(lhs.m_coefficients[i]);
    }

    for (size_t i = min_degree + 1; i <= rhs.m_degree; ++i) {
        result.m_coefficients.push_back(-rhs.m_coefficients[i]);
    }

    result.m_multiply = lhs.m_multiply;

    return result;
}

Polynomial operator*(const Polynomial& lhs, const Polynomial& rhs)
{
    auto start = std::chrono::high_resolution_clock::now();

    Polynomial res = lhs.m_multiply(lhs, rhs);

    auto end = std::chrono::high_resolution_clock::now();

    auto time_per_cols = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << time_per_cols << " millis\n";

    return res;
}

std::ostream& operator<<(std::ostream& stream, const Polynomial& polynomial)
{
    for (size_t i = 0; i <= polynomial.m_degree; ++i) {
        if (i != 0) {
            if (polynomial.m_coefficients[i] < 0)
                stream << " - ";
            else
                stream << " + ";
        }

        stream << abs(polynomial.m_coefficients[i]) << " * X^" << i;
    }

    return stream;
}

void Polynomial::multiply(std::function<Polynomial(const Polynomial& lhs, const Polynomial& rhs)> multiply)
{
    m_multiply = multiply;
}

Polynomial Polynomial::shift(int shift)
{
    std::vector<int> newCoeffs(shift, 0);
    newCoeffs.insert(newCoeffs.end(), m_coefficients.begin(), m_coefficients.end());

    return Polynomial(newCoeffs, m_multiply);
}

void Polynomial::coefficient(size_t index, int value)
{
    m_coefficients[index] = value;
}

const int& Polynomial::operator[](size_t index) const
{
    return m_coefficients[index];
}

void Polynomial::generate_random_coefficients()
{
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distr(0, 10);

    for (size_t i = 0; i <= m_degree; ++i) {
        m_coefficients.push_back(distr(gen));
    }
}
