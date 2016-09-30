_mul32  MACRO   product,x1,x2,y1,y2
        push    AX
        push    DX
        ;
        ; clear WORD PTR product[ 4 ]
        ; clear WORD PTR product[ 6 ]
        ;
        xor     AX,AX
        mov     WORD PTR product[ 4 ],AX
        mov     WORD PTR product[ 6 ],AX
        ;
        ; multiply x2 by y2
        ;
        mov     AX,x2
        mul     y2
        ;
        ; store AX in WORD PTR product[ 0 ]
        ; store DX in WORD PTR product[ 2 ]
        ;
        mov     WORD PTR product[ 0 ],AX
        mov     WORD PTR product[ 2 ],DX
        ;
        ; multiply x2 by y1
        ;
        mov     AX,x2
        mul     y1
        ;
        ; add AX to WORD PTR product[ 2 ]
        ; add DX with carry to WORD PTR product[ 4 ]
        ;
        add32   DWORD PTR product[ 2 ],DX,AX
        ;
        ; multiply x1 by y2
        ;
        mov     AX,x1
        mul     y2
        ;
        ; add AX to WORD PTR product[ 2 ]
        ; add DX with carry to WORD PTR product[ 4 ]
        ; add carry to WORD PTR product[ 6 ]
        ;
        add32   DWORD PTR product[ 2 ],DX,AX
        adc     WORD PTR product[ 6 ],0
        ;
        ; multiply x1 by y1
        ;
        mov     AX,x1
        mul     y1
        ;
        ; add AX to WORD PTR product[ 4 ]
        ; add DX with carry to WORD PTR product[ 6 ]
        ;
        add32   DWORD PTR product[ 4 ],DX,AX
        pop     DX
        pop     AX
        ENDM

_imul32 MACRO   product,x1,x2,y1,y2
        ;
        ; see if multiplicand signs match
        ;
        push    AX
        mov     AX,x1
        xor     AX,y1
        pop     AX
        .IF (SIGN?)
                ;
                ; signs do not match - product will be negative
                ;
                test    x1,08000H
                .IF (SIGN?)
                        ;
                        ; first multiplicand is negative
                        ;
                        neg     x1
                        neg     x2
                        sbb     x1,0
                .ELSE
                        ;
                        ; second multiplicand is negative
                        ;
                        neg     y1
                        neg     y2
                        sbb     y1,0
                .ENDIF
                ;
                ; perform the required multiplication
                ;
                _mul32  product,x1,x2,y1,y2
                ;
                ; make product negative
                ;
                not     WORD PTR product[ 6 ]
                not     WORD PTR product[ 4 ]
                not     WORD PTR product[ 2 ]
                neg     WORD PTR product[ 0 ]
                cmc
                adc     WORD PTR product[ 4 ],0
                adc32   DWORD PTR product,0
        .ELSE
                ;
                ; signs match - product will be positive
                ;
                test    x1,08000H
                .IF (SIGN?)
                        ;
                        ; both multiplicands are negative
                        ;
                        neg     x1
                        neg     x2
                        sbb     x1,0
                        neg     y1
                        neg     y2
                        sbb     y1,0
                .ENDIF
                _mul32  product,x1,x2,y1,y2
        .ENDIF
        ENDM

MUL32   MACRO   product:req,a1:req,a2:req,a3,a4
        IF IS_REG16(a1)
                ;;
                ;; first argument is a 16-bit register
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; second argument is a 16-bit register
                        ;;
                        IFB <a3>
                                .ERR <missing source argument>
                        ELSE
                                IF IS_REG16(a3)
                                        ;;
                                        ;; 3rd argument is a 16-bit register
                                        ;;
                                        IFB <a4>
                                                .ERR <missing source argument>
                                        ELSE
                                                IF IS_REG16(a4)
                                                        ;;
                                                        ;; 4th argument is a 16-bit register
                                                        ;;
                                                        _mul32  product,a1,a2,a3,a4
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        .ERR <32-bit multiply with a constant value is not supported>
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        _mul32  product,a1,a2,WORD PTR a3[ 2 ],WORD PTR a3[ 0 ]
                                ELSE
                                        .ERR <a3 is not a valid source>
                                ENDIF
                        ENDIF
                ELSE
                        .ERR <a2 must be a 16-bit register>
                ENDIF
        ELSEIF IS_MEM32(a1)
                ;;
                ;; first argument is a dword ptr
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; 2nd argument is a register
                        ;;
                        IFB <a3>
                                .ERR <2nd source register is missing>
                        ELSE
                                IF IS_REG16(a3)
                                        _mul32  product,WORD PTR a1[ 2 ],WORD PTR a1[ 0 ],a2,a3
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        .ERR <32-bit multiply with a constant value is not supported>
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

IMUL32  MACRO   product:req,a1:req,a2:req,a3,a4
        IF IS_REG16(a1)
                ;;
                ;; first argument is a 16-bit register
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; second argument is a 16-bit register
                        ;;
                        IFB <a3>
                                .ERR <missing source argument>
                        ELSE
                                IF IS_REG16(a3)
                                        ;;
                                        ;; 3rd argument is a 16-bit register
                                        ;;
                                        IFB <a4>
                                                .ERR <missing source argument>
                                        ELSE
                                                IF IS_REG16(a4)
                                                        ;;
                                                        ;; 4th argument is a 16-bit register
                                                        ;;
                                                        _imul32 product,a1,a2,a3,a4
                                                ELSE
                                                        .ERR <a4 must be a 16-bit register>
                                                ENDIF
                                        ENDIF
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        .ERR <32-bit multiply with a constant value is not supported>
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        _imul32 product,a1,a2,WORD PTR a3[ 2 ],WORD PTR a3[ 0 ]
                                ELSE
                                        .ERR <a3 is not a valid source>
                                ENDIF
                        ENDIF
                ELSE
                        .ERR <a2 must be a 16-bit register>
                ENDIF
        ELSEIF IS_MEM32(a1)
                ;;
                ;; first argument is a dword ptr
                ;;
                IF IS_REG16(a2)
                        ;;
                        ;; 2nd argument is a register
                        ;;
                        IFB <a3>
                                .ERR <2nd source register is missing>
                        ELSE
                                IF IS_REG16(a3)
                                        _imul32 product,WORD PTR a1[ 2 ],WORD PTR a1[ 0 ],a2,a3
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        .ERR <32-bit multiply with a constant value is not supported>
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM
