####################################################################
#                                                                  #
# Program name:  PM/AVIS/Local/Local.pm                            #
#                                                                  #
# Module name:   Local.pm                                          #
#                                                                  #
# Description:   Defines global variables on the local node.       #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1999                      #
#                                                                  #
# Author:        Morton Swimmer                                    #
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

package AVIS::Local;
require Exporter;
#use Config;
#use AVIS::IceBase;

@ISA = qw(Exporter);
@EXPORT = qw($isdnRun $isdnInfected $isdnSample $isdnSectors
             $isfnVRun $isdnVGoats $isdnVImages $isvalSigLength
             $isdnWorkRoot $isfnGeneralLF $isfnStatusFile $isfnAttributes
             $isdnSigRes $isdnVervRes $isfnSamples $isvalVMaxRepProc
             $isvalVReqCOM $isvalVReqEXE $isvalVReqPol $isfnVRepStat
             $isfnVidesLog $isfnVRepLog $isfnVEmuLog $isdnRootDrive
             $isdnROOT $isvalMaxPolySample $isdnServer $isdnServerRoot $isdnGraveyard
             $isdnBin $isdnBinDir $isdnData $isdnDataDir $isdnVResources
             $isfnCompare $isfnRead $isfnWrite $isfnPatchSec @isvaltype
             @isvalstrategy $isdnScan1dir $isdnScan2dir $isdnScan3dir
             $ignoreMacroNames $isdnNav $isdnNavroot $isqnClassifier
             $isqnSerializeBuild
             $isqnMrepctrl $isqnBrepctrl $isqnMrep $isqnBrep $isqnManalysis
             $isqnBanalysis $isqnDefer $isqnReturn $isqnEnd $isdnMConfigTop
             $isfnMbootlist $isfnComAttributes $isfnDFStartSig
             $isdnCorpusDB
             $isdnNAVdefDelta $isfnNAVdefDelta $isvalBinary $isdnTempScan
             $ismnMassCopyGoatMacroName
             $tempBaseDir $toolsBaseDir $db2Command $db2UserID $db2Password
             $db2DSN $isqnSARCExit $isfnDeltaPathName $isfnSamplePathName
             $isdnDefDirectoryName
);

@EXPORT_OK = @EXPORT;

#new stuff

# The names of the dataflow queues

#MQSeries queue names.
#$isqnClassifier                = "Q.CLASSIFIER";
#$isqnMrepctrl                  = "Q.M.REPLICATION_CONTROLLER";
#$isqnBrepctrl                  = "Q.V.REPLICATION_CONTROLLER";
#$isqnMrep                         = "Q.M.REPLICATOR";
#$isqnBrep                         = "Q.V.REPLICATOR";
#$isqnManalysis         = "Q.M.ANALYSIS";
#$isqnBanalysis         = "Q.V.ANALYSIS";
#$isqnDefer                        = "Q.DEFER";
#$isqnReturn                    = "Q.RETURN";
#$isqnTest                         = "Q.TEST";
#$isqnEnd                          = "Q.End";

#Real dataflow queue names.
$isqnClassifier    = "Classifier";
$isqnMrepctrl                   = "MacroController";
$isqnBrepctrl                   = "BinaryController";
$isqnMrep                          = "MacroReplicator";
$isqnBrep                          = "BinaryReplicator";
$isqnManalysis                  = "MacroAnalysis";
$isqnBanalysis                  = "BinaryAnalysis";
$isqnSerializeBuild         = "SerializeBuild";
$isqnDefer                         = "Deferrer";
$isqnReturn                        = "ReturnQueue";
$isqnTest                          = "Test";
$isqnEnd                              = "End";
$isqnSARCExit                      = "SARCExit"; 
$isfnDeltaPathName                = "DeltaPathName";
$isfnSamplePathName                = "SamplePathName";  
$isdnDefDirectoryName              = "DefinitionDirectoryName";


# commonly-used directories and stuff

$isdnServer          = "\\\\rushmore";
$isdnRootDrive       = (defined $ENV{'ISDN_ROOT'} ? $ENV{'ISDN_ROOT'} : "D:");

$isdnROOT            = eval {
    if ($^O eq 'os2' || $^O eq 'msdos' || $^O eq 'MSWin32') {
      return "$isdnRootDrive";
    }
    elsif (lc($^O) eq 'aix' || lc($^O) eq 'linux') {
      return (defined $ENV{'ISDN_ROOT'} ? $ENV{'ISDN_ROOT'} : $ENV{'HOME'});
    }
    return undef;
};

$isdnServerRoot      = "$isdnServer\\samplestorage";

$isdnBin             = "bin";
$isdnData            = "data";
$isdnNav             = "nav";
$isdnNavDefBuild     = "DefBuild";
# Directory containing binary executables
$isdnBinDir          = "$isdnROOT/$isdnBin";
# Directory containing constant data
$isdnDataDir         = "$isdnROOT/$isdnData";
# Directory containing other data files for VIDES
$isdnVResources      = "$isdnROOT/$isdnData/vresources";
# Directory for archiving analysis data on server
$isdnGraveyard       = "$isdnServer\\grave";

# the corpus
$isdnCorpusDB        = "$isdnROOT/corpusDB";
$isdnCorpusDB        =  $ENV{CORPUS_DB}           unless -d $isdnCorpusDB;
$isdnCorpusDB        = "$isdnServer\\corpusDB"    unless -d $isdnCorpusDB;


# location of the generated NAVdef deltas
$isdnNAVdefDelta     = "navdefs";                   # relative to UNC
$isfnNAVdefDelta     = "$isdnNAVdefDelta/delta";

# Directory containing the NAV local build
$isdnNavDefBuildRoot = "$isdnROOT/$isdnNavDefBuild";

# Directory containing NAV itself, for scanning with official defs
$isdnNavroot         =  "$isdnROOT/$isdnNav";

# Directories with the different scanners
$isdnScan1dir        = "$isdnROOT/fprot";
$isdnScan2dir        = "$isdnROOT/scan";
$isdnScan3dir        = "$isdnROOT/nav";
$isdnTempScan        = "tempscan";

# should we produce macro-virus defs that ignore macro names?
$ignoreMacroNames = 0;
# should we skip the mrep.pl import-export FTP hacks on this machine?
$skip_ftp_hacks = 0;
# the name of the macro used to detect mass copiers
$ismnMassCopyGoatMacroName = "Goat5817";
# constant for the directory of runs, in particular replication runs. (ALL)
$isdnRun             = "run";
# constant for the subdirectory underneath the $unc/$isdnRun/$ProcessID
# directory for the replicants (VIDES and Copelius.)
$isdnInfected        = "infected";
# constant for the directory holding the original sample either in
# $unc or $unc/$isdnRun/$ProcessID (VIDES, and pre-database solutions
# of Copelius)
$isdnSample          = "sample";
# constant for subdirectory underneath $unc/$isdnRun/$ProcessID for
# sectors captured by the replication process (VIDES).
$isdnSectors         = "sectors";
# constant for the name of the file containing the list of runs for VIDES2
$isfnVRun            = "vrun.lst";
# name of central log file
$isfnStatusFile      = "$isdnServerRoot/status.log";
# name of local general log file
$isfnGeneralLF       = "general.log";
# define name for the sigres directory
$isdnSigRes          = "sigres";
# define name for the Verv files directory
$isdnVervRes         = "vervres";
# constant for file name used to store information on generated samples
$isfnSamples         = "samples.lst";
# Max. Number of replication rounds
$isvalVMaxRepProc    = 1;
# Requested infected COM files for one original sample
$isvalVReqCOM        = 0;
# Requested infected EXE files for one original sample
$isvalVReqEXE        = 0;
# Requested infected files for one original sample (polymorphic)
$isvalVReqPol        = 1000;
# name for the statusfile created by VREP used by VREPCtrl
$isfnVRepStat        = "status.txt";
# name for the logfile created by VREP (contains replication informaton)
$isfnVRepLog         = "vreplog.log";
# name for the logfile created by VIDES (contains replication informaton)
$isfnVidesLog         = "vides.log";
# name for the internal emulator logfile
$isfnVEmuLog         = "bochs.log";
# Attributes Database
$isfnAttributes="CRUMBS.DD";
# Communications attributes database
$isfnComAttributes = "comattr.dd";
# Dataflow start signal file
$isfnDFStartSig = "dfstart.dat";

# VREP enviroments
@isvaltype=(
      "144-120-10M-I386-NONE-MSDOS330EA",
      "144-120-10M-I386-NONE-MSDOS500EA",
      "144-120-10M-I386-NONE-MSDOS622EA");
@isvalstrategy=(
      "FILE-DEFAULT-TSROPENCLOSE",
      "FILE-DEFAULT-TSREXECUTE",
      "FILE-DEFAULT-TSRCREATE",
      "FILE-DEFAULT-DASUBDIR",
      "FILE-MASSREP-TSROPENCLOSE",
      "FILE-MASSREP-TSREXECUTE",
      "FILE-MASSREP-TSRCREATE",
      "FILE-MASSREP-DASUBDIR",
      "BOOT-DEFAULT-ACAMANY",
      "BOOT-MASSREP-MASSREP");

# Decided if binary stuff should be processed 1=yes 0=no
$isvalBinary=1;

# Max number of polymorphen samples
$isvalMaxPolySample  =1000;

# static analyzer, compares goats and possible infected files
$isfnCompare         = "$isdnBinDir/compare";
# program to read sample from diskimage
$isfnRead                = "$isdnBinDir/dimgread";
#program to write sample into diskimage
$isfnWrite               = "$isdnBinDir/dimgwrite";
# program to write image into image
$isfnPatchSec        = "$isdnBinDir/patchsec.exe";


#Mrep environment
$isdnMConfigTop    = "/data/Avis/Macro/Files/Config";
$isfnMbootlist     = $isdnMConfigTop."/Applications/bootlist.cfg";


#old stuff

# get the working platform...
#$main::autoplatform = $Config{osname};

# some globalish variables
#$main::globtemp     = "$main::RootDrive/test/temp";
#$main::temppath     = "$main::globtemp/temp";
$isdnWorkRoot        = "$isdnROOT/worktemp";

# other globalish variables
#$main::maindir      = "$main::ROOT/main";
#$main::videsdir     = "$main::ROOT/vides";
#$main::imagepath    = "$main::ROOT/vides/images-dir"; # change this
# Directory containing VIDES images
$isdnVImages         = "$isdnROOT/$isdnData/vimages";
#$main::toolsdir     = "$main::ROOT/vides/tools-dir";
#$main::bochsdir     = "$main::ROOT/vides/bochs-dir";
#$main::asaxdir      = "$main::ROOT/vides/asax-dir";
#$isdnGoats          = "$isdnROOT/verv/goats";
# Directory for VIDES goats
$isdnVGoats          = "$isdnROOT/$isdnData/vresources/goats";

# Coderunner variables
#$main::cododir      = "$main::ROOT/codo";
#$main::odoexe      = "$main::cododir/codow.exe";
#$main::codata       = "$main::cododir/codata.pl";
#$main::decrypt      = "$main::cododir/decrypt.pl";

# some autosig related thingies...
#$main::autosigdir   = "$main::ROOT/sig";
#$main::sigtemppath  = "$main::temppath/sigtemp";

# Verv directory and binary
#$main::autovervdir  = "$main::ROOT/verv";
#$main::autovervbin  = "$main::autovervdir/vervw.exe";
#$main::statbytes2   = "$main::autovervdir/statbytes2.pl";

# CodeRunner temp directory
#$main::tempcododir  = "$main::temppath/temp_codo";

# Vides related global variables
#$main::videsscript  = "$main::videsdir/vides.pl";
#$main::videssamplepath = "$main::temppath/call-vides-samples";

# some corpus autosig related variables
#$main::corpusdir    = "$main::RootDrive/auto/corpus";
#$main::scanfiles    = "g:/scansmall";
#$main::bestsigs     = "$main::RootDrive/auto/corpus/bestsigs.history";

# triage dir thingy
#$main::triagedir    = "/labos2/raw/triage/ntriage/UNKNOWN/done";

# some usefull flexible useless parameters
#$main::CATPROG      = "type";
#$main::BIGCOPY      = "xcopy /E/Q";
#$main::NUL          = "nul:";

#shared global variables
#$main::debug_print  = 0;
#$main::bochs_timeout= "30000000";    # in instructions
#$main::codo_timeout = "20";          # in seconds
#$main::TOHELL_1     = "1>$main::NUL";
#$main::TOHELL_2     = "2>$main::NUL";
#$main::TOHELL_      = "1>$main::NUL 2>$main::NUL";

#$main::siglength       = 20;
$isvalSigLength      = 20;

# from macrocommon

#$main::TryAllLanguages = 0;
#$main::CfgFilesDir  = "$main::mactree/files/config/";
#$main::bootlist     = $main::CfgFilesDir."bootlist.cfg";
#$main::polyminsamples = 25;

#from metacommon

#$statusfile         = "c:/$ENV{USERDOMAIN}status";

#AVIS::IceBase::profile("IceRunner.prf");

# --------------------------------------------------------------
# The following scalar variables are required by the back office
# programs. These values are to be changed between the
# real pilot analysis center and the other test analysis centers
# used by the developers.
# Added by Senthil and Srikant on OCT/26/1999.
# --------------------------------------------------------------
$tempBaseDir = $isdnBinDir;
$toolsBaseDir = "$isdnROOT\\tools";
$db2Command = "c:\\sqllib\\bin\\db2cmd.exe";
$db2UserID = "db2admin";
$db2Password = "hicl";
$db2DSN = "AVISDATA";


1;

