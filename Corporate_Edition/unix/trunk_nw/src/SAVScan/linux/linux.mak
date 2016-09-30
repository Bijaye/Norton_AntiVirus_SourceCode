LIB_TARGET=../Shared/Lib/$(PLATFORM)/libdec2http.a

LIB_OBJS= \
Dec2HTTP.o \
HTTPEx.o \
HTTPFile.o

include ../CM/make.rul

