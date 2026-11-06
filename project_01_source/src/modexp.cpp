// NV2: Lũy thừa mô-đun. Kiểm thử: tests/test_modexp.cpp
//
// Chỉ dùng cộng, trừ, so sánh và bit của NV1. Mọi kết quả trung gian được lấy mod ngay
// nên luôn nhỏ hơn m, không cần số dài gấp đôi để chứa tích.
#include "dh.h"

#include <stdexcept>
#include <string>
#include <utility>

namespace
{
void require_modulus(const uint512_t &m, const char *who)
{
    if (m.is_zero())
    {
        throw std::invalid_argument(std::string(who) + ": mo-dun bang 0");
    }
}

// (a + b) mod m, với điều kiện a, b < m.
// Khi m đủ BIGINT_BITS bit, tổng thật có thể vượt 2^BIGINT_BITS và add_overflow báo tràn.
// Tổng thật luôn nhỏ hơn 2m nên trừ m một lần là đủ; phép trừ quay vòng nên vẫn đúng khi đã tràn.
uint512_t add_mod_reduced(const uint512_t &a, const uint512_t &b, const uint512_t &m)
{
    uint512_t sum;
    bool carry = add_overflow(a, b, sum);
    if (carry || sum >= m)
    {
        sum = sum - m;
    }
    return sum;
}

uint512_t reduced(const uint512_t &a, const uint512_t &m)
{
    return a < m ? a : reduce_mod(a, m);
}
} // namespace

// Trả về a mod m với m > 0.
uint512_t reduce_mod(const uint512_t &a, const uint512_t &m)
{
    require_modulus(m, "reduce_mod");
    if (a < m)
    {
        return a;
    }

    // Chia ở hệ 2, duyệt bit của a từ cao xuống: r = 2r + bit, rồi trừ m nếu r >= m.
    // Sau mỗi bước r < m; đi hết các bit thì r = a mod m.
    uint512_t r;
    for (size_t i = a.bit_length(); i-- > 0;)
    {
        uint512_t next;
        // 2r có thể tràn khỏi BIGINT_BITS khi m đủ BIGINT_BITS bit, nên giữ lại cờ tràn.
        bool carry = add_overflow(r, r, next);
        if (a.bit(i))
        {
            next.digits[0] |= 1; // bit 0 của 2r luôn là 0, nên cộng bit chỉ cần đặt bit 0
        }
        // 2r + bit < 2m nên trừ m một lần là đủ; khi đã tràn thì giá trị thật chắc chắn >= m.
        if (carry || next >= m)
        {
            next = next - m;
        }
        r = next;
    }
    return r;
}

// Trả về (a + b) mod m. Đầu vào nên nhỏ hơn m; nếu chưa thì được rút gọn trước.
uint512_t add_mod(const uint512_t &a, const uint512_t &b, const uint512_t &m)
{
    require_modulus(m, "add_mod");
    return add_mod_reduced(reduced(a, m), reduced(b, m), m);
}

// Trả về (a * b) mod m. Đầu vào nên nhỏ hơn m; nếu chưa thì được rút gọn trước.
uint512_t mul_mod(const uint512_t &a, const uint512_t &b, const uint512_t &m)
{
    require_modulus(m, "mul_mod");
    uint512_t x = reduced(a, m);
    uint512_t y = reduced(b, m);
    // Số vòng lặp bằng số bit của y, nên để y là thừa số ngắn hơn (nhân với 2 chỉ tốn 2 vòng).
    if (y.bit_length() > x.bit_length())
    {
        std::swap(x, y);
    }

    // Phân tích y thành tổng các lũy thừa của 2, duyệt bit từ cao xuống: mỗi bit nhân đôi
    // kết quả, gặp bit 1 thì cộng thêm x. Ví dụ 7 * 13 với 13 = 1101 (hệ 2):
    // ((7 * 2 + 7) * 2 * 2) + 7 = 91. Mọi phép cộng đều mod m nên kết quả luôn nhỏ hơn m.
    uint512_t r;
    for (size_t i = y.bit_length(); i-- > 0;)
    {
        r = add_mod_reduced(r, r, m);
        if (y.bit(i))
        {
            r = add_mod_reduced(r, x, m);
        }
    }
    return r;
}

// Trả về (base ^ exp) mod mod.
uint512_t modular_exponentiation(const uint512_t &base, const uint512_t &exp, const uint512_t &mod)
{
    require_modulus(mod, "modular_exponentiation");
    if (mod == uint512_t(1))
    {
        return uint512_t(0); // mọi số mod 1 đều bằng 0, kể cả base^0
    }
    uint512_t b = reduced(base, mod);

    // Phân tích exp thành tổng các lũy thừa của 2, duyệt bit từ trái sang phải: mỗi bit
    // bình phương kết quả, gặp bit 1 thì nhân thêm cơ số. Ví dụ 3^13 với 13 = 1101 (hệ 2):
    // ((3^2 * 3)^2)^2 * 3 = 3^13. Khi exp = 0 vòng lặp không chạy và kết quả là 1.
    uint512_t result(1);
    for (size_t i = exp.bit_length(); i-- > 0;)
    {
        result = mul_mod(result, result, mod);
        if (exp.bit(i))
        {
            result = mul_mod(result, b, mod);
        }
    }
    return result;
}
