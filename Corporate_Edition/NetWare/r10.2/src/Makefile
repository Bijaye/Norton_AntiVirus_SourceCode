# Copyright 2004-2005 Symantec Corporation. All rights reserved.
# Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
# Intent: Makefile for top level directory branching GNU Make on subprojects.

# The "../../Linux/src" has to be first as it contains a library that all the others
# use. "VPReg" has to be built next -- same reason.

Directories = ../../Linux/src\
				  VPReg\
				  NAV_Glue\
				  Rtvscan\
				  ../../../../Symantec_Client_Security/Linux_Security/src/regpop\
				  ../../../../Symantec_Client_Security/Linux_Security/src/regadd\
				  ../../../../Symantec_Client_Security/Linux_Security/src/savcli

# QsFwd VPRegEdt VPStart

include Rules.mk
