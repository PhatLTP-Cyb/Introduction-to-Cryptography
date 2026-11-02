// Kiểm thử NV2. Tham số: tệp dữ liệu do tests/run_tests.py sinh, mỗi dòng "phép a b m kết_quả".
#include "dh.h"
#include "test_util.h"

#include <fstream>
#include <string>

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
