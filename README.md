# Marisa
[![pipeline status](https://gitlab.com/ReimuNotMoe/Marisa-Github/badges/master/pipeline.svg)](https://gitlab.com/ReimuNotMoe/Marisa-Github/pipelines)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/a7b5b8bd05be401f9066852f8e8b3b43)](https://www.codacy.com/manual/ReimuNotMoe/Marisa?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=ReimuNotMoe/Marisa&amp;utm_campaign=Badge_Grade)
[![coverage report](https://gitlab.com/ReimuNotMoe/Marisa-Github/badges/master/coverage.svg)](https://gitlab.com/ReimuNotMoe/Marisa-Github/pipelines)



A lightening fast, express-like C++ web server framework

## Features
- Easy to use
- High efficiency
- Multithreaded event-based I/O handling
- Low memory footprint
- Built-in SSL/TLS & unix socket support
- Streamed operations, useful for a big POST
- Operations in middlewares can be blocking or nonblocking
- Will not claim to let people love programming again
- ...and more

## Usage
See the [documentation](https://marisa.readthedocs.io). (WIP)

Also remember to have a look at the examples in [Examples](https://github.com/ReimuNotMoe/Marisa/blob/master/Source/Examples/) directory.
Especially [hello_lambda.cpp](https://github.com/ReimuNotMoe/Marisa/blob/master/Source/Examples/hello_lambda.cpp).

## Benchmarks
See [benchmarks](Benchmark.md).

## Build
### Dependencies
- C++17 compatible compiler and runtime
- zlib
- boost_system

### Compile
Nearly all my projects use CMake. It's very simple:

    cd Source
    mkdir build
    cd build
    cmake ..
    make -j `nproc`

## ToDo
- Websocket support
- HTTP2 support (unlikely in near future)

## Caveats
The project is in alpha state. It's not recommended to use in production environments.

## History & Story
The name "Marisa" is derived from a [Touhou Project character](https://en.touhouwiki.net/wiki/Marisa_Kirisame), who rides a broom and flies fast.

The idea is based on a plain C HTTP server project I was working on when I was at school. It was called "Marisa" since then. It worked like PHP, but every webpage is a shared library (.so/.dll).

---

Time flies. Half a year ago, in a conversation about teamwork, my boss (at that time) told me:

    C++ is not suitable for writing business code.

I totally don't agree with this.

But anyway, finally they decided to use some programming language that doesn't have exceptions, has garbage collection, and enters kernel three times a row in order to complete a single `sleep()`. In my opinion, this is totally a disaster for large projects.

Eventually, I decided to restart this project.

It's 2019 now, the PHP style is pretty outdated, while express-like style works a better way. So this is a complete redesign.

---

Recently some Google guy told me:

    Labor cost of one software engineer worths cost of 100TB of RAM sticks.

I feel a bit meh.

We need more practise in efficient computing, at least for reducing carbon emission. Lol.
