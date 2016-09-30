
####################################################################
#                                                                  #
# Program name:  chngreg.pl                                        #
#                                                                  #
# Package name:                                                    #
#                                                                  #
# Description:                                                     #
#                                                                  #
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
# Change history:                                                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
#                                                                  #
#                                                                  #
#                                                                  #
####################################################################

use Win32::Registry;
use AVIS::Local;

use strict;

my ($p,$main,$McAfee);

$p = "SOFTWARE\\McAfee\\VirusScan";
$main::HKEY_LOCAL_MACHINE->Open($p, $McAfee) || system ("$isdnScan2dir\\mcafee.reg");





