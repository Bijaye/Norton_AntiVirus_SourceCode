#!perl -w

    # Initialize these variables.
    $totalCookiecount = 200;
    $uncPath = "";
    $count = 1;

    # Split the unlimited number of arguments for this subroutine.
    foreach (@ARGV)
    {
        if ($count == 1)
        {
            $uncPath = $_;
        }
        elsif ($count == 2)
        {
            $totalCookiecount  = $_;
        }
  
        $count++;
    }

    @args = ("del", "$uncPath", "/s", "/q"); 
    system (@args);
    print ("For this test, we are creating $totalCookiecount cookies in $uncPath.\n");
    
    for ($cnt = 1; $cnt <= $totalCookiecount;)
    {
	$randomCookieCnt = int (rand 50) + 1;
        print ("Creating $randomCookieCnt cookies from $cnt.\n");

        for ($fileCnt = 0; $fileCnt < $randomCookieCnt; $fileCnt++, $cnt++)
        {
            $cookiePath = sprintf "%08d", $cnt;
#            $subDirectory = $uncPath . "\\$cnt";
            $subDirectory = $uncPath . "\\" . $cookiePath;
            mkdir ($subDirectory, 0777);

            #Form the complete filename.
            $fileName = $subDirectory . "\\dfstart.dat";
            #Check if the file already exists.
            unless (-e $fileName)
            {
               #Create this file
               open (FILEHANDLE, ">$fileName");
	       close (FILEHANDLE);    
            }         
        }

        $delayTime = int (rand 120) + 10;
        print ("Sleeping for $delayTime seconds.\n");
	sleep ($delayTime);
    }

1;






