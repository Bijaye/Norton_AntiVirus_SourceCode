#!/usr/bin/perl -w

############################################################################
# File: sdgs.pm
# Owner: Chester/Formy
# Last modification date: 5/26/1999
# Description:
#    This script is the submission module as described by the Symantec
#    Definition Generation Service document provided by IBM.
###########################################################################

use LWP::UserAgent;
use HTTP::Status; 
use HTTP::Headers; 
use HTTP::Request::Common;

package SDGS;

my $ua = LWP::UserAgent->new;

$SM_DEBUG = 1;

#
#IBM Change the IP in this string!!!
#
$IMURL = "http://9.2.70.30/cgi-bin/IM.pl";

$| = 1;

$SDGS_OK = 0;
$SDGS_ERROR = 1;
$SDGS_FAIL = 2;
$SDGS_IN_PROGRESS = 3;

$IM_OK = 0;
$IM_ERROR = 1;
$IM_FAIL = 2;
$IM_INPROGRESS = 3;

##############################################################################
# Function: Lock
# Arguments: None
# Returns: $SDGS_OK - if success
#          $SDGS_IN_PROGRESS - if in progress
#          $SDGS_ERROR - if error
#
# Description:  This function sends a POST command to the Integration Module
#               to lock.  One must always sendlock before attempting to POST
#               anything else to the Integration Module
##############################################################################
sub Lock {
    print "[sdgs.pm] Sending request $IMRUL function 'Lock'\n" if $SM_DEBUG;
    my $response = $ua->request(main::POST "$IMURL", [ function => 'Lock' ]);

    return $SDGS_ERROR unless ($response->is_success);

    print "[sdgs.pm] Lock page fetch was successful\n" if $SM_DEBUG;
    if ($response->header('X-imstatus') == $IM_ERROR) { 
        print "[sdgs.pm] Lock IM_Status returned ERROR\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    if ($response->header('X-imstatus') == $IM_INPROGRESS) {
        print "[sdgs.pm] Lock IM_Status returned IN PROGRESS\n" if $SM_DEBUG;
        return $SDGS_IN_PROGRESS;
    }
    unless ($response->header('X-imstatus') == $IM_OK &&
            defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] Lock IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] Lock succeeded\n" if $SM_DEBUG;
    print "[sdgs.pm] Lock function returning: $SM_OK\n" if $SM_DEBUG;
    return $SDGS_OK;
}


##############################################################################
# Function: StartIncrementalDataBuild() 
# Arguments: IDSP
# Returns: $SDGS_IN_PROGRESS - if success
#          $SDGS_FAIL  -  if IDSP is invalid
#          $SDGS_ERROR - all other errors
#
#          Also returns $previousSequence number
#
# Description:  This function starts an incremental data build with the
#               given IDSP
##############################################################################

sub StartIncrementalDataBuild {
    my $source = shift || return $SDGS_ERROR;

    # Get Old Sequence Number
    print "[sdgs.pm] Sending request $IMRUL function 'GetSequenceNumber'\n" if $SM_DEBUG;


    my $response = $ua->request(main::POST "$IMURL", [ function => 'GetSequenceNumber' ]);

    return $SDGS_ERROR, unless ($response->is_success);
    if ($response->header('X-imstatus') == $IM_ERROR) { 
        print "[sdgs.pm] GetSequenceNumber IM_Status returned ERROR\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    unless ($response->header('X-imstatus') == $IM_OK &&
            defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] GetSequenceNumber IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    my $previousSequence = $response->header('X-sequencenum');
    print "[sdgs.pm] GetSequenceNumber succeeded: $previousSequence\n" if $SM_DEBUG;

    # MakeLocalCopy
    print "[sm.pl $$] In MakeLocalCopy\n" if $SM_DEBUG;
    print "[sm.pl $$] Sending request $IMRUL function 'MakeLocalCopy'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL", [ function => 'MakeLocalCopy']);
    return $SDGS_ERROR unless ($response->is_success);
    print "[sm.pl $$] MakeLocalCopy page fetch was successful\n" if $SM_DEBUG;
    return $SDGS_ERROR if ($response->header('X-imstatus') == $IM_ERROR);
    unless ($response->header('X-imstatus')== $IM_OK &&
            defined $response->header('X-imstatus'))
    {
        print "[sm.pl $$] MakeLocalCopy IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sm.pl $$] MakeLocalCopy succeeded\n" if $SM_DEBUG;

    # Submit new IDSP

    print "[sdgs.pm] In SubmitDefintions\n" if $SM_DEBUG;
    print "[sdgs.pm] Sending request $IMRUL function 'SubmitDefinition'\n" if $SM_DEBUG;

    $response = $ua->request(main::POST "$IMURL", 
                             Content_Type => 'form-data',
                             Content => [ function => 'SubmitDefinition',
                                          IDSP => ["$source","IDSP"], ]);

    return $SDGS_ERROR unless ($response->is_success);
    return $SDGS_ERROR if ($response->header('X-imstatus') == $IM_ERROR);
    return $SDGS_FAIL if ($response->header('X-imstatus') == $IM_FAIL);
    unless ($response->header('X-imstatus') == $IM_OK && defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] SubmitDefinitions IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] SubmitDefinitions succeeded\n" if $SM_DEBUG;

    # Build individual def
    print "[sdgs.pm] In BuildIndividualDef\n" if $SM_DEBUG;
    print "[sdgs.pm] Sending request $IMRUL function 'BuildIndividualDef'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL",
                             [ function => 'BuildLocalCopy', 
                               query => 'false',
                               buildtype => 'incremental' ]);
    return $SDGS_ERROR unless ($response->is_success);
    print "[sdgs.pm] BuildIndividualDef page fetch was successful\n" if $SM_DEBUG;
    return $SDGS_ERROR if ($response->header('X-imstatus') == $IM_ERROR);
    return $SDGS_FAIL if ($response->header('X-imstatus') == $IM_FAIL);
    unless ($response->header('X-imstatus') == $IM_INPROGRESS && defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] BuildIndividualDef IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] BuildLocalCopy succeeded\n" if $SM_DEBUG;
    print "[sdgs.pm] StartIncrementalDataBuild returning: $SDGS_IN_PROGRESS\n" if $SM_DEBUG;

    return ($SDGS_IN_PROGRESS, $previousSequence);
}

##############################################################################
# Function: IncrementalBuildStatus() 
# Arguments: None
# Returns: $SDGS_OK - if build is done
#          $SDGS_IN_PROGRESS - if build is still going
#          $SDGS_FAIL - if build failed
#          $SDGS_ERROR - all other errors
#
# Description:  This function sends a POST command to the Integration Module
#               to determine if IncrementalDataBuild has completed.
##############################################################################
sub IncrementalDataBuildStatus {
    print "[sdgs.pm] In IncrementalBuildStatus\n" if $SM_DEBUG;
    print "[sdgs.pm] Sending request $IMRUL function 'BuildIndividualDef'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL", 
                             [ function => 'BuildLocalCopy',
                               query => 'true',
                               buildtype => 'incremental' ]);
    return $SDGS_ERROR unless ($response->is_success);
    print "[sdgs.pm] BuildIndividualDef page fetch was successful\n" if $SM_DEBUG;
    return $SDGS_ERROR if ($response->header('X-imstatus') == $IM_ERROR);
    return $SDGS_IN_PROGRESS if ($response->header('X-imstatus') == $IM_INPROGRESS);
    return $SDGS_FAIL if ($response->header('X-imstatus') == $IM_FAIL);
    unless ($response->header('X-imstatus') == $IM_OK && defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] IncrementalBuildStatus IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] IncrementalBuild succeeded\n" if $SM_DEBUG;

    return $SDGS_OK;
}

##############################################################################
# Function: GetBuiltPackage() 
# Arguments: package type, package filename 
# Returns: $SDGS_OK - if success
#          $SDGS_ERROR - all other errors
#
# Description:  This function sends a POST command to the Integration Module
#               to request for the latest defintion files.
##############################################################################
sub GetBuiltPackage {
    my $packageType = shift || return $SDGS_ERROR;
    my $packageFilename = shift || return $SDGS_ERROR;

    print "[sdgs.pm] In GetBuiltPackage\n" if $SM_DEBUG;
    print "[sdgs.pm] Sending request $IMRUL function 'GetLocalVirusDataFiles'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL", [ function => 'GetLocalVirusDataFiles',
                                                    type     => 'built',
                                                    "package"  => $packageType ]);
     
    return $SDGS_ERROR unless ($response->is_success);
    print "[sdgs.pm] GetBuiltPackage page fetch was successful\n" if $SM_DEBUG;
    return $SDGS_ERROR if ($response->header('X-imstatus') == $IM_ERROR);
    unless ($response->header('X-imstatus') == $IM_OK && defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] GetBuiltPackage IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] GetLocalVirusDataFiles succeeded\n" if $SM_DEBUG;
    print "[sdgs.pm] GetLocalVirusDataFiles - Trying to open: $packageFilename\n" if $SM_DEBUG;
    open(DEFZIPFILE, ">$packageFilename") || return $SDGS_ERROR;
    binmode DEFZIPFILE;
    print (DEFZIPFILE $response->content) || return $SDGS_ERROR; 
    close(DEFZIPFILE) || return $SDGS_ERROR;
    print "[sdgs.pm] GetLocalVirusDataFiles - Created defs in $packageFilename\n" if $SM_DEBUG;
    print "[sdgs.pm] GetLocalVirusDataFiles returning: $SDGS_OK\n" if $SM_DEBUG;

    return $SDGS_OK;
}


##############################################################################
# Function: GetNewestPackage() 
# Arguments: package type, package filename, older sequence number 
# Returns: $SDGS_OK - if success
#           $SDGS_FAIL - no newer package exists
#          $SDGS_ERROR - all other errors
#
# Description:  This function sends a POST command to the Integration Module
#               to request for the latest defintion files newer than the 
#                sequence number provided.
##############################################################################
sub GetNewestPackage {
    my $packageType = shift || return $SDGS_ERROR;
    my $packageFilename = shift || return $SDGS_ERROR;
    my $previousSequence = shift || return $SDGS_ERROR;
    
    print "[sdgs.pm] In GetNewestPackage\n" if $SM_DEBUG;
    print "[sdgs.pm] Sending request $IMRUL function 'GetLocalVirusDataFiles'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL", [ function => 'GetLocalVirusDataFiles',
                                                    type     => 'newest',
                                                    sequencenumber => $previousSequence,
                                                    "package"  => $packageType ]);
    return $SDGS_ERROR unless ($response->is_success);
    my $newestSequence = $response->header('X-sequencenum');
    print "[sdgs.pm] GetNewestPackage page fetch was successful\n" if $SM_DEBUG;
    return $SDGS_ERROR if ($response->header('X-imstatus') == $IM_ERROR);
    return ($SDGS_FAIL, $newestSequence) if ($response->header('X-imstatus') == $IM_FAIL);
    unless ($response->header('X-imstatus') == $IM_OK && defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] GetNewestPackage IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] GetLocalVirusDataFiles succeeded\n" if $SM_DEBUG;
    print "[sdgs.pm] GetLocalVirusDataFiles - Trying to open: $packageFilename\n" if $SM_DEBUG;
    open(DEFZIPFILE, ">$packageFilename") || return $SDGS_ERROR;
    binmode DEFZIPFILE;
    print (DEFZIPFILE $response->content) || return $SDGS_ERROR; 
    close(DEFZIPFILE) || return $SDGS_ERROR;
    print "[sdgs.pm] GetLocalVirusDataFiles - Created defs in $packageFilename\n" if $SM_DEBUG;
    print "[sdgs.pm] GetLocalVirusDataFiles returning: $SDGS_OK\n" if $SM_DEBUG;

    return ($SDGS_OK, $newestSequence);
}

##############################################################################
# Function: GetBlessedPackage() 
# Arguments: package type, package filename, older sequence number 
# Returns: $SDGS_OK - if success
#          $SDGS_FAIL - no newer package exists
#          $SDGS_ERROR - all other errors
#
# Description:  This function sends a POST command to the Integration Module
#               to request for the latest defintion files newer than the 
#               sequence number provided.
##############################################################################
sub GetBlessedPackage {
    my $packageType = shift || return $SDGS_ERROR;
    my $packageFilename = shift || return $SDGS_ERROR;
    my $previousSequence = shift || return $SDGS_ERROR;
    
    print "[sdgs.pm] In GetBlessedPackage\n" if $SM_DEBUG;
    print "[sdgs.pm] Sending request $IMRUL function 'GetLocalVirusDataFiles'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL", [ function => 'GetLocalVirusDataFiles',
                                                    type     => 'blessed',
                                                    sequencenumber => $previousSequence,
                                                    "package"  => $packageType ]);
    return $SDGS_ERROR unless ($response->is_success);
    my $newestSequence = $response->header('X-sequencenum');
    print "[sdgs.pm] GetBlessedPackage page fetch was successful\n" if $SM_DEBUG;
    return $SDGS_ERROR if ($response->header('X-imstatus') == $IM_ERROR);
    return ($SDGS_FAIL, $newestSequence) if ($response->header('X-imstatus') == $IM_FAIL);
    unless ($response->header('X-imstatus') == $IM_OK && defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] GetBlessedPackage IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] GetLocalVirusDataFiles succeeded\n" if $SM_DEBUG;
    print "[sdgs.pm] GetLocalVirusDataFiles - Trying to open: $packageFilename\n" if $SM_DEBUG;
    open(DEFZIPFILE, ">$packageFilename") || return $SDGS_ERROR;
    binmode DEFZIPFILE;
    print (DEFZIPFILE $response->content) || return $SDGS_ERROR; 
    close(DEFZIPFILE) || return $SDGS_ERROR;
    print "[sdgs.pm] GetLocalVirusDataFiles - Created defs in $packageFilename\n" if $SM_DEBUG;
    print "[sdgs.pm] GetLocalVirusDataFiles returning: $SDGS_OK\n" if $SM_DEBUG;

    return ($SDGS_OK, $newestSequence);
}


##############################################################################
# Function: StartFullDataBuild() 
# Arguments: IDSP 
# Returns: $SDGS_IN_PROGRESS - if success
#          $SDGS_FAIL  -  if IDSP is invalid
#          $SDGS_ERROR - all other errors
#
#          Also returns $previousSequence number
#
# Description:  This function starts an incremental data build with the
#               given IDSP
##############################################################################

sub StartFullDataBuild {
    print "[sdgs.pm] Sending request $IMRUL function 'GetNextSequenceNumber'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL", [ function => 'GetNextSequenceNumber' ]);
    return $SDGS_ERROR unless ($response->is_success); 
    print "[sdgs.pm] GetNextSequenceNumber page fetch was successful\n" if $SM_DEBUG;
    return $SDGS_ERROR  if ($response->header('X-imstatus') == $IM_ERROR);
    unless ($response->header('X-imstatus') == $IM_OK &&
            defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] GetNextSequenceNumber IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR
        }
    my $newestSequence = $response->header('X-sequencenum');
    print "[sdgs.pm] GetNextSequenceNumber succeeded: $newestSequence\n" if $SM_DEBUG;


    print "[sdgs.pm] Sending request $IMRUL function 'GetNextDailyVersionNumber'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL", [ function => 'GetNextDailyVersionNumber' ]);
    return $SDGS_ERROR unless ($response->is_success); 
    print "[sdgs.pm] GetNextDailyVersionNumber page fetch was successful\n" if $SM_DEBUG;
    return $SDGS_ERROR  if ($response->header('X-imstatus') == $IM_ERROR);
    unless ($response->header('X-imstatus') == $IM_OK &&
            defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] GetNextDailyVersionNumber IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR
        }
    my $newestDVN = $response->header('X-dailyversionnum');
    print "[sdgs.pm] GetNextDailyVersionNumber succeeded: $newestDVN\n" if $SM_DEBUG;

    # Commit the def
    print "[sdgs.pm] In CommitLocalCopy\n" if $SM_DEBUG;
    print "[sdgs.pm] Sending request $IMRUL function 'CommitLocalCopy'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL", [ function => 'CommitLocalCopy' ]);
    return $SDGS_ERROR unless ($response->is_success);
    return $SDGS_ERROR if ($response->header('X-imstatus') == $IM_ERROR);
    unless ($response->header('X-imstatus') == $IM_OK && defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] CommitLocalCopy IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] CommitLocalCopy succeeded\n" if $SM_DEBUG;

    # Build full defs
    print "[sdgs.pm] In BuildLocalCopy\n" if $SM_DEBUG;
    print "[sdgs.pm] Sending request $IMRUL function 'BuildLocalCopy'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL",
                             [ function => 'BuildLocalCopy', 
                               query => 'false',
                               buildtype => 'full',
                               sequencenum => $newestSequence,
                               dailyversionnum => $newestDVN ]);
    return $SDGS_ERROR unless ($response->is_success);
    print "[sdgs.pm] BuildLocalCopy page fetch was successful\n" if $SM_DEBUG;
    return $SDGS_ERROR if ($response->header('X-imstatus') == $IM_ERROR);
    unless ($response->header('X-imstatus') == $IM_INPROGRESS && defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] BuildLocalCopy IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] StartFullDataBuild succeeded\n" if $SM_DEBUG;
    print "[sdgs.pm] StartFullDataBuild returning: $SDGS_IN_PROGRESS\n" if $SM_DEBUG;

    return ($SDGS_IN_PROGRESS, $newestSequence);
}

##############################################################################
# Function: FullBuildStatus() 
# Arguments: None
# Returns: $SDGS_OK - if build is done
#          $SDGS_IN_PROGRESS - if build is still going
#          $SDGS_FAIL - if build failed
#          $SDGS_ERROR - all other errors
#
# Description:  This function sends a POST command to the Integration Module
#               to determine if IncrementalDataBuild has completed.
##############################################################################
sub FullDataBuildStatus {
    print "[sdgs.pm] In FullBuildStatus\n" if $SM_DEBUG;
    print "[sdgs.pm] Sending request $IMRUL function 'BuildLocalCopy'\n" if $SM_DEBUG;
    $response = $ua->request(main::POST "$IMURL", 
                             [ function => 'BuildLocalCopy',
                               query => 'true',
                               buildtype => 'full' ]);
    return $SDGS_ERROR unless ($response->is_success);
    print "[sdgs.pm] BuildLocalCopy page fetch was successful\n" if $SM_DEBUG;
    return $SDGS_ERROR if ($response->header('X-imstatus') == $IM_ERROR);
    return $SDGS_IN_PROGRESS if ($response->header('X-imstatus') == $IM_INPROGRESS);

    #return $SDGS_FAIL if ($response->header('X-imstatus') == $IM_FAIL);
    ###Any build failure after committing should be treated as a critical error.

    unless ($response->header('X-imstatus') == $IM_OK && defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] BuildLocalCopy IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] FullDataBuild succeeded\n" if $SM_DEBUG;

    return $SDGS_OK;
}

##############################################################################
# Function: Unlock
# Arguments: None
# Returns: $SDGS_OK - if success
#          $SDGS_IN_PROGRESS - if in progress
#          $SDGS_ERROR - if error
#
# Description:  This function sends a POST command to the Integration Module
#               to unlock.  One must always unlock after finishing.
##############################################################################
sub Unlock {
    print "[sdgs.pm] Sending request $IMRUL function 'Unlock'\n" if $SM_DEBUG;
    my $response = $ua->request(main::POST "$IMURL", [ function => 'Unlock' ]);

    return $SDGS_ERROR unless ($response->is_success);

    print "[sdgs.pm] Unlock page fetch was successful\n" if $SM_DEBUG;
    if ($response->header('X-imstatus') == $IM_ERROR) { 
        print "[sdgs.pm] Unlock IM_Status returned ERROR\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    unless ($response->header('X-imstatus') == $IM_OK &&
            defined $response->header('X-imstatus'))
    {
        print "[sdgs.pm] Unlock IM_Status unknown\n" if $SM_DEBUG; 
        return $SDGS_ERROR;
    }
    print "[sdgs.pm] Unlock succeeded\n" if $SM_DEBUG;
    print "[sdgs.pm] Unlock function returning: $SM_OK\n" if $SM_DEBUG;
    return $SDGS_OK;
}


