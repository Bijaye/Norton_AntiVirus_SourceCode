package AVIS::Macro::Types::List;


sub new {
  my $class = shift;      # the class to bless into
  my $self = {};          # the anonymous hash to return

# The "private" instance variables
  $self->{Items} = [];
  $self->{ItemType} = undef;
  
# and we're done
  return bless($self, $class);
}


####################################
#
# Modify
#
####################################

sub Add {
  my $self = shift;
  my $item = shift;

  #initialize the type if needed
  if (!defined($self->{ItemType})) {
    $self->{ItemType} = ref($item);
  }  
  
  #check that we're not mixing objects of different types
  if ($self->{ItemType} eq ref($item)) {  
    my $itemnumber = $self->ItemNumber();
    (my $defaultid = ref($item)) =~ s/^.*:://;
    $defaultid .= $itemnumber + 1;
    my $id = @_ ? $_[0] : $defaultid;

    $item->Id($id);
    push @{$self->{Items}}, $item;
    return 1;
  }
  else {
    warn "Attempt to add an item of type ".ref($item)." to a list of items of type ".$self->ItemType;
    return 0;
  }
}

sub RemoveItemById {
  my $self = shift;
  my $id = shift;

  my @olditems = $self->ItemList;
  my @newitems = ();
  foreach $listitem (@olditems) {
    if ($listitem->Id() ne $id) {push @newitems, $listitem;}    
  }
  $self->{Items} = [@newitems];  
  return (scalar @olditems - scalar @newitems);
}

####################################
#
# Properties
#
####################################

sub ItemList {
  my $self = shift;
  my $item = shift;

  return @{$self->{Items}};
}

sub ItemNumber {
  my $self = shift;
  my $item = shift;

  return scalar @{$self->{Items}};
}

sub Contains {
  my $self = shift;
  my $item = shift;

  my @items = $self->ItemList();
  foreach $listitem (@items) {
    return 1 if $listitem->IsSameAs($item);
  }  
  return 0;
}

sub FindSimilarItem {
  my $self = shift;
  my $item = shift;

  my @items = $self->ItemList;
  foreach $listitem (@items) {
    return $listitem if $listitem->IsSameAs($item);
  }  
  return undef;
}

sub FindItemById {
  my $self = shift;
  my $id = shift;

  my @items = $self->ItemList;
  foreach $listitem (@items) {
    return $listitem if ($listitem->Id() eq $id);
  }  
  return undef;
}

######################################

1;
