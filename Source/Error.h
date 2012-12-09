/***********************************************************************\
 *                               Error.h                               *
 *              Copyright (C) by Stangl Roman, 1992, 2000              *
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

#define         INCL_WIN
#define         INCL_DOSMISC

#include        <os2.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>

#define         PM_ERR(hab, hwndOwner, ulWindow, ulStyle, pucMsg) \
                    PM_Error(hab, hwndOwner, ulWindow, ulStyle, pucMsg, _FILE_, __LINE__)
#define         DOS_ERR(rc, hwndOwner, ulWindow, ulStyle, pucMsg) \
                    Dos_Error(rc, hwndOwner, ulWindow, ulStyle, pucMsg, _FILE_, __LINE__)
#define         USR_ERR(hwndOwner, ulWindow, ulStyle, pucMsg) \
                    User_Error(hwndOwner, ulWindow, ulStyle, pucMsg, _FILE_, __LINE__)

extern ULONG PM_Error(HAB hab, HWND hwndOwner, ULONG ulWindow, ULONG ulStyle, UCHAR *pucMsg, PSZ ErrModule, LONG ErrLine);
extern ULONG Dos_Error(ULONG ulRc, HWND hwndOwner, ULONG ulWindow, ULONG ulStyle, UCHAR *pucMsg, PSZ ErrModule, LONG ErrLine);
extern ULONG User_Error(HWND hwndOwner, ULONG ulWindow, ULONG ulStyle, UCHAR *pucMsg, PSZ ErrModule, LONG ErrLine);

