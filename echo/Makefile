CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = echo_serv
SRC      = echo_serv.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
