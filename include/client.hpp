#pragma once
#include "socket.hpp"

class Client {
private:
    Socket _socket = Socket::createTcp();
    sockaddr_in _serverAddr{};
    sockaddr_in _addr{};
public:
    std::string inBuffer;
    std::string outBuffer;

    bool connect(const std::string& ip, u_int16_t port);
    bool connect(sockaddr_in& addr);

    SendResult send(std::string message) {
        return this->_socket.send(message);
    }

    RecvResult receive() {
        return this->_socket.recv();
    }
};