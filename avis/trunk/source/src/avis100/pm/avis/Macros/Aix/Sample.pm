####################################################################
#                                                                  #
# Package name:  AVIS::Macro::Sample                               #
#                                                                  #
# Description:                                                     #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1999                      #
#                                                                  #
# Author:        Jean-Michel Boulay, Morton Swimmer                #
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

package AVIS::Macro::Sample;

#use Macro::Mcv;

my (@list1, @list2);

my %fields = (
   name        => undef, #virus name
   file        => undef, #sample path
#got by analyzing the sample
   app         => undef, #values: "WORD", "EXCEL"
   apprls      => undef, #release: 7 or 8 for Word and Excel 
   macrotype   => undef, #values: "Wordbasic", "VBA3", "VBA5"
   nls         => \@list1, #reference to a list of values
#desired settings for replication
   rapp        => undef, #values: "WORD", "EXCEL"
   rapprls     => undef, #release: 7 or 8
   rnls        => \@list2, #reference to a list of values
   doctype     => "UNKNOWN", #values: "DOC", "GLOBAL", "STARTUP", "UNKNOWN"
   docpath     => undef #path that should be given to the file for suitable replication (essentially for startup files)
   );


#add some entries to the list of languages
sub AddToNlsList {
my $this = shift;

  my $ref = $this->nls();
  push @$ref, @_;
}


sub AddToRnlsList {
my $this = shift;

  my $ref = $this->rnls();
  push @$ref, @_;
}


#basic methods

sub new {
  my $that = shift;
  my $class = ref($that)||$that;
  my $self = {%fields};
  bless $self, $class;
  return $self;
}


sub AUTOLOAD {
  my $self = shift;
  my $type = ref($self)||warn "$self is not an object";
  my $name = $AUTOLOAD;
  $name =~ s/.*://; #strip fully-qualified portion
  unless (exists $self->{$name}) {
    warn "Can't access '$name' field in object of class $type";
  }
  if (@_) {
    return $self->{$name} = shift;
  } else {
    return $self->{$name};
  }
}


sub DESTROY {
#  warn "Destructor called";
}
