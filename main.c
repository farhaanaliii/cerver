#include "cerver.h"

int main(void)
{
    Cerver server;
    cerver_init(&server, 3000);
    cerver_add_route(&server, "/", "web/index.html");
    cerver_add_route(&server, "/about", "web/about.html");
    cerver_start(&server);
    cerver_shutdown(&server);
    return 0;
}
