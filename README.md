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

Include `server.h` in your code and compile `server.c` alongside your project files.

## Building

On Windows (MinGW / GCC):

```bash
gcc main.c server.c -o cerver.exe -lws2_32
```

On Linux and macOS:

```bash
gcc main.c server.c -o cerver
```

## Usage

```c
#include "server.h"

int main(void) {
    Server server;
    init_server(&server, 3000);

    add_route(&server, "/", "web/index.html");
    add_route(&server, "/about", "web/about.html");
    add_route(&server, "/contact", "web/contact.html");
    start_server(&server);
    shutdown_server(&server);

    return 0;
}
```

## API Reference

### Structs

#### Route

Represents an HTTP route mapping a request path to a static file.

```c
typedef struct {
    const char *route;
    const char *file_path;
} Route;
```

#### Server

Represents the server instance, socket listener state, and route table.

```c
typedef struct {
    int port;
    socket_t server_fd;
    int addrlen;
    struct sockaddr_in address;
    Route routes[MAX_ROUTES];
    int route_count;
} Server;
```

### Functions

#### `void init_server(Server *server, int port);`

Initializes platform socket libraries, sets socket options, binds to the specified port, and configures the server instance.

- `server`: Pointer to the `Server` structure.
- `port`: Port number to listen on.

#### `void add_route(Server *server, const char *route, const char *file_path);`

Registers an HTTP route path and its associated file to serve on the server instance.

- `server`: Pointer to the `Server` structure.
- `route`: URL path for the route.
- `file_path`: Path to the file to serve.

#### `void start_server(Server *server);`

Starts listening on the configured port and accepts incoming connections.

- `server`: Pointer to the `Server` structure.

#### `void shutdown_server(Server *server);`

Closes the active listening socket and executes platform socket cleanup.

- `server`: Pointer to the `Server` structure.

## Contributing

Contributions are welcome. Feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
