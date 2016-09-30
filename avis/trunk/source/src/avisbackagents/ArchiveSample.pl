###############################################################
# ArchiveSample.pl
# This program archives samples after the successful analysis. 
#
# Authors: Senthil Velayudham, Srikant Jalan
# Last Modified: Nov/15/1999
#
###############################################################
	# Specify the PERL modules used by this program.
    use Getopt::Long;
    use strict;
    use AVIS::Local;
    use Win32::Process;
    use SSDS;

    srand (time() ^ ($$ + ($$ << 15)));

	# Parse the command line arguments from @ARGV.
    my ($Cookie, $ProcessID, $Parameter, $unc);
    my $status;
    my $CookieStr;
    my $zipFilename;

    GetOptions("Cookie=i" => \$Cookie,
   "ProcessId=i" => \$ProcessID,
   "Parameter=s" => \$Parameter,
   "UNC=s" => \$unc);
    print "Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc\n";

    # Convert the cookieId into a string containing 8 digits. 
    $CookieStr = sprintf "%08d", $Cookie;

    $zipFilename = "$CookieStr.zip";
    # Archive this sample's directory into the archive area. 

    archive($isdnGraveyard, $zipFilename, "$unc");

    # Call the function from the SSDS (Symantec Sample Deferral Service) module to
    # indicate the completion of the sample. The function DeferSample is overloaded
    # to either actually defer the sample or just update the database state depending
    # upon the zip file name. If the zip file name has a ".deferred" in it then the sample
    # will be deferred for manual analysis otherwise it just updates the back office S&D database.
    # In the Immune System pilot, it is a do-nothing function. However in the 
    # production system, it will be a Symantec developed function which does the 
    # actual task.

    $status = SSDS::DeferSample ($Cookie, "$isdnGraveyard\\$zipFilename");

    # The return value from the function is ignored as the sample directory is already archived
    # and there is no way to inform AVISDF of the failure of the function.

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
    my $flags = "-r -1 -m";
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
		# Remove the sample directory that has been just archived.
		system ("rmdir $directory /s /q");
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
