/****************************************************************
ProductRegCOMNames.h
Copyright 1998 (c) Symantec Corp.

Author:		Mark Spiegel
Created:	12/09/98

Purpose:	This module contains #define statements that can be 
            used to acquire the property names in the Product
            Registration registry area.

            DO NOT TRANSLATE THESE STRINGS!!!!

****************************************************************/

#ifndef __PRODUCTREGCOMNAMES_H__
#define __PRODUCTREGCOMNAMES_H__

#define LUPR_PRODUCT                ( _T( "Product" ) )
#define LUPR_VERSION                ( _T( "Version" ) )
#define LUPR_LANGUAGE               ( _T( "Language" ) )
#define LUPR_TYPE                   ( _T( "Type" ) )
#define LUPR_CALLBACKPRESESSION     ( _T( "Callback_PreSession" ) )
#define LUPR_CALLBACKPOSTSESSION    ( _T( "Callback_PostSession" ) )
#define LUPR_CALLBACKPREPACKAGE     ( _T( "Callback_PrePackage" ) )
#define LUPR_CALLBACKPOSTPACKAGE    ( _T( "Callback_PostPackage" ) )
#define LUPR_CALLBACKWELCOMETEXT	( _T( "Callback_WelcomeText" ) )
#define LUPR_CALLBACKFINISHTEXT		( _T( "Callback_FinishText" ) )
#define LUPR_UPDATESTATUS           ( _T( "UpdateStatus" ) )
#define LUPR_DOWNLOADSTATUS         ( _T( "DownloadStatus" ) )
#define LUPR_PATCHSTATUS            ( _T( "PatchStatus" ) )
#define LUPR_DESCRIPTIVENAME		( _T( "DescriptiveName" ) )
#define LUPR_STATUSTEXT				( _T( "StatusText" ) )
#define LUPR_LIVEUPDATERETURNCODE   ( _T( "LiveUpdateRetCode" ) )
#define LUPR_OVERRIDEGROUP			( _T( "OverrideGroup" ) )
#define LUPR_ABORT					( _T( "Abort" ) )
#define LUPR_REFERENCECOUNT         ( _T( "ReferenceCount" ) )
#define LUPR_SUBSCRIPTION_ABORT		( _T( "SubscriptionAbort" ) )

// IluGroup pre-defined group types:
#define LUPR_GT_COUPLED				( _T( "Coupled") )
#define LUPR_GT_OVERRIDDEN			( _T( "Overridden") )

// LiveUpdate Command Line moniker for LUAll/ProductRegCom
#define LU_MONIKER                  ( _T( "{DE907F20-A4A0-11d2-A985-00104B70545A}" ) )

// LiveUpdate product hierarchy fields
#define LUPR_PRODUCTTITLE			( _T( "ProductTitle" ) )

// Product icon settings
#define LUPR_PRODUCTICON			( _T( "ProductIcon" ) )

// Is this an AutoUpdate friendly product / command line.
#define LUPR_AUTOUPDATE 			( _T( "AutoUpdate" ) )

//NO CODE BELOW THIS LINE
#endif  //#ifndef __PRODUCTREGCOMNAMES_H__
