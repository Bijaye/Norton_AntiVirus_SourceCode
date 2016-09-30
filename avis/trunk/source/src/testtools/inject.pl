#!/usr/bin/perl -w

# -----

=head1 NAME

inject.pl -- Inject files into the Immune System using standard paths.

=head1 STATEMENT

Licensed Materials - Property of IBM 

(c) Copyright IBM Corp. 2000

U.S. Government Users Restricted Rights - use,     
duplication or disclosure restricted by GSA ADP    
Schedule Contract with IBM Corp.                   

=head1 AUTHOR

E<lt>inw@us.ibm.comE<gt>, 2000-03-03.

=head1 REQUIREMENTS

Perl for Win32, any recent version should be fine.  Development was
done with Perl 5.005_03, ActiveState build 522.

=head1 SYNOPSIS

B<inject.pl> [B<--interval=E<lt>nE<gt>[,E<lt>oE<gt>]>]
             [B<--number=E<lt>nE<gt>[,E<lt>oE<gt>]>]
             [B<--live=E<lt>dirE<gt>>]
             [B<--freezedried=E<lt>dirE<gt>>]
             [B<--quarantine=E<lt>dirE<gt>>]
             [B<--merge>]
             [B<--randomise>]

=head1 DESCRIPTION

B<inject.pl> attempts to inject samples into the Immune System
by interacting with standard consumer-level NAV software.

=head1 OPTIONS

=over 5

=item B<--live=E<lt>dirE<gt>>

Assume that the directory hierarchy under E<lt>dirE<gt> contains
live viruses.  B<inject.pl> will open these files in an attempt
to trigger the on-access scanner, which will then quarantine the
sample.

This option may be specified multiple times, for multiple live
virus hierarchies.

=item B<--freezedried=E<lt>dirE<gt>>

Assume that the directory hierarchy under E<lt>dirE<gt> contains
'freezedried' quarantine samples.  These are samples which can
simply be copied into the central quarantine directory and will
then be submitted.

This option may be specified multiple times, for multiple freezedried
sample hierarchies.

If one or more B<--freezedried> options are given, B<--quarantine> must
also be present.

=item B<--quarantine=E<lt>dirE<gt>>

Specify the path to central quarantine.  There is no default for this,
although it's usually c:\quarantine.  This option is only needed if
B<--freezedried> is used.

B<inject.pl> is unable to verify that the directory is, in reality,
a central quarantine directory.  It can check that it's a directory,
though :-).

=item B<--merge>

If specified, multiple freezedried and live hierarchies will be regarded
as one.  So, for example, if the user did:

  --live=c:\l1 --live=c:\l2 --freezedried=c:\f1 --merge

then B<inject.pl> would regard l1 and l2 as part of the same
hierarchy, as opposed to separate.  For obvious reasons, freezedried
hierarchies are not merged with live hierarchies.

=item B<--randomise>

If specified, causes B<inject.pl> to submit samples in a random order
from the hierarchies, as opposed to the default behaviour of submitting
in whatever order the files are found on the disk.

=item B<--interval=E<lt>nE<gt>[,E<lt>oE<gt>]>

B<--interval> specifies the time, in seconds, for which B<inject.pl>
will sleep between cycles.  There are two possible forms for
this argument -- B<--interval=E<lt>nE<gt>> will cause B<inject.pl>
to sleep for E<lt>nE<gt> seconds between cycles; whereas
B<--interval=E<lt>nE<gt>,E<lt>oE<gt>> will causes B<inject.pl> to
pick a random number between E<lt>nE<gt> and E<lt>oE<gt>, and sleep
for that number of seconds.  For example:

  --interval=10,20

would cause B<inject.pl> to sleep for a number of seconds between
10 and 20 after each cycle.

This option is mandatory.

=item B<--number=E<lt>nE<gt>[,E<lt>oE<gt>]>

B<--number> specifies how many files B<inject.pl> will operate on each
cycle in each hierarchy.  As with B<--interval>, the user can specify
two numbers, in which case a random number between them will be chosen.

This option is mandatory.

=back

=head1 EXAMPLES:

=over 5

=item B<inject.pl --freezedried=c:\f1 --quarantine=c:\q --interval=10 --number=10>

Every 10 seconds, B<inject.pl> will copy 10 files from c:\f1 into c:\q.

=item B<inject.pl --live=c:\l1 --interval=0,5 --number=0,50>

B<inject.pl> will sleep for 0 to 5 (i.e. 0, 1, 2, 3, 4, or 5) seconds,
and attempt to access between 0 and 50 files from the c:\l1 hierarchy.

=item B<inject.pl --live=c:\l1 --live=c:\l2 --interval=0,5 --number=0,50>

As above, except that B<inject.pl> will access between 0 and
50 files from c:\l1 I<and> between 0 and 50 files from c:\l2 every cycle.

=item B<inject.pl --live=c:\l1 --live=c:\l2 --interval=0,5 --number=0,50 --merge>

As above, except that B<inject.pl> will access between 0 and
50 files from I<both> the c:\l1 and c:\l2 hierarchies every cycle.

=item B<inject.pl --live=c:\l1 --live=c:\l2 --interval=0,5 --number=0,50 --merge --randomise>

As above, except the file accesses will be randomise (they will not be in a
logical order).

=item B<inject.pl --live=c:\l1 --live=c:\l2 --interval=0,5 --number=0,50 --randomise --freezedried=c:\f1 --freezedried=c:\f2 --quarantine=c:\quarantine>

B<inject.pl> will sleep for 0 to 5 seconds, and then:

  * Access between 0 and 50 files from the c:\l1 hierarchy.
  * Access between 0 and 50 files from the c:\l2 hierarchy.
  * Copy between 0 and 50 files from the c:\f1 hierarchy
    into the c:\quarantine directory.
  * Copy between 0 and 50 files from the c:\f2 hierarchy
    into the c:\quarantine directory.

These accesses will be randomised.

=item B<inject.pl --live=c:\l1 --live=c:\l2 --interval=0,5 --number=0,50 --randomise --freezedried=c:\f1 --freezedried=c:\f2 --quarantine=c:\quarantine --merge>

B<inject.pl> will sleep for 0 to 5  seconds, and then:

  * Access between 0 and 50 files from the c:\l1 and c:\l2
    hierarchies.
  * Copy between 0 and 50 files from the c:\f1 and c:\f2
    hierarchies into the c:\quarantine directory.

=back

=head1 OTHER REQUIRMENTS

It is the user's responsibility to ensure the following:

=over 5

=item *

  Quarantine Server is correctly installed and configured.

=item *

  IcePack is correctly installed and configured.  Most important,
  IcePack should be configured with a non-zero default submission
  priority.

=item *

  Any freezedried samples used are ready for submission.  If
  the samples were manually captured, the freezedried samples
  must have been released prior to freezedrying.

=item *

  The gateway has a crippled def set, and that def set has rolled
  down the IcePack machine, and the NAV client.

=item *

  The NAV on-access scanner is correctly configured (to quarantine
  everything, and not repair anything).

=back

=head1 FAQ

=head2 What does B<inject.pl> do to the files on the disk?

Nothing.  It doesn't delete anything, it only makes copies,
if you use the B<--freezedried> option.  I<However>, users
should be aware that, if they use the B<--live> option,
the NAV client will probably delete the files which it
quarantines.

=head2 Gosh, the options are awfully long, aren't they?

Yes they are.  Deal with it.

=head2 That's not how I spell 'randomise'.

Well, it's how I spell it.

=back

=cut

# -----

use strict;
use IO::Handle;
use Getopt::Long;
use File::Copy;
use File::Find;
use FileHandle;
use HTTP::Date;

# -----

my @dirTypes = ('freezedried','live');

my $cheatyGlobalDirForFileFind;
my $cheatyGlobalListForFileFind;

# -----

#
# usageMsg() simply prints the usage message (duh!) and returns
# undef.  It returns failure because that's the code that
# the caller will want to return (you'll see what I mean later).
#
sub usageMsg()
{
  print <<EOM;

USAGE: inject.pl <options>

Available options:
  --freezedried=<dir>  : Use freezedried samples from <dir>.  May be
                         specified more than once.
  --live=<dir>         : Use live samples from <dir>.  May be
                         specified more than once.
  --merge              : Merge all freezedried directories together,
                         and all live directories together.
  --quarantine=<dir>   : Specify the location of the NAV central
                         quarantine directory.  This option must
                         be present if --freezedried is also present.
  --interval=<n>[,<o>] : Submit samples at intervals of <n> seconds.
                         If <o> is also specified, use a random number
                         of seconds s such that n <= s <= o.
  --number=<n>[,<o>]   : Submit <n> samples every interval.  If <o>
                         is also specified, use a random number of
                         samples s such that n <= s <= o.
  --randomise          : Submit samples randomly from the <dir>s.
                         If this is not specified, inject.pl will
                         submit in breadth-first order.

The author believes that the user is unlikely to be satisfied with any
defaults chosen that he did not choose any.
EOM

  return undef;
}

# -----

#
# handleNumOpts() handles the options that take the form
# <n> or <n>,<o>.  On failure, it returns undef (and has
# already shown the usage message); on success it
# returns a list of two numbers.  Both may be the same.
#
sub handleNumOpts($$)
{
  my ($val,$opt) = @_;
  my $n;
  my $o;

  (((! defined $val) || ($val eq "")) ||
   ((! defined $opt) || ($opt eq ""))) && return (usageMsg(),undef);

  ($n,$o) = split(/,/, $val);

  (! defined $o) && ($o = $n);

  if (($n =~ /\D/) || ($o =~ /\D/))
  {
    print "'$val' is a bogus value for '$opt'.\n";
    return (usageMsg(),undef);
  }

  # Switch around so the smallest is first, and that's it.
  return (($n > $o) ? ($o, $n) : ($n, $o));
}

# -----

#
# handleOptions() does the argument processing.  It also does
# argument validation.
#
# Returns: undef on error, hash ref to options on success.
#
sub handleOptions()
{
  my %options = ();
  my $gotOne  = 0;
  my $bailOut = 0;
  my $dir;
  my @tmpOpt;
  my $q;

  # Constuct the arguments to GetOptions() from the dir types.
  foreach $q (@dirTypes)
  {
    push(@tmpOpt, $q . "=s@");
  }

  # Get the user's options.
  (! GetOptions(\%options,
                @tmpOpt,
                "quarantine=s",
                "interval=s",
                "number=s",
                "merge!",
                "randomise!")) && return usageMsg();

  # The user's options appear valid.  Now see if they really are.

  # --number and --interval are both mandatory.
  if (! defined $options{'number'})
  {
    print "ERROR: --number is required.\n";
    $bailOut = 1;
  }
  if (! defined $options{'interval'})
  {
    print "ERROR: --interval is required.\n";
    $bailOut = 1;
  }

  # Need one or more of these.
  if ((! defined $options{'freezedried'}) && (! defined $options{'live'}))
  {
    print "ERROR: at least one of --freezedried and --live is required.\n";
    $bailOut = 1;
  }

  # If they specified freezedried trees, they also had to specify a
  # valid quarantine directory.
  if (defined $options{'freezedried'})
  {
    if ((! defined $options{'quarantine'}) || 
        ($options{'quarantine'} eq "")     ||
        (! -d $options{'quarantine'}))
    {
      print "ERROR: --quarantine is required with --freezedried, and must\n";
      print "be a valid directory.\n";
      $bailOut = 1
    }
  }

  # Build up a list of all the directory options specified.
  @tmpOpt = ();
  foreach $q (@dirTypes)
  {
    (! defined $options{$q}) && next;
    push(@tmpOpt, @{$options{$q}});
  }

  # Walk across the user's directory options...
  foreach $dir (@tmpOpt)
  {
    $gotOne = 1;
    if (! -d $dir)
    {
      print "ERROR: '$dir' is not a directory.\n";
      $bailOut = 1;
    }
  }

  # If we didn't get any directories, or we got one or more
  # bad ones, give up.
  ((! $gotOne) || ($bailOut)) && return usageMsg();

  # Handle the two numeric-type options.
  ($options{'minNum'},$options{'maxNum'}) =
         handleNumOpts($options{'number'},   "number");
  (! defined $options{'minNum'}) && return undef;

  ($options{'minInt'},$options{'maxInt'}) =
         handleNumOpts($options{'interval'}, "interval");
  (! defined $options{'minInt'}) && return undef;

  # Remove the now-unused virgin versions of those numeric options.
  delete $options{'number'};
  delete $options{'interval'};

  # Fill in the optional ones...
  (! defined $options{'randomise'}) && ($options{'randomise'} = 0);
  (! defined $options{'merge'})     && ($options{'merge'}     = 0);

  # JD.
  return \%options;
}

# -----

#
# handleFile() is called from the find() calls to dig up all
# the files in a tree.  It checks to ensure that the thing which
# find() has found is a real file, and makes a note of it.
#
sub handleFile
{
  # If it's not a regular file, skip it.
  (! -f $File::Find::name) && return;

  # Stick it on the end of the list.
  push @{$cheatyGlobalListForFileFind}, $File::Find::name;

  # JD.
  return;
}

# -----

#
# buildTrees walks the directory trees that the user specified,
# building lists of the files within.
#
sub buildTrees($)
{
  my ($options) = @_;
  my $type;
  my $num;

  # For both 'freezedried' and 'live', we need to walk the trees.
  foreach $type (@dirTypes)
  {
    # Alas, we need to use some globals here (or at least, it's
    # easier if we do).  So, put the starting directory into
    # one global, and initialise another to point to the list
    # into which to put the files (which is currently empty,
    # of course).
    foreach $cheatyGlobalDirForFileFind (@{$options->{$type}})
    {
      if ($options->{'merge'})
      {
        $cheatyGlobalListForFileFind = \@{$options->{'lists'}{$type}{'ONE'}};
      }
      else
      {
        $cheatyGlobalListForFileFind = \@{$options->{'lists'}{$type}{$cheatyGlobalDirForFileFind}};
      }
      print "Walking $cheatyGlobalDirForFileFind for '$type' tree... ";
      find(\&handleFile, $cheatyGlobalDirForFileFind);
      $num = scalar(@{$cheatyGlobalListForFileFind});
      print "...done [$num files].\n";
    }
  }

  # JD.
  return 1;
}

# -----

#
# reorderLists() goes through the lists of filenames held in the
# options hash, and randomises them (if the user turned that feature on).
# It's easier to do it this way than pick random elements out of the list
# later on.  If the user didn't choose to randomise, we reverse the lists
# instead.  This is deliberate also.
#
sub reorderLists($)
{
  my ($options) = @_;
  my $array;
  my $i;
  my $type;
  my $dirRoot;

  foreach $type (@dirTypes)
  {
    foreach $dirRoot (keys %{$options->{'lists'}{$type}})
    {
      if ($options->{'randomise'})
      {
        print "Randomising $dirRoot tree... ";

        # This is called a Fisher-Yates shuffle, and I copied it
        # from the Perl Cookbook.
        $array = \@{$options->{'lists'}{$type}{$dirRoot}};
        for ($i = @$array; --$i; )
        {
          my $j = int rand($i + 1);
          ($i == $j) && next;
          @$array[$i, $j] = @$array[$j, $i];
        }

        print "...done.\n";
      }
      else # not randomise
      {
        # Reverse the list.
        print "Reversing $dirRoot tree... ";

        my @new = reverse @{$options->{'lists'}{$type}{$dirRoot}};
        @{$options->{'lists'}{$type}{$dirRoot}} = @new;

        print "...done.\n";
      }
    }
  }

  # JD.
  return 1;
}

# -----

#
# pickRandBetween() picks a number between the two arguments given.
#
sub pickRandBetween($$)
{
  my ($min,$max) = @_;
  my $range;
  my $val;

  # Calculate the range.
  $range = $max - $min;

  # If the top and bottom numbers are the same, it's easy.
  (! $range) && return $min;

  # Pick a random value such that $min <= $val <= $max.
  return ($min + int(rand($range + 1)));
}

# -----

#
# getTimeString() returns an ISOish time string in the current time zone.
# (i.e. based on localtime()).
#
sub getTimeString()
{
  my @timeStuff;

  @timeStuff = localtime(time());
  return sprintf("%04d-%02d-%02d %02d:%02d:%02d", ($timeStuff[5] + 1900),
                                                  ($timeStuff[4] + 1),
                                                  $timeStuff[3],
                                                  $timeStuff[2],
                                                  $timeStuff[1],
                                                  $timeStuff[0]);
}

# -----

#
# sleepInChunks() sleeps for n seconds (n is the parameter) in chunks
# of one second.  Every second, it displays how many are left.
#
sub sleepInChunks($)
{
  my ($sleepFor) = @_;
  my $i;

  for ($i = 0; $i < $sleepFor; $i++)
  {
    print "Sleeping: ", ($sleepFor - $i), "   \r";
    sleep(1);
  }

  # Clear the line (important at the end of the program's run.
  print "               \r";

  # JD.
  return 1;
}

# -----

#
# doYourThing() is the guts -- it walks across all the lists
# constructed, with random sleep periods, and acts on the files
# held in the lists.
#
sub doYourThing($)
{
  my ($options) = @_;
  my $hadAChanceToDoOne;

  # Loop around until we've processed everything.
  do
  {
    my $type;
    my $dirRoot;
    my $numFiles;
    my $sleepFor;
    my $i;
    my $len;

    print getTimeString(), "\n";

    # Clear the flag.
    $hadAChanceToDoOne = 0;

    foreach $type (@dirTypes)
    {
      foreach $dirRoot (keys %{$options->{'lists'}{$type}})
      {
        # Discover the length of the array.
        $len = $#{$options->{'lists'}{$type}{$dirRoot}};

        # Skip on if there aren't any files left.
        ($len == 0) && next;

        $hadAChanceToDoOne = 1;

        # Pick the number of files to process in this tree.
        $numFiles = pickRandBetween($options->{'minNum'},
                                    (($options->{'maxNum'} > $len) ? $len : $options->{'maxNum'}));

        # Now go and process the file...
        for ($i = $len; $i > ($len - $numFiles); $i--)
        {
          actOnFile($options, $type, $options->{'lists'}{$type}{$dirRoot}[$i]);
        }

        # Adjust the length of the array down (chop off the final elements).
        $#{$options->{'lists'}{$type}{$dirRoot}} = $len - $numFiles;
      }
    }

    if ($hadAChanceToDoOne)
    {
      # Pick a sleep time, and sleep for it.
      $sleepFor = pickRandBetween($options->{'minInt'}, $options->{'maxInt'});
      sleepInChunks($sleepFor);
    }
  }
  while ($hadAChanceToDoOne);

  # JD.
  return 1;
}

# -----

#
# actOnFile() takes a type and filename from doYourThing() and
# actually does the appropriate thing to the file base upon the
# type.
#
sub actOnFile($$$)
{
  my ($options, $type, $src) = @_;
  my $hnd;
  my $fil;
  my $dest;
  my $state;
  my $verb;

  if ($type eq "freezedried")
  {
    $verb = "Quarantining";
    $fil = $src;
    $fil =~ s/^.*[\\\/]//;
    ((! defined $fil) || ($fil eq "")) && return 0;

    $dest = $options->{'quarantine'} . "/" . $fil;

    if (! File::Copy::copy($src, $dest))
    {
      $state = "FAILED";
    }
    else
    {
      $state = "SUCCEEDED";
    }
  }
  elsif ($type eq "live")
  {
    $verb = "Opening";
    $hnd = IO::Handle->new();

    # Open the file, and if it works, close it again immediately.
    if (open($hnd, $src))
    {
      $state = "SUCCEEDED";
      close($hnd);
    }
    else
    {
      $state = "FAILED";
    }
  }

  # Format the name for printing.
  $src =~ s/\\/\//g;

format inw =
[@<<<<<<<<<<] [@<<<<<<<<<<<] [@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<] [@<<<<<<<<]
$type, $verb, $src, $state
.

  STDOUT->format_name("inw");
  write;
  STDOUT->format_name("STDOUT");

  # JD.
  return 1;
}

# ----- START HERE -----

my $options;

# Spin the randometer.
srand();

# Get the user's options.  If this fails, give up.
$options = handleOptions();
(! defined $options) && exit 1;

# Ready to build our trees.
if (! buildTrees($options))
{
  print "Failed to build in-memory versions of the directory trees.\n";
  exit 1;
}

# Reorder the lists, according to the user's options.
if (! reorderLists($options))
{
  print "Failed to reorder in-memory versions of the directory trees.\n";
  exit 1;
}

# Now do the actual work.
if (! doYourThing($options))
{
  print "Failed to do the real work.\n";
  exit 1;
}

exit 0; 
