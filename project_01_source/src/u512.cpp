// NV1: Kiểu số lớn. Kiểm thử: tests/test_u512.cpp
#include "u512.h"
#include "common.h"

#include <ostream>

uint512_t::uint512_t(uint64_t value)
{
    // TODO: gán value vào limb thấp nhất
    not_implemented("uint512_t(uint64_t)");
}

uint512_t uint512_t::from_hex(const std::string &hex)
{
    // TODO: đọc từ ký tự cuối chuỗi, mỗi 16 ký tự hex là một limb
    not_implemented("uint512_t::from_hex");
}

std::string uint512_t::to_hex() const
{
    // TODO: in từ limb cao xuống, bỏ các số 0 ở đầu
    not_implemented("uint512_t::to_hex");
}

bool uint512_t::is_zero() const
{
    not_implemented("uint512_t::is_zero");
}

bool uint512_t::is_odd() const
{
    not_implemented("uint512_t::is_odd");
}

bool uint512_t::bit(size_t i) const
{
    // TODO: limb i / 64, vị trí i % 64
    not_implemented("uint512_t::bit");
}

void uint512_t::set_bit(size_t i)
{
    not_implemented("uint512_t::set_bit");
}

size_t uint512_t::bit_length() const
{
    not_implemented("uint512_t::bit_length");
}

uint64_t uint512_t::mod_u64(uint64_t m) const
{
    // TODO: duyệt từ limb cao xuống, r = (r * 2^64 + limb) % m, dùng uint128_t
    not_implemented("uint512_t::mod_u64");
}

uint512_t operator+(const uint512_t &a, const uint512_t &b)
{
    not_implemented("operator+");
}

uint512_t operator-(const uint512_t &a, const uint512_t &b)
{
    // TODO: trừ từ limb thấp lên, nhớ số mượn
    not_implemented("operator-");
}

uint512_t operator<<(const uint512_t &a, size_t n)
{
    // TODO: xử lý n >= BIGINT_BITS và n chia hết cho 64
    not_implemented("operator<<");
}

uint512_t operator>>(const uint512_t &a, size_t n)
{
    not_implemented("operator>>");
}

bool add_overflow(const uint512_t &a, const uint512_t &b, uint512_t &result)
{
    // TODO: cộng từ limb thấp lên bằng uint128_t, trả về số nhớ cuối cùng
    not_implemented("add_overflow");
}

bool operator==(const uint512_t &a, const uint512_t &b)
{
    not_implemented("operator==");
}

bool operator!=(const uint512_t &a, const uint512_t &b)
{
    return !(a == b);
}

bool operator<(const uint512_t &a, const uint512_t &b)
{
    // TODO: so sánh từ limb cao xuống
    not_implemented("operator<");
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
