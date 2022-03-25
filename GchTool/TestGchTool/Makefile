
all : targetA targetB targetC

DIRA = dirA
DIRB = dirB
DIRC = dirC

GCHDIRS = dirA  

export CXXFLAGS = -Werror -Wall -Winvalid-pch

targetA : 
	#@python2.7 ./gchcheck.py $(GCHDIRS)
	@ENTRY='$@file' DIR=$(DIRA) $(MAKE) -C $(DIRA)
	@sleep 2

targetB : targetA
	@ENTRY='$@file' DIR=$(DIRB) $(MAKE) -C $(DIRB)

targetC : targetA
	@ENTRY='$@file' DIR=$(DIRC) $(MAKE) -C $(DIRC)

clean:
	@$(MAKE) -C $(DIRA) clean;
	@$(MAKE) -C $(DIRB) clean;
	@$(MAKE) -C $(DIRC) clean;
