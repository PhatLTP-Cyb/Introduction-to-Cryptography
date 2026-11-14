# GUIDE NV3: MILLER-RABIN VÀ SINH SỐ NGUYÊN TỐ AN TOÀN (SAFE PRIME)

> Dựa trên `project_01_source/src/prime.cpp` và `src/dh.h` ở commit `0720eff` (main). Mọi `file:dòng` là của commit này; main đổi thì dòng có thể lệch.
> Đối tượng: thành viên nhóm **chưa viết NV3**. Mục tiêu kép: **hiểu vì sao** code viết như vậy và **tự làm lại được** phần này.
> Người viết: Nhi. Người đọc chéo: Võ Văn Cường (xem mục 13).
> Nối tiếp NV1 (`uint512_t`) và NV2 (`modular_exponentiation`): NV3 là nơi hai phần đó được dùng.

---

## 0. TỔNG QUAN

### 0.1. Câu chuyện xuyên suốt

> *"Đề yêu cầu một số nguyên tố p dài 512 bit mà (p-1)/2 cũng nguyên tố. Ta không có thư viện sinh sẵn, và kiểm tra một số 512 bit rất tốn. Vậy ta phải trả lời 4 câu hỏi:*
> 1. *Làm sao biết một số **có phải nguyên tố** không? (**Miller-Rabin**)*
> 2. *Làm sao **không phí công** với các số hiển nhiên là hợp số? (**Sàng số nhỏ**)*
> 3. *Làm sao **sinh ứng viên** sao cho cả q và p = 2q+1 cùng có cơ hội? (**Sinh q, sàng kết hợp**)*
> 4. *Làm sao **xếp các bước lọc** từ rẻ đến đắt? (**generate_safe_prime**)"*

### 0.2. Sơ đồ tổng thể

```
generate_safe_prime (prime.cpp:229)
  lặp:
    generate_candidate_q (106)              -> q lẻ, đúng bitsize-1 bit; p = 2q+1
    has_small_factor_for_safe_prime (123)   -> loại nếu q hoặc p có ước nhỏ < 20000
    passes_base_two_round(q) (98)           -> Miller-Rabin cơ số 2 cho q
    passes_base_two_and_certifies(p) (152)  -> cơ số 2 cho p, kèm gcd(3,p)=1
    is_probable_prime(q,20), (p,20) (182)   -> 20 vòng cơ số ngẫu nhiên
    đạt hết -> return p
```

Luồng tính toán nặng: `generate_safe_prime -> is_probable_prime -> miller_rabin_test -> modular_exponentiation`.
Các lớp lọc xếp **từ rẻ đến đắt**: chia số nhỏ, một vòng cơ số 2, rồi mới tới 20 vòng ngẫu nhiên.

### 0.3. Mục tiêu (đo được bằng hành vi cụ thể)

| # | Sau khi đọc, bạn có thể… | Mức độ | Kiểm tra bằng |
|---|--------------------------|--------|---------------|
| M1 | Giải thích vì sao Fermat không đủ còn Miller-Rabin loại được số Carmichael | Hiểu | Câu tự kiểm 1 (mục 1) |
| M2 | Tách n-1 = 2^r·m bằng tay và bằng `decompose_even` | Áp dụng | Bài tập 1 |
| M3 | Chỉ ra vì sao một vòng Miller-Rabin gọi `mul_mod(x, x, n)` tối đa r-1 lần | Hiểu | Bước 2 |
| M4 | Giải thích điều kiện sàng kết hợp cho q và p = 2q+1 | Phân tích | Bài tập 2 |
| M5 | Giải thích vì sao vòng cơ số 2 của p chỉ cần kiểm x ∈ {1, p-1} | Phân tích | Bước 5 |
| M6 | Nói đúng **tiền điều kiện** và **giới hạn** của chứng nhận Pocklington trong code | Đánh giá | Bài tập 3 |
| M7 | Dịch và chạy `test_prime` rồi đọc kết quả | Áp dụng | Mục 10 |
| M8 | Tự viết lại bản của mình cho các hàm trên mà không nhìn đáp án | Vận dụng | Mục 12 |

### 0.4. Kiến thức nền cần có

| # | Kiến thức nền | Dùng ở đâu |
|---|---------------|------------|
| 1 | **Số nguyên tố, hợp số**; ước nguyên tố nhỏ | Bước 1 |
| 2 | **Đồng dư (mod)**: a ≡ b (mod n) | Mọi bước |
| 3 | **Định lý nhỏ Fermat**: a^(p-1) ≡ 1 (mod p) | Bước 2, 5 |
| 4 | **Số nguyên tố an toàn**: p = 2q+1 với q nguyên tố | Bước 4 đến 6 |
| 5 | **Lũy thừa mô-đun** (NV2) và **nhân mô-đun** `mul_mod` | Bước 2, 3, 5 |
| 6 | **Xác suất độc lập**: k lần độc lập thì nhân xác suất | Bước 3 |

Tài liệu và vị trí đọc:

| Tài liệu | Slide/trang/mục | Cần hiểu |
|----------|-----------------|----------|
| Đề Lab01 (`nhi-local/docs`) | (điền sau khi mở) | Safe prime; p tối thiểu 512 bit; không dùng thư viện sinh nguyên tố |
| Slide bài 2 (`nhi-local/docs`) | (điền sau khi mở) | Số nguyên tố, Fermat nhỏ |
| Báo cáo mục 2 (`report-latex/sections/02-kiem-tra-nguyen-to.tex`) | 2.1 đến 2.4 | Fermat, Carmichael, Miller-Rabin, safe prime |

### 0.5. Ví dụ xuyên suốt (chỉ cần nhớ 3 số)

1. **n = 341 = 11 x 31**: hợp số nhưng qua Fermat cơ số 2. Dùng để thấy Fermat yếu.
2. **n = 561 = 3 x 11 x 17**: số Carmichael nhỏ nhất. Dùng để thấy Miller-Rabin mạnh hơn.
3. **p = 23, q = 11**: safe prime nhỏ để tự tính tay mọi bước sinh số.

### 0.6. Quy trình vận dụng 4 bước (sản phẩm bạn mang về) ⭐

| Bước | Việc phải làm | Đọc ở |
|------|---------------|-------|
| **1. Loại rẻ** | Chia thử cho số nguyên tố nhỏ | Bước 1, `prime.cpp:166` |
| **2. Kiểm thuần** | Một vòng Miller-Rabin đúng, tách n-1 | Bước 2 và 3, `prime.cpp:49-94, 182` |
| **3. Sinh ứng viên** | q lẻ đủ bit, p = 2q+1, sàng chung | Bước 4, `prime.cpp:106-136` |
| **4. Ghép và kiểm** | Xếp lọc rẻ đến đắt, đủ 20 vòng | Bước 5 và 6, `prime.cpp:96-162, 229` |

### 0.7. Chuẩn bị

- Nhánh sạch từ `0720eff`; mở sẵn `project_01_source/src/prime.cpp` và `src/dh.h` có hiện số dòng.
- Máy Windows có `g++` (MinGW-w64) ở `C:\msys64\mingw64\bin`, PowerShell 5.1.
- Giấy nháp để tính tay ví dụ 341, 561, 23.

### 0.8. Lộ trình

| Phần | Nội dung | Dòng code | Thời gian dự kiến |
|------|----------|-----------|-------------------|
| Bước 1 | Sàng số nhỏ | 9-37, 166-179 | 15' |
| Bước 2 | Tách n-1, một vòng MR | 39-94 | 25' |
| Bước 3 | `is_probable_prime` | 182-226 | 15' |
| Bước 4 | Sàng kết hợp q và p | 106-136 | 20' |
| Bước 5 | Cơ số 2 và Pocklington | 96-103, 138-162 | 20' |
| Bước 6 | `generate_safe_prime` | 228-263 | 15' |
| Tổng hợp | Chạy thử, bài tập, tự kiểm | | 30' |

Thời gian là **ước lượng của người soạn, chưa đo** trên người đọc thật.

---

## 1. KIẾN THỨC NỀN, TỰ KIỂM TRA TRƯỚC KHI ĐỌC CODE

> **Tư duy nguyên bản:** Ta cần một phép kiểm "chắc chắn khi nói *hợp số*, xác suất khi nói *nguyên tố*", chạy nhanh với số 512 bit. Hãy xuất phát từ câu hỏi: **"Tính chất nào mà mọi số nguyên tố đều có, còn hợp số thì thường không?"**

**📦 Hộp nền tảng, Fermat nhỏ:** nếu p nguyên tố và gcd(a, p) = 1 thì a^(p-1) ≡ 1 (mod p). Chiều ngược lại **không đúng**: 341 là hợp số mà 2^340 ≡ 1 (mod 341).

**📦 Hộp nền tảng, căn bậc hai của 1:** với p nguyên tố lẻ, x² ≡ 1 (mod p) chỉ có hai nghiệm x ≡ ±1. Hợp số lẻ có từ hai ước nguyên tố lẻ trở lên thì có thêm nghiệm khác, gọi là **căn bậc hai không tầm thường**. Miller-Rabin săn đúng thứ này.

**Tự kiểm tra (làm trước, đáp án ở mục 12):**
1. Vì sao Fermat không đủ, còn Miller-Rabin loại được số Carmichael?
2. Vì sao n-1 = 2^r·m với m lẻ luôn tách được khi n lẻ?
3. Safe prime p thì p-1 có những ước nguyên tố nào?
4. Vì sao cơ số b chọn trong [2, n-2]?
5. Sau k vòng độc lập, xác suất sai tối đa là bao nhiêu?

---

## 2. BẢN ĐỒ ĐỌC CODE

Đọc theo thứ tự này, trong `project_01_source/src/prime.cpp`:

| Thứ tự | Hàm | Dòng | Vì sao đọc ở đây |
|--------|-----|------|------------------|
| 1 | `build_small_primes`, `small_primes` | 14, 33 | Công cụ nhỏ nhất, không phụ thuộc gì |
| 2 | `decompose_even` | 49 | Cần trước mọi vòng Miller-Rabin |
| 3 | `miller_rabin_from_first_power`, `miller_rabin_test` | 66, 91 | Lõi của thuật toán |
| 4 | `has_small_factor`, `is_probable_prime` | 166, 182 | Hàm công khai, dùng lại các hàm trên |
| 5 | `generate_candidate_q`, `has_small_factor_for_safe_prime` | 106, 123 | Sinh và sàng ứng viên |
| 6 | `two_to_half_exponent_mod`, `passes_base_two_round`, `passes_base_two_and_certifies` | 139, 98, 152 | Lọc rẻ bằng cơ số 2 |
| 7 | `generate_safe_prime` | 229 | Ghép tất cả |

Chữ ký công khai nằm ở `src/dh.h`: `is_probable_prime` dòng 25, `generate_safe_prime` dòng 26. Hàm phụ nằm trong `namespace` ẩn của `prime.cpp` (dòng 7 đến 163), nên không gọi được từ file khác.

---

## 3. BƯỚC 1: SÀNG SỐ NGUYÊN TỐ NHỎ

> **Tư duy nguyên bản:** Phép lũy thừa mô-đun 512 bit rất tốn, còn chia một số 512 bit cho số nhỏ thì rẻ. Hãy hỏi: **"Có thể loại bao nhiêu hợp số mà không cần lũy thừa?"**

**Đọc trước:** `prime.cpp:9` (`SMALL_PRIME_LIMIT`), `14-37`, `166-179`.

**Gợi ý:** sàng Eratosthenes bắt đầu đánh dấu bội số từ đâu? Vì sao bảng phải là `static`?

<details><summary>Đáp án</summary>

- Bảng gồm các số nguyên tố nhỏ hơn 20000 (comment dòng 13 ghi 2262 số). Mỗi số nguyên tố `candidate` đánh dấu bội số từ `candidate*candidate` (dòng 23).
- `small_primes()` giữ bảng trong biến `static const` (dòng 35), nên chỉ sàng một lần cho cả chương trình.
- `has_small_factor` (dòng 173): `n.mod_u64(s) == 0 && n != uint512_t(s)`.
</details>

**Thay gì:** thay việc chạy Miller-Rabin cho mọi ứng viên bằng chia thử cho các số nguyên tố nhỏ trước.
**Vì sao:** đa số số lẻ ngẫu nhiên có ước nhỏ; chia số nhỏ rẻ hơn nhiều so với một phép lũy thừa mô-đun 512 bit.
**Nếu không làm thì sao:** kết quả vẫn đúng, nhưng phải chạy lũy thừa mô-đun cho cả những hợp số tầm thường.

⚠️ **Hiểu lầm hay gặp:** "chỉ cần `n.mod_u64(s) == 0`". Sai: nếu n chính là số nguyên tố nhỏ s thì n mod s = 0 và hàm kết luận nhầm n là hợp số. Vì vậy phải có `n != uint512_t(s)` (comment dòng 169-170). Với n = 0: 0 mod 2 = 0 và 0 khác 2 nên hàm trả true, đúng.

**Kết quả sau bước (suy từ code, tự chạy để xác nhận):** `has_small_factor(15)` là true, `has_small_factor(13)` là false, `has_small_factor(0)` là true.

---

## 4. BƯỚC 2: TÁCH n-1 VÀ MỘT VÒNG MILLER-RABIN

> **Tư duy nguyên bản:** Nếu a^(n-1) ≡ 1 mà n là hợp số, ta cần cách nhìn "chi tiết hơn" vào chuỗi bình phương. Hãy hỏi: **"Đi từ a^m đến a^(n-1) bằng bình phương liên tiếp, có điểm nào lộ ra căn bậc hai không tầm thường không?"**

**Đọc trước:** `prime.cpp:39-94`.

**Gợi ý:** vòng đếm trong `decompose_even` dừng ở đâu? Điều gì xảy ra nếu đầu vào là 0?

<details><summary>Đáp án</summary>

- `decompose_even` (dòng 56-61) đếm bit 0 liên tiếp từ bit thấp nhất đến bit 1 đầu tiên, trả `{r, value >> r}`.
- Đầu vào 0 không có bit 1 nên vòng lặp không dừng; dòng 51-55 ném `std::invalid_argument`.
- `miller_rabin_from_first_power` (dòng 66-85): nếu x = 1 hoặc n-1 thì qua. Ngược lại bình phương tối đa r-1 lần; gặp n-1 thì qua; hết vòng thì n là hợp số.
</details>

**Thay gì:** thay phép thử Fermat đơn lẻ bằng chuỗi bình phương x, x², x⁴, ... và theo dõi có gặp n-1 không.
**Vì sao:** nếu chuỗi đi từ 1 mà không qua n-1 thì có một căn bậc hai không tầm thường của 1, nghĩa là n là hợp số.
**Nếu không làm thì sao:** số Carmichael như 561 lọt qua phép thử Fermat với mọi cơ số nguyên tố cùng nhau.

**Vì sao tách `miller_rabin_from_first_power` khỏi `miller_rabin_test`:** vòng cơ số 2 (dòng 102) dùng hàm lũy thừa riêng cho cơ số 2, còn vòng ngẫu nhiên (dòng 93) dùng `modular_exponentiation`. Phần bình phương liên tiếp giống nhau nên dùng chung.
**Nếu không tách thì sao:** phải chép lại phần bình phương liên tiếp ở hai nơi.

**Kết quả sau bước:** n-1 = 2^r·m với m lẻ; bình phương liên tiếp dùng `mul_mod(x, x, n)` (dòng 77), vòng `step = 1 .. r-1` (dòng 75).

---

## 5. BƯỚC 3: `is_probable_prime`

> **Tư duy nguyên bản:** Một vòng với một cơ số chỉ cho bằng chứng yếu. Hãy hỏi: **"Lặp bao nhiêu vòng độc lập thì sai số nhỏ đến mức chấp nhận được?"**

**Đọc trước:** `prime.cpp:182-226`, `dh.h:25`.

**Gợi ý:** những n nào được trả lời ngay, không cần vòng ngẫu nhiên?

<details><summary>Đáp án</summary>

- `rounds < 1` ném ngoại lệ (dòng 184-187).
- n < 2 là false; n = 2 hoặc 3 là true; n chẵn là false (dòng 191-202).
- n lẻ >= 5 có ước nhỏ là false (dòng 205).
- Mỗi vòng chọn `random_range(ctx, 2, n-2)` (dòng 218) rồi gọi `miller_rabin_test`; một vòng thất bại là trả false ngay.
</details>

**Vì sao cơ số trong [2, n-2]:** b = 1 và b = n-1 luôn cho kết quả tầm thường (comment dòng 88-90).
**Nếu dùng cơ số cố định thì sao:** một hợp số có thể vượt qua đúng cơ số đó. Cơ số ngẫu nhiên độc lập mới cho cận 4^(-k) trong báo cáo mục 2.1.

**Kết quả sau bước:** hàm trả false chắc chắn cho hợp số, và true cho số nguyên tố xác suất qua đủ `rounds` vòng.

---

## 6. BƯỚC 4: SÀNG KẾT HỢP CHO q VÀ p = 2q+1

> **Tư duy nguyên bản:** Ta cần **hai** số cùng nguyên tố. Hãy hỏi: **"Một phép chia có kiểm tra được cả hai số cùng lúc không?"**

**Đọc trước:** `prime.cpp:106-136`.

**Gợi ý:** p = 2q+1 chia hết cho s thì q thỏa điều kiện gì theo mod s?

<details><summary>Đáp án</summary>

- 2q + 1 ≡ 0 (mod s) tương đương q ≡ (s-1)/2 (mod s), với s nguyên tố lẻ (comment dòng 117-119).
- Dòng 127-129: một phép `q.mod_u64(s)` cho `remainder`, rồi kiểm đồng thời `remainder == 0 && q != s` (cho q) và `remainder == (s-1)/2 && q != (s-1)/2` (cho p).
- `generate_candidate_q` (dòng 106-112) đặt bit cao nhất và bit thấp nhất bằng 1: đúng độ dài và q lẻ.
</details>

**Thay gì:** thay hai lần sàng (cho q, cho p) bằng một lần.
**Vì sao:** `mod_u64` trên số 512 bit là phần tốn của sàng; dùng chung một số dư cho cả hai số.
**Nếu không làm thì sao:** vẫn đúng, chỉ tốn gấp đôi số phép chia.

**Kết quả sau bước:** ứng viên có ước nhỏ ở q hoặc p bị loại sau một lượt duyệt bảng.

---

## 7. BƯỚC 5: CƠ SỐ 2 VÀ CHỨNG NHẬN POCKLINGTON CÓ ĐIỀU KIỆN

> **Tư duy nguyên bản:** p = 2q+1 có cấu trúc rất đặc biệt: p-1 chỉ có hai ước nguyên tố. Hãy hỏi: **"Cấu trúc đó giúp rút gọn phép kiểm ở chỗ nào?"**

**Đọc trước:** `prime.cpp:96-103`, `138-162`.

**Gợi ý:** với p = 2q+1 và q lẻ thì r bằng mấy? Khi đó vòng cơ số 2 chỉ cần kiểm gì?

<details><summary>Đáp án</summary>

- p-1 = 2^1·q nên r = 1: vòng cơ số 2 tính x = 2^q mod p và chỉ cần x ∈ {1, p-1}.
- Dòng 154-157: `p.mod_u64(3) == 0` thì loại (điều kiện gcd(3, p) = 1).
- Dòng 160-161: `x = two_to_half_exponent_mod(p)`, rồi `(x == 1 || x == p-1) && mul_mod(x, x, p) == 1`.
- Comment dòng 145-150: chứng nhận này **có điều kiện**, dựa trên giả định q nguyên tố (do Miller-Rabin xác nhận). Dòng 151: tiền điều kiện là p = 2q+1 với q lẻ, không dùng cho p tùy ý.
</details>

**Thay gì:** thay lũy thừa Fermat riêng cho p bằng việc dùng luôn kết quả của vòng cơ số 2.
**Vì sao:** x² ≡ 1 (mod p) suy ra điều kiện Fermat cơ số 2 mà không cần thêm phép lũy thừa 512 bit.
**Nếu dùng hàm này cho p tùy ý thì sao:** tiền điều kiện không còn đúng, kết quả không có ý nghĩa.

⚠️ **Hiểu lầm hay gặp:** "đây là chứng minh p nguyên tố". Không: bước này phụ thuộc giả định q nguyên tố, và q chỉ được xác nhận bằng thuật toán xác suất. Phần `is_probable_prime` vẫn là bước kết luận.

**Kết quả sau bước:** ứng viên không qua cơ số 2 bị loại trước khi chạy 20 vòng ngẫu nhiên.

---

## 8. BƯỚC 6: `generate_safe_prime`

> **Tư duy nguyên bản:** Ta có đủ công cụ; còn lại là thứ tự. Hãy hỏi: **"Bước nào loại được nhiều ứng viên nhất với chi phí thấp nhất? Đặt nó lên đầu."**

**Đọc trước:** `prime.cpp:228-263`, `dh.h:26`.

**Gợi ý:** vì sao bitsize bị chặn ở [16, BIGINT_BITS]?

<details><summary>Đáp án</summary>

- Dòng 235: ngoài khoảng thì ném `std::invalid_argument`. Tối thiểu 16 bit; tối đa `BIGINT_BITS` vì p = 2q+1 phải vừa trong `uint512_t` (comment dòng 231-234).
- Vòng `for (;;)` (dòng 241-262): q rồi p; sàng kết hợp; cơ số 2; `is_probable_prime` với `SAFE_PRIME_ROUNDS` (dòng 11, giá trị 20) cho cả q và p; đạt hết thì trả p.
- Listing 2 của báo cáo ghi số 20 trực tiếp, còn code dùng hằng `SAFE_PRIME_ROUNDS`. Không sai, chỉ lệch tên.
</details>

**Kết quả sau bước:** p đúng bitsize bit, p và (p-1)/2 cùng qua 20 vòng Miller-Rabin.

---

## 9. LỖI THƯỜNG GẶP VÀ CÁCH DEBUG

| Triệu chứng | Nguyên nhân thường gặp | Chỗ kiểm |
|-------------|------------------------|----------|
| Vòng lặp không dừng khi tách n-1 | Đầu vào 0 | dòng 51-55 |
| Số nguyên tố nhỏ bị báo là hợp số | Thiếu `n != uint512_t(s)` | dòng 173 |
| Hợp số lọt qua | Vòng bình phương lặp r lần thay vì r-1 | dòng 75 |
| `passes_base_two_and_certifies` cho kết quả vô nghĩa | p không có dạng 2q+1 với q lẻ | dòng 151 |
| Lỗi liên kết khi dịch tay trên Windows | Thiếu `-lbcrypt` ở cuối lệnh | mục 10 |
| Kết quả sai khi nhân số lớn | Nhân trực tiếp thay vì `mul_mod` | `dh.h:12` |

---

## 10. CHẠY THỬ (Windows PowerShell 5.1, trong `project_01_source`)

```powershell
$env:Path += ";C:\msys64\mingw64\bin"
$libs = "src/u512.cpp","src/modexp.cpp","src/random.cpp","src/prime.cpp","src/dh.cpp","lib/fips202/prng.cpp","lib/fips202/fips202.c"
$flags = "-std=c++14","-O3","-Wall","-Wextra","-Wpedantic","-I","src","-I","lib/fips202"
g++ @flags -I tests tests/test_prime.cpp @libs -o test_prime.exe -lbcrypt
.\test_prime.exe
```

**Kết quả đúng:** dòng `test_prime: 28 kiem tra, 0 sai`.
Bộ đầy đủ: `python tests\run_tests.py 512` cho `Tong: 6/6 dat` (u512 7018, modexp 6404, random 23205, prime 28, dh 604 kiểm tra, đều 0 sai) khi đã dịch đủ các tệp test.

---

## 11. BÀI TẬP VẬN DỤNG

**Bài tập 1 (M2).** Tách n-1 = 2^r·m cho n = 341 và n = 561. Rồi kiểm tra tay vòng Miller-Rabin cơ số 2 cho 341.

<details><summary>Đáp án mẫu</summary>

- 341: n-1 = 340 = 2² x 85, nên r = 2, m = 85.
- 561: n-1 = 560 = 2⁴ x 35, nên r = 4, m = 35.
- Vòng cơ số 2 cho 341: 2^10 = 1024 = 3·341 + 1 nên 2^10 ≡ 1 (mod 341), suy ra 2^85 = (2^10)^8 · 2^5 ≡ 32. Vì 32 khác 1 và 340, bình phương một lần (r-1 = 1): 32² = 1024 ≡ 1, vẫn khác 340. Hết vòng mà không gặp 340, nên 341 là hợp số. Đối chiếu: `python -c "print(pow(2,85,341), pow(32,2,341))"` phải ra `32 1`.
</details>

**Bài tập 2 (M4).** Cho q = 5, s = 11. Điều kiện sàng kết hợp có loại nhầm p = 11 không? Thử tiếp với q = 3, s = 7.

<details><summary>Đáp án mẫu</summary>

- q = 5: p = 11, remainder = 5 = (11-1)/2, nhưng q bằng đúng (s-1)/2 = 5 nên vế `q != (s-1)/2` sai, điều kiện **không** kích hoạt. p = 11 không bị loại nhầm.
- q = 3: p = 7, remainder = 3 = (7-1)/2 và q = 3 bằng (s-1)/2, nên cũng không kích hoạt.
- Ý nghĩa: vế `q != (s-1)/2` tồn tại để không loại nhầm khi p chính là số nguyên tố nhỏ s (comment dòng 119).
</details>

**Bài tập 3 (M6).** Nêu một ví dụ cho thấy `passes_base_two_and_certifies` **không** dùng được cho p tùy ý, và nêu một ràng buộc cần có khi gọi hàm.

<details><summary>Gợi ý đáp án</summary>

Hàm giả định p = 2q+1 với q lẻ (dòng 151, tức p ≡ 3 mod 4) và coi như q nguyên tố. Nó tính x = 2^((p-1)/2) mod p (dòng 160) rồi kiểm x ∈ {1, p-1} (dòng 161).

- **p = 13** (p ≡ 1 mod 4, p-1 = 12 = 2²·3, r = 2, q = 6 chẵn): 2^6 = 64 ≡ 12 = p-1 và 12² = 144 ≡ 1 (mod 13), nên hàm trả true. Nhưng (13-1)/2 = 6 không nguyên tố, nên 13 không phải safe prime; kết quả true ở đây không có ý nghĩa.
- **p = 341 = 11·31** (hợp số, 341 mod 3 = 2): 2^10 ≡ 1 (mod 341) nên 2^170 = (2^10)^17 ≡ 1, và 1² = 1. Hàm trả true cho một hợp số. Lý do: phép kiểm này chỉ khớp một vòng Miller-Rabin khi r = 1; với p ≡ 1 (mod 4) thì r ≥ 2 (341 - 1 = 340 = 2²·85).
- **Ràng buộc khi gọi:** p = 2q+1 với q lẻ và q đã qua kiểm tra nguyên tố. Trong `generate_safe_prime` điều này thỏa vì `generate_candidate_q` đặt bit thấp nhất bằng 1 (dòng 110) và q qua `passes_base_two_round` trước khi gọi hàm cho p (dòng 251).
- **Đối chiếu:** `python -c "print(pow(2,6,13), pow(12,2,13), pow(2,170,341), 341%3)"` phải ra `12 1 1 2`.
</details>

---

## 12. TÓM TẮT, TỰ KIỂM TRA CUỐI BÀI

### 12.1. Tóm tắt 1 trang

1. **Sàng số nhỏ** loại hợp số rẻ; luôn có `n != s`.
2. **Miller-Rabin** tách n-1 = 2^r·m, tính x = b^m, bình phương tối đa r-1 lần, tìm n-1.
3. **k vòng độc lập**, cơ số ngẫu nhiên trong [2, n-2]: xác suất sai không vượt quá 4^(-k); k = 20 cho 2^(-40).
4. **Safe prime** p = 2q+1: sàng kết hợp một lần cho cả q và p.
5. **Cơ số 2 cho p** (r = 1) chỉ cần x ∈ {1, p-1}; chứng nhận Pocklington là **có điều kiện**.
6. **Thứ tự lọc:** sàng, cơ số 2, 20 vòng ngẫu nhiên: rẻ trước, đắt sau.

### 12.2. Câu hỏi cuối bài

| Câu | Mục tiêu | Câu hỏi | Gợi ý đáp án |
|-----|----------|---------|--------------|
| 1 | M1 | Vì sao Fermat không đủ? | Có hợp số (341) qua phép thử; số Carmichael qua với mọi cơ số nguyên tố cùng nhau |
| 2 | M1 | Vì sao n-1 = 2^r·m với m lẻ luôn tách được? | n lẻ nên n-1 chẵn; chia 2 đến khi lẻ |
| 3 | M4 | Safe prime p thì p-1 có những ước nguyên tố nào? | 2 và q |
| 4 | M3 | Vì sao b nằm trong [2, n-2]? | b = 1 và n-1 cho kết quả tầm thường |
| 5 | M3 | Sau k vòng độc lập, xác suất sai tối đa? | 4^(-k) |
| 6 | M4 | Nếu p = 2q+1 chia hết cho 7 thì q mod 7 bằng mấy? | 3, vì (7-1)/2 = 3 |
| 7 | M6 | Bỏ điều kiện gcd(3, p) = 1 ở `passes_base_two_and_certifies` thì sao? | Đây là điều kiện gcd(2^((p-1)/q) - 1, p) = gcd(3, p) = 1 của định lý Pocklington (a = 2, (p-1)/q = 2). Đã chạy p ≡ 3 mod 4, p < 2·10^6 khi bỏ điều kiện: 17 hợp số qua phép kiểm x ∈ {1, p-1} (nhỏ nhất 2047 = 23·89), nhưng q của cả 17 số đều là hợp số và không số nào chia hết cho 3, nên chưa có phản ví dụ. Khi q thật sự nguyên tố, phép kiểm này đã kéo theo p nguyên tố (bậc của 2 mod p bằng p-1), nên điều kiện thừa về toán và được giữ để khớp giả thiết của định lý |
| 8 | M8 | Vẽ lại sơ đồ mục 0.2 từ trí nhớ | Đối chiếu với `generate_safe_prime` |

### 12.3. Hiểu lầm thường gặp

| Hiểu lầm | Sự thật |
|----------|---------|
| "Qua Miller-Rabin là chắc chắn nguyên tố" | Chỉ là **xác suất**; không qua mới là chắc chắn hợp số |
| "Qua Fermat cơ số 2 là nguyên tố" | 341 là phản ví dụ |
| "Sàng chỉ cần `mod == 0`" | Phải có `n != s` |
| "Chứng nhận Pocklington chứng minh p nguyên tố" | Có điều kiện, dựa trên giả định q nguyên tố |
| "Chọn cơ số cố định cũng đủ" | Cận xác suất 4^(-k) đòi cơ số ngẫu nhiên độc lập |

### 12.4. Bảng thuật ngữ

| Thuật ngữ | Tiếng Việt | Giải thích |
|-----------|-----------|------------|
| Safe prime | Số nguyên tố an toàn | p nguyên tố và (p-1)/2 nguyên tố |
| Witness | Chứng nhân hợp số | Cơ số b chứng tỏ n là hợp số |
| Carmichael number | Số Carmichael | Hợp số qua Fermat với mọi cơ số nguyên tố cùng nhau |
| Pseudoprime | Số giả nguyên tố | Hợp số qua phép thử với một cơ số nhất định |
| Sieve | Sàng | Loại ước nhỏ trước khi kiểm tốn kém |
| Modular exponentiation | Lũy thừa mô-đun | b^e mod n (NV2) |

---

## 13. ĐÃ ĐỌC CHÉO

- Võ Văn Cường (kiểm chéo NV3): chưa.

---

## PHỤ LỤC: LƯU Ý VỀ ĐỘ CHÍNH XÁC

1. Mọi số dòng lấy từ `prime.cpp` và `dh.h` ở commit `0720eff`; main đổi thì phải cập nhật.
2. Mục "Kết quả sau bước" ở Bước 1 được **suy từ đọc code**, chưa chạy. Chỉ phần ở mục 10 là kết quả đã đo.
3. Cột slide/trang ở mục 0.4 để trống: chưa mở slide để đối chiếu. Chỉ điền sau khi đọc nội dung thật.
4. Thời gian ở mục 0.8 là ước lượng của người soạn, chưa đo.
5. Các bài tập ở mục 11 do người soạn bổ sung, không có trong đề.
6. Phần "Hiểu lầm" và "Lỗi thường gặp" là tổng hợp của người soạn; mục "Lỗi liên kết khi thiếu `-lbcrypt`" dựa trên kinh nghiệm dịch trên máy Windows của nhóm.