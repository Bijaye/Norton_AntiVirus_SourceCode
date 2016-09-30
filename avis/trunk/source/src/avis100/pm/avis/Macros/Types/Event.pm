package AVIS::Macro::Types::Event;


##########################################
#
# Constructor
#
##########################################

sub new {
  my $class = shift;      # the class to bless into
  my $self = {};          # the anonymous hash to return

  if ((@_ ==1) && (ref($_[0]) eq $class)) {#copy constructor
    my $original = shift;
    %$self = %$original;
    $self->{POSTPROCESSED} = 0;
    $self->{RESCHEDULE} = 0;
  }
  else {#standard constructor
    $self->{OPERATION} = undef; #operation type: "Replication" or "Conversion"
    $self->{FILE} = undef; #virus sample
    $self->{APP} = undef; #application code
    $self->{LANG} = undef; #nls version of the application
    $self->{CMDS} = undef; #command file to use
    $self->{POSTPROCESSED} = 0; #event hasn't been postprocessed yet
    $self->{REPEATIFNEEDED} = 1; #can be rescheduled if led to a rare replicant
    $self->{REPEATONFAILURE} = 1; #can be rescheduled in case of failure
    $self->{RESCHEDULE} = 0; #reschedule
    $self->{SMPPATH} = undef; #specifies whether the virus sample must occupy a specific location
    $self->{CNVAPP} = undef; #destination application (conversions only)
    $self->{DATE} = undef; #(replications only)
    $self->{TIME} = undef; #(replications only)
  }
  
  return bless $self, $class;
}



############################################
#
# Methods
#
############################################
sub MarkAsPostProcessed {
  my $self = shift;
  $self->{POSTPROCESSED} = 1;
  return 1;
}

sub MarkAsNotPostProcessed {
  my $self = shift;
  $self->{POSTPROCESSED} = 0;
  return 1;
}

sub NeedsPostProcessing {
  my $self = shift;
  return !$self->{POSTPROCESSED};
}

sub IsRepeatableIfNeeded {
  my $self = shift;
  return $self->{REPEATIFNEEDED};
}

sub IsRepeatableOnFailure {
  my $self = shift;
  return $self->{REPEATONFAILURE};
}

sub DisableRepetitionIfNeeded {
  my $self = shift;
  $self->{REPEATIFNEEDED} = 0;
  return 1;
}

sub DisableRepetitionOnFailure {
  my $self = shift;
  $self->{REPEATONFAILURE} = 0;
  return 1;
}

sub EnableRepetitionIfNeeded {
  my $self = shift;
  $self->{REPEATIFNEEDED} = 1;
  return 1;
}

sub EnableRepetitionOnFailure {
  my $self = shift;
  $self->{REPEATONFAILURE} = 1;
  return 1;
}

sub Reschedule {
  my $self = shift;
  $self->{RESCHEDULE} = 1;
  return 1;
}

sub DoNotReschedule {
  my $self = shift;
  $self->{RESCHEDULE} = 0;
  return 1;
}

sub ApprovedForRescheduling {
  my $self = shift;
  return $self->{RESCHEDULE};
}

sub IsConversion {
  my $self = shift;
  return ($self->{OPERATION} eq "Conversion");
}

sub IsReplication {
  my $self = shift;
  return ($self->{OPERATION} eq "Replication");
}

sub SetAsConversion {
  my $self = shift;
  $self->{OPERATION} = "Conversion";
  return 1;
}

sub SetAsReplication {
  my $self = shift;
  $self->{OPERATION} = "Replication";
  return 1;
}

sub SampleFile {
my $self = shift;  

  if (@_) {
    $self->{FILE} = shift;
    return 1;
  }
  else {
    return $self->{FILE};
  }
}

sub Application {
my $self = shift;  

  if (@_) {
    $self->{APP} = shift;
    return 1;
  }
  else {
    return $self->{APP};
  }
}

sub Nls {
my $self = shift;  

  if (@_) {
    $self->{LANG} = shift;
    return 1;
  }
  else {
    return $self->{LANG};
  }
}

sub Commands {
my $self = shift;  

  if (@_) {
    $self->{CMDS} = shift;
    return 1;
  }
  else {
    return $self->{CMDS};
  }
}

sub SamplePath {
my $self = shift;  

  if (@_) {
    $self->{SMPPATH} = shift;
    return 1;
  }
  else {
    return $self->{SMPPATH};
  }
}

sub FinalApplication {
my $self = shift;  

  if (@_) {
    $self->{CNVAPP} = shift;
    return 1;
  }
  else {
    return $self->{CNVAPP};
  }
}

sub Date {
my $self = shift;  

  if (@_) {
    $self->{DATE} = shift;
    return 1;
  }
  else {
    return $self->{DATE};
  }
}

sub Time {
my $self = shift;  

  if (@_) {
    $self->{TIME} = shift;
    return 1;
  }
  else {
    return $self->{TIME};
  }
}

#########################################################

1;
