ifndef TARGET_PLATFORM
   TARGET_PLATFORM=solaris
endif

ifndef PLATFORM
   PLATFORM=solaris
endif
 
ifdef MK_DEBUG
   PROG_TARGET=../bin/$(TARGET_PLATFORM)/debug/SAVQuar
else
   PROG_TARGET=../bin/$(TARGET_PLATFORM)/release/SAVQuar
endif

PROG_OBJS= \
savquar.o

PROJ_DEP=\
../include/Standard.h

include ../CM/make.rul

