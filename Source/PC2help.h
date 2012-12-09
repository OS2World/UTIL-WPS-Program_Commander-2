/***********************************************************************\
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1993, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * PC2Help.h    Program Commander/2 header file. Contains definitions  *
 *              for all constants used in help files                   *
 *                                                                     *
\***********************************************************************/

                                        /* IDs used for HELP panels */
#define MAIN_HELP_TABLE         4001
#define MAIN_HELP_SUBTABLE      4002
#define ID_FAQ1                 4003
#define ID_FAQ2                 4004
#define ID_FAQ3                 4005
#define ID_FAQ4                 4006
#define ID_FAQ5                 4007
#define ID_GENERALHELP          4008
#define ID_CONCEPTSHELP         4009
#define ID_USINGHELP            4010
#define ID_ERRORHELP            4011
#define ID_INSTALLATIONHELP     4012
#define ID_OVERVIEWHELP         4013
#define ID_COMPATIBILITY        4014
#define ID_TRAPINFO             4015
#define ID_TECHINFO             4016
#define ID_SURVEY               4017
#define ID_BUGS                 4018

                                        /* Help pages */
#define PIID_EXAMPLE1           4050
#define CDID_HELP_1             4051
#define CDID_HELP_2             4052


                                        /* PC/2 error messages help panel IDs */
#define HELP_CREATEWINDOW       4100    /* PM environment creation failed */
#define HELP_PC2LOADED          4101    /* PC/2 already loaded */
#define HELP_DLLLOADED          4102    /* Can't load from PC2HOOK.DLL */
#define HELP_PC2INI             4103    /* Can't load from/write to PC2.INI */
#define HELP_PC2SPOOLDLL        4104    /* Can't load from PC2SPOOL.DLL */
#define HELP_PC2THREAD          4105    /* Can't start second thread */
#define HELP_PC2CFG             4106    /* Can't load from/write to PC2.CFG */
#define HELP_PC2ENV             4107    /* Can't load from/write to PC2.ENV */
#define HELP_PC2HOOK            4108    /* Can't load/unload hooks */
#define HELP_CREATEDIALOG       4109    /* WinDlgBox failed */
#define HELP_SETPATH            4110    /* Can't change current directory */
#define HELP_PRIORITY           4111    /* Can't change our priority */
#define HELP_LAUNCH             4112    /* Error when starting a session */
#define HELP_MEDIA              4113    /* Error during DosDevIOCtl */
#define HELP_PC2RUNNINGASWPS    4114    /* Can't close PC/2 when running as WPS */
                                        /* Can't close PC/2 when running as PROTSHELL */
#define HELP_PC2RUNNINGASPROTSHELL  4115    
#define HELP_PC2BADPASSWORD     4116    /* Lockup Password and Verification Password
                                           do not match */
#define HELP_PC2REBOOT          4117    /* Can't reboot via DOS.SYS */
#define HELP_PC2TIMER           4118    /* Can't acquire a timer */
                                        /* Popup-Menu's icon cache save notification */
#define HELP_ICONCACHE          4119
                                        /* Some modifications to a PM menu failed */
#define HELP_POPUPMENU          4120
                                        /* Some modifications to the SessionBar failed */
#define HELP_SESSIONBAR         4121
                                        /* Some modifications to the QuickSwitch failed */
#define HELP_QUICKSWITCH        4122
                                        /* Can't suspend or power off via APM.SYS */
#define HELP_POWERDOWN          4123
                                        /* Can't continue cleanup processing */
#define HELP_PC2CLEANUP         4124
                                        /* Backup requested/recommended/done/failed */
#define HELP_PC2BACKUP          4125
                                        /* One or more threads failed to reset alive counter */
#define HELP_PC2ALIVE           4126
                                        /* The Automatic Update has found some status information */
#define HELP_PC2STATUS          4127
                                        /* The Automatic Update has found some news information */
#define HELP_PC2NEWS            4128
                                        /* The Automatic Update has found some file updates */
#define HELP_PC2UPDATE          4129
                                        /* PC/2 has detected an Registration page filled 
                                           insufficiently */
#define HELP_PC2REGISTRATION    4130
                                        /* PC/2 has detected an error loading PC2NET.DLL */
#define HELP_PC2NETDLL          4131
                                        /* Some modifications to the BubbleHelp failed */
#define HELP_BUBBLEHELP         4132

#define HELP_HMERROR            4150    /* HM_ERROR detected */
#define HELP_PC2HELP            4151    /* Can't load from PC2.HLP */
#define HELP_PC2TRAP            4152    /* New exception found in PC2.TRP */

#define HELP_CREATEWINDOW_SUBTABLE      4200
#define HELP_PC2LOADED_SUBTABLE         4201
#define HELP_DLLLOADED_SUBTABLE         4202
#define HELP_PC2INI_SUBTABLE            4203
#define HELP_PC2ENV_SUBTABLE            4204
#define HELP_PC2SPOOLDLL_SUBTABLE       4205
#define HELP_PC2THREAD_SUBTABLE         4206
#define HELP_PC2CFG_SUBTABLE            4207
#define HELP_PC2HOOK_SUBTABLE           4208
#define HELP_CREATEDIALOG_SUBTABLE      4209
#define HELP_SETPATH_SUBTABLE           4210
#define HELP_PRIORITY_SUBTABLE          4211
#define HELP_LAUNCH_SUBTABLE            4212
#define HELP_MEDIA_SUBTABLE             4213
#define HELP_PC2RUNNINGASWPS_SUBTABLE   4214
#define HELP_PC2RUNNINGASPROTSHELL_SUBTABLE \
                                        4215
#define HELP_PC2BADPASSWORD_SUBTABLE    4216
#define HELP_PC2REBOOT_SUBTABLE         4217
#define HELP_PC2TIMER_SUBTABLE          4218
#define HELP_ICONCACHE_SUBTABLE         4219
#define HELP_POPUPMENU_SUBTABLE         4220
#define HELP_SESSIONBAR_SUBTABLE        4221
#define HELP_QUICKSWITCH_SUBTABLE       4222
#define HELP_POWERDOWN_SUBTABLE         4223
#define HELP_PC2CLEANUP_SUBTABLE        4224
#define HELP_PC2BACKUP_SUBTABLE         4225
#define HELP_PC2ALIVE_SUBTABLE          4226
#define HELP_PC2STATUS_SUBTABLE         4227
#define HELP_PC2NEWS_SUBTABLE           4228
#define HELP_PC2UPDATE_SUBTABLE         4229
#define HELP_PC2REGISTRATION_SUBTABLE   4230
#define HELP_PC2NETDLL_SUBTABLE         4231
#define HELP_BUBBLEHELP_SUBTABLE        4232
#define HELP_PC2TRAP_SUBTABLE           4233

