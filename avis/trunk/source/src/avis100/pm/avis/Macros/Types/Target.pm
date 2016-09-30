package AVIS::Macro::Types::Target;


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
    $self->{FORMID} = shift;
    $self->{DOCTYPE}  = shift;    
    $self->{Count}  = 1;
    $self->{VIABLE}  = 1;
    $self->{ID} = shift if (@_);
  }
  else {
    $self->{FORMID} = undef;
    $self->{DOCTYPE}  = undef;
    $self->{Count}  = 1;
    $self->{VIABLE}  = 1;
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
  my $othertarget = shift;

  return 0 if ($self->{FORMID} ne $othertarget->{FORMID});
  return 0 if ($self->{APP} ne $othertarget->{APP});
  return 0 if ($self->{DOCTYPE} ne $othertarget->{DOCTYPE});
  return 1;
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

###########################################
#
#Access/set properties
#
###########################################

sub Formid {
my $self = shift;

  if (@_) {return ($self->{FORMID} = shift());}
  else    {return $self->{FORMID};}
}

sub Doctype {
my $self = shift;

  if (@_) {return ($self->{DOCTYPE} = shift());}
  else    {return $self->{DOCTYPE};}
}

sub Instances {
my $self = shift;

  if (@_) {return ($self->{INSTANCES} = shift());}
  else    {return $self->{INSTANCES};}
}

sub Viable {
my $self = shift;

  if (@_) {return ($self->{VIABLE} = shift());}
  else    {return $self->{VIABLE};}
}


#########################################################

1;

