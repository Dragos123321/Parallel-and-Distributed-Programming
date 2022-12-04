#pragma once

#include "Polynomial.h"

Polynomial simple_sequential(const Polynomial& lhs, const Polynomial& rhs);
Polynomial karatsuba_sequential(const Polynomial& lhs, const Polynomial& rhs);
Polynomial simple_parallel(const Polynomial& lhs, const Polynomial& rhs);
Polynomial karatsuba_parallel(const Polynomial& lhs, const Polynomial& rhs);