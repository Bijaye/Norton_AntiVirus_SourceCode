#!/usr/bin/perl
####################################################################
#                                                                  #
# Program name:  vDefTest.pm                                       #
#                                                                  #
# Description:   binary virus definition unit test.                #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Andy Raybould                                     #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
# Assumed directory layout:
# $UNC/$isdnSample                 - original sample
# $UNC/$isdnSigRes                 - sigres directory
# $UNC/$isdnVervRes                - verv files directory
# $UNC/$isdnRun/PID                - directory with replication information,
#                                    where PID is the $ProcessID
# $UNC/$isdnRun/PID/$isdnInfected  - directory containing samples
# $UNC/$isdnRun/PID/$isdnSectors
# $UNC/$isdnRun/PID/$isdnSample    - Sample on which the replication was based

#= package ====================================================================

package vDefTest;
require Exporter;
@ISA = qw(Exporter);

@EXPORT = qw(
   DefTest
);

@EXPORT_OK = @EXPORT;

#= Imports ====================================================================

use lib ('//Shine/TestZoneShine/perl/lib/site'); #!!testing only

use Cwd;
use Win32::Mutex;
use File::Path;
use File::Copy;
use Data::Dumper;
use AVIS::FSTools;
use AVIS::Dirstack;
use AVIS::Local;
use AVIS::Logger;
use vUnitTest;
use English;
use strict;

#= Declarations ===============================================================

#   DefTest ()
sub DefTest ();

#   PAMbuild ($virusName,$opcodes,$PAMflags,$controlFlags,$infoFlags,\@ERS,\@sigList)
sub PAMbuild ($$$$$\@\@);
sub builddefs ($$);

#   sub PAMinstallDefs ($installDir)
sub PAMinstallDefs ($);

#   PAMremoveDefs ($appid)
sub PAMremoveDefs ($);

sub getOpcodes(\@);
sub getPAMflags(\@);
sub getERS(\@);
sub getSigs(\@);
sub getInfoFlags(\@);
sub getControlFlags(\@);
sub getDefType(\@);

sub treeSync ($$);

sub PD ($);
sub run ($@);
sub startNwait ($@);
sub fail ($);
sub Log ($);
sub Xcopy ($$$);

sub min ($$);
sub max ($$);
sub minstr ($$);
sub maxstr ($$);
sub chompList (@);
sub C_hexForm($);

$::HOME                  =  cwd()     unless defined $::HOME;
$::USE_SUBPROCESSES      =  1         unless defined $::USE_SUBPROCESSES;
$::USE_DEBUG_UNIT_TEST   =  0         unless defined $::USE_DEBUG_UNIT_TEST;
$::DEFINE_NODUMP         = '';        ###'/DNODUMP' unless defined $::DEFINE_NODUMP;       # if defined, the ERS code will, on fail, write debug info

1;

#- overrides (for testing) ----------------------------------------------------

sub testInit () {
   $::DBG = '';

   $::srvRoot            = "//Elbe/ElbeD";        ###"//Mosel/srvroot/vanalysis";
   $::pgmRoot            = "$::srvRoot/auto";
   $::NAVdefRoot         = "$::srvRoot/NAV";

# temporary unit testing...
   $::NAV_DIR              = PD("$isdnROOT/VUT_NAV");

   $::VIRUS_DEF           = PD("$::NAV_DIR/PAM/virus.def");
   $::VIRUS_DEF_TEMPLATE  = "$::VIRUS_DEF.template";
   $::DEF2E_SRC           = PD("$::NAV_DIR/virscan/def2E.src");
   $::DEF2E_SRC_TEMPLATE  = "$::DEF2E_SRC.template";
   $::TestBinRepair       = "$::pgmRoot/TestBinRepair/TestBinRepair.pl";
   $::SYS_DRIVE           = $ENV{'SYSTEMDRIVE'};
fail "%SYSTEMDRIVE% not defined\n" unless $::SYS_DRIVE;

#???   $::TEST_HOLD           = PD("$::WorkUNC/testfiles") if defined $::WorkUNC;   # keep copies of the unit test files
}

#==============================================================================

sub DefTest () {
   my ($virusName, $VID, $deltaFile,                   $replicantsFile) =
      ('Autoverv','2EFF',PD "$::UNC/$isfnNAVdefDelta", PD "$::UNC/replicants.lst");

   testInit(); #!!!

   my $mutex = Win32::Mutex->new(0,'BlueICE-BinaryUnitTest');
   fail "Unable to open unit test mutex\n" unless $mutex;
   $mutex->wait();

   my @delta = `type $deltaFile`;
   map {s/<VirusID>/$VID/g; s/<VirusName>/$virusName/g} @delta;
   my $opcodes = getOpcodes(@delta);
   my $PAMflags = getPAMflags(@delta);
   my @ERS = getERS(@delta);
   my @SigList = getSigs(@delta);
   my $infoFlags = getInfoFlags(@delta);
   my $controlFlags = getControlFlags(@delta);

   treeSync ($::NAVdefRoot, $::NAV_DIR);   #??temporary unit testing

   my $installDir = PAMbuild($virusName,$opcodes,$PAMflags,$controlFlags,$infoFlags,@ERS,@SigList);
   my $appid = PAMinstallDefs($installDir);
   return '' unless $appid =~ m/^NAVSDK_\d+$/;

   my $testResult = UnitTest ($replicantsFile, $virusName);

   PAMremoveDefs ($appid);
   $mutex->release();
   return $testResult;
}

#==============================================================================

sub PAMbuild ($$$$$\@\@) {
   my ($virusName, $opcodes, $PAMflags, $controlFlags, $infoFlags, $pERS, $pSigList) = @_;
   my $installDir = "$::NAV_DIR\\update";

   fail "*** no base definitions in $installDir\n" unless -f "$installDir\\virscan2.dat";
   fail "*** no definst utility $::NAV_DIR\\tools\\definst.exe\n" unless -f "$::NAV_DIR\\tools\\definst.exe";
   Log "building PAM defs\n";
   Log `erase "$::NAV_DIR\\PAM\\$virusName.obj" "$::NAV_DIR\\PAM\\$virusName.dat"`;

   open (ASM, ">$::NAV_DIR\\PAM\\$virusName.asm") or fail "Unit Test: cannot create $virusName.asm\n";
   print ASM @$pERS;
   close ASM;

   # create a PAM signature      #PAMsigs!!
   fail "Cannot find $::VIRUS_DEF\n" unless -s $::VIRUS_DEF;

   my @virus_def = `type $::VIRUS_DEF_TEMPLATE`;
   open VIRUSDEF, ">$::VIRUS_DEF" or fail "Unable to open $::VIRUS_DEF for updating\n";

   for (my $line = shift @virus_def; defined $line; $line = shift @virus_def) {  #PAM!!
      last if $virus_def[0] =~ m/^\[SigStart\]/i;
      print VIRUSDEF $line;
      print VIRUSDEF "$PAMflags\n"  if $line =~ m/^\[Flags\]/i;
      print VIRUSDEF "$opcodes\n"   if $line =~ m/^\[OpcodeList\]/i;
   }

   foreach my $sig (@$pSigList) {
      foreach (@virus_def) {
         print VIRUSDEF;
         last if m/^\[String\]/i;
      }
      print VIRUSDEF $sig;
      print VIRUSDEF "[SigEnd]\n";   # assumes the string is the last signature sub-field in $::VIRUS_DEF
   }

   print VIRUSDEF "[End]\n";         # assumes the signature is the last group of the definition in $::VIRUS_DEF
   close VIRUSDEF;
   system "type $::VIRUS_DEF" if $::VERBOSE;


   my @def2E_src = `type $::DEF2E_SRC`;
   open DEF2E, ">$::DEF2E_SRC" or fail "unable to open $::DEF2E_SRC for updating\n";

   for (my $line = shift @def2E_src; defined $line; $line = shift @def2E_src) { #PAM!!
      if     ($line =~ m/^\Q[Control_Flags]/i) {
                print DEF2E "[Control_Flags] = $controlFlags\n";
      }elsif ($line =~ m/^\Q[Info_Flags]/i) {
                print DEF2E "[Info_Flags] = $infoFlags\n";
      }else  {
                print DEF2E $line;
      }
   }
   close DEf2E;
   system "type $::DEF2E_SRC" if $::VERBOSE;


   builddefs ($virusName,"$::NAV_DIR");
   Log (`copy "$::NAV_DIR\\packages\\virscan2.dat" "$installDir\\virscan2.dat"`);
   Log ("PAM defs built...\n");

   if ($::USE_DEBUG_UNIT_TEST) {   # build the debugging PAM simulator
      open (ERS, ">$::NAV_DIR\\scaffolding\\$virusName.ers") or fail "Unit Test: cannot create ERS file\n";
      my $inInstr = 0;
      foreach (@$pERS) {
         $inInstr = 0 if m/InstrEnd:/;
         print ERS if $inInstr;
         $inInstr = 1 if m/Instructions:/;
      }
      close ERS;
      Log `del "$::NAV_DIR\\scaffolding\\pam.exe"`;
      my $RC = startNwait ("$::NAV_DIR\\scaffolding\\bld16.bat", "\"$::NAV_DIR\\scaffolding\"");
      Log `type \"$::NAV_DIR\\scaffolding\\log\"`;
      fail "debug unit test build failed\n" unless -s "$::NAV_DIR\\scaffolding\\pam.exe";
   }

   return $installDir;
}


sub builddefs ($$) {
   my ($virusName,$NAVroot) = @_;
   $ENV{'INCLUDE'} = "$NAVroot\\include;$NAVroot\\include\\dx";
   $ENV{'LIB'} = "$NAVroot\\lib;$NAVroot\\lib\\dx";
   $ENV{'QUAKEINC'} = "$NAVroot\\NAVEX";
   $ENV{'SARCBOT'} = "1";
   $ENV{'PLATFORM'} = ".DX";

   pushdir("$NAVroot\\PAM");
   unlink ("found.obj","found.exe",
           "found.dat","$virusName.obj","$virusName.exe","$virusName.dat",
           "prepair.dat","virscan2.dat",
           "sigs.dat","vsigs.dat");

   startNwait "..\\progs\\PAMCOMP", "virus.def exclude.dat sigs.dat";
   startNwait "..\\progs\\PAMSORT", "sigs.dat";
   startNwait "..\\progs\\FIXINDEX", "index.dat sigs.dat";
   print `copy index.dat /B + sigs.dat /B vsigs.dat`;

   startNwait "..\\progs\\ml", "/c $::DEFINE_NODUMP found.asm";
   startNwait "..\\progs\\link", "found.obj,,,,,";
   startNwait "..\\progs\\cexe2bin", "found.exe found.dat";

   startNwait "..\\progs\\ml", "/c $::DEFINE_NODUMP $virusName.asm";
   startNwait "..\\progs\\link", "$virusName.obj,,,,,";
   startNwait "..\\progs\\cexe2bin", "$virusName.exe $virusName.dat";

   startNwait "..\\progs\\rcomp",  "prepair.def prepair.dat";
   startNwait "..\\progs\\builddat", " build.def virscan2.dat version.def";

   popdir();
   pushdir ("$NAVroot\\packages");
   unlink ("virscan.dat","virscan2.pam","virscan2.dat");
   Xcopy ('',"..\\virscan\\virscan.dat","virscan.dat");
   Xcopy ('',"..\\PAM\\virscan2.dat","virscan2.pam");

   startNwait "..\\progs\\datgen", "virscan.dat";
   startNwait "..\\progs\\builddat", "..\\progs\\virscan2.def virscan2.dat ..\\progs\\version.dat";

   unlink (
      "boolist.dat",
      "claimed.dat",
      "algitw.dat",
      "algzoo.dat",
      "boo.dat",
      "crc.dat",
      "mem.dat",
      "names.dat",
      "namesidx.dat",
      "virinfo.dat",
      "virscan.a!!",
      "virscan.alg",
      "virscan.c!!",
      "virscan.crc",
      "virscan.m!!",
      "virscan.mlg",
      "virscan.mrc",
      "virscan.mpg",
      "virscan.p!!",
      "virscan.plg",
      "virscan.dat");

   startNwait "..\\progs\\glue", "virscan2.dat virscan2.pam virscan2.out";
   Xcopy ('',"virscan2.out","virscan2.dat");

   unlink ("virscan2.pam","virscan2.out");
   popdir();
}



sub PAMinstallDefs ($) {
   my ($installDir) = @_;
   my $appid = '';

   PAMremoveDefs($ENV{"NAVSDK_APPID"}) if defined $ENV{"NAVSDK_APPID"} and $ENV{"NAVSDK_APPID"} =~ m/^NAVSDK_\d+$/;
   run ('rd', '/s /q "'.$::SYS_DRIVE.'\\Program Files\\Common Files\\Symantec Shared\\VirusDefs"');

   startNwait "$::NAV_DIR\\tools\\definst.exe", '"'.$installDir.'"'; #??foreach (`$::NAV_DIR\\tools\\definst.exe "$::NAV_DIR\\update"`) { Log ($_);
   foreach(@::LOG) {
      $appid = $ENV{"NAVSDK_APPID"} = $1 if (m/(NAVSDK_\d+)/);
   }

   Log ("PAM defs installed with app. ID $appid\n");
   return $appid;
}



sub PAMremoveDefs ($) {
   my ($appid) = @_;
   Log `$::NAV_DIR\\tools\\definst.exe /U:$appid`;
}


sub getOpcodes (\@) {
   my ($pDelta) = @_;
   my $opcodes;

   my $inOpcodes = 0;
   foreach(@$pDelta) {
      $inOpcodes = 0 if m/\Q[OpcodeListEnd]/;
      $opcodes .= $_ if $inOpcodes;
      $inOpcodes = 1 if m/\Q[OpcodeList]/;
   }
   return $opcodes;
}


sub getPAMflags (\@) {
   my ($pDelta) = @_;

   foreach(@$pDelta) {
      return $1 if m/^\[PAM_FLAGS\]\s*(.*)$/i;
   }
   fail "PAM_FLAGS not found\n";
}


sub getERS (\@) {
   my ($pDelta) = @_;
   my @ERS;

   my $inERS = 0;
   foreach(@$pDelta) {
      $inERS = 0 if m/\Q[ERSCODEEND]/;
      push @ERS, $_ if $inERS;
      $inERS = 1 if m/\Q[ERSCODESTART]/;
   }
   return @ERS;
}


sub getSigs (\@) {
   my ($pDelta) = @_;
   my @Sigs;

   my $inSigs = 0;
   foreach(@$pDelta) {
      $inSigs = 0 if m/\Q[STRINGEND]/;
      push @Sigs, $_ if $inSigs;
      $inSigs = 1 if m/\Q[STRINGSTART]/;
   }
   return @Sigs;
}



sub getInfoFlags (\@) {
   my ($pDelta) = @_;
   foreach(@$pDelta) {return $1 if m/^\[Info_Flags\]\s*=\s*(.*)/};
   fail "no info flags\n";
}

sub getControlFlags (\@) {
   my ($pDelta) = @_;
   foreach(@$pDelta) {return $1 if m/^\[Control_Flags\]\s*=\s*(.*)/};
   fail "no control flags\n";
}


sub getDefType (\@) {
   my ($pDelta) = @_;
   my $flag = getControlFlags(@$pDelta);
   return 'repair' if $flag =~ m/PAM_REPAIR/;
   return 'detect' if $flag =~ m/NO_CLEAN/;
   fail "control flag $flag does not specify definition type\n";
}


sub treeSync ($$) {
   my ($src,$dest) = @_;
   Log "treeSync $src > $dest\n"; #??
   foreach (chompList (Xcopy('/s/D/l/h', $dest, $src))) {
      unlink unless m/\d+\s+File\(s\)/;
   }
   Xcopy ('/s/D/h/z', "$src/*", "$dest/*");
   system "attrib -a /s ".PD("$dest/*");
}

#==============================================================================

sub PD ($) {
   my ($path) = @_;
   if ($^O =~ m/win/i) {$path =~ tr[/][\\];}
   else                {$path =~ tr[\\][/];}
   return $path;
}


sub run ($@) {
   my ($pgm, @args) = @_;
   my $cmd = PD($pgm)." @args";
   Log cwd().">\n   RUN $cmd\n";
   system $cmd;                        #?? Log `$cmd`; return ($? >>8);
}


sub startNwait ($@) {
   my ($pgm, @args) = @_;

   my $RC;
   my $cmd = PD($pgm)." @args";
   Log cwd().">\n   START $cmd\n";

   if (!$::USE_SUBPROCESSES) {
      @::LOG = `$cmd`;
      $RC = $? >> 8;
   }else {
      if (defined $::RPID) {$::RPID++;} else {$::RPID = 0;}

      my $BATname = "$::WKDIR\\RPID$::RPID.BAT";
      open BAT, ">$BATname"                                                                        or return (@::LOG = `$cmd`, $? >> 8);
      print BAT "$cmd >$::WKDIR\\RPID$::RPID.LOG\necho %errorlevel% >$::WKDIR\\RPID$::RPID.RC\n"   or return (@::LOG = `$cmd`, $? >> 8);
      close BAT;

      system 'start "Vanalysis" /WAIT /MIN cmd /c '.$BATname;

      @::LOG = `type $::WKDIR\\RPID$::RPID.LOG`;
      ($RC = (`type $::WKDIR\\RPID$::RPID.RC`)[0]) =~ s/\D//g;
      unlink glob "$::WKDIR\\RPID$::RPID.*";
   }
   print @::LOG if $::VERBOSE;
   return $RC;
}


sub fail ($) {
   topdir();
   Log "FAIL: @_";
   exit 8;
}


sub Log ($) {
   print "@_\n";
}


sub Xcopy ($$$) {
   my ($opt,$src,$dest) = ($_[0],PD($_[1]),PD($_[2]));
   my $destType = ($dest =~ m<[\*\?][^\//]*$>) ? 'd' : 'f';
  `echo $destType | xcopy $opt "$src" "$dest"`;
}


sub min ($$) {return ($_[0] < $_[1] ? $_[0] : $_[1]);}

sub max ($$) {return ($_[0] > $_[1] ? $_[0] : $_[1]);}

sub minstr ($$) {return ($_[0] lt $_[1] ? $_[0] : $_[1]);}

sub maxstr ($$) {return ($_[0] gt $_[1] ? $_[0] : $_[1]);}

sub chompList (@) {
   foreach (@_) {chomp;}
   return @_;
}

sub C_hexForm($) {
   my ($sig) = @_;
   $sig =~ s/([0-9A-F]{2})/0x$1/ig;
   $sig =~ s/([0-9A-F]{2})(0x)/$1,$2/ig;
   return $sig;
}

