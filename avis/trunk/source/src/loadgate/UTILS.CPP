//===========================================================================
//
//                               UTILS.CPP
//
// Misc. utilities.
//
//===========================================================================


// Suppress warnings, coming from Bill Gates's code.
//
#pragma  warning (disable : 4201)   // Suppress warning: "Nonstandard extension used..." (from MS *.H files.)
#pragma  warning (disable : 4514)   // Suppress warning: "Unreferenced inline function has been removed..."

#include <windows.h>
#include <process.h>
#include <conio.h>
#include <stdio.h>

#include "tg_all.h"


//---------------------------------------------------------------------------
// Local definitions.

typedef struct _WIPETREE_TW_DATA
{
   BOOL        bWipeContents;          // =FALSE. TRUE:wipe file contents first.
   BOOL        bUseDoDWipe;            // =FALSE. TRUE:Use DoD to wipe file('bWipeContents' must be TRUE).
   DWORD       dwClusterSize;          // Cluster size (used for wiping unused file slack).
} WIPETREE_TW_DATA;

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Globals.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local function prototypes.
void                 TW_WipeTree_LevelBack   (DWORD /*dwParam*/, char *pszPathDirname);
int                  TW_WipeTree_PerFile     (DWORD /*dwParam*/, char *pszPathFilename, char * /*pszFilename*/, WIN32_FIND_DATA * /*pw32SrcData*/);
//---------------------------------------------------------------------------


//===========================================================================

BOOL  IsEscPressed   (VOID)
{
//---------------------------------------------------------------------------
   BOOL           bRetCode = FALSE;
//---------------------------------------------------------------------------

   // Keystroke detected! Process it.
   if (kbhit())
      {
      if (getch() == 0x1B)
         {
         int   cReply = 0x1B;
         textattr (FOREGROUND_RED|FOREGROUND_INTENSITY);
         printf ("\n<ESC> Pressed! Are you sure you want to abort (Y/N)? ");
         while (cReply != 'Y' && cReply != 'N')
            {
            Beep (150, 40);
            cReply = toupper (getch());
            }

         // Restore attributes.
         textattr (BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE);

         if (cReply == 'Y')
            {
            bRetCode = TRUE;
            goto  EXIT;
            }
         }
      else
         Beep (150, 20);
      }

EXIT:
   return (bRetCode);
}
//===========================================================================


//===========================================================================
//
// Returns TRUE if argument is present in the command line, FALSE otherwise.
//
// Returns: if TRUE:
//  *ppWholeArg will point to the whole argument string.
//  *ppRestArg  will point to the part of string immediately after "pArg".
// Say if pArg == "value=" and the argument typed was: "/value=1024"
// ppWholeArg will point to "/value=1024"
// ppRestArg  will point to "1024".
//
// Note for GUI apps. use my helpers:
//    AllocatePPArgsFromGUICommandLine()
//    FreePPArgs()

BOOL  IsArgument (
         char  **ppArgs,      // input:  As passed to 'main()'.
         int   ArgsNum,       // input:  As passed to 'main()'.
         char  *pArg,         // input:  "value="        Cannot be NULL.
         char  **ppWholeArg,  // output: "value=1024"    Cannot be NULL.
         char  **ppRestArg,   // output: "1024".         Cannot be NULL.
         BOOL  bStrict /*= FALSE*/ // TRUE - exact copy required (no partial).
         )
{
//---------------------------------------------------------------------------
   int            i;
   int            pArg_size;
   char           acDashArg[80];    // "-argument".
   char           acSlashArg[80];   // "/argument".
//---------------------------------------------------------------------------

   *ppWholeArg   = 0;
   *ppRestArg    = 0;

   // Set "-argument".
   acDashArg[0]  = '-';
   strcpy (&acDashArg[1], pArg);

   // Set "/argument".
   acSlashArg[0] = '/';
   strcpy (&acSlashArg[1], pArg);

   pArg_size = strlen(pArg);

   for (i=1; i<ArgsNum; i++)
      {
      if (!strnicmp (pArg, ppArgs[i], pArg_size))
         {
         *ppWholeArg = ppArgs[i];
         *ppRestArg  = ppArgs[i]+pArg_size;

         // Must be either not strict or zero, after the end of string.
         if (!bStrict || !(**ppRestArg))
            return TRUE;
         else
            {
            *ppWholeArg   = 0;
            *ppRestArg    = 0;
            }
         }

      if (
         !strnicmp (acDashArg,  ppArgs[i], pArg_size+1) ||
         !strnicmp (acSlashArg, ppArgs[i], pArg_size+1)
         )
         {
         *ppWholeArg = ppArgs[i];
         *ppRestArg  = ppArgs[i]+pArg_size+1;

         if (!bStrict || !(**ppRestArg))
            return TRUE;
         else
            {
            *ppWholeArg   = 0;
            *ppRestArg    = 0;
            }
         }
      }

   return FALSE;
}
//===========================================================================

//===========================================================================
//
// Clears console screen.

VOID  clrscr (VOID)
{
//---------------------------------------------------------------------------
   HANDLE         hConsole;
   COORD          coordScreen = {0, 0};   // Here's where we'll home the cursor.
   BOOL           bSuccess;
   DWORD          cCharsWritten;
   CONSOLE_SCREEN_BUFFER_INFO csbi;       // Ro get buffer info.
   DWORD          dwConSize;              // Number of character cells in the current buffer.
//---------------------------------------------------------------------------

   // Get handle for the standart output device.
   hConsole = GetStdHandle (STD_OUTPUT_HANDLE); // STD_[INPUT|OUTPUTERROR]_HANDLE
   if (hConsole == INVALID_HANDLE_VALUE)
      return;

   // Get the number of character cells in the current buffer.
   //
   bSuccess  = GetConsoleScreenBufferInfo (hConsole, &csbi);
   dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

   // Fill the entire screen with blanks.
   //
   bSuccess =
      FillConsoleOutputCharacter (
         hConsole,
         (TCHAR) ' ',
         dwConSize,
         coordScreen,
         &cCharsWritten
         );


   // Get the current text attribute.
   //
   bSuccess = GetConsoleScreenBufferInfo (hConsole, &csbi);

   // Now set the buffer's attributes accordingly.
   //
   bSuccess =
      FillConsoleOutputAttribute (
         hConsole,
         csbi.wAttributes,
         dwConSize,
         coordScreen,
         &cCharsWritten
         );

   // Put the cursor at (0, 0).
   //
   bSuccess = SetConsoleCursorPosition (hConsole, coordScreen);

   // No need to close console handle in here. Otherwise no
   // output will be visible!
   return;
}
//===========================================================================


//===========================================================================
//
// Function will add pointer into an array of pointers.
// If array not exists yet, it will be allocated.
// If array already exists if will be reallocated to fit one more pointer.
//
// Returns:
//    TRUE  - success.
//    FALSE - failure.
// Note: index of newly added element will always be = (*piArraySize - 1).

BOOL  AddPtrToArray (void ***pppArray, int *piArraySize, const void *pObject)
{
//---------------------------------------------------------------------------
   void           **ppNewArray;
   int            i;
//---------------------------------------------------------------------------

   if (!(*piArraySize))
      {
      delete [] (*pppArray);  // Even if 0 - ok.
      *pppArray = new void*[1];
      if (!(*pppArray))
         goto  ABORT;
      }
   else
      {
      // Allocate array of bigger on one more pointer.
      ppNewArray = new void*[(*piArraySize) + 1];
      if (!ppNewArray)
         goto  ABORT;

      // Copy old elements into the new array.
      for (i=0; i<(*piArraySize); i++)
         ppNewArray[i] = (*pppArray)[i];

      // Now we can get rid of old array of pointers.
      delete [] (*pppArray);

      *pppArray = ppNewArray;
      }

   // Here we have '*pppArray' is ready to fit one more element.
   (*pppArray)[*piArraySize] = (void *)pObject;

   // Increment number of elements in an array.
   (*piArraySize)++;


//EXIT:
   return (TRUE);

ABORT:
   return (FALSE);
}
//===========================================================================


//===========================================================================
//
// Function will remove pointer from an array of pointers, knowing index.
// Remaining elements in an array will be shifted together.
// 'iArraySize' will be decremented.
// Array will be deallocated if no pointers left in it.

VOID  RemovePtrFromArray (void ***pppArray, int *piArraySize, int iIndex)
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------


   if (iIndex >= *piArraySize)
      return;  // No requested string found in array.

   // Switch other elements on the place of removed one.
   for (i=iIndex; i<((*piArraySize)-1); i++)
      (*pppArray)[i] = (*pppArray)[i+1];

   // Zero last element;
   (*pppArray)[i] = NULL;

   // Number of new elements in an array.
   (*piArraySize)--;

   if (!(*piArraySize))
      {
      delete [] (*pppArray);
      (*pppArray) = 0;
      }
}
//===========================================================================

//===========================================================================
//
// Note: function will not 'delete' every pointer. Caller must make sure
//       all pointers are released before.
VOID  FreeArrayOfPtrs   (void ***pppArray, int *piArraySize)
{
   delete [] (*pppArray);
   *pppArray      = NULL;
   *piArraySize   = 0;
}
//===========================================================================

//===========================================================================
//
// Set current text attributes.
// Following attributes could be OR-ed in any combination as an input:
// FOREGROUND_BLUE
// FOREGROUND_GREEN
// FOREGROUND_RED
// FOREGROUND_INTENSITY
// BACKGROUND_BLUE
// BACKGROUND_GREEN
// BACKGROUND_RED
// BACKGROUND_INTENSITY
// Returns previous attributes.

WORD  textattr (WORD wAttributes)
{
//---------------------------------------------------------------------------
   HANDLE   hConsole = GetStdHandle (STD_OUTPUT_HANDLE); // STD_[INPUT|OUTPUTERROR]_HANDLE
   CONSOLE_SCREEN_BUFFER_INFO csb;
//---------------------------------------------------------------------------

   GetConsoleScreenBufferInfo ( // BOOL: Error=>0, else!=0.
      hConsole,               // HANDLE hConsoleOutput:  handle of console screen buffer.
      &csb                    // PCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo    // address of screen buffer info.
      );

   SetConsoleTextAttribute ( // BOOL: Error=>0, else!=0.
      hConsole,               // HANDLE hConsoleOutput:  handle of console screen buffer.
      wAttributes             // WORD wAttributes:       text and background colors.
      );

   return (csb.wAttributes);
}
//===========================================================================

//===========================================================================
//
// Function will allocate storage and copy pString to new storage.

VOID  SetString      (char **ppStrPtr, const char *pString)
{
//---------------------------------------------------------------------------
   int            iBuffSize;
//---------------------------------------------------------------------------

   *ppStrPtr = NULL;

   if (!pString)
      return;

   iBuffSize = strlen (pString) + 1;
   *ppStrPtr = new char [iBuffSize];

   if (!(*ppStrPtr))
      return;

   strcpy (*ppStrPtr, pString);
}
//===========================================================================

//===========================================================================
//

VOID  DeleteString   (char **ppStrPtr)
{
   delete [] (*ppStrPtr);
   *ppStrPtr = NULL;
}
//===========================================================================

//===========================================================================
//
// Function will add string to an array of pointers to strings.
// If array not exists yet, it will be allocated.
// There will be a private space allocated for string.
// If array already exists if will be reallocated to fit one more pointer to
// string
//
// Returns:
//    TRUE  - success.
//    FALSE - failure.

BOOL  AddStringToArray (char ***pppArray, int *piArraySize, const char *pszString)
{
//---------------------------------------------------------------------------
   char           *ptr;
//---------------------------------------------------------------------------

   // Here we have '*pppArray' is ready to fit one more element.
   ptr = new char [strlen(pszString) + 1];
   if (!(ptr))
      return FALSE;

   strcpy (ptr, pszString);

   return (AddPtrToArray ((void ***)pppArray, piArraySize, (void *)ptr));
}
//===========================================================================

//===========================================================================
//
// >=0 : Index of found string.
// <0  : Not found.

int   FindStringInArray (
         char        **ppStringArray,
         int         iElementsNum,
         const char  *pString,
         BOOL        bCaseSensitive /* = FALSE */
         )
{
//---------------------------------------------------------------------------
   int            i;
   int            iRetCode;
//---------------------------------------------------------------------------

   for (i=0; i<iElementsNum; i++)
      {
      if (bCaseSensitive)
         iRetCode = strcmp  (ppStringArray[i], pString);
      else
         iRetCode = strcmpi (ppStringArray[i], pString);

      if (iRetCode == 0)
         return (i);
      }

   return (-1);   // Not found.
}
//===========================================================================

//===========================================================================
//
// Function will remove string from an array of pointers to strings.
// Remaining elements in an array will be shifted together.
// String will be freed.
// 'iArraySize' will be decremented.

VOID  DeleteStringFromArray (char ***pppArray, int *piArraySize, const char *pszString)
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------


   for (i=0; i<(*piArraySize); i++)
      if (strcmpi ((*pppArray)[i], pszString) == 0)
         break;   // Requested string found at index = 'i'.

   if (i >= *piArraySize)
      return;  // No requested string found in array.

   // Free the one to be removed.
   delete [] (*pppArray)[i];

   RemovePtrFromArray ((void ***)pppArray, piArraySize, i);
}
//===========================================================================


//===========================================================================
//
//

VOID  DeleteStringArray   (char ***pppArray, int *piArraySize)
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------


   for (i=0; i<(*piArraySize); i++)
      delete [] (*pppArray)[i];

   delete [] (*pppArray);

   (*pppArray)  = NULL;
   *piArraySize = 0;
}
//===========================================================================

//===========================================================================
//
// Generates random number between iMin and iMax, inclusive.

int                  g_iFirstTime = TRUE;
//CriticalSection      g_CS_GetRandomNumber;

int   GetRandomNumber   (int iMin, int iMax, int iSeed /* = 0 */)
{
//---------------------------------------------------------------------------
//   int   iRandomNumber, sections, sec_size, which_sec, final_num;
//   CriticalSection   cs (&g_CS_GetRandomNumber);
   int            range_size;
   int            chunks_num;
   int            chunk_size;
   int            random_chunk_num;
   int            random_number_within_chunk;
   int            resulted_number;
//---------------------------------------------------------------------------


///!!!   if (g_iFirstTime)
//////////////////////////////!!!
   if (g_iFirstTime || iSeed)
//////////////////////////////!!!
      {
      if (!iSeed)
         {
         SYSTEMTIME     st;

         // Get local time in UTC.
         GetSystemTime (&st);
         srand ((unsigned int)(st.wMilliseconds));
         }
      else
         srand (iSeed);

      rand();

      g_iFirstTime = FALSE;
      }


   range_size = (iMax - iMin + 1);

   if (range_size <= RAND_MAX)
      return ((rand() % (iMax - iMin + 1)) + iMin);
   else
      {
      // Note: for number larger, than RAND_MAX the random number generated
      //       may never reach the last few values.

      // We want to make sure, that chunk size is less, than RAND_MAX.
      // Hence '+1' is there.
      chunks_num = (range_size / RAND_MAX) + 1;
      if (chunks_num > RAND_MAX)
         return (range_size/2);  // Error. Range size is larger than RAND_MAX*RAND_MAX;

      chunk_size = range_size / chunks_num;

      random_chunk_num =  rand() % chunks_num;

      random_number_within_chunk = rand() % (chunk_size-1);

      resulted_number = ((random_chunk_num * chunk_size) + random_number_within_chunk) + iMin;

      return (resulted_number);
      }

// This is original formula.
//   return ((rand() % (iMax - iMin + 1)) + iMin);
}
//===========================================================================

//===========================================================================
//
// Function will try to find the beginning of line of possibly binary buffer
// starting from the given pointer somewhere in the middle.
// Function will search "back" no more than, 'iCharsToSearch' chars.
// Function will count the beginning of buffer as the beginning of line.

char  *FindBeginningOfLine (
         const char  *pBuffer,
         const char  *pStartFrom,
         int         iCharsToSearch
         )
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------

   for (i=1; i<=iCharsToSearch && (pStartFrom-i) >= pBuffer; i++)
      if ((*(pStartFrom - i) == 0x0A) || (*(pStartFrom - i) == 0x0D))
         return (char *)(pStartFrom - i + 1);

   return (char *)(pStartFrom - i + 1);
}
//===========================================================================

//===========================================================================
//
// Function will try to find the end of line of possibly binary buffer.
// Function will search forward no more than, 'iCharsToSearch' chars.
// Function will return pointer to the last valid char in buffer.

char  *FindEndOfLine (
         const char  *pBuffer,
         int         iCharsToSearch
         )
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------

   for (i=1; i<iCharsToSearch; i++)
      if ((*(pBuffer + i) == 0x0A) || (*(pBuffer + i) == 0x0D))
         return (char *)(pBuffer + i - 1);

   return (char *)(pBuffer + i - 1);
}
//===========================================================================

//===========================================================================
//
// Search 'iBufferSize' bytes of possibly binary 'pBuffer' for the first
// occurence of 'pString'. Returns pointer to the found string | NULL.
//
// If 'bSpecSearch' is TRUE, then search for "o?e*tt?r" in buffer: "love_letter"
// will return TRUE.
// Notes:
//    -  Search for "o?e*tt?r" in buffer: "love\nletter" will return FALSE, because
//       CR or LF are not considered as matching to '?' or '*' spec.

// Known bug: search for "e*mp" in "ebmcmp" return FALSE.

char  *StrStr  (
         const char  *pBuffer,
         const char  *pString,
         int         iBufferSize,
         BOOL        bCaseSensitive,
         BOOL        bSpecSearch /*=FALSE*/,   // TRUE:'?' and '*' considered as spec, not as 'pString' part.
         int         *piFoundLen /*=NULL */
         )
{
//---------------------------------------------------------------------------
   int            i, j, k;
   int            iStrLen;
   char           *pSpecString    = NULL;
   BOOL           bFreeSpecString = FALSE;
//---------------------------------------------------------------------------

   if (piFoundLen)
      *piFoundLen = 0;

   if (!pBuffer || !pString || iBufferSize<=0)
      return (NULL);

   if (bSpecSearch)
      {
      pSpecString = new char [strlen (pString) + 1];
      // Get rid of '\' and conver '?' -> CHAR_QUE, '*' -> CHAR_STAR
      ConvertPatternStringToSpecString (pString, pSpecString);
      bFreeSpecString = TRUE;
      }
   else
      {
      pSpecString = (char *)pString;
      bFreeSpecString = FALSE;
      }

   iStrLen = strlen (pSpecString);

   i = 0;

#pragma  warning (disable : 4127)   // Suppress warning: "Conditional expression is constant".
   while(1)
#pragma  warning (default : 4127)
      {
      for (; i<iBufferSize; i++)
         {
         if (CHAR_VAL(pBuffer[i]) == CHAR_VAL(pSpecString[0]))
            break;   // pBuffer[i] = first char, that matched first char of string.
         }

      if (i>=iBufferSize)
         {
         if (bFreeSpecString)
            delete [] pSpecString;
         return (NULL);
         }

      // 'k' will get incremented during '*' matching.
      // We have to save 'i' as a pointer to the beginning of spec match.
      for (k=1, j=1; j<iStrLen && (i+k)<iBufferSize; j++, k++)
         {
         if (bSpecSearch)
            {
            if (pBuffer[i+k] == 0x0A || pBuffer[i+k] == 0x0D)
               {
               i += (k);  // Next time start from after CR LF.
               break;
               }

            if (pSpecString[j] == CHAR_QUE)
               continue;   // matched any char.

            if (pSpecString[j] == CHAR_STAR)
               {
               while (pSpecString[j] == CHAR_STAR)
                  j++; // Skip all "***" in spec.

               if (pSpecString[j] == CHAR_QUE)
                  continue;   // matched any char.

               while (((i+k)<iBufferSize) && (CHAR_VAL(pBuffer[i+k]) != CHAR_VAL(pSpecString[j])))
                  {
                  if (pBuffer[i+k] == 0x0A || pBuffer[i+k] == 0x0D)
                     break;

                  k++;

                  // Do not allow excessively long matchings.
                  if (k > 120)
                     break;
                  }

               if ((i+k)>=iBufferSize)
                  {
                  if (bFreeSpecString)
                     delete [] pSpecString;
                  return (NULL); // Next char after '*' in spec not found in buffer.
                  }

               // CRLF met. refresh searching...
               if (pBuffer[i+k] == 0x0A || pBuffer[i+k] == 0x0D)
                  {
                  i += (k);  // Next time start from after CR LF.
                  break;
                  }

               // Do not allow excessively long matchings.
               if (k > 120)
                  break;

               // Found matching next char after "*" in buffer.
               continue;
               }
            }

         if (CHAR_VAL(pBuffer[i+k]) != CHAR_VAL(pSpecString[j]))
            break;
         }

      if (j >= iStrLen)
         {
         if (piFoundLen)
            *piFoundLen = k; // Size = number of chars in spec + number of chars incremented due to the '*' in spec.

         if (bFreeSpecString)
            delete [] pSpecString;

         return (char *)(pBuffer+i);// Found!
         }
      else
         i++;  // Not found. Continue search.
      }
}
//=========================================================================

//=========================================================================
//
// Replaces '?' -> '\1', '*' -> '\2', '\\' -> '\'...
// To prepare it for StrStr ().

VOID  ConvertPatternStringToSpecString (const char *pPattern, char *pConvertedPattern)
{
//---------------------------------------------------------------------------
   int            i, j;
   BOOL           bAbsValue = FALSE;
//---------------------------------------------------------------------------

   for (i=0, j=0; pPattern[i]; i++)
      {
      if (!bAbsValue)
         {
         switch (pPattern [i])
            {
            case  '\\'  :  bAbsValue = TRUE;                         break;
            case  '?'   :  pConvertedPattern [j++] = CHAR_QUE;       break;
            case  '*'   :  pConvertedPattern [j++] = CHAR_STAR;      break;
            default:       pConvertedPattern [j++] = pPattern [i];   break;
            }

         continue;
         }
      else
         {
         bAbsValue = FALSE;
         pConvertedPattern [j++] = pPattern [i];
         }
      }

   pConvertedPattern [j] = 0;
}
//=========================================================================

//===========================================================================
//
// Search 'iBufferSize' bytes of possibly binary 'pBuffer' for the first
// occurence of 'pString'. Returns pointer to the found string | NULL.

char  *StrStr_old  (
         const char  *pBuffer,
         const char  *pString,
         int         iBufferSize,
         BOOL        bCaseSensitive
         )
{
//---------------------------------------------------------------------------
   int            i, j;
   int            iStrLen;
//---------------------------------------------------------------------------

   if (!pBuffer || !pString || iBufferSize<=0)
      return (NULL);

   iStrLen = strlen (pString);
   i = 0;

#pragma  warning (disable : 4127)   // Suppress warning: "Conditional expression is constant".
   while(1)
#pragma  warning (default : 4127)
      {
      for (; i<iBufferSize; i++)
         {
         if (bCaseSensitive)
            {
            if (pBuffer[i] == pString[0])
               break;   // pBuffer[i] = first char, that matched first char of string.
            }
         else
            {
            if (toupper(pBuffer[i]) == toupper(pString[0]))
               break;   // pBuffer[i] = first char, that matched first char of string.
            }
         }

      if (i>=iBufferSize)
         return (NULL);

      for (j=1; j<iStrLen && (i+j)<iBufferSize; j++)
         {
         if (bCaseSensitive)
            {
            if (pBuffer[i+j] != pString[j])
               break;
            }
         else
            {
            if (toupper(pBuffer[i+j]) != toupper(pString[j]))
               break;
            }
         }

      if (j >= iStrLen)
         return (char *)(pBuffer+i);// Found!
      else
         i++;  // Not found. Continue search.
      }
}
//=========================================================================

//===========================================================================
//
// Make case insensitive search of 'iBytes' of string 'pWhat' inside of
// 'pWhere'.
// Returns:
//    -1:not found
//    else:index of found string in 'pWhere'.
//       i.e. pWhere+i = pWhat.

int   StrStrNi (const char *pWhere, const char *pWhat, int iBytes)
{
//---------------------------------------------------------------------------
   int            i, j;
//---------------------------------------------------------------------------

   i = 0;

#pragma  warning (disable : 4127)   // Suppress warning: "Conditional expression is constant".
   while(1)
#pragma  warning (default : 4127)
      {
      // Search for the first occurence of char.
      for (; pWhere[i]; i++)
         if (toupper(pWhere[i]) == toupper(pWhat[0]))
            break;

      if (pWhere[i])
         {
         // First char found. Compare the whole string.
         for (j=1; j<iBytes && pWhere[i+j]; j++)
            if (toupper(pWhere[i+j]) != toupper(pWhat[j]))
               break;

         if (j >= iBytes)
            return (i);// Found!
         else
            i++;  // Not found. Continue search.
         }
      else
         return (-1);
      }
}
//===========================================================================

//===========================================================================
//
// Function checks whether file is matches spec.
// '*' and '?' are ok to be in spec.
// Filename may be with path.
//
// Returns:
//    TRUE - filename matches input spec. FALSE otherwise.
// Example:
//
// SpecMatch ("*.DOC",     "myfile.doc")           == TRUE.
// SpecMatch ("AB?C*.A?*", "c:\winnt35\win.ini")  == FALSE.
//
// Notes:
// ?     -  any single character.
// *     -  everything.
// *.*   -  name with extension.
// *.    -  name without extension.

BOOL  SpecMatch (const char *pSpec, const char *pFileName)
{
//---------------------------------------------------------------------------
   BOOL  bStarQ;
   int   fidx, sidx, iRetCode;    // File name index, spec index.
   int   i;
//---------------------------------------------------------------------------


   if (!pSpec || !pFileName)
      return FALSE;

   // Check whether filename matches...
   //
   for (fidx=sidx=0, bStarQ=FALSE; ; fidx++, sidx++)
      {
      if (pSpec[sidx] != '?' && pSpec[sidx] != 0)
         bStarQ = FALSE;   // Reset flag: in "*????" sequence...

      switch (pSpec[sidx])
         {
         case  '*':  // All chars.
            {
            // Skip all '*' glued together...
            while (pSpec[sidx] == '*')
               sidx++;  // Now look for next char after '*'...

            // "*"   ?
            if (!pSpec[sidx])
               return (TRUE);

            // "*.*" ?
            if (pSpec[sidx] == '.' && pSpec[sidx+1] == '*' && pSpec[sidx+2] == 0)
               return (TRUE);

            // "*."  ?
            if (pSpec[sidx] == '.' && pSpec[sidx+1] == 0)
               {
               // If the rest of filename does not contain '.' - TRUE, else:FALSE
               for (; pFileName[fidx]; fidx++)
                  if (pFileName[fidx] == '.')
                     return (FALSE);

               return (TRUE);
               }

            // Anything else...

            // Get number of valid chars in spec, that followed '*'.
            for (i=0; pSpec[sidx+i] && pSpec[sidx+i] != '*' && pSpec[sidx+i] != '?'; i++)
               if (pSpec[sidx+i] == '.' && pSpec[sidx+i+1] == 0)
                  break;

            if (i)
               {
               // Here 'pSpec + sidx' points on 'i'-bytes long string, that we'll
               // have to find in filename, starting from 'pFileName+fidx'
               iRetCode = StrStrNi (pFileName + fidx, pSpec + sidx, i);
               if (pSpec[sidx+i] == 0)
                  {
                  // If spec is finished, but filename is not yet -
                  // search for the same string until filename will finish also.
                  // cases: "m*a*t" -> "mail.txt"      or
                  //        "m*t"   -> "mail.txt"
                  while (iRetCode >= 0 && pFileName [fidx+iRetCode+i] != 0)
                     {
                     fidx = fidx + i + iRetCode;
                     iRetCode = StrStrNi (pFileName + fidx, pSpec + sidx, i);
                     }
                  if (iRetCode >= 0 && pFileName [fidx+iRetCode+i] == 0)
                     return (TRUE);
                  else
                     return (FALSE);
                  }
               else
                  {
                  if (iRetCode >= 0)
                     {
                     fidx += (iRetCode+i-1); // -1 because loop will increment them again.
                     sidx += (i-1);
                     continue;
                     }
                  else
                     return (FALSE);
                  }
               }
            else
               {
               if (pSpec[sidx] == '?')
                  bStarQ = TRUE; // If next char is '?'!
               sidx--;  // Pass it to the main loop.
               fidx--;  // Pass it to the main loop.
               }
            }
            break;


         case  '?':  // Any single char, but not \0.
            if (pFileName[fidx])
               continue;
            else
               return (FALSE);
            break;


         case  0:    // End of spec. If EOF or in '*????' sequence - TRUE.
            if (pFileName[fidx] == 0 || bStarQ)
               return (TRUE); // Ok!
            else
               return (FALSE);
            break;


         case  '.':
            // Previous could not be '*' - it would be processed above at [case '*'].
            //                v         v
            // Check for "*BDA." -> "BDA" file.
            if (pSpec[sidx+1] == 0 && pFileName[fidx] == 0)
               return (TRUE);
            if (pSpec[sidx] == pFileName[fidx])
               continue;
            else
               return (FALSE);

         default:
            if (toupper(pFileName[fidx]) == toupper(pSpec[sidx]))
               continue;
            else
               return (FALSE);
            break;
         }
      }
}
//===========================================================================


//===========================================================================
//
//

BOOL  LoadValue_Bool   (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         BOOL        *pbValue,         // Where to read the value.
         BOOL        bDefault,         // Default value for the value, if not found.
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
   return (LoadValue_Dword (hBaseKeyHandle, pFullKeyOrSectionName, pValueName, (DWORD *)pbValue, (DWORD)bDefault, pIniFilename));
}
//===========================================================================


//===========================================================================
//
//

BOOL  SaveValue_Bool   (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         BOOL        bValue,           // Value.
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
   return (SaveValue_Dword (hBaseKeyHandle, pFullKeyOrSectionName, pValueName, (DWORD)bValue, pIniFilename));
}
//===========================================================================


//===========================================================================
//
//

BOOL  LoadValue_Int    (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         int         *piValue,         // Where to read the value.
         int         iDefault,         // Default value for the value, if not found.
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
   return (LoadValue_Dword (hBaseKeyHandle, pFullKeyOrSectionName, pValueName, (DWORD *)piValue, (DWORD)iDefault, pIniFilename));
}
//===========================================================================


//===========================================================================
//
//

BOOL  SaveValue_Int    (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         int         iValue,           // Value.
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
   return (SaveValue_Dword (hBaseKeyHandle, pFullKeyOrSectionName, pValueName, (DWORD)iValue, pIniFilename));
}
//===========================================================================


//===========================================================================
//
// Function will read data either from registry or from INI file.

BOOL  LoadValue_Dword  (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         DWORD       *pdwValue,        // Where to read the value.
         DWORD       dwDefault,        // Default value for the value, if not found.
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
//---------------------------------------------------------------------------
   DWORD          dwBytes = sizeof(*pdwValue);
   BOOL           bRetCode = FALSE;
//---------------------------------------------------------------------------


   if (!pFullKeyOrSectionName)
      return (FALSE);

   // If use-INI-file specified, then information in INI file will
   // override the one in the registry.
   if (pIniFilename)
      {
      *pdwValue =
          GetPrivateProfileInt ( // UINT: key value|default(if not exists). 0:if key is <=0.
            pFullKeyOrSectionName, // LPCTSTR lpAppName:  address of section name
            pValueName,       // LPCTSTR lpKeyName:  address of key name
            dwDefault,        // INT nDefault: return value if key name is not found
            pIniFilename      // LPCTSTR lpFileName: address of initialization filename
            );

      bRetCode = TRUE;
      }
   else
      {
      bRetCode =
         HLP_RegLoadValue ( // BOOL: TRUE:success, FALSE:failure.
            hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            pFullKeyOrSectionName,   // const char  *pKeyName:     "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
            pValueName, // const char  *pValueName:   "Include Subdirectories"
            (char *)pdwValue,    // char        *pData,
            &dwBytes,   // Address of DWORD where: size of 'pData'/bytes copied. if pData=NULL => to receive size of data required.
            (char *)&dwDefault, // char        *pDefaultData=NULL,  // Default data buffer if key is not in registry.
            sizeof(dwDefault)    // DWORD       dwDefaultDataSize=0  // Size of default data buffer. 'pData' must be large enough to get 'dwDefaultDataSize' bytes.
            );
      }

   return (bRetCode);
}
//===========================================================================


//===========================================================================
//
// Function will save data either to registry or to INI file.

BOOL  SaveValue_Dword  (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,      // Name of value
         DWORD       dwValue,          // Value.
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode;
//---------------------------------------------------------------------------

   if (!pFullKeyOrSectionName)
      return (FALSE);

   if (pIniFilename)
      {
      char  acValueString[20];

      itoa (dwValue, acValueString, 10);  // Convert value-to-save to string representation.

      bRetCode =
         WritePrivateProfileString ( // BOOL: 0:fail, else:success.
            pFullKeyOrSectionName,  // LPCTSTR lpAppName:  pointer to section name
            pValueName,          // LPCTSTR lpKeyName:  pointer to key name
            acValueString,       // LPCTSTR lpString:   pointer to string to add
            pIniFilename         // LPCTSTR lpFileName: pointer to initialization filename
            );
      }
   else
      {
      bRetCode =
         HLP_RegSaveValue ( // BOOL: TRUE:success, FALSE:failure.
            hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            pFullKeyOrSectionName,  // const char  *pKeyName:     "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
            pValueName, // const char  *pValueName:   "Include Subdirectories"
            REG_DWORD,  // DWORD       dwValueType:   REG_[DWORD|SZ|MULTI_SZ|BINARY...].
            (char *)&dwValue,   // char        *pData,
            sizeof (dwValue)// DWORD       dwDataSize
            );
      }

   return (bRetCode);
}
//===========================================================================


//===========================================================================
//
//

BOOL  LoadValue_Filetime   (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,            // Name of value
         FILETIME    *pftToLoad,             // Where to read the value.
         FILETIME    *pftDefault,            // Default value for the value, if not found.
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
//---------------------------------------------------------------------------
   SYSTEMTIME  stToLoad;
   SYSTEMTIME  stDefault;

   BOOL        bRetCode;
//---------------------------------------------------------------------------

   if (!pFullKeyOrSectionName)
      return (FALSE);

   memset (&stToLoad, 0, sizeof(stToLoad));

   FileTimeToSystemTime (pftDefault, &stDefault);

   bRetCode = LoadValue_Systemtime (hBaseKeyHandle, pFullKeyOrSectionName, pValueName, &stToLoad, &stDefault, pIniFilename);

   SystemTimeToFileTime (&stToLoad, pftToLoad);

   return (bRetCode);
}
//===========================================================================


//===========================================================================
//
//

BOOL  SaveValue_Filetime  (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName,
         const char  *pValueName,
         FILETIME    *pftValue,
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
//---------------------------------------------------------------------------
   SYSTEMTIME  st;
//---------------------------------------------------------------------------

   if (!pFullKeyOrSectionName)
      return (FALSE);

   FileTimeToSystemTime (pftValue, &st);
   return   (SaveValue_Systemtime (hBaseKeyHandle, pFullKeyOrSectionName, pValueName, &st, pIniFilename));
}
//===========================================================================


//===========================================================================
//
// Function will get length of required string from the registry, allocate
// a buffer for the string, and will read an actual string from the registry.
// Note: FreeValue_String () must be called to release a stirng memory after
//       it is no longer needed.

BOOL  LoadValue_Systemtime   (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,
         SYSTEMTIME  *pstToLoad,
         SYSTEMTIME  *pstDefault,
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode;
   DWORD          dwBytes;
   char           acSystemTime[80];          // 1997/02/14 (5) 21:45:59.999
   char           acDefaultSystemTime[80];   // 1997/02/14 (5) 21:45:59.999
//---------------------------------------------------------------------------

   if (!pFullKeyOrSectionName)
      return (FALSE);

   // Set default value first...
   *pstToLoad = *pstDefault;

   // Convert default system time into the string...
   SystemTimeToString (pstDefault, acDefaultSystemTime);


   // If use-INI-file specified, then information in INI file will
   // override the one in the registry.
   if (pIniFilename)
      {
      // Use as default string the one already read from the registry.
      GetPrivateProfileString ( // DWORD: success:num of char copied, not incl. \0.
         pFullKeyOrSectionName,  // LPCTSTR lpAppName:  points to section name
         pValueName,             // LPCTSTR lpKeyName:  points to key name
         acDefaultSystemTime,    // LPCTSTR lpDefault:  points to default string
         acSystemTime,           // LPTSTR lpReturnedString:  points to destination buffer
         sizeof(acSystemTime),   // DWORD nSize:  size of destination buffer
         pIniFilename            // LPCTSTR lpFileName: points to initialization filename
         );

      bRetCode = TRUE;
      }
   else
      {
      dwBytes = sizeof(acSystemTime);

      // Now load the actual key...
      bRetCode =
         HLP_RegLoadValue ( // BOOL: TRUE:success, FALSE:failure.
            hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            pFullKeyOrSectionName,      // const char  *pKeyName:     "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
            pValueName,    // const char  *pValueName:   "Include Subdirectories"
            acSystemTime,  // char        *pData,
            &dwBytes,      // Address of DWORD where: size of 'pData'/bytes copied. if pData=NULL => to receive size of data required.
            acDefaultSystemTime,          // char        *pDefaultData=NULL,  // Default data buffer if key is not in registry.
            strlen(acDefaultSystemTime)+1 // DWORD       dwDefaultDataSize=0  // Size of default data buffer. 'pData' must be large enough to get 'dwDefaultDataSize' bytes.
            );
      }

   // Now, convert systemtime string into the structure elements...
   if (bRetCode)
      StringToSystemTime (acSystemTime, pstToLoad);

   return (bRetCode);
}
//===========================================================================


//===========================================================================
//
//

BOOL  SaveValue_Systemtime (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName,
         const char  *pValueName,
         SYSTEMTIME  *pstValue,
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode;
   char           acSystemTime[80]; // = "1997/12/15 (7) 23:59:59.999"
//---------------------------------------------------------------------------


   if (!pFullKeyOrSectionName)
      return (FALSE);

   // typedef struct _SYSTEMTIME {  // st
   //     WORD wYear;
   //     WORD wMonth;
   //     WORD wDayOfWeek;
   //     WORD wDay;
   //     WORD wHour;
   //     WORD wMinute;
   //     WORD wSecond;
   //     WORD wMilliseconds;
   // } SYSTEMTIME;

   SystemTimeToString (pstValue, acSystemTime);

   // If use-INI-file specified, then save information in INI file.
   //
   if (pIniFilename)
      {
      bRetCode =
         WritePrivateProfileString ( // BOOL: 0:fail, else:success.
            pFullKeyOrSectionName,  // LPCTSTR lpAppName:  pointer to section name
            pValueName,             // LPCTSTR lpKeyName:  pointer to key name
            acSystemTime,           // LPCTSTR lpString:   pointer to string to add
            pIniFilename            // LPCTSTR lpFileName: pointer to initialization filename
            );
      }
   else
      {
      bRetCode =
         HLP_RegSaveValue ( // BOOL: TRUE:success, FALSE:failure.
            hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            pFullKeyOrSectionName,  // const char  *pKeyName:     "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
            pValueName, // const char  *pValueName:   "Include Subdirectories"
            REG_SZ,     // DWORD       dwValueType:   REG_[DWORD|SZ|MULTI_SZ|BINARY...].
            acSystemTime,    // char        *pData,
            strlen(acSystemTime)+1// DWORD       dwDataSize
            );
      }

   return (bRetCode);
}
//===========================================================================


//===========================================================================
//
// Function will read string from the registry.
// 'pStrDefault' may point to the same string as 'pDestString'.

BOOL  LoadValue_String       (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,
         char        *pDestString,
         int         iDestStrSize,
         char        *pStrDefault,
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode;
   DWORD          dwBytes;
   DWORD          dwDefaultStrLen;
   char           *pStrDefaultCopy;
//---------------------------------------------------------------------------


   if (!pFullKeyOrSectionName)
      return (FALSE);

   dwBytes = iDestStrSize;

   dwDefaultStrLen = pStrDefault?strlen (pStrDefault):0;

   // Make copy of default string. Pass to API copy instead of original,
   // because SDK doesn't say if it is possible to pass same string as default
   // and as acceptor. And here we allow user to pass the same string.
   //
   pStrDefaultCopy = new char [dwDefaultStrLen + 1];
   strcpy (pStrDefaultCopy, pStrDefault);

   // If use-INI-file specified, then information in INI file will
   // override the one in the registry.
   if (pIniFilename)
      {
      // Use as default string the one already read from the registry.
      GetPrivateProfileString ( // DWORD: success:num of char copied, not incl. \0.
         pFullKeyOrSectionName,   // LPCTSTR lpAppName:  points to section name
         pValueName,       // LPCTSTR lpKeyName:  points to key name
         pStrDefaultCopy,  // LPCTSTR lpDefault:  points to default string
         pDestString,      // LPTSTR lpReturnedString:  points to destination buffer
         iDestStrSize,     // DWORD nSize:  size of destination buffer
         pIniFilename      // LPCTSTR lpFileName: points to initialization filename
         );

      bRetCode = TRUE;
      }
   else
      {
      bRetCode =
         HLP_RegLoadValue ( // BOOL: TRUE:success, FALSE:failure.
            hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            pFullKeyOrSectionName,   // const char  *pKeyName:     "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
            pValueName, // const char  *pValueName:   "Include Subdirectories"
            pDestString,// char        *pData,
            &dwBytes,   // Address of DWORD where: size of 'pData'/bytes copied. if pData=NULL => to receive size of data required.
            pStrDefaultCopy,  // char        *pDefaultData=NULL,  // Default data buffer if key is not in registry.
            dwDefaultStrLen+1 // DWORD       dwDefaultDataSize=0  // Size of default data buffer. 'pData' must be large enough to get 'dwDefaultDataSize' bytes.
            );
      }

   delete [] pStrDefaultCopy;
   return (bRetCode);
}
//===========================================================================


//===========================================================================
//
//

BOOL  SaveValue_String       (
         HKEY        hBaseKeyHandle,   // HKEY hKey|NULL(if INI): handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,
         char        *pString,
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode;
//---------------------------------------------------------------------------


   if (!pFullKeyOrSectionName)
      return (FALSE);

   // If use-INI-file specified, then save information in INI file.
   //
   if (pIniFilename)
      bRetCode =
         WritePrivateProfileString ( // BOOL: 0:fail, else:success.
            pFullKeyOrSectionName,  // LPCTSTR lpAppName:  pointer to section name
            pValueName,             // LPCTSTR lpKeyName:  pointer to key name
            pString,                // LPCTSTR lpString:   pointer to string to add
            pIniFilename            // LPCTSTR lpFileName: pointer to initialization filename
            );
   else
      bRetCode =
         HLP_RegSaveValue ( // BOOL: TRUE:success, FALSE:failure.
            hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            pFullKeyOrSectionName,   // const char  *pKeyName:     "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
            pValueName, // const char  *pValueName:   "Include Subdirectories"
            REG_SZ,     // DWORD       dwValueType:   REG_[DWORD|SZ|MULTI_SZ|BINARY...].
            pString,    // char        *pData,
            strlen(pString)+1// DWORD       dwDataSize
            );

   return (bRetCode);
}
//===========================================================================


//===========================================================================
//
// pppStrArray - address of pointer to string array.
// piStrArraySize - address of number of elements in string array.
// There is no default data for 'StringArray' type of data.
// Returns:
//    TRUE:    Success
//    FALSE:   Failure.
// Note:
//    Use 'DeleteStringArray()' call to free all memory allocated by this function.

BOOL  LoadValue_StringArray  (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,
         char        ***pppStrArray,
         int         *piStrArraySize,
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode;
   DWORD          dwBytes  = 0;

   char           acString [4096];

   char           *ptr;
   int            i;
//---------------------------------------------------------------------------


   if (!pFullKeyOrSectionName)
      return (FALSE);

   *pppStrArray      = NULL;
   *piStrArraySize   = 0;

   memset (acString, 0, sizeof(acString));

   // If use-INI-file specified, then information in INI file will
   // override the one in the registry.
   // Try to load string array from INI file.
   if (pIniFilename)
      {
      // Use as default string the one already read from the registry.
      dwBytes =
         GetPrivateProfileString ( // DWORD: success:num of char copied, not incl. \0.
            pFullKeyOrSectionName, // LPCTSTR lpAppName:  points to section name
            pValueName,       // LPCTSTR lpKeyName:  points to key name
            acString,         // LPCTSTR lpDefault:  points to default string
            acString,         // LPTSTR lpReturnedString:  points to destination buffer
            sizeof(acString), // DWORD nSize:  size of destination buffer
            pIniFilename      // LPCTSTR lpFileName: points to initialization filename
            );

      // Convert INI-style 'pString2'(where small strings separated with '|') to
      // registry-style 'REG_MULTI_SZ'.
      for (i=0; acString[i]; i++)
         if (acString[i] == '|')
            acString[i] = 0;
      if (i < sizeof(acString))
         acString[i+1] = 0; // Finally make array of stringZ \0\0 - terminated.

      bRetCode = TRUE;
      }
   else
      {
      dwBytes = sizeof (acString);

      // Now load the actual key...
      bRetCode =
         HLP_RegLoadValue ( // BOOL: TRUE:success, FALSE:failure.
            hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            pFullKeyOrSectionName,   // const char  *pKeyName:     "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
            pValueName, // const char  *pValueName:   "Include Subdirectories"
            acString,    // char        *pData,
            &dwBytes    // Address of DWORD where: size of 'pData'/bytes copied. if pData=NULL => to receive size of data required.
            /* =NULL */ // char        *pDefaultData=NULL,  // Default data buffer if key is not in registry.
            /* =0    */ // DWORD       dwDefaultDataSize=0  // Size of default data buffer. 'pData' must be large enough to get 'dwDefaultDataSize' bytes.
            );
      }


   // Ok, now read elements from a received buffer one-by-one and add it
   // to user's array.

   for (ptr = acString; ptr[0]; ptr += strlen(ptr)+1)
      AddStringToArray (pppStrArray, piStrArraySize, ptr);

   return (bRetCode);
}
//===========================================================================


//===========================================================================
//
// 'ppString' - array of pointers to strings.
// 'iStringsNum' - number of pointers in array.

BOOL  SaveValue_StringArray  (
         HKEY        hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pFullKeyOrSectionName, // Full name of registry key(if 'pIniFilename'=NULL) or name of section in INI file.
         const char  *pValueName,
         char        **ppString,
         int         iStringsNum,
         const char  *pIniFilename /*=NULL*/ // Name of INI file, if INI file must be used.
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode;
   int            iTotalDataLen;
   char           *pData, *ptr;
   int            i;
//---------------------------------------------------------------------------


   if (!pFullKeyOrSectionName)
      return (FALSE);

   // Let calculate first the total length of a string, required to store an
   // array of stringZ,\0...
   for (iTotalDataLen=0, i=0; i<iStringsNum; i++)
      iTotalDataLen += strlen (ppString[i]) + 1;

   iTotalDataLen ++;  // Final buffer must have \0\0 at the end.

   if (iTotalDataLen < 2)
      iTotalDataLen = 2;

   pData = new char [iTotalDataLen];

   if (!pData)
      return (FALSE);

   // Now copy strings to the 'pData' and separate each string with \0.
   for (pData[0]=0, pData[1]=0, i=0, ptr=pData; i<iStringsNum; i++)
      {
      strcpy (ptr, ppString[i]);
      ptr += strlen (ppString[i]) + 1;
      }


   *ptr = 0;   // Make it \0\0 - terminated.


   if (pIniFilename)
      {
      // Now copy strings to the 'pData' and separate each string with '|' -
      // for INI-compatible format.
      //
      for (pData[0]=0, i=0; i<iStringsNum; i++)
         {
         if (i)
            strcat (pData, "|");
         strcat (pData, ppString[i]);
         }

      // Save in into INI file now...
      bRetCode =
         WritePrivateProfileString ( // BOOL: 0:fail, else:success.
            pFullKeyOrSectionName,      // LPCTSTR lpAppName:  pointer to section name
            pValueName,          // LPCTSTR lpKeyName:  pointer to key name
            pData,               // LPCTSTR lpString:   pointer to string to add
            pIniFilename         // LPCTSTR lpFileName: pointer to initialization filename
            );
      }
   else
      {
      bRetCode =
         HLP_RegSaveValue ( // BOOL: TRUE:success, FALSE:failure.
            hBaseKeyHandle,   // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
            pFullKeyOrSectionName,      // const char  *pKeyName:     "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
            pValueName,    // const char  *pValueName:   "Include Subdirectories"
            REG_MULTI_SZ,  // DWORD       dwValueType:   REG_[DWORD|SZ|MULTI_SZ|BINARY...].
            pData,         // char        *pData,
            iTotalDataLen  // DWORD       dwDataSize
            );
      }

   delete [] pData;

   return (bRetCode);
}
//===========================================================================

//===========================================================================
//
// To query how many bytes needed to retrieve data, pass NULL for 'pData'
// parameter.
//
// Returns:
//    TRUE:    loaded successfully.
//    FALSE:   failed.

BOOL  HLP_RegLoadValue (
         HKEY        hBaseKeyHandle,// HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pKeyName,     // "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
         const char  *pValueName,   // "Include Subdirectories"
         char        *pData,        // Where to load data or NULL if data size query.
         DWORD       *pdwDataSize,  // Address of DWORD where: size of 'pData'/bytes copied. if pData=NULL => to receive size of data required.
         char        *pDefaultData     /*=NULL */,  // Default data buffer if key is not in registry.
         DWORD       dwDefaultDataSize /*=0    */   // Size of default data buffer. 'pData' must be large enough to get 'dwDefaultDataSize' bytes.
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode;
   LONG           lRetCode;
   HKEY           hKey           = (HKEY)INVALID_HANDLE_VALUE;
   DWORD          dwValueType;
//---------------------------------------------------------------------------


   // The RegOpenKeyEx function opens the specified key.
   // Does not create the key, if not exists.
   lRetCode =
      RegOpenKeyEx ( // LONG: ERROR_SUCCESS:success, else:error.
         hBaseKeyHandle,            // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         pKeyName,                  // LPCTSTR lpSubKey:   address of name of subkey to open
         0,                         // DWORD ulOptions: reserved, must be 0.
         KEY_READ,                  // REGSAM samDesired:  security access mask
         &hKey                      // PHKEY phkResult: address of handle of open key
         );
   if (lRetCode != ERROR_SUCCESS)
      goto  ABORT;

   // Retrieves the type and data for a specified value name associated
   // with an open registry key.
   lRetCode =
      RegQueryValueEx ( // LONG: ERROR_SUCCESS:success, else:error.
         hKey,          // HKEY hKey: handle of key to query
         pValueName,    // LPTSTR lpValueName: address of name of value to query
         0,             // LPDWORD lpReserved: reserved
         &dwValueType,  // LPDWORD lpType:  address of buffer for value type
         (LPBYTE)pData, // LPBYTE lpData:   address of data buffer
         pdwDataSize    // LPDWORD lpcbData:   address of data buffer size / copied bytes.
         );
   if (lRetCode != ERROR_SUCCESS)
      goto  ABORT;

   goto  EXIT;


ABORT:
   bRetCode = FALSE;
   if (dwDefaultDataSize && dwDefaultDataSize<=(*pdwDataSize) && pDefaultData)
      memcpy (pData, pDefaultData, dwDefaultDataSize);
   else
      if (pdwDataSize)
         *pdwDataSize = 0;
   goto  DONE;

EXIT:
   bRetCode = TRUE;
   goto  DONE;

DONE:
   // LONG: ERROR_SUCCESS:success, else:error.
   if (hKey && hKey != INVALID_HANDLE_VALUE)
      RegCloseKey (hKey);
   return (bRetCode);
}
//===========================================================================

//===========================================================================
//
// If 'pValueName' is NULL, then request is to create a key in the registry.
//
// Returns:
//    TRUE:    saved successfully
//    FALSE:   failed.

BOOL  HLP_RegSaveValue (
         HKEY        hBaseKeyHandle,// HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         const char  *pKeyName,     // "Software\MENSK Technologies Inc\BXCopy\1.0\Source Directories\E:/PROJECTS/MYPRJ"
         const char  *pValueName,   // "Include Subdirectories"
         DWORD       dwValueType,   // REG_[DWORD|SZ|MULTI_SZ|BINARY...].
         const char  *pData,
         DWORD       dwDataSize     // size of data [incl. \0]
         )
{
//---------------------------------------------------------------------------
   BOOL           bRetCode;
   LONG           lRetCode;
   HKEY           hKey           = (HKEY)INVALID_HANDLE_VALUE;
   DWORD          dwDisposition;
//---------------------------------------------------------------------------


   // The RegCreateKeyEx function creates the specified key.
   // If the key already exists in the registry, the function opens it.
   //
   lRetCode =
      RegCreateKeyEx ( // LONG: ERROR_SUCCESS:success, else:error.
         hBaseKeyHandle,            // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         pKeyName,                  // LPCTSTR lpSubKey:   address of subkey name
         0,                         // DWORD Reserved:  reserved. Must be 0.
         "BXCOPY",                  // LPTSTR lpClass:  address of class string
         REG_OPTION_NON_VOLATILE,   // DWORD dwOptions: special options flag
         KEY_ALL_ACCESS,            // REGSAM samDesired:  desired security access
         NULL,                      // LPSECURITY_ATTRIBUTES lpSecurityAttributes: address of key security structure
         &hKey,                     // PHKEY phkResult: address of buffer for opened handle
         &dwDisposition             // LPDWORD lpdwDisposition:  address of var. to receive one of: REG_[CREATED_NEW_KEY|OPENED_EXISTING_KEY].
         );
   if (lRetCode != ERROR_SUCCESS)
      goto  ABORT;


   // If 'pValueName' was NULL, then request was basically to create a key in
   // the registry.
   //
   if (pValueName)
      {
      // The RegSetValueEx function stores data in the value field of an
      // open registry key. It can also set additional value and type information
      // for the specified key.
      //
      lRetCode =
         RegSetValueEx ( // LONG: ERROR_SUCCESS:success, else:error.
            hKey,          // HKEY hKey: handle of key to set value for
            pValueName,    // LPCTSTR lpValueName:   address of value to set
            0,             // DWORD Reserved:  reserved
            dwValueType,   // DWORD dwType: flag for value type: REG_[DWORD|SZ|MULTI_SZ|BINARY...].
            (CONST BYTE *)pData, // CONST BYTE *lpData: address of value data
            dwDataSize     // DWORD cbData: size of value data [incl. \0]
            );
      if (lRetCode != ERROR_SUCCESS)
         goto  ABORT;
      }

   goto  EXIT;


ABORT:
   bRetCode = FALSE;
   goto  DONE;

EXIT:
   bRetCode = TRUE;
   goto  DONE;

DONE:
   // LONG: ERROR_SUCCESS:success, else:error.
   if (hKey && hKey != INVALID_HANDLE_VALUE)
      RegCloseKey (hKey);
   return (bRetCode);
}
//===========================================================================

//===========================================================================
//
// Function returns local time.

VOID  GetLocalSystemTime (SYSTEMTIME *pst)
{
   GetLocalTime (pst);
}
//===========================================================================

//===========================================================================
//
// Converts string to SYSTEMTIME structure.

//                                 1997/02/14 (5) 21:45:59.999
const char  *g_pSystemTimeFormat = "%d/%02d/%02d (%d) %02d:%02d:%02d.%03d";

VOID  StringToSystemTime   (const char *pString, SYSTEMTIME *pst)
{
//---------------------------------------------------------------------------
   int   wYear,wMonth,wDay,wDayOfWeek,wHour,wMinute,wSecond,wMilliseconds;
//---------------------------------------------------------------------------

   // Have to do it this way, because sscanf will assume 4 bytes for each var.
   sscanf (pString, g_pSystemTimeFormat,
         &wYear,
         &wMonth,
         &wDay,
         &wDayOfWeek,
         &wHour,
         &wMinute,
         &wSecond,
         &wMilliseconds
         );

   pst->wYear           =  (WORD)wYear        ;
   pst->wMonth          =  (WORD)wMonth       ;
   pst->wDay            =  (WORD)wDay         ;
   pst->wDayOfWeek      =  (WORD)wDayOfWeek   ;
   pst->wHour           =  (WORD)wHour        ;
   pst->wMinute         =  (WORD)wMinute      ;
   pst->wSecond         =  (WORD)wSecond      ;
   pst->wMilliseconds   =  (WORD)wMilliseconds;
}
//===========================================================================


//===========================================================================
//
// Converts SYSTEMTIME structure to string representation.

VOID  SystemTimeToString   (const SYSTEMTIME *pst, char *pString)
{
   sprintf (pString, g_pSystemTimeFormat,
         pst->wYear,
         pst->wMonth,
         pst->wDay,
         pst->wDayOfWeek,
         pst->wHour,
         pst->wMinute,
         pst->wSecond,
         pst->wMilliseconds
         );
}
//===========================================================================


//===========================================================================
//
// Converts SYSTEMTIME structure to string representation.
// Uses second version of format.
//
char *g_pMonths [] =     {
"???", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

char *g_pWeekDays [] =   {
"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

//                                  02 Nov 1997, 12:34:56.789
const char  *g_pSystemTimeFormat2 = "%02d %s %d, %02d:%02d:%02d.%03d";

VOID  SystemTimeToString2   (const SYSTEMTIME *pst, char *pString)
{
   if (!pst->wDay || !pst->wMonth)
      strcpy  (pString, "-- --- ----  --:--:--.---");
   else
      sprintf (pString, g_pSystemTimeFormat2,
            pst->wDay,
            g_pMonths[((pst->wMonth>12)||(pst->wMonth<0))?0:pst->wMonth],
            pst->wYear,
            pst->wHour,
            pst->wMinute,
            pst->wSecond,
            pst->wMilliseconds
            );
}
//===========================================================================

//===========================================================================
//
// Function converts current date/time to string representation:
//                                  "02 Nov 1997, 12:34:56.789"

VOID  CurrentDateTimeToString2 (char *pRetStr)
{
//---------------------------------------------------------------------------
   SYSTEMTIME  st;
//---------------------------------------------------------------------------

   GetLocalSystemTime  (&st);
   SystemTimeToString2 (&st, pRetStr);
}
//===========================================================================

//===========================================================================
//
// Function converts current date/time to string representation:
//                                  "Tue, 21 Dec 1999 20:19:11 GMT"

const char  *g_pSystemTimeFormat3 = "%s, %02d %s %d %02d:%02d:%02d GMT";

VOID  SystemTimeToString3 (const SYSTEMTIME *pst, char *pString)
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

   if (!pst->wDay || !pst->wMonth)
      strcpy  (pString, "-- --- ----  --:--:--.---");
   else
      sprintf (pString, g_pSystemTimeFormat3,
            g_pWeekDays[pst->wDayOfWeek],
            pst->wDay,
            g_pMonths[((pst->wMonth>12)||(pst->wMonth<0))?0:pst->wMonth],
            pst->wYear,
            pst->wHour,
            pst->wMinute,
            pst->wSecond
            );
}
//===========================================================================

//===========================================================================
//
// Function converts FILETIME to string representation:
//                             "Tue, 21 Dec 1999 20:19:11 GMT"

VOID  FileTimeToString3 (const FILETIME *pft, char *pString)
{
//---------------------------------------------------------------------------
   SYSTEMTIME  st;
//---------------------------------------------------------------------------

   FileTimeToSystemTime ( // BOOL
      pft,  // CONST FILETIME *lpFileTime:  pointer to file time to convert
      &st   // LPSYSTEMTIME lpSystemTime:   pointer to structure to receive system time
      );

   SystemTimeToString3 (&st, pString);
}
//===========================================================================

//===========================================================================
//
// Function converts current date/time to string representation:
//                                  "02 Nov 1997, 12:34:56.789"

VOID  CurrentDateTimeToString3 (char *pRetStr)
{
//---------------------------------------------------------------------------
   SYSTEMTIME  st;
//---------------------------------------------------------------------------

   GetLocalSystemTime  (&st);
   SystemTimeToString3 (&st, pRetStr);
}
//===========================================================================

//===========================================================================
//
// Converts day/mo/yr -> "02 Sep 1997"
//                                  02 Nov 1997, 12:34:56.789
const char  *g_pDMYFormat = "%02d %s %d";

VOID  DayMonthYearToString (int iDay, int iMonth, int iYear, char *pString)
{
   sprintf (
      pString,
      g_pDMYFormat,
      iDay,
      g_pMonths[((iMonth>12)||(iMonth<0))?0:iMonth],
      iYear
      );
}
//===========================================================================

//===========================================================================
//
// Note: this function is updated vs. function for INSTALL.EXE program.
// Function used to expand SUBST-ed drives into the real path.
// Result of conversion will be stored back into 'pszPath' buffer, which
// must be big enough.

VOID  ConvertPathToRealPath (char *pszPath)
{
//---------------------------------------------------------------------------
   char           acBuffer     [MAX_PATH];
   char           acDestBuffer [MAX_PATH];
   char           acDrive[3];
   DWORD          dwRetCode;
//---------------------------------------------------------------------------

   // Make path is started with the drive:
   if (pszPath[1] != ':')
      return;

   acDrive[0] = pszPath[0];
   acDrive[1] = pszPath[1];
   acDrive[2] = 0;

   // '\??\E:\projects' --------- NT 4.0
   // '\DosDevices\E:\projects' - NT 3.51
   dwRetCode =
      QueryDosDevice ( // DWORD: 0=>failure, else number of chars in 'lpTargetPath'.
         acDrive,    // LPCTSTR lpDeviceName:  address of MS-DOS device name string
         acBuffer,   // LPTSTR lpTargetPath:   address of buffer for storing query results
         sizeof (acBuffer) // DWORD ucchMax:   maximum storage capacity of buffer
         );

   if (!dwRetCode)
      return;

   // Just make sure it is in a format:
   // '\??\E:\projects'
   // or
   // '\DosDevices\E:\projects'
   if (
      (strncmp (acBuffer, "\\??\\", 4) == 0)          &&
      (acBuffer[5] == ':' && acBuffer[6] == '\\')
      )
      strcpy (acDestBuffer, &acBuffer[4]);
   else if (
      (strncmp (acBuffer, "\\DosDevices\\", 12) == 0) &&
      (acBuffer[13] == ':' && acBuffer[14] == '\\')
      )
      strcpy (acDestBuffer, &acBuffer[12]);
   else
      return;

   // Ok, here we have 'pszPath' based on a SUBST-ed drive.
   // Convert it...

   int   len = strlen (acDestBuffer);
   if (acDestBuffer[len-1] != '\\')
      strcat (acDestBuffer, "\\");

   if (pszPath[2] == '\\')
      strcat (acDestBuffer, &pszPath[3]);
   else
      strcat (acDestBuffer, &pszPath[2]);

   strcpy (pszPath, acDestBuffer);

   // Done.
   return;
}
//===========================================================================

//===========================================================================
//
// Convert possibly weird names, like:
// \xxx           ,
// .\             ,
// xxxx\yyy       ,
// %SystemRoot%\*.DLL
//
// To full: C:\DIR\NAME without terminating backslash.
// Full names will remain the same.
//
// Notes:
// 1. If 'ppFilenamePart' is valid - it will points into the filename part
//    in 'pFullPathName'.
// 2. 'pWeirdName' and 'pFullPathName' may point to the same buffer.

VOID  ConvertWeirdNameToFullPathName (
         const char  *pWeirdName,         // "..\..\.\.\dir1\..\..\dir2\file3
         char        *pFullPathName,      // "c:\dir1\dir2\file3"
         DWORD       dwFullPathNameSize,
         char        **ppFilenamePart     /* = NULL */
         )
{
//---------------------------------------------------------------------------
   char           *pEnvBegins = NULL;
   char           *pEnvEnds   = NULL;
   char           acInputName          [MAX_PATH];
   char           acExpandedInputName  [MAX_PATH];
   char           acEnvString          [MAX_PATH];
   char           acExpandedEnvString  [MAX_PATH];
   char           *pFName;
   DWORD          dwRetCode;
//---------------------------------------------------------------------------


   strcpy (acInputName, pWeirdName);

   do
      {
      // First search for possible environment variable inclusion.
      // Expand environment string, if found.
      //
      pEnvBegins = strchr (acInputName, '%');
      if (pEnvBegins)
         {
         pEnvEnds = strchr (pEnvBegins+1, '%');
         if (pEnvEnds)
            {
            memset  (acEnvString, 0, sizeof(acEnvString));
            strncpy (acEnvString, pEnvBegins+1, pEnvEnds-pEnvBegins-1);
            dwRetCode =
               GetEnvironmentVariable ( // DWORD: 0:fail, else:chars copied
                  acEnvString,         // LPCTSTR lpName:  address of environment variable name
                  acExpandedEnvString, // LPTSTR lpBuffer: address of buffer for variable value
                  sizeof(acExpandedEnvString)// DWORD nSize:  size of buffer, in characters
                  );
            if (dwRetCode)
               {
               memset (acExpandedInputName, 0, sizeof(acExpandedInputName));
               // Copy till the first '%'.
               strncpy (acExpandedInputName, acInputName, pEnvBegins-acInputName);
               strcat  (acExpandedInputName, acExpandedEnvString);
               // Do-Copy from the last '%'.
               strcat  (acExpandedInputName, pEnvEnds+1);

               // Ok, here we got new input name!
               strcpy  (acInputName, acExpandedInputName);
               }
            }
         }
      }
   while (pEnvBegins && pEnvEnds);


   GetFullPathName ( // DWORD
      acInputName,         // LPCTSTR lpFileName:  address of name of file to find path for
      dwFullPathNameSize,  // DWORD nBufferLength: size, in characters, of path buffer
      pFullPathName,       // LPTSTR lpBuffer:     address of path buffer
      &pFName              // LPTSTR *lpFilePart:  address of filename in path
      );

   if (ppFilenamePart)
      *ppFilenamePart = pFName;

   RTrimBackSlash (pFullPathName);

   return;
}
//===========================================================================

//===========================================================================
//
// Function will try erase all files and directories from the specified
// directory.

VOID  WipeTree  (
         const char *pszDirname,
         BOOL        bWipeContents, // =FALSE. TRUE:wipe file contents first.
         BOOL        bUseDoDWipe    // =FALSE. TRUE:Use DoD to wipe file('bWipeContents' must be TRUE).
         )
{
//---------------------------------------------------------------------------
   WIPETREE_TW_DATA  twData;
   char              acRootPath [MAX_PATH];
   DWORD             dwSecsPerCluster=0, dwBytesPerSector=0, dwTmp1, dwTmp2;

   DWORD             dwRetCode;
   BOOL              bRetCode;
//---------------------------------------------------------------------------
// typedef struct _WIPETREE_TW_DATA
// {
//    BOOL        bWipeContents;          // =FALSE. TRUE:wipe file contents first.
//    BOOL        bUseDoDWipe;            // =FALSE. TRUE:Use DoD to wipe file('bWipeContents' must be TRUE).
//    DWORD       dwClusterSize;          // Cluster size (used for wiping unused file slack).
// } WIPETREE_TW_DATA;

   dwRetCode =
      AssembleRootDirName (
         pszDirname,       // char     *pInputName,
         acRootPath,       // char     *pRootDirName,
         sizeof(acRootPath)// DWORD    dwRootDirSize
         );
   if (dwRetCode != HLP_SUCCESS)
      {
      strcpy (acRootPath, pszDirname);
      if (acRootPath[1] == ':')
         {
         acRootPath[2] = '\\';
         acRootPath[3] = 0;
         }
      }

   bRetCode =
      GetDiskFreeSpace ( // BOOL: 0:fail, else:success.
         acRootPath,    // LPCTSTR lpRootPathName:       address of root path
         &dwSecsPerCluster,   // LPDWORD lpSectorsPerCluster:  address of sectors per cluster
         &dwBytesPerSector,   // LPDWORD lpBytesPerSector:     address of bytes per sector
         &dwTmp1,             // LPDWORD lpNumberOfFreeClusters:  address of number of free clusters
         &dwTmp2              // LPDWORD lpTotalNumberOfClusters: address of total number of clusters
         );

   memset (&twData, 0, sizeof(twData));
   twData.bWipeContents = bWipeContents;
   twData.bUseDoDWipe   = bUseDoDWipe;
   twData.dwClusterSize = dwSecsPerCluster * dwBytesPerSector;

   // Execute eraser-treewalk.
   //
   TreeWalk (
      pszDirname, // const char *pszTreeSpec:   Starting tree.
      TW_WipeTree_LevelBack,   // void     (*pFnLevelBack)   (DWORD, char *pathname):   NULL|callback: -- dir level (pathname - before cutting level).
      NULL,                   // void     (*pFnLevelAdded)  (DWORD, char *pathname, char *name):   NULL|callback: ++ dir level (pathame - after level 'name' added).
      NULL,                   // int      (*pFnPerDir)      (DWORD, int DirWhat, char *pathname, char *name):  NULL|dir  found callback.
      TW_WipeTree_PerFile,    // int      (*pFnPerFile)     (DWORD, char *pathname, char *name, WIN32_FIND_DATA *pData):   NULL|file found callback.
      (DWORD)&twData          // DWORD    ulParam1:   Passed to all callback functions.
      );

   // Delete final 'root' directory. 'TW_WipeTree_LevelBack()' won't be called for
   // the root directory.
   // Powerful delete helper.
   DeleteFileEx   (
      pszDirname,       // const char  *pFileOrDirName,  // Name of file or directory to delete.
      FALSE,            // BOOL        bWipeContents,    // =FALSE. (files only) TRUE:wipe file contents first.
      FALSE,            // BOOL        bUseDoDWipe,      // =FALSE. (files only) TRUE:Use DoD to wipe file('bWipeContents' must be TRUE).
      0,                // DWORD       dwClusterSize,    // =0.     (files only) Cluster size of the filesystem, where file is located.
                                       //         Required to wipe unused file slack space('bWipeContents' must be TRUE).
      NULL,             // DWORD       *pdwLastErrorW32, // =NULL.  Ptr to where return WIN32 last error.
      NULL              // DWORD       *pdwLastErrorHLP  // =NULL.  Ptr to where to return HELPERS last error (HLP_ERROR_WRITE_FAILURE)
      );
}
//===========================================================================


//===========================================================================
//
// This function will be called every time tree search will about to
// return back one level. Second parameter passed - full name of directory
// *before* cutting off level.
// This callback will also be executed, after last directory
// under specified tree was found as last attempt to back up one
// level.

void  TW_WipeTree_LevelBack (DWORD /*dwParam*/, char *pszPathDirname)
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

   // That's the only purpose of it!
   // Powerful delete helper.
   DeleteFileEx   (
      pszPathDirname,   // const char  *pFileOrDirName,  // Name of file or directory to delete.
      FALSE,            // BOOL        bWipeContents,    // =FALSE. (files only) TRUE:wipe file contents first.
      FALSE,            // BOOL        bUseDoDWipe,      // =FALSE. (files only) TRUE:Use DoD to wipe file('bWipeContents' must be TRUE).
      0,                // DWORD       dwClusterSize,    // =0.     (files only) Cluster size of the filesystem, where file is located.
                                       //         Required to wipe unused file slack space('bWipeContents' must be TRUE).
      NULL,             // DWORD       *pdwLastErrorW32, // =NULL.  Ptr to where return WIN32 last error.
      NULL              // DWORD       *pdwLastErrorHLP  // =NULL.  Ptr to where to return HELPERS last error (HLP_ERROR_WRITE_FAILURE)
      );
}
//===========================================================================

//===========================================================================
//
// Returns:
//          TW_DEFAULT     -  proceed normal traversing.
//          TW_SUCCESS     -  proceed normal traversing.
//          TW_NO_DEEPER   -  do not go inside of currently reached level.
//          TW_CUT_SEARCH  -  Skip filesearch into found directory.
//                            'pFnPerFile()' won't be called also.
//          TW_EXIT        -  terminate search and exit.

int   TW_WipeTree_PerFile   (DWORD dwParam, char *pszPathFilename, char * /*pszFilename*/, WIN32_FIND_DATA * /*pw32SrcData*/)
{
//---------------------------------------------------------------------------
   WIPETREE_TW_DATA  *ptwData = (WIPETREE_TW_DATA *)dwParam;
//---------------------------------------------------------------------------

// typedef struct _WIPETREE_TW_DATA
// {
//    BOOL        bWipeContents;          // =FALSE. TRUE:wipe file contents first.
//    BOOL        bUseDoDWipe;            // =FALSE. TRUE:Use DoD to wipe file('bWipeContents' must be TRUE).
//    DWORD       dwClusterSize;          // Cluster size (used for wiping unused file slack).
// } WIPETREE_TW_DATA;

   DeleteFileEx   (  // BOOL: 0:fail, else:success.
      pszPathFilename,  // const char  *pFilename,       // Name of file to delete.
      ptwData->bWipeContents, // BOOL        bWipeContents,    // =FALSE. TRUE:wipe file contents first.
      ptwData->bUseDoDWipe,   // BOOL        bUseDoDWipe,      // =FALSE. TRUE:Use DoD to wipe file('bWipeContents' must be TRUE).
      ptwData->dwClusterSize, // DWORD       dwClusterSize,    // =0.     Cluster size of the filesystem, where file is located.
                                    //         Required to wipe unused file slack space('bWipeContents' must be TRUE).
      NULL,                   // DWORD       *pdwLastErrorW32, // =NULL.  Ptr to where return WIN32 last error.
      NULL                    // DWORD       *pdwLastErrorHLP  // =NULL.  Ptr to where to return HELPERS last error (HLP_ERROR_WRITE_FAILURE)
      );

   return (TW_DEFAULT);
}
//===========================================================================

//===========================================================================
//
// Function assembles root dir name, using input file or directory name.
//
// Returns:
//    HLP_SUCCESS, HLP_INVALID_PARAMETER(if input name is too weird).

DWORD AssembleRootDirName (
         const char  *pInputName,
         char        *pRootDirName,
         DWORD       dwRootDirSize
         )
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------

   ConvertWeirdNameToFullPathName (
      pInputName,       // const char  *pWeirdName,         // "..\..\.\.\dir1\..\..\dir2\file3
      pRootDirName,     // char        *pFullPathName,      // "c:\dir1\dir2\file3"
      dwRootDirSize,    // DWORD       dwFullPathNameSize,
      NULL              // char        **ppFilenamePart     /* = NULL */
      );

   if (pRootDirName [1] == ':')
      {
      pRootDirName [2] = '\\';
      pRootDirName [3] = 0;    // C:\DIR1 -> C:\.
      }
   else if (pRootDirName [0] == '\\' && pRootDirName[1] == '\\')
      {
      char  *pPtr;
      pPtr = strchr (pRootDirName+2, '\\');
      if (!pPtr)
         return (HLP_INVALID_PARAMETER); // Weird name?

      // Found beginning of possible share name.
      // Look for end of share name and put there '\\'.
      for (i=1; pPtr[i] && pPtr[i]!='\\' && pPtr[i]!='/'; i++)
         ;

      // Presumably found.
      pPtr[i]   = '\\';
      pPtr[i+1] = 0;
      }
   else
      return (HLP_INVALID_PARAMETER); // Still Weird name?

   return (HLP_SUCCESS);
}
//===========================================================================

//===========================================================================
//
// Function returns pointer to just the filename in a full spec.

char  *JustFilename  (const char *pszFullName, char cDividerChar /* = '\\' */)
{
//---------------------------------------------------------------------------
   char           *pNotifyJustFilename;
//---------------------------------------------------------------------------


   if (!pszFullName || !pszFullName[0])
      return ((char *)pszFullName);

   pNotifyJustFilename = strrchr (pszFullName, cDividerChar);

   if (pNotifyJustFilename)
      pNotifyJustFilename++;              // Next char after last '\'.
   else
      {
      if (pszFullName[1] == ':')
         pNotifyJustFilename = (char *)(pszFullName+2);  // C:FILENAME.EXT
      else
         pNotifyJustFilename = (char *)pszFullName;      // FILENAME.EXT : Its already just the name.
      }


   return (pNotifyJustFilename);
}
//===========================================================================

//===========================================================================
//
// Function returns pointer to the beginning of first dirname in the name
// of file or directory.

char  *JustDirname  (const char *pszFullName)
{
//---------------------------------------------------------------------------
   DWORD          dwRetCode   = 0;
   int            iDirNameIdx = 0;
//---------------------------------------------------------------------------

   dwRetCode = GetDirnameIndexFromName (pszFullName, &iDirNameIdx);
   if (dwRetCode != HLP_SUCCESS)
      return (NULL);

   return (char *)(pszFullName + iDirNameIdx);
}
//===========================================================================

//===========================================================================
//
//

char  *FileExtension (char *pFilename)
{
//---------------------------------------------------------------------------
   int            iNameLen = strlen (pFilename);
   int            i;
//---------------------------------------------------------------------------

   if (iNameLen<3)
      return (&pFilename[iNameLen]);

   if (iNameLen>3)
      i=iNameLen-4;
   else // ==3
      i=iNameLen-3;

   for (; pFilename[i]; i++)
      if (pFilename[i] == '.')
         return (pFilename + i + 1);

   // No extention exists.
   return (&pFilename[iNameLen]);
}
//===========================================================================

//===========================================================================
//
// Function will delete file or directory, stripping HSR attributes first.
//    And optionally (files only):
//    'bWipeContents'=TRUE: => Wipe out the file contents (fill it with 0's).
//    'bUseDoDWipe'  =TRUE:
//       Use DoD 5220.22-M U.S. Government File Wipe standard to wipe out file
//       contents, including:
//       -  Renaming file to random filename.
//       -  Wiping unused file slack.
//       -  Resetting filesize to 0.
//       -  Resetting date and time of file to semi random values.
//    'dwClusterSize' - size of the cluster on file system, where file is located.
//    if 'dwClusterSize' is set to 0, then function will use 4096 for cluster size.
//
//    DoD wipe:
//    1. Write all 1's
//    2. Write all 0's
//    3. Repeat above 3 times.
//    4. Write F6 once.
//    5. Verify last write.
//
// Returns:
//    TRUE:success, else:failure.
//    '*pdwLastErrorW32' and '*pdwLastErrorHLP' will be initialized.
//    Few of possible values for '*pdwLastErrorHLP':
//       HLP_ERROR_CANNOT_RENAME_FILE
//       HLP_ERROR_CANNOT_WIPE_FILE
//       HLP_ERROR_CANNOT_SET_FILESIZE
//       HLP_ERROR_CANNOT_SET_FILETIME
//
// Note: even if function returns TRUE, '*pdwLastErrorHLP' might still contains
//       additional information about error code. For example function might fail
//       to wipe file contents, but succeeded to delete file.

BOOL  DeleteFileEx   (
         const char  *pFileOrDirName,  // Name of file or directory to delete.
         BOOL        bWipeContents,    // =FALSE. (files only) TRUE:wipe file contents first.
         BOOL        bUseDoDWipe,      // =FALSE. (files only) TRUE:Use DoD to wipe file('bWipeContents' must be TRUE).
         DWORD       dwClusterSize,    // =0.     (files only) Cluster size of the filesystem, where file is located.
                                       //         Required to wipe unused file slack space('bWipeContents' must be TRUE).
         DWORD       *pdwLastErrorW32, // =NULL.  Ptr to where return WIN32 last error.
         DWORD       *pdwLastErrorHLP  // =NULL.  Ptr to where to return HELPERS last error (HLP_ERROR_WRITE_FAILURE)
         )
{
//---------------------------------------------------------------------------
   DWORD          dwFileAttributes;

   char           acFilename [MAX_PATH];  // Working Filename.

   int            i;
   UINT           uiRetCode;
   HANDLE         hFile = INVALID_HANDLE_VALUE;
   DWORD          dwFileSize;
   SYSTEMTIME     st;
   FILETIME       ft;

   BOOL           bRetCode = FALSE;
//---------------------------------------------------------------------------

   // Create working filename.
   strcpy (acFilename, pFileOrDirName);

   if (pdwLastErrorW32)
      *pdwLastErrorW32 = 0;
   if (pdwLastErrorHLP)
      *pdwLastErrorHLP = 0;

   // Strip HSR attributes from the destination file, if exist.
   dwFileAttributes = HLP_GetFileAttributes (acFilename);
   if (dwFileAttributes != 0xFFFFFFFF)
      {
      if (
         dwFileAttributes & FILE_ATTRIBUTE_SYSTEM     |
         dwFileAttributes & FILE_ATTRIBUTE_READONLY   |
         dwFileAttributes & FILE_ATTRIBUTE_HIDDEN
         )
         {
         dwFileAttributes &= (~(FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN));

         // Make sure file to-be-replaced is *not* read only or so.
         SetFileAttributes ( // BOOL:
            acFilename,       // LPCTSTR lpFileName:  address of filename
            dwFileAttributes  // DWORD dwFileAttributes: attributes to set
            );
         }
      }


   if (bWipeContents && !(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
      // Here we know, that 'bWipeContents' is TRUE.

      if (bUseDoDWipe)
         // Rename file to the temporary filename and generate new
         // 'acFilename'.
         {
         char  acDirName [MAX_PATH];
         char  acNewFileName [MAX_PATH];
         char  *pFilenamePart;
         char  acPrefix[4];
         for (i=0; i<(sizeof(acPrefix)-1); i++)
            acPrefix[i] = (char) GetRandomNumber ('A', 'Z');
         acPrefix[i] = 0;

         ConvertWeirdNameToFullPathName (
            acFilename,          // const char  *pWeirdName,         // "..\..\.\.\dir1\..\..\dir2\file3
            acDirName,           // char        *pFullPathName,      // "c:\dir1\dir2\file3"
            sizeof (acDirName),  // DWORD       dwFullPathNameSize,
            &pFilenamePart       // char        **ppFilenamePart     /* = NULL */
            );
         *(pFilenamePart-1) = 0; // Make it dirname.

         uiRetCode =
            GetTempFileName ( // UINT: 0:fail, else:numeric value, used in tmp filename.
               acDirName,     // LPCTSTR lpPathName: address of directory name for temporary file
               acPrefix,      // LPCTSTR lpPrefixString:   address of filename prefix
               0,             // UINT uUnique: number used to create temporary filename
               acNewFileName  // LPTSTR lpTempFileName: address of buffer that receives the new filename
               );
         if (uiRetCode != 0)
            {
            // 'GetTempFileName()' claims it creates filename. We have to delete it first.
            DeleteFile (acNewFileName);

            bRetCode =
               MoveFile ( // BOOL: TRUE:success, else:fail.
                  acFilename, // LPCTSTR lpExistingFileName:   address of name of the existing file
                  acNewFileName  // LPCTSTR lpNewFileName:  address of new name for the file
                  );
            // Ok, now we got new filename!
            if (bRetCode)
               strcpy (acFilename, acNewFileName);
            else
               {
               // Cannot rename file? Leave old name for processing.
               // Failed to generate randomized TEMP filename.
               if (pdwLastErrorHLP)
                  *pdwLastErrorHLP = HLP_ERROR_CANNOT_RENAME_FILE;
               }
            }
         else
            {
            // Failed to generate randomized TEMP filename.
            if (pdwLastErrorHLP)
               *pdwLastErrorHLP = HLP_ERROR_CANNOT_RENAME_FILE;
            }
         }

      // Open destination file.
      //
      hFile =
         CreateFile ( // HANDLE: INVALID_HANDLE_VALUE:error, else:handle.
            acFilename,       // LPCTSTR  lpFileName:   address of name of the file
            GENERIC_READ | GENERIC_WRITE,     // DWORD  dwDesiredAccess:   access (read-write) mode
            FILE_SHARE_READ,  // DWORD  dwShareMode: share mode.
            NULL,             // LPSECURITY_ATTRIBUTES  lpSecurityAttributes:   address of security descriptor
            OPEN_EXISTING,    // DWORD  dwCreationDistribution:  how to create
            FILE_FLAG_WRITE_THROUGH, // DWORD  dwFlagsAndAttributes: file attributes
            NULL              // HANDLE  hTemplateFile: handle of file with attributes to copy
            );
      if (hFile == INVALID_HANDLE_VALUE)
         {
         bRetCode = FALSE;
         if (pdwLastErrorW32)
            *pdwLastErrorW32 = GetLastError();

         if (pdwLastErrorHLP)
            *pdwLastErrorHLP = HLP_ERROR_CANNOT_WIPE_FILE;
         goto  DELETE_EXIT;   // Cannot open file? Try to delete it anyways.
         }

      dwFileSize = GetFileSize (hFile, NULL);

      if (bUseDoDWipe)
         {
         // DoD wipe requested.

         // Recalculate file size, considering wiping out of unused file slack.
         if (!dwClusterSize)
            dwClusterSize = 4096;   // Make it 4K by default.

         // Cluster size must be given to wipe slack.
         if (!dwFileSize || (dwFileSize % dwClusterSize))
            dwFileSize = ((dwFileSize / dwClusterSize) + 1) * dwClusterSize;

         for (i=0; i<3; i++)
            {
            // Write all 1's.
            bRetCode =
               SetFileContents   (
                  hFile,            // HANDLE         hFile:      Handle of opened file.
                  0xFF,             // unsigned char  ucByte:     Set contents of file to this value.
                  dwFileSize,       // DWORD          dwFileSize: Number of bytes to write.
                  FALSE,            // BOOL           bVerify: =FALSE. TRUE:verify write operation.
                  pdwLastErrorW32   // DWORD          *pdwLastErrorW32: Ptr to where return WIN32 last error.
                  );
            if (!bRetCode)
               {
               if (pdwLastErrorHLP)
                  *pdwLastErrorHLP = HLP_ERROR_CANNOT_WIPE_FILE;
               break;
               }

            // Write all 0's.
            bRetCode =
               SetFileContents   (
                  hFile,            // HANDLE         hFile:      Handle of opened file.
                  0x00,             // unsigned char  ucByte:     Set contents of file to this value.
                  dwFileSize,       // DWORD          dwFileSize: Number of bytes to write.
                  FALSE,            // BOOL           bVerify: =FALSE. TRUE:verify write operation.
                  pdwLastErrorW32   // DWORD          *pdwLastErrorW32: Ptr to where return WIN32 last error.
                  );
            if (!bRetCode)
               {
               if (pdwLastErrorHLP)
                  *pdwLastErrorHLP = HLP_ERROR_CANNOT_WIPE_FILE;
               break;
               }
            }

         // Write 0xF6 once with verification of last write operation.
         if (bRetCode)
            {
            bRetCode =
               SetFileContents   (
                  hFile,            // HANDLE         hFile:      Handle of opened file.
                  0xF6,             // unsigned char  ucByte:     Set contents of file to this value.
                  dwFileSize,       // DWORD          dwFileSize: Number of bytes to write.
                  TRUE,             // BOOL           bVerify: =FALSE. TRUE:verify write operation.
                  pdwLastErrorW32   // DWORD          *pdwLastErrorW32: Ptr to where return WIN32 last error.
                  );
            if (!bRetCode)
               {
               if (pdwLastErrorHLP)
                  *pdwLastErrorHLP = HLP_ERROR_CANNOT_WIPE_FILE;
               }
            }


         // Reset filesize to 0 before delete operation.
         SetFilePointer ( // DWORD: 0xFFFFFFFF:fail, else:low order of new ptr.
            hFile,      // HANDLE hFile: handle of file
            0,          // LONG lDistanceToMove:  number of bytes to move file pointer
            NULL,       // PLONG lpDistanceToMoveHigh:  address of high-order word of distance to move
            FILE_BEGIN  // DWORD dwMoveMethod: how to move
            );
         bRetCode = SetEndOfFile (hFile);
         if (!bRetCode)
            {
            if (pdwLastErrorHLP)
               *pdwLastErrorHLP = HLP_ERROR_CANNOT_SET_FILESIZE;
            }

         // Set File Time/Date to semi-random values.

   // typedef struct _SYSTEMTIME {  // st
   //     WORD wYear;
   //     WORD wMonth;        // 1-12
   //     WORD wDayOfWeek;    // 0-7
   //     WORD wDay;
   //     WORD wHour;
   //     WORD wMinute;
   //     WORD wSecond;
   //     WORD wMilliseconds;
   // } SYSTEMTIME;
         GetSystemTime (&st);
         st.wYear   = 1990;
         st.wMonth  = 1;
         st.wDayOfWeek = 1;
         st.wDay    = 1;
         st.wHour   = 1;
         st.wMinute = 1;
         st.wSecond = 1;
         st.wMilliseconds = 0;
         SystemTimeToFileTime (&st, &ft);

         bRetCode =
            SetFileTime ( // BOOL: 0:fail, else:success.
               hFile,   // HANDLE hFile: identifies the file
               &ft,     // CONST FILETIME *lpCreationTime: time the file was created
               &ft,     // CONST FILETIME *lpLastAccessTime:  time the file was last accessed
               &ft      // CONST FILETIME *lpLastWriteTime:   time the file was last written
               );
         if (!bRetCode)
            {
            if (pdwLastErrorHLP)
               *pdwLastErrorHLP = HLP_ERROR_CANNOT_SET_FILETIME;
            }
         }
      else
         {
         // If no DoD required - then just erase file contents (fill it with 0's).
         bRetCode =
            SetFileContents   (
               hFile,            // HANDLE         hFile:      Handle of opened file.
               0x00,             // unsigned char  ucByte:     Set contents of file to this value.
               dwFileSize,       // DWORD          dwFileSize: Number of bytes to write.
               FALSE,            // BOOL           bVerify: =FALSE. TRUE:verify write operation.
               pdwLastErrorW32   // DWORD          *pdwLastErrorW32: Ptr to where return WIN32 last error.
               );
         if (!bRetCode)
            {
            if (pdwLastErrorHLP)
               *pdwLastErrorHLP = HLP_ERROR_CANNOT_WIPE_FILE;
            }
         }


      if (hFile != INVALID_HANDLE_VALUE)
         CloseHandle (hFile);
      }


DELETE_EXIT:
   if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      bRetCode = RemoveDirectory (acFilename);
   else
      bRetCode = DeleteFile (acFilename);

   if (!bRetCode)
      {
      DWORD dwLastError = GetLastError();

      if (dwLastError != ERROR_FILE_NOT_FOUND && dwLastError != ERROR_PATH_NOT_FOUND)
         {
         // Try hard! Get ownership of file or dir.
         TakeOwnership (acFilename);

         if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            bRetCode = RemoveDirectory (acFilename);
         else
            bRetCode = DeleteFile (acFilename);

         dwLastError = GetLastError();
         }

      if (!bRetCode)
         {
         if (pdwLastErrorW32)
            *pdwLastErrorW32 = dwLastError;
         }
      }

//EXIT:
   return (bRetCode);
}
//===========================================================================

//==================================================================
//
// Function takes ownership of a file. Used to delete oversecured
// files.
// Code copied from Microsoft WIN32 ref.
//
// Returns:
//    TRUE:    Success.
//    FALSE:   Fail.

extern BOOL  AssertTakeOwnership  (BOOL fEnable);

BOOL TakeOwnership (char *pFilename)
{
//---------------------------------------------------------------------------
   SID_IDENTIFIER_AUTHORITY   g_siaNTAuthority  =  SECURITY_NT_AUTHORITY;
   PSID                       pSIDAliasAdmins   =  NULL;
   SECURITY_DESCRIPTOR        sd;
   BOOL                       bRetCode;
//---------------------------------------------------------------------------


   // Initialize a security descriptor and assign it a NULL
   // discretionary ACL to allow unrestricted access.
   // Assign the security descriptor to a file.

   bRetCode =
      InitializeSecurityDescriptor (
         &sd,
         SECURITY_DESCRIPTOR_REVISION
         );
   if (!bRetCode)
      return (FALSE);


   bRetCode =
      SetSecurityDescriptorDacl (
         &sd,
         TRUE,
         (PACL) NULL,
         FALSE
         );
   if (!bRetCode)
      return (FALSE);


   bRetCode =
      SetFileSecurity (
         pFilename,
         DACL_SECURITY_INFORMATION,
         &sd
         );
   if (bRetCode)
      return (TRUE); // Done!


   // SetFileSecurity fails, => create
   // an Administrator SID and use it to set the owner of
   // the security descriptor.

   bRetCode =
      AllocateAndInitializeSid (
         &g_siaNTAuthority,
         2,                     /* subauthority count  */
                                /* first subauthority */
         SECURITY_BUILTIN_DOMAIN_RID,
                                /* second subauthority */
         DOMAIN_ALIAS_RID_ADMINS,
         0,
         0,
         0,
         0,
         0,
         0,
         &pSIDAliasAdmins
         );
   if (!bRetCode)
      return (FALSE);

   bRetCode =
      SetSecurityDescriptorOwner (
         &sd,
         pSIDAliasAdmins,
         FALSE
         );
   if (!bRetCode)
      {
      FreeSid (pSIDAliasAdmins);
      return  (FALSE);
      }


   // If the following call to SetFileSecurity fails,
   // enable SE_TAKE_OWNERSHIP_NAME in the access token for
   // the current process and try again.

   if (!SetFileSecurity(pFilename,
         OWNER_SECURITY_INFORMATION,
         &sd))
      {
      if (!AssertTakeOwnership(TRUE))
         {
         // Must be administrator to do that!
         FreeSid (pSIDAliasAdmins);
         return FALSE;
         }

      if (!SetFileSecurity(pFilename,
             OWNER_SECURITY_INFORMATION,
             &sd))
         {
         // Must be administrator to do that!
         FreeSid (pSIDAliasAdmins);
         return FALSE;
         }
      }


   // The Administrator is now the owner of the file.
   // Try again to assign a NULL ACL.

   bRetCode =
      SetFileSecurity (
         pFilename,
         DACL_SECURITY_INFORMATION,
         &sd
         );
   AssertTakeOwnership (FALSE);

   FreeSid (pSIDAliasAdmins);

   return  (bRetCode);
}
//==================================================================

//==================================================================
//
//

BOOL  AssertTakeOwnership  (BOOL fEnable)
{
//---------------------------------------------------------------------------
   HANDLE            hToken;
   LUID              TakeOwnershipValue;
   TOKEN_PRIVILEGES  tkp;
//---------------------------------------------------------------------------

   /* Retrieve a handle of the access token.           */

   if (!OpenProcessToken(GetCurrentProcess(),
         TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
         &hToken))
      return FALSE;

   /*
   * Enable the SE_TAKE_OWNERSHIP_NAME privilege or
   * disable all privileges, depending on the fEnable
   * flag.
   */

   if (fEnable)
      {
      if (!LookupPrivilegeValue((LPSTR) NULL,
             SE_TAKE_OWNERSHIP_NAME,
             &TakeOwnershipValue))
         return FALSE;

      tkp.PrivilegeCount = 1;
      tkp.Privileges[0].Luid = TakeOwnershipValue;
      tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

      AdjustTokenPrivileges (
         hToken,
         FALSE,
         &tkp,
         sizeof(TOKEN_PRIVILEGES),
         (PTOKEN_PRIVILEGES) NULL,
         (PDWORD) NULL
         );

      /*
      * The return value of AdjustTokenPrivileges cannot
      * be tested.
      */

      if (GetLastError() != ERROR_SUCCESS)
         return FALSE;
      }
   else
      {
      AdjustTokenPrivileges (
         hToken,
         TRUE,               /* disable all privileges */
         (PTOKEN_PRIVILEGES) NULL,
         (DWORD) 0,
         (PTOKEN_PRIVILEGES) NULL,
         (PDWORD) NULL
         );

      /*
      * The return value of AdjustTokenPrivileges cannot
      * be tested.
      */

      if (GetLastError() != ERROR_SUCCESS)
         return FALSE;
      }

   return TRUE;
}
//==================================================================

//===========================================================================
//
// Function wipes out the file contents with the given byte.
// Note: function does not reset file pointer after the write operation.

BOOL  SetFileContents   (
         HANDLE         hFile,            // Handle of opened file.
         unsigned char  ucByte,           // Set contents of file to this value.
         DWORD          dwFileSize,       // Number of bytes to write.
         BOOL           bVerify,          // =FALSE. TRUE:verify write operation.
         DWORD          *pdwLastErrorW32  // =NULL.  Ptr to where return WIN32 last error.
         )
{
//---------------------------------------------------------------------------
   unsigned char  *pBuffer       =  NULL;
   unsigned char  *pVerifyBuffer =  NULL;
   DWORD          dwBufferSize   = _64K_BUFFER;

   DWORD          dwCurrentPtr;
   DWORD          dwBytes;
   DWORD          dwBytesRead;
   DWORD          dwBytesWritten;
   int            i;

   BOOL           bRetCode =  FALSE;
//---------------------------------------------------------------------------

   if (pdwLastErrorW32)
      *pdwLastErrorW32 = 0;

   pBuffer = new unsigned char [dwBufferSize];
   if (!pBuffer)
      {
      if (pdwLastErrorW32)
         *pdwLastErrorW32 = ERROR_NOT_ENOUGH_MEMORY;
      bRetCode = FALSE;
      goto  EXIT;
      }
   memset (pBuffer, ucByte, dwBufferSize);

   if (bVerify)
      {
      pVerifyBuffer = new unsigned char [dwBufferSize];
      if (!pVerifyBuffer)
         {
         if (pdwLastErrorW32)
            *pdwLastErrorW32 = ERROR_NOT_ENOUGH_MEMORY;
         bRetCode = FALSE;
         goto  EXIT;
         }
      }


   // Set PTR to the beginning of file.
   //
   SetFilePointer ( // DWORD: 0xFFFFFFFF:fail, else:low order of new ptr.
      hFile,      // HANDLE hFile: handle of file
      0,          // LONG lDistanceToMove:  number of bytes to move file pointer
      NULL,       // PLONG lpDistanceToMoveHigh:  address of high-order word of distance to move
      FILE_BEGIN  // DWORD dwMoveMethod: how to move
      );

   for (i=0, dwCurrentPtr=0; i < (int)(dwFileSize/dwBufferSize); i++, dwCurrentPtr+=dwBufferSize)
      {
      bRetCode =
         WriteFile ( // BOOL: fail=>0, else != 0.
            hFile,            // HANDLE hFile: handle to file to write to
            pBuffer,          // LPCVOID lpBuffer:   pointer to data to write to file
            dwBufferSize,     // DWORD nNumberOfBytesToWrite: number of bytes to write
            &dwBytesWritten,  // LPDWORD lpNumberOfBytesWritten: pointer to number of bytes written
            NULL              // LPOVERLAPPED lpOverlapped:   pointer to structure needed for overlapped I/O
            );
      if (!bRetCode || dwBufferSize != dwBytesWritten)
         {
         if (pdwLastErrorW32)
            *pdwLastErrorW32 = GetLastError();
         bRetCode = FALSE;
         goto  EXIT;    // Cannot write.
         }

      if (bVerify)
         {
         SetFilePointer ( // DWORD: 0xFFFFFFFF:fail, else:low order of new ptr.
            hFile,      // HANDLE hFile: handle of file
            dwCurrentPtr,// LONG lDistanceToMove:  number of bytes to move file pointer
            NULL,       // PLONG lpDistanceToMoveHigh:  address of high-order word of distance to move
            FILE_BEGIN  // DWORD dwMoveMethod: how to move
            );

         bRetCode =
            ReadFile ( // BOOL: fail=>0, else != 0.
               hFile,         // HANDLE hFile: handle of file to read
               pVerifyBuffer,    // LPVOID lpBuffer: address of buffer that receives data
               dwBufferSize,     // DWORD nNumberOfBytesToRead:  number of bytes to read
               &dwBytesRead,     // LPDWORD lpNumberOfBytesRead: address of number of bytes read
               NULL              // LPOVERLAPPED lpOverlapped:   address of structure for data
               );

         if (!bRetCode || dwBufferSize != dwBytesRead)
            {
            if (pdwLastErrorW32)
               *pdwLastErrorW32 = GetLastError();
            bRetCode = FALSE;
            goto  EXIT; // Cannot read/verify.
            }

         for (i=0; i<(int)dwBufferSize; i++)
            if (pVerifyBuffer[i] != ucByte)
               {
               if (pdwLastErrorW32)
                  *pdwLastErrorW32 = HLP_ERROR;
               bRetCode = FALSE;
               goto  EXIT; // Verification error.
               }
         }
      }

   // Write remainder bytes.
   dwBytes = dwFileSize % dwBufferSize;
   if (dwBytes)
      {
      bRetCode =
         WriteFile ( // BOOL: fail=>0, else != 0.
            hFile,        // HANDLE hFile: handle to file to write to
            pBuffer,      // LPCVOID lpBuffer:   pointer to data to write to file
            dwBytes,          // DWORD nNumberOfBytesToWrite: number of bytes to write
            &dwBytesWritten,  // LPDWORD lpNumberOfBytesWritten: pointer to number of bytes written
            NULL              // LPOVERLAPPED lpOverlapped:   pointer to structure needed for overlapped I/O
            );
      if (!bRetCode || dwBytes != dwBytesWritten)
         {
         if (pdwLastErrorW32)
            *pdwLastErrorW32 = GetLastError();
         bRetCode = FALSE;
         goto  EXIT;
         }

      if (bVerify)
         {
         SetFilePointer ( // DWORD: 0xFFFFFFFF:fail, else:low order of new ptr.
            hFile,      // HANDLE hFile: handle of file
            dwCurrentPtr,// LONG lDistanceToMove:  number of bytes to move file pointer
            NULL,       // PLONG lpDistanceToMoveHigh:  address of high-order word of distance to move
            FILE_BEGIN  // DWORD dwMoveMethod: how to move
            );

         bRetCode =
            ReadFile ( // BOOL: fail=>0, else != 0.
               hFile,         // HANDLE hFile: handle of file to read
               pVerifyBuffer,    // LPVOID lpBuffer: address of buffer that receives data
               dwBytes,          // DWORD nNumberOfBytesToRead:  number of bytes to read
               &dwBytesRead,     // LPDWORD lpNumberOfBytesRead: address of number of bytes read
               NULL              // LPOVERLAPPED lpOverlapped:   address of structure for data
               );

         if (!bRetCode || dwBytes != dwBytesRead)
            {
            if (pdwLastErrorW32)
               *pdwLastErrorW32 = GetLastError();
            bRetCode = FALSE;
            goto  EXIT; // Cannot read/verify.
            }

         for (i=0; i<(int)dwBytes; i++)
            if (pVerifyBuffer[i] != ucByte)
               {
               if (pdwLastErrorW32)
                  *pdwLastErrorW32 = HLP_ERROR;
               bRetCode = FALSE;
               goto  EXIT; // Verification error.
               }
         }
      }


EXIT:
   delete [] pBuffer;
   delete [] pVerifyBuffer;
   return (bRetCode);
}
//===========================================================================

//===========================================================================
//
// Function returns index of dirname in presumably normalized pathname.

DWORD    GetDirnameIndexFromName (const char *pDirOrFilename, int *piDirNameIdx)
{
//---------------------------------------------------------------------------
   int            i;
//---------------------------------------------------------------------------

   if (pDirOrFilename [1] == ':')
      {
      if (pDirOrFilename [2] == '\\')
         *piDirNameIdx = 3;
      else
         *piDirNameIdx = 2;
      }
   else if (pDirOrFilename [0] == '\\' && pDirOrFilename[1] == '\\')
      {
      char  *pPtr;
      pPtr = strchr (pDirOrFilename+2, '\\');
      if (!pPtr)
         return (HLP_INVALID_PARAMETER); // Weird name?

      // Found beginning of possible share name.
      // Look for end of share name.
      for (i=1; pPtr[i] && pPtr[i]!='\\' && pPtr[i]!='/'; i++)
         ;

      // pPtr[i] = 0 or \. Skip all '\'-s.
      while (pPtr[i] == '\\' || pPtr[i] == '/')
         i++;

      *piDirNameIdx = i + (pPtr-pDirOrFilename);
      }
   else
      return (HLP_INVALID_PARAMETER); // Still Weird name?

   return (HLP_SUCCESS);
}
//===========================================================================

//===========================================================================
//
//

VOID     GetFilenameIndexFromName (const char *pDirOrFilename, int *piFileNameIdx)
{
   *piFileNameIdx = JustFilename (pDirOrFilename) - pDirOrFilename;
}
//===========================================================================

//===========================================================================
//
// Function is supposed to be used instead of WIN32 'GetFileAttributes()'
// call.
// It was determined, that 'GetFileAttributes()' causes strange failures
// for MSWORD 95/97, when files are saved on the network drive.
// Below is the way to get attributes without causing failures.
// It was also determined and bug submitted to Microsoft:
// Calling plain 'GetFileAttributes()' from service causes data corruption
// if file is written to network.
//
// The objectname is converted to Unicode to support DBCS version of WindowsNT
// which allows to user to make a path name upto 260 characters. by T.Y

DWORD    HLP_GetFileAttributes (const char *pszObjectName, WIN32_FIND_DATA *pW32Data /*= NULL*/)
{
//---------------------------------------------------------------------------
   char              *pJustDirname;
   WIN32_FIND_DATA   w32Data;
   HANDLE            hFind;
   DWORD             dwAttributes;
//---------------------------------------------------------------------------

   if (pW32Data)
      memset (pW32Data, 0, sizeof (*pW32Data));

   pJustDirname = JustDirname (pszObjectName);

   // For ROOT directories - use old GetFileAttributes() call, because
   // this one returns 0xFFFFFFFF always for root.
   //
   if (pJustDirname && !pJustDirname[0])
      {
      dwAttributes = GetFileAttributes (pszObjectName);
      if (pW32Data)
         pW32Data->dwFileAttributes = dwAttributes;
      }
   else
      {
      hFind = FindFirstFile (pszObjectName, &w32Data);

      if (hFind != INVALID_HANDLE_VALUE)
         {
         if (pW32Data)
            *pW32Data = w32Data;
         FindClose (hFind);
         dwAttributes = w32Data.dwFileAttributes;
         }
      else
         {
         dwAttributes = 0xFFFFFFFF;
         if (pW32Data)
            pW32Data->dwFileAttributes = dwAttributes;
         }
      }

   return (dwAttributes);
}
//===========================================================================

//===========================================================================
//
// Function will delete the specified registry key recursively.

LONG  HLP_DeleteRegistryKey (HKEY hOpenedParentKey, const char *pszSubKeyNameToDelete)
{
//---------------------------------------------------------------------------
   LONG           lRetCode;
   HKEY           hSubKey;
   DWORD          dwMaxSubKeyLen, dwMaxSubKeyLen1;
   char           *pSubKeyName;
//---------------------------------------------------------------------------


   lRetCode =
      RegDeleteKey ( // LONG: ERROR_SUCCESS:success, else:error.
         hOpenedParentKey,       // HKEY hKey: handle of open key
         pszSubKeyNameToDelete   // LPCTSTR lpSubKey:   address of name of subkey to delete
         );

   if (lRetCode == ERROR_SUCCESS)
      return (lRetCode);   // Deleted.


   // Cannot delete the key. Probably subkeys exists.
   // Open 'pszSubKeyNameToDelete', enumerate and delete all subkeys...

   // Open key-to-be-deleted.
   // The RegOpenKeyEx function opens the specified key.
   // Does not create the key, if not exists.
   lRetCode =
      RegOpenKeyEx ( // LONG: ERROR_SUCCESS:success, else:error.
         hOpenedParentKey,    // HKEY hKey: handle of an open key or: HKEY_[CLASSES_ROOT|CURRENT_USER|LOCAL_MACHINE|USERS].
         pszSubKeyNameToDelete,// LPCTSTR lpSubKey:   address of name of subkey to open
         0,                   // DWORD ulOptions: reserved, must be 0.
         KEY_ALL_ACCESS,      // REGSAM samDesired:  security access mask
         &hSubKey             // PHKEY phkResult: address of handle of open key
         );
   if (lRetCode != ERROR_SUCCESS)
      return (lRetCode);   // Cannot delete the key and cannot open it. Aborting...

   // Get the length of the longest subkey...
   lRetCode =
      RegQueryInfoKey ( // LONG: ERROR_SUCCESS:success, else:error.
         hSubKey,          // HKEY hKey: handle of key to query
         NULL,             // LPTSTR lpClass:  address of buffer for class string
         NULL,             // LPDWORD lpcbClass:  address of size of class string buffer
         NULL,             // LPDWORD lpReserved: reserved
         NULL,             // LPDWORD lpcSubKeys: address of buffer for number of subkeys
         &dwMaxSubKeyLen,  // LPDWORD lpcbMaxSubKeyLen: address of buffer for longest subkey name length, not incl. \0.
         NULL,             // LPDWORD lpcbMaxClassLen:  address of buffer for longest class string length
         NULL,             // LPDWORD lpcValues:  address of buffer for number of value entries
         NULL,             // LPDWORD lpcbMaxValueNameLen: address of buffer for longest value name length
         NULL,             // LPDWORD lpcbMaxValueLen:  address of buffer for longest value data length
         NULL,             // LPDWORD lpcbSecurityDescriptor: address of buffer for security descriptor length
         NULL              // PFILETIME lpftLastWriteTime: address of buffer for last write time
         );
   if (lRetCode != ERROR_SUCCESS)
      {
      RegCloseKey (hSubKey);
      return (lRetCode);   // Cannot delete the key,
      }

   dwMaxSubKeyLen++; // Make it including \0.

   pSubKeyName = new char [dwMaxSubKeyLen];

   do
      {
      dwMaxSubKeyLen1 = dwMaxSubKeyLen;
      // The RegEnumKeyEx function enumerates subkeys of the specified open registry key.
      // The function retrieves information about one subkey each time it is called.
      // Unlike the RegEnumKey function, RegEnumKeyEx retrieves the class name of the subkey
      // and the time it was last modified.
      //
      // Because we are deleting keys, we will repeat this process for index 0, until
      // "no more keys".
      lRetCode =
         RegEnumKeyEx ( // LONG: ERROR_SUCCESS:success, ERROR_NO_MORE_ITEMS:no more keys, else:error.
            hSubKey,       // HKEY hKey: handle of key to enumerate
            0,             // DWORD dwIndex:   index of subkey to enumerate: 0...
            pSubKeyName,   // LPTSTR lpName:   address of buffer for subkey name
            &dwMaxSubKeyLen1,// LPDWORD lpcbName:   address for size of subkey buffer
            NULL,          // LPDWORD lpReserved: reserved
            NULL,          // LPTSTR lpClass:  address of buffer for class string
            NULL,          // LPDWORD lpcbClass:  address for size of class buffer
            NULL           // PFILETIME lpftLastWriteTime: address for time key last written to
            );

      if (lRetCode == ERROR_SUCCESS)
         HLP_DeleteRegistryKey (hSubKey, pSubKeyName);
      }
   while (lRetCode == ERROR_SUCCESS);

   delete [] pSubKeyName;  // Free buffer.

   RegCloseKey (hSubKey);  // Close key-to-be-deleted.

   // Second attempt to delete the key...
   lRetCode =
      RegDeleteKey ( // LONG: ERROR_SUCCESS:success, else:error.
         hOpenedParentKey,       // HKEY hKey: handle of open key
         pszSubKeyNameToDelete   // LPCTSTR lpSubKey:   address of name of subkey to delete
         );

   return (lRetCode);
}
//===========================================================================

//===========================================================================
//
// Function converts 'dwLastError' into text description of error.
// 'pBuffer' is filled with zero-terminated description text.

VOID  GetLastErrorText (char *pBuffer, int iBuffSize, DWORD dwLastError)
{
//---------------------------------------------------------------------------
   DWORD          dwRetCode;
//---------------------------------------------------------------------------

   dwRetCode =
      FormatMessage ( // DWORD: 0:fail, else:chars in output buffer.
         FORMAT_MESSAGE_FROM_SYSTEM,   // DWORD dwFlags:   source and processing options
         NULL,          // LPCVOID lpSource:   pointer to  message source
         dwLastError,   // DWORD dwMessageId:  requested message identifier
         MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),  // DWORD dwLanguageId: language identifier for requested message
         pBuffer,       // LPTSTR lpBuffer: pointer to message buffer
         iBuffSize,     // DWORD nSize:  maximum size of message buffer
         NULL           // va_list *Arguments: address of array of message inserts
         );

   pBuffer[dwRetCode] = 0;
}
//===========================================================================

//===========================================================================

DWORD    StartTimer  (VOID)
{
   return (GetTickCount());
}
//===========================================================================

//===========================================================================
//
// Function returns number of ms elapsed, since StartTimer() call.
DWORD    QueryTimer  (DWORD dwStartingValue)
{
   return (GetTickCount() - dwStartingValue);
}
//===========================================================================

//===========================================================================
//
// Function returns number of ms elapsed, since StartTimer() call.
DWORD    StopTimer  (DWORD dwStartingValue)
{
   return (GetTickCount() - dwStartingValue);
}
//===========================================================================

//===========================================================================
//
// Returns:
//    TRUE: key found and value (possibly empty one) retrieved and
//          Zero-terminated.
//    FALSE: no keys with the given name found.
//
// Example:
//  For buffer:
//  -----------
// "My_Key=01234"
// Function returns: TRUE and "01234"
//
//  For buffer:
//  -----------
// "My_Key="
// Function returns: TRUE and ""
//
//  For buffer:
//  -----------
// "Not_My_Key="
// Function returns: FALSE and ""
//
//  For buffer:
//  -----------
// "    My_Key=1234"
// Function returns: FALSE and ""

BOOL  FindKey (
         const char  *pBuffer,         // Where to search for.
         int         iBufferSize,      // Size of buffer.
         const char  *pKeyName,        // ASCIIZ name of key.
         char        cSeparator,
         BOOL        bCaseSensitive,   // TRUE:case sensitive
         char        *pKeyValueStr,    // Where to return key value.
         int         iKeyValueStrSize  // Size of destination buffer
         )
{
//---------------------------------------------------------------------------
   const char     *pFound;
   int            i, j;
//---------------------------------------------------------------------------

   *pKeyValueStr = 0;

   pFound = pBuffer;

   do
      {
      pFound =
         StrStr  (
            pFound,     // const char  *pBuffer,
            pKeyName,   // const char  *pString,
            iBufferSize - (pFound - pBuffer),// int         iBufferSize,
            bCaseSensitive // BOOL        bCaseSensitive
            );
      if (!pFound)
         return (FALSE);

      if (pFound == pBuffer)
         break;   // Found valid keyname.

      if (pFound[-1] == 0x0A || pFound[-1] == 0x0D)
         break;   // Found valid keyname.

      pFound++;
      }
#pragma  warning (disable : 4127)   // Suppress warning: "Conditional expression is constant".
   while (1);
#pragma  warning (default : 4127)

   // Skip space/tab until separator
   pFound += strlen (pKeyName);
   for (i=0; (pFound+i < pBuffer+iBufferSize) && (pFound[i] == ' ' || pFound[i] == 0x09); i++)
      ;

   if (pFound+i >= pBuffer+iBufferSize)
      return (FALSE);   // End of buffer reached prematurely.

   if (pFound[i] != cSeparator)
      return (FALSE);   // Key is not followed by separator.

   // Separator found! Skip space/tab until actual value.
   for (i++; (pFound+i < pBuffer+iBufferSize) && (pFound[i] == ' ' || pFound[i] == 0x09); i++)
      ;

   if ((pFound+i >= pBuffer+iBufferSize) || (pFound [i] <= ' '))
      return (TRUE);   // End of buffer/line reached, and key is empty.


   // Finally copy key value out.
   for (j=0; j<iKeyValueStrSize && (pFound[i] > ' ') && (pFound+i < pBuffer+iBufferSize); i++, j++)
      pKeyValueStr[j] = pFound[i];

   // Zero-terminate result.
   if (j<iKeyValueStrSize)
      pKeyValueStr[j]   = 0;
   else
      pKeyValueStr[j-1] = 0;


   return (TRUE);
}
//===========================================================================

//===========================================================================
//

DWORD GetRandomDword (VOID)
{
//---------------------------------------------------------------------------
   FILETIME       ft;
//---------------------------------------------------------------------------

   GetSystemTimeAsFileTime (&ft);
   // WCA 2000/05/23 date/time is a lousy random number. So add randomness.
   return ((ft.dwLowDateTime>>4)|(ft.dwHighDateTime<<28)) ^ rand();
}
//===========================================================================
