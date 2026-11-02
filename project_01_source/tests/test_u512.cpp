// Kiểm thử NV1. Tham số: tệp dữ liệu do tests/run_tests.py sinh, mỗi dòng "phép a b kết_quả".
#include "dh.h"
#include "test_util.h"

#include <cstdlib>
#include <fstream>
#include <string>

int main(int argc, char **argv)
{
    uint512_t zero;
    uint512_t one(1);
    CHECK(zero.is_zero());
    CHECK(!one.is_zero());
    CHECK(one.is_odd());
    CHECK(zero.to_hex() == "0");
    CHECK((one + one).to_hex() == "2");
    CHECK(zero.bit_length() == 0);
    CHECK(one.bit_length() == 1);
    CHECK(uint512_t::from_hex("ff").bit(7));
    CHECK(!uint512_t::from_hex("ff").bit(8));

    uint512_t x;
    x.set_bit(100);
    CHECK(x == (one << 100));
    CHECK(x.bit_length() == 101);

    // Số nhớ và số mượn đi qua ranh giới limb
    uint512_t limb_max(UINT64_MAX);
    CHECK((limb_max + one).to_hex() == "10000000000000000");
    CHECK(((limb_max + one) - one) == limb_max);

    // Tràn khỏi kích thước kiểu
    uint512_t all_ones = zero - one;
    uint512_t sum;
    CHECK(all_ones.bit_length() == BIGINT_BITS);
    CHECK(add_overflow(all_ones, one, sum));
    CHECK(sum.is_zero());
    CHECK(!add_overflow(one, one, sum));

    if (argc > 1)
    {
        std::ifstream f(argv[1]);
        CHECK(f.good());
        std::string op, a, b, r;
        while (f >> op >> a >> b >> r)
        {
            uint512_t ua = uint512_t::from_hex(a);
            uint512_t ub = uint512_t::from_hex(b);
            size_t n = std::strtoull(b.c_str(), nullptr, 16);
            if (op == "add")
                CHECK((ua + ub).to_hex() == r);
            else if (op == "sub")
                CHECK((ua - ub).to_hex() == r);
            else if (op == "shl")
                CHECK((ua << n).to_hex() == r);
            else if (op == "shr")
                CHECK((ua >> n).to_hex() == r);
            else if (op == "mod")
                CHECK(uint512_t(ua.mod_u64(n)).to_hex() == r);
            else if (op == "cmp")
                CHECK((ua < ub ? "lt" : (ua == ub ? "eq" : "gt")) == r);
            else if (op == "hex")
                CHECK(ua.to_hex() == r);
            else
                CHECK(false);
        }
    }
    return finish("test_u512");
}
