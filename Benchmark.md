# Benchmarks

## Test 1
- Test: [HelloAsync](https://github.com/ReimuNotMoe/Marisa/blob/master/Tests/hello_async.cpp)
- Configuration: 10 TCP instances (`app.listen(8080, 10)`)
- Hardware: AMD Ryzen Threadripper 2950X, 128GB DDR4 2400MHz
- Environment: Linux kernel 5.0.5, GCC 7.4.0, libstdc++ 7.4.0, glibc 2.27
- CFLAGS: `-O2 -march=znver1`
- Benchmark tool: [wrk](https://github.com/wg/wrk)
- Test command: `while :; do wrk -t 8 -c 256 http://127.0.0.1:8080; sleep 1; done`

### Results
- Memory usage in benchmark: 5788 - 4684 = 1104 KB
- Queries per second (QPS): ~150k
- Screenshot:

![](https://raw.githubusercontent.com/ReimuNotMoe/ReimuNotMoe.github.io/master/images/benchmark_0.png)