

# Copyright (C) 2022 Meepo All Rights Reserved.
# 
# File Name     : Makefile
# Author        : Meepo
# Creation Date : 2022-03-20
# INFO          :
# 


all: CheckInclude InsertInc RevertInc GetCommonInclude

.PHONY : all clean

CXXFLAGS = "-std=gnu++2a" "-g"

CheckInclude: CheckInclude.o
		$(CXX) $(CXXFLAGS) $^ -o $@

InsertInc: InsertInc.o
		$(CXX) $(CXXFLAGS) $^ -o $@

RevertInc: RevertInc.o
		$(CXX) $(CXXFLAGS) $^ -o $@

GetCommonInclude: GetCommonInclude.o
		$(CXX) $(CXXFLAGS) $^ -o $@

clean:
		@-rm -f  CheckInclude InsertInc RevertInc *.o
