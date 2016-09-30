//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/O97STR.H_v   1.0   15 Aug 1997 13:28:40   DCHI  $
//
// Description:
//  Header for O97 strings.
//  Generated using:
//      vbaemstr o97str.lst o97str O97
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/O97STR.H_v  $
// 
//    Rev 1.0   15 Aug 1997 13:28:40   DCHI
// Initial revision.
// 
//************************************************************************

typedef enum tagEO97_STR
{
    eO97_STR_application                         = 0,
    eO97_STR_err                                 = 1,
} EO97_STR_T, FAR *LPEO97_STR;

extern LPBYTE FAR galpbyO97_STR[];

#define NUM_O97_STR             2
