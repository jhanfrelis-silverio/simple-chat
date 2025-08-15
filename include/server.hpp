#pragma once
#include "socket.hpp"
#include "client.hpp"
#include <vector>

class Server {
private:
    fd_set _masterRead, _masterWrite;
    Socket _socket = Socket::createTcp();
    sockaddr_in _addr{};
    std::string _ip;
    int _port = -1, 
        _backlog, // backlog significa la máxima cantidad de conexiones que pueden estar en cola
        _maxfd;

    std::vector<Client> _peers;
public:
    Server(const std::string& ip, u_int16_t port, int backlog = 128) :
        _ip(ip), _port(port), _backlog(backlog) { }

    int fd() { return _socket.fd(); }
    const sockaddr_in addr() { return this->_addr; }

    bool bind();
    bool listen();
    bool start();
    bool run();

    bool receive(Client& peer);
    bool send(Client& peer);
    void broadcast(std::string message, Client& sender);

    void addClient(int fd, sockaddr_in addr);
    void removeClient(int fd);
    void calculateMaxFd();
};