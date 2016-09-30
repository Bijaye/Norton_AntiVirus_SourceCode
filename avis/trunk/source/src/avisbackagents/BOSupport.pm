package BOSupport;
############################################################################
# BOSupport.pm
############################################################################
# Mon Dec 13 15:23:32 1999: MPW
############################################################################
#
# Modifications in support of the new VDB package generation (and in
# the interest of abstracting such details)
#
#  (unpack_package): This is now a shell method that determines the
#      package type based on the file extension, and routes the
#      parameters off the the appropriate unpack method (_unpack_VDB
#      or _unpack_fatalbert as appropriate)
#
#  (migrate_package): This now derives the package type internally by
#      parsing the extension off the end of the $source_pkg parameter.
#       
#  (_verify_unpack): Instead of merely checking for ZDONE.DAT, this
#      method now tests for the existence of nearly a dozen files to
#      verify that a definition package has been successfully
#      unwrapped.
#
#  (_unpack_fatalbert): Broke the handling of fatalbert packages out
#      from unpack_package into this method.

#  (_unpack_VDB): Added unpack handler for VDB files (shells out to unzip)


############################################################################

# I haven't gotten the DataFlow.pm to work yet.  It seems to depend on
# modules that aren't in the source tree.  Nit.

use DataFlow;
use Win32::Process;
use File::Basename;
use Getopt::Long;

#-------------------------------------------------------------------
# Export options
#-------------------------------------------------------------------
# The only exports I have from this module are the methods
# themselves.  I don't believe in exporting data directly.  Ever.
# Really.
# I mean never.
#-------------------------------------------------------------------
use Exporter ();
@ISA = qw(Exporter);
@EXPORT_OK = qw( unpack_package dataflow_completion retry_on_error load_profile);
#-------------------------------------------------------------------

#-------------------------------------------------------------------
# I'm cheating with "use strict"  It doesn't seem to the "@ISA" line
# above.  Rather than fix it, I'm using a WORKAROUND ;)
#
# take "use strict;" out and die a horrible death.
#
#-------------------------------------------------------------------
use strict;
#-------------------------------------------------------------------

#-------------------------------------------------------------------
# Internal state vars (minimal)
#-------------------------------------------------------------------

# Internal profile information (not exported directly)
my %profile = ();

#-------------------------------------------------------------------

#-------------------------------------------------------------------
# sub parse_parms
#-------------------------------------------------------------------
# Takes the @ARGV list directly, and parses that scary input argument
# format into a hash table, a reference to which is returned.  Some
# innovative testor may use this to hack together his or her own
# simulation.  But I would certainly never consider subverting the
# system.
#-------------------------------------------------------------------
sub parse_parms
  {
    my @args = @_;
    my $parameter = "";
    my %parm_hash = ();
    #    foreach $parameter(split /\-\-/,(join " ",@args))
    #    {
    #        my ($key,$value) = split /\s/,$parameter,2;
    #   $parm_hash{$key} = $value;
    #    }
    my ($Cookie, $ProcessID, $Parameter, $unc);
    GetOptions("Cookie=i" => \$Cookie,
	       "ProcessId=i" => \$ProcessID,
	       "Parameter=s" => \$Parameter,
	       "UNC=s" => \$unc);
    %parm_hash = ('UNC',$unc,'Parameter',$Parameter,'ProcessId',$ProcessID,'Cookie',$Cookie);
    return \%parm_hash;
  }
#-------------------------------------------------------------------


#-------------------------------------------------------------------
# sub dataflow_completion
#-------------------------------------------------------------------
# There are a couple methods in the DataFlow package that need to be
# called on every child applet exit.  For the world I don't understand
# why they are separate functions.  I've decided to be wiser about
# choosing my battles, and instead wrapped them in this method.
#-------------------------------------------------------------------

sub dataflow_completion
  {
    my ($cookie,
	$process_id,
	$unc,
	$next_service,
	$export_parameter,
	$stopping) = @_;
    
    DataFlow::DoNext($cookie,
		     $process_id,
		     $unc,
		     $next_service,
		     $export_parameter);
    
    DataFlow::SpecifyStopping($cookie,
			      $process_id,
			      $unc,
			      $next_service,
			      $stopping);
    
    DataFlow::WriteSignatureForDataFlow($process_id,$unc);
    #-------------------------------------------------------------------
    
  }

#-------------------------------------------------------------------
# sub _verify_unpack
#-------------------------------------------------------------------
#
# This method, when given a path, checks to see if a full definition
# package has been successfully expanded there. (by testing for the
# existence of a few files.)
#-------------------------------------------------------------------
#
sub _verify_unpack
  {
    my ($pkg_path) = @_;
    my $status = $SDGS::SDGS_ERROR;

    if ((-e "$pkg_path\\zdone.dat") &&
	(-e "$pkg_path\\naveng32.dll") &&
	(-e "$pkg_path\\navex32a.dll") &&
	(-e "$pkg_path\\virscan1.dat") &&
	(-e "$pkg_path\\virscan2.dat") &&
	(-e "$pkg_path\\virscan3.dat") &&
	(-e "$pkg_path\\virscan4.dat") &&
	(-e "$pkg_path\\virscan5.dat") &&
	(-e "$pkg_path\\virscan6.dat") &&
	(-e "$pkg_path\\virscan7.dat") &&
	(-e "$pkg_path\\virscan8.dat") &&
	(-e "$pkg_path\\ZDONE.dat"))
      {
	$status  = $SDGS::SDGS_OK;
      }
    return $status;
  }

#-------------------------------------------------------------------
# sub _unpack_VDB
# -------------------------------------------------------------------
# Unpacks a VDB package, first into the temp directory, then moves
# that package to the final destination
# -------------------------------------------------------------------

sub _unpack_VDB
  {
    my ($filename,$destination) = @_;
    my $status = $SDGS::SDGS_ERROR;

    # Confirm the target directory exists and is clear
    # Confirm package exists
    if ((-d $destination) &&
	(-f $filename))
      {
        ##############################################################
        #
        # It's worth noting here that the directory specified for the
        # "unzip" executable (resolved with "$DataFlow::toolsBaseDir"
        # APPEARS be defined in dataflow.pm.  This is not the case.
        # toolsBaseDir is resolved two namespaces deeper...
        # .
        # DataFlow.pm uses FSTools.pm 
        # FSTools.pm uses AVIS::Local
        # AVIS::Local defines toolsBaseDir
        # .  
        # Because of the way it is exported through the EXPORT section
        # of the modules, it is visible from the DataFlow namespace
        # (and probably through the default namespace.)
        #
        # The "right thing to do" is probably to include AVIS::Local
        # in this module and reference toolsBaseDir through it.
        # However, in the interest of code-freeze, I am merely
        # documenting it.
        #
        ##############################################################

	# Unzip the package to the target
	# Command line	
        my $command_line = "$DataFlow::toolsBaseDir\\unzip -o $filename -d $destination";
        system $command_line;

	# Copy files from wild subdirectory
	system ("copy $destination\\wild\\virscan1.dat $destination\\virwild1.dat");
	system ("copy $destination\\wild\\virscan2.dat $destination\\virwild2.dat");
	system ("copy $destination\\wild\\virscan3.dat $destination\\virwild3.dat");
	system ("copy $destination\\wild\\virscan4.dat $destination\\virwild4.dat");
	system ("copy $destination\\wild\\*.exp $destination\\*.wld");
	system ("rmdir $destination\\wild /s /q");
	# Verify operation
	$status = _verify_unpack($destination);
      }
    
    return $status;
  }
# -------------------------------------------------------------------

# -------------------------------------------------------------------
# sub _unpack_fatalbert
# -------------------------------------------------------------------
# Extremely Yuckie.
#
# Given a package filename, this method unpacks it to a HARDCODED
# TEMPORARY DIRECTORY by spawning an external process.
# 
# -------------------------------------------------------------------

sub _unpack_fatalbert
  {
    my ($filename,$destination) = @_;
    # build Command line
    my $status = $SDGS::SDGS_ERROR;
    mkdir ($destination,777) unless -d $destination;
    if (-d $destination)
      {
	my $command_line = "$filename /QUIET " .
	  "/EXTRACT $destination";
	# Flush temp & destination
	#        unlink <"$destination\\*.*">;
	# unlink *.* on unc path seems to have a problem.
	# Hence we decided to use rmdir until someone finds a way to use unlink safely.
	system ("rmdir $destination /s /q");
	sleep (7);
	mkdir "$destination",777;
	# Create process
	my $process_obj = undef;
	Win32::Process::Create($process_obj,
			       "$filename",
			       $command_line,
			       0,
			       CREATE_SEPARATE_WOW_VDM,
			       ".");
	# Wait on process
	$process_obj->Wait(INFINITE);
	# Verify operation
	$status = _verify_unpack($destination);
	# Set Status
      }
    return $status;

  }

# -------------------------------------------------------------------
# sub unpack_package
# -------------------------------------------------------------------
#
# This will make a determination of the package type basedon the file
# extension.  It will then use that information to determine the
# appropriate unpack subroutine (either _unpack_fatalbert or
# _unpack_VDB 
#
# -------------------------------------------------------------------
sub unpack_package
  {
    
    my ($filename,$destination) = @_;
    my $status = $SDGS::SDGS_ERROR;
    if ($filename =~ /\.vdb$/i)
      {
	# This is a VDB package, and ought be treated accordingly
	$status = _unpack_VDB($filename,$destination);        
      }
    else
      {
	# Package Type is FatAlbert
	$status = _unpack_fatalbert($filename,$destination);
      }
    return $status;
  }


#-------------------------------------------------------------------
# sub migrate_package
#-------------------------------------------------------------------
# Yuckie.
#
# Given the name of a 'temporary' package file (that has been expanded
# into it's local path) this method copies the whole schmegeggie to
# the parametrized destination.
#-------------------------------------------------------------------

sub migrate_package
  {
    my ($source_pkg,$destination) = @_;

    # Use the extension of the $source_pkg filename parameter to
    # determine whether we are working with a VDB package or a
    # FatAlbert package... (for use in building filenames later on)

    my $package_extension = "exe";
    # Is this a VDB package or a FatAlbert package?
    if ($source_pkg =~ /\.vdb$/i)
      {
	$package_extension = "vdb";
      }

    my $status = $SDGS::SDGS_ERROR;
    File::Basename::fileparse_set_fstype("MSDOS");
    my ($srcdir)       = dirname  (lc($source_pkg));
    my ($filebase)     = basename (lc($source_pkg));
    my ($destdir)      = dirname  (lc($destination));
    my ($destfilebase) = basename (lc($destination),".$package_extension");
    my ($srcfilebase)  = basename (lc($source_pkg),".$package_extension");
    
    if (-d "$destdir\\$destfilebase")
      {
	#       unlink <"$destdir\\$destfilebase\\*.*">;
	# unlink *.* on unc path seems to have a problem.
	# Hence we decided to use rmdir until someone finds a way to use unlink safely.
	system ("rmdir $destdir\\$destfilebase /s /q");
	sleep (7);
	mkdir "$destdir\\$destfilebase",777;
      } 
    else
      {
	mkdir "$destdir\\$destfilebase",777;
      }
    system "copy $source_pkg $destination";
    system "copy $srcdir\\$srcfilebase\\*.* $destdir\\$destfilebase";
    $status = _verify_unpack("$destdir\\$destfilebase");
    return ($status,"$destdir\\$destfilebase");
  }

#-------------------------------------------------------------------
# sub retry_on_error
#-------------------------------------------------------------------
# Very simple.  returns the "retry_on_error" flag.  The return should
# be interpreted in a boolean context.
#-------------------------------------------------------------------
sub retry_on_error
  {
    return ($profile{"retry_on_error"});
  }
#-------------------------------------------------------------------

#-------------------------------------------------------------------
# sub load_profile
#-------------------------------------------------------------------
# Loads a config file into the internal %profile hash.  The format of
# the file is as follows:
# Individual name value pairs are formatted "name: value"
# There can be any number of empty lines or perl-style comment lines
#-------------------------------------------------------------------

sub load_profile
  {
    my ($filename) = @_;
    open PROFILE,"<$filename";
    foreach my $line(<PROFILE>)
      {
	if ($line !~ /^\s*\#/)
	  {
	    chomp $line;
	    my ($key,$value) = split /\:\s/,$line;
	    $profile{$key} = $value;
	}
    }
    close PROFILE;
  }
#-------------------------------------------------------------------

BEGIN
  {
    # Do something intensely meaningful
  }

1;

