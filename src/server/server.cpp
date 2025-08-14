#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

bool Server::start() {
    this->_socket.setReuseAddr();

    if (!bind())
        return false;

    if (!listen())
        return false;
    
    return true;
}

bool Server::listen() {
    // Verificar que el socket sea válido y tenga un puerto
    if (::listen(this->_socket.fd(), this->_backlog) != 0) {
        // mostrar error
        perror("listen");
        return false;
    }

    std::cout << "Socket escuchando en: " << this->_ip << ":" << this->_port << std::endl;

    return true;
};

bool Server::bind() {
    sockaddr_in addr{}; //define la estructura de ipv4
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(this->_port); //le dice el puerto que se va a estar utilizando

    inet_pton(AF_INET, this->_ip.c_str(), &addr.sin_addr); //convierte la ip string en binario ipv4

    int result = ::bind(this->_socket.fd(), (sockaddr*)&addr, sizeof(addr)); 

    if (result == 0) {
        this->_addr = addr;

        std::cout << "Puerto bindeado " << this->_port << " en la ip " << this->_ip << std::endl;

        return true;
    }

    return false;
}

bool Server::receive(Client& client) {


    return true;
}

bool Server::send(Client& client) {
    

    return true;
}