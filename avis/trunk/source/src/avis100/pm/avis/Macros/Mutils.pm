
#
# Change history:
#  1998/12/09 - First version.
#  1998/12/10 - Stick in /Q /E, as required by funny Perl.
#

package AVIS::Macro::Mutils;

use AVIS::FSTools;

#
# Some little necessary-evil utilities
#

#
# $result = AVIS::Macro::Mutils::Canonize($fname);
#
# converts all of the Other Kind of slashes into This Kind of
# slashes, as determined by the current operating system.  If
# the last character of $fname is either kind of slash, removes it.
#
# $result = AVIS::Macro::Mutils::Canonize($fname,$stem);
#
# does the above to both $fname and $stem, and then returns $stem,
# followed by This Kind of slash, followed by $fname.
#
# MS: oops, I just moved this into AVIS::FSTools...
#
sub Canonize {
  warn "Use of AVIS::Macro::Mutils::Canonize() is deprecated\n";
  return AVIS::FSTools::Canonize(@_);
}

1;

