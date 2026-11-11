// Kiểm thử NV2. Tham số: tệp dữ liệu do tests/run_tests.py sinh, mỗi dòng "phép a b m kết_quả".
#include "dh.h"
#include "test_util.h"

#include <fstream>
#include <stdexcept>
#include <string>

namespace
{
// Mô-đun 0 phải làm hàm ném std::invalid_argument.
template <typename Op>
bool throws_on_zero(Op op)
{
    try
    {
        op();
    }
    catch (const std::invalid_argument &)
    {
        return true;
    }
    return false;
}
} // namespace

int main(int argc, char **argv)
{
    uint512_t zero;
    uint512_t one(1);
    uint512_t m23(23);

    // Ví dụ trong bài: 3^13 mod 23 = 9
    CHECK(modular_exponentiation(uint512_t(3), uint512_t(13), m23) == uint512_t(9));

    // Các trường hợp biên
    CHECK(modular_exponentiation(uint512_t(5), zero, m23) == one);
    CHECK(modular_exponentiation(uint512_t(5), uint512_t(3), one).is_zero());
    CHECK(modular_exponentiation(uint512_t(28), uint512_t(1), m23) == uint512_t(5));
    CHECK(reduce_mod(uint512_t(28), m23) == uint512_t(5));

    // Mô-đun 0: cả 4 hàm ném ngoại lệ
    CHECK(throws_on_zero([&] { reduce_mod(m23, zero); }));
    CHECK(throws_on_zero([&] { add_mod(one, one, zero); }));
    CHECK(throws_on_zero([&] { mul_mod(one, one, zero); }));
    CHECK(throws_on_zero([&] { modular_exponentiation(one, one, zero); }));

    // Mô-đun 2^BIGINT_BITS - 1: tổng và phép nhân đôi tràn khỏi BIGINT_BITS
    uint512_t max = zero - one;
    uint512_t max_minus_1 = max - one;
    CHECK(reduce_mod(max, max).is_zero());
    CHECK(add_mod(max_minus_1, max_minus_1, max) == max - uint512_t(2));
    CHECK(mul_mod(max_minus_1, max_minus_1, max) == one); // (-1)^2 = 1
    CHECK(modular_exponentiation(max_minus_1, max, max) == max_minus_1); // (-1)^lẻ = -1
    CHECK(modular_exponentiation(uint512_t(2), uint512_t(BIGINT_BITS), max) == one); // 2^BITS = 1

    // Mô-đun chẵn
    uint512_t top = one << (BIGINT_BITS - 1); // 2^(BITS-1), đủ BIGINT_BITS bit
    CHECK(modular_exponentiation(uint512_t(3), uint512_t(13), uint512_t(24)) == uint512_t(3));
    CHECK(mul_mod(top - one, top - one, top) == one);
    CHECK(mul_mod(one << (BIGINT_BITS - 2), uint512_t(2), top).is_zero());
    CHECK(add_mod(top - one, top - one, top) == top - uint512_t(2));
    CHECK(modular_exponentiation(uint512_t(2), uint512_t(BIGINT_BITS - 1), top).is_zero());
    CHECK(modular_exponentiation(uint512_t(7), uint512_t(5), uint512_t(2)) == one);

    // Mô-đun lẻ đi qua nhân Montgomery
    uint512_t top_plus_1 = top + one; // 2^(BITS-1) + 1, lẻ và đủ BIGINT_BITS bit
    CHECK(mul_mod(top, top, top_plus_1) == one);                         // (-1)^2 = 1
    CHECK(mul_mod(top, uint512_t(2), top_plus_1) == top - one);          // -2
    CHECK(mul_mod(max, max, top_plus_1) == uint512_t(9));                // đầu vào >= m: max = -3
    CHECK(mul_mod(top, zero, top_plus_1).is_zero());
    CHECK(mul_mod(top, one, top_plus_1) == top);
    CHECK(mul_mod(uint512_t(2), uint512_t(2), uint512_t(3)) == one);     // mô-đun lẻ nhỏ nhất
    CHECK(modular_exponentiation(top, max, top_plus_1) == top);          // (-1)^lẻ = -1
    CHECK(modular_exponentiation(zero, uint512_t(5), top_plus_1).is_zero());
    CHECK(modular_exponentiation(zero, zero, top_plus_1) == one);
    // Định lý Fermat nhỏ với số nguyên tố 2^127 - 1 và 2^512 - 569 (cần BIGINT_BITS >= 512)
    uint512_t p127 = uint512_t::from_hex("7fffffffffffffffffffffffffffffff");
    uint512_t p512 = uint512_t::from_hex("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffdc7");
    CHECK(modular_exponentiation(uint512_t(3), p127 - one, p127) == one);
    CHECK(modular_exponentiation(uint512_t(3), p512 - one, p512) == one);
    CHECK(mul_mod(p512 - one, p512 - one, p512) == one);

    // Kiểm thử modular_exponentiation_base_two (2^exp mod mod)
    // (a) Mô-đun lẻ 23, 97, max với exp trong 0..200: so sánh với modular_exponentiation(2, exp, mod)
    uint512_t m97(97);
    for (uint64_t e = 0; e <= 200; ++e)
    {
        uint512_t exp(e);
        CHECK(modular_exponentiation_base_two(exp, m23) == modular_exponentiation(uint512_t(2), exp, m23));
        CHECK(modular_exponentiation_base_two(exp, m97) == modular_exponentiation(uint512_t(2), exp, m97));
        CHECK(modular_exponentiation_base_two(exp, max) == modular_exponentiation(uint512_t(2), exp, max));
    }

    // (b) Mô-đun chẵn 24 và 2 với exp trong 0..50: so sánh với modular_exponentiation(2, exp, mod)
    uint512_t m24(24);
    uint512_t m2(2);
    for (uint64_t e = 0; e <= 50; ++e)
    {
        uint512_t exp(e);
        CHECK(modular_exponentiation_base_two(exp, m24) == modular_exponentiation(uint512_t(2), exp, m24));
        CHECK(modular_exponentiation_base_two(exp, m2) == modular_exponentiation(uint512_t(2), exp, m2));
    }

    // (c) mod = 1 trả 0
    CHECK(modular_exponentiation_base_two(zero, one).is_zero());
    CHECK(modular_exponentiation_base_two(one, one).is_zero());
    CHECK(modular_exponentiation_base_two(uint512_t(13), one).is_zero());

    // (d) exp = 0 với mod > 1 trả 1
    CHECK(modular_exponentiation_base_two(zero, m23) == one);
    CHECK(modular_exponentiation_base_two(zero, m24) == one);
    CHECK(modular_exponentiation_base_two(zero, max) == one);
    CHECK(modular_exponentiation_base_two(zero, top) == one);
    CHECK(modular_exponentiation_base_two(zero, top_plus_1) == one);

    // (e) mod = 0 ném std::invalid_argument
    CHECK(throws_on_zero([&] { modular_exponentiation_base_two(one, zero); }));
    CHECK(throws_on_zero([&] { modular_exponentiation_base_two(zero, zero); }));

    // (f) 2^BIGINT_BITS mod max == 1
    CHECK(modular_exponentiation_base_two(uint512_t(BIGINT_BITS), max) == one);

    // (g) với mod = top_plus_1 (mô-đun BIGINT_BITS bit lẻ có sẵn trong test)
    for (uint64_t e = 0; e <= 50; ++e)
    {
        uint512_t exp(e);
        CHECK(modular_exponentiation_base_two(exp, top_plus_1) == modular_exponentiation(uint512_t(2), exp, top_plus_1));
    }

    // (h) số mũ cỡ 512 bit: đúng trường hợp prime.cpp dùng
    const uint512_t big_exps[] = {p512 - one, p512 - uint512_t(2), max, max - uint512_t(12345), top, top_plus_1, one << 300};
    for (const uint512_t &e : big_exps)
    {
        CHECK(modular_exponentiation_base_two(e, p512) == modular_exponentiation(uint512_t(2), e, p512));
        CHECK(modular_exponentiation_base_two(e, max) == modular_exponentiation(uint512_t(2), e, max));
        CHECK(modular_exponentiation_base_two(e, top_plus_1) == modular_exponentiation(uint512_t(2), e, top_plus_1));
        CHECK(modular_exponentiation_base_two(e, m97) == modular_exponentiation(uint512_t(2), e, m97));
    }
    CHECK(modular_exponentiation_base_two(p512 - one, p512) == one); // Fermat: 2^(p-1) = 1 (mod p)

    if (argc > 1)
    {
        std::ifstream f(argv[1]);
        CHECK(f.good());
        std::string op, a, b, m, r;
        while (f >> op >> a >> b >> m >> r)
        {
            uint512_t ua = uint512_t::from_hex(a);
            uint512_t ub = uint512_t::from_hex(b);
            uint512_t um = uint512_t::from_hex(m);
            if (op == "red")
                CHECK(reduce_mod(ua, um).to_hex() == r);
            else if (op == "add")
                CHECK(add_mod(ua, ub, um).to_hex() == r);
            else if (op == "mul")
                CHECK(mul_mod(ua, ub, um).to_hex() == r);
            else if (op == "pow")
                CHECK(modular_exponentiation(ua, ub, um).to_hex() == r);
            else
                CHECK(false);
        }
    }
    return finish("test_modexp");
}
