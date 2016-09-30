#!perl -w
#------------------------------------------------------
# Author: Senthil
# Last Modified: SEP/18/1998
# Description:
# This module will be used by the Immune System analysis
# programs. Whenever a particular analysis program completes,
# it will call two subroutines (DoNext and SpecifyStopping).
# 'DoNext' function writes the information about what to do next
# for the cookie being processed into a result file (resultXX.dat).
# 'SpecifyStopping' function writes the information about
# stopping conditions for the next job assigned for the cookie
# being processed into another data file (stopXX.dat). 
# The Data Flow module will read the result and stop files to
# determine what action to take on the current sample being processed.
#
# The function formats are shown below.
# DoNext (cookie, jobSequenceNumber, uncPath, nextService, parameters)
# DoNext info will be written to a file called
# Resultxx.dat where xx is the jobSequenceNumber.
#
# SpecifyStopping (cookie, jobSequenceNumber, uncPath, nextService, stopCondition)
# Stop conditions will be written to a file called
# Stopxx.dat where xx is the jobSequenceNumber.
#------------------------------------------------------
package DataFlow;
require Exporter;
@ISA = qw (Exporter);
@EXPORT = qw (DoNext SpecifyStopping WriteSignatureForDataFlow);

use AVIS::FSTools;

sub DoNext
{
    # Initialize these variables.
    $count = 1;
    $cookieId = "";
    $jobSequenceNumber = "";
    $uncPath = "";
    $serviceName = "";
    $parameters = "";

    # Split the unlimited number of arguments for this subroutine.
    foreach (@_)
    {
        if ($count == 1)
        {
            $cookieId = $_;
        }
        elsif ($count == 2)
        {
            $jobSequenceNumber = $_;
        }
        elsif ($count == 3)
        {
            $uncPath = $_;
        }
        elsif ($count == 4)
        {
            # For Jedi release, we made changes to send samples
            # through a slider switch after the classifier. The slider switch
            # will decide if the sample should be allowed for a particular
            # virus analysis based on the configuration made in the executive machine.
            # For more details, please look in the d:\bin\JediSlider.cfg file in Rainier machine.
            $moduleName = $0;
            $tempStr = "classifier.pl";
            if ($moduleName =~ /$tempStr$/i) 
            {
                $serviceName = "JediSlider $_";
            }
            else
            {
                $serviceName = $_;      	
            }
        }
        elsif ($count >= 5)
        {
            #We need to collect all the tokens at the end.
            $parameters .= ($_ . " ");        
        }

        $count++;
    }    

    #The Data Flow program will look for the results file in a 
    #subdirectory called 'dfresults' within the cookie directory
    #specified by the UNC pathName. Hence let us create the 
    #'dfresults' subdirectory.
    $subDirectory = AVIS::FSTools::Canonize("dfresults", $uncPath);
    mkdir ($subDirectory, 0777);
    #Form the complete filename.
    $fileName = AVIS::FSTools::Canonize("Result$jobSequenceNumber.dat", $subDirectory);
    #Check if the file already exists.
    unless (-e $fileName)
    {
        #Create this file
        open (FILEHANDLE, ">$fileName");
    }
    else
    {
        #File exists. Open it for append mode.
        open (FILEHANDLE, ">>$fileName");
    }

    #write the required data to the file.
    print FILEHANDLE ("CookieId: $cookieId\n");
    print FILEHANDLE ("Parameters: $parameters\n");
    print FILEHANDLE ("NextService: $serviceName\n");

    close (FILEHANDLE);
    $count;    
}

#------------------------------------------------------
# SpecifyStopping (cookie, jobSequenceNumber, uncPath, nextService, stopCondition)
# Stop conditions will be written to a file called
# Stopxx.dat where xx is the jobSequenceNumber.
#------------------------------------------------------
sub SpecifyStopping
{
    # Initialize these variables.
    $count = 1;
    $cookieId = "";
    $jobSequenceNumber = "";
    $uncPath = "";
    $serviceName = "";
    $stopCondition = "";

    # Split the individual arguments passed to this subroutine.
    foreach (@_)
    {
        if ($count == 1)
        {
            $cookieId = $_;
        }
        elsif ($count == 2)
        {
            $jobSequenceNumber = $_;
        }
        elsif ($count == 3)
        {
            $uncPath = $_;
        }
        elsif ($count == 4)
        {
            $serviceName = $_;      	
        }
        elsif ($count >= 5)
        {
            #We need to collect all the tokens at the end.
            $stopCondition .= ($_ . " ");        
        }

        $count++;
    }    

    #The Data Flow program will look for the stopping condition file in a 
    #subdirectory called 'dfresults' within the cookie directory
    #specified by the UNC pathName. Hence let us create the 
    #'dfresults' subdirectory.
    $subDirectory = AVIS::FSTools::Canonize("dfresults", $uncPath);
    mkdir ($subDirectory, 0777);
    #Form the complete filename.
    $fileName = AVIS::FSTools::Canonize("Stop$jobSequenceNumber.dat", $subDirectory);
    #Create this file
    open (FILEHANDLE, ">$fileName");

    #write the required data to the file.
    print FILEHANDLE ("CookieId: $cookieId\n");
    print FILEHANDLE ("StopCondition: $stopCondition\n");

    close (FILEHANDLE);
    $count;    
}

#------------------------------------------------------
# WriteSignatureForDataFlow (jobSequenceNumber, uncPath)
# This function writes the signature string at the end of
# ResultXX.DAT and StopXX.DAT files.
# Stopxx.dat where xx is the jobSequenceNumber.
#------------------------------------------------------
sub WriteSignatureForDataFlow
{
    # Initialize these variables.
    $count = 1;
    $jobSequenceNumber = "";
    $uncPath = "";

    # Split the individual arguments passed to this subroutine.
    foreach (@_)
    {
        if ($count == 1)
        {
            $jobSequenceNumber = $_;
        }
        elsif ($count == 2)
        {
            $uncPath = $_;
        }

        $count++;
    }    

    #The Data Flow program will look for the ResultXX.Dat and StopXX.Dat files in a 
    #subdirectory called 'dfresults' within the cookie directory
    #specified by the UNC pathName. We need to write the signature string at the end
    # of these files so that the DataFlow program can check for the data integrity.
    $subDirectory = AVIS::FSTools::Canonize("dfresults", $uncPath);
    #Form the complete filename.
    $fileName = AVIS::FSTools::Canonize("Stop$jobSequenceNumber.dat", $subDirectory);
    #Open this file
    open (FILEHANDLE, ">>$fileName");

    #write the required signature string to the file.
    print FILEHANDLE ("1122334455\n");
    close (FILEHANDLE);


    #Form the complete filename.
    $fileName = AVIS::FSTools::Canonize("Result$jobSequenceNumber.dat", $subDirectory);
    #Open this file
    open (FILEHANDLE, ">>$fileName");

    #write the required signature string to the file.
    print FILEHANDLE ("1122334455\n");
    close (FILEHANDLE);


    $count;    
}




