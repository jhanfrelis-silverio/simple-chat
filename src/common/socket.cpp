#include "socket.hpp"
#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>

Socket Socket::createTcp() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd <= 0) {
        std::cerr << "Error creating socket\n";
    }
    return Socket(socketfd);
}

bool Socket::setNonBlocking(bool on) {
    int flags = fcntl(this->_fd, F_GETFL, 0);
    return fcntl(this->_fd, F_SETFL, flags | O_NONBLOCK) == 0;
}

bool Socket::setReuseAddr(bool on) {
    int option = on ? 1 : 0;
    return ::setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
}

bool Socket::listen(int backlog) {
    // Verificar que el socket sea válido y tenga un puerto
    if (::listen(this->_fd, this->_port) != 0) {
        // mostrar error
        perror("listen");
        return false;
    }

    std::cout << "Socket escuchando en: " << this->_ip << ":" << this->_port << std::endl;

    return true;
};

bool Socket::bind(u_int16_t port) {
    return Socket::bind("0.0.0.0", port);
}

bool Socket::bind(const std::string& ip, u_int16_t port) {
    sockaddr_in addr{}; //define la estructura de ipv4
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(port); //le dice el puerto que se va a estar utilizando

    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr); //convierte la ip string en binario ipv4

    int result = ::bind(_fd, (sockaddr*)&addr, sizeof(addr)); //

    if (result == 0) {
        // Guardar ip y puerto
        this->_port = port;
        this->_ip = ip;

        std::cout << "Puerto bindeado " << port << " en la ip " << ip << std::endl;

        return true;
    }

    return false;
}