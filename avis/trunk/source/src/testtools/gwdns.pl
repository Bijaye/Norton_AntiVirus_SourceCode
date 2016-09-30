#!/usr/bin/perl -w

=head1 NAME

gwdns.pl -- Queries DNS for information on machine names.

=head1 STATEMENT

(c) Copyright IBM Corp. 2000

U.S. Government Users Restricted Rights - use,     
duplication or disclosure restricted by GSA ADP    
Schedule Contract with IBM Corp.                   

=head1 AUTHOR

Ian Whalley, E<lt>inw@us.ibm.comE<gt>, 2000-03-15

=head1 REQUIREMENTS

Any recent Perl for Win32, and Windows NT.

=head1 SYNOPSIS

B<gwdns.pl> E<lt>list of DNS names<gt>

=head1 DESCRIPTION

B<gwdns.pl> is designed to allow people configuring Immune System
gateways to see if their DNS people have correctly configured
the DNS [ahem].

=head2 Example One -- www.sophos.com

For example, consider the following following
output, on an obviously-expertly configured DNS:

  C:\hacks> gwdns.pl www.sophos.com

  SUCCESS : www.sophos.com : Primary name     : s-utsire.sophos.com
  SUCCESS : www.sophos.com : Alias            : www.sophos.com
  SUCCESS : www.sophos.com : IP address       : 193.82.145.130
    SUCCESS : 193.82.145.130 : Primary name     : s-utsire.sophos.com
    SUCCESS : 193.82.145.130 : Resolves back to : s-utsire.sophos.com

This shows a working DNS -- the 'customer-visible' name
(B<www.sophos.com>) is in fact a CNAME record for
B<s-utsire.sophos.com>.  This A record has only one IP address
associated with it, B<193.82.145.130>.  And, this IP address
has a valid reverse DNS record associated with it,
which points back to B<s-utsire.sophos.com>.

=head2 Example Two -- avis.watson.ibm.com

Consider the following, more complicated, example.  I have,
for reasons of formatting, shortened machine names in the
output -- the mapping between my shorthand and reality should
be fairly obvious, and does not conceal the main point:

  C:\hacks> gwdns.pl avis.watson.ibm.com

  Commencing DNS tests on 'a.watson'.
    SUCCESS : a.watson : Primary name     : a.watson
 *  SUCCESS : a.watson : IP address       : 9.2.70.29
 +    SUCCESS : 9.2.70.29 : Primary name     : n.watson
 -      SUCCESS : n.watson : Primary name     : n.watson
 -      SUCCESS : n.watson : Resolves back to : 9.2.70.29
 *  SUCCESS : a.watson : IP address       : 9.2.70.17
 =    SUCCESS : 9.2.70.17 : Primary name     : entry.i.watson
 /      SUCCESS : entry.i.watson : Primary name     : entry.i.watson
 /      SUCCESS : entry.i.watson : Resolves back to : 9.2.70.17

[Note -- I have changed/shortened the machine names for formatting
reasons]

This DNS also gets an A+, and it's more complicated too!  In this
case, B<a.watson> is a round-robin DNS record -- it has two IP
numbers associated with it -- B<9.2.70.29> and B<9.2.70.17>.  This
information is revealed by the lines marked with a '*' in the left
margin.

The first of these two IP numbers -- B<9.2.70.29> -- has a valid
reverse DNS record, B<n.watson> (line marked with '+').  This
record maps, in turn, to B<9.2.70.29> -- the same address that
this set started with (lines marked with '-').

The second of the two IP numbers -- B<9.2.70.17> -- also has
a valid reverse DNS record, B<entry.i.watson> (line marked with
'=').  This record maps, in turn, to B<9.2.70.17> -- the same
address that this set started with (lines marked with '/'.

=head2 Example Three -- www.symantec.com

Again, I have changed the names to preserve formatting:

  C:\hacks> gwdns.pl www.symantec.com

  Commencing DNS tests on 'w.symc.c'.
    SUCCESS : w.symc.c : Primary name     : w.symc.c
    SUCCESS : w.symc.c : IP address       : 208.226.167.22
      SUCCESS : 208.226.167.22 : Primary name     : w.symc.c
      SUCCESS : 208.226.167.22 : Alias            : 22.167.226.208.in-addr.arpa
      SUCCESS : 208.226.167.22 : Resolves back to : w.symc.c
    SUCCESS : w.symc.c : IP address       : 208.226.167.23
      SUCCESS : 208.226.167.23 : Primary name     : w.symc.c
      SUCCESS : 208.226.167.23 : Alias            : 23.167.226.208.in-addr.arpa
      SUCCESS : 208.226.167.23 : Resolves back to : w.symc.c
    SUCCESS : w.symc.c : IP address       : 216.35.137.200
      FAILED  : 216.35.137.200 : Does not resolve
    SUCCESS : w.symc.c : IP address       : 216.35.137.201
      FAILED  : 216.35.137.201 : Does not resolve
    SUCCESS : w.symc.c : IP address       : 216.35.137.202
      FAILED  : 216.35.137.202 : Does not resolve
    SUCCESS : w.symc.c : IP address       : 208.226.167.17
      SUCCESS : 208.226.167.17 : Primary name     : w.symc.c
      SUCCESS : 208.226.167.17 : Alias            : 17.167.226.208.in-addr.arpa
      SUCCESS : 208.226.167.17 : Resolves back to : w.symc.c

In this case, B<www.symantec.com> (shortened to B<w.symc.c>) is
a round-robin record between six different IP numbers.  Of these
six IPs, three have valid reverse DNS records -- the remaining
three do not.  As a point of interest, the three that do have
valid reverse records have aliases which are clearly bogus --
this is not a problem as far as the Immune System (if it were
using B<www.symantec.com> as a gateway), but it's incorrect
nevertheless.

The DNS for B<www.symantec.com> is not set up correctly,
and is unsuitable for use as an Immune System gateway.

=head2 Summary

The quick test to see if a gateway machine's name is
correctly installed in DNS is to simply run
B<gwdns.pl> against it.  If any lines prefixed with
'FAILED' come back, then gateway machine's DNS is not
correct.

It is necessary to run B<gwdns.pl> against all machines
in all gateway arrays, in addition to any and all round-robin
DNS records which are configured for the gateway system
as a whole.

=head1 BUGS

Doesn't work at all on Win9x.  This seems to be due to some
weird behaviour from the local DNS system on that platform.
To counter this, the script will not allow itself to be run
on Win95 or Win98.  Do not disable this check -- the script
will give encouraging results where none are merited.

=cut

# -----

use strict;

# -----

#
# Takes the output from gethostby{name,addr}, and performs
# some basic sanity checks.  Also presents some information
# to the user.
#
sub sanityCheck
{
  my ($spaces,$item,$name,$aliases,$addrType,$addrLen,@addrs) = @_;
  my $alias;
  my @aliases;

  # Did the gethostby{addr,name} work?
  if (! defined $name)
  {
    print $spaces, "FAILED  : $item : Does not resolve\n";
    return 0;
  }

  # Perform some sanity checks on the values.
  if ($addrType != 2)
  {
    print $spaces, "FAILED  : $item : Address is not of type AF_INET\n";
    return 0;
  }
  if ($addrLen != 4)
  {
    print $spaces, "FAILED  : $item : Address is not of length 4\n";
    return 0;
  }

  # Emit the primary name.
  print $spaces, "SUCCESS : $item : Primary name     : $name\n";

  # Now list out the aliases.  Uniqify the list first -- duplicates
  # seem to show up frequently when they should not.
  my %seen = ();
  @aliases = grep { ! $seen{$_} ++ } split(/ /, $aliases);

  foreach $alias (@aliases)
  {
    print $spaces, "SUCCESS : $item : Alias            : $alias\n";
  }

  # This should be impossible.
  if (scalar(@addrs) == 0)
  {
    print $spaces, "FAILED  : $item : No addresses associated with name [?!]\n";
    return 0;
  }

  return 1;
}

# -----

sub testOne($)
{
  my ($item) = @_;
  my $name;
  my $aliases;
  my $addrType;
  my $addrLen;
  my $addr;
  my @addrs;
  my $sanity;

  print "\nCommencing DNS tests on '$item'.\n";

  ($name,$aliases,$addrType,$addrLen,@addrs) = gethostbyname($item);

  # Perform the basic checks.
  $sanity = sanityCheck("  ",$item,$name,$aliases,$addrType,$addrLen,@addrs);

  (! $sanity) && return 0;

  MAINADDR: foreach $addr (@addrs)
  {
    my $nameTwo;
    my $aliasesTwo;
    my $addrTypeTwo;
    my $addrLenTwo;
    my @addrsTwo;
    my $addrStr;
    my $nameThree;
    my $aliasesThree;
    my $addrTypeThree;
    my $addrLenThree;
    my @addrsThree;
    my $itemThree;
    my $a;
    my $b;
    my $c;
    my $d;

    # Form a printable version of the number.
    ($a,$b,$c,$d) = unpack('C4', $addr);
    $addrStr = "$a.$b.$c.$d";

    print "  SUCCESS : $item : IP address       : $addrStr\n";

    # Look up the number.
    ($nameTwo,$aliasesTwo,$addrTypeTwo,$addrLenTwo,@addrsTwo) = gethostbyaddr($addr, 2);

    # How was it?
    $sanity = sanityCheck("    ",$addrStr,$nameTwo,$aliasesTwo,$addrTypeTwo,$addrLenTwo,@addrsTwo);
    (! $sanity) && next MAINADDR;

    if ($nameTwo eq $name)
    {
      print "    SUCCESS : $addrStr : Resolves back to : $name\n";
      next MAINADDR;
    }

    # If there's more than one address connected to the address,
    # something really wacked is happening.
    if (scalar(@addrsTwo) != 1)
    {
      print "    FAILED  : $addrStr : Too many addresses associated with address [?!]\n";
      next MAINADDR;
    }

    # Now get numbers from that the name again...
    ($nameThree,$aliasesThree,$addrTypeThree,$addrLenThree,@addrsThree) = gethostbyname($nameTwo);

    # How was it...
    $sanity = sanityCheck("      ",$nameTwo,$nameThree,$aliasesThree,$addrTypeThree,$addrLenThree,@addrsThree);
    (! $sanity) && next MAINADDR;

    # There must be only one address associated with this
    # second-level name.
    if (scalar(@addrsThree) != 1)
    {
      print "      FAILED  : $nameTwo : Too many addresses associated with name\n";
      next MAINADDR;
    }

    if ($addrsThree[0] eq $addr)
    {
      print "      SUCCESS : $nameTwo : Resolves back to : $addrStr\n";
    }
    else
    {
      print "      FAILED  : $nameTwo : Address does not map back to initial address\n";
      next MAINADDR;
    }
  }

  return 1;
}

# ----- START HERE -----

{
  local $^W = 0;
  if ($^O =~ /mswin32/i)
  {
    require Win32; import Win32;

    if (Win32::IsWin95())
    {
      print "ERROR: This is Win9x.\n\n";
      print "The local DNS system on Win9x has shown a disturbing\n";
      print "tendency to make up the answers, and is therefore not\n";
      print "supported.  You must run this on WinNT or better.  This\n";
      print "includes Unix.\n";
      exit 1;
    }
  }
}

my $didOne;
my $item;

$didOne = 0;
foreach $item (@ARGV)
{
  $didOne = 1;
  testOne($item);
}

if (! $didOne)
{
  print "USAGE: gwdns.pl <DNS name> [<DNS name> ...]\n";
  exit 1;
}

exit 0;
