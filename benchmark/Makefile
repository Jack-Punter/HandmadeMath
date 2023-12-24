ROOT_DIR=..

CFLAGS=-std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wno-missing-braces 
BENCH_INC=-I $(ROOT_DIR)/benchmark/include -I $(ROOT_DIR)
CC := gcc

all: runner

dbg_simd:
	$(CC) $(CFLAGS) benchmark.c -o dbg_simd.out -g $(BENCH_INC) -lm
rel_simd:
	$(CC) $(CFLAGS) benchmark.c -o rel_simd.out -O2 $(BENCH_INC) -lm
dbg_nonsimd:
	$(CC) $(CFLAGS) -DHANDMADE_MATH_NO_SIMD -o dbg_nonsimd.out benchmark.c -g $(BENCH_INC) -lm
rel_nonsimd:
	$(CC) $(CFLAGS) -DHANDMADE_MATH_NO_SIMD -o rel_nonsimd.out benchmark.c -O2 $(BENCH_INC) -lm

# .SILENT:
.PHONY:
runner: dbg_simd rel_simd rel_nonsimd dbg_nonsimd 
	echo Building with compiler ${CC}
	echo Debug SIMD
	@ ./dbg_simd.out
	echo Release SIMD
	@ ./rel_simd.out
	echo Debug NONSIMD
	@ ./dbg_nonsimd.out
	echo Release NONSIMD
	@ ./rel_nonsimd.out
	@ rm *.out
