/***********************************************************************\
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1994, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * PC2Spool.c   PC/2's spooler control, available when running PC/2    *
 *              as a WPS replacement.                                  *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#define         INCL_SPL
#define         INCL_SPLDOSPRINT

#include        "PC2.h"                 /* User include files */
#include        "Error.h"

#define         _FILE_  "PC2Spool.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;
const ULONG     _PC2SPOOLSIGNATURE=BLDLEVEL_SIGNATURE;

                                        /* Function prototype of the undocumented Spl32QMInitialize()
                                           OS/2 spooler start API. Thanks to Monte Copeland for
                                           demonstrating its use in his MSHELL program */
typedef BOOL    (APIENTRY SPL32QMINITIALIZE) (ULONG *pulError);
SPL32QMINITIALIZE   *pSpl32QmInitialize;

FIELDINFO       ContainerFieldInfo[]=
                    {
                    { 0, CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR, 0, "Queue"        , FIELDOFFSET(SPOOLERRECORD, pszQueue)       , NULL, NULL, 0 },
                    { 0, CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR, 0, "Queue\nStatus", FIELDOFFSET(SPOOLERRECORD, pszQueueStatus) , NULL, NULL, 0 },
                    { 0, CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR, 0, "Queue\nName"  , FIELDOFFSET(SPOOLERRECORD, pszQueueComment), NULL, NULL, 0 },
                    { 0, CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR, 0, "Queue\nDriver", FIELDOFFSET(SPOOLERRECORD, pszQueueDriver) , NULL, NULL, 0 },
                    { 0, CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR, 0, "Job"          , FIELDOFFSET(SPOOLERRECORD, pszJob)         , NULL, NULL, 0 },
                    { 0, CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR, 0, "Job\nNumber"  , FIELDOFFSET(SPOOLERRECORD, pszJobNumber)   , NULL, NULL, 0 },
                    { 0, CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR, 0, "Job\nStatus"  , FIELDOFFSET(SPOOLERRECORD, pszJobStatus)   , NULL, NULL, 0 },
                    { 0, CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR, 0, "Job\nComment" , FIELDOFFSET(SPOOLERRECORD, pszJobComment)  , NULL, NULL, 0 },
                    };

HOOKPARAMETERS  *pHP;
HWND            hwndSpoolerPopupMenu;
PFNWP           pfnContainerWindowProc; /* Container window procedure for subclassing */

ULONG   SpoolerInitialize(HMODULE hDLLPc2Hook)
{
ULONG   flCreate=FCF_SPOOLERWINDOW;     /* Frame creation control flag */
SWP     swp;                            /* According to a tip from members of OS/2 development in
                                           an article in OS/2 Developer magazine, it is more
                                           efficient to call WinSetMultWindowPos(), because
                                           WinSetWindowPos() internally calls WinSetMultWindowPos()
                                           (even if just one window is moved) */
HMODULE hDLLPmSpl;                      /* DLL handle of PMSPL.DLL */
UCHAR   ucBuffer[80];                   /* DosLoadModule() error buffer */
                                        /* Parameter for Spl32QmInitialize. Under Merlin Beta the return code
                                           is not set when running under the WPS, but the PC/2 seems to be able
                                           to control the spooler anyway, so preset it */
ULONG   ulSpoolerRc=NO_ERROR;

                                        /* Get access to the PC/2 control structure */
if(DosQueryProcAddr(hDLLPc2Hook, DLLENTRYPOINTHOOKPARAMETERS, NULL, (PFN *)(&pHP))!=NO_ERROR)
    return(ERROR_FILE_NOT_FOUND);
/*                                                                                      *\
 * Initialize the spooler interface of PMSPL.DLL, if PC/2 is running as a WPS           *
 * replacement, by loading SPL32QMINITIALIZE from PMSPL.DLL.                            *
\*                                                                                      */
if(DosLoadModule(ucBuffer, sizeof(ucBuffer)-1, "PMSPL", &hDLLPmSpl)!=NO_ERROR)
    {                                   /* DLL couldn't be found in the current PC/2
                                           directory or via the LIBPATH path */
    return(ERROR_FILE_NOT_FOUND);
    }
if(DosQueryProcAddr(hDLLPmSpl, 0, "SPL32QMINITIALIZE", (PFN *)(&pSpl32QmInitialize))!=NO_ERROR)
    {                                   /* An error occured */
    DosFreeModule(hDLLPmSpl);
    return(ERROR_FILE_NOT_FOUND);
    }
                                        /* Initialize the OS/2 2.x spooler */
pSpl32QmInitialize(&ulSpoolerRc);
                                        /* Return any error and disable Spooler Container, except for
                                           error 0x4FFD which is returned when the OS/2 Spooler is
                                           already running. Note, this error is not documented, so this
                                           assumption comes from trial and error... */
if(ulSpoolerRc==0x4FFD) ulSpoolerRc=NO_ERROR;
if(ulSpoolerRc!=NO_ERROR)
    return(ulSpoolerRc);
/*                                                                                      *\
 * If spooler could be initialized create PC/2 Spooler Control Window.                  *
\*                                                                                      */
if(!WinRegisterClass(                   /* Register window class */
    pHP->habPc2,                        /* Handle of anchor block */
    (PSZ)PC2_CLASSNAME_SPOOLER,         /* Window class name */
    (PFNWP)PC2_SpoolerWindowProc,       /* Address of window procedure */
                                        /* Class style */
    CS_SIZEREDRAW | CS_SAVEBITS | CS_MOVENOTIFY,
    0))                                 /* Extra window words */
    return(1);
                                        /* Create a standard window */
if((pHP->hwndSpoolerFrame=WinCreateStdWindow(
    HWND_DESKTOP,                       /* DESKTOP is parent */
    0,                                  /* Standard window styles */
    &flCreate,                          /* Frame control flags */
    (PSZ)PC2_CLASSNAME_SPOOLER,         /* Client window class name */
    "",                                 /* No window text */
    0,                                  /* No special class style */
    pHP->hDLLPc2Spooler,                /* Ressource is in PC2SPOOL.DLL file */
    ID_PC2SPOOLERWINDOW,                /* Frame window identifier */
    &pHP->hwndSpoolerClient)            /* Client window handle */
    )==NULLHANDLE)
    return(2);
WinSetWindowText(pHP->hwndSpoolerFrame, PC2_SPOOLER_WINDOW);
swp.fl=SWP_HIDE|SWP_MOVE|SWP_SIZE|SWP_DEACTIVATE;
swp.x=pHP->swpPC2Spooler.x;
swp.y=pHP->swpPC2Spooler.y;
swp.cx=pHP->swpPC2Spooler.cx;
swp.cy=pHP->swpPC2Spooler.cy;
swp.hwndInsertBehind=NULLHANDLE;
swp.hwnd=pHP->hwndSpoolerFrame;
WinSetMultWindowPos(pHP->habPc2, &swp, 1);
hwndSpoolerPopupMenu=WinLoadMenu(pHP->hwndSpoolerClient, pHP->hDLLPc2Spooler, ID_SPOOLERPOPUPMENU);
return(NO_ERROR);
}

MRESULT EXPENTRY PC2_SpoolerWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static CNRINFO      CnrInfo;            /* Container info structure */
static PFIELDINFO   pFieldInfo;
static PFIELDINFO   pFieldInfoFirst;

switch(msg)
{
case WM_CREATE:
/*                                                                                      *\
 * During spooler window creation, create a container class window as the child of the  *
 * client area. Initialize the columns for the details view, the spooler container      *
 * window will use. Then set the container info structure to details view with a        *
 * splitbar.
\*                                                                                      */
    {
    ULONG           ulColumn;
    PFIELDINFO      pFieldInfoSplitbar; /* Pointer to a the FIELDINFO structure of the 4th column
                                           to set splitbar afterwards */
    FIELDINFOINSERT FieldInfoInsert;    /* Control structure for column insertation */

                                        /* Create the container to fill in spooler queue
                                           and job data in details view. */
    pHP->hwndSpoolerContainer=WinCreateWindow(hwnd, WC_CONTAINER, "",
        WS_VISIBLE | CCS_READONLY | CCS_MINIRECORDCORE | CCS_MULTIPLESEL,
        0, 0, 0, 0, hwnd, HWND_TOP, ID_PC2SPOOLERCONTAINER, NULL, NULL);
                                        /* Set presentation parameters saved */
    WinSetPresParam(pHP->hwndSpoolerContainer, PP_FONTNAMESIZE,
        sizeof(pHP->ucPC2SpoolerFont), pHP->ucPC2SpoolerFont);
                                        /* Subclass container window to get hold of presentation
                                           parameter changes */
    pfnContainerWindowProc=WinSubclassWindow(pHP->hwndSpoolerContainer, SubclassedContainerWindowProc);
                                        /* Create fieldinfos for the columns in details view */
    pFieldInfo=(PFIELDINFO)WinSendMsg(pHP->hwndSpoolerContainer, CM_ALLOCDETAILFIELDINFO,
        MPFROMSHORT(sizeof(ContainerFieldInfo)/sizeof(ContainerFieldInfo[0])), NULL);
                                        /* Fill the columns avaiable into container */
    pFieldInfoFirst=pFieldInfo;
    pFieldInfoSplitbar=NULL;
    for(ulColumn=0; ulColumn<(sizeof(ContainerFieldInfo)/sizeof(ContainerFieldInfo[0])); ulColumn++)
        {
                                        /* Save 4th column to set splitbar afterwards */
        if(ulColumn==3) 
            pFieldInfoSplitbar=pFieldInfo;
        pFieldInfo->flTitle=ContainerFieldInfo[ulColumn].flTitle;
        pFieldInfo->pTitleData=strdup(ContainerFieldInfo[ulColumn].pTitleData);
        pFieldInfo->flData=ContainerFieldInfo[ulColumn].flData;
        pFieldInfo->offStruct=ContainerFieldInfo[ulColumn].offStruct;
        pFieldInfo=pFieldInfo->pNextFieldInfo;
        }
                                        /* Fill column insertation control */
    FieldInfoInsert.cb=sizeof(FieldInfoInsert);
                                        /* Insert from first column */
    FieldInfoInsert.pFieldInfoOrder=(PFIELDINFO)CMA_FIRST;
                                        /* Update container display now */
    FieldInfoInsert.fInvalidateFieldInfo=(ULONG)TRUE;
                                        /* Number of columns to insert */
    FieldInfoInsert.cFieldInfoInsert=sizeof(ContainerFieldInfo)/sizeof(ContainerFieldInfo[0]);
                                        /* Insert column field info into container */
    WinSendMsg(pHP->hwndSpoolerContainer, CM_INSERTDETAILFIELDINFO,
        MPFROMP(pFieldInfoFirst), MPFROMP(&FieldInfoInsert));
                                        /* Select details view */
    CnrInfo.flWindowAttr=CV_DETAIL | CA_DETAILSVIEWTITLES;
                                        /* Splitbar is set to last saved position */
    CnrInfo.xVertSplitbar=pHP->lSplitbarPosition;
                                        /* Splitbar occurs after the 4th column */
    CnrInfo.pFieldInfoLast=pFieldInfoSplitbar;
                                        /* Setup CNRINFO into container */
    WinSendMsg(pHP->hwndSpoolerContainer, CM_SETCNRINFO,
        MPFROMP(&CnrInfo), MPFROMLONG(CMA_FLWINDOWATTR|CMA_XVERTSPLITBAR|CMA_PFIELDINFOLAST));
    RefreshSpooler();                   /* Fill spooler container from queue data */
    WinSetFocus(HWND_DESKTOP, pHP->hwndSpoolerContainer);
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
 * During sizing (and moving) of the spooler window, ensure that the container always   *
 * fills the complete client area.                                                      *
\*                                                                                      */
    {
    LONG    lXBorder=WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
    LONG    lYBorder=WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER);
    SWP     swpPC2Spooler;
    RECTL   rectlClient;
    SWP     swp;

                                        /* Get the frame area size */
    WinQueryWindowPos(pHP->hwndSpoolerFrame, &swpPC2Spooler);
    if((swpPC2Spooler.cx>pHP->DesktopSize.x) || (pHP->swpPC2Spooler.cx<0))
        pHP->swpPC2Spooler.cx=pHP->DesktopSize.x/2;
    else
        pHP->swpPC2Spooler.cx=swpPC2Spooler.cx;
    if((swpPC2Spooler.cy>pHP->DesktopSize.y) || (pHP->swpPC2Spooler.cy<0))
        pHP->swpPC2Spooler.cy=pHP->DesktopSize.y/5;
    else
        pHP->swpPC2Spooler.cy=swpPC2Spooler.cy;
    if((swpPC2Spooler.x<(0-lXBorder)) ||
        ((swpPC2Spooler.x+pHP->swpPC2Spooler.cx)>(pHP->DesktopSize.x+lXBorder)))
        pHP->swpPC2Spooler.x=0;
    else
        pHP->swpPC2Spooler.x=swpPC2Spooler.x;
    if((swpPC2Spooler.y<(0-lYBorder)) ||
        ((swpPC2Spooler.y+pHP->swpPC2Spooler.cy)>(pHP->DesktopSize.y+lYBorder)))
        pHP->swpPC2Spooler.y=0;
    else
        pHP->swpPC2Spooler.y=swpPC2Spooler.y;
    WinSetWindowPos(pHP->hwndSpoolerFrame, NULLHANDLE, pHP->swpPC2Spooler.x, pHP->swpPC2Spooler.y,
        pHP->swpPC2Spooler.cx, pHP->swpPC2Spooler.cy, SWP_SIZE | SWP_MOVE);
                                        /* Size container to client area */
    WinQueryWindowRect(hwnd, &rectlClient);
    swp.fl=SWP_MOVE|SWP_SIZE|SWP_SHOW|SWP_ZORDER;
    swp.x=swp.y=0;
    swp.cx=rectlClient.xRight;
    swp.cy=rectlClient.yTop;
    swp.hwndInsertBehind=HWND_TOP;
    swp.hwnd=pHP->hwndSpoolerContainer;
    WinSetMultWindowPos(pHP->habPc2, &swp, 1);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SHOWSPOOLER, NULL, NULL                                                   *
\*                                                                                      */
case WM_SHOWSPOOLER:
/*                                                                                      *\
 * The spooler window is initially created invisible. When the user selects the         *
 * menuentry PC2 Spooler, this message is posted to set the spooler window visible.     *
\*                                                                                      */
    {
    SWP     swp;

    swp.fl=SWP_SHOW|SWP_ACTIVATE|SWP_ZORDER;
    swp.x=pHP->swpPC2Spooler.x;
    swp.y=pHP->swpPC2Spooler.y;
    swp.cx=pHP->swpPC2Spooler.cx;
    swp.cy=pHP->swpPC2Spooler.cy;
    swp.hwndInsertBehind=HWND_TOP;
    swp.hwnd=pHP->hwndSpoolerFrame;
    WinSetMultWindowPos(pHP->habPc2, &swp, 1);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_REFRESHSPOOLER, NULL, NULL                                                *
\*                                                                                      */
case WM_REFRESHSPOOLER:
/*                                                                                      *\
 * The spooler container regularily gets this message from a timer to update the        *
 * container with the current printer queue data, to have a real-time status of the     *
 * printer queue.                                                                       *
\*                                                                                      */
    RefreshSpooler();
    break;

case WM_CONTROL:
/*                                                                                      *\
 * The child window of the client area sends its notification messages to its parent,   *
 * so the client window procedure can expext container notification messages.           *
\*                                                                                      */
    switch(SHORT2FROMMP(mp1))
    {
    case CN_CONTEXTMENU:
        {
        SPOOLERRECORD   *pListSpoolerRecord;
        POINTL          ptlMouse;       /* Mouse position during creation of notification */
        USHORT          fsOptions=PU_NONE | PU_KEYBOARD | PU_MOUSEBUTTON1 |
                                  PU_HCONSTRAIN | PU_VCONSTRAIN;

                                        /* Query selected records in container and return them,
                                           or return the one below the mouse pointer if none is
                                           selected. */
        pListSpoolerRecord=QuerySelectedRecords((SPOOLERRECORD *)mp2);
        if(pListSpoolerRecord==NULL)
            pHP->pSpoolerObject=NULL;
        else
            pHP->pSpoolerObject=pListSpoolerRecord->pSpoolerObject;
                                        /* Get the mouse position */
        WinQueryMsgPos(pHP->habPc2, &ptlMouse);
        WinPopupMenu(HWND_DESKTOP, hwnd, hwndSpoolerPopupMenu,
            ptlMouse.x, ptlMouse.y, 0, fsOptions);
        }
        break;
    }
    break;

case WM_HELP:                           /* Help pressed */
    WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
        MPFROMSHORT(ID_PC2SPOOLERWINDOW), HM_RESOURCEID);
    break;

case WM_COMMAND:
/*                                                                                      *\
 * Process the messages generated by the menuentries of the spooler container popup     *
 * menu.                                                                                *
\*                                                                                      */
    switch(SHORT1FROMMP(mp1))
    {
    case ID_SPOOLERREFRESH:             /* User requested to refresh the spooler container window
                                           with the status of the actual spooler queue */
        RefreshSpooler();
        break;

    case ID_SPOOLERHOLDJOB:             /* The user selected to hold a job */
    case ID_SPOOLERRELEASEJOB:          /* The user selected to release a job */
    case ID_SPOOLERDELETEJOB:           /* The user selected to delete a job */
    case ID_SPOOLERHOLDQUEUE:           /* The user selected to hold a queue */
    case ID_SPOOLERRELEASEQUEUE:        /* The user selected to release a queue */
                                        /* Perform the action on linked list of selected records */
        NotifySelectedRecords((ULONG)SHORT1FROMMP(mp1));
        break;

    case ID_SPOOLERHELP:                /* The user requested some help */
        WinPostMsg(hwnd, WM_HELP, MPFROMLONG(ID_PC2SPOOLERWINDOW), MPFROM2SHORT(CMDSRC_MENU, TRUE));
        break;
    }
    break;

case WM_DESTROY:
/*                                                                                      *\
 * Clear container structures and free control structures used by PC/2.                 *
\*                                                                                      */
    {
    SPOOLERRECORD   *pSpoolerRecord;

                                        /* Query the actual container info structure */
    WinSendMsg(pHP->hwndSpoolerContainer, CM_QUERYCNRINFO,
        MPFROMP(&CnrInfo), MPFROMSHORT(sizeof(CnrInfo)));
    pHP->lSplitbarPosition=CnrInfo.xVertSplitbar;
                                        /* Prevent unnecessary container drawing */
    WinEnableWindowUpdate(pHP->hwndSpoolerContainer, FALSE);
                                        /* Get first container record to delete them sequentially */
    pSpoolerRecord=(SPOOLERRECORD *)WinSendMsg(pHP->hwndSpoolerContainer, CM_QUERYRECORD,
        NULL, MPFROM2SHORT(CMA_FIRST, CMA_ITEMORDER));
    while(pSpoolerRecord)
        {                               /* Delete and free records */
        free(pSpoolerRecord->pszQueue);
        free(pSpoolerRecord->pszQueueStatus);
        free(pSpoolerRecord->pszQueueComment);
        free(pSpoolerRecord->pszQueueDriver);
        free(pSpoolerRecord->pszJob);
        free(pSpoolerRecord->pszJobNumber);
        free(pSpoolerRecord->pszJobStatus);
        free(pSpoolerRecord->pszJobComment);
        free(pSpoolerRecord->pSpoolerObject);
                                        /* Delete record in container. Warning! as this function
                                           expects an array, but we only delete one record, we
                                           have to get the address of the pointer to the record. */
        WinSendMsg(pHP->hwndSpoolerContainer, CM_REMOVERECORD,
            MPFROMP(&pSpoolerRecord), MPFROM2SHORT(1, CMA_FREE|CMA_INVALIDATE));
                                        /* Get next container record to delete them sequentially */
        pSpoolerRecord=(SPOOLERRECORD *)WinSendMsg(pHP->hwndSpoolerContainer, CM_QUERYRECORD,
            NULL, MPFROM2SHORT(CMA_FIRST, CMA_ITEMORDER));
        }
    }
    break;

default:                                /* Default window procedure must be called */
    return((MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This function applies the current selected menu identifiert from the container's     *
 * context popup menu to the linked list of container records selected. E.g. if the     *
 * user wants to hold some jobs, he marks the jobs and then selects the menuentry to    *
 * hold a job on any selected job. All selected jobs (which are in the linked list      *
 * pointed to by pHP->pSpoolerObject) get holded.                                       *
 * Req:                                                                                 *
 *      ulActionCode .. The menu identifier of the container window context menu.       *
 * Returns:                                                                             *
\*--------------------------------------------------------------------------------------*/
void            NotifySelectedRecords(ULONG ulActionCode)
{
SPOOLERRECORD   *pCurrentSpoolerRecord;

                                        /* If linked list is empty just return */
if(pHP->pSpoolerObject==NULL)
    return;
else
    pCurrentSpoolerRecord=pHP->pSpoolerObject->pSpoolerRecord;
while(pCurrentSpoolerRecord!=NULL)
    {
    switch(ulActionCode)
    {
    case ID_SPOOLERHOLDJOB:             /* The user selected to hold a job */
        SplHoldJob(NULL, pCurrentSpoolerRecord->pszQueue, pCurrentSpoolerRecord->ulJob);
        break;

    case ID_SPOOLERRELEASEJOB:          /* The user selected to release a job */
        SplReleaseJob(NULL, pCurrentSpoolerRecord->pszQueue, pCurrentSpoolerRecord->ulJob);
        break;

    case ID_SPOOLERDELETEJOB:           /* The user selected to delete a job */
        SplDeleteJob(NULL, pCurrentSpoolerRecord->pszQueue, pCurrentSpoolerRecord->ulJob);
        break;

    case ID_SPOOLERHOLDQUEUE:           /* The user selected to hold a queue */
        SplHoldQueue(NULL, pCurrentSpoolerRecord->pszQueue);
        break;

    case ID_SPOOLERRELEASEQUEUE:        /* The user selected to release a queue */
        SplReleaseQueue(NULL, pCurrentSpoolerRecord->pszQueue);
        break;
    }
                                        /* Advance linked list */
    pCurrentSpoolerRecord=(SPOOLERRECORD *)pCurrentSpoolerRecord->MiniRec.preccNextRecord;
    }
RefreshSpooler();                       /* Update container records from spooler queue data */
}

/*--------------------------------------------------------------------------------------*\
 * This function queries the spooler container records that are selected. As an         *
 * argument it gets the SPOOLERRECORD of the container record the user wants to display *
 * the context menu. If this container records is not selected, this record is returned *
 * as a linked list of just one record.                                                 *
 * All selected container records are chained together in a linked list by updating     *
 * the MINIRECORD.preccNextRecord structure element.                                    *
 * Req:                                                                                 *
 *      pSelectedSpoolerRecord                                                          *
 *               ...... The SPOOLERRECORD the mouse pointer was used to activate the    *
 *                      container's context menu.                                       *
 * Returns:                                                                             *
 *      pSpoolerRecord  The first SPOOLERRECORD of a linked list of SPOOLERRECORDS that *
 *                      are all selected, or pSelectedSpoolerRecord if none is          *
 *                      selected or NULL if the container is empty.                     *
\*--------------------------------------------------------------------------------------*/
SPOOLERRECORD   *QuerySelectedRecords(SPOOLERRECORD *pSelectedSpoolerRecord)
{
SPOOLERRECORD   *pSpoolerRecord;
SPOOLERRECORD   *pCurrentSpoolerRecord;
SPOOLERRECORD   *pLastSpoolerRecord;

                                        /* If container contains no record return NULL */
if(pSelectedSpoolerRecord==NULL) return((SPOOLERRECORD *)NULL);
                                        /* Test if record the menu was selected above is selected */
if(pSelectedSpoolerRecord->MiniRec.flRecordAttr & CRA_SELECTED)
    {
                                        /* Query the first selected container record */
    pCurrentSpoolerRecord=(SPOOLERRECORD *)WinSendMsg(pHP->hwndSpoolerContainer, CM_QUERYRECORDEMPHASIS,
        (PRECORDCORE)CMA_FIRST, MPFROMSHORT(CRA_SELECTED));
                                        /* Because this if clause is only executed if one record
                                           is selected, we can safely save and return the first one,
                                           even if its just the only one */
    pSpoolerRecord=pCurrentSpoolerRecord;
    pLastSpoolerRecord=NULL;            /* We haven't found any record before, the current one can
                                           be appended */
    while(pCurrentSpoolerRecord)
        {
        if(pLastSpoolerRecord)
            {                           /* Append current record to linked list and advance linked list */
            pLastSpoolerRecord->MiniRec.preccNextRecord=(MINIRECORDCORE *)pCurrentSpoolerRecord;
            }
        pLastSpoolerRecord=pCurrentSpoolerRecord;
                                        /* Search next selected record starting with the current one */
        pCurrentSpoolerRecord=(SPOOLERRECORD *)WinSendMsg(pHP->hwndSpoolerContainer, CM_QUERYRECORDEMPHASIS,
            (PRECORDCORE)pCurrentSpoolerRecord, MPFROMSHORT(CRA_SELECTED));
        }
                                        /* After the last selected record found end linked list */
    pLastSpoolerRecord->MiniRec.preccNextRecord=NULL;
    }
else
    {                                   /* Because record under activation of the context menu
                                           by the mouse pointer is not selected, return this
                                           record, being a linked list of just one element */
    pSelectedSpoolerRecord->MiniRec.preccNextRecord=NULL;
    pSpoolerRecord=pSelectedSpoolerRecord;
    }
return(pSpoolerRecord);                 /* Return 1st element of linked list */
}

/*--------------------------------------------------------------------------------------*\
 * This function updates the spooler container, and the structures related to the       *
 * queues and jobs on this local machine, according to the current real time spooler    *
 * contents.                                                                            *
 * Req:                                                                                 *
 * Returns:                                                                             *
 *      NO_ERROR ...... If called sucessfully                                           *
 *      ulErrorCode ... If called unsucessfully                                         *
\*--------------------------------------------------------------------------------------*/
ULONG           RefreshSpooler(void)
{
ULONG           ulQueueCountReturned;   /* Number of queues returned in SplEnumQueue() */
ULONG           ulQueueCountNeeded;     /* Number of queues available */
ULONG           ulQueueSize;            /* Size of information returned */
PRQINFO3        *pQueueInfo;            /* Points to all information returned */
PRQINFO3        *pCurrentQueue;         /* Points to current queue within all queues available */
PRQINFO3        *pLastQueue;            /* Points to the queue current job is inserted */
ULONG           ulCurrentQueue;
PRJINFO2        *pCurrentJob;           /* Points to current job within one queue */
ULONG           ulCurrentJob;
ULONG           ulJobCount;             /* Number of jobs within current queue */
RECORDINSERT    RecordInsert;           /* RECORDINSERT structure to control record insertion */
SPOOLERRECORD   *pSpoolerRecord;        /* Pointer to new SPOOLERRECORD created, when a new entry
                                           was found in the queue */
SPOOLEROBJECT   *pCurrentSpoolerObject; /* Pointer to current SPOOLEROBJECT added to list, when a new
                                           entry was found in the queue */
SPOOLEROBJECT   *pAppendSpoolerObject;  /* Pointer to the last element in the SPOOLEROBJECT list,
                                           where the current queue entry is appended */

if(pHP->ulDebug>=DEBUG_ENTRY)
    printf("Spooler: Refresh()\n");
/*                                                                                      *\
 * Fill RECORDINSERT structure, which will be used for inserting records.               *
\*                                                                                      */
                                        /* Initialize RECORDINSERT structure */
RecordInsert.cb=sizeof(RecordInsert);
                                        /* Insert record at end of existing records */
RecordInsert.pRecordOrder=(PRECORDCORE)CMA_END;
RecordInsert.pRecordParent=NULL;        /* There is no parent */
RecordInsert.fInvalidateRecord=TRUE;    /* Update during insertation */
RecordInsert.zOrder=CMA_TOP;            /* Insert at top of Z-Order */
RecordInsert.cRecordsInsert=1;          /* Insert 1 record */
/*                                                                                      *\
 * Get first record from container, free SPOOLERRECORD structure and the corresponding  *
 * SPOOLEROBJECT structure, and then remove record from container until all records are *
 * removed from the spooler container.                                                  *
\*                                                                                      */
                                        /* Prevent unnecessary container drawing */
WinEnableWindowUpdate(pHP->hwndSpoolerContainer, FALSE);
                                        /* Get first container record to delete them sequentially */
pSpoolerRecord=(SPOOLERRECORD *)WinSendMsg(pHP->hwndSpoolerContainer, CM_QUERYRECORD,
    NULL, MPFROM2SHORT(CMA_FIRST, CMA_ITEMORDER));
while(pSpoolerRecord)
    {                                   /* Delete and free records */
    free(pSpoolerRecord->pszQueue);
    free(pSpoolerRecord->pszQueueStatus);
    free(pSpoolerRecord->pszQueueComment);
    free(pSpoolerRecord->pszQueueDriver);
    free(pSpoolerRecord->pszJob);
    free(pSpoolerRecord->pszJobNumber);
    free(pSpoolerRecord->pszJobStatus);
    free(pSpoolerRecord->pszJobComment);
    free(pSpoolerRecord->pSpoolerObject);
                                        /* Delete record in container. Warning! as this function
                                           expects an array, but we only delete one record, we
                                           have to get the address of the pointer to the record. */
    WinSendMsg(pHP->hwndSpoolerContainer, CM_REMOVERECORD,
        MPFROMP(&pSpoolerRecord), MPFROM2SHORT(1, CMA_FREE|CMA_INVALIDATE));
                                        /* Get next container record to delete them sequentially */
    pSpoolerRecord=(SPOOLERRECORD *)WinSendMsg(pHP->hwndSpoolerContainer, CM_QUERYRECORD,
        NULL, MPFROM2SHORT(CMA_FIRST, CMA_ITEMORDER));
    }
                                        /* We have deleted all container object */
pHP->pSpoolerObjectRoot=pAppendSpoolerObject=NULL;
/*                                                                                      *\
 * Now enumerate the printer queue of the local machine, and create container records   *
 * for each queue and its jobs, even if the queue currently doesn't have any print job  *
 * (the queue is inserted in this case to allow queue manipulation).                    *
\*                                                                                      */
SplEnumQueue(                           /* Get printer queue count of local machine */
    NULL,                               /* Local machine */
    4,                                  /* Detail level */
    NULL,                               /* Output buffer */
    0,                                  /* Output buffer size */
    &ulQueueCountReturned,              /* Number of entries returned */
    &ulQueueCountNeeded,                /* Number of entries available */
    &ulQueueSize,                       /* Size of available information */
    NULL);                              /* Reserved */
                                        /* If no queue available return error */
if(ulQueueCountNeeded==0) return(ERROR_INVALID_DATA);
                                        /* Get all local printer queues */
pQueueInfo=malloc(ulQueueSize);
SplEnumQueue(NULL, 4, pQueueInfo, ulQueueSize,
    &ulQueueCountReturned, &ulQueueCountNeeded, &ulQueueSize, NULL);
pCurrentQueue=pQueueInfo;               /* Begin with first queue. For each queue a PRQINFO3
                                           structure is available immediately followed by a
                                           PRJINFO2 structure for each job in the queue. */
for(ulCurrentQueue=0; ulCurrentQueue<ulQueueCountReturned; ulCurrentQueue++)
    {
    UCHAR   ucCurrentJob[256];
    UCHAR   ucCurrentQueueStatus[256];
    UCHAR   ucBuffer[256];

                                        /* Print queue status bitmask */
    switch(pCurrentQueue->fsStatus & PRQ_STATUS_MASK)
    {
    case PRQ_ACTIVE:
        strcpy(ucCurrentQueueStatus, "Active");
        break;

    case PRQ_PAUSED:
        strcpy(ucCurrentQueueStatus, "Paused");
        break;

    case PRQ_ERROR:
        strcpy(ucCurrentQueueStatus, "Error");
        break;

    case PRQ_PENDING:
        strcpy(ucCurrentQueueStatus, "Pending");
        break;
    }
    ulJobCount=pCurrentQueue->cJobs;    /* Get number of jobs in current queue */
    pLastQueue=pCurrentQueue;           /* Save the queue for the case it contains at least 1 job */
    pCurrentQueue++;                    /* Point to next PRQINFO3 structure if current queue
                                           has no jobs, or to a PRJINFO2 structure for a job */
                                        /* Get all jobs within current queue */
    for(ulCurrentJob=0, pCurrentJob=(PRJINFO2 *)pCurrentQueue; ulCurrentJob<ulJobCount;
        ulCurrentJob++, pCurrentJob++)
        {
        ucCurrentJob[0]='\0';           /* Clear to get sure, but it would be a surprise if this
                                           empty string doesn't get overwritten */
        switch(pCurrentJob->fsStatus & PRJ_QSTATUS)
        {
        case PRJ_QS_QUEUED:
            strcpy(ucCurrentJob, "Queued");
            break;

        case PRJ_QS_PAUSED:
            strcpy(ucCurrentJob, "Paused");
            break;

        case PRJ_QS_SPOOLING:
            strcpy(ucCurrentJob, "Spooling");
            break;

        case PRJ_QS_PRINTING:
            strcpy(ucCurrentJob, "Printing");
            break;

        default:
            strcpy(ucCurrentJob, "Unknown");
            break;
        }
        if(!(pCurrentJob->fsStatus & PRJ_DEVSTATUS))
            {
            strcat(ucCurrentJob, ", Normal");
            }
        else
            {
            if(pCurrentJob->fsStatus & PRJ_COMPLETE)
                {
                strcat(ucCurrentJob, ", Complete");
                }
            if(pCurrentJob->fsStatus & PRJ_INTERV)
                {
                strcat(ucCurrentJob, ", Intervention required");
                }
            if(pCurrentJob->fsStatus & PRJ_DESTOFFLINE)
                {
                strcat(ucCurrentJob, ", Destination offline");
                }
            if(pCurrentJob->fsStatus & PRJ_DESTPAUSED)
                {
                strcat(ucCurrentJob, ", Destination paused");
                }
            if(pCurrentJob->fsStatus & PRJ_NOTIFY)
                {
                strcat(ucCurrentJob, ", Alert raised");
                }
            if(pCurrentJob->fsStatus & PRJ_DESTNOPAPER)
                {
                strcat(ucCurrentJob, ", Out of paper");
                }
            if(pCurrentJob->fsStatus & PRJ_DESTFORMCHG)
                {
                strcat(ucCurrentJob, ", Form change");
                }
            if(pCurrentJob->fsStatus & PRJ_DESTCRTCHG)
                {
                strcat(ucCurrentJob, ", Cartridge change");
                }
            if(pCurrentJob->fsStatus & PRJ_DESTPENCHG)
                {
                strcat(ucCurrentJob, ", Pen change");
                }
            }
/*                                                                                      *\
 * Because we have found at least one job for the current queue, add one record to the  *
 * container for each job.                                                              *
\*                                                                                      */
                                        /* Because we found a job, get queue data of current queue */
                                        /* Allocate, setup and chain in a SPOOLEROBJECT structure */
        pCurrentSpoolerObject=malloc(sizeof(SPOOLEROBJECT));
        memset(pCurrentSpoolerObject, '\0', sizeof(SPOOLEROBJECT));
        pCurrentSpoolerObject->pSpoolerObjectNext=NULL;
        pCurrentSpoolerObject->pSpoolerObjectBack=pAppendSpoolerObject;
        pCurrentSpoolerObject->bInQueue=TRUE;
        if(pAppendSpoolerObject!=NULL)
                                        /* Advance pointer to current record */
            pAppendSpoolerObject->pSpoolerObjectNext=pCurrentSpoolerObject;
        else
            pAppendSpoolerObject=pCurrentSpoolerObject;
                                        /* Save pointer to first record */
        if(pHP->pSpoolerObjectRoot!=NULL) pHP->pSpoolerObjectRoot=pCurrentSpoolerObject;
                                        /* Allocate one record to insert into container */
        pSpoolerRecord=(SPOOLERRECORD *)WinSendMsg(pHP->hwndSpoolerContainer, CM_ALLOCRECORD,
            MPFROMLONG(sizeof(SPOOLERRECORD)-sizeof(MINIRECORDCORE)), MPFROMLONG(1));
                                        /* Chain SPOOLERRECORD and corresponding SPOOLEROBJECT together */
        pSpoolerRecord->pSpoolerObject=pCurrentSpoolerObject;
        pCurrentSpoolerObject->pSpoolerRecord=pSpoolerRecord;
                                        /* Fill record */
        pSpoolerRecord->pszQueue=strdup(pLastQueue->pszName);
        pSpoolerRecord->pszQueueStatus=strdup(ucCurrentQueueStatus);
        pSpoolerRecord->pszQueueComment=strdup(pLastQueue->pszComment);
        pSpoolerRecord->pszQueueDriver=strdup(pLastQueue->pszDriverName);
        pSpoolerRecord->pszJob=strdup(pCurrentJob->pszDocument);
        sprintf(ucBuffer, "%4d", pCurrentJob->uJobId);
        pSpoolerRecord->pszJobNumber=strdup(ucBuffer);
        pSpoolerRecord->pszJobStatus=strdup(ucCurrentJob);
        pSpoolerRecord->pszJobComment=strdup(pCurrentJob->pszComment);
        pSpoolerRecord->ulJob=pCurrentJob->uJobId;
                                        /* Insert record into container */
        WinSendMsg(pHP->hwndSpoolerContainer, CM_INSERTRECORD,
            MPFROMP(pSpoolerRecord), MPFROMP(&RecordInsert));
        }
    if(ulJobCount==0)
        {
/*                                                                                      *\
 * Because the current queue doesn't have at least on job, create a container record    *
 * anyway, even when there is no job data.                                              *
\*                                                                                      */
                                        /* Allocate, setup and chain in a SPOOLEROBJECT structure */
        pCurrentSpoolerObject=malloc(sizeof(SPOOLEROBJECT));
        memset(pCurrentSpoolerObject, '\0', sizeof(SPOOLEROBJECT));
        pCurrentSpoolerObject->pSpoolerObjectNext=NULL;
        pCurrentSpoolerObject->pSpoolerObjectBack=pAppendSpoolerObject;
        pCurrentSpoolerObject->bInQueue=TRUE;
        if(pAppendSpoolerObject!=NULL)
                                        /* Advance pointer to current record */
            pAppendSpoolerObject->pSpoolerObjectNext=pCurrentSpoolerObject;
        else
            pAppendSpoolerObject=pCurrentSpoolerObject;
                                        /* Save pointer to first record */
        if(pHP->pSpoolerObjectRoot!=NULL) pHP->pSpoolerObjectRoot=pCurrentSpoolerObject;
                                        /* Allocate one record in container */
        pSpoolerRecord=(SPOOLERRECORD *)WinSendMsg(pHP->hwndSpoolerContainer, CM_ALLOCRECORD,
            MPFROMLONG(sizeof(SPOOLERRECORD)-sizeof(MINIRECORDCORE)), MPFROMLONG(1));
                                        /* Chain SPOOLERRECORD and corresponding SPOOLEROBJECT together */
        pSpoolerRecord->pSpoolerObject=pCurrentSpoolerObject;
        pCurrentSpoolerObject->pSpoolerRecord=pSpoolerRecord;
                                        /* Fill record */
        pSpoolerRecord->pszQueue=strdup(pLastQueue->pszName);
        pSpoolerRecord->pszQueueStatus=strdup(ucCurrentQueueStatus);
        pSpoolerRecord->pszQueueComment=strdup(pLastQueue->pszComment);
        pSpoolerRecord->pszQueueDriver=strdup(pLastQueue->pszDriverName);
        pSpoolerRecord->pszJob=strdup("");
        pSpoolerRecord->pszJobNumber=strdup("");
        pSpoolerRecord->pszJobStatus=strdup("");
        pSpoolerRecord->pszJobComment=strdup("");
        pSpoolerRecord->ulJob=0;
                                        /* Insert record into container */
        WinSendMsg(pHP->hwndSpoolerContainer, CM_INSERTRECORD,
            MPFROMP(pSpoolerRecord), MPFROMP(&RecordInsert));
        }
                                        /* Last job of current queue reached, so current job
                                           points to next queue */
    pCurrentQueue=(PRQINFO3 *)pCurrentJob;
    }
free(pQueueInfo);
                                        /* Now draw changes in container window */
WinEnableWindowUpdate(pHP->hwndSpoolerContainer, TRUE);
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This subclassed window procedure handles the PC/2's Spooler container window.        *
 * Req: none                                                                            *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY SubclassedContainerWindowProc(HWND hwndMenu, ULONG msg, MPARAM mp1, MPARAM mp2)
{
switch(msg)
{
case WM_PRESPARAMCHANGED:
    switch((ULONG)mp1)
    {
    case PP_FONTNAMESIZE:
        {
        ULONG       ulAttrFound;

                                        /* Get font selected for PC/2's smart icon menu */
        WinQueryPresParam(hwndMenu, PP_FONTNAMESIZE, 0, &ulAttrFound,
            sizeof(pHP->ucPC2SpoolerFont), pHP->ucPC2SpoolerFont, 0);
        }
        break;
    }
}
                                        /* Call default window procedure */
return(pfnContainerWindowProc(hwndMenu, msg, mp1, mp2));
}

