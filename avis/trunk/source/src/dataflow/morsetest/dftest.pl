#!perl -w

    use File::Copy;

    # Initialize these variables.
    ($totalCookiecount, $startCount, $sampleDirectory) = @ARGV;

    if (!(defined $totalCookiecount) ||
        !defined ($sampleDirectory) ||
        !defined ($startCount))
    {
        print ("Usage: perl dftest.pl <SampleCnt> <StartCnt> <SampeDir>\n");
        exit;
    }

    print ("For this test, we are creating $totalCookiecount cookies.\n");
    if ($startCount == 1)
    {
        open (FILEHANDLE, ">c:\\MorseTest\\DeleteCookie.sql");
        print FILEHANDLE ("connect to AVISDATA user db2admin using hicl\n\n");
        print FILEHANDLE ("delete from analysisrequest\n");
        print FILEHANDLE ("delete from submittors\n\n");
        print FILEHANDLE ("disconnect AVISDATA\n");

        close (FILEHANDLE);
        open (FILEHANDLE, ">c:\\MorseTest\\DeleteCookie.bat");
        print FILEHANDLE ("db2 -f c:\\MorseTest\\DeleteCookie.sql\n");
        print FILEHANDLE ("copy c:\\MorseTest\\DeleteCookie.sql c:\\MorseTest\\DeleteCookie.done\n");
        print FILEHANDLE ("exit\n");

        close (FILEHANDLE);

        unlink ("c:\\MorseTest\\DeleteCookie.done");
        system ("c:\\sqllib\\bin\\db2cmd.exe c:\\MorseTest\\DeleteCookie.bat");
        while (1)
        {
            last if (-e "c:\\MorseTest\\DeleteCookie.done");
            sleep (1);
        }
	unlink ("c:\\MorseTest\\DeleteCookie.done");
	unlink ("c:\\MorseTest\\DeleteCookie.sql");
    	unlink ("c:\\MorseTest\\DeleteCookie.bat");
    }
    
    for ($cnt = 1; $cnt <= $totalCookiecount;)
    {
	$randomCookieCnt = int (rand ($totalCookiecount - $cnt + 1)) + 1;
        if ($randomCookieCnt > 100)
        {
            $randomCookieCnt = 100;
        }
        $startCookieCnt = $startCount + $cnt - 1;
        print ("Creating $randomCookieCnt cookies from $startCookieCnt.\n");

        for ($fileCnt = 0; $fileCnt < $randomCookieCnt; $fileCnt++)
        {
            $currentCookieCnt = $startCookieCnt + $fileCnt;
            print ("Creating $currentCookieCnt of $totalCookiecount Cookies\r");
            #Create a directory for this cookie.
            $subdirectory = sprintf ("%08d", $currentCookieCnt);            
            $cookieDirectory = "$sampleDirectory"."\\$subdirectory";
            if (-d "$cookieDirectory")
            {
	        system("rmdir /s /q $cookieDirectory");
            }
            mkdir ("$cookieDirectory", 0777);
            $cookieDirectory = "$sampleDirectory"."\\$subdirectory"."\\sample";
            mkdir ("$cookieDirectory", 0777);
            copy ("c:\\winnt\\win.ini", $cookieDirectory . "\\win.ini");
            $cookieDirectory = "$sampleDirectory"."\\$subdirectory"."\\DFResults";
            mkdir ("$cookieDirectory", 0777);
        }

        print ("Creating Database records.                          \n");
        open (FILEHANDLE, ">c:\\MorseTest\\CreateCookie.sql");
        print FILEHANDLE ("connect to AVISDATA user db2admin using hicl\n\n");

        if ($cnt == 1 && $startCount == 1)
        {
	    print FILEHANDLE ("insert into submittors (SUBMITTORID) values (\'1\')\n\n");
        }

        for ($fileCnt = 0; $fileCnt < $randomCookieCnt; $fileCnt++)
        {
            $priority = int (rand 100);
            $currentCookieCnt = $startCookieCnt + $fileCnt;
	    print FILEHANDLE ("insert into analysisrequest (LOCALCOOKIE, PRIORITY, ANALYSISSTATEID, CHECKSUM, SUBMITTORID) VALUES ($currentCookieCnt, $priority, (select ANALYSISSTATEID FROM ANALYSISSTATEINFO WHERE NAME = 'imported'), \'1\', \'1\')\n\n");
	}
        print FILEHANDLE ("disconnect AVISDATA\n");

        close (FILEHANDLE);

        open (FILEHANDLE, ">c:\\MorseTest\\CreateCookie.bat");
        print FILEHANDLE ("db2 -f c:\\MorseTest\\CreateCookie.sql\n");
        print FILEHANDLE ("copy c:\\MorseTest\\CreateCookie.sql c:\\MorseTest\\CreateCookie.done\n");
        print FILEHANDLE ("exit\n");

        close (FILEHANDLE);

        unlink ("c:\\MorseTest\\CreateCookie.done");
        system ("c:\\sqllib\\bin\\db2cmd.exe c:\\MorseTest\\CreateCookie.bat");
        while (1)
	{
	    last if (-e "c:\\MorseTest\\CreateCookie.done");
	    sleep (1);
	}
	unlink ("c:\\MorseTest\\CreateCookie.done");
	unlink ("c:\\MorseTest\\CreateCookie.sql");
	unlink ("c:\\MorseTest\\CreateCookie.bat");


	 $cnt += $randomCookieCnt;
#        $delayTime = int (rand 120) + 10;
#        print ("\nSleeping for $delayTime seconds.\n");
#        sleep ($delayTime);
    }

1;
