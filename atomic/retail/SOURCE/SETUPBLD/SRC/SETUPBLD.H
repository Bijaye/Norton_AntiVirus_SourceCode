/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/SETUPBLD/VCS/setupbld.h_v   1.2   11 Jul 1997 10:54:20   JBRENNA  $
//
//  Generic Install Builder Script for InstallShield 5
//
//    File Name:  SETUPBLD.H
//
//  Description:  This header contains the generic definitions for the
//                Symantec Setup Builder
//
//       Author:  Bruce Hellstrom       Date:  6-19-97
//
//     Comments:
//
//////////////////////////////////////////////////////////////////////////
// $Log:   S:/SETUPBLD/VCS/setupbld.h_v  $
// 
//    Rev 1.2   11 Jul 1997 10:54:20   JBRENNA
// Integrate Bruce Hellstrom's CSV parsing changes.
//
//////////////////////////////////////////////////////////////////////////

                                        // Message text ( localize )

#define STR_ERR_DLGBOXERR       "Internal Dialog Box Error"
#define STR_ERR_NODIRFILE       "Cannot open the directory file %s.  This file should be located in the same directory as SETUP.EXE."
#define STR_ERR_NOINFOFILE      "Cannot open the information file %s.  This file should be located in the same directory as SETUP.EXE."
#define STR_ERR_CANTMKDIR       "Cannot create directory %s"
#define STR_ERR_BADINFOFILE     "There is an error in the information file:\n%s\nline = %s"
#define STR_ERR_COPYERR         "An error occurred while trying to copy the file %s.\nError ( %d )\n%s"
#define STR_ERR_COMPERR         "An error occurred while creating the compressed file\n%s\nError ( %d )."
#define STR_ERR_SCRIPTERR       "An error occurred while compiling the install script %s.  The file may be missing or has compile errors.  Please fix the problem and rerun the install builder."
#define STR_ERR_PACKLISTERR     "An error occurred while compiling the packing list %s.  The file may be missing or has compile errors.  Please fix the problem and rerun the install builder."
#define STR_ERR_UNZIPPROJ       "An error occurred while trying to unzip the IS5 project file\n%s"
#define STR_ERR_FILEGROUP       "An error occurred while trying to create the file group files"
#define STR_ERR_LAUNCHIDE       "Setup could not launch the InstallShield 5 program.  You will have to run it manually.  The project file to open is\n%s"
#define STR_ERR_COPYSCRIPT      "An error occurred while trying to copy the script file\n%s\n to the IS5 project"
#define STR_ERR_COPYCUSTDLL     "An error occurred while trying to copy the custom dll file\n%s\n to the IS5 project"
#define STR_ERR_NOPLATFORM      "No PLATFORM environment variable is set.  You must ENV and PROJ first."
#define STR_ERR_INVPLATFORM     "The PLATFORM environment variable is not valid for this operation.  Please ENV and PROJ again."


#define STR_INSTALLTYPE         "Choose the type of install you would like to perform"
#define STR_OPT_NORMAL          "Normal - Install all the Norton Utilities"
#define STR_OPT_CUSTOM          "Custom - Let me choose the components to install"
#define STR_CONFIRM_COPYMSG     "Setup Builder has enough information to start creating the installation.  If you would like to make changes, press the Back button."
#define STR_CONFIRM_BASEDIR     "The installation will be built under the directory:"
#define STR_CONFIRM_DEFACTIONS  "The following parameters have been chosen:"
#define STR_CONFIRM_COPYALL     "All files will be copied"
#define STR_CONFIRM_COPYMISS    "Only missing files will be copied"
#define STR_CONFIRM_ACTIONSTXT  "Setup will perform the following actions:"
#define STR_CONFIRM_CREATDIRS   "Create the necessary directories"
#define STR_CONFIRM_COPYFILES   "Copy the required files to the directory structure"
#define STR_CONFIRM_SETDATES    "Set the proper dates and times on the files"
#define STR_CONFIRM_FINISH      "Unzip InstallShield 5 Project"
#define STR_CONFIRM_CREATEFG    "Create the InstallShield 5 File Group files"
#define STR_LIST_COPYERRS       "The following files were not copied.  The files may be missing from the build.  After correcting the problem, rerun the Setup Builder. This list has been copied to the file "
#define STR_COPYOPT_MSG         "Enter the type of file copy you want to do.  Copy only missing files if you are sure that no updates have been done to files that are already present"
#define STR_OPT1_FULLCOPY       "Copy or recopy all the files"
#define STR_OPT2_PARTCOPY       "Copy only missing files"

#define STR_ASKPATH             "Please select the base directory where the installation should be created.  All the files necessary for installaton will be copied to directories under this directory."
#define STR_DEFTAB              "    "

                                        // Install Constants ( Do not translate )

#define FILE_COPYERR            "Copyerr.txt"
#define EXTRA_BLOCKSPACE        200000
#define COMPANY_NAME            "Symantec"
#define PRODUCT_NAME            "Setup Builder"
#define PRODUCT_VERSION         "1.0"

#define MISSING_FILES           1

#define ENV_TARGETDIR_OVERRIDE  "TARGETDIR_OVERRIDE"
