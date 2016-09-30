#!perl -w

    use File::Copy;

    # Initialize these variables.
    ($cookieID, $priority) = @ARGV;

    if (!(defined $cookieID) || !(defined $priority))
    {
        print ("Usage: perl ChangePriority.pl <CookieID> <NewPriority> ...\n");
        exit;
    }

    open (FILEHANDLE, ">c:\\MorseTest\\ChangePriority.sql");
    print FILEHANDLE ("connect to AVISDATA user db2admin using hicl\n\n");
    $cookieId = 0;
    $priority = 0;

    foreach (@ARGV)
    {      
        if ($cookieId == 0)
	{
            $cookieId = $_;
        }
        else
        {
            print FILEHANDLE ("update analysisrequest set PRIORITY = $_ where LOCALCOOKIE = $cookieId\n");
            $cookieId = 0;
        }
    }

    print FILEHANDLE ("disconnect AVISDATA\n");

    close (FILEHANDLE);
    open (FILEHANDLE, ">c:\\MorseTest\\ChangePriority.bat");
    print FILEHANDLE ("db2 -f c:\\MorseTest\\ChangePriority.sql\n");
    print FILEHANDLE ("exit\n");

    close (FILEHANDLE);

    system ("c:\\sqllib\\bin\\db2cmd.exe c:\\MorseTest\\ChangePriority.bat");
    
1;
