#ifndef _PC2EMX_H_
#define _PC2EMX_H_
/***********************************************************************\
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1993, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * PC2EMX.h     Program Commander/2 header file for EMX support. (I    *
 *              don't have EMX installed, but the following            *
 *              definitions are likely to be missing.                  *
 *                                                                     *
\***********************************************************************/

                                        /* For compiling with EMX C/C++ Rolf Knebel suggested
                                           to define constants not defined by the EMX package,
                                           whereby I've added similar ones, because I know a
                                           PC/2 compilition with EMX for version 1.80 last */
                                        /* Thanks to Roland Emmerich for testing EMX and GCC
                                           compatibility */
#ifdef  __EMX__
                                        /* IOCTL definitions */
#define         IOCTL_DISK          0x08
#define         DSK_BLOCKREMOVABLE  0x20
#define         IOCTL_KEYBOARD      0x04
#define         KBD_SETSESMGRHOTKEY 0x56
                                        /* IOCTL constants missing in BSEDEV.H ? */
                                        /* Already defined for PC/2
#define         IOCTL_CHARDEVICE    0x82
#define         DSK_DEVICESTATUS    0x60
                                        */

                                        /* IOCTL datastructures */
#pragma pack(2)
typedef struct _CPID
{
    USHORT  idCodePage;
    USHORT  Reserved;
} CPID;
typedef CPID *PCPID;

typedef struct _KBDKEYINFO
{
    UCHAR   chChar;
    UCHAR   chScan;
    UCHAR   fbStatus;
    UCHAR   bNlsShift;
    USHORT  fsState;
    ULONG   time;
} KBDKEYINFO;
typedef KBDKEYINFO *PKBDKEYINFO;
#pragma pack()

#pragma pack(1)
struct _HOTKEY
{
    USHORT  fsHotKey;
    UCHAR   uchScancodeMake;
    UCHAR   uchScancdeBreak;
    USHORT  idHotKey;
} HOTKEY;
typedef HOTKEY *PHOTKEY;
#pragma pack()

define          DosSetPrty          DosSetPriority

#endif  /* __EMX__ */

#endif /* _PC2EMX_H_ */
