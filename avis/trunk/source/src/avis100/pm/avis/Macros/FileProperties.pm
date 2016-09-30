####################################################################
#                                                                  #
# Program name:  pm/AVIS/Macros/FileProperties.pm                  #
#                                                                  #
# Package name:  AVIS::Macro::FileProperties                       #
#                                                                  #
# Description:   Determine properties of macro files               #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1999-2000                                     #
#                                                                  #
# Author:        Jean-Michel Boulay                                #
#                                                                  #
# And also:      U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################

package AVIS::Macro::FileProperties;

use AVIS::Macro::Macrocrc;
use AVIS::FSTools;
use Mcv;
use AVIS::Macro::OLE2Doc;


#tries to figure out whether the file is a global template, a document, a startup template
#as Word 8 crashes when you use a document as global template, figuring out the type of the document is important
sub guess_file_type($$) {
  my $file = shift;
  my $unc = shift;
  
  $file = AVIS::FSTools::Canonize($file, $unc);
  if (Mcv::IsWordDoc($file) && (Mcv::GetWordDocVersion($file) ==8) && (Mcv::WfwGetTemplateByte($file) ==0)) {
    return "DOC";
  }
  return "UNKNOWN";
}


sub IsViable($$) {
  my $file = shift;
  my $unc = shift;
  
  my $diag = NonViabilityDiagnostic($file, $unc);
    
  return (defined $diag) ? 0: 1;
}

sub NonViabilityDiagnostic($$) {
  my $file = shift;
  my $unc = shift;
  
  my $fullfile = AVIS::FSTools::Canonize($file, $unc);
  if (Mcv::IsWfw($fullfile) && (Mcv::WfwGetTemplateByte($fullfile) ==0)) {
    #sample is not viable if it is a Word 6/7 document and its template byte is not set
    return "Template byte is not set for $file";
  }
  if (AVIS::Macro::OLE2Doc::IsOLE2($fullfile)) {
    my $ole2doc = new AVIS::Macro::OLE2Doc($fullfile);
    return undef if not defined $ole2doc; 
    return undef if not defined $ole2doc->DocType;
    if ($ole2doc->IsEncrypted) {return "$file is password-protected";}
    if ($ole2doc->HasEmbeddedMacros && !$ole2doc->HasMainMacros) {return "All macros are contained in embedded documents";}
    if ($ole2doc->HasEmbeddedMacros && $ole2doc->HasMainMacros) {return "Some macros are contained in embedded documents";}
  }
  return undef;  
}


sub has_macros($$) {
  my ($fname, $mobject) = @_;

  return scalar @{$mobject->GetMacroList($fname)};
}

sub get_file_applications($$) {
  my ($fname, $mobject) = @_;

  my @mlist = @{$mobject->GetMacroList($fname)};
  # Return empty arrayref if no macros found
  if (@mlist == 0) {
    return ();
  }
  
  my ($v, %seenapp, $hashref);
  my @applist = ();
  foreach $hashref ( @mlist ) {
    $v = $hashref->{App};
    if (not $seenapp{$v}) {
      $seenapp{$v} = 1;
      push @applist, $v;
    }
  }
  return @applist;
}


sub get_file_languages($$$) {
  my ($fname, $mobject, $unc) = @_;
  
  my @langlist = ();
  my $file = AVIS::FSTools::Canonize($fname, $unc);
  if (Mcv::IsWfw($file) && (Mcv::GetWfwCs($file) == 2)) {
     my $cc = Mcv::GetWfwDBCSCountryCode($file);
     @langlist = ("TW") if ($cc == Mcv::TW);
     @langlist = ("JP") if ($cc == Mcv::JP);
     @langlist = ("KR") if ($cc == Mcv::KR);
     @langlist = ("CH") if ($cc == Mcv::CH);
  }
  else {
    my @mlist = @{$mobject->GetMacroList($fname)};
    # Return empty arrayref if no macros found
    if (@mlist == 0) {
#      $logger->logString("$fname contains no macros.");
      return ();
    }
    my ($v, %seenlang, $hashref);
    foreach $hashref ( @mlist ) {
      $v = guess_language($hashref->{Macroname});
      if (($v ne "") &&(not $seenlang{$v})) {
        $seenlang{$v} = 1;
        push @langlist, $v;
      }
    }
  }
  return @langlist;
}




#
# Given a macro name, return the language code that it suggests;
# return "EN" if nothing else special springs to mind.
#
sub guess_language($) {
  my $n = shift;

  #German
  return "DE" if $n =~ /datei/i;
  return "DE" if $n =~ /datum/i;
  return "DE" if $n =~ /extrasmakro/i;
  #Dutch
  return "NL" if $n =~ /bestand/i;
  #English
  #rem: file exists both in English and Italian
  return "EN" if $n =~ /tools/i;
  return "EN" if $n =~ /fileopen/i;
  return "EN" if $n =~ /filenew/i;
  return "EN" if $n =~ /filesave/i;
  return "EN" if $n =~ /fileclose/i;
  return "EN" if $n =~ /fileexit/i;
  #French
  return "FR" if $n =~ /fichier/i;
  return "FR" if $n =~ /ouvrir/i;
  return "FR" if $n =~ /outil/i;
  #Italian
  return "IT" if $n =~ /filesalva/i;
  return "IT" if $n =~ /fileapri/i;
  return "IT" if $n =~ /filechiudi/i;
  return "IT" if $n =~ /filenuovo/i;
  return "IT" if $n =~ /fileesci/i;
  return "IT" if $n =~ /strum/i;
  # Portuguese
  return "PT" if $n =~ /arquivo/i;
  return "PT" if $n =~ /ferram/i;     #
  # Spanish
  return "SP" if $n =~ /archivo/i;
  return "SP" if $n =~ /herram/i;     #

  # Need lots more here!

  return "";
}

1;
