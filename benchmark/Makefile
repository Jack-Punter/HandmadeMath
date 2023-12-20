ROOT_DIR=..

CFLAGS=-std=c99 -D_POSIX_C_SOURCE=199309L -fsanitize=address
BENCH_INC=-I $(ROOT_DIR)/benchmark/include -I $(ROOT_DIR)

.SILENT:
benchmark_c: $(ROOT_DIR)/benchmark/benchmark.c
	echo SIMD Debug
	$(CC) $(CFLAGS) benchmark.c -g $(BENCH_INC) -lm
	./a.out
	echo SIMD Release 
	$(CC) $(CFLAGS) benchmark.c -O2 $(BENCH_INC) -lm
	./a.out
	echo NON-SIMD Debug
	$(CC) $(CFLAGS) -DHANDMADE_MATH_NO_SIMD benchmark.c -g $(BENCH_INC) -lm
	./a.out
	echo NON-SIMD Release 
	$(CC) $(CFLAGS) -DHANDMADE_MATH_NO_SIMD benchmark.c -O2 $(BENCH_INC) -lm
	./a.out
