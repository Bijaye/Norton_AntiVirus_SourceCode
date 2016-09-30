package AVIS::Macro::Implementations;


sub match_application_nls($$$) {
  my @nlscodes = @{shift()};
  my $appcode = shift;
  my $logger = shift;

  my $supported = {};  
  my $default = {};  
  my $ignore = 0;
  my $implemented = 1;
  $supported->{WD7}->{BR} = $implemented;
  $supported->{WD7}->{DT} = $implemented;
  $supported->{WD7}->{EN} = $implemented;
  $supported->{WD7}->{FR} = $implemented;
  $supported->{WD7}->{DE} = $implemented;
  $supported->{WD7}->{IT} = $implemented;
  $supported->{WD7}->{SP} = $implemented;
  $supported->{WD7}->{CH} = $ignore;
  $supported->{WD7}->{JP} = $implemented;
  $supported->{WD7}->{KR} = $ignore;
  $supported->{WD7}->{TW} = $implemented;
  $supported->{WD8}->{BR} = $implemented;
  $supported->{WD8}->{DT} = $implemented;
  $supported->{WD8}->{EN} = $implemented;
  $supported->{WD8}->{FR} = $implemented;
  $supported->{WD8}->{DE} = $implemented;
  $supported->{WD8}->{IT} = $implemented;
  $supported->{WD8}->{SP} = $implemented;
  $supported->{WD8}->{CH} = $ignore;
  $supported->{WD8}->{JP} = $ignore;
  $supported->{WD8}->{KR} = $ignore;
  $supported->{WD8}->{TW} = $ignore;
  $supported->{XL95}->{EN} = $implemented;
  $supported->{XL97}->{EN} = $implemented;
  $supported->{PP97}->{EN} = $implemented;

  $default->{XL95} = "EN";
  $default->{XL97} = "EN";
  $default->{WD7} = "EN";
  $default->{WD8} = "EN";
  $default->{PP97} = "EN";

  my %usablenls = ();
  if (!@nlscodes) {
    $usablenls{$default->{$appcode}} = 1;
  }
  foreach $nlscode (@nlscodes) {
    if (!exists $supported->{$appcode}->{$nlscode}) {
      #use the default
      $usablenls{$default->{$appcode}} = 1;
      #log something
      $logger->logString("Could not figure out the NLS version of the document. Defaulting to English.");
    }
    if ($supported->{$appcode}->{$nlscode} == $implemented) {
      $usablenls{$nlscode} = 1;
    }
    elsif ($supported->{$appcode}->{$nlscode} != $ignore) {
      $logger->logString("$nlscode is not supported for $appcode. Ignoring");
    }
  }
   
  return keys(%usablenls);
}


1;
