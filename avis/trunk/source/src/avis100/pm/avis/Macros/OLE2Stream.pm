####################################################################
#                                                                  #
# Program name:  pm/AVIS/Macros/OLE2Stream                         #
#                                                                  #
# Package name:  AVIS::Macro::OLE2Stream                           #
#                                                                  #
# Description:   Determine properties of OLE2 streams              #
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
# Determines properties of supported OLE2 streams, based on the    #
# attributes recognized by Symantec's ole2docnf.exe                #
#                                                                  #
####################################################################

package AVIS::Macro::OLE2Stream;

use strict;

my @macrotypes = ('VB', 'VBA', 'WB');
my @doctypes = ('X95', 'X97', 'W95', 'W97', 'W??');
my @streamtypes = @macrotypes, @doctypes;
my @encryptionstates = ('PLAIN', 'ENCRYPT');  


sub new {
  my $class = shift;
  my $type = shift;
  my $attributes = shift;
  
  my $self = {};
  $self->{'TYPE'} = $type;
  $self->{'ATTRIBUTES'} = $attributes;
  
  return bless $self, $class;    
}


sub Type {
  my $stream = shift;
  
  return (@_) ? $stream->{'TYPE'} = shift : $stream->{'TYPE'};
}


sub IsMacroStream {
  my $stream = shift;
  my $type = $stream->Type;
  return scalar grep {$_ eq $type} @macrotypes;
}

sub IsDocStream {
  my $stream = shift;
  my $type = $stream->Type;
  return scalar grep {$_ eq $type} @doctypes;
}


sub IsAttributeSet {
  my $stream = shift;
  my $attribute = shift;
  
  my @attributes = @{$stream->{'ATTRIBUTES'}};
  return scalar grep {$_ eq $attribute} @attributes;
}

sub SetAttribute {
  my $stream = shift;
  my $attribute = shift;
  
  push @{$stream->{'ATTRIBUTES'}}, $attribute;
  return 1;
}

sub ClearAttribute {
  my $stream = shift;
  my $attribute = shift;
  
  my @attributes = @{$stream->{'ATTRIBUTES'}};
  @{$stream->{'ATTRIBUTES'}} = grep {$_ ne $attribute} @attributes;
  return 1;
}


sub IsEncrypted {
  my $stream = shift;
  return 1 if $stream->IsAttributeSet('ENCRYPT');
  return 0 if $stream->IsAttributeSet('PLAIN');
  return -1;
}

sub IsEmbedded {
  my $stream = shift;
  return 1 if $stream->IsAttributeSet('EMBEDDED');
  return 0;

}

sub MarkAsEmbedded {
  my $stream = shift;
  return $stream->SetAttribute('EMBEDDED');
}

1;

