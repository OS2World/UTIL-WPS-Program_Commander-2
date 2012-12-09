/***********************************************************************\
 *                                                                     *
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1993, 1999              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * Dialog.c     Dialog window procedures.                              *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#include        "PC2.h"                 /* User include files */
#include        "Error.h"

#define         _FILE_  "Dialog.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;

#ifndef BKS_TABBEDDIALOG
#define BKS_TABBEDDIALOG            (0x00000800)
#endif  /* BKS_TABBEDDIALOG */

#define IOCTL_DOS                   0x00D5
#define DOS_REBOOT                  0x00AB

                                        /* Program Installation dialog notebook pages */
NBPAGE          nbPINotebookPage[]=
{
    {PI_NotebookProcedure,  0,  0,  "Application Settings"        , "~Application",    PIID_PAGE1,     PIEF_PROGRAMTITLE},
    {PI_NotebookProcedure,  0,  0,  "Session Settings"            , "S~ession",        PIID_PAGE2,     PIRB_SHELL},
    {PI_NotebookProcedure,  0,  0,  "Style Settings"              , "S~tyle",          PIID_PAGE3,     PIRB_DEFAULTSTYLE},
    {PI_NotebookProcedure,  0,  0,  "Hotkey Settings"             , "Hot~key",         PIID_PAGE4,     PIEF_SESSIONTITLE},
    {PI_NotebookProcedure,  0,  0,  "Priority Settings"           , "~Priority",       PIID_PAGE5,     PIRB_NOCHANGE},
    {PI_NotebookProcedure,  0,  0,  "Environment Settings"        , "En~vironment",    PIID_PAGE6,     PIMLE_ENVIRONMENT},
    {PI_NotebookProcedure,  0,  0,  "Scheduler Settings"          , "Schedu~ler",      PIID_PAGE7,     PIRB_DISABLESCHEDULER},
    {NULL,                  0,  0,  ""                            , "",                0,              0}
};

                                        /* Submenu Installation dialog notebook pages */
NBPAGE          nbMINotebookPage[]=
{
    {MI_NotebookProcedure,  0,  0,  "Submenu Settings"            , "S~ubmenu",        MIID_PAGE1,     MIEF_MENUTITLE},
    {NULL,                  0,  0,  ""                            , "",                0,              0}
};

                                        /* Control Configuration dialog notebook pages */
NBPAGE          nbCCNotebookPage[]=
{
    {CC_NotebookProcedure,  0,  0,  "Current Control"             , "Con~trol",        CCID_PAGE1,     CCEF_CONTROL},
    {CC_NotebookProcedure,  0,  0,  "Scheduler Settings"          , "Schedu~ler",      CCID_PAGE2,     CCRB_DISABLESCHEDULER},
    {NULL,                  0,  0,  ""                            , "",                0,              0}
};

                                        /* Desktop Settings dialog notebook pages */
#define DD_PAGE_VIRTUAL1            DD_PAGE_1A
#define DD_PAGE_VIRTUAL2            DD_PAGE_1B
#define DD_PAGE_OVERVIEW            DD_PAGE_2
#define DD_PAGE_SESSIONBAR          DD_PAGE_3
#define DD_PAGE_PHYSICAL            DD_PAGE_4
#define DD_PAGE_GENERAL             DD_PAGE_5
#define DD_PAGE_TITLEBAR            DD_PAGE_6
#define DD_PAGE_SECURITY            DD_PAGE_7
#define DD_PAGE_ENVIRON1            DD_PAGE_8A
#define DD_PAGE_ENVIRON2            DD_PAGE_8B
#define DD_PAGE_REGISTRATION        DD_PAGE_9
#define DD_PAGE_SPY                 DD_PAGE_10

NBPAGE          nbDDNotebookPage[]=
{
    {DD_NotebookProcedure,  0,  0,  "Virtual Desktop Settings 1/2", "~Desktop",        DDID_PAGE1A,    DDCB_VIRTUALDESKTOP},
    {DD_NotebookProcedure,  0,  0,  "Virtual Desktop Settings 2/2", "",                DDID_PAGE1B,    DDCB_SLIDINGFOCUS},
    {DD_NotebookProcedure,  0,  0,  "Overview Window Settings"    , "~Overview",       DDID_PAGE2,     DDCB_3DBORDER},
    {DD_NotebookProcedure,  0,  0,  "SessionBar Settings"         , "Session~Bar",     DDID_PAGE3,     DDCB_SESSIONBAR},
    {DD_NotebookProcedure,  0,  0,  "Physical Desktop Settings"   , "Scree~n",         DDID_PAGE4,     DDCB_BUTTON2ZORDER},
    {DD_NotebookProcedure,  0,  0,  "General PC/2 Settings"       , "~General",        DDID_PAGE5,     DDCB_NORMALSHUTDOWN},
    {DD_NotebookProcedure,  0,  0,  "Titlebar SmartIcons Settings", "Smart~Icons",     DDID_PAGE6,     DDCB_TITLEBARICONS},
    {DD_NotebookProcedure,  0,  0,  "Security Settings"           , "Securit~y",       DDID_PAGE7,     DDCB_PC2SECURITY},
    {DD_NotebookProcedure,  0,  0,  "Environment Settings 1/2"    , "En~vironments",   DDID_PAGE8A,    DDLB_ENVIRONMENT},
    {DD_NotebookProcedure,  0,  0,  "Environment Settings 2/2"    , "",                DDID_PAGE8B,    DDEF_ENVIRONMENT},
    {DD_NotebookProcedure,  0,  0,  "Registration Settings"       , "~Registration",   DDID_PAGE9,     DDCB_AUTOUPDATEENABLE},
    {DD_NotebookProcedure,  0,  0,  "Spy Window Settings"         , "S~py",            DDID_PAGE10,    DDCB_SPYFRAMEWINDOW},
    {NULL,                  0,  0,  ""                            , "",                0,              0}
};

                                        /* Backup of password entered in a readable form,
                                           used for the Deskop Dialog password entryfields
                                           on the lockup notebook page */
UCHAR   ucDDAccessPassword[PASSWORDSIZE+1];
UCHAR   ucDDVerifyAccessPassword[PASSWORDSIZE+1];
UCHAR   ucDDLockupPassword[PASSWORDSIZE+1];
UCHAR   ucDDVerifyLockupPassword[PASSWORDSIZE+1];
UCHAR   ucDDUserHttpPassword[PASSWORDSIZE+1];

typedef struct _NOTEBOOKSUBCLASS    NOTEBOOKSUBCLASS;

struct  _NOTEBOOKSUBCLASS               /* Structure to save which notebook was subclassed
                                           from which previous window procedure */
{
HWND    hwndNotebook;                   /* Notebook window handle */
PFNWP   pfnwpNotebook;                  /* Notebook control's window procedure before subclassing */
};

#define DD_SUBCLASSEDNOTEBOOK       0   /* Desktop dialog notebook subclassed */
#define PI_SUBCLASSEDNOTEBOOK       1   /* Program Installation dialog notebook subclassed */
#define MI_SUBCLASSEDNOTEBOOK       2   /* Menu Installation dialog notebook subclassed */
#define CC_SUBCLASSEDNOTEBOOK       3   /* Control Configuration dialog notebook subclassed */
#define NOTEBOOKSUBCLASSMAX         4

                                        /* As only one instance of the Desktop and Program Installation
                                           dialogs is allowed, its save to avoid more complicated
                                           per dialog instantiation, but use module class storage */
NOTEBOOKSUBCLASS    DialogNotebookSubclass[NOTEBOOKSUBCLASSMAX];

/*--------------------------------------------------------------------------------------*\
 * This procedure processes the keys sent to a dialog that implements the page of a     *
 * notebook control. Navigation keys are processed within the dialog page, all other    *
 * keys are forwarded to the frame window whose client area the notebook control is     *
 * part of.                                                                             *
 * Req:                                                                                 *
 *      hwndPage ...... Dialog implementing notebook page                               *
 *      hwndDialog .... Dialog where the notebook control is part of the client         *
 *      mp1 ........... Parameter 1 of WM_CHAR message sent to notebook page            *
 *      mp1 ........... Parameter 2 of WM_CHAR message sent to notebook page            *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: TRUE no error, FALSE otherwise                     *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
MPARAM  ProcessPageKey(HWND hwndPage, HWND hwndDialog, MPARAM mp1, MPARAM mp2)
{
#ifdef  DEBUG_DIALOG
printf("Dialog: Page - Got key (%02X)", (int)CHAR1FROMMP(mp2));
if(((CHAR1FROMMP(mp2))>=0x20) && ((CHAR1FROMMP(mp2))<0x7F))
    printf("(%c)", (char)(SHORT1FROMMP(mp2)));
printf("\n");
#endif  /* DEBUG_DIALOG */
                                        /* Process only the navigation keys in this dialog
                                           procedure, that implements a notebook dialog page. Forward
                                           all other keys to the dialog the notebook is part of the
                                           client */
if(SHORT1FROMMP(mp1) & KC_VIRTUALKEY)
    if((SHORT2FROMMP(mp2)==VK_TAB) ||
        (SHORT2FROMMP(mp2)==VK_BACKTAB) ||
        (SHORT2FROMMP(mp2)==VK_LEFT) ||
        (SHORT2FROMMP(mp2)==VK_RIGHT) ||
        (SHORT2FROMMP(mp2)==VK_UP) ||
        (SHORT2FROMMP(mp2)==VK_DOWN))
        {
#ifdef  DEBUG_DIALOG
        printf("Dialog: Page - is a navigation key\n");
#endif  /* DEBUG_DIALOG */
                                        /* Let the dialog page's window procedure handle the navigation
                                           keys (a break statement would just ignore the key the way this
                                           dialog is implemented) */
        return(WinDefDlgProc(hwndPage, WM_CHAR, mp1, mp2));
        }
#ifdef  DEBUG_DIALOG
printf("Dialog: Page - not a navigation key\n");
#endif  /* DEBUG_DIALOG */
                                        /* See If the main dialog handled this keystroke */
if((BOOL)WinSendMsg(hwndDialog, WM_CHAR, mp1, mp2))
    {
#ifdef  DEBUG_DIALOG
    printf("Dialog: Dialog - we handled it\n");
#endif  /* DEBUG_DIALOG */
    return((MRESULT)TRUE);
    }
#ifdef  DEBUG_DIALOG
printf("Dialog: Page - we handled it\n");
#endif  /* DEBUG_DIALOG */
return((MRESULT)FALSE);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure gets invoked from the dialog to forwards keystrokes (created within   *
 * the dialog itself, or passed on via the owner chain) to the notebook. The notebook   *
 * then has the chance to see if the keystroke matches with one of it accelerator keys. *
 * Usually this is done automatically via the owner chain, but the notebook control has *
 * some peculiar behaviour (design flaws) here.                                         *
 * This procedure is called when processing the WM_CHAR message in the "root" dialog's  *
 * window procedure.                                                                    *
 * Req:                                                                                 *
 *      pulRecursion .. We have to prevent recursive calling of the code via PM         *
 *                      messages to prevent looping                                     *
 *      hwndDialog .... Dialog where the notebook control is part of the client         *
 *      hwndNotebook .. Notebook control itself (part of the client)                    *
 *      pfnwpNotebook . Window procedure of notebook control                            *
 *      mp1 ........... Window procedure parameter 1                                    *
 *      mp2 ........... Window procedure parameter 2                                    *
 * Ret:                                                                                 *
 *      mresult........ Result of processing WM_CHAR                                    *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
MRESULT DispatchKeyToNotebook(ULONG *pulRecursion, HWND hwndDialog, HWND hwndNotebook, PFNWP pfnwpNotebook,
    MPARAM  mp1, MPARAM mp2)
{

                                        /* We have to exclude VK_NEWLINE because this does not allow
                                           us to enter a newline into multi-line entryfields. On
                                           Laptop computers the CRLF key causes a VK_NEWLINE, and
                                           the ENTER key can only be accessed by SHIFT+CRLF (at least
                                           on my ThinkPad) */
if(SHORT2FROMMP(mp2)==VK_ENTER)
    {
#ifdef  DEBUG_DIALOG
    printf("Dialog: Dialog - Enter\n");
#endif  /* DEBUG_DIALOG */
    WinPostMsg(hwndDialog, WM_COMMAND,
        MPFROMSHORT(DID_OK), MPFROM2SHORT(CMDSRC_ACCELERATOR, FALSE));
    }
if(SHORT2FROMMP(mp2)==VK_ESC)
    {
#ifdef  DEBUG_DIALOG
    printf("Dialog: Dialog - ESC\n");
#endif  /* DEBUG_DIALOG */
    WinPostMsg(hwndDialog, WM_COMMAND,
        MPFROMSHORT(DID_CANCEL), MPFROM2SHORT(CMDSRC_ACCELERATOR, FALSE));
    }
                                        /* Prevent recursion and prevent message processing then */
if(*pulRecursion==TRUE)
    return((MRESULT)FALSE);
                                        /* Avoid recursion and process key */
*pulRecursion=TRUE;
if((SHORT1FROMMP(mp1) & KC_ALT) &&
    !(SHORT1FROMMP(mp1) & KC_VIRTUALKEY) &&
    !(SHORT1FROMMP(mp1) & KC_KEYUP))
    {
#ifdef  DEBUG_DIALOG
    printf("Dialog: Dialog - Got key (%02X)", (int)(char)(SHORT1FROMMP(mp2)));
    if(((char)SHORT1FROMMP(mp2)>=0x20) && ((char)SHORT1FROMMP(mp2)<0x7F))
        printf("(%c)", (char)(SHORT1FROMMP(mp2)));
    printf("\n");
#endif  /* DEBUG_DIALOG */
                                        /* Let's see if this is a shortcut key for our dialog the
                                           notebook is part of the client */
    if(!(BOOL)WinDefDlgProc(hwndDialog, WM_CHAR, mp1, mp2))
        {
                                        /* Notebook page on top */
        HWND    hwndNotebookPage;

#ifdef  DEBUG_DIALOG
        printf("Dialog: Dialog - we didn't handle it\n");
#endif  /* DEBUG_DIALOG */
        hwndNotebookPage=(HWND)WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                (MPARAM)WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, NULL, MPFROM2SHORT(BKA_TOP, 0)),
                    NULL);
                                        /* Let's see if this is a shortcut key for the dialog that
                                           implements the current notebook page on top */
        if(!(BOOL)WinDefDlgProc(hwndNotebookPage, WM_CHAR, mp1, mp2))
            {
#ifdef  DEBUG_DIALOG
            printf("Dialog: Page - we didn't handle it\n");
#endif  /* DEBUG_DIALOG */
                                        /* Let's see if this is a notebook shortcut (with no ALT
                                           modifier) and activate the dialog page to give it the
                                           focus then */
            if((BOOL)(pfnwpNotebook)(hwndNotebook, WM_CHAR, MPFROM2SHORT(KC_CHAR, SHORT2FROMMP(mp1)), mp2))
                {
                SWP     swpPage;

#ifdef  DEBUG_DIALOG
                printf("Dialog: Notebook - we handled it\n");
#endif  /* DEBUG_DIALOG */
                memset(&swpPage, 0, sizeof(swpPage));
                swpPage.fl=SWP_ACTIVATE;
                swpPage.hwndInsertBehind=HWND_TOP;
                swpPage.hwnd=hwndNotebookPage;
                WinSetMultWindowPos(pHP->habPc2, &swpPage, 1);
                                        /* Now that a different notebook page is on top, get the new
                                           one to activate the control previously having the focus */
                hwndNotebookPage=(HWND)WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                        (MPARAM)WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, NULL, MPFROM2SHORT(BKA_TOP, 0)),
                            NULL);
#ifdef  DEBUG_DIALOG
                printf("Dialog: Page - activating control %08X on page %08X\n",
                WinQueryWindowULong(hwndNotebookPage, QWL_HWNDFOCUSSAVE), hwndNotebookPage);
#endif  /* DEBUG_DIALOG */
                WinSetFocus(HWND_DESKTOP, WinQueryWindowULong(hwndNotebookPage, QWL_HWNDFOCUSSAVE));
                }
#ifdef  DEBUG_DIALOG
            else
                printf("Dialog: Notebook - we didn't handle it\n");
#endif  /* DEBUG_DIALOG */
            }
        }
    *pulRecursion=FALSE;
    return((MRESULT)TRUE);
    }
*pulRecursion=FALSE;
return((MRESULT)FALSE);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure adds notebook pages from a NBPAGE template into a notebook, an        *
 * calculates the size required to hold the largest notebook tab text.                  *
 * Req:                                                                                 *
 *      hwndDialog .... Dialog where the notebook control is part of the client         *
 *      hwndNotebook .. Notebook control that is part of the client                     *
 *      nbpageNotebook  The NBPAGE structure the notebook pages are filled from         *
 *      pusTabTextLength                                                                *
 *                      Maximum length of the notebook's tab text                       *
 *      pusTabTextHeight                                                                *
 *                      Maximum height of the notebook's tab text (from                 *
 *                      FONTMETRICS.lMaxBaselineExt)                                    *
 * Ret:                                                                                 *
 *      mpRc .......... Return code: FALSE no error, errorcode otherwise                *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
MPARAM  AddPagesToNotebook(HWND hwndDialog, HWND hwndNotebook, NBPAGE nbpageNotebook[],
                           USHORT *pusTabTextLength, USHORT *pusTabTextHeight)
{
HPS         hps;                        /* Used to query font metrics for tab size */
FONTMETRICS fmFontMetrics;
POINTL      aptlTabText[TXTBOX_COUNT];
ULONG       ulPage, ulPageID;

                                        /* Get presentation space to query fontmetrics */
hps=WinGetPS(hwndNotebook);
memset(&fmFontMetrics, 0, sizeof(FONTMETRICS));
if(GpiQueryFontMetrics(hps, sizeof(FONTMETRICS), &fmFontMetrics))
    fmFontMetrics.lMaxBaselineExt<<=1;
else
    fmFontMetrics.lMaxBaselineExt=30;
*pusTabTextHeight=fmFontMetrics.lMaxBaselineExt;
                                        /* Load and associate all dialogs of the notebook pages.
                                           For unknown reasons, PM sends WM_CONTROL messages
                                           while processing WinLoadDlg(), which causes the WM_CONTROL
                                           processing to change the state of controls on pages not
                                           already loaded yet, sometimes (from PC2.TRP) this must
                                           have caused a trap in WinWindowFromID() while processing
                                           DisableDialogItem(). To avoid trap we clear the NBPAGE
                                           structure from last invocation of dialog */
for(ulPage=0; nbpageNotebook[ulPage].pDialogWndProc!=NULL; ulPage++)
    {
    nbpageNotebook[ulPage].hwndNBPage=NULLHANDLE;
    nbpageNotebook[ulPage].ulIDPage=0;
    }
for(ulPage=0; nbpageNotebook[ulPage].pDialogWndProc!=NULL; ulPage++)
    {
    USHORT  usPageStyle=(BKA_STATUSTEXTON | BKA_AUTOPAGESIZE);
    int     iTabLength;
                                        /* Load a dialog into a page */
    if(!(nbpageNotebook[ulPage].hwndNBPage=WinLoadDlg(
        hwndDialog,                     /* Parent window */
        hwndDialog,                     /* Owner window */
                                        /* Window procedure */
        nbpageNotebook[ulPage].pDialogWndProc,
        pHP->hDLLPc2Resource,           /* Resource idendity */
                                        /* Dialog idendity */
        nbpageNotebook[ulPage].ulIDDialogPage,
        NULL)))                         /* Application defined data area */
                                        /* On error suggest exiting */
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Creation of a dialog box failed - continuing...");
                                        /* Query length of largest tab text */
    iTabLength=strlen(nbpageNotebook[ulPage].pszTab);
    if(iTabLength)
        {
        if(GpiQueryTextBox(hps, iTabLength,
            nbpageNotebook[ulPage].pszTab, TXTBOX_COUNT, aptlTabText))
            if(*pusTabTextLength<aptlTabText[TXTBOX_CONCAT].x)
                *pusTabTextLength=aptlTabText[TXTBOX_CONCAT].x;
        usPageStyle|=BKA_MAJOR;
        }
    ulPageID=(ULONG)WinSendMsg(         /* Insert a page into the notebook */
        hwndNotebook,
        BKM_INSERTPAGE,
        (MPARAM)NULL,                   /* Page ID, ignored if BKA_FIRST or BKA_LAST specified */
                                        /* Style and order attribute */
        MPFROM2SHORT(usPageStyle, BKA_LAST));
    nbpageNotebook[ulPage].ulIDPage=ulPageID;
                                        /* Set focus */
    WinSetFocus(HWND_DESKTOP, nbpageNotebook[ulPage].ulIDFocus);
                                        /* Set text into the status line */
    WinSendMsg(hwndNotebook,
        BKM_SETSTATUSLINETEXT,
        MPFROMP(ulPageID),
        MPFROMP(nbpageNotebook[ulPage].pszStatusLine));
                                        /* Set text into tab */
    WinSendMsg(hwndNotebook,
        BKM_SETTABTEXT,
        MPFROMP(ulPageID),
        MPFROMP(nbpageNotebook[ulPage].pszTab));
                                        /* Associate page with dialog */
    WinSendMsg(hwndNotebook,
        BKM_SETPAGEWINDOWHWND,
        MPFROMP(ulPageID),
        MPFROMLONG(nbpageNotebook[ulPage].hwndNBPage));
    }
WinReleasePS(hps);
return(0);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure ensures that the dialog is optimally sized regarding to its client    *
 * controls, which are the notebook and the DID_OK, DID_CANCEL and DID_HELP             *
 * pushbuttons. The notebook pages are assumed to be NOTEBOOKPAGE_CX by NOTEBOOKPAGE_CY *
 * in size, the the DID_HELP value is assumed to be DID_CANCEL+1.                       *
 * Req:                                                                                 *
 *      hwndDialog .... Dialog where the notebook control is part of the client         *
 *      hwndNotebook .. Notebook control that is part of the client                     *
 *      ulNotebookStyle QWL_STYLE of the notebook control (required to take the new     *
 *                      notebook style into account that was introduced by Merlin beta) *
 *      usTabTextLength Maximum length of the notebook's tab text                       *
 *      usTabTextLength Maximum height of the notebook's tab text (from                 *
 *                      FONTMETRICS.lMaxBaselineExt, where we can apply a factor 0.8)   *
 * Ret:                                                                                 *
 *      mpRc .......... Return code: FALSE no error, errorcode otherwise                *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
MPARAM  AdjustNotebook(HWND hwndDialog, HWND hwndNotebook, ULONG ulNotebookStyle,
                       USHORT usTabTextLength, USHORT usTabTextHeight)
{
RECTL       rectlNB;                    /* Notebook rectangle to load notebook dialog page into */
POINTL      ptlNB;                      /* Notebook page conversion from dialog units to pixels */
SWP         swpNB;
SWP         swpDialog;
ULONG       ulButton;
ULONG       ulLastButton;
ULONG       ulEdge;
ULONG       ulSpace;
SWP         swpButton;


WinSendMsg(hwndNotebook,                /* Set background to dialog box background */
    BKM_SETNOTEBOOKCOLORS,
    MPFROMLONG(SYSCLR_FIELDBACKGROUND),
    MPFROMSHORT(BKA_BACKGROUNDPAGECOLORINDEX));
WinSendMsg(hwndNotebook,                /* Set tab dimension */
    BKM_SETDIMENSIONS,
    MPFROM2SHORT(usTabTextLength+5, (SHORT)((float)usTabTextHeight*0.8)),
    MPFROMSHORT(BKA_MAJORTAB));
WinSendMsg(hwndNotebook,                /* Set tab dimension */
    BKM_SETDIMENSIONS,
    MPFROM2SHORT(0 ,0),
    MPFROMSHORT(BKA_MINORTAB));
                                        /* Notebook page dialog panel size in dialog units */
ptlNB.x=NOTEBOOKPAGE_CX;
ptlNB.y=NOTEBOOKPAGE_CY;
                                        /* Map dialog units to screen pixels */
WinMapDlgPoints(hwndDialog, &ptlNB, 1, TRUE);
                                        /* Query frame and notebook size and position in pixels */
WinQueryWindowPos(hwndDialog, &swpDialog);
WinQueryWindowPos(hwndNotebook, &swpNB);
rectlNB.yBottom=rectlNB.xLeft=0;
rectlNB.xRight=swpNB.cx;
rectlNB.yTop=swpNB.cy;
                                        /* Return size of notebook page in pixel for given
                                           notebook window size (with tab text size set) */
WinSendMsg(hwndNotebook, BKM_CALCPAGERECT, MPFROMP(&rectlNB), MPFROMSHORT(TRUE));
                                        /* Calculate notebook size so that notebook dialog page fits
                                           into the notebook page */
swpNB.cx-=(rectlNB.xRight-rectlNB.xLeft)-ptlNB.x;
swpNB.cy-=(rectlNB.yTop-rectlNB.yBottom)-ptlNB.y;
                                        /* Size notebook accordingly */
swpNB.fl=SWP_SIZE|SWP_MOVE;
swpNB.hwndInsertBehind=NULLHANDLE;
                                        /* Use the DID_OK pushbutton as a reference */
WinQueryWindowPos(WinWindowFromID(hwndDialog, DID_OK), &swpButton);
                                        /* As we have created the notebook dynamically (not from
                                           dialog resource) because Merlin does not support to set
                                           the BKS_TABBEDDIALOG window style to be set by QWL_STYLE,
                                           we have to adjust from 0 position and size */
ulEdge=swpNB.x=swpButton.x;
                                        /* Take pushbutton selection border into account */
swpButton.fl=SWP_MOVE;
swpButton.x++;
WinSetMultWindowPos(pHP->habPc2, &swpButton, 1);
                                        /* Fit the notebook above the pushbuttons (equidistant
                                           to the pushbutton's position to the frame border).
                                           This requires the CS_CLIPCHILDREN style not to
                                           overpaint the pushbuttons */
swpNB.y=swpButton.cy+(swpButton.y<<1)-1;
if(((pHP->ulVersionMajor==OS2_MAJOR) && (pHP->ulVersionMinor>=OS2_MINOR_400)) ||
    (pHP->ulVersionMajor>OS2_MAJOR))
    swpNB.y=(swpButton.y<<1)-2;
                                        /* Size the dialog to the notebook's size (including the
                                           vertical space requirements of the pushbuttons) */
swpDialog.fl=SWP_SIZE;
swpDialog.cx=swpNB.cx+(ulEdge<<1);
swpDialog.cy=swpNB.y+swpNB.cy+WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR)+
    (WinQuerySysValue(HWND_DESKTOP,SV_CYDLGFRAME)<<1);
WinSetMultWindowPos(pHP->habPc2, &swpDialog, 1);
                                        /* Move the DID_CANCEL, DID_HELP and optional other
                                           pushbuttons being DID_CANCEL+x where x>1 (e.g.
                                           DID_BACKUP) to be inside the new dialog's size and
                                           being equidistant.
                                           Note: DID_HELP, DID_BACKUP pushbutton must have IDs
                                                 starting with DID_CANCEL+1 and continuous */
for(ulLastButton=DID_CANCEL+3; ulLastButton>DID_CANCEL; ulLastButton--)
    if(WinWindowFromID(hwndDialog, ulLastButton)!=NULLHANDLE)
        break;
ulSpace=(swpDialog.cx-(ulEdge<<1)-(swpButton.cx*ulLastButton))/(ulLastButton-DID_OK);
                                        /* Now we know the number of DID_CANCEL+x pushbuttons
                                           adjust spacing between buttons accordingly (of course
                                           all buttons must be of the same width). Position the
                                           buttons in the sequence DID_OK, DID_CANCEL, DID_CANCEL+x,
                                           ..., DID_HELP */
for(ulButton=ulLastButton; ulButton>=DID_CANCEL; ulButton--)
    {
    ULONG   ulButtonAdjusted;

    if(ulButton>DID_HELP)
        ulButtonAdjusted=ulButton-1;
    else if(ulButton==DID_HELP)
        ulButtonAdjusted=ulLastButton;
    else if(ulButton<DID_HELP)
        ulButtonAdjusted=ulButton;
    WinQueryWindowPos(WinWindowFromID(hwndDialog, ulButton), &swpButton);
    swpButton.fl=SWP_MOVE;
    swpButton.x=swpDialog.cx-ulEdge-
        (swpButton.cx*(ulLastButton-ulButtonAdjusted+1))-
        (ulSpace*(ulLastButton-ulButtonAdjusted));
    WinSetMultWindowPos(pHP->habPc2, &swpButton, 1);
    }
WinSetMultWindowPos(pHP->habPc2, &swpNB, 1);
                                        /* Center frame window */
CenterWindow(hwndDialog);
return(0);
}

/*--------------------------------------------------------------------------------------*\
 * This window procedure is a subclass procedure of a notebook, to support pressing     *
 * accelerator keys within a notebook page, the notebook itself and the dialog the      *
 * notebook is part of the client. Without this subclassing the notebook would not      *
 * forward keys it is not interested in to its owner (the dialog), but the owner would  *
 * probably be interested in such keys.                                                 *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY SubclassedNotebookProcedure(HWND hwndNotebook, ULONG msg, MPARAM mp1, MPARAM mp2)
{
ULONG   ulIndex;

for(ulIndex=0; ulIndex<NOTEBOOKSUBCLASSMAX; ulIndex++)
    {
    if(DialogNotebookSubclass[ulIndex].hwndNotebook==hwndNotebook)
        {
        if(DialogNotebookSubclass[ulIndex].pfnwpNotebook==NULL)
            {
#ifdef  DEBUG_DIALOG
            printf("Dialog: %ld Fatal error, pfnwpNotebook is 0\n", __LINE__);
            DosBeep(1500, 2000);
#endif  /* DEBUG_DIALOG */
            break;
            }
        if(msg==WM_CHAR)
            {
                                        /* We are only interested in WM_CHAR messages as a
                                           result of keystrokes the notebook is willing to accept.
                                           We CALL the window procdure to receive the evaluation,
                                           as we have to pass on to the owner what the notebook
                                           ignores e.g. ALT+C for the Cancel pushbutton (the notebook
                                           should do this itself but doesn't) */
            if(!(BOOL)(DialogNotebookSubclass[ulIndex].pfnwpNotebook)(hwndNotebook, msg, mp1, mp2))
                {
#ifdef  DEBUG_DIALOG
                printf("Dialog: Notebook - forwarding to dialog\n");
#endif  /* DEBUG_DIALOG */
                return(WinSendMsg(WinQueryWindow(hwndNotebook, QW_OWNER), msg, mp1, mp2));
                }
            else
                break;
            }
        else
            {
                                        /* All messages but WM_CHAR are routed to the window
                                           procedure before subclassing */
            return((DialogNotebookSubclass[ulIndex].pfnwpNotebook)(hwndNotebook, msg, mp1, mp2));
            }
        }
    }
return((MRESULT)TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure queries all selected lines in a list box and concatenates them        *
 * separated by \r\n into an dynamically allocated string. This resulting string is     *
 * then returned.                                                                       *
 * Req:                                                                                 *
 *      pucConcat ..... String to concatenate all selections                            *
 *      hwndDlg ....... Dialog window containing the listbox control                    *
 *      usListBoxID ... Identifier of the listbox within that dialog window             *
 * Ret:                                                                                 *
 *      pucConcat ..... Concatenated selections in listbox separated by \r\n            *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
UCHAR   *QueryEnvironmentSpaces(UCHAR *pucConcat, HWND hwndDlg, USHORT usListBoxID)
{
ULONG   ulIndex;
ULONG   ulLength;
UCHAR   *pucTemp;

                                /* Query selected listbox item and add it into
                                   a dynamically allocated buffer separated by \r\n */
ulIndex=LIT_FIRST;
while((ulIndex=(ULONG)WinSendDlgItemMsg(hwndDlg, usListBoxID,
    LM_QUERYSELECTION, (MPARAM)ulIndex, NULL))!=(ULONG)LIT_NONE)
    {
    ulLength=(ULONG)WinSendDlgItemMsg(hwndDlg, usListBoxID,
        LM_QUERYITEMTEXTLENGTH, (MPARAM)ulIndex, NULL);
    if(ulLength==(ULONG)LIT_ERROR)
        continue;
                                /* Allocate space to concatenate current listbox item
                                   including \r\n\0 termination */
    ulLength=strlen(pucConcat)+ulLength+3;
    pucTemp=malloc(ulLength);
    if(pucTemp==NULL)
        break;
    strcpy(pucTemp, pucConcat);
    free(pucConcat);
    pucConcat=pucTemp;
    pucTemp+=strlen(pucTemp);
    WinSendDlgItemMsg(hwndDlg, usListBoxID,
        LM_QUERYITEMTEXT, MPFROM2SHORT((SHORT)ulIndex, (SHORT)ulLength+1), (MPARAM)pucTemp);
    strcat(pucTemp, "\r\n");
    }
return(pucConcat);
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - About dialog.                               *
 * Req: none                                                                            *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY AD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
switch(msg)
{
case WM_INITDLG:
    {
    UCHAR   ucBuffer[128];

    sprintf(ucBuffer, "Build: %s %s", __DATE__, __TIME__);
    WinSetDlgItemText(hwndDlg, ADID_ABOUTBLDLEVEL, ucBuffer);
    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
                                        /* Center frame window */
    CenterWindow(hwndDlg);
    }
    break;

case WM_HELP:                           /* Help pressed */
    if(pHP->hwndHelp!=NULLHANDLE)
        WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
            MPFROMSHORT(ID_ABOUTDIALOG), HM_RESOURCEID);
    break;

case WM_COMMAND:                        /* Button pressed */
    switch(SHORT1FROMMP(mp1))
    {
    case DID_OK:                        /* Enter key pressed */
                                        /* Dialog terminated with DID_OK */
        WinDismissDlg(hwndDlg, DID_OK);
        break;

    case DID_CANCEL:                    /* Cancel key pressed */
                                        /* Dialog terminated with DID_CANCEL */
        WinDismissDlg(hwndDlg, DID_CANCEL);
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Desktop configuration dialog.               *
 * Req: none                                                                            *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY DD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static HWND         hwndNB;             /* Program Installation dialog notebook window handle */

switch(msg)
{
case WM_INITDLG:
    {
                                        /* Notebook style (to take the new Merlin notebook style into
                                           account, just try out SET NEWNOTEBOOS=YES in your Merlin
                                           beta CONFIG.SYS with your "old"-style notebooks).
                                           Under OS/2 WARP 4 (aka "Merlin"), there seems to be a bug,
                                           as a notebook created from a dialog resouce must already
                                           have the BKS_TABBEDNOTEBOOK style set when being created,
                                           as setting the window style dynamically does corrupt the
                                           notebook drawing */
    ULONG               ulNBStyle;
                                        /* Usually the first notebook page is displayed, but when
                                           e.g. no registration data is available, the Registration
                                           page will be displayed */
    DESKTOPDIALOGPARAMS *pdesktopdialogParams=(DESKTOPDIALOGPARAMS *)mp2;
    USHORT              usTabTextLength=50;
    USHORT              usTabTextHeight=5;

#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld -> WM_INITDLG\n", __LINE__);
#endif  /* DEBUG_DIALOG */
    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
    ulNBStyle=WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE;
    if(((pHP->ulVersionMajor==OS2_MAJOR) && (pHP->ulVersionMinor>=OS2_MINOR_400)) ||
        (pHP->ulVersionMajor>OS2_MAJOR))
        ulNBStyle|=(BKS_TABBEDDIALOG|BKS_MAJORTABTOP|BKS_BACKPAGESTR|
            BKS_STATUSTEXTRIGHT|BKS_TABTEXTCENTER|BKS_POLYGONTABS);
    else
        ulNBStyle|=(BKS_SPIRALBIND|BKS_MAJORTABRIGHT|BKS_BACKPAGESBR|
            BKS_STATUSTEXTCENTER|BKS_TABTEXTRIGHT);
    hwndNB=WinCreateWindow(hwndDlg, WC_NOTEBOOK, "", ulNBStyle, 0, 0, 0, 0, hwndDlg,
        HWND_BOTTOM, DDNB_NOTEBOOK, NULL, NULL);
                                        /* As the Lockup Settings notebook page may display a message
                                           box being this dialog the owner, associate the main helptable
                                           with this dialog */
    WinAssociateHelpInstance(pHP->hwndHelp, hwndDlg);
                                        /* Subclass notebook's window procedure to add handling of
                                           notebook pages, notebook and outer dialog accelerator keys */
    DialogNotebookSubclass[DD_SUBCLASSEDNOTEBOOK].hwndNotebook=hwndNB;
    DialogNotebookSubclass[DD_SUBCLASSEDNOTEBOOK].pfnwpNotebook=
        WinSubclassWindow(hwndNB, SubclassedNotebookProcedure);
                                        /* Fill the notebook and return the tab text requirements */
    AddPagesToNotebook(hwndDlg, hwndNB, nbDDNotebookPage, &usTabTextLength, &usTabTextHeight);
                                        /* Adjust dialog to optimally cover the notebook */
    AdjustNotebook(hwndDlg, hwndNB, ulNBStyle, usTabTextLength, usTabTextHeight);
                                        /* Now initialize all notebook page dialogs
                                           by posting to 1st page (the subclassed entryfield
                                           are already chained in) */
    WinPostMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, WM_SETUPNOTEBOOKPAGES,
        MPFROMHWND(hwndNB), mp2);
                                        /* Save spy notebook page window handle */
    pHP->hwndSpyFrameWindow=nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage;
                                        /* Set focus */
    WinSetFocus(HWND_DESKTOP, nbDDNotebookPage[pdesktopdialogParams->usTurnToPage].ulIDFocus);

#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld <- WM_INITDLG\n", __LINE__);
#endif  /* DEBUG_DIALOG */
    return((MRESULT)TRUE);
    }

case WM_DESTROY:
                                        /* Deassociate help instance */
    WinAssociateHelpInstance(NULLHANDLE, hwndDlg);
                                        /* Clear spy notebook page window handle */
    pHP->hwndSpyFrameWindow=NULLHANDLE;
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));

case WM_CHAR:
/*                                                                                      *\
 * All window procedure implementing a noteboook page and the notebook control itself,  *
 * which is part of this window's client, forward keystroke messages they don't have    *
 * handled themselfes here.                                                             *
\*                                                                                      */
    {
    static ULONG    ulRecursion=FALSE;

    return(DispatchKeyToNotebook(&ulRecursion, hwndDlg, hwndNB,
        DialogNotebookSubclass[DD_SUBCLASSEDNOTEBOOK].pfnwpNotebook, mp1, mp2));
    }

case WM_HELP:                           /* Help pressed */
    {
    ULONG   ulPageId, ulPage;
    ULONG   ulHelp[]={DDID_PAGE1A, DDID_PAGE1B, DDID_PAGE2, DDID_PAGE3, DDID_PAGE4,
                      DDID_PAGE5, DDID_PAGE6, DDID_PAGE7, DDID_PAGE8A, DDID_PAGE8B,
                      DDID_PAGE9, DDID_PAGE10};

    ulPageId=(ULONG)WinSendMsg(hwndNB,  /* Query the currently active page */
        BKM_QUERYPAGEID,
        MPFROMLONG(0),                  /* Location page ID, ignored if */
                                        /* Page ID is BKA_TOP */
        MPFROM2SHORT(BKA_TOP, 0));
    for(ulPage=0; nbDDNotebookPage[ulPage].pDialogWndProc!=NULL; ulPage++)
        if(nbDDNotebookPage[ulPage].ulIDPage==ulPageId)
            if(pHP->hwndHelp!=NULLHANDLE)
                WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
                    MPFROMSHORT(ulHelp[ulPage]), HM_RESOURCEID);
    }
    break;

case WM_CONTROL:
/*                                                                                      *\
 * When selecting a new notebook page on top, we enable the Add environment pushbutton  *
 * on the first Environment page, if the second environment space contains some data,   *
 * and the Environment Space's name does not exist allready.                            *
\*                                                                                      */
    if((SHORT1FROMMP(mp1)==DDNB_NOTEBOOK) && (SHORT2FROMMP(mp1)==BKN_PAGESELECTED))
        {
        ENVIRONMENTLIST *pEnvironmentListCurrent;
        UCHAR           *pucEnvironmentName;
        ULONG           ulLength;

                                        /* Get the Environment Space's name from the second page */
        pucEnvironmentName=malloc(EF_SIZE255);
        if(pucEnvironmentName==NULL)
            break;
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDEF_ENVIRONMENT),
            EF_SIZE255, pucEnvironmentName);
                                        /* Query the environment variables from the second page.
                                           MLM_QUERYTEXTLENGTH returns \r\n as 1 byte, whereas
                                           WinQueryWindowTextLength() returns the real size */
        ulLength=(ULONG)WinQueryWindowTextLength(WinWindowFromID(
            nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDMLE_ENVIRONMENT));
                                        /* If the second page has been filled in correctly, we
                                           can see if its time to change the Add or Change
                                           pushbutton */
        if((*pucEnvironmentName!='\0') && (ulLength!=0))
            {
                                        /* Loop through Environment Spaces sequentially and
                                           break out if we find the an entry with the same name */
            for(pEnvironmentListCurrent=pHP->pEnvironmentSpaces;
                pEnvironmentListCurrent!=NULL;
                pEnvironmentListCurrent=pEnvironmentListCurrent->pNext)
                {
                if(!strcmp(pEnvironmentListCurrent->pEnvironmentName, pucEnvironmentName))
                    {
                                        /* Enable the Change pushbutton if we found an
                                           Environment Space with the same name */
                    WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_CHANGEENVIRONMENT),
                        TRUE);
                    break;
                    }
                }
                                        /* If we couldn't find an Environment Space with the
                                           same name in the LIST, we can add it */
            if(pEnvironmentListCurrent==NULL)
                {
                                        /* Deselect the Environment Spaces listbox, because this
                                           is a new one the user has entered */
                WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT, LM_SELECTITEM,
                    MPFROMSHORT(LIT_NONE), NULL);
                                        /* Enable the Add and disable the Change and Remove pushbutton */
                WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_ADDENVIRONMENT),
                    TRUE);
                WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_CHANGEENVIRONMENT),
                    FALSE);
                WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_REMOVEENVIRONMENT),
                    FALSE);
                }
            }
        else
            {
                                        /* Disable the Add and Change pushbutton */
            WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_ADDENVIRONMENT),
                FALSE);
            WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_CHANGEENVIRONMENT),
                FALSE);

            }
        free(pucEnvironmentName);
        }
    break;

case WM_COMMAND:                        /* Button pressed */
/*                                                                                      *\
 * Before extracting any data, verify that the passwords entered are valid. We do this  *
 * here, because if they don't match we keep the dialog and extracting data first would *
 * take the data into effect, even if the user chooses to dismiss the dialog without    *
 * saving the changes after the password invalidation messagebox was dismissed.         *
\*                                                                                      */
    if(SHORT1FROMMP(mp1)==DID_OK)
        {
                                        /* We read the presentation parameter values by sending
                                           a WM_CONTROL to a random radiobutton again, which
                                           will cause the current values to be read (and new ones
                                           be set, but for that noone cares for) */
        WinSendMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, WM_CONTROL,
            MPFROM2SHORT(DDRB_OVERVIEWICONS, BN_CLICKED), NULL);
                                        /* Compare the Access Password and the verification password */
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_ACCESSPASSWORD),
            sizeof(ucDDAccessPassword), ucDDAccessPassword);
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_VERIFYACCESSPWD),
            sizeof(ucDDVerifyAccessPassword), ucDDVerifyAccessPassword);
        if(strcmp(ucDDAccessPassword, ucDDVerifyAccessPassword))
            {                           /* Inform user and keep the dialog */
            USR_ERR(hwndDlg, HELP_PC2BADPASSWORD, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "The Access Password and its verification do not match! "
                "Please reenter - continuing... ");
            break;
            }
                                        /* Compare the Lockup Password and the verification password */
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_LOCKUPPASSWORD),
            sizeof(ucDDLockupPassword), ucDDLockupPassword);
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_VERIFYLOCKUPPWD),
            sizeof(ucDDVerifyLockupPassword), ucDDVerifyLockupPassword);
        if(strcmp(ucDDLockupPassword, ucDDVerifyLockupPassword))
            {                           /* Inform user and keep the dialog */
            USR_ERR(hwndDlg, HELP_PC2BADPASSWORD, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "The Lockup Password and its verification do not match! "
                "Please reenter - continuing... ");
            break;
            }
/*                                                                                      *\
 * If the current Envrionment Space has not been saved by turning to the first Environ- *
 * ment Space settings page, we try to add or change the current Environment Space.     *
\*                                                                                      */
                                        /* Turn to the first Environment Space settings page
                                           which causes a WM_CONTROL message (which enables/disables
                                           the DDPB_ADDENVIRONMENT/DDPB_CHANGEENVIRONMENT pushbuttons). */
        WinSendMsg(hwndNB, BKM_TURNTOPAGE, MPFROMLONG(nbDDNotebookPage[DD_PAGE_ENVIRON1].ulIDPage), NULL);
        if(WinIsWindowEnabled(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_ADDENVIRONMENT)))
            WinSendMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, WM_COMMAND,
                MPFROMSHORT(DDPB_ADDENVIRONMENT), MPFROM2SHORT(CMDSRC_PUSHBUTTON, TRUE));
        if(WinIsWindowEnabled(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_CHANGEENVIRONMENT)))
            WinSendMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, WM_COMMAND,
                MPFROMSHORT(DDPB_CHANGEENVIRONMENT), MPFROM2SHORT(CMDSRC_PUSHBUTTON, TRUE));
                                        /* Check for changes that should be backuped */
        pHP->ulConfigurationChanges+=1;
        CopySetupFiles(hwndDlg, FALSE);
        }
                                        /* If the dialog will be dismissed, remove the subclassing
                                           of the notebook control within this dialog */
    if((SHORT1FROMMP(mp1)==DID_OK) || (SHORT1FROMMP(mp1)==DID_CANCEL))
        {
                                        /* Inform notebook page dialog procedure that we're closing down */
        WinSendMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, WM_SETUPNOTEBOOKPAGES, MPFROMHWND(NULLHANDLE), (MPARAM)NULL);
        if((pHP->ulVersionMajor==OS2_MAJOR) && (pHP->ulVersionMinor<OS2_MINOR_300))
            {
                                        /* Workaround for OS/2 2.x bug, that causes that the MLE's
                                           return only 1 character per line they contain unless their
                                           notebook page was at least on top once. This is a 2.x defect,
                                           not a PC/2 bug! */
            ULONG       ulPage;

            for(ulPage=0; nbDDNotebookPage[ulPage].pDialogWndProc!=NULL; ulPage++)
                WinSendMsg(WinWindowFromID(hwndDlg, DDNB_NOTEBOOK), BKM_TURNTOPAGE, MPFROMLONG(nbDDNotebookPage[ulPage].ulIDPage), NULL);
            }
        WinSubclassWindow(hwndNB, DialogNotebookSubclass[DD_SUBCLASSEDNOTEBOOK].pfnwpNotebook);
        DialogNotebookSubclass[DD_SUBCLASSEDNOTEBOOK].hwndNotebook=NULLHANDLE;
        DialogNotebookSubclass[DD_SUBCLASSEDNOTEBOOK].pfnwpNotebook=0;
                                        /* Also hide dialog */
        WinShowWindow(hwndDlg, FALSE);
                                        /* Clear spy flag anyway */
        pHP->ulRuntimeFlag&=(~SPYFRAMEWINDOW);
        }
    switch(SHORT1FROMMP(mp1))
    {
    case DID_OK:                        /* Enter key pressed */
        {
        UCHAR       ucBuffer[5];
        ULONG       ulOldStatusFlag=pHP->ulStatusFlag1;

#ifdef  DEBUG_DIALOG
        printf("Dialog: %ld -> Processing DID_OK\n", __LINE__);
#endif  /* DEBUG_DIALOG */
/*                                                                                      *\
 * First notebook page (page 1 and page 2).                                             *
\*                                                                                      */
                                        /* Query checkboxes' state and set according flag */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_VIRTUALDESKTOP))
            pHP->ulStatusFlag1|=VIRTUALDESKTOP;
        else pHP->ulStatusFlag1&=~VIRTUALDESKTOP;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_FOLLOWFOCUS))
            pHP->ulStatusFlag1|=FOLLOWFOCUS;
        else pHP->ulStatusFlag1&=~FOLLOWFOCUS;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_CLICK2MOVE))
            pHP->ulStatusFlag1|=CLICK2MOVE;
        else pHP->ulStatusFlag1&=~CLICK2MOVE;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_AUTOSCROLL))
            pHP->ulStatusFlag1|=AUTOSCROLL;
        else pHP->ulStatusFlag1&=~AUTOSCROLL;
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDEF_DESKTOPDELAY),
            sizeof(ucBuffer), ucBuffer);
        pHP->ulVirtualDesktopDelay=((ULONG)atol(ucBuffer))/TIMERINTERVALL100MS;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_MOVEDESKTOP))
            pHP->ulStatusFlag1|=MOVEDESKTOP;
        else pHP->ulStatusFlag1&=~MOVEDESKTOP;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_EXPANDWPS))
            pHP->ulStatusFlag1|=EXPANDWPS;
        else pHP->ulStatusFlag1&=~EXPANDWPS;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_OVERVIEW))
            pHP->ulStatusFlag1|=OVERVIEW;
        else pHP->ulStatusFlag1&=~OVERVIEW;
                                        /* Query scroll percentage entryfield (0-based !!!) */
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDEF_SCROLLPERCENTAGE),
            sizeof(ucBuffer), ucBuffer);
        pHP->ulScrollPercentage=(ULONG)atol(ucBuffer);
                                        /* The scroll percentage will be reinitialized by posting
                                           WM_PC2STARTUPSCREEN */
/*                                                                                      *\
 * Get the count of Virtual Desktops, and adjust HookParameters control structure       *
 * accordingly by posting WM_PC2STARTUPSCREEN. This will allow dynamic change on the    *
 * Overview window and allows user operation borders. If the Virtual Desktop is         *
 * disabled, the number of Virtual Desktops is set to one, even if the (disabled) spin  *
 * buttons contain different values.                                                    *
\*                                                                                      */
                                        /* Get number of horizontal and vertical Virtual Desktops */
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDSB_HORIZONTALDESKTOPS,
            SPBM_QUERYVALUE, MPFROMP(&pHP->ulHorizontalDesktopsSave), MPFROM2SHORT(0, SPBQ_UPDATEIFVALID));
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDSB_VERTICALDESKTOPS,
            SPBM_QUERYVALUE, MPFROMP(&pHP->ulVerticalDesktopsSave), MPFROM2SHORT(0, SPBQ_UPDATEIFVALID));
                                        /* Cause a recalculation of the number of Virtual Desktops
                                           used (in contrast to what numbers were configured, to
                                           take a disabled Virtual Desktop into account) */
        WinPostMsg(pHP->hwndClient, WM_PC2STARTUPSCREEN, NULL, NULL);
/*                                                                                      *\
 * Popup Menu activation click flags.                                                   *
\*                                                                                      */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTONCLICK))
            {
            if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTON1))
                pHP->ulClickFlag=WM_BUTTON1CLICK;
            else
                pHP->ulClickFlag=WM_BUTTON3CLICK;
            }
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTONDBLCLK))
            {
            if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTON1))
                pHP->ulClickFlag=WM_BUTTON1DBLCLK;
            else
                pHP->ulClickFlag=WM_BUTTON3DBLCLK;
            }
                                        /* Query over any window flag */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_SYSTEMWIDE))
            pHP->ulStatusFlag1|=POPUPSYSTEMWIDE;
        else pHP->ulStatusFlag1&=~POPUPSYSTEMWIDE;
/*                                                                                      *\
 * Popup Menu menuentry selection flags (mouse).                                        *
\*                                                                                      */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_SELECTFIRSTITEM))
            pHP->ulStatusFlag1|=SELECTFIRSTITEM;
        else pHP->ulStatusFlag1&=~SELECTFIRSTITEM;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_SELECTCENTER))
            pHP->ulStatusFlag1|=SELECTCENTERITEM;
        else pHP->ulStatusFlag1&=~SELECTCENTERITEM;
/*                                                                                      *\
 * Popup Menu menuentry icons flags.                                                    *
\*                                                                                      */
                                        /* Query Popup-Menu icon checkbox and set the Popup-Menu's
                                           presentation parameters to the current one, as this
                                           causes WM_MEASUREITEM again, which allows to adjust
                                           the Popup-Menu for a probably different icon size
                                           requested. */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_POPUPICON))
            pHP->ulStatusFlag2|=POPUPICON;
        else pHP->ulStatusFlag2&=~POPUPICON;
        pHP->ulIconScale=0;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_FULLSIZE))
            pHP->ulIconScale=0;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_HALFSIZE))
            pHP->ulIconScale=1;
/*                                                                                      *\
 * Popup Menu menuentry selection flags (keyboard).                                     *
\*                                                                                      */
        {
        ULONG   ulPopupKey;
        ULONG   ulShiftBitmask;

        for(ulPopupKey=DDCB_LEFTRIGHTSHIFT, ulShiftBitmask=0x00000001;
            ulPopupKey<=DDCB_SCROLLLOCK;
            ulPopupKey++, ulShiftBitmask<<=1)
            {
            if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, ulPopupKey))
                pHP->ulPopupKeyFlag|=ulShiftBitmask;
            else pHP->ulPopupKeyFlag&=~ulShiftBitmask;
            }
        }
/*                                                                                      *\
 * Sliding Focus flags.                                                                 *
\*                                                                                      */
                                        /* Query focus flag */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_SLIDINGFOCUS))
            pHP->ulStatusFlag1|=SLIDINGFOCUS;
        else pHP->ulStatusFlag1&=~SLIDINGFOCUS;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_PRESERVEZORDER))
            pHP->ulStatusFlag1|=PRESERVEZORDER;
        else pHP->ulStatusFlag1&=~PRESERVEZORDER;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_ACTIVATEWINDOW))
            pHP->ulStatusFlag1|=FOCUSWINDOWLEVEL;
        else pHP->ulStatusFlag1&=~FOCUSWINDOWLEVEL;
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDEF_SLIDINGDELAY),
            sizeof(ucBuffer), ucBuffer);
        pHP->ulSlidingFocusDelay=((ULONG)atol(ucBuffer))/TIMERINTERVALL100MS;
/*                                                                                      *\
 * Hardware assisted panning (Virtual Desktop).                                         *
\*                                                                                      */
                                        /* Query HW Panning flags */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_HWVIRTUALDESKTOP))
            pHP->ulStatusFlag2|=HWVIRTUALDESKTOP;
        else pHP->ulStatusFlag2&=~HWVIRTUALDESKTOP;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDRB_640X480))
            { pHP->ulHWSizeX=640; pHP->ulHWSizeY=480; }
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDRB_800X600))
            { pHP->ulHWSizeX=800; pHP->ulHWSizeY=600; }
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDRB_1024X768))
            { pHP->ulHWSizeX=1024; pHP->ulHWSizeY=768; }
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDRB_1280X1024))
            { pHP->ulHWSizeX=1280; pHP->ulHWSizeY=1024; }
        WinQueryDlgItemText(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDMLE_PANNINGEXCEPTION,
            sizeof(pHP->ucHWVirtualDesktopException), pHP->ucHWVirtualDesktopException);
        StripTrailing(pHP->ucHWVirtualDesktopException);
        strcat(pHP->ucHWVirtualDesktopException, "\r\n");
/*                                                                                      *\
 * Second notebook page.                                                                *
\*                                                                                      */
/*                                                                                      *\
 * Add or remove Titlebar + Hide Button or Smart icon bar according to user request.    *
\*                                                                                      */
                                        /* Query Overview window flag */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_HIDETITLEBAR))
            pHP->ulStatusFlag1|=HIDETITLEBAR;
        else pHP->ulStatusFlag1&=~HIDETITLEBAR;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_HIDESMARTICONBAR))
            pHP->ulStatusFlag1|=HIDESMARTICONBAR;
        else pHP->ulStatusFlag1&=~HIDESMARTICONBAR;
        {
        ULONG   ulChangedFCF;

        ulChangedFCF=0;
                                        /* If titlebar is set but should be hidden, update
                                           its window, which is a child of the frame */
        if( (!!(pHP->ulOverviewFCF & (FCF_TITLEBAR|FCF_HIDEBUTTON)))==
            (!!(pHP->ulStatusFlag1 & HIDETITLEBAR)) )
            ulChangedFCF|=FCF_TITLEBAR|FCF_HIDEBUTTON;
                                        /* If menubar is set but should be hidden, update
                                           its window */
        if( (!!(pHP->ulOverviewFCF & FCF_MENU))==
            (!!(pHP->ulStatusFlag1 & HIDESMARTICONBAR)) )
            ulChangedFCF|=FCF_MENU;
                                        /* Now update Overview window, including enabling the
                                           exit menuentry */
        if(ulChangedFCF)
            WinPostMsg(pHP->hwndClient, WM_OVERVIEWFCF, MPFROMLONG(ulChangedFCF), MPFROMLONG(TRUE));
                                        /* Query the Dynamic Overview window flags */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_DYNAMICOVERVIEW))
            pHP->ulStatusFlag2|=DYNAMICOVERVIEW;
        else pHP->ulStatusFlag2&=~DYNAMICOVERVIEW;
        pHP->ulDynamicOverview=0;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWUL))
            pHP->ulDynamicOverview|=OVERVIEW_UL;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWUR))
            pHP->ulDynamicOverview|=OVERVIEW_UR;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWLR))
            pHP->ulDynamicOverview|=OVERVIEW_LR;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWLL))
            pHP->ulDynamicOverview|=OVERVIEW_LL;
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDEF_OVERVIEWDELAY),
            sizeof(ucBuffer), ucBuffer);
        pHP->ulOverviewDelay=((ULONG)atol(ucBuffer))/TIMERINTERVALL100MS;
                                        /* Query Overview options */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_TARGETCURSOR))
            pHP->ulStatusFlag1|=OVERVIEWCURSOR;
        else pHP->ulStatusFlag1&=~OVERVIEWCURSOR;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_KEEPONTOPOV))
            pHP->ulStatusFlag1|=KEEPONTOPOV;
        else pHP->ulStatusFlag1&=~KEEPONTOPOV;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_ONECLICKSWITCH))
            pHP->ulStatusFlag1|=ONECLICKSWITCH;
        else pHP->ulStatusFlag1&=~ONECLICKSWITCH;
        }
/*                                                                                      *\
 * Query overview window style                                                          *
\*                                                                                      */
                                        /* Query draw mode flags */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_3DBORDER))
            pHP->ulStatusFlag1|=OVERVIEW3DBORDER;
        else pHP->ulStatusFlag1&=~OVERVIEW3DBORDER;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_OVERVIEWTITLES))
            pHP->ulStatusFlag1|=OVERVIEWTITLES;
        else pHP->ulStatusFlag1&=~OVERVIEWTITLES;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_TITLEAUTOCOLOR))
            pHP->ulStatusFlag1|=OVERVIEWAUTOCOLOR;
        else pHP->ulStatusFlag1&=~OVERVIEWAUTOCOLOR;
/*                                                                                      *\
 * Query OverView Window representation                                                 *
\*                                                                                      */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWICONS))
            pHP->ulOverviewWindowDrawMode=OVERVIEW_ICONS;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWGRADIENT))
            pHP->ulOverviewWindowDrawMode=OVERVIEW_GRADIENT;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWRANDOM))
            pHP->ulOverviewWindowDrawMode=OVERVIEW_RANDOM;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWMONO))
            pHP->ulOverviewWindowDrawMode=OVERVIEW_MONOCHROME;
/*                                                                                      *\
 * Query Sticky Windows                                                                 *
\*                                                                                      */
        WinQueryDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDMLE_STICKYWINDOWS,
            sizeof(pHP->ucStickyWindows), pHP->ucStickyWindows);
        StripTrailing(pHP->ucStickyWindows);
        strcat(pHP->ucStickyWindows, "\r\n");
/*                                                                                      *\
 * Overview window flags.                                                               *
\*                                                                                      */
        {                               /* Show or hide overview window according to
                                           the user selected flag */
        SWP     swp;

        swp.x=swp.y=swp.cx=swp.cy=0;
        swp.hwnd=pHP->hwndFrame;
        if(pHP->ulStatusFlag1 & OVERVIEW)
            {
            swp.fl=SWP_SHOW|SWP_DEACTIVATE;
            swp.hwndInsertBehind=HWND_BOTTOM;
            WinSetMultWindowPos(pHP->habPc2, &swp, 1);
            }
        else
            {
            swp.fl=SWP_HIDE|SWP_DEACTIVATE;
            swp.hwndInsertBehind=HWND_BOTTOM;
            WinSetMultWindowPos(pHP->habPc2, &swp, 1);
            }
        }
/*                                                                                      *\
 * Third notebook page.                                                                 *
\*                                                                                      */
/*                                                                                      *\
 * SessionBar flags.                                                                    *
\*                                                                                      */
                                        /* Query SessionBar flags */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_SESSIONBAR))
            pHP->ulStatusFlag2|=SHOWSESSIONBAR;
        else pHP->ulStatusFlag2&=~SHOWSESSIONBAR;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_KEEPONTOPSB))
            pHP->ulStatusFlag2|=KEEPONTOPSB;
        else pHP->ulStatusFlag2&=~KEEPONTOPSB;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_NOSESSIONNAME))
            pHP->ulStatusFlag2|=NOSESSIONNAMESB;
        else pHP->ulStatusFlag2&=~NOSESSIONNAMESB;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_DEFAULTDATETIME))
            pHP->ulStatusFlag2|=DEFAULTDATETIMESB;
        else pHP->ulStatusFlag2&=~DEFAULTDATETIMESB;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_NODATETIME))
            pHP->ulStatusFlag2|=NODATETIMESB;
        else pHP->ulStatusFlag2&=~NODATETIMESB;
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDEF_SESSIONBARDATETIME),
            sizeof(pHP->ucSessionBarDateTime), pHP->ucSessionBarDateTime);
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDRB_SESSIONBARLARGE))
            pHP->ulSessionBarScale=0;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDRB_SESSIONBARSMALL))
            pHP->ulSessionBarScale=1;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_SESSIONBARHELP))
            pHP->ulStatusFlag2|=BUBBLEHELPSB;
        else pHP->ulStatusFlag2&=~BUBBLEHELPSB;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_DYNAMICSESSIONBAR))
            pHP->ulStatusFlag2|=DYNAMICSESSIONBAR;
        else
            pHP->ulStatusFlag2&=~DYNAMICSESSIONBAR;
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDEF_SESSIONBARDELAY),
            sizeof(ucBuffer), ucBuffer);
        pHP->ulSessionBarDelay=((ULONG)atol(ucBuffer))/TIMERINTERVALL100MS;
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDSB_SESSIONBARHOTSPOT,
            SPBM_QUERYVALUE, MPFROMP(&pHP->ulSessionBarHotSpot), MPFROM2SHORT(0, SPBQ_UPDATEIFVALID));
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_CPUPERFORMANCE))
            pHP->ulStatusFlag2|=CPUPERFORMANCESB;
        else
            pHP->ulStatusFlag2&=~CPUPERFORMANCESB;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_PERFORMANCEFILL))
            pHP->ulStatusFlag2|=PERFORMANCEFILLSB;
        else
            pHP->ulStatusFlag2&=~PERFORMANCEFILLSB;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_NOGRID))
            pHP->ulStatusFlag2|=NOGRIDSB;
        else
            pHP->ulStatusFlag2&=~NOGRIDSB;
        WinQueryDlgItemText(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDMLE_SESSIONBAREXCEPTION,
            sizeof(pHP->ucSessionBarException), pHP->ucSessionBarException);
        StripTrailing(pHP->ucSessionBarException);
        strcat(pHP->ucSessionBarException, "\r\n");
                                        /* QuickSwitch flags */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_ENABLEQUICKSWITCH))
            pHP->ulStatusFlag2|=ENABLEQUICKSWITCH;
        else
            pHP->ulStatusFlag2&=~ENABLEQUICKSWITCH;
                                        /* Inform SessionBar to take any updates into account */
        if(pHP->hwndSessionClient)
            {
            WinSendMsg(pHP->hwndSessionClient, WM_SESSIONBAR, NULL, NULL);
            WinInvalidateRect(pHP->hwndSessionClient, NULL, FALSE);
            }
/*                                                                                      *\
 * Fourth notebook page.                                                                *
\*                                                                                      */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDCB_BUTTON2ZORDER))
            pHP->ulStatusFlag1|=BUTTON2ZORDER;
        else pHP->ulStatusFlag1&=~BUTTON2ZORDER;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDCB_BUTTON2MOVE))
            pHP->ulStatusFlag1|=BUTTON2MOVE;
        else pHP->ulStatusFlag1&=~BUTTON2MOVE;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDCB_USESPOOLER))
            pHP->ulStatusFlag1|=SHOWSPOOLERWINDOW;
        else pHP->ulStatusFlag1&=~SHOWSPOOLERWINDOW;
                                        /* Query Desktop and Window List name */
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_DESKTOPNAME),
            sizeof(pHP->ucDesktopName), pHP->ucDesktopName);
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_WINDOWLISTNAME),
            sizeof(pHP->ucWindowListName), pHP->ucWindowListName);
/*                                                                                      *\
 * Get PM background bitmap settings.                                                   *
\*                                                                                      */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDCB_BACKGROUNDBITMAP))
            pHP->ulStatusFlag1|=BACKGROUNDBITMAP;
        else pHP->ulStatusFlag1&=~BACKGROUNDBITMAP;
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_BACKGROUNDBITMAP),
            sizeof(pHP->ucBackgroundBitmap), pHP->ucBackgroundBitmap);
                                        /* Get number of tiled bitmaps */
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDSB_BITMAPTILEDCOUNT,
            SPBM_QUERYVALUE, MPFROMP(&pHP->ulBackgroundBitmapFlag), MPFROM2SHORT(0, SPBQ_UPDATEIFVALID));
                                        /* Query background bitmap style into upper USHORT */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDRB_BITMAPNORMAL))
            pHP->ulBackgroundBitmapFlag|=BITMAPNORMAL;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDRB_BITMAPSCALED))
            pHP->ulBackgroundBitmapFlag|=BITMAPSCALED;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDRB_BITMAPTILED))
            pHP->ulBackgroundBitmapFlag|=BITMAPTILED;
/*                                                                                      *\
 * Fifth notebook page.                                                                 *
\*                                                                                      */
/*                                                                                      *\
 * Get general environment settings flag.                                               *
\*                                                                                      */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_IGNOREAUTOSTART))
            pHP->ulStatusFlag1|=IGNOREAUTOSTART;
        else pHP->ulStatusFlag1&=~IGNOREAUTOSTART;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_IGNORESCHEDULER))
            pHP->ulStatusFlag1|=IGNORESCHEDULER;
        else pHP->ulStatusFlag1&=~IGNORESCHEDULER;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_HOTKEY4PC2ONLY))
            pHP->ulStatusFlag1|=HOTKEY4PC2ONLY;
        else pHP->ulStatusFlag1&=~HOTKEY4PC2ONLY;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ENABLEDIAGNOSTICS))
            pHP->ulStatusFlag2|=ENABLEDIAGNOSTICS;
        else pHP->ulStatusFlag2&=~ENABLEDIAGNOSTICS;
/*                                                                                      *\
 * ShutDown Option settings.                                                            *
\*                                                                                      */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_NORMALSHUTDOWN))
            pHP->ulStatusFlag1|=NORMALSHUTDOWN;
        else pHP->ulStatusFlag1&=~NORMALSHUTDOWN;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_NOVERIFYSHUTDOWN))
            pHP->ulStatusFlag1|=NOVERIFYSHUTDOWN;
        else pHP->ulStatusFlag1&=~NOVERIFYSHUTDOWN;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_SHUTDOWNREBOOT))
            pHP->ulStatusFlag1|=SHUTDOWNREBOOT;
        else pHP->ulStatusFlag1&=~SHUTDOWNREBOOT;
/*                                                                                      *\
 * PowerDown Option settings.                                                           *
\*                                                                                      */
        {
        ULONG   ulStatusFlagPrevious=pHP->ulStatusFlag2;

                                        /* APM support flag */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_SUPPORTAPM))
            pHP->ulStatusFlag2|=SUPPORTAPM;
        else pHP->ulStatusFlag2&=~SUPPORTAPM;
                                        /* APM enable/disable flag. If the flag has been
                                           toggled, then request working thread to toggle
                                           APM status via APM.SYS (if APM support is enabled) */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ENABLEAPM))
            pHP->ulStatusFlag2|=ENABLEAPM;
        else pHP->ulStatusFlag2&=~ENABLEAPM;
        if((ulStatusFlagPrevious & SUPPORTAPM) &&
            (ulStatusFlagPrevious & ENABLEAPM)!=(pHP->ulStatusFlag2 & ENABLEAPM))
            WinPostMsg(pHP->hwndWorkingThread, WM_APM, MPFROMLONG(TRUE), NULL);
        }
                                        /* PowerDown modes */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_NOVERIFYPOWERDOWN))
            pHP->ulStatusFlag2|=NOVERIFYPOWERDOWN;
        else pHP->ulStatusFlag2&=~NOVERIFYPOWERDOWN;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDRB_POWERDOWNSUSPEND))
            pHP->ulStatusFlag2|=POWERDOWNSUSPEND;
        else pHP->ulStatusFlag2&=~POWERDOWNSUSPEND;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDRB_POWERDOWNPOWEROFF))
            pHP->ulStatusFlag2|=POWERDOWNPOWEROFF;
        else pHP->ulStatusFlag2&=~POWERDOWNPOWEROFF;
                                        /* Enable PowerDown timer checkbox */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ENABLEPOWERDOWN))
            pHP->ulStatusFlag2|=ENABLEPOWERDOWN;
        else pHP->ulStatusFlag2&=~ENABLEPOWERDOWN;
                                        /* Get lockup timer count */
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDSB_POWERDOWNTIMER,
            SPBM_QUERYVALUE, MPFROMP(&pHP->ulPowerDownTimer), MPFROM2SHORT(0, SPBQ_UPDATEIFVALID));
/*                                                                                      *\
 * Advanced marking settings.                                                           *
\*                                                                                      */
        pHP->ulStatusFlag2&=~(ADVANCEDMARKING | ADVMARKINGBUTTON2 | ADVMARKINGBUTTON3);
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ADVANCEDMARKING))
            {
            pHP->ulStatusFlag2|=ADVANCEDMARKING;
            if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDRB_ADVMARKINGBUTTON3))
                pHP->ulStatusFlag2|=ADVMARKINGBUTTON3;
            else
                pHP->ulStatusFlag2|=ADVMARKINGBUTTON2;
            }
/*                                                                                      *\
 * Dynamic menu settings.                                                               *
\*                                                                                      */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_DYNAMICMENU))
            pHP->ulStatusFlag2|=DYNAMICMENU;
        else pHP->ulStatusFlag2&=~DYNAMICMENU;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_CASCADEMENU))
            pHP->ulStatusFlag2|=CASCADEMENU;
        else pHP->ulStatusFlag2&=~CASCADEMENU;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_DYNAMICMENUKBD))
            pHP->ulStatusFlag2|=DYNAMICMENUKBD;
        else pHP->ulStatusFlag2&=~DYNAMICMENUKBD;
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDEF_DYNAMICDELAY),
            sizeof(ucBuffer), ucBuffer);
        pHP->ulDynamicMenuDelay=((ULONG)atol(ucBuffer))/TIMERINTERVALL100MS;
/*                                                                                      *\
 * Sixth notebook page.                                                                 *
\*                                                                                      */
/*                                                                                      *\
 * Titlebar Smarticons setting. The Exception List must be terminated by \r\n.          *
\*                                                                                      */
        {
        ULONG   ulTitlebarSmarticon;
        ULONG   ulShiftBitmask;

        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_TITLEBAR].hwndNBPage, DDCB_TITLEBARICONS))
            pHP->ulStatusFlag2|=TITLEBARICONS;
        else pHP->ulStatusFlag2&=~TITLEBARICONS;
        for(ulTitlebarSmarticon=DDCB_CLOSEWINDOW, ulShiftBitmask=0x00000001;
            ulTitlebarSmarticon<=DDCB_MOVEEDGE;
            ulTitlebarSmarticon++, ulShiftBitmask<<=1)
            {
            if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_TITLEBAR].hwndNBPage, ulTitlebarSmarticon))
                pHP->ulTitlebarFlag|=ulShiftBitmask;
            else pHP->ulTitlebarFlag&=~ulShiftBitmask;
            }
        }
        WinQueryDlgItemText(nbDDNotebookPage[DD_PAGE_TITLEBAR].hwndNBPage, DDMLE_TITLEBAREXCEPTION,
            sizeof(pHP->ucTitlebarException), pHP->ucTitlebarException);
        StripTrailing(pHP->ucTitlebarException);
        strcat(pHP->ucTitlebarException, "\r\n");
/*                                                                                      *\
 * Seventh notebook page.                                                               *
\*                                                                                      */
        {
        LONG    lTemp;

                                        /* Encrypt entered password */
        strcpy(pHP->ucAccessPassword, ucDDAccessPassword);
        for(lTemp=strlen(pHP->ucAccessPassword)-1; lTemp>=0; lTemp--)
            pHP->ucAccessPassword[lTemp]=(pHP->ucAccessPassword[lTemp]<<1)^0x3C;
        strcpy(pHP->ucLockupPassword, ucDDLockupPassword);
        for(lTemp=strlen(pHP->ucLockupPassword)-1; lTemp>=0; lTemp--)
            pHP->ucLockupPassword[lTemp]=(pHP->ucLockupPassword[lTemp]<<1)^0x3C;
                                        /* PC2 Security checkbox */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDCB_PC2SECURITY))
            pHP->ulStatusFlag2|=PC2SECURITY;
        else pHP->ulStatusFlag2&=~PC2SECURITY;
                                        /* Enable Lockup Timer checkbox */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDCB_ENABLELOCKUP))
            pHP->ulStatusFlag2|=ENABLELOCKUP;
        else pHP->ulStatusFlag2&=~ENABLELOCKUP;
                                        /* Lockup after boot checkbox */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDCB_LOCKUPONBOOT))
            pHP->ulStatusFlag2|=LOCKUPONBOOT;
        else pHP->ulStatusFlag2&=~LOCKUPONBOOT;
                                        /* Get lockup timer count */
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDSB_LOCKUPTIMER,
            SPBM_QUERYVALUE, MPFROMP(&pHP->ulLockupTimer), MPFROM2SHORT(0, SPBQ_UPDATEIFVALID));
        }
/*                                                                                      *\
 * Eigth notebook page.                                                                 *
\*                                                                                      */
/*                                                                                      *\
 * Nineth notebook page.                                                                *
\*                                                                                      */
        {
        LONG    lTemp;

                                        /* Online registration flags */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDCB_AUTOUPDATEENABLE))
            pHP->ulRegistrationFlag|=REGISTER_ONLINE;
        else pHP->ulRegistrationFlag&=~REGISTER_ONLINE;
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDCB_SENDREGISTRATION))
            pHP->ulRegistrationFlag|=REGISTER_USER;
        else pHP->ulRegistrationFlag&=~REGISTER_USER;
                                        /* Retrieve user information */
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_FIRSTNAME),
            sizeof(pHP->ucUserFirstName), pHP->ucUserFirstName);
                                        /* Retrieve user information */
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_LASTNAME),
            sizeof(pHP->ucUserLastName), pHP->ucUserLastName);
                                        /* Retrieve user information */
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_EMAILADDRESS),
            sizeof(pHP->ucUserEmailAddress), pHP->ucUserEmailAddress);
                                        /* Retrieve Proxy URL */
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_PROXYURL),
            sizeof(pHP->ucProxyUrl), pHP->ucProxyUrl);
                                        /* Retrieve Update URL */
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_UPDATEURL),
            sizeof(pHP->ucUpdateUrl), pHP->ucUpdateUrl);
                                        /* Retrieve HTTP Userid and Password information */
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_HTTPUSERID),
            sizeof(pHP->ucUserHttpUserid), pHP->ucUserHttpUserid);
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_HTTPPASSWORD),
            sizeof(ucDDUserHttpPassword), ucDDUserHttpPassword);
        for(lTemp=strlen(ucDDUserHttpPassword)-1; lTemp>=0; lTemp--)
            ucDDUserHttpPassword[lTemp]=(ucDDUserHttpPassword[lTemp]<<1)^0x3C;
        strcpy(pHP->ucUserHttpPassword, ucDDUserHttpPassword);
                                        /* Check status */
        if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDCB_NETWORKSTATUS))
            pHP->ulRegistrationFlag|=REGISTER_SHOWSTATUS;
        else pHP->ulRegistrationFlag&=~REGISTER_SHOWSTATUS;
        }
/*                                                                                      *\
 * Tenth notebook page.                                                                 *
\*                                                                                      */
/*                                                                                      *\
 * Update INI file, environment profile and resize WPS when necessary.                  *
\*                                                                                      */
                                        /* Write changes to PC2.INI */
        if(!INIAccess(pHP->ucFilenameINI, FALSE))
            USR_ERR(pHP->hwndFrame, HELP_PC2INI, MB_INFORMATION|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
                "Can't save configuration data into PC2.INI - continuing... ");
                                        /* Size WPS according to expand WPS flag */
        if(pHP->ulStatusFlag1 & EXPANDWPS)
            WinPostMsg(pHP->hwndWorkingThread, WM_EXPANDWPS, (MPARAM)TRUE, NULL);
                                        /* If WPS was expanded before we must reduce it,
                                           if WPS is not the same as before */
        if((ulOldStatusFlag & EXPANDWPS) && !(pHP->ulStatusFlag1 & EXPANDWPS))
            WinPostMsg(pHP->hwndWorkingThread, WM_EXPANDWPS, (MPARAM)FALSE, NULL);
                                        /* Inform working thread to change PM background bitmap */
        WinPostMsg(pHP->hwndWorkingThread, WM_BACKGROUNDBITMAP, NULL, NULL);
                                        /* Save environment space profile */
        WinPostMsg(pHP->hwndClient, WM_SAVEENVIRONMENTSPACE, MPFROMLONG(ID_MAINTHREAD), NULL);
                                        /* Dialog terminated with DID_OK */
        WinDismissDlg(hwndDlg, DID_OK);
#ifdef  DEBUG_DIALOG
        printf("Dialog: %ld <- Processing DID_OK\n", __LINE__);
#endif  /* DEBUG_DIALOG */
        }
        break;

    case DID_CANCEL:                    /* Cancel key pressed */
                                        /* Dialog terminated with DID_CANCEL */
        WinDismissDlg(hwndDlg, DID_CANCEL);
        break;

    case DID_BACKUP:                    /* Backup pressed */
        CopySetupFiles(hwndDlg, TRUE);
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Desktop Settings dialog notebook pages.     *
 * Req:                                                                                 *
 *      nbDDNotebookPage[] ...  Structure of the Desktop Settings notebook pages        *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY DD_NotebookProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Notebook where this window procedure implements notebook pages */
static HWND     hwndNotebook=NULLHANDLE;
                                        /* Last OVEVIEW_* radiobutton selected */
static ULONG    ulOverviewWindowDrawMode=(ULONG)-1;
                                        /* Child windows that change with the DDCB_SLIDINGFOCUS checkbox */
static USHORT   usChild2SlidingFocusCB[]={DDCB_PRESERVEZORDER, DDCB_ACTIVATEWINDOW,
                                          DDHSB_SLIDINGDELAY,  DDEF_SLIDINGDELAY};
                                        /* Child window that change with DDCB_DYNAMICOVERVIEW checkbox */
static USHORT   usChild2DynamicOverviewCB[]={DDRB_OVERVIEWUL, DDRB_OVERVIEWUR,
                                             DDRB_OVERVIEWLL, DDRB_OVERVIEWLR,
                                             DDEF_OVERVIEWDELAY, DDHSB_OVERVIEWDELAY};
                                        /* Child windows that change with the DDCB_SESSIONBAR checkbox */
static USHORT   usChild2SessionBarCB[]={DDCB_KEEPONTOPSB, DDCB_NOSESSIONNAME,
                                        DDRB_SESSIONBARLARGE, DDRB_SESSIONBARSMALL,
                                        DDCB_SESSIONBARHELP,
                                        DDCB_DYNAMICSESSIONBAR, DDCB_CPUPERFORMANCE};
                                        /* Child windows that change with DDCB_NOSESSIONNAME checkbox */
static USHORT   usChild2SessionNameCB[]={DDCB_DEFAULTDATETIME, DDCB_NODATETIME, DDEF_SESSIONBARDATETIME};
static USHORT   usChild2DynamicSessionBarCB[]={DDHSB_SESSIONBARDELAY, DDEF_SESSIONBARDELAY,
                                               DDSB_SESSIONBARHOTSPOT};
                                        /* Child windows that change with DDCB_CPUPERFORMANCE checkbox */
static USHORT   usChild2CpuPerformanceCB[]={DDCB_PERFORMANCEFILL, DDCB_NOGRID};
                                        /* Child windows that change with the DDCB_HWVIRTUALDESKTOP checkbox */
static USHORT   usChild2HWVirtualDesktopCB[]={DDRB_640X480, DDRB_800X600, DDRB_1024X768, DDRB_1280X1024 };
                                        /* Child windows that change with the DDCB_VIRTUALDESKTOP checkbox */
static USHORT   usChild2VirtualDesktopCB[]={DDCB_FOLLOWFOCUS, DDCB_CLICK2MOVE, DDCB_AUTOSCROLL,
                                            DDCB_MOVEDESKTOP, DDCB_EXPANDWPS,
                                            DDEF_SCROLLPERCENTAGE, DDSB_HORIZONTALDESKTOPS,
                                            DDSB_VERTICALDESKTOPS};
                                        /* Child window that change with DDCB_MOVEDESKTOP checkbox */
static USHORT   usChild2Click2MoveCB[]={DDCB_AUTOSCROLL,
                                        DDHSB_DESKTOPDELAY, DDEF_DESKTOPDELAY};
                                        /* Child window that change with DDCB_MOVEDESKTOP checkbox */
static USHORT   usChild2MoveDesktopCB[]={DDCB_EXPANDWPS};
                                        /* Child windows that change with DDCB_POPUPICON */
static USHORT   usChild2PopupIconCB[]={DDRB_FULLSIZE, DDRB_HALFSIZE};
                                        /* Child windows that change with DDCB_BACKGROUNDBITMAP */
static USHORT   usChild2BackgroundBitmapCB[]={DDEF_BACKGROUNDBITMAP, DDPB_WINFILEDLG, DDRB_BITMAPNORMAL,
                                              DDRB_BITMAPSCALED, DDRB_BITMAPTILED, DDSB_BITMAPTILEDCOUNT};
                                        /* Child windows that change with DDCB_BITMAPTILED */
static USHORT   usChild2BitmapTiledCB[]={DDSB_BITMAPTILEDCOUNT};
                                        /* Child windows that change with DDCB_NORMALSHUTDOWN */
static USHORT   usChild2NormalShutdownCB[]={DDCB_SHUTDOWNREBOOT};
                                        /* Child windows that change with DDCB_ADVANCEDMARKING */
static USHORT   usChild2AdvancedMarkingCB[]={DDRB_ADVMARKINGBUTTON2, DDRB_ADVMARKINGBUTTON3};
                                        /* Child windows that change with DDCB_DYNAMICMENU */
static USHORT   usChild2DynamicMenuCB[]={DDCB_CASCADEMENU, DDCB_DYNAMICMENUKBD,
                                         DDHSB_DYNAMICDELAY, DDEF_DYNAMICDELAY};
                                        /* Child windows that change with DDCB_SUPPORTAPM */
static USHORT   usChild2APMCB[]={DDCB_ENABLEAPM, DDCB_NOVERIFYPOWERDOWN, DDRB_POWERDOWNSUSPEND,
                                 DDRB_POWERDOWNPOWEROFF, DDCB_ENABLEPOWERDOWN, DDSB_POWERDOWNTIMER};
                                        /* Child windows that change with DDCB_ENABLEPOWERDOWN */
static USHORT   usChild2PowerDownTimerCB[]={DDSB_POWERDOWNTIMER};
                                        /* Child windows that change with DDCB_ENABLELOCKUP */
static USHORT   usChild2LockupTimerCB[]={DDSB_LOCKUPTIMER};
                                        /* Child windows of DDLB_ENVIRONEMNT */
static USHORT   usChild2EnvironmentLB[]={DDPB_ADDENVIRONMENT, DDPB_CHANGEENVIRONMENT, DDPB_REMOVEENVIRONMENT};
                                        /* Child windows of DDCB_TITLEBARICONS */
static USHORT   usChild2TitlebarIconsCB[]={DDCB_CLOSEWINDOW, DDCB_ROLLWINDOW, DDCB_HELP,
                                           DDCB_MOVEVD, DDCB_MOVEEDGE};
                                        /* Child windows of Registration status text */
static USHORT   usChild2UpdateNowST[]={DDPB_ACCESSNET};

                                        /* During WinLoadDialog(), PM sends curious WM_CONTROL
                                           messages which causes us troubles as that message
                                           may have consequences on not yet loaded notebook
                                           pages, symptoms were traps in WinWindowFromID()
                                           during DisableDialogItem() */
if((msg!=WM_SETUPNOTEBOOKPAGES) && (hwndNotebook==NULLHANDLE))
    {
#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld Msg %04lX sent while notebook still initializing\n", __LINE__, msg);
#endif  /* DEBUG_DIALOG */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
switch(msg)
{
/*                                                                                      *\
 * Syntax: WM_SETUPNOTEBOOKPAGES, HWND hwndNotebook, DESKTOPDIALOGPARAMS *pDDParams     *                                           *
\*                                                                                      */
case WM_SETUPNOTEBOOKPAGES:
    {
    UCHAR               ucBuffer[5];
    DESKTOPDIALOGPARAMS *pDDParams=(DESKTOPDIALOGPARAMS *)mp2;

#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld -> WM_SETUPNOTEBOOKPAGES\n", __LINE__);
#endif  /* DEBUG_DIALOG */
    hwndNotebook=HWNDFROMMP(mp1);       /* Save notebook we're pages in */
    if(hwndNotebook==NULLHANDLE)
        break;
/*                                                                                      *\
 * Check the checkboxes that should be set according to the bitmap currently used in    *
 * PC2.INI. If the virtual Desktop is disabled, then its child checkboxes can't be set, *
 * because unchecking the virtual Desktop automatically unchecks its child checkboxes.  *
\*                                                                                      */
/*                                                                                      *\
 * First notebook page (page 1 and page 2).                                              *
\*                                                                                      */
                                        /* Check checkbox if Virtual Desktop flag is set */
    if(pHP->ulStatusFlag1 & VIRTUALDESKTOP)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_VIRTUALDESKTOP, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if Follow Focus flag is set */
    if(pHP->ulStatusFlag1 & FOLLOWFOCUS)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_FOLLOWFOCUS, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if click to move flag is set. If set
                                           disable autoscroll checkbox, else it may be set */
    if(pHP->ulStatusFlag1 & CLICK2MOVE)
        {
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_CLICK2MOVE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, usChild2Click2MoveCB,
            sizeof(usChild2Click2MoveCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
        }
                                        /* Check checkbox if autoscroll flag is set */
    if(pHP->ulStatusFlag1 & AUTOSCROLL)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_AUTOSCROLL, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Setup delay scrollbar between 0 and 2500 ms in 100 ms steps */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDHSB_DESKTOPDELAY,
        SBM_SETTHUMBSIZE, MPFROM2SHORT(1, 25), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDHSB_DESKTOPDELAY,
        SBM_SETSCROLLBAR, MPFROMSHORT(pHP->ulVirtualDesktopDelay),
        MPFROM2SHORT(0, 25));
                                        /* Insert the string */
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDEF_DESKTOPDELAY,
        (CHAR *)_itoa(pHP->ulVirtualDesktopDelay*TIMERINTERVALL100MS, ucBuffer, 10));
                                        /* Check checkbox if move Desktop flag is set. If set
                                           disable expand WPS checkbox, else it may be set */
    if(pHP->ulStatusFlag1 & MOVEDESKTOP)
        {
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_MOVEDESKTOP, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, usChild2MoveDesktopCB,
            sizeof(usChild2MoveDesktopCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
        }
                                        /* Check checkbox if expand WPS flag is set */
    if(pHP->ulStatusFlag1 & EXPANDWPS)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_EXPANDWPS, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if Overview window flag is set, disable
                                           Overview monochrome if not */
    if(pHP->ulStatusFlag1 & OVERVIEW)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_OVERVIEW, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
/*                                                                                      *\
 * Set scrolling percentage entryfield and horizontal scrollbar in 5 % increments.      *
\*                                                                                      */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDHSB_SCROLLPERCENTAGE,
        SBM_SETTHUMBSIZE, MPFROM2SHORT(1, 20), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDHSB_SCROLLPERCENTAGE,
        SBM_SETSCROLLBAR, MPFROMSHORT(pHP->ulScrollPercentage/5),
        MPFROM2SHORT(0, 19));
                                        /* Insert the string */
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDEF_SCROLLPERCENTAGE,
        (CHAR *)_itoa(pHP->ulScrollPercentage, ucBuffer, 10));
/*                                                                                      *\
 * Set horizontal and vertical count of Virtual Desktops into spinbuttons after         *
 * initialising them into the range of 1 to 9.                                          *
\*                                                                                      */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDSB_HORIZONTALDESKTOPS,
        SPBM_SETLIMITS, MPFROMLONG(9), MPFROMLONG(1));
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDSB_HORIZONTALDESKTOPS,
        SPBM_SETCURRENTVALUE, MPFROMLONG(pHP->ulHorizontalDesktopsSave), (MPARAM)0);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDSB_VERTICALDESKTOPS,
        SPBM_SETLIMITS, MPFROMLONG(9), MPFROMLONG(1));
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDSB_VERTICALDESKTOPS,
        SPBM_SETCURRENTVALUE, MPFROMLONG(pHP->ulVerticalDesktopsSave), (MPARAM)0);
/*                                                                                      *\
 * Disable the child windows of the virtual Desktop, if this checkbox isn't checked.    *
\*                                                                                      */
    if(!(pHP->ulStatusFlag1 & VIRTUALDESKTOP))
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, usChild2VirtualDesktopCB,
            sizeof(usChild2VirtualDesktopCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
/*                                                                                      *\
 * Set click flag.                                                                      *
\*                                                                                      */
    if(pHP->ulClickFlag==WM_BUTTON1CLICK)
        {
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTONCLICK, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTON1, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        }
    if(pHP->ulClickFlag==WM_BUTTON1DBLCLK)
        {
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTONDBLCLK, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTON1, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        }
    if(pHP->ulClickFlag==WM_BUTTON3CLICK)
        {
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTONCLICK, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTON3, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        }
    if(pHP->ulClickFlag==WM_BUTTON3DBLCLK)
        {
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTONDBLCLK, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_BUTTON3, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        }
                                        /* Check checkbox if button should be detected over
                                           any window */
    if(pHP->ulStatusFlag1 & POPUPSYSTEMWIDE)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_SYSTEMWIDE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if select first or center menuitem flag is set */
    if(!(pHP->ulStatusFlag1 & (SELECTFIRSTITEM|SELECTCENTERITEM)))
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_SELECTDEFAULT, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag1 & SELECTFIRSTITEM)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_SELECTFIRSTITEM, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag1 & SELECTCENTERITEM)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_SELECTCENTER, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
/*                                                                                      *\
 * Disable the child windows of the Popup-Menu icons, if this checkbox isn't checked.   *
\*                                                                                      */
                                        /* Check ckeckbox if Popup-Menu icons flag is set */
    if(pHP->ulStatusFlag2 & POPUPICON)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_POPUPICON, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, usChild2PopupIconCB,
            sizeof(usChild2PopupIconCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Select Popup-Menu icons' size flag */
    if(pHP->ulIconScale==0)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_FULLSIZE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDRB_HALFSIZE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
/*                                                                                      *\
 * Popup Menu keyboard activation flags.                                                *
\*                                                                                      */
    {
    ULONG   ulPopupKey;
    ULONG   ulShiftBitmask;

    for(ulPopupKey=DDCB_LEFTRIGHTSHIFT, ulShiftBitmask=0x00000001;
        ulPopupKey<=DDCB_SCROLLLOCK;
        ulPopupKey++, ulShiftBitmask<<=1)
        {
        if(pHP->ulPopupKeyFlag & ulShiftBitmask)
            WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, ulPopupKey, BM_SETCHECK,
                MPFROMSHORT(TRUE), (MPARAM)NULL);
        }
    }
/*                                                                                      *\
 * Disable the child windows of the sliding Focus, if this checkbox isn't checked.      *
\*                                                                                      */
    if(!(pHP->ulStatusFlag1 & SLIDINGFOCUS))
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, usChild2SlidingFocusCB,
            sizeof(usChild2SlidingFocusCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Check ckeckbox if sliding focus flag is set */
    if(pHP->ulStatusFlag1 & SLIDINGFOCUS)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_SLIDINGFOCUS, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check ckeckbox if preserve z-order flag is set */
    if(pHP->ulStatusFlag1 & PRESERVEZORDER)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_PRESERVEZORDER, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check ckeckbox if sliding focus should only activate
                                           windows not the control within the window the mouse pointer
                                           is positioned over */
    if(pHP->ulStatusFlag1 & FOCUSWINDOWLEVEL)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_ACTIVATEWINDOW, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Setup delay scrollbar between 0 and 2500 ms in 100 ms steps */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDHSB_SLIDINGDELAY,
        SBM_SETTHUMBSIZE, MPFROM2SHORT(1, 25), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDHSB_SLIDINGDELAY,
        SBM_SETSCROLLBAR, MPFROMSHORT(pHP->ulSlidingFocusDelay),
        MPFROM2SHORT(0, 25));
                                        /* Insert the string */
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDEF_SLIDINGDELAY,
        (CHAR *)_itoa(pHP->ulSlidingFocusDelay*TIMERINTERVALL100MS, ucBuffer, 10));
/*                                                                                      *\
 * Set hardware assisted panning (Virtual Desktop).                                     *
\*                                                                                      */
    if(pHP->ulStatusFlag2 & HWVIRTUALDESKTOP)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_HWVIRTUALDESKTOP, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, usChild2HWVirtualDesktopCB,
            sizeof(usChild2HWVirtualDesktopCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
    if((pHP->ulHWSizeX==1024) && (pHP->ulHWSizeY==768))
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDRB_1024X768, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else if((pHP->ulHWSizeX==800) && (pHP->ulHWSizeY==600))
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDRB_800X600, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDRB_640X480, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDMLE_PANNINGEXCEPTION, MLM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucHWVirtualDesktopException)), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDMLE_PANNINGEXCEPTION, pHP->ucHWVirtualDesktopException);
/*                                                                                      *\
 * Second notebook page.                                                                *
\*                                                                                      */
/*                                                                                      *\
 * Set overview window flags.                                                           *
\*                                                                                      */
    {
                                        /* To initialize the presentation parameters we send a
                                           WM_CONTROL message */
    ULONG   ulOverviewControl=0;

                                        /* Check Dynamic Overview window settings */
    if(pHP->ulStatusFlag2 & DYNAMICOVERVIEW)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_DYNAMICOVERVIEW, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, usChild2DynamicOverviewCB,
            sizeof(usChild2DynamicOverviewCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
    if(pHP->ulDynamicOverview & OVERVIEW_UL)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWUL, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulDynamicOverview & OVERVIEW_LL)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWLL, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulDynamicOverview & OVERVIEW_UR)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWUR, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulDynamicOverview & OVERVIEW_LR)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWLR, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);

    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDHSB_OVERVIEWDELAY,
        SBM_SETTHUMBSIZE, MPFROM2SHORT(1, 25), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDHSB_OVERVIEWDELAY,
        SBM_SETSCROLLBAR, MPFROMSHORT(pHP->ulOverviewDelay),
        MPFROM2SHORT(0, 25));
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDEF_OVERVIEWDELAY,
        (CHAR *)_itoa(pHP->ulOverviewDelay*TIMERINTERVALL100MS, ucBuffer, 10));
                                        /* Check ckeckbox if hide titlebar flag is set */
    if(pHP->ulStatusFlag1 & HIDETITLEBAR)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_HIDETITLEBAR, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check ckeckbox if hide smarticonbar flag is set */
    if(pHP->ulStatusFlag1 & HIDESMARTICONBAR)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_HIDESMARTICONBAR, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check Overview cursor type */
    if(pHP->ulStatusFlag1 & OVERVIEWCURSOR)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_TARGETCURSOR, BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if keep on top flag is set */
    if(pHP->ulStatusFlag1 & KEEPONTOPOV)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_KEEPONTOPOV, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag1 & ONECLICKSWITCH)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_ONECLICKSWITCH, BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check overview window draw mode controls */
    if(pHP->ulStatusFlag1 & OVERVIEW3DBORDER)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_3DBORDER, BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag1 & OVERVIEWTITLES)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_OVERVIEWTITLES, BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag1 & OVERVIEWAUTOCOLOR)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_TITLEAUTOCOLOR, BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Save current OVERVIEW_* style */
    if(pHP->ulOverviewWindowDrawMode==OVERVIEW_ICONS)
        {
        ulOverviewControl=DDRB_OVERVIEWICONS;
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWICONS, BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
        }
    if(pHP->ulOverviewWindowDrawMode==OVERVIEW_GRADIENT)
        {
        ulOverviewControl=DDRB_OVERVIEWGRADIENT;
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWGRADIENT, BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
        }
    if(pHP->ulOverviewWindowDrawMode==OVERVIEW_RANDOM)
        {
        ulOverviewControl=DDRB_OVERVIEWRANDOM;
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWRANDOM, BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
        }
    if(pHP->ulOverviewWindowDrawMode==OVERVIEW_MONOCHROME)
        {
        ulOverviewControl=DDRB_OVERVIEWMONO;
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDRB_OVERVIEWMONO, BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
        }
                                        /* Initialize presentation parameters of static text
                                           controls that should show the current Overview window
                                           colors */
    ulOverviewWindowDrawMode=(ULONG)-1;
    WinSendMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, WM_CONTROL,
        MPFROM2SHORT((USHORT)ulOverviewControl, BN_CLICKED), NULL);
/*                                                                                      *\
 * Set Sticky Windows. They must be terminated by \r\n.                                 *
\*                                                                                      */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDMLE_STICKYWINDOWS, MLM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucStickyWindows)), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDMLE_STICKYWINDOWS, pHP->ucStickyWindows);
    }
/*                                                                                      *\
 * Third notebook page.
\*                                                                                      */
/*                                                                                      *\
 * Set SessionBar flags.                                                                *
\*                                                                                      */
    if(pHP->ulStatusFlag2 & SHOWSESSIONBAR)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_SESSIONBAR, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        {
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2SessionBarCB,
            sizeof(usChild2SessionBarCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2SessionNameCB,
            sizeof(usChild2SessionNameCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2DynamicSessionBarCB,
            sizeof(usChild2DynamicSessionBarCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2CpuPerformanceCB,
            sizeof(usChild2CpuPerformanceCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
        }
                                        /* Check checkbox if keep on top flag is set */
    if(pHP->ulStatusFlag2 & KEEPONTOPSB)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_KEEPONTOPSB, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if no session name flag is set */
    if(pHP->ulStatusFlag2 & NOSESSIONNAMESB)
        {
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_NOSESSIONNAME, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2SessionNameCB,
            sizeof(usChild2SessionNameCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
        }
                                        /* Check checkbox if default date/time flag is set */
    if(pHP->ulStatusFlag2 & DEFAULTDATETIMESB)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_DEFAULTDATETIME, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if no display of date/time flag is set */
    if(pHP->ulStatusFlag2 & NODATETIMESB)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_NODATETIME, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Insert user date/time format */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_SESSIONBARDATETIME, EM_SETTEXTLIMIT,
        MPFROMSHORT(MAXNAMEL), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDEF_SESSIONBARDATETIME,
        pHP->ucSessionBarDateTime);
                                        /* Select SessionBar size flag */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage,
        (pHP->ulSessionBarScale==0 ? DDRB_SESSIONBARLARGE : DDRB_SESSIONBARSMALL), BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if BubbleHelp is requested */
    if(pHP->ulStatusFlag2 & BUBBLEHELPSB)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_SESSIONBARHELP, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if Dynamic SessionBar requested */
    if(pHP->ulStatusFlag2 & DYNAMICSESSIONBAR)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_DYNAMICSESSIONBAR, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2DynamicSessionBarCB,
            sizeof(usChild2DynamicSessionBarCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Setup delay scrollbar between 0 and 2500 ms in 100 ms steps */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDHSB_SESSIONBARDELAY,
        SBM_SETTHUMBSIZE, MPFROM2SHORT(1, 25), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDHSB_SESSIONBARDELAY,
        SBM_SETSCROLLBAR, MPFROMSHORT(pHP->ulSessionBarDelay),
        MPFROM2SHORT(0, 25));
                                        /* Insert the string */
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDEF_SESSIONBARDELAY,
        (CHAR *)_itoa(pHP->ulSessionBarDelay*TIMERINTERVALL100MS, ucBuffer, 10));
                                        /* Hotspot spinbutton */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDSB_SESSIONBARHOTSPOT,
        SPBM_SETLIMITS,
        MPFROMLONG(pHP->lIconSize+((SESSIONBAR_SPACE+SESSIONBAR_BORDER+SESSIONBAR_ICONFRAME)<<1)), MPFROMLONG(1));
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDSB_SESSIONBARHOTSPOT,
        SPBM_SETCURRENTVALUE, MPFROMLONG(pHP->ulSessionBarHotSpot), (MPARAM)0);
                                        /* Check checkbox if CPU Performance chart is requested */
    if(pHP->ulStatusFlag2 & CPUPERFORMANCESB)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_CPUPERFORMANCE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2CpuPerformanceCB,
            sizeof(usChild2CpuPerformanceCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Check checkbox if CPU Performance chart fill mode is requested */
    if(pHP->ulStatusFlag2 & PERFORMANCEFILLSB)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_PERFORMANCEFILL, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if CPU Performance no grid mode is requested */
    if(pHP->ulStatusFlag2 & NOGRIDSB)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_NOGRID, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Setup exception list for hidden icons */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDMLE_SESSIONBAREXCEPTION, MLM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucSessionBarException)), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDMLE_SESSIONBAREXCEPTION, pHP->ucSessionBarException);
                                        /* QuickSwitch window */
    if(pHP->ulStatusFlag2 & ENABLEQUICKSWITCH)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_ENABLEQUICKSWITCH, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
/*                                                                                      *\
 * Forth notebook page.                                                                 *
\*                                                                                      */
/*                                                                                      *\
 * Write the Desktop and Window List currently available or the default ones from       *
 * PC2.INI into the entryfields. The maximum length of both we assume MAXNAMEL.         *
\*                                                                                      */
                                        /* First set the limits to MAXNAMEL */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_DESKTOPNAME, EM_SETTEXTLIMIT,
        MPFROMSHORT(MAXNAMEL), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_WINDOWLISTNAME, EM_SETTEXTLIMIT,
        MPFROMSHORT(MAXNAMEL), (MPARAM)NULL);
                                        /* Insert the strings */
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_DESKTOPNAME, pHP->ucDesktopName);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_WINDOWLISTNAME, pHP->ucWindowListName);
                                        /* Check ckeckbox if button 2 z-order flag is set */
    if(pHP->ulStatusFlag1 & BUTTON2ZORDER)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDCB_BUTTON2ZORDER, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check ckeckbox if button 2 move window flag is set */
    if(pHP->ulStatusFlag1 & BUTTON2MOVE)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDCB_BUTTON2MOVE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if use Spooler Window flag is set */
    if(pHP->ulStatusFlag1 & SHOWSPOOLERWINDOW)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDCB_USESPOOLER, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
/*                                                                                      *\
 * Disable the child windows of the background bitmap, if this checkbox isn't checked.  *
\*                                                                                      */
    if(!(pHP->ulStatusFlag1 & BACKGROUNDBITMAP))
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, usChild2BackgroundBitmapCB,
            sizeof(usChild2BackgroundBitmapCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
    else
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDCB_BACKGROUNDBITMAP, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Setup bitmap name entryfield */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_BACKGROUNDBITMAP, EM_SETTEXTLIMIT,
        MPFROMSHORT(MAXNAMEL), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_BACKGROUNDBITMAP,
        pHP->ucBackgroundBitmap);
                                        /* Setup bitmap radiobuttons */
    if(pHP->ulBackgroundBitmapFlag & BITMAPNORMAL)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDRB_BITMAPNORMAL, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulBackgroundBitmapFlag & BITMAPSCALED)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDRB_BITMAPSCALED, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulBackgroundBitmapFlag & BITMAPTILED)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDRB_BITMAPTILED, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, usChild2BitmapTiledCB,
            sizeof(usChild2BitmapTiledCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
/*                                                                                      *\
 * Set tiled count of background bitmap into spinbuttons after initialising it into the *
 * range of 1 to 99 (taken from lower USHORT of ulBackgroundBitmapFlag).                *
\*                                                                                      */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDSB_BITMAPTILEDCOUNT,
        SPBM_SETLIMITS, MPFROMLONG(99), MPFROMLONG(1));
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDSB_BITMAPTILEDCOUNT,
        SPBM_SETCURRENTVALUE, MPFROMLONG(pHP->ulBackgroundBitmapFlag & 0xFFFF), (MPARAM)0);
/*                                                                                      *\
 * Fifth notebook page.                                                                 *
\*                                                                                      */
/*                                                                                      *\
 * Set general environment settings flag.                                               *
\*                                                                                      */
                                        /* Check checkbox if ignore autostart flag is set */
    if(pHP->ulStatusFlag1 & IGNOREAUTOSTART)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_IGNOREAUTOSTART, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if ignore scheduler flag is set */
    if(pHP->ulStatusFlag1 & IGNORESCHEDULER)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_IGNORESCHEDULER, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if Hotkeys should not be detected
                                           systemwide */
    if(pHP->ulStatusFlag1 & HOTKEY4PC2ONLY)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_HOTKEY4PC2ONLY, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check checkbox if PC/2 should enable Diagnostics features */
    if(pHP->ulStatusFlag2 & ENABLEDIAGNOSTICS)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ENABLEDIAGNOSTICS, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
/*                                                                                      *\
 * ShutDown option settings.                                                            *
\*                                                                                      */
                                        /* Check checkbox if normal shutdown flag is set
                                           and enable checkbuttons accordingly */
    if(pHP->ulStatusFlag1 & NORMALSHUTDOWN)
        {
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_NORMALSHUTDOWN, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2NormalShutdownCB,
            sizeof(usChild2NormalShutdownCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
        }
    if(pHP->ulStatusFlag1 & NOVERIFYSHUTDOWN)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_NOVERIFYSHUTDOWN, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag1 & SHUTDOWNREBOOT)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_SHUTDOWNREBOOT, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
/*                                                                                      *\
 * PowerDown option settings.                                                           *
\*                                                                                      */
    if(pHP->ulStatusFlag2 & SUPPORTAPM)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_SUPPORTAPM, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2APMCB,
            sizeof(usChild2APMCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
    if(pHP->ulStatusFlag2 & ENABLEAPM)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ENABLEAPM, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag2 & NOVERIFYPOWERDOWN)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_NOVERIFYPOWERDOWN, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag2 & POWERDOWNSUSPEND)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDRB_POWERDOWNSUSPEND, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag2 & POWERDOWNPOWEROFF)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDRB_POWERDOWNPOWEROFF, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Enable PowerDown timer */
    if(pHP->ulStatusFlag2 & ENABLEPOWERDOWN)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ENABLEPOWERDOWN, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2PowerDownTimerCB,
            sizeof(usChild2PowerDownTimerCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* PowerDown timer */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDSB_POWERDOWNTIMER,
        SPBM_SETLIMITS, MPFROMLONG(600), MPFROMLONG(1));
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDSB_POWERDOWNTIMER,
        SPBM_SETCURRENTVALUE, MPFROMLONG(pHP->ulPowerDownTimer & 0xFFFF), (MPARAM)0);
/*                                                                                      *\
 * Advanced marking settings.                                                           *
\*                                                                                      */
                                        /* Check checkbox if advanced marking is enabled */
    if(pHP->ulStatusFlag2 & ADVMARKINGBUTTON3)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDRB_ADVMARKINGBUTTON3, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDRB_ADVMARKINGBUTTON2, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag2 & ADVANCEDMARKING)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ADVANCEDMARKING, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2AdvancedMarkingCB,
            sizeof(usChild2AdvancedMarkingCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
/*                                                                                      *\
 * Dynamic menu settings.                                                               *
\*                                                                                      */
                                        /* Check checkbox if dynamic menu is enabled */
    if(pHP->ulStatusFlag2 & DYNAMICMENU)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_DYNAMICMENU, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2DynamicMenuCB,
            sizeof(usChild2DynamicMenuCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
    if(pHP->ulStatusFlag2 & CASCADEMENU)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_CASCADEMENU, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulStatusFlag2 & DYNAMICMENUKBD)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_DYNAMICMENUKBD, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Setup delay scrollbar between 0 and 2500 ms in 100 ms steps */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDHSB_DYNAMICDELAY,
        SBM_SETTHUMBSIZE, MPFROM2SHORT(1, 25), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDHSB_DYNAMICDELAY,
        SBM_SETSCROLLBAR, MPFROMSHORT(pHP->ulDynamicMenuDelay),
        MPFROM2SHORT(0, 25));
                                        /* Insert the string */
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDEF_DYNAMICDELAY,
        (CHAR *)_itoa(pHP->ulDynamicMenuDelay*TIMERINTERVALL100MS, ucBuffer, 10));
/*                                                                                      *\
 * Sixth notebook page. Query the Titlebar SmartIcons.                                  *
\*                                                                                      */
    {
    ULONG   ulTitlebarSmarticon;
    ULONG   ulShiftBitmask;

                                        /* Check checkbox if titlebar smarticons are enabled */
    if(pHP->ulStatusFlag2 & TITLEBARICONS)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_TITLEBAR].hwndNBPage, DDCB_TITLEBARICONS, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_TITLEBAR].hwndNBPage, usChild2TitlebarIconsCB,
            sizeof(usChild2TitlebarIconsCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
    for(ulTitlebarSmarticon=DDCB_CLOSEWINDOW, ulShiftBitmask=0x00000001;
        ulTitlebarSmarticon<=DDCB_MOVEEDGE;
        ulTitlebarSmarticon++, ulShiftBitmask<<=1)
        {
        if(pHP->ulTitlebarFlag & ulShiftBitmask)
            WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_TITLEBAR].hwndNBPage, ulTitlebarSmarticon, BM_SETCHECK,
                MPFROMSHORT(TRUE), (MPARAM)NULL);
        }
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_TITLEBAR].hwndNBPage, DDMLE_TITLEBAREXCEPTION, MLM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucTitlebarException)), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_TITLEBAR].hwndNBPage, DDMLE_TITLEBAREXCEPTION, pHP->ucTitlebarException);
    }
/*                                                                                      *\
 * Seventh notebook page. The passwords are converted to readable text and inserted.    *
 * When saving the values they are encrypted again.                                     *
\*                                                                                      */
    {
    LONG    lTemp;

                                        /* Access password */
    strcpy(ucDDAccessPassword, pHP->ucAccessPassword);
    for(lTemp=strlen(ucDDAccessPassword)-1; lTemp>=0; lTemp--)
        ucDDAccessPassword[lTemp]=(ucDDAccessPassword[lTemp]^0x3C)>>1;
    strcpy(ucDDVerifyAccessPassword, ucDDAccessPassword);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_ACCESSPASSWORD, EM_SETTEXTLIMIT,
        MPFROMSHORT(PASSWORDSIZE), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_ACCESSPASSWORD,
        ucDDAccessPassword);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_VERIFYACCESSPWD, EM_SETTEXTLIMIT,
        MPFROMSHORT(PASSWORDSIZE), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_VERIFYACCESSPWD,
        ucDDVerifyAccessPassword);
                                        /* Lockup password */
    strcpy(ucDDLockupPassword, pHP->ucLockupPassword);
    for(lTemp=strlen(ucDDLockupPassword)-1; lTemp>=0; lTemp--)
        ucDDLockupPassword[lTemp]=(ucDDLockupPassword[lTemp]^0x3C)>>1;
    strcpy(ucDDVerifyLockupPassword, ucDDLockupPassword);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_LOCKUPPASSWORD, EM_SETTEXTLIMIT,
        MPFROMSHORT(PASSWORDSIZE), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_LOCKUPPASSWORD,
        ucDDLockupPassword);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_VERIFYLOCKUPPWD, EM_SETTEXTLIMIT,
        MPFROMSHORT(PASSWORDSIZE), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDEF_VERIFYLOCKUPPWD,
        ucDDVerifyLockupPassword);
                                        /* PC2 Security */
    if(pHP->ulStatusFlag2 & PC2SECURITY)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDCB_PC2SECURITY, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Enable lockup */
    if(pHP->ulStatusFlag2 & ENABLELOCKUP)
        {
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDCB_ENABLELOCKUP, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, usChild2LockupTimerCB,
            sizeof(usChild2LockupTimerCB)/sizeof(USHORT), WS_VISIBLE);
        }
    else
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, usChild2LockupTimerCB,
            sizeof(usChild2LockupTimerCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Lockup on boot */
    if(pHP->ulStatusFlag2 & LOCKUPONBOOT)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDCB_LOCKUPONBOOT, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Lockup timer */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDSB_LOCKUPTIMER,
        SPBM_SETLIMITS, MPFROMLONG(60), MPFROMLONG(1));
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDSB_LOCKUPTIMER,
        SPBM_SETCURRENTVALUE, MPFROMLONG(pHP->ulLockupTimer & 0xFFFF), (MPARAM)0);
    }
/*                                                                                      *\
 * Eigth notebook page (page 1 and page 2). We fill the Environment Spaces listbox from *
 * the names of the Environment Spaces from the linked list.                            *
\*                                                                                      */
    {
    ENVIRONMENTLIST *pEnvironmentListCurrent;

                                        /* Disable the Add, Change and Remove pushbutton */
                                        /* Insert Environment Spaces sequentially */
    DisableDialogItem(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, usChild2EnvironmentLB,
        sizeof(usChild2EnvironmentLB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
    for(pEnvironmentListCurrent=pHP->pEnvironmentSpaces;
        pEnvironmentListCurrent!=NULL;
        pEnvironmentListCurrent=pEnvironmentListCurrent->pNext)
        {
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT,
            LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(pEnvironmentListCurrent->pEnvironmentName));
        }
                                        /* Set Environment Space name's text limit */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDEF_ENVIRONMENT, EM_SETTEXTLIMIT,
        MPFROMSHORT(EF_SIZE255), (MPARAM)NULL);
                                        /* Select first Environment Space if at least one
                                           is available */
    if(pHP->pEnvironmentSpaces!=NULL)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT, LM_SELECTITEM,
            MPFROMSHORT(0), (MPARAM)TRUE);
    }
/*                                                                                      *\
 * Nineth notebook page.                                                                *
\*                                                                                      */
    {
    LONG    lTemp;

    if(pHP->ulRegistrationFlag & REGISTER_ONLINE)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDCB_AUTOUPDATEENABLE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pHP->ulRegistrationFlag & REGISTER_USER)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDCB_SENDREGISTRATION, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Initialize user information */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_FIRSTNAME, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucUserFirstName)-1), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_LASTNAME, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucUserLastName)-1), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_EMAILADDRESS, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucUserEmailAddress)-1), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_FIRSTNAME, pHP->ucUserFirstName);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_LASTNAME, pHP->ucUserLastName);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_EMAILADDRESS, pHP->ucUserEmailAddress);
                                        /* Initialize URL data */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_PROXYURL, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucProxyUrl)-1), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_UPDATEURL, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucUpdateUrl)-1), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_PROXYURL,
        pHP->ucProxyUrl);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_UPDATEURL,
        pHP->ucUpdateUrl);
#ifdef  DEBUG
    WinSendMsg(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_UPDATEURL),
        EM_SETREADONLY, MPFROMSHORT(FALSE), NULL);
#endif
                                        /* Initialize HTTP Userid and Password */
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_HTTPUSERID, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucUserHttpUserid)-1), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_HTTPPASSWORD, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pHP->ucUserHttpPassword)-1), (MPARAM)NULL);
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_HTTPUSERID,
        pHP->ucUserHttpUserid);
    strcpy(ucDDUserHttpPassword, pHP->ucUserHttpPassword);
    for(lTemp=strlen(ucDDUserHttpPassword)-1; lTemp>=0; lTemp--)
        ucDDUserHttpPassword[lTemp]=(ucDDUserHttpPassword[lTemp]^0x3C)>>1;
    WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_HTTPPASSWORD,
        ucDDUserHttpPassword);
                                        /* Initialize networking status text */
    if(!(pHP->ulRuntimeFlag & TCPIPSUPPORTED))
        {
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDST_NETWORKSTATUS,
            "This feature requires TCP/IP and the DLL PC2NET.DLL, but no support could be detected!");
        DisableDialogItem(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, usChild2UpdateNowST,
            sizeof(usChild2UpdateNowST)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
        }
                                        /* Initialize networking status checkbox */
    if(pHP->ulRegistrationFlag & REGISTER_SHOWSTATUS)
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDCB_NETWORKSTATUS, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    }
/*                                                                                      *\
 * Tenth notebook page. We just set the length of the controls (which are readonly).    *
 * The data gets inserted while processing the WM_SPYFRAMEWINDOW message.               *
\*                                                                                      */
    {
    SPYFRAME    *pspyframeDummy=0;

    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMEWINDOWTEXT, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pspyframeDummy->ucWindowText)-1), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_EXECUTABLE, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pspyframeDummy->ucExecutable)-1), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMEWINDOWLIST, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pspyframeDummy->ucWindowList)-1), (MPARAM)NULL);
    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMEWINDOWCLASS, EM_SETTEXTLIMIT,
        MPFROMSHORT(sizeof(pspyframeDummy->ucWindowClass)-1), (MPARAM)NULL);
    }
/*                                                                                      *\
 * Now turn to the requested notebook page.                                             *
\*                                                                                      */
    if(pDDParams!=0)
        {
        WinSendMsg(hwndNotebook, BKM_TURNTOPAGE, MPFROMLONG(nbDDNotebookPage[pDDParams->usTurnToPage].ulIDPage), NULL);
        if(pDDParams->usMessageBox==HELP_PC2REGISTRATION)
            USR_ERR(hwndNotebook, pDDParams->usMessageBox,
                MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Please fill in the Registration information!\n\n"\
                "As you know PC/2 is and ever will be Freeware, but I would ask you to fill out the Registration "\
                "notebook page, which allows PC/2 to keep you informed about news and updates and allows me to "\
                "know you as a registered user, as this is the only way for me to find out the user base.\n\n"\
                "Please select Help for some tips filling in the dialog correctly (e.g. Proxy) and about the "
                "committment that by no means registering will cause you any harm or spamming!");
        }
#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld <- WM_SETUPNOTEBOOKPAGES\n", __LINE__);
#endif  /* DEBUG_DIALOG */
    break;
    }

/*                                                                                      *\
 * Syntax: WM_SPYFRAMEWINDOW, (SPYFRAME *pspyframeUpdate, NULL)                         *
\*                                                                                      */
case WM_SPYFRAMEWINDOW:
/*                                                                                      *\
 * This message is posted by the working thread if we need to update the Spy notebook   *
 * page from the structure passed by mp1.                                               *
\*                                                                                      */
    {
    SPYFRAME    *pspyframeUpdate=PVOIDFROMMP(mp1);
    UCHAR       ucBufferHex[]="0x????????";
    SWP         swpframeUpdate;
    UCHAR       ucBufferDec[]="????????";

#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld WM_SPYFRAMEWINDOW\n", __LINE__);
#endif  /* DEBUG_DIALOG */
    if(pspyframeUpdate)
        {
        WinQueryWindowPos(pspyframeUpdate->hwndFrame, &swpframeUpdate);
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMEWINDOWTEXT, pspyframeUpdate->ucWindowText);
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMEWINDOWCLASS, pspyframeUpdate->ucWindowClass);
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMEWINDOWLIST, pspyframeUpdate->ucWindowList);
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_EXECUTABLE, pspyframeUpdate->ucExecutable);
        _itoa(pspyframeUpdate->pidWindow, &ucBufferHex[2], 16);
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMEPID, ucBufferHex);
        _itoa(pspyframeUpdate->tidWindow, &ucBufferHex[2], 16);
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMETID, ucBufferHex);
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMEX,
            (CHAR *)_itoa(swpframeUpdate.x, ucBufferDec, 10));
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMEY,
            (CHAR *)_itoa(swpframeUpdate.y, ucBufferDec, 10));
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMECX,
            (CHAR *)_itoa(swpframeUpdate.cx, ucBufferDec, 10));
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMECY,
            (CHAR *)_itoa(swpframeUpdate.cy, ucBufferDec, 10));
        _itoa(pspyframeUpdate->hwndFrame, &ucBufferHex[2], 16);
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDEF_FRAMEHWND, ucBufferHex);
        }
                                        /* If the user has pressed the ALT key, then spying should
                                           be disabled to allow the user to copy & pasted the last
                                           spyed window */
    if(!(pHP->ulRuntimeFlag & SPYFRAMEWINDOW))
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDCB_SPYFRAMEWINDOW, BM_SETCHECK,
            MPFROMSHORT(FALSE), (MPARAM)NULL);
    }
    break;

case WM_CHAR:
/*                                                                                      *\
 * Process the navigation keys on this page and forward all unprocessed keys to the     *
 * dialog where the notebook is part of.                                                *
\*                                                                                      */
    ProcessPageKey(hwndDlg, WinQueryWindow(hwndNotebook, QW_OWNER), mp1, mp2);
    break;

case WM_HSCROLL:                        /* Messages generated by horizontal scrollbars */
/*                                                                                      *\
 * This message is generated when a horizontal scrollbar is manipulated. See if it is   *
 * our scrollbar DDHSB_SCROLLPERCENTAGE and process.                                    *
\*                                                                                      */
    {
    UCHAR       ucBuffer[4];
    SHORT       sSliderPosition=SHORT1FROMMP(mp2);

#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld -> WM_HSCROLL\n", __LINE__);
#endif  /* DEBUG_DIALOG */
    if(SHORT1FROMMP(mp1)==DDHSB_SCROLLPERCENTAGE)
        {
        switch(SHORT2FROMMP(mp2))
        {                               /* All messages contain the new slider position,
                                           so just use this value to change the priority
                                           delta entryfield value */
        case SB_LINELEFT:
            pHP->ulScrollPercentage-=5;
            break;

        case SB_PAGELEFT:
            pHP->ulScrollPercentage-=25;
            break;

        case SB_LINERIGHT:
            pHP->ulScrollPercentage+=5;
            break;

        case SB_PAGERIGHT:
            pHP->ulScrollPercentage+=25;
            break;

        case SB_SLIDERTRACK:
        case SB_SLIDERPOSITION:
            pHP->ulScrollPercentage=sSliderPosition*5;
            break;
        }
        if(((LONG)pHP->ulScrollPercentage)<=0) pHP->ulScrollPercentage=5;
        if(pHP->ulScrollPercentage>100) pHP->ulScrollPercentage=100;
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDEF_SCROLLPERCENTAGE,
            (CHAR *)_itoa(pHP->ulScrollPercentage, ucBuffer, 10));
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDHSB_SCROLLPERCENTAGE,
            SBM_SETPOS, MPFROMSHORT(pHP->ulScrollPercentage/5), (MPARAM)NULL);
        }
/*                                                                                      *\
 * This message is generated when a horizontal scrollbar is manipulated. See if it is   *
 * our scrollbar DDHSB_DESKTOPDELAY and process.                                        *
\*                                                                                      */
    if(SHORT1FROMMP(mp1)==DDHSB_DESKTOPDELAY)
        {
        switch(SHORT2FROMMP(mp2))
        {                               /* All messages contain the new slider position,
                                           so just use this value to change the priority
                                           delta entryfield value */
        case SB_LINELEFT:
            pHP->ulVirtualDesktopDelay-=1;
            break;

        case SB_PAGELEFT:
            pHP->ulVirtualDesktopDelay-=5;
            break;

        case SB_LINERIGHT:
            pHP->ulVirtualDesktopDelay+=1;
            break;

        case SB_PAGERIGHT:
            pHP->ulVirtualDesktopDelay+=5;
            break;

        case SB_SLIDERTRACK:
        case SB_SLIDERPOSITION:
            pHP->ulVirtualDesktopDelay=sSliderPosition;
            break;
        }
        if(((LONG)pHP->ulVirtualDesktopDelay)<0) pHP->ulVirtualDesktopDelay=0;
        if(pHP->ulVirtualDesktopDelay>25) pHP->ulVirtualDesktopDelay=25;
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDEF_DESKTOPDELAY,
            (CHAR *)_itoa(pHP->ulVirtualDesktopDelay*TIMERINTERVALL100MS, ucBuffer, 10));
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDHSB_DESKTOPDELAY,
            SBM_SETPOS, MPFROMSHORT(pHP->ulVirtualDesktopDelay), (MPARAM)NULL);
        }
/*                                                                                      *\
 * This message is generated when a horizontal scrollbar is manipulated. See if it is   *
 * our scrollbar DDHSB_SLIDINGDELAY and process.                                        *
\*                                                                                      */
    if(SHORT1FROMMP(mp1)==DDHSB_SLIDINGDELAY)
        {
        switch(SHORT2FROMMP(mp2))
        {                               /* All messages contain the new slider position,
                                           so just use this value to change the priority
                                           delta entryfield value */
        case SB_LINELEFT:
            pHP->ulSlidingFocusDelay-=1;
            break;

        case SB_PAGELEFT:
            pHP->ulSlidingFocusDelay-=5;
            break;

        case SB_LINERIGHT:
            pHP->ulSlidingFocusDelay+=1;
            break;

        case SB_PAGERIGHT:
            pHP->ulSlidingFocusDelay+=5;
            break;

        case SB_SLIDERTRACK:
        case SB_SLIDERPOSITION:
            pHP->ulSlidingFocusDelay=sSliderPosition;
            break;
        }
        if(((LONG)pHP->ulSlidingFocusDelay)<0) pHP->ulSlidingFocusDelay=0;
        if(pHP->ulSlidingFocusDelay>25) pHP->ulSlidingFocusDelay=25;
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDEF_SLIDINGDELAY,
            (CHAR *)_itoa(pHP->ulSlidingFocusDelay*TIMERINTERVALL100MS, ucBuffer, 10));
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDHSB_SLIDINGDELAY,
            SBM_SETPOS, MPFROMSHORT(pHP->ulSlidingFocusDelay), (MPARAM)NULL);
        }
/*                                                                                      *\
 * This message is generated when a horizontal scrollbar is manipulated. See if it is   *
 * our scrollbar DDHSB_OVERVIEWDELAY and process.                                       *
\*                                                                                      */
    if(SHORT1FROMMP(mp1)==DDHSB_OVERVIEWDELAY)
        {
        switch(SHORT2FROMMP(mp2))
        {                               /* All messages contain the new slider position,
                                           so just use this value to change the priority
                                           delta entryfield value */
        case SB_LINELEFT:
            pHP->ulOverviewDelay-=1;
            break;

        case SB_PAGELEFT:
            pHP->ulOverviewDelay-=5;
            break;

        case SB_LINERIGHT:
            pHP->ulOverviewDelay+=1;
            break;

        case SB_PAGERIGHT:
            pHP->ulOverviewDelay+=5;
            break;

        case SB_SLIDERTRACK:
        case SB_SLIDERPOSITION:
            pHP->ulOverviewDelay=sSliderPosition;
            break;
        }
        if(((LONG)pHP->ulOverviewDelay)<0) pHP->ulOverviewDelay=0;
        if(pHP->ulOverviewDelay>25) pHP->ulOverviewDelay=25;
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDEF_OVERVIEWDELAY,
            (CHAR *)_itoa(pHP->ulOverviewDelay*TIMERINTERVALL100MS, ucBuffer, 10));
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDHSB_OVERVIEWDELAY,
            SBM_SETPOS, MPFROMSHORT(pHP->ulOverviewDelay), (MPARAM)NULL);
        }
/*                                                                                      *\
 * This message is generated when a horizontal scrollbar is manipulated. See if it is   *
 * our scrollbar DDHSB_SESSIONBARDELAY and process.                                     *
\*                                                                                      */
    if(SHORT1FROMMP(mp1)==DDHSB_SESSIONBARDELAY)
        {
        switch(SHORT2FROMMP(mp2))
        {                               /* All messages contain the new slider position,
                                           so just use this value to change the priority
                                           delta entryfield value */
        case SB_LINELEFT:
            pHP->ulSessionBarDelay-=1;
            break;

        case SB_PAGELEFT:
            pHP->ulSessionBarDelay-=5;
            break;

        case SB_LINERIGHT:
            pHP->ulSessionBarDelay+=1;
            break;

        case SB_PAGERIGHT:
            pHP->ulSessionBarDelay+=5;
            break;

        case SB_SLIDERTRACK:
        case SB_SLIDERPOSITION:
            pHP->ulSessionBarDelay=sSliderPosition;
            break;
        }
        if(((LONG)pHP->ulSessionBarDelay)<0) pHP->ulSessionBarDelay=0;
        if(pHP->ulSessionBarDelay>25) pHP->ulSessionBarDelay=25;
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDEF_SESSIONBARDELAY,
            (CHAR *)_itoa(pHP->ulSessionBarDelay*TIMERINTERVALL100MS, ucBuffer, 10));
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDHSB_SESSIONBARDELAY,
            SBM_SETPOS, MPFROMSHORT(pHP->ulSessionBarDelay), (MPARAM)NULL);
        }
/*                                                                                      *\
 * This message is generated when a horizontal scrollbar is manipulated. See if it is   *
 * our scrollbar DDHSB_DYNAMICDELAY and process.                                        *
\*                                                                                      */
    if(SHORT1FROMMP(mp1)==DDHSB_DYNAMICDELAY)
        {
        switch(SHORT2FROMMP(mp2))
        {                               /* All messages contain the new slider position,
                                           so just use this value to change the priority
                                           delta entryfield value */
        case SB_LINELEFT:
            pHP->ulDynamicMenuDelay-=1;
            break;

        case SB_PAGELEFT:
            pHP->ulDynamicMenuDelay-=5;
            break;

        case SB_LINERIGHT:
            pHP->ulDynamicMenuDelay+=1;
            break;

        case SB_PAGERIGHT:
            pHP->ulDynamicMenuDelay+=5;
            break;

        case SB_SLIDERTRACK:
        case SB_SLIDERPOSITION:
            pHP->ulDynamicMenuDelay=sSliderPosition;
            break;
        }
        if(((LONG)pHP->ulDynamicMenuDelay)<0) pHP->ulDynamicMenuDelay=0;
        if(pHP->ulDynamicMenuDelay>25) pHP->ulDynamicMenuDelay=25;
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDEF_DYNAMICDELAY,
            (CHAR *)_itoa(pHP->ulDynamicMenuDelay*TIMERINTERVALL100MS, ucBuffer, 10));
        WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDHSB_DYNAMICDELAY,
            SBM_SETPOS, MPFROMSHORT(pHP->ulDynamicMenuDelay), (MPARAM)NULL);
        }
#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld <- WM_HSCROLL\n", __LINE__);
#endif  /* DEBUG_DIALOG */
    }
    break;

case WM_CONTROL:                        /* Test for checkbuttons buttons pressed */
/*                                                                                      *\
 * Only if the DDCB_VIRTUALDESKTOP checkbox is pressed, its child windows are to be     *
 * enabled. Otherwise the are disabled, but allways visible.                            *
\*                                                                                      */
    {
    ULONG   ulButtonChecked;

#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld -> WM_CONTROL\n", __LINE__);
#endif  /* DEBUG_DIALOG */
                                        /* Was button class window clicked or double-clicked ? */
    if((SHORT2FROMMP(mp1)==BN_CLICKED) || (SHORT2FROMMP(mp1)==BN_DBLCLICKED))
        switch(SHORT1FROMMP(mp1))
        {
        case DDCB_VIRTUALDESKTOP:       /* Virtal Desktop enable/disable checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_VIRTUALDESKTOP, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
                                        /* If checked enable and show VirtualDesktopCB child windows */
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, usChild2VirtualDesktopCB,
                sizeof(usChild2VirtualDesktopCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDCB_MOVEDESKTOP:          /* Move WPS selected */
            if((BOOL)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_MOVEDESKTOP, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL)==TRUE)
                {                       /* If expand WPS is checked, uncheck it and disable */
                WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_EXPANDWPS, BM_SETCHECK,
                    MPFROMSHORT(FALSE), (MPARAM)NULL);
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, usChild2MoveDesktopCB,
                    sizeof(usChild2MoveDesktopCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                }
            else
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, usChild2MoveDesktopCB,
                    sizeof(usChild2MoveDesktopCB)/sizeof(USHORT), WS_VISIBLE);
            break;

        case DDCB_CLICK2MOVE:           /* Click 2 Move selected */
            if((BOOL)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_CLICK2MOVE, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL)==TRUE)
                {                       /* If autoscroll is checked, uncheck it and disable */
                WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_AUTOSCROLL, BM_SETCHECK,
                    MPFROMSHORT(FALSE), (MPARAM)NULL);
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, usChild2Click2MoveCB,
                    sizeof(usChild2Click2MoveCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                }
            else
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, usChild2Click2MoveCB,
                    sizeof(usChild2Click2MoveCB)/sizeof(USHORT), WS_VISIBLE);
            break;

        case DDCB_POPUPICON:            /* Popup-Menu icons enable/disable checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_POPUPICON, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, usChild2PopupIconCB,
                sizeof(usChild2PopupIconCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDCB_SLIDINGFOCUS:         /* Sliding Focus enable/disable checkbox */
            if((BOOL)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_SLIDINGFOCUS, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL)==TRUE)
                                        /* Enable and show SlidingFocusCB child windows */
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, usChild2SlidingFocusCB,
                    sizeof(usChild2SlidingFocusCB)/sizeof(USHORT), WS_VISIBLE);
            else
                {                       /* If preserve Z-order is checked, uncheck it */
                if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_PRESERVEZORDER))
                    WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL1].hwndNBPage, DDCB_PRESERVEZORDER, BM_SETCHECK,
                    MPFROMSHORT(FALSE), (MPARAM)NULL);
                                        /* Disable and show SlidingFocusCB child windows */
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, usChild2SlidingFocusCB,
                    sizeof(usChild2SlidingFocusCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                }
            break;

        case DDCB_DYNAMICOVERVIEW:      /* Dynamic Overview enable/disable checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDCB_DYNAMICOVERVIEW, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
                                        /* If checked disable and show SessionNameCB child windows */
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, usChild2DynamicOverviewCB,
                sizeof(usChild2DynamicOverviewCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDCB_NOSESSIONNAME:        /* No session name checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_NOSESSIONNAME, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
                                        /* If checked disable and show SessionNameCB child windows */
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2SessionNameCB,
                sizeof(usChild2SessionNameCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE | WS_DISABLED : WS_VISIBLE));
            break;

        case DDCB_DYNAMICSESSIONBAR:    /* Dynamic SessionBar enable/disable checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_DYNAMICSESSIONBAR, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
                                        /* If checked enable and show DynamicSessionBarCB child windows */
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2DynamicSessionBarCB,
                sizeof(usChild2DynamicSessionBarCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDCB_CPUPERFORMANCE:       /* CPU Performance chart enable/disable checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_CPUPERFORMANCE, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
                                        /* If checked enable and show CpuPerformanceCB child windows */
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2CpuPerformanceCB,
                sizeof(usChild2CpuPerformanceCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDCB_DYNAMICMENU:          /* Dynamic Menuselection enable/disable checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_DYNAMICMENU, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
                                        /* If checked enable and show DynamicMenuCB child windows */
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2DynamicMenuCB,
                sizeof(usChild2DynamicMenuCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDRB_OVERVIEWICONS:
        case DDRB_OVERVIEWGRADIENT:
        case DDRB_OVERVIEWRANDOM:
        case DDRB_OVERVIEWMONO:
            {
            ULONG   ulDialogColor=SYSCLR_DIALOGBACKGROUND;

                                        /* Query Presentation Parameters of last active radiobutton */
            WinQueryPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR1),
                PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLOR, NULL, sizeof(pHP->ulOverviewRGBColorBgnd),
                &pHP->ulOverviewRGBColorBgnd, QPF_PURERGBCOLOR);
            WinQueryPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR2),
                PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLOR, NULL, sizeof(pHP->ulOverviewRGBColorFgnd),
                &pHP->ulOverviewRGBColorFgnd, QPF_PURERGBCOLOR);
            switch(ulOverviewWindowDrawMode)
            {
            case DDRB_OVERVIEWICONS:
                WinQueryPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR3),
                    PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLOR, NULL, sizeof(pHP->ulOverviewRGBColorWnd),
                    &pHP->ulOverviewRGBColorWnd, QPF_PURERGBCOLOR);
                break;

            case DDRB_OVERVIEWGRADIENT:
                WinQueryPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR3),
                    PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLOR, NULL, sizeof(pHP->ulOverviewGradientStart),
                    &pHP->ulOverviewGradientStart, QPF_PURERGBCOLOR);
                WinQueryPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR4),
                    PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLOR, NULL, sizeof(pHP->ulOverviewGradientEnd),
                    &pHP->ulOverviewGradientEnd, QPF_PURERGBCOLOR);
                break;

            case DDRB_OVERVIEWMONO:
                WinQueryPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR3),
                    PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLOR, NULL, sizeof(pHP->ulOverviewRGBColorWnd),
                    &pHP->ulOverviewRGBColorWnd, QPF_PURERGBCOLOR);
                break;
            }
                                        /* Load Presentation Parameters for current active radiobutton */
            WinSetPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR1),
                PP_BACKGROUNDCOLOR, sizeof(ULONG), &pHP->ulOverviewRGBColorBgnd);
            WinSetPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR2),
                PP_BACKGROUNDCOLOR, sizeof(ULONG), &pHP->ulOverviewRGBColorFgnd);
            WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_COLOR1, "Overview Color");
            WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_COLOR2, "Title Color");
            switch(SHORT1FROMMP(mp1))
            {
            case DDRB_OVERVIEWICONS:
                WinSetPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR3),
                    PP_BACKGROUNDCOLOR, sizeof(ULONG), &pHP->ulOverviewRGBColorWnd);
                WinSetPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR4),
                    PP_BACKGROUNDCOLOR, sizeof(ULONG), &ulDialogColor);
                WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_COLOR3, "Window Color");
                WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_COLOR4, "");
                break;

            case DDRB_OVERVIEWGRADIENT:
                WinSetPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR3),
                    PP_BACKGROUNDCOLOR, sizeof(ULONG), &pHP->ulOverviewGradientStart);
                WinSetPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR4),
                    PP_BACKGROUNDCOLOR, sizeof(ULONG), &pHP->ulOverviewGradientEnd);
                WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_COLOR3, "Gradient From");
                WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_COLOR4, "Gradient To");
                break;

            case DDRB_OVERVIEWRANDOM:
                WinSetPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR3),
                    PP_BACKGROUNDCOLOR, sizeof(ULONG), &ulDialogColor);
                WinSetPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR4),
                    PP_BACKGROUNDCOLOR, sizeof(ULONG), &ulDialogColor);
                WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_COLOR3, "");
                WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_COLOR4, "");
                break;

            case DDRB_OVERVIEWMONO:
                WinSetPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR3),
                    PP_BACKGROUNDCOLOR, sizeof(ULONG), &pHP->ulOverviewRGBColorWnd);
                WinSetPresParam(WinWindowFromID(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_RGBCOLOR4),
                    PP_BACKGROUNDCOLOR, sizeof(ULONG), &ulDialogColor);
                WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_COLOR3, "Window Color");
                WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDST_COLOR4, "");
                break;
            }
            ulOverviewWindowDrawMode=(ULONG)SHORT1FROMMP(mp1);
            }
            break;

        case DDCB_HWVIRTUALDESKTOP:     /* HW Virtual Desktop enable/disable checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, DDCB_HWVIRTUALDESKTOP, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_VIRTUAL2].hwndNBPage, usChild2HWVirtualDesktopCB,
                sizeof(usChild2HWVirtualDesktopCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDCB_SESSIONBAR:           /* SessionBar enable/disable checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_SESSIONBAR, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            if(ulButtonChecked==TRUE)
                {
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2SessionBarCB,
                    sizeof(usChild2SessionBarCB)/sizeof(USHORT), WS_VISIBLE);
                if((ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_NOSESSIONNAME, BM_QUERYCHECK,
                    (MPARAM)NULL, (MPARAM)NULL)!=TRUE)
                    DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2SessionNameCB,
                        sizeof(usChild2SessionNameCB)/sizeof(USHORT), WS_VISIBLE);
                if((ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_DYNAMICSESSIONBAR, BM_QUERYCHECK,
                    (MPARAM)NULL, (MPARAM)NULL)==TRUE)
                    DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2DynamicSessionBarCB,
                        sizeof(usChild2DynamicSessionBarCB)/sizeof(USHORT), WS_VISIBLE);
                if((ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, DDCB_CPUPERFORMANCE, BM_QUERYCHECK,
                    (MPARAM)NULL, (MPARAM)NULL)==TRUE)
                    DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2CpuPerformanceCB,
                        sizeof(usChild2CpuPerformanceCB)/sizeof(USHORT), WS_VISIBLE);
                }
            else
                {
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2SessionBarCB,
                    sizeof(usChild2SessionBarCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2SessionNameCB,
                    sizeof(usChild2SessionNameCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2DynamicSessionBarCB,
                    sizeof(usChild2DynamicSessionBarCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_SESSIONBAR].hwndNBPage, usChild2CpuPerformanceCB,
                    sizeof(usChild2CpuPerformanceCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                }
            break;

        case DDCB_BACKGROUNDBITMAP:     /* Background bitmap enable/disable checkbox */
            if((BOOL)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDCB_BACKGROUNDBITMAP, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL)==TRUE)
                {
                                        /* Enable and show BackgroundBitmapCB child windows */
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, usChild2BackgroundBitmapCB,
                    sizeof(usChild2BackgroundBitmapCB)/sizeof(USHORT), WS_VISIBLE);
                                        /* If bitmap should be not tiled, ensure that spinbutton is disabled */
                if((BOOL)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDRB_BITMAPTILED, BM_QUERYCHECK,
                    (MPARAM)NULL, (MPARAM)NULL)==FALSE)
                    DisableDialogItem(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, usChild2BitmapTiledCB,
                        sizeof(usChild2BitmapTiledCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                }
            else
                                        /* Disable and show BackgroundBitmapCB child windows */
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, usChild2BackgroundBitmapCB,
                    sizeof(usChild2BackgroundBitmapCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
            break;

        case DDRB_BITMAPNORMAL:         /* Enable tile count spinbutton on DDRB_BITMAPTILED otherwise disable it */
        case DDRB_BITMAPSCALED:
        case DDRB_BITMAPTILED:
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDRB_BITMAPTILED, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, usChild2BitmapTiledCB,
                sizeof(usChild2BitmapTiledCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDCB_NORMALSHUTDOWN:       /* ShutDown options */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_NORMALSHUTDOWN, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2NormalShutdownCB,
                sizeof(usChild2NormalShutdownCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE | WS_DISABLED : WS_VISIBLE));
            break;

        case DDCB_SUPPORTAPM:           /* Support APM */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_SUPPORTAPM, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2APMCB,
                sizeof(usChild2APMCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            if(ulButtonChecked==TRUE)
                {
                ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ENABLEPOWERDOWN, BM_QUERYCHECK,
                    (MPARAM)NULL, (MPARAM)NULL);
                DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2PowerDownTimerCB,
                    sizeof(usChild2PowerDownTimerCB)/sizeof(USHORT),
                    (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
                }
            break;

        case DDCB_ENABLEPOWERDOWN:      /* Enable PowerDown timer */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ENABLEPOWERDOWN, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2PowerDownTimerCB,
                sizeof(usChild2PowerDownTimerCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDCB_ADVANCEDMARKING:      /* Advanced Marking options */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, DDCB_ADVANCEDMARKING, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_GENERAL].hwndNBPage, usChild2AdvancedMarkingCB,
                sizeof(usChild2AdvancedMarkingCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));

        case DDCB_TITLEBARICONS:        /* Titlebar Smarticons options */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_TITLEBAR].hwndNBPage, DDCB_TITLEBARICONS, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_TITLEBAR].hwndNBPage, usChild2TitlebarIconsCB,
                sizeof(usChild2TitlebarIconsCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDCB_ENABLELOCKUP:         /* Enable lockup password options */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, DDCB_ENABLELOCKUP, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            DisableDialogItem(nbDDNotebookPage[DD_PAGE_SECURITY].hwndNBPage, usChild2LockupTimerCB,
                sizeof(usChild2LockupTimerCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case DDCB_SPYFRAMEWINDOW:       /* Spy frame window option */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_SPY].hwndNBPage, DDCB_SPYFRAMEWINDOW, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
            if(ulButtonChecked)
                pHP->ulRuntimeFlag|=SPYFRAMEWINDOW;
            else
                pHP->ulRuntimeFlag&=(~SPYFRAMEWINDOW);
        }
/*                                                                                      *\
 * When selecting one the the Environment Spaces on the first Environment page, update  *
 * the second Environment page according to the selected entry in the listbox.          *
 * When the user doubleclicks on an Environment Space, also turn to the second page.    *
\*                                                                                      */
    if((SHORT1FROMMP(mp1)==DDLB_ENVIRONMENT) &&
        ((SHORT2FROMMP(mp1)==LN_SELECT) || (SHORT2FROMMP(mp1)==LN_ENTER)))
        {
        ULONG           ulSelection;
        ENVIRONMENTLIST *pEnvironmentListCurrent;

                                        /* Set into new contents according to the selected
                                           Environment Space */
        ulSelection=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT, LM_QUERYSELECTION,
            (MPARAM)LIT_FIRST, NULL);
        if(ulSelection!=(ULONG)LIT_NONE)
            {
                                        /* If one Environment Space is selected, find it
                                           and load its details to the second page */
                                        /* Clear old contents */
            WinSetWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDEF_ENVIRONMENT), "");
            WinSetWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDMLE_ENVIRONMENT), "");
                                        /* Enable the Remove, Change, Export and Import pushbutton */
            WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_REMOVEENVIRONMENT),
                TRUE);
            WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_CHANGEENVIRONMENT),
                TRUE);
            WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_2BATCHENVIRONMENT),
                TRUE);
            WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_EXPORTENVIRONMENT),
                TRUE);
            for(pEnvironmentListCurrent=pHP->pEnvironmentSpaces;
                pEnvironmentListCurrent!=NULL;
                pEnvironmentListCurrent=pEnvironmentListCurrent->pNext, ulSelection--)
                {
                if(ulSelection==0)
                    {
                    WinSetWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDEF_ENVIRONMENT),
                        pEnvironmentListCurrent->pEnvironmentName);
                    WinSetWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDMLE_ENVIRONMENT),
                        pEnvironmentListCurrent->pEnvironment);
                    }
                }
                                        /* If the user doubleclicked on the item, also turn to the second
                                           notebook page */
            if(SHORT2FROMMP(mp1)==LN_ENTER)
                {
                WinSendMsg(hwndNotebook, BKM_TURNTOPAGE, MPFROMLONG(nbDDNotebookPage[DD_PAGE_ENVIRON2].ulIDPage), NULL);
                }
            }
        else
            {
                                        /* If none Environment Space is selected, disable the pushbuttons */
            WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_REMOVEENVIRONMENT),
                FALSE);
            WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_CHANGEENVIRONMENT),
                FALSE);
            WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_2BATCHENVIRONMENT),
                FALSE);
            WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_EXPORTENVIRONMENT),
                FALSE);
            }
        }
/*                                                                                      *\
 * When the Sticky Windows MLE looses the input focus, verify that the strings are      *
 * unique                                                                               *
\*                                                                                      */
    if((SHORT1FROMMP(mp1)==DDMLE_STICKYWINDOWS) && (SHORT2FROMMP(mp1)==MLN_KILLFOCUS))
        {
        WinQueryDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDMLE_STICKYWINDOWS,
            sizeof(pHP->ucStickyWindows), pHP->ucStickyWindows);
        StripTrailing(pHP->ucStickyWindows);
        strcat(pHP->ucStickyWindows, "\r\n");
        EnsureUniqueStrings(pHP->ucStickyWindows);
        WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_OVERVIEW].hwndNBPage, DDMLE_STICKYWINDOWS, pHP->ucStickyWindows);
        }
#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld <- WM_CONTROL\n", __LINE__);
#endif  /* DEBUG_DIALOG */
    }
    break;

case WM_COMMAND:                        /* Button pressed */
#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld -> WM_COMMAND\n", __LINE__);
#endif  /* DEBUG_DIALOG */
    switch(SHORT1FROMMP(mp1))
    {
    case DDPB_IMPORTENVIRONMENT:
        {
        FILEDLG         fdFileDlg;      /* Standard file dialog control */
        HWND            hwndFileDlg;    /* Handle of standard file dialog */
        FILE            *pfileProfile;  /* Handle of file we load from */
                                        /* Pointer to first ENVIRONMENTLIST imported */
        ENVIRONMENTLIST *pEnvironmentListImport;
        ENVIRONMENTLIST *pEnvironmentListImportNext;
                                        /* Pointer to previous ENVIRONMENTLIST of existing list */
        ENVIRONMENTLIST *pEnvironmentListPrevious;
                                        /* Pointer to current ENVIRONMENTLIST of existing list */
        ENVIRONMENTLIST *pEnvironmentListCurrent;
                                        /* Index to insert imported ENVIRONMENTLIST alphabetically */
        ULONG           ulIndex;

                                        /* Clear out structure */
        memset(&fdFileDlg, 0, sizeof(FILEDLG));
        fdFileDlg.cbSize=sizeof(FILEDLG);
        fdFileDlg.fl=FDS_CENTER | FDS_OPEN_DIALOG;
        fdFileDlg.pszOKButton="~Load";
        fdFileDlg.pszTitle="PC/2 Import Environment";
        strcpy(fdFileDlg.szFullFile, pHP->ucPathDLL);
        strcat(fdFileDlg.szFullFile, "\\*.ENV");
                                        /* Open the standard file dialog ...*/
        hwndFileDlg=WinFileDlg(HWND_DESKTOP, hwndDlg, &fdFileDlg);
        if(hwndFileDlg && (fdFileDlg.lReturn==DID_OK))
            {
            if((pfileProfile=fopen(fdFileDlg.szFullFile, "r"))!=NULL)
                {
                                        /* Load the Environment Spaces by calling a procedure */
                pEnvironmentListImport=LoadEnvironmentSpaces(pfileProfile);
                                        /* Now insert the new Environment Space in alphabetic order */
                while(pEnvironmentListImport)
                    {
                                        /* Point to next as we insert current one into whole list */
                    pEnvironmentListImportNext=pEnvironmentListImport->pNext;
                                        /* Now find where to insert alphabetically */
                    for(pEnvironmentListCurrent=pHP->pEnvironmentSpaces, ulIndex=0,
                        pEnvironmentListPrevious=(pEnvironmentListCurrent ? pEnvironmentListCurrent->pBack : NULL);
                        ;
                        pEnvironmentListPrevious=pEnvironmentListCurrent,
                            pEnvironmentListCurrent=pEnvironmentListCurrent->pNext, ulIndex++)
                        {
                                        /* Now insert a new Environment Space */
                        if((pEnvironmentListCurrent==NULL) ||
                            (strcmp(pEnvironmentListCurrent->pEnvironmentName, pEnvironmentListImport->pEnvironmentName)>=0))
                            {
                            if(pEnvironmentListPrevious==NULL)
                                {
                                        /* We are either at the beginning of the LIST,
                                           or have an empty LIST */
                                if(pEnvironmentListCurrent==NULL)
                                    {
                                        /* We have an empty LIST */
                                    pHP->pEnvironmentSpaces=pEnvironmentListImport;
                                    pEnvironmentListImport->pBack=0;
                                    pEnvironmentListImport->pNext=0;
                                    }
                                else
                                    {
                                        /* We have to insert at the beginning of the LIST */
                                    pEnvironmentListImport->pBack=0;
                                    pEnvironmentListImport->pNext=pEnvironmentListCurrent;
                                    pEnvironmentListCurrent->pBack=pEnvironmentListImport;
                                    pHP->pEnvironmentSpaces=pEnvironmentListImport;
                                    }
                                }
                            else
                                {
                                        /* We are either in the middle of the LIST,
                                           or we have to insert the new element as the last one */
                                if(pEnvironmentListCurrent==NULL)
                                    {
                                        /* We have to insert at the end of the LIST */
                                    pEnvironmentListPrevious->pNext=pEnvironmentListImport;
                                    pEnvironmentListImport->pBack=pEnvironmentListPrevious;
                                    pEnvironmentListImport->pNext=0;
                                    }
                                else
                                    {
                                        /* We have to insert somewhere in the LIST */
                                    pEnvironmentListImport->pNext=pEnvironmentListPrevious->pNext;
                                    pEnvironmentListCurrent->pBack=pEnvironmentListImport;
                                    pEnvironmentListImport->pBack=pEnvironmentListPrevious;
                                    pEnvironmentListPrevious->pNext=pEnvironmentListImport;
                                    }
                                }
                                        /* Insert new Environment Space name into listbox */
                            WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT,
                                LM_INSERTITEM, MPFROMSHORT(ulIndex), MPFROMP(pEnvironmentListImport->pEnvironmentName));
                                            /* Clear old contents of second environment page */
                            WinSetWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDEF_ENVIRONMENT),
                                pEnvironmentListImport->pEnvironmentName);
                            WinSetWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDMLE_ENVIRONMENT),
                                pEnvironmentListImport->pEnvironment);
                                        /* Added current imported item, for next we have to begin
                                           at the start of the list to ensure alphabetically insertion */
                            break;
                            }
                        }
                                        /* Advance to next Environment Entry imported */
                    pEnvironmentListImport=pEnvironmentListImportNext;
                    }
                }
            fclose(pfileProfile);
            }
        }
        break;

    case DDPB_2BATCHENVIRONMENT:
    case DDPB_EXPORTENVIRONMENT:
        {
        FILEDLG         fdFileDlg;      /* Standard file dialog control */
        HWND            hwndFileDlg;    /* Handle of standard file dialog */
        FILE            *pfileProfile;  /* Handle of file we load from */
                                        /* Pointer to first ENVIRONMENTLIST exported */
        ENVIRONMENTLIST *pEnvironmentListExport;
        ENVIRONMENTLIST *pEnvironmentListExportPrevious;
        ENVIRONMENTLIST *pEnvironmentListExportNext;
                                        /* Pointer to current ENVIRONMENTLIST of existing list */
        ENVIRONMENTLIST *pEnvironmentListCurrent;
                                        /* Index to export ENVIRONMENTLIST alphabetically */
        ULONG           ulSelection;
        ULONG           ulIndex;

                                        /* Clear out structure */
        memset(&fdFileDlg, 0, sizeof(FILEDLG));
        fdFileDlg.cbSize=sizeof(FILEDLG);
        fdFileDlg.fl=FDS_CENTER | FDS_ENABLEFILELB | FDS_SAVEAS_DIALOG;
        fdFileDlg.pszOKButton="~Save";
        strcpy(fdFileDlg.szFullFile, pHP->ucPathDLL);
        if(SHORT1FROMMP(mp1)==DDPB_2BATCHENVIRONMENT)
            {
            fdFileDlg.pszTitle="PC/2 Export To Batchfile";
            strcat(fdFileDlg.szFullFile, "\\*.CMD");
            }
        else
            {
            fdFileDlg.pszTitle="PC/2 Export Environment";
            strcat(fdFileDlg.szFullFile, "\\*.ENV");
            }
                                        /* Open the standard file dialog ...*/
        hwndFileDlg=WinFileDlg(HWND_DESKTOP, hwndDlg, &fdFileDlg);
        if(hwndFileDlg && (fdFileDlg.lReturn==DID_OK))
            {
            if((pfileProfile=fopen(fdFileDlg.szFullFile, "w"))!=NULL)
                {
                                        /* Start with the first selection and loop for all selections */
                pEnvironmentListExport=pEnvironmentListExportPrevious=pEnvironmentListExportNext=0;
                ulSelection=LIT_FIRST;
                ulSelection=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT, LM_QUERYSELECTION,
                    (MPARAM)LIT_FIRST, NULL);
                                        /* Loop for all selections */
                while(ulSelection!=(ULONG)LIT_NONE)
                    {
                                        /* Find the entry of the selection, as the listbox corresponds to
                                           the linked list,  */
                    for(ulIndex=ulSelection, pEnvironmentListCurrent=pHP->pEnvironmentSpaces;
                        pEnvironmentListCurrent!=0 && ulIndex>0;
                        ulIndex--)
                        pEnvironmentListCurrent=pEnvironmentListCurrent->pNext;
                                        /* Now make a copy of the selected ENVIRONMENTLIST */
                    pEnvironmentListExportNext=malloc(sizeof(ENVIRONMENTLIST));
                    memset(pEnvironmentListExportNext, 0, sizeof(ENVIRONMENTLIST));
                    pEnvironmentListExportNext->pEnvironmentName=strdup(pEnvironmentListCurrent->pEnvironmentName);
                    pEnvironmentListExportNext->pEnvironment=strdup(pEnvironmentListCurrent->pEnvironment);
                    pEnvironmentListExportNext->pBack=pEnvironmentListExportPrevious;
                    if(pEnvironmentListExportPrevious)
                        pEnvironmentListExportPrevious->pNext=pEnvironmentListExportNext;
                    else
                        pEnvironmentListExport=pEnvironmentListExportPrevious=pEnvironmentListExportNext;
                                        /* Current Environment Space exported will be the previous one
                                           for next one being exported */
                    pEnvironmentListExportPrevious=pEnvironmentListExportNext;
                                        /* Find the next selected Environment Space */
                    ulSelection=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT, LM_QUERYSELECTION,
                        (MPARAM)ulSelection, NULL);
                    }
                                        /* Write the selected Environment Spaces by calling a procedure,
                                           free the (temporary) copy afterwards */
                if(pEnvironmentListExport!=NULL)
                    {
                    if(SHORT1FROMMP(mp1)==DDPB_2BATCHENVIRONMENT)
                        SaveEnvironmentSpaces(pEnvironmentListExport, pfileProfile, TRUE);
                    else
                        SaveEnvironmentSpaces(pEnvironmentListExport, pfileProfile, FALSE);
                    FreeEnvironmentSpaces(pEnvironmentListExport);
                    }
                }
            fclose(pfileProfile);
            }
        }
        break;

    case DDPB_ADDENVIRONMENT:
        {
        ENVIRONMENTLIST *pEnvironmentListCurrent;
        ENVIRONMENTLIST *pEnvironmentListNew;
        ENVIRONMENTLIST *pEnvironmentListPrevious;
        UCHAR           *pucEnvironmentName;
        ULONG           ulIndex;
        ULONG           ulLength;
        UCHAR           *pucEnvironment;

                                        /* Allocate space for the environment name the user entered,
                                           query it afterwards. If entryfield is empty ignore add
                                           request (the Add pushbutton should be disabled in this
                                           case anyway) */
        pucEnvironmentName=malloc(EF_SIZE255);
        if(pucEnvironmentName==NULL)
            break;
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDEF_ENVIRONMENT),
            EF_SIZE255, pucEnvironmentName);
        if(*pucEnvironmentName=='\0')
            {
            free(pucEnvironmentName);
            break;
            }
                                        /* Query the environment variables and ensure that they are
                                           terminated by \r\n\0 */
        ulLength=(ULONG)WinQueryWindowTextLength(WinWindowFromID(
            nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDMLE_ENVIRONMENT));
        ulLength+=3;
        pucEnvironment=malloc(ulLength);
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDMLE_ENVIRONMENT),
            ulLength, pucEnvironment);
        StripTrailing(pucEnvironment);
        strcat(pucEnvironment, "\r\n");
                                        /* Now insert the new Environment Space in alphabetic order */
        for(pEnvironmentListCurrent=pHP->pEnvironmentSpaces, ulIndex=0,
                pEnvironmentListPrevious=(pEnvironmentListCurrent ? pEnvironmentListCurrent->pBack : NULL);
            ;
            pEnvironmentListPrevious=pEnvironmentListCurrent,
                pEnvironmentListCurrent=pEnvironmentListCurrent->pNext, ulIndex++)
            {
                                        /* Now insert a new Environment Space */
            if((pEnvironmentListCurrent==NULL) ||
                (strcmp(pEnvironmentListCurrent->pEnvironmentName, pucEnvironmentName)>=0))
                {
                pEnvironmentListNew=malloc(sizeof(ENVIRONMENTLIST));
                memset(pEnvironmentListNew, 0, sizeof(ENVIRONMENTLIST));
                pEnvironmentListNew->pEnvironmentName=strdup(pucEnvironmentName);
                pEnvironmentListNew->pEnvironment=strdup(pucEnvironment);
                if(pEnvironmentListPrevious==NULL)
                    {
                                        /* We are either at the beginning of the LIST,
                                           or have an empty LIST */
                    if(pEnvironmentListCurrent==NULL)
                        {
                                        /* We have an empty LIST */
                        pHP->pEnvironmentSpaces=pEnvironmentListNew;
                        }
                    else
                        {
                                        /* We have to insert at the beginning of the LIST */
                        pEnvironmentListNew->pNext=pEnvironmentListCurrent;
                        pEnvironmentListCurrent->pBack=pEnvironmentListNew;
                        pHP->pEnvironmentSpaces=pEnvironmentListNew;
                        }
                    }
                else
                    {
                                        /* We are either in the middle of the LIST,
                                       or we have to insert the new element as the last one */
                    if(pEnvironmentListCurrent==NULL)
                        {
                                        /* We have to insert at the end of the LIST */
                        pEnvironmentListPrevious->pNext=pEnvironmentListNew;
                        pEnvironmentListNew->pBack=pEnvironmentListPrevious;
                        }
                    else
                        {
                                        /* We have to insert somewhere in the LIST */
                        pEnvironmentListNew->pNext=pEnvironmentListPrevious->pNext;
                        pEnvironmentListCurrent->pBack=pEnvironmentListNew;
                        pEnvironmentListNew->pBack=pEnvironmentListPrevious;
                        pEnvironmentListPrevious->pNext=pEnvironmentListNew;
                        }
                    }
                                        /* Insert new Environment Space name into listbox and
                                           disable the Add pushbutton (as there is no longer
                                           a Environment Space to add on the second page) */
                WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT,
                    LM_INSERTITEM, MPFROMSHORT(ulIndex), MPFROMP(pEnvironmentListNew->pEnvironmentName));
                WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_ADDENVIRONMENT),
                    FALSE);
                                        /* Clear old contents of second environment page */
                WinSetWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDEF_ENVIRONMENT), "");
                WinSetWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDMLE_ENVIRONMENT), "");
                                        /* We inserted one entry, so we're finished */
                break;
                }
                                        /* We have inserted somewhere in the LIST, so we can't get into
                                           an endless loop, because this insertion causes a break out
                                           of the loop */
            }
        free(pucEnvironmentName);
        free(pucEnvironment);
        }
        break;

    case DDPB_REMOVEENVIRONMENT:
        {
        ULONG           ulSelection;
        ENVIRONMENTLIST *pEnvironmentListCurrent;
        ENVIRONMENTLIST *pEnvironmentListNext;
        ENVIRONMENTLIST *pEnvironmentListPrevious;

                                        /* Enable the Add, Remove, Change and Export pushbutton */
        WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_ADDENVIRONMENT),
            FALSE);
        WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_REMOVEENVIRONMENT),
            FALSE);
        WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_CHANGEENVIRONMENT),
            FALSE);
        WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_2BATCHENVIRONMENT),
            FALSE);
        WinEnableWindow(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDPB_EXPORTENVIRONMENT),
            FALSE);
                                        /* Clear old contents of second environment page */
        WinSetWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDEF_ENVIRONMENT), "");
        WinSetWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDMLE_ENVIRONMENT), "");
                                        /* Find the selected Environment Space */
        ulSelection=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT, LM_QUERYSELECTION,
            (MPARAM)LIT_FIRST, NULL);
                                        /* If one Environment Space is selected, find it
                                           and remove it from the LIST */
        while(ulSelection!=(ULONG)LIT_NONE)
            {
                                        /* Delete the selected Menuitem from the source listbox (the
                                           listbox and the linked list correspond) */
            WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT, LM_DELETEITEM,
                MPFROMSHORT((USHORT)ulSelection), (MPARAM)NULL);
            for(pEnvironmentListCurrent=pHP->pEnvironmentSpaces;
                pEnvironmentListCurrent!=0;
                pEnvironmentListCurrent=pEnvironmentListCurrent->pNext, ulSelection--)
                {
                if(ulSelection==0)
                    {
                    free(pEnvironmentListCurrent->pEnvironmentName);
                    free(pEnvironmentListCurrent->pEnvironment);
                    pEnvironmentListNext=pEnvironmentListCurrent->pNext;
                    pEnvironmentListPrevious=pEnvironmentListCurrent->pBack;
                    if(pEnvironmentListPrevious!=NULL)
                        {
                                        /* Remove any but first element from the LIST */
                        pEnvironmentListPrevious->pNext=pEnvironmentListNext;
                        if(pEnvironmentListNext!=NULL)
                            pEnvironmentListNext->pBack=pEnvironmentListPrevious;
                        }
                    else
                        {
                                        /* Remove the first element from the LIST */
                        pHP->pEnvironmentSpaces=pEnvironmentListNext;
                        if(pEnvironmentListNext)
                            pEnvironmentListNext->pBack=NULL;
                        }
                                        /* Free the selected Environment Space and exit loop */
                    free(pEnvironmentListCurrent);
                    break;
                    }
                }
                                        /* Find the next selected Environment Space (again from the
                                           beginning, as the last selected one has been deleted */
            ulSelection=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT, LM_QUERYSELECTION,
                (MPARAM)LIT_FIRST, NULL);
            }
        }
        break;

    case DDPB_CHANGEENVIRONMENT:
        {
        UCHAR           *pucEnvironment;
        ULONG           ulLength;
        ULONG           ulSelection;
        ENVIRONMENTLIST *pEnvironmentListCurrent;

                                        /* Query the environment variables and ensure that they are
                                           terminated by \r\n\0 */
        ulLength=(ULONG)WinQueryWindowTextLength(WinWindowFromID(
            nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDMLE_ENVIRONMENT));
        ulLength+=3;
        pucEnvironment=malloc(ulLength);
        WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_ENVIRON2].hwndNBPage, DDMLE_ENVIRONMENT),
            ulLength, pucEnvironment);
        StripTrailing(pucEnvironment);
        strcat(pucEnvironment, "\r\n");
                                        /* Find the selected Environment Space */
        ulSelection=(ULONG)WinSendDlgItemMsg(nbDDNotebookPage[DD_PAGE_ENVIRON1].hwndNBPage, DDLB_ENVIRONMENT, LM_QUERYSELECTION,
            (MPARAM)LIT_FIRST, NULL);
                                        /* If one Environment Space is selected, find it
                                           and remove it change the environment variables */
        if(ulSelection!=(ULONG)LIT_NONE)
            {
            for(pEnvironmentListCurrent=pHP->pEnvironmentSpaces;
                pEnvironmentListCurrent!=NULL;
                pEnvironmentListCurrent=pEnvironmentListCurrent->pNext, ulSelection--)
                {
                if(ulSelection==0)
                    {
                    free(pEnvironmentListCurrent->pEnvironment);
                    pEnvironmentListCurrent->pEnvironment=pucEnvironment;
                    break;
                    }
                }
            }
        }
        break;

    case DDPB_ACCESSNET:                /* Initiate a network based update */
/*                                                                                      *\
 * The user selected the Update Now pushbutton from the Registration notebook page to   *
 * immediately request PC/2 to run the automatic update facility.                       *
\*                                                                                      */
        if(pHP->hwndNetThread!=NULLHANDLE)
            {
            LONG    lTemp;

                                        /* Get lastest data */
            if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDCB_SENDREGISTRATION))
                pHP->ulRegistrationFlag|=REGISTER_USER;
            else pHP->ulRegistrationFlag&=~REGISTER_USER;
            WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_FIRSTNAME),
                sizeof(pHP->ucUserFirstName), pHP->ucUserFirstName);
            WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_LASTNAME),
                sizeof(pHP->ucUserLastName), pHP->ucUserLastName);
            WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_EMAILADDRESS),
                sizeof(pHP->ucUserEmailAddress), pHP->ucUserEmailAddress);
            WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_PROXYURL),
                sizeof(pHP->ucProxyUrl), pHP->ucProxyUrl);
            WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_UPDATEURL),
                sizeof(pHP->ucUpdateUrl), pHP->ucUpdateUrl);
            WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_HTTPUSERID),
                sizeof(pHP->ucUserHttpUserid), pHP->ucUserHttpUserid);
            WinQueryWindowText(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDEF_HTTPPASSWORD),
                sizeof(ucDDUserHttpPassword), ucDDUserHttpPassword);
            for(lTemp=strlen(ucDDUserHttpPassword)-1; lTemp>=0; lTemp--)
                ucDDUserHttpPassword[lTemp]=(ucDDUserHttpPassword[lTemp]<<1)^0x3C;
            strcpy(pHP->ucUserHttpPassword, ucDDUserHttpPassword);
            if(WinQueryButtonCheckstate(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDCB_NETWORKSTATUS))
                pHP->ulRegistrationFlag|=REGISTER_SHOWSTATUS;
            else pHP->ulRegistrationFlag&=~REGISTER_SHOWSTATUS;
                                        /* Request Network thread to do work */
            WinPostMsg(pHP->hwndNetThread, WM_NETACCESS,
                MPFROMHWND(WinWindowFromID(nbDDNotebookPage[DD_PAGE_REGISTRATION].hwndNBPage, DDST_NETWORKSTATUS)), NULL);
            }
        break;


    case DDPB_WINFILEDLG:               /* Locate file by standard file dialog */
/*                                                                                      *\
 * The user selected the File Find pushbutton to get the standard file dialog to find   *
 * a background bitmap.                                                                 *
\*                                                                                      */
        {
        FILEDLG fdFileDlg;              /* Standard file dialog control */
        HWND    hwndFileDlg;            /* Handle of standard file dialog */

                                        /* Clear out structure */
        memset(&fdFileDlg, 0, sizeof(FILEDLG));
                                        /* Structure size */
        fdFileDlg.cbSize=sizeof(FILEDLG);
                                        /* FDS_* flags */
        fdFileDlg.fl=FDS_CENTER | FDS_OPEN_DIALOG;
                                        /* Dialog title string */
        fdFileDlg.pszTitle="PC/2 File Search";
                                        /* Initial path, filename or file filter */
        strcpy(fdFileDlg.szFullFile, "*.BMP");
                                        /* Open the standard file dialog ...*/
        hwndFileDlg=WinFileDlg(HWND_DESKTOP, hwndDlg, &fdFileDlg);
        if(hwndFileDlg && (fdFileDlg.lReturn==DID_OK))
                                        /* Load the values of the standard file dialog to
                                           the corresponding entryfield in the Desktop
                                           settings dialog */
            WinSetDlgItemText(nbDDNotebookPage[DD_PAGE_PHYSICAL].hwndNBPage, DDEF_BACKGROUNDBITMAP,
                fdFileDlg.szFullFile);
        }
        break;
    }
#ifdef  DEBUG_DIALOG
    printf("Dialog: %ld <- WM_COMMAND\n", __LINE__);
#endif  /* DEBUG_DIALOG */
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This procdure loads the user settings from the Scheduler notebook page.              *
 * Req:                                                                                 *
 *      pSchedulerData ........ Pointer to SCHEDULERDATA structure to put data into     *
 *      hwndNotebookPage ...... Window handle of notebook page where we read data from  *
 * Ret:                                                                                 *
 *      mresult ............... TRUE                                                    *
\*--------------------------------------------------------------------------------------*/
MRESULT GetSchedulerData(SCHEDULERDATA *pSchedulerData, HWND hwndNotebookPage)
{
ULONG           ulIndex;
ULONG           ulShiftMask;
ULONG           *pulMask;

                                        /* Clear */
memset(pSchedulerData, 0, sizeof(SCHEDULERDATA));
                                        /* Query scheduler status selection */
if(WinQueryButtonCheckstate(hwndNotebookPage, PIRB_DISABLESCHEDULER))
    pSchedulerData->ulSchedulerFlag=SCHEDULE_NONE;
if(WinQueryButtonCheckstate(hwndNotebookPage, PIRB_SCHEDULEHOURLY))
    pSchedulerData->ulSchedulerFlag=SCHEDULE_HOURLY;
if(WinQueryButtonCheckstate(hwndNotebookPage, PIRB_SCHEDULEDAILY))
    pSchedulerData->ulSchedulerFlag=SCHEDULE_DAILY;
if(WinQueryButtonCheckstate(hwndNotebookPage, PIRB_SCHEDULEWEEKLY))
    pSchedulerData->ulSchedulerFlag=SCHEDULE_WEEKLY;
if(WinQueryButtonCheckstate(hwndNotebookPage, PIRB_SCHEDULEMONTHLY))
    pSchedulerData->ulSchedulerFlag=SCHEDULE_MONTHLY;
if(WinQueryButtonCheckstate(hwndNotebookPage, PIRB_SCHEDULEONCE))
    pSchedulerData->ulSchedulerFlag=SCHEDULE_ONCE;
if(WinQueryButtonCheckstate(hwndNotebookPage, PICB_SCHEDULESAVEANYWAY))
    pSchedulerData->ulSchedulerFlag|=SCHEDULE_SAVEANYWAY;
                                        /* Query selected minute(s), be careful about the possible
                                           wrap arounds */
pulMask=&pSchedulerData->ulStartingMinute1;
for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=59; ulIndex++, ulShiftMask<<=1)
    {
    if(ulIndex==(sizeof(ULONG)*8-1))
        {
        ulShiftMask=0x00000001;
        pulMask=&pSchedulerData->ulStartingMinute2;
        }
                                        /* Note: If ulIndex is 0, ulIndex-1 equals MIT_FIRST */
    if(ulIndex==(ULONG)WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEMINUTE,
        LM_QUERYSELECTION, (MPARAM)(ulIndex-1), NULL))
        (*pulMask)|=ulShiftMask;
    }
                                        /* Query selected hour(s) */
for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=23; ulIndex++, ulShiftMask<<=1)
    {
    if(ulIndex==(ULONG)WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEHOUR,
        LM_QUERYSELECTION, (MPARAM)(ulIndex-1), NULL))
        pSchedulerData->ulStartingHour|=ulShiftMask;
    }
                                        /* Query selected weekday(s) */
for(ulIndex=0, ulShiftMask=0x00000001;
    ulIndex<(sizeof(pucWeekDay)/sizeof(pucWeekDay[0]));
    ulIndex++, ulShiftMask<<=1)
    {
    if(ulIndex==(ULONG)WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEWEEKDAY,
        LM_QUERYSELECTION, (MPARAM)(ulIndex-1), NULL))
        pSchedulerData->ulStartingWeekday|=ulShiftMask;
    }
                                        /* Query selected day(s), the first day (bit 0) is day 1 */
for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=30; ulIndex++, ulShiftMask<<=1)
    {
    if(ulIndex==(ULONG)WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEDAY,
        LM_QUERYSELECTION, (MPARAM)(ulIndex-1), NULL))
        pSchedulerData->ulStartingDay|=ulShiftMask;
    }
                                        /* Query selected month(s) */
for(ulIndex=0, ulShiftMask=0x00000001;
    ulIndex<(sizeof(pucMonth)/sizeof(pucMonth[0]));
    ulIndex++, ulShiftMask<<=1)
    {
    if(ulIndex==(ULONG)WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEMONTH,
        LM_QUERYSELECTION, (MPARAM)(ulIndex-1), NULL))
        pSchedulerData->ulStartingMonth|=ulShiftMask;
    }
return((MRESULT)TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * This procdure loads the user settings into the Scheduler notebook page.              *
 * Req:                                                                                 *
 *      pSchedulerData ........ Pointer to SCHEDULERDATA structure to get data from     *
 *      hwndNotebookPage ...... Window handle of notebook page where we put data into   *
 * Ret:                                                                                 *
 *      mresult ............... TRUE                                                    *
\*--------------------------------------------------------------------------------------*/
MRESULT SetSchedulerData(SCHEDULERDATA *pSchedulerData, HWND hwndNotebookPage)
{
ULONG   ulIndex;
ULONG   ulShiftMask;
UCHAR   ucBuffer[3];
ULONG   *pulMask;
ULONG   ulSchedule[]={ PIRB_DISABLESCHEDULER, PIRB_SCHEDULEHOURLY, PIRB_SCHEDULEDAILY,
                       PIRB_SCHEDULEWEEKLY, PIRB_SCHEDULEMONTHLY, PIRB_SCHEDULEONCE,
                       PICB_SCHEDULESAVEANYWAY };
ULONG   ulDateTime[]={ PILB_SCHEDULEMINUTE, PILB_SCHEDULEHOUR, PILB_SCHEDULEWEEKDAY,
                       PILB_SCHEDULEDAY, PILB_SCHEDULEMONTH };

                                        /* Initialize the selection radiobuttons and the save
                                           anyway checkbox */
if(pSchedulerData->ulSchedulerFlag==0)
    pSchedulerData->ulSchedulerFlag=SCHEDULE_NONE;
for(ulIndex=0, ulShiftMask=0x00000001;
    ulIndex<(sizeof(ulSchedule)/sizeof(ulSchedule[0]));
    ulIndex++, ulShiftMask<<=1)
    if(pSchedulerData->ulSchedulerFlag & ulShiftMask)
        {
        WinSendDlgItemMsg(hwndNotebookPage, ulSchedule[ulIndex], BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        WinSendMsg(hwndNotebookPage, WM_CONTROL,
            MPFROM2SHORT(ulSchedule[ulIndex], BN_CLICKED), (MPARAM)NULL);
        }
                                        /* Load minutes and select required one(s) */
pulMask=&pSchedulerData->ulStartingMinute1;
for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=59; ulIndex++, ulShiftMask<<=1)
    {
    if(ulIndex==(sizeof(ULONG)*8-1))
        {
        ulShiftMask=0x00000001;
        pulMask=&pSchedulerData->ulStartingMinute2;
        }
    WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEMINUTE,
        LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((CHAR *)_itoa(ulIndex, ucBuffer, 10)));
    if((*pulMask) & ulShiftMask)
        WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEMINUTE,
            LM_SELECTITEM, MPFROMSHORT(ulIndex), (MPARAM)TRUE);
    }
                                        /* Load hours and select required one(s) */
for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=23; ulIndex++, ulShiftMask<<=1)
    {
    WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEHOUR,
        LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((CHAR *)_itoa(ulIndex, ucBuffer, 10)));
    if(pSchedulerData->ulStartingHour & ulShiftMask)
        WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEHOUR,
            LM_SELECTITEM, MPFROMSHORT(ulIndex), (MPARAM)TRUE);
    }
                                        /* Load weekdays and select required one(s) */
for(ulIndex=0, ulShiftMask=0x00000001; ulIndex<=6; ulIndex++, ulShiftMask<<=1)
    {
    WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEWEEKDAY,
        LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(pucWeekDay[ulIndex]));
    if(pSchedulerData->ulStartingWeekday & ulShiftMask)
        WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEWEEKDAY,
            LM_SELECTITEM, MPFROMSHORT(ulIndex), (MPARAM)TRUE);
    }
                                        /* Load days and select required one(s) */
for(ulIndex=1, ulShiftMask=0x00000001; ulIndex<=31; ulIndex++, ulShiftMask<<=1)
    {
    WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEDAY,
        LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP((CHAR *)_itoa(ulIndex, (ucBuffer+1), 10)));
    if(pSchedulerData->ulStartingDay & ulShiftMask)
        WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEDAY,
            LM_SELECTITEM, MPFROMSHORT(ulIndex-1), (MPARAM)TRUE);
    }
                                        /* Load months and select required one(s) */
for(ulIndex=0, ulShiftMask=0x00000001;
    ulIndex<(sizeof(pucMonth)/sizeof(pucMonth[0]));
    ulIndex++, ulShiftMask<<=1)
    {
    WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEMONTH,
        LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(pucMonth[ulIndex]));
    if(pSchedulerData->ulStartingMonth & ulShiftMask)
        WinSendDlgItemMsg(hwndNotebookPage, PILB_SCHEDULEMONTH,
            LM_SELECTITEM, MPFROMSHORT(ulIndex), (MPARAM)TRUE);
    }
                                        /* Each listbox must have checked at least one entry
                                           otherwise just select the first (this forces the
                                           user to correct the selection, which avoids having
                                           nothing selected at all, in which case the Scheduler
                                           would never find a match */
for(ulIndex=0; ulIndex<(sizeof(ulDateTime)/sizeof(ulDateTime[0])); ulIndex++)
    {
    if((LONG)WinSendDlgItemMsg(hwndNotebookPage, ulDateTime[ulIndex],
        LM_QUERYSELECTION, (MPARAM)(LIT_FIRST), NULL)==LIT_NONE)
        WinSendDlgItemMsg(hwndNotebookPage, ulDateTime[ulIndex],
            LM_SELECTITEM, MPFROMSHORT(0), (MPARAM)TRUE);

    }
return((MRESULT)TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * This procdure disables or enables the child controls of the Scheduler notebook page  *
 * depending in which radiobutton is selected.                                          *
 * Req:                                                                                 *
 *      ulControl ............. Identifier of contol invoked during WM_COMMAND          *
 *      hwndNotebookPage ...... Window handle of notebook page where we put data into   *
 * Ret:                                                                                 *
 *      mresult ............... TRUE                                                    *
\*--------------------------------------------------------------------------------------*/
#define     SCHEDULERCHILDS         (PIRB_SCHEDULEONCE-PIRB_DISABLESCHEDULER+1)

MRESULT CheckSchedulerSettings(ULONG ulControl, HWND hwndNotebookPage)
{
                                        /* Number of child controls that may need to be disabled/enabled */
ULONG       ulChildControls=SCHEDULERCHILDS;
                                        /* Number of child controls to enable */
ULONG       ulControlsEnable=0;
ULONG       ulMaskEnable=0;
USHORT      usChildControlsEnable[SCHEDULERCHILDS];
                                        /* Number of child controls to disable */
ULONG       ulControlsDisable=0;
ULONG       ulMaskDisable=0;
USHORT      usChildControlsDisable[SCHEDULERCHILDS];
                                        /* Bitmapped mask of child controls that need to be disabled depending
                                           on the ordinally ordered and 0-based PIRB_DISABLESCHEDULER to
                                           PIRB_SCHEDULEONCE controls selected:
                                           PILB_SCHEDULEMINUTE     0x00000001
                                           PILB_SCHEDULEHOUR       0x00000002
                                           PILB_SCHEDULEWEEKDAY    0x00000004
                                           PILB_SCHEDULEDAY        0x00000008
                                           PILB_SCHEDULEMONTH      0x00000010 */
ULONG       ulVisibilityMask[SCHEDULERCHILDS]={ 0x0000001F, 0x0000001E, 0x0000001C,
                                                0x00000018, 0x00000014, 0x00000004 };
ULONG       ulTemp;

                                        /* Clear */
memset(usChildControlsEnable, 0, sizeof(usChildControlsEnable));
memset(usChildControlsDisable, 0, sizeof(usChildControlsDisable));
                                        /* Assume all controls to be enabled */
ulMaskEnable=ulVisibilityMask[PIRB_DISABLESCHEDULER-PIRB_DISABLESCHEDULER];
                                        /* Depending on the radiobutton selected, some controls may
                                           need to be disabled */
if(ulControl-PIRB_DISABLESCHEDULER<=ulChildControls)
    ulMaskDisable=ulVisibilityMask[ulControl-PIRB_DISABLESCHEDULER];
                                        /* Now remove all controls that should be disabled from the
                                           enabled mask */
ulMaskEnable&=(~ulMaskDisable);
                                        /* Now create the two arrays of the controls needing to be
                                           enabled and disabled */
for(ulTemp=0; ulTemp<=(PILB_SCHEDULEMONTH-PILB_SCHEDULEMINUTE); ulTemp++, ulMaskEnable>>=1)
    if(ulMaskEnable & 0x00000001)
        usChildControlsEnable[ulControlsEnable++]=(ulTemp+PILB_SCHEDULEMINUTE);
for(ulTemp=0; ulTemp<=(PILB_SCHEDULEMONTH-PILB_SCHEDULEMINUTE); ulTemp++, ulMaskDisable>>=1)
    if(ulMaskDisable & 0x00000001)
        usChildControlsDisable[ulControlsDisable++]=(ulTemp+PILB_SCHEDULEMINUTE);
                                        /* Now enable and disable the controls */
DisableDialogItem(hwndNotebookPage, usChildControlsEnable,
    (USHORT)ulControlsEnable, WS_VISIBLE);
DisableDialogItem(hwndNotebookPage, usChildControlsDisable,
    (USHORT)ulControlsDisable, WS_DISABLED);
return((MRESULT)TRUE);
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Program Installation dialog.                *
 * Req:                                                                                 *
 *      StartSession .. a structure of type SESSIONDATA where the information entered   *
 *                      in the dialog is entered. If DID_OK is pressed this structure   *
 *                      is used to start the session and set its priority.              *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY PI_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static HWND         hwndNB;             /* Program Installation dialog notebook window handle */
static SESSIONDATA  *pSessionData;      /* Pointer to SESSIONDATA structure for current instance */
static KEYDATA      SavedKeyData;       /* Hotkey for current instance, saved to be able to restore
                                           when user presses CANCEL (because this instance's Hotkey
                                           is deleted while this dialog is up as the user may choose
                                           to change it) */

switch(msg)
{
case WM_INITDLG:
    {
    ULONG       ulNBStyle;              /* Notebook style (to take the new Merlin notebook style into
                                           account, just try out SET NEWNOTEBOOS=YES in your Merlin
                                           beta CONFIG.SYS with your "old"-style notebooks) */
    USHORT      usTabTextLength=50;
    USHORT      usTabTextHeight=5;

                                        /* Access parameters passed */
    pSessionData=(SESSIONDATA *)PVOIDFROMMP(mp2);
                                        /* Save Hotkey to restore during CANCEL processing */
    SavedKeyData=pSessionData->KeyData;
    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
    ulNBStyle=WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE;
    if(((pHP->ulVersionMajor==OS2_MAJOR) && (pHP->ulVersionMinor>=OS2_MINOR_400)) ||
        (pHP->ulVersionMajor>OS2_MAJOR))
        ulNBStyle|=(BKS_TABBEDDIALOG|BKS_MAJORTABTOP|BKS_BACKPAGESTR|
            BKS_STATUSTEXTRIGHT|BKS_TABTEXTCENTER|BKS_POLYGONTABS);
    else
        ulNBStyle|=(BKS_SPIRALBIND|BKS_MAJORTABRIGHT|BKS_BACKPAGESBR|
            BKS_STATUSTEXTCENTER|BKS_TABTEXTRIGHT);
    hwndNB=WinCreateWindow(hwndDlg, WC_NOTEBOOK, "", ulNBStyle, 5, 0, 0, 0, hwndDlg,
        HWND_BOTTOM, PINB_NOTEBOOK, NULL, NULL);
                                        /* Subclass notebook's window procedure to add handling of
                                           notebook pages, notebook and outer dialog accelerator keys */
    DialogNotebookSubclass[PI_SUBCLASSEDNOTEBOOK].hwndNotebook=hwndNB;
    DialogNotebookSubclass[PI_SUBCLASSEDNOTEBOOK].pfnwpNotebook=
        WinSubclassWindow(hwndNB, SubclassedNotebookProcedure);
                                        /* Fill the notebook and return the tab text requirements */
    AddPagesToNotebook(hwndDlg, hwndNB, nbPINotebookPage, &usTabTextLength, &usTabTextHeight);
                                        /* Adjust dialog to optimally cover the notebook */
    AdjustNotebook(hwndDlg, hwndNB, ulNBStyle, usTabTextLength, usTabTextHeight);
                                        /* Now initialize all notebook page dialogs
                                           by posting to 1st page */
    WinPostMsg(nbPINotebookPage[PI_PAGE_1].hwndNBPage, WM_SETUPNOTEBOOKPAGES, MPFROMHWND(hwndNB), MPFROMP(pSessionData));
                                        /* Set focus */
    WinSetFocus(HWND_DESKTOP, nbPINotebookPage[PI_PAGE_1].ulIDFocus);
    return((MRESULT)TRUE);
    }

case WM_HELP:                           /* Help pressed */
    {
    ULONG   ulPageId, ulPage;
    ULONG   ulHelp[]={PIID_PAGE1, PIID_PAGE2, PIID_PAGE3, PIID_PAGE4, PIID_PAGE5, PIID_PAGE6, PIID_PAGE7};

    ulPageId=(ULONG)WinSendMsg(hwndNB,  /* Query the currently active page */
        BKM_QUERYPAGEID,
        MPFROMLONG(0),                  /* Location page ID, ignored if */
                                        /* Page ID is BKA_TOP */
        MPFROM2SHORT(BKA_TOP, BKA_MAJOR));
    for(ulPage=0; nbPINotebookPage[ulPage].pDialogWndProc!=NULL; ulPage++)
        if(nbPINotebookPage[ulPage].ulIDPage==ulPageId)
            if(pHP->hwndHelp!=NULLHANDLE)
                WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
                    MPFROMSHORT(ulHelp[ulPage]), HM_RESOURCEID);
    }
    break;

case WM_CHAR:
/*                                                                                      *\
 * All window procedure implementing a noteboook page and the notebook control itself,  *
 * which is part of this window's client, forward keystroke messages they don't have    *
 * handled themselfes here.                                                             *
\*                                                                                      */
    {
    static ULONG    ulRecursion=FALSE;

    return(DispatchKeyToNotebook(&ulRecursion, hwndDlg, hwndNB,
        DialogNotebookSubclass[PI_SUBCLASSEDNOTEBOOK].pfnwpNotebook, mp1, mp2));
    }

case WM_COMMAND:                        /* Button pressed */
                                        /* If the dialog will be dismissed, remove the subclassing
                                           of the notebook control within this dialog */
    if((SHORT1FROMMP(mp1)==DID_OK) || (SHORT1FROMMP(mp1)==DID_CANCEL))
        {
                                        /* Inform notebook page dialog procedure that we're closing down */
        WinSendMsg(nbPINotebookPage[PI_PAGE_1].hwndNBPage, WM_SETUPNOTEBOOKPAGES, MPFROMHWND(NULLHANDLE), (MPARAM)NULL);
        if((pHP->ulVersionMajor==OS2_MAJOR) && (pHP->ulVersionMinor<OS2_MINOR_300))
            {
                                        /* Workaround for OS/2 2.x bug, that causes that the MLE's
                                           return only 1 character per line they contain unless their
                                           notebook page was at least on top once. This is a 2.x defect,
                                           not a PC/2 bug! */
            ULONG       ulPage;

            for(ulPage=0; nbPINotebookPage[ulPage].pDialogWndProc!=NULL; ulPage++)
                WinSendMsg(WinWindowFromID(hwndDlg, PINB_NOTEBOOK), BKM_TURNTOPAGE, MPFROMLONG(nbPINotebookPage[ulPage].ulIDPage), NULL);
            }
        WinSubclassWindow(hwndNB, DialogNotebookSubclass[PI_SUBCLASSEDNOTEBOOK].pfnwpNotebook);
        DialogNotebookSubclass[PI_SUBCLASSEDNOTEBOOK].hwndNotebook=NULLHANDLE;
        DialogNotebookSubclass[PI_SUBCLASSEDNOTEBOOK].pfnwpNotebook=0;
                                        /* Also hide dialog */
        WinShowWindow(hwndDlg, FALSE);
        }
    switch(SHORT1FROMMP(mp1))
    {
    case DID_OK:                        /* Enter key pressed */
/*                                                                                      *\
 * Query the Program Title, Filename, Parameters and DOS Settings and copy them to the  *
 * corresponding entries in the StartSession structure.                                 *
\*                                                                                      */
        {
        ULONG   ulDosSettingsLength;
        ULONG   ulCharIndex;

        WinQueryWindowText(             /* Query data entered in Program title entry */
            WinWindowFromID(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_PROGRAMTITLE),
                                        /* into SessionData structure */
            sizeof(pSessionData->PgmTitle),
            pSessionData->PgmTitle);
        WinQueryWindowText(             /* Query data entered in Path and Filename entry */
            WinWindowFromID(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_PATHFILENAME),
                                        /* into SessionData structure */
            sizeof(pSessionData->PgmName),
            pSessionData->PgmName);
        WinQueryWindowText(             /* Query data entered in working Directory entry */
            WinWindowFromID(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_DIRECTORY),
                                        /* into SessionData structure */
            sizeof(pSessionData->PgmDirectory),
            pSessionData->PgmDirectory);
        ulCharIndex=strlen(pSessionData->PgmDirectory);
        if((ulCharIndex>0) && (pSessionData->PgmDirectory[ulCharIndex-1]!='\\'))
            {
            pSessionData->PgmDirectory[ulCharIndex++]='\\';
            pSessionData->PgmDirectory[ulCharIndex]='\0';
            }
        WinQueryWindowText(             /* Query data entered in Parameters entry */
            WinWindowFromID(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_PARAMETERS),
                                        /* into SessionData structure */
            sizeof(pSessionData->PgmInputs),
            pSessionData->PgmInputs);
        WinQueryWindowText(             /* Query data entered in Icon file entry */
            WinWindowFromID(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_ICONFILE),
                                        /* into SessionData structure */
            sizeof(pSessionData->IconFile),
            pSessionData->IconFile);
                                        /* Now get the variable length DOS settings */
                                        /* Allocate required length + space for \r\n\0 */
        ulDosSettingsLength=(ULONG)(WinQueryWindowTextLength(WinWindowFromID(nbPINotebookPage[PI_PAGE_1].hwndNBPage,
            PIMLE_DOSSETTINGS)))+3;
                                        /* Deallocate old and allocate new Environment settings */
        free(pSessionData->PgmDosSettings);
        pSessionData->PgmDosSettings=malloc(ulDosSettingsLength);
        WinQueryWindowText(             /* Query the DOS settings */
            WinWindowFromID(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIMLE_DOSSETTINGS),
            ulDosSettingsLength,
            pSessionData->PgmDosSettings);
                                        /* Remove all trailing \r and \n to add just 1 \r\n */
        StripTrailing(pSessionData->PgmDosSettings);
                                        /* If not empty string ("\0") add 1 \r\n */
        if(*pSessionData->PgmDosSettings!='\0')
            strcat(pSessionData->PgmDosSettings, "\r\n");
        }
/*                                                                                      *\
 * Query the Program Type radiobuttons and set the corresponding bit into the           *
 * StartSession structure. No change or the default selection are preselected during    *
 * WM_INITDLG. It must be one of them set, so we don't need else parts for if-else.     *
\*                                                                                      */
                                        /* Program type: WPS default */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_SHELL))
            pSessionData->SessionType=PROG_DEFAULT;
                                        /* Program type: OS/2 Fullscreen */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_OS2FULLSCREEN))
            pSessionData->SessionType=PROG_FULLSCREEN;
                                        /* Program type: OS/2 Window */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_OS2WINDOW))
            pSessionData->SessionType=PROG_WINDOWABLEVIO;
                                        /* Program type: PM */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_PM))
            pSessionData->SessionType=PROG_PM;
                                        /* Program type: DOS Fullscreen */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_DOSFULLSCREEN))
            pSessionData->SessionType=PROG_VDM;
                                        /* Program type: DOS Window */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_DOSWINDOW))
            pSessionData->SessionType=PROG_WINDOWEDVDM;
                                        /* Program type: WPS Object */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_WPSOBJECT))
            pSessionData->SessionType=PROG_WPSOBJECT;
                                        /* Program type: Detached */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_DETACHED))
            pSessionData->SessionType=PROG_DETACHED;
                                        /* Program type: WIN-OS2 session. Type can then be
                                           queried from WIN-OS2 session radiobuttons */
        if((WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_WINOS2SEAMLESS)) ||
            (WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_WINOS2FULLSCREEN)))
            {
            BOOL    bCommonVDM=TRUE;
            BOOL    bSeamlessWINOS2=TRUE;

            if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_WINOS2FULLSCREEN))
                bSeamlessWINOS2=FALSE;
            if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PICB_WINSEPARATE))
                bCommonVDM=FALSE;
            if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_WINREAL))
                pSessionData->SessionType=PROG_WINDOW_REAL;
            if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_WINSTANDARD))
                {
                if(bSeamlessWINOS2==TRUE)
                    pSessionData->SessionType=
                        (bCommonVDM==TRUE ? PROG_31_STDSEAMLESSCOMMON : PROG_31_STDSEAMLESSVDM);
                else
                    pSessionData->SessionType=PROG_31_STD;
                }
            if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_WIN386ENHANCED))
                {
                if(bSeamlessWINOS2==TRUE)
                    pSessionData->SessionType=
                        (bCommonVDM==TRUE ? PROG_31_ENHSEAMLESSCOMMON : PROG_31_ENHSEAMLESSVDM);
                else
                    pSessionData->SessionType=PROG_31_ENH;
                }
            }
/*                                                                                      *\
 * Query Program Style radiobuttons and set the corresponding bits in the StartSession  *
 * structure. No change or the default selection are preselected during WM_INITDLG.     *
\*                                                                                      */
        {
        UCHAR   ucBuffer[5];            /* 4 numbers, \0 */

                                        /* Program style: Maximized */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIRB_MAXIMIZED))
            pSessionData->PgmControl|=SSF_CONTROL_MAXIMIZE;
        else
            pSessionData->PgmControl&=(~SSF_CONTROL_MAXIMIZE);
                                        /* Program style: Minimized */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIRB_MINIMIZED))
            pSessionData->PgmControl|=SSF_CONTROL_MINIMIZE;
        else
            pSessionData->PgmControl&=(~SSF_CONTROL_MINIMIZE);
                                        /* Program style: Invisible */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PICB_INVISIBLE))
            pSessionData->PgmControl|=SSF_CONTROL_INVISIBLE;
        else
            pSessionData->PgmControl&=(~SSF_CONTROL_INVISIBLE);
                                        /* Program style: No autoclose */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PICB_NOAUTOCLOSE))
            pSessionData->PgmControl|=SSF_CONTROL_NOAUTOCLOSE;
        else
            pSessionData->PgmControl&=(~SSF_CONTROL_NOAUTOCLOSE);
                                        /* Program style: Background */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PICB_BACKGROUND))
            pSessionData->FgBg|=SSF_FGBG_BACK;
        else
            pSessionData->FgBg&=(~SSF_FGBG_BACK);
                                        /* Program style: Password Protect */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PICB_PASSWORDPROTECT))
            pSessionData->PgmControl|=SSF_CONTROL_PASSWORD;
        else
            pSessionData->PgmControl&=(~SSF_CONTROL_PASSWORD);
                                        /* Program style: Autostart */
                                        /* V2.20 changed to set SSF_CONTROL_AUTOSTARTED additionally
                                           to prevent current changes to cause Menuentry to autostart
                                           as that should really only happen after PC/2 is newly
                                           launched */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PICB_AUTOSTART))
            pSessionData->PgmControl|=(SSF_CONTROL_AUTOSTART|SSF_CONTROL_AUTOSTARTED);
        else
            pSessionData->PgmControl&=(~SSF_CONTROL_AUTOSTART);
        WinQueryWindowText(WinWindowFromID(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIEF_AUTOSTARTDELAY), sizeof(ucBuffer), ucBuffer);
        pSessionData->AutostartDelay=(LONG)atol(ucBuffer);
        }
/*                                                                                      *\
 * Query Size & Position radiobutton, set the corresponding bit, and load the values    *
 * from the corresponding entryfields for the StartSession structure.                   *
\*                                                                                      */
        {
        UCHAR   ucBuffer[6];            /* Prefix, 4 numbers, \0 */

        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PICB_SIZEPOSITION))
            pSessionData->PgmControl|=SSF_CONTROL_SETPOS;
        else
            pSessionData->PgmControl&=(~SSF_CONTROL_SETPOS);
        WinQueryWindowText(WinWindowFromID(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIEF_X), sizeof(ucBuffer), ucBuffer);
        pSessionData->InitXPos=(SHORT)atol(ucBuffer);
        if(pSessionData->InitXPos<pHP->VirtualDesktopMin.x)
            pSessionData->InitXPos=pHP->VirtualDesktopMin.x;
        if(pSessionData->InitXPos>2*pHP->VirtualDesktopMax.x)
            pSessionData->InitXPos=pHP->VirtualDesktopMax.x;
        WinQueryWindowText(WinWindowFromID(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIEF_Y), sizeof(ucBuffer), ucBuffer);
        pSessionData->InitYPos=(SHORT)atol(ucBuffer);
        if(pSessionData->InitYPos<pHP->VirtualDesktopMin.y)
            pSessionData->InitYPos=pHP->VirtualDesktopMin.y;
        if(pSessionData->InitYPos>2*pHP->VirtualDesktopMax.y)
            pSessionData->InitYPos=pHP->VirtualDesktopMax.y;
        WinQueryWindowText(WinWindowFromID(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIEF_XSIZE), sizeof(ucBuffer), ucBuffer);
        pSessionData->InitXSize=(USHORT)atol(ucBuffer);
        if(pSessionData->InitXSize<0) pSessionData->InitXSize*=-1;
        WinQueryWindowText(WinWindowFromID(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIEF_YSIZE), sizeof(ucBuffer), ucBuffer);
        pSessionData->InitYSize=(USHORT)atol(ucBuffer);
        if(pSessionData->InitYSize<0) pSessionData->InitYSize*=-1;
        }
/*                                                                                      *\
 * Query Hotkey settings notebook page                                                  *
\*                                                                                      */
        {
        UCHAR   ucBuffer[]=" ";

        pSessionData->SwpFlag=0;        /* Assume no flags, which will now be corrected */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PIRB_WSMAXIMIZED))
            pSessionData->SwpFlag=SWP_MAXIMIZE;
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PIRB_WSRESTORE))
            pSessionData->SwpFlag=SWP_RESTORE;
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_DONTMOVE))
            pSessionData->SwpFlag|=SWP_NOMOVE;
        pSessionData->KeyData.usFlags=0;
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_RCTRL))
            pSessionData->KeyData.usFlags|=KC_RCTRL;
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_CTRL))
            pSessionData->KeyData.usFlags|=KC_CTRL;
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_ALT))
            pSessionData->KeyData.usFlags|=KC_ALT;
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_LWIN))
            pSessionData->KeyData.usFlags|=KC_LWIN;
        WinQueryWindowText(WinWindowFromID(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICBX_HOTKEY),
            sizeof(ucBuffer), ucBuffer);
                                        /* If no hotkey is contained in the hotkey combobox
                                           none is available or it is disabled ignore hotkey feature */
        if(!strcmp(ucBuffer, "") ||
            !WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_HOTKEYENABLED))
            {
            pSessionData->KeyData.usFlags=0;
            pSessionData->KeyData.usCh=0;
            }
        else                            /* Mark the selected key as used */
            {
            KEYDATA     *pKeyDataIndex;
            ULONG       ulTemp1;

            pSessionData->KeyData.usCh=(USHORT)ucBuffer[0];
                                        /* Point to Hotkey control structure */
            pKeyDataIndex=pHP->pKeyData;
                                        /* Loop for all keys and set used one */
            for(ulTemp1=0; ulTemp1<KEYDATACOUNT; pKeyDataIndex++, ulTemp1++)
                {
                                        /* Set current key to used */
                if((pKeyDataIndex->usCh==pSessionData->KeyData.usCh)  &&
                    (pKeyDataIndex->usFlags==pSessionData->KeyData.usFlags))
                    {
                    pKeyDataIndex->bUsed=TRUE;
                    pKeyDataIndex->pMenuData=pSessionData->KeyData.pMenuData;
                    }
                }
            }
                                        /* In order to reduce the chances of accidentially identifying
                                           a window because the session name matches, configure the
                                           session name field only automatically, when the window is
                                           marked to be kept on active Desktop or having defined a Hotkey.
                                           Unfortunately, moving the window to a user defined position
        if((pSessionData->SwpFlag & SWP_NOMOVE) || (pSessionData->KeyData.usCh!=0))
                                           does no longer work, because the window can only be found by its
                                           session name. I'm not sure if the Window List contains all sessions,
                                           even when no FCF_WINDOWLIST is specified, so that we can use the
                                           session id instead */
                                        /* Query the name of the session to search in Window List
                                           or in titlebars. If nothing is entered take the Program
                                           Title entryfield text, but ensure that tildes (~) are
                                           removed, because the session is launched with tiles
                                           removed. The tildes are only used to define mnemonics
                                           within PC/2's Popup-Menu. */
        WinQueryWindowText(WinWindowFromID(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PIEF_SESSIONTITLE),
            sizeof(pSessionData->WindowTitle), pSessionData->WindowTitle);
        if(!strcmp(pSessionData->WindowTitle, ""))
            {
            UCHAR   *pucTo, *pucFrom;

            for(pucFrom=pSessionData->PgmTitle, pucTo=pSessionData->WindowTitle; *pucFrom; pucFrom++)
                if(*pucFrom!='~') *pucTo++=*pucFrom;
            *pucTo='\0';                /* Zero termination */
            }
        }
/*                                                                                      *\
 * Query Program Priority radiobuttons and set the corresponding bits in the            *
 * StartSession structure. The default priority is No-Change. The listbox is also       *
 * queried and the Delta Priority is also set in the StartSession structure. It must be *
 * one of the radiobuttons, so else part of if-else is not required.                    *
\*                                                                                      */
        {
        UCHAR   ucBuffer[3];            /* 2 numbers, \0 */
                                        /* Program Priority: No-Change */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIRB_NOCHANGE))
            pSessionData->PriorityClass=PRTYC_NOCHANGE;
                                        /* Program Priority: Idle-time */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIRB_IDLETIME))
            pSessionData->PriorityClass=PRTYC_IDLETIME;
                                        /* Program Priority: Regular */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIRB_REGULAR))
            pSessionData->PriorityClass=PRTYC_REGULAR;
                                        /* Program Priority: Time-critical */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIRB_TIMECRITICAL))
            pSessionData->PriorityClass=PRTYC_TIMECRITICAL;
                                        /* Program Priority: Server */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIRB_FOREGROUNDSERVER))
            pSessionData->PriorityClass=PRTYC_FOREGROUNDSERVER;
                                        /* Query index of selected item on correct it */
        WinQueryWindowText(WinWindowFromID(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIEF_PRIORITYDELTA),
            sizeof(ucBuffer), ucBuffer);
        pSessionData->PriorityDelta=(SHORT)atol(ucBuffer);
        }
/*                                                                                      *\
 * Query Program Environment and replace the one currently set at pSessiontData->       *
 * Pgm(Leading/Trailing)PgmEnvironment. The environment is used to complete/replace     *
 * part of the Environment of PC/2 when the application is started. The environment may *
 * also contain BEGINLIBPATH and/or ENDLIBPATH which is not an environment variable,    *
 * but treated the same way (available only for OS/2 WARP 3.00+.                        *
\*                                                                                      */
        {
        UCHAR           *pucEnvironment=NULL;
        ULONG           ulPgmEnvironmentLength;

                                        /* Query the selection of the Startup Environment Spaces dialog */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_6].hwndNBPage, PICB_STARTUPENVIRONMENT))
            pSessionData->StatusFlag|=STARTUPENVIRONMENTS;
        else
            pSessionData->StatusFlag&=(~STARTUPENVIRONMENTS);
                                        /* Deallocate old Environment Spaces, because its
                                           easier to collect them new than to update */
        free(pSessionData->EnvironmentSpaces);
        pSessionData->EnvironmentSpaces=strdup("");
        pSessionData->EnvironmentSpaces=QueryEnvironmentSpaces(pSessionData->EnvironmentSpaces,
            nbPINotebookPage[PI_PAGE_6].hwndNBPage, PILB_ENVIRONMENTS);
                                        /* Allocate required length + space for \r\n\0 */
        ulPgmEnvironmentLength=(ULONG)(WinQueryWindowTextLength(WinWindowFromID(nbPINotebookPage[PI_PAGE_6].hwndNBPage,
            PIMLE_ENVIRONMENT)))+3;
                                        /* Deallocate old and allocate new Environment settings */
        free(pucEnvironment);
        pucEnvironment=malloc(ulPgmEnvironmentLength);
        WinQueryWindowText(             /* Query the environment settings */
            WinWindowFromID(nbPINotebookPage[PI_PAGE_6].hwndNBPage, PIMLE_ENVIRONMENT),
            ulPgmEnvironmentLength,
            pucEnvironment);
                                        /* Remove all trailing \r and \n to add just 1 \r\n */
        StripTrailing(pucEnvironment);
                                        /* If not empty string ("\0") add 1 \r\n */
        if(*pucEnvironment!='\0')
            strcat(pucEnvironment, "\r\n");
                                        /* Check if the current environment MLE contains the leading or
                                           trailing environment settings */
        if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_6].hwndNBPage, PIRB_LEADENVIRONMENT))
            {
            free(pSessionData->PgmLeadingEnvironment);
            pSessionData->PgmLeadingEnvironment=pucEnvironment;
            }
        else
            {
            free(pSessionData->PgmTrailingEnvironment);
            pSessionData->PgmTrailingEnvironment=pucEnvironment;
            }
        }
/*                                                                                      *\
 * Query Program Scheduler data for the information about automatic scheduling of the   *
 * menuentry.                                                                           *
\*                                                                                      */
        GetSchedulerData(&pSessionData->SDScheduler, nbPINotebookPage[PI_PAGE_7].hwndNBPage);
                                        /* Dialog terminated with DID_OK */
        WinDismissDlg(hwndDlg, DID_OK);
        break;

    case DID_CANCEL:                    /* Escape or Cancel pressed */
                                        /* Mark the selected key as used */
        {
        KEYDATA     *pKeyDataIndex;
        ULONG       ulTemp1;

                                    /* Point to Hotkey control structure */
        pKeyDataIndex=pHP->pKeyData;
                                    /* Loop for all keys and set used one */
        for(ulTemp1=0; ulTemp1<KEYDATACOUNT; pKeyDataIndex++, ulTemp1++)
            {
                                    /* Set original key to used */
            if((pKeyDataIndex->usCh==SavedKeyData.usCh)  &&
                (pKeyDataIndex->usFlags==SavedKeyData.usFlags))
                {
                pKeyDataIndex->bUsed=TRUE;
                pKeyDataIndex->pMenuData=pSessionData->KeyData.pMenuData;
                }
            }
        }
                                        /* Dialog terminated with DID_CANCEL */
        WinDismissDlg(hwndDlg, DID_CANCEL);
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Program Installation dialog notebook pages. *
 * Req:                                                                                 *
 *      nbPINotebookPage[] ...  Structure of the Program Installation notebook pages    *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY PI_NotebookProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Notebook where this window procedure implements notebook pages */
static HWND         hwndNotebook=NULLHANDLE;
static SESSIONDATA  *pSessionData;      /* Pointer to SESSIONDATA structure for current instance */
static SHORT        sPriorityDelta=PRTYC_NOCHANGE;
static LONG         lAutostartDelay=0;
                                        /* This flag reflects the state which environment autoradiobutton
                                           (PIRB_LEADENVIRONMENT or PIRB_TRAILENVIRONMENT) was the last one
                                           selected */
static ULONG        ulLastEnvironmentButton=0;
                                        /* Child Autostart checkbox */
static USHORT   usChild2AutostartCB[]={PIHSB_AUTOSTARTDELAY, PIEF_AUTOSTARTDELAY};
                                        /* Child windows that change witch Hotkey enable checkbox */
static USHORT   usChild2HotKeyCB[]={PIRB_NONE, PIRB_WSMAXIMIZED, PIRB_WSRESTORE,
                                    PICB_RCTRL, PICB_CTRL, PICB_ALT, PICB_LWIN, PICBX_HOTKEY};

                                        /* During WinLoadDialog(), PM sends curious WM_CONTROL
                                           messages which causes us troubles as that message
                                           may have consequences on not yet loaded notebook
                                           pages, symptoms were traps in WinWindowFromID()
                                           during DisableDialogItem() */
if((msg!=WM_SETUPNOTEBOOKPAGES) && (hwndNotebook==NULLHANDLE))
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
switch(msg)
{
/*                                                                                      *\
 * Syntax: WM_SETUPNOTEBOOKPAGES, (HWND hwndNotebook), (SESSIONDATA *pSessionData)      *                                           *
\*                                                                                      */
case WM_SETUPNOTEBOOKPAGES:
    {
    UCHAR               ucBuffer[5];

    hwndNotebook=HWNDFROMMP(mp1);       /* Save notebook we're pages in */
    if(hwndNotebook==NULLHANDLE)
        break;
/*                                                                                      *\
 * Set the maximum number of chars accepted from the entryfield (thus overwriting the   *
 * default number of 63 and load the data from the SESSIONDATA structure SessionData    *
 * into the entryfields.                                                                *
\*                                                                                      */
                                        /* Access parameters passed */
    pSessionData=(SESSIONDATA *)PVOIDFROMMP(mp2);
    WinSendDlgItemMsg(                  /* Send message to dialog window */
                                        /* Handle of page 1 */
        nbPINotebookPage[PI_PAGE_1].hwndNBPage,
        PIEF_PROGRAMTITLE,              /* Program title entryfield */
        EM_SETTEXTLIMIT,                /* Set text limit to 60 */
        MPFROMSHORT(MAXNAMEL),
        (MPARAM)NULL);                  /* No additional parameter */
    WinSetDlgItemText(                  /* Load the default text of the entryfield */
        nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_PROGRAMTITLE, pSessionData->PgmTitle);
    WinSendDlgItemMsg(
        nbPINotebookPage[PI_PAGE_1].hwndNBPage,
        PIEF_PATHFILENAME,              /* Path and Filename title entryfield */
        EM_SETTEXTLIMIT,                /* Set text limit to 255 */
        MPFROMSHORT(CCHMAXPATHCOMP-1),
        (MPARAM)NULL);
    WinSetDlgItemText(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_PATHFILENAME, pSessionData->PgmName);
    WinSendDlgItemMsg(
        nbPINotebookPage[PI_PAGE_1].hwndNBPage,
        PIEF_DIRECTORY,                 /* Working directory entryfield */
        EM_SETTEXTLIMIT,                /* Set text limit to 255 */
        MPFROMSHORT(CCHMAXPATHCOMP-1),
        (MPARAM)NULL);
    WinSetDlgItemText(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_DIRECTORY, pSessionData->PgmDirectory);
    WinSendDlgItemMsg(
        nbPINotebookPage[PI_PAGE_1].hwndNBPage,
        PIEF_PARAMETERS,                /* Program Parameters entryfield */
        EM_SETTEXTLIMIT,                /* Set text limit to 255 */
        MPFROMSHORT(EF_SIZE255),
        (MPARAM)NULL);
    WinSetDlgItemText(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_PARAMETERS, pSessionData->PgmInputs);
    WinSendDlgItemMsg(                  /* Send message to dialog window */
                                        /* Handle of page 1 */
        nbPINotebookPage[PI_PAGE_1].hwndNBPage,
        PIEF_PROGRAMTITLE,              /* Icon file entryfield */
        EM_SETTEXTLIMIT,                /* Set text limit to 260 */
        MPFROMSHORT(CCHMAXPATH),
        (MPARAM)NULL);                  /* No additional parameter */
    WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_ICONFILE,
        EM_SETTEXTLIMIT, MPFROMSHORT(CCHMAXPATH), (MPARAM)NULL);
    WinSetDlgItemText(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_ICONFILE, pSessionData->IconFile);
                                        /* Insert DOS Settings. */
    WinSendDlgItemMsg(
        nbPINotebookPage[PI_PAGE_1].hwndNBPage,
        PIMLE_DOSSETTINGS,              /* Program DOS Settings entryfield */
        MLM_INSERT,                     /* Insert text */
        MPFROMP(pSessionData->PgmDosSettings),
        (MPARAM)NULL);
/*                                                                                      *\
 * Now we preselect the radiobutton in the Program type group to the state indicated by *
 * the SessionData structure.                                                           *
\*                                                                                      */
    WinSendMsg(hwndDlg, WM_SETUPPROGRAMTYPE,
        MPFROMSHORT(pSessionData->SessionType), (MPARAM)NULL);
/*                                                                                      *\
 * Now we preselect the radiobutton in the Program style group to the state indicated   *
 * by the SessionData structure.                                                        *
\*                                                                                      */
    if(!(pSessionData->PgmControl & (SSF_CONTROL_MAXIMIZE | SSF_CONTROL_MINIMIZE)))
        WinSendDlgItemMsg(              /* Send message to Default radiobutton */
                                        /* Handle of dialog window */
            nbPINotebookPage[PI_PAGE_3].hwndNBPage,
            PIRB_DEFAULTSTYLE,          /* Program style: Default radiobutton */
            BM_SETCHECK,                /* Set it to pressed */
            MPFROMSHORT(TRUE),
            (MPARAM)NULL);
    if(pSessionData->PgmControl & SSF_CONTROL_MAXIMIZE)
        WinSendDlgItemMsg(
            nbPINotebookPage[PI_PAGE_3].hwndNBPage,
            PIRB_MAXIMIZED,             /* Program style: Maximized radiobutton */
            BM_SETCHECK,                /* Set it to pressed */
            MPFROMSHORT(TRUE),
            (MPARAM)NULL);
    if(pSessionData->PgmControl & SSF_CONTROL_MINIMIZE)
        WinSendDlgItemMsg(
            nbPINotebookPage[PI_PAGE_3].hwndNBPage,
            PIRB_MINIMIZED,             /* Program style: Minimized radiobutton */
            BM_SETCHECK,                /* Set it to pressed */
            MPFROMSHORT(TRUE),
            (MPARAM)NULL);
    if(pSessionData->PgmControl & SSF_CONTROL_INVISIBLE)
        WinSendDlgItemMsg(
            nbPINotebookPage[PI_PAGE_3].hwndNBPage,
            PICB_INVISIBLE,             /* Program style: Invisible checkbox */
            BM_SETCHECK,                /* Set it to pressed */
            MPFROMSHORT(TRUE),
            (MPARAM)NULL);
    if(pSessionData->PgmControl & SSF_CONTROL_NOAUTOCLOSE)
        WinSendDlgItemMsg(
            nbPINotebookPage[PI_PAGE_3].hwndNBPage,
            PICB_NOAUTOCLOSE,           /* Program style: NoAutoClose checkbox */
            BM_SETCHECK,                /* Set it to pressed */
            MPFROMSHORT(TRUE),
            (MPARAM)NULL);
    if(pSessionData->PgmControl & SSF_CONTROL_AUTOSTART)
        WinSendDlgItemMsg(
            nbPINotebookPage[PI_PAGE_3].hwndNBPage,
            PICB_AUTOSTART,             /* Program style: Autostart checkbox */
            BM_SETCHECK,                /* Set it to pressed */
            MPFROMSHORT(TRUE),
            (MPARAM)NULL);
        else
            DisableDialogItem(nbPINotebookPage[PI_PAGE_3].hwndNBPage, usChild2AutostartCB,
                sizeof(usChild2AutostartCB)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Setup delay scrollbar between 0 and 3600 s in 5s steps,
                                           which results in steps from 0 to 720 for the scrollbar */
    WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIHSB_AUTOSTARTDELAY,
        SBM_SETTHUMBSIZE, MPFROM2SHORT(1, AUTOSTARTDELAY_MAX/AUTOSTARTDELAY_SEC), (MPARAM)NULL);
    lAutostartDelay=pSessionData->AutostartDelay;
#ifdef  DEBUG
    if((lAutostartDelay<AUTOSTARTDELAY_MIN) || (lAutostartDelay>AUTOSTARTDELAY_MAX))
        {
        printf("Dialog: %ld Fatal error, lAutostartDelay %ld\n", __LINE__, lAutostartDelay);
        DosBeep(1500, 2500);
        lAutostartDelay=AUTOSTARTDELAY_MIN;
        }
#endif  /* DEBUG */
    WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIHSB_AUTOSTARTDELAY,
        SBM_SETSCROLLBAR, MPFROMSHORT(lAutostartDelay/AUTOSTARTDELAY_SEC),
        MPFROM2SHORT(0, AUTOSTARTDELAY_MAX/AUTOSTARTDELAY_SEC));
                                        /* Insert the string */
    WinSetDlgItemText(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIEF_AUTOSTARTDELAY,
        (CHAR *)_itoa(lAutostartDelay, ucBuffer, 10));
    if(pSessionData->FgBg & SSF_FGBG_BACK)
        WinSendDlgItemMsg(
            nbPINotebookPage[PI_PAGE_3].hwndNBPage,
            PICB_BACKGROUND,            /* Program style: Background checkbox */
            BM_SETCHECK,                /* Set it to pressed */
            MPFROMSHORT(TRUE),
            (MPARAM)NULL);
    if(pSessionData->PgmControl & SSF_CONTROL_PASSWORD)
        WinSendDlgItemMsg(
            nbPINotebookPage[PI_PAGE_3].hwndNBPage,
            PICB_PASSWORDPROTECT,       /* Program style: Password Protect checkbox */
            BM_SETCHECK,                /* Set it to pressed */
            MPFROMSHORT(TRUE),
            (MPARAM)NULL);
/*                                                                                      *\
 * Now we preselect the radiobutton in the Program Size & Position group to the state   *
 * indicated by the SessionData structure.                                              *
\*                                                                                      */
    if(pSessionData->PgmControl & SSF_CONTROL_SETPOS)
        {                               /* If requested select it for the first time */
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PICB_SIZEPOSITION, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        WinSendMsg(hwndDlg, WM_SETUPSIZEPOSITION,
            MPFROMSHORT(TRUE), MPFROMSHORT(TRUE));
        }
    else
        WinSendMsg(hwndDlg, WM_SETUPSIZEPOSITION,
            MPFROMSHORT(FALSE), MPFROMSHORT(TRUE));
    }
                                        /* Simulate the click to update the dialog's child windows */
    WinSendMsg(nbPINotebookPage[PI_PAGE_3].hwndNBPage, WM_CONTROL,
        MPFROM2SHORT(PICB_SIZEPOSITION, BN_CLICKED), (MPARAM)NULL);
/*                                                                                      *\
 * Now we initialize the priority settings notebook page.                               *
\*                                                                                      */
     WinSendMsg(hwndDlg, WM_SETUPPROGRAMPRIORITY,
        MPFROMSHORT(pSessionData->PriorityClass), MPFROMSHORT(pSessionData->PriorityDelta));
/*                                                                                      *\
 * Now we initialize the hotkey settings notebook page.                                 *
\*                                                                                      */
     WinSendMsg(hwndDlg, WM_SETUPHOTKEY, (MPARAM)NULL, (MPARAM)NULL);
/*                                                                                      *\
 * Now we initialize the Environment settings notebook page.                            *
\*                                                                                      */
    {
    ENVIRONMENTLIST *pEnvironmentListCurrent;
    ULONG           ulIndex;

                                        /* If requested select Startup Environment Spaces dialog */
    if(pSessionData->StatusFlag & STARTUPENVIRONMENTS)
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_6].hwndNBPage, PICB_STARTUPENVIRONMENT, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Load available environment spaces sequentially */
    for(pEnvironmentListCurrent=pHP->pEnvironmentSpaces;
        pEnvironmentListCurrent!=NULL;
        pEnvironmentListCurrent=pEnvironmentListCurrent->pNext)
        {
        ulIndex=(ULONG)WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_6].hwndNBPage, PILB_ENVIRONMENTS,
            LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(pEnvironmentListCurrent->pEnvironmentName));
        if(strstr(pSessionData->EnvironmentSpaces, pEnvironmentListCurrent->pEnvironmentName))
            WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_6].hwndNBPage, PILB_ENVIRONMENTS,
                LM_SELECTITEM, MPFROMSHORT(ulIndex), (MPARAM)TRUE);
        }
                                        /* Check leading environment settings radiobutton and reset
                                           the radiobutton state flag to 0 because no radiobutton was
                                           pressed yet */
    ulLastEnvironmentButton=0;
    WinSendDlgItemMsg(
        nbPINotebookPage[PI_PAGE_6].hwndNBPage,
        PIRB_LEADENVIRONMENT,           /* Program environment: Leading environment */
        BM_SETCHECK,                    /* Set it to pressed */
        MPFROMSHORT(TRUE),
        (MPARAM)NULL);
                                        /* Simulate the click to update the MLE containing either the
                                           leading or trailing environment settings */
    WinSendMsg(nbPINotebookPage[PI_PAGE_6].hwndNBPage, WM_CONTROL,
        MPFROM2SHORT(PIRB_LEADENVIRONMENT, BN_CLICKED), (MPARAM)NULL);
/*                                                                                      *\
 * Now we initialize the Scheduler settings notebook page.                              *
\*                                                                                      */
    SetSchedulerData(&pSessionData->SDScheduler, nbPINotebookPage[PI_PAGE_7].hwndNBPage);
                                        /* All pages initialized */
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SETUPPROGRAMTYPE, USHORT SessionType, NULL                                *
\*                                                                                      */
case WM_SETUPPROGRAMTYPE:
/*                                                                                      *\
 * Adjust the Program Type radiobuttons by selecting only the button corresponding to   *
 * the SessionType.                                                                     *
\*                                                                                      */
    {                                   /* Radiobutton IDs sorted */
    USHORT      usSessionTypeRBs[]={PIRB_SHELL      , PIRB_OS2FULLSCREEN, PIRB_OS2WINDOW,
                                    PIRB_PM         , PIRB_DOSFULLSCREEN, PIRB_DOSWINDOW,
                                    PIRB_DETACHED   , PIRB_WPSOBJECT};
                                        /* Corresponding session type IDs sorted */
    USHORT      usSessionTypeIDs[]={PROG_DEFAULT    , PROG_FULLSCREEN   , PROG_WINDOWABLEVIO,
                                    PROG_PM         , PROG_VDM          , PROG_WINDOWEDVDM,
                                    PROG_DETACHED   , PROG_WPSOBJECT};
                                        /* Radiobutton IDs sorted */
    USHORT      usWINOS2TypeRBs[]={PIRB_WINREAL     , PIRB_WINSTANDARD  , PIRB_WIN386ENHANCED,
                                   PIRB_WINSTANDARD                     , PIRB_WIN386ENHANCED,
                                   PIRB_WINSTANDARD                     , PIRB_WIN386ENHANCED};
                                        /* Corresponding session type IDs sorted */
    USHORT      usWINOS2TypeIDs[]={PROG_WINDOW_REAL , PROG_31_STD       , PROG_31_ENH,
                                   PROG_31_STDSEAMLESSCOMMON            , PROG_31_ENHSEAMLESSCOMMON,
                                   PROG_31_STDSEAMLESSVDM               , PROG_31_ENHSEAMLESSVDM};
                                        /* Child windows that change with allowance of environment specification */
    USHORT      usChild2Environment[]={PICB_STARTUPENVIRONMENT, PIST_ENVIRONMENTS, PIST_ENVIRONMENT,
                                       PILB_ENVIRONMENTS, PIRB_LEADENVIRONMENT, PIRB_TRAILENVIRONMENT,
                                       PIMLE_ENVIRONMENT, PICB_STARTUPENVIRONMENT};
    USHORT      usChild2EnvSettings[]={PIST_ENVSETTINGS};
    USHORT      usSessionType=SHORT1FROMMP(mp1);
    USHORT      usTemp;
                                        /* Loop for all buttons and deselect all but the
                                           one specified in usSessionType */
    for(usTemp=0; usTemp<=(sizeof(usSessionTypeRBs)/sizeof(USHORT)); usTemp++)
        if(usSessionTypeIDs[usTemp]==usSessionType)
            {
            WinSendDlgItemMsg(          /* Send message to one Program Type radiobutton,
                                           this only updates the radiobutton but doesn't
                                           create a WM_CONTROL message */
                                        /* Handle of dialog window */
                nbPINotebookPage[PI_PAGE_2].hwndNBPage,
                                        /* Program type radiobutton */
                usSessionTypeRBs[usTemp],
                BM_SETCHECK,            /* Set it to pressed */
                MPFROMSHORT(TRUE),
                (MPARAM)NULL);
                                        /* Simulate the click to update the dialog's child windows */
            WinSendMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, WM_CONTROL,
                MPFROM2SHORT(usSessionTypeRBs[usTemp], BN_CLICKED), (MPARAM)NULL);
            }
    if((usSessionType!=PROG_PM) &&
        (usSessionType!=PROG_FULLSCREEN) &&
        (usSessionType!=PROG_WINDOWABLEVIO) &&
        (usSessionType!=PROG_DETACHED))
        {
                                        /* Disable and show Environment entryfield */
        DisableDialogItem(nbPINotebookPage[PI_PAGE_6].hwndNBPage, usChild2Environment,
            sizeof(usChild2Environment)/sizeof(USHORT), WS_DISABLED);
        DisableDialogItem(nbPINotebookPage[PI_PAGE_6].hwndNBPage, usChild2EnvSettings,
            sizeof(usChild2EnvSettings)/sizeof(USHORT), WS_VISIBLE);
        }
    switch(usSessionType)
    {                                   /* WIN-OS2 sessions must be treated similar */
    case PROG_WINDOW_REAL:
    case PROG_31_STDSEAMLESSVDM:
    case PROG_31_ENHSEAMLESSVDM:
    case PROG_31_STD:
    case PROG_31_ENH:
                                        /* For seamless separate WIN-OS2 session check PICB_WINSEPARATE */
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PICB_WINSEPARATE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Accept goto here (to avoid adding checkbox PICB_WINSEPARATE
                                           to tables */
        goto SetupWinOs2;

    case PROG_31_STDSEAMLESSCOMMON:
    case PROG_31_ENHSEAMLESSCOMMON:
                                        /* For seamless common WIN-OS2 session uncheck PICB_WINSEPARATE */
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PICB_WINSEPARATE, BM_SETCHECK,
            MPFROMSHORT(FALSE), (MPARAM)NULL);
                                        /* Entry point for separate WIN-OS2 sessions for common handling
                                           of separate and common sessions */
    SetupWinOs2:
                                        /* Loop for all WIN-OS2 buttons and deselect all but the
                                           one specified in usSessionType */
        for(usTemp=0; usTemp<=(sizeof(usWINOS2TypeRBs)/sizeof(USHORT)); usTemp++)
            if(usWINOS2TypeIDs[usTemp]==usSessionType)
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, usWINOS2TypeRBs[usTemp],
                    BM_SETCHECK, MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Check WIN-OS2 session type radiobutton */
        if((usSessionType==PROG_31_STD) || (usSessionType==PROG_31_ENH))
            {
            WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_WINOS2FULLSCREEN, BM_SETCHECK,
                MPFROMSHORT(TRUE), (MPARAM)NULL);
            WinSendMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, WM_CONTROL,
                MPFROM2SHORT(PIRB_WINOS2FULLSCREEN, BN_CLICKED), (MPARAM)NULL);
            }
        else
            {
            WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_WINOS2SEAMLESS, BM_SETCHECK,
                MPFROMSHORT(TRUE), (MPARAM)NULL);
            WinSendMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, WM_CONTROL,
                MPFROM2SHORT(PIRB_WINOS2SEAMLESS, BN_CLICKED), (MPARAM)NULL);
            }
        break;

    default:
                                        /* Check WIN standard session radiobutton, because at
                                           least one should be checked, if no WIN-OS2 session
                                           type is selected */
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PICB_WINSEPARATE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, PIRB_WINSTANDARD, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
        break;
    }
                                        /* Hide DOS Settings window until the application
                                           type is DOS Window or DOS Fullscreen */
    if(usSessionType==PROG_WPSOBJECT)
                                        /* Only default Style is allowed */
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIRB_DEFAULTSTYLE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SETUPSIZEPOSITION, (BOOL bWriteable), (BOOL bInsert)                      *
\*                                                                                      */
case WM_SETUPSIZEPOSITION:
/*                                                                                      *\
 * The 4 entryfields for the Size&Position are a bit more complicated to handle.        *
 * First we set the entryfields to a size of 4 and the default values are read from the *
 * SessionData structure. Only if Size&Position is selected, this 4 entryfields are not *
 * readonly. Flag bWriteable is true if Size&Position is selected, Flag bInsert is true *
 * if we write the values the first time.                                               *
\*                                                                                      */
    {
    USHORT      usSizePositionEFs[]={PIEF_X, PIEF_Y, PIEF_XSIZE, PIEF_YSIZE};
    BOOL        bWriteable=SHORT1FROMMP(mp1);
    BOOL        bInsert=SHORT1FROMMP(mp2);
    USHORT      usTemp;
    SHORT       *psSizePosition;
    UCHAR       ucBuffer[6];            /* Prefix, 4 numbers, \0 */

                                        /* Get first Size & Position value */
    psSizePosition=&pSessionData->InitXPos;
                                        /* Loop for all entryfields and enable and insert
                                           values into them */
    for(usTemp=0; usTemp<=(sizeof(usSizePositionEFs)/sizeof(USHORT));
        usTemp++, psSizePosition++)
        {
        if(bWriteable==TRUE)            /* If writeable, enable them */
            WinSendDlgItemMsg(          /* Send message to entryfield */
                nbPINotebookPage[PI_PAGE_3].hwndNBPage,
                                        /* Program size & position entryfield */
                usSizePositionEFs[usTemp],
                EM_SETREADONLY,         /* Set to enable read/write */
                MPFROMSHORT(FALSE),
                (MPARAM)NULL);
        else
            WinSendDlgItemMsg(          /* Send message to entryfield */
                nbPINotebookPage[PI_PAGE_3].hwndNBPage,
                                        /* Program size & position entryfield */
                usSizePositionEFs[usTemp],
                EM_SETREADONLY,         /* Set to enable readonly */
                MPFROMSHORT(TRUE),
                (MPARAM)NULL);
        if(bInsert==TRUE)               /* If initializing, insert them */
            {
            WinSendDlgItemMsg(          /* Send message to dialog window */
                                        /* Handle of dialog window */
                nbPINotebookPage[PI_PAGE_3].hwndNBPage,
                                        /* Program size: X entryfield */
                usSizePositionEFs[usTemp],
                EM_SETTEXTLIMIT,        /* Set text limit to 5 */
                MPFROMSHORT(5),
                (MPARAM)NULL);
            WinSetDlgItemText(          /* Set a text string in a dialog item */
                nbPINotebookPage[PI_PAGE_3].hwndNBPage,
                usSizePositionEFs[usTemp],
                (CHAR *)_itoa(*psSizePosition, ucBuffer, 10));
            }
        }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SETUPHOTKEY, NULL, NULL                                                   *
\*                                                                                      */
case WM_SETUPHOTKEY:
/*                                                                                      *\
 * Load Program Title into entryfield, fill the combobox and fill the MLE containing    *
 * the current window list.                                                             *
\*                                                                                      */
    {
    ULONG       ulTemp1, ulTemp2;
    PSWBLOCK    pSwBlock;               /* Pointer to window list */
                                        /* Used to add \n to switchlist entries */
    UCHAR       ucSwName[MAXNAMEL+1];

                                        /* Set Window Title entryfield */
    WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PIEF_SESSIONTITLE,
        EM_SETTEXTLIMIT, MPFROMSHORT(MAXNAMEL), (MPARAM)NULL);
    if(strcmp(pSessionData->PgmTitle, "Insert here please"))
        WinSetDlgItemText(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PIEF_SESSIONTITLE,
            (strlen(pSessionData->WindowTitle) ? pSessionData->WindowTitle : pSessionData->PgmTitle));
    WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICBX_HOTKEY,
        EM_SETTEXTLIMIT, MPFROMSHORT(sizeof(" ")), (MPARAM)NULL);
                                        /* Set SWP control radiobuttons */
    if(pSessionData->SwpFlag & SWP_MAXIMIZE)
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PIRB_WSMAXIMIZED, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else if(pSessionData->SwpFlag & SWP_RESTORE)
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PIRB_WSRESTORE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    else
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PIRB_NONE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pSessionData->SwpFlag & SWP_NOMOVE)
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_DONTMOVE, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pSessionData->KeyData.usCh!=0)   /* Check if a hotkey is defined */
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_HOTKEYENABLED, BM_SETCHECK,
            MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Simulate the click to update the dialog's child windows */
    WinSendMsg(nbPINotebookPage[PI_PAGE_2].hwndNBPage, WM_CONTROL,
        MPFROM2SHORT(PICB_HOTKEYENABLED, BN_CLICKED), (MPARAM)NULL);
                                        /* Simulate the click to update the PICB_RCTRL, PICB_CTRL,
                                           PICB_LWIN and/or PICB_ALT radiobutton. This message
                                           invalidates pSessionData->KeyData !!! */
    if(pSessionData->KeyData.usFlags&KC_RCTRL)
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_RCTRL,
            BM_SETCHECK, MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pSessionData->KeyData.usFlags&KC_CTRL)
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_CTRL,
            BM_SETCHECK, MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pSessionData->KeyData.usFlags&KC_ALT)
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_ALT,
            BM_SETCHECK, MPFROMSHORT(TRUE), (MPARAM)NULL);
    if(pSessionData->KeyData.usFlags&KC_LWIN)
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_LWIN,
            BM_SETCHECK, MPFROMSHORT(TRUE), (MPARAM)NULL);
    WinSendMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, WM_CONTROL,
        MPFROM2SHORT(PICB_ALT, BN_CLICKED), (MPARAM)NULL);
                                        /* Query the number of entries in window list */
    ulTemp1=WinQuerySwitchList(pHP->habPc2, NULL, 0);
                                        /* Allocate space for window list */
    pSwBlock=(PSWBLOCK)malloc(ulTemp1=(ulTemp1*sizeof(SWENTRY)+sizeof(HSWITCH)));
                                        /* Load window list into LB */
    ulTemp1=WinQuerySwitchList(pHP->habPc2, pSwBlock, ulTemp1);
    for(ulTemp2=0; ulTemp2<=ulTemp1; ulTemp2++)
        {
        if((pSwBlock->aswentry[ulTemp2].swctl.fbJump==SWL_JUMPABLE) &&
            (pSwBlock->aswentry[ulTemp2].swctl.uchVisibility==SWL_VISIBLE))
            {
            strcpy(ucSwName, pSwBlock->aswentry[ulTemp2].swctl.szSwtitle);
            WinSendDlgItemMsg(
                nbPINotebookPage[PI_PAGE_4].hwndNBPage, PILB_WINDOWLIST,
                LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(ucSwName));
            }
        }
    free(pSwBlock);                     /* Free allocated space for window list */
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SETUPPROGRAMPRIORITY, (USHORT PriorityClass), (SHORT PriorityDelta)       *                                           *
\*                                                                                      */
case WM_SETUPPROGRAMPRIORITY:
/*                                                                                      *\
 * To enable all Priorities to be selected, the checkbox PICB_CHILDPROCESS must be      *
 * checked and is either checked by the user or by the Priority not equal to the        *
 * default selection of PRTYC_NOCHANGE and 0.                                           *
\*                                                                                      */
    {
    USHORT      usPriorityClassRBs[]={PIRB_NOCHANGE, PIRB_IDLETIME, PIRB_REGULAR,
                                      PIRB_TIMECRITICAL, PIRB_FOREGROUNDSERVER};
    USHORT      usPriorityClass=SHORT1FROMMP(mp1);
    UCHAR       ucBuffer[4];

    sPriorityDelta=SHORT1FROMMP(mp2);
                                        /* Check priority class radiobutton by simulation a
                                           radiobutton click */
    WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, usPriorityClassRBs[usPriorityClass],
        BM_SETCHECK, MPFROMSHORT(TRUE), (MPARAM)NULL);
    WinSendMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, WM_CONTROL,
        MPFROM2SHORT(usPriorityClass, BN_CLICKED), (MPARAM)NULL);
                                        /* Set priority delta entryfield and scrollbar */
    WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIEF_PRIORITYDELTA,
        EM_SETTEXTLIMIT, MPFROMSHORT(3), (MPARAM)NULL);
    WinSetDlgItemText(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIEF_PRIORITYDELTA,
        (CHAR *)_itoa(sPriorityDelta, ucBuffer, 10));
                                        /* Set scollbar size and position. Caution
                                           a scrollbar is 0 based */
    WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIHSB_PRIORITYDELTA,
        SBM_SETTHUMBSIZE, MPFROM2SHORT(1, PRTYD_MAXIMUM), (MPARAM)NULL);
    WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIHSB_PRIORITYDELTA,
        SBM_SETSCROLLBAR, MPFROMSHORT(sPriorityDelta),
        MPFROM2SHORT(0, PRTYD_MAXIMUM));
    }
    break;

case WM_CHAR:
/*                                                                                      *\
 * Process the navigation keys on this page and forward all unprocessed keys to the     *
 * dialog where the notebook is part of.                                                *
\*                                                                                      */
    ProcessPageKey(hwndDlg, WinQueryWindow(hwndNotebook, QW_OWNER), mp1, mp2);
    break;

case WM_HSCROLL:                        /* Messages generated by horizontal scrollbars */
/*                                                                                      *\
 * This message is generated when a horizontal scrollbar is manipulated. See if it is   *
 * one of our scrollbar messages and process them.                                      *
\*                                                                                      */
    if(SHORT1FROMMP(mp1)==PIHSB_AUTOSTARTDELAY)
        {
        UCHAR       ucBuffer[5];
        SHORT       sSliderPosition=SHORT1FROMMP(mp2);

        switch(SHORT2FROMMP(mp2))
        {                               /* All messages contain the new slider position,
                                           so just use this value to change the priority
                                           delta entryfield value */
        case SB_LINELEFT:
            lAutostartDelay-=AUTOSTARTDELAY_SEC;
            break;

        case SB_PAGELEFT:
            lAutostartDelay-=AUTOSTARTDELAY_SEC*6;
            break;

        case SB_LINERIGHT:
            lAutostartDelay+=AUTOSTARTDELAY_SEC;
            break;

        case SB_PAGERIGHT:
            lAutostartDelay+=AUTOSTARTDELAY_SEC*6;
            break;

        case SB_SLIDERTRACK:
        case SB_SLIDERPOSITION:
            lAutostartDelay=(LONG)sSliderPosition*AUTOSTARTDELAY_SEC;
            break;
        }
        if(lAutostartDelay<AUTOSTARTDELAY_MIN)
            lAutostartDelay=AUTOSTARTDELAY_MIN;
        if(lAutostartDelay>AUTOSTARTDELAY_MAX)
            lAutostartDelay=AUTOSTARTDELAY_MAX;
        WinSetDlgItemText(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIEF_AUTOSTARTDELAY,
            (CHAR *)_itoa(lAutostartDelay, ucBuffer, 10));
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIHSB_AUTOSTARTDELAY,
            SBM_SETPOS, MPFROMSHORT(lAutostartDelay/AUTOSTARTDELAY_SEC), (MPARAM)NULL);
        }
    if(SHORT1FROMMP(mp1)==PIHSB_PRIORITYDELTA)
        {
        UCHAR       ucBuffer[4];
        SHORT       sSliderPosition=SHORT1FROMMP(mp2);

        switch(SHORT2FROMMP(mp2))
        {                               /* All messages contain the new slider position,
                                           so just use this value to change the priority
                                           delta entryfield value */
        case SB_LINELEFT:
            sPriorityDelta--;
            break;

        case SB_PAGELEFT:
            sPriorityDelta-=10;
            break;

        case SB_LINERIGHT:
            sPriorityDelta++;
            break;

        case SB_PAGERIGHT:
            sPriorityDelta+=10;
            break;

        case SB_SLIDERTRACK:
        case SB_SLIDERPOSITION:
            sPriorityDelta=sSliderPosition;
            break;
        }
        if(sPriorityDelta<PRTYC_NOCHANGE) sPriorityDelta=PRTYC_NOCHANGE;
        if(sPriorityDelta>PRTYD_MAXIMUM) sPriorityDelta=PRTYD_MAXIMUM;
        WinSetDlgItemText(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIEF_PRIORITYDELTA,
            (CHAR *)_itoa(sPriorityDelta, ucBuffer, 10));
        WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIHSB_PRIORITYDELTA,
            SBM_SETPOS, MPFROMSHORT(sPriorityDelta), (MPARAM)NULL);
        }
    break;

case WM_CONTROL:                        /* Test for autoradio buttons pressed */
/*                                                                                      *\
 * The Size & Position radiobutton must be checked every time it is selected, because   *
 * only if it is selected, the user may have write access to X, Y, Size X and Size Y    *
 * entryfields, otherwise they are read only. The DOS Setting MLE is only visible if a  *
 * DOS window or fullscreen application type is selected.                               *
\*                                                                                      */
    {
    ULONG   ulButtonChecked;
                                        /* Child windows that change with the
                                           PIRB_WPSOBJECT radiobutton */
    USHORT      usChild2WPSObject[]={PIRB_MAXIMIZED, PIRB_MINIMIZED, PICB_SIZEPOSITION,
                                     PICB_INVISIBLE, PICB_NOAUTOCLOSE, PICB_BACKGROUND,
                                     PIEF_X, PIEF_Y, PIEF_XSIZE, PIEF_YSIZE};
                                        /* Disable the following if Size & Position is disabled */
    USHORT      usChild2SizeEnable[]={PIEF_X, PIEF_Y, PIEF_XSIZE, PIEF_YSIZE};
                                        /* Hide the following for WPS Objects */
    USHORT      usChild2WPSObjectHide[]={PIEF_DIRECTORY, PIEF_PARAMETERS};
                                        /* Child window that changes with a DOS radiobutton */
    USHORT      usChild2DOS[]={PIMLE_DOSSETTINGS, PIPB_SETTINGSDLG};
    USHORT      usChild2DOSSettings[]={PIST_DOSSETTINGS};
    USHORT      usChild2DOSP5[]={PIRB_IDLETIME, PIRB_REGULAR, PIRB_TIMECRITICAL, PIRB_FOREGROUNDSERVER,
                                 PIEF_PRIORITYDELTA, PIHSB_PRIORITYDELTA};
                                        /* Child windows that change with a WIN-OS2 radiobutton */
    USHORT      usChild2WINOS2[]={PIRB_WINREAL, PIRB_WINSTANDARD, PIRB_WIN386ENHANCED, PICB_WINSEPARATE};
                                        /* Child window for seamless WIN-OS2 sessions */
    USHORT      usChild2Seamless[]={PICB_WINSEPARATE};
                                        /* Child windows that change with allowance of environment specification */
    USHORT      usChild2Environment[]={PIST_ENVIRONMENTS, PIST_ENVIRONMENT, PILB_ENVIRONMENTS,
                                       PIRB_LEADENVIRONMENT, PIRB_TRAILENVIRONMENT, PIMLE_ENVIRONMENT};
    USHORT      usChild2EnvSettings[]={PIST_ENVSETTINGS};

                                        /* Was button class window clicked or double-clicked ? */
    if((SHORT2FROMMP(mp1)==BN_CLICKED) || (SHORT2FROMMP(mp1)==BN_DBLCLICKED))
        switch(SHORT1FROMMP(mp1))
        {
        case PICB_AUTOSTART:            /* Autostart enable/disable checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PICB_AUTOSTART, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
                                        /* If checked enable and show */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_3].hwndNBPage, usChild2AutostartCB,
                sizeof(usChild2AutostartCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case PICB_SIZEPOSITION:         /* Program type: User defined size & position */
            if((BOOL)WinSendDlgItemMsg( /* Send message to Size & Position radiobutton */
                                        /* Handle of dialog window */
                nbPINotebookPage[PI_PAGE_3].hwndNBPage,
                PICB_SIZEPOSITION,      /* Program size: Size & Position radiobutton */
                BM_QUERYCHECK,          /* Query the state */
                (MPARAM)NULL,
                (MPARAM)NULL)==TRUE)
                {
                WinSendMsg(hwndDlg, WM_SETUPSIZEPOSITION,
                    MPFROMSHORT(TRUE), MPFROMSHORT(FALSE));
                DisableDialogItem(nbPINotebookPage[PI_PAGE_3].hwndNBPage, usChild2SizeEnable,
                    sizeof(usChild2SizeEnable)/sizeof(USHORT), WS_VISIBLE);
                }
            else
                {
                WinSendMsg(hwndDlg, WM_SETUPSIZEPOSITION,
                    MPFROMSHORT(FALSE), MPFROMSHORT(FALSE));
                                        /* Enable and show Size & Position child windows */
                                        /* Disable and show Size & Position child windows */
                DisableDialogItem(nbPINotebookPage[PI_PAGE_3].hwndNBPage, usChild2SizeEnable,
                    sizeof(usChild2SizeEnable)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                }
            break;

        case PICB_HOTKEYENABLED:        /* Enable or disable and show Hotkey controls depending
                                           on Hotkey Enable checkbox */
            ulButtonChecked=(ULONG)WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_HOTKEYENABLED, BM_QUERYCHECK,
                (MPARAM)NULL, (MPARAM)NULL);
                                        /* If checked enable and show */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_4].hwndNBPage, usChild2HotKeyCB,
                sizeof(usChild2HotKeyCB)/sizeof(USHORT),
                (ulButtonChecked==TRUE ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case PICB_RCTRL:
        case PICB_CTRL:
        case PICB_ALT:
        case PICB_LWIN:
                                        /* Set hotkey MLE corresponding to radiobuttons. If a event
                                           is generated by PM, mp2 is the control's window handle, if
                                           we send this message during dialog initilization mp2 is NULL. */
            {
            KEYDATA     *pKeyDataIndex;
            UCHAR       ucBuffer[]="0"; /* String to be inserted into combobox */
            ULONG       ulTemp1;
            BOOL        bFirst=TRUE;    /* False if first key was inserted */
            USHORT      usFlags=0;

                                        /* For CTRL or ALT selection events disallow the RCTRL and WIN
                                           keys and for WIN key selection events disallow RCTRL, CTRL
                                           and ALT keys, and for the RCTRL key selection disallow all
                                           others */
            if((SHORT1FROMMP(mp1)==PICB_RCTRL) &&
                (WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_RCTRL)))
                {
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_LWIN,
                    BM_SETCHECK, MPFROMSHORT(FALSE), (MPARAM)NULL);
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_CTRL,
                    BM_SETCHECK, MPFROMSHORT(FALSE), (MPARAM)NULL);
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_ALT,
                    BM_SETCHECK, MPFROMSHORT(FALSE), (MPARAM)NULL);
                }
            if((SHORT1FROMMP(mp1)==PICB_CTRL) &&
                (WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_CTRL)))
                {
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_RCTRL,
                    BM_SETCHECK, MPFROMSHORT(FALSE), (MPARAM)NULL);
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_LWIN,
                    BM_SETCHECK, MPFROMSHORT(FALSE), (MPARAM)NULL);
                }
            if((SHORT1FROMMP(mp1)==PICB_ALT) &&
                (WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_ALT)))
                {
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_RCTRL,
                    BM_SETCHECK, MPFROMSHORT(FALSE), (MPARAM)NULL);
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_LWIN,
                    BM_SETCHECK, MPFROMSHORT(FALSE), (MPARAM)NULL);
                }
            if((SHORT1FROMMP(mp1)==PICB_LWIN) &&
                (WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_LWIN)))
                {
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_RCTRL,
                    BM_SETCHECK, MPFROMSHORT(FALSE), (MPARAM)NULL);
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_CTRL,
                    BM_SETCHECK, MPFROMSHORT(FALSE), (MPARAM)NULL);
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_ALT,
                    BM_SETCHECK, MPFROMSHORT(FALSE), (MPARAM)NULL);
                }
                                        /* At least on checkbox must be checked, otherwise
                                           check both (the CTRL and ALT checkbox) */
            if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_RCTRL))
                usFlags|=KC_RCTRL;
            if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_CTRL))
                usFlags|=KC_CTRL;
            if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_ALT))
                usFlags|=KC_ALT;
            if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_LWIN))
                usFlags=KC_LWIN;
            if(usFlags==0)
                {
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_CTRL,
                    BM_SETCHECK, MPFROMSHORT(TRUE), (MPARAM)NULL);
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICB_ALT,
                    BM_SETCHECK, MPFROMSHORT(TRUE), (MPARAM)NULL);
                usFlags|=(KC_CTRL|KC_ALT);
                }
            pKeyDataIndex=pHP->pKeyData;
                                        /* If a key is defined set it into entryfield of
                                           combobox. Clear it afterwards that user selections
                                           to the PICB_CTRL or PICB_ALT insert the first free
                                           available key */
            if(pSessionData->KeyData.usCh!=0)
                {
                ucBuffer[0]=(UCHAR)pSessionData->KeyData.usCh;
                WinSetDlgItemText(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICBX_HOTKEY, ucBuffer);
                bFirst=FALSE;
                }
            else bFirst=TRUE;
                                        /* Delete current entries in hotkey combobox */
            WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICBX_HOTKEY,
                LM_DELETEALL, (MPARAM)NULL, (MPARAM)NULL);
                                        /* Loop for all keys and set unused into combobox */
            for(ulTemp1=0; ulTemp1<KEYDATACOUNT; pKeyDataIndex++, ulTemp1++)
                {
                ucBuffer[0]=(UCHAR)pKeyDataIndex->usCh;
                                        /* Clear the menuentry's current key because it may not
                                           selected again, and the new key is set when this dialog
                                           is closed */
                if((pKeyDataIndex->usCh==pSessionData->KeyData.usCh) &&
                    (pKeyDataIndex->usFlags==pSessionData->KeyData.usFlags))
                    {
                    pSessionData->KeyData.usFlags=0;
                    pSessionData->KeyData.usCh=0;
                    pKeyDataIndex->bUsed=FALSE;
                    pKeyDataIndex->pMenuData=NULL;
                    }
                                        /* Load unused key of current type into combobox */
                if((pKeyDataIndex->bUsed==FALSE) &&
                    (usFlags==pKeyDataIndex->usFlags))
                    {
                    if(bFirst==TRUE)
                        {               /* The first valid hotkey is loaded into entryfield part
                                           of combobox */
                        WinSetDlgItemText(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICBX_HOTKEY, ucBuffer);
                        bFirst=FALSE;
                        }
                    WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PICBX_HOTKEY,
                        LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(ucBuffer));
                    }
                }
            }
            break;

        case PIRB_DOSFULLSCREEN:
        case PIRB_DOSWINDOW:
                                        /* Disable and hide Environment entryfield */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_6].hwndNBPage, usChild2Environment,
                sizeof(usChild2Environment)/sizeof(USHORT), WS_DISABLED);
            DisableDialogItem(nbPINotebookPage[PI_PAGE_6].hwndNBPage, usChild2EnvSettings,
                sizeof(usChild2EnvSettings)/sizeof(USHORT), WS_VISIBLE);
                                        /* Enable and show WPSObject child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2WPSObjectHide,
                sizeof(usChild2WPSObjectHide)/sizeof(USHORT), WS_VISIBLE);
                                        /* Enable and show DOS child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2DOS,
                sizeof(usChild2DOS)/sizeof(USHORT), WS_VISIBLE);
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2DOSSettings,
                sizeof(usChild2DOSSettings)/sizeof(USHORT), WS_DISABLED);
                                        /* Set default priority class radiobutton by simulation a
                                           radiobutton click */
            WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIRB_NOCHANGE,
                BM_SETCHECK, MPFROMSHORT(TRUE), (MPARAM)NULL);
            WinSendMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, WM_CONTROL,
                MPFROM2SHORT(PIRB_NOCHANGE, BN_CLICKED), (MPARAM)NULL);
                                        /* Set default priority delta entryfield and scrollbar */
            WinSetDlgItemText(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIEF_PRIORITYDELTA, "0");
                                        /* Set default scollbar position */
            WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIHSB_PRIORITYDELTA,
                SBM_SETPOS, MPFROMSHORT(0), (MPARAM)NULL);
                                        /* Now disable non-default priority settings */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_5].hwndNBPage, usChild2DOSP5,
                sizeof(usChild2DOSP5)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Enable and show WPSObject child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_3].hwndNBPage, usChild2WPSObject,
                sizeof(usChild2WPSObject)/sizeof(USHORT), WS_VISIBLE);
                                        /* Disable and show WIN-OS2 child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_2].hwndNBPage, usChild2WINOS2,
                sizeof(usChild2WINOS2)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
            break;

        case PIRB_OS2FULLSCREEN:
        case PIRB_OS2WINDOW:
        case PIRB_PM:
        case PIRB_DETACHED:
        case PIRB_SHELL:
                                        /* For all OS/2 and PM sessions enable environment specification */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_6].hwndNBPage, usChild2Environment,
                sizeof(usChild2Environment)/sizeof(USHORT),
                (SHORT1FROMMP(mp1)==PIRB_SHELL ? WS_DISABLED : WS_VISIBLE));
            DisableDialogItem(nbPINotebookPage[PI_PAGE_6].hwndNBPage, usChild2EnvSettings,
                sizeof(usChild2EnvSettings)/sizeof(USHORT),
                (SHORT1FROMMP(mp1)==PIRB_SHELL ? WS_VISIBLE : WS_DISABLED));
                                        /* Enable and hide DOS child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2DOS,
                sizeof(usChild2DOS)/sizeof(USHORT), 0);
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2DOSSettings,
                sizeof(usChild2DOSSettings)/sizeof(USHORT), WS_VISIBLE);
                                        /* Now enable priority settings */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_5].hwndNBPage, usChild2DOSP5,
                sizeof(usChild2DOSP5)/sizeof(USHORT), WS_VISIBLE);
                                        /* Enable and show WPSObject child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_3].hwndNBPage, usChild2WPSObject,
                sizeof(usChild2WPSObject)/sizeof(USHORT), WS_VISIBLE);
                                        /* Enable and show WPSObject child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2WPSObjectHide,
                sizeof(usChild2WPSObjectHide)/sizeof(USHORT), WS_VISIBLE);
                                        /* Disable and show WIN-OS2 child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_2].hwndNBPage, usChild2WINOS2,
                sizeof(usChild2WINOS2)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
            break;

        case PIRB_WPSOBJECT:
                                        /* Disable and hide Environment entryfield */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_6].hwndNBPage, usChild2Environment,
                sizeof(usChild2Environment)/sizeof(USHORT), WS_DISABLED);
                DisableDialogItem(nbPINotebookPage[PI_PAGE_6].hwndNBPage, usChild2EnvSettings,
                    sizeof(usChild2EnvSettings)/sizeof(USHORT), WS_VISIBLE);
                                        /* Disable and show WPSObject child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_3].hwndNBPage, usChild2WPSObject,
                sizeof(usChild2WPSObject)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Enable and hide DOS child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2DOS,
                sizeof(usChild2DOS)/sizeof(USHORT), 0);
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2DOSSettings,
                sizeof(usChild2DOSSettings)/sizeof(USHORT), WS_VISIBLE);
                                        /* Now disable non-default priority settings */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_5].hwndNBPage, usChild2DOSP5,
                sizeof(usChild2DOSP5)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Hide but enable WPSObject child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2WPSObjectHide,
                sizeof(usChild2WPSObjectHide)/sizeof(USHORT), 0);
                                        /* Disable and show WIN-OS2 child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_2].hwndNBPage, usChild2WINOS2,
                sizeof(usChild2WINOS2)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Only default Style is allowed */
            WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PIRB_DEFAULTSTYLE, BM_SETCHECK,
                MPFROMSHORT(TRUE), (MPARAM)NULL);
                                        /* Size and Position is not allowed */
            WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_3].hwndNBPage, PICB_SIZEPOSITION, BM_SETCHECK,
                MPFROMSHORT(FALSE), (MPARAM)NULL);
                                        /* Load empty text in not required fields */
            WinSetDlgItemText(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_DIRECTORY, "");
            WinSetDlgItemText(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIEF_PARAMETERS, "");
            break;

        case PIRB_WINOS2SEAMLESS:
        case PIRB_WINOS2FULLSCREEN:
                                        /* Disable and hide Environment entryfield */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_6].hwndNBPage, usChild2Environment,
                sizeof(usChild2Environment)/sizeof(USHORT), WS_DISABLED);
            DisableDialogItem(nbPINotebookPage[PI_PAGE_6].hwndNBPage, usChild2EnvSettings,
                sizeof(usChild2EnvSettings)/sizeof(USHORT), WS_VISIBLE);
                                        /* Enable and show WPSObject child windows, because
                                           a WIN-OS2 session is a specail DOS session */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2WPSObjectHide,
                sizeof(usChild2WPSObjectHide)/sizeof(USHORT), WS_VISIBLE);
                                        /* Enable and show DOS child windows, hide priority */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2DOS,
                sizeof(usChild2DOS)/sizeof(USHORT), WS_VISIBLE);
            DisableDialogItem(nbPINotebookPage[PI_PAGE_1].hwndNBPage, usChild2DOSSettings,
                sizeof(usChild2DOSSettings)/sizeof(USHORT), WS_DISABLED);
                                        /* Set default priority class radiobutton by simulation a
                                           radiobutton click */
            WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIRB_NOCHANGE,
                BM_SETCHECK, MPFROMSHORT(TRUE), (MPARAM)NULL);
            WinSendMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, WM_CONTROL,
                MPFROM2SHORT(PIRB_NOCHANGE, BN_CLICKED), (MPARAM)NULL);
                                        /* Set default priority delta entryfield and scrollbar */
            WinSetDlgItemText(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIEF_PRIORITYDELTA, "0");
                                        /* Set default scollbar position */
            WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_5].hwndNBPage, PIHSB_PRIORITYDELTA,
                SBM_SETPOS, MPFROMSHORT(0), (MPARAM)NULL);
                                        /* Now disable non-default priority settings */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_5].hwndNBPage, usChild2DOSP5,
                sizeof(usChild2DOSP5)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Enable and show WPSObject child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_3].hwndNBPage, usChild2WPSObject,
                sizeof(usChild2WPSObject)/sizeof(USHORT), WS_VISIBLE);
                                        /* Enable and show WIN-OS2 child windows */
            DisableDialogItem(nbPINotebookPage[PI_PAGE_2].hwndNBPage, usChild2WINOS2,
                sizeof(usChild2WINOS2)/sizeof(USHORT), WS_VISIBLE);
                                        /* Separate sessions are only available for seamless
                                           WIN-OS2 sessions */
             DisableDialogItem(nbPINotebookPage[PI_PAGE_2].hwndNBPage, usChild2Seamless,
                 sizeof(usChild2Seamless)/sizeof(USHORT),
                (SHORT1FROMMP(mp1)==PIRB_WINOS2SEAMLESS ? WS_VISIBLE : WS_VISIBLE | WS_DISABLED));
            break;

        case PIRB_LEADENVIRONMENT:
        case PIRB_TRAILENVIRONMENT:
            {
            UCHAR   *pucEnvironment=NULL;

                                        /* Replace the current MLE's contents with the one of the selected
                                           environment autoradiobutton after saving the old contents, but only
                                           if the MLE has allready been initialized */
            if(ulLastEnvironmentButton!=0)
                {
                ULONG   ulPgmEnvironmentLength=0;

                                        /* Allocate required length + space for \r\n\0 */
                ulPgmEnvironmentLength=(ULONG)(WinQueryWindowTextLength(WinWindowFromID(nbPINotebookPage[PI_PAGE_6].hwndNBPage,
                    PIMLE_ENVIRONMENT)))+3;
                                        /* Allocate possibly changed Environment settings */
                pucEnvironment=malloc(ulPgmEnvironmentLength);
                WinQueryWindowText(         /* Query the environment settings */
                    WinWindowFromID(nbPINotebookPage[PI_PAGE_6].hwndNBPage, PIMLE_ENVIRONMENT),
                    ulPgmEnvironmentLength,
                    pucEnvironment);
                                        /* Remove all trailing \r and \n to add just 1 \r\n */
                StripTrailing(pucEnvironment);
                                        /* If not empty string ("\0") add 1 \r\n */
                if(*pucEnvironment!='\0')
                    strcat(pucEnvironment, "\r\n");
                if(ulLastEnvironmentButton==PIRB_LEADENVIRONMENT)
                    {
                    free(pSessionData->PgmLeadingEnvironment);
                    pSessionData->PgmLeadingEnvironment=pucEnvironment;
                    }
                if(ulLastEnvironmentButton==PIRB_TRAILENVIRONMENT)
                    {
                    free(pSessionData->PgmTrailingEnvironment);
                    pSessionData->PgmTrailingEnvironment=pucEnvironment;
                    }
                }
            if(WinQueryButtonCheckstate(nbPINotebookPage[PI_PAGE_6].hwndNBPage, PIRB_LEADENVIRONMENT))
                {
                ulLastEnvironmentButton=PIRB_LEADENVIRONMENT;
                pucEnvironment=pSessionData->PgmLeadingEnvironment;
                }
            else
                {
                ulLastEnvironmentButton=PIRB_TRAILENVIRONMENT;
                pucEnvironment=pSessionData->PgmTrailingEnvironment;
                }
                                        /* Set (possibly replace) Environment settings */
            WinSetWindowText(WinWindowFromID(nbPINotebookPage[PI_PAGE_6].hwndNBPage, PIMLE_ENVIRONMENT),
                pucEnvironment);
            }
            break;

        case PIRB_DISABLESCHEDULER:
        case PIRB_SCHEDULEHOURLY:
        case PIRB_SCHEDULEDAILY:
        case PIRB_SCHEDULEWEEKLY:
        case PIRB_SCHEDULEMONTHLY:
        case PIRB_SCHEDULEONCE:
            CheckSchedulerSettings(SHORT1FROMMP(mp1), nbPINotebookPage[PI_PAGE_7].hwndNBPage);
            break;

        }
    if(SHORT1FROMMP(mp1)==PILB_WINDOWLIST)
        switch(SHORT2FROMMP(mp1))       /* Events of Window List listbox */
        {
        case LN_ENTER:                  /* When selected, copy selected item into session title entryfield */
            {
            UCHAR   ucTemp[MAXNAMEL];
            USHORT  usSelection;

            usSelection=(ULONG)WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PILB_WINDOWLIST,
                LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)NULL);
                                        /* Copy selected item */
            if(usSelection!=(USHORT)LIT_NONE)
                {
                WinSendDlgItemMsg(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PILB_WINDOWLIST,
                    LM_QUERYITEMTEXT, MPFROM2SHORT(usSelection, MAXNAMEL), MPFROMP(ucTemp));
                WinSetDlgItemText(nbPINotebookPage[PI_PAGE_4].hwndNBPage, PIEF_SESSIONTITLE, ucTemp);
                }
            break;
            }
        }
    }
    break;

case WM_COMMAND:                        /* Button pressed */
    switch(SHORT1FROMMP(mp1))
    {
    case PIPB_WINFILEDLG:               /* Locate file by standard file dialog */
/*                                                                                      *\
 * The user selected the File Find pushbutton to get the standard file dialog to find   *
 * the program he wants to install.                                                     *
\*                                                                                      */
        {
        FILEDLG fdFileDlg;              /* Standard file dialog control */
        HWND    hwndFileDlg;            /* Handle of standard file dialog */

                                        /* Clear out structure */
        memset(&fdFileDlg, 0, sizeof(FILEDLG));
                                        /* Structure size */
        fdFileDlg.cbSize=sizeof(FILEDLG);
                                        /* FDS_* flags */
        fdFileDlg.fl=FDS_CENTER | FDS_OPEN_DIALOG;
                                        /* Dialog title string */
        fdFileDlg.pszTitle="PC/2 File Search";
                                        /* Initial path, filename or file filter */
        strcpy(fdFileDlg.szFullFile, "C:\\*");
                                        /* Open the standard file dialog ...*/
        hwndFileDlg=WinFileDlg(HWND_DESKTOP, hwndDlg, &fdFileDlg);
        if(hwndFileDlg && (fdFileDlg.lReturn==DID_OK))
            {
                                        /* Load the values of the standard file dialog to
                                           the corresponding entryfield in the Program
                                           installation dialog. It is a file not an object. */
            UCHAR   ucBuffer[20];

            WinQueryWindowText(WinWindowFromID(hwndDlg, PIEF_PROGRAMTITLE),
                sizeof(ucBuffer), ucBuffer);
            if(!strcmp(ucBuffer, "Insert here please"))
                WinSetWindowText(WinWindowFromID(hwndDlg, PIEF_PROGRAMTITLE), "");
            InstallFilename2Dialog(hwndDlg, fdFileDlg.szFullFile, FALSE);
            }
        }
        break;

    case PIPB_SETTINGSDLG:              /* Locate file by standard file dialog */
/*                                                                                      *\
 * The user selected the Settings pushbutton to get the standard OS/2 DOS settings      *
 * dialog. Special thanks to Monte Copeland for giving me a sample source code on how   *
 * using this built-in but undocumented OS/2 feature.                                   *
\*                                                                                      */
        {
                                        /* PMVDMP.DLL export @11 WINDOWS32PROPERTYDIALOG */
        PFFUNCPTR4  *pWindows32PropertyDialog;
        HMODULE     hDllPMVDMP;         /* PMVDMP.DLL module handle */
        UCHAR       ucBuffer[80];       /* Buffer for possible return codes from DLL loading */
        UCHAR       *pucSettingsIn;
        UCHAR       *pucSettingsOut;
        USHORT      usLength, usWinMode=1;
        ULONG       ulLength;

                                        /* Get ordinal 11 function pointer of PMVDMP.DLL which
                                           contains the DOS settings dialog */
        if((DosLoadModule(ucBuffer, sizeof(ucBuffer)-1, "PMVDMP", &hDllPMVDMP)!=NO_ERROR) ||
            (DosQueryProcAddr(hDllPMVDMP, 11, NULL, (PFN *)(&pWindows32PropertyDialog))
            !=NO_ERROR))
            {                           /* An error occured */
            pWindows32PropertyDialog=NULL;
            DosFreeModule(hDllPMVDMP);  /* Free DLL reference */
            USR_ERR(pHP->hwndFrame, HELP_PC2INI, MB_INFORMATION|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
                "Can't load VDM support from PMVDMP.DLL. You have either not installed the "\
                "DOS support of OS/2, or your DOS support is not setup correctly. The DOS settings "\
                "dialog of PC/2 will not be available until you correct this problem - continuing...");
            }
                                        /* Without VDM support installed DOS support is not available */
        if(pWindows32PropertyDialog==NULL) break;
                                        /* Get current listbox data length */
        ulLength=(ULONG)WinQueryWindowTextLength(WinWindowFromID(nbPINotebookPage[PI_PAGE_1].hwndNBPage,
            PIMLE_DOSSETTINGS));
                                        /* Get current DOS settings or at least a NULL string */
        if(ulLength==0)
            ulLength=1;
        pucSettingsOut=malloc(ulLength);
        WinQueryWindowText(             /* Query data entered in DOS Settings MLE */
            WinWindowFromID(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIMLE_DOSSETTINGS),
            ulLength, pucSettingsOut);
                                        /* Convert to format used by WINDOWS32PROPERTYDIALOG */
        pucSettingsIn=ImportDosSettings(pucSettingsOut, &ulLength, FALSE);
                                        /* Free data read from DOS Settings MLE */
        free(pucSettingsOut);
                                        /* Call DOS Settings dialog by passing DOS Settings MLE
                                           data \0\0 terminated and expect a \0\0 terminated
                                           shared memory address containing the user changes */
        pucSettingsOut=pWindows32PropertyDialog(
            hwndDlg,                    /* Parent window handle ? */
            hwndDlg,                    /* Owner window handle ? */
            pucSettingsIn,              /* In-buffer containing DOS settings */
            (USHORT)ulLength,           /* Length of in-buffer containing DOS settings */
            &usLength,                  /* Length of out-buffer containing modified DOS settings,
                                           allocated by DosAllocateMem() */
            &usWinMode);                /* 0 if no WIN-OS2 settings are included
                                           ? if WIN-OS2 settings are included */
        if(pucSettingsIn) free(pucSettingsIn);
        if(pucSettingsOut!=NULL)        /* NULL returned when Cancel was pressed */
            {
            ulLength=(ULONG)usLength;
                                        /* Convert to format used PC/2 configuration file and
                                           Program Installation dialog */
            pucSettingsIn=ImportDosSettings(pucSettingsOut, &ulLength, TRUE);
            DosFreeMem(pucSettingsOut);
                                        /* Insert data in DOS Settings MLE */
            WinSetWindowText(WinWindowFromID(nbPINotebookPage[PI_PAGE_1].hwndNBPage, PIMLE_DOSSETTINGS),
                pucSettingsIn);
            free(pucSettingsIn);
            }
        DosFreeModule(hDllPMVDMP);      /* Free DLL reference */
        }
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

                                        /* Call function instead of subclassing notebook page 1
                                           notebook dialogs */
case DM_DRAGOVER:
case DM_DROP:
                                        /* Only on first page allow Drag'n Drop */
    if(hwndDlg==nbPINotebookPage[PI_PAGE_1].hwndNBPage)
        return(DragDropDialogProcedure(hwndDlg, msg, mp1, mp2, PIEF_ICONFILE));
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the Drag'n Drop support - Program Installation dialog. *
 * Req:                                                                                 *
 *      SessionData ... a structure of type SESSIONDATA where the information entered   *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY DragDropDialogProcedure(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2, USHORT usControl)
{
PDRAGINFO       pDraginfo;              /* Pointer to DRAGINFO structure */
PDRAGITEM       pDragitem;              /* Pointer to DRAGITEM structure */

switch(msg)
{
/*                                                                                      *\
 * Allow objects to be dropped on dialog box to add first dragitem as an application.   *
\*                                                                                      */
case DM_DRAGOVER:
    pDraginfo=(PDRAGINFO)mp1;           /* Get the pointer to the DRAGINFO structure */
                                        /* Access the structure */
    if(DrgAccessDraginfo(pDraginfo)==FALSE) break;
                                        /* Get the first itemp of the item(s) dragged
                                           onto dialog window */
    pDragitem=DrgQueryDragitemPtr(pDraginfo, 0);
    if((pDraginfo->cditem==1) &&
        ((DrgVerifyRMF(pDragitem, "DRM_OS2FILE", "DRF_UNKNOWN")) ||
        (DrgVerifyRMF(pDragitem, "DRM_OS2FILE", "DRF_ICON")) ||
        (DrgVerifyRMF(pDragitem, "DRM_OS2FILE", "DRF_TEXT"))))
                                        /* Allow drop of undefined operation onto dialog window */
        return(MPFROM2SHORT(DOR_DROP, DO_UNKNOWN));
                                        /* Don't allow dropping of more than one or unknown item */
    return(MPFROM2SHORT(DOR_NODROP, DO_UNKNOWN));

case DM_DROP:
    {
    UCHAR       ucBuffer[CCHMAXPATH];
    UCHAR       ucFileName[CCHMAXPATH];
    UCHAR       ucObjectType[CCHMAXPATH];
    UCHAR       *pucBuffer;

    pDraginfo=(PDRAGINFO)mp1;           /* Get the pointer to the DRAGINFO structure */
                                        /* Access the structure */
    if(DrgAccessDraginfo(pDraginfo)==FALSE) break;
                                        /* Get the first itemp of the item(s) dragged
                                           onto dialog window */
    pDragitem=DrgQueryDragitemPtr(pDraginfo, 0);
                                        /* Query the rendering format */
    DrgQueryStrName(pDragitem->hstrRMF, sizeof(ucObjectType), ucObjectType);
                                        /* Scan for an file object */
    if((DrgVerifyRMF(pDragitem, "DRM_OS2FILE", "DRF_UNKNOWN")) ||
        (DrgVerifyRMF(pDragitem, "DRM_OS2FILE", "DRF_ICON")) ||
        (DrgVerifyRMF(pDragitem, "DRM_OS2FILE", "DRF_TEXT")))
        {                               /* It is an application to get path and filename */
                                        /* Query path (container) */
        DrgQueryStrName(pDragitem->hstrContainerName, sizeof(ucBuffer), ucBuffer);
                                        /* Position to \0 of path */
        pucBuffer=ucBuffer+strlen(ucBuffer);
                                        /* Query filename and append it to path */
        DrgQueryStrName(pDragitem->hstrSourceName, sizeof(ucBuffer)-strlen(pucBuffer), pucBuffer);
                                        /* Copy filename of file object dropped and uppercase it */
        strcpy(ucFileName, ucBuffer);
        strupr(ucFileName);
        if(strstr(ucFileName, "ICO"))
            {
                                        /* If it is an icon file, set it to File icon entryfield.
                                           This is valid for both, the Program and Menu installation
                                           dialog */
            WinSetDlgItemText(hwnd, usControl, ucBuffer);
            }
        else if(usControl==PIEF_ICONFILE)
            {
                                        /* If it is an executable or batch file PC/2 can start it
                                           otherwise it must be an WPS object like a folder. This
                                           is only valid for the Program Installation dialog */
            if(strstr(ucFileName, "EXE") || strstr(ucFileName, "COM") ||
                strstr(ucFileName, "CMD") || strstr(ucFileName, "BAT"))
                                        /* Now change the entryfields to the dropped object
                                           which is not a WPS object */
                InstallFilename2Dialog(hwnd, ucBuffer, FALSE);
            else
                                        /* Now change the entryfields to the dropped object
                                           which is a WPS object */
                InstallFilename2Dialog(hwnd, ucBuffer, TRUE);
            }
        break;
        }
    }
    break;
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Submenu Installation dialog.                *
 * Req:                                                                                 *
 *      SessionData ... a structure of type SESSIONDATA where the information entered   *
 *                      in the dialog is entered. If DID_OK is pressed this structure   *
 *                      is used to start the session.                                   *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY MI_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static HWND         hwndNB;             /* Menu Installation dialog notebook window handle */
static SESSIONDATA  *pSessionData;      /* Pointer to SESSIONDATA structure for current instance */

switch(msg)
{
case WM_INITDLG:
    {
    ULONG       ulNBStyle;              /* Notebook style (to take the new Merlin notebook style into
                                           account, just try out SET NEWNOTEBOOS=YES in your Merlin
                                           beta CONFIG.SYS with your "old"-style notebooks) */
    USHORT      usTabTextLength=50;
    USHORT      usTabTextHeight=5;

                                        /* Access parameters passed */
    pSessionData=(SESSIONDATA *)PVOIDFROMMP(mp2);
    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
    ulNBStyle=WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE;
    if(((pHP->ulVersionMajor==OS2_MAJOR) && (pHP->ulVersionMinor>=OS2_MINOR_400)) ||
        (pHP->ulVersionMajor>OS2_MAJOR))
        ulNBStyle|=(BKS_TABBEDDIALOG|BKS_MAJORTABTOP|BKS_BACKPAGESTR|
            BKS_STATUSTEXTRIGHT|BKS_TABTEXTCENTER|BKS_POLYGONTABS);
    else
        ulNBStyle|=(BKS_SPIRALBIND|BKS_MAJORTABRIGHT|BKS_BACKPAGESBR|
            BKS_STATUSTEXTCENTER|BKS_TABTEXTRIGHT);
    hwndNB=WinCreateWindow(hwndDlg, WC_NOTEBOOK, "", ulNBStyle, 5, 0, 0, 0, hwndDlg,
        HWND_BOTTOM, MINB_NOTEBOOK, NULL, NULL);
                                        /* Subclass notebook's window procedure to add handling of
                                           notebook pages, notebook and outer dialog accelerator keys */
    DialogNotebookSubclass[MI_SUBCLASSEDNOTEBOOK].hwndNotebook=hwndNB;
    DialogNotebookSubclass[MI_SUBCLASSEDNOTEBOOK].pfnwpNotebook=
        WinSubclassWindow(hwndNB, SubclassedNotebookProcedure);
                                        /* Fill the notebook and return the tab text requirements */
    AddPagesToNotebook(hwndDlg, hwndNB, nbMINotebookPage, &usTabTextLength, &usTabTextHeight);
                                        /* Adjust dialog to optimally cover the notebook */
    AdjustNotebook(hwndDlg, hwndNB, ulNBStyle, usTabTextLength, usTabTextHeight);
                                        /* Now initialize all notebook page dialogs
                                           by posting to 1st page */
    WinPostMsg(nbMINotebookPage[MI_PAGE_1].hwndNBPage, WM_SETUPNOTEBOOKPAGES, MPFROMHWND(hwndNB), MPFROMP(pSessionData));
                                        /* Set focus */
    WinSetFocus(HWND_DESKTOP, nbMINotebookPage[MI_PAGE_1].ulIDFocus);
    return((MRESULT)TRUE);
    }

case WM_HELP:                           /* Help pressed */
    {
    ULONG   ulPageId, ulPage;
    ULONG   ulHelp[]={MIID_PAGE1};

    ulPageId=(ULONG)WinSendMsg(hwndNB,  /* Query the currently active page */
        BKM_QUERYPAGEID,
        MPFROMLONG(0),                  /* Location page ID, ignored if */
                                        /* Page ID is BKA_TOP */
        MPFROM2SHORT(BKA_TOP, BKA_MAJOR));
    for(ulPage=0; nbMINotebookPage[ulPage].pDialogWndProc!=NULL; ulPage++)
        if(nbMINotebookPage[ulPage].ulIDPage==ulPageId)
            if(pHP->hwndHelp!=NULLHANDLE)
                WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
                    MPFROMSHORT(ulHelp[ulPage]), HM_RESOURCEID);
    }
    break;

case WM_CHAR:
/*                                                                                      *\
 * All window procedure implementing a noteboook page and the notebook control itself,  *
 * which is part of this window's client, forward keystroke messages they don't have    *
 * handled themselfes here.                                                             *
\*                                                                                      */
    {
    static ULONG    ulRecursion=FALSE;

    return(DispatchKeyToNotebook(&ulRecursion, hwndDlg, hwndNB,
        DialogNotebookSubclass[MI_SUBCLASSEDNOTEBOOK].pfnwpNotebook, mp1, mp2));
    }

case WM_COMMAND:                        /* Button pressed */
                                        /* If the dialog will be dismissed, remove the subclassing
                                           of the notebook control within this dialog */
    if((SHORT1FROMMP(mp1)==DID_OK) || (SHORT1FROMMP(mp1)==DID_CANCEL))
        {
        WinSendMsg(nbMINotebookPage[MI_PAGE_1].hwndNBPage, WM_SETUPNOTEBOOKPAGES, MPFROMHWND(NULLHANDLE), (MPARAM)NULL);
        if((pHP->ulVersionMajor==OS2_MAJOR) && (pHP->ulVersionMinor<OS2_MINOR_300))
            {
                                        /* Workaround for OS/2 2.x bug, that causes that the MLE's
                                           return only 1 character per line they contain unless their
                                           notebook page was at least on top once. This is a 2.x defect,
                                           not a PC/2 bug! */
            ULONG       ulPage;

            for(ulPage=0; nbMINotebookPage[ulPage].pDialogWndProc!=NULL; ulPage++)
                WinSendMsg(WinWindowFromID(hwndDlg, MINB_NOTEBOOK), BKM_TURNTOPAGE, MPFROMLONG(nbMINotebookPage[ulPage].ulIDPage), NULL);
            }
        WinSubclassWindow(hwndNB, DialogNotebookSubclass[MI_SUBCLASSEDNOTEBOOK].pfnwpNotebook);
        DialogNotebookSubclass[MI_SUBCLASSEDNOTEBOOK].hwndNotebook=NULLHANDLE;
        DialogNotebookSubclass[MI_SUBCLASSEDNOTEBOOK].pfnwpNotebook=0;
                                        /* Also hide dialog */
        WinShowWindow(hwndDlg, FALSE);
        }
    switch(SHORT1FROMMP(mp1))
    {
    case DID_OK:                        /* Enter key pressed */
/*                                                                                      *\
 * Query the Program Title, Filename, Parameters and DOS Settings and copy them to the  *
 * corresponding entries in the StartSession structure.                                 *
\*                                                                                      */
        {
        WinQueryDlgItemText(nbMINotebookPage[MI_PAGE_1].hwndNBPage, MIEF_MENUTITLE,
            sizeof(pSessionData->PgmTitle), pSessionData->PgmTitle);
        WinQueryDlgItemText(nbMINotebookPage[MI_PAGE_1].hwndNBPage, MIEF_ICONFILE,
            sizeof(pSessionData->IconFile), pSessionData->IconFile);
        }
                                        /* Dialog terminated with DID_OK */
        WinDismissDlg(hwndDlg, DID_OK);
        break;

    case DID_CANCEL:                    /* Escape or Cancel pressed */
                                        /* Dialog terminated with DID_CANCEL */
        WinDismissDlg(hwndDlg, DID_CANCEL);
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Submenu Installation dialog notebook pages. *
 * Req:                                                                                 *
 *      nbMINotebookPage[] ...  Structure of the Submenu Installation notebook pages    *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY MI_NotebookProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Notebook where this window procedure implements notebook pages */
static HWND         hwndNotebook=NULLHANDLE;
static SESSIONDATA  *pSessionData;      /* Pointer to SESSIONDATA structure for current instance */

                                        /* During WinLoadDialog(), PM sends curious WM_CONTROL
                                           messages which causes us troubles as that message
                                           may have consequences on not yet loaded notebook
                                           pages, symptoms were traps in WinWindowFromID()
                                           during DisableDialogItem() */
if((msg!=WM_SETUPNOTEBOOKPAGES) && (hwndNotebook==NULLHANDLE))
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
switch(msg)
{
/*                                                                                      *\
 * Syntax: WM_SETUPNOTEBOOKPAGES, (HWND hwndNotebook), (SESSIONDATA *pSessionData)      *                                           *
\*                                                                                      */
case WM_SETUPNOTEBOOKPAGES:
    {
    hwndNotebook=HWNDFROMMP(mp1);       /* Save notebook we're pages in */
    if(hwndNotebook==NULLHANDLE)
        break;
    pSessionData=(SESSIONDATA *)PVOIDFROMMP(mp2);
/*                                                                                      *\
 * Set the maximum number of chars accepted from the entryfield (thus overwriting the   *
 * default number of 63.                                                                *
\*                                                                                      */
    WinSendDlgItemMsg(nbMINotebookPage[MI_PAGE_1].hwndNBPage, MIEF_MENUTITLE,
        EM_SETTEXTLIMIT, MPFROMSHORT(MAXNAMEL), (MPARAM)NULL);
    WinSetDlgItemText(nbMINotebookPage[MI_PAGE_1].hwndNBPage, MIEF_MENUTITLE, pSessionData->PgmTitle);
    WinSendDlgItemMsg(nbMINotebookPage[MI_PAGE_1].hwndNBPage, MIEF_ICONFILE,
        EM_SETTEXTLIMIT, MPFROMSHORT(CCHMAXPATH), (MPARAM)NULL);
    WinSetDlgItemText(nbMINotebookPage[MI_PAGE_1].hwndNBPage, MIEF_ICONFILE, pSessionData->IconFile);
    }
    break;

case WM_CHAR:
/*                                                                                      *\
 * Process the navigation keys on this page and forward all unprocessed keys to the     *
 * dialog where the notebook is part of.                                                *
\*                                                                                      */
    ProcessPageKey(hwndDlg, WinQueryWindow(hwndNotebook, QW_OWNER), mp1, mp2);
    break;

case WM_COMMAND:                        /* Button pressed */
    switch(SHORT1FROMMP(mp1))
    {
    case DID_CANCEL:                    /* Cancel key pressed on a dialog within a notebook page,
                                           ignore it within dialog but post it to parent dialog */
                                        /* Post DID_CANCEL to parent of notebook which is the dialog */
        WinPostMsg(WinQueryWindow(hwndNotebook, QW_PARENT), WM_COMMAND,
            MPFROMSHORT(DID_CANCEL), MPFROM2SHORT(CMDSRC_ACCELERATOR, FALSE));
                                        /* We have handled the message */
        return((MRESULT)TRUE);

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

                                        /* Call function instead of subclassing notebook page 1
                                           notebook dialogs */
case DM_DRAGOVER:
case DM_DROP:
                                        /* Only on first page allow Drag'n Drop */
    if(hwndDlg==nbMINotebookPage[MI_PAGE_1].hwndNBPage)
        return(DragDropDialogProcedure(hwndDlg, msg, mp1, mp2, MIEF_ICONFILE));
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Control Configuration dialog.               *
 * Req:                                                                                 *
 *      StartSession .. a structure of type SESSIONDATA where the information entered   *
 *                      in the dialog is entered. If DID_OK is pressed this structure   *
 *                      is used to start the session and set its priority.              *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY CC_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static HWND         hwndNB;             /* Control Configuration dialog notebook window handle */
static SESSIONDATA  *pSessionData;      /* Pointer to SESSIONDATA structure for current instance */

switch(msg)
{
case WM_INITDLG:
    {
    ULONG       ulNBStyle;              /* Notebook style (to take the new Merlin notebook style into
                                           account, just try out SET NEWNOTEBOOS=YES in your Merlin
                                           beta CONFIG.SYS with your "old"-style notebooks).
                                           Under OS/2 WARP 4 (aka "Merlin"), there seems to be a bug,
                                           as a notebook created from a dialog resouce must already
                                           have the BKS_TABBEDNOTEBOOK style set when being created,
                                           as setting the window style dynamically does corrupt the
                                           notebook drawing */
    USHORT      usTabTextLength=50;
    USHORT      usTabTextHeight=5;

                                        /* Access parameters passed */
    pSessionData=(SESSIONDATA *)PVOIDFROMMP(mp2);
    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
                                        /* Change dialog titlebar, as we're reusing the Program Installation's
                                           dialog */
    WinSetWindowText(WinWindowFromID(hwndDlg, FID_TITLEBAR), "PC/2 - Program Commander/2 (Control Configuration)");
    ulNBStyle=WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE;
    if(((pHP->ulVersionMajor==OS2_MAJOR) && (pHP->ulVersionMinor>=OS2_MINOR_400)) ||
        (pHP->ulVersionMajor>OS2_MAJOR))
        ulNBStyle|=(BKS_TABBEDDIALOG|BKS_MAJORTABTOP|BKS_BACKPAGESTR|
            BKS_STATUSTEXTRIGHT|BKS_TABTEXTCENTER|BKS_POLYGONTABS);
    else
        ulNBStyle|=(BKS_SPIRALBIND|BKS_MAJORTABRIGHT|BKS_BACKPAGESBR|
            BKS_STATUSTEXTCENTER|BKS_TABTEXTRIGHT);
    hwndNB=WinCreateWindow(hwndDlg, WC_NOTEBOOK, "", ulNBStyle, 0, 0, 0, 0, hwndDlg,
        HWND_BOTTOM, CCNB_NOTEBOOK, NULL, NULL);
                                        /* As the Lockup Settings notebook page may display a message
                                           box being this dialog the owner, associate the main helptable
                                           with this dialog */
    WinAssociateHelpInstance(pHP->hwndHelp, hwndDlg);
                                        /* Subclass notebook's window procedure to add handling of
                                           notebook pages, notebook and outer dialog accelerator keys */
    DialogNotebookSubclass[CC_SUBCLASSEDNOTEBOOK].hwndNotebook=hwndNB;
    DialogNotebookSubclass[CC_SUBCLASSEDNOTEBOOK].pfnwpNotebook=
        WinSubclassWindow(hwndNB, SubclassedNotebookProcedure);
                                        /* Fill the notebook and return the tab text requirements */
    AddPagesToNotebook(hwndDlg, hwndNB, nbCCNotebookPage, &usTabTextLength, &usTabTextHeight);
                                        /* Adjust dialog to optimally cover the notebook */
    AdjustNotebook(hwndDlg, hwndNB, ulNBStyle, usTabTextLength, usTabTextHeight);
                                        /* Now initialize all notebook page dialogs
                                           by posting to 1st page (the subclassed entryfield
                                           are already chained in) */
    WinPostMsg(nbCCNotebookPage[CC_PAGE_1].hwndNBPage, WM_SETUPNOTEBOOKPAGES, MPFROMHWND(hwndNB), MPFROMP(pSessionData));
                                        /* Set focus */
    WinSetFocus(HWND_DESKTOP, nbCCNotebookPage[DD_PAGE_VIRTUAL1].ulIDFocus);
    return((MRESULT)TRUE);
    }

case WM_DESTROY:
                                        /* Deassociate help instance */
    WinAssociateHelpInstance(NULLHANDLE, hwndDlg);
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));

case WM_HELP:                           /* Help pressed */
    {
    ULONG   ulPageId, ulPage;
    ULONG   ulHelp[]={CCID_PAGE1, CCID_PAGE2};

    ulPageId=(ULONG)WinSendMsg(hwndNB,  /* Query the currently active page */
        BKM_QUERYPAGEID,
        MPFROMLONG(0),                  /* Location page ID, ignored if */
                                        /* Page ID is BKA_TOP */
        MPFROM2SHORT(BKA_TOP, BKA_MAJOR));
    for(ulPage=0; nbCCNotebookPage[ulPage].pDialogWndProc!=NULL; ulPage++)
        if(nbCCNotebookPage[ulPage].ulIDPage==ulPageId)
            if(pHP->hwndHelp!=NULLHANDLE)
                WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
                    MPFROMSHORT(ulHelp[ulPage]), HM_RESOURCEID);
    }
    break;

case WM_CHAR:
/*                                                                                      *\
 * All window procedure implementing a noteboook page and the notebook control itself,  *
 * which is part of this window's client, forward keystroke messages they don't have    *
 * handled themselfes here.                                                             *
\*                                                                                      */
    {
    static ULONG    ulRecursion=FALSE;

    return(DispatchKeyToNotebook(&ulRecursion, hwndDlg, hwndNB,
        DialogNotebookSubclass[CC_SUBCLASSEDNOTEBOOK].pfnwpNotebook, mp1, mp2));
    }

case WM_COMMAND:                        /* Button pressed */
                                        /* If the dialog will be dismissed, remove the subclassing
                                           of the notebook control within this dialog */
    if((SHORT1FROMMP(mp1)==DID_OK) || (SHORT1FROMMP(mp1)==DID_CANCEL))
        {
                                        /* Inform notebook page dialog procedure that we're closing down */
        WinSendMsg(nbCCNotebookPage[CC_PAGE_1].hwndNBPage, WM_SETUPNOTEBOOKPAGES, MPFROMHWND(NULLHANDLE), (MPARAM)NULL);
        WinSubclassWindow(hwndNB, DialogNotebookSubclass[CC_SUBCLASSEDNOTEBOOK].pfnwpNotebook);
        DialogNotebookSubclass[CC_SUBCLASSEDNOTEBOOK].hwndNotebook=NULLHANDLE;
        DialogNotebookSubclass[CC_SUBCLASSEDNOTEBOOK].pfnwpNotebook=0;
                                        /* Also hide dialog */
        WinShowWindow(hwndDlg, FALSE);
        }
    switch(SHORT1FROMMP(mp1))
    {
    case DID_OK:                        /* Enter key pressed */
/*                                                                                      *\
 * Query Program Scheduler data for the information about automatic scheduling of the   *
 * menuentry.                                                                           *
\*                                                                                      */
        GetSchedulerData(&pSessionData->SDScheduler, nbCCNotebookPage[CC_PAGE_2].hwndNBPage);
                                        /* Dialog terminated with DID_OK */
        WinDismissDlg(hwndDlg, DID_OK);
        break;

    case DID_CANCEL:                    /* Escape or Cancel pressed */
                                        /* Dialog terminated with DID_CANCEL */
        WinDismissDlg(hwndDlg, DID_CANCEL);
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Control Configuration dialog notebook pages *
 * Req:                                                                                 *
 *      nbCCNotebookPage[] ...  Structure of the Control Configuration notebook pages   *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY CC_NotebookProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Notebook where this window procedure implements notebook pages */
static HWND         hwndNotebook=NULLHANDLE;
static SESSIONDATA  *pSessionData;      /* Pointer to SESSIONDATA structure for current instance */

                                        /* During WinLoadDialog(), PM sends curious WM_CONTROL
                                           messages which causes us troubles as that message
                                           may have consequences on not yet loaded notebook
                                           pages, symptoms were traps in WinWindowFromID()
                                           during DisableDialogItem() */
if((msg!=WM_SETUPNOTEBOOKPAGES) && (hwndNotebook==NULLHANDLE))
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
switch(msg)
{
/*                                                                                      *\
 * Syntax: WM_SETUPNOTEBOOKPAGES, (HWND hwndNotebook), (SESSIONDATA *pSessionData)      *                                           *
\*                                                                                      */
case WM_SETUPNOTEBOOKPAGES:
    {
    hwndNotebook=HWNDFROMMP(mp1);       /* Save notebook we're pages in */
    if(hwndNotebook==NULLHANDLE)
        break;
                                        /* Access parameters passed */
    pSessionData=(SESSIONDATA *)PVOIDFROMMP(mp2);
/*                                                                                      *\
 * Now we initialize the Control settings notebook page.                                *
\*                                                                                      */
    WinSendDlgItemMsg(                  /* Send message to dialog window */
                                        /* Handle of page 1 */
        nbCCNotebookPage[CC_PAGE_1].hwndNBPage,
        CCEF_CONTROL,                   /* Control title entryfield */
        EM_SETTEXTLIMIT,                /* Set text limit to 60 */
        MPFROMSHORT(MAXNAMEL),
        (MPARAM)NULL);                  /* No additional parameter */
    WinSetDlgItemText(                  /* Load the default text of the entryfield */
        nbCCNotebookPage[CC_PAGE_1].hwndNBPage, CCEF_CONTROL, pSessionData->PgmTitle);
/*                                                                                      *\
 * Now we initialize the Scheduler settings notebook page.                              *
\*                                                                                      */
    SetSchedulerData(&pSessionData->SDScheduler, nbCCNotebookPage[CC_PAGE_2].hwndNBPage);
                                        /* All pages initialized */
    }
    break;

case WM_CHAR:
/*                                                                                      *\
 * Process the navigation keys on this page and forward all unprocessed keys to the     *
 * dialog where the notebook is part of.                                                *
\*                                                                                      */
    ProcessPageKey(hwndDlg, WinQueryWindow(hwndNotebook, QW_OWNER), mp1, mp2);
    break;

case WM_CONTROL:                        /* Test for autoradio buttons pressed */
/*                                                                                      *\
 * The Size & Position radiobutton must be checked every time it is selected, because   *
 * only if it is selected, the user may have write access to X, Y, Size X and Size Y    *
 * entryfields, otherwise they are read only. The DOS Setting MLE is only visible if a  *
 * DOS window or fullscreen application type is selected.                               *
\*                                                                                      */
    {
                                        /* Was button class window clicked or double-clicked ? */
    if((SHORT2FROMMP(mp1)==BN_CLICKED) || (SHORT2FROMMP(mp1)==BN_DBLCLICKED))
        switch(SHORT1FROMMP(mp1))
        {
        case CCRB_DISABLESCHEDULER:
        case CCRB_SCHEDULEHOURLY:
        case CCRB_SCHEDULEDAILY:
        case CCRB_SCHEDULEWEEKLY:
        case CCRB_SCHEDULEMONTHLY:
        case CCRB_SCHEDULEONCE:
            CheckSchedulerSettings(SHORT1FROMMP(mp1), nbCCNotebookPage[CC_PAGE_2].hwndNBPage);
            break;
        }
    }
    break;

case WM_COMMAND:                        /* Button pressed */
    switch(SHORT1FROMMP(mp1))
    {
    case DID_CANCEL:                    /* Cancel key pressed on a dialog within a notebook page,
                                           ignore it within dialog but post it to parent dialog */
                                        /* Post DID_CANCEL to parent of notebook which is the dialog */
        WinPostMsg(WinQueryWindow(hwndNotebook, QW_PARENT), WM_COMMAND,
            MPFROMSHORT(DID_CANCEL), MPFROM2SHORT(CMDSRC_ACCELERATOR, FALSE));
                                        /* We have handled the message */
        return((MRESULT)TRUE);

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Control Addition dialog.                    *
 * Req:                                                                                 *
 *      SessionData ... a structure of type SESSIONDATA where the control style         *
 *                      selected in the dialog is entered. If DID_OK is pressed this    *
 *                      structure is used to add the control entries to the Popup-menu. *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY MD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static SESSIONDATA   *pSessionData;     /* Pointer to SESSIONDATA structure for current instance */

switch(msg)
{
case WM_INITDLG:
    {
    ULONG       ulid;
    ULONG       ulidCopy;
                                        /* Sort radiobuttons as the Menuentry IDs are
                                           sorted in PC2.h as ID_* */
    USHORT      usChild2Control[]={MDRB_CTRL_CONFIGMENU, MDRB_CTRL_CONFIGDESKTOP, MDRB_CTRL_ABOUT,
                                   MDRB_CTRL_SHUTDOWN, MDRB_CTRL_POWERDOWN, MDRB_CTRL_HELP, MDRB_CTRL_EXIT,
                                   MDRB_CTRL_SPOOLER, MDRB_CTRL_LOCKUP};

    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
                                        /* Center frame window */
    CenterWindow(hwndDlg);
                                        /* Access parameters passed */
    pSessionData=(SESSIONDATA *)PVOIDFROMMP(mp2);
    for(ulid=0; ulid<=(sizeof(usChild2Control)/sizeof(usChild2Control[0])); ulid++)
        {
        ulidCopy=ulid+ID_CONFIGDIALOG;  /* Copy because SearchItem() changes passed ID */
                                        /* Search in the linked list for entry ulid */
        if((SearchItem(pHP->pPopupMenu, &ulidCopy))!=NULL)
                                        /* Disable and show 1 Control child windows
                                           according to its id if id was found */
            DisableDialogItem(hwndDlg, &usChild2Control[ulid],
                1, WS_VISIBLE | WS_DISABLED);
        else
            {
                                        /* If the Popup Menu does not contain entry ulid,
                                           but the clipboard does, disable and show control too */
            if((pHP->pMenuDataCutPaste) && (SearchItem(pHP->pMenuDataCutPaste, &ulidCopy)))
                DisableDialogItem(hwndDlg, &usChild2Control[ulid],
                    1, WS_VISIBLE | WS_DISABLED);
            }
        }
                                        /* Select Separator RB because this is the only
                                           one selectable all times */
    WinSendDlgItemMsg(hwndDlg, MDRB_CTRL_SEPARATOR, BM_SETCHECK,
        MPFROMSHORT(TRUE), (MPARAM)NULL);
    break;
    }

case WM_HELP:                           /* Help pressed */
    if(pHP->hwndHelp!=NULLHANDLE)
        WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
            MPFROMSHORT(MDID_CONTROLDIALOG), HM_RESOURCEID);
    break;

case WM_COMMAND:                        /* Button pressed */
    switch(SHORT1FROMMP(mp1))
    {
    case DID_OK:                        /* Enter key pressed */
/*                                                                                      *\
 * Query through all radiabuttons for the control style the user has selected. Write    *
 * the corresponding string name into the  pSessionData->PgmTitle field.                *
\*                                                                                      */
        pSessionData->id=0;
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_CONFIGMENU))
            {
            strcpy(pSessionData->PgmTitle, CTRL_CONFIGMENU);
            pSessionData->id=ID_CONFIGDIALOG;
            }
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_CONFIGDESKTOP))
            {
            strcpy(pSessionData->PgmTitle, CTRL_CONFIGDESKTOP);
            pSessionData->id=ID_DESKTOPDIALOG;
            }
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_ABOUT))
            {
            strcpy(pSessionData->PgmTitle, CTRL_ABOUT);
            pSessionData->id=ID_ABOUTDIALOG;
            }
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_SHUTDOWN))
            {
            strcpy(pSessionData->PgmTitle, CTRL_SHUTDOWN);
            pSessionData->id=ID_SHUTDOWN;
            }
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_POWERDOWN))
            {
            strcpy(pSessionData->PgmTitle, CTRL_POWERDOWN);
            pSessionData->id=ID_POWERDOWN;
            }
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_HELP))
            {
            strcpy(pSessionData->PgmTitle, CTRL_HELP);
            pSessionData->id=ID_HELP;
            }
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_EXIT))
            {
            strcpy(pSessionData->PgmTitle, CTRL_EXIT);
            pSessionData->id=ID_EXIT;
            }
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_SPOOLER))
            {
            strcpy(pSessionData->PgmTitle, CTRL_SPOOLER);
            pSessionData->id=ID_SPOOLER;
            }
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_LOCKUP))
            {
            strcpy(pSessionData->PgmTitle, CTRL_LOCKUP);
            pSessionData->id=ID_LOCKUP;
            }
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_SEPARATOR))
            strcpy(pSessionData->PgmTitle, CTRL_SEPARATOR);
        if(WinQueryButtonCheckstate(hwndDlg, MDRB_CTRL_BRKSEPARATOR))
            strcpy(pSessionData->PgmTitle, CTRL_BREAKSEPARATOR);
                                        /* Dialog terminated with DID_OK */
        WinDismissDlg(hwndDlg, DID_OK);
        break;

    case DID_CANCEL:                    /* Escape or Cancel pressed */
                                        /* Dialog terminated with DID_CANCEL */
        WinDismissDlg(hwndDlg, DID_CANCEL);
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}


/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - ShutDown OS/2 and PowerDown OS/2 dialogs.   *
 * Req: none                                                                            *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY SD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
SHUTDOWNPARAMETERS  *psdParms;

switch(msg)
{
case WM_INITDLG:
    psdParms=(SHUTDOWNPARAMETERS *)mp2;
    WinSetWindowULong(hwndDlg, QWL_USER, (ULONG)psdParms);
                                        /* As dialog may display a message box being this dialog the
                                           owner, associate the main helptable with this dialog */
    WinAssociateHelpInstance(pHP->hwndHelp, hwndDlg);
    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
                                        /* Load before ShutDown info */
    if(psdParms->ulStatusFlag & SHUTDOWN_SHUTDOWN)
        {
        WinSetDlgItemText(hwndDlg, SDID_SHUTDOWNMESSAGE,
            "All disk activity will be suspended. After all disk activity has been stopped, "\
            "it is safe to power off or reset your PC.");
        }
    else
        {
        if(pHP->ulStatusFlag2 & POWERDOWNSUSPEND)
            WinSetDlgItemText(hwndDlg, SDID_SHUTDOWNMESSAGE,
                "Your PC will be suspended.");
        else
            WinSetDlgItemText(hwndDlg, SDID_SHUTDOWNMESSAGE,
                "All disk activity will be suspended. After all disk activity has been stopped, "\
                "your PC will power off automatically.");
        }
                                        /* Center frame window */
    CenterWindow(hwndDlg);
                                        /* Broadcast WM_SAVEAPPLICATION */
    WinBroadcastMsg(HWND_DESKTOP, WM_SAVEAPPLICATION, NULL, NULL,
        BMSG_POST|BMSG_DESCENDANTS|BMSG_FRAMEONLY);
                                        /* Do the ShutDown/PowerDown countdown */
    WinPostMsg(hwndDlg, WM_SHUTDOWN, mp2, NULL);
    break;

/*                                                                                      *\
 * Syntax: WM_SHUTDOWN, (SHUTDOWNPARAMETERS *psdParms), ULONG ulDelayCounter            *
\*                                                                                      */
case WM_SHUTDOWN:
/*                                                                                      *\
 * While informing the user about PC/2 is shutting down OS/2, shut down the file system *
 * and reboot the system after waiting 2 seconds for the user to read about PC/2        *
 * rebooting immediately or inform the user he can reboot manually. Even if this is our *
 * PM thread, blocking the message queue now can be ignored as we shut down anyway.     *
\*                                                                                      */
    {
    ULONG   ulAction=0;
    APIRET  apiretDD=NO_ERROR;
    ULONG   ulDelayCounter=LONGFROMMP(mp2);

                                        /* Shutdown counter in milliseconds */
#define SHUTDOWN_COUNTER                50
                                        /* WM_SAVEAPPLICATION delay in seconds */
#define SAVEAPPLICATION_DELAY           5

                                        /* If the counter has not expired yet, wait a while, increment
                                           the counter and repost the message again to give
                                           WM_SAVEAPPLICATION some time */
    if(ulDelayCounter<=((1000/SHUTDOWN_COUNTER)*SAVEAPPLICATION_DELAY))
        {
        DosSleep(SHUTDOWN_COUNTER);
        ulDelayCounter++;
        WinPostMsg(hwndDlg, WM_SHUTDOWN, mp1, MPFROMLONG(ulDelayCounter));
        return((MRESULT)TRUE);
        }
                                        /* Get parameters */
    psdParms=(SHUTDOWNPARAMETERS *)WinQueryWindowULong(hwndDlg, QWL_USER);
                                        /* Check for ShutDown OS/2 */
    if(psdParms->ulStatusFlag & SHUTDOWN_SHUTDOWN)
        {
                                        /* Open the DOS DD which can be used to request a reboot */
        if(pHP->ulStatusFlag1 & SHUTDOWNREBOOT)
            {
            apiretDD=DosOpen("\\DEV\\DOS$", &psdParms->hfileDD, &ulAction, 0, FILE_NORMAL,
                OPEN_ACTION_OPEN_IF_EXISTS,
                OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE, 0);
            if(apiretDD!=NO_ERROR)
                {
#ifdef  DEBUG
                printf("Opening DOS$ failed %08X\n", (int)apiretDD);
#endif  /* DEBUG */
                                        /* In case of an error, we can't reboot automatically,
                                           so inform the user (which of course doesn't help
                                           automated service machines */
                if(pHP->ulStatusFlag1 & SHUTDOWNREBOOT)
                    USR_ERR(hwndDlg, HELP_PC2REBOOT, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                        "PC/2 can't reboot automatically - please press CTRL+ALT+DEL after Shutdown "\
                        "of OS/2 has been completed!");
                }
            }
                                        /* Shutdown the system anyway */
        DosShutdown(0);
                                        /* If the user wants us to reboot, force a reboot. If
                                           the reboot doesn't work, inform user and try again. If
                                           the user just wanted to perform a shutdown, inform user
                                           when he can manually power off or reboot */
        if((pHP->ulStatusFlag1 & SHUTDOWNREBOOT) && (apiretDD==NO_ERROR))
            {
                                        /* Inform the user we're going to reboot */
           WinSetDlgItemText(hwndDlg, SDID_SHUTDOWNMESSAGE,
               "All disk activity has been stopped. Resetting your workstation...");
           DosSleep(02000);
           if(NO_ERROR!=DosDevIOCtl(psdParms->hfileDD, IOCTL_DOS, DOS_REBOOT, NULL, 0, NULL, NULL, 0, NULL))
               {
               WinSetDlgItemText(hwndDlg, SDID_SHUTDOWNMESSAGE,
                   "Resetting your workstation failed - retrying...");
               WinPostMsg(hwndDlg, msg, mp1, mp2);
               }
           DosClose(psdParms->hfileDD);
           }
       else
           {
           WinSetDlgItemText(hwndDlg, SDID_SHUTDOWNMESSAGE,
               "All disk activity has been stopped. It is now safe to power off, reset or "\
               "reboot your system. ");
           }
        }
                                        /* Check for PowerDown OS/2 */
    if(psdParms->ulStatusFlag & SHUTDOWN_POWERDOWN)
        {
        apiretDD=DosOpen("\\DEV\\APM$", &psdParms->hfileDD, &ulAction, 0, FILE_NORMAL,
            OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE, 0);
        if(apiretDD==NO_ERROR)
            {
            POWEREVENT  powereventAPM;
            ULONG       ulReturnCode;
            ULONG       ulPacketSize;
            ULONG       ulDataSize;

            memset(&powereventAPM, 0, sizeof(powereventAPM));
            ulReturnCode=0;
                                        /* Set PWR event */
            powereventAPM.usSubID=0x0006;
                                        /* All APM devices */
            powereventAPM.usData1=0x0001;
                                        /* APM request (we use either suspend or power off) */
            if(pHP->ulStatusFlag2 & POWERDOWNSUSPEND)
                powereventAPM.usData2=2;
            else
                powereventAPM.usData2=3;
            ulPacketSize=sizeof(powereventAPM);
            ulDataSize=sizeof(ulReturnCode);
            apiretDD=DosDevIOCtl(psdParms->hfileDD, IOCTL_POWER, POWER_SENDPOWEREVENT,
                &powereventAPM, ulPacketSize, &ulPacketSize,
                &ulReturnCode, ulDataSize, &ulDataSize);
            }
        if(apiretDD!=NO_ERROR)
            {
#ifdef  DEBUG
            printf("Opening APM$ failed %08X\n", (int)apiretDD);
#endif  /* DEBUG */
                                        /* In case of an error, we can't suspend or power off,
                                           so inform the user */
            USR_ERR(hwndDlg, HELP_POWERDOWN, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "PC/2 can't suspend or power off your PC!");
            }
        DosClose(psdParms->hfileDD);
        if(apiretDD==NO_ERROR)
            DosSleep(10000);
        WinDismissDlg(hwndDlg, DID_OK);
        }
    }
    break;

case WM_COMMAND:
                                        /* For the ShutDown OS/2 dialog eat up keys to avoid
                                           dismission of dialog with ESC key */
    return((MRESULT)TRUE);
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Startup Environment Spaces dialog.          *
 * Req: **pEnvironmentSpaces                                                            *
 *                  A pointer to a character string pointer pointing to the Environment *
 *                  spaces selected from the Program Installation dialog, which the     *
 *                  user may override here.                                             *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY SE_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
COMMANDLINEENVIRONMENT  *pCLECurrent;

switch(msg)
{
case WM_INITDLG:
    {
    ENVIRONMENTLIST     *pEnvironmentListCurrent;
    ULONG               ulIndex;

    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
    pCLECurrent=PVOIDFROMMP(mp2);
    WinSetWindowULong(hwndDlg, QWL_USER, (ULONG)pCLECurrent);
                                        /* Program to be started information text field */
    WinSetDlgItemText(hwndDlg, SETF_STARTUPPROGRAM, pCLECurrent->ucPgmTitle);
                                        /* Load available environment spaces sequentially */
    for(pEnvironmentListCurrent=pHP->pEnvironmentSpaces;
        pEnvironmentListCurrent!=NULL;
        pEnvironmentListCurrent=pEnvironmentListCurrent->pNext)
        {
        ulIndex=(ULONG)WinSendDlgItemMsg(hwndDlg, SELB_ENVIRONMENTS,
            LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(pEnvironmentListCurrent->pEnvironmentName));
        if(strstr(pCLECurrent->pucEnvironmentSpaces, pEnvironmentListCurrent->pEnvironmentName))
            WinSendDlgItemMsg(hwndDlg, SELB_ENVIRONMENTS,
                LM_SELECTITEM, MPFROMSHORT(ulIndex), (MPARAM)TRUE);
        }
    break;
    }

case WM_HELP:                           /* Help pressed */
    if(pHP->hwndHelp!=NULLHANDLE)
        WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
            MPFROMSHORT(SEID_STARTUPDIALOG), HM_RESOURCEID);
    break;

case WM_COMMAND:                        /* Button pressed */
    switch(SHORT1FROMMP(mp1))
    {
    case DID_OK:                        /* Enter key pressed */
        pCLECurrent=(COMMANDLINEENVIRONMENT *)WinQueryWindowULong(hwndDlg, QWL_USER);
        free(pCLECurrent->pucEnvironmentSpaces);
        pCLECurrent->pucEnvironmentSpaces=strdup("");
        pCLECurrent->pucEnvironmentSpaces=QueryEnvironmentSpaces(pCLECurrent->pucEnvironmentSpaces,
            hwndDlg, SELB_ENVIRONMENTS);
    case DID_CANCEL:                    /* Escape or Cancel pressed */
                                        /* Dialog terminated with DID_OK or DID_CANCEL */
        WinDismissDlg(hwndDlg, SHORT1FROMMP(mp1));
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Startup Parameters dialog.                  *
 * Req: *PgmInputs .....A pointer to a string of commandline params of an application   *
 *                      via mp2                                                         *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY SU_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
COMMANDLINEPARAMS   *pCLPParams;

switch(msg)
{
case WM_INITDLG:
    {
    UCHAR       ucBuffer[EF_SIZE255];
    ULONG       ulPUserSize=EF_SIZE255;

    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
    pCLPParams=PVOIDFROMMP(mp2);
    WinSetWindowULong(hwndDlg, QWL_USER, (ULONG)pCLPParams);
/*                                                                                      *\
 * Load text from CLPParams into the dialog box text fields, and set the text limit of  *
 * the entryfield.                                                                      *
\*                                                                                      */
                                        /* Program to be started information text field */
    WinSetDlgItemText(hwndDlg, SUTF_STARTUPPROGRAM, pCLPParams->ucPgmTitle);
    sprintf(ucBuffer, "%s[...] %s", pCLPParams->ucPBefore, pCLPParams->ucPAfter);
                                        /* User information text field */
    WinSetDlgItemText(hwndDlg, SUTF_STARTUPINFO, pCLPParams->ucPUser);
                                        /* Current parameters text field */
    ulPUserSize=EF_SIZE255;
    ulPUserSize-=strlen(pCLPParams->ucPBefore);
    ulPUserSize-=strlen(pCLPParams->ucPAfter);
    WinSendDlgItemMsg(hwndDlg, SUEF_STARTUPPARAMETERS, EM_SETTEXTLIMIT,
        MPFROMSHORT(ulPUserSize), (MPARAM)NULL);
    WinSetDlgItemText(hwndDlg, SUTF_STARTUPPARAMETERS, ucBuffer);
    break;
    }

case WM_HELP:                           /* Help pressed */
    if(pHP->hwndHelp!=NULLHANDLE)
        WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
            MPFROMSHORT(SUID_STARTUPDIALOG), HM_RESOURCEID);
    break;

case WM_COMMAND:                        /* Button pressed */
    pCLPParams=(COMMANDLINEPARAMS *)WinQueryWindowULong(hwndDlg, QWL_USER);
    switch(SHORT1FROMMP(mp1))
    {
    case SUPB_FILE:
        {
                                        /* Repeated invocations of the standard file dialog adds
                                           the selection to the previous one */
        UCHAR       ucPUser[EF_SIZE255];
        ULONG       ulPUserSize=EF_SIZE255;
        FILEDLG     fdFileDlg;          /* Standard file dialog control */
        HWND        hwndFileDlg;        /* Handle of standard file dialog */

                                        /* Query current contents */
        WinQueryWindowText(WinWindowFromID(hwndDlg, SUEF_STARTUPPARAMETERS), sizeof(ucPUser), ucPUser);
        ulPUserSize=0;
        ulPUserSize+=strlen(pCLPParams->ucPBefore);
        ulPUserSize+=strlen(pCLPParams->ucPAfter);
        ulPUserSize+=strlen(ucPUser);
                                        /* Clear out structure */
        memset(&fdFileDlg, 0, sizeof(FILEDLG));
        fdFileDlg.cbSize=sizeof(FILEDLG);
        fdFileDlg.fl=FDS_CENTER | FDS_OPEN_DIALOG | FDS_MULTIPLESEL;
        fdFileDlg.pszOKButton="~Select";
        fdFileDlg.pszTitle="PC/2 Startup Parameter(s)";
        if(pCLPParams->pucCurrentDirectory[0]!='\0')
            {
            fdFileDlg.pszIDrive=pCLPParams->pucCurrentDirectory;
            strcpy(fdFileDlg.szFullFile, "*");
            }
        else
            {
            strcpy(fdFileDlg.szFullFile, "C:\\*");
            fdFileDlg.szFullFile[0]=pHP->ucBootDrive;
            }
                                        /* Open the standard file dialog ...*/
        hwndFileDlg=WinFileDlg(HWND_DESKTOP, hwndDlg, &fdFileDlg);
        if(hwndFileDlg && (fdFileDlg.lReturn==DID_OK))
            {
            ULONG   ulIndex;

                                        /* Add selected files if their length do fit */
            for(ulIndex=0; ulIndex<fdFileDlg.ulFQFCount; ulIndex++)
                {
                ULONG   ulLength=strlen(*fdFileDlg.papszFQFilename[ulIndex])+1;

                if(ulPUserSize+ulLength<EF_SIZE255)
                    {
                    strcat(ucPUser, " ");
                    strcat(ucPUser, *fdFileDlg.papszFQFilename[ulIndex]);
                    }
                }
            WinFreeFileDlgList(fdFileDlg.papszFQFilename);
            }
                                        /* Set back new contents */
        WinSetDlgItemText(hwndDlg, SUEF_STARTUPPARAMETERS, ucPUser);
        }
        break;

    case DID_OK:                        /* Enter key pressed */
    case DID_CANCEL:                    /* Escape or Cancel pressed */
        WinQueryWindowText(WinWindowFromID(hwndDlg, SUEF_STARTUPPARAMETERS),
            sizeof(pCLPParams->ucPUser), pCLPParams->ucPUser);
                                        /* Dialog terminated with DID_OK or DID_CANCEL */
        WinDismissDlg(hwndDlg, SHORT1FROMMP(mp1));
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Resort dialog.                              *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY RD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static MENUDATA *pMDSorted;             /* Pointer to the first element of the resorted
                                           linked list */
static MENUDATA *pMDParent;             /* Menuitem, which current level is a child of */
static USHORT   usChild2InitPBs[]={DID_OK};
static USHORT   usChild2MovePBs[]={DID_CANCEL};
static USHORT   usChild2OKPBs[]={RDPB_MOVE};
static BOOL     bFirstMove;
switch(msg)
{
case WM_INITDLG:
    {
    MENUDATA    *pMDSource;             /* Pointer withing the current linked list */

    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
                                        /* Center frame window */
    CenterWindow(hwndDlg);
/*                                                                                      *\
 * Disable OK pushbutton, until all items have been moved. Get the first element of the *
 * current level of the linked list which will be resorted. Add all Menuitems of the    *
 * current level of the linked list to the source listbox. Save the parent Menuitem,    *
 * from where the current level is a child of.                                          *
\*                                                                                      */
    bFirstMove=FALSE;                   /* We didn't make any sort before */
    pMDSorted=NULL;                     /* Pointer of resorted linked list points to a non
                                           available Menuitem */
                                        /* Disable and show OK pushbutton */
    DisableDialogItem(hwndDlg, usChild2InitPBs,
        sizeof(usChild2InitPBs)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                                        /* Is current level is the root of all levels ? */
    if(pHP->pMenuData==pHP->pPopupMenu)
        pMDParent=NULL;                 /* Yes, then parent Menuitem is NULL */
    else                                /* No, then get parent Menuitem */
        pMDParent=pHP->pMenuData->Back;
                                        /* Get first element of linked list */
    pMDSource=pHP->pMenuData;
    do                                  /* Add current level of Menuitems to the source listbox */
    {
        if(pMDSource->Item==ENTRYSUBMENU) /* It is a Submenu */
            {
            UCHAR       ucBuffer[MAXNAMEL+4];
                                        /* Add >> for a Submenu */
            sprintf(ucBuffer, "%s >>", pMDSource->PgmTitle);
                                        /* Insert Menuitem at the end of the listbox */
            WinSendDlgItemMsg(hwndDlg, RDLB_SOURCEMENU, LM_INSERTITEM,
                MPFROMSHORT(LIT_END), MPFROMP(ucBuffer));
            }
                                        /* It's a Menuitem or Control */
        if((pMDSource->Item==ENTRYMENUITEM) || (pMDSource->Item==ENTRYCONTROL))
            WinSendDlgItemMsg(hwndDlg, RDLB_SOURCEMENU, LM_INSERTITEM,
                MPFROMSHORT(LIT_END), MPFROMP(pMDSource->PgmTitle));
                                        /* It may also be an empty entry, but then we
                                           ignore it, because it must be filled with
                                           Menuitem or Submenu data first */
        if(pMDSource->Next!=NULL)       /* Get through linked list without diving into
                                           Submenus */
                pMDSource=pMDSource->Next;
        else break;                     /* We're at the end of the linked list */
    } while(TRUE);
    break;
    }

case WM_HELP:                           /* Help pressed */
    if(pHP->hwndHelp!=NULLHANDLE)
        WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
            MPFROMSHORT(RDID_RESORTDIALOG), HM_RESOURCEID);
    break;

case WM_CONTROL:
    switch(SHORT1FROMMP(mp1))
    {
    case RDLB_SOURCEMENU:
        if(SHORT2FROMMP(mp1)==LN_ENTER)
                                        /* Simulate clicking the Resort pushbutton */
            WinPostMsg(hwndDlg, WM_COMMAND,
                MPFROM2SHORT(RDPB_MOVE, CMDSRC_PUSHBUTTON), MPFROMSHORT(TRUE));
        break;
    }
    break;

case WM_COMMAND:                        /* Button pressed */
    switch(SHORT1FROMMP(mp1))
    {
    case RDPB_MOVE:                     /* Move pushbutton */
        {
        MENUDATA        *pMDSource;     /* Walk through current linked list */
        MENUDATA        *pMDSourceNext; /* Next menuentry after pMDSource */
                                        /* Walk through destination linked list */
        MENUDATA        *pMDDestination;
        SHORT           sCount;
        SHORT           sCountSave;

/*                                                                                      *\
 * Now get the selected Menuitem of the source listbox, delete it from the listbox. If  *
 * the source listbox is now empty, reenable the OK pushbutton again.                   *
\*                                                                                      */
                                        /* Query the selected Menuitem from the source listbox */
        sCountSave=sCount=(LONG)WinSendDlgItemMsg(hwndDlg, RDLB_SOURCEMENU, LM_QUERYSELECTION,
            MPFROMSHORT(LIT_FIRST), (MPARAM)NULL);
        while(sCount!=LIT_NONE)
            {
                                        /* Point to the first element of the linked list */
            pMDSource=pHP->pMenuData;
            for( ; sCount>0; sCount--)  /* Walk through the linked list to the selected
                                           item */
                pMDSource=pMDSource->Next;
                                        /* Point to the first element of the resorted linked list */
            pMDDestination=pMDSorted;
            if(pMDDestination!=NULL)    /* Walk through the resorted linked list to the
                                           last Menuitem, if there's at least one element */
                for( ; pMDDestination->Next!=NULL; /* pMDDestination=pMDDestination->Next */)
                    pMDDestination=pMDDestination->Next;
                                        /* If the current menuitem is followed by a break
                                           separator, also resort the break separator because
                                           is is just a style of the current menuitem */
                                        /* A break separator can't be resorted, because
                                           it is only a style flag of the previous menu entry
                                           and is therefor moved only together with the
                                           menu entry */
            if(!strcmp(pMDSource->PgmTitle, CTRL_BREAKSEPARATOR))
                {
                return((MRESULT)FALSE);
                }
            {
            UCHAR   ucBuffer[MAXNAMEL+4];
            SHORT   usItems;
                                        /* Query the text of the selected Menuitem from the
                                           source listbox */
            WinSendDlgItemMsg(hwndDlg, RDLB_SOURCEMENU, LM_QUERYITEMTEXT,
                MPFROM2SHORT(sCountSave, sizeof(ucBuffer)), MPFROMP(ucBuffer));
                                        /* Delete the selected Menuitem from the source listbox */
            WinSendDlgItemMsg(hwndDlg, RDLB_SOURCEMENU, LM_DELETEITEM,
                MPFROMSHORT(sCountSave), (MPARAM)NULL);
                                        /* Insert the text of the selected Menuitem into the
                                           destination listbox */
            WinSendDlgItemMsg(hwndDlg, RDLB_DESTINATIONMENU, LM_INSERTITEM,
                MPFROMSHORT(LIT_END), MPFROMP(ucBuffer));
                                        /* Query the number of Menuitems from the source
                                           listbox */
            usItems=(ULONG)WinSendDlgItemMsg(hwndDlg, RDLB_SOURCEMENU, LM_QUERYITEMCOUNT,
                (MPARAM)NULL, (MPARAM)NULL);
/*                                                                                      *\
 * For the first resort operation, disable the Cancel pushbutton, after all items have  *
 * been resorted enable OK pushbutton.                                                  *
\*                                                                                      */
            if(bFirstMove==FALSE)       /* After the first move operation, disable the
                                           Cancel pushbutton */
                {
                bFirstMove=TRUE;        /* We make now the first move */
                DisableDialogItem(hwndDlg, usChild2MovePBs,
                    sizeof(usChild2MovePBs)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                }
            if(usItems==0)              /* If now more items left, reenable OK pushbutton
                                           and disable Resort pushbutton */
                {
                DisableDialogItem(hwndDlg, usChild2InitPBs,
                    sizeof(usChild2InitPBs)/sizeof(USHORT), WS_VISIBLE);
                DisableDialogItem(hwndDlg, usChild2OKPBs,
                    sizeof(usChild2OKPBs)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                }
            }
/*                                                                                      *\
 * Now remove the selected Menuitem from the current linked list, by updating the       *
 * pointers according to previous or following Menuitems.                               *
\*                                                                                      */
                                        /* Get menu entry that follows pMDSource */
            if(((pMDSourceNext=pMDSource->Next)!=NULL) &&
                (!strcmp(pMDSource->Next->PgmTitle, CTRL_BREAKSEPARATOR)))
                {                       /* Same as above */
                UCHAR   ucBuffer[MAXNAMEL+4];
                SHORT   usItems;

                                        /* Get menu entry that follows (pMDSource->Break Separator) */
                pMDSourceNext=pMDSource->Next->Next;
                WinSendDlgItemMsg(hwndDlg, RDLB_SOURCEMENU, LM_QUERYITEMTEXT,
                    MPFROM2SHORT(sCountSave, sizeof(ucBuffer)), MPFROMP(ucBuffer));
                WinSendDlgItemMsg(hwndDlg, RDLB_SOURCEMENU, LM_DELETEITEM,
                    MPFROMSHORT(sCountSave), (MPARAM)NULL);
                WinSendDlgItemMsg(hwndDlg, RDLB_DESTINATIONMENU, LM_INSERTITEM,
                    MPFROMSHORT(LIT_END), MPFROMP(ucBuffer));
                usItems=(ULONG)WinSendDlgItemMsg(hwndDlg, RDLB_SOURCEMENU, LM_QUERYITEMCOUNT,
                    (MPARAM)NULL, (MPARAM)NULL);
                if(usItems==0)
                    {
                    DisableDialogItem(hwndDlg, usChild2InitPBs,
                        sizeof(usChild2InitPBs)/sizeof(USHORT), WS_VISIBLE);
                    DisableDialogItem(hwndDlg, usChild2OKPBs,
                        sizeof(usChild2OKPBs)/sizeof(USHORT), WS_VISIBLE | WS_DISABLED);
                    }
                }
            else
                                        /* Point to menu entry following current selection */
                pMDSourceNext=pMDSource->Next;
/*                                                                                      *\
 * Now update the Popup-Menu, by moving the selected Menuitem. Break separators are     *
 * moved automatically by moving 2 menu entries, because they are only style flags of   *
 * a functional menu entry.                                                             *
\*                                                                                      */
            if(pMDDestination!=NULL)
                SetPopupMenu(pHP->hwndPopupMenu, MM_MOVEMENUITEM, MPFROMP(pMDSource), MPFROMP(pMDDestination));
            else
                SetPopupMenu(pHP->hwndPopupMenu, MM_MOVEMENUITEM, MPFROMP(pMDSource), MPFROMP(pMDParent));
            if((pMDSource->Back!=NULL) && (pMDSourceNext!=NULL))
                {                       /* Current element follows another and is followed
                                           by another element, so simple remove it by
                                           updating the pointers. Be carefull if the
                                           parent element is a Submenu where the current
                                           level is a leaf of. */
                if((pMDSource->Back)->Submenu==pMDSource)
                    {
                    (pMDSource->Back)->Submenu=pMDSourceNext;
                    pHP->pMenuData=pMDSourceNext;
                    }
                else
                    (pMDSource->Back)->Next=pMDSourceNext;
                (pMDSourceNext)->Back=pMDSource->Back;
                }
            if((pMDSource->Back!=NULL) && (pMDSourceNext==NULL))
                {                       /* Current element follows another but is the last
                                           one of the current linked list.  Be carefull if
                                           the parent element is a Submenu where the current
                                           level is a leaf of. */
                                        /* Next element of course is NULL */
                if((pMDSource->Back)->Submenu==pMDSource)
                    {
                    (pMDSource->Back)->Submenu=pMDSourceNext;
                    pHP->pMenuData=pMDSourceNext;
                    }
                else
                    (pMDSource->Back)->Next=pMDSourceNext;
                }
            if((pMDSource->Back==NULL) && (pMDSourceNext!=NULL))
                {                       /* Current element is the first one of the complete
                                           linked list, but is followed by another */
                (pMDSourceNext)->Back=NULL;
                pHP->pMenuData=pMDSourceNext;
                pHP->pPopupMenu=pMDSourceNext;
                }
            if((pMDSource->Back==NULL) && (pMDSourceNext==NULL))
                {                       /* Current element is the first one of the complete
                                           linked list, and isn't followed by another */
                pHP->pPopupMenu=pMDSorted;
                pHP->pMenuData=pHP->pPopupMenu;
                }
/*                                                                                      *\
 * Now add the deleted Menuitem at the end of the resorted linked list, by updating the *
 * pointer of the last element to point to the deleted one.                             *
\*                                                                                      */
            if(pMDDestination==NULL)
                {                       /* If the resorted linked list is empty, add
                                           deleted element and update pointers */
                pMDSorted=pMDSource;
                pMDDestination=pMDSource;
                pMDDestination->Next=NULL;
                pMDDestination->Back=NULL;
                }
            else
                {                       /* If the resorted linked list isn't empty, add
                                           deleted element at the end and update pointers */
                pMDDestination->Next=pMDSource;
                pMDSource->Back=pMDDestination;
                if((pMDSource->Next!=NULL) && (!strcmp(pMDSource->Next->PgmTitle, CTRL_BREAKSEPARATOR)))
                    pMDSource->Next->Next=NULL;
                else
                    pMDSource->Next=NULL;
                }
                                        /* Query the next selected Menuitem from the source listbox */
            sCountSave=sCount=(LONG)WinSendDlgItemMsg(hwndDlg, RDLB_SOURCEMENU, LM_QUERYSELECTION,
                MPFROMSHORT(LIT_FIRST), (MPARAM)NULL);
            }
        }
        return((MRESULT)FALSE);         /* We don't want to close the dialog by breaking
                                           out of the switch statement */

    case DID_OK:                        /* Enter key pressed */
/*                                                                                      *\
 * Now replace the current linked list, which is empty as all Menuitems have been re-   *
 * moved, with the resorted linked list.                                                *
\*                                                                                      */
        if(pMDParent==NULL)
            {                           /* If the root of the complete linked list has been
                                           resorted, update the root pointers */
            pHP->pPopupMenu=pMDSorted;
            pHP->pMenuData=pHP->pPopupMenu;
            }
        else
            {                           /* If the current level has a parent Menuitem, a
                                           Submenu of course, update the current level pointers */
            pHP->pMenuData=pMDSorted;
            pMDSorted->Back=pMDParent;
            pMDParent->Submenu=pHP->pMenuData;
            }
                                        /* Dialog terminated with DID_OK */
        WinDismissDlg(hwndDlg, DID_OK);
        break;

    case DID_CANCEL:                    /* Escape or Cancel pressed */
        if(bFirstMove==TRUE)            /* If first resort operation was made, don't dismiss
                                           dialog */
            return((MRESULT)TRUE);
        else
                                        /* Dialog terminated with DID_CANCEL */
            WinDismissDlg(hwndDlg, DID_CANCEL);
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Lockup dialog and the PC/2 - Password       *
 * dialog.                                                                              *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY LD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Set to TRUE when being used as Lockup dialog, or FALSE
                                           if used as Password dialog */
ULONG           ulLockupDialog;
static HWND     hwndMlePostit;
static HWND     hwndEfUser;
static HWND     hwndMleMessage;
static LONG     lOffset;
static USHORT   usChild2Postit[]={LDST_USER, LDEF_USER, LDMLE_MESSAGE, LDPB_POSTIT};

switch(msg)
{
case WM_INITDLG:
                                        /* As dialog may display a message box being this dialog the
                                           owner, associate the main helptable with this dialog */
    WinAssociateHelpInstance(pHP->hwndHelp, hwndDlg);
    WinDefDlgProc(hwndDlg, msg, mp1, mp2);
                                        /* Access parameter passed and save it into window word */
    ulLockupDialog=*((ULONG *)mp2);
    WinSetWindowULong(hwndDlg, QWL_USER, ulLockupDialog);
    hwndMlePostit=hwndEfUser=hwndMleMessage=NULLHANDLE;
                                        /* Center frame window and initialize Post It! area (for
                                           lockup dialog) */
    if(ulLockupDialog==TRUE)
        {
        hwndMlePostit=WinWindowFromID(hwndDlg, LDMLE_POSTIT);
        lOffset=-1;
        hwndEfUser=WinWindowFromID(hwndDlg, LDEF_USER);
        hwndMleMessage=WinWindowFromID(hwndDlg, LDMLE_MESSAGE);
        WinSendMsg(hwndEfUser, EM_SETTEXTLIMIT, MPFROMSHORT(MAXNAMEL), (MPARAM)NULL);
        WinSendMsg(hwndMleMessage, MLM_SETTEXTLIMIT, MPFROMSHORT(EF_SIZE255-5), (MPARAM)NULL);
        WinSendMsg(hwndMlePostit, MLM_SETTEXTLIMIT, MPFROMSHORT(5*EF_SIZE255), (MPARAM)NULL);
        CenterWindow(hwndDlg);
        }
    WinSendDlgItemMsg(hwndDlg, LDEF_LOCKUPPASSWORD, EM_SETTEXTLIMIT,
        MPFROMSHORT(PASSWORDSIZE), (MPARAM)NULL);
    break;

case WM_HELP:                           /* Help pressed */
                                        /* If Password dialog, dismiss it */
    ulLockupDialog=WinQueryWindowULong(hwndDlg, QWL_USER);
    if(pHP->hwndHelp!=NULLHANDLE)
        WinSendMsg(pHP->hwndHelp, HM_DISPLAY_HELP,
            MPFROMSHORT((ulLockupDialog==TRUE ? LDID_LOCKUPDIALOG : LDID_PASSWORDDIALOG)), HM_RESOURCEID);
    break;

case WM_COMMAND:                        /* Button pressed */
    switch(SHORT1FROMMP(mp1))
    {
    case LDPB_POSTIT:                   /* Post It! pressed */
        {
        UCHAR   ucBuffer[EF_SIZE255];
        ULONG   ulBytes=0;

                                        /* Add "From " */
        strcpy(ucBuffer, "From ");
        ulBytes=strlen(ucBuffer);
        WinSendMsg(hwndMlePostit, MLM_SETIMPORTEXPORT, MPFROMP(ucBuffer), MPFROMSHORT((USHORT)ulBytes));
        WinSendMsg(hwndMlePostit, MLM_IMPORT, MPFROMLONG(&lOffset), MPFROMP(&ulBytes));
                                        /* Add "Username" */
        memset(ucBuffer, '\0', sizeof(ucBuffer));
        WinQueryWindowText(hwndEfUser, sizeof(ucBuffer), ucBuffer);
        ulBytes=strlen(ucBuffer);
        WinSendMsg(hwndMlePostit, MLM_SETIMPORTEXPORT, MPFROMP(ucBuffer), MPFROMSHORT((USHORT)ulBytes));
        WinSendMsg(hwndMlePostit, MLM_IMPORT, MPFROMLONG(&lOffset), MPFROMP(&ulBytes));
                                        /* Add ": " */
        strcpy(ucBuffer, ": ");
        ulBytes=strlen(ucBuffer);
        WinSendMsg(hwndMlePostit, MLM_SETIMPORTEXPORT, MPFROMP(ucBuffer), MPFROMSHORT((USHORT)ulBytes));
        WinSendMsg(hwndMlePostit, MLM_IMPORT, MPFROMLONG(&lOffset), MPFROMP(&ulBytes));
                                        /* Add "Messagetext" */
        memset(ucBuffer, '\0', sizeof(ucBuffer));
        WinQueryWindowText(hwndMleMessage, sizeof(ucBuffer), ucBuffer);
        StripTrailing(ucBuffer);
        strcat(ucBuffer, "\r\n\r");
        ulBytes=strlen(ucBuffer);
        WinSendMsg(hwndMlePostit, MLM_SETIMPORTEXPORT, MPFROMP(ucBuffer), MPFROMSHORT((USHORT)ulBytes));
        WinSendMsg(hwndMlePostit, MLM_IMPORT, MPFROMLONG(&lOffset), MPFROMP(&ulBytes));
                                        /* Clear Post It! note */
        WinSetWindowText(hwndEfUser, "");
        WinSetWindowText(hwndMleMessage, "");
        }
        break;

    case DID_OK:                        /* Enter key pressed */
        {
                                        /* Password the user entered, which will be
                                           compared to the password the user configured
                                           in the Desktop dialog. They must match in order
                                           to dismiss this dialog */
        UCHAR   ucLDPassword[PASSWORDSIZE+1];
        UCHAR   ucPassword[PASSWORDSIZE+1];
        LONG    lTemp;

        ulLockupDialog=WinQueryWindowULong(hwndDlg, QWL_USER);
                                        /* Get the password the user entered */
        WinQueryWindowText(WinWindowFromID(hwndDlg, LDEF_LOCKUPPASSWORD),
            sizeof(ucLDPassword), ucLDPassword);
                                        /* Encrypt entered password */
        for(lTemp=strlen(ucLDPassword)-1; lTemp>=0; lTemp--)
            ucLDPassword[lTemp]=(ucLDPassword[lTemp]<<1)^0x3C;
                                        /* Compare entered password with defined one */
        strcpy(ucPassword, (ulLockupDialog==TRUE ? pHP->ucLockupPassword : pHP->ucAccessPassword));
        if(strcmp(ucLDPassword, ucPassword))
            {                           /* If the entered password was invalid
                                           inform user and delete data entered */
            UCHAR   ucError[PASSWORDSIZE+64];

            WinQueryWindowText(WinWindowFromID(hwndDlg, LDEF_LOCKUPPASSWORD),
                sizeof(ucLDPassword), ucLDPassword);
            strcpy(ucError, "Invalid Password \"");
            strcat(ucError, ucLDPassword);
            strcat(ucError, "\" - Please reenter!");
            WinSetDlgItemText(hwndDlg, LDST_INVALIDPASSWORD, ucError);
            WinShowWindow(WinWindowFromID(hwndDlg, LDST_INVALIDPASSWORD), TRUE);
            WinSetDlgItemText(hwndDlg, LDEF_LOCKUPPASSWORD, "");
                                        /* Set focut back to the password entryfield */
            WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwndDlg, LDEF_LOCKUPPASSWORD));
            }
        else
            {                           /* Dialog terminated with DID_OK */
            if(ulLockupDialog==TRUE)
                {
                if((WinIsWindowVisible(hwndMlePostit)) || ((ULONG)WinQueryWindowTextLength(hwndMlePostit)==0))
                    WinDismissDlg(hwndDlg, DID_OK);
                DisableDialogItem(hwndDlg, usChild2Postit, sizeof(usChild2Postit)/sizeof(USHORT), WS_DISABLED);
                WinShowWindow(hwndMlePostit, TRUE);
                }
            else
                {
                WinDismissDlg(hwndDlg, DID_OK);
                }
            }
        }
        break;

    case DID_CANCEL:                    /* Escape or Cancel pressed, we ignore it */
                                        /* If Password dialog, dismiss it */
        ulLockupDialog=WinQueryWindowULong(hwndDlg, QWL_USER);
        if(ulLockupDialog==FALSE)
            WinDismissDlg(hwndDlg, DID_CANCEL);
        break;

    default:
        return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    break;

default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

