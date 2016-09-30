//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOPD.CPv   1.0   15 Aug 1997 13:29:20   DCHI  $
//
// Description:
//  Handling of opcodes 0xD0-0xDF.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOPD.CPv  $
// 
//    Rev 1.0   15 Aug 1997 13:29:20   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_D0_PRINT_END_NONE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_D0_PRINT_END_NONE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xD0);

//    printf("O97_OP_D0_PRINT_END_NONE\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_D1_PRINT_MID_END_NONE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_D1_PRINT_MID_END_NONE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xD1);

//    printf("O97_OP_D1_PRINT_MID_END_NONE\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_D2_PRINT_SEMICOLON()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_D2_PRINT_SEMICOLON
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xD2);

//    printf("O97_OP_D2_PRINT_SEMICOLON\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_D3_PRINT_SPC_ARG()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_D3_PRINT_SPC_ARG
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xD3);

//    printf("O97_OP_D3_PRINT_SPC_ARG\n");

    // Pop the Spc() argument

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_D4_PRINT_TAB_ARG()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_D4_PRINT_TAB_ARG
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xD4);

//    printf("O97_OP_D4_PRINT_TAB_ARG\n");

    // Pop the Spc() argument

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_D5_PRINT_TAB()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_D5_PRINT_TAB
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xD5);

//    printf("O97_OP_D5_PRINT_TAB\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_D6()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_D6
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xD6);

//    printf("O97_OP_D6\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_D7()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_D7
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xD7);

//    printf("O97_OP_D7\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_D8_COMMENT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_D8_COMMENT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xD8);

//    printf("O97_OP_D8_COMMENT\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_D9_REDIM()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops any array dimension indices.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_D9_REDIM
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xD9);

//    printf("O97_OP_D9_REDIM\n");

    // Pop any waiting dimension indices

    if (O97EvalPopParameters(lpstEnv,
                             lpstEnv->lpstModEnv->wNumIndices) == FALSE)
        return(FALSE);

    lpstEnv->lpstModEnv->wNumIndices = 0;

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_DA_REDIM_PRESERVE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops any array dimension indices.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_DA_REDIM_PRESERVE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xDA);

//    printf("O97_OP_DA_REDIM_PRESERVE\n");

    // Pop any waiting dimension indices

    if (O97EvalPopParameters(lpstEnv,
                             lpstEnv->lpstModEnv->wNumIndices) == FALSE)
        return(FALSE);

    lpstEnv->lpstModEnv->wNumIndices = 0;

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_DB_LITERAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_DB_LITERAL
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xDB);

//    printf("O97_OP_DB_LITERAL\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_DC_REM()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_DC_REM
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xDC);

//    printf("O97_OP_DC_REM\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_DD_RESUME()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_DD_RESUME
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xDD);

//    printf("O97_OP_DD_RESUME\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_DE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_DE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xDE);

//    printf("O97_OP_DE\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_DF()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_DF
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xDF);

//    printf("O97_OP_DF\n");

    return(TRUE);
}


