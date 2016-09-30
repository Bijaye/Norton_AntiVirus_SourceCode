# Copyright 2004-2005 Symantec Corporation. All rights reserved.
# Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
# Intent: Linux/src Makefile

Root = ../../NetWare/src/

Macros = SERVER WS_32 NO_VSNPRINTF LIC=NO

Includes = $(VPReg)\
	$(PScan)\
	$(LDVP)/include\
	$(DebugNET)/include\
	$(AMS)/include\
	$(NetWare)/include\
	$(SymSafer)/include\
	$(Linux_Security)/src/symreg\
	$(CxxTest)


Sources = $(NetWare)/common/winStrings.cpp\
			 $(NetWare)/common/winThreads.cpp\
			 $(Linux)/Lock.cpp\
			 $(Linux)/Time.cpp\
			 $(Linux)/ndkConnections.cpp\
			 $(Linux)/ndkDebug.cpp\
			 $(Linux)/ndkDebugPrint.cpp\
			 $(Linux)/ndkFiles.cpp\
			 $(Linux)/ndkIntegers.cpp\
			 $(Linux)/ndkInternet.cpp\
			 $(Linux)/ndkNDSServices.cpp\
			 $(Linux)/ndkNLMFunctions.cpp\
			 $(Linux)/ndkPaths.cpp\
			 $(Linux)/ndkServerManagement.cpp\
			 $(Linux)/ndkStrings.cpp\
			 $(Linux)/ndkThreads.cpp\
			 $(Linux)/ndkTime.cpp\
			 $(Linux)/ndkVolumes.cpp\
			 $(Linux)/FileUtils.cpp\
			 $(Linux_Security)/src/symreg/ipcmessage.cpp\
			 $(Linux_Security)/src/symreg/socketcomm.cpp\
			 $(VPReg)/reg.cpp\
			 $(VPReg)/SymRegClient.cpp

LinkSources = $(Common)/common/Real_dprintf.cpp

Binary			= $(MakeMsgs)/MakeMsgs.bin
Targets			= $(Common)/pscan $(PScan)/ACTA.h
StaticSources	= $(Sources)
StaticLib		= ndkFunctions
DynamicSources = $(Sources)
DynamicLib		= ndkFunctions

testSources		= $(wildcard ndk[A-Z]*-test.c) \
				testSuite.cpp
cxxtestSources	= $(wildcard ndk[A-Z]*-test.h)
testDependents = $(testSources:%.c=%.d)
testObjects		= $(testSources:%.c=%.o)
testBinaries	= $(testSources:%.c=%.bin)
tests				= $(testSources:%.c=%.tst) \
				$(testSources:%.cpp=%.tst)


Precious = $(testDependents) $(testObjects) $(testBinaries)
Main		= %.o
List		= $(testSources)
Clean		= $(MakeMsgs)/src/MakeMsgs.o $(MakeMsgs)/src/MakeMsgs.d\
			  $(PScan)/ACTA.STR-pre $(PScan)/ACTA.h $(PScan)/ACTA.c\
			  $(PScan)/netware/rtvscan.msg $(MakeMsgs)/core.[0-9]*\
			*.d ndkDebugGlobal.o

include $(Root)Rules.mk

slibs = $(Slibs)											# Default slib gives self dependence

$(Binary) : $(MakeMsgs)/src/MakeMsgs.o\
				$(Linux)/ndkPaths.o $(Linux)/ndkTime.o $(Linux)/ndkDebugPrint.o\
				$(Linux)/ndkDebugGlobal.o $(Linux)/ndkStrings.o
	$(call mkBinary)

$(Common)/pscan :
	cd $(Common); $(LN) PScan pscan

$(PScan)/ACTA.h : $(PScan)/ACTA.STR-pre $(MakeMsgs)/MakeMsgs.ini $(Binary)
	cd $(MakeMsgs); ./MakeMsgs.bin h e

$(PScan)/ACTA.STR-pre : $(PScan)/ACTA.STR
	$(call preprocess)

-include $(MakeMsgs)/src/MakeMsgs.d

test : TEST ?= -c
test : $(tests)

testSuite.cpp: $(cxxtestSources)
	$(CxxTest)/cxxtestgen.pl --error-printer -o $@ $^ 
