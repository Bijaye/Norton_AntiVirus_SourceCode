####################################################################
#                                                                  #
# Program name:  PM/AVIS/Local/Local.pm                            #
#                                                                  #
# Module name:   Local.pm                                          #
#                                                                  #
# Description:   Defines global variables on the local node.       #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1999                      #
#                                                                  #
# Author:        Morton Swimmer                                    #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
#                                                                  #
#                                                                  #
####################################################################

package AVIS::Dirstack;
require Exporter;
use Cwd;

@ISA = qw(Exporter);
@EXPORT = qw(pushdir popdir topdir);

@EXPORT_OK = @EXPORT;


sub changedir ($) {
   my ($dest) = @_;
   `$1` if $dest =~ /^([a-zA-Z]\:)/;
   chdir $dest;
   
   my $here = cwd(); $here =~ tr</><\\>;
   
# ifdef TEST    print "Changedir $dest: now in $here\n";
# ifdef TEST    foreach my $pgm (glob "*.?xe *.?om") { #?? a bit cautious
# ifdef TEST       print     "\\\\shine\\house\\ibmav\\ibmavsn -nlog -pro -nmbr -copenerr -nb -cerr -nrep -nwipe -nfscan -nlmsg \"$here\\$pgm\"\n";
# ifdef TEST       my @lst = `\\\\shine\\house\\ibmav\\ibmavsn -nlog -pro -nmbr -copenerr -nb -cerr -nrep -nwipe -nfscan -nlmsg  "$here\\$pgm"`;
# ifdef TEST       if ($?) {
# ifdef TEST          my $qname = quotemeta "($here\\$pgm)";
# ifdef TEST          map {print "\nWARN: $1\n\n" if m<($qname.*((might have)|(verified)).*)>i} @lst;
# ifdef TEST       }
# ifdef TEST    }
}

sub pushdir ($) {
   push @dirstack::STACK, cwd();
   changedir ($_[0]);
}

sub popdir () {
   return 0 unless scalar @dirstack::STACK;
   changedir (pop @dirstack::STACK);
}

sub topdir () {
   my $rc = scalar @dirstack::STACK ? changedir ($dirstack::STACK[0]) : 1;
   @dirstack::STACK = ();
   return $rc;
}
