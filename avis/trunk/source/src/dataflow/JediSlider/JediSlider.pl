###############################################################
# JediSlider.pl
#
# This program is called as the very first step in the 
# automated analysis. This program reads a configuration file
# (d:\bin\JediSlider.cfg in the Rainier machine) to collect
# the details necessary to perform the slider operation in
# the Jedi release. In this release, Symantec is planning to
# incrementally turn on the automated analysis for macro and
# binary viruses. Symantec can also decide to defer all the
# samples without doing any processing. All the deferred 
# samples will be sent to the Scan And Deliver system for
# analysis. In order to incrementally turn on the system,
# the analysis center operator will have to enable the 
# appropriate options in the JediSlider.cfg file. 
# If needed, only a certain number of test machines can be 
# allowed to do automated analysis. A list of such test machines
# can be provided in the JediSlider.cfg file.
#
# This perl module will be called two times for every sample
# that is taken by the DataFlow program for analysis. 
# At the very first time, this perl module will be called as 
# the very first analysis task for a sample after the dataflow 
# initial rescan. At this time it will get a job sequence number of 1.
#
# This perl module may also be called right after the classifier job
# completes. At this time it will get a job sequence number greater than 1.
#
# Authors: Senthil Velayudham, Srikant Jalan
# Last Modified: Apr/23/2000
#
###############################################################
	# Specify the PERL modules used by this program.
    use Getopt::Long;
    use strict;
    use AVIS::Local;
    use DataFlow;
    use Win32::ODBC;

    # In order to increase the number of test clients, please change the
    # following value.
    my $maxCustomerCnt = 20;

    # Parse the command line arguments from @ARGV.
    my ($Cookie, $ProcessID, $Parameter, $unc);
    my ($doBinaryAnalysisForAll, $doMacroAnalysisForAll, $platformComputer,
        $platformDistributor, $customerContactEmail, $doBinaryAnalysisForThisCustomer,
        $doMacroAnalysisForThisCustomer);
    my $doBinaryAnalysisForAllKey = "DoBinaryAnalysisForAllCustomers = ";
    my $doMacroAnalysisForAllKey = "DoMacroAnalysisForAllCustomers = ";
    my $platformComputerKey = "X-Platform-Computer";
    my $platformDistributorKey = "X-Platform-Distributor";
    # Due to the database schema bug which creates the wrong value
    my $platformDstributorKey = "X-Platform-Dstributor";
    my $customerContactEmailKey = "X-Customer-Contact-Email";
    my $doBinaryAnalysisForThisCustomerKey = "DoBinaryAnalysisForThisCustomer";
    my $doMacroAnalysisForThisCustomerKey = "DoMacroAnalysisForThisCustomer";

    GetOptions("Cookie=i" => \$Cookie,
   "ProcessId=i" => \$ProcessID,
   "Parameter=s" => \$Parameter,
   "UNC=s" => \$unc);
    print "Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc\n";

    my $configurationFile = "$isdnROOT\\bin\\JediSlider.cfg";

    unless (open (FILEHANDLE, "<$configurationFile"))
    {
        print ("Unable to open the configuration file ($configurationFile).\n");
        # Unable to open the configuration file. In that case, defer all the samples.
        SpecifyStopping($Cookie, $ProcessID, $unc, "$isqnDefer", "1 of 1");
        DoNext ($Cookie, $ProcessID, $unc, "$isqnDefer", $Parameter);
        WriteSignatureForDataFlow ($ProcessID, $unc);
        exit;
    }

    # Read all the lines from the configuration file.
    my @configurationEntries = <FILEHANDLE>;
    # Remove the new line character at the end.
    chop @configurationEntries;
    close FILEHANDLE;

    my $configurationEntriesCnt = @configurationEntries;
    my $configurationEntryIndex;

    # Filter out the lines that start with a space, tab, #, ;, //. They are all
    # comment lines.
    for ($configurationEntryIndex = 0; $configurationEntryIndex < $configurationEntriesCnt; $configurationEntryIndex++)
    {
        if (($configurationEntries [$configurationEntryIndex] =~ /^#/) ||
            ($configurationEntries [$configurationEntryIndex] =~ /^;/) ||
            ($configurationEntries [$configurationEntryIndex] =~ /^\/\//) ||
            ($configurationEntries [$configurationEntryIndex] =~ /^ /) ||
            ($configurationEntries [$configurationEntryIndex] =~ /^\n/) ||
            ($configurationEntries [$configurationEntryIndex] =~ /^\r/) ||
            ($configurationEntries [$configurationEntryIndex] =~ /^\t/))
        {
            # Remove this comment line.
            splice (@configurationEntries, $configurationEntryIndex, 1);
            # Readjust the array length.
            $configurationEntriesCnt = @configurationEntries;
            # Since we removed an entry, we have to process the new entry
            # at the current index. Hence decrement the current index by 1.
            $configurationEntryIndex--;
        }
    }

    $configurationEntriesCnt = @configurationEntries;

    if ($configurationEntriesCnt == 0)
    {
        print ("There are no entries in the configuration file.\n");
        SpecifyStopping($Cookie, $ProcessID, $unc, "$isqnDefer", "1 of 1");
        DoNext ($Cookie, $ProcessID, $unc, "$isqnDefer", $Parameter);
        WriteSignatureForDataFlow ($ProcessID, $unc);
        exit;
    }

    # Read the values assigned in the configuration file for performing 
    # the binary analysis for all samples.
    ReadBinaryAnalysisForAllSetting ();
    # Read the values assigned in the configuration file for performing 
    # the macro analysis for all samples.
    ReadMacroAnalysisForAllSetting ();

    print "BinaryAnalysisForAll = $doBinaryAnalysisForAll\n";
    print "MacroAnalysisForAll = $doMacroAnalysisForAll\n";

    my $returnCode = 0;
    my $nextService = "$isqnDefer";
    my $nextServiceAfterClassifier = "";

    if ($ProcessID > 1)
    {
        # if we are here after the classifier, we have to read from the
        # result file created by the classfier.pl to get the original
        # next service intended by the classifier.
        $nextServiceAfterClassifier = GetOriginalNextService ();
        print "NextServiceAfterClassifier = $nextServiceAfterClassifier\n";
    }

    # Read the current customer values from the database.
    $returnCode = ReadCurrentCustomerSettingsFromDatabase ();

    if ($returnCode == 1)
    {
        # Error in reading the database. We will take the action based on the
        # settings made in the configuration file.
        # If the job sequence number is 1, that means we are here before the
        # classifier. In that case we will have to either go to classifier or
        # deferrer based on the configuration settings.
        # If the job sequence number is greater than 1, that means we are here after
        # the classifier. In that case we will have to either go to macro controller
        # or binary controller or deferrer based on the configuration settings.
        if ($ProcessID == 1)
        {
            if ($doBinaryAnalysisForAll == 0 && $doMacroAnalysisForAll == 0)
            {
                # It has been configured to defer all the samples by default.
                $nextService = "$isqnDefer";
            }
            elsif ($doBinaryAnalysisForAll == 1 || $doMacroAnalysisForAll == 1)
            {
                # It has been configured to do automated analysis.
                $nextService = "$isqnSARCExit";
            }
        }

        # If the job sequence number is greater than 1, that means we are here after
        # the classifier. In that case we will have to either go to macro controller
        # or binary controller or deferrer based on the configuration settings.
        if ($ProcessID > 1)
        {
            $nextService = $nextServiceAfterClassifier;

            if ($doBinaryAnalysisForAll == 0 && $nextServiceAfterClassifier eq $isqnBrepctrl)
            {
                # It has been configured not to do the binary analysis. So defer the sample.
                $nextService = "$isqnDefer";
            }
            elsif ($doMacroAnalysisForAll == 0 && $nextServiceAfterClassifier eq $isqnMrepctrl)
            {
                # It has been configured not to do the macro analysis. So defer the sample.
                $nextService = "$isqnDefer";
            }
        }

        print ("Error reading current customer settings from the database.\n");
        print "NextService = $nextService\n";
        $Parameter = "$isfnSamplePathName $isqnClassifier";
        SpecifyStopping($Cookie, $ProcessID, $unc, $nextService, "1 of 1");
        DoNext ($Cookie, $ProcessID, $unc, $nextService, $Parameter);
        WriteSignatureForDataFlow ($ProcessID, $unc);
        exit;        
    }

    print "[DB] X-Platform-Computer = $platformComputer\n";
    print "[DB] X-Platform-Distributor = $platformDistributor\n";
    print "[DB] X-Customer-ContactEmail = $customerContactEmail\n";

    # Read the values assigned in the configuration file for performing the 
    # binary analysis and macro analysis for the current customer.
    $returnCode = ReadBinaryAndMacroAnalysisForThisCustomerSettings ();

    if ($returnCode == 1)
    {
        # There are no current customer settings match found in the configuration file.
        # We will take the action based on the global settings made in the configuration file.
        # If the job sequence number is 1, that means we are here before the
        # classifier. In that case we will have to either go to classifier or
        # deferrer based on the configuration settings.
        # If the job sequence number is greater than 1, that means we are here after
        # the classifier. In that case we will have to either go to macro controller
        # or binary controller or deferrer based on the configuration settings.
        if ($ProcessID == 1)
        {
            if ($doBinaryAnalysisForAll == 0 && $doMacroAnalysisForAll == 0)
            {
                # It has been configured to defer all the samples by default.
                $nextService = "$isqnDefer";
            }
            elsif ($doBinaryAnalysisForAll == 1 || $doMacroAnalysisForAll == 1)
            {
                # It has been configured to do automated analysis.
                $nextService = "$isqnSARCExit";
            }
        }

        # If the job sequence number is greater than 1, that means we are here after
        # the classifier. In that case we will have to either go to macro controller
        # or binary controller or deferrer based on the configuration settings.
        if ($ProcessID > 1)
        {
            $nextService = $nextServiceAfterClassifier;

            if ($doBinaryAnalysisForAll == 0 && $nextServiceAfterClassifier eq $isqnBrepctrl)
            {
                # It has been configured not to do the binary analysis. So defer the sample.
                $nextService = "$isqnDefer";
            }
            elsif ($doMacroAnalysisForAll == 0 && $nextServiceAfterClassifier eq $isqnMrepctrl)
            {
                # It has been configured not to do the macro analysis. So defer the sample.
                $nextService = "$isqnDefer";
            }
        }

        print ("No match found for the current customer settings in the configuration file.\n");
        print "NextService = $nextService\n";
        $Parameter = "$isfnSamplePathName $isqnClassifier";
        SpecifyStopping($Cookie, $ProcessID, $unc, $nextService, "1 of 1");
        DoNext ($Cookie, $ProcessID, $unc, $nextService, $Parameter);
        WriteSignatureForDataFlow ($ProcessID, $unc);
        exit;        
    }
    else
    {
        print "doBinaryAnalysisForThisCustomer = $doBinaryAnalysisForThisCustomer\n";
        print "doMacroAnalysisForThisCustomer = $doMacroAnalysisForThisCustomer\n";

        # We found matching wntries for the current customer settings in the configuration file.
        # We will take the action based on the current customer settings made in the configuration file.
        # If the job sequence number is 1, that means we are here before the
        # classifier. In that case we will have to either go to classifier or
        # deferrer based on the configuration settings.
        # If the job sequence number is greater than 1, that means we are here after
        # the classifier. In that case we will have to either go to macro controller
        # or binary controller or deferrer based on the configuration settings.
        if ($ProcessID == 1)
        {
            if ($doBinaryAnalysisForThisCustomer == 0 && $doMacroAnalysisForThisCustomer == 0)
            {
                # It has been configured to defer all the samples by default.
                $nextService = "$isqnDefer";
            }
            elsif ($doBinaryAnalysisForThisCustomer == 1 || $doMacroAnalysisForThisCustomer == 1)
            {
                # It has been configured to do automated analysis.
                $nextService = "$isqnSARCExit";
            }
        }

        # If the job sequence number is greater than 1, that means we are here after
        # the classifier. In that case we will have to either go to macro controller
        # or binary controller or deferrer based on the configuration settings.
        if ($ProcessID > 1)
        {
            $nextService = $nextServiceAfterClassifier;

            if ($doBinaryAnalysisForThisCustomer == 0 && $nextServiceAfterClassifier eq $isqnBrepctrl)
            {
                # It has been configured not to do the binary analysis. So defer the sample.
                $nextService = "$isqnDefer";
            }
            elsif ($doMacroAnalysisForThisCustomer == 0 && $nextServiceAfterClassifier eq $isqnMrepctrl)
            {
                # It has been configured not to do the macro analysis. So defer the sample.
                $nextService = "$isqnDefer";
            }
        }

        print ("Match found for the current customer settings in the configuration file.\n");
        print "NextService = $nextService\n";
        $Parameter = "$isfnSamplePathName $isqnClassifier";
        SpecifyStopping($Cookie, $ProcessID, $unc, $nextService, "1 of 1");
        DoNext ($Cookie, $ProcessID, $unc, $nextService, $Parameter);
        WriteSignatureForDataFlow ($ProcessID, $unc);
        exit;        
    }


########################################################################
# This subroutine gets the value for the DoBinaryAnalysisForAll tag
# as specified in the JediSlider.cfg file.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub ReadBinaryAnalysisForAllSetting ()
{
    my $configurationEntriesCnt = @configurationEntries;
    my $configurationEntryIndex;
    my $line;

    # Intialize the value to 0.
    $doBinaryAnalysisForAll = 0;

    for ($configurationEntryIndex = 0; $configurationEntryIndex < $configurationEntriesCnt; $configurationEntryIndex++)
    {
        $line = $configurationEntries [$configurationEntryIndex];
        # Remove all leading and trailing white spaces in the line.
        $line =~ s/^\s*(.*?)\s*$/$1/;

        if ($line =~ /^$doBinaryAnalysisForAllKey/i)
        {
            # We found the BinaryAnalysisForAll string.
            # Remove the tag so that we will have only the value of the tag.
            $line =~ s/^$doBinaryAnalysisForAllKey//i;
            # Let us read the value assigned to this tag into the variable.
            $doBinaryAnalysisForAll = $line;
            last;            
        }
    }
}

########################################################################
# This subroutine gets the value for the DoMacroAnalysisForAll tag
# as specified in the JediSlider.cfg file.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub ReadMacroAnalysisForAllSetting ()
{
    my $configurationEntriesCnt = @configurationEntries;
    my $configurationEntryIndex;
    my $line;

    # Intialize the value to 0.
    $doMacroAnalysisForAll = 0;

    for ($configurationEntryIndex = 0; $configurationEntryIndex < $configurationEntriesCnt; $configurationEntryIndex++)
    {
        $line = $configurationEntries [$configurationEntryIndex];
        # Remove all leading and trailing white spaces in the line.
        $line =~ s/^\s*(.*?)\s*$/$1/;

        if ($line =~ /^$doMacroAnalysisForAllKey/i)
        {
            # We found the MacroAnalysisForAll string.
            # Remove the tag so that we will have only the value of the tag.
            $line =~ s/^$doMacroAnalysisForAllKey//i;
            # Let us read the value assigned to this tag into the variable.
            $doMacroAnalysisForAll = $line;
            last;            
        }
    }
}


########################################################################
# This subroutine reads the customer related information from the
# analysis center database.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
# This function returns 0 on success else it returns 1.
########################################################################
sub ReadCurrentCustomerSettingsFromDatabase ()
{
    my ($db, $sqlStatement, $rc, %Data);
    $platformComputer = "";
    $platformDistributor = "";
    $customerContactEmail = "";

    # Create an ODBC object to make transactions with AVISDATA.
#    $db = new Win32::ODBC ("dsn=" . $db2DSN . ";uid=" . $db2UserID . ";pwd=" . $db2Password);
    $db = new Win32::ODBC ("dsn=" . $db2DSN);

    # If we are unable to get an ODBC object, return from this program now.
    if ($db == undef)
    {
        print "Error in <new Win32::ODBC>.\n";
        return 1;
    }
    
    # Form an SQL statement for getting the attributes of the current cookie and execute the SQL statement.
    $sqlStatement = "SELECT TEXT, VALUE FROM ATTRIBUTES, ATTRIBUTEKEYS WHERE LOCALCOOKIE = $Cookie AND ATTRIBUTES.ATTRIBUTEKEY = ATTRIBUTEKEYS.ATTRIBUTEKEY ORDER BY TEXT";
    $rc = $db->Sql ($sqlStatement);

    # Check for any SQL execution errors.
    if ($rc)
    {
        print "Sql Failed while reading sample attributes. Error =" . $db->Error (). "\n";
        $db->Close ();
        return 1;
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

            if ($key eq $platformComputerKey)
            {
                # Store the X-Platform-Computer attribute.
                $platformComputer = $value;
            }
            elsif ($key eq $platformDstributorKey)
            {
                # Store the X-Platform-Distributor attribute.
                $platformDistributor = $value;
            }
            elsif ($key eq $customerContactEmailKey)
            {
                # Store the X-Customer-ContactEmail attribute.
                $customerContactEmail = $value;
            }
        }        
    }

    $db->Close ();

    if ($platformComputer eq "" || $platformDistributor eq "" ||
        $customerContactEmail eq "")
    {
        # We don't have proper value for one of the required keys.
        # return error in this case.
        return 1;
    }

    return 0;
}


########################################################################
# This subroutine gets the value for the DoBinaryAnalysisForThisCustomer
# and DoMacroAnalysisForThisCustomer tags as specified in the 
# JediSlider.cfg file.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
# This function returns 0 if it finds a successful match for the 
# current customer in the configuration file. Otherwise it returns 1.
########################################################################
sub ReadBinaryAndMacroAnalysisForThisCustomerSettings ()
{
    my $configurationEntriesCnt = @configurationEntries;
    my $configurationEntryIndex;
    my $line;
    my $tempCnt;
    my ($computerKey, $distributorKey, $contactEmailKey);
    my ($doBinaryAnalysisKey, $doMacroAnalysisKey);
    my $customerMatchFound;
    my ($computerCfgValue, $distributorCgfValue, $contactEmailCfgValue);


    # Intialize the values to 0.
    $doBinaryAnalysisForThisCustomer = 0;
    $doMacroAnalysisForThisCustomer = 0;
    $customerMatchFound = 0;

    # Try to get a match for the current customer's X-Platform-ComputerName in the
    # configuration file.
    for ($tempCnt = 1; $tempCnt <= $maxCustomerCnt; $tempCnt++)
    {
        $computerCfgValue = "";
        $distributorCgfValue = ""; 
        $contactEmailCfgValue = "";
        $computerKey = "$platformComputerKey" . "$tempCnt = ";
        $distributorKey = "$platformDistributorKey" . "$tempCnt = ";
        $contactEmailKey = "$customerContactEmailKey" . "$tempCnt = ";
        
        for ($configurationEntryIndex = 0; 
            $configurationEntryIndex < $configurationEntriesCnt; $configurationEntryIndex++)
        {
            $line = $configurationEntries [$configurationEntryIndex];
            # Remove all leading and trailing white spaces in the line.
            $line =~ s/^\s*(.*?)\s*$/$1/;

            if ($line =~ /^$computerKey/i)
            {
                # We found the X-Platform-Computer string.
                # Remove the tag so that we will have only the value of the tag.
                $line =~ s/^$computerKey//i;
                $computerCfgValue = $line;
            }
            elsif ($line =~ /^$distributorKey/i)
            {
                # We found the X-Platform-Distributor string.
                # Remove the tag so that we will have only the value of the tag.
                $line  =~ s/^$distributorKey//i;
                $distributorCgfValue  = $line;
            }
            elsif ($line =~ /^$contactEmailKey/i)
            {
                # We found the X-Customer-ContactEmail string.
                # Remove the tag so that we will have only the value of the tag.
                $line =~ s/^$contactEmailKey//i;
                $contactEmailCfgValue = $line;
            }

            if ($computerCfgValue ne "" && $distributorCgfValue ne "" &&
                $contactEmailCfgValue ne "")
            {
                last;
            }
        }


        if ($computerCfgValue ne "" && $distributorCgfValue ne "" &&
            $contactEmailCfgValue ne "")
        {
            if ($computerCfgValue =~ /$platformComputer$/i && 
                $distributorCgfValue =~ /$platformDistributor$/i &&
                $contactEmailCfgValue =~ /$customerContactEmail$/i)
            {
                $customerMatchFound = 1;
                print "CustomerIndex = $tempCnt\n";
                last;
            }
        }
    }
        
    if ($customerMatchFound != 1)
    {
        return 1;
    }

    # Now read the value for the DoBinaryAnalysisForThisCustomer setting from
    # configuration file.
    $doBinaryAnalysisKey = "$doBinaryAnalysisForThisCustomerKey" . "$tempCnt = ";

    for ($configurationEntryIndex = 0; 
        $configurationEntryIndex < $configurationEntriesCnt; $configurationEntryIndex++)
    {
        $line = $configurationEntries [$configurationEntryIndex];
        # Remove all leading and trailing white spaces in the line.
        $line =~ s/^\s*(.*?)\s*$/$1/;

        if ($line =~ /^$doBinaryAnalysisKey/i)
        {
            # We found the DoBinaryAnalysisForThisCustomer string.
            # Remove the tag so that we will have only the value of the tag.
            $line =~ s/^$doBinaryAnalysisKey//i;
            $doBinaryAnalysisForThisCustomer = $line;
            last;            
        }
    }

    # Now read the value for the DoMacroAnalysisForThisCustomer setting from
    # configuration file.
    $doMacroAnalysisKey = "$doMacroAnalysisForThisCustomerKey" . "$tempCnt = ";

    for ($configurationEntryIndex = 0; 
        $configurationEntryIndex < $configurationEntriesCnt; $configurationEntryIndex++)
    {
        $line = $configurationEntries [$configurationEntryIndex];
        # Remove all leading and trailing white spaces in the line.
        $line =~ s/^\s*(.*?)\s*$/$1/;

        if ($line =~ /^$doMacroAnalysisKey/i)
        {
            # We found the DoMacroAnalysisForThisCustomer string.
            # Remove the tag so that we will have only the value of the tag.
            $line =~ s/^$doMacroAnalysisKey//i;
            $doMacroAnalysisForThisCustomer = $line;
            last;            
        }
    }

    return 0;
}


########################################################################
# This subroutine reads the result file created by the Classifier.pl
# and reads the original NextService intended by the classifier program.
#
# Function arguments: 
#
# This function doesn't require any function arguments.
# This function returns the original next service token to the caller.
########################################################################
sub GetOriginalNextService()
{
    my $origNextService = $isqnDefer;
    my $resultFileName = sprintf "$unc\\DFResults\\Result%d.dat", $ProcessID - 1;
    print "ResultFileName = $resultFileName\n";

    unless (open (RESFILEHANDLE, "<$resultFileName"))
    {
        # Unable to open the results file of the previous process.
        return $origNextService;
    }

    # Read all the lines from the result file.
    my @resultEntries = <RESFILEHANDLE>;
    # Remove the new line character at the end.
    chop @resultEntries;
    close RESFILEHANDLE;

    my $resultEntriesCnt = @resultEntries;
    my $resultEntryIndex;

    my $nextServiceKey = "NextService:";

    for ($resultEntryIndex = 0; $resultEntryIndex < $resultEntriesCnt; $resultEntryIndex++)
    {
        my $resline = $resultEntries [$resultEntryIndex];
        # Remove all leading and trailing white spaces in the line.
        $resline =~ s/^\s*(.*?)\s*$/$1/;

        if ($resline =~ /^$nextServiceKey/i)
        {
            # We found the NextService: string.

            my ($key, $sliderService, $nextService);

            ($key, $sliderService, $nextService) = split /\s+/, $resline, 3;
            print "Key = $key, SliderService = $sliderService, NextService = $nextService\n";
            if ($nextService ne "")
            {
                $origNextService = $nextService;
            }
            last;            
        }
    }

    return $origNextService;
}
