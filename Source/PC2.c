/***********************************************************************\
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1993, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * PC/2 - Program Commander/2 is a configurable program starter for    *
 * OS/2 2.x PM. If the user clicks button 1 on the DESKTOP, a user     *
 * modifyable popup menu is displayed. The user then selects a program *
 * to be started, or configuration of PC/2 or dismisses it.            *
 * You can define Hotkeys to switch to a program, or start it if it is *
 * not already running.                                                *
 * PC/2 is an alternative method of starting programs compared to      *
 * icons and uses no space on DESKTOP, and no folder must be opended   *
 * to start a program. For frequently used programs, this reduces the  *
 * time to start an application.                                       *
 * PC/2 also implements an optional virtual Desktop and sliding focus. *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#include        "PC2.h"                 /* User include files */
#include        "Error.h"

#define         _FILE_  "PC2.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;

HMTX            hmtxHang;               /* Mutex Semaphore to simulate hanging Working Thread */
HMTX            hmtxExceptionHandler;   /* Mutex Sempahore used to count nestings levels of
                                           DosKillThread() to allow exception handler to
                                           if PC/2 did a DosKillThread() or if it came from
                                           and external application like e.g. WatchCat */
HEV             hevPc2;                 /* Handle of PC/2 semaphore */
UCHAR           *pucFilenameProfile;    /* The buffer holding the filename of the profile */
                                        /* The buffer holding the filename of the environment
                                           space profile */
UCHAR           *pucFilenameEnvironment;
UCHAR           *pucFilenameINI;        /* Path and filename of PC2.INI */
UCHAR           *pucFilenameTRP;        /* Path and filename of PC2.TRP */
UCHAR           *pucFilenameHLP;        /* The buffer holding the filename of the HLP file */
UCHAR           *pucPathDLL;            /* Path to directory PC/2 was started from and DLLs reside */
HMODULE         hDLLPc2Hook=NULLHANDLE; /* PC2HOOK.DLL module handle */
                                        /* PC2SPOOL.DLL module handle */
HMODULE         hDLLPc2Spooler=NULLHANDLE;
                                        /* PC2BRES.DLL (BGA) or PC2VRES.DLL (VGA) module handle */
HMODULE         hDLLPc2Resource=NULLHANDLE;
PFNWP           pfnMenuWindowProc;      /* PC/2 smarticons menu window procedure saved for subclassing */
PFNWP           pfnListboxWindowProc;   /* PC/2's setup dialog CDLB_MENUPROGRAM listbox window procedure
                                           saved for subclassing */
                                        /* In memory window DC */
HDC             hdcOverviewMemory=NULLHANDLE;
HDC             hdcSessionBarMemory=NULLHANDLE;
                                        /* Working thread control structure */
THREADDATA      *pthreaddataWorkingThread=NULL;
                                        /* I/O task thread control structure */
THREADDATA      *pthreaddataIOThread=NULL;
                                        /* Network thread control structure */
THREADDATA      *pthreaddataNetThread=NULL;

/*                                                                                      *\
 * Function pointer of functions loaded dynamically.                                    *
\*                                                                                      */
HOOKPARAMETERS  *pHP=NULL;              /* Pointer to PC/2 control structure loaded from PC2HOOK.DLL */
PFFUNCPTR1      *pInitialize=NULL;      /* Initialize PC2HOOK.DLL */
PFFUNCPTR1      *pCleanup=NULL;         /* Deinitialize PC2HOOK.DLL */
PFFUNCPTR2      *pInputHook=NULL;       /* Input hook procedure */
PFFUNCPTR3      *pWinSendMsgHook=NULL;  /* WinSendMsg() hook procedure */
PFFUNCPTR2      *pPreAccelHook=NULL;    /* Pre-Accelerator hook procedure */
                                        /* Initialize PC2SPOOL.DLL */
PFFUNCPTR5      *pSpoolerInitialize=NULL;
DOSVERIFYPIDTID *pDosVerifyPidTid=NULL; /* DosVerifyPidTid() procedure */

/*--------------------------------------------------------------------------------------*\
 * The main procedure.                                                                  *
 * Req:                                                                                 *
 * Returns:                                                                             *
 *      int ........... Exitcode (0, or errorlevel)                                     *
\*--------------------------------------------------------------------------------------*/
int main(void)
{
HMODULE hDLLDOSCALL1;                   /* Handle of DOSCALL1.DLL to get APIs we load dynamically */
HAB     habPc2;                         /* Anchor block handle */
HMQ     hmqPc2;                         /* Message queue handle */
HWND    hwndFrame;                      /* Frame window handle, used until HOOKPARAMETERS got
                                           loaded from PC2HOOK.DLL */
HWND    hwndClient;                     /* Client window handle, same as hwndFrame */
HWND    hwndHelp;                       /* Help window handle */
ULONG   ulOverviewFCF;                  /* Frame creation flags */
QMSG    qmsg;                           /* Message queue */
UCHAR   ucModuleName[CCHMAXPATH];       /* PC/2's full qualified path name, PC/2 was started
                                           from, obtained from PC/2's PIB */
UCHAR   *pucMessage;                    /* Error message buffer */
ULONG   ulBootDrive;                    /* Boot drive */
ULONG   *pulVersion;                    /* DLL signature */
TIB     *ptib;                          /* PC/2's thread information block */
PIB     *ppib;                          /* PC/2's process information block */
APIRET  Rc;                             /* Dos_* calls return code */
                                        /* Exception handler registration */
EXCEPTIONREGISTRATIONRECORD     excpregrecThread;

                                        /* Disable System Error popups by accessing invalid drives,
                                           because to avoid potential (though I can't say definitely
                                           this can happen) SIQ lockups when accessing invalid drives
                                           in a PM thread. For some unknown reasons, this gets undone
                                           during IniAccess() which is a wrapper for PC/2's PC2.INI
                                           file handling (I really have no idea why the Prf-APIs do
                                           that) */
DosError(FERR_DISABLEHARDERR);
/*                                                                                      *\
 * Bring up an anchor block as soon as possible, to be able to display message boxes.   *
\*                                                                                      */
do
{
/*                                                                                      *\
 * The only portable way to access the path PC/2 was invoked from, to access the        *
 * command-line parameters and the environment is to query the process information      *
 * block. Portable means that this is expected to work for OS/2 for PowerPC too (ok,    *
 * OS/2 for PowerPC is dead now :-(                                                     *
\*                                                                                      */
    DosQuerySysInfo(QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive, sizeof(ulBootDrive));
    ulBootDrive=ulBootDrive-1+'A';
    DosGetInfoBlocks(&ptib, &ppib);
    DosQueryModuleName(ppib->pib_hmte, sizeof(ucModuleName), ucModuleName);
/*                                                                                      *\
 * Startup PM.                                                                          *
\*                                                                                      */
                                        /* Initialize anchor block and message queue */
    if(WinStartUp(&habPc2, &hmqPc2)==FALSE)
        break;
    if(!WinRegisterClass(               /* Register window class */
        habPc2,                         /* Handle of anchor block */
        (PSZ)PC2_CLASSNAME,             /* Window class name */
        (PFNWP)PC2_MainWindowProc,      /* Address of window procedure */
                                        /* Class style */
        CS_SIZEREDRAW | CS_SAVEBITS | CS_MOVENOTIFY,
        0))                             /* Extra window words */
        {
        PM_ERR(habPc2, HWND_DESKTOP, HELP_CREATEWINDOW, MB_ERROR|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
            "Initializing the PM environment failed, PC/2 can't continue. You may have run out "\
            "of resources, close some windows or applications and retry. Exiting...");
        break;
        }
/*                                                                                      *\
 * If the ALT key is pressed while we launch, terminate immediately (so one can prevent *
 * PC/2 to launch from e.g. Startup.cmd or the Startup folder). This may be useful if   *
 * you want to replace parts of PC/2 that may be locked once PC/2 has started once      *
 * e.g. PC2HOOK.DLL usually will be locked due to hooking the system queues).           *
\*                                                                                      */
    if(WinGetKeyState(HWND_DESKTOP, VK_ALT) & 0x8000)
        {
        USR_ERR(HWND_DESKTOP, HELP_PC2LOADED, MB_ERROR|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
            "You have selected (by pressing and holding the ALT-Key while PC/2 starts) "
            "not to launch PC/2.\n\n"
            "PC/2 will thus terminate - exiting...");
        break;
        }
/*                                                                                      *\
 * Start frame window, which creates window(s) and WM_CREATE message. Then associate    *
 * a help instance with this window, to be able to display online help for message      *
 * boxes.                                                                               *
\*                                                                                      */
    ulOverviewFCF=FCF_OVERVIEWWINDOW;
    hwndFrame=WinCreateStdWindow(
        HWND_DESKTOP,                   /* DESKTOP is parent */
        0,                              /* Standard window styles */
        &ulOverviewFCF,                 /* Frame control flags */
        (PSZ)PC2_CLASSNAME,             /* Client window class name */
        "",                             /* No window text */
        0,                              /* No special class style */
        (HMODULE)0,                     /* Ressource is in .EXE file */
        ID_PC2MAINWINDOW,               /* Frame window identifier */
        &hwndClient);                   /* Client window handle */
    if(hwndFrame==NULLHANDLE)
        {
        PM_ERR(habPc2, HWND_DESKTOP, HELP_CREATEWINDOW, MB_ERROR|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
            "Creation of PC/2's Overview Window, PC/2 can't continue. You may have run out "\
            "of resources, close some windows or applications and retry. Exiting...");
        break;
        }
/*                                                                                      *\
 * Get the full path and filename of the running copy of PC/2 and change the extension  *
 * .EXE into .cfg to open the configuration file under this name. If the user supplies  *
 * [-,/Profile filename.ext] then use this filename as the Profile. Also change .EXE    *
 * into .HLP, .INI and PC/2 directory as the current directory to access .DLL. All path *
 * names contain the drive letter + directory PC/2 was started from.                    *
\*                                                                                      */
                                        /* Long enough to hold user Profile name */
    pucFilenameProfile=calloc(CCHMAXPATH+1, sizeof(BYTE));
    pucFilenameEnvironment=calloc(CCHMAXPATH+1, sizeof(BYTE));
    pucFilenameINI=calloc(CCHMAXPATH+1, sizeof(BYTE));
    pucFilenameTRP=calloc(CCHMAXPATH+1, sizeof(BYTE));
    pucFilenameHLP=calloc(CCHMAXPATH+1, sizeof(BYTE));
    pucPathDLL=calloc(CCHMAXPATH+1, sizeof(BYTE));
                                        /* If no drive letter is available, e.g. using
                                           SET RUNWORKPLACE=\PMAPPS\PC2.EXE, add current
                                           drive, which is the boot drive */
    if(ucModuleName[0]=='\\')
        {
        ULONG   ulDriveNumber;          /* Current logical drive */
        ULONG   ulLogicalDriveMap;      /* Logical drive bitmapped flag */
        UCHAR   ucCurrentDrive[3]="C:"; /* Current drive */

                                        /* Get current logical drive 1...A, 2...B, 3...C, ... */
        if(!DosQueryCurrentDisk(&ulDriveNumber, &ulLogicalDriveMap))
                                        /* On no error use current drive, otherwise assume C: */
            ucCurrentDrive[0]=(CHAR)(--ulDriveNumber)+'A';
                                        /* Add drive letter */
        strcpy(pucFilenameProfile, ucCurrentDrive);
        strcpy(pucFilenameEnvironment, ucCurrentDrive);
        strcpy(pucFilenameINI, ucCurrentDrive);
        strcpy(pucFilenameTRP, ucCurrentDrive);
        strcpy(pucFilenameHLP, ucCurrentDrive);
        strcpy(pucPathDLL, ucCurrentDrive);
        }
                                        /* Add the full qualified path PC/2 was started from,
                                           which we queried from the process information block */
    strcat(pucFilenameProfile, ucModuleName);
    strcat(pucFilenameEnvironment, ucModuleName);
    strcat(pucFilenameINI, ucModuleName);
    strcat(pucFilenameTRP, ucModuleName);
    strcat(pucFilenameHLP, ucModuleName);
    strcat(pucPathDLL, ucModuleName);
    strcpy(strchr(pucFilenameProfile, '.'), ".cfg");
    strcpy(strchr(pucFilenameEnvironment, '.'), ".env");
    strcpy(strchr(pucFilenameINI, '.'), ".ini");
    strcpy(strchr(pucFilenameTRP, '.'), ".trp");
    strcpy(strchr(pucFilenameHLP, '.'), ".hlp");
    strcpy(strrchr(pucPathDLL, '\\'), "");
/*                                                                                      *\
 * Register exception handler for the main thread                                       *
\*                                                                                      */
    RegisterExceptionHandler(TRUE, &excpregrecThread, pucFilenameTRP);
/*                                                                                      *\
 * Depending on the screen resolution, we have to load either our resouces from the DLL *
 * containing BGA resolution support or VGA resolution support. As the PC/2 Overview's  *
 * window should contain smarticons, we modify the menubar (containing MIS_TEXT) items  *
 * to be smarticons (MIS_BITMAP items), by changing the style and loading the smarticon *
 * bitmaps. (This is required due to a limitation of PM, which can only load bitmaps    *
 * from the executable's module for menus loaded from a resource, but as we don't want  *
 * have the resources irelevant for the current resolution in storage (which would      *
 * require our executable to hold both resolutions as the current resolution is unknown *
 * at compile time) we have a resolution dependent resource DLL where we load from).    *
\*                                                                                      */
    {
    UCHAR       ucBuffer[CCHMAXPATH];   /* Buffer for possible return codes from DLL loading */
    UCHAR       ucDrive;
    UCHAR       *pucLibrary;

                                        /* Get drive of PC/2's startup directory
                                           1=A, 2=B, 3=C,.... and direcotry itself */
    ucDrive=tolower(pucPathDLL[0]);
    DosSetDefaultDisk(++ucDrive-'a');
    DosSetCurrentDir(pucPathDLL+2);
                                        /* Use the sizes the display driver uses */
    if((WinQuerySysValue(HWND_DESKTOP, SV_CXICON)!=ICONSIZE_VGA))
        pucLibrary="PC2BRes";
    else
        pucLibrary="PC2VRes";
    Rc=DosLoadModule(                   /* Load the DLL of PC/2's resources */
        ucBuffer,                       /* Save failure there */
        sizeof(ucBuffer)-1,             /* Length of save area */
        pucLibrary,                     /* Library name */
        &hDLLPc2Resource);              /* DLL module handle */
    if(Rc!=NO_ERROR)
        {                               /* DLL couldn't be found in the current PC/2
                                           directory or via the LIBPATH path */
        DOS_ERR(Rc, hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't find PC2VRES.DLL or PC2BRES.DLL, please check that the DLLs reside in "
            "the directory PC/2 was started from - exiting");
        break;
        }
/*                                                                                      *\
 * Now initilize Help, if it can't be initialized the we get no help but that's no      *
 * reason to terminate.                                                                 *
\*                                                                                      */
    if(WinStartHelp(habPc2, hDLLPc2Resource, pucFilenameHLP, &hwndHelp, hwndFrame)==FALSE)
        PM_ERR(habPc2, hwndFrame, HELP_PC2HELP, MB_INFORMATION|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
            "Can't find or load from PC2.HLP, please check that PC2.HLP is in the directory you "\
            "started PC/2 from. All help help requests will be ignored - continuing...");
                                        /* Check DLL signature */
    Rc=DosQueryProcAddr(hDLLPc2Resource, DLLENTRYPOINTSIGNATURE, "_PC2RESSIGNATURE", (PFN *)(&pulVersion));
    if(Rc==NO_ERROR)
        {
#ifdef  DEBUG
        printf("PC2: Version PC2?Res.DLL %lX, expected %lX\n", *pulVersion, (ULONG)BLDLEVEL_SIGNATURE);
#endif  /* DEBUG */
        if(*pulVersion!=BLDLEVEL_SIGNATURE)
            Rc=ERROR_BAD_FORMAT;
        }
    if(Rc!=NO_ERROR)
        {
        pucMessage=malloc(1024);
        sprintf(pucMessage, "The signature of the PC/2 DLL \"%s\" does not match with this build. "
            "Please see the online help to verify PC/2 is installed correctly!\n\n", "PC2?RES.DLL");
        strcat(pucMessage, "PC/2 will thus terminate - exiting...");
        USR_ERR(hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1, pucMessage);
        free(pucMessage);
        break;
        }
    }
/*                                                                                      *\
 * Check if we are allready loaded before by querying a semaphore that is defined the   *
 * first time PC/2 runs.                                                                *
\*                                                                                      */
    if(DosCreateEventSem(               /* Create a semaphore */
        PC2_SEM,                        /* Name */
        &hevPc2,                        /* Handle */
        (ULONG)0,                       /* Named semaphores are allways shared */
        (BOOL32)FALSE))                 /* Initially set */
        {                               /* If an error occurs, either we can't create
                                           the semaphore or it allready exists. We assume
                                           that it exists, meaning PC/2 allready loaded.
                                           Parameters NULL force an immediate exit(1). */
        USR_ERR(hwndFrame, HELP_PC2LOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "One instance of PC/2 is already loaded. If you have just exited PC/2 before restarting "
            "it, just wait a few more seconds for the previous instance of PC/2 finishing its "
            "cleanup - exiting...");
        break;
        }
/*                                                                                      *\
 * Create Mutex Semaphores to protect our exception handler and to allow to trigger the *
 * DosKillThread()/ExceptionHandler processing by a window named "MagicMagicMagic"      *
\*                                                                                      */
    DosCreateMutexSem(PC2_EXCPHANDLER_SEM, &hmtxExceptionHandler, DC_SEM_SHARED, FALSE);
    DosCreateMutexSem(NULL, &hmtxHang, 0, TRUE);
/*                                                                                      *\
 * Load the Pc2Hook DLL either from the current directory or a LIBPATH path and         *
 * obtain the addresses of the entrypoints. There seems to be a little bug?, when the   *
 * library name contains a .DLL extension - the DLL is loaded sucessfully but not       *
 * always correct initialized? and calling functions in the DLL tend to fail. Extension *
 * .DLL therefore not appended to library name. Entry points are loaded by ordinal.     *
\*                                                                                      */
    {
    UCHAR       ucBuffer[CCHMAXPATH];   /* Buffer for possible return codes from DLL loading */
    UCHAR       *pucRunWorkPlace;       /* Scan environment for active WPS process */
    ULONG       ulIndex;                /* KEYDATA table index */

    Rc=DosLoadModule(                   /* Load the DLL of PC/2 */
        ucBuffer,                       /* Save failure there */
        sizeof(ucBuffer)-1,             /* Length of save area */
        "PC2Hook",                      /* Library name */
        &hDLLPc2Hook);                  /* DLL module handle */
    if(Rc!=NO_ERROR)
        {                               /* DLL couldn't be found in the current PC/2
                                           directory or via the LIBPATH path */
        DOS_ERR(Rc, hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't find PC2HOOK.DLL, please check that the DLL resides in the directory "\
            "PC/2 was started from - exiting");
        break;
        }
                                        /* Check DLL signature */
    Rc=DosQueryProcAddr(hDLLPc2Resource, DLLENTRYPOINTSIGNATURE, "_PC2HOOKSIGNATURE", (PFN *)(&pulVersion));
    if(Rc==NO_ERROR)
        {
#ifdef  DEBUG
        printf("PC2: Version PC2Hook.DLL %lX, expected %lX\n", *pulVersion, (ULONG)BLDLEVEL_SIGNATURE);
#endif  /* DEBUG */
        if(*pulVersion!=BLDLEVEL_SIGNATURE)
            Rc=ERROR_BAD_FORMAT;
        }
    if(Rc!=NO_ERROR)
        {
        UCHAR   *pucMessage;

        pucMessage=malloc(1024);
        sprintf(pucMessage, "The signature of the PC/2 DLL \"%s\" does not match with this build. "
            "Please see the online help to verify PC/2 is installed correctly!\n\n", "PC2HOOK.DLL");
        strcat(pucMessage, "PC/2 will thus terminate - exiting...");
        USR_ERR(hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1, pucMessage);
        free(pucMessage);
        break;
        }
    Rc=DosQueryProcAddr(hDLLPc2Hook, DLLENTRYPOINTINIT, NULL, (PFN *)(&pInitialize));
    if(Rc!=NO_ERROR)
        {                               /* An error occured */
        DosFreeModule(hDLLPc2Hook);     /* Free DLL reference */
        DOS_ERR(Rc, hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't load from PC2HOOK.DLL, maybe you are using an outdated version - exiting");
        pHP=0;
        break;
        }
    Rc=DosQueryProcAddr(hDLLPc2Hook, DLLENTRYPOINTCLEANUP, NULL, (PFN *)(&pCleanup));
    if(Rc!=NO_ERROR)
        {                               /* An error occured */
        DosFreeModule(hDLLPc2Hook);     /* Free DLL reference */
        DOS_ERR(Rc, hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't load from PC2HOOK.DLL, maybe you are using an outdated version - exiting");
        pHP=0;
        break;
        }
    Rc=DosQueryProcAddr(hDLLPc2Hook, DLLENTRYPOINTINPUTHOOK, NULL, (PFN *)(&pInputHook));
    if(Rc!=NO_ERROR)
        {                               /* An error occured */
        DosFreeModule(hDLLPc2Hook);     /* Free DLL reference */
        DOS_ERR(Rc, hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't load from PC2HOOK.DLL, maybe you are using an outdated version - exiting");
        pHP=0;
        break;
        }
    Rc=DosQueryProcAddr(hDLLPc2Hook, DLLENTRYPOINTPREACCELHOOK, NULL, (PFN *)(&pPreAccelHook));
    if(Rc!=NO_ERROR)
        {                               /* An error occured */
        DosFreeModule(hDLLPc2Hook);     /* Free DLL reference */
        DOS_ERR(Rc, hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't load from PC2HOOK.DLL, maybe you are using an outdated version - exiting");
        pHP=0;
        break;
        }
    Rc=DosQueryProcAddr(hDLLPc2Hook, DLLENTRYPOINTWINSENDMSGHOOK, NULL, (PFN *)(&pWinSendMsgHook));
    if(Rc!=NO_ERROR)
        {
        DosFreeModule(hDLLPc2Hook);
        DOS_ERR(Rc, hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't load from PC2HOOK.DLL, maybe you are using an outdated version - exiting");
        pHP=0;
        break;
        }
    Rc=DosQueryProcAddr(hDLLPc2Hook, DLLENTRYPOINTHOOKPARAMETERS, NULL, (PFN *)(&pHP));
    if(Rc!=NO_ERROR)
        {                               /* An error occured */
        DosFreeModule(hDLLPc2Hook);     /* Free DLL reference */
        DOS_ERR(Rc, hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't load from PC2HOOK.DLL, maybe you are using an outdated version - exiting");
        pHP=0;
        break;
        }
                                        /* Initialize */
    memset(pHP, 0, sizeof(HOOKPARAMETERS));
    pHP->hDLLPc2Hook=hDLLPc2Hook;
    Rc=DosQueryProcAddr(hDLLPc2Hook, DLLENTRYPOINTKEYDATA, NULL, (PFN *)(&pHP->pKeyData));
    if(Rc!=NO_ERROR)
        {
        DosFreeModule(hDLLPc2Hook);
        DOS_ERR(Rc, hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't load from PC2HOOK.DLL, maybe you are using an outdated version - exiting");
        pHP=0;
        break;
        }
/*                                                                                      *\
 * Initialize HookParameters structure.                                                 *
\*                                                                                      */
    pHP->pTib=ptib;                     /* Save thread and process information block of PC/2 */
    pHP->pPib=ppib;
    DosAllocSharedMem((PPVOID)&pHP->pucWindowText, NULL, sizeof(MAXNAMEL+4),
        PAG_COMMIT|OBJ_GIVEABLE|PAG_READ|PAG_WRITE);
    pHP->ProcessId=ppib->pib_ulpid;
    pHP->PriorityClass=(USHORT)((UCHAR)(ptib->tib_ptib2->tib2_ulpri & (~0xFF)) >> 8);
    pHP->PriorityDelta=(SHORT)((CHAR)(ptib->tib_ptib2->tib2_ulpri & 0xFF));
                                        /* Get boot drive (1=A, 2=B, ...) */
    pHP->ucBootDrive='C';
    if(DosQuerySysInfo(QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulIndex, sizeof(ulIndex))==NO_ERROR)
        pHP->ucBootDrive=((UCHAR)ulIndex)-1+'A';
    if(DosQuerySysInfo(QSV_VERSION_MAJOR, QSV_VERSION_MINOR, &pHP->ulVersionMajor,
        (sizeof(pHP->ulVersionMajor)+sizeof(pHP->ulVersionMinor))))
        {
        pHP->ulVersionMajor=OS2_MAJOR;
        pHP->ulVersionMinor=OS2_MINOR_200;
        }
    pHP->MenuDataId=ID_POPUPMENU;
    pHP->habPc2=habPc2;
    pHP->hmqPc2=hmqPc2;
    pHP->hwndFrame=hwndFrame;
    pHP->hwndClient=hwndClient;
    pHP->hwndHelp=hwndHelp;
    pHP->hDLLPc2Resource=hDLLPc2Resource;
    pHP->hwndZOrder[ZORDER_TOP]=HWND_TOP;
    pHP->ulOverviewFCF=FCF_OVERVIEWWINDOW;
    pHP->ulSpoolerFCF=FCF_SPOOLERWINDOW;
    strcpy(pHP->ucPathDLL, pucPathDLL);
    free(pucPathDLL);
                                        /* Copy filenames */
    strcpy(pHP->ucFilenameINI, pucFilenameINI);
    strcpy(pHP->ucFilenameProfile, pucFilenameProfile);
    strcpy(pHP->ucFilenameEnvironment, pucFilenameEnvironment);
    strcpy(pHP->ucFilenameTRP, pucFilenameTRP);
    free(pucFilenameINI);
    free(pucFilenameProfile);
    free(pucFilenameEnvironment);
/*                                                                                      *\
 * Now check possible commandline parameters, but read PC2.INI first to overwrite       *
 * values read by commandline parameters. Again, the portable way to access them is via *
 * the process information block.                                                       *
\*                                                                                      */
    {
    UCHAR   *pucCommandline;            /* Pointer to the commandline passed during invokation */
    UCHAR   *pucNextParameter;          /* Pointer to the current commandline parameter */
    ULONG   ulButton1Flag=FALSE;        /* True when -Button1 commandline parameters was found */
    ULONG   ulButton3Flag=FALSE;        /* True when -Button3 commandline parameters was found */

                                        /* Default the Popup-Menu is displayed after a
                                           WM_BUTTON1DBLCLK on the Desktop, but users may
                                           prefer WM_BUTTON1CLICK to popup the menu, which
                                           is set by default during reading PC2.INI, but
                                           we never know if there is a chance to miss it,
                                           so we explicitely test this */
    if(pHP->ulClickFlag==0) pHP->ulClickFlag=WM_BUTTON1DBLCLK;
                                        /* The commandline immediately follows the full qualified
                                           path PC/2 was invoked from */
    pucCommandline=pHP->pPib->pib_pchcmd+strlen(pHP->pPib->pib_pchcmd)+1;
    strupr(pucCommandline);             /* Uppercase commandline parameters, which are separated
                                           by (one or more) spaces */
                                        /* Loop as long commandline parameters are available */
    for(pucNextParameter=pucCommandline;
        (pucNextParameter && *pucNextParameter);
        pucNextParameter=strchr(++pucNextParameter, ' '))
        {
        UCHAR   *pucFilenameAvailable=0;

                                        /* Skip spaces and see if - or / starts a commandline
                                           parameter */
        while(*pucNextParameter==' ')
            pucNextParameter++;
        if((*pucNextParameter!='/') && (*pucNextParameter!='-'))
            continue;
                                        /* Skip - or / of parameter */
        pucNextParameter++;
                                        /* Test for /PROFILE or -PROFILE to get a
                                           profile name */
        if(strncmp(pucNextParameter, "PROFILE", sizeof("PROFILE")-1)==0)
            pucFilenameAvailable=pHP->ucFilenameProfile;
                                        /* Test for /ENVIRONMENT or -ENVIRONMENT to get a
                                           profile name */
        if(strncmp(pucNextParameter, "ENVIRONMENT", sizeof("ENVIRONMENT")-1)==0)
            pucFilenameAvailable=pHP->ucFilenameEnvironment;
                                        /* Test for /INI or -INI to get a
                                           profile name */
        if(strncmp(pucNextParameter, "INI", sizeof("INI")-1)==0)
            pucFilenameAvailable=pHP->ucFilenameINI;
        if(pucFilenameAvailable!=0)
            {
            UCHAR   *pucProfile;
            ULONG   ulCounter=0;
            UCHAR   ucSave='\0';

                                        /* Find next parameter */
            pucNextParameter=strchr(pucNextParameter, ' ');
            while(*pucNextParameter==' ')
                pucNextParameter++;
                                        /* For HPFS. the filename may be enclosed in quotes (") */
            if(*pucNextParameter=='"')
                {
                pucNextParameter++;
                while((*(pucNextParameter+ulCounter)!='"') && (*(pucNextParameter+ulCounter)!='\0'))
                    ulCounter++;
                }
            else
                while((*(pucNextParameter+ulCounter)!=' ') && (*(pucNextParameter+ulCounter)!='\0'))
                    ulCounter++;
                                        /* Modify the filename we assumed by default by replacing
                                           PC2.xxx with the value specified.
                                           However, if the value specified seems to be a fully qualified
                                           path (that is when it contains ":" or "\") then take this
                                           fully qualified path instead */
            ucSave=pucNextParameter[ulCounter];
            pucNextParameter[ulCounter]='\0';
            if((strchr(pucNextParameter, ':')) || (strchr(pucNextParameter, '\\')))
                {
                strcpy(pucFilenameAvailable, pucNextParameter);
                }
            else
                {
                pucProfile=pucFilenameAvailable+strlen(pucFilenameAvailable)-7;
                strcpy(pucProfile, pucNextParameter);
                }
            pucNextParameter[ulCounter]=ucSave;
            continue;
            }
                                        /* Test for /INSTALL or -INSTALL to start the help
                                           panels during initialization */
        if(strncmp(pucNextParameter, "INSTALL", sizeof("INSTALL")-1)==0)
            pHP->ulRuntimeFlag |= INSTALLATIONHELP;
                                        /* Test for /NOAUTOSTART or -NOAUTOSTART to ignore any
                                           autostart requests from menuentries */
        if(strncmp(pucNextParameter, "NOAUTOSTART", sizeof("NOAUTOSTART")-1)==0)
            pHP->ulRuntimeFlag |= NOAUTOSTART;
                                        /* Test for /NOSCHEDULER or -NOSCHEDULER to ignore any
                                           scheduling requests from menuentries */
        if(strncmp(pucNextParameter, "NOSCHEDULER", sizeof("NOSCHEDULER")-1)==0)
            pHP->ulRuntimeFlag |= NOSCHEDULER;
                                        /* Test for /NOCLEANUP or -NOCLEANUP to prevent cleaning up
                                           of the Popup Menu when exiting (which makes the termination
                                           much faster as we don't need to free the linked lists and
                                           the PM SIQ is not blocked due to deleting the (Sub(menus */
        if(strncmp(pucNextParameter, "NOCLEANUP", sizeof("NOCLEANUP")-1)==0)
            pHP->ulRuntimeFlag |= NOCLEANUP;
                                        /* Test for /NOPROMPT or -NOPROMPT to prevent asking the user
                                           or displaying user information */
        if(strncmp(pucNextParameter, "NOPROMPT", sizeof("NOPROMPT")-1)==0)
            pHP->ulRuntimeFlag |= NOPROMPT;
                                        /* Test for /RESERVEDRIVELETTER DRIVES or
                                           -RESERVEDRIVELETTER DRIVES to prevent changing to the root
                                           directory of the specified DRIVES (to workaround buggy drivers
                                           like e.g. the "OAD" parallel port ZIP driver) */
        if(strncmp(pucNextParameter, "RESERVEDRIVELETTER", sizeof("RESERVEDRIVELETTER")-1)==0)
            {
                                        /* Find next parameter */
            pucNextParameter=strchr(pucNextParameter, ' ');
            while(*pucNextParameter==' ')
                pucNextParameter++;
                                        /* Mask all drive letters specified into
                                           Bitmap of drives (Bit 0=A, 1=B, ...) */
            while(isalpha(*pucNextParameter))
                {
                pHP->ulReserveDriveLetter|=(0x00000001<<((*pucNextParameter)-'A'));
                pucNextParameter++;
                }
                                        /* Don't skip the space following after DRIVES
                                           (as it is used to find the next parameter) */
            if(pHP->ulReserveDriveLetter!=0)
                pucNextParameter--;
            }
                                        /* Test for /DOUBLECLICK or -DOUBLECLICK to display
                                           the Popup-Menu after a double-click instead of
                                           a single click */
        if(strncmp(pucNextParameter, "DOUBLECLICK", sizeof("DOUBLECLICK")-1)==0)
            pHP->ulClickFlag=WM_BUTTON1DBLCLK;
                                        /* Test for /SINGLECLICK or -SINGLECLICK to display
                                           the Popup-Menu after a double-click instead of
                                           a single click */
        if(strncmp(pucNextParameter, "SINGLECLICK", sizeof("SINGLECLICK")-1)==0)
            pHP->ulClickFlag=WM_BUTTON1CLICK;
                                        /* Test for /BUTTON1 or -BUTTON1 to display
                                           the Popup-Menu after a double-click instead of
                                           a single click */
        if(strncmp(pucNextParameter, "BUTTON1", sizeof("BUTTON1")-1)==0)
            ulButton1Flag=TRUE;
                                        /* Test for /BUTTON3 or -BUTTON3 to display
                                           the Popup-Menu after a double-click instead of
                                           a single click */
        if(strncmp(pucNextParameter, "BUTTON3", sizeof("BUTTON3")-1)==0)
            ulButton3Flag=TRUE;
        }
/*                                                                                      *\
 * Load from PC2.INI or whatever the user specified.                                    *
\*                                                                                      */
                                        /* Get data from PC2.INI into HookParameters. Commandline
                                           parameters may overload the settings in PC2.INI. */
    if(!(INIAccess(pHP->ucFilenameINI, TRUE)))
        USR_ERR(hwndFrame, HELP_PC2INI, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Can't load configuration data from PC2.INI. "\
            "Either you have started PC/2 for the first time, "\
            "or you upgraded from a previous version of PC/2, or PC2.INI "\
            "is defective. For unreadable data default values are assumed - continuing...");
                                        /* Replace button 1 with 3 when required */
    if(ulButton3Flag==TRUE)
        {
        if((pHP->ulClickFlag==WM_BUTTON1DBLCLK) || (pHP->ulClickFlag==WM_BUTTON3DBLCLK))
            pHP->ulClickFlag=WM_BUTTON3DBLCLK;
        else
            pHP->ulClickFlag=WM_BUTTON3CLICK;
        }
    if(ulButton1Flag==TRUE)
        {
        if((pHP->ulClickFlag==WM_BUTTON1DBLCLK) || (pHP->ulClickFlag==WM_BUTTON3DBLCLK))
            pHP->ulClickFlag=WM_BUTTON1DBLCLK;
        else
            pHP->ulClickFlag=WM_BUTTON1CLICK;
        }
    }
/*                                                                                      *\
 * PC/2 as WPS process should appear in Window List, to enable the user to unhide PC/2  *
 * after hiding, but can't be closed from Window List, Exit smarticon or Exit PC/2      *
 * menuentry. The door using the accelerator F3 will stay open, but undokumented. The   *
 * same applies when running PC/2 as the PROTSHELL, but even the backdoor is dangerous  *
 * as exiting the PROTSHELL process causes a PM hang.                                   *
\*                                                                                      */
    pucRunWorkPlace=NULL;
                                        /* Query environment for RUNWORKPLACE=[d:]path\PC2.EXE */
    DosScanEnv("RUNWORKPLACE", (PSZ *)&pucRunWorkPlace);
                                        /* Find P of PC2.EXE (or PMSHELL.EXE) */
    if(pucRunWorkPlace) for( ; *pucRunWorkPlace; pucRunWorkPlace++)
        {
        if((*pucRunWorkPlace!='P') && (*pucRunWorkPlace!='p')) continue;
                                        /* We found a P or p now look for PC2.EXE */
        if(!strnicmp(pucRunWorkPlace, "PC2.EXE", sizeof("PC2.EXE")-1))
            {
                                        /* If PC/2 is the WPS process save fact */
            pHP->ulRuntimeFlag|=PC2RUNNINGASWPS;
            }
        }
                                        /* If PC/2's parent process is 0 we're running as
                                           the PROTSHELL (or detached, but that's very
                                           unlikely) */
    if(pHP->pPib->pib_ulppid==0)
        pHP->ulRuntimeFlag|=PC2RUNNINGASPROTSHELL;
/*                                                                                      *\
 * initialize that our process gets dumped to a PMDUMP.nnn file once it traps. With the *
 * dump formatter, I'll have more chances to find the problem than just with the        *
 * register display (especially if we trap in OS/2 code). Additionally, the system      *
 * error popup is suppressed and the trap information is directly logged into the file  *
 * POPUPLOG.OS2. Both trap dumps files will be written into the root of the boot drive. *
\*                                                                                      */
    if(pHP->ulStatusFlag2 & ENABLEDIAGNOSTICS)
        {
        DosDumpProcess(DDP_ENABLEPROCDUMP, ulBootDrive, ppib->pib_ulpid);
        DosSuppressPopUps(SPU_ENABLESUPPRESSION, ulBootDrive);
        }
/*                                                                                      *\
 * Translate the scan codes recognized by the input hook to the ASCII code that is      *
 * produced from a scan code under the current code page. Thanks to Robert Mahoney for  *
 * his e-mail to suggest using KBD$ to translate the scan code to ASCII characters!     *
 * As he used only had 1.90 available he couldn't know that I've added this in between  *
 * while working on 2.00. However, I've added the codepage query according to his       *
 * suggestion, though it also work when using 0 as the codepage.                        *
\*                                                                                      */
    {
    HFILE   hfileKeyBoard=0;
    APIRET  rc=0;
    ULONG   ulActionTaken=0;
    ULONG   ulParameterLengthInOut=0;
    ULONG   ulDataLengthInOut=0;
    ULONG   ulScancodeIndex=0;
    ULONG   ulKeyDataIndex=0;
    USHORT  usCodePage=0;

    typedef struct _KBDXLATE
    {
        KBDKEYINFO  kbdKeyInfo;
        USHORT      usKbdDDFlags;
        USHORT      usXlateFlags;
        USHORT      usXlateState1;
        USHORT      usXlateState2;
    } KBDXLATE;
    typedef KBDXLATE *PKBDXLATE;
    CPID        PPF;                    /* IOCTL Parameter packet format */
    KBDXLATE    DPF;                    /* IOCTL Data packed format */

    rc=DosOpen("\\DEV\\KBD$", (PHFILE)&hfileKeyBoard, &ulActionTaken, 0, FILE_NORMAL,
        OPEN_ACTION_OPEN_IF_EXISTS,
        OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE, 0);
#ifdef  DEBUG
    if(rc) printf("DosOpen KBD$ failed: %x\n", (int)rc);
#endif  /* DEBUG */
                                        /* Get codepage */
    memset(&PPF, 0, sizeof(PPF));
    ulParameterLengthInOut=sizeof(PPF);
    rc=DosDevIOCtl(hfileKeyBoard, IOCTL_KEYBOARD, KBD_GETCODEPAGEID,
        &PPF, sizeof(PPF), &ulParameterLengthInOut,
        NULL, 0, NULL);
                                        /* Fix for Aurora as it in contrast to other versions
                                           does overwrite the value when calling KBD_XLATESCAN */
    usCodePage=PPF.idCodePage;
#ifdef  DEBUG
    if(rc)
        printf("DosDevIOCTL to get codepage failed: %x\n", (int)rc);
#endif  /* DEBUG */
                                        /* Special thanks to Istvan Kovacs for hinting me that different
                                           keyboards might use different physical keys to represent ASCII
                                           values than US and German keyboards (the ones I have/know) */
#if     FALSE
                                        /* Loop through all possible Hotkeys, and convert the
                                           Hotkey's scan code to an uppercase ASCII value. This allows
                                           the Hotkeys to be NLS-enabled, as e.g. scan code 0x2C gives
                                           ASCII Y in a US, and Z an a German keyboard (code page).
                                           Unfortunately that assumes that the keys used are always the
                                           same which is *not* true, e.g. on a Hungarian keyboard the key
                                           for 0 is left beside the key for 1 but we don't even have a
                                           scancode for that key 0 in our table */
    for(ulKeyDataIndex=0; ulKeyDataIndex<KEYDATACOUNT; ulKeyDataIndex++)
        {
        memset(&DPF, 0, sizeof(DPF));
        DPF.kbdKeyInfo.chScan=pHP->pKeyData[ulKeyDataIndex].ucScanCode;
                                        /* Fix for Aurora, see above */
        memset(&PPF, 0, sizeof(PPF));
        PPF.idCodePage=usCodePage;
                                        /* It's the final character and we request immediate conversion */
        DPF.kbdKeyInfo.fbStatus=0x60;
        ulParameterLengthInOut=sizeof(PPF);
        ulDataLengthInOut=sizeof(DPF);
        rc=DosDevIOCtl(hfileKeyBoard, IOCTL_KEYBOARD, KBD_XLATESCAN,
            &PPF, sizeof(PPF), &ulParameterLengthInOut,
            &DPF, sizeof(DPF), &ulDataLengthInOut);
#ifdef  DEBUG
        if(rc) printf("DosDevIOCtl to translate scan code failed %x\n", (int)rc);
#endif  /* DEBUG */
        pHP->pKeyData[ulKeyDataIndex].usCh=toupper(DPF.kbdKeyInfo.chChar);
                                        /* Because PC2HOOK.DLL may already be loaded in the system,
                                           we have to clear the Hotkeys already used from the previous
                                           invokation of PC/2. E.g. IBM CM/2 does lock PC2HOOK.DLL,
                                           for unknown reasons, even if PC/2 gets terminated, so
                                           the next invokation may get uninitialized Hotkey data */
        pHP->pKeyData[ulKeyDataIndex].bUsed=FALSE;
        }
#else
                                        /* To be not just NLS-enabled but independent of the physical keyboard
                                           layout we have to use a better approach than above. What we do is
                                           we loop through all scancodes that are part of the alphanumeric
                                           block of the keyboard (and that's always the same as once defined
                                           by IBM with the XT, AT and MF keyboards, because if different keyboards
                                           would have their individual scan codes and BIOS one would not
                                           be able to use different keyboards on different PCs). And as one
                                           can find in the good old IBM AT3 Technical Reference (it's a shame
                                           that those things no longer get published, because it did also contain
                                           the systemboard layout and the BIOS listing!!!) the scancodes range from
                                           1 to 64.
                                           We thus loop through all possible scancodes, ask the keyboard driver
                                           which ASCII character would result from that key, search for that
                                           character in our table and write the scancode there afterwards (note
                                           that more than one ASCII value appears multiple times due to the
                                           combination of it with ALT, CTRL and ALT+CTRL! */
    for(ulScancodeIndex=1; ulScancodeIndex<=64; ulScancodeIndex++)
        {
        memset(&DPF, 0, sizeof(DPF));
        DPF.kbdKeyInfo.chScan=(UCHAR)ulScancodeIndex;
                                        /* Fix for Aurora, see above */
        memset(&PPF, 0, sizeof(PPF));
        PPF.idCodePage=usCodePage;
                                        /* It's the final character and we request immediate conversion */
        DPF.kbdKeyInfo.fbStatus=0x60;
        ulParameterLengthInOut=sizeof(PPF);
        ulDataLengthInOut=sizeof(DPF);
        rc=DosDevIOCtl(hfileKeyBoard, IOCTL_KEYBOARD, KBD_XLATESCAN,
            &PPF, sizeof(PPF), &ulParameterLengthInOut,
            &DPF, sizeof(DPF), &ulDataLengthInOut);
#ifdef  DEBUG
        if(rc) printf("DosDevIOCtl to translate scan code failed %x\n", (int)rc);
#endif  /* DEBUG */
                                        /* We now know the NLS ASCII character represented by the current
                                           key's scancode for the current keyboard codepage, now let's see
                                           if that character is one we are interested in being able to
                                           detect it */
        for(ulKeyDataIndex=0; ulKeyDataIndex<KEYDATACOUNT; ulKeyDataIndex++)
            {
            if(pHP->pKeyData[ulKeyDataIndex].usCh==toupper(DPF.kbdKeyInfo.chChar))
                {
                pHP->pKeyData[ulKeyDataIndex].ucScanCode=(UCHAR)ulScancodeIndex;
                }
                                        /* Because PC2HOOK.DLL may already be loaded in the system,
                                           we have to clear the Hotkeys already used from the previous
                                           invokation of PC/2. E.g. IBM CM/2 does lock PC2HOOK.DLL,
                                           for unknown reasons, even if PC/2 gets terminated, so
                                           the next invokation may get uninitialized Hotkey data */
            pHP->pKeyData[ulKeyDataIndex].bUsed=FALSE;
            }
        }
#endif  /* FALSE */
    rc=DosClose(hfileKeyBoard);
#ifdef  DEBUG
    if(rc) printf("DosClose KBD$ failed: %x\n", (int)rc);
#endif  /* DEBUG */
    }
/*                                                                                      *\
 * Setup PC/2's environment into a form of: EnvVar1=Value1\r\nEnvVar2=Value2...\0 by    *
 * translating it from the process' info block                                          *
\*                                                                                      */
    {
    ULONG   ulPC2EnvironmentSize;
    UCHAR   *pucPC2EnvironmentEntry;
                                        /* PC/2's environment in the process information block
                                           is separated by \0, and terminated by \0\0 */
                                        /* Note: During testing the condition of the for-loop, we
                                           assume that when pucPC2EnvironmentEntry is not true,
                                           *pucPC2EnvironmentEntry is not evaluated, which would
                                           be typically for modern compilers */
    for(ulPC2EnvironmentSize=0, pucPC2EnvironmentEntry=(UCHAR *)pHP->pPib->pib_pchenv;
        (pucPC2EnvironmentEntry && *pucPC2EnvironmentEntry);
        pucPC2EnvironmentEntry=strchr(pucPC2EnvironmentEntry, '\0'),
            (pucPC2EnvironmentEntry ? pucPC2EnvironmentEntry++ : pucPC2EnvironmentEntry))
        {
                                        /* Add space for \r\n */
        ulPC2EnvironmentSize+=(strlen(pucPC2EnvironmentEntry)+2);
        }
                                        /* Allocate required size including a trailing \0 */
    pHP->pucPC2Environment=malloc(ulPC2EnvironmentSize+1);
    memset(pHP->pucPC2Environment, '\0', ulPC2EnvironmentSize+1);
                                        /* Translate environment from process information block
                                           to the form used PC/2 internal */
    for(pucPC2EnvironmentEntry=(UCHAR *)pHP->pPib->pib_pchenv;
        (pucPC2EnvironmentEntry && *pucPC2EnvironmentEntry);
        pucPC2EnvironmentEntry=strchr(pucPC2EnvironmentEntry, '\0'),
            (pucPC2EnvironmentEntry ? pucPC2EnvironmentEntry++ : pucPC2EnvironmentEntry))
        {
        strcat(pHP->pucPC2Environment, pucPC2EnvironmentEntry);
        strcat(pHP->pucPC2Environment, "\r\n");
        }
    }
/*                                                                                      *\
 * Load DOSCALL1.873 DOSSETEXTLIBPATH and DOSCALL1.874 DOSQUERYEXTLIBPATH OS/2 APIs,    *
 * which were newly introduced by OS/2 3.00 WARP. These APIs are use to set and query   *
 * the EXTENDED LIBPATH, allowing to change the search path for dynamically loading     *
 * DLLs. These settings are bound to the process's information block. Applications      *
 * started by PC/2 may specify values for these variables, which are inherited from     *
 * PC/2 after setting them before starting the application.                             *
\*                                                                                      */
    if(!DosLoadModule(ucBuffer, sizeof(ucBuffer)-1, "DOSCALL1", &hDLLDOSCALL1)!=NO_ERROR)
        {
        DosQueryProcAddr(hDLLDOSCALL1, ORDDOSSETEXTLIBPATH, NULL, (PFN *)(&pHP->DosSetExtLIBPATH));
        DosQueryProcAddr(hDLLDOSCALL1, ORDDOSQUERYEXTLIBPATH, NULL, (PFN *)(&pHP->DosQueryExtLIBPATH));
                                        /* Up to version 2.00 I had used the 16bit version of the
                                           dynamic LIBPATH API as I found them in a document not
                                           stating them being only 16bit. This seems to have been
                                           the cause that starting the first application had a
                                           noticable delay. This has been corrected by using the
                                           32bit version.
        DosQueryProcAddr(hDLLDOSCALL1, 998, "DOSSETEXTLIBPATH", (PFN *)(&pHP->DosSetExtLIBPATH));
        DosQueryProcAddr(hDLLDOSCALL1, 999, "DOSQUERYEXTLIBPATH", (PFN *)(&pHP->DosQueryExtLIBPATH));
                                           */
        }
                                        /* If possible save the dynamic LIBPATH environment that PC/2
                                           got inherited from its shell process */
    if(pHP->DosQueryExtLIBPATH)
        {
        UCHAR   *pucExtLibPath=calloc(LENENVENTRY, sizeof('\0'));

        if(pHP->DosQueryExtLIBPATH(pucExtLibPath, BEGINLIBPATH)==NO_ERROR)
            if(strcmp(pucExtLibPath, ""))
                {
                pHP->pucBeginLibpath=calloc(strlen(pucExtLibPath)+BEGINLIBPATHLEN+2, sizeof('\0'));
                strcpy(pHP->pucBeginLibpath, "BEGINLIBPATH=");
                strcat(pHP->pucBeginLibpath, pucExtLibPath);
                }
        if(pHP->DosQueryExtLIBPATH(pucExtLibPath, ENDLIBPATH)==NO_ERROR)
            if(strcmp(pucExtLibPath, ""))
                {
                pHP->pucEndLibpath=calloc(strlen(pucExtLibPath)+ENDLIBPATHLEN+2, sizeof('\0'));
                strcpy(pHP->pucEndLibpath, "ENDLIBPATH=");
                strcat(pHP->pucEndLibpath, pucExtLibPath);
                }
        free(pucExtLibPath);
        }
/*                                                                                      *\
 * Try to query the number of CPUs available (assume 1 if call fails). Also try to load *
 * Load DOSCALL1.447 DOSGETPROCESSORSTATUS, DOSCALL1.448 DOSSETPROCESSORSTATUS and      *
 * DOSCALL1.976 DOSPERFSYSCALL, the first 2 will only be available on Warp Srv. SMP+,   *
 * while the later is available beginning with Warp 4+. Only if the later one is        *
 * available we can display the CPU performance graphics in the SessionBar. To preserve *
 * compatibility with OS/2 2.x, dynamically try to load DosVerifyPidTid() which req.    *
 * Warp3+.                                                                              *
\*                                                                                      */
    if(DosQuerySysInfo(QSV_NUMPROCESSORS, QSV_NUMPROCESSORS,
        &pHP->ulNumProcessors, sizeof(pHP->ulNumProcessors))!=NO_ERROR)
        pHP->ulNumProcessors=1;
    DosQueryProcAddr(hDLLDOSCALL1, ORDDOSGETPROCESSORSTATUS, NULL, (PFN *)(&pHP->DosGetProcessorStatus));
    DosQueryProcAddr(hDLLDOSCALL1, ORDDOSSETPROCESSORSTATUS, NULL, (PFN *)(&pHP->DosSetProcessorStatus));
    DosQueryProcAddr(hDLLDOSCALL1, ORDDOSPERFSYSCALL, NULL, (PFN *)(&pHP->DosPerfSysCall));
    DosQueryProcAddr(hDLLDOSCALL1, ORDDOSVERIFYPIDTID, NULL, (PFN *)(&pDosVerifyPidTid));
/*                                                                                      *\
 * If PC/2 is running as the WPS process, get PC2SPOOL.DLL module handle.               *
\*                                                                                      */
    if((pHP->ulRuntimeFlag & PC2RUNNINGASWPS) || (pHP->ulStatusFlag1 & SHOWSPOOLERWINDOW))
        {
                                        /* Fully qualified path of PC2SPOOL.DLL */
        Rc=DosLoadModule(ucBuffer, sizeof(ucBuffer)-1, "PC2Spool", &hDLLPc2Spooler);
        if(Rc!=NO_ERROR)
            {                           /* DLL couldn't be found in the current PC/2
                                           directory or via the LIBPATH path */
            DOS_ERR(Rc, hwndFrame, HELP_PC2SPOOLDLL, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "PC/2 can't find PC2SPOOL.DLL, please check that the DLL resides in the directory "\
                "PC/2 was started from.\n\nSpooler window services will not be available - continuing...");
            }
        else
            {
            ULONG   *pulVersion;
            UCHAR   *pucMessage;

            Rc=DosQueryProcAddr(hDLLPc2Spooler, DLLENTRYPOINTSIGNATURE,
                "_PC2SPOOLSIGNATURE", (PFN *)(&pulVersion));
            if(Rc==NO_ERROR)
                {
#ifdef  DEBUG
                printf("PC2: Version PC2Spool.DLL %lX, expected %lX\n", *pulVersion, (ULONG)BLDLEVEL_SIGNATURE);
#endif  /* DEBUG */
                if(*pulVersion!=BLDLEVEL_SIGNATURE)
                    Rc=ERROR_BAD_FORMAT;
                }
            if(Rc!=NO_ERROR)
                {
                pucMessage=malloc(1024);
                sprintf(pucMessage, "The signature of the PC/2 DLL \"%s\" does not match with this build. "
                    "Please see the online help to verify PC/2 is installed correctly!\n\n", "PC2SPOOL.DLL");
                strcat(pucMessage, "Spooler window services will not be available - continuing...");
                USR_ERR(hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1, pucMessage);
                free(pucMessage);
                }
            if(Rc==NO_ERROR)
                {
                Rc=DosQueryProcAddr(hDLLPc2Spooler, DLLENTRYPOINTSPOOLERINITIALIZE,
                    "SpoolerInitialize", (PFN *)(&pSpoolerInitialize));
                if(Rc!=NO_ERROR)
                    DOS_ERR(Rc, hwndFrame, HELP_PC2SPOOLDLL, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                        "PC/2 can't load from PC2SPOOL.DLL, maybe you are using an outdated version\n\n"
                        "Spooler window services will not be available - continuing...");
                }
            if(Rc!=NO_ERROR)
                {
                DosFreeModule(hDLLPc2Spooler);
                hDLLPc2Spooler=NULLHANDLE;
                }
            pHP->hDLLPc2Spooler=hDLLPc2Spooler;
            }
        }
    }
/*                                                                                      *\
 * Initialize PC2HOOK.DLL.                                                              *
\*                                                                                      */
    pInitialize();                      /* Initialize data in PC2Hook DLL */
/*                                                                                      *\
 * Now setup the Popup-Menu for the Overview window client area. The menu gets loaded   *
 * with all menuentries checked (that is enabled in the Overview window's menubar). By  *
 * comparing this with the bitmapped flag from the profile, we can uncheck the items    *
 * the user did request to hide.                                                        *
\*                                                                                      */
    {
    ULONG   ulMenuItem;
    ULONG   ulShiftMask;

    pHP->hwndClientMenu=WinLoadMenu(
        pHP->hwndClient,                /* Owner window handle */
        NULLHANDLE,                     /* Ressource from file */
        ID_OVCLIENTMENU);               /* Menu identifier in ressource file */
    for(ulShiftMask=0x00000001, ulMenuItem=ID_OVICONEXIT;
        ulMenuItem<=ID_OVICONHELP;
        ulShiftMask<<=1, ulMenuItem++)
        if(!(pHP->ulOVSmartIconsFlag & ulShiftMask))
            WinSendMsg(pHP->hwndClientMenu, MM_SETITEMATTR,
                MPFROM2SHORT((USHORT)ulMenuItem, TRUE), MPFROM2SHORT(MIA_CHECKED, 0));
    }
/*                                                                                      *\
 * Now setup the Popup-Menu by loading the data from the profile and install the hook   *
 * into the system input queue, load the Popup-Menu while PC/2 startup.                 *
\*                                                                                      */
                                        /* Load popup menu */
    pHP->hwndPopupMenu=WinLoadMenu(
        HWND_OBJECT,                    /* Owner window handle */
        pHP->hDLLPc2Resource,           /* Ressource from file */
        ID_PC2POPUPMENU);               /* Menu identifier in ressource file */
                                        /* Load the data from the profile */
                                        /* Set this font also into the Popup-Menu */
    WinSetPresParam(pHP->hwndPopupMenu, PP_FONTNAMESIZE,
        sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
    WinPostMsg(pHP->hwndClient, WM_PC2STARTUP, NULL, NULL);
/*                                                                                      *\
 * Here we loop dispatching the messages...                                             *
\*                                                                                      */
    while(TRUE)
        {
        while(WinGetMsg(habPc2, &qmsg, 0, 0, 0))
                                        /* Dispatch messages to window procedure */
            WinDispatchMsg(habPc2, &qmsg);
                                        /* Break only when one WM_QUIT occurred before, because
                                           only after WM_CLOSE was processed we are really ready
                                           to shutdown (and WM_CLOSE posts WM_QUIT again) */
        if(pHP->ulRuntimeFlag & (QUITFROMWINDOWLIST|QUITFROMSHUTDOWN))
            {
#ifdef  DEBUG
            printf("PC2: WM_QUIT - exiting message loop\n");
#endif  /* DEBUG */
            break;
            }
                                        /* If we get closed by a system shutdown, we do not want to
                                           hold up system shutdown with a verification question */
        if((qmsg.hwnd==NULLHANDLE) && (qmsg.mp2!=(MPARAM)pHP->hwndFrame))
            {
#ifdef  DEBUG
            printf("PC2: WM_QUIT from ShutDown received\n");
#endif  /* DEBUG */
            pHP->ulRuntimeFlag|=QUITFROMSHUTDOWN;
            }
        else
            {
#ifdef  DEBUG
            printf("PC2: WM_QUIT from Window List received\n");
#endif  /* DEBUG */
            pHP->ulRuntimeFlag|=QUITFROMWINDOWLIST;
            }
                                        /* Dispatch WM_QUIT (which just removes the message from
                                           our queue) */
        WinDispatchMsg(habPc2, &qmsg);
                                        /* In case of an unrecoverable error quit anyway */
        if((pHP->ulRuntimeFlag & QUITFROMSHUTDOWN) ||
            (pHP->ulRuntimeFlag & PC2EMERGENCYEXIT))
            {
                                        /* Accept ShutDown (even when running as the WPS replacement
                                           as e.g. launching PMSHELL allows the user to trigger a
                                           ShutDown) */
#ifdef  DEBUG
            printf("PC2: WM_QUIT received, posting WM_CLOSE to cleanup\n");
#endif  /* DEBUG */
                                        /* Post WM_CLOSE to shutdown PC/2 in a orderly manner,
                                           which posts WM_QUIT again afterwards PC/2 cleaned up */
            WinPostMsg(pHP->hwndClient, WM_CLOSE, NULL, NULL);
            }
        else
            {
                                        /* If we should quit but not due to a ShutDown, process
                                           accordingly (but use WM_CLOSE to terminate in an orderly
                                           manner if required) */
                                        /* If PC/2 is running as a WPS replacement or PROTSHELL process,
                                           tell the user that the workplace process is always restarted
                                           (unless a ShutDown is being processed, which could be triggered
                                           e.g. by starting the PMSHELL) and the PROTSHELL can't terminate
                                           at all. */
            if(pHP->ulRuntimeFlag & PC2RUNNINGASWPS)
                {
                USR_ERR(pHP->hwndFrame, HELP_PC2RUNNINGASWPS, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "PC/2 is running as the WPS (WorkPlace Shell) process. The WorkPlace process mustn't be "\
                    "terminated. PC/2 will ignore your request - continuing...");
                                        /* Clear flags to prevent exiting by a subsequent invocation */
                pHP->ulRuntimeFlag &= ~(QUITFROMWINDOWLIST|QUITFROMSHUTDOWN);
                }
            else if(pHP->ulRuntimeFlag & PC2RUNNINGASPROTSHELL)
                {
                USR_ERR(pHP->hwndFrame, HELP_PC2RUNNINGASPROTSHELL, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "PC/2 is running as the PROTSHELL process. This process mustn't be terminated or PM "\
                    "will hang. PC/2 will ignore your request - continuing...");
                                        /* Clear flags to prevent exiting by a subsequent invocation */
                pHP->ulRuntimeFlag &= ~(QUITFROMWINDOWLIST|QUITFROMSHUTDOWN);
                }
            else
                {
#ifdef  DEBUG
                printf("PC2: WM_QUIT received, posting WM_CLOSE to cleanup\n");
#endif  /* DEBUG */
                                        /* Post WM_CLOSE to shutdown PC/2 in a orderly manner,
                                           which posts WM_QUIT again afterwards PC/2 cleaned up */
                WinPostMsg(pHP->hwndClient, WM_CLOSE, NULL, NULL);
                }
            }
        }
} while (FALSE);
                                        /* Free path references */
free(pucFilenameHLP);
#ifdef  DEBUG
printf("PC2: Waiting for threads to terminate\n");
#endif  /* DEBUG */
                                        /* Wait for working thread to close. We try to use the
                                           DosVerifyPidTid() API (available on Warp 3+) but can
                                           fall back to DosWaitThread() for 2.x */
#ifdef  DEBUG
if(pDosVerifyPidTid!=NULL)
    printf("PC2: Using DosVerifyPidTid()\n");
else
    printf("PC2: Using DosWaitThread()\n");
#endif  /* DEBUG */
if(pthreaddataWorkingThread!=NULL)
    {
    if(pDosVerifyPidTid!=NULL)
        while(pDosVerifyPidTid(ppib->pib_ulpid, pthreaddataWorkingThread->tidThread)==NO_ERROR)
            DosSleep(100);
    else
        DosWaitThread(&(pthreaddataWorkingThread->tidThread), DCWW_WAIT);
    }
#ifdef  DEBUG
printf("PC2: WTrd terminated\n");
#endif  /* DEBUG */
                                        /* Wait for I/O working thread to close */
if(pthreaddataIOThread!=NULL)
    {
    if(pDosVerifyPidTid!=NULL)
        while(pDosVerifyPidTid(ppib->pib_ulpid, pthreaddataIOThread->tidThread)==NO_ERROR)
            DosSleep(100);
    else
        DosWaitThread(&(pthreaddataIOThread->tidThread), DCWW_WAIT);
    }
#ifdef  DEBUG
printf("PC2: ITrd terminated\n");
#endif  /* DEBUG */
                                        /* Wait for network thread to close */
if(pthreaddataNetThread!=NULL)
    {
    if(pDosVerifyPidTid!=NULL)
        while(pDosVerifyPidTid(ppib->pib_ulpid, pthreaddataNetThread->tidThread)==NO_ERROR)
            DosSleep(100);
    else
        DosWaitThread(&(pthreaddataNetThread->tidThread), DCWW_WAIT);
    }
#ifdef  DEBUG
printf("PC2: NTrd terminated\n");
#endif  /* DEBUG */
#ifdef  DEBUG
printf("PC2: Threads terminated, cleaning up resources...\n");
#endif  /* DEBUG */
if(pHP!=NULL)
    {
                                        /* Free PC/2's translated environment */
    if(pHP->pucPC2Environment) free(pHP->pucPC2Environment);
                                        /* Free dynamic libpath inherited from shell */
    if(pHP->pucBeginLibpath) free(pHP->pucBeginLibpath);
    if(pHP->pucEndLibpath) free(pHP->pucEndLibpath);
                                        /* Destroy in memory presenation space */
    if(pHP->hpsOverviewMemory)
        {
        GpiSetBitmap(pHP->hpsOverviewMemory, NULLHANDLE);
        GpiDestroyPS(pHP->hpsOverviewMemory);
        }
    if(pHP->hpsSessionBarMemory)
        {
        GpiSetBitmap(pHP->hpsSessionBarMemory, NULLHANDLE);
        GpiDestroyPS(pHP->hpsSessionBarMemory);
        }
                                        /* Destroy in memory bitmap */
    if(pHP->hbmOverviewMemory)
        GpiDeleteBitmap(pHP->hbmOverviewMemory);
    if(pHP->hbmSessionBarMemory)
        GpiDeleteBitmap(pHP->hbmSessionBarMemory);
                                        /* Destroy memory device context */
    if(hdcOverviewMemory)
        DevCloseDC(hdcOverviewMemory);
    if(hdcSessionBarMemory)
        DevCloseDC(hdcSessionBarMemory);
                                        /* Destroy the shared memory for window texts */
    if(pHP->pucWindowText) DosFreeMem((PVOID)pHP->pucWindowText);
    }
#ifdef  DEBUG
printf("PC2: Cleaning up DLLs...\n");
#endif  /* DEBUG */
                                        /* Free DLL references */
if(hDLLPc2Spooler)
    DosFreeModule(hDLLPc2Spooler);
if(hDLLPc2Hook)
    DosFreeModule(hDLLPc2Hook);
if(hDLLPc2Resource)
    DosFreeModule(hDLLPc2Resource);
DosCloseEventSem(hevPc2);
if(pthreaddataWorkingThread)
    free(pthreaddataWorkingThread);
if(pthreaddataIOThread)
    free(pthreaddataIOThread);
if(pthreaddataNetThread)
    free(pthreaddataNetThread);
if(WinCloseDown(&hwndHelp, &habPc2, &hmqPc2)==FALSE)
    return(1);
if(hDLLDOSCALL1)
    DosFreeModule(hDLLDOSCALL1);
DosCloseMutexSem(hmtxHang);
/*                                                                                      *\
 * Deregister exception handler for the main thread                                     *
\*                                                                                      */
#ifdef  DEBUG
printf("PC2: Cleaning up Exceptionhandler...\n");
#endif  /* DEBUG */
RegisterExceptionHandler(FALSE, &excpregrecThread, NULL);
DosCloseMutexSem(hmtxExceptionHandler);
free(pucFilenameTRP);
                                        /* Check for memory leaks */
#ifdef  DEBUG
#ifdef  __DEBUG_ALLOC__
printf("PC2: Checking memory management...\n");
_dump_allocated(32);                    /* Display memory leaks */
#endif  /* __DEBUG_ALLOC__ */
DosBeep(2000, 100);
DosBeep(500, 100);
DosBeep(2000, 100);
#endif  /* DEBUG */
#ifdef  DEBUG
printf("PC2: Exiting finally...\n");
#endif  /* DEBUG */
return(0);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure is the PC/2 window procedure.                                         *
\*--------------------------------------------------------------------------------------*/
MRESULT EXPENTRY PC2_MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static ULONG    ulThreadReady=0;        /* Working thread ready indicator. It may be seen as (or
                                           replaced by) a semaphore that is set (THREADBUSY) whenever the
                                           working thread is doing his (time-consuming) work. Requests
                                           for further work are ignored until the thread gets ready
                                           again, to avoid queuing of work (which may be invalidated
                                           by the last job) */
static USHORT   usMenuCommand;          /* Last message from smarticons */
static HPOINTER hPointerMove;           /* Pointer when moving windows on Overview window */
static HPOINTER hPointerAction;         /* Pointer when performing the other actions on windows */
static ULONG    ulTimer;                /* Timer identity used to update spooler regularily */
static ULONG    ulTimerTicks=0;         /* Timer ticks (about every TIMERINTERVALLMS milliseconds) */
static HPOINTER hPointerReplacement;    /* Pointer used when we can't blit icon from executable */
static ULONG    ulEdgeActivation=FALSE; /* Set when Overview window was unhidden by Edge Activation */

                                        /* I had expected that messages I'm interested in can only happen after
                                           the main message loop is running (and a debugger trace confirmed that)
                                           but during 2.09e Beta testing from traps the opposite has been proven */
if(pHP==0)
    return((MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2));
                                        /* Process messages */
switch(msg)
{
case WM_MEASUREITEM:
/*                                                                                      *\
 * This message has a very peculiar behaviour for Menus (at least I have this           *
 * impression). The main bug (I really would call it a PM bug) is, that to specify a    *
 * menuentry with MIS_OWNERDRAW style you must NOT specify MIS_TEXT too, or the         *
 * menuentry's rectangle will be drawn with really nonsense values. But as a ownerdraw  *
 * menu also may contain text, I think it makes sense to allow MIT_TEXT too. This       *
 * would eliminate the requirement to keep a second list for the menuentries texts      *
 * (which PC/2 must do even without ownerdrawing the menu because our menu is dynamic,  *
 * but it would eliminate to search the text when measuring or drawing a menuentry).    *
 * Missing MIS_TEXT also prevent mnemonics to work, which is really inconvenient!.      *
\*                                                                                      */
    {
    OWNERITEM   *pOwnerItemMenu;        /* Passed by WM_MEASUREITEM */
    MENUDATA    *pMD;                   /* Popup-Menu item we're measuring */
    RECTL       rectlFnt;               /* Rectangle required to draw menuentry's text */
    ULONG       ulId;                   /* Popup-Menu item's identifier we're measuring */
    ULONG       ulHeight;               /* Menuentry's height which is either limited by the
                                           size of the menuentry's icon or text */

#ifdef  DEBUG_PC2
    printf("PC2: WM_MEASUREITEM");
#endif  /* DEBUG_PC2 */
                                        /* For our Popup Menu we measure the item using
                                           ownerdraw style */
    pOwnerItemMenu=(OWNERITEM *)PVOIDFROMMP(mp2);
                                        /* Get item's id and search item it belongs to */
    ulId=pOwnerItemMenu->idItem;
    pMD=SearchItem(pHP->pPopupMenu, &ulId);
                                        /* During resorting of the Popup Menu, we may be
                                           in the middle of the update */
    if(pMD==NULL)
        break;
                                        /* We ownerdraw this menuentry including the menuentry's icon,
                                           which we load now ("demand load") if not available currently */
    if(pMD->StatusFlag&ICONLOADED)
        {
#ifdef  DEBUG_PC2
        printf("\n");
#endif  /* DEBUG_PC2 */
        }
    else
        {
#ifdef  DEBUG_PC2
        printf(" - Preloading icon for %s\n", pMD->PgmTitle);
#endif  /* DEBUG_PC2 */
        InitializeIcon(pMD, TRUE);
        }
                                        /* Get text dimensions without actually drawing */
    memset(&rectlFnt, 0, sizeof(rectlFnt));
    rectlFnt.xRight=pHP->DesktopSize.x;
    rectlFnt.yTop=pHP->DesktopSize.y;
    WinDrawText(pOwnerItemMenu->hps, -1, pMD->PgmTitle, &rectlFnt, 0, 0,
        DT_LEFT|DT_BOTTOM|DT_MNEMONIC|DT_QUERYEXTENT|DT_MNEMONIC);
                                        /* Clear the lower rectangle border, because otherwise a (Sub)Menu that
                                           contains a vertical break also includes the part left to the vertical
                                           break:
                                           +----+----------+            +----+----+
                                           |    |          | instead of |    |    |
                                           +----+----------+            +----+----+
                                           <---> <---> <--->            <---> <--->
                                             a     a     b                a     b
                                                 <--------->
                                                      c
                                           Assuming that the left and right part of the menu (a and b) are of the
                                           same size, c is is drawn instead of b if we don't set the left border
                                           xLeft to 0 here (the rectangle passed to WM_MEASUREITEM contains the
                                           already measured size of the menuentries left to the vertical break
                                           for some unknown reasons - at least I don't know why PM thinks this makes
                                           sense this way) */
    pOwnerItemMenu->rclItem.xLeft=0;
                                        /* Adjust the menuentry's rectangle including the
                                           space required to draw the half-size menuentry's
                                           icon and the 3D border */
    pOwnerItemMenu->rclItem.xRight=pOwnerItemMenu->rclItem.xLeft+
        (rectlFnt.xRight-rectlFnt.xLeft);
                                        /* Space from menu to icon border + left icon frame border +
                                           space between icon's left frame and icon itself + icon's size +
                                           space between icon itself and icon's right frame +
                                           right icon frame border + space from icon to
                                           text */
    pOwnerItemMenu->rclItem.xRight+=ICONORTEXTSPACE+ICONFRAME+
        ICONSPACE+(pHP->lIconSize>>pHP->ulIconScale)+
        ICONSPACE+
        ICONFRAME+(ICONORTEXTSPACE<<1);
                                        /* Space from menuentry's lower border to icon border + lower icon
                                           frame border + space between icon's bottom frame and icon itself +
                                           icon's size + space beteen icon and icon's upper frame +
                                           upper icon frame border +
                                           space from icon border to menuentry's upper border */
    ulHeight=rectlFnt.yTop-rectlFnt.yBottom;
    if(ulHeight<(ICONFRAME+((ULONG)pHP->lIconSize>>pHP->ulIconScale)+ICONFRAME))
        ulHeight=ICONFRAME+((ULONG)pHP->lIconSize>>pHP->ulIconScale)+ICONFRAME;
    ulHeight+=ICONORTEXTSPACE+ICONORTEXTSPACE+ICONSPACE+ICONSPACE;
                                        /* Clear the lower rectangle border, because otherwise changing
                                           the presentation parameters (that is the Popup-Menu's font
                                           correctly calculates a rectangle not based at 0 (but based
                                           on the position for the previous presentation parameters)
                                           which will cause the rectangles to ownerdraw menuentries much
                                           too large (for me it seems that PM just uses yTop to calculate
                                           the item's height assuming yBottom is 0 instead of using the
                                           difference yTop - yBottom as the height). (I don't know why PM
                                           thinks this makes sense this way, too) */
    pOwnerItemMenu->rclItem.yBottom=0;
    pOwnerItemMenu->rclItem.yTop=pOwnerItemMenu->rclItem.yBottom+ulHeight;
    return((MRESULT)(TRUE));
    }
    break;

case WM_DRAWITEM:
    {
    OWNERITEM   *pOwnerItemMenu;        /* Passed by WM_MEASUREITEM */
    MENUDATA    *pMD;                   /* Popup-Menu item we're measuring */
    ULONG       ulId;                   /* Popup-Menu item's identifier we're measuring */
    RECTL       rectlItem;              /* Menuentry's rectangle */
    ULONG       ulIconX;                /* Menuentry's horizontal center of menuentry's icon */
    ULONG       ulIconY;                /* Menuentry's vertical center of menuentry's icon */
    LONG        lColor;                 /* Menuentry's background color */
    HPS         hpsMenu;                /* Cache menuentry's presentation space */

                                        /* For our Popup Menu we draw the textitems using
                                           ownerdraw style */
    pOwnerItemMenu=(OWNERITEM *)PVOIDFROMMP(mp2);
    hpsMenu=pOwnerItemMenu->hps;
                                        /* Get item's id and search item it belongs to */
    ulId=pOwnerItemMenu->idItem;
    pMD=SearchItem(pHP->pPopupMenu, &ulId);
                                        /* During resorting of the Popup Menu, we may be
                                           in the middle of the update */
    if(pMD==NULL)
        break;
    rectlItem=pOwnerItemMenu->rclItem;
                                        /* Draw the 3D border around the menuentry */
    WinDrawBorder(hpsMenu, &pOwnerItemMenu->rclItem, 1, 1, 0, 0,
        ((pOwnerItemMenu->fsAttribute&MIA_HILITED) ? DB_DEPRESSED : DB_RAISED));
                                        /* Set background color to SYSCLR_MENU and
                                           background mix to erase previously drawn text. Another bug (?)
                                           of ownerdrawn menus is, that if the user modifications to the
                                           default colors (e.g. highlighted background color) must be
                                           drawn self, unfortunately this overpaints the arrow bitmap
                                           of submenus! */
    lColor=SYSCLR_MENU;
    GpiSetBackColor(hpsMenu, lColor);
    GpiSetBackMix(hpsMenu, BM_OVERPAINT);
                                        /* Adjust rectangle to draw the menuentry's text by
                                           Space from menu to icon border + left icon frame border +
                                           icon's size + right icon frame border + space from icon to
                                           text */
    rectlItem.xLeft+=ICONORTEXTSPACE+ICONFRAME+
        (pHP->lIconSize>>pHP->ulIconScale)+
        ICONFRAME+(ICONORTEXTSPACE<<1);
                                        /* If the menuentry is highlighted, we move the text
                                           right and down by 1 pixel. As we overpaint the old
                                           text 1 pixel right and down too, we first remove
                                           the text at the old position by drawing the text
                                           with the background color again before drawing the
                                           text at the new position */
    if((pOwnerItemMenu->fsAttributeOld&MIA_HILITED)!=(pOwnerItemMenu->fsAttribute&MIA_HILITED))
        {
        RECTL   rectlItemOld=rectlItem;
                                        /* Erase old text by drawing it again with the background
                                           color */
        if(pOwnerItemMenu->fsAttributeOld&MIA_HILITED)
            {
            rectlItemOld.xLeft+=2;
            rectlItemOld.yTop-=2;
            }
                                        /* Unfortunately for ownerdraw menuitems PM does not
                                           support setting the menuitem's text and therefore
                                           does not react on mnemonics (which I think is a bad
                                           design) */
        WinDrawText(hpsMenu, -1, pMD->PgmTitle, &rectlItemOld,
            lColor, lColor, DT_LEFT|DT_VCENTER|DT_MNEMONIC);
        }
                                        /* Redraw current text by using the menu's colors,
                                           as we currently don't handle hilite attributes other
                                           then offsetting the text position */
    if(pOwnerItemMenu->fsAttribute&MIA_HILITED)
        {
        rectlItem.xLeft+=2;
        rectlItem.yTop-=2;
        }
    WinDrawText(hpsMenu, -1, pMD->PgmTitle, &rectlItem,
        SYSCLR_MENUTEXT, lColor, DT_LEFT|DT_VCENTER|DT_MNEMONIC);
                                        /* Adjust horizontal position of menuentry's icon border */
    ulIconX=pOwnerItemMenu->rclItem.xLeft+ICONORTEXTSPACE+ICONFRAME+ICONSPACE+
        ((pHP->lIconSize>>pHP->ulIconScale)>>1);
    rectlItem.xLeft=pOwnerItemMenu->rclItem.xLeft+ICONORTEXTSPACE;
    rectlItem.xRight=rectlItem.xLeft+ICONFRAME+ICONSPACE+
        (pHP->lIconSize>>pHP->ulIconScale)+ICONSPACE+ICONFRAME;
                                        /* Get horizontal center to draw menuentry's icon border
                                           centered around this line */
    ulIconY=(pOwnerItemMenu->rclItem.yTop-pOwnerItemMenu->rclItem.yBottom)>>1;
    rectlItem.yBottom=pOwnerItemMenu->rclItem.yBottom+ulIconY-
        ((pHP->lIconSize>>pHP->ulIconScale)>>1)-ICONFRAME-ICONSPACE;
    rectlItem.yTop=pOwnerItemMenu->rclItem.yBottom+ulIconY+
        ((pHP->lIconSize>>pHP->ulIconScale)>>1)+ICONFRAME+ICONSPACE;
    WinDrawBorder(hpsMenu, &rectlItem, ICONFRAME, ICONFRAME, 0, 0, DB_RAISED);
                                        /* Get the rectangle to blit the icon into and blit it (when
                                           blitting the coordinates are exclusive, so the rectangle
                                           covers an additional right and top column) */
    rectlItem.xLeft=ulIconX-((pHP->lIconSize>>pHP->ulIconScale)>>1);
    rectlItem.yBottom=pOwnerItemMenu->rclItem.yBottom+ulIconY-((pHP->lIconSize>>pHP->ulIconScale)>>1);
    rectlItem.xRight=rectlItem.xLeft+(pHP->lIconSize>>pHP->ulIconScale);
    rectlItem.yTop=rectlItem.yBottom+(pHP->lIconSize>>pHP->ulIconScale);
                                        /* Reset the presentation space attributes, as avoiding this may
                                           cause the icons to be drawn in wrong colors */
    GpiResetPS(hpsMenu, GRES_ALL);
                                        /* We assume that a color and monochrome bitmap is contained in
                                           the icon, which may not be true at all */
    BlitIcon(hpsMenu, pMD->hpointerIcon, &rectlItem);
    if(BlitIcon(hpsMenu, pMD->hpointerIcon, &rectlItem))
        BlitIcon(hpsMenu, hPointerReplacement, &rectlItem);
                                        /* If menuentry should be disabled, just disable it. For
                                           reasons unknown to me we have to make some workarounds
                                           as MIA_DISABLED does not seem to work like MIA_HILITED
                                           as it is supposed to from the documentation (even if I
                                           remove MIA_DISABLED PM sometimes does disable the entry
                                           once again though it shouldn't!) */
    if(pOwnerItemMenu->fsAttribute&MIA_DISABLED)
        {
        if((pMD->Next!=NULL) ||
            (pOwnerItemMenu->fsAttribute&MIA_HILITED) ||
            (pOwnerItemMenu->fsAttributeOld&MIA_HILITED))
            DisableWindowRect(hpsMenu, &pOwnerItemMenu->rclItem, lColor);
        pOwnerItemMenu->fsAttributeOld=(pOwnerItemMenu->fsAttribute&=~MIA_DISABLED);
        }
                                        /* Avoid that PM inverts the rectangle of menuentries
                                           with attribute MIA_HILITED (see PM Reference for the
                                           explanation how that message is supposed to work,
                                           in a few words powerful but a little bit confusing) */
    pOwnerItemMenu->fsAttributeOld=(pOwnerItemMenu->fsAttribute&=~MIA_HILITED);
    return((MRESULT)TRUE);
    }

#ifdef  DEBUG
case WM_USER+1000:
    printf("WM_USER+1000: mp1: %08X, mp2: %08X\n", (int)mp1, (int)mp2);
    break;

case WM_USER+1001:
    printf("WM_USER+1001: mp1: %08X, mp2: %08X\n", (int)mp1, (int)mp2);
    break;

case WM_USER+1002:
    printf("WM_USER+1002: mp1: %08X, mp2: %08X\n", (int)mp1, (int)mp2);
    break;
#endif  /* DEBUG */

case WM_CREATE:                         /* Create window by WinCreateStdWindow() */
                                        /* First call default window procedure */
    WinDefWindowProc(hwnd, msg, mp1, mp2);
    break;

case WM_BUTTON1DOWN:
    {
                                        /* Display client menu if CTRL keys are pressed while mouse
                                           button 1 is pressed */
    if(WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000)
        {
        POINTL  pointlPopupMenu;

        WinQueryPointerPos(HWND_DESKTOP, &pointlPopupMenu);
        WinSendMsg(pHP->hwndClientMenu, MM_SETITEMATTR,
            MPFROM2SHORT(ID_OVSUBMENUICONS, TRUE), MPFROM2SHORT(MIA_DISABLED, 0));
        WinPopupMenu(HWND_DESKTOP, hwnd, pHP->hwndClientMenu,
            pointlPopupMenu.x, pointlPopupMenu.y, ID_OVSUBMENUICONS,
            PU_POSITIONONITEM | PU_HCONSTRAIN | PU_VCONSTRAIN | PU_KEYBOARD |
            PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2);
        }
    }
    break;

case WM_BUTTON1UP:
    break;

case WM_PAINT:
    {
    HPS     hpsClient;
    RECTL   rectlClient;

#ifdef  DEBUG_PC2
    printf("PC2: WM_PAINT\n");
#endif  /* DEBUG_PC2 */
                                        /* Get a cached presentation space */
    hpsClient=WinBeginPaint(hwnd, NULLHANDLE, &rectlClient);
                                        /* When exiting, display a static text instead of the
                                           windows in the Overview window */
    if((pHP==NULL) || (pHP->ulRuntimeFlag & PC2CLEANUP))
        {
        static  HWND    hwndUserInfo=NULLHANDLE;
        SWP             swpUserInfo;

        swpUserInfo=pHP->swpPC2Client;
        if(hwndUserInfo==NULLHANDLE)
            hwndUserInfo=WinCreateWindow(pHP->hwndFrame, WC_STATIC, "Closing! - Please wait...",
                WS_VISIBLE|WS_SAVEBITS|SS_TEXT|DT_VCENTER|DT_CENTER,
                swpUserInfo.x, swpUserInfo.y, swpUserInfo.cx, swpUserInfo.cy,
                pHP->hwndClient, HWND_TOP, FID_CLIENT, NULL, NULL);
        swpUserInfo.fl=WM_SIZE;
        swpUserInfo.hwnd=hwndUserInfo;
        WinSetMultWindowPos(pHP->habPc2, &swpUserInfo, 1);
        }
    else
        WinPostMsg(pHP->hwndWorkingThread, WM_PAINT, NULL, NULL);
    WinEndPaint(hpsClient);
    }
    break;

case WM_PRESPARAMCHANGED:
#ifdef  DEBUG_PC2
    printf("PC2: WM_PRESPARAMCHANGED\n");
#endif  /* DEBUG_PC2 */
    switch(LONGFROMMP(mp1))
    {
    ULONG       ulAttrFound=0;

    case PP_FONTNAMESIZE:
        {
        FATTRS      *pfatPC2FontOverview;

                                        /* Get font selected for PC/2's Overview window */
        if(WinQueryPresParam(pHP->hwndClient, PP_FONTNAMESIZE, 0, &ulAttrFound,
            sizeof(pHP->ucPC2WindowFont), pHP->ucPC2WindowFont, 0))
            {
            pfatPC2FontOverview=QueryPresParamFont(pHP->hwndClient);
                                        /* Post to working thread to change Overview window font */
            WinPostMsg(pHP->hwndWorkingThread, WM_SETPRESPARAMFONT,
                MPFROMLONG(PP_FONTNAMESIZE), MPFROMP(pfatPC2FontOverview));
            }
        }
        break;

    case PP_BACKGROUNDCOLOR:
                                        /* Get background color selected for PC/2's Overview window or
                                           the window rectangles when drawing in monochrome mode */
        if(WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000)
            WinQueryPresParam(pHP->hwndClient, PP_BACKGROUNDCOLOR, 0, &ulAttrFound,
                sizeof(pHP->ulOverviewRGBColorBgnd), &pHP->ulOverviewRGBColorBgnd, QPF_NOINHERIT);
        else
            WinQueryPresParam(pHP->hwndClient, PP_BACKGROUNDCOLOR, 0, &ulAttrFound,
                sizeof(pHP->ulOverviewRGBColorWnd), &pHP->ulOverviewRGBColorWnd, QPF_NOINHERIT);
        break;

    case PP_FOREGROUNDCOLOR:
                                        /* Get foreground color selected for PC/2's Overview window when
                                           drawing window rectangles in monochrome mode */
        WinQueryPresParam(pHP->hwndClient, PP_FOREGROUNDCOLOR, 0, &ulAttrFound,
            sizeof(pHP->ulOverviewRGBColorFgnd), &pHP->ulOverviewRGBColorFgnd, QPF_NOINHERIT);
        break;
    }
    WinPostMsg(pHP->hwndWorkingThread, WM_PAINT, NULL, NULL);
    break;

/*                                                                                      *\
 * Syntax: WM_PC2STARTUP, NULL, NULL                                                    *
\*                                                                                      */
case WM_PC2STARTUP:
/*                                                                                      *\
 * This message is posted during main window initialization, after the access to the    *
 * HOOKPARAMETERS, loaded from PC2HOOK.DLL, is possible, to complete startup.           *
\*                                                                                      */
    {
    SWP     swp;                        /* According to a tip from members of OS/2 development in
                                           an article in OS/2 Developer magazine, it is more
                                           efficient to call WinSetMultWindowPos(), because
                                           WinSetWindowPos() internally calls WinSetMultWindowPos()
                                           (even if just one window is moved) */
    ULONG   ulChangedFCF=0;             /* Frame flags to adjust */

#ifdef  DEBUG_PC2
    printf("PC2: WM_PC2STARTUP\n");
#endif  /* DEBUG_PC2 */
                                        /* Ensure Overview window on top of Z-Order when required */
    if(pHP->ulStatusFlag1 & KEEPONTOPOV)
        pHP->hwndZOrder[ZORDER_OVERVIEW]=WinQueryWindow(hwnd, QW_PARENT);
                                        /* We first have to read the Popup Menu, before we can use it */
    pHP->ulRuntimeFlag|=POPUPMENULOADING;
                                        /* Initialize to a value not used by PM (hopefully) */
    pHP->hwndDesktop=0;
    pHP->hwndWPSFrame=pHP->hwndWPSClient=0;
                                        /* Get the current screen resolution settings. We call
                                           to avoid WM_SIZE/WM_MOVE messages to be processed in
                                           between, because they rely on valid screen resolution data */
    WinSendMsg(pHP->hwndClient, WM_PC2STARTUPSCREEN, NULL, NULL);
                                        /* Setup a memory device contect to draw into, when
                                           using a "fast" video subsystem, e.g. XGA-2 */
    WinSendMsg(pHP->hwndClient, WM_PC2STARTUPMEMORYDC, NULL, NULL);
                                        /* During initial creation, the (invisible) Overview
                                           window was created with a titlebar but without a menubar.
                                           As WM_OVERVIEWFCF toggles the flags, test here what
                                           must be destroyed or created */
    if(pHP->ulStatusFlag1 & HIDETITLEBAR)
        ulChangedFCF|=FCF_TITLEBAR|FCF_HIDEBUTTON;
    if(!(pHP->ulStatusFlag1 & HIDESMARTICONBAR))
        ulChangedFCF|=FCF_MENU;
    if(ulChangedFCF)
        WinSendMsg(pHP->hwndClient, WM_OVERVIEWFCF, MPFROMLONG(ulChangedFCF), MPFROMLONG(FALSE));
                                        /* Load pointers from resource */
    hPointerMove=WinLoadPointer(HWND_DESKTOP, pHP->hDLLPc2Resource, ID_MOVEPOINTER);
    hPointerAction=WinLoadPointer(HWND_DESKTOP, pHP->hDLLPc2Resource, ID_ACTIONPOINTER);
    hPointerReplacement=WinLoadPointer(HWND_DESKTOP, pHP->hDLLPc2Resource, ID_ICONSESSIONUNKNOWN);
    usMenuCommand=ID_ICONMOVE;          /* Setup smarticon's control */
    pHP->Windows.lDesktop=-1;           /* Set to -1 because currently we don't know Desktop's name */
    pHP->Windows.lWindowList=-1;        /* Set to -1 because currently we don't know Window List's name */
                                        /* For installation display help panels */
    if(pHP->ulRuntimeFlag & INSTALLATIONHELP)
        WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_HELP), NULL);
/*                                                                                      *\
 * Set text to titlebar. Size & position and ensurance that position is limited is      *
 * done in WM_SIZE and WM_MOVE during window creation.                                  *
\*                                                                                      */
    WinSendMsg(pHP->hwndClient, WM_COMMAND, MPFROMSHORT(ID_DEBUG), MPFROM2SHORT(CMDSRC_OTHER, FALSE));
    swp.fl=SWP_HIDE|SWP_MOVE|SWP_SIZE|SWP_DEACTIVATE;
    swp.x=pHP->swpPC2.x;
    swp.y=pHP->swpPC2.y;
    swp.cx=pHP->swpPC2.cx;
    swp.cy=pHP->swpPC2.cy;
    swp.hwndInsertBehind=NULLHANDLE;
    swp.hwnd=pHP->hwndFrame;
    WinSetMultWindowPos(pHP->habPc2, &swp, 1);
                                        /* Activate Lockup dialog when requested */
    if(pHP->ulStatusFlag2 & LOCKUPONBOOT)
        WinPostMsg(pHP->hwndClient, WM_PC2LOCKUP, NULL, NULL);
                                        /* Create the I/O and working thread. Initially I created
                                           both threads while processing a single WM_PC2STARTUPTHREAD
                                           message, but suddenly the I/O thread didn't work anymore.
                                           The window handle was valid, but WM_CREATE and all other
                                           messages weren't processed (without WinPostMsg() returning
                                           a problem). Under the debugger it worked always without
                                           changing a single line. It seems that using VisualAge C++ 3.0
                                           and Warp 4 Toolkit generates corrupt executables as a recompilition
                                           with Warp 3 Toolkit always cured the problem */
    WinPostMsg(pHP->hwndClient, WM_PC2STARTUPTHREAD, NULL, NULL);
    }
/*                                                                                      *\
 * Initialize PC/2's QuickSwitch window                                                 *
\*                                                                                      */
    {
    ULONG   ulSwitchFCF=FCF_QUICKSWITCH;
    SWP     swpQuickSwitch;
    SIZEL   sizelSwitchClient={0, 0};

    if(WinRegisterClass(pHP->habPc2, (PSZ)PC2_CLASSNAME_SWITCH, (PFNWP)PC2_SwitchWindowProc,
        CS_SIZEREDRAW | CS_SAVEBITS | CS_MOVENOTIFY, 0))
        {
        pHP->hwndSwitchFrame=WinCreateStdWindow(HWND_DESKTOP, 0, &ulSwitchFCF,
            (PSZ)PC2_CLASSNAME_SWITCH, "", 0, (HMODULE)0, ID_PC2SWITCHWINDOW,
            &pHP->hwndSwitchClient);
        }
    if(pHP->hwndSwitchFrame==NULLHANDLE)
        {
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_QUICKSWITCH, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Creation of QuickSwitch window failed - continuing...");
        break;
        }
    swpQuickSwitch.fl=SWP_MOVE|SWP_SIZE|SWP_DEACTIVATE;
    swpQuickSwitch.x=0;
    swpQuickSwitch.y=0;
    swpQuickSwitch.cx=QUICKSWITCH_CX+pHP->lIconSize+(pHP->swpScreen.cx*QUICKSWITCH_SCALEX/100);
    swpQuickSwitch.cy=QUICKSWITCH_CY+pHP->lIconSize;
    swpQuickSwitch.hwndInsertBehind=NULLHANDLE;
    swpQuickSwitch.hwnd=pHP->hwndSwitchFrame;
    WinSetMultWindowPos(pHP->habPc2, &swpQuickSwitch, 1);
    CenterWindow(pHP->hwndSwitchFrame);
                                        /* Open a device context and presentation space to draw
                                           into QuickSwitch window */
    pHP->hdcSwitch=WinOpenWindowDC(pHP->hwndSwitchClient);
    pHP->hpsSwitch=GpiCreatePS(pHP->habPc2, pHP->hdcSwitch, &sizelSwitchClient, PU_PELS|GPIA_ASSOC|GPIT_MICRO);
                                        /* Set QuickSwitch window font */
    WinSetPresParam(pHP->hwndSwitchClient, PP_FONTNAMESIZE,
        sizeof(pHP->ucSwitchWindowFont), pHP->ucSwitchWindowFont);
    }
/*                                                                                      *\
 * Initialize PC/2's BubbleHelp window                                                  *
\*                                                                                      */
    {
    ULONG   ulBubbleHelpFCF=FCF_BUBBLEHELP;
    SWP     swpQuickBubbleHelp;
    SIZEL   sizelBubbleHelpClient={0, 0};

    if(WinRegisterClass(pHP->habPc2, (PSZ)PC2_CLASSNAME_BUBBLE, (PFNWP)PC2_BubbleHelpWindowProc,
        CS_SIZEREDRAW | CS_SAVEBITS | CS_MOVENOTIFY, 0))
        {
        pHP->hwndBubbleHelpFrame=WinCreateStdWindow(HWND_DESKTOP, 0, &ulBubbleHelpFCF,
            (PSZ)PC2_CLASSNAME_BUBBLE, "", 0, (HMODULE)0, ID_PC2BUBBLEWINDOW,
            &pHP->hwndBubbleHelpClient);
        }
    if(pHP->hwndBubbleHelpFrame==NULLHANDLE)
        {
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_BUBBLEHELP, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Creation of BubbleHelp window failed - continuing...");
        break;
        }
    swpQuickBubbleHelp.fl=SWP_MOVE|SWP_SIZE|SWP_DEACTIVATE;
    swpQuickBubbleHelp.x=100;
    swpQuickBubbleHelp.y=100;
    swpQuickBubbleHelp.cx=pHP->lIconSize<<3;
    swpQuickBubbleHelp.cy=pHP->lIconSize<<3;
    swpQuickBubbleHelp.hwndInsertBehind=NULLHANDLE;
    swpQuickBubbleHelp.hwnd=pHP->hwndBubbleHelpFrame;
    WinSetMultWindowPos(pHP->habPc2, &swpQuickBubbleHelp, 1);
                                        /* Open a device context and presentation space to draw
                                           into BubbleHelp window */
    pHP->hdcBubbleHelp=WinOpenWindowDC(pHP->hwndBubbleHelpClient);
    pHP->hpsBubbleHelp=GpiCreatePS(pHP->habPc2, pHP->hdcBubbleHelp, &sizelBubbleHelpClient, PU_PELS|GPIA_ASSOC|GPIT_MICRO);
                                        /* Set BubbleHelp window font */
    WinSetPresParam(pHP->hwndBubbleHelpClient, PP_FONTNAMESIZE,
        sizeof(pHP->ucSessionWindowFont), pHP->ucSessionWindowFont);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_TITLEBARICONS, BOOL bEnable, NULL                                         *
\*                                                                                      */
case WM_TITLEBARICONS:
/*                                                                                      *\
 * Depending on the user request, enable (create) or disable (destroy) TitleBar Smart-  *
 * icons by adding our code to other processes window procedures (Yes, we can add code  *
 * to other PM processes in OS/2 by using an InputHook() hook!), because posted         *
 * messages are processed in the input hook in the receiving windows process/thread     *
 * context and if we provide the code in a systemwide shared DLL (which a systemwide    *
 * hook requires), voila we are in a different address space!).                         *
 * Note: A InputHook() can do a thread context switch, but the WinSendMsg() hook runs   *
 *       in the thread context of the sender, not the receiver. Even when the           *
 *       WinSendMsg() hook is the last chance to process a message before the window    *
 *       procedure gets it, we have to use the InputHook(), even if posted messages are *
 *       processed asynchronously.                                                      *
\*                                                                                      */
    {
    if(LONGFROMMP(mp1))
        {
                                        /* Subclass only if there is at least one Titlebar Smarticon */
        if(pHP->ulTitlebarFlag==0)
            break;
        pHP->ulRuntimeFlag|=SUBCLASSICONS;
                                        /* Post a message to all frame windows to get into
                                           their window's thread context when processing this
                                           message in the InputHook() procedure */
        WinBroadcastMsg(HWND_DESKTOP, WM_SYSCOMMAND,
            MPFROMSHORT(SC_CONTROLSINSTALL), MPFROM2SHORT(CMDSRC_OTHER, FALSE),
            BMSG_POST|BMSG_DESCENDANTS|BMSG_FRAMEONLY);
        }
    else
        {
        pHP->ulRuntimeFlag&=(~SUBCLASSICONS);
                                        /* Call a function in the DLL to remove our code from
                                           other frame windows window procedures */
                                        /* Again post a message to get into other window's thread
                                           context when processing this message in the InputHook()
                                           procedure */
        WinBroadcastMsg(HWND_DESKTOP, WM_SYSCOMMAND,
            MPFROMSHORT(SC_CONTROLSCLEANUP), MPFROM2SHORT(CMDSRC_OTHER, FALSE),
            BMSG_POST|BMSG_DESCENDANTS|BMSG_FRAMEONLY);
        DosSleep(0);                    /* Preempt ourselves, but this also causes above message for
                                           our window to be processed afterwards. It would make sense
                                           to wait until pCleanup() returns there are no more subclassed
                                           windows (at least not windows in our process) */
        pCleanup();
        }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SESSIONBARUPDATE, ULONG ulTask, NULL                                      *
\*                                                                                      */
case WM_SESSIONBARUPDATE:
/*                                                                                      *\
 * Depending on the user request, disable (destroy), enable (create) or resize          *
 * SessionBar window.                                                                   *                                                                              *
\*                                                                                      */
    switch(LONGFROMMP(mp1))
    {
    case SESSIONBAR_CREATE:
        {
        ULONG   ulSessionFCF=FCF_SESSIONBAR;
        SIZEL   sizelSessionClient={0, 0};

        if(!WinRegisterClass(pHP->habPc2, (PSZ)PC2_CLASSNAME_SESSION, (PFNWP)PC2_SessionWindowProc,
            CS_SIZEREDRAW | CS_SAVEBITS | CS_MOVENOTIFY, 0))
            {
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_SESSIONBAR, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Can't create or modify PC/2's SessionBar - continuing...");
            break;
            }
        pHP->hwndSessionFrame=WinCreateStdWindow(HWND_DESKTOP, 0, &ulSessionFCF,
            (PSZ)PC2_CLASSNAME_SESSION, "", 0, (HMODULE)0, ID_PC2SESSIONWINDOW,
            &pHP->hwndSessionClient);
        if(pHP->hwndSessionFrame==NULLHANDLE)
            {
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_SESSIONBAR, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Can't create or modify PC/2's SessionBar - continuing...");
            break;
            }
                                        /* Open a device context and presentation space to draw
                                           into SessionBar window */
        pHP->hdcSession=WinOpenWindowDC(pHP->hwndSessionClient);
        pHP->hpsSession=GpiCreatePS(pHP->habPc2, pHP->hdcSession, &sizelSessionClient, PU_PELS|GPIA_ASSOC|GPIT_MICRO);
                                            /* Change color table into RGB mode */
        GpiCreateLogColorTable(pHP->hpsSession, 0L, LCOLF_RGB, 0L, 0L, NULL);
                                        /* Set SessionBar window font */
        WinSetPresParam(pHP->hwndSessionClient, PP_FONTNAMESIZE,
            sizeof(pHP->ucSessionWindowFont), pHP->ucSessionWindowFont);
                                        /* Position SessionBar according to last saved position */
        WinSendMsg(hwnd, msg, (MPARAM)SESSIONBAR_RESIZE, NULL);
                                        /* Initialize current active window */
        WinPostMsg(pHP->hwndSessionClient, WM_DESKTOPMOVE, NULL, NULL);
                                        /* Now activate the Launch Button */
        if(!(pHP->ulRuntimeFlag & POPUPMENULOADING))
            WinPostMsg(pHP->hwndSessionClient, WM_DRAWLAUNCHBUTTON,
                NULL, MPFROMLONG(WM_ENABLE));
        break;
        }

    case SESSIONBAR_RESIZE:
                                        /* Position SessionBar according to last saved position */
        pHP->swpPC2Session.fl=SWP_SIZE|SWP_MOVE|SWP_ZORDER|SWP_DEACTIVATE;
        pHP->swpPC2Session.cx=pHP->DesktopSize.x;
        pHP->swpPC2Session.cy=(pHP->lIconSize>>pHP->ulSessionBarScale)+
            ((SESSIONBAR_SPACE+SESSIONBAR_BORDER+SESSIONBAR_ICONFRAME)<<1);
                                        /* Add the scaling factor (0 or 1 for half scaled SessionBar), as
                                           the division rest got lost */
        pHP->swpPC2Session.cy-=pHP->ulSessionBarScale;
        if(pHP->swpPC2Session.x!=0)
            pHP->swpPC2Session.x=0;
        if(pHP->ulSessionBarFlag & SB_POSITIONBOTTOM)
            pHP->swpPC2Session.y=(pHP->ulStatusFlag2 & DYNAMICSESSIONBAR ? -pHP->swpPC2Session.cy : 0);
        if(pHP->ulSessionBarFlag & SB_POSITIONTOP)
            pHP->swpPC2Session.y=(pHP->ulStatusFlag2 & DYNAMICSESSIONBAR ? pHP->swpScreen.cy+1 : pHP->swpScreen.cy-pHP->swpPC2Session.cy);
        pHP->swpPC2Session.hwndInsertBehind=HWND_TOP;
        pHP->swpPC2Session.hwnd=pHP->hwndSessionFrame;
                                        /* Calculate Dynamic SessionBar settings */
        pHP->ulSessionBarFlag&=~(SB_LASTTOP|SB_LASTBOTTOM);
        pHP->ulSessionBarFlag|=(pHP->swpPC2Session.y>(pHP->swpScreen.cy>>1) ? SB_LASTTOP : SB_LASTBOTTOM);
                                        /* Make the SessionBar visible when required */
        WinPostMsg(hwnd, msg, (MPARAM)SESSIONBAR_SHOW, NULL);
        WinSetMultWindowPos(pHP->habPc2, &pHP->swpPC2Session, 1);
        break;

    case SESSIONBAR_SHOW:
        if(pHP->ulStatusFlag2 & DYNAMICSESSIONBAR)
            WinShowWindow(pHP->hwndSessionFrame, FALSE);
        else
            WinShowWindow(pHP->hwndSessionFrame, TRUE);
        break;


    case SESSIONBAR_DESTROY:
        WinDestroyWindow(pHP->hwndSessionFrame);
        GpiDestroyPS(pHP->hpsSession);
        GpiDestroyPS(pHP->hdcSession);
        pHP->hwndSessionFrame=pHP->hwndSessionClient=NULLHANDLE;
        break;
    }
    break;

/*                                                                                      *\
 * Syntax: WM_NETACCESS, UCHAR pucAutomaticUpdate, ULONG ulTag                          *
\*                                                                                      */
case WM_NETACCESS:
/*                                                                                      *\
 * This message is posted by the Network thread if some Automatic Update information    *
 * has been found worth informing the user. The passed argument is allocated            *
 * dynamically and has to be freed here at the receiver therefore.                      *
\*                                                                                      */
    {
    static  ULONG   ulMessageBoxCount=0;
    UCHAR   *pucAutomaticUpdate=MPFROMP(mp1);
    ULONG   ulHelpPanel;

    if(pucAutomaticUpdate!=NULL)
        {
        if(LONGFROMMP(mp2)==AUTOUPDATE_STATUS)
            ulHelpPanel=HELP_PC2STATUS;
        else if(LONGFROMMP(mp2)==AUTOUPDATE_NEWS)
            ulHelpPanel=HELP_PC2NEWS;
        else if(LONGFROMMP(mp2)==AUTOUPDATE_UPDATE)
            ulHelpPanel=HELP_PC2UPDATE;
                                        /* Prevent nesting of message boxes, because
                                           due to the automatic retries we may soon run
                                           out of ressources */
        if(ulMessageBoxCount<=4)
            {
            ulMessageBoxCount++;
            USR_ERR(pHP->hwndFrame, ulHelpPanel,
                MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                pucAutomaticUpdate);
            ulMessageBoxCount--;
            }
        free(pucAutomaticUpdate);
        }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_STATUS, ULONG ulCreateWindow, PFN pfnStatusWindowProc                     *
\*                                                                                      */
case WM_NETSTATUS:
/*                                                                                      *\
 * This message is posted by the Network thread to create or destroy the Status window. *
 * The Status window will then be used by the Networking thread to show the Internet    *
 * connection activity when trying to access the Automatic Update webpage.              *
\*                                                                                      */
    {
                                        /* Dynamic loaded entrypoint to PC2Net.dll */
    static PFFUNCPTR6
                *pStatusWindowProc=NULL;
    ULONG       ulStatusFCF=FCF_TITLEBAR|FCF_HIDEBUTTON|FCF_BORDER;
    POINTL      pointlStatus;

    pStatusWindowProc=(PFFUNCPTR6 *)mp2;
                                        /* Depending on the argument mp1 either create (after
                                           loading the window procedure) or destroy the Status
                                           window */
    if(LONGFROMMP(mp1)==TRUE)
        {
        if(WinRegisterClass(pHP->habPc2, (PSZ)PC2_CLASSNAME_STATUS, (PFNWP)pStatusWindowProc,
            CS_SIZEREDRAW | CS_SAVEBITS | CS_MOVENOTIFY, 0))
            {
            pHP->hwndStatusFrame=WinCreateStdWindow(HWND_DESKTOP, 0, &ulStatusFCF,
                (PSZ)PC2_CLASSNAME_STATUS, "PC/2 Status", 0, (HMODULE)0, ID_PC2STATUSWINDOW,
                &pHP->hwndStatusClient);
            }
        if(pHP->hwndStatusFrame==NULLHANDLE)
            {
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_QUICKSWITCH, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Creation of Status window failed - continuing...");
            break;
            }
                                        /* Map height of 2 lines static text from dialog to screen
                                           coordinates */
        pointlStatus.x=pointlStatus.y=8*2;
        WinMapDlgPoints(HWND_DESKTOP, &pointlStatus, 1, TRUE);
                                        /* Size Status window to have titlebar and place for a 2-lines
                                           static text client area */
        pHP->swpPC2Status.fl=SWP_MOVE|SWP_SIZE|SWP_DEACTIVATE|SWP_HIDE;
        if(pHP->swpPC2Status.cx==0)
        pHP->swpPC2Status.cx=pHP->swpScreen.cx>>2;
        pHP->swpPC2Status.cy=WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR)+
            (WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER)<<1)+pointlStatus.y;
        if(pHP->swpPC2Status.x==0)
            pHP->swpPC2Status.x=pHP->swpScreen.cx-pHP->swpPC2Status.cx;
        if(pHP->swpPC2Status.y==0)
            pHP->swpPC2Status.y=pHP->swpScreen.cy-pHP->swpPC2Status.cy;
        pHP->swpPC2Status.hwndInsertBehind=NULLHANDLE;
        pHP->swpPC2Status.hwnd=pHP->hwndStatusFrame;
        WinSetMultWindowPos(pHP->habPc2, &pHP->swpPC2Status, 1);
        }
    else
        WinDestroyWindow(pHP->hwndStatusFrame);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_PC2STARTUPSCREEN, NULL, NULL                                              *
\*                                                                                      */
case WM_PC2STARTUPSCREEN:
/*                                                                                      *\
 * Now get device resolution and save it and other initilization data to the structure  *
 * used to communicate with PC2HOOK.DLL. We start be taking the values from the profile *
 * unless the Virtual Desktop is disabled at all (but still having configured x by y    *
 * Virtual Desktops).                                                                   *
\*                                                                                      */
    {
    POINTL  DesktopCountMin;
    POINTL  DesktopCountMax;

#ifdef  DEBUG_PC2
    printf("PC2: WM_PC2STARTUPSCREEN\n");
#endif  /* DEBUG_PC2 */
                                        /* See what count we are using (compared to what
                                           was configured) */
    if(pHP->ulStatusFlag1 & VIRTUALDESKTOP)
        {
        pHP->ulHorizontalDesktops=pHP->ulHorizontalDesktopsSave;
        pHP->ulVerticalDesktops=pHP->ulVerticalDesktopsSave;
        }
    else
        {
        pHP->ulHorizontalDesktops=1;
        pHP->ulVerticalDesktops=1;
        }
                                        /* Query the icon size, because when the Popup-Menu is
                                           ownerdraw, the menuentry's icon is this size or half
                                           of it (32*32 for VGA, 40*40 for SVGA) */
    pHP->lIconSize=WinQuerySysValue(HWND_DESKTOP, SV_CXICON);
                                        /* Query and save the device resolution, f.e.
                                           1024 * 768 */
    pHP->DesktopSize.x=pHP->swpScreen.cx=WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
    pHP->DesktopSize.y=pHP->swpScreen.cy=WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
                                        /* Get number of horizontal and vertical Virtual Desktops */
    DesktopCountMin.x=(pHP->ulHorizontalDesktops-1)>>1;
    DesktopCountMin.y=(pHP->ulVerticalDesktops-1)>>1;
    DesktopCountMax.x=pHP->ulHorizontalDesktops-DesktopCountMin.x;
    DesktopCountMax.y=pHP->ulVerticalDesktops-DesktopCountMin.y;
                                        /* Now initialize the virtual Desktop data as a percentage
                                           of the current resolution */
    pHP->LLHotBorder.x=(pHP->DesktopSize.x*VIRTUALDESKTOP_HOTSPOT)/100;
    pHP->LLHotBorder.y=(pHP->DesktopSize.y*VIRTUALDESKTOP_HOTSPOT)/100;
    pHP->URHotBorder.x=(pHP->DesktopSize.x*(100-VIRTUALDESKTOP_HOTSPOT))/100;
    pHP->URHotBorder.y=(pHP->DesktopSize.y*(100-VIRTUALDESKTOP_HOTSPOT))/100;
                                        /* Don't set the initial middle screen position, because
                                           when the Desktop dialog is opened on any Virtual Desktop
                                           this Virtual Desktop would become the middle screen
                                           position then
                                           pHP->VirtualDesktopPos.x=0;
                                           pHP->VirtualDesktopPos.y=0;
                                         */
                                        /* Calculate absolute lower left and upper right
                                           corners of the Virtual Desktop */
    pHP->VirtualDesktopMin.x=(-pHP->DesktopSize.x)*DesktopCountMin.x;
    pHP->VirtualDesktopMin.y=(-pHP->DesktopSize.y)*DesktopCountMin.y;
    pHP->VirtualDesktopMax.x=pHP->DesktopSize.x*DesktopCountMax.x;
    pHP->VirtualDesktopMax.y=pHP->DesktopSize.y*DesktopCountMax.y;
    pHP->SlidingXFactor=(pHP->ulScrollPercentage*
        pHP->DesktopSize.x)/100;
    pHP->SlidingYFactor=(pHP->ulScrollPercentage*
        pHP->DesktopSize.y)/100;
    if(pHP->hwndWorkingThread!=NULLHANDLE)
        WinPostMsg(pHP->hwndWorkingThread, WM_PAINT, NULL, NULL);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_PC2STARTUPMEMORYDC, NULL, NULL                                            *
\*                                                                                      */
case WM_PC2STARTUPMEMORYDC:
/*                                                                                      *\
 * Now setup a memory device context and associate a presentation space into. Into this *
 * presentation space we draw the Overview window and blit it into the client area      *
 * presenation space after all drawing has finished. This avoids the flickering in the  *
 * Overview window caused that we draw all windows from the bottom to the top of the    *
 * Z-order which causes windows to be drawn and overlayed immediately afterwards by     *
 * another window. Same applies for the SessionBar.                                     *
\*                                                                                      */
    {
    SIZEL   sizelClient;

#ifdef  DEBUG_PC2
    printf("PC2: WM_PC2STARTUPMEMORYDC\n");
#endif  /* DEBUG_PC2 */
                                            /* I'm not sure, why we can create a zero sized
                                               presentation space, but draw any sized graphics
                                               into, possibly because we associate a bitmap
                                               to the presentation space, which is sized
                                               according to the client area size. */
    sizelClient.cx=0;
    sizelClient.cy=0;
                                            /* Get memory device context */
    hdcOverviewMemory=DevOpenDC(pHP->habPc2, OD_MEMORY, "*", 0L, NULL, 0L);
    hdcSessionBarMemory=DevOpenDC(pHP->habPc2, OD_MEMORY, "*", 0L, NULL, 0L);
                                            /* Create a presentation space into the memory device context */
    pHP->hpsOverviewMemory=GpiCreatePS(pHP->habPc2, hdcOverviewMemory, &sizelClient, PU_PELS|GPIA_ASSOC|GPIT_MICRO);
    pHP->hpsSessionBarMemory=GpiCreatePS(pHP->habPc2, hdcSessionBarMemory, &sizelClient, PU_PELS|GPIA_ASSOC|GPIT_MICRO);
                                            /* Change color table into RGB mode */
    GpiCreateLogColorTable(pHP->hpsOverviewMemory, 0L, LCOLF_RGB, 0L, 0L, NULL);
    GpiCreateLogColorTable(pHP->hpsSessionBarMemory, 0L, LCOLF_RGB, 0L, 0L, NULL);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_PC2STARTUPTHREAD, NULL, NULL                                              *
\*                                                                                      */
case WM_PC2STARTUPTHREAD:
/*                                                                                      *\
 * Now that frame and client windows are created (of course invisible), start working   *
 * thread that opens a presentation space on the client area and performs all timely    *
 * intensive tasks. After starting a thread, cause to preempt ourselves because         *
 * otherwise startup sometimes does not work insofar that thread 1 seems just not to    *
 * resume execution. That should not happen, but it does and DosSleep() helps...        *
\*                                                                                      */
#ifdef  DEBUG_PC2
    printf("PC2: WM_PC2STARTUPTHREAD\n");
#endif  /* DEBUG_PC2 */
                                        /* Start IO working thread */
    pthreaddataIOThread=(THREADDATA *)calloc(sizeof(*pthreaddataIOThread), sizeof(BYTE));
    pthreaddataIOThread->usCb=sizeof(*pthreaddataIOThread);
    pthreaddataIOThread->ulThreadId=ID_IOTHREAD;
    pthreaddataIOThread->pfnwpThread=PC2_IOThreadWindowProc;
    pthreaddataIOThread->tidThread=_beginthread(PC2_Thread, NULL, 65536, pthreaddataIOThread);
    pHP->ulThreadIDITrd=(ULONG)pthreaddataIOThread->tidThread;
    DosSleep(50);
    if(pthreaddataIOThread->tidThread==(TID)-1)
        {
        USR_ERR(pHP->hwndFrame, HELP_PC2THREAD, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't start its I/O thread - exiting...");
                                        /* Post exit message */
        pHP->ulRuntimeFlag|=PC2EMERGENCYEXIT;
        WinPostMsg(pHP->hwndFrame, WM_QUIT, NULL, NULL);
        }
#ifdef  DEBUG
    printf("PC2: Started ITRD\n");
#endif  /* DEBUG */
                                        /* Start network thread */
    pthreaddataNetThread=(THREADDATA *)calloc(sizeof(*pthreaddataNetThread), sizeof(BYTE));
    pthreaddataNetThread->usCb=sizeof(*pthreaddataNetThread);
    pthreaddataNetThread->ulThreadId=ID_NETTHREAD;
    pthreaddataNetThread->pfnwpThread=PC2_NetThreadWindowProc;
    pthreaddataNetThread->tidThread=_beginthread(PC2_Thread, NULL, 65536, pthreaddataNetThread);
    pHP->ulThreadIDNTrd=(ULONG)pthreaddataNetThread->tidThread;
    DosSleep(50);
    if(pthreaddataNetThread->tidThread==(TID)-1)
        {
        USR_ERR(pHP->hwndFrame, HELP_PC2THREAD, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't start its Network thread - exiting...");
                                        /* Post exit message */
        pHP->ulRuntimeFlag|=PC2EMERGENCYEXIT;
        WinPostMsg(pHP->hwndFrame, WM_QUIT, NULL, NULL);
        }
#ifdef  DEBUG
    printf("PC2: Started NTRD\n");
#endif  /* DEBUG */
                                        /* Start working thread */
    pthreaddataWorkingThread=(THREADDATA *)calloc(sizeof(*pthreaddataWorkingThread), sizeof(BYTE));
    pthreaddataWorkingThread->usCb=sizeof(*pthreaddataWorkingThread);
    pthreaddataWorkingThread->ulThreadId=ID_WORKINGTHREAD;
    pthreaddataWorkingThread->pfnwpThread=PC2_WorkThreadWindowProc;
    pthreaddataWorkingThread->tidThread=_beginthread(PC2_Thread, NULL, 65536, pthreaddataWorkingThread);
    pHP->ulThreadIDWTrd=(ULONG)pthreaddataWorkingThread->tidThread;
    DosSleep(50);
    if(pthreaddataWorkingThread->tidThread==(TID)-1)
        {
        USR_ERR(pHP->hwndFrame, HELP_PC2THREAD, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 can't start its working thread - exiting...");
                                        /* Post exit message */
        pHP->ulRuntimeFlag|=PC2EMERGENCYEXIT;
        WinPostMsg(pHP->hwndFrame, WM_QUIT, NULL, NULL);
        }
#ifdef  DEBUG
    printf("PC2: Started WTRD\n");
#endif  /* DEBUG */
    break;

/*                                                                                      *\
 * Syntax: WM_OVERVIEWFCF, ULONG ulChangedFCF, ULONG bChangeEnabled                     *
\*                                                                                      */
case WM_OVERVIEWFCF:
/*                                                                                      *\
 * This message is posted by Desktop configuration dialog to change the titlebar and    *
 * menubar of PC/2 according to the user selection. The flag bChangeEnabled is used to  *
 * control if the after loading menuentries should be enabled from disabled state.      *
\*                                                                                      */
    {
    ULONG   ulChangedFCF;

#ifdef  DEBUG_PC2
    printf("PC2: WM_OVERVIEWFCF\n");
#endif  /* DEBUG_PC2 */
    ulChangedFCF=(ULONG)LONGFROMMP(mp1);
                                        /* Update titlebar + hide button only if changed */
    if(ulChangedFCF & (FCF_TITLEBAR|FCF_HIDEBUTTON))
        {
        if(pHP->ulOverviewFCF & (FCF_TITLEBAR|FCF_HIDEBUTTON))
            {                           /* If titlebar and hide button are no longer requested,
                                           destroy them */
            if(pHP->ulDebug>=DEBUG_FULL)
                printf("PC2: WM_OVERVIEWFCF destroying titlebar\n");
            WinDestroyWindow(WinWindowFromID(pHP->hwndFrame, FID_TITLEBAR));
            WinDestroyWindow(WinWindowFromID(pHP->hwndFrame, FID_MINMAX));
            }
        else
            {
            FRAMECDATA  framecdataOverview;

            if(pHP->ulDebug>=DEBUG_FULL)
                printf("PC2: WM_OVERVIEWFCF creating titlebar\n");
                                        /* Setup frame control data to create a titlebar and a
                                           hide button */
            framecdataOverview.cb=sizeof(FRAMECDATA);
            framecdataOverview.flCreateFlags=FCF_TITLEBAR|FCF_HIDEBUTTON;
            framecdataOverview.hmodResources=NULLHANDLE;
            framecdataOverview.idResources=ID_PC2MAINWINDOW;
            WinCreateFrameControls(pHP->hwndFrame, &framecdataOverview, NULL);
            }
                                        /* Toggle flag */
        pHP->ulOverviewFCF^=(FCF_TITLEBAR|FCF_HIDEBUTTON);
        }
                                        /* Update Smart Icon menubar only if changed */
    if(ulChangedFCF & FCF_MENU)
        {
        HDC         hdcMenu;
        HPS         hpsMenu;
        SIZEL       sizelMenu={0, 0};
        ULONG       ulMenuItem;
        MENUITEM    menuitemCurrent;

        if(pHP->ulOverviewFCF & FCF_MENU)
            {                           /* If menubar is no longer requested, destroy it */
            if(pHP->ulDebug>=DEBUG_FULL)
                printf("PC2: WM_OVERVIEWFCF destroying smarticonbar\n");
            WinSetParent(pHP->hwndMenu, HWND_OBJECT, FALSE);
            for(ulMenuItem=ID_ICONEXIT; ulMenuItem<=ID_ICONHELP; ulMenuItem++)
                {
                WinSendMsg(pHP->hwndMenu, MM_QUERYITEM,
                    MPFROM2SHORT(ulMenuItem, TRUE), MPFROMP(&menuitemCurrent));
                GpiDeleteBitmap((HBITMAP)menuitemCurrent.hItem);
                }
            WinDestroyWindow(pHP->hwndMenu);
            }
        else
            {
            ULONG   ulShiftMask;

            if(pHP->ulDebug>=DEBUG_FULL)
                printf("PC2: WM_OVERVIEWFCF creating smarticonbar\n");
            pHP->hwndMenu=WinLoadMenu(pHP->hwndFrame, NULLHANDLE, ID_PC2MAINWINDOW);
                                        /* Subclass smart icon menu bar to catch changes of
                                           presentation parameters */
            pfnMenuWindowProc=WinSubclassWindow(pHP->hwndMenu, SubclassedMenuWindowProc);
                                        /* Replace the text style (MIS_TEXT) menubar by smarticons,
                                           by changing the style (to MIS_BITMAP) and setting the
                                           smarticons bitmap into the menuitem */
            hdcMenu=WinOpenWindowDC(pHP->hwndMenu);
            hpsMenu=GpiCreatePS(pHP->habPc2, hdcMenu,
                &sizelMenu, PU_PELS|GPIA_ASSOC|GPIT_MICRO);
            for(ulMenuItem=ID_ICONEXIT, ulShiftMask=0x00000001;
                ulMenuItem<=ID_ICONHELP;
                ulMenuItem++, ulShiftMask<<=1)
                {
                if(pHP->ulOVSmartIconsFlag & ulShiftMask)
                    {
                    WinSendMsg(pHP->hwndMenu, MM_QUERYITEM,
                        MPFROM2SHORT(ulMenuItem, TRUE), MPFROMP(&menuitemCurrent));
                    menuitemCurrent.hItem=(HBITMAP)GpiLoadBitmap(hpsMenu,
                        hDLLPc2Resource, ulMenuItem, 0, 0);
                    menuitemCurrent.afStyle=MIS_BITMAP;
                    WinSendMsg(pHP->hwndMenu, MM_SETITEM,
                        MPFROM2SHORT(NULL, TRUE), MPFROMP(&menuitemCurrent));
                    }
                else
                    {
                    WinSendMsg(pHP->hwndMenu, MM_DELETEITEM,
                        MPFROM2SHORT((USHORT)ulMenuItem, TRUE), NULL);
                    }
                }
            GpiDestroyPS(hpsMenu);
            if((LONGFROMMP(mp2)==TRUE) && !(pHP->ulRuntimeFlag & (PC2RUNNINGASWPS|PC2RUNNINGASPROTSHELL)))
                                        /* Enable menuitem */
                WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                    MPFROM2SHORT(ID_ICONEXIT, TRUE), MPFROM2SHORT(MIA_DISABLED, 0));
            }
                                        /* Toggle flag */
        pHP->ulOverviewFCF^=FCF_MENU;
        }
                                        /* Force a redraw */
    if(ulChangedFCF)
        {
        WinSendMsg(pHP->hwndFrame, WM_UPDATEFRAME, MPFROMLONG(ulChangedFCF), NULL);
        WinSendMsg(pHP->hwndClient, WM_COMMAND, MPFROMSHORT(ID_DEBUG), MPFROM2SHORT(CMDSRC_OTHER, FALSE));
        }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_THREADSTARTUPREADY, ULONG ulThreadId, ULONG ulError                       *
\*                                                                                      */
case WM_THREADSTARTUPREADY:
/*                                                                                      *\
 * Working thread is now ready, make Overview window visible when requested.            *
\*                                                                                      */
#ifdef  DEBUG
    if(LONGFROMMP(mp1)==ID_IOTHREAD)
        printf("PC2: WM_THREADSTARTUPREADY (%s)\n", "ITrd");
    else if(LONGFROMMP(mp1)==ID_NETTHREAD)
        printf("PC2: WM_THREADSTARTUPREADY (%s)\n", "NTrd");
    else if(LONGFROMMP(mp1)==ID_WORKINGTHREAD)
        printf("PC2: WM_THREADSTARTUPREADY (%s)\n", "WTrd");
    else
        printf("PC2: WM_THREADSTARTUPREADY (%s)\n", "Unknown - Error!");
#endif  /* DEBUG */
    if(LONGFROMMP(mp1)==ID_IOTHREAD)
        {
                                        /* Now load the environment profile (Environment Spaces). As
                                           these may be used by the menuentries of the Popup-Menu, load
                                           them first */
        WinPostMsg(pHP->hwndClient, WM_LOADENVIRONMENTSPACE, MPFROMLONG(ID_MAINTHREAD), NULL);
                                        /* Now load the Popup-Menu from the profile */
        WinPostMsg(pHP->hwndClient, WM_LOADPOPUPMENU, MPFROMLONG(ID_MAINTHREAD), NULL);
                                        /* Now install the hook */
        WinPostMsg(pHP->hwndClient, WM_LOADHOOK, NULL, NULL);
                                        /* Initialize the Titlebar Smarticons when required */
        if(pHP->ulStatusFlag2 & TITLEBARICONS)
            WinPostMsg(pHP->hwndClient, WM_TITLEBARICONS, (MPARAM)TRUE, NULL);
        }
    if(LONGFROMMP(mp1)==ID_NETTHREAD)
        {
        if(LONGFROMMP(mp2)!=NO_ERROR)
            {
            UCHAR   *pucMessage;

            pucMessage=malloc(1024);
            sprintf(pucMessage, "The signature of the PC/2 DLL \"%s\" does not match with this build. "
                "Please see the online help to verify PC/2 is installed correctly!\n\n", "PC2NET.DLL");
            strcat(pucMessage, "PC2NET.DLL services (Online Automatic Update) will not be available, "
                "please ensure that TCP/IP is installed correctly  - continuing...");
            if(LONGFROMMP(mp2)==ERROR_FILE_NOT_FOUND)
                DOS_ERR(LONGFROMMP(mp2), pHP->hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1, pucMessage);
            else
                USR_ERR(pHP->hwndFrame, HELP_DLLLOADED, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1, pucMessage);
            free(pucMessage);
            }
        }
    if(LONGFROMMP(mp1)==ID_WORKINGTHREAD)
        {
                                        /* Create the SessionBar when required */
        if(pHP->ulStatusFlag2 & SHOWSESSIONBAR)
            WinPostMsg(pHP->hwndClient, WM_SESSIONBARUPDATE, MPFROMLONG(SESSIONBAR_CREATE), NULL);
                                        /* Set Overview window font */
        WinSetPresParam(pHP->hwndClient, PP_FONTNAMESIZE,
            sizeof(pHP->ucPC2WindowFont), pHP->ucPC2WindowFont);
                                        /* Display Overview window if requested */
        if(pHP->ulStatusFlag1 & OVERVIEW)
            {
            SWP     swp;

            swp.fl=SWP_SHOW|SWP_DEACTIVATE|SWP_ZORDER;
            swp.x=swp.y=swp.cx=swp.cy=0;
            swp.hwndInsertBehind=HWND_TOP;
            swp.hwnd=pHP->hwndFrame;
            WinSetMultWindowPos(pHP->habPc2, &swp, 1);
            WinInvalidateRect(pHP->hwndClient, NULL, FALSE);
            }
                                        /* Create a timer to update the spooler container regularily,
                                           and to advance Lockup timer */
        for(ulTimer=0; ulTimer<=TID_USERMAX; ulTimer++)
            if(WinStartTimer(pHP->habPc2, hwnd, ulTimer, TIMERINTERVALLMS))
                {
                if(pHP->ulDebug>=DEBUG_LOW)
                    printf("PC2: TimerID aquired %d\n", (int)ulTimer);
                break;
                }
        if(ulTimer>TID_USERMAX)
            USR_ERR(pHP->hwndFrame, HELP_PC2TIMER, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Configuration of a timer failed - continuing...");
                                        /* Check if the registration notebook page is filled
                                           in. If not request the Desktop dialog on the
                                           Registration page to be displayed */
        if((pHP->ucUserFirstName[0]=='\0') || (pHP->ucUserLastName[0]=='\0') || (pHP->ucUserEmailAddress[0]=='\0'))
            WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_DESKTOPDIALOG), MPFROM2SHORT(CMDSRC_ACCELERATOR, DD_PAGE_9));
        }
    break;

/*                                                                                      *\
 * Syntax: WM_LOADPOPUPMENU, ULONG ulThreadId, MPARAM mp2                               *
\*                                                                                      */
case WM_LOADPOPUPMENU:
/*                                                                                      *\
 * Open the profile for reading the linked list containing the popup menu data. If the  *
 * profile can't be opened, the file is assumed to be empty so the popup menu is empty. *
 * Loading the Popup Menu may take quite a while when the user requested to include the *
 * menuentries icons in the Popup Menu.                                                 *
\*                                                                                      */
#ifdef  DEBUG_PC2
    printf("PC2: WM_LOADPOPUPMENU (%s)\n", (LONGFROMMP(mp1)==ID_MAINTHREAD ? "PC2" : "ITrd"));
#endif  /* DEBUG_PC2 */
    if(LONGFROMMP(mp1)==ID_MAINTHREAD)
        {
        ULONG   ulSpoolerError;

                                        /* Tell I/O thread to load Popup-Menu */
        WinPostMsg(pHP->hwndIOThread, WM_LOADPOPUPMENU, MPFROMP(pHP->pPopupMenu), MPFROMP(pHP->ucFilenameProfile));
                                        /* If PC/2 is the WPS process or user requested
                                           to have the spooler window always, initialize spooler */
        if(pSpoolerInitialize==0)
            pHP->ulRuntimeFlag|=PC2SPOOLERERROR;
        if(((pHP->ulRuntimeFlag & PC2RUNNINGASWPS) || (pHP->ulStatusFlag1 & SHOWSPOOLERWINDOW)) &&
            !(pHP->ulRuntimeFlag & PC2SPOOLERERROR))
                                        /* Initialize the spooler */
            if((ulSpoolerError=pSpoolerInitialize(pHP->hDLLPc2Hook))!=NO_ERROR)
                {
                UCHAR   ucSpoolerError[256];

                sprintf(ucSpoolerError, "Initialization of the OS/2 Spooler failed. Because the "\
                    "Spooler PMSPL.DLL is part of any OS/2 installation, your installation may be"\
                    "defective. The error code is %04X. - Continuing without PC/2 Spooler Control "\
                    "Window...", (int)ulSpoolerError);
                USR_ERR(pHP->hwndFrame, HELP_PC2SPOOLDLL, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    ucSpoolerError);
                pHP->ulRuntimeFlag|=PC2SPOOLERERROR;
                                        /* Release spooler DLL */
                DosFreeModule(hDLLPc2Spooler);
                hDLLPc2Spooler=NULLHANDLE;
                }
        }
    if(LONGFROMMP(mp1)==ID_IOTHREAD)
        {
#ifdef  DEBUG_PC2
    printf("PC2: WM_LOADPOPUPMENU - Loading menu into PM\n");
#endif  /* DEBUG_PC2 */
                                        /* Now load the loaded menu (MENUDATA structure) into
                                           PM's menu (MENUITEM structure). Depending on the menu
                                           size it may take a while, but I'm not sure if we can do
                                           this from a different thread (as far as I know there is
                                           a relation between a window and the thread it was created
                                           in) */
                                        /* I'm not sure if that doesn't cause problems, when the I/O
                                           thread loads the PM menu structures as we have created the
                                           menu in this thread. Up to now it greatly reduced the SIQ
                                           blocking for large Popup Menus without a known problem */
        SetPMMenu(pHP->hwndPopupMenu, pHP->pPopupMenu);
        WinSetOwner(pHP->hwndPopupMenu, pHP->hwndClient);
        pHP->ulRuntimeFlag&=(~POPUPMENULOADING);
                                        /* Now activate the Exit smarticon and Launch Button */
        WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
            MPFROM2SHORT(ID_ICONEXIT, TRUE), MPFROM2SHORT(MIA_DISABLED, 0));
        WinPostMsg(pHP->hwndSessionClient, WM_DRAWLAUNCHBUTTON,
            (MPARAM)pHP->hpsSession, MPFROMLONG(WM_ENABLE));
#ifdef  DEBUG_PC2
    printf("PC2: WM_LOADPOPUPMENU - Finished loading menu into PM\n");
#endif  /* DEBUG_PC2 */
                                        /* If the I/O thread had a problem reading the Popup-Menu
                                           display the error message */
        if(mp2!=NULL)
            USR_ERR(pHP->hwndFrame, HELP_PC2CFG, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                PVOIDFROMMP(mp2));
                                        /* Initialize *MENUDATA for Configuration dialog
                                           procedure to a known value */
        pHP->pMenuData=pHP->pPopupMenu;
                                        /* If the user wants the Popup-Menu to contain icons, set the
                                           MIS_OWNERDRAW style into PM's MENUITEMs and request the icons
                                           to be loaded by the I/O thread afterwards */
        if(pHP->ulStatusFlag2 & POPUPICON)
            {
            pHP->ulRuntimeFlag|=POPUPMENUUPDATE;
            WinPostMsg(pHP->hwndIOThread, WM_POPUPMENUUPDATE,
                MPFROMP(pHP->pPopupMenu), MPFROMSHORT(TRUE));
            }
                                        /* If PC/2 is not the WPS process and Spooler is not enabled,
                                           or available, disable spooler menu item in Popup Menu */
        if(((!(pHP->ulRuntimeFlag & PC2RUNNINGASWPS)) && (!(pHP->ulStatusFlag1 & SHOWSPOOLERWINDOW))) ||
            (pHP->ulRuntimeFlag & PC2SPOOLERERROR))
            WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_SPOOLER, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
                                        /* If PC/2 is running as the WPS or PROTSHELL, then disable the
                                           Exit smarticon, and the Exit OS/2 menuentry */
        if(pHP->ulRuntimeFlag & (PC2RUNNINGASWPS|PC2RUNNINGASPROTSHELL))
            {
            WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_EXIT, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_ICONEXIT, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
            }
                                        /* If APM is disabled, then disable the PowerDown
                                           smarticons, and the PowerDown OS/2 menuentry, else
                                           enable them */
        WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
            MPFROM2SHORT(ID_POWERDOWN, TRUE), MPFROM2SHORT(MIA_DISABLED, (pHP->ulStatusFlag2 & SUPPORTAPM ? 0 : MIA_DISABLED)));
        WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
            MPFROM2SHORT(ID_ICONPOWERDOWN, TRUE), MPFROM2SHORT(MIA_DISABLED, (pHP->ulStatusFlag2 & SUPPORTAPM ? 0 : MIA_DISABLED)));
                                        /* Tell I/O thread to scan PC2.TRP for new exceptions logged */
        WinPostMsg(pHP->hwndIOThread, WM_ANALYZEPC2TRAP, NULL, NULL);
        }
    break;

/*                                                                                      *\
 * Syntax: WM_SAVEPOPUPMENU, ULONG ulThreadId, MPARAM mp2                               *
\*                                                                                      */
case WM_SAVEPOPUPMENU:
/*                                                                                      *\
 * Process the request to save the current Popup-Menu structure to the configuration    *
 * file. Pass the request to the I/O thread.                                            *
\*                                                                                      */
#ifdef  DEBUG_PC2
    printf("PC2: WM_SAVEPOPUPMENU (%s)\n", (LONGFROMMP(mp1)==ID_MAINTHREAD ? "PC2" : "ITrd"));
#endif  /* DEBUG_PC2 */
    if(LONGFROMMP(mp1)==ID_MAINTHREAD)
                                        /* Tell I/O thread to save Popup-Menu */
        WinPostMsg(pHP->hwndIOThread, WM_SAVEPOPUPMENU, MPFROMP(pHP->pPopupMenu), MPFROMP(pHP->ucFilenameProfile));
    if(LONGFROMMP(mp1)==ID_IOTHREAD)
        if(PVOIDFROMMP(mp2)!=NULL)
            USR_ERR(pHP->hwndFrame, HELP_PC2CFG, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                PVOIDFROMMP(mp2));
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
    if(LONGFROMMP(mp1)==ID_MAINTHREAD)
        WinPostMsg(pHP->hwndIOThread, WM_LOADENVIRONMENTSPACE,
            MPFROMLONG(ID_MAINTHREAD), MPFROMP(pHP->ucFilenameEnvironment));
    if(LONGFROMMP(mp1)==ID_IOTHREAD)
        if(PVOIDFROMMP(mp2)!=NULL)
            USR_ERR(pHP->hwndFrame, HELP_PC2ENV, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                PVOIDFROMMP(mp2));
    break;

/*                                                                                      *\
 * Syntax: WM_SAVEENVIRONMENTSPACE, ULONG ulThreadId, MPARAM mp2                        *
\*                                                                                      */
case WM_SAVEENVIRONMENTSPACE:
/*                                                                                      *\
 * Open the profile for writing the linked list containing the environment spaces       *
 * defined. If the profile can't be opened, changes can't be saved and may be lost.     *
\*                                                                                      */
    if(LONGFROMMP(mp1)==ID_MAINTHREAD)
        WinPostMsg(pHP->hwndIOThread, WM_SAVEENVIRONMENTSPACE,
            MPFROMLONG(ID_MAINTHREAD), MPFROMP(pHP->ucFilenameEnvironment));
    if(LONGFROMMP(mp1)==ID_IOTHREAD)
        if(PVOIDFROMMP(mp2)!=NULL)
            USR_ERR(pHP->hwndFrame, HELP_PC2ENV, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                PVOIDFROMMP(mp2));
    break;

/*                                                                                      *\
 * Syntax: WM_LOADHOOK, NULL, NULL                                                      *
\*                                                                                      */
case WM_LOADHOOK:
/*                                                                                      *\
 * Install the hook into the system input queue pointing to the PC2DLL_Hook() procedure *
 * in the DLL PC2HOOK.DLL. If we can't do this we exit after an error message box.      *
\*                                                                                      */
    {
    ULONG   ulError=FALSE;

    if(WinSetHook(                      /* Set a hook */
        pHP->habPc2,                    /* Handle of anchor block */
        NULLHANDLE,                     /* Hook into system message queue */
        HK_INPUT,                       /* Hook of system input queue */
        (PFN)pInputHook,                /* Pointer to hook procedure */
        pHP->hDLLPc2Hook)==FALSE)
        {
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_PC2HOOK, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Hooking the system input queue failed - exiting...");
        ulError=TRUE;
        }
                                        /* Hook of system input queue before the key gets its accelerator
                                           translation */
    if(WinSetHook(pHP->habPc2, NULLHANDLE, HK_PREACCEL, (PFN)pPreAccelHook,
        pHP->hDLLPc2Hook)==FALSE)
        {
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_PC2HOOK, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Hooking the system input queue before accelerator translation failed - exiting...");
        ulError=TRUE;
        }
                                        /* Hook of WinSendMsg() function when calling an
                                           application's window procedure */
    if(WinSetHook(pHP->habPc2, NULLHANDLE, HK_SENDMSG, (PFN)pWinSendMsgHook,
        pHP->hDLLPc2Hook)==FALSE)
        {
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_PC2HOOK, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Hooking the system sendmessage dispatcher failed - exiting...");
        ulError=TRUE;
        }
    if(ulError)
        {
        pHP->ulRuntimeFlag|=PC2EMERGENCYEXIT;
        WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
        }
    }
    break;

case WM_SIZE:
case WM_MOVE:
/*                                                                                      *\
 * One fundamental assumption is, that when getting here, the HOOKPARAMETERS structure  *
 * already contains the screen resolution data. Its therefore most important to call    *
 * WM_PC2STARTUPSCREEN using the synchronous WinSendMsg() in WM_PC2STARTUP, otherwise   *
 * we get a 0 sized Overview window.                                                    *
\*                                                                                      */
    {
    LONG            lXBorder=WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
    LONG            lYBorder=WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER);
    SWP             swpPC2;
    static ULONG    ulRecursion=0;

                                        /* As a WM_MOVE may cause a call to WinSetMultWindowPos() (when the
                                           user tries to move the window out off the screen), which
                                           recursively would invoke WM_MOVE again. We have to protect ourselves
                                           (even when PM doesn't seem to send WM_MOVE when moving the window
                                           to the same position the window already is!) */
    if(msg==WM_MOVE)
        {
        if(ulRecursion>0) break;
        else ulRecursion++;
        }
                                        /* Get the frame area size */
    WinQueryWindowPos(pHP->hwndFrame, &swpPC2);
    if((swpPC2.cx>pHP->DesktopSize.x) || (pHP->swpPC2.cx<=0))
        pHP->swpPC2.cx=pHP->DesktopSize.x/3;
    else
        pHP->swpPC2.cx=swpPC2.cx;
    if((swpPC2.cy>pHP->DesktopSize.y) || (pHP->swpPC2.cy<=0))
        pHP->swpPC2.cy=pHP->DesktopSize.y/4;
    else
        pHP->swpPC2.cy=swpPC2.cy;
    if((swpPC2.x<(0-lXBorder)) ||
        ((swpPC2.x+pHP->swpPC2.cx)>(pHP->DesktopSize.x+lXBorder)))
        pHP->swpPC2.x=0;
    else
        pHP->swpPC2.x=swpPC2.x;
    if((swpPC2.y<(0-lYBorder)) ||
        ((swpPC2.y+pHP->swpPC2.cy)>(pHP->DesktopSize.y+lYBorder)))
        pHP->swpPC2.y=0;
    else
        pHP->swpPC2.y=swpPC2.y;
    pHP->swpPC2.fl=SWP_SIZE|SWP_MOVE;
    pHP->swpPC2.hwndInsertBehind=NULLHANDLE;
    pHP->swpPC2.hwnd=pHP->hwndFrame;
    WinSetMultWindowPos(pHP->habPc2, &pHP->swpPC2, 1);
                                        /* Get the client area size */
    WinQueryWindowPos(pHP->hwndClient, &pHP->swpPC2Client);
                                        /* Request repaint from working thread when resized */
    if(msg==WM_SIZE)
        {

        BITMAPINFOHEADER2   bmihMemory; /* In memory Overview window bitmap info header */
                                        /* Bitmapformat of current display & driver combination */
        LONG                lBitmapFormat[2];

                                        /* Get the number of planes and the bitcount of the current
                                           display & driver combination */
        GpiQueryDeviceBitmapFormats(pHP->hpsOverviewMemory, 2L, lBitmapFormat);
                                        /* Create a bitmap into the memory presentation space of same
                                           size as client area */
        memset(&bmihMemory, 0, sizeof(BITMAPINFOHEADER2));
        bmihMemory.cbFix=sizeof(BITMAPINFOHEADER2);
        bmihMemory.cx=pHP->swpPC2Client.cx;
        bmihMemory.cy=pHP->swpPC2Client.cy;
        bmihMemory.cPlanes=lBitmapFormat[0];
        bmihMemory.cBitCount=lBitmapFormat[1];
                                        /* Delete previous bitmap if it exists, after removing it
                                           from the presentation space (or we will get a memory leak
                                           as GpiDeleteBitmap() fails because it can't delete selected
                                           bitmaps */
        if(pHP->hbmOverviewMemory)
            {
            GpiSetBitmap(pHP->hpsOverviewMemory, NULLHANDLE);
            GpiDeleteBitmap(pHP->hbmOverviewMemory);
            }
        pHP->hbmOverviewMemory=GpiCreateBitmap(pHP->hpsOverviewMemory, &bmihMemory, 0L, NULL, NULL);
        GpiSetBitmap(pHP->hpsOverviewMemory, pHP->hbmOverviewMemory);
                                        /* Now repaint Overview window */
        WinInvalidateRect(pHP->hwndClient, NULL, FALSE);
        }
    else
        ulRecursion--;
    }
    break;

/*                                                                                      *\
 * Syntax: WM_MOUSEMOVE, NULL, NULL                                                     *
\*                                                                                      */
case WM_MOUSEMOVE:                      /* Set mouse pointer accordingly to last smarticon pressed */
    if(pHP && (pHP->ulStatusFlag1 & OVERVIEWCURSOR))
        {
        if(usMenuCommand==ID_ICONMOVE)
            WinSetPointer(HWND_DESKTOP, hPointerMove);
        else
            WinSetPointer(HWND_DESKTOP, hPointerAction);
        }
    else
        {
        WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));
        }
    break;

/*                                                                                      *\
 * Syntax: WM_MOUSELEAVE, (SHORT sX, SHORT sY), (SHORT sFlag, sFlag)                    *
\*                                                                                      */
case WM_MOUSELEAVE:
/*                                                                                      *\
 * This message is generated by PC2HOOK.DLL when the pointer moved from on window to    *
 * another (for Warp 4, there exists an undocumented message, but this would break 2.11 *
 * and Warp 3 compatibility).                                                           *
\*                                                                                      */
                                        /* Test for valid window, or all CPU will be used (possibly
                                           due to for and back posting of WM_MOUSELEAVE message) */
    if(pHP->hwndSessionClient) WinPostMsg(pHP->hwndSessionClient, WM_MOUSELEAVE, mp1, mp2);
    if(pHP->hwndMenu) WinPostMsg(pHP->hwndMenu, WM_MOUSELEAVE, mp1, mp2);
    break;

/*                                                                                      *\
 * Syntax: WM_EXCPSEMDECREMENT, NULL, NULL                                              *
\*                                                                                      */
case WM_EXCPSEMDECREMENT:
    {
#ifdef  DEBUG
    PID     pidOwner=0;
    TID     tidOwner=0;
    ULONG   ulSemCount=0;
#endif  /* DEBUG */
    APIRET  apiretRc;

    apiretRc=DosReleaseMutexSem(hmtxExceptionHandler);
#ifdef  DEBUG
    if(apiretRc!=NO_ERROR)
        printf("PC2: DosReleaseMutexSem() Rc: %d\n", (int)apiretRc);
    apiretRc=DosQueryMutexSem(hmtxExceptionHandler, &pidOwner, &tidOwner, &ulSemCount);
    if(apiretRc!=NO_ERROR)
        printf("PC2: DosQueryMutexSem() Rc: %d\n", (int)apiretRc);
    printf("PC2: %s usage count is %d\n", PC2_EXCPHANDLER_SEM, ulSemCount);
#endif  /* DEBUG */
    }
    break;

/*                                                                                      *\
 * Syntax: WM_HOTKEY, (USHORT usFlags, USHORT usCh), ULONG ulKeyDataIndex               *
\*                                                                                      */
case WM_HOTKEY:
                                        /* Don't do it if the PM Hard Error or System Error popup
                                           window is active */
    if(pHP->ulRuntimeFlag & SYSTEMERRORACTIVE)
        {
        if(pHP->ulDebug>=DEBUG_LOW)
            printf("PC2: WM_HOTKEY SE active\n");
        break;
        }
    if(pHP->ulDebug>=DEBUG_LOW)
        printf("PC2: WM_HOTKEY\n");
                                        /* If the PC/2 Lockup dialog is active, keep the user
                                           from using any Hotkey */
    if(pHP->hwndLockup)
        break;
                                        /* Get the currently active top level window */
    pHP->hwndActiveFrameWindow=WinQueryActiveWindow(HWND_DESKTOP);
                                        /* Just pass this message to the working thread */
    WinPostMsg(pHP->hwndWorkingThread, WM_HOTKEY, MPFROMLONG(mp1), MPFROMLONG(mp2));
    break;

/*                                                                                      *\
 * Syntax: WM_ANALYZEPC2TRAP, ULONG ulHangCount, ULONG ulTrapCount                      *
\*                                                                                      */
case WM_ANALYZEPC2TRAP:
/*                                                                                      *\
 * Message posted by I/O thread when a more recent than previously known exception was  *
 * detected in PC2.TRP. Inform the user to contact the author.                          *
\*                                                                                      */
    {
    ULONG           ulHangCount=LONGFROMMP(mp1);
    ULONG           ulTrapCount=LONGFROMMP(mp2);
    UCHAR           *pucMessage;

    pucMessage=malloc(1024);
    if(pucMessage)
        {
        if(ulTrapCount!=0)
            strcpy(pucMessage,
                "PC/2 has detected by reading PC2.TRP that it did terminate abnormally previously, "
                "very likely caused by a severe internal PC/2 bug.\n\n"
                "In order to improve PC/2 I would strongly ask you not to delete PC2.TRP and to contact "
                "the author reporting that problem, thanks in advance!\n\n");
        else
            strcpy(pucMessage,
                "PC/2 has detected by reading PC2.TRP that it either terminated abnormally or "
                "one of its threads did hang previously.\n\n"
                "In order to improve PC/2 I would ask you to read the online help to check if this "
                "is already a known problem and contact the author reporting the problem, thanks!\n\n");
        strcat(pucMessage,
            "You may also check that PC2.TRP has not grown too much and delete it regularily - "
            "continuing...");
        USR_ERR(pHP->hwndFrame, HELP_PC2TRAP, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            pucMessage);
        free(pucMessage);
        }
    }
    break;


case WM_TIMER:
/*                                                                                      *\
 * The regular timer is used to refresh the container window, to have a real time       *
 * status for the spooler option and to query the WPS window handle regularily. Note    *
 * that the working thread resets pHP->ulCurrentTimer to 0 for every WM_TIMER message   *
 * it processes, if the current screen group is a fullscreen session, causing Lockup    *
 * and PowerDown not to trigger then.                                                   *
\*                                                                                      */
    {
                                        /* During cleanup count the number of ticks */
    static  ULONG   ulCleanupTicks=0;
                                        /* Prevent multiple error message boxes */
    static  ULONG   ulErrorTimeoutActive=FALSE;
    static  ULONG   ulErrorTimeoutCount=0;
    static  ULONG   ulErrorAliveActive=FALSE;
    UCHAR           *pucMessage=0;

                                        /* Verify to use our aquired timer */
    if(ulTimer!=LONGFROMMP(mp1)) break;
    ++ulTimerTicks;
    if(pHP->ulDebug>=DEBUG_LOW)
        if((ulTimerTicks % 10)==0)
            {
            DosBeep(1000, 10);
            printf("PC2: WM_TIMER\n");
            }
                                        /* Check if one thread has no reset its alive timer recently,
                                           which hints us that it is blocking/hanging/looping (I
                                           suspect it's blocking), and inform the user about that problem */
    if(ulErrorAliveActive==FALSE)
        {
        ULONG   ulAliveMsg=0;
        USHORT  usThreadMsg=0;
        ULONG   ulThreadIndex=0;
        TID     tidThread=0;
        UCHAR   ucThread[]="xTrd";

        pHP->ulAliveTimerITrd++;
        pHP->ulAliveTimerWTrd++;
        pHP->ulAliveTimerNTrd++;
#ifdef  DEBUG
        if(pHP->ulAliveTimerITrd>(TICKSPERMINUTE*3))
#else
        if(pHP->ulAliveTimerITrd>(TICKSPERMINUTE*5))
#endif  /* DEBUG */
            {
            ulAliveMsg=pHP->ulAliveMsgITrd;
            pHP->ulAliveTimerITrd=0;
            usThreadMsg=ID_OVDUMPITRD;
            ulThreadIndex=ID_IOTHREAD;
            tidThread=pthreaddataIOThread->tidThread;
            ucThread[0]='I';
            }
#ifdef  DEBUG
        if(pHP->ulAliveTimerNTrd>(TICKSPERMINUTE*3))
#else
        if(pHP->ulAliveTimerNTrd>(TICKSPERMINUTE*5))
#endif  /* DEBUG */
            {
            ulAliveMsg=pHP->ulAliveMsgNTrd;
            pHP->ulAliveTimerNTrd=0;
            usThreadMsg=ID_OVDUMPNTRD;
            ulThreadIndex=ID_NETTHREAD;
            tidThread=pthreaddataNetThread->tidThread;
            ucThread[0]='N';
            }
#ifdef  DEBUG
        if(pHP->ulAliveTimerWTrd>(TICKSPERMINUTE*1))
#else
        if(pHP->ulAliveTimerWTrd>(TICKSPERMINUTE*2))
#endif  /* DEBUG */
            {
            ulAliveMsg=pHP->ulAliveMsgWTrd;
            pHP->ulAliveTimerWTrd=0;
            usThreadMsg=ID_OVDUMPWTRD;
            ulThreadIndex=ID_WORKINGTHREAD;
            tidThread=pthreaddataWorkingThread->tidThread;
            ucThread[0]='W';
            }
        if(usThreadMsg!=0)
            {
            ULONG   ulResponse;

            pucMessage=malloc(1024);
            *pucMessage='\0';
                                        /* Due to user request, be silent by default */
            sprintf(pucMessage, "PC/2 has detected that thread ID:0x%X (%s) is not longer responding after having processed "\
                "checkpoint 0x%08lX at last (Diagnostics information logged into PC2.TRP.) "\
                "PC/2 has a built-in Hang-Protection, you may just need to press CTRL+SYSREQ "\
                "at the OverView window for a manual reinitialization, "\
                "if that does not help you have to (force) exit PC/2 by pressing ALT+CTRL+SHIFT+END at the OverView window, "\
                "killing it then with a process killer and relaunch it.\n\n"
                "In order to improve PC/2 I would ask you to read the online help and contact the author "
                "reporting the problem, thanks! - continuing...",
                (int)ulThreadIndex, ucThread, ulAliveMsg);
            WinSendMsg(hwnd, WM_COMMAND, MPFROMSHORT(usThreadMsg), MPFROM2SHORT(CMDSRC_OTHER, FALSE));
            ulErrorAliveActive=TRUE;
            if(pHP->ulDebug>=DEBUG_LOW)
                ulResponse=USR_ERR(pHP->hwndFrame, HELP_PC2ALIVE, MB_ERROR|MB_OKCANCEL|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    pucMessage);
            else
                ulResponse=MBID_OK;
            if(ulResponse==MBID_OK)
                {
                APIRET  apiretRc;
#ifdef  DEBUG
                PID     pidOwner=0;
                TID     tidOwner=0;
                ULONG   ulSemCount=0;

                printf("PC2: DosKillThread() for thread %d\n", (int)tidThread);
#endif  /* DEBUG */
                apiretRc=DosRequestMutexSem(hmtxExceptionHandler, SEM_IMMEDIATE_RETURN);
#ifdef  DEBUG
                if(apiretRc!=NO_ERROR)
                    printf("PC2: DosRequestMutexSem() Rc: %d\n", (int)apiretRc);
                apiretRc=DosQueryMutexSem(hmtxExceptionHandler, &pidOwner, &tidOwner, &ulSemCount);
                if(apiretRc!=NO_ERROR)
                    printf("PC2: DosQueryMutexSem() Rc: %d\n", (int)apiretRc);
                printf("PC2: %s usage count is %d\n", PC2_EXCPHANDLER_SEM, ulSemCount);
#endif  /* DEBUG */
                DosKillThread(tidThread);
                }
            free(pucMessage);
            pucMessage=0;
            ulErrorAliveActive=FALSE;
            }
        }
                                        /* During cleanup processing ensure that the Titlebar icons
                                           of all other processes have been removed before continuing
                                           cleanup processing */
    if(pHP->ulRuntimeFlag & PC2CLEANUP)
        {
        HWND    hwndCleanupNext=NULLHANDLE;

#ifdef  DEBUG
        printf("PC2: WM_CLEANUP Frame processing\n");
#endif  /* DEBUG */
        ulCleanupTicks++;
        if((ulErrorTimeoutActive==FALSE))
            hwndCleanupNext=(HWND)pCleanup();
if(hwndCleanupNext!=NULLHANDLE)
    printf("Waiting for %08X\n", hwndCleanupNext);
                                        /* Continue cleanup after having cleaned up all windows (which
                                           does not work under curious reasons, from my experience because
                                           being message boxes bypassing normal message processing or having
                                           changed HWND_OBJECT as owner) */
        if((hwndCleanupNext!=NULLHANDLE) &&
            (ulCleanupTicks>((TICKSPERMINUTE>>1))) &&
            (ulErrorTimeoutActive==FALSE))
            {
            ulErrorTimeoutActive=TRUE;
            ulErrorTimeoutCount++;
            pucMessage=malloc(1024);
            sprintf(pucMessage,
                "PC/2 detected a timeout during cleanup processing of window %08lX. As PC/2 has added "
                "code to other processes it can't ignore that problem and continue. Unfortunately if "
                "further waiting does not help, a reboot may be the only choice. Please try to rerun "
                "PC/2 with TitleBar SmartIcons disabled.", (ULONG)hwndCleanupNext);
                                        /* With 2.20 changed, see above
                                           ulCleanupTicks=0;
                                           */
            USR_ERR(pHP->hwndFrame, HELP_PC2CLEANUP, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                pucMessage);
            free(pucMessage);
            pucMessage=0;
            ulErrorTimeoutActive=FALSE;
                                        /* With 2.20 just ignore further errors, see above */
            ulCleanupTicks++;
            }
                                        /* If waiting does not help, just ignore the problem */
        if(ulErrorTimeoutCount>=2)
            {
            WinShowWindow(hwndCleanupNext, TRUE);
            hwndCleanupNext=0;
            }
        if((hwndCleanupNext==NULLHANDLE) && (ulErrorTimeoutActive==FALSE))
            {
            if(pHP->ulDebug>=DEBUG_LOW)
                printf("PC2: Posting WM_QUITTHREAD\n");
#ifdef  DEBUG
            printf("PC2: Posting WM_QUITTHREAD\n");
#endif  /* DEBUG */
                                        /* Prevent multiple calls */
            ulErrorTimeoutActive=TRUE;
                                        /* Post WM_QUITTHREAD to thread object windows, because this message
                                           informs the threads to reenable their message queues to receive
                                           WM_QUIT messages which the threads then posts to itself
                                           to terminate. Once all threads but thread 1 have terminated,
                                           the main thread terminates its message queue by posting a
                                           WM_QUITTHREAD message to itself */
            WinPostMsg(pHP->hwndWorkingThread, WM_QUITTHREAD, MPFROMLONG(ID_MAINTHREAD), NULL);
            WinPostMsg(pHP->hwndIOThread, WM_QUITTHREAD, MPFROMLONG(ID_MAINTHREAD), NULL);
            WinPostMsg(pHP->hwndNetThread, WM_QUITTHREAD, MPFROMLONG(ID_MAINTHREAD), NULL);
#ifdef  DEBUG
            printf("PC2: Back from Posting WM_QUITTHREAD\n");
#endif  /* DEBUG */
            }
                                        /* As during cleanup the windows we used to post WM_TIMER messages
                                           to are being destroyed and our saved window handles have already
                                           been cleared to NULLHANDLE, avoid posting further messages, as
                                           posting to a NULLHANDLE hangs PM */
        break;
        }
                                        /* Post message to SessionBar (required for e.g. to update
                                           the system time information) */
    WinPostMsg(pHP->hwndSessionClient, msg, mp1, mp2);
                                        /* Post message to working thread (required for e.g. the
                                           scheduler) */
    WinPostMsg(pHP->hwndWorkingThread, msg, mp1, mp2);
                                        /* Post message to other threads */
    WinPostMsg(pHP->hwndIOThread, msg, mp1, mp2);
    if((ulTimerTicks % TICKSPERMINUTE)==0)
        WinPostMsg(pHP->hwndNetThread, msg, mp1, mp2);
                                        /* The time has expired another TIMERINTERVALLMS milliseconds,
                                           so update the Lockup/APM activation timer when required.
                                           Note that the working thread my clear the timer during
                                           processing the WM_TIMER message if the current screen group
                                           is a fullscreen (OS/2 or DOS) session */
    if((ulTimerTicks % TICKSPERSECOND)==0)
        pHP->ulCurrentTimer++;
                                        /* If the Lockup timer is enabled, check if timer
                                           expired. PC2HOOK.DLL clears the timer every time
                                           activity is detected in the input hook, because
                                           this hook is called for any keyboard or mouse input */
    if((pHP->ulStatusFlag2 & ENABLELOCKUP) && (pHP->ulLockupTimer!=0))
        {
        if(pHP->ulCurrentTimer>=(pHP->ulLockupTimer*60))
            {
                                        /* Don't do it if the PM Hard Error or System Error popup
                                           window is active or if the current fullscreen session is
                                           not the PM session (value 1) - which is prevented by
                                           resetting the count above  */
            if(!(pHP->ulRuntimeFlag & SYSTEMERRORACTIVE))
                                        /* Request Lockup to show if the the time expired */
                WinPostMsg(pHP->hwndClient, WM_PC2LOCKUP, (MPARAM)TRUE, NULL);
            }
        else if((pHP->ulCurrentTimer<=1) && (pHP->hwndLockup!=NULLHANDLE))
                                        /* If timer was reset due to user activity, inform Lockup
                                           dialog (if already showing) */
            WinPostMsg(pHP->hwndClient, WM_PC2LOCKUP, (MPARAM)FALSE, NULL);
        }
                                        /* If the PowerDown timer is enabled, check if timer
                                           expired. PC2HOOK.DLL clears the timer every time
                                           activity is detected in the input hook, because
                                           this hook is called for any keyboard or mouse input */
    if((pHP->ulStatusFlag2 & SUPPORTAPM) &&
        (pHP->ulStatusFlag2 & ENABLEPOWERDOWN) &&
        (pHP->ulPowerDownTimer!=0))
        {
        if(pHP->ulCurrentTimer>=(pHP->ulPowerDownTimer*60))
            {
                                        /* Don't do it if the PM Hard Error or System Error popup
                                           window is active */
            if(!(pHP->ulRuntimeFlag & SYSTEMERRORACTIVE))
                                        /* Request lockup if the user wants so */
                WinPostMsg(pHP->hwndClient, WM_COMMAND, MPFROMSHORT(ID_POWERDOWN), MPFROMSHORT(CMDSRC_OTHER));
            }
        }
                                        /* Update every quarter of a minute */
    if((ulTimerTicks % (TICKSPERMINUTE>>2))==0)
        {
                                        /* Get format for date/time display */
        GetDateTimeNLS(TRUE);
                                        /* Post message to refresh spooler */
        WinPostMsg(pHP->hwndSpoolerClient, WM_REFRESHSPOOLER, NULL, NULL);
        }
    }
    break;

                                        /* Due to user requests, this has been disabled by
case WM_BUTTON1DOWN:
    return((MRESULT)TRUE);
                                           version 2.00 of PC/2 */

/*                                                                                      *\
 * Syntax: WM_BUTTON2DOWN, (SHORT sClickX, SHORT sClickY), NULL                         *
\*                                                                                      */
case WM_BUTTON2DOWN:
/*                                                                                      *\
 * This message detected and passed from the PC/2 window procedure is used to track or  *
 * perform certain actions on windows on the Virtual Desktops.                          *
\*                                                                                      */
    {
    WINDOWDATA  *pwdCurrent=0;          /* WINDOWDATA structure of current Desktop child window */
    TRACKINFO   tiWindow;               /* Trackinfo structure for one window */

    if(pHP->ulRuntimeFlag&QUITFROMWINDOWLIST)
        break;
                                        /* Get the WINDOWDATA on whose representation in the Overview window
                                           the user pressed mouse button 2 down */
    pwdCurrent=GetDesktopWindows(pHP->habPc2, DESKTOPWINDOWS_CLICK, mp1);
    if(pwdCurrent)
        {
        SWP     swpWindow;              /* Copy of Window's position, because working thread may update it */

                                        /* If the Desktop's (WPS's) rectangle is selected, only perform
                                           operation when the ALT key is pressed additionally */
        if((!strcmp(pwdCurrent->ucWindowTitle, pHP->ucDesktopName)) &&
            (!(WinGetKeyState(HWND_DESKTOP, VK_ALT) & 0x8000)))
            break;
                                        /* Initialize to most common values required */
        memcpy(&swpWindow, pwdCurrent->pswpWindow, sizeof(SWP));
        swpWindow.x=swpWindow.y=swpWindow.cx=swpWindow.cy=0;
        swpWindow.hwndInsertBehind=NULLHANDLE;
        switch(usMenuCommand)
        {
        case ID_ICONMOVE:
            memcpy(&swpWindow, pwdCurrent->pswpWindow, sizeof(SWP));
                                        /* Calculate and draw starting tracking rectangle */
            tiWindow.rclTrack.xLeft=pHP->ptlOrigin.x+
                (float)swpWindow.x*pHP->fScaleX;
            tiWindow.rclTrack.yBottom=pHP->ptlOrigin.y+
                (float)swpWindow.y*pHP->fScaleY;
            tiWindow.rclTrack.xRight=tiWindow.rclTrack.xLeft+
                (float)swpWindow.cx*pHP->fScaleX;
            tiWindow.rclTrack.yTop=tiWindow.rclTrack.yBottom+
                (float)swpWindow.cy*pHP->fScaleY;
            tiWindow.cxBorder=1;        /* Border width */
            tiWindow.cyBorder=1;
            tiWindow.cxGrid=1;          /* Grid width */
            tiWindow.cyGrid=1;
            tiWindow.cxKeyboard=1;      /* Movement in pixel for keyboard (keyboard currently can't
                                           start tracking) */
            tiWindow.cyKeyboard=1;
                                        /* Minimum tracking size */
            tiWindow.ptlMinTrackSize.x=2;
            tiWindow.ptlMinTrackSize.y=2;
                                        /* Maximum tracking size */
            tiWindow.ptlMaxTrackSize.x=pHP->swpPC2Client.cx;
            tiWindow.ptlMaxTrackSize.y=pHP->swpPC2Client.cy;
                                        /* Boundary rectangle */
            tiWindow.rclBoundary.xLeft=0;
            tiWindow.rclBoundary.yBottom=0;
            tiWindow.rclBoundary.xRight=pHP->swpPC2Client.cx;
            tiWindow.rclBoundary.yTop=pHP->swpPC2Client.cy;
                                        /* Tracking options */
            tiWindow.fs=TF_SETPOINTERPOS | TF_MOVE | TF_ALLINBOUNDARY;
            if(WinTrackRect(hwnd, NULLHANDLE, &tiWindow))
                {                       /* If tracking was successful reposition tracked window */
                swpWindow.fl=SWP_MOVE|SWP_NOADJUST;
                                        /* If user moved window to another desktop, keep its
                                           relative position */
                swpWindow.x=((float)(tiWindow.rclTrack.xLeft-pHP->ptlOrigin.x))/pHP->fScaleX;
                swpWindow.y=((float)(tiWindow.rclTrack.yBottom-pHP->ptlOrigin.y))/pHP->fScaleY;
                                        /* Force a reread of windows on Desktop */
                WinPostMsg(hwnd, WM_SETUPSIZEPOSITION, MPFROMLONG(WM_MOVE), NULL);
                }
            break;

        case ID_ICONHIDE:               /* Hide selected window */
            swpWindow.fl=SWP_HIDE;
                                        /* Force a reread of windows on Desktop (message is
                                           posted, so this message is finished before) */
            WinPostMsg(hwnd, WM_SETUPSIZEPOSITION, MPFROMLONG(WM_MOVE), NULL);
            break;

        case ID_ICONZORDERTOP:          /* Set selected window to top of Desktop */
            swpWindow.fl=SWP_ZORDER;
            swpWindow.hwndInsertBehind=HWND_TOP;
            break;

        case ID_ICONZORDERBOTTOM:       /* Set selected window to bottom of Desktop */
            swpWindow.fl=SWP_ZORDER;
            swpWindow.hwndInsertBehind=HWND_BOTTOM;
            break;

        case ID_ICONCLOSE:              /* Close selected window by simulating the selection of
                                           the "close" menuitem of the applications's system menu */
            WinPostMsg(swpWindow.hwnd, WM_SYSCOMMAND, MPFROMLONG(SC_CLOSE), MPFROM2SHORT(CMDSRC_MENU, FALSE));
            break;

        case ID_ICONMAXIMIZE:           /* Maximize selected window */
            swpWindow.fl=SWP_MAXIMIZE;
            break;

        case ID_ICONMINIMIZE:           /* Maximize selected window */
            swpWindow.fl=SWP_MINIMIZE;
            break;

        case ID_ICONRESTORE:            /* Maximize selected window */
            swpWindow.fl=SWP_RESTORE;
            break;
        }
                                        /* Apply changes */
        WinSetMultWindowPos(pHP->habPc2, &swpWindow, 1);
        }
    }
    break;

case WM_BUTTON1CLICK:
case WM_BUTTON1DBLCLK:
    if(pHP->ulRuntimeFlag & QUITFROMWINDOWLIST)
        break;
                                        /* Ignore request when not the one we're waiting for */
    if(msg!=DESKTOP_SWITCH_EVENT)
        break;
    if(pHP->ulDebug>=DEBUG_ENTRY)
        printf("PC2: WM_BUTTON1(CLICK|DBLCLK) ");
    if(!(ulThreadReady & THREADMOVEBUSY))
        {
        if(pHP->ulDebug>=DEBUG_ENTRY)
            printf("thread not busy\n");

                                        /* Set that working thread possibly has to
                                           move windows on the Virtual Desktop */
        if(WinPostMsg(pHP->hwndWorkingThread, msg, MPFROMLONG(mp1), MPFROMLONG(mp2)))
            ulThreadReady|=THREADMOVEBUSY;
        else
            WinPostMsg(hwnd, msg, mp1, mp2);
        }
    else
        if(pHP->ulDebug>=DEBUG_ENTRY)
            printf("thread busy\n");
    break;

case WM_BUTTON2DBLCLK:
#ifdef  DEBUG
    {
    KEYDATA *pKD=pHP->pKeyData;
    ULONG   ulIndex;
    CHAR    cTemp[9];               /* CTRL, ALT or CTRL+ALT as strings */

    for(ulIndex=0; ulIndex<KEYDATACOUNT; ulIndex++, pKD++)
        {
        if(pKD->usFlags==(KC_CTRL|KC_ALT))
            strcpy(cTemp, "CTRL+ALT");
        if(pKD->usFlags==KC_RCTRL)
            strcpy(cTemp, "RCTRL");
        if(pKD->usFlags==KC_CTRL)
            strcpy(cTemp, "CTRL");
        if(pKD->usFlags==KC_ALT)
            strcpy(cTemp, "ALT");
        if(pKD->usFlags==KC_LWIN)
            strcpy(cTemp, "LWIN");
        printf("%s+%c : %c\n",
            cTemp, (char)pKD->usCh, (pKD->bUsed==FALSE ? ' ' : '<'));
        }
    }
#endif  /* DEBUG */
    break;

/*                                                                                      *\
 * Syntax: WM_WINDOWLIST, (USHORT x, USHORT y), NULL                                    *
\*                                                                                      */
case WM_WINDOWLIST:
                                        /* Pass this message to the working thread */
#ifdef  DEBUG
    printf("PC2: WM_WINDOWLIST\n");
#endif  /* DEBUG */
    WinPostMsg(pHP->hwndWorkingThread, WM_WINDOWLIST, MPFROMLONG(mp1), MPFROMLONG(mp2));
    break;

/*                                                                                      *\
 * Syntax: WM_POPUPMENU, (SHORT x, SHORT y), HWND hwndPopup                             *
\*                                                                                      */
case WM_POPUPMENU:
/*                                                                                      *\
 * The hook found that button 1 was clicked on the Desktop and sent us this message. It *
 * is either a WM_BUTTON1CLICK or WM_BUTTON1DBLCLK. First we obtain the focus, to be    *
 * able to start our programs in the foreground. The coordinates passed are relative to *
 * the screen, that is, they need not to be translated from window coordinates to       *
 * Desktop coordinates. Currently we don't differentiate if button 1 was clicked on PM  *
 * or WPS, which can be determined by hwndPopup. The top level window active before the *
 * Popup-Menu got called was saved by the input hook.                                   *
 * The SessionBar also sends this message with (-1, -1) as the position, in this case   *
 * we have to align the Popup Menu relative to the SessionBar.                          *
\*                                                                                      */
    {
    static ULONG    ulMenuCentered=FALSE;
    POINTL          ptlPopupPosition;
    SWP             swpPopupMenu;
    USHORT          fsOptions=PU_NONE | PU_KEYBOARD | PU_MOUSEBUTTON1 |
                              PU_HCONSTRAIN | PU_VCONSTRAIN;

                                        /* Don't do it if the PM Hard Error or System Error popup
                                           window is active */
    if(pHP->ulRuntimeFlag & SYSTEMERRORACTIVE)
        {
        if(pHP->ulDebug>=DEBUG_LOW)
            printf("PC2: WM_POPUPMENU SE active\n");
        break;
        }
    if(pHP->ulDebug>=DEBUG_LOW)
        {
        DosBeep(1000, 50);
        printf("PC2: WM_POPUPMENU\n");
        }
                                        /* If the PC/2 Lockup dialog is active, keep the user
                                           from activating the Popup-Menu. Same when a system modal
                                           window is active */
                                        /* Unfortunately for some window (e.g. IBM's BookManager Read/2),
                                           (WinQuerySysModalWindow(HWND_DESKTOP)) returns a system modal
                                           window, even if it isn't one - that is other windows can get
                                           the input focus, we can therefore not test for system model
                                           windows to prevent activation of the Popup Menu */
    if((pHP->hwndLockup) && (pHP->pPopupMenu!=0))
        break;
                                        /* If we haven't loaded the menu into PM yet (this will not take
                                           very long anyway) ignore request of displaying Popup-Menu */
#ifdef  DEBUG_PC2
    printf("PC2: WM_POPUPMENU\n");
#endif  /* DEBUG_PC2 */
    if(pHP->ulRuntimeFlag & POPUPMENULOADING)
        {
#ifdef  DEBUG_PC2
    printf("PC2: WM_POPUPMENU - Busy, request ignored\n");
#endif  /* DEBUG_PC2 */
        WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE));
        break;
        }
                                        /* For the first time the Popup Menu is requested, pop it up
                                           to allow calculations (center, icons,...) */
    if(ulMenuCentered==FALSE)
        {
                                        /* Pop up the popup menu at a not visible position, because
                                           WinPopupMenu() calculates the Popup-Menu's size. Otherwise
                                           WinGetWindowPos() would return 0 for size in WM_POPUPMENU. */
        WinPopupMenu(HWND_DESKTOP, hwnd, pHP->hwndPopupMenu,
                (pHP->DesktopSize.x<<3), 0, 0, PU_NONE);
                                            /* Hide the menu again, as the calculation took already place */
        WinShowWindow(pHP->hwndPopupMenu, FALSE);
        ulMenuCentered=TRUE;
        }
                                        /* Display Configuration dialog if the Popup-Menu
                                           doesn't contain one */
    if(pHP->ulRuntimeFlag & DISPLAYCONFIGDIALOG)
        {
                                        /* Post message to load Configure PC/2 dialog box.
                                           We post it because send is synchronous and will
                                           block the message queue which however must
                                           not be blocked in order to handle the
                                           Configuration dialog window procedure. If the dialog
                                           is already displayed (and the Desktop dialog is not
                                           displayed - because even if the Setup dialog is dismissed
                                           WinProcessDlg() does not return until the Desktop dialog
                                           is dismissed also - don't ask me why), switch to it */
        if((pHP->hwndConfigDialog) && (!pHP->hwndDesktopDialog))
            WinFocusChange(HWND_DESKTOP, pHP->hwndConfigDialog, 0);
        else
            WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_CONFIGDIALOG), MPFROMSHORT(CMDSRC_OTHER));
        }
    else
        {                               /* If there is one menuentry in the Popup-Menu
                                           position on the first */
        if((pHP->pPopupMenu->Item!=ENTRYEMPTY) && (pHP->ulStatusFlag1 & SELECTFIRSTITEM))
            fsOptions|=PU_POSITIONONITEM;
                                        /* Get the position and window, where the user
                                           clicked to get the Popup-Menu */
        ptlPopupPosition.x=(ULONG)SHORT1FROMMP(mp1);
        ptlPopupPosition.y=(ULONG)SHORT2FROMMP(mp1);
                                        /* Check for the SessionBar specific settings */
        if((ptlPopupPosition.x==(ULONG)(USHORT)-1) && (ptlPopupPosition.y==(ULONG)(USHORT)-1))
            {
                                        /* Get the size of the (currently hidden) Popup-Menu */
            WinQueryWindowPos(pHP->hwndPopupMenu, &swpPopupMenu);
                                        /* Align the Popup Menu to the left border. The vertical position is
                                           dependent on the vertical position of the SessionBar and gets
                                           adjusted that the Popup Menu does not cover the SessionBar and
                                           can be displayed most probably complete */
            ptlPopupPosition.x=0;
            if((pHP->swpScreen.cy-(pHP->swpPC2Session.y+pHP->swpPC2Session.cy))>pHP->swpPC2Session.y)
                ptlPopupPosition.y=pHP->swpPC2Session.y+pHP->swpPC2Session.cy;
            else
                ptlPopupPosition.y=pHP->swpPC2Session.y-swpPopupMenu.cy;
            }
        else
            {
                                        /* Verify that the coordinates are valid, center to screen
                                           if one is invalid (because the Popup-Menu may be activated
                                           by SHIFT+SHIFT presses without any mouse installed) */
            if((ptlPopupPosition.x<0) || (ptlPopupPosition.x>pHP->DesktopSize.x))
                ptlPopupPosition.x=(pHP->DesktopSize.x>>1);
            if((ptlPopupPosition.y<0) || (ptlPopupPosition.y>pHP->DesktopSize.y))
                ptlPopupPosition.y=(pHP->DesktopSize.y>>1);
                                        /* If Popup-Menu should not position to the first entry
                                           automatically, we try to center it around the mouse
                                           pointer position */
            if(pHP->ulStatusFlag1 & SELECTCENTERITEM)
                {
                                        /* Get the size of the (currently hidden) Popup-Menu */
                WinQueryWindowPos(pHP->hwndPopupMenu, &swpPopupMenu);
                ptlPopupPosition.x-=(swpPopupMenu.cx>>1);
                ptlPopupPosition.y-=(swpPopupMenu.cy>>1);
                }
            }
        if(!WinPopupMenu(               /* Pop up the popup menu */
            HWND_DESKTOP,               /* Parent window handle */
            hwnd,                       /* Owner window handle that receives all the
                                           notification messages generated by the pop-up
                                           menu */
                                        /* Popup menu window handle */
            pHP->hwndPopupMenu,
            ptlPopupPosition.x,         /* x-coordinate of mouse pointer for popup menu */
            ptlPopupPosition.y,         /* y-coordinate of mouse pointer for popup menu */
                                        /* Input item identity, if PU_POSITIONONITEM or
                                           PU_SELECTITEM is set */
            pHP->pPopupMenu->id,
            fsOptions))                  /* Input options */
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEWINDOW, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Activation of PC/2's Popup-Menu failed - continuing...");
        }
    break;
    }

/*                                                                                      *\
 * Syntax: WM_MOVEREQUEST, (USHORT usMouseXPos, USHORT usMouseYPos), (ULONG ulMoveFlag) *
\*                                                                                      */
case WM_MOVEREQUEST:
/*                                                                                      *\
 * This message is sent by the PC/2 Input Hook as a result of clicking mouse button 1   *
 * onto any border row or column of the Desktop, requesting to move to another Virtual  *
 * Desktop. Also, it can come from ALT+CTRL+SHIFT+cursor keys, but due to that we have  *
 * to ensure not to get out of the defined Virtual Desktop overall size.                *
\*                                                                                      */
                                        /* Don't do it if the PM Hard Error or System Error popup
                                           window is active */
    if(pHP->ulRuntimeFlag & SYSTEMERRORACTIVE)
        {
        if(pHP->ulDebug>=DEBUG_LOW)
            printf("PC2: WM_MOVEREQUEST SE active");
        break;
        }
    if(pHP->ulDebug>=DEBUG_FULL)
        printf("PC2: WM_MOVEREQUEST ");
                                        /* Ensure that messages get not queued into the working
                                           threads queue if it is yet busy processing the last
                                           request (unless autoscrolling has set the repost flag) */
    if(ulThreadReady & THREADMOVEBUSY)
        {
        if(pHP->ulDebug>=DEBUG_FULL)
            printf("Thread busy\n");
        }
    else
        {
        if(pHP->ulDebug>=DEBUG_FULL)
            printf("Thread not busy\n");
                                        /* Set that working thread gets something to do. It will
                                           not post WM_THREADREADY until it has finished this
                                           moving request, and only WM_THREADREADY will reset our
                                           THREADMOVEBUSY flag. Therefore all move requests while
                                           the working thread is busy will be ignored */
        if(WinPostMsg(pHP->hwndWorkingThread, WM_MOVEREQUEST, mp1, mp2))
            ulThreadReady|=THREADMOVEBUSY;
        else
            WinPostMsg(hwnd, msg, mp1, mp2);
        }
    break;

/*                                                                                      *\
 * Syntax: WM_ZORDER, HWND hwndFrame2Bottom, NULL                                       *
\*                                                                                      */
case WM_ZORDER:
/*                                                                                      *\
 * The hook found that button 2 was clicked on a window's titlebar. After having set    *
 * this window to bottom and posted us this message, we find the current window below   *
 * the mouse pointer and activate it.                                                   *
\*                                                                                      */
    {
                                        /* Window immediately below (z-order) the mouse pointer */
    HWND    hwndNextFrame;
    HWND    hwndNextFrameOwner;
    POINTL  ptlMouse;

    WinQueryPointerPos(HWND_DESKTOP, &ptlMouse);
    hwndNextFrameOwner=hwndNextFrame=WinWindowFromPoint(HWND_DESKTOP, &ptlMouse, FALSE);
    do  {
        hwndNextFrame=hwndNextFrameOwner;
        hwndNextFrameOwner=WinQueryWindow(hwndNextFrame, QW_OWNER);
        } while(hwndNextFrameOwner!=NULLHANDLE);
                                        /* Now switch to the new frame window, causing it to
                                           get to top of Z-Order, if it was the one we just
                                           put to bottom of Z-Order we can't prevent it to
                                           get back to foreground as PM would assign the next
                                           window on top of Z-Order to have the input focus,
                                           which is likely the SessionBar which in turn activates
                                           the window below the mouse pointer */
    WinFocusChange(HWND_DESKTOP, WinWindowFromID(hwndNextFrame, FID_CLIENT), 0);
    }
    break;

/*                                                                                      *\
 * Syntax: WM_SETUPSIZEPOSITION, ULONG ulMsg, HWND hwndWindow                           *
\*                                                                                      */
case WM_SETUPSIZEPOSITION:
/*                                                                                      *\
 * This message is sent by the PC/2 WinSendMsg()-Hook as a result of creating, sizing,  *
 * moving, destroying a frame window or switching to another frame window. This         *
 * requires that one or more windows need to be redrawn on the Overview window.         *
\*                                                                                      */
                                        /* Skip while exiting PC/2 */
    if(pHP->ulRuntimeFlag & PC2CLEANUP)
        break;
    if(pHP->ulDebug>=DEBUG_FULL)
        {
        printf("PC2: WM_SETUPSIZEPOSITION ");
        if(LONGFROMMP(mp1)==WM_MOVE)
            printf("(WM_MOVE) ");
        else
            printf("(Desktop Enumeration) ");
        }
    if(!(ulThreadReady & THREADWINDOWBUSY))
        {
        if(pHP->ulDebug>=DEBUG_FULL)
            printf("Thread not busy\n");
                                        /* Set that working thread gets something to do */
        if(WinPostMsg(pHP->hwndWorkingThread, WM_SETUPSIZEPOSITION, MPFROMLONG(mp1), mp2))
            ulThreadReady|=THREADWINDOWBUSY;
        else
            WinPostMsg(hwnd, msg, mp1, mp2);
        }
    else
        {
        if(pHP->ulDebug>=DEBUG_FULL)
            printf("Thread busy, request queued\n");
                                        /* Save additional work until thread is not busy again */
        DosSleep(0);
        ulThreadReady|=THREADWINDOWQUEUED;
        }
    break;

/*                                                                                      *\
 * Syntax: WM_THREADREADY, ULONG ulFlag, NULL                                           *
\*                                                                                      */
case WM_THREADREADY:
/*                                                                                      *\
 * This message is sent by the PC/2 working thread when he finished handling a          *
 * WM_MOVEREQUEST or WM_SETUPSIZEPOSITION message. This ensures that pending messages   *
 * possibly caused by the same user action are not queued by WinPostMsg() calls.        *
\*                                                                                      */
                                        /* Skip while exiting PC/2 */
    if(pHP->ulRuntimeFlag & PC2CLEANUP)
        break;
    if(pHP->ulDebug>=DEBUG_FULL)
        printf("PC2: WM_THREADREADY ");
    if(LONGFROMMP(mp1)==THREADMOVEBUSY)
        {
        if(pHP->ulDebug>=DEBUG_FULL)
            {
            printf("(WM_MOVE) "); printf("\n");
            }
        ulThreadReady&=(~THREADMOVEBUSY);
                                        /* If autoscroll is active, just repeat a dummy WM_MOUSEMOVE message by
                                           sending it to the main thread (but the hook will trap it) and if
                                           the pointer is still at a sourrounding row and/or column after the
                                           last move request has finished, another move request will be generated */
        if(pHP->ulStatusFlag1 & AUTOSCROLL)
            {
            POINTL  pointlCurrent;

            WinQueryPointerPos(HWND_DESKTOP, &pointlCurrent);
            WinPostMsg(hwnd, WM_MOUSEMOVE,
                MPFROM2SHORT((SHORT)pointlCurrent.x, (SHORT)pointlCurrent.y), MPFROM2SHORT(HT_DISCARD, KC_NONE));
            }
        }
    else
        {
        if(pHP->ulDebug>=DEBUG_FULL)
            printf("(Desktop Enumeration) ");
                                        /* Set that working thread finished his work of
                                           enumerating windows */
        ulThreadReady&=(~THREADWINDOWBUSY);
                                        /* If some work was queued do work once */
        if(ulThreadReady & THREADWINDOWQUEUED)
            {
            if(pHP->ulDebug>=DEBUG_FULL)
                printf(", doing queued Desktop enumeration request\n");
                                        /* We are doing one queued enumerating windows request */
            ulThreadReady&=(~THREADWINDOWQUEUED);
                                        /* Set that working thread gets something to do */
            if(WinPostMsg(pHP->hwndWorkingThread, WM_SETUPSIZEPOSITION, NULL, NULL))
                ulThreadReady|=THREADWINDOWBUSY;
            else
                WinPostMsg(hwnd, msg, mp1, mp2);
            }
        else
            if(pHP->ulDebug>=DEBUG_FULL)
                printf("\n");
        }
    break;

/*                                                                                      *\
 * Syntax: WM_PC2LOCKUP, bLockupTimerExpired, NULL                                      *
\*                                                                                      */
case WM_PC2LOCKUP:
/*                                                                                      *\
 * This message is posted by the main thread to activate the Lockup OS/2 function.      *
\*                                                                                      */
    {
    APIRET  rc;
                                        /* Once the Lockup dialog is active, we may get
                                           further requests, which we will ignore then */
    static BOOL     bLockupActive=FALSE;
    static SWP      swpLockup;
    static int      iTimerTicks;

#ifdef  DEBUG_PC2
    printf("PC2: WM_PC2LOCKUP\n");
#endif  /* DEBUG_PC2 */
    if(pHP->ulDebug>=DEBUG_LOW)
        printf("PC2: WM_PC2LOCKUP\n");
    if(bLockupActive==FALSE)
        {                               /* Activate Lockup dialog */
        HFILE   hfileKeyBoard=0;
        ULONG   ulActionTaken;
        ULONG   ulParameterLengthInOut;
        ULONG   ulDataLengthInOut;
        HOTKEY  PPF;                    /* IOCTL Parameter Packet Format */
        ULONG   ulCreateParams=TRUE;

        bLockupActive=TRUE;
                                        /* Activate ourselves */
        WinSwitchToProgram(WinQuerySwitchHandle(pHP->hwndFrame, pHP->pPib->pib_ulpid));
                                        /* Open keyboard to disable session manager keys */
        rc=DosOpen("\\DEV\\KBD$", (PHFILE)&hfileKeyBoard, &ulActionTaken, 0, FILE_NORMAL,
            OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE, 0);
#ifdef  DEBUG_PC2
        if(rc) printf("     DosOpen KBD$ failed: %x\n", (int)rc);
#endif  /* DEBUG_PC2 */
        memset(&PPF, 0, sizeof(PPF));
        PPF.idHotKey=-1;
        ulParameterLengthInOut=sizeof(PPF);
        ulDataLengthInOut=0;
        rc=DosDevIOCtl(hfileKeyBoard, IOCTL_KEYBOARD, KBD_SETSESMGRHOTKEY,
            &PPF, sizeof(PPF), &ulParameterLengthInOut,
            0, 0, &ulDataLengthInOut);
#ifdef  DEBUG_PC2
        if(rc) printf("     DosDevIOCtl to deactivate sessmgr failed %x\n", (int)rc);
#endif  /* DEBUG_PC2 */
        pHP->hwndLockup=WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP,
            LD_DialogProcedure, pHP->hDLLPc2Resource, LDID_LOCKUPDIALOG, &ulCreateParams);
        if(pHP->hwndLockup==NULLHANDLE)
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Creation of a dialog box failed - continuing...");
        else
            {
            iTimerTicks=0;
            CenterWindow(pHP->hwndLockup);
            WinQueryWindowPos(pHP->hwndLockup, &swpLockup);
            WinProcessDlg(pHP->hwndLockup);
            WinDestroyWindow(pHP->hwndLockup);
            }
                                        /* Reset Lockup dialog detection window handle */
        pHP->hwndLockup=NULLHANDLE;
                                        /* Enable session manager keys again */
        memset(&PPF, 0, sizeof(PPF));
        PPF.idHotKey=-1;
        ulParameterLengthInOut=sizeof(PPF);
        ulDataLengthInOut=0;
        rc=DosDevIOCtl(hfileKeyBoard, IOCTL_KEYBOARD, KBD_SETSESMGRHOTKEY,
            &PPF, sizeof(PPF), &ulParameterLengthInOut,
            0, 0, &ulDataLengthInOut);
#ifdef  DEBUG_PC2
        if(rc) printf("     DosDevIOCtl to activate sessmgr failed %x\n", (int)rc);
#endif  /* DEBUG_PC2 */
                                        /* Close keyboard again */
        rc=DosClose(hfileKeyBoard);
#ifdef  DEBUG_PC2
        if(rc) printf("     DosClose KBD$ failed: %x\n", (int)rc);
#endif  /* DEBUG_PC2 */
        bLockupActive=FALSE;
        }
    else if((BOOL)mp1==TRUE)
        {
        double  dOffsetX;
        double  dOffsetY;

                                        /* When no user activity is shown, move Lockup dialog
                                           around to prevent screen burn-in (though that is
                                           unlikely with today's displays). For a "finer" movement
                                           we increment angle by 1/5 degree */
        if(iTimerTicks>=(360*5))
            iTimerTicks=0;
        dOffsetX=(pHP->swpScreen.cx-swpLockup.cx)>>1;
        dOffsetY=(pHP->swpScreen.cy-swpLockup.cy)>>1;
        dOffsetX+=(sin((2*(float)iTimerTicks/5)*2*3.1415926535/360))*dOffsetX;
        dOffsetY+=(sin((3*(float)iTimerTicks/5)*2*3.1415926535/360))*dOffsetY;
        swpLockup.fl=SWP_MOVE;
        swpLockup.x=(int)dOffsetX;
        swpLockup.y=(int)dOffsetY;
        WinSetMultWindowPos(pHP->habPc2, &swpLockup, 1);
        iTimerTicks++;
        }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_QUITTHREAD, ULONG ulThreadId, NULL                                        *
\*                                                                                      */
case WM_QUITTHREAD:
/*                                                                                      *\
 * All threads post this message when they are about to terminate. This allows us - the *
 * main thread - to wait for all working threads to close.                              *
\*                                                                                      */
    {
                                        /* Count the number of working threads closed */
    static ULONG    ulClosedThreads=0;

#ifdef  DEBUG
    printf("PC2: WM_QUITTHREAD from thread 0x%08X\n", (int)mp1);
#endif  /* DEBUG */
    ulClosedThreads|=1<<((ULONG)mp1-1);
                                        /* Register that a thread has closed */
    if(ulClosedThreads==((1<<(ID_IOTHREAD-1))|(1<<(ID_NETTHREAD-1))|(1<<(ID_WORKINGTHREAD-1))))
        {
                                        /* Stop the timer */
        if(WinStopTimer(pHP->habPc2, hwnd, ulTimer)==FALSE)
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_PC2TIMER, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Configuration of a timer failed - continuing...");
                                        /* Now that we're done, main thread informs itself to finalize
                                           with a WM_QUIT */
        WinPostMsg(hwnd, msg, MPFROMLONG(ID_MAINTHREAD), NULL);
        }
                                        /* We expect all threads to have terminated to exit the
                                           message loop by a WM_QUIT */
    if(ulClosedThreads==((1<<(ID_MAINTHREAD-1))|(1<<(ID_IOTHREAD-1))|(1<<(ID_NETTHREAD-1))|(1<<(ID_WORKINGTHREAD-1))))
                                        /* And here WM_QUIT comes */
        WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
    else
                                        /* Hide ourselfes */
        WinShowWindow(pHP->hwndFrame, FALSE);
    }
    break;

case WM_CLOSE:
    {
    static ULONG    ulCloseRequests=0;

#ifdef  DEBUG_PC2
    printf("PC2: WM_CLOSE\n");
#endif  /* DEBUG_PC2 */
    if(ulCloseRequests>=1)
        {
        return((MRESULT)TRUE);
        }
    ulCloseRequests++;
                                        /* When an internal processiong failure occured, that
                                           requires immediate shutdown of PC/2, the flag
                                           PC2EMERGENCYEXIT is set, and a WM_QUIT message is posted,
                                           which breaks us out of the message loop */
    if(!(pHP->ulRuntimeFlag & (PC2EMERGENCYEXIT|QUITFROMSHUTDOWN)))
        {
                                        /* When running as WPS or PROTSHELL, PC/2 has already disabled
                                           the menuentries to exit PC/2, so we don't need to touch
                                           them. */
        if(!(pHP->ulRuntimeFlag & (PC2RUNNINGASWPS|PC2RUNNINGASPROTSHELL)))
            {
                                        /* Disable menuitem during dialog display */
            WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_EXIT, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_ICONEXIT, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
            }
                                        /* Verify closing request, unless we're going to be shut down by
                                           OS/2 shutdown processing */
        if(!(pHP->ulRuntimeFlag & QUITFROMSHUTDOWN))
            {
            ULONG   ulCreateParams=FALSE;
            ULONG   ulSecurityResult=DID_OK;

                                        /* If we have found some cache paths to load the exectables icons
                                           inform the user, that not saving this will not improve the time
                                           to load the icons next time PC/2 is run */
            if(pHP->ulRuntimeFlag & ICONCACHEACTIVE)
                USR_ERR(pHP->hwndFrame, HELP_ICONCACHE, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "PC/2 has found some smart caches, that would improve load time for the menuentries' icons "
                    "the next time you launch PC/2! You may want to save them, by saving the Popup Menu.");
                                        /* When required request Password (but not when exiting via
                                           CTRL+ALT+F3). Clear bit afterwards to protect documented
                                           exit functions to bypass Password query */
            if((pHP->ulStatusFlag2 & PC2SECURITY) &&
                !(pHP->ulRuntimeFlag & PC2HIDDENEXIT))
                if((ulSecurityResult=WinDlgBox(HWND_DESKTOP, pHP->hwndFrame, LD_DialogProcedure,
                    pHP->hDLLPc2Resource, LDID_PASSWORDDIALOG, &ulCreateParams))==DID_ERROR)
                    PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                        "Creation of a dialog box failed - continuing...");
            pHP->ulRuntimeFlag&=(~PC2HIDDENEXIT);
                                        /* Ask the user if he really wants to exit */
            if((ulSecurityResult!=DID_OK) ||
                (USR_ERR(pHP->hwndFrame, 0, MB_QUERY | MB_YESNO | MB_MOVEABLE | MB_DEFBUTTON1,
                    "Are you sure you want to close PC/2?")!=MBID_YES))
                {
                if(!(pHP->ulRuntimeFlag & (PC2RUNNINGASWPS|PC2RUNNINGASPROTSHELL)))
                    {
                                        /* Enable menuitem again */
                    WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                        MPFROM2SHORT(ID_EXIT, TRUE), MPFROM2SHORT(MIA_DISABLED, 0));
                    WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                        MPFROM2SHORT(ID_ICONEXIT, TRUE), MPFROM2SHORT(MIA_DISABLED, 0));
                    }
                                        /* Reset */
                ulCloseRequests--;
                pHP->ulRuntimeFlag&=(~QUITFROMWINDOWLIST);
                return((MRESULT)TRUE);  /* Only exit if OK is pressed */
                }
            else
                                        /* Hide ourselfes */
                WinShowWindow(pHP->hwndFrame, FALSE);
            }
        }
#ifdef  DEBUG
    printf("PC2: WM_CLOSE cleanup processing starting\n");
#endif  /* DEBUG */
                                        /* Remove the existing Titlebar Smarticons. To do this, a message
                                           is broadcasted, and we wait for the timer to confirm that all
                                           other windows have performed their cleanup. If we would continue
                                           to exit the message loop and the program without having ensured
                                           that no process has Titlebar icons, e.g. EPM.EXE would get hanging
                                           in its exitlist processing - I have no idea why, it just is a
                                           complete PM deadlock */
    if(pHP->ulRuntimeFlag & SUBCLASSICONS)
        WinSendMsg(pHP->hwndClient, WM_TITLEBARICONS, (MPARAM)FALSE, NULL);
                                        /* As during WM_TITLEBARICONS processing a message is broadcasted,
                                           ensure cleanup processing starts after that message by posting
                                           another message at the end of the queue */
    WinPostMsg(pHP->hwndClient, WM_CLEANUP, NULL, NULL);
    }
    break;


/*                                                                                      *\
 * Syntax: WM_CLEANUP, NULL, NULL                                                       *
\*                                                                                      */
case WM_CLEANUP:
    {
    MENUDATA    *pMDFree;

#ifdef  DEBUG
    printf("PC2: WM_CLEANUP processing started\n");
#endif  /* DEBUG */
                                            /* Prevent further messages that the user should save the icon
                                               path caches, as we're closing now */
    pHP->ulRuntimeFlag&=(~ICONCACHEACTIVE);
    if(WinReleaseHook(pHP->habPc2, NULLHANDLE, HK_INPUT, (PFN)pInputHook,
        pHP->hDLLPc2Hook)==FALSE)
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_PC2HOOK, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Unhooking the system input queue failed, System ShutDown suggested - exiting...");
                                        /* Release hook of system input queue before accelerator
                                           translation */
    if(WinReleaseHook(pHP->habPc2, NULLHANDLE, HK_PREACCEL, (PFN)pPreAccelHook,
        pHP->hDLLPc2Hook)==FALSE)
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_PC2HOOK, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Unhooking the system input queue before accelerator translation failed, "
            "System ShutDown suggested - exiting...");
                                        /* Release hook of system sendmessage */
    if(WinReleaseHook(pHP->habPc2, NULLHANDLE, HK_SENDMSG, (PFN)pWinSendMsgHook,
        pHP->hDLLPc2Hook)==FALSE)
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_PC2HOOK, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Unhooking the sendmessage dispatcher failed, System ShutDown suggested - exiting...");
                                        /* Tell timer to see if the cleanup processing can continue
                                           (we have to ensure that there are no more Titlebar icons
                                           or we may hang us and other processing into a Exitlist
                                           processing - don't know why, just that it happens */
    pHP->ulRuntimeFlag|=PC2CLEANUP;
    WinInvalidateRect(pHP->hwndClient, NULL, FALSE);
                                        /* Destroy the windows no longer required */
    WinDestroyWindow(pHP->hwndSwitchFrame);
    WinDestroyWindow(pHP->hwndBubbleHelpFrame);
    WinSendMsg(pHP->hwndClient, WM_SESSIONBARUPDATE, MPFROMLONG(SESSIONBAR_DESTROY), NULL);
                                        /* Allow the spooler container window to terminate itself */
    WinDestroyWindow(pHP->hwndSpoolerFrame);
                                        /* Dismiss the Desktop Configuration and Setup dialogs,
                                           to avoid asynchronous deletion of windows and control
                                           structures invoke synchronously - we stay in the same
                                           thread anyway) */
    if(pHP->hwndDesktopDialog)
        WinSendMsg(pHP->hwndDesktopDialog, WM_COMMAND, MPFROMSHORT(DID_CANCEL), MPFROMSHORT(CMDSRC_OTHER));
    if(pHP->hwndConfigDialog)
        WinSendMsg(pHP->hwndConfigDialog, WM_COMMAND, MPFROMSHORT(DID_CANCEL), MPFROMSHORT(CMDSRC_OTHER));
                                        /* Reduce WPS to original size */
    WinPostMsg(pHP->hwndWorkingThread, WM_EXPANDWPS, (MPARAM)FALSE, NULL);
                                        /* Write changes to PC2.INI */
    if(!INIAccess(pHP->ucFilenameINI, FALSE))
        USR_ERR(pHP->hwndFrame, HELP_PC2INI, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "Error writing to PC2.INI - continuing...");
                                        /* Clean up the Popup-Menu's resources (reset PM's MIS_OWNERDRAW
                                           for the MENUITEMs) */
    if(!(pHP->ulRuntimeFlag & NOCLEANUP))
        if(pHP->ulStatusFlag2 & POPUPICON)
            SetItemStyle(pHP->pPopupMenu, FALSE, FALSE);
                                        /* Mark destroyed windows as no longer available */
    pMDFree=pHP->pMenuData;
    pHP->pPopupMenu=pHP->pMenuData=NULL;
                                        /* Destroy the Popup Menu and deallocate its resources (the menuentries'
                                           icons if loaded) */
    if(!(pHP->ulRuntimeFlag & NOCLEANUP))
        WinPostMsg(pHP->hwndIOThread, WM_POPUPMENUDELETE, MPFROMP(pMDFree), MPFROMSHORT(FALSE));
                                        /* If we fill the Popup Menu from our I/O thread, but created
                                           the menu from the main thread, this vialotes PM design, and
                                           WinDestroyWindow() would hang PM, though everything else works.
                                           So let's terminated without WinDestroyWindow() in that case.
                                           Even if we set the owner to HWND_OBJECT, this may take a
                                           while blocking the SIQ with high CPU requirements */
    if(!(pHP->ulRuntimeFlag & NOCLEANUP))
        {
        WinSetOwner(pHP->hwndPopupMenu, HWND_OBJECT);
        WinDestroyWindow(pHP->hwndPopupMenu);
        }
    pHP->hwndSwitchFrame=pHP->hwndSwitchClient=pHP->hwndSessionFrame=pHP->hwndSessionClient=
        pHP->hwndSpoolerFrame=pHP->hwndSpoolerClient=
        pHP->hwndBubbleHelpFrame=pHP->hwndBubbleHelpClient=
        pHP->hwndDesktopDialog=pHP->hwndConfigDialog=pHP->hwndPopupMenu=NULLHANDLE;
    }
    break;

/*                                                                                      *\
 * Syntax: WM_POPUPMENUREADY, NULL, NULL                                                *
\*                                                                                      */
case WM_POPUPMENUREADY:
/*                                                                                      *\
 * Posted by the I/O thread after the Popup Menu's menuentries have been updated.       *
 * The loading or unloading the menuentries icons may be a very time consuming task so  *
 * we let the I/O thread do this to prevent an unresponsive system                      *
\*                                                                                      */
#ifdef  DEBUG_PC2
    printf("PC2: WM_POPUPMENUREADY\n");
#endif  /* DEBUG_PC2 */
                                        /* If we have found some cache paths to load the exectables icons
                                           inform the user, that not saving this will not improve the time
                                           to load the icons next time PC/2 is run */
    if((pHP->hwndConfigDialog==NULLHANDLE) &&
        (pHP->ulRuntimeFlag & ICONCACHEACTIVE) &&
        (pHP->ulStatusFlag2 & POPUPICON) &&
        (!(pHP->ulRuntimeFlag & (PC2EMERGENCYEXIT|QUITFROMWINDOWLIST|QUITFROMSHUTDOWN))))
        USR_ERR(pHP->hwndFrame, HELP_ICONCACHE, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            "PC/2 has found some smart caches, that would improve load time for the menuentries' icons "
            "the next time you launch PC/2! You may want to save them, by saving the Popup Menu.");
    pHP->ulRuntimeFlag&=(~POPUPMENUUPDATE);
    break;

case HM_ERROR:
    {
                                        /* For OS/2 2.xx we may get HM_ERROR messages even if the
                                           help panel could be displayed for the titlebar control help
                                           smarticon */
    if(!((pHP->ulVersionMajor==OS2_MAJOR) &&
                                        /* As OS2_MINOR_200 is 0 any ULONG value is >= 0, so the
                                           comparism is always true, so in order to prevent a compiler
                                           waring
        (pHP->ulVersionMinor>=OS2_MINOR_200) &&
                                           above line should be disabled */
        (pHP->ulVersionMinor<=OS2_MINOR_211)))
        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEWINDOW, MB_INFORMATION|MB_OK|MB_MOVEABLE|MB_DEFBUTTON1,
            "Help request failed - continuing...");
    break;
    }

/*                                                                                      *\
 * Syntax: WM_SEAMLESSADJUST, HWND hwndSeamless, NULL                                   *
 *         WM_SEAMLESSADJUST, (SHORT sSeamlessX, SHORT sSeamlessY),                     *
 *                            (SHORT sSeamlessCX, SHORT sSeamlessCY)                    *
\*                                                                                      */
case WM_SEAMLESSADJUST:
/* Posted by PC2HOOK.DLL to support seamless WIN-OS2 sessions with HW panning support   *
 * and to adjust the seamless window's size and position to support the SessionBar.     *
 * As a seamless WIN-OS2 session consists of a "shadow" PM window, but the graphics     *
 * output seems to be located at a "lower" GRE level, we can't adjust the window's size *
 * and position during WM_WINDOWPOSCHANGED, as this only changes the "shadow" PM window *
 * but not the output of the "lower" GRE layer. The resulting screen corruption makes   *
 * the system unusuable, therefore we have to post and make the change here.            *
\*                                                                                      */
    {
    static LONG lX;
    static LONG lY;
    static LONG lCX;
    static LONG lCY;
    HWND        hwndSeamless;
    SWP         swpSeamless;


    if(LONGFROMMP(mp2)==0)
        {
        hwndSeamless=HWNDFROMMP(mp1);
        WinQueryWindowPos(hwndSeamless, &swpSeamless);
        swpSeamless.fl=SWP_SIZE|SWP_MOVE;
        swpSeamless.x=lX;
        swpSeamless.y=lY;
        swpSeamless.cx=lCX;
        swpSeamless.cy=lCY;
        swpSeamless.hwnd=hwndSeamless;
        WinSetMultWindowPos(WinQueryAnchorBlock(hwndSeamless), &swpSeamless, 1);
        }
    else
        {
        lX=(LONG)(SHORT)SHORT1FROMMP(mp1);
        lY=(LONG)(SHORT)SHORT2FROMMP(mp1);
        lCX=(LONG)(SHORT)SHORT1FROMMP(mp2);
        lCY=(LONG)(SHORT)SHORT2FROMMP(mp2);
        if(lX<0)
            lX=0;
        if(lY<0)
            lY=0;
        }
    }
    break;

/*                                                                                      *\
 * Syntax: WM_OVERVIEWUNHIDEDELAY, NULL, NULL                                           *
\*                                                                                      */
case WM_OVERVIEWUNHIDEDELAY:
/*                                                                                      *\
 * Posted by I/O Thread to unhide the Overview window as a result of Edge Activation.   *
 * If it is visible already, ensure it gets on top of Z-Order.                          *
\*                                                                                      */
    if(WinIsWindowVisible(pHP->hwndFrame)==FALSE)
        {
        ulEdgeActivation=TRUE;
        WinPostMsg(pHP->hwndWorkingThread, WM_QUICKSWITCH, MPFROMLONG(QUICKSWITCH_SWITCHTO), MPFROMHWND(pHP->hwndFrame));
        }
    else
        {
        SWP     swp;

        memset(&swp, 0, sizeof(swp));
        swp.fl=SWP_ZORDER;
        swp.hwndInsertBehind=HWND_TOP;
        swp.hwnd=pHP->hwndFrame;
        WinSetMultWindowPos(pHP->habPc2, &swp, 1);
        }
    break;

case WM_FOCUSCHANGE:
/*                                                                                      *\
 * If the Oveview window loses the input focus and it had been previously activated     *
 * through the Edge Activation, then hide it again (unless the window gaining the focus *
 * is part of the Overview frame window).                                               *
\*                                                                                      */
    if((SHORT1FROMMP(mp2)==FALSE) &&
        (ulEdgeActivation==TRUE) &&
        (HWNDFROMMP(mp1)!=pHP->hwndClientMenu) &&
        (WinQueryWindow(HWNDFROMMP(mp1), QW_PARENT)!=pHP->hwndFrame))
        {
        ulEdgeActivation=FALSE;
        WinShowWindow(pHP->hwndFrame, FALSE);
        }
    return((MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2));

case WM_SYSCOMMAND:
    if(LONGFROMMP(mp1)==SC_CLOSE)
        WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_EXIT), MPFROMSHORT(CMDSRC_MENU));
    return((MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2));

case WM_COMMAND:
    {
    USHORT      command;

    command=SHORT1FROMMP(mp1);          /* Extract the command value */
/*                                                                                      *\
 * Filter the IDs of the user defined items of the Popup-Menu. If one is found, call    *
 * SearchItem() to search for the corresponding MENUDATA structure, copy it to a        *
 * SESSIONDATA structure and start the session.                                         *
\*                                                                                      */
    if((command>=USERITEMFIRST) && (command<=USERITEMLAST))
        {
        SESSIONDATA     SessionData;    /* Used by Menu Installation dialog and by
                                           Program Installation dialog to store menu or
                                           program data, to be filled from the user or
                                           to be presented to the user. */
        ULONG           id=(ULONG)command;
        MENUDATA        *pMD=NULL;

                                        /* Search in the linked list for this entry */
        if((pMD=SearchItem(pHP->pPopupMenu, &id))!=NULL)
            while(pMD->Item==ENTRYMENUITEM)
                {
                ULONG   ulCreateParams=FALSE;

                                        /* Display Access Password dialog when security is enabled */
                if(pMD->PgmControl & SSF_CONTROL_PASSWORD)
                    {
                    ULONG   ulResult;

                    ulResult=(ULONG)WinDlgBox(HWND_DESKTOP, pHP->hwndFrame, LD_DialogProcedure,
                        pHP->hDLLPc2Resource, LDID_PASSWORDDIALOG, &ulCreateParams);
                    if(ulResult==DID_ERROR)
                        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                            "Creation of a dialog box failed - continuing...");
                    if(ulResult!=DID_OK)
                        break;
                    }
                                        /* Load SessionData with MENUDATA structure */
                LoadMenuData2SessionData(pMD, &SessionData);
                                        /* If user defined size and position defined
                                           set SWP_MOVEWINDOW flag, which invokes a
                                           reposition of the window, the first time the
                                           started application's window is found. The flag
                                           will be reset afterwards */
                if(pMD->PgmControl & SSF_CONTROL_SETPOS)
                    {                   /* If have to move the window create it invisible
                                           first to avoid drawing of window before movement.
                                           If the window should be not invisible set flag
                                           SWP_MOVEWINDOWVISIBLE to show window after movement.
                                           The flag will be reset afterwards. */
                    if(pMD->PgmControl & SSF_CONTROL_INVISIBLE)
                        pMD->SwpFlag|=SWP_MOVEWINDOW;
                    else
                        pMD->SwpFlag|=(SWP_MOVEWINDOW | SWP_MOVEWINDOWVISIBLE);
                                        /* Create window invisible before movement */
                    SessionData.PgmControl|=SSF_CONTROL_INVISIBLE;
                    }
                                        /* If the session should be started not in background
                                           switch PC/2 into foreground, because only the foreground
                                           application can start another session in foreground */
                if(!(pMD->FgBg & SSF_FGBG_BACK))
                    {
                                        /* First, try to activate PC/2 without changing the Z-Order
                                           (that is the Overview window gets popped up), only if
                                           that fails, try to switch to secondly */
                    if(WinFocusChange(HWND_DESKTOP, pHP->hwndFrame, FC_NOSETACTIVE)==FALSE)
                        WinSwitchToProgram(WinQuerySwitchHandle(pHP->hwndFrame, pHP->pPib->pib_ulpid));
                    }
                                        /* Start the session */
                StartSession(&SessionData, pMD);
                                        /* If the session was started in background by a Hotkey, try
                                           to activate the top level window that was active
                                           before PC/2 got active */
                if((pMD->FgBg & SSF_FGBG_BACK) &&
                    (pMD->KeyData.usCh!=0) &&
                    (pHP->hwndActiveFrameWindow!=NULLHANDLE))
                    {
                    WinFocusChange(HWND_DESKTOP,
                        WinWindowFromID(pHP->hwndActiveFrameWindow, FID_CLIENT), FC_NOSETACTIVE);
                    WinPostMsg(pHP->hwndActiveFrameWindow, WM_ACTIVATE,
                        MPFROMSHORT(TRUE), MPFROMHWND(WinWindowFromID(pHP->hwndActiveFrameWindow, FID_TITLEBAR)));
                    pHP->hwndActiveFrameWindow=NULLHANDLE;
                    }
                                        /* Free copies in SESSIONDATA */
                free(SessionData.PgmDosSettings);
                free(SessionData.PgmLeadingEnvironment);
                free(SessionData.PgmTrailingEnvironment);
                free(SessionData.EnvironmentSpaces);
                                        /* Because a working directory may have been set change
                                           to root directory of all local drives */
                WinPostMsg(pHP->hwndWorkingThread, WM_SETDRIVEROOT, NULL, NULL);
                break;
                }
        break;                          /* We don't need further testing for this command */
        }
/*                                                                                      *\
 * Filter the IDs of the Overview client window Popup-Menu to toggle Overview window    *
 * SmartIcons between enabled and disabled.                                             *
\*                                                                                      */
    if((command>=ID_OVICONEXIT) && (command<=ID_OVICONHELP))
        {
        USHORT  usState;
        ULONG   ulShiftMask;

        usState=(ULONG)WinSendMsg(pHP->hwndClientMenu, MM_QUERYITEMATTR,
            MPFROM2SHORT(command, TRUE), MPFROMSHORT(MIA_CHECKED));
        usState^=MIA_CHECKED;
        WinSendMsg(pHP->hwndClientMenu, MM_SETITEMATTR,
            MPFROM2SHORT(command, TRUE), MPFROM2SHORT(MIA_CHECKED, usState));
        for(ulShiftMask=0x00000001; command>ID_OVICONEXIT; command--)
            ulShiftMask<<=1;
        pHP->ulOVSmartIconsFlag^=ulShiftMask;
                                        /* Change SmartIcons, by removing and adding a new
                                           instance */
        if(pHP->ulOverviewFCF & FCF_MENU)
            {
            WinPostMsg(hwnd, WM_OVERVIEWFCF, MPFROMLONG(FCF_MENU), MPFROMLONG(TRUE));
            WinPostMsg(hwnd, WM_OVERVIEWFCF, MPFROMLONG(FCF_MENU), MPFROMLONG(TRUE));
            }
        break;
        }
/*                                                                                      *\
 * Process Overview window SmartIcons messages.                                         *
\*                                                                                      */
    pHP->hwndMenu=WinWindowFromID(pHP->hwndFrame, FID_MENU);
    switch(command)
    {
                                        /* Test for messages sent from smarticons */
    case ID_ICONEXIT:
        if(usMenuCommand!=ID_ICONMOVE)
            {
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(usMenuCommand, TRUE), MPFROM2SHORT(MIA_FRAMED|MIA_HILITED, 0));
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_ICONMOVE, TRUE), MPFROM2SHORT(MIA_FRAMED|MIA_HILITED, MIA_FRAMED|MIA_HILITED));
            }
        usMenuCommand=ID_ICONMOVE;
                                        /* Post exit to PC/2 */
        WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_EXIT), MPFROMSHORT(CMDSRC_MENU));
        break;

    case ID_ICONMOVE:
    case ID_ICONHIDE:
    case ID_ICONZORDERTOP:
    case ID_ICONZORDERBOTTOM:
    case ID_ICONCLOSE:
    case ID_ICONMAXIMIZE:
    case ID_ICONMINIMIZE:
    case ID_ICONRESTORE:
        if(usMenuCommand!=command)
            {
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(usMenuCommand, TRUE), MPFROM2SHORT(MIA_FRAMED|MIA_HILITED, 0));
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(command, TRUE), MPFROM2SHORT(MIA_FRAMED|MIA_HILITED, MIA_FRAMED|MIA_HILITED));
            }
        usMenuCommand=command;
        break;

    case ID_ICONSHUTDOWN:
        if(usMenuCommand!=ID_ICONMOVE)
            {
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(usMenuCommand, TRUE), MPFROM2SHORT(MIA_FRAMED|MIA_HILITED, 0));
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_ICONMOVE, TRUE), MPFROM2SHORT(MIA_FRAMED|MIA_HILITED, MIA_FRAMED|MIA_HILITED));
            }
        usMenuCommand=ID_ICONMOVE;
                                        /* Post ShutDown OS/2 to PC/2 */
        WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_SHUTDOWN), MPFROMSHORT(CMDSRC_MENU));
        break;

    case ID_ICONPOWERDOWN:
        if(usMenuCommand!=ID_ICONMOVE)
            {
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(usMenuCommand, TRUE), MPFROM2SHORT(MIA_FRAMED|MIA_HILITED, 0));
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_ICONMOVE, TRUE), MPFROM2SHORT(MIA_FRAMED|MIA_HILITED, MIA_FRAMED|MIA_HILITED));
            }
        usMenuCommand=ID_ICONMOVE;
                                        /* Post PowerDown OS/2 to PC/2 */
        WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_POWERDOWN), MPFROMSHORT(CMDSRC_MENU));
        break;

    case ID_ICONLOCKUP:
        WinPostMsg(pHP->hwndClient, WM_PC2LOCKUP, NULL, NULL);
        break;

    case ID_ICONHELP:
        if(usMenuCommand!=ID_ICONMOVE)
            {
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(usMenuCommand, TRUE), MPFROM2SHORT(MIA_FRAMED|MIA_HILITED, 0));
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_ICONMOVE, TRUE), MPFROM2SHORT(MIA_FRAMED|MIA_HILITED, MIA_FRAMED|MIA_HILITED));
            }
        usMenuCommand=ID_ICONMOVE;
                                        /* Post help to PC/2 */
        WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_HELP), MPFROMSHORT(CMDSRC_MENU));
        break;

    case ID_HELP:                       /* Display general help panel */
                                        /* Activate ourselves, because otherwise the help
                                           window gets attached to the active window when
                                           help is being invoked from PC/2's Popup Menu via
                                           the keyboard, and if the active window is none of
                                           PC/2's windows, we may run into troubles */
        WinSwitchToProgram(WinQuerySwitchHandle(pHP->hwndFrame, pHP->pPib->pib_ulpid));
        if(pHP->hwndHelp!=NULLHANDLE) WinSendMsg(
            pHP->hwndHelp,              /* Help window */
            HM_DISPLAY_HELP,            /* Display a help panel */
            MPFROMSHORT(ID_HELP),       /* Panel ID in ressource file */
            HM_RESOURCEID);             /* MP1 points to the help window identity */
        break;


    case ID_CONFIGDIALOG:
    case ID_DESKTOPDIALOG:
        {
        ULONG   ulResult=DID_OK;
        ULONG   ulCreateParams=FALSE;

                                        /* Display Password dialog when security is enabled */
        if(pHP->ulStatusFlag2 & PC2SECURITY)
            {
            if((ulResult=WinDlgBox(HWND_DESKTOP, pHP->hwndFrame, LD_DialogProcedure,
                pHP->hDLLPc2Resource, LDID_PASSWORDDIALOG, &ulCreateParams))==DID_ERROR)
                PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "Creation of a dialog box failed - continuing...");
            }
        if(ulResult!=DID_OK)
            break;
                                        /* Popup menuitem Configure Menu selected */
        if(command==ID_CONFIGDIALOG)
            {
                                        /* Prevent multiple activation of this dialog */
            static ULONG    ulConfigDialogActive=FALSE;

            if(ulConfigDialogActive==TRUE)
                break;
                                        /* Disable menuitem during dialog display */
            WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_CONFIGDIALOG, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
            ulConfigDialogActive=TRUE;
                                        /* Start Configure PC/2 dialog box */
            pHP->hwndConfigDialog=WinLoadDlg(
                HWND_DESKTOP,           /* DESKTOP is parent */
                HWND_DESKTOP,           /* DESKTOP is owner */
                CD_DialogProcedure,     /* Dialog procedure of Program Installation
                                           dialog */
                pHP->hDLLPc2Resource,   /* Ressource from file */
                CDID_CONFIGDIALOG,      /* ID of Configure PC/2 dialog */
                0);
            if(!pHP->hwndConfigDialog)
                PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "Creation of a dialog box failed - continuing...");
            else
                {
                WinProcessDlg(pHP->hwndConfigDialog);
                if(WinDestroyWindow(pHP->hwndConfigDialog))
                    pHP->hwndConfigDialog=NULLHANDLE;
                }
                                        /* Enable menuitem again */
            ulConfigDialogActive=FALSE;
            WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_CONFIGDIALOG, TRUE), MPFROM2SHORT(MIA_DISABLED, 0));
            }
                                        /* Popup menuitem Configure Desktop selected */
        else if(command==ID_DESKTOPDIALOG)
            {
            ULONG               ulStatusFlag2Previous;
            ULONG               ulTitlebarFlagPrevious;
            ULONG               ulIconScalePrevious;
            ULONG               ulSessionBarScalePrevious;
            DESKTOPDIALOGPARAMS desktopdialogParams={ sizeof(DESKTOPDIALOGPARAMS), DD_PAGE_1A, 0};

                                        /* Ok, if the source of the dialog activation is an accelerator
                                           key, then change the initial page to be displayed by the
                                           Desktop dialog (CMDSRC_ACCELERATOR should not appear unless
                                           we especially built such a message which we do when we
                                           detect that no user information is available) */
            if(SHORT1FROMMP(mp2)==CMDSRC_ACCELERATOR)
                {
                desktopdialogParams.usTurnToPage=SHORT2FROMMP(mp2);
                desktopdialogParams.usMessageBox=HELP_PC2REGISTRATION;
                }
                                        /* Save POPUPICON flag in order to avoid unnecessary loading
                                           of the menuentries icons. Save the reqested size too */
            ulStatusFlag2Previous=pHP->ulStatusFlag2;
            ulTitlebarFlagPrevious=pHP->ulTitlebarFlag;
            ulIconScalePrevious=pHP->ulIconScale;
            ulSessionBarScalePrevious=pHP->ulSessionBarScale;
                                        /* Disable menuitem during dialog display */
            WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_DESKTOPDIALOG, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
            pHP->hwndDesktopDialog=WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP, DD_DialogProcedure,
                pHP->hDLLPc2Resource, DDID_DESKTOPDIALOG, &desktopdialogParams);
            if(!pHP->hwndDesktopDialog)
                PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "Creation of a dialog box failed - continuing...");
            else
                {
                WinProcessDlg(pHP->hwndDesktopDialog);
                if(WinDestroyWindow(pHP->hwndDesktopDialog))
                    pHP->hwndDesktopDialog=NULLHANDLE;
                }
                                        /* Check for OverView and SessionBar windows to stay
                                           on top of Z-Order */
            if(pHP->ulStatusFlag1 & KEEPONTOPOV)
                pHP->hwndZOrder[ZORDER_OVERVIEW]=pHP->hwndFrame;
            else
                pHP->hwndZOrder[ZORDER_OVERVIEW]=NULLHANDLE;
            if(pHP->ulStatusFlag2 & KEEPONTOPSB)
                pHP->hwndZOrder[ZORDER_SESSIONBAR]=pHP->hwndSessionFrame;
            else
                pHP->hwndZOrder[ZORDER_SESSIONBAR]=NULLHANDLE;
                                        /* Depending on whether the user wants the Popup-Menu to be
                                           ownerdrawn (to include the menuentry's icon) or not, we
                                           set or unset the MIS_OWNERDRAW style to all menuentries, PM
                                           then automagically does the required recalculations. We just
                                           have to update our information to correctly calculate the
                                           center of the Popup-Menu when required by displaying the
                                           Popup-Menu at a now visible position for a moment */
            if(pHP->ulStatusFlag2 & POPUPICON)
                {
                if(!(ulStatusFlag2Previous & POPUPICON))
                    {
                    pHP->ulRuntimeFlag|=POPUPMENUUPDATE;
                                        /* Update PM's MENUITEM structures corresponding to our
                                           Popup-Menu and our MENUDATA structures afterwards */
                    SetItemStyle(pHP->pPopupMenu, FALSE, TRUE);
                    WinPostMsg(pHP->hwndIOThread, WM_POPUPMENUUPDATE,
                        MPFROMP(pHP->pPopupMenu), MPFROMSHORT(TRUE));
                    }
                if(pHP->ulIconScale!=ulIconScalePrevious)
                    WinSetPresParam(pHP->hwndPopupMenu, PP_FONTNAMESIZE,
                        sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
                WinPopupMenu(HWND_DESKTOP, hwnd, pHP->hwndPopupMenu,
                        (pHP->DesktopSize.x<<3), 0, 0, PU_NONE);
                WinShowWindow(pHP->hwndPopupMenu, FALSE);
                }
            else
                {
                if(ulStatusFlag2Previous & POPUPICON)
                    {
                    pHP->ulRuntimeFlag|=POPUPMENUUPDATE;
                                        /* Update PM's MENUITEM structures corresponding to our
                                           Popup-Menu and our MENUDATA structures afterwards */
                    SetItemStyle(pHP->pPopupMenu, FALSE, FALSE);
                    WinPostMsg(pHP->hwndIOThread, WM_POPUPMENUUPDATE,
                        MPFROMP(pHP->pPopupMenu), MPFROMSHORT(FALSE));
                    }
                }
            WinPopupMenu(HWND_DESKTOP, hwnd, pHP->hwndPopupMenu,
                    (pHP->DesktopSize.x<<3), 0, 0, PU_NONE);
            WinShowWindow(pHP->hwndPopupMenu, FALSE);
                                        /* Enable menuitem again */
            WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_DESKTOPDIALOG, TRUE), MPFROM2SHORT(MIA_DISABLED, 0));
                                        /* Depening on whether the user wants the SessionBar, create,
                                           resize or destroy SessionBar */
            if(pHP->ulStatusFlag2 & SHOWSESSIONBAR)
                {
                if(!(ulStatusFlag2Previous & SHOWSESSIONBAR))
                    WinPostMsg(pHP->hwndClient, WM_SESSIONBARUPDATE, MPFROMLONG(SESSIONBAR_CREATE), NULL);
                else
                    WinPostMsg(pHP->hwndClient, WM_SESSIONBARUPDATE, MPFROMLONG(SESSIONBAR_RESIZE), NULL);
                }
            else
                WinPostMsg(pHP->hwndClient, WM_SESSIONBARUPDATE, MPFROMLONG(SESSIONBAR_DESTROY), NULL);
                                        /* If the Titlebar Smarticons settings have change, make
                                           the change to all titlebars */
            if((pHP->ulStatusFlag2 & TITLEBARICONS)!=(ulStatusFlag2Previous & TITLEBARICONS))
                WinPostMsg(pHP->hwndClient, WM_TITLEBARICONS, (MPARAM)(pHP->ulStatusFlag2 & TITLEBARICONS), NULL);
            if((pHP->ulStatusFlag2 & TITLEBARICONS) && (pHP->ulTitlebarFlag!=ulTitlebarFlagPrevious))
                {
                WinPostMsg(pHP->hwndClient, WM_TITLEBARICONS, (MPARAM)FALSE, NULL);
                WinPostMsg(pHP->hwndClient, WM_TITLEBARICONS, (MPARAM)TRUE, NULL);
                }
                                        /* If APM is disabled, then disable the PowerDown
                                           smarticons, and the PowerDown OS/2 menuentry, else
                                           enable them */
            WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_POWERDOWN, TRUE), MPFROM2SHORT(MIA_DISABLED, (pHP->ulStatusFlag2 & SUPPORTAPM ? 0 : MIA_DISABLED)));
            WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_ICONPOWERDOWN, TRUE), MPFROM2SHORT(MIA_DISABLED, (pHP->ulStatusFlag2 & SUPPORTAPM ? 0 : MIA_DISABLED)));
            }
        }
        break;

    case ID_SHUTDOWN:                   /* ShutDown OS/2 menuitem selected */
                                        /* Ask the user if he really wants to shut down OS/2,
                                           if he wants to be asked */
        if(!(pHP->ulStatusFlag1 & NOVERIFYSHUTDOWN))
            {
            if(USR_ERR(pHP->hwndFrame, 0, MB_QUERY | MB_YESNO | MB_MOVEABLE | MB_DEFBUTTON1,
                "Are you really sure you want to ShutDown OS/2?")!=MBID_YES)
                                        /* Only shut down if OK is pressed */
                return((MRESULT)TRUE);
            }
                                        /* Write changes to PC2.INI */
        INIAccess(pHP->ucFilenameINI, FALSE);
        if(pHP->ulStatusFlag1 & NORMALSHUTDOWN)
                                        /* If the user requested the shutdown he knows from the
                                           WPS's context menu, do what is requested */
            WinShutdownSystem(pHP->habPc2, pHP->hmqPc2);
        else
            {
            SHUTDOWNPARAMETERS  sdParms={SHUTDOWN_SHUTDOWN};

                                        /* Start ShutDown OS/2 dialog box */
            if(WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, SD_DialogProcedure,
                pHP->hDLLPc2Resource, SDID_SHUTDOWNDIALOG, &sdParms)==DID_ERROR)
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Creation of a dialog box failed - continuing...");
            }
        break;

    case ID_POWERDOWN:                  /* PowerDown OS/2 menuitem selected */
        {
                                        /* Once the PowerDowndialog is active, we may get
                                           further requests, which we will ignore then (for
                                           standby, because for power off we won't be running
                                           many more seconds) */
        static BOOL         bPowerDownActive=FALSE;
        SHUTDOWNPARAMETERS  sdParms={SHUTDOWN_POWERDOWN};

        if (bPowerDownActive==TRUE)
            return((MRESULT)FALSE);
        else while(bPowerDownActive!=TRUE)
            {
            bPowerDownActive=TRUE;
                                        /* First see if APM support was found, because there is
                                           a funny effect that when we open APM$ in the PowerDown
                                           OS/2 dialog and APM$ is not available, all further
                                           OS/2 windows launched from PC/2 will open and immediately
                                           close afterwards. I think this is a bug */
            if(!(pHP->ulRuntimeFlag & APMSUPPORTED))
                {
                USR_ERR(pHP->hwndFrame, HELP_POWERDOWN, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "PC/2 can't detect APM support!");
                break;
                }
                                        /* Ask the user if he really wants to suspend or power
                                           off shut down OS/2, if he wants to be asked */
            if(!(pHP->ulStatusFlag2 & NOVERIFYPOWERDOWN))
                {
                                        /* Only suspend or power off if OK is pressed */
                if(USR_ERR(pHP->hwndFrame, 0, MB_QUERY | MB_YESNO | MB_MOVEABLE | MB_DEFBUTTON1,
                    "Are you really sure you want to suspend or power off your PC?")!=MBID_YES)
                    break;
                }
                                        /* Write changes to PC2.INI */
            INIAccess(pHP->ucFilenameINI, FALSE);
                                        /* Start PowerDown OS/2 dialog box */
            if(WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, SD_DialogProcedure,
                pHP->hDLLPc2Resource, SDID_SHUTDOWNDIALOG, &sdParms)==DID_ERROR)
                {
                PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "Creation of a dialog box failed - continuing...");
                break;
                }
            bPowerDownActive=FALSE;
            return((MRESULT)TRUE);
            }
        }
        break;

    case ID_EXIT:                       /* User selected F3 to shutdown PC/2, selected Exit PC/2
                                           from the Popup-Menu or selected the Exit smarticon. */

                                        /* If running as the WPS replacement, don't exit */
        if(!(pHP->ulRuntimeFlag & (PC2RUNNINGASWPS|PC2RUNNINGASPROTSHELL)))
            {
                                        /* Because WM_CLOSE is called, PC/2 is shutdown in
                                           an orderly manner, so we can accept the next WM_QUIT
                                           (a WM_QUIT terminates the message loop) message,
                                           posted during WM_CLOSE immediately when the user really
                                           wants to quit, to terminate PC/2 without any way to
                                           return then, by setting QUITFROMWINDOWLIST */
            pHP->ulRuntimeFlag|=QUITFROMWINDOWLIST;
            WinPostMsg(hwnd, WM_CLOSE, 0, 0);
            }
        break;

    case ID_EMERGENCYEXIT:              /* User selected CTRL+ALT+F3 to exit in an undocumented way,
                                           which is useful for testing or debugging when running
                                           as WPS replacement */
        pHP->ulRuntimeFlag|=(QUITFROMWINDOWLIST|PC2HIDDENEXIT);
        WinPostMsg(hwnd, WM_CLOSE, 0, 0);
        break;

    case ID_ABOUTDIALOG:                /* User selected About PC/2 dialog */
                                        /* Disable menuitem during dialog display */
        WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
            MPFROM2SHORT(ID_ABOUTDIALOG, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
        if(WinDlgBox(                   /* Start About PC/2 dialog box */
            HWND_DESKTOP, HWND_DESKTOP, AD_DialogProcedure, pHP->hDLLPc2Resource,
            ADID_ABOUTDIALOG, 0)==DID_ERROR)
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Creation of a dialog box failed - continuing...");
                                        /* Enable menuitem again */
        WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
            MPFROM2SHORT(ID_ABOUTDIALOG, TRUE), MPFROM2SHORT(MIA_DISABLED, 0));
        break;

    case ID_SPOOLER:                    /* User selected PC/2 spooler dialog */
        WinPostMsg(pHP->hwndSpoolerClient, WM_SHOWSPOOLER, NULL, NULL);
        break;

    case ID_LOCKUP:
        WinPostMsg(pHP->hwndClient, WM_PC2LOCKUP, NULL, NULL);
        break;

    case ID_DEBUG:                      /* User pressed SysRq key to toggle debug status (CMDSRC_ACCELERATOR)
                                           or its posted to set the titlebar text containing the current
                                           debug level (!CMDSRC_ACCELERATOR) and not toggling the debug status */
        {
        UCHAR   ucOverviewTitlebar[MAXNAMEL+4];
        UCHAR   ucDebugTitlebar[]=" (Debug: ";
        UCHAR   ucDebugLevel[]="0)";

        if(SHORT1FROMMP(mp2)==CMDSRC_ACCELERATOR)
            {
            pHP->ulDebug++;
            if(pHP->ulDebug>DEBUG_FULL) pHP->ulDebug=DEBUG_NO;
            }
        strcpy(ucOverviewTitlebar, PC2_OVERVIEW_WINDOW);
        if(pHP->ulDebug>DEBUG_NO)
            {
            ucDebugLevel[0]=pHP->ulDebug | 0x30;
            strcat(ucOverviewTitlebar, ucDebugTitlebar);
            strcat(ucOverviewTitlebar, ucDebugLevel);
            }
        WinSetWindowText(pHP->hwndFrame, ucOverviewTitlebar);
        }
        break;

    case ID_OVUNHANG:
    case ID_UNHANG:                     /* User pressed SysRq key to unhang the busy working thread. For
                                           unknown reasons (as it is difficult to reproduce and to diagnose
                                           what failed - possibly WinSetMultWindowPos()) sometimes the thread
                                           busy flag doesn't get reset. As a last resort the user may try to
                                           use the unhang key */
        WinSendMsg(hwnd, WM_THREADREADY, MPFROMLONG(THREADWINDOWBUSY), NULL);
        WinSendMsg(hwnd, WM_THREADREADY, MPFROMLONG(THREADMOVEBUSY), NULL);
        break;

    case ID_BREAKPOINT:                 /* User pressed ALT+CTRL+SHIFT+F3 to interrupt PC/2 (useful
                                           to get control when running under the remote debugger beta
                                           or the kernel debugger) */
        _interrupt(3);
        break;

    case ID_OVDUMPPROCESS:              /* Dump PC/2 process */
        DosDumpProcess(DDP_ENABLEPROCDUMP, (ULONG)pHP->ucBootDrive, pHP->pPib->pib_ulpid);
        DosDumpProcess(DDP_PERFORMPROCDUMP, (ULONG)pHP->ucBootDrive, 0);
        break;

    case ID_OVDUMPWTRD:                 /* Dump Working thread */
        DumpThread(pHP->pPib, pHP->pTibWorkingThread, ID_WORKINGTHREAD);
        break;

    case ID_OVDUMPITRD:                 /* Dump I/O thread */
        DumpThread(pHP->pPib, pHP->pTibIOThread, ID_IOTHREAD);
        break;

    case ID_OVDUMPNTRD:                 /* Dump network thread */
        DumpThread(pHP->pPib, pHP->pTibNetThread, ID_NETTHREAD);
        break;

    }
    break;
    }

default:                                /* Default window procedure must be called */
    return((MRESULT)WinDefWindowProc(hwnd, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This dialog procedure handles the PC/2 - Configuration (Setup) dialog.               *
 * Req: none                                                                            *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY CD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static ULONG    ulListboxCursor;        /* Index of the listbox item that has the cursor selection */

switch(msg)
{
case WM_INITDLG:
                                        /* Center frame window */
    CenterWindow(hwndDlg);
                                        /* As messageboxes may be displayed having this window as
                                           owner, associate the main helptable with this dialog */
    WinAssociateHelpInstance(pHP->hwndHelp, hwndDlg);
                                        /* Subclass CDLB_MENUPROGRAM listbox to allow dropping into */
    pfnListboxWindowProc=WinSubclassWindow(WinWindowFromID(hwndDlg, CDLB_MENUPROGRAM),
        SubclassedListboxWindowProc);
/*                                                                                      *\
 * If we have cut a Menuentry into our clipboard, change the cut pushbutton into the    *
 * paste pushbutton.                                                                    *
\*                                                                                      */
    if(pHP->pMenuDataCutPaste!=0)
        {
        HWND    hwndCutPasteButton=WinWindowFromID(hwndDlg, CDPB_CUT);

        WinSetWindowUShort(hwndCutPasteButton, QWS_ID, CDPB_PASTE);
        WinSetWindowText(hwndCutPasteButton, "Past~e");
        }

    ulListboxCursor=0;                  /* Set cursor selection to first entry */
                                        /* Set path information */
    WinPostMsg(hwndDlg, WM_SETUPPATH, NULL, NULL);
                                        /* Initialize the listbox */
    WinSendMsg(hwndDlg, WM_RELOADPOPUPMENU, MPFROMP(pHP->pMenuData), NULL);
/*                                                                                      *\
 * Fill in the Environment Spaces available. The user may than apply to add or remove   *
 * the selected Environment Space(s) to or from the selected menuentry.                 *
\*                                                                                      */
    {
    ENVIRONMENTLIST *pEnvironmentListCurrent;

    for(pEnvironmentListCurrent=pHP->pEnvironmentSpaces;
        pEnvironmentListCurrent!=NULL;
        pEnvironmentListCurrent=pEnvironmentListCurrent->pNext)
        {
        WinSendDlgItemMsg(hwndDlg, CDLB_ENVIRONMENT, LM_INSERTITEM,
            MPFROMSHORT(LIT_END), MPFROMP(pEnvironmentListCurrent->pEnvironmentName));
        }
    }
    break;

case WM_DESTROY:
                                        /* Deassociate help instance */
    WinAssociateHelpInstance(NULLHANDLE, hwndDlg);
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));

/*                                                                                      *\
 * Syntax: WM_RELOADPOPUPMENU, *MENUDATA, NULL                                          *
\*                                                                                      */
case WM_RELOADPOPUPMENU:                /* Load the current level of the Popup-Menu in
                                           the listbox after removing the old items */
    {
    MENUDATA    *pMD;

    pMD=PVOIDFROMMP(mp1);               /* Get the pointer to the first MENUDATA of the
                                           current level */
    WinSendDlgItemMsg(                  /* Send message to listbox */
        hwndDlg,                        /* Handle of dialog window */
        CDLB_MENUPROGRAM,               /* Submenu & Program listbox */
        LM_DELETEALL,                   /* Delete all list box items */
        (MPARAM)NULL,
        (MPARAM)NULL);
    if(pMD==NULL) break;                /* If linked list is empty break out */
    do
    {
        if(pMD->Item==ENTRYSUBMENU)     /* It is a Submenu */
            {
            UCHAR       Buffer[MAXNAMEL+4];
                                        /* Add >> for a Submenu */
            sprintf(Buffer, "%s >>", pMD->PgmTitle);
            WinSendDlgItemMsg(
                hwndDlg,
                CDLB_MENUPROGRAM,
                LM_INSERTITEM,          /* Insert Submenu Title at the end */
                MPFROMSHORT(LIT_END),
                MPFROMP(Buffer));
            }
        if(pMD->Item==ENTRYMENUITEM)    /* It's a Menuitem */
            WinSendDlgItemMsg(
                hwndDlg,
                CDLB_MENUPROGRAM,
                LM_INSERTITEM,          /* Insert Menuitem Title at the end */
                MPFROMSHORT(LIT_END),
                MPFROMP(pMD->PgmTitle));
        if(pMD->Item==ENTRYCONTROL)     /* It's a Control */
            {
            WinSendDlgItemMsg(
                hwndDlg,
                CDLB_MENUPROGRAM,
                LM_INSERTITEM,          /* Insert Control Title at the end */
                MPFROMSHORT(LIT_END),
                MPFROMP(pMD->PgmTitle));
            }
                                        /* It may also be an empty entry, but then we
                                           ignore it, because it must be filled with
                                           Menuitem or Submenu data first */
        if(pMD->Next!=NULL)             /* Get through linked list without diving into
                                           Submenus */
                pMD=pMD->Next;
        else break;                     /* We're at the end of the linked list */
    } while(TRUE);
                                        /* Set cursor selection to first entry */
    WinSendDlgItemMsg(hwndDlg, CDLB_MENUPROGRAM, LM_SETTOPINDEX,
        MPFROMSHORT((USHORT)(ulListboxCursor>7 ? (ulListboxCursor-7) : ulListboxCursor)), MPFROMCHAR(TRUE));
    WinSendDlgItemMsg(hwndDlg, CDLB_MENUPROGRAM, LM_SELECTITEM,
        MPFROMSHORT((USHORT)ulListboxCursor), MPFROMCHAR(TRUE));
    break;
    }

/*                                                                                      *\
 * Syntax: WM_SETUPPATH, NULL, NULL                                                     *
\*                                                                                      */
case WM_SETUPPATH:
/*                                                                                      *\
 * Fill the path of Submenus entered to get to the current Submenu into the current     *
 * location line.                                                                       *
\*                                                                                      */
    {
    HWND        hwndPath=WinWindowFromID(hwndDlg, CDST_PATH);
    SWP         swpPath;
    HPS         hpsPath;
    RECTL       rectlPath;
    POINTL      pointlFnt[TXTBOX_COUNT];
    LONG        lPathLength;
    LONG        lPathChars;
    MENUDATA    *pMD=pHP->pMenuData;
    MENUDATA    *pMDSave;
    UCHAR       ucPath[PATHSIZE]="";
    UCHAR       ucTemp[PATHSIZE]="";

    hpsPath=WinBeginPaint(hwndPath, NULLHANDLE, NULL);
    lPathChars=strlen("(Root)...");
    GpiQueryTextBox(hpsPath, lPathChars, "(Root)...", TXTBOX_COUNT, pointlFnt);
    lPathLength=pointlFnt[TXTBOX_CONCAT].x;
    WinQueryWindowPos(hwndPath, &swpPath);
                                        /* Insert Submenu path sequence */
    while(pMD)
        {
        strcpy(ucTemp, ucPath);
                                        /* Get parent Submenu */
        while(TRUE)
            {
            pMDSave=pMD;
            pMD=pMD->Back;
            if((pMD==NULL) || (pMD->Submenu==pMDSave))
                break;
            }
        if(pMD!=NULL)
            {
                                        /* Copy Submenu title text and query its length. Add
                                           the Submenu if it fits into the static text field,
                                           otherwise just add the a shortcut */
            strcpy(ucPath, "");
            strcat(ucPath, pMD->PgmTitle);
            GpiQueryTextBox(hpsPath, strlen(ucPath), ucPath, TXTBOX_COUNT, pointlFnt);
            lPathLength+=pointlFnt[TXTBOX_CONCAT].x;
            if((lPathLength>swpPath.cx) || (lPathChars+strlen(ucPath)>=PATHSIZE))
                {
                strcpy(ucPath, "(Root)...");
                strcat(ucPath, ucTemp);
                break;
                }
            lPathChars+=strlen(ucPath);
            }
        else
            strcpy(ucPath, "(Root)");
        strcat(ucPath, ucTemp);
        }
    WinEndPaint(hpsPath);
    WinSetDlgItemText(hwndDlg, CDST_PATH, ucPath);
    break;
    }

case WM_HELP:                           /* Help pressed */
    WinSendMsg(
        pHP->hwndHelp,                  /* Help window */
        HM_DISPLAY_HELP,                /* Display a help panel */
        MPFROMSHORT(ID_CONFIGDIALOG),   /* Panel ID in resource file */
        HM_RESOURCEID);                 /* MP1 points to the help window identity */
    break;

case WM_CONTROL:
                                        /* If user doubleclicked on an listbox item process */
    if(SHORT1FROMMP(mp1)==CDLB_MENUPROGRAM)
        switch(SHORT2FROMMP(mp1))
        {
        case LN_ENTER:
            {
            MENUDATA        *pMD;
            SHORT           sCount;

                                        /* Point to the first element of the linked list
                                           at the current level */
            pMD=pHP->pMenuData;
                                        /* Send message to listbox */
            sCount=(LONG)WinSendDlgItemMsg(
                hwndDlg,                /* Handle of dialog window */
                CDLB_MENUPROGRAM,       /* Submenu & Program listbox */
                LM_QUERYSELECTION,      /* Query first selected list box item */
                MPFROMSHORT(LIT_FIRST),
                (MPARAM)NULL);
                                        /* Save cursor selection */
            ulListboxCursor=(ULONG)sCount;
            if(sCount==LIT_NONE)        /* If no item selected ignore this button */
                return((MRESULT)FALSE);
            for( ;sCount>0; sCount--)   /* Walk through the linked list to the selected
                                           item */
                pMD=pMD->Next;
                                        /* For a Menuitem or Menucontrol simulate change entry, for a Submenu
                                           simulate level down */
            if(pMD->Item==ENTRYSUBMENU)
                WinPostMsg(hwndDlg, WM_COMMAND, MPFROM2SHORT(CDPB_LEVELDOWN, CMDSRC_OTHER), (MPARAM)TRUE);
            if((pMD->Item==ENTRYMENUITEM) || (pMD->Item==ENTRYCONTROL))
                WinPostMsg(hwndDlg, WM_COMMAND, MPFROM2SHORT(CDPB_CHANGEENTRY, CMDSRC_OTHER), (MPARAM)TRUE);
            }
            break;
        }
        break;


case WM_COMMAND:                        /* Button pressed */
    switch(SHORT1FROMMP(mp1))
    {
/*                                                                                      *\
 * Query the selected Environment Spaces and apply either a add or remove operation to  *
 * the selected menuentry. Of course Environment Spaces will only be applied where      *
 * appropriate (that is OS/2 executables), but if a Submenu is selected, we will        *
 * recursively chain into this Submenu and apply the selection too.                     *
\*                                                                                      */
    case CDPB_ADDENVIRONMENT:
    case CDPB_REMOVEENVIRONMENT:
        {
        MENUDATA        *pMD;           /* Pointer to a MENUDATA structure to add or remove
                                           Environment Spaces to or from */
        SHORT           sCount;
        UCHAR           *pucEnvironmentSpaces;
        ULONG           ulIndex;
        UCHAR           *pucTemp;
        ULONG           ulLength;

        pMD=pHP->pMenuData;             /* Point to the first element of the linked list
                                           at the current level and query listbox selection */
        sCount=(LONG)WinSendDlgItemMsg(hwndDlg, CDLB_MENUPROGRAM, LM_QUERYSELECTION,
            MPFROMSHORT(LIT_FIRST), (MPARAM)NULL);
                                        /* Walk through the linked list to the selected
                                           item */
        if(sCount==LIT_NONE)
            break;
                                        /* Even this update is done immediately, set
                                           the wait pointer */
        WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE));
        for( ;sCount>0; sCount--)
            pMD=pMD->Next;
                                        /* Query selected Environment Spaces and add them
                                           into a dynamically allocated buffer separated by \r\n */
        pucEnvironmentSpaces=strdup("");
        ulIndex=LIT_FIRST;
        while((ulIndex=(ULONG)WinSendDlgItemMsg(hwndDlg, CDLB_ENVIRONMENT,
            LM_QUERYSELECTION, (MPARAM)ulIndex, NULL))!=(ULONG)LIT_NONE)
            {
            ulLength=(ULONG)WinSendDlgItemMsg(hwndDlg, CDLB_ENVIRONMENT,
                LM_QUERYITEMTEXTLENGTH, (MPARAM)ulIndex, NULL);
            if(ulLength==(ULONG)LIT_ERROR)
                continue;
                                        /* Allocate space to concatenate current Environment Space
                                           including \r\n\0 termination */
            ulLength=strlen(pucEnvironmentSpaces)+ulLength+3;
            pucTemp=malloc(ulLength);
            if(pucTemp==NULL)
                break;
            memset(pucTemp, '\x5a', ulLength);
            strcpy(pucTemp, pucEnvironmentSpaces);
            free(pucEnvironmentSpaces);
            pucEnvironmentSpaces=pucTemp;
            pucTemp+=strlen(pucTemp);
            WinSendDlgItemMsg(hwndDlg, CDLB_ENVIRONMENT, LM_QUERYITEMTEXT,
                MPFROM2SHORT((SHORT)ulIndex, (SHORT)ulLength+1), (MPARAM)pucTemp);
            strcat(pucTemp, "\r\n");
            }
                                        /* Now apply the selected Environment Spaces to the
                                           selected menuentry */
        ApplyEnvironment(pMD, pucEnvironmentSpaces, (ULONG)(SHORT1FROMMP(mp1)));
                                        /* Cleanup */
        free(pucEnvironmentSpaces);
        WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));
        }
        break;

/*                                                                                      *\
 * Chain up the linked list until we find the node, where this part-list comes from or  *
 * the beginning of the complete list. The pointer pMenuData is adjusted.               *
\*                                                                                      */
    case CDPB_LEVELUP:                  /* Get up one level in the linked list */
        {
        MENUDATA        *pMD;           /* Pointer to a MENUDATA structure to find the
                                           Submenu where this part-list starts */

        pMD=pHP->pMenuData;             /* Point to the first element of the linked list
                                           at the current level */
        if(pMD->Back==NULL)             /* If we're at the beginning of the complete linked
                                           list ignore button */
            return((MRESULT)FALSE);
        else pMD=pMD->Back;             /* Submenu which started current level */
                                        /* Now chain back through the linked list and find
                                           the element, where the pointer to a Submenu
                                           equals the back pointer of the first element
                                           in this Submenu. Then we've found the node */
        while(TRUE)
            {
            if(pMD->Back==NULL)         /* If we're now at the beginning break */
                break;
            if((pMD->Back)->Submenu==pMD)
                break;
            else
                pMD=pMD->Back;
            }
        pHP->pMenuData=pMD;             /* Load as the top element of the current item */
                                        /* Set cursor selection */
        ulListboxCursor=0;
                                        /* Now redraw items in listbox */
        WinSendMsg(hwndDlg, WM_RELOADPOPUPMENU, MPFROMP(pHP->pMenuData), NULL);
                                        /* Set path information */
        WinPostMsg(hwndDlg, WM_SETUPPATH, NULL, NULL);
        return((MRESULT)FALSE);         /* We handled this button */
        }

/*                                                                                      *\
 * Test the user selection for being a Submenu. If one found chain into this submenu    *
 * and adjust the pointer pMenuData.                                                    *
\*                                                                                      */
    case CDPB_LEVELDOWN:                /* Get down one level in the linked list */
        {
        MENUDATA        *pMD;           /* Pointer to a MENUDATA structure to find the
                                           Submenu to chain into */
        SHORT           sCount;

        pMD=pHP->pMenuData;             /* Point to the first element of the linked list
                                           at the current level */
                                        /* Send message to listbox */
        sCount=(LONG)WinSendDlgItemMsg(
            hwndDlg,                    /* Handle of dialog window */
            CDLB_MENUPROGRAM,           /* Submenu & Program listbox */
            LM_QUERYSELECTION,          /* Query first selected list box item */
            MPFROMSHORT(LIT_FIRST),
            (MPARAM)NULL);
                                        /* If no item selected, ignore this button */
        if(sCount==LIT_NONE)
            return((MRESULT)FALSE);
        for( ;sCount>0; sCount--)       /* Walk through the linked list to the selected
                                           item */
            pMD=pMD->Next;
        if(pMD->Item!=ENTRYSUBMENU)     /* It's not a Submenu that's selected, ignore */
            return((MRESULT)FALSE);
                                        /* Otherwise chain into this part-list */
        pHP->pMenuData=pMD->Submenu;
                                        /* Set cursor selection */
        ulListboxCursor=0;
                                        /* Now redraw items in listbox */
        WinSendMsg(hwndDlg, WM_RELOADPOPUPMENU, MPFROMP(pHP->pMenuData), NULL);
                                        /* Set path information */
        WinPostMsg(hwndDlg, WM_SETUPPATH, NULL, NULL);
        return((MRESULT)FALSE);         /* We handled this button */
        }

/*                                                                                      *\
 * The user selected to paste a Menuentry from our clipboard. Add the Menuentry after   *
 * the current selection or at the end of the list and reload the PC/2 Configuration    *
 * dialog again.                                                                        *
\*                                                                                      */
    case CDPB_PASTE:                    /* Paste item from our clipboard */
/*                                                                                      *\
 * The user selected to add a (Sub)Menu. Thus keep the PC/2 Configuration dialog and    *
 * load the (Sub)Menu Installation dialog. The new (Sub)Menu is entered in a            *
 * STARTSESSION structure named StartSession. Save the changes and reload the PC/2      *
 * Configuration dialog again.                                                          *
\*                                                                                      */
    case CDPB_ADDMENU:                  /* Add a Menu to PC/2 Configuration selected */
/*                                                                                      *\
 * The user selected to add a Program. Thus keep the PC/2 Configuration dialog and      *
 * load the Menucontrol Addition dialog. The new control style is selected via          *
 * autoradiobuttons and is fille STARTSESSION structure named StartSession. Save the    *
 * changes and reload the PC/2 Configuration dialog again.                              *
\*                                                                                      */
    case CDPB_ADDPROGRAM:               /* Add a Program to PC/2 Configuration selected */
/*                                                                                      *\
 * The user selected to clone a Program. This keep the PC/2 Configuration dialog and    *
 * create a new Program menuentry by cloning the selected one. Then add the new entry   *
 * after the selected one.                                                              *
\*                                                                                      */
    case CDPB_CLONEPROGRAM:             /* Clone a Program */
/*                                                                                      *\
 * The user selected to add a Control Style. Thus dismiss the PC/2 Configuration dialog *
 * and load the Program Installation dialog. The new session data is entered in a       *
 * STARTSESSION structure named StartSession. Save the changes and reload the PC/2      *
 * Configuration dialog again.                                                          *
\*                                                                                      */
    case CDPB_ADDCONTROL:               /* Add a Control Entry to PC/2 Configuration selected */
        {
        SESSIONDATA     SessionData;    /* Used by Menu Installation dialog and by
                                           Program Installation dialog to store menu or
                                           program data, to be filled from the user or
                                           to be presented to the user. */
        MENUDATA        *pMD;           /* Pointer to a MENUDATA structure to insert a
                                           new MENUDATA stucture after */
        MENUDATA        *pMDNew;        /* Temporary pointer for the new item to be inserted
                                           after pMD */
                                        /* Each dialog procedure returns the result with
                                           WinDismissDlg() */
        ULONG           ulResult=DID_CANCEL;
        SHORT           sSelection;

        pMD=pHP->pMenuData;             /* Point to the first element of the linked list
                                           at the current level */
                                        /* Send message to listbox */
        sSelection=(LONG)WinSendDlgItemMsg(
            hwndDlg,                    /* Handle of dialog window */
            CDLB_MENUPROGRAM,           /* Submenu & Program listbox */
            LM_QUERYSELECTION,          /* Query first selected list box item */
            MPFROMSHORT(LIT_FIRST),
            (MPARAM)NULL);
                                        /* Save cursor selection */
        if(sSelection!=LIT_NONE)
            ulListboxCursor=(ULONG)sSelection;
                                        /* If no item selected, and there exists one,
                                           add the new Menuitem after the last available
                                           Menuitem by querying the number from the listbox.
                                           Subtract 0 because we use 0-based instead 1-based. */
        if((sSelection==LIT_NONE) && (pHP->pMenuData->Item!=ENTRYEMPTY))
            sSelection=(LONG)WinSendDlgItemMsg(hwndDlg, CDLB_MENUPROGRAM, LM_QUERYITEMCOUNT,
                MPFROM2SHORT(NULL, NULL), (MPARAM)NULL)-1;
                                        /* Walk through the linked list to the selected
                                           item */
        for( ;sSelection>0; sSelection--)
            pMD=pMD->Next;
                                        /* Allocate a new item. This may either be an empty
                                           MENUDATA structure or one cloned from the selected
                                           one */
        if(SHORT1FROMMP(mp1)==CDPB_CLONEPROGRAM)
            {
            if((sSelection==LIT_NONE) || (!pMD) || (pMD->Item!=ENTRYMENUITEM))
                break;
            pMDNew=AllocateMenuData(pMD);
            pMDNew->id=pHP->MenuDataId++;
            ulResult=DID_OK;
            }
        else
            {
            if(SHORT1FROMMP(mp1)==CDPB_PASTE)
                {
                HWND    hwndCutPasteButton=WinWindowFromID(hwndDlg, CDPB_PASTE);
                ULONG   ulIdConfigMenu;

                                        /* If we paste, use the item we have saved into our
                                           own clipboard */
                pMDNew=pHP->pMenuDataCutPaste;
                pHP->pMenuDataCutPaste=0;
                ulResult=DID_OK;
                WinSetWindowUShort(hwndCutPasteButton, QWS_ID, CDPB_CUT);
                WinSetWindowText(hwndCutPasteButton, "C~ut");
                                        /* If we paste the Config Menu menuentry, reset flag to display
                                           Setup dialog instead of Popup Menu */
                ulIdConfigMenu=ID_CONFIGDIALOG;
                if(SearchItem(pMDNew, &ulIdConfigMenu))
                    pHP->ulRuntimeFlag &= (~DISPLAYCONFIGDIALOG);
                }
            else
                {
                                        /* If we don't paste, we have to use our dialog to get
                                           the new data from the user */
                pMDNew=AllocateMenuData(NULL);
                if(SHORT1FROMMP(mp1)!=CDPB_ADDCONTROL)
                    {                       /* Don't modify MenuData structure if a control entry
                                               is added */
                    free(pMDNew->PgmTitle);
                    pMDNew->PgmTitle=strdup("Insert here please");
                                            /* Increment ID only for non control entries, because
                                               control entries get their own unique ID */
                    pMDNew->id=pHP->MenuDataId++;
                    }
                LoadMenuData2SessionData(pMDNew, &SessionData);
                if(SHORT1FROMMP(mp1)==CDPB_ADDMENU)
                    {
                                            /* Start Addmenu PC/2 dialog box */
                    if((ulResult=WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, MI_DialogProcedure,
                        pHP->hDLLPc2Resource, MIID_MENUDIALOG, &SessionData))==DID_ERROR)
                        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                            "Creation of a dialog box failed - continuing...");
                    }
                if(SHORT1FROMMP(mp1)==CDPB_ADDPROGRAM)
                    {
                                        /* Start Program Installation dialog box */
                    if((ulResult=WinDlgBox(HWND_DESKTOP, hwndDlg, PI_DialogProcedure,
                        pHP->hDLLPc2Resource, PIID_PROGRAMDIALOG, &SessionData))==DID_ERROR)
                        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                            "Creation of a dialog box failed - continuing...");
                    }
                if(SHORT1FROMMP(mp1)==CDPB_ADDCONTROL)
                    {
                                        /* Start Menucontrol Addition dialog box */
                    if((ulResult=WinDlgBox(HWND_DESKTOP, hwndDlg, MD_DialogProcedure,
                        pHP->hDLLPc2Resource, MDID_CONTROLDIALOG, &SessionData))==DID_ERROR)
                        PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                            "Creation of a dialog box failed - continuing...");
                    }
                                        /* Check for break separators, because they can only
                                           be applied to functional menuitems as a change of
                                           their style. Break separators also can't be applied
                                           an break saparators. */
                if(!strcmp(SessionData.PgmTitle, CTRL_BREAKSEPARATOR))
                    if((pMD->Back==NULL) || (pMD->Back->Submenu==pMD) ||
                        (!strcmp(pMD->PgmTitle, CTRL_BREAKSEPARATOR)))
                                        /* If user wants to apply a break separator style to
                                           first entry of a submenu, ignore request */
                        ulResult=DID_CANCEL;
                }
            }
        if(ulResult==DID_OK)            /* If manipulation is done successfully, then load
                                           the SESSIONDATA structure back to the MENUDATA
                                           structure and save the changes */
            {
                                        /* If a menuentry was cloned or pasted, we don't get data
                                           back from the Program/Menu/Menucontrol Installation dialog */
            if((SHORT1FROMMP(mp1)!=CDPB_CLONEPROGRAM) && (SHORT1FROMMP(mp1)!=CDPB_PASTE))
                LoadSessionData2MenuData(pMDNew, &SessionData);
            if(pMD->Item!=ENTRYEMPTY)   /* Add new entry, if the current entry isn't empty */
                {
                if(SHORT1FROMMP(mp1)==CDPB_ADDMENU)
                    {                   /* It it is a Submenu, we also must add an empty
                                           first item for it */
                    pMDNew->Submenu=AllocateMenuData(NULL);
                    pMDNew->Submenu->Back=pMDNew;
                    pMDNew->Item=ENTRYSUBMENU;
                    }
                if(SHORT1FROMMP(mp1)==CDPB_ADDPROGRAM)
                    pMDNew->Item=ENTRYMENUITEM;
                if(SHORT1FROMMP(mp1)==CDPB_ADDCONTROL)
                    {                   /* For controls also add the ID of the control. All but two
                                           controls have an predefined ID because the action
                                           they should start is allways the same */
                    pMDNew->Item=ENTRYCONTROL;
                    if(pMDNew->id==ID_CONFIGDIALOG)
                        {               /* If the Configuration Dialog is found reset flag
                                           to false */
                        pHP->ulRuntimeFlag&=(~DISPLAYCONFIGDIALOG);
                        pMDNew->id=ID_CONFIGDIALOG;
                        }
                                        /* For all controls except CTRL_BREAKSEPARATOR and
                                           CTRL_SEPARATOR the id was allready filled by the dialog */
                                        /* We assume that more than 1 separator may occur
                                           so to be able add, modify or delete on menuitems
                                           define a unique one */
                    if(!strcmp(pMDNew->PgmTitle, CTRL_BREAKSEPARATOR)) pMDNew->id=pHP->MenuDataId++;;
                    if(!strcmp(pMDNew->PgmTitle, CTRL_SEPARATOR)) pMDNew->id=pHP->MenuDataId++;;
                    }
                if(pMD->Next!=NULL)
                    (pMD->Next)->Back=pMDNew;
                pMDNew->Next=pMD->Next;
                pMDNew->Back=pMD;
                pMD->Next=pMDNew;
                                        /* Insert item after the existing item */
                SetPopupMenu(pHP->hwndPopupMenu, MM_INSERTITEMMENUITEM, MPFROMP(pMDNew), MPFROMP(pMD));
                }
            else                        /* If it is an empty entry fill it with user data */
                {
                if(SHORT1FROMMP(mp1)==CDPB_ADDMENU)
                    {                   /* It it is a Submenu, we also must add an empty
                                           first item for it */
                    pMDNew->Submenu=AllocateMenuData(NULL);
                    pMDNew->Submenu->Back=pMDNew;
                    pMDNew->Item=ENTRYSUBMENU;
                    }
                if(SHORT1FROMMP(mp1)==CDPB_ADDPROGRAM)
                    pMDNew->Item=ENTRYMENUITEM;
                if(SHORT1FROMMP(mp1)==CDPB_ADDCONTROL)
                    {                   /* For controls also add the ID of the control. These
                                           controls have an predefined ID because the action
                                           they should start is allways the same */
                    pMDNew->Item=ENTRYCONTROL;
                                        /* If the Configuration Dialog is found reset flag
                                           to false */
                    if(pMDNew->id==ID_CONFIGDIALOG)
                        pHP->ulRuntimeFlag&=(~DISPLAYCONFIGDIALOG);
                                        /* For all controls except CTRL_BREAKSEPARATOR and
                                           CTRL_SEPARATOR the id was allready filled by the dialog */
                                        /* We assume that more than 1 separator may occur
                                           so to be able add, modify or delete on menuitems
                                           define a unique one */
                    if(!strcmp(pMDNew->PgmTitle, CTRL_BREAKSEPARATOR)) pMDNew->id=pHP->MenuDataId++;;
                    if(!strcmp(pMDNew->PgmTitle, CTRL_SEPARATOR)) pMDNew->id=pHP->MenuDataId++;;
                    }
                                        /* Replace the ENTRYEMPTY item (which is the first one of an
                                           empty Submenu) by the one added by the user. This may also
                                           be the first entry of the root level submenu */
                if((pMD->Back) && (pMD->Back->Submenu))
                    {
                    pMDNew->Back=pMD->Back;
                    pMDNew->Back->Submenu=pMDNew;
                    pHP->pMenuData=pMDNew;
                                        /* This is the first item of a Submenu, then
                                           insert it there */
                    SetPopupMenu(pHP->hwndPopupMenu, MM_INSERTITEMSUBMENU, MPFROMP(pMDNew), MPFROMP(pMDNew->Back));
                    }
                else
                    {
                    pHP->pPopupMenu=pHP->pMenuData=pMDNew;
                                        /* This is the complete first item of the linked
                                           list, so insert at the end */
                    SetPopupMenu(pHP->hwndPopupMenu, MM_INSERTITEMMENUITEM, MPFROMP(pMDNew), MPFROMP(NULL));
                    }
                                        /* Deallocate the MENUDATA structure we replaced */
                DeAllocateMenuData(pMD);
                }
                                        /* Load Popup Menu entry's icon if applicable */
            if(pHP->ulStatusFlag2 & POPUPICON)
                InitializeIcon(pMDNew, TRUE);
            }
        else
            {
                                        /* Free copies in SESSIONDATA */
            free(SessionData.PgmDosSettings);
            free(SessionData.PgmLeadingEnvironment);
            free(SessionData.PgmTrailingEnvironment);
            free(SessionData.EnvironmentSpaces);
                                        /* Free temporary MENUDATA structure because user canceled
                                           adding to tne Popup Menu */
            DeAllocateMenuData(pMDNew);
            }
                                        /* Initialize the listbox */
        WinSendMsg(hwndDlg, WM_RELOADPOPUPMENU, MPFROMP(pHP->pMenuData), NULL);
                                        /* If PC/2 is not the WPS process and not enabled,
                                           disable spooler menu item in Popup Menu */
        if((!(pHP->ulRuntimeFlag & PC2RUNNINGASWPS)) && (!(pHP->ulStatusFlag1 & SHOWSPOOLERWINDOW)))
            WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_SPOOLER, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
                                        /* Disable Exit OS/2 menuitem when running as the
                                           WPS replacement or PROTSHELL */
        if(pHP->ulRuntimeFlag & (PC2RUNNINGASWPS|PC2RUNNINGASPROTSHELL))
            {
            WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
                MPFROM2SHORT(ID_EXIT, TRUE), MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED));
            }
                                        /* If APM is disabled, then disable the PowerDown
                                           smarticons, and the PowerDown OS/2 menuentry, else
                                           enable them */
        WinSendMsg(pHP->hwndPopupMenu, MM_SETITEMATTR,
            MPFROM2SHORT(ID_POWERDOWN, TRUE), MPFROM2SHORT(MIA_DISABLED, (pHP->ulStatusFlag2 & SUPPORTAPM ? 0 : MIA_DISABLED)));
        WinSendMsg(pHP->hwndMenu, MM_SETITEMATTR,
            MPFROM2SHORT(ID_ICONPOWERDOWN, TRUE), MPFROM2SHORT(MIA_DISABLED, (pHP->ulStatusFlag2 & SUPPORTAPM ? 0 : MIA_DISABLED)));
                                        /* Recalculate ownerdraw Popup-Menu */
        if(pHP->ulStatusFlag2 & POPUPICON)
            WinSetPresParam(pHP->hwndPopupMenu, PP_FONTNAMESIZE,
                sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
        break;
        }

/*                                                                                      *\
 * The user selected to change an item. Thus dismiss the PC/2 Configuration dialog and  *
 * load the Menu or Program Installation dialog. The new session data is entered in a   *
 * STARTSESSION structure named StartSession.                                           *
 * Then reload the PC/2 Configuration dialog again.                                     *
\*                                                                                      */
    case CDPB_CHANGEENTRY:              /* Change a Menu or Program configuration selected */
        {
        SESSIONDATA     SessionData;    /* Used by Menu Installation dialog and by
                                           Program Installation dialog to store menu or
                                           program data, to be filled from the user or
                                           to be presented to the user. */
                                        /* Each dialog procedure returns the result with
                                           WinDismissDlg() */
        ULONG           ulResult=DID_CANCEL;
        MENUDATA        *pMD;
        SHORT           sCount;

        pMD=pHP->pMenuData; /* Point to the first element of the linked list
                                           at the current level */
                                        /* Send message to listbox */
        sCount=(LONG)WinSendDlgItemMsg(
            hwndDlg,                    /* Handle of dialog window */
            CDLB_MENUPROGRAM,           /* Submenu & Program listbox */
            LM_QUERYSELECTION,          /* Query first selected list box item */
            MPFROMSHORT(LIT_FIRST),
            (MPARAM)NULL);
        if(sCount==LIT_NONE)            /* If no item selected ignore this button */
            return((MRESULT)FALSE);
                                        /* Save cursor selection */
        ulListboxCursor=(ULONG)sCount;
        for( ;sCount>0; sCount--)       /* Walk through the linked list to the selected
                                           item */
            pMD=pMD->Next;
                                        /* Controls can't be changed, they must be removed
                                           and newly inserted. Only the scheduler settings
                                           can be changed for some controls */
        while((pMD->Item==ENTRYCONTROL) &&
            (pMD->id!=ID_SHUTDOWN) &&
            (pMD->id!=ID_POWERDOWN) &&
            (pMD->id!=ID_EXIT) &&
            (pMD->id!=ID_LOCKUP))
            return((MRESULT)FALSE);
                                        /* Now load the MENUDATA to SESSIONDATA structure
                                           where the manipulations will take effect */
        LoadMenuData2SessionData(pMD, &SessionData);
        if(pMD->Item==ENTRYMENUITEM)
                                        /* It's a Menuitem  so call the Program
                                           Installation dialog box */
            if((ulResult=WinDlgBox(     /* Start Program Installation dialog box */
                HWND_DESKTOP,           /* DESKTOP is parent */
                hwndDlg,                /* This dialog is owner */
                PI_DialogProcedure,     /* Dialog procedure of Program Installation dialog */
                pHP->hDLLPc2Resource,   /* Ressource from file */
                PIID_PROGRAMDIALOG,     /* ID of Program Installation PC/2 dialog */
                                        /* Initialization data */
                &SessionData))==DID_ERROR)
                PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "Creation of a dialog box failed - continuing...");
        if(pMD->Item==ENTRYSUBMENU)
                                        /* It's a Submenu so call the Menu Installation
                                           dialog box */
            if((ulResult=WinDlgBox(     /* Start Addmenu PC/2 dialog box */
                HWND_DESKTOP,           /* DESKTOP is parent */
                hwndDlg,                /* This dialog is owner */
                MI_DialogProcedure,     /* Dialog procedure of Program Installation dialog */
                pHP->hDLLPc2Resource,   /* Ressource from file */
                MIID_MENUDIALOG,        /* ID of Addmenu PC/2 dialog */
                                        /* Initialization data */
                &SessionData))==DID_ERROR)
                PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "Creation of a dialog box failed - continuing...");
        if(pMD->Item==ENTRYCONTROL)
                                        /* It's a Menucontrol so call the Control
                                           Configuration dialog box */
            if((ulResult=WinDlgBox(     /* Start Control Configuration dialog box */
                HWND_DESKTOP,           /* DESKTOP is parent */
                hwndDlg,                /* This dialog is owner */
                CC_DialogProcedure,     /* Dialog procedure of Control Configuration dialog */
                pHP->hDLLPc2Resource,   /* Ressource from file */
                PIID_PROGRAMDIALOG,     /* ID of Program Installation PC/2 dialog */
                                        /* Initialization data */
                &SessionData))==DID_ERROR)
                PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "Creation of a dialog box failed - continuing...");
        if(ulResult==DID_OK)            /* If manipulation is done successfully, then load
                                           the SESSIONDATA structure back to the MENUDATA
                                           structure and save the changes */
            {
                                        /* Save the data changed in the notebook */
            LoadSessionData2MenuData(pMD, &SessionData);
            if(pMD->Item!=ENTRYCONTROL)
                {
                                        /* In case of drawing the entry's icon, reload because of
                                           possible changes */
                if(pHP->ulStatusFlag2 & POPUPICON)
                    {
                                        /* Unload Popup Menu entry's icon if applicable */
                    InitializeIcon(pMD, FALSE);
                                        /* Reload Popup Menu entry's icon if applicable */
                    InitializeIcon(pMD, TRUE);
                                        /* Recalculate ownerdraw Popup-Menu */
                    WinSetPresParam(pHP->hwndPopupMenu, PP_FONTNAMESIZE,
                        sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
                    }
                                        /* Now change the menuitem text to the new one */
                SetPopupMenu(pHP->hwndPopupMenu, MM_SETITEMTEXT, MPFROMP(pMD), MPFROMLONG(pMD->id));
                                        /* Initialize the listbox */
                WinSendMsg(hwndDlg, WM_RELOADPOPUPMENU, MPFROMP(pHP->pMenuData), NULL);
                }
            }
        else
            {
                                        /* Free copies in SESSIONDATA */
            free(SessionData.PgmDosSettings);
            free(SessionData.PgmLeadingEnvironment);
            free(SessionData.PgmTrailingEnvironment);
            free(SessionData.EnvironmentSpaces);
            }
        break;
        }

/*                                                                                      *\
 * The user selected to remove or cut (to clipboard) an item. If this item is the only  *
 * one in the linked list or the first item of a submenu set it to empty otherwise free *
 * resources and remove the entry (in case of remove) or cut (in case of cut into       *
 * clipboard) ressources.                                                               *
\*                                                                                      */
    case CDPB_CUT:                      /* Cut selected item into our clipboard */
    case CDPB_REMOVEENTRY:              /* Remove selected item from the PC/2 Setup dialog */
        {
        MENUDATA        *pMD;
        SHORT           sCount;

        pMD=pHP->pMenuData;             /* Point to the first element of the linked list
                                           at the current level */
                                        /* Send message to listbox */
        sCount=(LONG)WinSendDlgItemMsg(
            hwndDlg,                    /* Handle of dialog window */
            CDLB_MENUPROGRAM,           /* Submenu & Program listbox */
            LM_QUERYSELECTION,          /* Query first selected list box item */
            MPFROMSHORT(LIT_FIRST),
            (MPARAM)NULL);
        if(sCount==LIT_NONE)            /* If no item selected ignore this button */
            return((MRESULT)FALSE);
                                        /* Save cursor selection */
        ulListboxCursor=(ULONG)sCount;
        for( ;sCount>0; sCount--)       /* Walk through the linked list to the selected
                                           item */
            pMD=pMD->Next;
        while(TRUE)
            {
            MENUDATA    *pMenuDataBackOrParent;
            MENUDATA    *pMenuDataNext;

                                        /* Save the pointer to the next menuentry (which may be NULL
                                           of course) because the current one gets removed */
            pMenuDataBackOrParent=pMD->Back;
            pMenuDataNext=pMD->Next;
                                        /* If the selected menu entry is followed by a
                                           break separator, which is just a style of the
                                           menu entry itself, disallow removal before the
                                           break separator is deleted, because this avoids
                                           very complex logic to correctly remove the
                                           break separator / append it to previous menu entry */
            if(pMenuDataNext!=NULL)
                if(!strcmp(pMenuDataNext->PgmTitle, CTRL_BREAKSEPARATOR))
                    break;
                                        /* Ignore request to cut break separators */
            if((!strcmp(pMD->PgmTitle, CTRL_BREAKSEPARATOR)) && (SHORT1FROMMP(mp1)==CDPB_CUT))
                break;
                                        /* Ask the user if he is really sure */
            if(USR_ERR(hwndDlg, 0, MB_QUERY | MB_YESNO | MB_MOVEABLE | MB_DEFBUTTON1,
                (SHORT1FROMMP(mp1)==CDPB_CUT ?
                    "Are you sure you want to cut the selected menuentry?" :
                    "Are you sure you want to delete the selected menuentry?"))==MBID_NO)
                break;
                                        /* If the menuentry is the one and only entry in the Popup-Menu
                                           destroy it, and load an empty new one */
            if((pMenuDataBackOrParent==NULL) && (pMenuDataNext==NULL))
                {
                pHP->pPopupMenu=pHP->pMenuData=AllocateMenuData(NULL);
                pHP->pPopupMenu->hwndItem=pHP->hwndPopupMenu;
                }
                                        /* If the menuentry is the first one in the Popup-Menu, set the
                                           next one as the first one. This applies, first to the whole
                                           Popup Menu, second to any submenu level */
            else if((pMenuDataBackOrParent==NULL) && (pMenuDataNext!=NULL))
                {
                pHP->pPopupMenu=pHP->pMenuData=pMenuDataNext;
                pMenuDataNext->Back=NULL;
                }
            else if((pMenuDataBackOrParent!=NULL) && (pMenuDataBackOrParent->Submenu==pMD))
                {
                if(pMenuDataNext==NULL)
                    {
                    pMenuDataNext=AllocateMenuData(NULL);
                    pMenuDataNext->hwndItem=pMD->hwndItem;
                    pMenuDataNext->Back=pMenuDataBackOrParent;
                    }
                else
                    pMenuDataNext->Back=pMenuDataBackOrParent;
                pMenuDataBackOrParent->Submenu=pMenuDataNext;
                pHP->pMenuData=pMenuDataNext;

                }
            else
                                        /* The menuentry is preceeded by a menuentry and followed by either
                                           another menuentry or by none, we just remove current one */
                {
                pMenuDataBackOrParent->Next=pMenuDataNext;
                if(pMenuDataNext)
                    pMenuDataNext->Back=pMenuDataBackOrParent;
                }
            if(SHORT1FROMMP(mp1)==CDPB_REMOVEENTRY)
                {
                                        /* Delete the menuentry from the Popup Menu itself (the current menuentry's
                                           hwndItem is the handle of the parent submenu, either the previous node
                                           in the linked list, or pHP->hwndPopupMenu */
                SetPopupMenu(pMD->hwndItem, MM_DELETEITEM, MPFROMP(pMD), NULL);
                                        /* If we should remove the menuentry and the menuentry is a Submenu,
                                           destroy the Submenu window */
                if((pMD->Item==ENTRYSUBMENU))
                    WinDestroyWindow(pMD->Submenu->hwndItem);
                                        /* Remove the associated MENUDATA structure (or structures in case
                                           of a Submenu). As this may take a while, pass it on to the
                                           I/O thread */
                WinPostMsg(pHP->hwndIOThread, WM_POPUPMENUDELETE, MPFROMP(pMD), MPFROMSHORT(TRUE));
                }
            else
                {
                HWND    hwndCutPasteButton=WinWindowFromID(hwndDlg, CDPB_CUT);
                ULONG   ulIdConfigMenu;

                                        /* Remove the menuentry from the Popup Menu itself (the current menuentry's
                                           hwndItem is the handle of the parent submenu, either the previous node
                                           in the linked list, or pHP->hwndPopupMenu */
                SetPopupMenu(pMD->hwndItem, MM_REMOVEITEM, MPFROMP(pMD), NULL);
                                        /* Clear list pointers to reflect that the menuentry
                                           is now no longer part of the Popup Menu */
                pMD->Back=pMD->Next=NULL;
                                        /* If we should cut the menuentry, save it into our internal
                                           clipboard area and change the button from cut to paste */
                pHP->pMenuDataCutPaste=pMD;
                WinSetWindowUShort(hwndCutPasteButton, QWS_ID, CDPB_PASTE);
                WinSetWindowText(hwndCutPasteButton, "Past~e");
                                        /* If we cut the Config Menu menuentry, set flag to display
                                           Setup dialog instead of Popup Menu */
                ulIdConfigMenu=ID_CONFIGDIALOG;
                if(SearchItem(pHP->pMenuDataCutPaste, &ulIdConfigMenu))
                    pHP->ulRuntimeFlag |= DISPLAYCONFIGDIALOG;
                }
            break;
            WinPostMsg(pHP->hwndIOThread, WM_POPUPMENUDELETE, MPFROMP(pMD), MPFROMSHORT(TRUE));
            }
                                        /* Initialize the listbox */
        WinSendMsg(hwndDlg, WM_RELOADPOPUPMENU, MPFROMP(pHP->pMenuData), NULL);
                                        /* Recalculate ownerdraw Popup-Menu */
        if(pHP->ulStatusFlag2 & POPUPICON)
            WinSetPresParam(pHP->hwndPopupMenu, PP_FONTNAMESIZE,
                sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
        break;
        }

/*                                                                                      *\
 * The user selected to resort the current level of the menuentries. Load the dialog    *
 * and let the user resort the linked list of menues pointed to by pMenuData and to     *
 * resort the menuentries of the Popup-Menu.                                            *
\*                                                                                      */
    case CDPB_RESORT:                   /* Load the resort dialog */
        if(WinDlgBox(                   /* Start Resort dialog box */
            HWND_DESKTOP,               /* DESKTOP is parent */
            hwndDlg,                    /* This dialog is owner */
            RD_DialogProcedure,         /* Dialog procedure of Program Installation
                                           dialog */
            pHP->hDLLPc2Resource,       /* Ressource from file */
            RDID_RESORTDIALOG,          /* ID of Program Installation PC/2 dialog */
            0)==DID_ERROR)              /* No initialization data */
            PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_CREATEDIALOG, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                "Creation of a dialog box failed - continuing...");
                                        /* Initialize the listbox */
        WinSendMsg(hwndDlg, WM_RELOADPOPUPMENU, MPFROMP(pHP->pMenuData), NULL);
                                        /* Recalculate ownerdraw Popup-Menu */
        if(pHP->ulStatusFlag2 & POPUPICON)
            WinSetPresParam(pHP->hwndPopupMenu, PP_FONTNAMESIZE,
                sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
        break;

/*                                                                                      *\
 * The user selected import or export a menuentry from or to a file.                    *
 * resort the menuentries of the Popup-Menu.                                            *
\*                                                                                      */
    case CDPB_IMPORTPROGRAM:
    case CDPB_EXPORTPROGRAM:
        {
        FILEDLG         fdFileDlg;      /* Standard file dialog control */
        HWND            hwndFileDlg;    /* Handle of standard file dialog */
                                        /* Selected menuentry to save */
        SHORT           sSelection;
        MENUDATA        *pMenuDataSelected;
        MENUDATA        *pMenuDataFile; /* Copy of saved or newly loaded menuentry */
        FILE            *pfileProfile;  /* Handle of file we load from or save into */

                                        /* Find selected menuentry if we want to save it */
        if(SHORT1FROMMP(mp1)==CDPB_EXPORTPROGRAM)
            {
            pMenuDataSelected=pHP->pMenuData;
            sSelection=(LONG)WinSendDlgItemMsg(hwndDlg, CDLB_MENUPROGRAM, LM_QUERYSELECTION,
                MPFROMSHORT(LIT_FIRST), (MPARAM)NULL);
                                        /* If no item selected ignore this button */
            if(sSelection==LIT_NONE)
                return((MRESULT)FALSE);
            for( ;sSelection>0; sSelection--)
                pMenuDataSelected=pMenuDataSelected->Next;
            }
                                        /* Clear out structure */
        memset(&fdFileDlg, 0, sizeof(FILEDLG));
                                        /* Display file dialog */
        fdFileDlg.cbSize=sizeof(FILEDLG);
        if(SHORT1FROMMP(mp1)==CDPB_IMPORTPROGRAM)
            {
            fdFileDlg.fl=FDS_CENTER | FDS_OPEN_DIALOG;
            fdFileDlg.pszOKButton="~Load";
            }
        else
            {
            fdFileDlg.fl=FDS_CENTER | FDS_ENABLEFILELB | FDS_SAVEAS_DIALOG;
            fdFileDlg.pszOKButton="~Save";
            }
        fdFileDlg.pszTitle="PC/2 Popup Menu Entry";
        strcpy(fdFileDlg.szFullFile, pHP->ucPathDLL);
        strcat(fdFileDlg.szFullFile, "\\*.CFG");
                                        /* Open the standard file dialog ...*/
        hwndFileDlg=WinFileDlg(HWND_DESKTOP, hwndDlg, &fdFileDlg);
        if(hwndFileDlg && (fdFileDlg.lReturn==DID_OK))
            {
                                        /* Disable listbox until operation has finished by I/O
                                           thread */
            WinEnableWindow(WinWindowFromID(hwndDlg, CDLB_MENUPROGRAM), FALSE);
            if(SHORT1FROMMP(mp1)==CDPB_IMPORTPROGRAM)
                {
                pMenuDataFile=AllocateMenuData(NULL);
                                        /* Tell I/O thread to load menuentry */
                WinPostMsg(pHP->hwndIOThread, WM_LOADPOPUPMENU,
                    MPFROMP(pMenuDataFile), MPFROMP(strdup(fdFileDlg.szFullFile)));
                }
            else
                {
                pMenuDataFile=AllocateMenuData(NULL);
                *pMenuDataFile=*pMenuDataSelected;
                pMenuDataFile->Back=pMenuDataFile->Next=NULL;
                                        /* Tell I/O thread to load menuentry */
                WinPostMsg(pHP->hwndIOThread, WM_SAVEPOPUPMENU,
                    MPFROMP(pMenuDataFile), MPFROMP(strdup(fdFileDlg.szFullFile)));
                }
            }
        }
        break;

    case DID_OK:                        /* Enter key pressed */
                                        /* Save the changes */
        WinPostMsg(pHP->hwndClient, WM_SAVEPOPUPMENU, MPFROMLONG(ID_MAINTHREAD), NULL);
                                        /* Check for changes that should be backuped */
        pHP->ulConfigurationChanges+=2;
        CopySetupFiles(hwndDlg, FALSE);
                                        /* Dialog terminated with DID_OK */
        WinDismissDlg(hwndDlg, DID_OK);
        break;

    case DID_CANCEL:                    /* Escape or Cancel pressed */
                                        /* If the user wants to dismiss dialog but icon caches have been
                                           found note that to the user (allowing him to save dialog instead) */
        if(pHP->ulRuntimeFlag & ICONCACHEACTIVE)
            {
            if(USR_ERR(hwndDlg, HELP_ICONCACHE, MB_QUERY|MB_YESNO|MB_MOVEABLE|MB_DEFBUTTON1,
                "PC/2 has found some smart caches, that would improve load time for the menuentries' icons "
                "the next time you launch PC/2! You may want to save them, by saving the Popup Menu.")==MBID_YES)
                return((MRESULT)TRUE);
            }
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

/*                                                                                      *\
 * Syntax: WM_LOADPOPUPMENU, MENUDATA *pMenuData, UCHAR *pucError                       *
 * Syntax: WM_SAVEPOPUPMENU, MENUDATA *pMenuData, UCHAR *pucError                       *
\*                                                                                      */
case WM_LOADPOPUPMENU:
case WM_SAVEPOPUPMENU:
/*                                                                                      *\
 * The I/O thread has loaded a menuentry the user selected to import or saved a         *
 * menuentry the user has selected to export.                                           *
\*                                                                                      */
    {
    MENUDATA    *pMenuData=PVOIDFROMMP(mp1);
    MENUDATA    *pMenuDataTemp;
                                        /* Selected menuentry */
    SHORT       sSelection;
    MENUDATA    *pMenuDataSelected;

                                        /* Enable listbox that as operation has finished by I/O
                                           thread */
    WinEnableWindow(WinWindowFromID(hwndDlg, CDLB_MENUPROGRAM), TRUE);
                                        /* If an error occurred, inform the user */
    if(PVOIDFROMMP(mp2)!=NULL)
        {
        USR_ERR(hwndDlg, HELP_PC2CFG, MB_INFORMATION|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
            PVOIDFROMMP(mp2));
        break;
        }
                                        /* If we saved a menuentry, nothing more has to be done, if
                                           we loaded a menuentry we have to insert it at the selection */
    if(msg==WM_LOADPOPUPMENU)
        {
                                        /* Find selected menuentry to insert loaded menuentry afterwards */
        sSelection=(LONG)WinSendDlgItemMsg(hwndDlg, CDLB_MENUPROGRAM, LM_QUERYSELECTION,
            MPFROMSHORT(LIT_FIRST), (MPARAM)NULL);
        pMenuDataSelected=pHP->pMenuData;
        if(sSelection!=LIT_NONE)
            for( ;sSelection>0; sSelection--)
                pMenuDataSelected=pMenuDataSelected->Next;
                                        /* Insert loaded menuentry after selected menuentry */
        if(sSelection!=LIT_NONE)
            {
                                        /* Just in case more than one menuentry was imported
                                           (though this can only happen if a non-exported profile
                                           was imported, as during export only 1 menuentry can
                                           be selected in the listbox) */
            pMenuDataTemp=pMenuData;
            while(pMenuDataTemp->Next)
                pMenuDataTemp=pMenuDataTemp->Next;
                                        /* Insert to control structure and Popup-Menu, be sure to
                                           use exactly this sequence! */
            pMenuData->Back=pMenuDataSelected;
            SetPMMenu(pMenuDataSelected->hwndItem, pMenuData);
            pMenuDataTemp->Next=pMenuDataSelected->Next;
            pMenuDataSelected->Next=pMenuData;
            }
                                        /* Insert loaded menuentry as the last one (which
                                           may be the first one if the list is empty) */
        else
            {
            pMenuDataTemp=pHP->pMenuData;
            while(pMenuDataTemp->Next)
                pMenuDataTemp=pMenuDataTemp->Next;
                                        /* Add new entry, if the current entry isn't empty */
            if(pMenuDataTemp->Item!=ENTRYEMPTY)
                {
                                        /* Insert to control structure and Popup-Menu, be sure to
                                           use exactly this sequence! */
                pMenuData->Back=pMenuDataTemp;
                SetPMMenu(pMenuDataTemp->hwndItem, pMenuData);
                pMenuDataTemp->Next=pMenuData;
                }
            else
                {
                                        /* Replace the ENTRYEMPTY item (which is the first one of an
                                           empty Submenu) by the one added by the user. This may also
                                           be the first entry of the root level submenu */
                if((pMenuDataTemp->Back) && (pMenuDataTemp->Back->Submenu))
                    {
                                        /* Insert to control structure and Popup-Menu, be sure to
                                           use exactly this sequence! */
                    pMenuData->Back->Submenu=pMenuData;
                    SetPMMenu(pMenuData->Back->hwndItem, pMenuData);
                    pMenuData->Back=pMenuDataTemp->Back;
                    pHP->pMenuData=pMenuData;
                    SetPopupMenu(pHP->hwndPopupMenu, MM_INSERTITEMSUBMENU, MPFROMP(pMenuData), MPFROMP(pMenuData->Back));
                    }
                else
                    {
                                        /* This is the complete first item of the linked
                                           list of the complete Popup-Menu, so insert at the end */
                    pHP->pPopupMenu=pHP->pMenuData=pMenuData;
                    SetPMMenu(pHP->hwndPopupMenu, pMenuData);
                    }
                DeAllocateMenuData(pMenuDataTemp);
                }
            }

                                        /* Initialize the listbox */
        WinSendMsg(hwndDlg, WM_RELOADPOPUPMENU, MPFROMP(pHP->pMenuData), NULL);
        }
    }
    break;



default:                                /* Default window procedure must be called */
    return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
}
return((MRESULT)FALSE);                 /* We have handled the message */
}

/*--------------------------------------------------------------------------------------*\
 * This subclassed window procedure handles the PC/2's setup dialog CDLB_MENUPROGRAM    *
 * listbox to allow dropping into.                                                      *
 * The items (menuentries and submenus) dropped are inserted at the end of the current  *
 * leave of the tree. The end may either be at least one of existing items or the first *
 * item of a submenu. In the first case we just have to add it to the menu handle the   *
 * end is part of, in the second case we have first to create a new menu and add the    *
 * dropped item into this newly created menu (and of course add this menu to our parent *
 * submenu.                                                                             *
 * Req: none                                                                            *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY SubclassedListboxWindowProc(HWND hwndListbox, ULONG msg, MPARAM mp1, MPARAM mp2)
{
PDRAGINFO       pDraginfo;              /* Pointer to DRAGINFO structure */
PDRAGITEM       pDragitem;              /* Pointer to DRAGITEM structure */
SHORT           sSelection;             /* Line selected in listbox */

switch(msg)
{
/*                                                                                      *\
 * Allow known objects to be dropped.                                                   *
\*                                                                                      */
case DM_DRAGOVER:
    pDraginfo = (PDRAGINFO)mp1;         /* Get the pointer to the DRAGINFO structure */
                                        /* Access the structure */
    if(DrgAccessDraginfo(pDraginfo)==FALSE) break;
                                        /* Get the first itemp of the item(s) dragged
                                           onto dialog window */
    pDragitem = DrgQueryDragitemPtr(pDraginfo, 0);
                                        /* Allow only one object, coming from only WPS 2 PC/2 */
    if((pDraginfo->cditem!=1) || !(DrgVerifyRMF(pDragitem, "DRM_WPS2PC2", "DRF_WPS2PC2")))
                                        /* Don't allow dropping of more than one item */
        return(MPFROM2SHORT(DOR_NODROP, DO_UNKNOWN));
                                        /* Allow drop of undefined operation onto dialog window */
    return(MPFROM2SHORT(DOR_DROP, DO_UNKNOWN));

case DM_DROP:
    {
    UCHAR       ucObjectType[256];
    PBYTE       pbSharedMem;
    MENUDATA    *pMDInsertAfter;
    MENUDATA    *pMDInsertAfterNext;

    pDraginfo = (PDRAGINFO)mp1;         /* Get the pointer to the DRAGINFO structure */
                                        /* Access the structure */
    if(DrgAccessDraginfo(pDraginfo)==FALSE) break;
                                        /* Get the first itemp of the item(s) dragged
                                           onto dialog window */
    pDragitem = DrgQueryDragitemPtr(pDraginfo, 0);
                                        /* Query the rendering format */
    DrgQueryStrName(pDragitem->hstrRMF, sizeof(ucObjectType), ucObjectType);
                                        /* Scan for an file object */
    if(DrgVerifyRMF(pDragitem, DRM_WPS2PC2, DRF_WPS2PC2))
        {                               /* It was dragged from WPS 2 PC/2, so we know how to
                                           access the shared memory we expect */
        pbSharedMem=(PBYTE)pDragitem->ulItemID;
        if(DosGetSharedMem(pbSharedMem, PAG_READ | PAG_WRITE)==NO_ERROR)
            {
            ULONG   ulAddingAsFirstItem=FALSE;
            ULONG   ulNothingExtracted=FALSE;

                                        /* Find the selected menuentry */
            sSelection=(LONG)WinSendMsg(hwndListbox, LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)NULL);
            pMDInsertAfter=pHP->pMenuData;
            if(sSelection!=LIT_NONE)
                for( ;sSelection>0; sSelection--)
                    pMDInsertAfter=pMDInsertAfter->Next;
            else
                                        /* If none was selected, find last elment of current MENUDATA
                                           LIST pMenuData */
                for(pMDInsertAfter=pHP->pMenuData; pMDInsertAfter->Next; /* pMDInsertAfter=pMDInsertAfter->Next */)
                    pMDInsertAfter=pMDInsertAfter->Next;
                                        /* Save next element after selection, as ConvertWPSObjectList2MenuData()
                                           does cut the list (which is a requirement that SetPMMenu() does just
                                           insert the dropped menuentries */
            pMDInsertAfterNext=pMDInsertAfter->Next;
                                        /* If the item is empty, it must be the first item of a
                                           empty submenu */
            if(pMDInsertAfter->Item==ENTRYEMPTY)
                ulAddingAsFirstItem=TRUE;
                                        /* Insert the LIST of WPSOBJECTLISTs into current level of the
                                           Popup-Menu */
            ConvertWPSObjectList2MenuData(pMDInsertAfter, (WPSOBJECTLIST *)pbSharedMem);
                                        /* Load the smart icon caches when required */
            if(pHP->ulStatusFlag2 & POPUPICON)
                {
                pHP->ulRuntimeFlag|=POPUPMENUUPDATE;
                SetItemStyle(pHP->pPopupMenu, TRUE, TRUE);
                WinPostMsg(pHP->hwndIOThread, WM_POPUPMENUUPDATE, MPFROMP(pHP->pMenuData), MPFROMSHORT(TRUE));
                }
                                        /* Load the new Popup-Menu menuentry(ies) into PM's menu. We
                                           do expect, that the new menuentry is added after the menuentry
                                           that was the last one previously, otherwise bad things happend.
                                           First, find out if the items have been added after existing items,
                                           or as the first items in a new submenu */
            if(ulAddingAsFirstItem==TRUE)
                {
                                        /* The items have replaced the first empty entry of a submenu and
                                           added afterwards */
                if(pMDInsertAfter->Item==ENTRYEMPTY)
                    ulNothingExtracted=TRUE;
                else
                    {
                    MENUDATA    *pMDParentSubmenu;
                    HWND        hwndMenuWeInsert;

                                        /* Get the parent submenu the items have been inserted. If we
                                           have no parent, items have been inserted at the root of the
                                           Popup Menu, otherwise create a new menu for our parent */
                    pMDParentSubmenu=pMDInsertAfter->Back;
                    hwndMenuWeInsert=pHP->hwndPopupMenu;
                    if(pMDParentSubmenu!=NULL)
                        hwndMenuWeInsert=pMDParentSubmenu->hwndItem;
                    SetPMMenu(hwndMenuWeInsert, pMDInsertAfter);
                    }
                }
            else
                {
                                        /* The items have been added after (at lease on) existing item(s) */
                if(pMDInsertAfter->Next==0)
                    ulNothingExtracted=TRUE;
                else
                    SetPMMenu(pMDInsertAfter->hwndItem, pMDInsertAfter->Next);
                }
                                        /* Correct list */
            while(pMDInsertAfter->Next)
                pMDInsertAfter=pMDInsertAfter->Next;
            pMDInsertAfter->Next=pMDInsertAfterNext;
            if(pMDInsertAfterNext!=NULL)
                pMDInsertAfterNext->Back=pMDInsertAfter;
                                        /* The data we got from WPS 2 PC/2 should have added something */
            if(ulNothingExtracted==TRUE)
                PM_ERR(pHP->habPc2, pHP->hwndFrame, HELP_POPUPMENU, MB_ERROR|MB_OK|MB_HELP|MB_MOVEABLE|MB_DEFBUTTON1,
                    "No data extracted by WPS 2 PC/2 into PC/2's Popup Menu - continuing...");
                                        /* Give memory back to OS/2 */
            DosFreeMem(pbSharedMem);
                                        /* Initialize the listbox */
            WinSendMsg(WinQueryWindow(hwndListbox, QW_OWNER), WM_RELOADPOPUPMENU,
                MPFROMP(pHP->pMenuData), NULL);
            }
        break;
        }
    }
    return((MRESULT)FALSE);             /* We have handled the message */
}
                                        /* Call default window procedure */
return(pfnListboxWindowProc(hwndListbox, msg, mp1, mp2));
}

/*--------------------------------------------------------------------------------------*\
 * This subclassed window procedure handles the PC/2's smart icon menu.                 *
 * Req: none                                                                            *
\*--------------------------------------------------------------------------------------*/
MRESULT  EXPENTRY SubclassedMenuWindowProc(HWND hwndMenu, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static UCHAR    *ucFlyOverHelp[]={"Exit PC/2",
                                  "Move window",
                                  "Hide window",
                                  "Set to top",
                                  "Set to bottom",
                                  "Close window",
                                  "Maximize window",
                                  "Minimize window",
                                  "Restore window",
                                  "ShutDown OS/2",
                                  "PowerDown OS/2",
                                  "Lockup",
                                  "Help"};
static SHORT    sPreviousItemIdentity=MIT_NONE;

switch(msg)
{
case WM_PRESPARAMCHANGED:
    switch((ULONG)mp1)
    {
    case PP_FONTNAMESIZE:
        {
        ULONG       ulAttrFound;

                                        /* Get font selected for PC/2's smart icon menu */
        if(WinQueryPresParam(hwndMenu, PP_FONTNAMESIZE, 0, &ulAttrFound,
            sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont, 0))
            {
                                        /* Set this font also into the Popup-Menu */
            WinSetPresParam(pHP->hwndPopupMenu, PP_FONTNAMESIZE,
                sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
                                        /* Set this font also into the SessionBar's Popup-Menu */
            WinSetPresParam(pHP->hwndPopupMenuSessionBar, PP_FONTNAMESIZE,
                sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
                                        /* Set this font also into the SessionBar's Popup-Menu */
            WinSetPresParam(pHP->hwndClientMenu, PP_FONTNAMESIZE,
                sizeof(pHP->ucPopupMenuFont), pHP->ucPopupMenuFont);
            }
        }
        break;
    }
    break;

case WM_MOUSELEAVE:
    {
                                        /* This message is posted whenever the pointer is moved from
                                           one window to another (by PC2HOOK.DLL via PC/2's main window */
    POINTL  pointlMouse;
    RECTL   rectlMenu;

    pointlMouse.x=(LONG)SHORT1FROMMP(mp1);
    pointlMouse.y=(LONG)SHORT2FROMMP(mp1);
    WinMapWindowPoints(HWND_DESKTOP, hwndMenu, &pointlMouse, 1);
    WinQueryWindowRect(hwndMenu, &rectlMenu);
                                        /* If the pointer is outside the menu, reset titlebar to normal
                                           text, otherwise we get a WM_MOUSEMOVE anyway where we can
                                           check it the titlebar needs to be updated */
    if(!(WinPtInRect(pHP->habPc2, &rectlMenu, &pointlMouse)))
        {
        if(sPreviousItemIdentity!=MIT_NONE)
            WinPostMsg(pHP->hwndClient, WM_COMMAND, MPFROMSHORT(ID_DEBUG), MPFROMSHORT(CMDSRC_OTHER));
        sPreviousItemIdentity=MIT_NONE;
        }
    }
    break;

case WM_MOUSEMOVE:
    {
    SHORT       sItemCount;             /* Number of items in current menu */
    SHORT       sItemIndex;             /* Loop counter wihtin current menu */
                                        /* Menuentry identity we query its rectangle */
    SHORT       sCurrentItemIdentity=MIT_NONE;
    RECTL       rectlItem;              /* Rectangle that current menuentry occupies within current menu */
    POINTL      ptlMouse;               /* Pointer position relative to menu */

                                        /* Map the current mouse position from the screen
                                           coordinated to the menu coordinates */
    ptlMouse.x=(ULONG)SHORT1FROMMP(mp1);
    ptlMouse.y=(ULONG)SHORT2FROMMP(mp1);
                                        /* Loop through all items in the current menu,
                                           and query the item's rectangle. As we don't know the
                                           number of items in the menu, we break out if there
                                           is no more valid item */
    sItemCount=(BOOL)WinSendMsg(hwndMenu, MM_QUERYITEMCOUNT, NULL, NULL);
    for(sItemIndex=0; sItemIndex<sItemCount; sItemIndex++)
        {
                                        /* Let's see if we have another item available. Continue loop
                                           if there is something peculiar */
        sCurrentItemIdentity=(LONG)WinSendMsg(hwndMenu, MM_ITEMIDFROMPOSITION,
            MPFROMSHORT(sItemIndex), NULL);
                                        /* As MIT_ERROR=MIT_END=MIT_NONE=MIT_MEMERROR and may also be
                                           used for separators, just ignore this menuentries */
        if(sCurrentItemIdentity==MIT_ERROR) continue;
                                        /* Get the menuentry's rectangle to test if it covers the
                                           current mouse pointer position */
        WinSendMsg(hwndMenu, MM_QUERYITEMRECT,
            MPFROM2SHORT(sCurrentItemIdentity, FALSE), MPFROMP(&rectlItem));
                                        /* If the item's rectangle covers the pointer position,
                                           select this menuentry and exit our loop as our job
                                           is done */
        if((ptlMouse.x>rectlItem.xLeft) && (ptlMouse.x<=rectlItem.xRight) &&
            (ptlMouse.y>rectlItem.yBottom) && (ptlMouse.y<=rectlItem.yTop))
            {
            if(sPreviousItemIdentity!=sCurrentItemIdentity)
                {
                UCHAR   ucOverviewTitlebar[MAXNAMEL+4];
                UCHAR   *pucMenuAction;

                WinQueryWindowText(pHP->hwndFrame, sizeof(ucOverviewTitlebar), ucOverviewTitlebar);
                pucMenuAction=strchr(ucOverviewTitlebar, '-');
                if(pucMenuAction)
                    pucMenuAction+=2;
                else
                    {
                    strcat(ucOverviewTitlebar, " - ");
                    pucMenuAction=strchr(ucOverviewTitlebar, '\0');
                    }
                strcpy(pucMenuAction, ucFlyOverHelp[sCurrentItemIdentity-ID_BITMAPEXIT]);
                WinSetWindowText(pHP->hwndFrame, ucOverviewTitlebar);
                }
            sPreviousItemIdentity=sCurrentItemIdentity;
            break;
            }
        }
                                        /* If the pointer is outside any Smarticon, display the
                                           normal PC/2 Oveview window titlebar */
    if(sItemIndex>=sItemCount)
        {
        if(sPreviousItemIdentity!=MIT_NONE)
            WinPostMsg(pHP->hwndClient, WM_COMMAND, MPFROMSHORT(ID_DEBUG), MPFROMSHORT(CMDSRC_OTHER));
        sPreviousItemIdentity=MIT_NONE;
        }
    }
    break;
}
                                        /* Call default window procedure */
return(pfnMenuWindowProc(hwndMenu, msg, mp1, mp2));
}

