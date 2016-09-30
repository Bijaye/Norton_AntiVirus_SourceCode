#!perl -w
#------------------------------------------------------
# Author: Senthil
# Last Modified: SEP/21/1998
# Description:
# This module will be used by the Immune System analysis
# programs. Whenever a particular analysis program completes,
# it will call a subroutine (add_stop_condition) to write the
# information about the stopping conditions for the next
# job assigned for the cookie being processed.
# This information is written to a result file.
# The Data Flow module will read the result file to determine
# the stopping condition for the next job.
# This module will be called as shown below.
# add_stop_condition (cookie, processId, uncPath, stopCondition)
# Stop conditions will be written to a file called
# Stopxx.dat where xx is the processId.
#------------------------------------------------------
package AddStop;
require Exporter;
@ISA = qw (Exporter);
@EXPORT = qw (add_stop_condition);

sub add_stop_condition
{
    # Initialize these variables.
    $count = 1;
    $cookieId = "";
    $processId = "";
    $uncPath = "";
    $stopCondition = "";
    $eofMarker = "1122334455";

    # Split the individual arguments passed to this subroutine.
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
            $stopCondition = $_;      	
        }

        $count++;
    }    

    #Form the complete filename.
    $fileName = $uncPath . "\\" . "Stop$processId.dat";
    #Create this file
    open (FILEHANDLE, ">$fileName");

    #write the required data to the file.
    print FILEHANDLE ("CookieId: $cookieId\n");
    print FILEHANDLE ("StopCondition: $stopCondition\n");
    #write the application specific EndOfFileMarker at the very end.
    print FILEHANDLE ($eofMarker);
    print FILEHANDLE ("\n");

    close (FILEHANDLE);
    $count;    
}





