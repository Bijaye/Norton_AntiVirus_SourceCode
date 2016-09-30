###############################################################
# Deferrer.pl
# This program defers a sample to the back office
# for manual analysis. This program uses the SSDS.pm
# (Symantec Sample Deferral Service) which will be
# provided by Symantec some time in the end of 1999.
# Until then, IBM will use its own scaffolded SSDS.pm
# to defer samples during the pilot testing phase.
# Deferrer.pl will continue to work with the future version
# of SSDS.pm that will be fully developed by Symantec.
#
#
# Authors: Michael Wilson, Senthil Velayudham, Srikant Jalan
# Last Modified: Apr/21/2000
#
###############################################################

# Specify the PERL modules used by this program.
use Getopt::Long;
use DataFlow;
use SSDS;
use strict;
use AVIS::Local;
use Win32::Process;
use Win32::ODBC;

# Parse the command line arguments from @ARGV.
my ($Cookie, $ProcessID, $Parameter, $unc);

GetOptions("Cookie=i" => \$Cookie,
	    "ProcessId=i" => \$ProcessID,
		"Parameter=s" => \$Parameter,
		"UNC=s" => \$unc);

print "Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc\n";

# Define local variables to be used in this program.
my $status;
my $CookieStr;
my $zipFilename;
my $rc = 0;

# Covert the cookie id into a 8 character string with leading 0s if necessary.
$CookieStr = sprintf "%08d", $Cookie;
$zipFilename = "$CookieStr.deferred.zip";

# dffinish.dat indicates that DataFlow is done with this sample.
# Create thiis zero length file, if it doesn't exist.
if (!(-e "$unc\\dffinish.dat"))
{
    print ("Creating a zero length file \'$unc\\dffinish.dat\'.\n");
    open (FILEHANDLE, ">$unc\\dffinish.dat"); 
    close (FILEHANDLE);
}

# First, compress the entire cookie directory in the file server and store the
# zip file in the archive directory in the same file server.
$rc = archive($isdnGraveyard, $zipFilename, "$unc");

if ($rc)
{
    print ("Returning from the program without deferring the sample.\n");
    exit;
}

# Call the function from the SSDS (Symantec Sample Deferral Service) module to
# defer this sample for manual analysis. In the Immune System pilot, it is a
# do-nothing function. However in the production system, it will be a Symantec
# developed function which does the transportation of the zip file to the
# back office from the analysis center via a firewall.
$status = SSDS::DeferSample ($Cookie, "$isdnGraveyard\\$zipFilename");

if ($status == $SSDS::SSDS_OK)
{
    print ("Successfully deferred sample $Cookie to the back office.\n");
    #Set the sample's analysis state id to "deferred" in the database only if
    #the SSDS API function DeferSample returns SSDS_OK.
    setDeferredState ($Cookie);
	# Remove the sample directory that has been just archived.
	system ("rmdir $unc /s /q");
    print ("Removed the sample directory $unc.\n");
}
else
{
    print ("Unable to defer sample $Cookie. DeferSample function returned $status.\n"); 
    # Unable to send the zip file to the back office.
    # In that case, delete the zip file we created in the grave directory.
    # This zip file will get created when we try again to defer the sample.
    if (-e "$isdnGraveyard\\$zipFilename")
    {
        print ("Deleting the zip file $isdnGraveyard\\$zipFilename.\n"); 
        unlink ("$isdnGraveyard\\$zipFilename");
    }

    # Since we failed to defer this sample, we are not really done with this sample.
    # dffinish.dat indicates that DataFlow is done with this sample.
    # Hence, delete the dffinish.dat file if it is present. We will create the
    # dffinish.dat file when we try again to defer the sample.
    if (-e "$unc\\dffinish.dat")
    {
        print ("Deleting file $unc\\dffinish.dat.\n"); 
        unlink ("$unc\\dffinish.dat");
    }
}

print "Return status from DeferSample = $status\n";
exit;

########################################################################
# This subroutine archives the contents of the entire sample directory
# on the file server to the archive directory in the same file server.
#
# Function arguments: 
#
# 1) Full path to the archive directory
# 2) Name of the zip file that will be created in the archive directory.
# 3) Sample directory which has to be archived.
########################################################################
sub archive ($$$) {
	# Parse the function arguments into local variables. 
	my ($archivedir, $archivename, $directory) = @_;

	# Define some more local variables.
	my $program = "$toolsBaseDir\\zip.exe";
	my ($cc, $cmdline);

	# Convert the forwardslashes into backslashes in the directory name.
	$directory =~ tr/\//\\/;

	# Form the command to be executed.
	my $flags = "-r -1";
	$cmdline = "$program $flags $archivedir\\$archivename $directory\\*";
	print "Executing: '[$archivedir]> $cmdline'\n";

    # Launch compressing the sample directory as a separate job.
    my $process_obj = undef;
    Win32::Process::Create($process_obj,
         $program,
         $cmdline,
         0,
         CREATE_SEPARATE_WOW_VDM,
         ".");

    # Wait on the zip process to complete.
    $process_obj->Wait(INFINITE);

	# Delete the entire sample directory only if the sample has been archived successfully. 
	if (-e "$archivedir\\$archivename")
	{
       print ("Successfully created a zip file ($archivedir\\$archivename).\n");
       return (0);
	}
    else
    {
       print ("Unable to create a zip file ($archivedir\\$archivename).\n");
       return (1);
    }
}

########################################################################
# This subroutine sets the analysis state id of the sample to
# "deferred" in the database.
#
# Function arguments: 
#
# 1) Issue tracking number.
# 
########################################################################
sub setDeferredState ($)
{
	# Parse the function argument.
	my ($issueTrackingNumber) = @_;

    my ($db, $sqlStatement, $rc, %Data);
    # Create an ODBC object to make transactions with AVISDATA.
#    $db = new Win32::ODBC ("dsn=" . $db2DSN . ";uid=" . $db2UserID . ";pwd=" . $db2Password);
    $db = new Win32::ODBC ("dsn=" . $db2DSN);

    # If we are unable to get an ODBC object, return from this function now.
    if ($db == undef)
    {
        print "Error in <new Win32::ODBC>.\n";
        return (1);
    }

    # Form an SQL statement for updating the current state of the cookie to 'Deferred' and execute the SQL statement.
    $sqlStatement = "update analysisrequest set ANALYSISSTATEID = (SELECT ANALYSISSTATEID FROM ANALYSISSTATEINFO WHERE NAME = 'deferred') where LOCALCOOKIE = $issueTrackingNumber";
    $rc = $db->Sql ($sqlStatement);

    # Close the ODBC object.
    $db->Close ();

    # Check for any SQL execution errors.
    if ($rc)
    {
        print "Sql Failed. Error =" . $db->Error (). "\n";
        return (1);
    }
    else
    {
        print ("Successfully set the sample state as \'deferred\'.\n");
        return (0);
    }
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
    $mon++;
    return ("<$mon/$mday/$year $hr:$min:$sec> ");
}
