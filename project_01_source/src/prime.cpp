// NV3: Kiểm tra nguyên tố và sinh safe prime. Kiểm thử: tests/test_prime.cpp
#include "dh.h"
#include "common.h"

#include <stdexcept>
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

// Cấu trúc lưu kết quả phân tích n - 1 = 2^r * m với m lẻ.
struct DecomposedExponent
{
    size_t trailing_zeros; // r: số bit 0 ở cuối (số mũ của 2)
    uint512_t odd_factor;  // m: phần lẻ còn lại sau khi chia hết cho 2^r
};

// Phân tích số chẵn value = 2^r * m với m lẻ bằng cách đếm các bit 0 liên tiếp từ bit thấp nhất.
// Lý do: theo định lý Fermat nhỏ, nếu n nguyên tố thì b^(n - 1) = b^(2^r * m) = 1 (mod n).
// Trong một trường hữu hạn F_p, nghiệm duy nhất của x^2 = 1 là x = 1 hoặc x = -1 (tức n - 1).
// Do đó dãy bình phương liên tiếp từ b^m đến b^(2^(r-1)*m) chỉ có thể bằng 1 nếu bắt đầu bằng 1
// hoặc giá trị ngay trước khi đạt 1 là n - 1.
DecomposedExponent decompose_even(const uint512_t &even_value)
{
    size_t r = 0;
    while (!even_value.bit(r))
    {
        ++r;
    }
    return DecomposedExponent{r, even_value >> r};
}

// Thực hiện một vòng kiểm tra Miller-Rabin với cơ số b cho trước trên số lẻ n.
// Cơ số b được chọn trong khoảng [2, n - 2] vì:
// - b = 1 hoặc b = n - 1 luôn cho b^2 = 1 (mod n) tầm thường, không có tác dụng kiểm tra.
// - Nếu gcd(b, n) > 1 thì lũy thừa mod n không thể bằng 1 hoặc n - 1, thuật toán tự động phát hiện hợp số.
bool miller_rabin_test(const uint512_t &base, const DecomposedExponent &decomp, const uint512_t &n)
{
    const uint512_t n_minus_1 = n - uint512_t(1);
    uint512_t x = modular_exponentiation(base, decomp.odd_factor, n);
    if (x == uint512_t(1) || x == n_minus_1)
    {
        return true;
    }

    for (size_t step = 1; step < decomp.trailing_zeros; ++step)
    {
        x = mul_mod(x, x, n);
        if (x == n_minus_1)
        {
            return true;
        }
    }

    return false;
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
    if (rounds < 1)
    {
        throw std::invalid_argument("is_probable_prime: so vong kiem tra phai >= 1");
    }

    const uint512_t two(2);
    const uint512_t three(3);
    if (n < two)
    {
        return false;
    }
    if (n == two || n == three)
    {
        return true;
    }
    if (!n.is_odd())
    {
        return false;
    }

    // Với n >= 5 lẻ: loại nhanh các hợp số chia hết cho các số nguyên tố nhỏ < 2000
    if (has_small_factor(n))
    {
        return false;
    }

    // Phân tích n - 1 = 2^r * m với m lẻ
    const uint512_t n_minus_1 = n - uint512_t(1);
    const DecomposedExponent decomp = decompose_even(n_minus_1);

    // Vòng đầu tiên dùng cơ số b = 2 (lọc nhanh các số không vượt qua phép thử cơ sở 2)
    if (!miller_rabin_test(two, decomp, n))
    {
        return false;
    }

    // Các vòng sau chọn ngẫu nhiên b trong khoảng đóng [2, n - 2]
    const uint512_t max_base = n - two;
    for (int round = 1; round < rounds; ++round)
    {
        const uint512_t random_base = random_range(ctx, two, max_base);
        if (!miller_rabin_test(random_base, decomp, n))
        {
            return false;
        }
    }

    return true;
}

// Trả về p đúng bitsize bit sao cho p và (p - 1) / 2 đều nguyên tố.
uint512_t generate_safe_prime(prng_ctx *ctx, size_t bitsize)
{
    // TODO: chọn q lẻ đúng bitsize - 1 bit, đặt p = 2q + 1.
    // Loại nhanh bằng has_small_factor trên q và p, rồi mới Miller-Rabin.
    not_implemented("generate_safe_prime");
}
