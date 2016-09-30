#!c:/perl/bin/perl

use strict;

use SDGS;
use BOSupport;

use Win32::Process;
use File::Basename;

my $retry_cycle = 30; # seconds


# Package type to build and retrieve. 
my $definition_packaging = "VDB";
my $package_extension    = "vdb";

# my $definition_packaging = "FatAlbert";
# my $package_extension = "exe";

my $parm_hash = BOSupport::parse_parms(@ARGV);

# Formal Input parameter declarations
my $import_unc        = $parm_hash->{'UNC'};
my $import_parameter  = $parm_hash->{'Parameter'};
my $import_cookie     = $parm_hash->{'Cookie'};
my $import_process_id = $parm_hash->{'ProcessId'};

# Following are the values that are to be changed between the
# real pilot analysis center and the other test analysis centers
# used by the developers.
my $buildDefinitionsBaseDir = "d:\\bin";

# Define a name for the log file where all the events will be recorded.
my $buildDefinitionsLogFileName = "$buildDefinitionsBaseDir\\BuildDefinitions.log";


# script specific parms
my ($import_parameter_buildtype,$import_parameter_filename) = split /\s\s/,$import_parameter;


# Package temp destination
my $pkg_tmp_file = "c:\\temp\\navpkg.$package_extension";
my $pkg_temp_unpack = "c:\\temp\\navpkg";

mkdir ($pkg_temp_unpack,777) unless -d $pkg_temp_unpack;

# Rename the log file to a backup if the size exceeds 1MB. 
if (-e $buildDefinitionsLogFileName)
{
   my $logFileSize;
   $logFileSize = -s $buildDefinitionsLogFileName;

   if ($logFileSize > 1024000)
   {
       system ("copy $buildDefinitionsLogFileName $buildDefinitionsLogFileName.bak");
       unlink ($buildDefinitionsLogFileName);
   }
}         

# Create or open the log file.
if (-e $buildDefinitionsLogFileName)
{               
   open (LOGFILEHANDLE, ">>$buildDefinitionsLogFileName ");
}
else
{
   open (LOGFILEHANDLE, ">$buildDefinitionsLogFileName "); 
}


############################################################
# Local Logic
############################################################

#-------------------------------------------------------------------
# sub start_build
#-------------------------------------------------------------------
# Kick off a build depending on the build type.  Pass in the 'source
# file' name.
# Return status.
#-------------------------------------------------------------------
sub start_build
{
    my ($build_type,$source_file) = @_;
    
    my $status = $SDGS::SDGS_IN_PROGRESS;
    my $sequence = "";
    
    if ($build_type eq 'Full')
    {
        print LOGFILEHANDLE (getCurrentTime() . "(Inside start_build) Calling SDGS::StartFullDataBuild function with the following function arguments.\n");
        print LOGFILEHANDLE (getCurrentTime() . "source_file = $source_file.\n"); 
        ($status,$sequence) = SDGS::StartFullDataBuild($source_file);
    }
    else
    {
        print LOGFILEHANDLE (getCurrentTime() . "(Inside start_build) Calling SDGS::StartIncrementalDataBuild function with the following function arguments.\n");
        print LOGFILEHANDLE (getCurrentTime() . "source_file = $source_file.\n"); 
        ($status,$sequence) = SDGS::StartIncrementalDataBuild($source_file);
    }

    print LOGFILEHANDLE (getCurrentTime() . "Returning from start_build function with status = $status, sequence = $sequence.\n"); 
    return ($status,$sequence);
}

#-------------------------------------------------------------------
# sub check_build
#-------------------------------------------------------------------
# check (once) the definition generation service, pass back the
# returned status.
#-------------------------------------------------------------------
sub check_build
{
    my ($build_type) = @_;
    my $status = $SDGS::SDGS_IN_PROGRESS;

    if ($build_type eq 'Full')
    {
        $status = SDGS::FullDataBuildStatus();
    }
    else
    {
        $status = SDGS::IncrementalDataBuildStatus();
    }
    return $status;
}

#-------------------------------------------------------------------
# sub run_build
#-------------------------------------------------------------------
# Given a build type and 'source file', run a build (of the
# appropriate type 'full' or 'incremental'.
#
# return the build status and sequence number (if any) returned by the
# build process.
#-------------------------------------------------------------------
sub run_build
{
    my ($build_type,$source_file) = @_;

    print LOGFILEHANDLE (getCurrentTime() . "(Inside run_build) Calling start_build function with the following function arguments.\n");
    print LOGFILEHANDLE (getCurrentTime() . "build_type = $build_type, source_file = $source_file.\n"); 
    my ($status,$sequence) = start_build($build_type,$source_file);
    
    while ($status == $SDGS::SDGS_IN_PROGRESS)
    {
        $status = check_build($build_type);
        # Added the following line on May/12/2000.
        # We should not do a tight loop here. That causes the 
        # build status query HTTP transaction to fail randomly. 
        # (e-g: 1 in every 100,000 transactions).
        # That transaction error leads to critical error. 
        # Hence, let us query once in 10 seconds.
        sleep (10);
    }

    print LOGFILEHANDLE (getCurrentTime() . "Returning from run_build function with status = $status, sequence = $sequence.\n"); 
    return ($status,$sequence);
}

#-------------------------------------------------------------------
# sub get_package
#-------------------------------------------------------------------
# Retrieve the built package to the parametrized location.
#-------------------------------------------------------------------
# 9-10-99:MPW:
# Modified to take:
#         $buildtype ('Full' or 'Incremental')
#         $sequence  (package sequence number to retrieve)
# Due to a implementation problem in SDGS, a "Full" build may only be
# retrieved by using getNewestpackage (previously only used by
# getdefinitions.pl) with the "latest available sequence number".  As
# a HACK, I am passng getNewestPackage a decremented sequence number
# so that it retrieves the correct package.
# Unfortunately, the incremental package cannot be retrieved in this
# manner, because it has no sequence number.
#-------------------------------------------------------------------
sub get_package
{
    # 9-10-99 MPW: added to take $import_parameter_buildtype and $sequence

    my ($buildtype,$sequence,$filename) = @_;
    my $sdgs_response = $SDGS::SDGS_ERROR;
    my $new_sequence;

    if ($buildtype eq 'Incremental')
    {
        print LOGFILEHANDLE (getCurrentTime() . "(Inside get_package) Calling SDGS::GetBuiltPackage function with the following function arguments.\n");
        print LOGFILEHANDLE (getCurrentTime() . "packageType = $definition_packaging, filename = $filename.\n"); 
        $sdgs_response = SDGS::GetBuiltPackage($definition_packaging,$filename);
    }
    else
    {
        # The GetNewestPackage method will return the sequence number
        # of the package retreived.  This will equal $sequence.

        my $tempCnt = $sequence - 1;
        print LOGFILEHANDLE (getCurrentTime() . "(Inside get_package) Calling SDGS::GetNewestPackage function with the following function arguments.\n");
        print LOGFILEHANDLE (getCurrentTime() . "packageType = $definition_packaging, filename = $filename, sequence = $tempCnt.\n"); 
        ($sdgs_response,$new_sequence) = SDGS::GetNewestPackage($definition_packaging,
                                                                $filename,
                                                                ($sequence - 1));
      }
    
    print LOGFILEHANDLE (getCurrentTime() . "Returning from run_build function with sdgs_response = $sdgs_response.\n"); 
    return $sdgs_response;
  }

########################################################################
# This subroutine gets the current Date and Time and returns to 
# the caller
#
# Function arguments: 
#
# This function doesn't require any function arguments.
########################################################################
sub getCurrentTime ()
  {
    my ($sec, $min, $hr, $mday, $mon, $year, $wday, $yday, $isdst) = localtime (time);
    $year += 1900; #Y2K compliance 
    $mon++;
    return ("<$mon/$mday/$year $hr:$min:$sec> ");
  }


print LOGFILEHANDLE ("========================================================\n");
print LOGFILEHANDLE (getCurrentTime() . "BuildDefinitions.pl started with the following command line arguments.\n"); 
print LOGFILEHANDLE (getCurrentTime() . "import_unc = $import_unc, import_parameter = $import_parameter, import_cookie = $import_cookie, import_process_id = $import_process_id, import_parameter_buildtype = $import_parameter_buildtype, import_parameter_filename = $import_parameter_filename.\n"); 

my $IDSP_Path = "$import_unc\\navdefs\\delta";
print LOGFILEHANDLE (getCurrentTime() . "Calling run_build function with the following function arguments.\n"); 
print LOGFILEHANDLE (getCurrentTime() . "import_parameter_buildtype = $import_parameter_buildtype, IDSP_Path = $IDSP_Path.\n"); 
my ($status,$sequence) = run_build($import_parameter_buildtype,$IDSP_Path);
print LOGFILEHANDLE (getCurrentTime() . "run_build function returned: status = $status, sequence = $sequence.\n"); 

my $destination = "";
if ($status == $SDGS::SDGS_OK)
  {
    print LOGFILEHANDLE (getCurrentTime() . "Calling get_package function with the following function arguments.\n"); 
    print LOGFILEHANDLE (getCurrentTime() . "import_parameter_buildtype = $import_parameter_buildtype, sequence = $sequence, pkg_tmp_file = $pkg_tmp_file.\n"); 
    
    # 9-10-99 MPW: added to take $import_parameter_buildtype and $sequence
    $status = get_package($import_parameter_buildtype,$sequence,$pkg_tmp_file);
    print LOGFILEHANDLE (getCurrentTime() . "get_package function returned: status = $status.\n"); 
    
    if ($status == $SDGS::SDGS_OK)
      {
        print LOGFILEHANDLE (getCurrentTime() . "Calling unpack_package function with the following function arguments.\n"); 
        print LOGFILEHANDLE (getCurrentTime() . "pkg_tmp_file = $pkg_tmp_file, pkg_temp_unpack = $pkg_temp_unpack.\n"); 
        
        $status = BOSupport::unpack_package($pkg_tmp_file,$pkg_temp_unpack);
        print LOGFILEHANDLE (getCurrentTime() . "unpack_package function returned: status = $status.\n"); 
        
        if ($status == $SDGS::SDGS_OK)
        {
          print LOGFILEHANDLE (getCurrentTime() . "Calling migrate_package function with the following function arguments.\n"); 
          print LOGFILEHANDLE (getCurrentTime() . "pkg_tmp_file = $pkg_tmp_file, import_parameter_filename = $import_parameter_filename.\n"); 
          
          ($status,$destination) = 
            BOSupport::migrate_package($pkg_tmp_file,$import_parameter_filename);
          print LOGFILEHANDLE (getCurrentTime() . "migrate_package function returned: status = $status, destination = $destination.\n"); 
        }
      }
  }

# Set the export variables based on the $status code

my $next_service = "";
my $export_parameter = "";

if ($status == $SDGS::SDGS_OK)
{
    print LOGFILEHANDLE (getCurrentTime() . "After getting the SDGS result as $SDGS::SDGS_OK (SDGS::SDGS_OK), the following values are set.\n"); 

    if ($import_parameter_buildtype eq 'Incremental')
    {
        $next_service = 'IncrementalUnitTest';
    }
    else
    {
        $next_service = 'FullUnitTest';
    }

    my $sequencestring = sprintf("%08d",$sequence); #Convert to 8 characters
    $export_parameter = join ' ',($import_parameter_buildtype,$sequencestring,
                                  $destination);

    print LOGFILEHANDLE (getCurrentTime() . "next_service = $next_service, sequencestring = $sequencestring, export_parameter = $export_parameter.\n");     
}
###As of today (July 14, 1999) sdgs.pm doesn't return "SDGS_LOCKBROKEN"
###Raju & Senthil

elsif ($status == $SDGS::LOCKBROKEN)
{

    print LOGFILEHANDLE (getCurrentTime() . "After getting the SDGS result as $SDGS::LOCKBROKEN (SDGS::LOCKBROKEN), the following values are set.\n"); 

    if ($BOSupport::retry_on_error)

    {
        $next_service = 'SerializeBuild';
    }
    else
    {
        $next_service = 'CriticalError';
    }

    print LOGFILEHANDLE (getCurrentTime() . "next_service = $next_service.\n");     
}
elsif ($status == $SDGS::SDGS_FAIL)
{
    print LOGFILEHANDLE (getCurrentTime() . "After getting the SDGS result as $SDGS::SDGS_FAIL (SDGS::SDGS_FAIL), the following values are set.\n"); 

    $next_service = 'UnSerializeBuild';
    $export_parameter = 'Deferrer';
    print LOGFILEHANDLE (getCurrentTime() . "next_service = $next_service, export_parameter = $export_parameter.\n");     
}
else 
###  also covers ($status == $SDGS::SDGS_ERROR)
{
    print LOGFILEHANDLE (getCurrentTime() . "After getting the SDGS result as $SDGS::SDGS_ERROR (SDGS::SDGS_ERROR), the following values are set.\n"); 

    if ($BOSupport::retry_on_error)
    {
	if ($import_parameter_buildtype eq 'Incremental')
    	{
            $next_service = 'IncrementalDefBuilder';
    	}
    	else
    	{
            $next_service = 'FullDefBuilder';
    	}
        $export_parameter = $import_parameter;
    }
    else
    {
        $next_service = 'CriticalError';
        $export_parameter = '';
    }

    print LOGFILEHANDLE (getCurrentTime() . "next_service = $next_service, export_parameter = $export_parameter.\n");     
}


############################################################
# DataFlow post processing
############################################################


print LOGFILEHANDLE (getCurrentTime() . "Calling BOSupport::dataflow_completion function with the following function arguments.\n");     
print LOGFILEHANDLE (getCurrentTime() . "import_cookie = $import_cookie, import_process_id = $import_process_id, import_unc = $import_unc, next_service = $next_service, export_parameter = $export_parameter.\n");     

BOSupport::dataflow_completion($import_cookie,
                               $import_process_id,
                               $import_unc,
                               $next_service,
                               $export_parameter,
                               "");
