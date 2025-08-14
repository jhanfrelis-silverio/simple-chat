#pragma once
#include "socket.hpp"
#include "client.hpp"

class Server {
private:
    Socket _socket = Socket::createTcp();
    std::string _ip;
    int _port = -1, _backlog;
    sockaddr_in _addr{};
public:
    Server(const std::string& ip, u_int16_t port, int backlog = 128) {
        this->_ip = ip;
        this->_port = port;
        this->_backlog = backlog;

        // Inicialización del select
    }

    int fd() { return _socket.fd(); } // testing
    const sockaddr_in addr() { return this->_addr; }

    bool bind();
    bool listen(); // baog significa la máxima cantidad de conexiones que pueden estar en cola
    bool start();

    bool receive(Client& client);
    bool send(Client& client);
};