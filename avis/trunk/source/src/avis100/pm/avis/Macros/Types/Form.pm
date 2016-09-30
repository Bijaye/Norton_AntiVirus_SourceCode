package AVIS::Macro::Types::Form;

##########################################
#
# Create a new one.
#
##########################################

sub new($$$$) {
  my $class = shift;      # the class to bless into
  my $self = {};          # the anonymous hash to return

  my $delta = shift;

  @{$delta->{Delta}} = sort {
    $a->{CRC} cmp $b->{CRC} or
    $a->{Macroname} cmp $b->{Macroname};
  } @{$delta->{Delta}};
  my $thesenames = [];
  my $theseCRCs = [];
  my $theseLens = [];
  foreach $thismacro ( @{$delta->{Delta}} ) {
    push @$thesenames, $thismacro->{Macroname};
    push @$theseCRCs, $thismacro->{CRC};
    push @$theseLens, $thismacro->{Len};
  }

  $self->{Fname} = $delta->{Fname};
  $self->{App} = $delta->{Delta}[0]->{App};
  $self->{Mnames} = $thesenames;
  $self->{Count} = 1;
  $self->{Lengths} = $theseLens;
  $self->{Crcvals} = $theseCRCs; 
  $self->{ID} = undef; 

  return bless $self, $class;
}


############################################
#
# Methods
#
############################################
sub IncrementCount {
  my $self = shift;
  my $increment = shift||1;

  return ($self->{Count} += $increment);
}

############################################
#
# For use within the List package
#
############################################

#sets or returns the id of the form
sub Id {
  my $self = shift;
  if (@_) {
    $self->{ID} = $_[0];
  }
  return ($self->{ID});
}

sub IsSameAs($$) {
  my ($self, $otherform) = @_;
  return same_form($self, $otherform);
}






###########################################
#
# Misc functions
#
###########################################



###########################################
#
# Are these two forms the same?  (Looks at CRCs, ignores Fname)
#
sub same_form($$) {
  my ($a, $b) = @_;

  return ( ( $a->{App} eq $b->{App} )
            and same_list($a->{Mnames},$b->{Mnames})
            and same_list($a->{Crcvals},$b->{Crcvals}) );
}

#
# Are these two lists the same?
#
sub same_list($$) {
  my ($a, $b) = @_;
  my $i;

  return 0 if not @$a == @$b;

  for ($i=0;$i<@$a;$i++) {
    return 0 if not $a->[$i] eq $b->[$i];
  }
  return 1;
}


#########################################################

1;
