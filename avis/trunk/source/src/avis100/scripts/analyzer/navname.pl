#!/usr/bin/perl
####################################################################
#                                                                  #
# Program name:  navname.pl                                        #
#                                                                  #
# Package name:                                                    #
#                                                                  #
# Description:                                                     #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Raju Pavuluri                                     #
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


use navapi;
package navapi;

use AVIS::Local;

use strict;

my $file = shift;
my ($fileextention,$i,$lpdb,$HNavEngine,$NAV_VI_VIRUS_NAME,$n,$M,$exact);
my (@k,@l,@j,@m,@o,@p,@q,@x,@y,@z,@temp);

  
print "Filename: $file\n";
######Initialise the call back io functions
$i=navapi::CallBackInit();

#########Definition files

@k=NAVEngineInit($isdnScan3dir,$isdnScan3dir,$i,0,0);
print ("@k\n");
$HNavEngine=$k[0];
print ($HNavEngine);
@j=NAVGetVirusDBInfo($HNavEngine);
#@j=NAVGetNavapiVersionString(10);
print ("\n");
print ("@j\n");
$lpdb=$j[1];
bless $lpdb, "VIRUSDBINFOPtr";
print ($lpdb);
print ("\n");
$M=VIRUSDBINFO_dwVersion_get($lpdb);
print ($M);

####### deal with a virus


print ("\n");
print("scanning file\n");
@temp=split (/\./,$file);
#print "File extention: $temp[1]\n";
@m=NAVScanFile($HNavEngine,$file,$temp[1],1);
print ("printing results\n");
#print ("@m\n");
if ($m[0]==0) {
   @l=NAVGetVirusInfo($m[1],$NAV_VI_VIRUS_NAME,100);
   print("$l[1]\n");
   @o=NAVFreeVirusHandle($m[1]);
   @p=NAVGetVirusDefCount($HNavEngine);
}
#print("@p\n");
#print ("@o\n");
@x=NAVScanBoot($HNavEngine,"C");
#print("@x\n");
@y=NAVScanMemory($HNavEngine);
#print("@y\n");
$n=NAVEngineClose($HNavEngine);
#print ($n);
system("attrib -r $isdnScan3dir\\report.rep");
unlink("$isdnScan3dir\\Report.rep");
open(IN,">$isdnScan3dir\\Report.rep");
print IN $l[1];
close(IN) 

