#ifndef COMMON_H
#define COMMON_H

#include <cstdio>
#include <cstdlib>

// Hàm trong khung chưa được cài: in tên hàm rồi thoát với mã 2.
// Xóa lời gọi này khi cài xong hàm tương ứng.
[[noreturn]] inline void not_implemented(const char *name)
{
    std::fprintf(stderr, "CHUA CAI: %s\n", name);
    std::exit(2);
}

#endif
