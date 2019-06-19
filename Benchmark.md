# Benchmarks

## Test 1: High end workstation
- Test: [HelloAsync](https://github.com/ReimuNotMoe/Marisa/blob/master/Source/Tests/hello_async.cpp)
- Configuration: 16 TCP instances (`app.listen(8080, 16)`)
- Hardware: AMD Ryzen Threadripper 2950X **(not overclocked)**, 128GB DDR4 2400MHz
- Environment: Linux kernel 5.0.5, GCC 7.4.0, libstdc++ 7.4.0, glibc 2.27
- CFLAGS: `-O2 -march=znver1`
- Benchmark tool: [wrk](https://github.com/wg/wrk)
- Test command: `while :; do wrk -t 16 -c 384 http://127.0.0.1:8080; sleep 1; done`

### Results
- Memory usage in benchmark: 6268 - 5188 = 1080 KB
- Queries per second (QPS): ~220k
- Screenshot:

![](https://raw.githubusercontent.com/ReimuNotMoe/ReimuNotMoe.github.io/master/images/marisa_benchmark_0.png)

## Test 2: Ancient laptop
- Test: [HelloAsync](https://github.com/ReimuNotMoe/Marisa/blob/master/Source/Tests/hello_async.cpp)
- Configuration: 2 TCP instances (`app.listen(8080, 2)`)
- Server hardware: Intel Core 2 Duo U7700 @ 1.33 GHz, 2GB DDR2 667MHz
- Server network adapter: Broadcom BCM5752 @ 1 Gbps
- Environment: Ubuntu 18.10 stock
- CFLAGS: `-O2 -march=core2`
- Benchmark tool: [wrk](https://github.com/wg/wrk)
- Test command: `while :; do wrk -t 8 -c 256 http://169.254.9.2:8080; sleep 1; done`

### Results
- Memory usage in benchmark: 5824 - 4744 = 1080 KB
- Queries per second (QPS): ~6.8k
- Photo:

![](https://raw.githubusercontent.com/ReimuNotMoe/ReimuNotMoe.github.io/master/images/marisa_benchmark_1.jpg)

## Test 3: Awesome Gaming PC
- Test: [HelloAsync](https://github.com/ReimuNotMoe/Marisa/blob/master/Source/Tests/hello_async.cpp)
- Configuration: 16 TCP instances (`app.listen(8080, 16)`)
- Hardware: Intel i9 9900K **(not overclocked)**, 64GB DDR4 3000MHz
- Environment: Ubuntu 18.10 stock
- CFLAGS: `-O2 -march=native`
- Benchmark tool: [wrk](https://github.com/wg/wrk)
- Test command: `wrk -c 256 -t 16 http://localhost:8080`

### Results
- Memory usage in benchmark: 5932 - 5156 = 776 KB
- Queries per second (QPS): ~255k
- Photo:

![](https://raw.githubusercontent.com/ReimuNotMoe/ReimuNotMoe.github.io/master/images/marisa_benchmark_2.png)