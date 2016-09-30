###############################################################
# SarcExit.pl
# 
# This program allows termination of the analysis path on
# special conditions determined by SARC.
#
#
# Authors: Bill Arnold, Senthil Velayudham, Srikant Jalan
# Creation Date: Feb/11/2000
# Last Modified: Feb/16/2000
#
###############################################################

# Specify the PERL modules used by this program.
use Getopt::Long;
use DataFlow;
use strict;
use AVIS::Local;
use AVIS::Logger;

# Parse the command line arguments from @ARGV.
my ($Cookie, $ProcessID, $Parameter, $unc, $result);
my ($exportParameter, $logfile, $deltaPathName, $definitionDirName, $buildType, $samplePathName);

GetOptions("Cookie=i" => \$Cookie,
	    "ProcessId=i" => \$ProcessID,
	    "Parameter=s" => \$Parameter,
	    "UNC=s" => \$unc);

$logfile = new AVIS::Logger($unc);
$logfile->logString("Starting: $0");
$logfile->logString("Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc");

# $Parameter will have the values dataType and the nextService separated by a space.
# When the SARCExit program is launched after the manalysis and incremental unit test,
# $Paramter will have a 3rd token corresponding to the deltaPathName and the
# definitionDirName respectively.
my ($dataType,$nextService, $temp1, $temp2) = split /\s/,$Parameter;

$logfile->logString("Data Type = $dataType, Next Service = $nextService");

if ($dataType eq "$isfnSamplePathName")
{
    $samplePathName = "$unc\\Sample\\Sample.Original";
    $logfile->logString("SamplePathName = $samplePathName");
}
if ($dataType eq "$isfnDeltaPathName")
{
    $deltaPathName = $temp1;
    $logfile->logString("DeltaPathName = $deltaPathName");
}
elsif ($dataType eq "$isdnDefDirectoryName")
{
    $definitionDirName = $temp1;
    $buildType = $temp2;
    $exportParameter = $buildType;
    $logfile->logString("DefinitionDirName  = $definitionDirName, BuildType = $buildType");
}

# Call the SARCExit function provided by Symantec.
SARCExit ();

$logfile->logString("Next Service = $nextService, ExportParameter = $exportParameter");


# Tell the dataflow what to do next, ie. which queue to send stuff
# to. We are free to set the $Parameter, but $Cookie, $ProcessID and
# $unc MUST be passed through (although we can use the values.)
DataFlow::DoNext($Cookie,
                 $ProcessID,
                 $unc,
                 $nextService,
                 $exportParameter);         
WriteSignatureForDataFlow ($ProcessID, $unc);

$logfile->logString("Finishing: $0");
exit;



########################################################################
# This subroutine contains code written by SARC in order to provide
# special exit points during the automated analysis of a sample.
# Currently, the analysis pipeline will call SARCExit at the following
# 3 places.
#
# 1) Before a sample is sent to the Classifier.
#    (SARC can verify the sample to be analyzed at this point)
# 2) Before the SDGS lock is to be obtained. 
#    (SARC can verify the defintion delta file at this point)
# 3) Before a sample is sent to the FullDefBuilder.
#    (SARC can verify the output of the incremental build at this point before
#     we commit the delta file to the full build)
#
# If any of the 3 checkpoints fail and if SARC decides to teminate the
# automated analysis, this function will set the $nextService = "Deferrer".
# Otherwise, this function will simply exit without changing the 
# $nextService variable.
#
# Function arguments: 
#
# This function doesn't have any function arguments.
# This function will have access to all the variables defined at 
# the top of this file.
# 
# 1) $unc contains the full pathname of the sample directory.
#
# 2) $nextService contains the next state of the sample in the analysis
# pipeline. If the SARCExit function decides to stop the automated
# analysis for a sample, it will assign "Deferrer" to the $nextService.
# Otherwise, it will not modify the value of $nextService variable.
#
# 3) $dataType contains what data is being given to the SARCExit in order to
# perform the SARC filtering. The $dataType will have one of the 3 values as
# shown here a) "SamplePathName" b) "DeltaPathName" c) "DefinitionDirectoryName"
#
# 4) $deltaPathName points to the Definition Delta file produced during 
# the automated analysis.
#
# 5) $definitionDirName points to the location of the newly produced 
# definition.
########################################################################
sub SARCExit ()
{
    $result = 0;

    if ($dataType eq "$isfnSamplePathName")
    {
        # Remove only the commented block here and put the
        # SARC's filter code here.
        # For example, you may want to defer all the  samples with a size of 1001 bytes.
        # if (-s $samplePathName == 1001)
        # {
        #    $result = 1
        # }

        if ($result != 0)
        {
            $nextService = "Deferrer";
        }
    }
    elsif ($dataType eq "$isfnDeltaPathName")
    {
        # Remove only the commented block here and put the
        # SARC's filter code here.
        # For example, you may want to defer the sample, when the delta file size is 30 bytes.
        # if (-s $deltaPathName == 30)
        # {
        #    $result = 1
        # }
 
        if ($result != 0)
        {
            $nextService = "Deferrer";
        }
    }
    elsif ($dataType eq "$isdnDefDirectoryName")
    {
        # Remove only the commented block here and put the
        # SARC's filter code here.
        # For example, you may want to defer the sample, when the definition directory is
        # not a valid directory.
        # if (!(-d $definitionDirName))
        # {
        #    $result = 1
        # }

        if ($result != 0)
        {
            $nextService = "UnSerializeBuild";
            $exportParameter = "Deferrer";
        }
    }
}

