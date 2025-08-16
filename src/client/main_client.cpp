#include "client.hpp"
#include "terminal.hpp"
#include "functions.hpp"
#include <iostream>

int main() {
    Client client;

    if (!client.connect("127.0.0.1", 3000)) {
        perror("connect");
        return 1;
    }

    std::cout << "Conectado\n";

    // client.startChatting(); por implementar

    RawMode raw; // Cambiamos el comportamiento de la consola.
    std::string currentInput;
    
    std::cout << ">> " << std::flush;
    
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

            printIncomingAndRestore(result.data, currentInput);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            // Evaluamos caracter por caracter
            char ch;

            while (read(STDIN_FILENO, &ch, 1) == 1) {
                if (ch == '\r' || ch == '\n') {
                    // Verificamos que la linea actual no esté vacía
                    std::string trimmed = trim(currentInput);

                    if (trimmed.empty()) {
                        break;
                    }
                    
                    // Enviamos la línea actual
                    std::string line = trimmed;
                    line.push_back('\n');

                    SendResult result = client.send(line);

                    if (result.fail()) {
                        perror("send");
                        return 1;
                    }

                    std::cout << "\33[2K\r";
                    std::cout << "Yo >> " << line;
                    currentInput.clear();
                    std::cout << ">> " << std::flush;
                } else if (ch == 127 || ch == 8) {
                    // Backspace
                    if (!currentInput.empty()) {
                        currentInput.pop_back();
                        clearLineAndPrompt(currentInput);
                    }
                } else {
                    currentInput.push_back(ch);
                    std::cout << ch << std::flush;
                }
            }
        }
    }

    return 0;
}