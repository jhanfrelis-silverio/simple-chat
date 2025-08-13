#include <iostream>
#include<sys/select.h>
#include<sys/socket.h>
#include "socket.hpp"

int main() {
    Socket socket = Socket::createTcp(); // creamos el socket

    socket.bind(5000);  //conecta la ip con el puerto
    socket.listen();  //se habilita el puerto para la comunicacion

    fd_set master, readyFDs;
    FD_ZERO(&master);

    FD_SET(socket.fd()+1, &master);
    
    while(true) {
        readyFDs = master;
        if(select(socket.fd(), &master, nullptr, nullptr, nullptr) < 0){
            if(errno == EINTR) continue;
            perror("select error");
            break;
        }
    }

    if(FD_ISSET(socket.fd(), &readyFDs)){
        sockaddr cli{};


    }

    return 0;
}