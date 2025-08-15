#include <iostream>
#include "client.hpp"

int main() {
    Client client;

    if (client.connect("127.0.0.1", 5000)) {
        std::cout << "Conectado\n";
    } else {
        std::cout << "Hubo un error";
        std::cout << errno;
    }

    return 0;
}