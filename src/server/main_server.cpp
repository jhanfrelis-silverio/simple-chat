#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.hpp"
#include "client.hpp"
#include "socket.hpp"

int main() {
    Server server = Server("127.0.0.1", 3000);
    server.start();
    server.run();

    return 0;
}