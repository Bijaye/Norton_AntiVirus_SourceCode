# Deferrer.pl -- 
use Getopt::Long;
use DataFlow;
use strict;

srand (time() ^ ($$ + ($$ << 15)));

my ($Cookie, $ProcessID, $Parameter, $unc);

GetOptions("Cookie=i" => \$Cookie,
	  "ProcessId=i" => \$ProcessID,
	  "Parameter=s" => \$Parameter,
	  "UNC=s" => \$unc);

print STDERR "Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc\n";

sleep int(rand 20) + 1;

open (FILEHANDLE, ">c:\\MorseTest\\DoDefer.sql");
print FILEHANDLE ("connect to AVISDATA user admin using wa3syj\n\n");
print FILEHANDLE ("update analysisrequest set ANALYSISSTATEID = 7 where LOCALCOOKIE = $Cookie\n");
print FILEHANDLE ("disconnect AVISDATA\n");

close (FILEHANDLE);
open (FILEHANDLE, ">c:\\MorseTest\\DoDefer.bat");
print FILEHANDLE ("db2 -f c:\\MorseTest\\DoDefer.sql\n");
print FILEHANDLE ("exit\n");

close (FILEHANDLE);

system ("c:\\sqllib\\bin\\db2cmd.exe c:\\MorseTest\\DoDefer.bat");

sleep (2);
1;
