# ArchiveSample.pl --
use Getopt::Long;
use strict;
use AVIS::Local;

srand (time() ^ ($$ + ($$ << 15)));

my ($Cookie, $ProcessID, $Parameter, $unc);

GetOptions("Cookie=i" => \$Cookie,
	  "ProcessId=i" => \$ProcessID,
	  "Parameter=s" => \$Parameter,
	  "UNC=s" => \$unc);

print "Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc\n";
my $CookieStr;
$CookieStr = sprintf "%08d", $Cookie;
archive($isdnGraveyard, "$CookieStr.zip", "$unc");


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

