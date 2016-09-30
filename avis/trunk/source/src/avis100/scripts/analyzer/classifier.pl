####################################################################
#                                                                  #
# Program name:  classifier.pl                                     #
#                                                                  #
# Description:   finds out which kind of file type the actual file #
#                is and scan the file with three scanners          #
#                it decide whether the file go through the         #
#                binary or macro replicator                        #
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
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
####################################################################

# requires:
# typemgr.pm   (LIB\SITE)
# Crumbs.pm    (LIB\SITE\AVIS)
# Local.pm     (LIB\SITE\AVIS)
# Autoname.pm  (LIB\SITE\AVIS)
# Logger.pm    (LIB\SITE\AVIS)
# ExtList.txt and EXEList.txt must be in the current directory

use Getopt::Long;
use DataFlow;
use Typemgr;
use AVIS::Crumbs;
use AVIS::Local;
use AVIS::AutoName;
use AVIS::Logger;

use strict;

my ($Cookie,$unc,$Parameter,$ProcessID,$outqueue );
my ($file,$type,$names,%crumbs,$logfile,@allfiles);

GetOptions("Cookie=s" => \$Cookie,
	  "ProcessId=i" => \$ProcessID,
	  "Parameter=s" => \$Parameter,
	  "UNC=s" => \$unc);

#
# Log File
#
$logfile = new AVIS::Logger($unc);

$logfile->logString("Starting: $0");
$logfile->logString("Cookie: $Cookie, ProcessID: $ProcessID, Parameter: $Parameter, UNC: $unc");

#
# Get sample file name
#
opendir DIR,("$unc/$isdnSample");
@allfiles= grep !/^\.\.$/, readdir DIR;
closedir DIR;
$file="$unc/$isdnSample/$allfiles[1]";

$logfile->logString("Using sample name: $file\n");

#
# determine the type of file this is.
#
$logfile->logString("Calling TypeMgr::FileType($file,)");
$type = TypeMgr::FileType($file,$logfile);
$logfile->logString("Returned from TypeMgr::FileType($file,)");

#
# Writing the results in the crumbs file
#
$crumbs{CaptureName}="$isdnSample/$allfiles[1]";
$crumbs{FileTypes}=$type;

#
# Scan the file with three scanners and get the names
#
$logfile->logString("Calling AVIS::AutoName::GetRawNames($file,$unc)");
$names=AVIS::AutoName::GetRawNames($file,$unc);
$logfile->logString("Returned from AVIS::AutoName::GetRawNames($file,$unc)");

if ($names->{"NAV"}->{"exact"}==1) {
   $crumbs{VirusName}=$names->{"NAV"}->{"raw"};
}
elsif ($names->{"FPROT"}->{"exact"}==1) {
   $crumbs{VirusName}=$names->{"FPROT"}->{"raw"};
}
elsif ($names->{"SCAN"}->{"exact"}==1) {
   $crumbs{VirusName}=$names->{"SCAN"}->{"raw"};
}
$crumbs{NameList}=$names;

$outqueue=$isqnDefer;

if ($type->{'NOFILE'}==1) {
   $outqueue=$isqnDefer;
}
if ($type->{'EMPTY'}==1) {
   $outqueue =$isqnDefer;
}

#
# split filename to rename the extention
#
my @parts;
@parts=split(/\./,$file);

#
# Decide what to do next
#
if ($type->{'PE'}==1) {
#  File can not be processed
   $outqueue=$isqnDefer;
}
elsif ($type->{'NE'}==1) {
#  File can not be processed
   $outqueue=$isqnDefer;
}
elsif ($type->{'EXE'}==1) {
   if ($isvalBinary==1) {
      if (rename $file,"$parts[0]\.EXE"==1){
         $file="$parts[0]\.EXE";
      }
      $outqueue=$isqnBrepctrl;
   }
   else {
      $outqueue=$isqnDefer;
   }
}
elsif ($type->{'COM'}==1) {
   if ($isvalBinary==1) {
      if (rename $file,"$parts[0]\.COM"==1){;
         $file="$parts[0]\.COM";
      }
      $outqueue=$isqnBrepctrl;
   }
   else {
      $outqueue=$isqnDefer;
   }
}
elsif ($type->{'BOO'}==1) {
   if ($isvalBinary==1) {
      $outqueue=$isqnBrepctrl;
   }
   else {
      $outqueue=$isqnDefer;
   }
   $outqueue=$isqnBrepctrl;
}
elsif ($type->{'OLE'}==1) {
   $outqueue=$isqnMrepctrl;
}
#elsif ($type->{'OLE'}==1 && $type->{'WORD'}==1 && $type->{'NOTEMPLATEBYTE'}==1) {
#  File contains no macros
#  $outqueue=$isqnReturn;
#}
elsif ($type->{'TEXT'}==1) {
   $outqueue=$isqnReturn;
}

$logfile->logString("Outqueue: $outqueue");

AVIS::Crumbs::write($unc,$Cookie,%crumbs);

$logfile->logString("Finishing: $0");

####################################################################

# Specify what stopping condition to use for multiple DoNext calls. In
# this case, we do not actual do more than one thing next, so we
# specify a one out of one stopping condition. (In fact, nothing else
# is supported in the scaffolding anyway.) $Cookie, $ProcessID and
# $unc MUST be passed through unmodified.
SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");

# Tell the dataflow what to do next, ie. which queue to send stuff
# to. We are free to set the $Parameter, but $Cookie, $ProcessID and
# $unc MUST be passed through (although we can use the values.)
DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
WriteSignatureForDataFlow ($ProcessID, $unc);

1;
