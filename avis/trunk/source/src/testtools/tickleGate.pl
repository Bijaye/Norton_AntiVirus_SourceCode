#######################################################################
#
# This Perl program tests Immune System transactions to verify that 
# a gateway is operating.  See the "help" text below for information 
# about usage.
#
#                                -- Edward Pring <pring@watson.ibm.com>
#######################################################################

use File::Path;

# This program uses several public Perl packages, which must be installed 
# in the Perl library directory on the machine where this program executes:

use Compress::Zlib ();
use HTTP::Date ();
use HTTP::Request ();
use HTTP::Response ();
#use LWP::Debug qw(+); # enable all low-level tracing options ...
use LWP::UserAgent ();
use MD5 ();
use Socket;
use URI::URL ();

# Override the redirect_ok() method of LWP::UserAgent to return FALSE
# always.  This prevents LWP::UserAgent from handling redirects for all
# verbs, including GET.

package OverriddenUserAgent;

use LWP::UserAgent;
@OverriddenUserAgent::ISA = ("LWP::UserAgent");
sub redirect_ok { return 0; } 
1;

package main;

# If no command arguments were specified, display the "help" text and
# then terminate.

if (scalar(@ARGV)==0 || $ARGV[0] =~ m/^(\?|\-\?|\/\?)$/i)
{
    print "

This program conducts specified test transactions with a specified
gateway, and indicates whether or not the transactions succeeded.

The command syntax is:

   tickleGate.pl <gateway> <action>... 
                 [port=<port>]
                 [timeout=<timeout>]
                 [output=<directory>]
                 [/ssl]
                 [/vdb]
                 [/trace]

where:

   <gateway> specifies the TCP/IP name of the gateway to be tickled.

   <action>... specifies one or more actions to perform with the
   gateway.  The actions may be selected from this list:

      report   -- postSampleStatus with checksum '000..000' and final state 'tickled'
      query    -- getSampleStatus with checksum '000..000'
      export   -- postSignatureSet (not supported)
      blessed  -- headSignatureSet with sequence number 'blessed'
      newest   -- headSignatureSet with sequence number 'newest'
      download -- getSignatureSet with sequence number 'blessed'
      submit   -- postSuspectSample with EICAR test string
      import   -- getSuspectSample (not supported)

   port=<port> specifies the TCP/IP port to connect to on the gateway.
   If this argument is omitted, the default is 2847 (or 2848, if the
   '/ssl' switch is specified).

   timeout=<timeout> specifies the maximum time in seconds to wait for
   a response from the gateway.  If this argument is omitted, the
   default is 60 seconds.

   output=<directory> specifies the directory where the program will
   store its log file.  By default the program stores its log file in
   the current directory.

   /ssl causes the transaction to be authenticated and encrypted using
   the SSL protocol.  If this argument is omitted, the transaction is
   not authenticated or encrypted.

   /vdb causes VDB packages downloaded by the getSignatureSet
   transaction to be stored in the current dirctory, if the
   transaction is successful.  If this argument is omitted, VDB
   packages are discarded after downloading.

   /trace causes the HTTP request and response messages to be written
   to the program's log file.

Upon completion, this program writes a single summary line that
indicates whether the tickle succeeded, or if not, why it failed.  The
summary line is displayed on the console, and is written to the
'tickleGate.log' file in the output directory.

The completion code from the program is zero if all specified
transactions completed successfully.  If any of the specified
transactions failed for any reason, the completion code from the
program is the highest return code from any failed transaction:

   0 -- success
   1 -- network error
   2 -- redirected
   3 -- gateway error
   4 -- content error
  10 -- unsupported action
";
    exit 100;
}

# Parse the command arguments and store them in an argument hash.

$output = "";
my %arg = ();
my @actions = ();
foreach (@ARGV) 
{ 
    $arg{gateway}  = $_,   next unless $arg{gateway};
    $arg{port}     = $1,   next if     m/^port=(\d+)$/i;
    $arg{timeout}  = $1,   next if     m/^timeout=(\d+)$/i;
    $output        = $1,   next if     m/^output=(.+)$/i;
    $arg{ssl}      = 1,    next if     m/^[\/\-]ssl$/i;
    $arg{vdb}      = 1,    next if     m/^[\/\-]vdb$/i;
    $arg{sample}   = 1,    next if     m/^[\/\-]sample$/i;
    $arg{trace}    = 1,    next if     m/^[\/\-]trace$/i;
    push(@actions, $_);
}

die "no gateway specified\n" unless $arg{gateway};
die "no actions specified\n" unless scalar(@actions);

# Make sure the output directory exists, if specified.

if ($output)
{
    File::Path::mkpath($output) unless -d $output;
    $output .= "\\" unless substr($output,1,-1) eq "\\";
    print "output=$output\n";
}

# Store default values for missing arguments in the hash.

$arg{timeout} = 60 unless $arg{timeout};
$arg{port} = $arg{ssl} ? 2848 : 2847 unless $arg{port};
$arg{protocol} = $arg{ssl} ? "https" : "http";
###foreach (sort keys %arg) { print "arg{$_}=$arg{$_}\n"; }

# Sample submission transactions will use the EICAR test string.

my $eicar = "X5O!P\%\@AP[4\\PZX54(P^)7CC)7}\$EICAR-STAN" . "DARD-ANTIVIRUS-TEST-FILE!\$H+H*";

# Run the specified transactions, accumulating the highest completion
# code returned.

my $maxrc = 0;
foreach (@actions)
{
    my $rc = -1;
    $rc = &postSampleStatus ("0"x32, "tickled", %arg) if m/^report$/i;
    $rc = &getSampleStatus  ("0"x32,            %arg) if m/^query$/i;
    $rc = &postSignatureSet (0,0,               %arg) if m/^export$/i;
    $rc = &headSignatureSet ("blessed",         %arg) if m/^blessed$/i;
    $rc = &headSignatureSet ("newest",          %arg) if m/^newest$/i;
    $rc = &getSignatureSet  ("blessed",         %arg) if m/^download$/i;
    $rc = &getSignatureSet  ("newest",          %arg) if m/^downloadnewest$/i;
    $rc = &postSuspectSample($eicar,            %arg) if m/^submit$/i;
    $rc = &getSuspectSample (                   %arg) if m/^import$/i;
    print "unknown action \"$_\" ignored\n" if $rc == -1;
    $maxrc = $rc if $rc>$maxrc;
}
exit $maxrc;



#----------------------------------------------------------------------
# This function is called to test status reports.
#----------------------------------------------------------------------

sub postSampleStatus (%)
{
    # The arguments for this function specify the sample checksum and
    # the final state to report, plus the program's command arguments.

    my ($checksum,$state,%arg) = @_;

    # Create a hash for the status report input headers, and insert the
    # required sample-related attributes.

    my %inheader = ();
    $inheader{'X-CHECKSUM-METHOD'} = "md5";
    $inheader{'X-SAMPLE-CHECKSUM'} = $checksum;
    $inheader{'X-ANALYSIS-STATE'} = $state;
    $inheader{'X-DATE-ANALYZED'} = HTTP::Date::time2str();

    # Create a request message for the test submission and 
    # send it to the gateway.

    my $url = new URI::URL "$arg{protocol}://$arg{gateway}:$arg{port}/AVIS/postSampleStatus";
    my $headers = new HTTP::Headers %inheader;
    my $request = new HTTP::Request "GET", $url, $headers;
    my $response = new HTTP::Response;
    &transaction(\$request,\$response,\%arg);

    # Extract the output headers from the response message into a
    # hash.

    my %outheader = hashify($response->headers_as_string);

    # Check for errors at each layer.  If an error is found at any
    # layer, log it and return a non-zero completion code.

    &out("$arg{gateway} postSampleStatus: network error, " . $response->status_line . "\n"), return 1 
	if $response->is_error;

    &out("$arg{gateway} postSampleStatus: redirected\n"), return 2 
	if $response->is_redirect;

    &out("$arg{gateway} postSampleStatus: gateway error, $outheader{'X-ERROR'}\n"), return 3
	if $response->is_success && $outheader{'X-ERROR'};

    # If no errors are found at any layer, return a zero completion code.

    &out("$arg{gateway} postSampleStatus: OK, $arg{elapsed} seconds, final state is $state\n");
    return 0;
}

#----------------------------------------------------------------------
# This function is called to test status queries.
#----------------------------------------------------------------------

sub getSampleStatus (%)
{
    # The arguments for this function specify the sample checksum to
    # query, plus the program's command arguments.

    my ($checksum,%arg) = @_;

    # Create a hash for the status query input headers, and insert the
    # required sample-related attributes.

    my %inheader = ();
    $inheader{'X-CHECKSUM-METHOD'} = "md5";
    $inheader{'X-SAMPLE-CHECKSUM'} = $checksum;

    # Create a request message for the test submission and 
    # send it to the gateway.

    my $url = new URI::URL "$arg{protocol}://$arg{gateway}:$arg{port}/AVIS/getSampleStatus";
    my $headers = new HTTP::Headers %inheader;
    my $request = new HTTP::Request "GET", $url, $headers;
    my $response = new HTTP::Response;
    &transaction(\$request,\$response,\%arg);

    # Extract the output headers from the response message into a
    # hash.

    my %outheader = hashify($response->headers_as_string);

    # Check for errors at each layer.  If an error is found at any
    # layer, log it and return a non-zero completion code.

    &out("$arg{gateway} getSampleStatus: network error, " . $response->status_line . "\n"), return 1 
	if $response->is_error;

    &out("$arg{gateway} getSampleStatus: redirected\n"), return 2 
	if $response->is_redirect;

    &out("$arg{gateway} getSampleStatus: gateway error, $outheader{'X-ERROR'}\n"), return 3
	if $response->is_success && $outheader{'X-ERROR'};

    # If no errors are found at any layer, return a zero completion code.

    my $final = $outheader{'X-DATE-ANALYZED'} ? "final state" : "non-final state";
    &out("$arg{gateway} getSampleStatus: OK, $arg{elapsed} seconds, $final is $outheader{'X-ANALYSIS-STATE'}\n");
    return 0;
}

#----------------------------------------------------------------------
# This function is called to test definition publishing.
#----------------------------------------------------------------------

sub postSignatureSet (%)
{
    &out("$arg{gateway} postSignatureSet: not supported\n");
    return 10;

    # The arguments for this function specify the signature sequence
    # number of the VDB package to export and a boolean that indicates
    # whether or not they are blessed, plus the program's command
    # arguments.

    my ($sequence,$blessed,%arg) = @_;

    # Make sure the sequence number has a numeric value and then pad
    # it out to eight characters with leading zeroes.

    &out("$arg{gateway} postSignatureSet: \"$sequence\" is not numeric\n"), return 4 
	unless $sequence =~ m/^\d+$/i;
    $sequence = sprintf "%08d", $sequence;

    # Load the VDB package into a string variable.

    my $filename = "$sequence.VDB";
    &out("$arg{gateway} postSignatureSet: cannot find file $filename\n"), return 4 
	unless -f $filename;
    my $size = (stat($filename))[7];
    my $content;
    open VDB, "<$filename" 
	or &out("$arg{gateway} postSignatureSet: cannot open file $filename, $!\n"), return 4;
    binmode VDB;
    read VDB, $content, $size 
	or &out("$arg{gateway} postSignatureSet: cannot read file $filename, $!\n"), return 4;
    close VDB;
    &out("$arg{gateway} postSignatureSet: length error reading file $filename\n"), return 4 
	unless length($content)==$size;

    # Create an attribute hash and put the sequence number in it.

    my %inheader = ();
    $inheader{'X-SIGNATURES-SEQUENCE'} = $sequence;
    $inheader{'X-SIGNATURES-CHECKSUM'} = uc(MD5->hexhash($content));

    # Add the required date-related attributes to the hash.

    $inheader{'X-DATE-PRODUCED'} = HTTP::Date::time2str();
    $inheader{'X-DATE-PUBLISHED'} = HTTP::Date::time2str();
    $inheader{'X-DATE-BLESSED'} = HTTP::Date::time2str() if $blessed;

    # Add the required content-related attributes to the hash.

    $inheader{'X-CHECKSUM-METHOD'} = "md5";
    $inheader{'X-CONTENT-CHECKSUM'} = uc(MD5->hexhash($content));
    $inheader{'CONTENT-LENGTH'} = length($content);
    $inheader{'CONTENT-TYPE'} = "application/x-NAV-VDB";

    # Create a request message for the test submission and 
    # send it to the gateway.

    my $url = new URI::URL "$arg{protocol}://$arg{gateway}:$arg{port}/AVIS/postSignatureSet";
    my $headers = new HTTP::Headers %inheader;
    my $request = new HTTP::Request "POST", $url, $headers, $content;
    my $response = new HTTP::Response;
    &transaction(\$request,\$response,\%arg);

    # Extract the output headers from the response message into a
    # hash.

    my %outheader = hashify($response->headers_as_string);

    # Check for errors at each layer.  If an error is found at any
    # layer, log it and return a non-zero completion code.

    &out("$arg{gateway} postSignatureSet: network error, " . $response->status_line . "\n"), return 1 
	if $response->is_error;

    &out("$arg{gateway} postSignatureSet: redirected\n"), return 2 
	if $response->is_redirect;

    &out("$arg{gateway} postSignatureSet: gateway error, $outheader{'X-ERROR'}\n"), return 3
	if $response->is_success && $outheader{'X-ERROR'};

    # If no errors are found at any layer, return a zero completion code.

    my $which = $blessed ? "blessed sequence number" : "un-blessed sequence number";
    my $throughput = sprintf "%0.1f", (length($content)/1024)/$arg{elapsed};
    &out("$arg{gateway} postSignatureSet: OK, $arg{elapsed} seconds, $which is $sequence, $throughput kilobytes/second\n");
    return 0;
}

#----------------------------------------------------------------------
# This function is called to test definition availability.
#----------------------------------------------------------------------

sub headSignatureSet (%)
{
    # The arguments for this function specify the actual or symbolic
    # signature sequence number to check, plus the program's command
    # arguments.

    my ($sequence,%arg) = @_;

    # Create a hash for the signature availability input headers, and
    # insert the required signature-related attributes.

    my %inheader = ();
    $inheader{'X-SIGNATURES-SEQUENCE'} = $sequence;

    # Create a request message for the test submission and 
    # send it to the gateway.

    my $url = new URI::URL "$arg{protocol}://$arg{gateway}:$arg{port}/AVIS/headSignatureSet";
    my $headers = new HTTP::Headers %inheader;
    my $request = new HTTP::Request "HEAD", $url, $headers;
    my $response = new HTTP::Response;
    &transaction(\$request,\$response,\%arg);

    # Extract the output headers from the response message into a
    # hash.

    my %outheader = hashify($response->headers_as_string);

    # Check for errors at each layer.  If an error is found at any
    # layer, log it and return a non-zero completion code.

    &out("$arg{gateway} headSignatureSet: network error, " . $response->status_line . "\n"), return 1 
	if $response->is_error;

    &out("$arg{gateway} headSignatureSet: redirected\n"), return 2 
	if $response->is_redirect;

    &out("$arg{gateway} headSignatureSet: gateway error, $outheader{'X-ERROR'}\n"), return 3
	if $response->is_success && $outheader{'X-ERROR'};

    # If no errors are found at any layer, return a zero completion code.

    &out("$arg{gateway} headSignatureSet: OK, $arg{elapsed} seconds, $sequence is $outheader{'X-SIGNATURES-SEQUENCE'}\n");
    return 0;
}

#----------------------------------------------------------------------
# This function is called to test definition download.
#----------------------------------------------------------------------

sub getSignatureSet (%)
{
    # The arguments for this function specify the actual or symbolic
    # signature sequence number to download, plus the program's
    # command arguments.

    my ($sequence,%arg) = @_;

    # Create a hash for the signature download input headers, and
    # insert the required signature-related attributes.

    my %inheader = ();
    $inheader{'X-SIGNATURES-SEQUENCE'} = $sequence;
    $inheader{'ACCEPT'} = "application/*";

    # Create a request message for the test submission and 
    # send it to the gateway.

    my $url = new URI::URL "$arg{protocol}://$arg{gateway}:$arg{port}/AVIS/getSignatureSet";
    my $headers = new HTTP::Headers %inheader;
    my $request = new HTTP::Request "GET", $url, $headers;
    my $response = new HTTP::Response;
    &transaction(\$request,\$response,\%arg);

    # Extract the output headers from the response message into a
    # hash.

    my %outheader = hashify($response->headers_as_string);

    # Check for errors at each layer.  If an error is found at any
    # layer, log it and return a non-zero completion code.

    &out("$arg{gateway} getSignatureSet: network error, " . $response->status_line . "\n"), return 1 
	if $response->is_error;

    &out("$arg{gateway} getSignatureSet: redirected\n"), return 2 
	if $response->is_redirect;

    &out("$arg{gateway} getSignatureSet: gateway error, $outheader{'X-ERROR'}\n"), return 3
	if $response->is_success && $outheader{'X-ERROR'};

    # Verify that the content datatype is "application/x-*" without being
    # too picky.

    my $datatype = $outheader{'CONTENT-TYPE'};
    &out("$arg{gateway} getSignatureSet: content error, unknown content type\n"), return 4
	 unless $datatype =~ /application\/x-.+/i;

    # Verify that the actual and expected size of the content match.
    
    my $sizeContent = length($response->content);
    my $sizeHeader = $outheader{'CONTENT-LENGTH'};
    &out("$arg{gateway} getSignatureSet: content error, incorrect content length \n"), return 4
	unless $sizeContent==$sizeHeader;

    # Verify that the content checksum was calculated with the MD5 method.

    my $method = $outheader{'X-CHECKSUM-METHOD'};
    &out("$arg{gateway} getSignatureSet: content error, unknown checksum method\n"), return 4
	unless $method eq "md5";

    # Verify that the actual and expected checksums of the content match.

    my $digestContent = uc(MD5->hexhash($response->content)); 
    my $digestHeader = $outheader{'X-CONTENT-CHECKSUM'};
    &out("$arg{gateway} getSignatureSet: content error,  incorrect content checksum\n"), return 4
	unless uc($digestContent) eq uc($digestHeader);

    # Store the VDB package in the local directory, if necessary.

    if ($arg{vdb})
    {
	my $filename = "$outheader{'X-SIGNATURES-SEQUENCE'}.VDB";
	open VDB, ">$filename" or die "cannot open file $filename, $!";
	binmode VDB;
	print VDB $response->content or die "cannot write file $filename, $!";
	close VDB;
    }

    # If no errors are found at any layer, return a zero completion code.

    my $which = $outheader{'X-DATE-BLESSED'} ? "blessed sequence number" : "un-blessed sequence number";
    my $throughput = sprintf "%0.1f", ($sizeContent/1024)/$arg{elapsed};
    &out("$arg{gateway} getSignatureSet: OK, $arg{elapsed} seconds, $which is $outheader{'X-SIGNATURES-SEQUENCE'}, $throughput kilobytes/second\n");
    return 0;
}

#----------------------------------------------------------------------
# This function is called to test sample submissions.
#----------------------------------------------------------------------

sub postSuspectSample (%)
{
    # The arguments for this function specify the sample content to
    # submit, plus the program's command arguments.

    my ($content,%arg) = @_;

    # Create a hash for the submission input headers, and insert the
    # required sample-related attributes.

    my %inheader = ();
    $inheader{'X-CHECKSUM-METHOD'} = "md5";
    $inheader{'X-SAMPLE-CHECKSUM'} = uc(MD5->hexhash($content));
    $inheader{'X-SAMPLE-FILE'} = ".\\EICAR.COM";
    $inheader{'X-SAMPLE-EXTENSION'} = "COM";
    $inheader{'X-SAMPLE-PRIORITY'} = "999";
    $inheader{'X-SAMPLE-REASON'} = "unknown";
    $inheader{'X-SAMPLE-SIZE'} = length($content);
    $inheader{'X-SAMPLE-TYPE'} = "file";

    # Add the required date-related attributes to the hash.

    $inheader{'X-DATE-QUARANTINED'} = HTTP::Date::time2str();

    # Compress and scramble the test submission content.

    $content = Compress::Zlib::compress(\$content);
    $content = &scramble(\$content);
    $inheader{'X-CONTENT-COMPRESSION'} = "deflate";
    $inheader{'X-CONTENT-SCRAMBLING'} = "xor-vampish";

    # Add the required content-related attributes to the hash.

    $inheader{'X-CONTENT-CHECKSUM'} = uc(MD5->hexhash($content));
    $inheader{'CONTENT-LENGTH'} = length($content);
    $inheader{'CONTENT-TYPE'} = "application/x-avis-sample";

    # Create a request message for the test submission and 
    # send it to the gateway.

    my $url = new URI::URL "$arg{protocol}://$arg{gateway}:$arg{port}/AVIS/postSuspectSample";
    my $headers = new HTTP::Headers %inheader;
    my $request = new HTTP::Request "POST", $url, $headers, $content;
    my $response = new HTTP::Response;
    &transaction(\$request,\$response,\%arg);

    # Extract the output headers from the response message into a
    # hash.

    my %outheader = hashify($response->headers_as_string);

    # Check for errors at each layer.  If an error is found at any
    # layer, log it and return a non-zero completion code.

    &out("$arg{gateway} postSuspectSample: network error, " . $response->status_line . "\n"), return 1 
	if $response->is_error;

    &out("$arg{gateway} postSuspectSample: redirected\n"), return 2 
	if $response->is_redirect;

    &out("$arg{gateway} postSuspectSample: gateway error, $outheader{'X-ERROR'}\n"), return 3
	if $response->is_success && $outheader{'X-ERROR'};

    # If no errors are found at any layer, return a zero completion code.

    &out("$arg{gateway} postSuspectSample: OK, $arg{elapsed} seconds, state is $outheader{'X-ANALYSIS-STATE'}\n");
    return 0;
}

#----------------------------------------------------------------------
# This function is called to test definition publishing.
#----------------------------------------------------------------------

sub getSuspectSample (%)
{
    &out("$arg{gateway} getSuspectSample: not supported\n");
    return 10;

    # The arguments for this function are the program's command arguments.

    my (%arg) = @_;

    # Create a hash for the sample import input headers, and include
    # the required headers.

    my %inheader = ();
    $inheader{'ACCEPT'} = "application/*";

    # Create a request message for the test submission and 
    # send it to the gateway.

    my $url = new URI::URL "$arg{protocol}://$arg{gateway}:$arg{port}/AVIS/getSuspectSample";
    my $headers = new HTTP::Headers %inheader;
    my $request = new HTTP::Request "GET", $url, $headers;
    my $response = new HTTP::Response;
    &transaction(\$request,\$response,\%arg);

    # Extract the output headers from the response message into a
    # hash.

    my %outheader = hashify($response->headers_as_string);

    # Check for errors at each layer.  If an error is found at any
    # layer, log it and return a non-zero completion code.

    &out("$arg{gateway} getSuspectSample: network error, " . $response->status_line . "\n"), return 1 
	if $response->is_error;

    &out("$arg{gateway} getSuspectSample: redirected\n"), return 2 
	if $response->is_redirect;

    &out("$arg{gateway} getSuspectSample: gateway error, $outheader{'X-ERROR'}\n"), return 3
	if $response->is_success && $outheader{'X-ERROR'};

    # If no sample content was returned, then the transaction was
    # successful and there are no samples enqueued at the gateway.

    &out("$arg{gateway} : OK, $arg{elapsed} seconds, no samples enqueued\n"), return 0
	unless $outheader{'CONTENT-LENGTH'};

    # Verify that the content datatype is "application/x-*" without being
    # too picky.

    my $datatype = $outheader{'CONTENT-TYPE'};
    &out("$arg{gateway} getSuspectSample: content error, unknown content type\n"), return 4
	 unless $datatype =~ m/application\/x-.+/i;

    # Verify that the content checksum was calculated with the MD5 method.

    my $method = $outheader{'X-CHECKSUM-METHOD'};
    &out("$arg{gateway} getSuspectSample: content error, unknown checksum method \"$method\"\n"), return 4
	unless $method eq "md5";

    # Verify that the actual and expected size of the content match.
    
    my $sizeContent = length($response->content);
    my $sizeHeader = $outheader{'CONTENT-LENGTH'};
    &out("$arg{gateway} getSuspectSample: content error, incorrect content length \n"), return 4
	unless $sizeContent==$sizeHeader;

    # Verify that the actual and expected checksums of the content match.

    my $content = $response->content;
    my $digestContent = uc(MD5->hexhash($content)); 
    my $digestHeader = $outheader{'X-CONTENT-CHECKSUM'};
    &out("$arg{gateway} getSuspectSample: content error, incorrect content checksum\n"), return 4
	unless uc($digestContent) eq uc($digestHeader);

    # Unscramble the content, if necessary.

    my $method = $outheader{'X-CONTENT-SCRAMBLING'};
    if ($method)
    {
	&out("$arg{gateway} getSuspectSample: content error, unknown scrambling method\n"), return 4
	    unless $method eq "xor-vampish";
	$content = &scramble(\$content);
    }

    # Decompress the content, if necessary.

    my $method = $outheader{'X-CONTENT-COMPRESSION'};
    if ($method)
    {
	&out("$arg{gateway} getSuspectSample: content error, unknown compression method\n"), return 4
	    unless $method eq "deflate";
	$content = Compress::Zlib::uncompress(\$content);
    }

    # Verify that the actual and expected size of the sample match.
    
    my $sizeContent = length($content);
    my $sizeHeader = $outheader{'X-SAMPLE-SIZE'};
    &out("$arg{gateway} getSuspectSample: content error, incorrect sample length $sizeContent vs. $sizeHeader\n"), return 4
	unless $sizeContent==$sizeHeader;

    # Verify that the actual and expected checksums of the sample match.

    my $digestContent = uc(MD5->hexhash($content)); 
    my $digestHeader = $outheader{'X-SAMPLE-CHECKSUM'};
    &out("$arg{gateway} getSuspectSample: content error, incorrect sample checksum\n"), return 4
	unless uc($digestContent) eq uc($digestHeader);

    # Store the sample in the local directory, if necessary.

    if ($arg{sample})
    {
	my $filename = "$outheader{'X-SAMPLE-CHECKSUM'}.$outheader{'X-SAMPLE-EXTENSION'}";
	open SAMPLE, ">$filename" or die "cannot open file $filename, $!";
	binmode SAMPLE;
	print SAMPLE $content or die "cannot write file $filename, $!";
	close SAMPLE;
    }

    # If no errors are found at any layer, return a zero completion code.

    &out("$arg{gateway} getSuspectSample: OK, $arg{elapsed} seconds, sample \"$outheader{'X-SAMPLE-FILE'}\" found\n");
    return 0;
}

#----------------------------------------------------------------------
# This function handles one complete HTTP transaction with the gateway.
# it sends a reqeuest message, then waits for and receives the response 
# message.  If tracing is enabled, the request and response are 
# written to the log file.
#----------------------------------------------------------------------

sub transaction ($$$)
{
    # The parameters for this function are references to HTTP request 
    # and response objects.

    my ($requestRef,$responseRef,$argRef) = @_;

    # Set the HTTP protocol level explicitly.

    $$requestRef->protocol("HTTP/1.0");

    # Add several standard headers to the HTTP request object that
    # ensure the communications session will be terminated when the
    # transaction is complete, and that any proxy systems between
    # the local machine and the gateway do not cache its messages.

    $$requestRef->header("Connection" => "close",
			 "Date"       => HTTP::Date::time2str(),
			 "Pragma"     => "no-cache",
			 "User-Agent" => "tickleGate",
			 );
    
    # If the program is configured to trace HTTP messages, record
    # the request headers in the log.
    
    &log(">>>>> HTTP request\n",
	 $$requestRef->method . " ",
	 $$requestRef->url . " ",
	 $$requestRef->protocol  . "\n",
	 $$requestRef->headers_as_string,
	 "[request content size is " . length($$requestRef->content) . " bytes]\n",
	 $$requestRef->header('Content-Type') =~ m/^text/i ? 
	    $$requestRef->content : 
	    length($$requestRef->content)>0 ? "[request content not text]\n" : "",
	 ) if $$argRef{trace};
    
    # Create and initialize an HTTP client object for this transaction.

    my $userAgent = new OverriddenUserAgent; ### instead of "new LWP::UserAgent"
    $userAgent->timeout($$argRef{timeout});

    # Send the request message to the gateway, then wait for the
    # response message and save it. Calculate the elapsed time for the
    # transaction in seconds, and save it in the caller's argument
    # hash.

    my $start = time();
    $$responseRef = $userAgent->request($$requestRef);
    $$argRef{elapsed} = time() - $start;

    # If the program is configured to trace HTTP messages, record
    # the response headers in the log.

    &log("<<<<< HTTP response received after $$argRef{elapsed} second(s)\n",
	 $$responseRef->protocol . " ",
	 $$responseRef->code . " ",
	 $$responseRef->message  . "\n",
	 $$responseRef->headers_as_string,
	 "[response content size is " . length($$responseRef->content) . " bytes]\n",
	 $$responseRef->header('Content-Type') =~ m/^text/ ? 
	    $$responseRef->content :  
	    length($$responseRef->content)>0 ? "[response content not text]\n" : "",
	 ) if $$argRef{trace};
}

#----------------------------------------------------------------------
# This function scrambles a string by XORing it with a constant
# mask picked by Morton for his own impenetrable reasons.
#----------------------------------------------------------------------

sub scramble ($)
{
    # The argument for this function is a reference to a string.

    my ($contentRef) = @_;

    # This function scrambles the string by XORing it with a 
    # constant mask.

    my $scrambleMask = "YAKVAMPISHUCHEGOZHEBOLE..";
    my $scrambleSize = length($scrambleMask);
    my $contentSize = length($$contentRef);
	
    # Work through the string in chunks, XORing each string chunk
    # with the mask and collecting the results.

    my $result = "";
    for (my $i=0; $i<$contentSize; $i+=length($scrambleMask))
       { $result .= substr($$contentRef,$i,$scrambleSize) ^ $scrambleMask;  }

    # Set the length of the result to match the length of
    # the string, truncating any excess data from the last
    # XORed chunk.

    substr($result,$contentSize) = "";
    return $result;
}

#----------------------------------------------------------------------
# This function converts a string containing HTTP headers into an
# associative array.
#----------------------------------------------------------------------

sub hashify ($)
{
    # The argument for this function is a string containing HTTP
    # headers in standard format.

    my ($headers) = @_;

    # Parse the string into lines, and then parse each line into
    # an attribute name and value.  Capitalize the first letter of
    # each word in the name, and then add the name/value pair to
    # an associative array.

    my %attributes = ();
    foreach (split /[\r\n]/,$headers)
       { 
	   my ($name,$value) = /^([a-zA-Z0-9\-]+): *([^\n\r\f]*)/; 
	   $attributes{uc($name)} = $value if $name;
       }
    ###foreach (sort keys %attributes) { print "attributes{$_}: $attributes{$_}\n"; }

    # Return the associative array as the value of this function.

    return %attributes;
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
# This function is called when a message is to be displayed on 
# the console and written to the log.
#----------------------------------------------------------------------

sub out (@)
{
    &log(@_);
    print @_;
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

    # Format the current date and time into compact strings for use
    # in timestamping the message, and also for naming the log file.

    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time());
    my $timestamp = sprintf "%02d-%02d %02d:%02d:%02d", 1+$mon, $mday, $hour, $min, $sec;
    my $padding = " " x length($timestamp);

    # Compose a separator line, if necessary ..........

    my $logfile = $output . "tickleGate.log";
    ###my $separator = -f $logfile && !$main::logged ? "\\/"x40 . "\n\n" : "";
    ###$main::logged = 1;

    # Write the entire message into the log file, prepending the
    # current date and time to the first line, and padding of
    # equivalent length to all subsequent lines.

    open LOG, ">>$logfile" or die "cannot open file $logfile, $!\n";
    print LOG "$timestamp " . join("\n$padding ",@message) . "\n" 
	or die "cannot write file $logfile, $!\n";
    close LOG;      
}
