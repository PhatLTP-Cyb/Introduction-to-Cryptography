// NV4: Ngẫu nhiên và khóa riêng. Kiểm thử: tests/test_random.cpp
#include "dh.h"
#include "common.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

// Lỗi do tham số sai (khác với hàm chưa cài): in thông báo rồi dừng.
[[noreturn]] static void invalid_argument(const char *msg)
{
    std::fprintf(stderr, "LOI THAM SO: %s\n", msg);
    std::abort();
}

// Khởi tạo PRNG bằng seed 32 byte từ hệ điều hành. Đây là chế độ mặc định.
// Windows: BCryptGenRandom, Linux: getrandom (xem lib/fips202/prng.cpp).
void seed_from_os(prng_ctx *ctx)
{
    uint8_t seed[32] = {0};
    randombytes(seed, sizeof(seed));
    prng_seed(ctx, seed, sizeof(seed));
    // Xóa seed khỏi bộ nhớ sau khi dùng. volatile để trình biên dịch không bỏ vòng lặp.
    volatile uint8_t *p = seed;
    for (size_t i = 0; i < sizeof(seed); i++)
        p[i] = 0;
}

// Khởi tạo PRNG bằng seed cố định, chỉ dùng khi kiểm thử.
// Cùng text thì SHAKE128 hấp thụ cùng dữ liệu nên ra cùng dãy số.
void seed_from_text(prng_ctx *ctx, const char *text)
{
    prng_seed(ctx, reinterpret_cast<const uint8_t *>(text), std::strlen(text));
}

// Trả về số ngẫu nhiên trong [0, 2^bits).
// Lấp đủ các limb cần dùng bằng prng_next_u64, rồi xóa các bit thừa ở limb cao nhất.
// Mỗi bit độc lập và đều nên phân bố đều trên [0, 2^bits).
uint512_t random_bits(prng_ctx *ctx, size_t bits)
{
    uint512_t r;
    if (bits == 0)
        return r;
    if (bits > BIGINT_BITS)
        bits = BIGINT_BITS;

    size_t full = bits / 64; // số limb dùng trọn 64 bit
    size_t rest = bits % 64; // số bit dùng ở limb kế tiếp
    for (size_t i = 0; i < full; i++)
        r.digits[i] = prng_next_u64(ctx);
    if (rest != 0)
        r.digits[full] = prng_next_u64(ctx) & ((UINT64_C(1) << rest) - 1);
    return r;
}

// Trả về số ngẫu nhiên trong [low, high].
// Lấy mẫu loại bỏ: sinh x trong [0, 2^k) với k = bit_length(high - low), nhận nếu
// x <= high - low, ngược lại sinh lại. Vì 2^(k-1) <= high - low nên xác suất nhận >= 1/2,
// trung bình không quá 2 lần sinh. Không dùng phép mod nên không lệch phân bố.
uint512_t random_range(prng_ctx *ctx, const uint512_t &low, const uint512_t &high)
{
    if (high < low)
        invalid_argument("random_range: low > high");

    uint512_t span = high - low;
    if (span.is_zero())
        return low;

    size_t k = span.bit_length();
    uint512_t x;
    do
    {
        x = random_bits(ctx, k);
    } while (x > span);
    return low + x;
}

// Trả về khóa riêng trong [2, p - 2].
// Bỏ 0, 1 và p - 1 vì g^0 = g^(p-1) = 1 và g^1 = g làm lộ khóa.
uint512_t generate_private_key(prng_ctx *ctx, const uint512_t &p)
{
    // Cần p >= 5 thì khoảng [2, p - 2] mới không rỗng.
    if (p < uint512_t(5))
        invalid_argument("generate_private_key: p < 5");
    return random_range(ctx, uint512_t(2), p - uint512_t(2));
}
