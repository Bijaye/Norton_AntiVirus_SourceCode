#!/usr/bin/perl
###################################################################
#                                                                  #
# Program name:  vanalysis.pl                                      #
#                                                                  #
# Description:   binary virus analysis.                            #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Andy Raybould                                     #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#    creates DEFPAK output on stdout given an autoverv.ers file on #
#    stdin.                                                        #
#                                                                  #
####################################################################
use lib ('//Shine/TestZoneShine/perl/lib/site'); #!!testing only

my ($flags,$stop,$opcodes,@sigs) = @ARGV; #PAM!!

my $doesRepair = ($flags =~ m/R/);
my $doesCOM    = ($flags =~ m/C/);
my $doesEXE    = ($flags =~ m/X/);
(my $length = $flags) =~ s/\D//g;

my $date = scalar localtime;

$opcodes =~ s/(.{64})/$1\n/g;   # wrap at 64 chars per line

while (<DATA>) {
   if     (m/^   !copy the ERS entries here!$/) {
              if ($doesRepair) {while (<STDIN>) {print;}}
   }elsif (m/^   !copy the signatures here!$/) {
              foreach (@sigs) {print "$_\n";}
   }elsif (m/^   !copy the opcodes here!$/) { #PAM!! tbd
              print "$opcodes\n";
   }elsif (m/^\[Control_Flags\]/) {
              print '[Control_Flags]  = '. ($doesRepair ? 'PAM_REPAIR' : 'NO_CLEAN') ."\n";
   }elsif (m/^\[Info_Flags\]/) {
              print '[Info_Flags]     = WILD'. ($doesCOM ? ',COM' : '') . ($doesEXE ? ',EXE' : '') ."\n";
   }elsif (m/^\[Infection_Size\]/) {
              print "[Infection_Size] = #$length\n";
   }elsif (m/^\[PAM_FLAGS\]/) {
              print "[PAM_FLAGS] $stop, ";   # no \n; concatenate next line
   }else {
              s/<Date>/$date/;
              print;
   }
}
exit 0;

__END__
[DOSDEFSTART]
DEFTYPE = PAM
DEFPREV = WILD

[VIRSCANSTART]

[CommentBegin]
Virus Name    : <VirusName>
Def Author    : Automation
Revision Date : <Date>
VID           : <VirusID>
[CommentEnd]

[DEFBEGIN]
[Build_Status]   = PASSED
[Virus_Type]     = ALG
[Virus_ID]       = 0x<VirusID>
[Virus_Name]     = "<VirusName>"
[Virus_CRC]      = 0x0000
[Control_Flags]  = PAM_REPAIR
[Info_Flags]     = WILD,COM,EXE
[Fixed_ID]       = 0x0000
[Memory_Sig]     = None
[Signature]      = 0xDF, RetFalse()
[Infection_Size] = #0
[Tag_Size]       = #0
[Host_Jump_Loc]  = 0x0000
[Jump_Length]    = #0
[SS_Offset]      = 0xffff
[SP_Offset]      = 0xffff
[CS_Offset]      = 0xffff
[IP_Offset]      = 0xffff
[Header_Loc]     = 0xffff
[Header_Length]  = 0xffff
[Book_Mark_Loc]  = 0x0000
[Book_Mark]      = 0x<VirusID>
[DEFEND]

[VIRSCANEND]


[ERSDEFSTART]

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Virus     : <VirusName>
; Virus ID  : <VirusID>
; Author    : Automation
; Date      : <Date>
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[RepairStart]

[VirusID]
0x<VirusID>

[SigStart]
DONE
[SigEnd]

[VirusExecutionCap]
200000

[RepairExecutionCap]
200000

[RepairFile]
<ERSSourceFile>.dat             ; filled in by INTEGRATION

[RepairEnd]

[ERSDEFEND]

[ERSCODESTART]
;===================================================================
;
; Description:   Immune system binary virus verification & repair.
;
; Statement:     Licensed Materials - Property of IBM
;                (c) Copyright IBM Corp. 1998-1999
;
; Author:        Andy Raybould
;
;                U.S. Government Users Restricted Rights - use,
;                duplication or disclosure restricted by GSA ADP
;                Schedule Contract with IBM Corp.
;
;
;--------------------------------------------------------------------

.MODEL TINY

CSEG    SEGMENT 'CODE'
        ASSUME  CS:CSEG
        org     0h

include found.inc
include autoverv.dcl

Start:

        org     8000h

   ; set ds = cs
        mov     bx,cs
        mov     ds,bx

   ; set up the parameters
        mov     bx, offset Instructions
        mov     cx, offset InstrEnd

   ; call the autoverv routine in the PAM/ERS foundation.
        int     0EBh
        jc      Fail

OK:
        mov     ax, 0FD00h
        int     21h     

Fail:
        mov     ax, 0FDFFh
        int     21h     

;------------------------------------------------------------------------------

Instructions:
   !copy the ERS entries here!
InstrEnd:

;------------------------------------------------------------------------------
         ; the two lines below must be inserted by the REPLICATION component
         ; these are used to index the repair in multi-repair-source files.

         DB     "!!!VIDINFO"
         DW     0<VirusID>h

CSEG    ENDS

end Start
end
[ERSCODEEND]

[OpcodeList]
   !copy the opcodes here!
[OpcodeListEnd]

[PAM_FLAGS]
CALL128, REPSCAS, REPMOVS, REPSTOS, RELOC, RUN, INVALID_INDEX_WRITE, DONE

[STRINGSTART]
   !copy the signatures here!
[STRINGEND]

[DOSDEFEND]
