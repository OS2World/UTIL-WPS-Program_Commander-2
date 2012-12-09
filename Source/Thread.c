/***********************************************************************\
 *                               PC2.c                                 *
 *             Copyright (C) by Stangl Roman, 1994, 2000               *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * Thread.c     PC/2's object window working thread.                   *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#include        "PC2.h"                 /* User include files */
#include        "Error.h"

#define         _FILE_  "Thread.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;

HPS             hpsClient;              /* PC/2's client area presentation space */

                                        /* PC/2's working thread */
void _Optlink   PC2_Thread(void *ThreadArg)
{
THREADDATA      *pThreadData=(THREADDATA *)ThreadArg;
TIB             *ptib;                  /* Thread information block */
PIB             *ppib;                  /* Process information block */
EXCEPTIONRECORD excprecThread;          /* Exception handler registration for Thread procedure */
EXCEPTIONRECORD excprecMsgLoop;         /* Exception handler registration for message loop */

/*                                                                                      *\
 * Register exception handler for Thread procedure. This one does not use setjmp to     *
 * continue, while another exception handler around the message dispatching loop does   *
 * use setjmp to continue work after certain exceptions.                                *
\*                                                                                      */
memset(&excprecThread, 0, sizeof(EXCEPTIONRECORD));
excprecThread.ulSignature=BLDLEVEL_SIGNATURE;
RegisterExceptionHandler(TRUE, (EXCEPTIONREGISTRATIONRECORD *)&excprecThread, pHP->ucFilenameTRP);
/*                                                                                      *\
 * Save thread info block                                                               *
\*                                                                                      */
DosGetInfoBlocks(&ptib, &ppib);
if(pThreadData->ulThreadId==ID_WORKINGTHREAD)
    {
#ifdef  DEBUG
    printf("%s Starting\n", "WTrd");
    printf("Stack Wtrd : TID %d PTID %08X Base: %08X  Limit: %08X\n",
        ptib->tib_ptib2->tib2_ultid,
        ptib,
        ptib->tib_pstack,
        ptib->tib_pstacklimit);
#endif  /* DEBUG */
    pHP->pTibWorkingThread=ptib;
    }
else if(pThreadData->ulThreadId==ID_IOTHREAD)
    {
#ifdef  DEBUG
    printf("%s Starting\n", "ITrd");
    printf("Stack Itrd : TID %d PTID %08X Base: %08X  Limit: %08X\n",
        ptib->tib_ptib2->tib2_ultid,
        ptib,
        ptib->tib_pstack,
        ptib->tib_pstacklimit);
#endif  /* DEBUG */
    pHP->pTibIOThread=ptib;
    }
else if(pThreadData->ulThreadId==ID_NETTHREAD)
    {
#ifdef  DEBUG
    printf("%s Starting\n", "NTrd");
    printf("Stack Itrd : TID %d PTID %08X Base: %08X  Limit: %08X\n",
        ptib->tib_ptib2->tib2_ultid,
        ptib,
        ptib->tib_pstack,
        ptib->tib_pstacklimit);
#endif  /* DEBUG */
    pHP->pTibNetThread=ptib;
    }
while(TRUE)
{
                                        /* Initialize anchor block and message queue */
    if(WinStartUp(&pThreadData->habThread, &pThreadData->hmqThread)==FALSE)
        {
        USR_ERR(pHP->hwndFrame, HELP_CREATEWINDOW, MB_ERROR|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
            "Initializing thread's PM environment failed, PC/2 can't continue. You may have run out "\
            "of resources, close some windows or applications and retry. Exiting...");
                                        /* On error shut down PC/2 */
        pHP->ulRuntimeFlag|=PC2EMERGENCYEXIT;
        WinPostMsg(pHP->hwndClient, WM_QUIT, NULL, NULL);
        break;
        }
                                        /* Avoid posting WM_QUIT messages to our working thread's message
                                           queue during shutdown. The main thread has to post WM_QUITTHREAD
                                           before this message queue reenables to receive WM_QUIT */
    WinCancelShutdown(pThreadData->hmqThread, TRUE);
    if(!WinRegisterClass(               /* Register window class */
        pThreadData->habThread,         /* Handle of anchor block */
        (PSZ)PC2_CLASSNAME_THREAD,      /* Window class name */
                                        /* Address of window procedure */
        (PFNWP)pThreadData->pfnwpThread,
        CS_SIZEREDRAW | CS_SAVEBITS,
        4))                             /* Extra window words (reserve QWL_USER) */
        {
        PM_ERR(pThreadData->habThread, pHP->hwndFrame, HELP_CREATEWINDOW, MB_ERROR|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
            "Initializing thread's PM environment failed, PC/2 can't continue. You may have run out "\
            "of resources, close some windows or applications and retry. Exiting...");
                                        /* On error shut down PC/2 */
        pHP->ulRuntimeFlag|=PC2EMERGENCYEXIT;
        WinPostMsg(pHP->hwndClient, WM_QUIT, NULL, NULL);
        break;
        }
    pThreadData->hwndThread=WinCreateWindow(
        HWND_OBJECT,                    /* Parent window */
        PC2_CLASSNAME_THREAD,           /* Window class */
        NULL,                           /* Window text */
        0,                              /* Window style */
                                        /* Windos position & size */
        0, 0, 0, 0,
        HWND_OBJECT,                    /* Owner window */
        HWND_BOTTOM,                    /* Sibling window */
        ID_PC2MAINWINDOW,               /* Window ID */
        pThreadData,                    /* Control data */
        NULL);                          /* Presentation parameters */
                                        /* Initialize thread */
#ifdef  DEBUG
    if(pThreadData->ulThreadId==ID_WORKINGTHREAD)
        printf("%s Running ", "WTrd");
    else if(pThreadData->ulThreadId==ID_IOTHREAD)
        printf("%s Running ", "ITrd");
    else if(pThreadData->ulThreadId==ID_NETTHREAD)
        printf("%s Running ", "NTrd");
    printf("TID %ld Window 0x%08X@0x%08p HMQ 0x%08X\n",
        (int)ptib->tib_ptib2->tib2_ultid, (int)pThreadData->hwndThread,
        (int)pThreadData->pfnwpThread, (int)pThreadData->hmqThread);
#endif  /* DEBUG */
    WinPostMsg(pThreadData->hwndThread, WM_INITTHREAD, NULL, NULL);
/*                                                                                      *\
 * Here we loop dispatching the messages.... This message loop processing is protected  *
 * by an exception handler that allows to continue work by using setjmp in case of      *
 * certain exceptions (e.g. if the thread was killed after a hang by DosKillThread/()   *
 * in the main thread.                                                                  *
\*                                                                                      */
                                        /* Prepare exception handler */
    memset(&excprecMsgLoop, 0, sizeof(EXCEPTIONRECORD));
    excprecMsgLoop.ulSignature=BLDLEVEL_SIGNATURE;
    excprecMsgLoop.ulStatus=EXCEPTIONRECORD_JMPBUF;
    RegisterExceptionHandler(TRUE, (EXCEPTIONREGISTRATIONRECORD *)&excprecMsgLoop, pHP->ucFilenameTRP);
                                        /* Dispatch messages to window procedure */
    if(pThreadData->ulThreadId==ID_WORKINGTHREAD)
        {
                                        /* When our Exception Handler returns via longjmp()
                                           the stack is unwound and control returns here again.
                                           As the main thread is the owner of the Exception
                                           Handler semaphore we can't do a DosReleaseMutexSem()
                                           here but have to request that from the main thread */
        if(setjmp(excprecMsgLoop.jmpbufReturn)!=0)
            {
#ifdef  DEBUG
            printf("WTrd: ");
            printf("returned from XCPT_ASYNC_PROCESS_TERMINATE\n");
#endif  /* DEBUG */
            WinPostMsg(pHP->hwndClient, WM_EXCPSEMDECREMENT, NULL, NULL);
            WinPostMsg(pHP->hwndClient, WM_COMMAND, MPFROMSHORT(ID_UNHANG), MPFROMSHORT(CMDSRC_MENU));
            }
        pHP->ulAliveMsgWTrd=0;
        if(!(pHP->ulRuntimeFlag & (QUITFROMWINDOWLIST|QUITFROMSHUTDOWN)))
            while(WinGetMsg(pThreadData->habThread, &pThreadData->qmsgThread, 0, 0, 0))
            {
            MESSAGEPOINT(WTrd, pThreadData->qmsgThread.msg);
            WinDispatchMsg(pThreadData->habThread, &pThreadData->qmsgThread);
            MESSAGEPOINT(WTrd, 0);
            }
        }
    else if(pThreadData->ulThreadId==ID_IOTHREAD)
        {
        if(setjmp(excprecMsgLoop.jmpbufReturn)!=0)
            {
#ifdef  DEBUG
            printf("ITrd: ");
            printf("returned from XCPT_ASYNC_PROCESS_TERMINATE\n");
#endif  /* DEBUG */
            WinPostMsg(pHP->hwndClient, WM_EXCPSEMDECREMENT, NULL, NULL);
            WinPostMsg(pHP->hwndClient, WM_COMMAND, MPFROMSHORT(ID_UNHANG), MPFROMSHORT(CMDSRC_MENU));
            }
        pHP->ulAliveMsgITrd=0;
        if(!(pHP->ulRuntimeFlag & (QUITFROMWINDOWLIST|QUITFROMSHUTDOWN)))
            while(WinGetMsg(pThreadData->habThread, &pThreadData->qmsgThread, 0, 0, 0))
            {
            MESSAGEPOINT(ITrd, pThreadData->qmsgThread.msg);
            WinDispatchMsg(pThreadData->habThread, &pThreadData->qmsgThread);
            MESSAGEPOINT(ITrd, 0);
            }
        }
    else if(pThreadData->ulThreadId==ID_NETTHREAD)
        {
        if(setjmp(excprecMsgLoop.jmpbufReturn)!=0)
            {
#ifdef  DEBUG
            printf("NTrd: ");
            printf("returned from XCPT_ASYNC_PROCESS_TERMINATE\n");
#endif  /* DEBUG */
            WinPostMsg(pHP->hwndClient, WM_EXCPSEMDECREMENT, NULL, NULL);
            WinPostMsg(pHP->hwndClient, WM_COMMAND, MPFROMSHORT(ID_UNHANG), MPFROMSHORT(CMDSRC_MENU));
            }
        pHP->ulAliveMsgNTrd=0;
        if(!(pHP->ulRuntimeFlag & (QUITFROMWINDOWLIST|QUITFROMSHUTDOWN)))
            while(WinGetMsg(pThreadData->habThread, &pThreadData->qmsgThread, 0, 0, 0))
            {
            MESSAGEPOINT(ITrd, pThreadData->qmsgThread.msg);
            WinDispatchMsg(pThreadData->habThread, &pThreadData->qmsgThread);
            MESSAGEPOINT(ITrd, 0);
            }
        }
                                        /* Remove exception handler */
    RegisterExceptionHandler(FALSE, (EXCEPTIONREGISTRATIONRECORD *)&excprecMsgLoop, NULL);
                                        /* Ignore outstanding WM_QUIT messages, but accept them
                                           at any time */
    break;
}
                                        /* Inform main thread of immediate termination of this thread,
                                           so that it can wait for all threads to terminate before it
                                           terminates itself */
WinPostMsg(pHP->hwndClient, WM_QUITTHREAD, MPFROMLONG(pThreadData->ulThreadId), NULL);
                                        /* Close window */
WinDestroyWindow(pThreadData->hwndThread);
WinDestroyMsgQueue(pThreadData->hmqThread);
WinTerminate(pThreadData->habThread);
#ifdef  DEBUG
if(pThreadData->ulThreadId==ID_WORKINGTHREAD)
    printf("%s: terminating\n", "WTrd");
else if(pThreadData->ulThreadId==ID_IOTHREAD)
    printf("%s: terminating\n", "ITrd");
else if(pThreadData->ulThreadId==ID_NETTHREAD)
    printf("%s: terminating\n", "NTrd");
#endif  /* DEBUG */
/*                                                                                      *\
 * Deregister exception handler for thread                                              *
\*                                                                                      */
RegisterExceptionHandler(FALSE, (EXCEPTIONREGISTRATIONRECORD *)&excprecThread, NULL);
_endthread();                           /* Terminate C thread code */
DosExit(EXIT_THREAD, 0UL);              /* Terminate thread */
}

/*--------------------------------------------------------------------------------------*\
 * This procedure is the PC/2 I/O thread window procedure (is an object window).        *
\*--------------------------------------------------------------------------------------*/
MRESULT EXPENTRY PC2_IOThreadWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Get thread's window word */
THREADDATA      *pThreadData=(THREADDATA *)WinQueryWindowULong(hwnd, QWL_USER);
static ULONG    ulClosingThread=FALSE;  /* Set to TRUE after WM_QUITTHREAD was received */
                                        /* Count the number of (recursive) WM_POPUPMENUSETICON posted to
                                           ourselfes, and when we've done our current submenu */
static ULONG    ulSubmenusIncomplete=0;
static ULONG    ulTimerTicks=0;         /* Timer ticks (about every TIMERINTERVALLMS milliseconds) */
                                        /* High speed (100ms) timer */
static ULONG    ulTimer=0;
                                        /* Window handle for delayed Sliding Focus */
static HWND     hwndActiveWindowClient=0;
                                        /* Parameter mp1 of WM_MOUSEMOVE message which will
                                           be slightly modified when posting again to hook,
                                           to allow the focus change by the Sliding Focus
                                           in the hook */
static MPARAM   mp1SlidingFocus=(VOID *)-1;
                                        /* Current delay counter for Dynamic SessionBar */
static ULONG    ulDynamicSessionBarCount=0;
                                        /* Current delay counter for Sliding Focus */
static ULONG    ulSlidingFocusCount=0;
                                        /* Window handle for delayed Dynamic Menuselection */
static HWND     hwndCurrentMenu=0;
                                        /* Parameter mp1 of WM_MOUSEMOVE message which will
                                           be slightly modified when posting again to hook,
                                           as the menuentry's MENUITEM can only be queried
                                           in the thread context of the menu window and not
                                           in PC/2's */
static MPARAM   mp1DynamicMenu=(VOID *)-1;
                                        /* Current delay counter for Dynamic Menuselection */
static ULONG    ulDynamicMenuCount=0;
                                        /* Mouse position for delayed Virtual Desktop switching */
static ULONG    ulMousePos=0;
                                        /* Move flag for delayed Virtual Desktop */
static ULONG    ulMoveFlag=0;
                                        /* Current delay counter for Virtual Desktop */
static ULONG    ulVirtualDesktopCount=0;
                                        /* Overview flag for Dynamic Overview */
static ULONG    ulOverviewFlag=0;
                                        /* Current delay counter for Overview unhide */
static ULONG    ulOverviewUnhideCount=0;

switch(msg)
{
case WM_CREATE:
    if(pHP->ulDebug>=DEBUG_LOW)
        printf("ITrd: WM_CREATE\n");
    pThreadData=PVOIDFROMMP(mp1);
                                        /* Save cache of I/O thread's window handle */
    pHP->hwndIOThread=hwnd;
                                        /* Set the passed control structure into the QWL_USER window word */
    WinSetWindowULong(hwnd, QWL_USER, (ULONG)pThreadData);
    break;

case WM_TIMER:
/*                                                                                      *\
 * Message posted by the main thread or by the high speed (100ms) timer.                *
\*                                                                                      */
                                        /* Check for the high-frequency timer (every 100 ms) */
    if(ulTimer==LONGFROMMP(mp1))
        {
                                        /* If the delay for the timed Overview Window unhiding has
                                           expired, request it to unhide itself (and as the Overview
                                           knows that the timer here has unhidden it it will ensure
                                           to hide itself again when losing this input focus) */
        if(ulOverviewFlag!=0)
            {
            ulOverviewUnhideCount++;
            if(ulOverviewUnhideCount>=pHP->ulOverviewDelay)
                {
                WinPostMsg(pHP->hwndClient, WM_OVERVIEWUNHIDEDELAY, NULL, NULL);
                ulOverviewFlag=0;
                ulOverviewUnhideCount=0;
                }
            }
                                        /* If the delay for the Dynamic SessionBar has expired,
                                           then request it to unhide */
        if(pHP->ulSessionBarFlag & SB_UNHIDEDELAY)
            {
            ulDynamicSessionBarCount++;
            if((ulDynamicSessionBarCount>=pHP->ulSessionBarDelay) ||
                ((pHP->ulSessionBarFlag & SB_LASTTOP) && (pHP->swpPC2Session.y<(pHP->swpScreen.cy-1))) ||
                ((pHP->ulSessionBarFlag & SB_LASTBOTTOM) && (pHP->swpPC2Session.y>(-pHP->swpPC2Session.cy+1))))
                {
                pHP->ulSessionBarFlag&=~SB_UNHIDEDELAY;
                WinPostMsg(hwnd, WM_SESSIONBARUNHIDE,
                    MPFROMLONG(SB_LASTBOTTOM), MPFROMLONG(TRUE));
                ulDynamicSessionBarCount=0;
                }
            }
                                        /* If the delay for the Sliding Focus has expired, then
                                           set the focus to the current window */
        if(hwndActiveWindowClient!=0)
            {
            ulSlidingFocusCount++;
            if(ulSlidingFocusCount>=pHP->ulSlidingFocusDelay)
                {
                WinPostMsg(hwndActiveWindowClient, WM_MOUSEMOVE,
                    mp1SlidingFocus, MPFROM2SHORT((SHORT)HT_SLIDINGFOCUSDELAY, KC_NONE));
                hwndActiveWindowClient=0;
                ulSlidingFocusCount=0;
                }
            }
                                        /* If the delay for the Dynamic Menuselection has expired,
                                           then post a slightly modified WM_MOUSEMOVE message to
                                           the input hook. We can't select the menuentry with a
                                           MM_SELECTITEM message here, as we may recursively have to
                                           chain into submenus which required us to query the MENUITEM
                                           to access the submenu window handle. And querying the
                                           MENUITEM is thread context sensitive, that is one will get
                                           access violations when not doing that in the process the
                                           menu was created. Posting another WM_MOUSEMOVE to the menu
                                           allow us to trap it in the input hook, and that hook is
                                           running the the menu's thread context */
        if(hwndCurrentMenu!=0)
            {
            ulDynamicMenuCount++;
            if(ulDynamicMenuCount>=pHP->ulDynamicMenuDelay)
                {
                WinPostMsg(hwndCurrentMenu, WM_MOUSESELECT,
                    mp1DynamicMenu, MPFROM2SHORT(HT_DYNAMICMENUDELAY, KC_NONE));
                hwndCurrentMenu=0;
                mp1DynamicMenu=(VOID *)-1;
                ulDynamicMenuCount=0;
                }
            }
                                        /* If the delay for the timed Virtual Desktop switching has
                                           expired, then post a WM_MOVEREQUEST message with the saved
                                           parameters we got from the PC2HOOK.DLL */
        if(ulMoveFlag!=0)
            {
            ulVirtualDesktopCount++;
            if(ulVirtualDesktopCount>=pHP->ulVirtualDesktopDelay)
                {
                WinPostMsg(pHP->hwndClient, WM_MOVEREQUEST, MPFROMLONG(ulMousePos), MPFROMLONG(ulMoveFlag));
                ulMousePos=0;
                ulMoveFlag=0;
                }
            }
        break;
        }
                                        /* Reset alive timer main thread keeps counting */
    pHP->ulAliveTimerITrd=0;
                                        /* Update timer */
    ulTimerTicks++;
    if(pHP->ulDebug>=DEBUG_LOW)
        if((ulTimerTicks % 10)==0)
            {
            DosBeep(3000, 50);
            printf("ITrd: WM_TIMER\n");
            }
    break;

/*                                                                                      *\
 * Syntax: WM_INITTHREAD, NULL, NULL                                                    *
\*                                                                                      */
case WM_INITTHREAD:
/*                                                                                      *\
 * Message posted by thread to its message queue immediately before entering the        *
 * message loop, to allow the thread to initialize its thread specific data.            *
\*                                                                                      */
    {
#ifdef  DEBUG
    TIB     *ptib;
    PIB     *ppib;

    CHECKPOINT(ITrd, 0x0100);
    DosGetInfoBlocks(&ptib, &ppib);
    printf("ITrd: WM_INITTHREAD TID %ld\n", (int)ptib->tib_ptib2->tib2_ultid);
#endif  /* DEBUG */
#ifdef  DEBUG_ITRD
    printf("ITrd: WM_INITTHREAD\n");
#endif  /* DEBUG_ITRD */
    DosSleep(50);
                                        /* Start the high frequency (100ms) timer for delayed actions */
    for(ulTimer=0; ulTimer<=TID_USERMAX; ulTimer++)
        if(WinStartTimer(pThreadData->habThread, hwnd, ulTimer, 100))
            break;
                                        /* Post this message as the last one into the message queue,
                                           to inform client window that I/O thread is ready then */
    WinPostMsg(hwnd, WM_THREADSTARTUPREADY, MPFROMLONG(ID_IOTHREAD), NULL);
    CHECKPOINT(ITrd, 0x0100);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_QUITTHREAD, ULONG ulThreadId, NULL                                        *
\*                                                                                      */
case WM_QUITTHREAD:
/*                                                                                      *\
 * It is about time to terminate the I/O thread upon request of the main thread.        *
 * We unlock our thread's message queue to be able to receive WM_QUIT again and we post *
 * WM_QUIT to our message queue afterwards.                                             *
\*                                                                                      */
#ifdef  DEBUG_ITRD
    printf("ITrd: WM_QUITTHREAD\n");
#endif  /* DEBUG_ITRD */
    CHECKPOINT(ITrd, 0x0200);
                                        /* Stop high frequence (100ms) timer */
    WinStopTimer(pThreadData->habThread, hwnd, ulTimer);
                                        /* Yep, time to close */
    ulClosingThread=TRUE;
                                        /* Wait with the close request until we've done
                                           our work */
    if(ulSubmenusIncomplete!=0)
        WinPostMsg(hwnd, WM_QUITTHREAD, MPFROMLONG(ID_IOTHREAD), NULL);
    else
        {
                                        /* Cleanup the Environment Spaces linked list allocated */
        FreeEnvironmentSpaces(pHP->pEnvironmentSpaces);
        pHP->pEnvironmentSpaces=NULL;
                                        /* Reenable our message queue to receive future
                                           WM_QUIT messages */
        WinCancelShutdown(pThreadData->hmqThread, FALSE);
                                        /* And here WM_QUIT comes */
        WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
        }
    CHECKPOINT(ITrd, 0x0200);
    break;

/*                                                                                      *\
 * Syntax: WM_THREADSTARTUPREADY, ULONG ulThreadId, NULL                                *
\*                                                                                      */
case WM_THREADSTARTUPREADY:
/*                                                                                      *\
 * I/O thread is now ready                                                              *
\*                                                                                      */
#ifdef  DEBUG_ITRD
    printf("ITrd: WM_THREADSTARTUPREADY\n");
#endif  /* DEBUG_ITRD */
                                        /* Inform client window that working thread is now ready */
    WinPostMsg(pHP->hwndClient, WM_THREADSTARTUPREADY, MPFROMLONG(ID_IOTHREAD), NULL);
    break;

/*                                                                                      *\
 * Syntax: WM_LOADPOPUPMENU, MENUDATA pMenuDataLoad, UCHAR *pucProfile                  *
\*                                                                                      */
case WM_LOADPOPUPMENU:
/*                                                                                      *\
 * Open the profile for reading the linked list containing the popup menu data. If the  *
 * profile can't be opened, the file is assumed to be empty so the popup menu is empty. *
 * Loading the Popup Menu may take quite a while when the user requested to include the *
 * menuentries icons in the Popup Menu.                                                 *
\*                                                                                      */
    {
    MENUDATA    *pMenuDataLoad=PVOIDFROMMP(mp1);
    UCHAR       *pucProfile=PVOIDFROMMP(mp2);
    ULONG       ulError=0;

#ifdef  DEBUG_ITRD
    printf("ITrd: WM_LOADPOPUPMENU\n");
#endif  /* DEBUG_ITRD */
    if((pHP->Pc2Profile=fopen(pucProfile, "r"))==NULL)
        {
        CHECKPOINT(ITrd, 0x0300);
                                        /* Allocate an empty MENUDATA structure used as
                                           the first element of linked list */
        if(pMenuDataLoad==pHP->pPopupMenu)
            {
            pHP->pPopupMenu=pMenuDataLoad=AllocateMenuData(NULL);
                                        /* Assume the Configuration Dialog isn't part
                                           of the profile */
            pHP->ulRuntimeFlag|=DISPLAYCONFIGDIALOG;
            }
        ulError=(ULONG)"Cannot open confguration file - assuming empty file";
        }
    else
        {
        UCHAR   Buffer[64];

        CHECKPOINT(ITrd, 0x0301);
                                        /* Allocate an empty MENUDATA structure used as
                                           the first element of linked list, if we're loading
                                           the whole Popup-Menu not just a menuentry */
        if(pMenuDataLoad==pHP->pPopupMenu)
            {
            pHP->pPopupMenu=pMenuDataLoad=AllocateMenuData(NULL);
                                        /* Assume the Configuration Dialog isn't part
                                           of the profile */
            pHP->ulRuntimeFlag|=DISPLAYCONFIGDIALOG;
            }
        fgets(Buffer, sizeof(Buffer), pHP->Pc2Profile);
        while(!feof(pHP->Pc2Profile))
            {
            if(strstr(Buffer, "PROFILE START"))
                {
                                        /* Load the rest by calling a recursive procedure */
                LoadMenu(pHP->Pc2Profile, pMenuDataLoad, TRUE);
                EnsureUniqueStrings(pHP->ucStickyWindows);
                break;
                }
            fgets(Buffer, sizeof(Buffer), pHP->Pc2Profile);
            }
        fclose(pHP->Pc2Profile);
        pHP->Pc2Profile=NULL;
                                        /* Be sure that the Popup Menu exists, as the user may
                                           close PC/2 immediately after starting it */
        if((pMenuDataLoad) && (pMenuDataLoad->Item==ENTRYEMPTY))
            ulError=(ULONG)"Cannot open confguration file - assuming empty file";
        }
                                        /* I'm not sure if that doesn't cause problems, when the I/O
                                           thread loads the PM menu structures as we have created the
                                           menu in this thread. Up to now it greatly reduced the SIQ
                                           blocking for large Popup Menus without a known problem */
                                        /*
                                           SetPMMenu(pHP->hwndPopupMenu, pHP->pPopupMenu);
                                         */
    CHECKPOINT(ITrd, 0x0302);
                                        /* We've read the Popup-Menu, inform main thread including
                                           possible errors */
    if(pMenuDataLoad!=pHP->pPopupMenu)
        {
        free(pucProfile);
        WinPostMsg(pHP->hwndConfigDialog, WM_LOADPOPUPMENU, MPFROMP(pMenuDataLoad), MPFROMLONG(ulError));
        }
    else
        WinPostMsg(pHP->hwndClient, WM_LOADPOPUPMENU, MPFROMLONG(ID_IOTHREAD), MPFROMLONG(ulError));
    CHECKPOINT(ITrd, 0x0300);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SAVEPOPUPMENU, MENUDATA *pMenuDataSave, UCHAR *pucProfile                 *
\*                                                                                      */
case WM_SAVEPOPUPMENU:
/*                                                                                      *\
 * Process the request to save the current Popup-Menu structure to the configuration    *
 * file. Return the result to the main thread.                                          *
\*                                                                                      */
    {
    MENUDATA    *pMenuDataSave=PVOIDFROMMP(mp1);
    UCHAR       *pucProfile=PVOIDFROMMP(mp2);
    ULONG       ulError=0;

    CHECKPOINT(ITrd, 0x0400);
#ifdef  DEBUG_ITRD
    printf("ITrd: WM_SAVEPOPUPMENU\n");
#endif  /* DEBUG_ITRD */
    if((pHP->Pc2Profile=fopen(pucProfile, "w"))==NULL)
        ulError=(ULONG)"Cannot open confguration file - changes won't be saved";
    else
        {
        fprintf(pHP->Pc2Profile, "PROFILE START\n");
                                        /* Save the menu linked list */
        SaveMenu(pHP->Pc2Profile, pMenuDataSave);
        fprintf(pHP->Pc2Profile, "PROFILE END\n");
        fclose(pHP->Pc2Profile);
        pHP->Pc2Profile=NULL;
        }
    pHP->Pc2Profile=NULL;
                                        /* We've saved the Popup-Menu, inform main thread including
                                           possible errors */
    if(pMenuDataSave!=pHP->pPopupMenu)
        {
        free(pucProfile);
        WinPostMsg(pHP->hwndConfigDialog, WM_SAVEPOPUPMENU, MPFROMP(pMenuDataSave), MPFROMLONG(ulError));
        }
    else
        WinPostMsg(pHP->hwndClient, WM_SAVEPOPUPMENU, MPFROMLONG(ID_IOTHREAD), MPFROMLONG(ulError));
    CHECKPOINT(ITrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_LOADENVIRONMENTSPACE, ULONG ulThreadId, MPARAM mp2                        *
\*                                                                                      */
case WM_LOADENVIRONMENTSPACE:
/*                                                                                      *\
 * Open the profile for reading the linked list containing the environment spaces       *
 * defined. If the profile can't be opened, the file is assumed to be empty so the list *
 * of predefined environment spaces is empty.                                           *
\*                                                                                      */
    {
    ULONG   ulError=0;

    CHECKPOINT(ITrd, 0x0400);
#ifdef  DEBUG_ITRD
    printf("ITrd: WM_LOADENVIRONMENTSPACE\n");
#endif  /* DEBUG_ITRD */
    if((pHP->Pc2EnvironmentProfile=fopen(PVOIDFROMMP(mp2), "r"))==NULL)
        ulError=(ULONG)"Cannot open Environment Spaces configuration file - assuming empty file";
    else
        {
                                        /* Load the Environment Spaces by calling a procedure */
        pHP->pEnvironmentSpaces=LoadEnvironmentSpaces(pHP->Pc2EnvironmentProfile);
        fclose(pHP->Pc2EnvironmentProfile);
        pHP->Pc2EnvironmentProfile=NULL;
        }
                                        /* We've read the Environment Spaces, inform main thread including
                                           possible errors */
    WinPostMsg(pHP->hwndClient, WM_LOADENVIRONMENTSPACE, MPFROMLONG(ID_IOTHREAD), MPFROMLONG(ulError));
    CHECKPOINT(ITrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SAVEENVIRONMENTSPACE, ULONG ulThreadId, MPARAM mp2                        *
\*                                                                                      */
case WM_SAVEENVIRONMENTSPACE:
/*                                                                                      *\
 * Open the profile for writing the linked list containing the environment spaces       *
 * defined. If the profile can't be opened, changes can't be saved and may be lost.     *
\*                                                                                      */
    {
    ULONG   ulError=0;

    CHECKPOINT(ITrd, 0x0500);
#ifdef  DEBUG_ITRD
    printf("ITrd: WM_SAVEENVIRONMENTSPACE\n");
#endif  /* DEBUG_ITRD */
    if((pHP->Pc2EnvironmentProfile=fopen(PVOIDFROMMP(mp2), "w"))==NULL)
        ulError=(ULONG)"Cannot open environment spaces confguration file - changes won't be saved";
    else
        {
                                        /* Save the environment spaces linked list */
        SaveEnvironmentSpaces(pHP->pEnvironmentSpaces, pHP->Pc2EnvironmentProfile, FALSE);
        fclose(pHP->Pc2EnvironmentProfile);
        pHP->Pc2EnvironmentProfile=NULL;
        }
                                        /* We've written the Environment Spaces, inform main thread including
                                           possible errors */
    WinPostMsg(pHP->hwndClient, WM_SAVEENVIRONMENTSPACE, MPFROMLONG(ID_IOTHREAD), MPFROMLONG(ulError));
    CHECKPOINT(ITrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_POPUPMENUUPDATE, MENUDATA *pMenuData, BOOL bLoadIcon                      *
\*                                                                                      */
case WM_POPUPMENUUPDATE:
/*                                                                                      *\
 * Posted by the main thread to let the I/O thread load or unload the Popup Menu's      *
 * menuentries icons, which may be a very time-consuming task. This will keep the       *
 * system responsive. The best solution would be to prevent simultaneous updates and    *
 * queries of the structure of the linked list by a semaphore (which would have been    *
 * done if I had known all the feature I now support from beginning coding...).         *
\*                                                                                      */
    {
    CHECKPOINT(ITrd, 0x0600);
#ifdef  DEBUG_ITRD
    printf("ITrd: WM_POPUPMENUUPDATE (%s)\n", (SHORT1FROMMP(mp2)==TRUE ? "Load" : "Unload"));
    printf("      Submenus incomplete: %d (!0?)\n", ulSubmenusIncomplete);
#endif  /* DEBUG_ITRD */
    if(pHP->ulRuntimeFlag&(PC2EMERGENCYEXIT|QUITFROMSHUTDOWN))
        WinPostMsg(pHP->hwndClient, WM_POPUPMENUREADY, MPFROMLONG(ID_IOTHREAD), NULL);
    else
                                        /* We start with the root submenu */
        {
        ulSubmenusIncomplete++;
        WinPostMsg(pHP->hwndIOThread, WM_POPUPMENUSETICON, mp1, mp2);
        }
    }
    CHECKPOINT(ITrd, 0x0000);
    break;

/*                                                                                      *\
 * Syntax: WM_POPUPMENUSETICON, MENUDATA *pMenuData, BOOL bLoadIcon                     *
\*                                                                                      */
case WM_POPUPMENUSETICON:
/*                                                                                      *\
 * Posted by the I/O thread to itself with the first menuentry of a submenu as the      *
 * parameter, which again posts this message for all submenus within this submenu, and  *
 * loads all other icons directly.                                                      *
\*                                                                                      */
    {
                                        /* Pointer to root of current submenu within the Popup-Menu */
    MENUDATA        *pMenuDataCurrent=(MENUDATA *)PVOIDFROMMP(mp1);
    CHECKPOINT(ITrd, 0x0700);
#ifdef  DEBUG_ITRD
    printf("ITrd: WM_POPUPMENUSETICON (%d) %s:(%s)\n", ulSubmenusIncomplete, ((MENUDATA *)mp1)->PgmTitle,
        (SHORT1FROMMP(mp2)==TRUE ? "Load" : "Unload"));
#endif  /* DEBUG_ITRD */
    if(!(pHP->ulRuntimeFlag&(PC2EMERGENCYEXIT|QUITFROMSHUTDOWN)))
        {
        while(pMenuDataCurrent!=0)
            {
                                        /* For all items load or unload the icon directly, but prevent
                                           System Error popups by accessing invalid drives */
            if(pMenuDataCurrent->Item!=ENTRYEMPTY)
                InitializeIcon(pMenuDataCurrent, (BOOL)mp2);
                                        /* If it is a submenu, post this message again for the
                                           submenu */
            if(pMenuDataCurrent->Item==ENTRYSUBMENU)
                {
                if(pMenuDataCurrent->Submenu!=0)
                    {
                    ulSubmenusIncomplete++;
                    WinPostMsg(pHP->hwndIOThread, WM_POPUPMENUSETICON, MPFROMP(pMenuDataCurrent->Submenu), mp2);
                    }
                }
            pMenuDataCurrent=pMenuDataCurrent->Next;
            }
        }
    ulSubmenusIncomplete--;
                                        /* See if we did load/unload all menuentries' icons */
    if(ulSubmenusIncomplete==0)
        {
#ifdef  DEBUG_ITRD
        printf("ITrd: WM_POPUPMENUSETICON finished (%s)\n", (SHORT1FROMMP(mp2)==TRUE ? "Load" : "Unload"));
#endif  /* DEBUG_ITRD */
        WinPostMsg(pHP->hwndClient, WM_POPUPMENUREADY, MPFROMLONG(ID_IOTHREAD), NULL);
        }
    CHECKPOINT(ITrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_POPUPDELETE, MENUDATA *pMenuData, BOOL bCurrentOnly                       *
\*                                                                                      */
case WM_POPUPMENUDELETE:
/*                                                                                      *\
 * Posted by the main thread to in order to deallocate a MENUDATA structure, which may  *
 * either be a single ENTRYMENUITEM or ENTRYCONTROL, or a ENTRYSUBMENU submenu, in      *
 * which case the submenu's MENUDATA structures get deleted recursively. As this may    *
 * take a while, this is done by the I/O thread (even no I/O is required). If the       *
 * to be deallocated menuentries have their icons loaded, deallocate them first.        *
\*                                                                                      */
    {
                                        /* Pointer to root of current submenu or menuentry within the
                                           Popup-Menu */
    MENUDATA        *pMenuDataCurrent=(MENUDATA *)PVOIDFROMMP(mp1);

    CHECKPOINT(ITrd, 0x0800);
#ifdef  DEBUG_ITRD
    if(!(pHP->ulRuntimeFlag & POPUPMENUDELETE))
        printf("ITrd: WM_POPUPMENUDELETE: (%s) (%s)\n", pMenuDataCurrent->PgmTitle,
            (SHORT1FROMMP(mp2)==TRUE ? "Single" : "All"));
#endif  /* DEBUG_ITRD */
                                        /* Free the current single menuentry */
    if(!(pHP->ulRuntimeFlag & (PC2EMERGENCYEXIT|QUITFROMSHUTDOWN)))
        {
                                        /* If we're not currently deallocating icons of the Popup-Menu, start
                                           deallocation of the icons */
        if((pHP->ulStatusFlag2 & POPUPICON) && !(pHP->ulRuntimeFlag & POPUPMENUDELETE))
            {
                                        /* Prevent ourselves from recursion to delete the Popup-Menu we're currently
                                           deleting */
            pHP->ulRuntimeFlag|=(POPUPMENUUPDATE|POPUPMENUDELETE);
                                        /* If we delete a single menuentry, we can do all work locally here,
                                           otherwise we will synchronously invoke the deallocation routine */
            if(SHORT1FROMMP(mp2)==TRUE)
                {
                InitializeIcon(pMenuDataCurrent, FALSE);
                if(pMenuDataCurrent->Item==ENTRYSUBMENU)
                    WinSendMsg(pHP->hwndIOThread, WM_POPUPMENUUPDATE, MPFROMP(pMenuDataCurrent->Submenu), MPFROMSHORT(FALSE));
                else
                    pHP->ulRuntimeFlag&=(~POPUPMENUUPDATE);
                }
            else
                {
                WinSendMsg(pHP->hwndIOThread, WM_POPUPMENUUPDATE, MPFROMP(pMenuDataCurrent), MPFROMSHORT(FALSE));
                }
            }
                                        /* Before actually deleting the Popup Menu's linked list, wait that all
                                           changes to the menuentries inside have been done */
        if(pHP->ulRuntimeFlag & POPUPMENUUPDATE)
            {
            WinPostMsg(pHP->hwndIOThread, msg, mp1, mp2);
            return((MRESULT)FALSE);
            }
        else
            FreeMenu(pMenuDataCurrent, (BOOL)mp2);
                                        /* We've deallocated the Popup Menu */
        pHP->ulRuntimeFlag&=(~POPUPMENUDELETE);
        }
#ifdef  DEBUG_ITRD
    printf("ITrd: WM_POPUPMENUDELETE finished\n");
#endif  /* DEBUG_ITRD */
    CHECKPOINT(ITrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * WM_SESSIONBARUNHIDE, ULONG ulUnhideFlag, ULONG ulUnhideRequest                       *
\*                                                                                      */
case WM_SESSIONBARUNHIDE:
/*                                                                                      *\
 * Message posted by PC2HOOK.DLL when mouse pointer enters or leaves SessionBar area,   *
 * to allow dynamic unhiding and unhiding of SessionBar. The flag ulUnhideFlag is TRUE  *
 * if the SessionBar should be unhidden, and FALSE otherwise, the flag ulUnhideRequest  *
 * is TRUE if the Dynamic SessionBar timer has been expired and FALSE otherwise.        *
\*                                                                                      */
                                        /* Ensure that the SessionBar has been drawn, especially
                                           when using a memory presentation space */
    WinInvalidateRect(pHP->hwndSessionClient, NULL, FALSE);
    if(mp1)
        {
                                        /* If the Dynamic SessionBar should be unhidden
                                           then do that if the delay has expired */
        if(mp2)
            {
            SWP     swpSessionBar;

            memset(&swpSessionBar, 0, sizeof(swpSessionBar));
            if(pHP->hwndSessionFrame)
                {
                swpSessionBar.hwnd=pHP->hwndSessionFrame;
                swpSessionBar.fl=SWP_ZORDER|SWP_SHOW;
                swpSessionBar.hwndInsertBehind=HWND_TOP;
                WinSetMultWindowPos(pThreadData->habThread, &swpSessionBar, 1);
                }
            if(pHP->ulSessionBarFlag & SB_LASTTOP)
                {
                WinPostMsg(pHP->hwndSessionClient, WM_SESSIONBARUNHIDE,
                    MPFROMLONG(SB_LASTTOP|SB_UNHIDEFROMTOP), MPFROMLONG(pHP->swpPC2Session.y));
                }
            else
                {
                WinPostMsg(pHP->hwndSessionClient, WM_SESSIONBARUNHIDE,
                    MPFROMLONG(SB_LASTBOTTOM|SB_UNHIDEFROMBOTTOM), MPFROMLONG(pHP->swpPC2Session.y));
                }
            }
        else
                                        /* The timer has not been started yet, so start the
                                           timer (and thus this message could only have been
                                           posted in the hook as we get hereto) */
            pHP->ulSessionBarFlag|=SB_UNHIDEDELAY;
        }
    else
        {
                                        /* If the Dynamic SessionBar should be hidden,
                                           slide it out of Screen, even if the timer to
                                           show it is still running */
        pHP->ulSessionBarFlag&=~SB_UNHIDEDELAY;
        ulDynamicSessionBarCount=0;
        if(pHP->ulSessionBarFlag & SB_LASTTOP)
            {
            WinPostMsg(pHP->hwndSessionClient, WM_SESSIONBARUNHIDE,
                MPFROMLONG(SB_LASTTOP), MPFROMLONG(pHP->swpPC2Session.y));
            }
        else
            {
            WinPostMsg(pHP->hwndSessionClient, WM_SESSIONBARUNHIDE,
                MPFROMLONG(SB_LASTBOTTOM), MPFROMLONG(pHP->swpPC2Session.y));
            }
        }
    break;

/*                                                                                      *\
 * Syntax: WM_SLIDINGFOCUSDELAY, HWND hwndWindow, NULL                                  *
\*                                                                                      */
case WM_SLIDINGFOCUSDELAY:
/*                                                                                      *\
 * Posted by PC2HOOK.DLL when user has selected a delay for the activation of windows   *
 * by the Sliding Focus. In that case we simply restart the delay counter by clearing   *
 * it (this counter will be incremented every 100 ms in a WM_TIMER message). When the   *
 * delay counter expires, the hook will be informed to activate the window.             *
\*                                                                                      */
    {
    hwndActiveWindowClient=HWNDFROMMP(mp1);
    mp1SlidingFocus=mp2;
    ulSlidingFocusCount=0;
    }
    break;

/*                                                                                      *\
 * Syntax: WM_DYNAMICMENUDELAY, HWND hwndCurrentMenu, MPARAM mp2                        *
\*                                                                                      */
case WM_DYNAMICMENUDELAY:
/*                                                                                      *\
 * Posted by PC2HOOK.DLL when user has selected a delay for selecting a menuentry/sub-  *
 * menu by the Dynamic Menuselection. In that case we simply restart the delay counter  *
 * by clearing it (this counter will be incremented every 100 ms in a WM_TIMER message) *
 * and saving the parameter mp1 (passed via mp2) of the WM_MOUSEMOVE message we modify. *
 * When the delay counter expires, this parameter is passed back to the menu by posting *
 * another WM_MOUSEMOVE message but sligthly modified so that the hook knows the        *
 * working thread has posted it.                                                        *
\*                                                                                      */
    {
    hwndCurrentMenu=HWNDFROMMP(mp1);
    mp1DynamicMenu=mp2;
    ulDynamicMenuCount=0;
    }
    break;

/*                                                                                      *\
 * Syntax: WM_VIRTUALDESKTOPDELAY, (USHORT usMouseXPos, USHORT usMouseYPos),            *
 *                                 ULONG ulMoveFlag                                     *
\*                                                                                      */
case WM_VIRTUALDESKTOPDELAY:
/*                                                                                      *\
 * Posted by PC2HOOK.DLL when user has selected a delay before switching between        *
 * Virtual Desktops by moving the mouse pointer onto a surrounding row and/or column.   *
 * In that case we simply restart the dalay counter by clearing it (this counter will   *
 * be incremented every 100 ms in a WM_TIMER message) and saving the parameters mp1     *
 * and mp2 of the WM_MOUSEMOVE messages that will by used to request the scrolling if   *
 * the timer has expired.                                                               *
\*                                                                                      */
    {
    ulMousePos=LONGFROMMP(mp1);
    ulMoveFlag=LONGFROMMP(mp2);
    ulVirtualDesktopCount=0;
    }
    break;

/*                                                                                      *\
 * Syntax: WM_OVERVIEWUNHIDEDELAY, (USHORT usMouseXPos, USHORT usMouseYPos),            *
 *                                 ULONG ulOverviewFlag                                 *
\*                                                                                      */
case WM_OVERVIEWUNHIDEDELAY:
/*                                                                                      *\
 * Posted by PC2HOOK.DLL when user has selected that the Overview window should be      *
 * unhidden when the mouse pointer is moved at an edge of the screen.               .   *
 * In that case we simply restart the dalay counter by clearing it (this counter will   *
 * be incremented every 100 ms in a WM_TIMER message) and saving the parameters mp1     *
 * and mp2 of the WM_MOUSEMOVE messages that will by used to request the scrolling if   *
 * the timer has expired.                                                               *
\*                                                                                      */
    {
    ulOverviewFlag=LONGFROMMP(mp2);
    ulOverviewUnhideCount=0;
    }
    break;

/*                                                                                      *\
 * Syntax: WM_ANALYZEPC2TRAP, NULL, NULL                                                *
\*                                                                                      */
case WM_ANALYZEPC2TRAP:
/*                                                                                      *\
 * Message posted to open and analyse PC2.TRP's contents to check if a new exception    *
 * was logged to request the user to inform the author.                                 *
\*                                                                                      */
    {
    UCHAR   ucBuffer[EF_SIZE255+1];
    UCHAR   ucLastPC2Trap[EF_SIZE255+1];
    FILE    *pfileTrap;
    ULONG   ulHangCount=0;
    ULONG   ulTrapCount=0;

    memset(ucLastPC2Trap, '\0', sizeof(ucLastPC2Trap));
                                        /* Try to open PC2.TRP file */
    pfileTrap=fopen(pHP->ucFilenameTRP, "r");
    if(pfileTrap==NULL)
        break;
                                        /* Read PC2.TRP until the string "Time       : "
                                           is found. Save the data after the colon (and
                                           strip trailing whitespaces) */
    while(!feof(pfileTrap))
        {
        fgets(ucBuffer, sizeof(ucBuffer), pfileTrap);
                                        /* First, check if the entry is a Thread Hang or
                                           an Exception */
        if(!memcmp(ucBuffer, "Thread Dump Facility", sizeof("Thread Dump Facility")-1))
            {
            ulHangCount++;
            }
        if(!memcmp(ucBuffer, "Exception Dump Facility", sizeof("Exception Dump Facility")-1))
            {
            ulTrapCount++;
            }
                                        /* Second, get the entry's timestamp to find the
                                           last one in PC2.TRP. If we're lucky any know
                                           this entry as the last one of the last run of
                                           PC/2 then reset counters, as we can ignore
                                           problems reported earlier */
        if(!memcmp(ucBuffer, "Time       : ", sizeof("Time       : ")-1))
            {
            strcpy(ucLastPC2Trap, &ucBuffer[sizeof("Time       : ")-1]);
            StripTrailing(ucLastPC2Trap);
            if(!strcmp(pHP->ucLastPC2Trap, ucLastPC2Trap))
                {
                ulHangCount=0;
                ulTrapCount=0;
                }
            }
        }
    fclose(pfileTrap);
                                        /* If a string was found, check if it is different
                                           from the one from the last time (as new
                                           exceptions are added at the end of PC2.TRP the
                                           latest string we found automatically is the
                                           most recent one). If a new one was found inform
                                           main thread to display a message to the user */
    if(strlen(ucLastPC2Trap))
        {
        if(strcmp(pHP->ucLastPC2Trap, ucLastPC2Trap))
            {
            strcpy(pHP->ucLastPC2Trap, ucLastPC2Trap);
            WinPostMsg(pHP->hwndClient, WM_ANALYZEPC2TRAP, MPFROMLONG(ulHangCount), MPFROMLONG(ulTrapCount));
            }
        }
    }
    break;

default:                                /* Default window procedure must be called */
    return((MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This procedure is the PC/2 network window procedure (is an object window).           *
\*--------------------------------------------------------------------------------------*/
MRESULT EXPENTRY PC2_NetThreadWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Get thread's window word */
THREADDATA      *pThreadData=(THREADDATA *)WinQueryWindowULong(hwnd, QWL_USER);
                                        /* Dynamic loaded entrypoint to PC2Net.dll */
static PFFUNCPTR6
                *pNetworkInterface=NULL;
static PFFUNCPTR6
                *pStatusWindowProc=NULL;
                                        /* Pass C-Runtime functions to PC2NET.DLL which
                                           is only linked with the subsystem C library */
static CRUNTIME cruntimeInterface;
static ULONG    ulClosingThread=FALSE;  /* Set to TRUE after WM_QUITTHREAD was received */
static ULONG    ulTimerTicks=0;         /* Timer ticks (about every TIMERINTERVALLMS milliseconds) */

switch(msg)
{
case WM_CREATE:
    if(pHP->ulDebug>=DEBUG_LOW)
        printf("NTrd: WM_CREATE\n");
    pThreadData=PVOIDFROMMP(mp1);
                                        /* Save cache of I/O thread's window handle */
    pHP->hwndNetThread=hwnd;
                                        /* Set the passed control structure into the QWL_USER window word */
    WinSetWindowULong(hwnd, QWL_USER, (ULONG)pThreadData);
                                        /* Just be sure */
    pNetworkInterface=pStatusWindowProc=NULL;
    break;

/*                                                                                      *\
 * Syntax: WM_INITTHREAD, NULL, NULL                                                    *
\*                                                                                      */
case WM_INITTHREAD:
/*                                                                                      *\
 * Message posted by thread to its message queue immediately before entering the        *
 * message loop, to allow the thread to initialize its thread specific data.            *
\*                                                                                      */
    {
    UCHAR   ucBuffer[CCHMAXPATH];
    HMODULE hDLLPc2Net=NULLHANDLE;
    ULONG   *pulVersion;
    ULONG   ulError=NO_ERROR;

#ifdef  DEBUG
    TIB     *ptib;
    PIB     *ppib;

    DosGetInfoBlocks(&ptib, &ppib);
    printf("NTrd: WM_INITTHREAD TID %ld\n", (int)ptib->tib_ptib2->tib2_ultid);
#endif  /* DEBUG */
    CHECKPOINT(NTrd, 0x0100);
#ifdef  DEBUG_NTRD
    printf("NTrd: WM_INITTHREAD\n");
#endif  /* DEBUG_NTRD */
                                        /* Try to load PC2NET.DLL, which is PC/2's network interface
                                           DLL. If loading works we know TCP/IP has been installed,
                                           because PC2NET.DLL is statically linked with the TCP/IP
                                           DLLs, otherwise we know that TCP/IP is not installed.
                                           However, being able to load the DLL does not tell us if
                                           it is configured correctly, we can only try ... */

    ulError=DosLoadModule(ucBuffer, sizeof(ucBuffer)-1, "PC2Net", &hDLLPc2Net);
    if(ulError==NO_ERROR)
        {
        if(DosQueryProcAddr(hDLLPc2Net, 1, "_PC2NETSIGNATURE", (PFN *)(&pulVersion))!=NO_ERROR)
            {
            ulError=ERROR_FILE_NOT_FOUND;
            DosFreeModule(hDLLPc2Net);
            }
        else
            {
#ifdef  DEBUG
            printf("NTrd: Version PC2Net.DLL %lX, expected %lX\n", *pulVersion, (ULONG)BLDLEVEL_SIGNATURE);
#endif  /* DEBUG */
            if(*pulVersion!=BLDLEVEL_SIGNATURE)
                {
                ulError=ERROR_BAD_FORMAT;
                DosFreeModule(hDLLPc2Net);
                }
            else if((DosQueryProcAddr(hDLLPc2Net, 2, "NetworkInterface", (PFN *)(&pNetworkInterface))!=NO_ERROR) ||
                (DosQueryProcAddr(hDLLPc2Net, 3, "StatusWindowProc", (PFN *)(&pStatusWindowProc))!=NO_ERROR))
                {
                ulError=ERROR_PROC_NOT_FOUND;
                pNetworkInterface=pStatusWindowProc=NULL;
                DosFreeModule(hDLLPc2Net);
                }
            else
                {
#ifdef  DEBUG
                printf("NTrd: WM_INITTHREAD pNetworkInterface %p, pStatusWindowProc %p\n", pNetworkInterface, pStatusWindowProc);
#endif  /* DEBUG */
                                        /* Ok, network access should work as TCP/IP is installed */
                pHP->ulRuntimeFlag|=TCPIPSUPPORTED;
                pHP->hDLLPc2Net=hDLLPc2Net;
                                        /* Initialize C-Runtime access for PC2NET.DLL */
                memset(&cruntimeInterface, 0, sizeof(cruntimeInterface));
                cruntimeInterface.pfnMalloc=malloc;
                cruntimeInterface.pfnFree=free;
                cruntimeInterface.pfnStricmp=stricmp;
                cruntimeInterface.pfnStrnicmp=strnicmp;
                cruntimeInterface.pfnStrupr=strupr;
                cruntimeInterface.pfnStrdup=strdup;
                cruntimeInterface.pfnFprintf=fprintf;
                cruntimeInterface.pfnFopen=fopen;
                cruntimeInterface.pfnFclose=fclose;
                cruntimeInterface.pfnMktime=mktime;
                cruntimeInterface.pfnStripLeading=StripLeading;
                cruntimeInterface.pfnStripTrailing=StripTrailing;
                cruntimeInterface.pus_Ctype=(USHORT *)_ctype;
                pNetworkInterface(hwnd, msg, MPFROMP(pHP), MPFROMP(&cruntimeInterface));
                                        /* Request creation of Status window */
                WinPostMsg(pHP->hwndClient, WM_NETSTATUS, MPFROMLONG(TRUE), MPFROMP(pStatusWindowProc));
                }
            }
        }
    DosSleep(250);
                                        /* Init timer portion of C-Runtime */
    WinSendMsg(hwnd, WM_TIMER, NULL, NULL);
                                        /* Post this message as the last one into the message queue,
                                           to inform client window that I/O thread is ready then */
    WinPostMsg(hwnd, WM_THREADSTARTUPREADY, MPFROMLONG(ID_NETTHREAD), MPFROMLONG(ulError));
    CHECKPOINT(NTrd, 0x0000);
    }
    break;

case WM_TIMER:
/*                                                                                      *\
 * Message posted by the main thread every TIMERINTERVALLMS*(TICKSPERMINUTE)            *
 * milliseconds                                                                         *
\*                                                                                      */
    {
    time_t      time_tCurrent;
    struct tm   *ptmCurrent;

                                        /* Reset alive timer main thread keeps counting */
    pHP->ulAliveTimerNTrd=0;
                                        /* Update timer */
    ulTimerTicks++;
    if(pHP->ulDebug>=DEBUG_LOW)
        {
        DosBeep(3500, 50);
        printf("NTrd: WM_TIMER\n");
        }
                                        /* Set the current time */
    time(&time_tCurrent);
    ptmCurrent=localtime(&time_tCurrent);
    if(ptmCurrent!=0)
        cruntimeInterface.tm_Current=*ptmCurrent;
                                        /* Pass message on to PC2NET.DLL */
    if(pNetworkInterface!=NULL)
        pNetworkInterface(hwnd, msg, MPFROMP(pHP), MPFROMP(&cruntimeInterface));
    }
    break;

/*                                                                                      *\
 * Syntax: WM_QUITTHREAD, ULONG ulThreadId, NULL                                        *
\*                                                                                      */
case WM_QUITTHREAD:
/*                                                                                      *\
 * It is about time to terminate the network thread upon request of the main thread.    *
 * We unlock our thread's message queue to be able to receive WM_QUIT again and we post *
 * WM_QUIT to our message queue afterwards.                                             *
\*                                                                                      */
    CHECKPOINT(NTrd, 0x0200);
#ifdef  DEBUG_NTRD
    printf("NTrd: WM_QUITTHREAD\n");
#endif  /* DEBUG_NTRD */
                                        /* If available, deinitialize PC2NET.DLL */
    if((pHP->hDLLPc2Net!=NULLHANDLE) && (pNetworkInterface!=NULL))
        {
                                        /* Request destruction of Status window */
        WinSendMsg(pHP->hwndClient, WM_NETSTATUS, MPFROMLONG(FALSE), NULL);
                                        /* Pass on message */
        pNetworkInterface(hwnd, msg, mp1, mp2);
        pHP->ulRuntimeFlag&=(~TCPIPSUPPORTED);
        pNetworkInterface=pStatusWindowProc=NULL;
        DosFreeModule(pHP->hDLLPc2Net);
        pHP->hDLLPc2Net=NULLHANDLE;
#ifdef  DEBUG
        printf("NTrd: WM_QUITTHREAD pNetworkInterface %p, pStatusWindowProc %p\n", pNetworkInterface, pStatusWindowProc);
#endif  /* DEBUG */
        }
                                        /* Wait with the close request until we've done
                                           our work */
    ulClosingThread=TRUE;
                                        /* Reenable our message queue to receive future
                                           WM_QUIT messages */
    WinCancelShutdown(pThreadData->hmqThread, FALSE);
                                        /* And here WM_QUIT comes */
    WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
    CHECKPOINT(NTrd, 0x0000);
    break;

/*                                                                                      *\
 * Syntax: WM_THREADSTARTUPREADY, ULONG ulThreadId, ULONG ulError                       *
\*                                                                                      */
case WM_THREADSTARTUPREADY:
/*                                                                                      *\
 * Network thread is now ready                                                          *
\*                                                                                      */
#ifdef  DEBUG_NTRD
    printf("NTrd: WM_THREADSTARTUPREADY\n");
#endif  /* DEBUG_NTRD */
                                        /* Inform client window that network thread is now ready */
    WinPostMsg(pHP->hwndClient, WM_THREADSTARTUPREADY, MPFROMLONG(ID_NETTHREAD), mp2);
    break;

default:                                /* For unknown message, try to pass them to our
                                           network interface DLL */
    if(pNetworkInterface!=NULL)
        return(pNetworkInterface(hwnd, msg, mp1, mp2));
    else
        return((MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This procedure is the PC/2 working thread window procedure (is an object window).    *
\*--------------------------------------------------------------------------------------*/
MRESULT EXPENTRY PC2_WorkThreadWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Get thread's window word */
THREADDATA      *pThreadData=(THREADDATA *)WinQueryWindowULong(hwnd, QWL_USER);
POWERSTATUS     powerstatusAPM;         /* APM POWER_GETPOWERSTATUS event datastructure */
POWEREVENT      powereventAPM;          /* APM POWER_SENDPOWEREVENT event datastructure */
ULONG           ulReturnCode;           /* DosDevIOCtl parameters */
ULONG           ulPacketSize;
ULONG           ulDataSize;
static ULONG    ulClosingThread=FALSE;  /* Set to TRUE after WM_QUITTHREAD was received */
static HDC      hdcClient;              /* Client area DC */
                                        /* Timer ticks (about every TIMERINTERVALLMS milliseconds) */
static ULONG    ulTimerTicks=(ULONG)-1;
                                        /* PC/2 up time in seconds (derived from timer ticks) */
static ULONG    ulUpTimeSeconds=(ULONG)-1;
static HFILE    hfileAPM=NULLHANDLE;    /* APM$ device driver handle */

switch(msg)
{
case WM_CREATE:
    if(pHP->ulDebug>=DEBUG_LOW)
        printf("WTrd: WM_CREATE\n");
    pThreadData=PVOIDFROMMP(mp1);
                                        /* Save cache of working thread's window handle */
    pHP->hwndWorkingThread=hwnd;
                                        /* Set the passed control structure into the QWL_USER window word */
    WinSetWindowULong(hwnd, QWL_USER, (ULONG)pThreadData);
    break;

case WM_TIMER:
/*                                                                                      *\
 * Message posted by the main thread to give working thread a chance to initiate        *
 * regular processing events.                                                           *
\*                                                                                      */
    {
                                        /* If the current minute is not the last one, its time for us
                                           to see if the new minute should schedule any menuentry */
    static ULONG    ulLastMinute=(ULONG)-1;
    ULONG           ulFSProcess;
    ULONG           ulProgramType;

                                        /* Current date & time in format compatible with scheduler */
    SCHEDULERDATA   SDCurrent;
                                        /* Current time & time compatible with C runtime */
    time_t          time_tCurrent;
    struct tm       *ptmCurrent;
    LONG            lIndex;
    ULONG           *pulShiftMask;

                                        /* Reset alive timer main thread keeps counting */
    pHP->ulAliveTimerWTrd=0;
    ulTimerTicks++;
    if(pHP->ulDebug>=DEBUG_LOW)
        if((ulTimerTicks % 10)==0)
            {
            DosBeep(2000, 50);
            printf("WTrd: WM_TIMER\n");
            }
                                        /* Count the seconds of PC/2's uptime */
    if((ulTimerTicks % TICKSPERSECOND)==0)
        ulUpTimeSeconds++;
                                        /* Things we check every 30 seconds */
    if(ulTimerTicks % (TICKSPERSECOND*30)==0)
        {
                                        /* If the current fullscreen session is not the PM session
                                           itself (value 1), clear Lockup/APM activation timer */
        if(DosQuerySysInfo(QSV_FOREGROUND_PROCESS, QSV_FOREGROUND_PROCESS, &ulFSProcess, sizeof(ulFSProcess))==NO_ERROR)
            {
            ulProgramType=(ULONG)GetWindowList(pThreadData->habThread, WINDOWLIST_FINDPROCESS, (MPARAM)ulFSProcess);
            if((ulProgramType==PROG_FULLSCREEN) || (ulProgramType==PROG_VDM))
                {
                pHP->ulCurrentTimer=0;
                }
            }
        }
                                        /* Things we check every 5 seconds */
    if(ulTimerTicks % (TICKSPERSECOND*AUTOSTARTDELAY_SEC)==0)
        {
                                        /* Scan for autostart applications, however if the user
                                           has for some reason disabled the general setting,
                                           don't autostart the sessions, even when autostart
                                           was selected (optinally, autostarted sessions may
                                           have defined a delay to prevent e.g. race conditions).
                                           When there are no Autostart sessions left - because
                                           we have already started all of them - then there is
                                           no need to do anymore Autostart stuff */
        if(!(pHP->ulRuntimeFlag & NOPENDINGAUTOSTART))
            if(!(pHP->ulStatusFlag1 & IGNOREAUTOSTART) && !(pHP->ulRuntimeFlag & NOAUTOSTART))
                WinPostMsg(hwnd, WM_AUTOSTART, MPFROMLONG(ulUpTimeSeconds), NULL);

        }
                                        /* Get APM status every 10 seconds */
    if((ulTimerTicks % (TICKSPERSECOND*10)==0) && (hfileAPM!=NULLHANDLE))
        {
        WinSendMsg(hwnd, WM_APM, NULL, NULL);
        }
                                        /* Get current time in C runtime format */
    memset(&SDCurrent, 0, sizeof(SCHEDULERDATA));
    time(&time_tCurrent);
    ptmCurrent=localtime(&time_tCurrent);
                                        /* Only trigger scheduler on a change from a vaild minute
                                           to another. If this trigger would not be here, then
                                           if you schedule a reboot, and the reboot will take less
                                           then a minute, another reboot will take place, because
                                           the initialization value of -1 does not match the current
                                           minute (and we're still in the same minute) and the
                                           scheduler triggers another time */
    if(ulLastMinute==(ULONG)-1)
        ulLastMinute=(ULONG)ptmCurrent->tm_min;
                                        /* Do only once in a minute, for the first timer tick of the
                                           current minute we receive (if the scheduler is enabled) */
    if((!(pHP->ulStatusFlag1 & IGNORESCHEDULER) && !(pHP->ulRuntimeFlag & NOSCHEDULER)) &&
        (ulLastMinute!=(ULONG)ptmCurrent->tm_min))
        {
#ifdef  DEBUG_WTRD
        printf("WTrd: WM_TIMER (Scheduler)\n");
#endif  /* DEBUG_WTRD */
                                        /* Save the fact that we test the current minute with our
                                           Scheduler for any menuentry to be scheduled */
        ulLastMinute=ptmCurrent->tm_min;
                                        /* Load C runtime format into our Scheduler format */
        pulShiftMask=&SDCurrent.ulStartingMinute1;
        if((unsigned)ptmCurrent->tm_min>=(sizeof(ULONG)*8-1))
            {
            ptmCurrent->tm_min-=(sizeof(ULONG)*8-1);
            pulShiftMask=&SDCurrent.ulStartingMinute2;
            }
                                        /* Load C runtime format into our Scheduler format */
        for(lIndex=0, (*pulShiftMask)=0x00000001; lIndex<ptmCurrent->tm_min; lIndex++, (*pulShiftMask)<<=1)
            ;                           /* NOP */
        pulShiftMask=&SDCurrent.ulStartingHour;
        for(lIndex=0, (*pulShiftMask)=0x00000001; lIndex<ptmCurrent->tm_hour; lIndex++, (*pulShiftMask)<<=1)
            ;                           /* NOP */
        pulShiftMask=&SDCurrent.ulStartingWeekday;
        for(lIndex=0, (*pulShiftMask)=0x00000001; lIndex<ptmCurrent->tm_wday; lIndex++, (*pulShiftMask)<<=1)
            ;                           /* NOP */
        pulShiftMask=&SDCurrent.ulStartingDay;
        for(lIndex=1, (*pulShiftMask)=0x00000001; lIndex<ptmCurrent->tm_mday; lIndex++, (*pulShiftMask)<<=1)
            ;                           /* NOP */
        pulShiftMask=&SDCurrent.ulStartingMonth;
        for(lIndex=0, (*pulShiftMask)=0x00000001; lIndex<ptmCurrent->tm_mon; lIndex++, (*pulShiftMask)<<=1)
            ;                           /* NOP */
                                        /* Now recursively scan through the Popup Menu to items
                                           that should be scheduled now */
        ScheduleItem(pHP->pPopupMenu, &SDCurrent);
        }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_INITTHREAD, NULL, NULL                                                    *
\*                                                                                      */
case WM_INITTHREAD:
/*                                                                                      *\
 * Message posted by thread to its message queue immediately before entering the        *
 * message loop, to allow the thread to initialize its thread specific data.            *
\*                                                                                      */
    {
    ULONG   ulAction;
    SIZEL   sizelClient;

#ifdef  DEBUG
    TIB     *ptib;
    PIB     *ppib;

    DosGetInfoBlocks(&ptib, &ppib);
    printf("WTrd: WM_INITTHREAD TID %ld\n", (int)ptib->tib_ptib2->tib2_ultid);
#endif  /* DEBUG */
    CHECKPOINT(WTrd, 0x0100);
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_INITTHREAD\n");
#endif  /* DEBUG_WTRD */
    DosSleep(150);
                                        /* Get APM status, and initialize APM status, as when
                                           the working thread posts its ready condition the
                                           SessionBar will be created and draw according to
                                           the APM status data */
    if(DosOpen("\\DEV\\APM$", &hfileAPM, &ulAction, 0, FILE_NORMAL,
        OPEN_ACTION_OPEN_IF_EXISTS,
        OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE, NULL)==NO_ERROR)
        {
        pHP->ulRuntimeFlag|=APMSUPPORTED;
        WinSendMsg(hwnd, WM_APM, MPFROMLONG(TRUE), NULL);
        }
                                        /* First, get the PM Hard Error and System Error window handles */
    GetDesktopWindows(pThreadData->habThread, PM_ERRORWINDOWS_QUERY, 0);
                                        /* Get presentation space for PC/2's client window */
    sizelClient.cx=pHP->DesktopSize.x;
    sizelClient.cy=pHP->DesktopSize.y;
                                        /* Get device context and a presentation space associated
                                           for client window */
    hdcClient=WinOpenWindowDC(pHP->hwndClient);
    hpsClient=GpiCreatePS(pThreadData->habThread, hdcClient, &sizelClient, PU_PELS|GPIA_ASSOC);
    GpiCreateLogColorTable(hpsClient,   /* Change color table into RGB mode */
        0L,                             /* Options */
        LCOLF_RGB,                      /* Set color table into RGB mode */
        0L, 0L, NULL);                  /* Starting, ending item, table */
                                        /* Initialize APM status, Scheduler, ... */
    WinPostMsg(hwnd, WM_TIMER, NULL, NULL);
                                        /* Post message to query Desktop's/PM window handle */
    WinPostMsg(hwnd, WM_SETDESKTOPHANDLE, NULL, NULL);
                                        /* If required expand WPS */
    if(pHP->ulStatusFlag1 & EXPANDWPS)
        WinPostMsg(hwnd, WM_EXPANDWPS, (MPARAM)TRUE, NULL);
                                        /* Load PM background bitmap */
    WinPostMsg(hwnd, WM_BACKGROUNDBITMAP, NULL, NULL);
                                        /* Enumerate all windows. This takes a while during first
                                           invokation for unknown reasons (maybe OS/2 code is loaded) */
    WinPostMsg(hwnd, WM_SETUPSIZEPOSITION, MPFROMLONG(WM_MOVE), NULL);
                                        /* Change to all local drives root directory */
    WinPostMsg(hwnd, WM_SETDRIVEROOT, NULL, NULL);
                                        /* Post this message as the last one into the message queue,
                                           to inform client window that working thread is ready then */
    WinPostMsg(hwnd, WM_THREADSTARTUPREADY, MPFROMLONG(ID_WORKINGTHREAD), NULL);
    }
    CHECKPOINT(WTrd, 0x0000);
    break;

/*                                                                                      *\
 * Syntax: WM_THREADSTARTUPREADY, ULONG ulThreadId, NULL                                *
\*                                                                                      */
case WM_THREADSTARTUPREADY:
/*                                                                                      *\
 * Working thread is now ready, make Overview window visible when requested.            *
\*                                                                                      */
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_THREADSTARTUPREADY\n");
#endif  /* DEBUG_WTRD */
                                        /* Inform client window that working thread is now ready */
    WinPostMsg(pHP->hwndClient, WM_THREADSTARTUPREADY, MPFROMLONG(ID_WORKINGTHREAD), NULL);
    break;

/*                                                                                      *\
 * Syntax: WM_QUITTHREAD, ULONG ulThreadId, NULL                                        *
\*                                                                                      */
case WM_QUITTHREAD:
/*                                                                                      *\
 * It is about time to terminate the working thread upon request of the main thread.    *
 * We unlock our thread's message queue to be able to receive WM_QUIT again and we post *
 * WM_QUIT to our message queue afterwards.                                             *
\*                                                                                      */
    CHECKPOINT(WTrd, 0x0200);
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_QUITTHREAD\n");
#endif  /* DEBUG_WTRD */
                                        /* Yep, time to close */
    ulClosingThread=TRUE;
                                        /* Reset APM */
    DosClose(hfileAPM);
                                        /* Cleanup our copy of the HWND_DESKTOP child windows enumerated */
    GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_FREE, NULL);
                                        /* Reenable our message queue to receive future
                                           WM_QUIT messages */
    WinCancelShutdown(pThreadData->hmqThread, FALSE);
                                        /* Release presentation space */
    GpiDestroyPS(hpsClient);
                                        /* Release device context */
    DevCloseDC(hdcClient);
                                        /* And here WM_QUIT comes */
    WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
    CHECKPOINT(WTrd, 0x0000);
    break;

/*                                                                                      *\
 * Syntax: WM_APM, ULONG ulToggleAPM, NULL                                              *
\*                                                                                      */
case WM_APM:
/*                                                                                      *\
 * Query the APM status.                                                                *
\*                                                                                      */
    {
    ULONG   ulRuntimeFlagPrevious=pHP->ulRuntimeFlag;

    CHECKPOINT(WTrd, 0x0300);
                                        /* Use APM support only if requested to do so, as the
                                           Set PWR MGMT function to enable APM consistently hung
                                           the kernel on my Intellistation Z on Warp Server SMP */
    if(pHP->ulStatusFlag2 & SUPPORTAPM)
        {
                                        /* See if we need to toggle APM support in the APM.SYS
                                           device driver */
        if(LONGFROMMP(mp1)!=FALSE)
            {
            memset(&powereventAPM, 0, sizeof(powereventAPM));
            ulReturnCode=0;
                                            /* Set PWR MGMT function */
            powereventAPM.usSubID=(pHP->ulStatusFlag2 & ENABLEAPM ? 0x0003 : 0x0004);
            ulPacketSize=sizeof(powereventAPM);
            ulDataSize=sizeof(ulReturnCode);
            DosDevIOCtl(hfileAPM, IOCTL_POWER, POWER_SENDPOWEREVENT,
                &powereventAPM, ulPacketSize, &ulPacketSize,
                &ulReturnCode, ulDataSize, &ulDataSize);
            }
                                        /* Get the current APM power status */
         memset(&powerstatusAPM, 0, sizeof(powerstatusAPM));
         ulReturnCode=0;
         powerstatusAPM.usParmLength=sizeof(powerstatusAPM);
         ulPacketSize=sizeof(powerstatusAPM);
         ulDataSize=sizeof(ulReturnCode);
         DosDevIOCtl(hfileAPM, IOCTL_POWER, POWER_GETPOWERSTATUS,
             &powerstatusAPM, ulPacketSize, &ulPacketSize,
             &ulReturnCode, ulDataSize, &ulDataSize);
                                        /* Enable PC/2's APM status window, if APM is enabled
                                           (usPowerFlags!=0) and if it is a battery capable
                                           PC (ucACStatus!=255 and ucBatteryLife!=255) as it
                                           does not make sense just to display line power for
                                           line powered only PCs */
        if((powerstatusAPM.usPowerFlags==0) ||
            (powerstatusAPM.ucACStatus==255) ||
            (powerstatusAPM.ucBatteryLife==255))
            pHP->ulRuntimeFlag &= (~APMPOWERSUPPORT);
        else
            pHP->ulRuntimeFlag |= APMPOWERSUPPORT;
        pHP->ulAPMStatus=MAKELONG((SHORT)powerstatusAPM.ucACStatus, (SHORT)powerstatusAPM.ucBatteryLife);
        }
    else
        pHP->ulRuntimeFlag &= (~APMPOWERSUPPORT);
                                    /* If APM status changed, update SessionBar window */
    if(((ulRuntimeFlagPrevious & APMPOWERSUPPORT)!=(pHP->ulRuntimeFlag & APMPOWERSUPPORT)) &&
        (pHP->hwndSessionClient!=NULLHANDLE))
        {
        WinSendMsg(pHP->hwndSessionClient, WM_SESSIONBAR, NULL, NULL);
        WinInvalidateRect(pHP->hwndSessionClient, NULL, FALSE);
        }
    CHECKPOINT(WTrd, 0x0000);
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
#ifdef  DEBUG_WTRD
        printf("WTrd: WM_SETPRESPARAMFONT\n");
#endif  /* DEBUG_WTRD */
                                        /* Set Overview window font */
        SetPresParamFont(HPSOVERVIEWDRAW, (FATTRS *)PVOIDFROMMP(mp2), LCID_FONT);
                                        /* Repaint Overview window */
        WinInvalidateRect(pHP->hwndClient, NULL, FALSE);
        }
    break;

/*                                                                                      *\
 * Syntax: WM_AUTOSTART, ULONG ulUpTimeSeconds, NULL                                    *
\*                                                                                      */
case WM_AUTOSTART:
/*                                                                                      *\
 * This message is posted during initialization of the working thread to autostart all  *
 * menu items that have the autostart flag.                                             *
 * Ref.:                                                                                *
 *      pPopupMenu                                                                      *
\*                                                                                      */
    {
    ULONG   ulAutostartCount=0;

    CHECKPOINT(WTrd, 0x0400);
                                        /* Scan recursively through Popup-Menu and start
                                           all applications that have the autostart flag
                                           by simulating a selection of the application from
                                           the Popup-Menu (of course only if the optional
                                           autostart delay has expired) */
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_AUTOSTART\n");
#endif  /* DEBUG_WTRD */
    SearchAutoStartItem(pHP->pPopupMenu, &ulAutostartCount, (ULONG)mp1);
                                        /* If there are no more items left to be Autostarted,
                                           then we can skip further checking at the WM_TIMER
                                           events */
    if(ulAutostartCount==0)
        {
#ifdef  DEBUG
        pHP->ulRuntimeFlag|=NOPENDINGAUTOSTART;
        printf("WTrd: WM_AUTOSTART no more Autostart pending\n");
#endif  /* DEBUG */
        }
    CHECKPOINT(WTrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_PAINT, NULL, NULL                                                         *
\*                                                                                      */
case WM_PAINT:
/*                                                                                      *\
 * This message is send by PC/2's client window when a repaint is needed.               *
 * Ref.:                                                                                *
 *          none                                                                        *
\*                                                                                      */
    {
    RECTL       rcClient;               /* Rectangle to redraw */
    LONG        lX, lY;                 /* Position (x|y) on client area */
    float       fXSize, fYSize;         /* Length on client area */
    float       fOriginX,fOriginY;      /* Current desktop origin in overview window coordinates */
    SWP         swpWindow;              /* Any window to be drawn on Overview window */
    WINDOWDATA  *pwdCurrent=0;          /* WINDOWDATA structure of current Desktop child window */
    LONG        lWindowIndex=0;         /* Index in WINDOWDATA structure */
                                        /* Saved PS clipping region */
    HRGN        hrgnSaveClip=NULLHANDLE;
    LONG        lVisibleWindows=0;      /* Count total number of windows */
    LONG        lCurrentWindow;         /* Current window number */

#ifndef SLOWVIDEO
    POINTL      ptl;
#endif /* SLOWVIDEO */

    CHECKPOINT(WTrd, 0x0500);
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_PAINT\n");
#endif  /* DEBUG_WTRD */

                                        /* It seems that during PC/2 2.09e Beta testing somehow,
                                           we got an Overview window zero in one or two dimensions,
                                           causing a floating point exception during when adding
                                           1 to (swpWindow.x/fXSize). This could only happen when
                                           fXSize was near 0 giving an overflow so adding anything
                                           causes an exception. How we got a 0 sized Overview window
                                           I have no idea.
    1                                      For a hidden Overview window we don't need to draw at all */
    if((WinIsWindowVisible(pHP->hwndFrame)==FALSE) ||
        (pHP->swpPC2Client.cx==0) ||
        (pHP->swpPC2Client.cy==0))
        break;
                                        /* Set background to dialog background */
    WinQueryWindowRect(pHP->hwndClient, &rcClient);
    WinFillRect(HPSOVERVIEWDRAW, &rcClient, pHP->ulOverviewRGBColorBgnd);
                                        /* Now get scale factor to scale virtual Desktop
                                           to client area */
    pHP->fScaleX=(float)(pHP->swpPC2Client.cx)/
        (pHP->ulHorizontalDesktops*pHP->DesktopSize.x);
    pHP->fScaleY=(float)(pHP->swpPC2Client.cy)/
        (pHP->ulVerticalDesktops*pHP->DesktopSize.y);
                                        /* Get coordinates (0|0) origin, relativ to (0|0) of
                                           the Virtual Desktop */
    fOriginX=(pHP->VirtualDesktopPos.x-pHP->VirtualDesktopMin.x)*pHP->fScaleX;
    fOriginY=(pHP->VirtualDesktopPos.y-pHP->VirtualDesktopMin.y)*pHP->fScaleY;
    pHP->ptlOrigin.x=fOriginX;
    pHP->ptlOrigin.y=fOriginY;
                                        /* Get size of one of the number of Desktops that must fit into
                                           the client area */
    fXSize=(float)pHP->swpPC2Client.cx/pHP->ulHorizontalDesktops;
    fYSize=(float)pHP->swpPC2Client.cy/pHP->ulVerticalDesktops;
                                        /* Create clipping region */
    if(!(pHP->ulStatusFlag1 & OVERVIEW3DBORDER))
        {
        RECTL   rectl;

        hrgnSaveClip=GpiQueryClipRegion(HPSOVERVIEWDRAW);
        rectl.xLeft=1;
        rectl.xRight=(pHP->ulHorizontalDesktops)*fXSize-2;
        rectl.yBottom=1;
        rectl.yTop=(pHP->ulVerticalDesktops)*fYSize-2;
        GpiIntersectClipRectangle(HPSOVERVIEWDRAW, &rectl);
        }
                                        /* If WPS is expanded to whole Virtual Desktop draw Desktop
                                           first before drawing the borders of the Virtual Desktop */
    pwdCurrent=GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_DESKTOP, 0);
    swpWindow.fl=0;
    if((pHP->ulStatusFlag1 & EXPANDWPS) && (pwdCurrent!=0))
        {
                                        /* Calculate and draw window */
        swpWindow.x=fOriginX+(float)pwdCurrent->pswpWindow->x*pHP->fScaleX;
        swpWindow.y=fOriginY+(float)pwdCurrent->pswpWindow->y*pHP->fScaleY;
        swpWindow.cx=(float)pwdCurrent->pswpWindow->cx*pHP->fScaleX;
        swpWindow.cy=(float)pwdCurrent->pswpWindow->cy*pHP->fScaleY;
                                        /* Display Window List title or titlebar */
        if(strlen(pwdCurrent->ucWindowTitle))
            DrawWindow(HPSOVERVIEWDRAW, &swpWindow, FOREGROUND_WND, NULLHANDLE, pwdCurrent->ucWindowTitle, 6, 6);
        else
            DrawWindow(HPSOVERVIEWDRAW, &swpWindow, FOREGROUND_WND, NULLHANDLE, pwdCurrent->ucPgmTitle, 6, 6);
        }
                                        /* Draw all Virtual Desktops */
    for(lX=0; lX<(signed)pHP->ulHorizontalDesktops; lX++)
        for(lY=0; lY<(signed)pHP->ulVerticalDesktops; lY++)
            {
            swpWindow.x=lX*fXSize;
            swpWindow.y=lY*fYSize;
            swpWindow.cx=(lX+1)*fXSize-swpWindow.x-1;
            swpWindow.cy=(lY+1)*fYSize-swpWindow.y-1;
            DrawWindow(HPSOVERVIEWDRAW, &swpWindow,
                (pHP->ulStatusFlag1 & EXPANDWPS) ? BACKGROUND_WND | BORDERONLY_WND : BACKGROUND_WND,
                NULLHANDLE, NULL, 6, 6);
            }
                                        /* Destroy clipping region (if set) */
    if(!(pHP->ulStatusFlag1 & OVERVIEW3DBORDER))
        {
        GpiSetClipRegion(HPSOVERVIEWDRAW, hrgnSaveClip, &hrgnSaveClip);
        GpiDestroyRegion(HPSOVERVIEWDRAW, hrgnSaveClip);
        }
                                        /* Draw physical Desktop window (in white, as we can't change
                                           more than 1 foreground and 1 background color using the
                                           font palette as there is no way to tell which color the user
                                           wants to change) */
    swpWindow.x=fOriginX;
    swpWindow.y=fOriginY;
    swpWindow.cx=((swpWindow.x/fXSize)+1)*fXSize-swpWindow.x-1;
    swpWindow.cy=((swpWindow.y/fYSize)+1)*fYSize-swpWindow.y-1;
    DrawWindow(HPSOVERVIEWDRAW, &swpWindow, FOREGROUND_WND, NULLHANDLE, "Display", 6, 6);
                                        /* Count visible windows to display gradient correctly */
    for(lWindowIndex=pHP->Windows.lWindowLast;
        lWindowIndex!=-1 && pHP->Windows.lWindowLast!=-1;
        lWindowIndex--)
        {
        pwdCurrent=GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_INDEX, (MPARAM)lWindowIndex);
                                        /* Ignore invisible windows and desktop */
        if((!(pwdCurrent->ulStatus & VISIBLE)) ||
            (lWindowIndex==pHP->Windows.lDesktop))
            continue;
                                        /* Perform some primitive clipping */
        if((pwdCurrent->pswpWindow->x+pwdCurrent->pswpWindow->cx<(pHP->VirtualDesktopMin.x-pHP->VirtualDesktopPos.x)) ||
            (pwdCurrent->pswpWindow->y+pwdCurrent->pswpWindow->cy<(pHP->VirtualDesktopMin.y-pHP->VirtualDesktopPos.y)) ||
            (pwdCurrent->pswpWindow->x>(pHP->VirtualDesktopMax.x-pHP->VirtualDesktopPos.x)) ||
            (pwdCurrent->pswpWindow->y>(pHP->VirtualDesktopMax.y-pHP->VirtualDesktopPos.y)))
            continue;
        lVisibleWindows++;
        }
    lCurrentWindow = lVisibleWindows;
                                        /* Now display the windows from bottommost to topmost
                                           to display the real z-order in the Overview window */
    for(lWindowIndex=pHP->Windows.lWindowLast;
        lWindowIndex!=-1 && pHP->Windows.lWindowLast!=-1;
        lWindowIndex--)
        {
        pwdCurrent=GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_INDEX, (MPARAM)lWindowIndex);
                                        /* Ignore invisible windows and desktop */
        if((!(pwdCurrent->ulStatus & VISIBLE)) ||
            (lWindowIndex==pHP->Windows.lDesktop))
            continue;
                                        /* Perform some primitive clipping */
        if((pwdCurrent->pswpWindow->x+pwdCurrent->pswpWindow->cx<(pHP->VirtualDesktopMin.x-pHP->VirtualDesktopPos.x)) ||
            (pwdCurrent->pswpWindow->y+pwdCurrent->pswpWindow->cy<(pHP->VirtualDesktopMin.y-pHP->VirtualDesktopPos.y)) ||
            (pwdCurrent->pswpWindow->x>(pHP->VirtualDesktopMax.x-pHP->VirtualDesktopPos.x)) ||
            (pwdCurrent->pswpWindow->y>(pHP->VirtualDesktopMax.y-pHP->VirtualDesktopPos.y)))
            continue;
                                        /* Calculate and draw window */
        swpWindow.x=fOriginX+(float)pwdCurrent->pswpWindow->x*pHP->fScaleX;
        swpWindow.y=fOriginY+(float)pwdCurrent->pswpWindow->y*pHP->fScaleY;
        swpWindow.cx=(float)pwdCurrent->pswpWindow->cx*pHP->fScaleX;
        swpWindow.cy=(float)pwdCurrent->pswpWindow->cy*pHP->fScaleY;
        swpWindow.fl=(pwdCurrent->pswpWindow->hwnd==WinQueryActiveWindow(HWND_DESKTOP) ? SWP_ACTIVATE : 0);
                                        /* Display Window List title or titlebar */
        if(lCurrentWindow)
            lCurrentWindow--;
        if(strlen(pwdCurrent->ucWindowTitle))
            DrawWindow(HPSOVERVIEWDRAW, &swpWindow, FOREGROUND_WND, pwdCurrent->hptrIcon, pwdCurrent->ucWindowTitle, lCurrentWindow, lVisibleWindows);
        else
            DrawWindow(HPSOVERVIEWDRAW, &swpWindow, FOREGROUND_WND, pwdCurrent->hptrIcon, pwdCurrent->ucPgmTitle, lCurrentWindow, lVisibleWindows);
        }
#ifndef SLOWVIDEO
                                        /* Draw the whole memory bitmap into the client area,
                                           without any background or foreground color because
                                           we have no monochrome bitmap */
    ptl.x=ptl.y=0;
    WinDrawBitmap(hpsClient, pHP->hbmOverviewMemory, NULL, &ptl, 0L, 0L, DBM_NORMAL);
#endif /* SLOWVIDEO */
    CHECKPOINT(WTrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_DESKTOPMOVE, LONG SlidingXFactor, LONG SlidingYFactor                     *
\*                                                                                      */
case WM_DESKTOPMOVE:
/*                                                                                      *\
 * The hook found that the pointer was over one of the border rows and/or columns of    *
 * the physical Desktop or the user doubleclicked on a virtual Desktop on the Overview  *
 * window. The passed parameter mp1 contains the number of pixels to slide all windows  *
 * on the virtual Desktop horizontally. We calculate if and where the physical Desktop  *
 * must be moved, or ignore it, if we're allready at a border position of the virtual   *
 * Desktop. If move request is finished successfully, WM_THREADREADY is posted.         *
\*                                                                                      */
    {
                                        /* Slide in x direction in pixels */
    LONG        lSlidingXFactor=LONGFROMMP(mp1);
                                        /* Slide in y direction in pixels */
    LONG        lSlidingYFactor=LONGFROMMP(mp2);
    WINDOWDATA  *pwdCurrent=0;          /* WINDOWDATA structure of current Desktop child window */
    SWP         *pswpWindows=0;         /* Dynamically allocated array of the windows to be moved by
                                           PM, which, as the Physical Desktop moves in the opposite
                                           direction keeps the windows absolute position within the Virtual
                                           Desktop (that is the Overview window) */
    ULONG       ulWindowsCount=0;       /* Number of actual windows in pswpWindows[] to be moved */
    SWP         *pswpOwnedWindows=0;    /* Dynamically allocated array of owned windows to be moved. Owned
                                           windows get moved after their owner, because moving the owner
                                           also moves the ownee, but can corrupt or misplace the ownee */
    ULONG       ulOwnedWindowsCount=0;  /* Number of actual owned windows in pswpOwnedWindows[] to be moved */
    LONG        lWindowIndex=0;         /* Index in WINDOWDATA structure */

    CHECKPOINT(WTrd, 0x0600);
                                        /* Due to autoscroll, we may get more requests to slide than that are
                                           possible to keep within the limits of the whole Virtual Desktop */
    if(lSlidingXFactor>0)
        {
        if(pHP->VirtualDesktopPos.x<=pHP->VirtualDesktopMin.x)
            lSlidingXFactor=0;
        }
    else
        {
        if(pHP->VirtualDesktopPos.x>=(pHP->VirtualDesktopMax.x-pHP->DesktopSize.x))
            lSlidingXFactor=0;
        }
    if(lSlidingYFactor>0)
        {
        if(pHP->VirtualDesktopPos.y<=pHP->VirtualDesktopMin.y)
            lSlidingYFactor=0;
        }
    else
        {
        if(pHP->VirtualDesktopPos.y>=(pHP->VirtualDesktopMax.y-pHP->DesktopSize.y))
            lSlidingYFactor=0;
        }
    if(pHP->ulDebug>=DEBUG_FULL)
        printf("WTrd: WM_DESKTOPMOVE %lx:%lx\n", lSlidingXFactor, lSlidingYFactor);
                                        /* Now move all windows when required */
    if(lSlidingXFactor || lSlidingYFactor)
        {
                                        /* Begin with first window and allocate space for all windows possible,
                                           but not all windows may be included (because of being marked to stay on
                                           active Desktop,...). Note: WINDOWS.lWindowLast is 0 based, so add 1)  */
        pswpWindows=(SWP *)malloc((pHP->Windows.lWindowLast+1)*sizeof(SWP));
        ulWindowsCount=0;
                                        /* Same for owned windows */
        pswpOwnedWindows=(SWP *)malloc((pHP->Windows.lWindowLast+1)*sizeof(SWP));
        ulOwnedWindowsCount=0;
                                        /* From all windows (index is 0 based) select the ones we are
                                           interested to move */
        for(lWindowIndex=0; lWindowIndex<=pHP->Windows.lWindowLast; lWindowIndex++)
            {
            pwdCurrent=GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_INDEX, MPFROMLONG(lWindowIndex));

                                        /* Ignore known windows not being a movable frame window. During
                                           enumeration of all frame child windows of the Desktop, we already
                                           have marked our "special" windows (PS/2 windows, Window List,
                                           QuickSwitch window, windows to keep on active Desktop, ...) being
                                           non-moveable that is having not the style FRAMECLASS. So we can
                                           save some CPU here due to not testing this again. */
            if(!(pwdCurrent->ulStatus & FRAMECLASS))
                continue;
                                        /* Only move certain windows */
            pwdCurrent->pswpWindow->x+=lSlidingXFactor;
            pwdCurrent->pswpWindow->y+=lSlidingYFactor;
                                        /* Move window only if not marked non-movable. Such a window
                                           appears on every Virtual Desktop, that is, it does not appear
                                           to be moved out of the Physical Desktop.
                                           We have to differentiate if the current window is a ownee
                                           window, because this window has to be moved after its owner
                                           has been moved successfully */
            if((WinQueryWindow(pwdCurrent->pswpWindow->hwnd, QW_OWNER)!=NULLHANDLE) &&
                (!(pwdCurrent->SwpFlag & SWP_NOMOVE)))
                {
                pwdCurrent->pswpWindow->fl=SWP_MOVE | SWP_NOADJUST;
                                        /* Next ownee window to be moved by WinSetMultWindowPos() */
                pswpOwnedWindows[ulOwnedWindowsCount++]=*(pwdCurrent->pswpWindow);
                continue;
                }
            if(!(pwdCurrent->SwpFlag & SWP_NOMOVE))
                {
                pwdCurrent->pswpWindow->fl=SWP_MOVE | SWP_NOADJUST;
                                        /* Next not owned window to be moved by WinSetMultWindowPos() */
                pswpWindows[ulWindowsCount++]=*(pwdCurrent->pswpWindow);
                continue;
                }
            }
                                        /* First move the not owned windows, that is windows that are
                                           child frame windows of the Desktop and have no owner window.
                                           If the window is the owner of another window, the owned windows
                                           will not get moved correctly (that is, the stay at the border
                                           of the Physical Desktop they appeared before moving) */
        if(WinSetMultWindowPos(pThreadData->habThread, pswpWindows, ulWindowsCount))
            {
                                        /* Now move the owned windows, because when moving their owner,
                                           they (usually) don't get moved correctly (either they change their
                                           relative position to their owner, or the controls inside the ownee
                                           are moved outside the ownee (verified with PMTREE) if
                                           WinSetMultWindowPos() is applied to the ownee - possible reason is
                                           that the controls are kept relative to the owner by PM? */
            if(!WinSetMultWindowPos(pThreadData->habThread, pswpOwnedWindows, ulOwnedWindowsCount))
                {
                if(pHP->ulDebug>=DEBUG_ENTRY)
                    printf("WTrd: Ownees WinSetMultWindowPos()  failed\n");
                }
                                        /* Adjust Physical Desktop within Virtual Desktop */
            pHP->VirtualDesktopPos.x-=lSlidingXFactor;
            pHP->VirtualDesktopPos.y-=lSlidingYFactor;
                                        /* Windows have changed, request repaint client area and inform
                                           SessionBar */
            WinSendMsg(hwnd, WM_PAINT, NULL, NULL);
                                        /* Indicate that working thread is ready after moving windows
                                           from one Virtual Desktop to another */
            if(!WinPostMsg(pHP->hwndClient, WM_THREADREADY, MPFROMLONG(THREADMOVEBUSY), NULL))
                WinSendMsg(pHP->hwndClient, WM_THREADREADY, MPFROMLONG(THREADMOVEBUSY), NULL);
            }
        else
            {
            if(pHP->ulDebug>=DEBUG_ENTRY)
                printf("WTrd: WinSetMultWindowPos() failed\n");
                                        /* If windows weren't successfully moved retry,
                                           after waiting a little bit */
            DosSleep(50);
                                        /* WinSetMultWindowPos() may have failed because a window
                                           was destroyed between enumerating them and moving them */
            WinPostMsg(hwnd, WM_SETUPSIZEPOSITION, MPFROMLONG(WM_MOVE), NULL);
            WinPostMsg(hwnd, WM_DESKTOPMOVE, MPFROMLONG(lSlidingXFactor), MPFROMLONG(lSlidingYFactor));
            }
                                        /* Free allocated arrays */
        free(pswpWindows);
        free(pswpOwnedWindows);
        }
    else
        {
                                        /* As windows changed relative to the Physical Desktop (which has
                                           of course not been moved as the Sliding*Factors are 0), request
                                           repaint client area and inform SessionBar */
        WinPostMsg(pHP->hwndSessionClient, WM_DESKTOPMOVE, NULL, NULL);
        WinSendMsg(hwnd, WM_PAINT, NULL, NULL);
                                        /* Indicate that working thread is ready after enumerating
                                           windows */
        if(!WinPostMsg(pHP->hwndClient, WM_THREADREADY, MPFROMLONG(THREADWINDOWBUSY), NULL))
            WinSendMsg(pHP->hwndClient, WM_THREADREADY, MPFROMLONG(THREADWINDOWBUSY), NULL);
        }
    CHECKPOINT(WTrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SETDESKTOPHANDLE, HWND hwndWPSFrame, NULL                                 *
\*                                                                                      */
case WM_SETDESKTOPHANDLE:
/*                                                                                      *\
 * The WPS must have been initialized, as it's frame window text was set. Now we have   *
 * just find the client window handle to allow detection of mouse button events to be   *
 * able to detect request for the Popup-Menu. We are interested in the container's      *
 * window handle that implements the WPS's client window.                               *
\*                                                                                      */
    {
    UCHAR       ucClass[8];             /* Save class name here */
    HWND        hwndWPSFrame;           /* Save WPS frame client handle */
    HWND        hwndWPSClient;          /* Save WPS window client handle */
    HWND        hwndDesktop;            /* PM window handle */

    CHECKPOINT(WTrd, 0x0700);
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_SETDEKTOPHANDLE\n");
#endif  /* DEBUG_WTRD */
                                        /* Get to bottommost window handle of the "Desktop". If
                                           it has a container control as client window then we
                                           have found the WPS */
    hwndDesktop=WinQueryWindow(HWND_DESKTOP, QW_BOTTOM);
                                        /* Either we have to find the WPS or we got a hint from
                                           PC2HOOK.DLL */
    if(HWNDFROMMP(mp1)==NULLHANDLE)
        {
        hwndWPSFrame=hwndDesktop;
        hwndWPSClient=WinWindowFromID(hwndDesktop, FID_CLIENT);
        }
    else
        {
        hwndWPSFrame=HWNDFROMMP(mp1);
        hwndWPSClient=WinWindowFromID(hwndWPSFrame, FID_CLIENT);
        }
                                        /* Verify that this really is the WPS. If verification
                                           fails, then for now continue with the window we
                                           previously have identified as the WPS */
    WinQueryClassName(hwndWPSClient, sizeof(ucClass), (PCH)ucClass);
    if((hwndDesktop!=hwndWPSFrame) || (strcmp(ucClass, DESKTOP_CLASS)))
        hwndWPSClient=pHP->hwndWPSClient;
/*                                                                                      *\
 * Now get the PM window handle for the case, that the WPS is not installed, or moved   *
 * outwards of the display (by setting the move Desktop checkbox).                      *
\*                                                                                      */
                                        /* Without WPS installed we can only get the
                                           "Desktop" window handle */
    hwndDesktop=WinQueryDesktopWindow(pThreadData->habThread, NULLHANDLE);
                                        /* If a new WPS was detected and the WPS should be
                                           expanded to all Virtual Desktops, request resizing */
    if((hwndWPSClient!=pHP->hwndWPSClient) || (hwndDesktop!=pHP->hwndDesktop))
        {
        pHP->hwndWPSFrame=hwndWPSFrame;
        pHP->hwndWPSClient=hwndWPSClient;
        pHP->hwndDesktop=hwndDesktop;
        if(pHP->ulStatusFlag1 & EXPANDWPS)
            WinPostMsg(hwnd, WM_EXPANDWPS, (MPARAM)TRUE, NULL);
        }
    CHECKPOINT(WTrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SETUPSIZEPOSITION, ULONG ulMsg, HWND hwndWindow                           *
\*                                                                                      */
case WM_SETUPSIZEPOSITION:
/*                                                                                      *\
 * This message is posted as a result of a user event in the hook. All windows running  *
 * in the system are queried and registered in the Windows control structure.           *                                                                           *
 * The Window List is queried to find the Window List entries of the PM windows.        *
 * Ref.:                                                                                *
 *          Windows ........... WINDOWS structure containing all windows control data   *
\*                                                                                      */
    {
    CHECKPOINT(WTrd, 0x0800);
    if(pHP->ulDebug>=DEBUG_FULL)
        {
        printf("WTrd: WM_SETUPSIZEPOSITION ");
        if(LONGFROMMP(mp1)==WM_MOVE)
            printf("(WM_MOVE) ");
        else
            printf("(Desktop Enumeration) ");
        printf("\n");
        }
                                        /* Query all child windows of HWND_DESKTOP for PC/2's
                                           Overview window */
    if(ulClosingThread==FALSE)
        {
        CHECKPOINT(WTrd, 0x0801);
        GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_QUERY, 0);
        CHECKPOINT(WTrd, 0x0802);
        if(!WinPostMsg(hwnd, WM_DESKTOPMOVE, MPFROMLONG(0), MPFROMLONG(0)))
            WinSendMsg(hwnd, WM_DESKTOPMOVE, MPFROMLONG(0), MPFROMLONG(0));
        }
    }
    CHECKPOINT(WTrd, 0x0000);
    break;

/*                                                                                      *\
 * Syntax: WM_BUTTON1CLICK, (LONG mp1), (LONG mp2)                                      *
 * Syntax: WM_BUTTON1DBLCLK, (LONG mp1), (LONG mp2)                                     *
\*                                                                                      */
case WM_BUTTON1CLICK:
case WM_BUTTON1DBLCLK:
/*                                                                                      *\
 * This message detected and passed from the PC/2 window procedure is used to switch    *
 * between Virtual Desktops. Depending on the ONECLICKSWITCH we watch for single- or    *
 * doubleclicks.                                                                        *
 * Ref.:                                                                                *
 *          Windows ........... WINDOWS structure containing all windows control data   *
\*                                                                                      */
    {
    LONG        lClickX, lClickY;       /* Pointer position during click */
    LONG        lSlidingXFactor;        /* Slide in x direction in pixels */
    LONG        lSlidingYFactor;        /* Slide in y direction in pixels */

    CHECKPOINT(WTrd, 0x0900);
                                        /* Ignore request when not the one we're waiting for
                                           (though it shouldn't be posted anyway) */
    if(msg!=DESKTOP_SWITCH_EVENT)
        break;
    if(pHP->ulDebug>=DEBUG_FULL)
        printf("WTrd: WM_BUTTON1(CLICK|DBLCLK)\n");
                                        /* Get the virtual Desktop the user doubleclicked on */
    lClickX=pHP->VirtualDesktopMin.x;
    lClickY=pHP->VirtualDesktopMin.y;
    lClickX+=((LONG)(SHORT1FROMMP(mp1)))/
        (pHP->swpPC2Client.cx/pHP->ulHorizontalDesktops)*pHP->DesktopSize.x;
    lClickY+=((LONG)(SHORT2FROMMP(mp1)))/
        (pHP->swpPC2Client.cy/pHP->ulVerticalDesktops)*pHP->DesktopSize.y;
                                        /* The right and top borders are the limit (necessary because
                                           the frame rectangle is sized pointwise and the virtual Desktops
                                           drawn are sized every n points leaving 0 to n-1 points outside.
                                           Can be fixed by allowing the frame also sized only every n
                                           points, but this requires subclassing of the frame. */
    if(lClickX>(pHP->VirtualDesktopMax.x-pHP->DesktopSize.x))
        lClickX=(pHP->VirtualDesktopMax.x-pHP->DesktopSize.x);
    if(lClickY>(pHP->VirtualDesktopMax.y-pHP->DesktopSize.y))
        lClickY=(pHP->VirtualDesktopMax.y-pHP->DesktopSize.y);
                                        /* Calculate Desktop move in pixel */
    lSlidingXFactor=pHP->VirtualDesktopPos.x-lClickX;
    lSlidingYFactor=pHP->VirtualDesktopPos.y-lClickY;
    if(lSlidingXFactor || lSlidingYFactor)
        {
                                        /* Now move the windows */
        WinPostMsg(pHP->hwndWorkingThread, WM_DESKTOPMOVE, MPFROMLONG(lSlidingXFactor), MPFROMLONG(lSlidingYFactor));
                                        /* And switch to non-PC/2 window on top of Z-Order */
        WinPostMsg(pHP->hwndWorkingThread, WM_QUICKSWITCH, MPFROMLONG(QUICKSWITCH_ACTIVATE), NULL);
        }
    else
                                        /* Indicate that working thread is ready because it doesn't need
                                           to move windows from one Virtual Desktop to another */
        if(!WinPostMsg(pHP->hwndClient, WM_THREADREADY, MPFROMLONG(THREADMOVEBUSY), NULL))
            WinSendMsg(pHP->hwndClient, WM_THREADREADY, MPFROMLONG(THREADMOVEBUSY), NULL);
    }
    CHECKPOINT(WTrd, 0x0000);
    break;

/*                                                                                      *\
 * Syntax: WM_HOTKEY, (USHORT usFlags, USHORT usCh), ULONG ulKeyDataIndex               *
 *                                                                                      *
\*                                                                                      */
case WM_HOTKEY:
/*                                                                                      *\
 * WM_HOTKEY is a WM_CHAR message, passed when a hotkey was detected in PC2Hook.DLL.    *
 * The key passed from the input hook was flaged as used. Either the running program    *
 * corresponding to the hotkey is switched into the foreground, or if it is not already *
 * running it is started.                                                               *
 * Ref.:                                                                                *
 *          Windows ........... WINDOWS structure containing all windows control data   *
\*                                                                                      */
    {
                                        /* Index in KeyData of HotKey found */
    ULONG       ulKeyDataIndex=LONGFROMMP(mp2);
    WINDOWDATA  *pwdCurrent;            /* WINDOWDATA of window the Hotkey is defined for */

    if(pHP->ulDebug>=DEBUG_FULL)
        printf("WTrd: WM_HOTKEY\n");
                                        /* Get WINDOWDATA of window the Hotkey is defined for */
    pwdCurrent=GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_HOTKEY, mp1);
                                        /* Switch to and activate the window found or launch application,
                                           if window can't be found */
    if(pwdCurrent!=0)
        GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_SWITCHTO, MPFROMHWND(pwdCurrent->pswpWindow->hwnd));
    else
        {                               /* If no session found for the pressed hotkey, start
                                           the corresponding session */
        if((ulKeyDataIndex<KEYDATACOUNT) && ((pHP->pKeyData+ulKeyDataIndex)->pMenuData!=0))
            WinPostMsg(pHP->hwndClient, WM_COMMAND,
                                        /* Get ID and set it as posted by a menu control */
                MPFROM2SHORT((USHORT)((pHP->pKeyData+ulKeyDataIndex)->pMenuData->id),
                    CMDSRC_MENU),
                                        /* Simulate Message a result of a keyboard operation */
                MPFROMCHAR(FALSE));
        }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_WINDOWLIST, (USHORT x, USHORT y), NULL                                    *
\*                                                                                      */
case WM_WINDOWLIST:
/*                                                                                      *\
 * WM_WINDIWLIST is sent by PC2Hook.dll and passed from PC/2's window procedure, if     *
 * mouse clicks are detected on PM that would display the Window List on the WPS. Be-   *
 * cause the WPS displayed the Window List but not PM, we simulate this on PM.          *
 * Ref.:                                                                                *
 *          Windows ........... WINDOWS structure containing all windows control data   *
\*                                                                                      */
    {                                   /* Mouse position when Window List was requested */
    USHORT      usX=SHORT1FROMMP(mp1), usY=SHORT2FROMMP(mp1);
    WINDOWDATA  *pwdCurrent=0;          /* WINDOWDATA structure of current Desktop child window */

    CHECKPOINT(WTrd, 0x0A00);
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_WINDOWLIST\n");
#endif  /* DEBUG_WTRD */
                                        /* Display Window List */
    if((pwdCurrent=(WINDOWDATA *)GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_WINDOWLIST, 0))!=0)
        {
                                        /* Place Window List centered under mouse pointer
                                           whenever possible */
        if((pwdCurrent->pswpWindow->cx>>1)>usX)
            usX=0;
        else if(pHP->DesktopSize.x-(pwdCurrent->pswpWindow->cx>>1)<usX)
            usX=pHP->DesktopSize.x-pwdCurrent->pswpWindow->cx;
        else usX=usX-(pwdCurrent->pswpWindow->cx>>1);
        if((pwdCurrent->pswpWindow->cy>>1)>usY)
            usY=0;
        else if(pHP->DesktopSize.y-(pwdCurrent->pswpWindow->cy>>1)<usY)
            usY=pHP->DesktopSize.y-pwdCurrent->pswpWindow->cy;
        else usY=usY-(pwdCurrent->pswpWindow->cy>>1);
        pwdCurrent->pswpWindow->fl=
            SWP_MOVE|SWP_SHOW|SWP_ZORDER|SWP_NOADJUST;
        pwdCurrent->pswpWindow->x=usX;
        pwdCurrent->pswpWindow->y=usY;
        pwdCurrent->pswpWindow->hwndInsertBehind=HWND_TOP;
        WinSetMultWindowPos(pThreadData->habThread, pwdCurrent->pswpWindow, 1);
        if(pwdCurrent->hswitchWindow)
            WinSwitchToProgram(pwdCurrent->hswitchWindow);
        }
    CHECKPOINT(WTrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_MOVEREQUEST, (USHORT usMouseXPos, USHORT usMouseYPos), (ULONG ulMoveFlag) *
\*                                                                                      */
case WM_MOVEREQUEST:
/*                                                                                      *\
 * This local procedure is called from the PC2DLL_Hook procedure to move the windows    *
 * within the virtual Desktop on its behalf. Also due to using ALT+CTRL+SHIFT+cursor we *
 * have to check to stay within the limits of the overall Virtual Desktop.              *
 * Req.:                                                                                *
 *      usMouseXPos.... X Position of mouse pointer during creation of move message     *
 *      usMouseYPos.... Y Position of mouse pointer during creation of move message     *
 *      ulMoveFlag..... Bitmapped flag to control move                                  *
 * Returns:                                                                             *
 *      none                                                                            *
 * Ref.:                                                                                *
 *      Windows ....... WINDOWS structure containing all windows control data           *
\*                                                                                      */
    {
                                        /* Get pointer position */
    LONG        lMouseXPos=(LONG)SHORT1FROMMP(mp1);
    LONG        lMouseYPos=(LONG)SHORT2FROMMP(mp1);
    POINTL      VirtualDesktopPos;      /* Copy structure from HookParameters */
                                        /* Get bitmapped move control flag */
    ULONG       ulMoveFlag=LONGFROMMP(mp2);
    LONG        lSlidingXFactor;        /* Horizontal offset to move */
    LONG        lSlidingYFactor;        /* Vertical offset to move */
    LONG        lDiff;                  /* Difference between movement and Desktop size */

    CHECKPOINT(WTrd, 0x0B00);
    if(pHP->ulDebug>=DEBUG_FULL)
        printf("WTrd: WM_MOVEREQUEST\n");
    lSlidingXFactor=0;
    lSlidingYFactor=0;
    VirtualDesktopPos=pHP->VirtualDesktopPos;
    if((ulMoveFlag & MOVEXR))
        {                               /* Move physical Desktop left, but not over the
                                           left border of the virtual Desktop */
        lSlidingXFactor=pHP->SlidingXFactor;
        VirtualDesktopPos.x-=pHP->SlidingXFactor;
        lDiff=VirtualDesktopPos.x-pHP->VirtualDesktopMin.x;
        if(lDiff<0)
            {
            lSlidingXFactor+=lDiff;
            }
        }
    if((ulMoveFlag & MOVEXL))
        {                               /* Move physical Desktop right, but not over the
                                           right border of the virtual Desktop */
        lSlidingXFactor=-pHP->SlidingXFactor;
        VirtualDesktopPos.x+=pHP->SlidingXFactor;
        lDiff=VirtualDesktopPos.x-(pHP->VirtualDesktopMax.x-pHP->DesktopSize.x);
        if(lDiff>0)
            {
            lSlidingXFactor+=lDiff;
            }
        }
    if((ulMoveFlag & MOVEYU))
        {                               /* Move physical Desktop down, but not under the
                                           bottom border of the virtual Desktop */
        lSlidingYFactor=pHP->SlidingYFactor;
        VirtualDesktopPos.y-=pHP->SlidingYFactor;
        lDiff=VirtualDesktopPos.y-pHP->VirtualDesktopMin.y;
        if(lDiff<0)
            {
            lSlidingYFactor+=lDiff;
            }
        }
    if((ulMoveFlag & MOVEYD))
        {                               /* Move physical Desktop up, but not over the
                                           top border of the virtual Desktop */
        lSlidingYFactor=-pHP->SlidingYFactor;
        VirtualDesktopPos.y+=pHP->SlidingYFactor;
        lDiff=VirtualDesktopPos.y-(pHP->VirtualDesktopMax.y-pHP->DesktopSize.y);
        if(lDiff>0)
            {
            lSlidingYFactor+=lDiff;
            }
        }
                                        /* If there is nothing to move, because we are
                                           on a border position, don't do further processing
                                           but return */
    if(lSlidingXFactor || lSlidingYFactor)
        {
                                        /* Inform working thread to move windows */
        WinPostMsg(pHP->hwndWorkingThread, WM_DESKTOPMOVE,
            MPFROMLONG(lSlidingXFactor), MPFROMLONG(lSlidingYFactor));
                                        /* And afterwards try to set the input focus to the window
                                           on top of Z-Order */
        WinPostMsg(pHP->hwndWorkingThread, WM_QUICKSWITCH, MPFROMLONG(QUICKSWITCH_ACTIVATE), NULL);
        if(!(pHP->ulStatusFlag1 & AUTOSCROLL))
            {
                                        /* If we don't want to autoscroll, move pointer so that it is
                                           no longer on the surrounding row and/or column, that is on that
                                           pixel the pointer would be, if we hadn't moved the windows */
            if(pHP->ulScrollPercentage==100)
                WinSetPointerPos(HWND_DESKTOP, (lMouseXPos+=lSlidingXFactor*0.5),
                    (lMouseYPos+=lSlidingYFactor*0.5));
            else
                WinSetPointerPos(HWND_DESKTOP, (lMouseXPos+=lSlidingXFactor),
                    (lMouseYPos+=lSlidingYFactor));
            }
        }
    else
                                        /* If clicked on absolute border row or column we
                                           don't have to do anything but to reset move busy flag */
        if(!WinPostMsg(pHP->hwndClient, WM_THREADREADY, MPFROMLONG(THREADMOVEBUSY), NULL))
            WinSendMsg(pHP->hwndClient, WM_THREADREADY, MPFROMLONG(THREADMOVEBUSY), NULL);
    CHECKPOINT(WTrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_EXPANDWPS, (BOOL bExpand)                                                 *
\*                                                                                      */
case WM_EXPANDWPS:
/*                                                                                      *\
 * This local procedure is called to resize the WPS either to the whole Virtual         *
 * Desktop or to the size of the physical screen.                                       *
 * Req:                                                                                 *
 *      bExpand ....... TRUE if WPS should expand to whole Virtual Desktop              *
 *                      FALSE otherwise                                                 *
 * Returns:                                                                             *
 *      none                                                                            *
 * Ref.:                                                                                *
\*                                                                                      */
    {
    static SWP  swpWPSInitial;
    SWP         swpWPSNew;

                                        /* Border width of WPS is 1 pixel */
#define WPS_NOSIZING_BORDER             1

#ifdef  DEBUG_WTRD
    printf("WTrd: WM_EXPANDWPS (%s)\n", (SHORT1FROMMP(mp1)==TRUE ? "Expand" : "Shrink"));
#endif  /* DEBUG_WTRD */
                                        /* If no WPS is running it can't be sized */
    if(!pHP->hwndWPSClient) break;
    if((BOOL)SHORT1FROMMP(mp1)==TRUE)
        {
                                        /* Query the current size */
        WinQueryWindowPos(pHP->hwndWPSFrame, &swpWPSInitial);
        swpWPSNew=swpWPSInitial;
                                        /* Resize WPS which is the parent window of the WC_CONTAINER
                                           class client area we catch mouse clicks on to display
                                           the Popup-Menu */
        swpWPSNew.fl=SWP_SIZE|SWP_MOVE|SWP_NOADJUST;
        swpWPSNew.x=pHP->VirtualDesktopMin.x-pHP->VirtualDesktopPos.x-1;
        swpWPSNew.y=pHP->VirtualDesktopMin.y-pHP->VirtualDesktopPos.y-1;
        swpWPSNew.cx=pHP->DesktopSize.x*pHP->ulHorizontalDesktops+2*WPS_NOSIZING_BORDER;
        swpWPSNew.cy=pHP->DesktopSize.y*pHP->ulVerticalDesktops+
            WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR)+2*WPS_NOSIZING_BORDER;
        swpWPSNew.hwndInsertBehind=NULLHANDLE;
        WinSetMultWindowPos(pThreadData->habThread, &swpWPSNew, 1);
        }
    else
        {                               /* Reduce WPS to the size it has been before PC/2 did
                                           its expansion to the whole Virtual Desktops. If the
                                           WarpCenter was active, the WPS had been reduced by
                                           the WarpCenter, but we can't tell that reliably, so
                                           we return the WPS to the size before PC/2 was active */
        swpWPSInitial.fl=SWP_SIZE|SWP_MOVE|SWP_NOADJUST;
        WinSetMultWindowPos(pThreadData->habThread, &swpWPSInitial, 1);
        }
    WinPostMsg(pHP->hwndClient, WM_SETUPSIZEPOSITION,
        MPFROMLONG(WM_SIZE), MPFROMHWND(pHP->hwndWPSFrame));
    }
    break;

/*                                                                                      *\
 * Syntax: WM_BACKGROUNDBITMAP, NULL, NULL                                              *
\*                                                                                      */
case WM_BACKGROUNDBITMAP:
/*                                                                                      *\
 * This local procedure is called to change the PM background bitmap.                   *
 * Req:                                                                                 *
 *      HookParameters. Extract bitmap info                                             *
 * Returns:                                                                             *
 *      none                                                                            *
 * Ref.:                                                                                *
\*                                                                                      */
    {
    DESKTOP Desktop;                    /* Desktop state structure */

    CHECKPOINT(WTrd, 0x0C00);
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_BACKGROUNDBITMAP\n");
#endif  /* DEBUG_WTRD */
    Desktop.cbSize=sizeof(DESKTOP);     /* Length */
    Desktop.hbm=0;                      /* Bitmap handle */
    Desktop.x=Desktop.y=0;              /* Bitmap origin coordinates */
                                        /* Desktop background state settings */
    if(pHP->ulStatusFlag1 & BACKGROUNDBITMAP)
        {
                                        /* Load it from file and destroy existing one */
        Desktop.fl=SDT_LOADFILE;
        if(pHP->ulBackgroundBitmapFlag & BITMAPNORMAL)
            Desktop.fl|=SDT_CENTER;
        if(pHP->ulBackgroundBitmapFlag & BITMAPSCALED)
            Desktop.fl|=SDT_SCALE;
        if(pHP->ulBackgroundBitmapFlag & BITMAPTILED)
            {
            Desktop.fl|=SDT_TILE;
                                        /* Use tile count if set otherwise 0 */
            Desktop.lTileCount=pHP->ulBackgroundBitmapFlag & 0xFFFF;
            }
        else
            Desktop.lTileCount=0;
                                        /* Full qualified path to bitmap file */
        strcpy(Desktop.szFile, pHP->ucBackgroundBitmap);
        }
    else
        {                               /* If no bitmap should be selected unset and destroy it */
        Desktop.fl=SDT_DESTROY | SDT_NOBKGND;
        Desktop.lTileCount=0;           /* No tile count */
        Desktop.szFile[0]='\0';         /* No bitmap file */
        }
                                        /* Set new PM desktop background bitmap */
    WinSetDesktopBkgnd(HWND_DESKTOP, &Desktop);
    CHECKPOINT(WTrd, 0x0000);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SETDRIVEROOT, NULL, NULL                                                  *
\*                                                                                      */
case WM_SETDRIVEROOT:
/*                                                                                      *\
 * This message is posted to change to the root directory of all local drives, so that  *
 * before another application is startet via the Popup-Menu all local drives are        *
\*                                                                                      */
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_SETDRIVEROOT\n");
#endif  /* DEBUG_WTRD */
    SetDriveRoot();                     /* Change to root */
    break;

/*                                                                                      *\
 * Syntax: WM_QUICKSWITCH, ULONG ulFlag, PVOID pvoid                                    *
\*                                                                                      */
case WM_QUICKSWITCH:
/*                                                                                      *\
 * This message is sent by the main thread, when the keys CTRL+TAB or CTRL+SHIFT+TAB    *
 * have been detected. These keys are used to activate QuickSwitch, or to advance the   *
 * selection in the QuickSwitch to the next/previous application.                       *
\*                                                                                      */
    {
                                        /* Current Window List entry's frame window switch handle that is displayed
                                           in the QuickSwitch window */
    static HSWITCH  hswitchCurrentWindow=NULLHANDLE;
                                        /* The flag may either contain QUICKSWITCH_DISMISS (which
                                           means the QuickSwitch window is not visible) or
                                           QUICKSWITCH_NEXT/QUICKSWITCH_PREVIOUSE (which means the
                                           QuickSwitch window is visible) */
    static ULONG    ulQuickSwitchFlag=QUICKSWITCH_DISMISS;
                                        /* Switch control block corresponding to current window's switch handle */
    SWCNTRL         swcntrlCurrentWindow;
                                        /* SWENTRY structure of Desktop child window to switch to */
    SWENTRY         *pswentrySwitch;

                                        /* Disallow user to switch away focus during PC/2 Lockup */
    if(pHP->hwndLockup)
        break;
    CHECKPOINT(WTrd, 0x0D00);
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_QUICKSWITCH ");
#endif  /* DEBUG_WTRD */
                                        /* Disallow also during closing */
    if(pHP->ulRuntimeFlag & (QUITFROMSHUTDOWN|QUITFROMWINDOWLIST))
        {
#ifdef  DEBUG_WTRD
        printf("ignored\n");
#endif  /* DEBUG_WTRD */
        break;
        }
    switch(LONGFROMMP(mp1))
    {
    case QUICKSWITCH_SWITCH:
    case QUICKSWITCH_DISMISS:
/*                                                                                      *\
 * This message is sent by the hook when mp2 is NULL, or by the QuickSwitch window if   *
 * mp2 is not NULL. If the hook sends this message, we forward it to the QuickSwitch    *
 * window to give it a chance to hide itself. The QuickSwitch window passes this        *
 * message back, setting the frame's window handle to switch to into mp2 and we then    *
 * switch to this session.                                                              *
\*                                                                                      */
        {
        CHECKPOINT(WTrd, 0x0D01);
#ifdef  DEBUG_WTRD
        printf("(%s)\n", (LONGFROMMP(mp1)==QUICKSWITCH_SWITCH ? "SWITCH" : "DISMISS"));
#endif  /* DEBUG_WTRD */
                                        /* As the user wants to dismiss the QuickSwitch window,
                                           clear last selected window */
        hswitchCurrentWindow=0;
        if(HWNDFROMMP(mp2)==NULLHANDLE)
            {
                                        /* If mp2 is NULL, then we got this message from the hook. We
                                           then pass this message to the QuickSwitch window to give it
                                           a chance to hide itself */
            WinPostMsg(pHP->hwndSwitchClient, WM_QUICKSWITCH, mp1, NULL);
            }
        else
            {
                                        /* Ignore message when the QuickSwitch window is not visible, because
                                           if the user has activated the QuickSwitch window but dismissed it
                                           by clicking on it with the mouse, depressing the CTRL key again
                                           generates a QUICKSWITCH_SWITCH/QUICKSWITCH_DISMISS message, which
                                           we have to ignore */
            if(ulQuickSwitchFlag==QUICKSWITCH_DISMISS)
                break;
            ulQuickSwitchFlag=QUICKSWITCH_DISMISS;
                                        /* If mp2 contains the frame window's handle to switch to, we
                                           got this message from the QuickSwitch window (after hiding
                                           itself) to switch to the session then */
            if(LONGFROMMP(mp1)==QUICKSWITCH_SWITCH)
                {
                HWND    hwndCurrent=NULLHANDLE;

                                        /* Find the window of the Window List entry and switch to it */
                pswentrySwitch=(SWENTRY *)GetWindowList(pThreadData->habThread, WINDOWLIST_HANDLE, mp2);
                hwndCurrent=pswentrySwitch->swctl.hwnd;
                                        /* Enumerate Desktop frame childs and Window List again, as
                                           GetDesktopWindows() will use WinSendMsg() to synchronously
                                           invoke WM_DESKTOPMOVE which in turn will use the last enumerated
                                           SWP structures, which may not be up to date (e.g. the QuickSwitch
                                           window is not marked invisible there although it has been hidden
                                           by now - which is peculiar as each change in a windows position causes
                                           a WM_SETUPSIZEPOSTION message to be generated, which enumerates all
                                           Desktop frame childs again). But when the working thread is busy,
                                           WM_SETUPSIZEPOSITION is queued and we use outdated SWP structures,
                                           so we have to force a enumerate here, or the QuickSwitch window
                                           may be unhidden again. */
                CHECKPOINT(WTrd, 0x0D02);
                GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_QUERY, NULL);
                CHECKPOINT(WTrd, 0x0D03);
                GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_SWITCHTO, MPFROMHWND(hwndCurrent));
                CHECKPOINT(WTrd, 0x0D04);
                }
            }
        }
        CHECKPOINT(WTrd, 0x0000);
        break;

    case QUICKSWITCH_NEXT:
    case QUICKSWITCH_PREVIOUS:
/*                                                                                      *\
 * This message is sent by the hook when the user wants to activate the QuickSwitch     *
 * window, whereby mp2 is NULL.                                                         *
\*                                                                                      */
        {
        CHECKPOINT(WTrd, 0x0D11);
#ifdef  DEBUG_WTRD
        printf("(%s)\n", (LONGFROMMP(mp1)==QUICKSWITCH_NEXT ? "NEXT" : "PREVIOUS"));
#endif  /* DEBUG_WTRD */
                                        /* Save the fact the user wanted the QuickSwitch window to be
                                           visible */
        ulQuickSwitchFlag=(ULONG)mp1;
        WinQuerySwitchEntry(hswitchCurrentWindow, &swcntrlCurrentWindow);
        if(!hswitchCurrentWindow)
            {
                                        /* If we invoke QuickSwitch for the first time (to make it visible)
                                           get the current active frame window's Window List entry, and
                                           save it */
            hswitchCurrentWindow=(HSWITCH)GetWindowList(pThreadData->habThread, WINDOWLIST_ACTIVEWINDOW, 0);
#ifdef  DEBUG
            if(hswitchCurrentWindow==NULLHANDLE)
                printf("WTrd: WM_QUICKSWITCH WINDOWLIST_ACTIVEWINDOW failed\n");
#endif  /* DEBUG */
            }
        else
            {
                                        /* Otherwise get the next or previous Window List entry (the value
                                           of QUICKSWITCH_NEXT/PREVIOUS must equal WINDOWLIST_NEXT/PREVIOUS) */
            hswitchCurrentWindow=(HSWITCH)GetWindowList(pThreadData->habThread, (ULONG)mp1, MPFROMHWND(swcntrlCurrentWindow.hwnd));
#ifdef  DEBUG
            if(hswitchCurrentWindow==NULLHANDLE)
                printf("WTrd: WM_QUICKSWITCH WINDOWLIST_NEXT/PREVIOUS failed\n");
#endif  /* DEBUG */
            }
                                        /* As a last resort, try to start with our Overview window */
        if(!hswitchCurrentWindow)
            hswitchCurrentWindow=(HSWITCH)GetWindowList(pThreadData->habThread, (ULONG)mp1, MPFROMHWND(pHP->hwndFrame));
#ifdef  DEBUG
        if(hswitchCurrentWindow==NULLHANDLE)
            printf("WTrd: WM_QUICKSWITCH completely failed\n");
#endif  /* DEBUG */
        if(hswitchCurrentWindow)
            WinPostMsg(pHP->hwndSwitchClient, WM_QUICKSWITCH, mp1, (MPARAM)hswitchCurrentWindow);
        CHECKPOINT(WTrd, 0x0000);
        }
        break;

    case QUICKSWITCH_SWITCHTO:
/*                                                                                      *\
 * This message is sent by the SessionBar to switch to the Virtual Desktop the window   *
 * whose handle is passed to by mp2 resides on.                                         *                                                                    *
\*                                                                                      */
        CHECKPOINT(WTrd, 0x0D21);
#ifdef  DEBUG_WTRD
        printf("SwitchTo\n");
#endif  /* DEBUG_WTRD */
        GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_SWITCHTO, mp2);
        CHECKPOINT(WTrd, 0x0000);
        break;

    case QUICKSWITCH_MOVETO:
/*                                                                                      *\
 * This message is sent by PC2HOOK.DLL during processing of WM_ACTIVATE to move to the  *
 * Virtual Desktop the window whose handle is passed to by mp2 resides on.              *                                                                    *
\*                                                                                      */
        CHECKPOINT(WTrd, 0x0D31);
#ifdef  DEBUG_WTRD
        printf("MoveTo\n");
#endif  /* DEBUG_WTRD */
        if(!(pHP->ulStatusFlag1 & SLIDINGFOCUS))
            {
            CHECKPOINT(WTrd, 0x0D32);
            GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_QUERY, 0);
            CHECKPOINT(WTrd, 0x0D33);
            GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_MOVETO, mp2);
            CHECKPOINT(WTrd, 0x0D34);
            }
        CHECKPOINT(WTrd, 0x0000);
        break;

    case QUICKSWITCH_ACTIVATE:
/*                                                                                      *\
 * This message is posted to activate the window currently on Z-Order top of the        *
 * current Virtual Desktop. Thus the Z-Order of all windows is queried and the topmost  *
 * one is activated unless this is a PC/2 window.                                       *
\*                                                                                      */
        {
        HWND    hwndNextWindow=GetZOrderTop();

        CHECKPOINT(WTrd, 0x0D41);
#ifdef  DEBUG_WTRD
        printf("Activate\n");
#endif  /* DEBUG_WTRD */
        if(hwndNextWindow!=NULLHANDLE)
            WinPostMsg(hwnd, WM_QUICKSWITCH,
                MPFROMLONG(QUICKSWITCH_SWITCHTO), MPFROMHWND(hwndNextWindow));
        CHECKPOINT(WTrd, 0x0000);
        }
        break;

    case QUICKSWITCH_SWITCHTOWINDOW:
/*                                                                                      *\
 * This message is sent by the SessionBar to switch to the Virtual Desktop the window   *
 * whose handle is passed to by mp2 resides on. In contrast to QUICKSWITCH_SWICTHTO     *                                                                    *
 * only the specified window is activated, not any in the window's process that is      *                                                                    *
 * the one in the Window List.                                                          *                                                                    *
\*                                                                                      */
        CHECKPOINT(WTrd, 0x0D51);
#ifdef  DEBUG_WTRD
        printf("SwitchToWindow\n");
#endif  /* DEBUG_WTRD */
        GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_SWITCHTOWND, mp2);
        CHECKPOINT(WTrd, 0x0000);
        break;

    }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SESSIONBAR, ULONG ulFlag, PVOID pvoid                                     *
\*                                                                                      */
case WM_SESSIONBAR:
/*                                                                                      *\
 * This message is sent by the main thread, when the user made some actions to the      *
 * session bar, for example selected a session which he wants to activate.              *
\*                                                                                      */
#ifdef  DEBUG_WTRD
    printf("WTrd: WM_SESSIONBAR ");
#endif  /* DEBUG_WTRD */
    if(LONGFROMMP(mp1)==SESSIONBAR_SWITCH)
/*                                                                                      *\
 * This message is sent by the the main thread when the user wants to switch to a       *
 * session whose window handle is passed in mp2. We then request PM to switch to the    *
 * session corresponding to this window handle.                                         *
\*                                                                                      */
        {
        HWND    hwndCurrent=HWNDFROMMP(mp2);

#ifdef  DEBUG_WTRD
        printf("SWITCH\n");
#endif  /* DEBUG_WTRD */
                                        /* Let's do the session switch */
        GetDesktopWindows(pThreadData->habThread, DESKTOPWINDOWS_SWITCHTO, MPFROMHWND(hwndCurrent));
        }
    break;

/*                                                                                      *\
 * Syntax: WM_SPYFRAMEWINDOW, (SPYFRAME *pspyframeCurrent, NULL)                        *
\*                                                                                      */
case WM_SPYFRAMEWINDOW:
/*                                                                                      *\
 * This local procedure is called to resize the WPS either to the whole Virtual         *
 * Desktop or to the size of the physical screen.                                       *
 * Req:                                                                                 *
 *      pspyframeCurrent                                                                *
 *                      Pointer to a structure partly filled in by the Input-Hook, we   *
 *                      fill the remaining fields and pass on the pointer to the Spy    *
 *                      notebook page of the Desktop dialog.                            *
 *                      NULL is valid (e.g. to reset the spying flag after ALT was      *
 *                      was pressed.                                                    *
 * Returns:                                                                             *
 *      none                                                                            *
 * Ref.:                                                                                *
\*                                                                                      */
    {
    SPYFRAME    *pspyframeCurrent=(SPYFRAME *)mp1;
    SWENTRY     *pswentryCurrent;

    if(!pspyframeCurrent)
        {
        WinPostMsg(pHP->hwndSpyFrameWindow, WM_SPYFRAMEWINDOW, NULL, NULL);
        break;
        }
    pswentryCurrent=(SWENTRY *)GetWindowList(pThreadData->habThread, WINDOWLIST_HANDLE, MPFROMHWND(pspyframeCurrent->hwndFrame));
    if(pswentryCurrent)
        {
        strcpy(pspyframeCurrent->ucWindowList, pswentryCurrent->swctl.szSwtitle);
        WinPostMsg(pHP->hwndSpyFrameWindow, WM_SPYFRAMEWINDOW, MPFROMP(pspyframeCurrent), NULL);
        }
    }
    break;

default:                                /* Default window procedure must be called */
    return((MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

