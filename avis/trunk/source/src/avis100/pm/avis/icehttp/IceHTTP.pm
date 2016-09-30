=head1 NAME

Package: AVIS::IceHTTP

Filename: pm/AVIS/IceHTTP/IceHTTP.pm

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1998-1999 

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHOR

Edward Pring, Morton Swimmer

=head1 DESCRIPTION

=cut

package AVIS::IceHTTP;

# This package exports several public functions:

require Exporter;
$VERSION = 1.10;
@ISA = qw(Exporter);
@EXPORT = qw();
@EXPORT_OK = qw(headFile getFile getSuspectSample postSampleStatus postSignatureSet putSummary try transaction parse);

# This package uses several public Perl packages, which must be installed 
# in the Perl library directory of the machine where this program executes:

use Win32 ();
#use LWP::Debug qw(+); # enable all low-level tracing options ...
use LWP::UserAgent ();
use MIME::Base64 ();
use URI::URL ();
use HTTP::Date ();
use HTTP::Request ();
use HTTP::Response ();
use strict;

# This package uses several private Perl packages, which must be installed
# in the current directory of the machine where this program executes:

use AVIS::IceBase ();
use AVIS::IceFile ();

#----------------------------------------------------------------------
# This package uses these global variables...
#----------------------------------------------------------------------

my $userAgent;

#----------------------------------------------------------------------
# This function is called to get a file, unless it hasn't changed 
# since the last time it was gotten.  The file to get is specified
# as a fully-qualified URL, including scheme and server and path.
# If a timestamp previously returned by the server is also specified,
# and the file has not changed since then, the file is not gotten 
# again.  If the function succeeds, it returns a list containing
# the file's timestamp and content.
#----------------------------------------------------------------------

sub headFile ($;$)
{
    # The arguments for this function are a URL for a file to get,
    # and optionally a timestamp, meaning get the file only if newer.

    my ($url,$timestamp) = @_;
    return _getheadFile("HEAD", $url, $timestamp);
}

#----------------------------------------------------------------------
# 
#----------------------------------------------------------------------

sub getFile ($;$)
{
    # The arguments for this function are a URL for a file to get,
    # and optionally a timestamp, meaning get the file only if newer.

    my ($url,$timestamp) = @_;
    return _getheadFile("GET", $url, $timestamp);
}

#----------------------------------------------------------------------
# 
#----------------------------------------------------------------------

sub _getheadFile ($$;$) {
  # The arguments for this function are an HTTP method ("get" or 
  # "head"), a URL for a file to get or head, and optionally a timestamp, 
  # meaning get the file only if newer.

  my ($method,$url,$oldtime) = @_;
  AVIS::IceBase::event((uc($method) eq "HEAD" ? "checking" : "getting") ." file $url\n"); 

  # Create an HTTP request ...

  my $url1 = new URI::URL $url;
  my $headers = new HTTP::Headers("Accept"=>"*/*");
  $headers->header("If-Modified-Since"=>HTTP::Date::time2str($oldtime)) 
    if $oldtime;
  my $request = new HTTP::Request $method, $url1, $headers;
  my $response = new HTTP::Response;
  transaction(\$request,\$response) or return 0, "";

  # Revise the file's time, if the server returned one.

  my $newtime = HTTP::Date::str2time($response->header("Last-Modified"))
    if $response->header("Last-Modified");

  # ...
    
  #printf "code=%s, oldtime=$oldtime, newtime=$newtime, length(body)=%s\n", $response->code, length($response->content);

  AVIS::IceBase::event(uc($method) eq "HEAD" ? "checked\n" : "gotten\n"), return ($newtime, $response->content)
    if $response->code==200;
  AVIS::IceBase::event("unchanged\n"), return ($newtime,"")
    if $response->code==304;
  AVIS::IceBase::event("not found\n"), return (0,"")
    if $response->code==404;

  # ...

  return (0,"");
}

=head2 getSuspectSample

The parameter for this function is the gateway to poll for samples.

=cut

sub getSuspectSample ($$) {
  my ($directory,$gateway) = @_;
  AVIS::IceBase::fatal("cannot find directory $directory\n") 
    unless -d $directory;
  my $address =  AVIS::IceBase::config("gatewayAddress[$gateway]");
  AVIS::IceBase::fatal("cannot find address for gateway $gateway\n") 
    unless $address;

  # Create an HTTP request for getting the next sample into the lab
  # and send it to the gateway.  Save the reply from the gateway
  # in an HTTP response.

  my $url = new URI::URL "$address/AVIS/getSuspectSample";
  my $headers = new HTTP::Headers("Accept"=>"application/x-ibmav, text/*");
  my $request = new HTTP::Request "GET", $url, $headers;
  my $response = new HTTP::Response;
  transaction(\$request,\$response) or return "";
  AVIS::IceBase::error("$gateway returned $response->code $response->message"), return ""
    unless $response->is_success;

  # If the transaction completes successfully but there is no cookie
  # header in the response, then there is nothing for this function to
  # do now.

  my $cookie = $response->header('X-Analysis-Cookie');
  return "" unless $cookie;
  AVIS::IceBase::event("received sample $cookie from gateway $gateway\n");

  # Create an associative array containing all of the headers in 
  # the response message.  This really should be a method of the
  # HTTP::Headers class.

  my %headers = (); 
  my $line;
  foreach $line (split /\n/,$response->headers_as_string) { 
    my ($header,$value) = $line =~ m/^(\S+): (.*)$/;
    $headers{$header} = $value;
  }

  # Combine the gateway's name and cookie to produce a local cookie
  # that is unique.
    
  my $bigcookie = "$gateway-$cookie";

  # Create a file in the receive directory, using the local cookie
  # as a filename and "VS" as the extension.  Write the
  # headers from the response message into this file, followed by the
  # body of the sample.

  my $samplefile = "$directory\\$bigcookie.VS";
  AVIS::IceBase::error("duplicate sample $cookie received from $gateway\n") 
    if -f $samplefile;
  AVIS::IceBase::event("storing sample $bigcookie in $samplefile\n");
  unlink $samplefile if -f $samplefile;
  AVIS::IceFile::writehttp($samplefile, $response->content_ref, %headers);
  AVIS::IceBase::event("stored sample $bigcookie in $samplefile\n");
  undef $response;
  
  return $bigcookie;
}    

=head2 postSampleStatus

This function is called to send the contents of the status file to 
the gateway the sample was obtained from.

The arguments for this function are a cookie for a sample plus
an associative array of status attributes.

=cut

sub postSampleStatus ($;%) {
  my ($filename,%newAttributes) = @_;

  # ...

  my %attributes = ();
  AVIS::IceFile::readhttp($filename,undef,\%attributes) or return 0;

  # Merge the new attributes with the old attributes.  That is,
  # replace the values of any existing attributes, and add all
  # new attributes to the associative array.
  my $attribute;
  foreach $attribute (keys %newAttributes) { 
    $attributes{$attribute} = $newAttributes{$attribute}; 
  }

  # ...

  my $cookie = $attributes{'X-Analysis-Cookie'};
  my $state = $attributes{'X-Analysis-State'};
  my $stage = $attributes{'X-Analysis-Stage'};
  my $gateway = $attributes{'X-Gateway-Name'};
  my $address = $attributes{'X-Gateway-Address'};

  # ...

  AVIS::IceBase::event("sending status for sample $gateway-$cookie\n"); 
  AVIS::IceBase::event("state=$state stage=$stage\n"); 
  my $url = new URI::URL "$address/AVIS/postSampleStatus?cookie=$cookie";
  my $headers = new HTTP::Headers %attributes;
  my $request = new HTTP::Request "POST", $url, $headers;
  my $response = new HTTP::Response;
  transaction(\$request,\$response) or return 0;
  AVIS::IceBase::event("sent status for sample $gateway-$cookie\n"); 
  return 1;
}

=head2 putSummary

This function is called to put an HTML file on a gateway.

The parameters for this function are the name of the gateway
and an array containing the HTML file s content.

=cut

sub putSummary ($$@) {
  my ($gateway,$page,@summary) = @_;

  # Get the address of the gateway, and concatenate the HTML array
  # into a string.

  my $address = AVIS::IceBase::config("gatewayAddress[$gateway]");
  my $body = join "", @summary;

  # Construct the basic HTTP attributes for the content.

  my %attributes = ('Content-Length' => length($body),
		    'Content-Type' => 'text/html',
		   );

  # Put the file on the gateway.

  AVIS::IceBase::event("sending page $page to gateway $gateway\n"); 
  my $url = new URI::URL "$address/AVIS/putSummary/$page";
  my $headers = new HTTP::Headers %attributes;
  my $request = new HTTP::Request "PUT", $url, $headers, $body;
  my $response = new HTTP::Response;
  transaction(\$request,\$response) or return 0;
  AVIS::IceBase::event("sent page $page to gateway $gateway\n"); 
  return 1;
}

=head2 postSignatureSet

This function is called to send a set of signatures to a 
gateway.  The signatures have already been packed into a ZIP 
file and stored in the "signatures" directory.

The parameters for this function are the filename of a ZIP file to
be sent to a gateway.

=cut

sub postSignatureSet ($$) {
  my ($filename,$gateway) = @_;
  AVIS::IceBase::fatal("signatures $filename not found\n") 
    unless -f $filename;
  my $address =  AVIS::IceBase::config("gatewayAddress[$gateway]");
  AVIS::IceBase::fatal("cannot find address for gateway $gateway\n") 
    unless $address;

  # Parse the ZIP filename into components.

  my ($server,$share,$drive,$path,$identifier,$extension) = AVIS::IceFile::parse($filename);
  AVIS::IceBase::event("sending signatures $identifier.ZIP to $gateway\n");

  # Read the ZIP file's attributes, if it has a status file.

  my %attributes = ();
  my $statusfile = "$drive$path$identifier.SSA";
  AVIS::IceFile::readhttp($statusfile,undef,\%attributes);

  # Read the entire contents of the ZIP file into a scalar variable.    
    
  my $zip = "";
  my $size = (stat($filename))[7];
  open FILE,"<$filename" 
    or AVIS::IceBase::fatal("cannot open $filename, $!\n");
  binmode FILE;
  my $length = read FILE, $zip, $size 
    or AVIS::IceBase::fatal("cannot read $filename, $!\n");
  close FILE;
  AVIS::IceBase::fatal("only read $length of $size bytes from $filename\n") 
    unless $length==$size;

  # Add more attributes for the ZIP file.

  $attributes{'Content-Type'} = "application/x-zip";
  $attributes{'Content-Length'} = $length;
  $attributes{'X-Content-Checksum'} = AVIS::IceFile::md5digest($filename);
  $attributes{'X-Date-Modified'} = HTTP::Date::time2str((stat($filename))[9]);
  $attributes{'X-Date-Created'} = HTTP::Date::time2str((stat($filename))[10]);
  $attributes{'X-Signatures-Identifier'} = $identifier;
    
  # Create an HTTP request for sending the signature set send it to 
  # the lab's gateway.  Save the reply from the gateway in an HTTP response.
    
  my $url = new URI::URL "$address/AVIS/postSignatureSet?identifier=$identifier";
  my $headers = new HTTP::Headers %attributes;
  my $request = new HTTP::Request "POST", $url, $headers, $zip;
  my $response = new HTTP::Response;
  transaction(\$request,\$response) or return 0;

  AVIS::IceBase::event("sent signatures $identifier.ZIP to $gateway\n");
  return 1;
}

=head2 transaction

This function handles one complete HTTP transaction with the gateway
for the lab.  That is, it sends a reqeuest message, then waits for
and receives the response message.

The parameters for this function are references to HTTP request 
and response objects.

=cut

sub transaction ($$) {
  my ($requestRef,$responseRef) = @_;

  # ...

  AVIS::IceBase::counter() if Win32::IsWinNT;

  # Set the HTTP protocol level explicitly.

  $$requestRef->protocol("HTTP/1.0");

  # Add several standard headers to the HTTP request object that
  # ensure the communications session will be terminated when the
  # transaction is complete, and that any proxy systems between
  # the local machine and the gateway do not cache its messages.

  $$requestRef->header("Connection" => "close",
		       "Date"       => HTTP::Date::time2str(),
		       "Pragma"     => "no-cache",
		       "User-Agent" => _agent(),
		      );
    
  # If the program is configured to trace HTTP messages, record
  # the request headers in the log.
    
  AVIS::IceBase::event(">>>>> HTTP request\n",
		       $$requestRef->method . " ",
		       $$requestRef->url . " ",
		       $$requestRef->protocol  . "\n",
		       $$requestRef->headers_as_string,
		       "[request content size is " . length($$requestRef->content) . " bytes]\n",
		       $$requestRef->header('Content-Type') =~ m/^text/ ? 
		       $$requestRef->content : 
		       length($$requestRef->content)>0 ? "[request content not text]\n" : "",
		      ) if AVIS::IceBase::config('traceHTTP');
    
  # Create and initialize an HTTP client object for this transaction
  # with the lab's gateway.

  unless ($userAgent) {
    $userAgent = new LWP::UserAgent;
    $userAgent->agent(_agent() . " " . $userAgent->agent);
    $userAgent->timeout(AVIS::IceBase::config('httpTimeout'));
    $userAgent->from(AVIS::IceBase::config('emailAddress'))
      if AVIS::IceBase::config('emailAddress');
    $userAgent->proxy(['http','ftp'],AVIS::IceBase::config('proxyAddress')) # ??? ."/")
      if AVIS::IceBase::config('proxyAddress');
  }
  
  # Send the request message to the gateway, then wait for the
  # response message and save it.  And calculate the elapsed
  # time for the transaction in seconds.

  my $start = time;
  $$responseRef = $userAgent->request($$requestRef);
  my $elapsed = time - $start;

  # If the program is configured to trace HTTP messages, record
  # the response headers in the log.
  
  AVIS::IceBase::event("<<<<< HTTP response received after $elapsed second(s)\n",
		       $$responseRef->protocol . " ",
		       $$responseRef->code . " ",
		       $$responseRef->message  . "\n",
		       $$responseRef->headers_as_string,
		       "[response content size is " . length($$responseRef->content) . " bytes]\n",
		       $$responseRef->header('Content-Type') =~ m/^text/ ? 
		       $$responseRef->content :  
		       length($$responseRef->content)>0 ? "[response content not text]\n" : "",
		      ) if AVIS::IceBase::config('traceHTTP');
  
  # If the transaction was successful, this function is finished.
  # If this transaction was not successful, record the new error 
  # in the log and notify the user.
  
  return 1 if $$responseRef->code==200;
  return 1 if $$responseRef->code==201;
  return 1 if $$responseRef->code==204;
  return 1 if $$responseRef->code==304;
  return 1 if $$responseRef->code==404;
  
  my $message = $$requestRef->method . " " . 
    $$requestRef->url    . ", " . 
      $$responseRef->code  . " " . 
	$$responseRef->message . "\n";
  AVIS::IceBase::error($message);
  return 0;
}

#----------------------------------------------------------------------
# This function ...
# 
#----------------------------------------------------------------------

sub _agent () {
  my $program = $0;

  my $name = (AVIS::IceFile::parse($program))[4];

  my ($sec,$min,$hour,$mday,$mon,$year) = localtime((stat($program))[9]);
  $year += 100 if $year<70;

  return sprintf "%s/%04d-%02d-%02d", $name, 1900+$year, 1+$mon, $mday;
}

=head2 parse

This function parses a URL into separate components and returns
a list containing the components.  Unnecessary punctuation is
removed from the components before they are returned.

For example, if this URL is given:

=over 8

http://avimmune.watson.ibm.com:80/AVIS/getSampleStatus?cookie=0000123#top

=back

then the list returned will be:

=over 8

index  component  example

-----  ---------- -----------------

  0    method     http

  1    server     avimmune.ibm.com

  2    port       80

  3    path       /AVIS/getSampleStatus

  4    parameter  cookie=0000123

  5    label      top

Note that not all components are present in all URLs.  The function 
returns null values for missing components.

=back

The parameter for this function is a URL.

=cut

sub parse ($) {
  my ($url) = @_;

  # Parse the URL into components using a really ugly regular expression.

  my ($method,$server,$port,$path,$parameter,$label) = 
#    $url =~ m/^(\w+)?:?\/\/([\w.]*)?:?(\d*)?(.*?)\??([\w&%=]*)?#?(\w*)?$/;
    $url =~ m/^(\w+)?:?\/\/([\w.]*)?:?(\d*)?(.*?)\??([\w&%=]*)?\#?(\w*)?$/;

  # Return a list of six items containing the components of the 
  # filename.  Null values are used for missing components.  

  #print "<$url> parses to <$method> <$server> <$port> <$path> <$parameter> <$label>\n";
  return ($method,$server,$port,$path,$parameter,$label);
}

#----------------------------------------------------------------------
# This function ...
#----------------------------------------------------------------------

sub try
{
  # The parameter for this function is a URL to be tested for
  # server availability.  

  my ($url) = @_;

  # Extract the server address and port number from the URL, and
  # try to connect a socket to the server.

  my ($method,$address,$port) = parse($url);
  $port = 80 unless $port;
  my $sock = new IO::Socket::INET(PeerAddr=>$address, 
				  PeerPort=>$port, 
				  Proto=>'tcp', 
				  Timeout=>AVIS::IceBase::config('httpTimeout'),
				 );

  # Display an appropriate message indicating whether or not
  # the socket connected successfully to the server, but only
  # when the state has changed since the last test.  In any
  # case, save the new state and return a completion code
  # indicating whether or not the socket connected successfully.

  AVIS::IceBase::error("cannot connect to $address:$port, $!\n"), return 0 unless $sock;
  AVIS::IceBase::event($sock->sockhost.":".$sock->sockport.
		       " connected to ".
		       $sock->peerhost.":".$sock->peerport."\n") 
    if AVIS::IceBase::config('traceHTTP');
  close $sock;
  return 1;
}

######################################################################

1;

=head1 HISTORY

=head1 BUGS

=head1 SEE ALSO

=cut

