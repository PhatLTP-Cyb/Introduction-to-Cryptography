// Kiểm thử NV5.
#include "dh.h"
#include "test_util.h"

int main()
{
    prng_ctx ctx;
    seed_from_text(&ctx, "test-dh");
    uint512_t one(1), two(2);

    // Ví dụ trong tài liệu: p = 23, g = 5, a = 4, b = 3
    uint512_t p(23), g(5), a(4), b(3);
    uint512_t A = modular_exponentiation(g, a, p);
    uint512_t B = modular_exponentiation(g, b, p);
    CHECK(A == uint512_t(4));
    CHECK(B == uint512_t(10));
    CHECK(modular_exponentiation(B, a, p) == uint512_t(18));
    CHECK(modular_exponentiation(A, b, p) == uint512_t(18));

    // g tìm được phải là phần tử sinh của nhóm nhân
    for (uint64_t value : {23ULL, 47ULL, 83ULL, 107ULL})
    {
        uint512_t safe(value);
        uint512_t q = (safe - one) >> 1;
        for (int i = 0; i < 50; i++)
        {
            uint512_t found = find_generator(&ctx, safe);
            CHECK(found >= two && found <= safe - two);
            CHECK(modular_exponentiation(found, two, safe) != one);
            CHECK(modular_exponentiation(found, q, safe) != one);
        }
    }
    return finish("test_dh");
}
