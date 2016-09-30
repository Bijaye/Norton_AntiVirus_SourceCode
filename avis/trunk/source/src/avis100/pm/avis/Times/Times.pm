package AVIS::Times;


BEGIN {
  die "This module is intented to run on Win32 platforms, not on $^O" unless $^O =~ /win32$/i;
}


use strict;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);

require Exporter;
require DynaLoader;

@ISA = qw(Exporter DynaLoader);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT = qw(
	
);
$VERSION = '1.0';

bootstrap AVIS::Times $VERSION;


1;
__END__

=head1 NAME

AVIS::Times

=head1 SYNOPSIS

  use AVIS::Times;
  @times = AVIS::Times::times();

=head1 DESCRIPTION

This module implements a reliable version of times() on Windows NT.
It uses the GetProcessTimes win32 API call, which is not supported on Windows 9x.

AVIS::Times::times() returns the same set of results as the core version,
except that kernel and user times used by child processes are not computed.

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 2000

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHOR

Jean-Michel Boulay

=head1 SEE ALSO

perl(1).

=cut
