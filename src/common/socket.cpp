#include "socket.hpp"
#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<cstring>

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

SendResult Socket::send(std::string message) {
    SendResult result{};

    result.bytes = ::send(this->fd(), message.c_str(), message.length(), 0);
    
    if(result.bytes < 0)
        result.err = errno;

    return result;
}

RecvResult Socket::recv() {
    char buffer[1024];
    RecvResult result{};

    // Almacenamos los bytes enviados
    result.bytes = ::recv(this->fd(), buffer, sizeof(buffer) - 1, 0);
        
    // Si es menor a 0, es porque hubo un error
    if(result.bytes < 0) {
        result.err = errno;
        return result;
    }

    // Si el valor de bytes es 0, es porque se cerró la conexión
    if (result.bytes == 0) {
        return result;
    }

    // Si no se cumplieron ninguno de los criterios anteriores, guardamos el mensaje
    result.data.assign(
        buffer, 
        static_cast<size_t>(result.bytes)
    );

    return result;
}