###########################################################################
# THROW AWAY THIS CODE AFTER THE PILOT IS OVER.
#
# SDGS1.pm
# This perl module provides functions for getting the
# newest unblessed and blessed definition packages from the 
# back office. These functions are scaffolded functions that
# will be used ONLY during the Immune System pilot. These
# functions are also available in the SDGS.pm which was 
# developed by Symantec. Since there will not be any real
# back office in the pilot, we decided to scaffold these 2
# functions in this perl module SDGS1.pm. After the pilot is
# over and when the Immune System moves over to Santa Monica, CA,
# this SDGS1.pm can be thrown away. At that time the 2 original
# functions available in the SDGS.pm will be used. Please remember
# to change all occurrances of "SDGS1" to "SDGS" in the file
# "GetDefinitions.pl" at that time.
#
#
# Authors: Michael Wilson, Senthil Velayudham, Srikant Jalan
# Last Modified: Sep/07/1999
#
# The specification for SDGS is available in a detailed
# document titled "Definition Generation Service API specifications".
###########################################################################

	package SDGS1;
	require Exporter;
	@ISA = qw (Exporter);
	@EXPORT = qw (GetNewestPackage GetBlessedPackage);

    use AVIS::Local;
	use md5;

	# Define all the global constants used in this program.
	$SM_DEBUG = 1;
	$SDGS_OK = 0;
	$SDGS_ERROR = 1;
	$SDGS_FAIL = 2;
	$SDGS_IN_PROGRESS = 3;

    my $defImporterBaseDir = "$isdnServerRoot\\BackOfficeDefinitionImporter";

########################################################################
# This subroutine gets the newest unblessed package from the back office.
# This is typically a definition package built in SARC at Santa Monica.
#
# Function arguments: 
#
# 1) Definition package type.
# 2) Filename in which the newest package has to be copied.
# 3) Current def sequence number that is used in the analysis center.
#
# This function copies the newest definition package if one is available
# into the given filename and returns the newest sequence number.
########################################################################
sub GetNewestPackage ($$$)
{
    # Parse the function arguments.
    my ($packageType, $packageFileName, $olderSequence) = @_;
    # Define some local varaibles.
    my $newestSequenceNumberKey = "NewestSequenceNumber = ";
    my ($newestSequenceNumber);  

    # Initialize the sequence number value to 0.
    $newestSequenceNumber = 0;

    # The availability of a new definition package will be informed
    # using a package file.
    $fileName = "$defImporterBaseDir\\DefImporter.pkg";

    # If the package file is not there, just return.
    if (!(-e $fileName))
    {
        return ($SDGS_FAIL, 0);
    } 

    # Read the package filename.
    print ("$fileName exists\n");
    my (@fileContents, $line);
    open (FILEHANDLE, "<$fileName");
    @fileContents = <FILEHANDLE>;
    close (FILEHANDLE);
   
    # Parse the package filename contents.
    foreach $line (@fileContents)
    {
        # Remove all the leading and trailing white spaces.
        $line =~ s/^\s*(.*)\s*$/$1/;

        # Parse the Key:Value pair in the file contents and check if we have the correct key.
        if (substr($line, 0, length($newestSequenceNumberKey )) eq $newestSequenceNumberKey )
        {
           # Parse the sequence number specified in the package file.
           $newestSequenceNumber = substr($line, length($newestSequenceNumberKey ));
        }

        # If the sequence number is non-zero, exit out of this loop.
        if ($newestSequenceNumber != 0)
        {
            last;
        }
    }


    # If the newest sequence number from the back office is older than the one already in
    # the analysis center, we don't have to import it. Just ignore that package.
    if ($newestSequenceNumber <= $olderSequence)
    {
        return ($SDGS_FAIL, 0);
    }

    # We got a newer sequence number from the back office. Let us import it.
    my $tempString = sprintf ("%08d", $newestSequenceNumber);
    # Form a 8 digit definition sequence number.
    my $defFileName = "$defImporterBaseDir\\$tempString.exe";

    # Check if the user has put the new definition package in the proper directory.
    if (!(-e $defFileName))
    {
        # If the newer def package from the back office is not available, return now.
        return ($SDGS_FAIL, 0);
    }

    # Compute the MD5 checksum of the new definition package.
    my $checksum1 = ComputeMD5Checksum ($defFileName);
    # Copy the new definition package to the analysis center file server.
    system ("copy $defFileName $packageFileName");
    # Verify if the copy operation was successful by comparing the checksum
    my $checksum2 = ComputeMD5Checksum ($packageFileName);
    
    if ($checksum1 ne $checksum2)
    {
       # File copy was not successful.
       return ($SDGS_FAIL, 0);
    }  
    else
    {
      # The newer def package from the back office has been copied to the
      # analysis center file server. Return to the caller with the new sequence number.
      return ($SDGS_OK, $newestSequenceNumber);
    }
}


########################################################################
# This subroutine gets the blessed package from the back office.
# This is typically a definition package blessed in SARC at Santa Monica.
#
# Function arguments: 
#
# 1) Definition package type.
# 2) Filename in which the blessed package has to be copied.
# 3) Current blessed def sequence number that is used in the analysis center.
#
# This function copies the blessed definition package if one is available
# into the given filename and returns the newest sequence number.
########################################################################
sub GetBlessedPackage ($$$)
{
    # Parse the function arguments.
    my ($packageType, $packageFileName, $olderSequence) = @_;
    # Define some local varaibles.
    my $blessedSequenceNumberKey = "BlessedSequenceNumber = ";
    my ($blessedSequenceNumber);  

    # Initialize the sequence number value to 0.
    $blessedSequenceNumber = 0;

    # The availability of a new blessed definition package will be informed
    # using a package file.
    $fileName = "$defImporterBaseDir\\DefImporter.pkg";

    # If the package file is not there, just return.
    if (!(-e $fileName))
    {
        return ($SDGS_FAIL, 0);
    } 

    # Read the package filename.
    print ("$fileName exists\n");
    my (@fileContents, $line);
    open (FILEHANDLE, "<$fileName");
    @fileContents = <FILEHANDLE>;
    close (FILEHANDLE);

    # Parse the package filename contents.   
    foreach $line (@fileContents)
    {
        # Remove all the leading and trailing white spaces.
        $line =~ s/^\s*(.*)\s*$/$1/;

        # Parse the Key:Value pair in the file contents and check if we have the correct key.
        if (substr($line, 0, length($blessedSequenceNumberKey)) eq $blessedSequenceNumberKey)
        {
           # Parse the sequence number specified in the package file.
           $blessedSequenceNumber = substr($line, length($blessedSequenceNumberKey));
        }

        # If the sequence number is non-zero, exit out of this loop.
        if ($blessedSequenceNumber != 0)
        {
            last;
        }
    }

    # If the blessed sequence number from the back office is older than the one already in
    # the analysis center, we don't have to import it. Just ignore that package.
    if ($blessedSequenceNumber <= $olderSequence)
    {
        return ($SDGS_FAIL, 0);
    }

    # We got a newer blessed sequence number from the back office. Let us import it.
    my $tempString = sprintf ("%08d", $blessedSequenceNumber);
    # Form a 8 digit definition sequence number.
    my $defFileName = "$defImporterBaseDir\\$tempString.exe";

    # Check if the user has put the new definition package in the proper directory.
    if (!(-e $defFileName))
    {
        # If the newer blessed def package from the back office is not available, return now.
        return ($SDGS_FAIL, 0);
    }

    # Compute the MD5 checksum of the new definition package.
    my $checksum1 = ComputeMD5Checksum ($defFileName);
    # Copy the new definition package to the analysis center file server.
    system ("copy $defFileName $packageFileName");
    # Verify if the copy operation was successful by comparing the checksum.
    my $checksum2 = ComputeMD5Checksum ($packageFileName);
    
    if ($checksum1 ne $checksum2)
    {
       # File copy was not successful.
       return ($SDGS_FAIL, 0);
    }  
    else
    {
      # The newer def package from the back office has been copied to the
      # analysis center file server. Return to the caller with the new sequence number.
      return ($SDGS_OK, $blessedSequenceNumber);
    }
}

########################################################################
# This subroutine computes the MD5 checksum of a given file.
#
#
# Function arguments: 
#
# 1) Name of the file for which the MD5 checksum has to be computed.
#
# This function computes the MD5 checksum and returns it.
########################################################################
sub ComputeMD5Checksum ($)
{
    # Parse the function argument. 
    my ($fileName) = @_;

    # If the file can't be opened, return now.
    unless (open FILEHANDLE, $fileName)
    {
        return ("WrongChecksum");
    }

    # Set binary mode for I/O operations.
    binmode (FILEHANDLE);
    # Seek to the beginning of the file.
    seek(FILEHANDLE, 0, 0);
    # Compute the MD5 checksum.
    my $md5 = new MD5;
    $md5->reset;
    $md5->add(<FILEHANDLE>);
    my $d = $md5->hexdigest;
    print "All lines at once = $d\n";

    # Close the file and return the computed checksum value. 
    close(FILEHANDLE);
    return ($d);

}
