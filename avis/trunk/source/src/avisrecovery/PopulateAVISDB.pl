###############################################################
# PopulateAVISDB.pl
#
# This program populates the Analysis Request table with 
# sequential cookie ids. It assigns a random md5 checksum for
# those cookies. It also creates a result entry for all
# those cookie ids in the Analysis Results table and declares
# those samples as clean files.
#
# This program accepts the following command line options.
# perl PopulateAVISDB.pl <SampleCnt> <StartCnt>
# (OR)
# perl PopulateAVISDB.pl <CleanFilesDirectory>
# 
# Authors: Senthil Velayudham, Srikant Jalan
# Created on: Feb/07/2000
# Last Modified: June/01/2000
#
###############################################################
	# Specify the PERL modules used by this program.
    use Getopt::Long;
    use strict;
    use Win32::ODBC;
    use MD5;

    my ($totalCookiecount, $startCount, $cleanFilesDir, $argc);
    my (%Data, $checksum, $uninfectedID, $currentCookieCnt, $dupFileCnt);
    my (@attributeKeys);

    # Get the total number of command line arguments.
    $argc = @ARGV;

    if ($argc == 1)
    {
        # Only one argument means, they want us to add the checksums of
        # known clean files. User should provide us a directory name containing
        # the clean files.
        ($cleanFilesDir) = @ARGV;
    }
    elsif ($argc == 2)
    {
        # Two arguments mean, they want us to create a random list of checksums.
        # In this case, the user should provide us the maximum number of
        # checksums to be generated and starting cookie number from which we should
        # start.
        ($totalCookiecount, $startCount) = @ARGV;
    }


    if ((!(defined $totalCookiecount) ||
        !(defined $startCount)) &&
        !(defined $cleanFilesDir))
    {
        print ("Usage: perl PopulateAVISDB.pl <CleanFilesDirectory>\n");
#        print ("(OR)\n");
#        print ("Usage: perl PopulateAVISDB.pl <ChecksumCnt> <RandomChecksumFlag>\n");
        print ("\n=====================================================================\n");
        print ("Using this program, you can add a set of clean file checksums to the\n");
        print ("ANALYSISRESULTS table of the AVISDATA database. Please provide the\n");
        print ("name of the directory under which the clean files are stored.\n");
        print ("This program will recursively traverse through all the subdirectories\n");
        print ("and compute the MD5 checksum of all the files. Then it will add those\n");
        print ("MD5 checksums to the ANALYSISRESULTS table as known clean checksums.\n");
        print ("\nExample: perl PopulateAVISDB.pl t:\\cleanfiles\n");
        print ("=====================================================================\n");
        exit;
    }


    print ("Usage: perl PopulateAVISDB.pl <CleanFilesDirectory>\n");
#    print ("(OR)\n");
#    print ("Usage: perl PopulateAVISDB.pl <ChecksumCnt> <RandomChecksumFlag>\n");
    print ("\n=====================================================================\n");
    print ("This program makes changes to the AVISDATA database. This program\n");
    print ("should be run in isolation. Please ensure that you don't run either\n");
    print ("the Lotus GO server or the AVISACSampleComm program at this time.\n");
    print ("Press Enter to continue or Ctrl-C to exit this program.\n");
    print ("=====================================================================\n");
    my $key = getc ();

    my ($db, $sqlStatement, $rc);
    my ($db2DSN, $db2UserID, $db2Password, $currentTime);

    $db2DSN = "AVISDATA";
#    $db2UserID = "admin";
#    $db2Password = "wa3syj";

    # Create an ODBC object to make transactions with AVISDATA.
    #$db = new Win32::ODBC ("dsn=" . $db2DSN . ";uid=" . $db2UserID . ";pwd=" . $db2Password);
    $db = new Win32::ODBC ("dsn=" . $db2DSN);

    # If we are unable to get an ODBC object, exit from this program now.
    if ($db == undef)
    {
        print "Error in <new Win32::ODBC>.\n";
        exit;
    }

    GetUninfectedID ();

    if ($uninfectedID == -1)
    {
        exit;
    }

    # If $argc is 1, that means user has provided us a directory where
    # all the clean files are avaialble. All we have to do is simply
    # compute the checksums of those clean files add new request and 
    # result records.
    if ($argc == 1)
    {
        # If we have to only add checksums of a known set of clean files,
        # this code block will be used.

        $dupFileCnt = 0;
        $currentCookieCnt = 0;
        $currentTime = getCurrentTime ();
        print ("We are creating cookies of clean files from $cleanFilesDir $currentTime.\n");        
        CreateCookiesUsingCleanChecksums ($cleanFilesDir);
        $currentTime = getCurrentTime ();
        print ("We created $currentCookieCnt entries with clean checksums.$currentTime.\n");
    }
    else
    {
        # If we have to create our own random checksums, this code block
        # will be used.
        CreateCookiesUsingRandomChecksums ();
    }

    $db->Close ();
    exit;

########################################################################
# This subroutine creates a set of database records using the checksums 
# of the clean files provided by the user.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub CreateCookiesUsingCleanChecksums ($)
{
    # Parse the function argument. 
    my ($cleanFilesDir) = @_;

    # Check if the sample directory exists.           
    if (!(-d $cleanFilesDir))
    {
        # Invalid directory. Exit the program now.
        print ("$cleanFilesDir directory doesn't exist " . getCurrentTime() . ".\n");
        return;
    }    

    unless (opendir DIRHANDLE, $cleanFilesDir)
    {
        # Unable to open the sample directory. Exit the program now.
        print ("Unable to open $cleanFilesDir directory using opendir" . getCurrentTime() . ".\n");
        return;
    }

    # Read all the filenames in this directory into an array. Avoid reading "." and ".." entries.
    my @filesToBeZapped = grep !/^\.\.?$/, readdir DIRHANDLE;
    closedir DIRHANDLE;

    my ($fileName);

    # Stay in a loop and create a record in the analysisrequest and the 
    # analysisresults tables.
    foreach $fileName (@filesToBeZapped)
    {
        if (-d "$cleanFilesDir\\$fileName")
        {
            CreateCookiesUsingCleanChecksums ("$cleanFilesDir\\$fileName");
            next;
        }    
        
        print ("$currentCookieCnt, $dupFileCnt, $cleanFilesDir\\$fileName\n");

        # Calculate the MD5 checksum of this sample.
        $checksum = ComputeMD5Checksum ("$cleanFilesDir\\$fileName");
        if ($checksum eq "WrongChecksum")
        {
            print ("Skipping $cleanFilesDir\\$fileName\n");
            next;
        }

        # Form an SQL statement for inserting analysis results and execute the SQL statement.
        $sqlStatement = "insert into ANALYSISRESULTS values (\'$checksum\', NULL, $uninfectedID)";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            #print ("\n\n$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
            print ("Duplicate $cleanFilesDir\\$fileName\n");
            $dupFileCnt++;
            next;
        }        
        else
        {
            $currentCookieCnt++;
        }
    }
}

########################################################################
# This subroutine creates a set of database records using random
# checksums. All those cookies matching these checksums will be 
# declared as clean files.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub NewCreateCookiesUsingRandomChecksums ()
{
    $currentTime = getCurrentTime ();
    print ("We are creating $totalCookiecount cookies $currentTime.\n");
    my ($previousChecksum, $md5);
    $previousChecksum = "6df23dc03f9b54cc38a0fc1483df6e21";
    $checksum = $previousChecksum;
    my $cnt;
    my $dupcnt = 0;
    my $xdupcnt;

    # Now go ahead and create the requested number of cookie records.
    $md5 = new MD5;

    for ($cnt = 1; $cnt <= $totalCookiecount; $cnt++)
    {
        # Try to get a checksum that is different from the previous one.
        $xdupcnt = 0;
        while ($checksum == $previousChecksum)
        {
            $md5->reset();
            $md5->add($xdupcnt, times, time, $previousChecksum);
            $checksum = $md5->hexdigest;
            $xdupcnt++;
        }

        $previousChecksum = $checksum;

        print ("Creating $cnt of $totalCookiecount ($dupcnt, $xdupcnt)          \r");

        # Form an SQL statement for inserting analysis results and execute the SQL statement.
        $sqlStatement = "insert into ANALYSISRESULTS values (\'$checksum\', NULL, $uninfectedID)";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
#            print ("\n\n$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
            $cnt--;
            $dupcnt++;
            next;
        }        
	}      

    $currentTime = getCurrentTime ();
    print ("We created $totalCookiecount cookies $currentTime.\n");
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

########################################################################
# This subroutine computes the MD5 checksum of a given file.
#
#
# Function arguments: 
#
# 1) Name of the file for which the MD5 checksum has to be computed.
#
# This function computes the MD5 checksum and returns it.
########################################################################
sub ComputeMD5Checksum ($)
{
    # Parse the function argument. 
    my ($fileName) = @_;

    my $fileSize = -s $fileName;
    if ($fileSize >= 100000000)
    {
        return ("WrongChecksum");
    }

    # If the file can't be opened, return now.
    unless (open FILEHANDLE, $fileName)
    {
        return ("WrongChecksum");
    }


    # Set binary mode for I/O operations.
    binmode (FILEHANDLE);
    # Seek to the beginning of the file.
    seek(FILEHANDLE, 0, 0);
    # Compute the MD5 checksum.
    my $md5 = new MD5;
    $md5->reset;
    $md5->add(<FILEHANDLE>);
    my $d = $md5->hexdigest;

    # Close the file and return the computed checksum value. 
    close(FILEHANDLE);
    return ($d);
}


########################################################################
# This subroutine gets the value for the uninfected string from
# the ANALYSISSTATEINFO table.
#
#
# Function arguments: 
# No arguments required for this function.
#
########################################################################
sub GetUninfectedID ()
{
    $uninfectedID = -1;
    # Form an SQL statement for getting the next available local cookie number.
    $sqlStatement = "select ANALYSISSTATEID FROM ANALYSISSTATEINFO WHERE NAME = 'uninfected'";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("(select analysisstateid from analysisstateinfo)Sql Failed. Error =" . $db->Error (). "\n");
        return;
    } 
    else
    {
        # Read the value of the ANALYSISSTATEID
        if ($db->FetchRow ())
        {
            undef %Data;

            # Read the value of the ANALYSISSTATEID column into the hash.
            %Data = $db->DataHash ("ANALYSISSTATEID");
            $uninfectedID = $Data {ANALYSISSTATEID};
        }
        else
        {
            print ("(select analysisstateid from analysisstateinfo) $db->FetchRow() returned 0.\n");
            return;
        }
    }
}



########################################################################
# This subroutine gets the nextlocalcookie from the globals table.
#
#
# Function arguments: 
# No arguments required for this function.
#
########################################################################
sub GetNextLocalCookie ()
{
    $startCount = 0;
    # Form an SQL statement for getting the next available local cookie number.
    $sqlStatement = "select nextlocalcookie from globals";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("(select nextlocalcookie from globals)Sql Failed. Error =" . $db->Error (). "\n");
        return;
    } 
    else
    {
        # Read the value of the NEXTLOCALCOOKIE
        if ($db->FetchRow ())
        {
            undef %Data;

            # Read the value of the NEXTLOCALCOOKIE column into the hash.
            %Data = $db->DataHash ("NEXTLOCALCOOKIE");
            $startCount = $Data {NEXTLOCALCOOKIE};
        }
        else
        {
            print ("(select nextlocalcookie from globals) $db->FetchRow() returned 0.\n");
            return;
        }
    }
}


########################################################################
# This subroutine sets the nextlocalcookie in the globals table.
#
#
# Function arguments: 
# No arguments required for this function.
#
########################################################################
sub SetNextLocalCookie ()
{
    # Update the NEXTLOCALCOOKIE in the GLOBALS table.
    # This action has to be synchronized with other programs that create
    # new records in the analysisrequest table. It is advised that program be
    # run in isolation when other programs like Gateway or Sample importer are not
    # running.
    if ($currentCookieCnt > $startCount)
    {
        my $nextLocalCookie = $currentCookieCnt;
        # Form an SQL statement for updating the NEXTLOCALCOOKIE field and execute the SQL statement.
        $sqlStatement = "update GLOBALS set NEXTLOCALCOOKIE = $nextLocalCookie";
        # We are not interested in the result of this SQL execution.
        $rc = $db->Sql ($sqlStatement);        
    }
}

########################################################################
# This subroutine creates a set of database records using the checksums 
# of the clean files provided by the user.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub OldCreateCookiesUsingCleanChecksums ($)
{
    # Parse the function argument. 
    my ($cleanFilesDir) = @_;

    # Check if the sample directory exists.           
    if (!(-d $cleanFilesDir))
    {
        # Invalid directory. Exit the program now.
        print ("$cleanFilesDir directory doesn't exist " . getCurrentTime() . ".\n");
        return;
    }    

    unless (opendir DIRHANDLE, $cleanFilesDir)
    {
        # Unable to open the sample directory. Exit the program now.
        print ("Unable to open $cleanFilesDir directory using opendir" . getCurrentTime() . ".\n");
        return;
    }

    # Read all the filenames in this directory into an array. Avoid reading "." and ".." entries.
    my @filesToBeZapped = grep !/^\.\.?$/, readdir DIRHANDLE;
    closedir DIRHANDLE;

    my ($priority, $fileName);
    $priority = 1;

    # Stay in a loop and create a record in the analysisrequest and the 
    # analysisresults tables.
    foreach $fileName (@filesToBeZapped)
    {
        if (-d "$cleanFilesDir\\$fileName")
        {
            CreateCookiesUsingCleanChecksums ("$cleanFilesDir\\$fileName");
            next;
        }    
        
        print ("$currentCookieCnt, $dupFileCnt, $cleanFilesDir\\$fileName\n");

        # Calculate the MD5 checksum of this sample.
        $checksum = ComputeMD5Checksum ("$cleanFilesDir\\$fileName");
        if ($checksum eq "WrongChecksum")
        {
            print ("Skipping $cleanFilesDir\\$fileName\n");
            next;
        }

        print ("$currentCookieCnt, $dupFileCnt, $cleanFilesDir\\$fileName\n");

        # Form an SQL statement for inserting analysis records and execute the SQL statement.
        $sqlStatement = "insert into analysisrequest (LOCALCOOKIE, PRIORITY, ANALYSISSTATEID, CHECKSUM, SUBMITTORID, IMPORTED) VALUES ($currentCookieCnt, $priority, $uninfectedID, \'$checksum\', \'1\', CURRENT TIMESTAMP)";
        $rc = $db->Sql ($sqlStatement);

        $currentCookieCnt++;

        # Check for any SQL execution errors.
        if ($rc)
        {
            print ("\n\n$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
            next;
        }        


        # Form an SQL statement for inserting analysis results and execute the SQL statement.
        $sqlStatement = "insert into ANALYSISRESULTS values (\'$checksum\', NULL, $uninfectedID)";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            print ("\n\n$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
            print ("Duplicate $cleanFilesDir\\$fileName\n");
            $dupFileCnt++;
            next;
        }        
    }
}

########################################################################
# This subroutine creates a set of database records using random
# checksums. All those cookies matching these checksums will be 
# declared as clean files.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub CreateCookiesUsingRandomChecksums ()
{
    $currentTime = getCurrentTime ();
    print ("We are creating $totalCookiecount cookies $currentTime.\n");
    my ($priority, $previousChecksum, $md5);
    $previousChecksum = "6df23dc03f9b54cc38a0fc1483df6e21";
    $checksum = $previousChecksum;
    my $cnt;
    my $dupcnt = 0;
    my $xdupcnt;

    # Now go ahead and create the requested number of cookie records.
    $md5 = new MD5;

    GetNextLocalCookie();
    GetAttributeKeys();

    for ($cnt = 1; $cnt <= $totalCookiecount; $cnt++)
    {
        $currentCookieCnt = $startCount + $cnt - 1;
        $priority = 1;

        # Try to get a checksum that is different from the previous one.
        $xdupcnt = 0;
        while ($checksum == $previousChecksum)
        {
            $md5->reset();
            $md5->add($xdupcnt, times, time, $previousChecksum);
            $checksum = $md5->hexdigest;
            $xdupcnt++;
        }

        $previousChecksum = $checksum;

        print ("Creating $cnt of $totalCookiecount ($currentCookieCnt, $dupcnt, $xdupcnt)        \r");

        # If we are creating the very first cookie, we have to create a valid
        # submittor id.
        if ($cnt == 1 && $startCount == 1)
        {
            # Form an SQL statement for inserting a valid submittor id and execute the SQL statement.
            $sqlStatement = "insert into submittors (SUBMITTORID) values (\'1\')";
            $rc = $db->Sql ($sqlStatement);

            # Check for any SQL execution errors.
            if ($rc)
            {
                print ("\n\n$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
#               return;
            }
        }

        # Form an SQL statement for inserting analysis records and execute the SQL statement.
        $sqlStatement = "insert into analysisrequest (LOCALCOOKIE, PRIORITY, ANALYSISSTATEID, CHECKSUM, SUBMITTORID, IMPORTED) VALUES ($currentCookieCnt, $priority, $uninfectedID, \'$checksum\', \'1\', CURRENT TIMESTAMP)";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            print ("\n\n$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
            next;
        }        

        AddRandomAttributes ($currentCookieCnt);

        # Form an SQL statement for inserting analysis results and execute the SQL statement.
        $sqlStatement = "insert into ANALYSISRESULTS values (\'$checksum\', NULL, $uninfectedID)";
        $rc = $db->Sql ($sqlStatement);

        # Check for any SQL execution errors.
        if ($rc)
        {
            $dupcnt++;
#            print ("\n\n$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
#            last;
        }        
	}      

    $currentCookieCnt ++;
    SetNextLocalCookie ();

    $currentTime = getCurrentTime ();
    print ("We created $totalCookiecount cookies $currentTime.\n");
}

sub GetAttributeKeys ()
{
    # Form an SQL statement for getting the list of AttributeKeys.
    $sqlStatement = "select ATTRIBUTEKEY from ATTRIBUTEKEYS";
    $rc = $db->Sql ($sqlStatement);

    my ($attributeKey);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print ("$sqlStatement\nSql Failed. Error =" . $db->Error (). "\n");
        return;
    } 
    else
    {
        # Read the value of the ATTRIBUTEKEY
        while ($db->FetchRow ())
        {
            undef %Data;

            # Read the value of the ATTRIBUTEKEY column into the hash.
            %Data = $db->DataHash ("ATTRIBUTEKEY");
            $attributeKey = $Data {ATTRIBUTEKEY};
	        push @attributeKeys, "$attributeKey";
        }
    }
}

sub AddRandomAttributes ($)
{
    my ($attribCookie) = @_;

    my ($attributeKey);

    foreach $attributeKey (@attributeKeys)
    {
        $sqlStatement = "INSERT INTO ATTRIBUTES VALUES ($attribCookie, $attributeKey, \'$attribCookie $attributeKey\')";
        $rc = $db->Sql ($sqlStatement);
    }
}
