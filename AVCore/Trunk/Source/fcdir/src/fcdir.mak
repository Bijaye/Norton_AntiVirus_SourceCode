.INCLUDE $(QUAKEINC)\PLATFORM.MAK

####################################################
#                     IRA                          #
####################################################
%If ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
    TARGET_NAME=FCDIR32
    CLOPTS += /MT
    LINKOPTS += /subsystem:console
    TARGET_TYPE=EXE
    PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE)
    CLOPTS += /GX

    $(PROGRAM_NAME):                \
                        fcdir.obj   \
                        fileops.obj \
                        shared.obj


####################################################
#                     DX                           #
####################################################
%ElseIf ("$[u,$(PLATFORM)]" == ".DX")

    TARGET_NAME=FCDIR
    MODEL = L
    TARGET_TYPE=EXE
    PROGRAM_NAME = $(TARGET_NAME).$(TARGET_TYPE)
    
    $(PROGRAM_NAME):                \
                        fcdir.obj   \
                        fileops.obj \
                        shared.obj

####################################################
#                     NLM                          #
####################################################
%ElseIf ("$[u,$(PLATFORM)]" == ".NLM")

    TARGET_NAME=FCDIR
    TARGET_TYPE=NLM
    PROGRAM_NAME = $(TARGET_NAME).$(TARGET_TYPE)

    $(PROGRAM_NAME):                \
                        fcdir.obj   \
                        fileops.obj \
                        shared.obj  \
                        fcdir.den

%EndIf



#UPDATE#
fcdir.obj:          fcdir.cpp  fcdir.h
fileops.obj:        fileops.cpp fileops.h
shared.obj:         shared.cpp shared.h
#ENDUPDATE#
