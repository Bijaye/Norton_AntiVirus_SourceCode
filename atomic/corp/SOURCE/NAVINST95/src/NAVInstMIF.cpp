
///////////////////////////////////////////////////////////////////////////////
//
// File: NAVInstMIF.cpp
//
//       This module contains the MIF file to be written during uninstall.
//
///////////////////////////////////////////////////////////////////////////////


#include    "NavInst.h"
#include    "NavInstMIF.h"


char            g_szMIFStatements[][SI_MAXSTR]  =
{
    "\n",                                         // Blank Line
    "\n\tSTART COMPONENT",
    "\n\t\tNAME = \"WORKSTATION\"",
    "\n\t\tSTART GROUP" ,
    "\n\t\t\tNAME = \"ComponentID\"",
    "\n\t\t\tID = 1" ,
    "\n\t\t\tCLASS= \"DMTF|ComponentID|1.0\"",
    "\n\t\t\tSTART ATTRIBUTE" ,
    "\n\t\t\t\tNAME = \"Manufacturer\"",
    "\n\t\t\t\tID = 1" ,
    "\n\t\t\t\tACCESS = READ-ONLY" ,
    "\n\t\t\t\tSTORAGE= SPECIFIC" ,
    "\n\t\t\t\tTYPE = STRING(64)",
    "\n\t\t\t\tVALUE = ",
    "                                          ", // Manufacturer
    "\n\t\t\tEND ATTRIBUTE" ,
    "\n\t\t\tSTART ATTRIBUTE" ,
    "\n\t\t\t\tNAME = \"Product\"",
    "\n\t\t\t\tID = 2" ,
    "\n\t\t\t\tACCESS = READ-ONLY" ,
    "\n\t\t\t\tSTORAGE= SPECIFIC" ,
    "\n\t\t\t\tTYPE = STRING(64)",
    "\n\t\t\t\tVALUE = ",
    "                                          ", // Product
    "\n\t\t\tEND ATTRIBUTE" ,
    "\n\t\t\tSTART ATTRIBUTE" ,
    "\n\t\t\t\tNAME = \"Version\"",
    "\n\t\t\t\tID = 3" ,
    "\n\t\t\t\tACCESS = READ-ONLY" ,
    "\n\t\t\t\tSTORAGE= SPECIFIC" ,
    "\n\t\t\t\tTYPE = STRING(64)",
    "\n\t\t\t\tVALUE = ",
    "                                          ", // Version
    "\n\t\t\tEND ATTRIBUTE" ,
    "\n\t\t\tSTART ATTRIBUTE" ,
    "\n\t\t\t\tNAME = \"Licensed to\"",
    "\n\t\t\t\tID = 4" ,
    "\n\t\t\t\tACCESS = READ-ONLY" ,
    "\n\t\t\t\tSTORAGE= SPECIFIC" ,
    "\n\t\t\t\tTYPE = STRING(64)",
    "\n\t\t\t\tVALUE = ",
    "                                          ", // User
    " at ",
    "                                          ", // Company
    "\n\t\t\tEND ATTRIBUTE" ,
    "\n\t\t\tSTART ATTRIBUTE" ,
    "\n\t\t\t\tNAME = \"Installation\"",
    "\n\t\t\t\tID = 5" ,
    "\n\t\t\t\tACCESS = READ-ONLY",
    "\n\t\t\t\tSTORAGE = SPECIFIC" ,
    "\n\t\t\t\tTYPE = STRING(64)" ,
    "\n\t\t\t\tVALUE = ",
    "                                          ", // Date & Time
    "\n\t\t\tEND ATTRIBUTE" ,
    "\n\t\tEND GROUP",
    "\n\t\tSTART GROUP" ,
    "\n\t\t\tNAME = \"InstallStatus\"",
    "\n\t\t\tID = 2" ,
    "\n\t\t\tCLASS = SYMANTEC|JOBSTATUS|1.0",
    "\n\t\t\tSTART ATTRIBUTE" ,
    "\n\t\t\t\tNAME = \"Status\"",
    "\n\t\t\t\tID = 1" ,
    "\n\t\t\t\tACCESS = READ-ONLY" ,
    "\n\t\t\t\tSTORAGE = SPECIFIC" ,
    "\n\t\t\t\tTYPE = STRING(32)",
    "\n\t\t\t\tVALUE = ",
    "                                          ", // Install Status
    "\n\t\t\tEND ATTRIBUTE" ,
    "\n\t\t\tSTART ATTRIBUTE" ,
    "\n\t\t\t\tNAME = \"Description\"",
    "\n\t\t\t\tID = 2" ,
    "\n\t\t\t\tACCESS = READ-ONLY",
    "\n\t\t\t\tSTORAGE = SPECIFIC" ,
    "\n\t\t\t\tTYPE = STRING(120)" ,
    "\n\t\t\t\tVALUE = ",
    "                                          ", // Install Status Description
    "\n\t\t\tEND ATTRIBUTE" ,
    "\n\t\tEND GROUP",
    "\n\tEND COMPONENT" ,
    NULL
} ;

