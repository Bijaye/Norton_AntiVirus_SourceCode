.INCLUDE $(QUAKEINC)\PLATFORM.MAK

####################################################
#                     IRA                          #
####################################################
%If ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
    TARGET_NAME=CHGVDATE32
    CLOPTS += /MT
    LINKOPTS += /subsystem:console
    TARGET_TYPE=EXE
    PROGRAM_NAME    = $(TARGET_NAME).$(TARGET_TYPE)
    CLOPTS += /GX

    $(PROGRAM_NAME):                \
                        chgvdate.obj

####################################################
#                     DX                           #
####################################################
%ElseIf ("$[u,$(PLATFORM)]" == ".DX")

    TARGET_NAME=CHGVDATE
    MODEL = L
    TARGET_TYPE=EXE
    PROGRAM_NAME = $(TARGET_NAME).$(TARGET_TYPE)
    CLOPTS += /GX

    $(PROGRAM_NAME):                \
                        chgvdate.obj



####################################################
#                     NLM                          #
####################################################
%ElseIf ("$[u,$(PLATFORM)]" == ".NLM")

    TARGET_NAME=CHGVDATE
    TARGET_TYPE=NLM
    PROGRAM_NAME = $(TARGET_NAME).$(TARGET_TYPE)

    $(PROGRAM_NAME):                \
                        chgvdate.obj\
                        chgvdate.den

%EndIf