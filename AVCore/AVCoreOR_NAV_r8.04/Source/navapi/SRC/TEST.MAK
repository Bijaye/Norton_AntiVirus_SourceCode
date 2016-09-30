TARGET_TYPE=EXE
QUAKE_CUSTOM=
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK
OBJS=TEST.OBJ


# WIN16
# -----

%If ("$(CORE_WIN16)" != "")
    MODEL=L
    TARGET_NAME=TEST16

LIBS= \
    NAVAPI16.LIB


# WIN32
# -----

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "")
    TARGET_NAME=TEST32

LIBS= \
     NAVAPI32.LIB


# DX
# --

%ElseIf ("$[u,$(PLATFORM)]" == ".DX")  || \
        ("$[u,$(PLATFORM)]" == ".DVX") || \
        ("$[u,$(PLATFORM)]" == ".NCX")
    TARGET_NAME=TEST

LIBS= \
    NAVAPI.LIB \
    $(Q_SYMKRNL).LIB


# NLM
# ---

%ElseIf ("$[u,$(PLATFORM)]" == ".NLM")
    TARGET_NAME=TEST
    TARGET_TYPE=NLM

GIVEUP:
Echo I've given up on making this work.  Use the other makefile.


# VXD
# ---

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "V")
    TARGET_NAME=TEST

LIBS= \
    NAVAPI.LIB


# NTK
# ---

%ElseIf ("$(CORE_WIN32)" != "" && "$(CORE_SOFT)" == "K")
    TARGET_NAME=TEST

XOPTS += /Gs10000

LIBS= \
    NAVAPI.LIB


# UNSUPPORTED
# -----------

%Else
BAD_PLATFORM:
    beep
    Echo '$(PLATFORM)' is not yet supported.

%EndIf


# ALL PLATFORMS
# -------------

PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)
$(PROGRAM_NAME): $(OBJS) $(LIBS)


