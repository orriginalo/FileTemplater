CXX = g++
CXXFLAGS = -Oz -Iinclude
SRC_DIR = ./src
INCLUDE_DIR = ./include
SRC = $(wildcard $(SRC_DIR)/*.cpp)
DST = FileTemplater

# Определяем платформу
ifeq ($(OS),Windows_NT)
    RM = del /q
    RUN = .\$(DST).exe
    STRIP = strip $(DST).exe
else
    RM = rm -f
    RUN = ./$(DST)
    STRIP = strip $(DST)
endif

app: $(SRC)
	$(CXX) $(CXXFLAGS) -o $(DST) $(SRC)
	$(STRIP)
	$(RUN)

.PHONY: clean format superbuild

format:
	clang-format -i $(SRC_DIR)/*.cpp $(INCLUDE_DIR)/*.h

clean:
	$(RM) $(DST) 

# superbuild: $(SRC)
# 	$(CXX) $(CXXFLAGS) -o $(DST) $(SRC)
# 	strip $(DST).exe
# 	wsl $(CXX) $(CXXFLAGS) -o $(DST) $(SRC)
# 	strip $(DST)