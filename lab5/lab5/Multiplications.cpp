#include "Multiplications.h"

#include <algorithm>
#include <iostream>
#include <future>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

Polynomial simple_sequential(const Polynomial& lhs, const Polynomial& rhs)
{
    Polynomial result;

    auto degree = lhs.m_degree + rhs.m_degree;

    result.m_multiply = lhs.m_multiply;
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

    result.m_multiply = lhs.m_multiply;
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

Polynomial karatsuba_parallel(const Polynomial& lhs, const Polynomial& rhs)
{
    if (lhs.m_degree < 2 || rhs.m_degree < 2) {
        return simple_sequential(lhs, rhs);
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

    std::thread thread1([&lhsLow, &rhsLow](std::promise<Polynomial>&& promise) {
        promise.set_value(karatsuba_parallel(lhsLow, rhsLow));
    }, std::move(promise1));

    std::thread thread2([&lhsLow, &lhsHigh, &rhsLow, &rhsHigh](std::promise<Polynomial>&& promise) {
        promise.set_value(karatsuba_parallel(lhsLow + lhsHigh, rhsLow + rhsHigh));
    }, std::move(promise2));

    std::thread thread3([&lhsHigh, &rhsHigh](std::promise<Polynomial>&& promise) {
        promise.set_value(karatsuba_parallel(lhsHigh, rhsHigh));
    }, std::move(promise3));

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