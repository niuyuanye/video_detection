# Auth: lijd
# Date: 2021/02/24
 
CC       := g++
TARGET   := libjsoncpp.so
INCLUDES := ../../include
CXXFLAGS = -O3 -Wall  -fPIC
FLAGES   := $(addprefix -I, $(INCLUDES))
 
SRCS  	 := $(wildcard *.cpp)
 
$(TARGET): $(SRCS)
	$(CC) -shared -fPIC -o $@ $^ $(FLAGES) $(CXXFLAGS)
	@echo "==================================== Build OK!!! ===================================="
 
.PHONY: clean
clean:
	@rm -rf $(TARGET)