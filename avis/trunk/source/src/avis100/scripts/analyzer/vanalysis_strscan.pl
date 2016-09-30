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
use Getopt::Long;
use DataFlow;
use File::Path;
use File::Copy;
use AVIS::FSTools;
use AVIS::Local;
use AVIS::Bytes2;
use AVIS::Logger;
use English;
use strict;

#= Declarations ===============================================================

#   Vanalysis ($UNC,$Cookie,$ProcessID)    - the main analysis routine
sub Vanalysis ($$$);
sub Vanalysis_cleanup ();             #    - remove work files etc.

#   get_original_sample ($srcDir,$destDir,\@replicants)
sub get_original_sample ($$\@);

#   parse_replicants_list ($UNC,$DestRoot,$GoatRoot,\@replicants, \%attributes)
sub parse_replicants_list ($$$\@\%);

#   append_fake_signature ($infectedFile,$fake_signature)
sub append_fake_signature ($$);

#   code_data_segregation (\@replicants)   - applied to a list of replicants
sub code_data_segregation (\@);

#   Codo ($FilePath) - code-data segregation applied to a single file
sub Codo ($);

#   Autoverv ($root,@replicants,$optionalArgs)
sub Autoverv ($\@$);

#   RemoveExcludedSamples ($ExclusionListFile, \@replicants)
sub RemoveExcludedSamples ($\@);

#   testCodata (@replicants)
sub testCodata (\@);

#   get_analysis (\@replicants,$ERS,$infectionMap,$minGoodBytes)
sub get_analysis (\@$$$);

#   decrypt ($FilePath)
sub decrypt ($);

#   get_decrypted_infection_map (\@replicants,$infectionMap)
sub get_decrypted_infection_map (\@$);

#   write_vides_results (%pReplInfo)
sub write_vides_results (\%);

#   Autosig ($ConstBytesFile,$pReplInfo,$GlblCorpCntDir,$Corpus,$Mode,$Database,$sig_hist)
sub Autosig ($\%$$$$$);

#   Ngrams ($ConstBytesFile,$pReplInfo,$GlblCorpCntDir,$Corpus,$Mode,$Database)
sub Ngrams ($\%$$$$);

#   extract_signatures ($Mode)
sub extract_signatures ($);

#   score_signatures ($bestsigs_history)
sub score_signatures ($);

#   MakeDefs ($UNC,$ERSfile)
sub MakeDefs ($$);

#   fileType ($fileName)
sub fileType ($);

#   getParameters
sub getParameters();

#   startLog ($LogRoot)
sub startLog($);

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
sub chompList (@);

sub Log ($);

sub makeNewWorkDir ();

#   UnitTest (@Replicants)
sub UnitTest (\@);

#= Initialization =============================================================

getParameters();
startLog($::UNC);
Log (('=' x 80)."\n");
Log "VANALYSIS @ ".$ENV{'COMPUTERNAME'}.": Cookie $::COOKIE, PID $::PID, Parms $::PARM, UNC $::UNC\n\n";
makeNewWorkDir();

#- Default parameters ---------------------------------------------------------

$::Codo                = "$isdnBinDir/codow.exe";
$::Autoverv            = "$isdnBinDir/autoverv.exe";
$::Ngrams              = "$isdnBinDir/ngrams.exe";
$::Ngrams_thorough     = "$isdnBinDir/ngrams_thorough.exe";
$::Extractsig          = "$isdnBinDir/extractsig.exe";
$::Scoresig            = "$isdnBinDir/scoresig.exe";
$::Bindefpak           = "$isdnBinDir/Bindefpak.pl";

$::VIDES_RESULTS       = "vides.results";
$::INFECTION_MAP       = "autoverv.bytes2";
$::ERS_INSTRUCTIONS    = "autoverv.ers";
$::BESTSIG_HISTORY     = "$isdnCorpusRoot/bestsigs.history";



$::CODO_TIMEOUT        =  20;              # code-data segregation timeout in seconds   ?? was 20
$::DECRYPT_TIMEOUT     =  100;
$::MIN_REPLICANTS      =  4;
$::FAKE_SIGNATURE      = 'VIRUS';                #!!
$::MIN_GOOD_BYTES      =  50;                    #?? a function of file length change?
$::SIG_CRYPT_TYPE      = "plain";          # signatures will be sought in const. viral sections encrypted
                                           # this way (undef => any type; "plain" => unencrypted).
$::SIG_CODE_TYPE       = "yes";            # signatures will be sought in const. viral sections having a
                                           # code flag set to this value (undef => any value OK??).
$::NGRAM_MODE          =  5;
$::NGRAM_DB            = "";                     #?? set to the Trie DB if you want to use it?
                                                 #?? - doesn't seem to work yet
$::USE_DEBUG_UNIT_TEST =  0;               # use the test repair pgm, not PAM (more diagnostics). 
$::DEFINE_NODUMP       = '/DNODUMP';       # if defined, the ERS code will, on fail, write debug info 
                                           # into the target file (OK for either PAM or the test pgm).
$::USE_SUBPROCESSES    =  0;               # if true then programs and commands will be STARTed in 
                                           # their own DOS box. 
$::SKIP_SIGS           =  0;               # if true don't attempt to find signatures.
$::KEEP_WORK_DIR       =  0;               # - for post-run analysis.
$::COPY_CODO_FROM      = '';               # - instead of using codow to do code-data segregation.

#- overrides (for testing) ----------------------------------------------------

   my $srvRoot            = "//Elbe/ElbeD";        ###"//Mosel/srvroot/vanalysis";
   my $pgmRoot            = "$srvRoot/auto";
   my $corpusRoot         = "//Elbe/ElbeCorpus";   ### "$srvRoot";
   my $NAVdefRoot         = "$srvRoot/NAV";
   
   $::Codo                = "$pgmRoot/codo/codow.exe";
   $::Autoverv            = "//Shine/TestZoneShine/autoverv/test/autoverv.exe";
   $::Ngrams              = "$pgmRoot/sig/ngrams.exe";
   $::Ngrams_thorough     = "$pgmRoot/sig/ngrams_thorough.exe";
   $::Extractsig          = "$pgmRoot/sig/extractsig.exe";
   $::Scoresig            = "$pgmRoot/sig/scoresig.exe";
   $::Bindefpak           = "//Shine/TestZoneShine/analysis/bindefpak.pl";

   $isfnCorpus            = PD("$corpusRoot/output.cor");
   $isdnTrieDB            = PD("$corpusRoot/trie");

   $isdnCorpusRoot        = "$::WKDIR/corpus_counts";
   $::BESTSIG_HISTORY     = "$isdnCorpusRoot/bestsigs.history";
   Xcopy ('/s', "$pgmRoot/corpus/*", "$isdnCorpusRoot/*");

# run the script specified in the parameter argument
   if (-s $::PARM && !do $::PARM) {fail "$::PARM did not complete\n";}
 
 # temporary unit testing...
    unless (defined $::NAV_DIR) {
      $::NAV_DIR = PD("$isdnROOT/VUT_NAV");
      treeSync ($NAVdefRoot, $::NAV_DIR);
   }

   $::NAVappIDfile        = PD("$isdnROOT/VUT_NAVSDK.APPID");
   $::TestBinRepair       = "$pgmRoot/TestBinRepair/TestBinRepair.pl";

   $::SYS_DRIVE           = $ENV{'SYSTEMDRIVE'};
   fail "%SYSTEMDRIVE% not defined\n" unless $::SYS_DRIVE;

   $::TEST_HOLD           = PD (mkSubdir($::UNC,'testfiles'));   # keep copies of the unit test files
   
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
                  $::Ngrams,
                  $::Ngrams_thorough,
                  $::Extractsig,
                  $::Scoresig,
                  $::Bindefpak);

 # make a working copy of the infected files

   my $original = get_original_sample ("$::UNC/$isdnSample", cwd()."/plaintext/$isdnSample", @replicants);
   $attributes{'original'} = (split m[/],$original)[-1]; #??(reverse(split m[/],$original))[0];   #?? 
   Log "\noriginal sample: $original\n";

   parse_replicants_list ($UNC, cwd().'/plaintext', $isdnVGoats, @replicants, %attributes);
   map {Log "\nreplicant\t$_->{'infected'}\ngoat\t\t$_->{'goat'}\n";} @replicants[1..$#replicants];
   
   fail "Insufficient replicants: ".(@replicants-1)."\n" if @replicants-1 < $::MIN_REPLICANTS;

 # restart?

   goto $::RESTART if $::RESTART;

FAKESIG:# Testing!!: append a fake signature to the infected files

   if ($::FAKE_SIGNATURE) {
      map append_fake_signature($_->{'infected'},$::FAKE_SIGNATURE), @replicants;
   }

CODO: # do the code-data segregation

   code_data_segregation (@replicants);

AUTOVERV: # run Autoverv on the replicants.

   $rc = Autoverv ('plaintext',@replicants,'');
   fail "Autoverv failed with return code $rc\n" if ($rc);
   
   RemoveExcludedSamples (PD('plaintext/autoverv.xcl'), @replicants);

DECRYPT: # get the ERS instructions and the constant viral regions.

   get_analysis (@replicants,$::ERS_INSTRUCTIONS,$::INFECTION_MAP,$::MIN_GOOD_BYTES);
   $::SKIP_SIGS = 1, Log "WARN: Autoverv did not create an infection map\n" unless -s "$::INFECTION_MAP"; #??fail
   
AUTOSIG: # select a signature from the constant viral regions.

   Autosig ($::INFECTION_MAP, %attributes,
            $isdnCorpusRoot, $isfnCorpus, $::NGRAM_MODE, $::NGRAM_DB,
            $::BESTSIG_HISTORY)
   unless $::SKIP_SIGS;

DEFGEN: # generate definitions from the ERS instructions.

   fail "Autoverv did not create ERS instructions\n" unless -s "plaintext/$::ERS_INSTRUCTIONS";
   MakeDefs($UNC,$::ERS_INSTRUCTIONS);

TEST: # unit test

   UnitTest(@replicants);

DONE:
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



sub parse_replicants_list ($$$\@\%) {
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

   open LIST, ">autoverv.lst" or fail "Error $::ERRNO creating autoverv input list file ".cwd()."/autoverv.lst\n";
   foreach my $replicant (@$pReplicants) {
      if ($replicant->{'infected'}) {
         print LIST PD("$replicant->{'infected'},$replicant->{'goat'}\n");
      }
   }
   close LIST;

 # check the code-data segregation quality
   
   my $CodoIsOK = testCodata(@$pReplicants);
   $optionalArgs .= ' -codo' if $CodoIsOK;
   $::SIG_CODE_TYPE = $CodoIsOK ? 'yes' : undef;

 # run autoverv
   
   my $rc = run ($::Autoverv, "autoverv.lst -MinRep $::MIN_REPLICANTS $optionalArgs");

 # done
   
   popdir();
   return $rc;
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



sub get_analysis (\@$$$) {
   my ($pReplicants,$ERS,$infectionMap,$minGoodBytes) = @_;

   $::TRY_REPAIR = 1;   # set false if autoverv cannot generate a good repair.
 
 # always use the plaintext ERS
   
   Copy ("plaintext/$ERS", '.');
   $::TRY_REPAIR = 0 unless -s $ERS;

 # examine the constant viral regions from the Autoverv run.

   my $CVbytes;
   my $goodbytes = 0;
   Copy ("plaintext/$infectionMap", '.');
   if (-s $infectionMap) {
      $CVbytes = AVIS::Bytes2->map($infectionMap);
      $goodbytes = $CVbytes->count({"crypt" => $::SIG_CRYPT_TYPE, "code" => $::SIG_CODE_TYPE});
   }
   Log "INFO: $goodbytes good signature bytes in the plaintext map\n";

 # if there aren't enough constant bytes for a signature, have the virus 
 # decrypt itself and then re-run Autoverv on the result.

   unless ($goodbytes > $minGoodBytes) {
      get_decrypted_infection_map (@$pReplicants, $infectionMap);

      $goodbytes = 0;
      if (-s $infectionMap) {
         $CVbytes = AVIS::Bytes2->map($infectionMap);
         $goodbytes = $CVbytes->count({"crypt" => $::SIG_CRYPT_TYPE, "code" => "yes"});
      }
      Log "INFO: $goodbytes good signature bytes in the decrypted map\n";
   }

 # At this point, we merge the consecutive entries in the viral map
 # if they are separated by less than a signature length. This allows
 # extractsig to provide scoresig with wildcard signatures.

   if ($goodbytes > $isvalSigLength) {
      copy ($infectionMap, "$infectionMap.original"); # Keep a copy
      $CVbytes->mergecloseblocks($isvalSigLength);
      $CVbytes->unmap;  # Flush!
   }
}



sub get_decrypted_infection_map (\@$) {
   my ($pReplicants,$infectionMap) = @_;

   my @decrypted_replicants;

 # copy the plaintext work

   Xcopy ('/s', 'plaintext/*', 'decrypted/*');

 # decrypt the infected samples

   my $nDecrypted = 0;
   foreach my $replicant (@$pReplicants) {
      (my $replPath = $replicant->{'infected'}) =~ s[plaintext/][decrypted/];

      decrypt($replPath);

      if (-f $replPath) {
         $nDecrypted++;
         push @decrypted_replicants, {
            'infected' => $replPath,
            'goat'     => $replicant->{'goat'}
         }
      }
   }
   #?? remove replicants that don't decrypt so they won't be included in the unit test?
   #?? if so, don't need a separate decrypted_replicants list

 # analyse them

   code_data_segregation (@decrypted_replicants);
   my $rc = Autoverv ('decrypted',@decrypted_replicants,'-ImapOnly');

   unless ($rc == 0 and -s "decrypted/$infectionMap" > 0) {
      Log "WARN: decrypted-autoverv failed with return code $rc\n";   #??
   }

   RemoveExcludedSamples (PD('decrypted/autoverv.xcl'), @$pReplicants);

 # get the infection map that it has created.
   Copy ("decrypted/$infectionMap", '.');
}



sub decrypt ($) {
   $_[0] =~ m<^(.*)/([^/]*)$>;
   my ($File,$Dir) = ($2,$1);

   pushdir($Dir);

   run ($::Codo, " -f \"$File\"  -nodebug -decrypt -timeout $::DECRYPT_TIMEOUT");

   unlink $File;
   if (-f "$File.patch") {
      rename ("$File.patch", $File) or fail "Error $::ERRNO renaming decrypted file $File.patch\n";
   }

   popdir();
}



sub write_vides_results (\%) {
   my ($pReplInfo) = @_;
   open VR, ">$::VIDES_RESULTS" or fail "Error $::ERRNO creating vides-results file $::VIDES_RESULTS\n";

   print VR ":BEGIN\n".
            "Name.$pReplInfo->{'original'}\n".
            "Samples.$pReplInfo->{'count'}\n".
            "Sectors.0\n".
            "Size.$pReplInfo->{'length'}\n".
            "COM.".($pReplInfo->{'COM'} ? '1' : '0')."\n".
            "EXE.".($pReplInfo->{'EXE'} ? '1' : '0')."\n".
            "RES.0\n".
            "MBR.0\n".
            "BOO.0\n".
            "FFind.0\n".
            "HFind.0\n".
            "XOpen.0\n".
            "Open.0\n".
            "HLL.0\n".
            ":END\n";

   close VR;
}



sub Autosig ($\%$$$$$) {
   my ($infectionMap,$pReplInfo,$GlblCorpCntDir,$Corpus,$Mode,$Database,$sig_hist) = @_;
   my $rc = 1;

   unlink ('failure.report','warnings.report','bestsigs.lst');

# get the N-gram counts, unless we already have them

   unless (-s "hash.table" and -e "unigrams" and -e "bigrams" and -e "ngram.counts") {
      $rc = Ngrams ($infectionMap,%$pReplInfo,$GlblCorpCntDir,$Corpus,$Mode,$Database);
   }

 # extract the candidate signatures

   $rc = extract_signatures ($Mode) if $rc;

 # score the candidate signatures

   $rc = score_signatures ($sig_hist) if $rc;

   foreach (glob '*.report') {if (-s) {run ('type', PD($_));} else {unlink;}}
   Log "INFO: signature found\n" if $rc;
   
   return $rc;
}



sub Ngrams ($\%$$$$) {
   my ($ConstBytesFile,$pReplInfo,$GlblCorpCntDir,$Corpus,$Mode,$Database) = @_;

 # some messing around due to the fact that Ngrams has a lot of unnecessary
 # built-in assumptions about where things are.

   write_vides_results (%$pReplInfo);   # into the CWD
   my $Ngrams_Wkdir = mkSubdir (cwd(), "V-$pReplInfo->{'original'}")           or fail "Ngrams err $::ERRNO creating ngrams work directory ".cwd()."/V-$pReplInfo->{'original'}\n";
   Copy ($ConstBytesFile, "$Ngrams_Wkdir/V-$pReplInfo->{'original'}.bytes2")   or fail "Ngrams err $::ERRNO copying $ConstBytesFile to $Ngrams_Wkdir/V-$pReplInfo->{'original'}.bytes2\n";

   my $indirect_scanfile = PD("$Ngrams_Wkdir/scanfile.nam");
  `echo $Corpus>"$indirect_scanfile"`;

   copy "$GlblCorpCntDir/global_unigrams",     "unigrams"         or warn "Global unigrams file not found\n";
   copy "$GlblCorpCntDir/global_bigrams",      "bigrams"          or warn "Global bigrams file not found\n";
   copy "$GlblCorpCntDir/global_ngram.counts", "Ngram.counts"     or warn "Global ngram.counts file not found\n";

   my $Ngrams_pgm = (-e "unigrams" and -e "bigrams" and -e "Ngram.counts") ? $::Ngrams : $::Ngrams_thorough;
   my $DBopt      =  -d $Database   ?   ' -l 0 -db "'.PD($Database).'"'   :   ' -l 1';

 # finally, run Ngrams

   my $rc = run ($Ngrams_pgm, " -glob \"".PD($GlblCorpCntDir).'"'.
                              " -scan \"$indirect_scanfile\"".
                              " -m    $Mode".
                              " -d    . ".
                                      $DBopt);

   Log "WARN: Ngrams return code $rc\n" if $rc < 0; #??
   return $rc >= 0;  #??
}



sub extract_signatures ($) {
   my ($Mode) = @_;

   my $rc = run ($::Extractsig, "-l 20 -n 10 -m $Mode -d .");

   Log "WARN: Extractsig failed with return code $rc\n"              if $rc != 0; #?? fail?
   Log "WARN: Extractsig failed to produce candidate signatures\n"   unless -s 'candidate.signatures'; #?? fail?
   return $rc == 0 && -s 'candidate.signatures';
}



sub score_signatures ($) {
   my ($bestsig_history) = @_;

   #?? do I have to copy this?
   copy 'candidate.signatures', 'virsig.lst.20'               or fail "Can't copy candidate.signatures to virsig.lst.20\n";

   my $rc = run ($::Scoresig,  "-sigh \"$bestsig_history\" ".
                               "-v virsig.lst.20 -s score.card.20 -c complete.report.20 ".
                               "-frag 11 -mult 0.75 -const 11 -verv 0");

   Log "WARN: Scoresig failed with return code $rc\n"      if $rc != 0; #?? fail?
   Log "WARN: Scoresig failed to produce bestsigs.lst\n"   unless -s 'bestsigs.lst'; #?? fail?
   return $rc == 0 && -s 'bestsigs.lst';
}



sub MakeDefs ($$) {
   my ($UNC,$ers) = @_;
   return unless -s $ers;
   
   my $dpk = "$UNC/$isfnNAVdefDelta";
   mkSubdir ($UNC, $isdnNAVdefDelta) or fail "Error $::ERRNO creating delta directory $UNC/$isdnNAVdefDelta\n";

   my $rc = run ("perl -S -w $::Bindefpak", '<"'.PD($ers).'" >"'.PD($dpk).'"');

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


sub makeNewWorkDir () {
   $::WKDIR = PD (MkLocalWorkDir($::COOKIE,$::PID,$::UNC))   or fail "Error $::ERRNO creating a work directory\n";
   Wipe glob "$::WKDIR/*"                                    or fail "Error $::ERRNO cleaning the work directory $::WKDIR\n";
}
 

sub mkSubdir ($$) {
   my ($parent,$child) = @_;
   my $path = "$parent/$child";

   if (-d $path) {chmod(0777,$path)      or fail "Error $::ERRNO setting $path mode to 0777\n";}
   else          {mkpath($path,1,0777)   or fail "Error $::ERRNO creating $path\n";}

   Wipe glob("$path/*") unless $::RESTART;
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
   (my @RC = `type $::WKDIR\\RPID$::RPID.RC`) =~ s/\D//g;
   unlink glob "$::WKDIR\\RPID$::RPID.*";
   return $RC[0]; 
}


sub Log ($) {
   my ($txt) = @_;
   print $txt;
   if (defined $::log) {
      chomp $txt;
      $::log->logString($txt);
   }
}


sub fail ($) {
   topdir();
   Vanalysis_cleanup;
   Log "FAIL: @_";
   replaceLog() unless $ENV{'VANALYSIS_LOG'};   #?? testing
   hand_off_to_dataflow ($isqnDefer);
   exit (8);
}


sub replaceLog () {
   $::log->DESTROY();
   Xcopy ('', "$::UNC/Vanalysis.log", "$::UNC/AVISLOG0.LOG");
}


sub hand_off_to_dataflow ($) {
   my ($NextQueue) = @_;

   SpecifyStopping ($::COOKIE, $::PID, $::UNC, $NextQueue, "1 of 1");
   DoNext ($::COOKIE, $::PID, $::UNC, $NextQueue, $::PARM);
   WriteSignatureForDataFlow ($::PID, $::UNC);
}



sub min ($$) {return ($_[0] < $_[1] ? $_[0] : $_[1]);}

sub max ($$) {return ($_[0] > $_[1] ? $_[0] : $_[1]);}

sub chompList (@) {
   foreach (@_) {chomp;}
   return @_;
}


#!!?? temporary unit testing ==================================================


sub append_fake_signature ($$) {
   my ($file,$sig) = @_;
   open FS, ">>$file"        or fail "Error $::ERRNO opening $file to append fake signature\n";
   binmode FS;
   print FS $sig             or fail "Error $::ERRNO appending fake signature to $file\n";
   close FS;
}



sub UnitTest (\@) {
   PAMbuild();

   my ($pReplicantList) = @_;

   my @replicants = ();
   my @originals  = ();
   #??
   #open AVLIST, "plaintext/autoverv.lst" or fail "Error $::ERRNO opening plaintext/autoverv.lst\n";
   #foreach (<AVLIST>) {
   #   next unless m<^([^,]+),([^,]+)\n$>;
   #   push @replicants, PD($1);
   #   push @originals,  PD($2);
   #}
   #close AVLIST;
   foreach my $replicant (@$pReplicantList) {
      next unless $replicant->{'goat'};              #skip original sample(s)
      push @replicants, $replicant->{'infected'};
      push @originals,  $replicant->{'goat'};
   }

   my ($repair_OK, $n_repaired, $n_bad_repairs, $n_repair_not_tried, $n_inexact, $n_not_detected)
      = PAMrepTest (\@originals,\@replicants);
   Log "repaired: $n_repaired ($n_inexact inexact) ".
        "bad repairs: $n_bad_repairs ".
        "not tried: $n_repair_not_tried ".
        "not detected: $n_not_detected\n";
   unless ($repair_OK) {fail "failed unit test\n";}

   Log "INFO: passed unit test\n";
   return $repair_OK;
}



sub PAMbuild () {
   my $appid = '';

   return unless $::TRY_REPAIR;

   fail "*** no PAM/ERS source $::ERS_INSTRUCTIONS\n" unless -s $::ERS_INSTRUCTIONS;
   fail "*** no base definitions in $::NAV_DIR\\update\n" unless -f "$::NAV_DIR\\update\\virscan2.dat";
   fail "*** no definst utility $::NAV_DIR\\tools\\definst.exe\n" unless -f "$::NAV_DIR\\tools\\definst.exe";
   Log "building PAM defs\n";
   Log `copy "$::ERS_INSTRUCTIONS" "$::NAV_DIR\\PAM\\autoverv.ers"`;
   Log `erase "$::NAV_DIR\\PAM\\autoverv.obj" "$::NAV_DIR\\PAM\\autoverv.dat"`;

   makedefs ("$::NAV_DIR");
   Log ("PAM defs built...\n");

   Log (`copy "$::NAV_DIR\\packages\\virscan2.dat" "$::NAV_DIR\\update\\virscan2.dat"`);
   if (-s $::NAVappIDfile) {
      my $appid = `type "$::NAVappIDfile"`;
      Log (`$::NAV_DIR\\tools\\definst.exe /U:$appid}`);
   }
   run ('rd', '/s /q "'.$::SYS_DRIVE.'\\Program Files\\Common Files\\Symantec Shared\\VirusDefs"');
   
   startNwait "$::NAV_DIR\\tools\\definst.exe", "\"$::NAV_DIR\\update\""; #??foreach (`$::NAV_DIR\\tools\\definst.exe "$::NAV_DIR\\update"`) { Log ($_);
   foreach(@::LOG) {
      $appid = $ENV{"NAVSDK_APPID"} = $1 if (m/(NAVSDK_\d+)/);
   }
  `echo $appid >"$::NAVappIDfile"`;
   Log ("PAM defs installed\n");


   if ($::USE_DEBUG_UNIT_TEST) {   # build the debugging PAM simulator
      Log `copy autoverv.ers "$::NAV_DIR\\scaffolding\\autoverv.ers"`;
      Log `del "$::NAV_DIR\\scaffolding\\pam.exe"`;
      my $RC = startNwait ("$::NAV_DIR\\scaffolding\\bld16.bat", "\"$::NAV_DIR\\scaffolding\"");
      Log `type \"$::NAV_DIR\\scaffolding\\log\"`;
      fail "debug unit test build failed\n" unless -s "$::NAV_DIR\\scaffolding\\pam.exe";
   }
}


sub makedefs {
   my ($NAVroot) = @_;
   $ENV{'INCLUDE'} = "$NAVroot\\include;$NAVroot\\include\\dx";
   $ENV{'LIB'} = "$NAVroot\\lib;$NAVroot\\lib\\dx";
   $ENV{'QUAKEINC'} = "$NAVroot\\NAVEX";
   $ENV{'SARCBOT'} = "1";
   $ENV{'PLATFORM'} = ".DX";

   pushdir("$NAVroot\\PAM");
   unlink ("found.obj","found.exe",
           "found.dat","autoverv.obj","autoverv.exe","autoverv.dat",
           "prepair.dat","virscan2.dat");

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



sub PAMrepTest (\@\@) {
  my ($orig_ref,$repl_ref) = @_;
  return unless (ref($orig_ref) eq "ARRAY" and ref($repl_ref) eq "ARRAY");

  Log ("\nPAMrepTest\n");
  my $verbose = 1;

  my ($repair_OK, $n_repaired, $n_bad_repairs, $n_repair_not_tried, $n_inexact, $n_not_detected) = (1,0,0,0,0,0);

  my $repdir = "$::NAV_DIR\\repair";

  for (my $i = 0; $i < @$repl_ref; $i++) {
    my $infected = PD($repl_ref->[$i]);
    my $original = PD($orig_ref->[$i]);

    warn(" *** No infected file ($infected) for repair checking!\n"), next unless -f $infected;
    warn(" *** No original file ($original) for repair checking!\n"), next unless -f $original;


    (my $program = $infected) =~ s[.*\\][];
    my $repaired = "$repdir\\$program";
    system "rd /s /q \"$repdir\"";
    system "md \"$repdir\"";
    system "copy \"$infected\" \"$repaired\"";

Log ("PAM: to be repaired: $repaired --------------------------------\n");
Log ("PAM: infected: $infected\n");
Log ("PAM: original: $original\n");
if (defined $::TEST_HOLD) {
  Xcopy ('', $infected, "$::TEST_HOLD\\infected\\*.*");
  Xcopy ('', $original, "$::TEST_HOLD\\original\\*.*");
}
###Log (`fc "$original" "$repaired"`);

   my $result = try_repair($repaired);
   Log ("repair attempt on $repaired: ". ($result ? $result : 'Failed')."\n");

Xcopy ('', $repaired, "$::TEST_HOLD\\repaired\\*.*") if defined $::TEST_HOLD;

    # Check that the repair is good
    if ($result eq 'repaired') {
       system "fc \"$original\" \"$repaired\"";
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
          $repair_OK = 0;
          $n_bad_repairs++;
          Log ("PAM: BAD REPAIR on $program\n") if $verbose;
       }
    }
    elsif ($result eq 'clean') {
       $repair_OK = 0;
       $n_not_detected++;
       Log ("NO VIRUS DETECTED in $program\n") if $verbose;
    }else {
       $repair_OK = 0;
       $n_repair_not_tried++;
       Log ("PAM: NO REPAIR tried on $program\n") if $verbose;
    }
  }

  $repair_OK = 0 unless $n_repaired > 0;

  Log ("PAMrepTest end\n");
  return ($repair_OK, $n_repaired, $n_bad_repairs, $n_repair_not_tried, $n_inexact, $n_not_detected);
}



sub try_repair ($) {
   my ($infectedFile) = @_;

   Log ("No repair to test\n"), return '' unless $::TRY_REPAIR;

   if ($::USE_DEBUG_UNIT_TEST) {
      return startNwait("$::NAV_DIR\\scaffolding\\pam.exe", '"'.$infectedFile.'"') == 0 ? "repaired" : "";
   }else {
      my $RC = startNwait "$::NAV_DIR\\tools\\deftest.exe", "-r \"$infectedFile\"";
      Log ("unit test RC: $RC\n");
      return 'repaired' if $::LOG[-1] =~ m/infected with Autoverv; repaired/;
      return 'clean'    if $::LOG[-1] =~ m/clean/;
      return '';
      #??my $result = `$::NAV_DIR\\tools\\deftest.exe -r "$infectedFile"`;
      #??Log ("unit test RC: $?\n");
      #??return $result =~ m/infected with Autoverv; repaired/ ? "repaired" : "";
   }
}


#==============================================================================


package dirstack;
use Cwd;

sub ::changedir ($) {
   my ($dest) = @_;
   `$1` if $dest =~ /^([a-zA-Z]\:)/;
   chdir $dest;
}

sub ::pushdir ($) {
   push @dirstack::STACK, cwd();
   ::changedir ($_[0]);
}

sub ::popdir () {
   return 0 unless scalar @dirstack::STACK;
   ::changedir (pop @dirstack::STACK);
}

sub ::topdir () {
   my $rc = scalar @dirstack::STACK ? ::changedir ($dirstack::STACK[0]) : 1;
   @dirstack::STACK = ();
   return $rc;
}


#==============================================================================
