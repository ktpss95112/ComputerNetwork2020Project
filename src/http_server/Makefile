TARGET = http_server
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -DNDEBUG
LIBS = -pthread
INC = ../include


all: $(TARGET)

$(TARGET): http_server.cpp $(INC)/server.o $(INC)/http.o handler.o
	$(CXX) $(CXXFLAGS) $(LIBS) http_server.cpp handler.o $(INC)/server.o $(INC)/http.o -o $(TARGET)

handler.o: handler.cpp handler.hpp $(INC)/http.hpp $(INC)/definitions.hpp
	$(CXX) $(CXXFLAGS) handler.cpp -c -o handler.o

$(INC)/server.o: $(INC)/server.cpp $(INC)/server.hpp
	$(CXX) $(CXXFLAGS) $(INC)/server.cpp -c -o $(INC)/server.o

$(INC)/http.o: $(INC)/http.cpp $(INC)/http.hpp $(INC)/definitions.hpp
	$(CXX) $(CXXFLAGS) $(INC)/http.cpp -c -o $(INC)/http.o

clean:
	rm -f $(INC)/*.o *.o $(TARGET)
	rm -rf py_handler/__pycache__
