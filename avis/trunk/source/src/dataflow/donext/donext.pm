#!perl -w
#------------------------------------------------------
# Author: Senthil
# Last Modified: SEP/18/1998
# Description:
# This module will be used by the Immune System analysis
# programs. Whenever a particular analysis program completes,
# it will call a subroutine (add_doNext) to write the
# information about what to do next for the cookie being 
# processed into a result file. The Data Flow module will
# read the result file to determine what action to take on
# the current sample being processed.
# This module will be called as shown below.
# add_doNext (cookie, processId, uncPath, nextService, parameters)
# DoNext info will be written to a file called
# Resultxx.dat where xx is the processId.
#------------------------------------------------------
package DoNext;
require Exporter;
@ISA = qw (Exporter);
@EXPORT = qw (add_doNext);

sub add_doNext
{
    # Initialize these variables.
    $count = 1;
    $cookieId = "";
    $processId = "";
    $uncPath = "";
    $serviceName = "";
    $parameters = "";
    $eofMarker = "1122334455";
    $substituteCnt = 0;

    # Split the unlimited number of arguments for this subroutine.
    foreach (@_)
    {
        if ($count == 1)
        {
            $cookieId = $_;
        }
        elsif ($count == 2)
        {
            $processId = $_;
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
            $parameters .= ($_ . " ");        
        }

        $count++;
    }    

    #Form the complete filename.
    $fileName = $uncPath . "\\" . "Result$processId.dat";
    #Check if the file already exists.
    unless (-e $fileName)
    {
        #Create this file
        open (FILEHANDLE, ">$fileName");
    }
    else
    {
        #Get the file length.   
        ($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $fileLength,
        $atime, $mtime, $ctime, $blksize, $blocks) = stat $fileName;  
        #File exists. Let us read the file contents.
        open (FILEHANDLE, "<$fileName");    
        read FILEHANDLE, $fileBuffer, $fileLength;
        close (FILEHANDLE);
        #Delete the file.
        unlink $fileName;
        #Replace the EndOfFileMarker with a space character.
        $substituteCnt = $fileBuffer =~ s/$eofMarker/ /i;
        #Create this file again.
        open (FILEHANDLE, ">$fileName");        
    }

    #First write the previously availabe that we have read from this file.
    if ($substituteCnt != 0)
    {
        print FILEHANDLE ($fileBuffer);
    }

    #write the required data to the file.
    print FILEHANDLE ("CookieId: $cookieId\n");
    print FILEHANDLE ("Parameters: $parameters\n");
    print FILEHANDLE ("NextService: $serviceName\n");
    #write the EndOfFileMarker at the very end.
    print FILEHANDLE ($eofMarker);
    print FILEHANDLE ("\n");
    close (FILEHANDLE);
    $count;    
}





