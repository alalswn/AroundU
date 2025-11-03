
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude -Ithird_party

# 모든 .cpp 수집 (서브폴더 포함)
SRC = $(shell find src -name '*.cpp')
TARGET = program

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
