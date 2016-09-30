
=head1 NAME

Package: TypeMgr

Filename: src/avis100/pm/typemanager/TypeMgr.pm

Heuristics for deciding what kind of file a file is.

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1999

U.S. Government Users Restricted Rights - use,
duplication or disclosure restricted by GSA ADP
Schedule Contract with IBM Corp.

=head1 AUTHORS

 Till Teichmann
 David M. Chess

=head1 HISTORY

See VSS and CMVC

=head1 BUGS

The rules used to determine what a file is are somewhat heuristic,
and may sometimes give the wrong answer.  This is especially true of
obscure types like RIFF and ZOO.  Important things like EXE, NE, PE,
OLE, and so on will be correct as far as we know.  COM will be
included only if no other type seems to fit (since anything can
be a COM file).

=cut


package TypeMgr;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = (FileType);
@EXPORT_OK = @EXPORT;

use Mcv;
use AVIS::Logger;

use strict;

1; # OK to load module

=head1 TypeMgr::FileType($filename,$logger)

returns a reference to a hash.  The keys of the hash reflect the kind(s)
of file that the given file appears to be.  (The values in the hash
are undocumented, but are in fact all "1".)  If a nonzero $logger is
provided, it's assumed to be an AVIS::Logger to which logging should be
done.

The possible hash keys returned include:

  NOFILE - The file did not exist.

  EMPTY - The file was zero bytes long, or could not be read.

  EXE - A file that starts with MZ or ZM, and will therefore
        be treated as EXE format by the DOS/Windows loader

  NE - A file with an NE in the proper place, which will
       generally be treated as Win 3.1 format by the loader

  PE - A file with a PE in the proper place, which will
       generally be loaded as a 32-bit Portable Executable

  BOOTSECTOR - A file with 55 aa at the end of the first
       512 bytes, which is also a multiple of 512 bytes
       long, which may therefore be a disk image.

  OLE - A file that carries the OLE2 multiple-stream-storage
        signature, and is therefore probably an Office document
        or something boring and macro-ridden like that.

  COM - Any file which isn't in the EXE or OLE category, and which
        we might therefore speculate is intended to be loaded
        as a flat memory image by the program loader

Other keys may also be set; if any of them are important, they
should be added to this documentation.

=cut

sub FileType ($$) {

   my (@extfiletype,@extsearchstr,@extstartpos,@extendpos,@extlength,@extnumstr,@extloops);
   my (@exefiletype,@exesearchstr,@exestartpos,@exeendpos,@exelength,@exenumstr,@exeloops);

   my ($file,$logfile)=@_;
   my $bytes_to_scan = 1000;
   my ($type)=0;
   my (%typ,$i,$j,$buffer,$count);

   #
   # initialize the heuristic search strings
   #
   my @EXE = getExeList();
   my @EXT = getExtList();
   for ($i=0;$i<=$#EXE;$i++){
      ($exefiletype[$i],$exesearchstr[$i],$exestartpos[$i],$exeendpos[$i],$exelength[$i],$exenumstr[$i],$exeloops[$i])=split /\,/,$EXE[$i];
   }
   for ($i=0;$i<=$#EXT;$i++){
      ($extfiletype[$i],$extsearchstr[$i],$extstartpos[$i],$extendpos[$i],$extlength[$i],$extnumstr[$i],$extloops[$i])=split /\,/,$EXT[$i];
   }

   # Is the file in fact a file?
   if (not -f $file) {
      $typ{"NOFILE"}=1;
      $logfile->logString("File <$file> not found in TypeMgr.pm") if $logfile;
      return(\%typ);
   }

   #
   # Is it empty?
   #
   my $flen = -s $file;
   if ($flen==0) {
      $typ{"EMPTY"}=1;
      $logfile->logString("File <$file> is empty in TypeMgr.pm") if $logfile;
      return(\%typ);
   }

   #
   # open and read the start of the file
   #
   if (not open (OUT,"<$file")) {
      $typ{"EMPTY"}=1;
      $logfile->logString("Error <$!> opening <$file> in TypeMgr.pm");
      return(\%typ);
   }
   binmode(OUT);
   read(OUT,$buffer,$bytes_to_scan);
   close (OUT);

   # Do pattern-based heuristics on the start of the file just read

   #
   # look in the file for the various search strings
   #
   for ($j=0; $j<=$#EXT; $j++){
      $type=SearchBytes($buffer,$extfiletype[$j],$extsearchstr[$j],$extstartpos[$j],$extendpos[$j],$extlength[$j],$extnumstr[$j],$extloops[$j]) || "";
      if ($type ne "") {
         $typ{$type}=1;
         if ($type eq "BOOTSECTOR") {  # If BOOTSECTOR, sanity-check length
           delete $typ{$type} if ($flen % 512);
         }
         if ($type =~ /EXE/) {  # If EXE, check EXE subtype heuristics
            my $ft = "";
            for ($i=0; $i<$#exefiletype; $i++){
               $ft=SearchBytes($buffer,$exefiletype[$i],$exesearchstr[$i],$exestartpos[$i],$exeendpos[$i],$exelength[$i],$exenumstr[$i],$exeloops[$i]);
                if ($ft ne "") {
                  $typ{$ft}=1;
               }
            }
            #
            # NE and PE can't be reliably detected by just looking for
            # strings in the first 1000 bytes, so do more reliable tests
            #
            {
              local *FH;
              my ($xbuf,$xofs);
              open FH,"<$file";             # Open the input file again
              binmode FH;                   #  for binary, of course
              $count = read(FH,$xbuf,0x40); # Read the EXE header
              if ($count==0x40) {           # Only if it's long enough,
                $xbuf = substr($xbuf,0x3c); # Look at offset 0x3c,
                $xofs = unpack("V",$xbuf);  # for an Intel long.
                if (seek(FH,$xofs,0)) {     # If we can seek there,
                  $count = read(FH,$xbuf,2);#   read two bytes.
                  if ($count==2) {          # If that worked,
                    $typ{"NE"} = 1          #   set PE
                      if ($xbuf eq "NE");   #
                    $typ{"PE"} = 1          #   or NE
                      if ($xbuf eq "PE");   #    as appropriate
                  }  # end read two bytes
                }  # end seek worked
              }  # end read 0x40 bytes
              close FH;
            }  # end NE or PE possible
         }
         if ($type eq "OLE") {  # If OLE, use MCV for OLE subtypes
            #
            # IsOLE2($file): returns 1 if $file is an OLE2 file, else 0
            #
            if (Mcv::IsWfw($file)==1){
               #
               # returns 1 if $file is a Word6/7 document, else 0
               #
               $typ{"WORD"}=1;
            }
            if (Mcv::IsVBA5($file)==1){
               $typ{"VBA5"}=1;
            }
            if (Mcv::IsXl($file)==1){
               #
               # returns 1 if $file is an Excel 4/7 document
               #
               $typ{"EXCEL"}=1;
            }
            if (Mcv::GetWfwCs($file)==2){
               #
               # returns 2 if $file is a Word 6/7 DBCS document, 1 if a SBCS doc, else: 0
               #
               $typ{"DBCS"}=1;
            }
            if (Mcv::GetWfwCs($file)==1){
               $typ{"SBCS"}=1;
            }
            if (Mcv::GetWordDocVersion($file)==6){
               #
               # returns 6 or 7 for Word 6/7 documents, 8 for Word
               #  8 documents (based on the first word of the WordDocument stream); else: 0
               #
               $typ{"WORD6/7"}=1;
            }
            if (Mcv::GetWordDocVersion($file)==7){
               $typ{"WORD6/7"}=1;
            }
            if (Mcv::GetWordDocVersion($file)==8){
               $typ{"WORD8"}=1;
            }
            if (Mcv::GetXlDocVersion($file)==7){
               #
               # returns 7 if a "Book" stream is found, 8 if a "Workbook" stream is found, 0 else.
               #
               $typ{"BOOK"}=1;
            }
            if (Mcv::GetXlDocVersion($file)==8){
               $typ{"WORKBOOK"}=1;
            }
            if (Mcv::WfwGetTemplateByte($file)==0){
               #
               # returns (template byte)&0x01, or -1 if an error occurs.
               #
               $typ{"NOTEMPLATEBYTE"}=1;
            }
            if (Mcv::WfwGetTemplateByte($file)==1){
               #
               # returns (template byte)&0x01, or -1 if an error occurs.
               #
               $typ{"TEMPLATE"}=1;
            }
            #  WfwSetTemplateByte($file, $value): sets the template byte of the document to $value; returns 1 if success, 0 if failure.
            if (Mcv::IsXlDoc($file)==1){
               #
               #  returns 1 if a "Book" or "Workbook" stream is found
               #
               $typ{"BOOK/WORKBOOK-STREAM"}=1;
            }
            if (Mcv::IsWordDoc($file)==1){
               #
               # returns 1 if a "WordDocument" stream is found
               #
               $typ{"WORD-STREAM"}=1;
            }  # end if IsWordDoc
         }  # end if OLE
      }  # end if the type matched
   }  # end for each type

   #
   # Does it look like a text file?
   #
   if ($buffer =~ /^[A-Za-z0-9.,<>_"':;!?\r\n\t()\\=\+\-#\[\]%&{}\*\s]+$/) {
      $logfile->logString("Text file\n") if $logfile;
      $typ{"TEXT"}=1;
   }

   #
   # if not an EXE or OLE, include COM, to be safe.
   #
   if (not ($typ{"EXE"} or $typ{"OLE"})) {
      $typ{"COM"}=1;
   }

   return (\%typ);
}

#
# This routine looks in the given buffer for the given pattern (which may
# be a regexp), and returns the given filetype string if it's found,
# and returns undef otherwise.  The rest of the arguments have to do with
# exactly where in the buffer the string should appear, but details are
# hazy.  The last two arguments, for instance, seem to be ignored.  Anyone
# who figures out what they all do is urged to document it here.
#
sub SearchBytes($$$$$$$$) {
   my ($buffer,$extfiletype,$extsearchstr,$extstartpos,$extendpos,$extlength,$extnumstr,$extloops) = @_;
   my ($index1,$index2);
   my (@bufferarray);
   my (@strarray);
   my ($type);
   #
   # search string can be everywhere
   #
   if ($extlength==-1) {
      if ($buffer =~ /$extsearchstr/) {
         $type=$extfiletype;
         return($extfiletype);
      }
      return;
   }
   #
   # search string must be at the beginning of the file
   #
   if ($extendpos==-1) {
      if ($buffer =~ /^$extsearchstr/) {
         $type=$extfiletype;
         return($extfiletype);
      }
      return;
   }
   #
   # search string from a special position to a special end position
   #
   $buffer = substr($buffer, $extstartpos, $extendpos + $extlength - $extstartpos);
   if (!($buffer =~ /$extsearchstr/)) {
     return;
   }
   return($extfiletype);
}

#
# These records reflect various kinds of files, and heuristics to
# help identify each one.  The first field of each record is the
# human-readable name of the kind, the second field is a pattern
# to search for, and the rest of the arguments are the corresponding
# tail arguments to SearchBytes, in case anyone ever figures out what
# those actually do.
#
sub getExtList {
  return (
    "EXE,MZ,0,-1,2,1,0",
    "EXE,ZM,0,-1,2,1,0",
    "OLE,\\xd0\\xcf\\x11\\xe0\\xa1\\xb1\\x1a\\xe1,0,-1,8,1,0",
    "ZIP,\\x50\\x4b\\x03\\x04,0,-1,4,1,0",
    "ZIP,\\x50\\x4b\\x05\\x06,0,-1,4,1,0",

    "RIFF,RIFF,0,-1,4,2,0",
    "ARJ,\\x60\\xea,0,-1,2,1,0",
    "ARJ,aRJsf,0,30,5,1,0",
    "AIF,DOCTYPE HTML PUBLIC,0,100,-1,1,0",
    "AU,.SND,0,-1,4,1,0",
    "AU,.snd,0,-1,4,1,0",
    "ANI,ACON,0,100,-1,3,0",
    "ANI,anih,0,100,-1,3,0",
    "AVI,AVI,0,100,-1,2,0",
    "BAT,\@ECHO OFF,0,100,-1,3,0",
    "AT,\@echo off,0,100,-1,3,0",
    "BAT,REM ,0,100,-1,3,3",
    "BAT,rem ,0,100,-1,3,3",
    "BGI,pk\\b\\bBGI Device Driver,0,100,-1,1,0",
    "BGI,pk\\b\\bBGI Device Driver,0,100,-1,1,0",
    "BMP Windows,BM,0,-1,2,1,0",
    "BMP OS\/2,BA,0,-1,2,1,0",
    "BMP OS\/2,CI,0,-1,2,1,0",
    "BMP OS\/2,CP,0,-1,2,1,0",
    "BMP OS\/2,IC,0,-1,2,1,0",
    "BMP OS\/2,PT,0,-1,2,1,0",
    "BBK,ADLIB-,0,100,-1,1,0",
    "BOOTSECTOR,\\x55\\xaa,510,510,2,1,0",
    "CAB,MSCF,0,-1,4,1,0",
    "CAC,\\x3f\\x5f\\x03\\x00,0,-1,4,1,0",
    "CAL,\\xb5\\xa2\\xb0\\xb3\\xb3\\xb0\\xa2\\xb5,0,100,-1,1,0",
    "CDA,CDDA,0,100,-1,1,0",
    "CDR,MGC,0,-1,3,1,1",
    "CGI,\\xffFONT  ,0,-1,7,1,0",
    "CHR,PK\\b\\bBGI,0,-1,9,1,0",
    "CUR,\\x00\\x00\\x02\\x00\\x01\\x00\\x20\\x20\\x00\\x00,0,100,-1,1,0",
    "Compress5,SZDD,0,-1,4,1,0",
    "Compress6,KWAJ,0,-1,4,1,0",
    "DCX,\\x98\\x76\\x54\\x32,0,-1,4,1,0",
    "GEM,\\x00\\x01\\x00\\x08,0,-1,4,1,0",
    "GEM,\\xff\\xff\\x18\\x00,0,-1,4,1,0",
    "GIF87a,GIF87a,0,-1,6,1,0",
    "GIF89a,GIF89a,0,-1,6,1,0",
    "GRP,PMC,0,100,-1,1,0",
    "GZIP,\\x1f\\x8b,0,-1,2,1,0",
    "HLP,\\x3f\\x5f,0,-1,2,2,0",
    "HLP,\\xff\\xff\\xff\\xff,0,100,-1,2,0",
    "HTML,<HTML>,0,100,-1,2,0",
    "HTML,<HEAD>,0,100,-1,2,0",
    "HTML,<P>,0,100,-1,3,0",
    "HTML,HREF=\",0,100,-1,3,0",
    "HTML,<H2 ,0,100,-1,3,0",
    "IBK,\\x49\\x42\\x3b\\x1a,0,100,-1,1,0",
    "ICO,\\x00\\x00\\x01\\x00\\x02\\x00\\x20\\x20\\x10\\x00\\x00\\x00\\x00\\x00\\xe8\\x02\\x00\\x00\\x26\\x00,0,-1,20,1,0",
    "ICO,\\x00\\x00\\x01\\x00\\x02\\x00\\x20\\x20\\x10\\x00\\x00\\x00\\x00\\x00\\xe8\\xe2\\x00\\x00\\x26\\x00,0,-1,20,1,0",
    "JAVA,\\xca\\xfe\\xba\\xbe,0,100,-1,1,0",
    "JPG,JFIF,0,100,-1,2,0",
    "JPG,\\xff\\xd8\\xff\\xe0,0,100,-1,2,0",
    "LIB,!<arch>,0,-1,7,1,0",
    "LHZ,-lh,0,-1,2,1,0",
    "LHZ,-lz,0,-1,2,1,0",
    "LNK,\\x3c\\x00\\x00\\x00\\x01\\x14\\x02\\x00\\x00\\x00\\x00\\x0c,0,-1,12,1,0",
    "MIDI,MThd,0,-1,4,2,0",
    "MIDI,MTrk,0,100,-1,2,0",
    "MOV,mdat,0,100,-1,2,0",
    "MOV,moov,0,100,-1,2,0",
    "MPG,\\xff\\xfd\\x60\\xcc,0,100,-1,1,0",
    "MPG,\\xff\\xfa\\x80\\x04,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x80\\x04,0,-1,4,1,0",
    "MPG,\\xff\\xfa\\x80\\x44,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x80\\x44,0,-1,4,1,0",
    "MPG,\\xff\\xfa\\x80\\x64,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x80\\x64,0,-1,4,1,0",
    "MPG,\\xff\\xfa\\x90\\x04,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x90\\x04,0,-1,4,1,0",
    "MPG,\\xff\\xfa\\x90\\x44,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x90\\x44,0,-1,4,1,0",
    "MPG,\\xff\\xfa\\x90\\x64,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x90\\x64,0,-1,4,1,0",
    "MPG,\\xff\\xfa\\x90\\x04,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x90\\x04,0,-1,4,1,0",
    "MPG,\\xff\\xfa\\x90\\x44,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x90\\x44,0,-1,4,1,0",
    "MPG,\\xff\\xfa\\x90\\x64,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x90\\x64,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x90\\x04,0,-1,4,1,0",
    "MPG,\\xff\\xfb\\x58\\xc4,0,-1,4,1,0",
    "MPG Video,\\x00\\x00\\x01\\xb3,0,-1,4,1,0",
    "PAL,AH,0,-1,2,2,0",
    "PAL,Dr. Halo,0,100,-1,2,0",
    "PCX,\\x0a\\x00,0,-1,2,2,0",
    "PCX,\\x0a\\x02,0,-1,2,2,0",
    "PCX,\\x0a\\x03,0,-1,2,2,0",
    "PCX,\\x0a\\x05,0,-1,2,2,0",
    "PCX,XMIN,0,100,-1,4,1,0",
    "PCX,YMIN,0,100,-1,4,1,0",
    "PCX,XMAX,0,100,-1,4,1,0",
    "PCX,YMAX,0,100,-1,4,1,0",
    "PDF,%PDF,0,100,-1,1,0",
    "PFB,PS-AdobeFont,0,100,-1,1,0",
    "PSD,8BPS,0,100,-1,1,0",
    "PIC,\\x12\\x34,0,-1,2,1,0",
    "RAM,\\x2e\\x52\\x4d\\x46\\x00\\x00\\x00\\x12\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x05\\x50\\x52\\x4f\\x50,0,100,-1,1,0",
    "RTF,\\x7b\\x5c\\x72\\x74\\x66,0,100,-1,1,0",
    "RTF,\\x7b\\x72\\x74\\x66,0,100,-1,1,0",
    "RTF,\{\\rtf,0,-1,4,1,0",
    "SBI,\\x53\\x42\\x49\\x1a,0,-1,4,1,0",
    "SMF,\\x4d\\x54\\x68\\x64,0,-1,4,1,0",
    "VBScript,script language=\"VBScript\",0,100,-1,2,0",
    "VOC,Creative Voice File,0,-1,11,1,0",
    "WAV,WAVE,0,100,-1,2,0",
    "WMF,\\xd7\\xcd\\xc6\\x9a\\x00\\x00,0,-1,6,1,0",
    "WRI,\\x31\\xbe\\x00\\x00\\x00\\xab\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00,0,-1,13,1,0",
    "WRI,\\x32\\xbe\\x00\\x00\\x00\\xab\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00,0,-1,13,1,0",
    "ZOO,ZOO 2.00 Archive,0,100,-1,1,0",
    "ZOO,ZOO 2.10 Archive,0,100,-1,1,0",
    "ZOO,\\xfd\\xc4\\xa7\\xdc,0,-1,4,1,0",
  );
}


#
# These records are in the same format as those provided by
# getExtList().  They are used to further identify (or, rather,
# heuristically guess at) types of files that are subtypes of EXE.
# Note that NE and PE subtypes are not handled here, since they
# require more than a simple string-scan.
#
sub getExeList {
  return(
    "ARJ self extracting,RJSX,0,100,-1,1,0",
    "LZ self extracting,LZ09,0,100,-1,1,0",
    "LZ self extracting,LZ91,0,100,-1,1,0",
    "PKLITE,PKLITE,0,100,-1,1,0",
    "LHARC self extracting,LHarc's SFX,0,100,-1,1,0",
    "LHA self extracting,LHa's SFX,0,100,-1,1,0",
    "LHA self extracting,LHA's SFX,0,100,-1,1,0",
    "LHA self extracting,LH's SFX,0,100,-1,1,0",
    "LARC self extracting,SFX by LARC,0,100,-1,1,0",
    "FONT,FONTDIR,0,100,-1,1,0",
  );
}

