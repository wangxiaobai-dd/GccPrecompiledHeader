
all : aobj

CXXFLAGS += -I../common
SOURCE = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SOURCE))

$(OBJ) : %.o : %.cpp inc.h.gch
	@echo begin $(DIR)/$< >> ../$(ENTRY)
	@sleep 5
	@$(CXX) -c $(CXXFLAGS) $< -o $@
	@echo end $(DIR)/$< >> ../$(ENTRY)

aobj : $(OBJ)
	@echo beginlink $(DIR)/$@ >> ../$(ENTRY)
	@sleep 5
	@$(CXX) $(OBJ) -o $@
	@echo endlink $(DIR)/$@ >> ../$(ENTRY)

inc.h.gch : inc.h
	@echo begin $(DIR)/$< >> ../$(ENTRY)
	@sleep 5
	@$(CXX) -c $(CXXFLAGS) $< 
	@echo end $(DIR)/$< >> ../$(ENTRY)

clean:
	@rm -rf *.o aobj inc.h.gch
