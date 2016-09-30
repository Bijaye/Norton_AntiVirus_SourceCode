#!/usr/bin/perl

# ifdef TEST
# Anything marked this way is additional code needed to work with the standalone
# environment and regression-test suite currently on Shine. A Perl 1-liner to
# put a newline after that phrase readies this script for that environment.

# ifdef TEST if (-f '//shine/TestZoneShine/analysis/test/vana.pl') {  #!!
# ifdef TEST    exit system 'perl '.PD('//shine/TestZoneShine/analysis/test/vana.pl')." @ARGV";
# ifdef TEST }
# ifdef TEST
# ifdef TEST do "./vanalysis_test.pl" if -f "./vanalysis_test.pl"; #!!
# ifdef TEST $isdnCorpusDB = '//Mosel/CorpusDB';
# ifdef TEST
# ifdef TEST use strict;
# ifdef TEST use lib ('//Shine/TestZoneShine/analysis/test'); #!!testing only
# ifdef TEST use vDefTest;

####################################################################
#                                                                  #
# Program name:  vanalysis.pl                                      #
#                                                                  #
# Description:   binary virus analysis.                            #
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

# This script is invoked by the scheduler if the binary replicator has been
# successful, and performs the following actions:
#     Static analysis:
#        Codow: code-data segregation.
#        Autoverv: host-host & host-infected analysis, as in patent 5485575.
#        Find the candidate signatures within the 'page' containing the entry point.
#     If no signature found, perform dynamic analysis (emulation).
#     Choose one or more signatures.
#     Construct a delta file.

# Notes:

# codow: The output is used for two purposes in static analysis:
#     Matched sections should be either all code or all data.
#     Signatures should come from code sections.
#
# This program is not reliable, so its output is tested for usefulness, and it
# is ignored if it doesn't look good.

# Autoverv: There are a couple of enhancements beyond that specified in the patent:
#     When matching within an EXE header, which has a defined structure of multi-
#     byte fields, matches must begin and end on a field bundary.
#
#     Matched sections should be either all code or all data (see codow above).
#
#     When a virus attaches by other methods than simply appending, the pattern of
#     infection may depend on the relative sizes of virus and host. I autoverv
#     can't find a consistent pattern covering all the samples, and if it has
#     determined that the viral code is of the same length, it divides the
#     sample set according to whether the host is longer or shorter than the
#     virus, and attempts to solve for both groups independently.
#
#     When seeking a best anchor point for a set of matches, and no candidate
#     solution has zero range, candidates for which the adjustment always aligns
#     the match on a power-of-two boundary are preferred.
#
#     The program now emits repair instructions in the form of a list of macro
#     instantiations that will be assembled into a definition that uses the
#     immune system extension to the PAM/ERS foundation class. See autoverv.inc
#     and autoverv.dcl for information about the ERS repair macro format.

# Candidate signature generation:
#     The first attempt to find a signature looks in any constant, unencrypted
#     viral sections close enough to the entry point that PAM is guaranteed to
#     scan them (see the PAM documentation). If Symantec ever include a string
#     scanner, a more thorough search could be made.
#
#     In every other case, including when Autoverv has been completely unsuccessful
#     (e.g. with polymorphics), emulation is tried. An instrumented PAM emulator
#     emits each candidate signature within each of the sections scanned, every
#     time PAM does a scan.

# All candidate signatures are tested in the emulator on every sample, regardless
# of whether they were derived statically or dynamically. In addition to verifying
# them, this creates an opcode map, indicating which opcodes were used up to the
# point where the signature was found, and the number of instructions taken to find
# the signature.

# Signature Selection is based on the following principles:
#     Signatures must score at or above the defined threshold ($::SIG_WORST_SCORE).
#
#     A signature must cover at least all COM or all EXE files, if not both types. A
#     first pass attempts to find a solution using those signatures (if any) that
#     are found in all samples; the second pass seeks to find a significantly better
#     solution when the types are considered independently.
#
#     Signatures that are found quickly are preferred.
#
#     If a pair of signatures are found which together lead to an aggregate time-
#     to-detection for every sample, of the type being considered, that is better
#     than that achieved by any single signature, then the pair are preferred. This
#     may occur if a polymorphic virus may begin decryption from either end.

# Definition generation:
#     The script bindefpak.pl contains a template delta file, which it fills in
#     with its arguments.
#
#     The opcode map is the union of all the samples' maps at the point where they
#     are detected by PAM.
#
#     The iteration count limit is set by rounding up, to the next avaliable limit,
#     the maximum number of iterations taken to detect any of the samples multiplied
#     by a fudge factor ($::ITERATION_MARGIN).
#
#     It's OK to use emulation-generated detection with statically-generated repair
#     (no attempt has been made to generate repair via emulation for DOS viruses).

# Global variable names are all uppercase, and the $::NAME syntax is use to make it
# clear that they are globals. Once initialized, they are not expected to change.

#= Data structures ============================================================
#
# The structure of the UNC tree should be documented elsewhere (AVIS::local?)
#
# Replicants list:
#  An array of {
#     infected =>  name of an infected file
#     goat     =>  name of the corresponding goat, or '' for the original sample
#     Itype    =>  infected file type (COM or EXE)
#     Gtype    =>  goat file type (COM or EXE)
#     original =>  this is only used in the original sample entry, and it's the
#                  name given to it by the infrastructure. This is part of a
#                  workaround for the fact that it was originally sample.<type>,
#                  but is currently original.sample
#   };
#   In practice, the original sample is always the first entry. I don't think anything
#   assumes that, but I haven't tested or proved that to be the case.
#
# Attribute hash: this describes the sample-set.
#     count    => number of replicants
#     EXE      => number of EXE replicants
#     COM      => number of COM replicants
#     length   => minimum of the viral lengths, as reported by the replicator
#
#= Functions ==================================================================

#   Vanalysis ($UNC,$Cookie,$ProcessID)    - the main analysis routine
sub Vanalysis ($$$);
sub Vanalysis_cleanup ();             #    - remove work files etc.

#   get_original_sample ($srcDir,\@replicants)
#   - finds where it is and adds it to the replicants list
sub get_original_sample ($\@);

#   parse_replicants_list ($UNC,$Cookie,$GoatRoot,\@replicants, \%attributes)
#   - adds all the replicated sample - goat pairs to the replicants list
sub parse_replicants_list ($$$\@\%);

#   code_data_segregation (\@replicants)   - applied to a list of replicants
sub code_data_segregation (\@);

#   Codo ($FilePath) - code-data segregation applied to a single file
sub Codo ($);

#   Autoverv (@replicants,$optionalArgs) - static analysis
sub Autoverv (\@$);

#   RemoveExcludedSamples ($ExclusionListFile, \@replicants)
#   - autoverv may exclude some samples after a dot-product test. This function
#     removes them from the replicants list. Only a minority of samples may be
#     excluded.
sub RemoveExcludedSamples ($\@);

#   compressERS ($inFile, $outFile)
#   - a placeholder for the substitution of a compressed form of the ERS
#     instructions (not yet implemented).
sub compressERS ($$);

#   mergeERS (@instructions)
#   - If the same operation is generated for both COM and EXE files, they are
#     replaced by an all-files version.
sub mergeERS (@);

#   getStaticSigs ($pReplicants,$PAMmapFile,$ExecutionLimit)
#   - extract candidate signatures from static viral sections near to the entry point.
sub getStaticSigs (\@$$);

#   getEmulationSigs($pReplicants);
#   - emulate and parse the resulting log for candidate signatures.
sub getEmulationSigs(\@);

#   getSignatureSet ($pCandidates, $pReplicants)
#   - choose a signature-set from the candidates.
sub getSignatureSet (\@\@);

#   getCriticalLength ($ERSfile)
#   - if Autoverv found a length-dependent solution, find the threshold sample
#     length that differentiates between the two forms of the solution.
sub getCriticalLength ($);

#   setLengthDependency(@replicants, $lCritical);
#   - if there is a length dependency, mark each of the samples as short or long.
sub setLengthDependency(\@$);

#   testCodata (@replicants)
#   - sanity check on the codow output
sub testCodata (\@);

#   MakeDefs ($UNC,$ERSfile,\@PAMdefs,$testResult,\%attributes)
#   - main definition-building routine
sub MakeDefs ($$\@$\%);

#   fileType ($fileName) - COM or EXE?
sub fileType ($);

#   ORopcodes(x,y) - forms the union of opcode maps x and y.
sub ORopcodes($$);

#   getParameters - from the command line
sub getParameters();

#   startLog ($LogRoot)
sub startLog($);

#   printBanner ();
sub printBanner ();

#   mkSubdir ($parent,$child)
sub mkSubdir ($$);

#   Xcopy ($opt,$src,$dest)
sub Xcopy ($$$);

#   testPgmExists (@pgm)
sub testPgmExists (@);

#   testFileNotEmpty (@dir)
sub testFileNotEmpty (@);

#   writeReplicantList ($defType, @replicants, $file)
#   - tells unit test where the files are, and whether it's a detect or repair definition
sub writeReplicantList ($\@$);

#   run ($pgm, @args)
sub run ($@);

#   startNwait ($pgm,@args)
#   - if $::USE_SUBPROCESSES is set, runs the pgm with the given args in a separate
#     process, waiting for it to finish.
sub startNwait ($@);

#   PD ($path) - change path delimiters into those accepted by the current OS
sub PD ($);

#   fail ($message)
sub fail ($);

#   finish ($result)
sub finish ($);

sub min ($$);
sub max ($$);
sub minstr ($$);
sub maxstr ($$);
sub chompList (@);
sub Log ($);
sub C_hexForm($); # - if input is H1H2..., where Hn are hex digits, returns 0xH1,0xH2...

sub makeNewWorkDir ();

#= Imports ====================================================================

use Cwd;
use Getopt::Long;
use File::Path;
use File::Copy;
use Data::Dumper;
use AVIS::Vides;
use AVIS::FSTools;
use AVIS::Dirstack;
use AVIS::Local;
use AVIS::Logger;
use DataFlow;
use English;
use strict;

#= Initialization =============================================================

$::NEXT_QUEUE   = $isqnDefer;
$::STARTTIME    = time;
getParameters();
startLog($::UNC);
printBanner();
makeNewWorkDir();

#- Default parameters ---------------------------------------------------------

$::Codo                 = "$isdnBinDir/codow.exe";
$::Autoverv             = "$isdnBinDir/autoverv.exe";
$::Score                = "$isdnBinDir/DBscore.exe";
$::Bindefpak            = "$isdnBinDir/Bindefpak.pl";
$::PAMemulate           = "$isdnBinDir/PAMemulate.exe";

$::ERS_INSTRUCTIONS     = "autoverv.ers";
$::INFECTION_MAP        = "autoverv.PAMbytes";
$::CORPUS_DB            = PD("$isdnCorpusDB/");

$::CODO_TIMEOUT         =  20;             # code-data segregation timeout (seconds).
$::MIN_REPLICANTS       =  8;              # halt if the replicator has supplied fewer replicants

$::SIG_WORST_SCORE      = -80;             # Any signature must be at least this good.
$::MAX_SIG_OFFSET       = 512;             # A non-emulation (i.e. autoverv) signature must be
                                           # within this offset beyond the entry point.
$::ITERATION_MARGIN     = 1.3;             # set STOP this much above the max. iteration count
                                           # needed to detect any of the samples.

# the following may already have been set by the debug module, hence the 'unless defined'

# if true then programs and commands will be STARTed in their own DOS box.
$::USE_SUBPROCESSES    =  1 unless defined $::USE_SUBPROCESSES;

$::VERBOSE             =  0 unless defined $::VERBOSE;
$::KEEP_WORK_DIR       =  0 unless defined $::KEEP_WORK_DIR;  # - for post-run analysis.
$::DBG                 = '' unless defined $::DBG;            # perl debug flag, passed to subscripts.

#=== main =====================================================================

pushdir($::WKDIR);

$::WorkUNC = "$::WKDIR\\UNC";
Xcopy ('/s',"$::UNC\\*","$::WorkUNC\\*");
my $result = Vanalysis($::WorkUNC,$::COOKIE,$::PID);
popdir();
finish ($result);

#==============================================================================

sub Vanalysis ($$$) {
   my ($UNC,$Cookie,$ProcessID) = @_;
   my @replicants;       # infected files and their goats; [0] is the original sample
   my %attributes;       # info. for building a vides.results file
   my $defType;          # detect-only or repair also?
   my $rc;               # return codes

# check that necessary files are there.

   testPgmExists ($::Codo,
                  $::Autoverv,
                  $::Score,
                  $::Bindefpak,
                  $::PAMemulate);

   fail "No corpus DB ($isdnCorpusDB) found\n" unless glob($isdnCorpusDB.'/*');

   copy ("$isdnBinDir//master.dat", 'master.dat');
   copy ("$isdnBinDir//build.set0", 'build.set');

 # make a working copy of the infected files

   my $original = get_original_sample ("$UNC/$isdnSample", @replicants);
   $attributes{'original'} = (split m[/],$original)[-1];
   Log "\noriginal sample: $original\n";

   parse_replicants_list ($UNC, $Cookie, $isdnVGoats, @replicants, %attributes);
   map {Log "\nreplicant\t$_->{'infected'}\ngoat\t\t$_->{'goat'}\n";} @replicants[1..$#replicants];

   fail "Insufficient replicants: ".(@replicants-1)."\n" if @replicants-1 < $::MIN_REPLICANTS;

   # ifdef TEST goto $1 if $::PARM =~ m/retry=(\S+)/i;

 # do the code-data segregation.

   code_data_segregation (@replicants);

 # run Autoverv on the replicants.

 AUTOVERV:
   $rc = Autoverv (@replicants,'');
   Log "Autoverv completed with return code $rc\n" if ($rc);

   setLengthDependency (@replicants, getCriticalLength($::ERS_INSTRUCTIONS));

   unless ($rc > 1) {
      $defType = ('detect','repair')[$rc];
      RemoveExcludedSamples ('autoverv.xcl', @replicants);
      compressERS ($::ERS_INSTRUCTIONS, $::ERS_INSTRUCTIONS);
   }

 # select a signature from the constant viral regions.

 SIGS:
   my $pPAMsigs = getSignatureSet(@{getStaticSigs(@replicants,$::INFECTION_MAP,16)},@replicants);
   $pPAMsigs = getSignatureSet(@{getEmulationSigs(@replicants)},@replicants) unless scalar @$pPAMsigs > 0;
   fail "No PAM signatures found\n" unless scalar @$pPAMsigs > 0;
   $defType = 'detect' unless defined $defType;

 # generate definitions from the ERS instructions.

 MAKEDEFS:
   MakeDefs($::UNC, $::ERS_INSTRUCTIONS ,@$pPAMsigs, $defType, %attributes);

 # set up for unit testing

TEST:
   writeReplicantList ($defType, @replicants, "$::UNC/replicants.lst");

   # ifdef TEST Log sprintf("Analysis completed in %8.2f mins\n",(time-$::STARTTIME)/60);
   # ifdef TEST if (defined &DefTest) {
   # ifdef TEST    my $result = DefTest();
   # ifdef TEST    fail "failed unit test ($defType)\n" unless $result eq 'OK';
   # ifdef TEST    Log "INFO: passed unit test ($defType)\n";
   # ifdef TEST }

 # done.
   # ifdef TEST return 0;

   if ($defType eq 'detect') {   fail "Detect only definition - deferring\n"; }
   else { return 0; } #endif

}



sub Vanalysis_cleanup () {
   rmtree ($::WKDIR,0,1) unless $::KEEP_WORK_DIR;
   $::log->DESTROY();
}


#-------------------------------------------------------------------------------


sub get_original_sample ($\@) {
   my ($srcdir,$pReplicants) = @_;

   my @sampleList = glob "$srcdir/*";
   unless (@sampleList == 1) {fail "should be exactly one file in original sample directory $srcdir, not ".scalar @sampleList.":\n".map{"\t$_\n"}@sampleList;}
   my $name = PD($sampleList[0]);
   my $type = fileType($name);
   (my $newName = $name) =~ s<\.[^\.]*$> <.$type>;
   Xcopy ('',$name,$newName);
   fail "unable to rename original sample\n" unless -s $newName;
   push @$pReplicants, {
      'infected' =>  $newName,
      'goat'     =>  '',
      'Itype'    =>  $type,
      'Gtype'    =>  '',
      'original' =>  $name
   };
   return $newName;
}



sub parse_replicants_list ($$$\@\%) {
   my ($UNC,$Cookie,$goatRoot,$pReplicants,$pAttributes) = @_;
   my ($nReplicants, $minLength, $COM, $EXE) = (0,0,0,0);

   foreach my $replicant (ReadSamplesFileMkII($UNC,$Cookie,-1)) {
      $replicant =~ s<\Q$UNC><$::WorkUNC>;
      my ($infected, $goat, $length) = ($$replicant[$iPath],$$replicant[$iGoatName],(split /-/,$$replicant[$iAttributes])[0]);
      $length = 0 if $length =~ m<^none$>i;

      next if $infected =~ m</sectors/>i;
      Log ("WARN: missing goat for $infected\n"), next if $goat =~ m<^none$>i;

      my $infectedPath = "$UNC/$infected";
      Log ("WARN: zero-length infected file $infectedPath\n"), next unless -s $infectedPath;
      my $goatPath = "$goatRoot/$goat";
      Log ("WARN: zero-length goat file $goatPath\n"), next unless -s $goatPath;

      my $target = fileType($goatPath);

      push @$pReplicants, {
         'infected' =>  PD($infectedPath),
         'goat'     =>  PD($goatPath),
         'Itype'    =>  fileType($infectedPath),
         'Gtype'    =>  $target
      };

      $minLength = $nReplicants ? min ($minLength, $length) : $length;
      $nReplicants++;

      if    ($target eq 'COM') {$COM++;}
      elsif ($target eq 'EXE') {$EXE++;}
      else                     {fail "Goat $goatPath is of invalid type: $target\n";}
   }

   $pAttributes->{'count'}  = $nReplicants;
   $pAttributes->{'length'} = $minLength;
   $pAttributes->{'COM'}    = $COM;
   $pAttributes->{'EXE'}    = $EXE;
}



sub code_data_segregation (\@) {
   my ($pReplicants) = @_;

   foreach my $replicant (@$pReplicants) {
      my $replPath = $replicant->{'infected'};

      Codo($replPath);

      $replicant->{'codata'} = "$replPath.sections";
   }
}



sub Codo ($) {
   (my $Path = PD($_[0])) =~ m<^(.*)[/\\]([^/\\]*)$>;
   my ($Dir,$File) = ($1,$2);

   Wipe glob("$Path.codo,$Path.sections");

   Xcopy ('',$Path,"$Dir\\work\\$File");   # the things we do to keep these dumb programs happy!
   startNwait (PD($::Codo), " -d \"$Dir\\work\" -a -b -wow 1 -nodebug -timeout $::CODO_TIMEOUT");
   Xcopy ('',"$Dir\\work\\*","$Dir\\*");
   Wipe glob("$Dir\\work\\*");


   foreach (@::LOG) {
      if (m/error in/) {
         unlink "$Path.sections";
         Log "WARN: Bad code-data seg. for $File: $_";
         last;
      }
   }
   Log "WARN: no code-data segregation for $File\n" unless (-s "$Path.sections");

   Wipe glob("$Path.codo,$Dir/*.out");
}



sub Autoverv (\@$) {
   my ($pReplicants,$optionalArgs) = @_;

   unlink glob('*.bytes2,*.ers,*.vds');

 # create the input list for autoverv

   my @list = ();
   foreach my $replicant (@$pReplicants) {
      if ($replicant->{'infected'}) {
         my $line .= PD "$replicant->{'infected'}";
         $line    .= PD",$replicant->{'goat'}" if length($replicant->{'goat'}) > 0;
         push @list, "$line\n";
      }
   }
   open LIST, ">autoverv.lst" or fail "Error $::ERRNO creating autoverv input list file ".cwd()."/autoverv.lst\n";
   print LIST (sort sampleOrder @list);
   close LIST;

 # check the code-data segregation quality

   my $CodoIsOK = testCodata(@$pReplicants);
   $optionalArgs .= ' -codo' if $CodoIsOK;

 # run autoverv

   run ($::Autoverv, "autoverv.lst -MinRep $::MIN_REPLICANTS $optionalArgs");
}



sub sampleOrder {   # makes it easier to spot patterns & dependencies in the result; does not change them
   $a =~ m/,/ cmp $b =~ m/,/                            # original sample (i.e. without goat) first
      or
   uc(substr($a,-3)) cmp uc(substr($b,-3))              # com before exe
      or
   -s ($a =~ m/,(.*)/,$1) <=> -s ($b =~ m/,(.*)/,$1)    # by goat length
      or
   uc($a) cmp uc($b);
}



sub RemoveExcludedSamples ($\@) {
   my ($ExclListFile, $pReplicants) = @_;
   my @Exclusions = `type $ExclListFile`;
   return if scalar(@Exclusions) * 2 > scalar(@$pReplicants);  # if more than 1/2 were excluded, assume this is because
                                                               # the virus is polymorphic, not that they are bad samples
   my @NewReplicants;
   REPLICANT: foreach my $Replicant (@$pReplicants) {
      (my $FileName = $Replicant->{'infected'}) =~ tr<[a-z]\\><[A-Z]/>;
      foreach (@Exclusions) {
         chomp; tr<[a-z]\\><[A-Z]/>;
         next REPLICANT if $FileName eq $_;
      }
      push @NewReplicants, $Replicant;
   }
   Log "WARN: failed to remove all exclusions(@Exclusions)\n" unless scalar(@$pReplicants) == scalar(@NewReplicants) + scalar(@Exclusions);
   @$pReplicants = @NewReplicants;
}



sub compressERS ($$) {
   my ($inERS, $outERS) = @_;
   my (@hdr, @ver, @rep);

   open INERS,  "<$inERS"  or fail "unable to open $inERS as INERS\n";
   foreach (<INERS>) {
      if (m/^CriticalLength/) {
         push @hdr, $_;
      }elsif (m/^Verify/) {
         push @ver, $_;
      }else {
         push @rep, $_;
      }
   }
   close INERS;

   @ver = mergeERS(@ver);

   open OUTERS, ">$outERS" or fail "unable to open $outERS as OUTERS\n";
   print OUTERS @hdr,@ver,@rep;
   close OUTERS;
}


sub mergeERS (@) {
   my (@ERS) = @_;
   my @out;

   foreach (@ERS) {s/^(\S+ )([^,]+), (.+\n)/$1 $3 ! $2/;}  # move type to end

   my $currCode = my $currType = '';
   foreach (sort(@ERS),'!') {                               # append end-marker
      my ($code, $type) = split / ! /;
      if ($code eq $currCode) {
         $currType = "$currType,$type";
      }else {
         unless ($currCode eq '') {
            my ($instr,$args) = split / /, $currCode, 2;
            $currType =~ s/COMl,COMs/COM/;
            $currType =~ s/EXEl,EXEs/EXE/;
            $currType =~ s/COM,EXE/FILE/;
            foreach (split /,/, $currType) {
               push @out, "$instr $_, $args" if $_;
            }
         }
         $currCode = $code;
         $currType = $type;
      }
   }
   return @out;
}



sub testCodata (\@) {
   my ($pReplicants) = @_;
   my $badcount = 0;
   foreach my $replicant (@$pReplicants) {
      my $CodoFile = PD($replicant->{'codata'});
      my $length = -s PD($replicant->{'infected'});
      return 0 unless $length > 0;
      foreach (`type $CodoFile`) {
         if (not m< ^ \s* \*? \s* \[ \s* (\d+) \s+ (\d+) \s* \] \s* $ >x
         or $1 < 0 or $2 < $1 or $2 >= $length) {
            unlink $CodoFile;
            Log "WARN: Code-data seg. will not be used - $CodoFile: $_";
            $badcount++;
         }
      }
   }
   return 1; #?? return $badcount == 0;
}


# Try each signature on each sample. If detected, get the opcode map & # of iterations to detect
sub getStaticSigs (\@$$) {
   my ($pReplicants,$PAMmapFile,$ExecutionLimit) = @_;
   my $pSigs = [];

   my $tStart = time;
   Log "Getting static signatures\n";

   my @sigList;
   foreach my $signature (`type $PAMmapFile`) {
      my ($sig, $class, $encr, $txtType, $anchor, $offset) = split /\s+/,$signature;
      next unless $encr eq 'PLAIN' and $txtType eq 'CODE';   #??
      push @sigList, $sig;
   }
   return $pSigs if @sigList == 0;

   my $sigListFile = "signature.lst";
   unlink $sigListFile;
   my $prevSig = '';
   foreach (sort @sigList) {
      `echo $_>> $sigListFile` unless $_ eq $prevSig;
       my $prevSig = $_;
   }

   foreach my $replicant (@$pReplicants) {
      my $replPath = $replicant->{infected};
      my $rc = run ($::PAMemulate, " $replPath /MAX $ExecutionLimit /SIGLIST $sigListFile /SIGCOUNT ".(scalar @sigList).($::VERBOSE ? ' /V' : ''));
      if ($rc) {
         Log  "WARN: rc $rc emulating $replPath for opcode list\n";
         next;
      }
      my %fileSigs;
      foreach (`type emulate.log`) {
         next unless m/^SIGFOUND (\d+) (\d+) ([01]{256}) ([0-9a-fA-F]+)$/;
         fail ("siglist mismatch @ $1\n\t$sigList[$1]\n\t$4\n") unless (sort @sigList)[$1] eq $4; #??
         my ($sig,$nIter,$opcodes) = ($4,$2,$3);
         unless (defined $fileSigs{$sig} and $fileSigs{$sig}->{iterations} <= $nIter) {
            $fileSigs{$sig} = {iterations => $nIter, opcodes => $opcodes};
         }
      }
      while (my ($sig, $sigData) = each %fileSigs) {
         push @$pSigs, sprintf ("$sig|%8d|%s|$replPath",$sigData->{iterations},$sigData->{opcodes});
      }
      Log "Static signatures fetch: ".(time-$tStart)."s elapsed\n";
   }
   return $pSigs;
}


sub getEmulationSigs(\@) {
   my ($pReplicants) = @_;
   my $pSigs = [];

   my $tStart = time;
   Log "Getting emulation signatures\n";

   foreach my $replicant (@$pReplicants) {
      my $replPath = $replicant->{infected};
      my $rc = run "$::PAMemulate $replPath -NMOD 256";
      fail "detection emulation of $replPath failed with rc $rc\n" if $rc;

      my %fileSigs;
      my ($sig, $nIter, $opcodes);
      foreach (`type emulate.log`) {
         $opcodes   = $1       if m/^INST\s+([01]+)/;
         if (m/^SIG\s+\d+\s+(\d+)\s+(\S+)/) {
            $nIter  =  $1;
            ($sig = $2) =~ s/,//g;
            unless (defined $fileSigs{$sig} and $fileSigs{$sig}->{iterations} <= $nIter) {
               $fileSigs{$sig} = {iterations => $nIter, opcodes => $opcodes};
            }
         }
      }
      while (my ($sig, $sigData) = each %fileSigs) {
         push @$pSigs, sprintf ("$sig|%8d|%s|$replPath",$sigData->{iterations},$sigData->{opcodes});
      }
      Log "Emulation signature fetch: ".(time-$tStart)."s elapsed\n";
   }
   return $pSigs;
}


sub getSignatureSet (\@\@) {
   my ($pCandidates, $pReplicants) = @_;
   my $pSelection = [];

   return $pSelection unless @$pCandidates;

   my $tStart = time;
   Log "Begin selecting signatures\n";

   my (%sampleType,%typeCount);
   foreach my $replicant (@$pReplicants) {
      $sampleType{$replicant->{infected}} = $replicant->{Itype};
      $typeCount{$replicant->{Itype}}++;
   }
   Log "need to cover ".join(' ', keys %typeCount)."\n";

   # find the file types covered by each signature
   my @typeSigs;
   my ($currSig,$maxIter,$sigOpcodes,%missCount) = ('',0,'',%typeCount);
   foreach ((sort {nKey($a,1) cmp nKey($b,1);} @$pCandidates),'|||') {   # group by signature
      my ($sig, $nIter, $opcodes, $sample) = split /\|/;
      if ($sig ne $currSig and $currSig) {
         my @typeList;
         while (my($type,$misses) = each %missCount) {
            push @typeList, $type unless $misses;
         }
         fail "duplicate sig generated: $currSig\n" if @typeSigs and $currSig eq nKey($typeSigs[-1],1); #??
         push @typeSigs,"$currSig|$maxIter|$sigOpcodes|".(join ',',@typeList) if @typeList;
         Log "$maxIter $currSig @typeList\n" if @typeList and $::VERBOSE;
         ($maxIter,$sigOpcodes,%missCount) = (0,'',%typeCount);
      }
   last unless $sig;
      $currSig = $sig;
      $maxIter = max($nIter,$maxIter);
      $sigOpcodes = ORopcodes($opcodes,$sigOpcodes);
      $missCount{$sampleType{$sample}}--;
      fail "type count underflow\n" if $missCount{$sampleType{$sample}} < 0; #??
   }
   Log "Selecting signatures - file types determined after ".(time-$tStart)."s\n";
   Log "*** No candidate signature covered any file type\n" unless @typeSigs;

   # score the candidates
   my $sigListFile = "signature.lst";
   unlink $sigListFile;
   foreach (@typeSigs) {   # in signature order by construction
      my ($sig,@other) = split /\|/;
     `echo $sig>>$sigListFile`;
   }
   my %sigScore;
   foreach (`$::Score -dbr=$::CORPUS_DB \@$sigListFile`) {
      Log "DBscore: $_" if $::VERBOSE;
      my ($score, $sig) = split /\s+/;
      $sigScore{$sig} = $score;
   }
   my @scoredSigs;
   foreach (@typeSigs) {
      my ($sig, $nIter, $opcodes, $types) = split /\|/;
      next if $sigScore{$sig} > $::SIG_WORST_SCORE;
      my $scoreRanking = 1000 + int($sigScore{$sig}-0.5);
      push @scoredSigs, sprintf("$nIter|%4d|$sig|$sigScore{$sig}|$opcodes|$types",$scoreRanking);
      Log "$nIter $sig $sigScore{$sig} $types\n" if $::VERBOSE;
   }
   Log "Selecting signatures - signatures scored after ".(time-$tStart)."s\n";
   Log "*** No candidate signature scored well enough\n" unless @scoredSigs;

   # get enough signatures to cover all sample types
   my @initialSelection;
   my %uncovered = %typeCount;
   my $nUncoveredTypes = scalar(keys %uncovered);
   foreach (sort {nKey($a,2) cmp nKey($b,2);} @scoredSigs) {
      my ($nIter,$rank,$sig,$score,$opcodes,$types) = split /\|/;
      my $nCovers = 0;
      foreach my $type (split /,/,$types) {
         if ($uncovered{$type}) {
            $nCovers++;
            $uncovered{$type} = 0;
         }
      }
      push @initialSelection, "$sig|$nIter|$rank|$score|$opcodes|$types" if $nCovers;
      last unless ($nUncoveredTypes -= $nCovers) > 0;
   }
   Log "Selecting signatures - initial selection after ".(time-$tStart)."s\n";
   Log "*** Incomplete coverage by candidate signatures\n" if $nUncoveredTypes;
   return $pSelection if $nUncoveredTypes;

   # remove any signature that covers a subset of the types covered by another
   %uncovered = %typeCount;
   foreach (reverse @initialSelection) {
      my ($sig,$nIter,$rank,$score,$opcodes,$types) = split /\|/;
      my $nCovers = 0;
      foreach my $type (split /,/,$types) {
         if ($uncovered{$type}) {
            $nCovers++;
            $uncovered{$type} = 0;
         }
         push @$pSelection, {sig => $sig, score => $score, opcodes => $opcodes, instrCount => int($nIter)} if $nCovers;
         Log (sprintf "%7.2f $sig $types\n",$score) if $nCovers;
      }
   }
   Log "Selecting signatures - final selection after ".(time-$tStart)."s\n";
   return $pSelection;
}

# for use above
sub nKey ($$) {join '', (split (/\|/, ($_[0].'|'x$_[1]), $_[1]+1))[0..$_[1]-1];}
# alt - leaves in the delimiters   sub nKey ($$) {(($_[0].'|'x$_[1]) =~ m/^(([^\|]*\|){$_[1]})/)[0]}


# Read the critical length value (if any) set by autoverv in the ERS file
sub getCriticalLength ($) {
   my ($ERSfile) = @_;
   my $ers0   = (`type $ERSfile`)[0];
   return (defined $ers0 and $ers0 =~ m/^CriticalLength\s+(\d+)/) ? $1 : 0;
}


# If the solution involves a critical length, mark the replicant types with their length code
sub setLengthDependency(\@$) {
   my ($pReplicants, $lCritical) = @_;
   return unless $lCritical;

   foreach my $replicant (@$pReplicants) {
      $replicant->{Itype} .= (-s $replicant->{infected} <= $lCritical ? 's' : 'l');
   }
}



sub MakeDefs ($$\@$\%) {
   my ($UNC,$ers,$pPAMsigs,$defType,$pAttributes) = @_;
   return unless -s $ers or $defType eq 'detect';

   my $dpk = "$UNC/$isfnNAVdefDelta";
   mkSubdir ($UNC, $isdnNAVdefDelta) or fail "Error $::ERRNO creating delta directory $UNC/$isdnNAVdefDelta\n";

   my $flags = '';
   $flags .= 'D' if $defType eq 'detect';
   $flags .= 'R' if $defType eq 'repair';
   $flags .= 'C' if $pAttributes->{'COM'} > 0;
   $flags .= 'X' if $pAttributes->{'EXE'} > 0;
   $flags .= max($pAttributes->{'length'}, 0);
   return unless $flags =~ m/^[DR](CX|C|X)\d+$/;

   my $sigs = '';
   foreach (@$pPAMsigs) {$sigs .= C_hexForm($$_{sig}).',DONE ';}

   my $opcodes;
   foreach (@$pPAMsigs) {
      $opcodes = ORopcodes ($opcodes, $$_{opcodes});
   }
   $opcodes = '1' x 256 unless $opcodes;

   my $instrCount = 0;
   foreach (@$pPAMsigs) {
      $instrCount = max ($instrCount, $$_{instrCount}) if defined $$_{instrCount};
   }
   my $stop = '';
   $stop = 'STOP128K' if $instrCount * $::ITERATION_MARGIN < 128 * 1024;
   $stop = 'STOP96K'  if $instrCount * $::ITERATION_MARGIN <  96 * 1024;
   $stop = 'STOP80K'  if $instrCount * $::ITERATION_MARGIN <  80 * 1024;
   $stop = 'STOP64K'  if $instrCount * $::ITERATION_MARGIN <  64 * 1024;
   $stop = 'STOP48K'  if $instrCount * $::ITERATION_MARGIN <  48 * 1024;
   $stop = 'STOP32K'  if $instrCount * $::ITERATION_MARGIN <  32 * 1024;
   $stop = 'STOP16K'  if $instrCount * $::ITERATION_MARGIN <  16 * 1024;
   $stop = 'STOP1K'   if $instrCount * $::ITERATION_MARGIN <  1024;
   $stop = 'STOP16'   if $instrCount * $::ITERATION_MARGIN <  10;
   Log "$stop: \t$instrCount\n";
   # put limit??
   my $rc = run ("perl $::DBG -S -w $::Bindefpak", "$flags $stop $opcodes $sigs" .' <"'.PD($ers).'" >"'.PD($dpk).'"');

   fail "BinDefPak.pl failed with return code $rc\n" if $rc;
   fail "did not generate a defpak file\n" unless -s $dpk;
}



sub ORopcodes($$) {
   my ($x,$y) = @_;
   return $y if !defined $x;
   return $x if !defined $y;
   my @x = split //,$x;
   my @y = split //,$y;
   my $result;
   for (my $i = 0; $i < max(@x,@y); $i++) {
      if    (!defined $x[$i]) {$result .= $y[$i];}
      elsif (!defined $y[$i]) {$result .= $x[$i];}
      else                    {$result .= $x[$i] || $y[$i] ? 1 : 0;}
   }
   return $result;
 }



sub fileType ($) {
   my ($file) = @_;
   my $bfr;

   open PGM, $file or fail "Error $::ERRNO opening $file in fileType()\n";
   binmode PGM;
   if (read (PGM, $bfr, 2, 0) < 2) { fail "Error $::ERRNO reading $file in fileType()\n";}
   close PGM;

   if ($bfr eq 'MZ' or $bfr eq 'ZM' ) {return 'EXE';}
   if ($bfr eq "\xFF\xFF")            {return 'DEV';}
   return 'COM';
}

#-------------------------------------------------------------------------------

sub getParameters () {
# make the parameters global so they're always available to fail()
  GetOptions ("Cookie=s"    => \$::COOKIE,
              "ProcessId=i" => \$::PID,
              "Parameter=s" => \$::PARM,
              "UNC=s"       => \$::UNC,
             );
}



sub makeNewWorkDir () {
   $::WKDIR = $1, return if $::PARM =~ m/workdir=(\S+)/i;    # for testing ??
   $::WKDIR = PD (MkLocalWorkDir($::COOKIE,$::PID,$::UNC))   or fail "Error $::ERRNO creating a work directory\n";
   # ifdef TEST return if $::PARM =~ m/retry/i;
   Wipe glob "$::WKDIR/*"                                    or fail "Error $::ERRNO cleaning the work directory $::WKDIR\n";
}


sub mkSubdir ($$) {
   my ($parent,$child) = @_;
   my $path = "$parent/$child";

   if (-d $path) {chmod(0777,$path)      or fail "Error $::ERRNO setting $path mode to 0777\n";}
   else          {mkpath($path,1,0777)   or fail "Error $::ERRNO creating $path\n";}

   Wipe glob("$path/*");
   return (-d $path ? $path : '');
}


sub Xcopy ($$$) {
   my ($opt,$src,$dest) = ($_[0],PD($_[1]),PD($_[2]));
   my $destType = ($dest =~ m<[\*\?][^\//]*$>) ? 'd' : 'f';
  `echo $destType | xcopy $opt "$src" "$dest"`;
}



sub testPgmExists (@) {testFileNotEmpty @_};



sub testFileNotEmpty (@) {
   foreach (@_) {
      fail "$_ not found or is empty\n" unless -s;
   }
}



sub writeReplicantList ($\@$) {
   my ($defType,$pReplicants,$dest) = @_;

   $$pReplicants[0]->{'infected'} = $$pReplicants[0]->{'original'};
   map {$$_{'infected'} =~ s<\Q$::WorkUNC><$::UNC>} @$pReplicants;
   open (RL, ">$dest") or fail "Unable to open replicants list $dest\n";
   print RL Data::Dumper->new([$defType,$pReplicants],[qw($defType *replicants)])->Purity(1)->Useqq(1)->Dump;
   close RL;
}



# change path delimiters into those accepted by the current OS
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
   my ($txt) = @_;
   print $txt;
   if (defined $::log) {
      chomp $txt;
      $::log->logString($txt);
   }
}



sub startLog ($) {
   my ($LogRoot) = @_;

   $::log = new AVIS::Logger($LogRoot);

   my $captureFile = $ENV{'VANALYSIS_LOG'} ? $ENV{'VANALYSIS_LOG'} : "$LogRoot/Vanalysis.log";
   unless ($captureFile =~ m/^console$/i) {
      open STDOUT,  ">$captureFile"   or Log "Unable to redirect stdout to $captureFile\n" ;
      open STDERR,  ">&STDOUT"        or Log "Unable to dup stderr to stdout\n";
      STDERR->autoflush(1);
      STDOUT->autoflush(1);
   }
}



sub printBanner () {
   Log (('=' x 80)."\n");
   if (-s $0) {my $scriptDate = (stat $0)[9]; Log "$0 @ ".localtime($scriptDate)."\n";}
   Log "VANALYSIS @ ".$ENV{'COMPUTERNAME'}.": Cookie $::COOKIE, PID $::PID, Parms $::PARM, UNC $::UNC\n\t".scalar localtime($::STARTTIME)."\n";
   Log (('=' x 80)."\n");
}



sub fail ($) {
   topdir();
   Log "FAIL: @_";
   finish (8);
}


sub finish ($) {
   my ($result) = @_;
   $::NEXT_QUEUE = $result ? $isqnDefer : $isqnSARCExit;

   if ($::NEXT_QUEUE eq $isqnSARCExit) {
      $::PARM = "$isfnDeltaPathName $isqnSerializeBuild $::UNC/$isfnNAVdefDelta";
   }

   Log "VANALYSIS COMPLETED: $::NEXT_QUEUE; \t".sprintf("%8.2f mins elapsed\n",(time-$::STARTTIME)/60);
   Vanalysis_cleanup;
   system 'type "'.PD("$::UNC/Vanalysis.log").'" >> "'.PD("$::UNC/avislog0.log").'"' unless $ENV{'VANALYSIS_LOG'};   #?? testing

   exit ($result);
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


END {
   SpecifyStopping ($::COOKIE, $::PID, $::UNC, $::NEXT_QUEUE, "1 of 1");
   DoNext ($::COOKIE, $::PID, $::UNC, $::NEXT_QUEUE, $::PARM);
   WriteSignatureForDataFlow ($::PID, $::UNC);
}

