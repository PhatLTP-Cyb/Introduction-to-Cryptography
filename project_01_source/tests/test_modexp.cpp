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
