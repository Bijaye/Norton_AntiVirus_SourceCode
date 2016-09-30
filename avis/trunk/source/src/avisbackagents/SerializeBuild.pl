#!c:/perl/bin/perl

use strict;
use SDGS;
use BOSupport;

my $retry_cycle = 5; # seconds

# Parse ARGV

my $parm_hash = BOSupport::parse_parms(@ARGV);

# Formal Input parameter declarations

my $import_unc        = $parm_hash->{'UNC'};
my $import_parameter  = $parm_hash->{'Parameter'};
my $import_cookie     = $parm_hash->{'Cookie'};
my $import_process_id = $parm_hash->{'ProcessId'};

# Export Parameter declarations (with null initialization)
my $next_service = "";
my $export_parameter = "";

############################################################
# Local Logic
############################################################
my $lock_status = $SDGS::SDGS_IN_PROGRESS;

while ($lock_status == $SDGS::SDGS_IN_PROGRESS)   
{
    $lock_status = SDGS::Lock();
    if ($lock_status == $SDGS::SDGS_IN_PROGRESS)
    {
        sleep($retry_cycle);
    }
}

if ($lock_status == $SDGS::SDGS_OK)
{
    $next_service = 'IncrementalDefBuilder';
    $export_parameter    = 'Incremental';
}
elsif ($lock_status == $SDGS::SDGS_ERROR)
{
    if (BOSupport::retry_on_error())
    {
        $next_service = 'SerializeBuild';
        $export_parameter = '';
    }
    else
    {
        $next_service = 'CriticalError';
        $export_parameter = '';
    }
}

BOSupport::dataflow_completion($import_cookie,
                               $import_process_id,
                               $import_unc,
                               $next_service,
                               $export_parameter,
                               "");
