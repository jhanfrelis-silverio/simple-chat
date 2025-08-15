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

    while (true) {
        std::string message;

        std::cout << "Escribe un mensaje: ";
        getline(std::cin, message);

        client.send(message + '\n');

        RecvResult result = client.receive();

        if (result.ok()) {
            std::cout << result.data << std::endl;
        } else if (result.fail()) {
            std::cout << "Ocurrió un error al recibir el mensaje\n";
        }
    }

    return 0;
}