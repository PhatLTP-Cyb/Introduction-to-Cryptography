# Tổng quan đồ án Lab 01: Số nguyên tố & Trao đổi khóa Diffie-Hellman

Môn Nhập môn Mã hóa – Mật mã, FIT-HCMUS · Nhóm 15 · **Hạn nộp: 17/11/2026**

Tài liệu gốc: [Lab01.pdf](Lab01.pdf) · Phân công: [PhanCongCongViec.md](PhanCongCongViec.md) · Khung mã: [project_01_source](project_01_source/README.md)

## 1. Đồ án làm gì

Nhóm viết một chương trình C/C++ để Alice và Bob tạo ra cùng một bí mật chung, dù mọi giá trị trao đổi đều công khai. Chương trình phải tự sinh số nguyên tố an toàn ít nhất 512 bit và tự xử lý số lớn.

Bốn hàm chính theo đề:

| Hàm | Nhiệm vụ |
|---|---|
| `modular_exponentiation` | Tính `base^exponent mod mod` hiệu quả |
| `generate_safe_prime` | Sinh p sao cho p và (p − 1)/2 đều nguyên tố |
| `generate_private_key` | Sinh khóa riêng ngẫu nhiên trong [2, p − 2] |
| `main` | Thực hiện trao đổi khóa và xác minh bí mật chung |

## 2. Nguyên lý Diffie-Hellman

Ví dụ với p = 23, g = 5:

| Bước | Alice | Công khai | Bob |
|---|---|---|---|
| Chọn khóa riêng | a = 4 | | b = 3 |
| Tính khóa công khai | A = 5⁴ mod 23 = 4 | A = 4, B = 10 | B = 5³ mod 23 = 10 |
| Tính bí mật chung | s = 10⁴ mod 23 = 18 | | s = 4³ mod 23 = 18 |

Hai bên ra cùng kết quả vì `(g^b)^a = (g^a)^b = g^(ab) mod p`. Người nghe lén muốn tính s phải tìm a từ `g^a mod p`. Đây là bài toán logarit rời rạc, rất khó khi p đủ lớn.

## 3. Kiến thức cần dùng

| Kiến thức | Dùng ở đâu | Nguồn trong tài liệu môn học |
|---|---|---|
| Lũy thừa bằng phân tích số mũ thành lũy thừa của 2 | `modular_exponentiation` | Bài 2 (`powerMod`), gợi ý seminar |
| Miller-Rabin | Kiểm tra nguyên tố | Bài 2, gợi ý seminar (xem mục 9) |
| Số nguyên tố an toàn | `generate_safe_prime` | Đề 2.2 |
| Nhóm tuần hoàn, phần tử sinh | Chọn g | Bài 2 |
| Số ngẫu nhiên an toàn, PRNG | Sinh khóa và số nguyên tố | Mã mẫu, gợi ý seminar |

## 4. Các thành phần và quan hệ phụ thuộc

Làm từ trên xuống theo bảng này. Mỗi thành phần chỉ dùng các thành phần phía trên nó.

| Thứ tự | Thành phần | Dùng |
|---|---|---|
| 1 | Kiểu số lớn `uint512_t` | Không phụ thuộc |
| 2 | PRNG SHAKE128 | Có sẵn trong mã mẫu |
| 3 | Sinh số ngẫu nhiên lớn | `uint512_t`, PRNG |
| 4 | `mulMod`, `modular_exponentiation` | `uint512_t` |
| 5 | Miller-Rabin | Lũy thừa mô-đun, số ngẫu nhiên |
| 6 | `generate_safe_prime` | Miller-Rabin, số ngẫu nhiên |
| 7 | `generate_private_key` | Số ngẫu nhiên |
| 8 | Chọn g và `main` | Tất cả phần trên |

## 5. Mã mẫu

Nhóm làm trên khung mã `project_01_source/`, xây từ mã mẫu trong `prng_sample/`.

| Đã có trong khung | Nhóm phải viết |
|---|---|
| SHAKE128 và PRNG trong `lib/fips202/`, đã chạy được trên Windows | 16 hàm của `uint512_t` |
| Khai báo mọi hàm trong `src/u512.h`, `src/dh.h` | 4 hàm lũy thừa mô-đun |
| Hàm `main` đủ 5 bước theo mẫu | 3 hàm kiểm tra nguyên tố và sinh safe prime |
| Makefile và bộ kiểm thử trong `tests/` | 5 hàm ngẫu nhiên và khóa riêng, hàm chọn g |

Danh sách hàm chi tiết ở mục 4 của [PhanCongCongViec.md](PhanCongCongViec.md).

Các vấn đề của mã mẫu gốc, đã xử lý trong khung:

- Không dịch được bằng MinGW trên Windows vì thiếu `sys/random.h`. Khung thêm nhánh `BCryptGenRandom`.
- `Makefile` chưa đặt `-std=c++14`. Khung đã thêm cờ này.
- `src/main_prng.cpp` chỉ ghi 5 byte vào `seed[32]`, phần còn lại chưa khởi tạo. Hàm seed trong khung phải khởi tạo đủ 32 byte.
- `Makefile` có nhắc NTL và GMP. Khung đã bỏ, vì đề cấm thư viện hỗ trợ cho phần sinh số nguyên tố.

## 6. Hướng kỹ thuật đã chốt

| Vấn đề | Quyết định |
|---|---|
| Số lớn | `struct uint512_t` với hằng `LIMBS`, đổi 8 thành 16 để có 1024 bit |
| Tính số mũ lớn | Phân tích số mũ thành lũy thừa của 2, duyệt bit trái sang phải |
| Nhân mô-đun | Cộng-dịch trong `mul_mod` trước, nâng cấp Montgomery nếu kịp (12–13/11) |
| Kiểm tra nguyên tố | Lọc Fermat cơ số 2 trước, sau đó 20 vòng Miller-Rabin cơ số ngẫu nhiên |
| Sinh safe prime | Sàng số nguyên tố nhỏ hơn 2000 trên q và 2q + 1 trước khi kiểm tra; kiểm tra p bằng định lý Pocklington |
| Chọn g | Tìm g thỏa g² ≠ 1 và g^q ≠ 1 (mod p), với q = (p − 1)/2 |
| Ngẫu nhiên | Seed từ hệ điều hành, sinh bằng SHAKE128; seed cố định chỉ để kiểm thử |

Số đo tham khảo từ một bản cài đặt thử dùng `mul_mod` cộng-dịch, không nằm trong khung: cả chương trình 512 bit chạy trong 0,6–6,8 giây qua 3 lần trên Windows.

Số đo thêm từ một bản thử khác, cũng không nằm trong khung, chạy trên Windows với GCC 16.2. Mỗi ô là thời gian sinh safe prime trung bình, trong ngoặc là thấp nhất và cao nhất, kèm số lần chạy:

| Cách làm | 512 bit | 1024 bit | 2048 bit |
|---|---|---|---|
| Cộng-dịch, sàng, 20 vòng | 26 giây (1–86), 20 lần | Chưa đo | Chưa đo |
| Cộng-dịch, sàng, Fermat | 11 giây (1–35), 20 lần | 308 giây (138–467), 3 lần | Khoảng 1–2 giờ, ước tính |
| Montgomery, sàng, Fermat | 0,6 giây (0–2,5), 200 lần | 6,6 giây (0,5–16), 20 lần | 104 giây (37–274), 5 lần |

Thời gian dao động mạnh vì số ứng viên phải thử là ngẫu nhiên. Nhiều phép đo chạy cùng lúc nên máy rảnh có thể nhanh hơn. Cộng-dịch đủ dùng cho 512 bit và Montgomery chỉ là nâng cấp, còn 2048 bit gần như chỉ khả thi với Montgomery. Số liệu trong báo cáo phải đo lại trên mã thật.

## 7. Môi trường và cách chạy

Nhóm làm việc trên Windows, dùng g++ MinGW-w64 64 bit và `mingw32-make`. Chạy các lệnh sau từ thư mục `project_01_source`:

```powershell
mingw32-make
mingw32-make run
mingw32-make test
mingw32-make clean
```

Trên Linux, dùng `make` thay cho `mingw32-make`. Mã giữ nhánh Linux vì chưa rõ bài được chấm trên hệ điều hành nào.

`mingw32-make test` tự làm các việc sau và báo `DAT`, `SAI` hoặc `CHUA CAI` cho từng phần:

- Đối chiếu số lớn và lũy thừa với Python trên số 512 bit ngẫu nhiên.
- Thử Miller-Rabin với hợp số 561, 1729, 2047, 41041, 3215031751 và số nguyên tố 83, 107, 2^127 − 1.
- Kiểm tra lại p và (p − 1)/2 bằng Miller-Rabin viết riêng trong Python.
- Chạy chương trình `dh` và xác nhận hai bí mật chung trùng khớp.

Lệnh này sinh safe prime 128 bit cho nhanh. Dùng `python tests/run_tests.py 512` để kiểm tra đúng kích thước của đề.

## 8. Quy định nộp bài

- Hạn nộp: 17/11/2026. Nhóm tối đa 4 sinh viên.
- Dùng C/C++, phiên bản C++14 trở xuống.
- `project_01_source` được chứa `.h`, `.c`, Makefile. Makefile không bắt buộc nhưng nên có.
- Phần không biên dịch được hoặc bị lỗi thì không có điểm.
- Báo cáo PDF gồm 6 mục: hệ thống số lớn, kiểm tra nguyên tố, số ngẫu nhiên (không bắt buộc, tính điểm cộng), ứng dụng thực tế của Diffie-Hellman, mô tả chương trình, tự đánh giá.

Cấu trúc nộp (MSSV xếp theo thứ tự danh sách lớp):

```text
24120029-24120100-24120113-24120115.zip
├── project_01_source/    (src, lib, tests, Makefile, README.md)
└── project_01_report/    (báo cáo PDF)
```

Không nộp `prng_sample/`, tệp `.exe` và các file tài liệu nội bộ của nhóm.

## 9. Lưu ý về slide

Không chép nguyên các mã giả sau từ Bài 2:

| Trang PDF | Nội dung | Vấn đề |
|---|---|---|
| 25 | `gcd` nhị phân | Vòng lặp không dừng, cần đổi điều kiện thành `x >= y` |
| 29–30 | `primeTest` | Loại nhầm số nguyên tố, ví dụ 17 với b = 2 |
| 47 | `generator` | Có thể nhận g không phải phần tử sinh, ví dụ p = 13, g = 5 |

Miller-Rabin chuẩn: viết n − 1 = 2^r · m với m lẻ. Số n qua vòng kiểm tra khi b^m ≡ ±1, hoặc b^(2^k · m) ≡ −1 với 1 ≤ k < r.

## 10. Giới hạn của đồ án

- 512 bit chỉ dùng để học. Thực tế cần từ 2048 bit trở lên.
- Diffie-Hellman thuần không xác thực, nên có thể bị tấn công man-in-the-middle.
- Alice và Bob được mô phỏng trong một chương trình, đề không yêu cầu truyền qua mạng.

## 11. Trạng thái xác nhận

| Câu hỏi | Trạng thái |
|---|---|
| Hạn nộp | Đã xác nhận: 17/11/2026 |
| Makefile | Đã xác nhận: không bắt buộc nhưng nên có |
| `project_01_source` có được chứa `.h`, `.c`, Makefile không | Đã xác nhận: được chứa |
| Bài được chấm trên Linux hay Windows | Chưa rõ, cần gửi mail hỏi. Trong lúc chờ, bắt buộc dịch thử trên Linux |

Câu hỏi còn lại gửi tới cả hai địa chỉ theo đề: nvqhuy@fit.hcmus.edu.vn và ndhy@fit.hcmus.edu.vn.

## 12. Tài liệu tham khảo do đề đưa ra

- Diffie-Hellman: https://www.math.auckland.ac.nz/~sgal018/crypto-book/ch20.pdf (đề in thiếu dấu `~`, dấu này do nhóm suy ra, cần mở thử)
- Số nguyên tố an toàn: https://planetmath.org/SafePrime
- Miller-Rabin: https://crypto.stanford.edu/pbc/notes/numbertheory/millerrabin.html
- Cài đặt Diffie-Hellman: https://www.geeksforgeeks.org/implementation-diffie-hellman-algorithm/
- Thư viện chuẩn C++: https://en.cppreference.com/w/cpp/standard_library
