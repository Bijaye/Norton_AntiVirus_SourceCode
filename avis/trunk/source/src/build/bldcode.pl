# Perl Script to Perform IBM AntiVirus Product Build
# Modified for Immune System by Glenn and Robert
# 09/30/98
#
# IBM Research
# Robert B. King
# March 6, 1997
# Last Modified on October 12, 1998
#
# Assumptions:
# (1) Build Debug means that the environment variable BLDDEBUG
#     is defined.  Build Release means that it is undefined.
#
#

use lib perl;

use bldenv ();         # Package for Setting Build Environment Variables
use filedb ();         # Package for Manipulating File Database
require 'bldargs.pl';  # Including Build Argument Analysis
require 'bldinit.pl';  # Including Build DataStructure Initialization


# Process Incoming Arguments
($AVBUILDRIVE, $AVBUILDIR, $buildspin, $AVBUILDOS,
  $buildtype, $site, $scriptdebug, $langList, $envlist) = &validate_arguments;

$AVBUILDRIVE    .= ':';
$AVBUILDLANG     = "";
$AVBUILDSPIN     = sprintf "%03u",$buildspin;

$BldEnv::AVBUILDRIVE = $AVBUILDRIVE;
$BldEnv::AVBUILDIR   = $AVBUILDIR;

$AVSTRUCT        = {};

@buildtypes      = split //, $buildtype;

# Save Arguments Passed on Command Line

$AVSTRUCT->{LANGLIST} = [ @$langList ];
$AVSTRUCT->{BUILDROOT}= $AVBUILDIR;
$AVSTRUCT->{MAKEROOT} = [ ("$AVBUILDIR\\src", "$AVBUILDIR\\resdoc") ];

# Initialize Build Subsystem and Control Structure

&initialize_control_structure("$AVBUILDIR\\src\\build",$site,
			      $AVBUILDOS,\@buildtypes,$envlist);

# Read Build Version Information from Header File
local($versionname,$vrmname,$buildname)=&read_bldinfo_header_file($AVBUILDIR);

# Save the current environment (includes global "setup" settings)
# DOS environment is saved after the file is created.
%saveBldEnv=%ENV;

# Change directory to the root of the source tree
chdir $AVBUILDIR or die "Can't cd to $AVBUILDIR, $!, stopped";

# For each build target, iterate through all of the appropriate
# makefile platforms.

foreach $target (@{$AVSTRUCT->{TARGETS}})
{
    # For each platform...
    foreach $platform (keys %{$AVSTRUCT->{PLATFORM_DB}})
    {
        # For each language that must be processed
	foreach $AVBUILDLANG (@{$AVSTRUCT->{LANGLIST}})
	{
            # Verify that Language Settings Exist (otherwise it is not supported)
            next if (not defined @{$AVSTRUCT->{LANG_ENV}{$AVBUILDLANG}});

# ######### ######################################################################################
            # DOS Redirections
            if ($AVSTRUCT->{CREATEBAT})
            {
                # Do for each debug and release build
                foreach $type (@buildtypes)
                {

   	            local($logfile) = "${type}log\\$AVBUILDLANG\\$AVBUILDSPIN\\${AVBUILDOS}_$platform.txt";

                    # Print to screen for DOS builds
                    print STDOUT "** $builddescriptions{$type} $AVBUILDLANG Build Batch File for Platform $platform with Target $target **\n";

                    # Build command file for this specific makefile
                    local($batfile) = "$AVBUILDIR\\${type}obj\\$AVBUILDLANG\\build\\$target.bat";

                    open DOSBAT, ">>$batfile" or
                        die "Can't redirect to dosbat to $batfile, $!, stopped";

                    # Identification in the Batch File
                    print DOSBAT "\@ECHO ", "*" x 70, "\n";
                    print DOSBAT "\@ECHO ***** Building Target $target for $builddescriptions{$type} ",
		                 "*" x 20, "\n";
                    print DOSBAT "\@ECHO ", "*" x 70, "\n";

                    # For each makefile
                    foreach $makefile (@{$AVSTRUCT->{PLATFORM_DB}{$platform}})
                    {
                        # Set the Target-Specific Environment Settings
                        &setenv_if_defined("TARGET_ENV", $target);

                        # Set the Language Environment Settings
                        &setenv_if_defined("LANG_ENV", $AVBUILDLANG);

                        # Set the variable so that language dependent environment settings will work
                        $BldEnv::AVBUILDLANG = $AVBUILDLANG;

                        # Check for Debug Build versus Release Build
                        if ($type eq "d")
                        {
                            $ENV{BLDDEBUG}=1;
                            print DOSBAT "set BLDDEBUG=1\n";
                        }
    	                $BldEnv::AVBUILDTYPE=$type;

                        # Set the Log File Directory environment variable
                        $ENV{LOGD}="$AVBUILDIR\\${type}log\\$ENV{LANG1}\\$AVBUILDSPIN";
                        print DOSBAT "set LOGD=$AVBUILDIR\\${type}log\\\%LANG1\%\\$AVBUILDSPIN\n";

                        # Load the appropriate environment settings for overall build
                        &setenv_if_defined("BLDPLAT_ENV", "build");

                        # Load the platform-specific environment settings
                        # BldEnv::set_environment does nothing if environment is undefined.
                        &setenv_if_defined("BLDPLAT_ENV", $platform);
                        local($plattype)=$type . '_' . $platform;
                        &setenv_if_defined("BLDPLAT_ENV", $plattype);
                        $plattype = $platform . '.com';
                        &setenv_if_defined("BLDPLAT_ENV", $plattype);

                        # Set the build name, release name, and vrm name
                        $ENV{AVIS_VERSION} = $versionname;
                        $ENV{AVIS_BUILD}   = $buildname;
                        $ENV{AVIS_VRM}     = $vrmname;
                        print DOSBAT "set AVIS_VERSION=$versionname\n";
                        print DOSBAT "set AVIS_BUILD=$buildname\n";
                        print DOSBAT "set AVIS_VRM=$vrmname\n";

                        # Print the current environment variable settings
                        print DOSBAT "\@ECHO \"\"\n";
                        print DOSBAT "ECHO Current Environment Variables for $makefile\n";
                        print DOSBAT "\@ECHO \"\"\n";
                        print DOSBAT "SET\n";

                        # Build it
                        if ($scriptdebug == 0)
                        {
                            print DOSBAT "ECHO ON\n";
       	  	            print DOSBAT "$ENV{MAKECOMMAND} $makefile $target\n";
                            print DOSBAT "\@ECHO OFF\n";
		        }
                        else
                        {
                            print DOSBAT "\@ECHO \"\"\n";
                            print DOSBAT "ECHO REM $ENV{MAKECOMMAND} $makefile $target\n";
                        }

                        # Reset Current Environment
                        BldEnv::reset_environment(\*DOSBAT, \%saveBldEnv);

                        print DOSBAT "\@ECHO \"\"\n";
                        print DOSBAT "\@ECHO ", "-" x 70, "\n";
                    }

                    # Reset DOS Redirections
                    close DOSBAT;
                }
            }
# ######### ######################################################################################
            # NT/OS2 Redirections
            else
            {
                # Set the Target-Specific Environment Settings (done here because of DOS)
                # (Should not be a bad performance hit since this is usually small and
                # it is cached in memory).
                &setenv_if_defined("TARGET_ENV", $target);

                # Set the Language Environment Settings
                &setenv_if_defined("LANG_ENV", $AVBUILDLANG);

                # Set the variable so that language dependent environment settings will work
                $BldEnv::AVBUILDLANG = $AVBUILDLANG;

                # Save the current environment with the language and target settings.
                local(%saveLangEnv)=%ENV;

	        # For each debug and release build
	        foreach $type (@buildtypes)
                {
                    # "Real" (current) builds
                    # Set the environment variable for the Log File Directory
                    $ENV{LOGD}="$AVBUILDIR\\${type}log\\$AVBUILDLANG\\$AVBUILDSPIN";

                    # Create the name of the Log File
                    local($logfile) = "$ENV{LOGD}\\${AVBUILDOS}_${platform}.txt";

                    # Save current stdout and stderr filehandles
                    open SAVEOUT, ">&STDOUT";
                    open SAVEERR, ">&STDERR";

                    # Redirect to the log file
                    open STDOUT, ">>$logfile" or
   		        die "Can't redirect stdout to $logfile, $!, stopped";
                    open STDERR, ">>&STDOUT" or
  		        die "Can't dup stdout, $!, stopped";

                    # Make unbuffered
                    select STDERR; $|=1;
                    select STDOUT; $|=1;

                    # Print header to log file
                    print STDOUT "\n\n\n", "*" x 75, "\n";
                    print STDOUT "***** Building Target $target for $builddescriptions{$type} *****\n";
                    print STDOUT "*" x 75, "\n";

                    # Print to screen for NT/OS2 builds
                    print SAVEOUT "** $builddescriptions{$type} $ENV{LANG1} Build of Platform $platform using Make $target **\n";
                    # Check for Debug Build versus Release Build
                    if ($type eq "d")
                    {
                        $ENV{BLDDEBUG}=1;
                    }
	            $BldEnv::AVBUILDTYPE=$type;

                    # Load the appropriate environment settings for overall build
                    &setenv_if_defined("BLDPLAT_ENV", "build");

                    # Load the platform-specific environment settings
                    # BldEnv::set_environment does nothing if environment is undefined.
                    &setenv_if_defined("BLDPLAT_ENV", $platform);
	            local($plattype)=$type . '_' . $platform;
                    &setenv_if_defined("BLDPLAT_ENV", $plattype);
                    $plattype = $platform . '.com';
                    &setenv_if_defined("BLDPLAT_ENV", $plattype);

                    # Set the build name, release name, and vrm name
                    $ENV{AVIS_VERSION} = $versionname;
                    $ENV{AVIS_BUILD}   = $buildname;
                    $ENV{AVIS_VRM}     = $vrmname;

                    # Print environment settings to log file
                    BldEnv::print_environment;

                    # Build it
                    foreach (@{$AVSTRUCT->{PLATFORM_DB}{$platform}})
                    {
                        my($args)= "$ENV{MAKECOMMAND} $_ $target";

                        print STDOUT "\n%%% Calling $args\n";

                        if ($scriptdebug == 0)
                        {
                            if (system($args) != 0)
                            {
                                print STDERR "Can't issue system $args, $!, continuing\n";
			    }
                        }
                    }

                    # Reset Log File Redirection
                    # Close the Log File
                    close STDOUT;
                    close STDERR;

                    # Restore stdout and stderr filehandles
                    open STDOUT, ">&SAVEOUT";
                    open STDERR, ">&SAVEERR";

                    # Reset the Environment to language and target settings only
                    BldEnv::reset_environment(undef, \%saveLangEnv);
		}

                # Reset Current Environment
                BldEnv::reset_environment(undef, \%saveBldEnv);

            }
# ######### ######################################################################################
        }
    }
}

# If the platform is either NT or OS/2 (e.g., we completed the build),
# then exit since it is finished.  However, if we were creating a batch
# file as in for DOS, then perform some post processing.
if ($AVSTRUCT->{CREATEBAT} == 0)
{
    exit 0;
}

# Perform the additional processing for DOS prior to invoking the DOS build.
foreach $type (@buildtypes)
{
    # For each language, create the DOS execution file which will be called each time a new DOS
    # build window is created (a new one for each language/target combination)
    foreach $AVBUILDLANG (@{$AVSTRUCT->{LANGLIST}})
    {
        # Verify that Language Settings Exist (otherwise it is not supported)
        next if (not defined @{$AVSTRUCT->{LANG_ENV}{$AVBUILDLANG}});

        # Copy the main DOS batch file into appropriate subdirectory
        my($args)= "copy $AVBUILDIR\\builddos.bat $AVBUILDIR\\${type}obj\\$AVBUILDLANG\\build\\builddos.bat";
        print STDOUT "\n%%% Calling $args\n";
        if (system($args) != 0)
        {
            print STDERR "Can't issue system $args, $!, continuing\n";
            next;
        }

        # Open the DOS batch file to append appropriate CALL statements
        open DOSBAT, ">>$AVBUILDIR\\${type}obj\\$AVBUILDLANG\\build\\builddos.bat" or
	    die "Can't open $AVBUILDIR\\${type}obj\\$AVBUILDLANG\\build\\builddos.bat, $!, stopped";

        print DOSBAT "$AVBUILDRIVE\n";
        print DOSBAT "cd $AVBUILDIR\n";
        print DOSBAT "ECHO Calling DOS $builddescriptions{$type} Build Batch File for Language $AVBUILDLANG (Target %1)\n";
        print DOSBAT "CALL ${type}obj\\$AVBUILDLANG\\build\\\%1.bat >${type}log\\$AVBUILDLANG\\$AVBUILDSPIN\\${AVBUILDOS}_\%2.txt 2>&1\n";
        print DOSBAT "EXIT\n";

        close DOSBAT;
    }

    # Open the DOS batch file to append appropriate START statements
    open DOSBAT, ">>$AVBUILDIR\\${type}obj\\builddos.tmp" or
        die "Can't open $AVBUILDIR\\${type}obj\\builddos.tmp, $!, stopped";
    print DOSBAT "\@ECHO OFF\n";

    # Update this new main DOS batch file to call each individual LANG/target batch file
    foreach $target (@{$AVSTRUCT->{TARGETS}})
    {
        foreach $AVBUILDLANG (@{$AVSTRUCT->{LANGLIST}})
        {
            # Verify that Language Settings Exist (otherwise it is not supported)
            next if (not defined @{$AVSTRUCT->{LANG_ENV}{$AVBUILDLANG}});

	    print DOSBAT "$AVBUILDRIVE\n";
	    print DOSBAT "cd $AVBUILDIR\n";
            print DOSBAT "ECHO Calling DOS $builddescriptions{$type} Build Batch File for Language $AVBUILDLANG (Target $target)\n";
            print DOSBAT "START \"DOS $builddescriptions{$type} $AVBUILDLANG $target\" /SEPARATE /MIN /WAIT ${type}obj\\$AVBUILDLANG\\build\\builddos.bat $target " .
                substr($target, 0, 4) . "\n";
        }
    }

    # Create DOS_DONE.txt file now that the DOS build has completed
    foreach $AVBUILDLANG (@{$AVSTRUCT->{LANGLIST}})
    {
        # Verify that Language Settings Exist (otherwise it is not supported)
        next if (not defined @{$AVSTRUCT->{LANG_ENV}{$AVBUILDLANG}});

        local($donefile) = "${type}log\\$AVBUILDLANG\\$AVBUILDSPIN\\${AVBUILDOS}_done.txt";

	print DOSBAT "$AVBUILDRIVE\n";
	print DOSBAT "cd $AVBUILDIR\n";
        print DOSBAT "ECHO DOS IS DONE >$donefile 2>&1\n";
    }

    close DOSBAT;

    # Rename the main DOS batch file to an name which indicates that it is done
    if (rename("$AVBUILDIR\\${type}obj\\builddos.tmp",
               "$AVBUILDIR\\${type}obj\\builddos.bat") == 0)
    {
        print STDERR "Can't rename to $AVBUILDIR\\${type}obj\\builddos.bat, $!, continuing\n";
        next;
    }
}

exit 0;


#----------------------------------------------------------------------
#
# setenv_if_defined
#
# This function is used to alter the environment variables only if the
# array is defined.
#
#----------------------------------------------------------------------

sub setenv_if_defined
{
    # Parse the parameter list in the usual way
    local($category,$envname) = @_;

    # Check to see if the value is defined
    if (defined @{$AVSTRUCT->{$category}{$envname}})
    {
        # Since it is defined, check to see if creating a batch file.
	if ($AVSTRUCT->{CREATEBAT})
	{
            # Create a batch file... call with a pointer to the file handle
	    BldEnv::set_environment(\*DOSBAT, \@{$AVSTRUCT->{$category}{$envname}});
	}
        else
        {
            # Do not create a batch file... call with "undef" as the file handle
	    BldEnv::set_environment(undef, \@{$AVSTRUCT->{$category}{$envname}});
	}
    }
}

#----------------------------------------------------------------------
#
# read_bldinfo_header_file
#
# This function is used to read the "bldinfo.h" header file to obtain
# the Version, VRM, and Build Number.
#
#----------------------------------------------------------------------

sub read_bldinfo_header_file
{
    local($version) = undef;
    local($vrm)     = undef;
    local($build)   = undef;

    # Parse the parameter list in the usual way
    local($rootname) = @_;

    # Compute filename
    local($filename) = "$rootname\\src\\inc\\bldinfo.h";

    # Open the specified file
    open(INPUT, "<$filename")
	or die "Can't open $filename, $!, stopped";

    # For each line in the file
    while (<INPUT>)
    {
        next unless /^#define/;

        # Check for the version number
        if (/^#define[ \t]+AVIS_VERSION[ \t]+"(.+)"[ \r\t\n]*$/)
        {
            $version=$1;
	}
        # Check for the version number
        elsif (/^#define[ \t]+AVIS_VRM[ \t]+"(.+)"[ \r\t\n]*$/)
        {
            $vrm=$1;
	}
        # Check for the version number
        elsif (/^#define[ \t]+AVIS_BUILD[ \t]+"(.+)"[ \r\t\n]*$/)
        {
            $build=$1;
	}
    }

    close(INPUT);
    return($version, $vrm, $build);
}
