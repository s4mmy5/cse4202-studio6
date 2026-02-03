# Studio 6: Timing and Benchmarking

# Required Exercises:

1. Contributors:
   - Jonathan Rodriguez Gomez

2. Brief Description:
   - sort: generates a random array and performs in-place quicksort.
   - sing: generates text output.
   - dense_mm: performs naive matrix multiplication.
   - parallel_dense_mm: performs parallelized naive matrix multiplication.
   
3. Benchmarking with `time`:
```
./test.sh sing sort dense_mm parallel_dense_mm
============ Testing sing ============

real	0m0.002s
user	0m0.001s
sys	0m0.001s

real	0m0.001s
user	0m0.000s
sys	0m0.001s

real	0m0.001s
user	0m0.000s
sys	0m0.001s
======================================
============ Testing sort ============

real	0m0.001s
user	0m0.000s
sys	0m0.002s

real	0m0.001s
user	0m0.000s
sys	0m0.001s

real	0m0.001s
user	0m0.001s
sys	0m0.000s
======================================
============ Testing dense_mm ============

real	0m0.001s
user	0m0.001s
sys	0m0.000s

real	0m0.010s
user	0m0.008s
sys	0m0.001s

real	0m3.555s
user	0m3.547s
sys	0m0.007s
======================================
============ Testing parallel_dense_mm ============

real	0m0.002s
user	0m0.003s
sys	0m0.002s

real	0m0.002s
user	0m0.009s
sys	0m0.001s

real	0m0.436s
user	0m4.743s
sys	0m0.003s
======================================
```

4. Explanation of time
   - real: the time elapsed between invocation and termination in wall time.
   - user: the user CPU time used by the process to execute
   - sys: the system CPU time consumed by system overhead.

P.S: Both user and sys can be higher than real due to multi-threading, CPU time is counted per CPU and then added up.

5. Benchmark parallel_dense_mm vs dense_mm:

```
time ./parallel_dense_mm 1000 >/dev/null && time ./dense_mm 1000 >/dev/null

real	0m0.457s
user	0m4.846s
sys	0m0.006s

real	0m3.520s
user	0m3.519s
sys	0m0.001s
```

The reason why real is lower for parallel is because execution is distributed across CPUs but time is counted indiviually for each CPU and then added. Since dense_mm is single-threaded the cpu time more closely aligns with the real time.

6. Bencmark sing vs dense_mm:
```
real	0m0.019s
user	0m0.000s
sys	0m0.018s

real	0m3.599s
user	0m3.593s
sys	0m0.005s
```

We can see that sing incurs a much higher sys time penalty. This is because writing to stdout requires a write(2) syscall, this means that much of the IO is being done in kernel_mode. On the other hand dense_mm does much of the processing in user mode, thus its sys time penalty is not as high.

7. clock_getres():

   - Clock suitable for user-space benchmarking: CLOCK_PROCESS_CPUTIME_ID.
           - Reason: Oftentimes we are not concerned by system based bottlenecks and would simply like to benchmark the pure algorithmic efficiency of our program, in which case it makes sense to use CPU time as it should be mostly independent of issues like system load, which are not always present.
   - Clock unsuitable for user-space benchmarking: CLOCK_MONOTONIC.
           - Reason: This clock represents monotonic time since "some unspecified point in the past". Simply put using this clock would require more effort than necessary since it requires sampling the time at the start and end of the program. Additionally it will not differentiate between user or kernel mode execution making it extremely difficult to pinpoint bottlenecks.

8. Program output:
```
CLOCK_REALTIME precision (nsec): 1
CLOCK_REALTIME_COARSE precision (nsec): 1000000
CLOCK_MONOTONIC precision (nsec): 1
CLOCK_MONOTONIC_COARSE precision (nsec): 1000000
CLOCK_TAI precision (nsec): 1
```

Explanation

9. Explanation: The MONOTONIC clock by the Unix standard refers to a clock counting since an unspecified point in time, in linux this is by convention the start time, when the kernel boots up. COARSE precision is based on the jiffies method for time-keeping, given that the linux kernel currently sets HZ=1000, this lines up to give us one tick every 1 ms (i.e. 1000000 nsecs). On the other hand the non-COARSE timers use the high resolution timer API giving us precision down to 1 nsec.

10. getdelay.c output:

```
./getdelay
Time (pre-call): 1769987946.419165880
Time (post-call): 1769987946.419165880
Time (elapsed): 0.000000084
```

How to obtain: Sample the clock twice (using clock_getime) and take the difference.

11. Benchmarking parallel naive matrix multiplication:

```
Longest Multiplication: 0.005463917
Shortest Multiplication: 0.000798959
Average Multiplication: 0.001186007
```

A reasonable estimate of the common case would be 0.00001 (nsec) * N.

12. ktime Module:

I used ktime_get(), as advised by the linux docs "As a rule of thumb, using an accessor with a shorter name is preferred over one with a longer name if both are equally fit for a particular use case." 

System Log:
```
[  278.848434] simple_module: loading out-of-tree module taints kernel.
[  278.848961] Module Initialized
[  286.987911] Time elapsed: 8.138549653
[  286.987950] Module Unloaded
```

13. Reported timing values:

```
Longest Multiplication (tsc): 12204087
Shortest Multiplication (tsc): 10310328
Average Multiplication (tsc): 10419869
```
