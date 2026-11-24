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

12. **Phần review NV2 (Cường): Nhi đã kiểm gì?** Nhi tự trình bày theo phần review đã làm.

13. **Hiệu năng 1024 bit?**
    Chỉ nói số đã đo: `dh.exe` 1024 bit, 3 lần seed hệ điều hành ra 3,31 / 1,43 / 0,14 giây. Thời gian sinh safe prime ngẫu nhiên, dao động lớn, chỉ là ví dụ.

14. **Code chạy được không?**
    Test 6/6 đạt ở 512 bit và ở 1024 bit trên Windows. Chưa dịch thử trên Linux (báo cáo mục 6.3 ghi rõ).

## B. Câu hỏi thử trong nhóm (không đáp án)

- **Cường:** (1) `uint512_t` lưu số thế nào, phép nhân 512 bit tràn ra sao? (2) Montgomery giải quyết điều gì so với `mod` thường?
- **Phát:** (1) Seed từ hệ điều hành lấy thế nào, vì sao nhánh Windows dùng `BCryptGenRandom`? (2) SHAKE128 dùng làm gì, lấy mẫu loại bỏ trong `random_range` để tránh điều gì?
- **Nghi:** (1) `find_generator` kiểm điều kiện gì, vì sao safe prime giúp ít bước? (2) Alice và Bob trao đổi gì, ai biết gì, zip nộp gồm file nào?
- **Nhi:** (1) Vì sao Miller-Rabin thay Fermat? (2) Pocklington trong code làm gì và có tăng độ an toàn không?