#-------------------------------------------------------------------------
#                                                                       
# $Header:   S:/COMMON/VCS/NAVCONV.MAv   1.0   01 Mar 1996 18:26:50   BARRY  
#                                                                       
# Description:                                                          
#       NAVCONV makefile.
#                                                                       
#-------------------------------------------------------------------------
# $Log:   S:/COMMON/VCS/NAVCONV.MAv  
#  
#     Rev 1.0   01 Mar 1996 18:26:50   BARRY
#  Initial revision.
#  
#     Rev 1.0   30 Jan 1996 09:55:32   BARRY
#  Initial revision.
#-------------------------------------------------------------------------

TARGET_NAME=NAVCONV


OBJS= \
 nav3to4.obj

TARGET_TYPE=LIB
.INCLUDE $(QUAKEINC)\PLATFORM.MAK
PROGRAM_NAME=$(TARGET_NAME).LIB

$(PROGRAM_NAME): $(OBJS)


#UPDATE#
nav3to4.obj:	platform.h navconv.h oldnavop.h
#ENDUPDATE#
