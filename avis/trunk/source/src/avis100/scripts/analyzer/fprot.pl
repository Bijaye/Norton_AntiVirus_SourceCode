#!/usr/bin/perl
####################################################################
#                                                                  #
# Program name:  fprot.pl                                          #
#                                                                  #
# Package name:                                                    #
#                                                                  #
# Description:                                                     #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Morton Swimmer, Till Teichmann                    #
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

use AVIS::Local;

use strict;

my ($file)=$ARGV[0];

$isdnScan1dir=~s/\//\\/;
unlink ("$isdnScan1dir\\report.rep");

system "$isdnScan1dir\\fprot.bat $file $isdnScan1dir";

