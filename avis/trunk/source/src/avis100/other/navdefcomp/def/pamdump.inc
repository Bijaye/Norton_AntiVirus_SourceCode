nFailc          MACRO n
                local n
                mov DumpCode, n
                jc  Dump
                ENDM

nFailne         MACRO n
                local n
                mov DumpCode, n
                jne  Dump
                ENDM

nFailz          MACRO n
                local n
                mov DumpCode, n
                jz  Dump
                ENDM

nFailnz         MACRO n
                local n
                mov DumpCode, n
                jnz  Dump
                ENDM

nFailNow        MACRO n
                local n
                mov DumpCode, n
                jmp  Dump
                ENDM



Dump:           xor  cx, cx
                xor  dx, dx
                SeekEnd
                mov  dx, offset DumpHdr
                mov  cx, offset DumpEnd - offset DumpHdr
                FileWrite
                jmp  Fail



DumpHdr         DB  'DUMP>>>>'
DumpCode                DW  0
                        DW  0
                DB  '<<<<'
DumpEnd:
