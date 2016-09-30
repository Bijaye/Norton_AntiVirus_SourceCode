##############################################################################
# THROW AWAY THIS CODE AFTER THE PILOT IS OVER.
#
# SSSS.pm
# This perl module provides a function for updating any sample status
# changes to the back office.
# This perl module (Symantec Sample Status Service) 
# will be provided by Symantec some time in the end of 1999.
# Until then IBM will use its own scaffolded SSSS.pm to report any 
# status changes to the back office during the Immune System pilot.
#
#
# Authors: Michael Wilson, Senthil Velayudham, Srikant Jalan
# Last Modified: Sep/07/1999
#
# The specification for SSSS is available in a detailed
# document titled "Sample Status Service API specifications".
##############################################################################

	package SSSS;
	require Exporter;
	@ISA = qw (Exporter);
	@EXPORT = qw (SampleStatus);

    use AVIS::Local;

	# Define all the global constants used in this program.
	$SM_DEBUG = 1;
	$SSSS_OK = 0;
	$SSSS_ERROR = 1;
	$SSSS_FAIL = 2;
	$SSSS_IN_PROGRESS = 3;

    my $sampleStatusBaseDir = "$isdnServerRoot\\BackOfficeStatusUpdater";

########################################################################
# This subroutine sends the sample status to the back office whenever
# the status of a sample is changed in the analysis center. For the
# ImmuneSystem pilot, this function simply dumps all the sample status
# attributes to a file. The file is named as <IssueNumber>.attributes.
# This file is kept in the analysis center file server. For the real
# implementation of this function, Symantec may decide to send the
# attribute hash as a HTTP transaction to the database or directly
# update their issue tracking database in the back office by a direct
# SQL command through the firewall.
#
# Function arguments: 
#
# 1) Issue tracking number
# 2) Current analysis state of the sample
# 3) A hash containing all the sample attributes and their values.
#
########################################################################
sub SampleStatus ($$%)
{
    # Define the local variables here.
	# Parse the function arguments.
    my $issueTrackingNumber = shift;
    my $currentState = shift;
    my %attributes = @_;

    # Convert the issue tracking number to 8 digits.
    my $tempString = sprintf ("%08d", $issueTrackingNumber);

    # Create a file where all the attributes of a sample will be dumped.
    unless (open FILEHANDLE, ">$sampleStatusBaseDir\\$tempString.attributes")
    {
        return ($SSSS_FAIL);
    }

    # Write time, cookieId and currentState to the file.
    my $currentTime = getCurrentTime ();
    print FILEHANDLE ("Current Time: $currentTime\n");
    print FILEHANDLE ("Issue Tracking Number: $issueTrackingNumber\n");
    print FILEHANDLE ("Current State: $currentState\n");

    # Iterate through the hash and print each key:value pair to the file. 
    while (($key, $value) = each %attributes)
    {
        print FILEHANDLE ("$key: $value\n");
        print  ("$key: $value\n");
    }

    # Close the file.
    close (FILEHANDLE);
    return ($SSSS_OK);
}

########################################################################
# This subroutine gets the current Date and Time and returns to 
# the caller
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub getCurrentTime ()
{
    my ($sec, $min, $hr, $mday, $mon, $year, $wday, $yday, $isdst) = localtime (time);
    $year += 1900; #Y2K compliance 
    return ("<$mon/$mday/$year $hr:$min:$sec> ");
}
