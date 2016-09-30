###############################################################
# ModifyAnalysisResults.pl
#
# This program can be used to manually set the final state to
# a given sample in the AnalysisResults table. The same
# program can be used for removing a final state entry from
# the AnalysisResults table in the database. 
#
# Note: Whenever this program is used to remove a final state
# entry from the AnalysisResults table, it is necessary to do
# the same in all the Gateways and the Analysis center. 
# Otherwise, some gateways will be holding out of sync final 
# state for a given checksum.
#
# This tool changes the behavior of the system operation. Please
# use this only if you are sure what you are trying to do.
#
#
# Authors: Senthil Velayudham, Srikant Jalan
# Created on: Mar/27/2000
# Last Modified: Apr/23/2000
#
###############################################################
	# Specify the PERL modules used by this program.
    use Getopt::Long;
    use strict;
    use AVIS::Local;
    use Win32::Process;
    use Win32::ODBC;

    # Define some local variables used in this program.
    my ($db, $sqlStatement, $rc, %Data, $localCookie);

	# Parse the command line arguments from @ARGV.
    my ($option, $checksum, $finalState, $defSequence) = ("", "", "", "");
    ($option, $checksum, $finalState, $defSequence) = @ARGV;
    print "option = $option, checksum = $checksum, finalState = $finalState, defSequence = $defSequence\n";

    # Validate the command line arguments and display the program usage in case of
    # any invalid arguments.
    if (lc ($option) ne lc ("Add") &&
        lc ($option) ne lc ("Remove"))
    {
        PrintHelp ();
        exit;
    }

    if (length ($checksum) != 32)
    {
        PrintHelp ();
        exit;
    }

    if (lc ($option) eq lc ("Add") &&
        lc ($finalState) ne lc ("infected") &&
        lc ($finalState) ne lc ("uninfected") &&
        lc ($finalState) ne lc ("misfired"))
    {
        PrintHelp ();
        exit;
    }

    # Create an ODBC object to make transactions with AVISDATA.
#    $db = new Win32::ODBC ("dsn=" . $db2DSN . ";uid=" . $db2UserID . ";pwd=" . $db2Password);
    $db = new Win32::ODBC ("dsn=" . $db2DSN);

    # If we are unable to get an ODBC object, return from this function now.
    if ($db == undef)
    {
        print ("Error in <new Win32::ODBC>.\n");
        return;
    }

# ---------------------------- REMOVE A ROW IN THE RESULTS TABLE. ----------------------------
	# check if the user opted to remove a results row for a given checksum.
	if (lc ($option) eq lc ("Remove"))
	{
		#We need to delete the row corresponding to the checksum specified by the user from the
		#AnalysisResults table and the AnalysisRequest table.
		print ("Creating and executing the SQL command for deleting a row from the analysis results table.\n"); 
		# Form an SQL statement for deleting a row from the analysis results table and execute the SQL statement.
		$sqlStatement = "delete from ANALYSISRESULTS where checksum = \'$checksum\'";
		$rc = $db->Sql ($sqlStatement);

		# Check for any SQL execution errors.
		if ($rc)
		{
			#Unable to remove a row belonging to a checksum from the results table.
			print ("Unable to remove the row corresponding to the given checksum from the AnalysisResults table.\n");
			print ("You may verify the checksum value you specified OR\n");
			print ("you may ignore this error message if there is no row matching this checksum.\n");
			$db->Close ();
			exit;
		}
		
		print ("Removed the row in the AnalysisResults table corresponding to the given checksum.\n");

		# Now get the corresponding localcookie number for this checksum from the AnalysisRequest table.
		print ("Creating and executing the SQL command to get the LocalCookie for the given checksum.\n"); 
		# Form an SQL statement for deleting a row from the analysis results table and execute the SQL statement.
		$sqlStatement = "select LOCALCOOKIE from ANALYSISREQUEST where checksum = \'$checksum\'";
		$rc = $db->Sql ($sqlStatement);

		# Check for any SQL execution errors.
		if ($rc)
		{
			#It is okay if this SQL command fails. There may not be a corresponding row
			#in the request table if the sample was never received in the gateway or the
			#analysis center. This can happen if the final state was reported from the 
			#analysis center to all the gateways or the final state was manually added to
			#take care of any future sample submissions.
			print ("SQL command to get the LOCALCOOKIE for the given checksum failed.\n");
			print ("This is a non-critical error. You may ignore this error message.\n");
			$db->Close ();
			exit;
		}

        # Stay in a loop to process all the rows of the previous SQL execution.
        if ($db->FetchRow ())
        {
            undef %Data;

            # Read the value of the LOCALCOOKIE column into the hash.
            %Data = $db->DataHash ("LOCALCOOKIE");
            $localCookie = $Data {LOCALCOOKIE};
        }
		else
		{
			#It is okay if this SQL command doesn't return a row. There may not be a corresponding row
			#in the request table if the sample was never received in the gateway or the
			#analysis center. This can happen if the final state was reported from the 
			#analysis center to all the gateways or the final state was manually added to
			#take care of any future sample submissions.
			print ("SQL command to get the LOCALCOOKIE for the given checksum didn't fetch a matching row.\n");
			print ("This is a non-critical error. You may ignore this error message.\n");
			$db->Close ();
			exit;
		}
		
		# Now we can attempt to delete the matching row from the AnalysisRequest table.
		# There is no need to check if it really deleted the matching row(s).
		print ("Creating and executing the SQL command to delete the row from the AnalysisRequest table for the given checksum.\n"); 
		# Form an SQL statement for deleting a row from the analysis request table and execute the SQL statement.
		$sqlStatement = "delete from ANALYSISREQUEST where checksum = \'$checksum\'";
		$rc = $db->Sql ($sqlStatement);

		# Now we can attempt to delete the matching rows from the Attributes table.
		# There is no need to check if it really deleted the matching row(s).
		print ("Creating and executing the SQL command for deleting the rows from the Attributes table for the given checksum.\n"); 
		# Form an SQL statement for deleting a row from the analysis results table and execute the SQL statement.
		$sqlStatement = "delete from ATTRIBUTES where localcookie = $localCookie";
		$rc = $db->Sql ($sqlStatement);
		$db->Close ();
		exit;
	}

# ---------------------------- ADD A NEW ROW IN THE RESULTS TABLE. ----------------------------
	# The following code will be executed only if the user wants to add a new result
	# row for a given checksum in the AnalysisResults table.
    if ($defSequence == undef)
    {
        $defSequence = 0;
    }

	# Sequence number can be zero only if the sample is is uninfected.
	# Sequence number can be non-zero only if the sample is either infected or misfired.
	if (!((($defSequence == 0 && lc($finalState) eq "uninfected") || ($defSequence  != 0 && (lc($finalState) eq "infected" || lc($finalState) eq "misfired")))))
	{
		# If the results are not meaningful, exit now.
		print ("Exiting the program since the specified final results are not meaningful.\n");
        $db->Close ();
		exit;
	}

	# If the user wants to set a final state with a definiton sequence number, ensure that
	# we already have the sequence number in the system.	
	if ($defSequence != 0)
	{
##########   BEGIN CHECK FOR THE EXISTENCE OF SEQUENCE NUMBER IN THE SIGNATURES TABLE ####################
		print ("Creating and executing the SQL command for checking the existence of the def sequence number.\n"); 
		# Form an SQL statement for checking the existance of a defseqnumber and execute the SQL statement.
		$sqlStatement = "SELECT SIGNATURESEQNUM FROM SIGNATURES WHERE SIGNATURESEQNUM = $defSequence";
		$rc = $db->Sql ($sqlStatement);

		# Check for any SQL execution errors.
		if ($rc)
		{
			print ("Sql Failed. Error =" . $db->Error (). "\n");
			# if the sequence number is not in the database, then exit this program.            
			print ("Exiting the program since the definition sequence is not available in the system.\n");
			$db->Close ();
			exit;
		}
		else
		{
			# Process the resulting row of the previous SQL execution.
			if (!$db->FetchRow ())
			{
				# if the sequence number is not in the database, then exit this program.            
				print ("Exiting the program since the definition sequence is not available in the system.\n");
				$db->Close ();
				exit;
			}
		}
##########   END CHECK FOR EXISTANCE OF SEQUENCE NUMBER IN THE SIGNATURES TABLE ####################
	}

##########   BEGIN ADDING NEW ROW TO THE ANALYSIS RESULTS TABLE ####################

	print ("Creating and executing the SQL command for adding a new row to the analysis results table.\n"); 

    # Form an SQL statement for adding a new row to the analysis results table and execute the SQL statement.
    if ($defSequence != 0)
    {
        $sqlStatement = "insert into ANALYSISRESULTS values (\'$checksum\', $defSequence, (select ANALYSISSTATEID from ANALYSISSTATEINFO where NAME = '$finalState'))";
    }
    else
    {
        $sqlStatement = "insert into ANALYSISRESULTS values (\'$checksum\', NULL, (select ANALYSISSTATEID from ANALYSISSTATEINFO where NAME = '$finalState'))";
    }

    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
	# If the previous insert command failed, that means we already have a row for the
	# given checksum in the results table. In that case, we can directly try to update the
	# results table with the user specified final state.
    if ($rc)
    {
	    print ("Creating and executing the SQL command for updating the final state to the analysis results table.\n"); 
    
        # Form an SQL statement for setting the final state in the analysis results table and execute the SQL statement.
        if ($defSequence)
        {
            $sqlStatement = "update ANALYSISRESULTS set ANALYSISSTATEID = (select ANALYSISSTATEID from ANALYSISSTATEINFO where NAME = '$finalState'), SignatureSeqNum = $defSequence where checksum = \'$checksum\'";
        }
        else
        {
            $sqlStatement = "update ANALYSISRESULTS set ANALYSISSTATEID = (select ANALYSISSTATEID from ANALYSISSTATEINFO where NAME = '$finalState'), SignatureSeqNum = NULL where checksum = \'$checksum\'";
        }

        $rc = $db->Sql ($sqlStatement);

        if ($rc)
        {
            print ("Unable to update the AnalysisResults table.\n");
            $db->Close ();
            exit;
        }
    }

    print "Results added\n";


##########   END ADDING NEW ROW TO THE ANALYSIS RESULTS TABLE ####################

##########   BEGIN UPDATING THE SAMPLE's FINALSTATE IN THE ANALYSIS REQUEST TABLE ####################

    # Form an SQL statement for setting the final state in the analysis request table and execute the SQL statement.
	print ("Creating and executing the SQL command for updating the matching row in the analysis request table.\n"); 
    $sqlStatement = "update ANALYSISREQUEST set ANALYSISSTATEID = (select ANALYSISSTATEID from ANALYSISSTATEINFO where NAME = '$finalState'), FINISHED = CURRENT TIMESTAMP where checksum = \'$checksum\'";
    $rc = $db->Sql ($sqlStatement);

    if ($rc)
    {
        print ("Unable to update the AnalysisRequest table.\n");
        print ("This is a non-critical error. You may ignore this error message.\n");
    }
    else
    {
        print ("Updated the AnalysisRequest table.\n");
    }
##########   END UPDATING THE SAMPLE's FINALSTATE IN THE ANALYSIS REQUEST TABLE ####################

    $db->Close ();
    exit;


sub PrintHelp ()
{
    print ("----------------------------------------------------------------------\n");
    print ("perl ModifyAnalysisResults.pl <Option> <Checksum> <FinalState> <DefSequence>\n\n");
    print ("<Option> is either \"Add\" or \"Remove\".\n");
    print ("<Checksum> is a 32 character string.\n");
    print ("<FinalState> is either \"infected\" or \"uninfected\" or \"misfired\".\n");
    print ("<DefSequence> is a valid definition sequence number.\n\n");
    print ("The \"Add\" option requires all the 3 other arguments.\n");
    print ("The \"Remove\" option requires only the <Checksum> argument.\n");
    print ("----------------------------------------------------------------------\n");
}
