# Perl Routines for TopLevel Build Control Program
# Modified for Immune System Robert and Glenn
# 09/29/98
#
# IBM Research
# Robert B. King
# April 29, 1997
# Last Modified on October 12, 1998
#
#

use lib "perl";

use LogInfo ();
use Getopt::Long;

local($controldir, $levname, $bldmachine, $relname, $scriptdebug) = &validate_arguments;

local(%control_info);
&read_build_control_file($controldir, "buildcontrol.bld", $bldmachine, \%control_info, undef);

# Verify that LOCAL and NETWORK Build Drives are Defined (reserved variables)
if (not defined ${%control_info}{LOCALDRIVE})
{
    print STDERR "No LOCALDRIVE defined in Configuration File\n";
    exit 1;
}
if (not defined ${%control_info}{NETWORKDRIVE})
{
    print STDERR "No NETWORKDRIVE defined in Configuration File\n";
    exit 1;
}

# Compute the local and network build tree roots
#local($blddir) = ${${%control_info}{LOCALDRIVE}}[0]   . ":\\is${relname}.${levname}";
#local($resdir) = ${${%control_info}{NETWORKDRIVE}}[0] . ":\\is${relname}.${levname}";

# ----- Changed by Glenn Stubbs ----- Compute the local and network build tree roots
local($blddir) = ${${%control_info}{LOCALDRIVE}}[0]   . ":\\avis${relname}.${levname}";
local($resdir) = ${${%control_info}{NETWORKDRIVE}}[0] . ":\\avis${relname}.${levname}";

# Open the buildmachine logfile
&make_path("$resdir\\buildlog");
LogInfo::open_log("$resdir\\buildlog\\${bldmachine}.log", 1);

# Write the control information into the logfile
LogInfo::to_log("Control Information for Build Machine $bldmachine....");
print STDOUT "Control Information for Build Machine $bldmachine....\n";
foreach (sort (keys (%control_info)))
{
    LogInfo::to_log("    $_ = |", join(' ', @{${%control_info}{$_}}), "|");
    print STDOUT "    $_ = |", join(' ', @{${%control_info}{$_}}), "|\n";
}
LogInfo::to_log("\n");
print STDOUT "\n";

# Change directory to the build directory (probably already there)
chdir("$blddir\\src\\build") or die "Can't chdir $blddir\\src\\build, $!, stopped";

# Loop to Perform the Appropriate Actions
local($tobuild);
foreach $tobuild (@{${%control_info}{ORDER}})
{
    # Check to see if we need to alter processing to handle commands for each language
    if ($tobuild =~ /^(FOREACHLANG)((_)(\w+))?$/)
    {
        local(@lang,$dolang);

        if (defined @{${%control_info}{"${tobuild}.LANG"}})
        {
            @lang=@{${%control_info}{"${tobuild}.LANG"}};
        }
        elsif (defined @{${%control_info}{LANG}})
        {
            @lang=@{${%control_info}{LANG}};
        }

        LogInfo::to_log("$tobuild = |", join(' ', @lang), "|");
        print STDOUT "$tobuild = |", join(' ', @lang), "|\n";

        foreach $dolang (@lang)
        {
            LogInfo::to_log("$tobuild Language = $dolang");
            print STDOUT "$tobuild Language = $dolang\n";

            foreach $langbuild (@{${%control_info}{$tobuild}})
            {
                &perform_build_actions($scriptdebug, $langbuild, \%control_info, $controldir, $blddir, $resdir, $levname, $relname, $dolang );
            }
        }

        LogInfo::to_log("$tobuild Has Finished");
        print STDOUT "$tobuild Has Finished\n";
    }
    else
    {
        &perform_build_actions($scriptdebug, $tobuild, \%control_info, $controldir, $blddir, $resdir, $levname, $relname);
    }
}

LogInfo::close_log;
exit 0;


#----------------------------------------------------------------------
#
# perform_build_actions(filename, machinename)
#
# This function reads in the build control file from the specified file.
#
#----------------------------------------------------------------------

sub perform_build_actions
{
    local($scriptdebug, $tobuild, $control_info, $controldir, $blddir, $resdir, $levname, $relname, $lang) = @_;

    # First handle special cases that are specific commands....
    if ($tobuild =~ /^(XCOPY)((_)(\w+))?$/)
    {
        foreach (@{${%$control_info}{$tobuild}})
        {
            LogInfo::to_log("xcopy_files($scriptdebug, $blddir, $resdir, xcopy.$_.lst)");
            print STDOUT "xcopy_files($scriptdebug, $blddir, $resdir, xcopy.$_.lst)\n";
            &xcopy_files($scriptdebug, $blddir, $resdir, "${controldir}\\xcopy.$_.lst");
        }
    }
    elsif ($tobuild =~ /^(DONE)((_)(\w+))?$/)
    {
        LogInfo::to_log("write DONE files for $tobuild");
        print STDOUT "write DONE files for $tobuild\n";
        &indicate_bld_status($scriptdebug, $resdir, $control_info, $tobuild, $lang);
    }
    elsif ($tobuild =~ /^(WAIT)((_)(\w+))?$/)
    {
        LogInfo::to_log("wait upto 1 day for WAIT files for $tobuild");
        print STDOUT "wait upto 1 day for WAIT files for $tobuild\n";
        &wait_for_bld_status($scriptdebug, $resdir, $control_info, $tobuild, $lang, 1440, 60);
    }
    elsif ($tobuild =~ /^(DONELOCAL)((_)(\w+))?$/)
    {
        LogInfo::to_log("write local DONE files for $tobuild");
        print STDOUT "write local DONE files for $tobuild\n";
        &indicate_bld_status($scriptdebug, $blddir, $control_info, $tobuild, $lang);
    }
    elsif ($tobuild =~ /^(WAITLOCAL)((_)(\w+))?$/)
    {
        LogInfo::to_log("local wait upto 8 hours for WAIT files for $tobuild");
        print STDOUT "local wait upto 8 hours for WAIT files for $tobuild\n";
        &wait_for_bld_status($scriptdebug, $blddir, $control_info, $tobuild, $lang, 480, 60);
    }
    elsif ($tobuild =~ /^(FOREACHLANG)((_)(\w+))?$/)
    {
        LogInfo::to_log("$tobuild Ignored - Recursive FOREACHLANG Not Allowed");
        print STDOUT "$tobuild Ignored - Recursive FOREACHLANG Not Allowed\n";
    }
    else
    {
        LogInfo::to_log("Time to Issue Build for $tobuild");
        print STDOUT "Time to Issue Build for $tobuild\n";
        &call_build_scripts($scriptdebug, $control_info, $levname, $relname, $tobuild, $lang);
    }
}
#----------------------------------------------------------------------
#
# read_build_control_file(filename, machinename)
#
# This function reads in the build control file from the specified file.
#
#----------------------------------------------------------------------

sub read_build_control_file
{
    # Parse the parameter list in the usual way.
    local($filedir,$filename,$machinename,$control_keys,$template) = @_;

    # Open the control file.
    open(INPUT, "<$filedir\\$filename")
	or warn "Can't open $filedir\\$filename, $!, continuing";

    # For each line in the file
    while (<INPUT>)
    {
        next if /^[\b\t]*#/ || /^[\b\t]*$/;   # Ignore Comments and Blank Lines

        # Check for control strings
        if (/^(%include)[ \t]+(.+)[ \r\t\n]*$/)
        {
            print "Including $2\n";
            &read_build_control_file($filedir,$2,$machinename,$control_keys,$template);
            next;
        }
        elsif (/^(%iftime)[ \t]+(.+)[ \r\t\n]*$/)
        {
            # Syntax: %iftime hour dayofmonth month weekday (wildcard is *)
            local($sec,$min,$hour,$mday,$mon,$year,$wday) = localtime(time);
            local($chk_hour,$chk_mday,$chk_mon,$chk_wday) = split / /, $2;

	    LogInfo::to_log("Current Time = $hour $mday $mon $wday");
	    LogInfo::to_log("Control Time = $chk_hour $chk_mday $chk_mon $chk_wday");

            # Check to see if this clause should be entered....
            if ((($chk_hour ne '*') && ($chk_hour != $hour)) ||
                (($chk_mday ne '*') && ($chk_mday != $mday)) ||
                (($chk_mon  ne '*') && ($chk_mon  != $mon))  ||
                (($chk_wday ne '*') && ($chk_wday != $wday)))
            {
                # Skip until %endif or %else is found
                while (<INPUT>)
                {
                    if ((/^(%endif)/) || (/^(%else)/))
                    {
                        last;
                    }
                }
            }
            next;
        }
        elsif (/^(%else)/)
        {
            # Skip until %endif is found
            while (<INPUT>)
            {
                if (/^(%endif)/)
                {
                    last;
                }
            }
            next;
        }
        elsif (/^(%endif)/)
        {
            next;
        }

        # Parse the line
        if (/^[ \n\t\r]*(\w+)[ \t]+([\w.]+)[ \t]+(.+)[ \r\t\n]*$/)
        {
	    if (($machinename eq $1) && ("TEMPLATE" eq $2))
            {
                $template = $3;
            }
            elsif ($machinename eq $1)
            {
                @$control_keys{$2} = [ split(' ', $3) ];
            }
            elsif ((((defined $template) && ($template eq $1)) || ("BUILD" eq $1)) &&
		   (not defined @$control_keys{$2}))
            {
                @$control_keys{$2} = [ split(' ', $3) ];
            }
	}
    }
}

#----------------------------------------------------------------------
# This function calls the build scripts using the specified parameters.
#----------------------------------------------------------------------

sub call_build_scripts
{
    local($debug,$control_info,$levname,$relname,$tobuild,$lang) = @_;

    local($bldplat);
    local($argslist);

    # Check to see if there are multiple instances of the command
    if ($tobuild =~ /^(\w+)[.](\w+)$/)
    {
        $argslist = "perl $1.pl ";
        $bldplat  = $2;
    }
    else
    {
        $argslist = "perl ${tobuild}.pl ";
    }

    # Add the Release and Level Information....
    $argslist .= "-release $relname -level $levname ";

    # Add Build Platform Type (dos, os2, w32, w16)...
    if (defined $bldplat)
    {
        $argslist .= "-buildos $bldplat ";
    }

    # Add Build Drive
    if (defined @{${%$control_info}{"${tobuild}.DRIVE"}})
    {
        $argslist .= "-drive ";
        $argslist .= ${${%$control_info}{${${%$control_info}{"${tobuild}.DRIVE"}}[0]}}[0];
        $argslist .= ' ';
    }
    elsif (defined @{${%$control_info}{DRIVE}})
    {
        $argslist .= "-drive ";
        $argslist .= ${${%$control_info}{${${%$control_info}{DRIVE}}[0]}}[0];
        $argslist .= ' ';
    }

    # Add Build Spin Number
    if (defined @{${%$control_info}{"${tobuild}.SPIN"}})
    {
        if (${${%$control_info}{"${tobuild}.SPIN"}}[0] ne '\'\'')
        {
            $argslist .= "-spin ";
            $argslist .= ${${%$control_info}{"${tobuild}.SPIN"}}[0];
            $argslist .= ' ';
        }
    }
    elsif (defined @{${%$control_info}{SPIN}})
    {
        if (${${%$control_info}{SPIN}}[0] ne '\'\'')
        {
            $argslist .= "-spin ";
            $argslist .= ${${%$control_info}{SPIN}}[0];
            $argslist .= ' ';
        }
    }

    # Add Language List
    if (defined $lang)
    {
        $argslist .= "-lang $lang ";
    }
    elsif (defined @{${%$control_info}{"${tobuild}.LANG"}})
    {
        foreach (@{${%$control_info}{"${tobuild}.LANG"}})
        {
            $argslist .= "-lang $_ ";
        }
    }
    elsif (defined @{${%$control_info}{LANG}})
    {
        foreach (@{${%$control_info}{LANG}})
        {
            $argslist .= "-lang $_ ";
        }
    }

    # Add Build Type
    if (defined @{${%$control_info}{"${tobuild}.BUILDTYPE"}})
    {
        foreach (@{${%$control_info}{"${tobuild}.BUILDTYPE"}})
        {
            $argslist .= "-$_ ";
        }
    }
    elsif (defined @{${%$control_info}{BUILDTYPE}})
    {
        foreach (@{${%$control_info}{BUILDTYPE}})
        {
            $argslist .= "-$_ ";
        }
    }

    # Add Debug Option
    $argslist .= "-scriptdebug " if $debug;

    # Add Hardcoded Arguments....
    if (defined @{${%$control_info}{$tobuild}})
    {
        $argslist .= join ' ', @{${%$control_info}{$tobuild}};
    }

    LogInfo::to_log("Calling $argslist");
    print STDOUT "Calling $argslist\n";

    LogInfo::close_log;
    if (system($argslist) != 0)
    {
        LogInfo::open_log("$resdir\\buildlog\\${bldmachine}.log", 1);
        LogInfo::to_log("Can't issue system $argslist, $!, continuing");
        print STDERR "Can't issue system $argslist, $!, continuing\n";
    }
    else
    {
        LogInfo::open_log("$resdir\\buildlog\\${bldmachine}.log", 1);
    }
}

#----------------------------------------------------------------------
# This function process the arguments parsed on the command line.
#----------------------------------------------------------------------

sub validate_arguments
{
    local(%optctl);
    local($errcnt)      = 0;

    local($controldir)  = undef;
    local($levname)     = undef;

    local($bldmachine)  = undef;
    local($relname)     = "100";
    local($debug)       = 0;

    # Retrieve the arguments
    local($retval) = &GetOptions(\%optctl, 'control=s', 'level=s',
				 'bldmachine=s', 'release=s', 'scriptdebug!');
    if ($retval == 0)
    {
        $errcnt++;
    }

    # Iterate through all possible values.
    foreach $_ (keys %optctl)
    {
        # HANDLE the REQUIRED ARGUMENTS
        if ($_ eq "control")
        {
	    $controldir=$optctl{$_};
        }
        elsif ($_ eq "level")
        {
            $levname = substr($optctl{$_}, 0, 3);
        }
        # HANDLE the OPTIONAL ARGUMENTS
        elsif ($_ eq "release")
	{
            $relname = $optctl{$_};
        }
	elsif ($_ eq "scriptdebug")
        {
            $debug = $optctl{$_};
	}
        elsif ($_ eq "bldmachine")
	{
	    $bldmachine=$optctl{$_};
	}
    }	

    # Verify that the required arguments were specified
    if ((not defined $controldir) || (not defined $levname))
    {
        print STDERR "*** ERROR: Must specify a valid build status and control directory\n" unless
	    defined $controldir;
	print STDERR "*** ERROR: Must specify a valid build level\n" unless
	    defined $levname;
        $errcnt++;
    }

    # Verify that a valid hostname can be found
    elsif (not defined $bldmachine)
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
    else
    {
        # Verify that the base of control directory exists
        if (not -d $controldir)
        {
            print STDERR "***** ERROR: Can't access build status and control directory ($controldir)\n";
            $errcnt++;
        }

        # Verify that the build control file exists
        if (not -r "$controldir\\buildcontrol.bld")
        {
            print STDERR "***** ERROR: Can't access build machine control dir ($controldir\\buildcontrol.bld)\n";
  	    $errcnt++;
        }
    }

    # Check for Errors....
    if ($errcnt > 0)
    {
        print "Arguments:\n";
        print "   Build Machine            -bldmachine avbld01   Optional\n";
        print "   ControlDir               -control d:\\control   REQUIRED\n";
	print "   Level Name               -level 132            REQUIRED\n";
	print "   Release Name             -release 100          Optional\n";
        print "Option names may be abbreviated to uniqueness.\n";
        exit 0;
    }
    else
    {
        return ($controldir, $levname, $bldmachine, $relname, $debug);
    }
}

#----------------------------------------------------------------------
# Generate status flag filenames....
#----------------------------------------------------------------------

sub generate_bld_status_filenames
{
    local(%buildtypes) = ( develop => 'd', product => 'r', );
    local(@buildtype,@lang,$spin,$type,$bldtype);

    local($control_info,$tobuild,$lang) = @_;

    # For each line in the file
    if (defined @{${%$control_info}{"${tobuild}.BUILDTYPE"}})
    {
        @buildtype=@{${%$control_info}{"${tobuild}.BUILDTYPE"}};
    }
    elsif (defined @{${%$control_info}{BUILDTYPE}})
    {
        @buildtype=@{${%$control_info}{BUILDTYPE}};
    }

    if (defined $lang)
    {
        @lang=($lang);
    }
    elsif (defined @{${%$control_info}{"${tobuild}.LANG"}})
    {
        @lang=@{${%$control_info}{"${tobuild}.LANG"}};
    }
    elsif (defined @{${%$control_info}{LANG}})
    {
        @lang=@{${%$control_info}{LANG}};
    }

    if (defined @{${%$control_info}{"${tobuild}.SPIN"}})
    {
        $spin = ${${%$control_info}{"${tobuild}.SPIN"}}[0];
    }
    elsif (defined @{${%$control_info}{SPIN}})
    {
        $spin = ${${%$control_info}{SPIN}}[0];
    }

    local(@filenames) = ();

    foreach $type (@{${%$control_info}{$tobuild}})
    {
        foreach $bldtype (@buildtype)
        {
  	    foreach $lang (@lang)
            {
                push @filenames, "$buildtypes{$bldtype}" . "log\\${lang}\\"
		                    . sprintf("%03d", $spin)
                                    . "\\${type}_done.txt";
	    }
        }
    }

    return(\@filenames);
}

#----------------------------------------------------------------------
# This function creates files that will be used to indicate the status
# of the build
#----------------------------------------------------------------------

sub indicate_bld_status
{
    local($debug,$dest,$control_info,$tobuild,$lang) = @_;

    local($paths) = &generate_bld_status_filenames($control_info,$tobuild,$lang);

    # For each path in the list, generate an appropriate NULL file
    foreach (@{$paths})
    {
	local($filename)="$dest\\$_";
        LogInfo::to_log("Creating the file ${filename}");
        print STDOUT "Creating the file ${filename}\n";
        if (not $debug)
        {
            open(OUTPUT, ">$filename")
                or warn "Can't write $filename, $!, stopped";
            close(OUTPUT);
        }
    }
}

#----------------------------------------------------------------------
# This function waits for the specified status files to have been created
# on the file server.
#----------------------------------------------------------------------

sub wait_for_bld_status
{
    local(%waiton);

    local($debug,$dest,$control_info,$tobuild,$lang,$bldwait,$polltime) = @_;

    local($paths) = &generate_bld_status_filenames($control_info,$tobuild,$lang);

    # For each line in the file, initialize the data structure
    LogInfo::to_log("Getting names of status files");
    print STDOUT "Getting names of status files\n";
    local($lefttofind)=0;
    foreach (@{$paths})
    {
        local($filename)="$dest\\$_";
        LogInfo::to_log("    ${filename}");
        print STDOUT "    ${filename}\n";
	$waiton{${filename}}=0;
        $lefttofind += 1;
    }

    LogInfo::close_log;

    # Loop forever checking to see if the files exist
    local($startTime)=time;
    while ($lefttofind > 0)
    {
        # Check to see if all the status file exist
        foreach (keys %waiton)
        {
            if (not $waiton{"$_"})
            {
                if (-e "$_")
                {
                    $waiton{"$_"}=1;
		    $lefttofind--;
                    LogInfo::open_log("$resdir\\buildlog\\${bldmachine}.log", 1);
                    LogInfo::to_log("File $_ Exists.... $lefttofind Files Left to Find");
                    print STDOUT "File $_ Exists... $lefttofind Files Left to Find\n";
                    LogInfo::close_log;
		}
	    }
	}

	# Check to see my other build machines did not finish within a reasonable time
	local($deltaMins)=time - $startTime;
	$deltaMins /= 60;
	if (($bldwait > 0) && ($deltaMins > $bldwait))
        {
            # Timeout has occurred
            LogInfo::open_log("$resdir\\buildlog\\${bldmachine}.log", 1);
            LogInfo::to_log("Waited for Done Files for $deltaMins Minutes... giving up");
	    print STDOUT "Waited for Done Files for $deltaMins Minutes... giving up\n";
            return(0);
        }

        # Do not wait if running under debug mode
        last if ($debug);

        # Wait before checking status files
        print STDOUT "Waited $deltaMins Minutes for Other Build Machines to Finish ($polltime seconds)\n";
        sleep($polltime);
    }

    LogInfo::open_log("$resdir\\buildlog\\${bldmachine}.log", 1);
    return 1;
}


#----------------------------------------------------------------------
# This function copies files based upon the directory information
# contained in the control file.
#----------------------------------------------------------------------

sub xcopy_files
{
    local($debug,$source,$dest,$controlfile) = @_;

    # Open the control file.
    open(INPUT, "<$controlfile")
	or warn "Can't open $controlfile, $!, stopped";

    # Check to make sure that the destination directory exists
    if (not -d $dest)
    {
        &make_path($dest)
	    or warn "Can't create $dest, $!";
    }

    # For each line in the file
    while (<INPUT>)
    {
        chop;
        next if /^[\b\t]*#/ || /^[\b\t]*$/;

	if ("$_" eq ".")
        {
            local($args)="xcopy $source\\*.* $dest\\*.* /v";
        }
        else
        {
            local($args)="xcopy $source\\$_\\*.* $dest\\$_\\*.* /s /e /v";
        }
        LogInfo::to_log("Calling $args");
        print STDOUT "Calling $args\n";
        if ((not $debug) && (system($args) != 0))
        {
            LogInfo::to_log("*** Warning: No Files copied, continuing");
            print STDERR "No files copied during system $args, $!, continuing\n";
        }
    }

    close(INPUT);
}


#----------------------------------------------------------------------
#
# make_path(path)
#
# This function is used to create a path (expects \\ as separators).
#
# Adapted from File::path::mkpath.
#
#----------------------------------------------------------------------

sub make_path {
    my($path) = @_;
    local($")="\\";
    if (not -d $path)
    {
        my(@p);
        foreach(split(/\\/, $path))
        {
            push(@p, $_);
            next if -d "@p/";
	    mkdir("@p",0777) or die "Can't create @p, $!, stopped";
        }
    }
}
