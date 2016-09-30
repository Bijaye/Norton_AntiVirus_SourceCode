#ifdef MCRENG15

  #include "wd7api.h"
  #include "o97api.h"
  #include "wdscan15.h"

#else

  #include "storobj.h"
  #include "wdencdoc.h"
  #include "worddoc.h"
  #include "wdscan.h"

#endif

#include "macrovid.h"

///////////////////////////////////////////////////////////////////////////
//
// Name signatures
//
///////////////////////////////////////////////////////////////////////////

BYTE FAR gabyWD8NameSetOffsetSigs[] =
{
    //--------------------------------------------------
    // 0x0000 : NAME_ONLY_SHARETHEFUN
    // Only SHARETHEFUN

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x54, 0x49, 0x42, 0x53, 0x46, 0x55, 0x49, 
    0x46, 0x47, 0x56, 0x4F, 

    //--------------------------------------------------
    // 0x0001 : NAME_ONLY_FILECLOSEALL
    // Only FILECLOSEALL

    0x03,
    0x00, 0x5F, 0x79, 
    0x0D,
    0x0C, 0x47, 0x4A, 0x4D, 0x46, 0x44, 0x4D, 0x50, 
    0x54, 0x46, 0x42, 0x4D, 0x4D, 

    //--------------------------------------------------
    // 0x0002 : NAME_ONLY_EXIT_
    // Only EXIT_

    0x03,
    0x00, 0x5C, 0x79, 
    0x05,
    0x46, 0x59, 0x4A, 0x55, 0x60, 

    //--------------------------------------------------
    // 0x0003 : NAME_ONLY_STRYXONE
    // Only STRYXONE

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x54, 0x55, 0x53, 0x5A, 0x59, 0x50, 0x4F, 
    0x46, 

    //--------------------------------------------------
    // 0x0004 : NAME_ONLY_STRYXTWO
    // Only STRYXTWO

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x54, 0x55, 0x53, 0x5A, 0x59, 0x55, 0x58, 
    0x50, 

    //--------------------------------------------------
    // 0x0005 : NAME_ONLY_APPDER
    // Only APPDER

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x42, 0x51, 0x51, 0x45, 0x46, 0x53, 

    //--------------------------------------------------
    // 0x0006 : NAME_ONLY_OFFNEN
    // Only ôFFNEN

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0xD7, 0x47, 0x47, 0x4F, 0x46, 0x4F, 

    //--------------------------------------------------
    // 0x0007 : NAME_ONLY_SPEICHERN
    // Only SPEICHERN

    0x03,
    0x00, 0x5F, 0x79, 
    0x0A,
    0x09, 0x54, 0x51, 0x46, 0x4A, 0x44, 0x49, 0x46, 
    0x53, 0x4F, 

    //--------------------------------------------------
    // 0x0008 : NAME_ONLY_DATUMUNDUHRZEIT
    // Only DATUMUNDUHRZEIT

    0x03,
    0x00, 0x5F, 0x79, 
    0x10,
    0x0F, 0x45, 0x42, 0x55, 0x56, 0x4E, 0x56, 0x4F, 
    0x45, 0x56, 0x49, 0x53, 0x5B, 0x46, 0x4A, 0x55, 

    //--------------------------------------------------
    // 0x0009 : NAME_ONLY_DATEIALLESSPEICHERN
    // Only DATEIALLESSPEICHERN

    0x03,
    0x00, 0x5F, 0x79, 
    0x14,
    0x13, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x42, 0x4D, 
    0x4D, 0x46, 0x54, 0x54, 0x51, 0x46, 0x4A, 0x44, 
    0x49, 0x46, 0x53, 0x4F, 

    //--------------------------------------------------
    // 0x000A : NAME_ONLY_NEWFS
    // Only NEWFS

    0x03,
    0x00, 0x5C, 0x79, 
    0x05,
    0x4F, 0x46, 0x58, 0x47, 0x54, 

    //--------------------------------------------------
    // 0x000B : NAME_ONLY_NEWAO
    // Only NEWAO

    0x03,
    0x00, 0x5C, 0x79, 
    0x05,
    0x4F, 0x46, 0x58, 0x42, 0x50, 

    //--------------------------------------------------
    // 0x000C : NAME_ONLY_OFXX
    // Only OFXX

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x50, 0x47, 0x59, 0x59, 

    //--------------------------------------------------
    // 0x000D : NAME_ONLY_CFXX
    // Only CFXX

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x44, 0x47, 0x59, 0x59, 

    //--------------------------------------------------
    // 0x000E : NAME_ONLY_SHOW
    // Only SHOW

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x54, 0x49, 0x50, 0x58, 

    //--------------------------------------------------
    // 0x000F : NAME_ONLY_DOGGIE
    // Only DOGGIE

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x45, 0x50, 0x48, 0x48, 0x4A, 0x46, 

    //--------------------------------------------------
    // 0x0010 : NAME_ONLY_PARADISE
    // Only PARADISE

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x51, 0x42, 0x53, 0x42, 0x45, 0x4A, 0x54, 
    0x46, 

    //--------------------------------------------------
    // 0x0011 : NAME_ONLY_GANGSTERZ
    // Only GANGSTERZ

    0x03,
    0x00, 0x5F, 0x79, 
    0x0A,
    0x09, 0x48, 0x42, 0x4F, 0x48, 0x54, 0x55, 0x46, 
    0x53, 0x5B, 

    //--------------------------------------------------
    // 0x0012 : NAME_ONLY_STRYX1
    // Only STRYX1

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x54, 0x55, 0x53, 0x5A, 0x59, 0x32, 

    //--------------------------------------------------
    // 0x0013 : NAME_ONLY_STRYX2
    // Only STRYX2

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x54, 0x55, 0x53, 0x5A, 0x59, 0x33, 

    //--------------------------------------------------
    // 0x0014 : NAME_ONLY_DOKUMENTSCHLIEBEN
    // Only DOKUMENTSCHLIE·EN

    0x03,
    0x00, 0x5F, 0x79, 
    0x12,
    0x11, 0x45, 0x50, 0x4C, 0x56, 0x4E, 0x46, 0x4F, 
    0x55, 0x54, 0x44, 0x49, 0x4D, 0x4A, 0x46, 0xE0, 
    0x46, 0x4F, 

    //--------------------------------------------------
    // 0x0015 : NAME_ONLY_SPOOKY
    // Only SPOOKY

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x54, 0x51, 0x50, 0x50, 0x4C, 0x5A, 

    //--------------------------------------------------
    // 0x0016 : NAME_ONLY_DATEIOFFNEN
    // Only DATEIîFFNEN

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x45, 0x42, 0x55, 0x46, 0x4A, 0xD7, 0x47, 
    0x47, 0x4F, 0x46, 0x4F, 

    //--------------------------------------------------
    // 0x0017 : NAME_ONLY_DATEIDOKVORLAGEN
    // Only DATEIDOKVORLAGEN

    0x03,
    0x00, 0x5F, 0x79, 
    0x11,
    0x10, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x45, 0x50, 
    0x4C, 0x57, 0x50, 0x53, 0x4D, 0x42, 0x48, 0x46, 
    0x4F, 

    //--------------------------------------------------
    // 0x0018 : NAME_ONLY_FAST
    // Only FAST

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x47, 0x42, 0x54, 0x55, 

    //--------------------------------------------------
    // 0x0019 : NAME_ONLY_TALK
    // Only TALK

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x55, 0x42, 0x4D, 0x4C, 

    //--------------------------------------------------
    // 0x001A : NAME_ONLY_CANCEL
    // Only CANCEL

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x44, 0x42, 0x4F, 0x44, 0x46, 0x4D, 

    //--------------------------------------------------
    // 0x001B : NAME_ONLY_ABBRECHEN
    // Only ABBRECHEN

    0x03,
    0x00, 0x5F, 0x79, 
    0x0A,
    0x09, 0x42, 0x43, 0x43, 0x53, 0x46, 0x44, 0x49, 
    0x46, 0x4F, 

    //--------------------------------------------------
    // 0x001C : NAME_ONLY_INFIZIEREN
    // Only INFIZIEREN

    0x03,
    0x00, 0x5F, 0x79, 
    0x0B,
    0x0A, 0x4A, 0x4F, 0x47, 0x4A, 0x5B, 0x4A, 0x46, 
    0x53, 0x46, 0x4F, 

    //--------------------------------------------------
    // 0x001D : NAME_ONLY_EXTRASMACRO
    // Only EXTRASMACRO

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x46, 0x59, 0x55, 0x53, 0x42, 0x54, 0x4E, 
    0x42, 0x44, 0x53, 0x50, 

    //--------------------------------------------------
    // 0x001E : NAME_ONLY_DATEISCHLIEBEN
    // Only DATEISCHLIE·EN

    0x03,
    0x00, 0x5F, 0x79, 
    0x0F,
    0x0E, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x54, 0x44, 
    0x49, 0x4D, 0x4A, 0x46, 0xE0, 0x46, 0x4F, 

    //--------------------------------------------------
    // 0x001F : NAME_ONLY_LADING
    // Only LADING

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x4D, 0x42, 0x45, 0x4A, 0x4F, 0x48, 

    //--------------------------------------------------
    // 0x0020 : NAME_ONLY_IKWORDNIETGOED1
    // Only IKWORDNIETGOED1

    0x03,
    0x00, 0x5F, 0x79, 
    0x10,
    0x0F, 0x4A, 0x4C, 0x58, 0x50, 0x53, 0x45, 0x4F, 
    0x4A, 0x46, 0x55, 0x48, 0x50, 0x46, 0x45, 0x32, 

    //--------------------------------------------------
    // 0x0021 : NAME_ONLY_IKWORDNIETGOED2
    // Only IKWORDNIETGOED2

    0x03,
    0x00, 0x5F, 0x79, 
    0x10,
    0x0F, 0x4A, 0x4C, 0x58, 0x50, 0x53, 0x45, 0x4F, 
    0x4A, 0x46, 0x55, 0x48, 0x50, 0x46, 0x45, 0x33, 

    //--------------------------------------------------
    // 0x0022 : NAME_LETTER_FOUR_DIGITS
    // Letter Then 4 digits (OutLaw)

    0x0A,
    0x00, 0x23, 0x34, 0x42, 0x23, 0x34, 0x42, 0x23, 
    0x54, 0x79, 
    0x0F,
    0x00, 0x41, 0x5A, 0x00, 0x30, 0x39, 0x00, 0x30, 
    0x39, 0x00, 0x30, 0x39, 0x00, 0x30, 0x39, 

    //--------------------------------------------------
    // 0x0023 : NAME_ONLY_DMV
    // Only DMV

    0x03,
    0x00, 0x5A, 0x79, 
    0x03,
    0x45, 0x4E, 0x57, 

    //--------------------------------------------------
    // 0x0024 : NAME_ONLY_VAUTONEW
    // Only VAUTONEW

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x57, 0x42, 0x56, 0x55, 0x50, 0x4F, 0x46, 
    0x58, 

    //--------------------------------------------------
    // 0x0025 : NAME_ONLY_VFILESAVEAS
    // Only VFILESAVEAS

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x57, 0x47, 0x4A, 0x4D, 0x46, 0x54, 0x42, 
    0x57, 0x46, 0x42, 0x54, 

    //--------------------------------------------------
    // 0x0026 : NAME_ONLY_MICROSOFT03
    // Only MICROSOFT03

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x4E, 0x4A, 0x44, 0x53, 0x50, 0x54, 0x50, 
    0x47, 0x55, 0x31, 0x34, 

    //--------------------------------------------------
    // 0x0027 : NAME_ONLY_MICROSOFT01
    // Only MICROSOFT01

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x4E, 0x4A, 0x44, 0x53, 0x50, 0x54, 0x50, 
    0x47, 0x55, 0x31, 0x32, 

    //--------------------------------------------------
    // 0x0028 : NAME_ONLY_MICROSOFT04
    // Only MICROSOFT04

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x4E, 0x4A, 0x44, 0x53, 0x50, 0x54, 0x50, 
    0x47, 0x55, 0x31, 0x35, 

    //--------------------------------------------------
    // 0x0029 : NAME_ONLY_MICROSOFT05
    // Only MICROSOFT05

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x4E, 0x4A, 0x44, 0x53, 0x50, 0x54, 0x50, 
    0x47, 0x55, 0x31, 0x36, 

    //--------------------------------------------------
    // 0x002A : NAME_ONLY_BURONEU
    // Only BÅRONEU

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x43, 0xDD, 0x53, 0x50, 0x4F, 0x46, 0x56, 

    //--------------------------------------------------
    // 0x002B : NAME_ONLY_DROP
    // Only DROP

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x45, 0x53, 0x50, 0x51, 

    //--------------------------------------------------
    // 0x002C : NAME_ONLY_DROPBAK
    // Only DROPBAK

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x45, 0x53, 0x50, 0x51, 0x43, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x002D : NAME_ONLY_AUTOEXECBAK
    // Only AUTOEXECBAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x42, 0x56, 0x55, 0x50, 0x46, 0x59, 0x46, 
    0x44, 0x43, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x002E : NAME_ONLY_AUTOOPENBAK
    // Only AUTOOPENBAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x42, 0x56, 0x55, 0x50, 0x50, 0x51, 0x46, 
    0x4F, 0x43, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x002F : NAME_ONLY_DATEIOFFNENBAK
    // Only DATEIôFFNENBAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x0F,
    0x0E, 0x45, 0x42, 0x55, 0x46, 0x4A, 0xD7, 0x47, 
    0x47, 0x4F, 0x46, 0x4F, 0x43, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x0030 : NAME_ONLY_EXTRASMAKROBAK
    // Only EXTRASMAKROBAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x0F,
    0x0E, 0x46, 0x59, 0x55, 0x53, 0x42, 0x54, 0x4E, 
    0x42, 0x4C, 0x53, 0x50, 0x43, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x0031 : NAME_ONLY_DATEIBEENDENBAK
    // Only DATEIBEENDENBAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x10,
    0x0F, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x43, 0x46, 
    0x46, 0x4F, 0x45, 0x46, 0x4F, 0x43, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x0032 : NAME_ONLY_DATEIDRUCKENBAK
    // Only DATEIDRUCKENBAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x10,
    0x0F, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x45, 0x53, 
    0x56, 0x44, 0x4C, 0x46, 0x4F, 0x43, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x0033 : NAME_ONLY_DATEISPEICHERNBAK
    // Only DATEISPEICHERNBAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x12,
    0x11, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x54, 0x51, 
    0x46, 0x4A, 0x44, 0x49, 0x46, 0x53, 0x4F, 0x43, 
    0x42, 0x4C, 

    //--------------------------------------------------
    // 0x0034 : NAME_ONLY_DATEISPEICHERNUNTERBAK
    // Only DATEISPEICHERNUNTERBAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x17,
    0x16, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x54, 0x51, 
    0x46, 0x4A, 0x44, 0x49, 0x46, 0x53, 0x4F, 0x56, 
    0x4F, 0x55, 0x46, 0x53, 0x43, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x0035 : NAME_ONLY_DATEIDRUCKENSTANDARDBAK
    // Only DATEIDRUCKENSTANDARDBAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x18,
    0x17, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x45, 0x53, 
    0x56, 0x44, 0x4C, 0x46, 0x4F, 0x54, 0x55, 0x42, 
    0x4F, 0x45, 0x42, 0x53, 0x45, 0x43, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x0036 : NAME_ONLY_DUMMY
    // Only DUMMY

    0x03,
    0x00, 0x5C, 0x79, 
    0x05,
    0x45, 0x56, 0x4E, 0x4E, 0x5A, 

    //--------------------------------------------------
    // 0x0037 : NAME_ONLY_DATEISPEICHERN
    // Only DATEISPEICHERN

    0x03,
    0x00, 0x5F, 0x79, 
    0x0F,
    0x0E, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x54, 0x51, 
    0x46, 0x4A, 0x44, 0x49, 0x46, 0x53, 0x4F, 

    //--------------------------------------------------
    // 0x0038 : NAME_ONLY_DATEIDRUCKENSTANDARD
    // Only DATEIDRUCKENSTANDARD

    0x03,
    0x00, 0x5F, 0x79, 
    0x15,
    0x14, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x45, 0x53, 
    0x56, 0x44, 0x4C, 0x46, 0x4F, 0x54, 0x55, 0x42, 
    0x4F, 0x45, 0x42, 0x53, 0x45, 

    //--------------------------------------------------
    // 0x0039 : NAME_ONLY_DATEISPEICHERNUNTER
    // Only DATEISPEICHERNUNTER

    0x03,
    0x00, 0x5F, 0x79, 
    0x14,
    0x13, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x54, 0x51, 
    0x46, 0x4A, 0x44, 0x49, 0x46, 0x53, 0x4F, 0x56, 
    0x4F, 0x55, 0x46, 0x53, 

    //--------------------------------------------------
    // 0x003A : NAME_ONLY_DATEINEU
    // Only DATEINEU

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x4F, 0x46, 
    0x56, 

    //--------------------------------------------------
    // 0x003B : NAME_ONLY_TELEFONICA
    // Only TELEFONICA

    0x03,
    0x00, 0x5F, 0x79, 
    0x0B,
    0x0A, 0x55, 0x46, 0x4D, 0x46, 0x47, 0x50, 0x4F, 
    0x4A, 0x44, 0x42, 

    //--------------------------------------------------
    // 0x003C : NAME_ONLY_DATEIBEENDEN
    // Only DATEIBEENDEN

    0x03,
    0x00, 0x5F, 0x79, 
    0x0D,
    0x0C, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x43, 0x46, 
    0x46, 0x4F, 0x45, 0x46, 0x4F, 

    //--------------------------------------------------
    // 0x003D : NAME_ONLY_DATEIDRUCKEN
    // Only DATEIDRUCKEN

    0x03,
    0x00, 0x5F, 0x79, 
    0x0D,
    0x0C, 0x45, 0x42, 0x55, 0x46, 0x4A, 0x45, 0x53, 
    0x56, 0x44, 0x4C, 0x46, 0x4F, 

    //--------------------------------------------------
    // 0x003E : NAME_ONLY_WORDHELP
    // Only WORDHELP

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x58, 0x50, 0x53, 0x45, 0x49, 0x46, 0x4D, 
    0x51, 

    //--------------------------------------------------
    // 0x003F : NAME_ONLY_ANTIVIRUS
    // Only ANTIVIRUS

    0x03,
    0x00, 0x5F, 0x79, 
    0x0A,
    0x09, 0x42, 0x4F, 0x55, 0x4A, 0x57, 0x4A, 0x53, 
    0x56, 0x54, 

    //--------------------------------------------------
    // 0x0040 : NAME_ONLY_WORDHELPNT
    // Only WORDHELPNT

    0x03,
    0x00, 0x5F, 0x79, 
    0x0B,
    0x0A, 0x58, 0x50, 0x53, 0x45, 0x49, 0x46, 0x4D, 
    0x51, 0x4F, 0x55, 

    //--------------------------------------------------
    // 0x0041 : NAME_ONLY_INSERTPBREAK
    // Only INSERTPBREAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x0D,
    0x0C, 0x4A, 0x4F, 0x54, 0x46, 0x53, 0x55, 0x51, 
    0x43, 0x53, 0x46, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x0042 : NAME_ONLY_INSERTPAGEBREAK
    // Only INSERTPAGEBREAK

    0x03,
    0x00, 0x5F, 0x79, 
    0x10,
    0x0F, 0x4A, 0x4F, 0x54, 0x46, 0x53, 0x55, 0x51, 
    0x42, 0x48, 0x46, 0x43, 0x53, 0x46, 0x42, 0x4C, 

    //--------------------------------------------------
    // 0x0043 : NAME_ONLY_TOOLSSPELLING
    // Only TOOLSSPELLING

    0x03,
    0x00, 0x5F, 0x79, 
    0x0E,
    0x0D, 0x55, 0x50, 0x50, 0x4D, 0x54, 0x54, 0x51, 
    0x46, 0x4D, 0x4D, 0x4A, 0x4F, 0x48, 

    //--------------------------------------------------
    // 0x0044 : NAME_ONLY_VCLOSE
    // Only VCLOSE

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x57, 0x44, 0x4D, 0x50, 0x54, 0x46, 

    //--------------------------------------------------
    // 0x0045 : NAME_ONLY_VOPEN
    // Only VOPEN

    0x03,
    0x00, 0x5C, 0x79, 
    0x05,
    0x57, 0x50, 0x51, 0x46, 0x4F, 

    //--------------------------------------------------
    // 0x0046 : NAME_ONLY_AUTOEXIT
    // Only AUTOEXIT

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x42, 0x56, 0x55, 0x50, 0x46, 0x59, 0x4A, 
    0x55, 

    //--------------------------------------------------
    // 0x0047 : NAME_ONLY_AUTOCLOSE
    // "AUTOCLOSE...

    0x02,
    0x00, 0x7F, 
    0x0A,
    0x09, 0x42, 0x56, 0x55, 0x50, 0x44, 0x4D, 0x50, 
    0x54, 0x46, 

    //--------------------------------------------------
    // 0x0048 : NAME_ONLY_AUTOEXEC
    // "AUTOEXEC"

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x42, 0x56, 0x55, 0x50, 0x46, 0x59, 0x46, 
    0x44, 

    //--------------------------------------------------
    // 0x0049 : NAME_ONLY_AUTOOPEN
    // "AUTOOPEN"

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x42, 0x56, 0x55, 0x50, 0x50, 0x51, 0x46, 
    0x4F, 

    //--------------------------------------------------
    // 0x004A : NAME_ONLY_FILEEXIT
    // "FILEEXIT"

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x47, 0x4A, 0x4D, 0x46, 0x46, 0x59, 0x4A, 
    0x55, 

    //--------------------------------------------------
    // 0x004B : NAME_ONLY_FILENEW
    // "FILENEW"

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x47, 0x4A, 0x4D, 0x46, 0x4F, 0x46, 0x58, 

    //--------------------------------------------------
    // 0x004C : NAME_ONLY_FILESAVE
    // "FILESAVE"

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x47, 0x4A, 0x4D, 0x46, 0x54, 0x42, 0x57, 
    0x46, 

    //--------------------------------------------------
    // 0x004D : NAME_ONLY_FILESAVEAS
    // "FILESAVEAS...

    0x02,
    0x00, 0x7F, 
    0x0B,
    0x0A, 0x47, 0x4A, 0x4D, 0x46, 0x54, 0x42, 0x57, 
    0x46, 0x42, 0x54, 

    //--------------------------------------------------
    // 0x004E : NAME_ONLY_PAYLOAD
    // "PAYLOAD"

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x51, 0x42, 0x5A, 0x4D, 0x50, 0x42, 0x45, 

    //--------------------------------------------------
    // 0x004F : NAME_ONLY_INSERTPAYLOAD
    // Only INSERTPAYLOAD

    0x03,
    0x00, 0x5F, 0x79, 
    0x0E,
    0x0D, 0x4A, 0x4F, 0x54, 0x46, 0x53, 0x55, 0x51, 
    0x42, 0x5A, 0x4D, 0x50, 0x42, 0x45, 

    //--------------------------------------------------
    // 0x0050 : NAME_ONLY_DROPSURIV
    // Only DROPSURIV

    0x03,
    0x00, 0x5F, 0x79, 
    0x0A,
    0x09, 0x45, 0x53, 0x50, 0x51, 0x54, 0x56, 0x53, 
    0x4A, 0x57, 

    //--------------------------------------------------
    // 0x0051 : NAME_ONLY_TOOLSMACRO
    // "TOOLSMACRO"

    0x03,
    0x00, 0x5F, 0x79, 
    0x0B,
    0x0A, 0x55, 0x50, 0x50, 0x4D, 0x54, 0x4E, 0x42, 
    0x44, 0x53, 0x50, 

    //--------------------------------------------------
    // 0x0052 : NAME_ONLY_AUTONEW
    // Only AUTONEW

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x42, 0x56, 0x55, 0x50, 0x4F, 0x46, 0x58, 

    //--------------------------------------------------
    // 0x0053 : NAME_ONLY_RPAE
    // Only RPAE

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x53, 0x51, 0x42, 0x46, 

    //--------------------------------------------------
    // 0x0054 : NAME_ONLY_RPFS
    // Only RPFS

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x53, 0x51, 0x47, 0x54, 

    //--------------------------------------------------
    // 0x0055 : NAME_ONLY_RPAO
    // Only RPAO

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x53, 0x51, 0x42, 0x50, 

    //--------------------------------------------------
    // 0x0056 : NAME_ONLY_FILEOPEN
    // Only FILEOPEN

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x47, 0x4A, 0x4D, 0x46, 0x50, 0x51, 0x46, 
    0x4F, 

    //--------------------------------------------------
    // 0x0057 : NAME_ONLY_FILEPRINT
    // Only FILEPRINT

    0x03,
    0x00, 0x5F, 0x79, 
    0x0A,
    0x09, 0x47, 0x4A, 0x4D, 0x46, 0x51, 0x53, 0x4A, 
    0x4F, 0x55, 

    //--------------------------------------------------
    // 0x0058 : NAME_ONLY_TOOLSCUSTOMIZE
    // Only TOOLSCUSTOMIZE

    0x03,
    0x00, 0x5F, 0x79, 
    0x0F,
    0x0E, 0x55, 0x50, 0x50, 0x4D, 0x54, 0x44, 0x56, 
    0x54, 0x55, 0x50, 0x4E, 0x4A, 0x5B, 0x46, 

    //--------------------------------------------------
    // 0x0059 : NAME_ONLY_PRESENTW
    // Only PRESENTW

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x51, 0x53, 0x46, 0x54, 0x46, 0x4F, 0x55, 
    0x58, 

    //--------------------------------------------------
    // 0x005A : NAME_ONLY_PRESENTZ
    // Only PRESENTZ

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x51, 0x53, 0x46, 0x54, 0x46, 0x4F, 0x55, 
    0x5B, 

    //--------------------------------------------------
    // 0x005B : NAME_ONLY_VGOJOHNNY
    // Only VGOJOHNNY

    0x03,
    0x00, 0x5F, 0x79, 
    0x0A,
    0x09, 0x57, 0x48, 0x50, 0x4B, 0x50, 0x49, 0x4F, 
    0x4F, 0x5A, 

    //--------------------------------------------------
    // 0x005C : NAME_ONLY_FILECLOSE
    // Only FILECLOSE

    0x03,
    0x00, 0x5F, 0x79, 
    0x0A,
    0x09, 0x47, 0x4A, 0x4D, 0x46, 0x44, 0x4D, 0x50, 
    0x54, 0x46, 

    //--------------------------------------------------
    // 0x005D : NAME_ONLY_FCFINISH
    // Only FCFINISH

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x47, 0x44, 0x47, 0x4A, 0x4F, 0x4A, 0x54, 
    0x49, 

    //--------------------------------------------------
    // 0x005E : NAME_ONLY_AOPNFINISH
    // Only AOPNFINISH

    0x03,
    0x00, 0x5F, 0x79, 
    0x0B,
    0x0A, 0x42, 0x50, 0x51, 0x4F, 0x47, 0x4A, 0x4F, 
    0x4A, 0x54, 0x49, 

    //--------------------------------------------------
    // 0x005F : NAME_ONLY_RPFO
    // Only RPFO

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x53, 0x51, 0x47, 0x50, 

    //--------------------------------------------------
    // 0x0060 : NAME_ONLY_RPFSA
    // Only RPFSA

    0x03,
    0x00, 0x5C, 0x79, 
    0x05,
    0x53, 0x51, 0x47, 0x54, 0x42, 

    //--------------------------------------------------
    // 0x0061 : NAME_ONLY_MCAFEE1
    // Only MCAFEE1

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x4E, 0x44, 0x42, 0x47, 0x46, 0x46, 0x32, 

    //--------------------------------------------------
    // 0x0062 : NAME_ONLY_FILEPRINTDEFAULT
    // Only FILEPRINTDEFAULT

    0x03,
    0x00, 0x5F, 0x79, 
    0x11,
    0x10, 0x47, 0x4A, 0x4D, 0x46, 0x51, 0x53, 0x4A, 
    0x4F, 0x55, 0x45, 0x46, 0x47, 0x42, 0x56, 0x4D, 
    0x55, 

    //--------------------------------------------------
    // 0x0063 : NAME_ONLY_EXTRASMAKRO
    // Only EXTRASMAKRO

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x46, 0x59, 0x55, 0x53, 0x42, 0x54, 0x4E, 
    0x42, 0x4C, 0x53, 0x50, 

    //--------------------------------------------------
    // 0x0064 : NAME_ONLY_LONERAIDER
    // Only LONERAIDER

    0x03,
    0x00, 0x5F, 0x79, 
    0x0B,
    0x0A, 0x4D, 0x50, 0x4F, 0x46, 0x53, 0x42, 0x4A, 
    0x45, 0x46, 0x53, 

    //--------------------------------------------------
    // 0x0065 : NAME_ONLY_NF
    // Only NF

    0x03,
    0x00, 0x59, 0x79, 
    0x02,
    0x4F, 0x47, 

    //--------------------------------------------------
    // 0x0066 : NAME_ONLY_DARKSIDE1
    // Only DARKSIDE1

    0x03,
    0x00, 0x5F, 0x79, 
    0x0A,
    0x09, 0x45, 0x42, 0x53, 0x4C, 0x54, 0x4A, 0x45, 
    0x46, 0x32, 

    //--------------------------------------------------
    // 0x0067 : NAME_ONLY_HERRAMMACRO
    // Only HERRAMMACRO

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x49, 0x46, 0x53, 0x53, 0x42, 0x4E, 0x4E, 
    0x42, 0x44, 0x53, 0x50, 

    //--------------------------------------------------
    // 0x0068 : NAME_ONLY_FILETEMPLATES
    // Only FILETEMPLATES

    0x03,
    0x00, 0x5F, 0x79, 
    0x0E,
    0x0D, 0x47, 0x4A, 0x4D, 0x46, 0x55, 0x46, 0x4E, 
    0x51, 0x4D, 0x42, 0x55, 0x46, 0x54, 

    //--------------------------------------------------
    // 0x0069 : NAME_ONLY_TIMER
    // Only TIMER

    0x03,
    0x00, 0x5C, 0x79, 
    0x05,
    0x55, 0x4A, 0x4E, 0x46, 0x53, 

    //--------------------------------------------------
    // 0x006A : NAME_ONLY_TIMER_
    // Only TIMER_

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x55, 0x4A, 0x4E, 0x46, 0x53, 0x60, 

    //--------------------------------------------------
    // 0x006B : NAME_ONLY_MSRUN
    // Only MSRUN

    0x03,
    0x00, 0x5C, 0x79, 
    0x05,
    0x4E, 0x54, 0x53, 0x56, 0x4F, 

    //--------------------------------------------------
    // 0x006C : NAME_ONLY_DNZ
    // Only DNZ

    0x03,
    0x00, 0x5A, 0x79, 
    0x03,
    0x45, 0x4F, 0x5B, 

    //--------------------------------------------------
    // 0x006D : NAME_ONLY_EDITCUT
    // Only EDITCUT

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x46, 0x45, 0x4A, 0x55, 0x44, 0x56, 0x55, 

    //--------------------------------------------------
    // 0x006E : NAME_ONLY_EDITCOPY
    // Only EDITCOPY

    0x03,
    0x00, 0x5F, 0x79, 
    0x09,
    0x08, 0x46, 0x45, 0x4A, 0x55, 0x44, 0x50, 0x51, 
    0x5A, 

    //--------------------------------------------------
    // 0x006F : NAME_ONLY_CAP
    // Only CAP (from WM.Social/Cap.A)

    0x03,
    0x00, 0x5A, 0x79, 
    0x03,
    0x44, 0x42, 0x51, 

    //--------------------------------------------------
    // 0x0070 : NAME_ONLY_NIGHTSHADE
    // Only NIGHTSHADE

    0x03,
    0x00, 0x5F, 0x79, 
    0x0B,
    0x0A, 0x4F, 0x4A, 0x48, 0x49, 0x55, 0x54, 0x49, 
    0x42, 0x45, 0x46, 

    //--------------------------------------------------
    // 0x0071 : NAME_ONLY_PLONG
    // Only PLONG

    0x03,
    0x00, 0x5C, 0x79, 
    0x05,
    0x51, 0x4D, 0x50, 0x4F, 0x48, 

    //--------------------------------------------------
    // 0x0072 : NAME_ONLY_FRENZY
    // Only FRENZY

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x47, 0x53, 0x46, 0x4F, 0x5B, 0x5A, 

    //--------------------------------------------------
    // 0x0073 : NAME_ONLY_USERFORM999
    // Only USERFORM999

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x56, 0x54, 0x46, 0x53, 0x47, 0x50, 0x53, 
    0x4E, 0x3A, 0x3A, 0x3A, 

    //--------------------------------------------------
    // 0x0074 : NAME_ONLY_SPARKLE
    // Only SPARKLE

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x54, 0x51, 0x42, 0x53, 0x4C, 0x4D, 0x46, 

    //--------------------------------------------------
    // 0x0075 : NAME_ONLY_USERFORM900
    // Only USERFORM900

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x56, 0x54, 0x46, 0x53, 0x47, 0x50, 0x53, 
    0x4E, 0x3A, 0x31, 0x31, 

    0x00
};

BYTE FAR gabyWD8NameEndOffsetSigs[] =
{
    0x00
};

WD_SIG_INFO_T FAR gastWD8RawNameSigIndex[] =
{
    { 0, NULL }
};

BYTE FAR gabyWD8NameSigRawByteHit[256] =
{
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x00
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x08
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x10
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x18
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x20
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x28
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x30
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x38
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x40
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x48
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x50
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x58
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x60
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x68
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x70
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x78
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x80
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x88
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x90
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x98
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xA0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xA8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xB0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xB8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xC0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xC8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xD0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xD8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xE0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xE8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xF0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xF8
};

///////////////////////////////////////////////////////////////////////////
//
// Named CRC signatures
//
///////////////////////////////////////////////////////////////////////////

BYTE FAR gabyWD8NamedCRC0x00000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x0046F610 Sigs

    //--------------------------------------------------
    // 0x0000 : CRC_DATEISPEICHERNUNTER_SPOOKY
    // WM.Spooky:De: DATEISPEICHERNUNTER

    0x01,
    0x45, 
    0x01,
    0x39, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x00744F85 Sigs

    //--------------------------------------------------
    // 0x0001 : CRC_AUTOOPEN_NUCLEARE
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x0074A804 Sigs

    //--------------------------------------------------
    // 0x0002 : CRC_AUTOCLOSE_SATANIC
    // WM.Satanic: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x02000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x0215B17F Sigs

    //--------------------------------------------------
    // 0x0003 : CRC_AUTOEXEC_SWITCHES
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x03000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x0328F2CE Sigs

    //--------------------------------------------------
    // 0x0004 : CRC_FILESAVE_SHAREFUNA
    // WM.ShareFun.A: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x03C1E4B9 Sigs

    //--------------------------------------------------
    // 0x0005 : CRC_FILESAVE_MUCKA
    // WM.Muck.A: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x04000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x04043AFF Sigs

    //--------------------------------------------------
    // 0x0006 : CRC_AUTOCLOSE_DIVINAB
    // WM.Divina.B: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x05000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x05BC7AC4 Sigs

    //--------------------------------------------------
    // 0x0007 : CRC_AUTOCLOSE_MDMAB
    // WM.MDMA.B: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x06000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x06330781 Sigs

    //--------------------------------------------------
    // 0x0008 : CRC_FILESAVEAS_RAPIN
    // WM.Rapi.N: FILESAVEAS	WD7 same as Rapi.A

    0x01,
    0x45, 
    0x01,
    0x4D, 

    //--------------------------------------------------
    // 0x0009 : CRC_FILESAVEAS_RAPID
    // WM.Rapi.D: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    //--------------------------------------------------
    // 0x000A : CRC_RPFSA_RAPIA
    // WM.Rapi.A: RPFSA

    0x01,
    0x45, 
    0x01,
    0x60, 

    //--------------------------------------------------
    // 0x000B : CRC_FILESAVEAS_RAPIA
    // WM.Rapi.A: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x06B98B39 Sigs

    //--------------------------------------------------
    // 0x000C : CRC_FILESAVEAS_TEDIOUS
    // WM.Tedious: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    //--------------------------------------------------
    // 0x000D : CRC_VFILESAVEAS2_TEDIOUS
    // WM.Tedious: VFILESAVEAS2

    0x01,
    0x45, 
    0x01,
    0x25, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x09000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x09BC7CA5 Sigs

    //--------------------------------------------------
    // 0x000E : CRC_PARADISE_GANGSTERZ
    // WM.Gangsterz: PARADISE

    0x01,
    0x45, 
    0x01,
    0x10, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x0B000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x0B458673 Sigs

    //--------------------------------------------------
    // 0x000F : CRC_TOOLSMACRO_SURABAYAA
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x51, 

    //--------------------------------------------------
    // 0x0010 : CRC_FILETEMPLATES_SURABAYAA
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x68, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x0B537BF4 Sigs

    //--------------------------------------------------
    // 0x0011 : CRC_DATEIDRUCKEN_SPOOKY
    // WM.Spooky:De: DATEIDRUCKEN

    0x01,
    0x45, 
    0x01,
    0x3D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x0E000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x0E32D985 Sigs

    //--------------------------------------------------
    // 0x0012 : CRC_AUTOEXIT_GOODNIGHT
    // WM.GoodNight: AUTOEXIT

    0x01,
    0x45, 
    0x01,
    0x46, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x0EA7C757 Sigs

    //--------------------------------------------------
    // 0x0013 : CRC_AUTOEXEC_RAPIF
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x0EBB1156 Sigs

    //--------------------------------------------------
    // 0x0014 : CRC_FILESAVE_LUNCHA
    // WM.Lunch.A: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x0F000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x0F65DFC6 Sigs

    //--------------------------------------------------
    // 0x0015 : CRC_CAP_CAPD
    // WM.Cap.D: CAP

    0x01,
    0x45, 
    0x01,
    0x6F, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x0F93CBFD Sigs

    //--------------------------------------------------
    // 0x0016 : CRC_AUTOEXEC_SPOOKY
    // WM.Spooky:De: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x0FD040FC Sigs

    //--------------------------------------------------
    // 0x0017 : CRC_AUTOEXEC_SHAREFUNA
    // WM.ShareFun.A: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x10000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x10E6D600 Sigs

    //--------------------------------------------------
    // 0x0018 : CRC_FILECLOSE_POLITE
    // WM.Polite: FILECLOSE

    0x01,
    0x45, 
    0x01,
    0x5C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x12000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x1210550D Sigs

    //--------------------------------------------------
    // 0x0019 : CRC_ABBRECHEN_FRIENDLY
    // WM.Friendly:De: ABBRECHEN

    0x01,
    0x45, 
    0x01,
    0x1B, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x13000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x13786871 Sigs

    //--------------------------------------------------
    // 0x001A : CRC_TELEFONICA_TELE
    // WM.Tele:De: TELEFONICA

    0x01,
    0x45, 
    0x01,
    0x3B, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x14000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x148C1E87 Sigs

    //--------------------------------------------------
    // 0x001B : CRC_FILENEW_FRIENDLY
    // WM.Friendly:De: FILENEW

    0x01,
    0x45, 
    0x01,
    0x4B, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x15000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x15165FA7 Sigs

    //--------------------------------------------------
    // 0x001C : CRC_AUTOEXEC_HASSLE
    // WM.Hassle: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x157871C9 Sigs

    //--------------------------------------------------
    // 0x001D : CRC_AUTOOPEN_LUNCHA
    // WM.Lunch.A: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x17000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x17028C90 Sigs

    //--------------------------------------------------
    // 0x001E : CRC_MICROSOFT03_HASSLE
    // WM.Hassle: MICROSOFT03

    0x01,
    0x45, 
    0x01,
    0x26, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x172A3A16 Sigs

    //--------------------------------------------------
    // 0x001F : CRC_FILEOPEN_FRIENDLY
    // WM.Friendly:De: FILEOPEN

    0x01,
    0x45, 
    0x01,
    0x56, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x17B48CD9 Sigs

    //--------------------------------------------------
    // 0x0020 : CRC_FILESAVE_RAPID
    // WM.Rapi.D: FILESAVE diff CRCWD8

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x18000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x18958CE9 Sigs

    //--------------------------------------------------
    // 0x0021 : CRC_DATEIOFFNEN_SPOOKY
    // WM.Spooky:De: DATEIîFFNEN

    0x01,
    0x45, 
    0x01,
    0x16, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x19000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x196767D1 Sigs

    //--------------------------------------------------
    // 0x0022 : CRC_WORDHELPNT_IRISH
    // WM.Irish: WORDHELPNT

    0x01,
    0x45, 
    0x01,
    0x40, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x1A000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x1A20CD84 Sigs

    //--------------------------------------------------
    // 0x0023 : CRC_AUTOEXEC_BANDUNGM
    // WM.Bandung.M: AutoExec

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x1AD04632 Sigs

    //--------------------------------------------------
    // 0x0024 : CRC_FILESAVE_FRIENDLY
    // WM.Friendly:De: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x1B000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x1B1E0DA3 Sigs

    //--------------------------------------------------
    // 0x0025 : CRC_AUTOCLOSE_CAPA
    // WM.Cap.A: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x1BE90DC4 Sigs

    //--------------------------------------------------
    // 0x0026 : CRC_FILESAVE2_ANTICONCEPT
    // diff CRC for WD8

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x1C000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x1C3DE162 Sigs

    //--------------------------------------------------
    // 0x0027 : CRC_AUTOCLOSE_DARKA
    // WM.Dark.A: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x1C7E2053 Sigs

    //--------------------------------------------------
    // 0x0028 : CRC_FILESAVEAS2_TEDIOUS
    // WM.Tedious: FILESAVEAS2

    0x01,
    0x45, 
    0x01,
    0x4D, 

    //--------------------------------------------------
    // 0x0029 : CRC_VFILESAVEAS_TEDIOUS
    // WM.Tedious: VFILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x25, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x1CEEB062 Sigs

    //--------------------------------------------------
    // 0x002A : CRC_RPAO_RAPIB
    // Diff. CRCWD8 from .A

    0x01,
    0x45, 
    0x01,
    0x55, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x1E000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x1E25C00B Sigs

    //--------------------------------------------------
    // 0x002B : CRC_DATEISPEICHERN_SAVER
    // WM.Saver:De: DATEISPEICHERN

    0x01,
    0x45, 
    0x01,
    0x37, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x1F000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x1F82C663 Sigs

    //--------------------------------------------------
    // 0x002C : CRC_FILESAVEAS_NUCLEARB
    // WM.Nuclear.B: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x1FA89B22 Sigs

    //--------------------------------------------------
    // 0x002D : CRC_AUTOOPEN_GOODNIGHT
    // WM.GoodNight: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x20000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x2087F0D4 Sigs

    //--------------------------------------------------
    // 0x002E : CRC_AUTOOPEN_LUNCHB
    // WM.Lunch.B: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    //--------------------------------------------------
    // 0x002F : CRC_NEWAO_LUNCHB
    // WM.Lunch.B: NEWAO

    0x01,
    0x45, 
    0x01,
    0x0B, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x21000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x21B276D8 Sigs

    //--------------------------------------------------
    // 0x0030 : CRC_AUTOOPEN_RAPIB
    // Diff. CRCWD8 from .A

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x21FD466F Sigs

    //--------------------------------------------------
    // 0x0031 : CRC_FILEEXIT_NUCLEARA
    // WM.Nuclear.A: FILEEXIT

    0x01,
    0x45, 
    0x01,
    0x4A, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x22000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x224337BC Sigs

    //--------------------------------------------------
    // 0x0032 : CRC_FILECLOSEALL_GOODNIGHT
    // WM.GoodNight: FILECLOSEALL

    0x01,
    0x45, 
    0x01,
    0x01, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x22C1BFFB Sigs

    //--------------------------------------------------
    // 0x0033 : CRC_EXTRASMAKRO_SPOOKY
    // WM.Spooky:De: EXTRASMAKRO

    0x01,
    0x45, 
    0x01,
    0x63, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x23000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x237C8A14 Sigs

    //--------------------------------------------------
    // 0x0034 : CRC_DATEISPEICHERNUNTER_BOOM
    // WM.Boom:De: DATEISPEICHERNUNTER

    0x01,
    0x45, 
    0x01,
    0x39, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x24000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x2404ABAB Sigs

    //--------------------------------------------------
    // 0x0035 : CRC_AUTONEW_SATANIC
    // WM.Satanic: AUTONEW

    0x01,
    0x45, 
    0x01,
    0x52, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x245B764F Sigs

    //--------------------------------------------------
    // 0x0036 : CRC_AUTOOPEN_RAPID
    // WM.Rapi.D: AUTOOPEN diff CRCWD8 from A

    0x01,
    0x45, 
    0x01,
    0x49, 

    //--------------------------------------------------
    // 0x0037 : CRC_RPAO_RAPIM
    // WM.Rapi.M: RPAO  diff CRCWD8 from A

    0x01,
    0x45, 
    0x01,
    0x55, 

    //--------------------------------------------------
    // 0x0038 : CRC_AUTOOPEN_RAPIM
    // WM.Rapi.M: autoopen  diff CRCWD8 from A

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x26000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x2647411A Sigs

    //--------------------------------------------------
    // 0x0039 : CRC_AUTOOPEN_CAPA
    // WM.Cap.A: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x26D83252 Sigs

    //--------------------------------------------------
    // 0x003A : CRC_DATEISPEICHERN_FRIENDLY
    // WM.Friendly:De: DATEISPEICHERN

    0x01,
    0x45, 
    0x01,
    0x37, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x27000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x272B5519 Sigs

    //--------------------------------------------------
    // 0x003B : CRC_AUTOEXEC_CAPA
    // WM.Cap.A: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x28000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x281B1454 Sigs

    //--------------------------------------------------
    // 0x003C : CRC_EXTRASMAKRO_FRIENDLY
    // WM.Friendly:De: EXTRASMAKRO

    0x01,
    0x45, 
    0x01,
    0x63, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x2A000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x2A324BE3 Sigs

    //--------------------------------------------------
    // 0x003D : CRC_DATEINEU_TELE
    // WM.Tele:De: DATEINEU

    0x01,
    0x45, 
    0x01,
    0x3A, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x2B000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x2B2B7235 Sigs

    //--------------------------------------------------
    // 0x003E : CRC_AUTOEXIT_SMILEYA
    // WM.Smiley.A:De: AUTOEXIT

    0x01,
    0x45, 
    0x01,
    0x46, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x2C000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x2C5A32DB Sigs

    //--------------------------------------------------
    // 0x003F : CRC_OUTLAWA3
    // WM.Outlaw.A

    0x01,
    0x45, 
    0x01,
    0x22, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x2D000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x2D14CB4C Sigs

    //--------------------------------------------------
    // 0x0040 : CRC_FILESAVEAS_IMPOSTERA
    // WM.Imposter.A: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x2D20F031 Sigs

    //--------------------------------------------------
    // 0x0041 : CRC_AUTOOPEN_WAZZUA
    // WM.Wazzu.A: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x31000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x3159C164 Sigs

    //--------------------------------------------------
    // 0x0042 : CRC_WORDHELP_IRISH
    // WM.Irish: WORDHELP

    0x01,
    0x45, 
    0x01,
    0x3E, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x32000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x325619D5 Sigs

    //--------------------------------------------------
    // 0x0043 : CRC_RPAE_RAPIE
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x53, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x326EFB88 Sigs

    //--------------------------------------------------
    // 0x0044 : CRC_FILESAVEAS_DOGGIE
    // WM.Doggie: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x34000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x343FAEC5 Sigs

    //--------------------------------------------------
    // 0x0045 : CRC_AUTOCLOSE_SHOWOFF
    // WM.ShowOff: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x347D1E28 Sigs

    //--------------------------------------------------
    // 0x0046 : CRC_AUTOOPEN_DZTA
    // WM.Dzt.A: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x3499C680 Sigs

    //--------------------------------------------------
    // 0x0047 : CRC_FILESAVE_LUNCHB
    // WM.Lunch.B: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    //--------------------------------------------------
    // 0x0048 : CRC_NEWFS_LUNCHB
    // WM.Lunch.B: NEWFS

    0x01,
    0x45, 
    0x01,
    0x0A, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x35000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x35246CF0 Sigs

    //--------------------------------------------------
    // 0x0049 : CRC_NIGHTSHADE2_NIGHTSHADEA
    // W97M.NightShade.A: NIGHTSHADE2 Has a Uppercase I in text

    0x01,
    0x45, 
    0x01,
    0x70, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x356858D7 Sigs

    //--------------------------------------------------
    // 0x004A : CRC_TOOLSMACRO_DARKA
    // WM.Dark.A: TOOLSMACRO

    0x01,
    0x45, 
    0x01,
    0x51, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x35845FC4 Sigs

    //--------------------------------------------------
    // 0x004B : CRC_OUTLAWB1
    // WM.Outlaw.B

    0x01,
    0x45, 
    0x01,
    0x22, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x35D1B21B Sigs

    //--------------------------------------------------
    // 0x004C : CRC_DARKSIDE1_DARKA
    // WM.Dark.A: DARKSIDE1

    0x01,
    0x45, 
    0x01,
    0x66, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x37000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x37426D3D Sigs

    //--------------------------------------------------
    // 0x004D : CRC_AUTOOPEN_NPADAC
    // NPad.AC: AutoOpen

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x37C58617 Sigs

    //--------------------------------------------------
    // 0x004E : CRC_VAUTONEW2_TEDIOUS
    // WM.Tedious: VAUTONEW2

    0x01,
    0x45, 
    0x01,
    0x24, 

    //--------------------------------------------------
    // 0x004F : CRC_AUTONEW_TEDIOUS
    // WM.Tedious: AUTONEW

    0x01,
    0x45, 
    0x01,
    0x52, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x38000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x38885F16 Sigs

    //--------------------------------------------------
    // 0x0050 : CRC_AUTOOPEN_KILLDLL
    // WM.KillDLL: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x39000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x39137649 Sigs

    //--------------------------------------------------
    // 0x0051 : CRC_FILESAVEAS_GOODNIGHT
    // WM.GoodNight: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x396BE61E Sigs

    //--------------------------------------------------
    // 0x0052 : CRC_AUTOOPEN_NICEDAY
    // WM.NiceDay: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    //--------------------------------------------------
    // 0x0053 : CRC_AUTOOPEN_NICEDAYG
    // NiceDay.G: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    //--------------------------------------------------
    // 0x0054 : CRC_VOPEN_NICEDAY
    // WM.NiceDay: VOPEN

    0x01,
    0x45, 
    0x01,
    0x45, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x39B61B07 Sigs

    //--------------------------------------------------
    // 0x0055 : CRC_RPFO_RAPIA
    // WM.Rapi.A: RPFO

    0x01,
    0x45, 
    0x01,
    0x5F, 

    //--------------------------------------------------
    // 0x0056 : CRC_FILEOPEN_RAPIA
    // WM.Rapi.A: FILEOPEN

    0x01,
    0x45, 
    0x01,
    0x56, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x3C000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x3CBEC272 Sigs

    //--------------------------------------------------
    // 0x0057 : CRC_AUTOOPEN_PHANTOM
    // WM.Phantom: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x3CC627F9 Sigs

    //--------------------------------------------------
    // 0x0058 : CRC_FILESAVEAS_HYBRID
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x40000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x4082B807 Sigs

    //--------------------------------------------------
    // 0x0059 : CRC_DATEINEU_FRIENDLY
    // WM.Friendly:De: DATEINEU

    0x01,
    0x45, 
    0x01,
    0x3A, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x40851923 Sigs

    //--------------------------------------------------
    // 0x005A : CRC_DATEISPEICHERNUNTER_BIRTHDAY
    // WM.Birthday:De: DATEISPEICHERNUNTER

    0x01,
    0x45, 
    0x01,
    0x39, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x41000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x41670A52 Sigs

    //--------------------------------------------------
    // 0x005B : CRC_FILESAVEAS_SURABAYAA
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x41A05500 Sigs

    //--------------------------------------------------
    // 0x005C : CRC_AUTOOPEN_APPDER
    // WM.Appder: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x41C83079 Sigs

    //--------------------------------------------------
    // 0x005D : CRC_DOKUMENTSCHLIEBEN_STRYX
    // WM.Stryx:De: DOKUMENTSCHLIE·EN

    0x01,
    0x45, 
    0x01,
    0x14, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x42000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x4268723C Sigs

    //--------------------------------------------------
    // 0x005E : CRC_INSERTPAGEBREAK_HOT
    // WM.Hot: INSERTPAGEBREAK

    0x01,
    0x45, 
    0x01,
    0x42, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x43000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x430B531A Sigs

    //--------------------------------------------------
    // 0x005F : CRC_VGOJOHNNY_JOHNNY
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x5B, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x430CA51B Sigs

    //--------------------------------------------------
    // 0x0060 : CRC_AUTOEXIT_XENIXOS
    // WM.Xenixos:De: AUTOEXIT

    0x01,
    0x45, 
    0x01,
    0x46, 

    //--------------------------------------------------
    // 0x0061 : CRC_AUTONEW_XENIXOS
    // WM.Xenixos:De: AUTONEW

    0x01,
    0x45, 
    0x01,
    0x52, 

    //--------------------------------------------------
    // 0x0062 : CRC_AUTOCLOSE_XENIXOS
    // WM.Xenixos:De: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    //--------------------------------------------------
    // 0x0063 : CRC_DUMMY_XENIXOS
    // WM.Xenixos:De: DUMMY

    0x01,
    0x45, 
    0x01,
    0x36, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x435A5D4A Sigs

    //--------------------------------------------------
    // 0x0064 : CRC_DATEIBEENDEN_XENIXOS
    // WM.Xenixos:De: DATEIBEENDEN

    0x01,
    0x45, 
    0x01,
    0x3C, 

    //--------------------------------------------------
    // 0x0065 : CRC_DATEIBEENDENBAK_XENIXOS
    // WM.Xenixos:De: DATEIBEENDENBAK

    0x01,
    0x45, 
    0x01,
    0x31, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x43D94616 Sigs

    //--------------------------------------------------
    // 0x0066 : CRC_FILESAVEAS_TWOLINESB
    // WM.TwoLines.B: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x46000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x4615A075 Sigs

    //--------------------------------------------------
    // 0x0067 : CRC_AUTOOPEN_WAZZUH
    // WM.Wazzu.H: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x463C3A23 Sigs

    //--------------------------------------------------
    // 0x0068 : CRC_AUTOEXEC_SMILEYA
    // WM.Smiley.A:De: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x47000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x4781CD20 Sigs

    //--------------------------------------------------
    // 0x0069 : CRC_FILETEMPLATES_CAPA
    // WM.Cap.A: FILETEMPLATES

    0x01,
    0x45, 
    0x01,
    0x68, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x48000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x48FD1260 Sigs

    //--------------------------------------------------
    // 0x006A : CRC_FILESAVE_GOODNIGHT
    // WM.GoodNight: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x49000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x4937D907 Sigs

    //--------------------------------------------------
    // 0x006B : CRC_AUTOCLOSE_HASSLE
    // WM.Hassle: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x49DD12D5 Sigs

    //--------------------------------------------------
    // 0x006C : CRC_USERFORM999_SPARKLEA
    // Named CRC for USERFORM999

    0x01,
    0x45, 
    0x01,
    0x73, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x49DDD82F Sigs

    //--------------------------------------------------
    // 0x006D : CRC_AUTOCLOSE_HELLGATE
    // WM.Hellgate: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x4A000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x4AAA431F Sigs

    //--------------------------------------------------
    // 0x006E : CRC_AOPNFINISH_MADDOGB
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x5E, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x4AFC2D9D Sigs

    //--------------------------------------------------
    // 0x006F : CRC_TOOLSMACRO_SHAREFUNA
    // WM.ShareFun.A: TOOLSMACRO

    0x01,
    0x45, 
    0x01,
    0x51, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x4B000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x4B5BD6DE Sigs

    //--------------------------------------------------
    // 0x0070 : CRC_DMV2_IMPOSTERA
    // WM.Imposter.A: DMV2 (Imposter.A Templates)

    0x01,
    0x45, 
    0x01,
    0x23, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x4C000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x4C0DE0F7 Sigs

    //--------------------------------------------------
    // 0x0071 : CRC_GANGSTERZ_GANGSTERZ
    // WM.Gangsterz: GANGSTERZ

    0x01,
    0x45, 
    0x01,
    0x11, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x4C2456D9 Sigs

    //--------------------------------------------------
    // 0x0072 : CRC_USERFORM900_SPARKLEA
    // Named CRC for USERFORM900

    0x01,
    0x45, 
    0x01,
    0x75, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x4D000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x4D925A10 Sigs

    //--------------------------------------------------
    // 0x0073 : CRC_AUTOEXEC_RAPIO
    // WM.Rapi.O: AutoExec

    0x01,
    0x45, 
    0x01,
    0x48, 

    //--------------------------------------------------
    // 0x0074 : CRC_RPAE_RAPIO
    // WM.Rapi.O: RpAE

    0x01,
    0x45, 
    0x01,
    0x53, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x4DEE7C12 Sigs

    //--------------------------------------------------
    // 0x0075 : CRC_FILEEXIT_NUCLEARE
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x4A, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x4F000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x4F21D215 Sigs

    //--------------------------------------------------
    // 0x0076 : CRC_AUTOOPEN_MADDOGA
    // WM.MadDog.A: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x4F2CFE3C Sigs

    //--------------------------------------------------
    // 0x0077 : CRC_DATEIDRUCKENSTANDARD_SMILEYBDE
    // WM.Smiley.B:De: DATEIDRUCKENSTANDARD

    0x01,
    0x45, 
    0x01,
    0x38, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x4FF00686 Sigs

    //--------------------------------------------------
    // 0x0078 : CRC_FILESAVEAS_ANTICONCEPT
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x51000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x517A855C Sigs

    //--------------------------------------------------
    // 0x0079 : CRC_DATEISPEICHERN_SMILEYBDE
    // WM.Smiley.B:De: DATEISPEICHERN

    0x01,
    0x45, 
    0x01,
    0x37, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x52000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x52C6931F Sigs

    //--------------------------------------------------
    // 0x007A : CRC_AUTOOPEN_CLOCK
    // WM.Clock:De: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x53000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x53D1DB4E Sigs

    //--------------------------------------------------
    // 0x007B : CRC_DATEIOFFNENBAK_XENIXOS
    // WM.Xenixos:De: DATEIôFFNENBAK

    0x01,
    0x45, 
    0x01,
    0x2F, 

    //--------------------------------------------------
    // 0x007C : CRC_DATEIOFFNEN_XENIXOS
    // WM.Xenixos:De: DATEIôFFNEN

    0x01,
    0x45, 
    0x01,
    0x16, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x54000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x542FFB05 Sigs

    //--------------------------------------------------
    // 0x007D : CRC_PLONG_SURABAYAA
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x71, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x5465E1ED Sigs

    //--------------------------------------------------
    // 0x007E : CRC_AUTOOPEN_TELE
    // WM.Tele:De: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    //--------------------------------------------------
    // 0x007F : CRC_AUTOOPENBAK_XENIXOS
    // WM.Xenixos:De: AUTOOPENBAK

    0x01,
    0x45, 
    0x01,
    0x2E, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x548BA77B Sigs

    //--------------------------------------------------
    // 0x0080 : CRC_VCLOSE_NICEDAY
    // WM.NiceDay: VCLOSE

    0x01,
    0x45, 
    0x01,
    0x44, 

    //--------------------------------------------------
    // 0x0081 : CRC_AUTOCLOSE3_NICEDAY
    // WM.NiceDay: AUTOCLOSE diff CRCWD8

    0x01,
    0x45, 
    0x01,
    0x47, 

    //--------------------------------------------------
    // 0x0082 : CRC_VCLOSE_NICEDAYG
    // NiceDay.G: VCLOSE

    0x01,
    0x45, 
    0x01,
    0x44, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x54CD4B41 Sigs

    //--------------------------------------------------
    // 0x0083 : CRC_OUTLAWA1
    // WM.Outlaw.A

    0x01,
    0x45, 
    0x01,
    0x22, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x57000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x57D82050 Sigs

    //--------------------------------------------------
    // 0x0084 : CRC_IKWORDNIETGOED1_CONCEPTK
    // WM.Concept.K:NL: IKWORDNIETGOED1

    0x01,
    0x45, 
    0x01,
    0x20, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x57E820DB Sigs

    //--------------------------------------------------
    // 0x0085 : CRC_MICROSOFT05_HASSLE
    // WM.Hassle: MICROSOFT05

    0x01,
    0x45, 
    0x01,
    0x29, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x58000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x58097C7A Sigs

    //--------------------------------------------------
    // 0x0086 : CRC_TOOLSMACRO_HELLGATE
    // WM.Hellgate: TOOLSMACRO

    0x01,
    0x45, 
    0x01,
    0x51, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x58A05FE1 Sigs

    //--------------------------------------------------
    // 0x0087 : CRC_TOOLSMACRO_CAPD
    // CRC7 == TOOLSMACRO_MSWA -- CRC8 != TOOLSMACRO_MSWA

    0x01,
    0x45, 
    0x01,
    0x51, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x5A000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x5A0F4C02 Sigs

    //--------------------------------------------------
    // 0x0088 : CRC_FILESAVE_DZTC
    // WM.Dzt.C: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x5A1373D8 Sigs

    //--------------------------------------------------
    // 0x0089 : CRC_AUTOEXEC_GOODNIGHT
    // WM.GoodNight: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x5AD9E8B9 Sigs

    //--------------------------------------------------
    // 0x008A : CRC_EXTRASMAKRO_CLOCK
    // WM.Clock:De: EXTRASMAKRO

    0x01,
    0x45, 
    0x01,
    0x63, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x5ADFE684 Sigs

    //--------------------------------------------------
    // 0x008B : CRC_VCLOSE2_NICEDAY
    // WM.NiceDay: VCLOSE

    0x01,
    0x45, 
    0x01,
    0x44, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x5B000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x5B1E704F Sigs

    //--------------------------------------------------
    // 0x008C : CRC_AUTOOPEN_DOGGIE
    // WM.Doggie: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x5D000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x5D05D6D2 Sigs

    //--------------------------------------------------
    // 0x008D : CRC_AOPNFINISH_MADDOGA
    // WM.MadDog.A: AOPNFINISH

    0x01,
    0x45, 
    0x01,
    0x5E, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x60000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x6089B16C Sigs

    //--------------------------------------------------
    // 0x008E : CRC_AUTOOPEN_SHOWOFF
    // WM.ShowOff: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x60BF46C7 Sigs

    //--------------------------------------------------
    // 0x008F : CRC_PRESENTZ_JOHNNY
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x5A, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x60D12F54 Sigs

    //--------------------------------------------------
    // 0x0090 : CRC_FCFINISH_MADDOGB
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x5D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x61000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x61666B40 Sigs

    //--------------------------------------------------
    // 0x0091 : CRC_AUTOEXECBAK_XENIXOS
    // WM.Xenixos:De: AUTOEXECBAK

    0x01,
    0x45, 
    0x01,
    0x2D, 

    //--------------------------------------------------
    // 0x0092 : CRC_AUTOEXEC_XENIXOS
    // WM.Xenixos:De: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x6189B36A Sigs

    //--------------------------------------------------
    // 0x0093 : CRC_AUTOOPEN_SPOOKY
    // WM.Spooky:De: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x62000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x62DE6342 Sigs

    //--------------------------------------------------
    // 0x0094 : CRC_INSERTPBREAK_HOT
    // WM.Hot: INSERTPBREAK

    0x01,
    0x45, 
    0x01,
    0x41, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x64000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x64E6B43A Sigs

    //--------------------------------------------------
    // 0x0095 : CRC_FILESAVEAS_JOHNNY
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x65000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x650E5AA2 Sigs

    //--------------------------------------------------
    // 0x0096 : CRC_EDITCOPY_HELLGATE
    // WM.Hellgate: EDITCOPY

    0x01,
    0x45, 
    0x01,
    0x6E, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x659C75A2 Sigs

    //--------------------------------------------------
    // 0x0097 : CRC_EXTRASMACRO_FRIENDLY
    // WM.Friendly:De: EXTRASMACRO

    0x01,
    0x45, 
    0x01,
    0x1D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x65C95634 Sigs

    //--------------------------------------------------
    // 0x0098 : CRC_AUTOCLOSE_WAZZUBA
    // WM.Wazzu.BA: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x66000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x664CACA4 Sigs

    //--------------------------------------------------
    // 0x0099 : CRC_TALK_FRIENDLY
    // WM.Friendly:De: TALK

    0x01,
    0x45, 
    0x01,
    0x19, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x67000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x67C0019C Sigs

    //--------------------------------------------------
    // 0x009A : CRC_FILESAVE_DZTA
    // WM.Dzt.A: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x68000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x68569621 Sigs

    //--------------------------------------------------
    // 0x009B : CRC_AUTOOPEN_WAZZUAH
    // WM.Wazzu.AH: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x686D4C4E Sigs

    //--------------------------------------------------
    // 0x009C : CRC_AUTOEXEC_BANDUNGC
    // WM.Bandung.C: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x68BEAB13 Sigs

    //--------------------------------------------------
    // 0x009D : CRC_AUTOCLOSE_HELPERB
    // Helper.B: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x69000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x6948E2D7 Sigs

    //--------------------------------------------------
    // 0x009E : CRC_FILEEXIT_HELLGATE
    // WM.Hellgate: FILEEXIT

    0x01,
    0x45, 
    0x01,
    0x4A, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x69FA2799 Sigs

    //--------------------------------------------------
    // 0x009F : CRC_AUTOOPEN_MINIMALC
    // W97M.Minimal.C: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x6A000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x6AABC61A Sigs

    //--------------------------------------------------
    // 0x00A0 : CRC_RPAO_RAPID
    // WM.Rapi.D: RPAO  diff CRCWD8 from A

    0x01,
    0x45, 
    0x01,
    0x55, 

    //--------------------------------------------------
    // 0x00A1 : CRC_AUTOOPEN_RAPIM2
    // WM.Rapi.M2: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x6B000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x6B7EB918 Sigs

    //--------------------------------------------------
    // 0x00A2 : CRC_AUTOEXEC_CLOCK
    // WM.Clock:De: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x6BBF9C56 Sigs

    //--------------------------------------------------
    // 0x00A3 : CRC_OUTLAWA2
    // WM.Outlaw.A

    0x01,
    0x45, 
    0x01,
    0x22, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x6C000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x6CC37630 Sigs

    //--------------------------------------------------
    // 0x00A4 : CRC_IKWORDNIETGOED2_CONCEPTK
    // WM.Concept.K:NL: IKWORDNIETGOED2

    0x01,
    0x45, 
    0x01,
    0x21, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x6D000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x6D579FA6 Sigs

    //--------------------------------------------------
    // 0x00A5 : CRC_DATEIBEENDEN_TELE
    // WM.Tele:De: DATEIBEENDEN

    0x01,
    0x45, 
    0x01,
    0x3C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x6E000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x6E485EA0 Sigs

    //--------------------------------------------------
    // 0x00A6 : CRC_APPDER_APPDERD
    // WM.Appder.D: APPDERD

    0x01,
    0x45, 
    0x01,
    0x05, 

    //--------------------------------------------------
    // 0x00A7 : CRC_AUTOOPEN_APPDERD
    // WM.Appder.D: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x6F000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x6F231812 Sigs

    //--------------------------------------------------
    // 0x00A8 : CRC_RPAE_RAPIF
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x53, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x6F4B9E57 Sigs

    //--------------------------------------------------
    // 0x00A9 : CRC_TIMER_SMILEYBDE
    // WM.Smiley.B:De: TIMER

    0x01,
    0x45, 
    0x01,
    0x69, 

    //--------------------------------------------------
    // 0x00AA : CRC_TIMER__SMILEYA
    // WM.Smiley.A:De: TIMER_

    0x01,
    0x45, 
    0x01,
    0x6A, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x6F750333 Sigs

    //--------------------------------------------------
    // 0x00AB : CRC_SPARKLE_SPARKLEA
    // Named CRC for SPARKLE

    0x01,
    0x45, 
    0x01,
    0x74, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x72000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x723244B1 Sigs

    //--------------------------------------------------
    // 0x00AC : CRC_AUTOCLOSE_IMPOSTERA
    // WM.Imposter.A: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x73000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x73AC0611 Sigs

    //--------------------------------------------------
    // 0x00AD : CRC_AUTOCLOSE_DIVINAC
    // WM.Divina.C: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x74000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x7441FFFF Sigs

    //--------------------------------------------------
    // 0x00AE : CRC_FILENEW_HELLGATE
    // WM.Hellgate: FILENEW

    0x01,
    0x45, 
    0x01,
    0x4B, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x74BAC13B Sigs

    //--------------------------------------------------
    // 0x00AF : CRC_NIGHTSHADE_NIGHTSHADEA
    // W97M.NightShade.A: NIGHTSHADE

    0x01,
    0x45, 
    0x01,
    0x70, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x75000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x75BD8500 Sigs

    //--------------------------------------------------
    // 0x00B0 : CRC_AUTONEW_MUCKC
    // WM.Muck.C: AUTONEW

    0x01,
    0x45, 
    0x01,
    0x52, 

    //--------------------------------------------------
    // 0x00B1 : CRC_AUTOCLOSE_MUCKC
    // WM.Muck.C: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x77000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x77D97129 Sigs

    //--------------------------------------------------
    // 0x00B2 : CRC_PAYLOAD_NICEDAY
    // WM.NiceDay: PAYLOAD is missing extra Attribute lines

    0x01,
    0x45, 
    0x01,
    0x4E, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x7A000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x7A36CBE1 Sigs

    //--------------------------------------------------
    // 0x00B3 : CRC_TOOLSMACRO_HASSLE
    // WM.Hassle: TOOLSMACRO

    0x01,
    0x45, 
    0x01,
    0x51, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x7B000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x7B9192B7 Sigs

    //--------------------------------------------------
    // 0x00B4 : CRC_FILESAVEAS_COUNTTENA
    // WM.CountTen.A: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x7D000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x7D3A2B3E Sigs

    //--------------------------------------------------
    // 0x00B5 : CRC_EDITCUT_HELLGATE
    // WM.Hellgate: EDITCUT

    0x01,
    0x45, 
    0x01,
    0x6D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x7D45340C Sigs

    //--------------------------------------------------
    // 0x00B6 : CRC_AUTOCLOSE_DIVINAA
    // WM.Divina.A: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x7DCF4735 Sigs

    //--------------------------------------------------
    // 0x00B7 : CRC_DATEIDRUCKENSTANDARDBAK_XENIXOS
    // WM.Xenixos:De: DATEIDRUCKENSTANDARDBAK

    0x01,
    0x45, 
    0x01,
    0x35, 

    //--------------------------------------------------
    // 0x00B8 : CRC_DATEIDRUCKENSTANDARD_XENIXOS
    // WM.Xenixos:De: DATEIDRUCKENSTANDARD

    0x01,
    0x45, 
    0x01,
    0x38, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x7DFEC194 Sigs

    //--------------------------------------------------
    // 0x00B9 : CRC_FILECLOSE_SHAREFUNA
    // WM.ShareFun.A: FILECLOSE

    0x01,
    0x45, 
    0x01,
    0x5C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x7E000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x7EB12C09 Sigs

    //--------------------------------------------------
    // 0x00BA : CRC_DATEIBEENDEN_FRIENDLY
    // WM.Friendly:De: DATEIBEENDEN

    0x01,
    0x45, 
    0x01,
    0x3C, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x7EF24DD5 Sigs

    //--------------------------------------------------
    // 0x00BB : CRC_AUTOOPEN_DZTC
    // WM.Dzt.C: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x7F000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x7F9F565C Sigs

    //--------------------------------------------------
    // 0x00BC : CRC_AUTOCLOSE_NICEDAY
    // WM.NiceDay: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x80000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x80F4B7A7 Sigs

    //--------------------------------------------------
    // 0x00BD : CRC_AUTOOPEN_IRISHK
    // WM.IrishK: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x82000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x8286F05D Sigs

    //--------------------------------------------------
    // 0x00BE : CRC_FILECLOSE0_CAPA
    // WM.Cap.A: FILECLOSE0 (from Vesselin) (and Cap.D - jw)

    0x01,
    0x45, 
    0x01,
    0x5C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x83000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x834E0255 Sigs

    //--------------------------------------------------
    // 0x00BF : CRC_AUTOCLOSE_DMVE
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x83E29739 Sigs

    //--------------------------------------------------
    // 0x00C0 : CRC_AUTOOPEN_BUERO
    // WM.Buero:De: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x86000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x868AEF3F Sigs

    //--------------------------------------------------
    // 0x00C1 : CRC_DMV_IMPOSTERA
    // WM.Imposter.A: DMV (Imposter.A Docs)

    0x01,
    0x45, 
    0x01,
    0x23, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x89000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x895D4671 Sigs

    //--------------------------------------------------
    // 0x00C2 : CRC_MICROSOFT04_HASSLE
    // WM.Hassle: MICROSOFT04

    0x01,
    0x45, 
    0x01,
    0x28, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x89BCEFB5 Sigs

    //--------------------------------------------------
    // 0x00C3 : CRC_AUTOOPEN_SWLABSB
    // SWLABS.B: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x8A000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x8A581A55 Sigs

    //--------------------------------------------------
    // 0x00C4 : CRC_AUTOOPEN_NPADP
    // WM.NPAD.P: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x8B000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x8B9B8BB7 Sigs

    //--------------------------------------------------
    // 0x00C5 : CRC_DATEIDRUCKEN_SMILEYBDE
    // WM.Smiley.B:De: DATEIDRUCKEN

    0x01,
    0x45, 
    0x01,
    0x3D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x8BA9129A Sigs

    //--------------------------------------------------
    // 0x00C6 : CRC_STRYX2_STRYX
    // WM.Stryx:De: STRYX2

    0x01,
    0x45, 
    0x01,
    0x13, 

    //--------------------------------------------------
    // 0x00C7 : CRC_STRYXTWO_STRYX
    // WM.Stryx:De: STRYXTWO

    0x01,
    0x45, 
    0x01,
    0x04, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x8D000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x8D4F994C Sigs

    //--------------------------------------------------
    // 0x00C8 : CRC_AUTOCLOSE2_NICEDAY
    // WM.NiceDay: AUTOCLOSE diff CRCWD8

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x8E000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x8E898EB3 Sigs

    //--------------------------------------------------
    // 0x00C9 : CRC_FAST_FRIENDLY
    // WM.Friendly:De: FAST

    0x01,
    0x45, 
    0x01,
    0x18, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x8EE5CA7A Sigs

    //--------------------------------------------------
    // 0x00CA : CRC_AUTOCLOSE_APPDERD
    // WM.Appder.D: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x8F000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x8FD6310E Sigs

    //--------------------------------------------------
    // 0x00CB : CRC_AUTOEXEC_BANDUNGA
    // WM.Bandung.A:AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x90000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x904866E2 Sigs

    //--------------------------------------------------
    // 0x00CC : CRC_FILETEMPLATES_HELLGATE
    // WM.Hellgate: FILETEMPLATES

    0x01,
    0x45, 
    0x01,
    0x68, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x90D70869 Sigs

    //--------------------------------------------------
    // 0x00CD : CRC_WORDHELPNT_IRISHJ
    // WM.Irish.J: WORDHELPNT

    0x01,
    0x45, 
    0x01,
    0x40, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x90ED5321 Sigs

    //--------------------------------------------------
    // 0x00CE : CRC_AUTOCLOSE_DIVINAF
    // Divina.F: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x91000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x91D1CF78 Sigs

    //--------------------------------------------------
    // 0x00CF : CRC_AUTOOPEN_NUCLEARB
    // WM.Nuclear.B: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x95000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x9533CD79 Sigs

    //--------------------------------------------------
    // 0x00D0 : CRC_AUTOOPEN_CONCEPTK
    // WM.Concept.K:NL: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x958C9963 Sigs

    //--------------------------------------------------
    // 0x00D1 : CRC_FILESAVE_JOHNNY
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x96000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x96B593EF Sigs

    //--------------------------------------------------
    // 0x00D2 : CRC_AUTOCLOSE_MDMAO
    // Named CRC for MDMA.O

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x97000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x9704022A Sigs

    //--------------------------------------------------
    // 0x00D3 : CRC_DATEIALLESSPEICHERN_CLOCK
    // WM.Clock:De: DATEIALLESSPEICHERN

    0x01,
    0x45, 
    0x01,
    0x09, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x9712041C Sigs

    //--------------------------------------------------
    // 0x00D4 : CRC_FILEOPEN_SHAREFUNA
    // WM.ShareFun.A: FILEOPEN

    0x01,
    0x45, 
    0x01,
    0x56, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x98000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x9821C630 Sigs

    //--------------------------------------------------
    // 0x00D5 : CRC_AUTOCLOSE_RAPIK
    // WM.Rapi.K: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x98545051 Sigs

    //--------------------------------------------------
    // 0x00D6 : CRC_DATEIOFFNEN_FRIENDLY
    // WM.Friendly:De: DATEIôFFNEN

    0x01,
    0x45, 
    0x01,
    0x16, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x9A000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x9A52CD9D Sigs

    //--------------------------------------------------
    // 0x00D7 : CRC_ANTIVIRUS_IRISH
    // WM.Irish: ANTIVIRUS

    0x01,
    0x45, 
    0x01,
    0x3F, 

    //--------------------------------------------------
    // 0x00D8 : CRC_FILESAVE_IRISH
    // WM.Irish: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x9A64AD27 Sigs

    //--------------------------------------------------
    // 0x00D9 : CRC_AUTOOPEN_BIRTHDAY
    // WM.Birthday:De: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x9AC5717E Sigs

    //--------------------------------------------------
    // 0x00DA : CRC_FILESAVE_ANTICONCEPT
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x9AEA8C98 Sigs

    //--------------------------------------------------
    // 0x00DB : CRC_FILESAVE_COUNTTENB
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x9B000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x9BE64AE8 Sigs

    //--------------------------------------------------
    // 0x00DC : CRC_DATEIDRUCKENSTANDARD_SPOOKY
    // WM.Spooky:De: DATEIDRUCKENSTANDARD

    0x01,
    0x45, 
    0x01,
    0x38, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0x9E000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x9E086C35 Sigs

    //--------------------------------------------------
    // 0x00DD : CRC_AUTOOPEN_ALIEN
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0x9E14BFFA Sigs

    //--------------------------------------------------
    // 0x00DE : CRC_AUTOOPEN_SMILEYBDE
    // WM.Smiley.B:De: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xA0000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xA0694EC9 Sigs

    //--------------------------------------------------
    // 0x00DF : CRC_NF2_NFA
    // WM.NF.A: NF (NORMAL.DOT)

    0x01,
    0x45, 
    0x01,
    0x65, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xA0D210A9 Sigs

    //--------------------------------------------------
    // 0x00E0 : CRC_AUTONEW2_TEDIOUS
    // WM.Tedious: AUTONEW2

    0x01,
    0x45, 
    0x01,
    0x52, 

    //--------------------------------------------------
    // 0x00E1 : CRC_VAUTONEW_TEDIOUS
    // WM.Tedious: VAUTONEW

    0x01,
    0x45, 
    0x01,
    0x24, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xA1000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xA108855A Sigs

    //--------------------------------------------------
    // 0x00E2 : CRC_TOOLSMACRO_COLORSA
    // WM.Colors.A: TOOLSMACRO

    0x01,
    0x45, 
    0x01,
    0x51, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xA1FC5DCF Sigs

    //--------------------------------------------------
    // 0x00E3 : CRC_AUTOCLOSE_GOODNIGHT
    // WM.GoodNight: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xA4000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xA40376B4 Sigs

    //--------------------------------------------------
    // 0x00E4 : CRC_FILEOPEN_GOODNIGHT
    // WM.GoodNight: FILEOPEN

    0x01,
    0x45, 
    0x01,
    0x56, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xA445F6C1 Sigs

    //--------------------------------------------------
    // 0x00E5 : CRC_FILECLOSE_MADDOGA
    // WM.MadDog.A: FILECLOSE

    0x01,
    0x45, 
    0x01,
    0x5C, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xA4474EDC Sigs

    //--------------------------------------------------
    // 0x00E6 : CRC_AUTOEXEC_RAPIE
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xA4ED87DA Sigs

    //--------------------------------------------------
    // 0x00E7 : CRC_FILESAVEAS_ALIEN
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xA6000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xA627A78A Sigs

    //--------------------------------------------------
    // 0x00E8 : CRC_OFXX_SHOWOFF
    // WM.ShowOff: OFXX

    0x01,
    0x45, 
    0x01,
    0x0C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xA7000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xA7E049C0 Sigs

    //--------------------------------------------------
    // 0x00E9 : CRC_AUTOOPEN_MUCKA
    // WM.Muck.A: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xA8000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xA847977C Sigs

    //--------------------------------------------------
    // 0x00EA : CRC_FILESAVEAS_FRIENDLY
    // WM.Friendly:De: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xA8EC1294 Sigs

    //--------------------------------------------------
    // 0x00EB : CRC_NEWFS_LUNCHA
    // WM.Lunch.A: NEWFS

    0x01,
    0x45, 
    0x01,
    0x0A, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xA9000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xA92142D3 Sigs

    //--------------------------------------------------
    // 0x00EC : CRC_AUTOEXEC_SATANIC
    // WM.Satanic: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xA96AB51D Sigs

    //--------------------------------------------------
    // 0x00ED : CRC_AUTOCLOSE_MDMAII
    // WM.MDMA.I: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xA98FF8B8 Sigs

    //--------------------------------------------------
    // 0x00EE : CRC_FILEPRINT_NUCLEARE
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x57, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xAA000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xAA2BC12C Sigs

    //--------------------------------------------------
    // 0x00EF : CRC_AUTOEXEC_MADDOGB
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xAAAC883D Sigs

    //--------------------------------------------------
    // 0x00F0 : CRC_NEWAO_LUNCHA
    // WM.Lunch.A: NEWAO

    0x01,
    0x45, 
    0x01,
    0x0B, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xAAAE1AD6 Sigs

    //--------------------------------------------------
    // 0x00F1 : CRC_AUTOOPEN_FRIENDLY
    // WM.Friendly:De: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xAAD73768 Sigs

    //--------------------------------------------------
    // 0x00F2 : CRC_FILESAVEAS_CAPA
    // WM.Cap.A: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xAB000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xAB49DE61 Sigs

    //--------------------------------------------------
    // 0x00F3 : CRC_FRENZY_FRENZYA
    // Named CRC for FRENZY

    0x01,
    0x45, 
    0x01,
    0x72, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xABC8998B Sigs

    //--------------------------------------------------
    // 0x00F4 : CRC_OUTLAWB2
    // WM.Outlaw.B

    0x01,
    0x45, 
    0x01,
    0x22, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xAD000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xAD8DC036 Sigs

    //--------------------------------------------------
    // 0x00F5 : CRC_AUTOEXEC_TWOLINESA
    // WM.TwoLines.A: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xAE000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xAE196DAA Sigs

    //--------------------------------------------------
    // 0x00F6 : CRC_DATEISPEICHERNBAK_XENIXOS
    // WM.Xenixos:De: DATEISPEICHERNBAK

    0x01,
    0x45, 
    0x01,
    0x33, 

    //--------------------------------------------------
    // 0x00F7 : CRC_DATEISPEICHERN_XENIXOS
    // WM.Xenixos:De: DATEISPEICHERN

    0x01,
    0x45, 
    0x01,
    0x37, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xAEB347C9 Sigs

    //--------------------------------------------------
    // 0x00F8 : CRC_DNZ_HELLGATE
    // WM.Hellgate: DNZ

    0x01,
    0x45, 
    0x01,
    0x6C, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xAEE09BCD Sigs

    //--------------------------------------------------
    // 0x00F9 : CRC_DATEISPEICHERNUNTER_SMILEYBDE
    // WM.Smiley.B:De: DATEISPEICHERNUNTER

    0x01,
    0x45, 
    0x01,
    0x39, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xAEEE95A6 Sigs

    //--------------------------------------------------
    // 0x00FA : CRC_AUTOEXEC_FRIENDLY
    // WM.Friendly:De: AUTOEXEC also used in NJ-WMDLK1.F

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xAF000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xAFDC0624 Sigs

    //--------------------------------------------------
    // 0x00FB : CRC_AUTOOPEN_WAZZUX
    // WM.Wazzu.X: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xB0000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xB085E3BE Sigs

    //--------------------------------------------------
    // 0x00FC : CRC_WORDHELPNT_IRISHF
    // Irish.F: WordHelpNT

    0x01,
    0x45, 
    0x01,
    0x40, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xB0C6CDBF Sigs

    //--------------------------------------------------
    // 0x00FD : CRC_AUTOCLOSE_GOLDFISH
    // WM.Goldfish: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xB2000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xB2536191 Sigs

    //--------------------------------------------------
    // 0x00FE : CRC_AUTOOPEN_NPADO
    // WM.Npad.O: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xB2A346CD Sigs

    //--------------------------------------------------
    // 0x00FF : CRC_AUTOOPEN_COUNTTENB
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xB2EA2B98 Sigs

    //--------------------------------------------------
    // 0x0100 : CRC_DATEISCHLIEBEN_CLOCK
    // WM.Clock:De: DATEISCHLIE·EN

    0x01,
    0x45, 
    0x01,
    0x1E, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xB4000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xB4087D28 Sigs

    //--------------------------------------------------
    // 0x0101 : CRC_DATEIDRUCKEN_TELE
    // WM.Tele:De: DATEIDRUCKEN

    0x01,
    0x45, 
    0x01,
    0x3D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xB4610DF5 Sigs

    //--------------------------------------------------
    // 0x0102 : CRC_DATEISCHLIEBEN_FRIENDLY
    // WM.Friendly:De: DATEISCHLIE·EN

    0x01,
    0x45, 
    0x01,
    0x1E, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xB5000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xB53710EE Sigs

    //--------------------------------------------------
    // 0x0103 : CRC_AUTOEXEC_NUCLEARB
    // WM.Nuclear.B: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xB6000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xB6CCC4C2 Sigs

    //--------------------------------------------------
    // 0x0104 : CRC_FILESAVEAS_DZTA
    // WM.Dzt.A: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xB6CD073F Sigs

    //--------------------------------------------------
    // 0x0105 : CRC_MCAFEE1_NUCLEARE
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x61, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xB7000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xB716EF79 Sigs

    //--------------------------------------------------
    // 0x0106 : CRC_APPDER_APPDERE
    // WM.Appder.E: APPDERE

    0x01,
    0x45, 
    0x01,
    0x05, 

    //--------------------------------------------------
    // 0x0107 : CRC_AUTOOPEN_APPDERE
    // WM.Appder.E: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xB9000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xB94A7368 Sigs

    //--------------------------------------------------
    // 0x0108 : CRC_RPAE_RAPIB
    // WM.Rapi.B: RPAE

    0x01,
    0x45, 
    0x01,
    0x53, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xB97F12A6 Sigs

    //--------------------------------------------------
    // 0x0109 : CRC_LADING_CONCEPTK
    // WM.Concept.K:NL: LADING

    0x01,
    0x45, 
    0x01,
    0x1F, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xBA000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xBA208083 Sigs

    //--------------------------------------------------
    // 0x010A : CRC_AUTOCLOSE_WAZZUAZ
    // WM.Wazzu.AZ: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xBA9BADBD Sigs

    //--------------------------------------------------
    // 0x010B : CRC_DATEIDOKVORLAGEN_CLOCK
    // WM.Clock:De: DATEIDOKVORLAGEN

    0x01,
    0x45, 
    0x01,
    0x17, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xBB000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xBB302507 Sigs

    //--------------------------------------------------
    // 0x010C : CRC_FILESAVE1_CAPA
    // WM.Cap.A: FILESAVE1  ; Formerly Cap.B FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xBC000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xBC7D3F95 Sigs

    //--------------------------------------------------
    // 0x010D : CRC_RPAE_RAPIN
    // WM.Rapi.N: RpAE

    0x01,
    0x45, 
    0x01,
    0x53, 

    //--------------------------------------------------
    // 0x010E : CRC_AUTOEXEC_RAPIN
    // WM.Rapi.N: AutoExec

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xBD000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xBD43422C Sigs

    //--------------------------------------------------
    // 0x010F : CRC_TOOLSCUSTOMIZE_HELLGATE
    // WM.Hellgate: TOOLSCUSTOMIZE

    0x01,
    0x45, 
    0x01,
    0x58, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xBD7C7683 Sigs

    //--------------------------------------------------
    // 0x0110 : CRC_WORDHELPNT_IRISHK
    // WM.Irish.K: WORDHELPNT /corrupted WORDHELPNT

    0x01,
    0x45, 
    0x01,
    0x40, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xBD7CD06C Sigs

    //--------------------------------------------------
    // 0x0111 : CRC_AUTOOPEN2_RAPIA
    // WM.Rapi.A: AUTOOPEN2

    0x01,
    0x45, 
    0x01,
    0x49, 

    //--------------------------------------------------
    // 0x0112 : CRC_RPAO2_RAPIA
    // WM.Rapi.A: RPAO2

    0x01,
    0x45, 
    0x01,
    0x55, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xBDC45C7B Sigs

    //--------------------------------------------------
    // 0x0113 : CRC_AUTOOPEN_BOOM
    // WM.Boom:De: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xBF000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xBFE15561 Sigs

    //--------------------------------------------------
    // 0x0114 : CRC_AUTOCLOSE_NFB
    // WM.NF.B: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xC0000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xC01D752C Sigs

    //--------------------------------------------------
    // 0x0115 : CRC_ANTIVIRUS_IRISHK
    // WM.Irish.K: ANTIVIRUS

    0x01,
    0x45, 
    0x01,
    0x3F, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xC0B0E4BE Sigs

    //--------------------------------------------------
    // 0x0116 : CRC_APPDER_APPDER
    // WM.Appder: APPDER

    0x01,
    0x45, 
    0x01,
    0x05, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xC0FE5BC1 Sigs

    //--------------------------------------------------
    // 0x0117 : CRC_DATEISPEICHERNUNTER_FRIENDLY
    // WM.Friendly:De: DATEISPEICHERNUNTER

    0x01,
    0x45, 
    0x01,
    0x39, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xC3000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xC32D535D Sigs

    //--------------------------------------------------
    // 0x0118 : CRC_AUTOCLOSE_NFA
    // WM.NF.A: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xC3693DA5 Sigs

    //--------------------------------------------------
    // 0x0119 : CRC_MICROSOFT01B_HASSLE
    // WM.Hassle: MICROSOFT01 (97 Temps)

    0x01,
    0x45, 
    0x01,
    0x27, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xC39F94A5 Sigs

    //--------------------------------------------------
    // 0x011A : CRC_AUTOOPEN_MADDOGB
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xC7000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xC70F7243 Sigs

    //--------------------------------------------------
    // 0x011B : CRC_AUTOOPEN_SWITCHES
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xC718A894 Sigs

    //--------------------------------------------------
    // 0x011C : CRC_FILEEXIT_SHAREFUNA
    // WM.ShareFun.A: FILEEXIT

    0x01,
    0x45, 
    0x01,
    0x4A, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xC7BA7DE1 Sigs

    //--------------------------------------------------
    // 0x011D : CRC_AUTOOPEN_SURABAYAA
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xC8000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xC86D4358 Sigs

    //--------------------------------------------------
    // 0x011E : CRC_STRYXONE_STRYX
    // WM.Stryx:De: STRYXONE

    0x01,
    0x45, 
    0x01,
    0x03, 

    //--------------------------------------------------
    // 0x011F : CRC_STRYX1_STRYX
    // WM.Stryx:De: STRYX1

    0x01,
    0x45, 
    0x01,
    0x12, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xCA000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xCA09E75F Sigs

    //--------------------------------------------------
    // 0x0120 : CRC_FILESAVEAS_RAPI
    // WM.Rapi: FILESAVEAS diff CRCWD8 from RPFSA and RapiD

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xCAF24F0B Sigs

    //--------------------------------------------------
    // 0x0121 : CRC_LONERAIDER_TARGETB
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x64, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xCB000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xCBF75AD9 Sigs

    //--------------------------------------------------
    // 0x0122 : CRC_FILECLOSE_MADDOGB
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x5C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xCC000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xCC107359 Sigs

    //--------------------------------------------------
    // 0x0123 : CRC_EXIT__GOODNIGHT
    // Named CRC for EXIT_

    0x01,
    0x45, 
    0x01,
    0x02, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xCCA193BB Sigs

    //--------------------------------------------------
    // 0x0124 : CRC_FILEOPEN_CAPA
    // WM.Cap.A: FILEOPEN

    0x01,
    0x45, 
    0x01,
    0x56, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xCF000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xCFE14FB5 Sigs

    //--------------------------------------------------
    // 0x0125 : CRC_FILECLOSE_GOODNIGHT
    // WM.GoodNight: FILECLOSE

    0x01,
    0x45, 
    0x01,
    0x5C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xD0000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xD03D7094 Sigs

    //--------------------------------------------------
    // 0x0126 : CRC_FILENEW_ANTICONCEPT
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x4B, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xD072568E Sigs

    //--------------------------------------------------
    // 0x0127 : CRC_AUTOEXIT_NICEDAY
    // WM.NiceDay: AUTOEXIT

    0x01,
    0x45, 
    0x01,
    0x46, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xD2000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xD20B157C Sigs

    //--------------------------------------------------
    // 0x0128 : CRC_AUTOOPEN_TWOLINESF
    // WM.TwoLines.F: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xD290F69D Sigs

    //--------------------------------------------------
    // 0x0129 : CRC_DATEISCHLIEBEN_STRYX
    // WM.Stryx:De: DATEISCHLIE·EN

    0x01,
    0x45, 
    0x01,
    0x1E, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xD3000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xD3ACC3B8 Sigs

    //--------------------------------------------------
    // 0x012A : CRC_DOGGIE_DOGGIE
    // WM.Doggie: DOGGIE

    0x01,
    0x45, 
    0x01,
    0x0F, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xD3BD18AF Sigs

    //--------------------------------------------------
    // 0x012B : CRC_AUTOCLOSE_TWOLINESA
    // WM.TwoLines.A: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xD4000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xD49C49ED Sigs

    //--------------------------------------------------
    // 0x012C : CRC_FILESAVE_RAPIM
    // WM.Rapi.M: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    //--------------------------------------------------
    // 0x012D : CRC_RPFS_RAPIA
    // WM.Rapi.A: RPFS

    0x01,
    0x45, 
    0x01,
    0x54, 

    //--------------------------------------------------
    // 0x012E : CRC_FILESAVE_RAPIA
    // WM.Rapi.A: FILESAVE

    0x01,
    0x45, 
    0x01,
    0x4C, 

    //--------------------------------------------------
    // 0x012F : CRC_FILESAVE_RAPIN
    // WM.Rapi.N: FILESAVE	WD7 same as Rapi.A

    0x01,
    0x45, 
    0x01,
    0x4C, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xD4FBBE09 Sigs

    //--------------------------------------------------
    // 0x0130 : CRC_DATEIDRUCKENBAK_XENIXOS
    // WM.Xenixos:De: DATEIDRUCKENBAK

    0x01,
    0x45, 
    0x01,
    0x32, 

    //--------------------------------------------------
    // 0x0131 : CRC_DATEIDRUCKEN_XENIXOS
    // WM.Xenixos:De: DATEIDRUCKEN

    0x01,
    0x45, 
    0x01,
    0x3D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xD6000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xD64F9C5B Sigs

    //--------------------------------------------------
    // 0x0132 : CRC_TOOLSSPELLING_HELLGATE
    // WM.Hellgate: TOOLSSPELLING

    0x01,
    0x45, 
    0x01,
    0x43, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xD7000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xD7698BEA Sigs

    //--------------------------------------------------
    // 0x0133 : CRC_AUTOEXEC_COLORSA
    // WM.Colors.A: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xD78D9800 Sigs

    //--------------------------------------------------
    // 0x0134 : CRC_AUTOOPEN_HYBRID
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xD8000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xD8999A64 Sigs

    //--------------------------------------------------
    // 0x0135 : CRC_DATEIOFFNEN_TELE
    // WM.Tele:De: DATEIôFFNEN

    0x01,
    0x45, 
    0x01,
    0x16, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xD9000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xD92A267A Sigs

    //--------------------------------------------------
    // 0x0136 : CRC_PARADISE2_GANGSTERZ
    // WM.Gangsterz: PARADISE

    0x01,
    0x45, 
    0x01,
    0x10, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xD99A666F Sigs

    //--------------------------------------------------
    // 0x0137 : CRC_AUTOOPEN_ANTICONCEPT
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xDA000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xDA895C9C Sigs

    //--------------------------------------------------
    // 0x0138 : CRC_HERRAMMACRO_DARKA
    // WM.Dark.A: HERRAMMACRO

    0x01,
    0x45, 
    0x01,
    0x67, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xDAB76A43 Sigs

    //--------------------------------------------------
    // 0x0139 : CRC_AUTOCLOSE_MADDOGB
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xDB000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xDB5B9B98 Sigs

    //--------------------------------------------------
    // 0x013A : CRC_PRESENTW_JOHNNY
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x59, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xDBCC0D79 Sigs

    //--------------------------------------------------
    // 0x013B : CRC_EXTRASMAKRO_XENIXOS
    // WM.Xenixos:De: EXTRASMAKRO

    0x01,
    0x45, 
    0x01,
    0x63, 

    //--------------------------------------------------
    // 0x013C : CRC_EXTRASMAKROBAK_XENIXOS
    // WM.Xenixos:De: EXTRASMAKROBAK

    0x01,
    0x45, 
    0x01,
    0x30, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xDBFADCE3 Sigs

    //--------------------------------------------------
    // 0x013D : CRC_MICROSOFT01A_HASSLE
    // WM.Hassle: MICROSOFT01 (95 & 97 Docs)

    0x01,
    0x45, 
    0x01,
    0x27, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xDC000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xDC0BB267 Sigs

    //--------------------------------------------------
    // 0x013E : CRC_DATEIDOKVORLAGEN_SPOOKY
    // WM.Spooky:De: DATEIDOKVORLAGEN

    0x01,
    0x45, 
    0x01,
    0x17, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xDD000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xDDD65870 Sigs

    //--------------------------------------------------
    // 0x013F : CRC_FILEPRINTDEFAULT_NUCLEARE
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x62, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xDE000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xDE3EF34A Sigs

    //--------------------------------------------------
    // 0x0140 : CRC_AUTOCLOSE_ALIEN
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xDEC36955 Sigs

    //--------------------------------------------------
    // 0x0141 : CRC_DATEISPEICHERNUNTERBAK_XENIXOS
    // WM.Xenixos:De: DATEISPEICHERNUNTERBAK

    0x01,
    0x45, 
    0x01,
    0x34, 

    //--------------------------------------------------
    // 0x0142 : CRC_DATEISPEICHERNUNTER_XENIXOS
    // WM.Xenixos:De: DATEISPEICHERNUNTER

    0x01,
    0x45, 
    0x01,
    0x39, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xE0000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xE069CE59 Sigs

    //--------------------------------------------------
    // 0x0143 : CRC_AUTOOPEN_EASY
    // WM.Easy: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xE1000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xE139949B Sigs

    //--------------------------------------------------
    // 0x0144 : CRC_AUTOOPEN_NPADQ
    // WM.Npad.Q: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xE1D834FA Sigs

    //--------------------------------------------------
    // 0x0145 : CRC_FCFINISH_MADDOGA
    // WM.MadDog.A: FCFINISH

    0x01,
    0x45, 
    0x01,
    0x5D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xE4000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xE4D6F956 Sigs

    //--------------------------------------------------
    // 0x0146 : CRC_DATUMUNDUHRZEIT_CLOCK
    // WM.Clock:De: DATUMUNDUHRZEIT

    0x01,
    0x45, 
    0x01,
    0x08, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xE6000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xE6554C06 Sigs

    //--------------------------------------------------
    // 0x0147 : CRC_FILETEMPLATES_SHAREFUNA
    // WM.ShareFun.A: FILETEMPLATES

    0x01,
    0x45, 
    0x01,
    0x68, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xE67471C3 Sigs

    //--------------------------------------------------
    // 0x0148 : CRC_AUTOEXEC_SURABAYAA
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xE7000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xE7965AA6 Sigs

    //--------------------------------------------------
    // 0x0149 : CRC_SPEICHERN_CLOCK
    // WM.Clock:De: SPEICHERN

    0x01,
    0x45, 
    0x01,
    0x07, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xE7980039 Sigs

    //--------------------------------------------------
    // 0x014A : CRC_AUTOEXEC_TELE
    // WM.Tele:De: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xE9000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xE93CD133 Sigs

    //--------------------------------------------------
    // 0x014B : CRC_FILESAVEAS_POLITE
    // WM.Polite: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xE9DCE3D5 Sigs

    //--------------------------------------------------
    // 0x014C : CRC_AUTOOPEN_NPADC
    // CRC and name signature association

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xEA000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xEACD2F51 Sigs

    //--------------------------------------------------
    // 0x014D : CRC_AUTOCLOSE_APPDER
    // WM.Appder: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xEB000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xEB16E71B Sigs

    //--------------------------------------------------
    // 0x014E : CRC_AUTOCLOSE_TWOLINESF
    // WM.TwoLines.F: AUTOCLOSE

    0x01,
    0x45, 
    0x01,
    0x47, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xEB436D3A Sigs

    //--------------------------------------------------
    // 0x014F : CRC_MSRUN_TWOLINESA
    // WM.TwoLines.A: MSRUN

    0x01,
    0x45, 
    0x01,
    0x6B, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xEB65FD8E Sigs

    //--------------------------------------------------
    // 0x0150 : CRC_FILEEXIT_FRIENDLY
    // WM.Friendly:De: FILEEXIT

    0x01,
    0x45, 
    0x01,
    0x4A, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xEC000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xEC219B8A Sigs

    //--------------------------------------------------
    // 0x0151 : CRC_FILESAVEAS_DZTC
    // WM.Dzt.C: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xEC676034 Sigs

    //--------------------------------------------------
    // 0x0152 : CRC_NF_NFB
    // WM.NF.B: NF

    0x01,
    0x45, 
    0x01,
    0x65, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xF1000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xF1A3335F Sigs

    //--------------------------------------------------
    // 0x0153 : CRC_BURONEU_BUERO
    // WM.Buero:De: BÅRONEU

    0x01,
    0x45, 
    0x01,
    0x2A, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xF1C3772C Sigs

    //--------------------------------------------------
    // 0x0154 : CRC_FILESAVEAS_MUCKA
    // WM.Muck.A: FILESAVEAS

    0x01,
    0x45, 
    0x01,
    0x4D, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xF2000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xF2419ECF Sigs

    //--------------------------------------------------
    // 0x0155 : CRC_PAYLOAD_CONCEPTA
    // WM.Concept.A: PAYLOAD

    0x01,
    0x45, 
    0x01,
    0x4E, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xF24BC294 Sigs

    //--------------------------------------------------
    // 0x0156 : CRC_INFIZIEREN_FRIENDLY
    // WM.Friendly:De: INFIZIEREN

    0x01,
    0x45, 
    0x01,
    0x1C, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xF3000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xF31D20FD Sigs

    //--------------------------------------------------
    // 0x0157 : CRC_AUTOEXEC_RAPIM
    // WM.Rapi.M: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    //--------------------------------------------------
    // 0x0158 : CRC_RPAE_RAPIM
    // WM.Rapi.M: RPAE

    0x01,
    0x45, 
    0x01,
    0x53, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xF37EA2E3 Sigs

    //--------------------------------------------------
    // 0x0159 : CRC_AUTOEXEC_BOOM
    // WM.Boom:De: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xF4000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xF424A1C5 Sigs

    //--------------------------------------------------
    // 0x015A : CRC_AUTOOPEN_SATANIC
    // WM.Satanic: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xF5000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xF5839B7B Sigs

    //--------------------------------------------------
    // 0x015B : CRC_AUTOOPEN_SHAREFUNA
    // WM.ShareFun.A: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xF6000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xF68F95B2 Sigs

    //--------------------------------------------------
    // 0x015C : CRC_PARADISE3_GANGSTERZ
    // WM.Gangsterz: PARADISE

    0x01,
    0x45, 
    0x01,
    0x10, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xF6A6362A Sigs

    //--------------------------------------------------
    // 0x015D : CRC_CANCEL_FRIENDLY
    // WM.Friendly:De: CANCEL

    0x01,
    0x45, 
    0x01,
    0x1A, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xF9000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xF916B2CE Sigs

    //--------------------------------------------------
    // 0x015E : CRC_DROP_XENIXOS
    // WM.Xenixos:De: DROP

    0x01,
    0x45, 
    0x01,
    0x2B, 

    //--------------------------------------------------
    // 0x015F : CRC_DROPBAK_XENIXOS
    // WM.Xenixos:De: DROPBAK

    0x01,
    0x45, 
    0x01,
    0x2C, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xF9F201E2 Sigs

    //--------------------------------------------------
    // 0x0160 : CRC_CFXX_SHOWOFF
    // WM.ShowOff: CFXX

    0x01,
    0x45, 
    0x01,
    0x0D, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xF9F8D0F6 Sigs

    //--------------------------------------------------
    // 0x0161 : CRC_ANTIVIRUS2_IRISH
    // WM.Irish: ANTIVIRUS2

    0x01,
    0x45, 
    0x01,
    0x3F, 

    //--------------------------------------------------
    // 0x0162 : CRC_AUTOOPEN_IRISH
    // WM.Irish: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xFB000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xFB1867D6 Sigs

    //--------------------------------------------------
    // 0x0163 : CRC_AUTOOPEN_WAZZUBS
    // WM.Wazzu.BS: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xFB8CDD68 Sigs

    //--------------------------------------------------
    // 0x0164 : CRC_SHARETHEFUN_SHAREFUNA
    // WM.ShareFun.A: SHARETHEFUN

    0x01,
    0x45, 
    0x01,
    0x00, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xFD000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xFD1A876E Sigs

    //--------------------------------------------------
    // 0x0165 : CRC_SPOOKY_SPOOKY
    // WM.Spooky:De: SPOOKY

    0x01,
    0x45, 
    0x01,
    0x15, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xFDC2CFAF Sigs

    //--------------------------------------------------
    // 0x0166 : CRC_OFFNEN_CLOCK
    // WM.Clock:De: ôFFNEN

    0x01,
    0x45, 
    0x01,
    0x06, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xFDC5A4F0 Sigs

    //--------------------------------------------------
    // 0x0167 : CRC_RPAE_RAPIA
    // WM.Rapi.A: RPAE

    0x01,
    0x45, 
    0x01,
    0x53, 

    //--------------------------------------------------
    // 0x0168 : CRC_AUTOEXEC_RAPIA
    // WM.Rapi.A: AUTOEXEC

    0x01,
    0x45, 
    0x01,
    0x48, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xFDCAF6F4 Sigs

    //--------------------------------------------------
    // 0x0169 : CRC_RPAO_RAPIN
    // WM.Rapi.N: RPAO	 WD7 same as Rapi.A

    0x01,
    0x45, 
    0x01,
    0x55, 

    //--------------------------------------------------
    // 0x016A : CRC_AUTOOPEN_RAPIN
    // WM.Rapi.N: AUTOOPEN	WD7 same as Rapi.A

    0x01,
    0x45, 
    0x01,
    0x49, 

    //--------------------------------------------------
    // 0x016B : CRC_RPAO_RAPIA
    // WM.Rapi.A: RPAO

    0x01,
    0x45, 
    0x01,
    0x55, 

    //--------------------------------------------------
    // 0x016C : CRC_AUTOOPEN_RAPIA
    // WM.Rapi.A: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xFE000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xFE478B42 Sigs

    //--------------------------------------------------
    // 0x016D : CRC_AUTOOPEN_TWOLINESA
    // WM.TwoLines.A: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00,

    ////////////////////////////////////////////////////
    // NamedCRC 0xFEAEA366 Sigs

    //--------------------------------------------------
    // 0x016E : CRC_AUTOOPEN_DATE
    // WM.Date: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRC0xFF000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xFFD41F63 Sigs

    //--------------------------------------------------
    // 0x016F : CRC_AUTOOPEN_GOLDFISH
    // WM.Goldfish: AUTOOPEN

    0x01,
    0x45, 
    0x01,
    0x49, 

    0x00
};

BYTE FAR gabyWD8NamedCRCSigFullSet[] =
{
    0x03, 0xFF, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

WD_NAMED_CRC_SIG_INFO_T FAR gastWD8NamedCRCSigInfo[] =
{
    { 0x0000, 0x0000, gabyWD8NamedCRC0x00000000Sigs +    0 }, // 0x0046F610
    { 0x0001, 0x0001, gabyWD8NamedCRC0x00000000Sigs +    5 }, // 0x00744F85
    { 0x0002, 0x0002, gabyWD8NamedCRC0x00000000Sigs +   10 }, // 0x0074A804
    { 0x0003, 0x0003, gabyWD8NamedCRC0x02000000Sigs +    0 }, // 0x0215B17F
    { 0x0004, 0x0004, gabyWD8NamedCRC0x03000000Sigs +    0 }, // 0x0328F2CE
    { 0x0005, 0x0005, gabyWD8NamedCRC0x03000000Sigs +    5 }, // 0x03C1E4B9
    { 0x0006, 0x0006, gabyWD8NamedCRC0x04000000Sigs +    0 }, // 0x04043AFF
    { 0x0007, 0x0007, gabyWD8NamedCRC0x05000000Sigs +    0 }, // 0x05BC7AC4
    { 0x0008, 0x0008, gabyWD8NamedCRC0x06000000Sigs +    0 }, // 0x06330781
    { 0x0009, 0x000C, gabyWD8NamedCRC0x06000000Sigs +   17 }, // 0x06B98B39
    { 0x000A, 0x000E, gabyWD8NamedCRC0x09000000Sigs +    0 }, // 0x09BC7CA5
    { 0x000B, 0x000F, gabyWD8NamedCRC0x0B000000Sigs +    0 }, // 0x0B458673
    { 0x000C, 0x0011, gabyWD8NamedCRC0x0B000000Sigs +    9 }, // 0x0B537BF4
    { 0x000D, 0x0012, gabyWD8NamedCRC0x0E000000Sigs +    0 }, // 0x0E32D985
    { 0x000E, 0x0013, gabyWD8NamedCRC0x0E000000Sigs +    5 }, // 0x0EA7C757
    { 0x000F, 0x0014, gabyWD8NamedCRC0x0E000000Sigs +   10 }, // 0x0EBB1156
    { 0x0010, 0x0015, gabyWD8NamedCRC0x0F000000Sigs +    0 }, // 0x0F65DFC6
    { 0x0011, 0x0016, gabyWD8NamedCRC0x0F000000Sigs +    5 }, // 0x0F93CBFD
    { 0x0012, 0x0017, gabyWD8NamedCRC0x0F000000Sigs +   10 }, // 0x0FD040FC
    { 0x0013, 0x0018, gabyWD8NamedCRC0x10000000Sigs +    0 }, // 0x10E6D600
    { 0x0014, 0x0019, gabyWD8NamedCRC0x12000000Sigs +    0 }, // 0x1210550D
    { 0x0015, 0x001A, gabyWD8NamedCRC0x13000000Sigs +    0 }, // 0x13786871
    { 0x0016, 0x001B, gabyWD8NamedCRC0x14000000Sigs +    0 }, // 0x148C1E87
    { 0x0017, 0x001C, gabyWD8NamedCRC0x15000000Sigs +    0 }, // 0x15165FA7
    { 0x0018, 0x001D, gabyWD8NamedCRC0x15000000Sigs +    5 }, // 0x157871C9
    { 0x0019, 0x001E, gabyWD8NamedCRC0x17000000Sigs +    0 }, // 0x17028C90
    { 0x001A, 0x001F, gabyWD8NamedCRC0x17000000Sigs +    5 }, // 0x172A3A16
    { 0x001B, 0x0020, gabyWD8NamedCRC0x17000000Sigs +   10 }, // 0x17B48CD9
    { 0x001C, 0x0021, gabyWD8NamedCRC0x18000000Sigs +    0 }, // 0x18958CE9
    { 0x001D, 0x0022, gabyWD8NamedCRC0x19000000Sigs +    0 }, // 0x196767D1
    { 0x001E, 0x0023, gabyWD8NamedCRC0x1A000000Sigs +    0 }, // 0x1A20CD84
    { 0x001F, 0x0024, gabyWD8NamedCRC0x1A000000Sigs +    5 }, // 0x1AD04632
    { 0x0020, 0x0025, gabyWD8NamedCRC0x1B000000Sigs +    0 }, // 0x1B1E0DA3
    { 0x0021, 0x0026, gabyWD8NamedCRC0x1B000000Sigs +    5 }, // 0x1BE90DC4
    { 0x0022, 0x0027, gabyWD8NamedCRC0x1C000000Sigs +    0 }, // 0x1C3DE162
    { 0x0023, 0x0028, gabyWD8NamedCRC0x1C000000Sigs +    5 }, // 0x1C7E2053
    { 0x0024, 0x002A, gabyWD8NamedCRC0x1C000000Sigs +   14 }, // 0x1CEEB062
    { 0x0025, 0x002B, gabyWD8NamedCRC0x1E000000Sigs +    0 }, // 0x1E25C00B
    { 0x0026, 0x002C, gabyWD8NamedCRC0x1F000000Sigs +    0 }, // 0x1F82C663
    { 0x0027, 0x002D, gabyWD8NamedCRC0x1F000000Sigs +    5 }, // 0x1FA89B22
    { 0x0028, 0x002E, gabyWD8NamedCRC0x20000000Sigs +    0 }, // 0x2087F0D4
    { 0x0029, 0x0030, gabyWD8NamedCRC0x21000000Sigs +    0 }, // 0x21B276D8
    { 0x002A, 0x0031, gabyWD8NamedCRC0x21000000Sigs +    5 }, // 0x21FD466F
    { 0x002B, 0x0032, gabyWD8NamedCRC0x22000000Sigs +    0 }, // 0x224337BC
    { 0x002C, 0x0033, gabyWD8NamedCRC0x22000000Sigs +    5 }, // 0x22C1BFFB
    { 0x002D, 0x0034, gabyWD8NamedCRC0x23000000Sigs +    0 }, // 0x237C8A14
    { 0x002E, 0x0035, gabyWD8NamedCRC0x24000000Sigs +    0 }, // 0x2404ABAB
    { 0x002F, 0x0036, gabyWD8NamedCRC0x24000000Sigs +    5 }, // 0x245B764F
    { 0x0030, 0x0039, gabyWD8NamedCRC0x26000000Sigs +    0 }, // 0x2647411A
    { 0x0031, 0x003A, gabyWD8NamedCRC0x26000000Sigs +    5 }, // 0x26D83252
    { 0x0032, 0x003B, gabyWD8NamedCRC0x27000000Sigs +    0 }, // 0x272B5519
    { 0x0033, 0x003C, gabyWD8NamedCRC0x28000000Sigs +    0 }, // 0x281B1454
    { 0x0034, 0x003D, gabyWD8NamedCRC0x2A000000Sigs +    0 }, // 0x2A324BE3
    { 0x0035, 0x003E, gabyWD8NamedCRC0x2B000000Sigs +    0 }, // 0x2B2B7235
    { 0x0036, 0x003F, gabyWD8NamedCRC0x2C000000Sigs +    0 }, // 0x2C5A32DB
    { 0x0037, 0x0040, gabyWD8NamedCRC0x2D000000Sigs +    0 }, // 0x2D14CB4C
    { 0x0038, 0x0041, gabyWD8NamedCRC0x2D000000Sigs +    5 }, // 0x2D20F031
    { 0x0039, 0x0042, gabyWD8NamedCRC0x31000000Sigs +    0 }, // 0x3159C164
    { 0x003A, 0x0043, gabyWD8NamedCRC0x32000000Sigs +    0 }, // 0x325619D5
    { 0x003B, 0x0044, gabyWD8NamedCRC0x32000000Sigs +    5 }, // 0x326EFB88
    { 0x003C, 0x0045, gabyWD8NamedCRC0x34000000Sigs +    0 }, // 0x343FAEC5
    { 0x003D, 0x0046, gabyWD8NamedCRC0x34000000Sigs +    5 }, // 0x347D1E28
    { 0x003E, 0x0047, gabyWD8NamedCRC0x34000000Sigs +   10 }, // 0x3499C680
    { 0x003F, 0x0049, gabyWD8NamedCRC0x35000000Sigs +    0 }, // 0x35246CF0
    { 0x0040, 0x004A, gabyWD8NamedCRC0x35000000Sigs +    5 }, // 0x356858D7
    { 0x0041, 0x004B, gabyWD8NamedCRC0x35000000Sigs +   10 }, // 0x35845FC4
    { 0x0042, 0x004C, gabyWD8NamedCRC0x35000000Sigs +   15 }, // 0x35D1B21B
    { 0x0043, 0x004D, gabyWD8NamedCRC0x37000000Sigs +    0 }, // 0x37426D3D
    { 0x0044, 0x004E, gabyWD8NamedCRC0x37000000Sigs +    5 }, // 0x37C58617
    { 0x0045, 0x0050, gabyWD8NamedCRC0x38000000Sigs +    0 }, // 0x38885F16
    { 0x0046, 0x0051, gabyWD8NamedCRC0x39000000Sigs +    0 }, // 0x39137649
    { 0x0047, 0x0052, gabyWD8NamedCRC0x39000000Sigs +    5 }, // 0x396BE61E
    { 0x0048, 0x0055, gabyWD8NamedCRC0x39000000Sigs +   18 }, // 0x39B61B07
    { 0x0049, 0x0057, gabyWD8NamedCRC0x3C000000Sigs +    0 }, // 0x3CBEC272
    { 0x004A, 0x0058, gabyWD8NamedCRC0x3C000000Sigs +    5 }, // 0x3CC627F9
    { 0x004B, 0x0059, gabyWD8NamedCRC0x40000000Sigs +    0 }, // 0x4082B807
    { 0x004C, 0x005A, gabyWD8NamedCRC0x40000000Sigs +    5 }, // 0x40851923
    { 0x004D, 0x005B, gabyWD8NamedCRC0x41000000Sigs +    0 }, // 0x41670A52
    { 0x004E, 0x005C, gabyWD8NamedCRC0x41000000Sigs +    5 }, // 0x41A05500
    { 0x004F, 0x005D, gabyWD8NamedCRC0x41000000Sigs +   10 }, // 0x41C83079
    { 0x0050, 0x005E, gabyWD8NamedCRC0x42000000Sigs +    0 }, // 0x4268723C
    { 0x0051, 0x005F, gabyWD8NamedCRC0x43000000Sigs +    0 }, // 0x430B531A
    { 0x0052, 0x0060, gabyWD8NamedCRC0x43000000Sigs +    5 }, // 0x430CA51B
    { 0x0053, 0x0064, gabyWD8NamedCRC0x43000000Sigs +   22 }, // 0x435A5D4A
    { 0x0054, 0x0066, gabyWD8NamedCRC0x43000000Sigs +   31 }, // 0x43D94616
    { 0x0055, 0x0067, gabyWD8NamedCRC0x46000000Sigs +    0 }, // 0x4615A075
    { 0x0056, 0x0068, gabyWD8NamedCRC0x46000000Sigs +    5 }, // 0x463C3A23
    { 0x0057, 0x0069, gabyWD8NamedCRC0x47000000Sigs +    0 }, // 0x4781CD20
    { 0x0058, 0x006A, gabyWD8NamedCRC0x48000000Sigs +    0 }, // 0x48FD1260
    { 0x0059, 0x006B, gabyWD8NamedCRC0x49000000Sigs +    0 }, // 0x4937D907
    { 0x005A, 0x006C, gabyWD8NamedCRC0x49000000Sigs +    5 }, // 0x49DD12D5
    { 0x005B, 0x006D, gabyWD8NamedCRC0x49000000Sigs +   10 }, // 0x49DDD82F
    { 0x005C, 0x006E, gabyWD8NamedCRC0x4A000000Sigs +    0 }, // 0x4AAA431F
    { 0x005D, 0x006F, gabyWD8NamedCRC0x4A000000Sigs +    5 }, // 0x4AFC2D9D
    { 0x005E, 0x0070, gabyWD8NamedCRC0x4B000000Sigs +    0 }, // 0x4B5BD6DE
    { 0x005F, 0x0071, gabyWD8NamedCRC0x4C000000Sigs +    0 }, // 0x4C0DE0F7
    { 0x0060, 0x0072, gabyWD8NamedCRC0x4C000000Sigs +    5 }, // 0x4C2456D9
    { 0x0061, 0x0073, gabyWD8NamedCRC0x4D000000Sigs +    0 }, // 0x4D925A10
    { 0x0062, 0x0075, gabyWD8NamedCRC0x4D000000Sigs +    9 }, // 0x4DEE7C12
    { 0x0063, 0x0076, gabyWD8NamedCRC0x4F000000Sigs +    0 }, // 0x4F21D215
    { 0x0064, 0x0077, gabyWD8NamedCRC0x4F000000Sigs +    5 }, // 0x4F2CFE3C
    { 0x0065, 0x0078, gabyWD8NamedCRC0x4F000000Sigs +   10 }, // 0x4FF00686
    { 0x0066, 0x0079, gabyWD8NamedCRC0x51000000Sigs +    0 }, // 0x517A855C
    { 0x0067, 0x007A, gabyWD8NamedCRC0x52000000Sigs +    0 }, // 0x52C6931F
    { 0x0068, 0x007B, gabyWD8NamedCRC0x53000000Sigs +    0 }, // 0x53D1DB4E
    { 0x0069, 0x007D, gabyWD8NamedCRC0x54000000Sigs +    0 }, // 0x542FFB05
    { 0x006A, 0x007E, gabyWD8NamedCRC0x54000000Sigs +    5 }, // 0x5465E1ED
    { 0x006B, 0x0080, gabyWD8NamedCRC0x54000000Sigs +   14 }, // 0x548BA77B
    { 0x006C, 0x0083, gabyWD8NamedCRC0x54000000Sigs +   27 }, // 0x54CD4B41
    { 0x006D, 0x0084, gabyWD8NamedCRC0x57000000Sigs +    0 }, // 0x57D82050
    { 0x006E, 0x0085, gabyWD8NamedCRC0x57000000Sigs +    5 }, // 0x57E820DB
    { 0x006F, 0x0086, gabyWD8NamedCRC0x58000000Sigs +    0 }, // 0x58097C7A
    { 0x0070, 0x0087, gabyWD8NamedCRC0x58000000Sigs +    5 }, // 0x58A05FE1
    { 0x0071, 0x0088, gabyWD8NamedCRC0x5A000000Sigs +    0 }, // 0x5A0F4C02
    { 0x0072, 0x0089, gabyWD8NamedCRC0x5A000000Sigs +    5 }, // 0x5A1373D8
    { 0x0073, 0x008A, gabyWD8NamedCRC0x5A000000Sigs +   10 }, // 0x5AD9E8B9
    { 0x0074, 0x008B, gabyWD8NamedCRC0x5A000000Sigs +   15 }, // 0x5ADFE684
    { 0x0075, 0x008C, gabyWD8NamedCRC0x5B000000Sigs +    0 }, // 0x5B1E704F
    { 0x0076, 0x008D, gabyWD8NamedCRC0x5D000000Sigs +    0 }, // 0x5D05D6D2
    { 0x0077, 0x008E, gabyWD8NamedCRC0x60000000Sigs +    0 }, // 0x6089B16C
    { 0x0078, 0x008F, gabyWD8NamedCRC0x60000000Sigs +    5 }, // 0x60BF46C7
    { 0x0079, 0x0090, gabyWD8NamedCRC0x60000000Sigs +   10 }, // 0x60D12F54
    { 0x007A, 0x0091, gabyWD8NamedCRC0x61000000Sigs +    0 }, // 0x61666B40
    { 0x007B, 0x0093, gabyWD8NamedCRC0x61000000Sigs +    9 }, // 0x6189B36A
    { 0x007C, 0x0094, gabyWD8NamedCRC0x62000000Sigs +    0 }, // 0x62DE6342
    { 0x007D, 0x0095, gabyWD8NamedCRC0x64000000Sigs +    0 }, // 0x64E6B43A
    { 0x007E, 0x0096, gabyWD8NamedCRC0x65000000Sigs +    0 }, // 0x650E5AA2
    { 0x007F, 0x0097, gabyWD8NamedCRC0x65000000Sigs +    5 }, // 0x659C75A2
    { 0x0080, 0x0098, gabyWD8NamedCRC0x65000000Sigs +   10 }, // 0x65C95634
    { 0x0081, 0x0099, gabyWD8NamedCRC0x66000000Sigs +    0 }, // 0x664CACA4
    { 0x0082, 0x009A, gabyWD8NamedCRC0x67000000Sigs +    0 }, // 0x67C0019C
    { 0x0083, 0x009B, gabyWD8NamedCRC0x68000000Sigs +    0 }, // 0x68569621
    { 0x0084, 0x009C, gabyWD8NamedCRC0x68000000Sigs +    5 }, // 0x686D4C4E
    { 0x0085, 0x009D, gabyWD8NamedCRC0x68000000Sigs +   10 }, // 0x68BEAB13
    { 0x0086, 0x009E, gabyWD8NamedCRC0x69000000Sigs +    0 }, // 0x6948E2D7
    { 0x0087, 0x009F, gabyWD8NamedCRC0x69000000Sigs +    5 }, // 0x69FA2799
    { 0x0088, 0x00A0, gabyWD8NamedCRC0x6A000000Sigs +    0 }, // 0x6AABC61A
    { 0x0089, 0x00A2, gabyWD8NamedCRC0x6B000000Sigs +    0 }, // 0x6B7EB918
    { 0x008A, 0x00A3, gabyWD8NamedCRC0x6B000000Sigs +    5 }, // 0x6BBF9C56
    { 0x008B, 0x00A4, gabyWD8NamedCRC0x6C000000Sigs +    0 }, // 0x6CC37630
    { 0x008C, 0x00A5, gabyWD8NamedCRC0x6D000000Sigs +    0 }, // 0x6D579FA6
    { 0x008D, 0x00A6, gabyWD8NamedCRC0x6E000000Sigs +    0 }, // 0x6E485EA0
    { 0x008E, 0x00A8, gabyWD8NamedCRC0x6F000000Sigs +    0 }, // 0x6F231812
    { 0x008F, 0x00A9, gabyWD8NamedCRC0x6F000000Sigs +    5 }, // 0x6F4B9E57
    { 0x0090, 0x00AB, gabyWD8NamedCRC0x6F000000Sigs +   14 }, // 0x6F750333
    { 0x0091, 0x00AC, gabyWD8NamedCRC0x72000000Sigs +    0 }, // 0x723244B1
    { 0x0092, 0x00AD, gabyWD8NamedCRC0x73000000Sigs +    0 }, // 0x73AC0611
    { 0x0093, 0x00AE, gabyWD8NamedCRC0x74000000Sigs +    0 }, // 0x7441FFFF
    { 0x0094, 0x00AF, gabyWD8NamedCRC0x74000000Sigs +    5 }, // 0x74BAC13B
    { 0x0095, 0x00B0, gabyWD8NamedCRC0x75000000Sigs +    0 }, // 0x75BD8500
    { 0x0096, 0x00B2, gabyWD8NamedCRC0x77000000Sigs +    0 }, // 0x77D97129
    { 0x0097, 0x00B3, gabyWD8NamedCRC0x7A000000Sigs +    0 }, // 0x7A36CBE1
    { 0x0098, 0x00B4, gabyWD8NamedCRC0x7B000000Sigs +    0 }, // 0x7B9192B7
    { 0x0099, 0x00B5, gabyWD8NamedCRC0x7D000000Sigs +    0 }, // 0x7D3A2B3E
    { 0x009A, 0x00B6, gabyWD8NamedCRC0x7D000000Sigs +    5 }, // 0x7D45340C
    { 0x009B, 0x00B7, gabyWD8NamedCRC0x7D000000Sigs +   10 }, // 0x7DCF4735
    { 0x009C, 0x00B9, gabyWD8NamedCRC0x7D000000Sigs +   19 }, // 0x7DFEC194
    { 0x009D, 0x00BA, gabyWD8NamedCRC0x7E000000Sigs +    0 }, // 0x7EB12C09
    { 0x009E, 0x00BB, gabyWD8NamedCRC0x7E000000Sigs +    5 }, // 0x7EF24DD5
    { 0x009F, 0x00BC, gabyWD8NamedCRC0x7F000000Sigs +    0 }, // 0x7F9F565C
    { 0x00A0, 0x00BD, gabyWD8NamedCRC0x80000000Sigs +    0 }, // 0x80F4B7A7
    { 0x00A1, 0x00BE, gabyWD8NamedCRC0x82000000Sigs +    0 }, // 0x8286F05D
    { 0x00A2, 0x00BF, gabyWD8NamedCRC0x83000000Sigs +    0 }, // 0x834E0255
    { 0x00A3, 0x00C0, gabyWD8NamedCRC0x83000000Sigs +    5 }, // 0x83E29739
    { 0x00A4, 0x00C1, gabyWD8NamedCRC0x86000000Sigs +    0 }, // 0x868AEF3F
    { 0x00A5, 0x00C2, gabyWD8NamedCRC0x89000000Sigs +    0 }, // 0x895D4671
    { 0x00A6, 0x00C3, gabyWD8NamedCRC0x89000000Sigs +    5 }, // 0x89BCEFB5
    { 0x00A7, 0x00C4, gabyWD8NamedCRC0x8A000000Sigs +    0 }, // 0x8A581A55
    { 0x00A8, 0x00C5, gabyWD8NamedCRC0x8B000000Sigs +    0 }, // 0x8B9B8BB7
    { 0x00A9, 0x00C6, gabyWD8NamedCRC0x8B000000Sigs +    5 }, // 0x8BA9129A
    { 0x00AA, 0x00C8, gabyWD8NamedCRC0x8D000000Sigs +    0 }, // 0x8D4F994C
    { 0x00AB, 0x00C9, gabyWD8NamedCRC0x8E000000Sigs +    0 }, // 0x8E898EB3
    { 0x00AC, 0x00CA, gabyWD8NamedCRC0x8E000000Sigs +    5 }, // 0x8EE5CA7A
    { 0x00AD, 0x00CB, gabyWD8NamedCRC0x8F000000Sigs +    0 }, // 0x8FD6310E
    { 0x00AE, 0x00CC, gabyWD8NamedCRC0x90000000Sigs +    0 }, // 0x904866E2
    { 0x00AF, 0x00CD, gabyWD8NamedCRC0x90000000Sigs +    5 }, // 0x90D70869
    { 0x00B0, 0x00CE, gabyWD8NamedCRC0x90000000Sigs +   10 }, // 0x90ED5321
    { 0x00B1, 0x00CF, gabyWD8NamedCRC0x91000000Sigs +    0 }, // 0x91D1CF78
    { 0x00B2, 0x00D0, gabyWD8NamedCRC0x95000000Sigs +    0 }, // 0x9533CD79
    { 0x00B3, 0x00D1, gabyWD8NamedCRC0x95000000Sigs +    5 }, // 0x958C9963
    { 0x00B4, 0x00D2, gabyWD8NamedCRC0x96000000Sigs +    0 }, // 0x96B593EF
    { 0x00B5, 0x00D3, gabyWD8NamedCRC0x97000000Sigs +    0 }, // 0x9704022A
    { 0x00B6, 0x00D4, gabyWD8NamedCRC0x97000000Sigs +    5 }, // 0x9712041C
    { 0x00B7, 0x00D5, gabyWD8NamedCRC0x98000000Sigs +    0 }, // 0x9821C630
    { 0x00B8, 0x00D6, gabyWD8NamedCRC0x98000000Sigs +    5 }, // 0x98545051
    { 0x00B9, 0x00D7, gabyWD8NamedCRC0x9A000000Sigs +    0 }, // 0x9A52CD9D
    { 0x00BA, 0x00D9, gabyWD8NamedCRC0x9A000000Sigs +    9 }, // 0x9A64AD27
    { 0x00BB, 0x00DA, gabyWD8NamedCRC0x9A000000Sigs +   14 }, // 0x9AC5717E
    { 0x00BC, 0x00DB, gabyWD8NamedCRC0x9A000000Sigs +   19 }, // 0x9AEA8C98
    { 0x00BD, 0x00DC, gabyWD8NamedCRC0x9B000000Sigs +    0 }, // 0x9BE64AE8
    { 0x00BE, 0x00DD, gabyWD8NamedCRC0x9E000000Sigs +    0 }, // 0x9E086C35
    { 0x00BF, 0x00DE, gabyWD8NamedCRC0x9E000000Sigs +    5 }, // 0x9E14BFFA
    { 0x00C0, 0x00DF, gabyWD8NamedCRC0xA0000000Sigs +    0 }, // 0xA0694EC9
    { 0x00C1, 0x00E0, gabyWD8NamedCRC0xA0000000Sigs +    5 }, // 0xA0D210A9
    { 0x00C2, 0x00E2, gabyWD8NamedCRC0xA1000000Sigs +    0 }, // 0xA108855A
    { 0x00C3, 0x00E3, gabyWD8NamedCRC0xA1000000Sigs +    5 }, // 0xA1FC5DCF
    { 0x00C4, 0x00E4, gabyWD8NamedCRC0xA4000000Sigs +    0 }, // 0xA40376B4
    { 0x00C5, 0x00E5, gabyWD8NamedCRC0xA4000000Sigs +    5 }, // 0xA445F6C1
    { 0x00C6, 0x00E6, gabyWD8NamedCRC0xA4000000Sigs +   10 }, // 0xA4474EDC
    { 0x00C7, 0x00E7, gabyWD8NamedCRC0xA4000000Sigs +   15 }, // 0xA4ED87DA
    { 0x00C8, 0x00E8, gabyWD8NamedCRC0xA6000000Sigs +    0 }, // 0xA627A78A
    { 0x00C9, 0x00E9, gabyWD8NamedCRC0xA7000000Sigs +    0 }, // 0xA7E049C0
    { 0x00CA, 0x00EA, gabyWD8NamedCRC0xA8000000Sigs +    0 }, // 0xA847977C
    { 0x00CB, 0x00EB, gabyWD8NamedCRC0xA8000000Sigs +    5 }, // 0xA8EC1294
    { 0x00CC, 0x00EC, gabyWD8NamedCRC0xA9000000Sigs +    0 }, // 0xA92142D3
    { 0x00CD, 0x00ED, gabyWD8NamedCRC0xA9000000Sigs +    5 }, // 0xA96AB51D
    { 0x00CE, 0x00EE, gabyWD8NamedCRC0xA9000000Sigs +   10 }, // 0xA98FF8B8
    { 0x00CF, 0x00EF, gabyWD8NamedCRC0xAA000000Sigs +    0 }, // 0xAA2BC12C
    { 0x00D0, 0x00F0, gabyWD8NamedCRC0xAA000000Sigs +    5 }, // 0xAAAC883D
    { 0x00D1, 0x00F1, gabyWD8NamedCRC0xAA000000Sigs +   10 }, // 0xAAAE1AD6
    { 0x00D2, 0x00F2, gabyWD8NamedCRC0xAA000000Sigs +   15 }, // 0xAAD73768
    { 0x00D3, 0x00F3, gabyWD8NamedCRC0xAB000000Sigs +    0 }, // 0xAB49DE61
    { 0x00D4, 0x00F4, gabyWD8NamedCRC0xAB000000Sigs +    5 }, // 0xABC8998B
    { 0x00D5, 0x00F5, gabyWD8NamedCRC0xAD000000Sigs +    0 }, // 0xAD8DC036
    { 0x00D6, 0x00F6, gabyWD8NamedCRC0xAE000000Sigs +    0 }, // 0xAE196DAA
    { 0x00D7, 0x00F8, gabyWD8NamedCRC0xAE000000Sigs +    9 }, // 0xAEB347C9
    { 0x00D8, 0x00F9, gabyWD8NamedCRC0xAE000000Sigs +   14 }, // 0xAEE09BCD
    { 0x00D9, 0x00FA, gabyWD8NamedCRC0xAE000000Sigs +   19 }, // 0xAEEE95A6
    { 0x00DA, 0x00FB, gabyWD8NamedCRC0xAF000000Sigs +    0 }, // 0xAFDC0624
    { 0x00DB, 0x00FC, gabyWD8NamedCRC0xB0000000Sigs +    0 }, // 0xB085E3BE
    { 0x00DC, 0x00FD, gabyWD8NamedCRC0xB0000000Sigs +    5 }, // 0xB0C6CDBF
    { 0x00DD, 0x00FE, gabyWD8NamedCRC0xB2000000Sigs +    0 }, // 0xB2536191
    { 0x00DE, 0x00FF, gabyWD8NamedCRC0xB2000000Sigs +    5 }, // 0xB2A346CD
    { 0x00DF, 0x0100, gabyWD8NamedCRC0xB2000000Sigs +   10 }, // 0xB2EA2B98
    { 0x00E0, 0x0101, gabyWD8NamedCRC0xB4000000Sigs +    0 }, // 0xB4087D28
    { 0x00E1, 0x0102, gabyWD8NamedCRC0xB4000000Sigs +    5 }, // 0xB4610DF5
    { 0x00E2, 0x0103, gabyWD8NamedCRC0xB5000000Sigs +    0 }, // 0xB53710EE
    { 0x00E3, 0x0104, gabyWD8NamedCRC0xB6000000Sigs +    0 }, // 0xB6CCC4C2
    { 0x00E4, 0x0105, gabyWD8NamedCRC0xB6000000Sigs +    5 }, // 0xB6CD073F
    { 0x00E5, 0x0106, gabyWD8NamedCRC0xB7000000Sigs +    0 }, // 0xB716EF79
    { 0x00E6, 0x0108, gabyWD8NamedCRC0xB9000000Sigs +    0 }, // 0xB94A7368
    { 0x00E7, 0x0109, gabyWD8NamedCRC0xB9000000Sigs +    5 }, // 0xB97F12A6
    { 0x00E8, 0x010A, gabyWD8NamedCRC0xBA000000Sigs +    0 }, // 0xBA208083
    { 0x00E9, 0x010B, gabyWD8NamedCRC0xBA000000Sigs +    5 }, // 0xBA9BADBD
    { 0x00EA, 0x010C, gabyWD8NamedCRC0xBB000000Sigs +    0 }, // 0xBB302507
    { 0x00EB, 0x010D, gabyWD8NamedCRC0xBC000000Sigs +    0 }, // 0xBC7D3F95
    { 0x00EC, 0x010F, gabyWD8NamedCRC0xBD000000Sigs +    0 }, // 0xBD43422C
    { 0x00ED, 0x0110, gabyWD8NamedCRC0xBD000000Sigs +    5 }, // 0xBD7C7683
    { 0x00EE, 0x0111, gabyWD8NamedCRC0xBD000000Sigs +   10 }, // 0xBD7CD06C
    { 0x00EF, 0x0113, gabyWD8NamedCRC0xBD000000Sigs +   19 }, // 0xBDC45C7B
    { 0x00F0, 0x0114, gabyWD8NamedCRC0xBF000000Sigs +    0 }, // 0xBFE15561
    { 0x00F1, 0x0115, gabyWD8NamedCRC0xC0000000Sigs +    0 }, // 0xC01D752C
    { 0x00F2, 0x0116, gabyWD8NamedCRC0xC0000000Sigs +    5 }, // 0xC0B0E4BE
    { 0x00F3, 0x0117, gabyWD8NamedCRC0xC0000000Sigs +   10 }, // 0xC0FE5BC1
    { 0x00F4, 0x0118, gabyWD8NamedCRC0xC3000000Sigs +    0 }, // 0xC32D535D
    { 0x00F5, 0x0119, gabyWD8NamedCRC0xC3000000Sigs +    5 }, // 0xC3693DA5
    { 0x00F6, 0x011A, gabyWD8NamedCRC0xC3000000Sigs +   10 }, // 0xC39F94A5
    { 0x00F7, 0x011B, gabyWD8NamedCRC0xC7000000Sigs +    0 }, // 0xC70F7243
    { 0x00F8, 0x011C, gabyWD8NamedCRC0xC7000000Sigs +    5 }, // 0xC718A894
    { 0x00F9, 0x011D, gabyWD8NamedCRC0xC7000000Sigs +   10 }, // 0xC7BA7DE1
    { 0x00FA, 0x011E, gabyWD8NamedCRC0xC8000000Sigs +    0 }, // 0xC86D4358
    { 0x00FB, 0x0120, gabyWD8NamedCRC0xCA000000Sigs +    0 }, // 0xCA09E75F
    { 0x00FC, 0x0121, gabyWD8NamedCRC0xCA000000Sigs +    5 }, // 0xCAF24F0B
    { 0x00FD, 0x0122, gabyWD8NamedCRC0xCB000000Sigs +    0 }, // 0xCBF75AD9
    { 0x00FE, 0x0123, gabyWD8NamedCRC0xCC000000Sigs +    0 }, // 0xCC107359
    { 0x00FF, 0x0124, gabyWD8NamedCRC0xCC000000Sigs +    5 }, // 0xCCA193BB
    { 0x0100, 0x0125, gabyWD8NamedCRC0xCF000000Sigs +    0 }, // 0xCFE14FB5
    { 0x0101, 0x0126, gabyWD8NamedCRC0xD0000000Sigs +    0 }, // 0xD03D7094
    { 0x0102, 0x0127, gabyWD8NamedCRC0xD0000000Sigs +    5 }, // 0xD072568E
    { 0x0103, 0x0128, gabyWD8NamedCRC0xD2000000Sigs +    0 }, // 0xD20B157C
    { 0x0104, 0x0129, gabyWD8NamedCRC0xD2000000Sigs +    5 }, // 0xD290F69D
    { 0x0105, 0x012A, gabyWD8NamedCRC0xD3000000Sigs +    0 }, // 0xD3ACC3B8
    { 0x0106, 0x012B, gabyWD8NamedCRC0xD3000000Sigs +    5 }, // 0xD3BD18AF
    { 0x0107, 0x012C, gabyWD8NamedCRC0xD4000000Sigs +    0 }, // 0xD49C49ED
    { 0x0108, 0x0130, gabyWD8NamedCRC0xD4000000Sigs +   17 }, // 0xD4FBBE09
    { 0x0109, 0x0132, gabyWD8NamedCRC0xD6000000Sigs +    0 }, // 0xD64F9C5B
    { 0x010A, 0x0133, gabyWD8NamedCRC0xD7000000Sigs +    0 }, // 0xD7698BEA
    { 0x010B, 0x0134, gabyWD8NamedCRC0xD7000000Sigs +    5 }, // 0xD78D9800
    { 0x010C, 0x0135, gabyWD8NamedCRC0xD8000000Sigs +    0 }, // 0xD8999A64
    { 0x010D, 0x0136, gabyWD8NamedCRC0xD9000000Sigs +    0 }, // 0xD92A267A
    { 0x010E, 0x0137, gabyWD8NamedCRC0xD9000000Sigs +    5 }, // 0xD99A666F
    { 0x010F, 0x0138, gabyWD8NamedCRC0xDA000000Sigs +    0 }, // 0xDA895C9C
    { 0x0110, 0x0139, gabyWD8NamedCRC0xDA000000Sigs +    5 }, // 0xDAB76A43
    { 0x0111, 0x013A, gabyWD8NamedCRC0xDB000000Sigs +    0 }, // 0xDB5B9B98
    { 0x0112, 0x013B, gabyWD8NamedCRC0xDB000000Sigs +    5 }, // 0xDBCC0D79
    { 0x0113, 0x013D, gabyWD8NamedCRC0xDB000000Sigs +   14 }, // 0xDBFADCE3
    { 0x0114, 0x013E, gabyWD8NamedCRC0xDC000000Sigs +    0 }, // 0xDC0BB267
    { 0x0115, 0x013F, gabyWD8NamedCRC0xDD000000Sigs +    0 }, // 0xDDD65870
    { 0x0116, 0x0140, gabyWD8NamedCRC0xDE000000Sigs +    0 }, // 0xDE3EF34A
    { 0x0117, 0x0141, gabyWD8NamedCRC0xDE000000Sigs +    5 }, // 0xDEC36955
    { 0x0118, 0x0143, gabyWD8NamedCRC0xE0000000Sigs +    0 }, // 0xE069CE59
    { 0x0119, 0x0144, gabyWD8NamedCRC0xE1000000Sigs +    0 }, // 0xE139949B
    { 0x011A, 0x0145, gabyWD8NamedCRC0xE1000000Sigs +    5 }, // 0xE1D834FA
    { 0x011B, 0x0146, gabyWD8NamedCRC0xE4000000Sigs +    0 }, // 0xE4D6F956
    { 0x011C, 0x0147, gabyWD8NamedCRC0xE6000000Sigs +    0 }, // 0xE6554C06
    { 0x011D, 0x0148, gabyWD8NamedCRC0xE6000000Sigs +    5 }, // 0xE67471C3
    { 0x011E, 0x0149, gabyWD8NamedCRC0xE7000000Sigs +    0 }, // 0xE7965AA6
    { 0x011F, 0x014A, gabyWD8NamedCRC0xE7000000Sigs +    5 }, // 0xE7980039
    { 0x0120, 0x014B, gabyWD8NamedCRC0xE9000000Sigs +    0 }, // 0xE93CD133
    { 0x0121, 0x014C, gabyWD8NamedCRC0xE9000000Sigs +    5 }, // 0xE9DCE3D5
    { 0x0122, 0x014D, gabyWD8NamedCRC0xEA000000Sigs +    0 }, // 0xEACD2F51
    { 0x0123, 0x014E, gabyWD8NamedCRC0xEB000000Sigs +    0 }, // 0xEB16E71B
    { 0x0124, 0x014F, gabyWD8NamedCRC0xEB000000Sigs +    5 }, // 0xEB436D3A
    { 0x0125, 0x0150, gabyWD8NamedCRC0xEB000000Sigs +   10 }, // 0xEB65FD8E
    { 0x0126, 0x0151, gabyWD8NamedCRC0xEC000000Sigs +    0 }, // 0xEC219B8A
    { 0x0127, 0x0152, gabyWD8NamedCRC0xEC000000Sigs +    5 }, // 0xEC676034
    { 0x0128, 0x0153, gabyWD8NamedCRC0xF1000000Sigs +    0 }, // 0xF1A3335F
    { 0x0129, 0x0154, gabyWD8NamedCRC0xF1000000Sigs +    5 }, // 0xF1C3772C
    { 0x012A, 0x0155, gabyWD8NamedCRC0xF2000000Sigs +    0 }, // 0xF2419ECF
    { 0x012B, 0x0156, gabyWD8NamedCRC0xF2000000Sigs +    5 }, // 0xF24BC294
    { 0x012C, 0x0157, gabyWD8NamedCRC0xF3000000Sigs +    0 }, // 0xF31D20FD
    { 0x012D, 0x0159, gabyWD8NamedCRC0xF3000000Sigs +    9 }, // 0xF37EA2E3
    { 0x012E, 0x015A, gabyWD8NamedCRC0xF4000000Sigs +    0 }, // 0xF424A1C5
    { 0x012F, 0x015B, gabyWD8NamedCRC0xF5000000Sigs +    0 }, // 0xF5839B7B
    { 0x0130, 0x015C, gabyWD8NamedCRC0xF6000000Sigs +    0 }, // 0xF68F95B2
    { 0x0131, 0x015D, gabyWD8NamedCRC0xF6000000Sigs +    5 }, // 0xF6A6362A
    { 0x0132, 0x015E, gabyWD8NamedCRC0xF9000000Sigs +    0 }, // 0xF916B2CE
    { 0x0133, 0x0160, gabyWD8NamedCRC0xF9000000Sigs +    9 }, // 0xF9F201E2
    { 0x0134, 0x0161, gabyWD8NamedCRC0xF9000000Sigs +   14 }, // 0xF9F8D0F6
    { 0x0135, 0x0163, gabyWD8NamedCRC0xFB000000Sigs +    0 }, // 0xFB1867D6
    { 0x0136, 0x0164, gabyWD8NamedCRC0xFB000000Sigs +    5 }, // 0xFB8CDD68
    { 0x0137, 0x0165, gabyWD8NamedCRC0xFD000000Sigs +    0 }, // 0xFD1A876E
    { 0x0138, 0x0166, gabyWD8NamedCRC0xFD000000Sigs +    5 }, // 0xFDC2CFAF
    { 0x0139, 0x0167, gabyWD8NamedCRC0xFD000000Sigs +   10 }, // 0xFDC5A4F0
    { 0x013A, 0x0169, gabyWD8NamedCRC0xFD000000Sigs +   19 }, // 0xFDCAF6F4
    { 0x013B, 0x016D, gabyWD8NamedCRC0xFE000000Sigs +    0 }, // 0xFE478B42
    { 0x013C, 0x016E, gabyWD8NamedCRC0xFE000000Sigs +    5 }, // 0xFEAEA366
    { 0x013D, 0x016F, gabyWD8NamedCRC0xFF000000Sigs +    0 }, // 0xFFD41F63
};

///////////////////////////////////////////////////////////////////////////
//
// Macro signatures
//
///////////////////////////////////////////////////////////////////////////

BYTE FAR gabyWD8MacroSetOffsetSigs[] =
{
    0x00
};

BYTE FAR gabyWD8MacroEndOffsetSigs[] =
{
    0x00
};

BYTE FAR gabyWD8MacroSigFullSet[] =
{
    0x00
};

WD_SIG_INFO_T FAR gastWD8RawMacroSigIndex[] =
{
    { 0, NULL }
};

BYTE FAR gabyWD8MacroSigRawByteHit[256] =
{
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x00
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x08
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x10
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x18
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x20
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x28
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x30
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x38
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x40
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x48
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x50
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x58
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x60
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x68
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x70
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x78
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x80
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x88
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x90
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x98
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xA0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xA8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xB0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xB8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xC0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xC8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xD0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xD8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xE0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xE8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xF0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xF8
};

///////////////////////////////////////////////////////////////////////////
//
// CRC array
//
///////////////////////////////////////////////////////////////////////////

DWORD FAR gadwWD8CRCs[] =
{
    0x0046F610,    // CRC_ID: 0x0000
    0x00744F85,    // CRC_ID: 0x0001
    0x0074A804,    // CRC_ID: 0x0002
    0x0215B17F,    // CRC_ID: 0x0003
    0x0328F2CE,    // CRC_ID: 0x0004
    0x03C1E4B9,    // CRC_ID: 0x0005
    0x04043AFF,    // CRC_ID: 0x0006
    0x05BC7AC4,    // CRC_ID: 0x0007
    0x06330781,    // CRC_ID: 0x0008
    0x06B98B39,    // CRC_ID: 0x0009
    0x09BC7CA5,    // CRC_ID: 0x000A
    0x0B458673,    // CRC_ID: 0x000B
    0x0B537BF4,    // CRC_ID: 0x000C
    0x0E32D985,    // CRC_ID: 0x000D
    0x0EA7C757,    // CRC_ID: 0x000E
    0x0EBB1156,    // CRC_ID: 0x000F
    0x0F65DFC6,    // CRC_ID: 0x0010
    0x0F93CBFD,    // CRC_ID: 0x0011
    0x0FD040FC,    // CRC_ID: 0x0012
    0x10E6D600,    // CRC_ID: 0x0013
    0x1210550D,    // CRC_ID: 0x0014
    0x13786871,    // CRC_ID: 0x0015
    0x148C1E87,    // CRC_ID: 0x0016
    0x15165FA7,    // CRC_ID: 0x0017
    0x157871C9,    // CRC_ID: 0x0018
    0x17028C90,    // CRC_ID: 0x0019
    0x172A3A16,    // CRC_ID: 0x001A
    0x17B48CD9,    // CRC_ID: 0x001B
    0x18958CE9,    // CRC_ID: 0x001C
    0x196767D1,    // CRC_ID: 0x001D
    0x1A20CD84,    // CRC_ID: 0x001E
    0x1AD04632,    // CRC_ID: 0x001F
    0x1B1E0DA3,    // CRC_ID: 0x0020
    0x1BE90DC4,    // CRC_ID: 0x0021
    0x1C3DE162,    // CRC_ID: 0x0022
    0x1C7E2053,    // CRC_ID: 0x0023
    0x1CEEB062,    // CRC_ID: 0x0024
    0x1E25C00B,    // CRC_ID: 0x0025
    0x1F82C663,    // CRC_ID: 0x0026
    0x1FA89B22,    // CRC_ID: 0x0027
    0x2087F0D4,    // CRC_ID: 0x0028
    0x21B276D8,    // CRC_ID: 0x0029
    0x21FD466F,    // CRC_ID: 0x002A
    0x224337BC,    // CRC_ID: 0x002B
    0x22C1BFFB,    // CRC_ID: 0x002C
    0x237C8A14,    // CRC_ID: 0x002D
    0x2404ABAB,    // CRC_ID: 0x002E
    0x245B764F,    // CRC_ID: 0x002F
    0x2647411A,    // CRC_ID: 0x0030
    0x26D83252,    // CRC_ID: 0x0031
    0x272B5519,    // CRC_ID: 0x0032
    0x281B1454,    // CRC_ID: 0x0033
    0x2A324BE3,    // CRC_ID: 0x0034
    0x2B2B7235,    // CRC_ID: 0x0035
    0x2C5A32DB,    // CRC_ID: 0x0036
    0x2D14CB4C,    // CRC_ID: 0x0037
    0x2D20F031,    // CRC_ID: 0x0038
    0x3159C164,    // CRC_ID: 0x0039
    0x325619D5,    // CRC_ID: 0x003A
    0x326EFB88,    // CRC_ID: 0x003B
    0x343FAEC5,    // CRC_ID: 0x003C
    0x347D1E28,    // CRC_ID: 0x003D
    0x3499C680,    // CRC_ID: 0x003E
    0x35246CF0,    // CRC_ID: 0x003F
    0x356858D7,    // CRC_ID: 0x0040
    0x35845FC4,    // CRC_ID: 0x0041
    0x35D1B21B,    // CRC_ID: 0x0042
    0x37426D3D,    // CRC_ID: 0x0043
    0x37C58617,    // CRC_ID: 0x0044
    0x38885F16,    // CRC_ID: 0x0045
    0x39137649,    // CRC_ID: 0x0046
    0x396BE61E,    // CRC_ID: 0x0047
    0x39B61B07,    // CRC_ID: 0x0048
    0x3CBEC272,    // CRC_ID: 0x0049
    0x3CC627F9,    // CRC_ID: 0x004A
    0x4082B807,    // CRC_ID: 0x004B
    0x40851923,    // CRC_ID: 0x004C
    0x41670A52,    // CRC_ID: 0x004D
    0x41A05500,    // CRC_ID: 0x004E
    0x41C83079,    // CRC_ID: 0x004F
    0x4268723C,    // CRC_ID: 0x0050
    0x430B531A,    // CRC_ID: 0x0051
    0x430CA51B,    // CRC_ID: 0x0052
    0x435A5D4A,    // CRC_ID: 0x0053
    0x43D94616,    // CRC_ID: 0x0054
    0x4615A075,    // CRC_ID: 0x0055
    0x463C3A23,    // CRC_ID: 0x0056
    0x4781CD20,    // CRC_ID: 0x0057
    0x48FD1260,    // CRC_ID: 0x0058
    0x4937D907,    // CRC_ID: 0x0059
    0x49DD12D5,    // CRC_ID: 0x005A
    0x49DDD82F,    // CRC_ID: 0x005B
    0x4AAA431F,    // CRC_ID: 0x005C
    0x4AFC2D9D,    // CRC_ID: 0x005D
    0x4B5BD6DE,    // CRC_ID: 0x005E
    0x4C0DE0F7,    // CRC_ID: 0x005F
    0x4C2456D9,    // CRC_ID: 0x0060
    0x4D925A10,    // CRC_ID: 0x0061
    0x4DEE7C12,    // CRC_ID: 0x0062
    0x4F21D215,    // CRC_ID: 0x0063
    0x4F2CFE3C,    // CRC_ID: 0x0064
    0x4FF00686,    // CRC_ID: 0x0065
    0x517A855C,    // CRC_ID: 0x0066
    0x52C6931F,    // CRC_ID: 0x0067
    0x53D1DB4E,    // CRC_ID: 0x0068
    0x542FFB05,    // CRC_ID: 0x0069
    0x5465E1ED,    // CRC_ID: 0x006A
    0x548BA77B,    // CRC_ID: 0x006B
    0x54CD4B41,    // CRC_ID: 0x006C
    0x57D82050,    // CRC_ID: 0x006D
    0x57E820DB,    // CRC_ID: 0x006E
    0x58097C7A,    // CRC_ID: 0x006F
    0x58A05FE1,    // CRC_ID: 0x0070
    0x5A0F4C02,    // CRC_ID: 0x0071
    0x5A1373D8,    // CRC_ID: 0x0072
    0x5AD9E8B9,    // CRC_ID: 0x0073
    0x5ADFE684,    // CRC_ID: 0x0074
    0x5B1E704F,    // CRC_ID: 0x0075
    0x5D05D6D2,    // CRC_ID: 0x0076
    0x6089B16C,    // CRC_ID: 0x0077
    0x60BF46C7,    // CRC_ID: 0x0078
    0x60D12F54,    // CRC_ID: 0x0079
    0x61666B40,    // CRC_ID: 0x007A
    0x6189B36A,    // CRC_ID: 0x007B
    0x62DE6342,    // CRC_ID: 0x007C
    0x64E6B43A,    // CRC_ID: 0x007D
    0x650E5AA2,    // CRC_ID: 0x007E
    0x659C75A2,    // CRC_ID: 0x007F
    0x65C95634,    // CRC_ID: 0x0080
    0x664CACA4,    // CRC_ID: 0x0081
    0x67C0019C,    // CRC_ID: 0x0082
    0x68569621,    // CRC_ID: 0x0083
    0x686D4C4E,    // CRC_ID: 0x0084
    0x68BEAB13,    // CRC_ID: 0x0085
    0x6948E2D7,    // CRC_ID: 0x0086
    0x69FA2799,    // CRC_ID: 0x0087
    0x6AABC61A,    // CRC_ID: 0x0088
    0x6B7EB918,    // CRC_ID: 0x0089
    0x6BBF9C56,    // CRC_ID: 0x008A
    0x6CC37630,    // CRC_ID: 0x008B
    0x6D579FA6,    // CRC_ID: 0x008C
    0x6E485EA0,    // CRC_ID: 0x008D
    0x6F231812,    // CRC_ID: 0x008E
    0x6F4B9E57,    // CRC_ID: 0x008F
    0x6F750333,    // CRC_ID: 0x0090
    0x723244B1,    // CRC_ID: 0x0091
    0x73AC0611,    // CRC_ID: 0x0092
    0x7441FFFF,    // CRC_ID: 0x0093
    0x74BAC13B,    // CRC_ID: 0x0094
    0x75BD8500,    // CRC_ID: 0x0095
    0x77D97129,    // CRC_ID: 0x0096
    0x7A36CBE1,    // CRC_ID: 0x0097
    0x7B9192B7,    // CRC_ID: 0x0098
    0x7D3A2B3E,    // CRC_ID: 0x0099
    0x7D45340C,    // CRC_ID: 0x009A
    0x7DCF4735,    // CRC_ID: 0x009B
    0x7DFEC194,    // CRC_ID: 0x009C
    0x7EB12C09,    // CRC_ID: 0x009D
    0x7EF24DD5,    // CRC_ID: 0x009E
    0x7F9F565C,    // CRC_ID: 0x009F
    0x80F4B7A7,    // CRC_ID: 0x00A0
    0x8286F05D,    // CRC_ID: 0x00A1
    0x834E0255,    // CRC_ID: 0x00A2
    0x83E29739,    // CRC_ID: 0x00A3
    0x868AEF3F,    // CRC_ID: 0x00A4
    0x895D4671,    // CRC_ID: 0x00A5
    0x89BCEFB5,    // CRC_ID: 0x00A6
    0x8A581A55,    // CRC_ID: 0x00A7
    0x8B9B8BB7,    // CRC_ID: 0x00A8
    0x8BA9129A,    // CRC_ID: 0x00A9
    0x8D4F994C,    // CRC_ID: 0x00AA
    0x8E898EB3,    // CRC_ID: 0x00AB
    0x8EE5CA7A,    // CRC_ID: 0x00AC
    0x8FD6310E,    // CRC_ID: 0x00AD
    0x904866E2,    // CRC_ID: 0x00AE
    0x90D70869,    // CRC_ID: 0x00AF
    0x90ED5321,    // CRC_ID: 0x00B0
    0x91D1CF78,    // CRC_ID: 0x00B1
    0x9533CD79,    // CRC_ID: 0x00B2
    0x958C9963,    // CRC_ID: 0x00B3
    0x96B593EF,    // CRC_ID: 0x00B4
    0x9704022A,    // CRC_ID: 0x00B5
    0x9712041C,    // CRC_ID: 0x00B6
    0x9821C630,    // CRC_ID: 0x00B7
    0x98545051,    // CRC_ID: 0x00B8
    0x9A52CD9D,    // CRC_ID: 0x00B9
    0x9A64AD27,    // CRC_ID: 0x00BA
    0x9AC5717E,    // CRC_ID: 0x00BB
    0x9AEA8C98,    // CRC_ID: 0x00BC
    0x9BE64AE8,    // CRC_ID: 0x00BD
    0x9E086C35,    // CRC_ID: 0x00BE
    0x9E14BFFA,    // CRC_ID: 0x00BF
    0xA0694EC9,    // CRC_ID: 0x00C0
    0xA0D210A9,    // CRC_ID: 0x00C1
    0xA108855A,    // CRC_ID: 0x00C2
    0xA1FC5DCF,    // CRC_ID: 0x00C3
    0xA40376B4,    // CRC_ID: 0x00C4
    0xA445F6C1,    // CRC_ID: 0x00C5
    0xA4474EDC,    // CRC_ID: 0x00C6
    0xA4ED87DA,    // CRC_ID: 0x00C7
    0xA627A78A,    // CRC_ID: 0x00C8
    0xA7E049C0,    // CRC_ID: 0x00C9
    0xA847977C,    // CRC_ID: 0x00CA
    0xA8EC1294,    // CRC_ID: 0x00CB
    0xA92142D3,    // CRC_ID: 0x00CC
    0xA96AB51D,    // CRC_ID: 0x00CD
    0xA98FF8B8,    // CRC_ID: 0x00CE
    0xAA2BC12C,    // CRC_ID: 0x00CF
    0xAAAC883D,    // CRC_ID: 0x00D0
    0xAAAE1AD6,    // CRC_ID: 0x00D1
    0xAAD73768,    // CRC_ID: 0x00D2
    0xAB49DE61,    // CRC_ID: 0x00D3
    0xABC8998B,    // CRC_ID: 0x00D4
    0xAD8DC036,    // CRC_ID: 0x00D5
    0xAE196DAA,    // CRC_ID: 0x00D6
    0xAEB347C9,    // CRC_ID: 0x00D7
    0xAEE09BCD,    // CRC_ID: 0x00D8
    0xAEEE95A6,    // CRC_ID: 0x00D9
    0xAFDC0624,    // CRC_ID: 0x00DA
    0xB085E3BE,    // CRC_ID: 0x00DB
    0xB0C6CDBF,    // CRC_ID: 0x00DC
    0xB2536191,    // CRC_ID: 0x00DD
    0xB2A346CD,    // CRC_ID: 0x00DE
    0xB2EA2B98,    // CRC_ID: 0x00DF
    0xB4087D28,    // CRC_ID: 0x00E0
    0xB4610DF5,    // CRC_ID: 0x00E1
    0xB53710EE,    // CRC_ID: 0x00E2
    0xB6CCC4C2,    // CRC_ID: 0x00E3
    0xB6CD073F,    // CRC_ID: 0x00E4
    0xB716EF79,    // CRC_ID: 0x00E5
    0xB94A7368,    // CRC_ID: 0x00E6
    0xB97F12A6,    // CRC_ID: 0x00E7
    0xBA208083,    // CRC_ID: 0x00E8
    0xBA9BADBD,    // CRC_ID: 0x00E9
    0xBB302507,    // CRC_ID: 0x00EA
    0xBC7D3F95,    // CRC_ID: 0x00EB
    0xBD43422C,    // CRC_ID: 0x00EC
    0xBD7C7683,    // CRC_ID: 0x00ED
    0xBD7CD06C,    // CRC_ID: 0x00EE
    0xBDC45C7B,    // CRC_ID: 0x00EF
    0xBFE15561,    // CRC_ID: 0x00F0
    0xC01D752C,    // CRC_ID: 0x00F1
    0xC0B0E4BE,    // CRC_ID: 0x00F2
    0xC0FE5BC1,    // CRC_ID: 0x00F3
    0xC32D535D,    // CRC_ID: 0x00F4
    0xC3693DA5,    // CRC_ID: 0x00F5
    0xC39F94A5,    // CRC_ID: 0x00F6
    0xC70F7243,    // CRC_ID: 0x00F7
    0xC718A894,    // CRC_ID: 0x00F8
    0xC7BA7DE1,    // CRC_ID: 0x00F9
    0xC86D4358,    // CRC_ID: 0x00FA
    0xCA09E75F,    // CRC_ID: 0x00FB
    0xCAF24F0B,    // CRC_ID: 0x00FC
    0xCBF75AD9,    // CRC_ID: 0x00FD
    0xCC107359,    // CRC_ID: 0x00FE
    0xCCA193BB,    // CRC_ID: 0x00FF
    0xCFE14FB5,    // CRC_ID: 0x0100
    0xD03D7094,    // CRC_ID: 0x0101
    0xD072568E,    // CRC_ID: 0x0102
    0xD20B157C,    // CRC_ID: 0x0103
    0xD290F69D,    // CRC_ID: 0x0104
    0xD3ACC3B8,    // CRC_ID: 0x0105
    0xD3BD18AF,    // CRC_ID: 0x0106
    0xD49C49ED,    // CRC_ID: 0x0107
    0xD4FBBE09,    // CRC_ID: 0x0108
    0xD64F9C5B,    // CRC_ID: 0x0109
    0xD7698BEA,    // CRC_ID: 0x010A
    0xD78D9800,    // CRC_ID: 0x010B
    0xD8999A64,    // CRC_ID: 0x010C
    0xD92A267A,    // CRC_ID: 0x010D
    0xD99A666F,    // CRC_ID: 0x010E
    0xDA895C9C,    // CRC_ID: 0x010F
    0xDAB76A43,    // CRC_ID: 0x0110
    0xDB5B9B98,    // CRC_ID: 0x0111
    0xDBCC0D79,    // CRC_ID: 0x0112
    0xDBFADCE3,    // CRC_ID: 0x0113
    0xDC0BB267,    // CRC_ID: 0x0114
    0xDDD65870,    // CRC_ID: 0x0115
    0xDE3EF34A,    // CRC_ID: 0x0116
    0xDEC36955,    // CRC_ID: 0x0117
    0xE069CE59,    // CRC_ID: 0x0118
    0xE139949B,    // CRC_ID: 0x0119
    0xE1D834FA,    // CRC_ID: 0x011A
    0xE4D6F956,    // CRC_ID: 0x011B
    0xE6554C06,    // CRC_ID: 0x011C
    0xE67471C3,    // CRC_ID: 0x011D
    0xE7965AA6,    // CRC_ID: 0x011E
    0xE7980039,    // CRC_ID: 0x011F
    0xE93CD133,    // CRC_ID: 0x0120
    0xE9DCE3D5,    // CRC_ID: 0x0121
    0xEACD2F51,    // CRC_ID: 0x0122
    0xEB16E71B,    // CRC_ID: 0x0123
    0xEB436D3A,    // CRC_ID: 0x0124
    0xEB65FD8E,    // CRC_ID: 0x0125
    0xEC219B8A,    // CRC_ID: 0x0126
    0xEC676034,    // CRC_ID: 0x0127
    0xF1A3335F,    // CRC_ID: 0x0128
    0xF1C3772C,    // CRC_ID: 0x0129
    0xF2419ECF,    // CRC_ID: 0x012A
    0xF24BC294,    // CRC_ID: 0x012B
    0xF31D20FD,    // CRC_ID: 0x012C
    0xF37EA2E3,    // CRC_ID: 0x012D
    0xF424A1C5,    // CRC_ID: 0x012E
    0xF5839B7B,    // CRC_ID: 0x012F
    0xF68F95B2,    // CRC_ID: 0x0130
    0xF6A6362A,    // CRC_ID: 0x0131
    0xF916B2CE,    // CRC_ID: 0x0132
    0xF9F201E2,    // CRC_ID: 0x0133
    0xF9F8D0F6,    // CRC_ID: 0x0134
    0xFB1867D6,    // CRC_ID: 0x0135
    0xFB8CDD68,    // CRC_ID: 0x0136
    0xFD1A876E,    // CRC_ID: 0x0137
    0xFDC2CFAF,    // CRC_ID: 0x0138
    0xFDC5A4F0,    // CRC_ID: 0x0139
    0xFDCAF6F4,    // CRC_ID: 0x013A
    0xFE478B42,    // CRC_ID: 0x013B
    0xFEAEA366,    // CRC_ID: 0x013C
    0xFFD41F63,    // CRC_ID: 0x013D
};

///////////////////////////////////////////////////////////////////////////
//
// Known and unknown virus signatures
//
///////////////////////////////////////////////////////////////////////////

BYTE FAR gabyWD8VirusSigSet0000[] =
{
    //--------------------------------------------------
    // 0x0000 : VID_BANDUNGA
    // WM.Bandung.A    (for Office97) jjw 11 Feb 97

    0x01,
    0xF2, 
    0xCB, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0001 : VID_CONCEPTKK
    // WM.Concept.K:NL

    0x04,
    0x23, 0x28, 0x28, 0xF8, 
    0x09, 0x01, 0xD0, 0x84, 0xA4, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0002 : VID_RAPIA
    // WM.Rapi.A

    0x0C,
    0x23, 0x38, 0x28, 0x38, 0x93, 0x93, 0x33, 0x28, 
    0x38, 0x28, 0x98, 0xF8, 
    0x67, 0x01, 0x55, 0x2D, 0x01, 0x0A, 0x6C, 0x01, 
    0x11, 0x01, 0x6B, 0x01, 0x12, 0x01, 0x68, 0x01, 
    0x56, 0x2E, 0x01, 0x0B, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0003 : VID_RAPIB
    // WM.Rapi.B

    0x0A,
    0x23, 0x38, 0x28, 0x28, 0x22, 0x28, 0x38, 0x28, 
    0x98, 0xF8, 
    0x08, 0x01, 0x55, 0x2D, 0x01, 0x0A, 0x30, 0x9C, 
    0x2A, 0x56, 0x2E, 0x01, 0x0B, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0004 : VID_RAPIC
    // WM.Rapi.C

    0x0A,
    0x23, 0x38, 0x28, 0x28, 0x32, 0x28, 0x38, 0x28, 
    0x98, 0xF8, 
    0x67, 0x01, 0x55, 0x2D, 0x01, 0x0A, 0x30, 0x2A, 
    0x68, 0x01, 0x56, 0x2E, 0x01, 0x0B, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0005 : VID_RAPID
    // WM.Rapi.D

    0x0A,
    0x23, 0x38, 0x28, 0x28, 0x32, 0x28, 0x28, 0x28, 
    0x98, 0xF8, 
    0x67, 0x01, 0x55, 0x2D, 0x01, 0x0A, 0x36, 0xA0, 
    0x68, 0x01, 0x56, 0x20, 0x09, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0006 : VID_RAPIF
    // WM.Rapi.F

    0x0A,
    0x32, 0x28, 0x28, 0x28, 0x22, 0x38, 0x28, 0x28, 
    0x98, 0xF8, 
    0x55, 0x2D, 0x01, 0x0A, 0xA8, 0x30, 0x2A, 0x56, 
    0x2E, 0x01, 0x0B, 0x13, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0007 : VID_RAPIO
    // WM.Rapi.O          LEE 3/13/97

    0x05,
    0x22, 0x38, 0x38, 0x93, 0xF8, 
    0x55, 0x74, 0x2D, 0x01, 0x6C, 0x01, 0x6B, 0x01, 

    0x01,
    0xF2, 
    0x0A, 

    0x01,
    0xF2, 
    0x56, 

    0x01,
    0xF3, 
    0x2E, 0x01, 

    0x01,
    0xF2, 
    0x0B, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0008 : VID_RAPIA1
    // WM.Rapi.A1

    0x08,
    0x33, 0x28, 0x38, 0x33, 0x38, 0x28, 0x98, 0xF8, 
    0x67, 0x01, 0x2D, 0x01, 0x55, 0x6C, 0x01, 0x6B, 
    0x01, 0x68, 0x01, 0x2E, 0x01, 0x56, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0009 : VID_RAPIB1
    // WM.Rapi.B1

    0x08,
    0x33, 0x28, 0x28, 0x22, 0x38, 0x28, 0x98, 0xF8, 
    0x08, 0x01, 0x2D, 0x01, 0x0A, 0x30, 0x9C, 0x2A, 
    0x2E, 0x01, 0x0B, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x000A : VID_RAPIC1
    // WM.RAPI.C1

    0x08,
    0x33, 0x28, 0x28, 0x32, 0x38, 0x28, 0x98, 0xF8, 
    0x67, 0x01, 0x2D, 0x01, 0x0A, 0x30, 0x2A, 0x68, 
    0x01, 0x2E, 0x01, 0x0B, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x000B : VID_RAPID1
    // WM.Rapi.D1

    0x08,
    0x33, 0x28, 0x28, 0x32, 0x28, 0x28, 0x98, 0xF8, 
    0x67, 0x01, 0x2D, 0x01, 0x55, 0x36, 0xA0, 0x68, 
    0x01, 0x20, 0x56, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x000C : VID_RAPIF1
    // WM.Rapi.F1

    0x08,
    0x23, 0x28, 0x28, 0x32, 0x28, 0x28, 0x98, 0xF8, 
    0x2D, 0x01, 0x0A, 0xA8, 0x30, 0x2A, 0x2E, 0x01, 
    0x0B, 0x13, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x000D : VID_RAPIN1
    // WM.Rapi.N1          jjw 13 Mar 97

    0x03,
    0x33, 0x28, 0xF8, 
    0x0D, 0x01, 0x2D, 0x01, 0x0A, 

    0x01,
    0xFB, 

    0x01,
    0xF3, 
    0x0E, 0x01, 

    0x01,
    0xF3, 
    0x69, 0x01, 

    0x01,
    0xF3, 
    0x2F, 0x01, 

    0x01,
    0xF2, 
    0x08, 

    0x01,
    0xF3, 
    0x6A, 0x01, 

    0x00,

    //--------------------------------------------------
    // 0x000E : VID_RAPIO1
    // WM.Rapi.O1          LEE 3/13/97

    0x04,
    0x32, 0x38, 0x93, 0xF8, 
    0x74, 0x2D, 0x01, 0x6B, 0x01, 0x6C, 0x01, 

    0x01,
    0xFB, 

    0x01,
    0xF2, 
    0x0A, 

    0x01,
    0xF2, 
    0x73, 

    0x01,
    0xF3, 
    0x2E, 0x01, 

    0x01,
    0xF2, 
    0x0B, 

    0x01,
    0xF2, 
    0x0A, 

    0x00,

    //--------------------------------------------------
    // 0x000F : VID_RAPIB2
    // WM.Rapi.B2

    0x04,
    0x33, 0x28, 0x92, 0xF8, 
    0x08, 0x01, 0x2D, 0x01, 0x30, 0x2A, 

    0x01,
    0xFB, 

    0x01,
    0xF2, 
    0x9C, 

    0x01,
    0xF3, 
    0x2E, 0x01, 

    0x00,

    //--------------------------------------------------
    // 0x0010 : VID_RAPID2
    // WM.Rapi.D2 / A2

    0x08,
    0x33, 0x28, 0x93, 0x32, 0x39, 0x38, 0x98, 0xF8, 
    0x67, 0x01, 0x2D, 0x01, 0x36, 0x6C, 0x01, 0xA0, 
    0x6B, 0x01, 0x68, 0x01, 0x2E, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0011 : VID_RAPIE2
    // WM.Rapi.E2

    0x06,
    0x32, 0x28, 0x22, 0x38, 0x98, 0xF8, 
    0x43, 0x2D, 0x01, 0x30, 0xE6, 0x2A, 0x2E, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0012 : VID_RAPIF2
    // WM.Rapi.F2

    0x06,
    0x23, 0x28, 0x22, 0x38, 0x98, 0xF8, 
    0x2D, 0x01, 0xA8, 0x30, 0x2A, 0x13, 0x2E, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0013 : VID_RAPIM2
    // WM.Rapi.M2  renamed on 022897

    0x06,
    0x33, 0x28, 0x92, 0x32, 0x98, 0xF8, 
    0x58, 0x01, 0x2D, 0x01, 0x38, 0xA1, 0x37, 0x2C, 
    0x01, 

    0x01,
    0xFB, 

    0x01,
    0xF3, 
    0x57, 0x01, 

    0x00,

    //--------------------------------------------------
    // 0x0014 : VID_RAPIN2
    // WM.Rapi.N2          jjw 13 Mar 97

    0x02,
    0x33, 0xF8, 
    0x0D, 0x01, 0x2D, 0x01, 

    0x01,
    0xFB, 

    0x01,
    0xF3, 
    0x0E, 0x01, 

    0x01,
    0xF2, 
    0x0A, 

    0x01,
    0xF3, 
    0x69, 0x01, 

    0x01,
    0xF3, 
    0x2F, 0x01, 

    0x01,
    0xF2, 
    0x08, 

    0x01,
    0xF3, 
    0x6A, 0x01, 

    0x00,

    //--------------------------------------------------
    // 0x0015 : VID_RAPIO2
    // WM.Rapi.O2     LEE 3/5/97

    0x04,
    0x32, 0x38, 0x93, 0xF8, 
    0x74, 0x2D, 0x01, 0x6C, 0x01, 0x6B, 0x01, 

    0x01,
    0xFB, 

    0x01,
    0xF2, 
    0x73, 

    0x01,
    0xF3, 
    0x2E, 0x01, 

    0x00,

    //--------------------------------------------------
    // 0x0016 : VID_WAZZUA
    // WM.Wazzu.A

    0x01,
    0xF2, 
    0x41, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0017 : VID_WAZZUH
    // WM.Wazzu.H

    0x01,
    0xF2, 
    0x67, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0018 : VID_WAZZUX
    // WM.Wazzu.X

    0x01,
    0xF2, 
    0xFB, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0019 : VID_WAZZUAH
    // WM.Wazzu.AH

    0x01,
    0xF2, 
    0x9B, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x001A : VID_WAZZUAZ
    // WM.Wazzu.AZ

    0x01,
    0xF3, 
    0x0A, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x001B : VID_WAZZUBA
    // WM.Wazzu.BA

    0x01,
    0xF2, 
    0x98, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x001C : VID_CLOCK
    // WM.Clock:De (for Office97) jjw 11 Feb 97

    0x09,
    0x23, 0x28, 0x38, 0x28, 0x38, 0x38, 0x38, 0x28, 
    0xF8, 
    0x66, 0x01, 0xA2, 0x7A, 0x49, 0x01, 0x8A, 0x00, 
    0x01, 0x46, 0x01, 0x0B, 0x01, 0xD3, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x001D : VID_COLORSA
    // WM.Colors.A&B (for Office97) jjw 11 Feb 97

    0x01,
    0xF3, 
    0x33, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x001E : VID_COLORST
    // WM.Colors.T  .B w/ ShowOff's AUTOOPEN jjw 14 Mar 97

    0x02,
    0x32, 0xF8, 
    0xE2, 0x33, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x001F : VID_NPADC
    // WM.Npad.C

    0x01,
    0xF3, 
    0x4C, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0020 : VID_NPADO
    // WM.Npad.O

    0x01,
    0xF2, 
    0xFE, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0021 : VID_NPADP
    // WM.NPADP

    0x01,
    0xF2, 
    0xC4, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0022 : VID_NPADQ
    // WM.Npad.Q

    0x01,
    0xF3, 
    0x44, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0023 : VID_NPADAC
    // WM.NPAD.AC

    0x01,
    0xF2, 
    0x4D, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0024 : VID_IRISH
    // WM.Irish Docs only are infected

    0x04,
    0x32, 0x28, 0x28, 0xF8, 
    0xD7, 0x62, 0x01, 0x42, 0x22, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0025 : VID_IRISHF
    // WM.Irish.F	14 Mar 97 jjw - A lot like original Irish

    0x08,
    0x32, 0x28, 0x28, 0x38, 0x82, 0x82, 0x82, 0xF9, 
    0xD7, 0x62, 0x01, 0x42, 0xFC, 0x61, 0x01, 0xD8, 
    0x42, 0xFC, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0026 : VID_JOHNNY0
    // WM.Johnny

    0x05,
    0x23, 0x28, 0x28, 0x92, 0xF8, 
    0x3A, 0x01, 0x8F, 0x5F, 0xD1, 0x95, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0027 : VID_MDMAB
    // WM.MDMA.B

    0x01,
    0xF2, 
    0x07, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0028 : VID_SHOWOFF
    // WM.ShowOff

    0x04,
    0x23, 0x28, 0x82, 0xF9, 
    0x60, 0x01, 0x8E, 0xE8, 0x45, 

    0x01,
    0xFB, 

    0x01,
    0xF0, 
    0x0E, 

    0x01,
    0xF0, 
    0x48, 

    0x00,

    //--------------------------------------------------
    // 0x0029 : VID_HYBRID
    // WM.Hybrid

    0x03,
    0x23, 0x28, 0xF8, 
    0x34, 0x01, 0xD5, 0x58, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x002A : VID_ANTICON
    // WM.AntiConcept

    0x05,
    0x33, 0x28, 0x92, 0x28, 0xF8, 
    0x26, 0x01, 0x37, 0x01, 0xDA, 0x26, 0x78, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x002B : VID_ALIEN
    // WM.Alien

    0x03,
    0x32, 0x28, 0xF8, 
    0xDD, 0x40, 0x01, 0xE7, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x002C : VID_MADDOGB
    // WM.Maddog.B

    0x06,
    0x32, 0x28, 0x38, 0x38, 0x28, 0xF8, 
    0xEF, 0x1A, 0x01, 0x90, 0x39, 0x01, 0x22, 0x01, 
    0x6E, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x002D : VID_NUCLEARE
    // WM.Nuclear.E

    0x05,
    0x23, 0x28, 0x28, 0x38, 0xF8, 
    0x05, 0x01, 0x01, 0x75, 0xEE, 0x3F, 0x01, 

    0x01,
    0xFB, 

    0x01,
    0xF0, 
    0x4D, 

    0x00,

    //--------------------------------------------------
    // 0x002E : VID_SWITCHES
    // WM.Switches

    0x02,
    0x32, 0xF8, 
    0x03, 0x1B, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x002F : VID_TARGETB
    // WM.Target.B:De

    0x01,
    0xF3, 
    0x21, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0030 : VID_NUCLEARB
    // WM.Nuclear.B

    0x02,
    0x23, 0xF8, 
    0x03, 0x01, 0xCF, 

    0x01,
    0xFB, 

    0x01,
    0xF0, 
    0x57, 

    0x01,
    0xF0, 
    0x4E, 

    0x01,
    0xF0, 
    0x4F, 

    0x01,
    0xF0, 
    0x62, 

    0x01,
    0xF2, 
    0x2C, 

    0x00,

    //--------------------------------------------------
    // 0x0031 : VID_NUCLEARA
    // WM.Nuclear.A

    0x01,
    0xF2, 
    0x31, 

    0x01,
    0xFB, 

    0x01,
    0xF0, 
    0x4E, 

    0x01,
    0xF0, 
    0x48, 

    0x01,
    0xF0, 
    0x49, 

    0x01,
    0xF0, 
    0x50, 

    0x01,
    0xF0, 
    0x57, 

    0x01,
    0xF0, 
    0x4D, 

    0x01,
    0xF0, 
    0x4F, 

    0x01,
    0xF0, 
    0x62, 

    0x00,

    //--------------------------------------------------
    // 0x0032 : VID_NUCLEARC
    // WM.Nuclear.C

    0x01,
    0xF2, 
    0x31, 

    0x01,
    0xFB, 

    0x01,
    0xF0, 
    0x4E, 

    0x01,
    0xF0, 
    0x48, 

    0x01,
    0xF0, 
    0x49, 

    0x01,
    0xF0, 
    0x50, 

    0x01,
    0xF0, 
    0x57, 

    0x01,
    0xF0, 
    0x4D, 

    0x01,
    0xF0, 
    0x4F, 

    0x01,
    0xF0, 
    0x62, 

    0x00,

    //--------------------------------------------------
    // 0x0033 : VID_HOT
    // WM.Hot  jjw 13 Feb 97

    0x02,
    0x22, 0xF9, 
    0x94, 0x5E, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0034 : VID_TELE
    // WM.Tele:De   jjw 13 Feb 97

    0x07,
    0x23, 0x28, 0x28, 0x38, 0x28, 0x38, 0xF8, 
    0x4A, 0x01, 0x7E, 0x3D, 0x1A, 0x35, 0x01, 0xA5, 
    0x01, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0035 : VID_BOOM
    // WM.Boom:De (Office97)   jjw 11 Feb 97

    0x03,
    0x33, 0x28, 0xF8, 
    0x59, 0x01, 0x13, 0x01, 0x34, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0036 : VID_BIRTHDAY
    // WM.Birthday:De

    0x02,
    0x22, 0xF8, 
    0xD9, 0x5A, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0037 : VID_BUERO
    // WM.Buero:De (for Office97) jjw 11 Feb 97

    0x02,
    0x23, 0xF9, 
    0x53, 0x01, 0xC0, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0038 : VID_GOLDFISH
    // WM.Goldfish

    0x02,
    0x23, 0xF8, 
    0x6F, 0x01, 0xFD, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0039 : VID_HASSLE
    // WM.Hassle

    0x08,
    0x22, 0x28, 0x28, 0x83, 0x82, 0x32, 0x98, 0xF8, 
    0xB3, 0xC2, 0x85, 0x6B, 0x3D, 0x01, 0x1E, 0x1C, 
    0x19, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x003A : VID_EASY
    // WM.Easy

    0x01,
    0xF3, 
    0x43, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x003B : VID_TEDIOUS
    // WM.Tedious

    0x08,
    0x22, 0x28, 0x28, 0x28, 0x82, 0x82, 0x82, 0xF9, 
    0x4F, 0xE1, 0x0C, 0x29, 0xE0, 0x4E, 0x28, 0x0D, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x003C : VID_PHANTOM
    // WM.Phantom

    0x01,
    0xF2, 
    0x57, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x003D : VID_IMPOSTERA
    // WM.Imposter.A

    0x04,
    0x22, 0x28, 0x82, 0xF9, 
    0xC1, 0xAC, 0x70, 0x40, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x003E : VID_OUTLAWA
    // WM.Outlaw.A

    0x03,
    0x22, 0x28, 0xF8, 
    0x83, 0xA3, 0x3F, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x003F : VID_OUTLAWB
    // WM.Outlaw.B

    0x03,
    0x22, 0x28, 0xF8, 
    0x4B, 0xF4, 0xA3, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0040 : VID_MADDOGA
    // WM.MadDog.A

    0x06,
    0x22, 0x38, 0x38, 0x28, 0x28, 0xF8, 
    0xEF, 0x76, 0x45, 0x01, 0x39, 0x01, 0xE5, 0x8D, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0041 : VID_POLITE
    // WM.Polite

    0x02,
    0x32, 0xF8, 
    0x18, 0x4B, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0042 : VID_FRIENDLY
    // WM.Friendly:De

    0x14,
    0x22, 0x38, 0x28, 0x28, 0x28, 0x28, 0x38, 0x28, 
    0x28, 0x28, 0x28, 0x38, 0x28, 0x28, 0x28, 0x28, 
    0x38, 0x28, 0x38, 0xF8, 
    0xC9, 0x99, 0x5D, 0x01, 0x1B, 0xFA, 0xF1, 0x59, 
    0x50, 0x01, 0x1F, 0x24, 0x19, 0xEA, 0x56, 0x01, 
    0xD6, 0x97, 0x3C, 0xBA, 0x02, 0x01, 0x3A, 0x17, 
    0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0043 : VID_SAVER
    // WM.Saver:De

    0x01,
    0xF2, 
    0x2B, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0044 : VID_SPOOKY
    // WM.Spooky:De

    0x0A,
    0x23, 0x28, 0x28, 0x28, 0x28, 0x38, 0x28, 0x92, 
    0x28, 0xF8, 
    0x65, 0x01, 0x16, 0x93, 0x21, 0x33, 0x11, 0x3E, 
    0x01, 0x00, 0x00, 0xDC, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0045 : VID_DIVINAC
    // WM.Divina.C

    0x01,
    0xF2, 
    0xAD, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0046 : VID_DIVINAB
    // WM.Divina.B

    0x01,
    0xF2, 
    0x06, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0047 : VID_DIVINAA
    // WM.Divina.A

    0x01,
    0xF2, 
    0xB6, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0048 : VID_STRYX
    // WM.Stryx:De

    0x06,
    0x33, 0x29, 0x92, 0x38, 0x28, 0xF8, 
    0x1F, 0x01, 0x1E, 0x01, 0xC6, 0xC7, 0x29, 0x01, 
    0x5D, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0049 : VID_GANGSTERZ
    // WM.Gangsterz

    0x04,
    0x22, 0x93, 0x93, 0xF8, 
    0x71, 0x0E, 0x36, 0x01, 0x5C, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x004A : VID_DOGGIE
    // WM.Doggie

    0x02,
    0x22, 0xF9, 
    0x44, 0x8C, 

    0x01,
    0xFB, 

    0x01,
    0xF3, 
    0x2A, 0x01, 

    0x00,

    //--------------------------------------------------
    // 0x004B : VID_KILLDLL
    // WM.KillDLL

    0x01,
    0xF2, 
    0x50, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x004C : VID_SATANIC
    // WM.Satanic

    0x04,
    0x22, 0x38, 0x28, 0xF8, 
    0x35, 0xEC, 0x5A, 0x01, 0x02, 

    0x01,
    0xFB, 

    0x01,
    0xF0, 
    0x46, 

    0x00,

    //--------------------------------------------------
    // 0x004D : VID_DATE
    // WM.Date

    0x01,
    0xF3, 
    0x6E, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x004E : VID_LUNCHB
    // WM.Lunch.B for Office 97    jjw 13 Feb 97

    0x02,
    0x22, 0xF8, 
    0x2F, 0x48, 

    0x01,
    0xFB, 

    0x01,
    0xF2, 
    0x2E, 

    0x01,
    0xF2, 
    0x47, 

    0x00,

    //--------------------------------------------------
    // 0x004F : VID_LUNCHA
    // WM.Lunch.A

    0x06,
    0x22, 0x28, 0x28, 0x82, 0x82, 0xF9, 
    0xF0, 0xEB, 0x1D, 0xF0, 0xEB, 0x14, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0050 : VID_APPDER
    // WM.Appder

    0x05,
    0x23, 0x38, 0x38, 0x83, 0xF9, 
    0x16, 0x01, 0x5C, 0x4D, 0x01, 0x16, 0x01, 0x4D, 
    0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0051 : VID_NFB
    // WM.NF.B

    0x02,
    0x33, 0xF8, 
    0x52, 0x01, 0x14, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0052 : VID_DARKA
    // WM.Dark.A

    0x04,
    0x22, 0x28, 0x38, 0xF8, 
    0x27, 0x4C, 0x4A, 0x38, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0053 : VID_NFA
    // WM.NF.A

    0x04,
    0x33, 0x28, 0x83, 0xF9, 
    0x52, 0x01, 0x18, 0x01, 0xDF, 0x14, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0054 : VID_SWLABSB
    // WM.Swlabs.B

    0x01,
    0xF2, 
    0xC3, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0055 : VID_DZTA
    // WM.Dzt.A

    0x01,
    0xF3, 
    0x04, 0x01, 

    0x01,
    0xFB, 

    0x01,
    0xF2, 
    0x9A, 

    0x01,
    0xF2, 
    0x46, 

    0x00,

    //--------------------------------------------------
    // 0x0056 : VID_DZTB
    // WM.Dzt.B

    0x02,
    0x22, 0xF9, 
    0x9A, 0x46, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0057 : VID_NICEDAY
    // WM.NiceDay

    0x0D,
    0x22, 0x93, 0x38, 0x28, 0x92, 0x92, 0x28, 0x92, 
    0x32, 0x89, 0x83, 0x82, 0xF9, 
    0x54, 0xB2, 0x55, 0x01, 0x27, 0x01, 0xBC, 0xC8, 
    0x81, 0x80, 0x8B, 0xB2, 0x55, 0x01, 0x27, 0x01, 
    0x52, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0058 : VID_NICEDAYD
    // WM.Niceday.D  Word 8

    0x07,
    0x32, 0x28, 0x92, 0x28, 0x83, 0x82, 0xF9, 
    0x54, 0x27, 0x01, 0xBC, 0xC8, 0x80, 0x27, 0x01, 
    0x52, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0059 : VID_COUNTTENA
    // WM.CountTen.A (for Office97)  jjw 11 Feb 97

    0x03,
    0x22, 0x28, 0xF8, 
    0xFF, 0xDB, 0xB4, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x005A : VID_HELLGATE
    // WM.Hellgate jjw 13 Feb 97

    0x0A,
    0x22, 0x28, 0x28, 0x28, 0x28, 0x38, 0x38, 0x28, 
    0x82, 0xF9, 
    0xF8, 0x96, 0xB5, 0x9E, 0xAE, 0xCC, 0x0F, 0x01, 
    0x32, 0x01, 0x6D, 0x86, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x005B : VID_GOODNIGHT
    // WM.GoodNight

    0x02,
    0x22, 0xF8, 
    0x89, 0x12, 

    0x01,
    0xFB, 

    0x01,
    0xF2, 
    0x2D, 

    0x01,
    0xF2, 
    0x32, 

    0x01,
    0xF2, 
    0x6A, 

    0x01,
    0xF2, 
    0xE4, 

    0x01,
    0xF3, 
    0x25, 0x01, 

    0x01,
    0xF2, 
    0x51, 

    0x01,
    0xF3, 
    0x23, 0x01, 

    0x01,
    0xF2, 
    0xE3, 

    0x00,

    //--------------------------------------------------
    // 0x005C : VID_CAPD
    // WM.Cap.D

    0x0A,
    0x22, 0x28, 0x38, 0x38, 0x28, 0x28, 0x28, 0x28, 
    0x28, 0xF8, 
    0x15, 0x3B, 0x39, 0x24, 0x01, 0x0C, 0x01, 0x25, 
    0xBE, 0xF2, 0x87, 0x69, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x005D : VID_RUN
    // WM.TwoLines.B  new FILESAVEAS

    0x05,
    0x32, 0x28, 0x38, 0x38, 0xF8, 
    0x66, 0x4F, 0x01, 0xF5, 0x6D, 0x01, 0x2B, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x005E : VID_TWOLINESC
    // WM.TWOLINES.C

    0x03,
    0x23, 0x38, 0xF8, 
    0x4F, 0x01, 0xF5, 0x2B, 0x01, 

    0x01,
    0xFB, 

    0x01,
    0xF0, 
    0x49, 

    0x00,

    //--------------------------------------------------
    // 0x005F : VID_XENIXOS
    // WM.Xenixos:De

    0x0B,
    0x23, 0x28, 0x28, 0x28, 0x38, 0x28, 0x38, 0x28, 
    0x38, 0x28, 0xF8, 
    0x5E, 0x01, 0x63, 0x92, 0x7E, 0x7C, 0x3B, 0x01, 
    0x64, 0x31, 0x01, 0xF7, 0x42, 0x01, 0xB8, 

    0x01,
    0xFB, 

    0x01,
    0xF2, 
    0x61, 

    0x01,
    0xF2, 
    0x60, 

    0x01,
    0xF2, 
    0x62, 

    0x01,
    0xF3, 
    0x5F, 0x01, 

    0x01,
    0xF2, 
    0x91, 

    0x01,
    0xF2, 
    0x7F, 

    0x01,
    0xF3, 
    0x3C, 0x01, 

    0x01,
    0xF2, 
    0x65, 

    0x01,
    0xF3, 
    0x30, 0x01, 

    0x01,
    0xF2, 
    0xF6, 

    0x01,
    0xF3, 
    0x41, 0x01, 

    0x01,
    0xF2, 
    0x7B, 

    0x01,
    0xF2, 
    0xB7, 

    0x00,

    //--------------------------------------------------
    // 0x0060 : VID_SHAREFUNA
    // WM.ShareFun.A

    0x09,
    0x32, 0x38, 0x28, 0x28, 0x28, 0x28, 0x38, 0x38, 
    0xF8, 
    0x17, 0x5B, 0x01, 0x1C, 0x01, 0xD4, 0x04, 0xB9, 
    0x6F, 0x64, 0x01, 0x47, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0061 : VID_MUCKC
    // WM.Muck.C	; WD8 d&r 13 Mar 97 jjw

    0x05,
    0x22, 0x28, 0x28, 0x38, 0xF8, 
    0xB0, 0xE9, 0x05, 0xB1, 0x54, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0062 : VID_BANDUNGM
    // WM.Bandung.M	jjw 14 Mar 97

    0x01,
    0xF2, 
    0x23, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0063 : VID_NIGHTSHADEA
    // W97M.NightShade.A

    0x02,
    0x22, 0xF9, 
    0xAF, 0x49, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0064 : VID_MINIMALC
    // W97M.Minimal.C

    0x01,
    0xF2, 
    0x9F, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0065 : VID_SMILEYA
    // WM.Smiley.A:De

    0x07,
    0x22, 0x28, 0x28, 0x28, 0x28, 0x28, 0xF8, 
    0x68, 0x3E, 0xDE, 0xC5, 0x79, 0xF9, 0x77, 

    0x01,
    0xFB, 

    0x01,
    0xF2, 
    0xA9, 

    0x01,
    0xF2, 
    0xAA, 

    0x00,

    //--------------------------------------------------
    // 0x0066 : VID_SURABAYAA
    // WM.SURABAYA.A

    0x06,
    0x32, 0x38, 0x28, 0x28, 0x28, 0xF8, 
    0x7D, 0x48, 0x01, 0x1D, 0x01, 0x5B, 0x0F, 0x10, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0067 : VID_HELPERB
    // WM.Helper.B

    0x01,
    0xF2, 
    0x9D, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0068 : VID_MDMAII
    // WM.MDMA.I?

    0x01,
    0xF2, 
    0xED, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0069 : VID_IRISHJ
    // WM.Irish.J

    0x06,
    0x22, 0x28, 0x83, 0x23, 0x98, 0xF8, 
    0x42, 0xCD, 0xD7, 0x62, 0x01, 0x61, 0x01, 0xD8, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x006A : VID_RAPI
    // WM.Rapi  ; Added by ECC 4/21/97

    0x07,
    0x22, 0x93, 0x38, 0x83, 0x22, 0x89, 0xF9, 
    0x0A, 0x0B, 0x20, 0x01, 0x67, 0x01, 0x2D, 0x01, 
    0x36, 0xA1, 

    0x01,
    0xFB, 

    0x01,
    0xF3, 
    0x67, 0x01, 

    0x01,
    0xF2, 
    0x55, 

    0x01,
    0xF2, 
    0x55, 

    0x01,
    0xF2, 
    0xA0, 

    0x01,
    0xF3, 
    0x68, 0x01, 

    0x01,
    0xF2, 
    0x56, 

    0x01,
    0xF2, 
    0x20, 

    0x00,

    //--------------------------------------------------
    // 0x006B : VID_TWOLINESF
    // WM.Twolines.F

    0x03,
    0x33, 0x38, 0xF8, 
    0x4F, 0x01, 0x28, 0x01, 0x4E, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x006C : VID_WAZZUBS
    // WM.Wazzu.BS

    0x01,
    0xF3, 
    0x63, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x006D : VID_IRISHK
    // WM.Irish.k

    0x08,
    0x22, 0x28, 0x38, 0x38, 0x82, 0x82, 0x83, 0xF9, 
    0xD7, 0xBD, 0x42, 0x10, 0x01, 0x15, 0x01, 0xD8, 
    0x42, 0x10, 0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x006E : VID_APPDERD
    // WM.Appder.D

    0x02,
    0x22, 0xF8, 
    0xA6, 0xCA, 

    0x01,
    0xFB, 

    0x01,
    0xF2, 
    0xA7, 

    0x00,

    //--------------------------------------------------
    // 0x006F : VID_APPDERE
    // WM.Appder.E

    0x02,
    0x23, 0xF8, 
    0x06, 0x01, 0xCA, 

    0x01,
    0xFB, 

    0x01,
    0xF3, 
    0x07, 0x01, 

    0x00,

    //--------------------------------------------------
    // 0x0070 : VID_DZTC
    // WM.Dzt.C

    0x03,
    0x23, 0x92, 0xF8, 
    0x51, 0x01, 0x88, 0xBB, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0071 : VID_FRENZYA
    // Frenzy.A

    0x01,
    0xF2, 
    0xF3, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0072 : VID_SPARKLEA
    // Sparkle.A

    0x03,
    0x22, 0x28, 0xF8, 
    0x6C, 0xAB, 0x72, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0073 : VID_MDMAO
    // WM.MDMA.O

    0x01,
    0xF2, 
    0xD2, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0074 : VID_DIVINAF
    // WM.Divina.F

    0x01,
    0xF2, 
    0xCE, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0075 : VID_NICEDAYG
    // WM.NiceDay.G

    0x02,
    0x22, 0xF8, 
    0x82, 0x53, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0076 : VID_DMVE
    // WM.DMV.E

    0x01,
    0xF2, 
    0xBF, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0077 : VID_FULL_SET
    // Macro Component Virus (works for Remnants, too)

    0x02,
    0x6E, 0xFF, 

    0x02,
    0x2E, 0xFF, 

    0x00,

};

WD_VIRUS_SIG_INFO_T FAR gastWD8VirusSigInfo[] =
{
    { VID_BANDUNGA, gabyWD8VirusSigSet0000 + 0x0000 },
    { VID_CONCEPTKK, gabyWD8VirusSigSet0000 + 0x0006 },
    { VID_RAPIA, gabyWD8VirusSigSet0000 + 0x0013 },
    { VID_RAPIB, gabyWD8VirusSigSet0000 + 0x0037 },
    { VID_RAPIC, gabyWD8VirusSigSet0000 + 0x0052 },
    { VID_RAPID, gabyWD8VirusSigSet0000 + 0x006E },
    { VID_RAPIF, gabyWD8VirusSigSet0000 + 0x0089 },
    { VID_RAPIO, gabyWD8VirusSigSet0000 + 0x00A3 },
    { VID_RAPIA1, gabyWD8VirusSigSet0000 + 0x00C1 },
    { VID_RAPIB1, gabyWD8VirusSigSet0000 + 0x00DB },
    { VID_RAPIC1, gabyWD8VirusSigSet0000 + 0x00F2 },
    { VID_RAPID1, gabyWD8VirusSigSet0000 + 0x010A },
    { VID_RAPIF1, gabyWD8VirusSigSet0000 + 0x0121 },
    { VID_RAPIN1, gabyWD8VirusSigSet0000 + 0x0137 },
    { VID_RAPIO1, gabyWD8VirusSigSet0000 + 0x0156 },
    { VID_RAPIB2, gabyWD8VirusSigSet0000 + 0x0175 },
    { VID_RAPID2, gabyWD8VirusSigSet0000 + 0x018A },
    { VID_RAPIE2, gabyWD8VirusSigSet0000 + 0x01A4 },
    { VID_RAPIF2, gabyWD8VirusSigSet0000 + 0x01B6 },
    { VID_RAPIM2, gabyWD8VirusSigSet0000 + 0x01C8 },
    { VID_RAPIN2, gabyWD8VirusSigSet0000 + 0x01DF },
    { VID_RAPIO2, gabyWD8VirusSigSet0000 + 0x01FF },
    { VID_WAZZUA, gabyWD8VirusSigSet0000 + 0x0215 },
    { VID_WAZZUH, gabyWD8VirusSigSet0000 + 0x021B },
    { VID_WAZZUX, gabyWD8VirusSigSet0000 + 0x0221 },
    { VID_WAZZUAH, gabyWD8VirusSigSet0000 + 0x0227 },
    { VID_WAZZUAZ, gabyWD8VirusSigSet0000 + 0x022D },
    { VID_WAZZUBA, gabyWD8VirusSigSet0000 + 0x0234 },
    { VID_CLOCK, gabyWD8VirusSigSet0000 + 0x023A },
    { VID_COLORSA, gabyWD8VirusSigSet0000 + 0x0255 },
    { VID_COLORST, gabyWD8VirusSigSet0000 + 0x025C },
    { VID_NPADC, gabyWD8VirusSigSet0000 + 0x0265 },
    { VID_NPADO, gabyWD8VirusSigSet0000 + 0x026C },
    { VID_NPADP, gabyWD8VirusSigSet0000 + 0x0272 },
    { VID_NPADQ, gabyWD8VirusSigSet0000 + 0x0278 },
    { VID_NPADAC, gabyWD8VirusSigSet0000 + 0x027F },
    { VID_IRISH, gabyWD8VirusSigSet0000 + 0x0285 },
    { VID_IRISHF, gabyWD8VirusSigSet0000 + 0x0292 },
    { VID_JOHNNY0, gabyWD8VirusSigSet0000 + 0x02A8 },
    { VID_MDMAB, gabyWD8VirusSigSet0000 + 0x02B7 },
    { VID_SHOWOFF, gabyWD8VirusSigSet0000 + 0x02BD },
    { VID_HYBRID, gabyWD8VirusSigSet0000 + 0x02D0 },
    { VID_ANTICON, gabyWD8VirusSigSet0000 + 0x02DB },
    { VID_ALIEN, gabyWD8VirusSigSet0000 + 0x02EB },
    { VID_MADDOGB, gabyWD8VirusSigSet0000 + 0x02F6 },
    { VID_NUCLEARE, gabyWD8VirusSigSet0000 + 0x0309 },
    { VID_SWITCHES, gabyWD8VirusSigSet0000 + 0x031C },
    { VID_TARGETB, gabyWD8VirusSigSet0000 + 0x0325 },
    { VID_NUCLEARB, gabyWD8VirusSigSet0000 + 0x032C },
    { VID_NUCLEARA, gabyWD8VirusSigSet0000 + 0x0344 },
    { VID_NUCLEARC, gabyWD8VirusSigSet0000 + 0x0362 },
    { VID_HOT, gabyWD8VirusSigSet0000 + 0x0380 },
    { VID_TELE, gabyWD8VirusSigSet0000 + 0x0388 },
    { VID_BOOM, gabyWD8VirusSigSet0000 + 0x039D },
    { VID_BIRTHDAY, gabyWD8VirusSigSet0000 + 0x03A9 },
    { VID_BUERO, gabyWD8VirusSigSet0000 + 0x03B1 },
    { VID_GOLDFISH, gabyWD8VirusSigSet0000 + 0x03BA },
    { VID_HASSLE, gabyWD8VirusSigSet0000 + 0x03C3 },
    { VID_EASY, gabyWD8VirusSigSet0000 + 0x03D9 },
    { VID_TEDIOUS, gabyWD8VirusSigSet0000 + 0x03E0 },
    { VID_PHANTOM, gabyWD8VirusSigSet0000 + 0x03F4 },
    { VID_IMPOSTERA, gabyWD8VirusSigSet0000 + 0x03FA },
    { VID_OUTLAWA, gabyWD8VirusSigSet0000 + 0x0406 },
    { VID_OUTLAWB, gabyWD8VirusSigSet0000 + 0x0410 },
    { VID_MADDOGA, gabyWD8VirusSigSet0000 + 0x041A },
    { VID_POLITE, gabyWD8VirusSigSet0000 + 0x042C },
    { VID_FRIENDLY, gabyWD8VirusSigSet0000 + 0x0435 },
    { VID_SAVER, gabyWD8VirusSigSet0000 + 0x0466 },
    { VID_SPOOKY, gabyWD8VirusSigSet0000 + 0x046C },
    { VID_DIVINAC, gabyWD8VirusSigSet0000 + 0x0486 },
    { VID_DIVINAB, gabyWD8VirusSigSet0000 + 0x048C },
    { VID_DIVINAA, gabyWD8VirusSigSet0000 + 0x0492 },
    { VID_STRYX, gabyWD8VirusSigSet0000 + 0x0498 },
    { VID_GANGSTERZ, gabyWD8VirusSigSet0000 + 0x04AB },
    { VID_DOGGIE, gabyWD8VirusSigSet0000 + 0x04B9 },
    { VID_KILLDLL, gabyWD8VirusSigSet0000 + 0x04C5 },
    { VID_SATANIC, gabyWD8VirusSigSet0000 + 0x04CB },
    { VID_DATE, gabyWD8VirusSigSet0000 + 0x04DB },
    { VID_LUNCHB, gabyWD8VirusSigSet0000 + 0x04E2 },
    { VID_LUNCHA, gabyWD8VirusSigSet0000 + 0x04F0 },
    { VID_APPDER, gabyWD8VirusSigSet0000 + 0x0500 },
    { VID_NFB, gabyWD8VirusSigSet0000 + 0x0512 },
    { VID_DARKA, gabyWD8VirusSigSet0000 + 0x051C },
    { VID_NFA, gabyWD8VirusSigSet0000 + 0x0529 },
    { VID_SWLABSB, gabyWD8VirusSigSet0000 + 0x0538 },
    { VID_DZTA, gabyWD8VirusSigSet0000 + 0x053E },
    { VID_DZTB, gabyWD8VirusSigSet0000 + 0x054B },
    { VID_NICEDAY, gabyWD8VirusSigSet0000 + 0x0553 },
    { VID_NICEDAYD, gabyWD8VirusSigSet0000 + 0x0575 },
    { VID_COUNTTENA, gabyWD8VirusSigSet0000 + 0x0589 },
    { VID_HELLGATE, gabyWD8VirusSigSet0000 + 0x0593 },
    { VID_GOODNIGHT, gabyWD8VirusSigSet0000 + 0x05AD },
    { VID_CAPD, gabyWD8VirusSigSet0000 + 0x05CF },
    { VID_RUN, gabyWD8VirusSigSet0000 + 0x05E9 },
    { VID_TWOLINESC, gabyWD8VirusSigSet0000 + 0x05FA },
    { VID_XENIXOS, gabyWD8VirusSigSet0000 + 0x0609 },
    { VID_SHAREFUNA, gabyWD8VirusSigSet0000 + 0x0652 },
    { VID_MUCKC, gabyWD8VirusSigSet0000 + 0x066C },
    { VID_BANDUNGM, gabyWD8VirusSigSet0000 + 0x067B },
    { VID_NIGHTSHADEA, gabyWD8VirusSigSet0000 + 0x0681 },
    { VID_MINIMALC, gabyWD8VirusSigSet0000 + 0x0689 },
    { VID_SMILEYA, gabyWD8VirusSigSet0000 + 0x068F },
    { VID_SURABAYAA, gabyWD8VirusSigSet0000 + 0x06A7 },
    { VID_HELPERB, gabyWD8VirusSigSet0000 + 0x06B9 },
    { VID_MDMAII, gabyWD8VirusSigSet0000 + 0x06BF },
    { VID_IRISHJ, gabyWD8VirusSigSet0000 + 0x06C5 },
    { VID_RAPI, gabyWD8VirusSigSet0000 + 0x06D7 },
    { VID_TWOLINESF, gabyWD8VirusSigSet0000 + 0x0703 },
    { VID_WAZZUBS, gabyWD8VirusSigSet0000 + 0x0710 },
    { VID_IRISHK, gabyWD8VirusSigSet0000 + 0x0717 },
    { VID_APPDERD, gabyWD8VirusSigSet0000 + 0x072E },
    { VID_APPDERE, gabyWD8VirusSigSet0000 + 0x0739 },
    { VID_DZTC, gabyWD8VirusSigSet0000 + 0x0746 },
    { VID_FRENZYA, gabyWD8VirusSigSet0000 + 0x0751 },
    { VID_SPARKLEA, gabyWD8VirusSigSet0000 + 0x0757 },
    { VID_MDMAO, gabyWD8VirusSigSet0000 + 0x0761 },
    { VID_DIVINAF, gabyWD8VirusSigSet0000 + 0x0767 },
    { VID_NICEDAYG, gabyWD8VirusSigSet0000 + 0x076D },
    { VID_DMVE, gabyWD8VirusSigSet0000 + 0x0775 },

    { VID_FULL_SET, gabyWD8VirusSigSet0000 + 0x077B },
};


///////////////////////////////////////////////////////////////////////////
//
// Custom scan and repair function pointer arrays
//
///////////////////////////////////////////////////////////////////////////

LPF_WD_CUSTOM_SCAN FAR gapfWD8CustomScan[] =
{
    NULL
};

LPF_WD_CUSTOM_REPAIR FAR gapfWD8CustomRepair[] =
{
    NULL
};


///////////////////////////////////////////////////////////////////////////
//
// Signature set structure
//
///////////////////////////////////////////////////////////////////////////

WD_SIG_SET_T gstWD8SigSet =
{
    // Name sig info

    118,
    gabyWD8NameSetOffsetSigs,
    0,
    gabyWD8NameEndOffsetSigs,
    gastWD8RawNameSigIndex,
    gabyWD8NameSigRawByteHit,
    118,

    // Named CRC sig info

    318,
    368,
    gastWD8NamedCRCSigInfo,

    // Macro sig info

    0,
    gabyWD8MacroSetOffsetSigs,
    0,
    gabyWD8MacroEndOffsetSigs,
    gastWD8RawMacroSigIndex,
    gabyWD8MacroSigRawByteHit,
    0,

    // CRC info

    318,
    gadwWD8CRCs,

    // Virus signatures

    120,
    gastWD8VirusSigInfo,

    // Custom scan and repair arrays

    gapfWD8CustomScan,
    gapfWD8CustomRepair,

    // Named CRC sig full set

    9,
    gabyWD8NamedCRCSigFullSet,

    // Macro sig full set

    0,
    gabyWD8MacroSigFullSet
};


