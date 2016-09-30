;
; $Header:   S:\certlibn\vcs\avapi.inv   1.7   06 Sep 1994 18:00:48   CAREY  $
;
; This file prototypes the callback functions now used throughout CERTLIB(n).
;
;-------------------------------------------------------------------------
; $Log:   S:\certlibn\vcs\avapi.inv  $
;  
;     Rev 1.7   06 Sep 1994 18:00:48   CAREY
;  added FileLock, FileUnlock and Progress to callback structure for CERTLIB
;  
;     Rev 1.6   02 Sep 1994 17:31:26   RSTANEV
;  Got rid of Carey's "dirty language" f-words for VxD platform :-)
;
;     Rev 1.5   25 Aug 1994 14:38:50   CAREY
;
;     Rev 1.4   25 Aug 1994 11:56:12   CAREY
;  Changed prototypes of memalloc callbacks to take DWORD for size.
;-------------------------------------------------------------------------

SYM_MAX_PATH            equ             260


IF OS_32

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; callback prototypes for file functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


VLFileOpenProto         TYPEDEF PROTO NEAR32,:LPSTR,:UINT
VLFileOpen              TYPEDEF PTR VLFileOpenProto

VLFileCloseProto        TYPEDEF PROTO NEAR32,:UINT
VLFileClose             TYPEDEF PTR VLFileCloseProto

VLFileReadProto         TYPEDEF PROTO NEAR32,:UINT,:LPSTR,:UINT
VLFileRead              TYPEDEF PTR VLFileReadProto

VLFileWriteProto        TYPEDEF PROTO NEAR32,:UINT,:LPSTR,:UINT
VLFileWrite             TYPEDEF PTR VLFileWriteProto

VLFileSeekProto         TYPEDEF PROTO NEAR32,:UINT,:DWORD,:UINT
VLFileSeek              TYPEDEF PTR VLFileSeekProto

VLFileSizeProto         TYPEDEF PROTO NEAR32,:UINT
VLFileSize              TYPEDEF PTR VLFileSizeProto

VLFileGetDateTimeProto  TYPEDEF PROTO NEAR32,:UINT,:UINT, :DWORD,:DWORD
VLFileGetDateTime       TYPEDEF PTR VLFileGetDateTimeProto

VLFileSetDateTimeProto  TYPEDEF PROTO NEAR32,:UINT,:UINT, :WORD,:WORD
VLFileSetDateTime       TYPEDEF PTR VLFileSetDateTimeProto

VLFileDeleteProto       TYPEDEF PROTO NEAR32,:LPSTR
VLFileDelete            TYPEDEF PTR VLFileDeleteProto

VLFileGetAttrProto      TYPEDEF PROTO NEAR32,:LPSTR,:DWORD
VLFileGetAttr           TYPEDEF PTR VLFileGetAttrProto

VLFileSetAttrProto      TYPEDEF PROTO NEAR32,:LPSTR,:UINT
VLFileSetAttr           TYPEDEF PTR VLFileSetAttrProto

VLFileCreateProto       TYPEDEF PROTO NEAR32,:LPSTR,:UINT
VLFileCreate            TYPEDEF PTR VLFileCreateProto

VLFileLockProto         TYPEDEF PROTO NEAR32,:UINT,:DWORD,:DWORD
VLFileLock              TYPEDEF PTR VLFileLockProto

VLFileUnlockProto       TYPEDEF PROTO NEAR32,:UINT,:DWORD,:DWORD
VLFileUnlock            TYPEDEF PTR VLFileUnlockProto

VLProgressProto         TYPEDEF PROTO NEAR32,:WORD,:DWORD
VLProgress              TYPEDEF PTR VLProgressProto

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; callback prototypes for disk based functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

VLSDiskReadPhysicalProto        TYPEDEF PROTO NEAR32,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
VLSDiskReadPhysical             TYPEDEF PTR VLSDiskReadPhysicalProto

VLSDiskWritePhysicalProto       TYPEDEF PROTO NEAR32,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
VLSDiskWritePhysical            TYPEDEF PTR VLSDiskWritePhysicalProto

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; callback prototypes for memalloc functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

VLTempMemoryAllocProto          TYPEDEF PROTO FAR,:DWORD
VLTempMemoryAlloc               TYPEDEF PTR VLTempMemoryAllocProto

VLTempMemoryFreeProto           TYPEDEF PROTO FAR,:LPSTR
VLTempMemoryFree                TYPEDEF PTR VLTempMemoryFreeProto


VLPermMemoryAllocProto          TYPEDEF PROTO FAR,:DWORD
VLPermMemoryAlloc               TYPEDEF PTR VLPermMemoryAllocProto

VLPermMemoryFreeProto           TYPEDEF PROTO FAR,:LPSTR
VLPermMemoryFree                TYPEDEF PTR VLPermMemoryFreeProto

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ELSE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; callback prototypes for file functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


VLFileOpenProto         TYPEDEF PROTO FAR,:LPSTR,:UINT
VLFileOpen              TYPEDEF PTR VLFileOpenProto

VLFileCloseProto        TYPEDEF PROTO FAR,:UINT
VLFileClose             TYPEDEF PTR VLFileCloseProto

VLFileReadProto         TYPEDEF PROTO FAR,:UINT,:LPSTR,:UINT
VLFileRead              TYPEDEF PTR VLFileReadProto

VLFileWriteProto        TYPEDEF PROTO FAR,:UINT,:LPSTR,:UINT
VLFileWrite             TYPEDEF PTR VLFileWriteProto

VLFileSeekProto         TYPEDEF PROTO FAR,:UINT,:DWORD,:UINT
VLFileSeek              TYPEDEF PTR VLFileSeekProto

VLFileSizeProto         TYPEDEF PROTO FAR,:UINT
VLFileSize              TYPEDEF PTR VLFileSizeProto

VLFileGetDateTimeProto  TYPEDEF PROTO FAR,:UINT,:UINT,:DWORD,:DWORD
VLFileGetDateTime       TYPEDEF PTR VLFileGetDateTimeProto

VLFileSetDateTimeProto  TYPEDEF PROTO FAR,:UINT,:UINT,:WORD,:WORD
VLFileSetDateTime       TYPEDEF PTR VLFileSetDateTimeProto

VLFileDeleteProto       TYPEDEF PROTO FAR,:LPSTR
VLFileDelete            TYPEDEF PTR VLFileDeleteProto

VLFileGetAttrProto      TYPEDEF PROTO FAR,:LPSTR,:DWORD
VLFileGetAttr           TYPEDEF PTR VLFileGetAttrProto

VLFileSetAttrProto      TYPEDEF PROTO FAR,:LPSTR,:UINT
VLFileSetAttr           TYPEDEF PTR VLFileSetAttrProto

VLFileCreateProto       TYPEDEF PROTO FAR,:LPSTR,:UINT
VLFileCreate            TYPEDEF PTR VLFileCreateProto

VLFileLockProto         TYPEDEF PROTO FAR,:UINT,:DWORD,:DWORD
VLFileLock              TYPEDEF PTR VLFileLockProto

VLFileUnlockProto       TYPEDEF PROTO FAR,:UINT,:DWORD,:DWORD
VLFileUnlock            TYPEDEF PTR VLFileUnlockProto

VLProgressProto         TYPEDEF PROTO FAR,:WORD,:DWORD
VLProgress              TYPEDEF PTR VLProgressProto


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; callback prototypes for disk based functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

VLSDiskReadPhysicalProto        TYPEDEF PROTO FAR,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
VLSDiskReadPhysical             TYPEDEF PTR VLSDiskReadPhysicalProto

VLSDiskWritePhysicalProto       TYPEDEF PROTO FAR,:LPSTR,:BYTE,:WORD,:BYTE,:BYTE,:BYTE
VLSDiskWritePhysical            TYPEDEF PTR VLSDiskWritePhysicalProto

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; callback prototypes for memalloc functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

VLTempMemoryAllocProto          TYPEDEF PROTO FAR,:DWORD
VLTempMemoryAlloc               TYPEDEF PTR VLTempMemoryAllocProto

VLTempMemoryFreeProto           TYPEDEF PROTO FAR,:LPSTR
VLTempMemoryFree                TYPEDEF PTR VLTempMemoryFreeProto


VLPermMemoryAllocProto          TYPEDEF PROTO FAR,:DWORD
VLPermMemoryAlloc               TYPEDEF PTR VLPermMemoryAllocProto

VLPermMemoryFreeProto           TYPEDEF PROTO FAR,:LPSTR
VLPermMemoryFree                TYPEDEF PTR VLPermMemoryFreeProto

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ENDIF


CALLBACKREV1            STRUCT

        FileOpen                VLFileOpen              0
        FileClose               VLFileClose             0
        FileRead                VLFileRead              0
        FileWrite               VLFileWrite             0
        FileSeek                VLFileSeek              0
        FileSize                VLFileSize              0
        FileGetDateTime         VLFileGetDateTime       0
        FileSetDateTime         VLFileSetDateTime       0
        FileDelete              VLFileDelete            0
        FileGetAttr             VLFileGetAttr           0
        FileSetAttr             VLFileSetAttr           0
        FileCreate              VLFileCreate            0

        FileLock                VLFileLock              0
        FileUnlock              VLFileUnlock            0
        Progress                VLProgress              0

        SDiskReadPhysical       VLSDiskReadPhysical     0
        SDiskWritePhysical      VLSDiskWritePhysical    0

        TempMemoryAlloc         VLTempMemoryAlloc       0
        TempMemoryFree          VLTempMemoryFree        0

        PermMemoryAlloc         VLPermMemoryAlloc       0
        PermMemoryFree          VLPermMemoryFree        0

CALLBACKREV1    ENDS


