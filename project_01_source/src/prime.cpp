// NV3: Kiểm tra nguyên tố và sinh safe prime. Kiểm thử: tests/test_prime.cpp
#include "dh.h"
#include "common.h"

// Trả về true nếu n chia hết cho một số nguyên tố nhỏ khác chính n.
bool has_small_factor(const uint512_t &n)
{
    // TODO: dùng n.mod_u64(s) với các số nguyên tố s < 2000.
    not_implemented("has_small_factor");
}

// Miller-Rabin với cơ số ngẫu nhiên. Trả về false khi n chắc chắn là hợp số.
bool is_probable_prime(prng_ctx *ctx, const uint512_t &n, int rounds)
{
    // TODO: xử lý n < 5 và n chẵn trước.
    // Viết n - 1 = 2^r * m với m lẻ. Số n qua một vòng khi b^m = ±1,
    // hoặc b^(2^k * m) = -1 với 1 <= k < r. Không dùng primeTest trên slide.
    not_implemented("is_probable_prime");
}

// Trả về p đúng bitsize bit sao cho p và (p - 1) / 2 đều nguyên tố.
uint512_t generate_safe_prime(prng_ctx *ctx, size_t bitsize)
{
    // TODO: chọn q lẻ đúng bitsize - 1 bit, đặt p = 2q + 1.
    // Loại nhanh bằng has_small_factor trên q và p, rồi mới Miller-Rabin.
    not_implemented("generate_safe_prime");
}
