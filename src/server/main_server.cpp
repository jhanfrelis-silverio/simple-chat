#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.hpp"
#include "client.hpp"
#include "socket.hpp"

int main() {
    Server server = Server("127.0.0.1", 5000);
    server.start();

    sockaddr_in serverAddr = server.addr(), clientAddr;
    socklen_t clientLength = sizeof(clientAddr);

    Client client = Client();

    client.connect(serverAddr);
    
    int acceptedFd = ::accept(server.fd(), (sockaddr*)&clientAddr, &clientLength);
    if (acceptedFd < 0) {
        perror("accept");
        return 1;
    }

    fd_set master, readyFDs;  //creamos 2 sets de fds
    FD_ZERO(&master);         //vaciamos el set principal
    int highFD = socket.fd(); //tomamos creamos una variable para guardar el fd mas grande
    FD_SET(highFD+1, &master);//agregamos el fd al set principal


    
    while(true) {
        readyFDs = master;  //creamos una copia del set principal en cada iteracion
        if(select(highFD, &master, nullptr, nullptr, nullptr) < 0){//select va a revisar los fd que tengan mensaje
            if(errno == EINTR) continue;      //va a revisar si el error fue por ser interrumpido
            perror("select error");  
            break;       
        }


        std::vector<int> clientes;  //guardamos los fds de clientes en un vector 

        for(int i = 0;i<highFD; i++) {  //revisamos todos los fds menores al highfd
            int cfd;    //clientfd
            if(FD_ISSET(i, &readyFDs)) { //si el fd por el que vamos esta en el set de fds
                sockaddr_in client{};   //se crea la estructura ipv4 del cliente
                socklen_t length = sizeof(client); //el tamaño de la adress del cliente
                cfd = accept(i, (sockaddr*)&client, &length);//accept nos da un fd para hablar con el cliente
        
                if(cfd >= 0) {   //si es valido 
                    FD_SET(cfd, &master); //ponemos el nuevo fd en el master fd_set
                    clientes.push_back(cfd); //y lo agregamos al vector de clientes
                    if(highFD < cfd) highFD = cfd; //si el client fd es mayor al highFD se actualiza

                    //imprimir el fd, la ip y el puerto del nuevo cliente
                    char ip[INET_ADDRSTRLEN];  
                    inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
                    std::cout << "Nuevo cliente " << cfd << " desde " << ip
                                << ":" << ntohs(client.sin_port) << "\n";
                }
            }
        }

        for(auto i = clientes.begin(); i != clientes.end(); ) {
            int sd = *i;
            if(!FD_ISSET(sd, &readyFDs)) continue;

            RecvResult value = socket.recv();  //hay que mandarle el fd del socket cliente no el socket
            if(value.closed()) {
                std::cout<<"Cliente desconectado: "<<sd<<std::endl;
                close(sd);
                i = clientes.erase(i);
            }
            else if(value.fail());
            else {
                std::cout<<"Mensaje recibido de: "<<sd<<std::endl
                         <<'"'<<value.data<<'"'<<std::endl;
                std::string message;
                getline(std::cin,message);
                if(socket.send(  ,message)) std::cout<<"\nMensaje enviado"; 
                //hay que mandarle el fd del socket cliente no el socket
            }
        }
    }
    Socket peer(acceptedFd);

    std::cout << clientLength << std::endl;

    if (client.send("hola").ok()) {
        std::cout << "Mensaje enviado satisfactoriamente\n";
    } else {
        std::cout << "El mensaje no fue enviado correctamente\n";
    }

    RecvResult result = peer.recv();

    if (result.ok()) {
        std::cout << "Mensaje recibido desde el cliente.\n";
        std::cout << "Mensaje: " << result.data << std::endl;
    }

    peer.send("Te habla el servidor");

    RecvResult resultClient = client.receive();

    if (resultClient.ok()) {
        std::cout << "Mensaje recibido desde el cliente.\n";
        std::cout << "Mensaje: " << resultClient.data << std::endl;
    }

    return 0;
}