# Windows: mingw32-make [run | test | bench | clean]
# Linux:   make [run | test | bench | clean]
CXX := g++
CXXFLAGS := -std=c++14 -O3 -Wall -Wextra -Wpedantic -I src -I lib/fips202

ifeq ($(OS),Windows_NT)
  EXE := .exe
  LDLIBS := -static -lbcrypt
  PYTHON := python
  CLEAN := powershell -NoProfile -Command "Remove-Item -Force -ErrorAction SilentlyContinue *.exe"
else
  EXE :=
  LDLIBS :=
  PYTHON := python3
  CLEAN := rm -f dh test_u512 test_modexp test_random test_prime test_dh bench_modexp
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

bench_modexp$(EXE): tests/bench_modexp.cpp $(LIB_SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $< $(LIB_SRCS) -o $@ $(LDLIBS)

run: dh$(EXE)
	./dh$(EXE)

test: dh$(EXE) $(TEST_BINS)
	$(PYTHON) tests/run_tests.py

bench: bench_modexp$(EXE)
	./bench_modexp$(EXE)

clean:
	$(CLEAN)

.PHONY: all run test bench clean
