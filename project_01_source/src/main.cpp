// NV5: Trao đổi khóa Diffie-Hellman.
// Cách chạy: dh               seed từ hệ điều hành
//            dh --seed abc    seed cố định, chỉ dùng khi kiểm thử
#include "dh.h"

#include <cstring>
#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

int main(int argc, char **argv)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    prng_ctx ctx;
    if (argc == 3 && std::strcmp(argv[1], "--seed") == 0)
    {
        seed_from_text(&ctx, argv[2]);
        std::cout << "Chế độ seed: cố định (chỉ để kiểm thử)\n";
    }
    else
    {
        seed_from_os(&ctx);
        std::cout << "Chế độ seed: hệ điều hành\n";
    }

    // 1. Sinh số nguyên tố lớn p và phần tử sinh g
    size_t bit_size = 512; // Tối thiểu 512, tối đa BIGINT_BITS trong u512.h
    uint512_t p = generate_safe_prime(&ctx, bit_size);
    uint512_t g = find_generator(&ctx, p);

    // 2. Sinh khóa riêng của Alice và Bob
    uint512_t a = generate_private_key(&ctx, p);
    uint512_t b = generate_private_key(&ctx, p);

    // 3. Tính giá trị công khai của Alice và Bob
    uint512_t A = modular_exponentiation(g, a, p);
    uint512_t B = modular_exponentiation(g, b, p);

    // 4. Tính bí mật chung
    uint512_t alice_shared_secret = modular_exponentiation(B, a, p);
    uint512_t bob_shared_secret = modular_exponentiation(A, b, p);

    // 5. Hiển thị kết quả và xác minh rằng bí mật chung trùng khớp
    std::cout << "p = " << p << "\n";
    std::cout << "g = " << g << "\n";
    std::cout << "A = " << A << "\n";
    std::cout << "B = " << B << "\n";
    std::cout << "Bí mật chung Alice nhận được: " << alice_shared_secret << "\n";
    std::cout << "Bí mật chung Bob nhận được: " << bob_shared_secret << "\n";
    std::cout << "Quá trình tính toán đúng không? " << (alice_shared_secret == bob_shared_secret) << "\n";

    return 0;
}
