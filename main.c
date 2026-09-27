#include "server.h"
#include <stdlib.h>

int main(void)
{
    Server server;
    init_server(&server, 3000);
    add_route("/", "web/index.html");
    add_route("/about", "web/about.html");
    start_server(&server);
    shutdown_server(&server);
    return 0;
}
