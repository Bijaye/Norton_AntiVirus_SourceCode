;
;   Equates used for logging.  Each equate corresponds directly to a message in
;   the special CMB log file.  Therefore, item #0 corresponds to the 0th entry
;   in the log CMB file.  
;
;
;   In this version there are 79 possible log entries.  These will index two
;   separate log files.  One is for ONE-LINE network messages, and the other
;   is for 5 line file log entries.  The log entries are encoded as follows:
;
;   KNOWN   = Known virus being dealt with
;   UNKNOWN = Unknown virus
;   CHANGE  = Some CRC detected file change (virus unlikely)
;   STOP    = NAV3.0 has not let the current process continue
;   HALT    = The computer is halted
;   EXCLUDE = The file will be excluded from some category of detection
;   REINNOC = Reinnoculation of a file/MBR/Boot
;   CRIT    = This is a message ONLY written on CRITICAL (int 24) errors
;   SUCC    = This is a successful action
;
;   Here's a sample of some code in which the logging routines can be easily 
;   integrated:
;
;
;display_menu:
;
;    INVOKE GetUserOptions, fStruct, etc, etc
;
;    cmp     ax, butREPAIR
;    jnz     try_delete
;
;    call    RepairTheFile, fStruct
;
;    cmp     ax, 0                   ; success!
;    jnz     repair_fail
;
;    INVOKE  LOG3_GENERAL, KNOWN_FILE_REPAIRED_SUCC, KNOWN_CATEGORY, fStruct
;
;    jmp     all_done
;
;repair_fail:
;
;    cmp     ax, 0ffffh              ; is this a critical error?
;    jnz     display_menu            ; no.  Just unable to repair.  Go to menu
;
;    INVOKE  LOG3_GENERAL, KNOWN_FILE_REPAIRED_CRIT, KNOWN_CATEGORY, fStruct
;
;    jmp     all_done
;
;try_delete:
;
;    ...





; here come bit id's

KNOWN_CATEGORY          equ     1
UNKNOWN_CATEGORY        equ     2
CHANGE_CATEGORY         equ     4
UI_CATEGORY             equ     8               
TSR_CATEGORY            equ    16


; here come id #'s

KNOWN_FILE_DELETE_SUCC      equ     1
KNOWN_FILE_DELETE_CRIT      equ     2
KNOWN_FILE_REPAIR_SUCC      equ     3
KNOWN_FILE_REPAIR_CRIT      equ     4
KNOWN_FILE_STOP             equ     5
KNOWN_FILE_HALT             equ     6
KNOWN_FILE_EXCLUDE_SUCC     equ     7
KNOWN_FILE_EXCLUDE_CRIT     equ     8
KNOWN_FILE_NOACT            equ     9

UNKNOWN_FILE_DELETE_SUCC    equ     10
UNKNOWN_FILE_DELETE_CRIT    equ     11
UNKNOWN_FILE_REPAIR_SUCC    equ     12
UNKNOWN_FILE_REPAIR_CRIT    equ     13
UNKNOWN_FILE_NOACT          equ     14
UNKNOWN_FILE_REINNOC_SUCC   equ     15
UNKNOWN_FILE_REINNOC_CRIT   equ     16
UNKNOWN_FILE_STOP           equ     17
UNKNOWN_FILE_HALT           equ     18
UNKNOWN_FILE_EXCLUDE_SUCC   equ     19
UNKNOWN_FILE_EXCLUDE_CRIT   equ     20

CHANGE_FILE_DELETE_SUCC     equ     21
CHANGE_FILE_DELETE_CRIT     equ     22
CHANGE_FILE_REPAIR_SUCC     equ     23
CHANGE_FILE_REPAIR_CRIT     equ     24
CHANGE_FILE_NOACT           equ     25
CHANGE_FILE_REINNOC_SUCC    equ     26
CHANGE_FILE_REINNOC_CRIT    equ     27
CHANGE_FILE_STOP            equ     28
CHANGE_FILE_HALT            equ     29
CHANGE_FILE_EXCLUDE_SUCC    equ     30
CHANGE_FILE_EXCLUDE_CRIT    equ     31

CHANGE_ATTR_STOP            equ     32
CHANGE_ATTR_CONTINUE        equ     33
CHANGE_ATTR_EXCLUDE_SUCC    equ     34
CHANGE_ATTR_EXCLUDE_CRIT    equ     35

WRITE_TO_EXEC_STOP          equ     36
WRITE_TO_EXEC_CONTINUE      equ     37
WRITE_TO_EXEC_EXCLUDE_SUCC  equ     38
WRITE_TO_EXEC_EXCLUDE_CRIT  equ     39

WRITE_TO_BOOT_STOP          equ     40
WRITE_TO_BOOT_CONTINUE      equ     41
WRITE_TO_BOOT_EXCLUDE_SUCC  equ     42
WRITE_TO_BOOT_EXCLUDE_CRIT  equ     43                                     
WRITE_MBR_STOP              equ     44
WRITE_MBR_CONTINUE          equ     45
WRITE_MBR_EXCLUDE_SUCC      equ     46
WRITE_MBR_EXCLUDE_CRIT      equ     47

KNOWN_BOOT_REPAIR_SUCC      equ     48
KNOWN_BOOT_REPAIR_CRIT      equ     49
KNOWN_BOOT_NOACT            equ     50

KNOWN_MBR_REPAIR_SUCC       equ     51
KNOWN_MBR_REPAIR_CRIT       equ     52
KNOWN_MBR_NOACT             equ     53

CHANGE_BOOT_REPAIR_SUCC     equ     54
CHANGE_BOOT_REPAIR_CRIT     equ     55
CHANGE_BOOT_NOACT           equ     56

CHANGE_MBR_REPAIR_SUCC      equ     57
CHANGE_MBR_REPAIR_CRIT      equ     58
CHANGE_MBR_NOACT            equ     59

NEW_FILE_INNOC_SUCC         equ     60
NEW_FILE_INNOC_CRIT         equ     61
NEW_FILE_NOACT              equ     62

KNOWN_MEMORY_HALT           equ     63
KNOWN_MEMORY_CONTINUE       equ     64

CHANGE_TSR_REPAIR_SUCC      equ     65
CHANGE_TSR_REPAIR_CRIT      equ     66

CHANGE_TSR_CONTINUE         equ     67
CHANGE_TSR_STOP             equ     68

UNKNOWN_TSR_REPAIR_SUCC     equ     69
UNKNOWN_TSR_REPAIR_CRIT     equ     70
UNKNOWN_TSR_CONTINUE        equ     71
UNKNOWN_TSR_STOP            equ     72

KNOWN_TSR_REPAIR_SUCC       equ     73
KNOWN_TSR_REPAIR_CRIT       equ     74
KNOWN_TSR_HALT              equ     75

READ_FAIL                   equ     76


; OFFSET to be added to LOW_LEVEL stuff is 90

LOW_LEVEL_STOP              equ     0
LOW_LEVEL_CONTINUE          equ     1
LOW_LEVEL_EXCLUDE_SUCC      equ     2
LOW_LEVEL_EXCLUDE_CRIT      equ     3

; OFFSET to be added to FIXED_WRITE stuff is 100

FIXED_WRITE_STOP            equ     0
FIXED_WRITE_CONTINUE        equ     1
FIXED_WRITE_EXCLUDE_SUCC    equ     2
FIXED_WRITE_EXCLUDE_CRIT    equ     3

; OFFSET to be added to FLOPPY_WRITE stuff is 110 

FLOPPY_WRITE_STOP           equ     0
FLOPPY_WRITE_CONTINUE       equ     1
FLOPPY_WRITE_EXCLUDE_SUCC   equ     2
FLOPPY_WRITE_EXCLUDE_CRIT   equ     3

; OFFSET to be added to FLOPPY_ACCESS stuff is 120 

FLOPPY_ACCESS_STOP          equ     0
FLOPPY_ACCESS_CONTINUE      equ     1
FLOPPY_ACCESS_EXCLUDE_SUCC  equ     2
FLOPPY_ACCESS_EXCLUDE_CRIT  equ     3
