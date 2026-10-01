<div align="center">

# Cerver

A minimal, educational HTTP server written in C.

[![Stars](https://img.shields.io/github/stars/farhaanaliii/cerver?style=flat-square)](https://github.com/farhaanaliii/cerver/stargazers)
[![Forks](https://img.shields.io/github/forks/farhaanaliii/cerver?style=flat-square)](https://github.com/farhaanaliii/cerver/network/members)
[![Issues](https://img.shields.io/github/issues/farhaanaliii/cerver?style=flat-square)](https://github.com/farhaanaliii/cerver/issues)
[![License](https://img.shields.io/github/license/farhaanaliii/cerver?style=flat-square)](LICENSE)

</div>

Cerver is a minimal, straightforward HTTP server written from scratch in C. Built without external dependencies or complex abstractions, it focuses on simplicity and readability—demonstrating fundamental socket programming, basic routing, and static file delivery across Windows (Winsock2) and POSIX systems. It is single-threaded and synchronous, designed for learning, prototyping, and personal experiments rather than high-concurrency production workloads.

## Features

- Minimal, clean codebase with zero third-party dependencies
- Cross-platform support for Windows (Winsock2) and POSIX systems
- Route registration and path matching
- Static file serving with MIME type detection
- Automatic port reuse configuration
- Timestamped request logging

## Installation

Clone the repository into your project directory:

```bash
git clone https://github.com/farhaanaliii/cerver.git
```

Include `cerver.h` in your code and compile `cerver.c` alongside your project files.

## Building

On Windows (MinGW / GCC):

```bash
gcc main.c cerver.c -o cerver.exe -lws2_32
```

On Linux and macOS:

```bash
gcc main.c cerver.c -o cerver
```

## Usage

```c
#include "cerver.h"

int main(void) {
    Cerver server;
    if (!cerver_init(&server, 3000)) return 1;

    cerver_add_route(&server, "/", "web/index.html");
    cerver_add_route(&server, "/about", "web/about.html");
    cerver_add_route(&server, "/contact", "web/contact.html");

    return cerver_start(&server) ? 0 : 1;
}
```

## API Reference

### Structs

#### CerverRoute

Represents an HTTP route mapping a request path to a static file.

```c
typedef struct {
    const char *route;
    const char *file_path;
} CerverRoute;
```

#### Cerver

Represents the server instance, socket listener state, and route table.

```c
typedef struct {
    uint16_t port;
    socket_t server_fd;
    socklen_t addrlen;
    struct sockaddr_in address;
    CerverRoute routes[MAX_ROUTES];
    size_t route_count;
} Cerver;
```

### Functions

#### `bool cerver_init(Cerver *server, uint16_t port);`

Initializes platform socket libraries, sets socket options, binds to the specified port, and configures the server instance. Returns `true` on success, or `false` on failure.

- `server`: Pointer to the `Cerver` structure.
- `port`: Port number to listen on.

#### `bool cerver_add_route(Cerver *server, const char *route, const char *file_path);`

Registers an HTTP route path and its associated file to serve on the server instance. Returns `true` on success, or `false` if the maximum route limit has been reached.

- `server`: Pointer to the `Cerver` structure.
- `route`: URL path for the route.
- `file_path`: Path to the file to serve.

#### `bool cerver_start(Cerver *server);`

Starts listening on the configured port, accepts incoming connections, and automatically cleans up upon exit. Returns `false` on failure.

- `server`: Pointer to the `Cerver` structure.

#### `void cerver_shutdown(Cerver *server);`

Closes the active listening socket and executes platform socket cleanup. Automatically called by `cerver_start` upon exit, but can be invoked directly for manual teardown.

- `server`: Pointer to the `Cerver` structure.

## Contributing

Contributions are welcome. Feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
