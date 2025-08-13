CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude
LDFLAGS := 

BUILD := build
SRC := src
COMMON := $(SRC)/common
SERVER := $(SRC)/server
CLIENT := $(SRC)/client

SERVER_BIN := $(BUILD)/server
CLIENT_BIN := $(BUILD)/client

COMMON_OBJS := $(BUILD)/protocol.o $(BUILD)/net.o $(BUILD)/socket.o

all: $(SERVER_BIN) $(CLIENT_BIN)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/socket.o: $(COMMON)/socket.cpp include/socket.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/protocol.o: $(COMMON)/protocol.cpp include/protocol.hpp include/json.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/net.o: $(COMMON)/net.cpp include/net.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SERVER_BIN): $(SERVER)/main_server.cpp $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(CLIENT_BIN): $(CLIENT)/main_client.cpp $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD)

.PHONY: all clean
