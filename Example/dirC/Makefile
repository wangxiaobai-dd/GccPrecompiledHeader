
all : cobj

SOURCE = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SOURCE))

$(OBJ) : %.o : %.cpp
	@echo begin $(DIR)/$< >> ../$(ENTRY)
	@sleep 5
	@$(CXX) -c $< -o $@
	@echo end $(DIR)/$< >> ../$(ENTRY)

cobj : $(OBJ)
	@echo beginlink $(DIR)/$@ >> ../$(ENTRY)
	@sleep 5
	@$(CXX) $(OBJ) -o $@
	@echo endlink $(DIR)/$@ >> ../$(ENTRY)

clean:
	@rm -rf *.o cobj
