RCL32   MACRO   a1:req,a2:req,a3
        LOCAL   shifted
        LOCAL   shiftAgain
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
                                IF IS_ONE(a3)
                                        ;;
                                        ;; 3rd argument is 1
                                        ;;
                                        rcl     a2,1
                                        rcl     a1,1
                                ELSEIF IS_CL(a3)
                                        ;;
                                        ;; 3rd argument is CL register
                                        ;;
                                        push    CX
                                        xor     CH,CH
                                        jcxz    shifted
                                        shiftAgain:
                                        rcl     a2,1
                                        rcl     a1,1
                                        loop    shiftAgain
                                        shifted:
                                        pop     CX
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
                IF IS_ONE(a2)
                        ;;
                        ;; 2nd argument is 1
                        ;;
                        rcl     WORD PTR a1[ 2 ],1
                        rcl     WORD PTR a1[ 0 ],1
                ELSEIF IS_CL(a2)
                        ;;
                        ;; 2rd argument is CL register
                        ;;
                        push    CX
                        xor     CH,CH
                        jcxz    shifted
                        shiftAgain:
                        rcl     WORD PTR a1[ 2 ],1
                        rcl     WORD PTR a1[ 0 ],1
                        loop    shiftAgain
                        shifted:
                        pop     CX
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

RCR32   MACRO   a1:req,a2:req,a3
        LOCAL   shifted
        LOCAL   shiftAgain
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
                                IF IS_ONE(a3)
                                        ;;
                                        ;; 3rd argument is 1
                                        ;;
                                        rcr     a1,1
                                        rcr     a2,1
                                ELSEIF IS_CL(a3)
                                        ;;
                                        ;; 3rd argument is CL register
                                        ;;
                                        push    CX
                                        xor     CH,CH
                                        jcxz    shifted
                                        shiftAgain:
                                        rcr     a1,1
                                        rcr     a2,1
                                        loop    shiftAgain
                                        shifted:
                                        pop     CX
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
                IF IS_ONE(a2)
                        ;;
                        ;; 2nd argument is 1
                        ;;
                        rcr     WORD PTR a1[ 0 ],1
                        rcr     WORD PTR a1[ 2 ],1
                ELSEIF IS_CL(a2)
                        ;;
                        ;; 2rd argument is CL register
                        ;;
                        push    CX
                        xor     CH,CH
                        jcxz    shifted
                        shiftAgain:
                        rcr     WORD PTR a1[ 0 ],1
                        rcr     WORD PTR a1[ 2 ],1
                        loop    shiftAgain
                        shifted:
                        pop     CX
                ELSE
                        .ERR <a2 is not a valid source>
                ENDIF
        ELSE
                .ERR <a1 is not a valid argument>
        ENDIF
        ENDM

