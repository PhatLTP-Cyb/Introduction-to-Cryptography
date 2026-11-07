// NV1: Kiểu số lớn. Kiểm thử: tests/test_u512.cpp
//
// Một số gồm LIMBS "chữ số" 64 bit (limb), digits[0] là limb thấp nhất:
//     giá trị = digits[0] + digits[1] * 2^64 + ... + digits[LIMBS - 1] * 2^(64 * (LIMBS - 1))
// Mọi vòng lặp chạy theo LIMBS và BIGINT_BITS, nên chỉ cần đổi LIMBS trong u512.h là đổi được kích thước số.
#include "u512.h"

#include <ostream>
#include <stdexcept>

namespace
{
constexpr size_t LIMB_BITS = 64;
constexpr size_t HEX_PER_LIMB = LIMB_BITS / 4; // mỗi chữ số hex là 4 bit

// Giá trị của một chữ số hex, -1 nếu ký tự không phải hex. Nhận cả chữ hoa.
int hex_value(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    return -1;
}
} // namespace

uint512_t::uint512_t(uint64_t value)
{
    for (size_t i = 0; i < LIMBS; i++)
    {
        digits[i] = 0;
    }
    digits[0] = value;
}

uint512_t uint512_t::from_hex(const std::string &hex)
{
    if (hex.empty())
    {
        throw std::invalid_argument("from_hex: chuoi rong");
    }

    // Bỏ các số 0 ở đầu, phần còn lại phải vừa trong BIGINT_BITS bit.
    size_t first = 0;
    while (first < hex.size() && hex[first] == '0')
    {
        first++;
    }
    if (hex.size() - first > BIGINT_BITS / 4)
    {
        throw std::out_of_range("from_hex: so vuot qua BIGINT_BITS bit");
    }

    // Đọc từ ký tự cuối chuỗi. Chữ số hex thứ k (đếm từ 0, từ phải sang)
    // nằm ở limb k / 16, vị trí bit 4 * (k % 16) trong limb đó.
    uint512_t result;
    size_t k = 0;
    for (size_t pos = hex.size(); pos-- > first; k++)
    {
        int v = hex_value(hex[pos]);
        if (v < 0)
        {
            throw std::invalid_argument("from_hex: ky tu khong phai hex");
        }
        result.digits[k / HEX_PER_LIMB] |= static_cast<uint64_t>(v) << (4 * (k % HEX_PER_LIMB));
    }
    return result;
}

std::string uint512_t::to_hex() const
{
    static const char HEX[] = "0123456789abcdef";

    // In từ limb cao xuống, mỗi limb đủ 16 chữ số hex, rồi bỏ các số 0 ở đầu.
    std::string s;
    s.reserve(BIGINT_BITS / 4);
    for (size_t i = LIMBS; i-- > 0;)
    {
        for (int shift = 60; shift >= 0; shift -= 4)
        {
            s.push_back(HEX[(digits[i] >> shift) & 0xf]);
        }
    }

    size_t first = s.find_first_not_of('0');
    if (first == std::string::npos)
    {
        return "0";
    }
    return s.substr(first);
}

bool uint512_t::is_zero() const
{
    for (size_t i = 0; i < LIMBS; i++)
    {
        if (digits[i] != 0)
        {
            return false;
        }
    }
    return true;
}

bool uint512_t::is_odd() const
{
    return (digits[0] & 1) != 0;
}

bool uint512_t::bit(size_t i) const
{
    if (i >= BIGINT_BITS)
    {
        return false; // ngoài kích thước kiểu thì xem như bit 0
    }
    return ((digits[i / LIMB_BITS] >> (i % LIMB_BITS)) & 1) != 0;
}

void uint512_t::set_bit(size_t i)
{
    if (i >= BIGINT_BITS)
    {
        throw std::out_of_range("set_bit: vi tri vuot qua BIGINT_BITS");
    }
    digits[i / LIMB_BITS] |= static_cast<uint64_t>(1) << (i % LIMB_BITS);
}

size_t uint512_t::bit_length() const
{
    // Tìm limb khác 0 cao nhất, rồi đếm số bit có nghĩa trong limb đó.
    for (size_t i = LIMBS; i-- > 0;)
    {
        if (digits[i] != 0)
        {
            size_t n = 0;
            for (uint64_t top = digits[i]; top != 0; top >>= 1)
            {
                n++;
            }
            return i * LIMB_BITS + n;
        }
    }
    return 0;
}

uint64_t uint512_t::mod_u64(uint64_t m) const
{
    if (m == 0)
    {
        throw std::invalid_argument("mod_u64: chia cho 0");
    }

    // Chia như chia tay, đi từ limb cao xuống: r = (r * 2^64 + limb) mod m.
    // r < m < 2^64 nên r * 2^64 + limb luôn vừa trong 128 bit.
    uint64_t r = 0;
    for (size_t i = LIMBS; i-- > 0;)
    {
        uint128_t cur = (static_cast<uint128_t>(r) << LIMB_BITS) | digits[i];
        r = static_cast<uint64_t>(cur % m);
    }
    return r;
}

uint512_t operator+(const uint512_t &a, const uint512_t &b)
{
    uint512_t result;
    add_overflow(a, b, result); // bỏ số nhớ cuối cùng: cộng theo mod 2^BIGINT_BITS
    return result;
}

uint512_t operator-(const uint512_t &a, const uint512_t &b)
{
    // Trừ từ limb thấp lên, mượn 1 từ limb kế bên khi limb hiện tại không đủ.
    // Phép trừ số không dấu tự quay vòng, nên khi a < b kết quả là a - b + 2^BIGINT_BITS.
    uint512_t result;
    uint64_t borrow = 0;
    for (size_t i = 0; i < LIMBS; i++)
    {
        uint64_t x = a.digits[i];
        uint64_t y = b.digits[i];
        result.digits[i] = x - y - borrow;
        // Cần mượn khi x < y + borrow (so như số nguyên, không quay vòng).
        borrow = (x < y || (x == y && borrow != 0)) ? 1 : 0;
    }
    return result;
}

uint512_t operator<<(const uint512_t &a, size_t n)
{
    uint512_t result;
    if (n >= BIGINT_BITS)
    {
        return result; // mọi bit đều bị đẩy ra ngoài
    }

    // Dịch n bit = dịch n / 64 limb nguyên, rồi dịch n % 64 bit bên trong mỗi limb;
    // các bit rơi ra khỏi một limb được ghép vào limb cao hơn kế bên.
    size_t limb_shift = n / LIMB_BITS;
    size_t bit_shift = n % LIMB_BITS;
    for (size_t i = limb_shift; i < LIMBS; i++)
    {
        size_t src = i - limb_shift;
        uint64_t v = a.digits[src] << bit_shift;
        // Dịch uint64_t đi 64 bit là hành vi không xác định, nên bỏ qua khi bit_shift = 0.
        if (bit_shift != 0 && src > 0)
        {
            v |= a.digits[src - 1] >> (LIMB_BITS - bit_shift);
        }
        result.digits[i] = v;
    }
    return result;
}

uint512_t operator>>(const uint512_t &a, size_t n)
{
    uint512_t result;
    if (n >= BIGINT_BITS)
    {
        return result;
    }

    // Ngược với phép dịch trái: các bit rơi ra được ghép vào limb thấp hơn kế bên.
    size_t limb_shift = n / LIMB_BITS;
    size_t bit_shift = n % LIMB_BITS;
    for (size_t i = 0; i + limb_shift < LIMBS; i++)
    {
        size_t src = i + limb_shift;
        uint64_t v = a.digits[src] >> bit_shift;
        if (bit_shift != 0 && src + 1 < LIMBS)
        {
            v |= a.digits[src + 1] << (LIMB_BITS - bit_shift);
        }
        result.digits[i] = v;
    }
    return result;
}

bool add_overflow(const uint512_t &a, const uint512_t &b, uint512_t &result)
{
    // Cộng từ limb thấp lên như cộng tay. Hai limb cộng số nhớ có thể cần 65 bit,
    // nên tính trên uint128_t: 64 bit thấp là limb kết quả, phần trên là số nhớ.
    // Limb i của a và b được đọc trước khi ghi limb i của result,
    // nên gọi add_overflow(x, y, x) vẫn đúng.
    uint64_t carry = 0;
    for (size_t i = 0; i < LIMBS; i++)
    {
        uint128_t sum = static_cast<uint128_t>(a.digits[i]) + b.digits[i] + carry;
        result.digits[i] = static_cast<uint64_t>(sum);
        carry = static_cast<uint64_t>(sum >> LIMB_BITS);
    }
    return carry != 0;
}

bool operator==(const uint512_t &a, const uint512_t &b)
{
    for (size_t i = 0; i < LIMBS; i++)
    {
        if (a.digits[i] != b.digits[i])
        {
            return false;
        }
    }
    return true;
}

bool operator!=(const uint512_t &a, const uint512_t &b)
{
    return !(a == b);
}

bool operator<(const uint512_t &a, const uint512_t &b)
{
    // So từ limb cao xuống; limb đầu tiên khác nhau quyết định kết quả.
    for (size_t i = LIMBS; i-- > 0;)
    {
        if (a.digits[i] != b.digits[i])
        {
            return a.digits[i] < b.digits[i];
        }
    }
    return false;
}

bool operator>(const uint512_t &a, const uint512_t &b)
{
    return b < a;
}

bool operator<=(const uint512_t &a, const uint512_t &b)
{
    return !(b < a);
}

bool operator>=(const uint512_t &a, const uint512_t &b)
{
    return !(a < b);
}

std::ostream &operator<<(std::ostream &os, const uint512_t &a)
{
    return os << a.to_hex();
}
