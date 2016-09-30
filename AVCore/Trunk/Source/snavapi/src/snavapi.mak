
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

####################################################
#                     WIN32                        #
####################################################
%If ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")

    TARGET_NAME=SNAVAPI32
    CLOPTS += /MT
    LINKOPTS += /subsystem:console
    TARGET_TYPE=EXE
    PROGRAM_NAME = $(TARGET_NAME).$(TARGET_TYPE)
    CLOPTS += /GX

    $(PROGRAM_NAME):                \
                        snavapi.obj \
                        avshared.obj\
                        shared.obj  \
                        scanmem.obj \
                        scanmbr.obj \
                        scanboot.obj\
                        scanfile.obj\
                        apiver.obj  \
                        log.obj     \
                        md5.obj     \
                        fileops.obj \
                        spthread.obj\
                        NAVAPI32.LIB


####################################################
#                     NLM                          #
####################################################
%ElseIf ("$[u,$(PLATFORM)]" == ".NLM")

    TARGET_NAME=SNAVAPI
    TARGET_TYPE=NLM
    PROGRAM_NAME = $(TARGET_NAME).$(TARGET_TYPE)

    $(PROGRAM_NAME):                \
                        snavapi.obj \
                        avshared.obj\
                        spthread.obj\
                        scanfile.obj\
                        apiver.obj  \
                        shared.obj  \
                        log.obj     \
                        fileops.obj \
                        perf.obj    \
                        md5.obj     \
                        snavapi.den \
                        navapi.imp  \
                        navapi.exp  \

%EndIf
