#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <cstdio>

static int g_checks = 0;
static int g_failed = 0;

#define CHECK(cond)                                                     \
    do                                                                  \
    {                                                                   \
        ++g_checks;                                                     \
        if (!(cond))                                                    \
        {                                                               \
            if (++g_failed <= 5)                                        \
                std::printf("  SAI dong %d: %s\n", __LINE__, #cond);    \
        }                                                               \
    } while (0)

static int finish(const char *name)
{
    std::printf("%s: %d kiem tra, %d sai\n", name, g_checks, g_failed);
    return g_failed == 0 ? 0 : 1;
}

#endif
