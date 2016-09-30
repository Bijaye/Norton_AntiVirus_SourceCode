#!/usr/bin/perl
####################################################################
#                                                                  #
# Program name:  AutoName.pm                                       #
#                                                                  #
# Package name:  AVIS::AutoName.pm                                 #
#                                                                  #
# Description:                                                     #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Morton Swimmer, Till Teichmann                    #
#                                                                  #
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

package AVIS::AutoName;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(GetRawNames CraftNewName);
@EXPORT_OK = @EXPORT;

use AVIS::Local;
use AVIS::Logger;
use File::Copy;
use Win32::Process;
use Win32;
use Config;

use strict;

1; # OK to load module

sub GetRawNames ($$){
   my ($file,$unc)=@_;
  
   my ($navrawname, $navexact) = ("", 0);
   my ($fprotrawname, $fprotexact) = ("", 0);
   my ($scanrawname, $scanexact) = ("", 0);
   my ($ProcessObj1,$ProcessObj2,$ProcessObj3);
  
   #
   # Log File
   #
   my $logfile = new AVIS::Logger($unc);

   my (@line,$i,$path);

   $file = Win32::GetShortPathName($file);
  
   $file =~ s|/|\\|g;
   @line=split(/\\/,$file);
   
   mkdir ("$isdnROOT/$isdnTempScan",1);
   if (copy ($file,"$isdnROOT/$isdnTempScan/$line[$#line]")!=1){
      print "Can't copy file\n";
   }
   
   $file="$isdnROOT\\$isdnTempScan\\$line[$#line]";

  
   #
   # Delete Old report files
   #
   unlink("$isdnROOT/$isdnBin/report.rep");
   unlink("$isdnScan1dir/report.rep");
   unlink("$isdnScan2dir/report.rep");
   unlink("$isdnScan3dir/report.rep");

   $path=$Config{bin};
   print "$path\n";
   #
   # Start Process for NAV-Scanner
   #
   $logfile->logString("Start NAV...");
   Win32::Process::Create($ProcessObj1,
      	"$path/perl.exe",
	      "perl $isdnROOT/$isdnBin/navname.pl $file",
	      0,
	      DETACHED_PROCESS,
	      ".");
   $ProcessObj1->Wait(5000);

   $logfile->logString("finished...");

   #
   # Start Process for F-prot-Scanner
   #
   $logfile->logString("Start F-prot..."); 
   Win32::Process::Create($ProcessObj2,
      	"$path/perl.exe",
	      "perl $isdnROOT/$isdnBin/fprot.pl $file",
	      0,
	      DETACHED_PROCESS,
	      ".");
   $ProcessObj2->Wait(5000);
   
   $logfile->logString("finished...");

   #
   # Start Process for Scan-Scanner
   #
   $logfile->logString("Start Scan..."); 
   Win32::Process::Create($ProcessObj3,
      	"$path/perl.exe",
	      "perl $isdnROOT/$isdnBin/scan.pl $file",
	      0,
	      DETACHED_PROCESS,
	      ".");
   $ProcessObj3->Wait(5000);

   $logfile->logString("finished...");
           
   
   #
   # Get NAV-Name
   #
   if (-e "$isdnScan3dir/Report.rep") {
      open OUT,("$isdnScan3dir/Report.rep");
      $navrawname=<OUT>;
      $navexact=1;   
      close (OUT);
   }
   
   #
   # Get F-Prot-Name
   #
   if (-e "$isdnScan1dir/Report.rep") {
      open(LOG1, "<$isdnScan1dir/report.rep");
      while (<LOG1>) {
         if (/Infection/) {
            s/\s\s/\s/g;
            @line=split(/\s/,$_);
	         $fprotrawname = $line[1];
	         $fprotexact = 1;
         }
      }
      close(LOG1);
   }

   # 
   # Get Scan-Name
   #
   if (-e "$isdnScan2dir/Report.rep") {
      open(LOG2, "<$isdnScan2dir/report.rep");
      while (<LOG2>) {
         if (/Infected/) {
            s/\s\s/\s/g;
            @line=split(/\s/,$_);
            $scanrawname = $line[6];
            if ($line[8] =~ /Removable/) {
               $scanrawname="$scanrawname\_$line[7]";
            }
            $scanexact = 1;
         }
      }
      close(LOG2);
   }

   unlink ($file);
   rmdir ($isdnTempScan);
   
   #
   # Delete Old report files
   #
   system ("attrib -r $isdnScan1dir/report.rep");
   system ("attrib -r $isdnScan2dir/report.rep");
   system ("attrib -r $isdnScan3dir/report.rep");
   unlink("$isdnROOT/$isdnBin/report.rep");
   unlink("$isdnScan1dir/report.rep");
   unlink("$isdnScan2dir/report.rep");
   unlink("$isdnScan3dir/report.rep");

   return {  "NAV"	=> { "raw" => $navrawname	, "exact" => $navexact    },
	   "FPROT"	=> { "raw" => $fprotrawname	, "exact" => $fprotexact },
	   "SCAN"	=> { "raw" => $scanrawname	, "exact" => $scanexact   }
	};
}
