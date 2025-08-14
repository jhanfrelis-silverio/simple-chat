#include "client.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

bool Client::connect(const std::string& ip, u_int16_t port) {
    sockaddr_in addr{}; //define la estructura de ipv4
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(port); //le dice el puerto que se va a estar utilizando

    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr); //convierte la ip string en binario ipv4

    return connect(addr);
}

bool Client::connect(sockaddr_in& addr) {
    if (::connect(this->_socket.fd(), (sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        return false;
    }
    
    this->_serverAddr = addr;

    return true;
}