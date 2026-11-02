#ifndef U512_H
#define U512_H

#include <stddef.h>
#include <stdint.h>
#include <iosfwd>
#include <string>

typedef __uint128_t uint128_t;

// Số limb 64 bit. Đổi thành 16 để có 1024 bit, 32 để có 2048 bit (tên kiểu giữ nguyên).
constexpr size_t LIMBS = 8;
constexpr size_t BIGINT_BITS = LIMBS * 64;

struct uint512_t
{
    uint64_t digits[LIMBS]; // digits[0] là limb thấp nhất

    // Constructor mặc định khởi tạo mảng toàn số 0
    uint512_t()
    {
        for (size_t i = 0; i < LIMBS; i++)
        {
            digits[i] = 0;
        }
    }

    explicit uint512_t(uint64_t value);

    // Hệ 16 chữ thường, không có số 0 ở đầu; số 0 là "0".
    static uint512_t from_hex(const std::string &hex);
    std::string to_hex() const;

    bool is_zero() const;
    bool is_odd() const;
    bool bit(size_t i) const;      // Bit thứ i, tính từ bit thấp nhất
    void set_bit(size_t i);        // Đặt bit thứ i thành 1
    size_t bit_length() const;     // Số bit có nghĩa; số 0 trả về 0
    uint64_t mod_u64(uint64_t m) const; // Dư khi chia cho số 64 bit, dùng để sàng
};

// Cộng và trừ theo mod 2^BIGINT_BITS, dịch bit bỏ phần tràn.
uint512_t operator+(const uint512_t &a, const uint512_t &b);
uint512_t operator-(const uint512_t &a, const uint512_t &b);
uint512_t operator<<(const uint512_t &a, size_t n);
uint512_t operator>>(const uint512_t &a, size_t n);

// Như operator+ nhưng trả về true nếu tổng bị tràn khỏi BIGINT_BITS.
// p đủ 512 bit nên a + b có thể tràn, add_mod phải dùng hàm này.
bool add_overflow(const uint512_t &a, const uint512_t &b, uint512_t &result);

bool operator==(const uint512_t &a, const uint512_t &b);
bool operator!=(const uint512_t &a, const uint512_t &b);
bool operator<(const uint512_t &a, const uint512_t &b);
bool operator>(const uint512_t &a, const uint512_t &b);
bool operator<=(const uint512_t &a, const uint512_t &b);
bool operator>=(const uint512_t &a, const uint512_t &b);

std::ostream &operator<<(std::ostream &os, const uint512_t &a);

#endif
