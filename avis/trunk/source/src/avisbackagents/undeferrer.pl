###############################################################
# Undeferrer.pl
# This program undefers a sample to the back office
# for manual analysis. This program uses the SSDS.pm
# (Symantec Sample Deferral Service) which will be
# provided by Symantec some time in the end of 1999.
# Until then IBM will use its own scaffolded SSDS.pm
# to undefer samples during the Immune System pilot phase.
# Undeferrer.pl will continue to work with the Symantec's
# version of SSDS.pm in the future.
#
#
# Authors: Michael Wilson, Senthil Velayudham, Srikant Jalan
# Last Modified: May/25/2000
#
###############################################################

    # Define the PERL modules used by this program.
	use Getopt::Long;
	use DataFlow;
	use SSDS;
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

	#Define some of the local variables used in this program.
    my ($status, $returnedIssueTrackingNumber, $finalState, $sequenceNumber, $undeferBaseDir, $undeferLogFileName);

    $undeferBaseDir = "$isdnServerRoot\\BackOfficeUnDeferrer";

	# Define a name for the log file where all the events will be recorded.
    $undeferLogFileName = "$undeferBaseDir\\BackOfficeUnDeferrer.log";

	# If the base directory is not there in the file server, create it.
    if (!(-d $undeferBaseDir))
    {
        mkdir ($undeferBaseDir, 777);
    }

    # Rename the log file to a backup if the size exceeds 1MB. 
    if (-e $undeferLogFileName)
    {
       my $logFileSize;
       $logFileSize = -s $undeferLogFileName;

       if ($logFileSize > 1024000)
       {
           system ("copy $undeferLogFileName $undeferLogFileName.bak");
           unlink ($undeferLogFileName);
       }
    }         

    # Create or open the log file.
    if (-e $undeferLogFileName)
    {               
       open (LOGFILEHANDLE, ">>$undeferLogFileName ");
    }
    else
    {
       open (LOGFILEHANDLE, ">$undeferLogFileName "); 
    }

    my ($db, $sqlStatement, $rc, %Data, @deferredIssues, $newSequenceNumber);

    # Create an ODBC object to make transactions with AVISDATA.
#    $db = new Win32::ODBC ("dsn=" . $db2DSN . ";uid=" . $db2UserID . ";pwd=" . $db2Password);
    $db = new Win32::ODBC ("dsn=" . $db2DSN);

    # If we are unable to get an ODBC object, exit from this program now.
    if ($db == undef)
    {
        print LOGFILEHANDLE ("Error in <new Win32::ODBC>.\n");
        close LOGFILEHANDLE;
        exit;
    }


##########   BEGIN GET THE LIST OF ALL DEFERRED ISSUES ####################

    print LOGFILEHANDLE ("========================================================\n");
    print LOGFILEHANDLE (getCurrentTime() . "Creating and executing the SQL file for getting deferred issues\n"); 

    # Form an SQL statement for getting a list of all deferred issues and execute the SQL statement.
    $sqlStatement = "SELECT LOCALCOOKIE FROM ANALYSISREQUEST WHERE ANALYSISSTATEID = (SELECT ANALYSISSTATEID FROM ANALYSISSTATEINFO WHERE NAME = 'deferred') ORDER BY LOCALCOOKIE";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print LOGFILEHANDLE ("Sql Failed. Error =" . $db->Error (). "\n");
        print LOGFILEHANDLE (getCurrentTime() . "Unable to get the list of deferred issues. Exiting the program.\n");
        $db->Close ();
        close LOGFILEHANDLE;
        exit;
    }
    else
    {
        # Stay in a loop to process all the rows of the previous SQL execution.
        while ($db->FetchRow ())
        {
            undef %Data;

            # Read the value of the LOCALCOOKIE column into the hash.
            %Data = $db->DataHash ("LOCALCOOKIE");
            # For every line we read, it contains the cookieid of the sample that is
            # in the deferred state. Store that cookie value in a separate list.
            push @deferredIssues, $Data {LOCALCOOKIE};
        }
    }

    # Calculate the total number of samples in the deferred state. 
    my $totalCookies;
    $totalCookies = @deferredIssues;  

    if ($totalCookies == 0)
    {
        # If there is not even a single sample in the deferred state, exit now. 
        print LOGFILEHANDLE (getCurrentTime() . "No samples were found in deferred state.\n");  
        print LOGFILEHANDLE (getCurrentTime() . "Exiting the program.\n");
        $db->Close ();
        close LOGFILEHANDLE;
        exit;
    }
    else
    {
        print LOGFILEHANDLE (getCurrentTime() . "$totalCookies samples were found in deferred state.\n");  
    }
         
##########   END GET THE LIST OF ALL DEFERRED ISSUES ####################

    # Randomize the array containing the deferred issues.
    # If we don't randomize, we may get stuck on one particular sample when
    # the GetNextAnalyzedSample function returns wrong values (Caused by
    # human errors). If we shuffle the array everytime we call the GetNextAnalyzed
    # function, it will give a chance for other samples in the list.
    fisher_yates_shuffle(\@deferredIssues);

    # Stay in a loop and try if the back office has undeferred the issues that we have deferred.
	# For all the issues that were undeferred, remove that issue number from the array.     
	my $undeferredCount = 0;

	while ($totalCookies != 0)
	{
		# Call the function from the SSDS (Symantec Sample Deferral Service) module to
		# get the result of the next sample that has a final result that was found during
		# the manual analysis.
		($status, $returnedIssueTrackingNumber, $finalState, $sequenceNumber) = SSDS::GetNextAnalyzedSample (@deferredIssues);

		print LOGFILEHANDLE (getCurrentTime() . "Returned from GetNextAnalyzedSample function\n");
		print LOGFILEHANDLE (getCurrentTime() . "status = $status, returnedIssueTrackingNumber = $returnedIssueTrackingNumber, finalState = $finalState, sequenceNumber = $sequenceNumber\n");
		# If the function failed, simply exit now.
		if ($status != $SSDS::SSDS_OK)
		{
			print LOGFILEHANDLE (getCurrentTime() . "Successfully undeferrerd $undeferredCount issue(s).\n");
			print LOGFILEHANDLE (getCurrentTime() . "Exiting the program.\n");
            $db->Close ();
            close LOGFILEHANDLE;
			exit;
		}

		# Returned issue tracking number should be non zero. 
		# Sequence number can be zero only if the sample is is uninfected.
		# Sequence number can be non-zero only if the sample is either infected or misfired.
		# Final state can be only "infected" or "uninfected" or "misfired".
		if (!($returnedIssueTrackingNumber  != 0 && (($sequenceNumber == 0 && lc($finalState) eq "uninfected") || ($sequenceNumber != 0 && (lc($finalState) eq "infected" || lc($finalState) eq "misfired"))) && (lc($finalState) eq "infected" || lc($finalState) eq "uninfected" || lc($finalState) eq "misfired" )))
		{
			# If the results are not meaningful, exit now.
			print LOGFILEHANDLE (getCurrentTime() . "Successfully undeferrerd $undeferredCount issue(s).\n");
			print LOGFILEHANDLE (getCurrentTime() . "Exiting the program since the results are not meaningful.\n");
            $db->Close ();
            close LOGFILEHANDLE;
			exit;
		}

		# Convert the final state value to lowercase.
		$finalState = lc($finalState);    
		my $foundMatchingEntry = 0;

		# Validate the returned issue tracking number by checking if that sample is in
		# the deferred state.
		my $deferredIssuesIndex;

        # Find the index of the returnedIssueTrackingNumber in the array and remove that
		# element from the array. 
		for ($deferredIssuesIndex = 0; $deferredIssuesIndex < $totalCookies; $deferredIssuesIndex++)
		{
			if ($deferredIssues[$deferredIssuesIndex] == $returnedIssueTrackingNumber)
			{
				 splice (@deferredIssues, $deferredIssuesIndex, 1);
				 $foundMatchingEntry = 1;
				 last;
			}
		}

		$totalCookies = @deferredIssues;  

		if ($foundMatchingEntry == 0)
		{
		    # If the returned issue tracking number is not in the deferred state, exit now.
			print LOGFILEHANDLE (getCurrentTime() . "Successfully undeferrerd $undeferredCount issue(s).\n");
		    print LOGFILEHANDLE (getCurrentTime() . "Exiting the program since the returned issue tracking number is not in the deferred state.\n");
            $db->Close ();
            close LOGFILEHANDLE;
		    exit;
		}

		if ($sequenceNumber != 0)
		{
	##########   BEGIN CHECK FOR EXISTENCE OF SEQUENCE NUMBER IN THE SIGNATURES TABLE ####################
			print LOGFILEHANDLE (getCurrentTime() . "Creating and executing the SQL file for checking the existence of the def sequence number.\n"); 
            $newSequenceNumber = 0;
            # Form an SQL statement for checking the existence of a defseqnumber and execute the SQL statement.
            $sqlStatement = "SELECT SIGNATURESEQNUM FROM SIGNATURES WHERE SIGNATURESEQNUM = $sequenceNumber";
            $rc = $db->Sql ($sqlStatement);

            # Check for any SQL execution errors.
            if ($rc)
            {
                print LOGFILEHANDLE ("Sql Failed. Error =" . $db->Error (). "\n");
                print LOGFILEHANDLE (getCurrentTime() . "Unable to check the existence of the def sequence number. Exiting the program.\n");
                $db->Close ();
                close LOGFILEHANDLE;
                exit;
            }
            else
            {
                # Stay in a loop to process all the rows of the previous SQL execution.
                while ($db->FetchRow ())
                {
                    undef %Data;

                    # Read the value of the SIGNATURESEQNUM column into the hash.
                    %Data = $db->DataHash ("SIGNATURESEQNUM");
				    # The line we read contains the sequence number that is present in the
				    # Signatures table. Store that in a new variable.
				    $newSequenceNumber = $Data {SIGNATURESEQNUM};
				    last;
                }
            }

			if ($newSequenceNumber == 0)
			{
				# if the sequence number is not in the databse, then exit this program.            
				print LOGFILEHANDLE (getCurrentTime() . "Successfully undeferrerd $undeferredCount issue(s).\n");
				print LOGFILEHANDLE (getCurrentTime() . "Exiting the program since the definition sequence has not yet been imported into the analysis center.\n");
                $db->Close ();
                close LOGFILEHANDLE;
				exit;
			}

	##########   END CHECK FOR EXISTENCE OF SEQUENCE NUMBER IN THE SIGNATURES TABLE ####################
		}
		else
		{
			$sequenceNumber = 0;
			print LOGFILEHANDLE (getCurrentTime() . "Skipping the check for existence of sequence number.\n");
		}

	##########   BEGIN ADDING NEW ROW TO THE ANALYSIS RESULTS TABLE ####################

		print LOGFILEHANDLE (getCurrentTime() . "Creating and executing the SQL file for adding a new row to the analysis results table.\n"); 

        # Form an SQL statement for adding a new row to the analysis results table and execute the SQL statement.
        if ($sequenceNumber != 0)
        {
            $sqlStatement = "insert into ANALYSISRESULTS values ((select CHECKSUM from ANALYSISREQUEST where LOCALCOOKIE = $returnedIssueTrackingNumber), $sequenceNumber, (select ANALYSISSTATEID from ANALYSISSTATEINFO where NAME = '$finalState'))";
        }
        else
        {
            $sqlStatement = "insert into ANALYSISRESULTS values ((select CHECKSUM from ANALYSISREQUEST where LOCALCOOKIE = $returnedIssueTrackingNumber), NULL, (select ANALYSISSTATEID from ANALYSISSTATEINFO where NAME = '$finalState'))";
        }

        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            print LOGFILEHANDLE ("Sql Failed. Error =" . $db->Error (). "\n");
			# If we didn't add the a new row to the results table, exit now.
			print "Results not added\n";
			print LOGFILEHANDLE (getCurrentTime() . "Successfully undeferrerd $undeferredCount issue(s).\n");
			print LOGFILEHANDLE (getCurrentTime() . "Exiting the program since the sql command to add a new record to the results table returned an error.\n");
            $db->Close ();
            close LOGFILEHANDLE;
			exit;
        }
        else
        {
            print "Results added\n";
        }

	##########   END ADDING NEW ROW TO THE ANALYSIS RESULTS TABLE ####################

	##########   BEGIN UPDATING THE SAMPLE's FINALSTATE IN THE ANALYSIS REQUEST TABLE ####################

		print LOGFILEHANDLE (getCurrentTime() . "Creating and executing the SQL file for updating the final state to the analysis request table.\n"); 

        # Form an SQL statement for setting the final state in the analysis request table and execute the SQL statement.
        $sqlStatement = "update ANALYSISREQUEST set ANALYSISSTATEID = (select ANALYSISSTATEID from ANALYSISSTATEINFO where NAME = '$finalState'), FINISHED = CURRENT TIMESTAMP where (checksum = (select CHECKSUM from ANALYSISREQUEST where LOCALCOOKIE = $returnedIssueTrackingNumber) and ANALYSISSTATEID <> (select ANALYSISSTATEID from ANALYSISSTATEINFO where NAME = 'nodetect')) and (LOCALCOOKIE = $returnedIssueTrackingNumber or IMPORTED is not NULL)";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            print LOGFILEHANDLE ("Sql Failed. Error =" . $db->Error (). "\n");
			# If we didn't update the sample state in the analysis request table, exit now.
			print "Final state not added\n";
			print LOGFILEHANDLE (getCurrentTime() . "<$returnedIssueTrackingNumber> undefer failed.\n");
			print LOGFILEHANDLE (getCurrentTime() . "Successfully undeferrerd $undeferredCount issue(s).\n");
			print LOGFILEHANDLE (getCurrentTime() . "Exiting the program since the sql command to update final state to the analysis request table returned an error.\n");
            $db->Close ();
            close LOGFILEHANDLE;
			exit;
        }
        else
        {
			my $tempString;
			$tempString = sprintf ("%08d", $returnedIssueTrackingNumber); 
			open DONEFILEHANDLE, ">$undeferBaseDir\\$tempString.undefer.done";
			close DONEFILEHANDLE;

			print "Final state set\n";
			print LOGFILEHANDLE (getCurrentTime() . "<$returnedIssueTrackingNumber> undeferred successfully.\n");
        }

        $undeferredCount++;

	##########   END UPDATING THE SAMPLE's FINALSTATE IN THE ANALYSIS REQUEST TABLE ####################
	} # end of the while ($totalCookies != 0)
	
	print LOGFILEHANDLE (getCurrentTime() . "Successfully undeferrerd $undeferredCount issue(s).\n");
	print LOGFILEHANDLE (getCurrentTime() . "Exiting the program as there is no more issues to undefer.\n");
    $db->Close ();
    close LOGFILEHANDLE;
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

########################################################################
# fisher_yates_shuffle(\@array): Generate a random permutation
# of @array in place. This function was taken from the Perl cookbook.
#
# Function arguments: 
#
# This function takes a reference to an array as a function argument.
# (e-g:) You can call this function like fisher_yates_shuffle(\@array);
########################################################################
sub fisher_yates_shuffle
{
    my $array = shift;
    my $i;

    for ($i = @$array; --$i;)
    {
         my $j = int rand ($i+1);
         next if $i == $j;
         @$array[$i,$j] = @$array[$j,$i];
    }
}
