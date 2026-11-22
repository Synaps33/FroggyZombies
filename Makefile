# FroggyZombies - Nazi Zombies: Portable for SF2000 & GB300
TARGET_NAME := nzp

all:
	$(MAKE) -C nzp_core

sf2000:
	$(MAKE) -C nzp_core platform=sf2000

clean:
	$(MAKE) -C nzp_core clean

.PHONY: all sf2000 clean
