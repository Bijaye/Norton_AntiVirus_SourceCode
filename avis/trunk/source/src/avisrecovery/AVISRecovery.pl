###############################################################
# AVISRecovery.pl
#
# This program is the recovery tool used for getting back the 
# AVISDATA database and sample files from the backup. This tool
# has to be run with different command line options based on the
# recovery instructions specified in the "Disaster Recovery for
# AVIS Data" document. This program is typically required after
# a database or a file system crash.
#
# Authors: Senthil Velayudham, Srikant Jalan
# Created on: Mar/09/2000
# Last Modified: June/14/2000
#
###############################################################
	# Specify the PERL modules used by this program.
    use Getopt::Long;
    use AVIS::Local;
    use Win32::Process;
    use Win32::ODBC;

    # Define some local variables used in this program.
    my ($db, $sqlStatement, $rc, %Data);

	# Parse the command line arguments from @ARGV.
    my ($recoveryType, $arg1) = @ARGV;
    print "RecoveryType = $recoveryType, ARG1 = $arg1\n";

    my $serverName = substr $isdnServer, 2;

    #Change the following to point to a drive with enough space for the recovery procedure
    my $rootDrive = $isdnRootDrive;

    # Validate the command line arguments and display program usage in case of
    # any invalid arguments.
    if ((lc ($recoveryType) ne lc ("GetSamplesInAnalysis") &&
        lc ($recoveryType) ne lc ("FixSamplesInAnalysis") &&
        lc ($recoveryType) ne lc ("FilterNewGatewaySamples") &&
        lc ($recoveryType) ne lc ("SendNewDefinitionsToGateway") ||
        lc ($recoveryType) eq "?"))
    {
        print ("Usage: perl AVISRecovery.pl <RecoveryType> <GWSampleDir> <GatewayAddress>\n\n");
        print ("<RecoveryType> can be one of the following strings.\n");
        print ("<GWSampleDir> is required only for the third recovery type.\n");
        print ("<GatewayAddress> is required only for the last recovery type.\n");
        print ("GetSamplesInAnalysis\n");
        print ("FixSamplesInAnalysis\n");
        print ("FilterNewGatewaySamples\n");
        print ("SendNewDefinitionsToGateway\n");
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

    # Check the recovery type and branch out to the appropriate subroutine.
    if (lc($recoveryType) eq lc("GetSamplesInAnalysis"))
    {
        # Get a list of all the samples in the analysis state at the time of the
        # last backup.
        GetSamplesInAnalysis ();
    }
    elsif (lc($recoveryType) eq lc("FixSamplesInAnalysis"))
    {
        # Try to fix the samples that were in one of the analysis states at the time
        # of the last backup.
        FixSamplesInAnalysis ();
    }
    elsif (lc($recoveryType) eq lc("FilterNewGatewaySamples"))
    {
        # Validate the Gateway server address.
        if ($arg1)
        {
            # Get a list of all the samples imported from the given gateway and repopulate
            # the AnalysisRequest and Attributes tables.
            FilterNewGatewaySamples ($arg1);
        }
        else
        {
            print ("Missing Gateway sample directory name in the command line argument.\n");
        }
    }
    elsif (lc($recoveryType) eq lc("SendNewDefinitionsToGateway"))
    {
        # Validate the Gateway server address.
        if ($arg1)
        {
            # Mark the SigsToBeExported table to export the latest blessed and unblessed
            # definitions to the gateway.
            SendNewDefinitionsToGateway ($arg1);
        }
        else
        {
            print ("Missing Gateway server address in the command line argument.\n");
            return;
        }
    }

    # Close the database handle.
    $db->Close ();
    exit;

########################################################################
# First step in the recovery process is to derive a list of all the
# samples that were in one of the analysis states at the time of the
# last backup. This subroutine gets all the samples that have imported 
# date and not the finished date. The list of such samples will be 
# written to a file ($rootDrive\avis\SamplesInAnalysis.txt) which will enable
# the analysis center system administrator to collect the sample files
# from the backup media avaialable in the back office.
# On June/09/2000, I added code to reset the NewestBlssdSeqNum and 
# NewestSeqNum fields of the GLOBALS table to 1 and 2 respectively.
# In the recovery procedure, we use GetDefinitions.pl to get the 
# blessed and unblessed definitions from the back office. That will work
# only if we have smaller values in the database fields.
#
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub GetSamplesInAnalysis ()
{
    # (June/09/2000) At first let us slip in the following code snippet
    # to set smaller values for the NewestBlssdSeqNum and 
    # NewestSeqNum fields of the GLOBALS table (1 and 2 respectively).
    # This is required for the following reason.
    # In the recovery procedure, we use GetDefinitions.pl to get the 
    # blessed and unblessed definitions from the back office. That will work
    # only if we have smaller values in the database fields.
    # Form an SQL statement for updating the NewestBlssdSeqNum and the
    # NewestSeqNum fields in the Globals table.
    $sqlStatement = "UPDATE GLOBALS SET NEWESTBLSSDSEQNUM = 1, NEWESTSEQNUM = 2";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");        
        return;
    }
    else
    {
        print ("Successfully set the NEWESTBLSSDSEQNUM, NEWESTSEQNUM database fields to 1 and 2.\n");
    }            

    # Now do the real thing that this subroutine is supposed to do.
    # Create a file where the cookie numbers of the samples in one of
    # the analysis states will be written.
    unless (open FILEHANDLE, ">$rootDrive\\avis\\SamplesInAnalysis.txt")
    {
        print ("Error in creating $rootDrive\\avis\\SamplesInAnalysis.txt.\n");
        return;
    }
    
    print FILEHANDLE ("LocalCookie\n");

    # Form an SQL statement for getting the LOCALCOOKIE ids that are imported but not finished.
    $sqlStatement = "SELECT LOCALCOOKIE FROM ANALYSISREQUEST WHERE FINISHED IS NULL AND IMPORTED IS NOT NULL ORDER BY LOCALCOOKIE";
    $rc = $db->Sql ($sqlStatement);

    print ("\n");
    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        # Stay in a loop and process every row returned by the SQL statement.
        while ($db->FetchRow ())
        {
            my ($localCookie);
            undef %Data;

            # Read the value of the NAME column into the hash.
            %Data = $db->DataHash ("LOCALCOOKIE");
            $localCookie = $Data {LOCALCOOKIE};

            # Write the LocalCookie value to the file and also display it.
            print FILEHANDLE ("$localCookie\n");
            print ("LocalCookie in analysis = $localCookie\r");
        }
    }

    print ("\n");
    # Form an SQL statement for getting the largest LocalCookie number in the AnalysisRequest table.
    $sqlStatement = "SELECT MAX (LOCALCOOKIE) as MAXLOCALCOOKIE FROM ANALYSISREQUEST";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        if ($db->FetchRow ())
        {
            my ($maxLocalCookie);
            undef %Data;

            # Read the value of the MAXLOCALCOOKIE column into the hash.
            %Data = $db->DataHash ("MAXLOCALCOOKIE");
            $maxLocalCookie = $Data {MAXLOCALCOOKIE};

            print FILEHANDLE ("\nLast LocalCookie Number = $maxLocalCookie\n");
            print ("\nLast LocalCookie Number = $maxLocalCookie\n");
        }
        else
        {
            print FILEHANDLE ("\nLast LocalCookie Number = 0\n");
        }
    }

    close (FILEHANDLE);
}

########################################################################
# The second step in the recovery process is to fix all the samples that
# were in one of the analysis states at the time of the last backup.
# This function does many things like, changing the analysis stateid of
# the sample to 'imported', inserting new rows to the AnalysisRequest
# table, inserting new rows to the Attributes table etc.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub FixSamplesInAnalysis ()
{
    my $cookieFound = 0;
    my $dirName;
    my $sampleDir = "$rootDrive\\SamplesInAnalysis\\$serverName\\SampleStorage";
    my $tmpDirListFile = "$rootDrive\\SamplesInAnalysis\\dir.out";
    my $zipFileStorageDir = "$rootDrive\\SamplesInAnalysis";

    # Added on June/14/2000.
    # delete all records from SigsToBeExported table
    $sqlStatement = "DELETE FROM SIGSTOBEEXPORTED";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        print ("This table could be empty. You can ignore this error.\n");
    }
    else
    {
        print ("Deleted all records from the SigsToBeExported table.\n");
    }

    # Added on June/14/2000.
    # delete all signature records from the signatures table for seqnum > 2
    $sqlStatement = "DELETE FROM SIGNATURES WHERE SIGNATURESEQNUM > 2";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        print ("There may not be any records with definition sequence greater 2. You can ignore this error.\n");
    }
    else
    {
        print ("Deleted all records from the signatures table for sequence number greater than 2.\n");
    }

    # Unzip the zip files in the $rootDrive\SamplesInAnalysis directory.
    system ("dir /b $zipFileStorageDir\\*.zip > $tmpDirListFile");
    open (FILEHANDLE, "<$tmpDirListFile");

    # Read all the directory entries in this directory into an array. Avoid reading "." and ".." entries.
    my @directoryEntries = <FILEHANDLE>;
    chop @directoryEntries;
    close FILEHANDLE;
    unlink $tmpDirListFile;

    # Calculate the total number of sample directories in the SampleStorage.
    my $totalNumberOfDirEntries;
    $totalNumberOfDirEntries = @directoryEntries;  
    print ("Total ZIP file entries = $totalNumberOfDirEntries\n");

    # If there are no directory entries in the SampleStorage, exit the program now.
    if ($totalNumberOfDirEntries == 0)
    {
        print ("There are no sample backup (*.zip) files found in $zipFileStorageDir\n");
        return;
    }

    # Stay in a loop and process every sample directory.     
    foreach $dirName (@directoryEntries)
    {
        system ("$toolsBaseDir\\unzip.exe $zipFileStorageDir\\$dirName -d $zipFileStorageDir");
        unlink "$zipFileStorageDir\\$dirName";
    }

    # Derive all the cookie subdirectories under the SampleStorage directory.
    system ("dir /b $sampleDir > $tmpDirListFile");
    open (FILEHANDLE, "<$tmpDirListFile");

    # Read all the directory entries in this directory into an array. Avoid reading "." and ".." entries.
    @directoryEntries = ();
    @directoryEntries = <FILEHANDLE>;
    chop @directoryEntries;
    close FILEHANDLE;
    unlink $tmpDirListFile;

    # Calculate the total number of sample directories in the SampleStorage.
    $totalNumberOfDirEntries = @directoryEntries;  
    print ("Total dir entries = $totalNumberOfDirEntries\n");

    # If there are no directory entries in the SampleStorage, exit the program now.
    if ($totalNumberOfDirEntries == 0)
    {
        print ("There are no sample directories found in $sampleDir\n");
        return;
    }

    # Stay in a loop and process every sample directory.     
    foreach $dirName (@directoryEntries)
    {
        print ("$dirName\n");

        # If this is not a subdirectory, proceed to the next cookie directory.
        if (!(-d "$sampleDir\\$dirName"))
        {
            next;
        }

        # If it doesn't contain a "sample" directory, this is not a cookie directory.
        # In that case, proceed to the next directory.
        if (!(-d "$sampleDir\\$dirName\\sample"))
        {
            next;
        }

        $cookieFound = 0;
        # Form an SQL statement for getting the LOCALCOOKIE row corresponding to the current sampledir.
        $sqlStatement = "SELECT LOCALCOOKIE FROM ANALYSISREQUEST WHERE LOCALCOOKIE = $dirName";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
            next;
        }
        else
        {
            if ($db->FetchRow ())
            {
                $cookieFound = 1;
            }
            else
            {
                $cookieFound = 0;
            }
        }
    
        #If we already have a record in the database for this sample,
        # we will update the analysisstateid of that sample as 'imported'.
        if ($cookieFound)
        {
            # Update the analysis state for this sample in the database as imported.
            # Form an SQL statement for setting the final state in the analysis request table and execute the SQL statement.
            $sqlStatement = "update ANALYSISREQUEST set ANALYSISSTATEID = (select ANALYSISSTATEID from ANALYSISSTATEINFO where NAME = \'imported\') where (checksum = (select CHECKSUM from ANALYSISREQUEST where LOCALCOOKIE = $dirName))";
            $rc = $db->Sql ($sqlStatement);

            # Check for any SQL execution errors.
            if ($rc)
            {
                print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
                next;
            }
            else
            {                
			    print "AnalysisStateId updated as imported for local cookie $dirName\n";
            }
        }
        else
        {
            # We don't have a record for this sample in the database.
            # Try to insert a new row for this sample. Since this is fairly a
            # large task, that function is put in a separate subroutine.
            $rc = InsertNewSampleInDatabase ($dirName);

            if ($rc)
            {
                #Unable to insert new sample into the database.
                next;
            }
        }

        # At this point, we have successfully updated the analysisstateid or
        # inserted a new for the sample and added all its attributes.
        # Create the sample and the SampleAttribute directories in the Rushmore fileserver now.
        # Copy the sample and attribute.txt files now.
        system ("mkdir $isdnServerRoot\\$dirName");
        system ("mkdir $isdnServerRoot\\$dirName\\sample");
        system ("mkdir $isdnServerRoot\\$dirName\\SampleAttributes");
        system ("copy $sampleDir\\$dirName\\sample $isdnServerRoot\\$dirName\\sample");
        system ("copy $sampleDir\\$dirName\\SampleAttributes $isdnServerRoot\\$dirName\\SampleAttributes");
        #create import.complete and BackOfficeSampleExport.complete files.
        open (DUMMYHANDLE, ">$isdnServerRoot\\$dirName\\import.complete");
        close (DUMMYHANDLE);
        open (DUMMYHANDLE, ">$isdnServerRoot\\$dirName\\BackOfficeSampleExport.complete");
        close (DUMMYHANDLE);    
    }

    # Delete the SamplesInAnalysis directory.
    system ("rmdir $zipFileStorageDir /s /q");

    my ($maxLocalCookie);
    # Now adjust the next local cookie to an appropriate value.
    # Form an SQL statement for getting the largest LocalCookie number in the analysisrequest table.
    $sqlStatement = "SELECT MAX (LOCALCOOKIE) as MAXLOCALCOOKIE FROM ANALYSISREQUEST";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return;
    }
    else
    {
        if ($db->FetchRow ())
        {            
            undef %Data;

            # Read the value of the MAXLOCALCOOKIE column into the hash.
            %Data = $db->DataHash ("MAXLOCALCOOKIE");
            $maxLocalCookie = $Data {MAXLOCALCOOKIE};
            $maxLocalCookie++;
            print ("We are about to set the NEXTLOCALCOOKIE to $maxLocalCookie in the GLOBALS table.\n");
        }
        else
        {
            print ("Unable to obtain the max local cookie.\n");
            return;
        }
    }

    # Form an SQL statement for updating the NextLocalCookie field in the Globals table.
    $sqlStatement = "UPDATE GLOBALS SET NEXTLOCALCOOKIE = $maxLocalCookie";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");        
        return;
    }
    else
    {
        print ("Successfully set the NEXTLOCALCOOKIE to $maxLocalCookie in the GLOBALS table.\n");
    }            
}

########################################################################
# This function will be called when this program is run in the data
# storage machine of the gateway array. This function deletes the unwanted
# records from the Attributes, SigsToBeExported and RemoteMustBeUnique tables.
# This function deletes the records from the ANALYSISREQUEST table where
# the IMPORTED DATE is NULL. It adjusts the NextLocalCookie field in the
# GLOBALS tables to an appropriate value. It gets the directory listing of
# the gateway sample directory. For every sample file found in the sample
# directory, it checks if that checksum already exists in the ANALYSISRESULTS
# or the ANALYSISREQUEST table. It it exists, it deletes that sample file
# and processes the next file. This way, a new analysis record will be 
# created only when there is no existing entry in the database.
# It cleans up the tables such as SigsToBeExported and Servers tables. 
# It removes all the entries in the Signatures table except the first 2 rows 
# for definition sequence numbers 1 and 2. It readjusts the blessed and 
# unblessed values in the globals tables to 1 and 2 respectively.
#
# Function arguments: 
#
# This function takes one argument which is the name of the sample
# storage directory in the data storage machine of the gateway array.
########################################################################
sub FilterNewGatewaySamples ()
{
    my ($gatewaySampleDir) = @_;
    my ($serverID);

    # delete all records from RemoteMustBeUnique table
    $sqlStatement = "DELETE FROM REMOTEMUSTBEUNIQUE";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the RemoteMustBeUnique table.\n");
    }

    # delete all records from SigsToBeExported table
    $sqlStatement = "DELETE FROM SIGSTOBEEXPORTED";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the SigsToBeExported table.\n");
    }

    # delete all requests for which there is no valid imported date.
    $sqlStatement = "DELETE FROM ANALYSISREQUEST WHERE IMPORTED IS NULL";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the AnalysisRequest table with no imported date.\n");
    }

    # Set RemoteServer, RemoteCookie, NotifyClient and NotifySymantec to NULL
    $sqlStatement = "UPDATE ANALYSISREQUEST SET RemoteServerID = NULL, RemoteCookie = NULL, NotifyClient = X'00', NotifySymantec = X'00'";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Set RemoteServerID, RemoteCookie, NotifyClient, NotifySymantec to NULL in Analysisrequest table.\n");
    }

    my $maxLocalCookie;

    # get the maximum LocalCookie number
    $sqlStatement = "SELECT MAX(LOCALCOOKIE) AS MAXLOCALCOOKIE FROM ANALYSISREQUEST";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return 1;
    }
    else
    {
        if ($db->FetchRow ())
        {
            undef %Data;

            # Read the value of the MAXLOCALCOOKIE column into the hash.
            %Data = $db->DataHash ("MAXLOCALCOOKIE");
            $maxLocalCookie = $Data {MAXLOCALCOOKIE};
        }
        else
        {
            $maxLocalCookie = 0;
        }
    }

    $maxLocalCookie++;

    my $dirName;
    my $tmpDirListFile = "$rootDrive\\gwdir.out";

    # Read the directory listing of the gateway sample directory.
    print "dir /b $gatewaySampleDir\\*.sample > $tmpDirListFile";
    system ("dir /b $gatewaySampleDir\\*.sample > $tmpDirListFile");
    open (FILEHANDLE, "<$tmpDirListFile");

    # Read all the directory entries in this directory into an array. Avoid reading "." and ".." entries.
    my @directoryEntries = <FILEHANDLE>;
    chop @directoryEntries;
    close FILEHANDLE;
    unlink $tmpDirListFile;

    # Calculate the total number of sample directories in the SampleStorage.
    my $totalNumberOfDirEntries;
    $totalNumberOfDirEntries = @directoryEntries;  
    print ("Total gateway sample file entries = $totalNumberOfDirEntries\n");

    # If there are no directory entries in the SampleStorage, exit the program now.
    if ($totalNumberOfDirEntries == 0)
    {
        print ("There are no gateway sample files found in $gatewaySampleDir\n");
    }
    
    my $checksum = "";

    # Stay in a loop and process every sample directory.     
    foreach $dirName (@directoryEntries)
    {
        print ("$dirName\n");

        # Remove the ".sample" substring so that we will have only the checksum value.
        $dirName =~ s/$\.sample//i;
        $checksum = $dirName;

        # Check if this checksum is already present in the results table.
        # Form an SQL statement for getting the LOCALCOOKIE row corresponding to the current sampledir.
        $sqlStatement = "SELECT CHECKSUM FROM ANALYSISRESULTS WHERE CHECKSUM = \'$checksum\'";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
            next;
        }
        else
        {
            if ($db->FetchRow ())
            {
                # We found a match for this sample in the results table.
                # There is no need to process this sample again.
                # Delete the sample and attribute files.
                unlink ("$gatewaySampleDir\\$checksum.sample");
                unlink ("$gatewaySampleDir\\$checksum.attributes");
                next;
            }
        }


        # Check if this checksum is already present in the request table.
        # Form an SQL statement for getting the LOCALCOOKIE row corresponding to the current sampledir.
        $sqlStatement = "SELECT CHECKSUM FROM ANALYSISREQUEST WHERE CHECKSUM = \'$checksum\'";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
            next;
        }
        else
        {
            if ($db->FetchRow ())
            {
                # We found a match for this sample in the results table.
                # There is no need to process this sample again.
                # Delete the sample and attribute files.
                unlink ("$gatewaySampleDir\\$checksum.sample");
                unlink ("$gatewaySampleDir\\$checksum.attributes");
                next;
            }
        }
    

        # We don't have a record for this sample in the database.
        # Try to insert a new row for this sample. Since this is fairly a
        # large task, that function is put in a separate subroutine.
        $rc = InsertNewGatewaySampleInDatabase ($dirName, $maxLocalCookie, $gatewaySampleDir);
        $maxLocalCookie++;

        if ($rc)
        {
            #Unable to insert new sample into the database.
            # Delete the sample and attribute files.
            unlink ("$gatewaySampleDir\\$checksum.sample");
            unlink ("$gatewaySampleDir\\$checksum.attributes");
            next;
        }
    }

    # Update Globals Table
    $sqlStatement = "UPDATE GLOBALS SET NewestSeqNum = 2, NewestBlssdSeqNum = 1, NextLocalCookie = $maxLocalCookie, ThisServerID = 0, BackOfficeLock = X'00', TypeOfServer = X'00'";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Set new NextLocalCookie to $maxLocalCookie and initialized other values in the Globals table.\n");
    }

    # delete all signatures record from the signatures table for seqnum > 2
    $sqlStatement = "DELETE FROM SIGNATURES WHERE SIGNATURESEQNUM > 2";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the signatures table for sequence number greater than 2.\n");
    }

    # Delete the servers table since that table is not used in the gateways.
    $sqlStatement = "DELETE FROM SERVERS WHERE SERVERID > 0";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the Servers table.\n");
    }
}

########################################################################
# This function will be called after restoring the data storage machine
# of the gateway array. In this function, we are simply inserting 2
# definitions (blessed and unblessed) into the SigsToBeExported table.
# Those 2 definitions will be destined to be sent to the specified
# gateway address as part of the command line argument.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub SendNewDefinitionsToGateway ()
{
    # Define some local variables and initialize them.
    my ($newestSeqNum, $newestBlssdSeqNum, $serverId);
    my $gatewayName;
    $newestSeqNum = 0;
    $newestBlssdSeqNum = 0;
    $serverId = 123456;
    ($gatewayName) = @_;

##########   BEGIN GET THE SERVERID FROM THE SERVERS TABLE ####################
    print ("Creating and executing the SQL statement for getting the SERVERID.\n"); 
    # Form an SQL statement for getting the SERVERID from the SERVERS table.
    $sqlStatement = "SELECT SERVERID FROM SERVERS WHERE URL = \'$gatewayName\'";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return;
    }
    else
    {
        # Process the result row of the previous SQL execution.
        if ($db->FetchRow ())
        {
            undef %Data;
            # Read the value of the SERVERID column into the hash.
            %Data = $db->DataHash ("SERVERID");
            $serverId = $Data {SERVERID};
        }
        else
        {
            print ("Unable to get the SERVERID for '$gatewayName' from the SERVERS table.\n");
            return;
        }
    }

	print ("ServerId read from the database = $serverId.\n");  

##########   END GET THE SERVERID FROM THE GLOBALS TABLE ####################

##########   BEGIN GET THE SEQUENCE NUMBERS FROM THE GLOBALS TABLE ####################
    print ("Creating and executing the SQL file for getting the sequence numbers.\n"); 
    # Form an SQL statement for getting the NewestSeqNum and NewestBlssdSeqNumfrom the Globals table and execute the SQL statement.
    $sqlStatement = "SELECT NEWESTSEQNUM, NEWESTBLSSDSEQNUM FROM GLOBALS";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return;
    }
    else
    {
        # Stay in a loop to process all the rows of the previous SQL execution.
        if ($db->FetchRow ())
        {
            undef %Data;
            # Read the value of the NEWESTSEQNUM and NEWESTBLSSDSEQNUM columns into the hash.
            %Data = $db->DataHash ("NEWESTSEQNUM", "NEWESTBLSSDSEQNUM");
            $newestSeqNum = $Data {NEWESTSEQNUM};
            $newestBlssdSeqNum = $Data {NEWESTBLSSDSEQNUM};
        }
        else
        {
            print ("Unable to get the sequence numbers from the GLOBALS table.\n");
            return;
        }
    }

	print ("NewestSequenceNumber read from the database = $newestSeqNum.\n");  
	print ("NewestBlessedSequenceNumber read from the database = $newestBlssdSeqNum.\n");  

##########   END GET THE SEQUENCE NUMBERS FROM THE GLOBALS TABLE ####################

##########   BEGIN ADD SIGSTOBEEXPORTED ENTRIES WITH BLESSED DEFINITION ####################
    print ("Creating and executing the SQL file for adding a blessed entry in the SIGSTOBEEXPORTED table.\n"); 
    # Form an SQL statement for inserting entries into the SigsToBeExported table and execute the SQL statement.
    $sqlStatement = "INSERT INTO SIGSTOBEEXPORTED (ServerID, SignatureSeqNum) VALUES ($serverId, $newestBlssdSeqNum)";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        print ("Unable to add a blessed entry in the SIGSTOBEEXPORTED table.\n"); 
        print ("If the SQL error refers to a duplicate row, you can ignore this error.\n"); 
    }
    else
    {
        print ("Added a blessed entry in the SIGSTOBEEXPORTED table.\n"); 
    }     
##########   END ADD SIGSTOBEEXPORTED ENTRIES ####################

##########   BEGIN ADD SIGSTOBEEXPORTED ENTRIES WITH UNBLESSED DEFINITION ####################
    print ("Creating and executing the SQL file for adding an unblessed entry in the SIGSTOBEEXPORTED table.\n"); 
    # Form an SQL statement for inserting entries into the SigsToBeExported table and execute the SQL statement.
    $sqlStatement = "INSERT INTO SIGSTOBEEXPORTED (ServerID, SignatureSeqNum) VALUES ($serverId, $newestSeqNum)";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        print ("Unable to add an unblessed entry in the SIGSTOBEEXPORTED table.\n");
        print ("If the SQL error refers to a duplicate row, you can ignore this error.\n"); 
    }
    else
    {
        print ("Added an unblessed entry in the SIGSTOBEEXPORTED table.\n"); 
    }     
##########   END ADD SIGSTOBEEXPORTED ENTRIES ####################
}


########################################################################
# This function will be called in order to insert a new sample in the
# analysisrequest table. It also reads the attributes for that sample
# from the attributes.txt file and add them to the attributes table.
#
# Function arguments: 
#
# This function takes one argument which is the cookie id.
########################################################################
sub InsertNewSampleInDatabase ($)
{
    my ($newCookieID) = @_;

    my $cookieStr;
    $cookieStr = sprintf "%08d", $newCookieID;
    my $attribFileName = "$rootDrive\\SamplesInAnalysis\\$serverName\\SampleStorage\\$cookieStr\\SampleAttributes\\Attributes.txt";

    # Open Attributes File
    unless (open (FILEHANDLE, "<$attribFileName "))
    {
        print ("Error in opening $attribFileName\n");
        return 1;
    }

    # Read all attributes in a map
    my @attributeEntries = <FILEHANDLE>;

    # Delete the trailing newline character from each line
    chop @attributeEntries;
    close FILEHANDLE;

    my $attribute;
    my %attribList = ();
    my ($key, $value);
    foreach $attribute (@attributeEntries)
    {
        ($key, $value) = split /:/, $attribute, 2;

        # Remove leading and trailing white space from the Key and the Value
        $value =~ s/^\s*(.*?)\s*$/$1/;
        $key =~ s/^\s*(.*?)\s*$/$1/;
        if ($key)
        {
	    $key = uc($key);
            $attribList{$key} = $value;
        }
    }

    my ($checksum, $priority, $submittorID, $remoteServerName);
    my ($remoteServerID, $remoteCookie);

    # Read the value of X-Sample-Checksum into $checksum
    $checksum = $attribList{'X-SAMPLE-CHECKSUM'};

    # Read the value of X-Sample-Priority into $priority
    $priority = $attribList{'X-SAMPLE-PRIORITY'};

    # Read the value of X-Customer-Identifier into $submittorID
    $submittorID = $attribList{'X-CUSTOMER-IDENTIFIER'};

    # Read the value of X-Analysis-Cookie into $remoteCookie
    $remoteCookie = $attribList{'X-ANALYSIS-COOKIE'};

    # Read the value of X-Analysis-Gateway into $remoteServerName
    $remoteServerName = $attribList{'X-ANALYSIS-GATEWAY'};

    # Remove the Attribute keys which should not be added to the Attributes table
    delete $attribList{'CURRENT TIME'};
    delete $attribList{'ISSUE TRACKING NUMBER'};
    delete $attribList{'CURRENT STATE'};
    delete $attribList{'X-SAMPLE-CHECKSUM'};
    delete $attribList{'X-CUSTOMER-IDENTIFIER'};
    delete $attribList{'X-SAMPLE-PRIORITY'};
    delete $attribList{'X-ANALYSIS-GATEWAY'};

    if ($submittorID == undef || $submittorID eq "")
    {
       print "SubmittorID is undef. We are forcing it to 'unknown customer'.\n";
       $submittorID = "unknown customer";
    }

    # Check for the submittorID
    $sqlStatement = "SELECT SUBMITTORID FROM Submittors WHERE SubmittorID = \'$submittorID\'";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return 1;
    }
    else
    {
        if (!$db->FetchRow ())
        {
            $sqlStatement = "INSERT INTO Submittors (SubmittorID) VALUES (\'$submittorID\')";

            $rc = $db->Sql ($sqlStatement);

            # Check for any SQL execution errors.
            if ($rc)
            {
                print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
                return 1;
            }
            else
            {
                print ("Successfully inserted a new record with SubmittorID '$submittorID'.\n");
            }
        }
        else
        {
             print "Skipped Inserting duplicate Submittors.\n";
        }
    }

    # Get the $remoteServerID from the servers table
    $sqlStatement = "SELECT SERVERID FROM Servers WHERE URL = \'$remoteServerName\'";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return 1;
    }
    else
    {
        if ($db->FetchRow ())
        {
            undef %Data;

            # Read the value of the SERVERID column into the hash.
            %Data = $db->DataHash ("SERVERID");
            $remoteServerID = $Data {SERVERID};
        }
        else
        {
            print ("Invalid gateway $remoteServerName.\nHence remoteServerID is forcefully set to 1.\n");
            # Forcefully set the $remoteServerID to 1. This value really doesn't matter.
            # Because, we don't use the remoteServerID database field for anything.
            $remoteServerID = 1;
        }
    }

    $sqlStatement = "INSERT INTO AnalysisRequest VALUES ($newCookieID, \'$checksum\', $priority, CURRENT TIMESTAMP, NULL, NULL, CURRENT TIMESTAMP, (SELECT ANALYSISSTATEID FROM AnalysisStateInfo WHERE Name = \'imported\'), X'01', X'01', \'$submittorID\', $remoteServerID, $remoteCookie)";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return 1;
    }
    else
    {
        print ("Successfully inserted new cookieID $newCookieID.\n");
    }


# ========================================================================================
# Starting April/2000, there is no Attributes table in the AVISDATA database.
# The following code was written before April/2000. We don't have to do the
# addition of attributes into that table. (Code commented on April/17/2000)

    my $addAttributes = 0;

    if ($addAttributes != 0)
    {    
        # Insert all attribute key pairs in the Attributes table
        while (($key, $value) = each %attribList)
        {
            my $attributeKey;

            $sqlStatement = "SELECT ATTRIBUTEKEY FROM ATTRIBUTEKEYS WHERE TEXT = \'$key\'";
            $rc = $db->Sql ($sqlStatement);

            # Check for any SQL execution errors.
            if ($rc)
            {
                print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
                next;
            }
            else
            {
                if ($db->FetchRow ())
                {
                    undef %Data;

                    # Read the value of the ATTRIBUTEKEY column into the hash.
                    %Data = $db->DataHash ("ATTRIBUTEKEY");
                    $attributeKey = $Data {ATTRIBUTEKEY};

                    $sqlStatement = "INSERT INTO ATTRIBUTES (LocalCookie, AttributeKey, Value) VALUES ($newCookieID, $attributeKey, \'$value\')";
                    $rc = $db->Sql ($sqlStatement);
                }
                else
                {
                    print ("Invalid Key $key\n");
                    next;
                }
            }
        }
    }
# ========================================================================================

    return 0;
}


########################################################################
# This function will be called when this program is run in the data
# storage machine of the gateway array. This function deletes the unwanted
# records from the Attributes, SigsToBeExported and RemoteMustBeUnique tables
# for samples that have already been imported in the Analysis center. It
# retains the AnalysisRequest records for only those sample which were
# exported by this server and adjusts the LocalCookie number for these
# records. It adjusts
# the NextLocalCookie number to an appropriate value. It cleans up the tables
# such as SigsToBeExported and Servers tables. It removes all the entries
# in the Signatures table except the first 2 rows for definition sequence
# numbers 1 and 2. It readjusts the blessed and unblessed values in the 
# globals tables to 1 and 2 respectively.
#
# Function arguments: 
#
# This function takes one argument which is the HTTP address of the 
# gateway.
########################################################################
#===============
# This function is not needed from April/2000. It is present here only
# for the purpose of a reference code.
#===============
sub OldNotUsedFilterNewGatewaySamples ($)
{
    my ($gatewayName) = @_;
    my ($serverID);

    # get the server ID for the specified gateway
    $sqlStatement = "SELECT SERVERID FROM SERVERS WHERE URL = \'$gatewayName\'";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return 1;
    }
    else
    {
        if ($db->FetchRow ())
        {
            undef %Data;

            # Read the value of the SERVERID column into the hash.
            %Data = $db->DataHash ("SERVERID");
            $serverID = $Data {SERVERID};
            print ("ServerID of $gatewayName = $serverID\n");
        }
        else
        {
            print ("Invalid gateway '$gatewayName'\n");
            return 1;
        }
    }

    # delete all records from attributes table
    $sqlStatement = "DELETE FROM ATTRIBUTES";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the Attributes table.\n");
    }

    # delete all records from RemoteMustBeUnique table
    $sqlStatement = "DELETE FROM REMOTEMUSTBEUNIQUE";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the RemoteMustBeUnique table.\n");
    }

    # delete all records from SigsToBeExported table
    $sqlStatement = "DELETE FROM SIGSTOBEEXPORTED";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the SigsToBeExported table.\n");
    }

    # delete all requests not received from this serverID
    $sqlStatement = "DELETE FROM ANALYSISREQUEST WHERE REMOTESERVERID <> $serverID OR IMPORTED IS NULL";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the AnalysisRequest table which came from other servers.\n");
    }

    # Update LocalCookie as RemoteCookie
    $sqlStatement = "UPDATE ANALYSISREQUEST SET LocalCookie = RemoteCookie";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Updated LocalCookie number in the Analysisrequest table.\n");
    }

    # Set RemoteServer, RemoteCookie, NotifyClient and NotifySymantec to NULL
    $sqlStatement = "UPDATE ANALYSISREQUEST SET RemoteServerID = NULL, RemoteCookie = NULL, NotifyClient = X'00', NotifySymantec = X'00'";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Set RemoteServerID, RemoteCookie, NotifyClient, NotifySymantec to NULL in Analysisrequest table.\n");
    }

    my $maxLocalCookie;

    # get the maximum LocalCookie number
    $sqlStatement = "SELECT MAX(LOCALCOOKIE) AS MAXLOCALCOOKIE FROM ANALYSISREQUEST";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return 1;
    }
    else
    {
        if ($db->FetchRow ())
        {
            undef %Data;

            # Read the value of the MAXLOCALCOOKIE column into the hash.
            %Data = $db->DataHash ("MAXLOCALCOOKIE");
            $maxLocalCookie = $Data {MAXLOCALCOOKIE};
        }
        else
        {
            $maxLocalCookie = 0;
        }
    }
    $maxLocalCookie ++;

    # Update Globals Table
    $sqlStatement = "UPDATE GLOBALS SET NewestSeqNum = 2, NewestBlssdSeqNum = 1, NextLocalCookie = $maxLocalCookie, ThisServerID = $serverID, BackOfficeLock = X'00', TypeOfServer = X'00'";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Set new NextLocalCookie to $maxLocalCookie and initialized other values in the Globals table.\n");
    }

    # delete all records from Server table Where ServerID is not
    $sqlStatement = "DELETE FROM SERVERS WHERE SERVERID <> $serverID";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the Servers table pointing to other Gateways.\n");
    }

    # delete all signatures record from the signatures table for seqnum > 2
    $sqlStatement = "DELETE FROM SIGNATURES WHERE SIGNATURESEQNUM > 2";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
    }
    else
    {
        print ("Deleted all records from the signatures table for sequence number greater than 2.\n");
    }
}


########################################################################
# This function will be called in order to insert a new sample in the
# analysisrequest table. It also reads the attributes for that sample
# from the attributes.txt file and add them to the attributes table.
#
# Function arguments: 
#
# This function takes one argument which is the cookie id.
########################################################################
sub InsertNewGatewaySampleInDatabase ()
{
    my ($checksum, $newCookieID, $gatewaySampleDir) = @_;

    my $attribFileName = "$gatewaySampleDir\\$checksum.attributes";

    # Open Attributes File
    unless (open (FILEHANDLE, "<$attribFileName "))
    {
        print ("Error in opening $attribFileName\n");
        return 1;
    }

    # Read all attributes in a map
    my @attributeEntries = <FILEHANDLE>;

    # Delete the trailing newline character from each line
    chop @attributeEntries;
    close FILEHANDLE;

    my $attribute;
    my %attribList = ();
    my ($key, $value);
    foreach $attribute (@attributeEntries)
    {
        ($key, $value) = split /:/, $attribute, 2;

        # Remove leading and trailing white space from the Key and the Value
        $value =~ s/^\s*(.*?)\s*$/$1/;
        $key =~ s/^\s*(.*?)\s*$/$1/;
        if ($key)
        {
	    $key = uc($key);
            $attribList{$key} = $value;
        }
    }

    my ($priority, $submittorID);

    # Read the value of X-Sample-Priority into $priority
    $priority = $attribList{'X-SAMPLE-PRIORITY'};

    # Read the value of X-Customer-Identifier into $submittorID
    $submittorID = $attribList{'X-CUSTOMER-IDENTIFIER'};

    # Remove the Attribute keys which should not be added to the Attributes table
    delete $attribList{'CURRENT TIME'};
    delete $attribList{'ISSUE TRACKING NUMBER'};
    delete $attribList{'CURRENT STATE'};
    delete $attribList{'X-SAMPLE-CHECKSUM'};
    delete $attribList{'X-CUSTOMER-IDENTIFIER'};
    delete $attribList{'X-SAMPLE-PRIORITY'};
    delete $attribList{'X-ANALYSIS-GATEWAY'};

    if ($submittorID == undef || $submittorID eq "")
    {
       print "SubmittorID is undef. We are forcing it to 'unknown customer'.\n";
       $submittorID = "unknown customer";
    }

    # Check for the submittorID
    $sqlStatement = "SELECT SUBMITTORID FROM Submittors WHERE SubmittorID = \'$submittorID\'";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return 1;
    }
    else
    {
        if (!$db->FetchRow ())
        {
            $sqlStatement = "INSERT INTO Submittors (SubmittorID) VALUES (\'$submittorID\')";

            $rc = $db->Sql ($sqlStatement);

            # Check for any SQL execution errors.
            if ($rc)
            {
                print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
                return 1;
            }
            else
            {
                print ("Successfully inserted a new record with SubmittorID '$submittorID'.\n");
            }
        }
        else
        {
             print "Skipped Inserting duplicate Submittors.\n";
        }
    }

    $sqlStatement = "INSERT INTO AnalysisRequest VALUES ($newCookieID, \'$checksum\', $priority, CURRENT TIMESTAMP, NULL, NULL, CURRENT TIMESTAMP, (SELECT ANALYSISSTATEID FROM AnalysisStateInfo WHERE Name = \'accepted\'), X'00', X'00', \'$submittorID\', NULL, NULL)";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return 1;
    }
    else
    {
        print ("Successfully inserted new cookieID $newCookieID.\n");
    }


# ========================================================================================
# Starting April/2000, there is no Attributes table in the AVISDATA database.
# The following code was written before April/2000. We don't have to do the
# addition of attributes into that table. (Code commented on April/17/2000)

    my $addAttributes = 0;

    if ($addAttributes != 0)
    {    
        # Insert all attribute key pairs in the Attributes table
        while (($key, $value) = each %attribList)
        {
            my $attributeKey;

            $sqlStatement = "SELECT ATTRIBUTEKEY FROM ATTRIBUTEKEYS WHERE TEXT = \'$key\'";
            $rc = $db->Sql ($sqlStatement);

            # Check for any SQL execution errors.
            if ($rc)
            {
                print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
                next;
            }
            else
            {
                if ($db->FetchRow ())
                {
                    undef %Data;

                    # Read the value of the ATTRIBUTEKEY column into the hash.
                    %Data = $db->DataHash ("ATTRIBUTEKEY");
                    $attributeKey = $Data {ATTRIBUTEKEY};

                    $sqlStatement = "INSERT INTO ATTRIBUTES (LocalCookie, AttributeKey, Value) VALUES ($newCookieID, $attributeKey, \'$value\')";
                    $rc = $db->Sql ($sqlStatement);
                }
                else
                {
                    print ("Invalid Key $key\n");
                    next;
                }
            }
        }
    }
# ========================================================================================

    return 0;
}
