#include <iostream>
#include "socket.hpp"

int main() {
    Socket socket = Socket::createTcp(); // creamos el socket

    socket.bind(5000);
    socket.listen();
    
    return 0;
}