#!/usr/bin/perl -w

############################################################################
# File: sdgs.pm
# Owner: Formy
# Last modification date: 12/6/1999
# Description:
#    This module implements the API for the Symantec
#    Sample Deferral Service as defined by IBM.
###########################################################################

use LWP::UserAgent;
use HTTP::Status; 
use HTTP::Headers; 
use HTTP::Request::Common;

package SSDS;

my $ua = LWP::UserAgent->new;

$SD_DEBUG = 1;

#
#IBM Change the IP in this string!!!
#
#$CMURL = "http://155.64.154.16/cgi-bin/converter.pl";
#$IRURL = "http://155.64.154.16/cgi-bin/iresolver.pl";
$CMURL = "http://9.2.70.30/cgi-bin/converter.pl";
$IRURL = "http://9.2.70.30/cgi-bin/iresolver.pl";

$| = 1;

$SSDS_OK = 0;
$SSDS_ERROR = 1;
$SSDS_FAIL = 2;

$CM_OK = 0;
$CM_ERROR = 1;

$IR_OK = 0;
$IR_ERROR = 1;
$IR_FAIL = 2;

# Issue Status values
$NEW_VIRUS           = 0x80;
$MONTHLY             = 0x40;
$CURRENT             = 0x20;
$NO_ACTION           = 0x10;
$NO_VIRUS            = 0x08;
$FALSEID_COMP        = 0x04;
$FALSEID_SYM         = 0x02;
$NOT_PROCESSED       = 0x01;


##############################################################################
# Function: DeferSample
# Arguments: $issueTrackingNumber - AVIS cookie number
#            $sampleFilename - path to the file containing AVIS information
# Returns: $SSDS_OK - if success
#          $SSDS_ERROR - if error
#
# Description:  This function sends a POST command to the Converter Module
#               to Defer the sample.
##############################################################################
sub DeferSample {
    my $issueTrackingNumber = shift || return $SSDS_ERROR;
    my $sampleFilename = shift || return $SSDS_ERROR;
    my $processingRequired = 0;

    unless (-e $sampleFilename) {
        print "[ssds.pm] $sampleFilename does not exist\n" if $SD_DEBUG;
        reutrn $SSDS_ERROR;
    }

    if ($sampleFilename =~ /deferred/) {
        $processingRequired = 1;
    }

    my $filesize = (stat $sampleFilename)[7];

    print "[ssds.pm] Sending request $CMURL\n" if $SD_DEBUG;

    # Submit the zip file
    my $response = $ua->request(main::POST "$CMURL", 
                                Content_Type => 'form-data',
                                Content => [ Cookie => $issueTrackingNumber,
                                             ProcReq => $processingRequired,
                                             filesize => $filesize,
                                             AVISPack => ["$sampleFilename","AVISPack"], ]);

    return $SSDS_ERROR unless ($response->is_success);
    print "[ssds.pm] Response content from $CMURL\n",$response->content,"\n";
    return $SSDS_ERROR if ($response->header('X-cmstatus') == $CM_ERROR);

    unless ($response->header('X-cmstatus') == $CM_OK && defined $response->header('X-cmstatus'))
    {
        print "[ssds.pm] CM_Status unknown\n" if $SD_DEBUG; 
        return $SSDS_ERROR;
    }

    print "[ssds.pm] DeferSample succeeded\n" if $SD_DEBUG;
    return $SSDS_OK;
}


##############################################################################
# Function: GetNextAnalyzedSample
# Arguments: @deferredIssues - a list of tracking numbers of deferred issues
# Returns: A list of 4 values:
#          $status - the result of the operation
#            $SSDS_OK - if success
#            $SSDS_FAIL - if no results are available
#            $SSDS_ERROR - if error
#          $issueTrackingNumber
#            The tracking number if $status = $SSDS_OK
#            NULL otherwise
#          $finalState - The manula analysis result
#            INFECTED, UNINFECTED, MISFIRED
#          $sequenceNumber
#            The sequence number of the build needed to resolve the issue
#
# Description:  This function sends a POST command to the Issues Resolver
#               to get the status of previously deferred issues.
##############################################################################
sub GetNextAnalyzedSample {
    my @deferredIssues = @_;
    my ($issueTrackingNumber, $issueStatus, $finalState, $sequenceNumber);

    unless (@deferredIssues) {
        print "[ssds.pm] Argument list is empty\n" if $SD_DEBUG;
        return ($SSDS_FAIL,'','','');
    }

    foreach $issueTrackingNumber (@deferredIssues) {
        print "[ssds.pm] Sending request $IRURL for issue $issueTrackingNumber\n" if $SD_DEBUG;

        my $response = $ua->request(main::POST "$IRURL", [ Cookie => $issueTrackingNumber ]);

        return ($SSDS_ERROR,'','','') unless ($response->is_success);
        return ($SSDS_ERROR,'','','') if ($response->header('X-irstatus') == $IR_ERROR);
        next if ($response->header('X-irstatus') == $IR_FAIL);

        unless ($response->header('X-irstatus') == $IR_OK && defined $response->header('X-irstatus'))
        {
            print "[ssds.pm] IR_Status unknown\n" if $SD_DEBUG; 
            return ($SSDS_ERROR,'','','');
        }

        # We got the results back for this issue
        $issueStatus = $response->header('X-issuestatus');
        $sequenceNumber = $response->header('X-sequence');

        # Convert the #issueStatus to $finalState
        if ($issueStatus == $NEW_VIRUS || $issueStatus == $MONTHLY || $issueStatus == $CURRENT) {
            $finalState = "infected";
        } elsif ($issueStatus == $NO_ACTION || $issueStatus == $NO_VIRUS || $issueStatus == $FALSEID_COMP) {
            $finalState = "uninfected";
            $sequenceNumber = 0;
        } elsif ($issueStatus == $FALSEID_SYM) {
            $finalState = "misfired";
        } elsif ($issueStatus == $NOT_PROCESSED) {
            print "[ssds.pm] No results for issue $issueTrackingNumber\n" if $SD_DEBUG; 
            next;
        } else {
            print "[ssds.pm] Final State unknown\n" if $SD_DEBUG; 
            return ($SSDS_ERROR,$issueTrackingNumber,'unknown',0);
        }

        print "[ssds.pm] Returning final result for issue $issueTrackingNumber\n" if $SD_DEBUG;
        return ($SSDS_OK, $issueTrackingNumber, $finalState, $sequenceNumber);
         
    }

    # No results were found
    print "[ssds.pm] No results available\n" if $SD_DEBUG;
    return ($SSDS_FAIL,'','','');
}


