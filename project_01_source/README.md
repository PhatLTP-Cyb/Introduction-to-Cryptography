# Lab 01: Số nguyên tố & Trao đổi khóa Diffie-Hellman (Nhóm 15)

Hạn nộp: 17/11/2026. Khung mã đã dịch được, các hàm chưa cài sẽ in `CHUA CAI: <tên hàm>` rồi thoát.

## Cấu trúc

| Đường dẫn | Nội dung | Nhiệm vụ |
|---|---|---|
| `src/u512.h`, `src/u512.cpp` | Kiểu số lớn `uint512_t` | NV1 |
| `src/modexp.cpp` | Nhân và lũy thừa mô-đun | NV2 |
| `src/prime.cpp` | Miller-Rabin, sinh safe prime | NV3 |
| `src/random.cpp` | Seed, số ngẫu nhiên, khóa riêng | NV4 |
| `src/dh.cpp`, `src/main.cpp` | Chọn g, trao đổi khóa | NV5 |
| `src/dh.h` | Khai báo các hàm dùng chung | Cả nhóm |
| `lib/fips202/` | SHAKE128 và PRNG, đã sửa để chạy trên Windows | Có sẵn |
| `tests/`, `Makefile` | Kiểm thử và build | NV6 |

## Cách dịch và chạy

Trên Windows, cần g++ MinGW-w64 64 bit, `mingw32-make` và Python 3:

```powershell
mingw32-make
mingw32-make run
mingw32-make test
mingw32-make clean
```

Trên Linux, dùng `make` thay cho `mingw32-make`.

Không dùng Makefile:

```powershell
g++ -std=c++14 -O3 -I src -I lib/fips202 src/main.cpp src/u512.cpp src/modexp.cpp src/random.cpp src/prime.cpp src/dh.cpp lib/fips202/prng.cpp lib/fips202/fips202.c -o dh.exe -lbcrypt
```

## Kiểm thử

`mingw32-make test` chạy tất cả test và đối chiếu với Python. Mỗi test báo `DAT`, `SAI` hoặc `CHUA CAI`.

Kiểm tra safe prime đúng kích thước của đề:

```powershell
python tests/run_tests.py 512
```

## Đổi kích thước số

Đổi `LIMBS` trong `src/u512.h` (16 là 1024 bit) và `bit_size` trong `src/main.cpp`.

## Khi nộp

Không đưa tệp `.exe` vào zip. Chạy `mingw32-make clean` trước khi nén.
