
This module is no longer used, and should be disposed of.  It was
incorporated into TypeMgr.PM, since we couldn't conceive of anyone
else having any use for it.


####################################################################
#                                                                  #
# Program name:  EXEList.pm                                        #
#                                                                  #
# Package name:  EXEList.pm                                        #
#                                                                  #
# Description:                                                     #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Till Teichmann                                    #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
#                                                                  #
#                                                                  #
####################################################################

# This module determines the types of the incoming files

package EXEList;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(@EXE);
@EXPORT_OK = @EXPORT;

@EXE=("ARJ self extracting,RJSX,0,100,-1,1,0",
"LZ self extracting,LZ09,0,100,-1,1,0",
"LZ self extracting,LZ91,0,100,-1,1,0",
"PKLITE,PKLITE,0,100,-1,1,0",
"LHARC self extracting,LHarc's SFX,0,100,-1,1,0",
"LHA self extracting,LHa's SFX,0,100,-1,1,0",
"LHA self extracting,LHA's SFX,0,100,-1,1,0",
"LHA self extracting,LH's SFX,0,100,-1,1,0",
"LARC self extracting,SFX by LARC,0,100,-1,1,0",
"NE,NE,0,100,-1,1,0",
"PE,PE,0,100,-1,1,0",
"FONT,FONTDIR,0,100,-1,1,0");

1;
