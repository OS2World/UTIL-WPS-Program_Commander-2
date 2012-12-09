/***********************************************************************\
 *                               Error.c                               *
 *             Copyright (C) by Stangl Roman, 1992, 2000               *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed.                                                            *
 *                                                                     *
 * Requires: Error.c    The routing General_Error                      *
 *           Error.h    The include-file that defines the macros       *
 *                      GEN_ERR, DOS_ERR, USR_ERR                      *
 *                                                                     *
 * Error is a general errorhandler for OS/2 2.0 PM programmer to easy  *
 * program development. The routine PM_Error displays:                 *
 *      ErrMsg          The message that hints the error reason        *
 *      pszErrMod       The module containing the error                *
 *      lErrLine        The sourcecode line, that contains the error   *
 *      Error           The error returned by an OS/2 API              *
 * The routine requires the following parameters passed:               *
 *      HAB hab         The anchor block handle                        *
 *      HWND hwndOwner  The owner windowhandle of the message box      *
 *      ULONG ulWindow  The message box window ID, which is the ID of  *
 *                      an entry in the online help                    *
 *      ULONG ulStyle   The message box style                          *
 *      UCHAR *pucMsg   An optional message that describes where the   *
 *                      error occured                                  *
 *      PSZ pszErrMod   The pointer to the name of the module _FILE_   *
 *      LONG lErrLine   The pointer to the sourcecodeline __LINE__     *
 *                                                                     *
 * This routine displays a message box, with the style specified in    *
 * ulStyle. The message box automatically returns the pushbutton       *
 * selected by the user. OS/2 APIs get called to determine the reason  *
 * of the OS/2 (probably) internal error.                              *
 *                                                                     *
 * The routine Dos_Error displays:                                     *
 *      ErrMsg          The message that hints the error reason        *
 *      pszErrMod       The module containing the error                *
 *      lErrLine        The sourcecode line, that contains the error   *
 *      Error           The error returned by an OS/2 API              *
 * The routine requires the following parameters passed:               *
 *      ULONG ulRc      The returncode of an OS/2 API                  *
 *      HWND hwndOwner  The owner windowhandle of the message box      *
 *      ULONG ulWindow  The message box window ID, which is the ID of  *
 *                      an entry in the online help                    *
 *      ULONG ulStyle   The message box style                          *
 *      UCHAR *pucMsg   An optional message that describes where the   *
 *                      error occured                                  *
 *      PSZ pszErrMod   The pointer to the name of the module _FILE_   *
 *      LONG lErrLine   The pointer to the sourcecodeline __LINE__     *
 *                                                                     *
 * This routine displays a message box, with the style specified in    *
 * ulStyle. The message box automatically returns the pushbutton       *
 * selected by the user. The return code of an OS/2 API is displayed.  *
 *                                                                     *
 * The routine User_Error displays:                                    *
 *      ErrMsg          The message that hints the error reason        *
 *      pszErrMod       The module containing the error                *
 *      lErrLine        The sourcecode line, that contains the error   *
 *      Error           The error indicated by the user                *
 * The routine requires the following parameters passed:               *
 *      HWND hwndOwner  The owner windowhandle of the message box      *
 *      ULONG ulWindow  The message box window ID, which is the ID of  *
 *                      an entry in the online help                    *
 *      ULONG ulStyle   The message box style                          *
 *      UCHAR *pucMsg   An optional message that describes where the   *
 *                      error occured                                  *
 *      PSZ pszErrMod   The pointer to the name of the module _FILE_   *
 *      LONG lErrLine   The pointer to the sourcecodeline __LINE__     *
 *                                                                     *
 * This routine displays a message box, with the style specified in    *
 * ulStyle. The message box automatically returns the pushbutton       *
 * selected by the user. This function is most useful for user inter-  *
 * action, because no anchor block, but a message, is specified.       *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#include        "Pc2.h"                 /* User include files */
#include        "error.h"

#define         _FILE_  "Error.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;

#define         TITLEBAR_LENGTH         96
#define         ERRORBUFFER_LENGTH      4096
#define         MESSAGEBUFFER_LENGTH    256

                                        /* Titlebar for message boxes */
static unsigned char    Titlebar[TITLEBAR_LENGTH];
                                        /* The whole error message that is displayed to
                                           the user via WinMessageBox */
static unsigned char    ErrBuffer[ERRORBUFFER_LENGTH]; 
                                        /* For OS/2 errors, add text HELP SYSxxxx would display */
static unsigned char    MsgBuffer[MESSAGEBUFFER_LENGTH]; 

ULONG   PM_Error(HAB hab, HWND hwndOwner, ULONG ulWindow, ULONG ulStyle, UCHAR *pucMsg, PSZ pszErrMod, LONG lErrLine)
{
ERRORID     erroridCode;                /* Returned by WinGetLastError() */
PERRINFO    pErrInfoBlk;                /* Pointer to ERRINFO structure that is filled
                                           by WinGetErrorInfo */
PSZ         pszOffset;                  /* Pointer to the current error message returned
                                           by WinGetErrorInfo */
USHORT      usResponse;                 /* Message box return value */
                                        /* If no special test is given, assume empty string */
if(!pucMsg) pucMsg="";
sprintf(ErrBuffer, "%s\n\nModule: %s\nLinenumber: %ld\nError reported by PM: ",
    pucMsg, pszErrMod, lErrLine);

                                        /* Get the last error set by a PM function */
erroridCode=WinGetLastError(hab);
sprintf(ErrBuffer, "%s%08lX", ErrBuffer, (ULONG)erroridCode);
                                        /* Get last error for the current thread. We loop
                                           until no more error is found, although errors
                                           arn't stacked (but things may change) */
pErrInfoBlk=WinGetErrorInfo(hab);
                                        /* Do as long as there is an errorinfo available */
while(pErrInfoBlk!=(PERRINFO)NULL)
    {
                                        /* Find offset in array of message offsets */
    pszOffset = ((PSZ)pErrInfoBlk) + pErrInfoBlk->offaoffszMsg;
                                        /* Address error message in array of messages and 
                                           append error message to source code linenumber */
    strcat(ErrBuffer, "\nInfo: ");
    strcat(ErrBuffer, (((PSZ)pErrInfoBlk) + *((PSHORT)pszOffset)));
    WinFreeErrorInfo(pErrInfoBlk);      /* Free resource segment */
    pErrInfoBlk=WinGetErrorInfo(hab);
    }
                                        /* Broaden titlebar by spaces to make message box wider */
memset(Titlebar, ' ', TITLEBAR_LENGTH);
Titlebar[TITLEBAR_LENGTH-1]='\0';
memcpy(Titlebar, "PC/2: PM Error Message Information", sizeof("PC/2: PM Error Message Information")-1);
                                        /* If the user requested not to be prompted, don't display the
                                           message if it is just an informational one (in the future we
                                           might log to file) */
if((pHP!=NULL) && (pHP->ulRuntimeFlag & NOPROMPT) && (ulStyle & MB_INFORMATION))
    return(MB_OK);
                                        /* Signal to user via speaker */
if((pHP!=NULL) && !(pHP->ulRuntimeFlag & NOPROMPT))
    {
    DosBeep(1000,100);
    DosBeep(300,100);
    DosBeep(1000,200);
    }
usResponse=WinMessageBox(
    HWND_DESKTOP,                       /* Parent window is DESKTOP */
    hwndOwner,                          /* Owner window */
    (PSZ)ErrBuffer,                     /* General_Error message */
    Titlebar,                           /* Title bar message */
    (USHORT)ulWindow,                   /* Message box identifier for help */
    (USHORT)ulStyle);                   /* Message box style */
return(usResponse);
}

ULONG   Dos_Error(ULONG ulRc, HWND hwndOwner, ULONG ulWindow, ULONG ulStyle, UCHAR *pucMsg, PSZ pszErrMod, LONG lErrLine)
{
ULONG       ulLength;                   /* Message length */
ULONG       ulClass;                    /* Error class */
ULONG       ulAction;                   /* Error action */
ULONG       ulLocus;                    /* Error location */
USHORT      usResponse;                 /* Message box return value */

static unsigned char   *Err_Class[]={"Out of ressource",
                                     "Temporary situation",
                                     "Authorization failed",
                                     "Internal error",
                                     "Device hardware failure",
                                     "System failure",
                                     "Probable application failure",
                                     "Item not located",
                                     "Bad format for call/data",
                                     "Resource or data locked",
                                     "Incorrect media, CRC check",
                                     "Action already taken or done",
                                     "Unclassified",
                                     "Cannot perform requested action",
                                     "Timeout",
                                     "Error in file \"Error.c\""};
static unsigned char   *Err_Action[]={"Retry immediately",
                                      "Delay and retry",
                                      "Bad user input - get new values",
                                      "Terminate in an orderly manner",
                                      "Terminate immediately",
                                      "Ignore error",
                                      "Retry after user intervention",
                                      "Error in file \"Error.c\""};
static unsigned char   *Err_Locus[]={"Unknown",
                                     "Random access device such as a disk",
                                     "Network",
                                     "Serial device",
                                     "Memory",
                                     "Error in file \"Error.c\""};

if(ulRc)
    {
    if(!pucMsg) pucMsg="";              /* If no special test is given, assume empty string */
    DosErrClass(ulRc, &ulClass, &ulAction, &ulLocus);
                                        /* Get OS/2 message text */
    if(DosGetMessage(NULL, 0, MsgBuffer, sizeof(MsgBuffer), ulRc, "OSO001.MSG", &ulLength))
        MsgBuffer[0]='\0';
    else
        MsgBuffer[ulLength-2]='\0';     /* Remove trailing \r\n */
    sprintf(ErrBuffer, "%s\n\nModule: %s\nLinenumber: %ld\nError reported by OS/2: "\
        "%ld\nClass: %s\nAction: %s\nLocation: %s\n\n\"%s\"", 
        pucMsg, pszErrMod, lErrLine, ulRc, 
        Err_Class[ulClass-1], Err_Action[ulAction-1], Err_Locus[ulLocus-1],
        MsgBuffer);
    memset(Titlebar, ' ', TITLEBAR_LENGTH);
    Titlebar[TITLEBAR_LENGTH-1]='\0';
    memcpy(Titlebar, "PC/2: OS/2 Error Message Information", sizeof("PC/2: OS/2 Error Message Information")-1);
                                        /* If the user requested not to be prompted, don't display the
                                           message if it is just an informational one (in the future we
                                           might log to file) */
    if((pHP!=NULL) && (pHP->ulRuntimeFlag & NOPROMPT) && (ulStyle & MB_INFORMATION))
        return(MB_OK);
                                        /* Signal to user via speaker */
    if((pHP!=NULL) && !(pHP->ulRuntimeFlag & NOPROMPT))
        {
        DosBeep(1000,100);
        DosBeep(300,100);
        DosBeep(1000,200);
        }
    usResponse=WinMessageBox(HWND_DESKTOP, hwndOwner, (PSZ)ErrBuffer,
        Titlebar, (USHORT)ulWindow, (USHORT)ulStyle);
    return(usResponse);
    }
else
    return(MBID_IGNORE);
}

ULONG   User_Error(HWND hwndOwner, ULONG ulWindow, ULONG ulStyle, UCHAR *pucMsg, PSZ pszErrMod, LONG lErrLine)
{
USHORT      usResponse;                 /* Message box return value */

if(!pucMsg) pucMsg="Debug Information"; /* If no special test is given, assume debugging */
                                        /* For query dialogs, don't include the (debugging) module and
                                           linenumber information (note: MB_QUERY equals MB_ICONQUESTION) */
if((ulStyle & MB_INFORMATION)==MB_QUERY)
    strcpy(ErrBuffer, pucMsg);
else
    sprintf(ErrBuffer, "%s\n\nModule: %s\nLinenumber: %ld\n",
        pucMsg, pszErrMod, lErrLine);
memset(Titlebar, ' ', TITLEBAR_LENGTH);
Titlebar[TITLEBAR_LENGTH-1]='\0';
memcpy(Titlebar, "PC/2: User Information", sizeof("PC/2: User Information")-1);
                                        /* If the user requested not to be prompted, don't display the
                                           message if it is just an informational one (in the future we
                                           might log to file) */
if((pHP!=NULL) && (pHP->ulRuntimeFlag & NOPROMPT))
    { 
    if((ulStyle & MB_INFORMATION)==MB_QUERY)
        return(MBID_YES);
    if((ulStyle & MB_INFORMATION)==MB_INFORMATION)
        return(MB_OK);
    }
                                        /* Unfortunately, MB_INFORMATION equals MB_ICONQUESTION|MB_ICONEXCLAMATION,
                                           which is quite a silly */
if((ulStyle & MB_INFORMATION)==MB_INFORMATION)
    {
                                        /* Signal to user via speaker */
    if((pHP!=NULL) && !(pHP->ulRuntimeFlag & NOPROMPT))
        {
        DosBeep(1000,100);
        DosBeep(300,100);
        DosBeep(1000,200);
        }
    }
usResponse=WinMessageBox(HWND_DESKTOP, hwndOwner, (PSZ)ErrBuffer,
    Titlebar, (USHORT)ulWindow, (USHORT)ulStyle);
return(usResponse);
}
