/***********************************************************************\
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1993, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * PC2.h        Program Commander/2 header file.                       *
 *                                                                     *
\***********************************************************************/

#define         INCL_WIN                /* Environment include files */
#define         INCL_GPI
#define         INCL_DOS
#define         INCL_BASE
#define         INCL_DOSERRORS
#define         INCL_GPIERRORS
#define         INCL_DOSPROCESS
#define         INCL_DOSDEVIOCTL
#include        <os2.h>

#include        <stdio.h>               /* C Set++ include files */
#include        <stdlib.h>
#include        <string.h>
#include        <ctype.h>
#include        <memory.h>
#include        <time.h>
#include        <math.h>
#include        <setjmp.h>
#ifndef __EMX__
#include        <builtin.h>
#else
#include        <sys/builtin.h>
#endif  /* __EMX__ */

                                        /* Include dialog an help resource ids */
#include        "PC2Res.h"
#include        "PC2Help.h"
                                        /* Include header for additions to the OS/2 Toolkits */
#include        "PC2Tlkt.h"

                                        /* PC/2 BLDLEVEL information (in C source modules added via macro
                                           concatenation) for BuildLevel.cmd to generate BLDLEVEL information.
                                           Version and Signature should correspond (at least for a GA build) */
#define BLDLEVEL_VENDOR         "(C) Roman_Stangl@at.ibm.com"
#define BLDLEVEL_VERSION        "2.20 (12, 2000)"
#define BLDLEVEL_SIGNATURE      0x00020014
#define BLDLEVEL_RELEASE        ((((BLDLEVEL_SIGNATURE & 0xFFFF0000)>>16)*100)+(BLDLEVEL_SIGNATURE & 0x000000FF))
#define BLDLEVEL_INFO           "PC/2 - Program Commander/2"
#define BLDLEVEL_PRODUCT        "PC/2"

                                        /* Include header for additions to the OS/2 Toolkits */
#include        "PC2Tlkt.h"

/*                                                                     *\
 * Now we can safely define more macros with values already occuring   *
 * in the dialog controls definition, because the dialog editor only   *
 * takes the first definition.                                         *
\*                                                                     */

                                        /* PM Message diagnostics support */
#define MESSAGEPOINT(timer, message)    {\
                                        pHP->ulAliveMsg##timer&=0xFFFF0000;\
                                        pHP->ulAliveMsg##timer|=(message & 0x0000FFFF);\
                                        }
                                        /* Checkpoint diagnostics support */
#define CHECKPOINT(timer, checkpoint)   {\
                                        pHP->ulAliveMsg##timer&=0x0000FFFF;\
                                        pHP->ulAliveMsg##timer|=((checkpoint & 0x0000FFFF)<<16);\
                                        }

                                        /* Draw the reduced Virtual Desktop layout into the
                                           in memory presentation space to avoid flickering.
                                           For debugging purposes it may be useful to draw
                                           the same things directly into the client area's
                                           presentation space, to be able the see what the
                                           drawing does. So you may want to replace pHP->hpsMemory
                                           by hpsClient then. */
#ifdef  SLOWVIDEO
#define         HPSOVERVIEWDRAW         (hpsClient)
#define         HPSSESSIONBARDRAW       (pHP->hpsSession)
#else
#define         HPSOVERVIEWDRAW         (pHP->hpsOverviewMemory)
#define         HPSSESSIONBARDRAW       (pHP->hpsSessionBarMemory)
#endif /* SLOWVIDEO */

                                        /* PC/2 semaphore to avoid loading PC/2 twice */
#define         PC2_SEM                 "\\SEM32\\PC2_SEM.SEM"
                                        /* PC/2 semaphore to protect DosKillThread()/ExceptionHandler
                                           processing of PC/2's Hang Protection */
#define         PC2_EXCPHANDLER_SEM     "\\SEM32\\PC2_EXCEPTIONHANDLER.SEM"
                                        /* Class name of the "Desktop" window handle (which
                                           is reserved in the older Toolkits) but it's a container */
#define         DESKTOP_CLASS           "#37"

                                        /* Oveview window frame creation flags */
#define         FCF_OVERVIEWWINDOW          FCF_ICON | FCF_SIZEBORDER | FCF_TASKLIST |            \
                                            FCF_TITLEBAR | FCF_HIDEBUTTON | FCF_ACCELTABLE |      \
                                            FCF_NOBYTEALIGN | FCF_AUTOICON
                                        /* Spooler window frame creation flags */
#define         FCF_SPOOLERWINDOW           FCF_ICON | FCF_SIZEBORDER | FCF_TITLEBAR |            \
                                            FCF_HIDEBUTTON | FCF_NOBYTEALIGN | FCF_AUTOICON
                                        /* WPS 2 PC/2 window frame creation flags */
#define         FCF_WPS2PC2WINDOW           FCF_BORDER | FCF_TITLEBAR | FCF_SYSMENU
                                        /* Quickswitch window frame creation flags */
#define         FCF_QUICKSWITCH             0
                                        /* SessionBar window frame creation flags */
#define         FCF_SESSIONBAR              0
                                        /* BubbleHelp window frame creation flags */
#define         FCF_BUBBLEHELP              0

                                        /* Define an additional session type of WPS Object
                                           representing any WPS object */
#define         PROG_WPSOBJECT              21
                                        /* Define an additional session type of Detached
                                           representing programs detached by DosExecPgm() */
#define         PROG_DETACHED               22
                                        /* Defined additional to SSF_CONTROL_* styles */
#define         SSF_CONTROL_AUTOSTARTED     0x1000
#define         SSF_CONTROL_PASSWORD        0x2000
#define         SSF_CONTROL_AUTOSTART       0x4000
                                        /* Defined additional to FAPPTYP_* styles */
#define         FAPPTYP_WINDOWSPROT31       0x1000
                                        /* Number of KEYDATA hotkeys defined for PC/2. 10 numeric keys
                                           + 26 alphabetic keys with RCTRL, CTRL or ALT or LWIN or
                                           CTRL+ALT pressed */
#define         KEYDATACOUNT                (36*5)
#define         CLASSNAMESIZE               (MAXNAMEL+1)

                                        /* Additional FID_* flags that are used to identify the additional
                                           titlebar controls that PC/2 may add to the titlebars of framewindows
                                           (even for other processed). Hopefully the identifiers used are
                                           not used in future OS/2 versions, as this may cause problems */
#define         FID_TITLEBARBEGIN           0x8040
                                        /* Close window (for pre WARP 4 only) */
#define         FID_CLOSEWINDOW             0x8040
                                        /* Roll window up to/down from the size of frame and titlebar only */
#define         FID_ROLLWINDOW              0x8041
                                        /* Activate context help, which causes F1 to be simulated for the
                                           control having the focus during the next mouse button click */
#define         FID_HELP                    0x8042
                                        /* Move window to the next adjacent Virtual Desktop */
#define         FID_MOVEVD                  0x8043
                                        /* Move window to the edge of Physical Desktop */
#define         FID_MOVEEDGE                0x8044
#define         FID_TITLEBAREND             0x8044

                                        /* Additional SC_* flags that are used for the WM_SYSCOMMAND messages
                                           the additional titlebar controls generate */
#define         SC_CLOSEWINDOW              FID_CLOSEWINDOW
#define         SC_ROLLWINDOW               FID_ROLLWINDOW
#define         SC_HELP                     FID_HELP
#define         SC_MOVEVD                   FID_MOVEVD
#define         SC_MOVEEDGE                 FID_MOVEEDGE
#define         SC_DESTROY                  FID_TITLEBAREND+1

                                        /* Statusflags for FRAMELIST status (Off = flag not set, On = flag
                                           set), which may cause the titlbar control's bitmap to be toggled  */
#define         FL_TITLEBARBEGIN            0x00000001
#define         FL_CLOSEWINDOW              0x00000001
#define         FL_ROLLWINDOW               0x00000002
#define         FL_HELP                     0x00000004
#define         FL_MOVEVD                   0x00000008
#define         FL_MOVEEDGE                 0x00000010

                                        /* Controlsflag mask for FRAMELIST status (On = titlebar control will
                                           be/was added to the titlebar */
#define         FL_CONTROLSMASK             (FL_CLOSEWINDOW|FL_ROLLWINDOW|FL_HELP|FL_MOVEVD|FL_MOVEEDGE)

                                        /* Statusflag mask for FRAMELIST status (On = titlebar control can be
                                           toggled, and 2 bitmaps exist for untoggled/toggled state) */
#define         FL_TOGGLEMASK               (FL_ROLLWINDOW)

                                        /* Additional SC_* flags that are used to communicate between
                                           the subclassed frames (will be in other processes) and the
                                           process of PC/2 */
                                        /* Subclass frame window (running in the thread context of
                                           the to be subclassed frame window procedure) */
#define         SC_CONTROLSINSTALL          0x8050
                                        /* Initialize FRAMELIST control structure (running in the thread
                                           context of the subclassed frame window procedure) */
#define         SC_CONTROLSINITIALIZE       0x8051
                                        /* Cleanup FRAMELIST control structure (running in the thread
                                           context of the subclassed frame window procedure) and
                                           remove frame subclassing */
#define         SC_CONTROLSCLEANUP          0x8052

                                        /* Number of additional titlebar controls (1 based) */
#define         NUM_TITLEBARCONTROLS        (FID_TITLEBAREND-FID_TITLEBARBEGIN+1)

                                        /* Additional WS_* flag, that is set if PC/2 has hidden a frame
                                           child control in order to prevent it from overlapping the rolled
                                           up titlebar (and the titlebar controls). This style flag is
                                           currently not used by PM, but this may break in the future as
                                           the bit is reserved. This must be verified with each new version
                                           of OS/2 */
#define         WS_HIDDENBYPC2              0x00080000L

                                        /* Additional SWP_* flag, that is set, when the user selected the
                                           option "Enable" for the user defined size & position in the
                                           Program Installation's Style page. This flag is used, if the
                                           option "Invisible" is not set. */
#define         SWP_MOVEWINDOWVISIBLE       0x20000000
                                        /* Additional SWP_* flag, that is set, when the user selected the
                                           option "Keep on active Desktop" in the Program Installation's
                                           Hotkey page. If this flag is set in MENUDATA.swpFlag,
                                           the window will excluded from moves by the thread processing
                                           WM_DESKTOPMOVE */
#define         SWP_NOMOVE                  0x40000000
                                        /* Additional SWP_* flag, that is set, when the user selected the
                                           option "Enable" for the user defined size & position in the
                                           Program Installation's Style page. SWP_MOVEWINDOWVISIBLE and
                                           SWP_MOVEWINDOW are only used to launch the menuentry, if the
                                           menuentry's window was created by PM (detected during enumeration)
                                           the window is moved to the defined position and both flags are
                                           reset in MENUDATA.swpFlag. */
#define         SWP_MOVEWINDOW              0x80000000

                                        /* All notebook dialog pages that are loaded into any of
                                           PC/2's notebooks are 215 * 162 dialog units in size.
                                           This dialog units are used to size the notebook so that
                                           a notebook dialog page fits into the notebook area where
                                           dialogs are loaded into. The dialog window is somewhat
                                           larger as required on 8514/A and VGA displays to have
                                           some spare place to size the notebook within */
#define         NOTEBOOKPAGE_CX             215
#define         NOTEBOOKPAGE_CY             162

                                        /* Percentage of Hotspot for Edge Activation of Overview
                                           window */
#define         DYNAMICOVERVIEW_HOTSPOT     3
                                        /* Percentage of Hotspot for switching between Virtual Desktops
                                           by moving the pointer over the surrounding row or column */
#define         VIRTUALDESKTOP_HOTSPOT      15


                                        /* Used for font selection */
#define         LCID_FONT                   1

                                        /* Macro which gets dynamically replaced by the boot drive */
#define         BOOTDRIVE                   "%BOOTDRIVE%"
#define         BOOTDRIVELEN                (sizeof(BOOTDRIVE))
                                        /* Macro which gets dynamically replaced by the path PC/2 was
                                           loaded from */
#define         PC2                         "%PC2%"
#define         PC2LEN                      (sizeof(PC2))
                                        /* Macro which gets dynamically replaced by the first CD drive */
#define         CDDRIVE                     "%CDDRIVE%"
#define         CDDRIVELEN                  (sizeof(CDDRIVE))

                                        /* Rendering for WPS 2 PC/2 and PC/2 communication */
#define         DRT_WPS2PC2                 "DRT_OBJECT"
#define         DRM_WPS2PC2                 "DRM_WPS2PC2"
#define         DRF_WPS2PC2                 "DRF_WPS2PC2"
#define         RMF_WPS2PC2                 "<"DRM_WPS2PC2","DRF_WPS2PC2">"

                                        /* Debug details flags */
#define         DEBUG_NO                    0
#define         DEBUG_LOW                   1
#define         DEBUG_ENTRY                 2
#define         DEBUG_FULL                  3

                                        /* QuickSwitch definitions */
#define         QUICKSWITCH_SWITCH          0
                                        /* Next 2 must be same as WINDOWLIST_NEXT and WINDOWLIST_PREVIOUS */
#define         QUICKSWITCH_NEXT            1
#define         QUICKSWITCH_PREVIOUS        2
#define         QUICKSWITCH_DISMISS         3
#define         QUICKSWITCH_SWITCHTO        4
#define         QUICKSWITCH_MOVETO          5
#define         QUICKSWITCH_ACTIVATE        6
#define         QUICKSWITCH_SWITCHTOWINDOW  7

                                        /* SessionBar creation definitions */
#define         SESSIONBAR_DESTROY          0
#define         SESSIONBAR_CREATE           1
#define         SESSIONBAR_RESIZE           2
#define         SESSIONBAR_SWITCH           3
#define         SESSIONBAR_SHOW             4

                                        /* SessionBar CPU Performance intervalls */
#define         PERFORMANCE_60S             0x00000001
#define         PERFORMANCE_5M              0x00000002
#define         PERFORMANCE_30M             0x00000004
#define         PERFORMANCE_1H              0x00000008

                                        /* SessionBar SB_* flags (required to resize/move windows
                                           that would overlap the SessionBar otherwise) */
#define         SB_POSITIONANY              0x00000001
#define         SB_POSITIONTOP              0x00000002
#define         SB_POSITIONBOTTOM           0x00000004
#define         SB_LASTTOP                  0x00000010
#define         SB_LASTBOTTOM               0x00000020
#define         SB_HIDDEN                   0x00000100
#define         SB_UNHIDEFROMTOP            0x00000200
#define         SB_UNHIDEFROMBOTTOM         0x00000400
#define         SB_UNHIDEDELAY              0x00000800

                                        /* SessionBar Statusarea flags */
#define         SB_STATUSLEFTJUSTIFIED      0x00000001
#define         SB_STATUSRIGHTJUSTIFIED     0x00000002

                                        /* Parameter for GetWindowList() */
#define         WINDOWLIST_QUERY            0
                                        /* Next 2 must be same as QUICKSWITCH_NEXT and QUICKSWITCH_PREVIOUS */
#define         WINDOWLIST_NEXT             QUICKSWITCH_NEXT
#define         WINDOWLIST_PREVIOUS         QUICKSWITCH_PREVIOUS
#define         WINDOWLIST_HANDLE           3
#define         WINDOWLIST_ACTIVEWINDOW     4
#define         WINDOWLIST_FREE             5
#define         WINDOWLIST_FINDPROCESS      6

                                        /* Parameter for GetDesktopWindows() */
#define         DESKTOPWINDOWS_QUERY        0
#define         DESKTOPWINDOWS_DESKTOP      1
#define         DESKTOPWINDOWS_WINDOWLIST   2
#define         DESKTOPWINDOWS_INDEX        3
#define         DESKTOPWINDOWS_PREINDEX     4
#define         DESKTOPWINDOWS_HANDLE       5
#define         DESKTOPWINDOWS_HOTKEY       6
#define         DESKTOPWINDOWS_CLICK        7
#define         DESKTOPWINDOWS_SWITCHTO     8
#define         DESKTOPWINDOWS_MOVETO       9
#define         DESKTOPWINDOWS_FREE         10
#define         DESKTOPWINDOWS_SWITCHTOWND  11
#define         PM_ERRORWINDOWS_QUERY       12

                                        /* QuickSwitch window:
                                           +--------------------------------------------+
                                           | +----------------------------------------+ |
                                           | | +---+                                  | |
                                           | | | I |      Centered SessionTitle       | |
                                           | | +---+                                  | |
                                           | +----------------------------------------+ |
                                           +--------------------------------------------+
                                           where I is the session's icon for the current
                                           resolution.
                                           The QuickSwitch window consists of a 3D border,
                                           and the Icon is within a 3D border */
#define         QUICKSWITCH_BORDER          2
#define         QUICKSWITCH_SPACE           3
#define         QUICKSWITCH_ICONSPACE       5
                                        /* QuickSwitch window's width exclusive default icon
                                           size (e.g. 40 for SVGA) and exclusive space for the
                                           text, which is a percentage of the screen size */
#define         QUICKSWITCH_CX              2*QUICKSWITCH_ICONSPACE*2*QUICKSWITCH_SPACE+4*QUICKSWITCH_BORDER
                                        /* QuickSwitch window's height exclusive default icon size */
#define         QUICKSWITCH_CY              2*QUICKSWITCH_ICONSPACE+2*QUICKSWITCH_SPACE+4*QUICKSWITCH_BORDER
                                        /* Percentage of screen for QuickSwitch text width, this results
                                           in a size of the QuickSwitch window depending on the screen
                                           resolution not pixels */
#define         QUICKSWITCH_SCALEX          40

                                        /* SessionBar window:
                                           +-------+------------------------------------------------------+
                                           | +---+ | +-+ +---+ +---+   +---+ +-+ +-+ +-+ +--------------+ |
                                           | | L | | |<| | 1 | | 2 |...| n | |>| |A| |P| | SessionTitle | |
                                           | +---+ | +-+ +---+ +---+   +---+ +-+ +-+ +-+ +--------------+ |
                                           +-------+------------------------------------------------------+
                                           where L is the PC/2 launch button
                                           <,> are the scroll buttons
                                           1...n are the running session's icons
                                           A... (optional) APM graphical status
                                           P... (optional) CPU Performance graphical status
                                           SessionTitle is the name of the session currently active, or
                                           the name of session 1...n when the mouse pointer is above it
                                           The QuickSwitch window consists of a 3D border,
                                           and the Icon are within a 3D border, and the icons are framed
                                           itself */
#define         SESSIONBAR_BORDER           2
#define         SESSIONBAR_SPACE            3
#define         SESSIONBAR_ICONFRAME        2
                                        /* SessionBar window's width is the screen size */
                                        /* SessionBar window's height is (exclusive default icon size and icon
                                           (2*SESSIONBAR_SPACE+2*SESSIONBAR_BORDER)) adjusted by the icon
                                           scaling factor */
                                        /* SessionBar's PC/2 launch button width */
#define         SESSIONBAR_LAUNCHWIDTHVGA   80
#define         SESSIONBAR_LAUNCHWIDTHBGA   100
                                        /* Percentage of screen for SessionBar's session name text width, this
                                           results in a size of the session name rectangle depending on the screen
                                           resolution not pixels (corrected that an non-floating number of
                                           session icons is drawn) */
#define         SESSIONBAR_NAMESCALEX       15
                                        /* Percentage of screen for SessionBar's CPU Performance width, this
                                           results in a size of the CPU Performance rectangle depending on the screen
                                           resolution not pixels (corrected that an non-floating number of
                                           session icons is drawn). */
#define         SESSIONBAR_CPUSCALEX        10
                                        /* "Norm" intervall size in pixels for SessionBar's CPU Performance width.
                                           This will be adjusted (multiplied by 2, 4, ... or divided by 2, 3, ...)
                                           so that a single pixel represents a discret time intervall, e.g. 1
                                           second or 1 minute, including to fit all processor graphics into the
                                           SESSIONBAR_CPUSCALEX width */
#define         SESSIONBAR_CPUPIXELS        60
                                        /* Space between 2 CPU's Performance graphics */
#define         SESSIONBAR_CPUSPACE         3
                                        /* APM status area minimum width */
#define         SESSIONBAR_APMPIXELS        5

                                        /* BubbleHelp window:
                                           +------------------------+
                                           | +--------------------+ |
                                           | | Centered Help Text | |
                                           | +--------------------+ |
                                           +------------------------+
                                           The BubbleHelp window consists of a 3D border,
                                           and some (multiline) text inside */
#define         BUBBLEHELP_BORDER           1
#define         BUBBLEHELP_SPACE            2
                                        /* BubbleHelp definitions */
#define         BUBBLEHELP_HIDE             0
#define         BUBBLEHELP_SHOW             1
#define         BUBBLEHELP_RELOCATE         2
#define         BUBBLEHELP_TEXT             3

                                        /* Timer intervall (in milliseconds). Warning: logic
                                           only works between 1 and 1000 ms, otherwise we have
                                           to change integer mathematics or use floating point */
#define         TIMERINTERVALL100MS         100
#define         TIMERINTERVALLMS            500
#define         TICKSPERSECOND              (1000/TIMERINTERVALLMS)
#define         TICKSPERMINUTE              (60*TICKSPERSECOND)

                                        /* Multiplier for timerticks to get Sliding Focus delay
                                           in ms */
#define         SLIDINGFOCUSMULTIPLIER      (100)

                                        /* When drawing the Popup-Menu during ownerdraw, the menuentry's
                                           icon is drawn too, similar to WIN95's taskbar */
                                        /* Frame thickness */
#define         ICONFRAME                   2
                                        /* Space between icon and text */
#define         ICONORTEXTSPACE             2
                                        /* Space between icons frame and icon itself */
#define         ICONSPACE                   1

/*--------------------------------------------------------------------------------------*\
 *                                                                                      *
 * +-----+-----+-----+                                                                  *
 * | www |    www========== Some windows, only two seen by the user on the display.     *
 * | www |     |     |                                                                  *
 * +-----+-----www---+===== Virtual Desktop (9fold-size of physical Desktop).           *
 * |     | www wwww  |                                                                  *
 * |  +----www | ww  |                                                                  *
 * +--|ww  www-+-----+                                                                  *
 * |  |ww   |============== Physical Desktop somewhere within virtual Desktop. The      *
 * |  +-----+  |     |      physical Desktop is the part of the virtual Desktop which   *
 * +-----+-----+-----+      the user visually can see, therefor large parts of the      *
 *                          virtual Desktop can't be ssen by the user. He must use      *
 * his pointing device, the slide the physical Desktop around the virtual Desktop, by   *
 * moving the pointer over the surrounding rows and columns of the physical screen.     *
 *                                                                                      *
\*------------------------------------------------------------------------------------- */
                                        /* Status bits defined for bitmapped ulMoveFlag and
                                           ulOveviewFlag (in PC2HOOK.DLL) */
                                        /* Set if we move all windows right (our physical
                                           Desktop will move left on the virtual Desktop) */
#define         MOVEXR              0x0001
                                        /* Set if we move all windows left */
#define         MOVEXL              0x0002
                                        /* Set if we move all windows up (our physical
                                           Desktop will move down on the virtual Desktop */
#define         MOVEYU              0x0004
                                        /* Set if we move all windows down */
#define         MOVEYD              0x0008
                                        /* Set if we are to move all windows, but the
                                           CLICK2MOVE flag is set, and therefor we require
                                           a click before we really move */
#define         MOVED4CLICK         0x0010
                                        /* Set if the pointer position changed for the
                                           WM_MOUSEMOVE message */
#define         PTRPOSCHANGED       0x0020
                                        /* A delayed Virtual Desktop switch is under way */
#define         PENDINGSWITCH       0x0040

                                        /* Result flags for IsWindowInRectangle(), a function that
                                           is used to find out if a PM window would be covered by the
                                           Physical Desktop (anywhere within the Virtual Desktop)
                                           if the Physical Desktop would cover a certain rectangle. */
#define         XYMASK              0x000F
                                        /* The window's rectangle may fit completely (including sizing
                                           border and titlebar) horizontally and/or vertically */
#define         XWINDOW             0x0001
#define         YWINDOW             0x0002
                                        /* The window rectangle's center may fit horizontally and/or
                                           vertically. If the center is inside the rectangle, at least
                                           half (but usually more) of the window would be covered */
#define         XCENTER             0x0004
#define         YCENTER             0x0008
                                        /* The window's rectangle may be outside the Virtual Desktop,
                                           then accept the Desktop that is the nearest one */
#define         XRIGHT              0x0010
#define         YTOP                0x0020
#define         XLEFT               0x0040
#define         YBOTTOM             0x0080

                                        /* Status bits defined for bitmapped ulStatusFlag1 */
                                        /* Set if the user requested a sliding focus */
#define         SLIDINGFOCUS        0x00000001
                                        /* Set if the user requested a virtual Desktop */
#define         VIRTUALDESKTOP      0x00000002
                                        /* Set if the user also wants the WPS to move */
#define         MOVEDESKTOP         0x00000004
                                        /* Set if the user wants to click before Desktop
                                           moves */
#define         CLICK2MOVE          0x00000008
                                        /* Set if the user wants an Overview window */
#define         OVERVIEW            0x00000010
                                        /* Set if sliding focus should preserve the Z-order of
                                           the windows */
#define         PRESERVEZORDER      0x00000020
                                        /* Set if mouse button 2 click downs on titlebars should
                                           set the window to the bottom of all windows */
#define         BUTTON2ZORDER       0x00000040
                                        /* Set if Overview window should be the topmost window,
                                           even when another window has the focus */
#define         KEEPONTOPOV         0x00000080
                                        /* Set if the WPS should be expanded to the whole Virtual Desktop */
#define         EXPANDWPS           0x00000100
                                        /* Set if a bitmap is set for PM Background */
#define         BACKGROUNDBITMAP    0x00000200
                                        /* Set if the first item of the Popup-Menu should be selected */
#define         SELECTFIRSTITEM     0x00000400
                                        /* Set if the user wants to hide PC/2's titlebar */
#define         HIDETITLEBAR        0x00000800
                                        /* Set if the user wants to hide PC/2's smarticon bar */
#define         HIDESMARTICONBAR    0x00001000
                                        /* Set if the user wants PC/2's Spooler Window even when
                                           PC/2 is running on top of the WPS */
#define         SHOWSPOOLERWINDOW   0x00002000
                                        /* Set if the user wants normal OS/2 shutdown, that is
                                           the user gets asked as you would select shutdown from
                                           the WPS's conext menu */
#define         NORMALSHUTDOWN      0x00004000
                                        /* Set if Autostart should be ignored when invoking PC/2
                                           (e.g. for debugging purposes) */
#define         IGNOREAUTOSTART     0x00008000
                                        /* Set if Hotkeys should be detected only when either PC/2,
                                           the Window List or the WPS has the focus */
#define         HOTKEY4PC2ONLY      0x00010000
                                        /* Set if Scheduler should be disabled when invoking PC/2
                                           (e.g. for debugging purposes) */
#define         IGNORESCHEDULER     0x00020000
                                        /* Set if no verification is requested for Fast Shutdown */
#define         NOVERIFYSHUTDOWN    0x00040000
                                        /* Set if automatic reboot is requested for Fast Shutdown */
#define         SHUTDOWNREBOOT      0x00080000
                                        /* Set if Popup-Menu should be accessible by clicking over
                                           any windows (usefull only with mouse button 3 without
                                           degrading system usability) */
#define         POPUPSYSTEMWIDE     0x00100000
                                        /* Set if sliding focus activates only on window
                                           level instead of activating the control beneath
                                           the mouse pointer */
#define         FOCUSWINDOWLEVEL    0x00200000
                                        /* Set if the center of the Popup-Menu should be selected */
#define         SELECTCENTERITEM    0x00400000
                                        /* Set if mouse button 2 click on frame window borders
                                           allows the window to be moved */
#define         BUTTON2MOVE         0x00800000
                                        /* Set if overview window displays titles for desktop windows */
#define         OVERVIEWTITLES      0x01000000
                                        /* Set if scrolling between Virtual Desktops should not stop
                                           unless the user removes the mouse pointer from the border
                                           rows and columns (when CLICK2MOVE is not set) */
#define         AUTOSCROLL          0x02000000
                                        /* Set if PC/2 should automagically switch to the Virtual
                                           Desktop the active window is on (this may cause some
                                           switching that appears random, but ensures that you are
                                           always in from of the window having the input focus) */
#define         FOLLOWFOCUS         0x04000000
                                        /* Draw 3D border around overview window */
#define         OVERVIEW3DBORDER    0x08000000
                                        /* Change mouse cursor when pointer is over overview window */
#define         OVERVIEWCURSOR      0x10000000
                                        /* Optimize window title color for maximum visibility */
#define         OVERVIEWAUTOCOLOR   0x20000000
                                        /* Switch desktops with one click instead of double-click */
#define         ONECLICKSWITCH      0x40000000


                                        /* Status bits defined for bitmapped ulStatusFlag2 */
#define         ADVANCEDMARKING     0x00000001
                                        /* Set if the user wants us to automatically select the menuentry
                                           below the mouse pointer if a menuentry was selected previously */
#define         DYNAMICMENU         0x00000002
                                        /* Set if the user wants PC/2's SessionBar window */
#define         SHOWSESSIONBAR      0x00000004
                                        /* Set if PC/2 security is enabled */
#define         PC2SECURITY         0x00000008
                                        /* Set if Lockup is enabled */
#define         ENABLELOCKUP        0x00000010
                                        /* Set if Lockup should be activated after booting */
#define         LOCKUPONBOOT        0x00000020
                                        /* Set if Popup-Menu should include the menuentry's icon (by
                                           using the MIS_OWNERDRAW style) */
#define         POPUPICON           0x00000040
                                        /* Set while PC/2 is allowed to add icons to other process' frame
                                           windows titlebars */
#define         TITLEBARICONS       0x00000080
                                        /* Set when PC/2 should prevent Desktop child frame windows from
                                           becoming larger than a certain size (e.g. to use the hardware
                                           Virtual Desktop feature on IBM ThinkPads) */
#define         HWVIRTUALDESKTOP    0x00000100
                                        /* Set when PC/2 should activate debugging features (equivalent
                                           to DUMPPROCESS and SUPPRESSPOPUPS in CONFIG.SYS */
#define         ENABLEDIAGNOSTICS   0x00000200
                                        /* Set when PC/2 should enable APM support (useful when
                                           running PC/2 as the WPS as there is no Power WPS Object then) */
#define         ENABLEAPM           0x00000400
                                        /* Set if no verification is requested for PowerDown */
#define         NOVERIFYPOWERDOWN   0x00000800
                                        /* Set when PC/2 should suspend for PowerDown events */
#define         POWERDOWNSUSPEND    0x00001000
                                        /* Set when PC/2 should power off for PowerDown events */
#define         POWERDOWNPOWEROFF   0x00002000
                                        /* Set if PowerDown timer is enabled */
#define         ENABLEPOWERDOWN     0x00004000
                                        /* Set if sliding focus should preserve the Z-order of
                                           the windows */
#define         KEEPONTOPSB         0x00008000
                                        /* Set if SessionBar should display the default format
                                           of Date/Time instead of one derived from the settings
                                           of the Country WPS object */
#define         DEFAULTDATETIMESB   0x00010000
                                        /* Set if button 2 is used for Advanced Marking paste */
#define         ADVMARKINGBUTTON2   0x00020000
                                        /* Set if button 3 is used for Advanced Marking paste */
#define         ADVMARKINGBUTTON3   0x00040000
                                        /* Set if APM should not be touched at all, because
                                           under Warp Server SMP enabling APM causes a kernel hang */
#define         SUPPORTAPM          0x00080000
                                        /* Set when SessionBar should be activated
                                           dynamically */
#define         DYNAMICSESSIONBAR   0x00100000
                                        /* Set when SessionBar should show CPU Performance graphics */
#define         CPUPERFORMANCESB    0x00200000
                                        /* Set when SessionBar CPU Performance should draw in "fill" mode */
#define         PERFORMANCEFILLSB   0x00400000
                                        /* Set when SessionBar should not display session name area */
#define         NOSESSIONNAMESB     0x00800000
                                        /* Set when SessionBar should not display date/time information */
#define         NODATETIMESB        0x01000000
                                        /* Set when SessionBar should display texts instead of icons */
#define         DISPLAYTEXTSB       0x02000000
                                        /* Set when Dynamic Menuselection should recursively cascade all
                                           submenus */
#define         CASCADEMENU         0x04000000
                                        /* Set when Dynamic Menuselection should also be performed by
                                           keyboard operations of a menu */
#define         DYNAMICMENUKBD      0x08000000
                                        /* Set when the hidden Overview window should be unhidden by
                                           moving the mouse pointer at a edge of the screen for a
                                           configurable delay */
#define         DYNAMICOVERVIEW     0x10000000
                                        /* Set when SessionBar CPU Performance should not draw a grid */
#define         NOGRIDSB            0x20000000
                                        /* Set if the user wants PC/2's QuickSwitch window */
#define         ENABLEQUICKSWITCH   0x40000000
                                        /* Set when SessionBar should display BubbleHelp */
#define         BUBBLEHELPSB        0x80000000

                                        /* Status bits defined for bitmapped ulStatusFlag3 */

                                        /* Flags only used during PC/2 running in
                                           ulRuntimeFlag */
                                        /* Set if /INSTALL commandline parameter was given */
#define         INSTALLATIONHELP    0x00000001
                                        /* Set when configuration dialog should be displayed,
                                           because Popup-Menu is empty */
#define         DISPLAYCONFIGDIALOG 0x00000002
                                        /* Set when used closed PC/2 via Window List, in this case
                                           ignore current WM_QUIT to shutdown PC/2 which than posts
                                           WM_QUIT to itself again */
#define         QUITFROMWINDOWLIST  0x00000004
                                        /* Set when closed via system ShutDown, or PC/2 should exit
                                           by request via its own Scheduler, in this cases ignore
                                           WM_QUIT to shutdown PC/2 which than posts WM_QUIT to
                                           itself again */
#define         QUITFROMSHUTDOWN    0x00000008
                                        /* Set when PC/2 is the WPS process running */
#define         PC2RUNNINGASWPS     0x00000010
                                        /* Set when PC/2 is the Protshell process running */
#define         PC2RUNNINGASPROTSHELL \
                                    0x00000020
                                        /* Set when the Spooler or PMSPL.DLL can't be accessed */
#define         PC2SPOOLERERROR     0x00000040
                                        /* Set while the Popup Menu is not loaded completely to
                                           prevent display of an incomplete menu */
#define         POPUPMENULOADING    0x00000080
                                        /* Set when an update of the Popup Menu's linked list is in
                                           progress by the I/O thread. This flag is used when loading
                                           or unloading the Popup menuentries icons, which may be a very
                                           time consuming task */
#define         POPUPMENUUPDATE     0x00000100
                                        /* Set when an the part/all of the Popup Menu's linked list is about
                                           to be deleted I/O thread. This flag prevents the Popup Menu to
                                           be deallocated, before all resources allocated for the menuentries
                                           have been freed */
#define         POPUPMENUDELETE     0x00000200
                                        /* Set when loading the icons for the Popup-Menu can be improved by
                                           setting the fully qualified path we loaded the executable's icon
                                           from in the cache MENUDATA.PgmFullPathExecutable. This
                                           would require the user to save the Popup-Menu to file */
#define         ICONCACHEACTIVE     0x00000400
                                        /* Set when an OS/2 System error (e.g. trap information or drive
                                           not ready) is displayed, which should stop all PM features (e.g.
                                           Sliding Focus, switching the Virtual Deskop,...) to prevent the
                                           user not being able to dismiss this system modal message box or
                                           8o get into a state not being able to interact with PM anymore */
#define         SYSTEMERRORACTIVE   0x00000800
                                        /* Set when the SessionBar was created to allow the window
                                           enumeration to inform the SessionBar of the windows available */
#define         INITSESSIONBAR      0x00001000
                                        /* Set while PC/2's WinSendMsg() hook is allowed to subclass
                                           all windows' titlebars to add Titlebar Smarticons */
#define         SUBCLASSICONS       0x00002000
                                        /* Set when the Popup Menu was displayed via using the
                                           keyboard to avoid Dynamic Menuselection then */
#define         POPUPMENU4KEYBOARD  0x00004000
                                        /* Set when PC/2 waits for timer to tigger cleanup processing */
#define         PC2CLEANUP          0x00008000
                                        /* Set when PC/2 should exit via ALT+CTRL+F3 */
#define         PC2HIDDENEXIT       0x00010000
                                        /* Set when a internal error forces us to close PC/2 */
#define         PC2EMERGENCYEXIT    0x00020000
                                        /* Set when PC/2 should report the current frame window handle
                                           to the Desktop Dialogs Spy notebook page */
#define         SPYFRAMEWINDOW      0x00040000
                                        /* Set when PC/2 detects APM support */
#define         APMSUPPORTED        0x00080000
                                        /* Set when PC/2 detects a battery based PC from APM */
#define         APMPOWERSUPPORT     0x00100000
                                        /* Set when PC/2 should suspend switching to the active window */
#define         SUSPENDSWITCHACTIVE 0x00200000
                                        /* Set when PC/2 detects networking support installed */
#define         TCPIPSUPPORTED      0x00400000
                                        /* Set when /NOAUTOSTART commandline option specified */
#define         NOAUTOSTART         0x00800000
                                        /* Set when /NOSCHEDULER commandline option specified */
#define         NOSCHEDULER         0x01000000
                                        /* Set when /NOCLEANUP commandline option specified */
#define         NOCLEANUP           0x02000000
                                        /* Set when /NOPROMPT commandline option specified */
#define         NOPROMPT            0x04000000
                                        /* Set when last Popup Menu enumeration showed no pending
                                           Autostart */
#define         NOPENDINGAUTOSTART  0x08000000

                                        /* Background bitmap style flag (lower USHORT contains tile count */
                                        /* Normal style */
#define         BITMAPNORMAL        0x00010000
                                        /* Scaled */
#define         BITMAPSCALED        0x00020000
                                        /* Tiled */
#define         BITMAPTILED         0x00040000

                                        /* Bitmapped flag for ulThreadReady flag, that is used by
                                           the client window procedure to serialize access to the
                                           working thread, to avoid queueing of work */
                                        /* Working thread is just doing his work of enumerating
                                           windows for new, moved, sized or destroyed windows  */
#define         THREADWINDOWBUSY    0x00000001
                                        /* Working thread is just doing his work of moving from
                                           one Virtual Desktop to another */
#define         THREADMOVEBUSY      0x00000002
                                        /* Working thread was busy when more window enumeration
                                           requests were available, save fact and do the work
                                           when not busy */
#define         THREADWINDOWQUEUED  0x00000004

#define         DESKTOP_SWITCH_EVENT    (ULONG)((pHP->ulStatusFlag1 & ONECLICKSWITCH) ? \
                                                 WM_BUTTON1CLICK : WM_BUTTON1DBLCLK)

                                        /* Some macros to work with RGB colors */
#define         GETRVALUE(rgb)          ((BYTE)(rgb))
#define         GETGVALUE(rgb)          ((BYTE)(((USHORT)(rgb)) >> 8))
#define         GETBVALUE(rgb)          ((BYTE)((rgb)>>16))
#define         RGB(r,g,b)              ((ULONG) (((BYTE)(r)|((USHORT)((BYTE)(g))<<8))|(((ULONG)(BYTE)(b))<<16)))
                                        /* Get color intensity squared */
#define         SQUARERGBINTENSITY(rgb) (int)GETRVALUE(rgb) * (int)GETRVALUE(rgb) + \
                                        (int)GETGVALUE(rgb) * (int)GETGVALUE(rgb) + \
                                        (int)GETBVALUE(rgb) * (int)GETBVALUE(rgb)

                                        /* Style definitions for DrawWindow() functions */
#define         BACKGROUND_WND  0x0001  /* Set for 3D effect of a deeper than surface window */
#define         FOREGROUND_WND  0x0002  /* Set for 3D effect of a higher than surface window */
#define         BORDERONLY_WND  0x0001  /* Set if only the border of a window should be drawn */


#define         EF_SIZE255      (CCHMAXPATHCOMP-1)
#define         PASSWORDSIZE    32      /* Lockup password length */
#define         PATHSIZE        96      /* Configuration dialog path info length */
#define         LENENVENTRY     1024    /* Length of a single EnvironmentVariable=Value entry */

                                        /* Functions loaded dynamically and called via function pointers */
typedef ULONG   (EXPENTRY PFFUNCPTR1)   (void);
typedef BOOL    (EXPENTRY PFFUNCPTR2)   (HAB hab, PQMSG pqmsg, ULONG option);
typedef void    (EXPENTRY PFFUNCPTR3)   (HAB hab, PSMHSTRUCT pSmh, BOOL bInterTask);
typedef PCH     (EXPENTRY PFFUNCPTR4)   (HWND hwndClient, HWND hwnd, PSZ pcIn, USHORT usIn,\
                                         USHORT *pusOut, USHORT *pusWinMode);
typedef ULONG   (EXPENTRY PFFUNCPTR5)   (HMODULE hDLLPc2Hook);
typedef MRESULT (EXPENTRY PFFUNCPTR6)   (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

                                        /* Functions used by PC2Net.c from the C-Runtime passed via
                                           function pointers */
#define             C_RT_CONV               _Optlink
typedef void        *(C_RT_CONV PFN_MALLOC)(size_t);
typedef void        (C_RT_CONV PFN_FREE)(void *);
typedef int         (C_RT_CONV PFN_STRICMP)(const char *, const char *);
typedef int         (C_RT_CONV PFN_STRNICMP)(const char *, const char *, size_t);
typedef char        *(C_RT_CONV PFN_STRUPR)(char *);
typedef char        *(C_RT_CONV PFN_STRDUP)(const char *);
typedef int         (C_RT_CONV PFN_FPRINTF)(FILE *, const char *, ...);
typedef FILE        *(C_RT_CONV PFN_FOPEN)(const unsigned char *, const unsigned char *);
typedef int         (C_RT_CONV PFN_FCLOSE)(FILE *);
typedef time_t      (C_RT_CONV PFN_MKTIME)(struct tm *);
typedef char        *(PFN_STRIPLEADING)(char *);
typedef char        *(PFN_STRIPTRAILING)(char *);

typedef struct  _THREADDATA             THREADDATA;
typedef struct  _SCHEDULERDATA          SCHEDULERDATA;
typedef struct  _SESSIONDATA            SESSIONDATA;
typedef struct  _MENUDATA               MENUDATA;
typedef struct  _COMMANDLINEPARAMS      COMMANDLINEPARAMS;
typedef struct  _COMMANDLINEENVIRONMENT COMMANDLINEENVIRONMENT;
typedef struct  _DESKTOPDIALOGPARAMS    DESKTOPDIALOGPARAMS;
typedef struct  _HOOKPARAMETERS         HOOKPARAMETERS;
typedef struct  _NBPAGE                 NBPAGE;
typedef struct  _KEYDATA                KEYDATA;
typedef struct  _WINDOWDATA             WINDOWDATA;
typedef struct  _WINDOWS                WINDOWS;
typedef struct  _SPOOLERRECORD          SPOOLERRECORD;
typedef struct  _SPOOLEROBJECT          SPOOLEROBJECT;
typedef struct  _WPSOBJECTLIST          WPSOBJECTLIST;
typedef struct  _ENVIRONMENTLIST        ENVIRONMENTLIST;
typedef struct  _COLORWINDOW            COLORWINDOW;
typedef struct  _FRAMEDATA              FRAMEDATA;
typedef struct  _FRAMELIST              FRAMELIST;
typedef struct  _SWITCHLIST             SWITCHLIST;
typedef struct  _POPUPMENU4KBD          POPUPMENU4KBD;
typedef struct  _SPYFRAME               SPYFRAME;
typedef struct  _SHUTDOWNPARAMETERS     SHUTDOWNPARAMETERS;
typedef struct  _CPUPERFORMANCE         CPUPERFORMANCE;
typedef struct  _CPUTICKS               CPUTICKS;
typedef struct  _CRUNTIME               CRUNTIME;
typedef struct  _URL                    URL;
typedef struct  _AUTOUPDATE             AUTOUPDATE;
typedef struct  _TAGSTATUS              TAGSTATUS;
typedef struct  _TAGREGISTER            TAGREGISTER;
typedef struct  _TAGNEWS                TAGNEWS;
typedef struct  _TAGUPDATE              TAGUPDATE;
typedef struct  _EXCEPTIONRECORD        EXCEPTIONRECORD;

#define ENTRYSUBMENU            0       /* Popup-Menu entry is a (sub)menu */
#define ENTRYMENUITEM           1       /* Popup-Menu entry is a program */
#define ENTRYCONTROL            2       /* Popup-Menu entry is a PC/2 configuration control */
#define ENTRYEMPTY              3       /* Popup-Menu entry is empty */

#define WPSOBJECTPROGRAM        0       /* WPSOBJECTLIST entry for WPProgram Objects */
#define WPSOBJECTFOLDER         1       /* WPSOBJECTLIST entry for WPFolder Objects */
#define WPSOBJECTOBJECT         2       /* WPSOBJECTLIST entry for any other WPS Objects */

#define PI_PAGE_1               0       /* Index of 1st program installation notebook page */
#define PI_PAGE_2               1
#define PI_PAGE_3               2
#define PI_PAGE_4               3
#define PI_PAGE_5               4
#define PI_PAGE_6               5
#define PI_PAGE_7               6

#define DD_PAGE_1A              0       /* Index of 1st desktop dialog notebook page */
#define DD_PAGE_1B              1
#define DD_PAGE_2               2
#define DD_PAGE_3               3
#define DD_PAGE_4               4
#define DD_PAGE_5               5
#define DD_PAGE_6               6
#define DD_PAGE_7               7
#define DD_PAGE_8A              8
#define DD_PAGE_8B              9
#define DD_PAGE_9               10
#define DD_PAGE_10              11

#define MI_PAGE_1               0       /* Index of 1st menu installation notebook page */
#define MI_PAGE_2               1

#define CC_PAGE_1               0       /* Index of 1st control configuration notebook page */
#define CC_PAGE_2               1

                                        /* PC/2's first thread */
#define ID_MAINTHREAD           0x00000001
                                        /* I/O thread ID */
#define ID_IOTHREAD             0x00000002
                                        /* Network thread ID */
#define ID_NETTHREAD            0x00000003
                                        /* Working thread ID */
#define ID_WORKINGTHREAD        0x00000004
#define ID_LASTTHREAD           ID_NETTHREAD

#pragma pack(4)
struct  _THREADDATA                     /* Structure pointing to the control data of a thread that runs
                                           an object window (which doesn't take part of PM's synchronous
                                           focus processing and therefor does not need to obey the 1/10s
                                           rule to process messages) */
{
USHORT          usCb;                   /* Size of structure */
ULONG           ulThreadId;             /* PC/2 Thread id */
TID             tidThread;              /* OS/2 Thread id */
HAB             habThread;              /* Anchor block */
HMQ             hmqThread;              /* Message queue handle */
QMSG            qmsgThread;             /* Queue message structure */
PFNWP           pfnwpThread;            /* Window procedure */
HWND            hwndThread;             /* Object window handle */
};

struct  _KEYDATA                        /* Structure that defined 1 hotkey */
{
USHORT          usFlags;                /* Keyboard control codes: KC_CTRL, KC_ALT,... */
USHORT          usCh;                   /* Keyboard character value: 0, 1,... */
UCHAR           ucScanCode;             /* Keyboard character scan code */
BOOL            bUsed;                  /* True if this hotkey is defined */
MENUDATA        *pMenuData;             /* Pointer to MenuData structure where is Hotkey
                                           is defined */
};

                                        /* Flags defined for scheduler */
                                        /* Scheduler is disabled */
#define         SCHEDULE_NONE       0x00000001
                                        /* Schedule hourly */
#define         SCHEDULE_HOURLY     0x00000002
                                        /* Schedule daily */
#define         SCHEDULE_DAILY      0x00000004
                                        /* Schedule weekly */
#define         SCHEDULE_WEEKLY     0x00000008
                                        /* Schedule monthly */
#define         SCHEDULE_MONTHLY    0x00000010
                                        /* Schedule only once */
#define         SCHEDULE_ONCE       0x00000020
                                        /* Schedule data selections should be saved into the
                                           configuration file, even when the Scheduler is disabled */
#define         SCHEDULE_SAVEANYWAY 0x00000040

struct _SCHEDULERDATA                   /* Control structure for scheduler */
{
ULONG           ulSchedulerFlag;        /* Scheduler control */
ULONG           ulStartingMinute1;      /* Bitmapped starting minute(s) (0...31) */
ULONG           ulStartingMinute2;      /* Bitmapped starting minute(s) (32...59) */
ULONG           ulStartingHour;         /* Bitmapped starting hour(s) (0...23) */
ULONG           ulStartingWeekday;      /* Bitmapped starting weekday(s) (0...6) */
ULONG           ulStartingDay;          /* Bitmapped starting day(s) (0...30) */
ULONG           ulStartingMonth;        /* Bitmapped starting month(s) (0...11) */
};

struct _SESSIONDATA                     /* Control structure to start a new session */
{
ULONG           id;                     /* Session ID for menu ressources */
USHORT          StatusFlag;             /* Status flag */
UCHAR           PgmTitle[MAXNAMEL+1];   /* Session title */
UCHAR           IconFile[CCHMAXPATH];   /* Session icon file */
                                        /* Session (sub)title on window's titlebar */
UCHAR           WindowTitle[MAXNAMEL+1];
UCHAR           PgmName[CCHMAXPATHCOMP];/* Session path and filename */
                                        /* Session working directory */
UCHAR           PgmDirectory[CCHMAXPATHCOMP];
                                        /* Session parameter */
UCHAR           PgmInputs[EF_SIZE255+1];
PSZ             PgmDosSettings;         /* DOS Settings for OS/2 DOS and WIN-OS2 sessions */
PSZ             PgmLeadingEnvironment;  /* Optional environment addition/replacement of environment inherited
                                           from PC/2 to the started application before any Environment Space */
PSZ             PgmTrailingEnvironment; /* Optional environment addition/replacement of environment inherited
                                           from PC/2 to the started application after any Environment Space */
PSZ             EnvironmentSpaces;      /* Environment Spaces */
USHORT          SessionType;            /* Session type */
USHORT          PgmControl;             /* Session control */
USHORT          FgBg;                   /* Start session in foreground/background */
SHORT           InitXPos;               /* X position */
SHORT           InitYPos;               /* Y position */
SHORT           InitXSize;              /* X size */
SHORT           InitYSize;              /* Y size */
KEYDATA         KeyData;                /* Hotkey data, if none defined, usCh=0, bUsed=0, pMenuData=NULL */
ULONG           SwpFlag;                /* Activation on hotkey SWP flag: SWP_RESTORE or SWP_MAXIMIZE,
                                           SWP_NOMOVE */
LONG            AutostartDelay;         /* Autostart delay in seconds */
USHORT          PriorityClass;          /* Priority Class */
SHORT           PriorityDelta;          /* Priority Delta */
SCHEDULERDATA   SDScheduler;            /* Scheduler data */
};


                                        /* Autostart delay */
#define         AUTOSTARTDELAY_MIN  0
#define         AUTOSTARTDELAY_MAX  3600
#define         AUTOSTARTDELAY_SEC  5
                                        /* Status flags */
                                        /* Set if we do a request to load an icon of an menuentry just after
                                           the function is entered, because WinLoadFileIcon() may preempt the
                                           loading thread, and another thread may try to load the same icon,
                                           if we only test for valid pointer handles returned */
#define         ICONLOADED          0x0001
                                        /* Set if the menuentry's icon was loaded by WinLoadPointer() from
                                           our resource instead with WinLoadFileIcon() */
#define         ICONFROMRESOURCE    0x0002
                                        /* Set if the menuentry's icon was loaded by WinLoadPointer() from
                                           the file specified by WinLoadFileIcon() */
#define         ICONFROMICONFILE    0x0004
                                        /* Set if the user wants to get the Startup Environment Spaces
                                           dialog to be invoked */
#define         STARTUPENVIRONMENTS 0x0008

struct _MENUDATA                        /* Control structure to build popup menu */
{
USHORT          Item;                   /* MENUITEM or SUBMENU */
USHORT          StatusFlag;             /* Status flag */
HWND            hwndItem;               /* Window handle within Popup-Menu */
ULONG           id;                     /* Session ID for menu ressources */
PSZ             PgmTitle;               /* Session title */
PSZ             IconFile;               /* Session icon file */
PSZ             WindowTitle;            /* Session (sub)title on window's titlebar, in Window List or
                                           window's class name */
PSZ             PgmName;                /* Session path and filename */
PSZ             PgmDirectory;           /* Session working directory */
PSZ             PgmFullPathExecutable;  /* Fully qualified path to load icon for Popup Menu
                                           from to avoid scanning the path */
PSZ             PgmInputs;              /* Session parameter */
PSZ             PgmDosSettings;         /* Session settings for DOS applications */
PSZ             EnvironmentSpaces;      /* Session environment spaces separated by \r\n */
PSZ             PgmLeadingEnvironment;  /* Session leading environment settings */
PSZ             PgmTrailingEnvironment; /* Session trailing environment settings */
USHORT          SessionType;            /* Session type */
USHORT          PgmControl;             /* Session control */
USHORT          FgBg;                   /* Start session in foreground/background */
SHORT           InitXPos;               /* X position */
SHORT           InitYPos;               /* Y position */
SHORT           InitXSize;              /* X size */
SHORT           InitYSize;              /* Y size */
KEYDATA         KeyData;                /* Hotkey data, if none defined, usCh=0, bUsed=0, pMenuData=NULL */
ULONG           SwpFlag;                /* Activation on hotkey SWP flag: SWP_RESTORE or SWP_MAXIMIZE,
                                           SWP_NOMOVE */
LONG            AutostartDelay;         /* Autostart delay in seconds */
USHORT          PriorityClass;          /* Priority Class */
SHORT           PriorityDelta;          /* Priority Delta */
HPOINTER        hpointerIcon;           /* When drawing the Popup-Menu ownerdraw, the user wanted to
                                           include the session's icon, which we load during creation of
                                           the menuentry. Using the system drawing this handle is NULLHANDLE */
SCHEDULERDATA   SDScheduler;            /* Scheduler data */
MENUDATA        *Back;                  /* Pointer to previous menu */
MENUDATA        *Submenu;               /* Pointer to a submenu */
MENUDATA        *Next;                  /* Pointer to next menu */
};

struct  _COMMANDLINEPARAMS
{
UCHAR           ucPgmTitle[MAXNAMEL+1]; /* Session title */
                                        /* Parameters before user requested parameter */
UCHAR           ucPBefore[EF_SIZE255+1];
UCHAR           ucPUser[EF_SIZE255+1];  /* User requested parameter */
UCHAR           ucPAfter[EF_SIZE255+1]; /* Parameters after user requested parameter */
UCHAR           *pucCurrentDirectory;   /* Working directory */
};

struct  _COMMANDLINEENVIRONMENT
{
UCHAR           ucPgmTitle[MAXNAMEL+1]; /* Session title */
UCHAR           *pucEnvironmentSpaces;  /* Environment Spaces */
};

struct  _DESKTOPDIALOGPARAMS            /* Parameter passed during Desktop dialog creation */
{
USHORT  usLength;                       /* Length of structure */
USHORT  usTurnToPage;                   /* Notebook page to display initially */
USHORT  usMessageBox;                   /* Message box should be displayed informing what to enter on page */
};

struct  _NBPAGE                         /* Notebook pages setup data */
{
PFNWP           pDialogWndProc;         /* Window procedure of a notebook page */
HWND            hwndNBPage;             /* Window handle of a notebook page */
ULONG           ulIDPage;               /* ID of notebook page */
PSZ             pszStatusLine;          /* Status line text */
PSZ             pszTab;                 /* Tab text */
ULONG           ulIDDialogPage;         /* ID of a dialog resource */
ULONG           ulIDFocus;              /* ID of control to receive focus */
};

                                        /* WINDOWDATA ulStatus flags */
                                        /* Set if window visible on Overview window */
#define         VISIBLE             0x0001
                                        /* Set if window has a known frame class */
#define         FRAMECLASS          0x0002
                                        /* Set when Window List switch handle was inserted from
                                           another window in the same process (e.g. when a single
                                           Window List entry has >1 window open) */
#define         SHAREDHSWITCH       0x0004

struct  _WINDOWDATA                     /* All we need to know about a certain window */
{
UCHAR           ucPgmTitle[MAXNAMEL+4]; /* Session title on titlebar */
                                        /* Session title in Window List */
UCHAR           ucWindowTitle[MAXNAMEL+4];
                                        /* Session's class name */
UCHAR           ucClassName[CLASSNAMESIZE];
SWP             *pswpWindow;            /* Window's SWP structure within WINDOWS.pswpWindows */
KEYDATA         KeyData;                /* Hotkey data */
ULONG           SwpFlag;                /* Activation on hotkey SWP flag: SWP_RESTORE or SWP_MAXIMIZE,
                                           SWP_NOMOVE */
HSWITCH         hswitchWindow;          /* Switch handle of the window */
ULONG           ulStatus;               /* Status on Overview window */
HPOINTER        hptrIcon;               /* Window icon handle */
};

                                        /* Assume 128 windows as the maximum */
#define         MAXWINDOWS          128

struct  _WINDOWS                        /* All windows on PM are traced here. For all windows one entry
                                           is allocated in an array */
{
LONG            lDesktop;               /* Index (0 based) of Desktop in WINDOWDATA[] */
LONG            lWindowList;            /* Index (0 based) of Window List in WINDOWDATA[] */
LONG            lWindowLast;            /* Index (0 based) of last window of current WINDOWDATA[] */
LONG            lWindowPreviousLast;    /* Index (0 based) of last window of previous SWP array */
WINDOWDATA      *pwdWindows;            /* Pointer to an array of all Desktop child windows in system */
PSWP            pswpWindows;            /* Pointer to an array of all current Desktop child windows
                                           in system */
};

                                        /* Z-Order manipulation to keep certain PC/2 windows on top
                                           of Z-Order, HWND_TOP as top of Z-Order may get replaced by
                                           PC/2's Overview window, SessionBar, QuickSwitch and
                                           BubbleHelp - when they are available - in this order (that
                                           is the Overview window will be the absolute top of Z-Order) */
#define         ZORDER_QUICKSWITCH  0
#define         ZORDER_SESSIONBAR   1
#define         ZORDER_BUBBLEHELP   2
#define         ZORDER_OVERVIEW     3
#define         ZORDER_TOP          4
#define         NUM_ZORDER          (ZORDER_TOP+1)

                                        /* Popup Menu activation keys (in same order as in the Desktop
                                           Configuration dialog. Values are stored in ulPopupKeyFlag */
#define         POPUPLEFTRIGHTSHIFT 0x00000001
#define         POPUPLEFTCTRL       0x00000002
#define         POPUPRIGHTCTRL      0x00000004
#define         POPUPLEFTWIN95      0x00000008
#define         POPUPRIGHTWIN95     0x00000010
#define         POPUPWIN95MENU      0x00000020
#define         POPUPCAPSLOCK       0x00000040
#define         POPUPSCROLLLOCK     0x00000080

                                        /* Overview window draw mode. Values are stored in
                                           ulOverviewWindowDrawMode */
#define         OVERVIEW_ICONS      0x00000000
#define         OVERVIEW_GRADIENT   0x00000001
#define         OVERVIEW_MONOCHROME 0x00000002
#define         OVERVIEW_RANDOM     0x00000003

                                        /* Registration configuration. Values are stored in
                                           ulRegistrationFlag */
#define         REGISTER_ONLINE     0x00000001
#define         REGISTER_USER       0x00000002
#define         REGISTER_PROXY      0x00000004
#define         REGISTER_SOCKS      0x00000008
#define         REGISTER_SHOWSTATUS 0x00010000

                                        /* Dynamic Overview window configuration */
#define         OVERVIEW_UL         0x00000001
#define         OVERVIEW_UR         0x00000002
#define         OVERVIEW_LL         0x00000004
#define         OVERVIEW_LR         0x00000008

struct  _HOOKPARAMETERS
{
TIB             *pTib;                  /* TIB of PC/2 main thread (ID_MAINTHREAD) */
TIB             *pTibWorkingThread;     /* TIB of PC/2 working thread (ID_WORKINGTHREAD) */
TIB             *pTibIOThread;          /* TIB of PC/2 I/O thread (ID_IOTHREAD) */
TIB             *pTibNetThread;         /* TIB of PC/2 network thread (ID_NETTHREAD) */
PIB             *pPib;                  /* PIB of PC/2 */
KEYDATA         *pKeyData;              /* Hotkeys allowed for PC/2 */
HAB             habPc2;                 /* Anchor block handle of PC/2 */
HMQ             hmqPc2;                 /* Message queue handle of PC/2 */
HPS             hpsOverviewMemory;      /* Presentation space in a memory DC to draw
                                           Overview window into and blit it into the
                                           Overview window's client area afterwards, to
                                           avoid the flickering of drawing in the client
                                           area directly */
HPS             hpsSessionBarMemory;    /* Presentation space in a memory DC to draw
                                           SessionBar window into and blit it into the
                                           SessionBar window's client area afterwards, to
                                           avoid the flickering when dynamically unhiding the
                                           SessionBar */
HDC             hdcSwitch;              /* Device context of QuickSwitch window */
HPS             hpsSwitch;              /* Presentation space of QuickSwitch window */
HDC             hdcSession;             /* Device context of SessionBar window */
HPS             hpsSession;             /* Presentation space of SessionBar window */
HDC             hdcBubbleHelp;          /* Device context of BubbleHelp window */
HPS             hpsBubbleHelp;          /* Presentation space of BubbleHelp window */
HBITMAP         hbmOverviewMemory;      /* Bitmap handle of Overview in memory presentation space */
HBITMAP         hbmSessionBarMemory;    /* Bitmap handle of SessionBar in memory presentation space */
HWND            hwndFrame;              /* PC/2's Overview window frame */
HWND            hwndClient;             /* PC/2's Overview window client to send/post
                                           messages to */
HWND            hwndWorkingThread;      /* PC/2's working thread window handle to send/post messages to
                                           (caches threadDataWorkingThread.hwndThread) */
HWND            hwndIOThread;           /* PC/2's I/O thread window handle to send/post messages to
                                           (caches threadDataIOThread.hwndThread) */
HWND            hwndNetThread;          /* PC/2's Network thread window handle to send/post messages to
                                           (caches threadDataNetThread.hwndThread) */
HWND            hwndSpoolerFrame;       /* PC/2's Spooler Control window */
HWND            hwndSpoolerClient;      /* PC/2's Spooler client window handle to send/post
                                           messages to when PC/2 runs as a WPS replacement */
HWND            hwndSpoolerContainer;   /* Handle of the container which is the client of PC/2's
                                           spooler window */
HWND            hwndPopupMenu;          /* Handle of popup menu window */
                                        /* Handle of SessionBar's popup menu window */
HWND            hwndPopupMenuSessionBar;
                                        /* Handle of SessionBar's processor performance popup menu window */
HWND            hwndPopupMenuProcessor;
                                        /* Handle of SessionBar's processor performance popup menu window */
HWND            hwndPopupSubMenuProcessor;
                                        /* Handle of SessionBar's Statusarea popup menu window */
HWND            hwndPopupMenuStatus;
HWND            hwndMenu;               /* Handle of Overview window menu */
HWND            hwndClientMenu;         /* Handle of Overview window client menu */
HWND            hwndHelp;               /* Help window handle */
HWND            hwndWPSFrame;           /* WPS window frame handle */
HWND            hwndWPSClient;          /* WPS window client handle where mouse clicks on WPS
                                           can be found */
HWND            hwndDesktop;            /* PM window handle where mouse clicks on Desktop
                                           can be found */
HWND            hwndWindowList;         /* Window List window handle to avoid changing its
                                           Z-order in the WinSendMsgHook() */
HWND            hwndLockup;             /* PC/2 Lockup dialog window handle to avoid changing its
                                           Z-order in the WinSendMsgHook(), because when visible,
                                           this system-modal dialog should stay on top */
HWND            hwndActiveFrameWindow;  /* Top level frame window that was active, before PC/2
                                           detected an event that results in a change of
                                           the active window and input focus */
HWND            hwndSwitchFrame;        /* QuickSwitch window handle */
HWND            hwndSwitchClient;       /* QuickSwitch client window handle */
HWND            hwndSessionFrame;       /* SessionBar window handle */
HWND            hwndSessionClient;      /* SessionBar client window handle */
HWND            hwndStatusFrame;        /* Status frame window handle (used by PC2NET.DLL) */
HWND            hwndStatusClient;       /* Status client window handle */
HWND            hwndBubbleHelpFrame;    /* Bubble help window handle */
HWND            hwndBubbleHelpClient;   /* Bubble help client window handle */
HWND            hwndZOrder[NUM_ZORDER]; /* Z-Order control structure */
HWND            hwndConfigDialog;       /* Program Installation dialog window handle */
HWND            hwndDesktopDialog;      /* Desktop Configuration dialog window handle */
HWND            hwndSpyFrameWindow;     /* Desktop Configuration notebook page to post spy results to */
HWND            hwndPMHardError;        /* PM Hard Error window handle */
HWND            hwndSystemError;        /* System Error window handle */
MENUDATA        *pPopupMenu;            /* First entry of linked list of menus */
MENUDATA        *pMenuData;             /* This pointer points to the current level of
                                           Submenus and Menuitems within the configuration
                                           dialog procedure */
MENUDATA        *pMenuDataCutPaste;     /* This pointer point to NULL, to a Menuitem or a Submenu,
                                           and functions like a clipboard */
ENVIRONMENTLIST *pEnvironmentSpaces;    /* Pointer to a list of environment spaces which are
                                           saved into and read from PC2.ENV */
SPOOLEROBJECT   *pSpoolerObjectRoot;    /* This pointer points the the first SPOOLEROBJECT filled
                                           by enumerating the printer queue or NULL */
SPOOLEROBJECT   *pSpoolerObject;        /* This pointer points the the current SPOOLEROBJECTs list,
                                           which contains all selected container records or NULL */
SWBLOCK         *pWindowList;           /* Pointer to Window List contents returned by WinQuerySwitchList() */
WINDOWS         Windows;                /* Trace all windows here */
USHORT          MenuDataId;             /* Create linked list by starting with ID ID_POPUPMENU */
USHORT          PriorityClass;          /* PC/2's standard Priority Class */
SHORT           PriorityDelta;          /* PC/2's stantard Priority Delta */
                                        /* Path of PC2.INI */
UCHAR           ucFilenameINI[CCHMAXPATH];
                                        /* Path of PC2.CFG (or replacement) */
UCHAR           ucFilenameProfile[CCHMAXPATH];
                                        /* Path of PC2.ENV (or replacement) */
UCHAR           ucFilenameEnvironment[CCHMAXPATH];
                                        /* Path of PC2.TRP */
UCHAR           ucFilenameTRP[CCHMAXPATH];
UCHAR           ucPathDLL[CCHMAXPATH];  /* Path to PC/2 DLLs */
                                        /* Name of Desktop, f.e. OS/2 2.0 Desktop */
UCHAR           ucDesktopName[MAXNAMEL+1];
                                        /* Name of Task List, f.e. Window List */
UCHAR           ucWindowListName[MAXNAMEL+1];
                                        /* Name of PM background bitmap */
UCHAR           ucBackgroundBitmap[MAXNAMEL+1];
                                        /* Font used for Popup-Menu (set by dropping font onto
                                           smart icons) */
UCHAR           ucPopupMenuFont[MAXNAMEL+1];
                                        /* Font for Overview window */
UCHAR           ucPC2WindowFont[MAXNAMEL+1];
                                        /* Font for QuickSwitch window */
UCHAR           ucSwitchWindowFont[MAXNAMEL+1];
                                        /* Font for SessionBar window */
UCHAR           ucSessionWindowFont[MAXNAMEL+1];
                                        /* Font for spooler window */
UCHAR           ucPC2SpoolerFont[MAXNAMEL+1];
                                        /* Access password */
UCHAR           ucAccessPassword[PASSWORDSIZE+1];
                                        /* Lockup password */
UCHAR           ucLockupPassword[PASSWORDSIZE+1];
UCHAR           ucBootDrive;            /* Boot drive */
UCHAR           ucCDDrive1;             /* CD drive (1st) */
                                        /* PC/2's environment in the form of:
                                           EnvVar1=Value1\r\nEnvVar2=Value2\r\n...\0 */
UCHAR           *pucPC2Environment;
                                        /* Pointer to shared memory which PC/2 gives to all other processes
                                           that have PM windows, as WinQueryWindowText() can (and does
                                           from my experience) trap for interprocess calls when not
                                           passing memory shared by both processes */
UCHAR           *pucWindowText;
                                        /* Name of MTE's where we can't add Titlebar Smarticons */
UCHAR           ucTitlebarException[CCHMAXPATH];
                                        /* Name of MTE's where we can't restrict window size */
UCHAR           ucHWVirtualDesktopException[CCHMAXPATH];
                                        /* Name of MTE's we hide from SessionBar */
UCHAR           ucSessionBarException[CCHMAXPATH];
                                        /* SessionBar customization date/time format (strtime()
                                           formatting) */
UCHAR           ucSessionBarDateTime[MAXNAMEL+1];
                                        /* Titlebar, Window List or Class substring for Sticky Windows */
UCHAR           ucStickyWindows[CCHMAXPATH];
                                        /* User's first name */
UCHAR           ucUserFirstName[MAXNAMEL+1];
                                        /* Users's last name */
UCHAR           ucUserLastName[MAXNAMEL+1];
                                        /* User's E-Mail address */
UCHAR           ucUserEmailAddress[MAXNAMEL+1];
                                        /* User's HTTP userid (to authenticate with proxy or server) */
UCHAR           ucUserHttpUserid[MAXNAMEL+1];
                                        /* User's HTTP password */
UCHAR           ucUserHttpPassword[PASSWORDSIZE+1];
                                        /* Proxy URL address */
UCHAR           ucProxyUrl[EF_SIZE255+1];
                                        /* URL to retrieve PC/2 update information from */
UCHAR           ucUpdateUrl[EF_SIZE255+1];
                                        /* URL to post PC/2 User Registration to */
UCHAR           ucRegistrationUrl[EF_SIZE255+1];
                                        /* Local TCP/IP address in dotted-decimal ASCII
                                           (filled in by PC2NET.DLL) */
UCHAR           ucLocalAddress[EF_SIZE255+1];
                                        /* Local TCP/IP hostname in ASCII (filled in by PC2NET.DLL) */
UCHAR           ucLocalHostname[EF_SIZE255+1];
                                        /* "Time" string of last exception logged into PC2.TRP */
UCHAR           ucLastPC2Trap[EF_SIZE255+1];
ULONG           ulBackgroundBitmapFlag; /* Upper USHORT contains PM background bitmap style flag,
                                           lower USHORT contains tile count */
ULONG           ProcessId;              /* PC/2's process ID */
ULONG           ulScrollPercentage;     /* Percentage of physical Desktop used to scroll
                                           through virtual Desktop (25, 50, 75, 100 %) */
ULONG           ulHorizontalDesktops;   /* Number of horizontal Desktops actually using */
ULONG           ulVerticalDesktops;     /* Number of vertical Desktops actually using */
                                        /* Number of horizontal Desktops configured */
ULONG           ulHorizontalDesktopsSave;
ULONG           ulVerticalDesktopsSave; /* Number of vertical Desktops configured */
ULONG           ulStatusFlag1;          /* Bitmapped status flag 1 for persistent options */
ULONG           ulStatusFlag2;          /* Bitmapped status flag 2 for persistent options */
ULONG           ulStatusFlag3;          /* Bitmapped status flag 3 for persistent options */
                                        /* OVERVIEW_DRAW_* move, defines how windows in the Overview
                                           window are drawn (as icons, gradient, monochrome, ...) */
ULONG           ulOverviewWindowDrawMode;
ULONG           ulRuntimeFlag;          /* Bitmapped status flag for runtime-only options */
ULONG           ulClickFlag;            /* WM_BUTTON1CLICK for a single click, and
                                           WM_BUTTON1DBLCLK for a double click, or
                                           WM_BUTTON3CLICK for a single click and
                                           WM_BUTTON3DBLCLK for a double click */
ULONG           ulTitlebarFlag;         /* Bitmapped status flag for Titlebar Smarticons enabled */
ULONG           ulPopupKeyFlag;         /* Bitmapped status flag for Popup Menu keyboard activation */
ULONG           ulRegistrationFlag;     /* Bitmapped registration configuration flag */
ULONG           ulLockupTimer;          /* Lockup timer (in minutes) */
ULONG           ulPowerDownTimer;       /* PowerDown timer (in minutes) */
ULONG           ulCurrentTimer;         /* Current time (in seconds) without any user input on PM. Will
                                           be compared with ulLockupTimer and ulPowerDownTimer to
                                           eventually post WM_PC2LOCKUP to activate lockup and
                                           WMCOMMAND(ID_POWERDOWN) to activate PowerDown */
ULONG           ulLogicalDriveMap;      /* Bitmapped flag for all drives available locally */
ULONG           ulOverviewFCF;          /* FCF_OVERVIEWWINDOW or user modification */
ULONG           ulSpoolerFCF;           /* FCF_SPOOLERWINDOW or user modification (currently not used) */
ULONG           ulDebug;                /* Debug status flag */
ULONG           ulIconScale;            /* Icon scale (0 for full size, 1 for 1/2 size, 2 for 1/4 size, ...) */
ULONG           ulSessionBarScale;      /* SessionBar scale (0 for full size, 1 for 1/2 size, 2 for 1/4 size, ...) */
ULONG           ulSessionBarHotSpot;    /* SessionBar hotspot height (for Dynamic SessionBar) */
ULONG           ulSessionBarFlag;       /* SessionBar position flag (SB_*) */
ULONG           ulSessionBarStatusarea; /* SessionBar Statusarea flag (SB_*) */
ULONG           ulPerformanceFlag;      /* SessionBar CPU Performance flag (PERFORMANCE_*) */
ULONG           ulVersionMajor;         /* OS/2 major version */
ULONG           ulVersionMinor;         /* OS/2 minor version */
ULONG           ulHWSizeX;              /* Size of screen for hardware supported Virtual Desktops */
ULONG           ulHWSizeY;              /* Size of screen for hardware supported Virtual Desktops */
ULONG           ulOverviewRGBColorBgnd; /* Overview window background color */
ULONG           ulOverviewRGBColorWnd;  /* Overview window rectangle background color when in monochrome mode */
ULONG           ulOverviewRGBColorFgnd; /* Overview window rectangle foreground color when in monochrome mode */
ULONG           ulOverviewGradientStart;/* Overview windows gradient start color */
ULONG           ulOverviewGradientEnd;  /* Overview windows gradient final color */
                                        /* SessionBar window foreground color */
ULONG           ulSessionBarRGBColorFgnd;
                                        /* SessionBar window background color */
ULONG           ulSessionBarRGBColorBgnd;
                                        /* SessionBar CPU Performance interrupt color */
ULONG           ulSessionBarRGBColorIntr;
                                        /* SessionBar CPU Performance busy color */
ULONG           ulSessionBarRGBColorBusy;
                                        /* SessionBar CPU Performance grid color */
ULONG           ulSessionBarRGBColorGrid;
                                        /* SessionBar CPU Performance background color */
ULONG           ulSessionBarRGBColorCPU;
ULONG           ulAPMStatus;            /* Battery capacity available if APM supports battery power
                                           management (APMENABLED flag set), low word is capacity,
                                           high word is line or battery power */
ULONG           ulOVSmartIconsFlag;     /* Bitmapped status flag for enabled Overview window SmartIcons
                                           0 ... ID_OVICONEXIT (Exit icon enabled)
                                           1 ... ID_OVICONMOVE (Move icon enabled)
                                             ... */
ULONG           ulDynamicOverview;      /* Bitmapped OVEVIEW_* status flag for the Dynamic
                                           Overview window */
ULONG           ulOverviewDelay;        /* Dynamic Overview activation delay (100 ms steps) */
ULONG           ulSlidingFocusDelay;    /* Sliding Focus activation delay (in 100 ms) */
ULONG           ulDynamicMenuDelay;     /* Dynamic Menuselection activation delay (100 ms steps) */
ULONG           ulSessionBarDelay;      /* Dynamic SessionBar activation delay (100 ms steps) */
ULONG           ulVirtualDesktopDelay;  /* Virtual Desktop switching delay (100 ms steps) */
ULONG           ulNumProcessors;        /* Number of processors supported by OS/2 (for non SMP -> 1) */
ULONG           ulProcessorStatus;      /* Each bit represents a Processor online status */
ULONG           ulConfigurationChanges; /* Number of configuration changes to remember user doing a backup */
                                        /* Threads TIDs, alive counters and last messages processed */
ULONG           ulThreadIDITrd;
ULONG           ulThreadIDWTrd;
ULONG           ulThreadIDNTrd;
ULONG           ulAliveTimerITrd;
ULONG           ulAliveTimerWTrd;
ULONG           ulAliveTimerNTrd;
ULONG           ulAliveMsgITrd;
ULONG           ulAliveMsgWTrd;
ULONG           ulAliveMsgNTrd;
ULONG           ulReserveDriveLetter;   /* Don't change to root directory for specified drives */
LONG            lSplitbarPosition;      /* Splitbar position in pixel of spooler container splitbar */
LONG            SlidingXFactor;         /* Sliding offset in pixel for horizontal movements */
LONG            SlidingYFactor;         /* Sliding offset in pixel for vertical movements */
LONG            lIconSize;              /* Icon size (ususally 32*32 for VGA, 40*40 for SVGA) */
float           fScaleX, fScaleY;       /* Reduce factor to reduce horizontal size of virtual
                                           Desktop to horizonal client window size */
SWP             swpScreen;              /* The screen dimensions */
SWP             swpPC2;                 /* Overview window size and position */
SWP             swpPC2Client;           /* Overview window client's size and position */
SWP             swpPC2Spooler;          /* Spooler window size and position */
SWP             swpPC2Session;          /* SessionBar window size and position */
SWP             swpPC2Status;           /* Status window size and position */
POINTL          ptlOrigin;              /* Coordinates (0|0) within client area */
POINTL          LLHotBorder;            /* Lower left coordinates that force sliding in x & y direction */
POINTL          URHotBorder;            /* Upper right coordinates that force sliding in -x & -y direction */
POINTL          DesktopSize;            /* Size of the physical Desktop screen */
POINTL          VirtualDesktopPos;      /* Position of the physical Desktop within the virtual */
POINTL          VirtualDesktopMin;      /* Lower left position of whole virtual Desktop */
POINTL          VirtualDesktopMax;      /* Upper right position of whole virtual Desktop */
HMODULE         hDLLPc2Hook;            /* PC2HOOK.DLL handle */
HMODULE         hDLLPc2Spooler;         /* PC2SPOOL.DLL handle */
HMODULE         hDLLPc2Net;             /* PC2NET.DLL handle */
HMODULE         hDLLPc2Resource;        /* PC2BRES.DLL (BGA) or PC2VRES.DLL (VGA) handle */
FILE            *Pc2Profile;            /* Open the profile, where the user entered menu
                                           data is stored, with this handle */
FILE            *Pc2EnvironmentProfile; /* Open the profile, where the defined environment spaces
                                           are stored, with this handle */
UCHAR           *pucBeginLibpath;       /* BEGINLIBPATH inherited by PC/2 from shell */
UCHAR           *pucEndLibpath;         /* ENDLIBPATH inherited by PC/2 from shell */
                                        /* DOSCALL1.873 DOSSETEXTLIBPATH API introduced
                                           with OS/2 WARP 3+ */
DOSSETEXTLIBPATH    *DosSetExtLIBPATH;
                                        /* DOSCALL1.874 DOSQUERYTEXTLIBPATH API introduced
                                           with OS/2 WARP 3+ */
DOSQUERYEXTLIBPATH  *DosQueryExtLIBPATH;
                                        /* DOSCALL1.447 DOSGETPROCESSORSTATUS API introduced
                                           with OS/2 WARP Srv. SMP+ */
GETPROCESSORSTATUS  *DosGetProcessorStatus;
                                        /* DOSCALL1.448 DOSSETPROCESSORSTATUS API introduced
                                           with OS/2 WARP Srv. SMP+ */
SETPROCESSORSTATUS  *DosSetProcessorStatus;
                                        /* DOSCALL1.976 DOSGETPERFSYSCALL API introduced
                                           with OS/2 WARP 4+ */
DOSPERFSYSCALL      *DosPerfSysCall;
};

struct  _SPOOLERRECORD                  /* Within the spooler window container spooler objects
                                           are stored as SPOOLERRECORD objects */
{
MINIRECORDCORE  MiniRec;                /* A (MINI)RECORDCORE must be the first element of any
                                           type of container window structure */
SPOOLEROBJECT   *pSpoolerObject;        /* Pointer to the linked list of SPOOLEROBJECTs that
                                           controls the SPOOLERRECORD list used by the container internally */
PSZ             pszQueue;               /* Pointer to the queue name column of a spooler container record */
PSZ             pszQueueStatus;         /* Pointer to the queue status column */
PSZ             pszQueueComment;        /* Pointer to the queue comment column */
PSZ             pszQueueDriver;         /* Pointer to the queue status column */
PSZ             pszJob;                 /* Pointer to the job name column */
PSZ             pszJobNumber;           /* Pointer to the job identification number in ASCII */
PSZ             pszJobStatus;           /* Pointer to the job status column */
PSZ             pszJobComment;          /* Pointer to the job document comment */
ULONG           ulJob;                  /* Job identification number */
};

struct  _SPOOLEROBJECT                  /* PC/2 internal structure that is a copy of the list used
                                           within the container to control the container objects to
                                           avoid message exchange with the container, when updating
                                           the spooler data */
{
SPOOLEROBJECT   *pSpoolerObjectNext;    /* Pointer to the next SPOOLEROBJECT (next row in container) */
SPOOLEROBJECT   *pSpoolerObjectBack;    /* Pointer to the previous SPOOLEROBJECT */
BOOL            bInQueue;               /* TRUE if current job found during queue enumeration */
SPOOLERRECORD   *pSpoolerRecord;        /* Pointer to a SPOOLERRECORD that is used within the container
                                           to represent a row in the spooler container details view */
};

struct  _WPSOBJECTLIST                  /* LIST structure of WPS objects extracted by WPS2PC2.DLL
                                           into shared memory, which is used in PC2.EXE to create
                                           menuitems from */
{
ULONG           ulType;                 /* WPSOBJECT* type value of list element */
WPSOBJECTLIST   *pNext;                 /* Pointer to next LIST element */
WPSOBJECTLIST   *pFolder;               /* Pointer to LIST of elements within the current folder */
VOID            *pData;                 /* Pointer to data item of LIST, e. g. variable length
                                           PROGDETAILS or UCHAR[] structures */
};

struct  _ENVIRONMENTLIST                /* LIST structure of environment spaces. A environment space
                                           is a character array in the form:
                                           EnvVar1=Value1\r\nEnvVar2=Value2\r\n...\0
                                           E.g. you may create a environment space for the IBM
                                           Developers Toolkit and one for IBM's VisualAge C++ and
                                           combine them to create a development environment */
{
PSZ             pEnvironmentName;       /* Environment space's name */
PSZ             pEnvironment;           /* Environment space */
ENVIRONMENTLIST *pBack;                 /* Pointer to previous LIST element */
ENVIRONMENTLIST *pNext;                 /* Pointer to next LIST element */
};

struct  _COLORWINDOW                    /* Foreground and background colors to draw on Overview window */
{
ULONG           ulBackgroundRGB;        /* RGB background color of window drawn on Overview window */
ULONG           ulForegroundRGB;        /* RGB foreground color of window text */
};

struct  _FRAMEDATA                      /* This structure overlays a 4096 byte page allocated in the context
                                           of other processes, while processing a subclassed frame window
                                           procedure. It is used instead of saving automatic data on the
                                           stack to avoid running out of stack */
{
UCHAR           ucBuffer[CCHMAXPATH];   /* Error buffer for loading PC2?Res.dll */
ULONG           ulControl;              /* Titlebar Smarticon */
ULONG           ulFrameChildFlags;      /* Window style QWL_STYLE */
ULONG           ulFrameFlags;
ULONG           ulIndex;                /* Indices to manage Titlebar Smarticons */
ULONG           ulLoadIndex;
ULONG           ulRollupHeight;         /* Difference between Rollup and Rolldown */
ULONG           ulSWPFlag;              /* SWP flag to correct size, position and/or visibility */
ULONG           ulShift;                /* Mask for bitmapped status flags */
ULONG           ulShiftBitcount;
ULONG           ulShiftBitmask;
ULONG           ulStyle;                /* Window style QWL_STYLE */
ULONG           ulStyleActual;
ULONG           ulToggleMask;
USHORT          usFrameCtlCount;        /* Number of frame controls */
USHORT          usFrameID;              /* Frame ID QWS_ID */
USHORT          usFrameTitlebar;
LONG            lSmarticonSize;         /* Titlebar Smarticon size (titlebar height) */
HWND            hwndFrameChild;         /* Frame windows and related windows */
HWND            hwndFrameCurrent;
HWND            hwndFrameHelp;
HWND            hwndFrameParent;
HENUM           henumFrame;             /* Enumeration handle to hide controls during Rollup */
HMODULE         hmoduleTemp;            /* Handle of PC2?Res.dll */
HPS             hpsTemp;                /* Presentation space to load Titlebar Smarticons */
RECTL           rectlBlit;              /* Rectangles to blit and border Titlebar Smarticons */
RECTL           rectlBorder;
RECTL           rectlFrameChild;
POINTL          ptlMouse;               /* Mouse position within Titlebar Smarticons */
POINTL          ptlMouseRotation;
SWP             *pswpControl;           /* Frame windows position */
SWP             *pswpFrame;
SWP             *pswpFrameControls;
SWP             *pswpTitlebar;
SWP             swpFrame;
SWP             swpFrameChild;
SWP             swpTitlebar;
PPIB            ppibFrame;              /* Frame's process and thread information */
PTIB            ptibFrame;
MENUITEM        menuitemFrame;          /* Structure to query/set menuitems */
USERBUTTON      *puserbuttonControl;    /* Structure to ownerdraw Titlebar Smarticons */
BOOL            bVisibility;            /* Visibility flag */
};

                                        /* Window flag */
#define         WINDOWSUBCLASSED    0x00000001

struct  _FRAMELIST                      /* List of frame windows detected by the SendMsgHook() where we
                                           trace all frame windows (by looking for WM_CREATE and WM_DESTROY) */
{
HWND            hwndFrame;              /* Frame window (or NULLHANDLE) */
PFNWP           pfnwpFrame;             /* Frame window procedure (or NULL) before subclassing the frame */
FRAMEDATA       *pfdFrame;              /* Frame window automatic data */
MRESULT         mresultFrame;           /* Result of the subclassed frame window procedure */
ULONG           ulStatusFlag;           /* Status FL_* flags (ON = FL_* flag set) */
ULONG           ulControlsFlag;         /* Status FL_* flags (Control added to titlebar = FL_* flag set) */
ULONG           ulWindowFlag;           /* Window control flags */
LONG            lRollupDelta;           /* Vertical size of the frame window was reduced by rolling it up */
HMODULE         hmodulePc2Hook;         /* PC2HOOK.DLL handle */
                                        /* Array holding the additional titlebar controls windows */
HWND            hwndControls[NUM_TITLEBARCONTROLS];
                                        /* Array holding the bitmap handles for normal up state */
HBITMAP         hbmControls0[NUM_TITLEBARCONTROLS];
                                        /* Array holding the bitmap handles for toggeled up state */
HBITMAP         hbmControls1[NUM_TITLEBARCONTROLS];
                                        /* Array holding the bitmap handles for normal down
                                           state (for WARP 4) */
HBITMAP         hbmControls2[NUM_TITLEBARCONTROLS];
                                        /* Array holding the bitmap handles for toggeled
                                           down state (for WARP 4) */
HBITMAP         hbmControls3[NUM_TITLEBARCONTROLS];
};

                                        /* Status flag set if current session was painted at least once
                                           (required as working thread posts repaint requests asynchronously
                                           to WM_PAINT caused by invalidated client) */
#define         SESSIONPAINTED      0x00000001
                                        /* Status flag set if current session is the active one */
#define         SESSIONACTIVE       0x00000002
                                        /* Status flag set if current session should be hidden */
#define         SESSIONHIDE         0x00000004

struct  _SWITCHLIST                     /* List of switch handles enumerated from the Window List by the
                                           working thread and posted to the SessionBar, which uses this
                                           linked list to keep track of them */
{
HSWITCH         hswitchSession;         /* Switch handle of a window's representation in the Window List */
HWND            hwndIcon;               /* Cache window's icon */
ULONG           ulStatusFlag;           /* Status flag */
ULONG           ulSession;              /* Index (1-based) of the window's representation for last drawing
                                           of SessionBar */
SWITCHLIST      *pPrevious;             /* Pointer to the previous window's SWITCHLIST or NULL */
SWITCHLIST      *pNext;                 /* Pointer to the next window's SWITCHLIST or NULL */
};

struct  _POPUPMENU4KBD                  /* Structure to control the usage and detection of keys that
                                           may be used as Hotkeys to display PC/2's Popup Menu */
{
                                        /* Popup status flag in HookParameters.ulPopupKeyFlag */
ULONG           ulPopupKeyFlag;
                                        /* Flag that tells us, if we should remove the key event */
ULONG           ulRemoveKeyFlag;
                                        /* Current WM_CHAR message mp1 after applying MASK_WIN95_KEY_MP1).
                                           The highest byte is the scancode, and this should be unique
                                           between all keys */
ULONG           ulWMCHARmp1;
                                        /* Current WM_CHAR message mp2 after applying MASK_WIN95_KEY_MP2) for
                                           !KC_KEYUP events, currently we have to be cautious to use this
                                           parameter as it differs between PM and command processor windows) */
ULONG           ulWMCHARmp2Down;
                                        /* Current WM_CHAR message mp2 after applying MASK_WIN95_KEY_MP2) for
                                           KC_KEYUP events, currently we have to be cautious to use this
                                           parameter as it differs between PM and command processor windows) */
ULONG           ulWMCHARmp2Up;
};

struct  _SPYFRAME                       /* To allow the user to find out the data of a frame window PC/2
                                           uses to identify that window (e.g. to keep it on active Desktop)
                                           the following data is queried in the input hook and displayed
                                           in the Desktop Dialog when requested */
{
                                        /* Frame window the data belongs to */
HWND            hwndFrame;
                                        /* Frame window's window text */
UCHAR           ucWindowText[MAXNAMEL+4];
                                        /* Frame window's class name */
UCHAR           ucWindowClass[MAXNAMEL+4];
                                        /* Frame window's Window List entry */
UCHAR           ucWindowList[MAXNAMEL+4];
                                        /* Frame window's executable */
UCHAR           ucExecutable[CCHMAXPATH+1];
                                        /* Frame window's process and thread id */
PID             pidWindow;
TID             tidWindow;
};

                                        /* CPU Performance percentages */
struct  _CPUPERFORMANCE
{
UCHAR           ucIdle;
UCHAR           ucBusy;
UCHAR           ucIntr;
ULONG           ucReserved;
};

struct  _CPUTICKS
{
double          dTotal;
double          dIdle;
double          dBusy;
double          dIntr;
};

                                        /* Status flag for ShutDown OS/2/PowerDown OS/2 dialog */
#define SHUTDOWN_SHUTDOWN           0x00000001
#define SHUTDOWN_POWERDOWN          0x00000002

struct  _SHUTDOWNPARAMETERS             /* Parameters passed to ShutDown OS/2/PowerDown OS/2 dialog */
{
ULONG           ulStatusFlag;           /* Status flag (SHUTDOWN_*) */
HFILE           hfileDD;                /* Handle of DOS$ or APM$ */
};

                                        /* This structure defines the Status tag of the
                                           AUTOUPDATE structure */
struct  _TAGSTATUS
{
                                        /* Date the last successful connection to Internet */
ULONG           ulDay;
ULONG           ulMonth;
ULONG           ulYear;
ULONG           ulHour;
ULONG           ulMinute;
};

                                        /* This structure defines the Register tag of the
                                           AUTOUPDATE structure (when Registration has been posted) */
struct  _TAGREGISTER
{
                                        /* Date of Registration info sent */
ULONG           ulDay;
ULONG           ulMonth;
ULONG           ulYear;
                                        /* Version of PC/2 used to register (as a string and in
                                           decimal by factor 100, e.g. 2.09a would result in 209 */
UCHAR           ucVersion[8];
ULONG           ulVersion;
};

                                        /* This structure defines the News tag of the
                                           AUTOUPDATE structure */
struct  _TAGNEWS
{
                                        /* Date the news apply to */
ULONG           ulDay;
ULONG           ulMonth;
ULONG           ulYear;
                                        /* News text */
UCHAR           *pucNews;
                                        /* The TAGNEWS structures are connected through
                                           a linked list */
TAGNEWS         *pPrev;
TAGNEWS         *pNext;
};

                                        /* This structure defines the Update tag of the
                                           AUTOUPDATE structure, which is a sequence of
                                           optionally repetitive TAGUPDATE structures */
#define TAGUPDATE_REPLACEMENT           0x00000001
#define TAGUPDATE_ADDITION              0x00000002
#define TAGUPDATE_DOWNLOADED            0x80000000

struct  _TAGUPDATE
{
                                        /* TAGUPDATE_* flags */
ULONG           ulStatus;
                                        /* Descriptive information for updates */
UCHAR           ucFilename[CCHMAXPATH];
UCHAR           ucDescription[CCHMAXPATH<<1];
UCHAR           ucVersion[8];
ULONG           ulSize;
ULONG           ulCrc;
UCHAR           ucUrl[CCHMAXPATH<<1];
                                        /* The TAGUPDATE structures are connected through
                                           a linked list */
TAGUPDATE       *pPrev;
TAGUPDATE       *pNext;
};

                                        /* The AUTOUPDATE structure is the key element for
                                           the Internet base update facility */
#define AUTOUPDATE_STATUS               0x00000001
#define AUTOUPDATE_REGISTER             0x00000002
#define AUTOUPDATE_NEWS                 0x00000004
#define AUTOUPDATE_UPDATE               0x00000008

struct  _AUTOUPDATE
{
                                        /* The current keyword whose data are described in
                                           current AUTOUPDATE structure, e.g. News or Update */
                                        /* AUTOUPDATE_* flag */
ULONG           ulStatus;
union
    {
    TAGSTATUS   *tagStatus;
    TAGREGISTER *tagRegister;
    TAGNEWS     *tagNews;
    TAGUPDATE   *tagUpdate;
    } Tags;
                                        /* The AUTOUPDATE structures are connected though a
                                           linked list */
AUTOUPDATE      *pPrev;
AUTOUPDATE      *pNext;
};

                                        /* The URL structure defines the data required to
                                           make a TCP/IP connection to access a Url */
struct _URL
{
                                        /* Server service, e.g. 80 for HTTP requests */
ULONG           ulService;
                                        /* Server host TCP/IP address, either symbolic or
                                           dotted decimal */
UCHAR           ucHost[CCHMAXPATH];
                                        /* Server port, e.g. 8080 for HTTP requests */
ULONG           ulPort;
                                        /* Server path, e.g. / for root directory */
UCHAR           ucPath[CCHMAXPATH];
                                        /* Socket which was opened for connection */
int             iSocket;
};

                                        /* Status information */
#define EXCEPTIONRECORD_NONE            0x00000000
#define EXCEPTIONRECORD_JMPBUF          0x00000001

                                        /* Exception handler record */
struct _EXCEPTIONRECORD
{
                                        /* Exception registration record */
EXCEPTIONREGISTRATIONRECORD excpregrec;
                                        /* Signature */
ULONG           ulSignature;
                                        /* Status information */
ULONG           ulStatus;
                                        /* Return buffer to jump back */
jmp_buf         jmpbufReturn;

};

                                        /* Interface between the Network thread and PC2NET.DLL and
                                           C-Runtime functions exported by Network thread to
                                           PC2NET.DLL, which is only linked with the subsystem DLLs */
struct  _CRUNTIME
{
AUTOUPDATE          *pautoupdateRemote;
AUTOUPDATE          *pautoupdateLocal;
int                 iSocketLANPPP;
int                 iSocketHTTP;
ULONG               ulInterfacesLAN;
ULONG               ulInterfacesPPP;
HWND                hwndStatusText;
PFN_MALLOC          *pfnMalloc;
PFN_FREE            *pfnFree;
PFN_STRICMP         *pfnStricmp;
PFN_STRNICMP        *pfnStrnicmp;
PFN_STRUPR          *pfnStrupr;
PFN_STRDUP          *pfnStrdup;
PFN_FPRINTF         *pfnFprintf;
PFN_FOPEN           *pfnFopen;
PFN_FCLOSE          *pfnFclose;
PFN_MKTIME          *pfnMktime;
PFN_STRIPLEADING    *pfnStripLeading;
PFN_STRIPTRAILING   *pfnStripTrailing;
USHORT              *pus_Ctype;
struct tm           tm_Current;
};

                                        /* WM_NETSCHEDULE message function parameter mp1 */
                                        /* Query LAN and PPP interfaces */
#define NET_QUERYINTERFACE      1
                                        /* Load the local AUTOUPDATE mirror from PC2.NET */
#define NET_LOADUPDATE          2
                                        /* Load the remote AUTOUPDATE from the Automatic Update Url */
#define NET_GETUPDATE           3
                                        /* Save the remote AUTOUPDATE structure into PC2.NET */
#define NET_SAVEUPDATE          4
                                        /* Free an AUTOUPDATE structure */
#define NET_FREEUPDATE          5
                                        /* Check for news tag updates */
#define NET_CHECKNEWS           6
                                        /* Check for update tag updates */
#define NET_CHECKUPDATE         7
                                        /* Post the user registration data */
#define NET_POSTEMAIL           8

#pragma pack()

                                        /* As we adjust our drawing to the default icon
                                           size of the current resolution, we may have to
                                           find out if we're running in VGA or BGA */
#define ICONSIZE_VGA            32
#define ICONSIZE_BGA            40

#define PC2_CLASSNAME           "PC2 - Main"
#define PC2_CLASSNAME_THREAD    "PC2 - Thread"
#define PC2_CLASSNAME_SPOOLER   "PC2 - Spooler"
#define PC2_CLASSNAME_WPS2PC2   "PC2 - WPS 2 PC/2"
#define PC2_CLASSNAME_SWITCH    "PC2 - QuickSwitch"
#define PC2_CLASSNAME_SESSION   "PC2 - SessionBar"
#define PC2_CLASSNAME_STATUS    "PC2 - Status"
#define PC2_CLASSNAME_BUBBLE    "PC2 - Bubble Help"

#define PC2_OVERVIEW_WINDOW     "PC/2 Overview"
#define PC2_SPOOLER_WINDOW      "PC/2 Spooler"
#define PC2_WPS2PC2_WINDOW      "WPS 2 PC/2"

#define PC2_WPS2PC2_HELP        "WPS2PC2.HLP"

#define CTRL_CONFIGMENU         "Config ~Menu"
#define CTRL_CONFIGDESKTOP      "Config ~Desktop"
#define CTRL_ABOUT              "~About PC/2"
#define CTRL_SHUTDOWN           "~ShutDown OS/2"
#define CTRL_POWERDOWN          "~PowerDown OS/2"
#define CTRL_HELP               "~Help\tF1"
#define CTRL_EXIT               "E~xit\tF3"
#define CTRL_SPOOLER            "PC/2 Spoo~ler"
#define CTRL_BREAKSEPARATOR     "-- Vert. Break --"
#define CTRL_SEPARATOR          "--- Separator ---"
#define CTRL_LOCKUP             "~Lockup"

                                        /* Load the current level into configuration listbox */
#define WM_LOADPOPUPMENU        WM_USER+1
                                        /* Load the Popup-Menu from Profile */
#define WM_LOADPOPUPMENU        WM_USER+1
                                        /* Save Popup-Menu to Profile */
#define WM_SAVEPOPUPMENU        WM_USER+2
                                        /* Initialize the input queue hook */
#define WM_LOADHOOK             WM_USER+3
                                        /* Posted by thread to itself to initialize thread specific data */
#define WM_INITTHREAD           WM_USER+4
                                        /* Inform working thread to terminate itself */
#define WM_QUITTHREAD           WM_USER+5
                                        /* Initialize PC/2 main window */
#define WM_PC2STARTUP           WM_USER+6
#define WM_PC2STARTUPSCREEN     WM_USER+7
#define WM_PC2STARTUPMEMORYDC   WM_USER+8
#define WM_PC2STARTUPTHREAD     WM_USER+9
                                        /* Display Popup-Menu on Desktop */
#define WM_POPUPMENU            WM_USER+10
                                        /* Insert an item to the linked list */
#define MM_INSERTITEMMENUITEM   WM_USER+11
                                        /* Change the first item of a Submenu to an item */
#define MM_INSERTITEMSUBMENU    WM_USER+12
                                        /* Move an item of a Submenu to another position */
#define MM_MOVEMENUITEM         WM_USER+13
                                        /* ShutDown message to ShutDown dialog */
#define WM_SHUTDOWN             WM_USER+14
                                        /* Query and set desktop window handle */
#define WM_SETDESKTOPHANDLE     WM_USER+15
                                        /* Sent by PC2HOOK.DLL if windows need to be moved */
#define WM_DESKTOPMOVE          WM_USER+16
                                        /* Sent by PC2Hook.DLL when a WM_CHAR message was detected
                                           that is used by the Hotkey function */
#define WM_HOTKEY               WM_USER+17
                                        /* Sent by PC2Hook.DLL when mouse clicks are detected on
                                           PM that would display the window list on the WPS */
#define WM_WINDOWLIST           WM_USER+18
                                        /* Sent by PC2Hook.DLL when the pointer is positioned on
                                           a surrounding row and/or column and the Desktop should
                                           be moved */
#define WM_MOVEREQUEST          WM_USER+19
                                        /* Sent to working thread window to resize WPS */
#define WM_EXPANDWPS            WM_USER+20
                                        /* Sent to main window and passed to working thread
                                           from PC2HOOK.DLL when a frame window's z order should
                                           be adjusted because of a button 2 click on titlebar */
#define WM_ZORDER               WM_USER+21
                                        /* Sent to working thread to start applications that have
                                           the autostart flag */
#define WM_AUTOSTART            WM_USER+22
                                        /* Sent to working thread to change the PM background bitmap */
#define WM_BACKGROUNDBITMAP     WM_USER+23
                                        /* Sent by working thread to indicate he finished his work */
#define WM_THREADREADY          WM_USER+24
                                        /* Sent by working thread to client window when startup finished */
#define WM_THREADSTARTUPREADY   WM_USER+25
                                        /* Sent to working thread to reinitialize to root of all drives */
#define WM_SETDRIVEROOT         WM_USER+26
                                        /* Sent to spooler window to bring to top of Z-order */
#define WM_SHOWSPOOLER          WM_USER+27
                                        /* Sent to spooler window to refresh the container holding
                                           the real-time status of the printer queues */
#define WM_REFRESHSPOOLER       WM_USER+28
                                        /* Sent to Overview Window to change FCF_* flags */
#define WM_OVERVIEWFCF          WM_USER+29
                                        /* Sent by PC2HOOK.DLL to display next/window window within
                                           QuickSwitch, used to communicated between main and working
                                           thread about QuickSwitch status too */
#define WM_QUICKSWITCH          WM_USER+30
                                        /* Sent to activate lockup OS/2 function */
#define WM_PC2LOCKUP            WM_USER+31
                                        /* Load environment spaces from PC2.ENV */
#define WM_LOADENVIRONMENTSPACE WM_USER+32
                                        /* Save environment spaces */
#define WM_SAVEENVIRONMENTSPACE WM_USER+33
                                        /* Sent to working thread to update the Popup Menu's linked list
                                           (loading or unloading the icons) */
#define WM_POPUPMENUUPDATE      WM_USER+34
                                        /* Posted by the IO thread to itself to load/unlod the icon
                                           for the current submenu */
#define WM_POPUPMENUSETICON     WM_USER+35
                                        /* Sent to the main thread after finishing the update */
#define WM_POPUPMENUREADY       WM_USER+36
                                        /* Sent to the I/O thread to deallocate MENUDATA structure, which
                                           is used to build the linked list of menuentries */
#define WM_POPUPMENUDELETE      WM_USER+37
                                        /* Modified WM_PRESPARAMCHANGED including the font to be set */
#define WM_SETPRESPARAMFONT     WM_USER+38
                                        /* Posted/Sent to SessionBar's Launch button */
#define WM_DRAWLAUNCHBUTTON     WM_USER+39
                                        /* Posted/Sent to SessionBar's Sessions icons */
#define WM_DRAWSESSIONICONS     WM_USER+40
                                        /* Posted/Sent to SessionBar's Sessions icons */
#define WM_HITTESTSESSIONICONS  WM_USER+41
                                        /* Posted/Sent to SessionBar's Session name textfield */
#define WM_DRAWSESSIONTEXT      WM_USER+42
                                        /* Posted to SessionBar to change a session's icon */
#define WM_SESSIONBARUPDATE     WM_USER+43
                                        /* Sent by main thread to working thread to switch to the selected
                                           SessionBar session. Posted by the main thread to the SessionBar to
                                           adopt itself to configuration changes. Posted by main thread to
                                           itself to create/destroy SessionBar, depending on user selections */
#define WM_SESSIONBAR           WM_USER+44
                                        /* Sent by main thread to itself to show/hide Titlebar Smarticons */
#define WM_TITLEBARICONS        WM_USER+45
                                        /* Posted by PC2HOOK.DLL to working thread and from working thread to
                                           Desktop dialog spy notebook page */
#define WM_SPYFRAMEWINDOW       WM_USER+46
                                        /* Posted by PC2HOOK.DLL to adjust size and/or position of
                                           seamless WIN-OS2 sessions */
#define WM_SEAMLESSADJUST       WM_USER+47
                                        /* Posted/Sent to SessionBar's APM status area */
#define WM_DRAWAPMSTATUS        WM_USER+48
                                        /* Posted by main thread to itself to start cleanup processing */
#define WM_CLEANUP              WM_USER+49
                                        /* Posted/Sent by working thread to itself */
#define WM_APM                  WM_USER+50
                                        /* Posted by PC2HOOK.DLL to working thread to restart delayed
                                           Sliding Focus timer (in 100 ms increments) */
#define WM_SLIDINGFOCUSDELAY    WM_USER+51
                                        /* Posted by PC2HOOK.DLL to working thread to restart delayed
                                           Dynamic Menuselection timer (in 100 ms increments) */
#define WM_DYNAMICMENUDELAY     WM_USER+52
                                        /* Posted by PC2HOOK.DLL to working thread to restart delayed
                                           Virtual Desktop timer (in 100 ms increments) */
#define WM_VIRTUALDESKTOPDELAY  WM_USER+53
                                        /* Posted by PC2HOOK.DLL when mouse pointer enters
                                           or leaves the area that would be covered by the
                                           unhidden SessionBar */
#define WM_SESSIONBARUNHIDE     WM_USER+54
                                        /* Posted/Sent to calculate SessionBar's CPU Performance */
#define WM_CALCULATECPUSTATUS   WM_USER+55
                                        /* Posted/Sent to SessionBar's CPU Performance graphics */
#define WM_DRAWCPUSTATUS        WM_USER+56
                                        /* Posted/Sent to SessionBar's CPU Performance */
#define WM_HITTESTCPUSTATUS     WM_USER+57
                                        /* Posted/Sent to SessionBar's Statusarea */
#define WM_HITTESTSTATUSAREA    WM_USER+58
                                        /* Posted by PC2HOOK.DLL to working thread to restart delayed
                                           Overview Window timer (in 100 ms increments) */
#define WM_OVERVIEWUNHIDEDELAY  WM_USER+59
                                        /* Posted by a Thread that recovered from a DosKillThread()
                                           to allow main decrement Exception Handler semaphore */
#define WM_EXCPSEMDECREMENT     WM_USER+60
                                        /* Sent by SessionBar to show/hide the BubbleHelp window */
#define WM_BUBBLEHELP           WM_USER+61
                                        /* Posted the I/O Thread to parse PC2.TRP for new
                                           exceptions logged in PC2.TRP */
#define WM_ANALYZEPC2TRAP       WM_USER+62

                                        /* Program Type configuration of Program Installation
                                           dialog */
#define WM_SETUPPROGRAMTYPE     WM_USER+100
                                        /* Program Priority configuration of Program Installation
                                           dialog */
#define WM_SETUPPROGRAMPRIORITY WM_USER+101
                                        /* Program Size & Position configuration of Program
                                           Installation dialog */
#define WM_SETUPSIZEPOSITION    WM_USER+102
                                        /* Hotkey settings to be initialized by Program
                                           Installation dialog */
#define WM_SETUPHOTKEY          WM_USER+103
                                        /* Sent by dialogs to dialog window procedure to initialize
                                           notebook pages with user data */
#define WM_SETUPNOTEBOOKPAGES   WM_USER+104
                                        /* Control dialog to setup path info */
#define WM_SETUPPATH            WM_USER+105
                                        /* Posted to Network thread to access Internet */
#define WM_NETACCESS            WM_USER+106
                                        /* Posted by Network thread to itself to schedule an Internet
                                           access. Parameter mp1 contains the NET_* function selection */
#define WM_NETSCHEDULE          WM_USER+107
                                        /* Sent to Network thread to display a status message */
#define WM_NETSTATUS            WM_USER+108
                                        /* Load the current level into configuration listbox */
#define WM_RELOADPOPUPMENU      WM_USER+109

                                        /* Posted by the Input hook to the SessionBar if the mouse just
                                           moved out */
#define WM_MOUSELEAVE           WM_USER+200
                                        /* Posted by the Sendmsg hook and Working Thread during delayed
                                           Dynamic Menuselection */
#define WM_MOUSESELECT          WM_USER+201

                                        /* Remove trailing \r and \n characters */
extern char             *StripTrailing(char *pcString);
                                        /* Remove leading spaces */
extern char             *StripLeading(char *pcString);
                                        /* Procedures */
extern BOOL             WinStartUp(HAB *pHab, HMQ *pHmq);
extern BOOL             WinStartHelp(HAB hab, HMODULE hmodLibrary, UCHAR *pHelpfile, HWND *pHwndHelp, HWND HwndFrame);
extern BOOL             WinCloseDown(HWND *pHwndHelp, HAB *pHab, HMQ *pHmq);
extern UCHAR            *CreateEnvironment(PSZ SrcEnvironment, PSZ PgmEnvironment);
extern UCHAR            *ConvertEnvironment(PSZ PgmEnvironment);
extern UCHAR            *SessionEnvironment(MENUDATA *pMenuData);
extern void             StartSession(SESSIONDATA *SessionData, MENUDATA *pMenuData);
extern BOOL             LoadMenuData2SessionData(MENUDATA *pMenuData, SESSIONDATA *pSessionData);
extern BOOL             LoadSessionData2MenuData(MENUDATA *pMenuData, SESSIONDATA *pSessionData);
extern MENUDATA         *AllocateMenuData(MENUDATA *pMenuDataCloneFrom);
extern void             DeAllocateMenuData(MENUDATA *pMenuData);
extern MRESULT          LoadSchedulerData(SCHEDULERDATA *pSchedulerData, FILE *fileProfile);
extern MRESULT          SaveSchedulerData(SCHEDULERDATA *pSchedulerData, FILE *fileProfile);
extern void             LoadMenu(FILE *pfileProfile, MENUDATA *pMenuData, BOOL bReadNextLine);
extern void             SaveMenu(FILE *pfileProfile, MENUDATA *pMenuData);
extern void             FreeMenu(MENUDATA *pMenuData, BOOL bCurrentOnly);
extern ENVIRONMENTLIST  *LoadEnvironmentSpaces(FILE *pfileProfile);
extern void             SaveEnvironmentSpaces(ENVIRONMENTLIST *pEnvironmentList, FILE *pfileProfile, ULONG ulAsBatch);
extern void             FreeEnvironmentSpaces(ENVIRONMENTLIST *pEnvironmentList);
extern MENUDATA        *SetPMMenu(HWND hwndMenu, MENUDATA *pMenuData);
extern MRESULT          SetPopupMenu(HWND hwndMenu, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MENUDATA         *SearchItem(MENUDATA *pMD, ULONG *id);
extern MENUDATA         *ScheduleItem(MENUDATA *pMD, SCHEDULERDATA *pSDCurrent);
extern ULONG            SetItemStyle(MENUDATA *pMD, BOOL bCurrentOnly, BOOL bOwnerDraw);
extern ENVIRONMENTLIST  *SearchEnvironment(UCHAR *pucEnvironmentName);
extern MENUDATA         *SearchTitle(MENUDATA *pMD, WINDOWDATA *pWD);
extern MENUDATA         *ApplyEnvironment(MENUDATA *pMenuData, UCHAR *pucEnvironmentSpaces, ULONG ulOperation);
extern void             SearchAutoStartItem(MENUDATA *pMD, ULONG *pulAutostartCount, ULONG ulAutostartDelay);
extern void             InstallFilename2Dialog(HWND hwndDlg, UCHAR *pucFullFileName, BOOL bObject);
extern void             DisableDialogItem(HWND hwndDlg, USHORT usDialogIDs[], USHORT usItemCount, ULONG ulStyle);
extern ULONG            INIAccess(UCHAR *pucFilenameINI, BOOL bRead);
extern void             DrawWindow(HPS hpsClient, SWP *pswpWindow, ULONG ulStyle, HPOINTER hptrIcon, PSZ pszWindowName, ULONG ulWindowNumber, ULONG ulWindowCount);
extern UCHAR            *ImportDosSettings(UCHAR *pucSettings, ULONG *pulLength, BOOL bImport);
extern ULONG            LoadProgDetails2MenuData(MENUDATA *pMenuData, PROGDETAILS *pProgDetails);
extern ULONG            SetDriveRoot(void);
extern ULONG            ConvertWPSObjectList2MenuData(MENUDATA *pMenuData, WPSOBJECTLIST *pListRoot);
extern UCHAR            *LocateInEnvironment(UCHAR *pucEnvironment, UCHAR *pucEnvironmentVariable, UCHAR *pucObject);
extern ULONG            InitializeIcon(MENUDATA *pMenuData, BOOL bLoad);
extern MRESULT          CenterWindow(HWND hwndDlg);
extern ULONG            BlitIcon(HPS hpsDestination, HPOINTER hpointerIcon, RECTL *prectlIcon);
extern ULONG            BlitBitmap(HWND hwndDestination, HPS hpsDestination, HBITMAP hbitmapBitmap, RECTL *prectlBitmap);
extern ULONG            DisableWindowRect(HPS hpsDestination, RECTL *prectlDestination, ULONG ulBackgroundColor);
extern FATTRS           *QueryPresParamFont(HWND hwndWindow);
extern ULONG            SetPresParamFont(HPS hpsWindow, FATTRS *pfatWindow, ULONG ullcidWindowFont);
extern MRESULT          GetWindowList(HAB hab, ULONG ulTask, MPARAM mp1);
extern ULONG            IsWindowInRectangle(RECTL *prectlWindow, RECTL *prectlRectangle, ULONG ulDirection);
extern MRESULT          GetDesktopWindows(HAB hab, ULONG ulTask, MPARAM mp1);
extern HWND             GetZOrderTop(void);
extern UCHAR            *GetDateTimeNLS(ULONG ulInitialize);
extern ULONG            ExpandMacros(UCHAR *pucDestination, UCHAR *pucSource, UCHAR *pucEnvironment);
extern ULONG            CopySetupFiles(HWND hwndOwner, ULONG ulRequested);
extern ULONG            EnsureUniqueStrings(UCHAR *pucStrings);

                                        /* Dialog.c */
extern MRESULT          ProcessPageKey(HWND hwndPage, HWND hwndDialog, MPARAM mp1, MPARAM mp2);
extern MRESULT          DispatchKeyToNotebook(ULONG *pulRecursion, HWND hwndDialog, HWND hwndNotebook, PFNWP pfnwpNotebook, MPARAM  mp1, MPARAM mp2);
extern MPARAM           AddPagesToNotebook(HWND hwndDialog, HWND hwndNotebook, NBPAGE nbpageNotebook[],
                                           USHORT *pusTabTextLength, USHORT *pusTabTextHeight);
extern MPARAM           AdjustNotebook(HWND hwndDialog, HWND hwndNotebook, ULONG ulNotebookStyle,
                                       USHORT usTabTextLength, USHORT usTabTextHeight);
extern MRESULT EXPENTRY SubclassedNotebookProcedure(HWND hwndNotebook, ULONG msg, MPARAM mp1, MPARAM mp2);
extern UCHAR            *QueryEnvironmentSpaces(UCHAR *pucConcat, HWND hwndDlg, USHORT usListBoxID);
extern MRESULT          GetSchedulerData(SCHEDULERDATA *pSchedulerData, HWND hwndNotebookPage);
extern MRESULT          SetSchedulerData(SCHEDULERDATA *pSchedulerData, HWND hwndNotebookPage);
extern MRESULT          CheckSchedulerSettings(ULONG ulControl, HWND hwndNotebookPage);
extern MRESULT EXPENTRY DragDropDialogProcedure(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2, USHORT usControl);
extern BOOL             GetWindowName(HWND hwndParent, PSZ pszName, int iNameSize);

                                        /* Pc2Spool.c */
extern ULONG            SpoolerInitialize(HMODULE hDLLPc2Hook);
extern ULONG            RefreshSpooler(void);
extern SPOOLERRECORD    *QuerySelectedRecords(SPOOLERRECORD *pSelectedSpoolerRecord);
extern void             NotifySelectedRecords(ULONG ulActionCode);

                                        /* Pc2Net.c */
extern MRESULT EXPENTRY NetworkInterface(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT EXPENTRY StatusWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern AUTOUPDATE       *LoadAutoUpdate(UCHAR *pucFilename, UCHAR *pucDocument);
extern AUTOUPDATE       *SaveAutoUpdate(UCHAR *pucFilename, AUTOUPDATE *pautoupdateRoot);
extern LONG             FreeAutoUpdate(AUTOUPDATE *pautoupdateDestroy);
extern UCHAR            *CheckNews(AUTOUPDATE *pautoupdateRemote, AUTOUPDATE *pautoupdateLocal);
extern UCHAR            *CheckUpdate(URL *purlProxy, URL *purlServer,AUTOUPDATE *pautoupdateRemote, AUTOUPDATE *pautoupdateLocal);
extern LONG             ParseUrl(URL *purlServer, UCHAR *pucUrl, ULONG ulServiceDefault, ULONG ulPortDefault);
extern LONG             InterfaceStatus(ULONG *pulConnectionLAN, ULONG *pulConnectionPPP);
extern LONG             SocketInit(void);
extern LONG             ConnectionOpen(URL *purlProxy, URL *purlServer);
extern LONG             ConnectionClose(URL *purlProxy, URL *purlServer);
extern LONG             RequestSend(URL *purlProxy, URL *purlServer, UCHAR *pucDocument, BOOL bAuthenticate);
extern LONG             RequestPost(URL *purlProxy, URL *purlServer, UCHAR *pucDocument, UCHAR *pucParameters, BOOL bAuthenticate);
extern LONG             RequestReceive(URL *purlProxy, URL *purlServer, UCHAR **ppucDocument, ULONG *pulDocumentLength);
extern UCHAR            *ParseCode(UCHAR *pucString);
extern UCHAR            *EncodeBase64(UCHAR *pucUserid, UCHAR *pucPassword);

                                        /* Pc2Hook.c */
extern  ULONG           FormatClipbrdData(HWND hwnd);

                                        /* Except.c */
extern  ULONG           DumpThread(PIB *ppibThread, TIB *ptibThread, ULONG ulThreadIndex);
extern  ULONG           RegisterExceptionHandler(ULONG ulRegisterFlag, EXCEPTIONREGISTRATIONRECORD *pexcpregrecThread, UCHAR *pucExceptionFile);
extern  ULONG _System   ThreadExceptionHandler(PEXCEPTIONREPORTRECORD pexcpreprec,
                                               PEXCEPTIONREGISTRATIONRECORD pexcpregrec,
                                               PCONTEXTRECORD pcontextrec,
                                               PVOID pvoid);

                                        /* DLL signature */
#define DLLENTRYPOINTSIGNATURE          1

                                        /* DLL functions (see PC2HOOK.DEF for exported ordinals) */
#define DLLENTRYPOINTINIT               2
#define DLLENTRYPOINTCLEANUP            3
#define DLLENTRYPOINTINPUTHOOK          4
#define DLLENTRYPOINTPREACCELHOOK       5
#define DLLENTRYPOINTWINSENDMSGHOOK     6
#define DLLENTRYPOINTHOOKPARAMETERS     10
#define DLLENTRYPOINTKEYDATA            11

                                        /* DLL functions (see PC2NET.DEF for exported ordinals) */
#define DLLENTRYPOINTNETWORKINTERFACE   2
#define DLLENTRYPOINTSTATUSWINDOWPROC   3

                                        /* DLL functions (see PC2SPOOL.DEF for exported ordinals) */
#define DLLENTRYPOINTSPOOLERINITIALIZE  2

extern PFFUNCPTR1       *pInitialize;
extern PFFUNCPTR1       *pCleanup;
extern PFFUNCPTR2       *pInputHook;
extern PFFUNCPTR3       *pWinSendMsgHook;
extern PFFUNCPTR2       *pPreAccelHook;

                                        /* PC/2's working thread */
extern void _Optlink    PC2_Thread(void *ThreadArg);

                                        /* Window procedures */
extern MRESULT  EXPENTRY PC2_MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY PI_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY PI_NotebookProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY CC_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY CC_NotebookProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY AD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY MI_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY MI_NotebookProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY CD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY SD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY SE_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY SU_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY RD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY DD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY DD_NotebookProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY MD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY LD_DialogProcedure(HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY SubclassedMenuWindowProc(HWND hwndMenu, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY SubclassedListboxWindowProc(HWND hwndLB, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY PC2_SwitchWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY PC2_SessionWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY PC2_BubbleHelpWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

                                        /* Working thread (object) window procedures */
extern MRESULT  EXPENTRY PC2_WorkThreadWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY PC2_IOThreadWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY PC2_NetThreadWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

                                        /* Spooler window procedures */
extern MRESULT  EXPENTRY PC2_SpoolerWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
extern MRESULT  EXPENTRY SubclassedContainerWindowProc(HWND hwndMenu, ULONG msg, MPARAM mp1, MPARAM mp2);

                                        /* Pointer to PC/2 control structure loaded from PC2HOOK.DLL */
extern HOOKPARAMETERS   *pHP;

                                        /* Scheduler data */
#define NUMSCHEDULE             7
#define NUMWEEKDAYS             7
#define NUMMONTHS               12

extern UCHAR *pucSchedulerControl[NUMSCHEDULE];
extern UCHAR *pucWeekDay[NUMWEEKDAYS];
extern UCHAR *pucMonth[NUMMONTHS];

                                        /* Pointer to OS/2 APIs dynamically linked */
#if defined(IBMTLKT30) || defined(IBMTLKT40)
#
#else
extern DOSSETEXTLIBPATH *DosSetExtLIBPATH;
extern DOSQUERYEXTLIBPATH
                        *DosQueryExtLIBPATH;
#endif /* IBMTLKT30 IBMTLKT40 */

#if defined(IBMTLKT21) || defined(IBMTLKT30) || defined(IBMTLKT40)
                                        /* Undocumented styles of WinDrawBorder() */
#define DB_RAISED                   0x0400
#define DB_DEPRESSED                0x0800
#define DB_TROUGH                   0x1000
#define DB_FENCE                    0x2000
#define DB_FIELD                    0x4000
#define DB_CORNERBORDER             0x8000
#endif /* IBMTLKT21 || IBMTLKT30 || IBMTLKT40 */

