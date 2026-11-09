// NV3: Kiểm tra nguyên tố và sinh safe prime. Kiểm thử: tests/test_prime.cpp
#include "dh.h"
#include "common.h"

#include <vector>

namespace
{
constexpr uint64_t SMALL_PRIME_LIMIT = 2000;

// Sinh danh sách 303 số nguyên tố nhỏ hơn SMALL_PRIME_LIMIT bằng sàng Eratosthenes.
std::vector<uint64_t> build_small_primes()
{
    std::vector<bool> is_composite(SMALL_PRIME_LIMIT, false);
    std::vector<uint64_t> primes;
    for (uint64_t candidate = 2; candidate < SMALL_PRIME_LIMIT; ++candidate)
    {
        if (!is_composite[candidate])
        {
            primes.push_back(candidate);
            for (uint64_t multiple = candidate * candidate; multiple < SMALL_PRIME_LIMIT; multiple += candidate)
            {
                is_composite[multiple] = true;
            }
        }
    }
    return primes;
}

// Giữ bảng nguyên tố nhỏ dạng static để chỉ sàng một lần duy nhất trong suốt chương trình.
const std::vector<uint64_t> &small_primes()
{
    static const std::vector<uint64_t> primes = build_small_primes();
    return primes;
}
} // namespace

// Trả về true nếu n chia hết cho một số nguyên tố nhỏ khác chính n.
bool has_small_factor(const uint512_t &n)
{
    // Loại sớm các hợp số có ước nguyên tố nhỏ trước khi chạy Miller-Rabin tốn kém.
    // So sánh n != uint512_t(s) để bản thân số nguyên tố nhỏ s không bị kết luận nhầm là hợp số,
    // đồng thời n = 0 chia hết cho 2 và 0 != 2 nên trả về true.
    for (uint64_t s : small_primes())
    {
        if (n.mod_u64(s) == 0 && n != uint512_t(s))
        {
            return true;
        }
    }
    return false;
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
