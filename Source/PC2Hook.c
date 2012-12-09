/***********************************************************************\
 *                               PC2.c                                 *
 *             Copyright (C) by Stangl Roman, 1993, 2000               *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed.                                                            *
 *                                                                     *
 * Pc2Hook.c    Hook the input queue to filter certain messages.       *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#include        "PC2.h"                 /* User include files */
#include        "Error.h"

#define         _FILE_  "PC2Hook.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;
const ULONG     _PC2HOOKSIGNATURE=BLDLEVEL_SIGNATURE;

                                        /* To de/activate the TitleBar SmartIcons we need to post/send a
                                           unique message. I though that enhancing WM_SYSCOMMAND by not
                                           used parameters would work and does but for NetScape for OS/2,
                                           which traps during WM_DESTROY, so hopefully this alternative doesn't
                                           break applications other than NetScape */
#define         WM_SMARTICONS       (WM_USER+1234)

                                        /* When detecting mouse movements we compare the current mouse
                                           position with the previously saved position. If the mouse
                                           has not moved more than MOUSEMOTION pixels in on direction,
                                           we assume no mouse movement at all (as knocking on the desk
                                           may move the mouse pointer a few mickeys) */
#define         MOUSEMOTION         3

                                        /* Status flags for ulPopupKeyStatus */
#define         MAKEBREAKINVALID    0x00000001

                                        /* SHIFT keys scan code */
#define         LEFT_SHIFT_KEY      0x2A
#define         RIGHT_SHIFT_KEY     0x36
                                        /* CTRL keys scan code */
#define         LEFT_CTRL_KEY       0x1D
#define         RIGHT_CTRL_KEY      0x5B
                                        /* It is peculiar, but an IBM WIN95 keyboard generates other
                                           scancodes than OEM WIN95 keyboards, the left WIN95 key provides
                                           the same scancode as the right CTRL key, and  the secondd
                                           parameter depends on being on a PM or VIO window. Thanks to
                                           Joel Swartz for finding out the codes of OEM WIN95 keyboards.
                                           The parameters of WM_CHAR get applied a mask */
#define         MASK_WIN95_KEY_MP1      0xFF000000
#define         MASK_WIN95_KEY_MP2      0xFFFFFFFF
                                        /* The left CTRL key (for mp2 P is for PM windows, W for VIO windows,
                                           D is for down and U is for up) */
#define         LCTRL_KEY_MP1           0x1D000000
#define         LCTRL_KEY_MP2_P         0x000A0000
#define         LCTRL_KEY_MP2_WD        0x00070000
#define         LCTRL_KEY_MP2_WA        0x00470000
                                        /* The right CTRL key */
#define         RCTRL_KEY_MP1           0x5B000000
#define         RCTRL_KEY_MP2_P         0x000A0000
#define         RCTRL_KEY_MP2_WD        0x00870000
#define         RCTRL_KEY_MP2_WU        0x00C70000

                                        /* The Sroll Lock key */
#define         SLOCK_KEY_MP1           0x46000000
#define         SLOCK_KEY_MP2_P         0x001C0000
#define         SLOCK_KEY_MP2_WD        0x00070000
#define         SLOCK_KEY_MP2_WU        0x00470000

                                        /* The Caps Lock key */
#define         CLOCK_KEY_MP1           0x3A000000
#define         CLOCK_KEY_MP2_P         0x000E0000
#define         CLOCK_KEY_MP2_WD        0x00070000
#define         CLOCK_KEY_MP2_WU        0x00470000

                                        /* The left WIN95 key */
#define         LWIN95_KEY_IBMMP1       0x5B000000
#define         LWIN95_KEY_IBMMP2_P     0x000AEC00
#define         LWIN95_KEY_IBMMP2_WD    0x00A0EC00
#define         LWIN95_KEY_IBMMP2_WU    0x00E0EC00

#define         LWIN95_KEY_OEMMP1       0x7E000000
#define         LWIN95_KEY_OEMMP2_P     0x0000EC00
#define         LWIN95_KEY_OEMMP2_WD    0x00A0EC00
#define         LWIN95_KEY_OEMMP2_WU    0x00E0EC00

                                        /* The right WIN95 key */
#define         RWIN95_KEY_IBMMP1       0x5C000000
#define         RWIN95_KEY_IBMMP2_PD    0x0000002F
#define         RWIN95_KEY_IBMMP2_PU    0x0000ED00
#define         RWIN95_KEY_IBMMP2_WD    0x00A0ED00
#define         RWIN95_KEY_IBMMP2_WU    0x00E0ED00

#define         RWIN95_KEY_OEMMP1       0x7F000000
#define         RWIN95_KEY_OEMMP2_P     0x0000ED00
#define         RWIN95_KEY_OEMMP2_WD    0x00A0ED00
#define         RWIN95_KEY_OEMMP2_WU    0x00E0ED00

                                        /* Finally the WIN95 menu key (the IBM keyboard generates the
                                           VK_PRINTSCRN code additionally, this is just crazy) */
#define         MWIN95_KEY_IBMMP1       0x5D000000
#define         MWIN95_KEY_IBMMP2_P     0x0019EE00
#define         MWIN95_KEY_IBMMP2_WD    0x00A0EE00
#define         MWIN95_KEY_IBMMP2_WU    0x00E0EE00

#define         MWIN95_KEY_OEMMP1       0x7C000000
#define         MWIN95_KEY_OEMMP2_P     0x0000EE00
#define         MWIN95_KEY_OEMMP2_WD    0x00A0EE00
#define         MWIN95_KEY_OEMMP2_WU    0x00E0EE00

                                        /* Cursor keys */
#define         CUR_KEY_UP              0x61000000
#define         CUR_KEY_LEFT            0x63000000
#define         CUR_KEY_DOWN            0x66000000
#define         CUR_KEY_RIGHT           0x64000000
                                        /* Nummeric keypad cursor keys */
#define         NUM_KEY_LEFT_UP         0x47000000
#define         NUM_KEY_UP              0x48000000
#define         NUM_KEY_RIGHT_UP        0x49000000
#define         NUM_KEY_LEFT            0x4B000000
#define         NUM_KEY_RIGHT           0x4D000000
#define         NUM_KEY_LEFT_DOWN       0x4F000000
#define         NUM_KEY_DOWN            0x50000000
#define         NUM_KEY_RIGHT_DOWN      0x51000000

                                        /* Status flags for ulShiftStatus */
#define         LEFT_SHIFT          0x00000002
#define         RIGHT_SHIFT         0x00000001

                                        /* TAB key scan code */
#define         TAB_KEY             0x0F

                                        /* Window classes of seamless WIN-OS2 windows */
#define         SHIELD_SEAMLESS     "SeamlessClass"

                                        /* Window classes of AVIO windows */
#define         SHIELD_AVIO         "Shield"
                                        /* Shield (AVIO) window system menu IDs for marking */
#define         SHIELD_MARK         0x0000009C
#define         SHIELD_COPY         0x0000009D
#define         SHIELD_PASTE        0x0000009F

                                        /* Window classes of EPM editors */
#define         EPM_551             "Editor"
#define         EPM_603             "NewEditWndClass"
#define         EPM_VERSION_551     0551L
#define         EPM_VERSION_603     0603L
                                        /* EPM window menubar ID's for marking */
                                        /* Identifiers used for EPM 5.51 and 6.03b (!) */
#define         EPM_551_COPY        0x00000328
#define         EPM_551_PASTE       0x0000032A
                                        /* Identifiers used for EPM 6.03a */
#define         EPM_603_COPY        0x000000D2
#define         EPM_603_PASTE       0x000000E7

                                        /* Window classes of E editor */
#define         E_EDITOR            "EHXMAIN"
#define         E_EDITOR_COPY       0x00000423
#define         E_EDITOR_PASTE      0x00000424

                                        /* Window classes of CM/2 */
#define         CM2_110             "KWM_CLIENT_CLASS"
                                        /* IBM Communications Manager/2 menubar IDs for marking */
#define         CM2_CUT             0x00000007
#define         CM2_COPY            0x00000008
#define         CM2_PASTE           0x00000009

                                        /* Window classes of Personal Communications/3270 */
#define         PCM_4XX             "PCSWS:Pres:"
                                        /* IBM Personal Communications/3270 menubar IDs for marking */
#define         PCM_CUT             0x00000412
#define         PCM_COPY            0x00000413
#define         PCM_PASTE           0x00000414

                                        /* Window classes of PassPort/2 */
#define         PP2_131             "Passport.C"
                                        /* Advantis PassPort/2 menubar IDs for marking */
#define         PP2_CUT             0x0000025D
#define         PP2_COPY            0x0000025C
#define         PP2_PASTE           0x00000270

                                        /* Titlebar of NetScape Communicator/2 2.02 */
#define         NETSCAPE_202        "Netscape - "
#define         NETSCAPE_202_COPY   0x000E122

                                        /* Window class of NetScape Communicator/2 4.04 */
#define         NETSCAPE_404        "NSAPI_0000"
#define         NETSCAPE_404_COPY    0x000E122

                                        /* Window classes of Lotus Notes 4.52 */
#define         NOTES_452           "NotesRichText"
                                        /* Notes menubar IDs for marking */
#define         NOTES_COPY          0x00007C02
#define         NOTES_PASTE         0x00007C03

                                        /* Subclassed frame windows' instance data length of private
                                           memory allocated to avoid stack usage */
#define         WND_DATA_LEN        ((4096+sizeof(FRAMEDATA)) & 0xFFFFF000)

                                        /* Reroute message from original destionation
                                           to NULLHANDLE and return TRUE no to pass message
                                           to next hook in chain */
#define         REROUTEMSGTONULL()  {                           \
                                        pqmsg->hwnd=NULLHANDLE; \
                                        return(TRUE);           \
                                    }

/*--------------------------------------------------------------------------------------*\
 * The following functions are exported in PC2Hook.def                                  *
\*--------------------------------------------------------------------------------------*/
ULONG EXPENTRY      PC2DLL_Initialize(void);
ULONG EXPENTRY      PC2DLL_Cleanup(void);
BOOL EXPENTRY       PC2DLL_InputHook(HAB hab, PQMSG pqmsg, ULONG option);
BOOL EXPENTRY       PC2DLL_PreAccelHook(HAB hab, PQMSG pqmsg, ULONG option);
void EXPENTRY       PC2DLL_WinSendMsgHook(HAB hab, PSMHSTRUCT pSmh, BOOL bInterTask);

/*--------------------------------------------------------------------------------------*\
 * Function prototypes used locally within PC2Hook.c                                    *
\*--------------------------------------------------------------------------------------*/
ULONG               FormatClipbrdData(HWND hwnd);
ULONG               CopyToClipbrdData(HWND hwnd);
LONG                AdjustOverlap(SWP *pswpDestination, SWP *pswpSource);
BOOL                LookupMTEName(HWND hwndFrame, UCHAR *pucProcessList);
FRAMELIST           *LookupFrame(HWND hwndFrame);
FRAMELIST           *RegisterFrame(HWND hwndFrame, BOOL bSet);
MRESULT EXPENTRY    pfnwpDispatchFrameWindowProc(HWND hwndFrame, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    pfnwpSubclassFrameWindowProc(FRAMELIST *pframelistCurrent, FRAMEDATA *pframedataCurrent, HWND hwndFrame, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    pfnwpFrameWindowProc(HWND hwndFrame, ULONG msg, MPARAM mp1, MPARAM mp2);

/*--------------------------------------------------------------------------------------*\
 * The following datastructures are exported in PC2Hook.def                             *
\*--------------------------------------------------------------------------------------*/
HOOKPARAMETERS  HookParameters;         /* Central control structure for PC/2 */
                                        /* Hotkeys detected within PC/2. PC/2 detects the scan code of
                                           a key's WM_CHAR message, and used this table as a lookup whether
                                           the key is used as a Hotkey or not */
KEYDATA         KeyData[KEYDATACOUNT]={
                                        /* Left WIN key */
                    {KC_LWIN       , '0', 0x0B, FALSE, NULL}, {KC_LWIN       , '1', 0x02, FALSE, NULL},
                    {KC_LWIN       , '2', 0x03, FALSE, NULL}, {KC_LWIN       , '3', 0x04, FALSE, NULL},
                    {KC_LWIN       , '4', 0x05, FALSE, NULL}, {KC_LWIN       , '5', 0x06, FALSE, NULL},
                    {KC_LWIN       , '6', 0x07, FALSE, NULL}, {KC_LWIN       , '7', 0x08, FALSE, NULL},
                    {KC_LWIN       , '8', 0x09, FALSE, NULL}, {KC_LWIN       , '9', 0x0A, FALSE, NULL},
                    {KC_LWIN       , 'A', 0x1E, FALSE, NULL}, {KC_LWIN       , 'B', 0x30, FALSE, NULL},
                    {KC_LWIN       , 'C', 0x2E, FALSE, NULL}, {KC_LWIN       , 'D', 0x20, FALSE, NULL},
                    {KC_LWIN       , 'E', 0x12, FALSE, NULL}, {KC_LWIN       , 'F', 0x21, FALSE, NULL},
                    {KC_LWIN       , 'G', 0x22, FALSE, NULL}, {KC_LWIN       , 'H', 0x23, FALSE, NULL},
                    {KC_LWIN       , 'I', 0x17, FALSE, NULL}, {KC_LWIN       , 'J', 0x24, FALSE, NULL},
                    {KC_LWIN       , 'K', 0x25, FALSE, NULL}, {KC_LWIN       , 'L', 0x26, FALSE, NULL},
                    {KC_LWIN       , 'M', 0x32, FALSE, NULL}, {KC_LWIN       , 'N', 0x31, FALSE, NULL},
                    {KC_LWIN       , 'O', 0x18, FALSE, NULL}, {KC_LWIN       , 'P', 0x19, FALSE, NULL},
                    {KC_LWIN       , 'Q', 0x10, FALSE, NULL}, {KC_LWIN       , 'R', 0x13, FALSE, NULL},
                    {KC_LWIN       , 'S', 0x1F, FALSE, NULL}, {KC_LWIN       , 'T', 0x14, FALSE, NULL},
                    {KC_LWIN       , 'U', 0x16, FALSE, NULL}, {KC_LWIN       , 'V', 0x2F, FALSE, NULL},
                    {KC_LWIN       , 'W', 0x11, FALSE, NULL}, {KC_LWIN       , 'X', 0x2D, FALSE, NULL},
                    {KC_LWIN       , 'Y', 0x15, FALSE, NULL}, {KC_LWIN       , 'Z', 0x2C, FALSE, NULL},
                                        /* Right CTRL key */
                    {KC_RCTRL      , '0', 0x0B, FALSE, NULL}, {KC_RCTRL      , '1', 0x02, FALSE, NULL},
                    {KC_RCTRL      , '2', 0x03, FALSE, NULL}, {KC_RCTRL      , '3', 0x04, FALSE, NULL},
                    {KC_RCTRL      , '4', 0x05, FALSE, NULL}, {KC_RCTRL      , '5', 0x06, FALSE, NULL},
                    {KC_RCTRL      , '6', 0x07, FALSE, NULL}, {KC_RCTRL      , '7', 0x08, FALSE, NULL},
                    {KC_RCTRL      , '8', 0x09, FALSE, NULL}, {KC_RCTRL      , '9', 0x0A, FALSE, NULL},
                    {KC_RCTRL      , 'A', 0x1E, FALSE, NULL}, {KC_RCTRL      , 'B', 0x30, FALSE, NULL},
                    {KC_RCTRL      , 'C', 0x2E, FALSE, NULL}, {KC_RCTRL      , 'D', 0x20, FALSE, NULL},
                    {KC_RCTRL      , 'E', 0x12, FALSE, NULL}, {KC_RCTRL      , 'F', 0x21, FALSE, NULL},
                    {KC_RCTRL      , 'G', 0x22, FALSE, NULL}, {KC_RCTRL      , 'H', 0x23, FALSE, NULL},
                    {KC_RCTRL      , 'I', 0x17, FALSE, NULL}, {KC_RCTRL      , 'J', 0x24, FALSE, NULL},
                    {KC_RCTRL      , 'K', 0x25, FALSE, NULL}, {KC_RCTRL      , 'L', 0x26, FALSE, NULL},
                    {KC_RCTRL      , 'M', 0x32, FALSE, NULL}, {KC_RCTRL      , 'N', 0x31, FALSE, NULL},
                    {KC_RCTRL      , 'O', 0x18, FALSE, NULL}, {KC_RCTRL      , 'P', 0x19, FALSE, NULL},
                    {KC_RCTRL      , 'Q', 0x10, FALSE, NULL}, {KC_RCTRL      , 'R', 0x13, FALSE, NULL},
                    {KC_RCTRL      , 'S', 0x1F, FALSE, NULL}, {KC_RCTRL      , 'T', 0x14, FALSE, NULL},
                    {KC_RCTRL      , 'U', 0x16, FALSE, NULL}, {KC_RCTRL      , 'V', 0x2F, FALSE, NULL},
                    {KC_RCTRL      , 'W', 0x11, FALSE, NULL}, {KC_RCTRL      , 'X', 0x2D, FALSE, NULL},
                    {KC_RCTRL      , 'Y', 0x15, FALSE, NULL}, {KC_RCTRL      , 'Z', 0x2C, FALSE, NULL},
                                        /* Both CTRL keys */
                    {KC_CTRL       , '0', 0x0B, FALSE, NULL}, {KC_CTRL       , '1', 0x02, FALSE, NULL},
                    {KC_CTRL       , '2', 0x03, FALSE, NULL}, {KC_CTRL       , '3', 0x04, FALSE, NULL},
                    {KC_CTRL       , '4', 0x05, FALSE, NULL}, {KC_CTRL       , '5', 0x06, FALSE, NULL},
                    {KC_CTRL       , '6', 0x07, FALSE, NULL}, {KC_CTRL       , '7', 0x08, FALSE, NULL},
                    {KC_CTRL       , '8', 0x09, FALSE, NULL}, {KC_CTRL       , '9', 0x0A, FALSE, NULL},
                    {KC_CTRL       , 'A', 0x1E, FALSE, NULL}, {KC_CTRL       , 'B', 0x30, FALSE, NULL},
                    {KC_CTRL       , 'C', 0x2E, FALSE, NULL}, {KC_CTRL       , 'D', 0x20, FALSE, NULL},
                    {KC_CTRL       , 'E', 0x12, FALSE, NULL}, {KC_CTRL       , 'F', 0x21, FALSE, NULL},
                    {KC_CTRL       , 'G', 0x22, FALSE, NULL}, {KC_CTRL       , 'H', 0x23, FALSE, NULL},
                    {KC_CTRL       , 'I', 0x17, FALSE, NULL}, {KC_CTRL       , 'J', 0x24, FALSE, NULL},
                    {KC_CTRL       , 'K', 0x25, FALSE, NULL}, {KC_CTRL       , 'L', 0x26, FALSE, NULL},
                    {KC_CTRL       , 'M', 0x32, FALSE, NULL}, {KC_CTRL       , 'N', 0x31, FALSE, NULL},
                    {KC_CTRL       , 'O', 0x18, FALSE, NULL}, {KC_CTRL       , 'P', 0x19, FALSE, NULL},
                    {KC_CTRL       , 'Q', 0x10, FALSE, NULL}, {KC_CTRL       , 'R', 0x13, FALSE, NULL},
                    {KC_CTRL       , 'S', 0x1F, FALSE, NULL}, {KC_CTRL       , 'T', 0x14, FALSE, NULL},
                    {KC_CTRL       , 'U', 0x16, FALSE, NULL}, {KC_CTRL       , 'V', 0x2F, FALSE, NULL},
                    {KC_CTRL       , 'W', 0x11, FALSE, NULL}, {KC_CTRL       , 'X', 0x2D, FALSE, NULL},
                    {KC_CTRL       , 'Y', 0x15, FALSE, NULL}, {KC_CTRL       , 'Z', 0x2C, FALSE, NULL},
                                        /* Both ALT keys (though today the right ALT is usually ALTGR) */
                    {KC_ALT        , '0', 0x0B, FALSE, NULL}, {KC_ALT        , '1', 0x02, FALSE, NULL},
                    {KC_ALT        , '2', 0x03, FALSE, NULL}, {KC_ALT        , '3', 0x04, FALSE, NULL},
                    {KC_ALT        , '4', 0x05, FALSE, NULL}, {KC_ALT        , '5', 0x06, FALSE, NULL},
                    {KC_ALT        , '6', 0x07, FALSE, NULL}, {KC_ALT        , '7', 0x08, FALSE, NULL},
                    {KC_ALT        , '8', 0x09, FALSE, NULL}, {KC_ALT        , '9', 0x0A, FALSE, NULL},
                    {KC_ALT        , 'A', 0x1E, FALSE, NULL}, {KC_ALT        , 'B', 0x30, FALSE, NULL},
                    {KC_ALT        , 'C', 0x2E, FALSE, NULL}, {KC_ALT        , 'D', 0x20, FALSE, NULL},
                    {KC_ALT        , 'E', 0x12, FALSE, NULL}, {KC_ALT        , 'F', 0x21, FALSE, NULL},
                    {KC_ALT        , 'G', 0x22, FALSE, NULL}, {KC_ALT        , 'H', 0x23, FALSE, NULL},
                    {KC_ALT        , 'I', 0x17, FALSE, NULL}, {KC_ALT        , 'J', 0x24, FALSE, NULL},
                    {KC_ALT        , 'K', 0x25, FALSE, NULL}, {KC_ALT        , 'L', 0x26, FALSE, NULL},
                    {KC_ALT        , 'M', 0x32, FALSE, NULL}, {KC_ALT        , 'N', 0x31, FALSE, NULL},
                    {KC_ALT        , 'O', 0x18, FALSE, NULL}, {KC_ALT        , 'P', 0x19, FALSE, NULL},
                    {KC_ALT        , 'Q', 0x10, FALSE, NULL}, {KC_ALT        , 'R', 0x13, FALSE, NULL},
                    {KC_ALT        , 'S', 0x1F, FALSE, NULL}, {KC_ALT        , 'T', 0x14, FALSE, NULL},
                    {KC_ALT        , 'U', 0x16, FALSE, NULL}, {KC_ALT        , 'V', 0x2F, FALSE, NULL},
                    {KC_ALT        , 'W', 0x11, FALSE, NULL}, {KC_ALT        , 'X', 0x2D, FALSE, NULL},
                    {KC_ALT        , 'Y', 0x15, FALSE, NULL}, {KC_ALT        , 'Z', 0x2C, FALSE, NULL},
                                        /* Both ALT and CTRL keys */
                    {KC_ALT|KC_CTRL, '0', 0x0B, FALSE, NULL}, {KC_ALT|KC_CTRL, '1', 0x02, FALSE, NULL},
                    {KC_ALT|KC_CTRL, '2', 0x03, FALSE, NULL}, {KC_ALT|KC_CTRL, '3', 0x04, FALSE, NULL},
                    {KC_ALT|KC_CTRL, '4', 0x05, FALSE, NULL}, {KC_ALT|KC_CTRL, '5', 0x06, FALSE, NULL},
                    {KC_ALT|KC_CTRL, '6', 0x07, FALSE, NULL}, {KC_ALT|KC_CTRL, '7', 0x08, FALSE, NULL},
                    {KC_ALT|KC_CTRL, '8', 0x09, FALSE, NULL}, {KC_ALT|KC_CTRL, '9', 0x0A, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'A', 0x1E, FALSE, NULL}, {KC_ALT|KC_CTRL, 'B', 0x30, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'C', 0x2E, FALSE, NULL}, {KC_ALT|KC_CTRL, 'D', 0x20, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'E', 0x12, FALSE, NULL}, {KC_ALT|KC_CTRL, 'F', 0x21, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'G', 0x22, FALSE, NULL}, {KC_ALT|KC_CTRL, 'H', 0x23, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'I', 0x17, FALSE, NULL}, {KC_ALT|KC_CTRL, 'J', 0x24, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'K', 0x25, FALSE, NULL}, {KC_ALT|KC_CTRL, 'L', 0x26, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'M', 0x32, FALSE, NULL}, {KC_ALT|KC_CTRL, 'N', 0x31, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'O', 0x18, FALSE, NULL}, {KC_ALT|KC_CTRL, 'P', 0x19, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'Q', 0x10, FALSE, NULL}, {KC_ALT|KC_CTRL, 'R', 0x13, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'S', 0x1F, FALSE, NULL}, {KC_ALT|KC_CTRL, 'T', 0x14, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'U', 0x16, FALSE, NULL}, {KC_ALT|KC_CTRL, 'V', 0x2F, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'W', 0x11, FALSE, NULL}, {KC_ALT|KC_CTRL, 'X', 0x2D, FALSE, NULL},
                    {KC_ALT|KC_CTRL, 'Y', 0x15, FALSE, NULL}, {KC_ALT|KC_CTRL, 'Z', 0x2C, FALSE, NULL} };


/*--------------------------------------------------------------------------------------*\
 * The following datastructures are local for in PC2Hook.dll but shared for all         *
 * processes in whose context this hook gets called.                                    *
\*--------------------------------------------------------------------------------------*/
                                        /* Control structure for activating the Popup Menu via selected
                                           keys (after optinally removing them), if ulWMCHARmp2* is 0,
                                           we don't need to test for the second parameter of WM_CHAR to
                                           match the right key */
POPUPMENU4KBD   Popup4Keyboard[]={
                    { POPUPLEFTCTRL      , FALSE , LCTRL_KEY_MP1         , 0                    , 0                    },
                    { POPUPRIGHTCTRL     , FALSE , RCTRL_KEY_MP1         , RCTRL_KEY_MP2_P      , RCTRL_KEY_MP2_P      },
                    { POPUPRIGHTCTRL     , FALSE , RCTRL_KEY_MP1         , RCTRL_KEY_MP2_WD     , RCTRL_KEY_MP2_WU     },
                    { POPUPLEFTWIN95     , TRUE  , LWIN95_KEY_IBMMP1     , LWIN95_KEY_IBMMP2_P  , LWIN95_KEY_IBMMP2_P  },
                    { POPUPLEFTWIN95     , TRUE  , LWIN95_KEY_IBMMP1     , LWIN95_KEY_IBMMP2_WD , LWIN95_KEY_IBMMP2_WU },
                    { POPUPLEFTWIN95     , TRUE  , LWIN95_KEY_OEMMP1     , LWIN95_KEY_OEMMP2_P  , LWIN95_KEY_OEMMP2_P  },
                    { POPUPLEFTWIN95     , TRUE  , LWIN95_KEY_OEMMP1     , LWIN95_KEY_OEMMP2_WD , LWIN95_KEY_OEMMP2_WU },
                    { POPUPRIGHTWIN95    , TRUE  , RWIN95_KEY_IBMMP1     , RWIN95_KEY_IBMMP2_PD , RWIN95_KEY_IBMMP2_PU },
                    { POPUPRIGHTWIN95    , TRUE  , RWIN95_KEY_IBMMP1     , RWIN95_KEY_IBMMP2_WD , RWIN95_KEY_IBMMP2_WU },
                    { POPUPRIGHTWIN95    , TRUE  , RWIN95_KEY_OEMMP1     , RWIN95_KEY_OEMMP2_P  , RWIN95_KEY_OEMMP2_P  },
                    { POPUPRIGHTWIN95    , TRUE  , RWIN95_KEY_OEMMP1     , RWIN95_KEY_OEMMP2_WD , RWIN95_KEY_OEMMP2_WU },
                    { POPUPWIN95MENU     , TRUE  , MWIN95_KEY_IBMMP1     , MWIN95_KEY_IBMMP2_P  , MWIN95_KEY_IBMMP2_P  },
                    { POPUPWIN95MENU     , TRUE  , MWIN95_KEY_IBMMP1     , MWIN95_KEY_IBMMP2_WD , MWIN95_KEY_IBMMP2_WU },
                    { POPUPWIN95MENU     , TRUE  , MWIN95_KEY_OEMMP1     , MWIN95_KEY_OEMMP2_P  , MWIN95_KEY_OEMMP2_P  },
                    { POPUPWIN95MENU     , TRUE  , MWIN95_KEY_OEMMP1     , MWIN95_KEY_OEMMP2_WD , MWIN95_KEY_OEMMP2_WU },
                    { POPUPCAPSLOCK      , TRUE  , CLOCK_KEY_MP1         , CLOCK_KEY_MP2_P      , CLOCK_KEY_MP2_P      },
                    { POPUPCAPSLOCK      , TRUE  , CLOCK_KEY_MP1         , CLOCK_KEY_MP2_WD     , CLOCK_KEY_MP2_WU     },
                    { POPUPSCROLLLOCK    , TRUE  , SLOCK_KEY_MP1         , SLOCK_KEY_MP2_P      , SLOCK_KEY_MP2_P      },
                    { POPUPSCROLLLOCK    , TRUE  , SLOCK_KEY_MP1         , SLOCK_KEY_MP2_WD     , SLOCK_KEY_MP2_WU     } };
SPYFRAME        spyframeCurrent;        /* Window we are spying */
SPYFRAME        spyframeLast;           /* Window we are spying working copy */
UCHAR   ucClassname[MAXNAMEL+4];        /* Window class e.g. #1 for WC_FRAME */
UCHAR   ucClientClassname[MAXNAMEL+4];  /* Window client class e.g. SeamlessClass for WIN-OS2 sessions */
UCHAR   ucWindowText[MAXNAMEL+4];       /* Window name e.g. OS/2 2.0 Desktop */
UCHAR   ucScanCode;                     /* Keyboard scan code of character pressed */
USHORT  usFlags;                        /* KC_* flags */
USHORT  usFlagsWinKeys;                 /* KC_*WIN* flags (which we support in addition to the keyboard
                                           driver to allow WIN keys to function equivalent to ALT and/or CTRL */
USHORT  usFlagsCtrlKeys;                /* KC_*CTRL flags (which we support in addition to the keyboard
                                           driver to allow differentiation between both CTRL to function 
                                           equivalent to ALT and/or CTRL */
ULONG   ulMoveFlag;                     /* 76543210 (<-Bits)
                                                  | MOVEXR ... Move all windows in x direction
                                                 |  MOVEXL ... Move in -x direction
                                                |   MOVEYU ... Move in y direction
                                               |    MOVEYD ... Move in -y direction
                                              |     MOVED4CLOCK ... Click required to move
                                             |      PTRPOSCHANGED ... Pointer moved
                                            |       PENDINGSWITCH ... Delays switch pending */
ULONG   ulOverviewFlag;                 /* 76543210 (<-Bits)
                                                  | OVERVIEW_UL
                                                 |  OVERVIEW_UR
                                                |   OVERVIEW_LL
                                               |    OVERVIEW_UR */
ULONG   ulButtonDown;                   /* Last button down on PM, either WM_BUTTON1DOWN
                                           or WM_BUTTON2DOWN or 0 if pointer is not over
                                           PM */
ULONG   ulShiftStatus;                  /* Status flag that traces the state of the SHIFT key,
                                           to be able to activate the Popup-Menu via the
                                           SHIFT+SHIFT keys
                                           76543210 (<-Bits)
                                                  | Right SHIFT was previously pressed
                                                 |  Left SHIFT was previously pressed */
ULONG   ulPopupKeyStatus;               /* Status flag for activating the Popup Menu from keyboard. This
                                           requires the user to press and immediately depress the key so
                                           that a make code is followed by the break code of the same key
                                           76543210 (<-Bits)
                                                  | As the key's make code is not followed by the key's break
                                                    code we can not accept this key */
ULONG   ulButton2Count;                 /* Status flag that traces the mouse button 2 clicks on
                                           KWM_CLIENT_CLASS windows (from IBM Communications Manager/2)
                                           to allow the events to be discarded. We must discard mouse
                                           button 2 events while Advanced Marking is active, because the
                                           first button down event activates the keypad popup, but the
                                           text is pasted from clipboard only after a double click, which
                                           causes the keypad popup window to flash shortly */
ULONG   ulKeyDataIndex;                 /* Array index */
ULONG   ulQuickSwitch;                  /* QuickSwitch activity indicator */
ULONG   ulLastWMCHARmp1;                /* Parameter mp1 of last WM_CHAR message */
ULONG   ulLastWMCHARmp2;                /* Parameter mp2 of last WM_CHAR message */
HWND    hwndActiveFrameWindow;          /* Window handle of active frame class window on Desktop */
HWND    hwndLastActiveFrameWindow;      /* Window handle of last active frame class window on Desktop */
HWND    hwndLastActiveFrameWindowOwner; /* Window handle of owner of last frame class window set active */
HWND    hwndFrame2Bottom;               /* Window handle of frame window user clicked mouse button 2 onto */
HWND    hwndLastActiveWindow;           /* The previous window the WM_MOUSEMOVE message was sent to (used
                                           by PC/2's SessionBar window to get hold of the mouse leaving event) */
HWND    hwndActiveWindow;               /* Window handle of active window we're processing an event */
HWND    hwndActiveWindowClient;         /* Window handle of client of active window */
HWND    hwndFocusSave;                  /* Window handle of active window's last active child control */
HWND    hwndActiveWindowOwner;          /* Window Handle of owner of active window */
HWND    hwndActiveWindowParent;         /* Window handle of parent of active window */
HWND    hwndTitlebar;                   /* Window handle of application's titlebar */
HWND    hwndActiveControl;              /* Window handle of active control user is setting the sliding focus */
HWND    hwndLastActiveControl;          /* Window handle of last active control the sliding focus was set */
HWND    hwndShieldParent;               /* Shield's (AVIO) frame window we are currently marking */
HWND    hwndCurrentSpyFrame;            /* Window handle of frame window we are spying */
HWND    hwndLastSpyFrame;               /* Window handle of last frame window we were spying */
HWND    hwndLastMenu;                   /* Window handle of last submenu selected for Dynamic MenuSelection */
HAB     habWindow;                      /* Anchor block cache */
POINTS  pointsCurrent;                  /* Mouse position of mouse relative to window from last
                                           WM_MOUSEMOVE message */
POINTS  pointsShieldMarking;            /* Pointer position on Shield (AVIO) window when marking was about to
                                           be started */
POINTL  pointlLast;                     /* Last position of mouse during WM_MOUSEMOVE */
POINTL  pointlMotionLast;               /* Last position to compare with current to detect MOUSEMOTION */
POINTL  pointlCurrent;                  /* Current position of mouse relative to Desktop from
                                           last WM_MOUSEMOVE message */
QUERYRECFROMRECT        QueryRect;      /* Rectangle to query underlaying containers */
KEYDATA *pKeyDataIndex;                 /* Pointer to KEYDATA structure in array */
                                        /* Classinfo of current window, to detect frame windows,
                                           because not all frame windows are of class WC_FRAME,
                                           e.g. EPM's frame is of class EFrame */
CLASSINFO   classinfoWindow;


#define         MAX_FRAME       128     /* Maximum number of frame windows we can subclass */
#define         FID_TEST        0x8020  /* User control window ID */

FRAMELIST   framelistWindows[MAX_FRAME];

/*--------------------------------------------------------------------------------------*\
 * This procedure initializes the data structures used by PC2.EXE and PC2HOOK.DLL.      *
 * Req:                                                                                 *
 *      none                                                                            *
 * Returns:                                                                             *
 *      ulResult ...... Return value                                                    *
\*--------------------------------------------------------------------------------------*/
ULONG EXPENTRY  PC2DLL_Initialize(void)
{
                                        /* Initialize to query the topmost underlaying
                                           container, that is partially hit by a rectangle
                                           around the pointer */
QueryRect.cb=sizeof(QUERYRECFROMRECT);
QueryRect.fsSearch=CMA_PARTIAL | CMA_ZORDER;
ulButtonDown=0;
ulQuickSwitch=0;
ulButton2Count=0;
ulLastWMCHARmp1=ulLastWMCHARmp2=0;
usFlags=usFlagsWinKeys=0;
usFlags=usFlagsCtrlKeys=0;
ulPopupKeyStatus=MAKEBREAKINVALID;
hwndShieldParent=NULLHANDLE;
memset(&pointsShieldMarking, 0, sizeof(pointsShieldMarking));
memset(framelistWindows, 0, sizeof(framelistWindows));
return(0);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure cleans up the data structures used by PC2.EXE and PC2HOOK.DLL.        *
 * DLL.                                                                                 *
 * Req:                                                                                 *
 *      none                                                                            *
 * Returns:                                                                             *
 *      ulResult ...... Window handle of the first window that has not removed its      *
 *                      subclassing yet                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG EXPENTRY  PC2DLL_Cleanup(void)
{
ULONG   ulIndex;
ULONG   ulResult;

ulResult=0;
for(ulIndex=0; ulIndex<MAX_FRAME; ulIndex++)
    if(framelistWindows[ulIndex].hwndFrame!=NULLHANDLE)
        {
        HWND    hwndFrame=framelistWindows[ulIndex].hwndFrame;
        HAB     habFrame=WinQueryAnchorBlock(hwndFrame);

                                        /* Some windows (windowed command processors from my experience)
                                           seem to get invalid, without the WinSendMsg() hook being able
                                           to "see" the WM_DESTROY */
        WinQueryClassName(hwndFrame, sizeof(ucClassname), ucClassname);
        WinQueryClassInfo(habFrame, ucClassname, &classinfoWindow);
                                        /* Ignore frame windows no longer existing */
        if((!WinIsWindow(habFrame, hwndFrame)) ||
            (!(classinfoWindow.flClassStyle & CS_FRAME)))
            {
            memset(&framelistWindows[ulIndex], 0, sizeof(framelistWindows[0]));
            continue;
            }
                                        /* Ignore windows that are now owned by HWND_OBJECT */
        if(WinQueryWindow(habFrame, QW_PARENT)==HWND_OBJECT)
            {
            memset(&framelistWindows[ulIndex], 0, sizeof(framelistWindows[0]));
            continue;
            }
        if(ulResult==0)
            ulResult=(ULONG)hwndFrame;
        WinBroadcastMsg(hwndFrame, WM_SYSCOMMAND,
            MPFROMSHORT(SC_CONTROLSCLEANUP), MPFROM2SHORT(CMDSRC_MENU, FALSE),
            BMSG_POST|BMSG_FRAMEONLY);
        }
return(ulResult);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure implements the hook of the input queue into the system queue.         *
 * Req:                                                                                 *
 *      hab ........... Anchor block handle                                             *
 *      pqmsg ......... Pointer to system QMSG structure                                *
 *      option ........ PM_REMOVE ..... message is being removed                        *
 *                      PM_NOREMOVE ... message is not being removed                    *
 * Returns:                                                                             *
 *      FALSE ......... OS/2 should process QMSG in the normal way passing to next hook *
 *                      or to the application                                           *
 *      TRUE .......... OS/2 doesn't pass the message to next hook or the application   *
\*--------------------------------------------------------------------------------------*/
BOOL EXPENTRY   PC2DLL_InputHook(HAB hab, PQMSG pqmsg, ULONG option)
{
if(pqmsg->msg==WM_SYSCOMMAND)
    {
    WinQueryClassName(pqmsg->hwnd, sizeof(ucClassname), ucClassname);
    WinQueryClassInfo(WinQueryAnchorBlock(pqmsg->hwnd), ucClassname, &classinfoWindow);
    if((!strcmp(ucClassname, "#1") || (classinfoWindow.flClassStyle & CS_FRAME)))
        {
        if((SHORT1FROMMP(pqmsg->mp1)==SC_CONTROLSINSTALL) &&
            (HookParameters.ulRuntimeFlag & SUBCLASSICONS))
            {
                                        /* Only register the frame window if it also consists of a
                                           titlebar control, otherwise it does not make much sense */
            if(WinWindowFromID(pqmsg->hwnd, FID_TITLEBAR))
                {
                                        /* If we haven't registered this frame window yet (which
                                           is very likely), register it (this message can be only
                                           caused by PC/2, but to be on the save side, we verify it) */
                if(LookupFrame(pqmsg->hwnd)==NULL)
                    if(RegisterFrame(pqmsg->hwnd, TRUE))
                        {
                        WinPostMsg(pqmsg->hwnd, WM_SYSCOMMAND,
                            MPFROMSHORT(SC_CONTROLSINITIALIZE), MPFROM2SHORT(CMDSRC_OTHER, FALSE));
                        }
                }
            }
        }
    }
/*                                                                                      *\
 * Here we catch any input activity that is detectable when the user does some mouse    *
 * or keyboard input. This indicates us, that the Lockup Timer should be reset. Only    *
 * when no user input is detected, the Lockup Timer is not reset, allowing eventually   *
 * to reach the user-selected Lockup Timer limit, activating Lockup.                    *
\*                                                                                      */
if((pqmsg->msg==WM_CHAR) || ((pqmsg->msg>=WM_BUTTONCLICKFIRST) && (pqmsg->msg<=WM_BUTTONCLICKLAST)))
    HookParameters.ulCurrentTimer=0;
if(pqmsg->msg==WM_MOUSEMOVE)
    {
                                        /* Deltas between the current mouse position and the
                                           previously save position (signed !) */
    long        lMouseDeltaX=0;
    long        lMouseDeltaY=0;

                                        /* Get current mouse position, relative to window and relative
                                           to Desktop */
    pointsCurrent=*((POINTS *)(&pqmsg->mp1));
    pointlCurrent=pqmsg->ptl;
/*                                                                                      *\
 * If mouse pointer is over the area covered by the SessionBar post message to working  *
 * thread to inform when the pointer has entered or left the area. Keep the             *
 * SessionBar's Popup Menus into account to (that is not removing the SessionBar while  *
 * a menu is displayed that covers also area outside the SessionBar or the user will    *
 * have a problem making selections). As the area can be reduced by using a HotSpot     *
 * that is smaller than the height of the SessionBar, we have to ensure that while the  *
 * pointer covers the SessionBar and not just its Popup Menus, that the SessionBar does *
 * not get hidden automatically.                                                        *
\*                                                                                      */
    if(HookParameters.ulStatusFlag2 & DYNAMICSESSIONBAR)
        {
        if(((HookParameters.ulSessionBarFlag & SB_LASTBOTTOM) &&
            (pointlCurrent.y<=(HookParameters.ulSessionBarHotSpot>>HookParameters.ulSessionBarScale))) ||
                                        /* Without HotSpot support
                                           (pointlCurrent.y<=HookParameters.swpPC2Session.cy)) ||
                                         */
            ((HookParameters.ulSessionBarFlag & SB_LASTTOP) &&
            (pointlCurrent.y>=(HookParameters.swpScreen.cy-(HookParameters.ulSessionBarHotSpot>>HookParameters.ulSessionBarScale)))))
                                        /* Without HotSpot support
                                           (pointlCurrent.y>=(HookParameters.swpScreen.cy-HookParameters.swpPC2Session.cy))))
                                         */
            {
            if(HookParameters.ulSessionBarFlag & SB_HIDDEN)
                {
                HookParameters.ulSessionBarFlag&=~SB_HIDDEN;
                WinPostMsg(HookParameters.hwndIOThread, WM_SESSIONBARUNHIDE,
                    MPFROMLONG(TRUE), NULL);
                }
            }
        else
            {
            if((!(HookParameters.ulSessionBarFlag & SB_HIDDEN)) &&
                (pqmsg->hwnd!=HookParameters.hwndSessionClient) &&
                (pqmsg->hwnd!=HookParameters.hwndPopupMenuSessionBar) &&
                (pqmsg->hwnd!=HookParameters.hwndPopupMenuProcessor) &&
                (pqmsg->hwnd!=HookParameters.hwndPopupSubMenuProcessor))
                {
                HookParameters.ulSessionBarFlag|=SB_HIDDEN;
                WinPostMsg(HookParameters.hwndIOThread, WM_SESSIONBARUNHIDE,
                    MPFROMLONG(FALSE), NULL);
                }
            }
        }
                                        /* Inform PC/2 windows that the mouse pointer was moved out of
                                           a window to another window. Beginning with Warp V4 someone
                                           has recognized that 2 messages are missing in PM, one to
                                           enter and one to leave a window, and IBM has added them.
                                           For compability with previous versions, we stay with our
                                           own handling */
    if(hwndLastActiveWindow!=pqmsg->hwnd)
        {
        WinPostMsg(HookParameters.hwndClient, WM_MOUSELEAVE,
            MPFROM2SHORT((SHORT)pqmsg->ptl.x, (SHORT)pqmsg->ptl.y), MPFROM2SHORT(HT_DISCARD, KC_NONE));
        }
                                        /* The current window will become the previous window the next time */
    hwndLastActiveWindow=pqmsg->hwnd;
                                        /* Calculate mouse position delta */
    lMouseDeltaX=pointlCurrent.x-pointlMotionLast.x;
    lMouseDeltaY=pointlCurrent.y-pointlMotionLast.y;
                                        /* Reset lockup counter and button 2 click counter
                                           only if mouse was moved at little. We have to test
                                           this, because sometimes WM_MOUSEMOVE messages are generated
                                           even noone moves the mouse pointer. */
    if((lMouseDeltaX < -MOUSEMOTION) || (lMouseDeltaX > MOUSEMOTION) ||
        (lMouseDeltaY < -MOUSEMOTION) || (lMouseDeltaY > MOUSEMOTION))
        {
        pointlMotionLast=pointlCurrent;
        HookParameters.ulCurrentTimer=0;
        ulButton2Count=0;
                                        /* If PC/2's Popup Menu was activated via the keyboard don't preselect
                                           a menuentry for the first time after displaying it to allow the user
                                           keyboard navigation */
        HookParameters.ulRuntimeFlag&=(~POPUPMENU4KEYBOARD);
        }
                                        /* Detect if mouse has been moved */
    if(memcmp(&pointlCurrent, &pointlLast, sizeof(pointlCurrent)))
        ulMoveFlag|=PTRPOSCHANGED;
    else
        ulMoveFlag&=~PTRPOSCHANGED;
                                        /* Save current pointer position */
    pointlLast=pointlCurrent;
    }
/*                                                                                      *\
 * Here we catch mouse button clicks on the PM to be able to send PC/2 a message when   *
 * it should display the window list, because the window list is only displayed if the  *
 * mouse is clicked on the WPS (not displaying the window list on PM).                  *
 * The window list is invoked (like on the WPS) when one mouse button is down, and the  *
 * other mouse button is pressed down too, but only if we are over PM.                  *
\*                                                                                      */
                                        /* If we click on one of the surrounding rows or
                                           columns used to switch between Virtual Desktops,
                                           don't start any processing regarding the input
                                           focus window */
if((pqmsg->msg==WM_BUTTON1DOWN) && (ulMoveFlag & (MOVEXR|MOVEXL|MOVEYD|MOVEYU)))
    goto SkipButton1FocusProcessing;
switch(pqmsg->msg)
{
                                        /* As our manipulation may call the input hook recursively
                                           again, we have to allocated our (access cache) variables
                                           on the stack */
HWND    hwndFocusWindow;
HWND    hwndFocusFrameWindow;

case WM_BUTTON1DOWN:
    {
                                        /* Cache access to active frame window */
    hwndFocusWindow=pqmsg->hwnd;
    hwndFocusFrameWindow=WinQueryActiveWindow(HWND_DESKTOP);
                                        /* If user clicked on the Desktop switch focus to Desktop */
    if(hwndFocusWindow==HookParameters.hwndDesktop)
        {
                                        /* Get the currently active top level window, so that
                                           PC/2 can switch back to this window, when the session
                                           started needed only be started in background. Be sure
                                           that the top level window gets saved, but not the WPS,
                                           because when the Popup-Menu gets activated with a double
                                           click, the first click switches the focus to the WPS,
                                           and the active window for the second click would then
                                           be the WPS (and activated afterwards which we want to
                                           avoid)  */
        if(hwndFocusFrameWindow!=HookParameters.hwndDesktop)
            HookParameters.hwndActiveFrameWindow=hwndFocusFrameWindow;
        WinFocusChange(HWND_DESKTOP, hwndFocusWindow, 0);
        }
                                        /* If user clicked on the WPS, ensure we keep the top level
                                           window stored, that had the focus before the WPS */
    if(hwndFocusWindow==HookParameters.hwndWPSClient)
        {
        if(hwndFocusFrameWindow!=WinQueryWindow(HookParameters.hwndDesktop, QW_PARENT))
            HookParameters.hwndActiveFrameWindow=hwndFocusFrameWindow;
        }
                                        /* If one button is down and other button was down before
                                           save it because window list requires second button
                                           down too */
    if(ulButtonDown!=WM_BUTTON2DOWN)
        ulButtonDown=WM_BUTTON1DOWN;
    else
        {
        ulButtonDown=WM_WINDOWLIST;
                                        /* If the user holds down both mouse buttons on PM display Window List */
        if(hwndFocusWindow==HookParameters.hwndDesktop)
            {
            WinPostMsg(HookParameters.hwndClient, WM_WINDOWLIST, MPFROMLONG(pqmsg->mp1), NULL);
            REROUTEMSGTONULL()
            }
        }
    if((HookParameters.ulStatusFlag2 & ADVANCEDMARKING) &&
        (!(WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000)))
        {
                                        /* Cache access to active window's class */
        WinQueryClassName(hwndFocusWindow, sizeof(ucClassname), ucClassname);
                                        /* If we're over a Shield (AVIO) class window, save the pointer
                                           position, because when we activate marking in WM_BUTTON1MOTIONSTART
                                           we may have already moved many pixels, but the user wants to mark at
                                           the position he pressed mouse button 1 not where we technically
                                           detect he is marking */
        if(!strcmp(ucClassname, SHIELD_AVIO))
            pointsShieldMarking=pointsCurrent;
                                        /* If the frame window the user clicked on is not the
                                           active frame window, activate it */
        if(hwndFocusFrameWindow!=hwndFocusWindow)
            {
                                        /* If we're over a PCSWS:Pres: window (from IBM Personal Communications
                                           /3270), ensure that PCM/3270 has the focus and simulate a mouse
                                           button 1 click to position the cursor to the mouse position (though
                                           sometimes it will by a row to low) */
            if(strstr(ucClassname, PCM_4XX))
                {
                WinSendMsg(hwndFocusWindow, WM_BUTTON1DOWN, pqmsg->mp1, pqmsg->mp2);
                WinSendMsg(hwndFocusWindow, WM_BUTTON1UP, pqmsg->mp1, pqmsg->mp2);
                WinSendMsg(hwndFocusWindow, WM_BUTTON1CLICK, pqmsg->mp1, pqmsg->mp2);
                }
                                        /* If we're over a Passport.c window (from Advantis PassPort/2),
                                           ensure that PassPort/2 has the focus. Unfortunately, if
                                           PassPort/2 already has the focus, the mouse pointer will
                                           be repositioned to the position where marking begins,
                                           so
            if(strstr(ucClassname, PP2_131))
                WinSetFocus(HWND_DESKTOP, hwndFocusWindow);
                                           this has to be disabled, even if that disallows Advanced
                                           Marking immediately after switching the focus to PassPort/2 */
            }
        }
                                        /* If the Sliding Focus and Preserve Z-Order is active, the user
                                           can't put a PM window into the foreground by clicking anywhere,
                                           so we have to catch the clicks and explicitely put it onto
                                           top of Z-Order */
    if((HookParameters.ulStatusFlag1 & SLIDINGFOCUS) && (HookParameters.ulStatusFlag1 & PRESERVEZORDER) &&
        (strcmp(ucClassname, SHIELD_SEAMLESS)))
        WinSetWindowPos(hwndFocusFrameWindow, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER);
    }
    break;

case WM_BUTTON2DOWN:
    {
                                        /* Cache access to active frame window */
    hwndFocusWindow=pqmsg->hwnd;
    hwndFocusFrameWindow=WinQueryActiveWindow(HWND_DESKTOP);
                                        /* If one button is down and other button was down before
                                           save it because window list requires second button
                                           down too */
    if(hwndFocusWindow==HookParameters.hwndDesktop)
        WinFocusChange(HWND_DESKTOP, hwndFocusWindow, 0);
    if(ulButtonDown!=WM_BUTTON1DOWN)
        ulButtonDown=WM_BUTTON2DOWN;
    else
        {
        ulButtonDown=WM_WINDOWLIST;
        if(hwndFocusWindow==HookParameters.hwndDesktop)
            {
            WinPostMsg(HookParameters.hwndClient, WM_WINDOWLIST, MPFROMLONG(pqmsg->mp1), NULL);
            REROUTEMSGTONULL()
            }
        }
    if(HookParameters.ulStatusFlag1 & BUTTON2MOVE)
        {
                                        /* Find out if user clicked over a frame window */
        hwndFocusWindow=pqmsg->hwnd;
        WinQueryClassName(hwndFocusWindow, sizeof(ucClassname), ucClassname);
        WinQueryClassInfo(WinQueryAnchorBlock(hwndFocusWindow), ucClassname, &classinfoWindow);
        if(!strcmp(ucClassname, "#1") || (classinfoWindow.flClassStyle & CS_FRAME))
            {
            SWP     swpActiveFrameWindow;
            LONG    lCXBorder;
            LONG    lCYBorder;

                                        /* Query the active window (which is a frame) position,
                                           to ensure that we request the window to be moved only
                                           when clicking on the border. Note: Dialog windows have
                                           no client window, so clicks on any are not covered by
                                           child controls would cause us to request the window move.
                                           Therefore we check for the click to be inside a sizeable
                                           border, even when the window has not this border at all,
                                           but I want use least CPU and perform movement even
                                           without a border */
            WinQueryWindowPos(hwndFocusWindow, &swpActiveFrameWindow);
            lCXBorder=WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
            lCYBorder=WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER);
                                        /* To simplify the comparism, we do to misuse the SWP
                                           structure so I must be used with care within this
                                           block scope */
            swpActiveFrameWindow.cx+=swpActiveFrameWindow.x;
            swpActiveFrameWindow.cy+=swpActiveFrameWindow.y;
            if(((pqmsg->ptl.x>=swpActiveFrameWindow.x) && (pqmsg->ptl.x<=swpActiveFrameWindow.x+lCXBorder)) ||
                ((pqmsg->ptl.y>=swpActiveFrameWindow.y) && (pqmsg->ptl.y<=swpActiveFrameWindow.y+lCYBorder)) ||
                ((pqmsg->ptl.x<=swpActiveFrameWindow.cx) && (pqmsg->ptl.x>=swpActiveFrameWindow.cx-lCXBorder)) ||
                ((pqmsg->ptl.y<=swpActiveFrameWindow.cy) && (pqmsg->ptl.y>=swpActiveFrameWindow.cy-lCYBorder)))
                {
                                        /* Set move pointer, as it is currently the size
                                           or arrow pointer */
                WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_MOVE, FALSE));
                                        /* Using WinSendMsg() instead would trap when moving
                                           PC/2's Overview window due to recursion */
                WinPostMsg(hwndFocusWindow, WM_TRACKFRAME, MPFROMSHORT(TF_MOVE), NULL);
                REROUTEMSGTONULL()
                }
            }
        }
                                        /* If the Sliding Focus and Preserve Z-Order is active, the user
                                           can't put a window into the foreground by clicking anywhere,
                                           so we have to catch the clicks and explicitely put it onto
                                           top of Z-Order */
    if((HookParameters.ulStatusFlag1 & SLIDINGFOCUS) && (HookParameters.ulStatusFlag1 & PRESERVEZORDER))
        WinSetWindowPos(hwndFocusFrameWindow, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER);
    }
    if(HookParameters.ulStatusFlag2 & ADVMARKINGBUTTON2)
        goto WM_BUTTONxDOWN;
    break;

case WM_BUTTON3DOWN:
    if(HookParameters.ulStatusFlag2 & ADVMARKINGBUTTON3)
        goto WM_BUTTONxDOWN;
    break;

WM_BUTTONxDOWN:
    if((HookParameters.ulStatusFlag2 & ADVANCEDMARKING) &&
        (!(WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000)))
        {
                                        /* Cache access to active window's class */
        WinQueryClassName(hwndFocusWindow, sizeof(ucClassname), ucClassname);
                                        /* If we're over a KWM_CLIENT_CLASS window (from IBM Communications
                                           Manager/2), ignore mouse button 2 events in order to avoid the
                                           popup keypad to be displayed for the mouse button 2 down events. */
        if(!strcmp(ucClassname, CM2_110))
            REROUTEMSGTONULL()
                                        /* If we're over a PCSWS:Pres: window (from IBM Personal Communications
                                           /3270), ignore mouse button 2 events in order to avoid the
                                           popup keypad to be displayed for the mouse button 2 down events. */
        if(strstr(ucClassname, PCM_4XX))
            REROUTEMSGTONULL()
                                        /* If we're over a Passport.C window (from Advantis PassPort/2),
                                           ignore mouse button 2 events in order to avoid the
                                           popup keypad to be displayed for the mouse button 2 down events. */
        if(strstr(ucClassname, PP2_131))
            REROUTEMSGTONULL()
                                        /* If we're over one of the EPM windows, ignore mouse button 2 events
                                           in order to avoid the popup menu to be displayed for the mouse
                                           button 2 up events */
        if((!strcmp(ucClassname, EPM_551)) || (!strcmp(ucClassname, EPM_603)))
            REROUTEMSGTONULL()
                                        /* If we're over a NotesRichText class window (from Lotus
                                           Notes), ignore mouse button 2 events in order to avoid a
                                           the WM_BUTTON2DOWN message processing, as the WM_BUTTON2UP
                                           messages will be removed to avoid the popup menu (not
                                           removing it won't display the popup menu as we remove the
                                           WM_BUTTON2UP event anyway, but with next WM_MOUSEMOVE a
                                           WM_BUTTON2MOTIONSTART would be generated) */
        if(!strncmp(ucClassname, NOTES_452, sizeof(NOTES_452)-1))
            REROUTEMSGTONULL()
        }
    break;

case WM_MOUSEMOVE:
    hwndActiveWindow=pqmsg->hwnd;
                                        /* If the mouse is not over PM reset window list flag */
    if(hwndActiveWindow!=HookParameters.hwndDesktop)
        ulButtonDown=0;
    break;

case WM_BUTTON1MOTIONSTART:
    {
    HWND    hwndFocusWindow;

    hwndFocusWindow=pqmsg->hwnd;
    if((HookParameters.ulStatusFlag2 & ADVANCEDMARKING) &&
        (!(WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000)))
        {
                                        /* Cache access to active window's class */
        WinQueryClassName(hwndFocusWindow, sizeof(ucClassname), ucClassname);
                                        /* If we're over a Shield (AVIO) class window, save
                                           the shield's frame window to aktivate marking with
                                           the mouse pointer motion until the motion ends, which
                                           will also copy the marked text */
        if(!strcmp(ucClassname, SHIELD_AVIO))
            {
            hwndShieldParent=WinQueryWindow(hwndFocusWindow, QW_PARENT);
                                        /* If this is the first mouse move on a Shield (AVIO) window
                                           while mouse button 1 was pressed, then activate the
                                           Shield's marking and reset Shield marking control */
            WinSendMsg(hwndFocusWindow, WM_BUTTON1UP,
                (MPARAM)(*((ULONG *)&pointsShieldMarking)), NULL);
            WinSendMsg(hwndShieldParent, WM_SYSCOMMAND,
                MPFROMLONG(SHIELD_MARK), MPFROM2SHORT(CMDSRC_MENU, TRUE));
            WinSendMsg(hwndFocusWindow, WM_BUTTON1DOWN,
                (MPARAM)(*((ULONG *)&pointsShieldMarking)), NULL);
            }
        }
    }
    break;

case WM_BUTTON1MOTIONEND:
    {
    HWND    hwndFocusWindow;

    hwndFocusWindow=pqmsg->hwnd;
    if((HookParameters.ulStatusFlag2 & ADVANCEDMARKING) &&
        (!(WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000)))
        {
                                        /* Cache access to active window's class */
        WinQueryClassName(hwndFocusWindow, sizeof(ucClassname), ucClassname);
                                        /* If the ALT key is pressed, don't paste into clipboard */
        if(!(WinGetKeyState(HWND_DESKTOP, VK_ALT) & 0x8000))
            {
            hwndFocusFrameWindow=WinQueryWindow(hwndFocusWindow, QW_PARENT);
                                        /* If we're over a Shield (AVIO) class window, set
                                           the Copy mode */
            if(!strcmp(ucClassname, SHIELD_AVIO))
                {
                if(hwndShieldParent!=NULLHANDLE)
                    WinSendMsg(hwndShieldParent, WM_SYSCOMMAND,
                        MPFROMLONG(SHIELD_COPY), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                }
                                        /* If we're over a EPM editor window, copy the selected
                                           text into the clipboard */
                                        /* Unfortunately EPM 6.03b (e.g. from Merlin) implements the 5.51
                                           identifiers again, but we can't differentiate between them, so
                                           we need to send both messages */
            if((!strcmp(ucClassname, EPM_551)) || (!strcmp(ucClassname, EPM_603)))
                {
                WinSendMsg(hwndFocusFrameWindow, WM_COMMAND,
                    MPFROMSHORT(EPM_551_COPY), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                WinSendMsg(hwndFocusFrameWindow, WM_COMMAND,
                    MPFROMSHORT(EPM_603_COPY), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                }
                                        /* If we're over a E window, copy the selected test into
                                           the clipboard */
            if(!strcmp(ucClassname, E_EDITOR))
                WinPostMsg(hwndFocusWindow, E_EDITOR_COPY, NULL, NULL);
                                        /* If we're over a WC_BUTTON class window, copy the
                                           text into the clipboard */
            if(!strcmp(ucClassname, "#3"))
                CopyToClipbrdData(hwndFocusWindow);
                                        /* If we're over a WC_STATIC class window, copy the
                                           text into the clipboard */
            if(!strcmp(ucClassname, "#5"))
                CopyToClipbrdData(hwndFocusWindow);
                                        /* If we're over a WC_ENTRYFIELD class window, copy
                                           the marked text into the clipboard */
            if(!strcmp(ucClassname, "#6"))
                WinPostMsg(hwndFocusWindow, EM_COPY, NULL, NULL);
                                        /* If we're over a WC_MLE class window, copy
                                           the marked text into the clipboard */
            if(!strcmp(ucClassname, "#10"))
                WinPostMsg(hwndFocusWindow, MLM_COPY, NULL, NULL);
                                        /* If we're over a KWM_CLIENT_CLASS window (from IBM Communications
                                           Manager/2), copy the marked text into the clipboard */
            if(!strcmp(ucClassname, CM2_110))
                {
                                        /* Does not work without this, I think a delay here does
                                           influence the message processing timing */
                DosSleep(25);
                WinPostMsg(hwndFocusFrameWindow, WM_COMMAND,
                    MPFROMSHORT(CM2_COPY), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                }
                                        /* If we're over a PCSWS:Pres: window (from IBM Personal
                                           Communications /3270), copy the marked text into the clipboard */
            if(strstr(ucClassname, PCM_4XX))
                WinPostMsg(hwndFocusFrameWindow, WM_COMMAND,
                    MPFROMSHORT(PCM_COPY), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                                        /* If we're over a Passport.C window (from Advantis PassPort/2)
                                           copy the marked text into the clipboard */
            if(strstr(ucClassname, PP2_131))
                WinPostMsg(hwndFocusFrameWindow, WM_COMMAND,
                    MPFROMSHORT(PP2_COPY), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                                        /* Check for Netscape Communicator/2 2.02 and copy the selected text
                                           into the clipboard. As WinQueryWindowText() seems to cause
                                           the ucClassname to be overwritten - likely due to some recursion -
                                           we have to check for NetScape here */
            WinQueryWindowText(WinWindowFromID(hwndFocusFrameWindow, FID_TITLEBAR),
                sizeof(ucWindowText), ucWindowText);
            if(!strncmp(ucWindowText, NETSCAPE_202, sizeof(NETSCAPE_202)-1))
                {
                                        /* Due to a bug/design flaw, Netscape does not empty the
                                           clipboard before copying into it, so that the Netscape
                                           data may add to previously copied data from a different
                                           application. This is considered bad PM programming practice,
                                           which we fix here */
                habWindow=WinQueryAnchorBlock(hwndFocusFrameWindow);
                WinOpenClipbrd(habWindow);
                WinEmptyClipbrd(habWindow);
                WinCloseClipbrd(habWindow);
                WinSendMsg(hwndFocusFrameWindow, WM_COMMAND,
                    MPFROMLONG(NETSCAPE_202_COPY), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                }
                                        /* Check for Netscape Communicator/2 4.04 and copy the selected text
                                           into the clipboard */
            if(!strncmp(ucClassname, NETSCAPE_404, sizeof(NETSCAPE_404)-1))
                {
                                        /* Good news, Netscape 4.04 no longer does not empty the
                                           clipboard before copying into it, so IBM has agreed that
                                           this is a bug in 2.02 ;-) */
                WinSendMsg(WinQueryWindow(hwndFocusFrameWindow, QW_PARENT),
                    WM_COMMAND,
                    MPFROMLONG(NETSCAPE_404_COPY), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                }
                                        /* If we're over a NotesRichText class window (from Lotus
                                           Notes), copy the marked text into the clipboard */
            if(!strncmp(ucClassname, NOTES_452, sizeof(NOTES_452)-1))
                {
                                        /* The Notes window hierachy is somewhat complex, not to say
                                           confusing/mad (mad may really be used as e.g. the un-
                                           highlighting the main frame's titlebar when clicking onto
                                           a menubar is bad PM programming practice in my eyes)

                                           Frame (child of HWND_DESKTOP)
                                           -> NOTES (FID_CLIENT)
                                              -> NOTES_MDICLIENT
                                                 -> Frame
                                                    -> NotesSubprog (FID_CLIENT)
                                                       -> NotesSplitter
                                                          -> Frame
                                                             -> NotesSubprog (FID_CLIENT)
                                                                -> NotesRichText (FID_CLIENT) */
                HWND    hwndNotesWindow=pqmsg->hwnd;
                HWND    hwndNotesWindowParent=WinQueryWindow(hwndNotesWindow, QW_PARENT);

                while((hwndNotesWindowParent!=HookParameters.hwndDesktop) &&
                    (hwndNotesWindowParent!=NULLHANDLE))
                    {                   /* Loop until we get the Desktop window handle. The
                                           previous child window of the Desktop is then the
                                           WC_FRAME class window of the point under the mouse
                                           pointer which is not the Desktop. */
                    hwndNotesWindow=hwndNotesWindowParent;
                    hwndNotesWindowParent=WinQueryWindow(hwndNotesWindow, QW_PARENT);
                    }
                WinPostMsg(hwndNotesWindow, WM_COMMAND,
                    MPFROMLONG(NOTES_COPY), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                }
            }
        else
            {
                                        /* If we're over a Shield (AVIO) class window, unset
                                           the Mark mode */
            if(!strcmp(ucClassname, SHIELD_AVIO))
                {
                if(hwndShieldParent!=NULLHANDLE)
                    WinSendMsg(hwndShieldParent, WM_SYSCOMMAND,
                        MPFROMLONG(SHIELD_MARK), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                }
            }
        }
                                        /* The end of a mouse motion always terminates marking (in
                                           Shield (AVIO) windows too) */
    hwndShieldParent=NULLHANDLE;
    }
    break;

case WM_BUTTON2DBLCLK:
    if(HookParameters.ulStatusFlag2 & ADVMARKINGBUTTON2)
        goto WM_BUTTONxDBLCLICK;
    break;

case WM_BUTTON3DBLCLK:
    if(HookParameters.ulStatusFlag2 & ADVMARKINGBUTTON3)
        goto WM_BUTTONxDBLCLICK;
    break;

WM_BUTTONxDBLCLICK:
    {
    HWND    hwndFocusWindow;

    hwndFocusWindow=pqmsg->hwnd;
                                        /* Cache access to active window's class */
    WinQueryClassName(hwndFocusWindow, sizeof(ucClassname), ucClassname);
                                        /* If we're WC_ENTRYFIELD class window, paste
                                           the marked text from the clipboard */
    if(!strcmp(ucClassname, "#6"))
        {
        FormatClipbrdData(hwndFocusWindow);
                                        /* The following is a workaround for copying into NetScape
                                           Communicator/2 4.04's "Go to:" entryfield (and seems not to
                                           harm other entryfields).
                                           If we were not doing that, NetScape would (recursively ?)
                                           paste the text again and again (until the entryfield is full
                                           or something in PMMERGE traps), I suspect this is do to that
                                           NetScape subclasses the entryfield to select all text during
                                           a WM_BUTTON1DOWN message (so that the next key replaces all
                                           text) */
        WinSendMsg(hwndFocusWindow, EM_SETSEL, MPFROM2SHORT(0, -1), NULL);
        WinSendMsg(hwndFocusWindow, EM_CLEAR, NULL, NULL);
        WinSendMsg(hwndFocusWindow, EM_PASTE, NULL, NULL);
        }
                                        /* If we're over a WC_MLE class window, paste
                                           the marked text from the clipboard */
    if(!strcmp(ucClassname, "#10"))
        {
        FormatClipbrdData(hwndFocusWindow);
        WinPostMsg(hwndFocusWindow, MLM_PASTE, NULL, NULL);
        }
                                        /* If we're over a Shield (AVIO) class window, set
                                           the Paste mode. Any terminating \r and/or \n will
                                           be removed from the clipboard, before the text
                                           is pasted, unless the CTRL key is pressed
                                           simultaneously. This makes sense, because a
                                           terminating \r\n executes the data entered as
                                           a command. */
    if(!strcmp(ucClassname, SHIELD_AVIO))
        {
        FormatClipbrdData(hwndFocusWindow);
                                    /* Now let the shield window paste the modified
                                       text from clipboard */
        WinPostMsg(WinQueryWindow(hwndFocusWindow, QW_PARENT), WM_SYSCOMMAND,
            MPFROMLONG(SHIELD_PASTE), MPFROM2SHORT(CMDSRC_MENU, TRUE));
        }
                                        /* If we're over a E window, paste the selected test into
                                           the clipboard */
    if(!strcmp(ucClassname, E_EDITOR))
        {
        FormatClipbrdData(hwndFocusWindow);
        WinPostMsg(hwndFocusWindow, E_EDITOR_PASTE, NULL, NULL);
        }
    }
    break;

case WM_BUTTON1UP:
                                        /* If the mouse is released reset window list flag */
    ulButtonDown=0;
    break;

case WM_BUTTON2UP:
                                        /* If the mouse is released reset window list flag */
    ulButtonDown=0;
    if(HookParameters.ulStatusFlag2 & ADVMARKINGBUTTON2)
        goto WM_BUTTONxUP;
    break;

case WM_BUTTON3UP:
    if(HookParameters.ulStatusFlag2 & ADVMARKINGBUTTON3)
        goto WM_BUTTONxUP;
    break;

WM_BUTTONxUP:
    {
    HWND    hwndFocusWindow;
    ULONG   ulEPMVersion=0;

    hwndFocusWindow=pqmsg->hwnd;
                                        /* Cache access to active window's class */
    WinQueryClassName(hwndFocusWindow, sizeof(ucClassname), ucClassname);
                                        /* If we're over a KWM_CLIENT_CLASS window (from IBM Communications
                                           Manager/2), we have to change the default mouse button 2 events
                                           in order to replace the keypad popup by the requirements of
                                           Advanced Marking */
    if(!strcmp(ucClassname, CM2_110))
        {
                                        /* We got a click */
        ulButton2Count++;
                                        /* Only the first click generates a mouse button 1 message to
                                           place the cursor at the current mouse position (mouse button
                                           1 clicks do this) */
        if(ulButton2Count==1)
            {
                                        /* If another window but CM/2 had the focus, the simulated click
                                           only changes the focus to the CM/2 window, but does not
                                           change the cursor position. So we activate the focus ourself
                                           to be on the save side */
            WinSetFocus(HWND_DESKTOP, hwndFocusWindow);
            WinSendMsg(hwndFocusWindow, WM_BUTTON1DOWN, pqmsg->mp1, pqmsg->mp2);
            WinSendMsg(hwndFocusWindow, WM_BUTTON1UP, pqmsg->mp1, pqmsg->mp2);
            WinSendMsg(hwndFocusWindow, WM_BUTTON1CLICK, pqmsg->mp1, pqmsg->mp2);
            }
        if(ulButton2Count==2)
            {
            FormatClipbrdData(hwndFocusWindow);
                                        /* Paste just displays the marker to put the text into */
            WinPostMsg(WinQueryWindow(hwndFocusWindow, QW_PARENT), WM_COMMAND,
                MPFROMSHORT(CM2_PASTE), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                                        /* Paste does only display the marker, now simulate a click
                                           to actually paste the text from clipboard into the marker */
            WinPostMsg(hwndFocusWindow, WM_BUTTON1DOWN, pqmsg->mp1, pqmsg->mp2);
            WinPostMsg(hwndFocusWindow, WM_BUTTON1UP, pqmsg->mp1, pqmsg->mp2);
            WinPostMsg(hwndFocusWindow, WM_BUTTON1CLICK, pqmsg->mp1, pqmsg->mp2);
                                        /* Reset button 2 counter */
            ulButton2Count=0;
            }
                                        /* During advanced marking we ignore mouse button 2 messages */
        REROUTEMSGTONULL()
        }
                                        /* If we're over a PCSWS:Pres: window (from IBM Personal Communications
                                           /3270), we have to change the default mouse button 2 events
                                           in order to replace the keypad popup by the requirements of
                                           Advanced Marking */
    if(strstr(ucClassname, PCM_4XX))
        {
                                        /* We got a click */
        ulButton2Count++;
                                        /* Only the first click generates a mouse button 1 message to
                                           place the cursor at the current mouse position (mouse button
                                           1 clicks do this) */
        if(ulButton2Count==1)
            {
                                        /* If another window but PCM/3270 had the focus, the simulated click
                                           only changes the focus to the PCM/3270 window, but does not
                                           change the cursor position. So we activate the focus ourself
                                           to be on the save side. (Again the cursor is usually places one row
                                           too low, unfortunately I have no idea why that happens, even positioning
                                           is by button 1 manually does the same, though it automagically gets
                                           corrected after a second) */
            WinSendMsg(hwndFocusWindow, WM_BUTTON1DOWN, pqmsg->mp1, pqmsg->mp2);
            WinSendMsg(hwndFocusWindow, WM_BUTTON1UP, pqmsg->mp1, pqmsg->mp2);
            WinSendMsg(hwndFocusWindow, WM_BUTTON1CLICK, pqmsg->mp1, pqmsg->mp2);
            }
        if(ulButton2Count==2)
            {
            FormatClipbrdData(hwndFocusWindow);
                                        /* Paste just displays the marker to put the text into */
            WinPostMsg(WinQueryWindow(hwndFocusWindow, QW_PARENT), WM_COMMAND,
                MPFROMSHORT(PCM_PASTE), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                                        /* Paste does only display the marker, now simulate a click
                                           to actually paste the text from clipboard into the marker */
            WinPostMsg(hwndFocusWindow, WM_BUTTON1DOWN, pqmsg->mp1, pqmsg->mp2);
            WinPostMsg(hwndFocusWindow, WM_BUTTON1UP, pqmsg->mp1, pqmsg->mp2);
            WinPostMsg(hwndFocusWindow, WM_BUTTON1CLICK, pqmsg->mp1, pqmsg->mp2);
                                        /* Reset button 2 counter */
            ulButton2Count=0;
            }
                                        /* During advanced marking we ignore mouse button 2 messages */
        REROUTEMSGTONULL()
        }
                                        /* If we're over a Passport.C window (from Advantis PassPort/2),
                                           we have to change the default mouse button 2 events
                                           in order to replace the keypad popup by the requirements of
                                           Advanced Marking */
    if(strstr(ucClassname, PP2_131))
        {
                                        /* We got a click */
        ulButton2Count++;
                                        /* Only the first click generates a mouse button 1 message to
                                           place the cursor at the current mouse position (mouse button
                                           1 clicks do this) */
        if(ulButton2Count==1)
            {
                                        /* If another window but PassPort/2 had the focus, the simulated click
                                           only changes the focus to the PassPort/2 window, but does not
                                           change the cursor position. So we activate the focus ourself
                                           to be on the save side */
            WinSetFocus(HWND_DESKTOP, hwndFocusWindow);
            WinSendMsg(hwndFocusWindow, WM_BUTTON1DOWN, pqmsg->mp1, pqmsg->mp2);
            WinSendMsg(hwndFocusWindow, WM_BUTTON1UP, pqmsg->mp1, pqmsg->mp2);
            WinSendMsg(hwndFocusWindow, WM_BUTTON1CLICK, pqmsg->mp1, pqmsg->mp2);
            }
                                        /* On the second click paste the text */
        if(ulButton2Count==2)
            {
            FormatClipbrdData(hwndFocusWindow);
            WinPostMsg(WinQueryWindow(hwndFocusWindow, QW_PARENT), WM_COMMAND,
                MPFROMSHORT(PP2_PASTE), MPFROM2SHORT(CMDSRC_MENU, TRUE));
                                        /* Reset button 2 counter */
            ulButton2Count=0;
            }
                                        /* During advanced marking we ignore mouse button 2 messages */
        REROUTEMSGTONULL()
        }
                                        /* If we're over a EPM editor window, paste the selected
                                           text from the clipboard. As we remove the mouse button 2 up
                                           messages from EPM, we have to track the count of the button
                                           2 clicks ourselves. As formatting the data may cause the hook
                                           to be called by another context, which may overwrite ucClassname
                                           save a local flag to be sure to compare for the same editor
                                           version after formatting clipboard data */
    ulEPMVersion=0;
    if(!strcmp(ucClassname, EPM_551))
        ulEPMVersion=EPM_VERSION_551;
    if(!strcmp(ucClassname, EPM_603))
        ulEPMVersion=EPM_VERSION_603;
    if(ulEPMVersion)
        {
                                        /* We got a click */
        ulButton2Count++;
                                        /* The first click gets also removed, so simulate a mouse button
                                           1 click to switch to the EPM window with the first moust button
                                           2 click too */
        if(ulButton2Count==1)
            {
            WinPostMsg(hwndFocusWindow, WM_BUTTON1DOWN, pqmsg->mp1, pqmsg->mp2);
            WinPostMsg(hwndFocusWindow, WM_BUTTON1UP, pqmsg->mp1, pqmsg->mp2);
            WinPostMsg(hwndFocusWindow, WM_BUTTON1CLICK, pqmsg->mp1, pqmsg->mp2);
            }
                                        /* On the second click paste the text */
        if(ulButton2Count==2)
            {
            FormatClipbrdData(hwndFocusWindow);
                                        /* Unfortunately EPM 6.03b (e.g. from Merlin) implements the 5.51
                                           identifiers again, but we can't differentiate between them, so
                                           we need to send both messages */
            WinPostMsg(WinQueryWindow(hwndFocusWindow, QW_PARENT), WM_COMMAND,
                MPFROMSHORT(EPM_603_PASTE), MPFROM2SHORT(CMDSRC_MENU, TRUE));
            WinPostMsg(WinQueryWindow(hwndFocusWindow, QW_PARENT), WM_COMMAND,
                MPFROMSHORT(EPM_551_PASTE), MPFROM2SHORT(CMDSRC_MENU, TRUE));
            ulButton2Count=0;
            }
        REROUTEMSGTONULL()
        }
                                        /* If we're over a NotesRichText class window (from Lotus
                                           Notes), copy the marked text into the clipboard */
    if(!strncmp(ucClassname, NOTES_452, sizeof(NOTES_452)-1))
        {
                                        /* We got a click */
        ulButton2Count++;
                                        /* The first click gets also removed, so simulate a mouse button
                                           1 click to switch to the Notes window with the first moust button
                                           2 click too */
        if(ulButton2Count==1)
            {
            WinPostMsg(hwndFocusWindow, WM_BUTTON1DOWN, pqmsg->mp1, pqmsg->mp2);
            WinPostMsg(hwndFocusWindow, WM_BUTTON1UP, pqmsg->mp1, pqmsg->mp2);
            WinPostMsg(hwndFocusWindow, WM_BUTTON1CLICK, pqmsg->mp1, pqmsg->mp2);
            }
                                        /* On the second click paste the text */
        if(ulButton2Count==2)
            {
            HWND    hwndNotesWindow=hwndFocusWindow;
            HWND    hwndNotesWindowParent=WinQueryWindow(hwndNotesWindow, QW_PARENT);

            FormatClipbrdData(hwndFocusWindow);
            while((hwndNotesWindowParent!=HookParameters.hwndDesktop) &&
                (hwndNotesWindowParent!=NULLHANDLE))
                {                       /* Loop until we get the Desktop window handle. The
                                           previous child window of the Desktop is then the
                                           WC_FRAME class window of the point under the mouse
                                           pointer which is not the Desktop. */
                hwndNotesWindow=hwndNotesWindowParent;
                hwndNotesWindowParent=WinQueryWindow(hwndNotesWindow, QW_PARENT);
                }
            WinPostMsg(hwndFocusWindow, WM_BUTTON2UP, pqmsg->mp1, pqmsg->mp2);
            WinPostMsg(hwndNotesWindow, WM_COMMAND,
                MPFROMLONG(NOTES_PASTE), MPFROM2SHORT(CMDSRC_MENU, TRUE));
            ulButton2Count=0;
            }
        REROUTEMSGTONULL()
        }
    }
    break;
}

/*                                                                                      *\
 * Here we catch all WM_BUTTON2DOWN messages. If it was clicked on any window's         *
 * titlebar then set this window to the bottom of the Desktop. WM_BUTTON2DOWN is used   *
 * to prevent the moving frame to be drawn (which is the default action of a            *
 * WM_BUTTON2DOWN message on a window's titlebar.                                       *
 * This function using WM_BUTTON2DBLCLK was first implemented by Robert Mahoney's       *
 * utiltiy WinBack, modified by Rolf Knebel to add this functionality to PC/2. However  *
 * using a doubleclick with mouse button 2 has the drawback that the window is brought  *
 * into the foreground first (because clicking on the titlebar activates a window) and  *
 * then moved to the bottom (causing many unnecessary drawings).                        *
\*                                                                                      */
if((pqmsg->msg==WM_BUTTON2DOWN) && (HookParameters.ulStatusFlag1 & BUTTON2ZORDER))
    {
                                        /* Parent window handle of the window we clicked on */
    hwndActiveWindowParent=WinQueryWindow(pqmsg->hwnd, QW_PARENT);
                                        /* If we click on a titlebar the current window handle
                                           equals the parent's titlebar window handle. If the
                                           ALT key is pressed we pass this message to the titlebar
                                           instead of processing to change its Z-Order */
    if((pqmsg->hwnd==WinWindowFromID(hwndActiveWindowParent, FID_TITLEBAR)) &&
        (!(WinGetKeyState(HWND_DESKTOP, VK_ALT) & 0x8000)) &&
        (!(HookParameters.ulRuntimeFlag & SYSTEMERRORACTIVE)))
        {
                                        /* Save frame window the user clicked on its titlebar to put it
                                           to bottom of Z-Order */
        hwndActiveWindowOwner=hwndActiveWindowParent;
                                        /* Query owner until there is no owner as we must move the
                                           owner to bottom of Z-Order not the ownee */
        do
            {
            hwndFrame2Bottom=hwndActiveWindowOwner;
            hwndActiveWindowOwner=WinQueryWindow(hwndFrame2Bottom, QW_OWNER);
            } while(hwndActiveWindowOwner!=NULLHANDLE);
                                        /* Now set the window (or its owner) to the bottom of Z-Order
                                           and request the main thread to put the now newly visible window
                                           below the mouse pointer to top of Z-Order */
        if(!HookParameters.hwndLockup)
            {
            WinSetWindowPos(hwndFrame2Bottom, HWND_BOTTOM, 0, 0, 0, 0, SWP_ZORDER|SWP_DEACTIVATE);
            WinSendMsg(HookParameters.hwndClient, WM_ZORDER, MPFROMHWND(hwndFrame2Bottom), NULL);
            }
                                        /* Remove message from titlebar processing in order to avoid
                                           the moving tracking rectangle to be drawn */
        REROUTEMSGTONULL()
        }
    }
/*                                                                                      *\
 * Here we catch mouse button 1 clicks, either the move the Desktop or to display the   *
 * Popup-Menu.                                                                          *
\*                                                                                      */
                                        /* Here we get when after skipping any input focus
                                           processing as the user clicked onto a surrounding
                                           row or columns to switch between Virtual Desktops */
SkipButton1FocusProcessing:
while(pqmsg->msg==HookParameters.ulClickFlag)
    {
/*                                                                                      *\
 * If the user clicked on at least one of the surrounding rows or columns of the        *
 * display, we shift the physical Desktop on the virtual Desktop. The flag MOVED4CLICK  *
 * is set, if the user click on the display borders.                                    *
\*                                                                                      */
    if(ulMoveFlag & MOVED4CLICK)
        {
        if((HookParameters.ulVirtualDesktopDelay!=0) && !(HookParameters.ulStatusFlag1 & CLICK2MOVE))
            {
            ulMoveFlag|=PENDINGSWITCH;
            WinPostMsg(HookParameters.hwndIOThread, WM_VIRTUALDESKTOPDELAY,
                MPFROM2SHORT(pqmsg->ptl.x, pqmsg->ptl.y), MPFROMLONG(ulMoveFlag));
            }
        else
            WinPostMsg(HookParameters.hwndClient, WM_MOVEREQUEST,
                MPFROM2SHORT(pqmsg->ptl.x, pqmsg->ptl.y), MPFROMLONG(ulMoveFlag));
        ulMoveFlag&=~MOVED4CLICK;       /* Reset flag, because only a move before
                                           a click may set it */
        REROUTEMSGTONULL()
        }
/*                                                                                      *\
 * If the user clicked on the WPS or PM window, send PC/2 a message to display the      *
 * Popup-Menu, however not if stroke was part of the Window List activation.            *
\*                                                                                      */
    if((pqmsg->hwnd==HookParameters.hwndWPSClient) && (ulButtonDown!=WM_WINDOWLIST))
        {                               /* The user clicked on WPS "Desktop" window.
                                           We construct a small rectangle around the
                                           current position of the pointer, relative to
                                           the WPS window (not the screen) */
        QueryRect.rect.xLeft=(ULONG)SHORT1FROMMP(pqmsg->mp1);
        QueryRect.rect.xRight=QueryRect.rect.xLeft+1;
        QueryRect.rect.yBottom=(ULONG)SHORT2FROMMP(pqmsg->mp1);
        QueryRect.rect.yTop=QueryRect.rect.yBottom+1;
        if(WinSendMsg(HookParameters.hwndWPSClient, CM_QUERYRECORDFROMRECT,
            MPFROMLONG(CMA_FIRST), &QueryRect)==NULL)
                                        /* If no container is under the rectangle of the
                                           mouse pointer, we can display our Popup-Menu.
                                           The type of container is unknown, but because
                                           we test only on the WPS, they should usually
                                           be the icons (but not the minimized programs,
                                           which are windows with a different window handle). */
                                        /* Pass the pointer position in coordinates relative
                                           to the screen (not WPS window) and the handle of
                                           that window. */
            {
            WinPostMsg(HookParameters.hwndClient, WM_POPUPMENU,
                MPFROM2SHORT((USHORT)pqmsg->ptl.x, (USHORT)pqmsg->ptl.y), MPFROMHWND(pqmsg->hwnd));
                                        /* Force that sliding focus makes an update of focus window,
                                           otherwise PC/2 treats the last being the active one, even
                                           if the Popup-Menu gained the focus */
            hwndLastActiveFrameWindow=NULLHANDLE;
            hwndLastActiveControl=NULLHANDLE;
            REROUTEMSGTONULL()
            }
        break;                          /* If clicked on an container, pass message to WPS */
        }
    if((pqmsg->hwnd==HookParameters.hwndDesktop) &&
        (ulButtonDown!=WM_WINDOWLIST))
        {                               /* The user clicked on the PM "Desktop" window.
                                           If the WPS isn't installed we only get the PM
                                           windows. We can now display our Popup-Menu.
                                           Pass the pointer position in coordinates relative
                                           to the screen (not WPS window) and the handle of
                                           that window. */
        WinPostMsg(HookParameters.hwndClient, WM_POPUPMENU,
            MPFROM2SHORT((USHORT)pqmsg->ptl.x, (USHORT)pqmsg->ptl.y), MPFROMHWND(pqmsg->hwnd));
                                        /* Force that sliding focus makes an update of focus window */
        hwndLastActiveFrameWindow=NULLHANDLE;
        hwndLastActiveControl=NULLHANDLE;
        REROUTEMSGTONULL()
        }
    if(HookParameters.ulStatusFlag1 & POPUPSYSTEMWIDE)
        {                               /* If the user enabled Popup-Menu activation key
                                           detection over any window on PM, request Popup-Menu
                                           too. This makes of course only sense when the Popup-Menu
                                           gets activated by the third mouse button, because otherwise
                                           you would degrade system usablility. */
        WinPostMsg(HookParameters.hwndClient, WM_POPUPMENU,
            MPFROM2SHORT((USHORT)pqmsg->ptl.x, (USHORT)pqmsg->ptl.y), MPFROMHWND(pqmsg->hwnd));
                                        /* Force that sliding focus makes an update of focus window */
        hwndLastActiveFrameWindow=NULLHANDLE;
        hwndLastActiveControl=NULLHANDLE;
                                        /* When skipping this message Communicator/2 loops with
                                           WM_BUTTON3CLICK|DBLCLK messages. Removing seems not to
                                           break other applications.
        REROUTEMSGTONULL()
                                         */
        }
    break;                              /* Break out of while loop */
    }
                                        /* Return if mouse is captured */
if(WinQueryCapture(HWND_DESKTOP)!=NULLHANDLE)
    return(FALSE);
/*                                                                                      *\
 * If enabled, here we catch all mouse movements, to set the window under the mouse     *
 * pointer as the active one, if it isn't currently active or the window list or        *
 * optionally the Desktop window. If enabled too, we also make sure that the menuentry  *
 * just below the mouse pointer is the selected one (as Win95 does for its taskbar),    *
 * this does no longer require you to click menuentries that are submenus.              *
\*                                                                                      */
if((pqmsg->msg==WM_MOUSEMOVE) || (pqmsg->msg==WM_MOUSESELECT))
    {
                                        /* WM_MOUSESELECT will be posted during delayed Dynamic
                                           Menuselection (but should not cause any harm) */
    WinQueryClassName(pqmsg->hwnd, sizeof(ucClassname), ucClassname);
                                        /* Let's see if we are above a menu class window and the
                                           user wants us to select the menuentry below the mouse
                                           pointer */
    if((!strcmp(ucClassname, "#4")) && (HookParameters.ulStatusFlag2 & DYNAMICMENU))
        {
                                        /* Current menu cached */
        static  HWND        hwndCurrentMenu=NULLHANDLE;
                                        /* Menuentry identity that was selected during last run */
        static  SHORT       sLastItemIdentity=MIT_NONE;
                                        /* Menuentry identity that should be selected this run */
        static  SHORT       sCurrentItemIdentity=MIT_NONE;
                                        /* Current menuentry's menuitem structure */
        static  MENUITEM    menuitemCurrent;
                                        /* Flag set if we're processing a WM_MOUSEMOVE message posted
                                           by the PC/2 Working Thread when the delayed Dynamic Menuselection
                                           has expired */
        static  ULONG       ulDynamicMenuDelayed=FALSE;

                                        /* Normally WM_MOUSEMOVE messages will have a HT_* parameter
                                           here, but when it is HT_DYNAMICMENUDELAY, we know PC/2's
                                           working thread has posted a WM_MENUSELECT for delayed Dynamic
                                           Menuselection */
        ulDynamicMenuDelayed=FALSE;
        if(SHORT1FROMMP(pqmsg->mp2)==HT_DYNAMICMENUDELAY)
            ulDynamicMenuDelayed=TRUE;
                                        /* Only ensure to select the menuentry below the mouse pointer if
                                           already one menuentry is selected. For the first menuentry of PC/2's
                                           Popup Menu don't use Dynamic Menuselection to allow the user
                                           keyboard navigation */
        if(((LONG)WinSendMsg(pqmsg->hwnd, MM_QUERYSELITEMID, MPFROMLONG((LONG)FALSE), NULL)!=MIT_NONE) &&
            (!(HookParameters.ulRuntimeFlag & POPUPMENU4KEYBOARD)))
            {
                                        /* Number of items in current menu */
            SHORT   sItemCount;
                                        /* Loop counter wihtin current menu */
            SHORT   sItemIndex;
                                        /* Rectangle that current menuentry occupies within current menu */
            RECTL   rectlItem;
            POINTL  ptlMouse;
                                        /* Cache submenu handle, as we reuse the MENUITEM strucutre */
            HWND    hwndSubMenu;
                                        /* Identity of first menuentry of submenu, which we will select */
            SHORT   sItemIdentity;

                                        /* Cache current menu's window handle */
            hwndCurrentMenu=pqmsg->hwnd;
                                        /* Check for keyboard based Dynamic Menuselection. I had
                                           to split that from the mouse based code, as I got unwanted
                                           effects e.g. uninitialized variables, due to using gotos
                                           to optimize code */
            if(SHORT1FROMMP(pqmsg->mp2)==HT_DYNAMICMENUKBD)
                {
                ptlMouse.x=SHORT1FROMMP(pqmsg->mp1);
                ptlMouse.y=SHORT2FROMMP(pqmsg->mp1);
                sCurrentItemIdentity=SHORT2FROMMP(pqmsg->mp2);
                ulDynamicMenuDelayed=FALSE;
                                        /* Query the current menuentry's menuitem structure */
                WinSendMsg(hwndCurrentMenu, MM_QUERYITEM,
                    MPFROM2SHORT(sCurrentItemIdentity, FALSE), MPFROMP(&menuitemCurrent));
                if(sLastItemIdentity!=sCurrentItemIdentity)
                    {
                    if(HookParameters.ulDynamicMenuDelay!=0)
                        {
                                        /* When the user has selected having a delay for activation
                                           of Dynamic Menuselection, then inform the Working
                                           thread to restart the counter */
                        WinPostMsg(HookParameters.hwndIOThread, WM_DYNAMICMENUDELAY,
                            MPFROMHWND(hwndCurrentMenu), pqmsg->mp1);
                        }
                    else
                        {
                                        /* Select the menuentry just below the mouse pointer. If the
                                           item is a submenu, then the first item is selected too
                                           (unfortunately!, which displays conditionally cascaded
                                           submenus and I found no way to prevent/undo this except
                                           for not selecting such submenus at all, which would be somewhat
                                           inconsistent for the user) */
                        WinSendMsg(hwndCurrentMenu, MM_SELECTITEM,
                            MPFROM2SHORT(sCurrentItemIdentity, FALSE), MPFROM2SHORT(0, FALSE));
                                        /* If the menuentry is a submenu, then we may have to select
                                           the first menuentry of the submenu too */
                        while(menuitemCurrent.afStyle & MIS_SUBMENU)
                            {
                            static ULONG    ulQuerySuccess;

                                        /* Get the identity of the first menuentry in the submenu.
                                           Exit iteration when an error is returned (e.g. empty
                                           submenu) */
                            hwndLastMenu=hwndSubMenu=menuitemCurrent.hwndSubMenu;
                            sItemIdentity=(LONG)WinSendMsg(hwndSubMenu, MM_ITEMIDFROMPOSITION,
                                MPFROMSHORT(0), NULL);
                            if(sItemIdentity==MIT_ERROR)
                                break;
                                        /* Get the first menuentry of the current submenu, to
                                           again select its first menuentry recursively */
                            ulQuerySuccess=(ULONG)WinSendMsg(hwndSubMenu, MM_QUERYITEM,
                                MPFROM2SHORT(sItemIdentity, FALSE), MPFROMP(&menuitemCurrent));
                            hwndLastMenu=menuitemCurrent.hwndSubMenu;
                                        /* Select the submenu's first menuentry */
                            WinSendMsg(hwndSubMenu, MM_SELECTITEM,
                                MPFROM2SHORT(sItemIdentity, FALSE), MPFROM2SHORT(0, FALSE));
                                        /* If the user selected that we should not recursively cascade
                                           further into submenus, then simply don't do that. Unfortunately
                                           sometimes 2 levels are selected, this is a unexplainable PM
                                           behaviour for me */
                            if((!(HookParameters.ulStatusFlag2 & CASCADEMENU)) || (ulQuerySuccess==FALSE))
                                break;
                            }
                        }
                                        /* Save our the menuentry that is selected within the current
                                           menu now, and break out of the loop (because we've found what
                                           we have been looking for) */
                    sLastItemIdentity=sCurrentItemIdentity;
                    }
                goto DYNAMIC_MENUSELECTION_EXIT;
                }
                                        /* Map the current mouse position from the screen
                                           coordinated to the menu coordinates */
            ptlMouse.x=(ULONG)SHORT1FROMMP(pqmsg->mp1);
            ptlMouse.y=(ULONG)SHORT2FROMMP(pqmsg->mp1);
                                        /* Loop through all items in the current menu,
                                           and query the item's rectangle. As we don't know the
                                           number of items in the menu, we break out if there
                                           is no more valid item */
            sItemCount=(BOOL)WinSendMsg(hwndCurrentMenu, MM_QUERYITEMCOUNT,
                NULL, NULL);
            for(sItemIndex=0; sItemIndex<sItemCount; sItemIndex++)
                {
                                        /* Let's see if we have another item available. Continue loop
                                           if there is something peculiar */
                sCurrentItemIdentity=(LONG)WinSendMsg(hwndCurrentMenu, MM_ITEMIDFROMPOSITION,
                    MPFROMSHORT(sItemIndex), NULL);
                                        /* As MIT_ERROR=MIT_END=MIT_NONE=MIT_MEMERROR and may also be
                                           used for separators, just ignore this menuentries */
                if(sCurrentItemIdentity==MIT_ERROR) continue;
                                        /* Get the menuentry's rectangle to test if it covers the
                                           current mouse pointer position */
                WinSendMsg(hwndCurrentMenu, MM_QUERYITEMRECT,
                    MPFROM2SHORT(sCurrentItemIdentity, FALSE), MPFROMP(&rectlItem));
                                        /* If the item's rectangle covers the pointer position,
                                           select this menuentry and exit our loop as our job
                                           is done */
                if((ptlMouse.x>rectlItem.xLeft) && (ptlMouse.x<=rectlItem.xRight) &&
                    (ptlMouse.y>rectlItem.yBottom) && (ptlMouse.y<=rectlItem.yTop))
                    {
DYNAMIC_MENUSELECTION_KEYBOARD_SIMULATION:
                                        /* Query the current menuentry's menuitem structure */
                    WinSendMsg(hwndCurrentMenu, MM_QUERYITEM,
                        MPFROM2SHORT(sCurrentItemIdentity, FALSE), MPFROMP(&menuitemCurrent));
                                        /* If the message was posted by PC/2's Working thread,
                                           then the delay has expired and we can select the
                                           menuentry/submenu */
                    if(ulDynamicMenuDelayed==TRUE)
                        {
                                        /* A goto is tolerable here as we want to save both code
                                           size and speed (because slowing down hook processing
                                           slows down the whole GUI processing) */
                        goto DYNAMIC_MENUSELECTION_SELECT_SUBMENU;
                        }
                                        /* Reduce work to the only required one */
                    else if(sLastItemIdentity!=sCurrentItemIdentity)
                        {
                        if(HookParameters.ulDynamicMenuDelay!=0)
                            {
                                        /* When the user has selected having a delay for activation
                                           of Dynamic Menuselection, then inform the Working
                                           thread to restart the counter */
                            WinPostMsg(HookParameters.hwndIOThread, WM_DYNAMICMENUDELAY,
                                MPFROMHWND(hwndCurrentMenu), pqmsg->mp1);
                            }
                        else
                            {
DYNAMIC_MENUSELECTION_SELECT_SUBMENU:
                                        /* Select the menuentry just below the mouse pointer. If the
                                           item is a submenu, then the first item is selected too
                                           (unfortunately!, which displays conditionally cascaded
                                           submenus and I found no way to prevent/undo this except
                                           for not selecting such submenus at all, which would be somewhat
                                           inconsistent for the user) */
                            WinSendMsg(hwndCurrentMenu, MM_SELECTITEM,
                                MPFROM2SHORT(sCurrentItemIdentity, FALSE), MPFROM2SHORT(0, FALSE));
                                        /* If the menuentry is a submenu, then we may have to select
                                           the first menuentry of the submenu too */
                            while(menuitemCurrent.afStyle & MIS_SUBMENU)
                                {
                                static ULONG    ulQuerySuccess;

                                        /* Get the identity of the first menuentry in the submenu.
                                           Exit iteration when an error is returned (e.g. empty
                                           submenu) */
                                hwndLastMenu=hwndSubMenu=menuitemCurrent.hwndSubMenu;
                                sItemIdentity=(LONG)WinSendMsg(hwndSubMenu, MM_ITEMIDFROMPOSITION,
                                    MPFROMSHORT(0), NULL);
                                if(sItemIdentity==MIT_ERROR)
                                    break;
                                        /* Get the first menuentry of the current submenu, to
                                           again select its first menuentry recursively */
                                ulQuerySuccess=(ULONG)WinSendMsg(hwndSubMenu, MM_QUERYITEM,
                                    MPFROM2SHORT(sItemIdentity, FALSE), MPFROMP(&menuitemCurrent));
                                hwndLastMenu=menuitemCurrent.hwndSubMenu;
                                        /* Select the submenu's first menuentry */
                                WinSendMsg(hwndSubMenu, MM_SELECTITEM,
                                    MPFROM2SHORT(sItemIdentity, FALSE), MPFROM2SHORT(0, FALSE));
                                        /* If the user selected that we should not recursively cascade
                                           further into submenus, then simply don't do that. Unfortunately
                                           sometimes 2 levels are selected, this is a unexplainable PM
                                           behaviour for me */
                                if((!(HookParameters.ulStatusFlag2 & CASCADEMENU)) || (ulQuerySuccess==FALSE))
                                    break;
                                }
                            }
                                        /* Save our the menuentry that is selected within the current
                                           menu now, and break out of the loop (because we've found what
                                           we have been looking for) */
                        sLastItemIdentity=sCurrentItemIdentity;
                        break;
                        }
                    else
                        break;
                    }
                }
            }
        else
                                        /* Reset to initialize dynamic menuselection for the next
                                           time we will be using it */
            sLastItemIdentity=MIT_NONE;
        }
DYNAMIC_MENUSELECTION_EXIT:
                                        /* If we are going to spy, see if the frame window below
                                           the mouse pointer has changed, and update our spy window
                                           accordingly */
    while(HookParameters.ulRuntimeFlag & SPYFRAMEWINDOW)
        {
        PTIB    ptibFrame;
        PPIB    ppibFrame;
        HWND    hwndCurrentSpyFrameParent;

                                        /* Find the frame window for the window below the mouse
                                           pointer by getting up the parent chain until the parent
                                           is the Desktop */
        hwndCurrentSpyFrame=pqmsg->hwnd;
        hwndCurrentSpyFrameParent=WinQueryWindow(hwndCurrentSpyFrame, QW_PARENT);
        while((hwndCurrentSpyFrameParent!=HookParameters.hwndDesktop) &&
            (hwndCurrentSpyFrameParent!=NULLHANDLE))
            {
            hwndCurrentSpyFrame=hwndCurrentSpyFrameParent;
            hwndCurrentSpyFrameParent=WinQueryWindow(hwndCurrentSpyFrame, QW_PARENT);
            }
        if(hwndLastSpyFrame!=hwndCurrentSpyFrame)
            {
                                        /* If the frame window under the mouse pointer is a different
                                           one than the last mouse move, query the window's data */
            memset(&spyframeLast, 0, sizeof(spyframeLast));
            spyframeCurrent.hwndFrame=hwndCurrentSpyFrame;
            WinQueryWindowText(hwndCurrentSpyFrame, sizeof(spyframeCurrent.ucWindowText), spyframeCurrent.ucWindowText);
            WinQueryClassName(hwndCurrentSpyFrame, sizeof(spyframeCurrent.ucWindowClass), spyframeCurrent.ucWindowClass);
            DosGetInfoBlocks(&ptibFrame, &ppibFrame);
            strcpy(spyframeCurrent.ucExecutable, ppibFrame->pib_pchcmd);
            spyframeCurrent.pidWindow=ppibFrame->pib_ulpid;
            spyframeCurrent.tidWindow=ptibFrame->tib_ptib2->tib2_ultid;
            memcpy(&spyframeLast, &spyframeCurrent, sizeof(spyframeLast));
            WinPostMsg(HookParameters.hwndWorkingThread, WM_SPYFRAMEWINDOW, MPFROMP(&spyframeLast), NULL);
                                        /* And remember the new frame window we are spying */
            hwndLastSpyFrame=hwndCurrentSpyFrame;
            }
        break;
        }
    while((HookParameters.ulStatusFlag1 & SLIDINGFOCUS) && !(HookParameters.ulRuntimeFlag & SYSTEMERRORACTIVE))
        {
                                        /* Flag set if we're processing a WM_MOUSEMOVE message posted
                                           by the PC/2 Working Thread when the delayed Sliding Focus
                                           has expired */
        static  ULONG       ulSlidingFocusDelayed=FALSE;
                                        /* Flag set when the Sliding Focus should be processed immediately */
        static  ULONG       ulSlidingFocusNow=FALSE;

                                        /* If enabled, use sliding focus to activate window
                                           under the mouse pointer (with some exceptions).
                                           Caution! Menus have a class WC_MENU, but their
                                           parent is not the frame window WC_FRAME but the
                                           Desktop itself. */
                                        /* Don't process message as possible focus change for PC/2,
                                           if SHIFT is pressed */
        if((WinGetKeyState(HWND_DESKTOP, VK_SHIFT) & 0x8000))
            return(FALSE);
                                        /* Normally WM_MOUSEMOVE messages will have a HT_* parameter
                                           here, but when it is HT_SLIDINGFOCUSDELAY, we know the Working
                                           thread has posted a message for delayed Sliding Focus, however,
                                           for this to occur, there must be a delay configured */
        ulSlidingFocusDelayed=ulSlidingFocusNow=FALSE;
        if((LONG)SHORT1FROMMP(pqmsg->mp2)==(LONG)HT_SLIDINGFOCUSDELAY)
            {
            ulSlidingFocusDelayed=TRUE;
            ulSlidingFocusNow=TRUE;
            }
        if(HookParameters.ulSlidingFocusDelay==0)
            ulSlidingFocusNow=TRUE;
                                        /* If the mouse pointer is moving over the same window
                                           as for the last move, we don't need to do anything */
        if((hwndLastActiveControl==(hwndActiveWindow=hwndActiveControl=pqmsg->hwnd)) &&
            (ulSlidingFocusDelayed==FALSE))
            break;
                                        /* Query the currently active window, where HWND_DESKTOP
                                           is the parent window. It will be a WC_FRAME class
                                           window */
        hwndActiveFrameWindow=WinQueryActiveWindow(HWND_DESKTOP);
                                        /* Don't switch away from Window List */
        if(hwndActiveFrameWindow==HookParameters.hwndWindowList) break;
                                        /* If the window under the mouse pointer is one of the
                                           Desktops, don't do any changes */
        if((hwndActiveWindow==HookParameters.hwndDesktop) || (hwndActiveWindow==HookParameters.hwndWPSClient))
            break;
                                        /* Get the frame window being the parent of the control under
                                           the mouse pointer, by chaining up the parent order until
                                           the Desktop is the parent, because frame windows are childs
                                           of the Desktop */
        hwndActiveWindowParent=WinQueryWindow(hwndActiveWindow, QW_PARENT);
        while((hwndActiveWindowParent!=HookParameters.hwndDesktop) &&
            (hwndActiveWindowParent!=NULLHANDLE))
            {                           /* Loop until we get the Desktop window handle. The
                                           previous child window of the Desktop is then the
                                           WC_FRAME class window of the point under the mouse
                                           pointer which is not the Desktop. */
            hwndActiveWindow=hwndActiveWindowParent;
            hwndActiveWindowParent=WinQueryWindow(hwndActiveWindow, QW_PARENT);
            }
                                        /* If the window going to be activated is the SessionBar,
                                           don't do that, because the SessionBar would immediately
                                           switch the input focus further to the next lower window
                                           in Z-Order, which may cause funny effects */
        if(hwndActiveWindow==HookParameters.hwndSessionFrame)
            break;
                            /* If the message was posted by PC/2's Working thread,
                               then the delay has expired and we can select the
                               menuentry/submenu */
        if(ulSlidingFocusDelayed==TRUE)
            {
                            /* A goto is tolerable here as we want to save both code
                               size and speed (because slowing down hook processing
                               slows down the whole GUI processing) */
            goto SLIDING_FOCUS_FRAMEWINDOW_PERFORM;
            }
        if(hwndLastActiveFrameWindow!=hwndActiveWindow)
            {
                                        /* Find out some more details about the application window,
                                           we are going to switch to. Sort with expected descending
                                           probability, to avoid unnecessary cpu load */
                                        /* Query the class of the frame window of the
                                           designated target of WM_MOUSEMOVE */
            WinQueryClassName(hwndActiveWindow, sizeof(ucClassname), ucClassname);
                                        /* Don't switch to a menu class window */
            if(!strcmp(ucClassname, "#4")) return(FALSE);
                                        /* Don't switch to a combobox's listbox window */
            if(!strcmp(ucClassname, "#7")) return(FALSE);
                                        /* Query the frame window name of the designated
                                           target of WM_MOUSEMOVE */
            WinQueryWindowText(hwndActiveWindow, sizeof(ucWindowText), ucWindowText);
                                        /* Don't switch to seamless WIN-OS2 menus */
            if(strstr(ucWindowText, "Seamless")) return(FALSE);
                                        /* If current frame is not equal than last frame we have
                                           to activate current frame window */
                                        /* Don't switch to the WC_FRAME class window of PC/2 */
            if(strstr(ucWindowText, "PC/2")) return(FALSE);
                                        /* Get the client window of the application window below the
                                           mouse pointer, or NULLHANDLE if no client exists (frame
                                           windows created from dialog templates) */
            hwndActiveWindowClient=WinWindowFromID(hwndActiveWindow, FID_CLIENT);
            goto SLIDING_FOCUS_FRAMEWINDOW_PERFORM_SKIP_NOW;
SLIDING_FOCUS_FRAMEWINDOW_PERFORM:
            WinQueryClassName(pqmsg->hwnd, sizeof(ucClassname), ucClassname);
            if(!strcmp(ucClassname, "#4"))
                {
                hwndLastActiveControl=hwndActiveControl=pqmsg->hwnd;
                return(FALSE);
                }
SLIDING_FOCUS_FRAMEWINDOW_PERFORM_SKIP_NOW:
                                        /* When the selected delay of the Sliding Focus is 0, then
                                           the focus change is done here in the hook, otherwise a
                                           message is posted to the Working Thread, which when the
                                           counter has expired performs the focus change */
            if(HookParameters.ulStatusFlag1 & PRESERVEZORDER)
                {
                                        /* Change focus, but preserve Z-order */
                hwndFocusSave=WinQueryWindowULong(hwndActiveWindow, QWL_HWNDFOCUSSAVE);
                if(hwndFocusSave!=NULLHANDLE)
                    {
                                        /* First try to set focus to last active control, but don't
                                           send WM_ACTIVATE to window with new focus */
                    if(ulSlidingFocusNow==TRUE)
                        WinFocusChange(HWND_DESKTOP, hwndFocusSave, FC_NOSETACTIVE);
                    else
                        WinPostMsg(HookParameters.hwndIOThread, WM_SLIDINGFOCUSDELAY,
                            MPFROMHWND(hwndFocusSave), NULL);
                    hwndLastActiveControl=hwndFocusSave;
                    }
                else
                    {
                    if(hwndActiveWindowClient!=NULLHANDLE)
                        {
                                        /* Second try to activate client window, again don't send
                                           WM_ACTIVATE to window with new focus */
                        if(ulSlidingFocusNow==TRUE)
                            WinFocusChange(HWND_DESKTOP, hwndActiveWindowClient, FC_NOSETACTIVE);
                        else
                            WinPostMsg(HookParameters.hwndIOThread, WM_SLIDINGFOCUSDELAY,
                                MPFROMHWND(hwndActiveWindowClient), NULL);
                        hwndLastActiveControl=hwndActiveWindowClient;
                        }
                    else
                        {
                                        /* Finally try to activate frame, and too don't send
                                           WM_ACTIVATE to window with new focus */
                        if(ulSlidingFocusNow==TRUE)
                            WinFocusChange(HWND_DESKTOP, hwndActiveWindow, FC_NOSETACTIVE);
                        else
                            WinPostMsg(HookParameters.hwndIOThread, WM_SLIDINGFOCUSDELAY,
                                MPFROMHWND(hwndActiveWindow), NULL);
                        hwndLastActiveControl=hwndActiveWindow;
                        }
                    }
                if(ulSlidingFocusNow==TRUE)
                    {
                                        /* Deactivate old window (deactivate titlebar) */
                    WinPostMsg(hwndLastActiveFrameWindow, WM_ACTIVATE, MPFROMSHORT(FALSE), MPFROMHWND(hwndLastActiveControl));
                                        /* Activate new window (activate titlebar) */
                    if((hwndTitlebar=WinWindowFromID(hwndActiveWindow, FID_TITLEBAR))!=NULLHANDLE)
                        WinPostMsg(hwndActiveWindow, WM_ACTIVATE, MPFROMSHORT(TRUE), MPFROMHWND(hwndTitlebar));
                    }
                else
                    WinPostMsg(HookParameters.hwndIOThread, WM_SLIDINGFOCUSDELAY,
                        MPFROMHWND(hwndActiveWindow), NULL);
                }
            else
                {
                                        /* Now switch to the new frame window, causing
                                           a new Z-order. It will generate all messages
                                           of deactivating old and activating the
                                           new window. */
                if(hwndActiveWindowClient!=NULLHANDLE)
                    {
                    if(ulSlidingFocusNow==TRUE)
                        WinSetFocus(HWND_DESKTOP, hwndActiveWindowClient);
                    else
                        WinPostMsg(HookParameters.hwndIOThread, WM_SLIDINGFOCUSDELAY,
                            MPFROMHWND(hwndActiveWindowClient), NULL);
                    hwndLastActiveControl=hwndActiveWindowClient;
                    }
                else
                    {
                    if(ulSlidingFocusNow==TRUE)
                        WinSetFocus(HWND_DESKTOP, hwndActiveWindow);
                    else
                        WinPostMsg(HookParameters.hwndIOThread, WM_SLIDINGFOCUSDELAY,
                            MPFROMHWND(hwndActiveWindow), NULL);
                    hwndLastActiveControl=hwndActiveWindow;
                    }
                }
                                        /* Save window we've switched to */
            hwndLastActiveFrameWindow=hwndActiveWindow;
            }
                                        /* If the message was posted by PC/2's Working thread,
                                           then the delay has expired and we can select the
                                           menuentry/submenu */
        if(ulSlidingFocusDelayed==TRUE)
            {
                                        /* A goto is tolerable here as we want to save both code
                                           size and speed (because slowing down hook processing
                                           slows down the whole GUI processing) */
            goto SLIDING_FOCUS_CONTROL_PERFORM;
            }
        if(hwndLastActiveControl!=hwndActiveControl)
            {
SLIDING_FOCUS_CONTROL_PERFORM:
                                        /* If current control below mouse pointer is not the one
                                           being the last, deactivate last one and activate
                                           current one, but only if the the sliding focus should
                                           not activate only on window level instead of control
                                           level or the ALT key is not pressed */
            if((!(HookParameters.ulStatusFlag1 & FOCUSWINDOWLEVEL)) &&
                ((WinGetKeyState(HWND_DESKTOP, VK_ALT) & 0x8000)==(LONG)FALSE))
                {
                                        /* Just activate control */
                if(ulSlidingFocusNow==TRUE)
                    WinFocusChange(HWND_DESKTOP, hwndActiveControl, FC_NOSETACTIVE);
                else
                    WinPostMsg(HookParameters.hwndIOThread, WM_SLIDINGFOCUSDELAY,
                        MPFROMHWND(hwndActiveControl), NULL);
                }
            hwndLastActiveControl=hwndActiveControl;
            REROUTEMSGTONULL()
            }
        }
/*                                                                                      *\
 * If enabled, here we catch all mouse movements that are on the surrounding edges of   *
 * the physical Desktop, to unhide the Overview Window.                                 *
\*                                                                                      */
    while((HookParameters.ulStatusFlag2 & DYNAMICOVERVIEW) && !(HookParameters.ulRuntimeFlag & SYSTEMERRORACTIVE))
        {
        POINTL  ptlMouse;
                                        /* Calculate Hotspot size as a percentage of the screen */
        ULONG   ulHotSpot=(HookParameters.DesktopSize.x*DYNAMICOVERVIEW_HOTSPOT)/100;

        ulOverviewFlag&=~(OVERVIEW_UL | OVERVIEW_UR | OVERVIEW_LL | OVERVIEW_LR);
                                        /* See below (additional, over menus this effect also happens) */
        ptlMouse.x=pqmsg->ptl.x & 0x0000FFFF;
        ptlMouse.y=pqmsg->ptl.y & 0x0000FFFF;


        if(HookParameters.ulDynamicOverview & OVERVIEW_UL)
            {
            if((ptlMouse.x<=ulHotSpot) &&
                (ptlMouse.y>=(HookParameters.swpScreen.cy-ulHotSpot+ptlMouse.x)))
                ulOverviewFlag|=OVERVIEW_UL;
            }
        if(HookParameters.ulDynamicOverview & OVERVIEW_UR)
            {
            if((ptlMouse.x>=(HookParameters.swpScreen.cx-ulHotSpot)) &&
                (ptlMouse.y>=(HookParameters.swpScreen.cy+HookParameters.swpScreen.cx-ptlMouse.x-ulHotSpot)))
                ulOverviewFlag|=OVERVIEW_UR;
            }
        if(HookParameters.ulDynamicOverview & OVERVIEW_LR)
            {
            if((ptlMouse.x>=(HookParameters.swpScreen.cx-ulHotSpot)) &&
                (ptlMouse.y<=(ptlMouse.x-HookParameters.swpScreen.cx+ulHotSpot)))
                ulOverviewFlag|=OVERVIEW_LR;
            }
        if(HookParameters.ulDynamicOverview & OVERVIEW_LL)
            {
            if((ptlMouse.x<=ulHotSpot) &&
                (ptlMouse.y<=(ulHotSpot-ptlMouse.x)))
                ulOverviewFlag|=OVERVIEW_LL;
            }
                                        /* If the mouse pointer is over an edge, then request the
                                           Overview window to be unhidden immediately or start the
                                           timer if the user wanted a delay */
        if((ulOverviewFlag & (OVERVIEW_UL | OVERVIEW_UR | OVERVIEW_LL | OVERVIEW_LR))==0)
            {
            if(ulOverviewFlag & PENDINGSWITCH)
                {
                ulOverviewFlag &= ~PENDINGSWITCH;
                WinPostMsg(HookParameters.hwndIOThread, WM_OVERVIEWUNHIDEDELAY,
                    MPFROM2SHORT(-1, -1), MPFROMLONG(0));
                }
            break;
            }
        else
            {
            if(!(ulOverviewFlag & PENDINGSWITCH))
                {
                ulOverviewFlag|=PENDINGSWITCH;
                WinPostMsg(HookParameters.hwndIOThread, WM_OVERVIEWUNHIDEDELAY,
                    MPFROM2SHORT(ptlMouse.x, ptlMouse.y), MPFROMLONG(ulOverviewFlag));
                }
            }
                                        /* Don't remove message or otherwise the edge selected for
                                           Edge Activation would not work to switch between Virtual
                                           Desktops
                                           REROUTEMSGTONULL()
                                         */
        break;
        }
/*                                                                                      *\
 * If enabled, here we catch all mouse movements that are on the surrounding rows and   *
 * columns of the physical Desktop, to adjust the position of the physical Desktop      *
 * within the virtual Desktop.                                                          *
\*                                                                                      */
    while((HookParameters.ulStatusFlag1 & VIRTUALDESKTOP) && !(HookParameters.ulRuntimeFlag & SYSTEMERRORACTIVE))
        {
        POINTL  ptl;

        ulMoveFlag&=~(MOVEXR | MOVEXL | MOVEYU | MOVEYD | MOVED4CLICK);
                                        /* For some unknown reasons the pointer position
                                           pqmsg->ptl.x is not correct over the notebook
                                           control of the IBM Works Notepad application.
                                           Exactly over the the notebook the pointer
                                           position is to large by 0x00010000 (64 k).
                                           I've seen the high order 16 bits wrong only
                                           for the WARP II beta before, and Boca had to
                                           admit a bug in the beta upon my problem report.
                                           To fix I remove the high order 16 bits when
                                           comparing to the screen dimensions. Removing the
                                           the high order 16 bits from the QMSG structure
                                           itself seems to break Drag & Drop, even if the use
                                           of these bits is nowhere documented. */
        ptl.x=pqmsg->ptl.x & 0x0000FFFF;
        ptl.y=pqmsg->ptl.y & 0x0000FFFF;
        if(ptl.x<=0)
            {                           /* If we are on the left border of our physical
                                           Desktop, move all windows right as we shift
                                           it leftwards on the virtual Desktop */
            ulMoveFlag|=MOVEXR;
                                        /* If we're in the lower left corner, also move
                                           all windows up and shift downwards on the
                                           virtual Desktop */
            if(ptl.y<=HookParameters.LLHotBorder.y) ulMoveFlag|=MOVEYU;
                                        /* If we're in the upper left corner, also move
                                           all windows down and shift upwards on the
                                           virtual Desktop */
            if(ptl.y>=HookParameters.URHotBorder.y) ulMoveFlag|=MOVEYD;
            }
        if(ptl.x>=HookParameters.DesktopSize.x-1)
            {                           /* If we are on the right border of our physical
                                           Desktop, move all windows left as we shift
                                           it rightwards on the virtual Desktop */
            ulMoveFlag|=MOVEXL;
            if(ptl.y<=HookParameters.LLHotBorder.y) ulMoveFlag|=MOVEYU;
            if(ptl.y>=HookParameters.URHotBorder.y) ulMoveFlag|=MOVEYD;
            }
        if(ptl.y<=0)
            {                           /* If we are on the bottom border of our physical
                                           Desktop, move all windows up as we shift
                                           it downwards on the virtual Desktop */
            ulMoveFlag|=MOVEYU;
            if(ptl.x<=HookParameters.LLHotBorder.x) ulMoveFlag|=MOVEXR;
            if(ptl.x>=HookParameters.URHotBorder.x) ulMoveFlag|=MOVEXL;
            }
        if(ptl.y>=HookParameters.DesktopSize.y-1)
            {                           /* If we are on the top border of our physical
                                           Desktop, move all windows down as we shift
                                           it upwards on the virtual Desktop */
            ulMoveFlag|=MOVEYD;
            if(ptl.x<=HookParameters.LLHotBorder.x) ulMoveFlag|=MOVEXR;
            if(ptl.x>=HookParameters.URHotBorder.x) ulMoveFlag|=MOVEXL;
            }
                                        /* If the mouse pointer position is not a potential request
                                           to switch between Virtual Desktops, reset delay Virtual
                                           Destop timer (though it doesn't do damage if it isn't used
                                           currently, e.g. when "Click to Move" is selected) and don't
                                           do any further processing and exit loop. As no flags
                                           are set, the click loop will not find the necessity to move. */
        if(ulMoveFlag==0) break;
        if((ulMoveFlag & (MOVEXR | MOVEXL | MOVEYU | MOVEYD))==0)
            {
            if(ulMoveFlag & PENDINGSWITCH)
                {
                ulMoveFlag &= ~PENDINGSWITCH;
                WinPostMsg(HookParameters.hwndIOThread, WM_VIRTUALDESKTOPDELAY,
                    MPFROM2SHORT(-1, -1), MPFROMLONG(0));
                }
            break;
            }
                                        /* We're now about to move, but if the user
                                           selected to click before move, we exit this
                                           loop with the flags set. The click loop
                                           will then use these flags */
        ulMoveFlag|=MOVED4CLICK;
        if(HookParameters.ulStatusFlag1 & CLICK2MOVE) break;
                                        /* Post move request always when the pointer is at a
                                           surrounding row and/or column, as if the autoscroll flag
                                           is not set, the pointer will be removed from these rows
                                           or columns so that the next WM_MOUSEMOVE message will
                                           have other coordinates, if the autoscroll flag is set,
                                           the next WM_MOUSEMOVE will be posted with no change in
                                           the rows and columns, and another move request will be
        
                                   generated. If the user moves the mouse along a surrounding
                                           row and/or column while a move request is performed, the
                                           thread will ignore the resulting move requests */
        if(HookParameters.ulVirtualDesktopDelay!=0)
            {
            ulMoveFlag|=PENDINGSWITCH;
            WinPostMsg(HookParameters.hwndIOThread, WM_VIRTUALDESKTOPDELAY,
                MPFROM2SHORT(ptl.x, ptl.y), MPFROMLONG(ulMoveFlag));
            }
        else
            WinPostMsg(HookParameters.hwndClient, WM_MOVEREQUEST,
                MPFROM2SHORT(ptl.x, ptl.y), MPFROMLONG(ulMoveFlag));
        REROUTEMSGTONULL()
        }
    }
return(FALSE);                          /* Process the message in the normal way */
}

/*--------------------------------------------------------------------------------------*\
 * This procedure implements the hook of the input queue into the system queue before   *
 * the accelerator key translation took place (thanks to Robert Mahoney for telling me  *
 * that undocumented interface!).                                                       *
 * Req:                                                                                 *
 *      hab ........... Anchor block handle                                             *
 *      pqmsg ......... Pointer to system QMSG structure                                *
 *      option ........ PM_REMOVE ..... message is being removed                        *
 *                      PM_NOREMOVE ... message is not being removed                    *
 * Returns:                                                                             *
 *      FALSE ......... OS/2 should process QMSG in the normal way passing to next hook *
 *                      or to the application                                           *
 *      TRUE .......... OS/2 doesn't pass the message to next hook or the application   *
\*--------------------------------------------------------------------------------------*/
BOOL EXPENTRY   PC2DLL_PreAccelHook(HAB hab, PQMSG pqmsg, ULONG option)
{
                                        /* Check for ALT+CTRL+PRTSCRN key combination (on 
                                           KC_KEYUP event) */
if((pqmsg->msg==WM_CHAR) &&
    (SHORT1FROMMP(pqmsg->mp1) & (KC_SCANCODE|KC_CTRL|KC_ALT|KC_KEYUP))==(KC_SCANCODE|KC_CTRL|KC_ALT|KC_KEYUP))
    {
    if(((ULONG)pqmsg->mp1 & 0xFF000000)==0x54000000)
        {
                                        /* ALT+CTRL+PRTSCRN pressed */
        WinBroadcastMsg(HWND_DESKTOP, WM_SYSCOMMAND,
            MPFROMSHORT(VK_PRINTSCRN), MPFROM2SHORT(CMDSRC_ACCELERATOR, FALSE),
            BMSG_POST|BMSG_FRAMEONLY);
        DosBeep(800, 200);
        REROUTEMSGTONULL()
        }
    else
        {
                                        /* ALT+CTRL+? pressed */
        }
    }
/*                                                                                      *\
 * As PC/2 is very complicated and interfaces deeply with the OS/2 system, I've added   *
 * a key combination that dumps (but not terminates) the PC/2 process. Hopefully this   *
 * will allow me to better diagnose bugs/hangs (hopefully because only limited          *
 * information gets dumped, I need to have the correct dump formatter, and usage of the *
 * dump formatter would be new for me).                                                 *
\*                                                                                      */
if(((SHORT1FROMMP(pqmsg->mp1)) & (KC_KEYUP|KC_ALT|KC_CTRL|KC_SHIFT))==(KC_KEYUP|KC_ALT|KC_CTRL|KC_SHIFT))
    {
    static ULONG    ulLastKey;
    ULONG           ulCurrentKey;

                                        /* If the ALT+CTRL+SHIFT keys are depressed while the user
                                           depresses F9 for the second time, preform a dump of PC/2 */
    ulCurrentKey=((ULONG)pqmsg->mp1) & 0xFF000000;
    if((ulCurrentKey==0x43000000) && (ulLastKey==0x43000000))
        {
        DosBeep(1000, 1000);
                                        /* There is a confirmed bug (at the moment Warp4 is the
                                           latest release) that causes hangs of OS/2
        DosDumpProcess(DDP_PERFORMPROCDUMP, (ULONG)HookParameters.ucBootDrive, HookParameters.ProcessId);
                                           so we have to disable that statement */
        ulLastKey=0;
        REROUTEMSGTONULL()
        }
/*                                                                                      *\
 * Check for cursor and nummeric keypad keys to switch between Virtual Desktops with    *
 * the cursor keys, that is ALT+CTRL+SHIFT+cursor                                       *
\*                                                                                      */
    ulMoveFlag&=~(MOVEXR | MOVEXL | MOVEYU | MOVEYD | MOVED4CLICK);
    switch(ulCurrentKey)
    {
    case NUM_KEY_LEFT_UP:
        ulMoveFlag|=(MOVEXR | MOVEYD);
        break;

    case CUR_KEY_UP:
    case NUM_KEY_UP:
        ulMoveFlag|=MOVEYD;
        break;

    case NUM_KEY_RIGHT_UP:
        ulMoveFlag|=(MOVEXL | MOVEYD);
        break;

    case CUR_KEY_RIGHT:
    case NUM_KEY_RIGHT:
        ulMoveFlag|=MOVEXL;
        break;

    case NUM_KEY_RIGHT_DOWN:
        ulMoveFlag|=(MOVEXL | MOVEYU);
        break;

    case CUR_KEY_DOWN:
    case NUM_KEY_DOWN:
        ulMoveFlag|=MOVEYU;
        break;

    case NUM_KEY_LEFT_DOWN:
        ulMoveFlag|=(MOVEXR | MOVEYU);
        break;

    case CUR_KEY_LEFT:
    case NUM_KEY_LEFT:
        ulMoveFlag|=MOVEXR;
        break;
    }
    if(!HookParameters.hwndLockup)
        if((ulMoveFlag & (MOVEXR | MOVEXL | MOVEYU | MOVEYD | MOVED4CLICK))!=0)
            WinPostMsg(HookParameters.hwndClient, WM_MOVEREQUEST,
                MPFROM2SHORT(100, 100), MPFROMLONG(ulMoveFlag));
    ulLastKey=ulCurrentKey;
    }
/*                                                                                      *\
 * Here we catch all WM_CHAR messages filtering them for hotkeys. Found hotkeys are     *
 * sent to PC/2. Here we also check for keys defined to activate PC/2's Popup Menu. We  *
 * support not only keys available on all keyboards (SHIFT and CTRL), but also keys     *
 * available on the newer WIN95 keyboards (all 3, however for the WIN95 MENU key we     *
 * have to remove it, because it would generate a PRTSCRN too). As the WIN95 keyboard   *
 * generates for the new keys also scancodes, the standard OS/2 keyboard driver will    *
 * work for those keys, hence we're able to filter them.                                *
\*                                                                                      */
while((pqmsg->msg==WM_CHAR) && !(HookParameters.ulRuntimeFlag & SYSTEMERRORACTIVE))
    {
    ULONG           ulRemoveKeyFlag=FALSE;

                                        /* If no scan code is available for key, ignore. We
                                           need to have the scan code, because this is the
                                           only common part of WM_CHAR between PM and
                                           WINDOWCOMPAT sessions. As we have queried KBD$ for
                                           the translation between scan codes and ASCII values
                                           there is is no problem that NLS-keyboards have
                                           different scan code to ASCII translations */
    if(!(SHORT1FROMMP(pqmsg->mp1) & KC_SCANCODE))
        break;
                                        /* Keyboard scan code of character pressed */
    ucScanCode=CHAR4FROMMP(pqmsg->mp1);
                                        /* We now have a pressed or depressed key's scan code, now
                                           we are only interested if a ALT or CTRL key was pressed
                                           and hold before and if SHIFT was pressed too. */
    usFlags=((SHORT1FROMMP(pqmsg->mp1)) & (KC_ALT|KC_CTRL|KC_SHIFT));
                                        /* Do not update with KC_*WIN* key flags 
    usFlags|=usFlagsWinKeys;
                                           as this will prevent the right CTRL key to work
                                           as RCTRL and as part of the CTRL keys (due to the
                                           LWIN key having the same scancode than the right
                                           CTRL key on IBM WIN95 keyboards (at least on mine) */
                                        /* If the ALT key was pressed disable spying now (so that
                                           the user then can copy & paste the last spyed window */
    if((usFlags==KC_ALT) && (HookParameters.ulRuntimeFlag & SPYFRAMEWINDOW))
        {
        HookParameters.ulRuntimeFlag&=(~SPYFRAMEWINDOW);
        WinPostMsg(HookParameters.hwndWorkingThread, WM_SPYFRAMEWINDOW, NULL, NULL);
        }
                                        /* It can only be a Hotkey when either the left Windows,
                                           the right CTRL (is part of the CTRL keys), the ALT 
                                           and/or the CTRL keys were pressed, but SHIFT was 
                                           not pressed. */
    while((usFlagsWinKeys==KC_LWIN) || 
        (usFlags==KC_CTRL) || 
        (usFlags==KC_ALT) || 
        (usFlags==(KC_CTRL|KC_ALT)))
        {
        ULONG   ulAccept=FALSE;

                                        /* If the user wants the Hotkeys detected only when
                                           either PC/2, the Window List or the WPS is active
                                           determine the active window */
        if(HookParameters.ulStatusFlag1 & HOTKEY4PC2ONLY)
            {
                                        /* Query the currently active window, where HWND_DESKTOP
                                           is the parent window. It will be a WC_FRAME class
                                           window */
            hwndActiveFrameWindow=WinQueryActiveWindow(HWND_DESKTOP);
            WinQueryWindowText(hwndActiveFrameWindow, sizeof(ucWindowText), ucWindowText);
                                        /* We can accept the WPS having the focus */
            if((strstr(ucWindowText, HookParameters.ucDesktopName)) ||
                                        /* We can accept PM having the focus */
                (hwndActiveFrameWindow==HookParameters.hwndDesktop) ||
                                        /* We can accept the Window List having the focus */
                (strstr(ucWindowText, HookParameters.ucWindowListName)) ||
                                        /* We can accept any PC/2 Window having the focus */
                (strstr(ucWindowText, "PC/2")) ||
                                        /* We can accept no window having the focus */
                (hwndActiveFrameWindow==NULLHANDLE))
                ulAccept=TRUE;
            }
        else
            ulAccept=TRUE;
                                        /* The active window is not one of the required ones,
                                           so don't handle this message any further as possible
                                           Hotkey */
        if(!ulAccept)
            break;
                                        /* Now try to find the key, if the key is found skip the key down
                                           (pressed) event and use only the key up (depressed) event. We
                                           have to look for the key in both events, because the key down
                                           event will result in a accelerator key translation if not removed,
                                           but PC/2 should only react on key up events (to get no pending key
                                           up event to the new/hotkeyed session into)  */
        for(ulKeyDataIndex=0, pKeyDataIndex=KeyData; ulKeyDataIndex<KEYDATACOUNT; ulKeyDataIndex++, pKeyDataIndex++)
            {
            if((pKeyDataIndex->ucScanCode==ucScanCode) &&
                ((pKeyDataIndex->usFlags==usFlags) || 
                (pKeyDataIndex->usFlags==usFlagsCtrlKeys) ||
                (pKeyDataIndex->usFlags==usFlagsWinKeys)))
                {
                if(pKeyDataIndex->bUsed==TRUE)
                    {
                    if(SHORT1FROMMP(pqmsg->mp1) & KC_KEYUP)
                        {
                        WinPostMsg(HookParameters.hwndClient, WM_HOTKEY,
                            MPFROM2SHORT(pKeyDataIndex->usFlags, pKeyDataIndex->usCh), MPFROMLONG(ulKeyDataIndex));
                        }
                                        /* Clear last WM_CHAR message pressed to avoid the unremoved part
                                           of the Hotkey (e.g. CTRL) to activate the Popup Menu (when
                                           configured to accessible by a single e.g. CTRL key click) */
                    ulLastWMCHARmp1=ulLastWMCHARmp2=0;
                    REROUTEMSGTONULL()
                    }
                                        /* Even if our key is not used we have to search the whole
                                           array as the right CTRL key is also part of both CTRL
                                           keys. So if we found the entry for RCTRL+* is empty
                                           that does not mean that CTRL+* is not empty later on.
                                           Important, this has changed with 2.20 as previously we
                                           had coded
                else break; 
                                           here */
                }
            }
                                        /* Get out of loop anyway */
        break;
        }
                                        /* KC_* flags */
    usFlags=SHORT1FROMMP(pqmsg->mp1);
                                        /* Update with KC_*WIN* key flags */
    usFlags|=usFlagsWinKeys;
                                        /* We assume that the required make and break code of a key is
                                           not available unless we find the opposite */
    ulPopupKeyStatus|=MAKEBREAKINVALID;
    while(TRUE)
        {
        ULONG   ulCurrentWMCHARmp1=((ULONG)pqmsg->mp1 & MASK_WIN95_KEY_MP1);
        ULONG   ulCurrentWMCHARmp2=((ULONG)pqmsg->mp2 & MASK_WIN95_KEY_MP2);
        ULONG   ulPopup4KeyboardIndex;

                                        /* We now have a pressed key's scan code, now we are
                                           interested in the left and right SHIFT key, to activate
                                           the Popup-Menu with */
        if((usFlags & KC_SHIFT) && (HookParameters.ulPopupKeyFlag & POPUPLEFTRIGHTSHIFT))
            {
            if(ucScanCode==LEFT_SHIFT_KEY)
                {
                if(ulShiftStatus & RIGHT_SHIFT)
                    {                   /* We detected the left SHIFT key while the right SHIFT key
                                           was already pressed */
                    HookParameters.ulRuntimeFlag|=POPUPMENU4KEYBOARD;
                    WinPostMsg(HookParameters.hwndClient, WM_POPUPMENU,
                        MPFROM2SHORT((USHORT)pointlCurrent.x, (USHORT)pointlCurrent.y), MPFROMHWND(pqmsg->hwnd));
                    }
                                        /* Mark that left SHIFT was pressed */
                ulShiftStatus|=LEFT_SHIFT;
                }
            if(ucScanCode==RIGHT_SHIFT_KEY)
                {
                if(ulShiftStatus & LEFT_SHIFT)
                    {                   /* We detected the right SHIFT key while the left SHIFT key
                                           was already pressed */
                    HookParameters.ulRuntimeFlag|=POPUPMENU4KEYBOARD;
                    WinPostMsg(HookParameters.hwndClient, WM_POPUPMENU,
                        MPFROM2SHORT((USHORT)pointlCurrent.x, (USHORT)pointlCurrent.y), MPFROMHWND(pqmsg->hwnd));
                    }
                                        /* Mark that right SHIFT was pressed */
                ulShiftStatus|=RIGHT_SHIFT;
                }
            break;
            }
                                        /* We continue to test for another key to activate the
                                           Popup-Menu with only when no other key was pressed in
                                           between */
        if((ulLastWMCHARmp1 & MASK_WIN95_KEY_MP1)==ulCurrentWMCHARmp1)
            {
                                        /* If the same key was depressed (break code) and was previously
                                           pressed (make code) but not previously pressed too, we
                                           found a valid key */
            if(((usFlags & (KC_PREVDOWN|KC_KEYUP))==KC_KEYUP)
                && (!(SHORT1FROMMP(ulLastWMCHARmp1) & KC_PREVDOWN)))
                                        /* If the current code is the break code of the same key we
                                           previously found a make code that was the first make code
                                           of this key at all, then we accept this key. Other
                                           make and break sequences are unacceptable */
                ulPopupKeyStatus&=(~MAKEBREAKINVALID);
            }
                                        /* Loop trough our table of possible Hotkeys to activate the
                                           Popup Menu with. We even search for make codes, as for some
                                           keys we also remove the make code to avoid problems (e.g.
                                           on IBM's WIN95 keyboard the right WIN95 key would do Enter
                                           in IBM's CM/2 or the WIN95 menu key would do PrtScrn) */
        for(ulPopup4KeyboardIndex=0;
            ulPopup4KeyboardIndex<(sizeof(Popup4Keyboard)/sizeof(Popup4Keyboard[0]));
            ulPopup4KeyboardIndex++)
            {
            ULONG   ulCacheMP2Down;
            ULONG   ulCacheMP2Up;
                                        /* If we are not interested in the to be tested key's index,
                                           continue */
            if(!(HookParameters.ulPopupKeyFlag & Popup4Keyboard[ulPopup4KeyboardIndex].ulPopupKeyFlag))
                continue;
                                        /* If it is not our key's first parameter, continue search */
            if(Popup4Keyboard[ulPopup4KeyboardIndex].ulWMCHARmp1!=ulCurrentWMCHARmp1)
                continue;
                                        /* If it is not one of our key's second parameter (where 0 is
                                           used to avoid the comparism at all), continue search */
            ulCacheMP2Down=Popup4Keyboard[ulPopup4KeyboardIndex].ulWMCHARmp2Down;
            ulCacheMP2Up=Popup4Keyboard[ulPopup4KeyboardIndex].ulWMCHARmp2Up;
            if((ulCacheMP2Down!=0) && (ulCacheMP2Up!=0))
                if((ulCacheMP2Down!=ulCurrentWMCHARmp2) && (ulCacheMP2Up!=ulCurrentWMCHARmp2))
                    continue;
                                        /* See if that key needs to be removed */
            ulRemoveKeyFlag=Popup4Keyboard[ulPopup4KeyboardIndex].ulRemoveKeyFlag;
                                        /* If we found the key, and the user just released the key after
                                           a single click, it's time to activate the Popup Menu */
            if((usFlags & KC_KEYUP) && !(ulPopupKeyStatus & MAKEBREAKINVALID))
                {
                HookParameters.ulRuntimeFlag|=POPUPMENU4KEYBOARD;
                WinPostMsg(HookParameters.hwndClient, WM_POPUPMENU,
                    MPFROM2SHORT((USHORT)pointlCurrent.x, (USHORT)pointlCurrent.y), MPFROMHWND(pqmsg->hwnd));
                }
                                        /* Once we found a key we have found out everything we can by now */
            break;
            }
                                        /* When detecting the make code of the WIN keys, then
                                           save the fact by introducing the artificial flags
                                           KC_*WIN* (which can then be used when detecting another
                                           keys make code equivalent to e.g. the KC_ALT flag, in
                                           other words we add a KC_*WIN* flags what the keyboard
                                           driver does not do natively) */ 
        if((ulCurrentWMCHARmp1==LWIN95_KEY_IBMMP1) || (ulCurrentWMCHARmp1==LWIN95_KEY_OEMMP1))
            {
            if((usFlags & KC_KEYUP)==KC_KEYUP)
                usFlagsWinKeys&=(~KC_LWIN);
            else
                usFlagsWinKeys|=KC_LWIN;
            }
                                        /* Same for the CTRL keys */
        if(ulCurrentWMCHARmp1==RCTRL_KEY_MP1)
            {
            if((usFlags & KC_KEYUP)==KC_KEYUP)
                usFlagsCtrlKeys&=(~KC_RCTRL);
            else
                usFlagsCtrlKeys|=KC_RCTRL;
            }
                                        /* Break out of loop anyway */
        break;
        }
                                        /* Save last WM_CHAR message pressed. For the WIN95 keys remove
                                           them when requested (e.g. for the WIN95 menu key, this prevents
                                           a PRTSCRN on IBM keyboards */
    ulLastWMCHARmp1=(ULONG)pqmsg->mp1;
    ulLastWMCHARmp2=(ULONG)pqmsg->mp2;
                                        /* Remove the key when required to prevent further processing */
    if(ulRemoveKeyFlag==TRUE)
        REROUTEMSGTONULL()
/*                                                                                      *\
 * Here we catch all WM_CHAR messages filtering them for the QuickSwitch feature.       *
\*                                                                                      */
                                        /* For QuickSwitch we are interested in the key depressed
                                           and in the status of the CTRL and SHIFT keys
                                           only interested if a ALT or CTRL key was pressed
                                           and hold before and if SHIFT was pressed too. */
    usFlags=((SHORT1FROMMP(pqmsg->mp1)) & (KC_CTRL|KC_SHIFT|KC_KEYUP));
    if(HookParameters.ulStatusFlag2 & ENABLEQUICKSWITCH)
        {
        if(!(usFlags & KC_KEYUP))
            {
                                        /* Search for QuickSwitch hotkeys CTRL+TAB and CTRL+SHIFT+TAB */
            if((usFlags & KC_CTRL) && (ucScanCode==TAB_KEY))
                {
                if(usFlags==KC_CTRL)
                    {                   /* CTRL+TAB switches to next session and discard key */
                    ulQuickSwitch++;
                    WinPostMsg(HookParameters.hwndWorkingThread, WM_QUICKSWITCH,
                        MPFROMLONG(QUICKSWITCH_NEXT), NULL);
                    REROUTEMSGTONULL()
                    }
                if(usFlags==(KC_CTRL|KC_SHIFT))
                    {                   /* CTRL+SHIFT+TAB switches to previous session and discard key */
                    ulQuickSwitch++;
                    WinPostMsg(HookParameters.hwndWorkingThread, WM_QUICKSWITCH,
                        MPFROMLONG(QUICKSWITCH_PREVIOUS), NULL);
                    REROUTEMSGTONULL()
                    }
                }
            }
        else
            {
                                        /* If any key is depressed, clear SHIFT status flag */
            ulShiftStatus=0;
                                        /* If QuickSwitch is active, and any other key than the tab
                                           key or shift keys are depressed, inform PC/2 to dismiss
                                           the QuickSwitch window without switching to an application */
            if((ucScanCode!=TAB_KEY) && (ucScanCode!=LEFT_SHIFT_KEY) &&
                (ucScanCode!=RIGHT_SHIFT_KEY) && (ulQuickSwitch))
                {
                                        /* If the CTRL key was released, switch to the selected application,
                                           otherwise just dismiss the QuickSwitch window */
                if(usFlags & KC_CTRL)
                    WinPostMsg(HookParameters.hwndWorkingThread, WM_QUICKSWITCH,
                        MPFROMLONG(QUICKSWITCH_DISMISS), NULL);
                else
                    WinPostMsg(HookParameters.hwndWorkingThread, WM_QUICKSWITCH,
                        MPFROMLONG(QUICKSWITCH_SWITCH), NULL);
                ulQuickSwitch=0;
                REROUTEMSGTONULL()
                }
            return(FALSE);
            }
        }
                                        /* Get out of loop */
    break;
    }
return(FALSE);                          /* Process the message in the normal way */
}

/*--------------------------------------------------------------------------------------*\
 * This procedure formats the contents of the clipboard in order to paste the clipboard *
 * contents containing or not containing a trailing \r\n. If the CTRL key is not        *
 * pressed, trailing \r\n characters are removed, if the CTRL key is pressed, a single  *
 * trailing \r\n is added. The text can then be pasted by posting a message to a        *
 * window.                                                                              *
 * Req:                                                                                 *
 *      hwnd .......... Window handle context to format clipboard data in               *
 * Ret:                                                                                 *
 *      ulRc .......... NO_ERROR, as we can ignore errors savely                        *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   FormatClipbrdData(HWND hwnd)
{
HAB     hab;
ULONG   ulLength=0;
UCHAR   *pucTextOld=NULL;
UCHAR   *pucTextNew=NULL;

                                        /* Paste the text from clipboard, remove any \r and/or
                                           \n, add a single \r\n if the CTRL key is pressed,
                                           and copy the modified text into the clipboard */
hab=WinQueryAnchorBlock(hwnd);
WinOpenClipbrd(hab);
                                        /* Paste text from clipboard */
pucTextOld=(UCHAR *)WinQueryClipbrdData(hab, CF_TEXT);
if(pucTextOld)
    {
                                        /* We may have to add \r\n and of course \0 */
    ulLength=strlen(pucTextOld)+3;
                                        /* Allocate unnamed shared memory and copy the text */
    if(!DosAllocSharedMem((PVOID)&pucTextNew, NULL, ulLength,
        PAG_WRITE|PAG_COMMIT|OBJ_GIVEABLE))
        {
        strcpy(pucTextNew, pucTextOld);
                                        /* Close clipboard as we've copied its contents. This
                                           also invalidates the shared memory the clipboard
                                           pasted the text into */
        WinCloseClipbrd(hab);
                                        /* Reopen clipboard and copy modified text into */
        WinOpenClipbrd(hab);
        WinEmptyClipbrd(hab);
        pucTextOld=strchr(pucTextNew, '\0');
        for(pucTextOld=strchr(pucTextOld, '\0');
            pucTextOld && pucTextOld>=pucTextNew;
            pucTextOld--)
            {
            if(*pucTextOld=='\0') continue;
            if(*pucTextOld=='\n') { *pucTextOld='\0'; continue; }
            if(*pucTextOld=='\r') { *pucTextOld='\0'; continue; }
            break;
            }
                                        /* Add a single \r\n if the CTRL key is pressed */
        if((WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000))
            strcat(pucTextNew, "\r\n");
        WinSetClipbrdData(hab, (ULONG)pucTextNew, CF_TEXT, CFI_POINTER);
        }
    }
WinCloseClipbrd(hab);
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure copies the contens of static text windows into the clipboard.         *
 * Req:                                                                                 *
 *      hwnd .......... Window handle context to copy data into clipboard               *
 * Ret:                                                                                 *
 *      ulRc .......... NO_ERROR, as we can ignore errors savely                        *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   CopyToClipbrdData(HWND hwnd)
{
HAB     hab;
ULONG   ulLength=0;
UCHAR   *pucText=NULL;

                                        /* Copy the text into the clipboard */
ulLength=WinQueryWindowTextLength(hwnd);
if(ulLength!=0)
    {
    ulLength++;
    if(!DosAllocSharedMem((PVOID)&pucText, NULL, ulLength, PAG_WRITE|PAG_COMMIT|OBJ_GIVEABLE))
        {
        hab=WinQueryAnchorBlock(hwnd);
        WinOpenClipbrd(hab);
        WinEmptyClipbrd(hab);
        memset(pucText, '\0', ulLength);
        WinQueryWindowText(hwnd, ulLength, pucText);
        WinSetClipbrdData(hab, (ULONG)pucText, CF_TEXT, CFI_POINTER);
        WinCloseClipbrd(hab);
        }
    }
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure calculates the overlapping between one window and another.            *
 * Req:                                                                                 *
 *      pswpDestination The destination window, which should not be overlapped          *
 *      pswpSource .... The source window which may overlap the destination window,     *
 *                      the overlapping is calculated and returned then                 *
 * Returns:                                                                             *
 *      lAdjust ....... The amount of overlapping between the two windows (>0 means     *
 *                      source must be moved upwards, <0 means source must be moved     *
 *                      downwards, 0 means no move required at all)                     *
\*--------------------------------------------------------------------------------------*/
LONG    AdjustOverlap(SWP *pswpDestination, SWP *pswpSource)
{
RECTL   rectlDestination;
RECTL   rectlSource;
LONG    lAdjust;

                                        /* Calculate rectangles of windows */
rectlDestination.xLeft=rectlDestination.xRight=pswpDestination->x;
rectlDestination.yBottom=rectlDestination.yTop=pswpDestination->y;
rectlDestination.xRight+=pswpDestination->cx;
rectlDestination.yTop+=pswpDestination->cy;
rectlSource.xLeft=rectlSource.xRight=pswpSource->x;
rectlSource.yBottom=rectlSource.yTop=pswpSource->y;
rectlSource.xRight+=pswpSource->cx;
rectlSource.yTop+=pswpSource->cy;
                                        /* Check where the windows overlap and calculate the
                                           adjustment required for the source window accordingly */
lAdjust=0;
if((rectlSource.xLeft<=rectlDestination.xRight) || (rectlSource.xRight>=rectlDestination.xLeft))
    {
                                        /* Check if the source's bottom is inside the destination's
                                           window */
    if((rectlSource.yBottom>=rectlDestination.yBottom) && (rectlSource.yBottom<=rectlDestination.yTop))
        {
        lAdjust=pswpDestination->cy-(rectlSource.yBottom-rectlDestination.yBottom);
        }
                                        /* Check if the source's top is inside the destination's
                                           window */
    if((rectlSource.yTop>=rectlDestination.yBottom) && (rectlSource.yTop<=rectlDestination.yTop))
        {
        lAdjust=rectlDestination.yBottom-rectlSource.yTop;
        }
    }
return(lAdjust);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure implements the hook of WinSendMsg() calls in the system message       *
 * queue.                                                                               *
 * Req:                                                                                 *
 *      hab ........... Anchor block handle                                             *
 *      pSmh .......... Pointer to send message hook structure                          *
 *      bInterTask .... TRUE .... Message is sent between tasks (intertask)             *
 *                      FALSE ... Message is sent within task (intratask)               *
 * Returns:                                                                             *
\*--------------------------------------------------------------------------------------*/
void EXPENTRY   PC2DLL_WinSendMsgHook(HAB hab, PSMHSTRUCT pSmh, BOOL bInterTask)
{
CLASSINFO   classinfoWindow;

/*                                                                                      *\
 * Track the visibility of the PM Hard Error and System Error system modal dialogs.     *
 * These dialogs are part of the PM process itself, and a child of HWND_OBJECT, unless  *
 * they are made active (e.g. by typing DIR A: without a diskette in drive A:), which   *
 * causes HWND_DESKTOP to be set as the new parent. When this dialog is active, we have *
 * to prevent PC/2's focus and Virtual Desktop manipulations to avoid the PM focus      *
 * processing to hang. Once dismissed, the parent is set back to HWND_OBJECT.           *
\*                                                                                      */
if((pSmh->hwnd==HookParameters.hwndPMHardError) || (pSmh->hwnd==HookParameters.hwndSystemError))
    {
    if(pSmh->msg==WM_WINDOWPOSCHANGED)
        {
                                        /* Set the flag depending on the visibility of an error window.
                                           We have to test for visibility, because the system error window,
                                           that appears also when using CTRL+ESC to unhang the PM SIQ,
                                           does not create a WM_WINDOWPOSCHANGED message where HWND_OBJECT
                                           again becomes the parent after having dismissed it. The hard
                                           error window (e.g. for a dir a: where a: is empty) does. So
                                           testing for HWND_DESKTOP to be the parent (<=> visible) and
                                           HWND_OBJECT to be the parent (<=> invisible window) does not
                                           work for the system error window after a CTRL+ESC. We explicitely
                                           therefore test for visibility instead */
        if(WinIsWindowVisible(pSmh->hwnd))
            HookParameters.ulRuntimeFlag|=SYSTEMERRORACTIVE;
        else
            HookParameters.ulRuntimeFlag&=(~SYSTEMERRORACTIVE);
        }
    }
                                        /* We may avoid some problems to return without changing
                                           anything when the System Error popups are active */
if(HookParameters.ulRuntimeFlag & SYSTEMERRORACTIVE)
    return;
/*                                                                                      *\
 * Check for selection of a Menuentry. If the user has requested to allow Dynamic       *
 * Menuselection not just with the mouse but also with the keyboard, then simulate that *
 * by a dummy WM_MOUSEMOVE message. This message will then get "caught" by the          *
 * WM_MOUSEMOVE processing where it can be differentiated by the otherwise unexpected   *
 * parameter HT_DYNAMICMENUKBD.                                                         *
\*                                                                                      */
if((pSmh->msg==WM_MENUSELECT) &&
    ((HookParameters.ulStatusFlag2 & (DYNAMICMENU|DYNAMICMENUKBD))==(DYNAMICMENU|DYNAMICMENUKBD)))
    {
    RECTL   rectlItem;
    USHORT  usMenuId=WinQueryWindowUShort((HWND)pSmh->mp2, QWS_ID);

                                        /* Check for MINMAX and SYSMENU menus as at least for
                                           Communicator/2 and Notes not doing so causes what seems
                                           to be a (though killable from the Window List) loop. Dynamic
                                           MenuSelection doesn't make sense for these kinds of menus
                                           anyway. Also prevent to select again what is already selected,
                                           e.g. for submenues this is important to stop recursive
                                           cascading down the selection. */
    if(((usMenuId!=FID_MINMAX) && (usMenuId!=FID_SYSMENU)) &&
        (hwndLastMenu!=HWNDFROMMP(pSmh->mp2)))
        {
        WinSendMsg(HWNDFROMMP(pSmh->mp2), MM_QUERYITEMRECT,
            MPFROM2SHORT((SHORT)pSmh->mp1, FALSE), MPFROMP(&rectlItem));
                                        /* An alternativ implementation would be to reposition the
                                           mouse pointer. Unfortunately this:
                                           pointlMouse.x=(rectlItem.xLeft+((rectlItem.xRight-rectlItem.xLeft)>>1));
                                           pointlMouse.y=(rectlItem.yBottom+((rectlItem.yTop-rectlItem.yBottom)>>1));
                                           WinMapWindowPoints(HWNDFROMMP(pSmh->mp2), HWND_DESKTOP, &pointlMouse, 1);
                                           WinSetPointerPos(HWND_DESKTOP, pointlMouse.x, pointlMouse.y);
                                           does not work, because we can't differentiate between mouse
                                           and keyboard initiated menu selection. That causes the mouse
                                           pointer to "prevent" it to be removed from a menu, because
                                           moving away the pointer causes a "un-"selection which again
                                           repositions the mouse pointer,... */
        WinPostMsg((HWND)pSmh->mp2, WM_MOUSESELECT,
            MPFROM2SHORT((SHORT)(rectlItem.xLeft+((rectlItem.xRight-rectlItem.xLeft)>>1)),
                         (SHORT)(rectlItem.yBottom+((rectlItem.yTop-rectlItem.yBottom)>>1))),
            MPFROM2SHORT(HT_DYNAMICMENUKBD, (SHORT)pSmh->mp1));
        }
    }
/*                                                                                      *\
 * Initialize common data structures                                                    *
\*                                                                                      */
WinQueryClassName(pSmh->hwnd, sizeof(ucClassname), ucClassname);
memset(&classinfoWindow, 0, sizeof(classinfoWindow));
WinQueryClassInfo(WinQueryAnchorBlock(pSmh->hwnd), ucClassname, &classinfoWindow);
/*                                                                                      *\
 * We are only interested in frame windows.                                             *
\*                                                                                      */
if((!strcmp(ucClassname, "#1") || (classinfoWindow.flClassStyle & CS_FRAME)))
    {
                                        /* If a text is set into a frame window, check if it
                                           is the WPS, because if it didn't exist already this
                                           message means that it must have been created, and
                                           we are interested in that fact to be able to get the
                                           WPS's client handle to detect mouse clicks on. Do
                                           set the WPS handle immediately, or the WPS will also
                                           be subject to positional corrections not to cover the
                                           SessionBar.
                                           This is a workaround to avoid trapping of SmartSuite
                                           for OS/2 Warp's Lotus 1-2-3 when querying the window text */
    if(pSmh->msg==WM_SETWINDOWPARAMS)
        {
        if((((WNDPARAMS *)pSmh->mp1)->fsStatus & WPM_TEXT) &&
            (((WNDPARAMS *)pSmh->mp1)->pszText!=0) &&
            (!strcmp(((WNDPARAMS *)pSmh->mp1)->pszText, HookParameters.ucDesktopName)))
            {
            WinPostMsg(HookParameters.hwndWorkingThread, WM_SETDESKTOPHANDLE,
                MPFROMHWND(pSmh->hwnd), NULL);
            }
        }
    if(HookParameters.ulRuntimeFlag & SUBCLASSICONS)
        {
/*                                                                                      *\
 * If Titlebar Smarticons are enabled, we detect changes in the window position of      *
 * frame windows. If the frame window is not yet registered yet having Titlebar Smart-  *
 * icons, we will request the input hook to register and subclass it.                   *
\*                                                                                      */
        if(pSmh->msg==WM_CREATE)
            {
            WinPostMsg(pSmh->hwnd, WM_SYSCOMMAND,
                MPFROMSHORT(SC_CONTROLSINSTALL), MPFROM2SHORT(CMDSRC_OTHER, FALSE));
            }
/*                                                                                      *\
 * If Titlebar Smarticons are enabled and a window gets destroyed the subclassed window *
 * procedure deregisters and deallocates the Smarticons during the WM_DESTROY           *
 * processing, however for windowed command processors WM_DESTROY no longer gets        *
 * routed to the subclassed window procedures for unknown reasons.                      *
\*                                                                                      */
        else if(pSmh->msg==WM_DESTROY)
            {
                                        /* If we can't do the cleanup in our subclassed window
                                           procedure do it here (from experience it seems we are
                                           running in the correct thread context). Using WM_SYSCOMMAND
                                           works for all applications but NetScape, hopefully
                                           getting NetScape work does not break other applications */
            if(WinSendMsg(pSmh->hwnd, WM_SMARTICONS,
                MPFROMSHORT(SC_DESTROY), MPFROM2SHORT(CMDSRC_MENU, FALSE))!=(MRESULT)pSmh->hwnd)
                {
                RegisterFrame(pSmh->hwnd, FALSE);
                }
            }
        }
/*                                                                                      *\
 * If a window gets activated, ensure that we switch to the Virtual Desktop the window  *
 * resides on. If the activation changes, ensure Sliding Focus gets an update.          *
\*                                                                                      */
                                        /* If we would activate a frame because we clicked
                                           on one of the surrounding rows or columns to switch
                                           to another Virtual Desktop, don't ensure that we
                                           don't switch to that window, because the click
                                           to switch to an adjacent Virtual Desktop would
                                           switch a second time. Usually that frame is the WPS.
                                           Activation of the WPS (e.g. as a result to display
                                           PC/2's Popup Menu) will not cause to switch to
                                           the Virtual Desktop it is residing on at all */
    if((pSmh->msg==WM_ACTIVATE) && 
        ((ulMoveFlag & (MOVEXR|MOVEXL|MOVEYD|MOVEYU))==0) &&
        (pSmh->hwnd!=HookParameters.hwndWPSFrame))
        {
        if((BOOL)pSmh->mp1==TRUE)
            {
            if((pSmh->hwnd!=HookParameters.hwndFrame) &&
                (HookParameters.ulStatusFlag1 & FOLLOWFOCUS))
                WinPostMsg(HookParameters.hwndWorkingThread, WM_QUICKSWITCH,
                    MPFROMLONG(QUICKSWITCH_MOVETO), MPFROMHWND(pSmh->hwnd));
            }
        else
            {
            if(HookParameters.ulStatusFlag1 & SLIDINGFOCUS)
                {
                hwndLastActiveControl=NULLHANDLE;
                WinPostMsg(pSmh->msg, WM_MOUSEMOVE,
                    MPFROM2SHORT((SHORT)pointlLast.x, (SHORT)pointlLast.y),
                    MPFROM2SHORT(TRUE, KC_NONE));
                }
            }
        }
/*                                                                                      *\
 * To prevent the SessionBar to be overlapped, and to keep PC/2 windows on top of       *
 * Z-Order we are interested in position changes of frame windows being child windows   *
 * of HWND_DESKTOP. Possibly WM_ADJUSTFRAMEPOS is an alternative.                       *
\*                                                                                      */
    if(((pSmh->msg==WM_WINDOWPOSCHANGED) || (pSmh->msg==WM_ADJUSTWINDOWPOS)) &&
        (WinQueryWindow(pSmh->hwnd, QW_PARENT)==HookParameters.hwndDesktop))
        {
        SWP     *pswpWindow=(SWP *)(pSmh->mp1);

/*                                                                                      *\
 * If the SessionBar is active and set onto top of Z-Order, try to keep all windows at  *
 * a size and/or position, that the window does not get overlapped by the SessionBar    *
 * and keeps maximum visibility. We are only interested in Desktop child frames.        *
\*                                                                                      */
        while((pSmh->hwnd!=HookParameters.hwndFrame) && (pSmh->hwnd!=HookParameters.hwndSessionFrame))
            {
/*                                                                                      *\
 * HW Panning support allows to limit windows to a predifined size (during creation,    *
 * resizing and maximization). This allows one to use the HW Panning support which is   *
 * available at least for IBM ThinkPads in a usuable way. HW Panning runs the logical   *
 * video resolution up to the available video memory (e.g. 1024*768*250) on my ThinkPad *
 * 701C, while the TFT LCD display supports only an VGA (640*480*256) sized area. You   *
 * will get a hardware supported, and thus very fast, Virtual Desktop.                  *
 * We have to check for non-sizeable windows, that is not having the FS_SIZEBORDER      *
 * frame style, because though we could resize that windows, the user may no longer be  *
 * able to operate them (and e.g. the WPS has no sizable border).                       *
\*                                                                                      */
                                        /* Share use of variables to reduce stack requirements */
            SWP     swpSessionBar;
            ULONG   ulSwpFlag=0;
            LONG    lChangeA=0;
            LONG    lChangeB=0;
            LONG    lAdjust=0;
            SHORT   sYMinimize=0;

                                        /* Seamless WIN-OS2 windows require special treatment, so we
                                           better know this */
            WinQueryClassName(WinWindowFromID(pSmh->hwnd, FID_CLIENT), sizeof(ucClientClassname), ucClientClassname);
/*                                                                                      *\
 * Seamless WIN-OS2 windows support.                                                    *
\*                                                                                      */
            if(!strcmp(ucClientClassname, SHIELD_SEAMLESS))
                {
                SWP     swpSeamless=*pswpWindow;

                if((HookParameters.ulStatusFlag2 & HWVIRTUALDESKTOP) &&
                    (WinQueryWindowULong(pSmh->hwnd, QWL_STYLE) & FS_SIZEBORDER))
                    {
                                        /* If the window is the PM "shadow" frame window of a seamless WIN-OS2
                                           window, replacing the proposed frame window size causes that the PM
                                           "shadow" window and therefore the input focus area get reduced, but
                                           the graphics output of the "lower" GRE level keeps the old size. This
                                           causes a unoperateable Desktop */
                    if((!LookupMTEName(pSmh->hwnd, HookParameters.ucHWVirtualDesktopException)) &&
                        (pSmh->msg==WM_WINDOWPOSCHANGED))
                        {
                        if(swpSeamless.cx>(LONG)HookParameters.ulHWSizeX)
                            {
                            swpSeamless.cx=HookParameters.ulHWSizeX;
                            ulSwpFlag=TRUE;
                            }
                        if(swpSeamless.cy>(LONG)HookParameters.ulHWSizeY)
                            {
                            swpSeamless.cy=HookParameters.ulHWSizeY;
                            ulSwpFlag=TRUE;
                            }
                        }
                    }
                                        /* Adjust window position only if SessionBar is at
                                           top or bottom of screen */
                while(!(HookParameters.ulSessionBarFlag & SB_POSITIONANY))
                    {
                    if((swpSeamless.cx>0) && (swpSeamless.cy>0))
                        {
                        swpSessionBar=HookParameters.swpPC2Session;
                                        /* Query current values */
                        lChangeA=WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER);
                        lChangeB=WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
                                        /* We do our adjustments only if the SessionBar is on top
                                           or at the bottom of the screen (which can be changed by
                                           dragging the SessionBar with mouse button 2) */
                        if(HookParameters.ulSessionBarFlag & SB_POSITIONBOTTOM)
                            swpSessionBar.y-=(lChangeA);
                        else if(HookParameters.ulSessionBarFlag & SB_POSITIONTOP)
                            swpSessionBar.cy+=(lChangeA+lChangeB);
                                        /* Adjust the SessionBar's position by adding a sizing border,
                                           because when the frame of a window is outside the screen
                                           (maximized windows), we want to do the adjustments in this
                                           case too.
                                           Increase SessionBar frame by border height (and titlebar
                                           height) to avoid windows to be moved "below" the SessionBar
                                           (for vertically correction already done above) */
                        swpSessionBar.x-=lChangeA;
                        swpSessionBar.cx+=(lChangeA<<1);
                        if(pswpWindow->fl & SWP_MOVE)
                            {
                                        /* Get the overlapping between the new proposed position and
                                           the SessionBar and remove the overlapping */
                            lAdjust=AdjustOverlap(&swpSessionBar, &swpSeamless);
                            if(lAdjust!=0)
                                {
                                ulSwpFlag=TRUE;
                                        /* Remove overlapping */
                                swpSeamless.y+=lAdjust;
                                        /* If the SessionBar is overlapped, also adjust for the
                                           a sizing border, so that the sizing border is just above
                                           the SessionBar */
                                if(lAdjust>0)
                                    swpSeamless.y+=lChangeA;
                                }
                            }
                        if(pswpWindow->fl & SWP_SIZE)
                            {
                                        /* Get the overlapping between the new proposed size and the
                                           SessionBar and remove the overlapping */
                            lAdjust=AdjustOverlap(&swpSessionBar, &swpSeamless);
                            if(lAdjust!=0)
                                {
                                ulSwpFlag=TRUE;
                                        /* If the SessionBar is on bottom of the screen, try to keep the
                                           window sized to be completely visible */
                                swpSeamless.y+=lAdjust;
                                lAdjust=swpSeamless.y+swpSeamless.cy-HookParameters.swpScreen.cy;
                                if(lAdjust>0)
                                    swpSeamless.cy-=lAdjust;
                                }
                            }
                                        /* For minimizing and maximizing we have to check for WM_WINDOWPOSCHANGED,
                                           because during WM_ADJUSTWINDOWPOS to "old" previous position is
                                           passed not the new proposed one */
                        if(pSmh->msg==WM_WINDOWPOSCHANGED)
                            {
                                        /* We can detect a maxmized seamless WIN-OS2 window only by its
                                           style, as the SWP_MAXIMIZED flag seems not to be set */
                            if(WinQueryWindowULong(pSmh->hwnd, QWL_STYLE) & WS_MAXIMIZED)
                                {
                                        /* If the SessionBar should not be kept on top of Z-Order, then
                                           don't do any size and position adjustments if they are part
                                           of a maximization operation. In other words, the SessionBar
                                           allows maximized windows to cover it, though it will prevent
                                           restored window to cover it */
                                if(!(HookParameters.ulStatusFlag2 & KEEPONTOPSB))
                                    break;
                                        /* If the maximized window is outside the screen, then we can assume
                                           that we haven't yet restricted is with the HW Panning Support */
                                if(swpSeamless.y<0)
                                    {
                                    swpSeamless.x=0;
                                    swpSeamless.cx=HookParameters.swpScreen.cx;
                                    swpSeamless.y=0;
                                    swpSeamless.cy=HookParameters.swpScreen.cy;
                                    }
                                lAdjust=AdjustOverlap(&swpSessionBar, &swpSeamless);
                                        /* Keep maximized window outside of the SessionBar. If the
                                           SessionBar is on top, reduce or move the window downwards.
                                           If the SessionBar is on bottom, reduce and move the window
                                           upwards and take the sizing border into account */
                                if(HookParameters.ulSessionBarFlag & SB_POSITIONTOP)
                                    {
                                    if(lAdjust)
                                        {
                                        swpSeamless.cy-=(HookParameters.swpPC2Session.cy);
                                        ulSwpFlag=TRUE;
                                        break;
                                        }
                                    }
                                else
                                    {
                                    if(lAdjust)
                                        {
                                        swpSeamless.y=HookParameters.swpPC2Session.cy-1;
                                        swpSeamless.cy=HookParameters.swpScreen.cy-
                                            HookParameters.swpPC2Session.cy+1;
                                        ulSwpFlag=TRUE;
                                        break;
                                        }
                                    }
                                }
                                        /* If the window gets minimized while the SessionBar is at
                                           the bottom, move icon and icontext upwards. Just be sure
                                           to save the "old" vertical position into the window words,
                                           otherwise, each mimimization will add the SessionBar height
                                           another time */
                            if((pswpWindow->fl & SWP_MINIMIZE) && (HookParameters.ulSessionBarFlag & SB_POSITIONBOTTOM))
                                {
                                sYMinimize=WinQueryWindowUShort(pSmh->hwnd, QWS_YMINIMIZE);
                                if(sYMinimize<=swpSessionBar.cy)
                                    {
                                    swpSeamless.y=sYMinimize+swpSessionBar.cy;
                                        /* Unfortunately when minimizing a window, the message
                                           WM_ADJUSTWINDOWPOS is sent before the window is minimized,
                                           so we have to ensure the new position ourselves (instead
                                           of just adding the offset and returning) */
                                    WinSetWindowPos(pSmh->hwnd, 0, swpSeamless.x, swpSeamless.y,
                                        swpSeamless.cx, swpSeamless.cy, SWP_MOVE);
                                    WinSetWindowUShort(pSmh->hwnd, QWS_YMINIMIZE, sYMinimize);
                                    }
                                        /* No need for further processing */
                                return;
                                }
                            }
                        }

                                        /* Exit loop */
                    break;
                    }
                                        /* If changes have been made let the main thread apply them */
                if(ulSwpFlag!=0)
                    {
                    WinPostMsg(HookParameters.hwndClient, WM_SEAMLESSADJUST,
                        MPFROM2SHORT((SHORT)swpSeamless.x, (SHORT)swpSeamless.y),
                        MPFROM2SHORT((SHORT)swpSeamless.cx, (SHORT)swpSeamless.cy));
                    WinPostMsg(HookParameters.hwndClient, WM_SEAMLESSADJUST,
                        MPFROMHWND(pSmh->hwnd), NULL);
                    }
                break;
                }
/*                                                                                      *\
 * PM windows support.                                                                  *
\*                                                                                      */
                                        /* For all Desktop child frame windows (except the Desktop itself)
                                           limit the size of the window when the user has selected to use
                                           the hardware panning support, which is supported e.g. by IBM
                                           ThinkPads. Note that if just WM_WINDOWPOSCHANGED is used, folders
                                           will be scrolled down 1 page, so we check both messages */
            if((HookParameters.ulStatusFlag2 & HWVIRTUALDESKTOP) &&
                (WinQueryWindowULong(pSmh->hwnd, QWL_STYLE) & FS_SIZEBORDER))
                {
                                        /* See if the current process' executable is on the HW Virutal Desktop
                                           exception list, in which case we don't restrict the size of the
                                           to windows of this process (except the Window List, which is not
                                           manipulated anyway).
                                           For resizing, checking for WM_ADJUSTWINDOWPOS is sufficient, however
                                           during maximization, only WM_WINDOWPOSADJUSTED is sent, so we have
                                           to trap both (which is not a problem anyway, as once the size
                                           has been corrected, we won't ajust it again) */
                if((!LookupMTEName(pSmh->hwnd, HookParameters.ucHWVirtualDesktopException)) &&
                    (pSmh->hwnd!=HookParameters.hwndWindowList))
                    {
                    if(pswpWindow->cx>(LONG)HookParameters.ulHWSizeX)
                        {
                        lChangeA=(LONG)WinQueryWindowUShort(pSmh->hwnd, QWS_XRESTORE);
                        lChangeB=(LONG)WinQueryWindowUShort(pSmh->hwnd, QWS_CXRESTORE);
                        pswpWindow->cx=HookParameters.ulHWSizeX;
                        lAdjust=pswpWindow->x=lChangeA-((pswpWindow->cx-lChangeB)/2);
                        if(lAdjust<0)
                            pswpWindow->x=0;
                        lAdjust=HookParameters.DesktopSize.x-(pswpWindow->x+pswpWindow->cx);
                        if(lAdjust<0)
                            pswpWindow->x+=lAdjust;
                        ulSwpFlag|=(SWP_SIZE|SWP_MOVE);
                        }
                    if(pswpWindow->cy>(LONG)HookParameters.ulHWSizeY)
                        {
                        lChangeA=(LONG)WinQueryWindowUShort(pSmh->hwnd, QWS_YRESTORE);
                        lChangeB=(LONG)WinQueryWindowUShort(pSmh->hwnd, QWS_CYRESTORE);
                        pswpWindow->cy=HookParameters.ulHWSizeY;
                        lAdjust=pswpWindow->y=lChangeA-((pswpWindow->cy-lChangeB)/2);
                        if(lAdjust<0)
                            pswpWindow->y=0;
                        lAdjust=HookParameters.DesktopSize.y-(pswpWindow->y+pswpWindow->cy);
                        if(lAdjust<0)
                            pswpWindow->y+=lAdjust;
                        ulSwpFlag|=(SWP_SIZE|SWP_MOVE);
                        }
                    if(ulSwpFlag)
                        {
                        WinSetWindowPos(pSmh->hwnd, 0, pswpWindow->x, pswpWindow->y,
                            pswpWindow->cx, pswpWindow->cy, ulSwpFlag);
                        return;
                        }
                    }
                }
/*                                                                                      *\
 * If the SessionBar is active and set onto top of Z-Order, try to keep all windows at  *
 * a size and/or position, that the window does not get overlapped by the SessionBar    *
 * and keeps maximum visibility. We are only interested in Desktop child frames.        *
 * When running PC/2 as the WPS we may not know the WPS's handle during its creation,   *
 * so we have to explicitely look for the NLS name.                                     *
\*                                                                                      */
                                        /* Adjust window position (except WPS) only if SessionBar is at
                                           top or bottom of screen and not dynamic */
            while((!(HookParameters.ulSessionBarFlag & SB_POSITIONANY)) &&
                (!(HookParameters.ulStatusFlag2 & DYNAMICSESSIONBAR)))
                {
                                        /* Initially I detected if the current window was the WPS here
                                           by querying the window text and comparing it to the Desktop's
                                           name. Unfortunately this did trap SmartSuite 97's Lotus 1-2-3
                                           (likely due to some bad coding there), so I changed to detect
                                           the WPS (re)initialization by checking texts to be set into
                                           frame windows */
                if(pSmh->hwnd==HookParameters.hwndWPSFrame)
                    break;
                                        /* If the SessionBar should not be kept on top of Z-Order, then
                                           don't do any size and position adjustments if they are part
                                           of a maximization operation. In other words, the SessionBar
                                           allows maximized windows to cover it, though it will prevent
                                           restored window to cover it */
                if(!(HookParameters.ulStatusFlag2 & KEEPONTOPSB) &&
                    (pswpWindow->fl & SWP_MAXIMIZE))
                    break;
                                        /* We adjust the window's position for moving or sizing before
                                           the window gets set into the proposed position, by changing
                                           the proposed position in WM_ADJUSTWINDOWPOS. Unfortunately
                                           while this message is sent some windows (e.g. the
                                           printer settings dialogs) are 0 sized and this message
                                           does not occur when the window is no longer 0 sized.
                                           So we have to process also the WM_WINDOWPOSCHANGED
                                           message to correct the window's position and size
                                           after it has been moved or sized */
                if((pswpWindow->cx>0) && (pswpWindow->cy>0))
                    {
                    swpSessionBar=HookParameters.swpPC2Session;
                                        /* Query current values */
                    lChangeA=WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER);
                    lChangeB=WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
                                        /* We do our adjustments only if the SessionBar is on top
                                           or at the bottom of the screen (which can be changed by
                                           dragging the SessionBar with mouse button 2) */
                    if(HookParameters.ulSessionBarFlag & SB_POSITIONBOTTOM)
                        swpSessionBar.y-=(lChangeA);
                    else if(HookParameters.ulSessionBarFlag & SB_POSITIONTOP)
                        swpSessionBar.cy+=(lChangeA+lChangeB);
                                        /* Adjust the SessionBar's position by adding a sizing border,
                                           because when the frame of a window is outside the screen
                                           (maximized windows), we want to do the adjustments in this
                                           case too.
                                           Increase SessionBar frame by border height (and titlebar
                                           height) to avoid windows to be moved "below" the SessionBar
                                           (for vertically correction already done above) */
                    swpSessionBar.x-=lChangeA;
                    swpSessionBar.cx+=(lChangeA<<1);
                                        /* For minimizing and maximizing we have to check for WM_WINDOWPOSCHANGED,
                                           because during WM_ADJUSTWINDOWPOS to "old" previous position is
                                           passed not the new proposed one */
                    if(pSmh->msg==WM_WINDOWPOSCHANGED)
                        {
                                        /* If the window gets minimized while the SessionBar is at
                                           the bottom, move icon and icontext upwards. Just be sure
                                           to save the "old" vertical position into the window words,
                                           otherwise, each mimimization will add the SessionBar height
                                           another time */
                        if((pswpWindow->fl & SWP_MINIMIZE) && (HookParameters.ulSessionBarFlag & SB_POSITIONBOTTOM))
                            {
                                        /* We have to use WinSetWindowPos() instead of modifying
                                           pswpWindow directly, because to restore the previous
                                           position into the window words */
                            sYMinimize=WinQueryWindowUShort(pSmh->hwnd, QWS_YMINIMIZE);
                            if(sYMinimize<swpSessionBar.cy)
                                {
                                pswpWindow->y=sYMinimize+swpSessionBar.cy;
                                        /* Unfortunately when mainimizing a window, the message
                                           WM_ADJUSTWINDOWPOS is sent before the window is minimized,
                                           so we have to ensure this ourselves */
                                WinSetWindowPos(pSmh->hwnd, 0, pswpWindow->x, pswpWindow->y,
                                    pswpWindow->cx, pswpWindow->cy, SWP_MOVE);
                                WinSetWindowUShort(pSmh->hwnd, QWS_YMINIMIZE, sYMinimize);
                                }
                                        /* No need for further processing */
                            break;
                            }
                                        /* If the window gets maximized, we adjust its position after
                                           it has been calculated, but before it gets visible there,
                                           by correcting the calculated position. We must test for
                                           maximized windows before we check for moved and resized windows,
                                           otherwise (e.g. for EPM's search dialog) WinSetMultWindowPos()
                                           call would remove (probably because this API synchronously sends some
                                           WM_ADJUSTWINDOWPOS and WM_WINDOWPOSADJUSTED messages, before we have
                                           a chance to make the maximization adjustments) the changes we made
                                           for the maximized window will not become effective */
                        if(pswpWindow->fl & SWP_MAXIMIZE)
                            {
                                        /* For some unknown reasons, moving a window outside the
                                           screen on the left and/or bottom side causes positions
                                           of smaller than (SHORT)-1 instead of (LONG)-1, which
                                           of course causes problems as (LONG)((SHORT)-1)>0L. This
                                           only applies to 2.10 and 2.11 (2.0 ?), but works with
                                           LONGs as expected for Warp+ */
                            if(pswpWindow->x & 0x00008000)
                                pswpWindow->x|=0xFFFF0000;
                            if(pswpWindow->y & 0x00008000)
                                pswpWindow->y|=0xFFFF0000;
                                        /* Get the overlapping between the new proposed position and
                                           size and the SessionBar and remove the overlapping */
                            lAdjust=AdjustOverlap(&swpSessionBar, pswpWindow);
                            ulSwpFlag|=(SWP_MOVE|SWP_SIZE);
                                        /* Keep maximized window outside of the SessionBar. If the
                                           SessionBar is on top, reduce or move the window downwards.
                                           If the SessionBar is on bottom, reduce and move the window
                                           upwards and take the sizing border into account */
                            if(HookParameters.ulSessionBarFlag & SB_POSITIONTOP)
                                {
                                if(lAdjust)
                                    {
                                    lAdjust=(HookParameters.swpPC2Session.cy+2);
                                    if(pswpWindow->y>lAdjust)
                                        pswpWindow->y-=lAdjust;
                                    else
                                        pswpWindow->cy-=lAdjust;
                                    }
                                }
                            else
                                {
                                if(lAdjust)
                                    {
                                    pswpWindow->y=HookParameters.swpPC2Session.cy-1;
                                    pswpWindow->cy=HookParameters.swpScreen.cy-
                                        HookParameters.swpPC2Session.cy+1;
                                    }
                                }
                                        /* Unfortunately when maximizing a window, the message
                                           WM_ADJUSTWINDOWPOS is sent before the window is maximized,
                                           so our replacement logic would not take place. We therefore
                                           have to reposition/resize the maximized window while
                                           processing WM_WINDOWPOSCHANGED */
                            WinSetWindowPos(pSmh->hwnd, 0, pswpWindow->x, pswpWindow->y,
                                pswpWindow->cx, pswpWindow->cy, ulSwpFlag);
                            return;
                            }
                        }
                                        /* Checking for SWP_MOVE and SWP_SIZE in an
                                           if(pSmh->msg==WM_ADJUSTWINDOWPOS) does not work for EPM's
                                           search dialog and some printer job properties, so we have
                                           to accept the overhead */
                    if(pswpWindow->fl & SWP_MOVE)
                        {
                                        /* Get the overlapping between the new proposed position and
                                           the SessionBar and remove the overlapping */
                        lAdjust=AdjustOverlap(&swpSessionBar, pswpWindow);
                        if(lAdjust!=0)
                            {
                                        /* Remove overlapping */
                            pswpWindow->y+=lAdjust;
                                        /* If the SessionBar is overlapped, also adjust for the
                                           a sizing border, so that the sizing border is just above
                                           the SessionBar */
                            if(lAdjust>0)
                                pswpWindow->y+=lChangeA;
                                        /* Unfortunately e.g. for the some WPS printer job properties, or
                                           EPM's search dialog, only a WM_WINDOWPOSCHANGED is sent where
                                           modification of the SWP structure without a WinSetWindowPos()
                                           does not suffice */
                            if(pSmh->msg==WM_WINDOWPOSCHANGED)
                                WinSetMultWindowPos(WinQueryAnchorBlock(pSmh->hwnd), pswpWindow, 1);
                            }
                        }
                    if(pswpWindow->fl & SWP_SIZE)
                        {
                                        /* Get the overlapping between the new proposed size and the
                                           SessionBar and remove the overlapping */
                        lAdjust=AdjustOverlap(&swpSessionBar, pswpWindow);
                        if(lAdjust!=0)
                            {
                                        /* If the SessionBar is on bottom of the screen, try to keep the
                                           window sized to be completely visible */
                            pswpWindow->y+=lAdjust;
                            lAdjust=pswpWindow->y+pswpWindow->cy-HookParameters.swpScreen.cy;
                            if(lAdjust>0)
                                pswpWindow->cy-=lAdjust;
                            }
                        }
                    }
                                        /* Exit loop */
                break;
                }
                                        /* Initially I had instead of above code here the statements
                                           if((pSmh->msg==WM_WINDOWPOSCHANGED) && (ulSwpFlag))
                                               WinSetWindowPos(pSmh->hwnd, 0, pswpWindow->x, pswpWindow->y,
                                                   pswpWindow->cx, pswpWindow->cy, ulSwpFlag);
                                           here, but this caused the active window to be moved
                                           to the newly activated Desktop when switching between
                                           Virtual Desktops (likely recursively as sometimes windows
                                           ran out of stack) */
                                        /* Exit loop */
            break;
            }
/*                                                                                      *\
 * Track changing of windows positions (even if we don't test for the change of Z-order *
 * we're only interested in) and tracking of windows, if these windows are childs of    *
 * the Desktop. If PC/2's Overview window, the SessionBar and QuickSwitch window should *
 * be on top of Z-order, this code segement will ensure it, unless the window selected  *
 * by the user is the Window List, or PC/2's LockupDialog. If the QuickSwitch window is *
 * visible, keep this on top of Z-order.                                                *
 * When the focus window gets destroyed, PM switches the focus to the new window        *
 * topmost of Z-Order, which when being the SessionBar, shows no window with active     *
 * titlebar! Therefore the PC/2 SessionBar traps the WM_FOCUSCHANGE and removes the     *
 * focus from itself and activates the next lower window (except the PC/2 Overview      *
 * window) in Z-Order.                                                                  *
\*                                                                                      */
                                        /* Not restricting to WM_ADJUSTWINDOWPOS messages (that is also
                                           trapping WM_WINDOWPOSCHANGED), ensures that clicking on the client
                                           area of OS/2 VIO windows keeps that window behind PC/2's windows,
                                           which otherwise doesn't work (for unknown reasons for me, as
                                           for PM window clicking on the client works well)
                                           && (pSmh->msg==WM_ADJUSTWINDOWPOS)) */
        while(((SWP *)(pSmh->mp1))->hwndInsertBehind==HWND_TOP)
            {
                                        /* Variables on stack because our window Z-Order manipulation may
                                           cause recursive calls */
                                        /* Cache current window we are processing */
            HWND    hwndLookup;
                                        /* Index in the array of windows managed by PC/2 */
            LONG    lIndex;
                                        /* Index of a window managed by PC/2 being the bottommost one
                                           in Z-Order */
            LONG    lIndexBottom;
                                        /* Index of a window managed by PC/2 being the topmost one
                                           in Z-Order */
            LONG    lIndexTop;
                                        /* Index of a window managed by PC/2 to find a match with the
                                           current window */
            LONG    lIndexManaged;

                                        /* Even if we test for frame windows, the frame window would
                                           overlap the Popup Menu, the WPS menu or the Window List
                                           popup menu as the frame would put itself onto top of Z-Order
                                           anyway if we don't prevent it after a menu gets activated */
            if(!strcmp(ucClassname, "#4"))
                break;
                                        /* Cache access (on stack due to possible recursion) */
            hwndLookup=pSmh->hwnd;
                                        /* Prevent Seamless WIN-OS2 sessions to be modified in their Z-Order,
                                           as the underlying PM window can be influenced, but the graphics
                                           output seems to be routed to a "lower" level of the GRE, which
                                           causes PC/2 to draw "in parallel" to the WIN-OS2 session */
            WinQueryClassName(WinWindowFromID(hwndLookup, FID_CLIENT), sizeof(ucClientClassname), ucClientClassname);
            if(!strcmp(ucClientClassname, SHIELD_SEAMLESS))
                break;
                                        /* Let's keep the PC/2 Lockup dialog and the Window List on
                                           top of Z-Order */
            if((hwndLookup==HookParameters.hwndWindowList) || (hwndLookup==HookParameters.hwndLockup))
                break;
                                        /* Clear index of windows managed by PC/2 that is bottommost
                                           and topmost in Z-Order */
            lIndexManaged=lIndexBottom=lIndexTop=ZORDER_TOP;
                                        /* Lookup for the current frame window for one of PC/2's
                                           windows that should stay on top of Z-Order. The lowest index
                                           which is not NULLHANDLE, is the PC/2 window we insert the
                                           current window behind */
            for(lIndex=0; lIndex!=ZORDER_TOP; lIndex++)
                {
                                        /* If the current entry is not valid (because the PC/2 window
                                           of this index is invisible/unavailable), skip it */
                if(HookParameters.hwndZOrder[lIndex]==NULLHANDLE)
                    continue;
                else
                    {
                                        /* This window is the topmost until we find one higher in the array */
                    lIndexTop=lIndex;
                                        /* If we found the bottommost window managed by PC/2 save its index */
                    if(lIndexBottom==ZORDER_TOP)
                        lIndexBottom=lIndex;
                    }
                                        /* If the current window is found in the array of windows managed
                                           by PC/2 (that is, OverView, SessionBar and QuickSwitch window),
                                           accept this match without changing anything */
                if(HookParameters.hwndZOrder[lIndex]==hwndLookup)
                    lIndexManaged=lIndex;
                }
                                        /* If none of the PC/2 managed windows should be kept on top, let
                                           the current application's window become top of Z-Order */
            if(lIndexTop==ZORDER_TOP)
                break;
                                        /* Now we've either found the window in the array of managed PC/2
                                           windows (lIndexManaged!=ZORDER_TOP), or we don't manage this window
                                           (lIndexManaged==ZORDER_TOP) that is it's none of the PC/2 created
                                           windows */
            if(lIndexManaged==ZORDER_TOP)
                {
                                        /* The current window is not part of the windows PC/2 keeps track
                                           of (that is, OverView, SessionBar and QuickSwitch window when
                                           active), to ensure that the Z-Order doesn't get changed by the user
                                           for them */
                SWP     swpSetZOrder;

                                        /* Get the window's owner window, until either one of the windows
                                           created by PC/2 is the owner, or there is no owner */
                hwndActiveWindowOwner=WinQueryWindow(pSmh->hwnd, QW_OWNER);
                while(hwndActiveWindowOwner!=NULLHANDLE)
                    {
                                        /* If one of PC/2's windows is the owner (e.g. PC/2's online help), we
                                           don't change the z-order, because this window is top of Z-Order to
                                           its owner, and PC/2's Overview window can't be on top of Z-Order
                                           relative to its owned windows. Ignoring this eats up CPU in 2.11 (as
                                           the message queue gets called recursively) and traps or causes
                                           IPE's in Warp */
                    for(lIndex=0; lIndex!=ZORDER_TOP; lIndex++)
                        {
                        if((HookParameters.hwndZOrder[lIndex]!=NULLHANDLE) &&
                            (hwndActiveWindowOwner==HookParameters.hwndZOrder[lIndex]))
                            return;
                        }
                    hwndLookup=hwndActiveWindowOwner;
                    hwndActiveWindowOwner=WinQueryWindow(hwndLookup, QW_OWNER);
                    }
                                        /* If the PC/2 window that should be topmost in Z-Order is
                                           not on top, set it to top. However it seems that does not
                                           work when a systemmodal window (e.g. Lockup) sets itself on top
                                           of Z-Order. A trace has shown me that though PC/2 windows set
                                           themselves on top of Z-Order they don't get there when querying
                                           the topmost window immediately after setting it. The API
                                           WinSetMultWindowPos() doesn't fail, so it seems to be the PM
                                           behaviour we've to accept (which isn't a problem anyway) */
                if(WinQueryWindow(HWND_DESKTOP, QW_TOP)!=HookParameters.hwndZOrder[lIndexTop])
                    {
                    memset(&swpSetZOrder, 0, sizeof(swpSetZOrder));
                    swpSetZOrder.fl=SWP_ZORDER;
                    swpSetZOrder.hwndInsertBehind=HWND_TOP;
                    swpSetZOrder.hwnd=HookParameters.hwndZOrder[lIndexTop];
                    WinSetMultWindowPos(WinQueryAnchorBlock(swpSetZOrder.hwnd), &swpSetZOrder, 1);
                    }
                                        /* As the current window is none of the window PC/2 manages and
                                           not a window owned by PC/2's Overview window, set it behind
                                           the bottommost window in Z-Order */
                lIndexManaged=lIndexBottom;
                }
            else
                {
                                        /* The current window is one of the windows PC/2 keeps track of
                                           (that is, OverView, SessionBar and QuickSwitch window when
                                           active), so insert window behind the one PC/2 manages to be the
                                           next. Prevent recursion by avoiding calling WinSetWindowPos()
                                           for the window's context we are currently running in */
                if(HookParameters.hwndZOrder[lIndexTop]!=hwndLookup)
                    {
                                        /* If the PC/2 window that should be on top of Z-Order is not
                                           on top set window to top */
                    if(WinQueryWindow(HWND_DESKTOP, QW_TOP)!=HookParameters.hwndZOrder[lIndexTop])
                        WinSetWindowPos(HookParameters.hwndZOrder[lIndexTop], HWND_TOP, 0, 0, 0, 0, SWP_ZORDER);
                    }
                do  {
                                        /* Insert the current window behind to next window created by PC/2
                                           higher in Z-Order. If the next window index is invalid/unavailable, just
                                           take the next one, until we find one or got HWND_TOP as the final resort */
                    lIndexManaged++;
                    if(lIndexManaged>=ZORDER_TOP)
                        break;
                    } while(HookParameters.hwndZOrder[lIndexManaged]==NULLHANDLE);
                }
                                        /* Adjust the current window's Z-Order */
            ((SWP *)(pSmh->mp1))->hwndInsertBehind=HookParameters.hwndZOrder[lIndexManaged];
            ((SWP *)(pSmh->mp1))->fl|=SWP_ZORDER;
                                        /* Get out of while loop anyway */
            break;
            }
                                        /* Post this message to PC/2 because to reflect a frame
                                           window's creation, resizing, moving or destruction */
        if(pSmh->msg==WM_WINDOWPOSCHANGED)
            WinPostMsg(HookParameters.hwndClient, WM_SETUPSIZEPOSITION,
                MPFROMLONG(WM_MOVE), MPFROMHWND(pSmh->hwnd));
        }
    }
}

/*--------------------------------------------------------------------------------------*\
 * This procedure tries to locate the current executables MTE name in a list, where     *
 * the name of processes is separated by \r\n. This function must be run in the address *
 * space of the process which we want to find in the exception list.                    *
 * Req:                                                                                 *
 *      hwndFrame ....  The window handle of the window of interest (only required to   *
 *                      test for command windows, as these windows run in the PM        *
 *                      process, that is PMSHELL, and adding PMSHELL into the exception *
 *                      list would also cause the WPS windows to match)                 *
 *      pucProcessList  The list of processes separated by \r\n                         *
 * Ret:                                                                                 *
 *      bFound ........ TRUE if current executable could be matched, FALSE otherwise    *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
BOOL        LookupMTEName(HWND hwndFrame, UCHAR *pucProcessList)
{
PTIB        ptibFrame;
PPIB        ppibFrame;
PID         pidFrame;
TID         tidFrame;
UCHAR       *pucProcessName;
UCHAR       *pucTemp;
ULONG       ulProcessNameSize;
UCHAR       ucProcessName[CCHMAXPATH];

WinQueryClassName(WinWindowFromID(hwndFrame, FID_CLIENT), sizeof(ucClientClassname), ucClientClassname);
if(!strcmp(ucClientClassname, SHIELD_AVIO))
    {
                                        /* If the window is a OS/2 command processor window, then replace
                                           PMSHELL (which we would get from DosGetInfoBlocks()) by CMD */
    strcpy(ucProcessName, "CMD\r");
    }
else if(!strcmp(ucClientClassname, SHIELD_SEAMLESS))
    {
                                        /* If the window is a seamless WIN-OS2 session, then replace
                                           PMSHELL (which we would get from DosGetInfoBlocks()) by SEAMLESS */
    strcpy(ucProcessName, "SEAMLESS\r");
    }
else
    {
                                        /* Get the process and thread id that created the frame window,
                                           as the messages during whose processing we are called may
                                           run in the context of the sending window, ensure we know the
                                           window's process exactly */
    WinQueryWindowProcess(hwndFrame, &pidFrame, &tidFrame);
                                        /* See if the current process' executable is contained in the
                                           Exception list, in which case we return our successful match,
                                           of this process. The name of the process can be found in the process
                                           info block at pib_pchcmd, where it is the first ASCIIZ string
                                           of the commandline passed to the executable by the OS/2 loader,
                                           which may (not must) contain the fully qualified path, but is case
                                           respecting compared to DosQueryModuleName() which on the other hand
                                           allways returns the fully qualified path. For the exception list
                                           we are only interested in the case sensitive executable */
    DosGetInfoBlocks(&ptibFrame, &ppibFrame);
                                        /* If we don't process in the thread context of the frame window,
                                           then we can't look for a match (as we would search for the
                                           sending window's process not of the receiving one), then
                                           return a match.
                                           A match, because sizing with the sizing border runs in the
                                           correct thread context, and when the message results from
                                           a different thread context it's unlikely a sizing window
                                           (unless e.g. caused by PC/2's Overview window).
                                           Unfortunately, there is no way telling the executable that
                                           has created a window unless a list is made, which has the
                                           problem that it must load before PM (or some windows may
                                           missing) */
    if(ppibFrame->pib_ulpid!=pidFrame)
        return(TRUE);
    pucProcessName=strrchr(ppibFrame->pib_pchcmd, '\\');
    if(pucProcessName==0)
        pucProcessName=strrchr(ppibFrame->pib_pchcmd, ':');
    if(pucProcessName==0)
        pucProcessName=ppibFrame->pib_pchcmd;
    else
        pucProcessName++;
#ifdef  DEBUG_PC2HOOK
    if(pucProcessName)
        {
        UCHAR *puC=pucProcessName;
        int     ii;

        WinPostMsg(HookParameters.hwndClient, WM_USER+1000, (MPARAM)hwndFrame, (MPARAM)0x00020002);
        for(ii=0; ii<1; ii++)
            {
            WinPostMsg(HookParameters.hwndClient, WM_USER+1000, (MPARAM)*((ULONG *)puC), (MPARAM)0x00030003);
            puC+=4;
            }
        }
    else
        DosBeep(1000, 500);
#endif  /* DEBUG_PC2HOOK */
                                        /* Don't return success for a process whose name can't be determined */
    if(pucProcessName==0)
        return(FALSE);
                                        /* The executable's name (without the file extension), but terminated
                                           by a \r is used to match with PC/2's Exception List, which is
                                           ensured to be terminated by \r\n. The match of filename\r ensures that
                                           no substrings match with an entry in the Exception List
                                           (as each executable is terminated by \r\n) */
    pucTemp=strchr(pucProcessName, '.');
    if(pucTemp==0)
        pucTemp=strchr(pucProcessName, '\0');
    ulProcessNameSize=(ULONG)pucTemp-(ULONG)pucProcessName;
    memset(ucProcessName, '\0', sizeof(ucProcessName));
    memcpy(ucProcessName, pucProcessName, ulProcessNameSize);
    ucProcessName[ulProcessNameSize++]='\r';
    ucProcessName[ulProcessNameSize]='\0';
    }
                                        /* Now that we know the current process' executable, see if it is
                                           in the Exception List. However ensure that we get exact matches,
                                           that is PMSHELL does not match PmShell, XPMSHELL or PMSHELLX,
                                           this is ensured by searching for PMSHELL\r with a strncmp()
                                           in the Exception List */
ulProcessNameSize=strlen(ucProcessName);
while((pucProcessList!=0) && (*pucProcessList!='\0'))
    {
    if(!strncmp(pucProcessList, ucProcessName, ulProcessNameSize))
        return(TRUE);
    pucProcessList=strstr(pucProcessList, "\r\n");
    if(pucProcessList!=0)
        pucProcessList+=2;
    }
return(FALSE);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure finds the FRAMELIST entry of an window handle passed in the array     *
 * framelistWindows.                                                                    *
 * Req:                                                                                 *
 *      hwnd .......... Frame window handle to find in framelistWindows                 *
 * Ret:                                                                                 *
 *      pframelist .... Pointer to the FRAMELIST entry corresponding the frame window   *
 *                      handle passed, or NULL if the entry was not found               *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
FRAMELIST   *LookupFrame(HWND hwndFrame)
{
ULONG       ulIndex;
FRAMELIST   *pframelistCurrent=NULL;

                                        /* Loop through all entries until we find the entry
                                           corresponding to the window handle passed */
for(ulIndex=0; ulIndex<MAX_FRAME; ulIndex++)
    if(framelistWindows[ulIndex].hwndFrame==hwndFrame)
        {
        pframelistCurrent=&framelistWindows[ulIndex];
        return(pframelistCurrent);
        }
return(NULL);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure adds or removes the current frame window handle into the array        *
 * framelistWindows.                                                                    *
 * Req:                                                                                 *
 *      hwnd .......... Frame window handle to add to or remove from framelistWindows   *
 *      bSet .......... TRUE if the window should be added, FALSE if the window should  *
 *                      be removed. TRUE will only be honored while the flag            *
 *                      TITLEBARICON is set in HookParameters.ulStatusFlag1.            *
 * Ret:                                                                                 *
 *      pframelist .... Pointer to the FRAMELIST entry corresponding the frame window   *
 *                      handle passed, or NULL if an error occured                      *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
FRAMELIST   *RegisterFrame(HWND hwndFrame, BOOL bSet)
{
ULONG       ulIndex;
FRAMELIST   *pframelistCurrent=NULL;

                                        /* First lets see if an entry already exists for the window */
pframelistCurrent=LookupFrame(hwndFrame);
if(pframelistCurrent==NULL)
    {
                                        /* If this window couldn't be found, then add it when required */
    if(bSet==TRUE)
        {
                                        /* Don't subclass the PM Hard Error and System Error dialogs */
        if((hwndFrame==HookParameters.hwndPMHardError) || (hwndFrame==HookParameters.hwndSystemError))
            return(NULL);
                                        /* See if the current process' executable is on the Titlebar
                                           exception list, in which case we don't add Titlebar Smarticons
                                           to windows of this process */
        if(LookupMTEName(hwndFrame, HookParameters.ucTitlebarException))
            return(NULL);
                                        /* Search for next free slot */
        for(ulIndex=0; ulIndex<MAX_FRAME; ulIndex++)
            if(framelistWindows[ulIndex].hwndFrame==NULLHANDLE)
                {
                pframelistCurrent=&framelistWindows[ulIndex];
                memset(pframelistCurrent, 0, sizeof(*pframelistCurrent));
                pframelistCurrent->hwndFrame=hwndFrame;
                pframelistCurrent->pfnwpFrame=WinSubclassWindow(hwndFrame, pfnwpFrameWindowProc);
                if(pframelistCurrent->pfnwpFrame==NULL)
                    return(NULL);
                pframelistCurrent->ulControlsFlag=HookParameters.ulTitlebarFlag;
                                        /* Createing the control does not update the titlebar */
                break;
                }
        }
    }
else
    {
                                        /* If this window could be found, then remove it when required */
    if(bSet==FALSE)
        {
        ULONG   ulIndex=0;

                                        /* Undo subclassing */
        if(pframelistCurrent->pfnwpFrame!=NULL)
            WinSubclassWindow(hwndFrame, pframelistCurrent->pfnwpFrame);
                                        /* Destroy resources */
        for(ulIndex=0; ulIndex<NUM_TITLEBARCONTROLS; ulIndex++)
            {
            if(pframelistCurrent->hwndControls[ulIndex]!=NULLHANDLE)
                WinDestroyWindow(pframelistCurrent->hwndControls[ulIndex]);
            if(pframelistCurrent->hbmControls0[ulIndex]!=NULLHANDLE)
                GpiDeleteBitmap(pframelistCurrent->hbmControls0[ulIndex]);
            if(pframelistCurrent->hbmControls1[ulIndex]!=NULLHANDLE)
                GpiDeleteBitmap(pframelistCurrent->hbmControls1[ulIndex]);
            }
        memset(pframelistCurrent, 0, sizeof(*pframelistCurrent));
        pframelistCurrent=NULL;
        }
    }
return(pframelistCurrent);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure blits a bitmap into a destination rectangle.                          *
 * Req:                                                                                 *
 *      hwndDestination The window to blit icon into                                    *
 *      hpsDestination  This presentation space to blit icon into                       *
 *      hbitmapBitmap . The bitmap's handle to be blitted                               *
 *      prectlBitamp .. The target rectangle to blit bitmap into                        *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: NO_ERROR no error, PMERR_* otherwise               *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   BlitBit(HWND hwndDestination, HPS hpsDestination, HBITMAP hbitmapBitmap, RECTL *prectlBitmap)
{
POINTL              ptlBlt[4];          /* Target and source points for blitting icon's bitmaps */
ULONG               ulRc=NO_ERROR;

ptlBlt[0].x=prectlBitmap->xLeft;
ptlBlt[0].y=prectlBitmap->yBottom;
ulRc=WinDrawBitmap(hpsDestination, hbitmapBitmap, NULL, &ptlBlt[0], 0, 0, DBM_NORMAL);
return(ulRc);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure is the subclassed frame window procedure for all frame windows we had *
 * subclassed to add Titlebar Smarticons into. The code is shared for all instances,    *
 * but we get our instance data by using the window handle as a lookup in our table.    *
 * Req:                                                                                 *
 *      hwndFrame ..... Frame window handle we're running in                            *
 *      msg ........... Message we're processing                                        *
 *      mp1 ........... Parameter 1                                                     *
 *      mp2 ........... Parameter 2                                                     *
 * Ret:                                                                                 *
 *      MRESULT ....... Window procedure return code (by default aquired from original  *
 *                      frame window procedure                                          *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
MRESULT EXPENTRY    pfnwpFrameWindowProc(HWND hwndFrame, ULONG msg, MPARAM mp1, MPARAM mp2)
{
                                        /* Up to now, I havan't found any reason to include a stack size
                                           check like ((ULONG)&ptib-((ULONG)ptib->tib_pstack)), where
                                           ptib is a automatic variable */

                                        /* Get the FRAMELIST entry that corresponds to this frame
                                           window whose window procedure we're part off */
FRAMELIST   *pframelistCurrent=0;
                                        /* Get the FRAMEDATA instance data of the subclassed frame
                                           window procedure to avoid using up stack for automatic
                                           data */
FRAMEDATA   *pframedataCurrent=0;
MRESULT     mresultCurrent=0;

                                        /* If we're running in the context of a window procedure, then we've
                                           subclassed this window and we expect that there exists a control
                                           structure FRAMELIST within framelistWindows[] representing this
                                           window */
if((pframelistCurrent=LookupFrame(hwndFrame))==NULL)
    return((MRESULT)TRUE);
                                        /* We need to allocate instance data for the window
                                           to avoid using stack space for our "automatic" data
                                           in the subclassed frame window procedure, as we
                                           may run out of stack during recursion */
if(DosAllocMem((VOID **)&pframedataCurrent, WND_DATA_LEN, PAG_COMMIT|PAG_READ|PAG_WRITE)!=NO_ERROR)
    return((MRESULT)TRUE);
mresultCurrent=pfnwpSubclassFrameWindowProc(pframelistCurrent, pframedataCurrent, hwndFrame, msg, mp1, mp2);
DosFreeMem(pframedataCurrent);
return(mresultCurrent);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure is the part of the subclassed frame window procedure. In addition to  *
 * the usual window procedure parameters, pointers to the frame window instance data is *
 * added to avoid using precious stack space, (as our requirements caused exceptions,   *
 * because of running out of stack during recursions.                                   *
 * Req:                                                                                 *
 *      pframelistCurrent                                                               *
 *                      Pointer to frame control data                                   *
 *      pframedataCurrent                                                               *
 *                      Pointer to frame instance data to avoid using precious stack    *
 *      hwndFrame ..... Frame window handle we're running in                            *
 *      msg ........... Message we're processing                                        *
 *      mp1 ........... Parameter 1                                                     *
 *      mp2 ........... Parameter 2                                                     *
 * Ret:                                                                                 *
 *      MRESULT ....... Window procedure return code (by default aquired from original  *
 *                      frame window procedure                                          *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
MRESULT EXPENTRY    pfnwpSubclassFrameWindowProc(FRAMELIST *pframelistCurrent, FRAMEDATA *pframedataCurrent, HWND hwndFrame, ULONG msg, MPARAM mp1, MPARAM mp2)
{

                                        /* Up to now, I havan't found any reason to include a stack size
                                           check like ((ULONG)&ptib-((ULONG)ptib->tib_pstack)), where
                                           ptib is a automatic variable */
switch(msg)
{
                                        /* I tried to look for WM_DESTROY messages here to undo our
                                           subclassing and deallocating the resources, however this
                                           message was never processed here. I assume that WM_DESTROY
                                           already shut down the window in a way subclassing no longer
                                           works. I therefore look in the WinSendMsg() hook for that
                                           message */
case WM_SYSCOMMAND:
                                        /* The buttons generate WM_SYSCOMMAND messages, but during WM_DESTROY
                                           of NetScape sending WM_SYSCOMMAND traps NetScape, but not when
                                           using a different message */
case WM_SMARTICONS:
    switch(SHORT1FROMMP(mp1))
    {
    case SC_DESTROY:
                                        /* Sent from WM_DESTROY message trapped in WinSendMsg() hook
                                           to allow our cleanup processing */
        RegisterFrame(hwndFrame, FALSE);
        return((MRESULT)hwndFrame);

    case SC_CONTROLSCLEANUP:
        {
                                        /* Remove the FRAMELIST controls structure for this window
                                           and remove the subclassing also. Then let the frame redraw
                                           as it was before PC/2 did its magic. This code has to be
                                           within the subclassed frame window procedure, because it
                                           can't be guaranteed how long the input hook is available
                                           during closing of PC/2 */
        RegisterFrame(hwndFrame, FALSE);
        WinPostMsg(hwndFrame, WM_UPDATEFRAME, (MPARAM)FCF_TITLEBAR, NULL);
        return((MRESULT)hwndFrame);
        }

    case SC_CONTROLSINITIALIZE:
        {
                                        /* Initialize the FRAMELIST control structure accompanying the
                                           frame window's additional titlebar controls added by PC/2 */
#if     0
                                        /* Delay initialization by reposting message. Tests have shown
                                           no siginificant differences */
        if(SHORT1FROMMP(mp2)==CMDSRC_OTHER)
            {
            WinPostMsg(hwndFrame, msg, mp1, MPFROM2SHORT(CMDSRC_PUSHBUTTON, FALSE));
            return((MRESULT)TRUE);
            }
#endif  /* 0 */
                                        /* Get a handle of PC2?Res.DLL for the thread context we're running
                                           in (as I think that each load request of a process increases a
                                           OS/2 internal reference count, and using the handle obtained by
                                           PC/2 itself may circumvent this) */
        DosLoadModule(pframedataCurrent->ucBuffer, sizeof(pframedataCurrent->ucBuffer)-1,
            (HookParameters.lIconSize==ICONSIZE_VGA ? "PC2VRes" : "PC2BRes"), &pframedataCurrent->hmoduleTemp);
                                        /* Get a presentation space */
        pframedataCurrent->hpsTemp=WinBeginPaint(hwndFrame, NULLHANDLE, NULL);
                                        /* Loop through all VGA controls (and add the offset for the
                                           toggled state and for BGA when required). Load the untoggled
                                           titlebar control bitmap and if a toggled state is also defined
                                           also the toggled titlebar control bitmap */
        pframedataCurrent->ulLoadIndex=ID_BITMAP0TITLEBARBEGIN;
        for(pframedataCurrent->ulToggleMask=FL_TITLEBARBEGIN, pframedataCurrent->ulIndex=0;
            pframedataCurrent->ulIndex<NUM_TITLEBARCONTROLS;
            pframedataCurrent->ulToggleMask<<=1, pframedataCurrent->ulIndex++, pframedataCurrent->ulLoadIndex++)
            {
                                        /* Load the untoggled bitmap in up state */
            pframelistCurrent->hbmControls0[pframedataCurrent->ulIndex]=GpiLoadBitmap(pframedataCurrent->hpsTemp,
                pframedataCurrent->hmoduleTemp, pframedataCurrent->ulLoadIndex, 0, 0);
                                        /* Load the untoggled bitmap in down state (for WARP 4) */
            if((HookParameters.ulVersionMajor==OS2_MAJOR) &&
                (HookParameters.ulVersionMinor>=OS2_MINOR_400))
                {
                pframelistCurrent->hbmControls2[pframedataCurrent->ulIndex]=GpiLoadBitmap(pframedataCurrent->hpsTemp,
                    pframedataCurrent->hmoduleTemp, (pframedataCurrent->ulLoadIndex+DOWNSTATE), 0, 0);
                }
                                        /* If appropriate, load the toggled bitmap */
            if(FL_TOGGLEMASK & pframedataCurrent->ulToggleMask)
                {
                                        /* Up state */
                pframelistCurrent->hbmControls1[pframedataCurrent->ulIndex]=GpiLoadBitmap(pframedataCurrent->hpsTemp,
                    pframedataCurrent->hmoduleTemp, (pframedataCurrent->ulLoadIndex+TOGGLESTATE), 0, 0);
                                        /* Down state (for WARP 4) */
                if((HookParameters.ulVersionMajor==OS2_MAJOR) &&
                    (HookParameters.ulVersionMinor>=OS2_MINOR_400))
                    {
                    pframelistCurrent->hbmControls3[pframedataCurrent->ulIndex]=GpiLoadBitmap(pframedataCurrent->hpsTemp,
                        pframedataCurrent->hmoduleTemp, (pframedataCurrent->ulLoadIndex+DOWNSTATE+TOGGLESTATE), 0, 0);
                    }
                }
            }
        WinEndPaint(pframedataCurrent->hpsTemp);
                                        /* Release handle of PC2?Res.DLL */
        DosFreeModule(pframedataCurrent->hmoduleTemp);
        pframelistCurrent->ulWindowFlag|=WINDOWSUBCLASSED;
        WinPostMsg(hwndFrame, WM_UPDATEFRAME, NULL, NULL);
        return((MRESULT)hwndFrame);
        }

    case SC_ROLLWINDOW:
        {
                                        /* Get frame and titlebar window size and position */
        WinQueryWindowPos(hwndFrame, &pframedataCurrent->swpFrame);
        WinQueryWindowPos(WinWindowFromID(hwndFrame, FID_TITLEBAR), &pframedataCurrent->swpTitlebar);
        pframedataCurrent->ulFrameFlags=WinQueryWindowULong(hwndFrame, QWL_STYLE);
        if(pframelistCurrent->ulStatusFlag & FL_ROLLWINDOW)
            {
                                        /* Restore height to value before window was rolled up */
            pframedataCurrent->swpFrame.y-=pframelistCurrent->lRollupDelta;
            pframedataCurrent->swpFrame.cy+=pframelistCurrent->lRollupDelta;
                                        /* Now size and move the frame window to its original size and
                                           position before the child controls are made visible again */
            pframedataCurrent->swpFrame.fl=SWP_SIZE|SWP_MOVE|SWP_NOADJUST;
            WinSetMultWindowPos(WinQueryAnchorBlock(hwndFrame), &pframedataCurrent->swpFrame, 1);
            }
        else
            {
                                        /* Set height to titlebar plus 2 times the border height. As the
                                           frame is positioned relative to the lower left point, we also
                                           have to move the lower left corner upwards by the number of
                                           pixel the size of the frame is reduced */
            pframelistCurrent->lRollupDelta=pframedataCurrent->swpFrame.cy;
            pframedataCurrent->swpFrame.y+=pframedataCurrent->swpFrame.cy;
            pframedataCurrent->swpFrame.cy=WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
            if(pframedataCurrent->ulFrameFlags & FS_DLGBORDER)
                pframedataCurrent->swpFrame.cy+=(WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME)<<1);
            if(pframedataCurrent->ulFrameFlags & FS_BORDER)
                pframedataCurrent->swpFrame.cy+=(WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER)<<1);
            if(pframedataCurrent->ulFrameFlags & FS_SIZEBORDER)
                pframedataCurrent->swpFrame.cy+=(WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER)<<1);
            pframedataCurrent->ulRollupHeight=pframedataCurrent->swpFrame.cy;
            pframedataCurrent->swpFrame.y-=pframedataCurrent->swpFrame.cy;
            pframelistCurrent->lRollupDelta-=pframedataCurrent->swpFrame.cy;
            }
                                        /* Adjust visibility of controls to avoid the titlebar and
                                           its controls to be overlapped by another window. We are
                                           using hereby a special window style to mark our manipulations
                                           which is not used by OS/2 currently (which may not be true
                                           in the future though) */
        pframedataCurrent->henumFrame=WinBeginEnumWindows(hwndFrame);
        while((pframedataCurrent->hwndFrameChild=WinGetNextWindow(pframedataCurrent->henumFrame))!=NULLHANDLE)
            {
            pframedataCurrent->ulFrameChildFlags=WinQueryWindowULong(pframedataCurrent->hwndFrameChild, QWL_STYLE);
            if(pframelistCurrent->ulStatusFlag & FL_ROLLWINDOW)
                {
                                        /* If we have hidden the window previously, unhide it */
                if(pframedataCurrent->ulFrameChildFlags & WS_HIDDENBYPC2)
                    {
                    pframedataCurrent->ulFrameChildFlags&=(~WS_HIDDENBYPC2);
                    WinSetWindowULong(pframedataCurrent->hwndFrameChild, QWL_STYLE, pframedataCurrent->ulFrameChildFlags);
                    WinShowWindow(pframedataCurrent->hwndFrameChild, TRUE);
                    }
                }
            else
                {
                                        /* If we are rolling up the frame check for controls that
                                           are below the titlebar's vertical position to hide them */
                WinQueryWindowPos(pframedataCurrent->hwndFrameChild, &pframedataCurrent->swpFrameChild);
                if(pframedataCurrent->swpFrameChild.y<(LONG)pframedataCurrent->ulRollupHeight)
                    {
                                        /* If the control is not already hidden, mark it and hide it */
                    if(pframedataCurrent->ulFrameChildFlags & WS_VISIBLE)
                        {
                        pframedataCurrent->ulFrameChildFlags|=WS_HIDDENBYPC2;
                        WinSetWindowULong(pframedataCurrent->hwndFrameChild, QWL_STYLE, pframedataCurrent->ulFrameChildFlags);
                        WinShowWindow(pframedataCurrent->hwndFrameChild, FALSE);
                        }
                    }
                }
            }
        WinEndEnumWindows(pframedataCurrent->henumFrame);
                                        /* Now size and move the frame window to its reduced size and
                                           position after the child controls were made invisible */
        if(!(pframelistCurrent->ulStatusFlag & FL_ROLLWINDOW))
            {
            pframedataCurrent->swpFrame.fl=SWP_SIZE|SWP_MOVE|SWP_NOADJUST;
            WinSetMultWindowPos(WinQueryAnchorBlock(hwndFrame), &pframedataCurrent->swpFrame, 1);
            }
                                        /* Toggle flag and force the rollup button to be redrawn */
        pframelistCurrent->ulStatusFlag^=FL_ROLLWINDOW;
                                        /* Force redraw by invalidating window, to ensure correctly
                                           drawn ROLLWINDOW Titlebar Smarticon (as this one needs to
                                           be changed) */
        WinInvalidateRect(WinWindowFromID(hwndFrame, FID_ROLLWINDOW), NULL, FALSE);
        }
                                        /* We have handled the messages of our titlebar controls */
        return((MRESULT)TRUE);

                                        /* Close window button pressed. Unfortunately when adding the
                                           titlebar controls we don't know it there will be a DID_OK,
                                           DID_CANCEL,... pushbutton, as when we subclass, the child
                                           controls have not yet been created. We could test for the
                                           systemmenu containing SC_CLOSE, but this would prevent the
                                           close button from most dialogs */
    case SC_CLOSEWINDOW:
        {
                                        /* We could query the window word QWL_DEFBUTTON which returne the
                                           window handle of the default pushbutton, but e.g. for EPM's
                                           search dialog this would be the Find pushbutton which does not
                                           close the dialog. Therefore it seems to be more effective to
                                           skip the default pushbutton */
                                        /* Try to find either a DID_OK (MBID_OK), MBID_YES, MBID_ENTER,
                                           DID_CANCEL (MBID_CANCEL) pushbuttons */
        if(WinWindowFromID(hwndFrame, DID_OK))
            WinPostMsg(hwndFrame, WM_COMMAND, MPFROMSHORT(DID_OK), MPFROM2SHORT(CMDSRC_PUSHBUTTON, FALSE));
        else if(WinWindowFromID(hwndFrame, MBID_YES))
            WinPostMsg(hwndFrame, WM_COMMAND, MPFROMSHORT(MBID_YES), MPFROM2SHORT(CMDSRC_PUSHBUTTON, FALSE));
        else if(WinWindowFromID(hwndFrame, MBID_ENTER))
            WinPostMsg(hwndFrame, WM_COMMAND, MPFROMSHORT(MBID_ENTER), MPFROM2SHORT(CMDSRC_PUSHBUTTON, FALSE));
        else if(WinWindowFromID(hwndFrame, DID_CANCEL))
            WinPostMsg(hwndFrame, WM_COMMAND, MPFROMSHORT(DID_CANCEL), MPFROM2SHORT(CMDSRC_PUSHBUTTON, FALSE));
        else if(WinSendMsg(WinWindowFromID(hwndFrame, FID_SYSMENU), MM_QUERYITEM,
            MPFROM2SHORT(SC_CLOSE, TRUE), MPFROMP(&pframedataCurrent->menuitemFrame)))
            WinPostMsg(hwndFrame, WM_SYSCOMMAND, MPFROMLONG(SC_CLOSE), MPFROM2SHORT(CMDSRC_MENU, FALSE));
        else
            WinPostMsg(WinWindowFromID(hwndFrame, FID_CLIENT), WM_SYSCOMMAND, MPFROMLONG(SC_CLOSE), MPFROM2SHORT(CMDSRC_MENU, FALSE));
        }
        return((MRESULT)TRUE);

                                        /* We will only never receive this message for OS/2 WARP, as the help
                                           button has a BS_HELP style, which causes direct interaction with the
                                           help manager.
                                           For OS/2 2.11 and before BS_HELP does not work, in this case we
                                           simulate the help style as good as we can */
    case SC_HELP:
        {
                                        /* Unfortunately the help instance is only returned by WinQueryHelpInstance()
                                           after the help panel was activated the first time, as we don't know the
                                           help instance before, we can't activate the help panel for the first time.
                                           Generating the messages a F1 key event would create does not work, because
                                           the message (sent or posted) doesn't get into the system input queue,
                                           where it must be for the accelerator translation to occur (maybe a
                                           journal playback hook could help) */
        pframedataCurrent->hwndFrameHelp=WinQueryHelpInstance(hwndFrame);
        pframedataCurrent->usFrameID=WinQueryWindowUShort(hwndFrame, QWS_ID);
        if(pframedataCurrent->hwndFrameHelp)
            {
                                        /* First try to get context sensitive help, if this doesn't
                                           work try to get the contents and help index */
            if(WinSendMsg(pframedataCurrent->hwndFrameHelp, HM_DISPLAY_HELP, MPFROMSHORT(pframedataCurrent->usFrameID), NULL))
                if(WinSendMsg(pframedataCurrent->hwndFrameHelp, HM_HELP_CONTENTS, NULL, NULL))
                    WinSendMsg(pframedataCurrent->hwndFrameHelp, HM_HELP_INDEX, NULL, NULL);
            }
        }
                                        /* We have handled the messages of our titlebar controls */
        return((MRESULT)TRUE);

    case SC_MOVEVD:
    case SC_MOVEEDGE:
        {
                                        /* Get mouse position and map to titlebar control that was
                                           just pressed */
        WinQueryPointerPos(HWND_DESKTOP, &pframedataCurrent->ptlMouse);
        pframedataCurrent->hwndFrameChild=WinWindowFromID(hwndFrame, SHORT1FROMMP(mp1));
        WinMapWindowPoints(HWND_DESKTOP, pframedataCurrent->hwndFrameChild, &pframedataCurrent->ptlMouse, 1);
        WinQueryWindowRect(pframedataCurrent->hwndFrameChild, &pframedataCurrent->rectlFrameChild);
        WinQueryWindowPos(hwndFrame, &pframedataCurrent->swpFrame);
        if(SHORT1FROMMP(mp1)==SC_MOVEEDGE)
            {
                                        /* Move the frame window to that corner on the screen according
                                           in which quadrant of the titlebar control the mouse was clicked in */
            if(pframedataCurrent->ptlMouse.x>(pframedataCurrent->rectlFrameChild.xRight>>1))
                pframedataCurrent->swpFrame.x=HookParameters.swpScreen.cx-pframedataCurrent->swpFrame.cx;
            else
                pframedataCurrent->swpFrame.x=0;
            if(pframedataCurrent->ptlMouse.y>(pframedataCurrent->rectlFrameChild.yTop>>1))
                pframedataCurrent->swpFrame.y=HookParameters.swpScreen.cy-pframedataCurrent->swpFrame.cy;
            else
                pframedataCurrent->swpFrame.y=0;
            }
        if(SHORT1FROMMP(mp1)==SC_MOVEVD)
            {
                                        /* SIN(45ø)=COS(45ø) multiplied by factor 1000 (to avoid floating point
                                           arithmetic */
            #define ROTATION    0707L
                                        /* Move the frame window in the direction on another Virtual Desktop according
                                           in which quadrant rotated counterclockwise by 45ø of the titlebar control
                                           the mouse was clicked in
                                           Rotation: x'=x*cos(45ø)+y*sin(45ø)
                                                     y'=-x*sin(45ø)+y*cos(45ø)
                                           Don't forget to divide result be 1000 (avoiding floating point arithmetic) */
                                        /* According to a dump, traps on DeScribe's toolbar (during
                                           multiplication by ROTATION) have ben observed. I don't have
                                           DeScribe, nor can I amagine why a trap could happen here */
            pframedataCurrent->ptlMouseRotation.x=(((pframedataCurrent->ptlMouse.x-(pframedataCurrent->rectlFrameChild.xRight>>1))*ROTATION)+
                ((pframedataCurrent->ptlMouse.y-(pframedataCurrent->rectlFrameChild.yTop>>1))*ROTATION))/1000;
            pframedataCurrent->ptlMouseRotation.y=((-(pframedataCurrent->ptlMouse.x-(pframedataCurrent->rectlFrameChild.xRight>>1))*ROTATION)+
                ((pframedataCurrent->ptlMouse.y-(pframedataCurrent->rectlFrameChild.yTop>>1))*ROTATION))/1000;
                                        /* Move up, left, down, right for point in quadrant I, II, III, IV */
            if(pframedataCurrent->ptlMouseRotation.x>=0)
                {
                if(pframedataCurrent->ptlMouseRotation.y>=0)
                    {
                                        /* Move frame window up by the size of the screen. If this would
                                           move the frame out of the Virtual Desktop, than wrap the position
                                           around to the bottommost Virtual Desktop */
                    if((HookParameters.VirtualDesktopMax.y-pframedataCurrent->swpFrame.cx)>=
                        (HookParameters.VirtualDesktopPos.y+HookParameters.DesktopSize.y+pframedataCurrent->swpFrame.y))
                        pframedataCurrent->swpFrame.y+=HookParameters.DesktopSize.y;
                    else
                        pframedataCurrent->swpFrame.y+=(HookParameters.VirtualDesktopMin.y-HookParameters.VirtualDesktopPos.y);
                    }
                else
                    {
                                        /* Move frame window right by the size of the screen. If this would
                                           move the frame out of the Virtual Desktop, than wrap the position
                                           around to the leftmost Virtual Desktop */
                    if((HookParameters.VirtualDesktopMax.x-pframedataCurrent->swpFrame.cx)>=
                        (HookParameters.VirtualDesktopPos.x+HookParameters.DesktopSize.x+pframedataCurrent->swpFrame.x))
                        pframedataCurrent->swpFrame.x+=HookParameters.DesktopSize.x;
                    else
                        pframedataCurrent->swpFrame.x+=(HookParameters.VirtualDesktopMin.x-HookParameters.VirtualDesktopPos.x);
                    }
                }
            else
                {
                if(pframedataCurrent->ptlMouseRotation.y>=0)
                    {
                                        /* Move frame window left by the size of the screen. If this would
                                           move the frame out of the Virtual Desktop, than wrap the position
                                           around to the rightmost Virtual Desktop */
                    if(HookParameters.VirtualDesktopMin.x<=
                        (HookParameters.VirtualDesktopPos.x-HookParameters.DesktopSize.x+pframedataCurrent->swpFrame.x))
                        pframedataCurrent->swpFrame.x-=HookParameters.DesktopSize.x;
                    else
                        pframedataCurrent->swpFrame.x+=(HookParameters.VirtualDesktopMax.x-HookParameters.VirtualDesktopPos.x-HookParameters.DesktopSize.x);
                    }
                else
                    {
                                        /* Move frame window down by the size of the screen. If this would
                                           move the frame out of the Virtual Desktop, than wrap the position
                                           around to the topmost Virtual Desktop */
                    if(HookParameters.VirtualDesktopMin.y<=
                        (HookParameters.VirtualDesktopPos.y-HookParameters.DesktopSize.y+pframedataCurrent->swpFrame.y))
                        pframedataCurrent->swpFrame.y-=HookParameters.DesktopSize.y;
                    else
                        pframedataCurrent->swpFrame.y+=(HookParameters.VirtualDesktopMax.y-HookParameters.VirtualDesktopPos.y-HookParameters.DesktopSize.y);
                    }
                }
            }
                                        /* Now move the frame window to its new position */
        pframedataCurrent->swpFrame.fl=SWP_MOVE|SWP_NOADJUST;
        WinSetMultWindowPos(WinQueryAnchorBlock(hwndFrame), &pframedataCurrent->swpFrame, 1);
        }
                                        /* We have handled the messages of our titlebar controls */
        return((MRESULT)TRUE);

    default:
        return(pframelistCurrent->pfnwpFrame(hwndFrame, msg, mp1, mp2));
    }

case WM_QUERYFRAMECTLCOUNT:
    {
                                        /* Pass the message on to the next window procedure and
                                           adjust our additions afterwards */
    pframedataCurrent->usFrameCtlCount=SHORT1FROMMR(pframelistCurrent->pfnwpFrame(hwndFrame, msg, mp1, mp2));
    if(!(pframelistCurrent->ulWindowFlag  & WINDOWSUBCLASSED))
        return((MRESULT)(ULONG)(pframedataCurrent->usFrameCtlCount));
                                        /* For windows running in PC/2's address space, remove MOVEVD and MOVEEDGE
                                           titlebar controls */
    DosGetInfoBlocks(&pframedataCurrent->ptibFrame, &pframedataCurrent->ppibFrame);
    if(pframedataCurrent->ppibFrame->pib_ulpid==HookParameters.ProcessId)
        pframelistCurrent->ulControlsFlag&=(~(FL_MOVEVD|FL_MOVEEDGE));
                                        /* If this window's parent is not HWND_DESKTOP, than remove
                                           the MOVEVD, and MOVEEDGE titlebar controls */
    pframedataCurrent->hwndFrameParent=WinQueryWindow(hwndFrame, QW_PARENT);
    if(pframedataCurrent->hwndFrameParent!=HookParameters.hwndDesktop)
        pframelistCurrent->ulControlsFlag&=(~(FL_MOVEVD|FL_MOVEEDGE));
                                        /* If this window is a window inside a help window, remove HELP
                                           (MOVEVD and MOVEEDGE are already removed because this window
                                           has not HWND_DESKTOP as parent) titlebar controls */
    pframedataCurrent->hwndFrameCurrent=pframedataCurrent->hwndFrameParent;
    while((pframedataCurrent->hwndFrameParent!=HookParameters.hwndDesktop) && (pframedataCurrent->hwndFrameParent!=NULLHANDLE))
        {
        pframedataCurrent->hwndFrameCurrent=pframedataCurrent->hwndFrameParent;
        pframedataCurrent->hwndFrameParent=WinQueryWindow(pframedataCurrent->hwndFrameCurrent, QW_PARENT);
        }
    pframedataCurrent->hwndFrameHelp=WinWindowFromID(pframedataCurrent->hwndFrameCurrent, FID_CLIENT);
    WinQueryClassName(pframedataCurrent->hwndFrameHelp, sizeof(ucClassname), ucClassname);
    if(!strcmp(ucClassname, "CLASS_COVERPAGE"))
        pframelistCurrent->ulControlsFlag&=(~FL_HELP);
#if     0
                                        /* If the window already has a close button (default for WARP 4),
                                           then remove our close titlebar control */
    if(WinSendMsg(WinWindowFromID(hwndFrame, FID_MINMAX), MM_QUERYITEM,
        MPFROM2SHORT(SC_CLOSE, TRUE), &pframedataCurrent->menuitemFrame))
        pframelistCurrent->ulControlsFlag&=(~FL_CLOSEWINDOW);
#else
                                        /* If the window already has a close button (default for WARP 4),
                                           then remove it, as PC/2's close button tries to find the user's
                                           preference not just posting a simple WM_SYSCOMMAND SC_CLOSE */
    if((WinSendMsg(WinWindowFromID(hwndFrame, FID_MINMAX), MM_QUERYITEM,
        MPFROM2SHORT(SC_CLOSE, TRUE), &pframedataCurrent->menuitemFrame)) &&
        (pframelistCurrent->ulControlsFlag & FL_CLOSEWINDOW))
        WinSendMsg(WinWindowFromID(hwndFrame, FID_MINMAX), MM_DELETEITEM,
            MPFROM2SHORT(SC_CLOSE, TRUE), NULL);
#endif  /* 0 */
                                        /* Count the number of titlebar controls we're going to add now */
    for(pframedataCurrent->ulShiftBitcount=(sizeof(pframelistCurrent->ulControlsFlag)<<3), pframedataCurrent->ulShiftBitmask=0x00000001;
        pframedataCurrent->ulShiftBitcount>0;
        pframedataCurrent->ulShiftBitcount--, pframedataCurrent->ulShiftBitmask<<=1)
        if(pframelistCurrent->ulControlsFlag & pframedataCurrent->ulShiftBitmask)
            pframedataCurrent->usFrameCtlCount++;
    return((MRESULT)(ULONG)pframedataCurrent->usFrameCtlCount);
    }

case WM_FORMATFRAME:
    {
    pframedataCurrent->usFrameCtlCount=(LONG)(pframelistCurrent->pfnwpFrame(hwndFrame, msg, mp1, mp2));
    if(!(pframelistCurrent->ulWindowFlag & WINDOWSUBCLASSED))
        return((MRESULT)(ULONG)(pframedataCurrent->usFrameCtlCount));
    pframedataCurrent->pswpFrameControls=PVOIDFROMMP(mp1);
                                        /* Find the titlebar in the SWP array, as we have to reduce
                                           its size in order to add our own frame controls */
    for(pframedataCurrent->usFrameTitlebar=0;
        pframedataCurrent->usFrameTitlebar<pframedataCurrent->usFrameCtlCount;
        pframedataCurrent->usFrameTitlebar++)
        if(WinQueryWindowUShort(pframedataCurrent->pswpFrameControls[pframedataCurrent->usFrameTitlebar].hwnd, QWS_ID)==FID_TITLEBAR)
            break;
                                        /* If we found the titlebar, shrink is horizontally and
                                           add our controls instead of the shrunk space */
    if(pframedataCurrent->usFrameTitlebar!=pframedataCurrent->usFrameCtlCount)
        {
                                        /* Cache access to the titlebar's SWP structure */
        pframedataCurrent->pswpTitlebar=&pframedataCurrent->pswpFrameControls[pframedataCurrent->usFrameTitlebar];
                                        /* Cache access to the user added control's SWP structure */
        pframedataCurrent->pswpControl=&pframedataCurrent->pswpFrameControls[pframedataCurrent->usFrameCtlCount];
                                        /* Default SWP_* flags used to add Smarticons */
        pframedataCurrent->ulSWPFlag=SWP_SIZE|SWP_MOVE|SWP_SHOW;
        pframedataCurrent->ulStyle=WS_VISIBLE|WS_SYNCPAINT|BS_NOCURSORSELECT|BS_NOPOINTERFOCUS|BS_SYSCOMMAND|BS_USERBUTTON;
                                        /* If there is no space for 8 Smarticons, don't add our
                                           Smarticons visibly */
        pframedataCurrent->lSmarticonSize=pframedataCurrent->pswpTitlebar->cy;
        if(pframedataCurrent->pswpTitlebar->cx<(pframedataCurrent->pswpTitlebar->cy<<3))
            {
            pframedataCurrent->ulSWPFlag&=(~SWP_SHOW);
            pframedataCurrent->ulStyle&=(~WS_VISIBLE);
            pframedataCurrent->lSmarticonSize=0;
            }
                                        /* Create the windows from right to left, so start with the
                                           rightmost one */
        for(pframedataCurrent->ulShiftBitmask=0x00000001, pframedataCurrent->ulControl=FID_TITLEBARBEGIN;
            pframedataCurrent->ulControl<(FID_TITLEBARBEGIN+NUM_TITLEBARCONTROLS);
            pframedataCurrent->ulShiftBitmask<<=1, pframedataCurrent->ulControl++)
            {
                                        /* If the current titlebar control should not be created, skip it */
            if(!(pframelistCurrent->ulControlsFlag & pframedataCurrent->ulShiftBitmask))
                continue;
                                        /* Cache 0 based array index */
            pframedataCurrent->ulIndex=pframedataCurrent->ulControl-FID_TITLEBARBEGIN;
                                        /* Cache access current frame control */
            pframedataCurrent->pswpControl=&pframedataCurrent->pswpFrameControls[pframedataCurrent->usFrameCtlCount];
                                        /* Subtrace width of control from titlebar size */
            pframedataCurrent->pswpTitlebar->cx-=pframedataCurrent->lSmarticonSize;
                                        /* Fit the control right beside the titlebar */
            pframedataCurrent->pswpControl->fl=pframedataCurrent->ulSWPFlag;
            pframedataCurrent->pswpControl->cx=pframedataCurrent->pswpTitlebar->cy-1;
            pframedataCurrent->pswpControl->cy=pframedataCurrent->pswpTitlebar->cy-1;
            pframedataCurrent->pswpControl->x=pframedataCurrent->pswpTitlebar->x+pframedataCurrent->pswpTitlebar->cx;
            pframedataCurrent->pswpControl->y=pframedataCurrent->pswpTitlebar->y;
            pframedataCurrent->pswpControl->hwndInsertBehind=NULLHANDLE;
                                        /* The window of the control must exist after the first
                                           reference to it, and ensure that the window is created
                                           successfully */
            if(pframelistCurrent->hwndControls[pframedataCurrent->ulIndex]==NULLHANDLE)
                {
                pframedataCurrent->ulStyleActual=pframedataCurrent->ulStyle;
                                        /* For the help button we may get context sensitive help for
                                           windows having defined some, otherwise HM_ERROR
                                           (HMERR_PANEL_NOT_FOUND) will be posted to the frame this
                                           help was activated from.
                                           Unfortunately the BS_HELP button style only works for OS/2
                                           WARP (as I would say it should work), for OS/2 2.xx we will
                                           have to simulate during SC_SYSCOMMAND:SC_HELP what WARP does
                                           automagically */
                if(pframedataCurrent->ulControl==FID_HELP)
                    if(!((HookParameters.ulVersionMajor==OS2_MAJOR) &&
                                        /* As OS2_MINOR_200 is 0 any ULONG value is >= 0, so the
                                           comparism is always true, so in order to prevent a compiler
                                           waring
                        (HookParameters.ulVersionMinor>=OS2_MINOR_200) &&
                                           above line should be disabled */
                        (HookParameters.ulVersionMinor<=OS2_MINOR_211)))
                        pframedataCurrent->ulStyleActual|=BS_HELP;
                pframelistCurrent->hwndControls[pframedataCurrent->ulIndex]=WinCreateWindow(hwndFrame, WC_BUTTON, "PC/2",
                    pframedataCurrent->ulStyleActual,
                    pframedataCurrent->pswpControl->x, pframedataCurrent->pswpControl->y,
                    pframedataCurrent->pswpControl->cx, pframedataCurrent->pswpControl->cy,
                    hwndFrame, HWND_TOP, pframedataCurrent->ulControl, NULL, NULL);
                }
                                        /* Insert control's window handle */
            pframedataCurrent->pswpControl->hwnd=pframelistCurrent->hwndControls[pframedataCurrent->ulIndex];
                                        /* Advance to next control */
            pframedataCurrent->usFrameCtlCount++;
            }
        }
    return((MRESULT)(ULONG)(pframedataCurrent->usFrameCtlCount));
    }

case WM_CONTROL:
    if(SHORT2FROMMP(mp1)==BN_PAINT)
        {
                                        /* Control we're drawing */
        pframedataCurrent->ulControl=(ULONG)SHORT1FROMMP(mp1);
                                        /* For any controls out of our own range, pass request on to
                                           next frame window procedure */
        if((pframedataCurrent->ulControl<FID_TITLEBARBEGIN) || (pframedataCurrent->ulControl>(FID_TITLEBAREND)))
            return(pframelistCurrent->pfnwpFrame(hwndFrame, msg, mp1, mp2));
        if(!(pframelistCurrent->ulWindowFlag & WINDOWSUBCLASSED))
            return((MRESULT)TRUE);
                                        /* Cache 0 based array index */
        pframedataCurrent->ulIndex=pframedataCurrent->ulControl-FID_TITLEBARBEGIN;
                                        /* Access the structure we draw our SmartIcons with */
        pframedataCurrent->puserbuttonControl=PVOIDFROMMP(mp2);
        WinQueryWindowRect(pframedataCurrent->puserbuttonControl->hwnd, &pframedataCurrent->rectlFrameChild);
                                        /* Blit the bitmap with its position dependent on normal or
                                           highlighted state */
        pframedataCurrent->ulShift=(pframedataCurrent->puserbuttonControl->fsState & BDS_HILITED ? 1 : 0);
                                        /* Calculate the border around the ownerdraw titlebar
                                           pushbutton and also the location where to blit the bitmap.
                                           Both depend from the button highlighted state */
        pframedataCurrent->rectlBorder=pframedataCurrent->rectlFrameChild;
        pframedataCurrent->rectlBlit=pframedataCurrent->rectlFrameChild;
        pframedataCurrent->rectlBorder.xRight+=1;
        pframedataCurrent->rectlBorder.yTop+=1;
        if((HookParameters.ulVersionMajor==OS2_MAJOR) && (HookParameters.ulVersionMinor>=OS2_MINOR_400))
            {
                                        /* For WARP 4 (aka Merlin) the Titlebar Controls get
                                           replaced with an "inverted" version (dark grey and
                                           white exchanged) instead of displaying the same
                                           bitmap 1 pixel right and down for the down state */
            pframedataCurrent->rectlBlit.xLeft+=1;
            pframedataCurrent->rectlBlit.yBottom+=1;
                                        /* If the statusflag corresponding to this titlebar control is
                                           set display the toggled state bitmap if a toggled state exists,
                                           otherwise the untoggled one */
            if(pframedataCurrent->ulShift)
                {
                if((pframelistCurrent->ulStatusFlag & (1<<pframedataCurrent->ulIndex)) &&
                    (FL_TOGGLEMASK & (1<<pframedataCurrent->ulIndex)))
                    WinDrawBitmap(pframedataCurrent->puserbuttonControl->hps, pframelistCurrent->hbmControls3[pframedataCurrent->ulIndex], NULL,
                        (POINTL *)&pframedataCurrent->rectlBlit.xLeft, 0, 0, DBM_NORMAL);
                else
                    WinDrawBitmap(pframedataCurrent->puserbuttonControl->hps, pframelistCurrent->hbmControls2[pframedataCurrent->ulIndex], NULL,
                        (POINTL *)&pframedataCurrent->rectlBlit.xLeft, 0, 0, DBM_NORMAL);
                }
            else
                {
                if((pframelistCurrent->ulStatusFlag & (1<<pframedataCurrent->ulIndex)) &&
                    (FL_TOGGLEMASK & (1<<pframedataCurrent->ulIndex)))
                    WinDrawBitmap(pframedataCurrent->puserbuttonControl->hps, pframelistCurrent->hbmControls1[pframedataCurrent->ulIndex], NULL,
                        (POINTL *)&pframedataCurrent->rectlBlit.xLeft, 0, 0, DBM_NORMAL);
                else
                    WinDrawBitmap(pframedataCurrent->puserbuttonControl->hps, pframelistCurrent->hbmControls0[pframedataCurrent->ulIndex], NULL,
                        (POINTL *)&pframedataCurrent->rectlBlit.xLeft, 0, 0, DBM_NORMAL);
                }
                                        /* Draw the 3D border around the pushbutton (for
                                           WARP 4 systems) */
            WinDrawBorder(pframedataCurrent->puserbuttonControl->hps, &pframedataCurrent->rectlBorder, 1, 1,
                SYSCLR_DIALOGBACKGROUND, 0, DB_PATCOPY);
            }
        else
            {
                                        /* For pre WARP 4 display the same bitmap at 1 pixel
                                           right and down for the down state */
            pframedataCurrent->rectlBlit.xLeft+=1+pframedataCurrent->ulShift;
            pframedataCurrent->rectlBlit.xRight+=pframedataCurrent->ulShift;
            pframedataCurrent->rectlBlit.yBottom+=1-pframedataCurrent->ulShift;
            pframedataCurrent->rectlBlit.yTop-=pframedataCurrent->ulShift;
                                        /* If the statusflag corresponding to this titlebar control is
                                           set display the toggled state bitmap if a toggled state exists,
                                           otherwise the untoggled one */
            if((pframelistCurrent->ulStatusFlag & (1<<pframedataCurrent->ulIndex)) &&
                (FL_TOGGLEMASK & (1<<pframedataCurrent->ulIndex)))
                BlitBit(pframelistCurrent->hwndControls[pframedataCurrent->ulIndex],
                   pframedataCurrent->puserbuttonControl->hps, pframelistCurrent->hbmControls1[pframedataCurrent->ulIndex], &pframedataCurrent->rectlBlit);
            else
                BlitBit(pframelistCurrent->hwndControls[pframedataCurrent->ulIndex],
                   pframedataCurrent->puserbuttonControl->hps, pframelistCurrent->hbmControls0[pframedataCurrent->ulIndex], &pframedataCurrent->rectlBlit);
                                        /* Draw the 3D border around the pushbutton (for pre
                                           WARP 4 systems) */
            WinDrawBorder(pframedataCurrent->puserbuttonControl->hps, &pframedataCurrent->rectlBorder, 1, 1, 0, 0,
                ((pframedataCurrent->puserbuttonControl->fsState & BDS_HILITED) ? DB_DEPRESSED : DB_RAISED));
            }
        pframedataCurrent->puserbuttonControl->fsStateOld=pframedataCurrent->puserbuttonControl->fsState;
        return((MRESULT)TRUE);
        }
                                        /* Pass on all unhandled messages (we could also "fall" through
                                           the default label of the switch statement, but when some code
                                           gets added and I forget to pass on all unhandled message
                                           there may be some difficult to find problems */
    return(pframelistCurrent->pfnwpFrame(hwndFrame, msg, mp1, mp2));

                                        /* If our frame having PC/2's TitleBar Smarticons is going to be
                                           minimized or restored, we have to hide or unhide the Smarticons,
                                           to avoid a corrupted icon on the Desktop */
case WM_ADJUSTWINDOWPOS:
    {
    pframedataCurrent->pswpFrame=PVOIDFROMMP(mp1);
    pframedataCurrent->bVisibility=(BOOL)-1;
    if(pframedataCurrent->pswpFrame!=0)
        {
        if(pframedataCurrent->pswpFrame->fl & SWP_MINIMIZE)
            pframedataCurrent->bVisibility=FALSE;
        if(pframedataCurrent->pswpFrame->fl & SWP_RESTORE)
            pframedataCurrent->bVisibility=TRUE;
        if(pframedataCurrent->bVisibility!=(BOOL)-1)
            for(pframedataCurrent->ulIndex=0;
                pframedataCurrent->ulIndex<NUM_TITLEBARCONTROLS;
                pframedataCurrent->ulIndex++)
                if(pframelistCurrent->hwndControls[pframedataCurrent->ulIndex])
                    WinShowWindow(pframelistCurrent->hwndControls[pframedataCurrent->ulIndex], pframedataCurrent->bVisibility);
        }
    }
    return(pframelistCurrent->pfnwpFrame(hwndFrame, msg, mp1, mp2));

default:
    return(pframelistCurrent->pfnwpFrame(hwndFrame, msg, mp1, mp2));
}
return((MRESULT)FALSE);
}

