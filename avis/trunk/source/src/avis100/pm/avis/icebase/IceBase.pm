=head1 NAME

Package: AVIS::IceBase

Filename: pm/AVIS/IceBase/IceBase.pm

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

package AVIS::IceBase;

# This package exports several public functions:

require Exporter;
$VERSION = 1.10;

@ISA = qw(Exporter);
@EXPORT = ();
@EXPORT_OK = qw(restart counter profile logfile config event error fatal notify launch cleanup hostname timestamp);

# This package uses several public Perl packages, which must be installed 
# in the Perl library directory of the machine where this program executes:

use Net::Domain ();
use Net::SMTP ();
use strict;

#----------------------------------------------------------------------
# ... global variables in this package's variable space ...
#----------------------------------------------------------------------

my %configuration = ();
my $profileName = "";
my $logfileName = "";
my $previousMessage = "???";
my $previousError = "???";
my $cleanupFunctionRef = "";
my $restartCounter = 100;
my $programModified;
my $profileModified;
my $hostDomainName;

#----------------------------------------------------------------------
# 
#----------------------------------------------------------------------

sub counter ()
{
  $restartCounter--;
}

=head2 restart

This function is called to check the file this program was loaded
from.  If the file has not changed since the last time it was checked,
there is nothing for this function to do now.  If the file has
changed, the program is re-started from the beginning and this
function does not return.  And, if the profile contains a program
lifetime, this number is decremented and if zero the program is
re-started.

=cut

sub restart () {
    # If the file this program was loaded from has not changed and the
    # program lifetime counter has not reached zero, there is nothing 
    # for this function to do.

    my $filename = $0;
    $programModified = (stat($filename))[9] unless defined $programModified;
    return if $programModified==(stat($filename))[9] && $restartCounter>0;

    # Perform any system-level cleanup needed by this program before
    # restarting.

    &$cleanupFunctionRef("restart") if $cleanupFunctionRef;

    # If the file this program was loaded from has changed, the
    # program is re-started from the beginning.
    
    event("$filename @ARGV restarting ...\n");
    my $rc = exec "perl", $filename,  @ARGV;

    # If the "exec" function fails and does not return an error
    # completion code, do not continue.

    fatal("exec \"perl $filename\" failed, rc=$rc, $!\n");
    exit;
}

=head2 profile


This function is called to check the profile for this program.
If the profile does not yet exist, it is created and the current
values of the program's configuration parameters are written into 
it.  If the profile does exist, and it has changed since the last 
time it was checked, it is read and the values of the program's 
configuration parameters are replaced.

=cut

sub profile (;$%) {
    my ($name,%defaults) = @_;

    # Save the parameters in global variables, if specified.
    
    $profileName = $name if $name;
    fatal("cannot find profile $profileName\n") unless -f $profileName;    

    # ...
    my $parameter;
    foreach $parameter (keys %defaults) 
       { $configuration{$parameter} = $defaults{$parameter}; }

    # If the profile has not changed since the last time we checked, 
    # there is nothing to do.

    return if (stat($profileName))[9]==$profileModified;

    # If the profile does not exist or has changed since the last time we
    # checked it, load its contents into an associative array.

    event("re-loading profile $profileName\n") if $profileModified;
    open PROFILE,"<$profileName" or fatal("cannot open $profileName, $!\n");
    while (<PROFILE>) 
       { $configuration{$1} = $2 if /^[ \t]*([a-zA-Z0-9\[\]]+)[ \t]*=[ \t]*([^\n\r\f]*)/; }
    close PROFILE;	
    event("re-loaded profile $profileName\n") if $profileModified;

    # Save the profile's current timestamp in a global variable so 
    # that it can be checked for change the next time this function
    # is called.

    $profileModified = (stat($profileName))[9];
}

=head2 config

This function returns the value of a specified configuration 
parameter.

The second parameter makes a missing configuration parameter
NON-FATAL. If it is missing or set to 0, config will stop the program
if it can't find the configuration parameter.

=cut

sub config ($;$) {
    my ($parameter, $dontcare) = @_;
    
    fatal("parameter \"$parameter\" missing in profile $profileName\n") 
	unless defined $configuration{$parameter} || $dontcare;
    return $configuration{$parameter};
}

=head2 logfile

=cut

sub logfile ($) {
    my ($name) = @_;

    $logfileName = $name;
}

=head2 event

This function is called when a significant event has occurred.  It
writes the first line of the specified message to standard output,
and then writes all of the lines of the message into the log file, 
prefixed with the current date and time.

=cut

sub event (@) {
    # The parameters for this function are one or more lines of 
    # messages, separated by newline characters, specified as a 
    # list of one or more list items.  To make sense of this, 
    # concatenate the list into one long string and then split
    # the string at the newline characters to make a sensible list.

    my @message = split(/\n/, join("",@_));

    # Write the first line of the message to standard output, unless 
    # its identical to the previous message and we are configured
    # to show repeated messages.

    return 0 unless $previousMessage ne $message[0] || config('repeatedMessages');
    $previousMessage = $message[0];

    my ($date0,$date1,$time1) = timestamp();
    print "$time1 $message[0]\n";

    # If no profile has been loaded yet, then don't try to write
    # this event into a log file.

    return 1 unless $profileName;

    # Figure out what the log filename should be and create its
    # directory, if necessary.

    unless ($logfileName)
    {
	($logfileName) = $0 =~ m/([^\\]+)\.\w+$/;
    }

    my $directory = config('logDirectory');
    _create($directory) or return 0;
    $directory = $directory."\\" if $directory;

    # Write the whole message to the log file, prefixing the first line
    # with the current date and time and all remaining lines with a 
    # padding string of equal length.

    my $padding = " " x length("$date1 $time1");
    my $filename = "$directory$logfileName.log.$date0";
#    open FILE,">>$filename" or die "cannot open $filename, $!, stopped";
#    print FILE "$date1 $time1 " . join("\n$padding ",@message) . "\n" 
#	or die "cannot write $filename, $!, stopped";
#    close FILE;	
    if (open FILE,">>$filename") {
      print FILE "$date1 $time1 " . join("\n$padding ",@message) . "\n" 
	or die "cannot write $filename, $!, stopped";
    close FILE;	
    } else {
      warn "cannot open $filename, $!";
    } 
    
    return 1;
}

#----------------------------------------------------------------------
# This function ...
# 
#----------------------------------------------------------------------

sub _create ($)
{
    # The parameter for this function is a directory path to be
    # created, if necessary.
    
    my ($directory) = @_;
    return 1 if $directory eq "";
    return 1 if -d "$directory";
    return 1 if -d "$directory\\";

    my ($root,$stem) = $directory =~ m/(.*)\\(.+)$/;
    _create($root) or return 0;

    # Try to create the last directory in the path, record our success or
    # failure, and return an indication of success or failure to our 
    # caller.

    mkdir $directory, \0777 or print("cannot create $directory, $!\n"), return 0;
    return 1;
}

=head2 timestamp

This private function returns the current date and time, according 
to the local system clock, in several popular formats.

=cut

sub timestamp (;$) {
    my ($when) = @_;

    $when = time unless $when;

    # Convert the current date and time into an array of familiar
    # integers, and construct an array of strings containing them 
    # in the formats "mmmddyyyy" and "mm/dd" and "hh:mm:ss".

    my @months = qw(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec);
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($when);
    my $cyear = $year + ($year<70 ? 2000 : 1900);

    return 
	(sprintf "%s%02d%04d", $months[$mon],$mday,$cyear),
	(sprintf "%02d/%02d", 1+$mon, $mday),
	(sprintf "%02d:%02d:%02d", $hour, $min, $sec),
    ;
}

=head2 error

This function is called when an error has occurred.  It logs the
specified error message, writes it in an "ERRROR" companion file,
and notifies the user.

=cut

sub error (@) {
    my @message = @_;

    # Write the first line of the message to standard output, unless 
    # its identical to the previous message and we are configured
    # to show repeated messages.

    return 0 unless $previousError ne $message[0] || config('repeatedMessages');
    $previousError = $message[0];

    event("ERROR: ", @message) or return 0;
    return 0;
}

=head2 cleanup

=cut

sub cleanup ($) {
    ($cleanupFunctionRef) = @_;
}

=head2 fatal

This function is called when a fatal error has occurred.  It logs 
the specified error message, notifies the user,  and terminates 
the program.

=cut

sub fatal (@) {
    # The parameter for this function is an error message.

    my @message = @_;

    # Log the message, notify the user, and terminate the program.

    event("FATAL: ", @message);
    notify("FATAL: ", @message);
    &$cleanupFunctionRef("fatal") if $cleanupFunctionRef;
    exit(1);
}

=head2 notify

This function is called when the user s attention is required.  It
sends a message via email to an address specified in a configuration
parameter. 

The parameters for this function are one or more lines of 
messages, separated by newline characters, specified as a 
list of one or more list items.  To make sense of this, 
concatenate the list into one long string and then split
the string at the newline characters to make a sensible list.

=cut

sub notify (@) {
  my @body = split(/\n/, join("",@_));

  # Construct values needed for SMTP headers.  Unless an email
  # gateway and address are specified, there is no point in trying
  # to send a message.    

  my $gateway = config('emailGateway');
  my $to = config('emailAddress');
  return 1 unless $to && $gateway && hostname();
  my ($basename) = $0 =~ m/([\w $_]+)\.\w+/;
  my $from = $basename . "\@" . hostname();
  my ($date0,$date1,$time1) = timestamp();
  event("sending notice to $to from $from\n");
    
  # Construct the SMTP header lines.  For diagnostic purposes, 
  # repeat the headers in the body of the message.

  my @headers;  
  push @headers, "To: $to\n";
  push @headers, "From: $from\n";
  push @headers, "Date: $date1 $time1\n";
  push @headers, "Subject: $body[0]\n";
  push @headers, "\n";
  push @headers, "to $to\n";
  push @headers, "from $from\n";
  push @headers, "about $body[0]\n";
  push @headers, "via $gateway\n";
  push @headers, "at $date1 $time1\n";
  push @headers, "\n";

  # Send the message to the configured email address via the
  # configured email gateway.

  my $smtp = new Net::SMTP($gateway, Timeout=> config('emailTimeout'))
    or event("cannot connect to $gateway\n"), return 0;
  # MS: $program may be defined in the mail program, but that doesn't
  # seem to be the case in practice.
#  $smtp->mail($program) or event("cannot identify to $gateway\n"), return 0;
  $smtp->mail("DryIce") or event("cannot identify to $gateway\n"), return 0;
  $smtp->to($to) or event("cannot address to $gateway\n"), return 0;
  $smtp->data(@headers, @body) or event("cannot send to $gateway\n"), return 0;
  $smtp->quit or event("cannot disconnect from $gateway\n"), return 0;
  event("sent notice to $to from $from\n");
  return 1;
}

=head2 hostname

This private function returns the fully-qualified TCP/IP domain name 
for the computer on which the program is running. 

=cut

sub hostname () {
  # If the host name has already been resolved by an earlier
  # call to this function, just return the saved host name.

  return $hostDomainName if $hostDomainName;
  
  # Resolve the fully-qualified TCP/IP domain name for this
  # computer, save it in a global variable for future calls
  # to this function, and return the value as the host name.

  event("resolving hostname\n");
  $hostDomainName = Net::Domain::hostfqdn();
  event("resolved hostname to $hostDomainName\n");
  return $hostDomainName;
}


######################################################################

1;

=head1 HISTORY

=head1 BUGS

=head1 SEE ALSO

=cut


