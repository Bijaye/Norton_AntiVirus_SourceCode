#
# Change history:
#  1999/01/22 - First version stolen from MkDefs.pm
#  1999/01/26 - Call Qdint->undoIntegration() when done
#  1999/02/04 - virusID and virusName arguments to Qdtest()
#  1999/03/04 - take logObject, pass to Qdtest and Qdbuild.
#

package AVIS::Macro::Qut;

#
# A small piece of OO-Perl that knows how to do a quick-and-cursory
# unit test of a set of NAV definition-deltas produced by manalysis.
# It actually does none of the work itself; it just parses data to and
# from the Qd* modules.
#
# Class Methods:
#
#    $whatever = new AVIS::Macro::Qut($deltaFile,$infectedFiles);
#
#      Returns a Qut object, ready to do a quick unit-test of the
#      deltas names in $deltaFile, on the files named in the array
#      referenced by $infectedFiles.  All filenames must be plain
#      (Qut doesn't know anything about UNC-relativization).
#
# Instance Methods:
#
#    $success = $whatever->doTest();
#
#      Actually does the quick unit-test and returns true if the deltas
#      passed the test, and false otherwise.
#
#    $logstring = $whatever->logString();
#
#      Returns a human-readable string to be logged, containing any
#      interesting information about the results of the last doTest().
#      The string may contain embedded newlines.
#
#    $whatever->logObject($logger);
#
#      Gives the quick-unit-test object a logging object (as in
#      AVIS::Logger) to use for non-final progress logging.  This
#      call is optional; if it is not used, no progress logging is
#      done.  Whether or not this call is used, the final status
#      string is still accessed via the logString() method.
#

use AVIS::Macro::Qdint();
use AVIS::Macro::Qdbuild();
use AVIS::Macro::Qdtest();
use AVIS::Local();

#
# The usual oo-perl new() routine
#
sub new {
  my $class = shift;      # the class to bless into
  my $dname = shift;      # the first argument we were given
  my $fnames = shift;     # the second argument we were given
  my $self = {};          # the anonymous hash to return
  bless $self, $class;
# Simple instance variables
  $self->{logString} = "No test has been run.";
  $self->{debug} = 0;
  $self->{logObject} = 0;
# The "private" instance variables
  $self->{_deltaFile} = $dname;
  $self->{_infectedFiles} = $fnames;
# and we're done
  return $self;
}

#
# The main routine that actually does the test
#
sub doTest {
  my $self = shift;     # Who we are
  my ($thisQdint, $thisQdbuild, $thisQdtest);
  my ($success, $changed_files, $new_defs, $virusID, $virusName);
  my $navroot = $AVIS::Local::isdnNavDefBuildRoot;

  $thisQdint = new AVIS::Macro::Qdint($self->{_deltaFile},$navroot);
  $success = $thisQdint->doIntegration();
  if ($self->debug) {print $thisQdint->logString();}
  unless ($success) {
    $self->{logString} = $thisQdint->logString();
    return $success;
  }
  $changed_files = $thisQdint->changedFiles();
  $virusID = $thisQdint->virusID();
  $virusName = $thisQdint->virusName();
  if ($self->debug) {
    my $x = join " ",@$changed_files;
    print " Changed: [$x]; ID: $virusID; Name: $virusName\n";
    my $thischange;
    local *FH;
    local *FH2;
    open FH2,">debug.out";
    binmode FH2;
    foreach $thischange (@$changed_files) {
      open FH,"<$thischange";
      binmode FH;
      print FH2 join "",<FH>;
      close FH;
    }
    close FH2;
  }

  $thisQdbuild = new AVIS::Macro::Qdbuild($navroot,$changed_files);
  $thisQdbuild->logObject($self->{"logObject"})
    if $self->{"logObject"};
  $success = $thisQdbuild->doBuild();
  if ($self->debug) {print $thisQdbuild->logString();}
  unless ($success) {
    $self->{logString} = $thisQdbuild->logString();
    $thisQdint->undoIntegration($changed_files);
    return $success;
  }
  $new_defs = $thisQdbuild->newDefs();
  if ($self->debug) {print " New defs: $new_defs\n";}

  $thisQdtest = new AVIS::Macro::Qdtest($new_defs,
                                        $self->{_infectedFiles},
                                        $virusID,
                                        $virusName);
  $thisQdtest->logObject($self->{"logObject"})
    if $self->{"logObject"};
  $success = $thisQdtest->doTest();
  if ($self->debug) {print $thisQdtest->logString()."\n";}
  unless ($success) {
    $self->{logString} = $thisQdtest->logString();
    $thisQdint->undoIntegration($changed_files);
    return $success;
  }

  # Really need to check return code here, and log errors!
  $thisQdint->undoIntegration($changed_files);
  if ($self->debug) {print $thisQdint->logString()."\n";}

  # All done OK
  $self->{logString} = "Quick unit-test completed OK.";
  return $success;
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

