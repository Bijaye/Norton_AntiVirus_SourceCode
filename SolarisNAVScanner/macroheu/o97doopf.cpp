//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOPF.CPv   1.0   15 Aug 1997 13:29:24   DCHI  $
//
// Description:
//  Handling of opcodes 0xF0-0xFF.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOPF.CPv  $
// 
//    Rev 1.0   15 Aug 1997 13:29:24   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_F0()
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

BOOL O97_OP_F0
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xF0);

//    printf("O97_OP_F0\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_F1()
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

BOOL O97_OP_F1
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xF1);

//    printf("O97_OP_F1\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_F2()
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

BOOL O97_OP_F2
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xF2);

//    printf("O97_OP_F2\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_F3()
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

BOOL O97_OP_F3
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xF3);

//    printf("O97_OP_F3\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_F4()
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

BOOL O97_OP_F4
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xF4);

//    printf("O97_OP_F4\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_F5()
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

BOOL O97_OP_F5
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xF5);

//    printf("O97_OP_F5\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_F6_FOR_NEXT_A()
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

BOOL O97_OP_F6_FOR_NEXT_A
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xF6);

//    printf("O97_OP_F6_FOR_NEXT_A\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_F7_FOR_NEXT()
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

BOOL O97_OP_F7_FOR_NEXT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xF7);

//    printf("O97_OP_F7_FOR_NEXT\n");

    // This just signifies the beginning of a FOR statement

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_F8()
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

BOOL O97_OP_F8
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xF8);

//    printf("O97_OP_F8\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_F9_WITH()
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

BOOL O97_OP_F9_WITH
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xF9);

//    printf("O97_OP_F9_WITH\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_FA()
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

BOOL O97_OP_FA
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xFA);

//    printf("O97_OP_FA\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_FB()
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

BOOL O97_OP_FB
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xFB);

//    printf("O97_OP_FB\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_FC()
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

BOOL O97_OP_FC
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xFC);

//    printf("O97_OP_FC\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_FD()
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

BOOL O97_OP_FD
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xFD);

//    printf("O97_OP_FD\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_FE()
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

BOOL O97_OP_FE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xFE);

//    printf("O97_OP_FE\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_FF()
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

BOOL O97_OP_FF
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xFF);

//    printf("O97_OP_FF\n");

    return(TRUE);
}

