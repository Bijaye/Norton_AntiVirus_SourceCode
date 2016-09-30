#########################################################################
#                                                                  					#
# Program name: unittest.pl                                      				#
#                                                                					#
# Description:   unit test the definitions produced by analysis center.            		#
#                                                                  					#
#                                                                  					#
# Statement:     Licensed Materials - Property of IBM              			#
#                                                                  					#
# Author:       Raju Pavuluri                                        				#
#                                                                  					#
#                U.S. Government Users Restricted Rights - use,    			#
#                duplication or disclosure restricted by GSA ADP   			#
#                Schedule Contract with IBM Corp.                 				#
#                                                                  					#
# Change history:                                                  				#
#   1999/07/19 - First version complete                            				#
#   1999/08/06 - Added MacrodB thingy to get replicants				#
#   1999/08/16 - Deleted extra arguments to Unserializebuild			#
#   1999/09/02 - Additions for Binary tests					#
#########################################################################

use Getopt::Long();            # For parameter parsing
use DataFlow();          # For talking to Dataflow
use AVIS::Macro::Macrodb();    # For getting the replicant list
use AVIS::Macro::Qdtest();        # For quick unit test of macro viruses
use AVIS::Macro::Qdbintst();     # For quick unit test of binary viruses
use AVIS::FSTools();           # Random annoying utilities
use AVIS::Local();             # System globals
use AVIS::Logger();            # A logging object
use File::Path();

my $started = 1;            # Just so the END block can exit if -c


#
# Other globals
#
$test_done = 0;             # Must be set to 1 before normal exit; see END
$nextevent  = "";
$nextparam = "";
$virusID = 0;
$virusName = "";
$success = 0;
$infectedFiles = [];            ##array to hold the infected file names - to pass to unit test
#
# Get the parameters that were passed to us from Dataflow
#
Getopt::Long::GetOptions("Cookie=s" => \$Cookie,
			 "ProcessId=i" => \$ProcessID,
			 "Parameter=s" => \$Parameter,
			 "UNC=s" => \$unc);


# Make the passed UNC canonical, just in case
$unc = AVIS::FSTools::Canonize($unc);

# Get a logging object
$logger = new AVIS::Logger($unc);
$logger->screenEcho(1);

##Check whether this is a test for binary viruses or macro viruses
$binflag = 0;
my ($defType,@replicants);
my ($tryRepair) = 0;

if (-e "$unc\\replicants.lst") {
	$binflag = 1;
	$replicantLst = "$unc\\replicants.lst";
}

if ($binflag) {
	###this is a binary virus test
	$logger->logString("Starting to do Binary virus test");
	eval `type $replicantLst`;  ##Get stuff from replicants.lst file
	$tryRepair = ($defType eq 'repair');
	@$infectedFiles = @replicants;
} else {
	###this is a macro virus test	
	#
	# Get a database-access object, and read the list of replicants from it
	#
	$logger->logString("Starting to do Macro virus test");
	$dbobject = new AVIS::Macro::Macrodb($Cookie,$unc,$ProcessID,$Parameter);
	$dberr = $dbobject->LastError();
	if ("" ne $dberr) {
  		$errmsg = "Error opening database: $dberr";
 		 $logger->logString($errmsg);
  		die "$errmsg  Stopped ";
	}
	###You can get the virus name too... let's handle it in the next version
	####$virusname = $dbobject->VirusName;

	$orgfname = $dbobject->OriginalFileName;
	$reps = $dbobject->Replications();
	# NOTE: assumes that mrepctrl.pl has already screened out files that
	# did not have any macros added to them during replication, or that
	# otherwise are probably not infected.
	foreach $thisrep ( @$reps ) {
 		 push @$infectedFiles, AVIS::FSTools::Canonize($thisrep->{After},$unc);
	}
	# Pass along the original sample, as well
	push @$infectedFiles, AVIS::FSTools::Canonize($orgfname,$unc);
}##endif

#Deal with the parameter list

($UnitTestType,$DefSequenceNumber,$DefDirectoryName) = split (" ",$Parameter);

$logger->logString("Starting: $0");
$logger->logString("unittest: Cookie = $Cookie, ProcessID = $ProcessID, UNC = $unc, UnitTestType = $UnitTestType, DefSequenceNumber = $DefSequenceNumber, DefDirectoryName = $DefDirectoryName");

#Make sure the def directory exists
 if (not -d $DefDirectoryName) {
    $errmsg = "$DefDirectoryName directory does not exist!!";
    $logger->logString($errmsg);
    die "$errmsg  Stopped ";    # kick out to END routine
  }

##Now do the test
if ($binflag) { 
	##do the binary test
	$thisQdbintst = new AVIS::Macro::Qdbintst($DefDirectoryName,$infectedFiles);
	$thisQdbintst->tryRepair($tryRepair);
	$thisQdbintst->logObject($logger);
	$success = $thisQdbintst->doTest();
	if ($success) {
		$logger->logString("New definitions passed unit test");
		$virusID = $thisQdbintst->virusID();
		$virusName = $thisQdbintst->virusName();
	} else {
		$logstring = $thisQdbintst->logString();
		$errmsg = "Unit test failed: $logstring";
		$logger->logString($errmsg);
	}##endif
} else {
	# do the macro test
	$thisQdtest = new AVIS::Macro::Qdtest($DefDirectoryName,$infectedFiles);
	$thisQdtest->logObject($logger);
	$success = $thisQdtest->doTest();

	if ($success) {
		$logger->logString("New definitions passed unit test");
		$virusID = $thisQdtest->virusID();
		$virusName = $thisQdtest->virusName();
	} else {
		$logstring = $thisQdtest->logString();
		$errmsg = "Unit test failed: $logstring";
		$logger->logString($errmsg);
	}##endif
} ##endif

#
# Schedule the next event for Dataflow
#
if ($UnitTestType eq "Incremental") {
	if ($success) {
		$nextevent = "$AVIS::Local::isqnSARCExit";
		$nextparam = "$AVIS::Local::isdnDefDirectoryName FullDefBuilder $DefDirectoryName Full";
	} else {
		$nextevent = "UnSerializeBuild";
		$nextparam = "Deferrer";
	}
}elsif ($UnitTestType eq "Full") {
	if ($success) {
		$nextevent = "UnSerializeBuild";
		$nextparam = "ArchiveSample";
	}else {
		$nextevent = "CriticalError";
	}
}
$logger->logString("unittest: Calling next dataflow event with $Cookie, $ProcessID, $unc,$nextevent,$nextparam");
DataFlow::DoNext ($Cookie, $ProcessID, $unc,$nextevent,$nextparam);
DataFlow::WriteSignatureForDataFlow($ProcessID,$unc);

#set the flag to done
$test_done = 1;
exit;


END {
  exit if not $started;
  if ($test_done) {
#   Do any last-minute normal cleanup in here.
    $logger->logString("unit test finished normally.");
  } else {
           if ($UnitTestType eq "Incremental") {
	## nothing critical here, just defer it
	$nextevent = "UnSerializeBuild";
	$nextparam = "Deferrer";
           } else {
	##  Signal a terrible unexpected error, and clean up, here.
	$nextevent = "CriticalError";
           }
           DataFlow::DoNext ($Cookie, $ProcessID, $unc,$nextevent,$nextparam);
           DataFlow::WriteSignatureForDataFlow($ProcessID,$unc);
           $logger->logString("unittest did not finish normally...");
  }
  $logger->logString("Finishing: $0");
  undef $logger;
}

