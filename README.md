# Lab 01: Số nguyên tố & Trao đổi khóa Diffie-Hellman

Đồ án môn Nhập môn Mã hóa – Mật mã, FIT-HCMUS. Nhóm 15. **Hạn nộp: 17/11/2026.**

Chương trình C++ mô phỏng Alice và Bob tạo ra cùng một bí mật chung qua kênh công khai bằng giao thức Diffie-Hellman. Nhóm tự cài số nguyên lớn, lũy thừa mô-đun, kiểm tra nguyên tố Miller-Rabin và sinh số nguyên tố an toàn ít nhất 512 bit, không dùng thư viện số lớn.

## Trạng thái

Khung mã đã dịch được trên Windows và Linux. Nhóm còn 29 hàm phải cài, nên hiện tại chương trình dừng ở hàm đầu tiên chưa cài và báo:

```text
CHUA CAI: seed_from_os
```

Danh sách hàm và yêu cầu của từng nhiệm vụ nằm trong [PhanCongCongViec.md](PhanCongCongViec.md).

## Nhóm 15

- Võ Văn Cường - 24120029
- Tô Tuệ Nghi - 24120100
- Trần Hoàng Nhi - 24120113
- Lê Tấn Phát - 24120115

Giảng viên hướng dẫn: Trần Hải Anh Điền, Võ Nam Thục Đoan, Đặng Trần Minh Hậu, Lê Phúc Lữ.

## Cấu trúc kho

```text
.
├── README.md
├── Lab01.pdf                         Đề bài
├── TongQuan_DoAn.md                  Tổng quan đồ án và kiến thức nền
├── PhanCongCongViec.md               Nhiệm vụ, yêu cầu, danh sách hàm, kế hoạch
├── BienBanHop_PhanCong_Lab01.docx    Biên bản họp và phân công
├── prng_sample/                      Mã mẫu gốc của đề, chỉ để đối chiếu
└── project_01_source/                Mã nguồn của nhóm
    ├── Makefile
    ├── src/
    │   ├── u512.h, u512.cpp          NV1: kiểu số lớn uint512_t
    │   ├── modexp.cpp                NV2: nhân và lũy thừa mô-đun
    │   ├── prime.cpp                 NV3: Miller-Rabin, sinh safe prime
    │   ├── random.cpp                NV4: seed, số ngẫu nhiên, khóa riêng
    │   ├── dh.cpp, main.cpp          NV5: chọn g, trao đổi khóa
    │   ├── dh.h                      Khai báo các hàm dùng chung
    │   └── common.h                  Báo hàm chưa cài
    ├── lib/fips202/                  SHAKE128 và PRNG, đã chạy được trên Windows
    └── tests/                        Test C++ và run_tests.py đối chiếu bằng Python
```

## Yêu cầu

| Công cụ | Yêu cầu |
|---|---|
| g++ | MinGW-w64 bản 64 bit trên Windows, hỗ trợ C++14 |
| make | `mingw32-make` trên Windows, `make` trên Linux |
| Python | Python 3, không cần thư viện thêm, chỉ dùng để kiểm thử |

Kiểm tra g++ là bản 64 bit:

```powershell
g++ -dumpmachine
```

Kết quả trên Windows phải là `x86_64-w64-mingw32`. Không dùng g++ 32 bit hoặc MSVC vì mã cần `__uint128_t`.

## Cách chạy

```powershell
git clone https://github.com/PhatLTP-Cyb/Introduction-to-Cryptography.git
cd Introduction-to-Cryptography/project_01_source
mingw32-make
mingw32-make run
```

Trên Linux, dùng `make` thay cho `mingw32-make`.

Khi còn hàm chưa cài, `mingw32-make run` sẽ báo `CHUA CAI` kèm dòng `Error 2`. Đây là hành vi bình thường của khung.

| Lệnh | Tác dụng |
|---|---|
| `mingw32-make` | Dịch ra `dh.exe` |
| `mingw32-make run` | Dịch và chạy chương trình, seed lấy từ hệ điều hành |
| `.\dh.exe --seed abc` | Chạy với seed cố định trong PowerShell, chỉ dùng khi kiểm thử |
| `mingw32-make test` | Dịch và chạy toàn bộ test, safe prime 128 bit cho nhanh |
| `python tests/run_tests.py 512` | Chạy lại test với safe prime 512 bit, cần chạy `mingw32-make test` trước |
| `mingw32-make clean` | Xóa các tệp `.exe` |

Dịch không dùng Makefile:

```powershell
g++ -std=c++14 -O3 -I src -I lib/fips202 src/main.cpp src/u512.cpp src/modexp.cpp src/random.cpp src/prime.cpp src/dh.cpp lib/fips202/prng.cpp lib/fips202/fips202.c -o dh.exe -lbcrypt
```

## Kiểm thử

`mingw32-make test` báo một trong ba trạng thái cho từng phần:

| Trạng thái | Ý nghĩa |
|---|---|
| `DAT` | Qua hết kiểm tra |
| `SAI` | Có kiểm tra sai, chi tiết in ngay bên dưới |
| `CHUA CAI` | Còn hàm chưa cài, kèm tên hàm |

Bộ test đối chiếu số lớn và lũy thừa với Python trên số ngẫu nhiên, thử Miller-Rabin với các hợp số như 561, 1729, 2047, và kiểm tra lại safe prime bằng Miller-Rabin viết riêng trong Python. Mục tiêu là 6/6 `DAT`.

## Đổi kích thước số

1. Đổi `LIMBS` trong `project_01_source/src/u512.h`: 8 là 512 bit, 16 là 1024 bit, 32 là 2048 bit.
2. Đổi `bit_size` trong `project_01_source/src/main.cpp`.

## Quy trình làm việc

Mỗi thành viên làm trên một nhánh riêng:

```powershell
git switch main
git pull
git switch -c <Ten>
```

Cách cài một hàm:

1. Mở tệp của nhiệm vụ, tìm hàm và đọc ghi chú `TODO`.
2. Xóa dòng `not_implemented(...)`, viết phần cài đặt.
3. Chạy `mingw32-make test` và xem test của nhiệm vụ đó.

Khi xong, đẩy nhánh lên và tạo pull request. Người kiểm tra chéo đọc mã, chạy test rồi mới nhập vào `main`. Không đổi tên hoặc tham số các hàm trong `src/u512.h`, `src/dh.h` khi chưa báo cả nhóm.

## Giới hạn

- Đây là bài tập học tập, không dùng cho hệ thống thật. Thực tế cần số nguyên tố từ 2048 bit trở lên.
- Diffie-Hellman thuần không xác thực, nên có thể bị tấn công man-in-the-middle.
- Alice và Bob được mô phỏng trong một chương trình, không truyền qua mạng.
