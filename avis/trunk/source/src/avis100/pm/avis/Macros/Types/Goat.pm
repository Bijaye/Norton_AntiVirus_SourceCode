package AVIS::Macro::Types::Goat;


##########################################
#
# Create a new one.
#
##########################################

sub new {
  my $class = shift;      # the class to bless into
  my $self = {};          # the anonymous hash to return

# The "private" instance variables
  if (@_) {
  }
  else {
    $self->{Type} = undef;
    $self->{TargetId}  = undef;
    $self->{Father}  = undef;
    $self->{Children}  = undef;
    $self->{After}  = undef;
    $self->{Before} = undef;
    $self->{Path} = undef;
    $self->{RunId} = undef;
    $self->{Convert} = "No";
    $self->{ID} = undef;
  }
  
# and we're done
  return bless $self, $class;
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

sub IsSameAs {
  my $self = shift;
  my $othergoat = shift;

  return 0;
}

############################################
#
# Methods
#
############################################

sub TargetId {
my $self = shift;

  if (@_) {return ($self->{TargetId} = shift());}
  else    {return $self->{TargetId};}
}

sub Type {
my $self = shift;

  if (@_) {return ($self->{Type} = shift());}
  else    {return $self->{Type};}
}

sub BackupCopy {
my $self = shift;

  if (@_) {return ($self->{Before} = shift());}
  else    {return $self->{Before};}
}

sub UsedCopy {
my $self = shift;

  if (@_) {return ($self->{After} = shift());}
  else    {return $self->{After};}
}

sub Father {
my $self = shift;

  if (@_) {return ($self->{Father} = shift());}
  else    {return $self->{Father};}
}

sub Children {
my $self = shift;

  return $self->{Children};
}

sub IncrementChildrenCount {
my $self = shift;

  return ++$self->{Children};
}

sub Path {
my $self = shift;

  if (@_) {return ($self->{Path} = shift());}
  else    {return $self->{Path};}
}

sub IsUpConvert($) {
my $self = shift;

  return ($self->{Convert} eq "Up") ? 1 : 0;
}

sub IsDownConvert($) {
my $self = shift;

  return ($self->{Convert} eq "Down") ? 1 : 0;
}

sub MarkAsUpConvert($) {
my $self = shift;

  return ($self->{Convert} = "Up");
}

sub MarkAsDownConvert($) {
my $self = shift;

  return ($self->{Convert} = "Down");
}

sub IsConvert($) {
  my $self = shift;
  
  return ($self->IsUpConvert()||$self->IsDownConvert()); 
}


#########################################################

1;

