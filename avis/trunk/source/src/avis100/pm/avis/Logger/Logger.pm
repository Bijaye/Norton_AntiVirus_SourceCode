####################################################################
#                                                                  #
# Program name:  pm/AVIS/Logger/Logger.pm                          #
#                                                                  #
# Package name:  AVIS::Logger                                      #
#                                                                  #
# Description:   Log-managment for AVIS                            #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1998-1999                                     #
#                                                                  #
# Author:        Dave Chess                                        #
#                                                                  #
# And also:      U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
# A small piece of OO-Perl that knows how to create and update and #
# find and generally manage logs for AVIS.  See POD stuff at the   #
# end (or use perldoc), or see the official external documentation,#
# for details.                                                     #
#                                                                  #
# Andy Raybould (AJR) 3/11/99:                                     #
#   Added capture method for redirecting filehandles into the log. #
#                                                                  #
# Andy Raybould (AJR) 3/22/99:                                     #
#   Make 'CONSOLE' a class variable.                              #
#                                                                  #
####################################################################

package AVIS::Logger;

use AVIS::FSTools();    # For Canonize() etc
use FileHandle;         # To get a simple storable filehandle for local files
use Fcntl ":flock";     # To get nice flock symbols

#
# AJR 3/22/99: open class stream CONSOLE as a dup of STDERR
#
BEGIN {
   $AVIS::Logger::CONSOLE = new FileHandle(">>&STDERR");
   $AVIS::Logger::CONSOLE->autoflush(1);
}

END {$AVIS::Logger::CONSOLE->close() if defined $AVIS::Logger::CONSOLE;}


#
# The usual oo-perl new() routine
#
sub new {
  my $class = shift;      # the class to bless into
  my $self = {};          # the anonymous hash to return
  bless $self, $class;
  $self->_empty(@_);      # initialize the variables
# and we're done
  return $self;
}

#
# Set all variables pristine; done at new() and DESTROY() times.
#
sub _empty {
  my $self = shift;
  my $unc = undef;          # So I'm paranoid!
  $unc = shift if @_;       # the argument we were given, if any
# Simple instance variables
  $self->{screenEcho} = 0;
  $self->{"localFile"} = "";
  $self->{subcode} = "";
  $self->{unc} = (defined $unc) ? $unc : "" ;
# The "private" instance variables
  $self->{_localHandle} = undef;
  $self->{_warned} = 0;
  return;
}

#
# When a local file is set, make sure it can be opened.
#
sub localFile {
  my $self = shift;

  # undocumented zero-argument query function
  return $self->{"localFile"} if not @_;   # Query function

  # otherwise take the argument
  my $newvalue = shift;

  # return at once if it's already in effect
  return $newvalue if $newvalue eq $self->{"localFile"};

  # otherwise, if a handle is already open, close it
  if (defined $self->{_localHandle}) {
    $self->{_localHandle}->close();
    $self->{_localHandle} = undef;
    $self->{"localFile"} = "";
  }

  # if the argument is "", we're done
  if ($newvalue eq "") {
    $self->{_localHandle} = undef;
    $self->{"localFile"} = "";
    return 1;    # always works!
  }

  # otherwise try to open a new handle
  $self->{_localHandle} = new FileHandle(">> $newvalue");
  $self->{_localHandle}->autoflush(1);

  # error if fails.
  if (not defined $self->{_localHandle}) {
    warn "Error opening local log file [$newvalue].\n";
    $self->{"localFile"} = "";
    return 0;
  }

  # non-error if works
  $self->{"localFile"} = $newvalue;
  return 1;
}

#
# redirect filehandles to the local log
#
sub capture {
   my $self = shift;
   return 0 unless $self->{_localHandle};
   my $nErr = 0; $! = 0;
   foreach my $fh (@_) {
      if ($fh->open ('>&'.$self->{_localHandle}->fileno())) {
         $fh->autoflush(1);
      } else {
         $self->{_console}->print("Logger::capture $fh: $!\n");
         $nErr++;
      };
   }
   return $nErr == 0;
}

#
# The routine that actually logs stuff
#
sub logString {
  my $self = shift;
  my $string = shift;
  $string = $self->_fancyUp($string);
  $AVIS::Logger::CONSOLE->print($string) if $self->{screenEcho} and defined $AVIS::Logger::CONSOLE;   # AJR 3/11/99
  return 0 unless ($self->_logToGlobal($string));
  return 0 unless ($self->_logToLocal($string));
  return 1;
}

#
# Subroutine that logs to the local file, if any
#
sub _logToLocal {
  my $self = shift;
  my $string = shift;
  return 1 if $self->{"localFile"} eq "";
  $self->{_localHandle}->print($string) or return 0;
  return 1;
}

#
# Take a raw string and make it nice for the log.
#
sub _fancyUp {
  my $self = shift;
  my $string = shift;
  my ($s, $min, $h, $d, $mon, $y);
  ($s, $min, $h, $d, $mon, $y) = localtime();
  return sprintf "%04d/%02d/%02d %02d:%02d:%02d $string\n",
                      1900+$y,1+$mon,$d,$h,$min,$s;
}

#
# Subroutine that logs to the global file, if any
#
sub _logToGlobal {
  my $self = shift;
  my $string = shift;
  my $logname = $self->_logName();
  my $tries;
  my $locked;
  my $locks_exist = 1;
  local *FH;

  return 1 if $self->{unc} eq "";

  return 0 unless open FH, ">> $logname";
  for ($tries=0;$tries<5;$tries++) {
    # try to get a lock
    $locked = eval { flock FH, LOCK_EX | LOCK_NB; };
    # if eval failed, flock probably not supported
    if (not defined $locked) {
      warn "flock not supported here; ignored" if not $self->{_warned};
      $self->{_warned} = 1;
      $locks_exist = 0;
      last;      # Continue blithely on
    }
    last if $locked;
    warn "Waiting for log file to be available...";
    sleep(5);
  }
  return 0 if $locks_exist and not $locked;
  unless (print FH $string) {
    close FH;
    flock FH, LOCK_UN if $locked;
    return 0;
  }
  close FH;
  flock FH, LOCK_UN if $locked;

  return 1;
}

#
# Mapping of a unc and subcode to a global filename
#
sub _logName {
  my $self = shift;
  my $token = $self->{subcode} eq "" ? "LOG0" : $self->{subcode};
  return $self->_logPattern($token);
}

#
# Mapping to a global filename, or globbing pattern
#
sub _logPattern {
  my $self = shift;
  my $input = shift;
  my $answer = sprintf "AVIS%s.LOG", $input;
  return AVIS::FSTools::Canonize($answer,$self->{unc});
}

#
# Routine that returns the list of log files
#
sub getLogList {
  my $self = shift;
  my $pattern = $self->_logPattern("*");
  my $answer = [<"$pattern">];   # These quotes are important!!
  return [] if ($answer->[0] eq $pattern);
  return $answer;
}


#
# Called when we go away, or explicitly.  Just close up any open
# handle we might happen to have, and unset variables.
#
sub DESTROY {
  my $self = shift;
  if (defined $self->{_localHandle}) {
    $self->{_localHandle}->close();
    $self->{_localHandle} = undef;
  }
  $self->_empty();
}


#
# Basic C-record-structure imitation for simple instance variables.
# Note that all instance variables must be initialized before this
# routine will work on them; this generally happens in new().  Also,
# variables starting with a _ cannot be accessed this way.
#
sub AUTOLOAD {
  my $self = shift;
  my $type = ref($self) || confess("$self is not an object");
  my $name = $AUTOLOAD;

  $name =~ s/.*://;                 # Extract just the name part
  return undef if $name =~ /^_.*/;  # Hide private variables
  return undef if not exists($self->{$name});
  return $self->{$name} = shift if @_;
  return $self->{$name};
}

1;    # Just in case


__END__

=head1 NAME

AVIS::Logger == Log management for AVIS.

=head1 SYNOPSIS

 # simplest use

 my $logger = new AVIS::Logger($myUNC);
 ...
 $logger->logString("Something has happened!");    # Logged normally
 ...
 return;               # Probably an implicit DESTROY happens here

=head1 DESCRIPTION

=over 2

=item $thislogger = new AVIS::Logger($unc);

returns a new logging object, ready to log things to the
global (per-cookie) log file corresponding to the given
$unc.  If the given $unc is equal to "", the logging object
is connected to no global log file yet, and everything
logged to it will just be thrown away, unless some of the
following calls are used.

=item $thislogger->screenEcho($newvalue);

sets the value of the "screenEcho" switch on the log
object.  If this switch is true, everything logged is also
echoed to the device to which this class' CONSOLE filehandle 
is opened (CONSOLE is a dup of STDERR taken at class
initialization time - i.e. when the compiler evaluated its
BEGIN routine).
  
The switch is initially false in new objects.

=item $success = $thislogger->localFile($filespec);

sets the localFile string of this object to the given
filename.  If this string is set, eveything logged will (in
addition to anywhere else it's going) be written to the
given local file.  Note that this is intended for
single-processing simple local files; the file will be kept
open for write, and if two log objects attempt to log to
the same local file at the same time, something will fail.
Returns true for success, false if the file could not be
opened.  If $filespec is equal to "", resets the localFile
string, so no local logging is done.  The localFile string
is reset in new objects.

=item $success = $thislogger->capture(@filehandles);

The given filehandes will be redirected to the local log
(if it exists) - e.g. $log->capture (STDOUT, STDERR). 
If screenEcho is on, echoing of logged strings will be
done to the device to which STDERR was directed when this
class was initialized (see above).

Regardless of the setting of screenEcho, captured (as 
opposed to logged) output is not echoed to the screen.

=item $thislogger->unc($unc);

sets the unc string of this object to the given string;
future stuff logged to this object will go to the global
(per-cookie) log file corresponding to the given $unc.  If
the given $unc is equal to "", the logging object is made
to be connected to no global log file.

=item $thislogger->subcode($string);

sets the "subcode" string of this object to the given
string, or removes the subcode if the given string is equal
to "".  Newly-created objects have no subcode.  If a
subcode is set, it will be used in determining the name of
the global log file to write to within the set unc.  This
should be used only when necessary (when, for instance, two
different tasks are working on the same unc, but they
cannot both see the same physical log file, and it's
desirable that two different files, both associated with
the same unc, be created for their logs).

=item $success = $thislogger->logString($string);

sends the given string to the log object, to be sent to
whatever place or places the logger is currently set to log
to.  The string will be fancied up (probably with the
current time and date at the start, and a newline at the
end) before being recorded.  Returns true for success,
false for failure (failure means that the global file could
not be opened or could not be written, or the local file
could not be written).  When logging to a global per-cookie
log file, this method opens the file for append, writes the
string, and closes the file again.
If the file was already locked by someone else, this method 
waits a reasonable amount of time and retries before giving
up and returning failure.

=item $thislogger->DESTROY();

causes the log object to close any open local logging file,
and otherwise clean up.  Perl will call this method when
the last reference to the object goes away, so you won't
ordinarily need to call it yourself.  Just do "undef
$thislogger;" or let it go out of scope, or whatever.

=item $loglist = $thislogger->getLogList();

returns a reference to an array of fully-qualified
filenames, which represent the totality of global
(per-cookie) log files associated with the current unc that
are visible in the current environment.  This will
generally be just one file, unless the subcode method has
been used.  Note that this method works by just doing a
wildcarded directory search; if someone has nefariously
slipped in a file with the same sort of name that was not
in fact created by a Logger, its name will be returned
also; there is no magic here!


=head1 EXAMPLES:

 # simplest use

 my $logger = new AVIS::Logger($myUNC);
 ...
 $logger->logString("Something has happened!");    # Logged normally
 ...
 return;                     # Probably an implicit DESTROY happens here


 # More complex, showing off the features

 my $logger = new AVIS::logger("");   # Don't know the UNC yet
 $logger->screenEcho(1);              # but show stuff on the screen
 $logger->localFile("slooty.log");    # and stick it in a local file
 $logger->logString("We are just setting up now...");
 ...
 $logger->unc($unc);                   # Now we know the UNC!
 $logger->logString("Set up OK.");     # This goes to the global log too
 $logger->screenEcho(0);               # turn off the console echo
 $logger->localFile("");               # and the local file
 $logger->logString("All's well");     # Goes just to the per-cookie log
 ...
 undef $logger;                  # cleans up, if no one else has a copy
 ...
 my $loggetter = new AVIS::logger($thatunc);  # get a log object for unc
 $loglist = $loggetter->getLogList();         # ask it for a list of logs
 foreach $thislog ( @$loglist ) {             # for each name returned,
   processLogFileName($thislog);              # do something
 }
 undef $loggetter;                            # and then we're all done


=head1 HISTORY

See VSS or CMVC or whatever.

=head1 BUGS

The behavior of logString() on the global unc log, opening and
writing and closing for every call, has obvious performance
implications.  If we log more than a few lines
per major step, we may need to add some methods that allow
you to open the file, log a whole bunch of stuff, and then
close the file.  If anyone knows that we do in fact need to
be able to do that, let me know!

=head1 AUTHOR

Dave Chess, chess@us.ibm.com

=head1 SEE ALSO

perl(1), other AVIS stuffola

=cut


