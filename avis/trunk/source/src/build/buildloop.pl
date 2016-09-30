# Perl Script to Loop to Determine When to Perform IBM AntiVirus Product Build
# Modified for Immune System Robert and Glenn
# 09/29/98
#
# IBM Research
# Robert B. King
# Created on April 29, 1997
# Last Modified on October 31, 1998
#
#
# This file is used to perform the builds.... after processing the arguments:
# Loop Forever:
#    Wait for the next build to be retrieved from CMVC
#    Copy the ZIP file to the build machine
#    UnZIP the source files into the build tree
#    Call Perl Script to Perform Build
#    Repeat
#

use lib "perl";

use LogInfo ();
use RecentList ();
use PruneOld ();
use Getopt::Long;
use Net::Domain ();
use Net::SMTP ();

$PgmLastUpdated  = "October 31, 1998";   # Last Time this Program was Updated

$shutgui_pgm     = 'c:\ntreskit\shutgui.exe';  # NT Shutdown Program
$polltime        = 60;                         # Seconds
$unzippath       = "c:\\infozip\\unzip";       # UNZIP Program
$builddir        = "src\\build";
$upstairsbuilddir        = "src\\avis100\\build";
$localdrive      = "d:";

print "\nIBM Immune System Build Machine Loop Control Program\n";
print "*** IBM Confidential ****\n";
print "Developed by IBM Research, Last Updated ${PgmLastUpdated}\n\n";

chdir($localdrive) or die "Can't chdir $localdrive, $!, stopped";

local($logdir, $bldmachine, $controldir, $relname,
      $ziprelname, $zipsrcdir, $scriptdebug) = &validate_arguments;

# Adjust for debug mode
$polltime = ($scriptdebug ? 10 : $polltime);

# Open the Log
LogInfo::open_log("${logdir}\\${bldmachine}.log", 1);

# Record Initial StartUp Information
LogInfo::to_log("\n****************************************");
LogInfo::to_log("****************************************");
LogInfo::to_log("IBM Immune System Build Machine Loop Control Program");
LogInfo::to_log("*** IBM Confidential ****");
LogInfo::to_log("Developed by IBM Research, Last Updated ${PgmLastUpdated}\n");
LogInfo::to_log("Build Log Directory ======= $logdir");
LogInfo::to_log("Build Machine Name ======== $bldmachine");
LogInfo::to_log("Release Name ============== $relname");
LogInfo::to_log("ZIP Source Directory ====== $zipsrcdir");
LogInfo::to_log("ZIP Release Name ========== $ziprelname");
LogInfo::to_log("Build Control Directory === $controldir");
LogInfo::to_log("Script Debug ============== ". ($scriptdebug ? "On" : "Off"));
LogInfo::to_log("****************************************");

# Get Latest Build Level on Local Machine
$curbld = &RecentList::get_list("\\", "^avis${relname}\.([0-9][0-9][0-9])\$", undef);
LogInfo::to_log("Last Local Build was $curbld");
LogInfo::close_log;

# Loop Forever Waiting for Code to Build
for (;;)
{
    local($levname);
    local(@tobuild);

    # Get a List of Levels to Build
    @tobuild = &RecentList::get_list($zipsrcdir, "^avis${ziprelname}([0-9][0-9][0-9]).txt\$", $curbld);
    if (($#tobuild == 0) && (not defined $tobuild[0]))
    {
        LogInfo::open_log("${logdir}\\${bldmachine}.log", 1);
        LogInfo::to_log("*** Error Reading ZIP Source Directory $zipsrcdir ***");
        LogInfo::close_log;
        print STDOUT "*** Error Reading ZIP Source Directory $zipsrcdir ***\n";
        @tobuild=();
    }

    # Kick off the build for each Level
    foreach $levname (@tobuild)
    {
        local($args);
        local($blddir) = "\\avis${relname}.$levname";
	local($zipname) = "avis${ziprelname}${levname}.zip";

        LogInfo::open_log("${logdir}\\${bldmachine}.log", 1);

        LogInfo::to_log("Determined that Level $levname in Release $relname is ready to be built");
        print STDOUT "Determined That Level $levname in Release $relname Is Ready to be Built\n";

        LogInfo::to_log("Copying the ZIP File $zipname to the local machine");
        print STDOUT "Copying the ZIP File $zipname to the local machine\n";
        $args="xcopy $zipsrcdir\\$zipname \\";
        system($args) == 0 or warn "Can't issue system $args, $!, continuing";

        LogInfo::to_log("Make the root directory $blddir");
        print STDOUT "Make the root directory $blddir\n";
        mkdir($blddir, 777) or warn "Can't create $blddir, $!, continuing";

        LogInfo::to_log("Unzip the Source Files into $blddir");
        LogInfo::to_log("NOTE: Halts if multiple filenames differ only by case");
        print STDOUT "Unzip the Source Files into $blddir\n";
        print STDOUT "NOTE: Halts if multiple filenames differ only by case\n";

        if (CheckForDuplicateFiles("\\$zipname")) {
          notify("FATAL::", "Email for AVIS Build Duplicate file name");
        }

        $args="$unzippath -q \\$zipname -d $blddir";
        system($args) == 0 or warn "Can't issue system $args, $!, continuing";

        LogInfo::to_log("Erase local copy of ZIP File");
        print STDOUT "Erase local copy of ZIP File\n";
        unlink("\\$zipname") or warn "Can't delete $zipname, $!, continuing";

        # Only keep the current level on the build machine
        LogInfo::to_log("Deleting all but the current build level $levname");
        PruneOld::files("\\.", "^avis${relname}.[0-9][0-9][0-9]\$",
			($scriptdebug ? -1 : 1), 0);
        LogInfo::to_log("****************************************");

        LogInfo::to_log("Changing to Directory $blddir\\$builddir");
        print STDOUT "Changing to Directory $blddir\\$builddir\n";
        chdir("$blddir\\$builddir") or warn "Can't chdir $blddir\\$builddir, $!, continuing";

        LogInfo::to_log("Calling Perl Build Script for IcePack, Communications, Dataflow etc..");
        $args="perl buildav.pl -control ${controldir} -level ${levname} -release ${relname} -bldmachine ${bldmachine}";
      	$args .= " -scriptdebug" if $scriptdebug;
        LogInfo::to_log("Calling $args");
        LogInfo::close_log;
	if (system($args) != 0)
        {
            LogInfo::open_log("${logdir}\\${bldmachine}.log", 1);
	    LogInfo::to_log("*** Error: Can't issue above system call, continuing");
            print STDERR "Can't issue system $args, $!, continuing\n";
        }
        else
        {
            LogInfo::open_log("${logdir}\\${bldmachine}.log", 1);
        }
#        LogInfo::to_log("Finished Building Level $levname on Build Machine");
#        $curbld = $levname;

        # Reboot the machine if it is an NT machine (e.g., the program exists)
#        if ((-f $shutgui_pgm) && (-r $shutgui_pgm))
#        {
#            $args="$shutgui_pgm /L /R /T:20 /C";
#            LogInfo::to_log("Issuing Reboot Request $args");
#            system($args) == 0 or warn "Can't issue system $args, $!, continuing";
#            LogInfo::to_log("Shutting down.... in 20 seconds");
#            LogInfo::to_log("****************************************");
#            LogInfo::close_log;
#            exit 0;
#        }

#### This is where the Automatic Analysis components are built from upstair.

        LogInfo::to_log("Calling Perl Build Script for Automated Analysis components");
	$batparam = "avis${relname}.${levname}";
        LogInfo::to_log("Changing to Directory $blddir\\$upstairsbuilddir");
        print STDOUT "Changing to Directory $blddir\\$upstairsbuilddir\n";
        chdir("$blddir\\$upstairsbuilddir") or warn "Can't chdir $blddir\\$upstairsbuilddir, $!, continuing";
        $args="Isbuild.bat $batparam";
        LogInfo::to_log("Calling $args");
        LogInfo::close_log;
	if (system($args) != 0)
        {
            LogInfo::open_log("${logdir}\\${bldmachine}.log", 1);
	    LogInfo::to_log("*** Error: Can't issue above system call, continuing");
            print STDERR "Can't issue system $args, $!, continuing\n";
        }
        else
        {
            LogInfo::open_log("${logdir}\\${bldmachine}.log", 1);
        }
        LogInfo::to_log("Finished Building Level $levname on Build Machine");
        $curbld = $levname;


        LogInfo::to_log("****************************************");
        LogInfo::close_log;
    }

    # Wait before Rechecking
    print STDOUT "Current Level is $curbld....";
    print STDOUT "Waiting for $polltime Seconds\n";
    sleep($polltime);
}

# Should never get here....
exit 0;

#----------------------------------------------------------------------
# This function process the arguments parsed on the command line.
#----------------------------------------------------------------------

sub validate_arguments
{
    local(%optctl);
    local($errcnt)      = 0;

    local($bldmachine)  = undef;
    local($logdir)      = undef;

    local($controldir)  = "haw";
    local($relname)     = "100";
    local($ziprelname)  = "100";
    local($zipsrcdir)   = "u:\\avsource";
    local($debug)       = 0;

    # Retrieve the arguments
    local($retval) = &GetOptions(\%optctl, 'logdir=s', 'bldmachine=s', 'controldir=s',
				 'release=s', 'ziprelease=s', 'zipsrcdir=s', 'scriptdebug!');
    if ($retval == 0)
    {
        $errcnt++;
    }

    # Iterate through all possible values.
    foreach $_ (keys %optctl)
    {
        # HANDLE the REQUIRED ARGUMENTS
        if ($_ eq "logdir")
        {
	    $logdir=$optctl{$_};
        }
        # HANDLE the OPTIONAL ARGUMENTS
        elsif ($_ eq "bldmachine")
	{
	    $bldmachine=$optctl{$_};
	}
        elsif ($_ eq "controldir")
        {
	    $controldir=$optctl{$_};
        }
        elsif ($_ eq "release")
	{
            $relname = $optctl{$_};
        }
	elsif ($_ eq "scriptdebug")
        {
            $debug = $optctl{$_};
	}
        elsif ($_ eq "ziprelease")
	{
            $ziprelname = $optctl{$_};
        }
        elsif ($_ eq "zipsrcdir")
	{
            $zipsrcdir = $optctl{$_};
        }
    }	

    # Verify that the required arguments were specified
    if (not defined $logdir)
    {
        print STDERR "*** ERROR: Must specify a valid log directory\n";
        $errcnt++;
    }

    # Verify that a valid hostname can be found
    if (not defined $bldmachine)
    {
        # Obtain hostname of machine
	$_ =`hostname`;
        if (/^(\w+)[ \r\t\n.]*/)
        {
            $bldmachine = $1;
        }
        else
        {
 	    print STDERR "*** ERROR: Must specify valid hostname\n";
            $errcnt++;
        }
    }

    # Verify that the ZIP source directory exists
    if (not -d $zipsrcdir)
    {
            print STDERR "*** ERROR: Can't access ZIP source file directory ($zipsrcdir)\n";
            $errcnt++;
    }

    # Check for Errors....
    if ($errcnt > 0)
    {
        print "Arguments:\n";
        print "   Build Machine            -bldmachine avbld01       Optional\n";
        print "   ControlDir               -control    haw           Optional\n";
        print "   LogDir                   -logdir     m:\\logs       REQUIRED\n";
	print "   Release Name             -release    100           Optional\n";
        print "   ZIP Release Name         -ziprelease 100           Optional\n";
        print "   ZIP Source Directory     -zipsrcdir  u:\\avsource   Optional\n";
        print "Option names may be abbreviated to uniqueness.\n";
        exit 0;
    }
    else
    {
        return ($logdir, $bldmachine, $controldir, $relname, $ziprelname, $zipsrcdir, $debug);
    }
}


sub notify (@) {
  my @body = split(/\n/, join("",@_));

  # Construct values needed for SMTP headers.  Unless an email
  # gateway and address are specified, there is no point in trying
  # to send a message.    

  my $gateway = "watson.ibm.com";
  my $to = "kennyt\@us.ibm.com";
  return 1 unless $to && $gateway && hostname();
  my ($basename) = $0 =~ m/([\w $_]+)\.\w+/;
  my $from = $basename . "\@" . hostname();
  my ($date0,$date1,$time1) = timestamp();
    
  # Construct the SMTP header lines.  For diagnostic purposes, 
  # repeat the headers in the body of the message.

  my @headers;  
  push @headers, "To: $to\n";
  push @headers, "From: $from\n";
  push @headers, "Date: $date1 $time1\n";
  push @headers, "Subject: @body[0]\n";
  push @headers, "\n";
  push @headers, "to $to\n";
  push @headers, "from $from\n";
  push @headers, "about ...\n";
  push @headers, "via $gateway\n";
  push @headers, "at $date1 $time1\n";
  push @headers, "\n";

  # Send the message to the configured email address via the
  # configured email gateway.

  my $smtp = new Net::SMTP($gateway, Timeout=> 5);
  if (!defined $smtp) {warn $!; return 0;}

  # MS: $program may be defined in the mail program, but that doesn't
  # seem to be the case in practice.

  if (!$smtp->mail("AVISBuild")) {
   return 0;
  }
  $smtp->to($to);
  $smtp->data(@headers, @body);
  $smtp->quit;
  return 1;
}

=head2 hostname

This private function returns the fully-qualified TCP/IP domain name 
for the computer on which the program is running. 

=cut

sub hostname () {
  # If the host name has already been resolved by an earlier
  # call to this function, just return the saved host name.

  return $hostDomainName if $hostDomainName;
  
  # Resolve the fully-qualified TCP/IP domain name for this
  # computer, save it in a global variable for future calls
  # to this function, and return the value as the host name.

  $hostDomainName = Net::Domain::hostfqdn();
  return $hostDomainName;
}


=head2 timestamp

This private function returns the current date and time, according 
to the local system clock, in several popular formats.

=cut

sub timestamp (;$) {
    my ($when) = @_;

    $when = time unless $when;

    # Convert the current date and time into an array of familiar
    # integers, and construct an array of strings containing them 
    # in the formats "mmmddyyyy" and "mm/dd" and "hh:mm:ss".

    my @months = qw(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec);
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($when);
    my $cyear = $year + ($year<70 ? 2000 : 1900);

    return 
	(sprintf "%s%02d%04d", $months[$mon],$mday,$cyear),
	(sprintf "%02d/%02d", 1+$mon, $mday),
	(sprintf "%02d:%02d:%02d", $hour, $min, $sec),
    ;
}


# check whether a zip files contains files which have the same name (notwithstanding theb case)
sub CheckForDuplicateFiles($) {
my $zipfile = shift;

my $cmdline = "$unzippath -l $zipfile";
my %entries = ();
my $duplicates = 0;
my @zipoutput = ();

  @zipoutput = `$cmdline`;
  foreach (@zipoutput) {
    if (m/^\s+\d+\s+\d+\-\d+\-\d+\s+\d+:\d{2}\s+(.*)\n/) {
      my $entryname = $1;
      $entryname =~ s/\\/\//g;
      if (!/\/$/) { #ignore directory names
        ++$entries{lc($entryname)}{INSTANCES};
        $entries{lc($entryname)}{NAMES}{$entryname} = 1;
      }
    }
#    else {
#      warn "unable to parse $_";
#    }    
  }
  foreach $entry (keys(%entries)) {
    if ($entries{$entry}{INSTANCES} > 1) {
      ++$duplicates;
      my @duplicates = keys(%{$entries{$entry}{NAMES}});
      for ($i = 1; $i < @duplicates; ++$i) {
        print $duplicates[$i]." also exists as ".$duplicates[0]." in $zipfile", "\n";
      }
    }
  }
 return $duplicates;
}
