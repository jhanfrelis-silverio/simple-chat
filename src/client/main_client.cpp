#include <iostream>
#include "client.hpp"

int main() {
    Client client;

    if (client.connect("127.0.0.1", 3000)) {
        std::cout << "Conectado\n";
    } else {
        std::cout << "Hubo un error";
        std::cout << errno;
    }

    int maxfd = std::max(client.fd(), STDIN_FILENO);

    while (true) {
        fd_set readfds;
        
        FD_ZERO(&readfds);
        FD_SET(client.fd(), &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        
        int readyfds = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if (readyfds < 0) {
            if (errno == EINTR)
                continue;
            perror("select");
            break;
        }

        // Verificamos si llegó algo del servidor
        if (FD_ISSET(client.fd(), &readfds)) {
            RecvResult result = client.receive();

            if (result.closed()) {
                std::cout << "El servidor cerró la conexión";
                break;
            }
            
            if (result.fail()) {
                perror("recv");
                break;
            }

            std::cout << "Servidor >> " << result.data << std::flush;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            std::string line;

            if (!std::getline(std::cin, line)) { // EOF o error en stdin
                break;
            }

            line.push_back('\n');
            SendResult result = client.send(line);

            if (result.fail()) {
                perror("send");
                break; 
            }
        }
    }

    return 0;
}