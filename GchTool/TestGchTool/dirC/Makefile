
CXXFLAGS += -I../common
SOURCE = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SOURCE))

all : aobj

include inc.d
inc.d :inc.h
	@set -e; rm -f $@; \
		$(CXX) -M $(CXXFLAGS) $< > $@.$$$$;\
		sed 's,\($*\)\.o[ :]*,\1.h.gch $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

$(OBJ) : %.o : %.cpp inc.h.gch
	$(CXX) -c $(CXXFLAGS) $< -o $@

aobj : $(OBJ)
	$(CXX) $(OBJ) -o $@

inc.h.gch : inc.h
	$(CXX) -c $(CXXFLAGS) $< 

clean:
	@rm -rf *.o aobj inc.h.gch
