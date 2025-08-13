#pragma once
#include <string>
#include <unistd.h>

class Socket {
    int _fd = -1;
public:
    Socket() = default; // Crea un objeto por defecto
    explicit Socket(int fd) : _fd(fd) {} // Inicializamos _fd con fd

    // Solo si hay un fd válido lo destruye
    ~Socket() {
        if (_fd >= 0)
            ::close(_fd);
    }

    // Para evitar copias que contengan el mismo _fd, evitando error de que se destruyan al mismo
    // tiempo
    Socket(const Socket& ) = delete;
    Socket& operator=(const Socket&) = delete;

    // Evita que al momento de mover un socket a otro, toma el valor del socket y le quita el valor del
    // _fd al objeto original
    Socket(Socket&& socket) noexcept : _fd(socket._fd) { socket._fd = -1; }

    int fd() const { return _fd; } // función para obtener el _fd del socket
    int valid() const { return _fd >= 0; } // si el fd es menor a 0, el socket es inválido
    bool bind(u_int16_t port);  // bindea el puerto con el socket
    bool listen(int backlog = 128); // backlog significa la máxima cantidad de conexiones que pueden estar en cola
    
    static Socket createTcp(); // crea un socket y lo devuelve listo para seguir siendo usado
    bool setNonBlocking(bool on = true); // útil para trabajar con select() ; no se bloquea esperando o enviando datos
    bool setReuseAddr(bool on = true);
};