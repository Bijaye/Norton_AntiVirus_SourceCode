#######################################################################
#
# This Perl program processes Immune System gateway servlet log 
# files.  See the "help" text below for information about usage.
# 
#                                -- Edward Pring <pring@watson.ibm.com>
#######################################################################

use File::Path ();

# If no command arguments were specified, display the "help" text and
# then terminate.

if (scalar(@ARGV)==0 || $ARGV[0] =~ m/^(\?|\-\?|\/\?)$/i)
{
    print "
This program summarizes Immune System gateway transaction logs, and
stores the details of transactions that have errors or exceptions.
Optionally, the program can also store the details of all
transactions, organized by sample and signature numbers.

To summarize old transactions in one or more existing log files or
directories, the command syntax is:

   parseServletLogs.pl <filename|filespec|directory>...
                       [output=<directory>]
                       [before=hh:mm:ss]
                       [after=hh:mm:ss]
                       [/verbose]
                       [/detail]

To summarize new transactions as they are appended to the current log
file in a particular directory, the command syntax is:

   parseServletLogs.pl <directory> /follow 
                       [output=<directory>]
                       [/verbose]
                       [/detail]

where:

   <filename> specifies an Immune System gateway log file.

   <filespec> specifies a pattern (containing '*' as a wildcard) that
   matches one or more Immune system log files.

   <directory> specifies a directory containing Immune System gateway
   logs that match the pattern 'AVISservlets.*.log'.

   output=<directory> specifies the directory where the program will
   store its summary file, its problem file if necessary, and its 
   detail directory if specified.  By default the program stores 
   its output in the current directory.

   before=hh:mm:ss restricts the program to lines recorded before the
   specified time.  All entries with timestamps later than this time
   will be ignored.

   after=hh:mm:ss restricts the program to lines recorded after the
   specified time.  All entries with timestamps earlier than this time
   will be ignored.

   /follow <directory> specifies a directory containing Immune System
    gateway logs.  The current log file will be processed, starting
    at the end of the file (skipping all old records already in the
    file), as new records are appended to it.  When this switch is
    specified, the program will not terminate until manually cancelled.

   /verbose specifies that a summary of each transaction should be
    displayed as it completes.

   /detail specifies that a separate trace file should be created for
    each conversation.  The names of these files are based on sample
    cookie or signature sequence numbers.  Each trace file contains all
    log entries related to a particular sample or signature.

Operations notes:

* When more than one gateway log file is specified, they are processed
  in alphabetical order.

* Summaries are a single line for each transaction processed,
  containing only a few key attributes of the transaction.  Each
  summary record includes starting date and time, duration, thread,
  HTTP verb, and transaction type.  The record may also include sample
  checksum, signature sequence number, analysis state, error code, or
  exception, as appropriate.

* Summary records are written to summary files when transactions end,
  not when transactions end.  Since multiple transactions of varying
  duration may be processed simultaneously, the starting times of
  transactions may be out of order in summary files.

* Summary files are written to the output directory.  The names of
  the summary files are based on the log filenames, with
  '*.MM-DD.summary.log' replacing '*.log', where 'MM-DD' is the month
  and day that the transactions in the file started.

* Detail records include all of the trace lines for individual
  transactions, separated from all other concurrent transactions,
  preceded by the summary records for the transactions.

* Detail files contain all of the detail records for a particular
  sample or definition.  The detail filenames are based on the sample
  checksum or signature sequence number, 'sample.<checksum>.txt' or
  'signature.<sequence>.txt', respectively.  Detail files are created
  only if the '-detail' switch is specified.

* The detail directory contains all of the detail files.  The detail
  directory is a subdirectory of the output directory.  The name of
  the detail directory is based on the log filename, with '*.detail'
  replacing '*.log'.  The detail directory is created only if the
  '-detail' switch is specified.

* The problem file contains the details of all transactions containing
  errors or exceptions.  The problem file is written to the current
  directory, using a name based on the log filename, with
  '*.problem.log' replacing '*.log'.  The problem file is not created
  if none of the transactions processed contains any errors or
  exceptions.
";
    exit 0;
}

# These global variables contain the command arguments, and set their
# default values.

@inputList = ();
$outputDirectory = "";
$intervalAfter = "00:00:00";
$intervalBefore = "23:59:59";
$follow = 0;
$verbose = 0;
$detail = 0;

# A record of this program's processing will be appended to this
# log file.

$logfile = "parseServletLogs.log";

# These global variables are used to separate log entries by thread
# number into separate transactions.  When a complete transaction has
# been accumulated, it is summarized and its details are recorded.
# The thread's storage then is re-used for the next transaction on
# that thread.

%traceAttributes = ();
%traceLines = ();
$traceThread = 0;
$traceDate = "00-00";
$traceTime = "00:00:00";
$traceName = "";

# These global variables are used to accumulate statistics as the log
# files are processed.

$characterPercent = 0;
$characterCounter = 0;
$lineCounter = 0;
$kilobyteCounter = 0;
$kilobyteTotal = 0;
$transactionCounter = 0;
%functionCounter = ();
%detailCounter = ();

# Parse the command arguments and store them in the appropriate
# global variables.

foreach (@ARGV) 
{ 
    $outputDirectory = $1, next if /^output=(.+)$/i;
    $intervalBefore = $1, next if /^before=(\d\d:\d\d:\d\d)$/i;
    $intervalAfter = $1, next if /^after=(\d\d:\d\d:\d\d)$/i;
    $follow = 1, next if /^[\/\-]follow$/i;
    $verbose = 1, next if /[\/\-]verbose$/i;
    $detail = 1, next if /[\/\-]detail$/i;
    push @inputList, $_;
}

# Make sure the follow directory exists, if specified.

if ($follow)
{
    die "no directory specified to follow\n" if scalar(@inputList)<1;
    die "more than one directory specified to follow\n" if scalar(@inputList)>1;
    die "directory $inputList[0] to follow does not exist\n" if !(-d $inputList[0]);
}

# Make sure the output directory exists, if specified.

if ($outputDirectory)
{
    File::Path::mkpath($outputDirectory) unless -d $outputDirectory;
    $outputDirectory .= "\\" unless substr($outputDirectory,1,-1) eq "\\";
}

# Make sure the interval boundaries are not inverted.

die "inverted interval boundaries $intervalAfter to $intervalBefore\n"
    unless $intervalAfter le $intervalBefore;

# Do the processing, whatever it is.

$follow ? &processFollow() : &processFileList();
exit 0;





#----------------------------------------------------------------------
# This function watches the end of the current log in the specified
# directory, and when new records are appended to it, processes them.
# If the current log does not exist yet, the function waits for it 
# to be created.  If the name of the current log changes because the
# program continues running past midnight, this function switches to
# the new current log as soon as it is created.
#----------------------------------------------------------------------

sub processFollow # ()
{
    # Record some preliminary statistics for the user to ponder while we
    # wait for new input files.

    my $statistics = "started at " . scalar(localtime(time)) . ":\n";
    $statistics .= "   following directory $inputList[0]\n";
    $statistics .= "   processing interval will be $intervalAfter to $intervalBefore\n";
    $statistics .= "   transaction detail files will be created\n" if $detail;
    &log($statistics);

    # This loop processes new input files as they appear in the 
    # input directory.

    my $seek = 1;
    MAIN: while (1)
    {
	# Get the most recent Immune System log file from the
	# directory.  If there are no log files in that directory yet,
	# wait a while and look again.  If there are log files in that
	# directory, pick the most recent one to follow.

	my $file;
	while (!($file = (&expandFileList())[-1])) { $seek = 0; sleep(60); } 
	&log("following log file $file ...\n");
	$traceName = $1 if $file =~ m/\\?(\w+)(\..+)?$/i;

	# Open the current log file, and seek to the end only if this
	# is the first log file processed, and we did not have to wait
	# for it to appear in the directory.

	open FILE,"<$file" or &end("cannot open file $file, $!\n");
	if ($seek) { seek FILE, 0, 2 or &end("cannot seek to end of file $file, $!\n"); }
	$seek = 0;

	# Process the current log file in the list, starting at the
	# end, as records are appended to it.  When records are no
	# longer being appended to it, look for a more recent log file
	# in the directory, and if there is one, start following it
	# instead.

	my $idle = 0;
	READ: while (1)
	{
	    # Process all records in the log file, from the current
	    # position to the end of the file.

	    while (<FILE>) { $idle = 0; &processRecord($_); }

	    # Wait in one second increments for up to one minute for
	    # more records to be appended to the current log file.

	    sleep(1), next if ++$idle%60>0;

	    # After one minute has elapsed with no new records
	    # appended to the current log file, look in the directory
	    # for newer log files.  If one is found, repeat the outer
	    # loop.

	    last if (&expandFileList())[-1] ne $file;
	}

	# Close the old log file, and repeat the outer loop to open
	# the new log file.

	close FILE;
    }
}

#----------------------------------------------------------------------
# This function expands the input list by replacing any names
# containing wildcards with all files that match the pattern, and by
# replacing any directories with all servlet logs in the directory.
#----------------------------------------------------------------------

sub expandFileList # ()
{
    my @files;

    # Build a new list by expanding each element of the input list as
    # appropriate.

    foreach (@inputList)
    {
	push(@files, $_), next if -f $_;
	push(@files, glob($_)), next if m/\*/i;
	push(@files, glob("$_\\AVISservlets.*.log")), next if -d $_;
	print "ignoring unknown input parameter \"$_\"\n";
    }
    
    # Sort the new list into alphabetical order and return it.
    
    return sort {uc($a) cmp uc($b);} @files;
}

#----------------------------------------------------------------------
# This function prints statistics about the input files that will
# be processed, then calls another function to process each input
# file, and then prints more statistics about what it did.  
#----------------------------------------------------------------------

sub processFileList # ()
{
    # Expand the input list into a alphabetical list of log files.

    my @files = &expandFileList();

    # Count the total number of input characters in all input files.
    
    &end("no input files files found\n") unless scalar(@files);
    foreach (@files) { $kilobyteTotal += (stat($_))[7] >> 10; }
    
    # Record some preliminary statistics for the user to ponder while we
    # grind through the input files.

    my $statistics = "started at " . scalar(localtime(time)) . ":\n";
    $statistics .= join( '', map("   input file $_\n", @files ) );
    $statistics .= "   total input data will be " . &roundCount($kilobyteTotal) . "\n";
    $statistics .= "   processing interval will be $intervalAfter to $intervalBefore\n";
    $statistics .= "   transaction detail files will be created\n" if $detail;
    &log($statistics); 

    # Process each input file.  Then, after all input files have been
    # processed, process any incomplete transactions.
    
    my $start = time;
    foreach $file (@files) { &processFile($file); }
    foreach (sort keys %traceAttributes) { &processTransaction($_); }
    my $elapsed = time - $start;
    $elapsed++ unless $elapsed;
    
    # Relinquish control for a moment...
    
    sleep(1);
    
    # Add more information to the statistics file.
    
    $statistics = "finished at " . scalar(localtime(time)) . ":\n";
    $statistics .= "   $transactionCounter transactions\n";
    $statistics .= join( '', map("   $functionCounter{$_} $_ transactions\n", sort keys %functionCounter) );
    $statistics .= "   " . scalar(keys %detailCounter) . " detail files created\n";
    $statistics .= "parser performance:\n";
    $statistics .= "   " . &roundTime($elapsed) . " elapsed time\n";
    $statistics .= "   $transactionCounter transactions (" . int($transactionCounter/$elapsed) ." transactions/second)\n";
    $statistics .= "   $lineCounter lines (" . int($lineCounter/$elapsed) . " lines/second)\n";
    $statistics .= "   $kilobyteCounter kilobytes (" . int($kilobyteCounter/$elapsed) . " kilobytes/second)\n";
    &log($statistics); 
}

#----------------------------------------------------------------------
# This function rounds a kilobyte counter off to the nearest kilobyte
# or megabyte or gigabyte and returns a string suitable for display.
#----------------------------------------------------------------------

sub roundCount # ($)
{
    my ($kilobytes) = @_;

    return sprintf("%.1f GB", $kilobytes/(1024*1024)) if $kilobytes>2*1024*1024;
    return sprintf("%.1f MB", $kilobytes/(1024)) if $kilobytes>2*1024;
    return sprintf("%.1f KB", $kilobytes);
}

#----------------------------------------------------------------------
# This function rounds a second counter off to the nearest minute
# or hour and returns a string suitable for display.
#----------------------------------------------------------------------

sub roundTime # ($)
{
    my ($seconds) = @_;

    return sprintf("%.1f hours", $seconds/(60*60)) if $seconds>2*60*20;
    return sprintf("%.1f minutes", $seconds/60) if $seconds>2*60;
    return "$seconds seconds";
} 

#----------------------------------------------------------------------
# This function processes a log file one line at a time.  Some basic
# statistics are accumulated, and "percent complete" is displayed.
# Lines are separated by thread and accumulated in a global
# variable. When the beginning or end of a transaction is found, the
# accumulated lines are processed further by another function.
#----------------------------------------------------------------------

sub processFile # ($)
{
    # The argument for this function is the name of a log file.

    my ($input) = @_;
    &log("processing log file \"$input\" ...\n");

    # These global variables contain the names of the files in which
    # transaction summary records and transactions containing exceptions
    # will be accumulated, plus the directory where transaction details
    # will be stored.
    
    $traceName = $1 if $input =~ m/\\?(\w+)(\..+)?$/i;

    # The specified log file is processed one line at a time.
    
    open INPUT,"<$input" or &end("cannot open file $input, $!\n");
    while (<INPUT>) { processRecord($_); }
    close INPUT;
}

#----------------------------------------------------------------------
# This function processes a line from a log file.  Lines are
# accumulated by thread in a global variable. When the beginning or
# end of a transaction is found, the accumulated lines are processed
# further by another function.
#----------------------------------------------------------------------

sub processRecord # ($)
{
    my ($record) = @_;

    # Accumulate basic statistics first.
    
    $lineCounter++;
    $characterCounter += length($record);
    if ($characterCounter>1024)
    {
	$kilobyteCounter += int($characterCounter/1024);
	$characterCounter %= 1024;
    }
    
    # Display "percent complete" based on the statistics accumulated
    # so far.
    
    if ($kilobyteTotal>0 && int(100*($kilobyteCounter/$kilobyteTotal))>$characterPercent)
    {
	#print "kilobyteCounter=$kilobyteCounter kilobyteTotal=$kilobyteTotal\n";
	$characterPercent = int(100*($kilobyteCounter/$kilobyteTotal));
	print "   $traceDate $traceTime $characterPercent% " . "\b"x30;
    }

    # Extract the thread number and timestamp from this line into
    # global variables, if it has them.  Otherwise, this line
    # belongs to the same thread as the previous line, so use its
    # values. If we are restricted to a particular time interval,
    # skip this line unless it falls within the interval.
    
    ($traceThread,$traceDate,$traceTime) = ($1,$2,$3) if $record =~ m/\d+ (\d+) ([\d\-]+) ([\d\:\.]+)/;
    return unless ($intervalAfter le $traceTime) && ($traceTime le $intervalBefore);

    # If this line is the beginning of a transaction, and we have
    # accumulated lines for this thread in a global variable,
    # process the accumulated lines as a complete transaction, and
    # then reset the global variable.
    
    if ( $record =~ m/ProcessClient Executing (\w+)/i )
    {
	&processTransaction($traceThread);
	undef $traceLines{$traceThread};
	undef $traceAttributes{$traceThread};
	$traceAttributes{$traceThread}{"function"} = $1;
	$traceAttributes{$traceThread}{"startdate"} = "$traceDate";
	$traceAttributes{$traceThread}{"starttime"} = "$traceTime";
    }
    
    # Store this line in a global array.
    
    push(@{$traceLines{$traceThread}}, $_);
    
    # If this line contains an attribute, extract and save the
    # name and value in a global hash.  This may replace a
    # previously saved value, if an attribute name appears more
    # than once in a transaction.  And the name is capitalized,
    # since attribute names are case-insensitive.

    $traceAttributes{$traceThread}{uc($2)} = $3 if $record =~ m/(INHEADER|OUTHEADER) (\S+): ([^\r\n]*)/i;

    # If this line contains an exception, extract and save the
    # exception in the global hash.
    
    $traceAttributes{$traceThread}{"exception"} = $1 if $record =~ m/exception([^\r\n]*)/i;
    
    # If this line is the end of a transaction, process the
    # accumulated lines in the global variable as a complete
    # transaction, and then reset the global variable.
    
    if ( $record =~ m/Destroying TraceClient/i )
    {
	$traceAttributes{$traceThread}{"enddate"} = "$traceDate";
	$traceAttributes{$traceThread}{"endtime"} = "$traceTime";
	&processTransaction($traceThread);
	undef $traceLines{$traceThread};
	undef $traceAttributes{$traceThread};
    }
}

#----------------------------------------------------------------------
# This function processes a trace of a transaction that has been
# accumulated from a log file.  The transaction may be complete or
# incomplete.  Some basic statistics are accumulated.  The type of
# transaction and a few key attributes are extracted from the trace,
# and a summary of the transaction is formatted.  The summary and/or
# trace are written to separate output files, as specified by the
# command arguments.
#----------------------------------------------------------------------

sub processTransaction # ($)
{
    # The argument for this function is the thread number for the
    # transaction to be processed.  This thread number is the index
    # for the hashes in which traced lines and attributes are stored.

    my ($thread) = @_;
    return unless scalar(keys %{$traceAttributes{$thread}});

    # If starting or ending dates and times for this transaction were
    # found in the log, save them in local variables.

    my $startdate = $traceAttributes{$thread}{"startdate"};
    my $starttime = $traceAttributes{$thread}{"starttime"};
    my $nostart = "";

    my $enddate = $traceAttributes{$thread}{"enddate"};
    my $endtime = $traceAttributes{$thread}{"endtime"};
    my $noend = "";

    # If there is no starting timestamp, use the first timestamp in
    # the trace detail records.

    if (!$startdate) 
       { foreach (@{$traceLines{$thread}}) 
	 { ($nostart,$startdate,$starttime) = ("?",$2,$3), last if m/\d+ (\d+) ([\d\-]+) ([\d\:\.]+)/; } }

    # If there is no ending timestamp, use the last timestamp in
    # the trace detail records.

    if (!$enddate) 
       {
	   $enddate = $traceDate;
	   $endtime = $traceTime;
	   $noend = "?";
       }

    # Compute the duration of the transaction, as best we can.  This
    # function will return "?" if the starting or ending time is
    # missing.

    my $duration = &elapsedTime($startdate,$starttime,$enddate,$endtime);

    # Extract the transaction function from the global hash, if
    # it is known, or substitute a placeholder, if it is not.

    my $function = $traceAttributes{$thread}{"function"};
    $function = "unknown" unless $function;

    # Accumulate some basic statistics.

    $transactionCounter++;
    $functionCounter{$function}++;

    # Extract the important attributes from the global hash.

    my $method    = $traceAttributes{$thread}{"REQUEST_METHOD"};
    my $cookie    = $traceAttributes{$thread}{"X-ANALYSIS-COOKIE"};
    my $checksum  = uc($traceAttributes{$thread}{"X-SAMPLE-CHECKSUM"});
    my $state     = $traceAttributes{$thread}{"X-ANALYSIS-STATE"};
    my $sequence  = $traceAttributes{$thread}{"X-SIGNATURES-SEQUENCE"};
    my $analyzed  = $traceAttributes{$thread}{"X-DATE-ANALYZED"};
    my $blessed   = $traceAttributes{$thread}{"X-DATE-BLESSED"};
    my $location  = $2 if $traceAttributes{$thread}{"HTTP_LOCATION"} =~ /(http|https):\/\/(\w+)/i;
    my $error     = $traceAttributes{$thread}{"X-ERROR"};
    my $exception = $traceAttributes{$thread}{"exception"};

    # Make sure that any cookie and sequence numbers extracted from
    # the trace have leading zeroes.

    $cookie = sprintf "%08d", $cookie if $cookie  =~ /^\d+$/;
    $sequence = sprintf "%08d", $sequence if $sequence =~ /^\d+$/;

    # Compose a single line describing this transaction for the
    # summary and/or trace files.

    my $description = "$startdate $starttime$nostart $duration$noend [$thread] $method $function";
    $description .= " state=$state" if $state;
    $description .= "[final]" if $state && $analyzed;
    $description .= " sequence=$sequence" if $sequence;
    $description .= "[blessed]" if $sequence && $blessed;
    $description .= " cookie=$cookie" if $cookie;
    $description .= " checksum=$checksum" if $checksum;
    $description .= " error=$error" if $error;
    $description .= " location=$location" if $location;
    $description .= " exception=$exception" if $exception;
    $description .= "\n";
    print $description if $verbose;

    # Calculate the name of the summary file.  If the summary file
    # exists, but this instance of this program has not yet written to
    # it, compose an extra line to separate our transactions from
    # previous transactions.

    my $filename = "$outputDirectory$traceName.$startdate.summary.txt";
    my $separator = -f $filename && !$summaryFiles{$filename} ? "\\/"x40 . "\n\n" : "";
    $summaryFiles{$filename} = 1;

    # Append the description to the summary file.

    open SUMMARY,">>$filename" or &end("cannot open file $filename, $!\n");
    binmode SUMMARY;
    print SUMMARY $separator . $description or &end("cannot write file $filename, $!\n");
    close SUMMARY;

    # If there is no detail for this transaction, or the detail is
    # not needed, we are done.

    return unless scalar(@{$traceLines{$thread}});
    return unless $error || $exception || $detail;

    # Format the detail for this transaction.

    my $transaction = "<><> $description";
#    $transaction .= join('', 
#		    map("     $_=$traceAttributes{$thread}{$_}\n", 
#			sort keys %{$traceAttributes{$thread}} ) );
    $transaction .= join('',@{$traceLines{$thread}});
    $transaction .='-'x60 . "\n";

    # Append the transaction detail to the problem file, if necessary.

    if ($error || $exception)
    {
	$functionCounter{"problem"}++;
	my $filename = "$outputDirectory$traceName.$startdate.problem.txt";
	open PROBLEM,">>$filename" or &end("cannot open file $filename, $!\n");
	binmode PROBLEM;
	print PROBLEM $transaction or &end("cannot write file $filename, $!\n");
	close PROBLEM;
    }

    # Append the transaction detail to the appropriate sample or
    # definition file in the detail directory.

    if ($detail)
    {
	my $directory = "$outputDirectory$traceName.detail";
	File::Path::mkpath($directory) unless -d $directory;
	my $filename = $function;
	$filename = "sample.$checksum" if $checksum && $function =~ m/sample/i;
	$filename = "signature.$sequence" if $sequence && $function =~ m/signature/i;
	$detailCounter{$filename}++;
	$filename = "$directory\\$filename.txt";
	open DETAIL,">>$filename" or &end("cannot open file $filename, $!\n");
	binmode DETAIL;
	print DETAIL $transaction or &end("cannot write file $filename, $!\n");
	close DETAIL;
    }
}

#----------------------------------------------------------------------
# This function extracts the starting and ending timestamps from a
# trace of a transaction, and calculates the elapsed time for the
# transaction.  It returns a string containing the starting timestamp,
# and the elapsed time in seconds.
#----------------------------------------------------------------------

sub elapsedTime # ($)
{
    # The arguments for this function are the starting and ending date
    # and time for a transaction.

    my ($startdate,$starttime,$enddate,$endtime) = @_;
    return "?" unless $startdate && $starttime && $enddate && $endtime;

    # Extract the individual components of the starting date and time.

    $startdate =~ m/(\d\d)-(\d\d)/i;
    my ($month0,$day0) = ($1,$2);
    $starttime =~ m/(\d\d):(\d\d):(\d\d)(\.\d+)?/i;
    my ($hour0,$minute0,$second0,$milli0) = ($1,$2,$3,$4);

    # Extract the individual components of the ending date and time.

    $enddate =~ m/(\d\d)-(\d\d)/i;
    my ($month1,$day1) = ($1,$2);
    $endtime =~ m/(\d\d):(\d\d):(\d\d)(\.\d+)?/i;
    my ($hour1,$minute1,$second1,$milli1) = ($1,$2,$3,$4);

    # If the transaction spanned a month boundary, adjust the ending
    # day accordingly.

    $day1 = $day0 + 1 if $day1<$day0;

    # Calculate the number of seconds from the beginning of the month
    # to the first and last timestamp.

    my $time0 = 24*60*60*$day0 + 60*60*$hour0 + 60*$minute0 + $second0 + $milli0;
    my $time1 = 24*60*60*$day1 + 60*60*$hour1 + 60*$minute1 + $second1 + $milli1;

    # Return the starting timestamp and the elapsed time in seconds.

    my $duration = $milli1 eq "" ? $time1 - $time0 : sprintf("%0.3f", $time1 - $time0);
    return $duration;
}

#----------------------------------------------------------------------
# This function is called when the program terminates.  It writes
# a final message to the log and terminates the program.
#----------------------------------------------------------------------

sub end (@)
{
    &log("END: " . @_);
    exit 1;
}

#----------------------------------------------------------------------
# This function is called when an event worth recording has occurred.
# It writes one or more lines to standard output.  The lines are also
# written to the program's log file, prefixed with the current date
# and time.  The log file is kept closed except when writing.
#----------------------------------------------------------------------

sub log (@)
{
    # The parameters for this function are one or more lines of 
    # messages, separated by newline and/or carriage return characters, 
    # specified as a list of one or more list items.  To make sense of 
    # this, concatenate the list into one long string and then split
    # the string into a list at the newline and/or carriage return
    # characters.

    my @message = split(/[\r\n]/, join("",@_));

    # Write the message to the console.

    print join("\n", @message) . "\n";

    # Format the current date and time into compact strings for use
    # in timestamping the message, and also for naming the log file.

    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time());
    my $timestamp = sprintf "%02d-%02d %02d:%02d:%02d", 1+$mon, $mday, $hour, $min, $sec;
    my $padding = " " x length($timestamp);

    # Write the entire message into the log file, prepending the
    # current date and time to the first line, and padding of
    # equivalent length to all subsequent lines.

    my $filename = "$outputDirectory$logfile";
    my $separator = -f $filename && !$logged ? "\\/"x40 . "\n\n" : "";
    $logged = 1;
    open LOG, ">>$filename" or die "cannot open file $filename, $!\n";
    print LOG $separator . "$timestamp " . join("\n$padding ",@message) . "\n" 
	or die "cannot write file $filename, $!\n";
    close LOG;      
}
