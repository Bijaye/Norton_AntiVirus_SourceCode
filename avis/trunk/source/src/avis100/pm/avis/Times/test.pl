# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

BEGIN { $| = 1; print "1..1\n"; }
END {print "not ok 1\n" unless $loaded;}
use AVIS::Times;
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):
use strict;

my ($cpubefore, $cpuafter, $timebefore, $timeafter);
my @times;

@times = AVIS::Times::times;
$cpubefore = $times[0] + $times[1];
$timebefore = time;
sleep(30);
@times = AVIS::Times::times;
$cpuafter = $times[0] + $times[1];
$timeafter = time;
print $cpubefore, " ", $cpuafter, " ", $cpuafter - $cpubefore, " ", $timeafter - $timebefore, "\n";
print "ok 2\n";

@times = AVIS::Times::times;
$cpubefore = $times[0] + $times[1];
$timebefore = time;
my $i;
for ($i = 0; $i < 1000000; ++$i) {
  my $tmp = 1.23456789 * 9.87654321;
}
@times = AVIS::Times::times;
$cpuafter = $times[0] + $times[1];
$timeafter = time;
print $cpubefore, " ", $cpuafter, " ", $cpuafter - $cpubefore, " ", $timeafter - $timebefore, "\n";
print "ok 3\n";
