#---------------------------------------------------------------------------------------
# avissampleexport.pl
# The procedure backs up a sample stored in the sample's Storage direcory.
# The files are stored in the weekday\tables (Mon, Tue, Wed, Thu, Fri, Sat, Sun)
# subdirectory of the backup directory.
#
#----------------------------------------------------------------------------------------     
# History: 02/21/2000  S.N. Krasikov - created 
#          03/21/2000  S.N. Krasikov  - commented out 'getattribute' 
#          03/21/2000  S.N. Krasikov  - deleted the week day 
#          06/05/2000  S.N. Krasikov  - replaced return with exit when there are no samples available to export 
#----------------------------------------------------------------------------------------
    use File::Copy;
#    use AVIS::Local;
#    use Win32::ODBC;

    # NOTE: 	Please go through the customization block
    #		and change the directories and files' names
    #		and paths where it is needed.  
    #---------------------------------------------
    #   customization block begins
    #------------------------------------------
    # sample storage directory  
    # ------------------------ 
    #    $samplestoragedir="i:\\backup";
         $samplestoragedir="\\\\Rushmore\\samplestorage";
    # ------------------------ 
    # zip file name  
    # ------------------------ 
    # main backup directory  
    # ------------------------ 
        $mainbackupdir="\\\\Rushmore\\acbackup";
    # ------------------------ 
    # zip program  
    # ------------------------ 
	$zipexe  = "d:\\tools\\zip.exe";       
    #---------------------------------------------
    #   customization block ends
    #------------------------------------------
#    ($line,  $currentState, @attributes);
#    ($db, $sqlStatement, $rc, %Data);
#
# Create an ODBC object to make transactions with AVISDATA.
#    $db = new Win32::ODBC ("dsn=" . $db2DSN . ";uid=" . $db2UserID . ";pwd=" . #$db2Password);
#
# If we are unable to get an ODBC object, exit from this program now.
#    if ($db == undef)
#    {
#        print "Error in <new Win32::ODBC>.\n";
#        exit;
#    }





        # check for backup dir
#        if (!(-d $mainbackupdir))
#        {
#            if (!mkdir ("$mainbackupdir", 0)) {
#	        print ("Unable to create directory $mainbackupdir\n");
#                exit;
#            }
#        }
        # create a "weekday" directory if it does not exist 
	$currtime = time(); 
       	$ctime = localtime($currtime);
        @words = split (/ /, $ctime);
        $result = $mainbackupdir =~ /\\$/;
        if (!$result) {
            $mainbackupdir = $mainbackupdir . "\\";
        }
#        $backupdir2=$mainbackupdir . $words[0];
#        if (!(-d $backupdir2))
#        {
#            if (!mkdir ("$backupdir2", 0)) {
#	        print ("Unable to create directory $backupdir2\n");
#                exit;
#            }
#        }




        $result = $samplestoragedir =~ /\\$/;
        if (!$result) {
          $samplestoragedir = $samplestoragedir . "\\";
        }
	# Read the specified directory and store the names it contains in 
    	# an array.
    

#    	$dirlist = $mainbackupdir . "\\";
    	$dirlist = $dirlist . "dir.list";

    	system ("dir /b $samplestoragedir > $dirlist");

	open (FILEHANDLE, "<$dirlist");

	my @names = <FILEHANDLE>;
    	chop @names;
    	close FILEHANDLE;
    	unlink $dirlist;
    	# if the list is empty, return.
    	my $numberOfEntries;
    	$numberOfEntries = @names;  
    	print ("Total dir entries = $numberOfEntries\n");

    	# If there are no directory entries in the SampleStorage, exit the program now.
    	if ($numberOfEntries == 0)
    	{
        	exit;
    	}

      

	# For each name in the directory that is a sub-directory (except the
    	# directories "." and ".."), go into sample directory and look for a file sample
    	# sample.original						    
    	foreach (@names)
    	{      
                $subdir = $samplestoragedir . "$_";
		next unless -d $subdir;
		next if $_ eq ".";
		next if $_ eq "..";
                $SampleOriginal = $subdir . "\\sample\\sample.original";
                $SampleAttributes = $subdir . "\\sampleattributes\\attributes.txt";
                $ImportComplete = $subdir . "\\import.complete";
                $ExportComplete = $subdir . "\\BackOfficeSampleExport.complete";
                $Done = $subdir . "\\BackOfficeSampleExport.complete";
                $zipname = "$_" . ".";                
                $zipname = $zipname . "sample.original";                
                $zipname = $zipname . ".zip";                
#                $zippath = $mainbackupdir . "\\";
                $zippath = $mainbackupdir . $zipname;

                if ((-e $SampleOriginal))
	        {
	                if ((-e $ImportComplete))
		        {
	 	                if (!(-e $Done))
		        	{ 
				                                                																										
					$sampleAttributesDir = "$subdir\\SampleAttributes";


			        	# Create a subdirectory to store the sample attributes in a file.
#     	   				if (!(-d $sampleAttributesDir)) {
#            					mkdir ($sampleAttributesDir, 777);
#        				}

	        			#Call a function to get all the attributes for the current cookie.
#        				GetSampleAttributes ($_, $sampleAttributesDir);
        				# Clear the contents of the attributes array for the next sample.
#        				@attributes = ();



					system("$zipexe $zippath $SampleOriginal $SampleAttributes\n");     																



					open (DONE, ">$ExportComplete");
                        		close(DONE); 
                                }
                        }
                } 
        }


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
sub getCurrentTime ()
{
    my ($sec, $min, $hr, $mday, $mon, $year, $wday, $yday, $isdst) = localtime (time);
    $year += 1900; #Y2K compliance 
    $mon++;
    return ("<$mon/$mday/$year $hr:$min:$sec> ");
}


