#pragma once
#include "socket.hpp"

class Client {
private:
    Socket _socket = Socket::createTcp();
    sockaddr_in _serverAddr{};
    sockaddr_in _addr{};
public:
    Client() {};
    Client(int fd, sockaddr_in addr)
        : _socket(fd), _addr(addr) {
        _socket.setNonBlocking();
    }

    std::string inBuffer;
    std::string outBuffer;

    int fd() const { return _socket.fd(); }
    sockaddr_in addr() { return _addr; }

    bool connect(const std::string& ip, u_int16_t port);
    bool connect(sockaddr_in& addr);

    SendResult send(std::string message) {
        return _socket.send(message);
    }

    RecvResult receive() {
        return _socket.recv();
    }

    void close() {
        _socket.close();
    }
};