####################################################################
#                                                                  #
# Package name:  AVIS::Macro::Replication                          #
#                                                                  #
# Description:   Replicate macro viruses on unix machines          #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Jean-Michel Boulay, Morton Swimmer                #
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
package AVIS::Macro::Replication;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw();
@EXPORT_OK = qw();

$VERSION = 2.0;


use strict;

use File::Copy;
use File::Path;
use Cwd;

use AVIS::Local;
use AVIS::Logger;
use AVIS::Macro::Setup 2.0;
use AVIS::Macro::Command;
use AVIS::Emulator::SW95;
use AVIS::Emulator::Setup::SW95;
use AVIS::Emulator::Setup::Drives;


my $swincfgtmpl = "/data/Avis/Macro/Files/Config/Images/English/swincfg";
my $cmddir = "/data/Avis/Macro/Files/Commands";
my $progdir = "/data/Avis/Macro/Progs";

my $debug = 0;


my ($rcrundir, $rcfiledir, $rctmpdir, $rcsampledir, $rcgoatdir, $rcprogdir);
my ($rccmdfile, $rcgoatlist, $rcgeneralfile, $rcnlscfgfile, $rcappcfgfile, $rcvarfile);

my $isfnrcsession = "session.bat";
my $isfnrcappcfg  = "AppCfg.cfg";
my $isfnrccmds    = "commands.lst";
my $isfngoatlist  = "goats.lst";
my $isfngencfg    = "general.cfg";
my $isfnnlscfg    = "language.cfg";
my $isfnvarcfg    = "vars.lst";
my $isfnreplist   = "replic.lst";
my $isfntexec     = "texec.exe";
my $isfnrc        = "rc.exe";
my $isfnkillproc  = "killproc.exe";

my $isvalRCTout = 900;
my $isvalSWTout = 1200;


sub Replicate {
  my $app = shift;
  my $rls = shift;
  my $nls = shift;
  my $filename = shift;
  my $cmdslist = shift;
  my $resultsdir = shift;
  my $logger = shift;
  my $samplepath = shift if (@_);

  my $rc = 0;
#  mkpath($ndriveroot."/docs", 0, 0755)||warn "unable to mkpath $ndriveroot";

  my $emulation = new AVIS::Emulator::SW95();
  my $emulsetup = $emulation->Setup;
  #default values
  $emulsetup->Import($swincfgtmpl);
  
  #setup drives  
  my $drives = new AVIS::Emulator::Setup::Drives();
  if (not defined AVIS::Macro::Setup::SelectDriveSet($app, $rls, $nls, "b1", "SoftWindows95", $drives)) {
     my $msg = "Failed to select a drive set for ($app, $rls, $nls, b1, SoftWindows95): ".AVIS::Macro::Setup::GetLastError();
     $logger->logString($msg);
     return 0;
  }  
  $emulsetup->DriveList($drives);

  my $filedrive = $drives->GetFileDriveLetter();
  $rcrundir    = $filedrive.":\\Run";
  $rcfiledir   = $rcrundir."\\Files";
  $rctmpdir    = $rcrundir."\\Tmp";
  $rcgoatdir   = $rcrundir."\\Goats";
  $rcprogdir   = $rcrundir."\\Progs";
  $rccmdfile   = $rcfiledir."\\".$isfnrccmds;
  $rcgoatlist  = $rcfiledir."\\".$isfngoatlist;
  $rcgeneralfile = $rcfiledir."\\".$isfngencfg;
  $rcnlscfgfile  = $rcfiledir."\\".$isfnnlscfg;
  $rcappcfgfile = $rcfiledir."\\".$isfnrcappcfg;
  $rcvarfile     = $rcfiledir."\\".$isfnvarcfg;


  #put necessary things into image
  #directories: c:\run, c:\run\files, c:\run\sample, c:\run\goats, c:\run\tmp
  my $drivelist = $emulsetup->DriveList->ExtractAdapters();
  $drivelist->MkDir($rcrundir);
  $drivelist->MkDir($rcgoatdir);
  $drivelist->MkDir($rctmpdir);
  if ($drivelist->CopyTo($filename, $rctmpdir)) {
    my $msg = "Failed to copy $filename to $rctmpdir. Aborting.";
    $logger->logString($msg);
    $drives->Cleanup() unless $debug;
    return 0;
  }
  $drivelist->MkDir($rcfiledir);
  if ($drivelist->CopyTo($cmddir."/".$cmdslist, $rccmdfile)) {
     my $msg = "Failed to copy $cmddir/$cmdslist to $rccmdfile. Aborting.";
     $logger->logString($msg);
     $drives->Cleanup() unless $debug;
     return 0;
  }
  $drivelist->MkDir($rcprogdir);
  if ($drivelist->CopyTo($progdir."/*", $rcprogdir)) {
     my $msg = "Failed to copy $progdir/* to $rcprogdir. Aborting.";
     $logger->logString($msg);
     $drives->Cleanup() unless $debug;
     return 0;
  }
  
  #copy the config files needed by rc: general.cfg, goats.cfg, appcfg.cfg, language.cfg, command file
  
  my $goatdir = AVIS::Macro::Setup::GetGoatDir($app, $rls, $nls, "anybuild");
  if ($drivelist->CopyTo($goatdir."/*", $rcgoatdir)) {
    my $msg = "Failed to copy $goatdir/* to $rcgoatdir. Aborting.";
    $logger->logString($msg);
    $drives->Cleanup() unless $debug;
    return 0;
  }

  my $goatlist = AVIS::Macro::Setup::GetGoatList($app, $rls, $nls, "anybuild");
  if ($drivelist->CopyTo($goatlist, $rcgoatlist)) {
    my $msg = "Failed to copy $goatlist to $rcgoatlist. Aborting.";
    $drives->Cleanup() unless $debug;
    $logger->logString($msg);
    return 0;
  }

  my $generalfile = AVIS::Macro::Setup::GetGeneralFile($app, $rls, "anybuild");
  if ($drivelist->CopyTo($generalfile, $rcgeneralfile)) {
    my $msg = "Failed to copy $generalfile to $rcgeneralfile. Aborting.";
    $logger->logString($msg);
    $drives->Cleanup() unless $debug;
    return 0;
  }

  my $nlscfgfile = AVIS::Macro::Setup::GetNlsConfigFile($app, $rls, $nls, "anybuild");
  if ($drivelist->CopyTo($nlscfgfile, $rcnlscfgfile)) {
     my $msg = "Failed to copy $nlscfgfile to $rcnlscfgfile. Aborting.";
     $logger->logString($msg);
     $drives->Cleanup() unless $debug;
     return 0;
   }
  
  my $files2copy = AVIS::Macro::Setup::GetAppFilesToCopy($app, $rls, $nls, "anybuild");
  foreach (@$files2copy) {
    if ($drivelist->CopyTo($_->{'src'}, $_->{'dest'})) {
      my $msg = "Failed to copy ".$_->{'src'}." to ".$_->{'dest'}.". Aborting.";
      $logger->logString($msg);
      $drives->Cleanup() unless $debug;
      return 0;
    }
  }


  #copy appcfg.cfg to c:/run/files
  AVIS::Macro::Setup::CreateRCParameterFile($app, $rls, $nls, "anybuild", "SoftWindows95", $isfnrcappcfg) or warn "CreateParameterFile failed: ".AVIS::Macro::Setup::GetLastError();
  Dosify($isfnrcappcfg);
  if ($drivelist->CopyTo(cwd()."/".$isfnrcappcfg, $rcappcfgfile)) {
     my $msg = "Failed to copy ".cwd()."/".$isfnrcappcfg." to $rcappcfgfile. Aborting.";
     $logger->logString($msg);
     $drives->Cleanup() unless $debug;
     return 0;
  }

  #variables file
  (my $virfile = $filename) =~ s/.*\///;
  ($virfile = $rctmpdir."/".$virfile) =~ s/\//\\/g;
  AVIS::Macro::Setup::SetVariable('VIRUSPATH', $virfile);
  
  AVIS::Macro::Setup::SetVariable('GOATSDIR', $rcgoatdir);  #used?
  AVIS::Macro::Setup::SetVariable('RCGOATDIR', $rcgoatdir); #used?
  AVIS::Macro::Setup::SetVariable('RCPROGDIR', $rcprogdir); #general.cfg
  AVIS::Macro::Setup::SetVariable('RCFILEDIR', $rcfiledir); #used?

  AVIS::Macro::Setup::GetVariables($app, $rls, $nls, $cmdslist, $drives);
  AVIS::Macro::Setup::CreateVariableFile('vars.lst');
  Dosify("vars.lst");
  if ($drivelist->CopyTo("vars.lst", $rcfiledir)) {
     my $msg = "Failed to copy vars.lst to $rcfiledir. Aborting.";
     $logger->logString($msg);
     $drives->Cleanup() unless $debug;
     return 0;
  }

  #create the session file and copy it to the image
  my $sessionfile = AVIS::Macro::Setup::GetStartupFilePath($drives);
  CreateSessionFile($isfnrcsession);
  $drivelist->CopyTo(cwd()."/".$isfnrcsession, $sessionfile);

  #run the whole thing  
  my $startreptime = time();  
  my $emurc = $emulation->Run();
  my $reptime = time() - $startreptime;  

  if ($emurc == PROCESS_KILLED) {
    $main::SWstatus = 'Killed';
    my $msg = "Emulator was killed after $reptime seconds.";
    warn $msg; $logger->logString($msg);    
    $rc = 0;
  }
  else {
    $main::SWstatus = 'Finished';
    my $msg = "Emulator ran during $reptime seconds.";
    warn $msg; $logger->logString($msg);

    #postprocessing:
    #pickup the results
    rmtree($resultsdir, 0, 1);
    mkdir($resultsdir, 0755);
    $drivelist->CopyFrom("$rctmpdir/replic.lst", $resultsdir);
    $drivelist->CopyFrom("$rctmpdir/*.log", $resultsdir);
  
    #correct the replic.lst file created by RC
    #this file establishes the relationship between used copies and backup copies of the goats
    #when created by RC, the paths of the files are the DOS paths in the image
    my $replist = $resultsdir.'/'.$isfnreplist;
    if (-e $replist) {
      copy($replist, $replist.'.bak');
      Unixify($replist);
      my $replistcnt;
      my $backupdir = $goatdir;
      my $imggoatdir = $rcgoatdir;
      open(REPLIST, $replist);
      while(<REPLIST>) {
        if (m/^(.*?)\|(.*?)\|(.*?)\|(.*)\n/) {
	       my ($type, $dosbackup, $dosused, $filepath) = ($1, $2, $3, $4);
          my ($unixbackup, $unixused);
          if ($dosbackup =~ /\Q$imggoatdir\E/i) {
            ($unixbackup = $dosbackup) =~ s/.*\\(.*)/$backupdir\/$1/;
          }
          elsif ($dosbackup =~ /\Q$virfile\E/i) {
            $unixbackup = $filename;
          }
	       ($unixused = $dosused) =~ s/.*\\(.*)/$resultsdir\/$1/;
          $dosused =~ s/\\/\//g;
          $drivelist->CopyFrom($dosused, $unixused);
          $replistcnt .= "$type|$unixbackup|$unixused|$filepath\n";
        }
      }
      close(REPLIST);
      open(REPLIST, ">$replist");
      print REPLIST $replistcnt;
      close(REPLIST);
      $rc = 1;
    }
    else {
      my $errmsg = 'Unable to open the goat list';
      warn $errmsg; $logger->logString($errmsg);
      
      $rc = 0;
    }
  }
  
#  $drivelist->Cleanup() unless $debug;
  $drives->Cleanup() unless $debug;
  return $rc;
}



sub CreateSessionFile($) {
  my $sfile = shift;
      
  my $cmdline = $rcprogdir."\\$isfnrc -general $rcgeneralfile -goats $rcgoatlist -appcfg $rcappcfgfile -language $rcnlscfgfile -commands $rccmdfile -variables $rcvarfile -timeout 2000";

  open(SESSION, ">".$sfile)||warn "unable to open $sfile for output: $!";
  (my $tmpdirdrive = $rctmpdir) =~ s/^(\w:).*/$1/;
  my $sessioncmds = $tmpdirdrive."\n";
  $sessioncmds .=  "cd $rctmpdir\n";
  if (defined $main::date) {
    $sessioncmds .= "echo ".$main::date." >> c:\\date.txt\n";
    $sessioncmds .= "date < c:\\date.txt\n";
  }
  $sessioncmds .= $rcprogdir."\\$isfnkillproc -log\n";
  $sessioncmds .= $rcprogdir."\\$isfntexec -kill $isvalRCTout ".$cmdline."\n";
  $sessioncmds .= $rcprogdir."\\$isfnkillproc -delta\n";
  $sessioncmds .= $rcprogdir."\\$isfntexec -isleep 5000 -16bit c:\\insignia\\exitswin.com /s\nexit\n";
  $sessioncmds =~ s/\n/\r\n/g; #conversion to DOS textfile format
  print SESSION $sessioncmds;
  close SESSION;
}


sub Dosify {
  my $filename = shift;
  my $newfilename = @_ ? shift : $filename;
  
  my $contents = "";
  
  if (!open(FILE, $filename)) {
    warn "Dosify failed: unable to open $filename for input: $!";
    return 0;
  }
  binmode FILE; while(<FILE>) {$contents .= $_; } close FILE;
  
  $contents =~ s/\r\n/\n/g;
  $contents =~ s/\n/\r\n/g;  
  if (!open(FILE, ">".$newfilename)) {
    warn "Dosify failed: unable to open $newfilename for output: $!";
    return 0;
  }
  binmode FILE; print FILE $contents; close FILE;
  
  return 1;
}


sub Unixify {
  my $filename = shift;
  my $newfilename = @_ ? shift : $filename;
  
  my $contents = "";
  
  if (!open(FILE, $filename)) {
    warn "Dosify failed: unable to open $filename for input: $!";
    return 0;
  }
  binmode FILE; while(<FILE>) {$contents .= $_;} close FILE;
  
  $contents =~ s/\r\n/\n/g;  
  if (!open(FILE, ">".$newfilename)) {
    warn "Dosify failed: unable to open $newfilename for output: $!";
    return 0;
  }
  binmode FILE; print FILE $contents; close FILE;
  
  return 1;
}
