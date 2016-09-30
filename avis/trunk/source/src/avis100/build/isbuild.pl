#!perl
# build.pl -- master build and staging script for BlueICE
# $Header: /Build/build.pl 27    8/10/98 5:26p Swimmer $
#
# Author: Morton Swimmer
#
#$History: build.pl $
#
#*****************  Version 22  *****************
#User: Swimmer      Date: 7/17/98    Time: 8:31p
#Updated in $/Build
#Fix compile problem for Perl extension crc and remove unneeded
#installation directories.
#
#*****************  Version 21  *****************
#User: Swimmer      Date: 7/14/98    Time: 7:44p
#Updated in $/Build
#Add perl5/ext/crc to build
#
#*****************  Version 20  *****************
#User: Swimmer      Date: 7/13/98    Time: 10:12p
#Updated in $/Build
#Change install location of some .pm files
#
#*****************  Version 19  *****************
#User: Swimmer      Date: 7/13/98    Time: 9:52p
#Updated in $/Build
#More flexibility in installing scripts; use build.pm
#
#*****************  Version 17  *****************
#User: Swimmer      Date: 7/09/98    Time: 8:21p
#Updated in $/Build
#extract macros.dsm to correct location and use GetOpt:: instead of
#@ARGV
#
#*****************  Version 16  *****************
#User: Swimmer      Date: 7/09/98    Time: 5:30p
#Updated in $/Build
#change header to reflect new file name
# 
# Revision information as imported from SourceSafe:
my $ssRevision = (qw(
$Revision: 27 $
))[1];
my ($ssDate, $ssTime) =     (qw(
$Date: 8/10/98 5:26p $
))[1 .. 2];
#my $ssLabel = (qw(
#$Label:              $
#))[1];
$ssLabel = "alpha";

use File::Copy;
use File::Path;
use File::Find;
use Cwd;
use Env;
use Getopt::Long;
use Isbuild;

sub logmsg(@);
sub InstallFileHash;
sub ImportPLgFiles;
sub DateStamp;

# define directories that need to be deleted before a complete build
my @buildtree = 
(
 "AutoSig",
 "Autoverv",
 "Build",
 "CodeRunner",
 "Macro",
 "Other",
 "pm",
 "Scripts",
 "Vanalysis",
 "verv",
 "Vides"
);

my @scriptfiles = 
(
 "Scripts/Analyzer/classifier.pl",
 "Scripts/Analyzer/manalysis.pl",
 "Scripts/Analyzer/mrep.pl",
 "Scripts/Analyzer/mrepctrl.pl",
 "Scripts/Analyzer/navname.pl",
 "Scripts/Analyzer/fprot.pl",
 "Scripts/Analyzer/scan.pl",
 "Scripts/Analyzer/test.pl",
 "Scripts/Analyzer/unittest.pl",
 "Vanalysis/vanalysis.pl",
 "Vanalysis/bindefpak/bindefpak.pl",
 "Scripts/Analyzer/vrep.pl",
 "Scripts/Analyzer/vrepctrl.pl"
);

my %modulefiles = 
(
 "pm/Dataflow/Dataflow.pm"                             => $stageperllibsite,
 "pm/Readmefileparser/readme.pm"                       => $stageperllibsite,
 "pm/Typemanager/typemgr.pm"                           => $stageperllibsite,
 "pm/Typemanager/ExtList.pm"                           => $stageperllibsite,
 "pm/Typemanager/EXEList.pm"                           => $stageperllibsite,
 "pm/AVIS/AutoName/autoname.pm"                        => $stageperllibsite."/AVIS",
 "pm/AVIS/Bytes2/Bytes2.pm"                            => $stageperllibsite."/AVIS",
 "pm/AVIS/CheckExtraction/CheckExtraction.pm"          => $stageperllibsite."/AVIS",
 "pm/AVIS/CreateScript/CreateScript.pm"                => $stageperllibsite."/AVIS",
 "pm/AVIS/Crumbs/Crumbs.pm"                            => $stageperllibsite."/AVIS",
 "pm/AVIS/FSTools/FSTools.pm"                          => $stageperllibsite."/AVIS",
 "pm/AVIS/Local/Local.pm"                              => $stageperllibsite."/AVIS",
 "pm/AVIS/Log/Log.pm"                                  => $stageperllibsite."/AVIS",
 "pm/AVIS/Logger/Logger.pm"                            => $stageperllibsite."/AVIS",
 "pm/AVIS/lvirsig/lvirsig.pm"                          => $stageperllibsite."/AVIS",
 "pm/AVIS/Vides/Vides.pm"                              => $stageperllibsite."/AVIS",
 "pm/AVIS/VMTools/Vmtools.pm"                          => $stageperllibsite."/AVIS",
 "pm/AVIS/ParamStr/ParamStr.pm"                        => $stageperllibsite."/AVIS",
 "pm/AVIS/Macros/cosust.pm"                            => $stageperllibsite."/AVIS",
 "vanalysis/perl/lib/site/AVIS/Dirstack.pm"            => $stageperllibsite."/AVIS",
 "vanalysis/perl/lib/site/AVIS/pathenvtools.pm"        => $stageperllibsite."/AVIS",
 "vanalysis/perl/lib/site/AVIS/testbinrepair.pm"       => $stageperllibsite."/AVIS",
 "pm/AVIS/Macros/FileProperties.pm"                    => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Implementations.pm"                   => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/macrobod.pm"                          => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Macrocrc/Macrocrc.pm"                 => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Macrodb.pm"                           => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Mkdefs/Cleans.pm"                     => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Mkdefs/Mkdefs.pm"                     => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Mutils.pm"                            => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/OLE2Doc.pm"                           => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/OLE2Stream.pm"                        => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Qut/Qdbuild.pm"                       => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Qut/Qdint.pm"                         => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Qut/Qdtest.pm"                        => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Qut/Qut.pm"                           => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Qut/Qdbintst.pm"                      => $stageperllibsite."/AVIS/Macro",
 "pm/AVIS/Macros/Types/Delta.pm"                       => $stageperllibsite."/AVIS/Macro/Types",
 "pm/AVIS/Macros/Types/Event.pm"                       => $stageperllibsite."/AVIS/Macro/Types",
 "pm/AVIS/Macros/Types/Form.pm"                        => $stageperllibsite."/AVIS/Macro/Types",
 "pm/AVIS/Macros/Types/Goat.pm"                        => $stageperllibsite."/AVIS/Macro/Types",
 "pm/AVIS/Macros/Types/List.pm"                        => $stageperllibsite."/AVIS/Macro/Types",
 "pm/AVIS/Macros/Types/Target.pm"                      => $stageperllibsite."/AVIS/Macro/Types",
 "pm/AVIS/Misc/Misc.pm"                                => $stageperllibsite."/AVIS/Macro",
 "pm/scopy/scopy.pm"                                   => $stageperllibsite,
 "pm/mcv/mcv.pm"                                       => $stageperllibsite."/AVIS/Macro"
);

# define the config files
my %inifiles = 
(
 "Vides/Bochsw/bochs.ini"                              => $stagebin,
 "CodeRunner/src/options.opt"                          => $stagebin,
 "CodeRunner/src/options_s.opt"                        => $stagebin,
 "CodeRunner/src/options_l.opt"                        => $stagebin,
 "pm/Dataflow/Dataflow.pm"                             => $stagebin,
 "pm/NAVAPI/Navapi32.dll"                              => $stagebin,
 "Other/NAVTools/a97ext.exe"                           => $stagebin,
 "Other/NAVTools/macroext.exe"                         => $stagebin,
 "Other/NAVTools/macroprt.exe"                         => $stagebin,
 "Other/NAVTools/ole.exe"                              => $stagebin,
 "Other/NAVTools/oledocnf.exe"                         => $stagebin,
 "Other/NAVTools/tools.txt"                            => $stagebin,
 "Other/NAVTools/vba5ext.exe"                          => $stagebin,
 "Other/NAVTools/wd7crc.exe"                           => $stagebin,
 "Other/NAVTools/wd7ren.exe"                           => $stagebin,
 "Other/NAVTools/wdfndkey.exe"                         => $stagebin,
 "Other/NAVTools/xl4ext.exe"                           => $stagebin,
 "Other/NAVTools/xl5ext.exe"                           => $stagebin,
 "Other/NAVTools/xl5prt.exe"                           => $stagebin,
 "Other/NAVTools/xlfndkey.exe"                         => $stagebin,
 "Other/NAVTools/xlrec.exe"                            => $stagebin,
 "Other/NAVTools/xlsheet.exe"                          => $stagebin
);

# define the commands files
my %cmdsfiles = 
(
 "macro/Files/Commands/doc2doccmds.lst" => $stagecmds,
 "macro/Files/Commands/docinfcmds.lst"  => $stagecmds,
 "macro/Files/Commands/excelcmds.lst"   => $stagecmds,
 "macro/Files/Commands/globinfcmds.lst" => $stagecmds
);

# define the asax rule files
my %asafiles = 
(
 "Vides/bochsw/vides2g/bootmultipart.asa"           => $stagedata,
 "Vides/bochsw/vides2g/companion.asa"               => $stagedata,
 "Vides/bochsw/vides2g/execonprompt.asa"            => $stagedata,
 "Vides/bochsw/vides2g/extraction.asa"              => $stagedata,
 "Vides/bochsw/vides2g/fcb.asa"                     => $stagedata,
 "Vides/bochsw/vides2g/findtrigger.asa"             => $stagedata,
 "Vides/bochsw/vides2g/handle.asa"                  => $stagedata,
 "Vides/bochsw/vides2g/heuristic.asa"               => $stagedata,
 "Vides/bochsw/vides2g/inputsim.asa"                => $stagedata,
 "Vides/bochsw/vides2g/main.asa"                    => $stagedata,
 "Vides/bochsw/vides2g/resident.asa"                => $stagedata,
 "Vides/bochsw/vides2g/utils.asa"                   => $stagedata,
 "Vides/bochsw/vides2g/vides2.adf"                  => $stagedata
);

# define the image files
##### my %imagefiles = 
##### (

##### );

# define the goat files 
##### my %goatfiles = 
##### ( 

##### );

# define which projects to fetch from source code control
my @sccProjects = 
(
 "\$/Avis100/AutoSig",
 "\$/Avis100/Autoverv",
 "\$/Avis100/Build",  
 "\$/Avis100/CodeRunner",
 "\$/Avis100/Macro",
 "\$/Avis100/Other",
 "\$/Avis100/pm",
 "\$/Avis100/Scripts",
 "\$/Avis100/verv",
 "\$/Avis100/Vides",
);

my $rootpath         = cwd();
my $DateStamp        = DateStamp;
my $logfile          = "build-".$DateStamp.".log";
my $summaryfile      = "build-".$DateStamp.".sum";
my $sccEXE           = "ss";
my $sccGetCommand    = "Get";
my $sccOptions       = "-r -GF -I-Y -O&$logfile";
#my $vb = "\"c:\\program files\\microsoft visual basic\\vb32\"";
my $vb = "vb32";

# import all environment variables to global variables (e.g. PATH is
# now in $PATH)
Env::import();

die "MSDEVDIR is not set. Did you forget to set MS VC environment variables?" if ($MSDEVDIR eq "");

# get parameters from command line
my ($optInstall, $optCompile, $optExtract, $optAll) = (0,0,0,0);
my $result;
$result = &GetOptions("install!" => \$optInstall,
	    "compile!" => \$optCompile,
	    "extract!" => \$optExtract,
	    "all!"     => \$optAll);
if (!$result) {
  print "\nYou obviously don't know what the options are. Take a look:\n";
  print "\t--compile\tcompile the code\n";
  print "\t--extract\textract everything from source code control\n";
  print "\t--install\tinstall compiled code on stageing directory\n";
  print "\nNow try again!\n\n";
  exit;
}
if ($optAll || (!$optInstall && !$optCompile && !$optExtract)) {
  $optInstall = $optCompile = $optExtract = 1;
}

open LOG, ">$logfile";

logmsg("Isbuild.pl -- Build script for the Immune System");
logmsg("Revision number: $ssLabel $ssRevision, date: $ssDate, time: $ssTime");
logmsg("", "Perl version number: $]","");
logmsg("Build started on: ".scalar localtime);
logmsg("Selected options: compile: $optCompile, install: $optInstall, extract: $optExtract");

# delete the contents of the staging directories

foreach ($stagebin, $stageperl) {
  unlink <$_/*>;
  mkdir $_, 755 or logmsg "Cannot create $_ directory";
}

# extract the source files from source code control
# the -r option specifies recursive get
# the -GF option forces retrieval in the working folder as defined
#if (<@buildtarget> eq "extract" || <@buildtarget> eq "all") {
if ($optExtract) {
  #delete the build tree first
  foreach (@buildtree) {
    rmtree($_, 1, 0);
  }
  #get the sources

  logmsg "==================== Starting extract ==================";
  close LOG;
  foreach (@sccProjects) {
    system "$sccEXE $sccGetCommand $_ $sccOptions";
    # pause;
  }
  open LOG, ">>$logfile";
  logmsg "==================== Ending extract ====================";
}

# copy the DevStudio macro file to the DevStudio macros directory

# build the immune system
if ($optCompile) {
  # build C programs
  unlink("D:\\avis100build.done");
  system "msdev -ex Avis100";
  while (!(-e "D:\\avis100build.done"))
	{
	sleep (1);
	};
  ImportPLgFiles;

  &BuildMakefile("autosig/sigcompr", "all");
}

#install scripts and other stuff on the staging directories
if ($optInstall) {
  foreach (@scriptfiles) {
    print "Copying $_ to $stagebin ...\n";
    copy ($_, $stagebin) 
      or logmsg "fatal error copying script $_ to $stagebin: $!";
  }
  InstallFileHash(\%modulefiles);
  InstallFileHash(\%inifiles);
  InstallFileHash(\%cmdsfiles);
  InstallFileHash(\%asafiles);
  InstallFileHash(\%imagefiles);
}

logmsg "Isbuild.pl ending normally at: ".scalar localtime;
print "\nHang on while I create the summary file and copy to the LAN\n";
close LOG;
open LOG, "<$logfile";
open SUM, ">$summaryfile";
while (<LOG>) {
  print SUM if (/fatal error/);
}
close SUM;
close LOG;
copy($logfile, $logfiledirectory."/".$logfile) 
  or warn "copying $logfile failed";
unlink $logfile if ! $!;

copy($summaryfile, $logfiledirectory."/".$summaryfile) 
  or warn "copying $summaryfile failed";
unlink $summaryfile if ! $!;


exit;


sub BuildVB {
  my($targetname,$targetpath,$targetproject) = @_;
  my($ok);

  logmsg "Building VB project $targetproject in directory $targetpath\n";

  $ok = chdir($targetpath);
  if ($ok) {
    system "$vb /make $targetproject";
    if (-e $targetname) {
      copy($targetname, "D:/is/stage/bin") or logmsg "fatal error : copying $targetname";
    } else {
      logmsg "fatal error : File '$targetname' not built";
    }
    chdir $rootpath;
  }
}

sub BuildPerlExt {
  my ($builddir, $perldir) = @_;
  my $currentpath, $rc;
  
  logmsg "Building $builddir\n";

  $perldir =~ tr/\//\\/; # make the path conform to DOS standards

  if (-d $builddir) {
    $currentpath = cwd();
    chdir $builddir;

    $rc = 0xffff && system "perl makefile.pl LIB=$perldir\\lib";
    printf "system(%s) returned %#04x: ", "nmake $target", $rc;
    if ($rc == 0) {
      print "ran with normal exit\n";
    } elsif ($rc == 0xff00) {
      print "command failed: $!\n";
    } elsif ($rc > 80) {
      $rc >>= 8;
      print "ran with non-zero exit status $rc\n";
    } else {
      print "ran with ";
      if ($rc &  0x80) {
	$rc &= ~0x80;
	print "core dump from ";
      }
      print "signal $rc\n";
    }
    $ok = ($rc != 0);

    chdir $currentpath;
    &BuildMakefile($builddir, "");
    &BuildMakefile($builddir, "install");
  }
}


sub BuildMakefile {
  my ($builddir, $target) = @_;
  my $currentpath, $rc;
  
  logmsg "Building $builddir\n";

  if (-d $builddir) {
    $currentpath = cwd();
    chdir $builddir;
    $rc = 0xffff && system("nmake $target");
    printf "system(%s) returned %#04x: ", "nmake $target", $rc;
    if ($rc == 0) {
      print "ran with normal exit\n";
    } elsif ($rc == 0xff00) {
      print "command failed: $!\n";
    } elsif ($rc > 80) {
      $rc >>= 8;
      print "ran with non-zero exit status $rc\n";
    } else {
      print "ran with ";
      if ($rc &  0x80) {
	$rc &= ~0x80;
	print "core dump from ";
      }
      print "signal $rc\n";
    }
    $ok = ($rc != 0);

    chdir $currentpath;
  }
}

sub logmsg (@){
  foreach (@_) {
    print STDERR "$_\n";
    print LOG "$_\n";
  }
}

sub pause {
  print "pausing..."; getc; print "\n";
}

sub CopyIfPLg {
  if (/^.*\.plg$/i) {
    print LOG ">>>>>>>>>>>> Imported from: $File::Find::name  <<<<<<<<<<<<<<\n";
    open TMP, "$_";
    while (<TMP>) {
      print LOG;
    }
    close TMP;
  }
}

sub ImportPLgFiles {
  find(\&CopyIfPLg, ".");
}

sub DateStamp {
  my @now = localtime;
  return sprintf "%02d%02d%02d-%02d%02d%02d",  $now[5], $now[4]+1, $now[3], $now[2], $now[1], $now[0];
}

sub InstallFileHash  {
  local *hash = shift;
  foreach (keys %hash) {
    my $targetdir = $hash{$_};
    logmsg "Copying $_ to $targetdir ...\n";
    unless (-d $targetdir) {
      mkdir $targetdir, 755 or logmsg "Cannot create $targetdir directory";;
    }
    copy ($_, $targetdir) 
      or logmsg "fatal error : copying $_ to $targetdir: $!";
  }
}



















