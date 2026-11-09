// Đo thời gian NV2. Cách chạy: mingw32-make bench (Linux: make bench)
//
// Mô-đun là số lẻ đủ BIGINT_BITS bit như p của đề; cơ số và số mũ ngẫu nhiên nhỏ hơn mô-đun.
// Số ngẫu nhiên lấy từ PRNG của lib/fips202 với seed cố định, không phụ thuộc NV4,
// nên lần chạy nào cũng đo trên cùng bộ số. Đo thời gian thực bằng steady_clock.
#include "dh.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <vector>

namespace
{
typedef std::chrono::steady_clock Clock;

uint512_t random_below_top(prng_ctx *ctx)
{
    uint512_t x;
    for (size_t i = 0; i < LIMBS; i++)
    {
        x.digits[i] = prng_next_u64(ctx);
    }
    return x;
}

uint512_t random_modulus(prng_ctx *ctx)
{
    uint512_t m = random_below_top(ctx);
    m.set_bit(BIGINT_BITS - 1); // đủ BIGINT_BITS bit, phép nhân đôi trong mul_mod có thể tràn
    m.set_bit(0);               // lẻ như p
    return m;
}

uint512_t random_below(prng_ctx *ctx, const uint512_t &m)
{
    return reduce_mod(random_below_top(ctx), m);
}

double elapsed_ms(Clock::time_point start)
{
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

// Chạy ít nhất min_runs lần và đủ khoảng 1 giây, in trung vị, nhanh nhất và trung bình.
// Đầu vào sinh ngoài vùng đo; sink giữ kết quả để trình dịch không bỏ phép tính.
template <typename Op>
void measure(const char *name, prng_ctx *ctx, int min_runs, double scale, const char *unit, Op op, uint64_t &sink)
{
    std::vector<double> samples;
    double total = 0;
    while (static_cast<int>(samples.size()) < min_runs || total < 1000.0)
    {
        uint512_t m = random_modulus(ctx);
        uint512_t x = random_below(ctx, m);
        uint512_t y = random_below(ctx, m);
        Clock::time_point start = Clock::now();
        sink ^= op(x, y, m).digits[0];
        double t = elapsed_ms(start);
        samples.push_back(t);
        total += t;
    }
    std::sort(samples.begin(), samples.end());
    size_t n = samples.size();
    std::printf("%-34s trung vi %8.3f %s, nhanh nhat %8.3f %s, trung binh %8.3f %s (%zu lan)\n",
                name, samples[n / 2] * scale, unit, samples[0] * scale, unit, total / n * scale, unit, n);
}
} // namespace

int main()
{
    prng_ctx ctx;
    const uint8_t seed[] = "bench-modexp-nhom15";
    prng_seed(&ctx, seed, sizeof(seed) - 1);
    uint64_t sink = 0;

    std::printf("Do thoi gian NV2: BIGINT_BITS = %zu, mo-dun le du %zu bit\n", BIGINT_BITS, BIGINT_BITS);

    // Chạy nóng máy một lần để CPU lên xung nhịp trước khi đo.
    {
        uint512_t m = random_modulus(&ctx);
        sink ^= modular_exponentiation(random_below(&ctx, m), random_below(&ctx, m), m).digits[0];
    }

    measure("modular_exponentiation, co so bat ky", &ctx, 10, 1.0, "ms",
            [](const uint512_t &x, const uint512_t &y, const uint512_t &m)
            { return modular_exponentiation(x, y, m); },
            sink);

    // Cơ số 2 là phép lọc Fermat mà NV3 chạy trên mỗi ứng viên.
    measure("modular_exponentiation, co so 2", &ctx, 10, 1.0, "ms",
            [](const uint512_t &, const uint512_t &y, const uint512_t &m)
            { return modular_exponentiation(uint512_t(2), y, m); },
            sink);

    // Một phép mul_mod quá nhanh để đo riêng, nên đo 1000 phép liên tiếp:
    // số ms của 1000 phép bằng số µs của một phép, nên hệ số đổi vẫn là 1.
    measure("mul_mod (1 phep)", &ctx, 10, 1.0, "us",
            [](const uint512_t &x, const uint512_t &y, const uint512_t &m)
            {
                uint512_t r = x;
                for (int i = 0; i < 1000; i++)
                {
                    r = mul_mod(r, y, m);
                }
                return r;
            },
            sink);

    std::printf("(sink %llx)\n", static_cast<unsigned long long>(sink));
    return 0;
}
