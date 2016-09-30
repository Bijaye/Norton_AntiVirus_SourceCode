# Perl Routines for Initialization of IBM AntiVirus Product Build
# IBM Research
# Robert B. King
# March 16, 1997 (last modified on May 13, 1997)
#
#
#


#----------------------------------------------------------------------
# 
# initialize_control_structure(buildir,targets)
#
# This function is used to initialize the build control structure.
#
#----------------------------------------------------------------------

sub initialize_control_structure
{
    # Parse the parameter list in the usual way
    local($buildir,$site,$buildos,$buildtypes,$bldenvlist) = @_;

    local($comControlDir) = "$buildir\\com";
    local($siteControlDir) = "$buildir\\$site";
    local($bldControlDir)  = "$buildir\\$buildos";

    # Verify that the Site Control Directory Exists
    die "No site control directory $siteControlDir, $!, stopped"
        if (not -d $siteControlDir);

    # Create the appropriate results directories 
    foreach (@{$AVSTRUCT->{LANGLIST}})
    {
        # Only Use Languages that Have Config Files
        next unless -f "$buildir\\lang\\$_.env";

        $AVBUILDLANG=$_;                        # Set Language Name
        &make_directories($comControlDir, "mkdir.lst");
    }

    # Read the List of Makefile Search Directories from "XXXroot.lst" and store
    # the names in an array.
    $AVSTRUCT->{MAKEROOT} = [ &read_make_root($siteControlDir, $buildos, $AVSTRUCT->{BUILDROOT}) ];

    # Create the Platform-Specific Database
    $AVSTRUCT->{PLATFORM_DB} = { &FileDB::create_file_db($bldControlDir, '(....).env$', '.mak$',
							 \@{$AVSTRUCT->{MAKEROOT}}, $bldenvlist) };

    # Read the List of Passes (e.g., Makefile Targets) from "targets.xxx" and
    # store the names in an array.

    $AVSTRUCT->{TARGETS} = [ &read_target_list($siteControlDir, $buildos) ];
    if (scalar($AVSTRUCT->{TARGETS}) == 0)
    {
        print STDERR "No Targets Found\n";
        die;
    }

    # Read in any Target-specific Environment Settings

    $AVSTRUCT->{TARGET_ENV} = { &read_target_environments($buildir, \@{ $AVSTRUCT->{TARGETS} }) };

    # Read in the Language Environment Settings

    $AVSTRUCT->{LANG_ENV} = { &read_language_environments($buildir) };

    # Read in the Platform-specific Environment Settings

    $AVSTRUCT->{BLDPLAT_ENV} = { &read_buildplat_environments($buildir, $buildos,
							      \%{ $AVSTRUCT->{PLATFORM_DB} }) };

    # If the platform is not DOS, then just set the environment variables that
    # indicate the locations of the tools.  This datastructure is site specific.

    if ($buildos ne "dos")
    {
	$AVSTRUCT->{CREATEBAT} = 0;

        local(@envlist) = BldEnv::read_environment("$siteControlDir\\$buildos" . "setup.env");
        BldEnv::set_environment(undef, [@envlist]);
    }

    # Create DOS Batch File and set the the build machine environment variables that
    # do not change at all during the build process.

    else
    {
        # Create the appropriate DOS build directories and/or check to make
        # sure that there are no old batch files around.  This latter check
        # is critical since we just append to the files (not recreate them).
        foreach $AVBUILDLANG (@{$AVSTRUCT->{LANGLIST}})
        {
            # Only Use Languages that Have Config Files
            next unless -f "$buildir\\lang\\$AVBUILDLANG.env";

            # For each build type (release and debug)
            foreach $type (@$buildtypes)
            {
		local($bldobj)="$AVBUILDIR\\${type}obj\\$AVBUILDLANG\\build";
		
                # Check to See if the build object directory exists
                if (-d $bldobj)
                {
                    # Read the directory and delete all batch files                
            	    opendir(DIR, $bldobj) or
               		die "Can't open $bldobj, $!, stopped";
         	    local(@dosbldir) = readdir(DIR);
        	    closedir(DIR);

  	            # Foreach Valid DOS Batch File
        	    foreach (@dosbldir)
        	    {
        		next unless -f "$bldobj\\$_";                   # Only Delete Files
		        next unless (/.bat$/i);                         # Ignore Non-Batch Files

  		        unlink "$bldobj\\$_" or
		            die "Can't unlink $bldobj\\$_, $!, stopped";
	            }
                }
		else
		{
                    mkdir($bldobj, 777)
			or die "Can't create $bldobj, $!, stopped";
		}

                # Check to See if the builddos.bat file exists
                if (-f "$AVBUILDIR\\${type}obj\\builddos.bat")
                {
                    unlink "$AVBUILDIR\\${type}obj\\builddos.bat" or
                        die "Can't unlink $AVBUIDIR\\${type}obj\\builddos.bat, $!, stopped";
                }
	    }	
        }

        open(DOSBAT, ">$AVBUILDIR\\builddos.bat")
            or die "Can't open $AVBUILDIR\\builddos.bat, $!, stopped";
        $AVSTRUCT->{CREATEBAT} = 1;

        print DOSBAT "\@ECHO OFF\n";

        local(@envlist) = BldEnv::read_environment("$siteControlDir\\$buildos" . "setup.env");
        BldEnv::set_environment(\*DOSBAT, [@envlist]);

        # Need to save any DOS variables that are defined prior to the build.
        print DOSBAT "SET SAVEPATH=%PATH%\n";
        close DOSBAT;
    }
}


#----------------------------------------------------------------------
#----------------------------------------------------------------------
#----------------------------------------------------------------------


#----------------------------------------------------------------------
# 
# make_directories(directory, filename)
#
# This function reads from a file and executes the PERL commands.
#
# 1) Directory containing the file
#
# 2) The filename to read
#
#----------------------------------------------------------------------

sub make_directories
{
    # Parse the parameter list in the usual way
    local($directory,$name) = @_;

    # Read the specified file and store the names it contains in an
    # array.
    open(INPUT, "<$directory\\$name")
	or die "Can't open $directory\\$name, $!, stopped";
    local(@dirs) = <INPUT>;
    close(INPUT);

    # For each line in the file
    foreach (@dirs)
    {
        next if /^#/;   # Ignore Comments
    
        # Parse the line
        if (/^[ \n\t\r]*(.+)[ \r\t\n]*$/)
        {
	    $dirname=eval($1);
            next if -d $dirname;
            mkdir($dirname, 777)
		or die "Can't create $dirname, $!, stopped";
        }
    }
}


#----------------------------------------------------------------------
# 
# read_target_list(directory, buildos)
#
# This function reads the list of valid targets.
#
#----------------------------------------------------------------------

sub read_target_list
{
    # Parse the parameter list in the usual way
    local($directory,$buildos) = @_;

    # Read the List of Passes (e.g., Makefile Targets) and store the names
    # in an array.
    open(INPUT, "<$directory\\$buildos" . "targt.lst")
	or die "Can't open $directory\\$buildos" . "targts.lst, $!, stopped";

    # Create an Empty List
    local(@targets)=();

    # Read each line in the file
    while (<INPUT>)
    {
        next if /^#/;                              # Ignore Comments
        next unless /^[ \n\t\r]*(.+)[ \r\t\n]*$/;  # Ignore Blank Lines
        push @targets, $1;
    }
    close(INPUT);

    # Return the list of targets
    return @targets;
}    


#----------------------------------------------------------------------
# 
# read_make_root(directory, buildos)
#
# This function reads the list of valid makefile search directories.
#
#----------------------------------------------------------------------

sub read_make_root
{
    # Parse the parameter list in the usual way
    local($directory,$buildos,$bldroot) = @_;

    # Read the List of Makefile Root Directories and store the names
    # in an array.
    open(INPUT, "<$directory\\${buildos}root.lst")
	or die "Can't open $directory\\${buildos}root.lst, $!, stopped";

    # Create an Empty List
    local(@searchmake)=();

    # Read each line in the file
    while (<INPUT>)
    {
        next if /^#/;                              # Ignore Comments
        next unless /^[ \n\t\r]*(.+)[ \r\t\n]*$/;  # Ignore Blank Lines
        push @searchmake, "$bldroot\\$1";
    }
    close(INPUT);

    # Return the list of targets
    return @searchmake;
}    


#----------------------------------------------------------------------
# 
# read_buildplat_environments(buildir,buildos,platform_db)
#
# This function reads the environment settings for all the platforms.
#
#----------------------------------------------------------------------

sub read_buildplat_environments
{
    # Parse the parameter list in the usual way
    local($buildir,$buildos,$platform_db) = @_;

    # Get Variable for bldenv Database
    local(@bldenv_db)=();
    local($plattype);

    # Get Build Environment from "build.env"
    local(@envlist) = BldEnv::read_environment("$buildir\\com\\build.env");
    $bldenv_db{build} = [ @envlist ];

    # Foreach Platform, Get their Environment List
    foreach $plats (keys %{$platform_db})
    {
        if (-f "$buildir\\$buildos\\$plats.env")
        {
            @envlist = BldEnv::read_environment("$buildir\\$buildos\\$plats.env");
	    $bldenv_db{$plats} = [ @envlist ];
        }
        
	$plattype="r_$plats";
        if (-f "$buildir\\$buildos\\$plattype.env")
        {
            @envlist = BldEnv::read_environment("$buildir\\$buildos\\$plattype.env");
	    $bldenv_db{$plattype} = [ @envlist ];
        }
        
	$plattype="d_$plats";
        if (-f "$buildir\\$buildos\\$plattype.env")
        {
            @envlist = BldEnv::read_environment("$buildir\\$buildos\\$plattype.env");
	    $bldenv_db{$plattype} = [ @envlist ];
        }

	$plattype=$plats . '.com';
        if (-f "$buildir\\com\\$plats.env")
        {
            @envlist = BldEnv::read_environment("$buildir\\com\\$plats.env");
	    $bldenv_db{$plattype} = [ @envlist ];
        }
    }
	    
    return %bldenv_db;
}


#----------------------------------------------------------------------
# 
# read_language_environments(buildir)
#
# This function reads the environment settings for all the languages.
#
#----------------------------------------------------------------------

sub read_language_environments
{
    # Parse the parameter list in the usual way
    local($buildir) = @_;

    # Get Variable for bldenv Database
    local(@envlist);
    local(@langenv_db)=();

    # Read the Language Directory
    opendir(DIR, "$buildir\\lang") or die "Can't open $buildir\\lang, $!, stopped";
    local(@langdir) = readdir(DIR);
    closedir(DIR);

    # Foreach Language File Name, Get their Environment List
    foreach (@langdir)
    {
        next unless -f "$buildir\\lang\\$_";       # Only Process Files
        next unless (/^(..).env$/i);               # Ignore Invalid File Names

        @envlist = BldEnv::read_environment("$buildir\\lang\\$_");
	$langenv_db{$1} = [ @envlist ];
    }
	    
    return %langenv_db;
}


#----------------------------------------------------------------------
# 
# read_target_environments(buildir,targets)
#
# This function reads any target-specific environment settings.
#
# The second argument is call-by-reference (instead of call by value).
#
#----------------------------------------------------------------------

sub read_target_environments
{
    # Parse the parameter list in the usual way
    local($buildir,$targets) = @_;

    # Get Variable for bldenv Database
    local(@envlist);
    local(@targenv_db)=();

    # Foreach Platform, Get their Environment List
    foreach $target (@$targets)
    {
        if (-f "$buildir\\com\\$target.env")
        {
            @envlist = BldEnv::read_environment("$buildir\\com\\$target.env");
	    $targenv_db{$target} = [ @envlist ];
        }
    }
	    
    return %targenv_db;
}

1;
