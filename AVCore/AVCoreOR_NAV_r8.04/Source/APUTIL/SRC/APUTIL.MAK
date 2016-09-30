TARGET_NAME=APUTIL
DEF += /D_MBCS
TARGET_TYPE=LIB
PROGRAM_NAME=$(TARGET_NAME).$(TARGET_TYPE)
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(PROGRAM_NAME): \
 APQUAR.OBJ   \
 exclstuf.obj \
 excludel.obj \
 exclutil.obj \
 terminat.obj \
 exclmtch.obj \
 excluden.obj

#UPDATE#
#ENDUPDATE#
