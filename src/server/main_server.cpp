#include <iostream>
#include<sys/select.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include "socket.hpp"

int main() {
    Socket socket = Socket::createTcp(); // creamos el socket

    socket.bind(5000);  // conecta la ip con el puerto
    socket.listen();  // se habilita el puerto para la comunicacion

    /*fd_set master, readyFDs;
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

    for(int i = 0;i<socket.fd(); i++){
        if(FD_ISSET(i, &readyFDs)){
            sockaddr_in client{};
            socklen_t length = sizeof(client);
            int cfd = accept(i, (sockaddr*)& cli, length);
        }

    }*/

    return 0;
}