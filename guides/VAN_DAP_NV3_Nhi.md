# Bộ câu hỏi vấn đáp Lab 01 (Diffie-Hellman và safe prime)

Tài liệu ôn vấn đáp online (Google Meet). Phần A: câu hỏi và đáp án ngắn cho phần NV3 (Miller-Rabin, safe prime) của Nhi, kèm link về guide. Phần B: câu để hỏi thử trong nhóm, không kèm đáp án.

Ghi chú: link `#sN` chỉ nhảy đúng mục sau khi PR thêm khung điều hướng cho guide (#65) được merge.

## A. Câu hỏi phần NV3 (Nhi)

1. **Safe prime là gì, vì sao dùng cho DH?**
   p và q = (p-1)/2 đều nguyên tố. Tìm phần tử sinh dễ: chỉ kiểm g² ≠ 1 và g^q ≠ 1, mỗi lần thử trúng khoảng 1/2. Kiểm giá trị công khai cũng gọn vì chỉ có nhóm cấp p-1 hoặc (p-1)/2. Xem [mục 0](GUIDE_NV3_Nhi.md#s0), [mục 8](GUIDE_NV3_Nhi.md#s8).

2. **Vì sao không dùng Fermat mà dùng Miller-Rabin?**
   Số Carmichael (ví dụ 561 = 3×11×17) qua Fermat với mọi cơ số nguyên tố cùng nhau. Miller-Rabin kiểm thêm căn bậc hai không tầm thường của 1 nên bắt được. Xem [mục 1](GUIDE_NV3_Nhi.md#s1), [mục 4](GUIDE_NV3_Nhi.md#s4).

3. **20 vòng Miller-Rabin sai với xác suất bao nhiêu?**
   Hợp số lẻ lọt qua mỗi vòng không quá 1/4 (HAC Fact 4.23, 4.25), nên không quá 4^-20 = 2^-40 ≈ 9,09×10^-13. Đây là xác suất hợp số lọt qua, không phải xác suất p là hợp số (HAC Note 4.47). Xem [mục 5](GUIDE_NV3_Nhi.md#s5), [phụ lục](GUIDE_NV3_Nhi.md#phu-luc).

4. **Cơ số chọn thế nào?**
   Trong [2, n-2] (HAC Algorithm 4.24). Vòng đầu dùng cơ số 2 cố định (câu 7). Xem [mục 4](GUIDE_NV3_Nhi.md#s4), [mục 5](GUIDE_NV3_Nhi.md#s5).

5. **Sàng số nguyên tố nhỏ để làm gì?**
   Loại nhanh phần lớn hợp số bằng phép chia nhỏ trước khi chạy lũy thừa mô-đun đắt hơn. Xem [mục 3](GUIDE_NV3_Nhi.md#s3).

6. **Sàng chung cho q và p = 2q+1 là gì?**
   p = 2q+1 chia hết cho s lẻ khi q ≡ (s-1)/2 (mod s). Một phép `mod_u64` trên q kiểm được cả hai. Xem [mục 6](GUIDE_NV3_Nhi.md#s6).

7. **Pocklington trong code là gì, kết luận có chắc không?**
   n = 1+2Rq, q lẻ nguyên tố, q > R. Nếu a^(n-1) ≡ 1 và gcd(a^(2R)-1, n) = 1 thì n nguyên tố (HAC Fact 4.40, 4.59). Code dùng R=1, a=2 nên điều kiện gcd thành gcd(3,p)=1, tức `p.mod_u64(3) != 0`. Điều kiện Fermat suy ra từ vòng cơ số 2 (p-1 = 2^1·q). Kết luận có điều kiện vì q chỉ nguyên tố theo xác suất. Xem [mục 7](GUIDE_NV3_Nhi.md#s7).

8. **Pocklington có tăng độ an toàn không?**
   Không. Kiểm nghiệm Python trên 37.214 hợp số p = 2q+1 < 10^6 với q nguyên tố: không số nào qua vòng Miller-Rabin cơ số 2, nên chứng nhận không loại thêm hợp số nào. Xem [mục 7](GUIDE_NV3_Nhi.md#s7), [phụ lục](GUIDE_NV3_Nhi.md#phu-luc).

9. **Có nhanh hơn không?**
   Không. Pocklington/main: gộp 1,0358, trung vị 1,0167. Lũy thừa cơ số 2/Pocklington cũ: gộp 0,9241, trung vị 0,9203. Nguyên nhân mức giảm chưa rõ, không gán lý do. Mỗi lần đo là 20 giá trị p × 3 lượt, cùng seed, một máy, không phải 60 p độc lập. Xem [mục 7](GUIDE_NV3_Nhi.md#s7).

10. **`generate_safe_prime` chạy thế nào?**
    Sinh q ngẫu nhiên, sàng chung q và p, Miller-Rabin cơ số 2 kèm Pocklington, rồi các vòng Miller-Rabin còn lại. Xem [mục 8](GUIDE_NV3_Nhi.md#s8).

11. **Khóa riêng vì sao nằm trong [2, p-2]?** (phần NV4 của Phát, chỉ nói ý chính)
    x = 0 hoặc p-1 cho g^x = 1, x = 1 làm lộ khóa. Lấy mẫu loại bỏ để không lệch phân bố.

12. **Phần review NV2 (Cường): Nhi đã kiểm gì?**
    Review trên commit `bcea98d` (`origin/Cuong`), kết quả ghi ở `nv2-review.md`. (a) Đọc code `modexp.cpp` đối chiếu tiêu chí NV2 trong `PhanCongCongViec.md`: đủ 4 hàm, duyệt bit trái sang phải, biên exp = 0, mod = 1, base ≥ mod. (b) Bộ đối chiếu viết riêng so với `pow(a,b,m)` của Python, seed 20261109: 2328 vector, 0 lỗi; chạy lại với `LIMBS = 16` (1024 bit): 2328 vector, 0 lỗi. Gồm cả m chẵn, m = 2^512-1, cộng tràn, đầu vào a, b ≥ m. (c) m = 0: cả 4 hàm ném `std::invalid_argument`. (d) Biên dịch `-Wall -Wextra -Wpedantic`: 0 cảnh báo. (e) `test_modexp` của repo: 4006 kiểm tra, 0 sai. Gợi ý, không phải lỗi: test của repo chỉ dùng m lẻ nên thiếu biên; `digits[0] |= 1` truy cập trực tiếp biểu diễn. Chưa làm được: ASan và UBSan có runtime không chạy trên máy này (chỉ chạy được UBSan kiểu trap, không trap). Chưa kiểm `main.cpp` có bắt ngoại lệ m = 0 không. Thời gian đo (modular_exponentiation 512 bit, median 8,8 ms, 20 cặp, một máy, một đợt) chỉ để tham khảo.
13. **Hiệu năng 1024 bit?**
    Chỉ nói số đã đo: `dh.exe` 1024 bit, 3 lần seed hệ điều hành ra 3,31 / 1,43 / 0,14 giây. Thời gian sinh safe prime ngẫu nhiên, dao động lớn, chỉ là ví dụ.

14. **Code chạy được không?**
    Test 6/6 đạt ở 512 bit và ở 1024 bit trên Windows. Chưa dịch thử trên Linux (báo cáo mục 6.3 ghi rõ).

15. **Định lý Fermat nhỏ nói gì, và 341 cho thấy điều gì?**
    p nguyên tố, gcd(a,p)=1 thì a^(p-1) ≡ 1 (mod p). Chiều ngược không đúng: 341 = 11×31 là hợp số nhưng 2^340 ≡ 1 (mod 341) (giả nguyên tố cơ số 2). Số Carmichael còn tệ hơn (câu 2). Xem [mục 1](GUIDE_NV3_Nhi.md#s1), [mục 4](GUIDE_NV3_Nhi.md#s4).

16. **Vì sao x² ≡ 1 (mod p) với p nguyên tố chỉ có x ≡ ±1?**
    x² - 1 = (x-1)(x+1) chia hết cho p, p nguyên tố nên p | (x-1) hoặc p | (x+1). Xem [mục 1](GUIDE_NV3_Nhi.md#s1).

17. **Hợp số thì x² ≡ 1 có thể có nghiệm nào khác?**
    Hợp số lẻ có ít nhất 2 ước nguyên tố lẻ phân biệt thì có từ 4 nghiệm (định lý phần dư Trung Hoa). Ví dụ mod 35: 1, 6, 29, 34 (6² = 36 ≡ 1, 29² = 841 = 24×35 + 1). Nghiệm 6 và 29 là căn bậc hai không tầm thường của 1, Miller-Rabin bắt đúng chỗ này. Xem [mục 1](GUIDE_NV3_Nhi.md#s1).

18. **Một vòng Miller-Rabin làm gì? Witness là gì?**
    Viết n-1 = 2^s·d, d lẻ. Tính x = b^d mod n. Nếu x = 1 hoặc x = n-1 thì qua. Ngược lại bình phương liên tiếp tối đa s-1 lần: gặp n-1 thì qua; hết mà không gặp thì n chắc chắn hợp số. Cơ số b làm lộ hợp số gọi là witness. Xem [mục 4](GUIDE_NV3_Nhi.md#s4).

19. **Cận 1/4 từ đâu ra? Chứng minh thế nào?**
    Chỉ nêu kết luận và nguồn: hợp số lẻ có không quá 1/4 cơ số là strong liar (HAC Fact 4.23, 4.25; Rabin 1980). Nhi không tự chứng minh, guide cũng không có chứng minh. Xem [mục 5](GUIDE_NV3_Nhi.md#s5).

20. **Lũy thừa cơ số 2 dùng phép cộng thay phép nhân ở đâu?**
    Với cơ số 2, nhân đôi x mod n bằng x + x rồi trừ n nếu cần (`add_mod_reduced`), thay cho một phép nhân Montgomery. Chỉ dùng khi mô-đun lẻ (Montgomery yêu cầu). Xem [mục 7](GUIDE_NV3_Nhi.md#s7).

21. **Vì sao bảng số đo chỉ dùng tỉ số tổng và trung vị?**
    Ở lượt 2 của bản Pocklington cũ có hai mẫu chênh lớn: p thứ 7 là 57,9 ms so với khoảng 37 ms, p thứ 8 là 302 ms so với khoảng 173 ms. Thời gian từng p dao động mạnh nên chỉ báo tỉ số tổng và trung vị, không báo từng mẫu. Xem [mục 12.5](GUIDE_NV3_Nhi.md#s12).

22. **Thứ tự chạy khi đo có thiên lệch không?**
    Đợt Pocklington chạy thứ tự cố định (main trước, pock sau); đợt cơ số 2 chạy xen kẽ. Nên không loại trừ ảnh hưởng của thứ tự ở đợt Pocklington, chưa đo riêng. Xem [mục 12.5](GUIDE_NV3_Nhi.md#s12).

## B. Câu hỏi thử trong nhóm (không đáp án)

- **Cường:** (1) `uint512_t` lưu số thế nào, phép nhân 512 bit tràn ra sao? (2) Montgomery giải quyết điều gì so với `mod` thường?
- **Phát:** (1) Seed từ hệ điều hành lấy thế nào, vì sao nhánh Windows dùng `BCryptGenRandom`? (2) SHAKE128 dùng làm gì, lấy mẫu loại bỏ trong `random_range` để tránh điều gì?
- **Nghi:** (1) `find_generator` kiểm điều kiện gì, vì sao safe prime giúp ít bước? (2) Alice và Bob trao đổi gì, ai biết gì, zip nộp gồm file nào?
- **Nhi:** (1) Vì sao Miller-Rabin thay Fermat? (2) Pocklington trong code làm gì và có tăng độ an toàn không?