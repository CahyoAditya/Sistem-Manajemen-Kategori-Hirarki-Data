CXX = g++
CXXFLAGS = -O2 -std=c++11
SOURCES = main.cpp ui.cpp data_structures.cpp
TARGET = main

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
