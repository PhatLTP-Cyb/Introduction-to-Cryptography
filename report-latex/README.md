# Khung Báo Cáo LaTeX - Lab 01: Mã hóa và Mật mã học (Nhóm 15)

Thư mục này chứa toàn bộ khung mã nguồn LaTeX cho báo cáo bài tập thực hành Lab 01.

---

## 1. Phân công người viết từng mục

Theo thỏa thuận phân công công việc trong `PhanCongCongViec.md` và đề bài mục 2.5:

| Tệp phần nội dung | Tiêu đề mục | Người phụ trách | Ghi chú |
|---|---|---|---|
| `sections/00-bia-nhom.tex` | Thông tin nhóm, bảng mức độ đóng góp | Toàn nhóm (Phát điều phối) | Bắt buộc bảng % đóng góp |
| `sections/01-so-lon.tex` | 1. Hệ thống xử lý số lớn | **Võ Văn Cường** | *Cần xác nhận lại chi tiết* |
| `sections/02-kiem-tra-nguyen-to.tex` | 2. Hàm kiểm tra số nguyên tố | **Trần Hoàng Nhi** | Số đo từ nv3-e, nv3-f |
| `sections/03-so-ngau-nhien.tex` | 3. Số ngẫu nhiên (Điểm cộng) | **Lê Tấn Phát** | Chuẩn CSPRNG, FIPS 202 |
| `sections/04-ung-dung-dh.tex` | 4. Trao đổi khóa Diffie-Hellman | **Tô Tuệ Nghi** | Giao thức TLS 1.3 / SSH |
| `sections/05-mo-ta-chuong-trinh.tex` | 5. Mô tả chương trình đã cài đặt | **Tô Tuệ Nghi** | Hướng dẫn lệnh, đổi bit |
| `sections/06-tu-danh-gia.tex` | 6. Tự đánh giá ưu nhược điểm | **Lê Tấn Phát** | Đánh giá khách quan |

---

## 2. Quy ước viết bài và định dạng

1. **Chỉ ghi số liệu thật:** Mọi số liệu đo lường thời gian (ms), số vòng lặp, số lượng vector kiểm thử và đầu ra console **bắt buộc phải lấy từ các lần chạy thực nghiệm thật** trên máy tính của nhóm (không ước lượng hay bịa đặt số liệu).
2. **Quy ước thẻ TODO:** Mỗi thành viên mở tệp thuộc phần mình phụ trách, tìm các dòng chú thích `% TODO(<Tên>): ...` để viết nội dung tương ứng.
3. **Đưa hình ảnh minh họa:**
   - Tạo thư mục `report-latex/figures/` (nếu chưa có).
   - Đặt tệp ảnh định dạng `.png` hoặc `.pdf` vào `figures/`.
   - Sử dụng lệnh:
     ```latex
     \begin{figure}[h!]
     \centering
     \includegraphics[width=0.8\textwidth]{figures/ten_file_anh.png}
     \caption{Mô tả hình ảnh}
     \label{fig:nhan-hinh}
     \end{figure}
     ```
4. **Trích dẫn tài liệu tham khảo:**
   - Thêm nguồn vào tệp `references.bib`.
   - Sử dụng lệnh `\cite{khoa_trich_dan}` trong bài viết (ví dụ: `\cite{diffie1976new}`).

---

## 3. Hướng dẫn biên dịch sang PDF trên Windows

### Cách 1: Biên dịch bằng `pdflatex` và `bibtex` (Phương án chính)
Do máy Windows của nhóm thiếu môi trường Perl để chạy `latexmk`, quy trình biên dịch chuẩn sử dụng chuỗi lệnh thủ công:
```powershell
cd report-latex
pdflatex main.tex
bibtex main
pdflatex main.tex
pdflatex main.tex
```

### Cách 2: Sử dụng Overleaf (Phương án dự phòng)
Trong trường hợp máy tính cá nhân gặp sự cố môi trường hoặc thiếu gói LaTeX, tải toàn bộ thư mục `report-latex/` lên **Overleaf**, chọn trình biên dịch `pdfLaTeX` để biên dịch trực tuyến và tải tệp PDF về.

---

## 4. Xuất tệp PDF báo cáo nộp bài

Sau khi hoàn tất biên dịch và rà soát báo cáo, sao chép tệp `main.pdf` sang thư mục báo cáo của đồ án:
```powershell
# Tạo thư mục project_01_report nếu chưa có
if (-not (Test-Path ..\project_01_report)) { New-Item -ItemType Directory -Path ..\project_01_report }

# Sao chép tệp PDF hoàn chỉnh
Copy-Item main.pdf -Destination ..\project_01_report\BaoCao_Lab01_Nhom15.pdf
```
Tệp PDF này sẽ được kiểm tra chéo và đóng gói vào tệp zip nộp bài cuối cùng theo quy định của môn học.
