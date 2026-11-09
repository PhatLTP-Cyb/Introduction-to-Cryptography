// Kiểm thử NV3. Tham số: số bit của safe prime cần sinh, mặc định 128.
// In dòng "p=<hex>" để tests/run_tests.py kiểm tra lại độc lập bằng Python.
#include "dh.h"
#include "test_util.h"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <set>

// Kiểm tra nguyên tố bằng chia thử độc lập để sinh tập đối chứng safe prime 16-bit
static bool is_prime_trial(uint64_t n)
{
    if (n < 2)
    {
        return false;
    }
    if (n == 2 || n == 3)
    {
        return true;
    }
    if (n % 2 == 0 || n % 3 == 0)
    {
        return false;
    }
    for (uint64_t d = 5; d * d <= n; d += 6)
    {
        if (n % d == 0 || n % (d + 2) == 0)
        {
            return false;
        }
    }
    return true;
}

// Tập hợp toàn bộ số nguyên tố an toàn 16-bit trong [32768, 65535] bằng phương pháp chia thử
static std::set<uint64_t> collect_expected_16bit_safe_primes()
{
    std::set<uint64_t> safe_primes;
    for (uint64_t p = 32768; p <= 65535; ++p)
    {
        if (p % 2 == 1 && is_prime_trial(p))
        {
            const uint64_t q = (p - 1) / 2;
            if (is_prime_trial(q))
            {
                safe_primes.insert(p);
            }
        }
    }
    return safe_primes;
}

int main(int argc, char **argv)
{
    size_t bits = argc > 1 ? std::strtoull(argv[1], nullptr, 10) : 128;

    prng_ctx ctx;
    seed_from_text(&ctx, "test-prime");

    // Số nguyên tố
    for (uint64_t n : {2ULL, 3ULL, 5ULL, 7ULL, 83ULL, 107ULL, 7919ULL})
        CHECK(is_probable_prime(&ctx, uint512_t(n)));
    CHECK(is_probable_prime(&ctx, uint512_t::from_hex("7fffffffffffffffffffffffffffffff")));

    // Hợp số, gồm số Carmichael và số giả nguyên tố cơ sở 2
    for (uint64_t n : {0ULL, 1ULL, 4ULL, 15ULL, 561ULL, 1729ULL, 2047ULL, 41041ULL, 3215031751ULL})
        CHECK(!is_probable_prime(&ctx, uint512_t(n)));

    CHECK(has_small_factor(uint512_t(1001)));
    CHECK(!has_small_factor(uint512_t(83)));
    CHECK(!has_small_factor(uint512_t::from_hex("7fffffffffffffffffffffffffffffff")));

    // Quét toàn bộ số nguyên tố an toàn 16-bit để đảm bảo thuật toán sinh không loại nhầm
    const std::set<uint64_t> expected_16bit_safe_primes = collect_expected_16bit_safe_primes();
    CHECK(expected_16bit_safe_primes.size() == 193);

    prng_ctx ctx_16bit;
    seed_from_text(&ctx_16bit, "test-prime-16bit");
    std::set<uint64_t> found_16bit_safe_primes;
    bool all_16bit_in_expected = true;
    for (int iter = 0; iter < 100000; ++iter)
    {
        uint512_t generated_p = generate_safe_prime(&ctx_16bit, 16);
        uint64_t p_val = generated_p.digits[0];
        if (expected_16bit_safe_primes.find(p_val) == expected_16bit_safe_primes.end())
        {
            all_16bit_in_expected = false;
        }
        found_16bit_safe_primes.insert(p_val);
        if (found_16bit_safe_primes.size() == 193)
        {
            break;
        }
    }
    CHECK(all_16bit_in_expected);
    CHECK(found_16bit_safe_primes.size() == 193);

    auto start = std::chrono::steady_clock::now();
    uint512_t p = generate_safe_prime(&ctx, bits);
    auto stop = std::chrono::steady_clock::now();
    uint512_t q = (p - uint512_t(1)) >> 1;
    CHECK(p.bit_length() == bits);
    CHECK(is_probable_prime(&ctx, p));
    CHECK(is_probable_prime(&ctx, q));

    std::cout << "p=" << p << "\n";
    std::cout << "Thoi gian sinh " << bits << " bit: "
              << std::chrono::duration<double>(stop - start).count() << " giay\n";
    return finish("test_prime");
}
