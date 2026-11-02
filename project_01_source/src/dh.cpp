// NV5: Chọn phần tử sinh. Kiểm thử: tests/test_dh.cpp
#include "dh.h"
#include "common.h"

// Trả về phần tử sinh g của nhóm nhân mod p, với p là safe prime.
uint512_t find_generator(prng_ctx *ctx, const uint512_t &p)
{
    // TODO: đặt q = (p - 1) / 2, chọn g ngẫu nhiên trong [2, p - 2],
    // nhận g khi g^2 != 1 và g^q != 1 (mod p).
    not_implemented("find_generator");
}
