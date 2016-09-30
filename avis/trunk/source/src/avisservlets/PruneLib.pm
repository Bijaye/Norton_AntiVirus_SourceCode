package PruneLib;

use Win32::ODBC;            # used extensively

use File::Basename;         # for the fileparse method used in
                            # load_profile() 

use File::Path;             # for rmtree method used in
                            # prune_signature 
require Exporter;

@ISA = qw (Exporter);
@EXPORT = qw (load_profile prune_logs prune_sigs prune_samples);

use strict;

my $DB_ERROR = undef;
my $DEBUG_MODE = 1;

# This populates the following values from a specified "avisservlets.prf" file
my $log_path = undef;
my $sig_root = undef;
my $sample_path = undef;


sub log_msg
  {
    my ($message) = @_;
    if ($DEBUG_MODE == 1)
      {
        print $message . "\n";
      }
  }
sub load_profile
  {
    my ($profile) = @_;
    my $success = undef;
    open INFILE,"<$profile";
    foreach my $line(<INFILE>)
      {
        chomp $line;
        $line = lc($line);
        if ($line =~ /signaturedirectory\s*=\s*(.*)$/)
          {
            $sig_root = $1;
          }
        elsif ($line =~ /sampledirectory\s*=\s*(.*)$/)
          {
            $sample_path = $1;
          }
        elsif ($line =~ /logfile\s*=\s*(.*)$/)
          {
            # this one isn't QUITE as simple.  It specifies the log
            # filename itself (before munging) we need to pull off 
            # the path from the front
            # This uses the File::Basename module
            my ($base,$dir,$ext) = File::Basename::fileparse($1);
            $log_path = $dir;
          }
      }
    close INFILE;
    
    # Now test that all three of those values were populated
    if ( ($log_path    !~ /^$/) &&
         ($sig_root    !~ /^$/) &&
         ($sample_path !~ /^$/) 
       )
      {
        $success = 1;
      }
    else
      {
        print STDERR "\nOne or more of the required directories is not\n";
        print STDERR "specified\n";
        print STDERR "sample directory: '$sample_path'\n";
        print STDERR "log path:         '$log_path'\n";
        print STDERR "signature root:   '$sig_root'\n";
        
      }
    return $success;
  }

# This is one of those fun little routines that's profoundly suboptimal
sub _n_days_ago_db2
  {
    my ($n) = @_;
    my $current_timestamp = time();
    my $correct_day = $current_timestamp - (3600 * 24 * $n);
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($correct_day);
    $year += 1900;
    $mon += 1;
    return "'$year-$mon-$mday-$hour.$min.$sec.0'";
  }

# Prune log files more than a certain number of days old.

sub prune_logs
  {
    my ($age) = @_;  
    # Prune all log files older than $age days The use of avisservlets
    # as a string constant here is extremely bad form.  However, in
    # the interest of time...
    foreach (glob "$log_path/*.*")
      {
        if (-M $_ > $age)
          {
            unlink;
          }
      }
  }

# Prune signatures that are
# a) older than the latest blessed signatures
# b) more than N days old (as parameterized)

sub prune_sigs
  {
    my ($age) = @_;
    my @siglist = prune_which_sigs($age);
    foreach (@siglist)
      {
        prune_signature($_);
      }
  }

# Prune samples that are
# a) in finished state
# b) older than N days old (as parameterized)

sub prune_samples
  {
    my ($age) = @_;
    my @sample_list = prune_which_samples($age);
    foreach (@sample_list)
      {
        prune_sample($_);
      }
  }

sub prune_which_sigs
  {
    my ($age) = @_;
    my @sigs_to_prune = ();
    # Search the database for all signature sequence numbers
    # Compare list against the "latest blessed"
    # 
    my $db2_date = _n_days_ago_db2($age);
    my $sql = "select signatureseqnum from signatures " .
      "where signatureseqnum < " .
        "(select min(newestblssdseqnum) from globals where alwayszero = 0)" .
          " and published < $db2_date";
    print "\n\n$sql\n\n";
    my $db_handle = run_query("AVISDATA",$sql);
    if ($db_handle == undef)
      {
        print STDERR $DB_ERROR;
      }
    else
      {
        # Select those sequence numbers as candidates for pruning
        
        while ($db_handle->FetchRow())
          {
            my ($sequence_number) = $db_handle->Data();
            push @sigs_to_prune,$sequence_number;
            print "---$sequence_number\n";
          }
        $db_handle->Close();
        # Return list of candidates that pass all tests
      }
    return @sigs_to_prune;
  }

sub run_query
  {
    my ($dsn,$sqlquery) = @_;
    # returns a list of hash references to records.
    # It's a little sloppy, but there's not enough data to worry about
    # performance yet.
    my @results = ();
    
    my $dbh = new Win32::ODBC($dsn);
    if ($dbh)
      {
        if ($dbh->Sql($sqlquery))
          {
            $DB_ERROR = $dbh->Error();
            print $DB_ERROR;
            $dbh->Close();
            $dbh = undef;
          }
      }
    return $dbh;
  }

sub prune_signature
  {
    my ($sequence) = @_;
    print "pruning signature $sequence\n";
    # remove signature record from database
    my $sql = "delete from signatures where signatureseqnum = $sequence";
    my $db_handle = run_query("AVISDATA",$sql);
    if ($db_handle == undef)
      {
        print STDERR "\n$DB_ERROR\n";
      }
    else
      {
        $db_handle->Close();
      }
    
    my $sig_base = $sig_root."\\" . sprintf("%08d",$sequence);
    # Remove file from filesystem
    if (-f "$sig_base.exe")
      {
        print STDERR "removing $sig_base.exe\n";
        unlink "$sig_base.exe";
      }
    elsif (-f "$sig_base.vdb")
      {
        print STDERR "removing $sig_base.vdb\n";
        unlink "$sig_base.vdb";
      }
    print STDERR "removing $sig_base";
    File::Path::rmtree $sig_base;
  }

sub prune_which_samples
  {
    my ($age) = @_;
    my @samples_to_prune = ();
    # Select all "finished" samples
    my $sql = "select distinct(analysisrequest.checksum) from analysisrequest, analysisresults " .
      "where analysisrequest.checksum = analysisresults.checksum";
    # Select all sample cookies that are in finished state (simple
    # join)
    my $db_handle = run_query("AVISDATA",$sql);
    # test -M > $age on all candidates 
    while ($db_handle->FetchRow())
      {
        my ($checksum) = $db_handle->Data();
        print "testing $sample_path/$checksum.sample\n";
        if (-M "$sample_path/$checksum.sample" > $age)
          {
            print "adding sample $checksum to prune list\n";
            push @samples_to_prune,$checksum;
          }
      }
    $db_handle->Close();
    # Return list of cookies that pass all tests
    return @samples_to_prune;
  }

sub prune_sample
  {
    my ($checksum) = @_;
    # Remove the sample from the database
    # Remove the AnalysisRequest record
    my $sql = "delete from AnalysisRequest where checksum = '$checksum'";
    my $dbh = run_query("AVISDATA",$sql);
    if ($dbh)
      {
        $dbh->Close();
      }
    # Remove the sample and attributes files from the filesystem
    unlink "$sample_path/$checksum.sample";
    unlink "$sample_path/$checksum.attributes";
  }

BEGIN
  {
  }
1;

