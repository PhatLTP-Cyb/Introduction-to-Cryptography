# Phân chia công việc Lab 01

Đồ án: Số nguyên tố & Trao đổi khóa Diffie-Hellman · Môn Nhập môn Mã hóa – Mật mã, FIT-HCMUS

Tổng quan đồ án: [TongQuan_DoAn.md](TongQuan_DoAn.md) · Khung mã: [project_01_source](project_01_source/README.md)

> **Hạn nộp: 17/11/2026.**

## 1. Thông tin nhóm

**Nhóm 15**

| STT | MSSV | Họ và tên |
|---|---|---|
| 1 | 24120029 | Võ Văn Cường |
| 2 | 24120100 | Tô Tuệ Nghi |
| 3 | 24120113 | Trần Hoàng Nhi |
| 4 | 24120115 | Lê Tấn Phát |

Nhóm trưởng: ........................ 

## 2. Giảng viên hướng dẫn

- [Trần Hải Anh Điền](https://courses.fit.hcmus.edu.vn/user/profile.php?id=21964)
- [Võ Nam Thục Đoan](https://courses.fit.hcmus.edu.vn/user/profile.php?id=7459)
- [Đặng Trần Minh Hậu](https://courses.fit.hcmus.edu.vn/user/profile.php?id=7444)
- [Lê Phúc Lữ](https://courses.fit.hcmus.edu.vn/user/profile.php?id=16185)

Thắc mắc về đồ án gửi cả hai địa chỉ theo đề: nvqhuy@fit.hcmus.edu.vn và ndhy@fit.hcmus.edu.vn

## 3. Bảng nhiệm vụ và yêu cầu đạt được

Đường dẫn tính từ `project_01_source/`. Danh sách hàm của từng nhiệm vụ ở mục 4.

| Mã | Nhiệm vụ | Phụ trách | Kiểm tra chéo | Phụ thuộc | Hạn | Sản phẩm đầu ra | Yêu cầu đạt được |
|---|---|---|---|---|---|---|---|
| NV1 | Kiểu số lớn `uint512_t` | Võ Văn Cường | Trần Hoàng Nhi | Không | 06/11 | `src/u512.cpp` | - Cài đủ 16 hàm ở mục 4.1<br>- Giữ đúng tên kiểu `uint512_t` của mẫu<br>- Đúng ở các biên: 0, 2^512 − 1, số nhớ và số mượn<br>- `test_u512` báo `DAT` (hơn 7.000 kiểm tra, đối chiếu Python) |
| NV2 | Lũy thừa mô-đun | Trần Hoàng Nhi | Lê Tấn Phát | NV1 | 08/11 | `src/modexp.cpp`<br>Số đo thời gian lũy thừa | - Cài đủ 4 hàm ở mục 4.2<br>- `modular_exponentiation` dùng phân tích số mũ thành lũy thừa của 2<br>- Không tràn khi mô-đun đủ 512 bit<br>- Đúng ở các biên: exponent = 0, mod = 1, base ≥ mod<br>- `test_modexp` báo `DAT`<br>- Duyệt bit trái sang phải, gọi `mul_mod(r, base, m)` với cơ số ở tham số thứ hai<br>- Sau 08/11: thêm Montgomery vào `modular_exponentiation`, giữ `mul_mod` cộng-dịch; có nhánh dự phòng và test cho mô-đun chẵn |
| NV3 | Kiểm tra nguyên tố và sinh safe prime | Lê Tấn Phát | Tô Tuệ Nghi | NV1, NV2, NV4 | 11/11 | `src/prime.cpp`<br>Số đo thời gian sinh safe prime | - Cài đủ 3 hàm ở mục 4.3<br>- Miller-Rabin chuẩn, cơ số ngẫu nhiên, đổi được số vòng; không dùng `primeTest` trên slide<br>- Có sàng số nguyên tố nhỏ trước Miller-Rabin<br>- Lọc Fermat cơ số 2 trên cả q và p trước 20 vòng Miller-Rabin<br>- Không dùng thư viện hỗ trợ<br>- `python tests/run_tests.py 512` báo `test_prime` là `DAT`, Python xác nhận p là safe prime |
| NV4 | Ngẫu nhiên và khóa riêng | Tô Tuệ Nghi | Lê Tấn Phát | NV1 | 07/11 | `src/random.cpp` | - Cài đủ 5 hàm ở mục 4.4<br>- Chế độ mặc định lấy seed 32 byte từ hệ điều hành<br>- Sinh số không lệch phân bố, không dùng phép mod<br>- Khóa riêng luôn nằm trong [2, p − 2]<br>- Không dùng `rand()`, `mt19937`, seed theo thời gian<br>- `test_random` báo `DAT` |
| NV5 | Trao đổi khóa Diffie-Hellman | Tô Tuệ Nghi | Võ Văn Cường | NV2, NV3, NV4 | 11/11 | `src/dh.cpp`, `src/main.cpp`<br>Kết quả chạy mẫu cho báo cáo | - Cài `find_generator` ở mục 4.5<br>- `main` in p, g, A, B và hai bí mật chung<br>- Đổi được `bit_size`, tối thiểu 512 bit<br>- Bí mật chung trùng khớp qua ít nhất 10 lần chạy với seed khác nhau<br>- `test_dh` và `dh` báo `DAT` |
| NV6 | Tích hợp, kiểm thử, đóng gói | Võ Văn Cường | Tô Tuệ Nghi | NV1–NV5 | 13/11 | `Makefile`, `tests/`<br>`24120029-24120100-24120113-24120115.zip` | - `mingw32-make test` báo 6/6 `DAT`<br>- Bỏ `-Wno-unused-parameter`, dịch không còn cảnh báo<br>- Dịch được từ tệp zip trên máy Windows khác; dịch thử thêm trên Linux<br>- Tên và cấu trúc zip đúng đề, không kèm `.exe`<br>- Thêm kiểm tra 10 seed vào `tests/run_tests.py`<br>- Chạy toàn bộ test một lần với `LIMBS = 16` |
| NV7 | Báo cáo PDF | Cả nhóm; Trần Hoàng Nhi ghép bản cuối | Cả nhóm | NV1–NV6 | 15/11 | Tệp PDF trong `project_01_report/` | - Đủ 6 mục của đề 2.5, gồm cả mục số ngẫu nhiên để lấy điểm cộng<br>- Mục mô tả chương trình ghi rõ cách cài công cụ và chạy trên Windows<br>- Có thông tin nhóm và bảng mức độ đóng góp<br>- Mọi kết quả và số đo lấy từ lần chạy thật<br>- Có nguồn tham khảo<br>- Chỉ ghi các kích thước số đã chạy thật<br>- Mục tự đánh giá nêu việc g làm lộ tính chẵn lẻ của khóa riêng |

### Phân công theo người

Ai dùng trực tiếp phần nào thì kiểm tra chéo phần đó; NV7 do cả nhóm đọc chéo.

| Nhóm | Người | Nhiệm vụ | Mục báo cáo | Kiểm tra chéo |
|---|---|---|---|---|
| 1 | Võ Văn Cường | NV1, NV6 | 1, 5 | NV5 |
| 2 | Trần Hoàng Nhi | NV2, Montgomery | 6, ghép bản cuối (NV7) | NV1 |
| 3 | Tô Tuệ Nghi | NV4, NV5 | 3, 4 | NV3, NV6 |
| 4 | Lê Tấn Phát | NV3 | 2 | NV2, NV4 |

## 4. Khung mã và danh sách hàm cần làm

Khung mã đã có sẵn trong `project_01_source/`. Mọi hàm dưới đây đã được khai báo, dịch được và có ghi chú `TODO`. Hàm chưa cài sẽ in `CHUA CAI: <tên hàm>` rồi thoát, nên chạy test là biết còn thiếu hàm nào.

Cách làm một hàm:

1. Mở tệp của nhiệm vụ, tìm hàm và đọc ghi chú `TODO`.
2. Xóa dòng `not_implemented(...)`, viết phần cài đặt.
3. Chạy `mingw32-make test` và xem test của nhiệm vụ đó.

Không đổi tên hoặc tham số các hàm trong `src/u512.h`, `src/dh.h` khi chưa báo cả nhóm.

### 4.1. NV1: `src/u512.cpp`

| Xong | Hàm | Việc cần làm |
|---|---|---|
| ☐ | `uint512_t(uint64_t value)` | Tạo số lớn từ số 64 bit |
| ☐ | `from_hex(hex)` | Đọc chuỗi hệ 16 |
| ☐ | `to_hex()` | In hệ 16 chữ thường, bỏ số 0 ở đầu; số 0 là `"0"` |
| ☐ | `is_zero()` | Kiểm tra số 0 |
| ☐ | `is_odd()` | Kiểm tra số lẻ |
| ☐ | `bit(i)` | Lấy bit thứ i |
| ☐ | `set_bit(i)` | Đặt bit thứ i thành 1 |
| ☐ | `bit_length()` | Đếm số bit có nghĩa |
| ☐ | `mod_u64(m)` | Dư khi chia cho số 64 bit |
| ☐ | `add_overflow(a, b, result)` | Cộng, trả về true nếu tràn |
| ☐ | `operator+` | Cộng theo mod 2^512 |
| ☐ | `operator-` | Trừ theo mod 2^512 |
| ☐ | `operator<<` | Dịch trái, bỏ phần tràn |
| ☐ | `operator>>` | Dịch phải |
| ☐ | `operator==` | So sánh bằng |
| ☐ | `operator<` | So sánh nhỏ hơn |

Các phép `!=`, `>`, `<=`, `>=` và in bằng `std::cout` đã viết sẵn dựa trên các hàm trên.

### 4.2. NV2: `src/modexp.cpp`

| Xong | Hàm | Việc cần làm |
|---|---|---|
| ☐ | `reduce_mod(a, m)` | Tính a mod m; giữ lại bit tràn khi nhân đôi phần dư |
| ☐ | `add_mod(a, b, m)` | Tính (a + b) mod m bằng `add_overflow` |
| ☐ | `mul_mod(a, b, m)` | Tính (a · b) mod m bằng phân tích b thành lũy thừa của 2 |
| ☐ | `modular_exponentiation(base, exp, mod)` | Tính lũy thừa bằng bình phương và nhân |

### 4.3. NV3: `src/prime.cpp`

| Xong | Hàm | Việc cần làm |
|---|---|---|
| ☐ | `has_small_factor(n)` | Sàng bằng các số nguyên tố nhỏ hơn 2000 |
| ☐ | `is_probable_prime(ctx, n, rounds)` | Miller-Rabin, mặc định 20 vòng |
| ☐ | `generate_safe_prime(ctx, bitsize)` | Chọn q, đặt p = 2q + 1, sàng rồi kiểm tra cả hai |

### 4.4. NV4: `src/random.cpp`

| Xong | Hàm | Việc cần làm |
|---|---|---|
| ☐ | `seed_from_os(ctx)` | Seed 32 byte từ `randombytes` |
| ☐ | `seed_from_text(ctx, text)` | Seed cố định, chỉ để kiểm thử |
| ☐ | `random_bits(ctx, bits)` | Số ngẫu nhiên trong [0, 2^bits) |
| ☐ | `random_range(ctx, low, high)` | Số ngẫu nhiên trong [low, high], ngoài khoảng thì sinh lại |
| ☐ | `generate_private_key(ctx, p)` | Khóa riêng trong [2, p − 2] |

### 4.5. NV5: `src/dh.cpp`, `src/main.cpp`

| Xong | Hàm | Việc cần làm |
|---|---|---|
| ☐ | `find_generator(ctx, p)` | Tìm g thỏa g² ≠ 1 và g^q ≠ 1 (mod p), với q = (p − 1)/2 |
| ☐ | `main` | Đã viết đủ 5 bước theo mẫu; rà lại nội dung in ra và lấy kết quả chạy cho báo cáo |

### 4.6. NV6: `Makefile`, `tests/`

Không có hàm cần cài. Makefile và bộ kiểm thử đã có sẵn; NV6 giữ chúng chạy đúng khi nhóm thêm mã, và đóng gói bài nộp.

| Lệnh | Tác dụng |
|---|---|
| `mingw32-make` | Dịch ra `dh.exe` |
| `mingw32-make run` | Dịch và chạy chương trình |
| `mingw32-make test` | Chạy toàn bộ test, safe prime 128 bit cho nhanh |
| `python tests/run_tests.py 512` | Chạy lại test với safe prime 512 bit, cần dịch test trước |
| `mingw32-make clean` | Xóa các tệp `.exe` |

## 5. Ràng buộc kỹ thuật và công cụ

Nhóm làm việc trên Windows.

### Ràng buộc kỹ thuật

| Ràng buộc | Nội dung | Nguồn |
|---|---|---|
| Ngôn ngữ | C/C++, phiên bản C++14 trở xuống; luôn dịch với `-std=c++14` | Đề mục 3 |
| Thư viện | Tự cài số lớn và sinh số nguyên tố; không dùng NTL, GMP hay thư viện hỗ trợ tương tự | Đề 2.2 |
| Kích thước | p tối thiểu 512 bit, đổi được trong code bằng `LIMBS` trong `src/u512.h` và `bit_size` trong `src/main.cpp` | Đề 2.3 |
| Tệp nộp | `project_01_source` được chứa `.h`, `.c`, Makefile; Makefile không bắt buộc nhưng nên có | Đã xác nhận |
| Kiến trúc | Dùng g++ 64 bit vì `__uint128_t` không có trên g++ 32 bit và MSVC | Nhóm |
| Ngẫu nhiên | Trên Windows dùng `BCryptGenRandom`, liên kết `-lbcrypt`; đã có sẵn trong khung | Nhóm |
| Tương thích | Giữ cả nhánh `_WIN32` và nhánh Linux, vì chưa rõ bài được chấm trên hệ điều hành nào | Nhóm |
| Mã hóa văn bản | Lưu tệp nguồn UTF-8; `main` đã gọi `SetConsoleOutputCP(CP_UTF8)` để in tiếng Việt | Nhóm |

### Công cụ

| Công cụ | Yêu cầu | Dùng để |
|---|---|---|
| g++ MinGW-w64 | Bản 64 bit; `g++ -dumpmachine` trả về `x86_64-w64-mingw32` | Dịch chương trình |
| `mingw32-make` | Cài cùng MinGW-w64 | Chạy Makefile |
| Python 3 | Không cần thư viện thêm | Sinh dữ liệu và đối chiếu kiểm thử |
| Git | Kho riêng tư | Quản lý mã nguồn |
| WSL Ubuntu | Không bắt buộc, chỉ cần một người có | Dịch thử trên Linux trước khi nộp |

Trình soạn thảo do mỗi người tự chọn. Báo cáo viết chung bằng Word hoặc Google Docs rồi xuất PDF; dùng LaTeX nếu cả nhóm đã quen.

### Dịch không dùng Makefile

Đặt `-lbcrypt` sau các tệp nguồn:

```powershell
g++ -std=c++14 -O3 -I src -I lib/fips202 src/main.cpp src/u512.cpp src/modexp.cpp src/random.cpp src/prime.cpp src/dh.cpp lib/fips202/prng.cpp lib/fips202/fips202.c -o dh.exe -lbcrypt
```

### Khác biệt của khung so với mã mẫu

- `lib/fips202/prng.cpp`: thêm nhánh `BCryptGenRandom` cho Windows, vẫn giữ `getrandom` cho Linux.
- `lib/fips202/prng.h`: bỏ `#include <sys/random.h>`, chuyển sang `prng.cpp`.
- `src/dh_sample.h`, `src/u512_sample.h`: thay bằng `src/dh.h`, `src/u512.h`, có thêm các hàm ở mục 4.
- `Makefile`: viết lại, chạy được trên Windows và Linux.

Mã mẫu gốc trong `prng_sample/` giữ nguyên để đối chiếu, không nộp thư mục này.

## 6. Phân công báo cáo

| Mục theo đề 2.5 | Người viết |
|---|---|
| 1. Hệ thống xử lý số lớn | Võ Văn Cường |
| 2. Hàm kiểm tra số nguyên tố | Lê Tấn Phát |
| 3. Số ngẫu nhiên (điểm cộng) | Tô Tuệ Nghi |
| 4. Trao đổi khóa Diffie-Hellman và ứng dụng thực tế | Tô Tuệ Nghi |
| 5. Mô tả chương trình đã cài đặt | Võ Văn Cường |
| 6. Tự đánh giá | Trần Hoàng Nhi |

## 7. Kế hoạch theo mốc

Nên nộp từ ngày 16/11, không để sát hạn 17/11/2026.

| Thời gian | Công việc | Kết quả cần có |
|---|---|---|
| 03–04/11 | Họp, phân công, dựng kho mã, cài công cụ | Mọi người chạy được `mingw32-make test` trên máy mình |
| 05–08/11 | NV1, NV4, NV2 | Lũy thừa khớp Python trên số 512 bit |
| 09–11/11 | NV3, NV5, thêm Montgomery vào NV2 | Sinh được p, bí mật chung trùng khớp |
| 12–13/11 | NV6, sửa lỗi | `mingw32-make run` chạy được trên máy Windows khác |
| 14–15/11 | NV7, đọc chéo báo cáo | Bản PDF hoàn chỉnh |
| 16/11 | Dự phòng, đóng gói, nộp bài | Tệp zip đã giải nén thử và nộp |
| 17/11 | Hạn nộp | Chỉ dùng khi có sự cố |

## 8. Quy ước làm việc

- **Môi trường:** mọi người làm trên Windows với công cụ ở mục 5.
- **Mã nguồn:** dùng kho Git riêng tư, mỗi thành viên một nhánh, mỗi nhiệm vụ một pull request.
- **Nhập mã:** người kiểm tra chéo đọc và chạy kiểm thử trước khi nhập vào nhánh chính.
- **Hoàn thành nhiệm vụ:** chỉ tính là xong khi đạt hết yêu cầu ở mục 3 và đã qua kiểm tra chéo.
- **Báo tiến độ:** cập nhật bảng ở mục 9 sau mỗi mốc; báo sớm cho nhóm nếu trễ quá 1 ngày.
- **Dùng tài liệu:** đối chiếu mọi mã giả từ slide bằng kiểm thử trước khi dùng.

Kênh liên lạc: ........................ · Lịch họp định kỳ: ........................

## 9. Theo dõi tiến độ

| Mã | Trạng thái | Ngày xong | Ghi chú |
|---|---|---|---|
| NV1 | Chưa làm | | |
| NV2 | Chưa làm | | |
| NV3 | Chưa làm | | |
| NV4 | Chưa làm | | |
| NV5 | Chưa làm | | |
| NV6 | Chưa làm | | |
| NV7 | Chưa làm | | |

## 10. Rủi ro và phương án xử lý

| Rủi ro | Phương án xử lý | Người theo dõi |
|---|---|---|
| NV1 trễ làm kẹt NV2, NV3, NV4 | Ưu tiên các hàm lõi trước; đưa thêm người hỗ trợ NV1 | |
| Sinh safe prime quá chậm | Đo sớm, tăng phạm vi sàng, chuyển sang Montgomery | |
| Bài chạy được trên máy nhóm nhưng lỗi trên máy chấm | Dịch lại từ zip trên Windows và Linux, xác nhận môi trường chấm | |
| Báo cáo làm dồn vào cuối | Viết song song với code, nháp mục của mình trước 13/11 | |

## 11. Danh sách kiểm tra trước khi nộp

- [ ] Tên tệp là `24120029-24120100-24120113-24120115.zip`, đã kiểm tra thứ tự MSSV theo danh sách lớp
- [ ] Có `project_01_source` và `project_01_report`
- [ ] Đã chạy `mingw32-make clean`, không kèm tệp `.exe`, `.o` hay thư mục Git
- [ ] Giải nén và dịch thử từ tệp zip thành công với `-std=c++14` trên Windows
- [ ] Dịch thử từ tệp zip trên Linux
- [ ] `mingw32-make test` báo 6/6 `DAT`, và `python tests/run_tests.py 512` đạt
- [ ] Chương trình dùng seed từ hệ điều hành ở chế độ mặc định
- [ ] Báo cáo là PDF, đủ 6 mục, có bảng đóng góp
- [ ] Số liệu trong báo cáo khớp với bản mã nộp

## 12. Mức độ đóng góp

Điền khi kết thúc đồ án. Tổng cột "Mức đóng góp" là 100%.

| MSSV | Họ và tên | Công việc đã làm | Mức đóng góp |
|---|---|---|---|
| 24120029 | Võ Văn Cường | | |
| 24120100 | Tô Tuệ Nghi | | |
| 24120113 | Trần Hoàng Nhi | | |
| 24120115 | Lê Tấn Phát | | |
