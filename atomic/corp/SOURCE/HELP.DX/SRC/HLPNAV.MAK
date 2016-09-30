#
# $Header:   S:/help.dx/VCS/hlpnav.mav   1.0   06 Feb 1997 20:50:50   RFULLER  $
#
#   Makefile for NAV scanner help.
#
#*************************************************************************
# $Log:   S:/help.dx/VCS/hlpnav.mav  $
#  
#     Rev 1.0   06 Feb 1997 20:50:50   RFULLER
#  Initial revision
#  
#     Rev 1.9   29 Aug 1996 11:03:32   JBRENNA
#  Add %PROJVERS% when copying to NAME.TMP from the language area.
#  
#     Rev 1.8   29 Aug 1996 10:17:02   JBRENNA
#  Add cleanup to remove name.tmp after building the help file.
#  
#     Rev 1.7   29 Aug 1996 10:01:58   JBRENNA
#  Add logic for copying NAME.TMP from either NAV.IHS or NAVBOOT.IHS based on
#  the current TARGET. Remember that MAIN.HS includes NAME.TMP.
#  
#     Rev 1.6   10 Jul 1996 17:20:22   MZAREMB
#  Split dlgs.ihs and proc.ihs because they were too large for the help compiler,
#  so now parts are in dlgs2.ihs and proc2.ihs.
#  
#     Rev 1.5   06 Jun 1995 16:57:12   ROB
#  Removed included.ihs and added dlgs.ihs, glos.ihs, menu.ihs, and proc.ihs
#  
#  
#     Rev 1.4   27 Apr 1995 22:26:28   BARRY
#  Make NAVBOOT.HLP instead of NAV.HLP
#  
#     Rev 1.3   27 Apr 1994 14:46:32   DALLEE
#  Trunk conversion.
#*************************************************************************

HELP_FILE=%TARGET%.HLP
NAME_FILE=%TARGET%.IHS

TARGET_TYPE=HELP
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

all: $(HELP_FILE) cleanup

$(HELP_FILE): name.tmp main.hs dlgs.ihs dlgs2.ihs glos.ihs menu.ihs proc.ihs proc2.ihs 
 
name.tmp: AlwaysCopyName

# This target finds the $(NAME_FILE) and copies it to the current directory as
# NAME.TMP. Notice that MAIN.HS includes NAME.TMP. Thus, NAME.TMP determines the
# help PRODUCTNAME and OUTPUTFILE.
AlwaysCopyName:
    if exist name.tmp   del /q name.tmp
    iff exist $(NAME_FILE) then
        copy $(NAME_FILE) name.tmp
    elseiff exist %LANGPROJ%\SRC%PROJVERS%\$(NAME_FILE) then
        copy %LANGPROJ%\SRC%PROJVERS%\$(NAME_FILE) name.tmp
    else
        beep
        echo Could not find $(NAME_FILE).
    endiff

# This target removes the NAME.TMP file.
cleanup:
    if exist name.tmp   del /q name.tmp
