ifndef TARGET_PLATFORM
   TARGET_PLATFORM=solaris
endif

ifndef PLATFORM
   PLATFORM=solaris
endif
 
ifdef MK_DEBUG
   PROG_TARGET=../bin/$(TARGET_PLATFORM)/debug/SAVScan
else
   PROG_TARGET=../bin/$(TARGET_PLATFORM)/release/SAVScan
endif

PROG_OBJS= \
savscan.o

PROJ_DEP=\
../include/Standard.h

include ../CM/make.rul

