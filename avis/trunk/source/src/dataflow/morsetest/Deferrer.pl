# Deferrer.pl -- 
use Getopt::Long;
use DataFlow;
use strict;
use AVIS::Local;

srand (time() ^ ($$ + ($$ << 15)));

my ($Cookie, $ProcessID, $Parameter, $unc);

GetOptions("Cookie=i" => \$Cookie,
	  "ProcessId=i" => \$ProcessID,
	  "Parameter=s" => \$Parameter,
	  "UNC=s" => \$unc);

print STDERR "Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc\n";

open (FILEHANDLE, ">d:\\bin\\DoDefer.sql");
print FILEHANDLE ("connect to AVISDATA user db2admin using hicl\n\n");
print FILEHANDLE ("update analysisrequest set ANALYSISSTATEID = (SELECT ANALYSISSTATEID FROM ANALYSISSTATEINFO WHERE NAME = 'deferred') where LOCALCOOKIE = $Cookie\n");
print FILEHANDLE ("disconnect AVISDATA\n");

close (FILEHANDLE);
open (FILEHANDLE, ">d:\\bin\\DoDefer.bat");
print FILEHANDLE ("db2 -f d:\\bin\\DoDefer.sql\n");
print FILEHANDLE ("exit\n");

close (FILEHANDLE);

system ("c:\\sqllib\\bin\\db2cmd.exe d:\\bin\\DoDefer.bat");

sleep (2);

my $CookieStr;
$CookieStr = sprintf "%08d", $Cookie;
archive($isdnGraveyard, "$CookieStr.deferred.zip", "$unc");


sub archive ($$$) {
  my ($archivedir, $archivename, $directory) = @_;
  my $program = "d:\\tools\\zip.exe";
  my ($cc, $cmdline);

  $directory =~ tr/\//\\/;

  my $flags = "-r -m";
  $cmdline = "$program $flags $archivedir\\$archivename $directory\\*";
  print "Executing: '[$archivedir]> $cmdline'\n";
  $cc = system($cmdline);

  system ("rmdir $unc /s /q");
}

1;
