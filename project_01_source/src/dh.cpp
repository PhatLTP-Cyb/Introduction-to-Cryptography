// NV5: Chọn phần tử sinh. Kiểm thử: tests/test_dh.cpp
#include "dh.h"
#include "common.h"

// Trả về phần tử sinh g của nhóm nhân mod p, với p là safe prime.
uint512_t find_generator(prng_ctx *ctx, const uint512_t &p)
{
    // p = 2q + 1 với q nguyên tố nên bậc của g chỉ có thể là 1, 2, q hoặc 2q.
    // g^2 != 1 loại bậc 1 và 2; g^q != 1 loại bậc q; còn lại bậc p - 1.
    const uint512_t one(1);
    const uint512_t two(2);
    const uint512_t q = (p - one) >> 1;

    for (;;)
    {
        uint512_t g = random_range(ctx, two, p - two);
        if (modular_exponentiation(g, two, p) != one &&
            modular_exponentiation(g, q, p) != one)
        {
            return g;
        }
    }
}