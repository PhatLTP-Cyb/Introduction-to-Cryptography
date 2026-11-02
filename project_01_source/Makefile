# Windows: mingw32-make [run | test | clean]
# Linux:   make [run | test | clean]
CXX := g++
# Bỏ -Wno-unused-parameter khi đã cài xong mọi hàm trong khung.
CXXFLAGS := -std=c++14 -O3 -Wall -Wextra -Wpedantic -Wno-unused-parameter -I src -I lib/fips202

ifeq ($(OS),Windows_NT)
  EXE := .exe
  LDLIBS := -lbcrypt
  PYTHON := python
  CLEAN := powershell -NoProfile -Command "Remove-Item -Force -ErrorAction SilentlyContinue *.exe"
else
  EXE :=
  LDLIBS :=
  PYTHON := python3
  CLEAN := rm -f dh test_u512 test_modexp test_random test_prime test_dh
endif

HEADERS := src/common.h src/u512.h src/dh.h lib/fips202/prng.h lib/fips202/fips202.h
LIB_SRCS := src/u512.cpp src/modexp.cpp src/random.cpp src/prime.cpp src/dh.cpp lib/fips202/prng.cpp lib/fips202/fips202.c
TESTS := test_u512 test_modexp test_random test_prime test_dh
TEST_BINS := $(addsuffix $(EXE),$(TESTS))

all: dh$(EXE)

dh$(EXE): src/main.cpp $(LIB_SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) src/main.cpp $(LIB_SRCS) -o $@ $(LDLIBS)

test_%$(EXE): tests/test_%.cpp tests/test_util.h $(LIB_SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -I tests $< $(LIB_SRCS) -o $@ $(LDLIBS)

run: dh$(EXE)
	./dh$(EXE)

test: dh$(EXE) $(TEST_BINS)
	$(PYTHON) tests/run_tests.py

clean:
	$(CLEAN)

.PHONY: all run test clean
