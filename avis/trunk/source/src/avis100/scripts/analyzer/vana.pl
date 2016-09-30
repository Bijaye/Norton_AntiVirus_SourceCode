#!/usr/bin/perl
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
# Assumed directory layout:
# $UNC/$isdnSample                 - original sample
# $UNC/$isdnSigRes                 - sigres directory
# $UNC/$isdnVervRes                - verv files directory
# $UNC/$isdnRun/PID                - directory with replication information,
#                                    where PID is the $ProcessID
# $UNC/$isdnRun/PID/$isdnInfected  - directory containing samples
# $UNC/$isdnRun/PID/$isdnSectors
# $UNC/$isdnRun/PID/$isdnSample    - Sample on which the replication was based

#= Imports ====================================================================
use lib ('//Shine/TestZoneShine/perl/lib/site'); #!!testing only

use Cwd;
use Win32::Mutex;
use Getopt::Long;
use DataFlow;
use File::Path;
use File::Copy;
use AVIS::Vides;
use AVIS::FSTools;
use AVIS::Dirstack;
use AVIS::Local;
use AVIS::Logger;
use English;
use strict;

#= Declarations ===============================================================

#   Vanalysis ($UNC,$Cookie,$ProcessID)    - the main analysis routine
sub Vanalysis ($$$);
sub Vanalysis_cleanup ();             #    - remove work files etc.

#   get_original_sample ($srcDir,$destDir,\@replicants)
sub get_original_sample ($$\@);

#   parse_replicants_list ($UNC,$Cookie,$DestRoot,$GoatRoot,\@replicants, \%attributes)
sub parse_replicants_list ($$$$\@\%);

#   code_data_segregation (\@replicants)   - applied to a list of replicants
sub code_data_segregation (\@);

#   Codo ($FilePath) - code-data segregation applied to a single file
sub Codo ($);

#   Autoverv ($root,@replicants,$optionalArgs)
sub Autoverv ($\@$);

#   RemoveExcludedSamples ($ExclusionListFile, \@replicants)
sub RemoveExcludedSamples ($\@);

#   findPAMsignatures ($PAMmapFile);
sub findPAMsignatures ($);

#   getBestCommonSig (@PAMmap,@fileType)
sub getBestCommonSig (\@@);

#??#   getSigOfType (@PAMmap, $type)
#??sub getSigOfType (\@$);

#   scoreSig ($sig)
sub scoreSig ($);

#   testCodata (@replicants)
sub testCodata (\@);

#   MakeDefs ($UNC,$ERSfile,\@PAMdefs,$testResult,\%attributes)
sub MakeDefs ($$\@$\%);

#   fileType ($fileName)
sub fileType ($);

#   getParameters
sub getParameters();

#   startLog ($LogRoot)
sub startLog($);

#   printBanner ();
sub printBanner ();
   
#   mkSubdir ($parent,$child)
sub mkSubdir ($$);

#   Copy ($src_pattern, $dest)
sub Copy ($$);

#   Xcopy ($opt,$src,$dest)
sub Xcopy ($$$);

#   treeSync($src,$dest)
sub treeSync ($$);

#   testPgmExists (@pgm)
sub testPgmExists (@);

#   testDirExists (@dir)
sub testDirExists (@);

#   testFileNotEmpty (@dir)
sub testFileNotEmpty (@);

#   run ($pgm,@args)
sub run ($@);


#   startNwait ($pgm,@args)
sub startNwait ($@);

#   PD ($path) - change path delimiters into those accepted by the current OS
sub PD ($);

#   fail ($message)
sub fail ($);

sub min ($$);
sub max ($$);
sub minstr ($$);
sub maxstr ($$);
sub chompList (@);
sub member ($\@);
sub Log ($);

sub makeNewWorkDir ();

#   UnitTest (@Replicants,$ERSfile,@sigList)
sub UnitTest (\@$\@);

#   PAMbuild ($ERSfile,\@sigList)
sub PAMbuild ($\@);

#   sub PAMinstallDefs ($installDir)
sub PAMinstallDefs ($);

#   PAMremoveDefs ($appid)
sub PAMremoveDefs ($);

#   PAMrepTest (@pReplicantList)
sub PAMrepTest (\@);

#= Initialization =============================================================

$::STARTTIME = time;
getParameters();
startLog($::UNC);
printBanner();
makeNewWorkDir();

#- Default parameters ---------------------------------------------------------

$::Codo                = "$isdnBinDir/codow.exe";
$::Autoverv            = "$isdnBinDir/autoverv.exe";
$::Score               = "$isdnBinDir/DBscore.exe";
$::Bindefpak           = "$isdnBinDir/Bindefpak.pl";


$::ERS_INSTRUCTIONS    = "autoverv.ers";
$::CORPUS_DB           = PD("$isdnCorpusDB/");

$::CODO_TIMEOUT        =  20;              # code-data segregation timeout in seconds   ?? was 20
$::MIN_REPLICANTS      =  2;
                                           #?? - doesn't seem to work yet
$::SIG_WORST_SCORE      = -1;                                                    
$::SIG_WORST_COMN_SCORE = -1;                                                    
$::MAX_SIG_OPFFSET      = 512;

$::USE_DEBUG_UNIT_TEST =  0;               # use the test repair pgm, not PAM (more diagnostics). 
$::DEFINE_NODUMP       = '/DNODUMP';       # if defined, the ERS code will, on fail, write debug info 
                                           # into the target file (OK for either PAM or the test pgm).
$::USE_SUBPROCESSES    =  0;               # if true then programs and commands will be STARTed in 
                                           # their own DOS box. 
$::KEEP_WORK_DIR       =  0;               # - for post-run analysis.
$::COPY_CODO_FROM      = '';               # - instead of using codow to do code-data segregation.

#- overrides (for testing) ----------------------------------------------------


   my $srvRoot            = "//Elbe/ElbeD";        ###"//Mosel/srvroot/vanalysis";
   my $pgmRoot            = "$srvRoot/auto";
   my $NAVdefRoot         = "$srvRoot/NAV";
   
   $::Codo                = "$pgmRoot/codo/codow.exe";
   $::Autoverv            = "//Shine/TestZoneShine/autoverv/test/autoverv.exe";
   $::Score               = "//Shine/TestZoneShine/analysis/test/DBscore.exe";
   $::Bindefpak           = "//Shine/TestZoneShine/analysis/bindefpak.pl";

   $::CORPUS_DB           = PD("//Mosel/CorpusDB/");

# run the script specified in the parameter argument
   if (-s $::PARM && !do $::PARM) {fail "$::PARM did not complete\n";}
 
 # temporary unit testing...
    unless (defined $::NAV_DIR) {
      $::NAV_DIR = PD("$isdnROOT/VUT_NAV");
      $::DoTreeSync = 1;
   }
   $::VIRUS_DEF           = PD("$::NAV_DIR/PAM/virus.def");
   $::NAVappIDfile        = PD("$isdnROOT/VUT_NAVSDK.APPID");
   $::TestBinRepair       = "$pgmRoot/TestBinRepair/TestBinRepair.pl";

   $::SYS_DRIVE           = $ENV{'SYSTEMDRIVE'};
   fail "%SYSTEMDRIVE% not defined\n" unless $::SYS_DRIVE;

   $::TEST_HOLD           = PD (mkSubdir($::UNC,'testfiles'));   # keep copies of the unit test files
   
   $::INFECTION_MAP       = "autoverv.PAMbytes";

 # workaround replicator bugs!!!
 ##run ("perl", "\\\\shine\\testzoneshine\\analysis\\test\\fixlst.pl $::UNC $::COOKIE $isdnVGoats");

#=== main =====================================================================

pushdir($::WKDIR);

# do the analysis

  my $result = Vanalysis($::UNC,$::COOKIE,$::PID);

# terminate

  popdir();
  Vanalysis_cleanup;
  hand_off_to_dataflow ($result ? $isqnDefer : $isqnReturn);
  exit ($result);

#==============================================================================

sub Vanalysis ($$$) {
   my ($UNC,$Cookie,$ProcessID) = @_;
   my @replicants;       # infected files and their goats; [0] is the original sample
   my %attributes;       # info. for building a vides.results file
   my $rc;               # return codes

# check that necessary files are there

   testPgmExists ($::Codo,
                  $::Autoverv,
                  $::Score,
                  $::Bindefpak);

 # make a working copy of the infected files

   my $original = get_original_sample ("$::UNC/$isdnSample", cwd()."/plaintext/$isdnSample", @replicants);
   $attributes{'original'} = (split m[/],$original)[-1]; #??(reverse(split m[/],$original))[0];   #?? 
   Log "\noriginal sample: $original\n";

   parse_replicants_list ($UNC, $Cookie, cwd().'/plaintext', $isdnVGoats, @replicants, %attributes);
   map {Log "\nreplicant\t$_->{'infected'}\ngoat\t\t$_->{'goat'}\n";} @replicants[1..$#replicants];
   
   fail "Insufficient replicants: ".(@replicants-1)."\n" if @replicants-1 < $::MIN_REPLICANTS;

CODO: # do the code-data segregation

   code_data_segregation (@replicants);

AUTOVERV: # run Autoverv on the replicants.

   $rc = Autoverv ('plaintext',@replicants,'');
   fail "Autoverv failed with return code $rc\n" if ($rc);
   
   RemoveExcludedSamples (PD('plaintext/autoverv.xcl'), @replicants);
   
   Copy ("plaintext/$::ERS_INSTRUCTIONS", '.');

AUTOSIG: # select a signature from the constant viral regions.

   my @PAMsigs = findPAMsignatures ($::INFECTION_MAP);
   fail "No PAM signatures found\n" unless scalar @PAMsigs > 0;

TEST: # unit test

   my $result = UnitTest(@replicants,$::ERS_INSTRUCTIONS,@PAMsigs);
   fail "failed unit test\n" unless $result eq 'repaired' or $result eq 'detected';

DEFGEN: # generate definitions from the ERS instructions.
   
   MakeDefs($UNC, $::ERS_INSTRUCTIONS ,@PAMsigs, $result, %attributes) if $result;
   
DONE:

   Log "INFO: passed unit test ($result)\n";
   return 0;
}



sub Vanalysis_cleanup () {
   rmtree ($::WKDIR,0,1) unless $::KEEP_WORK_DIR;
}


#-------------------------------------------------------------------------------


sub get_original_sample ($$\@) {
   my ($srcdir,$destdir,$pReplicants) = @_;

   my @sampleList = glob "$srcdir/*";
   unless (@sampleList == 1) {fail "should be exactly one file in original sample directory $::UNC/$isdnSample, not ".scalar @sampleList.":\n".map{"\t$_\n"}@sampleList;}

   Xcopy ('', $sampleList[0], "$destdir/*");
   my $original = (glob "$destdir/*")[0];
   unless (-f $original) {fail "Error $::ERRNO copying original sample $sampleList[0] to $original\n";}

   push @$pReplicants, {'infected' => $original, 'goat' => ''};

   return $original;
}



sub parse_replicants_list ($$$$\@\%) {
   my ($UNC,$Cookie,$destRoot,$goatRoot,$pReplicants,$pAttributes) = @_;
   my ($nReplicants, $minLength, $COM, $EXE) = (0,0,0,0);

   foreach my $replicant (ReadSamplesFileMkII($UNC,$Cookie,-1)) {
      my ($infected, $goat, $length) = ($$replicant[$iPath],$$replicant[$iGoatName],(split /-/,$$replicant[$iAttributes])[0]);
      $length = 0 if $length =~ m<^none$>i;
      
      next if $infected =~ m</sectors/>i;
      next if $goat =~ m<^none$>i;   #?? or $infMethod =~ m<^none$>i or $target !~ m[COM|EXE]i)?;

      my $dest = "$destRoot/$infected";
      my $goatPath = "$goatRoot/$goat";
      next unless (-s $goatPath);

      Xcopy ('', "$UNC/$infected", $dest);
      unless (-f $dest) {fail "did not copy replicant to $dest\n";}

      push @$pReplicants, {
         'infected' =>  $dest,
         'goat'     =>  $goatPath
      };

      $minLength = $nReplicants ? min ($minLength, $length) : $length;
      $nReplicants++;

      my $target = fileType($goatPath);
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

   if ($::COPY_CODO_FROM) {Copy("$::COPY_CODO_FROM/$File.sections", $Dir);fail "$Dir/$File.sections not found\n" unless -s "$Dir/$File.sections"; return;}   #??testing

   pushdir($Dir);
   Wipe glob("$File.codo,$File.sections");

   startNwait ($::Codo, " -f \"$File\" -b -wow 1 -nodebug -timeout $::CODO_TIMEOUT");
   foreach (@::LOG) {
      if (m/error in/) {
         unlink "$File.sections";
         Log "WARN: Bad code-data seg. for $File: $_";
         last;
      }
   }
   Log "WARN: no code-data segregation for $File" unless (-s "$File.sections");

   Wipe glob("$File.codo,*.out");
   popdir();
}



sub Autoverv ($\@$) {
   my ($root,$pReplicants,$optionalArgs) = @_;

   pushdir($root);
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
   
   my $rc = run ($::Autoverv, "autoverv.lst -MinRep $::MIN_REPLICANTS $optionalArgs");

 # done
   
   popdir();
   return $rc;
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



sub MakeDefs ($$\@$\%) {
   my ($UNC,$ers,$pPAMsigs,$testResult,$pAttributes) = @_;
   return unless -s $ers or $testResult eq 'detected';
   
   my $dpk = "$UNC/$isfnNAVdefDelta";
   mkSubdir ($UNC, $isdnNAVdefDelta) or fail "Error $::ERRNO creating delta directory $UNC/$isdnNAVdefDelta\n";

   my $flags = '';
   $flags .= 'D' if $testResult eq 'detected';
   $flags .= 'R' if $testResult eq 'repaired';
   $flags .= 'C' if $pAttributes->{'COM'} > 0;
   $flags .= 'X' if $pAttributes->{'EXE'} > 0;
   $flags .= max($pAttributes->{'length'}, 0);
   return unless $flags =~ m/^[DR](CX|C|X)\d+$/;

   my $sigs = '';
   foreach (@$pPAMsigs) {$sigs .= "$_ ";}

   my $rc = run ("perl -S -w $::Bindefpak", "$flags $sigs" .' <"'.PD($ers).'" >"'.PD($dpk).'"');

   fail "BinDefPak.pl failed with return code $rc\n" if $rc;
   fail "did not generate a defpak file\n" unless -s $dpk;
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
              "UNC=s"       => \$::UNC);
}



sub makeNewWorkDir () {
   $::WKDIR = PD (MkLocalWorkDir($::COOKIE,$::PID,$::UNC))   or fail "Error $::ERRNO creating a work directory\n";
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


sub Copy ($$) {return map {copy $_, $_[1]} glob $_[0];}


sub Xcopy ($$$) {
   my ($opt,$src,$dest) = ($_[0],PD($_[1]),PD($_[2]));
   my $destType = ($dest =~ m<[\*\?][^\//]*$>) ? 'd' : 'f';
  `echo $destType | xcopy $opt "$src" "$dest"`;
}


sub treeSync ($$) {
   my ($src,$dest) = @_;
   Log "treeSync $src > $dest\n"; #??
   foreach (chompList (Xcopy('/s/D/l/h', $dest, $src))) {
      unlink unless m/\d+\s+File\(s\)/;
   }
   Xcopy ('/s/D/h/k/z', "$src/*", "$dest/*");
}



sub testPgmExists (@) {testFileNotEmpty @_};



sub testDirExists (@) {
   foreach (@_) {
      fail "$_ not found\n" unless -d;
   }
}



sub testFileNotEmpty (@) {
   foreach (@_) {
      fail "$_ not found or is empty\n" unless -s;
   }
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

   my $cmd = PD($pgm)." @args";
   Log cwd().">\n   START $cmd\n";

   unless ($::USE_SUBPROCESSES) {
      @::LOG = `$cmd`;
      return ($? >> 8);
   }
   
   if (defined $::RPID) {$::RPID++;} else {$::RPID = 0;}
   
   my $BATname = "$::WKDIR\\RPID$::RPID.BAT";
   open BAT, ">$BATname"                                                                        or return (@::LOG = `$cmd`, $? >> 8);
   print BAT "$cmd >$::WKDIR\\RPID$::RPID.LOG\necho %errorlevel% >$::WKDIR\\RPID$::RPID.RC\n"   or return (@::LOG = `$cmd`, $? >> 8);
   close BAT;
   
   system 'start "Vanalysis" /WAIT /MIN cmd /c '.$BATname;
   
   @::LOG = `type $::WKDIR\\RPID$::RPID.LOG`;
   map {Log $_} @::LOG;
   (my $RC = (`type $::WKDIR\\RPID$::RPID.RC`)[0]) =~ s/\D//g;
   unlink glob "$::WKDIR\\RPID$::RPID.*";
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
   #??temp until new logger in place $::log->screenEcho(1);

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
   Vanalysis_cleanup;
   Log "FAIL: @_";
   hand_off_to_dataflow ($isqnDefer);
   appendToLog() unless $ENV{'VANALYSIS_LOG'};   #?? testing
   exit (8);
}


sub appendToLog () {
   $::log->DESTROY();
   system 'type "'.PD("$::UNC/Vanalysis.log").'" >> "'.PD("$::UNC/avislog0.log").'"'; 
}


sub hand_off_to_dataflow ($) {
   my ($NextQueue) = @_;

   SpecifyStopping ($::COOKIE, $::PID, $::UNC, $NextQueue, "1 of 1");
   DoNext ($::COOKIE, $::PID, $::UNC, $NextQueue, $::PARM);
   WriteSignatureForDataFlow ($::PID, $::UNC);
   Log "VANALYSIS COMPLETED: $NextQueue; \t".sprintf("%8.2f mins elapsed\n",(time-$::STARTTIME)/60);
}



sub min ($$) {return ($_[0] < $_[1] ? $_[0] : $_[1]);}

sub max ($$) {return ($_[0] > $_[1] ? $_[0] : $_[1]);}

sub minstr ($$) {return ($_[0] lt $_[1] ? $_[0] : $_[1]);}

sub maxstr ($$) {return ($_[0] gt $_[1] ? $_[0] : $_[1]);}

sub chompList (@) {
   foreach (@_) {chomp;}
   return @_;
}


#!!?? temporary unit testing ==================================================


sub UnitTest (\@$\@) {
   my ($pReplicantList,$ERSfile,$pSigList) = @_;

   my $mutex = Win32::Mutex->new(0,'BlueICE-BinaryUnitTest');
   fail "Unable to open unit test mutex\n" unless $mutex;
   $mutex->wait();
   
   treeSync ($NAVdefRoot, $::NAV_DIR)    if $::DoTreeSync;   #??temporary unit testing
   
   my $installDir = PAMbuild($ERSfile,@$pSigList);
   my $appid = PAMinstallDefs($installDir);
   return '' unless $appid =~ m/^NAVSDK_\d+$/;
   
   my $testResult = PAMrepTest (@$pReplicantList);
   
   PAMremoveDefs ($appid);
   $mutex->release();
   return $testResult;
}



sub PAMbuild ($\@) {
   my ($ERSfile, $pSigList) = @_;
   my $installDir = "$::NAV_DIR\\update";

   fail "*** no PAM/ERS source $ERSfile\n" unless -f $ERSfile;
   print "WARN: PAM/ERS source $ERSfile is empty\n" unless -s $ERSfile;
   fail "*** no base definitions in $installDir\n" unless -f "$installDir\\virscan2.dat";
   fail "*** no definst utility $::NAV_DIR\\tools\\definst.exe\n" unless -f "$::NAV_DIR\\tools\\definst.exe";
   Log "building PAM defs\n";
   Log `copy "$ERSfile" "$::NAV_DIR\\PAM\\autoverv.ers"`;
   Log `erase "$::NAV_DIR\\PAM\\autoverv.obj" "$::NAV_DIR\\PAM\\autoverv.dat"`;

   # create a PAM signature      #PAMsigs!!
   fail "Cannot find $::VIRUS_DEF\n" unless -s $::VIRUS_DEF;
      
   my @virus_def = `type $::VIRUS_DEF`;
   open VIRUSDEF, ">$::VIRUS_DEF" or fail "Unable to open $::VIRUS_DEF for updating\n";
   
   while (scalar @virus_def > 0 and $virus_def[0] !~ m/^\[SigStart\]/i) {
      print VIRUSDEF shift(@virus_def);
   }
   
   foreach (@$pSigList) {
      my $sig = $_;
      foreach (@virus_def) {
         print VIRUSDEF;
         last if m/^\[String\]/i;
      }
      $sig =~ s/([0-9A-F]{2})/0x$1/ig;
      $sig =~ s/([0-9A-F]{2})(0x)/$1, $2/ig;
      print VIRUSDEF "$sig, DONE\n";
      print VIRUSDEF "[SigEnd]\n";   # assumes the string is the last signature sub-field in $::VIRUS_DEF
   }
 
   print VIRUSDEF "[End]\n";         # assumes the signature is the last group of the definition in $::VIRUS_DEF
   close VIRUSDEF;


   builddefs ("$::NAV_DIR");
   Log (`copy "$::NAV_DIR\\packages\\virscan2.dat" "$installDir\\virscan2.dat"`);
   Log ("PAM defs built...\n");

   if ($::USE_DEBUG_UNIT_TEST) {   # build the debugging PAM simulator
      Log `copy autoverv.ers "$::NAV_DIR\\scaffolding\\autoverv.ers"`;
      Log `del "$::NAV_DIR\\scaffolding\\pam.exe"`;
      my $RC = startNwait ("$::NAV_DIR\\scaffolding\\bld16.bat", "\"$::NAV_DIR\\scaffolding\"");
      Log `type \"$::NAV_DIR\\scaffolding\\log\"`;
      fail "debug unit test build failed\n" unless -s "$::NAV_DIR\\scaffolding\\pam.exe";
   }

   return $installDir;
}


sub builddefs {
   my ($NAVroot) = @_;
   $ENV{'INCLUDE'} = "$NAVroot\\include;$NAVroot\\include\\dx";
   $ENV{'LIB'} = "$NAVroot\\lib;$NAVroot\\lib\\dx";
   $ENV{'QUAKEINC'} = "$NAVroot\\NAVEX";
   $ENV{'SARCBOT'} = "1";
   $ENV{'PLATFORM'} = ".DX";

   pushdir("$NAVroot\\PAM");
   unlink ("found.obj","found.exe",
           "found.dat","autoverv.obj","autoverv.exe","autoverv.dat",
           "prepair.dat","virscan2.dat",
           "sigs.dat","vsigs.dat");

   startNwait "..\\progs\\PAMCOMP", "virus.def exclude.dat sigs.dat";
   startNwait "..\\progs\\PAMSORT", "sigs.dat";
   startNwait "..\\progs\\FIXINDEX", "index.dat sigs.dat";
   print `copy index.dat /B + sigs.dat /B vsigs.dat`;

   startNwait "..\\progs\\ml", "/c $::DEFINE_NODUMP found.asm";
   startNwait "..\\progs\\link", "found.obj,,,,,";
   startNwait "..\\progs\\cexe2bin", "found.exe found.dat";

   startNwait "..\\progs\\ml", "/c $::DEFINE_NODUMP autoverv.asm";
   startNwait "..\\progs\\link", "autoverv.obj,,,,,";
   startNwait "..\\progs\\cexe2bin", "autoverv.exe autoverv.dat";

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



sub PAMrepTest (\@) {
  my ($pReplicantList) = @_;
   
  my $verbose = 1;

  my ($n_tested, $n_detected, $n_repaired, $n_inexact, $n_bad_repairs) = 
     (0,         0,           0,           0,          0             );

  my $repdir = "$::NAV_DIR\\repair";

  foreach my $replicant (@$pReplicantList) {
    next unless $replicant->{'goat'};              # skip original sample(s)
    my $infected = PD($replicant->{'infected'});
    my $original = PD($replicant->{'goat'});

    warn("WARN: No infected file ($infected) for repair checking!\n"), next unless -f $infected;
    warn("WARN: No original file ($original) for repair checking!\n"), next unless -f $original;


    (my $program = $infected) =~ s[.*\\][];
    my $repaired = "$repdir\\$program";
    system "rd /s /q \"$repdir\"";
    system "md \"$repdir\"";
    system "copy \"$infected\" \"$repaired\"";

#!!
Log ("PAM: to be repaired: $repaired --------------------------------\n");
Log (sprintf ("PAM: infected: %s (%d)\n",$infected, -s $infected));
Log (sprintf ("PAM: original: %s (%d)\n",$original, -s $original));
if (defined $::TEST_HOLD) {
  Xcopy ('', $infected, "$::TEST_HOLD\\infected\\*.*");
  Xcopy ('', $original, "$::TEST_HOLD\\original\\*.*");
}
###Log (`fc "$original" "$repaired"`);

    my $result = try_repair($repaired);
    Log ("repair attempt on $repaired: $result\n");
    $n_tested++;

Xcopy ('', $repaired, "$::TEST_HOLD\\repaired\\*.*") if defined $::TEST_HOLD;

  # Check that the repair is good
    if ($result eq 'repaired') {
       $n_detected++;
       map {print if $_} (`fc "$original" "$repaired"`)[0..40]; #??
       my $repair_type = run "perl -w $::TestBinRepair", "\"$repaired\" \"$original\"";
       if ($repair_type == 0) {      #exact
          $n_repaired++;
          Log ("PAM: $program repaired exactly\n") if $verbose;
       }
       elsif ($repair_type == 1) {   #inexact
          $n_repaired++;
          $n_inexact++;
          Log ("PAM: $program repaired inexactly\n") if $verbose;
       }
       else {
          $n_bad_repairs++;
          Log ("PAM: BAD REPAIR on $program\n") if $verbose;
       }
    }elsif ($result eq 'detected') {
       $n_detected++;
       Log ("PAM: detection only on $program\n") if $verbose;
    }else {
       Log ("NO VIRUS DETECTED in $program\n") if $verbose;
    }
  }  # end of 'for all samples'

  Log "PAMrepTest end\n".
      "detected: $n_detected ".
      "repaired: $n_repaired ($n_inexact inexact) ".
      "bad repairs: $n_bad_repairs ".
      "not detected: ".($n_tested - $n_detected)."\n";

      #!! remove repairs from delta if bad repairs
  if ($n_tested > 0) {
     return 'repaired' if $n_repaired == $n_tested;
     return 'detected' if $n_detected == $n_tested;
  }
  return 'failed';
}



sub try_repair ($) {
   my ($infectedFile) = @_;

   if ($::USE_DEBUG_UNIT_TEST) {
      return startNwait("$::NAV_DIR\\scaffolding\\pam.exe", '"'.$infectedFile.'"') == 0 ? 'repaired' : 'clean'; #?? fix to handle detection only
   }else {
      my $RC = startNwait "$::NAV_DIR\\tools\\deftest.exe", "-r \"$infectedFile\"";
      Log ("unit test (RC = $RC): $::LOG[-1]\n");
      return 'repaired' if $::LOG[-1] =~ m/infected with Autoverv; repaired/;
      return 'detected' if $::LOG[-1] =~ m/infected with Autoverv/;
      fail "unit test detected a different virus ($1)!\n" if $::LOG[-1] =~ m/infected with ([^;]*)/;
      return 'clean'    if $::LOG[-1] =~ m/clean/;
      fail "unexpected unit test result\n";
   }
}



sub PAMremoveDefs ($) {
   my ($appid) = @_;
   Log `$::NAV_DIR\\tools\\definst.exe /U:$appid`;
}

#==============================================================================
 #PAMsigs!!
 
sub findPAMsignatures ($) {
   my ($PAMmapFile) = @_;
   my @PAMsigs = ();
   
   my @PAMmap = `type plaintext\\$PAMmapFile`;

   my ($bestSig, $bestScore, $fileCount) = getBestCommonSig(@PAMmap,('COMl','COMs','EXEl','EXEs'));
   ($bestSig, $bestScore, $fileCount) = getBestCommonSig(@PAMmap,('COM','EXE')) unless $bestSig and $bestScore <= $::SIG_WORST_COMN_SCORE;
   if ($bestSig and $bestScore <= $::SIG_WORST_COMN_SCORE) {  #?? score threshold?
      push @PAMsigs, $bestSig;
      return @PAMsigs;
   }
   
   ($bestSig, $bestScore, $fileCount) = getBestCommonSig(@PAMmap,('COMl','COMs'));
   ($bestSig, $bestScore, $fileCount) = getBestCommonSig(@PAMmap,('COM')) unless $bestSig and $bestScore <= $::SIG_WORST_COMN_SCORE;
   if ($fileCount > 0) {
      fail "no good COM sig\n" if !$bestSig or $bestScore > $::SIG_WORST_SCORE;
      push @PAMsigs, $bestSig;
   }
      
   ($bestSig, $bestScore, $fileCount) = getBestCommonSig(@PAMmap,('EXEl','EXEs'));
   ($bestSig, $bestScore, $fileCount) = getBestCommonSig(@PAMmap,('EXE')) unless $bestSig and $bestScore <= $::SIG_WORST_COMN_SCORE;
   if ($fileCount > 0) {
      fail "no good EXE sig\n" if !$bestSig or $bestScore > $::SIG_WORST_SCORE;
      push @PAMsigs, $bestSig;
   }

   return @PAMsigs;
}



sub getBestCommonSig (\@@) {
   my ($pPAMmap,@fileTypes) = @_;
   @fileTypes = sort {$b cmp $a} @fileTypes;
   my ($bestSig, $bestScore, $count) = ('', 0.0, 0);
   my $prevSig = '';
   my $sigCount = 0;

   foreach (sort @$pPAMmap) {
      my ($sig, $type, $encr, $code, $EP, $offset, $length) = split;
      next unless member($type,@fileTypes);
      $count++;
      next if $encr ne 'PLAIN' or $code ne 'CODE' or $EP ne 'T';
      next if $offset > $::MAX_SIG_OPFFSET;

      if ($sig eq $prevSig) {
         $sigCount++;
      }else {
         $prevSig = $sig;
         $sigCount = 1;
      }
      
      if ($type =~ /$fileTypes[0]/ and $sigCount == scalar @fileTypes) {
         my $score = scoreSig ($sig);
         printf ("Sig %6.2f\t$sig".(' ' x (50 - length $sig)).'F'.($code eq 'CODE' ? 'c':'d')."%6d %2d\r",$score,$offset,$length) if lc($ENV{'VANALYSIS_LOG'}) eq 'console';
         if ($score < $bestScore) {
            $bestSig = $sig;
            $bestScore = $score;
            Log sprintf ("Sig %6.2f\t$sig".(' ' x (50 - length $sig)).'F'.($code eq 'CODE' ? 'c':'d')."%6d %2d\n",$score,$offset,$length);
         }
      }
   }
   print ((' ' x 80)."\n");
   return $bestSig, $bestScore, $count;
}                                                        



#??sub getSigOfType (\@$) {
#??   my ($pPAMmap,$typeToGet) = @_;
#??   my ($bestSig, $bestScore, $count) = ('', 0.0, 0);
#??   
#??   foreach (@$pPAMmap){
#??      my ($sig, $type, $encr, $code, $EP, $offset, $length) = split;
#??      next unless $type eq $typeToGet;
#??      next if $encr ne 'PLAIN' or $code ne 'CODE' or $EP ne 'T';
#??      next if $offset > $::MAX_SIG_OPFFSET;
#??      $count++;
#??      my $score = scoreSig ($sig);
#??      printf ("Sig %6.2f\t$sig".(' ' x (50 - length $sig)).($type eq 'EXE' ? 'X':'C').($code eq 'CODE' ? 'c ':'d ')."%6d %2d\r",$score,$offset,$length) if lc($ENV{'VANALYSIS_LOG'}) eq 'console';
#??      if ($score < $bestScore) {
#??         $bestSig = $sig;
#??         $bestScore = $score;
#??         Log sprintf ("Sig %6.2f\t$sig".(' ' x (50 - length $sig)).($type eq 'EXE' ? 'X':'C').($code eq 'CODE' ? 'c ':'d ')."%6d %2d\n",$score,$offset,$length);
#??      }
#??   }
#??   print ((' 'x80)."\n");
#??   return $bestSig, $bestScore, $count;
#??}


sub member ($\@) {
   my ($item,$pSet) = @_;
   map {return 1 if $item eq $_} @$pSet;
   return 0;
}



sub scoreSig ($) {`$::Score -dbr=$::CORPUS_DB -hex=$_[0]`;}


#########################




sub parse_replicants_listX ($$$\@\%) {
   my ($UNC,$destRoot,$goatRoot,$pReplicants,$pAttributes) = @_;
   my ($nReplicants, $minLength, $COM, $EXE) = (0,0,0,0);

   open REPLIST, "$UNC/$isfnSamples" or fail "replicants list $UNC/$isfnSamples not found\n";
   foreach (<REPLIST>) {
      next if m<[^,]*/sectors/>i;

      unless (m<^([^,]+),[^,]*,([^,]+),[^,]*,(-?\d+|none)-[^-]+-[^-]+-([^-]+)-([^,]+),[^,]*,[^,]*,[^,]*\n$>i) {
         Log "WARN: invalid line in $UNC/$isfnSamples: $_";
         next;
      }
      my ($infected, $goat, $length) = ($1,$2,$3);   #?? ($target, $infMethod) = ($4,$5)? 
      $length = 0 if $length =~ m<^none$>i;
      
      next if ($goat =~ m<^none$>i);   #?? or $infMethod =~ m<^none$>i or $target !~ m[COM|EXE]i)?;

      my $dest = "$destRoot/$infected";
      my $goatPath = "$goatRoot/$goat";
      next unless (-s $goatPath);

      Xcopy ('', "$UNC/$infected", $dest);
      unless (-f $dest) {fail "did not copy replicant to $dest\n";}

      push @$pReplicants, {
         'infected' =>  $dest,
         'goat'     =>  $goatPath
      };

      $minLength = $nReplicants ? min ($minLength, $length) : $length;
      $nReplicants++;

      my $target = fileType($goatPath);
      if    ($target eq 'COM') {$COM++;}
      elsif ($target eq 'EXE') {$EXE++;}
      else                     {fail "Goat $goatPath is of invalid type: $target\n";}
   }
   close REPLIST;

   $pAttributes->{'count'}  = $nReplicants;
   $pAttributes->{'length'} = $minLength;
   $pAttributes->{'COM'}    = $COM;
   $pAttributes->{'EXE'}    = $EXE;
}



sub XLog ($) {
   my ($txt) = @_;
   chomp $txt;
   $::log->logString($txt);
}



sub XstartLog ($) {
   my ($LogRoot) = @_;

   $::log = new AVIS::Logger($LogRoot);
   $::log->screenEcho(1);

   my $captureFile = $ENV{'VANALYSIS_LOG'} ? $ENV{'VANALYSIS_LOG'} : "$LogRoot/Vanalysis.log";
   unless ($captureFile =~ m/^console$/i) {
      $::log->localFile($captureFile);
      $::log->capture(*STDOUT,*STDERR);
   }
}

