// Kiểm thử NV4.
#include "dh.h"
#include "test_util.h"

#include <set>
#include <string>

int main()
{
    // Seed cố định phải cho cùng dãy số, seed khác phải cho dãy khác
    prng_ctx c1, c2, c3;
    seed_from_text(&c1, "nhom15");
    seed_from_text(&c2, "nhom15");
    seed_from_text(&c3, "nhom15b");
    uint512_t r1 = random_bits(&c1, 256);
    CHECK(r1 == random_bits(&c2, 256));
    CHECK(r1 != random_bits(&c3, 256));

    // Seed từ hệ điều hành không lặp lại
    prng_ctx o1, o2;
    seed_from_os(&o1);
    seed_from_os(&o2);
    CHECK(random_bits(&o1, 256) != random_bits(&o2, 256));

    prng_ctx ctx;
    seed_from_text(&ctx, "test-random");

    for (size_t bits : {1, 63, 64, 65, 128, 300})
    {
        for (int i = 0; i < 200; i++)
            CHECK(random_bits(&ctx, bits).bit_length() <= bits);
    }

    // Khoảng nhỏ: phải nằm trong biên và đi qua mọi giá trị
    std::set<std::string> seen;
    uint512_t low(2), high(21);
    for (int i = 0; i < 10000; i++)
    {
        uint512_t x = random_range(&ctx, low, high);
        CHECK(x >= low && x <= high);
        seen.insert(x.to_hex());
    }
    CHECK(seen.size() == 20);

    // Khóa riêng nằm trong [2, p - 2]
    uint512_t p23(23);
    seen.clear();
    for (int i = 0; i < 10000; i++)
    {
        uint512_t k = generate_private_key(&ctx, p23);
        CHECK(k >= uint512_t(2) && k <= uint512_t(21));
        seen.insert(k.to_hex());
    }
    CHECK(seen.size() == 20);

    uint512_t big = uint512_t::from_hex("7fffffffffffffffffffffffffffffff");
    for (int i = 0; i < 2000; i++)
    {
        uint512_t k = generate_private_key(&ctx, big);
        CHECK(k >= uint512_t(2) && k <= big - uint512_t(2));
    }
    return finish("test_random");
}
