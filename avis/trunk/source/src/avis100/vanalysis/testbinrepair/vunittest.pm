use AVIS::Local;
#!/usr/bin/perl
####################################################################
#                                                                  #
# Program name:  vUnitTest.pm                                      #
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

package vUnitTest;
require Exporter;
@ISA = qw(Exporter);

@EXPORT = qw(
   UnitTest
);

@EXPORT_OK = @EXPORT;

#= Imports ====================================================================

use lib ('//Shine/TestZoneShine/perl/lib/site'); #!!testing only

use Cwd;
use AVIS::Local;
use AVIS::Vides;
use File::Copy;
use Getopt::Long;
use strict;

#= Declarations ===============================================================

#   UnitTest ($replicantsList,$virusName);
sub UnitTest ($$);

#   scanSample ($sample, $virusName, $tryRepair);
sub scanSample ($$$);

#   testSample ($Sample, $virusName, $tryRepair, $Goat)
sub testSample ($$$$);

sub isRepairOf ($$);
sub isExactRepairOf ($$);
sub isInexactRepairOf ($$);
sub _readEXEHeader ($);
sub _writeEXEHeader ($@);
sub _EXElenfrommoddiv ($$);
sub copySample($);

sub PD ($);
sub run ($@);
sub startNwait ($@);
sub Log ($);

sub min ($$);
sub max ($$);
sub minstr ($$);
sub maxstr ($$);

$::USE_SUBPROCESSES      = 1        unless defined $::USE_SUBPROCESSES;
$::VERBOSE               = 1        unless defined $::VERBOSE;

my $compprog             = "compare.exe";
my $writeheaderprog      = "writeheader.exe";
my $truncprog            = "truncate.exe";

1;

#==============================================================================

sub UnitTest ($$) {
   my ($replicantsList,$virusName) = @_;
   my ($n_tested, $n_detected, $n_repaired, $n_inexact, $n_bad_repairs) =
      (0,         0,           0,           0,          0             );

   my ($defType, @replicants);
   die "replicants list $replicantsList not found or empty\n" unless -s $replicantsList;
   eval `type $replicantsList`;
   die "UnitTest was unable to get the replicants list from $replicantsList\n" unless defined @replicants;
   my $tryRepair = ($defType eq 'repair');

   my $originalSample = (shift @replicants)->{infected};
   my $originalTestSample = copySample($originalSample);
   my $originalResult = scanSample($originalTestSample,$virusName,$tryRepair);
   my $originalSampleOK = ($originalResult eq 'detected' or $originalResult eq 'repaired');   #?? $defType.'ed');
   unlink $originalTestSample;

   if ($::VERBOSE and !$originalSampleOK) {print "FAIL: cannot $defType original sample $originalSample\n";}

   # continue regardless, so as to see how good the defs are
   foreach (@replicants) {
      my ($infected, $goat) = (PD $$_{infected}, PD $$_{goat});
      next if $infected =~ m</sectors/>i or $goat =~ m</none$>i;

      my $sample = copySample($infected);
      my $result = testSample ($sample, $virusName, $tryRepair, $goat);
      unlink $sample;
      $n_tested++;
      if ($::VERBOSE) {print "$result\n";}

      if ($result eq 'detected'or ($result eq 'repaired' and !$tryRepair)) {
         $n_detected++;
      }elsif ($result eq 'repaired (exact)') {
         $n_detected++;
         $n_repaired++;
      }elsif ($result eq 'repaired (inexact)') {
         $n_detected++;
         $n_repaired++;
         $n_inexact++;
      }elsif ($result eq 'bad repair') {
         $n_detected++;
         $n_bad_repairs++;
      }elsif ($::VERBOSE) {
         print "? $result\n";
      }
   }

if (defined $ENV{TEST_FALSE_POSITIVES} and defined $ENV{SYSTEMROOT}) {
   my $nFalsePositives = 0;
   foreach (`dir /s/b/a-d $ENV{SYSTEMROOT}\\*.exe`) {
      chomp;
      my $sample = copySample($_);
      my $result = testSample ($sample, $virusName, 0, '');
      $nFalsePositives++ unless $result eq 'clean';
      print (($result eq 'clean' ? "    " : "*** ")."$_ $result\n");
      unlink $sample;
   }
   print ("\n".('=' x 80)."\n$nFalsePositives FALSE POSITIVES!!!\n") if $nFalsePositives;
   print ("\n".('=' x 80)."\nno false positives\n") unless $nFalsePositives;
}else {print ("\n".('=' x 80)."\nSet TEST_FALSE_POSITIVES (and SYSTEMROOT) to test for false positives\n");}

   if ($::VERBOSE) {print (('=' x 80)."\nUnit test of $n_tested samples:\n".
      "   detected: $n_detected".
      "   repaired: $n_repaired ($n_inexact inexact)".
      "   bad repairs: $n_bad_repairs".
      "   not detected: ".($n_tested - $n_detected)."\n");
   }

   # OK if some not repaired, so long as no bad repairs?
   my $result = 'FAIL';
   $result = 'OK' if  ($originalSampleOK or $ENV{SKIP_ORIGINAL_SAMPLE})
                  and ($n_detected == $n_tested)
                  and ($n_repaired == $n_tested or !$tryRepair);
   if ($::VERBOSE) {print "Def test: $result\n".('=' x 80)."\n";}
   return $result;
}

#==============================================================================

sub testSample ($$$$) {
   my ($Sample,$virusName,$tryRepair,$Goat) = @_;
   my $result = scanSample ($Sample,$virusName,$tryRepair);
   return $result unless $tryRepair and $result eq 'repaired';
   if ($::VERBOSE) {map {print if $_} (`fc /B "$Sample" "$Goat"`)[1..40];}
   return isRepairOf ($Sample, $Goat);
}



sub scanSample ($$$) {
   my ($Sample,$virusName,$tryRepair) = @_;

   my $RC = startNwait "$::NAV_DIR\\tools\\deftest.exe", ($tryRepair ? '-r' : '') ." \"$Sample\"";
   return 'repaired' if $::LOG[-1] =~ m/infected with $virusName; repaired/;
   return 'detected' if $::LOG[-1] =~ m/infected with $virusName/;
   return 'clean'    if $::LOG[-1] =~ m/clean/;

   return  "ERR: unit test detected a different virus ($1)!\n" if $::LOG[-1] =~ m/infected with ([^;]*)/;
   return  "ERR: unexpected unit test result\n@::LOG\n";
}



sub isRepairOf ($$) {
  my ($repaired, $original) = @_;
  return "repaired (exact)"   if isExactRepairOf   ($repaired, $original);
  return "repaired (inexact)" if isInexactRepairOf ($repaired, $original);
  return "bad repair";
}


sub isExactRepairOf ($$) {
  my ($repaired, $original) = @_;
  my $retval = system "$compprog $original $repaired >nul";
  return $retval == 0;
}


sub isInexactRepairOf ($$) {
  my ($repaired, $original) = @_;
  my $len = -s $original;
  my $rlen = -s $repaired;
  my (  $sig1, $sig2,
        $mod512, $div512,
        $relocnum, $hsize,
        $min, $max,
        $SS, $SP,
        $checksum,
        $IP, $CS,
        $relocofs, $ovlnum
        ) = _readEXEHeader ($original);
  my (  $rsig1, $rsig2,
        $rmod512, $rdiv512,
        $rrelocnum, $rhsize,
        $rmin, $rmax,
        $rSS, $rSP,
        $rchecksum,
        $rIP, $rCS,
        $rrelocofs, $rovlnum
        ) = _readEXEHeader ($repaired);

  my $sig   = pack ("CC", $sig1, $sig2);
  my $rsig  = pack ("CC",$rsig1,$rsig2);
  my $lenfromheader  = _EXElenfrommoddiv( $mod512,  $div512);
  my $rlenfromheader = _EXElenfrommoddiv($rmod512, $rdiv512);

  if (($sig eq "MZ" or $sig eq "ZM") and ($rsig eq "MZ" or $rsig eq "ZM")) {

     if (  $len == $lenfromheader &&       # No overlay in original file
           $rlenfromheader >= $len &&      # Enough bytes loaded by DOS from the repaired file
           $rlenfromheader <= $rlen &&     # Bytes indicated in header do exist
           $rlen >= $len                   # Repaired file at least as long as original
        ) {

        # We write the header even in the equality case to fix the checksum anyway
        # This could be a good inexact repair, we fix the checksum, the sig and truncate the repaired file

        # write a new header
        _writeEXEHeader($repaired,
                        $sig1, $sig2,
                        $mod512, $div512,
                        $rrelocnum, $rhsize,
                        $rmin, $rmax,
                        $rSS, $rSP,
                        $checksum,
                        $rIP, $rCS,
                        $rrelocofs, $rovlnum);

        # truncate
        system "$truncprog $repaired $len >nul";
        # Finally redo a comparison
        return isExactRepairOf($repaired, $original);
    }
    else {
        # Length conditions are no good
        return 0;
    }
  }
  else {
    # The file is not an EXE
    return 0;
  }
}


sub _readEXEHeader ($) {
   my $file = shift;
   my $headerbuffer = "";
   open (FILE, $file);
   binmode FILE;
   my $n = read FILE, $headerbuffer, 28; print "TestBinRepair::_readEXEHeader read only $n bytes from $file\n" unless $n == 28;
   close (FILE);
   return unpack ("C2S13", $headerbuffer);
}


sub _writeEXEHeader ($@) {
   my ($file, @headerfields) = @_;
   my $headerbuffer;
   my $tmpheadfile = "_wehtmp$$";

   $headerbuffer = pack ("C2S13", @headerfields);
   open (TMPTMP, ">".$tmpheadfile);
   binmode TMPTMP;
   print TMPTMP $headerbuffer;
   close (TMPTMP);
   system "$writeheaderprog $tmpheadfile $file";
   unlink $tmpheadfile;
}


sub _EXElenfrommoddiv ($$) {
  my ($mod512, $div512) = @_;
  return 512 * $div512 + ($mod512 == 0 ? 0 : $mod512 - 512);
}


sub copySample ($) {
   my ($infected) = @_;
   if ($::VERBOSE) {print (('=' x 80)."\n$infected\n");}
   my $wkdir = $ENV{TEMP} ? $ENV{TEMP} : $ENV{TMP} ? $ENV{TMP} : '.';
   my $sample = PD "$wkdir/AVISvSample.tmp";
   copy ($infected, $sample);
   return (-s $sample == -s $infected ? $sample : "ERR: did not copy $infected to $sample\n");
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


sub Log ($) {
   print "@_\n" if ($::VERBOSE);
}


sub min ($$) {return ($_[0] < $_[1] ? $_[0] : $_[1]);}

sub max ($$) {return ($_[0] > $_[1] ? $_[0] : $_[1]);}

sub minstr ($$) {return ($_[0] lt $_[1] ? $_[0] : $_[1]);}

sub maxstr ($$) {return ($_[0] gt $_[1] ? $_[0] : $_[1]);}

