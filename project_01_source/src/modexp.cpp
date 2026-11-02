// NV2: Lũy thừa mô-đun. Kiểm thử: tests/test_modexp.cpp
#include "dh.h"
#include "common.h"

// Trả về a mod m với m > 0.
uint512_t reduce_mod(const uint512_t &a, const uint512_t &m)
{
    // TODO: duyệt bit của a từ cao xuống: nhân đôi phần dư, cộng bit, trừ m nếu cần.
    // Nhân đôi có thể tràn khi m đủ BIGINT_BITS, phải giữ lại bit bị đẩy ra.
    not_implemented("reduce_mod");
}

// Trả về (a + b) mod m với a, b < m.
uint512_t add_mod(const uint512_t &a, const uint512_t &b, const uint512_t &m)
{
    // TODO: dùng add_overflow; tràn hoặc tổng >= m thì trừ m.
    not_implemented("add_mod");
}

// Trả về (a * b) mod m với a, b < m.
uint512_t mul_mod(const uint512_t &a, const uint512_t &b, const uint512_t &m)
{
    // TODO: phân tích b thành lũy thừa của 2, cộng dồn bằng add_mod (slide Bài 2, mulMod).
    // Nâng cấp sau nếu kịp: Montgomery.
    not_implemented("mul_mod");
}

// Trả về (base ^ exp) mod mod.
uint512_t modular_exponentiation(const uint512_t &base, const uint512_t &exp, const uint512_t &mod)
{
    // TODO: phân tích exp thành lũy thừa của 2, bình phương và nhân.
    // Lưu ý: exp = 0 trả về 1; mod = 1 trả về 0; base có thể >= mod.
    not_implemented("modular_exponentiation");
}
