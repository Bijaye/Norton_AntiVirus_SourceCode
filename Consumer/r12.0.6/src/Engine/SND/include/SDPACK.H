// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/sdpack.h_v   1.5   06 Aug 1998 14:37:36   DCHI  $
//
// Description:
//  Header file for Scan and Deliver packaging DLL.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/sdpack.h_v  $
// 
//    Rev 1.5   06 Aug 1998 14:37:36   DCHI
// Added definition of SDPackDLLImportExport to nothing if not _WINDLL.
// 
//    Rev 1.4   14 Apr 1998 15:17:20   DCHI
// Added support for product strings.
// 
//    Rev 1.3   08 Apr 1998 17:06:26   DCHI
// Consolidated Platinum and Gold to pin.  Changed CaseNum to AtlasID.
// 
//    Rev 1.2   06 Apr 1998 19:19:50   DCHI
// Removed InfectionStateProvince and InfectionType.
// 
//    Rev 1.1   06 Apr 1998 17:51:14   DCHI
// Added SetFieldInfoDateTime(), and modified field enumerations.
// 
//    Rev 1.0   13 Mar 1998 12:50:46   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _SDPACK_H_

#define _SDPACK_H_

#ifdef _WINDLL
#ifdef _SDPACK_CPP_
#define SDPackDLLImportExport __declspec(dllexport)
#else
#define SDPackDLLImportExport __declspec(dllimport)
#endif
#else
#define SDPackDLLImportExport
#endif  

// Items should only be added to the end of this list

#define IDS_SDPACK_NAV_300_WIN3X_INTEL              1
#define IDS_SDPACK_NAV_400_WIN3X_INTEL              2
#define IDS_SDPACK_NAV_200_WIN95_INTEL              3
#define IDS_SDPACK_NAV_400_WIN95_INTEL              4
#define IDS_SDPACK_NAV_500_WIN95_INTEL              5
#define IDS_SDPACK_NAV_400_WINNT_ALPHA              6
#define IDS_SDPACK_NAV_500_WINNT_ALPHA              7
#define IDS_SDPACK_NAV_200_WINNT_INTEL              8
#define IDS_SDPACK_NAV_400_WINNT_INTEL              9
#define IDS_SDPACK_NAV_500_WINNT_INTEL              10
#define IDS_SDPACK_NAV_500_MACOS_MAC                11
#define IDS_SDPACK_LOTUSNOTES_100_WINNT_INTEL       12
#define IDS_SDPACK_MSEXCHANGE_100_WINNT_INTEL       13
#define IDS_SDPACK_NAV_100_NLM_INTEL                14
#define IDS_SDPACK_IEG_100_WINNT_INTEL              15
#define IDS_SDPACK_FW_100_WINNT_INTEL               16
#define IDS_SDPACK_SAM_400_MACOS_MAC                17

#define IDS_SDPACK_COUNT                            18

typedef BOOL (*PPackageProgressCB)
(
    LPVOID      lpvProgressCookie,
    int         nPercentageComplete
);

class SDPackDLLImportExport CPackageSamples
{
    public:

        enum PACKAGE_STATUS
        {
            PACKAGE_OK,
            PACKAGE_CANCELED,
            PACKAGE_NO_RESET,
            PACKAGE_FILE_NOT_FOUND,
            PACKAGE_GENERAL_ERROR,
            PACKAGE_FIELD_INVALID
        };

        enum FIELD
        {
            NoField = -1,
            OtherField = 0,
            FirstName,
            LastName,
            Company,
            SupportNum,
            AtlasID,
            AddressLine1,
            AddressLine2,
            City,
            StateProvince,
            ZipPostalCode,
            Country,
            Phone,
            Fax,
            Email,
            ProductOS,
            DefinitionsDate,
            InfectionCountry,
            Symptoms,
            Language,
            Version,
            OSBits,

            // The following must be the last FIELD enum

            FieldReserved
        };

        enum OS
        {
            OtherOS         = 0,
            WIN3X           = 1,
            WIN95           = 2,
            WIN98           = 3,
            WINNT           = 4,
            WINCE           = 5,
            NLM             = 6,
            MACOS           = 7,
            SOLARIS         = 8,
            LINUX           = 9,
            PALMOS          = 10,

            // The following must be the last OS enum

            OSReserved
        };

        enum PLATFORM
        {
            OtherPlatform   = 0,
            INTEL           = 1,
            ALPHA           = 2,
            MAC             = 3,

            // The following must be the last PLATFORM enum

            PlatformReserved
        };

        enum PRODUCT
        {
            OtherProduct    = 0,
            NAV             = 1,
            SAM             = 2,
            IEG             = 3,
            FW              = 4,
            LOTUSNOTES      = 5,
            MSEXCHANGE      = 6,

            // The following must be the last PRODUCT enum

            ProductReserved
        };

        CPackageSamples();
        ~CPackageSamples();

        virtual BOOL GetProductList
        (
            LPINT               lpnProductCount,
            LPINT *             lplpnListIndex,
            LPTSTR **           lplplpszProductList
        );

        virtual BOOL Reset
        (
            LPCTSTR             lpszPackageName
        );

        virtual BOOL SubmitSample
        (
            LPCTSTR             lpszSampleName,
            LPGUID              lpstGUID,
            LPCTSTR             lpszMachineName,
            LPCTSTR             lpszOriginalName,
            LPCTSTR             lpszMd5,
            LPCTSTR             lpszVID,
            LPCTSTR             lpszCategories,
            LPCTSTR             lpszVirName
        );

        virtual BOOL GetTotalFileSizeCount
        (
            LPDWORD             lpdwFileSize,
            LPDWORD             lpdwFileCount
        );

        virtual PACKAGE_STATUS Finish
        (
            PPackageProgressCB  lpfnProgressCB,
            LPVOID              lpvProgressCookie,
            DWORD               dwFlags,
            LPBYTE *            lplpbyPreSubmissionData,
            FIELD *             lpnInvalidField
        );

        virtual void ReleaseSubmissionData
        (
            LPBYTE              lpbyPreSubmissionData
        );

        virtual void Release
        (
            void
        );

        virtual BOOL SetInfoField
        (
            FIELD               eField,
            LPCTSTR             lpszField
        );

        virtual BOOL SetInfoFieldDateTime
        (
            FIELD               eField,
            int                 nYear,
            int                 nMonth,
            int                 nDay,
            int                 nHour,
            int                 nMinute,
            int                 nSecond
        );

        virtual BOOL SetProductOSField
        (
            int                 nProductID
        );

        virtual BOOL SetProductOSField
        (
            PRODUCT             eProduct,
            int                 nVersion,
            OS                  eOS,
            PLATFORM            ePlatform
        );

    protected:

        LPVOID          m_lpvData;

        virtual void InitData
        (
            void
        );

        virtual void FreeData
        (
            void
        );

        virtual FIELD ValidateInfoFields
        (
            void
        );

        virtual PACKAGE_STATUS CreateSARCTXT
        (
            void
        );

        virtual PACKAGE_STATUS CreatePackage
        (
            PPackageProgressCB  lpfnProgressCB,
            LPVOID              lpvProgressCookie
        );

        virtual PACKAGE_STATUS RenameSARCTXT
        (
            void
        );

        virtual PACKAGE_STATUS EncryptPackage
        (
            void
        );

        virtual PACKAGE_STATUS CreatePreSubmissionData
        (
            LPBYTE *            lplpbyPreSubmissionData
        );

};

#endif // #ifndef _SDPACK_H_

