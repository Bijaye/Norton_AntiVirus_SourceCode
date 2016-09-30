# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

BEGIN { $| = 1; print "1..8\n"; }
END {print "not ok 1\n" unless $loaded;}
use EntTrac;
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):

print &EntTrac::alg_inslen("\xE9\x05\x04", 0) == 3 ? "ok 2\n" : "not ok 2\n";
print &EntTrac::alg_inslen("\xFC\xE8\xC7\x1B", 0) == 1 ? "ok 3\n" : "not ok 3\n";
print &EntTrac::alg_inslen("\xFC\xE8\xC7\x1B", 1) == 3 ? "ok 4\n" : "not ok 4\n";
print &EntTrac::i86OpcLen("\xE9\x05\x04", 0) == 3 ? "ok 5\n" : "not ok 5\n";
print &EntTrac::i86OpcLen("\xFC\xE8\xC7\x1B", 0) == 1 ? "ok 6\n" : "not ok 6\n";
print &EntTrac::i86OpcLen("\xFC\xE8\xC7\x1B", 1) == 3 ? "ok 7\n" : "not ok 7\n";
print &TestPartition() ? "ok 8\n" : "not ok 8\n";

1;

sub TestPartition {
  my @filelist = qw (
		     t/e1000.exe
		     t/e10000.exe
		     t/e50000.exe
		     t/e600.exe
		     t/c10.com
		     t/c100.com
		     t/c1000.com
		     t/c10000.com
		    );
  
  my %part = &EntTrac::Partition(@filelist);
#  foreach (keys %part) {
#    print "File: $_ is in partition: $part{$_}\n";
#  }

  my $partition1ok = (($part{"t/e1000.exe"} == $part{"t/e10000.exe"}) 
		      && ($part{"t/e50000.exe"} == $part{"t/e600.exe"}) 
		      && ($part{"t/e1000.exe"} == $part{"t/e600.exe"})
		     );
  my $partition2ok = ($part{"t/c10.com"} == $part{"t/c100.com"});
  my $partition3ok = ($part{"t/c1000.com"} == $part{"t/c10000.com"});
#  print "Results 1: $partition1ok 2: $partition2ok 3: $partition3ok\n";
  ($partition1ok && $partition2ok && $partition3ok);
}
