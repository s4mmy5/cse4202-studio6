all:
	gcc -Wall -o dense_mm dense_mm.c
	gcc -Wall -o parallel_dense_mm parallel_dense_mm.c -fopenmp
	gcc -Wall -o sort sort.c
	gcc -Wall -o sing sing.c
	gcc -Wall -o timed_parallel_dense_mm timed_parallel_dense_mm.c -fopenmp

clean:
	rm dense_mm parallel_dense_mm sort sing
