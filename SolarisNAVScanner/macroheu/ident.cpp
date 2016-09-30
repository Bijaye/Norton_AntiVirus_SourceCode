//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/ident.cpv   1.3   18 May 1998 11:14:58   DCHI  $
//
// Description:
//  Macro emulation environment miscellaneous state functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/ident.cpv  $
// 
//    Rev 1.3   18 May 1998 11:14:58   DCHI
// Fixed problem in StateAssign().
// 
//    Rev 1.2   15 Jul 1997 17:04:16   DCHI
// Changed dialog structure field sSize to wSize.
// 
//    Rev 1.1   09 Jul 1997 13:43:22   DCHI
// Added push of normal filename for FileSummaryInfo.Template reference.
// 
//    Rev 1.0   30 Jun 1997 16:15:36   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"

#include "wbutil.h"

//*************************************************************************
//
// void StateEXPR()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state is used for evaluating an expression.
//
//  If the current token is an operator, the state performs the
//  following conversion upon pushing the operator:
//
//      eTOKEN_CLOSE_PAREN      eOPERATOR_CLOSE_PAREN
//      eTOKEN_OPEN_PAREN       eOPERATOR_OPEN_PAREN
//      eTOKEN_NOT              eOPERATOR_NOT
//      eTOKEN_AND              eOPERATOR_AND
//      eTOKEN_OR               eOPERATOR_OR
//      eTOKEN_PLUS             eOPERATOR_ADD
//      eTOKEN_MINUS            eOPERATOR_SUB
//      eTOKEN_SLASH            eOPERATOR_DIV
//      eTOKEN_ASTERISK         eOPERATOR_MUL
//      eTOKEN_MOD              eOPERATOR_MOD
//      eTOKEN_EQ               eOPERATOR_EQ
//      eTOKEN_NE               eOPERATOR_NE
//      eTOKEN_LT               eOPERATOR_LT
//      eTOKEN_GT               eOPERATOR_GT
//      eTOKEN_LE               eOPERATOR_LE
//      eTOKEN_GE               eOPERATOR_GE
//
//  If the token is eTOKEN_CLOSE_PAREN, the state determines whether
//  all open parentheses have been matched by closing parentheses.
//  If not, then the state transitions out by pushing an eOPERATOR_END,
//  popping the state, and redoing the token.  The waiting state
//  will then have a value waiting on the evaluation stack.
//
//  An eTOKEN_WORD_VALUE or eTOKEN_QUOTED_STRING value causes a push
//  onto the evaluation operand stack.  Errors pushing are ignored.
//
//  An eTOKEN_IDENTIFIER causes a transition to the esEXPR_IDENTIFIER
//  state.  The identifier may be a variable or it may be a function
//  or macro call.
//
//  An eTOKEN_DIALOG causes a transition to the esEXPR_DIALOG state
//  to evaluate the result of a dialog call.
//
//  An eTOKEN_FUNCTION_VALUE causes a transition to the esEXPR_BUILTIN
//  state to evaluate a function call to a built-in function.
//
//  Any other token ends the expression, causing the push of an
//  eOPERATOR_END, a transition out of this state, and a redo of the
//  token.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateEXPR
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esEXPR);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_CLOSE_PAREN:
            if (((LPEXPR)ControlTop(lpstEnv))->wParenDepth-- == 0)
            {
                assert(((LPEXPR)ControlTop(lpstEnv))->ecType == ectEXPR);
                ControlDestroyTopControl(lpstEnv);

                // End expression

                if (SubStateTop(lpstEnv) != essSKIP)
                {
                    if (EvalPushOperator(lpstEnv,
                                         eOPERATOR_END) == FALSE)
                    {
                        StateNew(lpstEnv,esERROR);
                        return;
                    }
                }

                // pop (esEXPR)

                StatePop(lpstEnv);

                // Redo token

                RedoToken(lpstEnv);
                return;
            }

            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_CLOSE_PAREN) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_OPEN_PAREN:
            ((LPEXPR)ControlTop(lpstEnv))->wParenDepth++;
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_OPEN_PAREN) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_WORD_VALUE:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushLongOperand(lpstEnv,
                                        lpstEnv->uTokenData.wValue) == FALSE)
                {
                    // Ignore the error
                    //StateNew(lpstEnv,esERROR);
                    //return;
                }
            }
            break;

        case eTOKEN_QUOTED_STRING:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperand(lpstEnv,
                                    eTYPE_STRING,
                                    lpstEnv->uTokenData.String.lpby) == FALSE)
                {
                    // Ignore the error
                    //StateNew(lpstEnv,esERROR);
                    //return;
                }
            }
            break;

        case eTOKEN_NOT:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_NOT) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_AND:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_AND) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_OR:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_OR) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_PLUS:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_ADD) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_MINUS:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_SUB) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_SLASH:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_DIV) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_ASTERISK:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_MUL) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_MOD:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_MOD) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_EQ:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_EQ) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_NE:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_NE) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_LT:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_LT) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_GT:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_GT) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_LE:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_LE) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_GE:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_GE) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_IDENTIFIER:
            StatePush(lpstEnv,esEXPR_IDENTIFIER);
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (ControlCreateControl(lpstEnv,ectIDENTIFIER) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }

            break;

        case eTOKEN_DIALOG:
            StatePush(lpstEnv,esEXPR_DIALOG);
            break;

        case eTOKEN_FUNCTION_VALUE:
            StatePush(lpstEnv,esEXPR_BUILTIN);
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (ControlCreateControl(lpstEnv,ectBUILTIN) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_COMMA:
        default:
            // End expression

            assert(((LPEXPR)ControlTop(lpstEnv))->ecType == ectEXPR);
            ControlDestroyTopControl(lpstEnv);
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushOperator(lpstEnv,
                                     eOPERATOR_END) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }

            // pop (esSTATE_EXPR)

            StatePop(lpstEnv);

            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateFUNCTION_CALL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an identifier is seen and all of the
//  arguments to it have been gotten.
//
//  If the current substate is essSKIP, the state is popped and the
//  token is redone.
//
//  The identifier may either be a variable or a function identifier.
//  The state first checks to see if the identifier is equal to the Err
//  identifier.  If so, then the state pushes the current value of the
//  error variable onto the operand stack.  Otherwise, the state makes
//  a check to determine whether the identifier is that of a variable
//  or a function.
//
//  If the identifier is that of a variable, the state checks to see
//  whether the variable has been defined yet or not.  If not, then
//  if the identifier ends in a '$', then an empty string is pushed
//  onto the operand stack.  If the identifier does not end in a '$',
//  then the value zero is pushed.
//
//  Otherwise, the identifier is assumed to be that of a function
//  in the same module.  If the IP of the function cannot be found,
//  the state transitions to the esERROR state.
//
//  Assuming the location of the function is found, the following
//  steps occur to transfer control to the function and to ensure
//  a proper return:
//      1. Create a local variable frame
//      2. Create the local variable with the same name as the
//         function and that will be used to hold the return value.
//         The variable is initialized with the empty string if the
//         identifier ends with a '$', and a zero otherwise.
//      3. Push the esCALL_FUNCTION state which will properly clean
//         up after the function call.
//      4. Create the CALL control and set the return IP field.
//      5. Push the routine start IP field in the environment so
//         that it is globally known where execution of the function
//         starts.
//      6. Set up the pointer to the parameters and push to
//         transition to the esCALL_GET_ARGUMENTS state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateFUNCTION_CALL
(
    LPENV       lpstEnv
)
{
    EIDENTIFIER_T   eIdentifierType;
    LPIDENTIFIER    lpstIdentifier;
    LPVAR           lpstVar;
    LPELEMENT       lpstElement;
    LPPARAMETER     lpstParam;
    LPVOID          lpvData;
    LPCALL          lpstCall;
    LPCALL_GET_ARGS lpstCallGetArgs;

    assert(StateTop(lpstEnv) == esFUNCTION_CALL);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esFUNCTION_CALL)

        RedoToken(lpstEnv);

        StatePop(lpstEnv);
        return;
    }

    // Get the top control which is the identifier of the variable
    //  to pass by reference or it could be a function call with
    //  no parameters

    lpstIdentifier = (LPIDENTIFIER)ControlTop(lpstEnv);

    assert(lpstIdentifier->ecType == ectIDENTIFIER);

    if (PascalEqErrStr(lpstIdentifier->lpbyName) == TRUE)
    {
        // Push the value of error

        if (EvalPushLongOperand(lpstEnv,lpstEnv->lErr) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        ControlDestroyTopControl(lpstEnv);
        StatePop(lpstEnv);
        return;
    }

    eIdentifierType =
        ModuleEnvClassifyExprIdentifier(lpstEnv,
                                        lpstIdentifier->lpbyName[0],
                                        lpstIdentifier->lpbyName + 1);

    lpstParam = lpstIdentifier->u.lpstFirstParam;
    if (eIdentifierType == eIDENTIFIER_LOCAL_VAR ||
        eIdentifierType == eIDENTIFIER_GLOBAL_VAR)
    {
        RedoToken(lpstEnv);

        // Get the variable

        if (lpstParam == NULL)
        {
            lpstVar = VarGet(lpstEnv,
                             lpstIdentifier->lpbyName[0],
                             lpstIdentifier->lpbyName+1);

            if (lpstVar != NULL)
            {
                if (lpstVar->eType == eTYPE_VAR)
                    lpstVar = (LPVAR)lpstVar->uData.lpvstVar;

                if (lpstVar->eType == eTYPE_LONG)
                {
                    if (EvalPushLongOperand(lpstEnv,
                                            lpstVar->uData.lValue) == FALSE)
                    {
                        StateNew(lpstEnv,esERROR);
                        return;
                    }
                }
                else
                if (lpstVar->eType == eTYPE_STRING)
                {
                    if (EvalPushOperand(lpstEnv,
                                        eTYPE_STRING,
                                        lpstVar->uData.lpbyStr) == FALSE)
                    {
                        StateNew(lpstEnv,esERROR);
                        return;
                    }
                }
                else
                {
                    // It must be unknown

                    if (lpstIdentifier->
                        lpbyName[lpstIdentifier->lpbyName[0]] == '$')
                    {
                        // Push an empty string

                        if (EvalPushSZOperand(lpstEnv,"") == FALSE)
                        {
                            StateNew(lpstEnv,esERROR);
                            return;
                        }
                    }
                    else
                    {
                        // Push 0

                        if (EvalPushLongOperand(lpstEnv,0) == FALSE)
                        {
                            StateNew(lpstEnv,esERROR);
                            return;
                        }
                    }
                }

                ControlDestroyTopControl(lpstEnv);
                StatePop(lpstEnv);
                return;
            }
        }
        else
        {
            lpstVar = VarArrayGet(lpstEnv,
                                  lpstIdentifier->lpbyName[0],
                                  lpstIdentifier->lpbyName+1);
        }

        // Get the array element if necessary

        if (lpstVar != NULL)
        {
            WORD        wIndex0;
            WORD        wIndex1;

            switch (lpstParam->eType)
            {
                case eTYPE_LONG:
                    wIndex0 = (WORD)lpstParam->uData.lValue;
                    break;

                case eTYPE_VAR:
                    if (((LPVAR)lpstParam->uData.lpvstVar)->eType ==
                        eTYPE_LONG)
                        wIndex0 = (WORD)
                            ((LPVAR)lpstParam->uData.lpvstVar)->
                                uData.lValue;
                    break;

                default:
                    wIndex0 = 0;
                    break;
            }

            if (lpstParam->lpstNext != NULL)
            {
                lpstParam = lpstParam->lpstNext;

                switch (lpstParam->eType)
                {
                    case eTYPE_LONG:
                        wIndex1 = (WORD)lpstParam->uData.lValue;
                        break;

                    case eTYPE_VAR:
                        if (((LPVAR)lpstParam->uData.lpvstVar)->eType ==
                            eTYPE_LONG)
                            wIndex1 = (WORD)
                                ((LPVAR)lpstParam->uData.lpvstVar)->
                                    uData.lValue;
                        break;

                    default:
                        wIndex1 = 0;
                        break;
                }
            }
            else
                wIndex1 = 0;

            // Delete the index parameters

            if (ControlFreeParameterList(lpstEnv,
                                         lpstIdentifier->
                                             u.lpstFirstParam) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }

            lpstIdentifier->u.lpstFirstParam = NULL;

            // Find the element

            lpstElement = VarArrayGetElement(lpstVar,
                                             wIndex0,
                                             wIndex1);

            if (lpstElement != NULL)
            {
                if (lpstElement->eType == eTYPE_LONG)
                    lpvData = &lpstElement->uData.lValue;
                else
                    lpvData = lpstElement->uData.lpbyStr;

                if (EvalPushOperand(lpstEnv,
                                    lpstElement->eType,
                                    lpvData) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }

                ControlDestroyTopControl(lpstEnv);
                StatePop(lpstEnv);
                return;
            }
        }

        // Variable has not been assigned a value yet

        assert(lpstIdentifier->lpbyName[0] > 0);

        if (lpstIdentifier->
            lpbyName[lpstIdentifier->lpbyName[0]] == '$')
        {
            // Push an empty string

            if (EvalPushSZOperand(lpstEnv,"") == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
        }
        else
        {
            // Push 0

            if (EvalPushLongOperand(lpstEnv,0) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
        }

        ControlDestroyTopControl(lpstEnv);
        StatePop(lpstEnv);
        return;
    }

    // It is a function call

    if (ModuleEnvGetFunctionIP(lpstEnv,
                               lpstIdentifier->lpbyName[0],
                               lpstIdentifier->lpbyName + 1,
                               &lpstEnv->dwNextIP) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Create a frame

    if (VarPushFrame(lpstEnv) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Create the variable for the return value

    lpstVar = VarAlloc(lpstEnv,
                       FALSE,
                       FALSE,
                       lpstIdentifier->lpbyName[0],
                       lpstIdentifier->lpbyName + 1);

    if (lpstVar == NULL)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Set the initial value

    if (lpstIdentifier->lpbyName[lpstIdentifier->lpbyName[0]] == '$')
    {
        BYTE by = 0;

        if (VarSet(lpstEnv,
                   lpstVar,
                   eTYPE_STRING,
                   &by) == NULL)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }
    else
    {
        long l = 0;

        if (VarSet(lpstEnv,
                   lpstVar,
                   eTYPE_LONG,
                   &l) == NULL)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }

    ControlDestroyTopControl(lpstEnv);
    StateNew(lpstEnv,esCALL_FUNCTION);

    if (ControlCreateControl(lpstEnv,ectCALL) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    lpstCall = (LPCALL)ControlTop(lpstEnv);

    lpstCall->dwReturnIP = lpstEnv->dwIP;

    // Push the routine start IP

    lpstCall->dwRoutineStartIP = lpstEnv->dwRoutineStartIP;
    lpstEnv->dwRoutineStartIP = lpstEnv->dwNextIP;

    StatePush(lpstEnv,esCALL_GET_ARGUMENTS);

    if (ControlCreateControl(lpstEnv,ectCALL_GET_ARGS) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    lpstCallGetArgs = (LPCALL_GET_ARGS)ControlTop(lpstEnv);
    lpstCallGetArgs->lpstParams = lpstParam;
}


//*************************************************************************
//
// void StateSUB_CALL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an identifier is seen as the first token
//  of a statement and all of the arguments to it have been gotten.
//
//  If the current token is eTOKEN_EQ, then the statement is an
//  assignment.  The state transitions to the expression evaluation
//  state and then the esASSIGN state.
//
//  Otherwise the statement is assumed to be a subroutine call to a
//  subroutine in the same module.  If the current substate is essSKIP,
//  the current token is redone and the esSUB_CALL state is popped.
//
//  If the current substate is not essSKIP, the IP of the subroutine
//  is gotten.  But if the IP of the subroutine cannot be found,
//  the state ignores the call and frees all resources associated with
//  the call if the parameter list is not empty.  Otherwise, the
//  state assumes that it is an external module call and transitions
//  to the esEXTERNAL_SUB_CALL state.
//
//  Assuming the location of the subroutine is found, the following
//  steps occur to transfer control to the subroutine and to ensure
//  a proper return:
//      1. Create a local variable frame
//      2. Push the esCALL_SUB state which will properly clean
//         up after the subroutine call.
//      3. Create the CALL control and set the return IP field.
//      4. Push the routine start IP field in the environment so
//         that it is globally known where execution of the function
//         starts.
//      5. Set up the pointer to the parameters and push to
//         transition to the esCALL_GET_ARGUMENTS state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSUB_CALL
(
    LPENV       lpstEnv
)
{
    LPIDENTIFIER    lpstIdentifier;
    LPPARAMETER     lpstParam;
    LPCALL          lpstCall;
    LPCALL_GET_ARGS lpstCallGetArgs;

    assert(StateTop(lpstEnv) == esSUB_CALL);

    // Get the top control which is the identifier of the variable
    //  to pass by reference or it could be a function call with
    //  no parameters

    if (lpstEnv->eToken == eTOKEN_EQ)
    {
        // This is an assignment

        StateNew(lpstEnv,esASSIGN);
        StatePush(lpstEnv,esEXPR);
        if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
        if (SubStateTop(lpstEnv) != essSKIP)
        {
            if (EvalPushBegin(lpstEnv) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
        }
    }
    else
    if (SubStateTop(lpstEnv) != essSKIP)
    {
        lpstIdentifier = (LPIDENTIFIER)ControlTop(lpstEnv);

        assert(lpstIdentifier->ecType == ectIDENTIFIER);

        // This is definitely a subroutine call

        if (ModuleEnvGetSubIP(lpstEnv,
                              lpstIdentifier->lpbyName[0],
                              lpstIdentifier->lpbyName + 1,
                              &lpstEnv->dwNextIP) == FALSE)
        {
            // Unknown IP, try external macro call

            // If it has parameters, then it cannot be an external
            //  macro call

            if (lpstIdentifier->u.lpstFirstParam == NULL)
            {
                StateNew(lpstEnv,esEXTERNAL_SUB_CALL);
                RedoToken(lpstEnv);
                return;
            }

            // Delete all the parameters and return

            if (ControlFreeParameterList(lpstEnv,
                                         lpstIdentifier->u.
                                             lpstFirstParam) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }

            lpstIdentifier->u.lpstFirstParam = NULL;

            ControlDestroyTopControl(lpstEnv);
            StatePop(lpstEnv);
            return;
        }

        // Create a frame

        if (VarPushFrame(lpstEnv) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        // Remember the parameters

        lpstParam = lpstIdentifier->u.lpstFirstParam;

        ControlDestroyTopControl(lpstEnv);
        StateNew(lpstEnv,esCALL_SUB);

        if (ControlCreateControl(lpstEnv,ectCALL) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        lpstCall = (LPCALL)ControlTop(lpstEnv);

        lpstCall->dwReturnIP = lpstEnv->dwIP;

        // Push the routine start IP

        lpstCall->dwRoutineStartIP = lpstEnv->dwRoutineStartIP;
        lpstEnv->dwRoutineStartIP = lpstEnv->dwNextIP;

        StatePush(lpstEnv,esCALL_GET_ARGUMENTS);

        if (ControlCreateControl(lpstEnv,ectCALL_GET_ARGS) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        lpstCallGetArgs = (LPCALL_GET_ARGS)ControlTop(lpstEnv);
        lpstCallGetArgs->lpstParams = lpstParam;
    }
    else
    {
        RedoToken(lpstEnv);

        // pop (esSUB_CALL)

        StatePop(lpstEnv);
    }
}


//*************************************************************************
//
// void StateEXTERNAL_FUNCTION_CALL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an identifier is seen in an expression
//  followed by a dotted identifier and all of the arguments to it
//  have been gotten.
//
//  If the current substate is essSKIP, the state is popped and the
//  token is redone.
//
//  The identifier may either be a function identifier or a reference
//  to the dialog field of a dialog variable.  To determine which,
//  the state attempts to find a variable by the same name.  If one
//  is found and it is a dialog variable, then it is a reference
//  to a dialog field.  Otherwise, it is assumed to be a call to a
//  function of some other module.
//
//  The dotted identifier is passed along as the first parameter.
//  For now, for a dialog field reference, a zero is pushed onto the
//  operand stack, the token is redone, the parameters are freed,
//  and control is returned to the waiting state.
//
//  For an external function call, the state sets up the call to the
//  function in the external module by calling ExternalCallSetup().
//  If the call fails, the state transitions to esERROR.
//
//  Assuming the location of the function is found, the following
//  steps occur to transfer control to the function and to ensure
//  a proper return:
//      1. Create a local variable frame
//      2. Create the local variable with the same name as the
//         function and that will be used to hold the return value.
//         The variable is initialized with the empty string if the
//         identifier ends with a '$', and a zero otherwise.
//      3. Push the esEXTERNAL_CLEANUP state which will properly clean
//         up upon returning from the external function.
//      4. Push the esCALL_FUNCTION state which will properly clean
//         up after the function call.
//      5. Create the CALL control and set the return IP field.
//      6. Push the routine start IP field in the environment so
//         that it is globally known where execution of the function
//         starts.
//      7. Set up the pointer to the parameters and push to
//         transition to the esCALL_GET_ARGUMENTS state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateEXTERNAL_FUNCTION_CALL
(
    LPENV           lpstEnv
)
{
    LPPARAMETER     lpstParam;
    LPIDENTIFIER    lpstIdentifier;
    LPCALL          lpstCall;
    LPCALL_GET_ARGS lpstCallGetArgs;
    LPVAR           lpstVar;

    assert(StateTop(lpstEnv) == esEXTERNAL_FUNCTION_CALL);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esEXTERNAL_FUNCTION_CALL)

        RedoToken(lpstEnv);

        StatePop(lpstEnv);
        return;
    }

    lpstIdentifier = (LPIDENTIFIER)ControlTop(lpstEnv);

    // The first parameter should be a string containing the name
    //  of the function or a user dialog field

    lpstParam = lpstIdentifier->u.lpstFirstParam;

    // Check to see if it is a reference to a dialog

    lpstVar = VarGet(lpstEnv,
                     lpstIdentifier->lpbyName[0],
                     lpstIdentifier->lpbyName+1);

    if (lpstVar != NULL && lpstVar->eType == eTYPE_DIALOG)
    {
        // Just push a zero

        if (EvalPushLongOperand(lpstEnv,0) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        // Free the field identifier

        if (ControlFreeParameter(lpstEnv,lpstParam) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        // Destroy the identifier control

        ControlDestroyTopControl(lpstEnv);

        // pop (esEXTERNAL_FUNCTION_CALL)

        StatePop(lpstEnv);

        RedoToken(lpstEnv);

        return;
    }


    assert(lpstParam->eType == eTYPE_STRING);

    // Set everything up for an external call

    if (ExternalCallSetup(lpstEnv,
                          lpstIdentifier->lpbyName,
                          lpstParam->uData.lpbyStr,
                          NULL) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Remove the first parameter which is the function identifier

    lpstIdentifier->u.lpstFirstParam = lpstParam->lpstNext;

    // Create a local frame

    if (VarPushFrame(lpstEnv) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Create the variable for the return value

    if (VarAlloc(lpstEnv,
                 FALSE,
                 FALSE,
                 lpstParam->uData.lpbyStr[0],
                 lpstParam->uData.lpbyStr + 1) == NULL)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Free the parameter

    if (ControlFreeParameter(lpstEnv,lpstParam) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Remember the first parameter

    lpstParam = lpstIdentifier->u.lpstFirstParam;

    // Destroy the identifier control

    ControlDestroyTopControl(lpstEnv);

    // Need to clean up after the function return

    StateNew(lpstEnv,esEXTERNAL_CLEANUP);

    StatePush(lpstEnv,esCALL_FUNCTION);

    if (ControlCreateControl(lpstEnv,ectCALL) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    lpstCall = (LPCALL)ControlTop(lpstEnv);

    lpstCall->dwReturnIP = lpstEnv->dwIP;

    // Push the routine start IP

    lpstCall->dwRoutineStartIP = lpstEnv->dwRoutineStartIP;
    lpstEnv->dwRoutineStartIP = lpstEnv->dwNextIP;

    StatePush(lpstEnv,esCALL_GET_ARGUMENTS);

    if (ControlCreateControl(lpstEnv,ectCALL_GET_ARGS) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    lpstCallGetArgs = (LPCALL_GET_ARGS)ControlTop(lpstEnv);
    lpstCallGetArgs->lpstParams = lpstParam;
}


//*************************************************************************
//
// void StateEXTERNAL_SUB_CALL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an identifier is seen as the first token
//  of a statement with no parameters or followed by a dotted identifier
//  and all of the arguments to it have been gotten.
//
//  If the current substate is essSKIP, the state is popped and the
//  token is redone.
//
//  If the identifier is alone, then it is assumed to be a call to
//  the main subroutine of an external module.  If not alone, then
//  it is assumed to be a call to some other subroutine of an external
//  module.
//
//  For an external function call, the state sets up the call to the
//  subroutine in the external module by calling ExternalCallSetup().
//  If the call fails, the call is ignored and the state returns
//  to the waiting state.
//
//  Assuming the location of the subroutine is found, the following
//  steps occur to transfer control to it and to ensure
//  a proper return:
//      1. Create a local variable frame
//      2. Push the esEXTERNAL_CLEANUP state which will properly clean
//         up upon returning from the external function.
//      3. Push the esCALL_SUB state which will properly clean
//         up after the function call.
//      4. Create the CALL control and set the return IP field.
//      5. Push the routine start IP field in the environment so
//         that it is globally known where execution of the function
//         starts.
//      6. Set up the pointer to the parameters and push to
//         transition to the esCALL_GET_ARGUMENTS state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateEXTERNAL_SUB_CALL
(
    LPENV           lpstEnv
)
{
    LPPARAMETER     lpstParam;
    LPIDENTIFIER    lpstIdentifier;
    LPCALL          lpstCall;
    LPCALL_GET_ARGS lpstCallGetArgs;

    assert(StateTop(lpstEnv) == esEXTERNAL_SUB_CALL);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esEXTERNAL_SUB_CALL)

        RedoToken(lpstEnv);

        StatePop(lpstEnv);
        return;
    }

    // The first parameter should be a string containing the name
    //  of the function

    lpstIdentifier = (LPIDENTIFIER)ControlTop(lpstEnv);
    lpstParam = lpstIdentifier->u.lpstFirstParam;

    if (lpstParam == NULL)
    {
        // Set everything up for an external call

        if (ExternalCallSetup(lpstEnv,
                              lpstIdentifier->lpbyName,
                              NULL,
                              NULL) == FALSE)
        {
            // Just ignore the call

            ControlDestroyTopControl(lpstEnv);
            StatePop(lpstEnv);
            RedoToken(lpstEnv);
            return;
        }
    }
    else
    {
        assert(lpstParam->eType == eTYPE_STRING);

        if (ExternalCallSetup(lpstEnv,
                              lpstIdentifier->lpbyName,
                              NULL,
                              lpstParam->uData.lpbyStr) == FALSE)
        {
            // Just ignore the call

            ControlDestroyTopControl(lpstEnv);
            StatePop(lpstEnv);
            RedoToken(lpstEnv);
            return;
        }

        // Remove the first parameter which is the function identifier

        lpstIdentifier->u.lpstFirstParam = lpstParam->lpstNext;

        // Free the parameter which is the name of the function

        if (ControlFreeParameter(lpstEnv,lpstParam) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }

    // Create a local frame

    if (VarPushFrame(lpstEnv) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Remember the first parameter

    lpstParam = lpstIdentifier->u.lpstFirstParam;

    // Destroy the identifier control

    ControlDestroyTopControl(lpstEnv);

    // Need to clean up after the function return

    StateNew(lpstEnv,esEXTERNAL_CLEANUP);

    StatePush(lpstEnv,esCALL_SUB);

    if (ControlCreateControl(lpstEnv,ectCALL) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    lpstCall = (LPCALL)ControlTop(lpstEnv);

    lpstCall->dwReturnIP = lpstEnv->dwIP;

    // Push the routine start IP

    lpstCall->dwRoutineStartIP = lpstEnv->dwRoutineStartIP;
    lpstEnv->dwRoutineStartIP = lpstEnv->dwNextIP;

    StatePush(lpstEnv,esCALL_GET_ARGUMENTS);

    if (ControlCreateControl(lpstEnv,ectCALL_GET_ARGS) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    lpstCallGetArgs = (LPCALL_GET_ARGS)ControlTop(lpstEnv);
    lpstCallGetArgs->lpstParams = lpstParam;
}


//*************************************************************************
//
// void StateEXTERNAL_CLEANUP()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state should never be called.  It is only a placeholder.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateEXTERNAL_CLEANUP
(
    LPENV           lpstEnv
)
{
    // This function should never be called

    assert(0);

    StateNew(lpstEnv,esERROR);
}


//*************************************************************************
//
// void StateCALL_GET_PARAM()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state is used for getting parameters.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCALL_GET_PARAM
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esCALL_GET_PARAM);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_IDENTIFIER:
            // Hold, possibly passing by reference

            if (SubStateTop(lpstEnv) == essSKIP)
            {
                StateNew(lpstEnv,esCALL_PUSH_PARAM);
                StatePush(lpstEnv,esEXPR);
                if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
                RedoToken(lpstEnv);
            }
            else
            {
                StateNew(lpstEnv,esCALL_PASS_BY_REF);
                if (ControlCreateControl(lpstEnv,
                                         ectIDENTIFIER) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_CLOSE_PAREN:
        case eTOKEN_NEWLINE:
        case eTOKEN_LABEL:
        case eTOKEN_WORD_LABEL:
            StatePop(lpstEnv);

            assert(StateTop(lpstEnv) == esFUNCTION_CALL ||
                   StateTop(lpstEnv) == esSUB_CALL ||
                   StateTop(lpstEnv) == esBUILTIN_FUNCTION_CALL ||
                   StateTop(lpstEnv) == esBUILTIN_SUB_CALL ||
                   StateTop(lpstEnv) == esDIALOG_FUNCTION_CALL ||
                   StateTop(lpstEnv) == esDIALOG_SUB_CALL ||
                   StateTop(lpstEnv) == esEXTERNAL_FUNCTION_CALL ||
                   StateTop(lpstEnv) == esEXTERNAL_SUB_CALL);

            if (lpstEnv->eToken != eTOKEN_CLOSE_PAREN)
                RedoToken(lpstEnv);

            break;

        case eTOKEN_DIALOG_FIELD:
            StatePop(lpstEnv);

            if (SubStateTop(lpstEnv) != essSKIP)
            {
                // Push dialog field as a parameter

                if (ControlCallPushParameter(lpstEnv,
                                             eTYPE_DIALOG_FIELD,
                                             &lpstEnv->uTokenData.
                                                 wValue) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }

            StatePush(lpstEnv,esGET_DIALOG_FIELD_PARAM);
            break;

        default:
            // Need to evaluate the result of an expression and then
            //  push the result as the parameter

            StateNew(lpstEnv,esCALL_PUSH_PARAM);

            // Push expr begin

            StatePush(lpstEnv,esEXPR);
            if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushBegin(lpstEnv) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }

            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateGET_DIALOG_FIELD_PARAM()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state is used for getting the argument for a given
//  dialog field parameter.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateGET_DIALOG_FIELD_PARAM
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esGET_DIALOG_FIELD_PARAM);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_EQ:
            StateNew(lpstEnv,esCALL_PUSH_PARAM);

            // Push expr begin

            StatePush(lpstEnv,esEXPR);
            if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushBegin(lpstEnv) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_COMMA:
            StateNew(lpstEnv,esCALL_GET_PARAM);
            break;

        default:
            StatePop(lpstEnv);
            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateCALL_PUSH_PARAM()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state is used for pushing a parameter.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCALL_PUSH_PARAM
(
    LPENV       lpstEnv
)
{
    ETYPE_T     eType;
    LPVOID      lpvData;

    assert(StateTop(lpstEnv) == esCALL_PUSH_PARAM);

    // pop (esCALL_PUSH_PARAM)

    StatePop(lpstEnv);

    if (SubStateTop(lpstEnv) != essSKIP)
    {
        // Get operand from stack

        if (EvalQueryTopOperand(lpstEnv,
                                &eType,
                                &lpvData) != FALSE)
        {
           // Push parameter

            if (ControlCallPushParameter(lpstEnv,
                                         eType,
                                         lpvData) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }

            // Free the operand

            if (EvalPopTopOperand(lpstEnv) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
        }
    }

    if (lpstEnv->eToken == eTOKEN_COMMA)
    {
        StatePush(lpstEnv,esCALL_GET_PARAM);
    }
    else
    {
        assert(StateTop(lpstEnv) == esFUNCTION_CALL ||
               StateTop(lpstEnv) == esSUB_CALL ||
               StateTop(lpstEnv) == esBUILTIN_FUNCTION_CALL ||
               StateTop(lpstEnv) == esBUILTIN_SUB_CALL ||
               StateTop(lpstEnv) == esDIALOG_FUNCTION_CALL ||
               StateTop(lpstEnv) == esDIALOG_SUB_CALL ||
               StateTop(lpstEnv) == esEXTERNAL_FUNCTION_CALL ||
               StateTop(lpstEnv) == esEXTERNAL_SUB_CALL);

        if (lpstEnv->eToken != eTOKEN_CLOSE_PAREN)
            RedoToken(lpstEnv);
    }
}


//*************************************************************************
//
// void StateCALL_PUSH_PARAM()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state is used for potentially passing a variable by reference.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCALL_PASS_BY_REF
(
    LPENV       lpstEnv
)
{
    EIDENTIFIER_T   eIdentifierType;
    LPIDENTIFIER    lpstIdentifier;
    LPVAR           lpstVar;

    assert(StateTop(lpstEnv) == esCALL_PASS_BY_REF);

    assert(SubStateTop(lpstEnv) != essSKIP);

    // Get the top control which is the identifier of the variable
    //  to pass by reference or it could be a function call with
    //  no parameters

    lpstIdentifier = (LPIDENTIFIER)ControlTop(lpstEnv);

    assert(lpstIdentifier->ecType == ectIDENTIFIER);

    // Get the ptr to the variable

    eIdentifierType =
        ModuleEnvClassifyExprIdentifier(lpstEnv,
                                        lpstIdentifier->lpbyName[0],
                                        lpstIdentifier->lpbyName + 1);

    if (eIdentifierType == eIDENTIFIER_LOCAL_VAR ||
        eIdentifierType == eIDENTIFIER_GLOBAL_VAR)
    {
        lpstVar = VarGet(lpstEnv,
                         lpstIdentifier->lpbyName[0],
                         lpstIdentifier->lpbyName+1);

        if (lpstVar == NULL)
        {
            // Create it

            lpstVar = VarAlloc(lpstEnv,
                               (eIdentifierType == eIDENTIFIER_LOCAL_VAR) ?
                                FALSE : TRUE,
                               FALSE,
                               lpstIdentifier->lpbyName[0],
                               lpstIdentifier->lpbyName+1);

            if (lpstVar == NULL)
            {
                // Could not allocate variable

                StateNew(lpstEnv,esERROR);
                return;
            }
        }

        switch (lpstEnv->eToken)
        {
            case eTOKEN_COMMA:
            case eTOKEN_CLOSE_PAREN:
            case eTOKEN_NEWLINE:
            case eTOKEN_LABEL:
            case eTOKEN_WORD_LABEL:
                // Destroy identifier control

                ControlDestroyTopControl(lpstEnv);

                // pop (esCALL_PASS_BY_REF)

                StatePop(lpstEnv);

                assert(StateTop(lpstEnv) == esFUNCTION_CALL ||
                       StateTop(lpstEnv) == esSUB_CALL ||
                       StateTop(lpstEnv) == esBUILTIN_FUNCTION_CALL ||
                       StateTop(lpstEnv) == esBUILTIN_SUB_CALL ||
                       StateTop(lpstEnv) == esDIALOG_FUNCTION_CALL ||
                       StateTop(lpstEnv) == esDIALOG_SUB_CALL ||
                       StateTop(lpstEnv) == esEXTERNAL_FUNCTION_CALL ||
                       StateTop(lpstEnv) == esEXTERNAL_SUB_CALL);

                // Pass variable by reference

                if (ControlCallPushParameter(lpstEnv,
                                             eTYPE_VAR,
                                             lpstVar) == FALSE)
                {
                    StatePush(lpstEnv,esERROR);
                }

                if (lpstEnv->eToken == eTOKEN_COMMA)
                {
                    // Get another parameter

                    StatePush(lpstEnv,esCALL_GET_PARAM);
                }

                return;

            default:
                break;
        }
    }

    ControlSetTopControl(lpstEnv,NULL);

    // Push expr begin

    StateNew(lpstEnv,esCALL_PUSH_PARAM);

    StatePush(lpstEnv,esEXPR);
    if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }
    if (EvalPushBegin(lpstEnv) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    StatePush(lpstEnv,esEXPR_IDENTIFIER);
    ControlSetTopControl(lpstEnv,lpstIdentifier);

    RedoToken(lpstEnv);
}


//*************************************************************************
//
// void StateCALL_FUNCTION()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after returning from a function call.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCALL_FUNCTION
(
    LPENV       lpstEnv
)
{
    LPCALL      lpstCall;
    ETYPE_T     eType;
    LPVOID      lpvData;

    assert(StateTop(lpstEnv) == esCALL_FUNCTION);

    assert(SubStateTop(lpstEnv) != essSKIP);

    // Push the return value onto the operand stack

    if (VarGetFirstFrameVar(lpstEnv,
                            &eType,
                            &lpvData) == NULL)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    if (EvalPushOperand(lpstEnv,
                        eType,
                        lpvData) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Pop the local variable frame

    if (VarPopFrame(lpstEnv) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Return to the caller

    lpstCall = (LPCALL)ControlTop(lpstEnv);

    lpstEnv->dwNextIP = lpstCall->dwReturnIP;

    // Restore routine start IP

    lpstEnv->dwRoutineStartIP = lpstCall->dwRoutineStartIP;

    ControlDestroyTopControl(lpstEnv);

    // pop (esCALL_FUNCTION)

    StatePop(lpstEnv);

    if (StateTop(lpstEnv) == esEXTERNAL_CLEANUP)
    {
        if (ExternalCallCleanup(lpstEnv) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        // pop (esEXTERNAL_CLEANUP)

        StatePop(lpstEnv);
    }
}


//*************************************************************************
//
// void StateCALL_SUB()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after returning from a subroutine call.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCALL_SUB
(
    LPENV       lpstEnv
)
{
    LPCALL      lpstCall;

    assert(StateTop(lpstEnv) == esCALL_SUB);

    assert(SubStateTop(lpstEnv) != essSKIP);

    // Pop the local variable frame

    if (VarPopFrame(lpstEnv) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Return to the caller

    lpstCall = (LPCALL)ControlTop(lpstEnv);

    lpstEnv->dwNextIP = lpstCall->dwReturnIP;

    // Restore routine start IP

    lpstEnv->dwRoutineStartIP = lpstCall->dwRoutineStartIP;

    ControlDestroyTopControl(lpstEnv);

    // pop (esCALL_SUB)

    StatePop(lpstEnv);

    if (StateTop(lpstEnv) == esEXTERNAL_CLEANUP)
    {
        if (ExternalCallCleanup(lpstEnv) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        // pop (esEXTERNAL_CLEANUP)

        StatePop(lpstEnv);
    }
}


//*************************************************************************
//
// void StateCALL_GET_ARGUMENTS()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state is used for assigning arguments to parameter variables
//  upon entering a subroutine or function.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCALL_GET_ARGUMENTS
(
    LPENV       lpstEnv
)
{
    LPVAR           lpstVar;
    LPCALL_GET_ARGS lpstCallGetArgs;
    LPPARAMETER     lpstParam;
    LPVOID          lpvData;

    assert(StateTop(lpstEnv) == esCALL_GET_ARGUMENTS);
    assert(SubStateTop(lpstEnv) != essSKIP);
    assert(((LPEXPR)ControlTop(lpstEnv))->ecType == ectCALL_GET_ARGS);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_IDENTIFIER:
            lpstCallGetArgs = (LPCALL_GET_ARGS)ControlTop(lpstEnv);
            lpstParam = lpstCallGetArgs->lpstParams;

            if (lpstParam == NULL)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }

            // Grab argument value and create local variable

            lpstVar = VarAlloc(lpstEnv,
                               FALSE,
                               FALSE,
                               lpstEnv->uTokenData.Identifier.byLen,
                               lpstEnv->uTokenData.Identifier.lpby);

            if (lpstVar == NULL)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }

            if (lpstParam->eType == eTYPE_LONG)
                lpvData = &lpstParam->uData.lValue;
            else
                lpvData = lpstParam->uData.lpbyStr;

            if (VarSet(lpstEnv,
                       lpstVar,
                       lpstParam->eType,
                       lpvData) == NULL)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }

            // Free the parameter

            lpstCallGetArgs->lpstParams = lpstParam->lpstNext;
            if (ControlFreeParameter(lpstEnv,lpstParam) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }

            break;

        case eTOKEN_NEWLINE:
        case eTOKEN_LABEL:
        case eTOKEN_WORD_LABEL:
            lpstCallGetArgs = (LPCALL_GET_ARGS)ControlTop(lpstEnv);
            if (lpstCallGetArgs->lpstParams != NULL)
            {
                // All parameters were not exhausted, free the rest

                if (ControlFreeParameterList(lpstEnv,
                                             lpstCallGetArgs->
                                                 lpstParams) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }

                lpstCallGetArgs->lpstParams = NULL;
            }

            ControlDestroyTopControl(lpstEnv);

            // pop (esCALL_GET_ARGUMENTS)

            StatePop(lpstEnv);

            // Transition to the body

            StatePush(lpstEnv,esCALL_BODY);
            if (lpstEnv->eToken != eTOKEN_NEWLINE)
                RedoToken(lpstEnv);
            break;

        default:
            // Ignore everything else
            break;
    }
}


//*************************************************************************
//
// void StateCALL_GET_ARGUMENTS()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state executes the statements of a subroutine or function body.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCALL_BODY
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esCALL_BODY);

    assert(SubStateTop(lpstEnv) != essSKIP);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_END:
            // Transition to the CALL_END state

            StateNew(lpstEnv,esCALL_END);
            break;

        case eTOKEN_NEWLINE:
            // Blank line
            break;

        case eTOKEN_NEXT:
        case eTOKEN_WEND:
            // Ignore the line
            StatePush(lpstEnv,esSKIP_LINE);
            break;

        default:
            // redo token

            StatePush(lpstEnv,esSTATEMENT);
            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateCALL_END()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when an END is seen in the body of a subroutine
//  or function.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCALL_END
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esCALL_END);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_IF:
            // Ignore it, transition back

            StateNew(lpstEnv,esCALL_BODY);
            break;

        default:
            // pop (esCALL_END)

            StatePop(lpstEnv);

            RedoToken(lpstEnv);

            assert(StateTop(lpstEnv) == esCALL_FUNCTION ||
                   StateTop(lpstEnv) == esCALL_SUB);
    }
}


//*************************************************************************
//
// BOOL PushDialogFieldValue()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when a dialog field's value is referenced.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL PushDialogFieldValue
(
    LPENV       lpstEnv
)
{
    LPVAR       lpstVar;

    // Look up the dialog structure

    lpstVar = VarGet(lpstEnv,
                     ((LPIDENTIFIER)ControlTop(lpstEnv))->lpbyName[0],
                     ((LPIDENTIFIER)ControlTop(lpstEnv))->lpbyName + 1);

    if (lpstVar != NULL && lpstVar->eType == eTYPE_DIALOG)
    {
        switch (((LPDIALOG)lpstVar->uData.lpvstDlg)->eWDCMD)
        {
            case eWDCMD_FileSaveAs:
            {
                LPDLG_FILESAVEAS lpstDlg =
                    (LPDLG_FILESAVEAS)lpstVar->uData.lpvstDlg;

                switch (lpstEnv->uTokenData.wValue)
                {
                    case eWDDLG_Format:
                        return EvalPushLongOperand(lpstEnv,lpstDlg->byFormat);
                        //return EvalPushLongOperand(lpstEnv,0);

                    case eWDDLG_Name:
                        return EvalPushSZOperand(lpstEnv,lpstEnv->lpszFN);

                    case eWDDLG_Password:
                    case eWDDLG_WritePassword:
                        return EvalPushSZOperand(lpstEnv,"");

                    default:
                        break;
                }
            }

            case eWDCMD_FileNew:
            {
                LPDLG_FILENEW lpstDlg =
                    (LPDLG_FILENEW)lpstVar->uData.lpvstDlg;

                switch (lpstEnv->uTokenData.wValue)
                {
                    case eWDDLG_NewTemplate:
                        return EvalPushLongOperand(lpstEnv,
                                                   lpstDlg->byNewTemplate);

                    default:
                        break;
                }
            }

            case eWDCMD_FileSummaryInfo:
                switch (lpstEnv->uTokenData.wValue)
                {
                    case eWDDLG_FileName:
                        return EvalPushSZOperand(lpstEnv,lpstEnv->lpszFN);

                    case eWDDLG_Directory:
                        return EvalPushSZOperand(lpstEnv,"");

                    case eWDDLG_Template:
                        return EvalPushSZOperand(lpstEnv,lpstEnv->lpszNormal);

                    default:
                        break;
                }
                break;

            case eWDCMD_FileTemplates:
                if (lpstEnv->uTokenData.wValue == eWDDLG_Template)
                {
                    return EvalPushSZOperand(lpstEnv,lpstEnv->lpszFN);
                }
                break;

            default:
                switch (lpstEnv->uTokenData.wValue)
                {
                    case eWDDLG_Name:
                    case eWDDLG_FileName:
                        return EvalPushSZOperand(lpstEnv,lpstEnv->lpszFN);

                    case eWDDLG_Directory:
                        return EvalPushSZOperand(lpstEnv,"");

                    default:
                        break;
                }
                break;
        }
    }

    return EvalPushLongOperand(lpstEnv,0);
}


//*************************************************************************
//
// void StateSTATEMENT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when an identifier is encountered in an
//  expression.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateEXPR_IDENTIFIER
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esEXPR_IDENTIFIER);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_OPEN_PAREN:
            // Could either be an array or a function

            StateNew(lpstEnv,esFUNCTION_CALL);
            StatePush(lpstEnv,esCALL_GET_PARAM);
            break;

        case eTOKEN_DIALOG_FIELD:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                // Get the value of the token and push it onto the operand stack

                if (PushDialogFieldValue(lpstEnv) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }

                ControlDestroyTopControl(lpstEnv);
            }

            // pop (esEXPR_IDENTIFIER)

            StatePop(lpstEnv);
            break;

        case eTOKEN_EXTERNAL_MACRO:
        case eTOKEN_SUB_IDENTIFIER:
            // Make the first parameter the function within the macro

            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (ControlCallPushParameter(lpstEnv,
                                             eTYPE_STRING,
                                             lpstEnv->uTokenData.
                                                 String.lpby) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }

            StateNew(lpstEnv,esEXPR_EXTERNAL);
            break;

        default:
            StateNew(lpstEnv,esFUNCTION_CALL);
            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateEXPR_BUILTIN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when an eTOKEN_FUNCTION_VALUE is encountered in an
//  expression.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateEXPR_BUILTIN
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esEXPR_BUILTIN);

    StateNew(lpstEnv,esBUILTIN_FUNCTION_CALL);
    if (lpstEnv->eToken == eTOKEN_OPEN_PAREN)
        StatePush(lpstEnv,esCALL_GET_PARAM);
    else
        RedoToken(lpstEnv);
}


//*************************************************************************
//
// void StateEXPR_EXTERNAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when an external function call is detected.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateEXPR_EXTERNAL
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esEXPR_EXTERNAL);

    StateNew(lpstEnv,esEXTERNAL_FUNCTION_CALL);
    if (lpstEnv->eToken == eTOKEN_OPEN_PAREN)
        StatePush(lpstEnv,esCALL_GET_PARAM);
    else
        RedoToken(lpstEnv);
}


//*************************************************************************
//
// void StateSTATEMENT_EXTERNAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when an external subroutine call is detected.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSTATEMENT_EXTERNAL
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSTATEMENT_EXTERNAL);

    StateNew(lpstEnv,esEXTERNAL_SUB_CALL);
    switch (lpstEnv->eToken)
    {
        case eTOKEN_OPEN_PAREN:
            StatePush(lpstEnv,esCALL_GET_PARAM);
            break;

        case eTOKEN_EQ:
            // It's an assignment to a dialog field

            // Destroy the IDENTIFIER

            if (SubStateTop(lpstEnv) != essSKIP)
            {
                // The first parameter should be a string containing
                //  the name of the field.  Free it.

                if (ControlFreeParameterList(lpstEnv,
                                             ((LPIDENTIFIER)ControlTop(lpstEnv))->
                                                 u.lpstFirstParam) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }

                ((LPIDENTIFIER)ControlTop(lpstEnv))->u.lpstFirstParam = NULL;

                // Free the IDENTIFIER control

                ControlDestroyTopControl(lpstEnv);
            }

            // Evaluate the expression but ignore the result

            StateNew(lpstEnv,esIGNORE_EXPR_RESULT);
            StatePush(lpstEnv,esEXPR);
            if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushBegin(lpstEnv) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        default:
            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateSTATEMENT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state looks at the first token of a statement and hands
//  off to the appropriate state to handle the token.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSTATEMENT
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSTATEMENT);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_IDENTIFIER:
            StateNew(lpstEnv,esSTATEMENT_IDENTIFIER);
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (ControlCreateControl(lpstEnv,ectIDENTIFIER) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_CALL:
            // The next token should be a subroutine identifier
            break;

        case eTOKEN_FUNCTION_VALUE:
            StateNew(lpstEnv,esSTATEMENT_BUILTIN);

            if (lpstEnv->uTokenData.eWDCMD == eWDCMD_MacroCopy ||
                lpstEnv->uTokenData.eWDCMD == eWDCMD_Organizer)
            {
                // Always do macrocopies and organizer operations

                SubStateNew(lpstEnv,essDO);
            }

            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (ControlCreateControl(lpstEnv,ectBUILTIN) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_IF:
            // Evaluate the conditional expression and then
            //  go to the IF state

            StateNew(lpstEnv,esIF_COND);
            StatePush(lpstEnv,esEXPR);
            if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushBegin(lpstEnv) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }

            break;

        case eTOKEN_FOR:
            StateNew(lpstEnv,esFOR_IDENTIFIER);
            break;

        case eTOKEN_WHILE:
            StateNew(lpstEnv,esWHILE);
            break;

        case eTOKEN_SELECT:
            StateNew(lpstEnv,esSELECT);
            break;

        case eTOKEN_REDIM:
        case eTOKEN_DIM:
            StateNew(lpstEnv,esDIM);
            break;

        case eTOKEN_GET_CUR_VALUES:
            StateNew(lpstEnv,esGET_CUR_VALUES);
            break;

        case eTOKEN_DIALOG:
            StateNew(lpstEnv,esDIALOG_SUB_CALL);
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (ControlCreateControl(lpstEnv,ectDIALOG_CALL) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            StatePush(lpstEnv,esCALL_GET_PARAM);
            break;

        case eTOKEN_BEGIN:
            StateNew(lpstEnv,esBEGIN_DIALOG);
            break;

        case eTOKEN_GOTO:
            StateNew(lpstEnv,esGOTO);
            break;

        case eTOKEN_END:
        case eTOKEN_ELSE:
        case eTOKEN_ELSE_IF:
        case eTOKEN_NEXT:
        case eTOKEN_WEND:
        case eTOKEN_NEWLINE:
            StatePop(lpstEnv);
            RedoToken(lpstEnv);
            break;

        case eTOKEN_ON:
            StatePush(lpstEnv,esON);
            break;

        case eTOKEN_LABEL:
        case eTOKEN_WORD_LABEL:
        case eTOKEN_COLON:
        case eTOKEN_LET:
            // Ignore the label, colons, and let
            break;

        case eTOKEN_PRINT:
        case eTOKEN_INPUT:
        case eTOKEN_LINE:
        case eTOKEN_WRITE:
        case eTOKEN_NAME:
        case eTOKEN_OUTPUT:
        case eTOKEN_OPEN:
        case eTOKEN_DECLARE:
        case eTOKEN_TOOLS_GET_SPELLING:
        case eTOKEN_TOOLS_GET_SYNONYMS:
        case eTOKEN_CLOSE:
        case eTOKEN_READ:
            // Skip the line

            StatePush(lpstEnv,esSKIP_LINE);
            break;

        default:
            // Error?

            //StatePush(lpstEnv,esERROR);

            // Just skip the line

            StatePush(lpstEnv,esSKIP_LINE);
            break;
    }
}


//*************************************************************************
//
// void StateSTATEMENT_DLG_FIELD()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_DIALOG_FIELD is seen after
//  an eTOKEN_IDENTIFIER is seen as the first token of a statement.
//  The state prepares for an assignment to a dialog field of a
//  dialog variable.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSTATEMENT_DLG_FIELD
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSTATEMENT_DLG_FIELD);

    if (lpstEnv->eToken == eTOKEN_EQ)
    {
        StateNew(lpstEnv,esDLG_FIELD_ASSIGN);
        StatePush(lpstEnv,esEXPR);
        if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
        if (SubStateTop(lpstEnv) != essSKIP)
        {
            if (EvalPushBegin(lpstEnv) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
        }
    }
    else
        StateNew(lpstEnv,esERROR);
}


//*************************************************************************
//
// void StateSTATEMENT_IDENTIFIER()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_IDENTIFIER is seen as the
//  first token of a statement.  The state prepares for either a
//  subroutine call or an assignment, depending on what the current
//  token is.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSTATEMENT_IDENTIFIER
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSTATEMENT_IDENTIFIER);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_NEWLINE:
        case eTOKEN_LABEL:
        case eTOKEN_WORD_LABEL:
            StateNew(lpstEnv,esSUB_CALL);
            RedoToken(lpstEnv);
            break;

        case eTOKEN_EQ:
            StateNew(lpstEnv,esASSIGN);
            StatePush(lpstEnv,esEXPR);
            if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (EvalPushBegin(lpstEnv) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_OPEN_PAREN:
            StateNew(lpstEnv,esSUB_CALL);
            StatePush(lpstEnv,esCALL_GET_PARAM);
            break;

        case eTOKEN_DIALOG_FIELD:
            if (SubStateTop(lpstEnv) != essSKIP)
            {
                // Get ready for a dialog field assignment

                ((LPIDENTIFIER)ControlTop(lpstEnv))->u.eWDDLG =
                    lpstEnv->uTokenData.eWDDLG;

            }
            StateNew(lpstEnv,esSTATEMENT_DLG_FIELD);
            break;

        case eTOKEN_EXTERNAL_MACRO:
            // Make the first parameter the subroutine within the macro

            if (SubStateTop(lpstEnv) != essSKIP)
            {
                if (ControlCallPushParameter(lpstEnv,
                                             eTYPE_STRING,
                                             lpstEnv->uTokenData.
                                                 String.lpby) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }

            StateNew(lpstEnv,esSTATEMENT_EXTERNAL);
            break;

        default:
            StateNew(lpstEnv,esSUB_CALL);
            StatePush(lpstEnv,esCALL_GET_PARAM);
            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateSTATEMENT_BUILTIN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_FUNCTION_VALUE is seen as the
//  first token of a statement.  The state prepares for getting the
//  parameters for a built-in subroutine call.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSTATEMENT_BUILTIN
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSTATEMENT_BUILTIN);

    StateNew(lpstEnv,esBUILTIN_SUB_CALL);
    StatePush(lpstEnv,esCALL_GET_PARAM);
    if (lpstEnv->eToken != eTOKEN_OPEN_PAREN)
        RedoToken(lpstEnv);
}


//*************************************************************************
//
// void StateON()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_ON is seen as the first token
//  of a statement.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateON
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esON);

    // pop (esON)

    StatePop(lpstEnv);

    if (lpstEnv->eToken == eTOKEN_ERROR)
    {
        StatePush(lpstEnv,esON_ERROR);
    }
    else
    {
        StatePush(lpstEnv,esERROR);
    }
}


//*************************************************************************
//
// void StateON_ERROR()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_ERROR is seen in the esON state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateON_ERROR
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esON_ERROR);

    // pop (esON_ERROR)

    StatePop(lpstEnv);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_GOTO:
            StatePush(lpstEnv,esON_ERROR_GOTO);
            break;

        case eTOKEN_RESUME:
            StatePush(lpstEnv,esON_ERROR_RESUME);
            break;

        default:
            StatePush(lpstEnv,esERROR);
            break;
    }
}


//*************************************************************************
//
// void StateON_ERROR_GOTO()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_GOTO is seen in the
//  esON_ERROR state.  The statement is ignored.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateON_ERROR_GOTO
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esON_ERROR_GOTO);

    // pop (esON_ERROR_GOTO)

    StatePop(lpstEnv);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_IDENTIFIER:
            break;

        case eTOKEN_WORD_VALUE:
            break;

        default:
            StatePush(lpstEnv,esERROR);
            break;
    }
}


//*************************************************************************
//
// void StateON_ERROR_RESUME()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_RESUME is seen in the
//  esON_ERROR state.  The statement is ignored.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateON_ERROR_RESUME
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esON_ERROR_RESUME);

    // pop (esON_ERROR_RESUME)

    StatePop(lpstEnv);

    if (lpstEnv->eToken == eTOKEN_NEXT)
    {
    }
    else
    {
        StatePush(lpstEnv,esERROR);
    }
}


//*************************************************************************
//
// void StateDLG_FIELD_ASSIGN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state assigns a value to a field of a dialog variable.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateDLG_FIELD_ASSIGN
(
    LPENV           lpstEnv
)
{
    ETYPE_T         eType;
    LPVOID          lpvData;
    LPVAR           lpstVar;

    assert(StateTop(lpstEnv) == esDLG_FIELD_ASSIGN);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esDLG_FIELD_ASSIGN)

        StatePop(lpstEnv);
        return;
    }

    if (EvalQueryTopOperand(lpstEnv,
                            &eType,
                            &lpvData) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Look up the dialog structure

    lpstVar = VarGet(lpstEnv,
                     ((LPIDENTIFIER)ControlTop(lpstEnv))->lpbyName[0],
                     ((LPIDENTIFIER)ControlTop(lpstEnv))->lpbyName + 1);

    if (lpstVar != NULL && lpstVar->eType == eTYPE_DIALOG)
    {
        switch (((LPDIALOG)lpstVar->uData.lpvstDlg)->eWDCMD)
        {
            case eWDCMD_FileSaveAs:
            {
                LPDLG_FILESAVEAS lpstDlg =
                    (LPDLG_FILESAVEAS)lpstVar->uData.lpvstDlg;

                switch (((LPIDENTIFIER)ControlTop(lpstEnv))->u.eWDDLG)
                {
                    case eWDDLG_Format:
                        if (eType == eTYPE_LONG)
                            lpstDlg->byFormat = (BYTE)*(LPLONG)lpvData;
                        else
                            lpstDlg->byFormat = 0;

                        break;

                    default:
                        break;
                }

                if (EvalPopTopOperand(lpstEnv) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
                break;
            }

            case eWDCMD_FileNew:
            {
                LPDLG_FILENEW lpstDlg =
                    (LPDLG_FILENEW)lpstVar->uData.lpvstDlg;

                switch (((LPIDENTIFIER)ControlTop(lpstEnv))->u.eWDDLG)
                {
                    case eWDDLG_NewTemplate:
                        if (eType == eTYPE_LONG)
                            lpstDlg->byNewTemplate = (BYTE)*(LPLONG)lpvData;
                        else
                            lpstDlg->byNewTemplate = 0;

                        break;

                    default:
                        break;
                }

                if (EvalPopTopOperand(lpstEnv) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
                break;
            }

            default:
                break;
        }
    }

    ControlDestroyTopControl(lpstEnv);

    // pop (esDLG_FIELD_ASSIGN)

    StatePop(lpstEnv);

    RedoToken(lpstEnv);
}


//*************************************************************************
//
// void StateASSIGN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state assigns a value to a variable.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateASSIGN
(
    LPENV       lpstEnv
)
{
    ETYPE_T         eType;
    LPVOID          lpvData;
    LPIDENTIFIER    lpstIdentifier;
    EIDENTIFIER_T   eIdentifierType;
    LPVAR           lpstVar;
    LPELEMENT       lpstElement;
    LPPARAMETER     lpstParam;

    assert(StateTop(lpstEnv) == esASSIGN);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esASSIGN)

        StatePop(lpstEnv);

        // Redo the token

        RedoToken(lpstEnv);
        return;
    }

    // Get the data to assign to the variable

    if (EvalQueryTopOperand(lpstEnv,
                            &eType,
                            &lpvData) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Get the top control which is the identifier to assign to

    lpstIdentifier = (LPIDENTIFIER)ControlTop(lpstEnv);

    assert(lpstIdentifier->ecType == ectIDENTIFIER);

    eIdentifierType =
        ModuleEnvClassifyExprIdentifier(lpstEnv,
                                        lpstIdentifier->lpbyName[0],
                                        lpstIdentifier->lpbyName + 1);

    lpstParam = lpstIdentifier->u.lpstFirstParam;

    // Get the variable

    if (lpstParam == NULL)
    {
        lpstVar = VarGet(lpstEnv,
                         lpstIdentifier->lpbyName[0],
                         lpstIdentifier->lpbyName+1);
    }
    else
    {
        lpstVar = VarArrayGet(lpstEnv,
                              lpstIdentifier->lpbyName[0],
                              lpstIdentifier->lpbyName+1);
    }

    // Allocate the variable if necessary

    if (lpstVar == NULL)
    {
        // If the data is NULL or empty, don't create it

        if (lpvData == NULL ||
            (eType == eTYPE_STRING && (*(LPBYTE)lpvData) == 0))
        {
            // Pop top operand value that was assigned

            if (EvalPopTopOperand(lpstEnv) == FALSE)
            {
                StatePush(lpstEnv,esERROR);
                return;
            }

            // Destroy the ASSIGN

            ControlDestroyTopControl(lpstEnv);

            // pop (esASSIGN)

            StatePop(lpstEnv);

            // Redo the token

            RedoToken(lpstEnv);
            return;
        }

        // Create it

        lpstVar = VarAlloc(lpstEnv,
                           (eIdentifierType == eIDENTIFIER_LOCAL_VAR) ?
                            FALSE : TRUE,
                           (lpstParam == NULL) ? FALSE : TRUE,
                           lpstIdentifier->lpbyName[0],
                           lpstIdentifier->lpbyName+1);

        if (lpstVar == NULL)
        {
            // Could not allocate variable

            StateNew(lpstEnv,esERROR);
            return;
        }
    }

    if (lpstParam == NULL)
    {
        // This is an assignment to a normal variable

        if (VarSet(lpstEnv,
                   lpstVar,
                   eType,
                   lpvData) == NULL)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }
    else
    {
        // Get the array element

        WORD        wIndex0;
        WORD        wIndex1;

        switch (lpstParam->eType)
        {
            case eTYPE_LONG:
                wIndex0 = (WORD)lpstParam->uData.lValue;
                break;

            case eTYPE_VAR:
                if (((LPVAR)lpstParam->uData.lpvstVar)->eType ==
                    eTYPE_LONG)
                    wIndex0 = (WORD)
                        ((LPVAR)lpstParam->uData.lpvstVar)->
                            uData.lValue;
                break;

            default:
                wIndex0 = 0;
                break;
        }

        if (lpstParam->lpstNext != NULL)
        {
            lpstParam = lpstParam->lpstNext;

            switch (lpstParam->eType)
            {
                case eTYPE_LONG:
                    wIndex1 = (WORD)lpstParam->uData.lValue;
                    break;

                case eTYPE_VAR:
                    if (((LPVAR)lpstParam->uData.lpvstVar)->eType ==
                        eTYPE_LONG)
                        wIndex1 = (WORD)
                            ((LPVAR)lpstParam->uData.lpvstVar)->
                                uData.lValue;
                    break;

                default:
                    wIndex1 = 0;
                    break;
            }
        }
        else
            wIndex1 = 0;

        // Free the index parameters

        if (ControlFreeParameterList(lpstEnv,
                                     lpstIdentifier->
                                         u.lpstFirstParam) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        lpstIdentifier->u.lpstFirstParam = NULL;

        // Find the element

        lpstElement = VarArrayGetElement(lpstVar,
                                         wIndex0,
                                         wIndex1);

        if (lpstElement == NULL)
        {
            // If the data is NULL or empty, don't create it

            if (lpvData == NULL ||
                (eType == eTYPE_STRING && (*(LPBYTE)lpvData) == 0))
            {
                // Pop top operand value that was assigned

                if (EvalPopTopOperand(lpstEnv) == FALSE)
                {
                    StatePush(lpstEnv,esERROR);
                    return;
                }

                // Destroy the ASSIGN

                ControlDestroyTopControl(lpstEnv);

                // pop (esASSIGN)

                StatePop(lpstEnv);

                // Redo the token

                RedoToken(lpstEnv);
                return;
            }

            lpstElement = VarArrayElementAlloc(lpstEnv,
                                               lpstVar,
                                               wIndex0,
                                               wIndex1);

            if (lpstElement == NULL)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
        }

        // This is an assignment to an array element variable

        if (VarSet(lpstEnv,
                   (LPVAR)lpstElement,
                   eType,
                   lpvData) == NULL)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }

    // Pop top operand value that was assigned

    if (EvalPopTopOperand(lpstEnv) == FALSE)
    {
        StatePush(lpstEnv,esERROR);
        return;
    }

    // Destroy the ASSIGN

    ControlDestroyTopControl(lpstEnv);

    // pop (esASSIGN)

    StatePop(lpstEnv);

    // Redo the token

    RedoToken(lpstEnv);
}


//*************************************************************************
//
// void StateDIM()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_DIM is seen as the
//  first token of a statement.  If the current token is an identifier,
//  the state transitions to esDIM_IDENTIFIER to possibly dimension
//  a dialog variable.  Otherwise, the rest of the line is skipped.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateDIM
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esDIM);

    if (lpstEnv->eToken != eTOKEN_IDENTIFIER)
    {
        StateNew(lpstEnv,esSKIP_LINE);
        return;
    }

    if (ControlCreateControl(lpstEnv,ectIDENTIFIER) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    StateNew(lpstEnv,esDIM_IDENTIFIER);
}


//*************************************************************************
//
// void StateDIM_IDENTIFIER()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an identifier is seen in the esDIM state.
//  If the next token is not eTOKEN_AS, the rest of the line is skipped.
//  Otherwise, the state transitions to esDIM_IDENTIFIER_AS to possibly
//  dimension the identifier as a dialog variable.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateDIM_IDENTIFIER
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esDIM_IDENTIFIER);

    if (lpstEnv->eToken != eTOKEN_AS)
    {
        ControlDestroyTopControl(lpstEnv);
        StateNew(lpstEnv,esSKIP_LINE);
        RedoToken(lpstEnv);
        return;
    }

    StateNew(lpstEnv,esDIM_IDENTIFIER_AS);
}


//*************************************************************************
//
// void StateDIM_IDENTIFIER_AS()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after a DIM dlg AS sequence of tokens has been
//  seen.  The expected token in this state is a function value
//  indicating the dialog to dimension the variable as.  A dialog
//  variable is created if so.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateDIM_IDENTIFIER_AS
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esDIM_IDENTIFIER_AS);

    if (lpstEnv->eToken == eTOKEN_FUNCTION_VALUE)
    {
        DIALOG_T        stDlg;
        LPVAR           lpstVar;
        LPIDENTIFIER    lpstIdentifier;

        lpstIdentifier = (LPIDENTIFIER)ControlTop(lpstEnv);

        // First see if the variable already exists

        lpstVar = VarGet(lpstEnv,
                         lpstIdentifier->lpbyName[0],
                         lpstIdentifier->lpbyName + 1);

        // Create a dialog record variable

        switch (stDlg.eWDCMD = lpstEnv->uTokenData.eWDCMD)
        {
            case eWDCMD_FileSaveAs:
                stDlg.wSize = sizeof(DLG_FILESAVEAS_T);
                if (lpstVar == NULL)
                {
                    lpstVar = VarAlloc(lpstEnv,
                                       FALSE,
                                       FALSE,
                                       lpstIdentifier->lpbyName[0],
                                       lpstIdentifier->lpbyName + 1);
                }

                if (lpstVar == NULL)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }

                if (VarSet(lpstEnv,
                           lpstVar,
                           eTYPE_DIALOG,
                           &stDlg) == NULL)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
                break;

            case eWDCMD_FileNew:
                stDlg.wSize = sizeof(DLG_FILENEW_T);
                if (lpstVar == NULL)
                {
                    lpstVar = VarAlloc(lpstEnv,
                                       FALSE,
                                       FALSE,
                                       lpstIdentifier->lpbyName[0],
                                       lpstIdentifier->lpbyName + 1);
                }

                if (lpstVar == NULL)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }

                if (VarSet(lpstEnv,
                           lpstVar,
                           eTYPE_DIALOG,
                           &stDlg) == NULL)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
                break;

            default:
                // Allocate a generic dialog
                stDlg.wSize = sizeof(DIALOG_T);
                if (lpstVar == NULL)
                {
                    lpstVar = VarAlloc(lpstEnv,
                                       FALSE,
                                       FALSE,
                                       lpstIdentifier->lpbyName[0],
                                       lpstIdentifier->lpbyName + 1);
                }

                if (lpstVar == NULL)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }

                if (VarSet(lpstEnv,
                           lpstVar,
                           eTYPE_DIALOG,
                           &stDlg) == NULL)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }

                break;
        }
    }

    ControlDestroyTopControl(lpstEnv);
    StateNew(lpstEnv,esSKIP_LINE);
}


//*************************************************************************
//
// void StateGET_CUR_VALUES()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_GET_CUR_VALUES is seen as the
//  first token of a statement.  It initializes the values of the
//  dialog variable.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateGET_CUR_VALUES
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esGET_CUR_VALUES);

    if (lpstEnv->eToken == eTOKEN_IDENTIFIER)
    {
        LPVAR       lpstVar;

        lpstVar = VarGet(lpstEnv,
                         lpstEnv->uTokenData.Identifier.byLen,
                         lpstEnv->uTokenData.Identifier.lpby);

        if (lpstVar != NULL && lpstVar->eType == eTYPE_DIALOG)
        {
            switch (((LPDIALOG)lpstVar->uData.lpvstDlg)->eWDCMD)
            {
                case eWDCMD_FileSaveAs:
                {
                    LPDLG_FILESAVEAS lpstDlg =
                        (LPDLG_FILESAVEAS)lpstVar->uData.lpvstDlg;

                    lpstDlg->byFormat = 0;
                    break;
                }

                case eWDCMD_FileNew:
                {
                    LPDLG_FILENEW lpstDlg =
                        (LPDLG_FILENEW)lpstVar->uData.lpvstDlg;

                    lpstDlg->byNewTemplate = 0;
                    break;
                }

                default:
                    break;
            }
        }
    }

    // pop (esGET_CUR_VALUES)

    StatePop(lpstEnv);
}


//*************************************************************************
//
// void StateEXPR_DIALOG()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when a dialog token is seen in an expression.
//  The state sets up for a dialog function call.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateEXPR_DIALOG
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esEXPR_DIALOG);

    if (lpstEnv->eToken != eTOKEN_OPEN_PAREN)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    StateNew(lpstEnv,esDIALOG_FUNCTION_CALL);
    if (SubStateTop(lpstEnv) != essSKIP)
    {
        if (ControlCreateControl(lpstEnv,ectDIALOG_CALL) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }

    StatePush(lpstEnv,esCALL_GET_PARAM);
}


//*************************************************************************
//
// void StateDIALOG_FUNCTION_CALL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs for a dialog subroutine call.  It ignores the
//  call and just releases resources allocated for the call.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateDIALOG_SUB_CALL
(
    LPENV       lpstEnv
)
{
    LPDIALOG_CALL   lpstDialogCall;

    assert(StateTop(lpstEnv) == esDIALOG_SUB_CALL);

    RedoToken(lpstEnv);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esDIALOG_SUB_CALL)

        StatePop(lpstEnv);
        return;
    }

    lpstDialogCall = (LPDIALOG_CALL)ControlTop(lpstEnv);

    assert(lpstDialogCall->ecType == ectDIALOG_CALL);

    // Call off

    // Delete parameters

    if (ControlFreeParameterList(lpstEnv,
                                 lpstDialogCall->lpstFirstParam) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    lpstDialogCall->lpstFirstParam = NULL;

    // Free control top

    ControlDestroyTopControl(lpstEnv);

    // pop (esDIALOG_SUB_CALL)

    StatePop(lpstEnv);
}


//*************************************************************************
//
// void StateDIALOG_FUNCTION_CALL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs for a dialog function call.  It sets the result
//  of the call to -1 by pushing -1 onto the operand stack.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateDIALOG_FUNCTION_CALL
(
    LPENV       lpstEnv
)
{
    LPDIALOG_CALL   lpstDialogCall;

    assert(StateTop(lpstEnv) == esDIALOG_FUNCTION_CALL);

    RedoToken(lpstEnv);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esDIALOG_FUNCTION_CALL)

        StatePop(lpstEnv);
        return;
    }

    lpstDialogCall = (LPDIALOG_CALL)ControlTop(lpstEnv);

    assert(lpstDialogCall->ecType == ectDIALOG_CALL);

    // Call off

    // Push a -1 onto the operand stack

    if (EvalPushLongOperand(lpstEnv,-1) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Delete parameters

    if (ControlFreeParameterList(lpstEnv,
                                 lpstDialogCall->lpstFirstParam) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    lpstDialogCall->lpstFirstParam = NULL;

    // Free control top

    ControlDestroyTopControl(lpstEnv);

    // pop (esDIALOG_FUNCTION_CALL)

    StatePop(lpstEnv);
}


//*************************************************************************
//
// void StateBEGIN_DIALOG()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_BEGIN is seen in the esSTATEMENT
//  state.  A Begin signals the beginning of a Begin Dialog...End Dialog
//  structure.  This state remains static until an eTOKEN_END is seen
//  in the token stream.  The eTOKEN_END potentially signals the end
//  of the Begin Dialog...End Dialog state.  Thus the state transitions
//  to the esBEGIN_DIALOG_END state to check this condition.  If the
//  esBEGIN_DIALOG_END does not detect the condition, then the state
//  will transition back to this state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateBEGIN_DIALOG
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esBEGIN_DIALOG);

    if (lpstEnv->eToken == eTOKEN_END)
    {
        // Could it be "End Dialog" ?

        StateNew(lpstEnv,esBEGIN_DIALOG_END);
    }
}


//*************************************************************************
//
// void StateBEGIN_DIALOG_END()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_END is seen while in the
//  esBEGIN_DIALOG state.  This state checks to see if the next token
//  is an eTOKEN_DIALOG, signaling the end of the
//  Begin Dialog...End Dialog structure.  If the next token is an
//  eTOKEN_DIALOG, then the state is popped, causing a transition to the
//  next state on the control state stack.  Otherwise, the state
//  transitions back to the esBEGIN_DIALOG state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateBEGIN_DIALOG_END
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esBEGIN_DIALOG_END);

    if (lpstEnv->eToken == eTOKEN_DIALOG)
    {
        // Found the end of the dialog

        StatePop(lpstEnv);
    }
    else
    {
        // Not the end

        StateNew(lpstEnv,esBEGIN_DIALOG);
    }
}


//*************************************************************************
//
// void StateIGNORE_EXPR_RESULT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state is used when the result of an expression evaluation
//  should be ignored.  If the substate is not essSKIP, then the
//  state pops the top operand off of the operand stack.
//
//  The state is popped so that the next state is the next state
//  on the control state stack.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateIGNORE_EXPR_RESULT
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esIGNORE_EXPR_RESULT);

    // Pop top operand value that was assigned

    if (SubStateTop(lpstEnv) != essSKIP)
    {
        if (EvalPopTopOperand(lpstEnv) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }

    // pop (esIGNORE_EXPR_RESULT)

    StatePop(lpstEnv);
}


//*************************************************************************
//
// void StateSKIP_LINE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state eats up tokens until an eTOKEN_NEWLINE or label is seen.
//  The state is used to skip the rest of the tokens on a line.
//  After a newline is seen the state is popped.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSKIP_LINE
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSKIP_LINE);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_NEWLINE:
            // pop (esSKIP_LINE)

            StatePop(lpstEnv);
            break;

        case eTOKEN_LABEL:
        case eTOKEN_WORD_LABEL:
            StatePop(lpstEnv);
            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateERROR()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state sets lpstEnv->eERROR to eERROR_GENERIC.  This is an
//  end state, meaning that there is no transition out of it.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateERROR
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esERROR);

    lpstEnv->eError = eERROR_GENERIC;
}


