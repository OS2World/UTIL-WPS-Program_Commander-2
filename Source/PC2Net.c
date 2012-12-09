/***********************************************************************\
 *                                                                     *
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1998, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * PC2Net.c     Network access procedures.                             *
 *                                                                     *
 * This module is linked with the subsystem C-Runtime library, that    *
 * means that most (that is all functions not able to be inlined)      *
 * functions are not available directly, but via a structure passed    *
 * by the PC/2 Network thread during initialization. I decided to not  *
 * use the full C-Runtime to avoid 2 copies (one in PC2.EXE and one in *
 * PC2NET.DLL) which may also cause problems (e.g. allocations in one, *
 * and frees in the other instance).                                   *
 * If I knew a way to "export" symbols from an EXE then I could build  *
 * an import library and use that, but I haven't found a way to get    *
 * that working (and OS/2 does not seem to support that).              *
 *                                                                     *
 * This module contains the "networking" code that allows PC/2 to      *
 * retrieve the latest information from the author's homepage auto-    *
 * matically without requireing the user to look for that him/herself, *
 * it's a kind of remotely initiated "push" technology. In PC/2 I      *
 * used the keyword "Registration" for that, though not fitting        *
 * perfectly.                                                          *
 *                                                                     *
 * As this module is linked with the TCP/IP DLLs, it can only be       *
 * loaded by PC/2 if TCP/IP networking support is installed, thus PC/2 *
 * can easily find out if this module can be used at all. Once TCP/IP  *
 * is available there are 2 other problems:                            *
 * -) Usually, PC's are not directly connected to the Internet but     *
 *    through a firewall, likely acting as a Proxy. While this is no   *
 *    drawback, it requires the user to configure PC/2's network       *
 *    access correctly.                                                *
 * -) Likely, the PC's are connected to the internet through a dial-up *
 *    connection. First, PC/2 doesn't know when the user made a        *
 *    connection, and second PC/2 should not cause such a connection   *
 *    to be established ("dial on demand") too often. Thus PC/2 should *
 *    try to download information from the Internet not too often but  *
 *    allow the user to initiate a connection always manually.         *
 *                                                                     *
 * What gets downloaded (regularily) from the Internet is a HTML page  *
 * (currently located at:                                              *
 * http://www.geocities.com/SiliconValley/Pines/7885/AutoUpdate.html)  *
 * This page contains human readable (that is doesn't look too bad     *
 * when being looked at with a browser) information that is parsed to  *
 * let PC/2 check for information it is interested for.                *
 *                                                                     *
 * The document will have the following layout:                        *
 * -) <h2>Product: product</h2>                                        *
 *    The Product tag defines for which  product this section is valid *
 *    for, for PC/2 it will read <h2>Product: PC/2</h2>                *
 * -) <h3>keyword: [dd.mm.yy[yy]]</h3>                                 *
 *    Each Product contains a variable amount of keywords and an       *
 *    optional date for when this keywords applies.                    *
 *    +) News: dd.mm.yy[yy] hh:mm                                      *
 *       This tag identifies the status of the last successful access  *
 *       to the Internet. This is used to find out if the time has     *
 *       expired to check for news again.                              *
 *    +) News: dd.mm.yy[yy]                                            *
 *       This tag identifies news of general importance, they may not  *
 *       exactly be related to the Product, but when found they are    *
 *       displayed to the user.                                        *
 *    +) Update:                                                       *
 *       This tag identifies updates and how PC/2 should download them *
 *       e.g. fixes or enhancements.                                   *
 *       Each document contains the following tags:                    *
 *       Replacement | Addition: filename.extension                    *
 *         Description: descriptive text                               *
 *         [Version: major.minor] (not implemented yet)                *
 *         [Size: size] (not implemented yet)                          *
 *         [CRC: checksum] (not implemented yet)                       *
 *         Url: url of the document to be downloaded                   *
 *       There may be multiple documents specified, or end with the    *
 *       next tag or keyword.                                          *
 * -) <!--- End --->                                                   *
 *    This HTML comment will mark the end where the document needs to  *
 *    be parsed. As the document may contain further HTML, this allows *
 *    to skip parsing of it.                                           *
 *                                                                     *
 * From above layout the following structure will be built:            *
 *                                                                     *
 *    +------------+    +-----------+                                  *
 *    ! AUTOUPDATE !--->! TAGSTATUS |                                  *
 *    +------------+    +-----------+                                  *
 *      !        ^                                                     *
 *      !        !                                                     *
 *      v        !                                                     *
 *    +------------+    +---------+     +---------+                    *
 *    ! AUTOUPDATE !--->! TAGNEWS !<--->! TAGNEWS !<--->...            *
 *    +------------+    +---------+     +---------+                    *
 *      !        ^                                                     *
 *      !        !                                                     *
 *      v        !                                                     *
 *    +------------+    +-----------+     +-----------+                *
 *    ! AUTOUPDATE !--->! TAGUPDATE !<--->! TAGUPDATE !<--->...        *
 *    +------------+    +-----------+     +-----------+                *
 *                                                                     *
 * The information downloaded from the Internet will then be compared  *
 * to what is already known locally and only the chances will be       *
 * applied:                                                            *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#include        "PC2.h"                 /* User include files */
#include        "Error.h"

#define         _FILE_  "PC2Net.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;
const ULONG     _PC2NETSIGNATURE=BLDLEVEL_SIGNATURE;

#include        <io.h>
#include        <fcntl.h>
#include        <sys\stat.h>

                                        /* Include Warp 4 Toolkit's TCP/IP Toolkit 5 headers.
                                           Unfortunately it seems the headers are currently
                                           in migration to the 32-bit TCP/IP protocol stack
                                           introduced with TCP/IP V4.1 (Software choice) and
                                           TCP/IP V4.2 (Aurora). Thus, when using the Warp 4
                                           Toolkit from the DevCon V2R1 just compile it, when
                                           using it from DevCon V2R2 define TCPV5HDR additionally */
                                        /* With Warp 4.5 Toolkit one can choose to compile for the
                                           16-bit TCP/IP protocol stack (up to TCP/IP 4.0) by
                                           defining the macro TCPV40HDRS or to compile for the
                                           32-bit TCP/IP stack (TCP/IP V4.1 and V4.21). However
                                           the later would link with TCPIP32.DLL instead of
                                           SO32DLL.DLL and TCP32DLL.DLL, thus being not backwards
                                           compatible. */
#include        <types.h>
#include        <nerrno.h>
#include        <utils.h>
#include        <netinet\in.h>
#include        <sys\socket.h>
#include        <sys\ioctl.h>
#include        <net\if.h>
#include        <netdb.h>

#ifndef TCPV40HDRS
#include        <unistd.h>
#include        <arpa\inet.h>
#endif  /* TCPV40HDRS */

#ifndef htons
#ifdef  TCPV40HDRS
unsigned short _System  bswap(unsigned short sSwap);
#endif  /* TCPV40HDRS */
#define htons(x)    (bswap(x))
#endif  /* htons */

#define         MALLOC                  pCruntime->pfnMalloc
#define         FREE                    pCruntime->pfnFree
#define         STRICMP                 pCruntime->pfnStricmp
#define         STRNICMP                pCruntime->pfnStrnicmp
#define         STRUPR                  pCruntime->pfnStrupr
#define         STRDUP                  pCruntime->pfnStrdup
#define         FPRINTF                 pCruntime->pfnFprintf
#define         FOPEN                   pCruntime->pfnFopen
#define         FCLOSE                  pCruntime->pfnFclose
#define         MKTIME                  pCruntime->pfnMktime
#define         STRIPLEADING            pCruntime->pfnStripLeading
#define         STRIPTRAILING           pCruntime->pfnStripTrailing

#define         REQUESTBUFFERSIZE       4000

HOOKPARAMETERS  *pHP=0;
CRUNTIME        *pCruntime=0;

/*--------------------------------------------------------------------------------------*\
 * This is the network interface procedure, which extends the Network thread's object   *
 * window procedure. It will be loaded by the Network thread during its initilization,  *
 * which only succeeds if TCP/IP is installed, as PC2NET.DLL is statically linked with  *
 * the TCP/IP DLLs.                                                                     *
\*--------------------------------------------------------------------------------------*/
MRESULT EXPENTRY NetworkInterface(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
UCHAR           ucMessage[EF_SIZE255];

switch(msg)
{
/*                                                                                      *\
 * Syntax: WM_INITTHREAD, NULL, NULL                                                    *
\*                                                                                      */
case WM_INITTHREAD:
/*                                                                                      *\
 * Message direclty called by Network thread after loading PC2NET.DLL to allow us to    *
 * initialize now.                                                                      *
\*                                                                                      */
    {
    LONG                lErrorCode=NO_ERROR;

                                        /* We initialize by being called by PC/2's Network
                                           thread, which passes by the required parameters
                                           (e.g. the CRUNTIME structure which allows us to
                                           call C-Runtime functions from the runtime linked
                                           to PC2.EXE */
#ifdef  DEBUG_PC2NET
    printf("PC2Net: WM_INITTHREAD\n");
#endif  /* DEBUG_PC2NET */
    if((mp1==0) || (mp2==0))
        {
#ifdef  DEBUG_PC2NET
        printf("PC2Net: Initialization failed\n");
#endif  /* DEBUG_PC2NET */
        return((MRESULT)FALSE);
        }
                                        /* Ok, save the parameters passed by */
    pHP=(HOOKPARAMETERS *)mp1;
    pCruntime=(CRUNTIME *)mp2;
    _ctype=pCruntime->pus_Ctype;
                                        /* Initialize TCP/IP sockets, query the interfaces
                                           available and load the Automatic Update local mirror */
    while(TRUE)
        {
        lErrorCode=SocketInit();
        if(lErrorCode!=NO_ERROR)
            break;
        lErrorCode=(LONG)WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_QUERYINTERFACE), NULL);
        if(lErrorCode!=NO_ERROR)
            break;
        lErrorCode=(LONG)WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_LOADUPDATE), NULL);
        break;
        }
    return((MRESULT)lErrorCode);
    }

/*                                                                                      *\
 * Syntax: WM_QUITTHREAD, ULONG ulThreadId, NULL                                        *
\*                                                                                      */
case WM_QUITTHREAD:
/*                                                                                      *\
 * Message direclty called by Network thread during closing up of Network thread to     *
 * allow us to cleanup.                                                                 *
\*                                                                                      */
                                        /* Free allocated structures */
    WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_FREEUPDATE), MPFROMP(&pCruntime->pautoupdateRemote));
    WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_FREEUPDATE), MPFROMP(&pCruntime->pautoupdateLocal));
                                        /* Close the socket (the local communication endpoint) */
    if((pCruntime->iSocketLANPPP!=0) && (soclose(pCruntime->iSocketLANPPP)==-1))
        {
        LONG    lReturnCode=NO_ERROR;

        lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
        printf("PC2Net: soclose() error %d.\n", lReturnCode);
#endif  /* DEBUG_PC2NET */
        return((MRESULT)lReturnCode);
        }
    break;

/*                                                                                      *\
 * Syntax: WM_NETACCESS, HWND hwndStatusText, ULONG ulBackground                        *
\*                                                                                      */
case WM_NETACCESS:
/*                                                                                      *\
 * This message is posted by the Registration notebook page of the Desktop dialog when  *
 * the user has selected the Update Now pushbutton to initialize an access to tne       *
 * Internet to check for update information. The error code is returned and the local   *
 * mirror is loaded from file again to make it consistent when the user has modified    *
 * the file (otherwise, only the next restart of PC/2 would have reloaded it).          *
\*                                                                                      */
    {
    LONG                lErrorCode=NO_ERROR;

    pCruntime->hwndStatusText=HWNDFROMMP(mp1);
#ifdef  DEBUG_PC2NET
    printf("PC2Net: WM_NETACCESS\n");
#endif  /* DEBUG_PC2NET */
    while(TRUE)
        {
        lErrorCode=(LONG)WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_QUERYINTERFACE), mp2);
        if(lErrorCode!=NO_ERROR)
            break;
        if(pCruntime->pautoupdateLocal!=0)
            WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_FREEUPDATE), MPFROMP(&pCruntime->pautoupdateLocal));
        lErrorCode=(LONG)WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_LOADUPDATE), mp2);
        if(lErrorCode!=NO_ERROR)
            break;
        if(pCruntime->pautoupdateRemote!=0)
            WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_FREEUPDATE), MPFROMP(&pCruntime->pautoupdateRemote));
        lErrorCode=(LONG)WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_GETUPDATE), mp2);
        if(lErrorCode!=NO_ERROR)
            break;
        lErrorCode=(LONG)WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_CHECKNEWS), NULL);
        if(lErrorCode!=NO_ERROR)
            break;
        lErrorCode=(LONG)WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_CHECKUPDATE), MPFROMLONG(TRUE));
        if(lErrorCode!=NO_ERROR)
            break;
        lErrorCode=(LONG)WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_POSTEMAIL), NULL);
        if(lErrorCode!=NO_ERROR)
            break;
        WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("")), NULL);
                                        /* Exit loop */
        break;
        }
                                        /* Write status back as far as we know */
                                        /* Do not display anything if Background operation is
                                           specified even if we got an error, unless the error
                                           is a HTTP Status Code of type Client Error 4xx or
                                           Server Error 5xx */
    if(((lErrorCode!=NO_ERROR) && (LONGFROMMP(mp2)==FALSE)) ||
        ((lErrorCode>=400) && (lErrorCode<600)))
        {
        sprintf(ucMessage, "TCP/IP error %ld accessing Network or WEB-Server ...\n\n"
            "Module: %s\nLinenumber: %ld\n", lErrorCode, _FILE_, __LINE__);
        if((LONGFROMMP(mp2)==FALSE) || ((lErrorCode>=400) && (lErrorCode<600)))
            WinPostMsg(pHP->hwndClient, WM_NETACCESS, MPFROMP(STRDUP(ucMessage)), MPFROMLONG(AUTOUPDATE_STATUS));
        WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP(ucMessage)), NULL);
        }
                                        /* Write status back as far as we know */
    lErrorCode=(LONG)WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_SAVEUPDATE), NULL);
    if(lErrorCode!=NO_ERROR)
        {
        sprintf(ucMessage, "OS/2 error %ld writing PC2.NET ...\n\n"
            "Module: %s\nLinenumber: %ld\n", lErrorCode, _FILE_, __LINE__);
        WinPostMsg(pHP->hwndClient, WM_NETACCESS, MPFROMP(STRDUP(ucMessage)), MPFROMLONG(AUTOUPDATE_STATUS));
        WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP(ucMessage)), NULL);
        }
    return((MRESULT)lErrorCode);
    }

/*                                                                                      *\
 * Syntax: WM_TIMER, NULL, NULL                                                         *
\*                                                                                      */
case WM_TIMER:
/*                                                                                      *\
 * Message direclty called by Network thread for each WM_TIMER event to allow us to     *
 * check if it is time to check for Automatic Updates online (of the user has allowed   *
 * us to do that, we'll try to make a connection).                                      *
\*                                                                                      */
    {
                                        /* Skip the comparism if it's time to schedule for
                                           this minutes, as the last comparism this hour already
                                           has shown that it's not time to execute an Online
                                           Automatic Update */
    static ULONG    ulScheduleSkip=0;
    static ULONG    ulTimerTicks=(ULONG)-1;
    static ULONG    ulSchedulePending=FALSE;
    AUTOUPDATE      *pautoupdateCurrent;
    TAGSTATUS       *ptagstatusCurrent;
    ULONG           ulSchedule=TRUE;
    LONG            lErrorCode=NO_ERROR;

                                        /* Update the timer tick, which updates once a minute */
    ulTimerTicks++;
                                        /* Query currently available interfaces (in background).
                                           If no pending schedule is in progress, then don't
                                           do a comparism if we already have done that the last hour */
    if(ulSchedulePending==TRUE)
        WinSendMsg(hwnd, WM_NETSCHEDULE, MPFROMLONG(NET_QUERYINTERFACE), MPFROMLONG(TRUE));
    else
        {
        if(ulScheduleSkip>0)
            {
#ifdef  DEBUG
            printf("PC2Net: WM_TIMER skipping %ld minutes\n", ulScheduleSkip);
#endif  /* DEBUG */
            ulScheduleSkip--;
            break;
            }
        }
                                        /* We schedule only an Automatic Update if the required
                                           time has passed since the last access of the network */
    pautoupdateCurrent=pCruntime->pautoupdateLocal;
                                        /* First, find the date of the last previous connection */
    while(pautoupdateCurrent!=0)
        {
        if(pautoupdateCurrent->ulStatus & AUTOUPDATE_STATUS)
            break;
        pautoupdateCurrent=pautoupdateCurrent->pNext;
        }
                                        /* Second, find the date of the last remote connection. If
                                           none is found, then schedule an Automatic Update request.
                                           If a request is scheduled, no need to test for it. */
    if((ulSchedulePending==FALSE) && (pautoupdateCurrent!=0))
        {
        struct tm       tm_Schedule;
        struct tm       tm_Status;
        time_t          time_tSchedule;
        time_t          time_tStatus;

                                        /* Convert current time */
        tm_Schedule=pCruntime->tm_Current;
#ifdef  DEBUG
        tm_Schedule.tm_hour-=24;
#else
        tm_Schedule.tm_hour-=(24*2);
#endif
        tm_Schedule.tm_min=0;
        time_tSchedule=MKTIME(&tm_Schedule);
                                        /* Convert last logged access. If there is no need to
                                           schedule an Online Automatic Update, then don't test
                                           this again this hour */
        ptagstatusCurrent=pautoupdateCurrent->Tags.tagStatus;
        memset(&tm_Status, 0, sizeof(tm_Status));
        tm_Status.tm_hour=ptagstatusCurrent->ulHour;
        tm_Status.tm_mday=ptagstatusCurrent->ulDay;
        tm_Status.tm_mon=ptagstatusCurrent->ulMonth-1;
        tm_Status.tm_year=ptagstatusCurrent->ulYear-1900;
        time_tStatus=MKTIME(&tm_Status);
        if(time_tSchedule<=time_tStatus)
            {
            ulSchedule=FALSE;
            ulScheduleSkip=60-pCruntime->tm_Current.tm_min;
            }
        }
                                        /* Third, if an Automatic Update is pending, then see if
                                           it makes sense to (re)try now. If we currently have a
                                           PPP interface available, retry to update immediately because
                                           a connection may be initializing or initialized. If we
                                           currently only have a LAN interface available, it does
                                           not make much sense to retry very often, because if a LAN
                                           fails it likely is a major longterm problem */
    if(ulSchedulePending==TRUE)
        {
        if(pCruntime->ulInterfacesPPP>0)
            ulSchedule=TRUE;
        else if(pCruntime->ulInterfacesLAN>0)
            {
#ifdef  DEBUG
            if((ulTimerTicks % 2)==0)
                ulSchedule=TRUE;
#else
            if((ulTimerTicks % 15)==0)
                ulSchedule=TRUE;
#endif  /* DEBUG */
            else
                ulSchedule=FALSE;
            }
        }
                                        /* Forth, if we are on a different day, then try to make
                                           the connection to load the Automatic Update webpage.
                                           Even if we didn't have an AUTOUPDATE structure before,
                                           the implicit call of SaveAutoUpdate() during WM_NETACCESS
                                           will have created one */
    if((ulSchedule==TRUE) && (pHP->ulRegistrationFlag & REGISTER_ONLINE))
        {
#ifdef  DEBUG_PC2NET
        printf("PC2Net: WM_TIMER %d.%d.%d %d:%d\n",
            (int)pCruntime->tm_Current.tm_mday,
            (int)pCruntime->tm_Current.tm_mon+1,
            (int)pCruntime->tm_Current.tm_year+1900,
            (int)pCruntime->tm_Current.tm_hour,
            (int)pCruntime->tm_Current.tm_min);
#endif  /* DEBUG_PC2NET */
        lErrorCode=(LONG)WinSendMsg(hwnd, WM_NETACCESS, NULL, MPFROMLONG(TRUE));
        if(lErrorCode!=NO_ERROR)
            ulSchedulePending=TRUE;
        else
            ulSchedulePending=FALSE;
        }
    DosSleep(2000);
    WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("")), NULL);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_NETSCHEDULE, ULONG ulRequestCode, MPARAM mp2                              *
\*                                                                                      */
case WM_NETSCHEDULE:
/*                                                                                      *\
 * This message is posted to perform the request ulRequestCode.                         *
\*                                                                                      */
    {
    LONG                lErrorCode=NO_ERROR;
    URL                 urlProxy;
    URL                 *purlProxy=0;
    URL                 urlServer;
    UCHAR               ucFilename[CCHMAXPATH];
    UCHAR               *pucDocument;
    UCHAR               *pucNews;
    UCHAR               *pucUpdate;
    ULONG               ulDocumentLength;

    if(LONGFROMMP(mp1)==NET_QUERYINTERFACE)
        {
                                        /* Query current interface status. The interfaces are
                                           divided into LAN and Point-to-Point (dial-up) interfaces,
                                           because, if there are only LAN interfaces and we can't
                                           connect to the Automatic Update host, it is unlikely that
                                           this is just a temporary problem (thus the retry interval can
                                           be longer). If PPP interfaces are found then very likely
                                           we have an up and running dial-up connection (thus retry
                                           interval can be shorter) */
#ifdef  DEBUG_PC2NET
        printf("PC2Net: WM_NETSCHEDULE (NET_QUERYINTERFACE)\n");
#endif  /* DEBUG_PC2NET */
        lErrorCode=InterfaceStatus(&pCruntime->ulInterfacesLAN, &pCruntime->ulInterfacesPPP);
        if(LONGFROMMP(mp2)==FALSE)
            {
            WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Accessing TCP/IP stack, querying interfaces ...")), NULL);
            DosSleep(2000);
            }
        if((lErrorCode==NO_ERROR) && (LONGFROMMP(mp2)==FALSE))
            {
            sprintf(ucMessage, "Accessing TCP/IP stack, found %ld LAN and %ld PPP interfaces ...",
                pCruntime->ulInterfacesLAN, pCruntime->ulInterfacesPPP);
            WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP(ucMessage)), NULL);
            DosSleep(2000);
            }
        return((MRESULT)lErrorCode);
        }
    else if(LONGFROMMP(mp1)==NET_LOADUPDATE)
        {
                                        /* Load the Automatic Update information from the local
                                           mirror file */
#ifdef  DEBUG_PC2NET
        printf("PC2Net: WM_NETSCHEDULE (NET_LOADUPDATE)\n");
#endif  /* DEBUG_PC2NET */
        if(LONGFROMMP(mp2)==FALSE)
            WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Reading Automatic Update from local mirror PC2.NET ...")), NULL);
        strcpy(ucFilename, pHP->ucPathDLL);
        strcat(ucFilename, "\\PC2.net");
        pCruntime->pautoupdateLocal=LoadAutoUpdate(ucFilename, 0);
        DosSleep(2000);
        return(NO_ERROR);
        }
    else if(LONGFROMMP(mp1)==NET_GETUPDATE)
        {
        LONG    lTemp;

                                        /* Load the Automatic Update information from the remote
                                           host */
#ifdef  DEBUG_PC2NET
        printf("PC2Net: WM_NETSCHEDULE (NET_GETUPDATE)\n");
#endif  /* DEBUG_PC2NET */
        if(LONGFROMMP(mp2)==FALSE)
            WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Downloading Automatic Update from webpage ...")), NULL);
        ParseUrl(&urlProxy, pHP->ucProxyUrl, 80, 80);
        ParseUrl(&urlServer, pHP->ucUpdateUrl, 80, 80);
        if(pHP->ucProxyUrl[0]!='\0')
            purlProxy=&urlProxy;
        while(TRUE)
            {
            lErrorCode=ConnectionOpen(purlProxy, &urlServer);
            if(lErrorCode!=NO_ERROR)
                break;
                                        /* When doing a background update, only update the PC/2
                                           Status window when we could connect to either the proxy
                                           or Automatic Update Information host */
            if(LONGFROMMP(mp2)==TRUE)
                {
                sprintf(ucMessage, "Accessing TCP/IP stack, found %ld LAN and %ld PPP interfaces ...",
                    pCruntime->ulInterfacesLAN, pCruntime->ulInterfacesPPP);
                WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP(ucMessage)), NULL);
                DosSleep(2000);
                WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Downloading Automatic Update from webpage ...")), NULL);
                DosSleep(2000);
                }
                                        /* Send the request and wait for a result. The result
                                           may by a error 401 (WWW-Authenticate) or 407
                                           (Proxy-Authenticate), we just retry the request
                                           including Authentication then */
            lErrorCode=RequestSend(purlProxy, &urlServer, "", FALSE);
            if(lErrorCode!=NO_ERROR)
                break;
            lErrorCode=RequestReceive(purlProxy, &urlServer, &pucDocument, &ulDocumentLength);
            if((lErrorCode==401) || (lErrorCode==407))
                {
                if(LONGFROMMP(mp2)==TRUE)
                    {
                    WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Downloading Automatic Update from webpage "\
                        "using Authorization ...")), NULL);
                    DosSleep(2000);
                    }
                lErrorCode=ConnectionOpen(purlProxy, &urlServer);
                if(lErrorCode!=NO_ERROR)
                    break;
                lErrorCode=RequestSend(purlProxy, &urlServer, "", TRUE);
                if(lErrorCode!=NO_ERROR)
                    break;
                lErrorCode=RequestReceive(purlProxy, &urlServer, &pucDocument, &ulDocumentLength);
                }
            if(lErrorCode!=NO_ERROR)
                break;
            pCruntime->pautoupdateRemote=LoadAutoUpdate(0, pucDocument);
            break;
            }
        lTemp=ConnectionClose(purlProxy, &urlServer);
        if((lTemp!=NO_ERROR) && (lErrorCode==NO_ERROR))
            lErrorCode=lTemp;
        DosSleep(2000);
        return((MRESULT)lErrorCode);
        }
    else if(LONGFROMMP(mp1)==NET_SAVEUPDATE)
        {
                                        /* Save the Automatic Update information into the local
                                           mirror file and replace the local mirror AUTOUPDATE
                                           structure */
#ifdef  DEBUG_PC2NET
        printf("PC2Net: WM_NETSCHEDULE (NET_SAVEUPDATE)\n");
#endif  /* DEBUG_PC2NET */
        strcpy(ucFilename, pHP->ucPathDLL);
        strcat(ucFilename, "\\PC2.net");
        if(pCruntime->pautoupdateRemote!=0)
            {
            FreeAutoUpdate(pCruntime->pautoupdateLocal);
            pCruntime->pautoupdateLocal=SaveAutoUpdate(ucFilename, pCruntime->pautoupdateRemote);
            pCruntime->pautoupdateRemote=0;
            if(pCruntime->pautoupdateLocal!=0)
                return((MRESULT)NO_ERROR);
            else
                return((MRESULT)ERROR_ACCESS_DENIED);
            }
        return((MRESULT)NO_ERROR);
        }
    else if(LONGFROMMP(mp1)==NET_FREEUPDATE)
        {
                                        /* Free the Automatic Update information */
#ifdef  DEBUG_PC2NET
        printf("PC2Net: WM_NETSCHEDULE (NET_FREEUPDATE)\n");
#endif  /* DEBUG_PC2NET */
        if((VOID *)mp2!=0)
            {
            FreeAutoUpdate(*(AUTOUPDATE **)mp2);
            *(AUTOUPDATE **)mp2=0;
            }
        return((MRESULT)NO_ERROR);
        }
    else if(LONGFROMMP(mp1)==NET_CHECKNEWS)
        {
                                        /* Check for News. If some News were found, inform
                                           main thread to inform the user */
#ifdef  DEBUG_PC2NET
        printf("PC2Net: WM_NETSCHEDULE (NET_CHECKNEWS)\n");
#endif  /* DEBUG_PC2NET */
        WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Checking for News ...")), NULL);
        pucNews=CheckNews(pCruntime->pautoupdateRemote, pCruntime->pautoupdateLocal);
        if(pucNews)
            {
#ifdef  DEBUG_PC2NET
            UCHAR   ucDebug[160];

            memset(ucDebug, '\0', sizeof(ucDebug));
            strncpy(ucDebug, pucNews, sizeof(ucDebug)-1);
            printf("News: %s\n", ucDebug);
#endif  /* DEBUG_PC2NET */
            WinPostMsg(pHP->hwndClient, WM_NETACCESS, MPFROMP(pucNews), MPFROMLONG(AUTOUPDATE_NEWS));
            pucNews=0;
            }
        DosSleep(2000);
        return((MRESULT)NO_ERROR);
        }
    else if(LONGFROMMP(mp1)==NET_CHECKUPDATE)
        {
                                        /* Check for Updates. If some Updates were found, inform main
                                           thread to inform the user. For every call, we only download
                                           one file to allow processing of other queued message before
                                           we download the next update. Finally when all files have been
                                           downloaded, write back changes to local mirror */
#ifdef  DEBUG_PC2NET
        printf("PC2Net: WM_NETSCHEDULE (NET_CHECKUPDATE)\n");
#endif  /* DEBUG_PC2NET */
        ParseUrl(&urlProxy, pHP->ucProxyUrl, 80, 80);
        ParseUrl(&urlServer, pHP->ucUpdateUrl, 80, 80);
        if(pHP->ucProxyUrl[0]!='\0')
            purlProxy=&urlProxy;
        WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Checking for file Replacements and Additions ...")), NULL);
        pucUpdate=CheckUpdate(purlProxy, &urlServer, pCruntime->pautoupdateRemote, pCruntime->pautoupdateLocal);
        while(pucUpdate!=NULL)
            {
#ifdef  DEBUG_PC2NET
            UCHAR   ucDebug[160];

            memset(ucDebug, '\0', sizeof(ucDebug));
            strncpy(ucDebug, pucUpdate, sizeof(ucDebug)-1);
            printf("PC2Net: Update: %s\n", ucDebug);
#endif  /* DEBUG_PC2NET */
            WinPostMsg(pHP->hwndClient, WM_NETACCESS, MPFROMP(pucUpdate), MPFROMLONG(AUTOUPDATE_UPDATE));
            DosSleep(1000);
            if(LONGFROMMP(mp2)==TRUE)
                pucUpdate=CheckUpdate(purlProxy, &urlServer, pCruntime->pautoupdateRemote, pCruntime->pautoupdateLocal);
            else
                {
                WinPostMsg(hwnd, msg, mp1, mp2);
                return((MRESULT)NO_ERROR);
                }
            }
        return((MRESULT)NO_ERROR);
        }
    else if(LONGFROMMP(mp1)==NET_POSTEMAIL)
        {
                                        /* Upload user Registration data. For that, the user must have
                                           allowed us to do that and must have filled in all required
                                           fields. Then, we create the Url to post and write that
                                           request into the socket, wait for any response, if successful
                                           log the request to the local Automatic Update mirror */
        UCHAR       ucPostUrl[(EF_SIZE255<<2)];
        ULONG       ulUpdate=TRUE;
        ULONG       ulUpdateRemote=FALSE;
        AUTOUPDATE  *pautoupdateCurrent;
        TAGREGISTER *ptagregisterLocal=0;

#ifdef  DEBUG_PC2NET
        printf("PC2Net: WM_NETSCHEDULE (NET_POSTEMAIL)\n");
#endif  /* DEBUG_PC2NET */

                                        /* If all other conditions are met, then the user registration
                                           information is not posted if it has already been. We can find
                                           out that by looking for the Registration tag in just the local
                                           mirror (if it was posted during a previous launch of PC/2 it
                                           will have been filled) as a successfull online Automatic Update
                                           will immediately be reflected by the local mirror structure.
                                           As the local mirror gets replaced by the remote structure,
                                           we need to copy that */
        pautoupdateCurrent=pCruntime->pautoupdateLocal;
        while(pautoupdateCurrent!=0)
            {
            if(pautoupdateCurrent->ulStatus & AUTOUPDATE_REGISTER)
                {
                ptagregisterLocal=pautoupdateCurrent->Tags.tagRegister;
                ulUpdate=FALSE;
                break;
                }
            pautoupdateCurrent=pautoupdateCurrent->pNext;
            }
                                        /* Just to improve debugging */
#ifdef  DEBUG
        if(ptagregisterLocal!=0)
            printf("PC2Net: Registered %ld [%s], running %ld [%s]\n",
                ptagregisterLocal->ulVersion, ptagregisterLocal->ucVersion,
                (ULONG)BLDLEVEL_RELEASE, BLDLEVEL_VERSION);
        else
            printf("PC2Net: Registered [n/a], running %ld [%s]\n",
                (ULONG)BLDLEVEL_RELEASE, BLDLEVEL_VERSION);
#endif  /* DEBUG */
                                        /* If the version has changed, then also a new registration
                                           should be done. For a GA release the version is usually defined
                                           as m.nn whereas during Beta it's m.nnx (where m is the major,
                                           nn the minor version and x the Beta postfix) */
#ifdef  DEBUG
        if((ptagregisterLocal!=0) && ((ptagregisterLocal->ulVersion!=BLDLEVEL_RELEASE) ||
            (strncmp(ptagregisterLocal->ucVersion, BLDLEVEL_VERSION, strlen(ptagregisterLocal->ucVersion)))))
#else
        if((ptagregisterLocal!=0) && (ptagregisterLocal->ulVersion!=BLDLEVEL_RELEASE))
#endif  /* DEBUG */
            ulUpdate=TRUE;
                                        /* If all conditions are fullfilled, then post the user
                                           registration information and add the registration date */
        if((pHP->ulRegistrationFlag & REGISTER_USER) &&
            (strlen(pHP->ucUserFirstName)) &&
            (strlen(pHP->ucUserLastName)) &&
            (strlen(pHP->ucUserEmailAddress)) &&
            (ulUpdate==TRUE))
            {
#ifdef  DEBUG
            printf("PC2Net: Posting Registration\n");
#endif  /* DEBUG */
            WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Posting user registration information ...")), NULL);
            sprintf(ucPostUrl, "Subject=PC/2+V%s+Online+Automatic+Update"
                "&FirstName=%s"
                "&LastName=%s"
                "&Email=%s"
                "&TCP/IP Address=%s"
                "&Hostname=%s",
                BLDLEVEL_VERSION,
                pHP->ucUserFirstName,
                pHP->ucUserLastName,
                pHP->ucUserEmailAddress,
                (pHP->ucLocalAddress[0]!='\0' ? pHP->ucLocalAddress : "(n/a)"),
                (pHP->ucLocalHostname[0]!='\0' ? pHP->ucLocalHostname : "(n/a)"));
            ParseUrl(&urlProxy, pHP->ucProxyUrl, 80, 80);
            ParseUrl(&urlServer, pHP->ucRegistrationUrl, 80, 80);
            if(pHP->ucProxyUrl[0]!='\0')
                purlProxy=&urlProxy;
            lErrorCode=ConnectionOpen(purlProxy, &urlServer);
            lErrorCode=RequestPost(purlProxy, &urlServer, "cgi-bin/homestead/mail.pl?warpguru", ucPostUrl, FALSE);
            if(lErrorCode==NO_ERROR)
                {
                lErrorCode=RequestReceive(purlProxy, &urlServer, &pucDocument, &ulDocumentLength);
                if((lErrorCode==401) || (lErrorCode==407))
                    {
                    WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Posting user registration information "\
                        "using Authorization ...")), NULL);
                    DosSleep(2000);
                    lErrorCode=ConnectionOpen(purlProxy, &urlServer);
                    lErrorCode=RequestPost(purlProxy, &urlServer, "cgi-bin/homestead/mail.pl?warpguru", ucPostUrl, TRUE);
                    if(lErrorCode==NO_ERROR)
                        lErrorCode=RequestReceive(purlProxy, &urlServer, &pucDocument, &ulDocumentLength);
                    }
                if(lErrorCode==NO_ERROR)
                    ulUpdateRemote=TRUE;
                }
            if(lErrorCode!=NO_ERROR)
                WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Posting user registration failed ...")), NULL);
            ConnectionClose(purlProxy, &urlServer);
            DosSleep(2000);
            }
                                        /* If an existing registration should be preserved, then copy
                                           it to the remote information as we're going to save from that data,
                                           otherwise create a new one */
        pautoupdateCurrent=pCruntime->pautoupdateRemote;
        if(pautoupdateCurrent!=0)
            {
            AUTOUPDATE  *pautoupdateRegister;
            TAGREGISTER *ptagregisterRemote;
            UCHAR       ucTemp[]="x.yyz";


            while(pautoupdateCurrent->pNext!=0)
                pautoupdateCurrent=pautoupdateCurrent->pNext;
            pautoupdateRegister=MALLOC(sizeof(AUTOUPDATE));
            memset(pautoupdateRegister, 0, sizeof(AUTOUPDATE));
            pautoupdateCurrent->pNext=pautoupdateRegister;
            pautoupdateRegister->pPrev=pautoupdateCurrent;
            ptagregisterRemote=MALLOC(sizeof(TAGREGISTER));
            memset(ptagregisterRemote, 0, sizeof(TAGREGISTER));
            if(lErrorCode==NO_ERROR)
                {
                                        /* If registration did work, fill in current information */
                if((ptagregisterLocal!=0) && (ulUpdateRemote==FALSE))
                    {
                    *ptagregisterRemote=*ptagregisterLocal;
                    }
                else
                    {
                    ptagregisterRemote->ulDay=pCruntime->tm_Current.tm_mday;
                    ptagregisterRemote->ulMonth=pCruntime->tm_Current.tm_mon+1;
                    ptagregisterRemote->ulYear=pCruntime->tm_Current.tm_year+1900;
                    ptagregisterRemote->ulVersion=BLDLEVEL_RELEASE;
                    strncpy(ucTemp, BLDLEVEL_VERSION, sizeof(ucTemp)-1);
                    strcpy(ptagregisterRemote->ucVersion, ucTemp);
                    }
                }
            else
                {
                                        /* If registration failed, fill in NULL information or
                                           the data from the last successful registration */
                ptagregisterRemote->ulDay=1;
                ptagregisterRemote->ulMonth=1;
                ptagregisterRemote->ulYear=2000;
                ptagregisterRemote->ulVersion=0;
                strncpy(ucTemp, "0.00", sizeof(ucTemp)-1);
                strcpy(ptagregisterRemote->ucVersion, ucTemp);
                if(ptagregisterLocal!=0)
                    *ptagregisterRemote=*ptagregisterLocal;
                }
            pautoupdateRegister->ulStatus=AUTOUPDATE_REGISTER;
            pautoupdateRegister->Tags.tagRegister=ptagregisterRemote;
            }
        else
            lErrorCode=ERROR_INVALID_FUNCTION;
        return((MRESULT)lErrorCode);
        }
    return((MRESULT)NO_ERROR);
    }

/*                                                                                      *\
 * Syntax: WM_NETSTATUS, UCHAR *pucStatusText, NULL                                     *
\*                                                                                      */
case WM_NETSTATUS:
/*                                                                                      *\
 * This message is sent from within PC2NET.DLL functions to be able to inform the user  *
 * about the current status of the Automatic Update. The status text passed is a copy   *
 * and has to be freed afterwards therefore (by the Status window)                      *
\*                                                                                      */
    {
    UCHAR   *pucStatusText=PVOIDFROMMP(mp1);

    if(pucStatusText!=0)
        {
        if((pHP->hwndDesktopDialog) && (pCruntime->hwndStatusText!=NULLHANDLE))
            WinSetWindowText(pCruntime->hwndStatusText, pucStatusText);
        }
    if(pHP->hwndStatusClient!=NULLHANDLE)
        WinPostMsg(pHP->hwndStatusClient, msg, mp1, mp2);
    else
        FREE(pucStatusText);
    }
    return((MRESULT)NO_ERROR);
}
return(FALSE);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure parses the automatic update document either downloaded from the       *
 * Internet or loaded locally from a file. This is the basis to determine if there are  *
 * updates available on the Internet.                                                   *
 * Req:                                                                                 *
 *      pucFilename ... Fully qualified path of the local mirror file, or NULL when     *
 *                      the document was loaded from the Internet                       *
 *      pucDocument ... Document downloaded from the Internet, or NULL to load from     *
 *                      local mirror of the update information                          *
 * Ret:                                                                                 *
 *      autoupdateRoot  AUTOUPDATE root structure reflecting parsed data                *
\*--------------------------------------------------------------------------------------*/
AUTOUPDATE  *LoadAutoUpdate(UCHAR *pucFilename, UCHAR *pucDocument)
{
AUTOUPDATE  *pautoupdateRoot=0;
AUTOUPDATE  *pautoupdateCurrent=0;
AUTOUPDATE  *pautoupdatePrevious=0;
TAGSTATUS   *ptagstatusCurrent=0;
TAGREGISTER *ptagregisterCurrent=0;
TAGNEWS     *ptagnewsCurrent=0;
TAGNEWS     *ptagnewsPrevious=0;
TAGUPDATE   *ptagupdateCurrent=0;
TAGUPDATE   *ptagupdatePrevious=0;
UCHAR       ucTag[CCHMAXPATH];
UCHAR       *pucBegin;
UCHAR       *pucEnd;
ULONG       ulLength;

                                        /* If we don't parse from a document already in
                                           memory (that is something not downloaded from
                                           the internet), then we have to load it from a
                                           local mirror file first */
if(pucDocument==0)
    {
    FILESTATUS3     filestatusFileIn;
    int             iFileIn;

    if(pucFilename==0)
        return(NULL);
    if(DosQueryPathInfo(pucFilename, FIL_STANDARD, &filestatusFileIn, sizeof(filestatusFileIn))!=NO_ERROR)
        return(NULL);
    ulLength=filestatusFileIn.cbFile;
    pucBegin=pucEnd=MALLOC(ulLength+1);
    memset(pucBegin, 0, ulLength+1);
    iFileIn=open(pucFilename, O_BINARY|O_RDONLY, S_IREAD);
    read(iFileIn, pucBegin, ulLength);
    close(iFileIn);
    }
                                        /* Now parse though the document and build up a
                                           AUTOUPDATE structure. Only for documents downloaded
                                           from the internet we first have to find the
                                           Product tag as a starting point */
else
    {
    pucBegin=pucDocument;
    strcpy(ucTag, "<h2>Product: ");
    strcat(ucTag, BLDLEVEL_PRODUCT);
    strcat(ucTag, "</h2>");
    pucBegin=strstr(pucBegin, ucTag);
    if(pucBegin==0)
        return(0);
    pucBegin+=strlen(ucTag);
    }
                                        /* Now we point to the data for the product we are
                                           interested in, so we have to loop through all the
                                           tags we're interested in */
while(TRUE)
    {
    while((*pucBegin=='\r') || (*pucBegin=='\n') || (*pucBegin==' '))
        pucBegin++;
                                        /* If we find a Status tag parse it completely. This tag
                                           should only be found when loading from the local mirror */
    if(pucDocument!=0)
        strcpy(ucTag, "<h3>Status: ");
    else
        strcpy(ucTag, "<Status ");
    if(!STRNICMP(pucBegin, ucTag, strlen(ucTag)))
        {
                                        /* First, allocate the required control structures */
        ptagstatusCurrent=MALLOC(sizeof(TAGSTATUS));
        memset(ptagstatusCurrent, 0, sizeof(TAGSTATUS));
        pautoupdateCurrent=MALLOC(sizeof(AUTOUPDATE));
        memset(pautoupdateCurrent, 0, sizeof(AUTOUPDATE));
        pautoupdateCurrent->Tags.tagStatus=ptagstatusCurrent;
        pautoupdateCurrent->ulStatus=AUTOUPDATE_STATUS;
                                        /* Second, parse the Status tag */
        pucBegin+=strlen(ucTag);
        pucEnd=pucBegin;
        ptagstatusCurrent->ulDay=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        if((*pucEnd=='.') || (*pucEnd=='/'))
            pucEnd++;
        ptagstatusCurrent->ulMonth=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        if((*pucEnd=='.') || (*pucEnd=='/'))
            pucEnd++;
        ptagstatusCurrent->ulYear=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        if(ptagstatusCurrent->ulYear<80)
            ptagstatusCurrent->ulYear+=2000;
        else
            if(ptagstatusCurrent->ulYear<100)
            ptagstatusCurrent->ulYear+=1900;
        while(*pucEnd==' ')
            pucEnd++;
        ptagstatusCurrent->ulHour=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        if(*pucEnd==':')
            pucEnd++;
        ptagstatusCurrent->ulMinute=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        while((*pucEnd!='>') && (*pucEnd!='\0'))
            pucEnd++;
        pucEnd++;
        while((*pucEnd=='\r') || (*pucEnd=='\n') || (*pucEnd==' '))
            pucEnd++;
        ulLength=0;
        while((*pucEnd!='<') && (*pucEnd!='\0'))
            pucEnd++;
        pucBegin=pucEnd;
        goto TAG_FOUND;
        }
                                        /* If we find a Register tag parse it completely. This tag
                                           should only be found when loading from the local mirror */
    if(pucDocument!=0)
        strcpy(ucTag, "<h3>Registration: ");
    else
        strcpy(ucTag, "<Registration ");
    if(!STRNICMP(pucBegin, ucTag, strlen(ucTag)))
        {
                                        /* First, allocate the required control structures */
        ptagregisterCurrent=MALLOC(sizeof(TAGREGISTER));
        memset(ptagregisterCurrent, 0, sizeof(TAGREGISTER));
        pautoupdateCurrent=MALLOC(sizeof(AUTOUPDATE));
        memset(pautoupdateCurrent, 0, sizeof(AUTOUPDATE));
        pautoupdateCurrent->Tags.tagRegister=ptagregisterCurrent;
        pautoupdateCurrent->ulStatus=AUTOUPDATE_REGISTER;
                                        /* Second, parse the Registration tag */
        pucBegin+=strlen(ucTag);
        pucEnd=pucBegin;
        ptagregisterCurrent->ulDay=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        if((*pucEnd=='.') || (*pucEnd=='/'))
            pucEnd++;
        ptagregisterCurrent->ulMonth=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        if((*pucEnd=='.') || (*pucEnd=='/'))
            pucEnd++;
        ptagregisterCurrent->ulYear=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        if(ptagregisterCurrent->ulYear<80)
            ptagregisterCurrent->ulYear+=2000;
        else
            if(ptagregisterCurrent->ulYear<100)
            ptagregisterCurrent->ulYear+=1900;
        while((*pucEnd!='>') && (*pucEnd!='\0'))
            pucEnd++;
        pucEnd++;
        while((*pucEnd=='\r') || (*pucEnd=='\n') || (*pucEnd==' '))
            pucEnd++;
        pucBegin=pucEnd;
        if(!STRNICMP(pucBegin, "Version: ", sizeof("Version: ")-1))
            {
            pucEnd=pucBegin+=sizeof("Version: ")-1;
            ulLength=0;
            while((*pucEnd!='\r') && (*pucEnd!='\n') && (*pucEnd!=' ') &&
                (*pucEnd!='\0') && (ulLength<sizeof(ptagregisterCurrent->ucVersion)))
                {
                ptagregisterCurrent->ucVersion[ulLength++]=*pucEnd;
                if(isdigit(*pucEnd))
                    ptagregisterCurrent->ulVersion=ptagregisterCurrent->ulVersion*10+(*pucEnd-'0');
                pucEnd++;
                }
            }
        ulLength=0;
        while((*pucEnd!='<') && (*pucEnd!='\0'))
            pucEnd++;
        pucBegin=pucEnd;
                                        /* If the tag could be read, accept its contents even
                                           if it is empty as we don't know better, and not try
                                           to do:
                                           if(ptagregisterCurrent->ulVersion==0)
                                               {
                                               UCHAR   ucTemp[]="x.yyz";
                                   
                                               strncpy(ucTemp, BLDLEVEL_VERSION, sizeof(ucTemp)-1);
                                               strcpy(ptagregisterCurrent->ucVersion, ucTemp);
                                               ptagregisterCurrent->ulVersion=BLDLEVEL_RELEASE;
                                               }
                                           */
        goto TAG_FOUND;
        }
                                        /* Next, parse the News tag. If we find a News tag
                                           parse it completely */
    if(pucDocument!=0)
        strcpy(ucTag, "<h3>News: ");
    else
        strcpy(ucTag, "<News ");
    if(!STRNICMP(pucBegin, ucTag, strlen(ucTag)))
        {
                                        /* First, allocate the required control structures */
        ptagnewsCurrent=MALLOC(sizeof(TAGNEWS));
        memset(ptagnewsCurrent, 0, sizeof(TAGNEWS));
        if(ptagnewsPrevious==0)
            {
            pautoupdateCurrent=MALLOC(sizeof(AUTOUPDATE));
            memset(pautoupdateCurrent, 0, sizeof(AUTOUPDATE));
            pautoupdateCurrent->Tags.tagNews=ptagnewsPrevious=ptagnewsCurrent;
            pautoupdateCurrent->ulStatus=AUTOUPDATE_NEWS;
            }
        else
            {
            ptagnewsPrevious->pNext=ptagnewsCurrent;
            ptagnewsCurrent->pPrev=ptagnewsPrevious;
            }
        ptagnewsPrevious=ptagnewsCurrent;
                                        /* Second, parse the News tag */
        pucBegin+=strlen(ucTag);
        pucEnd=pucBegin;
        ptagnewsCurrent->ulDay=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        if((*pucEnd=='.') || (*pucEnd=='/'))
            pucEnd++;
        ptagnewsCurrent->ulMonth=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        if((*pucEnd=='.') || (*pucEnd=='/'))
            pucEnd++;
        ptagnewsCurrent->ulYear=atoi(pucEnd);
        while(isdigit(*pucEnd))
            pucEnd++;
        if(ptagnewsCurrent->ulYear<80)
            ptagnewsCurrent->ulYear+=2000;
        else
            if(ptagnewsCurrent->ulYear<100)
            ptagnewsCurrent->ulYear+=1900;
        while((*pucEnd!='>') && (*pucEnd!='\0'))
            pucEnd++;
        pucEnd++;
        while((*pucEnd=='\r') || (*pucEnd=='\n') || (*pucEnd==' '))
            pucEnd++;
        pucBegin=pucEnd;
        ulLength=0;
        while((*pucEnd!='<') && (*pucEnd!='\0'))
            pucEnd++;
        ulLength=(ULONG)pucEnd-(ULONG)pucBegin;
        ptagnewsCurrent->pucNews=MALLOC(ulLength+1);
        memset(ptagnewsCurrent->pucNews, '\0', ulLength+1);
        pucEnd=ptagnewsCurrent->pucNews;
        while(ulLength>0)
            {
            if(*pucBegin!='\r')
                {
                if(*pucBegin!='\n')
                    *pucEnd++=*pucBegin;
                else
                    *pucEnd++=' ';
                }
            pucBegin++;
            ulLength--;
            }
        goto TAG_FOUND;
        }
                                        /* If we find a Update tag parse it completely */
    if(pucDocument!=0)
        strcpy(ucTag, "<h3>Update:</h3>");
    else
        strcpy(ucTag, "<Update>");
    if(!STRNICMP(pucBegin, ucTag, strlen(ucTag)))
        {
                                        /* First, parse the Update tag */
        pucBegin+=strlen(ucTag);
        while((*pucBegin=='\r') || (*pucBegin=='\n') || (*pucBegin==' '))
            pucBegin++;
        while(TRUE)
            {
            if(!STRNICMP(pucBegin, "Replacement: ", sizeof("Replacement: ")-1))
                {
                pucEnd=pucBegin+sizeof("Replacement: ")-1;
                ptagupdateCurrent=MALLOC(sizeof(TAGUPDATE));
                memset(ptagupdateCurrent, 0, sizeof(TAGUPDATE));
                ptagupdateCurrent->ulStatus=TAGUPDATE_REPLACEMENT;
                }
            else if(!STRNICMP(pucBegin, "Addition: ", sizeof("Addition: ")-1))
                {
                pucEnd=pucBegin+sizeof("Addition: ")-1;
                ptagupdateCurrent=MALLOC(sizeof(TAGUPDATE));
                memset(ptagupdateCurrent, 0, sizeof(TAGUPDATE));
                ptagupdateCurrent->ulStatus=TAGUPDATE_ADDITION;
                }
                                        /* Second, allocate the required control structures */
            if((ptagupdateCurrent!=0) && (pucBegin!=pucEnd))
                {
                if(ptagupdatePrevious==0)
                    {
                    pautoupdateCurrent=MALLOC(sizeof(AUTOUPDATE));
                    memset(pautoupdateCurrent, 0, sizeof(AUTOUPDATE));
                    pautoupdateCurrent->Tags.tagUpdate=ptagupdatePrevious=ptagupdateCurrent;
                    pautoupdateCurrent->ulStatus=AUTOUPDATE_UPDATE;
                    }
                else
                    {
                    ptagupdatePrevious->pNext=ptagupdateCurrent;
                    ptagupdateCurrent->pPrev=ptagupdatePrevious;
                    }
                ptagupdatePrevious=ptagupdateCurrent;
                ulLength=0;
                while((*pucEnd!='\r') && (*pucEnd!='\n') && (*pucEnd!=' ') && (*pucEnd!='\0'))
                    ptagupdateCurrent->ucFilename[ulLength++]=*pucEnd++;
                while((*pucEnd=='\r') || (*pucEnd=='\n') || (*pucEnd==' '))
                    pucEnd++;
                pucBegin=pucEnd;
                STRIPTRAILING(ptagupdateCurrent->ucFilename);
                }
                                        /* Third, search for Update tag keywords, and ignore unknown ones */
            if(ptagupdateCurrent!=0)
                {
                if(!STRNICMP(pucBegin, "Description: ", sizeof("Description: ")-1))
                    {
                    pucEnd=pucBegin+=sizeof("Description: ")-1;
                    ulLength=0;
                    while((*pucEnd!='\r') && (*pucEnd!='\n') &&
                        (*pucEnd!='\0') && (ulLength<sizeof(ptagupdateCurrent->ucDescription)))
                        {
                        if((*pucEnd!='\r') && (*pucEnd!='\n'))
                            ptagupdateCurrent->ucDescription[ulLength++]=*pucEnd++;
                        }
                    STRIPTRAILING(ptagupdateCurrent->ucDescription);
                    }
                else if(!STRNICMP(pucBegin, "Version: ", sizeof("Version: ")-1))
                    {
                    pucEnd=pucBegin+=sizeof("Version: ")-1;
                    ulLength=0;
                    while((*pucEnd!='\r') && (*pucEnd!='\n') &&
                        (*pucEnd!='\0') && (ulLength<sizeof(ptagupdateCurrent->ucVersion)))
                        ptagupdateCurrent->ucVersion[ulLength++]=*pucEnd++;
                    STRIPTRAILING(ptagupdateCurrent->ucVersion);
                    }
                else if(!STRNICMP(pucBegin, "Size: ", sizeof("Size: ")-1))
                    {
                    pucEnd=pucBegin+=sizeof("Size: ")-1;
                    ptagupdateCurrent->ulSize=atoi(pucEnd);
                    while(isdigit(*pucEnd))
                        pucEnd++;
                    }
                else if(!STRNICMP(pucBegin, "Crc: ", sizeof("Crc: ")-1))
                    {
                    pucEnd=pucBegin+=sizeof("Crc: ")-1;
                    ptagupdateCurrent->ulCrc=atoi(pucEnd);
                    while(isdigit(*pucEnd))
                        pucEnd++;
                    }
                else if(!STRNICMP(pucBegin, "Url: ", sizeof("Url: ")-1))
                    {
                    pucEnd=pucBegin+=sizeof("Url: ")-1;
                    ulLength=0;
                    while((*pucEnd!='\r') && (*pucEnd!='\n') &&
                        (*pucEnd!='\0') && (ulLength<sizeof(ptagupdateCurrent->ucUrl)))
                        ptagupdateCurrent->ucUrl[ulLength++]=*pucEnd++;
                    STRIPTRAILING(ptagupdateCurrent->ucUrl);
                    }
                else
                    {
                    while((*pucEnd!='\r') && (*pucEnd!='\n') && (*pucEnd!='<') && (*pucEnd!='\0'))
                        pucEnd++;
                    }
                while((*pucEnd=='\r') || (*pucEnd=='\n') || (*pucEnd==' '))
                    pucEnd++;
                pucBegin=pucEnd;
                ptagupdatePrevious=ptagupdateCurrent;
                if((*pucBegin=='<') || (*pucBegin=='\0'))
                    {
                    while(*pucBegin=='<')
                        {
                        if(!STRNICMP(pucBegin, "<br>", sizeof("<br>")-1))
                            pucBegin+=sizeof("<br>")-1;
                        else if(!STRNICMP(pucBegin, "<p>", sizeof("<p>")-1))
                            pucBegin+=sizeof("<p>")-1;
                        else
                            break;
                        while((*pucBegin=='\r') || (*pucBegin=='\n') || (*pucBegin==' '))
                            pucBegin++;
                        }
                    pucEnd=pucBegin;
                    if((*pucBegin=='<') || (*pucBegin=='\0'))
                        break;
                    }
                }
            else
                break;
            }
            goto TAG_FOUND;
        }
    else
        {
                                        /* If no known tag was found, then we have to look for
                                           the beginning of the next tag or the end, or we'll
                                           loop endlessly */
        if(pucDocument!=0)
            strcpy(ucTag, "<h3>");
        else
            strcpy(ucTag, "<");
        pucEnd=pucBegin;
        pucBegin=strstr(pucBegin+1, ucTag);
        if(pucBegin==0)
            pucBegin=strchr(pucEnd, '\0');
        }
TAG_FOUND:
                                        /* Link newly found tag into AUTOUPDATE list */
    if((pautoupdateCurrent!=0) && (pautoupdateCurrent!=pautoupdatePrevious))
        {
        if(pautoupdateRoot==0)
            pautoupdateRoot=pautoupdateCurrent;
        else
            {
            pautoupdatePrevious->pNext=pautoupdateCurrent;
            pautoupdateCurrent->pPrev=pautoupdatePrevious;
            }
        pautoupdatePrevious=pautoupdateCurrent;
        }
                                        /* Break out of loop if nothing else left to parse */
    if((*pucBegin=='\0') ||
        (!STRNICMP(pucBegin, "<!--- End --->", sizeof("<!--- End --->")-1)) ||
        (!STRNICMP(pucBegin, "<h2>Product: ", sizeof("<h2>Product: ")-1)))
        break;
    }
return(pautoupdateRoot);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure writes the automatic update information into a file. For the news we  *
 * only write the latest news, to avoid logging unnecessary backlogs (and the old news  *
 * are deleted). The returned result should then replace the previous local mirror.     *
 * Req:                                                                                 *
 *      pucFilename ... Fully qualified path of the local mirror file                   *
 *      pucDocument ... Document to be saved as the local mirror of the update          *
 *                      information                                                     *
 * Ret:                                                                                 *
 *      autoupdateRoot  AUTOUPDATE root structure, or NULL in case of an eror           *
\*--------------------------------------------------------------------------------------*/
AUTOUPDATE  *SaveAutoUpdate(UCHAR *pucFilename, AUTOUPDATE *pautoupdateRoot)
{
AUTOUPDATE  *pautoupdateCurrent=0;
AUTOUPDATE  *pautoupdateInsert=0;
TAGSTATUS   *ptagstatusCurrent=0;
TAGREGISTER *ptagregisterCurrent=0;
TAGNEWS     *ptagnewsCurrent=0;
TAGNEWS     *ptagnewsLatest=0;
TAGNEWS     *ptagnewsNext=0;
TAGUPDATE   *ptagupdateCurrent=0;
FILE        *pfileUpdate=0;

                                        /* Check parameters */
if((pautoupdateRoot==0) || (pucFilename==0))
    return(NULL);
                                        /* Open automatic update information file */
pfileUpdate=FOPEN(pucFilename, "w");
if(pfileUpdate==0)
    return(NULL);
pautoupdateCurrent=pautoupdateRoot;
                                        /* As the remote automatic update information may not
                                           contain a status tag, one may be needed to be created
                                           artificial. If available, it will be the first tag,
                                           otherwise we replace the first tag with a newly allocated
                                           Status tag */
if(pautoupdateCurrent->ulStatus & AUTOUPDATE_STATUS)
    {
    ptagstatusCurrent=pautoupdateCurrent->Tags.tagStatus;
    }
else
    {
    ptagstatusCurrent=MALLOC(sizeof(TAGSTATUS));
    memset(ptagstatusCurrent, 0, sizeof(TAGSTATUS));
    pautoupdateInsert=MALLOC(sizeof(AUTOUPDATE));
    memcpy(pautoupdateInsert, pautoupdateCurrent, sizeof(AUTOUPDATE));
    memset(pautoupdateCurrent, 0, sizeof(AUTOUPDATE));
    pautoupdateInsert->pPrev=pautoupdateCurrent;
    pautoupdateCurrent->ulStatus=AUTOUPDATE_STATUS;
    pautoupdateCurrent->Tags.tagStatus=ptagstatusCurrent;
    pautoupdateCurrent->pNext=pautoupdateInsert;
    }
if(ptagstatusCurrent!=0)
    {
    ptagstatusCurrent->ulDay=pCruntime->tm_Current.tm_mday;
    ptagstatusCurrent->ulMonth=pCruntime->tm_Current.tm_mon+1;
    ptagstatusCurrent->ulYear=pCruntime->tm_Current.tm_year+1900;
    ptagstatusCurrent->ulHour=pCruntime->tm_Current.tm_hour;
    ptagstatusCurrent->ulMinute=pCruntime->tm_Current.tm_min;
    FPRINTF(pfileUpdate, "<Status %ld.%ld.%ld %ld:%ld>\n",
        ptagstatusCurrent->ulDay, ptagstatusCurrent->ulMonth, ptagstatusCurrent->ulYear,
        ptagstatusCurrent->ulHour, ptagstatusCurrent->ulMinute);
    }
                                        /* If we found the register information, just file that as
                                           the second entry into PC2.NET */
while(pautoupdateCurrent!=0)
    {
    if(pautoupdateCurrent->ulStatus & AUTOUPDATE_REGISTER)
        {
        ptagregisterCurrent=pautoupdateCurrent->Tags.tagRegister;
        FPRINTF(pfileUpdate, "<Registration %ld.%ld.%ld>\n",
            ptagregisterCurrent->ulDay, ptagregisterCurrent->ulMonth, ptagregisterCurrent->ulYear);
        if(ptagregisterCurrent->ucVersion[0]!='\0')
            FPRINTF(pfileUpdate, " Version: %s\n", ptagregisterCurrent->ucVersion);
        }
    pautoupdateCurrent=pautoupdateCurrent->pNext;
    }
pautoupdateCurrent=pautoupdateRoot;
while(pautoupdateCurrent!=0)
    {
                                        /* If we found the news information, then locate the
                                           latest news and just file that. After that delete
                                           all news but the latest */
    if(pautoupdateCurrent->ulStatus & AUTOUPDATE_NEWS)
        {
        ptagnewsLatest=ptagnewsCurrent=pautoupdateCurrent->Tags.tagNews;
        while(ptagnewsCurrent!=0)
            {
            if(((ptagnewsCurrent->ulYear*10000)+(ptagnewsCurrent->ulMonth*100)+ptagnewsCurrent->ulDay)>
                ((ptagnewsLatest->ulYear*10000)+(ptagnewsLatest->ulMonth*100)+ptagnewsLatest->ulDay))
                ptagnewsLatest=ptagnewsCurrent;
            ptagnewsCurrent=ptagnewsCurrent->pNext;
            }
        FPRINTF(pfileUpdate, "<News %ld.%ld.%ld>\n",
            ptagnewsLatest->ulDay, ptagnewsLatest->ulMonth, ptagnewsLatest->ulYear);
        FPRINTF(pfileUpdate, " %s\n", ptagnewsLatest->pucNews);
        ptagnewsNext=pautoupdateCurrent->Tags.tagNews;
        pautoupdateCurrent->Tags.tagNews=0;
        while(ptagnewsNext!=0)
            {
            ptagnewsCurrent=ptagnewsNext;
            ptagnewsNext=ptagnewsCurrent->pNext;
            if(ptagnewsCurrent!=ptagnewsLatest)
                {
                FREE(ptagnewsCurrent->pucNews);
                FREE(ptagnewsCurrent);
                }
            else
                {
                pautoupdateCurrent->Tags.tagNews=ptagnewsLatest;
                ptagnewsLatest->pPrev=ptagnewsLatest->pNext=0;
                }
            }
        }
                                        /* If we found the update information, then enumerate all
                                           list elements and log them to file */
    else if(pautoupdateCurrent->ulStatus & AUTOUPDATE_UPDATE)
        {
        ptagupdateCurrent=pautoupdateCurrent->Tags.tagUpdate;
        FPRINTF(pfileUpdate, "<Update>\n");
        while(ptagupdateCurrent)
            {
            if(ptagupdateCurrent->ulStatus & TAGUPDATE_REPLACEMENT)
                FPRINTF(pfileUpdate, " Replacement: %s\n", ptagupdateCurrent->ucFilename);
            else if(ptagupdateCurrent->ulStatus & TAGUPDATE_ADDITION)
                FPRINTF(pfileUpdate, " Addition: %s\n", ptagupdateCurrent->ucFilename);
            if(ptagupdateCurrent->ucDescription[0]!='\0')
                FPRINTF(pfileUpdate, "  Description: %s\n", ptagupdateCurrent->ucDescription);
            if(ptagupdateCurrent->ucVersion[0]!='\0')
                FPRINTF(pfileUpdate, "  Version: %s\n", ptagupdateCurrent->ucVersion);
            if(ptagupdateCurrent->ulSize!=0)
                FPRINTF(pfileUpdate, "  Size: %ld\n", ptagupdateCurrent->ulSize);
            if(ptagupdateCurrent->ulCrc!=0)
                FPRINTF(pfileUpdate, "  Crc: %ld\n", ptagupdateCurrent->ulCrc);
            if(ptagupdateCurrent->ucUrl[0]!='\0')
                FPRINTF(pfileUpdate, "  Url: %s\n", ptagupdateCurrent->ucUrl);
            ptagupdateCurrent=ptagupdateCurrent->pNext;
            }
        }
    pautoupdateCurrent=pautoupdateCurrent->pNext;
    }
FCLOSE(pfileUpdate);
return(pautoupdateRoot);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure frees a complete AUTOUPDATE structure (including all lists of lists). *
 * Req:                                                                                 *
 *      pautoupdateDestroy                                                              *
 *                      Root of AUTOUPDATE structure to free.                           *
 * Ret:                                                                                 *
 *      lReturnCode ... NO_ERROR if success, error otherwise                            *
\*--------------------------------------------------------------------------------------*/
LONG        FreeAutoUpdate(AUTOUPDATE *pautoupdateDestroy)
{
LONG        lReturnCode=NO_ERROR;

AUTOUPDATE  *pautoupdateCurrent=0;
TAGNEWS     *ptagnewsCurrent=0;
TAGNEWS     *ptagnewsNext=0;
TAGUPDATE   *ptagupdateCurrent=0;
TAGUPDATE   *ptagupdateNext=0;

                                        /* Check parameters */
if(pautoupdateDestroy==0)
    return(ERROR_INVALID_PARAMETER);
                                        /* Free list of lists */
for(pautoupdateCurrent=pautoupdateDestroy; pautoupdateCurrent!=0; pautoupdateCurrent=pautoupdateCurrent->pNext)
    {
    pautoupdateCurrent->pPrev=0;
                                        /* Free lists */
    if(pautoupdateCurrent->ulStatus & AUTOUPDATE_STATUS)
        {
        FREE(pautoupdateCurrent->Tags.tagStatus);
        pautoupdateCurrent->Tags.tagStatus=0;
        }
    if(pautoupdateCurrent->ulStatus & AUTOUPDATE_REGISTER)
        {
        FREE(pautoupdateCurrent->Tags.tagRegister);
        pautoupdateCurrent->Tags.tagRegister=0;
        }
    else if(pautoupdateCurrent->ulStatus & AUTOUPDATE_NEWS)
        {
        ptagnewsCurrent=pautoupdateCurrent->Tags.tagNews;
        pautoupdateCurrent->Tags.tagNews=0;
        for(ptagnewsNext=ptagnewsCurrent->pNext;
            ptagnewsCurrent!=0;
            ptagnewsCurrent=ptagnewsNext)
            {
            if(ptagnewsNext!=0)
                ptagnewsNext->pPrev=0;
            ptagnewsNext=ptagnewsCurrent->pNext;
            FREE(ptagnewsCurrent->pucNews);
            FREE(ptagnewsCurrent);
            }
        }
    else if(pautoupdateCurrent->ulStatus & AUTOUPDATE_UPDATE)
        {
        ptagupdateCurrent=pautoupdateCurrent->Tags.tagUpdate;
        pautoupdateCurrent->Tags.tagUpdate=0;
        for(ptagupdateNext=ptagupdateCurrent->pNext;
            ptagupdateCurrent!=0;
            ptagupdateCurrent=ptagupdateNext)
            {
            if(ptagupdateNext!=0)
                ptagupdateNext->pPrev=0;
            ptagupdateNext=ptagupdateCurrent->pNext=0;
            FREE(ptagupdateCurrent);
            }
        }
    }
return(lReturnCode);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure compares the automatic update document loaded from the Internet with  *
 * what is already available locally. If news are detected, the news are accumulated    *
 * into an allocated string and returned to the caller (who is responsible to free the  *
 * returned string then). The caller should also write the document loaded from the     *
 * Internet into a local file or the next time the same news will be detected as new.   *
 * Req:                                                                                 *
 *      autoupdateRemote                                                                *
 *                      Automatic update information downloaded from the Internet       *
 *      autoupdateLocal                                                                 *
 *                      Automatic update information loaded from local mirror           *
 * Ret:                                                                                 *
 *      pucNews ....... News information not already known locally, or NULL             *
\*--------------------------------------------------------------------------------------*/
UCHAR       *CheckNews(AUTOUPDATE *pautoupdateRemote, AUTOUPDATE *pautoupdateLocal)
{
UCHAR       *pucNews=0;
UCHAR       *pucNewsTemp=0;
ULONG       ulNewsLength=0;
TAGNEWS     *ptagnewsRemote=0;
TAGNEWS     *ptagnewsLocal=0;

                                        /* Check parameters */
if(pautoupdateRemote==0)
    return(NULL);
                                        /* Try to locate remote news information */
while(pautoupdateRemote!=0)
    {
    if(pautoupdateRemote->ulStatus & AUTOUPDATE_NEWS)
        {
        ptagnewsRemote=pautoupdateRemote->Tags.tagNews;
        break;
        }
    pautoupdateRemote=pautoupdateRemote->pNext;
    }
                                        /* Try to locate local news information */
while(pautoupdateLocal!=0)
    {
    if(pautoupdateLocal->ulStatus & AUTOUPDATE_NEWS)
        {
        ptagnewsLocal=pautoupdateLocal->Tags.tagNews;
        break;
        }
    pautoupdateLocal=pautoupdateLocal->pNext;
    }
                                        /* There are to be some remote news or we can't dislay
                                           any. Locally, we don't need to have news read yet,
                                           because this means that all remote news are unknown
                                           and should be returned as news (and SaveAutoUpdate()
                                           will then write the most recent one locally) */
for( ; ptagnewsRemote!=0; ptagnewsRemote=ptagnewsRemote->pNext)
    {
    if(ptagnewsLocal!=0)
        {
        if(((ptagnewsLocal->ulYear*10000)+(ptagnewsLocal->ulMonth*100)+ptagnewsLocal->ulDay)>=
            ((ptagnewsRemote->ulYear*10000)+(ptagnewsRemote->ulMonth*100)+ptagnewsRemote->ulDay))
            continue;
        }
                                        /* We found some news, thus append the news found to
                                           the news we've already found */
    pucNewsTemp=0;
    ulNewsLength=sizeof("99.99.9999: ")+strlen(ptagnewsRemote->pucNews);
    if(pucNews!=0)
        {
        pucNewsTemp=pucNews;
        ulNewsLength+=strlen(pucNewsTemp);
        ulNewsLength+=sizeof("\n\n");
        }
    pucNews=MALLOC(ulNewsLength);
    memset(pucNews, '\0', ulNewsLength);
    if(pucNewsTemp)
        {
        strcpy(pucNews, pucNewsTemp);
        strcat(pucNews, "\n\n");
        FREE(pucNewsTemp);
        }
    pucNewsTemp=strchr(pucNews, '\0');
    sprintf(pucNewsTemp, "%2ld.%2ld.%4ld: %s", ptagnewsRemote->ulDay, ptagnewsRemote->ulMonth,
        ptagnewsRemote->ulYear, ptagnewsRemote->pucNews);
    }
return(pucNews);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure compares the automatic update document loaded from the Internet with  *
 * what is already available locally. If updates are detected, the replacement/addition *
 * file is downloaded and the user is informet what to do. Only the first update is     *
 * downloaded, so this procedure must be called multiple times until NULL is returned,  *
 * which means there's just no more update information available. In case of network    *
 * problems -1 is returned, which means the operation should be retried only after a    *
 * delay (if LAN interfaces are available, the delay may be longer, when PPP interfaces *
 * are available the delay may be shorter).                                             *
 * Req:                                                                                 *
 *      purlServer .... URL structure filled with parsed Url                            *
 *      pucUrl ........ Url string to parse                                             *
 *      autoupdateRemote                                                                *
 *                      Automatic update information downloaded from the Internet       *
 *      autoupdateLocal                                                                 *
 *                      Automatic update information loaded from local mirror           *
 * Ret:                                                                                 *
 *      pucUpdate ..... Update information of downloaded files, or -1 or NULL.          *
\*--------------------------------------------------------------------------------------*/
UCHAR       *CheckUpdate(URL *purlProxy, URL *purlServer, AUTOUPDATE *pautoupdateRemote, AUTOUPDATE *pautoupdateLocal)
{
LONG        lErrorCode;
UCHAR       *pucUpdate=0;
TAGUPDATE   *ptagupdateRemote=0;
TAGUPDATE   *ptagupdateLocal=0;
TAGUPDATE   *ptagupdateLocalTemp=0;
TAGUPDATE   *ptagupdateFound;
URL         urlRemote;
URL         *purlRemote;
UCHAR       *pucFile=0;
ULONG       ulFileLength=0;
UCHAR       *pucUrlTemp=0;

                                        /* Check parameters */
if((pautoupdateRemote==0))
    return(NULL);
                                        /* Try to locate remote update information */
while(pautoupdateRemote!=0)
    {
    if(pautoupdateRemote->ulStatus & AUTOUPDATE_UPDATE)
        {
        ptagupdateRemote=pautoupdateRemote->Tags.tagUpdate;
        break;
        }
    pautoupdateRemote=pautoupdateRemote->pNext;
    }
                                        /* Try to locate local update information */
while(pautoupdateLocal!=0)
    {
    if(pautoupdateLocal->ulStatus & AUTOUPDATE_UPDATE)
        {
        ptagupdateLocal=pautoupdateLocal->Tags.tagUpdate;
        break;
        }
    pautoupdateLocal=pautoupdateLocal->pNext;
    }
                                        /* There are to be some remote updates or we can't download
                                           anything. For all remote updates we check if the download
                                           was already mirrored locally, otherwise we download the
                                           update unconditionally. As already downloaded updates
                                           should not be downloaded again, check for the
                                           TAGUPDATE_DOWNLOADED flag (which is set after a successful
                                           download) */
for( ; ptagupdateRemote!=0; ptagupdateRemote=ptagupdateRemote->pNext)
    {
    if(ptagupdateRemote->ulStatus & TAGUPDATE_DOWNLOADED)
        continue;
    ptagupdateFound=0;
                                        /* For the current remote update enumerate through all local
                                           updates (that had been filed from a previous session).
                                           If the contents except for the list management pointers
                                           are equivalent, then the update can be ignored, if the
                                           remote update is not found locally, then it is new and
                                           should be downloaded, otherwise assume it's new unless
                                           we find it locally too */
    ptagupdateFound=ptagupdateRemote;
    if(ptagupdateLocal!=0)
        {
        for(ptagupdateLocalTemp=ptagupdateLocal;
            ptagupdateLocalTemp!=0; ptagupdateLocalTemp=ptagupdateLocalTemp->pNext)
            {
            if(!memcmp(ptagupdateLocalTemp, ptagupdateRemote, sizeof(TAGUPDATE)-sizeof(TAGUPDATE *)*2))
                {
                ptagupdateFound=0;
                break;
                }
            }
        }
                                        /* Now that we found an update to be downloaded, download it. The
                                           update information may specify the full Url of the document to
                                           be downloaded or only a path. In the first case we try to connect
                                           to the server specified in the Url, otherwise we type to connect
                                           to the server passed as an argument and download the document
                                           just via the specified path */
    if(ptagupdateFound!=0)
        {
        lErrorCode=NO_ERROR;
        while(TRUE)
            {
            purlRemote=&urlRemote;
            lErrorCode=ParseUrl(purlRemote, ptagupdateFound->ucUrl, 80, 80);
            if(lErrorCode==NO_ERROR)
                {
                lErrorCode=ConnectionOpen(purlProxy, purlRemote);
                if(lErrorCode)
                    break;
                lErrorCode=RequestSend(purlProxy, purlRemote, 0, FALSE);
                }
            else
                {
                purlRemote=purlServer;
                lErrorCode=ConnectionOpen(purlProxy, purlRemote);
                if(lErrorCode)
                    break;
                lErrorCode=RequestSend(purlProxy, purlRemote, ptagupdateFound->ucUrl, FALSE);
                }
            if(lErrorCode)
                break;
            lErrorCode=RequestReceive(purlProxy, purlRemote, &pucFile, &ulFileLength);
            if((lErrorCode==401) || (lErrorCode==407))
                {
                if(purlRemote!=purlServer)
                    lErrorCode=RequestSend(purlProxy, purlRemote, 0, FALSE);
                else
                    lErrorCode=RequestSend(purlProxy, purlRemote, ptagupdateFound->ucUrl, TRUE);
                if(lErrorCode)
                    break;
                lErrorCode=RequestReceive(purlProxy, purlRemote, &pucFile, &ulFileLength);
                }
            if(lErrorCode)
                break;
            if(pucFile!=0)
                {
                UCHAR   ucFilename[CCHMAXPATH];
                int     iFileOut;

                strcpy(ucFilename, pHP->ucPathDLL);
                if(ucFilename[strlen(ucFilename)-1]!='\\')
                    strcat(ucFilename, "\\");
                strcat(ucFilename, ptagupdateFound->ucFilename);
#ifdef  DEBUG_PC2NET
                printf("PC2Net: Downloading %s\n", ucFilename);
#endif  /* DEBUG_PC2NET */
                iFileOut=open(ucFilename, O_CREAT|O_BINARY|O_TRUNC|O_WRONLY, S_IWRITE);
                write(iFileOut, pucFile, ulFileLength);
                close(iFileOut);
                }
            break;
            }


                                        /* Return after this file, but first mark the remote update
                                           that we already have downloaded this update, so the next
                                           time this function is called, we skip this update */
        if(lErrorCode==NO_ERROR)
            {
            ptagupdateFound->ulStatus|=TAGUPDATE_DOWNLOADED;
            pucUpdate=MALLOC(strlen("The replacement file \"%s\" has been downloaded, manual intervention is "\
                "required. Please refer to the description below and select Help for more information!\n\n%s")
                +strlen(ptagupdateFound->ucFilename)
                +strlen(ptagupdateFound->ucDescription)
                +1);
            if(ptagupdateFound->ulStatus & TAGUPDATE_REPLACEMENT)
                sprintf(pucUpdate, "The replacement file \"%s\" has been downloaded, manual intervention is "\
                    "required. Please refer to the description below and select Help for more information!\n\n%s",
                    ptagupdateFound->ucFilename,
                    ptagupdateFound->ucDescription);
            else
                sprintf(pucUpdate, "The update \"%s\" has been downloaded, manual intervention might be "\
                    "required. Please refer to the description below and select Help for more information!\n\n%s",
                    ptagupdateFound->ucFilename,
                    ptagupdateFound->ucDescription);
            }
        else
            ConnectionClose(purlProxy, purlRemote);
        break;
        }

    }
return(pucUpdate);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure parses an Url and fills an URL structure with the data detected.      *
 * A Url should be of the form [protocol://]host.subdomain.domain[:port][/path]         *
 * Req:                                                                                 *
 *      purlServer .... URL structure filled with parsed Url                            *
 *      pucUrl ........ Url string to parse                                             *
 *      ulServiceDefault                                                                *
 *                      Default service to be used if none supplied                     *
 *      ulPortDefault . Default port to be used if none specified                       *
 * Mod:                                                                                 *
 *      purlServer                                                                      *
 * Ret:                                                                                 *
 *      lReturnCode ... NO_ERROR if success, error otherwise                            *
\*--------------------------------------------------------------------------------------*/
LONG    ParseUrl(URL *purlServer, UCHAR *pucUrl, ULONG ulServiceDefault, ULONG ulPortDefault)
{
LONG    lReturnCode=NO_ERROR;
UCHAR   *pucUrlCopy;
UCHAR   *pucBegin;
UCHAR   *pucEnd;
UCHAR   *pucTemp;

                                        /* Check parameters */
if((purlServer==0) || (pucUrl==0))
    return(ERROR_INVALID_PARAMETER);
memset(purlServer, 0, sizeof(URL));
pucUrlCopy=STRDUP(pucUrl);
while(TRUE)
    {
                                        /* Check if the specified Url does contain ://, which
                                           means that the first part should be a known
                                           protocol (that is defined in the TCP/IP services
                                           file. Note, in the URL structure we treat the data
                                           in host format not network byte order (thus we have
                                           to reverse again the result from getservbyname() as
                                           this call returns the service in network byte order) */
    pucBegin=pucUrlCopy;
    pucEnd=strstr(pucBegin, "://");
    if(pucEnd!=0)
        {
        struct servent  *pserventProtocol;

        *pucEnd='\0';
        if(!STRICMP(pucBegin, "http"))
            pserventProtocol=getservbyname("www-http", "tcp");
        else
            pserventProtocol=getservbyname(pucBegin, "tcp");
        if(pserventProtocol!=0)
            purlServer->ulService=htons(pserventProtocol->s_port);
        else
            purlServer->ulService=ulServiceDefault;
        pucBegin=pucEnd+sizeof("://")-1;
        }
    else
        purlServer->ulService=ulServiceDefault;
                                        /* Unless we find a difference, we assume that the service
                                           uses the default port */
    purlServer->ulPort=purlServer->ulService;
                                        /* So we are at the hostname, which can either be
                                           symbolic or dotted decimal, and either terminated
                                           by a port or an (possibly empty) path */
    pucEnd=strchr(pucBegin, ':');
    if(pucEnd!=0)
        {
        pucTemp=strchr(pucBegin, '/');
        if((pucTemp!=0) && (pucTemp<pucEnd))
            {
                                        /* host.subd/omain.domain:port/path\0
                                           !        |            |
                                           !        pucTemp      pucEnd
                                           pucBegin                                 */
            *pucTemp='\0';
            pucEnd=pucTemp+1;
            purlServer->ulPort=ulPortDefault;
            }
        else
            {
                                        /* host.subdomain.domain:port/path\0
                                           !                    !    |
                                           !                    !    pucTemp|0
                                           pucBegin             pucEnd              */
            *pucEnd='\0';
            pucEnd++;
            if(pucTemp!=0)
                *pucTemp='\0';
            purlServer->ulPort=(ULONG)atoi(pucEnd);
            if(pucTemp!=0)
                pucEnd=pucTemp+1;
            else
                while(isdigit(*pucEnd))
                    pucEnd++;
            }
        }
    else
        {
        if(ulPortDefault!=0)
            purlServer->ulPort=ulPortDefault;
        pucEnd=strchr(pucBegin, '/');
                                        /* host.subdomain.domain/path\0
                                           !                    !
                                           !                    !
                                           pucBegin             pucEnd|0            */
        if(pucEnd!=0)
            *(pucEnd++)='\0';
        else
            pucEnd=strchr(pucBegin, '\0');
        }
    strncpy(purlServer->ucHost, pucBegin, sizeof(purlServer->ucHost)-1);
    pucBegin=pucEnd;
                                        /* No we are at the requested path, which may be
                                           empty though */
    strncpy(purlServer->ucPath, pucBegin, sizeof(purlServer->ucPath)-1);
                                        /* Exit loop */
    break;
    }
FREE(pucUrlCopy);
#ifdef  DEBUG_PC2NET
printf("PC2Net: Url parsed: %ld://%s:%ld/%s\n", purlServer->ulService, purlServer->ucHost, purlServer->ulPort, purlServer->ucPath);
#endif  /* DEBUG_PC2NET */
                                        /* An empty hostname does not make sense, on the other
                                           hand an empty path means to load the root document */
if(purlServer->ucHost[0]=='\0')
    return(ERROR_INVALID_PARAMETER);
                                        /* Return */
return(lReturnCode);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure initializes the TCP/IP network interface.                             *
 * Ret:                                                                                 *
 *      lReturnCode ... NO_ERROR if success, error otherwise                            *
\*--------------------------------------------------------------------------------------*/
LONG    SocketInit(void)
{
LONG                lReturnCode=NO_ERROR;
UCHAR               ucMessage[EF_SIZE255];

WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP("Initializing TCP/IP sockets ...")), NULL);
DosSleep(2000);
                                        /* Initialize TCP/IP sockets */
if(sock_init()==1)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: sock_init() %ld\n", lReturnCode);
    psock_errno("");
#endif  /* DEBUG_PC2NET */
    sprintf(ucMessage, "Error %ld initializing TCP/IP sockets ...\n\n"
        "Module: %s\nLinenumber: %ld\n", lReturnCode, _FILE_, __LINE__);
    WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP(ucMessage)), NULL);
    DosSleep(2000);
    return(lReturnCode);
    }
return(lReturnCode);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure closes the connection to the serving host.                            *
 * Req:                                                                                 *
 *      pulConnectionLAN                                                                *
 *                      Number of LAN based interfaces available                        *
 *      pulConnectionPPP                                                                *
 *                      Number of Point-to-Point interfaces available                   *
 * Mod:                                                                                 *
 *      pulConnectionLAN                                                                *
 *      pulConnectionPPP                                                                *
 * Ret:                                                                                 *
 *      lReturnCode ... NO_ERROR if success, error otherwise                            *
\*--------------------------------------------------------------------------------------*/
LONG    InterfaceStatus(ULONG *pulConnectionLAN, ULONG *pulConnectionPPP)
{
LONG                lReturnCode=NO_ERROR;
struct ifmib        ifmibNetwork;
struct iftable      *piftableInterface=0;
ULONG               ulInterface;

                                        /* Check parameters */
if((pulConnectionLAN==0) || (pulConnectionPPP==0))
    return(ERROR_INVALID_PARAMETER);
                                        /* Create a socket (communication endpoint) */
if(pCruntime->iSocketLANPPP==0)
    {
    if((pCruntime->iSocketLANPPP=socket(PF_INET, SOCK_STREAM, 0))==-1)
        {
        lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
        printf("PC2Net: socket() %ld\n", lReturnCode);
        psock_errno("");
#endif  /* DEBUG_PC2NET */
        return(lReturnCode);
        }
#ifdef  DEBUG
    printf("PC2Net: InterfaceStatus() socket %d created\n", pCruntime->iSocketLANPPP);
#endif  /* DEBUG */
    }
                                        /* Initialize */
*pulConnectionLAN=*pulConnectionPPP=0;
memset(&ifmibNetwork, 0, sizeof(ifmibNetwork));
                                        /* Query interface information, which by the way is what
                                           NETSTAT -n uses too, to display the interface status */
if(ioctl(pCruntime->iSocketLANPPP, SIOSTATIF42, (caddr_t)&ifmibNetwork, sizeof(ifmibNetwork))==-1)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: ioctl(SIOSTATIF42) error %d.\n", lReturnCode);
#endif  /* DEBUG_PC2NET */
    return(lReturnCode);
    }
                                        /* Now we have obtained the status of all possible
                                           interfaces, though not all possible ones will be
                                           available (it's more that only a few interfaces will
                                           be available */
for(ulInterface=0; ulInterface<IFMIB_ENTRIES; ulInterface++)
    {
    piftableInterface=&ifmibNetwork.iftable[ulInterface];
                                        /* If we find an available interface, then we assume it is
                                           a PPP interface (PPP or SLIP) if it is marked Point-to-Point
                                           or slower than 2Mbit/s (the "slowest" LAN I know, which was
                                           the no longer used IBM PC LAN) otherwise it is counted as
                                           a LAN interface (e.g. Ethernet has 10Mbit/s, 100Mbit/s or even
                                           1 Gbit/s, the superior Token-Ring has 4Mbit/s, 16Mbit/s or even
                                           100Mbit/s, or less widely used types like FDDI, ATM,...).
                                           TCP/IP V4.1 marks the loopback interface as Point-to-Point
                                           in contrast to earlier versions, however in contrast to other
                                           interfaces loopback is a 0Mbit/s. */
#ifndef TCPV40HDRS
    if(piftableInterface->iftType!=0)
#else
    if(piftableInterface->ifType!=0)
#endif  /* TCPV40HDRS */
        {
#ifndef TCPV40HDRS
        if((piftableInterface->iftType & IFF_POINTOPOINT) || (piftableInterface->iftSpeed<2000000))
            {
            if(piftableInterface->iftSpeed>0)
                (*pulConnectionPPP)++;
            }
        else
            {
            if(piftableInterface->iftSpeed>0)
                (*pulConnectionLAN)++;
            }
#else
        if((piftableInterface->ifType & IFF_POINTOPOINT) || (piftableInterface->ifSpeed<2000000))
            {
            if(piftableInterface->ifSpeed>0)
                (*pulConnectionPPP)++;
            }
        else
            {
            if(piftableInterface->ifSpeed>0)
                (*pulConnectionLAN)++;
            }
#endif  /* TCPV40HDRS */
        if((*pulConnectionPPP+*pulConnectionLAN)>=ifmibNetwork.ifNumber)
            break;
        }
    }
#ifdef  DEBUG_PC2NET
printf("PC2Net: %ld LAN, %ld PPP interfaces found\n", *pulConnectionLAN, *pulConnectionPPP);
#endif  /* DEBUG_PC2NET */
                                        /* Keep that socket open to avoid running out of sockets,
                                           as soclose() does close a socket, but not return it to
                                           the pool of sockets unless the application terminated */
return(lReturnCode);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure opens a connection to the serving host (or proxy server if required)  *
 * so we can send and receive data then.                                                *
 * Req:                                                                                 *
 *      purlProxy ..... URL of Proxy to pass the request over, or NULL                  *
 *      purlServer .... URL of Server to load document from                             *
 * Mod:                                                                                 *
 *      purlProxy                                                                       *
 *      purlServer                                                                      *
 * Ret:                                                                                 *
 *      lReturnCode ... NO_ERROR if success, error otherwise                            *
\*--------------------------------------------------------------------------------------*/
LONG    ConnectionOpen(URL *purlProxy, URL *purlServer)
{
LONG                lReturnCode=NO_ERROR;
URL                 *purlRemote=0;
int                 iFlagNoBlocking;
int                 iFlagSocketReuse;
struct sockaddr_in  sockaddrServer;
struct sockaddr_in  sockaddrLocal;
struct hostent      *phostentServer;
struct hostent      *phostentLocal;
char                *pcsockaddrLocal;
int                 isockaddrLen=sizeof(sockaddrLocal);

                                        /* Check parameters */
if(purlServer==0)
    return(ERROR_INVALID_PARAMETER);
if(purlProxy!=0)
    purlRemote=purlProxy;
else
    purlRemote=purlServer;
                                        /* Create a socket (communication endpoint) */
if((pCruntime->iSocketHTTP=socket(PF_INET, SOCK_STREAM, 0))==-1)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: socket() %ld\n", lReturnCode);
    psock_errno("");
#endif  /* DEBUG_PC2NET */
    return(lReturnCode);
    }
#ifdef  DEBUG
printf("PC2Net: ConnectionOpen() socket %d created\n", pCruntime->iSocketHTTP);
#endif  /* DEBUG */
purlRemote->iSocket=pCruntime->iSocketHTTP;
                                        /* Initialize the socket for a remote connection. The
                                           remote host may be specified either as a symbolic
                                           name (which requires us to do a DNS lookup by using
                                           gethostbyname() or in dotted decimal form (what we
                                           can use directly). If a proxy is specified we connect
                                           through the proxy, which means that our remote communication
                                           endpoint is the proxy, and the proxy itself forwards our
                                           requests and its responses to and from the remote server
                                           we want to connect to */
memset(&sockaddrServer, 0, sizeof(sockaddrServer));
#ifndef TCPV40HDRS
                                        /* There seems to be a bug in the DevCon V2R2 header
                                           NETINET\IN.H as sockaddr_in is defined as:
                                               struct sockaddr_in {
                                                       u_char  sin_len;
                                                       u_char  sin_family;
                                                       u_short sin_port;
                                                       struct  in_addr sin_addr;
                                                       char    sin_zero[8];
                                               };
                                           instead as:
                                               struct sockaddr_in {
                                                       u_short sin_family;
                                                       u_short sin_port;
                                                       struct  in_addr sin_addr;
                                                       char    sin_zero[8];
                                               };
                                           I can't believe that this is not a bug, because it will
                                           break all applications (because on Intel due to the little
                                           endian format the sin_family 0xabcd in storage becomes
                                           0xcdab for u_short but only 0x00cd for u_char, and this
                                           really *is* different */

(*(short *)&sockaddrServer)=(short)AF_INET;
#else
sockaddrServer.sin_family=AF_INET;
#endif  /* TCPV40HDRS */
sockaddrServer.sin_addr.s_addr=inet_addr(purlRemote->ucHost);
if(sockaddrServer.sin_addr.s_addr==-1)
    {
    phostentServer=gethostbyname(purlRemote->ucHost);
    if(phostentServer==0)
        {
#ifdef  DEBUG_PC2NET
        printf("PC2Net: Remote %s can't be resolved\n", purlRemote->ucHost);
#endif  /* DEBUG_PC2NET */
        return(SOCEINVAL);
        }
    else
        memcpy(&sockaddrServer.sin_addr, phostentServer->h_addr, sizeof(sockaddrServer.sin_addr));
    }
sockaddrServer.sin_port=htons(purlRemote->ulPort);
                                        /* Now connect to the remote host through the socket we've
                                           opened so that we can send and receive data to and from
                                           that remote host using the socket */
if(connect(purlRemote->iSocket, (struct sockaddr *)&sockaddrServer, sizeof(sockaddrServer))==-1)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: connect() %ld\n", lReturnCode);
    psock_errno("");
#endif  /* DEBUG_PC2NET */
    return(lReturnCode);
    }
                                        /* Try to get the local host data. First try to get
                                           the local socket address, then try to convert that
                                           into the dotted-decimal IP-address in ASCII and
                                           finally do a reverse-DNS to get the hostname */
if(getsockname(purlRemote->iSocket, (struct sockaddr *)&sockaddrLocal, &isockaddrLen)==-1)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: getsockname() %ld\n", lReturnCode);
    psock_errno("");
#endif  /* DEBUG_PC2NET */
    }
else
    {
                                        /* Try to get the dotted-decimal IP-address in ASCII */
    memset(pHP->ucLocalAddress, '\0', sizeof(pHP->ucLocalAddress));
    pcsockaddrLocal=inet_ntoa(sockaddrLocal.sin_addr);
    if(pcsockaddrLocal==(char *)-1)
        {
#ifdef  DEBUG_PC2NET
        printf("PC2Net: inet_ntoa() %ld\n", lReturnCode);
        psock_errno("");
#endif  /* DEBUG_PC2NET */
        }
    else
        strncpy(pHP->ucLocalAddress, pcsockaddrLocal, sizeof(pHP->ucLocalAddress)-1);
                                        /* Try to get the local hostname in ASCII, preferably
                                           including the domain name */
    memset(pHP->ucLocalHostname, '\0', sizeof(pHP->ucLocalAddress));
    phostentLocal=gethostbyaddr((char *)&sockaddrLocal.sin_addr.s_addr, sizeof(sockaddrLocal.sin_addr.s_addr), AF_INET);
    if(phostentLocal==NULL)
        {
        lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
        printf("PC2Net: gethostbyaddr() %ld\n", lReturnCode);
        psock_errno("");
#endif  /* DEBUG_PC2NET */
        }
    else
        {
        char    **ppcLocalAlias;

                                        /* If the official name of the host does not contain
                                           points, then try if list of aliases are better */
        if(phostentLocal->h_name!=NULL)
            strncpy(pHP->ucLocalHostname, phostentLocal->h_name, sizeof(pHP->ucLocalHostname)-1);
        if(!strchr(pHP->ucLocalHostname, '.'))
            {
            ppcLocalAlias=phostentLocal->h_aliases;
            while(*ppcLocalAlias!=NULL)
                {
                if(strchr(*ppcLocalAlias, '.'))
                    {
                    strncpy(pHP->ucLocalHostname, *ppcLocalAlias, sizeof(pHP->ucLocalHostname)-1);
                    break;
                    }
                ppcLocalAlias++;
                }
            }
        }
    }
                                        /* Set socket to nonblocking mode */
iFlagNoBlocking=TRUE;
if(ioctl(purlRemote->iSocket, FIONBIO, (char *)&iFlagNoBlocking, sizeof(iFlagNoBlocking))==-1)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: ioctl(FIONBIO) error %ld\n", lReturnCode);
    psock_errno("");
#endif  /* DEBUG_PC2NET */
    return(lReturnCode);
    }
                                        /* Set socket to be reuseable (I'm not sure if this
                                           has any effect on a client application (that is
                                           an application which opens a connection, in contrast
                                           to one waiting for connections)) */
iFlagSocketReuse=TRUE;
if(setsockopt(purlRemote->iSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&iFlagSocketReuse, sizeof(iFlagSocketReuse))==-1)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: setsockopt(SO_REUSEADDR) error %ld\n", lReturnCode);
    psock_errno("");
#endif  /* DEBUG_PC2NET */
    return(lReturnCode);
    }
return(lReturnCode);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure closes the connection to the serving host.                            *
 * Req:                                                                                 *
 *      purlProxy ..... URL of Proxy to pass the request over, or NULL                  *
 *      purlServer .... URL of Server to load document from                             *
 * Mod:                                                                                 *
 *      purlProxy                                                                       *
 *      purlServer                                                                      *
 * Ret:                                                                                 *
 *      lReturnCode ... NO_ERROR if success, error otherwise                            *
\*--------------------------------------------------------------------------------------*/
LONG    ConnectionClose(URL *purlProxy, URL *purlServer)
{
LONG                lReturnCode=NO_ERROR;
URL                 *purlRemote=0;

                                        /* Check parameters */
if(purlServer==0)
    return(ERROR_INVALID_PARAMETER);
if(purlProxy!=0)
    purlRemote=purlProxy;
else
    purlRemote=purlServer;
                                        /* Shutdown the socket connection (in both directions)
                                           to the remote host). This may fail if we haven't been
                                           able to connect successfully, in which case we ignore
                                           the error and continue with shutdown() */
if(shutdown(purlRemote->iSocket, 2)<0)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: shutdown() error %ld\n", lReturnCode);
    psock_errno("");
#endif  /* DEBUG_PC2NET */
    if(lReturnCode==SOCEINVAL)
        lReturnCode=NO_ERROR;
    }
                                        /* Close the socket (the local communication endpoint) */
if(soclose(purlRemote->iSocket)==-1)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: soclose() error %d.\n", lReturnCode);
#endif  /* DEBUG_PC2NET */
    return(lReturnCode);
    }
return(lReturnCode);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure sends an HTTP request to a Server (if supplied by directing the       *
 * request to the optional proxy).                                                      *
 * Req:                                                                                 *
 *      purlProxy ..... URL of Proxy to pass the request over, or NULL                  *
 *      purlServer .... URL of Server to load document from                             *
 *      pucDocument ... Path of document to load from Server                            *
 *      bAuthenticate . FALSE/TRUE if Authentication (401 or 407) is required           *
 * Ret:                                                                                 *
 *      lReturnCode ... NO_ERROR if success, error otherwise                            *
\*--------------------------------------------------------------------------------------*/
LONG    RequestSend(URL *purlProxy, URL *purlServer, UCHAR *pucDocument, BOOL bAuthenticate)
{
LONG    lReturnCode=NO_ERROR;
UCHAR   *pucDocumentUrl=0;
UCHAR   ucAuthenticate[3*MAXNAMEL];
UCHAR   *pucAuthenticate=0;
int     iSocket=0;

                                        /* Check parameters */
if(purlServer==0)
    return(ERROR_INVALID_PARAMETER);
                                        /* If authentication is requested, create string for
                                           WWW-Authentication or Proxy-Authentication */
memset(ucAuthenticate, '\0', sizeof(ucAuthenticate));
if(bAuthenticate==TRUE)
    pucAuthenticate=EncodeBase64(pHP->ucUserHttpUserid, pHP->ucUserHttpPassword);
                                        /* Allocate space to create the full document Url */
pucDocumentUrl=(UCHAR *)MALLOC(REQUESTBUFFERSIZE);
if(pucDocumentUrl==0)
    return(ERROR_NOT_ENOUGH_MEMORY);
memset(pucDocumentUrl, '\0', REQUESTBUFFERSIZE);
if(purlProxy!=0)
    {
    if(bAuthenticate==TRUE)
        {
        strcpy(ucAuthenticate, "Proxy-Authorization: Basic ");
        strcat(ucAuthenticate, pucAuthenticate);
        strcat(ucAuthenticate, "\r\n");
        }
    sprintf(pucDocumentUrl, "GET http://%s:%ld/%s%s HTTP/1.0\r\nConnection: Close\r\n"\
        "User-Agent: PC/2 2.20 (OS/2; I)\r\nPragma: no-cache\r\nHost: %s\r\n"\
        "Accept: text/html, text/plain, application/x-zip-compressed\r\n"\
        "%s\r\n",
        purlServer->ucHost, purlServer->ulPort, purlServer->ucPath,
        (pucDocument!=0 ? pucDocument : ""), 
        purlServer->ucHost,
        ucAuthenticate);
    }
else
    {
    if(bAuthenticate==TRUE)
        {
        strcpy(ucAuthenticate, "Authorization: Basic ");
        strcat(ucAuthenticate, pucAuthenticate);
        strcat(ucAuthenticate, "\r\n");
        }
    sprintf(pucDocumentUrl, "GET /%s%s HTTP/1.0\r\nConnection: Close\r\n"\
        "User-Agent: PC/2 2.20 (OS/2; I)\r\nPragma: no-cache\r\nHost: %s\r\n"\
        "Accept: text/html, text/plain, application/x-zip-compressed\r\n"\
        "%s\r\n",
        purlServer->ucPath,
        (pucDocument!=0 ? (pucDocument[0]=='/' ? pucDocument+1 : pucDocument) : ""), 
        purlServer->ucHost,
        ucAuthenticate);
    }
#ifdef  DEBUG_PC2NET
printf("PC2Net: Sending GET request:\n%s", pucDocumentUrl);
#endif  /* DEBUG_PC2NET */
                                        /* Now send the request document to the proxy server
                                           if supplied or directly to the server if none was
                                           supplied */
if(purlProxy!=0)
    iSocket=purlProxy->iSocket;
else
    iSocket=purlServer->iSocket;
if(send(iSocket, pucDocumentUrl, strlen(pucDocumentUrl), 0)<0)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: send() error %ld\n", lReturnCode);
    psock_errno("");
#endif  /* DEBUG_PC2NET */
    }
                                        /* Return */
if(pucAuthenticate!=0)
    FREE(pucAuthenticate);
FREE(pucDocumentUrl);
return(lReturnCode);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure receives an HTTP response from a Server. As the socket is in non-     *
 * blocking mode, we have to check for a blocking socket during recv(). Between         *
 * individual receive requests we wait for an increasing period of time for the recv()  *
 * to finally succeed. If after some retries recv() still does not work, an eror is     *
 * returned. The non-blocking socket allows some finetuning which may help to work with *
 * low-speed (usually Point-to-Point) connections.                                      *
 * Req:                                                                                 *
 *      purlProxy ..... URL of Proxy to pass the request over, or NULL                  *
 *      purlServer .... URL of Server to load document from                             *
 *      pucDocument ... Path of document to load from Server                            *
 *      pucParameters . Parameters to be passed with HTTP Post request                  *
 *      bAuthenticate . FALSE/TRUE if Authentication (401 or 407) is required           *
 * Ret:                                                                                 *
 *      lReturnCode ... NO_ERROR if success, error otherwise                            *
\*--------------------------------------------------------------------------------------*/
LONG    RequestPost(URL *purlProxy, URL *purlServer, UCHAR *pucDocument, UCHAR *pucParameters, BOOL bAuthenticate)
{
LONG    lReturnCode=NO_ERROR;
UCHAR   *pucParametersCoded=0;
UCHAR   *pucDocumentUrl=0;
UCHAR   ucAuthenticate[3*MAXNAMEL];
UCHAR   *pucAuthenticate=0;
int     iSocket=0;

                                        /* Check parameters */
if((purlServer==0) || (pucDocument==0) || (pucParameters==0))
    return(ERROR_INVALID_PARAMETER);
                                        /* If authentication is requested, create string for
                                           WWW-Authentication or Proxy-Authentication */
memset(ucAuthenticate, '\0', sizeof(ucAuthenticate));
if(bAuthenticate==TRUE)
    pucAuthenticate=EncodeBase64(pHP->ucUserHttpUserid, pHP->ucUserHttpPassword);
                                        /* Code parameters string into escape sequence */
pucParametersCoded=ParseCode(pucParameters);
                                        /* Allocate space to create the full document Url */
pucDocumentUrl=(UCHAR *)MALLOC(REQUESTBUFFERSIZE);
if(pucDocumentUrl==0)
    return(ERROR_NOT_ENOUGH_MEMORY);
memset(pucDocumentUrl, '\0', REQUESTBUFFERSIZE);
if(purlProxy!=0)
    {
    if(bAuthenticate==TRUE)
        {
        strcpy(ucAuthenticate, "Proxy-Authorization: Basic ");
        strcat(ucAuthenticate, pucAuthenticate);
        strcat(ucAuthenticate, "\r\n");
        }
    sprintf(pucDocumentUrl, "POST http://%s:%ld/%s HTTP/1.0\r\nConnection: Close\r\n"\
        "User-Agent: PC/2 2.20 (OS/2; I)\r\nPragma: no-cache\r\nHost: %s\r\n"\
        "Accept: text/html, text/plain, Content-type: application/x-www-form-urlencoded\r\n"\
        "Content-length: %ld\r\n%s\r\n%s",
        purlServer->ucHost, 
        purlServer->ulPort, 
        pucDocument,
        purlServer->ucHost, strlen(pucParametersCoded), 
        ucAuthenticate,
        pucParametersCoded);
    }
else
    {
    if(bAuthenticate==TRUE)
        {
        strcpy(ucAuthenticate, "Authorization: Basic ");
        strcat(ucAuthenticate, pucAuthenticate);
        strcat(ucAuthenticate, "\r\n");
        }
    sprintf(pucDocumentUrl, "POST /%s HTTP/1.0\r\nConnection: Close\r\n"\
        "User-Agent: PC/2 2.20 (OS/2; I)\r\nPragma: no-cache\r\nHost: %s\r\n"\
        "Accept: text/html, text/plain, Content-type: application/x-www-form-urlencoded\r\n"\
        "Content-length: %ld\r\n%s\r\n%s",
        pucDocument,
        purlServer->ucHost, 
        strlen(pucParametersCoded), 
        ucAuthenticate,
        pucParametersCoded);
    }
#ifdef  DEBUG_PC2NET
printf("PC2Net: Sending POST request\n");
#endif  /* DEBUG_PC2NET */
                                        /* Now post the request document to the proxy server
                                           if supplied or directly to the server if none was
                                           supplied */
if(purlProxy!=0)
    iSocket=purlProxy->iSocket;
else
    iSocket=purlServer->iSocket;
if(send(iSocket, pucDocumentUrl, strlen(pucDocumentUrl), 0)<0)
    {
    lReturnCode=sock_errno();
#ifdef  DEBUG_PC2NET
    printf("PC2Net: send() error %ld\n", lReturnCode);
    psock_errno("");
#endif  /* DEBUG_PC2NET */
    }
                                        /* Return */
FREE(pucParametersCoded);
FREE(pucDocumentUrl);
if(pucAuthenticate!=0)
    FREE(pucAuthenticate);
return(lReturnCode);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure receives an HTTP response and returns that response which may have    *
 * been split into multiple parts during transmission.                                  *
 * Req:                                                                                 *
 *      purlProxy ..... URL of Proxy to pass the request over, or NULL                  *
 *      purlServer .... URL of Server to load document from                             *
 *      ppucDocument .. Pointer into which the loaded document or NULL gets returned    *
 *      pulDocumentLength                                                               *
 *                      Pointer to the length of the document returned or 0             *
 * Ret:                                                                                 *
 *      lReturnCode ... NO_ERROR if success, error otherwise                            *
\*--------------------------------------------------------------------------------------*/
LONG    RequestReceive(URL *purlProxy, URL *purlServer, UCHAR **ppucDocument, ULONG *pulDocumentLength)
{
LONG    lReturnCode=NO_ERROR;
LONG    lHTTPStatusCode=NO_ERROR;
LONG    lRetryCount=0;
URL     *purlRemote=0;
UCHAR   ucStatus[CCHMAXPATH];
UCHAR   ucResponse[4001];
UCHAR   ucResponseUppercase[4001];
LONG    lResponseLength=0;
LONG    lContentLength=0;
UCHAR   *pucDocument=0;
LONG    lDocumentLength=0;
LONG    lBytesReceived=0;
UCHAR   *pucBegin=0;
UCHAR   *pucEnd=0;
UCHAR   *pucTemp=0;

                                        /* Check parameters */
if((purlServer==0) || (ppucDocument==0) || (pulDocumentLength==0))
    return(ERROR_INVALID_PARAMETER);
*ppucDocument=0;
*pulDocumentLength=0;
if(purlProxy!=0)
    purlRemote=purlProxy;
else
    purlRemote=purlServer;
                                        /* Receive the document. If the document returned is
                                           a multipart document, then this is the first part
                                           which will give us the details required how much
                                           data we are about to receive in other parts (by
                                           looking into the Content-Length response header) */
memset(ucResponse, '\0', sizeof(ucResponse));
for(lRetryCount=1; lRetryCount<=10; DosSleep(1000*lRetryCount), lRetryCount++)
    {
    lResponseLength=recv(purlRemote->iSocket, ucResponse, sizeof(ucResponse)-1, 0);
    if(lResponseLength<0)
        {
        lReturnCode=sock_errno();
        if((lReturnCode==SOCEWOULDBLOCK) && (lRetryCount<=10))
            continue;
#ifdef  DEBUG_PC2NET
        printf("PC2Net: recv() error %ld\n", lReturnCode);
        psock_errno("");
#endif  /* DEBUG_PC2NET */
        return(lReturnCode);
        }
    else
        {
        lReturnCode=NO_ERROR;
        break;
        }
    }
strcpy(ucResponseUppercase, ucResponse);
STRUPR(ucResponseUppercase);
                                        /* Try to locate the HTTP Status Code which is at the
                                           beginning of the Response as: HTTP/1.0 200 OK for
                                           correctly processed requests */
pucBegin=strchr(ucResponseUppercase, ' ');
if(pucBegin!=0)
    lHTTPStatusCode=atoi(pucBegin);
#ifdef  DEBUG_PC2NET
printf("PC2Net: HTTP Status Code is \"%d\"\n", lHTTPStatusCode);
#endif  /* DEBUG_PC2NET */
                                        /* Try to locate the Content-Length response header, as
                                           we don't know the exact spelling, we have to convert
                                           response header to uppercase */
pucBegin=strstr(ucResponseUppercase, "CONTENT-LENGTH: ");
if(pucBegin!=0)
    {
    pucEnd=pucBegin+=(sizeof("CONTENT-LENGTH: ")-1);
    while(isdigit(*pucEnd))
        pucEnd++;
    lContentLength=atoi(pucBegin);
    }
                                        /* Find the end of the response header, which is where
                                           the real document begins */
pucBegin=strstr(ucResponse, "\r\n\r\n");
if((pucBegin!=0) && (lContentLength!=0))
    {
    pucBegin+=(sizeof("\r\n\r\n")-1);
    lDocumentLength=lContentLength+1;
    pucDocument=(UCHAR *)MALLOC(lDocumentLength);
    if(pucDocument==0)
        return(ERROR_NOT_ENOUGH_MEMORY);
    memset(pucDocument, '\0', lContentLength+1);
    pucEnd=strchr(pucDocument, '\0');
    lBytesReceived=lResponseLength-((LONG)pucBegin-(LONG)ucResponse);
    memcpy(pucEnd, pucBegin, lBytesReceived);
    pucEnd+=lBytesReceived;
                                        /* For multipart archives, just load one part after another,
                                           as http uses the TCP protocol we won't get parts out of
                                           order. We have already allocates the whole size for the
                                           document response so we simply have to append one part
                                           after the other until we received the complete size of the
                                           response (and we know the size from the Content-Length
                                           response header we got in the first part) */
    while(lBytesReceived<(lDocumentLength-1))
        {
        for(lRetryCount=1; lRetryCount<=10; DosSleep(1000*lRetryCount), lRetryCount++)
            {
            lResponseLength=recv(purlRemote->iSocket, ucResponse, sizeof(ucResponse)-1, 0);
            if(lResponseLength<0)
                {
                lReturnCode=sock_errno();
                if((lReturnCode==SOCEWOULDBLOCK) && (lRetryCount<=10))
                    continue;
#ifdef  DEBUG_PC2NET
                printf("PC2Net: recv() error %ld\n", lReturnCode);
                psock_errno("");
#endif  /* DEBUG_PC2NET */
                return(lReturnCode);
                }
            else
                {
                lReturnCode=NO_ERROR;
                break;
                }
            }
        if(lResponseLength==0)
            break;
        pucBegin=ucResponse;
        memcpy(pucEnd, pucBegin, lResponseLength);
        lBytesReceived+=lResponseLength;
        pucEnd+=lResponseLength;
                                        /* Inform user about the download */
        pucTemp=strrchr(purlServer->ucPath, '/');
        if((pucTemp!=0) && (lContentLength!=0))
            {
            sprintf(ucStatus, "Downloading %s (%ld of %ld bytes)",
                pucTemp+1, lBytesReceived, lContentLength);
            WinSendMsg(pHP->hwndNetThread, WM_NETSTATUS, MPFROMP(STRDUP(ucStatus)), NULL);
            }
        }
    }
                                        /* Now return what we downloaded */
if(pucDocument!=0)
    {
    *ppucDocument=pucDocument;
    *pulDocumentLength=(ULONG)lDocumentLength;
    }
#ifdef  DEBUG_PC2NET
if(pucDocument!=0)
    printf("PC2Net: Response parsed with Content-Length\n");
else
    printf("PC2Net: Response did not contain Content-Length\n");
#endif  /* DEBUG_PC2NET */
if((lReturnCode!=NO_ERROR) && (pucDocument))
    {
    FREE(pucDocument);
    pucDocument=0;
    }
                                        /* Parse document returned for HTTP status code,
                                           and return any error (that is anything else
                                           than 200 OK) */
if(lReturnCode==NO_ERROR)
    {
    lReturnCode=lHTTPStatusCode;
    if(lReturnCode==200)
        lReturnCode=NO_ERROR;
    }
return(lReturnCode);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure encodes an string into HTTP conforming encoding, that is it replaces  *
 * characters by their escape sequence and returns a new string.                        *
 * Req:                                                                                 *
 *      pucString       Pointer to the length of the document returned or 0             *
 * Ret:                                                                                 *
 *      pucStringCoded  String with characters coded in escape sequence, or NULL        *
\*--------------------------------------------------------------------------------------*/
UCHAR   *ParseCode(UCHAR *pucString)
{
UCHAR   *pucStringCoded=0;
UCHAR   *pucBegin;
UCHAR   ucCharacter;
UCHAR   ucTranslate[]={ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
ULONG   ulLength;

                                        /* Check parameters */
if(pucString==0)
    return(NULL);
                                        /* Allocate result, which may be the threefold of
                                           the original string if all characters need to
                                           be escaped (unlikely, but one never knows) */
ulLength=strlen(pucString)*3+1;
pucStringCoded=MALLOC(ulLength);
memset(pucStringCoded, '\0', ulLength);
for(pucBegin=pucStringCoded; *pucString!='\0'; pucString++)
    {
    ucCharacter=*pucString;
    if(((ucCharacter>='0') && (ucCharacter<='9')) ||
        ((ucCharacter>='A') && (ucCharacter<='Z')) ||
        ((ucCharacter>='a') && (ucCharacter<='z')) ||
        (ucCharacter=='.') ||
        (ucCharacter=='&') ||
        (ucCharacter=='+') ||
        (ucCharacter=='='))
        *pucBegin++=ucCharacter;
    else
        {
        *pucBegin++='%';
        *pucBegin++=ucTranslate[(((ULONG)ucCharacter>>4) & 0x0000000F)];
        *pucBegin++=ucTranslate[(((ULONG)ucCharacter) & 0x0000000F)];
        }
    }
return(pucStringCoded);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure encodes a Userid and encrypted Password into the Base64 format used   *
 * for HTTP authentication.                                                             *
 * Req:                                                                                 *
 *      pucUserid       Pointer to a Userid string                                      *
 *      pucPassword     Pointer to an encrypted Password string                         *
 * Ret:                                                                                 *
 *      pucBase64       Userid:Password encoded into Base64 format                      *
\*--------------------------------------------------------------------------------------*/
UCHAR   *EncodeBase64(UCHAR *pucUserid, UCHAR *pucPassword)
{
LONG    lIndex;
LONG    lLengthNoBase64=0;
LONG    lLengthBase64=0;
UCHAR   *pucPasswordDecrypted=0;
UCHAR   *pucNoBase64=0;
UCHAR   *pucBase64=0;
ULONG   ulNoBase64=0;
ULONG   ulTranslate=0;
UCHAR   ucTranslate[]={ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
                        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
                        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                        '8', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

if((pucUserid==NULL) && (pucPassword==NULL))
    return("");
                                        /* Decrypt Password */
if(pucPassword!=NULL)
    {
    pucPasswordDecrypted=STRDUP(pucPassword);
    for(lIndex=strlen(pucPasswordDecrypted)-1; lIndex>=0; lIndex--)
        pucPasswordDecrypted[lIndex]=(pucPasswordDecrypted[lIndex]^0x3C)>>1;
                                        /* Count length of the Userid:Password string
                                           both ASCII and Base64 encoded */
    lLengthNoBase64=strlen(pucPasswordDecrypted);
    }
if(pucUserid!=NULL);
    {
                                        /* Count length of the Userid:Password string
                                           both ASCII and Base64 encoded */
    lLengthNoBase64+=strlen(pucUserid);
    lLengthNoBase64++;
    }
#ifdef  DEBUG
if((pucUserid!=NULL) && (pucPassword!=NULL))
    printf("PC2Net: Authenticate with \"%s\":\"%s\"\n", pucUserid, pucPasswordDecrypted);
#endif  /* DEBUG */
                                        /* Round up to the next multiple of 3 Byte
                                           words, that is 24 bits (plus \0) */
lIndex=((lLengthNoBase64+2)/3)*3+1;
                                        /* Allocate storage to format non Base64
                                           encoded Userid:Password string */
pucNoBase64=MALLOC(lIndex);
memset(pucNoBase64, '\0', lIndex);
if(pucUserid!=NULL)
    {
    strcat(pucNoBase64, pucUserid);
    strcat(pucNoBase64, ":");
    }
if(pucPasswordDecrypted!=NULL)
    {
    strcat(pucNoBase64, pucPasswordDecrypted);
    FREE(pucPasswordDecrypted);
    pucPasswordDecrypted=0;
    }
                                        /* Allocate storage for the Base64 encoded
                                           Userid:Password string, which uses 4 Byte
                                           words, that is 24 bits with 6-bit Bytesize 
                                           (plus = termination, plus \0) */
lLengthBase64=(lIndex*4)/3+2;
pucBase64=MALLOC(lLengthBase64);
memset(pucBase64, '\0', lLengthBase64);
                                        /* Convert ASCII to Base64 */
for(lIndex=0, lLengthBase64=0; lIndex<lLengthNoBase64; lIndex+=3)
    {
    LONG    lShift;

                                        /* Load a 24 bit (3 8-bit Bytes) word */
    ulNoBase64=0;
    for(lShift=0; lShift<3; lShift++)
        {
        ulNoBase64<<=8;
        ulNoBase64|=((ULONG)pucNoBase64[lIndex+lShift]) & 0x000000FF;
        }
                                        /* Fiter out the 4 6-bit Bytes of the 24 bit word
                                           to translate each 6-bit Byte into Base64 code */
    for(lShift=0; (lShift<=3 && (lIndex+lShift)<=lLengthNoBase64); lShift++)
        {
        ulTranslate=(ulNoBase64>>(18-(6*lShift))) & 0x0000003F;
        pucBase64[lLengthBase64++]=ucTranslate[ulTranslate];
        }
    }
                                        /* Fill rest to next 24 bit word with padding
                                           character (in other words, as we only work on
                                           multiples of 24 bit words, and a Byte has 6 bit
                                           in Base64 code, the Base64 encoded string is a
                                           multiple of 4 Base64-code characters. If the
                                           unencoded string has ended, we don't encode "0"
                                           Bytes but pad with "=" */
while((lLengthBase64 % 4)!=0)
    pucBase64[lLengthBase64++]='=';
if(pucBase64[lLengthBase64-1]!='=')
    pucBase64[lLengthBase64]='=';
FREE(pucNoBase64);
return(pucBase64);
}

/*--------------------------------------------------------------------------------------*\
 * This is the window procedure for the Automatic Update status window, which is shown  *
 * when PC/2 tries to connect to the Internet to display the status of the connection.  *
 * As this is a PM window, it is run by the main thread (otherwise, the Network object  *
 * window thread would become a PM thread possibly blocking the SIQ).                   *
\*--------------------------------------------------------------------------------------*/
MRESULT EXPENTRY StatusWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Handle of static text control covering the
                                           client */
static HWND     hwndStatusText=NULLHANDLE;

switch(msg)
{
case WM_CREATE:
    {
    LONG    lForeGround=RGB_RED;
    LONG    lBackGround=WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);

                                        /* Create static text control covering the client
                                           and used to display the status */
    hwndStatusText=WinCreateWindow(hwnd, WC_STATIC, "",
        WS_VISIBLE|WS_SAVEBITS|SS_TEXT|DT_LEFT|DT_WORDBREAK,
        0, 0, 0, 0,
        hwnd, HWND_TOP, ID_PC2STATUSTEXT, NULL, NULL);
                                        /* Set font */
    WinSetPresParam(hwndStatusText, PP_FONTNAMESIZE,
        sizeof(pHP->ucPC2WindowFont), pHP->ucPC2WindowFont);
                                        /* Set colors */
    WinSetPresParam(hwndStatusText, PP_FOREGROUNDCOLOR,
        sizeof(lForeGround), &lForeGround);
    WinSetPresParam(hwndStatusText, PP_BACKGROUNDCOLOR,
        sizeof(lBackGround), &lBackGround);
    }
    break;

case WM_PAINT:
    {
    HPS     hpsClient;

                                        /* Get a cached presentation space */
    hpsClient=WinBeginPaint(hwnd, NULLHANDLE, NULL);
    WinEndPaint(hpsClient);
    }
    break;

case WM_SIZE:
case WM_MOVE:
/*                                                                                      *\
 * During sizing (and moving) of the Status window, ensure that the static text always  *
 * fills the complete client area.                                                      *
\*                                                                                      */
    {
    RECTL   rectlClient;
    SWP     swpStatus;

                                        /* Save current size and position */
    WinQueryWindowPos(pHP->hwndStatusFrame, &pHP->swpPC2Status);
                                        /* Size the text control to client area */
    WinQueryWindowRect(hwnd, &rectlClient);
    swpStatus.fl=SWP_MOVE|SWP_SIZE|SWP_SHOW|SWP_ZORDER;
    swpStatus.x=swpStatus.y=0;
    swpStatus.cx=rectlClient.xRight;
    swpStatus.cy=rectlClient.yTop;
    swpStatus.hwndInsertBehind=HWND_TOP;
    swpStatus.hwnd=hwndStatusText;
    WinSetMultWindowPos(pHP->habPc2, &swpStatus, 1);
    }
    break;

case WM_SYSCOMMAND:
    if(SHORT1FROMMP(mp1)==SC_CLOSE)
        printf("Close selected\n");
    return((MRESULT)TRUE);

/*                                                                                      *\
 * Syntax: WM_NETSTATUS, UCHAR *pucStatusText, NULL                                     *
\*                                                                                      */
case WM_NETSTATUS:
/*                                                                                      *\
 * This message is posted from within PC2NET.DLL functions to be able to inform the     *
 * user about the current status of the Automatic Update. The status text passed is a   *
 * copy and has to be freed afterwards therefore.                                       *
\*                                                                                      */
    {
    UCHAR   *pucStatusText=PVOIDFROMMP(mp1);
    SWP     swpStatus;

    memset(&swpStatus, 0, sizeof(SWP));
    swpStatus.hwndInsertBehind=HWND_TOP;
    swpStatus.hwnd=pHP->hwndStatusFrame;

    if((pucStatusText!=0) && (pucStatusText[0]!='\0'))
        {
        swpStatus.fl=SWP_ZORDER|SWP_SHOW;
        WinSetWindowText(hwndStatusText, pucStatusText);
        FREE(pucStatusText);
        }
    else
        {
        swpStatus.fl=SWP_HIDE;
        }
    if(pHP->ulRegistrationFlag & REGISTER_SHOWSTATUS)
        WinSetMultWindowPos(pHP->habPc2, &swpStatus, 1);
    }
    return((MRESULT)NO_ERROR);

default:                                /* Default window procedure must be called */
    return((MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * For debug purposes PC2Net.c will be linked with printf.obj from the IBM EWS PMPRINTF *
 * package. However, this object requires the linker to resolv the time() symbol, and   *
 * as this DLL is linked with the subsystem library only we have to provide a dummy     *
 * one (liked as a subsystem DLL, because we don't require the C-runtime but use the    *
 * one already linked to PC2.EXE).                                                      *
\*--------------------------------------------------------------------------------------*/
long    time(long *ptimer)
{
return(*ptimer);
}

/*--------------------------------------------------------------------------------------*\
 * Required by C-Runtime functions.                                                     *
\*--------------------------------------------------------------------------------------*/
const unsigned short    *_ctype=0;

