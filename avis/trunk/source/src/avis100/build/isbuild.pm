#!perl
# build.pm
# $Header: /Build/build.pm 6     8/10/98 5:25p Swimmer $
#
# holds global defines for build and installation scripts
#
#$History: build.pm $
#
#*****************  Version 6  *****************
#User: Swimmer      Date: 8/10/98    Time: 5:25p
#Updated in $/Build
#make installation directories dependent on environment variables
#
#*****************  Version 5  *****************
#User: Swimmer      Date: 7/17/98    Time: 8:31p
#Updated in $/Build
#Fix compile problem for Perl extension crc and remove unneeded
#installation directories.
#
#*****************  Version 4  *****************
#User: Swimmer      Date: 7/13/98    Time: 9:53p
#Updated in $/Build
#Added perl paths
#
#*****************  Version 3  *****************
#User: Swimmer      Date: 7/09/98    Time: 9:00p
#Updated in $/Build
#
#*****************  Version 2  *****************
#User: Swimmer      Date: 7/08/98    Time: 6:43p
#Updated in $/Build
#Add installation support
#
#*****************  Version 1  *****************
#User: Swimmer      Date: 7/08/98    Time: 5:22p
#Created in $/Build
#holds global defines for build and installation scripts
#
#

package build;
require Exporter;
@ISA = qw(Exporter);
@EXPORT =  qw($stagebin  $stageperl $stageperllib $logfiledirectory $stagecmds $stagedata);
@EXPORT_OK = qw($stagebin  $stageperl $stageperllib $logfiledirectory $stagecmds $stagedata);
use Env;

if ($PERL5DIR) {
  $main::stageperl        = $PERL5DIR;
  $main::stageperllib     = $PERL5DIR."/lib";
  $main::stageperllibsite = $PERL5DIR."/lib/site";
  $main::stageperllibsiteavis = $PERL5DIR."/lib/site/avis";
} else {

  $main::stageperl        = "D:/is/progs/perl";
  $main::stageperllib     = $stageperl."/lib";
  $main::stageperllibsite = $stageperl."/lib/site";
  $main::stageperllibsiteavis = $stageperl."/lib/site/avis";
}
if ($ISDIR) {
  $main::stagebin         = $ISDIR."/bin";
  $main::stagecmds        = $ISDIR."/macro/tree/files/config";
  $main::stagedata        = $ISDIR."/data/vresources/rules";
} else {

  $main::stagebin         = "D:/is/progs/stage/bin";
  $main::stagecmds        = "D:/is/progs/stage/macro/tree/files/config";
  $main::stagedata        = "D:/is/progs/stage/data";
}

  $main::logfiledirectory = "D:/is/wrkfiles/BuildLogs";

1;





