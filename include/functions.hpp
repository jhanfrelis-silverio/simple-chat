#include <string>

std::string trim(const  std::string& str) {
    std::string copy = str;
    
    copy.erase(0, copy.find_first_not_of(" \t\r\n"));
    copy.erase(copy.find_last_not_of(" \t\r\n") + 1);

    return copy;
}