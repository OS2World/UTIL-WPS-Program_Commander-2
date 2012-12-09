/***********************************************************************\
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1993, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * PC2Tlkt.h    Program Commander/2 OS/2 Toolkit additions             *
 *                                                                     *
\***********************************************************************/

#ifndef __PC2TLKT_H__
#define __PC2TLKT_H__

                                        /* Define OS/2 major and minor versions */
#define         OS2_MAJOR               0x00000014
#define         OS2_MINOR_200           0x00000000
#define         OS2_MINOR_210           0x0000000A
#define         OS2_MINOR_211           0x0000000B
#define         OS2_MINOR_300           0x0000001E
#define         OS2_MINOR_400           0x00000028

                                        /* Same as KC_DBCSRSRVD1, but seems to be acceptable
                                           to me */
#define KC_LWIN                         0x4000
                                        /* Same as KC_DBCSRSRVD2, but seems to be acceptable
                                           to me */
#define KC_RCTRL                        0x8000


                                        /* OS/2 3.00 WARP+ supports the new APIs DosSetExtLIBPATH
                                           (DOSCALL1.998) and DosQueryExtLIBPATH (DOSCALL1.999)
                                           to dynamically modify the search sequence for locating
                                           dynamic link libraries (DLLs). PC/2 tries dynamic linking
                                           of these APIs, instead of statically linking, for
                                           compatibility with OS/2 2.x.
                                           To my surprise, these APIs seem to be in a 16:16 segment,
                                           APIENTRY does not work (at the time I write this I have
                                           no documentation that emphasis or denys this).
                                           The 32 bit entrypoints are 873 and 874 (thanks to Andrew
                                           Zabolotny for the hint) */
#define         BEGINLIBPATH    1
#define         ENDLIBPATH      2
#define         BEGINLIBPATHLEN (sizeof("BEGINLIBPATH=")-1)
#define         ENDLIBPATHLEN   (sizeof("ENDLIBPATH=")-1)
typedef APIRET  (APIENTRY DOSSETEXTLIBPATH)(PSZ pszExtLIBPATH, ULONG ulFlags);
typedef APIRET  (APIENTRY DOSQUERYEXTLIBPATH)(PSZ pszExtLIBPATH, ULONG ulFlags);
                                        /* 16-bit APIs no longer used, but here for completeness */
typedef APIRET  (APIENTRY16 DOS16SETEXTLIBPATH)(PSZ pszExtLIBPATH, ULONG ulFlags);
typedef APIRET  (APIENTRY16 DOS16QUERYEXTLIBPATH)(PSZ pszExtLIBPATH, ULONG ulFlags);

                                        /* Get/SetProcessorStatus() interface (undocumented, requires
                                           Warp 3 Srv. Adv. SMP+) */
typedef APIRET  (APIENTRY GETPROCESSORSTATUS)(ULONG ulProcNum, PULONG pulStatus);
typedef APIRET  (APIENTRY SETPROCESSORSTATUS)(ULONG ulProcNum, ULONG pulStatus);

                                        /* DosPerfSysCall() interface (undocumented) */
typedef APIRET  (APIENTRY DOSPERFSYSCALL)(ULONG ulCommand, ULONG ulParm1, ULONG ulParm2, ULONG ulParm3);

                                        /* DosQueryModFromEIP() interface (undocumented, requires
                                           Warp 3+) */
typedef APIRET  (APIENTRY DOSVERIFYPIDTID)(PID Pid, TID Tid);


#define HK_PREACCEL             17      /* Pre-Accelerator hook (undocumented, thanks to Robert
                                           Mahoney for telling me about) */
#define WM_DRAW                 0x20D   /* WM_DRAW message (undocumented) */

                                        /* DosDumpProcess() interface (undocumented) */
#ifndef DDP_DISABLEPROCDUMP
#define DDP_DISABLEPROCDUMP     0x00000000
#define DDP_ENABLEPROCDUMP      0x00000001
#define DDP_PERFORMPROCDUMP     0x00000002
#endif  /* DDP_DISABLEPROCDUMP */
APIRET APIENTRY DosDumpProcess(ULONG ulFlag, ULONG ulDrive, PID pid);

                                        /* DosSuppressPopUps() interface (undocumented) */
#ifndef SPU_DISABLESUPPRESSION
#define SPU_DISABLESUPPRESSION  0x00000000
#define SPU_ENABLESUPPRESSION   0x00000001
#endif  /* SPU_DISABLESUPPRESSION */
APIRET APIENTRY DosSuppressPopUps(ULONG ulFlag, ULONG ulDrive);

                                        /* If SMP Toolkit is not included, define: */
#ifndef QSV_NUMPROCESSORS
#define QSV_NUMPROCESSORS               26
#endif  /* QSV_NUMPROCESSORS */

                                        /* Not defined in Warp 3 Toolkit */
#ifndef ORDDOSSETEXTLIBPATH
#define ORDDOSSETEXTLIBPATH             873
#endif  /* ORDDOSSETEXTLIBPATH */
APIRET APIENTRY DosSetExtLibpath(PSZ pszExtLIBPATH, ULONG ulFlags);

#ifndef ORDDOSQUERYEXTLIBPATH
#define ORDDOSQUERYEXTLIBPATH           874
#endif  /* ORDDOSQUERYEXTLIBPATH */
APIRET APIENTRY DosQueryExtLibpath(PSZ pszExtLIBPATH, ULONG ulFlags);

                                        /* Not defined in SMP or Warp Toolkit */
#ifndef ORDDOSGETPROCESSORSTATUS
#define ORDDOSGETPROCESSORSTATUS        447
#endif  /* ORDDOSGETPROCESSORSTATUS */
APIRET APIENTRY DosGetProcessorStatus(ULONG ulProcNum, PULONG pulStatus);

#ifndef ORDDOSSETPROCESSORSTATUS
#define ORDDOSSETPROCESSORSTATUS        448
#define PROCESSOR_OFFLINE               0
#define PROCESSOR_ONLINE                1
#endif  /* ORDDOSSETPROCESSORSTATUS */
APIRET APIENTRY DosSetProcessorStatus(ULONG ulProcNum, ULONG ulStatus);

                                        /* DosPerfSysCall() interface (documented) */
#ifndef ORDDOSPERFSYSCALL
#define ORDDOSPERFSYSCALL               976
#define CMD_KI_ENABLE                   0x60
#define CMD_KI_KLRDCNT                  0x63
#define CMD_SOFTTRACE_LOG               0x14
#endif  /* ORDDOSPERFSYSCALL */
APIRET APIENTRY DosPerfSysCall(ULONG ulCommand, ULONG ulParm1, ULONG ulParm2, ULONG ulParm3);

#ifndef CPUUTIL
typedef struct  _CPUUTIL
{
    ULONG   ulTotalLow;
    ULONG   ulTotalHigh;
    ULONG   ulIdleLow;
    ULONG   ulIdleHigh;
    ULONG   ulBusyLow;
    ULONG   ulBusyHigh;
    ULONG   ulIntrLow;
    ULONG   ulIntrHigh;
} CPUUTIL, *PCPUUTIL;
                                        /* Convert 8-byte (low, high) time value to double */
#define LL2F(high, low) (4294967296.0*(high)+(low))
#endif  /* CPUUTIL */

                                        /* DosQueryModFromEIP() interface (undocumented, requires
                                           Warp 3+) */
#ifndef ORDDOSVERIFYPIDTID
#define ORDDOSVERIFYPIDTID      460
#endif  /* ORDDOSVERIFYPIDTID */
APIRET  APIENTRY DosVerifyPidTid(PID Pid, TID Tid);

                                        /* DosQueryModFromEIP() interface (undocumented) */
APIRET APIENTRY DosQueryModFromEIP(HMODULE *phMod, ULONG *pulObjNum, ULONG ulBuffLen, CHAR *pcBuff, ULONG *pulOffset, ULONG ulAddress);

#ifndef IBMTLKT40
                                        /* DosQueryThreadContext() interface (undocumented) */
APIRET APIENTRY DosQueryThreadContext(TID tid, ULONG ulLevel, PCONTEXTRECORD pcontectrec);
#endif  /* IBMTLKT40 */

                                        /* APM interfaces (not defined in Toolkit) */
typedef struct  _POWERSTATUS            POWERSTATUS;
typedef struct  _POWEREVENT             POWEREVENT;

struct  _POWERSTATUS
{
USHORT  usParmLength;
USHORT  usPowerFlags;
UCHAR   ucACStatus;
UCHAR   ucBatteryStatus;
UCHAR   ucBatteryLife;
};

struct  _POWEREVENT
{
USHORT  usSubID;
USHORT  usReserved;
USHORT  usData1;
USHORT  usData2;
};

                                        /* National settings from OS2.INI (as DosQueryCrtyInfo()
                                           does not change what it returns when changing the
                                           date/time settings with the Country WPS object) */
typedef struct  _PMNATIONAL             PMNATIONAL;

struct  _PMNATIONAL
{
ULONG   ulDateFmt;                      /* Date format (see COUNTRYINFO.fsDateFmt) */
ULONG   ulTimeFmt;                      /* Time format (see COUNTRYINFO.fsTimeFmt) */
UCHAR   ucDateSeparator;                /* Date separator (see COUNTRYINFO.szDateSeparator) */
UCHAR   ucTimeSeparator;                /* Time separator (see COUNTRYINFO.szTimeSeparator) */
UCHAR   ucAMFmt[3];                     /* AM format (e.g. "AM", "am", ...) */
UCHAR   ucPMFmt[3];                     /* PM format (e.g. "PM", "pm", ...) */
};

                                        /* Defined for the delayed Sliding Focus */
#define         HT_SLIDINGFOCUSDELAY    (HT_NORMAL+1)
                                        /* Defined for the delayed Dynamic Menuselection */
#define         HT_DYNAMICMENUDELAY     (HT_NORMAL+2)
                                        /* Defined for keyboard simulated WM_MOUSEMOVE message when the
                                           keyboard should also cause the Dynamic Menuselection be activated */
#define         HT_DYNAMICMENUKBD       (HT_NORMAL+3)

#endif  /* __PC2TLKT_H__ */
