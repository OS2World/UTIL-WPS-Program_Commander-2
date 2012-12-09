/***********************************************************************\
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1993, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * Utility.c    General functions that are not window procedures.      *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#include        "PC2.h"                 /* User include files */
#include        "Error.h"

#define         _FILE_  "Utility.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;

                                        /* IOCTL constants missing in BSEDEV.H ? */
#define         IOCTL_CHARDEVICE    0x82
#define         DSK_DEVICESTATUS    0x60

typedef struct  _SESSIONTYPE        SESSIONTYPE;
typedef struct  _PGMCONTROL         PGMCONTROL;

struct  _SESSIONTYPE                    /* Structure to convert binary SessionTypes to ASCII and
                                           vice versa */
{
USHORT  usSessionType;                  /* Binary PROG_* values */
UCHAR   *pucSessionType;                /* ASCII PROG_* strings */
};

SESSIONTYPE     SessionType[]=          /* Map 1.80 session types to 1.90 when reading PC2.CFG */
                                        /* Map 1.90 session types to 2.00 when reading PC2.CFG
                                           (OS/2 2.0 is no longer tested, so you may have problems
                                           when starting WIN-OS2 sessions under 2.0) */
                                        /* The strings in this table are matched as substrings
                                           for a line read from PC2.CFG, so be sure that the search
                                           terminates after the first match and that the longer
                                           strings are matched first */
{
    { 1, "PROG_FULLSCREEN"},
    { 2, "PROG_WINDOWABLEVIO"},
    { 3, "PROG_PM"},
    { 4, "PROG_VDM"},
    { 7, "PROG_WINDOWEDVDM"},
    {10, "PROG_WINDOW_REAL"},
                                        /* Does not exist under PC/2 anymore,
    {11, "PROG_WINDOW_PROT"},
                                           so skip it */
    {15, "PROG_31_STDSEAMLESSVDM"},
    {16, "PROG_31_STDSEAMLESSCOMMON"},
    {17, "PROG_31_ENHSEAMLESSVDM"},
    {18, "PROG_31_ENHSEAMLESSCOMMON"},
    {19, "PROG_31_ENH"},
    {20, "PROG_31_STD"},
    {20, "PROG_WINDOW_PROT"},
    {21, "PROG_WPSOBJECT"},
    {22, "PROG_DETACHED"}
};

#define SESSIONTYPESIZE         (sizeof(SessionType)/sizeof(SessionType[0]))

struct  _PGMCONTROL                     /* Structure to convert bitmapped PgmControl style to ASCII and
                                           vice versa */
{
USHORT  usPgmControl;                   /* Bitmapped SSF_* values */
UCHAR   *pucPgmControl;                 /* ASCII SSF_* strings */
};

                                        /* Conversion between binary (previous) and textual (current)
                                           saving of a programs style */
PGMCONTROL      PgmControl[]=
{
    {SSF_CONTROL_VISIBLE,       "SSF_CONTROL_VISIBLE"},
    {SSF_CONTROL_INVISIBLE,     "SSF_CONTROL_INVISIBLE"},
    {SSF_CONTROL_MAXIMIZE,      "SSF_CONTROL_MAXIMIZE"},
    {SSF_CONTROL_MINIMIZE,      "SSF_CONTROL_MINIMIZE"},
    {SSF_CONTROL_NOAUTOCLOSE,   "SSF_CONTROL_NOAUTOCLOSE"},
    {SSF_CONTROL_PASSWORD,      "SSF_CONTROL_PASSWORD"},
    {SSF_CONTROL_AUTOSTART,     "SSF_CONTROL_AUTOSTART"},
    {SSF_CONTROL_SETPOS,        "SSF_CONTROL_SETPOS"}
};

#define PGMCONTROLSIZE          (sizeof(PgmControl)/sizeof(PgmControl[0]))

                                        /* Variables used in (recursive) functions. Declared
                                           here to prevent declaration multiple times */
static UCHAR    Buffer[LENENVENTRY+1];

                                        /* Scheduler initialization data */
UCHAR           *pucSchedulerControl[NUMSCHEDULE]={ "", "Hourly", "Daily", "Weekly", "Monthly", "Once",
                                                    "Save anyway"};
UCHAR           *pucWeekDay[NUMWEEKDAYS]={ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
                                           "Friday", "Saturday" };
UCHAR           *pucMonth[NUMMONTHS]={ "January", "Feburary", "March", "April", "May", "June", "July",
                                       "August", "September", "October", "November", "December" };

/*--------------------------------------------------------------------------------------*\
 * Remove trailing \r and/or \n and\or trailing space characters from a string.         *
 * Req:                                                                                 *
 *      pcString ...... A pointer to a character string to remove \r and \n from        *
 * Returns:                                                                             *
 *      pcString ...... Modified string                                                 *
\*--------------------------------------------------------------------------------------*/
char    *StripTrailing(char *pcString)
{
char    *pcIndex;

for(pcIndex=strchr(pcString, '\0'); pcIndex>=pcString; pcIndex--)
    {
    if((*pcIndex=='\r') || (*pcIndex=='\n') || (*pcIndex==' '))
        {
        *pcIndex='\0';
        continue;
        }
    if(*pcIndex!='\0')
        break;
    }
return(pcString);
}

/*--------------------------------------------------------------------------------------*\
 * Remove leading spaces from a string.                                                 *
 * Req:                                                                                 *
 *      pcString ...... A pointer to a character string to remove leading spaces        *
 * Returns:                                                                             *
 *      pcString ...... Modified string                                                 *
\*--------------------------------------------------------------------------------------*/
char    *StripLeading(char *pcString)
{
char    *pcIndex;

for(pcIndex=pcString; pcIndex && *pcIndex; pcIndex++)
    if(*pcIndex!=' ')
        break;
return(pcIndex);
}

/*--------------------------------------------------------------------------------------*\
 * Procedure to initialize a window and its message queue.                              *
 * Req:                                                                                 *
 *      pHab .......... A pointer to be filled with the anchor block of the window      *
 *      pHmq .......... A pointer to be filled with the message queue of the window     *
 * Returns:                                                                             *
 *      TRUE/FALSE .... If called sucessfully/unsucessfully                             *
\*--------------------------------------------------------------------------------------*/
BOOL    WinStartUp(HAB *pHab, HMQ *pHmq)
{
                                        /* Initialize handle of anchor block */
if((*pHab=WinInitialize(0))==NULLHANDLE)
    return(FALSE);
                                        /* Initialize handle of message queue */
if((*pHmq=WinCreateMsgQueue(*pHab, 250))==NULLHANDLE)
    return(FALSE);
return(TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * Procedure to initialize HELP.                                                        *
 * Req:                                                                                 *
 *      hab ........... Anchor block handle                                             *
 *      hmodLibrary ... Library to load resources from                                  *
 *      pHelpFile ..... A pointer to helppanel filename, located in PC/2's directory    *
 *      pHwndHelp ..... A pointer to a HWND structure                                   *
 *      pHwndFrame .... Frame window handle to associate help with                      *
 * Returns:                                                                             *
 *      pHwndHelp ..... If called sucessfully/unsucessfully hwnd/NULL                   *
\*--------------------------------------------------------------------------------------*/
BOOL    WinStartHelp(HAB hab, HMODULE hmodLibrary, UCHAR *pHelpFile, HWND *pHwndHelp, HWND HwndFrame)
{
HELPINIT        HelpInit;

HelpInit.cb=sizeof(HELPINIT);           /* Size of HELPINIT structure */
HelpInit.ulReturnCode=0;                /* Returnvalue from HelpManager */
HelpInit.pszTutorialName=NULL;          /* No tutorial */
                                        /* Ressource of Helptable */
HelpInit.phtHelpTable=(PHELPTABLE)MAKEULONG(MAIN_HELP_TABLE, 0xffff);
                                        /* Module containing resources */
HelpInit.hmodHelpTableModule=hmodLibrary;
                                        /* No handle */
HelpInit.hmodAccelActionBarModule=NULLHANDLE;
HelpInit.idAccelTable=0;                /* None */
HelpInit.idActionBar=0;                 /* None */
                                        /* Window title of help window */
HelpInit.pszHelpWindowTitle="PC/2 - Program Commander/2 Help";
HelpInit.pszHelpLibraryName=pHelpFile;  /* Library name of help panel via PC/2 directory */
HelpInit.fShowPanelId=0;                /* Panel ID not displayed */
/*                                                                                      *\
 * First assume PC2.HLP where the fully qualified path passed here points to, which is  *
 * the directory PC/2 started from.                                                     *
\*                                                                                      */
*pHwndHelp=WinCreateHelpInstance(       /* Create help */
    hab,                                /* Anchor block */
    &HelpInit);
                                        /* Test for successful help creation */
if((*pHwndHelp) && (!HelpInit.ulReturnCode))
                                        /* Associate HELP with frame window */
    if(WinAssociateHelpInstance(*pHwndHelp, HwndFrame)!=FALSE)
        return(TRUE);
/*                                                                                      *\
 * Second assume PC2.HLP in a directory pointed to by the HELP environment variable,    *
 * and try to create it from there.                                                     *
\*                                                                                      */
HelpInit.ulReturnCode=0;                /* Returnvalue from HelpManager */
HelpInit.pszHelpLibraryName="PC2.HLP";  /* Library name of help panel via HELP path */
*pHwndHelp=WinCreateHelpInstance(hab, &HelpInit);
if((*pHwndHelp) && (!HelpInit.ulReturnCode))
    if(WinAssociateHelpInstance(*pHwndHelp, HwndFrame)!=FALSE)
        return(TRUE);
*pHwndHelp=NULLHANDLE;
return(FALSE);
}

/*--------------------------------------------------------------------------------------*\
 * Procedure to close a window and its message queue.                                   *
 * Req:                                                                                 *
 *      pHwndHelp ..... A pointer to HELP window handle                                 *
 *      pHab .......... A pointer to extract the anchor block of the window             *
 *      pHmq .......... A pointer to extract message queue of the window                *
 * Returns:                                                                             *
 *      TRUE/FALSE .... If called sucessfully/unsucessfully                             *
\*--------------------------------------------------------------------------------------*/
BOOL    WinCloseDown(HWND *pHwndHelp, HAB *pHab, HMQ *pHmq)
{
if(*pHwndHelp)                          /* Release HELP */
    WinDestroyHelpInstance(*pHwndHelp);
if(*pHmq)                               /* Release handle of message queue */
    WinDestroyMsgQueue(*pHmq);
if(*pHab)                               /* Release handle of anchor block */
    WinTerminate(*pHab);
                                        /* Any error during WinStartUp */
if((*pHab==NULLHANDLE) || (*pHmq==NULLHANDLE)) return(FALSE);
else return(TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * Allocate a storage area that contains the source environment inclusive additions/    *
 * replacements made from the data entered in the Envrionment notebook page.            *
 * This function is designed to be used to add n Environment spaces to PC/2's original  *
 * environment and then add the session specific modifications to the environment       *
 * merged in the steps before.                                                          *
 * Req:                                                                                 *
 *      SrcEnvironment  A pointer to the source Environment in the form:                *
 *                      EnvVar1=Value1\r\nEnvVar2=Value2\r\n...\0                       *
 *                      which is going to be enhanced/modified by the environment       *
 *                      specified by Pgm(Leading/Trailing)Environment and the Environ-  *
 *                      ment Spaces                                                     *
 *      PgmEnvironment  A pointer to the user entered Environment in the form of:       *
 *                      EnvVar1=Value1\r\nEnvVar2=Value2\r\n...\0                       *
 * Returns:                                                                             *
 *      pucEnvironment  A pointer to the combined environment to inherit into the       *
 *                      started session, which must be freed after the session was      *
 *                      started, in the form of:                                        *
 *                      EnvVar1=Value1\r\nEnvVar2=Value2\r\n...\0                       *
\*--------------------------------------------------------------------------------------*/
UCHAR   *CreateEnvironment(PSZ SrcEnvironment, PSZ PgmEnvironment)
{
UCHAR   *pucEnvironment;                /* Merge of source's and user added/replaced environment */
UCHAR   *pucEnvironmentEntry;           /* Pointer within pucEnvironment to add next entry */
UCHAR   **ppucEnvironment;              /* Temporary table containing all EnvironmentVariable=Value */
UCHAR   **ppucEnvironmentEntry;         /* Pointer within ppucEnvironment to add next entry
                                           entry pointers */
ULONG   ulEnvironmentSize;              /* Number of bytes required for all EnvironmentVariable=Value
                                           strings that get inherited to the started session, after
                                           converting the \r\n-terminated format to the \0-terminated
                                           format (excluding \r\n or \n) */
ULONG   ulEnvironmentEntries;           /* Number of EnvironmentVariable=Value entries that are
                                           to be returned in pucEnvironment at maximum */
UCHAR   *pucSrcEnvironmentEntry;        /* Pointer to an entry in source's environment */
UCHAR   *pucUserEnvironmentEntry;       /* Pointer to an EnvironmentVariable in user's environment
                                           additions/replacements */
UCHAR   *pucUserEnvironmentEntryValue;  /* Pointer to an Value in user's environment additions/replacements */
UCHAR   *pucMatch;

#ifdef  DEBUG_UTILITY
printf("Entering CreateEnvironment\n");
#endif  /* DEBUG_UTILITY */
                                        /* Count entries and their length of the source environment */
for(ulEnvironmentEntries=0, pucSrcEnvironmentEntry=SrcEnvironment;
    pucSrcEnvironmentEntry;
    ulEnvironmentEntries++, pucSrcEnvironmentEntry=strstr(pucSrcEnvironmentEntry, "\r\n"),
        (pucSrcEnvironmentEntry ? pucSrcEnvironmentEntry+=2 : pucSrcEnvironmentEntry))
    {
    ;                                   /* Nop */
    }
                                        /* Save PC/2's environment size, as all \r\n terminations are
                                           allready included, we don't have to add space for \r\n for the
                                           flat string */
ulEnvironmentSize=strlen(SrcEnvironment);
                                        /* Count entries and their length of user's environment
                                           additions/replacements */
for(pucUserEnvironmentEntry=PgmEnvironment;
    pucUserEnvironmentEntry;
    ulEnvironmentEntries++, pucUserEnvironmentEntry=strstr(pucUserEnvironmentEntry, "\r\n"),
        (pucUserEnvironmentEntry ? pucUserEnvironmentEntry+=2 : pucUserEnvironmentEntry))
    {
    ;                                   /* Nop */
    }
                                        /* Add 2 entries for the dynamic LIBPATH settings, because even
                                           if BEGINLIBPATH and ENDLIBPATH are part of the environment, we
                                           treat them syntactically as environment entries for consitency
                                           with the OS/2 command processor CMD.EXE. Add 1 entry for the
                                           %PC2% macro which points to the path PC/2 was launched from.
                                           Add 1 for the %BOOTDRIVE% macro for the bootdrive. Add 1
                                           for the %CDDRIVE% macro for the first CD-ROM drive. */
    ulEnvironmentEntries+=5;
                                        /* Allocate the temporary table to merge source's and
                                           user entered environment entries. Because we allow
                                           the following:
                                           source's environment: INCLUDE=......
                                                                 CINCLUDE=......
                                                                 CPLUSINCLUDE=......
                                           User's addition: INCLUDE=%CINCLUDE%......
                                           the maximum number of environment entries is the sum
                                           between the number of entries in source's and the user's
                                           environment (only when both are disjoint), otherwise
                                           we waste some space, which should be no problem, as
                                           all environment space is freed after the application
                                           has been started. The array is terminated by a NULL
                                           pointer, so allocate one element more */
ppucEnvironment=ppucEnvironmentEntry=(UCHAR **)malloc((ulEnvironmentEntries+1)*sizeof(UCHAR *));
*ppucEnvironmentEntry=0;
                                        /* Copy source environment into temporary table, from which
                                           additional user environment entries will be added or
                                           existing one will be replaced */
for(pucSrcEnvironmentEntry=SrcEnvironment;
    (pucSrcEnvironmentEntry && *pucSrcEnvironmentEntry);
    pucSrcEnvironmentEntry=strstr(pucSrcEnvironmentEntry, "\r\n"),
        (pucSrcEnvironmentEntry ? pucSrcEnvironmentEntry+=2 : pucSrcEnvironmentEntry))
    {
    ULONG   ulSrcEnvironmentEntrySize;

    ulSrcEnvironmentEntrySize=0;
                                        /* Get the length of current EnvironmentVariable=Value
                                           entry in the source environment */
    pucMatch=strstr(pucSrcEnvironmentEntry, "\r\n");
    if(pucMatch) ulSrcEnvironmentEntrySize=pucMatch-pucSrcEnvironmentEntry;
                                        /* Copy the source environment entry */
    *ppucEnvironmentEntry=malloc(ulSrcEnvironmentEntrySize+1);
    memcpy(*ppucEnvironmentEntry, pucSrcEnvironmentEntry, ulSrcEnvironmentEntrySize);
    (*ppucEnvironmentEntry)[ulSrcEnvironmentEntrySize]='\0';
    *(++ppucEnvironmentEntry)=NULL;
    }
                                        /* Now add the dynamic LIBPATH settings to the environment,
                                           if available */
    if(pHP->DosQueryExtLIBPATH)
        {
        if(pHP->pucBeginLibpath)
            {
            *ppucEnvironmentEntry=strdup(pHP->pucBeginLibpath);
            ulEnvironmentSize+=strlen(*ppucEnvironmentEntry)+2;
            *(++ppucEnvironmentEntry)=NULL;
            }
        if(pHP->pucEndLibpath)
            {
            *ppucEnvironmentEntry=strdup(pHP->pucEndLibpath);
            ulEnvironmentSize+=strlen(*ppucEnvironmentEntry)+2;
            *(++ppucEnvironmentEntry)=NULL;
            }
        }
                                        /* Now add the expanded %PC2% macro */
    if(pHP->ucPathDLL)
        {
        *ppucEnvironmentEntry=malloc(strlen("PC2=")+strlen(pHP->ucPathDLL)+1);
        strcpy(*ppucEnvironmentEntry, "PC2=");
        strcat(*ppucEnvironmentEntry, pHP->ucPathDLL);
        ulEnvironmentSize+=strlen(*ppucEnvironmentEntry)+2;
        *(++ppucEnvironmentEntry)=NULL;
        }
                                        /* Now add the expanded %BOOTDRIVE% macro */
    *ppucEnvironmentEntry=malloc(strlen("BOOTDRIVE=?:")+1);
    strcpy(*ppucEnvironmentEntry, "BOOTDRIVE=?:");
    (*ppucEnvironmentEntry)[10]=pHP->ucBootDrive;
    ulEnvironmentSize+=strlen(*ppucEnvironmentEntry)+2;
    *(++ppucEnvironmentEntry)=NULL;
                                        /* Now add the expanded %CDDRIVE% macro */
    *ppucEnvironmentEntry=malloc(strlen("CDDRIVE=?:")+1);
    strcpy(*ppucEnvironmentEntry, "CDDRIVE=?:");
    (*ppucEnvironmentEntry)[8]=pHP->ucCDDrive1;
    ulEnvironmentSize+=strlen(*ppucEnvironmentEntry)+2;
    *(++ppucEnvironmentEntry)=NULL;
                                        /* Now add user environment entries which can't be found
                                           in source's environment at the bottom of the temporary table
                                           or replace matching entries */
for(pucUserEnvironmentEntry=PgmEnvironment;
    (pucUserEnvironmentEntry && *pucUserEnvironmentEntry);
    pucUserEnvironmentEntry=strstr(pucUserEnvironmentEntry, "\r\n"),
        (pucUserEnvironmentEntry ? pucUserEnvironmentEntry+=2 : pucUserEnvironmentEntry))
    {
    ULONG   ulUserEnvironmentEntrySize=0;
    ULONG   ulUserEnvironmentVariableSize=0;
                                        /* Pointer to next and previous to next % character found */
    UCHAR   *pucPercentBegin=0;
    UCHAR   *pucPercentEnd=0;
                                        /* Buffer to replace user specified %EnvironmentVariable% string
                                           in EnvironmentVariable=[Value][%EnvironmentVariable%][Value]
                                           with the Value in source's environment */
    UCHAR   *pucCompositeEnvironmentEntry=0;
                                        /* Here we extract the %EnvironmentVariable% (without % signs, but
                                           adding an = character) ) to locate this EnvironmentVariable in
                                           source's environment for replacing it */
    UCHAR   *pucSubstitutionEnvironmentVariable=0;
                                        /* Used to search source's environment in the temporary table
                                           for pucSubstitutionEnvrionmentVariable */
    UCHAR   **ppucEnvironmentEntrySearch=0;

                                        /* Get the length of current EnvironmentVariable=Value
                                           entry in user environment */
    pucMatch=strstr(pucUserEnvironmentEntry, "\r\n");
    if(pucMatch) ulUserEnvironmentEntrySize=pucMatch-pucUserEnvironmentEntry;
                                        /* Allocate and copy the current environment entry (add space for the
                                           terminating \0, but leave out the \r\n */
    pucCompositeEnvironmentEntry=(UCHAR *)malloc(ulUserEnvironmentEntrySize+1);
    memset(pucCompositeEnvironmentEntry, '\0', ulUserEnvironmentEntrySize+1);
    memcpy(pucCompositeEnvironmentEntry, pucUserEnvironmentEntry, ulUserEnvironmentEntrySize);
                                        /* Get the length of current EnvironmentVariable in user
                                           environment. A environment entry must have an = character,
                                           otherwise ignore the "unexpected text" (e.g. a environment
                                           entry "Hello World\r\n" does not make sense) */
    pucMatch=strstr(pucCompositeEnvironmentEntry, "=");
    if(!pucMatch)
        continue;
    ulUserEnvironmentVariableSize=pucMatch-pucCompositeEnvironmentEntry;
    pucUserEnvironmentEntryValue=pucMatch+1;
                                        /* Now try to locate the EnvironmentVariable just found
                                           in source's environment. If it is found replace it, otherwise
                                           add it at the bottom of the temporary table */
    for(ppucEnvironmentEntry=ppucEnvironment; *ppucEnvironmentEntry; ppucEnvironmentEntry++)
        {
                                        /* Is the current user EnvironmentVariable the current
                                           EnvironmentVariable of source's environment iterated.
                                           We compare the strings EnvironmentVariable= including
                                           "=", because otherwise we could match substrings too.
                                           We compare the variable's name caseinsensitive again. */
        if(strnicmp(*ppucEnvironmentEntry, pucUserEnvironmentEntry, ulUserEnvironmentVariableSize+1)==0)
                                        /* We found the EnvrionmentVariable in source's environment, which
                                           the user wanted to replace */
            break;
        }
                                        /* Now we either point to an EnvironmentVariable in source's
                                           environment which we replace, or to a NULL pointer in case
                                           the user specified EnvironmentVariable could not be located
                                           in source's environment (which means we have to add it) */
                                        /* Find the next %EnvironmentVariable% string in the current copied user
                                           environment entry. The % characters are pointed to by pucPercentBegin
                                           and pucPercentEnd. If the % characters for the current user environment
                                           entry don't occur in matching pair, the one found will be ignored. If
                                           a %% is found, the user wants a singe % sign in the environment, and
                                           we ignore %% to be taken as part of an EnvironmentVariable name  */
    for(pucPercentBegin=strchr(pucCompositeEnvironmentEntry, '%'),
            (pucPercentBegin ? pucPercentEnd=strchr(pucPercentBegin+1, '%') : pucPercentEnd);
        pucPercentBegin && pucPercentBegin<(pucCompositeEnvironmentEntry+ulUserEnvironmentEntrySize) &&
            pucPercentEnd && pucPercentEnd<(pucCompositeEnvironmentEntry+ulUserEnvironmentEntrySize);
        /* NOP */)
        {
                                        /* Length of %EnvironmentVariable% without % characters */
        ULONG   ulSubstitutionEnvironmentVariableSize=(pucPercentEnd-pucPercentBegin)-1;

                                        /* If a token %% is found, the user wants a % sign in the environment, so
                                           advance to next %EnvironmentVariable%' s % characters */
        if(*(pucPercentBegin+1)=='%')
            {
            pucPercentBegin=strchr(pucPercentEnd+1, '%');
            (pucPercentBegin ? pucPercentEnd=strchr(pucPercentBegin+1, '%') : pucPercentEnd);
            continue;
            }
                                        /* Temporarily copy the %EnvironmentVariable% (without % characters)
                                           append a = character and search for this Value in the array of
                                           all previous found environment entries */
        pucSubstitutionEnvironmentVariable=(UCHAR *)malloc(ulSubstitutionEnvironmentVariableSize+2);
        memset(pucSubstitutionEnvironmentVariable, '\0', ulSubstitutionEnvironmentVariableSize+2);
        memcpy(pucSubstitutionEnvironmentVariable, pucPercentBegin+1, ulSubstitutionEnvironmentVariableSize);
        strcat(pucSubstitutionEnvironmentVariable, "=");
                                        /* Now find this environment variable in the array */
        for(ppucEnvironmentEntrySearch=ppucEnvironment; *ppucEnvironmentEntrySearch; ppucEnvironmentEntrySearch++)
            {
                                        /* Search for EnvironmentVariable= case insensitive and replace
                                           %EnvironmentVariable% with the found environment entry's value */
            if(strnicmp(*ppucEnvironmentEntrySearch, pucSubstitutionEnvironmentVariable,
                ulSubstitutionEnvironmentVariableSize+1)==0)
                {
                UCHAR   *pucSubstitutionEnvironmentEntry=0;
                ULONG   ulSubstitutionEnvironmentEntrySize=0;

                                        /* Point to the Value */
                pucMatch=strchr(*ppucEnvironmentEntrySearch, '=')+1;
                                        /* Reserve space for current user environment entry, subtract the
                                           size of the to be replaced %EnvironmentVariable% and add space for
                                           replacement Value and the terminating \0 */
                ulSubstitutionEnvironmentEntrySize=ulUserEnvironmentEntrySize-ulSubstitutionEnvironmentVariableSize-2+
                    strlen(pucMatch)+1;
                pucSubstitutionEnvironmentEntry=(UCHAR *)malloc(ulSubstitutionEnvironmentEntrySize);
                memset(pucSubstitutionEnvironmentEntry, '\0', ulSubstitutionEnvironmentEntrySize);
                memcpy(pucSubstitutionEnvironmentEntry, pucCompositeEnvironmentEntry,
                    (pucPercentBegin-pucCompositeEnvironmentEntry));
                strcat(pucSubstitutionEnvironmentEntry, pucMatch);
                memcpy(strchr(pucSubstitutionEnvironmentEntry, '\0'), pucPercentEnd+1,
                    (ulUserEnvironmentEntrySize-(pucPercentEnd+1-pucCompositeEnvironmentEntry)));
                                        /* Now replace current user environment entry with the one just
                                           added the replaced %EnvironmentVariable% */
                free(pucCompositeEnvironmentEntry);
                pucCompositeEnvironmentEntry=pucSubstitutionEnvironmentEntry;
                pucSubstitutionEnvironmentEntry=0;
                                        /* Reset pointers to % characters */
                pucPercentBegin=pucPercentEnd=pucCompositeEnvironmentEntry;
                                        /* Correct size (without terminating \0 as we didn't count the \r\n
                                           previously too) */
                ulUserEnvironmentEntrySize=ulSubstitutionEnvironmentEntrySize-1;
                                        /* We found it, so skip not required comparisms */
                break;
                }
            }
                                        /* If %EnvironmentVariable% was not found in array of all previous
                                           environment entries, remove it in pucCompositeEnvironmentEntry
                                           as it can't be replaced */
        if(!*ppucEnvironmentEntrySearch)
            {
            strcpy(pucPercentBegin, pucPercentEnd+1);
            ulUserEnvironmentEntrySize=strlen(pucCompositeEnvironmentEntry);
            pucPercentBegin=pucPercentEnd=pucCompositeEnvironmentEntry;
            }
                                        /* We no longer need EnvironmentVariable= string we used to search
                                           for in array of environment entries */
        free(pucSubstitutionEnvironmentVariable);
                                        /* Advance to next %EnvironmentVariable%' s % characters */
        pucPercentBegin=strchr(pucPercentEnd+1, '%');
        (pucPercentBegin ? pucPercentEnd=strchr(pucPercentBegin+1, '%') : pucPercentEnd);
        }
                                        /* Now add the new EnvironmentVariable (including 2 characters for the
                                           terminating \r\n) to the space required flat string space */
    ulEnvironmentSize+=(strlen(pucCompositeEnvironmentEntry)+2);
                                        /* Now we have in ucCompositeEnvironmentEntry the user
                                           EnvironmentVariable ready to insert into the temporary table
                                           or to append it at the last position */
    if(*ppucEnvironmentEntry)
        {
                                        /* As we replace an existing EnvironmentVariable and we've allready
                                           added the space for the replacement, subtract the space no
                                           longer required (including the \r\n) */
        ulEnvironmentSize-=(strlen(*ppucEnvironmentEntry)-2);
                                        /* We have to replace the EnvironmentVariable from source's
                                           environment by the user's modification in the temporary table */
        free(*ppucEnvironmentEntry);
        *ppucEnvironmentEntry=pucCompositeEnvironmentEntry;
        }
    else
        {                               /* We have define a unique entry, which we add at the bottom
                                           of the temporary table */
        *ppucEnvironmentEntry=pucCompositeEnvironmentEntry;
                                        /* Advance end of table indicator */
        *(++ppucEnvironmentEntry)=NULL;
        }
                                        /* See if the user has added a %% token, which means he wants a %
                                           sign in the environment and that we have to remove single % sign
                                           of this %% token now. We do this for %% pairs, so %%%% gets
                                           reduced to %% */
    pucMatch=pucCompositeEnvironmentEntry;
    if(pucMatch)
        while((pucMatch=strstr(pucMatch, "%%"))!=NULL)
            {
            memcpy(pucMatch, (pucMatch+1), strlen(pucMatch+1)+1);
            pucMatch++;
            }
    }
                                        /* Allocate environment to inherit to session being
                                           started and reserve for terminating \0 */
pucEnvironment=pucEnvironmentEntry=(UCHAR *)malloc(ulEnvironmentSize+1);
                                        /* Now copy new Environment from table into a flat
                                           string terminating an entry with \r\n and all entries
                                           with \0 and free the table entries */
for(ppucEnvironmentEntry=ppucEnvironment; *ppucEnvironmentEntry; ppucEnvironmentEntry++)
    {
    strcpy(pucEnvironmentEntry, *ppucEnvironmentEntry);
    pucEnvironmentEntry+=strlen(*ppucEnvironmentEntry);
    *pucEnvironmentEntry++='\r';
    *pucEnvironmentEntry++='\n';
                                        /* Free table entry */
    free(*ppucEnvironmentEntry);
    }
                                        /* Add \0 because this terminates the whole Environment */
*pucEnvironmentEntry='\0';
                                        /* Free table of entries */
free(ppucEnvironment);
#ifdef  DEBUG_UTILITY
printf("Exiting CreateEnvironment\n");
#endif  /* DEBUG_UTILITY */
                                        /* Return the Environment which comsists of source's with
                                           additions/replacement from the user */
return(pucEnvironment);
}

/*--------------------------------------------------------------------------------------*\
 * Allocate a storage area that contains the to be started session's environment in the *
 * \0-terminated form required by DosStartSession. This function converts the \r\n      *
 * format of the environment into the \0 format, frees the \r\n format and returns the  *
 * \0 format.                                                                           *
 * Req:                                                                                 *
 *      PgmEnvironment  A pointer to the session's Environment (created by              *
 *                      CreateEnvironment() in the form of:                             *
 *                      EnvVar1=Value1\r\nEnvVar2=Value2\r\n...\0                       *
 * Returns:                                                                             *
 *      pucEnvironment  A pointer to \0-terminated format of the session's environment, *
 *                      in the form of:                                                 *
 *                      EnvVar1=Value1\0EnvVar2=Value2\0...EnvVarn=Valuen\0\0           *
 *                      Or NULL if the required space could not be allocated.           *
\*--------------------------------------------------------------------------------------*/
UCHAR   *ConvertEnvironment(PSZ PgmEnvironment)
{
ULONG   ulPgmEnvironmentSize;           /* Size of PgmEnvironment to allocate space to
                                           convert to \0-terminated format */
UCHAR   *pucEnvironment;                /* The \0-terminated format of PgmEnvironment */
UCHAR   *pucSourceBegin;                /* Pointer within PgmEnvironment for a single EnvironmentVariable,
                                           terminated by \r\n */
UCHAR   *pucSourceEnd;                  /* Termination of pucSourceBegin (either \r\n or \0) */
UCHAR   *pucTargetBegin;                /* Pointer within pucEnvironment for a single EnvironmentVariable,
                                           terminated by \0 */

#ifdef  DEBUG_UTILITY
printf("Entering ConvertEnvironment\n");
#endif  /* DEBUG_UTILITY */
                                        /* Now test for EnvironmentVariables named BEGINLIBPATH and
                                           ENDLIBPATH. It these are found, than they are not added
                                           to the environment, but DosSetExtLIBPATH is called
                                           to write them into PC/2's process information block
                                           to inherit them. Both are not environment variables, CMD.EXE
                                           and PC/2 just treat them this way because the user
                                           should set them consistent with environment variables for
                                           simplicity. We treat the environment strings case sensitive
                                           but compare the variable name caseinsensitive. */
for(pucSourceBegin=PgmEnvironment, pucSourceEnd=strstr(pucSourceBegin, "\r\n");
    *pucSourceBegin!='\0';
    pucSourceBegin=(pucSourceEnd ? pucSourceEnd+=2 : (UCHAR *)strchr(pucSourceBegin, '\0')),
        pucSourceEnd=(*pucSourceBegin ? (UCHAR *)strstr(pucSourceBegin, "\r\n") : NULL))
    {
    if((!strnicmp(pucSourceBegin, "BEGINLIBPATH=", BEGINLIBPATHLEN)) ||
        (!strnicmp(pucSourceBegin, "ENDLIBPATH=", ENDLIBPATHLEN)))
        {
        ULONG   ulSourceLength=(pucSourceEnd-pucSourceBegin);
        UCHAR   *pucDynamicLibPath=(UCHAR *)calloc(ulSourceLength+1, sizeof('\0'));

                                        /* Copy dynamic LIBPATH settings string excluding its \r\n
                                           termination */
        pucTargetBegin=pucDynamicLibPath;
        memcpy(pucTargetBegin, pucSourceBegin, ulSourceLength);
        pucTargetBegin+=ulSourceLength;
        *pucTargetBegin='\0';
                                        /* Now set the dynamic LIBPATH with the value
                                           following BEGINLIBPATH= or ENDLIBPATH= */
        if(!strnicmp(pucDynamicLibPath, "BEGINLIBPATH=", BEGINLIBPATHLEN))
            if(pHP->DosSetExtLIBPATH)
                pHP->DosSetExtLIBPATH(&pucDynamicLibPath[BEGINLIBPATHLEN], BEGINLIBPATH);
        if(!strnicmp(pucDynamicLibPath, "ENDLIBPATH=", ENDLIBPATHLEN))
            if(pHP->DosSetExtLIBPATH)
                pHP->DosSetExtLIBPATH(&pucDynamicLibPath[ENDLIBPATHLEN], ENDLIBPATH);
                                        /* Cleanup */
        free(pucDynamicLibPath);
        pucDynamicLibPath=0;
                                        /* Now remove the LIBPATH settings from the environment, as
                                           they are not part of the environment at all, just treated
                                           syntactically alike */
        strcpy(pucSourceBegin, pucSourceEnd+2);
        pucSourceEnd=(*pucSourceBegin ? strstr(pucSourceBegin, "\r\n") : NULL);
        }
    }
                                        /* Allocate environment to inherit to session being
                                           started and reserve for terminating \0\0 (we
                                           expect a \r\n termination, but we're on the save
                                           side this way) */
ulPgmEnvironmentSize=strlen(PgmEnvironment)+2;
pucEnvironment=(UCHAR *)malloc(ulPgmEnvironmentSize);
if(pucEnvironment==NULL)
    return(NULL);
                                        /* Now copy the Environment from the \r\n-terminated
                                           format to the \0 terminated format */
for(pucSourceBegin=PgmEnvironment, pucSourceEnd=strstr(pucSourceBegin, "\r\n"), pucTargetBegin=pucEnvironment;
    *pucSourceBegin!='\0';
    pucSourceBegin=(pucSourceEnd ? pucSourceEnd+=2 : (UCHAR *)strchr(pucSourceBegin, '\0')),
        pucSourceEnd=(*pucSourceBegin ? (UCHAR *)strstr(pucSourceBegin, "\r\n") : NULL))
    {
    ULONG   ulSourceLength=(pucSourceEnd-pucSourceBegin);

    memcpy(pucTargetBegin, pucSourceBegin, ulSourceLength);
    pucTargetBegin+=ulSourceLength;
    *pucTargetBegin++='\0';
    }
                                        /* Add \0\0 because this terminates the whole Environment */
*pucTargetBegin++='\0';
*pucTargetBegin++='\0';
                                        /* Free \r\n-terminated format of session's Environment */
free(PgmEnvironment);
#ifdef  DEBUG_UTILITY
printf("Exiting ConvertEnvironment\n");
#endif  /* DEBUG_UTILITY */
                                        /* Return the \0-terminated Environment, ready to be used
                                           by DosStartSession() */
return(pucEnvironment);
}

/*--------------------------------------------------------------------------------------*\
 * Create an application's complete environment, by merging PC/2's environment, the     *
 * application's environment settings and the Environment Spaces selected for the       *
 * application. The returned pointer must be freed after using.                         *
 * Req:                                                                                 *
 *      pMenuData       A pointer to menuentry's MENUDATA structure we want to create   *
 *                      a session's environment from. The environment settings and the  *
 *                      Environment Spaces selected for this session are assumed to be  *
 *                      terminated by \r\n for all individual environment entries.      *
 * Returns:                                                                             *
 *      pucEnvironment  A pointer to \0-terminated format of the session's environment, *
 *                      in the form of:                                                 *
 *                      EnvVar1=Value1\0EnvVar2=Value2\0...EnvVarn=Valuen\0\0           *
 *                      Or NULL if the required space could not be allocated.           *
\*--------------------------------------------------------------------------------------*/
UCHAR   *SessionEnvironment(MENUDATA *pMenuData)
{
                                        /* Before adding the environment settings and the Environment Spaces,
                                           we append all settings into a single string in order to call
                                           CreateEnvironment() only once, because this function causes a
                                           time consuming reformatting to merge these environments */
UCHAR           *pucCompositeUserEnvironment=NULL;
                                        /* Allocation required for pucCompositeUserEnvironmentEntry */
ULONG           ulCompositeUserEnvironmentLength=0;
                                        /* List of Environment Spaces selected for this session */
ENVIRONMENTLIST *pEnvironmentListCurrent=NULL;
ULONG           ulAdvance=TRUE;         /* Set to TRUE when advancing to next of all available Environment
                                           Spaces */
                                        /* PC/2 supports 2 environment settings for a session, one that
                                           is added to PC/2's environment before adding any Environment Space
                                           and one that is added afterwards. TRUE means the first, FALSE the
                                           second one */
ULONG           ulAheadEnvironment=TRUE;
                                        /* Set to TRUE after all Environment Spaces have been added */
ULONG           ulDoneEnvironmentSpaces=FALSE;
                                        /* Environment containing the merge of PC/2's environment, the sessions
                                           leading and trailing environment and the selected Environment Spaces */
UCHAR           *pucEnvironment=NULL;

#ifdef  DEBUG_UTILITY
printf("Entering SessionEnvironment\n");
#endif  /* DEBUG_UTILITY */
for(pEnvironmentListCurrent=NULL;
    /* NOP */;
    (ulAdvance==TRUE ? pEnvironmentListCurrent=pEnvironmentListCurrent->pNext : pEnvironmentListCurrent))
    {
    UCHAR       *pucSourceEnvironment=pucCompositeUserEnvironment;
    UCHAR       *pucAppendEnvironment=NULL;

    ulAdvance=TRUE;
    if(pEnvironmentListCurrent==NULL)
        {
                                        /* If we're not adding an Environment Space, we add the session's
                                           environment settings */
        if(ulAheadEnvironment==TRUE)
            {
            pucAppendEnvironment=pMenuData->PgmLeadingEnvironment;
            ulAheadEnvironment=FALSE;
            }
        else
            {
            ulDoneEnvironmentSpaces=TRUE;
            pucAppendEnvironment=pMenuData->PgmTrailingEnvironment;
            }
        }
    else
        {
                                        /* Add the current Environment Space to the session's
                                           environment, if the session marked to require this
                                           setting in the Program Installation dialog */
        if(!strstr(pMenuData->EnvironmentSpaces, pEnvironmentListCurrent->pEnvironmentName))
            continue;
                                        /* We're adding an Environment Space */
        pucAppendEnvironment=pEnvironmentListCurrent->pEnvironment;
        }
                                        /* Add the next environment to create the session's composite
                                           user environment */
    if(pucCompositeUserEnvironment==NULL)
        pucCompositeUserEnvironment=strdup(pucAppendEnvironment);
    else
        {
        ulCompositeUserEnvironmentLength=strlen(pucSourceEnvironment)+
            strlen(pucAppendEnvironment)+1;
        pucCompositeUserEnvironment=(UCHAR *)malloc(ulCompositeUserEnvironmentLength);
        memset(pucCompositeUserEnvironment, '\0', ulCompositeUserEnvironmentLength);
        strcpy(pucCompositeUserEnvironment, pucSourceEnvironment);
        strcat(pucCompositeUserEnvironment, pucAppendEnvironment);
        free(pucSourceEnvironment);
        }
                                        /* The pointer to the Environment Spaces is 0 two times, first
                                           for the first loop where we add the leading environment settings
                                           and second after having added the last Environment Space when
                                           we have to add the trailing environment settings */
    if(pEnvironmentListCurrent==NULL)
        {
        if(ulDoneEnvironmentSpaces==TRUE)
            break;
        else
            {
            ulAdvance=FALSE;
                                        /* The Environment Spaces must be loaded before the Popup-Menu itself,
                                           otherwise we've got a problem here, that the contents of the menuentry's
                                           Environment Spaces can't be loaded here */
            pEnvironmentListCurrent=pHP->pEnvironmentSpaces;
            if(!pEnvironmentListCurrent)
                break;
            }
        }
    }
                                        /* If the additions/modifications of the session's
                                           environment should be merged with PC/2's environment
                                           after merging any Environment Space merge it now */
pucEnvironment=CreateEnvironment(pHP->pucPC2Environment, pucCompositeUserEnvironment);
free(pucCompositeUserEnvironment);
#ifdef  DEBUG_UTILITY
printf("Exiting SessionEnvironment\n");
#endif  /* DEBUG_UTILITY */
return(ConvertEnvironment(pucEnvironment));
}

/*--------------------------------------------------------------------------------------*\
 * A SESSIONDATA data structure is used to extract the parameters to start a new        *
 * session. If sucessfull, additional parameters are extracted to set the priority of   *
 * the new session.                                                                     *
 * Req:         A SessionData structure to start session from                           *
 *    :         A MenuData structure to start session from                              *
\*--------------------------------------------------------------------------------------*/
void    StartSession(SESSIONDATA *pSessionData, MENUDATA *pMenuData)
{
UCHAR           ucMessageBuffer[2048];
UCHAR           ucPgmTitle[MAXNAMEL+1];
UCHAR           *pucDestination, *pucSource;
                                        /* The name of the executable to start */
UCHAR           ucPgmName[CCHMAXPATH];
                                        /* The parameters passed to the executable */
UCHAR           ucPgmInputs[EF_SIZE255+1];
                                        /* If DosStartSession() fails, display current directory */
ULONG           ulCurrentDrive;         /* Current drive (1=A, 2=B, ...) */
ULONG           ulLogicalDriveMap;      /* Bitmap of available drives (Bit 0=A, 1=B, ...) */
                                        /* The current directory, which is our working directory */
UCHAR           ucCurrentDirectory[CCHMAXPATH];
ULONG           ulCurrentDirectoryLen=sizeof(ucCurrentDirectory);
STARTDATA       StartData;              /* Control structure for DosStartSession() */
PROGDETAILS     ProgDetails;            /* Control structure for WinStartApp() */
ULONG           SessID;
PID             Pid;
APIRET          rc;

/*                                                                                      *\
 * Change to the root directory of all non-removable drives when a drive was added or   *
 * removed since last change to all drives root directory.                              *
\*                                                                                      */
#ifdef  DEBUG_UTILITY
printf("Entering StartSession\n");
#endif  /* DEBUG_UTILITY */

                                        /* I'm not sure who turns that on always, but it seems to
                                           happen */
DosError(FERR_DISABLEHARDERR);
memset(&StartData, 0, sizeof(StartData));
StartData.Length=sizeof(StartData);     /* Length of StartData */
                                        /* Buffer containing the module contributed to
                                           an error that the session could not be started */
StartData.ObjectBuffer=malloc(CCHMAXPATH);
memset(StartData.ObjectBuffer, 0, CCHMAXPATH);
StartData.ObjectBuffLen=CCHMAXPATH;
                                        /* Independent session */
StartData.Related=SSF_RELATED_INDEPENDENT;
StartData.FgBg=pSessionData->FgBg;      /* Foreground application */
                                        /* No trace */
StartData.TraceOpt=SSF_TRACEOPT_NONE;
                                        /* Session title string */
for(pucSource=pSessionData->PgmTitle, pucDestination=ucPgmTitle; *pucSource; pucSource++)
                                        /* Remove ~ (tilde) */
    if(*pucSource!='~') *pucDestination++=*pucSource;
*pucDestination='\0';                   /* Add 0-termination */
StartData.PgmTitle=ucPgmTitle;
                                        /* Input arguments, assume none as we add them later on,
                                           if there are some */
StartData.PgmInputs=0;
StartData.TermQ=0;                      /* No termination queue */
                                        /* Has the user overwritten the environment? */
if((*pSessionData->PgmLeadingEnvironment=='\0') &&
    (*pSessionData->PgmTrailingEnvironment=='\0') &&
    (*pSessionData->EnvironmentSpaces=='\0') &&
    !(pSessionData->StatusFlag & STARTUPENVIRONMENTS))
    StartData.Environment=0;            /* Inherit environment from PC/2 */
else
    {
                                        /* Save a pointer to the originally selected Environment
                                           Spaces, which temporarily gets replaced by the ones selected
                                           from the Startup Environment Spaces dialog (when requested
                                           so by the user) */
    PSZ                     EnvironmentSpacesOriginal=pMenuData->EnvironmentSpaces;
    PSZ                     EnvironmentSpacesStartup=strdup(EnvironmentSpacesOriginal);
    COMMANDLINEENVIRONMENT  CLECurrent;

    if(pSessionData->StatusFlag & STARTUPENVIRONMENTS)
        {
        ULONG   ulResult;

        strcpy(CLECurrent.ucPgmTitle, StartData.PgmTitle);
        CLECurrent.pucEnvironmentSpaces=EnvironmentSpacesStartup;
        ulResult=WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, SE_DialogProcedure,
            pHP->hDLLPc2Resource, SEID_STARTUPDIALOG, &CLECurrent);
        if(ulResult==DID_ERROR)
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Creation of a dialog box failed - continuing...");
        EnvironmentSpacesStartup=CLECurrent.pucEnvironmentSpaces;
        }
                                        /* Set the temporary modifications */
    pMenuData->EnvironmentSpaces=EnvironmentSpacesStartup;
                                        /* Create a modified environment to inherit to the
                                           session we're going to start */
    StartData.Environment=SessionEnvironment(pMenuData);
                                        /* Restore original settings and free copy */
    pMenuData->EnvironmentSpaces=EnvironmentSpacesOriginal;
    free(EnvironmentSpacesStartup);
    }
                                        /* Inherit from PC/2's environment to change to
                                           requested drive & directory */
StartData.InheritOpt=SSF_INHERTOPT_PARENT;
/*                                                                                      *\
 * Find the application the user wants to start.                                        *
\*                                                                                      */
                                        /* Program path-name string. See if the %BOOTDRIVE% macro is 
                                           used, so that we have to change to the drive we booted from */
memset(ucPgmName, '\0', sizeof(ucPgmName));
ExpandMacros(ucPgmName, pSessionData->PgmName, 
    (StartData.Environment==0 ? pHP->pPib->pib_pchenv : StartData.Environment));
StartData.PgmName=ucPgmName;
                                        /* Query drive bit map */
DosQueryCurrentDisk(&ulCurrentDrive, &ulLogicalDriveMap);
                                        /* If drive map changed query drives and change to root
                                           directories */
if(pHP->ulLogicalDriveMap!=ulLogicalDriveMap)
    SetDriveRoot();
/*                                                                                      *\
 * Test for the working directory the user wants the session to be started from. We     *
 * then change into this directory before launching the session. The working directory  *
 * may be of the forms:                                                                 *
 *     x:\OS2\...           Where x: is a drive letter                                  *
 *     %BOOTDRIVE%\OS2\...  Where %BOOTDRIVE% is a macro that has to be replaced by the *
 *                          drive we booted from                                        *
 *     %PC2%\Apps\........  Where %PC2% is a macro that has to be replaced by the path  *
 *                          PC/2 was launched from                                      *
 *     %CDDRIVE%\Apps\....  Where %CDDRIVE% is a macro that has to be replaced by the   *
 *                          drive of the first CD-ROM drive                             *
 * Analyze the directory, if none exists, we are already in the root of the drive which *
 * we keep.                                                                             *
 *                                                                                      *
 * If an OS/2 or DOS window gets started and no working directory is specified, assume  *
 * that %BOOTDRIVE%\ is specified to avoid the root of the last local? drive to be it,  *
 * which it would as we regularily change to the root of all drives in ascending order  *
 * and then stay at the last local? drive.                                              *
\*                                                                                      */
memset(ucCurrentDirectory, '\0', sizeof(ucCurrentDirectory));
if(!strcmp(pSessionData->PgmDirectory, ""))
    {
    ULONG   ulSessionType=pSessionData->SessionType;

    strcpy(pSessionData->PgmDirectory, BOOTDRIVE);
    strcat(pSessionData->PgmDirectory, "\\");
    }
if(strlen(pSessionData->PgmDirectory)>=2)
    {
    UCHAR       ucDrive;
    ULONG       ulCharIndex;

                                        /* See if the %BOOTDRIVE%, %PC2% or %CDDRIVE% macro is used, so 
                                           that we have to change to the drive we booted or the path
                                           we launched from */
    ucDrive=pHP->ucBootDrive;
    ExpandMacros(ucCurrentDirectory, pSessionData->PgmDirectory,
        (StartData.Environment==0 ? pHP->pPib->pib_pchenv : StartData.Environment));
                                        /* Then get drive letter if there is one */
    if(ucCurrentDirectory[1]==':')
        ucDrive=ucCurrentDirectory[0];
                                        /* Ensure that backslash terminates directory only
                                           for the root directory (e.g. C:\) but not for a
                                           subdirectory (e.g. C:\OS2\) */
    ulCharIndex=strlen(ucCurrentDirectory);
    if((ulCharIndex>3) &&
        (ucCurrentDirectory[ulCharIndex-1]=='\\'))
        {
        ucCurrentDirectory[ulCharIndex-1]='\0';
        }
                                        /* 1=A, 2=B, 3=C,.... Ensure that ucDrive gets
                                           converted to lowercase for calculation */
    rc=DosSetDefaultDisk((++ucDrive|0x20)-'a');
    rc|=DosSetCurrentDir(ucCurrentDirectory);
    if(rc!=NO_ERROR)
        {
        sprintf(ucMessageBuffer, "Can't change into the directory \"%s\" to invoke \"%s\" - continuing...",
            pSessionData->PgmDirectory, pSessionData->PgmTitle);
        DOS_ERR(rc, pHP->hwndFrame, HELP_SETPATH, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            ucMessageBuffer);
        }
    }
                                        /* Session type */
StartData.SessionType=pSessionData->SessionType;
StartData.IconFile=0;                   /* No icon, use default */
StartData.PgmHandle=0;                  /* Don't use installation file */
                                        /* Session initial state */
StartData.PgmControl=(pSessionData->PgmControl & ~(SSF_CONTROL_AUTOSTARTED|SSF_CONTROL_AUTOSTART|SSF_CONTROL_PASSWORD));
                                        /* Initial window size */
if(StartData.PgmControl & SSF_CONTROL_SETPOS)
    {                                   /* Position relative to (0|0) of virtual Desktop */
    StartData.InitXPos=0-pHP->VirtualDesktopPos.x+pSessionData->InitXPos;
    StartData.InitYPos=0-pHP->VirtualDesktopPos.y+pSessionData->InitYPos;
    StartData.InitXSize=pSessionData->InitXSize;
    StartData.InitYSize=pSessionData->InitYSize;
    }
/*                                                                                      *\
 * Search for user-addable commandline parameter. If one found, display dialog and get  *
 * it. It will be added to the current arguments. Also ensure that the %BOOTDRIVE%      *
 * macro gets replaced by the actual boot drive.                                        *
\*                                                                                      */
while(TRUE)
    {
    COMMANDLINEPARAMS   CLPParams;
    ULONG               ulResult;       /* Value returned by WinDismissDlg() */
    INT                 iTemp;
    UCHAR               *pucMatch;

                                        /* Replace %BOOTDRIVE%, %PC2% or %CDDRIVE% macros by actual drive 
                                           we booted from, the path we launched from or the path of the
                                           first CD-ROM drive */
    memset(ucPgmInputs, '\0', sizeof(ucPgmInputs));
    ExpandMacros(ucPgmInputs, pSessionData->PgmInputs,
        (StartData.Environment==0 ? pHP->pPib->pib_pchenv : StartData.Environment));
                                        /* As we currently can't be sure that there are
                                           user-addable commandline parameters, assume we
                                           have the complete commandline parameters */
    StartData.PgmInputs=ucPgmInputs;
                                        /* Copy application to add CLPs for */
    strcpy(CLPParams.ucPgmTitle, StartData.PgmTitle);
                                        /* Search for [, break if not found */
    if((pucMatch=strchr(ucPgmInputs, '['))==NULL) break;
                                        /* Search for ], break if not found */
    if(strchr(pucMatch, ']')==NULL) break;
                                        /* Break commandline parameters into three parts */
    for(iTemp=0, pucMatch=StartData.PgmInputs; *pucMatch!='['; iTemp++, pucMatch++)
        CLPParams.ucPBefore[iTemp]=*pucMatch;
    CLPParams.ucPBefore[iTemp]='\0';
    pucMatch++;                         /* Skip [ */
    for(iTemp=0; *pucMatch!=']'; iTemp++, pucMatch++)
        CLPParams.ucPUser[iTemp]=*pucMatch;
    CLPParams.ucPUser[iTemp]='\0';
    pucMatch++;                         /* Skip ] */
    for(iTemp=0; *pucMatch!='\0'; iTemp++, pucMatch++)
        CLPParams.ucPAfter[iTemp]=*pucMatch;
    CLPParams.ucPAfter[iTemp]='\0';
                                        /* Start Startup Parameters dialog box */
    CLPParams.pucCurrentDirectory=ucCurrentDirectory;
    ulResult=WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, SU_DialogProcedure,
        pHP->hDLLPc2Resource, SUID_STARTUPDIALOG, &CLPParams);
    if(ulResult==DID_ERROR)
        {
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Creation of a dialog box failed - continuing...");
        break;
        }
                                        /* If the user selected cancel, don't launch the session, as
                                           no user input can be processed also by DID_OK and cancel
                                           allows the user to correct misselections */
    if(ulResult==DID_CANCEL)
        goto CANCELLED_BY_USER;
                                        /* Replace existing commandline parameters with
                                           user-edited ones if OK was pressed */
    if(ulResult==DID_OK)
        {
        sprintf(ucPgmInputs, "%s%s %s",CLPParams.ucPBefore, CLPParams.ucPUser, CLPParams.ucPAfter);
        StartData.PgmInputs=ucPgmInputs;
        }
                                        /* If Cancel was pressed, leave empty string */
    break;                              /* Break out ! */
    }
/*                                                                                      *\
 * If we're to start a DOS session, then set the DOS-Settings via the Environment. This *
 * is an undocumented feature (the toolkit says that the Environment is reserved and    *
 * must be 0 for a DOS session. To use the DOS Settings each Setting must be followed   *
 * by \0 and the last Setting must be followed by two \0s. It seems that the settings   *
 * must replace OFF by 0 and ON by 1.                                                   *
 * Any WIN-OS2 session is hereto equivalent to a DOS session.                           *
\*                                                                                      */
switch(StartData.SessionType)
{
case PROG_VDM:
case PROG_WINDOWEDVDM:
case PROG_WINDOW_REAL:
case PROG_31_STD:
case PROG_31_ENH:
case PROG_31_STDSEAMLESSCOMMON:
case PROG_31_STDSEAMLESSVDM:
case PROG_31_ENHSEAMLESSCOMMON:
case PROG_31_ENHSEAMLESSVDM:
    {
    ULONG       ulLength=strlen(pSessionData->PgmDosSettings)+1;
    UCHAR       *pucDosSettings;

                                        /* Convert to format used by OS/2 in environment of session
                                           to start (same as used for WINDOWS32PROPERTYDIALOG) */
    pucDosSettings=ImportDosSettings(pSessionData->PgmDosSettings, &ulLength, FALSE);
    StartData.Environment=pucDosSettings;
    }
    break;
}
switch(StartData.SessionType)
{
case PROG_WINDOW_REAL:
case PROG_31_STD:
case PROG_31_ENH:
case PROG_31_STDSEAMLESSCOMMON:
case PROG_31_STDSEAMLESSVDM:
case PROG_31_ENHSEAMLESSCOMMON:
case PROG_31_ENHSEAMLESSVDM:
    {
    memset(&ProgDetails, 0, sizeof(ProgDetails));
                                        /* Start a WIN-OS2 or Windows session (we support real, standard,
                                           enhanced, common and separate seamless sessions here) */
    ProgDetails.Length=sizeof(PROGDETAILS);
    ProgDetails.progt.progc=StartData.SessionType;
    ProgDetails.progt.fbVisible=SHE_VISIBLE;
    ProgDetails.pszTitle=StartData.PgmTitle;
    ProgDetails.pszExecutable=StartData.PgmName;
    ProgDetails.pszParameters=StartData.PgmInputs;
    ProgDetails.pszStartupDir=pSessionData->PgmDirectory;
    ProgDetails.pszEnvironment=StartData.Environment;
    }
    break;
}
/*                                                                                      *\
 * Now set the requested priority, the started session will inherit these settings (at  *
 * least non-WPS-object sessions. Afterwards reset to default priority. For DOS         *
 * sessions changing priority will hang the system, so DOS sessions have the default    *
 * priority only.                                                                       *
\*                                                                                      */
if((pSessionData->PriorityClass!=PRTYC_NOCHANGE) || (pSessionData->PriorityDelta!=PRTYC_NOCHANGE))
    {
    rc=DosSetPriority(PRTYS_PROCESSTREE, (ULONG)pSessionData->PriorityClass, (LONG)pSessionData->PriorityDelta,
        pHP->ProcessId);
    if(rc!=NO_ERROR)
        {
        sprintf(ucMessageBuffer, "Can't change the priority while launching %s - continuing...",
            pSessionData->PgmTitle);
        DOS_ERR(rc, pHP->hwndFrame, HELP_PRIORITY, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            ucMessageBuffer);
        }
    }
/*                                                                                      *\
 * Now start the session, but beware of the error code ERROR_SMG_START_IN_BACKGROUND,   *
 * which isn't actually an error code, but an informational message we ignore.          *
\*                                                                                      */
switch(StartData.SessionType)
{
case PROG_WPSOBJECT:
    {
    HOBJECT     hWPSObject;

                                        /* WPS object can't be started when WPS is not running,
                                           or invalid ObjectID or folder name is passed, so we
                                           we assume there is a error */
    rc=ERROR_INVALID_HANDLE;
                                        /* Try to launch when the WPS was detected */
    if(pHP->Windows.lWindowList!=-1)
        {
                                        /* Find the handle of the WPS object */
        hWPSObject=WinQueryObject(StartData.PgmName);
                                        /* Launch the object by calling the WPS */
        if(hWPSObject!=NULLHANDLE)
            if(WinSetObjectData(hWPSObject, "OPEN=DEFAULT")==TRUE)
                {
                                        /* Put WPS into foreground in order that opened folders
                                           become the active window (as the WPS does for <WP_*>
                                           objects) */
                WinSwitchToProgram(WinQuerySwitchHandle(pHP->hwndWPSFrame, 0));
                rc=NO_ERROR;
                }
        }
    }
    break;

case PROG_WINDOW_REAL:
case PROG_31_STD:
case PROG_31_ENH:
case PROG_31_STDSEAMLESSCOMMON:
case PROG_31_STDSEAMLESSVDM:
case PROG_31_ENHSEAMLESSCOMMON:
case PROG_31_ENHSEAMLESSVDM:
    {
    HAPP    hApp;

    hApp=WinStartApp(NULLHANDLE, &ProgDetails, "", NULL, SAF_INSTALLEDCMDLINE);
    if(hApp==NULLHANDLE)
        {
        sprintf(ucMessageBuffer, "An error occured while launching \"%s\" - continuing...",
            pSessionData->PgmTitle);
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_LAUNCH, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            ucMessageBuffer);
        }
    rc=NO_ERROR;
    }
    break;

default:
    {
    UCHAR   ucExecutable[CCHMAXPATH+1];
    UCHAR   *pucFullPathExecutable=NULL;
    UCHAR   *pucExtension=NULL;

    if(strlen(pMenuData->PgmName))
        {
        UCHAR   *pucEnvironment;

                                        /* If no working directory is defined, try to find executable in
                                           PATH environment */
        strcpy(ucExecutable, ucPgmName);
                                        /* If the extenstion is not .EXE or .COM, try to add them */
        pucExtension=strrchr(ucExecutable, '.');
        if(pucExtension!=NULL)
            if((stricmp(pucExtension, ".EXE")) && (stricmp(pucExtension, ".COM")))
                pucExtension=0;
                                        /* The extension is not .EXE or .COM, try with adding .EXE for
                                           fully qualified executables */
        if(!pucExtension && ((ucExecutable[1]==':') || (ucExecutable[1]=='\\')))
            {
            strcat(ucExecutable, ".EXE");
            goto SKIP_ENVIRONMENT;
            }
                                        /* See which environment is available */
        if(StartData.Environment)
            pucEnvironment=StartData.Environment;
        else
            pucEnvironment=(UCHAR *)pHP->pPib->pib_pchenv;
                                        /* The extension is not .EXE or .COM, try adding .EXE and .COM
                                           for not fully qualified executables */
        if(!pucExtension)
            {
            strcat(ucExecutable, ".EXE");
            pucExtension=pucFullPathExecutable=LocateInEnvironment(pucEnvironment, "PATH", ucExecutable);
            }
        if(!pucExtension)
            {
            strcpy(strrchr(ucExecutable, '.'), ".COM");
            pucFullPathExecutable=LocateInEnvironment(pucEnvironment, "PATH", ucExecutable);
            }
                                        /* If we could locate the executable, save it for future use */
        if(pucFullPathExecutable)
            {
                                        /* Use the fully qualified path we may have found */
            StartData.PgmName=ucExecutable;
            strcpy(ucExecutable, pucFullPathExecutable);
            free(pMenuData->PgmFullPathExecutable);
            pMenuData->PgmFullPathExecutable=pucFullPathExecutable;
            pHP->ulRuntimeFlag|=ICONCACHEACTIVE;
            }
SKIP_ENVIRONMENT:
        ;
        }
#ifdef  DEBUG
    printf("Utility.c: Launching \"%s\"\n", StartData.PgmName);
#endif  /* DEBUG */
    DosQueryCurrentDisk(&ulCurrentDrive, &ulLogicalDriveMap);
    DosQueryCurrentDir(ulCurrentDrive, ucCurrentDirectory, &ulCurrentDirectoryLen);
                                        /* Now start the session, just be sure to handle
                                           detach sessions differently. As detached sessions
                                           can only be started by DosExecPgm() and that API
                                           can only start a session of the same type, we
                                           have to start a command processor which gets the
                                           required arguments passed to start our session
                                           detached */
    if(StartData.SessionType==PROG_DETACHED)
        {
        UCHAR   ucPgmNameDetached[CCHMAXPATH];
        UCHAR   ucPgmInputsDetached[(CCHMAXPATH<<1)];

                                        /* Actually it's quite tricky what we do, e.g. assume
                                           we want to start the executable "F:\os2\cmd.exe"
                                           with the parameter "/c beep.cmd" from directory
                                           "h:\programming\pc2\" detached.
                                           The working directory has already been set and gets
                                           inherited to the process's we start. What we start
                                           directly is a hidden OS/2 window session, but the
                                           supplied parameters are "/c \"detach ", the
                                           executable "F:\os2\cmd.exe", the parameter "/c beep.cmd"
                                           and the closing "\"". The net effect is that we start
                                           a temporary hidden windowed session which detaches a
                                           windowed session that runs beep.cmd.
                                           Of course we could have saved some logic when we could
                                           DosExecPgm() another session type (DosExecPgm() can only
                                           start the same session type) */
        strcpy(ucPgmInputsDetached, "/c \"detach ");
        strcat(ucPgmInputsDetached, StartData.PgmName);
        strcat(ucPgmInputsDetached, " ");
        strcat(ucPgmInputsDetached, StartData.PgmInputs);
        strcat(ucPgmInputsDetached, "\"");
        StartData.PgmInputs=ucPgmInputsDetached;
        strcpy(ucPgmNameDetached, "C:\\OS2\\CMD.EXE");
        ucPgmNameDetached[0]=pHP->ucBootDrive;
        StartData.PgmName=ucPgmNameDetached;
        StartData.SessionType=SSF_TYPE_WINDOWABLEVIO;
        StartData.PgmControl=SSF_CONTROL_INVISIBLE;
        rc=DosStartSession(&StartData, &SessID, &Pid);

        }
    else
        rc=DosStartSession(&StartData, &SessID, &Pid);
    }
}
switch(rc)
{
case NO_ERROR:                          /* Error codes for errors that are informational */
case ERROR_SMG_START_IN_BACKGROUND:
    break;

case ERROR_INVALID_HANDLE:
    sprintf(ucMessageBuffer, "An error occured while launching \"%s\". Please ensure that the WorkPlace "\
        "Shell (WPS) process (PMSHELL.EXE) is running - continuing...",
        pSessionData->PgmTitle);
    USR_ERR(pHP->hwndFrame, HELP_LAUNCH, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
        ucMessageBuffer);
    break;

default:
    sprintf(ucMessageBuffer, 
        "An error occured while launching \"%s\" (Executable \"%s\" from Directory \"%c:\\%s\").\n\n"\
        "Module \"%s\" contributed to the failure - continuing...",
        pSessionData->PgmTitle, pSessionData->PgmName, 
        'A'+((char)ulCurrentDrive-1), ucCurrentDirectory, StartData.ObjectBuffer);
    DOS_ERR(rc, pHP->hwndFrame, HELP_LAUNCH, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
        ucMessageBuffer);
    break;
}
                                        /* Set PC/2's startup priority */
if((pSessionData->PriorityClass!=PRTYC_NOCHANGE) || (pSessionData->PriorityDelta!=PRTYC_NOCHANGE))
    {
    rc=DosSetPriority(PRTYS_PROCESSTREE, (ULONG)PRTYC_REGULAR, (LONG)(-pSessionData->PriorityDelta),
        pHP->ProcessId);
    if(rc!=NO_ERROR)
        {
        sprintf(ucMessageBuffer, "Can't change the priority back to default after launching %s - continuing...",
            pSessionData->PgmTitle);
        DOS_ERR(rc, pHP->hwndFrame, HELP_PRIORITY, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            ucMessageBuffer);
        }
    }
/*                                                                                      *\
 * Release memory allocated for the DOS Settings or for OS/2 sessions environments.     *
\*                                                                                      */
                                        /* I know goto statements are not a good style except for
                                           a few things, and avoiding duplicate code should be one
                                           of them */
CANCELLED_BY_USER:
                                        /* If PC/2 inherited a customized environment,
                                           its now time to deallocate it */
if(StartData.Environment)
    free(StartData.Environment);
                                        /* Reset dynamic LIBPATH (WARP +) to the settings PC/2 inherited from the
                                           shell (to remove any modifications made for the started session).
                                           Due to an OS/2 bug (?) under at least WARP 4, calling the DosSetExtLIBPATH()
                                           API modifys the string passed (the CP reference has no hint on this) */
if(pHP->DosSetExtLIBPATH)
    {
    UCHAR   *pucDynamicLibpath;

    if(pHP->pucBeginLibpath)
        {
        pucDynamicLibpath=strdup(pHP->pucBeginLibpath);
        pHP->DosSetExtLIBPATH(&pucDynamicLibpath[BEGINLIBPATHLEN], BEGINLIBPATH);
        free(pucDynamicLibpath);
        }
    if(pHP->pucEndLibpath)
        {
        pucDynamicLibpath=strdup(pHP->pucEndLibpath);
        pHP->DosSetExtLIBPATH(&pucDynamicLibpath[ENDLIBPATHLEN], ENDLIBPATH);
        free(pucDynamicLibpath);
        }
    }
#ifdef  DEBUG_UTILITY
printf("Exiting StartSession\n");
#endif  /* DEBUG_UTILITY */
free(StartData.ObjectBuffer);
}

/*--------------------------------------------------------------------------------------*\
 * Procedure to load a SESSIONDATA structure from a MENUDATA structure.                 *
 * Req:                                                                                 *
 *      pMenuData ..... A pointer to a MENUDATA structure to extract the data required  *
 *                      for a Menu/Program Installation dialog.                         *
 *      pSessionData .. A pointer to a SESSIONDATA structure to write the extracted     *
 *                      data into, which is then used in subsequent Menu/Program        *
 *                      Installation dialogs window procedures.                         *
 * Returns:                                                                             *
 *      TRUE/FALSE .... If called sucessfully/unsucessfully                             *
\*--------------------------------------------------------------------------------------*/
BOOL    LoadMenuData2SessionData(MENUDATA *pMenuData, SESSIONDATA *pSessionData)
{
pSessionData->id=pMenuData->id;
pSessionData->StatusFlag=pMenuData->StatusFlag;
strcpy(pSessionData->PgmTitle, pMenuData->PgmTitle);
strcpy(pSessionData->WindowTitle, pMenuData->WindowTitle);
strcpy(pSessionData->PgmName, pMenuData->PgmName);
strcpy(pSessionData->PgmDirectory, pMenuData->PgmDirectory);
strcpy(pSessionData->PgmInputs, pMenuData->PgmInputs);
strcpy(pSessionData->IconFile, pMenuData->IconFile);
/*                                                                                      *\
 * Just straight forward copy of data from MENUDATA structure to SESSIONDATA structure. *
 * The allocated MENUDATA structure is filled during allocation with default values,    *
 * we don't differentiate between empty and non-empty structures any more.              *
\*                                                                                      */
pSessionData->PgmDosSettings=strdup(pMenuData->PgmDosSettings);
pSessionData->PgmLeadingEnvironment=strdup(pMenuData->PgmLeadingEnvironment);
pSessionData->PgmTrailingEnvironment=strdup(pMenuData->PgmTrailingEnvironment);
pSessionData->EnvironmentSpaces=strdup(pMenuData->EnvironmentSpaces);
pSessionData->SessionType=pMenuData->SessionType;
pSessionData->PgmControl=pMenuData->PgmControl;
pSessionData->AutostartDelay=pMenuData->AutostartDelay;
pSessionData->FgBg=pMenuData->FgBg;
pSessionData->InitXPos=pMenuData->InitXPos;
pSessionData->InitYPos=pMenuData->InitYPos;
pSessionData->InitXSize=pMenuData->InitXSize;
pSessionData->InitYSize=pMenuData->InitYSize;
(pSessionData->KeyData).usFlags=(pMenuData->KeyData).usFlags;
(pSessionData->KeyData).usCh=(pMenuData->KeyData).usCh;
(pSessionData->KeyData).pMenuData=pMenuData;
pSessionData->SwpFlag=pMenuData->SwpFlag;
pSessionData->PriorityClass=pMenuData->PriorityClass;
pSessionData->PriorityDelta=pMenuData->PriorityDelta;
memcpy(&pSessionData->SDScheduler, &pMenuData->SDScheduler, sizeof(SCHEDULERDATA));
return(TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * Procedure to save a MENUDATA structure to a SESSIONDATA structure.                   *
 * Req:                                                                                 *
 *      pMenuData ..... A pointer to a MENUDATA structure to write the data from a      *
 *                      Menu/Program Installation dialog.                               *
 *      pSessionData .. A pointer to a SESSIONDATA structure to extract the data from,  *
 *                      which the user entered.                                         *
 * Returns:                                                                             *
 *      TRUE/FALSE .... If called sucessfully/unsucessfully                             *
\*--------------------------------------------------------------------------------------*/
BOOL    LoadSessionData2MenuData(MENUDATA *pMenuData, SESSIONDATA *pSessionData)
{
pMenuData->id=pSessionData->id;
pMenuData->StatusFlag=pSessionData->StatusFlag;
                                        /* Ignore if not changed otherwise release memory
                                           and allocate a new one */
if(strcmp(pMenuData->PgmTitle, pSessionData->PgmTitle)!=0)
    {
    free(pMenuData->PgmTitle);
    pMenuData->PgmTitle=strdup(pSessionData->PgmTitle);
    }
if(strcmp(pMenuData->WindowTitle, pSessionData->WindowTitle)!=0)
    {
    free(pMenuData->WindowTitle);
    pMenuData->WindowTitle=strdup(pSessionData->WindowTitle);
    }
if(strcmp(pMenuData->PgmName, pSessionData->PgmName)!=0)
    {
    free(pMenuData->PgmName);
    pMenuData->PgmName=strdup(pSessionData->PgmName);
    }
if(strcmp(pMenuData->PgmDirectory, pSessionData->PgmDirectory)!=0)
    {
    free(pMenuData->PgmDirectory);
    pMenuData->PgmDirectory=strdup(pSessionData->PgmDirectory);
    }
if(strcmp(pMenuData->PgmInputs, pSessionData->PgmInputs)!=0)
    {
    free(pMenuData->PgmInputs);
    pMenuData->PgmInputs=strdup(pSessionData->PgmInputs);
    }
if(strcmp(pMenuData->IconFile, pSessionData->IconFile)!=0)
    {
    free(pMenuData->IconFile);
    pMenuData->IconFile=strdup(pSessionData->IconFile);
    }
switch(pSessionData->SessionType)
{                                       /* Save DOS settings for DOS type (WIN-OS2 are DOS type) sessions */
case PROG_VDM:
case PROG_WINDOWEDVDM:
case PROG_WINDOW_REAL:
case PROG_WINDOW_PROT:
case PROG_31_STD:
case PROG_31_ENH:
case PROG_31_STDSEAMLESSVDM:
case PROG_31_STDSEAMLESSCOMMON:
case PROG_31_ENHSEAMLESSVDM:
case PROG_31_ENHSEAMLESSCOMMON:
                                        /* Accept DOS sessings */
    free(pMenuData->PgmDosSettings);
    pMenuData->PgmDosSettings=pSessionData->PgmDosSettings;
    break;

default:
                                        /* For all other we load empty DOS settings.
                                           Normally they are empty, but if someone
                                           modifys the profile there may be some, which
                                           we force to be cleared */
    free(pSessionData->PgmDosSettings);
    free(pMenuData->PgmDosSettings);
    pMenuData->PgmDosSettings=strdup("");
    break;
}
switch(pSessionData->SessionType)
{
case PROG_PM:
case PROG_FULLSCREEN:
case PROG_WINDOWABLEVIO:
case PROG_DETACHED:
                                        /* Accept environment modifications */
    free(pMenuData->PgmLeadingEnvironment);
    free(pMenuData->PgmTrailingEnvironment);
    pMenuData->PgmLeadingEnvironment=pSessionData->PgmLeadingEnvironment;
    pMenuData->PgmTrailingEnvironment=pSessionData->PgmTrailingEnvironment;
    break;

default:
                                        /* For all DOS based sessions, don't accept an environment */
    free(pSessionData->PgmLeadingEnvironment);
    free(pMenuData->PgmLeadingEnvironment);
    pMenuData->PgmLeadingEnvironment=strdup("");
    free(pSessionData->PgmTrailingEnvironment);
    free(pMenuData->PgmTrailingEnvironment);
    pMenuData->PgmTrailingEnvironment=strdup("");
}
free(pMenuData->EnvironmentSpaces);
pMenuData->EnvironmentSpaces=pSessionData->EnvironmentSpaces;
pMenuData->SessionType=pSessionData->SessionType;
pMenuData->PgmControl=pSessionData->PgmControl;
pMenuData->AutostartDelay=pSessionData->AutostartDelay;
pMenuData->FgBg=pSessionData->FgBg;
pMenuData->InitXPos=pSessionData->InitXPos;
pMenuData->InitYPos=pSessionData->InitYPos;
pMenuData->InitXSize=pSessionData->InitXSize;
pMenuData->InitYSize=pSessionData->InitYSize;
(pMenuData->KeyData).usFlags=(pSessionData->KeyData).usFlags;
(pMenuData->KeyData).usCh=(pSessionData->KeyData).usCh;
pMenuData->SwpFlag=pSessionData->SwpFlag;
pMenuData->PriorityClass=pSessionData->PriorityClass;
pMenuData->PriorityDelta=pSessionData->PriorityDelta;
memcpy(&pMenuData->SDScheduler, &pSessionData->SDScheduler, sizeof(SCHEDULERDATA));
return(TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure allocates a MENUDATA structure and initializes it to the default      *
 * values of an empty structure or duplicates an existing one.                          *
 * Req:                                                                                 *
 *      none                                                                            *
 * Returns:                                                                             *
 *      pMenuData ..... A pointer to a MENUDATA structure                               *
\*--------------------------------------------------------------------------------------*/
MENUDATA *AllocateMenuData(MENUDATA *pMenuDataCloneFrom)
{
MENUDATA        *pMenuData;
UCHAR           *pucNullString="";

pMenuData=malloc(sizeof(MENUDATA));     /* Allocate a MENUDATA structure */
if(pMenuDataCloneFrom==NULL)
    {
                                        /* It's an empty structure */
    memset(pMenuData, '\0', sizeof(MENUDATA));
    pMenuData->Item=ENTRYEMPTY;
    pMenuData->StatusFlag=0;
    pMenuData->id=0;
    pMenuData->hwndItem=NULLHANDLE;
                                        /* Load default values */
    pMenuData->PgmTitle=strdup(pucNullString);
    pMenuData->WindowTitle=strdup(pucNullString);
    pMenuData->PgmName=strdup(pucNullString);
    pMenuData->PgmDirectory=strdup(pucNullString);
    pMenuData->PgmFullPathExecutable=strdup(pucNullString);
    pMenuData->PgmInputs=strdup(pucNullString);
    pMenuData->IconFile=strdup(pucNullString);
    pMenuData->PgmDosSettings=strdup(pucNullString);
    pMenuData->PgmLeadingEnvironment=strdup(pucNullString);
    pMenuData->PgmTrailingEnvironment=strdup(pucNullString);
    pMenuData->EnvironmentSpaces=strdup(pucNullString);
    pMenuData->SessionType=PROG_DEFAULT;
    pMenuData->PgmControl=SSF_CONTROL_VISIBLE;
    pMenuData->AutostartDelay=0;
    pMenuData->FgBg=SSF_FGBG_FORE;
    pMenuData->InitXPos=pHP->swpScreen.cx*0.15;
    pMenuData->InitYPos=pHP->swpScreen.cy*0.15;
    pMenuData->InitXSize=pHP->swpScreen.cx*0.70;
    pMenuData->InitYSize=pHP->swpScreen.cy*0.70;
    (pMenuData->KeyData).usFlags=KC_CTRL|KC_ALT;
                                        /* Already done by memset()
    (pMenuData->KeyData).usCh=0;
    (pMenuData->KeyData).ucScanCode=0;
    (pMenuData->KeyData).bUsed=0;
    (pMenuData->KeyData).pMenuData=NULL;
    pMenuData->SwpFlag=0;
                                           so we don't need initialization again */
    pMenuData->PriorityClass=PRTYC_NOCHANGE;
    pMenuData->PriorityDelta=PRTYC_NOCHANGE;
                                        /* Already done by memset()
    pMenuData->hpointerIcon=NULLHANDLE;
    pMenuData->Back=NULL;
    pMenuData->Submenu=NULL;
    pMenuData->Next=NULL;
                                           so we don't need initialization again */
    }
else
    {
                                        /* Copy an existing structure */
    memcpy(pMenuData, pMenuDataCloneFrom, sizeof(MENUDATA));
                                        /* Correct those values that can't be copied */
    pMenuData->hwndItem=NULLHANDLE;
    pMenuData->id=0;
    pMenuData->PgmTitle=strdup(pMenuDataCloneFrom->PgmTitle);
    pMenuData->WindowTitle=strdup(pMenuDataCloneFrom->WindowTitle);
    pMenuData->PgmName=strdup(pMenuDataCloneFrom->PgmName);
    pMenuData->PgmDirectory=strdup(pMenuDataCloneFrom->PgmDirectory);
    pMenuData->PgmInputs=strdup(pMenuDataCloneFrom->PgmInputs);
    pMenuData->IconFile=strdup(pMenuDataCloneFrom->IconFile);
    pMenuData->PgmFullPathExecutable=strdup(pMenuDataCloneFrom->PgmFullPathExecutable);
    pMenuData->PgmDosSettings=strdup(pMenuDataCloneFrom->PgmDosSettings);
    pMenuData->PgmLeadingEnvironment=strdup(pMenuDataCloneFrom->PgmLeadingEnvironment);
    pMenuData->PgmTrailingEnvironment=strdup(pMenuDataCloneFrom->PgmTrailingEnvironment);
    pMenuData->EnvironmentSpaces=strdup(pMenuDataCloneFrom->EnvironmentSpaces);
    memset(&pMenuData->KeyData, 0, sizeof(pMenuData->KeyData));
    (pMenuData->KeyData).usFlags=KC_CTRL|KC_ALT;
    pMenuData->Back=NULL;
    pMenuData->Submenu=NULL;
    pMenuData->Next=NULL;
    }
return(pMenuData);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure deallocates a MENUDATA structure                                      *
 * Req:                                                                                 *
 *      pMenuData ..... A pointer to a MENUDATA structure                               *
 * Returns:                                                                             *
 *      none                                                                            *
\*--------------------------------------------------------------------------------------*/
void    DeAllocateMenuData(MENUDATA *pMenuData)
{
                                        /* If the Configuration dialog is removed, we have to
                                           display this dialog instead of the Popup Menu (or we will
                                           no more be able to activate the Configuration dialog */
if(pMenuData->id==ID_CONFIGDIALOG)
    pHP->ulRuntimeFlag|=DISPLAYCONFIGDIALOG;
                                        /* Destroy the pointer resources used for this menuentry */
if(pHP->ulStatusFlag2 & POPUPICON)
    InitializeIcon(pMenuData, FALSE);
                                        /* Cleanup */
free(pMenuData->PgmTitle);
free(pMenuData->WindowTitle);
free(pMenuData->PgmName);
free(pMenuData->PgmDirectory);
free(pMenuData->PgmInputs);
free(pMenuData->IconFile);
free(pMenuData->PgmFullPathExecutable);
free(pMenuData->PgmDosSettings);
free(pMenuData->PgmLeadingEnvironment);
free(pMenuData->PgmTrailingEnvironment);
free(pMenuData->EnvironmentSpaces);
                                        /* Loop for all keys and delete used one if applicable */
if(pMenuData->KeyData.usFlags && pMenuData->KeyData.usCh)
    {
    ULONG   ulTemp;
    KEYDATA *pKeyDataIndex=pHP->pKeyData;

    for(ulTemp=0; ulTemp<KEYDATACOUNT; pKeyDataIndex++, ulTemp++)
        {
                                        /* Clear the menuentry's current key */
        if((pKeyDataIndex->usCh==pMenuData->KeyData.usCh) &&
            (pKeyDataIndex->usFlags==pMenuData->KeyData.usFlags))
            {
            pKeyDataIndex->bUsed=FALSE;
            pKeyDataIndex->pMenuData=NULL;
            }
        }
    }
memset(pMenuData, 0, sizeof(MENUDATA));
free(pMenuData);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure loads the Scheduler data for a menuentry from the profile.            *
 * Req:                                                                                 *
 *      pSchedulerData  A pointer to a SCHEDULERDATA structure to load setup into       *
 *      fileProfile ..  A pointer to the file to read from                              *
 * Returns:                                                                             *
 *      mresult ......  TRUE                                                            *
\*--------------------------------------------------------------------------------------*/
MRESULT LoadSchedulerData(SCHEDULERDATA *pSchedulerData, FILE *fileProfile)
{
ULONG           ulIndex;
ULONG           ulShiftMask;
ULONG           *pulMask;
ULONG           ulValue;
UCHAR           *pucTemp;

fgets(Buffer, sizeof(Buffer), fileProfile);
pSchedulerData->ulSchedulerFlag=SCHEDULE_NONE;
while((!strstr(Buffer, "SCHEDULERDATA END")) &&
    (!feof(fileProfile)))
    {
                                        /* Remove trailing \r and/or \n and leading spaces */
    StripTrailing(Buffer);
                                        /* Control settings, skip SCHEDULE_NONE, because this is
                                           our default assumption */
    if(strstr(Buffer, "Control:"))
        {
        for(ulIndex=1, ulShiftMask=0x00000002;
            ulIndex<(sizeof(pucSchedulerControl)/sizeof(pucSchedulerControl[0]));
            ulIndex++, ulShiftMask<<=1)
            if(strstr(Buffer, pucSchedulerControl[ulIndex]))
                {
                pSchedulerData->ulSchedulerFlag&=~(ulShiftMask|SCHEDULE_SAVEANYWAY);
                pSchedulerData->ulSchedulerFlag|=ulShiftMask;
                }
        }
                                        /* Check for minute */
    else if(strstr(Buffer, "Minute:"))
        {
        pucTemp=strchr(Buffer, ':')+1;
        while(pucTemp)
            {
            ulValue=atoi(pucTemp);
            pucTemp=StripLeading(pucTemp);
            pulMask=&pSchedulerData->ulStartingMinute1;
            for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=ulValue; ulIndex++, ulShiftMask<<=1)
                {
                if(ulIndex==(sizeof(ULONG)*8-1))
                    {
                    ulShiftMask=0x00000001;
                    pulMask=&pSchedulerData->ulStartingMinute2;
                    }
                if(ulIndex==ulValue)
                    {
                    (*pulMask)|=ulShiftMask;
                    break;
                    }
                }
            pucTemp=strchr(pucTemp, ' ');
            }
        }
                                        /* Check for hour */
    else if(strstr(Buffer, "Hour:"))
        {
        pucTemp=strchr(Buffer, ':')+1;
        while(pucTemp)
            {
            ulValue=atoi(pucTemp);
            pucTemp=StripLeading(pucTemp);
            for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=ulValue; ulIndex++, ulShiftMask<<=1)
                {
                if(ulIndex==ulValue)
                    {
                    pSchedulerData->ulStartingHour|=ulShiftMask;
                    break;
                    }
                }
            pucTemp=strchr(pucTemp, ' ');
            }
        }
                                        /* Check for weekday */
    else if(strstr(Buffer, "Weekday:"))
        for(ulIndex=0, ulShiftMask=0x00000001;
            ulIndex<(sizeof(pucWeekDay)/sizeof(pucWeekDay[0]));
            ulIndex++, ulShiftMask<<=1)
            {
            if(strstr(Buffer, pucWeekDay[ulIndex]))
                pSchedulerData->ulStartingWeekday|=ulShiftMask;
            }
                                        /* Check for day */
    else if(strstr(Buffer, "Day:"))
        {
        pucTemp=strchr(Buffer, ':')+1;
        while(pucTemp)
            {
            ulValue=atoi(pucTemp);
            pucTemp=StripLeading(pucTemp);
            pulMask=&pSchedulerData->ulStartingMinute1;
            for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=ulValue; ulIndex++, ulShiftMask<<=1)
                {
                if(ulIndex==(ulValue-1))
                    {
                    pSchedulerData->ulStartingDay|=ulShiftMask;
                    break;
                    }
                }
            pucTemp=strchr(pucTemp, ' ');
            }
        }
                                        /* Check for month */
    else if(strstr(Buffer, "Month:"))
        {
        for(ulIndex=0, ulShiftMask=0x00000001;
            ulIndex<(sizeof(pucMonth)/sizeof(pucMonth[0]));
            ulIndex++, ulShiftMask<<=1)
            {
            if(strstr(Buffer, pucMonth[ulIndex]))
                pSchedulerData->ulStartingMonth|=ulShiftMask;
            }
        }
    fgets(Buffer, sizeof(Buffer), fileProfile);
    }
return((MRESULT)TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure writes the Scheduler data from a menuentry into the profile.          *
 * Req:                                                                                 *
 *      pSchedulerData  A pointer to a SCHEDULERDATA structure to load setup into       *
 *      fileProfile ..  A pointer to the file to read from                              *
 * Returns:                                                                             *
 *      mresult ......  TRUE                                                            *
\*--------------------------------------------------------------------------------------*/
MRESULT SaveSchedulerData(SCHEDULERDATA *pSchedulerData, FILE *fileProfile)
{
ULONG           ulIndex;
ULONG           ulShiftMask;
ULONG           *pulMask;

fprintf(fileProfile, "    SCHEDULERDATA BEGIN\n");
                                        /* Save control flag, except for SCHEDULE_NONE */
for(ulIndex=1, ulShiftMask=0x00000002;
    ulIndex<(sizeof(pucSchedulerControl)/sizeof(pucSchedulerControl[0]));
    ulIndex++, ulShiftMask<<=1)
    if(pSchedulerData->ulSchedulerFlag&ulShiftMask)
        fprintf(fileProfile, "     Control: %s\n", pucSchedulerControl[ulIndex]);
                                        /* Save minute */
pulMask=&pSchedulerData->ulStartingMinute1;
fprintf(fileProfile, "     Minute:");
for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=59; ulIndex++, ulShiftMask<<=1)
    {
    if(ulIndex==(sizeof(ULONG)*8-1))
        {
        ulShiftMask=0x00000001;
        pulMask=&pSchedulerData->ulStartingMinute2;
        }
    if((*pulMask)&ulShiftMask)
        fprintf(fileProfile, " %ld", ulIndex);
    }
fprintf(fileProfile, "\n");
                                        /* Save hour */
if(pSchedulerData->ulSchedulerFlag>=SCHEDULE_DAILY)
    {
    fprintf(fileProfile, "     Hour:");
    for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=23; ulIndex++, ulShiftMask<<=1)
        {
        if(pSchedulerData->ulStartingHour&ulShiftMask)
            fprintf(fileProfile, " %ld", ulIndex);
        }
    fprintf(fileProfile, "\n");
    }
                                        /* Save weekday */
if(pSchedulerData->ulSchedulerFlag & (SCHEDULE_SAVEANYWAY|SCHEDULE_WEEKLY))
    {
    fprintf(fileProfile, "     Weekday:");
    for(ulIndex=0, ulShiftMask=0x00000001;
        ulIndex<(sizeof(pucWeekDay)/sizeof(pucWeekDay[0]));
        ulIndex++, ulShiftMask<<=1)
        {
        if(pSchedulerData->ulStartingWeekday&ulShiftMask)
            fprintf(fileProfile, " %s", pucWeekDay[ulIndex]);
        }
    fprintf(fileProfile, "\n");
    }
                                        /* Save day, the first day (bit 0) is day 1 */
if(pSchedulerData->ulSchedulerFlag & (SCHEDULE_SAVEANYWAY|SCHEDULE_ONCE|SCHEDULE_MONTHLY))
    {
    fprintf(fileProfile, "     Day:");
    for(ulIndex=1, ulShiftMask=0x00000001; ulIndex<=31; ulIndex++, ulShiftMask<<=1)
        {
        if(pSchedulerData->ulStartingDay&ulShiftMask)
            fprintf(fileProfile, " %ld", ulIndex);
        }
    fprintf(fileProfile, "\n");
    }
                                        /* Save month */
if(pSchedulerData->ulSchedulerFlag & (SCHEDULE_SAVEANYWAY|SCHEDULE_ONCE))
    {
    fprintf(fileProfile, "     Month:");
    for(ulIndex=0, ulShiftMask=0x00000001;
        ulIndex<(sizeof(pucMonth)/sizeof(pucMonth[0]));
        ulIndex++, ulShiftMask<<=1)
        {
        if(pSchedulerData->ulStartingMonth&ulShiftMask)
            fprintf(fileProfile, " %s", pucMonth[ulIndex]);
        }
    fprintf(fileProfile, "\n");
    }
                                        /* Scheduler data end */
fprintf(fileProfile, "    SCHEDULERDATA END\n");
return((MRESULT)TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * This recursive procedure loads the popup menu from the profile.                      *
 * Req:                                                                                 *
 *      pfileProfile .. A pointer to the FILE handle to load from.                      *
 *      pMenuData ..... A pointer to an MENUDATA structure.                             *
 * Returns:                                                                             *
 *      none                                                                            *
\*--------------------------------------------------------------------------------------*/
void    LoadMenu(FILE *pfileProfile, MENUDATA *pMenuData, BOOL bReadNextLine)
{
UCHAR   *pucMatch;
ULONG   ulFlag;

if(bReadNextLine==TRUE)
    fgets(Buffer, sizeof(Buffer), pfileProfile);
do
{
                                        /* Should read MENUITEM or MENUCONTROL or
                                           SUBMENU BEGIN or SUBMENU END */
    if(strstr(Buffer, "SUBMENU END"))
        {
        fgets(Buffer, sizeof(Buffer), pfileProfile);
        return;                         /* We are at an end of the list, terminate it
                                           and shell up one level by return() */
        }
    pMenuData->id=pHP->MenuDataId++;    /* Fill with current id and increment id */
    if(strstr(Buffer, "PROFILE END")) return;
    if(strstr(Buffer, "MENUITEM"))
        ulFlag=ENTRYMENUITEM;
    else if(strstr(Buffer, "MENUCONTROL"))
            ulFlag=ENTRYCONTROL;
        else
            ulFlag=ENTRYSUBMENU;

/*                                                                                      *\
 * Get the entry from the profile, but remove the heading description and the \n from   *
 * the strings.                                                                         *
\*                                                                                      */
                                        /* Get the session title */
    fgets(Buffer, sizeof(Buffer), pfileProfile);
                                        /* Remove trailing \r and/or \n and leading spaces */
    StripTrailing(Buffer);
    pucMatch=StripLeading(Buffer);
                                        /* Find text after PgmTitle keyboard */
    if((pucMatch=strchr(pucMatch, ' '))==NULL)
        pucMatch=strchr(Buffer, '\0');
    else
        pucMatch=StripLeading(pucMatch);
    free(pMenuData->PgmTitle);
    pMenuData->PgmTitle=strdup(pucMatch);
    if(ulFlag==ENTRYMENUITEM)
        {
        pMenuData->Item=ENTRYMENUITEM;  /* It's a Menuitem */
        do                              /* Do until a complete MENUITEM was read */
            {
                                        /* Get a new line */
            fgets(Buffer, sizeof(Buffer), pfileProfile);
                                        /* Remove trailing \r and/or \n */
            StripTrailing(Buffer);
                                        /* Find first space after a ':' or NULL, if
                                           none can be found */
            pucMatch=strchr(Buffer, ':');
            if(pucMatch==NULL) pucMatch=strchr(Buffer, '\0');
            else                        /* We found a ':', so we search for ' ' and
                                           return it or NULL, if none can be found */
                if((pucMatch=strchr(pucMatch, ' '))==NULL)
                    pucMatch=strchr(Buffer, '\0');
                else
                    pucMatch=StripLeading(pucMatch);
                                        /* Now fill in the characters after the
                                           space, according for what structure
                                           element it is given */
            if(strstr(Buffer, "WindowTitle"))
                {
                free(pMenuData->WindowTitle);
                pMenuData->WindowTitle=strdup(pucMatch);
                }
            else if(strstr(Buffer, "PgmName"))
                {
                free(pMenuData->PgmName);
                pMenuData->PgmName=strdup(pucMatch);
                }
            else if(strstr(Buffer, "PgmDirectory"))
                {
                free(pMenuData->PgmDirectory);
                pMenuData->PgmDirectory=strdup(pucMatch);
                }
            else if(strstr(Buffer, "PgmExecutable"))
                {
                free(pMenuData->PgmFullPathExecutable);
                pMenuData->PgmFullPathExecutable=strdup(pucMatch);
                }
            else if(strstr(Buffer, "PgmInputs"))
                {
                free(pMenuData->PgmInputs);
                pMenuData->PgmInputs=strdup(pucMatch);
                }
            else if(strstr(Buffer, "IconFile"))
                {
                free(pMenuData->IconFile);
                pMenuData->IconFile=strdup(pucMatch);
                }
            else if(strstr(Buffer, "ENVIRONMENTSPACES BEGIN"))
                {
                ULONG   ulEnvironmentSpacesLength;
                UCHAR   *pucTemp;

                fgets(Buffer, sizeof(Buffer), pfileProfile);
                while((!strstr(Buffer, "ENVIRONMENTSPACES END")) &&
                    (!feof(pfileProfile)))
                    {
                    UCHAR   *pucSetting;

                                        /* Remove trailing \r and/or \n and leading spaces */
                    StripTrailing(Buffer);
                    pucSetting=StripLeading(Buffer);
                                        /* Allocate space to add next environment space
                                           to the already existing (including \r\n\0) */
                    ulEnvironmentSpacesLength=strlen(pMenuData->EnvironmentSpaces)+strlen(pucSetting)+3;
                    pucTemp=malloc(ulEnvironmentSpacesLength);
                    if(pucTemp!=NULL)
                        {
                        strcpy(pucTemp, pMenuData->EnvironmentSpaces);
                        strcat(pucTemp, pucSetting);
                        strcat(pucTemp, "\r\n");
                        free(pMenuData->EnvironmentSpaces);
                        pMenuData->EnvironmentSpaces=pucTemp;
                        }
                    fgets(Buffer, sizeof(Buffer), pfileProfile);
                    }
                }
            else if(strstr(Buffer, "DOSSETTINGS BEGIN"))
                {
                ULONG   ulDosSettingsLength;
                UCHAR   *pucTemp;

                fgets(Buffer, sizeof(Buffer), pfileProfile);
                while((!strstr(Buffer, "DOSSETTINGS END")) &&
                    (!feof(pfileProfile)))
                    {
                    UCHAR   *pucSetting;

                                        /* Remove trailing \r and/or \n and leading spaces */
                    StripTrailing(Buffer);
                    pucSetting=StripLeading(Buffer);
                                        /* Allocate space to add next DOS setting
                                           to the already existing (including \r\n\0) */
                    ulDosSettingsLength=strlen(pMenuData->PgmDosSettings)+strlen(pucSetting)+3;
                    pucTemp=malloc(ulDosSettingsLength);
                    if(pucTemp!=NULL)
                        {
                        strcpy(pucTemp, pMenuData->PgmDosSettings);
                        strcat(pucTemp, pucSetting);
                        strcat(pucTemp, "\r\n");
                        free(pMenuData->PgmDosSettings);
                        pMenuData->PgmDosSettings=pucTemp;
                        }
                    fgets(Buffer, sizeof(Buffer), pfileProfile);
                    }
                }
            else if(strstr(Buffer, "LEADING ENVIRONMENT BEGIN"))
                {
                ULONG   ulEnvSettingsLength;
                UCHAR   *pucTemp;

                fgets(Buffer, sizeof(Buffer), pfileProfile);
                while((!strstr(Buffer, "ENVIRONMENT END")) &&
                    (!feof(pfileProfile)))
                    {
                    UCHAR   *pucSetting;

                                        /* Remove trailing \r and/or \n and leading spaces */
                    StripTrailing(Buffer);
                    pucSetting=StripLeading(Buffer);
                                        /* Allocate space to add next Environment setting
                                           to the already existing (including \r\n\0) */
                    ulEnvSettingsLength=strlen(pMenuData->PgmLeadingEnvironment)+strlen(pucSetting)+3;
                    pucTemp=malloc(ulEnvSettingsLength);
                    if(pucTemp!=NULL)
                        {
                        strcpy(pucTemp, pMenuData->PgmLeadingEnvironment);
                        strcat(pucTemp, pucSetting);
                        strcat(pucTemp, "\r\n");
                        free(pMenuData->PgmLeadingEnvironment);
                        pMenuData->PgmLeadingEnvironment=pucTemp;
                        }
                    fgets(Buffer, sizeof(Buffer), pfileProfile);
                    }
                }
            else if((strstr(Buffer, "TRAILING ENVIRONMENT BEGIN")) ||
                                        /* For compatibility with 1.90-, upgrade to the trailing
                                           Environment if the old ENVIRONMENT BEGIN begins within
                                           the first 4 characters of the line */
                (((Buffer+3)>=(UCHAR *)strstr(Buffer, "ENVIRONMENT BEGIN")) &&
                (Buffer<=(UCHAR *)strstr(Buffer, "ENVIRONMENT BEGIN"))))
                {
                ULONG   ulEnvSettingsLength;
                UCHAR   *pucTemp;

                fgets(Buffer, sizeof(Buffer), pfileProfile);
                while((!strstr(Buffer, "ENVIRONMENT END")) &&
                    (!feof(pfileProfile)))
                    {
                    UCHAR   *pucSetting;

                                        /* Remove trailing \r and/or \n and leading spaces */
                    StripTrailing(Buffer);
                    pucSetting=StripLeading(Buffer);
                                        /* Allocate space to add next Environment setting
                                           to the already existing (including \r\n\0) */
                    ulEnvSettingsLength=strlen(pMenuData->PgmTrailingEnvironment)+strlen(pucSetting)+3;
                    pucTemp=malloc(ulEnvSettingsLength);
                    if(pucTemp!=NULL)
                        {
                        strcpy(pucTemp, pMenuData->PgmTrailingEnvironment);
                        strcat(pucTemp, pucSetting);
                        strcat(pucTemp, "\r\n");
                        free(pMenuData->PgmTrailingEnvironment);
                        pMenuData->PgmTrailingEnvironment=pucTemp;
                        }
                    fgets(Buffer, sizeof(Buffer), pfileProfile);
                    }
                }
            else if(strstr(Buffer, "SCHEDULERDATA BEGIN"))
                LoadSchedulerData(&pMenuData->SDScheduler, pfileProfile);
            else if(strstr(Buffer, "SessionType"))
                {
                ULONG   ulTemp;

                pMenuData->SessionType=(USHORT)-1;
                for(ulTemp=0; ulTemp<SESSIONTYPESIZE; ulTemp++)
                    if(strstr(pucMatch, SessionType[ulTemp].pucSessionType))
                        {
                        pMenuData->SessionType=SessionType[ulTemp].usSessionType;
                                        /* We've found it, exit loop to avoid matching of
                                           another string */
                        break;
                        }
                if(pMenuData->SessionType==(USHORT)-1)
                    pMenuData->SessionType=(USHORT)atol(pucMatch);
                }
            else if(strstr(Buffer, "PgmControl"))
                {
                ULONG   ulTemp;

                for(ulTemp=0; ulTemp<PGMCONTROLSIZE; ulTemp++)
                    if(strstr(pucMatch, PgmControl[ulTemp].pucPgmControl))
                        pMenuData->PgmControl|=PgmControl[ulTemp].usPgmControl;
                if(pMenuData->PgmControl==0)
                    pMenuData->PgmControl=(USHORT)atol(pucMatch);
                }
            else if(strstr(Buffer, "Autostartdelay"))
                {
                pMenuData->AutostartDelay=(LONG)atol(pucMatch);
                }
            else if(strstr(Buffer, "EnvControl"))
                {
                if(strstr(pucMatch, "Startup"))
                    pMenuData->StatusFlag|=STARTUPENVIRONMENTS;
                }
            else if(strstr(Buffer, "FgBg"))
                {
                if(strstr(pucMatch, "SSF_FGBG_BACK"))
                    pMenuData->FgBg=SSF_FGBG_BACK;
                else
                    pMenuData->FgBg=SSF_FGBG_FORE;
                }
            else if(strstr(Buffer, "InitXPos"))
                {
                pMenuData->InitXPos=(SHORT)atol(pucMatch);
                }
            else if(strstr(Buffer, "InitYPos"))
                {
                pMenuData->InitYPos=(SHORT)atol(pucMatch);
                }
            else if(strstr(Buffer, "InitXSize"))
                {
                pMenuData->InitXSize=(USHORT)atol(pucMatch);
                }
            else if(strstr(Buffer, "InitYSize"))
                {
                pMenuData->InitYSize=(USHORT)atol(pucMatch);
                }
            else if(strstr(Buffer, "KeyControl"))
                {
                (pMenuData->KeyData).usFlags=0;
                if(strstr(pucMatch, "KC_RCTRL"))
                    (pMenuData->KeyData).usFlags|=KC_RCTRL;
                if(strstr(pucMatch, "KC_CTRL"))
                    (pMenuData->KeyData).usFlags|=KC_CTRL;
                if(strstr(pucMatch, "KC_ALT"))
                    (pMenuData->KeyData).usFlags|=KC_ALT;
                if(strstr(pucMatch, "KC_LWIN"))
                    (pMenuData->KeyData).usFlags|=KC_LWIN;
                if((pMenuData->KeyData).usFlags==0)
                    (pMenuData->KeyData).usFlags|=(KC_CTRL|KC_ALT);
                }
            else if(strstr(Buffer, "KeyData"))
                {
                KEYDATA *pKeyDataIndex;
                ULONG   ulTemp;

                                        /* Find first char from end of string, which
                                           must be our hotkey */
                pucMatch=strrchr(Buffer, ' ');
                if(pucMatch!=NULL)
                    {
                    (pMenuData->KeyData).usCh=(USHORT)*(++pucMatch);
                    pKeyDataIndex=pHP->pKeyData;
                                        /* If we found a key set it to used */
                    for(ulTemp=0; ulTemp<KEYDATACOUNT; pKeyDataIndex++, ulTemp++)
                        if((pKeyDataIndex->usCh==(pMenuData->KeyData).usCh) &&
                            (pKeyDataIndex->usFlags==(pMenuData->KeyData).usFlags))
                            {
                            pKeyDataIndex->bUsed=TRUE;
                            pKeyDataIndex->pMenuData=pMenuData;
                            }
                    }
                }
            else if(strstr(Buffer, "WindowControl"))
                {
                if(strstr(pucMatch, "SWP_MAXIMIZE"))
                    pMenuData->SwpFlag=(pMenuData->SwpFlag | SWP_MAXIMIZE) & ~SWP_RESTORE;
                if(strstr(pucMatch, "SWP_RESTORE"))
                    pMenuData->SwpFlag=(pMenuData->SwpFlag | SWP_RESTORE) & ~SWP_MAXIMIZE;
                if(strstr(pucMatch, "SWP_NOMOVE"))
                    {
                    pMenuData->SwpFlag|=SWP_NOMOVE;
                                        /* Since PC/2 V2.10 we add the "Keep on active Desktop"
                                           windows into the Sticky Windows MLE due to user requests
                                           too, but still support the Hotkey page entry (as it is
                                           required for the Hotkey and User defined Size & Position.
                                           Ensure to accord to the format of the Sticky Windows array:
                                           [[[StickyWindow1]\r\nStickyWindow2]...]\r\n */
                    if(sizeof(pHP->ucStickyWindows)>=(strlen(pHP->ucStickyWindows)+strlen(pMenuData->WindowTitle)+3))
                        {
                        if(pHP->ucStickyWindows[0]=='\r')
                            strcpy(pHP->ucStickyWindows, pMenuData->WindowTitle);
                        else
                            strcat(pHP->ucStickyWindows, pMenuData->WindowTitle);
                        strcat(pHP->ucStickyWindows, "\r\n");
                        }
                    }
                }
            else if(strstr(Buffer, "PriorityClass"))
                {
                pMenuData->PriorityClass=(USHORT)atol(pucMatch);
                }
            else if(strstr(Buffer, "PriorityDelta"))
                {
                pMenuData->PriorityDelta=(SHORT)atol(pucMatch);
                }
            } while((!strstr(Buffer, "MENUITEM")) &&
                (!strstr(Buffer, "MENUCONTROL")) &&
                (!strstr(Buffer, "SUBMENU BEGIN")) &&
                (!strstr(Buffer, "SUBMENU END")) &&
                (!strstr(Buffer, "PROFILE END")) &&
                !feof(pfileProfile));
                                        /* The user may manipulate the profile, so ensure
                                           that bad things are prevented */
            switch(pMenuData->SessionType)
            {
            case PROG_FULLSCREEN:
            case PROG_WINDOWABLEVIO:
            case PROG_PM:
            case PROG_DETACHED:
                break;

            default:
                                        /* Priority for non OS/2 sessions lock PM, so ignore */
                pMenuData->PriorityClass=PRTYC_NOCHANGE;
                pMenuData->PriorityDelta=PRTYC_NOCHANGE;
                break;
            }
            if(pMenuData->AutostartDelay<AUTOSTARTDELAY_MIN)
                pMenuData->AutostartDelay=AUTOSTARTDELAY_MIN;
            if(pMenuData->AutostartDelay>AUTOSTARTDELAY_MAX)
                pMenuData->AutostartDelay=AUTOSTARTDELAY_MAX;
        }
    if(ulFlag==ENTRYCONTROL)
        {
        pHP->MenuDataId--;              /* Don't use the default ID, because controls have
                                           their own */
        pMenuData->Item=ENTRYCONTROL;   /* It's a Menucontrol */
                                        /* For conversion from version 1.70 to 1.80 we support
                                           also a string containing no mnemonic during read.
                                           For non-mnemonic strings we just scan for substrings.
                                           Written is always the new style */
        if(!strcmp(pMenuData->PgmTitle, CTRL_CONFIGMENU) ||
            strstr(pMenuData->PgmTitle, "~Menu"))
                { strcpy(pMenuData->PgmTitle, CTRL_CONFIGMENU); pMenuData->id=ID_CONFIGDIALOG; }
        else if(!strcmp(pMenuData->PgmTitle, CTRL_CONFIGDESKTOP) ||
            strstr(pMenuData->PgmTitle, "~Desktop"))
                { strcpy(pMenuData->PgmTitle, CTRL_CONFIGDESKTOP); pMenuData->id=ID_DESKTOPDIALOG; }
        else if(!strcmp(pMenuData->PgmTitle, CTRL_ABOUT) ||
            strstr(pMenuData->PgmTitle, "~About"))
                { strcpy(pMenuData->PgmTitle, CTRL_ABOUT); pMenuData->id=ID_ABOUTDIALOG; }
        else if(!strcmp(pMenuData->PgmTitle, CTRL_SHUTDOWN) ||
            strstr(pMenuData->PgmTitle, "~ShutDown"))
                { strcpy(pMenuData->PgmTitle, CTRL_SHUTDOWN); pMenuData->id=ID_SHUTDOWN; }
        else if(!strcmp(pMenuData->PgmTitle, CTRL_POWERDOWN) ||
            strstr(pMenuData->PgmTitle, "~PowerDown"))
                { strcpy(pMenuData->PgmTitle, CTRL_POWERDOWN); pMenuData->id=ID_POWERDOWN; }
        else if(!strcmp(pMenuData->PgmTitle, CTRL_HELP) ||
            strstr(pMenuData->PgmTitle, "~Help"))
                { strcpy(pMenuData->PgmTitle, CTRL_HELP); pMenuData->id=ID_HELP; }
        else if(!strcmp(pMenuData->PgmTitle, CTRL_SPOOLER) ||
            strstr(pMenuData->PgmTitle, "Spoo~ler"))
                { strcpy(pMenuData->PgmTitle, CTRL_SPOOLER); pMenuData->id=ID_SPOOLER; }
        else if(!strcmp(pMenuData->PgmTitle, CTRL_EXIT) ||
            strstr(pMenuData->PgmTitle, "E~xit"))
                { strcpy(pMenuData->PgmTitle, CTRL_EXIT); pMenuData->id=ID_EXIT; }
        else if(!strcmp(pMenuData->PgmTitle, CTRL_LOCKUP) ||
            strstr(pMenuData->PgmTitle, "~Lockup"))
                { strcpy(pMenuData->PgmTitle, CTRL_LOCKUP); pMenuData->id=ID_LOCKUP; }
                                        /* We assume that more than 1 separator may occur
                                           so to be able add, modify or delete on menuitems
                                           define a unique one */
        else if(!strcmp(pMenuData->PgmTitle, CTRL_BREAKSEPARATOR)) pMenuData->id=pHP->MenuDataId++;
        else if(!strcmp(pMenuData->PgmTitle, CTRL_SEPARATOR)) pMenuData->id=pHP->MenuDataId++;
                                        /* Get a new line */
        fgets(Buffer, sizeof(Buffer), pfileProfile);
                                        /* When available load Scheduler data */
        if(strstr(Buffer, "SCHEDULERDATA BEGIN"))
            {
            LoadSchedulerData(&pMenuData->SDScheduler, pfileProfile);
            fgets(Buffer, sizeof(Buffer), pfileProfile);
            }
                                        /* Replace CRLF with \0 */
        Buffer[strlen(Buffer)-1]='\0';
        }
    if(ulFlag==ENTRYSUBMENU)
        {                               /* If we load a SUBMENU BEGIN, fill with empty strings */
        MENUDATA        *pMenuDataTemp;

        pMenuData->Item=ENTRYSUBMENU;   /* It's a Submenu */
                                        /* Now obtain a entry for a submenu, adjust the
                                           linked list to it and call this procedure with
                                           the new entry recursivly again */
        pMenuDataTemp=AllocateMenuData(NULL);
        pMenuData->Submenu=pMenuDataTemp;
        pMenuDataTemp->Back=pMenuData;
                                        /* See if the Submenu is followed by the optional Icon
                                           filename */
        fgets(Buffer, sizeof(Buffer), pfileProfile);
        StripTrailing(Buffer);
        pucMatch=strchr(Buffer, ':');
        if(pucMatch==NULL) pucMatch=strchr(Buffer, '\0');
        else
            if((pucMatch=strchr(pucMatch, ' '))==NULL)
                pucMatch=strchr(Buffer, '\0');
            else
                pucMatch=StripLeading(pucMatch);
        if(strstr(Buffer, "IconFile"))
            {
            free(pMenuData->IconFile);
            pMenuData->IconFile=strdup(pucMatch);
            LoadMenu(pfileProfile, pMenuDataTemp, TRUE);
            }
        else
            LoadMenu(pfileProfile, pMenuDataTemp, FALSE);
                                        /* It's assumed to be an empty entry, which will
                                           be corrected, if the first entry of the Submenu
                                           is found. As after recursion into we continue
                                           here, the next statement must be the test
                                           for the end of the profile */
        }
/*                                                                                      *\
 * Now see if we're at the end of the profile. If so, then terminate linked list with   *
 * 2 Null pointers, otherwise abtain a new menu space and adjust the menu pointer       *
 * pMenuData to the newly created menu.                                                 *
\*                                                                                      */
    if(strstr(Buffer, "PROFILE END"))
        break;                          /* Empty lines may follow and feof() then is FALSE
                                           and we loop again, reading invalid input. Avoid
                                           this by breaking out of the loop */
    else
        {                               /* If a SUBMENU END follows ignore it, because
                                           execution will return at beginning of the loop
                                           otherwise add a new item to the end of the
                                           linked list */
        if(!strstr(Buffer, "SUBMENU END"))
            {
            MENUDATA    *pMenuDataTemp;

            pMenuDataTemp=AllocateMenuData(NULL);
            pMenuData->Next=pMenuDataTemp;
            pMenuDataTemp->Back=pMenuData;
            pMenuData=pMenuData->Next;
            }
        }
} while(!feof(pfileProfile));
return;
}

/*--------------------------------------------------------------------------------------*\
 * This recursive procedure saves the popup menu into the profile.                      *
 * Req:                                                                                 *
 *      pfileProfile .. A pointer to the FILE handle to write to.                       *
 *      pMenuData ..... A pointer to an MENUDATA structure.                             *
 * Returns:                                                                             *
 *      none                                                                            *
\*--------------------------------------------------------------------------------------*/
void    SaveMenu(FILE *pfileProfile, MENUDATA *pMenuData)
{
                                        /* Reset the Popup-Menu icons cache flag, as we're going
                                           to save the caches now too */
pHP->ulRuntimeFlag&=(~ICONCACHEACTIVE);
do
{
    if(pMenuData->Item==ENTRYSUBMENU)
        {
/*                                                                                      *\
 * If this is a SUBMENU, then write the header SUBMENU BEGIN and then write the profile *
 * data from teh MENUDATA structure pointet by pMenuData. Then increment the depth      *
 * counter and call this procedure recursivly again. After coming back, restore the     *
 * depth counter and write the header SUBMENU END.                                      *
\*                                                                                      */
        fprintf(pfileProfile, "SUBMENU BEGIN\n");
        fprintf(pfileProfile, " PgmTitle: %s\n", pMenuData->PgmTitle);
        if(strcmp(pMenuData->IconFile, ""))
            fprintf(pfileProfile, " IconFile: %s\n", pMenuData->IconFile);
        SaveMenu(pfileProfile, pMenuData->Submenu);
        fprintf(pfileProfile, "SUBMENU END\n");
        }
    if(pMenuData->Item==ENTRYMENUITEM)
        {
/*                                                                                      *\
 * If it is a MENUITEM, so write the header MENUITEM and then write the profile data    *
 * from the MENUDATA structure pointed by pMenuData.                                    *
\*                                                                                      */
        fprintf(pfileProfile, "  MENUITEM\n");
        fprintf(pfileProfile, "   PgmTitle: %s\n", pMenuData->PgmTitle);
                                        /* Window's title only written if defined */
        if(strcmp(pMenuData->WindowTitle, ""))
            fprintf(pfileProfile, "   WindowTitle: %s\n", pMenuData->WindowTitle);
        if(strcmp(pMenuData->PgmName, ""))
            fprintf(pfileProfile, "   PgmName: %s\n", pMenuData->PgmName);
        if(strcmp(pMenuData->PgmDirectory, ""))
            fprintf(pfileProfile, "   PgmDirectory: %s\n", pMenuData->PgmDirectory);
        if(strcmp(pMenuData->PgmFullPathExecutable, ""))
            fprintf(pfileProfile, "   PgmExecutable: %s\n", pMenuData->PgmFullPathExecutable);
        if(strcmp(pMenuData->PgmInputs, ""))
            fprintf(pfileProfile, "   PgmInputs: %s\n", pMenuData->PgmInputs);
        if(strcmp(pMenuData->IconFile, ""))
            fprintf(pfileProfile, "   IconFile: %s\n", pMenuData->IconFile);
                                        /* Write DOS Settings only if available */
        if(strlen(pMenuData->PgmDosSettings)!=0)
            {
            if((pMenuData->SessionType==PROG_VDM) ||
                (pMenuData->SessionType==PROG_WINDOWEDVDM) ||
                (pMenuData->SessionType==PROG_WINDOW_REAL) ||
                (pMenuData->SessionType==PROG_WINDOW_PROT) ||
                (pMenuData->SessionType==PROG_31_STDSEAMLESSVDM) ||
                (pMenuData->SessionType==PROG_31_STDSEAMLESSCOMMON) ||
                (pMenuData->SessionType==PROG_31_ENHSEAMLESSVDM) ||
                (pMenuData->SessionType==PROG_31_ENHSEAMLESSCOMMON) ||
                (pMenuData->SessionType==PROG_31_STD) ||
                (pMenuData->SessionType==PROG_31_ENH))
                {
                UCHAR   *pucBegin;
                UCHAR   *pucEnd;

                fprintf(pfileProfile, "    DOSSETTINGS BEGIN\n");
                for(pucBegin=pMenuData->PgmDosSettings, pucEnd=strstr(pucBegin, "\r\n");
                    pucEnd!=NULL;
                    pucBegin=pucEnd+2, pucEnd=strstr(pucBegin, "\r\n"))
                    {
                                        /* Terminate a single DOS setting entry for
                                           a moment, print it and restore previous
                                           character */
                    *pucEnd='\0';
                    fprintf(pfileProfile, "     %s\n", pucBegin);
                    *pucEnd='\r';
                    }
                fprintf(pfileProfile, "    DOSSETTINGS END\n");
                }
            }
                                        /* Write Environment settings only if available */
        if(strlen(pMenuData->PgmLeadingEnvironment)!=0)
            {
            if((pMenuData->SessionType==PROG_FULLSCREEN) ||
                (pMenuData->SessionType==PROG_WINDOWABLEVIO) ||
                (pMenuData->SessionType==PROG_PM) ||
                (pMenuData->SessionType==PROG_DETACHED))
                {
                UCHAR   *pucBegin;
                UCHAR   *pucEnd;

                fprintf(pfileProfile, "    LEADING ENVIRONMENT BEGIN\n");
                for(pucBegin=pMenuData->PgmLeadingEnvironment, pucEnd=strstr(pucBegin, "\r\n");
                    pucEnd!=NULL;
                    pucBegin=pucEnd+2, pucEnd=strstr(pucBegin, "\r\n"))
                    {
                                        /* Terminate a single environment entry for
                                           a moment, print it and restore previous
                                           character */
                    *pucEnd='\0';
                    fprintf(pfileProfile, "     %s\n", pucBegin);
                    *pucEnd='\r';
                    }
                fprintf(pfileProfile, "    ENVIRONMENT END\n");
                }
            }
        if(pMenuData->StatusFlag & STARTUPENVIRONMENTS)
            fprintf(pfileProfile, "   EnvControl: Startup\n");
        if(strcmp(pMenuData->EnvironmentSpaces, ""))
            {
            if((pMenuData->SessionType==PROG_FULLSCREEN) ||
                (pMenuData->SessionType==PROG_WINDOWABLEVIO) ||
                (pMenuData->SessionType==PROG_PM) ||
                (pMenuData->SessionType==PROG_DETACHED))
                {
                UCHAR   *pucBegin;
                UCHAR   *pucEnd;

                fprintf(pfileProfile, "    ENVIRONMENTSPACES BEGIN\n");
                for(pucBegin=pMenuData->EnvironmentSpaces, pucEnd=strstr(pucBegin, "\r\n");
                    pucEnd!=NULL;
                    pucBegin=pucEnd+2, pucEnd=strstr(pucBegin, "\r\n"))
                    {
                                        /* Terminate a single environment space for
                                           a moment, print it and restore previous
                                           character */
                    *pucEnd='\0';
                    fprintf(pfileProfile, "     %s\n", pucBegin);
                    *pucEnd='\r';
                    }
                fprintf(pfileProfile, "    ENVIRONMENTSPACES END\n");
                }
            }
        if(strlen(pMenuData->PgmTrailingEnvironment)!=0)
            {
            if((pMenuData->SessionType==PROG_FULLSCREEN) ||
                (pMenuData->SessionType==PROG_WINDOWABLEVIO) ||
                (pMenuData->SessionType==PROG_PM) ||
                (pMenuData->SessionType==PROG_DETACHED))
                {
                UCHAR   *pucBegin;
                UCHAR   *pucEnd;

                fprintf(pfileProfile, "    TRAILING ENVIRONMENT BEGIN\n");
                for(pucBegin=pMenuData->PgmTrailingEnvironment, pucEnd=strstr(pucBegin, "\r\n");
                    pucEnd!=NULL;
                    pucBegin=pucEnd+2, pucEnd=strstr(pucBegin, "\r\n"))
                    {
                                        /* Terminate a single environment entry for
                                           a moment, print it and restore previous
                                           character */
                    *pucEnd='\0';
                    fprintf(pfileProfile, "     %s\n", pucBegin);
                    *pucEnd='\r';
                    }
                fprintf(pfileProfile, "    ENVIRONMENT END\n");
                }
            }
        if(pMenuData->SessionType!=PROG_DEFAULT)
            {
            ULONG   ulTemp;

            fprintf(pfileProfile, "   SessionType: ");
            for(ulTemp=0; ulTemp<SESSIONTYPESIZE; ulTemp++)
                if(pMenuData->SessionType==SessionType[ulTemp].usSessionType)
                    {
                    fprintf(pfileProfile, "%s", SessionType[ulTemp].pucSessionType);
                    break;
                    }
            fprintf(pfileProfile, "\n");
            }
        if((pMenuData->PgmControl & ~(SSF_CONTROL_VISIBLE|SSF_CONTROL_AUTOSTARTED))!=0)
            {
            ULONG   ulTemp;

            fprintf(pfileProfile, "   PgmControl:");
            for(ulTemp=0; ulTemp<PGMCONTROLSIZE; ulTemp++)
                if(pMenuData->PgmControl&PgmControl[ulTemp].usPgmControl)
                    fprintf(pfileProfile, " %s", PgmControl[ulTemp].pucPgmControl);
            fprintf(pfileProfile, "\n");
            }
        if(pMenuData->AutostartDelay!=0)
            fprintf(pfileProfile, "   Autostartdelay: %ld\n", (LONG)pMenuData->AutostartDelay);
        if(pMenuData->FgBg & SSF_FGBG_BACK)
            fprintf(pfileProfile, "   FgBg: SSF_FGBG_BACK\n");
        if(pMenuData->PgmControl & SSF_CONTROL_SETPOS)
            {
            fprintf(pfileProfile, "   InitXPos: %ld\n", (LONG)pMenuData->InitXPos);
            fprintf(pfileProfile, "   InitYPos: %ld\n", (LONG)pMenuData->InitYPos);
            fprintf(pfileProfile, "   InitXSize: %lu\n", (ULONG)pMenuData->InitXSize);
            fprintf(pfileProfile, "   InitYSize: %lu\n", (ULONG)pMenuData->InitYSize);
            }
                                        /* Though we have added this window to the Sticky Windows,
                                           we still support that flag here, as it does not cost
                                           additional performance because we have to find the Window
                                           Name for the Hotkey and User defined Size & Position anyway
                                           and 1 additonal comparism doesn't hurt too much */
        if(pMenuData->SwpFlag & SWP_NOMOVE)
            fprintf(pfileProfile, "   WindowControl: SWP_NOMOVE\n");
        if((pMenuData->KeyData).usCh!=0)
            {                           /* KC_CTRL and/or KC_ALT may be set */
            fprintf(pfileProfile, "   KeyControl:");
            if((pMenuData->KeyData).usFlags&KC_RCTRL)
                fprintf(pfileProfile, " KC_RCTRL");
            if((pMenuData->KeyData).usFlags&KC_CTRL)
                fprintf(pfileProfile, " KC_CTRL");
            if((pMenuData->KeyData).usFlags&KC_ALT)
                fprintf(pfileProfile, " KC_ALT");
            if((pMenuData->KeyData).usFlags&KC_LWIN)
                fprintf(pfileProfile, " KC_LWIN");
            fprintf(pfileProfile, "\n");
            if(pMenuData->SwpFlag & SWP_MAXIMIZE)
                fprintf(pfileProfile, "   WindowControl: SWP_MAXIMIZE\n");
            if(pMenuData->SwpFlag & SWP_RESTORE)
                fprintf(pfileProfile, "   WindowControl: SWP_RESTORE\n");
            fprintf(pfileProfile, "   KeyData: %c\n", (UCHAR)(pMenuData->KeyData).usCh);
            }
        if((pMenuData->PriorityClass!=PRTYC_NOCHANGE) || (pMenuData->PriorityDelta!=PRTYC_NOCHANGE))
            {
            fprintf(pfileProfile, "   PriorityClass: %lu\n", (ULONG)pMenuData->PriorityClass);
            fprintf(pfileProfile, "   PriorityDelta: %ld\n", (ULONG)pMenuData->PriorityDelta);
            }
                                        /* See if the scheduler is configured */
        if(pMenuData->SDScheduler.ulSchedulerFlag&(~SCHEDULE_NONE))
            SaveSchedulerData(&pMenuData->SDScheduler, pfileProfile);
        }
    if(pMenuData->Item==ENTRYCONTROL)
        {
/*                                                                                      *\
 * If it is a CONTROL so write the header MENUCONTROL and then write the profile data   *
 * from the MENUDATA structure pointed by pMenuData.                                    *
\*                                                                                      */
        fprintf(pfileProfile, "  MENUCONTROL\n");
        fprintf(pfileProfile, "   PgmTitle: %s\n", pMenuData->PgmTitle);
                                        /* For some Menucontrols its allowed to use the Scheduler */
        if((pMenuData->id==ID_SHUTDOWN) || (pMenuData->id==ID_POWERDOWN) ||
            (pMenuData->id==ID_EXIT) || (pMenuData->id==ID_LOCKUP))
            {
                                        /* See if the scheduler is configured */
            if((pMenuData->SDScheduler.ulSchedulerFlag&(~SCHEDULE_NONE)) &&
                (pMenuData->SDScheduler.ulSchedulerFlag!=0))
                SaveSchedulerData(&pMenuData->SDScheduler, pfileProfile);
            }
        }
/*                                                                                      *\
 * If one is available, get the next element in the linked list, else we are at the end *
 * either at a leaf or at the real last element, in both cases shell back one level.    *
 * Shell back either exits this procedure completle (we have written the complete       *
 * linked list) or on level (we have written a complete submenu leaf).                  *
\*                                                                                      */
    if(pMenuData->Next!=NULL) pMenuData=pMenuData->Next;
    else break;
} while(TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * This recursive procedure traverses through the Popup Menu's linked list, removes and *
 * frees all menuitems from the list. The Popup Menu's icon will be freed too, but the  *
 * menuentry will not be removed from the Popup Menu itself, because its more efficient *
 * to delete a submenu with WinDestroyWindow() and FreeMenu() can be performed no in    *
 * the PM thread that created the Popup Menu.                                           *
 * Req:                                                                                 *
 *      pMenuData ..... A pointer to the first element to begin deletion with           *
 *      bCurrentOnly .  TRUE/FALSE Remove only current/Remove starting with current     *
 *                      menuitem                                                        *
 * Returns:                                                                             *
 *      none                                                                            *
\*--------------------------------------------------------------------------------------*/
void    FreeMenu(MENUDATA *pMenuData, BOOL bCurrentOnly)
{
MENUDATA    *pMenuDataCurrent=pMenuData;
MENUDATA    *pMenuDataNext;

                                        /* Do until the end of the current submenu level */
while(pMenuDataCurrent!=NULL)
    {
                                        /* Save the pointer to the next menuentry (which may be NULL
                                           of course) because the current one gets removed */
    pMenuDataNext=pMenuDataCurrent->Next;
                                        /* If the current menuentry is a submenu delete the submenu entries
                                           recursively before deleting the submenu itself */
    if(pMenuDataCurrent->Item==ENTRYSUBMENU)
        {
        FreeMenu(pMenuDataCurrent->Submenu, FALSE);
        pMenuDataCurrent->Submenu=NULL;
        }
                                        /* If the Setup Dialog is going to be removed, set the
                                           flag describing the existance of this dialog to false */
    if(!strcmp(pMenuDataCurrent->PgmTitle, CTRL_CONFIGMENU))
        pHP->ulRuntimeFlag|=DISPLAYCONFIGDIALOG;
                                        /* Free resources after having initialized storage to 0 */
    DeAllocateMenuData(pMenuDataCurrent);
                                        /* Advance to the next element */
    pMenuDataCurrent=pMenuDataNext;
                                        /* If only the current menuentry should be removed, ignore
                                           all following */
    if(bCurrentOnly==TRUE)
        break;
    }
}

/*--------------------------------------------------------------------------------------*\
 * This procedure loads the linked list of Environment Spaces from a file (usually      *
 * PC2.ENV)                                                                             *
 * Req:                                                                                 *
 *      pfileProfile .. File handle to load Environment Spaces from                     *
 * Returns:                                                                             *
 *      ENVIRONMENTLIST * ...                                                           *
 *                      Pointer to the first environment space loaded or NULL if none   *
 *                      could be read successfully.                                     *
\*--------------------------------------------------------------------------------------*/
ENVIRONMENTLIST  *LoadEnvironmentSpaces(FILE *pfileProfile)
{
UCHAR           *pucMatch;
ENVIRONMENTLIST *pEnvironmentListFirst=NULL;
ENVIRONMENTLIST *pEnvironmentListCurrent=NULL;

fgets(Buffer, sizeof(Buffer), pfileProfile);
                                        /* We don't exepect EOF yet */
if(feof(pfileProfile))
    return(pEnvironmentListFirst);
                                        /* The first line must be as expected to assume a PC/2
                                           Enviroment Spaces file */
if(!strstr(Buffer, "ENVIRONMENTPROFILE START"))
    return(pEnvironmentListFirst);
fgets(Buffer, sizeof(Buffer), pfileProfile);
do
    {
                                        /* We have read the profile */
    if(strstr(Buffer, "ENVIRONMENTPROFILE END"))
        break;
                                        /* There is a new environment space */
    if(strstr(Buffer, "ENVIRONMENT SPACE"))
        {
        do
            {
                                        /* Get next line */
            fgets(Buffer, sizeof(Buffer), pfileProfile);
                                        /* Replace CRLF with \0 */
            StripTrailing(Buffer);
                                        /* Find first space after a ':' or NULL, if
                                           none can be found */
            pucMatch=strchr(Buffer, ':');
            if(pucMatch==NULL)
                pucMatch=strchr(Buffer, '\0');
            else                        /* We found a ':', so we search for ' ' and
                                           return it or NULL, if none can be found */
                if((pucMatch=strchr(pucMatch, ' '))==NULL) pucMatch=strchr(Buffer, '\0');
                else for( ; (*pucMatch==' ') && (*pucMatch!='\0'); /* pucMatch++ */)
                    pucMatch++;
                                        /* Now fill in the characters after the
                                           space, according for what structure
                                           element it is given */
            if(strstr(Buffer, "Name:"))
                {
                                        /* Allocate a new element for linked list,
                                           if none exists create the first one otherwise
                                           add it after the current element */
                if(pEnvironmentListCurrent)
                    {
                    pEnvironmentListCurrent->pNext=malloc(sizeof(ENVIRONMENTLIST));
                    memset(pEnvironmentListCurrent->pNext, 0, sizeof(ENVIRONMENTLIST));
                                        /* Link in the newly created element */
                    pEnvironmentListCurrent->pNext->pBack=pEnvironmentListCurrent;
                    pEnvironmentListCurrent=pEnvironmentListCurrent->pNext;
                    pEnvironmentListCurrent->pEnvironment=strdup("");
                    }
                else
                    {
                    pEnvironmentListFirst=malloc(sizeof(ENVIRONMENTLIST));
                    memset(pEnvironmentListFirst, 0, sizeof(ENVIRONMENTLIST));
                    pEnvironmentListCurrent=pEnvironmentListFirst;
                    pEnvironmentListCurrent->pEnvironment=strdup("");
                    }
                pEnvironmentListCurrent->pEnvironmentName=malloc(strlen(pucMatch)+1);
                strcpy(pEnvironmentListCurrent->pEnvironmentName, pucMatch);
                continue;
                }
            if(strstr(Buffer, "VARIABLES BEGIN"))
                {
                ULONG   ulEnvSettingsLength;
                UCHAR   *pucEnvVar;

                fgets(Buffer, sizeof(Buffer), pfileProfile);
                while((!strstr(Buffer, "VARIABLES END")) &&
                    (!feof(pfileProfile)))
                    {
                    UCHAR   *pucTemp;

                                        /* Remove trailing \r and/or \n */
                    StripTrailing(Buffer);
                                        /* Remove leading spaces */
                    pucEnvVar=StripLeading(Buffer);
                                        /* Allocate space to add next Environment variable
                                           to the already existing (including \r\n\0) */
                    ulEnvSettingsLength=strlen(pEnvironmentListCurrent->pEnvironment)+strlen(pucEnvVar)+3;
                    pucTemp=malloc(ulEnvSettingsLength);
                    if(pucTemp!=NULL)
                        {
                        strcpy(pucTemp, pEnvironmentListCurrent->pEnvironment);
                        strcat(pucTemp, pucEnvVar);
                        strcat(pucTemp, "\r\n");
                        free(pEnvironmentListCurrent->pEnvironment);
                        pEnvironmentListCurrent->pEnvironment=pucTemp;
                        }
                    fgets(Buffer, sizeof(Buffer), pfileProfile);
                    }
                }
            } while((!strstr(Buffer, "ENVIRONMENT SPACE")) &&
                (!strstr(Buffer, "ENVIRONMENTPROFILE END")) &&
                (!feof(pfileProfile)));
        }
    } while(!feof(pfileProfile));
return(pEnvironmentListFirst);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure saves the linked list of Environment Spaces into a file (usually      *
 * PC2.ENV)                                                                             *
 * Req:                                                                                 *
 *      ENVIRONMENTLIST * ...                                                           *
 *                      Pointer to the first environment space to be saved or NULL if   *
 *                      none environment spaces are available.                          *
 *      pfileProfile .. File handle to write Environment Spaces into                    *
 *      ulAsBatch ..... TRUE/FALSE save a Batch (CMD)/Environment (ENV) file            *
 * Returns:                                                                             *
\*--------------------------------------------------------------------------------------*/
void             SaveEnvironmentSpaces(ENVIRONMENTLIST *pEnvironmentList, FILE *pfileProfile, ULONG ulAsBatch)
{
if(ulAsBatch==FALSE)
    fprintf(pfileProfile, "ENVIRONMENTPROFILE START\n");
                                        /* Loop through linked list */
for( ; pEnvironmentList; pEnvironmentList=pEnvironmentList->pNext)
    {
    if(ulAsBatch==FALSE)
        {
        fprintf(pfileProfile, "ENVIRONMENT SPACE\n");
        fprintf(pfileProfile, " Name: %s\n", pEnvironmentList->pEnvironmentName);
        }
    if(pEnvironmentList->pEnvironment)
        {
        UCHAR   *pucBegin;
        UCHAR   *pucEnd;

        if(ulAsBatch==FALSE)
            fprintf(pfileProfile, "  VARIABLES BEGIN\n");
                                        /* The required \n is already part of the string */
        for(pucBegin=pEnvironmentList->pEnvironment, pucEnd=strstr(pucBegin, "\r\n");
            pucEnd!=NULL;
            pucBegin=pucEnd+2, pucEnd=strstr(pucBegin, "\r\n"))
            {
                                        /* Terminate a single environment space for
                                           a moment, print it and restore previous
                                           character */
            *pucEnd='\0';
            if(ulAsBatch==FALSE)
                {
                fprintf(pfileProfile, "   %s\n", pucBegin);
                }
            else
                {
                if(strchr(pucBegin, '='))
                    fprintf(pfileProfile, "SET %s\n", pucBegin);
                else
                    fprintf(pfileProfile, "%s\n", pucBegin);
                }
            *pucEnd='\r';
            }
        if(ulAsBatch==FALSE)
            fprintf(pfileProfile, "  VARIABLES END\n");
        }
    }
if(ulAsBatch==FALSE)
    fprintf(pfileProfile, "ENVIRONMENTPROFILE END\n");
}

/*--------------------------------------------------------------------------------------*\
 * This procedure deallocates the linked list of environment spaces loaded by PC/2.     *
 * Req:                                                                                 *
 *      ENVIRONMENTLIST * ...                                                           *
 *                      Pointer to the first environment space to be deallocated        *
 * Returns:                                                                             *
\*--------------------------------------------------------------------------------------*/
void             FreeEnvironmentSpaces(ENVIRONMENTLIST *pEnvironmentList)
{
ENVIRONMENTLIST *pEnvironmentListCurrent=pEnvironmentList;
ENVIRONMENTLIST *pEnvironmentListNext;

                                        /* Do for whole linked list */
while(pEnvironmentListCurrent!=NULL)
    {
    pEnvironmentListNext=pEnvironmentListCurrent->pNext;

    free(pEnvironmentListCurrent->pEnvironmentName);
    free(pEnvironmentListCurrent->pEnvironment);
    free(pEnvironmentListCurrent);
    pEnvironmentListCurrent=pEnvironmentListNext;
    }
}

/*--------------------------------------------------------------------------------------*\
 * This recursive procedure searches through the Popup Menu's linked list for one or    *
 * more menuentries that should be scheduled now.                                       *
 * Req:                                                                                 *
 *      pMD ........... A pointer to the first element to search on                     *
 * Returns:                                                                             *
 *      MENUDATA * .... Pointer pMD                                                     *
\*--------------------------------------------------------------------------------------*/

#define MATCH_MINUTE                    0x00000001
#define MATCH_HOUR                      0x00000002
#define MATCH_WEEKDAY                   0x00000004
#define MATCH_DAY                       0x00000008
#define MATCH_MONTH                     0x00000010

#define MATCH_HOURLY                    (MATCH_MINUTE)
#define MATCH_DAILY                     (MATCH_MINUTE|MATCH_HOUR)
#define MATCH_WEEKLY                    (MATCH_MINUTE|MATCH_HOUR|MATCH_WEEKDAY)
#define MATCH_MONTHLY                   (MATCH_MINUTE|MATCH_HOUR|MATCH_DAY)
#define MATCH_ONCE                      (MATCH_MINUTE|MATCH_HOUR|MATCH_DAY|MATCH_MONTH)

MENUDATA        *ScheduleItem(MENUDATA *pMD, SCHEDULERDATA *pSDCurrent)
{
SCHEDULERDATA   *pSDMenuData;
ULONG           ulMatch;

while(pMD!=NULL)
    {
                                        /* If it is a menuentry or (one of the allowed) menucontrols, see
                                           if the Scheduler is enabled for this item, and if it is enable,
                                           see if it should be scheduled at the current time */
    if((pMD->Item==ENTRYMENUITEM) || (pMD->Item==ENTRYCONTROL))
        {
                                        /* Set to true when we found something to schedule */
        ULONG   ulSchedulerMatch=FALSE;

        pSDMenuData=&pMD->SDScheduler;
                                        /* Test which parts of the current date/time match with the
                                           scheduled date/time of the menuentry */
        ulMatch=0;
        if((pSDMenuData->ulStartingMinute1 & pSDCurrent->ulStartingMinute1) ||
            (pSDMenuData->ulStartingMinute2 & pSDCurrent->ulStartingMinute2))
            ulMatch|=MATCH_MINUTE;
        if(pSDMenuData->ulStartingHour & pSDCurrent->ulStartingHour)
            ulMatch|=MATCH_HOUR;
        if(pSDMenuData->ulStartingWeekday & pSDCurrent->ulStartingWeekday)
            ulMatch|=MATCH_WEEKDAY;
        if(pSDMenuData->ulStartingDay & pSDCurrent->ulStartingDay)
            ulMatch|=MATCH_DAY;
        if(pSDMenuData->ulStartingMonth & pSDCurrent->ulStartingMonth)
            ulMatch|=MATCH_MONTH;
                                        /* Now see if we have at least one match and if
                                           that match is sufficiently for us */
        while((ulMatch!=0) && (pSDMenuData->ulSchedulerFlag & (~SCHEDULE_NONE)))
            {
                                        /* Test for current minute */
            if((pSDMenuData->ulSchedulerFlag & SCHEDULE_HOURLY) &&
                ((ulMatch & MATCH_HOURLY)==MATCH_HOURLY))
                { ulSchedulerMatch=TRUE; break; }
                                        /* Same test for current hour */
            if((pSDMenuData->ulSchedulerFlag & SCHEDULE_DAILY) &&
                ((ulMatch & MATCH_DAILY)==MATCH_DAILY))
                { ulSchedulerMatch=TRUE; break; }
                                        /* Same test for current weekday */
            if((pSDMenuData->ulSchedulerFlag & SCHEDULE_WEEKLY) &&
                ((ulMatch & MATCH_WEEKLY)==MATCH_WEEKLY))
                { ulSchedulerMatch=TRUE; break; }
                                        /* Same test for current month */
            if((pSDMenuData->ulSchedulerFlag & SCHEDULE_MONTHLY) &&
                ((ulMatch & MATCH_MONTHLY)==MATCH_MONTHLY))
                { ulSchedulerMatch=TRUE; break; }
                                        /* Same test for single launches */
            if((pSDMenuData->ulSchedulerFlag & SCHEDULE_ONCE) &&
                ((ulMatch & MATCH_ONCE)==MATCH_ONCE))
                { ulSchedulerMatch=TRUE; break; }
                                        /* Exit while loop */
            break;
            }
        if(ulSchedulerMatch==TRUE)
            {
                                        /* If PC/2 should exit itself via its Scheduler, and its now
                                           time to schedule this, simulate that PC/2 exits as it would
                                           under shutdown OS/2 to avoid the verification dialog */
            if(pMD->id==ID_EXIT)
                pHP->ulRuntimeFlag|=QUITFROMSHUTDOWN;
                                        /* The PC/2 Scheduler simply invoke the menuentry as the user
                                           would have done from the Popup Menu, so no further code
                                           changes are required */
            WinPostMsg(pHP->hwndClient, WM_COMMAND, MPFROMSHORT(pMD->id), MPFROMSHORT(CMDSRC_OTHER));
            }
        }
                                        /* Shell into the Submenus */
    if(pMD->Item==ENTRYSUBMENU)
        ScheduleItem(pMD->Submenu, pSDCurrent);
                                        /* Keep on searching until the end of linked list */
    if(pMD->Next!=NULL)
        pMD=pMD->Next;
    else
        break;
    }
return(pMD);
}

/*--------------------------------------------------------------------------------------*\
 * This recursive procedure searches through the Popup Menu's linked list for an        *
 * element with a certain ID.                                                           *
 * Req:                                                                                 *
 *      pMD ........... A pointer to the first element to search on                     *
 *      pid ........... Pointer to the ID to search for (pointer because we don't want  *
 *                      to get a copy during recursion                                  *
 * Returns:                                                                             *
 *      MENUDATA * .... Pointer to match or NULL if not found                           *
\*--------------------------------------------------------------------------------------*/
MENUDATA        *SearchItem(MENUDATA *pMD, ULONG *pid)
{
MENUDATA    *pMDReturn=NULL;

while(pMD!=NULL)
    {
                                        /* If found, save the pointer of it, and break out
                                           of loop */
    if(pMD->id==*pid)
        { pMDReturn=pMD; break; }
                                        /* Shell into the Submenus, and break out of loop if
                                           found */
    if(pMD->Item==ENTRYSUBMENU)
        pMDReturn=SearchItem(pMD->Submenu, pid);
    if(pMDReturn!=NULL)
        break;
                                        /* Keep on searching until found or end of linked list */
    pMD=pMD->Next;
    }
return(pMDReturn);
}

/*--------------------------------------------------------------------------------------*\
 * This recursive procedure loops through the Popup Menu's linked list to set or unset  *
 * the MIS_OWNERDRAW flag in PM's MENUITEM structure.                                   *
 * Req:                                                                                 *
 *      pMD ........... A pointer to the first element to search on                     *
 *      bCurrentOnly .. Set (TRUE) if only the current MENUITEM should be changed, or   *
 *                      unset (FALSE) if the next MENUITEM should be changed too.       *
 *      bOwnerDraw .... Set (TRUE) if all MENUITEMs should be ownerdrawn to include the *
 *                      menuentry's icon or unset (FALSE) if PM should draw the Popup   *
 *                      Menu.                                                           *
 * Returns:                                                                             *
\*--------------------------------------------------------------------------------------*/
ULONG           SetItemStyle(MENUDATA *pMD, BOOL bCurrentOnly, BOOL bOwnerDraw)
{
MENUITEM    MenuItem;

while(pMD!=NULL)
    {
                                        /* Set or unset MENUITEM's MIS_OWNERDRAW style for
                                           the menuentry */
    WinSendMsg(pMD->hwndItem, MM_QUERYITEM, MPFROM2SHORT(pMD->id, FALSE), MPFROMP(&MenuItem));
        {
        if(bOwnerDraw)
            {                           /* Set MIS_OWNERDRAW flag where applicable. For unknown reasons
                                           MIS_TEXT must not be set while MIS_OWNERDRAW is set, or the
                                           menu size will not be calculated (and drawn) correctly. For
                                           me this is a but in PM, because first, this makes the handling
                                           of menus complicated and second this kills the chance for the
                                           menu to act on mnemonic keys (the underlined letters) */
            if(pMD->Item==ENTRYSUBMENU)
                MenuItem.afStyle=(MenuItem.afStyle|MIS_OWNERDRAW)&(~MIS_TEXT);
            if(pMD->Item==ENTRYMENUITEM)
                MenuItem.afStyle=(MenuItem.afStyle|MIS_OWNERDRAW)&(~MIS_TEXT);
            if(pMD->Item==ENTRYCONTROL)
                {
                if(strcmp(pMD->PgmTitle, CTRL_SEPARATOR))
                    MenuItem.afStyle=(MenuItem.afStyle|MIS_OWNERDRAW)&(~MIS_TEXT);
                }
            }
        else
            {                           /* Unset MIS_OWNERDRAW flag where applicable */
            if(pMD->Item==ENTRYSUBMENU)
                MenuItem.afStyle&=(~MIS_OWNERDRAW);
            if(pMD->Item==ENTRYMENUITEM)
                MenuItem.afStyle=(MenuItem.afStyle|MIS_TEXT)&(~MIS_OWNERDRAW);
            if(pMD->Item==ENTRYCONTROL)
                {
                if(strcmp(pMD->PgmTitle, CTRL_SEPARATOR))
                    MenuItem.afStyle=(MenuItem.afStyle|MIS_TEXT)&(~MIS_OWNERDRAW);
                }
            }
                                        /* Now we inform PM about the menuentry's new style. For
                                           some unknown (at least for me) reasons, PM does not
                                           remember the menuentry's text once the menuentry had
                                           the MIS_OWNERDRAW style, so we need to set it again */
        WinSendMsg(pMD->hwndItem, MM_SETITEM, MPFROM2SHORT(NULL, FALSE), MPFROMP(&MenuItem));
        WinSendMsg(pMD->hwndItem, MM_SETITEMTEXT, MPFROMSHORT(pMD->id), MPFROMP(pMD->PgmTitle));
        }
                                        /* Shell into the Submenus and change all MENUITEMs there */
    if(pMD->Item==ENTRYSUBMENU)
        SetItemStyle(pMD->Submenu, FALSE, bOwnerDraw);
                                        /* If we should just change a single MENUITEM then exit,
                                           otherwise continue with the next one */
    if(bCurrentOnly==TRUE)
        break;
                                        /* If next element is NULL, the while loop terminates anyway */
    pMD=pMD->Next;
    }
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure searches through the linked list of environments for a certain name   *
 * of a environment space.                                                              *
 * Req:                                                                                 *
 *      pucEnvironmentName ...                                                          *
 *                      Name of the environment space to find.                          *
 * Returns:                                                                             *
 *      ENVIRONMENTLIST * ...                                                           *
 *                      Pointer to the ENVIRONMENTLIST linked list entry that matches   *
 *                      with the environment space's name to search for or NULL if none *
 *                      can be found.                                                   *
\*--------------------------------------------------------------------------------------*/
ENVIRONMENTLIST *SearchEnvironment(UCHAR *pucEnvironmentName)
{
ENVIRONMENTLIST *pEnvironmentList=pHP->pEnvironmentSpaces;
                                        /* If not one environment space exits return NULL
if(!pEnvironmentList)
    return(NULL);
                                        /* Loop through list to find an environment space
                                           with the requested name */
for( ; pEnvironmentList->pNext!=NULL; pEnvironmentList=pEnvironmentList->pNext)
    {
    if(!strcmp(pEnvironmentList->pEnvironmentName, pucEnvironmentName))
        return(pEnvironmentList);
    }
                                        /* Didn't find it */
return(NULL);
}

/*--------------------------------------------------------------------------------------*\
 * This recursive procedure searches through the linked list for an element.            *
 * Req:                                                                                 *
 *      pMD ........... A pointer to the first element to search on                     *
 *      pWD ........... Pointer to the a WINDOWDATA structure containing the session    *
 *                      and titlebar title to compare with during recursion             *
 * Returns:                                                                             *
 *      MENUDATA * .... Pointer to match or NULL if not found                           *
\*--------------------------------------------------------------------------------------*/
MENUDATA        *SearchTitle(MENUDATA *pMD, WINDOWDATA *pWD)
{
MENUDATA    *pMDReturn=NULL;

while(pMD!=NULL)
    {
                                        /* If (a non NULL) string is found, save the pointer of match */
    if(strlen(pMD->WindowTitle))
        {
                                        /* To identify a PM window as a certain entry in PC/2's
                                           Popup-Menu, either the titlebar, the Window List entry
                                           or the classname must match case sensitive */
        if(strstr(pWD->ucPgmTitle, pMD->WindowTitle) ||
            strstr(pWD->ucWindowTitle, pMD->WindowTitle) ||
            strstr(pWD->ucClassName, pMD->WindowTitle))
            {
                                        /* Indicate we found and won't search further */
            pMDReturn=pMD;
            break;
            }
        }
                                        /* Shell into the Submenus, and break out of loop if
                                           found */
    if(pMD->Item==ENTRYSUBMENU)
        pMDReturn=SearchTitle(pMD->Submenu, pWD);
    if(pMDReturn!=NULL)
        break;
                                        /* Keep on searching until found or end of linked list */
    pMD=pMD->Next;
    }
return(pMDReturn);
}

/*--------------------------------------------------------------------------------------*\
 * This recursive procedure adds or removes Environment Spaces to or from the menuentry *
 * passed. If this menuentry is a Submenu, than we will apply the operation to all      *
 * menuentries of the Submenu too. Of course Environment Spaces apply only to certain   *
 * executables, that is OS/2 applications, and is ignored for all other.                *
 * Req:                                                                                 *
 *      pMenuData ..... A pointer to the first element to search on                     *
 *      pucEnvironmentSpaces                                                            *
 *         ............ The Environment Spaces to be added or removed in the form of    *
 *                      EnvironmentSpace1\r\n...EnvironmentSpacen\r\n\0                 *
 *      ulOperation ... The operation to be done (CDPB_ADDENVIRONMENT to add or         *
 *                      CDPB_REMOVEENVIRONMENT to remove Environment Spaces)            *
 * Returns:                                                                             *
 *      MENUDATA * .... Pointer passed (pMenuData)                                      *
\*--------------------------------------------------------------------------------------*/
MENUDATA        *ApplyEnvironment(MENUDATA *pMenuData, UCHAR *pucEnvironmentSpaces, ULONG ulOperation)
{
static ULONG    ulRecursion=0;

while(pMenuData!=NULL)
    {
                                        /* Break if pointer is invalid */
    if((pMenuData==NULL) || (pucEnvironmentSpaces==NULL))
        break;
                                        /* We can apply the operation only to OS/2 exeuctables */
    if(pMenuData->Item==ENTRYMENUITEM)
        {
        if((pMenuData->SessionType==PROG_FULLSCREEN) ||
            (pMenuData->SessionType==PROG_WINDOWABLEVIO) ||
            (pMenuData->SessionType==PROG_PM) ||
            (pMenuData->SessionType==PROG_DETACHED))
            {
                                        /* A single Environment Space to be added or removed */
            UCHAR   *pucSourceBegin=NULL;
            UCHAR   *pucSourceEnd=NULL;
                                        /* Match of the Environment Space for the executable */
            UCHAR   *pucMatch=NULL;
                                        /* Environment after applying operation */
            UCHAR   *pucEnvironmentNew=NULL;
            ULONG   ulLength=0;

                                        /* Loop through all single Environment Spaces we add
                                           to or remove from the executable */
            for(pucSourceBegin=pucEnvironmentSpaces;
                pucSourceBegin && *pucSourceBegin!='\0';
                pucSourceBegin=pucSourceEnd+2)
                {
                                        /* Find an Environment Space or break out if unsuccessfull */
                pucSourceEnd=strchr(pucSourceBegin, '\r');
                if(!pucSourceEnd)
                    break;
                                        /* Temporarily terminate this single Environment Space */
                *pucSourceEnd='\0';
                                        /* Let's see if the executable contains the current
                                           Environment Space */
                pucMatch=strstr(pMenuData->EnvironmentSpaces, pucSourceBegin);
                                        /* We add the Environment Space to the executable,
                                           if it does not already contain it */
                if((ulOperation==CDPB_ADDENVIRONMENT) &&
                    (pucMatch==NULL))
                    {
                    ulLength=strlen(pMenuData->EnvironmentSpaces);
                    ulLength+=strlen(pucSourceBegin)+3;
                    pucEnvironmentNew=malloc(ulLength);
                                        /* Source */
                    strcpy(pucEnvironmentNew, pMenuData->EnvironmentSpaces);
                                        /* Addition */
                    strcat(pucEnvironmentNew, pucSourceBegin);
                                        /* Terminate by \r\n\0 */
                    strcat(pucEnvironmentNew, "\r\n");
                    free(pMenuData->EnvironmentSpaces);
                    pMenuData->EnvironmentSpaces=pucEnvironmentNew;
                    }
                                        /* We remove the Environment Space from the executable,
                                           if does already contain it */
                if((ulOperation==CDPB_REMOVEENVIRONMENT) &&
                    (pucMatch!=NULL))
                    {
                                        /* Find the end of the matched Environment Space and
                                           replace the matched one with the next one by copying
                                           in place */
                    pucEnvironmentNew=strchr(pucMatch, '\r');
                                        /* If one follows, copy in place (this may be a problem
                                           if the removed Environment Space's length is smaller
                                           than the CPU databus size and using a highly optimizing
                                           compiler, but I don't expect this)*/
                    if(pucEnvironmentNew)
                        strcpy(pucMatch, pucEnvironmentNew+2);
                                        /* If nothing follows, simply terminate string (and
                                           neglect that we now don't use a few allocated space) */
                    else
                        pucMatch='\0';
                    }
                                        /* Restore temporary change */
                *pucSourceEnd='\r';
                }
            }
                                        /* If we're in the root just return, otherwise pass on
                                           to next menuentry within the current Submenu level */
        if(ulRecursion==0)
            break;
        }
                                        /* Shell into the Submenus */
    if(pMenuData->Item==ENTRYSUBMENU)
        {
        ulRecursion++;
        ApplyEnvironment(pMenuData->Submenu, pucEnvironmentSpaces, ulOperation);
        ulRecursion--;
        }
                                        /* Within a Submenu keep on searching all menuentries
                                           until the end of linked list and apply operation */
    if((ulRecursion>0) && (pMenuData->Next!=NULL))
        pMenuData=pMenuData->Next;
                                        /* We're either done or at the end of the linked list */
    else
        break;
    }
return(pMenuData);
}

/*--------------------------------------------------------------------------------------*\
 * This recursive procedure searches through the linked list for all applications that  *
 * have the autostart flag set and start it by simulating the applications selection    *
 * from the Popup-Menu. This procedure is called every 15 seconds to ensure that        *
 * delayed autostart items will be started too when the delay has expired.              *
 * Req:                                                                                 *
 *      pMD ........... A pointer to the first element to search on                     *
 *      pulAutostartCount                                                               *
 *                      Pointer to return the number of items still left to be Auto-    *
 *                      started (which when 0 allows to skip further looking for items  *
 *                      as there can't be some (PC/2 needs to be restarted or you have  *
 *                      to use the Scheduler)                                           *
 *      ulAutostartDelay                                                                *
 *                      The time in seconds PC/2 was launched                           *
 * Returns:                                                                             *
\*--------------------------------------------------------------------------------------*/
void    SearchAutoStartItem(MENUDATA *pMD, ULONG *pulAutostartCount, ULONG ulAutostartDelay)
{
while(pMD!=NULL)
    {
                                        /* If it is a Sub-Menu recurse into */
    if(pMD->Item==ENTRYSUBMENU)
        SearchAutoStartItem(pMD->Submenu, pulAutostartCount, ulAutostartDelay);
                                        /* If the autostart flag is set simulate selection
                                           from Popup-Menu by simulating its message. But
                                           ensure that this happens only once. And do count the
                                           items still left to be autostarted. */
    if((pMD->PgmControl & (SSF_CONTROL_AUTOSTART|SSF_CONTROL_AUTOSTARTED))==SSF_CONTROL_AUTOSTART)
        {
        if(pMD->AutostartDelay<=ulAutostartDelay)
            {
            pMD->PgmControl|=SSF_CONTROL_AUTOSTARTED;
                                        /* Why on the hell using WinPostMsg() does not work,
                                           that is DosStartSession() fails because its implicit
                                           call of DosExecPgm() is sometimes not able to load
                                           a statically linked DLL, even if the working directory
                                           is set up correctly, I don know.
                                           Even using WinPostMsg() and calling SearchAutoStartItem()
                                           from the main thread fails with the same symptom. By the
                                           way, it even doesn't fail consistent, sometimes it works
                                           and the failing sessions are not always the same, not
                                           always the same count of sessions started...very peculiar */
            WinPostMsg(pHP->hwndClient, WM_COMMAND,
                                        /* Get ID and set it as posted by a menu control */
                MPFROM2SHORT((USHORT)(pMD->id), CMDSRC_MENU),
                                        /* Simulate Message a result of a keyboard operation */
                MPFROMCHAR(FALSE));
            }
        else
            {
                                        /* Ok, we have something that wants to be autostarted, but
                                           hey time isn't ready yet so just count it */
            (*pulAutostartCount)++;

            }
        }
    if(pMD->Next!=NULL) pMD=pMD->Next;  /* Keep on searching until found end of linked list */
    else break;
    }
}

/*--------------------------------------------------------------------------------------*\
 * This recursive procedure searches through the linked list for all menuentries to be  *
 * added to a menu.                                                                     *
 * Req:                                                                                 *
 *      hwndMenu ...... PM menu window handle to insert menuentries according to the    *
 *                      linked list pMenuData                                           *
 *      pMenuData ..... A pointer to a linked list of menuentries to be inserted into   *
 *                      the hwndMenu menu window handle                                 *
 * Returns:                                                                             *
\*--------------------------------------------------------------------------------------*/
MENUDATA        *SetPMMenu(HWND hwndMenu, MENUDATA *pMenuData)
{
while(pMenuData!=NULL)
{
                                        /* If we have a submenu that does not contain a vaild item,
                                           the list contains an ENTRYEMPTY item, which we don't insert
                                           into the Popup-Menu */
    if(pMenuData->Item==ENTRYEMPTY)
        break;
                                        /* Insert this Menuitem at the end of the Popup-Menu */
    if(pMenuData->Back!=NULL)
        {                               /* This isn't the first item, insert after an
                                           existing item */
        if((pMenuData->Back)->Submenu==pMenuData)
                                        /* If this is the first item of a Submenu, then
                                           insert it as this */
            SetPopupMenu(hwndMenu, MM_INSERTITEMSUBMENU, MPFROMP(pMenuData), MPFROMP(pMenuData->Back));
        else
                                        /* Insert item after the existing item */
            SetPopupMenu(hwndMenu, MM_INSERTITEMMENUITEM, MPFROMP(pMenuData), MPFROMP(pMenuData->Back));
        }
    else                                /* This is the first item, insert at the end */
        SetPopupMenu(hwndMenu, MM_INSERTITEMMENUITEM, MPFROMP(pMenuData), MPFROMP(NULL));
                                        /* Shell into the Submenus */
    if(pMenuData->Item==ENTRYSUBMENU)
        {
        SetPMMenu(hwndMenu, pMenuData->Submenu);
        }
                                        /* Keep on searching until found or end of linked list */
    if(pMenuData->Next!=NULL)
        pMenuData=pMenuData->Next;
    else
        break;
}
return(pMenuData);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure adds/changes/query/removes an item to/from the Popup-Menu.            *
 * Req:                                                                                 *
 *      hwndMenu ...... Menu window handle to add/change/query/remove menuentries       *
 *      msg ........... What to do                                                      *
 *      mp1 ........... Parameter 1                                                     *
 *      mp2 ........... Parameter 2                                                     *
 * Returns:                                                                             *
 *      MRESULT ....... Returned value of function                                      *
\*--------------------------------------------------------------------------------------*/
MRESULT SetPopupMenu(HWND hwndMenu, ULONG msg, MPARAM mp1, MPARAM mp2)
{
MENUDATA        *pMDChange;             /* Pointer to menuentry to change within menu */
MENUDATA        *pMD;                   /* Pointer to menuentry where changed menuentry gets
                                           modified afterwards */
ULONG           id;                     /* ID of the menuentry pointed to bz pMD */
MENUITEM        miMI;                   /* Update menus with this structure */
HWND            hwndMenuAdjust;         /* Menu window handle to insert into */
HWND            hwndSubMenuAdjust;      /* Window handle of a submenu of a menu window */
MRESULT         mr;                     /* PM API result */
BOOL            bResult;

bResult=FALSE;
switch(msg)
{
/*                                                                                      *\
 * Syntax: MM_INSERTITEM(MENUITEM|SUBMENU), MENUDATA *pMDChange, MENUDATA *pMD          *
\*                                                                                      */
/*                                                                                      *\
 * Insert a Menuitem, a Submenu, Menuentry or Control, into a (Sub)menu, even if it is  *
 * empty.                                                                               *
\*                                                                                      */
case MM_INSERTITEMMENUITEM:
/*                                                                                      *\
 * Insert a Menuitem, a Submenu or Menuentry as the first child entry of a parent       *
 * Submenu.                                                                             *
\*                                                                                      */
case MM_INSERTITEMSUBMENU:
    pMDChange=PVOIDFROMMP(mp1);         /* Get pointer to MENUDATA structure to insert */
    pMD=PVOIDFROMMP(mp2);               /* Get pointer to MENUDATA structure insert afterwards */
    if(pMD==NULL)
        id=MIT_END;                     /* Insert at end */
    else if(!strcmp(pMD->PgmTitle, CTRL_BREAKSEPARATOR))
        id=pMD->Back->id;               /* If we insert after a break separator, we effectifely
                                           insert after the menu entry before the break separator,
                                           because the break separator is only a style */
    else
        id=pMD->id;                     /* Get id of menuentry insert afterwards */
                                        /* If the Configuration dialog is added set flag
                                           to true */
    if(pMDChange->id==ID_CONFIGDIALOG)
        pHP->ulRuntimeFlag&=(~DISPLAYCONFIGDIALOG);

/*                                                                                      *\
 * An item (Menuitem or Submenu) is to be inserted into the Popup-Menu, either after    *
 * a Menuitem or as the first item of a/the (Sub)menu.                                  *
\*                                                                                      */
    if(WinSendMsg(
        hwndMenu,
        MM_QUERYITEM,                   /* Query a menuitem */
        MPFROM2SHORT(id, TRUE),         /* Identifier, include submenus */
        (MPARAM)&miMI)==FALSE)          /* Into MENUITEM structure */
        miMI.hwndSubMenu=0;
                                        /* If we're going to insert a break separator, we don't
                                           insert a functional menu entry, but have to change the
                                           style of the menu we "insert" break separator afterwards */
    if(!strcmp(pMDChange->PgmTitle, CTRL_BREAKSEPARATOR))
        {
                                        /* Modify style */
        miMI.afStyle|=MIS_BREAKSEPARATOR;
        if(WinSendMsg(
            hwndMenu,
            MM_SETITEM,                 /* Set a menuitem's attribute */
                                        /* Identifier, include submenus */
            MPFROM2SHORT(miMI.id, TRUE),
                                        /* Attributes to set */
            (MPARAM)&miMI)==FALSE)
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_POPUPMENU, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Can't modify PC/2's Popup Menu - continuing...");
                                        /* Save the menu window handle the item belongs to */
        pMDChange->hwndItem=pMDChange->Back->hwndItem;
        break;
        }
                                        /* If the item after we insert is a Submenu, then
                                           use the Submenu handle to insert new items,
                                           otherwise use the handle of the previous item */
    if((miMI.hwndSubMenu!=0) && (msg==MM_INSERTITEMSUBMENU))
        {
        hwndMenuAdjust=miMI.hwndSubMenu;
        id=MIT_END;
        }
    if(msg==MM_INSERTITEMMENUITEM)
        {                               /* If this is the first item, use the Popup-Menu
                                           window handle */
        if(pMDChange->Back==NULL) hwndMenuAdjust=hwndMenu;
                                        /* If we insert after an available item, get it's
                                           window handle */
        else hwndMenuAdjust=(pMDChange->Back)->hwndItem;
        }
                                        /* If previous exists, insert after the item with
                                           ID id */
    if(id!=(ULONG)MIT_END) miMI.iPosition++;
    else miMI.iPosition=id;             /* Insert at end MIT_END */
    miMI.afAttribute=0;                 /* Special attribute */
    miMI.id=pMDChange->id;              /* Item identifier */
    miMI.hItem=0;                       /* No handle */
                                        /* Insert the item as a submenu (if the to be inserted item
                                           is a Submenu) or as a menuitem (if the to be inserted item
                                           is not a Submenu) */
    miMI.hwndSubMenu=pMDChange->hwndItem;
    if((pMDChange->Item==ENTRYSUBMENU) && (miMI.hwndSubMenu==NULLHANDLE))
        {                               /* If we insert a Submenu, than we need to obtain a handle to
                                           create one, if it not already exists */
        hwndSubMenuAdjust=WinCreateMenu(/* Create a submenu menuitem */
            hwndMenuAdjust,             /* Owner- and parent-window handle */
            NULL);                      /* Binary menu template */
        miMI.afStyle=MIS_SUBMENU;       /* Style to insert */
        if(pHP->ulStatusFlag2 & POPUPICON)
            miMI.afStyle|=MIS_OWNERDRAW;
                                        /* Submenu within menuentry */
        miMI.hwndSubMenu=hwndSubMenuAdjust;
        }
    if(pMDChange->Item==ENTRYMENUITEM)
        {                               /* We insert a Menuitem */
                                        /* Style to insert */
        if(pHP->ulStatusFlag2 & POPUPICON)
            miMI.afStyle=MIS_OWNERDRAW;
        else
            miMI.afStyle=MIS_TEXT;
        miMI.hwndSubMenu=0;             /* No pulldown menu */
        }
    if(pMDChange->Item==ENTRYCONTROL)
        {                               /* We insert a Control */
        if(!strcmp(pMDChange->PgmTitle, CTRL_SEPARATOR))
            miMI.afStyle=MIS_SEPARATOR; /* For a seperator insert a separator */
        else
            {
                                        /* Style to insert */
            if(pHP->ulStatusFlag2 & POPUPICON)
                miMI.afStyle=MIS_OWNERDRAW;
            else
                miMI.afStyle=MIS_TEXT;
            }
        miMI.hwndSubMenu=0;
        }
    pMDChange->hwndItem=hwndMenuAdjust; /* Save the window handle of the item */
    mr=WinSendMsg(
        hwndMenuAdjust,
        MM_INSERTITEM,                  /* Insert a menu item */
        &miMI,                          /* Item to insert */
        pMDChange->PgmTitle);           /* Text to insert */
    if(((LONG)mr==MIT_ERROR) || ((LONG)mr==MIT_MEMERROR))
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_POPUPMENU, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Can't modify PC/2's Popup Menu - continuing...");
    else bResult=TRUE;
    break;

/*                                                                                      *\
 * Syntax: MM_MOVEMENUITEM, MENUDATA *pMDSource, MENUDATA *pMDDestination               *
\*                                                                                      */
case MM_MOVEMENUITEM:
/*                                                                                      *\
 * Move a MENUITEM structure with idSource after the idDestination.                     *
\*                                                                                      */
    {
    MENUDATA    *pMDSource;
    MENUDATA    *pMDDestination;
    ULONG       idSource;               /* Id of Menuitem to be moved */
    ULONG       idDestination;          /* Id of Menuitem after which the removed Menuitem
                                           will be inserted */
    MENUITEM    miSource;               /* MENUITEM structure of to be moved Menuitem */
    MENUITEM    miDestination;          /* MENUITEM structure of Menuitem after which
                                           the removed Menuitem will be inserted */

    pMDSource=PVOIDFROMMP(mp1);
    pMDDestination=PVOIDFROMMP(mp2);
    idSource=pMDSource->id;             /* Get id of to be removed Menuitem */
                                        /* Query all (Sub)menus for to be moved Menuitem */
    WinSendMsg(hwndMenu, MM_QUERYITEM,
        MPFROM2SHORT(idSource, TRUE), (MPARAM)&miSource);
                                        /* Delete the to be moved Menuitem. Don't use MM_DELETEITEM
                                           because it frees all OS/2 internal structures,
                                           whereas MM_REMOVEITEM doesn't free them */
    WinSendMsg(hwndMenu, MM_REMOVEITEM,
        MPFROM2SHORT(idSource, TRUE), (MPARAM)NULL);
                                        /* If a menuitem has to be moved after a break separator
                                           move it after previous menu entry, because a break
                                           separator is just a style of this entry */
    if(pMDDestination==NULL)
        {                               /* If the destination isn't available, insert it at the
                                           first position of a (Sub)menu */
        miSource.iPosition=0;
        mr=WinSendMsg(hwndMenu, MM_INSERTITEM, &miSource, pMDSource->PgmTitle);
        break;
        }
    if(!strcmp(pMDDestination->PgmTitle, CTRL_BREAKSEPARATOR))
        idDestination=pMDDestination->Back->id;
    else
                                    /* Get id of Menuitem after which removed Menuitem
                                       will be inserted */
        idDestination=pMDDestination->id;
                                        /* If the source and destination Menuitem are elements
                                           of the same level then they have the same item handle */
    if(pMDSource->hwndItem==pMDDestination->hwndItem)
        bResult=TRUE;
    else
        bResult=FALSE;
                                        /* Query all (Sub)menus for Menuitem after which
                                           the removed Menuitem will be inserted */
    WinSendMsg(hwndMenu, MM_QUERYITEM,
        MPFROM2SHORT(idDestination, TRUE), (MPARAM)&miDestination);

    if(bResult==TRUE)
        {                               /* If the destination of the Source Menuitem follows
                                           any previous Menuitem in the same level, just
                                           insert it one position behind */
        miSource.iPosition=++miDestination.iPosition;
        hwndMenuAdjust=pMDDestination->hwndItem;
        mr=WinSendMsg(hwndMenuAdjust, MM_INSERTITEM, &miSource, pMDSource->PgmTitle);
        }
    else
        {                               /* If the destination of the source Menuitem is the
                                           first position of a Submenu, insert it at 0-based
                                           position 0 */
        hwndMenuAdjust=miDestination.hwndSubMenu;
        miSource.iPosition=0;
        mr=WinSendMsg(hwndMenuAdjust, MM_INSERTITEM, &miSource, pMDSource->PgmTitle);
        }
    }
    break;

/*                                                                                      *\
 * Syntax: MM_SETITEMTEXT, MENUDATA *pMDChange, ULONG id                                *
\*                                                                                      */
case MM_SETITEMTEXT:
    pMDChange=PVOIDFROMMP(mp1);         /* Get pointer to MENUDATA structure to update */
    id=LONGFROMMP(mp2);                 /* Get id to update */
/*                                                                                      *\
 * A available menuitem was selected to change. Change the text of the menuitem to the  *
 * new one.                                                                             *
\*                                                                                      */
    if(WinSendMsg(
        hwndMenu,
        MM_SETITEMTEXT,                 /* Set the text of a menuitem */
        MPFROMSHORT(id),                /* Item ID */
                                        /* New menuitem text */
        (MPARAM)pMDChange->PgmTitle)==FALSE)
        {
                                        /* MM_SETITEMTEXT will fail with error 2101 (PMERR_SETID_IN_USE)
                                           when the menuitem has style MIS_OWNERDRAW for some unknown
                                           reason (I think this is a bad design of PM, because if setting
                                           the text fails, the menu is not able to react on mnemonics too). */
        if(!pHP->ulStatusFlag2 & POPUPICON)
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_POPUPMENU, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Can't modify PC/2's Popup Menu - continuing...");
        }
    else bResult=TRUE;
    break;

/*                                                                                      *\
 * Syntax: MM_DELETEITEM, MENUDATA *pMDChange, NULL                                     *
 * Syntax: MM_REMOVEITEM, MENUDATA *pMDChange, NULL                                     *
\*                                                                                      */
case MM_DELETEITEM:
                                        /* Delete item from menu, include deletion of bitmaps,... */
case MM_REMOVEITEM:
                                        /* Remove item from menu, without deleting it */
    pMDChange=PVOIDFROMMP(mp1);         /* Get pointer to MENUDATA structure to delete */
                                        /* Now unload the menuentry's icon as we destroy the menuentry
                                           it is referring to */
    if(pHP->ulStatusFlag2 & POPUPICON)
        InitializeIcon(pMDChange, FALSE);
                                        /* If we delete a break separator, we effectifely
                                           delete the style of the previous menu entry */
                                        /* Get id to delete */
    if(!strcmp(pMDChange->PgmTitle, CTRL_BREAKSEPARATOR))
        id=pMDChange->Back->id;
    else
        id=pMDChange->id;
/*                                                                                      *\
 * A available menuitem was selected to delete. Delete the specified menuitem.          *
\*                                                                                      */
                                        /* If we're going to delete a break separator, we don't
                                           insert a functional menu entry, but have to change the
                                           style of the menu we "delete" break separator afterwards */
    if(!strcmp(pMDChange->PgmTitle, CTRL_BREAKSEPARATOR))
        {
        if(WinSendMsg(
            hwndMenu,
            MM_QUERYITEM,               /* Query a menuitem */
            MPFROM2SHORT(id, TRUE),     /* Identifier, include submenus */
            (MPARAM)&miMI)==FALSE)      /* Into MENUITEM structure */
            miMI.hwndSubMenu=0;
                                        /* Modify style */
        miMI.afStyle&=(~MIS_BREAKSEPARATOR);
        if(WinSendMsg(
            hwndMenu,
            MM_SETITEM,                 /* Set a menuitem's attribute */
                                        /* Identifier, include submenus */
            MPFROM2SHORT(pMDChange->id, TRUE),
                                        /* Attributes to set */
            (MPARAM)&miMI)==FALSE)
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_POPUPMENU, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Can't modify PC/2's Popup Menu - continuing...");
        break;
        }
    if(msg==MM_REMOVEITEM)
        {
                                        /* If we are removing a menuitem into our clipboard,
                                           save the Submenu window handle (which will be 0
                                           if the menuitem is not a Submenu */
        WinSendMsg(hwndMenu, MM_QUERYITEM, MPFROM2SHORT(id, TRUE), (MPARAM)&miMI);
        pMDChange->hwndItem=miMI.hwndSubMenu;
        }
    mr=WinSendMsg(
        hwndMenu,
        msg,                            /* Delete or remove a menuitem */
        MPFROM2SHORT(id, TRUE),         /* Item ID, include Submenus */
        (MPARAM)NULL);
    bResult=TRUE;
    break;
}
return(MPFROMSHORT(bResult));
}

/*--------------------------------------------------------------------------------------*\
 * This procedure handles to copy a fully qualified path & filename into the corres-    *
 * ponding entryfields of the Program Installation dialog.                              *
 * Req:                                                                                 *
 *      hwndDlg ....... handle of Program installation dialog                           *
 *      pucFullFileName fully qualified path & filename of application to add           *
 *                      the name of an object to add                                    *
 *      bObject ....... TRUE if it is an WPS object                                     *
\*--------------------------------------------------------------------------------------*/
void InstallFilename2Dialog(HWND hwndDlg, UCHAR *pucFullFileName, BOOL bObject)
{
UCHAR   ucBuffer[CCHMAXPATH+6];         /* Longer than 260 because of "s */
UCHAR   ucFullFileName[CCHMAXPATHCOMP];
UCHAR   *pucMatch;
BOOL    bBatchFile=FALSE;
ULONG   ulAppType;                      /* Type of application we're installing */
USHORT  usSessionType=PROG_DEFAULT;     /* Session type (assuming default PROG_DEFAULT) */

if(bObject==TRUE)
    {
    usSessionType=PROG_WPSOBJECT;       /* It is an WPS object */
                                        /* Set title and object name info entryfields */
    if(!WinQueryWindowTextLength(WinWindowFromID(hwndDlg, PIEF_PROGRAMTITLE)))
        {
        if((pucMatch=strrchr(pucFullFileName, '\\'))!=NULL)
            WinSetDlgItemText(hwndDlg, PIEF_PROGRAMTITLE, ++pucMatch);
        else
            WinSetDlgItemText(hwndDlg, PIEF_PROGRAMTITLE, pucFullFileName);
        }
    WinSetDlgItemText(hwndDlg, PIEF_PATHFILENAME, pucFullFileName);
    WinSetDlgItemText(hwndDlg, PIEF_PARAMETERS, "");
    WinSetDlgItemText(hwndDlg, PIEF_DIRECTORY, "");
    }
else
    {                                   /* It is a file */
                                        /* Get the type of application */
    DosQueryAppType(pucFullFileName, &ulAppType);
    if((ulAppType & 0x7)==FAPPTYP_WINDOWAPI) usSessionType=PROG_PM;
    if((ulAppType & 0x7)==FAPPTYP_NOTWINDOWCOMPAT) usSessionType=PROG_FULLSCREEN;
    if((ulAppType & 0x7)==FAPPTYP_WINDOWCOMPAT) usSessionType=PROG_WINDOWABLEVIO;
    if(ulAppType & FAPPTYP_DOS) usSessionType=PROG_WINDOWEDVDM;
    if(ulAppType & FAPPTYP_WINDOWSREAL) usSessionType=PROG_WINDOW_REAL;
    if(ulAppType & FAPPTYP_WINDOWSPROT) usSessionType=PROG_31_STDSEAMLESSCOMMON;
    if(ulAppType & FAPPTYP_WINDOWSPROT31) usSessionType=PROG_31_ENHSEAMLESSCOMMON;
    }
if(bObject==FALSE)
    {
                                        /* Convert to uppercase to test for .CMD extension */
    strcpy(ucFullFileName, pucFullFileName);
    strupr(ucFullFileName);
                                        /* Now test for a OS/2 batch file */
    if(strstr(ucFullFileName, ".CMD")!=NULL)
        {
        usSessionType=PROG_WINDOWABLEVIO;
        bBatchFile=TRUE;
        if(strchr(pucFullFileName, ' ')!=NULL)
            {                           /* If path and filename contains spaces, insert
                                           two quotation marks */
            strcpy(ucBuffer, "/c \"\"");
            strcat(ucBuffer, pucFullFileName);
            strcat(ucBuffer, "\"\"");
            }
        else
            {                           /* Else add just /c to [path]filename.cmd */
            strcpy(ucBuffer, "/c ");
            strcat(ucBuffer, pucFullFileName);
            }
        }
                                        /* Now test for a DOS batch file */
    if(strstr(ucFullFileName, ".BAT")!=NULL)
        {
        usSessionType=PROG_WINDOWEDVDM;
        bBatchFile=TRUE;
        strcpy(ucBuffer, "/c ");        /* Add just /c to [path]filename.cmd */
        strcat(ucBuffer, pucFullFileName);
        }
    if(bBatchFile==TRUE)
        {                               /* Set batchfile as parameter and empty path & filename */
        WinSetDlgItemText(hwndDlg, PIEF_PARAMETERS, ucBuffer);
        WinSetDlgItemText(hwndDlg, PIEF_PATHFILENAME, "");
        }
    else
        {                               /* Set full qualified path and empty parameters */
        WinSetDlgItemText(hwndDlg, PIEF_PATHFILENAME, pucFullFileName);
        WinSetDlgItemText(hwndDlg, PIEF_PARAMETERS, "");
        }
    strcpy(ucBuffer, pucFullFileName);  /* Save full path & filename */
                                        /* Extract filename */
    pucMatch=pucFullFileName+strlen(pucFullFileName);
    for( ; (*pucMatch!='\\') && (pucMatch>=pucFullFileName); /* pucMatch-- */)
        pucMatch--;
                                        /* Set filename */
    if(!WinQueryWindowTextLength(WinWindowFromID(hwndDlg, PIEF_PROGRAMTITLE)))
        WinSetDlgItemText(hwndDlg, PIEF_PROGRAMTITLE, (pucMatch+1));
    *pucMatch='\0';                     /* Get path as working directory */
                                        /* Set working directory */
    WinSetDlgItemText(hwndDlg, PIEF_DIRECTORY, pucFullFileName);
    }
                                        /* Reflect the application type with the Program
                                           Type radiobuttons */
WinSendMsg(hwndDlg, WM_SETUPPROGRAMTYPE,
    MPFROMSHORT(usSessionType), (MPARAM)NULL);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure disables or enables child windows of a dialog window according to the *
 * bDisable flag.                                                                       *
 * Req:                                                                                 *
 *      hwndDlg ....... handle of Program installation dialog                           *
 *      usDialogIDs ... array of IDs of the child windows of a dialog                   *
 *      usItemCount ... number of IDs in the array                                      *
 *      ulStyle ....... WS_VISIBLE | WS_DISABLED or not                                 *
\*--------------------------------------------------------------------------------------*/
void    DisableDialogItem(HWND hwndDlg, USHORT usDialogIDs[], USHORT usItemCount, ULONG ulStyle)
{
USHORT  usTemp;

if(hwndDlg==NULLHANDLE)
    return;
if(ulStyle & WS_DISABLED)
                                        /* Enumerate and disable all child windows */
    for(usTemp=0; usTemp<usItemCount; usTemp++)
        WinEnableWindow(WinWindowFromID(hwndDlg, usDialogIDs[usTemp]), FALSE);
else
                                        /* Enumerate and enable all child windows */
    for(usTemp=0; usTemp<usItemCount; usTemp++)
        WinEnableWindow(WinWindowFromID(hwndDlg, usDialogIDs[usTemp]), TRUE);
if(ulStyle & WS_VISIBLE)
                                        /* Enumerate and show all child windows */
    for(usTemp=0; usTemp<usItemCount; usTemp++)
        WinShowWindow(WinWindowFromID(hwndDlg, usDialogIDs[usTemp]), TRUE);
else
                                        /* Enumerate and hide all child windows */
    for(usTemp=0; usTemp<usItemCount; usTemp++)
        WinShowWindow(WinWindowFromID(hwndDlg, usDialogIDs[usTemp]), FALSE);
}

static char APPLICATIONNAMEPC2[]=       "PC/2 Desktop Configuration";
static char APPLICATIONNAMEPC2SPOOLER[]="PC/2 Spooler Configuration";
static char APPLICATIONNAMEPC2SESSIONBAR[]="PC/2 SessionBar Configuration";
static char APPLICATIONNAMEPC2STATUS[]= "PC/2 Statuswindow Configuration";
static char PC2VERSION[]=               "PC/2 Version";
static char STATUSFLAG1[]=              "Desktop Status";
static char STATUSFLAG2[]=              "Desktop Status 2";
static char STATUSFLAG3[]=              "Desktop Status 3";
static char OVERVIEWDRAWMODE[]=         "Overview Draw Mode";
static char OVERVIEWGRADIENTSTART[]=    "Overview Gradient Start";
static char OVERVIEWGRADIENTEND[]=      "Overview Gradient End";
static char OVERVIEWSTICKYWINDOWS[]=    "Overview Sticky Windows";
static char TITLEBARFLAG[]=             "Titlebar Flag";
static char SMARTICONSFLAG[]=           "SmartIcons Flag";
static char POPUPKEYFLAG[]=             "Popup Key Flag";
static char DYNAMICOVERVIEWDELAY[]=     "Dynamic Overview Delay";
static char DYNAMICOVERVIEWFLAG[]=      "Dynamic Overview";
static char SLIDINGFOCUSDELAY[]=        "Sliding Focus Delay";
static char DYNAMICMENUDELAY[]=         "Dynamic Menuselection Delay";
static char DYNAMICSESSIONBARDELAY[]=   "Dynamic SessionBar Delay";
static char VIRTUALDESKTOPDELAY[]=      "Virtual Desktop Delay";
static char DEBUGFLAG[]=                "Debug Flag";
static char SCROLLPERCENTAGE[]=         "Scroll Percentage";
static char CLICKFLAG[]=                "Click Flag";
static char XPOSITION[]=                "X Position";
static char YPOSITION[]=                "Y Position";
static char XSIZE[]=                    "X Size";
static char YSIZE[]=                    "Y Size";
static char SPLITBARPOSITION[]=         "Splitbar Position";
static char XDESKTOPCOUNT[]=            "X Desktop Count";
static char YDESKTOPCOUNT[]=            "Y Desktop Count";
static char BACKGROUNDBITMAPFLAG[]=     "Background Bitmap Flag";
static char DESKTOPNAME[]=              "Desktop Name";
static char NLSDESKTOP[]=               "Desktop";
static char WINDOWLISTNAME[]=           "Window List Name";
static char NLSWINDOWLIST[]=            "Window List";
static char BACKGROUNDBITMAPNAME[]=     "Background Bitmap";
static char OS2LOGOBMP[]=               "\\OS2\\BITMAP\\OS2LOGO.BMP";
static char HELV8PTS[]=                 "8.Helv";
static char WINDOWFONTNAME[]=           "Window Font";
static char QUICKSWITCHFONTNAME[]=      "QuickSwitch Font";
static char SESSIONBARFONTNAME[]=       "SessionBar Font";
static char POPUPMENUFONTNAME[]=        "Popup-Menu Font";
static char SPOOLERFONTNAME[]=          "Spooler Font";
static char ACCESSPASSWORD[]=           "Access Password";
static char LOCKUPPASSWORD[]=           "Lockup Password";
static char TITLEBAREXCEPTION[]=        "Titlebar Exception";
static char HWVIRTUALDESKTOPEXCEPTION[]="Panning Exception";
static char STICKYWINDOWS[]=            "Sticky Windows";
static char LOCKUPTIMER[]=              "Lockup Timer";
static char POWERDOWNTIMER[]=           "PowerDown Timer";
static char ICONSCALE[]=                "Icon Scale";
static char SESSIONBARFLAG[]=           "SessionBar Status";
static char SESSIONBARSTATUSAREA[]=     "SessionBar Statusarea";
static char SESSIONBARDATETIME[]=       "SessionBar Date/Time";
static char SESSIONBARSCALE[]=          "SessionBar Scale";
static char SESSIONBARHOTSPOT[]=        "SessionBar HotSpot";
static char SESSIONBAREXCEPTION[]=      "SessionBar Exception";
static char HWSCREENSIZEX[]=            "HW Screen X Size";
static char HWSCREENSIZEY[]=            "HW Screen Y Size";
static char COLORBGND[]=                "Background Color";
static char COLORWND[]=                 "Window Color";
static char COLORFGND[]=                "Foreground Color";
static char COLORINTR[]=                "Interrupt Color";
static char COLORBUSY[]=                "Busy Color";
static char COLORGRID[]=                "Grid Color";
static char COLORCPU[]=                 "Status Background Color";
static char BACKUPCOUNTER[]=            "Backup Counter";
static char CPUPERFORMANCEFLAG[]=       "CPU Performance Flag";
static char REGISTRATIONFLAG[]=         "Online Registration Flag";
static char REGISTERFIRSTNAME[]=        "First Name";
static char REGISTERLASTNAME[]=         "Last Name";
static char REGISTEREMAILADDRESS[]=     "E-Mail";
static char PROXYURL[]=                 "Proxy URL";
static char UPDATEURL[]=                "Update URL";
static char HTTPUSERID[]=               "HTTP Userid";
static char HTTPPASSWORD[]=             "HTTP Password";
static char LASTPC2TRAP[]=              "PC2.TRP";

/*--------------------------------------------------------------------------------------*\
 * This procedure accesses the PC2.INI configuration file.                              *
 * Req:                                                                                 *
 *      pucFilenameINI. pointer to path of PC2.INI                                      *
 *      bRead ......... TRUE/FALSE if read/write from/to PC2.INI                        *
 * Ref:                                                                                 *
 *      HookParameters. read/write from/to PC2.INI                                      *
 * Ret:                                                                                 *
 *      NULL .......... Error occured, otherwise no error occured                       *
\*--------------------------------------------------------------------------------------*/
ULONG   INIAccess(UCHAR *pucFilenameINI, BOOL bRead)
{
HINI    hiniPC2INI;
UCHAR   *pucTemp;
ULONG   ulTemp;
UCHAR   ucPC2Version[sizeof(BLDLEVEL_VERSION)];
ULONG   ulPC2Update=FALSE;
ULONG   ulRc=(ULONG)TRUE;

                                        /* Open PC2.INI */
hiniPC2INI=PrfOpenProfile(pHP->habPc2, pucFilenameINI);
if(hiniPC2INI!=NULLHANDLE)
    {                                   /* Only try to access a valid PC2.INI */
    if(bRead==TRUE)
        {
        ulTemp=sizeof(ULONG);
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, STATUSFLAG1,
            &pHP->ulStatusFlag1, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulStatusFlag1=VIRTUALDESKTOP|CLICK2MOVE|OVERVIEW|BUTTON2ZORDER|NORMALSHUTDOWN|
                SELECTCENTERITEM|BUTTON2MOVE|OVERVIEWTITLES|OVERVIEW3DBORDER|OVERVIEWCURSOR;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, STATUSFLAG2,
            &pHP->ulStatusFlag2, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulStatusFlag2=POWERDOWNPOWEROFF|ADVANCEDMARKING|ADVMARKINGBUTTON2|DYNAMICMENU|SHOWSESSIONBAR;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, STATUSFLAG3,
            &pHP->ulStatusFlag3, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulStatusFlag3=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, REGISTRATIONFLAG,
            &pHP->ulRegistrationFlag, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulRegistrationFlag=REGISTER_ONLINE;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, OVERVIEWDRAWMODE,
            &pHP->ulOverviewWindowDrawMode, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulOverviewWindowDrawMode=OVERVIEW_GRADIENT;
            pHP->ulStatusFlag1|=OVERVIEWAUTOCOLOR;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, OVERVIEWGRADIENTSTART,
            &pHP->ulOverviewGradientStart, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulOverviewGradientStart=RGB_BLACK;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, OVERVIEWGRADIENTEND,
            &pHP->ulOverviewGradientEnd, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulOverviewGradientEnd=RGB_BLUE;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, TITLEBARFLAG,
            &pHP->ulTitlebarFlag, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulTitlebarFlag=(FL_CLOSEWINDOW|FL_ROLLWINDOW|FL_HELP|FL_MOVEVD|FL_MOVEEDGE);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, SMARTICONSFLAG,
            &pHP->ulOVSmartIconsFlag, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulOVSmartIconsFlag=0xFFFFFFFF;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, POPUPKEYFLAG,
            &pHP->ulPopupKeyFlag, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulPopupKeyFlag=POPUPLEFTRIGHTSHIFT|POPUPLEFTCTRL|POPUPWIN95MENU;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, DYNAMICOVERVIEWDELAY,
            &pHP->ulOverviewDelay, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulOverviewDelay=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, DYNAMICOVERVIEWFLAG,
            &pHP->ulDynamicOverview, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulDynamicOverview=OVERVIEW_LL;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, SLIDINGFOCUSDELAY,
            &pHP->ulSlidingFocusDelay, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSlidingFocusDelay=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, DYNAMICMENUDELAY,
            &pHP->ulDynamicMenuDelay, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulDynamicMenuDelay=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, DYNAMICSESSIONBARDELAY,
            &pHP->ulSessionBarDelay, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarDelay=5;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, VIRTUALDESKTOPDELAY,
            &pHP->ulVirtualDesktopDelay, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulVirtualDesktopDelay=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, DEBUGFLAG,
            &pHP->ulDebug, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulDebug=DEBUG_NO;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, SCROLLPERCENTAGE,
            &pHP->ulScrollPercentage, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulScrollPercentage=100;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, CLICKFLAG,
            &pHP->ulClickFlag, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulClickFlag=WM_BUTTON1DBLCLK;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, XPOSITION,
            &pHP->swpPC2.x, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2.x=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, YPOSITION,
            &pHP->swpPC2.y, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2.y=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, XSIZE,
            &pHP->swpPC2.cx, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2.cx=-1;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, YSIZE,
            &pHP->swpPC2.cy, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2.cy=-1;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SPOOLER, XPOSITION,
            &pHP->swpPC2Spooler.x, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2Spooler.x=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SPOOLER, YPOSITION,
            &pHP->swpPC2Spooler.y, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2Spooler.y=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SPOOLER, XSIZE,
            &pHP->swpPC2Spooler.cx, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2Spooler.cx=-1;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SPOOLER, YSIZE,
            &pHP->swpPC2Spooler.cy, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2Spooler.cy=-1;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SPOOLER, SPLITBARPOSITION,
            &pHP->lSplitbarPosition, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->lSplitbarPosition=100;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2STATUS, XPOSITION,
            &pHP->swpPC2Status.x, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2Status.x=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2STATUS, YPOSITION,
            &pHP->swpPC2Status.y, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2Status.y=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, XDESKTOPCOUNT,
            &pHP->ulHorizontalDesktopsSave, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulHorizontalDesktopsSave=3;
            }
        if(pHP->ulHorizontalDesktopsSave==0)
            pHP->ulHorizontalDesktopsSave=3;
        pHP->ulHorizontalDesktops=pHP->ulHorizontalDesktopsSave;
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, YDESKTOPCOUNT,
            &pHP->ulVerticalDesktopsSave, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulVerticalDesktopsSave=1;
            }
        if(pHP->ulVerticalDesktopsSave==0)
            pHP->ulVerticalDesktopsSave=1;
        pHP->ulVerticalDesktops=pHP->ulVerticalDesktopsSave;
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, BACKGROUNDBITMAPFLAG,
            &pHP->ulBackgroundBitmapFlag, &ulTemp)==FALSE)
            {
            ulRc=0L;
                                        /* Low USHORT of ulBackgroundBitmapFlag contains tile count */
            pHP->ulBackgroundBitmapFlag=BITMAPNORMAL | 3;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, LOCKUPTIMER,
            &pHP->ulLockupTimer, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulLockupTimer=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, POWERDOWNTIMER,
            &pHP->ulPowerDownTimer, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulPowerDownTimer=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, ICONSCALE,
            &pHP->ulIconScale, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulIconScale=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, SESSIONBARSTATUSAREA,
            &pHP->ulSessionBarStatusarea, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarStatusarea=SB_STATUSRIGHTJUSTIFIED;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, SESSIONBARSCALE,
            &pHP->ulSessionBarScale, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarScale=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, SESSIONBARHOTSPOT,
            &pHP->ulSessionBarHotSpot, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarHotSpot=pHP->lIconSize+((SESSIONBAR_SPACE+SESSIONBAR_BORDER+SESSIONBAR_ICONFRAME)<<1);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, HWSCREENSIZEX,
            &pHP->ulHWSizeX, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulHWSizeX=640;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, HWSCREENSIZEY,
            &pHP->ulHWSizeY, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulHWSizeY=480;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, COLORBGND,
            &pHP->ulOverviewRGBColorBgnd, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulOverviewRGBColorBgnd=WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, COLORWND,
            &pHP->ulOverviewRGBColorWnd, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulOverviewRGBColorWnd=WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, COLORFGND,
            &pHP->ulOverviewRGBColorFgnd, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulOverviewRGBColorFgnd=RGB_BLUE;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, BACKUPCOUNTER,
            &pHP->ulConfigurationChanges, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulConfigurationChanges=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR, SESSIONBARFLAG,
            &pHP->ulSessionBarFlag, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarFlag=SB_POSITIONANY;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR, XPOSITION,
            &pHP->swpPC2Session.x, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2Session.x=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR, YPOSITION,
            &pHP->swpPC2Session.y, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->swpPC2Session.y=0;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR, COLORBGND,
            &pHP->ulSessionBarRGBColorBgnd, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarRGBColorBgnd=WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR, COLORFGND,
            &pHP->ulSessionBarRGBColorFgnd, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarRGBColorFgnd=RGB_BLACK;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR, COLORINTR,
            &pHP->ulSessionBarRGBColorIntr, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarRGBColorIntr=RGB_RED;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR, COLORBUSY,
            &pHP->ulSessionBarRGBColorBusy, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarRGBColorBusy=RGB_GREEN;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR, COLORGRID,
            &pHP->ulSessionBarRGBColorGrid, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarRGBColorGrid=RGB_BLUE;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR, COLORCPU,
            &pHP->ulSessionBarRGBColorCPU, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulSessionBarRGBColorCPU=WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR, CPUPERFORMANCEFLAG,
            &pHP->ulPerformanceFlag, &ulTemp)==FALSE)
            {
            ulRc=0L;
            pHP->ulPerformanceFlag=PERFORMANCE_60S;
            }
                                        /* The length of Desktop name and Window List name
                                           is the same namely MAXNAMEL+1 */
        ulTemp=MAXNAMEL+1;
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, DESKTOPNAME,
            pHP->ucDesktopName, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucDesktopName, NLSDESKTOP);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, WINDOWLISTNAME,
            pHP->ucWindowListName, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucWindowListName, NLSWINDOWLIST);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, BACKGROUNDBITMAPNAME,
            pHP->ucBackgroundBitmap, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucBackgroundBitmap, OS2LOGOBMP);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, WINDOWFONTNAME,
            pHP->ucPC2WindowFont, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucPC2WindowFont, HELV8PTS);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, QUICKSWITCHFONTNAME,
            pHP->ucSwitchWindowFont, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucSwitchWindowFont, HELV8PTS);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, SESSIONBARFONTNAME,
            pHP->ucSessionWindowFont, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucSessionWindowFont, HELV8PTS);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, POPUPMENUFONTNAME,
            pHP->ucPopupMenuFont, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucPopupMenuFont, HELV8PTS);
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, SPOOLERFONTNAME,
            pHP->ucPC2SpoolerFont, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucPC2SpoolerFont, HELV8PTS);
            }
        ulTemp=PASSWORDSIZE+1;
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, ACCESSPASSWORD,
            pHP->ucAccessPassword, &ulTemp)==FALSE)
            {
            ulRc=0L;
            memset(pHP->ucAccessPassword, '\0', sizeof(pHP->ucAccessPassword));
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, LOCKUPPASSWORD,
            pHP->ucLockupPassword, &ulTemp)==FALSE)
            {
            ulRc=0L;
            memset(pHP->ucLockupPassword, '\0', sizeof(pHP->ucLockupPassword));
            }
                                        /* Titlebar Exception list must be terminated by \r\n */
        ulTemp=sizeof(pHP->ucTitlebarException)-2;
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, TITLEBAREXCEPTION,
            pHP->ucTitlebarException, &ulTemp)==FALSE)
            {
            ulRc=0L;
            memset(pHP->ucTitlebarException, '\0', ulTemp);
            }
        StripTrailing(pHP->ucTitlebarException);
        strcat(pHP->ucTitlebarException, "\r\n");
                                        /* HW Virtual Desktop Exception list must be terminated by \r\n */
        ulTemp=sizeof(pHP->ucHWVirtualDesktopException)-2;
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, HWVIRTUALDESKTOPEXCEPTION,
            pHP->ucHWVirtualDesktopException, &ulTemp)==FALSE)
            {
            ulRc=0L;
            memset(pHP->ucHWVirtualDesktopException, '\0', ulTemp);
            }
        StripTrailing(pHP->ucHWVirtualDesktopException);
        strcat(pHP->ucHWVirtualDesktopException, "\r\n");
                                        /* SessionBar User date/time */
        ulTemp=sizeof(pHP->ucSessionBarDateTime)-1;
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, SESSIONBARDATETIME,
            pHP->ucSessionBarDateTime, &ulTemp)==FALSE)
            {
            ulRc=0L;
            memset(pHP->ucSessionBarDateTime, '\0', ulTemp);
            }
                                        /* SessionBar Exception list must be terminated by \r\n */
        ulTemp=sizeof(pHP->ucSessionBarException)-2;
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, SESSIONBAREXCEPTION,
            pHP->ucSessionBarException, &ulTemp)==FALSE)
            {
            ulRc=0L;
            memset(pHP->ucSessionBarException, '\0', ulTemp);
            }
        StripTrailing(pHP->ucSessionBarException);
        strcat(pHP->ucSessionBarException, "\r\n");
                                        /* Sticky Windows list must be terminated by \r\n */
        ulTemp=sizeof(pHP->ucStickyWindows)-2;
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, STICKYWINDOWS,
            pHP->ucStickyWindows, &ulTemp)==FALSE)
            {
            ulRc=0L;
            memset(pHP->ucStickyWindows, '\0', ulTemp);
            }
        StripTrailing(pHP->ucStickyWindows);
        strcat(pHP->ucStickyWindows, "\r\n");
        ulTemp=sizeof(pHP->ucUserFirstName);
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, REGISTERFIRSTNAME,
            pHP->ucUserFirstName, &ulTemp)==FALSE)
            {
            ulRc=0L;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, REGISTERLASTNAME,
            pHP->ucUserLastName, &ulTemp)==FALSE)
            {
            ulRc=0L;
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, REGISTEREMAILADDRESS,
            pHP->ucUserEmailAddress, &ulTemp)==FALSE)
            {
            ulRc=0L;
            }
        ulTemp=sizeof(pHP->ucProxyUrl);
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, PROXYURL,
            pHP->ucProxyUrl, &ulTemp)==FALSE)
            {
            ulRc=0L;
#ifdef  DEBUG
            strcpy(pHP->ucProxyUrl, "proxy:8080");
#else
            strcpy(pHP->ucProxyUrl, "proxy:80");
#endif  /* DEBUG */
            }
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, UPDATEURL,
            pHP->ucUpdateUrl, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucUpdateUrl, "http://www.geocities.com/SiliconValley/Pines/7885/AutoUpdate.html");
            }
        strcpy(pHP->ucRegistrationUrl, pHP->ucUpdateUrl);
        pucTemp=strstr(pHP->ucRegistrationUrl, "://");
        if(pucTemp!=0)
            pucTemp=strchr(pucTemp+3, '/');
        else
            pucTemp=strchr(pHP->ucRegistrationUrl, '/');
        if(pucTemp!=0)
            strcpy(pucTemp, "/cgi-bin/homestead/mail.pl\?warpguru");
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, HTTPUSERID,
            pHP->ucUserHttpUserid, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucUserHttpUserid, "");
            }
        ulTemp=sizeof(pHP->ucUserHttpPassword);
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, HTTPPASSWORD,
            pHP->ucUserHttpPassword, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucUserHttpPassword, "");
            }
        ulTemp=sizeof(pHP->ucLastPC2Trap);
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, LASTPC2TRAP,
            pHP->ucLastPC2Trap, &ulTemp)==FALSE)
            {
            ulRc=0L;
            strcpy(pHP->ucLastPC2Trap, "");
            }
                                        /* Check for a PC/2 version change */
        ulTemp=sizeof(ucPC2Version);
        if(PrfQueryProfileData(hiniPC2INI, APPLICATIONNAMEPC2, PC2VERSION,
            ucPC2Version, &ulTemp)==FALSE)
            ulPC2Update=TRUE;
        else
            if(strcmp(ucPC2Version, BLDLEVEL_VERSION))
                ulPC2Update=TRUE;
        if(ulPC2Update==TRUE)
            {
            pHP->ulStatusFlag2|=(BUBBLEHELPSB|ENABLEQUICKSWITCH|CASCADEMENU);
            pHP->ulRegistrationFlag|=(REGISTER_ONLINE|REGISTER_USER|REGISTER_SHOWSTATUS);
            }
        }
    else
        {
        WinSetPointer(                  /* Set wait pointer */
            HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE));
                                        /* Write binary data to profile */
        if(!PrfWriteProfileData(
            hiniPC2INI,                 /* Handle of profile */
                                        /* Application name */
            APPLICATIONNAMEPC2,
            STATUSFLAG1,                /* Key name */
                                        /* Value data */
            &pHP->ulStatusFlag1,
            sizeof(ULONG)))             /* Size of value data */
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            STATUSFLAG2, &pHP->ulStatusFlag2, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            STATUSFLAG3, &pHP->ulStatusFlag3, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            REGISTRATIONFLAG, &pHP->ulRegistrationFlag, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            OVERVIEWDRAWMODE, &pHP->ulOverviewWindowDrawMode, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            OVERVIEWGRADIENTSTART, &pHP->ulOverviewGradientStart, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            OVERVIEWGRADIENTEND, &pHP->ulOverviewGradientEnd, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            TITLEBARFLAG, &pHP->ulTitlebarFlag, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            SMARTICONSFLAG, &pHP->ulOVSmartIconsFlag, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            POPUPKEYFLAG, &pHP->ulPopupKeyFlag, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            DYNAMICOVERVIEWDELAY, &pHP->ulOverviewDelay, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            DYNAMICOVERVIEWFLAG, &pHP->ulDynamicOverview, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            SLIDINGFOCUSDELAY, &pHP->ulSlidingFocusDelay, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            DYNAMICMENUDELAY, &pHP->ulDynamicMenuDelay, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            DYNAMICSESSIONBARDELAY, &pHP->ulSessionBarDelay, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            VIRTUALDESKTOPDELAY, &pHP->ulVirtualDesktopDelay, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            DEBUGFLAG, &pHP->ulDebug, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            SCROLLPERCENTAGE, &pHP->ulScrollPercentage, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            CLICKFLAG, &pHP->ulClickFlag, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            XPOSITION, &pHP->swpPC2.x, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            YPOSITION, &pHP->swpPC2.y, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            XSIZE, &pHP->swpPC2.cx, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            YSIZE, &pHP->swpPC2.cy, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SPOOLER,
            XPOSITION, &pHP->swpPC2Spooler.x, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SPOOLER,
            YPOSITION, &pHP->swpPC2Spooler.y, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SPOOLER,
            XSIZE, &pHP->swpPC2Spooler.cx, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SPOOLER,
            YSIZE, &pHP->swpPC2Spooler.cy, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SPOOLER,
            SPLITBARPOSITION, &pHP->lSplitbarPosition, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2STATUS,
            XPOSITION, &pHP->swpPC2Status.x, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2STATUS,
            YPOSITION, &pHP->swpPC2Status.y, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            XDESKTOPCOUNT, &pHP->ulHorizontalDesktopsSave, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            YDESKTOPCOUNT, &pHP->ulVerticalDesktopsSave, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            BACKGROUNDBITMAPFLAG, &pHP->ulBackgroundBitmapFlag, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            LOCKUPTIMER, &pHP->ulLockupTimer, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            POWERDOWNTIMER, &pHP->ulPowerDownTimer, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            ICONSCALE, &pHP->ulIconScale, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            SESSIONBARSTATUSAREA, &pHP->ulSessionBarStatusarea, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            SESSIONBARSCALE, &pHP->ulSessionBarScale, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            SESSIONBARHOTSPOT, &pHP->ulSessionBarHotSpot, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            HWSCREENSIZEX, &pHP->ulHWSizeX, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            HWSCREENSIZEY, &pHP->ulHWSizeY, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            COLORBGND, &pHP->ulOverviewRGBColorBgnd, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            COLORWND, &pHP->ulOverviewRGBColorWnd, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            COLORFGND, &pHP->ulOverviewRGBColorFgnd, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            BACKUPCOUNTER, &pHP->ulConfigurationChanges, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR,
            SESSIONBARFLAG, &pHP->ulSessionBarFlag, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR,
            XPOSITION, &pHP->swpPC2Session.x, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR,
            YPOSITION, &pHP->swpPC2Session.y, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR,
            COLORFGND, &pHP->ulSessionBarRGBColorFgnd, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR,
            COLORBGND, &pHP->ulSessionBarRGBColorBgnd, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR,
            COLORINTR, &pHP->ulSessionBarRGBColorIntr, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR,
            COLORBUSY, &pHP->ulSessionBarRGBColorBusy, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR,
            COLORGRID, &pHP->ulSessionBarRGBColorGrid, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR,
            COLORCPU, &pHP->ulSessionBarRGBColorCPU, sizeof(ULONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2SESSIONBAR,
            CPUPERFORMANCEFLAG, &pHP->ulPerformanceFlag, sizeof(LONG)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            DESKTOPNAME, pHP->ucDesktopName, sizeof(pHP->ucDesktopName)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            WINDOWLISTNAME, pHP->ucWindowListName,
            sizeof(pHP->ucWindowListName)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            BACKGROUNDBITMAPNAME, pHP->ucBackgroundBitmap,
            sizeof(pHP->ucBackgroundBitmap)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            WINDOWFONTNAME, pHP->ucPC2WindowFont,
            sizeof(pHP->ucPC2WindowFont)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            QUICKSWITCHFONTNAME, pHP->ucSwitchWindowFont,
            sizeof(pHP->ucSwitchWindowFont)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            SESSIONBARFONTNAME, pHP->ucSessionWindowFont,
            sizeof(pHP->ucSessionWindowFont)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            POPUPMENUFONTNAME, pHP->ucPopupMenuFont,
            sizeof(pHP->ucPopupMenuFont)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            SPOOLERFONTNAME, pHP->ucPC2SpoolerFont,
            sizeof(pHP->ucPC2SpoolerFont)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            ACCESSPASSWORD, pHP->ucAccessPassword,
            sizeof(pHP->ucAccessPassword)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            LOCKUPPASSWORD, pHP->ucLockupPassword,
            sizeof(pHP->ucLockupPassword)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            TITLEBAREXCEPTION, pHP->ucTitlebarException,
            sizeof(pHP->ucTitlebarException)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            HWVIRTUALDESKTOPEXCEPTION, pHP->ucHWVirtualDesktopException,
            sizeof(pHP->ucHWVirtualDesktopException)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            SESSIONBARDATETIME, pHP->ucSessionBarDateTime,
            sizeof(pHP->ucSessionBarDateTime)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            SESSIONBAREXCEPTION, pHP->ucSessionBarException,
            sizeof(pHP->ucSessionBarException)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            STICKYWINDOWS, pHP->ucStickyWindows,
            sizeof(pHP->ucStickyWindows)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            REGISTERFIRSTNAME, pHP->ucUserFirstName,
            sizeof(pHP->ucUserFirstName)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            REGISTERLASTNAME, pHP->ucUserLastName,
            sizeof(pHP->ucUserLastName)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            REGISTEREMAILADDRESS, pHP->ucUserEmailAddress,
            sizeof(pHP->ucUserEmailAddress)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            PROXYURL, pHP->ucProxyUrl,
            sizeof(pHP->ucProxyUrl)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            UPDATEURL, pHP->ucUpdateUrl,
            sizeof(pHP->ucUpdateUrl)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            HTTPUSERID, pHP->ucUserHttpUserid,
            sizeof(pHP->ucUserHttpUserid)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            HTTPPASSWORD, pHP->ucUserHttpPassword,
            sizeof(pHP->ucUserHttpPassword)))
            ulRc=0L;
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            LASTPC2TRAP, pHP->ucLastPC2Trap,
            sizeof(pHP->ucLastPC2Trap)))
            ulRc=0L;
                                        /* Write PC/2 Version information */
        strcpy(ucPC2Version, BLDLEVEL_VERSION);
        if(!PrfWriteProfileData(hiniPC2INI, APPLICATIONNAMEPC2,
            PC2VERSION, ucPC2Version,
            sizeof(ucPC2Version)))
            ulRc=0L;

        WinSetPointer(                  /* Set arrow pointer */
            HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));
        }
                                        /* Close PC2.INI */
    if(PrfCloseProfile(hiniPC2INI)==FALSE)
        USR_ERR(pHP->hwndFrame, HELP_PC2INI, MB_INFORMATION|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
            "Can't save configuration data into PC2.INI - continuing... ");
    }
else
    USR_ERR(pHP->hwndFrame, HELP_PC2INI, MB_INFORMATION|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
        "Can't save configuration data into PC2.INI - continuing... ");
                                        /* Disable System Error popups as the Prf-APIs seem to undo
                                           that for unknown (and IMHO for no sound) reasons */
DosError(FERR_DISABLEHARDERR);
return(ulRc);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure computes the middle between two colors depending on percentage        *
 * Req:                                                                                 *
 *      ulRGBStart .... Starting color                                                  *
 *      ulRGBEnd ...... Final color                                                     *
 *      lPercent ....... 0(ulRGBStart)..100(ulRGBEnd)                                   *
 * Ret:                                                                                 *
 *      ULONG ......... Returns the middle color between ulRGBStart and ulRGBEnd        *
 * Ref:                                                                                 *
 *      none                                                                            *
\*--------------------------------------------------------------------------------------*/
ULONG RGBRamp(ULONG ulRGBStart, ULONG ulRGBEnd, LONG lPercent)
{
    int     iRed,iGreen,iBlue;

                                        /* Sanity check */
    if(lPercent>100) lPercent=100;
    if(lPercent<0) lPercent=0;
    iRed=GETRVALUE(ulRGBStart) + ((int)GETRVALUE(ulRGBEnd) - (int)GETRVALUE(ulRGBStart)) * lPercent/100;
    iGreen=GETGVALUE(ulRGBStart) + ((int)GETGVALUE(ulRGBEnd) - (int)GETGVALUE(ulRGBStart)) * lPercent/100;
    iBlue=GETBVALUE(ulRGBStart) + ((int)GETBVALUE(ulRGBEnd) - (int)GETBVALUE(ulRGBStart)) * lPercent/100;
    return RGB(iRed, iGreen, iBlue);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure draws a representation of an window onto the overview window.         *
 * Req:                                                                                 *
 *      hpsClient ..... Presentation space to draw in                                   *
 *      pswpWindow .... Pointer to SWP structure containing the size & position to draw *
 *      hptrIcon ...... Window icon handle                                              *
 *      ulWindowColor . Color to fill rectangle with                                    *
 *      ulTextColor ... Color to draw rectangle text                                    *
 *      ulStyle ....... BACKGROUND_WND if it is a background window                     *
 *                      FOREGROUND_WND if it is a foreground window                     *
 *                      BORDERONLY_WND if the border should be drawn only               *
 *      ulWindowNumber  Window Z-order number                                           *
 *      ulWindowCount . Total number of windows                                         *
 *      pszWindowName . Pointer to the window's Window List entry or titlebar text      *
 * Ref:                                                                                 *
 *      none                                                                            *
\*--------------------------------------------------------------------------------------*/
void    DrawWindow(HPS hpsClient, SWP *pswpWindow, ULONG ulStyle, HPOINTER hptrIcon, PSZ pszWindowName, ULONG ulWindowNumber, ULONG ulWindowCount)
{
RECTL           rectlWindow;            /* Coordinates of one window to draw on Overview window */
POINTL          pointlText;             /* Coordinates of lower left point of window text */
                                        /* Dimensions of window's name */
POINTL          pointlFnt[TXTBOX_COUNT];
ULONG           ulNameLength;           /* Count of characters of window's name used to display */
                                        /* Window's name textbox size */
LONG            lFntHeight, lFntLength;
ULONG           ulColorSchemeIndex;     /* Index within COLORWINDOW structure */
                                        /* Window background color */
ULONG           ulWindowColor=RGB_WHITE;
ULONG           ulTextColor=RGB_BLACK;  /* Window title text color */
BOOL            bDrawIcon=FALSE;
                                        /* "Palette" for "Random"-style window coloring */
static COLORWINDOW  ColorWindow[]= { {RGB_YELLOW, RGB_BLACK}, {RGB_PINK,  RGB_WHITE}, {RGB_RED,   RGB_WHITE},
                                     {RGB_CYAN,   RGB_BLACK}, {RGB_GREEN, RGB_BLACK}, {RGB_BLUE,  RGB_WHITE},
                                     {RGB_BLACK,  RGB_WHITE} };

rectlWindow.xLeft=rectlWindow.xRight=pswpWindow->x;
rectlWindow.xRight+=pswpWindow->cx;
rectlWindow.yBottom=rectlWindow.yTop=pswpWindow->y;
rectlWindow.yTop+=pswpWindow->cy;
                                        /* Decide how to draw the window */
switch (pHP->ulOverviewWindowDrawMode)
{
                                        /* Draw pointer if requested, but fall through to
                                           set the colors anyway */
case OVERVIEW_ICONS:
    if(hptrIcon)
        bDrawIcon=TRUE;

                                        /* Otherwise fallback to monochrome image */
case OVERVIEW_MONOCHROME:
    ulWindowColor=pHP->ulOverviewRGBColorWnd;
    ulTextColor=pHP->ulOverviewRGBColorFgnd;
    break;

case OVERVIEW_RANDOM:
    ulColorSchemeIndex=ulWindowNumber % (sizeof(ColorWindow)/sizeof(COLORWINDOW));
    ulWindowColor=ColorWindow[ulColorSchemeIndex].ulBackgroundRGB;
    if(pHP->ulStatusFlag1 & OVERVIEWAUTOCOLOR)
        ulTextColor=ColorWindow[ulColorSchemeIndex].ulForegroundRGB;
    else
        ulTextColor=pHP->ulOverviewRGBColorFgnd;
    break;

case OVERVIEW_GRADIENT:
    if(ulWindowCount)
        ulWindowColor=RGBRamp(pHP->ulOverviewGradientStart, pHP->ulOverviewGradientEnd,
            100-(ulWindowNumber*100/ulWindowCount));
    else
        ulWindowColor=pHP->ulOverviewGradientStart;
    if(pHP->ulStatusFlag1 & OVERVIEWAUTOCOLOR)
        {
        if(SQUARERGBINTENSITY(ulWindowColor)>(128*128)*3)
            ulTextColor=RGB_BLACK;
        else
            ulTextColor=RGB_WHITE;
        }
    else
        ulTextColor=pHP->ulOverviewRGBColorFgnd;
    break;

default:
    return;
}
if(!(ulStyle & BORDERONLY_WND))
    {
                                        /* Fill Window background */
    GpiSetColor(hpsClient, ulWindowColor);
    GpiMove(hpsClient, (POINTL *)(&rectlWindow.xLeft));
    GpiBox(hpsClient, DRO_OUTLINEFILL, (POINTL *)(&rectlWindow.xRight), 0, 0);
                                        /* Draw icon if corresponding flag is set */
    if(bDrawIcon)
        BlitIcon(hpsClient, hptrIcon, &rectlWindow);
    }
if(pHP->ulStatusFlag1 & OVERVIEW3DBORDER)
    {
                                        /* Draw 3D border (upper right point is exclusive) */
    rectlWindow.xRight++;
    rectlWindow.yTop++;
    WinDrawBorder(hpsClient, &rectlWindow, 1, 1, 0, 0, (ulStyle & FOREGROUND_WND ? DB_RAISED : DB_DEPRESSED));
    }
else
    {
                                        /* Draw a black (or red) border around window */
    GpiSetColor(hpsClient, pswpWindow->fl & SWP_ACTIVATE ? RGB_RED : RGB_BLACK);
    GpiMove(hpsClient, (POINTL *)(&rectlWindow.xLeft));
    GpiBox(hpsClient, DRO_OUTLINE, (POINTL *)(&rectlWindow.xRight), 0, 0);
    rectlWindow.xRight++;
    rectlWindow.yTop++;
    }
                                        /* Draw window title text */
if((ulStyle & FOREGROUND_WND) && (pHP->ulStatusFlag1 & OVERVIEWTITLES))
    {
                                        /* Get dimensions of window's name */
    GpiQueryTextBox(hpsClient, 1L, pszWindowName, TXTBOX_COUNT, pointlFnt);
    lFntHeight=pointlFnt[TXTBOX_TOPLEFT].y;
    lFntLength=pointlFnt[TXTBOX_CONCAT].x;
                                        /* If the text is too high or long, don't display it and exit */
    if((lFntHeight>(pswpWindow->cy-2)) || (lFntLength>(pswpWindow->cx-4))) return;
    for(ulNameLength=2; ulNameLength<=strlen(pszWindowName); ulNameLength++)
        {
                                        /* Get dimensions of window's name */
        GpiQueryTextBox(hpsClient, ulNameLength, pszWindowName, TXTBOX_COUNT, pointlFnt);
                                        /* If current number of characters don't fit anymore
                                           into the window, break */
        if(pointlFnt[TXTBOX_CONCAT].x>=(pswpWindow->cx-4))
            break;
                                        /* Get valid dimensions */
        lFntHeight=pointlFnt[TXTBOX_TOPLEFT].y;
        lFntLength=pointlFnt[TXTBOX_CONCAT].x;
        }
    ulNameLength--;                     /* Adjust for not fitting/last character */
                                        /* Display windows title from Window List or titlebar */
    GpiSetColor(hpsClient, ulTextColor);
    pointlText.x=pswpWindow->x+(pswpWindow->cx-lFntLength)/2;
    pointlText.y=pswpWindow->y+pswpWindow->cy-lFntHeight+1;
    GpiMove(hpsClient, &pointlText);
    GpiCharString(hpsClient, ulNameLength, pszWindowName);
    }
}

/*--------------------------------------------------------------------------------------*\
 * This procedure converts DOS settings used in the configuration file and Program      *
 * Installation dialog to the format required by the WINDOWS32PROPERTYDIALOG in the     *
 * OS/2 PMVDMP.DLL.                                                                     *
 * Req:                                                                                 *
 *      pucSettings ... Pointer to the DOS Settings                                     *
 *      pulLength ..... Length of the DOS Settings buffer pucSettings on entry/exit     *
 *      bImport ....... TRUE if we convert data retured by WINDOWS32PROPERTYDIALOG      *
 *                      FALSE if we convert data into WINDOWS32PROPERTYDIALOG format    *
 * Ret:                                                                                 *
 *      pucBuffer ..... Buffer when successfull containing the converted DOS Settings   *
 *                      NULL on error                                                   *
 *                      This buffer is allocated as required by ImportDosSettings().    *
 * Ref:                                                                                 *
 *      <string>\n<string>\n<string>            Format of a DOS setting that contains   *
 *                                              more than one entry, e.g. DOS_VERSION   *
 *      <string>\0h                             One complete DOS setting, whereby       *
 *                                              <string> may of the above format        *
 *      \0h\0h                                  End of DOS settings buffer              *
\*--------------------------------------------------------------------------------------*/
UCHAR   *ImportDosSettings(UCHAR *pucSettings, ULONG *pulLength, BOOL bImport)
{
UCHAR   *pucBuffer;                     /* Return buffer */
UCHAR   *pucChar;
ULONG   ulIndex;
ULONG   ulCounter;

if(bImport==TRUE)
    {
                                        /* Count all \0 characters */
    for(ulIndex=0, ulCounter=0, pucChar=pucSettings; ulIndex<*pulLength; ulIndex++, pucChar++)
        if(*pucChar=='\0') ulCounter++;
    *pulLength+=++ulCounter;            /* Double because all \0 are replaced by \r\n which
                                           is CRLF, and add \0 termination */
    pucBuffer=malloc(*pulLength);       /* Allocate return buffer */
                                        /* Copy DOS settings into return buffer by replacing
                                           \0 by \r\n (CRLF), replace \n by *. Reserve last
                                           char for terminating \0 */
    for(ulIndex=0, pucChar=pucBuffer; ulIndex+1<*pulLength; ulIndex++, pucChar++, pucSettings++)
        {
        if(*pucSettings=='\n') *pucChar='*';
        else if(*pucSettings=='\0')
            {
            *pucChar++='\r';
            *pucChar='\n';
            ulIndex++;
            }
        else
            *pucChar=*pucSettings;
        }
    *pucChar='\0';                      /* Zero termination */
    }
else
    {
                                        /* Count all \r characters */
    for(ulIndex=0, ulCounter=0, pucChar=pucSettings; ulIndex<*pulLength; ulIndex++, pucChar++)
        if(*pucChar=='\r') ulCounter++;
    (*pulLength)-=++ulCounter;          /* Half because all \r\n (CRLF) are replaced by \0, remove
                                           trailing \0 termination */
    if(!*pulLength) return(NULL);
                                        /* Add 2 to ensure to terminate DOS settings with \0\0 */
    (*pulLength)+=2;
    pucBuffer=malloc(*pulLength);       /* Allocate return buffer */
                                        /* Ensure \0\0 termination */
    memset(pucBuffer, '\0', *pulLength);
                                        /* Copy DOS settings into return buffer by replacing
                                           \r\n by \0 and * by \n, start with index 2 to keep place
                                           for \0\0 termination */
    for(ulIndex=2, pucChar=pucBuffer; ulIndex<*pulLength; ulIndex++, pucChar++, pucSettings++)
        {
        if(*pucSettings=='*') *pucChar='\n';
        else if(*pucSettings=='\r')
            {
            *pucChar='\0';
            pucSettings++;
            }
        else
            *pucChar=*pucSettings;
        }
    }
return(pucBuffer);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure fills a program MENUDATA structure from a PROGDETAILS structure.      *
 * Req:                                                                                 *
 *      pMenuData ..... MENUDATA structure to be filled from pProgDetails               *
 *      pProgDetails .. PROGDETAILS structure of WPS Object extracted to fill a program *
 *                      MENUDATA Menuitem from                                          *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: NO_ERROR for no error, errorcode otherwise         *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
#define WPS_PARM_PATH                   "%**P"
#define WPS_PARM_DRIVE                  "%**D"
#define WPS_PARM_FILENAME               "%**N"
#define WPS_PARM_FILEEXT                "%**F"
#define WPS_PARM_EXT                    "%**E"
#define WPS_PARM_FULLPATH               "%*"

#define NUM_WPS_PARMS                   6

ULONG   LoadProgDetails2MenuData(MENUDATA *pMenuData, PROGDETAILS *pProgDetails)
{
UCHAR   pszExecutable[CCHMAXPATH];
UCHAR   ucBuffer[CCHMAXPATH*2];
BOOL    bCommandFile=FALSE;
UCHAR   *pucWPSParameter;
UCHAR   *pucWPSParameters[NUM_WPS_PARMS]={ WPS_PARM_PATH,
                                           WPS_PARM_DRIVE,
                                           WPS_PARM_FILENAME,
                                           WPS_PARM_FILEEXT,
                                           WPS_PARM_EXT,
                                           WPS_PARM_FULLPATH };

pMenuData->Item=ENTRYMENUITEM;          /* Its a executable, therefor a Menuitem */
                                        /* Get the programs's name from WPSOBJECTLIST as
                                           the Menuitem's title */
if(pProgDetails->pszTitle)
    {
    free(pMenuData->PgmTitle);
    pMenuData->PgmTitle=strdup(pProgDetails->pszTitle);
                                        /* Get the program's session title */
    free(pMenuData->WindowTitle);
    pMenuData->WindowTitle=strdup(pProgDetails->pszTitle);
    }
else
    {
    free(pMenuData->PgmTitle);          /* At least a title is required */
    pMenuData->PgmTitle=strdup("Unknown");
    }
                                        /* Get the program's working directory */
if(pProgDetails->pszStartupDir)
    {
    ULONG   ulCharIndex;

    free(pMenuData->PgmDirectory);
    ulCharIndex=strlen(pProgDetails->pszStartupDir);
    pMenuData->PgmDirectory=malloc(ulCharIndex+2);
    strcpy(pMenuData->PgmDirectory, pProgDetails->pszStartupDir);
    if((ulCharIndex>0) && (pMenuData->PgmDirectory[ulCharIndex-1]!='\\'))
        {
        pMenuData->PgmDirectory[ulCharIndex++]='\\';
        pMenuData->PgmDirectory[ulCharIndex]='\0';
        }
    }
                                        /* Get the program's icons filename (though the WPS
                                           doesn't seem to pass this information */
if(pProgDetails->pszIcon)
    {
    free(pMenuData->IconFile);
    pMenuData->IconFile=strdup(pProgDetails->pszIcon);
    }
if(pProgDetails->pszExecutable)
    {
                                        /* Test for *.CMD or *.BAT as program's executable,
                                           which we must move to the MENUDATA.PgmInputs field */
    strcpy(pszExecutable, pProgDetails->pszExecutable);
    strupr(pszExecutable);
    if((strstr(pszExecutable, ".CMD")) || (strstr(pszExecutable, ".BAT")))
        {
        bCommandFile=TRUE;
        if(strchr(pszExecutable, ' '))
            {                           /* If filename contains spaces, insert two
                                           quotation marks */
            strcpy(ucBuffer, "/c \"");
            strcat(ucBuffer, pProgDetails->pszExecutable);
            strcat(ucBuffer, "\"\"");
            }
        else
            {
            strcpy(ucBuffer, "/c ");
            strcat(ucBuffer, pProgDetails->pszExecutable);
            }
        if(pProgDetails->pszParameters)
            {                           /* Add additional parameters too */
            strcat(ucBuffer, " ");
            strcat(ucBuffer, pProgDetails->pszParameters);
            }
        free(pMenuData->PgmInputs);
        pMenuData->PgmInputs=strdup(ucBuffer);
        }
    else
        {
                                        /* Get the program's executable name, but ignore "*", which is
                                           used by the WPS to mark DOS and OS/2 command processors */
        if(strcmp(pszExecutable, "*"))
            {
            free(pMenuData->PgmName);
            pMenuData->PgmName=strdup(pProgDetails->pszExecutable);
            }
        }
    }
                                        /* Get the program's commandline parameters */
if((pProgDetails->pszParameters) && (bCommandFile==FALSE))
    {
    free(pMenuData->PgmInputs);
    pMenuData->PgmInputs=strdup(pProgDetails->pszParameters);
    }
                                        /* Remove WPS specific support for object oriented use of
                                           program and data file object (in other words the
                                           %x parameters, that get expanded when the WPS launches
                                           a program object with a data file object as the parameter */
StripTrailing(pMenuData->PgmInputs);
pucWPSParameter=strchr(pMenuData->PgmInputs, '%');
while(pucWPSParameter!=NULL)
    {
    ULONG   ulIndex;
    ULONG   ulWPSParameterLength;
    ULONG   ulMatch=FALSE;

    for(ulIndex=0; ulIndex<NUM_WPS_PARMS; ulIndex++)
        {
        ulWPSParameterLength=strlen(pucWPSParameters[ulIndex]);
        if(!strncmp(pucWPSParameter, pucWPSParameters[ulIndex], ulWPSParameterLength))
            {
            strcpy(pucWPSParameter, pucWPSParameter+ulWPSParameterLength);
            ulMatch=TRUE;
            break;
            }
        }
    if(ulMatch==FALSE)
        pucWPSParameter=strchr(pucWPSParameter+1, '%');
    }
if(!strcmp(pMenuData->PgmInputs, "%"))
    strcpy(pMenuData->PgmInputs, "");
                                        /* Get the program's environment, which is usually
                                           used to store the DOS settings */
if(pProgDetails->pszEnvironment)
    {
    ULONG   ulLengthOut;                /* Lenght of DOS Settings returned by
                                           ImportDosSettings() */
    UCHAR   *pucMatch;                   /* Used to find terminating \0\0 in DOS Settings
                                           used in PROGDETAILS */
    UCHAR   *pucDosSettingsOut;         /* DOS Settings converted from \0 terminated
                                           form used in PROGDETAILS to \n\r terminated
                                           form used in MENUDATA structure */

                                        /* Count size of DOS Settings by counting all characters
                                           until terminating \0\0 is found */
    for(ulLengthOut=1, pucMatch=pProgDetails->pszEnvironment;
        !((*pucMatch=='\0') && (*(pucMatch+1)=='\0'));
        pucMatch++)
        {
                                        /* \n appeares for DOS Settings that may have multiple
                                           lines, e.g. DOS_DEVICE=x.sys\ny.sys\0 */
        if(*pucMatch=='\n') *pucMatch='*';
        ulLengthOut++;
        }
    free(pMenuData->PgmDosSettings);
    pucDosSettingsOut=ImportDosSettings(pProgDetails->pszEnvironment, &ulLengthOut , TRUE);
                                        /* ImportDosSettings() allocated a buffer of length
                                           ulLengthOut, in which the DOS Settings have been
                                           copied. */
    pMenuData->PgmDosSettings=pucDosSettingsOut;
    }
                                        /* Set program's session control */
if(pProgDetails->progt.fbVisible & SHE_INVISIBLE)
    pMenuData->PgmControl|=SSF_CONTROL_INVISIBLE;
                                        /* Set program's session type */
pMenuData->SessionType=pProgDetails->progt.progc;
                                        /* Set no autoclose flag */
if(pProgDetails->swpInitial.fl & SWP_NOAUTOCLOSE)
    pMenuData->PgmControl|=SSF_CONTROL_NOAUTOCLOSE;
                                        /* Set minimized flag */
if(pProgDetails->swpInitial.fl & SWP_MINIMIZE)
    pMenuData->PgmControl|=SSF_CONTROL_MINIMIZE;
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure changes to the root directory of all local drives. This includes also *
 * networked drives. But it avoids to change the directory on drives that have no       *
 * file system attached, e.g. CD-ROM drives playing music CDs.                          *
\*--------------------------------------------------------------------------------------*/
ULONG   SetDriveRoot(void)
{
/*                                                                                      *\
 * Change to the root directory of all non-removable drives.                            *
\*                                                                                      */
ULONG   ulDriveNumber;                  /* Current drive (1=A, 2=B, ...) */
ULONG   ulLogicalDriveMap;              /* Bitmap of available drives (Bit 0=A, 1=B, ...) */
UCHAR   ucDrive[]="C:";                 /* Current drive */
ULONG   ulParamLengthInOut;             /* Parameter Packet Length */
ULONG   ulDataLengthInOut;              /* Data Packet Length */
ULONG   ulTemp;
HFILE   hfDiskHandle;                   /* File handle of current drive */
ULONG   ulActionTaken;                  /* Action taken on opened file (drive) */
APIRET  rc;

                                        /* Query drive bit map */
DosQueryCurrentDisk(&ulDriveNumber, &ulLogicalDriveMap);
                                        /* Save current drive map to detect added local drives */
pHP->ulLogicalDriveMap=ulLogicalDriveMap;
if(!DosOpen("\\DEV\\CD-ROM2$", &hfDiskHandle, &ulActionTaken, 0, FILE_NORMAL,
    OPEN_ACTION_OPEN_IF_EXISTS, OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY, 0))
    {
    struct _DPF                         /* Data Packet Format */
    {
        USHORT      usCDCount;          /* Number of CD-ROMs */
        USHORT      usCDFirst;          /* Number of drive of first CD-ROM (0...A, 1...B, ...) */
    } DPF;

    ulDataLengthInOut=sizeof(DPF);
                                        /* Get the first CD-ROM and number of CD-ROM drives */
    rc=DosDevIOCtl(hfDiskHandle, IOCTL_CHARDEVICE, DSK_DEVICESTATUS,
        NULL, 0, NULL,
        &DPF, ulDataLengthInOut, &ulDataLengthInOut);
    DosClose(hfDiskHandle);
    if(rc!=NO_ERROR)
        DOS_ERR(rc, pHP->hwndFrame, HELP_MEDIA, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Can't request media parameters - continuing...");
                                        /* Mark all CD-ROMs out of logical drive map to avoid
                                           changing to its root directory */
                                        /* Get first CD-ROM, Bit 0=A, 1=B, ... */
    for(ucDrive[0]='A', ulTemp=0x00000001; DPF.usCDFirst; DPF.usCDFirst--)
        {
        ucDrive[0]++;
        ulTemp<<=1;
        }
    pHP->ucCDDrive1=ucDrive[0];
                                        /* For all CD-ROMs map them out */
    for( ; DPF.usCDCount; DPF.usCDCount--)
        {
        ulLogicalDriveMap&=(~ulTemp);
        ulTemp<<=1;
        }
    DosClose(hfDiskHandle);
    }
                                        /* Exclude the drive letters requested by the 
                                           -ReserveDriveLetter Drives commandline option */
ulLogicalDriveMap&=(~pHP->ulReserveDriveLetter);
                                        /* Change to the root of all remaining drives */
for(ucDrive[0]='C', ulTemp=(ULONG)ucDrive[0]-'A', ulLogicalDriveMap>>=2; ulTemp<=(ULONG)('Z'-'A');
    ulTemp++, ucDrive[0]++, ulLogicalDriveMap>>=1)
    {                                   /* Loop for drive C: to Z: (blocks of 0s must be
                                           expected because of network drives) */
                                        /* If drive is not attached ignore drive letter */
    if((ulLogicalDriveMap&0x00000001)==0) continue;
                                        /* Open drive device readonly and fail call on error */
    rc=DosOpen(ucDrive, &hfDiskHandle, &ulActionTaken, 0, FILE_NORMAL,
        OPEN_ACTION_OPEN_IF_EXISTS, OPEN_FLAGS_DASD | OPEN_FLAGS_FAIL_ON_ERROR |
        OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY, 0);
                                        /* Local and NFS drives will return 0, LAN drives on clients
                                           will return ERROR_NETWORK_ACCESS_DENIED, LAN drives on servers
                                           will return ERROR_PATH_NOT_FOUND, TVFS drives will return
                                           ERROR_FILE_NOT_FOUND */
    if(rc==NO_ERROR)
        {                               /* On non-removeable media close it and change to the
                                           root directory of it. Don't change to root directory
                                           on removable media that isn't inserted or to not
                                           attached  drives */
        struct _PPF                     /* Parameter Packet Format */
        {
            BYTE        bCommandInformation;
            BYTE        bDriveUnit;
        } PPF={0, 0};
        struct _DPF                     /* Data Packet Format */
        {
            BYTE        bData;
        } DPF;

        ulParamLengthInOut=sizeof(PPF);
        ulDataLengthInOut=sizeof(DPF);
                                        /* Now query if the media is removable. The media
                                           needs not to be inserted */
        rc=DosDevIOCtl(hfDiskHandle, IOCTL_DISK, DSK_BLOCKREMOVABLE,
            &PPF, ulParamLengthInOut, &ulParamLengthInOut,
            &DPF, ulDataLengthInOut, &ulDataLengthInOut);
                                        /* Only Error 0 (no error) or Error 50
                                           (Network request not supported) are
                                           not treated as errors */
        if((rc!=NO_ERROR) && (rc!=ERROR_NOT_SUPPORTED))
            DOS_ERR(rc, pHP->hwndFrame, HELP_MEDIA, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Can't request media parameters - continuing...");
        DosClose(hfDiskHandle);
        }
    else if((rc==ERROR_NETWORK_ACCESS_DENIED) || (rc==ERROR_PATH_NOT_FOUND) || (rc==ERROR_FILE_NOT_FOUND))
        {
        rc=NO_ERROR;
        }
    if(rc==NO_ERROR)
        {                               /* If it is a nonremovable media, change to its root */
                                        /* 1=A, 2=B, 3=C,... */
        rc=DosSetDefaultDisk((ucDrive[0]+1)-'A');
        if(rc==NO_ERROR)
            rc=DosSetCurrentDir("\\");
        if((rc!=NO_ERROR) && 
                                        /* Data CD in CD-ROM drive */
            (rc!=ERROR_DEVICE_IN_USE) &&
                                        /* CD-ROM problem ? */
            (rc!=ERROR_GEN_FAILURE))
            DOS_ERR(rc, pHP->hwndFrame, HELP_SETPATH, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Can't change current directory - continuing...");
        }
    }
                                        /* Before returning, set the current directory to the
                                           boot drive (alternatively one could also use the drive
                                           PC/2 was started from). If we would not be doing that
                                           PC/2 would keep the current directory on the drive
                                           with the highest drive letter, if that is a network
                                           drive it can't be unmounted then */
rc=DosSetDefaultDisk((pHP->ucBootDrive+1)-'A');
if(rc==NO_ERROR)
    rc=DosSetCurrentDir("\\");
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure adds recursively a LIST of WPSOBJECTLIST elements to the Popup-Menu   *
 * (That is PC/2's internal MENUDATA linked list, not for PM which must be done with    *
 * SetPMMenu() afterwards).                                                             *
 * Req:                                                                                 *
 *      pMenuData ..... "Root" level of Popup-Menu to insert                            *
 *      pListRoot ..... Pointer to LIST of WPSOBJECTLIST to insert a pMenuData          *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: NO_ERROR for no error, errorcode otherwise         *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   ConvertWPSObjectList2MenuData(MENUDATA *pMenuData, WPSOBJECTLIST *pListRoot)
{
WPSOBJECTLIST   *pWPOLCurrent;          /* Current LIST element of pListRoot for current list level */
                                        /* Last MENUDATA structure of current list level pMenuData */
MENUDATA        *pMDInsertAfter=pMenuData;
MENUDATA        *pMDInsert;             /* Current MENUDATA structure to insert */

                                        /* Enumerate current level of WPSOBJECTLIST LIST pListRoot */
pWPOLCurrent=pListRoot;
while(pWPOLCurrent)
    {
    if(pMDInsertAfter->Item==ENTRYEMPTY)
        {                               /* An emtpy MENUDATA LIST item is the first item of a Submenu,
                                           so we have to fill this one */
                                        /* Fill this one from WPSOBJECTLIST LIST and assign it the
                                           next free id */
        pMDInsert=pMDInsertAfter;
        pMDInsert->id=pHP->MenuDataId++;
        }
    else
        {                               /* For any nonempty MENUDATA LIST item, we have to append a
                                           new MENUDATA item */
                                        /* Allocate a MENUDATA structure for current list
                                           element we are going to extract from the LIST and
                                           assign it the next free identifier */
        pMDInsert=AllocateMenuData(NULL);
        pMDInsert->id=pHP->MenuDataId++;
                                        /* Link the newly created in */
        pMDInsert->Back=pMDInsertAfter;
        pMDInsertAfter->Next=pMDInsert;
        }
    pMDInsertAfter=pMDInsert;           /* Next iteration we insert after the MENUDATA item
                                           we currently insert/replace */
/*                                                                                      *\
 * If the current element is a WPProgram Object extracted from the WPS, we have to      *
 * convert a PROGDETAILS structure into a ENTRYMENUITEM type MENUDATA structure.        *
\*                                                                                      */
    if(pWPOLCurrent->ulType==WPSOBJECTPROGRAM)
        {
        pMDInsert->Item=ENTRYMENUITEM;  /* It's a Menuitem */
        LoadProgDetails2MenuData(pMDInsert, (PROGDETAILS *)pWPOLCurrent->pData);
        }
/*                                                                                      *\
 * If the current element is a WPFolder Object extracted from the WPS, we have to       *
 * convert a character pointer into a ENTRYSUBMENU type MENUDATA structure.             *
\*                                                                                      */
    if(pWPOLCurrent->ulType==WPSOBJECTFOLDER)
        {
        MENUDATA    *pMDSubmenu;        /* First Menuitem within the Submenu */

        pMDInsert->Item=ENTRYSUBMENU;   /* It's a Submenu */
                                        /* Get the folder's name from WPSOBJECTLIST as
                                           the Submenu's title */
        free(pMDInsert->PgmTitle);
        pMDInsert->PgmTitle=strdup((UCHAR *)pWPOLCurrent->pData);
                                        /* Now obtein a entry for a submenu, adjust the
                                           linked list to it and call this procedure with
                                           the new entry recursively again */
        pMDSubmenu=AllocateMenuData(NULL);
        pMDInsert->Submenu=pMDSubmenu;
        pMDSubmenu->Back=pMDInsert;
                                        /* Recursively add the folder's contents to the
                                           Submenu Menuitem */
        ConvertWPSObjectList2MenuData(pMDSubmenu, pWPOLCurrent->pFolder);
        }
/*                                                                                      *\
 * If the current element is a WPObject Object extracted from the WPS, we have to       *
 * convert a character pointer of the Object's ObjectID into a ENTRYMENUITEM type       *
 * MENUDATA structure.                                                                  *
\*                                                                                      */
    if(pWPOLCurrent->ulType==WPSOBJECTOBJECT)
        {
        UCHAR   *pucSeparator;          /* We expect pData point to a string of
                                           pszObjectTitle@<ObjectID> where @ is our
                                           separator. This format is reliable, because
                                           WPS 2 PC/2 ensures it */

        pMDInsert->Item=ENTRYMENUITEM;  /* It's a Menuitem */
                                        /* And it's a WPS Object */
        pMDInsert->SessionType=PROG_WPSOBJECT;
        pucSeparator=strrchr((UCHAR *)pWPOLCurrent->pData, '@');
        *pucSeparator++='\0';           /* Separate pszObjectTitle from <ObjectID> */
                                        /* Get the ObjectID from WPSOBJECTLIST as
                                           the Menuitem's title and path & filename */
        free(pMDInsert->PgmTitle);
        pMDInsert->PgmTitle=strdup((UCHAR *)pWPOLCurrent->pData);
        free(pMDInsert->WindowTitle);
        pMDInsert->WindowTitle=strdup((UCHAR *)pWPOLCurrent->pData);
        free(pMDInsert->PgmName);
        pMDInsert->PgmName=strdup(pucSeparator);
        }
                                        /* Adjust to next WPSOBJECTLIST LIST element */
    pWPOLCurrent=pWPOLCurrent->pNext;
    }
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure tries to locate an object (e.g. an executable) in the path pointed to *
 * by an environment variable.                                                          *
 * This may be used if the user only specified the executable's filename without a      *
 * extension and/or fully qualified path.                                               *
 * Req:                                                                                 *
 *      pucEnvironment  Environment that included the Environment Variable we are       *
 *                      interested in that may contain the path to the object we are    *
 *                      searching for                                                   *
 *      pucEnvironmentVariable                                                          *
 *                      The Environment Variable containing the paths for the object we *
 *                      are searching for                                               *
 *      pucObject ..... The object we are searching in the Environment                  *
 * Ret:                                                                                 *
 *      pucObjectFound  The fully qualified path the object was found (dynamically      *
 *                      allocated to CCHMAXPATH length), or NULL otherwise              *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
UCHAR   *LocateInEnvironment(UCHAR *pucEnvironment, UCHAR *pucEnvironmentVariable, UCHAR *pucObject)
{
UCHAR           *pucIndex;
UCHAR           *pucVariable;
ULONG           ulVariableLength;
HDIR            hdirFileHandle;
FILEFINDBUF3    filefindbuf3File;
ULONG           ulFileCount;
APIRET          apiretRc;
UCHAR           ucObjectPath[CCHMAXPATH+1];
UCHAR           *pucObjectFound=NULL;

                                        /* Append "=" character to Environment Variable */
pucVariable=(UCHAR *)malloc(strlen(pucEnvironmentVariable)+2);
strcpy(pucVariable, pucEnvironmentVariable);
strcat(pucVariable, "=");
ulVariableLength=strlen(pucVariable);
                                        /* Loop the environment which is terminated by a \0\0 and
                                           search for the Environment Variable environment entry */
for(pucIndex=pucEnvironment; *pucIndex && !pucObjectFound; pucIndex=strchr(pucIndex, '\0')+1)
    {
                                        /* We're only interested in the Environment Variable environment
                                           entry, which has a layout like: PATH=.;D:\\OS2;...H:\\IBMCPP\\BIN;\0 */
    if(!strncmp(pucIndex, pucVariable, strlen(pucVariable)))
        {
        UCHAR   *pucPath=NULL;
        UCHAR   *pucPathEnd=NULL;

        pucIndex+=ulVariableLength;
        for(pucPath=pucIndex; pucPath && *pucPath; pucPath=pucPathEnd+1)
            {
                                        /* Each path within the environment entry is expected to be
                                           terminated by a ';'. If we don't find one, assume there is none */
            pucPathEnd=strchr(pucPath, ';');
                                        /* Copy the path we found to append the object's name afterwards.
                                           If there can't be found a ; goto the end of the path, but correct
                                           for the increment at the end of the for-loop */
            if(!pucPathEnd)
                pucPathEnd=strchr(pucPath, '\0');
            memset(ucObjectPath, '\0', sizeof(ucObjectPath));
            memcpy(ucObjectPath, pucPath, (pucPathEnd-pucPath));
            if(ucObjectPath[strlen(ucObjectPath)-1]!='\\')
                strcat(ucObjectPath,"\\");
            strcat(ucObjectPath, pucObject);
                                        /* Now test if the fully qualified filename is really the
                                           filename of a existing file */
            hdirFileHandle=HDIR_CREATE;
            ulFileCount=1;
            apiretRc=DosFindFirst(ucObjectPath, &hdirFileHandle,
                FILE_ARCHIVED|FILE_SYSTEM|FILE_HIDDEN|FILE_READONLY,
                &filefindbuf3File, sizeof(filefindbuf3File), &ulFileCount, FIL_STANDARD);
            DosFindClose(hdirFileHandle);
                                        /* If a file exists for the fully qualified filename, return it */
            if(apiretRc==NO_ERROR)
                {
                pucObjectFound=(UCHAR *)malloc(CCHMAXPATH+1);
                strcpy(pucObjectFound, ucObjectPath);
                break;
                }
            }
        }
    }
free(pucVariable);
                                        /* Return found file or NULL */
return(pucObjectFound);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure loads or unloads a menuentry's icon used during ownerdraw of the      *
 * Popup-Menu. We have to differentiate between executables, WPS Objects, PC/2 Control  *
 * menuitems and Submenus.                                                              *
 * Req:                                                                                 *
 *      pMenuData ..... Menuentry to load or unload icon                                *
 *      bLoad ......... Flag to control load (TRUE) or unload (FALSE) of icon           *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: NO_ERROR for no error, errorcode otherwise         *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   InitializeIcon(MENUDATA *pMenuData, BOOL bLoad)
{
UCHAR               *pucFullPathExecutableReference=NULL;
UCHAR               *pucFullPathExecutable=NULL;
UCHAR               *pucFullPathIconFile=NULL;
                                        /* Icon's pointer info */
POINTERINFO         pointerinfoIcon;
                                        /* Bitmap implementing the icon */
BITMAPINFOHEADER2   bitmapinfoheaderIcon;


                                        /* We are loading the icon by WinLoadFileIcon(), which may block our
                                           current thread in the file system, so set a flag when we're already
                                           loading the icon for the current MENUDATA */
if(bLoad==TRUE)
    {
                                        /* I'm not sure who turns that on always, but it seems to
                                           happen */
    DosError(FERR_DISABLEHARDERR);
                                        /* As multiple threads (the main thread during WM_MEASUREITEM, and
                                           the I/O thread during WM_POPUPMENUSETICON) may make a load request
                                           for an icon, ignore requests for menuentries with already loaded
                                           icons or where a thread just blocks in the file system during
                                           WinLoadFileIcon() and got preempted. This flag ensures that even
                                           if one thread blocks in WinLoadFileIcon() another thread doesn't try
                                           to load the icon again, as the first has not returned a valid pointer
                                           handle yet */
    if(pMenuData->StatusFlag & ICONLOADED)
        return(NO_ERROR);
    pMenuData->hpointerIcon=NULLHANDLE;
    pMenuData->StatusFlag|=ICONLOADED;
    pucFullPathExecutableReference=malloc(CCHMAXPATH+1);
    pucFullPathExecutable=pucFullPathExecutableReference;
    memset(pucFullPathExecutable, '\0', CCHMAXPATH+1);
                                        /* Set boot drive */
    strcpy(pucFullPathExecutable, "C:\\");
    pucFullPathExecutable[0]=pHP->ucBootDrive;
                                        /* Allow that customized icons are tried to be loaded
                                           preferable (even when PC/2 macros are used) */
    pucFullPathIconFile=malloc(CCHMAXPATH+1);
    strcpy(pucFullPathIconFile, pMenuData->IconFile);
    ExpandMacros(pucFullPathIconFile, pMenuData->IconFile, NULL);
    }
#ifdef  DEBUG_UTILITY
    printf("Utility: %s icon for \"%s\"\n", (bLoad ? "Load" : "Unload"), pMenuData->PgmTitle);
#endif  /* DEBUG_UTILITY */
                                        /* Load the menuentry's file icon. The menuentry can either
                                           be a submenu, program or menucontrol */
switch(pMenuData->Item)
    {
    case ENTRYSUBMENU:
                                        /* Load or unload a shared folder icon */
        if(bLoad)
            {
            if(strcmp(pucFullPathIconFile, ""))
                {
                pMenuData->hpointerIcon=WinLoadFileIcon(pucFullPathIconFile, FALSE);
                pMenuData->StatusFlag|=ICONFROMICONFILE;
                }
            if(!pMenuData->hpointerIcon)
                pMenuData->hpointerIcon=WinLoadFileIcon(pucFullPathExecutable, FALSE);
            }
        else
            WinFreeFileIcon(pMenuData->hpointerIcon);
        break;

    case ENTRYMENUITEM:
        {
        if(bLoad)
            {
                                        /* Unfortunately PgmName sometimes is NULL not a
                                           NULL string as it should be */
            if(pMenuData->PgmName==0)
                {
#ifdef  DEBUG
                _interrupt(3);
#endif  /* DEBUG */
                break;
                }
                                        /* The executable's program name may contain the %BOOTDRIVE%, %PC2%
                                           or %CDDRIVE% macro, in that case we have to replace it with the 
                                           drive we booted from, or the path we launched from or the
                                           path of the first CD-ROM drive */
            ExpandMacros(pucFullPathExecutable, pMenuData->PgmName, NULL);
                                        /* See if the icon's cache path is not invalid by now (because
                                           of the user changing the menuentry we try to load the icon for.
                                           We do this by looking if the executable's name (without any path
                                           information) from the cache matches the executable's name in
                                           PgmName, except for empty executables (OS/2, DOS,... windows)  */
            if((strcmp(pMenuData->PgmFullPathExecutable, "")) &&
                (strcmp(pMenuData->PgmName, "")))
                {
                UCHAR   *pucFileNameBegin;
                UCHAR   *pucFileNameEnd;

                pucFileNameBegin=strrchr(pMenuData->PgmFullPathExecutable, '\\');
                if(pucFileNameBegin==NULL)
                    pucFileNameBegin=pMenuData->PgmFullPathExecutable;
                else
                    pucFileNameBegin++;
                pucFileNameEnd=strrchr(pucFileNameBegin, '.');
                if(pucFileNameEnd)
                    *pucFileNameEnd='\0';
                                        /* Little drawback is that e.g. FDISK matches also FDISKPM */
                if(!strstr(pMenuData->PgmName, pucFileNameBegin))
                    {
#ifdef  DEBUG_UTILITY
                    printf("Utility: Clearing cache for %s\n", pMenuData->PgmTitle);
#endif  /* DEBUG_UTILITY */
                    strcpy(pMenuData->PgmFullPathExecutable, "");
                    }
                if(pucFileNameEnd)
                    *pucFileNameEnd='.';
                }
            switch(pMenuData->SessionType)
                {
                                        /* WPS Objects require a special processing below */
                case PROG_WPSOBJECT:
                    break;

                                        /* Check for OS/2 command line sessions (fullscreen and
                                           windowed) */
                case PROG_FULLSCREEN:
                case PROG_WINDOWABLEVIO:
                    if(!strcmp(pMenuData->PgmName, ""))
                        strcat(pucFullPathExecutable, "OS2\\CMD.EXE");
                    break;

                                        /* Check for DOS command line sessions (fullscreen and
                                           windowed) */
                case PROG_VDM:
                case PROG_WINDOWEDVDM:
                    if(!strcmp(pMenuData->PgmName, ""))
                        strcat(pucFullPathExecutable, "OS2\\MDOS\\COMMAND.COM");
                    break;

                                        /* All other session types have their own icon. The fully qualified
                                           path to the executable may be available by either the PgmName,
                                           or by PgmDirectory\\PgmName, or within the PATH defined by the
                                           Environment settings (which is the most complicated one) */
                default:
                                        /* If the session's path contains a : or a \ as the second character,
                                           then we have a fully qualified path (including UNC name) available
                                           and we must not append the PgmName to PgmDirectory but use
                                           the fully qualified PgmName instead */
                    if((pMenuData->PgmName[1]==':') || (pMenuData->PgmName[1]=='\\'))
                        strcpy(pucFullPathExecutable, pMenuData->PgmName);
                    else if(pMenuData->PgmName[0]!='%')
                        {
                        ULONG   ulTemp=strlen(pMenuData->PgmDirectory);

                                        /* The executable's directory may contain the %BOOTDRIVE%, %PC2%
                                           or %CDDRIVE% macro left aligned, in that case we have to replace it 
                                           with the drive we booted from, or the path we launched from or
                                           the path of the first CD-ROM drive. We do not cover other macros 
                                           here (as we need to construct the session's environment first) */
                        ExpandMacros(pucFullPathExecutable, pMenuData->PgmDirectory, NULL);
                                        /* Ensure that the last character of the working
                                           directory is a \ to append session's filename after */
                        ulTemp=strlen(pucFullPathExecutable);
                        if((ulTemp>0) && (pucFullPathExecutable[ulTemp-1]!='\\'))
                            strcat(pucFullPathExecutable, "\\");
                        strcat(pucFullPathExecutable, pMenuData->PgmName);
                        }
                    break;
                }
            }
                                        /* Now try to load or unload the executable's icon with fully qualified
                                           path information, which may need further checks if it doesn't work. 
                                           Check for WPS objects, where we don't have access to currently */
        if(pMenuData->SessionType==PROG_WPSOBJECT)
            {
            if(bLoad)
                {
                if(strcmp(pucFullPathIconFile, ""))
                    pMenuData->hpointerIcon=WinLoadFileIcon(pucFullPathIconFile, FALSE);
                if(pMenuData->hpointerIcon)
                    pMenuData->StatusFlag|=ICONFROMICONFILE;
                else
                    pMenuData->hpointerIcon=WinLoadPointer(HWND_DESKTOP,
                        pHP->hDLLPc2Resource, ID_ICONWPSOBJECT);
                }
            else
                WinDestroyPointer(pMenuData->hpointerIcon);
            }
        else
            {
            if(bLoad)
                {
                UCHAR   *pucExtension;
                BOOL    bFound=FALSE;

                                        /* Try to load the icon from cached path (which gets cleared
                                           when the menuentry changes) */
                if(strcmp(pucFullPathIconFile, ""))
                    pMenuData->hpointerIcon=WinLoadFileIcon(pucFullPathIconFile, FALSE);
                if(pMenuData->hpointerIcon)
                    {
                    pMenuData->StatusFlag|=ICONFROMICONFILE;
                    bFound=TRUE;
                    goto LOAD_UNKNOWNICON;
                    }
                                        /* If we have not path, skip loading */
                if(!strlen(pucFullPathExecutable))
                    goto LOAD_UNKNOWNICON;
                                        /* If a previous scan led to a fully qualified path of an
                                           executable, try to load icon from this cached path
                                           first */
                if(*(pMenuData->PgmFullPathExecutable))
                    {
                    if((pMenuData->hpointerIcon=WinLoadFileIcon(pMenuData->PgmFullPathExecutable, FALSE))!=NULLHANDLE)
                        strcpy(pucFullPathExecutable, pMenuData->PgmFullPathExecutable);
                    }
                                        /* Try to load the icon from the fully qualified path of PgmName or
                                           PgmDirectory\\PgmName concatenated above. Only if this path points
                                           to the actual executable the icon can be loaded */
                if(!pMenuData->hpointerIcon)
                    pMenuData->hpointerIcon=WinLoadFileIcon(pucFullPathExecutable, FALSE);
                                        /* If we couldn't load from the fully qualified path, test if the
                                           exeutable's extension .EXE or .COM is missing */
                if(!pMenuData->hpointerIcon)
                    {
                                        /* An fully qualified executable by default has either the extension
                                           .EXE or .COM, so test for a file with these possible extensions,
                                           if there is none yet */
                    pucExtension=strrchr(pucFullPathExecutable, '.');
                    if(!pucExtension)
                        pucExtension=strrchr(pucFullPathExecutable, '\0');
                    if((stricmp(pucExtension, ".EXE")) && (stricmp(pucExtension, ".COM")))
                        {
                                        /* If the extenstion is not .EXE or .COM, try with adding .EXE */
                        strcpy(pucExtension, ".EXE");
                        pMenuData->hpointerIcon=WinLoadFileIcon(pucFullPathExecutable, FALSE);
                                        /* Try adding .COM */
                        if(pMenuData->hpointerIcon==NULLHANDLE)
                            {
                            strcpy(pucExtension, ".COM");
                            pMenuData->hpointerIcon=WinLoadFileIcon(pucFullPathExecutable, FALSE);
                            }
                                        /* Save some CPU time by skipping the next redundant
                                           checking, as the fully qualified path didn't help us, and
                                           if the user specifies such a path, it's his decision */
                        goto SKIP_SCANENVIRONMENT;
                        }
                    else if((pMenuData->PgmName[1]==':') || (pMenuData->PgmName[1]=='\\'))
                        goto LOAD_UNKNOWNICON;
                    }
                                        /* As the fully qualified path from above couldn't load the executable's
                                           icon, this path may not point to the executable, so we have to
                                           search through the Environment settings of the PATH environment
                                           variable as a last resort. This works for menuentries like:
                                             PgmName: E.EXE
                                             PgmDirectory: H:\\TOOLKT\\
                                             PgmInputs: README
                                             "Environment": PATH=...D:\\OS2;...
                                           Here H:\\TOOLKT\\E.EXE does not load an icon, because the E editor
                                           started via the PATH should just load the document H:\\TOOLKT\\README.
                                          
                                           Before searching through the PATH we have to consider macros used
                                           for menuentries like:
                                             PgmName: %DRIVE%\\FDISKPM.EXE
                                             PgmDirectory: %DRIVE%\\
                                             "Environmen": DRIVE=M:...
                                           Here we have to expand the macros to get the fully qualified path,
                                           because FDISKPM will be invoked from M:\\FDISKPM.EXE */
                if(!pMenuData->hpointerIcon)
                    {
                    UCHAR   *pucObject;
                    UCHAR   *pucEnvironment=NULL;
                    UCHAR   *pucFullPathInEnvironment=NULL;

                                        /* Getting the environment and looping through all possible paths really
                                           is a time consuming task, which may be omitted the next time by
                                           saving the cache path containing what we will find */
                    pucEnvironment=SessionEnvironment(pMenuData);
                    pucExtension=strrchr(pucFullPathExecutable, '.');
                                        /* As a last resort to assume a fully qualified path is specified,
                                           expand the macros specified for the executable and see if that
                                           gets our icon loaded */
                    ExpandMacros(pucFullPathExecutable, pMenuData->PgmName, pucEnvironment);
                    pMenuData->hpointerIcon=WinLoadFileIcon(pucFullPathExecutable, FALSE);
                                        /* As even the fully qualified path does not work, try to locate
                                           our executable by checking the path */
                    if(!pMenuData->hpointerIcon)
                        {
                                        /* Try to locate executable via PATH path, which we load in the
                                           form: EnvVar1=Value1\0EnvVar2=Value2\0...EnvVarn=Valuen\0\0 */
                        if((pucExtension=strrchr(pucFullPathExecutable, '\\'))!=NULL)
                            strcpy(pucFullPathExecutable, pucExtension+1);
                        pucObject=pucFullPathExecutable;
                                        /* If the extenstion is not .EXE or .COM, try with adding .EXE and .COM */
                        if(pucExtension)
                            if(!((stricmp(pucExtension, ".EXE")) && (stricmp(pucExtension, ".COM"))))
                                pucFullPathInEnvironment=LocateInEnvironment(pucEnvironment, "PATH", pucObject);
                        if(!pucExtension)
                            {
                            strcat(pucObject, ".EXE");
                            pucExtension=pucFullPathInEnvironment=LocateInEnvironment(pucEnvironment, "PATH", pucObject);
                            }
                        if(!pucExtension)
                            {
                            pucExtension=strrchr(pucObject, '.');
                            strcpy(pucExtension, ".COM");
                            pucFullPathInEnvironment=LocateInEnvironment(pucEnvironment, "PATH", pucObject);
                            }
                                        /* Now try to load the executable's icon from this fully qualified path */
                        if(pucFullPathInEnvironment)
                            {
                            strcpy(pucFullPathExecutable, pucFullPathInEnvironment);
                            free(pucFullPathInEnvironment);
                            pMenuData->hpointerIcon=WinLoadFileIcon(pucFullPathExecutable, FALSE);
                            }
                        }
                                        /* Free environment */
                    free(pucEnvironment);
                    }
SKIP_SCANENVIRONMENT:
                if(pMenuData->hpointerIcon)
                    {
                                        /* We found the executable in a path defined in the session's unique
                                           environment settings, saving this cache would decrease load time
                                           the next time PC/2 is launched */
                    if(strcmp(pucFullPathExecutable, pMenuData->PgmFullPathExecutable))
                        {
                        free(pMenuData->PgmFullPathExecutable);
                        pMenuData->PgmFullPathExecutable=strdup(pucFullPathExecutable);
                        if(pHP->ulStatusFlag2 | POPUPICON)
                            pHP->ulRuntimeFlag|=ICONCACHEACTIVE;
                        }
#ifdef  DEBUG_UTILITY
                    printf("  Path: %s\n", pMenuData->PgmFullPathExecutable);
#endif  /* DEBUG_UTILITY */
                    bFound=TRUE;
                    }
                                        /* Load default icon from resource if we couldn't load an icon from
                                           the executable located by searching the PATH environment entry */
LOAD_UNKNOWNICON:
                if(!bFound)
                    {
                    pMenuData->hpointerIcon=WinLoadPointer(HWND_DESKTOP,
                        pHP->hDLLPc2Resource, ID_ICONUNKNOWN);
                    pMenuData->StatusFlag|=ICONFROMRESOURCE;
#ifdef  DEBUG_UTILITY
                    printf("  Unknown\n");
#endif  /* DEBUG_UTILITY */
                    }
                }
            else
                {
                if(pMenuData->StatusFlag&ICONFROMRESOURCE)
                    WinDestroyPointer(pMenuData->hpointerIcon);
                else
                    WinFreeFileIcon(pMenuData->hpointerIcon);
                }
            }
        }
        break;

    case ENTRYCONTROL:
        if(bLoad)
            pMenuData->hpointerIcon=WinLoadPointer(HWND_DESKTOP,
                pHP->hDLLPc2Resource, pMenuData->id);
        else
            WinDestroyPointer(pMenuData->hpointerIcon);
        break;
    }

                                        /* If we had the icon unloaded, clear its handle too */
if(!bLoad)
    {
    pMenuData->hpointerIcon=NULLHANDLE;
    pMenuData->StatusFlag&=(~(ICONLOADED|ICONFROMRESOURCE|ICONFROMICONFILE));
    }
else
    {
                                        /* Assume that Icon size can not be determined and use default 
                                           one, unless we know better */
//@@@@@@ This does not work, that is the icon is always 40x40, we have to load the file
//       using DOS I/O and parse it directly to get the correct information. I have no
//       idea why PM works that way... (regardless if icons saved in OS/2 1.2 or 2.0 format)
    pHP->lIconSize=pHP->lIconSize; 
    if(WinQueryPointerInfo(pMenuData->hpointerIcon, &pointerinfoIcon))
        {
        memset(&bitmapinfoheaderIcon, 0, sizeof(bitmapinfoheaderIcon));
        bitmapinfoheaderIcon.cbFix=16;
        if(GpiQueryBitmapInfoHeader(pointerinfoIcon.hbmColor, &bitmapinfoheaderIcon))
            {
#ifdef  DEBUG
            if((pHP->lIconSize!=bitmapinfoheaderIcon.cx) ||
                (pHP->lIconSize!=bitmapinfoheaderIcon.cy))
                printf("Utility: Icon %s is %d:%d\n", pMenuData->PgmTitle, 
                    bitmapinfoheaderIcon.cx, bitmapinfoheaderIcon.cy);
#endif  /* DEBUG */
            bitmapinfoheaderIcon.cx;
            bitmapinfoheaderIcon.cy;
            }
        }
    free(pucFullPathExecutableReference);
    free(pucFullPathIconFile);
    }
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure centers a (frame) window on the Desktop.                              *
 * Req:                                                                                 *
 *      hwndDialog .... Dialog (frame window) to center                                 *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: TRUE no error, FALSE otherwise                     *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
MRESULT CenterWindow(HWND hwndDialog)
{
SWP         swp;

WinQueryWindowPos(                      /* Query position of dialog window */
    hwndDialog,                         /* Handle of dialog window */
    &swp);                              /* Fill with position */
swp.fl=SWP_MOVE;                        /* Center dialog window */
swp.x=(pHP->swpScreen.cx-swp.cx)>>1;
swp.y=(pHP->swpScreen.cy-swp.cy)>>1;
swp.cx=swp.cy=0;
swp.hwndInsertBehind=NULLHANDLE;
WinSetMultWindowPos(pHP->habPc2, &swp, 1);
return((MRESULT)TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure blits an icon into a destination rectangle. The icon is blitted       *
 * transparently, that is the original pixel are not overwritten where the icon         *
 * consists of background pixels.                                                       *
 * Req:                                                                                 *
 *      hpsDestination  This presentation space to blit icon into                       *
 *      hpointerIcon .. The icon's handle to be blitted                                 *
 *      prectlIcon .... The target rectangle to blit icon into                          *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: NO_ERROR no error, PMERR_* otherwise               *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   BlitIcon(HPS hpsDestination, HPOINTER hpointerIcon, RECTL *prectlIcon)
{
ULONG               ulMiniIcon=FALSE;   /* TRUE if mini bitmaps should be used */
HBITMAP             hbitmapPointer;     /* Monochrome bitmap handle to blit with GPI */
HBITMAP             hbitmapColor;       /* Color bitmap handle to blit with GPI */
                                        /* Icon's pointer info */
POINTERINFO         pointerinfoIcon;
                                        /* Bitmap implementing the icon */
BITMAPINFOHEADER2   bitmapinfoheaderIcon;
HDC                 hdcIcon;            /* Memory DC and PS to blit icon's bitmap from */
HPS                 hpsMemory;
LONG                lColorHps;
SIZEL               sizelBitmap;
POINTL              ptlBlt[4];          /* Target and source points for blitting icon's bitmaps */
ULONG               ulRc=NO_ERROR;

if(!WinQueryPointerInfo(hpointerIcon, &pointerinfoIcon))
    return(PMERR_INVALID_HPTR);
                                        /* Test for availability of mini pointer */
if((prectlIcon->xRight-prectlIcon->xLeft)<=(pHP->lIconSize>>1))
    ulMiniIcon=TRUE;
hbitmapPointer=pointerinfoIcon.hbmMiniPointer;
if((ulMiniIcon==FALSE) || (hbitmapPointer==NULLHANDLE))
    hbitmapPointer=pointerinfoIcon.hbmPointer;
hbitmapColor=pointerinfoIcon.hbmMiniColor;
if((ulMiniIcon==FALSE) || (hbitmapColor==NULLHANDLE))
    hbitmapColor=pointerinfoIcon.hbmColor;
                                        /* Save current color and set to default color, or blitting the monochrome
                                           bitmap fails (for some unknown reasons, possibly because different
                                           settings in window and memory presentation space) */
lColorHps=GpiQueryColor(hpsDestination);
GpiSetColor(hpsDestination, CLR_DEFAULT);
                                        /* Get the icon's bitmaps used to draw the icon. First the monochrome
                                           bitmap of the icon is blitted, which works like a bitmask that
                                           allows/deny blitting of the color bitmap at the pixel level */
bitmapinfoheaderIcon.cbFix=16;
                                        /* For some unknown reasons this call fails with PMERR_HBITMAP_BUSY
                                           for EPM editor windows, unless the window is minimized for the
                                           first time (the docu says that this message means the bitmap
                                           is accessed by our thread while being locked by another thread) */
if(!GpiQueryBitmapInfoHeader(hbitmapColor, &bitmapinfoheaderIcon))
    return(PMERR_HBITMAP_BUSY);
                                        /* Create a memory presentation space containing the monochrome and color
                                           bitmap as the source to blit from */
hdcIcon=DevOpenDC(pHP->habPc2, OD_MEMORY, "*", 0L, NULL, NULLHANDLE);
sizelBitmap.cx=0;
sizelBitmap.cy=0;
hpsMemory=GpiCreatePS(pHP->habPc2, hdcIcon, &sizelBitmap, PU_PELS|GPIA_ASSOC|GPIT_MICRO);
                                        /* Target rectangle to blit icon to */
ptlBlt[0].x=prectlIcon->xLeft; ptlBlt[0].y=prectlIcon->yBottom;
ptlBlt[1].x=prectlIcon->xRight; ptlBlt[1].y=prectlIcon->yTop;
                                        /* Source point to blit monochrome icon from */
ptlBlt[2].x=0;
ptlBlt[2].y=bitmapinfoheaderIcon.cy;
ptlBlt[3].x=ptlBlt[2].x+bitmapinfoheaderIcon.cx;
ptlBlt[3].y=ptlBlt[2].y+bitmapinfoheaderIcon.cy;
                                        /* Blit monochrome bitmask */
GpiSetBitmap(hpsMemory, hbitmapPointer);
GpiBitBlt(hpsDestination, hpsMemory, sizeof(ptlBlt)/sizeof(ptlBlt[0]), ptlBlt, 0x22, BBO_IGNORE);
GpiDeleteBitmap(hpsMemory);
                                        /* Blit color bitmap into bitmask from last blit */
GpiSetBitmap(hpsMemory, hbitmapColor);
ptlBlt[2].y=0;
ptlBlt[3].y=ptlBlt[2].y+bitmapinfoheaderIcon.cy;
GpiBitBlt(hpsDestination, hpsMemory, sizeof(ptlBlt)/sizeof(ptlBlt[0]), ptlBlt, ROP_SRCPAINT, BBO_IGNORE);
                                        /* Cleanup */
GpiSetBitmap(hpsMemory, NULLHANDLE);
GpiDestroyPS(hpsMemory);
DevCloseDC(hdcIcon);
                                        /* Restore */
GpiSetColor(hpsDestination, lColorHps);
return(ulRc);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure blits a bitmap into a destination rectangle.                          *
 * Req:                                                                                 *
 *      hwndDestination The window handle to blit icon into its presentation space      *
 *      hpsDestination  The presentation space to blit icon into                        *
 *      hbitmapBitmap . The bitmap's handle to be blitted                               *
 *      prectlBitamp .. The target rectangle to blit bitmap into                        *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: NO_ERROR no error, PMERR_* otherwise               *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   BlitBitmap(HWND hwndDestination, HPS hpsDestination, HBITMAP hbitmapBitmap, RECTL *prectlBitmap)
{
                                        /* Bitmap implementing the icon */
BITMAPINFOHEADER2   bitmapinfoheaderBitmap;
HAB                 habWindow;          /* Anchor block of window to blit bitmap into */
HDC                 hdcIcon;            /* Memory DC and PS to blit bitmap from */
HPS                 hpsMemory;
SIZEL               sizelBitmap;
POINTL              ptlBlt[4];          /* Target and source points for blitting icon's bitmaps */
LONG                lColorHps;
ULONG               ulRc=NO_ERROR;

                                        /* Get info about the bitmap to blit */
bitmapinfoheaderBitmap.cbFix=16;
GpiQueryBitmapInfoHeader(hbitmapBitmap, &bitmapinfoheaderBitmap);
                                        /* Create a memory presentation space bitmap as the source
                                           to blit from */
habWindow=WinQueryAnchorBlock(hwndDestination);
hdcIcon=DevOpenDC(habWindow, OD_MEMORY, "*", 0L, NULL, NULLHANDLE);
sizelBitmap.cx=0;
sizelBitmap.cy=0;
hpsMemory=GpiCreatePS(habWindow, hdcIcon, &sizelBitmap, PU_PELS|GPIA_ASSOC|GPIT_MICRO);
                                        /* Target rectangle to blit bitmap to */
ptlBlt[0].x=prectlBitmap->xLeft; ptlBlt[0].y=prectlBitmap->yBottom;
ptlBlt[1].x=prectlBitmap->xRight; ptlBlt[1].y=prectlBitmap->yTop;
                                        /* Source point to blit bitmap from */
ptlBlt[2].x=0;
ptlBlt[2].y=0;
ptlBlt[3].x=ptlBlt[2].x+bitmapinfoheaderBitmap.cx;
ptlBlt[3].y=ptlBlt[2].y+bitmapinfoheaderBitmap.cy;
                                        /* Blit color bitmap */
GpiSetBitmap(hpsMemory, hbitmapBitmap);
GpiBitBlt(hpsDestination, hpsMemory, sizeof(ptlBlt)/sizeof(ptlBlt[0]), ptlBlt, ROP_SRCCOPY, BBO_IGNORE);
                                        /* Cleanup */
GpiDeleteBitmap(hpsMemory);
GpiDestroyPS(hpsMemory);
DevCloseDC(hdcIcon);
return(ulRc);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure disables a rectangle by drawing a halftoned background colored box    *
 * into the rectangle.                                                                  *
 * Req:                                                                                 *
 *      hpsDestination  The presentation space to blit icon into                        *
 *      prectlDestination                                                               *
 *                      The target rectangle to draw halftoned box into                 *
 *      ulBackgroundColor                                                               *
 *                      The background color to be used                                 *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: NO_ERROR no error, PMERR_* otherwise               *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   DisableWindowRect(HPS hpsDestination, RECTL *prectlDestination, ULONG ulBackgroundColor)
{
GpiSetPattern(hpsDestination, PATSYM_HALFTONE);
GpiSetColor(hpsDestination, ulBackgroundColor);
GpiMove(hpsDestination, (POINTL *)(&prectlDestination->xLeft));
GpiBox(hpsDestination, DRO_OUTLINEFILL, (POINTL *)(&prectlDestination->xRight), 0, 0);
GpiSetPattern(hpsDestination, PATSYM_SOLID);
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure returns a font attributs structure that contains the definition for   *
 * the font that is currently selected in a window. The font may be selected by         *
 * dropping a font from the font palette or by setting the PP_FONTNAMESIZE presentation *
 * parameter.                                                                           *
 * Req:                                                                                 *
 *      hwndWindow .... Window to query font set                                        *
 * Ret:                                                                                 *
 *      pfatWindowFont  An allocated FATTRS structure with the definition of the        *
 *                      window's current outline of bitmapped font                      *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
FATTRS      *QueryPresParamFont(HWND hwndWindow)
{
HPS         hpsWindow;
FONTMETRICS fmWindowFont;
FATTRS      *pfatWindowFont;

                                        /* Allocate and clear FATTRS structure */
pfatWindowFont=(FATTRS *)malloc(sizeof(FATTRS));
memset(pfatWindowFont, '\0', sizeof(FATTRS));
                                        /* Get the font that was set (dropped from the font palette, or
                                           set as presentation parameter) */
hpsWindow=WinGetPS(hwndWindow);
GpiQueryFontMetrics(hpsWindow, sizeof(FONTMETRICS), &fmWindowFont);
WinReleasePS(hpsWindow);
pfatWindowFont->usRecordLength=sizeof(FATTRS);
pfatWindowFont->fsSelection=0;
pfatWindowFont->lMatch=0;
strcpy(pfatWindowFont->szFacename, fmWindowFont.szFacename);
pfatWindowFont->idRegistry=fmWindowFont.idRegistry;
pfatWindowFont->usCodePage=fmWindowFont.usCodePage;
                                        /* The font set into the presentation space may either be a
                                           outline of bitmapped font */
if(fmWindowFont.fsDefn&FM_DEFN_OUTLINE)
    {
    pfatWindowFont->lMaxBaselineExt=fmWindowFont.lMaxBaselineExt;
    pfatWindowFont->lAveCharWidth=0;
    pfatWindowFont->fsType=0;
    pfatWindowFont->fsFontUse=FATTR_FONTUSE_OUTLINE|FATTR_FONTUSE_TRANSFORMABLE;
    }
else
    {
    pfatWindowFont->lMaxBaselineExt=fmWindowFont.lMaxBaselineExt;
    pfatWindowFont->lAveCharWidth=fmWindowFont.lAveCharWidth;
    pfatWindowFont->fsType=0;
    pfatWindowFont->fsFontUse=FATTR_FONTUSE_NOMIX;
    }
                                        /* Return the window's font definition to allow this font to
                                           be applied to another window too */
return(pfatWindowFont);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure sets a passed outline or bitmapped font (of a logical ID) into a      *
 * presentation space. The passed FATTRS font definition in pfatWindow is deallocated   *
 * after the font has been set.                                                         *
 * parameter.                                                                           *
 * Req:                                                                                 *
 *      hpsWindow ..... Presentation space to set font definition into                  *
 *      pfatWindowFont  An allocated FATTRS structure with the definition of the        *
 *                      window's current outline of bitmapped font                      *
 *      ullcidWindowFont                                                                *
 *                      Logical font ID of the font to be selected into the PS          *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: NO_ERROR no error, PMERR_* otherwise               *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG       SetPresParamFont(HPS hpsWindow, FATTRS *pfatWindow, ULONG ullcidWindowFont)
{
USHORT  usMaxBaselineExt;
SIZEF   sizefFont;

                                        /* For outline fonts the baseline is expected to be 0 */
usMaxBaselineExt=(USHORT)pfatWindow->lMaxBaselineExt;
if(pfatWindow->fsFontUse&FATTR_FONTUSE_OUTLINE)
    pfatWindow->lMaxBaselineExt=0;
                                        /* Delete previous logical font */
GpiDeleteSetId(hpsWindow, ullcidWindowFont);
                                        /* Create the logical font and select it into the presentation
                                           space */
GpiCreateLogFont(hpsWindow, NULL, ullcidWindowFont, pfatWindow);
GpiSetCharSet(hpsWindow, ullcidWindowFont);
sizefFont.cx=MAKEFIXED(usMaxBaselineExt, 0);
sizefFont.cy=MAKEFIXED(usMaxBaselineExt, 0);
GpiSetCharBox(hpsWindow, &sizefFont);
                                        /* Free allocated font attributes structure */
free(pfatWindow);
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure manipulates a copy of the current Window List contents.               *
 * Req:                                                                                 *
 *      hab ........... Anchor block handle of calling thread                           *
 *      ulTask ........ Request to apply on Window List copy in pHP->pWindowList        *
 *      mp1 ........... General purpose parameter                                       *
 * Ret:                                                                                 *
 *      ulRc .......... Result: Index within Window List copy                           *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
MRESULT         GetWindowList(HAB hab, ULONG ulTask, MPARAM mp1)
{
                                        /* Last Window List saved to be able to compare what has changed */
static SWBLOCK  *pWindowListPrevious=NULL;
                                        /* Number of last Window List entries (1 based) so subtract 1 when
                                           using as an index into the pHP->pWindowList[] array */
static LONG     lWindowListCountPrevious=0;
                                        /* Number of Window List entries (1 based) so subtract 1 when
                                           using as an index into the pHP->pWindowList[] array */
static LONG     lWindowListCount=0;
                                        /* Index in Window List */
LONG            lWindowListIndex=0;

if(ulTask==WINDOWLIST_FREE)
                                        /* Deallocate our resource */
    {
    if(pWindowListPrevious)
        {
        free(pWindowListPrevious);
        pWindowListPrevious=NULL;
        }
    if(pHP->pWindowList)
        {
        free(pHP->pWindowList);
        pHP->pWindowList=NULL;
        }
    lWindowListCountPrevious=lWindowListCount=0;
    return((MRESULT)0);
    }
if(ulTask==WINDOWLIST_QUERY)
    {
                                        /* Index (0-based) into last Window List and current Window List
                                           to detect changes */
    LONG            lWindowListIndexPrevious=-1;
    LONG            lWindowListIndexCurrent=-1;
                                        /* Cache access */
    SWENTRY         *pswentryPrevious=NULL;
    SWENTRY         *pswentryCurrent=NULL;

    CHECKPOINT(WTrd, 0x0121);
                                        /* During window destruction, the Window List may contain entries
                                           for already destroyed windows. In this case we count them to allow
                                           the caller to repeat the enumeration.
                                           Additionally to the indices in the Window Lists, we have to
                                           inform the SessionBar which entry (0-based) we are working with */

                                        /* Free the one previous to the previous one */
    if(pWindowListPrevious)
        free(pWindowListPrevious);
                                        /* If the SessionBar needs to be informed about the
                                           entries in the Window List, ensure that the current
                                           Window List gets compared to an empty previous
                                           Window List */
    if(pHP->ulRuntimeFlag & INITSESSIONBAR)
        {
        if(pHP->pWindowList)
            free(pHP->pWindowList);
        pHP->pWindowList=0;
        lWindowListCount=0;
        if(pHP->hwndSessionClient!=NULLHANDLE)
            pHP->ulRuntimeFlag&=(~INITSESSIONBAR);
        }
                                        /* Save previous one, as we now get the current one */
    pWindowListPrevious=pHP->pWindowList;
    lWindowListCountPrevious=lWindowListCount;
    pHP->pWindowList=NULL;
                                        /* Query the number of entries in window list */
    lWindowListCount=WinQuerySwitchList(hab, NULL, 0);
#ifdef  DEBUG_UTILITY
    if(lWindowListCount>lWindowListCountPrevious)
        printf("Utility: Windowlist %d -> %d\n", lWindowListCountPrevious, lWindowListCount);
#endif  /* DEBUG_UTILITY */
                                        /* Allocate space for window list (a SWBLOCK consists of a single
                                           ULONG and multiple SWENTRYs) */
    CHECKPOINT(WTrd, 0x0122);
    pHP->pWindowList=(PSWBLOCK)malloc(lWindowListCount=((lWindowListCount+1)*sizeof(SWENTRY)+sizeof(ULONG)));
                                        /* Get Window List contents */
    lWindowListCount=WinQuerySwitchList(hab, pHP->pWindowList, lWindowListCount);
                                        /* Now detect changes between last and current Window List. We have
                                           to start with the highest index as the enumeration of the Window
                                           List starts with the window on top of Z-order, but we want to
                                           start with the window at the bottom of Z-order (as new sessions
                                           are usually started into the foreground = top of Z-order) */
    CHECKPOINT(WTrd, 0x0123);
    for(lWindowListIndexPrevious=(lWindowListCountPrevious-1),
            lWindowListIndexCurrent=(lWindowListCount-1),
            lWindowListIndex=0;
        (lWindowListIndexPrevious!=-1) && (lWindowListIndexCurrent!=-1);
        lWindowListIndexPrevious--,
            lWindowListIndexCurrent--,
            lWindowListIndex++)
        {
                                        /* Find first visible and jumpable entry of current Window List */
        while(lWindowListIndexCurrent!=-1)
            {
            pswentryCurrent=&pHP->pWindowList->aswentry[lWindowListIndexCurrent];
                                        /* For us, a valid Window List entry must have a handle and be jumpable,
                                           visible and must have a process it is running in (we have to test for
                                           the process identifier, as at least under Warp 4  processes dying
                                           unexpectedly may leave dummy entries in the Window List) */
            if((pswentryCurrent->swctl.hwnd!=NULLHANDLE) &&
                (pswentryCurrent->swctl.fbJump==SWL_JUMPABLE) &&
                (pswentryCurrent->swctl.uchVisibility==SWL_VISIBLE) &&
                (pswentryCurrent->swctl.idProcess!=0))
                break;
            lWindowListIndexCurrent--;
            }
                                        /* Find first visible and jumpable entry of previous Window List */
        while(lWindowListIndexPrevious!=-1)
            {
            pswentryPrevious=&pWindowListPrevious->aswentry[lWindowListIndexPrevious];
            if((pswentryPrevious->swctl.hwnd!=NULLHANDLE) &&
                (pswentryPrevious->swctl.fbJump==SWL_JUMPABLE) &&
                (pswentryPrevious->swctl.uchVisibility==SWL_VISIBLE) &&
                (pswentryPrevious->swctl.idProcess!=0))
                break;
            lWindowListIndexPrevious--;
            }
        if((lWindowListIndexCurrent==-1) || (lWindowListIndexPrevious==-1))
            break;
                                        /* We now have one previous and one current entry that both are
                                           valid within the SessionBar. First let's see if both entries
                                           are not equal, in which case the previous SessionBar contents
                                           have to be changed */
        if(pswentryPrevious->hswitch!=pswentryCurrent->hswitch)
            {
                                        /* As the current and previous Window List do not match, change
                                           the previous to the current session in the SessionBar */
#ifdef  DEBUG_UTILITY
            printf("Utility: %s (%d) replaces %s (%d)\n",
                pswentryCurrent->swctl.szSwtitle,
                (int)pswentryCurrent->hswitch,
                pswentryPrevious->swctl.szSwtitle,
                (int)pswentryPrevious->hswitch);
#endif  /* DEBUG_UTILITY */
            WinPostMsg(pHP->hwndSessionClient, WM_SESSIONBARUPDATE,
                MPFROMLONG(lWindowListIndex), (MPARAM)pswentryCurrent->hswitch);
                                        /* We have handled both entries */
            continue;
            }
                                        /* Now as nothing has changed for that entry, continue */
        }
                                        /* We've compared all entries that are both in the previous and in
                                           the current Window List, so differences between both are a result
                                           of added/removed entries which we will tell the SessionBar to also
                                           update its state */
    CHECKPOINT(WTrd, 0x0124);
    if(lWindowListIndexPrevious!=-1)
        {
                                        /* The previous Window List had more entries, so some sessions must
                                           have been removed */
        for( /* NOP */ ; (lWindowListIndexPrevious!=-1); lWindowListIndexPrevious--, lWindowListIndex++)
            {
            pswentryPrevious=&pWindowListPrevious->aswentry[lWindowListIndexPrevious];
            if((pswentryPrevious->swctl.hwnd!=NULLHANDLE) &&
                (pswentryPrevious->swctl.fbJump==SWL_JUMPABLE) &&
                (pswentryPrevious->swctl.uchVisibility==SWL_VISIBLE) &&
                (pswentryPrevious->swctl.idProcess!=0))
                {
#ifdef  DEBUG_UTILITY
                printf("Utility: Last entry (%s ?) got removed\n",
                    pswentryPrevious->swctl.szSwtitle);
#endif  /* DEBUG_UTILITY */
                WinPostMsg(pHP->hwndSessionClient, WM_SESSIONBARUPDATE,
                    MPFROMLONG(lWindowListIndex), NULL);
                }
            else
                lWindowListIndex--;
            }
        }
    CHECKPOINT(WTrd, 0x0125);
    if(lWindowListIndexCurrent!=-1)
        {
                                        /* The current Window List had more entries, so some sessions must
                                           have been added */
        for( /* NOP */ ; lWindowListIndexCurrent!=-1; lWindowListIndexCurrent--, lWindowListIndex++)
            {
            pswentryCurrent=&pHP->pWindowList->aswentry[lWindowListIndexCurrent];
                                        /* A valid Window List entry for us must be jumpable, visible and
                                           must have a process it is running in (we have to test for the
                                           process identifier, as at least under Warp 4  processes dying
                                           unexpectedly may leave dummy entries in the Window List) */
            if((pswentryCurrent->swctl.fbJump==SWL_JUMPABLE) &&
                (pswentryCurrent->swctl.uchVisibility==SWL_VISIBLE) &&
                (pswentryCurrent->swctl.idProcess!=0))
                {
#ifdef  DEBUG_UTILITY
                printf("Utility: %s (%d) got added\n",
                    pswentryCurrent->swctl.szSwtitle, (int)pswentryCurrent->hswitch);
#endif  /* DEBUG_UTILITY */
                WinPostMsg(pHP->hwndSessionClient, WM_SESSIONBARUPDATE,
                    MPFROMLONG(lWindowListIndex), (MPARAM)pswentryCurrent->hswitch);
                }
            else
                lWindowListIndex--;
            }
        }
    CHECKPOINT(WTrd, 0x0126);
    return((MRESULT)NULLHANDLE);
    }
else
    {
                                        /* Return invalid pointer if we're not initialized */
    if(pHP->pWindowList==0)
        return((MRESULT)0);
                                        /* See what we should do */
    switch(ulTask)
    {
    case WINDOWLIST_NEXT:
                                        /* Query next Window List switch entry for QuickSwitch */
    case WINDOWLIST_PREVIOUS:
                                        /* Query previous Window List switch entry for QuickSwitch */
        {
        HWND    hwndSearch=HWNDFROMMP(mp1);
        LONG    lWindowListSave=0;

                                        /* Find the current window's Window List entry */
        for(lWindowListIndex=0; lWindowListIndex<lWindowListCount; lWindowListIndex++)
            if(pHP->pWindowList->aswentry[lWindowListIndex].swctl.hwnd==hwndSearch)
                break;
                                        /* If we didn't find it, use current active window's entry */
        if(lWindowListIndex==lWindowListCount)
            {
            hwndSearch=WinQueryActiveWindow(HWND_DESKTOP);
            for(lWindowListIndex=0; lWindowListIndex<lWindowListCount; lWindowListIndex++)
                if(pHP->pWindowList->aswentry[lWindowListIndex].swctl.hwnd==hwndSearch)
                    break;
            }
                                        /* We found the current active window's entry, now find the next
                                           frame window that can be possibly active, but check for wrap arounds */
        lWindowListSave=lWindowListIndex;
        while(TRUE)
            {
            if(ulTask==WINDOWLIST_NEXT)
                {
                --lWindowListIndex;
                                        /* If we wrapped around once, there is no window to switch to available,
                                           so return 0 */
                if(lWindowListSave==lWindowListIndex)
                    return((MRESULT)0);
                                        /* Wrap around */
                if(lWindowListIndex==-1)
                    lWindowListIndex=lWindowListCount-1;
                }
            else
                {
                ++lWindowListIndex;
                                        /* If we wrapped around once, there is no window to switch to available,
                                           so return 0 */
                if(lWindowListSave==lWindowListIndex)
                    return((MRESULT)0);
                                        /* Wrap around */
                if(lWindowListIndex>=lWindowListCount)
                    lWindowListIndex=0;
                }
                                        /* If we find a jumpable and visible Window List entry, return it */
            if((pHP->pWindowList->aswentry[lWindowListIndex].swctl.fbJump==SWL_JUMPABLE) &&
                (pHP->pWindowList->aswentry[lWindowListIndex].swctl.uchVisibility==SWL_VISIBLE))
                break;
            }
        return((MRESULT)(pHP->pWindowList->aswentry[lWindowListIndex].hswitch));
        }

    case WINDOWLIST_HANDLE:
                                        /* Find the Window List entry of a certain window */
        {
        HWND    hwndSearch=HWNDFROMMP(mp1);
        PID     pidSearch=0;
        TID     tidSearch=0;

                                        /* First try to find the Window List entry for the window
                                           handle directly */
        for(lWindowListIndex=0; lWindowListIndex<lWindowListCount; lWindowListIndex++)
            if(pHP->pWindowList->aswentry[lWindowListIndex].swctl.hwnd==hwndSearch)
                return((MRESULT)&(pHP->pWindowList->aswentry[lWindowListIndex]));
                                        /* If above didn't work, see if the window runs in the same
                                           process as a Window List entry */
        WinQueryWindowProcess(hwndSearch, &pidSearch, &tidSearch);
        for(lWindowListIndex=0; lWindowListIndex<lWindowListCount; lWindowListIndex++)
            {
            if(pHP->pWindowList->aswentry[lWindowListIndex].swctl.idProcess==pidSearch)
                return((MRESULT)&(pHP->pWindowList->aswentry[lWindowListIndex]));
            }
        return((MRESULT)0);
        }

    case WINDOWLIST_ACTIVEWINDOW:
                                        /* Get the active primary window to find its Window List switch handle */
        {
        HWND        hwndActive=NULLHANDLE;
        SWENTRY     *pswentryActiveWindow=NULL;
        HSWITCH     hswitchActiveWindow=NULLHANDLE;

        hwndActive=WinQueryActiveWindow(HWND_DESKTOP);
        pswentryActiveWindow=(SWENTRY *)GetWindowList(hab, WINDOWLIST_HANDLE, MPFROMHWND(hwndActive));
        if(pswentryActiveWindow)
            hswitchActiveWindow=pswentryActiveWindow->hswitch;
        return((MRESULT)hswitchActiveWindow);
        }

    case WINDOWLIST_FINDPROCESS:
        {
                                        /* First try to find the Window List entry for the process
                                           we are looking for. As we only return the program type
                                           it does not matter which window we find, because a single
                                           process can have only one program type */
        for(lWindowListIndex=0; lWindowListIndex<lWindowListCount; lWindowListIndex++)
            if(pHP->pWindowList->aswentry[lWindowListIndex].swctl.idProcess==(PID)mp1)
                return((MRESULT)pHP->pWindowList->aswentry[lWindowListIndex].swctl.bProgType);
                                        /* If we can't determine that assume we're running under the
                                           PM screen group */
        return((MRESULT)PROG_PM);
        }
    }
    }
                                        /* We do not expect to come here */
return((MRESULT)0);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure determines if a window fits in a rectangle, either completely (in-    *
 * cluding sizing border and titlebar in calculation) or at least the majority (by      *
 * using the window's size that would fit on current or next Virtual Desktop).          *
 * Req:                                                                                 *
 *      prectlWindow .. Pointer to the window's rectangle                               *
 *      prectlRectangle Pointer to the rectangle that may cover the window's rectangle  *
 *      ulDirection ... Flag containing the direction the Physical Desktop will be      *
 *                      moved within the Virtual Desktop if the window isn't be covered *
 *                      by the rectangle                                                *
 * Ret:                                                                                 *
 *      ulResult ...... Flag containing the relation between the window and rectangle   *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   IsWindowInRectangle(RECTL *prectlWindow, RECTL *prectlRectangle, ULONG ulDirection)
{
                                        /* We compare the window with the passed rectangle enlarged
                                           by the left and right, bottom and top border size and titlebar
                                           height (so that if the rectangle is the physical screen, the
                                           WPS fits in the screen's rectangle) */
LONG    lLeftRightBorder=WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
LONG    lLowerBorder=WinQuerySysValue(HWND_DESKTOP,  SV_CYSIZEBORDER);
LONG    lUpperBorder=lLowerBorder+WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
                                        /* Calculate center of window */
POINTL  pointlWindowCenter;
ULONG   ulResult=0;
LONG    lPixelOnCurrentDesktop;
LONG    lPixelOnNextDesktop;

pointlWindowCenter.x=(prectlWindow->xLeft+prectlWindow->xRight)>>1;
pointlWindowCenter.y=(prectlWindow->yBottom+prectlWindow->yTop)>>1;
                                        /* First see if the window's width fits within the rectangle */
if((prectlWindow->xLeft>=(prectlRectangle->xLeft-lLeftRightBorder)) &&
    (prectlWindow->xRight<=(prectlRectangle->xRight+lLeftRightBorder)))
    ulResult|=XWINDOW;
                                        /* See if the window's height fits within the rectangle */
if((prectlWindow->yBottom>=(prectlRectangle->yBottom-lLowerBorder)) &&
    (prectlWindow->yTop<=(prectlRectangle->yTop+lUpperBorder)))
    ulResult|=YWINDOW;
                                        /* When Physical Desktop is moving right, see if the next
                                           slide does better cover the majority of the window, and if
                                           the window is completely outside the Virtual Desktop */
if(ulDirection&MOVEXL)
    {
    lPixelOnCurrentDesktop=prectlRectangle->xRight-prectlWindow->xLeft;
    lPixelOnNextDesktop=prectlWindow->xRight-(prectlRectangle->xLeft+pHP->SlidingXFactor);
    if(lPixelOnCurrentDesktop>=lPixelOnNextDesktop)
        ulResult|=XCENTER;
    if(pointlWindowCenter.x>pHP->VirtualDesktopMax.x)
        ulResult|=XRIGHT;
    }
                                        /* When Physical Desktop is moving left, see if the next
                                           slide does better cover the majority of the window, and if
                                           the window is completely outside the Virtual Desktop */
if(ulDirection&MOVEXR)
    {
    lPixelOnCurrentDesktop=prectlWindow->xRight-prectlRectangle->xLeft;
    lPixelOnNextDesktop=(prectlRectangle->xRight-pHP->SlidingXFactor)-prectlWindow->xLeft;
    if(lPixelOnCurrentDesktop>=lPixelOnNextDesktop)
        ulResult|=XCENTER;
    if(pointlWindowCenter.x<pHP->VirtualDesktopMin.x)
        ulResult|=XLEFT;
    }
                                        /* When Physical Desktop is moving up, see if the next
                                           slide does better cover the majority of the window and if
                                           the window is completely outside the Virtual Desktop */
if(ulDirection&MOVEYD)
    {
    lPixelOnCurrentDesktop=prectlRectangle->yTop-prectlWindow->yBottom;
    lPixelOnNextDesktop=prectlWindow->yTop-(prectlRectangle->yBottom+pHP->SlidingYFactor);
    if(lPixelOnCurrentDesktop>=lPixelOnNextDesktop)
        ulResult|=YCENTER;
    if(pointlWindowCenter.y>pHP->VirtualDesktopMax.y)
        ulResult|=YTOP;
    }
                                        /* When Physical Desktop is moving down, see if the next
                                           slide does better cover the majority of the window and if
                                           the window is completely outside the Virtual Desktop */
if(ulDirection&MOVEYU)
    {
    lPixelOnCurrentDesktop=prectlWindow->yTop-prectlRectangle->yBottom;
    lPixelOnNextDesktop=(prectlRectangle->yTop-pHP->SlidingYFactor)-prectlWindow->yBottom;
    if(lPixelOnCurrentDesktop>=lPixelOnNextDesktop)
        ulResult|=YCENTER;
    if(pointlWindowCenter.y<pHP->VirtualDesktopMin.y)
        ulResult|=YBOTTOM;
    }
return(ulResult);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure manipulates an array of all child windows of HWND_DESKTOP queried by  *
 * WinBeginEnumWindows().                                                               *
 * Req:                                                                                 *
 *      hab ........... Anchor block handle of calling thread                           *
 *      ulTask ........ Request to apply on windows array in pHP->Windows               *
 *      mp1 ........... General purpose parameter                                       *
 * Ret:                                                                                 *
 *      mresult ....... Result: Index within windows array                              *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
MRESULT     GetDesktopWindows(HAB hab, ULONG ulTask, MPARAM mp1)
{
static ULONG    ulStructuresFreed=FALSE;

if(ulTask==DESKTOPWINDOWS_FREE)
                                        /* Deallocate our resources */
    {
    ulStructuresFreed=TRUE;
                                        /* Cleanup our copy of the Window List */
    GetWindowList(hab, WINDOWLIST_FREE, NULL);
                                        /* Clean our copy of HWND_DEKSTOP's child windows */
    if(pHP->Windows.pwdWindows)
        {
        free(pHP->Windows.pwdWindows);
        pHP->Windows.pwdWindows=NULL;
        }
    if(pHP->Windows.pswpWindows)
        {
        free(pHP->Windows.pswpWindows);
        pHP->Windows.pswpWindows=NULL;
        }
    pHP->Windows.lDesktop=(ULONG)-1;
    pHP->Windows.lWindowList=(ULONG)-1;
    pHP->Windows.lWindowLast=(ULONG)-1;
    pHP->Windows.lWindowPreviousLast=(ULONG)-1;
    return((MRESULT)0);
    }
if(ulTask==DESKTOPWINDOWS_QUERY)
                                        /* Query a new array of all child windows of HWND_DESKTOP from PM */
    {
    ULONG       ulWindowCount=0;        /* Number of PM windows returned by WinBeginEnumWindows() */
    ULONG       ulWindowCurrent=0;      /* Index in pHP->Windows.pwdWindows */
    HENUM       henumDesktop;           /* Window handle of WC_FRAME class Desktop */
    HWND        hwndApplication;        /* Window handles of enumerated application */
    SWENTRY     *pswentryCurrent=0;     /* Current pHP->Windows.WindowList[] array element */
    WINDOWDATA  *pwdCurrent=0;          /* Current pHP->Windows.pwdWindows[] array element */
    PID         pidCurrent;             /* Current pHP->Windows.pwdWindows[] array element process ID */
    TID         tidCurrent;             /* Current pHP->Windows.pwdWindows[] array element thread ID */
    MENUDATA    *pMD=0;                 /* Pointer to MENUDATA structure of corresponding window */
    UCHAR       *pucStickyWindow=0;     /* Pointer to current Sticky Window */
    UCHAR       *pucStickyWindowNext=0; /* Pointer to next Sticky Window */
    UCHAR       ucStickyWindow[CCHMAXPATH];

    CHECKPOINT(WTrd, 0x0110);
    if(ulStructuresFreed==TRUE)
        {
#ifdef  DEBUG
        printf("Utility: %ld Fatal, windows already freed\n", __LINE__);
        DosBeep(1500, 2000);
#endif  /* DEBUG */
        return((MRESULT)-1);
        }
                                        /* Query the current Window List */
    CHECKPOINT(WTrd, 0x0120);
    GetWindowList(hab, WINDOWLIST_QUERY, 0);
    CHECKPOINT(WTrd, 0x0130);
    pHP->Windows.lDesktop=-1;           /* Set to -1 if we don't find the Desktop's name */
                                        /* Set to -1 if we don't find the Window List's name */
    pHP->Windows.lWindowList=-1;
                                        /* Enumerate all descendants of HWND_DESKTOP,
                                           which are the frame windows seen on Desktop,
                                           but not having necessarily the class WC_FRAME
                                           to be able to allocate an array for all windows */
    henumDesktop=WinBeginEnumWindows(HWND_DESKTOP);
    while((hwndApplication=WinGetNextWindow(henumDesktop))!=NULLHANDLE)
        ulWindowCount++;
    CHECKPOINT(WTrd, 0x0140);
    WinEndEnumWindows(henumDesktop);
    CHECKPOINT(WTrd, 0x0150);
                                        /* As we may be preempted between this enumeration and the next one
                                           we'll be on the sure side, if we add a little space in advance,
                                           because there is a chance, that during our preemption, some windows
                                           get created */
    ulWindowCount+=10;
                                        /* Free array of windows and and allocate new one
                                           (ulWindowCount is 1 based) */
    if(pHP->Windows.pwdWindows!=0)
        free(pHP->Windows.pwdWindows);
    pHP->Windows.lWindowPreviousLast=pHP->Windows.lWindowLast;
    pHP->Windows.pwdWindows=(WINDOWDATA *)malloc(sizeof(WINDOWDATA)*ulWindowCount);
                                        /* Free array of window positions and and allocate new one
                                           (ulWindowCount is 1 based) */
    if(pHP->Windows.pswpWindows!=0)
        free(pHP->Windows.pswpWindows);
    pHP->Windows.pswpWindows=(SWP *)malloc(sizeof(SWP)*ulWindowCount);
    memset(pHP->Windows.pwdWindows, '\0', (sizeof(WINDOWDATA)*ulWindowCount));
    memset(pHP->Windows.pswpWindows, '\0', (sizeof(SWP)*ulWindowCount));
                                        /* Adjust ulWindowCount to be 0 based (when using it in the context
                                           of an array indes) */
    if(ulWindowCount!=0)
        --ulWindowCount;
                                        /* Enumerate all descendants of HWND_DESKTOP,
                                           which are the frame windows seen on Desktop,
                                           but not having necessarily the class WC_FRAME, beginning
                                           with index 0 */
    ulWindowCurrent=0;
    henumDesktop=WinBeginEnumWindows(HWND_DESKTOP);
    CHECKPOINT(WTrd, 0x0160);
    while((hwndApplication=WinGetNextWindow(henumDesktop))!=NULLHANDLE)
        {
                                        /* Ensure that we don't fill more array elements than we allocated.
                                           As we may be preempted between the first and this enumeration,
                                           there may be a difference (greater than the difference of 5 windows,
                                           we've taken into account previously), we have to test for */
        if(ulWindowCurrent>ulWindowCount)
            {
            --ulWindowCurrent;
            break;
            }
        pwdCurrent=&pHP->Windows.pwdWindows[ulWindowCurrent];
        pwdCurrent->pswpWindow=&pHP->Windows.pswpWindows[ulWindowCurrent];
                                        /* Assume window visible and a moveable frame */
        pwdCurrent->ulStatus|=(SHAREDHSWITCH|VISIBLE|FRAMECLASS);
                                        /* Get window's size and position */
        if(WinQueryWindowPos(hwndApplication, pwdCurrent->pswpWindow)==FALSE)
            continue;
                                        /* If we'll not find an entry in the Window List set
                                           it to default empty string */
        pwdCurrent->ucWindowTitle[0]='\0';
                                        /* If we don't find an entry set to 0 */
        pwdCurrent->hswitchWindow=0;
                                        /* Assume no icon image */
        pwdCurrent->hptrIcon=0;
                                        /* If window handle of frame window and window handle in
                                           Window List are equal, copy the name of the entry from the
                                           Window List */
        pswentryCurrent=(SWENTRY *)GetWindowList(hab, WINDOWLIST_HANDLE, MPFROMHWND(hwndApplication));
        CHECKPOINT(WTrd, 0x0170);
        if(pswentryCurrent!=0)
            {
            strcpy(pwdCurrent->ucWindowTitle, pswentryCurrent->swctl.szSwtitle);
                                        /* Get the switch handle */
            pwdCurrent->hswitchWindow=pswentryCurrent->hswitch;
                                        /* Get the icon from the switch entry */
            pwdCurrent->hptrIcon=pswentryCurrent->swctl.hwndIcon;
                                        /* If we found the switch handle in the Window List and it
                                           represends just this window, save that fact */
            if(hwndApplication==pswentryCurrent->swctl.hwnd)
                pwdCurrent->ulStatus&=(~SHAREDHSWITCH);

            }
                                        /* The following call sometimes blocks the thread,
                                           can only recovered by a restart of PC/2
                                           if(!pwdCurrent->hptrIcon)
                                               pwdCurrent->hptrIcon=(HPOINTER)WinSendMsg(hwndApplication, WM_QUERYICON, NULL, NULL);
                                               pwdCurrent->hptrIcon=(HPOINTER)WinSendMsg(pswentryCurrent->swctl.hwnd, WM_QUERYICON, NULL, NULL);
                                         */
                                        /* Get window's visibility */
        CHECKPOINT(WTrd, 0x0180);
        if(pwdCurrent->pswpWindow->fl & SWP_HIDE)
            pwdCurrent->ulStatus &= (~VISIBLE);
        else
            if(!pwdCurrent->hptrIcon)
                pwdCurrent->hptrIcon=(HPOINTER)WinSendMsg(hwndApplication, WM_QUERYICON, NULL, NULL);
        CHECKPOINT(WTrd, 0x0190);
                                        /* Get window's class name */
        WinQueryClassName(hwndApplication, CLASSNAMESIZE,
            pwdCurrent->ucClassName);
        CHECKPOINT(WTrd, 0x01A0);
                                        /* If it is an minimized icon text class window
                                           treat it as an invisible one */
        if(!strcmp(pwdCurrent->ucClassName, "#32765"))
            pwdCurrent->ulStatus &= (~VISIBLE);
                                        /* If it is a menu treat it as an invisible one and
                                           not being a frame window */
        if(!strcmp(pwdCurrent->ucClassName, "#4"))
            pwdCurrent->ulStatus &= (~(VISIBLE|FRAMECLASS));
                                        /* If it is a combobox treat it as an invisible one and
                                           not being a frame window */
        if(!strcmp(pwdCurrent->ucClassName, "#7"))
            pwdCurrent->ulStatus &= (~(VISIBLE|FRAMECLASS));
        CHECKPOINT(WTrd, 0x01A1);
                                        /* Get window's titlebar. Note: If this function references
                                           the window of another process, the memory must be shared
                                           between both processes, otherwise a memory fault can (and
                                           from my experience will) occur. */
        WinQueryWindowProcess(hwndApplication, &pidCurrent, &tidCurrent);
        CHECKPOINT(WTrd, 0x01A2);
        if(DosGiveSharedMem(pHP->pucWindowText, pidCurrent, PAG_READ|PAG_WRITE)==NO_ERROR)
            {
            CHECKPOINT(WTrd, 0x01A3);
            WinQueryWindowText(hwndApplication, sizeof(pwdCurrent->ucPgmTitle),
                pHP->pucWindowText);
            CHECKPOINT(WTrd, 0x01A4);
                                        /* For testing allow use of some signatures in titlebars
                                           cause PC/2 to simulate a problem */
            if(strstr(pHP->pucWindowText, "MagicMagicMagic Trap"))
                _interrupt(3);
            if(strstr(pHP->pucWindowText, "MagicMagicMagic Hang"))
                {
                extern HMTX    hmtxHang;
            
#ifdef  DEBUG
                printf("Utility: Requesting HANG semaphore %08X\n", (int)hmtxHang);
#endif  /* DEBUG */
                DosRequestMutexSem(hmtxHang, SEM_INDEFINITE_WAIT);
#ifdef  DEBUG
                printf("Utility: Received HANG semaphore -> incorrect\n");
#endif  /* DEBUG */
                DosReleaseMutexSem(hmtxHang);
                }
            strcpy(pwdCurrent->ucPgmTitle, pHP->pucWindowText);
            }
                                        /* If we found the index of the Window List save it */
        CHECKPOINT(WTrd, 0x01B0);
        if(strstr(pwdCurrent->ucPgmTitle, pHP->ucWindowListName))
            {
            static ULONG    ulWindowListVisiblePrevious=FALSE;
            ULONG           ulWindowListVisibleCurrent;
            SWP             swpWindowList;
            LONG            lTemp;

#ifdef  DEBUG_UTILITY
            printf("Utility: Found Window List \"%s\" (\"%s\")\n", pwdCurrent->ucPgmTitle, pwdCurrent->ucWindowTitle);
#endif  /* DEBUG_UTILITY */
            pHP->hwndWindowList=hwndApplication;
            pHP->Windows.lWindowList=ulWindowCurrent;
                                        /* Don't move the Window List out of the Physical
                                           Desktop when switching between Virtual Desktops
                                           (even if it would be moveable, but this
                                           does not make sense for the Window List) */
            pwdCurrent->ulStatus &= (~FRAMECLASS);
                                        /* Ensure Window List on Physical Desktop. Test only when
                                           Window List becomes visible to avoid hogging the CPU */
            ulWindowListVisibleCurrent=(ULONG)WinIsWindowVisible(hwndApplication);
            if((ulWindowListVisiblePrevious==FALSE) && (ulWindowListVisibleCurrent==TRUE))
                {
                WinQueryWindowPos(hwndApplication, &swpWindowList);
                lTemp=swpWindowList.x+swpWindowList.cx;
                if(swpWindowList.x<0)
                    swpWindowList.x=0;
                else if(lTemp>pHP->DesktopSize.x)
                    swpWindowList.x-=(lTemp-pHP->DesktopSize.x);
                lTemp=swpWindowList.y+swpWindowList.cy;
                if(swpWindowList.y<0)
                    swpWindowList.y=0;
                else if(lTemp>pHP->DesktopSize.y)
                    swpWindowList.y-=(lTemp-pHP->DesktopSize.y);
                swpWindowList.fl=SWP_MOVE|SWP_NOADJUST;
                WinSetMultWindowPos(WinQueryAnchorBlock(hwndApplication), &swpWindowList, 1);
                                        /* Generate an additional dummy key that the PreAccelHook()
                                           will filter and therefore reset that the CTRL key's
                                           break code follows the make code and will not display
                                           PC/2's Popup Menu (assuming that CTRL+ESC was pressed).
                                           So why is this required? Well, OS/2 filters the ESC
                                           key when being part of CTRL+ESC by a device driver, so
                                           our hook doesn't see the ESC key, just only the CTRL key.
                                           If the CTRL key is clicked in a way that the make code
                                           is followed by a break code, the hook does detect this and
                                           will display the Popup Menu. As OS/2 can't know that CTRL
                                           may be part of CTRL+ESC, the CTRL key does not get filtered
                                           and voila, the Window List pops up, but gets replaced by
                                           PC/2's Popup Menu immediately. But when we send a dummy key,
                                           the hook will detect that the CTRL keys make code is not
                                           followed by a break code, and not display the Popup Menu.
                                           Unfortunately the time window is very small, as the Window
                                           List gets displayed after the break code of the ESC key,
                                           and all previous CTRL key make codes get filtered after
                                           the ESC key's break code by OS/2 */
                WinSendMsg(pHP->hwndClient, WM_CHAR,
                    MPFROMSH2CH((KC_SCANCODE|KC_DEADKEY), 1, 0xFF), MPFROM2SHORT(0xFF, 0));
                }
            ulWindowListVisiblePrevious=ulWindowListVisibleCurrent;
            }
                                        /* If we found the index of the WPS (Desktop) save it */
        if(strstr(pwdCurrent->ucPgmTitle, pHP->ucDesktopName))
            {
#ifdef  DEBUG_UTILITY
            printf("Utility: Found Desktop \"%s\" (\"%s\")\n", pwdCurrent->ucPgmTitle, pwdCurrent->ucWindowTitle);
#endif  /* DEBUG_UTILITY */
            pHP->Windows.lDesktop=ulWindowCurrent;
                                        /* Move Desktop only when required to do so */
            if(!(pHP->ulStatusFlag1 & (MOVEDESKTOP | EXPANDWPS)))
                pwdCurrent->ulStatus &= (~FRAMECLASS);
            }
                                        /* If we found a "shadow" window created by seamless
                                           WIN-OS2 ignore it. E.g. a dropdown menu is painted
                                           into a PM window named "Seamless" to allow the menu
                                           to be displayed outside any area covered by the WIN-OS2
                                           application too */
        if(strstr(pwdCurrent->ucPgmTitle, "Seamless"))
            pwdCurrent->ulStatus &= (~(VISIBLE|FRAMECLASS));
                                        /* If we found PC/2, or any of PC/2's dialogs save it,
                                           after hiding them from the Overview window, and after marking
                                           these windows not being a FRAMECLASS (which prevents theses
                                           windows from being moved out of the Virtual Desktop, keeping
                                           it on the Physical Desktop) */
        if(hwndApplication==pHP->hwndFrame)
            pwdCurrent->ulStatus &= (~(VISIBLE|FRAMECLASS));
        if(hwndApplication==pHP->hwndSessionFrame)
            pwdCurrent->ulStatus &= (~(VISIBLE|FRAMECLASS));
        if(hwndApplication==pHP->hwndSwitchFrame)
            pwdCurrent->ulStatus &= (~(VISIBLE|FRAMECLASS));
        if(hwndApplication==pHP->hwndBubbleHelpFrame)
            pwdCurrent->ulStatus &= (~(VISIBLE|FRAMECLASS));
        if((strstr(pwdCurrent->ucPgmTitle, "PC/2")) && (pHP->ProcessId==pidCurrent))
            pwdCurrent->ulStatus &= (~(VISIBLE|FRAMECLASS));
                                        /* Check for Sticky Windows. The array is formatted as:
                                           [[[StickyWindow1]\r\nStickyWindow2]...]\r\n
                                           which means that an empty list contains at least \r\n,
                                           thus, an pointer to an Sticky Windows entry is only valid
                                           if it isn't a NULL (to be exact NULL+1) pointer, does not
                                           point to \r and not to \0 (which should occur, as we stop
                                           checking after finding the last \r\n, but to be sure...) */
        pucStickyWindow=pHP->ucStickyWindows;
        pucStickyWindowNext=strchr(pucStickyWindow, '\n')+1;
        for( ; (pucStickyWindow!=(UCHAR *)1) && (*pucStickyWindow!='\r') && (*pucStickyWindow!='\0'); )
            {
            memset(ucStickyWindow, '\0', sizeof(ucStickyWindow));
            strncpy(ucStickyWindow, pucStickyWindow, (ULONG)(pucStickyWindowNext-pucStickyWindow)-2);
            if((strstr(pwdCurrent->ucWindowTitle, ucStickyWindow)) ||
                (strstr(pwdCurrent->ucWindowTitle, ucStickyWindow)) ||
                (strstr(pwdCurrent->ucClassName, ucStickyWindow)))
                {
                pwdCurrent->ulStatus &= (~FRAMECLASS);
                break;
                }
            pucStickyWindow=pucStickyWindowNext;
            pucStickyWindowNext=strchr(pucStickyWindow, '\n')+1;
            }
                                        /* Search if current window is contained in PC/2's
                                           Popup-Menu */
        pMD=SearchTitle(pHP->pPopupMenu, pwdCurrent);
        CHECKPOINT(WTrd, 0x01C0);
        if(pMD!=NULL)
            {
                                        /* If found copy flag because we need to determine
                                           if window should be moved on virtual Desktops
                                           and how it should be restored when hotkeying to it */
            pwdCurrent->SwpFlag=pMD->SwpFlag;
            memcpy(&(pwdCurrent->KeyData), &(pMD->KeyData), sizeof(KEYDATA));
            if(pMD->SwpFlag & SWP_MOVEWINDOW)
                {
                SWP     swp;
                USHORT  usSwp;
                                        /* If window was only invisible because of the movement
                                           make it visible again */
                if(pMD->SwpFlag & SWP_MOVEWINDOWVISIBLE)
                    usSwp=SWP_MOVE | SWP_SHOW | SWP_NOADJUST;
                else
                    usSwp=SWP_MOVE | SWP_NOADJUST;
                                        /* If this flag is set we found an application's window
                                           the first time after invokation and we have to reposition
                                           it and reset the flag */
                pMD->SwpFlag&=(~(SWP_MOVEWINDOW | SWP_MOVEWINDOWVISIBLE));
                                        /* Now calculate window's position on Virtual Desktops and
                                           move it to its absolute (relative to logical 0|0) position */
                swp.fl=usSwp;
                swp.x=pMD->InitXPos-pHP->VirtualDesktopPos.x;
                swp.y=pMD->InitYPos-pHP->VirtualDesktopPos.y;
                swp.hwndInsertBehind=HWND_TOP;
                swp.hwnd=hwndApplication;
                                        /* As PM doesn't support Virtual Desktops natively, even
                                           when SWP_NOADJUST is requested, some windows are
                                           stubborn when being moved out of the screen, so let's
                                           get on the save side by preemting ourselfes and repeating
                                           the operation */
                CHECKPOINT(WTrd, 0x01D0);
                WinSetMultWindowPos(hab, &swp, 1);
                DosSleep(0);
                WinSetMultWindowPos(hab, &swp, 1);
                CHECKPOINT(WTrd, 0x01E0);
                }
            }
        else
            {
                                        /* If we don't find any indice that this window is also
                                           available in PC/2's Popup-Menu, no Hotkey will be
                                           available for this PM window */
            pwdCurrent->SwpFlag=0;
            memset(&(pwdCurrent->KeyData), 0, sizeof(KEYDATA));
            }
                                        /* The following call sometimes blocks the thread,
                                           can only recover by a restart of PC/2
                                           if((!pwdCurrent->hptrIcon) && (pwdCurrent->ulStatus & VISIBLE))
                                               pwdCurrent->hptrIcon=(HPOINTER)WinSendMsg(pswentryCurrent->swctl.hwnd, WM_QUERYICON, NULL, NULL);
                                               pwdCurrent->hptrIcon=(HPOINTER)WinSendMsg(hwndApplication, WM_QUERYICON, NULL, NULL);
                                         */
                                        /* Address current element in array allocated previously,
                                           to cache accesses */
        ulWindowCurrent++;
        CHECKPOINT(WTrd, 0x01F0);
        }
    CHECKPOINT(WTrd, 0x01F1);
    WinEndEnumWindows(henumDesktop);    /* End enumeration */
                                        /* Check for the System Error message boxes detected in the
                                           in the WinSendMsg() hook */
#ifdef  DEBUG_UTILITY
    if(pHP->ulRuntimeFlag & SYSTEMERRORACTIVE)
        printf("PM Error popup active\n");
    else
        printf("PM Error popup inactive\n");
#endif  /* DEBUG_UTILITY */
    pHP->Windows.lWindowLast=(ulWindowCurrent-1);
    CHECKPOINT(WTrd, 0x01F2);
    }
else
    {
    switch(ulTask)
    {
    case DESKTOPWINDOWS_DESKTOP:
                                        /* Return the WINDOWDATA structure of Desktop (WPS) or 0 */
        if(pHP->Windows.lDesktop!=-1)
            return((MRESULT)&(pHP->Windows.pwdWindows[pHP->Windows.lDesktop]));
        else
            return((MRESULT)0);

    case DESKTOPWINDOWS_WINDOWLIST:
                                        /* Return the WINDOWDATA structure of the Window List or 0 */
        if(pHP->Windows.lWindowList!=-1)
            return((MRESULT)&(pHP->Windows.pwdWindows[pHP->Windows.lWindowList]));
        else
            return((MRESULT)0);

    case DESKTOPWINDOWS_INDEX:
                                        /* Return the WINDOWDATA structure of a window whose index
                                           was passed in mp1 or 0 */
        {
        LONG    lIndex=LONGFROMMP(mp1);

        if(lIndex<=pHP->Windows.lWindowLast)
            return((MRESULT)&(pHP->Windows.pwdWindows[lIndex]));
        else
            return((MRESULT)0);
        }

    case DESKTOPWINDOWS_HANDLE:
                                        /* Return the WINDOWDATA structure of a window whose window handle
                                           was passed in mp1 or 0 */
        {
        HWND        hwndIndex=HWNDFROMMP(mp1);
        WINDOWDATA  *pwdCurrent=0;      /* WINDOWDATA structure of current Desktop child window */
        LONG        lWindowIndex=0;     /* Index in WINDOWDATA structure */

                                        /* Find window whose handle match with the passed one */
        for(lWindowIndex=0; lWindowIndex<=pHP->Windows.lWindowLast; lWindowIndex++)
            {
            pwdCurrent=GetDesktopWindows(hab, DESKTOPWINDOWS_INDEX, MPFROMLONG(lWindowIndex));
                                        /* Should not occur, but does as we don't protect access with
                                           a semaphore */
            if((!pwdCurrent) || (!pwdCurrent->pswpWindow))
                return((MRESULT)0);
            if(pwdCurrent->pswpWindow->hwnd==hwndIndex)
                break;
            }
                                        /* Return found WINDOWDATA entry or 0 if the window handle could
                                           not be found */
        if(lWindowIndex<=pHP->Windows.lWindowLast)
            return((MRESULT)pwdCurrent);
        else
            return((MRESULT)0);
        }

    case DESKTOPWINDOWS_HOTKEY:
                                        /* Return the WINDOWDATA structure of a window for which the
                                           Hotkey passed in mp1 is set or 0 */
        {
                                        /* Get key code */
        USHORT      usFlags=SHORT1FROMMP(mp1);
                                        /* Get ASCII key value */
        USHORT      usCh=SHORT2FROMMP(mp1);
        WINDOWDATA  *pwdCurrent=0;      /* WINDOWDATA structure of current Desktop child window */
        LONG        lWindowIndex=0;     /* Index in WINDOWDATA structure */

                                        /* Find match between actual hotkey pressed and
                                           any (available) window this hotkey is defined for */
        for(lWindowIndex=0; lWindowIndex<=pHP->Windows.lWindowLast; lWindowIndex++)
            {
            pwdCurrent=GetDesktopWindows(hab, DESKTOPWINDOWS_INDEX, MPFROMLONG(lWindowIndex));
            if((pwdCurrent->KeyData.usFlags==usFlags) &&
                (pwdCurrent->KeyData.usCh==usCh))
                break;
            }
                                        /* Return found WINDOWDATA entry or 0 if the window, the Hotkey
                                           was defined for is not running */
        if(lWindowIndex<=pHP->Windows.lWindowLast)
            return((MRESULT)pwdCurrent);
        else
            return((MRESULT)0);
        }

    case DESKTOPWINDOWS_CLICK:
                                        /* Return the WINDOWDATA structure of a window on whose representation
                                           in the Overview window the user clicked or 0 */
        {
        LONG        lClickX, lClickY;   /* Pointer position on Overview window during click */
        WINDOWDATA  *pwdCurrent=0;      /* WINDOWDATA structure of current Desktop child window */
        LONG        lWindowIndex=0;     /* Index in WINDOWDATA structure */

                                        /* Get position of mouse button 1 down */
        lClickX=(ULONG)(SHORT1FROMMP(mp1));
        lClickY=(ULONG)(SHORT2FROMMP(mp1));
        lClickX=((float)(lClickX-pHP->ptlOrigin.x))/pHP->fScaleX;
        lClickY=((float)(lClickY-pHP->ptlOrigin.y))/pHP->fScaleY;

                                        /* Loop for all windows, from topmost to bottommost, on
                                           Overview window */
        for(lWindowIndex=0; lWindowIndex<=pHP->Windows.lWindowLast; lWindowIndex++)
            {
            pwdCurrent=GetDesktopWindows(hab, DESKTOPWINDOWS_INDEX, MPFROMLONG(lWindowIndex));
                                        /* Ignore invisible windows */
            if(!(pwdCurrent->ulStatus & VISIBLE)) continue;
            if((pwdCurrent->pswpWindow->x<=lClickX) &&
                (pwdCurrent->pswpWindow->x+pwdCurrent->pswpWindow->cx>=lClickX) &&
                (pwdCurrent->pswpWindow->y<=lClickY) &&
                (pwdCurrent->pswpWindow->y+pwdCurrent->pswpWindow->cy>=lClickY))
                {                       /* We have found one */
                return((MRESULT)pwdCurrent);
                }
            }
        return((MRESULT)0);
        }

    case DESKTOPWINDOWS_SWITCHTO:
    case DESKTOPWINDOWS_MOVETO:
    case DESKTOPWINDOWS_SWITCHTOWND:
                                        /* Switch and activate the window, or only switch to the window
                                           whose HWND is passed by mp1, return the window's WINDOWDATA */
        {
                                        /* Session ID of current fullscreen session */
        ULONG       ulFSSession;
                                        /* WINDOWDATA structure of window to switch to */
        WINDOWDATA  *pwdCurrent=0;
                                        /* Save the window we're switching to, as WM_DESKTOPMOVE reenumerates
                                           all Desktop frame child windows (to get their new position after
                                           switching to the Virtual Desktop the window is located at). We than
                                           use the window's handle to find its then new WINDOWDATA to activate it */
        HWND        hwndCurrent=HWNDFROMMP(mp1);
        RECTL       rectlDesktopPos;    /* Rectangle the Physical Desktop covers on the Virtual Desktop, relative
                                           to (0|0) */
        RECTL       rectlWindowPos;     /* Rectangle of the window relative to (0|0) not lower left corner of
                                           screen */
        ULONG       ulRectangleFlag=0;  /* Result from IsWindowInRectangle() for current Physical Desktop
                                           position */
        LONG        lSlidingXFactor=0;  /* Slide in x direction in pixels */
        LONG        lSlidingYFactor=0;  /* Slide in y direction in pixels */
        LONG        lDiff;              /* Temporary slide in pixel */
        USHORT      usfl;               /* Flags for WinSetMultWindowPos() */

                                        /* Don't switch to or move to a window if temporarily suspended
                                           (e.g. when putting a window on bottom of Z-Order by clicking
                                           with mouse button 2 on titlebar while a PC/2 window is on top
                                           of Z-Order - which would cause a WM_FOCUSCHANGE to that window) */
        if(pHP->ulRuntimeFlag & SUSPENDSWITCHACTIVE)
            return(NULL);
                                        /* If the current fullscreen session is not the PM session
                                           (1) itself but another fullscreen session (2...15), don't switch 
                                           away from that fullscreen session */
        DosQuerySysInfo(QSV_FOREGROUND_FS_SESSION, QSV_FOREGROUND_FS_SESSION, &ulFSSession, sizeof(ulFSSession));
        if((ulFSSession>=2) && (ulFSSession<=15))
            return(NULL);
                                        /* Find the details of the window we want to switch to */
        pwdCurrent=(WINDOWDATA *)GetDesktopWindows(hab, DESKTOPWINDOWS_HANDLE, MPFROMHWND(hwndCurrent));
        if(pwdCurrent==NULL)
            return(NULL);
                                        /* If the window is currently minimized, switch to the Virtual
                                           Desktop it is going to be restored and not to the one that
                                           covers the minimized/hidden position (as the position is -1,-1
                                           we would switch to the left- and bottommost Virtual Desktop */
        if(pwdCurrent->pswpWindow->fl & SWP_MINIMIZE)
            {
            pwdCurrent->pswpWindow->x=(LONG)WinQueryWindowUShort(hwndCurrent, QWS_XRESTORE);
            pwdCurrent->pswpWindow->y=(LONG)WinQueryWindowUShort(hwndCurrent, QWS_YRESTORE);
            pwdCurrent->pswpWindow->cx=(LONG)WinQueryWindowUShort(hwndCurrent, QWS_CXRESTORE);
            pwdCurrent->pswpWindow->cy=(LONG)WinQueryWindowUShort(hwndCurrent, QWS_CYRESTORE);
            }

                                        /* The Physical Desktop (that is the screen we're currently looking at) is
                                           at a certain position within the Virtual Desktop */
        rectlDesktopPos.xLeft=rectlDesktopPos.xRight=pHP->VirtualDesktopPos.x;
        rectlDesktopPos.xRight+=pHP->DesktopSize.x;
        rectlDesktopPos.yBottom=rectlDesktopPos.yTop=pHP->VirtualDesktopPos.y;
        rectlDesktopPos.yTop+=pHP->DesktopSize.y;
                                        /* The window is relative the the lower left corner of the Physical Desktop,
                                           which itself is relative to the absolute (0|0) of the Virtual Desktop, so
                                           to compare the window's rectangle with the Physical Desktop's rectangle,
                                           we have to convert the window's PM coordinates to coordinates on the
                                           Virtual Desktop */
        rectlWindowPos.xLeft=rectlWindowPos.xRight=rectlDesktopPos.xLeft+pwdCurrent->pswpWindow->x;
        rectlWindowPos.xRight+=pwdCurrent->pswpWindow->cx;
        rectlWindowPos.yBottom=rectlWindowPos.yTop=rectlDesktopPos.yBottom+pwdCurrent->pswpWindow->y;
        rectlWindowPos.yTop+=pwdCurrent->pswpWindow->cy;
                                        /* The Physical Desktop must be contained within the limits of
                                           the Virtual Desktop while moving rightwards. We keep trying to move the
                                           Physical Desktop rightwards to see, if it can cover the window's rectangle
                                           either completely, or at least the major part (measured by comparing the
                                           window's rectangle center) */
        ulRectangleFlag=0;
        while(rectlDesktopPos.xRight<pHP->VirtualDesktopMax.x)
            {
                                        /* See if the window and window's center would fit within the
                                           rectangle were the Physical Desktop may be moved to */
            ulRectangleFlag=IsWindowInRectangle(&rectlWindowPos, &rectlDesktopPos, MOVEXL);
                                        /* If the window fits completely into the rectangle let's move
                                           the Physical Desktop to the rectangle. Windows that are
                                           larger than the Physical Desktop this will never match, so
                                           test also for the window's center too */
            if(ulRectangleFlag&(XWINDOW|XCENTER))
                break;
                                        /* Adjust the pixel counter to move windows left */
            lSlidingXFactor-=pHP->SlidingXFactor;
                                        /* The Physical Desktop moves right inside the Virtual Desktop */
            rectlDesktopPos.xLeft+=pHP->SlidingXFactor;
            rectlDesktopPos.xRight+=pHP->SlidingXFactor;
                                        /* Ensure that the Physical Desktop can be only moved as right as
                                           the Virtual Desktop is defined */
            lDiff=rectlDesktopPos.xRight-pHP->VirtualDesktopMax.x;
            if(lDiff>0)
                {
                lSlidingXFactor+=lDiff;
                break;
                }
            }
                                        /* Try the same leftwards if rightwards didn't succeed and the window
                                           is not outside the Virtual Desktop (right side) */
        if(!(ulRectangleFlag&(XWINDOW|XRIGHT))) while(rectlDesktopPos.xLeft>pHP->VirtualDesktopMin.x)
            {
            ulRectangleFlag=IsWindowInRectangle(&rectlWindowPos, &rectlDesktopPos, MOVEXR);
            if(ulRectangleFlag&(XWINDOW|XCENTER))
                break;
            lSlidingXFactor+=pHP->SlidingXFactor;
            rectlDesktopPos.xLeft-=pHP->SlidingXFactor;
            rectlDesktopPos.xRight-=pHP->SlidingXFactor;
            lDiff=rectlDesktopPos.xLeft-pHP->VirtualDesktopMin.x;
            if(lDiff<0)
                {
                lSlidingXFactor+=lDiff;
                break;
                }
            }
                                        /* Reset all flags except the ones set when the window's rectangle
                                           is (mostly) outside the Virtual Desktop */
        ulRectangleFlag&=XYMASK;
                                        /* Make the same upwards */
        while(rectlDesktopPos.yTop<pHP->VirtualDesktopMax.y)
            {
            ulRectangleFlag=IsWindowInRectangle(&rectlWindowPos, &rectlDesktopPos, MOVEYD);
            if(ulRectangleFlag&(YWINDOW|YCENTER))
                break;
            lSlidingYFactor-=pHP->SlidingYFactor;
            rectlDesktopPos.yBottom+=pHP->SlidingYFactor;
            rectlDesktopPos.yTop+=pHP->SlidingYFactor;
            lDiff=rectlDesktopPos.yTop-pHP->VirtualDesktopMax.y;
            if(lDiff>0)
                {
                lSlidingYFactor+=lDiff;
                break;
                }
            }
                                        /* Try the same downwards if upwards didn't succeed and the
                                           window is not outside the Virtual Desktop (top side) */
        if(!(ulRectangleFlag&(YWINDOW|YTOP))) while(rectlDesktopPos.yBottom>pHP->VirtualDesktopMin.y)
            {
            ulRectangleFlag=IsWindowInRectangle(&rectlWindowPos, &rectlDesktopPos, MOVEYU);
            if(ulRectangleFlag&(YWINDOW|YCENTER))
                break;
            lSlidingYFactor+=pHP->SlidingYFactor;
            rectlDesktopPos.yBottom-=pHP->SlidingYFactor;
            rectlDesktopPos.yTop-=pHP->SlidingYFactor;
            lDiff=rectlDesktopPos.yBottom-pHP->VirtualDesktopMin.y;
            if(lDiff<0)
                {
                lSlidingYFactor+=lDiff;
                break;
                }
            }
                                        /* Move physical Desktop up, but not over the
                                           top border of the virtual Desktop until
                                           the hotkeyed window's vertiacal position comes
                                           onto the Desktop */
        if(lSlidingXFactor || lSlidingYFactor)
                                        /* Now move the windows and activate window after move. Prior
                                           version 1.80 WinPostMsg() was used to switch to a Virtual
                                           Desktop. This worked unless the window we switched to with
                                           the Hotkey was minmized. In this case the window got
                                           activated first by the WinSetMultWindowPos() call, before
                                           the Virtual Desktop got switched by WM_DESKTOPMOVE, causing
                                           the minimized window expanding to the current Desktop, with
                                           a switch to the Virtual Desktop the minimized window was
                                           afterwards.
                                           Version 1.90+ uses WinSendMsg() to switch to the Virtual
                                           Desktop first, before restoring the minimized window. */
            WinSendMsg(pHP->hwndWorkingThread, WM_DESKTOPMOVE, MPFROMLONG(lSlidingXFactor), MPFROMLONG(lSlidingYFactor));
                                        /* As above WM_DESKTOPMOVE has started a new enumeration of all Desktop
                                           frame child windows, our pointer is no longer valid, but luckily as we
                                           are, we can retrieve the new one with the window's handle we've saved.
                                           (This is only true if we switch to a different Virtual Desktop, but
                                           let's stay at the safe side and assume a invalid pointer) */
        pwdCurrent=(WINDOWDATA *)GetDesktopWindows(hab, DESKTOPWINDOWS_HANDLE, MPFROMHWND(hwndCurrent));
        if((pwdCurrent) && (ulTask!=DESKTOPWINDOWS_MOVETO))
            {
                                        /* Get flags WinSetMultWindowPos() */
            usfl=(pwdCurrent->SwpFlag & ~SWP_NOMOVE) | SWP_SHOW;
                                        /* If no switch handle found activate window through
                                           WinSetMultWindowPos() else via WinSwitchToProgram which
                                           also switches to different screen groups */
            if(!(pwdCurrent->hswitchWindow) || (ulTask==DESKTOPWINDOWS_SWITCHTOWND))
                usfl|=(SWP_ACTIVATE | SWP_ZORDER);
            pwdCurrent->pswpWindow->fl=usfl;
            pwdCurrent->pswpWindow->hwndInsertBehind=HWND_TOP;
            WinSetMultWindowPos(hab, pwdCurrent->pswpWindow, 1);
            if((pwdCurrent->hswitchWindow) && (ulTask!=DESKTOPWINDOWS_SWITCHTOWND))
                WinSwitchToProgram(pwdCurrent->hswitchWindow);
            }
        return((MRESULT)pwdCurrent);
        }

    case PM_ERRORWINDOWS_QUERY:
                                        /* Query the PM Herd Error and the System Error window handles,
                                           as when these windows are active, we must not switch to
                                           Virtual Desktops, because once these windows are active there
                                           is no way to get back to the Desktop these windows reside on
                                           as they hook away all messages systemwide. Also don't add the
                                           Titlebar Smarticons to them */
        {
        PID         pidObject;          /* HWND_OBJECT's process and thread id */
        TID         tidObject;
        PID         pidObjectChild;     /* HWND_OBJECT's child window process and thread id */
        TID         tidObjectChild;
        HENUM       henumObject;        /* HWND_OBJECT enumeration handle */
        HWND        hwndObjectChild;    /* Window handle of current HWND_OBJECT child */
        UCHAR       ucClassName[32];    /* Window class e.g. #1 for WC_FRAME */
        CLASSINFO   classinfoWindow;    /* Class info of current HWND_OBJECT child */

                                        /* Query HWND_OBJECT's window process */
        WinQueryWindowProcess(WinQueryObjectWindow(HWND_DESKTOP), &pidObject, &tidObject);
                                        /* Enumerate all child windows of HWND_OBJECT */
        henumObject=WinBeginEnumWindows(HWND_OBJECT);
        while((hwndObjectChild=WinGetNextWindow(henumObject))!=NULLHANDLE)
            {
                                        /* See if the current HWND_OBJECT child window runs in the
                                           process of HWND_OBJECT (PM) */
            WinQueryWindowProcess(hwndObjectChild, &pidObjectChild, &tidObjectChild);
            if(pidObject==pidObjectChild)
                {
                                        /* Get the HWND_OBJECT child window's data */
                WinQueryClassName(hwndObjectChild, sizeof(ucClassName), ucClassName);
                WinQueryClassInfo(WinQueryAnchorBlock(hwndObjectChild), ucClassName, &classinfoWindow);
                if((!strcmp(ucClassName, "#1") || (classinfoWindow.flClassStyle&CS_FRAME)))
                    {
                                        /* If the child window is a frame window and running in
                                           HWND_OBJECT's (PM's) window process, it must be the
                                           PM Hard Error or System Error window */
                    WinQueryClassName(WinWindowFromID(hwndObjectChild, FID_CLIENT), sizeof(ucClassName), ucClassName);
                    if(!strcmp(ucClassName, "PM Hard Error"))
                        {
#ifdef  DEBUG_UTILITY
                        printf("Utility: Found PM Hard Error %08X\n", (int)hwndObjectChild);
#endif  /* DEBUG_UTILITY */
                        pHP->hwndPMHardError=hwndObjectChild;
                        }
                    else
                        {
#ifdef  DEBUG_UTILITY
                        printf("Utility: Found System Error %08X\n", (int)hwndObjectChild);
#endif  /* DEBUG_UTILITY */
                        pHP->hwndSystemError=hwndObjectChild;
                        }
                    }
                }
            }
        WinEndEnumWindows(henumObject);

        }
    }
    }
return((MRESULT)0);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure queries all windows and returns the one on top of Z-Order at the      *
 * current screen which not belongs to PC/2.                                            *
 * Req:                                                                                 *
 * Ret:                                                                                 *
 *      hwndWindow .... Result: Window on Z-Order top on current Desktop which not      *
 *                      belongs to PC/2, or NULLHANDLE if none was found                *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
HWND    GetZOrderTop(void)
{
HENUM   henumDesktop;
ULONG   ulFound;
HWND    hwndPreviousWindow;
HWND    hwndNextWindow;
SWP     swpNextWindow;

                                        /* Enumerate Desktop frame window childs until our SessionBar
                                           frame window was the previous one and then switch to and
                                           activate the current one */
hwndPreviousWindow=hwndNextWindow=HWND_DESKTOP;
henumDesktop=WinBeginEnumWindows(HWND_DESKTOP);
while((hwndNextWindow=WinGetNextWindow(henumDesktop))!=NULLHANDLE)
    {
    if((hwndPreviousWindow==pHP->hwndSessionFrame) ||
        ((hwndPreviousWindow==HWND_DESKTOP) && (hwndNextWindow!=pHP->hwndSessionFrame)))
        {
                                        /* Don't switch to the Overview window, don't switch to
                                           icon texts, and also don't switch invisible,
                                           minimized, disabled and fullscreen session (which are
                                           minimized on PM) windows */
        if(hwndNextWindow!=pHP->hwndFrame)
            {
            ULONG   ulStyle;
            UCHAR   ucClassname[MAXNAMEL+4];

            WinQueryClassName(hwndNextWindow, sizeof(ucClassname), ucClassname);
            if(!strcmp(ucClassname, "#32765"))
                continue;
            ulStyle=WinQueryWindowULong(hwndNextWindow, QWL_STYLE);
            if((ulStyle & (WS_VISIBLE|WS_DISABLED|WS_MINIMIZED))==WS_VISIBLE)
                {
                                        /* If the window is not on the current Destop, don't
                                           switch to it */
                WinQueryWindowPos(hwndNextWindow, &swpNextWindow);
                if((swpNextWindow.x<-(WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER))) ||
                    (swpNextWindow.y<-(WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER))) ||
                    (swpNextWindow.x+swpNextWindow.cx>pHP->swpScreen.cx+WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER)) ||
                    (swpNextWindow.y+swpNextWindow.cy>pHP->swpScreen.cy+WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER)))
                    {
#ifdef  DEBUG_SB
                    printf("Utility: WM_ZORDER skipped %08X\n", (int)hwndNextWindow);
#endif  /* DEBUG_SB */
                    continue;
                    }
                ulFound=TRUE;
                break;
                }
            }
        continue;
        }
    hwndPreviousWindow=hwndNextWindow;
    }
WinEndEnumWindows(henumDesktop);
if(ulFound==TRUE)
    return(hwndNextWindow);
else
    return(NULLHANDLE);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure reads the date/time settings and returns an allocated string of the   *
 * current date/time. DosQueryCtryInfo() would give us this data, but when changing the *
 * preferenced from the WPS Country object it does not get reflected before a reboot,   *
 * while reading from the OS2.INI file it does get reflected immediately.               *
 * Req:                                                                                 *
 *      ulInitialize .. FALSE/TRUE Initialize and return datetime/return datetime       *
 * Ret:                                                                                 *
 *      pucDateTime ... NULL/pointer to allocated string                                *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
UCHAR   *GetDateTimeNLS(ULONG ulInitialize)
{
static PMNATIONAL   pmnationalCurrent;
static PMNATIONAL   *ppmnationalCurrent=NULL;
ULONG               ulTemp;
UCHAR               *pucDateTime=NULL;
DATETIME            datetimeCurrent;

if((ulInitialize==TRUE) || (ppmnationalCurrent==NULL))
    {
    ppmnationalCurrent=&pmnationalCurrent;
    memset(ppmnationalCurrent, 0, sizeof(PMNATIONAL));
                                        /* Get data from OS2.INI */
    ulTemp=sizeof(ULONG);
    if(PrfQueryProfileData(HINI_USERPROFILE, "PM_National", "iDate", &ppmnationalCurrent->ulDateFmt, &ulTemp)==FALSE)
        ppmnationalCurrent->ulDateFmt=1;
    ppmnationalCurrent->ulDateFmt&=0x0000000F;
    ulTemp=sizeof(ULONG);
    if(PrfQueryProfileData(HINI_USERPROFILE, "PM_National", "iTime", &ppmnationalCurrent->ulTimeFmt, &ulTemp)==FALSE)
        ppmnationalCurrent->ulTimeFmt=1;
    ppmnationalCurrent->ulTimeFmt&=0x0000000F;
    ulTemp=sizeof(UCHAR);
    if(PrfQueryProfileData(HINI_USERPROFILE, "PM_National", "sDate", &ppmnationalCurrent->ucDateSeparator, &ulTemp)==FALSE)
        ppmnationalCurrent->ucDateSeparator='.';
    ulTemp=sizeof(UCHAR);
    if(PrfQueryProfileData(HINI_USERPROFILE, "PM_National", "sTime", &ppmnationalCurrent->ucTimeSeparator, &ulTemp)==FALSE)
        ppmnationalCurrent->ucTimeSeparator=':';
    ulTemp=sizeof(UCHAR[3]);
    if(PrfQueryProfileData(HINI_USERPROFILE, "PM_National", "s1159", &ppmnationalCurrent->ucAMFmt, &ulTemp)==FALSE)
        strcpy(ppmnationalCurrent->ucAMFmt, "AM");
    ppmnationalCurrent->ucAMFmt[3]='\0';
    ulTemp=sizeof(UCHAR[3]);
    if(PrfQueryProfileData(HINI_USERPROFILE, "PM_National", "s2359", &ppmnationalCurrent->ucPMFmt, &ulTemp)==FALSE)
        strcpy(ppmnationalCurrent->ucPMFmt, "PM");
    ppmnationalCurrent->ucPMFmt[3]='\0';
    }
if(ulInitialize==FALSE)
    {
    UCHAR   ucFieldSeparator[3];

    DosGetDateTime(&datetimeCurrent);
    pucDateTime=malloc(32);
    memset(pucDateTime, 0, 32);
    if(pHP->ulStatusFlag2 & DEFAULTDATETIMESB)
        {
                                        /* If the user selected the default date/time format to
                                           be displayed, then format it according to my choice */
        sprintf(pucDateTime, "%.3s. %2d.%.3s.%04d %02d:%02d:%02d",
            pucWeekDay[datetimeCurrent.weekday],
            datetimeCurrent.day, pucMonth[datetimeCurrent.month-1], datetimeCurrent.year,
            datetimeCurrent.hours, datetimeCurrent.minutes, datetimeCurrent.seconds);
        }
    else
        {
                                        /* If the user did not select to get the default
                                           date/time format to be displayed, derive the format
                                           from the Country WPS Object selection */
        if(ppmnationalCurrent->ulDateFmt==0)
            {
            sprintf(&pucDateTime[0], "%02d%c%02d%c%04d ",
                datetimeCurrent.month, ppmnationalCurrent->ucDateSeparator,
                datetimeCurrent.day, ppmnationalCurrent->ucDateSeparator,
                datetimeCurrent.year);
            }
        else if(ppmnationalCurrent->ulDateFmt==1)
            {
            sprintf(&pucDateTime[0], "%02d%c%02d%c%04d ",
                datetimeCurrent.day, ppmnationalCurrent->ucDateSeparator,
                datetimeCurrent.month, ppmnationalCurrent->ucDateSeparator,
                datetimeCurrent.year);
            }
        else if(ppmnationalCurrent->ulDateFmt==2)
            {
            sprintf(&pucDateTime[0], "%04d%c%02d%c%02d ",
                datetimeCurrent.year, ppmnationalCurrent->ucDateSeparator,
                datetimeCurrent.month, ppmnationalCurrent->ucDateSeparator,
                datetimeCurrent.day);
            }
        else if(ppmnationalCurrent->ulDateFmt==3)
            {
            sprintf(&pucDateTime[0], "%04d%c%02d%c%02d ",
                datetimeCurrent.year, ppmnationalCurrent->ucDateSeparator,
                datetimeCurrent.day, ppmnationalCurrent->ucDateSeparator,
                datetimeCurrent.month);
            }
        if(ppmnationalCurrent->ulTimeFmt==0)
            {
            if(datetimeCurrent.hours<13)
                strcpy(ucFieldSeparator, ppmnationalCurrent->ucAMFmt);
            else
                {
                strcpy(ucFieldSeparator, ppmnationalCurrent->ucPMFmt);
                datetimeCurrent.hours-=12;
                }
            sprintf(&pucDateTime[11], "%02d%c%02d%c%02d %2s",
                datetimeCurrent.hours, ppmnationalCurrent->ucTimeSeparator,
                datetimeCurrent.minutes, ppmnationalCurrent->ucTimeSeparator,
                datetimeCurrent.seconds,
                ucFieldSeparator);
            }
        else if(ppmnationalCurrent->ulTimeFmt==1)
            {
            sprintf(&pucDateTime[11], "%02d%c%02d%c%02d",
                datetimeCurrent.hours, ppmnationalCurrent->ucTimeSeparator,
                datetimeCurrent.minutes, ppmnationalCurrent->ucTimeSeparator,
                datetimeCurrent.seconds);
            }
        }
    }
return(pucDateTime);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure tries to replace the %BOOTDRIVE%, %PC2% and %CDDRIVE% macros with the *
 * actual contents those macros should have (e.g. C: and C:\PC2 respectively). The      *
 * destination string may not be larger than CCHMAXPATH. If no pucEnvironment is        *
 * specified, then only %PC2%, %BOOTDRIVE% or %CDDRIVE% macros will be handled.         *
 * Req:                                                                                 *
 *      pucDestination                                                                  *
 *                      Character string where expanded macro is written at             *
 *      pucSource ..... Character string where macros are searched for                  *
 *      pucEnvironment  Character string to the session's environment which is in the   *
 *                      form EnvVar1=Value1\0EnvVar2=Value2...EnvVarN=ValueN\0\0        *
 * Ret:                                                                                 *
 *      ulCharsReplaced NULL/number of characters replaced                              *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   ExpandMacros(UCHAR *pucDestination, UCHAR *pucSource, UCHAR *pucEnvironment)
{
UCHAR   *pucEnvironmentSave=pucEnvironment;
UCHAR   *pucTemp;
UCHAR   *pucMacroBegin=0;
UCHAR   *pucMacroEnd=0;
UCHAR   *pucReplace=0;
ULONG   ulCharsReplaced=0;

if((pucDestination==NULL) || (pucSource==NULL) || (strlen(pucSource)==0))
    return(0);
                                        /* Just copy the string in any case */
strcpy(pucDestination, pucSource);
                                        /* Ensure that we are positioned at a %-sign, as this
                                           one is the starting point to search for macros. The
                                           macro must terminate with a %-sign too */
pucMacroBegin=strchr(pucSource, '%');
if(pucMacroBegin==NULL)
    {
    strcpy(pucDestination, pucSource);
    return(0);
    }
pucMacroEnd=strchr(pucMacroBegin+1, '%');
if(pucMacroEnd!=NULL)
    pucReplace=strchr(pucDestination, '%');
while(pucMacroEnd!=NULL)
    {
    if(!strnicmp(pucMacroBegin, BOOTDRIVE, BOOTDRIVELEN-1))
        {
                                        /* For the %BOOTDRIVE% macro, just set the boot
                                           drive e.g. C: and add the remaining source string */
        *pucReplace++=pHP->ucBootDrive;
        *pucReplace++=':';
        ulCharsReplaced+=2;
        strcpy(pucReplace++, &pucMacroBegin[BOOTDRIVELEN-1]);
                                        /* Check for next macro to be replaced */
        pucMacroBegin=strchr(pucMacroEnd+1, '%');
        if(pucMacroBegin==NULL)
            break;
        pucMacroEnd=strchr(pucMacroBegin+1, '%');
        pucReplace=strchr(pucDestination, '%');
        continue;
        }
    else if(!strnicmp(pucMacroBegin, PC2, PC2LEN-1))
        {
                                        /* For the %PC2% macro, just set the path PC/2 was
                                           loaded from and append the remaining source string */
        if((strlen(pucDestination)+strlen(pHP->ucPathDLL)-PC2LEN+1)>CCHMAXPATH)
            break;
        strcpy(pucReplace, pHP->ucPathDLL);
        ulCharsReplaced+=strlen(pHP->ucPathDLL);
        strcpy(pucReplace, &pucMacroBegin[PC2LEN-1]);
                                        /* Check for next macro to be replaced */
        pucMacroBegin=strchr(pucMacroEnd+1, '%');
        if(pucMacroBegin==NULL)
            break;
        pucMacroEnd=strchr(pucMacroBegin+1, '%');
        pucReplace=strchr(pucDestination, '%');
        continue;
        }
    else if(!strnicmp(pucMacroBegin, CDDRIVE, CDDRIVELEN-1))
        {
                                        /* For the %CDDRIVE% macro, just set the first CD-ROM
                                           drive e.g. D: and add the remaining source string */
        *pucReplace++=pHP->ucCDDrive1;
        *pucReplace++=':';
        ulCharsReplaced+=2;
        strcpy(pucReplace++, &pucMacroBegin[CDDRIVELEN-1]);
                                        /* Check for next macro to be replaced */
        pucMacroBegin=strchr(pucMacroEnd+1, '%');
        if(pucMacroBegin==NULL)
            break;
        pucMacroEnd=strchr(pucMacroBegin+1, '%');
        pucReplace=strchr(pucDestination, '%');
        continue;
        }
                                        /* If Environment was specified, check if any EnvironmentVariable
                                           matches the macro specified */ 
    else if(pucEnvironment!=NULL)
        {
        if(!strnicmp(pucEnvironment, pucMacroBegin+1, (pucMacroEnd-pucMacroBegin)-1)) 
            {
                                        /* For any EnvironmentVariable found, replace the Makro
                                           by the Value of the EnvironmentVariable and then append
                                           the rest of the source string */
            pucTemp=strchr(pucEnvironment, '=')+1;
            if((strlen(pucDestination)+strlen(pucTemp)-(pucMacroEnd-pucMacroBegin)+1)>CCHMAXPATH)
                break;
            strcpy(pucReplace, pucTemp);
            ulCharsReplaced+=strlen(pucMacroBegin);
            strcat(pucReplace, pucMacroEnd+1);
                                        /* We have to rescan with the first EnvironmentVariable because
                                           there may be multiple macros in the source string */
            pucEnvironment=pucEnvironmentSave;
                                        /* Check for next macro to be replaced */
            pucMacroBegin=strchr(pucMacroEnd+1, '%');
            if(pucMacroBegin==NULL)
                break;
            pucMacroEnd=strchr(pucMacroBegin+1, '%');
            pucReplace=strchr(pucDestination, '%');
            continue;
            }
                                        /* Check for next EnvironmentVariable */
        pucEnvironment=strchr(pucEnvironment, '\0');
        pucEnvironment++;
        if(*pucEnvironment=='\0')
            break;
        }
                                        /* If the macro can't be resolved, stop trying */
    else
        break;
    }
return(ulCharsReplaced);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure makes backup copies of PC2.INI, PC2.CFG and PC2.ENV,                  *
 * Req:                                                                                 *
 *      hwndOwner ..... Window handle to display message box for                        *
 *      ulRequested ... TRUE if user requested a backup, FALSE otherwise                *
 * Ret:                                                                                 *
 *      ulReturnCode .. NO_ERROR if success, error otherwise                            *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   CopySetupFiles(HWND hwndOwner, ULONG ulRequested)
{
ULONG   ulReturnCode=NO_ERROR;
ULONG   ulDoBackup=ulRequested;
UCHAR   ucTargetFile[CCHMAXPATH];
UCHAR   *pucTemp;

                                        /* If user didn't request a backup, but some changes have
                                           accumulated, then advise user to do so */
if((ulRequested==FALSE) && (pHP->ulConfigurationChanges>10))
    {
    if((USR_ERR(hwndOwner, HELP_PC2BACKUP, MB_QUERY|MB_YESNO|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
        "You have made some significant changes, do you want to run the recommended "\
        "automatic backup of your PC/2 configuration files now?")==MBID_YES))
        ulDoBackup=TRUE;
    }
                                        /* If changes should be backuped now, just do it */
if(ulDoBackup==TRUE)
    {
                                        /* Backup PC2.INI */
    strcpy(ucTargetFile, pHP->ucPathDLL);
    strcat(ucTargetFile, "\\PC2-Ini.Bak");
    pucTemp=strrchr(ucTargetFile, '-')+1;
    ulReturnCode|=DosCopy(pHP->ucFilenameINI, ucTargetFile, DCPY_EXISTING);
                                        /* Backup PC2.CFG or replacement */
    strncpy(pucTemp, "Cfg", sizeof("Cfg")-1);
    ulReturnCode|=DosCopy(pHP->ucFilenameProfile, ucTargetFile, DCPY_EXISTING);
                                        /* Backup PC2.ENV or replacement */
    strncpy(pucTemp, "Env", sizeof("Env")-1);
    ulReturnCode|=DosCopy(pHP->ucFilenameEnvironment, ucTargetFile, DCPY_EXISTING);
                                        /* Depending on the outcome of the backup, give the
                                           user feedback */
    if(ulReturnCode!=NO_ERROR)
        {
        DOS_ERR(ulReturnCode, hwndOwner, HELP_PC2BACKUP, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Unexpected problem while backing up your configuration - continuing...");
        }
    else
        {
        pHP->ulConfigurationChanges=0;
        if(ulRequested==TRUE)
            USR_ERR(hwndOwner, HELP_PC2BACKUP, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Your current configuration files have been backuped into PC2-INI.BAK, PC2-ENV.BAK "\
                "and PC2-CFG.BAK - continuing...");
        }
    }
return(ulReturnCode);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure ensures that the \r\n separated strings are unique and removes        *
 * duplicates.                                                                          *
 * Req:                                                                                 *
 *      pucStrings .... A sequence of strings separated by \r\n                         *
 * Ret:                                                                                 *
 *      ulReturnCode .. NO_ERROR if success, error otherwise                            *
 * Ref:                                                                                 *
 *      pucStrings .... The contents may be modified, but is can only get shorter       *
\*--------------------------------------------------------------------------------------*/
ULONG   EnsureUniqueStrings(UCHAR *pucStrings)
{
ULONG   ulReturnCode=NO_ERROR;
UCHAR   *pucSource;
UCHAR   *pucSourceCopy;
UCHAR   *pucTarget;
UCHAR   *pucTargetCopy;
UCHAR   *pucTemp;
ULONG   ulSourceLen;
ULONG   ulTargetLen;

if(pucStrings==NULL)
    return(NO_ERROR);
                                        /* The strings are in a sequence of:
                                           string-1\r\nstring-2\r\n...string-n\r\n
                                           where n can even be 0 (that is a single \r\n is passed).
                                           We now loop through all string-n and check if all strings
                                           string-m where n!=m are unique (that is not duplicate
                                           or substrings */
pucSource=pucStrings;
for(pucSource=pucStrings; (pucSource!=(UCHAR *)1) && (*pucSource!='\0'); )
    {
    for(pucTarget=strchr(pucSource, '\n')+1; (pucTarget!=(UCHAR *)1) && (*pucTarget!='\0'); )
        {
                                        /* Copy source string */
        pucTemp=strchr(pucSource, '\n')+1;
        ulSourceLen=(ULONG)(pucTemp-pucSource)-2;
        pucSourceCopy=calloc(ulSourceLen+1, sizeof(UCHAR));
        strncpy(pucSourceCopy, pucSource, ulSourceLen);
                                        /* Same for target string */
        pucTemp=strchr(pucTarget, '\n')+1;
        ulTargetLen=(ULONG)(pucTemp-pucTarget)-2;
        pucTargetCopy=calloc(ulTargetLen+1, sizeof(UCHAR));
        strncpy(pucTargetCopy, pucTarget, ulTargetLen);
                                        /* Either the source or target string is longer, we take the
                                           shorter one as the substring the longer one must match with */
        if(ulSourceLen<=ulTargetLen)
            {
                                        /* If source is a substring of target then replace target by
                                           what follows target */
            if(strstr(pucTargetCopy, pucSourceCopy))
                strcpy(pucTarget, pucTemp);
            else
                pucTarget=strchr(pucTarget, '\n')+1;
            }
        else
            {
                                        /* If target is a substring of source then replace source by
                                           what follows source */
            if(strstr(pucSourceCopy, pucTargetCopy))
                {
                pucTemp=strchr(pucSource, '\n')+1;
                strcpy(pucSource, pucTemp);
                pucTarget=strchr(pucSource, '\n')+1;
                }
            else
                pucTarget=strchr(pucTarget, '\n')+1;
            }
        free(pucSourceCopy);
        free(pucTargetCopy);
        }
    pucSource=strchr(pucSource, '\n')+1;
    }
return(ulReturnCode);
}

