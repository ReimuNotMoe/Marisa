# Marisa

A lightening fast, express-like C++ web server framework

## Features
- Easy to use
- High efficiency
- Low memory footprint, ~2MB for a Hello World @ 140k qps \[[1](#cite-notes)\]
- Built-in SSL/TLS & unix socket support
- Streamed operations, useful for a big POST
- Operations in middlewares can be blocking or nonblocking
- Will not claim to let people love programming again
- ...and more

## Usage
For now, have a look at the examples in `Tests` directory. The documentation is on its way.

## Build
### Dependencies
- C++17 compatible compiler and runtime
- zlib
- boost_system

### Compile
Nearly all my projects use CMake. It's very simple:

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
The idea and name are based on a plain C HTTP server project I was working on when I was at school. It worked like PHP, but every webpage is a shared library (.so/.dll).

Time flies. Half a year ago, in a conversation about teamwork, my boss (at that time) told me:

    C++ is not suitable for writing business code.

I totally don't agree with this.

But anyway, finally they decided to use some programming language that doesn't have exceptions, and enters kernel three times a row in order to complete a single `sleep()`. In my opinion, this is totally a disaster for large projects.

Eventually, I decided to restart this project.

It's 2019 now, the PHP style is pretty outdated, while express-like style works a better way. So this is a complete redesign.

## Cite notes
\[1\] Test environment: Linux kernel 5.0.5, GCC 7.4.0, libstdc++ 7.4.0, glibc 2.27, `wrk -t 8 -c 256`
