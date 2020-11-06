TARGET = text_server
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -DNDEBUG
LIBS = -pthread
INC = ../include

all: $(TARGET)

$(TARGET): text_server.cpp $(INC)/server.o
	$(CXX) $(CXXFLAGS) $(LIBS) text_server.cpp $(INC)/server.o -o $(TARGET)

$(INC)/server.o: $(INC)/server.cpp $(INC)/server.hpp
	$(CXX) $(CXXFLAGS) $(INC)/server.cpp -c -o $(INC)/server.o

clean:
	rm -f $(INC)/*.o *.o $(TARGET)
