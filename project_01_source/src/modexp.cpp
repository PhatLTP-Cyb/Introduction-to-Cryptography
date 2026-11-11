// NV2: Lũy thừa mô-đun. Kiểm thử: tests/test_modexp.cpp
//
// Mô-đun lẻ (như p và q của đề): nhân Montgomery. Mỗi phép nhân mô-đun tốn 2 * LIMBS^2 phép nhân
// limb 64 bit, không cần phép chia và không cần số dài gấp đôi để chứa tích.
// Mô-đun chẵn: Montgomery không dùng được vì cần m nguyên tố cùng nhau với 2^64, nên giữ cách
// cộng-dịch chỉ dùng cộng, trừ, so sánh và bit của NV1.
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

// (x * y) mod m bằng cộng-dịch, với điều kiện x, y < m. Dùng cho mô-đun chẵn.
uint512_t mul_mod_shift_add(uint512_t x, uint512_t y, const uint512_t &m)
{
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

// Nghịch đảo của số lẻ m0 theo mod 2^64 bằng lặp Newton: nếu m0 * x = 1 (mod 2^k) thì
// x * (2 - m0 * x) đúng tới mod 2^(2k). Bắt đầu từ x = m0 đã đúng 3 bit vì m0^2 = 1 (mod 8),
// nên 5 vòng cho 96 bit, thừa 64 bit cần dùng.
uint64_t inverse_mod_2_64(uint64_t m0)
{
    uint64_t x = m0;
    for (int i = 0; i < 5; i++)
    {
        x *= 2 - m0 * x;
    }
    return x;
}

// Nhân Montgomery với R = 2^BIGINT_BITS, chỉ dùng được khi m lẻ.
// Dạng Montgomery của x là x * R mod m. Hàm mul(a, b) trả về a * b * R^-1 mod m, nên tích của hai
// số ở dạng Montgomery vẫn ở dạng Montgomery. Chia cho R chỉ là bỏ các limb thấp, nhờ vậy phép
// rút gọn theo mod m không cần phép chia nào.
struct Montgomery
{
    uint512_t m;         // mô-đun lẻ
    uint64_t neg_inv;    // -m^-1 mod 2^64
    uint512_t one;       // R mod m, tức số 1 ở dạng Montgomery
    uint512_t r_squared; // R^2 mod m, nhân với nó để đổi một số sang dạng Montgomery

    explicit Montgomery(const uint512_t &modulus) : m(modulus)
    {
        neg_inv = 0 - inverse_mod_2_64(m.digits[0]);
        // R - m chính là phép trừ quay vòng 0 - m, và đồng dư với R theo mod m.
        one = reduce_mod(uint512_t() - m, m);
        // 2R mod m là số 2 ở dạng Montgomery. Nâng nó lên lũy thừa BIGINT_BITS được
        // 2^BIGINT_BITS ở dạng Montgomery, tức R * R mod m.
        r_squared = pow(add_mod_reduced(one, one, m), uint512_t(BIGINT_BITS));
    }

    // Trả về a * b * R^-1 mod m, với điều kiện a, b < m.
    // Xen kẽ từng limb của b: cộng a * b[i] vào t, rồi cộng thêm u * m với u chọn sao cho limb
    // thấp nhất của t bằng 0 và dời t xuống một limb (chia cho 2^64). Sau LIMBS vòng đã chia cho R.
    uint512_t mul(const uint512_t &a, const uint512_t &b) const
    {
        // Sau mỗi vòng t < 2m, nên ngoài LIMBS limb chỉ cần thêm 2 limb chứa phần tràn.
        uint64_t t[LIMBS + 2] = {0};
        for (size_t i = 0; i < LIMBS; i++)
        {
            // t = t + a * b[i]
            uint64_t carry = 0;
            for (size_t j = 0; j < LIMBS; j++)
            {
                uint128_t cur = static_cast<uint128_t>(a.digits[j]) * b.digits[i] + t[j] + carry;
                t[j] = static_cast<uint64_t>(cur);
                carry = static_cast<uint64_t>(cur >> 64);
            }
            uint128_t top = static_cast<uint128_t>(t[LIMBS]) + carry;
            t[LIMBS] = static_cast<uint64_t>(top);
            t[LIMBS + 1] = static_cast<uint64_t>(top >> 64);

            // t = (t + u * m) / 2^64. Vì u = -t[0] * m^-1 (mod 2^64) nên limb thấp nhất của
            // t + u * m bằng 0 và phép chia là chính xác.
            uint64_t u = t[0] * neg_inv;
            uint128_t cur = static_cast<uint128_t>(u) * m.digits[0] + t[0];
            carry = static_cast<uint64_t>(cur >> 64);
            for (size_t j = 1; j < LIMBS; j++)
            {
                cur = static_cast<uint128_t>(u) * m.digits[j] + t[j] + carry;
                t[j - 1] = static_cast<uint64_t>(cur);
                carry = static_cast<uint64_t>(cur >> 64);
            }
            top = static_cast<uint128_t>(t[LIMBS]) + carry;
            t[LIMBS - 1] = static_cast<uint64_t>(top);
            t[LIMBS] = t[LIMBS + 1] + static_cast<uint64_t>(top >> 64);
        }

        uint512_t result;
        for (size_t j = 0; j < LIMBS; j++)
        {
            result.digits[j] = t[j];
        }
        // Giá trị thật là t[LIMBS] * 2^BIGINT_BITS + result và nhỏ hơn 2m, nên trừ m một lần là đủ;
        // phép trừ quay vòng nên vẫn đúng khi t[LIMBS] = 1.
        if (t[LIMBS] != 0 || result >= m)
        {
            result = result - m;
        }
        return result;
    }

    // Trả về base^exp ở dạng Montgomery, với base_mont là cơ số ở dạng Montgomery.
    // Phân tích exp thành tổng các lũy thừa của 2, duyệt bit từ trái sang phải: mỗi bit
    // bình phương kết quả, gặp bit 1 thì nhân thêm cơ số. Ví dụ 3^13 với 13 = 1101 (hệ 2):
    // ((3^2 * 3)^2)^2 * 3 = 3^13. Khi exp = 0 vòng lặp không chạy và kết quả là 1.
    uint512_t pow(const uint512_t &base_mont, const uint512_t &exp) const
    {
        uint512_t result = one;
        for (size_t i = exp.bit_length(); i-- > 0;)
        {
            result = mul(result, result);
            if (exp.bit(i))
            {
                result = mul(result, base_mont);
            }
        }
        return result;
    }

    // Trả về 2^exp ở dạng Montgomery.
    // Giống pow nhưng với cơ số 2: duyệt bit của exp từ trái sang phải, mỗi bit bình phương
    // kết quả bằng mul(result, result); gặp bit 1 thì nhân đôi bằng add_mod_reduced(result, result, m).
    // Phép nhân đôi ở dạng Montgomery: nếu số x có dạng Montgomery là x~ = (x * R) mod m,
    // thì dạng Montgomery của 2x là ((2x) * R) mod m = (2 * (x * R)) mod m = (x~ + x~) mod m.
    // Do đó, nhân đôi một số ở dạng Montgomery chính là cộng nó với chính nó theo mod m,
    // không cần gọi hàm mul và không cần đổi cơ số sang dạng Montgomery.
    uint512_t pow_base_two(const uint512_t &exp) const
    {
        uint512_t result = one;
        for (size_t i = exp.bit_length(); i-- > 0;)
        {
            result = mul(result, result);
            if (exp.bit(i))
            {
                result = add_mod_reduced(result, result, m);
            }
        }
        return result;
    }
};
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
    // Số tạo từ (số bit của m) - 1 bit cao nhất của a chắc chắn nhỏ hơn m, nên lấy luôn làm r
    // ban đầu và chỉ duyệt các bit còn lại.
    size_t remaining = a.bit_length() - (m.bit_length() - 1);
    uint512_t r = a >> remaining;
    for (size_t i = remaining; i-- > 0;)
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
    if (!m.is_odd())
    {
        return mul_mod_shift_add(x, y, m);
    }

    // mul(x, y) = x * y * R^-1; nhân tiếp với R^2 thì còn x * y * R^2 * R^-2 = x * y (mod m).
    const Montgomery mont(m);
    return mont.mul(mont.mul(x, y), mont.r_squared);
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

    if (mod.is_odd())
    {
        // Đổi cơ số sang dạng Montgomery một lần, tính cả lũy thừa ở dạng đó, rồi đổi kết quả về:
        // mul(b, R^2) = b * R và mul(x * R, 1) = x.
        const Montgomery mont(mod);
        const uint512_t result_mont = mont.pow(mont.mul(b, mont.r_squared), exp);
        return mont.mul(result_mont, uint512_t(1));
    }

    // Mô-đun chẵn: cùng cách duyệt bit từ trái sang phải như Montgomery::pow, nhân bằng cộng-dịch.
    uint512_t result(1);
    for (size_t i = exp.bit_length(); i-- > 0;)
    {
        result = mul_mod_shift_add(result, result, mod);
        if (exp.bit(i))
        {
            result = mul_mod_shift_add(result, b, mod);
        }
    }
    return result;
}

// Trả về (2 ^ exp) mod mod.
uint512_t modular_exponentiation_base_two(const uint512_t &exp, const uint512_t &mod)
{
    require_modulus(mod, "modular_exponentiation_base_two");
    if (mod == uint512_t(1))
    {
        return uint512_t(0); // mọi số mod 1 đều bằng 0, kể cả 2^0
    }

    if (mod.is_odd())
    {
        const Montgomery mont(mod);
        const uint512_t result_mont = mont.pow_base_two(exp);
        return mont.mul(result_mont, uint512_t(1));
    }

    return modular_exponentiation(uint512_t(2), exp, mod);
}
