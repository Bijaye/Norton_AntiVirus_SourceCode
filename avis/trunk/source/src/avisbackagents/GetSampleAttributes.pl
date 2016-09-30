###############################################################
# GetSampleAttributes.pl
#
# This program obtains the sample attributes stored in the
# database and stores it in a file under the cookie directory. 
# The data stored in the file can be viewed optionally from the
# DataFlow program or by a text editor.
#
# Authors: Senthil Velayudham, Srikant Jalan
# Last Modified: Apr/23/2000
#
###############################################################
	# Specify the PERL modules used by this program.
    use Getopt::Long;
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

    # Derive all the cookie subdirectories under the SampleStorage directory.
    my $tmpDirListFile = "$isdnBinDir\\dir.out";
    system ("dir /b $isdnServerRoot > $tmpDirListFile");

    open (FILEHANDLE, "<$tmpDirListFile");

    # Read all the directory entries in this directory into an array. Avoid reading "." and ".." entries.
    my @directoryEntries = <FILEHANDLE>;
    chop @directoryEntries;
    close FILEHANDLE;
    unlink $tmpDirListFile;

    # Calculate the total number of sample directories in the SampleStorage.
    my $totalNumberOfDirEntries;
    $totalNumberOfDirEntries = @directoryEntries;  
    print ("Total dir entries = $totalNumberOfDirEntries\n");

    # If there are no directory entries in the SampleStorage, exit the program now.
    if ($totalNumberOfDirEntries == 0)
    {
        exit;
    }

    my ($dirName, $line, @fileContents, $currentState, @attributes);
    my ($db, $sqlStatement, $rc, %Data);

    # Create an ODBC object to make transactions with AVISDATA.
#    $db = new Win32::ODBC ("dsn=" . $db2DSN . ";uid=" . $db2UserID . ";pwd=" . $db2Password);
    $db = new Win32::ODBC ("dsn=" . $db2DSN);

    # If we are unable to get an ODBC object, exit from this program now.
    if ($db == undef)
    {
        print "Error in <new Win32::ODBC>.\n";
        exit;
    }


    # Stay in a loop and process every sample directory.     
    foreach $dirName (@directoryEntries)
    {
        print ("$dirName\n");

        # If this is not a subdirectory, proceed to the next cookie directory.
        if (!(-d "$isdnServerRoot\\$dirName"))
        {
            next;
        }

        # If it doesn't contain a "sample" directory, this is not a cookie directory.
        # In that case, proceed to the next cookie directory.
        if (!(-d "$isdnServerRoot\\$dirName\\sample"))
        {
            next;
        }

        my $sampleAttributesDir = "$isdnServerRoot\\$dirName\\SampleAttributes";

        # If there is an "attributes.txt" file present, we don't have to create it again.
        # In that case, proceed to the next cookie directory.
        if (-e "$sampleAttributesDir\\attributes.txt")
        {
            next;
        }

        # Create a subdirectory to store the sample attributes in a file.
        if (!(-d $sampleAttributesDir))
        {
            mkdir ($sampleAttributesDir, 777);
        }

        #Call a function to get all the attributes for the current cookie.
        GetSampleAttributes ($dirName, $sampleAttributesDir);
        # Clear the contents of the attributes array for the next sample.
        @attributes = ();

    }

    $db->Close ();
    exit;


########################################################################
# This subroutine retrieves the sample attributes stored in the database
# and writes them to a text file in the cookie directory.
#
# Function arguments: 
#
# 1) Cokkie id.
# 2) Full path to the attribute sub-directory inside the sample directory.
########################################################################
sub GetSampleAttributes ($$)
{
	# Parse the function arguments into local variables. 
    my ($cookie, $attributesDir) = @_;

    # Convert the cookieId into a string containing 8 digits. 
    my ($CookieStr, $attributeFileName);
    $CookieStr = sprintf "%08d", $cookie;
    $attributeFileName = "$attributesDir\\attributes.txt";

    # Create a file where all the attributes of a sample will be dumped.
    unless (open FILEHANDLE, ">$attributeFileName")
    {
        return;
    }

    # Write time, cookieId and currentState to the file.
    my $currentTime = getCurrentTime ();
    print FILEHANDLE ("Current Time: $currentTime\n");
    print FILEHANDLE ("Issue Tracking Number: $CookieStr\n");

    # Form an SQL statement for getting the current state of the cookie and execute the SQL statement.
    $sqlStatement = "SELECT NAME FROM ANALYSISSTATEINFO WHERE ANALYSISSTATEID = (SELECT ANALYSISSTATEID FROM ANALYSISREQUEST WHERE LOCALCOOKIE = $cookie)";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print FILEHANDLE ("Sql Failed. Error =" . $db->Error (). "\n");
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

        # Write currentState to the file.
        print FILEHANDLE ("Current State: $currentState\n");
    }

    # Form an SQL statement for getting the checksum, customerid, priority of the cookie and execute the SQL statement.
    $sqlStatement = "SELECT CHECKSUM, SUBMITTORID, PRIORITY FROM ANALYSISREQUEST WHERE LOCALCOOKIE = $cookie";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print FILEHANDLE ("Sql Failed. Error =" . $db->Error (). "\n");
        print FILEHANDLE ("\nX-Sample-Checksum: Unknown\n");
        print FILEHANDLE ("X-Customer-Identifier: Unknown\n");
        print FILEHANDLE ("X-Sample-Priority: Unknown\n");
    }
    else
    {
        if ($db->FetchRow ())
        {
            my ($checksum, $customerID, $priority);
            undef %Data;

            # Read the value of the NAME column into the hash.
            %Data = $db->DataHash ("CHECKSUM", "SUBMITTORID", "PRIORITY");
            $checksum = $Data {CHECKSUM};
            $customerID = $Data {SUBMITTORID};
            $priority = $Data {PRIORITY};

            print FILEHANDLE ("\nX-Sample-Checksum: $checksum\n");
            print FILEHANDLE ("X-Customer-Identifier: $customerID\n");
            print FILEHANDLE ("X-Sample-Priority: $priority\n");
        }
        else
        {
            print FILEHANDLE ("\nX-Sample-Checksum: Unknown\n");
            print FILEHANDLE ("X-Customer-Identifier: Unknown\n");
            print FILEHANDLE ("X-Sample-Priority: Unknown\n");
        }
    }

    # Form an SQL statement for getting the gateway URL of the cookie and execute the SQL statement.
    $sqlStatement = "SELECT URL FROM ANALYSISREQUEST, SERVERS WHERE LOCALCOOKIE = $cookie AND ANALYSISREQUEST.REMOTESERVERID = SERVERS.SERVERID";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print FILEHANDLE ("Sql Failed. Error =" . $db->Error (). "\n");
        print FILEHANDLE ("X-Analysis-Gateway: Unknown\n\n");
    }
    else
    {
        if ($db->FetchRow ())
        {
            my ($url);
            undef %Data;

            # Read the value of the NAME column into the hash.
            %Data = $db->DataHash ("URL");
            $url = $Data {URL};

            print FILEHANDLE ("X-Analysis-Gateway: $url\n\n");
        }
        else
        {
            print FILEHANDLE ("X-Analysis-Gateway: Unknown\n\n");
        }
    }

    # Form an SQL statement for getting the attributes of the current cookie and execute the SQL statement.
    $sqlStatement = "SELECT TEXT, VALUE FROM ATTRIBUTES, ATTRIBUTEKEYS WHERE LOCALCOOKIE = $cookie AND ATTRIBUTES.ATTRIBUTEKEY = ATTRIBUTEKEYS.ATTRIBUTEKEY ORDER BY TEXT";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print FILEHANDLE ("Sql Failed. Error =" . $db->Error (). "\n");
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
            # Save the attributes into an array.
	        push @attributes, "$key: $value";
        }

        # Check how many keys we have added to the hash.
#        my $totalKeys;
#        $totalKeys = @attributes;  

#        print FILEHANDLE ("Total attributes: $totalKeys\n\n");

        # Iterate through the array and print each key:value pair to the file. 
        foreach $line (@attributes)
        {
            print FILEHANDLE ("$line\n");
        }
    }

    # Close the file.
    close (FILEHANDLE);
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
