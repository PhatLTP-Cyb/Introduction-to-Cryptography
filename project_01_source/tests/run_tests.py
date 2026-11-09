"""Sinh dữ liệu đối chiếu bằng Python, chạy các test C++ và kiểm tra lại kết quả.

Cách chạy: python tests/run_tests.py [số bit safe prime, mặc định 128]
Dùng 512 để kiểm tra đúng kích thước của đề.
"""
import os
import random
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = ".exe" if os.name == "nt" else ""
CASES = 1000


def word_bits():
    with open(os.path.join(ROOT, "src", "u512.h"), encoding="utf-8") as f:
        return 64 * int(re.search(r"LIMBS = (\d+)", f.read()).group(1))


BITS = word_bits()
MASK = (1 << BITS) - 1


def rand_value(rng):
    """Số ngẫu nhiên, ưu tiên các độ dài và mẫu bit dễ lộ lỗi số nhớ."""
    kind = rng.randrange(4)
    if kind == 0:
        return rng.getrandbits(BITS)
    if kind == 1:
        return rng.getrandbits(rng.randrange(1, BITS + 1))
    if kind == 2:
        return ((1 << rng.randrange(1, BITS + 1)) - 1) & MASK
    return (rng.getrandbits(BITS) | (1 << (BITS - 1))) & MASK


def write_u512(path, rng):
    with open(path, "w", encoding="utf-8") as f:
        for _ in range(CASES):
            a, b = rand_value(rng), rand_value(rng)
            n = rng.randrange(0, BITS)
            small = rng.getrandbits(rng.randrange(1, 64)) | 1
            c = a if rng.randrange(8) == 0 else b
            state = "lt" if a < c else ("eq" if a == c else "gt")
            f.write(f"hex {a:x} 0 {a:x}\n")
            f.write(f"add {a:x} {b:x} {(a + b) & MASK:x}\n")
            f.write(f"sub {a:x} {b:x} {(a - b) & MASK:x}\n")
            f.write(f"shl {a:x} {n:x} {(a << n) & MASK:x}\n")
            f.write(f"shr {a:x} {n:x} {a >> n:x}\n")
            f.write(f"mod {a:x} {small:x} {a % small:x}\n")
            f.write(f"cmp {a:x} {c:x} {state}\n")


def write_modexp(path, rng):
    # Nửa số ca mô-đun lẻ như p, nửa còn lại để nguyên nên có cả mô-đun chẵn.
    # Thêm các mô-đun biên: 2, 2^BITS - 1, 2^BITS - 2 và 2^(BITS-1).
    special = [2, MASK, MASK - 1, 1 << (BITS - 1)]
    moduli = [rand_value(rng) | (i % 2) for i in range(CASES)]
    moduli += [m for m in special for _ in range(CASES // 40)]
    with open(path, "w", encoding="utf-8") as f:
        for i, m in enumerate(moduli):
            if m < 2:
                m = 3
            full = rand_value(rng)
            a, b = rng.randrange(m), rng.randrange(m)
            f.write(f"red {full:x} 0 {m:x} {full % m:x}\n")
            f.write(f"add {a:x} {b:x} {m:x} {(a + b) % m:x}\n")
            f.write(f"mul {a:x} {b:x} {m:x} {a * b % m:x}\n")
            f.write(f"pow {full:x} {b:x} {m:x} {pow(full, b, m):x}\n")
            if i >= CASES:  # mô-đun biên: thêm số mũ bất kỳ, có thể lớn hơn mô-đun
                e = rand_value(rng)
                f.write(f"pow {a:x} {e:x} {m:x} {pow(a, e, m):x}\n")


def is_prime(n, rng, rounds=40):
    """Miller-Rabin độc lập với bản C++."""
    if n < 4:
        return n in (2, 3)
    if n % 2 == 0:
        return False
    m, r = n - 1, 0
    while m % 2 == 0:
        m //= 2
        r += 1
    for _ in range(rounds):
        x = pow(rng.randrange(2, n - 1), m, n)
        if x in (1, n - 1):
            continue
        for _ in range(r - 1):
            x = x * x % n
            if x == n - 1:
                break
        else:
            return False
    return True


def run(name, *args):
    """Chạy một chương trình, trả về (trạng thái, nội dung in ra)."""
    path = os.path.join(ROOT, name + EXE)
    if not os.path.exists(path):
        return "CHUA DICH", ""
    res = subprocess.run([path, *args], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    text = res.stdout.decode("utf-8", "replace")
    if res.returncode == 2 and "CHUA CAI" in text:
        return "CHUA CAI", text
    return ("DAT" if res.returncode == 0 else "SAI"), text


def report(name, state, text):
    last = text.strip().splitlines()[-1] if text.strip() else ""
    print(f"{state:<10}{name:<14}{last}")
    if state == "SAI":
        print(text)


def main():
    sys.stdout.reconfigure(encoding="utf-8")
    prime_bits = sys.argv[1] if len(sys.argv) > 1 else "128"
    rng = random.Random(15)
    results = []

    with tempfile.TemporaryDirectory() as tmp:
        data_u512 = os.path.join(tmp, "u512.txt")
        data_modexp = os.path.join(tmp, "modexp.txt")
        write_u512(data_u512, rng)
        write_modexp(data_modexp, rng)
        jobs = [("test_u512", data_u512), ("test_modexp", data_modexp), ("test_random",)]
        for job in jobs:
            state, text = run(*job)
            report(job[0], state, text)
            results.append(state)

    state, text = run("test_prime", prime_bits)
    found = re.search(r"^p=([0-9a-f]+)", text, re.M)
    if state == "DAT" and found:
        p = int(found.group(1), 16)
        if not (is_prime(p, rng) and is_prime((p - 1) // 2, rng) and p.bit_length() == int(prime_bits)):
            state = "SAI"
            text += "\nPython khong xac nhan p la safe prime\n"
    report("test_prime", state, text)
    results.append(state)

    state, text = run("test_dh")
    report("test_dh", state, text)
    results.append(state)

    state, text = run("dh", "--seed", "nhom15")
    if state == "DAT" and not text.strip().endswith("1"):
        state = "SAI"
    report("dh", state, text)
    results.append(state)

    done = sum(1 for s in results if s == "DAT")
    print(f"\nTong: {done}/{len(results)} dat")
    return 1 if "SAI" in results else 0


if __name__ == "__main__":
    sys.exit(main())
