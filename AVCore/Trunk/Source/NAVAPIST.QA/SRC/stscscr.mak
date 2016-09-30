
QUAKE_CUSTOM=

.INCLUDE $(QUAKEINC)\PLATFORM.MAK

# WIN32

    TARGET_NAME=STSCSCR
    CLOPTS += /MT
    LINKOPTS += /subsystem:console
    LIBS += $(Q_SYMKRNL).LIB
    TARGET_TYPE=EXE
    PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE)
    CLOPTS += /GX
    LIBS += FILESERV.LIB
    LIBS += LOGQA.LIB
    OBJS += stscscr.obj
    OBJS += EventObj.obj
    $(PROGRAM_NAME): $(OBJS) $(LIBS)


#UPDATE#
Stscscr.obj:         stscscr.cpp
EventObj.obj:	  EventObj.cpp EventObj.h
#ENDUPDATE#
