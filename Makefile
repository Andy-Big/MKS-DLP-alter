
include config.mk

.PHONY: project clean distr

APP=MKSDLP_Alter


FIRMWARE=bin/$(APP).elf
GDB=$(TOOLCHAINPATH)/bin/arm-none-eabi-gdb

all : debug

build/Makefile : project

project:
		premake5 --scripts=premake arm_make 
		
	
debug: 
		PATH=$(TOOLCHAINPATH)/bin:${PATH} $(MAKE) -C build verbose=1 $(APP)

release:
		PATH=$(TOOLCHAINPATH)/bin:${PATH} $(MAKE) -C build verbose=1 config=release $(APP)


clean:
		$(MAKE) -C build clean
		$(MAKE) -C build clean config=release
	

distr:
	mkdir -p distr
	cp bin/$(APP).bin distr/mksDLP.bin
	mkdir -p distr/alterupd/images/btn
	mkdir -p distr/alterupd/images/scr
	mkdir -p distr/alterupd/images/icn
	cp IMGS/original/btn_*.cimg distr/alterupd/images/btn/
	cp IMGS/original/scr_*.cimg distr/alterupd/images/scr/
	cp IMGS/original/logo_*.cimg distr/alterupd/images/scr/
	cp IMGS/original/icn_*.cimg distr/alterupd/images/icn/
	cp IMGS/original/uvtest_*.cimg distr/alterupd/images/icn/


			