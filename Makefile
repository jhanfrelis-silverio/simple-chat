CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude
LDFLAGS := 

# === Directorios ===
BUILD := build
SRC := src
COMMON := $(SRC)/common
SERVER := $(SRC)/server
CLIENT := $(SRC)/client

SERVER_BIN := $(BUILD)/server
CLIENT_BIN := $(BUILD)/client

# === Objetos comunes ===
COMMON_OBJS := $(BUILD)/protocol.o \
               $(BUILD)/net.o \
               $(BUILD)/socket.o

# === Objetos específicos ===
# hay que remover el $build/client.o de server_objs -> estoy testeando !
SERVER_OBJS := $(COMMON_OBJS) $(BUILD)/server.o $(BUILD)/main_server.o $(BUILD)/client.o
CLIENT_OBJS := $(COMMON_OBJS) $(BUILD)/client.o $(BUILD)/main_client.o

# === Regla por defecto ===
all: $(SERVER_BIN) $(CLIENT_BIN)

# === Crear directorio build ===
$(BUILD):
	mkdir -p $(BUILD)

# === Compilación de objetos comunes ===
$(BUILD)/socket.o: $(COMMON)/socket.cpp include/socket.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/protocol.o: $(COMMON)/protocol.cpp include/protocol.hpp include/json.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/net.o: $(COMMON)/net.cpp include/net.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Compilación de objetos del servidor ===
$(BUILD)/server.o: $(SERVER)/server.cpp include/server.hpp include/socket.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/main_server.o: $(SERVER)/main_server.cpp include/server.hpp include/socket.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Compilación de objetos del cliente ===
$(BUILD)/client.o: $(CLIENT)/client.cpp include/client.hpp include/socket.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/main_client.o: $(CLIENT)/main_client.cpp include/client.hpp include/socket.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Enlazado final ===
$(SERVER_BIN): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# === Limpieza ===
clean:
	rm -rf $(BUILD)

.PHONY: all clean
