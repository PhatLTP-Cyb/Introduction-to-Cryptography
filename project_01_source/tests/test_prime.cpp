// Kiểm thử NV3. Tham số: số bit của safe prime cần sinh, mặc định 128.
// In dòng "p=<hex>" để tests/run_tests.py kiểm tra lại độc lập bằng Python.
#include "dh.h"
#include "test_util.h"

#include <chrono>
#include <cstdlib>
#include <iostream>

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
