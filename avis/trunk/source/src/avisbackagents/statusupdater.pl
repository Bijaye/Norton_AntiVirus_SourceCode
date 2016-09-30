###############################################################
# statusUpdater.pl
# This program updates the sample status changes to the back office
# This program uses the SSSS.pm(Symantec Sample Status Service)
# which will be provided by Symantec some time in the end of 1999.
# Until then IBM will use its own scaffolded SSSS.pm
# to update the sample satus during the Immune System pilot phase.
# statusUpdaterer.pl will continue to work with the Symantec's
# version of SSSS.pm in the future.
#
#
# Authors: Michael Wilson, Senthil Velayudham, Srikant Jalan
# Last Modified: Apr/23/2000
#
###############################################################

	# Specify the other PERL modules used by this program.
	use Getopt::Long;
	use SSSS;
	use strict;
    use AVIS::Local;
    use Win32::ODBC;

	# Parse the command line arguments from @ARGV.
	my ($Cookie, $ProcessID, $Parameter, $unc);

	GetOptions("Cookie=i" => \$Cookie,
       "ProcessId=i" => \$ProcessID,
	   "Parameter=s" => \$Parameter,
	   "UNC=s" => \$unc);
	print "Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc\n";

	# Define some of the local variables used in this program.
    my ($status, $returnedIssueTrackingNumber, $finalState, $sequenceNumber, $statusUpdateLogFileName);

    my $statusUpdateBaseDir = "$isdnServerRoot\\BackOfficeStatusUpdater";
	# Name of the log file where all the events will be recorded.
	$statusUpdateLogFileName = "$statusUpdateBaseDir\\BackOfficeStatusUpdater.log";

	# If the base directory doesn't exist in the file server, create it.
    if (!(-d $statusUpdateBaseDir))
    {
        mkdir ($statusUpdateBaseDir, 777);
    }

    # Rename the log file to a backup if the size exceeds 1MB. 
    if (-e $statusUpdateLogFileName)
    {
       my $logFileSize;
       $logFileSize = -s $statusUpdateLogFileName;

       if ($logFileSize > 1024000)
       {
           system ("copy $statusUpdateLogFileName $statusUpdateLogFileName.bak");
           unlink ($statusUpdateLogFileName);
       }
    }         

    # Create or open the log file.
    if (-e $statusUpdateLogFileName)
    {               
       open (LOGFILEHANDLE, ">>$statusUpdateLogFileName ");
    }
    else
    {
       open (LOGFILEHANDLE, ">$statusUpdateLogFileName "); 
    }

##########   BEGIN GET THE LIST OF ALL ISSUES TO BE NOTIFIED TO SYMANTEC  ####################        
    print LOGFILEHANDLE ("========================================================\n");
    print LOGFILEHANDLE (getCurrentTime() . "Creating and executing the SQL file for getting the issues in notify symantec state\n"); 
    my ($db, $sqlStatement, $rc, %Data, $localCookie);
    my @statusUpdatedIssues;
    # Create an ODBC object to make transactions with AVISDATA.
#    $db = new Win32::ODBC ("dsn=" . $db2DSN . ";uid=" . $db2UserID . ";pwd=" . $db2Password);
    $db = new Win32::ODBC ("dsn=" . $db2DSN);

    # If we are unable to get an ODBC object, exit from this program now.
    if ($db == undef)
    {
        print LOGFILEHANDLE (getCurrentTime() . "Error in <new Win32::ODBC>.\n"); 
        exit;
    }

    # Form an SQL statement for getting all the cookies that have the NotifySymantec field set to 1 and execute the SQL statement.
    $sqlStatement = "SELECT LOCALCOOKIE FROM ANALYSISREQUEST WHERE NOTIFYSYMANTEC = x'01' ORDER BY LOCALCOOKIE";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print LOGFILEHANDLE (getCurrentTime() . "Sql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        # Stay in a loop to process all the rows of the previous SQL execution.
        while ($db->FetchRow ())
        {
            undef %Data;

            # Read the value of the LOCALCOOKIE column into the hash.
            %Data = $db->DataHash ("LOCALCOOKIE");
            $localCookie = $Data {LOCALCOOKIE};
            # For every line we read, it contains the cookieid of the sample that is
            # in the notify symantec state. Store that cookie value in a separate list.
            push @statusUpdatedIssues, $localCookie;
        }
    }

    # Calculate the total number of samples in the notify symantec state. 
    my $totalCookies;
    $totalCookies = @statusUpdatedIssues;  

    if ($totalCookies == 0)
    {
        # If there is not even a single sample in the notify symantec state, exit now. 
        print LOGFILEHANDLE (getCurrentTime() . "No sample found in notify symantec state.\n");  
        print LOGFILEHANDLE (getCurrentTime() . "Exiting the program.\n");
        # Close the ODBC connection.
        $db->Close ();
        exit;
    }
         
##########   END GET THE LIST OF ALL ISSUES TO BE NOTIFIED TO SYMANTEC   ####################

##########   BEGIN PROCESSING INDIVIDUAL ISSUES FROM THE LIST OF ISSUES TO BE NOTIFIED ###########
    # For each issue number in the statusUpdatedIssues array
    # Get the current state
    # Get the Attribute Key and Value pair
    # Call the API function SampleStatus

    my $issueNumber;
    my $currentState;

    foreach $issueNumber (@statusUpdatedIssues)
    {
        $currentState = "";     
        print LOGFILEHANDLE (getCurrentTime() . "Creating and executing the SQL file for getting the current analysis state for issue # <$issueNumber>\n"); 
        # Form an SQL statement for getting the current analysis state of the cookie and execute the SQL statement.
        $sqlStatement = "SELECT NAME FROM ANALYSISSTATEINFO WHERE ANALYSISSTATEID = (SELECT ANALYSISSTATEID FROM ANALYSISREQUEST WHERE LOCALCOOKIE = $issueNumber)";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            print LOGFILEHANDLE (getCurrentTime() . "Sql Failed. Error =" . $db->Error (). "\n");
        }
        else
        {
            # Stay in a loop to process all the rows of the previous SQL execution.
            while ($db->FetchRow ())
            {
                undef %Data;

                # Read the value of the NAME column into the hash.
                %Data = $db->DataHash ("NAME");
                $currentState = $Data {NAME};
                # Since we got the current state, exit the loop after the first iteration.
                last;
            }
        }

        if ($currentState ne "")
        {
            print LOGFILEHANDLE (getCurrentTime() . "Current analysis state for issue <$issueNumber> is <$currentState>\n"); 
        }
        else
        {
            print LOGFILEHANDLE (getCurrentTime() . "Cannot determine the Current analysis state for issue <$issueNumber>\n"); 
        }

        # Define an associative array (hash) to store all the Attribute:Value pairs.
        my %map = ();

        # Form an SQL statement for getting the current analysis state of the cookie and execute the SQL statement.
        $sqlStatement = "SELECT TEXT, VALUE FROM ATTRIBUTES, ATTRIBUTEKEYS WHERE LOCALCOOKIE = $issueNumber AND ATTRIBUTES.ATTRIBUTEKEY = ATTRIBUTEKEYS.ATTRIBUTEKEY";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            print LOGFILEHANDLE ("Sql Failed. Error =" . $db->Error (). "\n");
        }
        else
        {
            # Stay in a loop to process all the rows of the previous SQL execution.
            while ($db->FetchRow ())
            {
                undef %Data;

                # Read the value of the TEXT and VALUE columns into the hash.
                %Data = $db->DataHash ("TEXT", "VALUE");
                my ($key, $value);        
                $key = $Data {TEXT};
                $value = $Data {VALUE};
                # Save the attributes into the hash.
	            $map{$key} = $value;
            }
        }

        # Check how many keys we have added to the hash.
        my $totalKeys;
        $totalKeys = keys %map;  

        if ($totalKeys == 0)
        {
            # There is not even a single attribute found for this sample. 
            print LOGFILEHANDLE (getCurrentTime() . "No attributes found in the databse for sample <$issueNumber>.\n");  
            # Form an SQL statement for resetting the NotifySymantec field for this cookie and execute the SQL statement.
            $sqlStatement = "UPDATE ANALYSISREQUEST SET NOTIFYSYMANTEC = x'00' WHERE LOCALCOOKIE = $issueNumber";
            $rc = $db->Sql ($sqlStatement);

            # Check for any SQL execution errors.
            if ($rc)
            {
                print LOGFILEHANDLE (getCurrentTime() . "Sql Failed. Error =" . $db->Error (). "\n");
            }
            else
            {
                print LOGFILEHANDLE (getCurrentTime() . "Notify Symantec Flag has been reset for sample <$issueNumber>\n"); 
            }
        }
        else
        {
            my $status;
			print LOGFILEHANDLE (getCurrentTime() . "Calling SampleStatus function for sample <$issueNumber>.\n"); 
            $status = SSSS::SampleStatus ($issueNumber, $currentState, %map);
			print LOGFILEHANDLE (getCurrentTime() . "SampleStatus function returned status = <$status>.\n"); 

            if ($status == $SSSS::SSSS_OK)
            {
                # Form an SQL statement for resetting the NotifySymantec field for this cookie and execute the SQL statement.
                $sqlStatement = "UPDATE ANALYSISREQUEST SET NOTIFYSYMANTEC = x'00' WHERE LOCALCOOKIE = $issueNumber";
                $rc = $db->Sql ($sqlStatement);

                # Check for any SQL execution errors.
                if ($rc)
                {
                    print LOGFILEHANDLE (getCurrentTime() . "Sql Failed. Error =" . $db->Error (). "\n");
                }
                else
                {
                    print LOGFILEHANDLE (getCurrentTime() . "Notify Symantec Flag has been reset for sample <$issueNumber>\n"); 
                }
            }
        }
    }
##########   END PROCESSING INDIVIDUAL ISSUES FROM THE LIST  ###########

	print LOGFILEHANDLE (getCurrentTime() . "Sample status has been updated to the back office.\n"); 
	print LOGFILEHANDLE (getCurrentTime() . "Exiting the program.\n"); 
    # Close the ODBC connection.
    $db->Close ();
	exit;

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
