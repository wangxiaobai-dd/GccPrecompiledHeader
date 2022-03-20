
all : aobj

CXXFLAGS += -I../common
SOURCE = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SOURCE))

$(OBJ) : %.o : %.cpp inc.h.gch
	$(CXX) -c $(CXXFLAGS) $< -o $@

aobj : $(OBJ)
	$(CXX) $(OBJ) -o $@

inc.h.gch : inc.h
	$(CXX) -c $(CXXFLAGS) $< 

clean:
	@rm -rf *.o aobj inc.h.gch
