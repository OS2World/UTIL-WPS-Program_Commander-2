/***********************************************************************\
 *                                PC2.c                                *
 *            Copyright (C) by Stangl Roman, 1996, 2000                *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * Switch.c     QuickSwitch and SessionBar windows.                    *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#include        "PC2.h"                 /* User include files */
#include        "Error.h"

#define         _FILE_  "Switch.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;


/*--------------------------------------------------------------------------------------*\
 * This procedure is the PC/2 SessionBar window procedure.                              *
\*--------------------------------------------------------------------------------------*/
                                        /* WM_DRAWSESSIONTEXT parameters for mp2 */
                                        /* Repaint current contents of SessionBar text rectangle (due to
                                           a invalidated rectangle) */
#define SESSIONBAR_REPAINTCURRENT           0
                                        /* The timer regularily tried to update the time of the status
                                           line, which is honored unless the mouse pointer is over a
                                           session's icon, in which case we draw the session's Window List
                                           entry instead */
#define SESSIONBAR_DRAWSTATUS               1
                                        /* Remove a session's Window List entry, as the mouse pointer is
                                           not over the session's icon */
#define SESSIONBAR_NOSESSIONHIT             2
                                        /* Anything other is the text to draw */

MRESULT EXPENTRY PC2_SessionWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Replacement pointer for not accessible pointers */
static HPOINTER     hpointerReplacement=NULLHANDLE;
                                        /* Pointer to the first element of a linked list of SWITCHLIST
                                           structures */
static SWITCHLIST   *pswitchlistAnchor=NULL;
                                        /* PC/2's Launch Button handle */
static HBITMAP      hbitmapLaunchButton=NULLHANDLE;
                                        /* Scroll button handles */
static HBITMAP      hbitmapScrollLeft=NULLHANDLE;
static HBITMAP      hbitmapScrollRight=NULLHANDLE;
                                        /* Uncorrected (for small icons) definitions */
static ULONG        ulIconSize=ICONSIZE_VGA;
static ULONG        ulBorderSize=SESSIONBAR_BORDER;
static ULONG        ulSpaceSize=SESSIONBAR_SPACE;
static ULONG        ulFrameSize=SESSIONBAR_ICONFRAME;
                                        /* Number of sessions (icons) in Window List */
static ULONG        ulSessionIconsCount;
                                        /* Number of sessions (icons) that fit into the SessionBar */
static ULONG        ulSessionIconsDrawn;
                                        /* Index of first icon in SessionBar (points to first icon
                                           in SessionBar if ulSessionIconsCount>ulSessionIconsDrawn) */
static ULONG        ulSessionIconsFirst;
                                        /* Session name textfield width (resolution dependent) */
static ULONG        ulSessionNameWidth=SESSIONBAR_NAMESCALEX;
                                        /* Launch Button state (WM_BUTTON1UP|WM_BUTTON1DOWN) */
static ULONG        ulButton1State=WM_BUTTON1UP;
                                        /* Number of sessions in SessionBar (1-based) */
static ULONG        ulSessionNumber;
                                        /* Number of previous session active (0-based) */
static ULONG        ulSessionPrevious;
                                        /* Number of current session active (0-based) */
static ULONG        ulSessionCurrent;
                                        /* Redraw scroll button status */
static ULONG        ulRedrawScrollButtons=TRUE;
                                        /* Previous APM status */
static ULONG        ulAPMStatusPrevious;
                                        /* CPU Performance graphics per processor width */
static ULONG        ulCPUProcessorPixels;
                                        /* Number of pixels columns for a single CPU Performance value */
static ULONG        ulCPUTicksAdjustMult;
static ULONG        ulCPUTicksAdjustDiv;
                                        /* CPU Performance status are Initialization flag (required, because
                                           after the first WM_PAINT, the timer may not be running yet, so we
                                           miss some ticks (causing distorted graphics). We solve that by
                                           waiting that we get initialized by the first WM_TIMER message
                                           processed for this window. Same applies when changing the time
                                           basis (e.g. from 5M to 60S) */
static ULONG        ulCPUInitialized;
                                        /* CPU Performance number of ticks since initialization */
static  ULONG       ulTimerTicks;
                                        /* Move pointer position */
static POINTL       pointlMouse;
                                        /* Client window rectangle */
static RECTL        rectlClient;
                                        /* Rectangle of the area that causes the status area text
                                           to display data about (e.g. a session's icon, APM status
                                           area or CPU Performance status area) */
static RECTL        rectlHotspot;
                                        /* Current time (from asctime()) */
static UCHAR        ucCurrentTime[32]="";
                                        /* Current battery capacity */
static UCHAR        ucStatusText[MAXNAMEL+1]="";
                                        /* Launch Button rectangle (including 3D border) */
static RECTL        *prectlLaunchFrame;
                                        /* Launch Button and icons separator rectangle */
static RECTL        *prectlSeparator;
                                        /* Scrollbars to scroll Sessions systemmenus icons */
static RECTL        *prectlScrollLeft;
static RECTL        *prectlScrollRight;
                                        /* Sessions systemmenus icons (including backward and forward button) */
static RECTL        *prectlSessionIcons;
                                        /* APM status rectangle (including 3D border) */
static RECTL        *prectlAPMStatus;
                                        /* CPU Performance status rectangle (including 3D border) */
static RECTL        *prectlCPUStatus;
                                        /* Session name rectangle (including 3D border) */
static RECTL        *prectlSessionName;
                                        /* Selected session (by moving mouse pointer over it) */
static SWITCHLIST   *pswitchlistSelected;
                                        /* Current time (updated immediately during next timer tick) */
static struct tm    tmPrevious;
                                        /* Current time (to draw CPU Performance status for) */
static struct tm    tmCPUStatus;
                                        /* Array of CPU Performance value history for all processors */
static CPUPERFORMANCE   *pCPUPerformanceStack=NULL;
                                        /* Current CPU number we work with */
ULONG               ulNumProcessor;
                                        /* Current CPU status we work with */
ULONG               ulProcessorStatus;
                                        /* Work area */
ULONG               ulTemp;
                                        /* CPU Performance popup menu setup */
MENUITEM            menuitemCPU;
                                        /* SessionBar repositioning temp */
SWP                 swpSessionBar;

switch(msg)
{
#ifdef DEBUG_SB
case WM_CHAR:
    DosBeep(1000, 10);
    break;
#endif  /* DEBUG_SB */

case WM_CREATE:
                                        /* Cleanup */
    ulSessionIconsCount=0;
    ulSessionIconsDrawn=0;
    ulSessionIconsFirst=0;
    ulSessionNumber=0;
    ulSessionPrevious=(ULONG)-1;
    ulSessionCurrent=(ULONG)-1;
    ulCPUTicksAdjustMult=1;
    ulCPUTicksAdjustDiv=1;
    ulCPUInitialized=FALSE;
    ulTimerTicks=(ULONG)-1;
    prectlLaunchFrame=NULL;
    prectlSeparator=NULL;
    prectlScrollLeft=NULL;
    prectlScrollRight=NULL;
    prectlSessionIcons=NULL;
    prectlAPMStatus=NULL;
    prectlCPUStatus=NULL;
    prectlSessionName=NULL;
    pswitchlistSelected=NULL;
                                        /* Initialize the performance counters or
                                           Aurora will display nonsense values. Special
                                           thanks to Scott E. Garfinkle for providing me
                                           that stuff that was forgotten to be added to
                                           the toolkit documentation! */
    if(pHP->DosPerfSysCall!=NULL)
        pHP->DosPerfSysCall(CMD_KI_ENABLE, 0, 0, 0);
                                        /* Initialize datastructures */
    if(pHP->ulStatusFlag2 & KEEPONTOPSB)
        pHP->hwndZOrder[ZORDER_SESSIONBAR]=WinQueryWindow(hwnd, QW_PARENT);
    if(!hpointerReplacement)
        hpointerReplacement=WinLoadPointer(HWND_DESKTOP, pHP->hDLLPc2Resource, ID_ICONSESSIONUNKNOWN);
    if(!prectlLaunchFrame)
        prectlLaunchFrame=malloc(sizeof(RECTL));
    if(prectlSeparator==NULL)
        prectlSeparator=malloc(sizeof(RECTL));
    if(!prectlSessionIcons)
        {
        prectlScrollLeft=malloc(sizeof(RECTL));
        prectlScrollRight=malloc(sizeof(RECTL));
        prectlSessionIcons=malloc(sizeof(RECTL));
        }
    if(!prectlAPMStatus)
        prectlAPMStatus=malloc(sizeof(RECTL));
    if(!prectlCPUStatus)
        prectlCPUStatus=malloc(sizeof(RECTL));
    if(!prectlSessionName)
        prectlSessionName=malloc(sizeof(RECTL));
                                        /* Load the SessionBar's session icon's menu */
    pHP->hwndPopupMenuSessionBar=WinLoadMenu(HWND_DESKTOP, NULLHANDLE, ID_PC2SESSIONWINDOW);
    WinSetPresParam(pHP->hwndPopupMenuSessionBar, PP_FONTNAMESIZE,
        sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
                                        /* Load the SessionBar's processor performance menu,
                                           additionally query the Settings submenu handle to
                                           avoid scrolling away of the Dynamic SessionBar during
                                           selecting in the submenu */
    pHP->hwndPopupMenuProcessor=WinLoadMenu(HWND_DESKTOP, NULLHANDLE, ID_SBPERFORMANCEMENU);
    if(pHP->ulNumProcessors>1)
        for(ulNumProcessor=2; ulNumProcessor<=pHP->ulNumProcessors; ulNumProcessor++)
            {
            menuitemCPU.iPosition=MIT_END;
            menuitemCPU.afStyle=MIS_TEXT;
            menuitemCPU.afAttribute=MIA_CHECKED;
            menuitemCPU.id=ID_SBPROCESSORMENU+ulNumProcessor;
            menuitemCPU.hwndSubMenu=NULLHANDLE;
            menuitemCPU.hItem=NULLHANDLE;
            sprintf(ucStatusText, "CPU ~%ld online", ulNumProcessor);
            WinSendMsg(pHP->hwndPopupMenuProcessor, MM_INSERTITEM,
                &menuitemCPU, ucStatusText);
            }
    WinSendMsg(pHP->hwndPopupMenuProcessor, MM_QUERYITEM,
        MPFROM2SHORT(ID_SBSETTINGSMENU, TRUE), (MPARAM)&menuitemCPU);
    pHP->hwndPopupSubMenuProcessor=menuitemCPU.hwndSubMenu;
    WinSetPresParam(pHP->hwndPopupMenuProcessor, PP_FONTNAMESIZE,
        sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
                                        /* Load the SessionBar's Statusare menu */
    pHP->hwndPopupMenuStatus=WinLoadMenu(HWND_DESKTOP, NULLHANDLE, ID_SBSTATUSMENU);
    WinSendMsg(pHP->hwndPopupMenuStatus, MM_SETITEMATTR,
        MPFROM2SHORT((pHP->ulSessionBarStatusarea & SB_STATUSLEFTJUSTIFIED ? ID_SBLEFTJUSTIFIED : ID_SBRIGHTJUSTIFIED), TRUE),
        MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
                                        /* We're ready to receive messages from GetWindowList()
                                           which windows are in the system */
    pHP->ulRuntimeFlag|=INITSESSIONBAR;
                                        /* As we're now ready, request another enumeration of
                                           the windows on the Desktop */
    WinPostMsg(pHP->hwndClient, WM_SETUPSIZEPOSITION, MPFROMLONG(WM_MOVE), NULL);
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));

case WM_DESTROY:
    pHP->hwndZOrder[ZORDER_SESSIONBAR]=NULLHANDLE;
    WinDestroyWindow(pHP->hwndPopupMenuStatus);
    WinDestroyWindow(pHP->hwndPopupMenuProcessor);
    WinDestroyWindow(pHP->hwndPopupMenuSessionBar);
    pHP->hwndSessionFrame=pHP->hwndSessionClient=
        pHP->hwndPopupMenuStatus=pHP->hwndPopupMenuSessionBar=pHP->hwndPopupMenuProcessor=NULLHANDLE;
    for(ulTemp=ulSessionIconsCount; ulTemp!=0; ulTemp--)
        WinSendMsg(hwnd, WM_SESSIONBARUPDATE, MPFROMLONG(ulTemp), NULL);
                                        /* Free our resources */
    WinDestroyPointer(hpointerReplacement);
    hpointerReplacement=NULLHANDLE;
    GpiDeleteBitmap(hbitmapLaunchButton);
    GpiDeleteBitmap(hbitmapScrollLeft);
    GpiDeleteBitmap(hbitmapScrollRight);
    hbitmapLaunchButton=hbitmapScrollLeft=hbitmapScrollRight=NULLHANDLE;
                                        /* Free our datastructures */
    if(pswitchlistAnchor)
        free(pswitchlistAnchor);
    pswitchlistAnchor=NULL;
    WinSendMsg(hwnd, WM_CALCULATECPUSTATUS, MPFROMLONG(WM_DESTROY), NULL);
    free(prectlSessionName);
    free(prectlCPUStatus);
    free(prectlAPMStatus);
    free(prectlSessionIcons);
    free(prectlScrollRight);
    free(prectlScrollLeft);
    free(prectlSeparator);
    free(prectlLaunchFrame);
    prectlLaunchFrame=prectlSeparator=prectlScrollLeft=prectlScrollRight=
        prectlSessionIcons=prectlSessionName=NULL;
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));

case WM_PAINT:
    {
    HPS     hpsClient;
    RECTL   rectlTemp;

                                        /* Get a cached presentation space */
    hpsClient=WinBeginPaint(hwnd, NULLHANDLE, &rectlTemp);
                                            /* Change color table into RGB mode */
    GpiCreateLogColorTable(hpsClient, 0L, LCOLF_RGB, 0L, 0L, NULL);
                                        /* Load launch button bitmap */
    if(!hbitmapLaunchButton)
        hbitmapLaunchButton=GpiLoadBitmap(HPSSESSIONBARDRAW, pHP->hDLLPc2Resource,
            ID_BITMAPLAUNCHBUTTON, 0, 0);
    if(!hbitmapScrollLeft)
        hbitmapScrollLeft=GpiLoadBitmap(HPSSESSIONBARDRAW, pHP->hDLLPc2Resource,
            ID_BITMAPSCROLLLEFT, 0, 0);
    if(!hbitmapScrollRight)
        hbitmapScrollRight=GpiLoadBitmap(HPSSESSIONBARDRAW, pHP->hDLLPc2Resource,
            ID_BITMAPSCROLLRIGHT, 0, 0);
#ifdef  SLOWVIDEO
    WinSendMsg(hwnd, WM_DRAW, (MPARAM)hpsClient, NULL);
#else
    WinSendMsg(hwnd, WM_DRAW, (MPARAM)HPSSESSIONBARDRAW, NULL);
#endif  /* SLOWVIDEO */
    WinEndPaint(hpsClient);
    }
    break;

case WM_SIZE:
                                        /* Query client area */
    WinQueryWindowRect(hwnd, &rectlClient);
case WM_MOVE:
                                        /* Adjust to sizing */
    WinQueryWindowPos(pHP->hwndSessionFrame, &pHP->swpPC2Session);
    if(pHP->swpPC2Session.y<=0)
        {
        pHP->ulSessionBarFlag&=~(SB_POSITIONANY|SB_POSITIONTOP);
        pHP->ulSessionBarFlag|=SB_POSITIONBOTTOM;
        pHP->ulSessionBarFlag|=SB_LASTBOTTOM;
        }
    else if(pHP->swpPC2Session.y+pHP->swpPC2Session.cy>=pHP->swpScreen.cy)
        {
        pHP->ulSessionBarFlag&=~(SB_POSITIONANY|SB_POSITIONBOTTOM);
        pHP->ulSessionBarFlag|=SB_POSITIONTOP;
        pHP->ulSessionBarFlag|=SB_LASTTOP;
        }
    else
        {
        pHP->ulSessionBarFlag&=~(SB_POSITIONTOP|SB_POSITIONBOTTOM);
        pHP->ulSessionBarFlag|=SB_POSITIONANY;
        }
                                        /* Initialize */
    ulBorderSize=SESSIONBAR_BORDER;
    ulSpaceSize=SESSIONBAR_SPACE;
    ulFrameSize=SESSIONBAR_ICONFRAME;
                                        /* Recalculate client window */
    WinSendMsg(hwnd, WM_SESSIONBAR, NULL, NULL);
#ifndef SLOWVIDEO
                                        /* Request repaint when resized */
    if(msg==WM_SIZE)
        {

        BITMAPINFOHEADER2   bmihMemory; /* In memory SessionBar window bitmap info header */
                                        /* Bitmapformat of current display & driver combination */
        LONG                lBitmapFormat[2];

                                        /* Get the number of planes and the bitcount of the current
                                           display & driver combination */
        GpiQueryDeviceBitmapFormats(pHP->hpsSessionBarMemory, 2L, lBitmapFormat);
                                        /* Create a bitmap into the memory presentation space of same
                                           size as client area */
        memset(&bmihMemory, 0, sizeof(BITMAPINFOHEADER2));
        bmihMemory.cbFix=sizeof(BITMAPINFOHEADER2);
        bmihMemory.cx=pHP->swpPC2Session.cx;
        bmihMemory.cy=pHP->swpPC2Session.cy;
        bmihMemory.cPlanes=lBitmapFormat[0];
        bmihMemory.cBitCount=lBitmapFormat[1];
                                        /* Delete previous bitmap if it exists, after removing it
                                           from the presentation space (or we will get a memory leak
                                           as GpiDeleteBitmap() fails because it can't delete selected
                                           bitmaps */
        if(pHP->hbmSessionBarMemory)
            {
            GpiSetBitmap(pHP->hpsSessionBarMemory, NULLHANDLE);
            GpiDeleteBitmap(pHP->hbmSessionBarMemory);
            }
        pHP->hbmSessionBarMemory=GpiCreateBitmap(pHP->hpsSessionBarMemory, &bmihMemory, 0L, NULL, NULL);
        GpiSetBitmap(pHP->hpsSessionBarMemory, pHP->hbmSessionBarMemory);
                                        /* Now repaint Overview window */
        WinInvalidateRect(pHP->hwndClient, NULL, FALSE);
        }
#endif  /* SLOWVIDEO */
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));

/*                                                                                      *\
 * WM_SESSIONBARUNHIDE, ULONG ulUnhideFlag, LONG lPositionY                             *
\*                                                                                      */
case WM_SESSIONBARUNHIDE:
/*                                                                                      *\
 * Message posted by working thread and then by our window procedure to unhide or hide  *
 * the SessionBar dynamically.                                                          *
\*                                                                                      */
                                        /* Depending if we're using a memory presentation
                                           space or having to redraw the SessionBar at every
                                           position it moves to, we can adjust the rate the
                                           SessionBar gets slided in */
#ifdef  SLOWVIDEO
#define SESSIONBAR_INCREMENT            4
#else
#define SESSIONBAR_INCREMENT            2
#endif  /* SLOWVIDEO */
    {
    LONG    lPositionY=LONGFROMMP(mp2);

                                        /* We have to differentiate if the SessionBar should
                                           slide in from top or bottom of screen (because
                                           the calculation of the limiting positions differ) */
    if(LONGFROMMP(mp1) & SB_LASTTOP)
        {
        if(lPositionY<pHP->swpScreen.cy-pHP->swpPC2Session.cy) lPositionY=pHP->swpScreen.cy-pHP->swpPC2Session.cy;
        if(lPositionY>(pHP->swpScreen.cy+1)) lPositionY=pHP->swpScreen.cy+1;
                                        /* Advance to current position */
        memset(&swpSessionBar, 0, sizeof(swpSessionBar));
        swpSessionBar.hwnd=pHP->hwndSessionFrame;
        swpSessionBar.fl=SWP_MOVE;
        swpSessionBar.y=lPositionY;
        WinSetMultWindowPos(pHP->habPc2, &swpSessionBar, 1);
#ifdef  SLOWVIDEO
                                        /* Without a memory presentation space, we have
                                           to redraw for every position.
                                           Call WM_PAINT explicitely as WinInvalidateRect()
                                           does not cause an update and repaint at every
                                           position, I think this is because WM_PAINT messages
                                           are accumulated to some degree by PM) */
        WinSendMsg(hwnd, WM_PAINT, NULL, NULL);
#else
                                        /* Blitting from a memory presentation space
                                           is faster for sure */
        WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory,
            NULL, (POINTL *)&rectlClient, 0L, 0L, DBM_NORMAL);
        DosSleep(0);
#endif  /* SLOWVIDEO */
        if(LONGFROMMP(mp1) & SB_UNHIDEFROMTOP)
            {
            if((lPositionY>(pHP->swpScreen.cy-pHP->swpPC2Session.cy)) && !(pHP->ulSessionBarFlag & SB_HIDDEN))
                WinPostMsg(hwnd, WM_SESSIONBARUNHIDE,
                    mp1, MPFROMLONG(lPositionY-SESSIONBAR_INCREMENT));
            }
        else
            {
            if((lPositionY<(pHP->swpScreen.cy+1)) && (pHP->ulSessionBarFlag & SB_HIDDEN))
                WinPostMsg(hwnd, WM_SESSIONBARUNHIDE,
                    mp1, MPFROMLONG(lPositionY+SESSIONBAR_INCREMENT));
            }
        }
    else
        {
        if(lPositionY>0) lPositionY=0;
        if(lPositionY<-pHP->swpPC2Session.cy) lPositionY=-pHP->swpPC2Session.cy;
        memset(&swpSessionBar, 0, sizeof(swpSessionBar));
        swpSessionBar.hwnd=pHP->hwndSessionFrame;
        swpSessionBar.fl=SWP_MOVE;
        swpSessionBar.y=lPositionY;
        WinSetMultWindowPos(pHP->habPc2, &swpSessionBar, 1);
#ifdef  SLOWVIDEO
        WinSendMsg(hwnd, WM_PAINT, NULL, NULL);
#else
        WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory,
            NULL, (POINTL *)&rectlClient, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
        DosSleep(0);
        if(LONGFROMMP(mp1) & SB_UNHIDEFROMBOTTOM)
            {
            if((lPositionY<0) && !(pHP->ulSessionBarFlag & SB_HIDDEN))
                WinPostMsg(hwnd, WM_SESSIONBARUNHIDE,
                    mp1, MPFROMLONG(lPositionY+SESSIONBAR_INCREMENT));
            }
        else
            {
            if((lPositionY>-pHP->swpPC2Session.cy) && (pHP->ulSessionBarFlag & SB_HIDDEN))
                WinPostMsg(hwnd, WM_SESSIONBARUNHIDE,
                    mp1, MPFROMLONG(lPositionY-SESSIONBAR_INCREMENT));
            }
        }
    }
    break;

case WM_FOCUSCHANGE:
    if((SHORT1FROMMP(mp2)==TRUE) && (pHP->ulStatusFlag1 & FOLLOWFOCUS))
        WinPostMsg(hwnd, WM_ZORDER, NULL, NULL);
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));

/*                                                                                      *\
 * Syntax: WM_ZORDER, HWND hwndFrame2Bottom, NULL                                       *
\*                                                                                      */
case WM_ZORDER:
/*                                                                                      *\
 * This message is posted by the SessionBar to itself, when the SessionBar got the      *
 * input focus, to change the input focus to the next window lower in Z-order, as it    *
 * does not make sense for the user to have the focus at the SessionBar, as there is    *
 * no indication and nothing he can do there.                                           *
\*                                                                                      */
    {
    HWND    hwndNextWindow;

    if(HWNDFROMMP(mp1)!=NULLHANDLE)
        {
        HWND    hwndNextFrame;
        HWND    hwndNextFrameOwner;
        POINTL  ptlMouse;
    
        WinQueryPointerPos(HWND_DESKTOP, &ptlMouse);
        hwndNextFrameOwner=hwndNextFrame=WinWindowFromPoint(HWND_DESKTOP, &ptlMouse, FALSE);
        do  {
            hwndNextFrame=hwndNextFrameOwner;
            hwndNextFrameOwner=WinQueryWindow(hwndNextFrame, QW_OWNER);
            } while(hwndNextFrameOwner!=NULLHANDLE);

        WinPostMsg(pHP->hwndWorkingThread, WM_QUICKSWITCH,
            MPFROMLONG(QUICKSWITCH_SWITCHTO), MPFROMHWND(hwndNextFrame));
        return((MRESULT)TRUE);
        }
    hwndNextWindow=GetZOrderTop();
/*                                                                                      *\
 * If a window matching our criterias could be found switch to it, otherwise post a     *
 * message to change the input focus to the window below the mouse pointer as a last    *
 * resort.                                                                              *
\*                                                                                      */
    if(hwndNextWindow!=NULLHANDLE)
        {
                                        /* If we found a window, "misuse" the working thread to switch
                                           to that window */
#ifdef  DEBUG_SB
        printf("SessionBar: WM_ZORDER switched to %08X\n", (int)hwndNextWindow);
#endif  /* DEBUG_SB */
        WinPostMsg(pHP->hwndWorkingThread, WM_QUICKSWITCH,
            MPFROMLONG(QUICKSWITCH_SWITCHTOWINDOW), MPFROMHWND(hwndNextWindow));
        }
    else
                                        /* Otherwise switch to window below mouse pointer */
        WinPostMsg(pHP->hwndClient, WM_ZORDER, NULL, NULL);
    }
    break;

case WM_TIMER:
/*                                                                                      *\
 * This message is posted by the OverView window to allow us to adjust the current time *
 * displayed in the SessionBar if the mouse pointer is not over a Session's icon.       *
\*                                                                                      */
    {
                                        /* Current time & time compatible with C runtime */
    time_t              time_tCurrent;
    struct  tm          *ptmCurrent;
    ULONG               ulProcessorStatus=0;

                                        /* Get current time in C runtime format */
    time(&time_tCurrent);
    ptmCurrent=localtime(&time_tCurrent);
                                        /* Only update time information in the SessionBar's session
                                           name rectangle if time changed */
    if(memcmp(&tmPrevious, ptmCurrent, sizeof(tmPrevious)))
        {
        UCHAR   *pucDateTime;
        int     iCharCount;

        tmPrevious=*ptmCurrent;
                                        /* If no time is requested, don't display it. If it is
                                           requested, either display NLS or user defined format */
        if(pHP->ulStatusFlag2 & NODATETIMESB)
            ucCurrentTime[0]='\0';
        else if(strlen(pHP->ucSessionBarDateTime)==0)
            {
            pucDateTime=GetDateTimeNLS(FALSE);
            strcpy(ucCurrentTime, pucDateTime);
            free(pucDateTime);
            }
        else
            {
            iCharCount=strftime(ucCurrentTime, sizeof(ucCurrentTime)-1, 
                pHP->ucSessionBarDateTime, ptmCurrent);
            }
        WinSendMsg(hwnd, WM_DRAWSESSIONTEXT,
            MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(SESSIONBAR_DRAWSTATUS));
        }
                                        /* If we have required OS/2 support, query the current
                                           processor status. If the processor status has changed
                                           e.g. by using MPCPUMON.EXE or my CPU/2 utility, then
                                           update the popup menu accordingly. For e.g. Warp 4,
                                           where this API is not supported, we can safely assume
                                           that CPU 1 is always online (same for SMP) */
    if(pHP->DosGetProcessorStatus)
        {
        ulTemp=0;
        for(ulNumProcessor=1; ulNumProcessor<=pHP->ulNumProcessors; ulNumProcessor++)
            {
            (pHP->DosGetProcessorStatus)(ulNumProcessor, &ulProcessorStatus);
            if(ulProcessorStatus)
                ulTemp|=(0x00000001<<(ulNumProcessor-1));
            WinSendMsg(pHP->hwndPopupMenuProcessor, MM_QUERYITEM,
                MPFROM2SHORT(ID_SBPROCESSORMENU+ulNumProcessor, TRUE), (MPARAM)&menuitemCPU);
            if((!!ulProcessorStatus)!=(!!(menuitemCPU.afAttribute & MIA_CHECKED)))
                WinSendMsg(pHP->hwndPopupMenuProcessor, MM_SETITEMATTR,
                    MPFROM2SHORT(ID_SBPROCESSORMENU+ulNumProcessor, TRUE),
                    MPFROM2SHORT(MIA_CHECKED, (menuitemCPU.afAttribute & MIA_CHECKED ? 0 : MIA_CHECKED)));
            }
                                        /* If the processor status has change for one or more CPUs
                                           then redraw CPU Performance graphics area and clear
                                           historic values for the just offlined processor */
        if(ulTemp!=pHP->ulProcessorStatus)
            {
            pHP->ulProcessorStatus=ulTemp;
            WinSendMsg(hwnd, WM_CALCULATECPUSTATUS, MPFROMLONG(WM_CREATE), MPFROMLONG(pHP->ulProcessorStatus));
            WinInvalidateRect(pHP->hwndSessionClient, prectlCPUStatus, FALSE);
            }
        }
    else
        pHP->ulProcessorStatus=0x00000001;
                                        /* We update the CPU Performance automatically only if the
                                           timer has expired the required amount of timer ticks */
    while((pHP->DosPerfSysCall!=NULL) && (pHP->ulStatusFlag2 & CPUPERFORMANCESB))
        {
                                        /* If the timer is not initialized yet, don't draw any CPU
                                           Performance graphics to avoid updates in "uneven" seconds
                                           (e.g. 3s, 13s, ... instead of 0s, 10s, ...). We initialize
                                           the uninitialized "-1" timer ticks on "even" seconds */
        if(ulTimerTicks==(ULONG)-1)
            {
            if((pHP->ulPerformanceFlag & PERFORMANCE_60S) ||
                ((pHP->ulPerformanceFlag & PERFORMANCE_5M) && (tmPrevious.tm_sec % 5==0)) ||
                ((pHP->ulPerformanceFlag & PERFORMANCE_30M) && (tmPrevious.tm_sec % 30==0)) ||
                ((pHP->ulPerformanceFlag & PERFORMANCE_1H) && (tmPrevious.tm_sec==0)))
                ulTimerTicks=0;
            else
                break;
            }
                                        /* If ulCPUProcessorPixels<SESSIONBAR_CPUPIXELS, then update only
                                           all fractional timer ticks */
        if((ulCPUProcessorPixels) && (ulCPUProcessorPixels<SESSIONBAR_CPUPIXELS))
            if((ulTimerTicks % (TICKSPERSECOND*SESSIONBAR_CPUPIXELS/ulCPUProcessorPixels))!=0)
                break;
                                        /* Update CPU Processor area when WM_TIMER is functional the first
                                           time (as WM_PAINT may be earlier than WM_TIMERs, we initialize
                                           the CPU Performance graphics from WM_TIMER). */
                                        /* The current reporting intervall gets a checkmark in the
                                           popup menu */
        if(ulCPUInitialized==FALSE)
            {
            for(ulProcessorStatus=0;
                !((pHP->ulPerformanceFlag>>ulProcessorStatus) & 0x00000001);
                ulProcessorStatus++)
                ; /* NOP */
            ulProcessorStatus+=ID_SBSETTINGS60S;
            WinSendMsg(pHP->hwndPopupMenuProcessor, MM_SETITEMATTR,
                MPFROM2SHORT(ulProcessorStatus, TRUE),
                MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
            }
                                        /* Now depending on the scale, ensure that we update the CPU Performance
                                           graphics by shifing leftwards pixelwise, by posting the message
                                           only when such an update is pending (related to our "nominal" CPU
                                           Performance graphics width of SESSIONBAR_CPUPIXELS) */
        if(((pHP->ulPerformanceFlag & PERFORMANCE_60S) && (ulTimerTicks % (TICKSPERSECOND/ulCPUTicksAdjustMult))==0) ||
            ((pHP->ulPerformanceFlag & PERFORMANCE_5M) && (ulTimerTicks % (TICKSPERSECOND*5/ulCPUTicksAdjustMult))==0) ||
            ((pHP->ulPerformanceFlag & PERFORMANCE_30M) && (ulTimerTicks % (TICKSPERSECOND*30/ulCPUTicksAdjustMult))==0) ||
            ((pHP->ulPerformanceFlag & PERFORMANCE_1H) && (ulTimerTicks % (TICKSPERSECOND*60/ulCPUTicksAdjustMult))==0))
            {
                                        /* Save current time as a "reference" time */
            tmCPUStatus=tmPrevious;
                                        /* When required (e.g. during startup or changing the time basis),
                                           (re)initialize the CPU Performance history and graphics */
            if(ulCPUInitialized==FALSE)
                {
                WinSendMsg(hwnd, WM_CALCULATECPUSTATUS, MPFROMLONG(WM_CREATE), NULL);
                WinSendMsg(hwnd, WM_DRAWCPUSTATUS, (MPARAM)HPSSESSIONBARDRAW, MPFROMLONG(WM_CREATE));
                WinSendMsg(hwnd, WM_DRAWCPUSTATUS, (MPARAM)HPSSESSIONBARDRAW, MPFROMLONG(WM_PAINT));
                }
                                        /* The current time is used for (re)drawing the CPU Performance
                                           graphics until it gets updated by a new timer tick */
            WinSendMsg(hwnd, WM_DRAWCPUSTATUS, (MPARAM)HPSSESSIONBARDRAW, MPFROMLONG(WM_DRAW));
            }
                                        /* Break loop anyway */
        break;
        }
                                        /* Update APM status area */
    if(ulAPMStatusPrevious!=pHP->ulAPMStatus)
        {
        WinPostMsg(hwnd, WM_DRAWAPMSTATUS, (MPARAM)HPSSESSIONBARDRAW, NULL);
        WinPostMsg(hwnd, WM_DRAWAPMSTATUS, (MPARAM)HPSSESSIONBARDRAW, MPFROMLONG(WM_MOUSEMOVE));
        }
                                        /* Advance to next timer tick */
    if(ulTimerTicks!=(ULONG)-1)
        ulTimerTicks++;
    }
    break;

/*                                                                                      *\
 * Syntax: WM_DRAW, HPS hpsDraw, RECTL *prectlDraw                                      *
\*                                                                                      */
case WM_DRAW:
/*                                                                                      *\
 * This undocumented message is used by PM controls to allow them to paint itself into  *
 * a provided presentation space. If rectlDraw is 0, we have to draw the entire control.*
\*                                                                                      */
    {
    HPS         hpsDraw=(HPS)LONGFROMMP(mp1);

#ifdef  DEBUG_SB
    printf("SessionBar: WM_DRAW\n");
#endif  /* DEBUG_SB */
                                        /* If now presentation space is passed (from WM_PAINT), use
                                           permanent presentation space */
    if(!hpsDraw)
        hpsDraw=HPSSESSIONBARDRAW;
                                        /* Fill invalid rectangle with dialog background color */
    WinFillRect(hpsDraw, &rectlClient, pHP->ulSessionBarRGBColorBgnd);
                                        /* Draw PC/2 Launch Button and its frame */
    WinSendMsg(hwnd, WM_DRAWLAUNCHBUTTON, MPFROMP(hpsDraw), MPFROMLONG(WM_NULL));
                                        /* Draw separator */
    WinDrawBorder(hpsDraw, prectlSeparator, ulBorderSize, ulBorderSize, 0, 0, DB_RAISED);
                                        /* Draw 3D border */
    WinDrawBorder(hpsDraw, &rectlClient, ulBorderSize, ulBorderSize, 0, 0, DB_RAISED);
                                        /* Draw Sessions systemmenu icons */
    ulRedrawScrollButtons=TRUE;
    WinSendMsg(hwnd, WM_HITTESTSESSIONICONS, MPFROMP(hpsDraw), MPFROMLONG(WM_MOUSEMOVE));
    for(ulTemp=0; ulTemp<ulSessionIconsCount; ulTemp++)
        WinSendMsg(hwnd, WM_DRAWSESSIONICONS, MPFROMP(hpsDraw), MPFROMLONG(ulTemp));
                                        /* Draw APM status frame and area */
    WinDrawBorder(hpsDraw, prectlAPMStatus, ulBorderSize, ulBorderSize, 0, 0, DB_DEPRESSED);
    WinSendMsg(hwnd, WM_DRAWAPMSTATUS, MPFROMP(hpsDraw), NULL);
                                        /* Draw CPU status frame and area */
    if(pHP->ulStatusFlag2 & CPUPERFORMANCESB)
        {
#ifdef  SLOWVIDEO
        WinFillRect(hpsDraw, prectlCPUStatus, pHP->ulSessionBarRGBColorCPU);
#endif  /* SLOWVIDEO */
        WinDrawBorder(hpsDraw, prectlCPUStatus, ulBorderSize, ulBorderSize, 0, 0, DB_DEPRESSED);
        WinSendMsg(hwnd, WM_DRAWCPUSTATUS, MPFROMP(hpsDraw), MPFROMLONG(WM_PAINT));
        }
                                        /* Draw Session name's 3D frame and the textfield */
    WinDrawBorder(hpsDraw, prectlSessionName, ulBorderSize, ulBorderSize, 0, 0, DB_DEPRESSED);
    WinSendMsg(hwnd, WM_DRAWSESSIONTEXT,
        MPFROMP(pHP->hpsSession), MPFROMLONG(SESSIONBAR_REPAINTCURRENT));
#ifndef SLOWVIDEO
                                        /* Draw the whole memory bitmap into the client area,
                                           without any background or foreground color because
                                           we have no monochrome bitmap */
    WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory, NULL,
        (POINTL *)&rectlClient, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SESSIONBAR, NULL, NULL                                                    *
\*                                                                                      */
case WM_SESSIONBAR:
/*                                                                                      *\
 * This message is sent by the SessionBar window to itself whenever the client gets     *
 * resized or moved.                                                                    *
\*                                                                                      */
    {
                                        /* Launch Button width (resolution dependent) */
    ULONG       ulLaunchButtonWidth=SESSIONBAR_LAUNCHWIDTHVGA;
                                        /* Scroll buttons height */
    ULONG       ulScrollHeight;
                                        /* Width of the rectangle we draw the sessions icons into */
    ULONG       ulSessionIconsWidth;
                                        /* Temporary correction to fit only whole icons */
    ULONG       ulLayoutCorrection;

#ifdef  DEBUG_SB
#endif  /* DEBUG_SB */
                                        /* Calculate the session icons size */
    ulIconSize=pHP->lIconSize>>pHP->ulSessionBarScale;
                                        /* Calculate the Launch Button rectangle (including 3D frame) */
    ulLaunchButtonWidth=SESSIONBAR_LAUNCHWIDTHVGA;
    if(pHP->lIconSize==ICONSIZE_BGA)
        ulLaunchButtonWidth=SESSIONBAR_LAUNCHWIDTHBGA;
    *prectlLaunchFrame=rectlClient;
    prectlLaunchFrame->xLeft+=(ulBorderSize+ulSpaceSize);
    prectlLaunchFrame->yBottom+=(ulBorderSize+ulSpaceSize);
    prectlLaunchFrame->xRight=prectlLaunchFrame->xLeft+(ulLaunchButtonWidth>>pHP->ulSessionBarScale)
        +(ulFrameSize<<1);
    prectlLaunchFrame->yTop-=(ulBorderSize+ulSpaceSize);
                                        /* Calculate separator */
    *prectlSeparator=rectlClient;
    prectlSeparator->xLeft=prectlLaunchFrame->xRight+ulSpaceSize;
    prectlSeparator->xRight=prectlSeparator->xLeft+(2*ulBorderSize);
                                        /* Calculate SessionBar's session name textfield (including 3D frame) */
    ulSessionNameWidth=(pHP->swpScreen.cx*SESSIONBAR_NAMESCALEX)/100;
    *prectlSessionName=*prectlLaunchFrame;
    prectlSessionName->xLeft=prectlSessionName->xRight=rectlClient.xRight-
        (ulBorderSize+ulSpaceSize);
    if(!(pHP->ulStatusFlag2 & NOSESSIONNAMESB))
        prectlSessionName->xLeft-=(ulSessionNameWidth+ulBorderSize);
                                        /* Calculate CPU Performance area, whose width should be a multiple
                                           of 60 pixels per processor of SESSIONBAR_CPUSCALEX percentage of
                                           the screen in sum. The CPU Performance area matches the Session
                                           Name field's left border with 0 width, if no CPU Performance
                                           measurements can be made */
    *prectlCPUStatus=*prectlSessionName;
    prectlCPUStatus->xRight=prectlCPUStatus->xLeft;
                                        /* If no CPU Performance graphics is wanted, then initialize the
                                           utilization history data when it was deactivated in the Desktop
                                           Dialog but was available previously */
    if(!(pHP->ulStatusFlag2 & CPUPERFORMANCESB) && (ulCPUInitialized==TRUE))
        {
        WinSendMsg(hwnd, WM_CALCULATECPUSTATUS, MPFROMLONG(WM_DESTROY), NULL);
        ulCPUInitialized=FALSE;
        }
    if((pHP->DosPerfSysCall!=NULL) && (pHP->ulStatusFlag2 & CPUPERFORMANCESB))
        {
                                        /* The CPU Performance graphics area is nominal 60 pixels
                                           (SESSIONBAR_CPUPIXELS) wide, but to adjust for larger and
                                           smaller displays, we allow multiples and fractions.
                                           Though this could be extended, more than twice is a problem
                                           as the timer ticks currently twice a second (and for the
                                           shortest supported intervall this means 2 ticks/second and
                                           not more), less than half is a problem, because the graphics
                                           becomes unreadable/distorted */
        ULONG   ulFactors[]={1, 2};
        ULONG   ulFactorIndex;
        ULONG   ulCPUCompleteSize;

        ulCPUProcessorPixels=((pHP->swpScreen.cx*SESSIONBAR_CPUSCALEX)/100)/pHP->ulNumProcessors;
        if(ulCPUProcessorPixels>=SESSIONBAR_CPUPIXELS)
            {
                                        /* Find the multiple of SESSIONBAR_CPUPIXELS that causes the CPU
                                           Performance graphics to be nearest to the SESSIONBAR_CPUSCALEX
                                           percentage of the screen in pixels */
            ulFactorIndex=ulCPUProcessorPixels/SESSIONBAR_CPUPIXELS;
            if(ulFactorIndex>(sizeof(ulFactors)/sizeof(ulFactors[0]))-1)
                ulFactorIndex=(sizeof(ulFactors)/sizeof(ulFactors[0]))-1;
            else if((SESSIONBAR_CPUPIXELS*ulFactors[ulFactorIndex]-ulCPUProcessorPixels)>=
                (ulCPUProcessorPixels-SESSIONBAR_CPUPIXELS*ulFactors[ulFactorIndex-1]))
                ulFactorIndex--;
            ulCPUProcessorPixels=SESSIONBAR_CPUPIXELS*ulFactors[ulFactorIndex];
            }
        else
            {
                                        /* Find the fraction of SESSIONBAR_CPUPIXELS that causes the CPU
                                           Performance graphics to be nearest to the SESSIONBAR_CPUSCALEX
                                           percentage of the screen in pixels */
            for(ulFactorIndex=1; ulFactorIndex<(sizeof(ulFactors)/sizeof(ulFactors[0])); ulFactorIndex++)
                if(ulCPUProcessorPixels>(SESSIONBAR_CPUPIXELS/ulFactors[ulFactorIndex]))
                    break;
            if((ulCPUProcessorPixels-SESSIONBAR_CPUPIXELS/ulFactors[ulFactorIndex])>=
                (SESSIONBAR_CPUPIXELS/ulFactors[ulFactorIndex-1]-ulCPUProcessorPixels))
                ulFactorIndex--;
            ulCPUProcessorPixels=SESSIONBAR_CPUPIXELS/ulFactors[ulFactorIndex];
            }
                                        /* Now sum up the space required for all processors, which for
                                           multiple processors includes a space in between of
                                           SESSIONBAR_CPUSPACE pixels */
        ulCPUCompleteSize=(ulCPUProcessorPixels+SESSIONBAR_CPUSPACE)*pHP->ulNumProcessors-SESSIONBAR_CPUSPACE;
                                        /* And calculate the size including borders and space to adjacent
                                           areas */
        prectlCPUStatus->xLeft=prectlCPUStatus->xRight=prectlSessionName->xLeft-(ulBorderSize+ulSpaceSize);
        prectlCPUStatus->xLeft-=((ulBorderSize+ulSpaceSize)+ulCPUCompleteSize-1);
                                        /* And calculate the number of pixel columns a single CPU Performance
                                           value represents (e.g. if ulCPUProcessorPixels/SESSIONBAR_CPUPIXELS
                                           is 2 that means 2 columns need to be drawn for every timer based
                                           WM_DRAW event). However, for 2 ticks per column we also use a
                                           positive value */
        ulCPUTicksAdjustMult=ulCPUTicksAdjustDiv=1;
        if(ulCPUProcessorPixels>=SESSIONBAR_CPUPIXELS)
            ulCPUTicksAdjustMult=ulCPUProcessorPixels/SESSIONBAR_CPUPIXELS;
        else
            ulCPUTicksAdjustDiv=SESSIONBAR_CPUPIXELS/ulCPUProcessorPixels;
        }
                                        /* Calculate APM status area, whose width is a fourth of the
                                           SessionBar's height. The APM status area matches the CPU Performance
                                           area's left border with 0 width, if no APM management could
                                           be detected */
    *prectlAPMStatus=*prectlCPUStatus;
    prectlAPMStatus->xRight=prectlAPMStatus->xLeft;
    if(pHP->ulRuntimeFlag & APMPOWERSUPPORT)
        {
        prectlAPMStatus->xLeft=prectlAPMStatus->xRight=prectlCPUStatus->xLeft-(ulBorderSize+ulSpaceSize);
        prectlAPMStatus->xLeft-=((ulBorderSize+ulSpaceSize)+
            (SESSIONBAR_APMPIXELS+(rectlClient.yTop-rectlClient.yBottom)>>2));
        }
                                        /* Calculate Sessions systemmenu icon rectangles (including backward, sessions
                                           and forward icons). The SessionBar's APM status and Session name will get
                                           adjusted for an optimal layout */
                                        /* Calculate scroll buttons */
    *prectlScrollLeft=*prectlAPMStatus;
    ulScrollHeight=(ULONG)-1;
    if(pHP->ulSessionBarScale==0)
        ulScrollHeight=(prectlScrollLeft->yTop-prectlScrollLeft->yBottom-((pHP->lIconSize>>1)+(ulBorderSize<<1)))>>1;
    prectlScrollLeft->yBottom+=ulScrollHeight;
    prectlScrollLeft->yTop=prectlScrollLeft->yBottom+(ulBorderSize<<1)+(pHP->lIconSize>>1);
    prectlScrollLeft->xLeft=prectlSeparator->xRight+ulSpaceSize;
    prectlScrollLeft->xRight=prectlScrollLeft->xLeft+(ulBorderSize<<1)+(pHP->lIconSize>>1);
    *prectlScrollRight=*prectlScrollLeft;
    prectlScrollRight->xRight=prectlAPMStatus->xLeft-ulSpaceSize;
    prectlScrollRight->xLeft=prectlScrollRight->xRight-((pHP->lIconSize>>1)+(ulBorderSize<<1));
                                        /* Calculate SessionBar's sessions icons */
    *prectlSessionIcons=*prectlAPMStatus;
    prectlSessionIcons->xLeft=prectlScrollLeft->xRight+ulSpaceSize;
    prectlSessionIcons->xRight=prectlScrollRight->xLeft-ulSpaceSize;
                                        /* Calculate layout correction of Session Name field, to ensure that
                                           we have no fractional icons in the SessionBar */
    ulSessionIconsWidth=prectlSessionIcons->xRight-prectlSessionIcons->xLeft;
    ulLayoutCorrection=ulIconSize+2*ulFrameSize+ulSpaceSize;
    ulSessionIconsDrawn=ulSessionIconsWidth/ulLayoutCorrection;
    ulLayoutCorrection=ulSessionIconsWidth-ulLayoutCorrection*ulSessionIconsDrawn;
    ulLayoutCorrection+=ulSpaceSize;
    prectlSessionIcons->xRight-=ulLayoutCorrection;
    prectlScrollRight->xLeft-=ulLayoutCorrection;
    prectlScrollRight->xRight-=ulLayoutCorrection;
    prectlAPMStatus->xLeft-=ulLayoutCorrection;
    prectlAPMStatus->xRight-=ulLayoutCorrection;
    prectlCPUStatus->xLeft-=ulLayoutCorrection;
    prectlCPUStatus->xRight-=ulLayoutCorrection;
    prectlSessionName->xLeft-=ulLayoutCorrection;
    }
    break;

/*                                                                                      *\
 * Syntax: WM_DRAWLAUNCHBUTTON, HPS hpsDraw, ULONG ulMouseEvent                         *
\*                                                                                      */
case WM_DRAWLAUNCHBUTTON:
/*                                                                                      *\
 * Draw the PC/2 Launch Button and its frame. The second parameter is either WM_NULL,   *
 * when the button should be just redrawn (window invalidated), or WM_BUTTON1DOWN &     *
 * WM_BUTTON1UP or WM_MOUSEMOVE, when a mouse event occured.                            *
\*                                                                                      */
    {
    static ULONG    ulLaunchButtonEnabled=FALSE;
    static ULONG    ulLaunchButtonStateNew=WM_BUTTON1UP;
    static ULONG    ulLaunchButtonState=WM_BUTTON1UP;
    HPS             hpsDraw=(HPS)LONGFROMMP(mp1);
    ULONG           ulMouseEvent=(ULONG)LONGFROMMP(mp2);
    RECTL           rectlLaunchButton;
    ULONG           ulShift=0;
    ULONG           ulMouseInRectangle;
    POINTL          pointlPopupMenu;

    if(prectlLaunchFrame==0)
        break;
    rectlLaunchButton=*prectlLaunchFrame;
                                        /* Check for drawing shoud change to enabled style (after PC/2
                                           initialization) */
    if(ulMouseEvent==WM_ENABLE)
        {
        ulLaunchButtonEnabled=TRUE;
        ulMouseEvent=WM_NULL;
        hpsDraw=HPSSESSIONBARDRAW;
        }
                                        /* See if anything changed so we avoid unnecessary painting */
    ulLaunchButtonStateNew=ulLaunchButtonState;
    ulMouseInRectangle=(ULONG)WinPtInRect(pHP->habPc2, prectlLaunchFrame, &pointlMouse);
                                        /* If the pointer is outside the Launch Button, force it into
                                           the effect of a WM_MOUSEBUTTON1UP state. Otherwise the state
                                           can only be changed by the mouse button */
    if(ulMouseInRectangle==FALSE)
        ulLaunchButtonStateNew=WM_BUTTON1UP;
    else
        {
        if(ulMouseEvent==WM_MOUSEMOVE)
            return((MRESULT)FALSE);
        if(ulMouseEvent!=WM_NULL)
            ulLaunchButtonStateNew=ulMouseEvent;
        }
    if((ulMouseEvent!=WM_NULL) && (ulLaunchButtonState==ulLaunchButtonStateNew))
        return((MRESULT)FALSE);
#ifdef  DEBUG_SB
    printf("SessionBar: WM_DRAWLAUNCHBUTTON\n");
#endif  /* DEBUG_SB */
                                        /* Get the Launch Button position with correction of the
                                           depressed/pressed 3D effect. Initially the button is in
                                           a disabled style, until PC/2 startup is ready */
    if(ulLaunchButtonEnabled==FALSE)
        {
        if(ulLaunchButtonStateNew==WM_BUTTON1DOWN)
            WinAlarm(HWND_DESKTOP, WA_WARNING);
        ulLaunchButtonStateNew=WM_BUTTON1UP;
        }
    if(ulLaunchButtonStateNew==WM_BUTTON1DOWN)
        ulShift=1;
    rectlLaunchButton.xLeft+=(ulFrameSize+ulShift);
    rectlLaunchButton.yBottom+=(ulFrameSize-ulShift);
    rectlLaunchButton.xRight-=(ulFrameSize-ulShift);
    rectlLaunchButton.yTop-=(ulFrameSize+ulShift);
    BlitBitmap(hwnd, hpsDraw, hbitmapLaunchButton, &rectlLaunchButton);
    WinDrawBorder(hpsDraw, prectlLaunchFrame, ulFrameSize, ulFrameSize, 0, 0,
        (ulShift==0 ? DB_RAISED : DB_DEPRESSED));
    if(ulLaunchButtonEnabled==FALSE)
        DisableWindowRect(hpsDraw, &rectlLaunchButton, SYSCLR_DIALOGBACKGROUND);
#ifndef SLOWVIDEO
    WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory,
        prectlLaunchFrame, (POINTL *)prectlLaunchFrame, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
                                        /* If the Launch button is depressed, activate the Popup Menu,
                                           aligned to the current SessionBar position (by passing (-1, -1)
                                           as the coordinated) */
    pointlPopupMenu=pointlMouse;
    WinMapWindowPoints(hwnd, HWND_DESKTOP, &pointlPopupMenu, 1);
    if((ulLaunchButtonState==WM_BUTTON1DOWN) &&
        (ulLaunchButtonStateNew==WM_BUTTON1UP) &&
        (ulMouseInRectangle==TRUE))
        WinPostMsg(pHP->hwndClient, WM_POPUPMENU,
            MPFROM2SHORT(-1, -1), MPFROMHWND(pHP->hwndPopupMenu));
    if(ulLaunchButtonStateNew!=WM_NULL)
        ulLaunchButtonState=ulLaunchButtonStateNew;
    return((MRESULT)FALSE);
    }

/*                                                                                      *\
 * Syntax: WM_DRAWSESSIONICONS, HPS hpsDraw, ULONG ulSession                            *
\*                                                                                      */
case WM_DRAWSESSIONICONS:
/*                                                                                      *\
 * Draw the PC/2 Session icons and their frames. The second parameter is the index of   *
 * the button (0 based) that should be redrawn. The scroll buttons are also taken into  *
 * the calculation, so that only those icons are drawn that are visibly.                *
\*                                                                                      */
    {
    HPS             hpsDraw=(HPS)LONGFROMMP(mp1);
    ULONG           ulSession=LONGFROMMP(mp2);
    ULONG           ulForceDraw=FALSE;
    ULONG           ulSessionIconsDraw;
    RECTL           rectlCurrentSession;
    RECTL           rectlIcon;
    POINTL          ptlLL, ptlUR;
    SWITCHLIST      *pswitchlistCurrentSession;
    HPOINTER        hpointerFrame;


#ifdef  DEBUG_SB
    printf("SessionBar: WM_DRAWSESSIONICONS (%d)\n", (int)ulSession);
#endif  /* DEBUG_SB */
    if(hpsDraw==NULLHANDLE)
        {
        ulForceDraw=TRUE;
        hpsDraw=HPSSESSIONBARDRAW;
        }
                                        /* Start with 1st session */
    pswitchlistCurrentSession=pswitchlistAnchor;
                                        /* Start with 1st session's icon */
    rectlCurrentSession=*prectlSessionIcons;
    rectlCurrentSession.xRight=rectlCurrentSession.xLeft+2*ulFrameSize+ulIconSize;
                                        /* Find the session whose icon we're going to draw */
    ulSessionIconsDraw=0;
    while(ulSession!=0)
        {
                                        /* Advance to next icon if the current session is one
                                           that is currently visible on the SessionBar */
        if((pswitchlistCurrentSession!=NULL) && !(pswitchlistCurrentSession->ulStatusFlag & SESSIONHIDE))
            {
            if(ulSessionIconsDraw>=ulSessionIconsFirst)
                {
                rectlCurrentSession.xLeft+=2*ulFrameSize+ulIconSize+ulSpaceSize;
                rectlCurrentSession.xRight=rectlCurrentSession.xLeft+2*ulFrameSize+ulIconSize;
                }
            }
        ulSessionIconsDraw++;
        ulSession--;
                                        /* Advance to next session icon's control structure */
        if(pswitchlistCurrentSession!=NULL)
            pswitchlistCurrentSession=pswitchlistCurrentSession->pNext;
        }
                                        /* If a icons should be drawn that is ordinally before the
                                           first icon drawn in the SessionBar, or ordinally after the
                                           last icon draw, we don't need to draw it at all */
    if((ulSessionIconsDraw<ulSessionIconsFirst) ||
        (rectlCurrentSession.xLeft>prectlSessionIcons->xRight))
        break;
    rectlIcon=rectlCurrentSession;
                                        /* If there is no longer a session at the current position,
                                           clear previous icon */
    if(!pswitchlistCurrentSession)
        {
        GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorBgnd);
        GpiMove(hpsDraw, (POINTL *)(&rectlIcon.xLeft));
        GpiBox(hpsDraw, DRO_OUTLINEFILL, (POINTL *)(&rectlIcon.xRight), 0, 0);
#ifndef SLOWVIDEO
        WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory,
            &rectlCurrentSession, (POINTL *)&rectlCurrentSession, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
        }
                                        /* Blit the session's icon if available (that is if the session
                                           for which we are drawing the icon, hasn't been removed from
                                           the Window List) */
    while(pswitchlistCurrentSession)
        {
        ULONG   ulError=PMERR_HBITMAP_BUSY;
        SWCNTRL swcntrlCurrentSession;

                                        /* If we should draw a hidden session's icon, just ignore
                                           the request, as hiding one icon cause all icons to redraw,
                                           so this icon's position will be covered by a different
                                           icon immediately */
        if(pswitchlistCurrentSession->ulStatusFlag & SESSIONHIDE)
            break;
                                        /* If session's icon should be painted due to an invalidated client
                                           or due to reordering paint it in any case.
                                           Always blit from memory bitmap */
        if((hpsDraw!=pHP->hpsSession) ||
            (pswitchlistCurrentSession->ulSession!=((ULONG)mp2)+1))
            pswitchlistCurrentSession->ulStatusFlag&=(~SESSIONPAINTED);
        WinQuerySwitchEntry(pswitchlistCurrentSession->hswitchSession, &swcntrlCurrentSession);
                                        /* Avoid unnecessary repainting requests */
        if(pswitchlistCurrentSession->ulStatusFlag & SESSIONPAINTED)
            break;
                                        /* The session's icon we draw is either the active one or not */
        if(pswitchlistCurrentSession->ulStatusFlag & SESSIONACTIVE)
            {
#ifdef  DEBUG_SB
            printf("SessionBar: Activating %s\n", swcntrlCurrentSession.szSwtitle);
#endif  /* DEBUG_SB */
            WinDrawBorder(hpsDraw, &rectlCurrentSession, ulFrameSize, ulFrameSize, 0, 0, DB_DEPRESSED);
            }
        else
            {
#ifdef  DEBUG_SB
            printf("SessionBar: Deactivating %s\n", swcntrlCurrentSession.szSwtitle);
#endif  /* DEBUG_SB */
            WinDrawBorder(hpsDraw, &rectlCurrentSession, ulFrameSize, ulFrameSize, 0, 0, DB_RAISED);
            }
                                        /* We have painted the session's icon */
        pswitchlistCurrentSession->ulStatusFlag|=SESSIONPAINTED;
                                        /* If we're the active window and we've already drawn the icon at the
                                           current position, don't draw unless the client was invalidated.
                                           Always blit from memory bitmap */
        if((pswitchlistCurrentSession->ulSession==((ULONG)mp2)+1) &&
            (hpsDraw==pHP->hpsSession) &&
            (ulForceDraw==FALSE))
            break;
                                        /* Save our (1-based) index for this session */
        pswitchlistCurrentSession->ulSession=((ULONG)mp2)+1;
                                        /* We have to update the session's icon */
        rectlIcon.xLeft+=ulFrameSize;
        rectlIcon.yBottom+=ulFrameSize;
        rectlIcon.xRight-=ulFrameSize;
        rectlIcon.yTop-=ulFrameSize;
                                        /* Clear previous session's icon and blit current session's one */
        ptlLL.x=rectlIcon.xLeft; ptlLL.y=rectlIcon.yBottom;
        ptlUR.x=rectlIcon.xRight-1; ptlUR.y=rectlIcon.yTop-1;
        GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorBgnd);
        GpiMove(hpsDraw, &ptlLL);
        GpiBox(hpsDraw, DRO_OUTLINEFILL, &ptlUR, 0, 0);
        hpointerFrame=(HPOINTER)WinSendMsg(swcntrlCurrentSession.hwnd, WM_QUERYICON, NULL, NULL);
        if(hpointerFrame!=NULLHANDLE)
            ulError=BlitIcon(hpsDraw, hpointerFrame, &rectlIcon);
        if(ulError)
            BlitIcon(hpsDraw, hpointerReplacement, &rectlIcon);
#ifndef SLOWVIDEO
        WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory,
            &rectlCurrentSession, (POINTL *)&rectlCurrentSession, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
                                        /* Don't forget to exit loop */
        break;
        }
    return((MRESULT)TRUE);
    }

/*                                                                                      *\
 * Syntax: WM_HITTESTSESSIONICONS, HPS hpsDraw, ULONG ulMouseEvent                      *
\*                                                                                      */
case WM_HITTESTSESSIONICONS:
/*                                                                                      *\
 * Test which Session's icon is below the mouse pointer. The second parameter is either *
 * WM_NULL, when the mouse is just moved, or WM_BUTTON1DOWN & WM_BUTTON1UP or           *
 * WM_MOUSEMOVE, when a mouse event occured. The scroll buttons are also taken into the *
 * calculations.                                                                        *
 * The ulMouseEvent may also be WM_BUTTON2DOWN and WM_BUTTON2UP, in that case we        *
 * display a popup menu that lets the user close the window represented by the icon, or *
 * the application the represented window belongs to.                                   *
\*                                                                                      */
    {
                                        /* Save the current and new state of the scroll buttons */
    static ULONG    ulScrollLeftStateNew=WM_BUTTON1UP;
    static ULONG    ulScrollLeftState=WM_NULL;
    static ULONG    ulScrollRightStateNew=WM_BUTTON1UP;
    static ULONG    ulScrollRightState=WM_NULL;
                                        /* Save the previous button and the current mouse click state */
    static ULONG    ulSessionButtonStateNew=WM_BUTTON1UP;
    static ULONG    ulSessionButtonState=WM_BUTTON1UP;
    static ULONG    ulSessionIconsCountPrevious=0;
                                        /* By default let's assume that there are less icons in the
                                           SessionBar than the number of icons that would fit */
    ULONG           ulScrollLeftDisabled=TRUE;
    ULONG           ulScrollRightDisabled=TRUE;
                                        /* Save the previous ordinally first session's icon that was drawn
                                           into the SessionBar */
    ULONG           ulSessionIconsFirstPrevious=ulSessionIconsFirst;
                                        /* Ensure that the mouse not only hits a rectangle where possibly
                                           an icon could be drawn, but where the rectangle really covers
                                           an icon */
    ULONG           ulSessionIconsHittest;
                                        /* Some data to work with */
    SWITCHLIST      *pswitchlistCurrentSession=NULL;
    SWITCHLIST      *pswitchlistShift=NULL;
    RECTL           rectlCurrentSession;
    ULONG           ulMouseOverLeftScrollButton;
    ULONG           ulMouseOverRightScrollButton;
    ULONG           ulMouseOverIcon;
    HPS             hpsDraw=(HPS)LONGFROMMP(mp1);

    if((LONGFROMMP(mp2)!=WM_BUTTON2UP) && (LONGFROMMP(mp2)!=WM_BUTTON2DOWN))
        {
/*                                                                                      *\
 * Draw scroll buttons according to mouse pointer position and mouse button 1 state.    *
\*                                                                                      */
                                        /* Check if the scroll buttons may need to be redrawn
                                           depending on the current number of sessions available
                                           compared to the number of session available previously */
        if(ulSessionIconsCountPrevious!=ulSessionIconsCount)
            {
                                        /* If there are more sessions than we have space for their
                                           icons, assume that the scoll buttons need to be redrawn
                                           anyway */
            if(ulSessionIconsCount>=ulSessionIconsDrawn)
                ulRedrawScrollButtons=TRUE;
            ulSessionIconsCountPrevious=ulSessionIconsCount;
            }
                                        /* Process the left scroll button first. Process the mouse event
                                           and the effects on the scroll button */
        ulMouseOverLeftScrollButton=(ULONG)WinPtInRect(pHP->habPc2, prectlScrollLeft, &pointlMouse);
        if(ulMouseOverLeftScrollButton==FALSE)
            ulScrollLeftStateNew=WM_BUTTON1UP;
        else
            if(LONGFROMMP(mp2)!=WM_MOUSEMOVE)
                ulScrollLeftStateNew=LONGFROMMP(mp2);
                                        /* Blit the scroll button only if changed (or repaint
                                           is required). The button will be disabled when there
                                           is nothing to scroll left in the SessionBar, and the
                                           speaker will beep. Always blit from memory bitmap */
        if((ulScrollLeftState!=ulScrollLeftStateNew) ||
            (ulRedrawScrollButtons==TRUE))
            {
            RECTL   rectlScrollLeftBitmap=*prectlScrollLeft;
            ULONG   ulShift=0;
            ULONG   ulScrollShift=0;

                                        /* See if we can scroll leftwards and find last not hidden
                                           session */
            if(ulSessionIconsFirst>0)
                {
                for(ulScrollShift=ulSessionIconsFirst, pswitchlistShift=pswitchlistAnchor;
                    ulScrollShift>0;
                    ulScrollShift--, pswitchlistShift=pswitchlistShift->pNext)
                    ; /* NOP */
                ulScrollShift=ulSessionIconsFirst;
                while((pswitchlistShift!=NULL) && ((pswitchlistShift=pswitchlistShift->pPrevious)!=NULL))
                    {
                    ulScrollShift--;
                    if(!(pswitchlistShift->ulStatusFlag & SESSIONHIDE))
                        {
                        if((ulScrollLeftState==WM_BUTTON1DOWN) &&
                            (ulScrollLeftStateNew==WM_BUTTON1UP))
                            {
                            ulSessionIconsFirst=ulScrollShift;
                            while((pswitchlistShift=pswitchlistShift->pPrevious)!=NULL)
                                {
                                if(!(pswitchlistShift->ulStatusFlag & SESSIONHIDE))
                                    break;
                                ulScrollShift--;
                                }
                            }
                        else
                            ulScrollShift=ulSessionIconsFirst;
                        break;
                        }
                    }
                }
                                        /* If we couldn't scroll or can no further scroll leftwards,
                                           disable the scroll left button, be sure to take hidden
                                           session icons into account */
              if((ulSessionIconsFirst>0) &&
                (ulScrollShift>0))
                ulScrollLeftDisabled=FALSE;
            ulScrollLeftState=ulScrollLeftStateNew;
            if(ulScrollLeftState==WM_BUTTON1DOWN)
                ulShift=1;
            rectlScrollLeftBitmap.xLeft+=(ulBorderSize+ulShift);
            rectlScrollLeftBitmap.yBottom+=(ulBorderSize-ulShift);
            rectlScrollLeftBitmap.xRight-=(ulBorderSize-ulShift);
            rectlScrollLeftBitmap.yTop-=(ulBorderSize+ulShift);
            BlitBitmap(hwnd, hpsDraw, hbitmapScrollLeft, &rectlScrollLeftBitmap);
            WinDrawBorder(hpsDraw, prectlScrollLeft, ulBorderSize, ulBorderSize, 0, 0,
                ((ulScrollLeftState==WM_BUTTON1DOWN) ? DB_DEPRESSED : DB_RAISED));
            if(ulScrollLeftDisabled==TRUE)
                DisableWindowRect(hpsDraw, prectlScrollLeft, SYSCLR_DIALOGBACKGROUND);
#ifndef SLOWVIDEO
            WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory,
                prectlScrollLeft, (POINTL *)prectlScrollLeft, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
            }
        if(ulMouseOverLeftScrollButton==TRUE)
            {
            if((LONGFROMMP(mp2)==WM_BUTTON1DOWN) && (ulScrollLeftDisabled==TRUE))
                WinAlarm(HWND_DESKTOP, WA_WARNING);
            }
                                        /* Same for right scroll button */
        ulMouseOverRightScrollButton=(ULONG)WinPtInRect(pHP->habPc2, prectlScrollRight, &pointlMouse);
                                        /* Changed to allow right scroll button to paint
                                           depressed (and disabled) while still having
                                           not filled up the SessionBar completely
                                           if((ulMouseOverRightScrollButton==FALSE) || 
                                               (ulSessionIconsCount<=ulSessionIconsDrawn))
                                           */
        if(ulMouseOverRightScrollButton==FALSE)
            ulScrollRightStateNew=WM_BUTTON1UP;
        else
            if(LONGFROMMP(mp2)!=WM_MOUSEMOVE)
                ulScrollRightStateNew=LONGFROMMP(mp2);
        if((ulScrollRightState!=ulScrollRightStateNew) ||
            (ulRedrawScrollButtons==TRUE))
            {
            RECTL       rectlScrollRightBitmap=*prectlScrollRight;
            ULONG       ulShift=0;
            ULONG       ulSessionIcon;
            ULONG       ulSessionIconsLast;
            ULONG       ulSessionIconsLeft;

            ulSessionIconsLast=ulSessionIconsFirst+ulSessionIconsDrawn;
            if((ulSessionIconsCount>ulSessionIconsDrawn) && (ulSessionIconsLast<ulSessionIconsCount))
                ulScrollRightDisabled=FALSE;

                                        /* If there are hidden icons, exclude them from the number
                                           of icons left available to scroll right, be sure not to
                                           count hidden session icons */
            pswitchlistCurrentSession=pswitchlistAnchor;
            ulSessionIcon=0;
            ulSessionIconsLeft=0;
            while(pswitchlistCurrentSession!=NULL)
                {
                if((ulSessionIcon>=ulSessionIconsFirst) &&
                    !(pswitchlistCurrentSession->ulStatusFlag & SESSIONHIDE))
                    ulSessionIconsLeft++;
                ulSessionIcon++;
                pswitchlistCurrentSession=pswitchlistCurrentSession->pNext;
                }
            if((ulSessionIconsFirst+ulSessionIconsLeft)<=ulSessionIconsLast)
                ulScrollRightDisabled=TRUE;
            else if((ulScrollRightState==WM_BUTTON1DOWN) &&
                (ulScrollRightStateNew==WM_BUTTON1UP) &&
                (ulSessionIconsLast<ulSessionIconsCount))
                {
                for(ulShift=ulSessionIconsFirst, pswitchlistShift=pswitchlistAnchor;
                    ulShift>0;
                    ulShift--, pswitchlistShift=pswitchlistShift->pNext)
                    ; /* NOP */
                while(pswitchlistShift)
                    {
                    ulSessionIconsFirst++;
                    if(!(pswitchlistShift->pNext->ulStatusFlag & SESSIONHIDE))
                        break;
                    pswitchlistShift=pswitchlistShift->pNext;
                    }
                }
            ulScrollRightState=ulScrollRightStateNew;
            if(ulScrollRightState==WM_BUTTON1DOWN)
                ulShift=1;
            rectlScrollRightBitmap.xLeft+=(ulBorderSize+ulShift);
            rectlScrollRightBitmap.yBottom+=(ulBorderSize-ulShift);
            rectlScrollRightBitmap.xRight-=(ulBorderSize-ulShift);
            rectlScrollRightBitmap.yTop-=(ulBorderSize+ulShift);
            BlitBitmap(hwnd, hpsDraw, hbitmapScrollRight, &rectlScrollRightBitmap);
            WinDrawBorder(hpsDraw, prectlScrollRight, ulBorderSize, ulBorderSize, 0, 0,
                ((ulScrollRightState==WM_BUTTON1DOWN) ? DB_DEPRESSED : DB_RAISED));
            if(ulScrollRightDisabled==TRUE)
                DisableWindowRect(hpsDraw, prectlScrollRight, SYSCLR_DIALOGBACKGROUND);
#ifndef SLOWVIDEO
            WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory,
                prectlScrollRight, (POINTL *)prectlScrollRight, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
            }
        if(ulMouseOverRightScrollButton==TRUE)
            {
            if((LONGFROMMP(mp2)==WM_BUTTON1DOWN) && (ulScrollRightDisabled==TRUE))
                WinAlarm(HWND_DESKTOP, WA_WARNING);
            }
        ulRedrawScrollButtons=FALSE;
                                        /* If the user has scrolled the icons in the ScrollBar,
                                           then we have to redraw the icons and to save the new
                                           ordinally first session's icon */
        if(ulSessionIconsFirstPrevious!=ulSessionIconsFirst)
            {
            ulSessionIconsFirstPrevious=ulSessionIconsFirst;
            WinInvalidateRect(pHP->hwndSessionClient, prectlSessionIcons, FALSE);
            WinInvalidateRect(pHP->hwndSessionClient, prectlScrollLeft, FALSE);
            WinInvalidateRect(pHP->hwndSessionClient, prectlScrollRight, FALSE);
            }
        else if((ulMouseOverLeftScrollButton==TRUE) || (ulMouseOverRightScrollButton==TRUE))
            return((MRESULT)FALSE);
        }
/*                                                                                      *\
 * Find the Session's systemmenu icon the mouse pointer is positioned on. If mouse      *
 * button 1 is depressed also request the session corresponding to the icon to be       *
 * switched to by the working thread.                                                   *
\*                                                                                      */
                                        /* Assume mouse pointer is not over a session's icon
                                           unless proven otherwise */
    pswitchlistSelected=NULL;
                                        /* See if anything changed so we avoid unnecessary painting */
    if(LONGFROMMP(mp2)!=WM_MOUSEMOVE)
        ulSessionButtonStateNew=LONGFROMMP(mp2);
                                        /* Start with 1st session */
    pswitchlistCurrentSession=pswitchlistAnchor;
    if(pswitchlistCurrentSession==NULL)
        return((MRESULT)FALSE);
                                        /* Start with 1st session's icon to find which icon is below the
                                           mouse pointer */
    rectlCurrentSession=*prectlSessionIcons;
    rectlCurrentSession.xRight=rectlCurrentSession.xLeft+2*ulFrameSize+ulIconSize;
                                        /* Find the icon below the current mouse pointer */
    ulSessionIconsHittest=0;
    do  {
        ulMouseOverIcon=(ULONG)WinPtInRect(pHP->habPc2, &rectlCurrentSession, &pointlMouse);
                                       /* Skip session icons that are hidden in SessionBar */
        if((ulSessionIconsHittest>=ulSessionIconsFirst) &&
            (pswitchlistCurrentSession->ulStatusFlag & SESSIONHIDE))
            {
            pswitchlistCurrentSession=pswitchlistCurrentSession->pNext;
            continue;
            }
                                        /* If the mouse pointer is above an session's icon, then
                                           load the current SessionBar's session name with the
                                           session's Window List entry. Be careful as the ordinal n-th rectangle
                                           can be related to the m-th session, as the SessionBar displays
                                           only as many icons as fit into */
        if((ulMouseOverIcon==TRUE) && (ulSessionIconsHittest>=ulSessionIconsFirst))
            {
            SWCNTRL swcntrlCurrentSession;
            UCHAR   *pucCurrentSwTitle;

            WinQuerySwitchEntry(pswitchlistCurrentSession->hswitchSession, &swcntrlCurrentSession);
            pucCurrentSwTitle=swcntrlCurrentSession.szSwtitle;
            rectlHotspot=rectlCurrentSession;
            WinSendMsg(hwnd, WM_DRAWSESSIONTEXT, MPFROMP(hpsDraw), MPFROMP(pucCurrentSwTitle));
            if(LONGFROMMP(mp2)==WM_BUTTON2UP)
                {
                POINTL  pointlPopupMenu;

                                        /* If mouse button 2 was pressed, display the SessionBar popup menu,
                                           where the user can decide to close the window or application */
                pswitchlistSelected=pswitchlistCurrentSession;
                pswitchlistSelected->hwndIcon=swcntrlCurrentSession.hwnd;
                pointlPopupMenu=pointlMouse;
                WinMapWindowPoints(hwnd, HWND_DESKTOP, &pointlPopupMenu, 1);
                WinPopupMenu(HWND_DESKTOP, hwnd, pHP->hwndPopupMenuSessionBar,
                    pointlPopupMenu.x, pointlPopupMenu.y, ID_ICONEXIT,
                    PU_POSITIONONITEM | PU_HCONSTRAIN | PU_VCONSTRAIN | PU_KEYBOARD |
                    PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2);
                }
            else if((ulSessionButtonState==WM_BUTTON1DOWN) && (ulSessionButtonStateNew==WM_BUTTON1UP))
                {
                                        /* If mouse button 1 was depressed, switch to the selected
                                           session, via request to working thread and save button state */
                WinPostMsg(pHP->hwndWorkingThread, WM_SESSIONBAR,
                    MPFROMLONG(SESSIONBAR_SWITCH), MPFROMHWND(swcntrlCurrentSession.hwnd));
                ulSessionButtonState=ulSessionButtonStateNew;
                }
            ulSessionButtonState=ulSessionButtonStateNew;
            return((MRESULT)TRUE);
            }
                                        /* Advance to next icon */
        pswitchlistCurrentSession=pswitchlistCurrentSession->pNext;
                                        /* Advance to next icon if the current session is one
                                           that is currently visible on the SessionBar */
        if(ulSessionIconsHittest>=ulSessionIconsFirst)
            {
            rectlCurrentSession.xLeft+=2*ulFrameSize+ulIconSize+ulSpaceSize;
            rectlCurrentSession.xRight=rectlCurrentSession.xLeft+2*ulFrameSize+ulIconSize;
            }
                                        /* Skip testing for icons outside the ones contained
                                           in the SessionBar */
        if(++ulSessionIconsHittest>=(ulSessionIconsFirst+ulSessionIconsDrawn))
            break;
        } while(pswitchlistCurrentSession);
                                        /* Save button state */
    ulSessionButtonState=ulSessionButtonStateNew;
    return((MRESULT)FALSE);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_HITTESTCPUSTATUS, HPS hpsDraw, ULONG ulMouseEvent                         *
\*                                                                                      */
case WM_HITTESTCPUSTATUS:
/*                                                                                      *\
 * Test if the CPU Performance area is below the mouse pointer. The second parameter is *
 * either WM_NULL, when the mouse is just moved, or WM_BUTTON1DOWN & WM_BUTTON1UP or    *
 * WM_MOUSEMOVE, when a mouse event occured.                                            *
 * The ulMouseEvent may also be WM_BUTTON2DOWN and WM_BUTTON2UP, in that case we        *
 * display a popup menu that lets the user toggle CPUs to on/offline or to select the   *
 * report intervall (out of the available choices).                                     *
\*                                                                                      */
    {
    ULONG           ulMouseInRectangle;
    RECTL           rectlCPUXStatus=*prectlCPUStatus;

    ulMouseInRectangle=(ULONG)WinPtInRect(pHP->habPc2, prectlCPUStatus, &pointlMouse);
    if(ulMouseInRectangle==FALSE)
        return((MRESULT)FALSE);
    if(LONGFROMMP(mp2)==WM_BUTTON2UP)
        {
        POINTL  pointlPopupMenu;

        pointlPopupMenu=pointlMouse;
        WinMapWindowPoints(hwnd, HWND_DESKTOP, &pointlPopupMenu, 1);
        WinPopupMenu(HWND_DESKTOP, hwnd, pHP->hwndPopupMenuProcessor,
            pointlPopupMenu.x, pointlPopupMenu.y, ID_SBPROCESSORMENU+pHP->ulNumProcessors,
            PU_POSITIONONITEM | PU_HCONSTRAIN | PU_VCONSTRAIN | PU_KEYBOARD |
            PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2);
        }
    else if(LONGFROMMP(mp2)==WM_MOUSEMOVE)
        {
                                        /* Skip 3D border for our calculations here */
        rectlCPUXStatus.yBottom+=ulBorderSize;
        rectlCPUXStatus.yTop-=ulBorderSize;
                                        /* For each processor we draw into a separate rectangle
                                           which is basically a grid of horizontal load percentage
                                           lines and vertical time intervalls. When the WM_DRAW
                                           event is received, the window is scrolled to the left
                                           one pixel and the current CPU Performance is drawn
                                           additionally */
        for(ulNumProcessor=1; ulNumProcessor<=pHP->ulNumProcessors; ulNumProcessor++)
            {
                                        /* Calculate the current CPU's rectangle */
            rectlCPUXStatus.xLeft=rectlCPUXStatus.xRight=
                prectlCPUStatus->xLeft+ulBorderSize+
                (ulNumProcessor-1)*(ulCPUProcessorPixels+SESSIONBAR_CPUSPACE);
            rectlCPUXStatus.xRight+=(ulCPUProcessorPixels-1);
                                        /* If mouse is over the CPU's rectangle, then display current
                                           CPU status in Session Name field */
            if(WinPtInRect(pHP->habPc2, &rectlCPUXStatus, &pointlMouse))
                {
                sprintf(ucStatusText, "CPU %ld (", ulNumProcessor);
                if(pHP->ulPerformanceFlag & PERFORMANCE_60S)
                    strcat(ucStatusText, "1");
                else if(pHP->ulPerformanceFlag & PERFORMANCE_5M)
                    strcat(ucStatusText, "5");
                else if(pHP->ulPerformanceFlag & PERFORMANCE_30M)
                    strcat(ucStatusText, "30");
                else if(pHP->ulPerformanceFlag & PERFORMANCE_1H)
                    strcat(ucStatusText, "60");
                    strcat(ucStatusText, " minute(s))");
                rectlHotspot=rectlCPUXStatus;
                WinSendMsg(hwnd, WM_DRAWSESSIONTEXT, mp1, MPFROMP(ucStatusText));
                }
            }
        }
    return((MRESULT)TRUE);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_CALCULATECPUSTATUS, ULONG ulEvent, ULONG  ulProcessorMask                 *
\*                                                                                      */
case WM_CALCULATECPUSTATUS:
/*                                                                                      *\
 * Calculate the current CPU Performance and add it to the history of CPU performance   *
 * values. If ulProcessorMask is not 0, then we clear just the CPU's history data which *
 * is currently offline, otherwise we clear all CPUs history data.                      *
\*                                                                                      */
    {
    static CPUUTIL  *pcpuutilCurrent=NULL;
    static CPUUTIL  *pcpuutilPrevious=NULL;
    static CPUTICKS *pcputicksCurrent=NULL;
    static CPUTICKS *pcputicksPrevious=NULL;
    ULONG           ulProcessor;
    CPUPERFORMANCE  *pCPUPerformanceStackCpu;
    double          dTimerDelta;
    double          dConvert;

                                        /* Allocate and (re)initialize CPU Performance values history */
    if(LONGFROMMP(mp1)==WM_CREATE)
        {
        if(pCPUPerformanceStack==NULL)
            {
                                        /* The CPU Performance history values is an array of CPUPERFORMANCE
                                           structures that is organized as:

                                           History value          ! CPU 0 ! CPU 1 ! ...   ! CPU n
                                           -----------------------!-------!-------!-------!------
                                           0                      !       !       !       !
                                           -----------------------!-------!-------!-------!------
                                           1                      !       !       !       !
                                           -----------------------!-------!-------!-------!------
                                           ...                    !       !       !       !
                                           -----------------------!-------!-------!-------!------
                                           ulCPUProcessorPixels-1 !       !       !       !
                                           -----------------------!-------!-------!-------!------

                                           For each timer tick, all values 1...ulCPUProcessorPixels-2 for
                                           CPU n will be moved to 0, and ulCPUProcessorPixels-1 will be
                                           filled with the current calculated values */
            pCPUPerformanceStack=malloc(sizeof(CPUPERFORMANCE)*pHP->ulNumProcessors*ulCPUProcessorPixels);
            pcpuutilCurrent=malloc(sizeof(CPUUTIL)*pHP->ulNumProcessors);
            pcpuutilPrevious=malloc(sizeof(CPUUTIL)*pHP->ulNumProcessors);
            pcputicksCurrent=malloc(sizeof(CPUTICKS)*pHP->ulNumProcessors);
            pcputicksPrevious=malloc(sizeof(CPUTICKS)*pHP->ulNumProcessors);
                                        /* When being called the first time, ensure that history data
                                           gets initialized */
            mp2=NULL;
            }
                                        /* Either clear all history data or just the one of the
                                           CPUs being offline. If 0 is passed, then this works as
                                           if no CPU is being online (which of course can't happen,
                                           as CPU 1 is always online) */
        for(ulNumProcessor=1; ulNumProcessor<=pHP->ulNumProcessors; ulNumProcessor++)
            {
            ulProcessor=ulNumProcessor-1;
            if((LONGFROMMP(mp2) & (0x00000001<<ulProcessor))==0)
                {
                for(ulTemp=0; ulTemp<ulCPUProcessorPixels; ulTemp++)
                    memset(&pCPUPerformanceStack[ulProcessor+(pHP->ulNumProcessors*(ulTemp))],
                        0xFF,
                        sizeof(CPUPERFORMANCE));
                }
            }
                                        /* Initialize the "previous" CPU Performance data, so that for
                                           the next timer tick some changes to the data can be detected */
        memset(pcpuutilCurrent, 0xFF, sizeof(CPUUTIL)*pHP->ulNumProcessors);
        memset(pcputicksCurrent, 0xFF, sizeof(CPUTICKS)*pHP->ulNumProcessors);
        memset(pcpuutilPrevious, 0xFF, sizeof(CPUUTIL)*pHP->ulNumProcessors);
        memset(pcputicksPrevious, 0xFF, sizeof(CPUTICKS)*pHP->ulNumProcessors);
        pHP->DosPerfSysCall(CMD_KI_KLRDCNT, (ULONG)pcpuutilPrevious, 0, 0);
        for(ulNumProcessor=1; ulNumProcessor<=pHP->ulNumProcessors; ulNumProcessor++)
            {
            ulProcessor=ulNumProcessor-1;
            pcputicksPrevious[ulProcessor].dTotal=LL2F(pcpuutilPrevious[ulProcessor].ulTotalHigh,
                pcpuutilPrevious[ulProcessor].ulTotalLow);
            pcputicksPrevious[ulProcessor].dIdle=LL2F(pcpuutilPrevious[ulProcessor].ulIdleHigh,
                pcpuutilPrevious[ulProcessor].ulIdleLow);
            pcputicksPrevious[ulProcessor].dBusy=LL2F(pcpuutilPrevious[ulProcessor].ulBusyHigh,
                pcpuutilPrevious[ulProcessor].ulBusyLow);
            pcputicksPrevious[ulProcessor].dIntr=LL2F(pcpuutilPrevious[ulProcessor].ulIntrHigh,
                pcpuutilPrevious[ulProcessor].ulIntrLow);
            }
        }
                                        /* Free allocation */
    else if(LONGFROMMP(mp1)==WM_DESTROY)
        {
        if(pCPUPerformanceStack)
            {
            free(pCPUPerformanceStack);
            free(pcpuutilCurrent);
            free(pcpuutilPrevious);
            free(pcputicksCurrent);
            free(pcputicksPrevious);
            }
        pCPUPerformanceStack=NULL;
        pcpuutilCurrent=pcpuutilPrevious=NULL;
        pcputicksCurrent=pcputicksPrevious=NULL;
        }
                                        /* Scroll CPU Performance history values to history and
                                           add the current one */
    else
        {
                                        /* Scroll history values backwards to make place for
                                           the new current values */
        for(ulTemp=0; ulTemp<(ulCPUProcessorPixels-1); ulTemp++)
            {
            memcpy(&pCPUPerformanceStack[(pHP->ulNumProcessors*(ulTemp))],
                &pCPUPerformanceStack[(pHP->ulNumProcessors*(ulTemp+1))],
                sizeof(CPUPERFORMANCE)*pHP->ulNumProcessors);
            }
                                        /* Query all processors CPU Performance. However, if a CPU is
                                           offline, don't calculate any Performance data, but leave the
                                           history values of 0xFF, which when the CPU gets turned online
                                           again prevents any history data to displayed in the Performance
                                           graphics */
        pHP->DosPerfSysCall(CMD_KI_KLRDCNT, (ULONG)pcpuutilCurrent, 0, 0);
        for(ulNumProcessor=1; ulNumProcessor<=pHP->ulNumProcessors; ulNumProcessor++)
            {
            if((pHP->ulProcessorStatus & (0x00000001<<(ulNumProcessor-1)))==0)
                continue;
                                        /* Calculate current timer ticks */
            ulProcessor=ulNumProcessor-1;
            pcputicksCurrent[ulProcessor].dTotal=LL2F(pcpuutilCurrent[ulProcessor].ulTotalHigh,
                pcpuutilCurrent[ulProcessor].ulTotalLow);
            pcputicksCurrent[ulProcessor].dIdle=LL2F(pcpuutilCurrent[ulProcessor].ulIdleHigh,
                pcpuutilCurrent[ulProcessor].ulIdleLow);
            pcputicksCurrent[ulProcessor].dBusy=LL2F(pcpuutilCurrent[ulProcessor].ulBusyHigh,
                pcpuutilCurrent[ulProcessor].ulBusyLow);
            pcputicksCurrent[ulProcessor].dIntr=LL2F(pcpuutilCurrent[ulProcessor].ulIntrHigh,
                pcpuutilCurrent[ulProcessor].ulIntrLow);
            dTimerDelta=pcputicksCurrent[ulProcessor].dTotal-
                pcputicksPrevious[ulProcessor].dTotal;
                                        /* Address the last row of the history values for the
                                           current CPU */
            pCPUPerformanceStackCpu=&pCPUPerformanceStack[ulProcessor+
                (pHP->ulNumProcessors*(ulCPUProcessorPixels-1))];
                                        /* Calculate the percentages the tasks consumed the CPU
                                           between the previous and current timer ticks */
            pCPUPerformanceStackCpu->ucIdle=dConvert=
                ((pcputicksCurrent[ulProcessor].dIdle-pcputicksPrevious[ulProcessor].dIdle)/dTimerDelta*100.0);
            if(dConvert-(double)(ULONG)dConvert>=0.5)
                pCPUPerformanceStackCpu->ucIdle++;
            pCPUPerformanceStackCpu->ucBusy=dConvert=
                ((pcputicksCurrent[ulProcessor].dBusy-pcputicksPrevious[ulProcessor].dBusy)/dTimerDelta*100.0);
            if(dConvert-(double)(ULONG)dConvert>=0.5)
                pCPUPerformanceStackCpu->ucBusy++;
                                        /* Commented out, because this calculation may result in <100% total
                                           due to the rounding differences, e.g. 19.4+29.4+50.2 = 98 (as
                                           19+29+50 is summed)
            pCPUPerformanceStackCpu->ucIntr=(UCHAR)dConvert=
                ((pcputicksCurrent[ulProcessor].dIntr-pcputicksPrevious[ulProcessor].dIntr)/dTimerDelta*100.0);
            if(dConvert-(double)(ULONG)dConvert>=0.5)
                pCPUPerformanceStackCpu->ucIntr++;
                                         */
            pCPUPerformanceStackCpu->ucIntr=
                100-(pCPUPerformanceStackCpu->ucIdle+pCPUPerformanceStackCpu->ucBusy);
                                        /* I'm not sure if this could happen */
#ifdef  DEBUG
            if((pCPUPerformanceStackCpu->ucIdle+
                pCPUPerformanceStackCpu->ucBusy+
                pCPUPerformanceStackCpu->ucIntr)>100)
                printf("SB: CPU time >100%\n");
#endif  /* DEBUG */
            if(pCPUPerformanceStackCpu->ucIntr>100)
                {
#ifdef  DEBUG
                printf("SB: CPU interrupt time >100%\n");
#endif  /* DEBUG */
                pCPUPerformanceStackCpu->ucIntr=100;
                }
                                        /* Shift current performance timer ticks to previous one */
            pcputicksPrevious[ulProcessor]=pcputicksCurrent[ulProcessor];
            }
        }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_DRAWCPUSTATUS, HPS hpsDraw, ULONG ulMouseEvent                            *
\*                                                                                      */
case WM_DRAWCPUSTATUS:
/*                                                                                      *\
 * Draw the SessionBar's CPU Performance status area and its frame. Per processor the   *
 * history of the CPU Performance items interrupt and idle time will be drawn together  *
 * with a raster for load percentages and time intervalls.                              *
\*                                                                                      */
    {
                                        /* Cache parameters */
    HPS             hpsDraw=(HPS)LONGFROMMP(mp1);
    ULONG           ulMouseEvent=LONGFROMMP(mp2);
                                        /* Required to count <1s ticks, as the timer has only
                                           1s granularity */
    static ULONG    ulScrollTicks=0;
                                        /* Ensure that we draw vertical timeline only once in
                                           one second (so that is just 1 pixel wide) */
    static ULONG    ulLastTime=(ULONG)-1;
    ULONG           ulDrawTimeline=FALSE;
                                        /* Rectangle of 1 CPU's graphics area */
    RECTL           rectlCPUXStatus=*prectlCPUStatus;
                                        /* Rectangle of border between 2 CPUs */
    RECTL           rectlCPUBorder;
                                        /* Working points */
    POINTL          pointlLL, pointlUR;
                                        /* CPU Performance calculation data */
    ULONG           ulDelta;
    CPUPERFORMANCE  *pCPUPerformanceStackCpu;
    CPUPERFORMANCE  *pCPUPerformanceStackCpuPrevious;

                                        /* Wait for initialization, because only if the timer ticks are
                                           appearing stable, we can display somewhat stable data */
    if(ulMouseEvent==WM_CREATE)
        {
        ulCPUInitialized=TRUE;
        break;
        }
    if(ulCPUInitialized==FALSE)
        break;

#ifdef  DEBUG_SB
    printf("SB: WM_DRAWCPUSTATUS %s\n", (ulMouseEvent==WM_DRAW ? "Scroll" : "Repaint"));
#endif  /* DEBUG_SB */
                                        /* Skip 3D border for our calculations here */
    rectlCPUXStatus.yBottom+=ulBorderSize;
    rectlCPUXStatus.yTop-=ulBorderSize;
                                        /* Ensure that we get the current CPU Performance, but only
                                           if we were called by a timer tick (otherwise is may just
                                           be a repaint request, in which case we draw fron the saved
                                           data) */
    if(ulMouseEvent==WM_DRAW)
        {
        WinSendMsg(hwnd, WM_CALCULATECPUSTATUS, NULL, NULL);
                                        /* Ensure that the vertical timeline is only drawn
                                           once in a second (and there can be TICKSPERSECOND
                                           ticks in one second) */
        if(ulLastTime!=tmCPUStatus.tm_min*60+tmCPUStatus.tm_sec)
            {
            ulScrollTicks=0;
            ulDrawTimeline=TRUE;
            ulLastTime=tmCPUStatus.tm_min*60+tmCPUStatus.tm_sec;
            }
        }
                                        /* For each processor we draw into a separate rectangle
                                           which is basically a grid of horizontal load percentage
                                           lines and vertical time intervalls. When the WM_DRAW
                                           event is received, the window is scrolled to the left
                                           one pixel and the current CPU Performance is drawn
                                           additionally */
    for(ulNumProcessor=1; ulNumProcessor<=pHP->ulNumProcessors; ulNumProcessor++)
        {
                                        /* Calculate the current CPU's rectangle */
        rectlCPUXStatus.xLeft=rectlCPUXStatus.xRight=
            prectlCPUStatus->xLeft+ulBorderSize+
            (ulNumProcessor-1)*(ulCPUProcessorPixels+SESSIONBAR_CPUSPACE);
        rectlCPUXStatus.xRight+=(ulCPUProcessorPixels-1);
#ifndef SLOWVIDEO
                                        /* We have to "clear" the rectangle we draw the CPU Performance
                                           graphics into anyway, because for WM_DRAW (that is not repaint
                                           requests of WM_PAINT) we would otherwise draw on top of the
                                           previous drawing */
        pointlLL=*((POINTL *)(&rectlCPUXStatus.xLeft));
        pointlUR=*((POINTL *)(&rectlCPUXStatus.xRight));
        pointlUR.y--;
        GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorCPU);
        GpiMove(hpsDraw, &pointlLL);
        GpiBox(hpsDraw, DRO_OUTLINEFILL, &pointlUR, 0, 0);
#endif  /* SLOWVIDEO */
                                        /* For a processor that is offline, we just tell that the user */
        if((pHP->ulProcessorStatus & (0x00000001<<(ulNumProcessor-1)))==0)
            {
            WinDrawText(hpsDraw, -1, "Offline", &rectlCPUXStatus, pHP->ulSessionBarRGBColorFgnd, 0,
                DT_CENTER|DT_VCENTER);
            continue;
            }
                                        /* During a WM_PAINT processing, we also have to draw the border
                                           between 2 processor's graphics */
        if((ulMouseEvent==WM_PAINT) && (ulNumProcessor<pHP->ulNumProcessors))
            {
                                        /* We draw the border between 2 CPUs only if there is still
                                           another CPU left (otherwise we'll. (For calculations we have
                                           to take into account that x+y gives y+1 points when counting
                                           x, x+1, x+2, ..., x+y, so usually we have to calculate x+y-1) */
            rectlCPUBorder=rectlCPUXStatus;
            rectlCPUBorder.xLeft=rectlCPUBorder.xRight+1;
            rectlCPUBorder.xRight+=SESSIONBAR_CPUSPACE+1;
            WinDrawBorder(hpsDraw, &rectlCPUBorder, (SESSIONBAR_CPUSPACE>>1), (SESSIONBAR_CPUSPACE>>1), 0, 0, DB_RAISED);
            }
                                        /* For a timer generated update, scroll leftwards and add the
                                           new value */
#ifdef  SLOWVIDEO
        if(ulMouseEvent==WM_DRAW)
#else
        if((ulMouseEvent==WM_DRAW) && (hpsDraw!=pHP->hpsSessionBarMemory))
#endif  /* SLOWVIDEO */
            {
                                        /* Just scroll the rectangle left by one pixel and then draw
                                           the current CPU Performance */
            rectlCPUXStatus.xLeft+=1;
            rectlCPUXStatus.xRight+=1;
            WinScrollWindow(pHP->hwndSessionClient, -1, 0, &rectlCPUXStatus, NULL, NULLHANDLE, NULL, 0);
            rectlCPUXStatus.xLeft-=1;
            rectlCPUXStatus.xRight-=1;
                                        /* We update just the last column. Therefore, we "clean" out last
                                           CPU Performance status, the grid and the current CPU Performance
                                           status will be added afterwards (downwards in code) */
            pointlLL=*((POINTL *)(&rectlCPUXStatus.xLeft));
            pointlUR=*((POINTL *)(&rectlCPUXStatus.xRight));
            pointlUR.y-=1;
                                        /* Restore to update area of rectangle */
            pointlLL.x=pointlUR.x;
            GpiMove(hpsDraw, &pointlLL);
            GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorCPU);
            GpiLine(hpsDraw, &pointlUR);
            }
                                        /* Now we draw the CPU Performance history from our saved values. For
                                           that we either start at the current CPU Performance if we just do
                                           a timer update, or from the first value in the CPU Performance
                                           history during a repaint. We do that only in "fill" mode however. */
        if(pHP->ulStatusFlag2 & PERFORMANCEFILLSB)
            {
#ifdef  SLOWVIDEO
            if(ulMouseEvent==WM_PAINT)
#else
            if((ulMouseEvent==WM_PAINT) || (hpsDraw==pHP->hpsSessionBarMemory))
#endif  /* SLOWVIDEO */
                {
                ulTemp=0;
                pointlUR.x=rectlCPUXStatus.xLeft;
                }
            else
                {
                ulTemp=ulCPUProcessorPixels-1;
                pointlUR.x=rectlCPUXStatus.xRight;
                }
            for( ; ulTemp<ulCPUProcessorPixels; ulTemp++, pointlLL.x++, pointlUR.x++)
                {
                pCPUPerformanceStackCpu=&pCPUPerformanceStack[(ulNumProcessor-1)+
                    (pHP->ulNumProcessors*(ulTemp))];
                                        /* First, we can only draw something, if at least the current value
                                           already is available */
                if(pCPUPerformanceStackCpu->ucIntr==(UCHAR)0xFF)
                    continue;
                                        /* Second, we address the last pixel column for the current
                                           processor's Interrupt value */
                pointlLL.x=pointlUR.x;
                pointlLL.y=rectlCPUXStatus.yBottom;
                pointlUR.y=rectlCPUXStatus.yBottom+((ulIconSize-1)*((LONG)pCPUPerformanceStackCpu->ucIntr)/100);
                                        /* Third, we draw a line between 0 and the Interrupt value */
                GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorIntr);
                GpiMove(hpsDraw, &pointlLL);
                GpiLine(hpsDraw, &pointlUR);
                GpiMove(hpsDraw, &pointlUR);
                                        /* Fourth, we draw from the Interrupt value to the Busy value */
                pointlUR.y+=((ulIconSize-1)*((LONG)pCPUPerformanceStackCpu->ucBusy)/100);
                GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorBusy);
                GpiLine(hpsDraw, &pointlUR);
                }
                                        /* Fifth point to the last column so that the grid can be added to
                                           it afterwards (downwards in code). If we do a repaint, than the
                                           following WM_PAINT processing will change away from the last
                                           column to the whole rectangle */
            pointlLL=*((POINTL *)(&rectlCPUXStatus.xLeft));
            pointlUR=*((POINTL *)(&rectlCPUXStatus.xRight));
            pointlUR.y-=1;
            pointlLL.x=pointlUR.x;
            }
                                        /* For a WM_PAINT repaint processing, we also have to draw the
                                           graphic's vertical lines representing the time checkpoints */
#ifdef  SLOWVIDEO
        if((ulMouseEvent==WM_PAINT) && !(pHP->ulStatusFlag2 & NOGRIDSB))
#else
        if(((ulMouseEvent==WM_PAINT) || (hpsDraw==pHP->hpsSessionBarMemory)) && !(pHP->ulStatusFlag2 & NOGRIDSB))
#endif  /* SLOWVIDEO */
            {
                                        /* We update the whole rectangle, thus we draw also the
                                           vertical time levels */
            pointlLL=*((POINTL *)(&rectlCPUXStatus.xLeft));
            pointlUR=*((POINTL *)(&rectlCPUXStatus.xRight));
            pointlUR.y-=1;
            GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorGrid);
                                        /* The vertical lines represent the following checkpoints:
                                           for 60S ... 10s, 20s, ..., 50s (1s/pixel, 6 checkpoints)
                                           for 5M .... 60s, 120s, ..., 240s (5s/pixel, 5 checkpoints)
                                           for 30M ... 5m, 10m, ..., 25m (30s/pixel, 6 checkpoints)
                                           for 60M ... 10m, 20m, ..., 50m (60s/pixel, 6 checkpoints)
                                           The logic takes the CPU Performance graphics area's size
                                           into account too, by adjusting the pixel offest of the first
                                           vertical line using the ulCPUTicksAdjust* factors, e.g. if
                                           the area is 120 pixel <=> 2*SESSIONBAR_CPUPIXELS and the
                                           reporting intervall is 30M, then instead of 30s/pixel we
                                           only have 15s/pixel */
            ulDelta=ulCPUProcessorPixels/6;
            if(pHP->ulPerformanceFlag & PERFORMANCE_60S)
                {
                ulTemp=ulDelta-(tmCPUStatus.tm_sec % 10)*ulCPUTicksAdjustMult/ulCPUTicksAdjustDiv;
                if(ulScrollTicks & 0x00000001)
                    ulTemp--;
                }
            else if(pHP->ulPerformanceFlag & PERFORMANCE_5M)
                {
                ulDelta=ulCPUProcessorPixels/5;
                ulTemp=(tmCPUStatus.tm_sec/5)*5;
                if(ulTemp==tmCPUStatus.tm_sec)
                    ulTemp=ulDelta-((ulTemp*ulCPUTicksAdjustMult/ulCPUTicksAdjustDiv) / 5);
                else
                    ulTemp=ulDelta-((ulTemp*ulCPUTicksAdjustMult/ulCPUTicksAdjustDiv) / 5)-1;
                ulTemp--;
                }
            else if(pHP->ulPerformanceFlag & PERFORMANCE_30M)
                {
                ulTemp=ulDelta-(((tmCPUStatus.tm_min*60+tmCPUStatus.tm_sec) % 300) /
                    (30*ulCPUTicksAdjustDiv/ulCPUTicksAdjustMult));
                }
            else if(pHP->ulPerformanceFlag & PERFORMANCE_1H)
                {
                ulTemp=ulDelta-(((tmCPUStatus.tm_min*60+tmCPUStatus.tm_sec) % 600) /
                    (60*ulCPUTicksAdjustDiv/ulCPUTicksAdjustMult));
                }
                                        /* Correct overrun, e.g. 10 - (50s % 10) => 10, which means that
                                           we are to draw a vertical line as we are at a second evenly
                                           divisible by our grid intervall of 10s. In general, this means
                                           that ulTemp=ulDelta then. We have done our calculations based
                                           on the number of ticks per second, which we now have to undo
                                           when correlating the ticks to pixel offsets */
            if(ulTemp==ulDelta)
                ulTemp=0;
            pointlLL.x=pointlUR.x=rectlCPUXStatus.xLeft+ulTemp;
            for( ; pointlUR.x<=rectlCPUXStatus.xRight; pointlLL.x=pointlUR.x+=ulDelta)
                {
                GpiMove(hpsDraw, &pointlLL);
                GpiLine(hpsDraw, &pointlUR);
                }
                                        /* Restore to full per processor rectangle */
            pointlLL=*((POINTL *)(&rectlCPUXStatus.xLeft));
            pointlUR=*((POINTL *)(&rectlCPUXStatus.xRight));
            }
                                        /* Now, we can draw the the horizontal lines representing the
                                           performance levels and then the current CPU Performance.
                                           We draw the horizontel lines over the whole rectangle during
                                           a WM_REPAINT and only for the current column during WM_PAINT,
                                           pointlLL and pointlUR have already been set accordingly */
                                        /* First we draw the CPU Performance percentage levels (of
                                           0, 25, 50, 75 and 100%) */
        GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorGrid);
        if(!(pHP->ulStatusFlag2 & NOGRIDSB))
            for(ulTemp=0; ulTemp<=100; /* NOP */)
            {
            pointlLL.y=pointlUR.y=rectlCPUXStatus.yBottom+((ulIconSize-1)*ulTemp/100);
            GpiMove(hpsDraw, &pointlLL);
            GpiLine(hpsDraw, &pointlUR);
            if(pHP->ulSessionBarScale>1)
                ulTemp+=50;
            else
                ulTemp+=25;
            }
#ifdef  SLOWVIDEO
                                        /* Second, if we're at an even fraction of our timeline (e.g. seconds
                                           0, 10, ..., 50 if we draw the last minute), then draw a vertical
                                           time level */
        ulTemp=0;
        if(pHP->ulPerformanceFlag & PERFORMANCE_60S)
            {
            if((tmCPUStatus.tm_sec % 10)==0)
                ulTemp=pHP->ulSessionBarRGBColorGrid;
            }
        else if(pHP->ulPerformanceFlag & PERFORMANCE_5M)
            {
            if(tmCPUStatus.tm_sec==0)
                ulTemp=pHP->ulSessionBarRGBColorGrid;
            }
        else if(pHP->ulPerformanceFlag & PERFORMANCE_30M)
            {
            if((tmCPUStatus.tm_min % 5)==0)
                ulTemp=pHP->ulSessionBarRGBColorGrid;
            }
        else if(pHP->ulPerformanceFlag & PERFORMANCE_1H)
            {
            if((tmCPUStatus.tm_min % 10)==0)
                ulTemp=pHP->ulSessionBarRGBColorGrid;
            }
                                        /* If we found an even fraction, then draw vertical timeline,
                                           but only once per second (and there may be TICKSPERSECOND
                                           timer ticks in a single second) */
        if((ulTemp==pHP->ulSessionBarRGBColorGrid) && (ulDrawTimeline==TRUE) && !(pHP->ulStatusFlag2 & NOGRIDSB))
            {
            pointlLL.y=rectlCPUXStatus.yBottom;
            pointlLL.x=pointlUR.x;
            GpiMove(hpsDraw, &pointlLL);
            GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorGrid);
            GpiLine(hpsDraw, &pointlUR);
            }
#endif  /* SLOWVIDEO */
                                        /* And finally we draw current CPU Performance, which means that
                                           we draw the Interrupt time percentage from the bottom of
                                           the rectangle and increase if by the Busy time percentage
                                           (leaving the rest to the top of the rectangle being the Idle
                                           time percentage). We don't do that in "fill" mode however. */
        if(!(pHP->ulStatusFlag2 & PERFORMANCEFILLSB))
            {
                                        /* Now we draw the CPU Performance history from our saved values. For
                                           that we either start at the current and previous CPU Performance
                                           if we just do a timer update, or from the first value in the
                                           CPU Performance stack during a repaint */
#ifdef  SLOWVIDEO
            if(ulMouseEvent==WM_PAINT)
#else
            if((ulMouseEvent==WM_PAINT) || (hpsDraw==pHP->hpsSessionBarMemory))
#endif  /* SLOWVIDEO */
                {
                ulTemp=1;
                pointlUR.x=rectlCPUXStatus.xLeft+1;
                }
            else
                {
                ulTemp=ulCPUProcessorPixels-1;
                pointlUR.x=rectlCPUXStatus.xRight;
                }
            pointlLL.x=pointlUR.x-1;
            for( ; ulTemp<ulCPUProcessorPixels; ulTemp++, pointlLL.x++, pointlUR.x++)
                {
                pCPUPerformanceStackCpu=&pCPUPerformanceStack[(ulNumProcessor-1)+
                    (pHP->ulNumProcessors*(ulTemp))];
                pCPUPerformanceStackCpuPrevious=&pCPUPerformanceStack[(ulNumProcessor-1)+
                    (pHP->ulNumProcessors*(ulTemp-1))];
                                        /* First, we can only draw something, if at least the current value
                                           already is available */
                if(pCPUPerformanceStackCpu->ucIntr==(UCHAR)0xFF)
                    continue;
                                        /* Second, we address the last and previous to last pixel column
                                           for the current processor's Interrupt value */
                pointlLL.y=rectlCPUXStatus.yBottom+((ulIconSize-1)*((LONG)pCPUPerformanceStackCpuPrevious->ucIntr)/100);
                pointlUR.y=rectlCPUXStatus.yBottom+((ulIconSize-1)*((LONG)pCPUPerformanceStackCpu->ucIntr)/100);
                                        /* Third, we draw a line between the previous and current value,
                                           unless the previous value isn't available yet in which case
                                           we just draw a pixel */
                GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorIntr);
                if(pCPUPerformanceStackCpuPrevious->ucIntr==(UCHAR)0xFF)
                    GpiSetPel(hpsDraw, &pointlUR);
                else
                    {
                    GpiMove(hpsDraw, &pointlLL);
                    GpiLine(hpsDraw, &pointlUR);
                    }
                                        /* Fourth, we do the same for the Busy value */
                pointlLL.y+=((ulIconSize-1)*((LONG)pCPUPerformanceStackCpuPrevious->ucBusy)/100);
                pointlUR.y+=((ulIconSize-1)*((LONG)pCPUPerformanceStackCpu->ucBusy)/100);
                GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorBusy);
                if(pCPUPerformanceStackCpuPrevious->ucIntr==(UCHAR)0xFF)
                    GpiSetPel(hpsDraw, &pointlUR);
                else
                    {
                    GpiMove(hpsDraw, &pointlLL);
                    GpiLine(hpsDraw, &pointlUR);
                    }
                }
            }
        }
#ifndef SLOWVIDEO
    WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory,
        prectlCPUStatus, (POINTL *)prectlCPUStatus, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
    ulScrollTicks++;
    return((MRESULT)FALSE);
    }

/*                                                                                      *\
 * Syntax: WM_DRAWAPMSTATUS, HPS hpsDraw, ULONG ulMouseEvent                            *
\*                                                                                      */
case WM_DRAWAPMSTATUS:
/*                                                                                      *\
 * Draw the SessionBar's APM status area and its frame. A battery will by drawn where   *
 * the battery load is drawn in green unless it is lower than 10 percent in which case  *
 * it will be drawn in red.                                                             *
\*                                                                                      */
    {
    HPS             hpsDraw=(HPS)LONGFROMMP(mp1);
    RECTL           rectlAPMStatus=*prectlAPMStatus;
    ULONG           ulBatteryLife=SHORT2FROMMP(pHP->ulAPMStatus);
    ULONG           ulFullChargedHeight;
    POINTL          pointlLL;
    POINTL          pointlUR;

                                        /* If due to no APM status available during SessionBar
                                           creation the APM status area is 0 width, ignore message */
    if(prectlAPMStatus->xLeft==prectlAPMStatus->xRight)
        break;
                                        /* Calculate rectangle of APM status area */
    rectlAPMStatus.xLeft+=ulFrameSize;
    rectlAPMStatus.yBottom+=ulFrameSize;
    rectlAPMStatus.xRight-=ulFrameSize;
    rectlAPMStatus.yTop-=ulFrameSize;
    if(LONGFROMMP(mp2)==WM_MOUSEMOVE)
        {
                                        /* For hittest requests, format and display current
                                           battery/line status */
        if((WinPtInRect(pHP->habPc2, prectlAPMStatus, &pointlMouse)) ||
            (ulAPMStatusPrevious!=pHP->ulAPMStatus))
            {
            if(ulAPMStatusPrevious!=pHP->ulAPMStatus)
                WinPostMsg(hwnd, msg, (MPARAM)hpsDraw, NULL);
            ulAPMStatusPrevious=pHP->ulAPMStatus;
            if((pHP->ulRuntimeFlag & APMPOWERSUPPORT) && (ulBatteryLife<=100))
                sprintf(ucStatusText, "%s powered: %03d %%",
                    (SHORT1FROMMP(pHP->ulAPMStatus)==1 ? "AC"  : "Battery"),
                    (int)SHORT2FROMMP(pHP->ulAPMStatus));
            else
                strcpy(ucStatusText, "Unknown");
            rectlHotspot=*prectlAPMStatus;
            WinSendMsg(hwnd, WM_DRAWSESSIONTEXT, MPFROMP(hpsDraw), MPFROMP(ucStatusText));
            return((MRESULT)TRUE);
            }
        }
    else
        {
                                        /* For drawing requests, draw the 3D battery symbol */
        WinDrawBorder(hpsDraw, &rectlAPMStatus, ulFrameSize, ulFrameSize, 0, 0, DB_RAISED);
                                        /* Adjust for 3D border (and GpiBox()) */
        rectlAPMStatus.xLeft+=ulFrameSize;
        rectlAPMStatus.yBottom+=ulFrameSize;
        rectlAPMStatus.xRight-=ulFrameSize+1;
        rectlAPMStatus.yTop-=ulFrameSize+1;
                                        /* Calculate and display charge percentage, display the
                                           current load in green, except if below 10 % in which
                                           case we switch to red */
        if(ulBatteryLife>100)
            ulBatteryLife=0;
        ulFullChargedHeight=(rectlAPMStatus.yTop-rectlAPMStatus.yBottom);
        pointlLL=*((POINTL *)(&rectlAPMStatus.xLeft));
        pointlUR=*((POINTL *)(&rectlAPMStatus.xRight));
        pointlUR.y-=((ulFullChargedHeight*(100-ulBatteryLife))/100);
        GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorBusy);
        if(ulBatteryLife<=10)
            GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorIntr);
        GpiMove(hpsDraw, &pointlLL);
        GpiBox(hpsDraw, DRO_OUTLINEFILL, &pointlUR, 0, 0);
        pointlLL.y=pointlUR.y+1;
        pointlUR=*((POINTL *)(&rectlAPMStatus.xRight));
        GpiSetColor(hpsDraw, pHP->ulSessionBarRGBColorBgnd);
        GpiMove(hpsDraw, &pointlLL);
        GpiBox(hpsDraw, DRO_OUTLINEFILL, &pointlUR, 0, 0);
#ifndef SLOWVIDEO
        WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory,
            prectlAPMStatus, (POINTL *)prectlAPMStatus, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
        }
    return((MRESULT)FALSE);
    }

/*                                                                                      *\
 * Syntax: WM_HITTESTSTATUSAREA, HPS hpsDraw, ULONG ulMouseEvent                        *
\*                                                                                      */
case WM_HITTESTSTATUSAREA:
/*                                                                                      *\
 * Test if the Status Area is below the mouse pointer.                                  *
\*                                                                                      */
    {
    ULONG           ulMouseInRectangle;

    ulMouseInRectangle=(ULONG)WinPtInRect(pHP->habPc2, prectlSessionName, &pointlMouse);
    if(ulMouseInRectangle==FALSE)
        return((MRESULT)FALSE);
    if(LONGFROMMP(mp2)==WM_BUTTON2UP)
        {
        POINTL  pointlPopupMenu;

        pointlPopupMenu=pointlMouse;
        WinMapWindowPoints(hwnd, HWND_DESKTOP, &pointlPopupMenu, 1);
        WinPopupMenu(HWND_DESKTOP, hwnd, pHP->hwndPopupMenuStatus,
            pointlPopupMenu.x, pointlPopupMenu.y, ID_SBRIGHTJUSTIFIED,
            PU_POSITIONONITEM | PU_HCONSTRAIN | PU_VCONSTRAIN | PU_KEYBOARD |
            PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2);
        }
    return((MRESULT)TRUE);
    }
    break;



/*                                                                                      *\
 * Syntax: WM_DRAWSESSIONTEXT, HPS hpsDraw, UCHAR *pucCurrentSwTitle                    *
\*                                                                                      */
case WM_DRAWSESSIONTEXT:
/*                                                                                      *\
 * Draw the SessionBar's text field and its frame. The argument pucCurrentSwTitle       *
 * either points to a session's Window List entry or is one of the SESSIONBAR_* values  *
 * if no text is passed (which controls the status).                                    *
\*                                                                                      */
    {
    HPS             hpsDraw=(HPS)LONGFROMMP(mp1);
    RECTL           rectlSessionName=*prectlSessionName;
    static ULONG    ulSessionActive=FALSE;
    static CHAR     szPreviousSwtitle[MAXNAMEL+4]="";
    static CHAR     szCurrentSwtitle[MAXNAMEL+4]="";
    static ULONG    ulCurrentStatusIsTime=FALSE;
    static ULONG    ulPreviousStatusIsTime=FALSE;

    if(LONGFROMMP(mp2)==SESSIONBAR_DRAWSTATUS)
        {
                                        /* If the mouse pointer is not over any session's icon on the
                                           SessionBar, then we no longer need to display the session's
                                           Window List entry */
        if(ulSessionActive==FALSE)
            {
            ulCurrentStatusIsTime=TRUE;
            memset(szCurrentSwtitle, '\0', sizeof(szCurrentSwtitle));
            strncpy(szCurrentSwtitle, ucCurrentTime, sizeof(szCurrentSwtitle)-1);
            }
        }
    if(LONGFROMMP(mp2)==SESSIONBAR_NOSESSIONHIT)
        {
                                        /* If the status line is requested to draw a session's Window List
                                           entry load and draw it, as long the mouse pointer is over the
                                           icon */
        if((pHP->hwndBubbleHelpClient) && (ulSessionActive==TRUE))
            WinSendMsg(pHP->hwndBubbleHelpClient, WM_BUBBLEHELP, MPFROMLONG(BUBBLEHELP_HIDE), NULL);
        ulSessionActive=FALSE;
        ulCurrentStatusIsTime=TRUE;
        memset(szCurrentSwtitle, '\0', sizeof(szCurrentSwtitle));
        strncpy(szCurrentSwtitle, ucCurrentTime, sizeof(szCurrentSwtitle)-1);
                                        /* Hide the BubbleHelp */
        }
    if(LONGFROMMP(mp2)>SESSIONBAR_NOSESSIONHIT)
        {
                                        /* It must be a pointer to a text (a session's Window List
                                           entry) to draw */
        ulSessionActive=TRUE;
        ulCurrentStatusIsTime=FALSE;
        memset(szCurrentSwtitle, '\0', sizeof(szCurrentSwtitle));
        strncpy(szCurrentSwtitle, (char *)mp2, sizeof(szCurrentSwtitle)-1);
        }
    if((strcmp(szPreviousSwtitle, szCurrentSwtitle)) || (LONGFROMMP(mp2)==SESSIONBAR_REPAINTCURRENT))
        {
#ifdef  DEBUG_SB
        printf("SessionBar: WM_DRAWSESSIONTEXT (%s)\n", (char *)szCurrentSwtitle);
#endif  /* DEBUG_SB */
                                        /* If we wouldn't display the time in the status area,
                                           inform the BubbleHelp to update its position, size and
                                           text drawn. Then unhide it */
        if((pHP->ulStatusFlag2 & BUBBLEHELPSB) &&
            (pHP->hwndBubbleHelpClient) && 
            (ulCurrentStatusIsTime==FALSE))
            {
            POINTL  pointlHotspot;

            WinQueryWindowPos(pHP->hwndSessionFrame, &swpSessionBar);
            pointlHotspot.x=(rectlHotspot.xLeft+rectlHotspot.xRight)>>1;
            pointlHotspot.y=(rectlHotspot.yBottom+rectlHotspot.yTop)>>1;
            WinMapWindowPoints(hwnd, HWND_DESKTOP, &pointlHotspot, 1);
            if(pointlHotspot.y>(pHP->swpScreen.cy>>1))
                pointlHotspot.y=(0-swpSessionBar.y);
            else
                pointlHotspot.y=(swpSessionBar.y+swpSessionBar.cy);
            WinSendMsg(pHP->hwndBubbleHelpClient, WM_BUBBLEHELP, 
                MPFROMLONG(BUBBLEHELP_RELOCATE), MPFROM2SHORT((SHORT)pointlHotspot.x, (SHORT)pointlHotspot.y));
            WinPostMsg(pHP->hwndBubbleHelpClient, WM_BUBBLEHELP, 
                MPFROMLONG(BUBBLEHELP_TEXT), MPFROMP(strdup(szCurrentSwtitle)));
                                        /* Only if we actually draw some text we need to
                                           unhide the BubbleHelp (if we repaint it the BubbleHelp
                                           must already be visible so no need to show it again).
                                           This is required as during PC/2 startup a repaint
                                           is requested during WM_DRAW where we don't have a
                                           text to draw yet */
            if(LONGFROMMP(mp2)>SESSIONBAR_REPAINTCURRENT)
                {
                WinPostMsg(pHP->hwndBubbleHelpClient, WM_BUBBLEHELP, 
                    MPFROMLONG(BUBBLEHELP_SHOW), NULL);
                }
            }
        rectlSessionName.xLeft+=(ulFrameSize<<1);
        rectlSessionName.yBottom+=(ulFrameSize<<1);
        rectlSessionName.xRight-=(ulFrameSize<<1);
        rectlSessionName.yTop-=(ulFrameSize<<1);
                                        /* If session name area should not be drawn, don't draw
                                           anything, though the rectangle will be drawn as we
                                           can't scale the session icons fractional and the
                                           "excess" space goes into the session name rectangle */
                                        /* Repaint previous SessionName text with background color
                                           to erase it */
        if(!(pHP->ulStatusFlag2 & NOSESSIONNAMESB))
            {
            ULONG   ulCommand;

            ulCommand=(ulPreviousStatusIsTime==TRUE ? (DT_RIGHT|DT_VCENTER) : (DT_LEFT|DT_VCENTER));
            if(pHP->ulSessionBarStatusarea & SB_STATUSLEFTJUSTIFIED)
                ulCommand=(DT_LEFT|DT_VCENTER);
            WinDrawText(hpsDraw, -1, szPreviousSwtitle, &rectlSessionName, pHP->ulSessionBarRGBColorBgnd, 0,
                ulCommand);
            strcpy(szPreviousSwtitle, szCurrentSwtitle);
            ulCommand=(ulCurrentStatusIsTime==TRUE ? (DT_RIGHT|DT_VCENTER) : (DT_LEFT|DT_VCENTER));
            if(pHP->ulSessionBarStatusarea & SB_STATUSLEFTJUSTIFIED)
                ulCommand=(DT_LEFT|DT_VCENTER);
            WinDrawText(hpsDraw, -1, szPreviousSwtitle, &rectlSessionName, pHP->ulSessionBarRGBColorFgnd, 0,
                ulCommand);
            }
        ulPreviousStatusIsTime=ulCurrentStatusIsTime;
#ifndef SLOWVIDEO
        WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory,
            &rectlSessionName, (POINTL *)&rectlSessionName, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
        return((MRESULT)TRUE);
        }
    return((MRESULT)FALSE);
    }

/*                                                                                      *\
 * Syntax: WM_SESSIONBARUPDATE, ULONG ulSession, HSWITCH hswitchSession                 *
\*                                                                                      */
case WM_SESSIONBARUPDATE:
/*                                                                                      *\
 * This message is posted to the SessionBar by the working thread, if he finds out,     *
 * that the n-th session ulSession in the Window List differs between the current       *
 * enumeration and the last enumeration.                                                *
 * The changed session's switch handle hswitchSession is either the switch handle of    *
 * the current session ulSession or NULLHANDLE, if session ulSession no longer exists.  *
\*                                                                                      */
    {
    SWITCHLIST      *pswitchlistCurrentSession;
    SWITCHLIST      *pswitchlistPreviousSession;
    ULONG           ulSession=LONGFROMMP(mp1);
    HSWITCH         hswitchSession=(HSWITCH)mp2;
    SWCNTRL         swcntrlCurrentSession;

#ifdef  DEBUG_SB
    printf("SessionBar: WM_SESSIONBARUPDATE ulSession (%d), hswitchSession(%d)\n",
        (int)mp1, (int)mp2);
#endif  /* DEBUG_SB */
                                        /* Start with 1st session */
    pswitchlistPreviousSession=pswitchlistCurrentSession=pswitchlistAnchor;
    if(hswitchSession==NULLHANDLE)
        {
                                        /* When a session is destroyed, the number of sessions is
                                           reduced, so only the last one in the list must be removed
                                           (and also the last icon in the SessionBar therefore).
                                           If the session removed is in the middle of the Window List,
                                           then the following elements will just be changed and only
                                           the last gets removed instead */
        ulSessionNumber--;
#ifdef  DEBUG_SB
        WinQuerySwitchEntry(pswitchlistCurrentSession->hswitchSession, &swcntrlCurrentSession);
        printf("Session %d (%d) is %s\n", ulTemp++, pswitchlistCurrentSession->hswitchSession, swcntrlCurrentSession.szSwtitle);
        if(ulSessionNumber==(ULONG)-1)
            printf("SessionBar: WM_SESSIONBARUPDATE removed non-existent session\n");
        if(pswitchlistCurrentSession==NULL)
            printf("SessionBar: WM_SESSIONBARUPDATE removed from 0 available sessions\n");
#endif  /* DEBUG_SB */
        while(pswitchlistCurrentSession->pNext)
            {
                                        /* All switch handles that no longer exist return SWCNTRL structure
                                           filled from a previously queried valid switch handle, this appears
                                           to me as a bug (found under Warp) */
            pswitchlistPreviousSession=pswitchlistCurrentSession;
            pswitchlistCurrentSession=pswitchlistCurrentSession->pNext;
#ifdef  DEBUG_SB
            strcpy(swcntrlCurrentSession.szSwtitle, "\"Unknown\", deleted?");
            WinQuerySwitchEntry(pswitchlistCurrentSession->hswitchSession, &swcntrlCurrentSession);
            printf("SessionBar: Session #%d (%d) is %s\n", ulTemp++, pswitchlistCurrentSession->hswitchSession, swcntrlCurrentSession.szSwtitle);
#endif  /* DEBUG_SB */
            }
        if(pswitchlistPreviousSession)
            {
#ifdef  DEBUG_SB
            WinQuerySwitchEntry(pswitchlistCurrentSession->hswitchSession, &swcntrlCurrentSession);
            printf("SessionBar: %s (%d) gets removed\n", swcntrlCurrentSession.szSwtitle, pswitchlistCurrentSession->hswitchSession);
#endif  /* DEBUG_SB */
                                        /* We lost one icon for the SessionBar */
            ulSessionIconsCount--;
            free(pswitchlistPreviousSession->pNext);
            pswitchlistPreviousSession->pNext=NULL;
#ifdef  DEBUG_SB
            printf("SessionBar: Removing calls WM_DRAWSESSIONICONS %d\n", ulSessionNumber);
#endif  /* DEBUG_SB */
                                        /* Remove the session's icon */
            WinSendMsg(hwnd, WM_DRAWSESSIONICONS, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(ulSessionNumber));
                                        /* If ScrollBar was scrolled to rightmost positiion, then any
                                           session's icons deletion causes a hole, which we try to
                                           avoid by scrolling one session leftwards. */
            if(ulSessionIconsCount<(ulSessionIconsFirst+ulSessionIconsDrawn))
                if(ulSessionIconsFirst>0)
                    {
                    ulSessionIconsFirst--;
                    WinInvalidateRect(pHP->hwndSessionClient, prectlSessionIcons, FALSE);
                    }
                                        /* The removed session may require the SessionBar's scroll
                                           buttons to be updated */
            ulRedrawScrollButtons=TRUE;
            WinSendMsg(hwnd, WM_HITTESTSESSIONICONS, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(WM_MOUSEMOVE));
            }
        }
    else
        {
                                        /* When a session gets changed, then either an existing one
                                           gets changed, or the number of sessions is increased, in
                                           which case we add one element to the last one. If the session
                                           added is in the middle of the Window List, the following
                                           elements will just be changed and a new one gets added */
        while((pswitchlistCurrentSession) && (ulSession!=0))
            {
            pswitchlistPreviousSession=pswitchlistCurrentSession;
            pswitchlistCurrentSession=pswitchlistCurrentSession->pNext;
            ulSession--;
            }
        if(pswitchlistCurrentSession)
            {
            WinQuerySwitchEntry(hswitchSession, &swcntrlCurrentSession);
#ifdef  DEBUG_SB
            printf("SessionBar: %s (%d->%d) got changed\n", swcntrlCurrentSession.szSwtitle,
                pswitchlistCurrentSession->hswitchSession, hswitchSession);
#endif  /* DEBUG_SB */
                                        /* As the Window List for unknown reasons (to minimized update
                                           frequency?) sometimes contains entries for windows whose
                                           window handle isn't valid anymore (window gets/was destroyed)
                                           and when our logic removes such entries, the SessionBar will
                                           get to change a session to itself. In this case we must not
                                           increment the ordinal number of that session */
            if(pswitchlistCurrentSession->hswitchSession!=hswitchSession)
                {
                                        /* As the current entry got replaced by the following one,
                                           we have to ensure that the icon gets updated too */
                pswitchlistCurrentSession->hswitchSession=hswitchSession;
                pswitchlistCurrentSession->hwndIcon=swcntrlCurrentSession.hwnd;
                pswitchlistCurrentSession->ulStatusFlag=0;
                }
#ifdef  DEBUG_SB
            else
                printf("SessionBar: Replaced with existing session\n");
#endif  /* DEBUG_SB */
            }
        else
            {
                                        /* We have to add a one icon for the SessionBar */
            ulSessionIconsCount++;
            ulSessionNumber++;
#ifdef  DEBUG_SB
            WinQuerySwitchEntry(hswitchSession, &swcntrlCurrentSession);
            printf("SessionBar: %s (%d) got added\n", swcntrlCurrentSession.szSwtitle, hswitchSession);
#endif  /* DEBUG_SB */
            if(pswitchlistPreviousSession)
                pswitchlistCurrentSession=pswitchlistPreviousSession->pNext=malloc(sizeof(SWITCHLIST));
            else
                pswitchlistCurrentSession=pswitchlistAnchor=malloc(sizeof(SWITCHLIST));
            memset(pswitchlistCurrentSession, 0, sizeof(SWITCHLIST));
            pswitchlistCurrentSession->hswitchSession=hswitchSession;
            pswitchlistCurrentSession->pPrevious=pswitchlistPreviousSession;
                                        /* The added session may require the SessionBar's scroll
                                           buttons to be updated */
            ulRedrawScrollButtons=TRUE;
            WinSendMsg(hwnd, WM_HITTESTSESSIONICONS, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(WM_MOUSEMOVE));
            }
        {
        UCHAR   *pucExceptionItemBegin;
        UCHAR   *pucExceptionItemEnd;
        ULONG   ulExceptionItemLength;

                                        /* If the current window was requested to be hidden, hide its
                                           icon in the SessionBar (if the list is empty, that is just
                                           contains \r\n don't search at all) */
        WinQuerySwitchEntry(hswitchSession, &swcntrlCurrentSession);
        pucExceptionItemBegin=pHP->ucSessionBarException;
        pucExceptionItemEnd=strchr(pucExceptionItemBegin, '\r');
        if(pucExceptionItemBegin!=pucExceptionItemEnd)
            while(pucExceptionItemEnd)
            {
            ulExceptionItemLength=(ULONG)(pucExceptionItemEnd-pucExceptionItemBegin);
            *pucExceptionItemEnd='\0';
            if(strstr(swcntrlCurrentSession.szSwtitle, pucExceptionItemBegin))
                {
#ifdef  DEBUG_SB
                printf("SessionBar: %s's icon will be hidden\n", swcntrlCurrentSession.szSwtitle);
#endif  /* DEBUG_SB */
                pswitchlistCurrentSession->ulStatusFlag|=SESSIONHIDE;
                                        /* If the session's icon we insert should be hidden
                                           and also the first one, then the first one must be
                                           the next one (if that one isn't hidden also). That
                                           way, we start with visible session icons only.
                                           Initially, we used the statement
                                           if(ulSession==ulSessionIconsFirst)
                                           but as ulSession not longer corresponds to mp1, this
                                           cause that we started drawing the 2nd visible icon,
                                           instead of the 1st when a hidden icons was request */
                if((ULONG)mp1==ulSessionIconsFirst)
                    ulSessionIconsFirst++;
                WinInvalidateRect(hwnd, prectlSessionIcons, FALSE);
                }
            *pucExceptionItemEnd='\r';
            pucExceptionItemBegin=pucExceptionItemEnd+2;
            if(*pucExceptionItemBegin=='\0')
                break;
            pucExceptionItemEnd=strchr(pucExceptionItemBegin, '\r');
            }
        }
#ifdef  DEBUG_SB
        printf("SessionBar: Adding/Changing calls WM_DRAWSESSIONICONS %d\n", (int)mp1);
#endif  /* DEBUG_SB */
                                        /* Add or change the session's icon */
        WinSendMsg(hwnd, WM_DRAWSESSIONICONS, NULL, mp1);
        }
    }
    break;

case WM_PRESPARAMCHANGED:
#ifdef  DEBUG_SB
    printf("SessionBar: WM_PRESPARAMCHANGED\n");
#endif  /* DEBUG_SB */
    switch((ULONG)mp1)
    {
    ULONG       ulAttrFound=0;

    case PP_FONTNAMESIZE:
        {
        ULONG       ulAttrFound;
        FATTRS      *pfatPC2FontSession;
        FATTRS      *pfatPC2FontBubbleHelp;

                                        /* Get font selected for PC/2's SessionBar window */
        if(WinQueryPresParam(hwnd, PP_FONTNAMESIZE, 0, &ulAttrFound,
            sizeof(pHP->ucSessionWindowFont), pHP->ucSessionWindowFont, 0))
            {
            pfatPC2FontSession=QueryPresParamFont(hwnd);
            pfatPC2FontBubbleHelp=QueryPresParamFont(hwnd);
                                        /* Post to SessionBar window to change window font */
            WinPostMsg(hwnd, WM_SETPRESPARAMFONT,
                MPFROMLONG(PP_FONTNAMESIZE), MPFROMP(pfatPC2FontSession));
                                        /* Post to BubbleHelp window to change window font */
            WinPostMsg(pHP->hwndBubbleHelpClient, WM_SETPRESPARAMFONT,
                MPFROMLONG(PP_FONTNAMESIZE), MPFROMP(pfatPC2FontBubbleHelp));
            }
        }
        break;

    case PP_FOREGROUNDCOLOR:
                                        /* Get color selected for PC/2's SessionBar, just for the
                                           CPU Performance Status area be more specific */
        WinQueryPointerPos(HWND_DESKTOP, &pointlMouse);
        WinMapWindowPoints(HWND_DESKTOP, hwnd, &pointlMouse, 1);
        if(WinSendMsg(hwnd, WM_HITTESTCPUSTATUS, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(WM_NULL)))
            {
            if(WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000)
                {
                WinQueryPresParam(hwnd, PP_FOREGROUNDCOLOR, 0, &ulAttrFound,
                    sizeof(pHP->ulSessionBarRGBColorGrid), &pHP->ulSessionBarRGBColorGrid, QPF_NOINHERIT);
#ifdef  DEBUG
                printf("SessionBar: CPU Performance Grid color changed\n");
#endif  /* DEBUG */
                }
            else
                {
                WinQueryPresParam(hwnd, PP_FOREGROUNDCOLOR, 0, &ulAttrFound,
                    sizeof(pHP->ulSessionBarRGBColorBusy), &pHP->ulSessionBarRGBColorBusy, QPF_NOINHERIT);
#ifdef  DEBUG
                printf("SessionBar: CPU Performance Busy color changed\n");
#endif  /* DEBUG */
                }
            }
        else
            WinQueryPresParam(hwnd, PP_FOREGROUNDCOLOR, 0, &ulAttrFound,
                sizeof(pHP->ulSessionBarRGBColorFgnd), &pHP->ulSessionBarRGBColorFgnd, QPF_NOINHERIT);
        WinInvalidateRect(hwnd, NULL, FALSE);
        break;

    case PP_BACKGROUNDCOLOR:
                                        /* Get color selected for PC/2's SessionBar, just for the
                                           CPU Performance Status area be more specific */
        WinQueryPointerPos(HWND_DESKTOP, &pointlMouse);
        WinMapWindowPoints(HWND_DESKTOP, hwnd, &pointlMouse, 1);
        if(WinSendMsg(hwnd, WM_HITTESTCPUSTATUS, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(WM_NULL)))
            {
            if(WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000)
                {
                WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, 0, &ulAttrFound,
                    sizeof(pHP->ulSessionBarRGBColorCPU), &pHP->ulSessionBarRGBColorCPU, QPF_NOINHERIT);
#ifdef  DEBUG
                printf("SessionBar: CPU Performance Background color changed\n");
#endif  /* DEBUG */
                }
            else
                {
                WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, 0, &ulAttrFound,
                    sizeof(pHP->ulSessionBarRGBColorIntr), &pHP->ulSessionBarRGBColorIntr, QPF_NOINHERIT);
#ifdef  DEBUG
                printf("SessionBar: CPU Performance Interrupt color changed\n");
#endif  /* DEBUG */
                }
            }
        else
            WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, 0, &ulAttrFound,
                sizeof(pHP->ulSessionBarRGBColorBgnd), &pHP->ulSessionBarRGBColorBgnd, QPF_NOINHERIT);
        WinInvalidateRect(hwnd, NULL, FALSE);
        break;
    }
    break;

/*                                                                                      *\
 * Syntax: WM_DESKTOPMOVE, NULL, NULL                                                   *
\*                                                                                      */
case WM_DESKTOPMOVE:
/*                                                                                      *\
 * Something changed with the windows, so let's see if the active window changed and    *
 * this should be reflected in the SessionBar according to the Window List.             *
\*                                                                                      */
    {
    SWITCHLIST      *pswitchlistCurrentSession;
    SWCNTRL         swcntrlCurrentSession;
    HWND            hwndActiveWindow;
    HWND            hwndQueryWindow;

#ifdef  DEBUG_SB
    printf("SessionBar: WM_DESKTOPMOVE\n");
#endif  /* DEBUG_SB */
                                        /* Start with 1st session */
    pswitchlistCurrentSession=pswitchlistAnchor;
                                        /* Reset the current active session flag, as this may
                                           have changed since the last invocation */
    while(pswitchlistCurrentSession)
        {
        pswitchlistCurrentSession->ulStatusFlag&=(~(SESSIONACTIVE|SESSIONPAINTED));
        pswitchlistCurrentSession=pswitchlistCurrentSession->pNext;
        }
                                        /* Find the session (screen group) the current active window belongs
                                           to. As a screen group may be active, even if the frame window
                                           registered in the Window List is not the active one, but a derived
                                           child window, some tests must be done to find it most likely */
    hwndActiveWindow=hwndQueryWindow=WinQueryActiveWindow(HWND_DESKTOP);
    ulSessionCurrent=0;
    pswitchlistCurrentSession=pswitchlistAnchor;
    while(pswitchlistCurrentSession)
        {
        WinQuerySwitchEntry(pswitchlistCurrentSession->hswitchSession, &swcntrlCurrentSession);
        hwndActiveWindow=hwndQueryWindow;
        if(swcntrlCurrentSession.hwnd==hwndActiveWindow)
            break;
                                        /* First let's see if the current frame window is the owner window
                                           of the current active window */
        hwndActiveWindow=WinQueryWindow(hwndQueryWindow, QW_OWNER);
        while(hwndActiveWindow!=NULLHANDLE)
            {
            if(hwndActiveWindow==swcntrlCurrentSession.hwnd)
                break;
            hwndActiveWindow=WinQueryWindow(hwndActiveWindow, QW_OWNER);
            }
        if(hwndActiveWindow==swcntrlCurrentSession.hwnd)
            break;
                                        /* As the current frame window is not the owner window of the current
                                           active window, second let's see if it is the parent window */
        hwndActiveWindow=WinQueryWindow(hwndQueryWindow, QW_PARENT);
        while((hwndActiveWindow!=pHP->hwndDesktop) && (hwndActiveWindow!=NULLHANDLE))
            {
            if(hwndActiveWindow==swcntrlCurrentSession.hwnd)
                break;
            hwndActiveWindow=WinQueryWindow(hwndActiveWindow, QW_PARENT);
            }
        if(hwndActiveWindow==swcntrlCurrentSession.hwnd)
            break;
                                        /* We haven't found anything, so let's try the next session */
        pswitchlistCurrentSession=pswitchlistCurrentSession->pNext;
        ulSessionCurrent++;
        }
                                        /* If the owner/parent window chain didn't give us any result, let's
                                           try the process/thread level. We have to do this only in case
                                           the owner/parent window chain didn't give us a result, as e.g.
                                           the EPM Search dialog window may be owned by n EPM Editor windows
                                           (all running in the same process, but having different Window
                                           List entries), but only the (active ?) one is the owner. Changing
                                           the active EPM Editor window also changes the owner of the Search
                                           dialog */
    if(!pswitchlistCurrentSession)
        {
        PID         pidCurrentSession;
        TID         tidCurrentSession;
        PID         pidActiveWindow;
        TID         tidActiveWindow;
        PID         pidDesktop;
        TID         tidDesktop;

        ulSessionCurrent=0;
        pswitchlistCurrentSession=pswitchlistAnchor;
        while(pswitchlistCurrentSession)
            {
                                        /* As the current frame window is neither the owner or parent window
                                           of the current active window, third let's see if it is in the same
                                           process as the current active one. If PM is the process this window
                                           runs in, ignore it, as we first never have a session icon for the
                                           PM process itself and second the first session icon whose window
                                           runs in the PM process (OS/2 Windows, Window List,...) will be
                                           marked as the active one then - which is definitely semantically
                                           wrong */
            WinQuerySwitchEntry(pswitchlistCurrentSession->hswitchSession, &swcntrlCurrentSession);
            WinQueryWindowProcess(swcntrlCurrentSession.hwnd, &pidCurrentSession, &tidCurrentSession);
            WinQueryWindowProcess(hwndQueryWindow, &pidActiveWindow, &tidActiveWindow);
            WinQueryWindowProcess(pHP->hwndDesktop, &pidDesktop, &tidDesktop);
            if((pidCurrentSession==pidActiveWindow) && (pidCurrentSession!=pidDesktop))
                break;
                                        /* We haven't found anything, so let's try the next session */
            pswitchlistCurrentSession=pswitchlistCurrentSession->pNext;
            ulSessionCurrent++;
            }
        }
                                        /* If we found a session (screen group) the current active window
                                           belongs to, then mark it as the active one, if not, we've already
                                           unmarked the previous active one as being no longer active */
    if(pswitchlistCurrentSession)
        {
        pswitchlistCurrentSession->ulStatusFlag&=(~SESSIONPAINTED);
        pswitchlistCurrentSession->ulStatusFlag|=SESSIONACTIVE;
                                        /* Save our (1-based) index for this session */
        pswitchlistCurrentSession->ulSession=ulSessionCurrent+1;
        }
                                        /* If the current active session is different from the
                                           previous one, we have to redraw the SessionBar icons */
    if(ulSessionPrevious!=ulSessionCurrent)
        {
                                        /* Redraw deactivated session's icon */
        if(ulSessionPrevious!=(ULONG)-1)
            {
#ifdef  DEBUG_SB
            printf("SessionBar: Deactivating calls WM_DRAWSESSIONICONS %d\n", (int)ulSessionPrevious);
#endif  /* DEBUG_SB */
            WinPostMsg(hwnd, WM_DRAWSESSIONICONS,
                MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(ulSessionPrevious));
            ulSessionPrevious=(ULONG)-1;
            }
                                        /* Redraw activated session's icon */
        if(pswitchlistCurrentSession)
            {
#ifdef  DEBUG_SB
            printf("SessionBar: Activating calls WM_DRAWSESSIONICONS %d\n", (int)ulSessionCurrent);
#endif  /* DEBUG_SB */
            WinPostMsg(hwnd, WM_DRAWSESSIONICONS,
                MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(ulSessionCurrent));
            ulSessionPrevious=ulSessionCurrent;
            }
        }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SETPRESPARAMFONT, ULONG ulPresentationParameter, FATTRS *pfattrsFont      *
\*                                                                                      */
case WM_SETPRESPARAMFONT:
/*                                                                                      *\
 * Modified WM_PRESPARAMCHANGED message for font setting.                               *
\*                                                                                      */
    if((ULONG)mp1==PP_FONTNAMESIZE)
        {
#ifdef  DEBUG_SB
        printf("SessionBar: WM_SETPRESPARAMFONT\n");
#endif  /* DEBUG_SB */
                                        /* Set SessionBar window font */
        SetPresParamFont(HPSSESSIONBARDRAW, (FATTRS *)PVOIDFROMMP(mp2), LCID_FONT);
                                        /* Repaint SessionBar window */
        WinInvalidateRect(hwnd, NULL, FALSE);
        }
    break;

case WM_MOUSEMOVE:
                                        /* Save current mouse position */
    pointlMouse.x=(LONG)SHORT1FROMMP(mp1);
    pointlMouse.y=(LONG)SHORT2FROMMP(mp1);
                                        /* Find the part of the SessionBar that accepts the mouse position */
    if(!WinSendMsg(hwnd, WM_DRAWLAUNCHBUTTON, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(msg)))
        if(!WinSendMsg(hwnd, WM_DRAWAPMSTATUS, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(msg)))
            if(!WinSendMsg(hwnd, WM_HITTESTCPUSTATUS, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(msg)))
                if(!WinSendMsg(hwnd, WM_HITTESTSESSIONICONS, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(msg)))
                    {
                                        /* If noone claimed the mouse position, ensure that the status field
                                           contains the time information */
                    WinPostMsg(hwnd, WM_DRAWSESSIONTEXT,
                        MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(SESSIONBAR_NOSESSIONHIT));
                    }
                                        /* As our input hook sends an additional WM_MOUSEMOVE after (!) the
                                           mouse was moved from our window (to reset the Launch Button and the
                                           session name), we should not pass this message on to PM, because
                                           otherwise it gets confused and does not point pointer shapes
                                           (e.g. Overview Window, EPM,...) correctly. If the position is
                                           within our window, pass it on to PM to ensure that the arrow mouse
                                           pointer gets set */
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));

case WM_MOUSELEAVE:
    {
                                        /* Posted by input hook in PC2HOOK.DLL after the mouve was moved
                                           off the SessionBar window. We will replace any text in the status
                                           field with the time information. Get the mouse position relative to
                                           the SessionBar (which is of course outside the SessionBar, so
                                           we don't get a match with the last mouse position inside the
                                           SessionBar anywhere) */
    WinQueryPointerPos(HWND_DESKTOP, &pointlMouse);
    WinMapWindowPoints(HWND_DESKTOP, hwnd, &pointlMouse, 1);
    WinPostMsg(hwnd, WM_DRAWSESSIONTEXT,
        MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(SESSIONBAR_NOSESSIONHIT));
    }
    break;

case WM_BUTTON1DOWN:
                                        /* Display client menu if CTRL keys are pressed while mouse
                                           button 1 is pressed */
    if(WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000)
        {
        POINTL  pointlPopupMenu;

        WinQueryPointerPos(HWND_DESKTOP, &pointlPopupMenu);
        WinSendMsg(pHP->hwndClientMenu, MM_SETITEMATTR,
            MPFROM2SHORT(ID_OVSUBMENUICONS, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
        WinPopupMenu(HWND_DESKTOP, pHP->hwndClient, pHP->hwndClientMenu,
            pointlPopupMenu.x, pointlPopupMenu.y, ID_OVSUBMENUDEBUG,
            PU_POSITIONONITEM | PU_HCONSTRAIN | PU_VCONSTRAIN | PU_KEYBOARD |
            PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2);
        }
#ifndef SLOWVIDEO
                                        /* Draw the whole memory bitmap into the client area,
                                           without any background or foreground color because
                                           we have no monochrome bitmap */
    WinDrawBitmap(pHP->hpsSession, pHP->hbmSessionBarMemory, NULL,
        (POINTL *)&rectlClient, 0L, 0L, DBM_NORMAL);
#endif  /* SLOWVIDEO */
case WM_BUTTON1UP:
                                        /* Save mouse button 1 state */
    ulButton1State=msg;
    WinSendMsg(hwnd, WM_DRAWLAUNCHBUTTON, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(msg));
case WM_BUTTON2DOWN:
                                        /* In case SessionBar is not kept on top of Z-Order
                                           set it to top of Z-Order */
    if(!(pHP->ulStatusFlag2 & KEEPONTOPSB))
        {
        memset(&swpSessionBar, 0, sizeof(swpSessionBar));
        swpSessionBar.hwnd=pHP->hwndSessionFrame;
        swpSessionBar.fl=SWP_ZORDER;
        swpSessionBar.hwndInsertBehind=HWND_TOP;
        WinSetMultWindowPos(pHP->habPc2, &swpSessionBar, 1);
        }
case WM_BUTTON2UP:
    {
                                        /* Let the user move the SessionBar window with mouse
                                           button 2. The following code
                                               WinSendMsg(pHP->hwndSessionFrame, WM_TRACKFRAME,
                                                   MPFROMLONG(TF_ALLINBOUNDARY|TF_MOVE), NULL);
                                           would also let the user track the window, but would not
                                           limit it to the screen dimensions */
    TRACKINFO   trackinfoSession;

    if((!WinSendMsg(hwnd, WM_HITTESTSESSIONICONS, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(msg))) &&
        (!WinSendMsg(hwnd, WM_HITTESTCPUSTATUS, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(msg))) &&
        (!WinSendMsg(hwnd, WM_HITTESTSTATUSAREA, MPFROMP(HPSSESSIONBARDRAW), MPFROMLONG(msg))) &&
        (msg==WM_BUTTON2DOWN) &&
        !(pHP->ulStatusFlag2 & DYNAMICSESSIONBAR))
        {
        memset(&trackinfoSession, 0, sizeof(trackinfoSession));
        trackinfoSession.cxBorder=WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
        trackinfoSession.cyBorder=WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER);
        WinQueryWindowRect(pHP->hwndSessionFrame, &trackinfoSession.rclTrack);
        WinMapWindowPoints(pHP->hwndSessionFrame, HWND_DESKTOP, (PPOINTL)&trackinfoSession.rclTrack, 2);
        trackinfoSession.rclBoundary.xRight=pHP->swpScreen.cx;
        trackinfoSession.rclBoundary.yTop=pHP->swpScreen.cy;
        trackinfoSession.ptlMaxTrackSize.x=trackinfoSession.rclTrack.xRight-trackinfoSession.rclTrack.xLeft;
        trackinfoSession.ptlMaxTrackSize.y=trackinfoSession.rclTrack.yTop-trackinfoSession.rclTrack.yBottom;
        trackinfoSession.fs=TF_ALLINBOUNDARY|TF_MOVE;
        if(WinTrackRect(HWND_DESKTOP, NULLHANDLE, &trackinfoSession))
            {
            memset(&swpSessionBar, 0, sizeof(swpSessionBar));
            swpSessionBar.hwnd=pHP->hwndSessionFrame;
            swpSessionBar.x=trackinfoSession.rclTrack.xLeft;
            swpSessionBar.y=trackinfoSession.rclTrack.yBottom;
            swpSessionBar.fl=SWP_MOVE;
            WinSetMultWindowPos(pHP->habPc2, &swpSessionBar, 1);
            }
        }
    }
                                        /* Prevent the SessionBar window to get the focus (by bypassing
                                           default window procedure) */
    break;

#ifdef  SLOWVIDEO
case WM_MENUEND:
                                        /* When drawing directly into the SessionBar presentation space
                                           (in contrast to blitting from memory presentation space), then
                                           the CPU Performance popup menu may cause some drawings missing
                                           as WinScrollWindow() can't scroll graphics to visible if it
                                           was clipped away in that part of the presentation space covered
                                           by the menu */
    if(HWNDFROMMP(mp2)==pHP->hwndPopupMenuProcessor)
        WinInvalidateRect(pHP->hwndSessionClient, prectlCPUStatus, FALSE);
    break;
#endif  /* SLOWVIDEO */

case WM_COMMAND:
    {
    USHORT  usCommand;

    usCommand=SHORT1FROMMP(mp1);
    if((usCommand>=ID_SBPROCESSOR_CPU1) && (usCommand<=ID_SBPROCESSOR_CPULAST))
        {
                                        /* If the user has selected a processor from the SessionBar's
                                           Processor Performance context menu, then toggle the status
                                           of that processor (the context menu gets updated automatically
                                           due to the timer) */
        ulNumProcessor=(ULONG)usCommand-ID_SBPROCESSORMENU;
        (pHP->DosGetProcessorStatus)(ulNumProcessor, &ulProcessorStatus);
        (pHP->DosSetProcessorStatus)(ulNumProcessor,
            (ulProcessorStatus==PROCESSOR_ONLINE ? PROCESSOR_OFFLINE : PROCESSOR_ONLINE));
        break;
        }
    if((usCommand>=ID_SBSETTINGS60S) && (usCommand<=ID_SBSETTINGS1H))
        {
                                        /* Change the checkmark from the previous to the current
                                           selection, update the CPU Performance flag accordingly and
                                           cause a redraw (after clearing out the history data) */
        for(ulProcessorStatus=0;
            !((pHP->ulPerformanceFlag>>ulProcessorStatus) & 0x00000001);
            ulProcessorStatus++)
            ; /* NOP */
        WinSendMsg(pHP->hwndPopupMenuProcessor, MM_SETITEMATTR,
            MPFROM2SHORT((SHORT)ID_SBSETTINGS60S+ulProcessorStatus, TRUE),
            MPFROM2SHORT(MIA_CHECKED, 0));
        WinSendMsg(pHP->hwndPopupMenuProcessor, MM_SETITEMATTR,
            MPFROM2SHORT(usCommand, TRUE),
            MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
        ulProcessorStatus=PERFORMANCE_60S<<(usCommand-ID_SBSETTINGS60S);
        if(ulProcessorStatus!=(pHP->ulPerformanceFlag & (PERFORMANCE_60S|PERFORMANCE_5M|PERFORMANCE_30M|PERFORMANCE_1H)))
            {
            WinSendMsg(hwnd, WM_CALCULATECPUSTATUS, MPFROMLONG(WM_CREATE), NULL);
            ulCPUInitialized=FALSE;
            ulTimerTicks=(ULONG)-1;
            }
                                        /* Update the status, force a redraw of the CPU Performance
                                           graphics, but allow the timer to wait for an "even" value
                                           for minutes and seconds (e.g. so we don't draw for second
                                           3, 13, ... but 0, 5, 10, ... instead) */
        pHP->ulPerformanceFlag&=~(PERFORMANCE_60S|PERFORMANCE_5M|PERFORMANCE_30M|PERFORMANCE_1H);
        pHP->ulPerformanceFlag|=ulProcessorStatus;
        WinInvalidateRect(pHP->hwndSessionClient, prectlCPUStatus, FALSE);
        break;
        }
                                        /* Adjust text justification appropriately */
    if((usCommand==ID_SBLEFTJUSTIFIED) || (usCommand==ID_SBRIGHTJUSTIFIED))
        {
        WinSendMsg(pHP->hwndPopupMenuStatus, MM_SETITEMATTR,
            MPFROM2SHORT((pHP->ulSessionBarStatusarea & SB_STATUSLEFTJUSTIFIED ? ID_SBLEFTJUSTIFIED : ID_SBRIGHTJUSTIFIED), TRUE),
            MPFROM2SHORT(MIA_CHECKED, 0));
        WinSendMsg(pHP->hwndPopupMenuStatus, MM_SETITEMATTR,
            MPFROM2SHORT(usCommand, TRUE),
            MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED));
        pHP->ulSessionBarStatusarea&=~(SB_STATUSLEFTJUSTIFIED|SB_STATUSRIGHTJUSTIFIED);
        pHP->ulSessionBarStatusarea|=(SB_STATUSLEFTJUSTIFIED << (usCommand-ID_SBLEFTJUSTIFIED));
        break;
        }
    if(pswitchlistSelected==NULL)
        break;
    switch(usCommand)
    {
    case ID_ICONSUNHIDE:
        {
        SWITCHLIST  *pswitchlistCurrentSession;

                                        /* User wants to unhide all hidden session icons in SessionBar */
        pswitchlistCurrentSession=pswitchlistAnchor;
        while(pswitchlistCurrentSession!=NULL)
            {
            pswitchlistCurrentSession->ulStatusFlag &=(~SESSIONHIDE);
            pswitchlistCurrentSession=pswitchlistCurrentSession->pNext;
            }
        ulRedrawScrollButtons=TRUE;
        WinInvalidateRect(pHP->hwndSessionClient, prectlSessionIcons, FALSE);
        WinInvalidateRect(pHP->hwndSessionClient, prectlScrollLeft, FALSE);
        WinInvalidateRect(pHP->hwndSessionClient, prectlScrollRight, FALSE);
        }
        break;

    case ID_ICONHIDE:
        {
                                        /* User wants to hide current session's icon in SessionBar */
        pswitchlistSelected->ulStatusFlag|=SESSIONHIDE;
                                        /* Force a repaint be invalidating the rectangle of the icons */
        ulRedrawScrollButtons=TRUE;
        WinInvalidateRect(pHP->hwndSessionClient, prectlSessionIcons, FALSE);
        WinInvalidateRect(pHP->hwndSessionClient, prectlScrollLeft, FALSE);
        WinInvalidateRect(pHP->hwndSessionClient, prectlScrollRight, FALSE);
        }
        break;

    case ID_ICONSHUTDOWN:
                                        /* User wants to quit the whole application the selected
                                           window belongs to. We do this by posting WM_QUIT, to
                                           terminate the message loop */
        WinPostMsg(pswitchlistSelected->hwndIcon, WM_QUIT, NULL, NULL);
        break;

    case ID_ICONEXIT:
                                        /* User wants to close the selected window, so we post a
                                           WM_SYSCOMMAND SC_CLOSE message */
        WinPostMsg(pswitchlistSelected->hwndIcon, WM_SYSCOMMAND, MPFROMLONG(SC_CLOSE), MPFROM2SHORT(CMDSRC_MENU, FALSE));
        break;
    }
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This procedure is the PC/2 QuickSwith window procedure.                              *
\*--------------------------------------------------------------------------------------*/
MRESULT EXPENTRY PC2_SwitchWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Replacement pointer for not accessible pointers */
static HPOINTER hpointerReplacement=NULLHANDLE;
                                        /*  Current Window List entry's HSWITCH that is used to represent
                                            the window in the QuickSwitch window */
static HSWITCH  hswitchCurrentSession=NULLHANDLE;
                                        /* Switch control block corresponding to current window's switch handle */
SWCNTRL         swcntrlCurrentSession;
                                        /* Last Window List entry's titlebar displayed on QuickSwitch window
                                           before the current window, used to redraw the previous text with
                                           the background color and drawing the current text with foreground
                                           color above */
static CHAR     szPreviousSwtitle[MAXNAMEL+4]="";
                                        /* Client window rectangle */
static RECTL    rectlClient;
                                        /* Rectangle for border, icon, text,... */
static RECTL    rectlTemp;

switch(msg)
{
case WM_CREATE:
    if(!hpointerReplacement)
        hpointerReplacement=WinLoadPointer(HWND_DESKTOP, pHP->hDLLPc2Resource, ID_ICONSESSIONUNKNOWN);
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));

case WM_DESTROY:
                                        /* Cleanup other windows dependencies */
    pHP->hwndSwitchFrame=pHP->hwndSwitchClient=NULLHANDLE;
                                        /* Free our resources */
    WinDestroyPointer(hpointerReplacement);
                                        /* Destroy QuickSwitch's presenation space and device context */
    GpiDestroyPS(pHP->hpsSwitch);
    DevCloseDC(pHP->hdcSwitch);
    pHP->hpsSwitch=pHP->hdcSwitch=NULLHANDLE;
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));

case WM_PAINT:
    {
    HPS     hpsClient;
                                        /* Get a cached presentation space */
    hpsClient=WinBeginPaint(hwnd, NULLHANDLE, &rectlClient);
                                        /* Fill invalid rectangle with dialog background color */
    WinFillRect(hpsClient, &rectlClient, SYSCLR_DIALOGBACKGROUND);
                                        /* Query client rectangle */
    WinQueryWindowRect(hwnd, &rectlTemp);
                                        /* Draw outer 3D border (QUICKSWITCH_BORDER pixel width) */
    WinDrawBorder(hpsClient, &rectlTemp, QUICKSWITCH_BORDER, QUICKSWITCH_BORDER, 0, 0, DB_RAISED);
                                        /* Draw inner 3D border (QUICKSWITH_BORDER pixel width) */
    rectlTemp.xLeft+=(QUICKSWITCH_BORDER+QUICKSWITCH_SPACE);
    rectlTemp.yBottom+=(QUICKSWITCH_BORDER+QUICKSWITCH_SPACE);
    rectlTemp.xRight-=(QUICKSWITCH_BORDER+QUICKSWITCH_SPACE);
    rectlTemp.yTop-=(QUICKSWITCH_BORDER+QUICKSWITCH_SPACE);
    WinDrawBorder(hpsClient, &rectlTemp, QUICKSWITCH_BORDER, QUICKSWITCH_BORDER, 0, 0, DB_DEPRESSED);
    WinSendMsg(hwnd, WM_DRAW, NULL, NULL);
    WinEndPaint(hpsClient);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_DRAW, HPS hpsDraw, RECTL *prectlDraw                                      *
\*                                                                                      */
case WM_DRAW:
/*                                                                                      *\
 * This undocumented message is used by PM controls to allow them to paint itself into  *
 * a provided presentation space. If rectlDraw is 0, we have to draw the entire control.*
\*                                                                                      */
    {
    HPS         hpsDraw=(HPS)LONGFROMMP(mp1);
    HPOINTER    hpointerFrame=NULLHANDLE;
    ULONG       ulError=NO_ERROR;

                                        /* Query client area */
    WinQueryWindowRect(hwnd, &rectlClient);
                                        /* If no presentation space is passed (from WM_PAINT), use
                                           permanent presentation space */
    if(!hpsDraw)
        hpsDraw=pHP->hpsSwitch;
                                        /* Draw QuickSwitch icon (the icon of the next session to
                                           quickswitch to) */
    WinQuerySwitchEntry(hswitchCurrentSession, &swcntrlCurrentSession);
    hpointerFrame=(HPOINTER)WinSendMsg(swcntrlCurrentSession.hwnd, WM_QUERYICON, NULL, NULL);
    rectlTemp=rectlClient;
    rectlTemp.xLeft+=2*QUICKSWITCH_BORDER+QUICKSWITCH_SPACE+QUICKSWITCH_ICONSPACE;
    rectlTemp.yBottom+=2*QUICKSWITCH_BORDER+QUICKSWITCH_SPACE+QUICKSWITCH_ICONSPACE;
    rectlTemp.xRight=rectlTemp.xLeft+pHP->lIconSize;
    rectlTemp.yTop=rectlTemp.yBottom+pHP->lIconSize;
    GpiSetColor(hpsDraw, SYSCLR_DIALOGBACKGROUND);
    GpiMove(hpsDraw, (POINTL *)(&rectlTemp.xLeft));
    GpiBox(hpsDraw, DRO_OUTLINEFILL, (POINTL *)(&rectlTemp.xRight), 0, 0);
    ulError=BlitIcon(hpsDraw, hpointerFrame, &rectlTemp);
                                        /* In case of blitting errors display at least a question mark
                                           to represent an unknown icon */
    if(ulError)
        {
#ifdef  DEBUG_SB
        printf("QuickSwitch: WM_QUERYICON returned bad pointer %08X\n", (int)hpointerFrame);
#endif  /* DEBUG_SB */
        BlitIcon(hpsDraw, hpointerReplacement, &rectlTemp);
        }
                                        /* Draw QuickSwitch text (the title of the next session to
                                           quickswitch to after erasing the previous text (which
                                           is likely to be present)) */
    rectlTemp=rectlClient;
    rectlTemp.xLeft+=2*QUICKSWITCH_BORDER+2*QUICKSWITCH_SPACE+2*QUICKSWITCH_ICONSPACE+pHP->lIconSize;
    rectlTemp.yBottom+=2*QUICKSWITCH_BORDER+2*QUICKSWITCH_SPACE;
    rectlTemp.xRight-=(2*QUICKSWITCH_BORDER+QUICKSWITCH_SPACE+QUICKSWITCH_ICONSPACE);
    rectlTemp.yTop-=(2*QUICKSWITCH_BORDER+2*QUICKSWITCH_SPACE);
                                        /* Repaint previous QuickSwitch text with background color
                                           to erase it */
    WinDrawText(hpsDraw, -1, szPreviousSwtitle, &rectlTemp, SYSCLR_DIALOGBACKGROUND, 0,
        DT_CENTER|DT_VCENTER);
    strcpy(szPreviousSwtitle, swcntrlCurrentSession.szSwtitle);
    WinDrawText(hpsDraw, -1, szPreviousSwtitle, &rectlTemp, CLR_BLACK, 0,
        DT_CENTER|DT_VCENTER);
    }
    break;

case WM_PRESPARAMCHANGED:
#ifdef  DEBUG_QS
    printf("QuickSwitch: WM_PRESPARAMCHANGED\n");
#endif  /* DEBUG_QS */
    if((ULONG)mp1==PP_FONTNAMESIZE)
        {
        ULONG       ulAttrFound;
        FATTRS      *pfatPC2FontSwitch;

                                        /* Get font selected for PC/2's QuickSwitch window */
        if(WinQueryPresParam(pHP->hwndSwitchClient, PP_FONTNAMESIZE, 0, &ulAttrFound,
            sizeof(pHP->ucSwitchWindowFont), pHP->ucSwitchWindowFont, 0))
            {
            pfatPC2FontSwitch=QueryPresParamFont(pHP->hwndSwitchClient);
                                        /* Post to QuickSwitch window to change window font */
            WinPostMsg(pHP->hwndSwitchClient, WM_SETPRESPARAMFONT,
                MPFROMLONG(PP_FONTNAMESIZE), MPFROMP(pfatPC2FontSwitch));
            }
        }
    break;

/*                                                                                      *\
 * Syntax: WM_SETPRESPARAMFONT, ULONG ulPresentationParameter, FATTRS *pfattrsFont      *
\*                                                                                      */
case WM_SETPRESPARAMFONT:
/*                                                                                      *\
 * Modified WM_PRESPARAMCHANGED message for font setting.                               *
\*                                                                                      */
    if((ULONG)mp1==PP_FONTNAMESIZE)
        {
#ifdef  DEBUG_QS
        printf("QuickSwitch: WM_SETPRESPARAMFONT\n");
#endif  /* DEBUG_QS */
                                        /* Set QuickSwitch window font */
        SetPresParamFont(pHP->hpsSwitch, (FATTRS *)PVOIDFROMMP(mp2), LCID_FONT);
                                        /* Repaint QuickSwitch window */
        WinInvalidateRect(pHP->hwndSwitchClient, NULL, FALSE);
        }
    break;

/*                                                                                      *\
 * Syntax: WM_QUICKSWITCH, ULONG ulFlag, HSWITCH hswitchCurrent                         *
\*                                                                                      */
case WM_QUICKSWITCH:
/*                                                                                      *\
 * This message is sent by the working thread, when the keys CTRL+TAB or CTRL+SHIFT+TAB *
 * have been detected. These keys are used to activate QuickSwitch, or to advance the   *
 * selection in the QuickSwitch to the next/previous application.                       *
\*                                                                                      */
    {
#ifdef  DEBUG_QS
    printf("QuickSwitch: WM_QUICKSWITCH ");
#endif  /* DEBUG_QS */
    switch(LONGFROMMP(mp1))
    {
    case QUICKSWITCH_SWITCH:
    case QUICKSWITCH_DISMISS:
        {
#ifdef  DEBUG_QS
        printf("(%s)\n", (LONGFROMMP(mp1)==QUICKSWITCH_SWITCH ? "SWITCH" : "DISMISS"));
#endif  /* DEBUG_QS */
                                        /* Hide QuickSwitch window */
        WinShowWindow(pHP->hwndSwitchFrame, FALSE);
        pHP->hwndZOrder[ZORDER_QUICKSWITCH]=NULLHANDLE;
                                        /* Pass the message back to the working thread by adding the
                                           frame window's handle to switch to in mp2. The thread will
                                           then switch to that session (PM and non-PM) */
        WinQuerySwitchEntry(hswitchCurrentSession, &swcntrlCurrentSession);
        if(hswitchCurrentSession)
            WinPostMsg(pHP->hwndWorkingThread, WM_QUICKSWITCH, mp1, MPFROMHWND(swcntrlCurrentSession.hwnd));
        }
        break;

    case QUICKSWITCH_NEXT:
    case QUICKSWITCH_PREVIOUS:
        {
#ifdef  DEBUG_QS
        printf("(%s)\n", (LONGFROMMP(mp1)==QUICKSWITCH_NEXT ? "NEXT" : "PREVIOUS"));
#endif  /* DEBUG_QS */
        hswitchCurrentSession=(HSWITCH)PVOIDFROMMP(mp2);
                                        /* If we invoke QuickSwitch for the first time (to make it visible)
                                           get the current active frame window's Window List entry */
        if(!WinIsWindowVisible(hwnd))
            {
            SWP     swp;

#ifdef  DEBUG_QS
            printf("QuickSwitch: Showing window\n");
#endif  /* DEBUG_QS */
            memset(&swp, '\0', sizeof(swp));
            swp.fl=SWP_SHOW|SWP_ZORDER;
            swp.hwndInsertBehind=HWND_TOP;
            swp.hwnd=pHP->hwndSwitchFrame;
            WinSetMultWindowPos(pHP->habPc2, &swp, 1);
            pHP->hwndZOrder[ZORDER_QUICKSWITCH]=pHP->hwndSwitchFrame;
            }
                                        /* Force redraw, because the switch request alone does not
                                           invalidate the client area (which then would cause a WM_PAINT) */
        WinPostMsg(hwnd, WM_DRAW, 0, 0);
        }
        break;
    }
    }
    break;

case WM_BUTTON1DOWN:
case WM_BUTTON2DOWN:
case WM_BUTTON3DOWN:
                                        /* Prevent the QuickSwitch window to get the focus, but ensure
                                           that a click cancels the QuickSwitch window */
    WinPostMsg(pHP->hwndWorkingThread, WM_QUICKSWITCH, MPFROMLONG(QUICKSWITCH_DISMISS), NULL);
    break;

default:                                /* Default window procedure must be called */
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This procedure is the PC/2 Bubble Help window procedure.                             *
\*--------------------------------------------------------------------------------------*/
MRESULT EXPENTRY PC2_BubbleHelpWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Frame window rectangle */
static RECTL    rectlFrame;
                                        /* Client window rectangle */
static RECTL    rectlClient;
                                        /* Current text to display in BubbleHelp window */
static CHAR     szCurrentText[MAXNAMEL+4]="";
                                        /* Current hotspot on screen to display BubbleHelp
                                           around horizontally centered */
static POINTL   pointlHotspot;
                                        /* Rectangle for border, icon, text,... */
RECTL           rectlTemp;

switch(msg)
{
case WM_CREATE:
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));

case WM_DESTROY:
                                        /* Cleanup other windows dependencies */
    pHP->hwndBubbleHelpFrame=pHP->hwndBubbleHelpClient=NULLHANDLE;
                                        /* Destroy BubbleHelp's presenation space and device context */
    GpiDestroyPS(pHP->hpsBubbleHelp);
    DevCloseDC(pHP->hdcBubbleHelp);
    pHP->hpsBubbleHelp=pHP->hdcBubbleHelp=NULLHANDLE;
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));

case WM_PAINT:
    {
    HPS     hpsClient;
                                        /* Get a cached presentation space */
    hpsClient=WinBeginPaint(hwnd, NULLHANDLE, &rectlClient);
    WinSendMsg(hwnd, WM_DRAW, (MPARAM)hpsClient, NULL);
    WinEndPaint(hpsClient);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_DRAW, HPS hpsDraw, RECTL *prectlDraw                                      *
\*                                                                                      */
case WM_DRAW:
/*                                                                                      *\
 * This undocumented message is used by PM controls to allow them to paint itself into  *
 * a provided presentation space. If rectlDraw is 0, we have to draw the entire control.*
\*                                                                                      */
    {
    HPS         hpsDraw=(HPS)LONGFROMMP(mp1);
    ULONG       ulError=NO_ERROR;

                                        /* Query client area */
    WinQueryWindowRect(hwnd, &rectlClient);
                                        /* If no presentation space is passed (from WM_PAINT), use
                                           permanent presentation space */
    if(!hpsDraw)
        hpsDraw=pHP->hpsBubbleHelp;
                                        /* Fill invalid rectangle with dialog background color */
    WinFillRect(hpsDraw, &rectlClient, SYSCLR_DIALOGBACKGROUND);
                                        /* Query client rectangle */
    WinQueryWindowRect(hwnd, &rectlTemp);
                                        /* Draw outer 3D border (QUICKSWITCH_BORDER pixel width) */
    WinDrawBorder(hpsDraw, &rectlTemp, BUBBLEHELP_BORDER, BUBBLEHELP_BORDER, 0, 0, DB_RAISED);
                                        /* Draw inner 3D border (QUICKSWITH_BORDER pixel width) */
    rectlTemp.xLeft+=(BUBBLEHELP_BORDER+BUBBLEHELP_SPACE);
    rectlTemp.yBottom+=(BUBBLEHELP_BORDER+BUBBLEHELP_SPACE);
    rectlTemp.xRight-=(BUBBLEHELP_BORDER+BUBBLEHELP_SPACE);
    rectlTemp.yTop-=(BUBBLEHELP_BORDER+BUBBLEHELP_SPACE);
    WinDrawBorder(hpsDraw, &rectlTemp, BUBBLEHELP_BORDER, BUBBLEHELP_BORDER, 0, 0, DB_DEPRESSED);
                                        /* Draw QuickSwitch text (the title of the next session to
                                           quickswitch to after erasing the previous text (which
                                           is likely to be present)) */
    rectlTemp=rectlClient;
    WinDrawText(hpsDraw, -1, szCurrentText, &rectlTemp, CLR_BLACK, 0,
        DT_CENTER|DT_VCENTER);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SETPRESPARAMFONT, ULONG ulPresentationParameter, FATTRS *pfattrsFont      *
\*                                                                                      */
case WM_SETPRESPARAMFONT:
/*                                                                                      *\
 * Modified WM_PRESPARAMCHANGED message for font setting.                               *
\*                                                                                      */
    if((ULONG)mp1==PP_FONTNAMESIZE)
        {
#ifdef  DEBUG_BH
        printf("BubbleHelp: WM_SETPRESPARAMFONT\n");
#endif  /* DEBUG_BH */
                                        /* Set SessionBar window font */
        SetPresParamFont(pHP->hpsBubbleHelp, (FATTRS *)PVOIDFROMMP(mp2), LCID_FONT);
                                        /* Repaint SessionBar window */
        WinInvalidateRect(hwnd, NULL, FALSE);
        }
    break;

/*                                                                                      *\
 * Syntax: WM_BUBBLEHELP, ULONG ulFlag                                                  *
\*                                                                                      */
case WM_BUBBLEHELP:
/*                                                                                      *\
 * This message is posted to hide or show the BubbleHelp window.                        *
\*                                                                                      */
    {
#ifdef  DEBUG_BH
    printf("BubbleHelp: WM_BUBBLEHELP ");
#endif  /* DEBUG_BH */
    switch(LONGFROMMP(mp1))
    {
    case BUBBLEHELP_HIDE:
        {
#ifdef  DEBUG_BH
        printf("HIDE\n");
#endif  /* DEBUG_QS */
                                        /* Hide BubbleHelp window */
        WinShowWindow(pHP->hwndBubbleHelpFrame, FALSE);
        pHP->hwndZOrder[ZORDER_BUBBLEHELP]=NULLHANDLE;
        }
        break;

    case BUBBLEHELP_SHOW:
        {
        SWP     swp;

#ifdef  DEBUG_BH
        printf("MOVE\n");
#endif  /* DEBUG_BH */
                                        /* Move and show (if applicable) BubbleHelp window */
        memset(&swp, '\0', sizeof(swp));
        swp.fl=SWP_SHOW|SWP_MOVE|SWP_SIZE|SWP_ZORDER;
        swp.cx=(rectlFrame.xRight-rectlFrame.xLeft);
        swp.cy=(rectlFrame.yTop-rectlFrame.yBottom);
        swp.x=pointlHotspot.x-(swp.cx>>1);
        if(pointlHotspot.y>0)
            swp.y=pointlHotspot.y;
        else
            swp.y=(0-pointlHotspot.y)-swp.cy;
        if(swp.x<0) swp.x=0;
        if(swp.x>pHP->swpScreen.cx) swp.x=pHP->swpScreen.cx-swp.cx;
        if(swp.y<0) swp.y=0;
        if((swp.y+swp.cy)>pHP->swpScreen.cy) swp.y=pHP->swpScreen.cy-swp.cy;
        if(swp.cx<=(BUBBLEHELP_BORDER+BUBBLEHELP_SPACE+BUBBLEHELP_BORDER+2*BUBBLEHELP_SPACE)<<1)
            swp.fl&=(~SWP_SHOW);
        swp.hwndInsertBehind=HWND_TOP;
        swp.hwnd=pHP->hwndBubbleHelpFrame;
        WinSetMultWindowPos(pHP->habPc2, &swp, 1);
        pHP->hwndZOrder[ZORDER_BUBBLEHELP]=pHP->hwndBubbleHelpFrame;
                                        /* Force redraw, because the switch request alone does not
                                           invalidate the client area (which then would cause a WM_PAINT) */
        WinPostMsg(hwnd, WM_DRAW, 0, 0);
        }
        break;

    case BUBBLEHELP_RELOCATE:
#ifdef  DEBUG_BH
        printf("RELOCATE\n");
#endif  /* DEBUG_BH */
                                        /* Save current hotspot to possibly unhide BubbleHelp
                                           around */ 
        pointlHotspot.x=(LONG)(SHORT)SHORT1FROMMP(mp2);
        pointlHotspot.y=(LONG)(SHORT)SHORT2FROMMP(mp2);
        break;

    case BUBBLEHELP_TEXT:
        {
        POINTL       pointlFnt[TXTBOX_COUNT];

#ifdef  DEBUG_BH
        printf("TEXT: \"%s\"\n", PVOIDFROMMP(mp2));
#endif  /* DEBUG_BH */
                                        /* Save current text to possibly drawn in BubbleHelp */
        strcpy(szCurrentText, (CHAR *)mp2);
                                        /* Get dimensions of window's name */
        GpiQueryTextBox(pHP->hpsBubbleHelp, strlen(szCurrentText), szCurrentText, TXTBOX_COUNT, pointlFnt);
        rectlFrame.xLeft=0;
        rectlFrame.xRight=(BUBBLEHELP_BORDER+BUBBLEHELP_SPACE+BUBBLEHELP_BORDER+2*BUBBLEHELP_SPACE)<<1;
        rectlFrame.yBottom=0;
        rectlFrame.yTop=(BUBBLEHELP_BORDER+BUBBLEHELP_SPACE+BUBBLEHELP_BORDER+2*BUBBLEHELP_SPACE)<<1;
        rectlFrame.xRight+=pointlFnt[TXTBOX_CONCAT].x;
        rectlFrame.yTop+=(pointlFnt[TXTBOX_TOPRIGHT].y-pointlFnt[TXTBOX_BOTTOMLEFT].y);
        free(mp2);
        }
        break;
    }
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefWindowProc(hwnd, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

