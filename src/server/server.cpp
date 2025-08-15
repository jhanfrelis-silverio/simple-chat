#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

bool Server::start() {
    _socket.setReuseAddr();

    if (!bind())
        return false;

    if (!listen())
        return false;

    // Inicializamos el set actual
    FD_ZERO(&_masterRead);
    FD_ZERO(&_masterWrite);
    FD_SET(_socket.fd(), &_masterRead);

    // Colocamos el máximo de los fds
    _maxfd = _socket.fd() + 1;
    
    return true;
}

bool Server::run() {
    while (true) {
        fd_set readfds = _masterRead; // Hacemos una copia porque select es destructivo.
        fd_set writefds = _masterWrite;

        // Solo queremos leer, por lo tanto solo colocamos el parámetro de leer.
        int readyfds = select(_maxfd + 1, &readfds, &writefds, NULL, NULL);

        if (readyfds < 0) {
            if (errno == EINTR) // EINTR -> INTERRUPTED
                continue; // reintentar
            perror("select");
            return false;
        }

        // Verificamos si hay una nueva conexión, si la hay, añadimos el cliente
        if (FD_ISSET(_socket.fd(), &readfds)) {
            sockaddr_in clientAddr;
            socklen_t len = sizeof(clientAddr);

            int cfd = ::accept(_socket.fd(), (sockaddr*)&clientAddr, &len);

            if (cfd > _maxfd)
                _maxfd = cfd;

            addClient(cfd, clientAddr);
        }

        for (size_t i = 0; i < _peers.size(); i++) {
            // Verificamos cuales están listos para ser leídos y para escribirles.
            // Llamamos a la función que lee y envía información para cada peer.
            // Si  retorna false, se tiene que eliminar ese peer.

            if (FD_ISSET(_peers[i].fd(), &readfds)) {
                if (!receive(_peers[i])) {
                    removeClient(_peers[i].fd());
                    continue;
                }
            }

            if (FD_ISSET(_peers[i].fd(), &writefds)) {
                if (!send(_peers[i])) {
                    removeClient(_peers[i].fd());
                    continue;
                }
            }
        }
    }

    return true;
}

bool Server::listen() {
    // Verificar que el socket sea válido y tenga un puerto
    if (::listen(_socket.fd(), _backlog) != 0) {
        perror("listen");
        return false;
    }

    std::cout << "Socket escuchando en: " << _ip << ":" << _port << std::endl;

    return true;
};

bool Server::bind() {
    sockaddr_in addr{}; //define la estructura de ipv4
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(_port); //le dice el puerto que se va a estar utilizando

    inet_pton(AF_INET, _ip.c_str(), &addr.sin_addr); //convierte la ip string en binario ipv4

    int result = ::bind(_socket.fd(), (sockaddr*)&addr, sizeof(addr)); 

    if (result == 0) {
        _addr = addr;

        std::cout << "Puerto bindeado " << _port << " en la ip " << _ip << std::endl;

        return true;
    }

    return false;
}

bool Server::receive(Client& peer) {
    // Recibimos la información del peer en específico
    RecvResult result = peer.receive();

    // Si hay un fallo, verificamos si es un error en específico para seguir intentándolo
    if (result.closed()) {
        return false;
    }

    if (result.fail()) {
        if (result.err == EAGAIN || result.err == EWOULDBLOCK ) // Seguir intentándolo
            return true;
        perror("recv");
        return false;
    }

    // Vamos guardando lo que vamos recibiendo
    peer.inBuffer.append(result.data);

    // Comprobamos si hay un salto de línea
    size_t start = 0;
    
    while (true) {
        size_t endline = peer.inBuffer.find('\n', start);

        // Verificamos si no fue encontrado el salto de línea: los datos están incompletos.
        if (endline == std::string::npos) {
            // Si ya hemos guardado algo, lo eliminamos para que solo quede
            // la parte sin procesar 
            if (start > 0) {
                peer.inBuffer.erase(0, start);
            }

            break;
        }

        // Obtenemos la línea
        std::string line = peer.inBuffer.substr(start, endline - start + 1);
        
        // Actualizamos la posición de start
        start = endline + 1;

        broadcast(line, peer);
    }

    if (start > 0) {
        if (start < peer.inBuffer.size()) 
            peer.inBuffer.erase(0, start);
        else 
            peer.inBuffer.clear();
    }

    return true;
}

bool Server::send(Client& peer) {
    if (peer.outBuffer.empty()) {
        FD_CLR(peer.fd(), &_masterWrite);
        return true;
    }

    SendResult result = peer.send(peer.outBuffer);

    if (result.bytes >= 0) {
        peer.outBuffer.erase(0, result.bytes);

        if (peer.outBuffer.empty())
            FD_CLR(peer.fd(), &_masterWrite);

        return true;
    }
    
    if (result.tryagain()) {
        return true;
    }

    perror("send");
    return false;
}

void Server::broadcast(std::string message, Client& sender) {
    for (auto &peer : _peers) {
        if (peer.fd() == sender.fd())
            continue;
        peer.outBuffer.append(message);
        FD_SET(peer.fd(), &_masterWrite);
    }
}

void Server::addClient(int cfd, sockaddr_in addr) {
    if (cfd > _maxfd)
        _maxfd = cfd;

    FD_SET(cfd, &_masterRead);

    Client peer(cfd, addr);
    _peers.push_back(std::move(peer));

    sockaddr_in peerAddr = peer.addr();
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &peerAddr.sin_addr, ip, sizeof(ip));
    std::cout << "Nuevo cliente " << cfd << " desde " << ip
              << ":" << ntohs(peerAddr.sin_port) << "\n";
}

void Server::removeClient(int fd) {
    for(auto current = _peers.begin(); current != _peers.end(); current++) {
        if (current->fd() == fd) {
            current->close();

            FD_CLR(fd, &_masterRead);
            FD_CLR(fd, &_masterWrite);

            _peers.erase(current);

            calculateMaxFd();
            return;
        }
    }
}

void Server::calculateMaxFd() {
    _maxfd = _socket.fd();

    for (auto &peer : _peers) {
        if (peer.fd() > _maxfd)
            _maxfd = peer.fd();
    }
}