CXX = g++
CXXFLAGS = -std=c++11 -Wall

TARGET = hw2_chat_server
SOURCE = hw2_chat_server.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
