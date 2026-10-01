#include "cerver.h"

int main(void)
{
    Cerver server;
    if(!cerver_init(&server, 3000)){
        return 1;
    }

    cerver_add_route(&server, "/", "web/index.html");
    cerver_add_route(&server, "/about", "web/about.html");

    return cerver_start(&server) ? 0 : 1;
}
