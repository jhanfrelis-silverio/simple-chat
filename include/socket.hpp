#pragma once
#include <string>
#include <unistd.h>
#include <netinet/in.h>

struct RecvResult {
    ssize_t bytes;
    std::string data;
    int err = 0;
    bool closed() const { return bytes == 0; }
    bool ok() const { return bytes > 0; }
    bool fail() const { return bytes < 0; }
};

struct SendResult {
    ssize_t bytes;
    int err = 0;
    bool ok() const { return bytes > 0; }
    bool fail() const { return bytes < 0; }
    bool tryagain() const { return bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK); }
};

class Socket {
private:
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

    // Evita que al momento de mover un socket a otro se duplique el fd
    // Toma el valor del socket y le quita el valor del _fd al objeto original
    Socket(Socket&& socket) noexcept : _fd(socket._fd) { socket._fd = -1; }

    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            if (_fd >= 0) ::close(_fd); // cerramos el actual que poseemos
            _fd = other._fd;            // adoptamos el del otro
            other._fd = -1;             // el otro ya no es dueño
        }
        return *this;
    }

    int fd() const { return _fd; } // función para obtener el _fd del socket
    int valid() const { return _fd >= 0; } // si el fd es menor a 0, el socket es inválido
    
    static Socket createTcp(); // crea un socket y lo devuelve listo para seguir siendo usado
    bool setNonBlocking(bool on = true); // útil para trabajar con select() ; no se bloquea esperando o enviando datos
    bool setReuseAddr(bool on = true);

    SendResult send(std::string message);
    RecvResult recv();
    void close();
};