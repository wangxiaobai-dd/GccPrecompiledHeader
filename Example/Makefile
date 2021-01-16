
all : targetA targetB targetC

DIRA = dirA
DIRB = dirB
DIRC = dirC

GCHDIRS = dirA dirB 

export CXXFLAGS = -Werror -Wall -Winvalid-pch

targetA : 
	@python2.7 ./gchcheck.py $(GCHDIRS)
	@python2.7 ./compliedisplay.py targetA targetB targetC
	@ENTRY='$@file' DIR=$(DIRA) $(MAKE) -C $(DIRA) 2>>$@error
	@sleep 2
	@echo finish >> $@file

targetB : targetA
	@ENTRY='$@file' DIR=$(DIRB) $(MAKE) -C $(DIRB) 2>>$@error
	@echo finish >> $@file

targetC : targetA
	@ENTRY='$@file' DIR=$(DIRC) $(MAKE) -C $(DIRC) 2>>$@error
	@echo finish >> $@file

clean:
	@$(MAKE) -C $(DIRA) clean;
	@$(MAKE) -C $(DIRB) clean;
	@$(MAKE) -C $(DIRC) clean;
