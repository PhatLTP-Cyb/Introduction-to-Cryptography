// NV3: Kiểm tra nguyên tố và sinh safe prime. Kiểm thử: tests/test_prime.cpp
#include "dh.h"

#include <stdexcept>
#include <vector>

namespace
{
constexpr uint64_t SMALL_PRIME_LIMIT = 20000;
constexpr size_t MIN_SAFE_PRIME_BITS = 16;
constexpr int SAFE_PRIME_ROUNDS = 20;

// Sinh danh sách 2262 số nguyên tố nhỏ hơn SMALL_PRIME_LIMIT bằng sàng Eratosthenes.
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
// Lý do: kiểm tra Miller-Rabin trên số lẻ n cần phân tích n - 1 = 2^r * m để tính lũy thừa khởi đầu
// b^m (mod n) và sau đó bình phương liên tiếp tối đa r - 1 lần.
DecomposedExponent decompose_even(const uint512_t &even_value)
{
    if (even_value.is_zero())
    {
        // Lý do: số 0 không có bit 1 nào để dừng vòng đếm, sẽ lặp vô hạn.
        throw std::invalid_argument("decompose_even: even_value khong duoc bang 0");
    }
    size_t r = 0;
    while (!even_value.bit(r))
    {
        ++r;
    }
    return DecomposedExponent{r, even_value >> r};
}

// Tiếp tục kiểm tra Miller-Rabin từ giá trị lũy thừa đầu tiên x = base^d mod n.
// Tách hàm để vòng cơ số 2 dùng lũy thừa riêng cho cơ số 2.
bool miller_rabin_from_first_power(const uint512_t &first_power, const DecomposedExponent &decomp, const uint512_t &n)
{
    const uint512_t n_minus_1 = n - uint512_t(1);
    uint512_t x = first_power;
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

// Thực hiện một vòng kiểm tra Miller-Rabin với cơ số b cho trước trên số lẻ n.
// Cơ số b được chọn trong khoảng [2, n - 2] vì:
// - b = 1 hoặc b = n - 1 luôn cho b^2 = 1 (mod n) tầm thường, không có tác dụng kiểm tra.
// - Nếu gcd(b, n) > 1 thì lũy thừa mod n không thể bằng 1 hoặc n - 1, thuật toán tự động phát hiện hợp số.
bool miller_rabin_test(const uint512_t &base, const DecomposedExponent &decomp, const uint512_t &n)
{
    return miller_rabin_from_first_power(modular_exponentiation(base, decomp.odd_factor, n), decomp, n);
}

// Phân tích n - 1 và thực hiện một vòng kiểm tra Miller-Rabin với cơ số 2.
// Lý do: hầu hết hợp số lẻ bị loại ngay ở cơ số 2, giúp lọc nhanh trước khi chạy các vòng ngẫu nhiên.
bool passes_base_two_round(const uint512_t &n)
{
    const uint512_t n_minus_1 = n - uint512_t(1);
    const DecomposedExponent decomp = decompose_even(n_minus_1);
    return miller_rabin_from_first_power(modular_exponentiation_base_two(decomp.odd_factor, n), decomp, n);
}

// Sinh số lẻ ngẫu nhiên q có đúng q_bits bit (bit cao nhất và bit thấp nhất đều bằng 1).
uint512_t generate_candidate_q(prng_ctx *ctx, size_t q_bits)
{
    uint512_t q = random_bits(ctx, q_bits);
    q.set_bit(q_bits - 1); // Đảm bảo bit cao nhất bằng 1 để q có đúng q_bits bit
    q.set_bit(0);          // Đảm bảo q là số lẻ
    return q;
}

// Sàng kết hợp cho cả q và p = 2q + 1 với các số nguyên tố nhỏ s < 20000.
// Lý do:
// - q chia hết cho s khi rem = q mod s == 0 (loại khi q != s để không loại nhầm bản thân số nguyên tố s).
// - p = 2q + 1 chia hết cho s khi 2q + 1 = 0 (mod s), tức 2q = -1 = s - 1 (mod s).
//   Với mọi số nguyên tố lẻ s, vì gcd(2, s) = 1 nên chia 2 hai vế ta được q = (s - 1) / 2 (mod s).
//   (loại khi q != (s - 1) / 2 để không loại nhầm trường hợp p chính là số nguyên tố s).
//   (Với s = 2, q lẻ nên rem = 1, còn (2 - 1)/2 = 0, điều kiện không bị kích hoạt nhầm).
// Nhờ đó, chỉ một lần tính mod_u64 trên q là kiểm tra được ước nguyên tố nhỏ cho cả q và p
// mà không cần tính riêng cho p, giúp loại bỏ sớm ứng viên hợp số.
bool has_small_factor_for_safe_prime(const uint512_t &q)
{
    for (uint64_t s : small_primes())
    {
        const uint64_t remainder = q.mod_u64(s);
        const bool q_divisible_by_s = (remainder == 0 && q != uint512_t(s));
        const bool p_divisible_by_s = (remainder == (s - 1) / 2 && q != uint512_t((s - 1) / 2));
        if (q_divisible_by_s || p_divisible_by_s)
        {
            return true;
        }
    }
    return false;
}

// Tính 2^((p - 1) / 2) mod p.
uint512_t two_to_half_exponent_mod(const uint512_t &p)
{
    const uint512_t half_exp = (p - uint512_t(1)) >> 1;
    return modular_exponentiation_base_two(half_exp, p);
}

// Kiểm tra vòng Miller-Rabin cơ số 2 đồng thời xác nhận điều kiện định lý Pocklington cho p = 2q + 1.
// Khi q lẻ, phân tích p - 1 = 2^1 * q (r = 1), do đó vòng Miller-Rabin cơ số 2 tính x = 2^q mod p
// và chỉ cần kiểm tra x == 1 hoặc x == p - 1. Khi đó, điều kiện Fermat 2^(p-1) == 1 (mod p) được suy ra
// trực tiếp từ x^2 == 1 (mod p) mà không cần tính thêm một phép lũy thừa modulo 512-bit riêng biệt.
// Điều kiện gcd(3, p) == 1 tương đương p mod 3 != 0.
// Lưu ý: Chứng nhận Pocklington này CÓ ĐIỀU KIỆN dựa trên giả định q là số nguyên tố (được Miller-Rabin xác nhận).
// Tiền điều kiện: p = 2q + 1 với q lẻ (p ≡ 3 mod 4); hàm không dùng được cho p tùy ý.
bool passes_base_two_and_certifies(const uint512_t &p)
{
    if (p.mod_u64(3) == 0)
    {
        return false;
    }
    const uint512_t one(1);
    const uint512_t p_minus_one = p - one;
    const uint512_t x = two_to_half_exponent_mod(p);
    return (x == one || x == p_minus_one) && mul_mod(x, x, p) == one;
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

    // Với n >= 5 lẻ: loại nhanh các hợp số chia hết cho các số nguyên tố nhỏ < 20000
    if (has_small_factor(n))
    {
        return false;
    }

    // Phân tích n - 1 = 2^r * m với m lẻ
    const uint512_t n_minus_1 = n - uint512_t(1);
    const DecomposedExponent decomp = decompose_even(n_minus_1);

    // Mọi vòng kiểm tra Miller-Rabin thuần đều chọn ngẫu nhiên cơ số b trong khoảng đóng [2, n - 2]
    const uint512_t max_base = n - two;
    for (int round = 0; round < rounds; ++round)
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
    // Ràng buộc kích thước:
    // - bitsize tối thiểu là MIN_SAFE_PRIME_BITS (16) với q >= 2^14 = 16384. Hàm sàng has_small_factor_for_safe_prime
    //   đã xử lý không loại nhầm khi q hoặc p trùng số nguyên tố nhỏ trong bảng (s < 20000).
    // - bitsize tối đa là BIGINT_BITS vì p = 2q + 1 phải vừa trong độ rộng cố định của uint512_t.
    if (bitsize < MIN_SAFE_PRIME_BITS || bitsize > BIGINT_BITS)
    {
        throw std::invalid_argument("generate_safe_prime: bitsize phai trong [16, BIGINT_BITS]");
    }

    const uint512_t one(1);
    for (;;)
    {
        const uint512_t q = generate_candidate_q(ctx, bitsize - 1);
        const uint512_t p = (q << 1) + one;

        if (has_small_factor_for_safe_prime(q))
        {
            continue;
        }

        if (!passes_base_two_round(q) || !passes_base_two_and_certifies(p))
        {
            continue;
        }

        if (!is_probable_prime(ctx, q, SAFE_PRIME_ROUNDS) || !is_probable_prime(ctx, p, SAFE_PRIME_ROUNDS))
        {
            continue;
        }

        return p;
    }
}
