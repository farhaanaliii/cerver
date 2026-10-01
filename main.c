#include "server.h"

int main(void)
{
    Server server;
    init_server(&server, 3000);
    add_route(&server, "/", "web/index.html");
    add_route(&server, "/about", "web/about.html");
    start_server(&server);
    shutdown_server(&server);
    return 0;
}
