package AVIS::Macro::Types::Delta;

use AVIS::Macro::Macrocrc;

##########################################
#
# Create a new one.
#
##########################################

sub new($$$$) {
  my $class = shift;      # the class to bless into
  my $self = {};          # the anonymous hash to return

  my ($before, $after, $mobject) = @_;
  my ($thismacro, $already, @answer);

  my @beforelist = @{$mobject->GetMacroList($before)};
  my @afterlist = @{$mobject->GetMacroList($after)};
  my @delta = ();

  foreach $thismacro ( @afterlist ) {
    $already = 0;
    foreach $thatmacro ( @beforelist ) {
      if (same_macro($thismacro,$thatmacro)) {
        $already = 1;
        last;
      }
    }
    push @delta, $thismacro if not $already;
  }

  $self->{Fname} = $after;
  $self->{Delta} = \@delta;

  return bless $self, $class;
}


#########################################################
#
# Are these two macros the same macro?
#
#########################################################
sub same_macro {
  my ($a, $b) = @_;

  return ( $a->{App} eq $b->{App}
            and $a->{Macroname} eq $b->{Macroname}
            and $a->{CRC} eq $b->{CRC} );
}



#########################################################
#
# Special Properties
#
#########################################################

sub IsEmpty {
my $self = shift;

    @{$self->{Delta}} = sort {
      $a->{CRC} cmp $b->{CRC} or
      $a->{Macroname} cmp $b->{Macroname};
    } @{$self->{Delta}};
    my $thesenames = [];
    my $theseCRCs = [];
    my $theseLens = [];
    foreach $thismacro ( @{$self->{Delta}} ) {
      push @$thesenames, $thismacro->{Macroname};
      push @$theseCRCs, $thismacro->{CRC};
      push @$theseLens, $thismacro->{Len};
    }
    return !(scalar(@$thesenames));
}

sub HasOneApp {
my $self = shift;

  for ($i=1;$i<@{$self->{Delta}};$i++) {
     if ($self->{Delta}[$i]->{App} != $self->{Delta}[0]->{App}) {
        return 0;
     }
  }
  return 1;
}



1;
