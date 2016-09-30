####################################################################
#                                                                  #
# Program name:  pm/AVIS/Vides/Vides.pm                            #
#                                                                  #
# Package name:  AVIS::Vides                                       #
#                                                                  #
# Description:   Perl front end for VIDES 2                        #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1998-1999 IBM Corp.                           #
#                                                                  #
# Author:        Morton Swimmer, Stefan Tode                       #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:      Binary Replication                                #
#                                                                  #
# To do: define array index globals in 
####################################################################

package AVIS::Vides;
require Exporter;
use File::Copy;
use File::Compare;
use File::Basename;
use File::Find;
use DATA::Dumper ();
use AVIS::Local;
use AVIS::FSTools;
use AVIS::CreateScript;
use AVIS::CheckExtraction;
use AVIS::Log;
use AVIS::Logger;
use AVIS::VMTools;
use MD5;

@ISA = qw(Exporter);
@EXPORT = qw(ReplicateFile ReplicateBoot VidesSummary logV ReadSamplesFileMkII WriteSamplesFileMkII $iBoxPath $iPath $iGoatName $iAttributes $iDetermination $iModifiedZones $iExecZones $iRunNr);
@EXPORT_OK = @EXPORT;

use strict;

####################################################################

my $isdnVides  = "$isdnBinDir"; # MS - move this to AVIS::Local!!
my $emulator	= "$isdnBinDir/factory.exe";
my $isdnExtracted = "extracted";
my $isdnExtractBefore = "real-time"; 
my $isdnExtractPost = "afterwards";
my $floppya	 = "144.img";
my $floppyb	 = "120.img";
my $diskc	 = "10m.img";
my $isVCPU   = "I386";
my $isVCPUid = "NONE";
my $isOSvers = "PCDOS700EA";      # operation system version
my $isEMULOG = "bochs.log";
my $isfnVRLOG = "nul";
my ($iBoxPath, $iPath, $iGoatName, $iAttributes, $iDetermination, $iModifiedZones, $iExecZones, $iRunNr);

BEGIN {
# Constants for array access:
    $AVIS::Vides::iBoxPath = 0;
    $AVIS::Vides::iPath = 1;
    $AVIS::Vides::iGoatName = 2;
    $AVIS::Vides::iAttributes = 3;
    $AVIS::Vides::iDetermination = 4;
    $AVIS::Vides::iModifiedZones = 5;
    $AVIS::Vides::iExecZones = 6;
    $AVIS::Vides::iRunNr = 7;
}
$iBoxPath = 0;
$iPath = 1;
$iGoatName = 2;
$iAttributes = 3;
$iDetermination = 4;
$iModifiedZones = 5;
$iExecZones = 6;
$iRunNr = 7;

my $isfnAdf	= "$isdnVResources/rules/vides2.adf";
my $isfnMainRule= "$isdnVResources/rules/main"; # Do not put the .asa
                                                # extension here
$isfnAdf     =~ s|/|\\|g;                    # convert "/" to "\\"
$isfnMainRule=~ s|/|\\|g;                    # Asax needs "\\" instead of "/"

my $strategy	= 0;
my $verbosepath = 0;
my $verbosemsg = 1;
my (%filenamesfound);

1;

######################################################################
######################################################################
# File Replication #################################################

sub ReplicateFile (%) {
  my ($isParaRef) = @_;
  my ($extractdir, %set, $thisset, $cargs, $cmdline, $script, $thisextractdir, $chckextractdir, $imageset, $img_opts, $expertsys_opts, $misc_opts, $outputdir);
  my $vreplogger = $isParaRef->{VREPLOGGER};
       
  $isfnVRLOG = "$isParaRef->{ISDNVLOGFILE}"; # set global variable
  logV("="x78, 1);
  logV("\n", 1);
  $vreplogger->screenEcho(1);
  $vreplogger->logString("ReplicateFile: $isParaRef->{SAMPLEFILE}");
  $vreplogger->logString("File $isParaRef->{SAMPLEFILE} does not exist!"), return 0 unless -f $isParaRef->{SAMPLEFILE};
  ###$vreplogger->screenEcho(0);
  
  my $isVarTime = AVIS::FSTools::DateStr2Long($isParaRef->{RunDate}); # convert the date  
  
  # ========================================================================
  # now get the RunType from a string like this:
  # "144-120-10M-I386-NONE-MSDOS622EA"
  # ========================================================================
  $vreplogger->logString("getting Runtype information");
  ($floppya,$floppyb,$diskc,$isVCPU,$isVCPUid,$isOSvers) = getRunType($isParaRef->{RunType});

  # =======================================================================
  # clean the destination directory
  # =======================================================================
  $outputdir = $isParaRef->{isdnTempPath};
  $vreplogger->logString("Cleaning $outputdir");

  File::Path::rmtree($outputdir,$verbosepath,1);    # delete it, if it exists
  File::Path::mkpath($outputdir,$verbosepath,0777); # create it again
  
  # =======================================================================
  # create extraction directory
  # =======================================================================

  $extractdir = "$outputdir/$isdnExtracted";
  $vreplogger->logString("Cleaning $extractdir");

  File::Path::rmtree($extractdir,$verbosepath,1);   # delete it, if it exists
  File::Path::mkpath($extractdir,$verbosepath,0777);

  %set = (
	  $floppya => "$outputdir/a.img",
	  $floppyb => "$outputdir/b.img",
	  $diskc   => "$outputdir/c.img"
	 );
  
  $imageset = "$isdnVImages/$isOSvers";
  $thisset = $isOSvers;
  $vreplogger->logString("Using imageset: $imageset\nimagesdir: $isdnVImages");
  $vreplogger->logString("Copying disk images to $outputdir...");
  
  copy("$imageset/$floppya", $set{$floppya}) 
    or $vreplogger->logString("copy failed: $imageset/$floppya to $set{$floppya} $!\n"), return 0;
  copy("$imageset/$floppyb", $set{$floppyb}) 
    or $vreplogger->logString("copy failed: $!\n"), return 0;
  copy("$imageset/$diskc", $set{$diskc})   
    or $vreplogger->logString("copy failed: $!\n"), return 0;
  
  # write infected sample to disk
  
  $vreplogger->logString("Copying sample onto drive A:");
  
  # setup the drive configuration file

  my $drive_options = " partition=1";
  my $copy_options = "-X -a";    #"-/ -a";
  my $msdos = new AVIS::VMTools("");  # create a  new object
    
  $msdos->localFile("$outputdir/mtools.cfg"); # setup configuration file

  # add the following drives to the configuration file

  $msdos->VAddDrive("A",$set{$floppya},"");
  $msdos->VAddDrive("B",$set{$floppyb},"");
  $msdos->VAddDrive("C",$set{$diskc},$drive_options);

  # =====================
  # write the sample file
  # =====================

  $msdos->Vwrite($isParaRef->{SAMPLEFILE},"C:\\SAMPLE.COM","");

  ###AVIS::Mtools::Vwrite("$set{$floppya}","K",$isParaRef->{SAMPLEFILE},"sample.com");

  # =======================================================================
  # now get the RunStrategy from a string like this:
  # "FILE-DEFAULT-TSROPENCLOSE"
  # =======================================================================

  $vreplogger->logString("getting RunStrategy information");
  my $isvnSecTimeout=180;
  my ($RunStrategyMode,$RunStrategyType,$RunStrategyFunction) 
    = getRunStrategy($isParaRef->{RunStrategy});
  if ($RunStrategyType =~ /DEFAULT/) {
    $isvnSecTimeout=480; #180;  # Timeout to 480 seconds
  } elsif ($RunStrategyType =~ /MASSREP/) {
    $isvnSecTimeout=480;
  }

  # =======================================================================
  # Create command script for the emulator
  # =======================================================================
    
  $vreplogger->logString("Creating script");

  $script = "$outputdir/script.$thisset";
  #CreateScript($imageset . "\\" . $floppya . ".description", $script, "sample");
  copy("$isdnVImages/$isOSvers/REPLICAT.BAT", $script);

  $vreplogger->logString("Trying to replicate sample $isParaRef->{SAMPLEFILE}");

  # =======================================================================
  # Create real-time extraction directory
  # =======================================================================
    
  $thisextractdir = "$extractdir/$thisset.$isdnExtractBefore";
  $chckextractdir = "$extractdir/$thisset.$isdnExtractPost";
  File::Path::mkpath([$thisextractdir, $chckextractdir], $verbosepath, 0777);

  # =======================================================================
  # Generate the commandline switches for the emulator
  # =======================================================================

  my $isvarUsePBS = 1;                         # use -usePBS commandline switch
  my $isdnPBS     = "$isdnVGoats/$isOSvers";   # directory where the PBS objects are stored
  my $emurun = 0;
  $img_opts	     = "-imgA=$set{$floppya} -imgB=$set{$floppyb} -imgC=$set{$diskc}";
  $expertsys_opts    = "-module=$isfnMainRule -descrfile=$isfnAdf";
  $expertsys_opts   .= " -extract=$thisextractdir -cmdlines=$script";
  $misc_opts	     = "-sectimeout=$isvnSecTimeout -bootA -log $outputdir/$isfnVEmuLog";
  $misc_opts	    .= " -time=$isVarTime"  if($isVarTime > 0);
  $expertsys_opts   .= " -usePBS=A,$isdnPBS/fddboota.img;B,$isdnPBS/fddbootb.img;C,$isdnPBS/Hddboot.img" if ($isvarUsePBS);

  # build the commandline
  $cmdline = "$emulator $img_opts $expertsys_opts $misc_opts > $outputdir\\emulator.log$emurun";

  $vreplogger->logString("[$cmdline]");

  # =======================================================================
  # Execute the Emulator
  # =======================================================================
    
  system $cmdline;

  # Trial code to restart the emulator if key sector infections were found

  if (KeySectorInfectionFound("$outputdir\\emulator.log$emurun")) {
    # rebuild the options. Do we need new goats? we probably need a new
    # log file.
    logV( "="x78, 1);
    logV( "\nRestarting emulation\n", 1);
    copy("$outputdir/$isfnVEmuLog", "$outputdir/$isfnVEmuLog$emurun");
    $emurun++;
    $img_opts        = "-imgA=$set{$floppya}";
    $img_opts       .= " -imgB=$set{$floppyb}";
    $img_opts       .= " -imgC=$set{$diskc}";
    $expertsys_opts  = "-module=$isfnMainRule -descrfile=$isfnAdf";
    $expertsys_opts .= " -extract=$thisextractdir -cmdlines=$script";
    $misc_opts	     = "-sectimeout=$isvnSecTimeout";
    $misc_opts	    .= " -bootC"; # boot from C this time
    $misc_opts	    .= " -log $outputdir/$isfnVEmuLog";
    $misc_opts      .= " -time=$isVarTime"  if($isVarTime > 0);
    $expertsys_opts .= " -usePBS=A,$isdnPBS/fddboota.img;B,$isdnPBS/fddbootb.img;C,$isdnPBS/Hddboot.img" if ($isvarUsePBS);
    
    # build the commandline
    $cmdline = "$emulator $img_opts $expertsys_opts $misc_opts > $outputdir\\emulator.log$emurun";
    
    $vreplogger->logString("[$cmdline]");

    system $cmdline;
  }

  # =======================================================================
  # Create the afterwards extraction directory
  # =======================================================================

  File::Path::mkpath(["$chckextractdir/A","$chckextractdir/C"],$verbosepath,0777);
  #File::Path::mkpath("$chckextractdir\\B",$verbosepath,0777);

  # =================================================================
  # copy recursive all files from the image C beginning from the root
  # =================================================================
    
  # ST: there is a problem with the recursive copy, so I used a
  # workaround with Vxread
    
  # ST: don't use "-b" switch

  $msdos->Vxread("A:\\",$chckextractdir,$copy_options); 
  $msdos->Vxread("B:\\",$chckextractdir,$copy_options);
  $msdos->Vxread("C:\\",$chckextractdir,$copy_options);

  $vreplogger->logString("Checking real-time extraction");

  if (SameContent($chckextractdir, $thisextractdir)) {
    $vreplogger->logString("real-time extraction succeeded");
  } else {
    $vreplogger->logString("REAL-TIME EXTRACTION FAILED!");
  }

  unless (open (VLOG, ">$outputdir/$isfnVidesLog")) {
    logVW("Can't open $isfnVidesLog!\n", $verbosemsg); 
    return 0;
  };

  copy("$outputdir/emulator.log", \*VLOG);
  copy("$outputdir/emulator.log0", \*VLOG);
  copy("$outputdir/emulator.log1", \*VLOG);
  copy("$outputdir/emulator.log2", \*VLOG);
  close VLOG;
           
  $isParaRef->{ReturnExitMsg} = "bochs: Exit OK";
  $vreplogger->logString("returning from replicatfile");
  return 1;
}

######################################################################
######################################################################

sub ReplicateBoot (%) {
  my ($isParaRef) = @_;
  my ($extractdir, %set, $imageset, $thisset, $thisextractdir, $chckextractdir, $img_opts, $expertsys_opts, $misc_opts, $fixed_opts, $S_writes, $C_infection, $A_infection, $B_infection, $outputdir, @samples, $iPassNumber);
  
  Verror("Begin Boot Replicate\n","ok");    
  
  my $vreplogger = $isParaRef->{VREPLOGGER};
  $isfnVRLOG = "$isParaRef->{ISDNVLOGFILE}"; # set global variable
  logV("\t***\tRunning ReplicateBoot pass: $iPassNumber\t***\n",1);
  $vreplogger->logString("Running ReplicateBoot pass: $iPassNumber");
  logVW("File $isParaRef->{SAMPLEFILE} does not exist!", $verbosemsg), return 0 unless -f $isParaRef->{SAMPLEFILE};
#  logVW("Cannot handle more than one sector!\n", $verbosemsg), return 0 unless -s $isParaRef->{SAMPLEFILE} == 512;

  my $isVarTime = AVIS::FSTools::DateStr2Long($isParaRef->{RunDate}); # convert the date  
  
  # ========================================================================
  # now get the RunType from a string like this:
  # "144-120-10M-I386-NONE-MSDOS622EA"
  # ========================================================================
  logV("getting Runtype information for BootReplication\n");
  $vreplogger->logString("getting Runtype information for BootReplication");
  ($floppya,$floppyb,$diskc,$isVCPU,$isVCPUid,$isOSvers) = getRunType($isParaRef->{RunType});

  # ========================================================================
  # clean the destination directory
  # ========================================================================
  $outputdir = $isParaRef->{isdnTempPath};
  logV("Cleaning $outputdir\n",1);
  
  File::Path::rmtree($outputdir,$verbosepath,1);    # delete it, if it exists 
  File::Path::mkpath($outputdir,$verbosepath,0777); # create it again

  # ========================================================================
  # create extraction directory
  # ========================================================================

  $extractdir = "$outputdir/$isdnExtracted";

  File::Path::rmtree($extractdir,$verbosepath,1);  # delete it, if it exists
  File::Path::mkpath($extractdir,$verbosepath,0777);

  %set = (
	  $floppya => "$outputdir/a.img",
	  $floppyb => "$outputdir/b.img",
	  $diskc   => "$outputdir/c.img"
	 );
  # -------------------------------------- 
  logV("isdnVImages: $isdnVImages\n");
  $imageset = "$isdnVImages/$isOSvers";
  logV("imageset: $imageset\nimagesdir: $isdnVImages\n");
  $vreplogger->logString("imageset: $imageset\nimagesdir: $isdnVImages");
  $thisset = $isOSvers;

  # ---------------------------------------------

  logV("Using image set $imageset\n");
  logV("Copying disk images to $outputdir...\n");

  copy("$imageset/$floppya", $set{$floppya}) 
    or logVW("copy failed: $!\n", $verbosemsg), return 0;
  copy("$imageset/$floppyb", $set{$floppyb}) 
    or logVW("copy failed: $!\n", $verbosemsg), return 0;
  copy("$imageset/$diskc"  , $set{$diskc})   
    or logVW("copy failed: $!\n", $verbosemsg), return 0;
  
  # ======================================================================
  # now install virus on disk, default is floppya
  # ======================================================================
  PrepareBootSectorVirusImg($set{$floppya}, $isParaRef->{SAMPLEFILE}) 
    or logVW("Can't prepare boot image!\n", $verbosemsg), return 0;
  
  #    print "Trying to replicate sample $isParaRef->{SAMPLEFILE}\n";

  $thisextractdir = "$extractdir/$thisset.$isdnExtractBefore";
  $chckextractdir = "$extractdir/$thisset.$isdnExtractPost";
  File::Path::mkpath([$thisextractdir, $chckextractdir],$verbosepath,0777);

  # ======================================================================
  # now get the RunStrategy from a string like this: "BOOT-DEFAULT-ACAMANY"
  # ======================================================================
    
  $vreplogger->logString("getting RunStrategy information");
  my $isvnSecTimeout=180;
  my ($RunStrategyMode,$RunStrategyType,$RunStrategyFunction) = getRunStrategy($isParaRef->{RunStrategy});
  if ($RunStrategyType =~ /DEFAULT/) {
    $isvnSecTimeout=180;
  } elsif ($RunStrategyType =~ /MASSREP/) {
    $isvnSecTimeout=500;
  }

  # ======================================================================
  # Generate the commandline switches for the emulator
  # ======================================================================
    
  # define general options

  my $isvarUsePBS = 1;                     # use -usePBS commandline switch
  my $isdnPBS = "$isdnVGoats/$isOSvers";   # directory where the PBS objects are stored

  $img_opts = "-imgA=$set{$floppya} -imgB=$set{$floppyb} -imgC=$set{$diskc}";
  $expertsys_opts  = "-module=$isfnMainRule -descrfile=$isfnAdf";
  $expertsys_opts .= " -extract=$thisextractdir";
  if ($isvarUsePBS) {
    $expertsys_opts .= " -usePBS=A,$isdnPBS/fddboota.img;B,$isdnPBS/fddbootb.img;C,$isdnPBS/Hddboot.img";
  }
  $misc_opts = "-log $outputdir/$isfnVEmuLog";
  # if we have a RunTime use it, else use the current Time
  if($isVarTime > 0) { 
    $misc_opts .= " -time=$isVarTime";
  }
    
  $fixed_opts = "$img_opts $expertsys_opts $misc_opts";

  # ======================================================================
  # Create Stopscript to terminate emulator
  # ======================================================================

  open (STOPSCRIPT, ">stopscript") 
    or logVW("Can't create script file", $verbosemsg), return 0;
  print STOPSCRIPT "stop"; # changed ST "s"; (For this we need a
                           # program called STOP.COM on the disk)
  close (STOPSCRIPT);

  # Booting from A: trying to infect C:
  
  my $cmdline = "$emulator $fixed_opts -bootA -cmdlines=stopscript -sectimeout=10 > $outputdir\\emulator.log1";
  logV("[$cmdline]", 1);
  $vreplogger->logString("[$cmdline]");
  system $cmdline;
  
  $isParaRef->{ReturnExitMsg1} = GetEmuExitCode("$isParaRef->{ISDNWORKDIR}/$isfnVEmuLog");
  
  # moved ST
  # Replace A:
  # unlink $set{$floppya};
  # copy $imageset . "/" . $floppya, $set{$floppya} or logVW("copy failed: $!\n", $verbosemsg), return 0;
  
  $C_infection = 0;
  open (ELOG, "$outputdir/emulator.log1") 
    or logVW("Can't open emulator log!\n", $verbosemsg), return 0;
  while (<ELOG>) {
    # find something like:
    # *** BIOS Write (80,01,0000,01,01 ) HDD Boot Sector - Buffer at 9000:0000
    #$S_writes++      if /BIOS Write \(.+ \).+ Buffer at /;
    #$C_infection = 1 if /BIOS Write \(.+ \) HDD Boot Sector - Buffer at /;
    #$C_infection = 1 if /BIOS Write \(.+ \) HDD MBR - Buffer at /;      
    #$C_infection = 1 if /BIOS Write \(.+\)HDD Boot Sector - Buffer at /; # old
    #$C_infection = 1 if /BIOS Write \(.+\)HDD MBR - Buffer at /;         # old
    #$C_infection = 1 if /Write HDD Boot Sector/;                         # old
    #$C_infection = 1 if /Write HDD MBR/;                                 # old
    
    $S_writes++      if /BIOS Write \(.+\).+Buffer at /;
    $C_infection = 1 if /Write.+HDD Boot Sector/;      # old, but shorter
    $C_infection = 1 if /Write.+HDD MBR/;              # old, but shorter
    if (/EXTRACTING SECTOR(.+)$/) { 
      my $tmpname = $1; 
      $tmpname =~ s/.*[\\\/]//;  # remove path name, get the filename
      $tmpname =~ s/.*[\.]//g;    # remove anything before the last point
      $iPassNumber = $1 if (m/([0-9]+)_([0-9]+)/);  # get the major passnumber
    }
  }
  close (ELOG);
    
  unless ($C_infection) {
    logV( "REPLICATION FAILED - NO HARD DISK INFECTION\n", 1);
  } else {
    # Booting from C: trying to infect floppy drives in A:
    
    # Replace A:
    unlink $set{$floppya};
    copy("$imageset/$floppya", $set{$floppya}) or logVW("copy failed: $!\n", $verbosemsg), return 0;
    
    # increment the iPassNumber, this is the major number in the
    # extension of the extracted files.  iPassNumber should at least
    # be 1.
    $iPassNumber++;
    logV("\t***\tRunning ReplicateBoot pass: $iPassNumber\t***\n");
    $vreplogger->logString("\t***\tRunning ReplicateBoot pass: $iPassNumber\t***");
    # should there be a different time ?
    $cmdline = "$emulator $fixed_opts -pass=$iPassNumber -bootC -boottriage=$imageset/$floppya -sectimeout=$isvnSecTimeout >> $outputdir\\emulator.log2";
    logV("[$cmdline]");
    $vreplogger->logString("[$cmdline]");
    system $cmdline;
    
    $isParaRef->{ReturnExitMsg2} = GetEmuExitCode("$isParaRef->{ISDNWORKDIR}/$isfnVEmuLog");

    $A_infection = 0;
    $B_infection = 0;
    open (ELOG, "$outputdir/emulator.log2") or logVW("Can't open emulator log!\n", $verbosemsg), return 0;
    while (<ELOG>) {
      # *** BIOS Write (00,00,0000,01,01 ) FDD Boot Sector - Buffer at 9F80:0200
      # i.e. Anticmos
      #$A_infection = 1 if /BIOS Write \(.+ \) FDD Boot Sector - Buffer at /;

      #$A_infection = 1 if /Write FDD Boot Sector/;  # old
      $A_infection++ if /Write.+FDA Boot Sector/;  # shorter
      $B_infection++ if /Write.+FDB Boot Sector/;  # shorter
    }
    close (ELOG);
    
    if ($A_infection) {
      @samples = glob("$thisextractdir\\*.*");
      logV("Successful replication - " . scalar @samples . " samples\n");
      $vreplogger->logString("Successful replication - " . scalar @samples . " samples");
      ### $isParaRef->{CountSamples} = ($#samples+1);      
    } else {
      logV("REPLICATION FAILED\n");
      $vreplogger->logString("REPLICATION FAILED");
      ### $isParaRef->{CountSamples} = 0;
    }
  } # end if infection on C:
  # now merge all emulator logfiles into one big file
  open (VLOG, ">$outputdir/$isfnVidesLog") or logVW("Can't open $isfnVidesLog!\n", $verbosemsg), return 0;

  copy("$outputdir/emulator.log", \*VLOG) or logVW("Can't open $outputdir/emulator.log!\n");
  copy("$outputdir/emulator.log1", \*VLOG) or logVW("Can't open $outputdir/emulator.log1!\n");
  copy("$outputdir/emulator.log2", \*VLOG)  or logVW("Can't open $outputdir/emulator.log1!\n");
  close VLOG;

  my $tmpstr = sprintf("S_writes: %04d, C_infections: %02d, A_infections: %02d, B_infections: %02d\n",$S_writes,$C_infection,$A_infection,$B_infection);
  logV($tmpstr);
  $vreplogger->logString("$tmpstr");
  # Now, delete the disk images, because they take a lot of space
  ###unlink (values %set) or logVW("Can't delete disk images: $!\n", $verbosemsg);
 
  return 1;
}

######################################################################
######################################################################

sub PrepareBootSectorVirusImg ($$) {
  my ($dest_img, $virus_img) = @_;
  my $rc;

  return 0 unless -f $virus_img;
  return 0 if -s $virus_img < 512;

  if (-s $virus_img == 512) {
    # Only one sector
    logV( "Patching boot virus sample onto drive A: DBS\n", 1);
    $rc = system "$isfnPatchSec $dest_img $virus_img 0 0 1 >nul";
    return 0 unless $rc == 0;
  } elsif (-s $virus_img > 300*1024) { # more than 300K
    # Full image (!)
    logV( "Copying infected image to use it as A:\n", 1);
    copy($virus_img, $dest_img) or return 0;
  } else {
    return PrepareImg_MultiSec($dest_img, $virus_img);
  }
}

######################################################################
######################################################################

sub PrepareImg_MultiSec ($$) {
  my ($dest_img, $virus_img) = @_;

  if ((-s $virus_img % 512) == 0) {
    # Multiple contiguous sectors
    logV( "Patching boot virus sample onto drive A: DBS and following sector(s)\n", 1);
    my $rc = system "$isfnPatchSec -multi $dest_img $virus_img 0 0 1 >nul";
    return !$rc;
  } else {
    return 0; # not implemented
  }
}

######################################################################
######################################################################
# VidesSummary
# analyzes logfile of replication process
# input: \%ReplicationParameter
# output: number of generated samples
#
# analyzes logfile
# call StaticAnalyzer
# writes samples.lst
#

sub VidesSummary(%) {
  my ($isParaRef) = @_;

  my ($debug) = 1;
  my ($isvarWipeTmp) = 1;  # 1 if the worktemp should be cleaned after run 
  #my ($isvarWipeTmp) = 0;  # 1 if the worktemp should be cleaned after run 

  my ($isdnDstType, %cmpParam, @SampleTmp, @Sample);

  ###########################################################################  
  # get RunType
  ###########################################################################

  my ($floppya,$floppyb,$diskc,$isVCPU,$isVCPUid,$isOSvers) 
    = getRunType($isParaRef->{RunType});
  my $isdnOSGoats = $isOSvers;
  my $vreplogger  = $isParaRef->{VREPLOGGER};
  
  logV("VidesSummary: Analyzing logfile $isParaRef->{ISDNWORKDIR}/$isfnVidesLog\n",1);
  logV("VidesSummary: *** current \$isParaRef->{ISDNWORKDIR} $isParaRef->{ISDNWORKDIR} \$isdnRun $isdnRun\n",1);

  copy("$isParaRef->{ISDNWORKDIR}/$isfnVidesLog", "$isParaRef->{ISDNUNC}/$isdnRun/$isParaRef->{Parameter}/$isfnVidesLog") 
    or logVW("Can't copy $isfnVidesLog\n", $verbosemsg);
  copy("$isParaRef->{ISDNWORKDIR}/$isfnVEmuLog", "$isParaRef->{ISDNUNC}/$isdnRun/$isParaRef->{Parameter}/$isfnVEmuLog") 
    or logVW("Can't copy $isfnVEmuLog\n",$verbosemsg);

  my (@FileNameArray, @FileInfType, @TriggerArray, @FileSector_array, @FileExtractSrc, @FileExtractDst, @SectorInfT_array, $C_infectionBS, $C_infectionMB, $A_infectionBS, $B_infectionBS);

  ParseVidesLog("$isParaRef->{ISDNWORKDIR}/$isfnVidesLog", 
		\@FileNameArray, 
		\@FileInfType, 
		\@TriggerArray, 
		\@FileSector_array, 
		\@FileExtractSrc, 
		\@FileExtractDst, 
		\@SectorInfT_array, 
		\$C_infectionBS, 
		\$C_infectionMB, 
		\$A_infectionBS, 
		\$B_infectionBS);

  # ==========================================
  # now check the files in the extracted dir
  # ==========================================

  my %FileDiffGoat = CheckExtractedFiles($isParaRef, $isdnOSGoats);

  ###########################################################################
  # Process the file infections
  ###########################################################################
  
  logV("VidesSummary: Process the file infections\n", 1);
  logV("VidesSummary: FileNameArray length: $#FileNameArray\n",1);
  my ($i, $SourceName, $NewName);
  for ($i=0; $i <= $#FileNameArray; $i++) {
    logV("VidesSummary: \$FileExtractSrc[$i]: '$FileExtractSrc[$i]', \$FileNameArray[$i]: '$FileNameArray[$i]', \$FileExtractDst[$i]: '$FileExtractDst[$i]'\n", 1);
    # Now find the SourceName and NewName
    if ($FileExtractSrc[$i] ne "") {
      $SourceName = $FileExtractSrc[$i];
    } elsif ($FileNameArray[$i] ne "") {
      $SourceName = $FileNameArray[$i];
    } else {
      $SourceName = "none";
    }
    if ($FileExtractDst[$i] ne "") {
      $NewName    = $FileExtractDst[$i];
    } elsif ($FileNameArray[$i] ne "") {
      $NewName    = "none"; #$FileNameArray[$i];
    } else {
      $NewName    = "none";
    }
    my $VinfMethod = GetVinfectionMethod($FileInfType[$i]);
    logV("VidesSummary: \$SourceName: $SourceName, \$NewName: $NewName, \$VinfMethod: $VinfMethod\n",1);
    logV("VidesSummary: Vgoat:      ".GetVgoat($SourceName,$isdnOSGoats,$VinfMethod)." = GetVgoat($SourceName, $isdnOSGoats, $VinfMethod)\n", 1);
    logV("VidesSummary: adding to \@SampleTmp: [$SourceName, $NewName, ".GetVgoat($SourceName,$isdnOSGoats,$VinfMethod).", ".GetVAttr($SourceName,$VinfMethod).", ES1-none, ".GetVMzones($SourceName).", ".GetVEzones($SourceName).", ".$isParaRef->{Parameter}."]\n", 1);
    push @SampleTmp, [$SourceName, #BoxPath
		      $NewName,    #Path
		      GetVgoat($SourceName,$isdnOSGoats,$VinfMethod), #GoateName
		      GetVAttr($SourceName,$VinfMethod), #Attributes
		      "ES1-none",               #Determination
		      GetVMzones($SourceName),  #ModifiedZones
		      GetVEzones($SourceName),  #ExecZones
		      $isParaRef->{Parameter}]; #RunNr

    $cmpParam{SampleBoxPath} = $SourceName; # filename in image
    $cmpParam{SamplePath}    = $NewName;    # filename outside (modify it)
    $cmpParam{SampleGoatName}= "";
    if ($SourceName =~ /\.COM/i || $SourceName =~ /\.EXE/i) {
      $isdnDstType = "$isdnInfected";
    } else {
      $isdnDstType = "$isdnSectors";
    }
    $cmpParam{Destination} = "$isParaRef->{ISDNUNC}/$isdnRun/$isParaRef->{Parameter}/$isdnDstType";
    $cmpParam{Destination} =~ s/\\/\//g; # just to make sure that 
                                         # we are forward-slashified
 
    $cmpParam{DiffList} = \%FileDiffGoat;
    logV("VidesSummary: doing staticCheck\n",1);
    my $scStatus = staticCheck(\%cmpParam);
    logV("VidesSummary: Result 1 of staticCheck: $scStatus ... '$cmpParam{DiffList}->{$NewName}' '$SourceName'\n", 1);
    if ($scStatus == 1) {
      $scStatus=0; #???
      my $shortSamplePath = RemoveUNC($cmpParam{SamplePath}, $isParaRef->{ISDNUNC});    # filename outside
      my $goatname = MorphGoatName($cmpParam{SampleGoatName}, $isdnExtractBefore, $isdnExtractPost, $isdnOSGoats);

      logV("VidesSummary: Adding \@Sample: [$cmpParam{SampleBoxPath}, $shortSamplePath, $goatname, ".GetAttributes(calcVirusSize($cmpParam{SamplePath}, "$isdnVGoats/$goatname]"), $goatname).", ES1-SA1, ".GetVMzones($SourceName).", ".GetVEzones($SourceName).", $isParaRef->{Parameter}]", 1);

      push @Sample, [$cmpParam{SampleBoxPath}, 
		     $shortSamplePath,
		     $goatname,
		     GetAttributes(calcVirusSize($cmpParam{SamplePath},
						 "$isdnVGoats/$goatname]"), ### MS this looks wrong
				   $goatname),
		     "ES1-SA1",
		     GetVMzones($SourceName),
		     GetVEzones($SourceName),
		     $isParaRef->{Parameter}
		    ];
      $vreplogger->logString("original infected object path: $cmpParam{SampleBoxPath}");
      $vreplogger->logString("path of extracted object     : $shortSamplePath");
      $vreplogger->logString("goat name                    : $goatname");
    }
  }

#  return 0;
  # =========================================================================
  # Now process all the remaining files, which haven't been detected
  # by the expert system
  # =========================================================================
  my $keyfile;
  foreach $keyfile (sort keys %FileDiffGoat) { 
    if ($FileDiffGoat{$keyfile} == 0) {
      $SourceName = $keyfile;
      if ($SourceName =~ /\.COM/i || $SourceName =~ /\.EXE/i) {
	$isdnDstType = $isdnInfected;
      } else {
	$isdnDstType = $isdnSectors;
      }
      #$cmpParam{Destination} = "$isParaRef->{ISDNUNC}/$isdnRun/$isParaRef->{Parameter}/$isdnDstType";
      my $Destination = "$isParaRef->{ISDNUNC}/$isdnRun/$isParaRef->{Parameter}/$isdnDstType";
      logV("VidesSummary: DiffToGoat: $keyfile, $FileDiffGoat{$keyfile}, $Destination\n",1);
      my ($rv, $newName) = changeName($Destination, $keyfile);
      if ($rv == 1) {
	logV("VidesSummary: StaticAnalyzer: $newName\n",1);
	my $shortSamplePath = RemoveUNC($newName, $isParaRef->{ISDNUNC});
	my $shortSampleInBoxPath = $keyfile;          # original goat file
	$shortSampleInBoxPath =~ s/(.+\.$isdnExtractPost[\\\/])([A-Za-z])([\\\/].+)/$2:$3/i; # original goat file
	#if ($cmpParam{SampleGoatName} eq "") {
	#  $cmpParam{SampleGoatName} = $keyfile;
	#}
	my $SampleGoatName = $keyfile;
	#my $goatname = MorphGoatName($cmpParam{SampleGoatName}, $isdnExtractBefore, $isdnExtractPost, $isdnOSGoats);
	my $goatname = MorphGoatName($SampleGoatName, $isdnExtractBefore, $isdnExtractPost, $isdnOSGoats);

	logV("VidesSummary: adding to \@Sample: [$shortSampleInBoxPath, $shortSamplePath, $goatname, ".GetAttributes(calcVirusSize($newName,"$isdnVGoats/$goatname"),$goatname).", ES0-SA1, NONE, NONE, $isParaRef->{Parameter}]\n", 1);

	push @Sample, [$shortSampleInBoxPath,
		       $shortSamplePath,
		       $goatname,
		       GetAttributes(calcVirusSize($newName,
						   "$isdnVGoats/$goatname"), 
				     $goatname),
		       "ES0-SA1",
		       "NONE",
		       "NONE",
		       $isParaRef->{Parameter}
		      ];

	$vreplogger->logString("original infected object path: $shortSampleInBoxPath");
	$vreplogger->logString("path of extracted object     : $shortSamplePath");
	$vreplogger->logString("goat name                    : $goatname");
	$rv = 0;
	logV("VidesSummary: MOVED $keyfile $newName\n",1);
      }
    }
  }
#return 0;
  
  ###########################################################################
  # 
  ###########################################################################
  
  my $WrittenSystemSectors = $C_infectionBS + $C_infectionMB + $A_infectionBS + $B_infectionBS;
  $isParaRef->{DeBuGSECTORSamples} = ($#FileSector_array +1) . " - $WrittenSystemSectors" if ($FileSector_array[0]);
  if ($C_infectionBS) { 
    $vreplogger->logString(sprintf("%02d possible bootsector infection(s) on hard drive C\n",$C_infectionBS));
  }
  if ($C_infectionMB) { 
    $vreplogger->logString(sprintf("%02d possible MBR        infection(s) on hard drive C\n",$C_infectionMB));
  }
  if ($A_infectionBS) { 
    $vreplogger->logString(sprintf("%02d possible bootsector infection(s) on disk drive A\n",$A_infectionBS));
  }
  if ($B_infectionBS) { 
    $vreplogger->logString(sprintf("%02d possible bootsector infection(s) on disk drive B\n",$B_infectionBS));
  }

  ###########################################################################
  # Process the Sector infections
  ###########################################################################
 
  logV("VidesSummary: FileSector_array length: $#FileSector_array\n",1);
  my $j;
  for($j=0; $j <= $#FileSector_array; $j++) {
    logV("VidesSummary: $j sector written: $FileSector_array[$j] type $SectorInfT_array[$j]\n");
    my $NewName    = $FileSector_array[$j];
    my $VinfMethod = GetVinfectionMethod($SectorInfT_array[$j]);
    push @SampleTmp, [File::Basename::basename($FileSector_array[$j]), 
		      $NewName, 
		      GetVgoat("none",$isdnOSGoats,$VinfMethod), 
		      GetVAttr("none",$VinfMethod), 
		      "ES1-none", 
		      GetVMzones("none"), 
		      GetVEzones("none"), 
		      $isParaRef->{Parameter}];
  }
  #logV("VidesSummary:\n",1);
  #for ($i=0; $i < $#SampleTmp; $i++) {
  #  my $line = $SampleTmp[$i];
  #  logV("$i: ", 1);
  #  for ($j=0; $j < scalar @$line; $j++) {
  #    logV("$$line[$j], ", 1);
  #  }
  #  logV("\n", 1);
  #}
  # static analyze files
  StaticAnalyzeMkII($isParaRef->{isdnTempPath},
		    $isParaRef->{ISDNUNC}, 
		    $isParaRef->{Parameter},
		    $isdnOSGoats,
		    \@SampleTmp,
		    $WrittenSystemSectors);
  
  # check the termination of the emulator
  
  $isParaRef->{ReturnExitMsg} = GetEmuExitCode("$isParaRef->{ISDNWORKDIR}/$isfnVEmuLog");
  
  # write the sample file
  
  logV("VidesSummary: writing samples.lst\n");

  for ($i = 0; $i <= $#Sample; $i++) {
      logV("VidesSummary: BoxPath: $Sample[$i][$iBoxPath], Path: $Sample[$i][$iPath], GoatName: $Sample[$i][$iGoatName], Attributes: $Sample[$i][$iAttributes], Determination: $Sample[$i][$iDetermination], ModifiedZones: $Sample[$i][$iModifiedZones], ExecZones: $Sample[$i][$iExecZones], RunNr: $Sample[$i][$iRunNr]\n", 1);
  }
  WriteSamplesFileMkII($isParaRef->{ISDNUNC}, 
		   $isParaRef->{Cookie}, 
		   $isParaRef->{Parameter}, 
		   \@Sample);

  # clean the temp dir
  if ($isvarWipeTmp == 1) {
    $vreplogger->logString("cleaning $isParaRef->{ISDNWORKDIR}");
    File::Path::rmtree("$isParaRef->{ISDNWORKDIR}",$verbosepath,1); # delete it
  }
  # now return the number of infected objects
  
  # return count of file samples
  $isParaRef->{CountFiles} = $#Sample + 1;
  $vreplogger->logString("total possible file infections: $isParaRef->{CountFiles}");
  
  # return count of sector samples
  $isParaRef->{CountSectors} = ($#FileSector_array +1) 
    if ($FileSector_array[0]);
  $isParaRef->{CountSystemSectors} = $WrittenSystemSectors 
    if ($WrittenSystemSectors);
  $vreplogger->logString("total possible sector infections: $isParaRef->{CountSystemSectors}");
  
  # return total count of infected samples
  #$isParaRef->{CountSamples} = (scalar ($isParaRef->{CountFiles} + $isParaRef->{CountSystemSectors}));      
  #$isParaRef->{CountSamples} = (scalar ($isParaRef->{CountFiles} + $isParaRef->{CountSystemSectors}));      
  $isParaRef->{CountSamples} = $isParaRef->{CountFiles} + $isParaRef->{CountSystemSectors};      
  return (scalar ($isParaRef->{CountSamples}));  # return count samples
}


sub CheckExtractedFiles ($$) {
  my ($isParaRef, $OSGoats) = @_;
  logV("CheckExtractedFiles: checking dir: $isParaRef->{isdnTempPath}/$isdnExtracted\n",1);
  
  # compare extracted files with original goats
  # read in the file names
  my %extractedFiles = getDir("$isParaRef->{isdnTempPath}/$isdnExtracted");
  my (%FileDiffGoat, $keyfile);
  my $searchpath = "$isParaRef->{isdnTempPath}/$isdnExtracted";
  $searchpath =~ s/\\/\//g;
  my $replacepath = "$isdnVGoats/$OSGoats";
  $replacepath =~ s/\\/\//g;
  foreach $keyfile (keys %extractedFiles) {
    #logV("CheckExtractedFiles: Checking $keyfile\n", 1);
    my $filename = $keyfile;
    $filename =~ s/\\/\//g;
    my $goat = $filename;
    $goat =~ s/$searchpath\/$OSGoats\.($isdnExtractPost|$isdnExtractBefore)/$replacepath/i;
    $goat =~ s/(.+)(\..+)(\.)(\d+)$/$1$2/;  # remove .2  from a name BC.COM.2
    #logV("CheckExtractedFiles: compfiles($filename, $goat)\n",1);
    my $compStatus = compfiles($filename, $goat, 0); # possible replacement: File::Compare::compare, which seems to have the same functionality
    #logV("CheckExtractedFiles: \$compStatus: $compStatus = compfiles($filename, $goat)\n", 0);
    if ($compStatus == 1) {
      $FileDiffGoat{$filename} = 0; # initialize hash
    } 
  }
  foreach (keys %FileDiffGoat) { logV("CheckExtractedFiles: \$FileDiffGoat{$_} = '$FileDiffGoat{$_}'\n", 1); }
  logV("CheckExtractedFiles: end checkdir\n",1);
  return %FileDiffGoat;
}



######################################################################
######################################################################
# purpose: check the Termination code of the emulator
# input:   $filename of Emulator logfile (bochs.log)
# output:  Referenz to a hash with the following fields
# $isTerminateHash{TerminationMsg}
#  returns "OK" if the termination is normal
#  returns "INTERROR" if there was an internal Error
# $isTerminateHash{isLastPanicMsg}
#  returns the last termination message (the last panic)
# if no error message could be retrieved an "none" is returned
# in both fields

sub GetEmuExitCode($) {
  my ($isvarVElog) = @_; 
  my (%isTerminateHash) = ();

  $isTerminateHash{TerminationMsg} = "none";
  $isTerminateHash{LastPanicMsg}   = "none";

  open(ISFNVEMULOG,"<$isvarVElog") or Verror("open file $isvarVElog:", $!), return -1 ;
  while (<ISFNVEMULOG>) {
    if (/^bochs: panic/) {  # how had the emu been terminated?
      $isTerminateHash{LastPanicMsg} = $_;
      # search for something like this:  
      # bochs: panic at 0F6D:00000104, 0xFFFF Panic opcode found!
      # the segment address can allways change
      # (notice: this message is generated by the emulator on executing 
      # the STOP.COM program,
      #          which contains an invalid opcode 0xFFFF)
      if (/^bochs: panic at ([0-9A-F])+:00000104, 0xFFFF Panic opcode found!$/) {
	$isTerminateHash{TerminationMsg} = "OK";  # ok, this is our normal termination.
      } else {
	# means internal error of emulator
	# last panic error message is accesable via the hash
	$isTerminateHash{TerminationMsg} = "INTERROR";
      }  
      next;
    } # endif
    if (/^\^\^\^ /) { # was there a special warning / error ("^^^ INT") ?
      $isTerminateHash{SpecialError} = $_;
    }
  } # end while
  
  close ISFNVEMULOG;
  return (\%isTerminateHash);
}

######################################################################
######################################################################

sub GetVpath($) { 
  my ($fname)=@_;
  my ($tmpfn, $isdnDstType);
  # get path of infected sample
  $tmpfn = File::Basename::basename($fname);
  if ($tmpfn =~ /\.COM/i || $tmpfn =~ /\.EXE/i) {
    $isdnDstType = $isdnInfected;
  } else {
    $isdnDstType = $isdnSectors;
  }
  my $f_fullpath = "$isdnDstType/$tmpfn";
  logV( "GetVpath: $isdnDstType/$fname: $f_fullpath\n", 1);
  return $f_fullpath;
}

# MS: I think this is producing incorrect results. I think it should
# not ignore the drive letter and path completely: instead of using
# basename, it should just lob off any leading drive letter and
# convert it to the correct goat path. Or was this intensional???

sub GetVgoat($$$) { 
  my ($fname, $isdnOSGoats, $VinfMethod) = @_; 
  my $Vgoat; 
  if ($fname eq "none") { 
    if ((uc($VinfMethod) ne "POSTFIX") 
	&& (uc($VinfMethod) ne "INFIX") 
	&& (uc($VinfMethod) ne uc("none"))) { 
      $Vgoat = "$isdnOSGoats/VinfMethod.IMG"; 
    } else { 
      $Vgoat = "none"; 
    } 
  } else {
    my $newfname = $fname;
    $newfname =~ s/\://g; # strip the colon
    $Vgoat = "$isdnOSGoats/$newfname";
  } 
  return $Vgoat; 
}

# Get Determination
# ES1 = ExpertAnalyse fired
# SA1 = StaticAnalyse fired
# ES0 = ExpertAnalyse detected nothing
# SA0 = StaticAnalyse detected nothing
sub GetVdet ($) { 
  my ($fname)=@_;
  my $isvalVES = 1; # ExpertAnalyse found something
  my $isvalVSA = 1; # StaticAnalyse found something
  $isvalVES = 0;
  $isvalVSA = 0;
  my $Vdet = sprintf("ES%0d-SA%0d",$isvalVES,$isvalVSA);
  return $Vdet;
}

sub GetVAttr($$) { 
  my ($fname,$VinfMethod)=@_;
  # write some attributes
  # Question: No infection method?
  # $VinfMethod i.e. infection method "Postfix"
  
  my $len = "1701";                   # infection length
  my $dates = "19980929115902";       # file date / time
  my $AccessAttr = 0x41;              # file attributes
  $len        = "none";
  $dates      = "none";
  $AccessAttr = "none";
  my $VinfTarget = "none";             # infection target
  if ($fname =~ /\.COM/i) {
    $VinfTarget = "COM";
  } elsif ($fname =~ /\.EXE/i) {
    $VinfTarget = "EXE";
  }
  my $rc = sprintf("%s-%s-%s-%s-%s",$len,$dates,$AccessAttr,$VinfMethod,$VinfTarget);
  return $rc;
}

# input: infection method string
# output: string with reduced message of infection type
# returns for files "POSTFIX", "INFIX"
# returns for sectors"HDDMBR", "FDDBOOTA", "FDDBOOTB" , "HDDBOOT";
# or "none";
sub GetVinfectionMethod($\$) { 
  my ($VMethod,$GoatBase)=@_;
  my $VinfMethod = "none";             # infection method "Postfix"
  
  if ($VMethod =~ /Parasitic/i) {
    $VinfMethod = "POSTFIX";
  }
  elsif ($VMethod =~ /Overwriting/i) {
    $VinfMethod = "INFIX";
  } 
  elsif ($VMethod =~ /HDDMBR/i) {
    $VinfMethod = "HDDMBR";       
  } 
  elsif ($VMethod =~ /FDDBOOTA/i) {
    $VinfMethod = "FDDBOOTA";
  }
  elsif ($VMethod =~ /FDDBOOTB/i) {
    $VinfMethod = "FDDBOOTB";
  }
  elsif ($VMethod =~ /HDDBOOT/i) {
    $VinfMethod = "HDDBOOT";
  } 
  #elsif ($VMethod eq "") {  # this is redundant
  else {
    $VinfMethod = "none";
  }
  return $VinfMethod;
}

sub GetVMzones($) { 
  my ($fname)=@_;
  # list modification zones
  my $VMzones = "none";
  return $VMzones;
}

sub GetVEzones($) { 
  my ($fname)=@_;
  # list exec zones
  my $VEzones = "none";
  return $VEzones;
}


sub getNewSectors($$) { # get a copy of some system sectors
  my ($dst,$isdnOSGoats) = @_;
  my $isdnExtractPost = "afterwards";
  
  getFloppybootsector("$dst/a.img","$dst/extracted/$isdnOSGoats.$isdnExtractPost/00-00-0000-01.img");
  getFloppybootsector("$dst/b.img","$dst/extracted/$isdnOSGoats.$isdnExtractPost/01-00-0000-01.img");
  getHddmbrsector("$dst/c.img","$dst/extracted/$isdnOSGoats.$isdnExtractPost/80-00-0000-01.img");
  getHddbootsector("$dst/c.img","$dst/extracted/$isdnOSGoats.$isdnExtractPost/80-01-0000-01.img");

}

######################################################################
######################################################################

sub StaticAnalyzeMkII($$$$$$) {
  my ($tmpdir, $myunc, $myparam, $isdnOSGoats, $SamplesRef ,$bootcopy)  = @_;
  my ($i, $isdnDstType, $filename, $postname, $goatname, $TmpWork, $rvsaCheck, $SrcCpyName, $DstCpyName);

  logV("StaticAnalyzeMkII: Number of samples ". scalar @$SamplesRef ."\n", 1);
  $tmpdir =~ s/\\/\//g;
  getNewSectors($tmpdir,$isdnOSGoats);

  my @Samples = @$SamplesRef;
  for ($i = 0; $i <= $#Samples; $i++) {
    my $Sample = $Samples[$i];
    logV("StaticAnalyzeMkII: Path: $$Sample[$iPath] Goatname: $$Sample[$iGoatName] BoxPath: $$Sample[$iBoxPath]\n", 1);

    # get only the filename of the realtime extraction.
    # if there is a real-time extraction name
    if ($$Sample[$iBoxPath] !~ /none/i) { 
      # remove the path
      $filename = File::Basename::basename($$Sample[$iBoxPath]);  
    } else {
      # no filename
      $filename = "none";                                     
    }
    logV("StaticAnalyzeMkII: Analysis2: $filename\n", 1);
    # build path
    $TmpWork  = "$tmpdir/$isdnExtracted/$isdnOSGoats"; 
    # generate the afterwards filename
    $postname = "$TmpWork.$isdnExtractPost/$filename"; 
    # path to original goat
    $goatname = "$isdnVGoats/$$Sample[$iGoatName]"; 
    logV("StaticAnalyzeMkII: \$goatname: $goatname\n", 1);
    
    $rvsaCheck = saCheck($i,\$$Sample[$iPath],\$postname,\$goatname,\$$Sample[$iAttributes]);
    #######
    ####### Fexist as parameter for saCheck
    ####### need information if post is missing
    #######
    logV("StaticAnalyzeMkII: \$rvsaCheck $rvsaCheck\n", 1);
    
    # what's up when $$sampBoxPath[$i] == "none" ???
    
    if ($$Sample[$iGoatName] =~ /\.COM/i || $$Sample[$iGoatName] =~ /\.EXE/i) {
      $isdnDstType = "$isdnInfected";
    } else {
      $isdnDstType = "$isdnSectors";
    }
    ### $$sampPath[$i] =~ s|.+/$isdnExtracted/|$myunc/$isdnRun/$myparam/$isdnDstType/IBMnet0000-|; 
    
    $rvsaCheck -= 8 if $rvsaCheck >=8; # this means there is one file missing
    if ($rvsaCheck == 3 || $rvsaCheck == 7 || $rvsaCheck == 2) {
      $SrcCpyName = "$TmpWork.$isdnExtractBefore/$filename";
      $DstCpyName = "$myunc/$isdnRun/$myparam/$$Sample[$iPath]";
    }
    if ($rvsaCheck == 6) {
      logV("StaticAnalyzeMkII: Debug $_\n", 1);
      $SrcCpyName = "$TmpWork.$isdnExtractBefore/$filename";
      $DstCpyName = "$myunc/$isdnRun/$myparam/$$Sample[$iPath]";
    } elsif ($rvsaCheck == 1 || $rvsaCheck == 5) {
      my $dummyname = File::Basename::basename($$Sample[$iPath]);
      $SrcCpyName = "$TmpWork.$isdnExtractPost/$filename";
      $DstCpyName = "$myunc/$isdnRun/$myparam/$isdnDstType/$dummyname";
    } elsif ($bootcopy) {
      my $dummyname = File::Basename::basename($$Sample[$iPath]);
      $SrcCpyName = "$TmpWork.$isdnExtractBefore/$filename";
      $DstCpyName = "$myunc/$isdnRun/$myparam/$isdnDstType/$dummyname";
    }
    
    # mark file as analyzed 
    # set the sa# to sa3 sa7 sa5 or sa1, depending on returnvalue of $rvsaCheck
    
    # replace none with SA3 or SA7
    $$Sample[$iDetermination] =~ s/(.+)-(.+)/$1-SA$rvsaCheck/; 
    $$Sample[$iPath] = "$isdnRun/$myparam/$isdnDstType/". File::Basename::basename($$Sample[$iPath]);
    logV("StaticAnalyzeMkII: \$\$Sample[$iPath] $$Sample[$iPath]\n", 1);
    if (   $rvsaCheck == 1 
	|| $rvsaCheck == 5 
	|| $rvsaCheck == 3 
	|| $rvsaCheck == 7 
	|| $rvsaCheck == 2 
	|| $rvsaCheck == 6) {
      logV( "StaticAnalyzeMkII: copy $SrcCpyName $DstCpyName\n", 1);
      copy($SrcCpyName, $DstCpyName) 
	or logVW("StaticAnalyzeMkII: copy failed at line " . __LINE__ . " $SrcCpyName to $DstCpyName $!\n", $verbosemsg), return 0;
    } elsif ($bootcopy) { # if there was a write to the disk
      logV("StaticAnalyzeMkII: copy $SrcCpyName $DstCpyName\n", 1);
      # use a warning here, cause now we copy also objects which are not systemsectors
      logV("StaticAnalyzeMkII: Warning: copy additional sectors because $rvsaCheck\n", 1);
      copy($SrcCpyName, $DstCpyName) or logVW("StaticAnalyzeMkII: copy failed at line " . __LINE__ . " $SrcCpyName to $DstCpyName $!\n", $verbosemsg), return 0;
    } else {
      logVW("StaticAnalyzeMkII: Warning: Can't copy the files because $rvsaCheck\n", $verbosemsg);
    }
  }
}

######################################################################
######################################################################
# static analysis check
# compares three files, return the result of the three compares
# input:
#  index
#  sampPath ($f1)
#  postname ($f2)
#  goatname ($f3)
#  Vattribut
# Output:
#  sampPath and postname could be modified
# 
# the return value is binary, it is constructed by
# r = realtime.extraction.file
# p = postrun.extraction.file
# g = original.goat.file
# 0 = equal
# 1 = unequal, different
# rp= compare realtime with postrun extraction
# rg= compare realtime with original goat
# pg= compare postrun  with original goat
# * = copy files / + = problem, but copy files / - = problem, error
#     e {rp}{rg}{pg}
# 0 | 0  0   0   0   => - problem(rprgpg=000); 
#                         extraction; but no modification; eraseflag(r,p);
# 1 | 0  0   0   1   => +- error(rprgpg=001); 
#                          can't be, illegal, unless r is deleted
# 2 | 0  0   1   0   => - error(rprgpg=010); illegal or legal if p is missing
# 3 | 0  0   1   1   => * copy(r); eraseflag(p); 
#                         *** this is what we want!!! ***
# 4 | 0  1   0   0   => - error(rprgpg=100); can't be, illegal
# 5 | 0  1   0   1   => + problem(rprgpg=101); real is not modified; 
#                         copy(p); eraseflag(r,p);
# 6 | 0  1   1   0   => + problem(rprgpg=110); cause (rp=1)!; 
#                         copy(r); eraseflag(p);
# 7 | 0  1   1   1   => * all different; copy(r,p); eraseflag(r,p);
# 8 | 1              => if there was an error
sub saCheck($$$$$) {
  my ($n,$f1,$f2,$f3,$VAttribut) = @_;
  
  my ($r1,$r2,$r3,$p) = (-1,-1,-1,0);
  my ($Fexist, $isdnDstType, $extension);

  my $r0_p0_g0 = 0;
  my $r0_p0_g1 = 1;
  my $r0_p1_g0 = 2;
  my $r0_p1_g1 = 3;
  my $r1_p0_g0 = 4;
  my $r1_p0_g1 = 5;
  my $r1_p1_g0 = 6;
  my $r1_p1_g1 = 7;
    
  logV("saCheck: staticanalysis saCheck: $n\n");
  logV("saCheck: InImage: $$f1\n");
  logV("saCheck: After:   $$f2\n");
  logV("saCheck: OrgGoat: $$f3\n");
    
  # check which files exist
  $Fexist = FileExist($$f1,$$f2,$$f3);
    
  # if $r# == 0 => files equal
  # if $r# == 1 => files different
  if ($Fexist == $r1_p1_g0 || $Fexist == $r1_p1_g1) {
    $r1 = compare($$f1,$$f2,65536);
  }   
  if ($Fexist == $r1_p0_g1 || $Fexist == $r1_p1_g1) {
    $r2 = compare($$f1,$$f3,65536);
  }
  if ($Fexist == $r0_p1_g1 || $Fexist == $r1_p1_g1) {
    $r3 = compare($$f2,$$f3,65536);
  }

  # now create the binary return value
  $p = 0;
  $p += 8 if ($r1 == -1 || $r2== -1 || $r3== -1); # if there was an error 
  $p += 4 if ($r1 == 1);                          # if they were different
  $p += 2 if ($r2 == 1);
  $p += 1 if ($r3 == 1);
    
  # add diffsize
  my $isvarDiff_fsize = getVirusSize($$f1,$$f2,$$f3);
  my @VAttrib = split(/-/,$$VAttribut); # split attributes
  $$VAttribut = "$isvarDiff_fsize-$VAttrib[1]-$VAttrib[2]-$VAttrib[3]-$VAttrib[4]";
  logV("saCheck: new Attr: $$VAttribut\n", 1);

  logV("saCheck: Compare: $p = [$r1,$r2,$r3]; Fexist: $Fexist\n");
  # ideal: real equal post, real diff goat, post diff goat (0,1,1) (3)
  logV("real unequal post, ") if ($r1==1);
  logV("real equal post, ")   if ($r1==0);
  logV("real not found, ")    if ($r1==-1);
    
  logV("real unequal goat, ") if ($r2==1);
  logV("real equal goat, ")   if ($r2==0);
  logV("goat not found, ")    if ($r2==-1);
    
  logV("post unequal goat, ") if ($r3==1);
  logV("post equal goat, ")   if ($r3==0);
  logV("post not found, ")    if ($r3==-1);
  logV("\n");
    
  if ($$f3 =~ /.COM$/i || $$f3 =~ /.EXE$/i) {
    $isdnDstType = "$isdnInfected";
  } else {
    $isdnDstType = "$isdnSectors";
  }
  if ($p == 3) { #real equ post, real diff goat, post diff goat
    # get file extension, remove everything before the first point,
    # includeing the point.  
    $extension = (File::Basename::fileparse($$f1, "\..*"))[2];
    $$f1 = sprintf("$isdnDstType/%08d.$extension",$n);
    ###$$f1 = "$isdnDstType/" . File::Basename::basename($$f2);
    logV("saCheck: $p:" . $$f1 . " ==> " . File::Basename::basename($$f2) . "\n");
  }
  if ($p == 5 || $p == 1 || $p == 8+1 || $p == 8+5) {
    #$extension = File::Basename::basename($$f2);
    #$extension =~ s/.*\.//;
    $extension = (File::Basename::fileparse($$f2, "\..*"))[2];
    $$f1 = sprintf("$isdnDstType/%08d.$extension",$n);
    ###$$f1 = "$isdnDstType/" . File::Basename::basename($$f2);
    logV("saCheck: $p:" . $$f1 . " ==> " . File::Basename::basename($$f2) . "\n");   
  }
  # (($p == 8 + 2)&& ($Fexist == $r1_p0_g1))) means post doesn't
  # exist, and r is different from g
  if (($p == 8 + 2) && ($Fexist == $r1_p0_g1)) {
    $$f2 = File::Basename::basename($$f1);
    $extension = (File::Basename::fileparse($$f2, "\..*"))[2];
    $$f1 = sprintf("$isdnDstType/%08d.$extension",$n);
    ###$$f1 = "$isdnDstType/" . File::Basename::basename($$f2);
    logV("saCheck: $p:" . $$f1 . " ==> " . File::Basename::basename($$f2) . "\n");   
  }
  if ($p == 6 ) {
    my $newf1 = "$isdnDstType/" . File::Basename::basename($$f1);
    $$f1 = $newf1;
    logV("saCheck: $$f1 --> $newf1\n");
  }
  logV("saCheck: $p=f1: $$f1 f2: $$f2\n");
  return $p;
}

######################################################################
######################################################################
# returns integer value, if files r,p,g exist.
# r = realtime.extraction.file
# p = postrun.extraction.file
# g = original.goat.file
# 0   file does not exist
# 1   file exists
# # | {r} {p} {g}    
# 0 |  0   0   0   => - error(rpg=000); no file exist
# 1 |  0   0   1   => problem(rpg=001); only goat exist; rp are deleted?
# 2 |  0   1   0   => problem(rpg=010); only post exist; new generated file?;
# 3 |  0   1   1   => * ok, no realtime extraction, but afterwards
# 4 |  1   0   0   => prob(rpg=100); maybe sector extract, otherwise problem
# 5 |  1   0   1   => + prob(rpg=101); r exist but p not?; maybe p is erased?
# 6 |  1   1   0   => + prob(rpg=110); r & p newly generated?
# 7 |  1   1   1   => * ok, all files exist  
sub FileExist($$$){
  my (@p) = @_;
  my ($rv) = 0;
  $rv += 4 if (-e $p[0] && !-z $p[0]);
  $rv += 2 if (-e $p[1] && !-z $p[1]);
  $rv += 1 if (-e $p[2] && !-z $p[2]);
  return $rv; 
}

######################################################################
######################################################################

sub staticCheck(%) {
  my ($isParaRef) = @_;
  my $copyReal = 0;
  my $difftogoat1 = $isParaRef->{DiffList};
  my $rv = 0;

  my %difftogoat2 = %$difftogoat1;
  logV("staticCheck: \$isParaRef->{DiffList}->{$isParaRef->{SamplePath}}: '$isParaRef->{DiffList}->{$isParaRef->{SamplePath}}'\n",1);
  logV("staticCheck: \$isParaRef->{SampleBoxPath}: '$isParaRef->{SampleBoxPath}'\n",1);
  logV("staticCheck: \$isParaRef->{SamplePath}: '$isParaRef->{SamplePath}'\n", 1);
  logV("staticCheck: \$isParaRef->{Destination}: '$isParaRef->{Destination}'\n", 1);
  my $file = File::Basename::basename($isParaRef->{SamplePath});  # get only the filename
  $file =~ s/(.+)(\..+)(\.)(\d+)$/$1$2/;   # remove .2  from a name BC.COM.2
  
  my @specificfiles = grep { m/$file/i} (keys %difftogoat2);
  my @rtfiles       = grep { m/$isdnExtractBefore/i } (@specificfiles);
  my @awfiles       = grep { m/$isdnExtractPost/i } (@specificfiles);
  my ($x, $f);
  for ($x=0; $x <= $#specificfiles; $x++) {     
    foreach $f (@rtfiles, @awfiles) {
      logV("staticCheck: \$x = $x, \$f = '$f', \$isParaRef->{SamplePath} = '$isParaRef->{SamplePath}'\n", 1); 
      if (($f !~ /$isParaRef->{SamplePath}/i) 
	  && (($difftogoat2{$isParaRef->{SamplePath}} != 1) 
	      || ($difftogoat2{$f} != 1))) {
	if (compfiles($isParaRef->{SamplePath}, $f, 1) == 0) { # if equal
	  if ($difftogoat2{$isParaRef->{SamplePath}} == 0 
	      && $difftogoat2{$f} == 0) {
	    $difftogoat2{$isParaRef->{SamplePath}} = 1;
	    $isParaRef->{DiffList}->{$isParaRef->{SamplePath}} = 1;
	    $isParaRef->{DiffList}->{$f} = 1;
	    $difftogoat2{$f} = 1;
	    $copyReal = 1;
	    $isParaRef->{SampleGoatName} = $f;
	    logV("staticCheck: request copy of $isParaRef->{SamplePath} $difftogoat2{$isParaRef->{SamplePath}}, $difftogoat2{$f}\n",1);
	  } elsif ($difftogoat2{$isParaRef->{SamplePath}} == 1 
		   && $difftogoat2{$f} == 1) {
	    logV("staticCheck: don't request copy of $difftogoat2{$isParaRef->{SamplePath}}, $difftogoat2{$f}\n",1);
	  } elsif ($difftogoat2{$isParaRef->{SamplePath}} == 0 
		   && $difftogoat2{$f} == 1) {
	    $difftogoat2{$isParaRef->{SamplePath}} = 1;
	    $isParaRef->{DiffList}->{$isParaRef->{SamplePath}} = 1;
	    #$difftogoat2{$ucSPath} = 1;
	    $isParaRef->{DiffList}->{$isParaRef->{SamplePath}} = 1;
	    logV("staticCheck: don't request copy $difftogoat2{$isParaRef->{SamplePath}},$difftogoat2{$f}\n",1);
	  } elsif ($difftogoat2{$isParaRef->{SamplePath}} == 1 
		   && $difftogoat2{$f} == 0) {
	    $difftogoat2{$f} = 1;
	    $isParaRef->{DiffList}->{$f} = 1;
	    logV("staticCheck: don't request copy $difftogoat2{$isParaRef->{SamplePath}},$difftogoat2{$f}\n",1);
	  }
	} else { # unequal
	  logV("staticCheck: different $isParaRef->{SamplePath} $difftogoat2{$isParaRef->{SamplePath}}, $difftogoat2{$f}\n",1); 
	}
      }
    }
  }

  if ($copyReal) {
    $difftogoat2{$isParaRef->{SamplePath}} = 1;
    $isParaRef->{DiffList}->{$isParaRef->{SamplePath}} = 1;
    logV( "staticCheck: copyreal: $isParaRef->{Destination},$isParaRef->{SamplePath}\n", 1);
    my ($rv, $newName) = changeName($isParaRef->{Destination},$isParaRef->{SamplePath});
    if ($rv == 1) {
      $isParaRef->{SamplePath} = $newName;
      $copyReal = 0;
      $rv = 1;
      logV("staticCheck: MOVED $isParaRef->{SamplePath} $newName\n",1);
    }
  }
  logV("staticCheck: SampleBoxPath: $isParaRef->{SampleBoxPath}\n", 1);
  logV("staticCheck: SamplePath:    ". ($isParaRef->{SamplePath})."\n", 1);
  logV("staticCheck: \$test:        $file\n",1);
  return $rv;
}

######################################################################
######################################################################
    
sub getFloppybootsector($$) {
  my ($srcimage, $dstimage, ) = @_;
 
  logV(  "getFloppybootsector: getting bootsector of $srcimage\n", 1);
  system "$isdnBinDir/fddboot $srcimage";
  my $src = "fdd.boo";
  
  if (-e $src) {
    logV( "getFloppybootsector: $src extracted\n", 1);
    copy $src, $dstimage 
      or Verror("error on copy $src to $dstimage", $!), return -1;
    unlink $src 
      or Verror("error on unlink $src", $!), return -1;
  }
}

######################################################################
######################################################################

sub getHddbootsector($$) {
  my ($srcimage, $dstimage, ) = @_;
  
  logV(  "getHddbootsector: getting hddboot sector of $srcimage\n", 1);
  system "$isdnBinDir/hddboot $srcimage";
  my $src = "hdd.boo";
  
  if (-e $src) {
    logV( "getHddbootsector: $src extracted\n", 1);
    copy($src, $dstimage) 
      or Verror("error on copy $src to $dstimage", $!), return -1;
    unlink $src 
      or Verror("error on unlink $src", $!), return -1;
  }
}

######################################################################
######################################################################
 
sub getHddmbrsector($$) {
  my ($srcimage, $dstimage, ) = @_;
  
  logV(  "getHddmbrsector: getting hddmbr of $srcimage\n", 1);
  system "$isdnBinDir/hddmbr $srcimage";
  my $src = "hdd.mbr";
    
  if (-e $src) {
    logV( "getHddmbrsector: $src extracted\n", 1);
    copy($src, $dstimage)
      or Verror("error on copy $src to $dstimage", $!), return -1;
    unlink $src 
      or Verror("error on unlink $src", $!), return -1;
  }
}

######################################################################
######################################################################
#
#  parses  RunType string
#  input:  RunType string
#  output: $floppya,$floppyb,$diskc,$isVCPU,$isVCPUid,$isOSvers
#
sub getRunType($) {
  my ($VRtype) = @_;
  my $MaxRunTypeFields = 6; # i.e. "144-120-10M-I386-NONE-MSDOS622EA"
  my @VRtyp = split(/-/,$VRtype);
   
  if (scalar $#VRtyp != ($MaxRunTypeFields - 1)) { # correct number of fields
    logVW("getRunType: Warning: Wrong number of parameters in vruntype $#VRtyp != $MaxRunTypeFields\n");
    for(my $i=0;$i < $#VRtyp; $i++) {
      logVW("getRunType: $i: $VRtyp[$i]\n");
    }
  }
  return("$VRtyp[0].img", # Floppy A image
	 "$VRtyp[1].img", # Floppy B image
	 "$VRtyp[2].img", # Disk C image
	 $VRtyp[3],       # CPU
	 $VRtyp[4],       # CPU Id
	 $VRtyp[5]        # Operating System version
	);
}

######################################################################
######################################################################
#
#  parses  RunStrategy string
#  input:  RunStrategy string
#  output: 
#         $RunStrategyMode      == "FILE" or "BOOT"
#         $RunStrategyType      == "DEFAULT" 
#         $RunStrategyFunction  == "TSROPENCLOSE"
#
sub getRunStrategy($) {
  my ($VRStrat) = @_;
  my ($RunStrategyMode,$RunStrategyType,$RunStrategyFunction);
  my $MaxRunStrategyFields = 3; # i.e. "FILE-DEFAULT-TSROPENCLOSE"
  my @VRStr = split(/-/,$VRStrat);
   
  if (scalar $#VRStr != ($MaxRunStrategyFields - 1)) { # correct number of fields
    logVW("getRunStrategy: Warning: Wrong number of parameters in vrunstrategy $#VRStr != $MaxRunStrategyFields\n");
    for(my $i=0;$i < $#VRStr; $i++) {
      logVW("getRunStrategy: $i: $VRStr[$i]\n");
    }
  }
   
  $RunStrategyMode     = $VRStr[0] ;
  $RunStrategyType     = $VRStr[1] ;
  $RunStrategyFunction = $VRStr[2] ;
  return($RunStrategyMode,$RunStrategyType,$RunStrategyFunction);
}

######################################################################
######################################################################
# input: fnReal => realtime extraction filename
#        fnPost => post     extraction filename
#        fnOrg  => original goat       filename 
# output: diffsize (length of virus)
#        0 => no files
#        0 => cavity
#         
sub getVirusSize($$$) {
  my ($fnReal, $fnPost, $fnOrg) = @_;
  my ($isvarFileFsize, $isvarPostFsize, $isvarOrigFsize, $isvarDiffFsize);
 
  # file must exist and doesn't have zero filesize
  # get filesize
  logV("getVirusSize($fnReal, $fnPost, $fnOrg)\n", 1);
  # get filesize of
  # real-time extraction
  $isvarFileFsize = ((stat($fnReal)) [7]) if(-e $fnReal && !-z $fnReal);  
  # post-extraction
  $isvarPostFsize = ((stat($fnPost)) [7]) if(-e $fnPost && !-z $fnPost);
  # original goat  
  $isvarOrigFsize = ((stat($fnOrg))  [7]) if(-e $fnOrg && !-z $fnOrg);
  logV("getVirusSize: (current: $isvarFileFsize, post: $isvarPostFsize, original: $isvarOrigFsize)\n", 1);
  
  if($isvarFileFsize && $isvarOrigFsize) { # get virus size
    $isvarDiffFsize = $isvarFileFsize - $isvarOrigFsize;
  } elsif ($isvarPostFsize && $isvarOrigFsize){
    $isvarDiffFsize = $isvarPostFsize - $isvarOrigFsize;
  } else {
    $isvarDiffFsize = "none"
  }
 
  logV("getVirusSize: $fnReal: OrgSize: $isvarOrigFsize NewSize: $isvarFileFsize , $isvarPostFsize DiffSize: $isvarDiffFsize\n",1);
  return ($isvarDiffFsize)
}

######################################################################
######################################################################
# input: name of infected file
#        name of original goat
# output: diffsize (length of virus)
#        0 => no files
#        0 => cavity
#         
sub calcVirusSize($$) {
  my ($fnFile,$fnOrg) = @_;
  my ($isvarFileFsize, $isvarOrigFsize, $isvarDiffFsize);
  #logV("calcVirusSize($fnFile, $fnOrg)\n", 1);
  # file must exist and doesn't have zero filesize
  
  # get filesize of
  $isvarFileFsize = ((stat($fnFile)) [7]) if(-e $fnFile && !-z $fnFile);  # infected file
  $isvarOrigFsize = ((stat($fnOrg))  [7]) if(-e $fnOrg && !-z $fnOrg);    # original goat
  logV("calcVirusSize: current size: $isvarFileFsize, original size: $isvarOrigFsize\n", 1);
    
  if($isvarFileFsize && $isvarOrigFsize) { # get virus size
    $isvarDiffFsize = $isvarFileFsize - $isvarOrigFsize;
  } else {
    $isvarDiffFsize = "none"
  }
 
  logV("calcVirusSize: $fnFile: OrgSize: $isvarOrigFsize NewSize: $isvarFileFsize DiffSize: $isvarDiffFsize\n",1);
  return $isvarDiffFsize;
}

######################################################################
######################################################################

sub Verror($$) {
  my ($msg1,$msg2) = @_;
  logV( "Verror: #### $msg1 $msg2\n",1);
  logV( "Verror: at " . __LINE__ . " in " . __FILE__ . "\n",1);
  return 1;
}

######################################################################
######################################################################

sub findfunction {
  return unless -f;      # no directories
  $filenamesfound{$File::Find::name} = 0; 
}

######################################################################
######################################################################
# input: Directory.
# output: Ref. to hash, containing all files in this Directory and
# below as the key
sub getDir($) {
  my ($StartDir) = @_;
  File::Find::find(\&findfunction, $StartDir);
  return (%filenamesfound);
}

######################################################################
######################################################################
# returns 0 if files are equal
#         1 if files are different
#        -1 if there was a file problem
sub checkfiles {
  my ($file1, $file2, $destination,$verbose) = @_;
  my ($newfilename, $srStatus, $cmpStatus, $rv);
  logV("checkfiles: $file1, $file2, $destination\n",1) if $verbose;
  if (-e $file1 && !-z $file1 && -e $file2 && !-z $file2) {
    $cmpStatus = File::Compare::compare($file1,$file2,65536);
    $rv = $cmpStatus;
    if ($cmpStatus) {
      # create newfilename
      $newfilename = "$destination/".File::Basename::basename($file1); 
      ($srStatus, $newfilename) = smartrename($newfilename);
      if ($srStatus == 0) {
	logV("checkfiles: copy $file1, $newfilename\n") if $verbose;
	copy $file1, $newfilename;
      }
    }
  } else { 
    $rv = -1;  # error at least one file is missing or has zero value
  }
  return ($rv, $newfilename);
}

# purpose:
# use:   my ($returncode,$NewFileName) = smartrename($FileName);
#
# input: filename
# returns:
#    integer 
#      0 = "success"
#      1 = "failed" ("could not generate new filename, new file allready exist")

######################################################################
######################################################################
#    string  (containing a (new) filename)
#
# function: return's a variation of the filename, so that each file is unique
#           in a given directory
# appends an underscore plus a number from 1 to 999.
# C100.COM = newname => C100_1.COM
# C100_1.COM => C100_2.COM
#
sub smartrename($) {
  my ($orgname) = @_;
  my $icMaxExtensionCounter = 999;  # counter limit
  my $ivExtensionCounter = 0;
  my $orgnameBase = $orgname;
  my $orgnameExt  = $orgname;
  my $orgnameNum  = 0;
  my $rc = 0;
  while( ($ivExtensionCounter < $icMaxExtensionCounter) && ($orgname) && (-e $orgname)) {
    $ivExtensionCounter++;
    if ($orgname =~ m/_(\d+)\./i) {  # c1000_1.com
      $orgname =~ m/(.+)_(\d+)\.(.+)$/i;  # everything before last point
      $orgnameBase= $1;
      $orgnameNum = $2;
      $orgnameExt = $3;
      if ($orgnameNum+1 >= $icMaxExtensionCounter) {
	$orgnameNum = $ivExtensionCounter;
      }
      $orgname = sprintf("%s_%d.%s",$orgnameBase,($orgnameNum+1),$orgnameExt);
    } else { # no number in filename
      $orgnameBase = "";
      $orgnameExt  = "";
      $orgname =~ m/(.+)\.(.+)$/i;
      $orgnameBase = $1; 	# everything before last point
      $orgnameExt  = $2; 	# everything after the first point
      $orgname = sprintf("%s_%d.%s",$orgnameBase,$ivExtensionCounter,$orgnameExt);
    }
  }
  # if the file still exist, return error code
  $rc = 1 if (-e $orgname); 
  return ($rc , $orgname);
}

######################################################################
######################################################################
# returns 0 if files are equal
#         1 if files are different
#        -1 if there was a file problem
sub compfiles ($$$) {
  my ($file1, $file2, $verbose) = @_;
  my ($cmpStatus, $rv);
  logV("compfiles($file1, $file2)\n", 1) if $verbose;
  if ((-e $file1) && (!-z $file1) && (-e $file2) && (!-z $file2)) {
    if (-s $file1 == -s $file2) { # compare file sizes
      #$cmpStatus = File::Compare::compare($file1,$file2,65536);
      $cmpStatus = File::Compare::compare($file1,$file2);
      $rv = $cmpStatus;
    } else { 
      $rv = 1; # filesize is different
    }
  } else { 
    $rv = -1;  # error at least one file is missing or has zero value
  }
  return ($rv);
}

######################################################################
######################################################################

sub changeName {
  my ($destination,$file,$verbose) = @_;
  my ($newfilename, $srStatus, $rv); 
  $rv = 0;
  logV("changeName: $destination $file\n") if $verbose;
  $newfilename = "$destination/" . File::Basename::basename($file); # create newfilename
  ($srStatus, $newfilename) = smartrename($newfilename);
  if ($srStatus == 0) {
      logV( "changeName: copy $file, $newfilename\n",1);
    copy $file, $newfilename  or logVW("changeName: copy failed at line " . __LINE__ . " $file to $newfilename $!\n", $verbose), return 0;
    $rv = 1;
  }

  return( $rv, $newfilename);
}

######################################################################
######################################################################
# equal to warn, but with logging
sub logV {
  my ($msg,$verbose) = @_;
   
  unless ($verbose) { # if there is a 2nd parameter, don't show it on screen
    AVIS::Log::Log($isfnVRLOG,$msg,"OFF");
  } else {             # show it on screen
    AVIS::Log::Log($isfnVRLOG,$msg,"ON");
  }
}

######################################################################
######################################################################
# equal to logV but instead of using print to screen, it's using warn,
# which also goes to stderr
sub logVW {
  my ($msg,$verbose) = @_;
  
  unless ($verbose) { # if there is a 2nd parameter, don't show it on screen
    AVIS::Log::Log($isfnVRLOG,$msg,"OFF");
  } else {             # show it on screen
    AVIS::Log::Log($isfnVRLOG,$msg,"OFF");
    warn($msg);
  }
}

######################################################################
######################################################################
sub ParseVidesLog {
  my ($logfile, $FileNameArray, $FileInfType, $TriggerArray, $FileSector_array, $FileExtractSrc, $FileExtractDst, $SectorInfT_array, $C_infectionBS, $C_infectionMB, $A_infectionBS, $B_infectionBS) = @_;
  # Local variables:
  my ($FileNameIndex, @TriggerCount, @ExtractCount, @FileInfCount, $SectorCount, $ExtractIndex);
  $FileNameIndex = 0;
  $SectorCount = 0;
  open(ISFNLOG,"<$logfile") or return 0;
  
  while (<ISFNLOG>) {    
    chomp;                  # no newline
    s/\#.*//;               # no comments
    s/^\s+//;               # no leading white
    s/\s+$//;               # no trailing white
    next unless length;     # anything left?
    #   now parse it
    if (0) { # only look for extracted files
      if (/Possible Parasitic Infection on (.+)/) {
      logV("ParseVidesLog: $_\n", 1);
      my $InfectedFile = $1;
      $InfectedFile =~ s|\\|/|g;
      $FileNameArray->[$FileNameIndex] = $InfectedFile;
      $FileInfType->[$FileNameIndex] = "Parasitic";
      logV("ParseVidesLog: \$FileNameArray->[$FileNameIndex]: $FileNameArray->[$FileNameIndex]\n\$FileInfType->[$FileNameIndex]: $FileInfType->[$FileNameIndex]\n", 1);
      push @FileInfCount , $FileNameIndex;  # append todo index
      push @TriggerCount , $FileNameIndex;  # append todo index
      push @ExtractCount , $FileNameIndex;  # append todo index
      $FileNameIndex++;
      next;
    }
    if (/Possible Overwriting Infection on (.+)/) {
      logV("ParseVidesLog: $_\n", 1);
      my $InfectedFile = $1;
      $InfectedFile =~ s|\\|/|g;
      $FileNameArray->[$FileNameIndex] = $InfectedFile;
      $FileInfType->[$FileNameIndex] = "Overwriting";
      logV("ParseVidesLog: \$FileNameArray->[$FileNameIndex]: $FileNameArray->[$FileNameIndex]\n\$FileInfType->[$FileNameIndex]: $FileInfType->[$FileNameIndex]\n", 1);
      push @FileInfCount , $FileNameIndex;  # append todo index   
      push @TriggerCount , $FileNameIndex;  # append todo index
      push @ExtractCount , $FileNameIndex;  # append todo index
      $FileNameIndex++;
      next;
    }
    if(/Possible infection trigger is DOS function ([0-9A-F]+)/){
      logV("ParseVidesLog: $_\n", 0);
      my $TriggerFunction = $1;
      if (@TriggerCount) {
	my $TriggerIndex = shift @TriggerCount;
	$TriggerArray->[$TriggerIndex] = $TriggerFunction;
	logV("ParseVidesLog: \$TriggerArray->[$TriggerIndex]: $TriggerArray->[$TriggerIndex]\n", 0);
      }
      next;
    }
    }
    if(/EXTRACTING FILE (.+) TO FILE (.+)/){
      logV("ParseVidesLog: $_\n", 0);
      my $FileNameInImage   = $1;           # the name of the file in the image
      my $FileNameExtracted = $2;           # the extracted filename,
                                            # which may be different
                                            # from the above
      $FileNameInImage   =~ s|\\|/|g;
      $FileNameExtracted =~ s|\\|/|g;
      if (@ExtractCount) {
	$ExtractIndex = shift @ExtractCount;
	$FileExtractSrc->[$ExtractIndex] =  $FileNameInImage;
	$FileExtractDst->[$ExtractIndex] =  $FileNameExtracted;
	logV("ParseVidesLog: \$FileExtractSrc->[$ExtractIndex]: $FileExtractSrc->[$ExtractIndex]\n\$FileExtractDst->[$ExtractIndex]: $FileExtractDst->[$ExtractIndex]\n", 0);
      }
      next;
    }
    if(/EXTRACTING SECTOR (.+)/){
      logV("ParseVidesLog: $_\n", 0);
      my $SectorFileNameExtracted = $1;
      $SectorFileNameExtracted =~ s|\\|/|g;
      # match something like 80-00-0000-01 (HD-MBR)
      # 00-00-0000-01 (FDD-Boot)
      # 80-01-0000-01
      my $SectorInfType = "none";
      if ($SectorFileNameExtracted =~ /[08][01]-0[01]-0000-01/) {
	if    ($SectorFileNameExtracted =~ /80-00-0000-01/) {
	  $SectorInfType = "HDDMBR";
	} elsif ($SectorFileNameExtracted =~ /00-00-0000-01/) {
	  $SectorInfType = "FDDBOOTA";   # Floppy Bootsector A               
	} elsif ($SectorFileNameExtracted =~ /80-01-0000-01/) {
	  $SectorInfType = "HDDBOOT";   # Harddisk Bootsector            
	} elsif ($SectorFileNameExtracted =~ /01-00-0000-01/) {
	  $SectorInfType = "FDDBOOTB";               
	}
      }
      #logV("ParseVidesLog: captured sector: $SectorFileNameExtracted,$SectorInfType\n");
      
      $FileSector_array->[$SectorCount] = $SectorFileNameExtracted;
      $SectorInfT_array->[$SectorCount] = $SectorInfType;
      logV("ParseVidesLog: \$FileSector_array->[$SectorCount]:$FileSector_array->[$SectorCount]\n\$SectorInfT_array->[$SectorCount]: $SectorInfT_array->[$SectorCount]\n", 0);
      $SectorCount++;
      next;
    }
    # old, but shorter
    $$C_infectionBS++ if /Write.+HDD Boot Sector/;           
    # old, but shorter
    $$C_infectionMB++ if /Write.+HDD MBR/;                     
    # shorter
    $$A_infectionBS++ if /Write.+FDA Boot Sector/;
    # shorter             
    $$B_infectionBS++ if /Write.+FDB Boot Sector/;
  }
  close ISFNLOG;
  logV("ParseVidesLog: \$C_infectionBS: $$C_infectionBS, \$C_infectionMB: $$C_infectionMB, \$A_infectionBS: $$A_infectionBS, \$B_infectionBS: $$B_infectionBS\n", 1);

}

######################################################################
######################################################################
sub KeySectorInfectionFound ($) {
  my ($logfile) = @_;
  my $SectorCount = 0;

  logV( "entering KeySectorInfectionFound\n" ,1);
  open(ISFNLOG,"<$logfile") or return 0;
  
  while (<ISFNLOG>) {    
    chomp;                  # no newline
    s/\#.*//;               # no comments
    s/^\s+//;               # no leading white
    s/\s+$//;               # no trailing white
    next unless length;     # anything left?
    #   now parse it
    if (/Write.+HDD Boot Sector/) {
      $SectorCount++ ;
      logV( "KeySectorInfectionFound: Possible bootsector infection",1);
    }
    # old, but shorter
    if (/Write.+HDD MBR/) {
      $SectorCount++ ;
      logV( "KeySectorInfectionFound: Possible MBR infection",1);
    }
  }
  close ISFNLOG;
  logV( "Found $SectorCount sectors\n",1);
  return $SectorCount;
}

######################################################################
######################################################################

sub GetAttributes($$) {
  my ($virussize, $goatname) = @_;

  if ($goatname =~ /\.COM/i) {
    return "$virussize-none-none-none-COM";
  } elsif ($goatname =~ /\.EXE/i) {
    return "$virussize-none-none-none-EXE";
  } elsif ($goatname =~ /\.SYS/i) {
    return "$virussize-none-none-none-SYS";
  } else {
    return "$virussize-none-none-none-none";
  }
}

######################################################################
######################################################################

=head2 ReadSamplesFile

ReadSamplesFile($unc, $cookie, $run, $num, $Path, $BoxPath, $GoatName, $Determination, $Attributes, $ModifiedZones, $ExecZones, $runnr)

Read the samples file of a run into arrays specified by their references. 

=cut

sub ReadSamplesFileMkII($$$) {
  my ($unc, $cookie, $run) = @_;
  my ($filename, $Samples);
  
  # define the samples list file based on the $unc and the $run. If
  # $run is -1 the use is requesting the central samples file which is
  # located in $unc/. Otherwise the file is located in
  # $unc/$isdnRun/$run/ (which might look like:
  # \\blueice/Cookie00004711/Run/0/).
  if ($run == -1) {
    $filename = "$unc/$isfnSamples";
  } else {
    $filename = "$unc/$isdnRun/$run/$isfnSamples";
  }
  logV( "ReadSamplesFileMkII: Using '$filename' as samples file\n",1);
  my ($d, $rc);
  open IN, "<$filename" or return ();
  $rc = read IN, $d, 32000;
  close IN;
  logV( "ReadSamplesFileMkII: Samples file: {$d}\n",1);
  eval $d;
  if ($verbosemsg) {
    my ($row, $col);  
    foreach $row (@$Samples) {
      foreach $col (@$row) {
	logV( "$col ",1);
      }
      logV( "\n",1);
    }
  }
  return @$Samples;
}

######################################################################
######################################################################

=head2 WriteSamplesFile

WriteSamplesFile($unc, $cookie, $run, $num, $Path, $BoxPath, $GoatName, $Determination, $Attributes, $ModifiedZones, $ExecZones, $runnr)

Write the samples file of a run from arrays specified by their references. 

=cut

sub WriteSamplesFileMkII($$$$) {
  my ($unc, $cookie, $run, $Samples) = @_;
  my ($filename);
    
  # define the samples list file based on the $unc and the $run. If
  # $run is -1 the use is requesting the central samples file which is
  # located in $unc/. Otherwise the file is located in
  # $unc/$isdnRun/$run/ (which might look like:
  # \\blueice/Cookie00004711/Run/0/).
  if ($run == -1) {
    $filename = "$unc/$isfnSamples";
  } else {
    $filename = "$unc/$isdnRun/$run/$isfnSamples";
    File::Path::mkpath("$unc/$isdnRun/$run",0,0777);
  }
  if (open(OUT, ">$filename")) {
    my $d = Data::Dumper->new([$Samples], [qw(Samples)]);
    $d->Purity(0);
    $d->Useqq(1);
    print OUT $d->Dump;
    close OUT;
  } else {
    logV( "WriteSamplesFileMkII: File $filename could not be written to\n",1);
  }
  return 1;
}

sub MorphGoatName ($$$$) {
  my ($originalgoatname, $isdnExtractBefore, $isdnExtractPost, $isdnOSGoats) = @_;
  my $goatname = $originalgoatname;
  $goatname =~ s/(.+\.$isdnExtractPost[\\\/])([A-Za-z])([\\\/].+)/$2$3/i;
  $goatname =~ s/(.+\.$isdnExtractBefore[\\\/])([A-Za-z])([\\\/].+)/$2$3/i;
  $goatname = "$isdnOSGoats/$goatname";
  return $goatname;
}

sub RemoveUNC ($$) {
  my ($shortSamplePath, $myunctmp) = @_;
  $shortSamplePath =~ s/\\/\//g;
  $myunctmp =~ s/\\/\//g;
  $shortSamplePath =~ s/($myunctmp)([\\\/])(.+)/$3/i; # now remove the UNC
  return $shortSamplePath;
}


