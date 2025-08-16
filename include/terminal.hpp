#include <unistd.h>
#include <termios.h>
#include <string>

struct RawMode {
    termios oldconfig{};
    bool ok = false;
    RawMode() {
        if (tcgetattr(STDIN_FILENO, &oldconfig) == 0) {
            termios config = oldconfig;
            // La canonalidad permite que los carácteres se lean en tiempo real
            config.c_lflag &= ~(ECHO | ICANON); // Remover la funcionalidad de echo y canonalidad
            config.c_cc[VMIN] = 0; // Carácteres minimos para aplicar la no canonalidad
            config.c_cc[VTIME] = 0; // Tiempo mínimo para aplicar la no canonalidad

            // TCSANOW -> El cambio ocurre desde que se aplique
            if (tcsetattr(STDIN_FILENO, TCSANOW, &config))
                ok = true;
        }
    }

    ~RawMode() {
        if (ok)
            tcsetattr(STDIN_FILENO, TCSANOW, &oldconfig);
    }
};

void clearLineAndPrompt(const std::string& currentInput);
void printIncomingAndRestore(const std::string& message, const std::string& currentInput);