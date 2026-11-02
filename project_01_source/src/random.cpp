// NV4: Ngẫu nhiên và khóa riêng. Kiểm thử: tests/test_random.cpp
#include "dh.h"
#include "common.h"

// Khởi tạo PRNG bằng seed 32 byte từ hệ điều hành. Đây là chế độ mặc định.
void seed_from_os(prng_ctx *ctx)
{
    // TODO: uint8_t seed[32] = {0}; randombytes(...); prng_seed(...);
    not_implemented("seed_from_os");
}

// Khởi tạo PRNG bằng seed cố định, chỉ dùng khi kiểm thử.
void seed_from_text(prng_ctx *ctx, const char *text)
{
    // TODO: dùng cùng text phải ra cùng dãy số.
    not_implemented("seed_from_text");
}

// Trả về số ngẫu nhiên trong [0, 2^bits).
uint512_t random_bits(prng_ctx *ctx, size_t bits)
{
    // TODO: ghép các prng_next_u64, xóa các bit thừa ở đầu.
    not_implemented("random_bits");
}

// Trả về số ngẫu nhiên trong [low, high].
uint512_t random_range(prng_ctx *ctx, const uint512_t &low, const uint512_t &high)
{
    // TODO: sinh trong [0, high - low], ngoài khoảng thì sinh lại. Không dùng phép mod.
    not_implemented("random_range");
}

// Trả về khóa riêng trong [2, p - 2].
uint512_t generate_private_key(prng_ctx *ctx, const uint512_t &p)
{
    not_implemented("generate_private_key");
}
