####################################################################
#                                                                  #
# Program name:  pm/AVIS/Macros/OLE2Doc                            #
#                                                                  #
# Package name:  AVIS::Macro::OLE2Doc                              #
#                                                                  #
# Description:   Determine properties of OLE2 files                #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 2000                                          #
#                                                                  #
# Author:        Jean-Michel Boulay                                #
#                                                                  #
# And also:      U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
# Uses Symantec's ole2docnf.exe to determine whether a supported   #
# OLE2 file is encrypted or has embedded macros.                   #
# Supported OLE2 files are currently Word documents and Excel      #
# workbooks.                                                       #
#                                                                  #
####################################################################

package AVIS::Macro::OLE2Doc;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw();
@EXPORT_OK = qw();

$VERSION = 1.0;

use Cwd;
use File::Path;

use AVIS::FSTools();
use AVIS::Local;
use AVIS::Macro::OLE2Stream;

use strict;

my $ToolDir = "$isdnROOT\\$isdnBin";


sub new {
  my $class = shift;
  my $fname = shift;
  
  my $self = {};  
  $self->{'STREAMS'} = [];
  $self->{'FILENAME'} = $fname;
  bless($self, $class);
    
  return (_Analyze($self)) ? $self : undef;
}


sub IsOLE2
{
  my $ole2 = pack ("C8", 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1);

  my $buf;
  my ($fn) = @_;
  open (FILE, "$fn"); binmode FILE; read (FILE, $buf, 8); close FILE;
  return ($ole2 eq $buf) ? 1 : 0;
}


sub _Analyze($) {
  my $self = shift;

  my $program = AVIS::FSTools::Canonize("oledocnf.exe",$ToolDir);
  my $fullfname = Win32::GetShortPathName($self->{'FILENAME'});
  my $command = "$program $fullfname";
  my @results = `$command`;
  
  if ($? != 0) { #an error occured
    warn "$command failed. return code: $?";
    return 0;
  }
  foreach (@results) {
    my $embedded = (s/^\+\+//) ? 1 : 0;
    
    my @streamattributes = split;    
    my $streamtype = shift(@streamattributes);    
    my $stream = new AVIS::Macro::OLE2Stream($streamtype, [@streamattributes]);
    $stream->MarkAsEmbedded() if $embedded;
    $self->AddStream($stream);
  }
  return 1;
}


sub AddStream($$) {
  my $self = shift;
  my $stream = shift;
  push @{$self->{'STREAMS'}}, $stream;
}

sub Streams($$) {
  my $self = shift;
  my $stream = shift;
  return @{$self->{'STREAMS'}};
}


sub IsEncrypted($) {
my $ole2doc = shift;

  my @streams = $ole2doc->Streams;
  foreach (@streams) {
    if ((!$_->IsEmbedded) && ($_->IsDocStream) && ($_->IsEncrypted)) {
      return 1;
    }
  }
  return 0;
}


sub DocType($) {
my $ole2doc = shift;

  my $embedded = 0;
  my @streams = $ole2doc->Streams;
  foreach (@streams) {
    return $_->Type if ((!$_->IsEmbedded) && ($_->IsDocStream));
  }
  return undef;
}


sub HasEmbeddedStreams($) {
my $ole2doc = shift;

  my $embedded = 0;
  my @streams = $ole2doc->Streams;
  foreach (@streams) {
    ++$embedded if ($_->IsEmbedded);
  }
  return $embedded;
}

sub HasMainMacros($) {
my $ole2doc = shift;

  my @streams = $ole2doc->Streams;
  foreach (@streams) {
    if ((!$_->IsEmbedded) && ($_->IsMacroStream)) {
      return 1;
    }
  }
  return 0;
}

sub HasEmbeddedMacros($) {
my $ole2doc = shift;

  my @streams = $ole2doc->Streams;
  foreach (@streams) {
    if (($_->IsEmbedded) && ($_->IsMacroStream)) {
      return 1;
    }
  }
  return 0;
}


1;

