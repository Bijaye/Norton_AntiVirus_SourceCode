TARGET_NAME=THUNK
TARGET_TYPE=DLL

.INCLUDE $(QUAKEINC)\PLATFORM.MAK
.INCLUDE $(QUAKEINC)\ML.MAK
.INCLUDE $(QUAKEINC)\THUNK.MAK

%If ("$(PLATFORM)" == ".IRA" || "$(PLATFORM)" == ".IDA")

PROGRAM_NAME=TmHelp32.DLL
CLOPTS+= /DSYM_COMPILEDTHUNK
ASMOPTS+= /DIS_32

$(PROGRAM_NAME):  StdAfx.obj\
                  32to16.obj\
                  TmHelp32.obj\
				  TmHelp32.d32 

32to16.asm: timehelp.thk
#timehelp.obj: timehelp.asm

%ElseIf ("$(PLATFORM)" == ".WRA" || "$(PLATFORM)" == ".WDA")

PROGRAM_NAME=timehelp.DLL
ASMOPTS+= /DIS_16
LIBS = $(Q_SYMKRNL).LIB TOOLHELP.LIB
RC=R:\WIN32SDK\MSTOOLS\BINW16\RC
RCBINDOPTS=-T -40

$(PROGRAM_NAME):  timehelp.obj \
                  32to16.obj \
                  timehelp.def

%EndIf
