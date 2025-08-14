#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.hpp"
#include "client.hpp"
#include "socket.hpp"

int main() {
    Server server = Server("127.0.0.1", 5000);
    server.start();

    sockaddr_in serverAddr = server.addr(), clientAddr;
    socklen_t clientLength = sizeof(clientAddr);

    Client client = Client();

    client.connect(serverAddr);
    
    int acceptedFd = ::accept(server.fd(), (sockaddr*)&clientAddr, &clientLength);
    if (acceptedFd < 0) {
        perror("accept");
        return 1;
    }

    Socket peer(acceptedFd);

    std::cout << clientLength << std::endl;

    if (client.send("hola").ok()) {
        std::cout << "Mensaje enviado satisfactoriamente\n";
    } else {
        std::cout << "El mensaje no fue enviado correctamente\n";
    }

    RecvResult result = peer.recv();

    if (result.ok()) {
        std::cout << "Mensaje recibido desde el cliente.\n";
        std::cout << "Mensaje: " << result.data << std::endl;
    }

    peer.send("Te habla el servidor");

    RecvResult resultClient = client.receive();

    if (resultClient.ok()) {
        std::cout << "Mensaje recibido desde el cliente.\n";
        std::cout << "Mensaje: " << resultClient.data << std::endl;
    }

    return 0;
}