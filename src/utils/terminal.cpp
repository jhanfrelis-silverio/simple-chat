#include "terminal.hpp"
#include <iostream>
#include <string>

void clearLineAndPrompt(const std::string& currentInput) {
    // Limpia la línea completa. 
    // \33 -> Viene una entrada especial
    // [2K -> Borra la línea completa
    // \r  -> Lleva el cursor al inicio de la línea sin salto de línea
    std::cout << "\33[2K\r"; 
    std::cout << ">> " << currentInput << std::flush;
}

void printIncomingAndRestore(const std::string& message, const std::string& currentInput) {
    std::cout << "\33[2K\r"; 
    std::cout << "Servidor >> " << message;
    
    if (message.empty() || message.back() != '\n')
        std::cout << "\n";

    std::cout << ">> " << currentInput << std::flush;
}