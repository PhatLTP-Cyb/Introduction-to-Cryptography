#ifndef DH_H
#define DH_H

#include <stddef.h>
#include <stdint.h>
#include "prng.h"
#include "u512.h"

// NV2 - src/modexp.cpp
uint512_t reduce_mod(const uint512_t &a, const uint512_t &m);
uint512_t add_mod(const uint512_t &a, const uint512_t &b, const uint512_t &m);
uint512_t mul_mod(const uint512_t &a, const uint512_t &b, const uint512_t &m);
uint512_t modular_exponentiation(const uint512_t &base, const uint512_t &exp, const uint512_t &mod);

// NV4 - src/random.cpp
void seed_from_os(prng_ctx *ctx);
void seed_from_text(prng_ctx *ctx, const char *text);
uint512_t random_bits(prng_ctx *ctx, size_t bits);
uint512_t random_range(prng_ctx *ctx, const uint512_t &low, const uint512_t &high);
uint512_t generate_private_key(prng_ctx *ctx, const uint512_t &p);

// NV3 - src/prime.cpp
bool has_small_factor(const uint512_t &n);
bool is_probable_prime(prng_ctx *ctx, const uint512_t &n, int rounds = 20);
uint512_t generate_safe_prime(prng_ctx *ctx, size_t bitsize = 512);

// NV5 - src/dh.cpp
uint512_t find_generator(prng_ctx *ctx, const uint512_t &p);

#endif
