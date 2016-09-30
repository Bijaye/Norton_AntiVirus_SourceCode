DIV32   MACRO   a1,a2,a3,a4
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
                                        .ERR <Can't divide R4:R3:DX:AX by R2:R1>
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        IF a3 EQ 0
                                                INT     0
                                        ELSE
                                        xor32   DI,SI,DI,SI
                                        mov     CX,64
                                        .REPEAT
                                                shl32   DX,AX,1
                                                rcl32   a1,a2,1
                                                rcl32   DI,SI,1
                                                .IF ((CARRY?) || (DI > HIGHWORD a3) || ((DI == HIGHWORD a3) && (SI >= LOWWORD a3)))
                                                        sub32   DI,SI,a3
                                                        inc     AX
                                                .ENDIF
                                        .UNTILCXZ
                                        .IF ((a1 != 0) || (a2 != 0))
                                                INT     0
                                        .ENDIF
                                        MOV     a2,SI
                                        MOV     a1,DI
                                        ENDIF
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        .IF (((WORD PTR a3[ 0 ]) == 0) && ((WORD PTR a3[ 2 ]) == 0))
                                                int     0
                                        .ENDIF
                                        xor32   DI,SI,DI,SI
                                        mov     CX,64
                                        .REPEAT
                                                shl32   DX,AX,1
                                                rcl32   a1,a2,1
                                                rcl32   DI,SI,1
                                                .IF ((CARRY?) || (DI > WORD PTR a3[ 2 ]) || ((DI == WORD PTR a3[ 2 ]) && (SI >= WORD PTR a3[ 0 ])))
                                                        sub32   DI,SI,a3
                                                        inc     AX
                                                .ENDIF
                                        .UNTILCXZ
                                        .IF ((a1 != 0) || (a2 != 0))
                                                INT     0
                                        .ENDIF
                                        MOV     a2,SI
                                        MOV     a1,DI
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
                                        .IF ((a3 == 0) && (a2 == 0))
                                                int     0
                                        .ENDIF
                                        xor32   DI,SI,DI,SI
                                        mov     CX,64
                                        .REPEAT
                                                shl32   DX,AX,1
                                                rcl32   a1,1
                                                rcl32   DI,SI,1
                                                .IF ((CARRY?) || (DI > a2) || ((DI == a2) && (SI >= a3)))
                                                        sub32   DI,SI,a2,a3
                                                        inc     AX
                                                .ENDIF
                                        .UNTILCXZ
                                        .IF (((WORD PTR a1[ 2 ]) != 0) || ((WORD PTR a1[ 0 ]) != 0))
                                                INT     0
                                        .ENDIF
                                        MOV     WORD PTR a1[ 0 ],SI
                                        MOV     WORD PTR a1[ 2 ],DI
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        IF a2 EQ 0
                                INT 0
                        ELSE
                        xor32   DI,SI,DI,SI
                        mov     CX,64
                        .REPEAT
                                shl32   DX,AX,1
                                rcl32   a1,1
                                rcl32   DI,SI,1
                                .IF ((CARRY?) || (DI > HIGHWORD a2) || ((DI == HIGHWORD a2) && (SI >= LOWWORD a2)))
                                        sub32   DI,SI,a2
                                        inc     AX
                                .ENDIF
                        .UNTILCXZ
                        .IF (((WORD PTR a1[ 2 ]) != 0) || ((WORD PTR a1[ 0 ]) != 0))
                                INT     0
                        .ENDIF
                        MOV     WORD PTR a1[ 0 ],SI
                        MOV     WORD PTR a1[ 2 ],DI
                        ENDIF
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

IDIV32  MACRO   a1,a2,a3,a4
        LOCAL   divisor
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
                                        .ERR <Can't divide R4:R3:DX:AX by R2:R1>
                                ELSEIF IS_CONSTANT(a3)
                                        ;;
                                        ;; 3rd argument is a constant
                                        ;;
                                        ;
                                        ; if high bit of dividend and divisor are not same, we have to change the sign of the result...
                                        ;
                                        push    AX
                                        mov     AX,a1
                                        xor     AX,HIGHWORD a3
                                        pop     AX
                                        pushf
                                        ;
                                        ; make sure dividend is positive
                                        ;
                                        test    a1,8000h
                                        .IF (SIGN?)
                                                not     a1
                                                not     a2
                                                not     DX
                                                neg     AX
                                                cmc
                                                adc     DX,0
                                                adc32   a1,a2,0
                                        .ENDIF
                                        ;
                                        ; make sure divisor is positive
                                        ;
                                        IF a3 LT 0
                                                divisor = -a3
                                        ELSEIF a3 EQ 0
                                                int     0
                                        ELSE
                                                divisor = a3
                                        ENDIF
                                        ;
                                        ; clear remainder
                                        ;
                                        xor32   DI,SI,DI,SI
                                        ;
                                        ; set bit count to 64 bits
                                        ;
                                        mov     CX,64
                                        .REPEAT
                                                ;
                                                ; shift dividend left 1 bit
                                                ;
                                                shl32   DX,AX,1
                                                rcl32   a1,a2,1
                                                ;
                                                ; shift remainder left 1 bit
                                                ;
                                                rcl32   DI,SI,1
                                                ;
                                                ; if overflowed or remainder >= divisor
                                                ;
                                                .IF ((CARRY?) || (DI > HIGHWORD divisor) || ((DI == HIGHWORD divisor) && (SI >= LOWWORD divisor)))
                                                        ;
                                                        ; subtract divisor from remainder
                                                        ;
                                                        sub32   DI,SI,divisor
                                                        ;
                                                        ; add 1 to quotient
                                                        ;
                                                        inc     AX
                                                .ENDIF
                                        .UNTILCXZ
                                        ;
                                        ; check for overflow in the quotient
                                        ;
                                        .IF ((a1 != 0) || (a2 != 0))
                                                int     0
                                        .ENDIF
                                        ;
                                        ; save the remainder in the high dword of the former dividend
                                        ;
                                        mov     a2,SI
                                        mov     a1,DI
                                        ;
                                        ; round the quotient up 1
                                        ;
                                        shl32   DI,SI,1
                                        .IF ((CARRY?) || ((DI > HIGHWORD divisor) || ((DI == HIGHWORD divisor) && (SI >= LOWWORD divisor))))
                                                add32   DX,AX,1
                                                .IF (CARRY?)
                                                        int     0
                                                .ENDIF
                                        .ENDIF
                                        popf
                                        .IF (CARRY?)
                                                not     DX
                                                neg     AX
                                                cmc
                                                adc     DX,0
                                        .ENDIF
                                ELSEIF IS_MEM32(a3)
                                        ;;
                                        ;; 3rd argument is a dword ptr
                                        ;;
                                        ;
                                        ; if high bit of dividend and divisor are not same, we have to change the sign of the result...
                                        ;
                                        push    AX
                                        mov     AX,a1
                                        xor     AX,WORD PTR a3[ 2 ]
                                        pop     AX
                                        pushf
                                        ;
                                        ; make sure dividend is positive
                                        ;
                                        test    a1,8000h
                                        .IF (SIGN?)
                                                not     a1
                                                not     a2
                                                not     DX
                                                neg     AX
                                                cmc
                                                adc     DX,0
                                                adc32   a1,a2,0
                                        .ENDIF
                                        ;
                                        ; make sure divisor is positive
                                        ;
                                        test    WORD PTR a3[ 2 ],08000H
                                        .IF (SIGN?)
                                                neg     WORD PTR a3[ 2 ]
                                                neg     WORD PTR a3[ 0 ]
                                                sbb     WORD PTR a3[ 2 ],0
                                        .ENDIF
                                        ;
                                        ; make sure divisor is not zero
                                        ;
                                        .IF (((WORD PTR a3[ 0 ]) == 0) && ((WORD PTR a3[ 2 ]) == 0))
                                                int     0
                                        .ENDIF
                                        ;
                                        ; clear remainder
                                        ;
                                        xor32   DI,SI,DI,SI
                                        ;
                                        ; set bit count to 64 bits
                                        ;
                                        mov     CX,64
                                        .REPEAT
                                                ;
                                                ; shift dividend left 1 bit
                                                ;
                                                shl32   DX,AX,1
                                                rcl32   a1,a2,1
                                                ;
                                                ; shift remainder left 1 bit
                                                ;
                                                rcl32   DI,SI,1
                                                ;
                                                ; if overflowed or remainder >= divisor
                                                ;
                                                .IF ((CARRY?) || (DI > (WORD PTR a3[ 2 ])) || ((DI == (WORD PTR a3[ 2 ])) && (SI >= (WORD PTR a3[ 0 ]))))
                                                        ;
                                                        ; subtract divisor from remainder
                                                        ;
                                                        sub32   DI,SI,a3
                                                        ;
                                                        ; add 1 to quotient
                                                        ;
                                                        inc     AX
                                                .ENDIF
                                        .UNTILCXZ
                                        ;
                                        ; check for overflow in the quotient
                                        ;
                                        .IF ((a1 != 0) || (a2 != 0))
                                                int     0
                                        .ENDIF
                                        ;
                                        ; save the remainder in the high dword of the former dividend
                                        ;
                                        mov     a2,SI
                                        mov     a1,DI
                                        ;
                                        ; round the quotient up 1
                                        ;
                                        shl32   DI,SI,1
                                        .IF ((CARRY?) || ((DI > (WORD PTR a3[ 2 ])) || ((DI == (WORD PTR a3[ 2 ])) && (SI >= (WORD PTR a3[ 0 ])))))
                                                add32   DX,AX,1
                                                .IF (CARRY?)
                                                        int     0
                                                .ENDIF
                                        .ENDIF
                                        popf
                                        .IF (CARRY?)
                                                not     DX
                                                neg     AX
                                                cmc
                                                adc     DX,0
                                        .ENDIF
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
                                        ;
                                        ; if high bit of dividend and divisor are not same, we have to change the sign of the result...
                                        ;
                                        push    AX
                                        mov     AX,WORD PTR a1[ 2 ]
                                        xor     AX,a2
                                        pop     AX
                                        pushf
                                        ;
                                        ; make sure dividend is positive
                                        ;
                                        test    WORD PTR a1[ 2 ],8000h
                                        .IF (SIGN?)
                                                not     WORD PTR a1[ 2 ]
                                                not     WORD PTR a1[ 0 ]
                                                not     DX
                                                neg     AX
                                                cmc
                                                adc     DX,0
                                                adc32   a1,0
                                        .ENDIF
                                        ;
                                        ; make sure divisor is positive
                                        ;
                                        or      a2,a2
                                        .IF (SIGN?)
                                                neg     a2
                                                neg     a3
                                                sbb     a2,0
                                        .ENDIF
                                        ;
                                        ; make sure divisor is not zero
                                        ;
                                        .IF ((a3 == 0) && (a2 == 0))
                                                int     0
                                        .ENDIF
                                        ;
                                        ; clear remainder
                                        ;
                                        xor32   DI,SI,DI,SI
                                        ;
                                        ; set bit count to 64 bits
                                        ;
                                        mov     CX,64
                                        .REPEAT
                                                ;
                                                ; shift dividend left 1 bit
                                                ;
                                                shl32   DX,AX,1
                                                rcl32   a1,1
                                                ;
                                                ; shift remainder left 1 bit
                                                ;
                                                rcl32   DI,SI,1
                                                ;
                                                ; if overflowed or remainder >= divisor
                                                ;
                                                .IF ((CARRY?) || (DI > a2) || ((DI == a2) && (SI >= a3)))
                                                        ;
                                                        ; subtract divisor from remainder
                                                        ;
                                                        sub32   DI,SI,a2,a3
                                                        ;
                                                        ; add 1 to quotient
                                                        ;
                                                        inc     AX
                                                .ENDIF
                                        .UNTILCXZ
                                        ;
                                        ; check for overflow in the quotient
                                        ;
                                        .IF (((WORD PTR a1[ 2 ]) != 0) || ((WORD PTR a1[ 0 ]) != 0))
                                                int     0
                                        .ENDIF
                                        ;
                                        ; save the remainder in the high dword of the former dividend
                                        ;
                                        mov     WORD PTR a1[ 0 ],SI
                                        mov     WORD PTR a1[ 2 ],DI
                                        ;
                                        ; round the quotient up 1
                                        ;
                                        shl32   DI,SI,1
                                        .IF ((CARRY?) || ((DI > a2) || ((DI == a2) && (SI >= a3))))
                                                add32   DX,AX,1
                                                .IF (CARRY?)
                                                        int     0
                                                .ENDIF
                                        .ENDIF
                                        popf
                                        .IF (CARRY?)
                                                not     DX
                                                neg     AX
                                                cmc
                                                adc     DX,0
                                        .ENDIF
                                ELSE
                                        .ERR <a3 must be a 16-bit register>
                                ENDIF
                        ENDIF
                ELSEIF IS_CONSTANT(a2)
                        ;
                        ; if high bit of dividend and divisor are not same, we have to change the sign of the result...
                        ;
                        push    AX
                        mov     AX,WORD PTR a1[ 2 ]
                        xor     AX,HIGHWORD a2
                        pop     AX
                        pushf
                        ;
                        ; make sure dividend is positive
                        ;
                        test    WORD PTR a1[ 2 ],8000h
                        .IF (SIGN?)
                                not     WORD PTR a1[ 2 ]
                                not     WORD PTR a1[ 0 ]
                                not     DX
                                neg     AX
                                cmc
                                adc     DX,0
                                adc32   a1,0
                        .ENDIF
                        ;
                        ; make sure divisor is positive
                        ;
                        IF a2 LT 0
                                divisor = -a2
                        ELSEIF a2 EQ 0
                                int     0
                        ELSE
                                divisor = a2
                        ENDIF
                        ;
                        ; clear remainder
                        ;
                        xor32   DI,SI,DI,SI
                        ;
                        ; set bit count to 64 bits
                        ;
                        mov     CX,64
                        .REPEAT
                                ;
                                ; shift dividend left 1 bit
                                ;
                                shl32   DX,AX,1
                                rcl32   a1,1
                                ;
                                ; shift remainder left 1 bit
                                ;
                                rcl32   DI,SI,1
                                ;
                                ; if overflowed or remainder >= divisor
                                ;
                                .IF ((CARRY?) || (DI > HIGHWORD divisor) || ((DI == HIGHWORD divisor) && (SI >= LOWWORD divisor)))
                                        ;
                                        ; subtract divisor from remainder
                                        ;
                                        sub32   DI,SI,divisor
                                        ;
                                        ; add 1 to quotient
                                        ;
                                        inc     AX
                                .ENDIF
                        .UNTILCXZ
                        ;
                        ; check for overflow in the quotient
                        ;
                        .IF (((WORD PTR a1[ 2 ]) != 0) || ((WORD PTR a1[ 0 ]) != 0))
                                int     0
                        .ENDIF
                        ;
                        ; save the remainder in the high dword of the former dividend
                        ;
                        mov     WORD PTR a1[ 0 ],SI
                        mov     WORD PTR a1[ 2 ],DI
                        ;
                        ; round the quotient up 1
                        ;
                        shl32   DI,SI,1
                        .IF ((CARRY?) || ((DI > HIGHWORD divisor) || ((DI == HIGHWORD divisor) && (SI >= LOWWORD divisor))))
                                add32   DX,AX,1
                                .IF (CARRY?)
                                        int     0
                                .ENDIF
                        .ENDIF
                        popf
                        .IF (CARRY?)
                                not     DX
                                neg     AX
                                cmc
                                adc     DX,0
                        .ENDIF
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM
