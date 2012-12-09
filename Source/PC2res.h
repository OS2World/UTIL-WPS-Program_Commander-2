/***********************************************************************\
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1993, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * PC2Res.h     Program Commander/2 header file. Contains definitions  *
 *              for all constants used in resource files.              *
 *                                                                     *
\***********************************************************************/

#define DID_OK                  1       /* DID_OK */
#define DID_CANCEL              2       /* DID_CANCEL */
#define DID_HELP                3       /* DID_CANCEL+1 */
#define DID_BACKUP              4       /* DID_CANCEL+2 */

#define ADID_ABOUTDIALOG        1000    /* About program dialog */
#define ADID_ABOUTBLDLEVEL      1001

#define LDID_LOCKUPDIALOG       1100    /* Lockup password verification dialog to unlock */
#define LDID_PASSWORDDIALOG     1101    /* Security password verification dialog to unlock */
#define LDEF_LOCKUPPASSWORD     1102
#define LDST_INVALIDPASSWORD    1103
#define LDST_USER               1104
#define LDEF_USER               1105
#define LDMLE_MESSAGE           1106
#define LDPB_POSTIT             1107
#define LDMLE_POSTIT            1108

#define PIID_PROGRAMDIALOG      1200    /* Program installation notebook dialog */
#define PINB_NOTEBOOK           1201

#define PIID_PAGE1              1250    /* First notebook page */
#define PIGRP_PROGRAM           1251    /* Program data group */
#define PIEF_PROGRAMTITLE       1252
#define PIEF_ICONFILE           1253
#define PIEF_PATHFILENAME       1254
#define PIEF_DIRECTORY          1255
#define PIEF_PARAMETERS         1256
#define PIMLE_DOSSETTINGS       1257
#define PIPB_WINFILEDLG         1258
#define PIPB_SETTINGSDLG        1259
#define PIST_DOSSETTINGS        1260

#define PIID_PAGE2              1300    /* Second notebook page */
#define PIGRP_TYPE              1301    /* Program type group */
#define PIRB_SHELL              1302
#define PIRB_OS2FULLSCREEN      1303
#define PIRB_OS2WINDOW          1304
#define PIRB_PM                 1305
#define PIRB_DOSFULLSCREEN      1306
#define PIRB_DOSWINDOW          1307
#define PIRB_WPSOBJECT          1308
#define PIRB_DETACHED           1309
#define PIRB_WINOS2SEAMLESS     1310
#define PIRB_WINOS2FULLSCREEN   1311

#define PIGRP_WINOS2            1350    /* Seamless WIN-OS2 settings */
#define PIRB_WINREAL            1351
#define PIRB_WINSTANDARD        1352
#define PIRB_WIN386ENHANCED     1353
#define PICB_WINSEPARATE        1354

#define PIID_PAGE3              1400    /* Third notebook page */
#define PIGRP_STYLE             1401    /* Program style group */
#define PIRB_DEFAULTSTYLE       1402
#define PIRB_MAXIMIZED          1403
#define PIRB_MINIMIZED          1404
#define PICB_INVISIBLE          1405
#define PICB_NOAUTOCLOSE        1406
#define PICB_BACKGROUND         1407
#define PICB_AUTOSTART          1408
#define PIHSB_AUTOSTARTDELAY    1409
#define PIEF_AUTOSTARTDELAY     1410
#define PICB_PASSWORDPROTECT    1411
#define PIGRP_SIZEPOSITION      1412    /* User defined size & position */
#define PICB_SIZEPOSITION       1413
#define PIEF_X                  1414
#define PIEF_Y                  1415
#define PIEF_XSIZE              1416
#define PIEF_YSIZE              1417

#define PIID_PAGE4              1450    /* Forth notebook page */
#define PIGRP_HOTKEY            1451    /* Hotkey settings group */
#define PIEF_SESSIONTITLE       1452
#define PICB_HOTKEYENABLED      1453
#define PIRB_WSMAXIMIZED        1454
#define PIRB_WSRESTORE          1455
#define PIRB_NONE               1456
#define PICB_RCTRL              1457
#define PICB_CTRL               1458
#define PICB_ALT                1459
#define PICB_LWIN               1460
#define PICBX_HOTKEY            1461
#define PIGRP_DESKTOP           1462
#define PICB_DONTMOVE           1463
#define PIGRP_WINDOWLIST        1464
#define PILB_WINDOWLIST         1465

#define PIID_PAGE5              1500    /* Fifth notebook page */
#define PIGRP_PROCESSPRIORITY   1501    /* Process priority */
#define PIRB_IDLETIME           1502
#define PIRB_REGULAR            1503
#define PIRB_NOCHANGE           1504
#define PIRB_TIMECRITICAL       1505
#define PIRB_FOREGROUNDSERVER   1506
#define PIEF_PRIORITYDELTA      1507
#define PIHSB_PRIORITYDELTA     1508

#define PIID_PAGE6              1550    /* Sixth notebook page */
#define PIGRP_ENVIRONMENT       1551    /* Process environment */
#define PIST_ENVIRONMENTS       1552
#define PILB_ENVIRONMENTS       1553    /* Predefined environment macros */
#define PIMLE_ENVIRONMENT       1554    /* Additions/Replacements of PC/2's environment and/or
                                           predefined environment macros for the started session */
#define PIST_ENVIRONMENT        1555
#define PIRB_LEADENVIRONMENT    1556    /* Set to display the environment merged before Environment Spaces */
#define PIRB_TRAILENVIRONMENT   1557    /* Set to display the environment merged after Environment Spaces */
#define PIST_ENVSETTINGS        1558
#define PICB_STARTUPENVIRONMENT 1559

#define PIID_PAGE7              1600    /* Seventh notebook page (shared with control configuration notebook) */
#define PIGRP_SCHEDULE          1601
#define PIRB_DISABLESCHEDULER   1602
#define PIRB_SCHEDULEHOURLY     1603
#define PIRB_SCHEDULEDAILY      1604
#define PIRB_SCHEDULEWEEKLY     1605
#define PIRB_SCHEDULEMONTHLY    1606
#define PIRB_SCHEDULEONCE       1607
#define PILB_SCHEDULEMINUTE     1608
#define PILB_SCHEDULEHOUR       1609
#define PILB_SCHEDULEWEEKDAY    1610
#define PILB_SCHEDULEDAY        1611
#define PILB_SCHEDULEMONTH      1612
#define PICB_SCHEDULESAVEANYWAY 1613

                                        /* Control configuration notebook dialog */
#define CCID_CONTROLDIALOG      1650
#define CCNB_NOTEBOOK           PINB_NOTEBOOK

#define CCID_PAGE1              1700    /* First notebook page */
#define CCGRP_CONTROL           1701
#define CCEF_CONTROL            1702

                                        /* Second notebook page */
#define CCID_PAGE2              PIID_PAGE7
#define CCGRP_SCHEDULE          PIGRP_SCHEDULE
#define CCRB_DISABLESCHEDULER   PIRB_DISABLESCHEDULER
#define CCRB_SCHEDULEHOURLY     PIRB_SCHEDULEHOURLY
#define CCRB_SCHEDULEDAILY      PIRB_SCHEDULEDAILY
#define CCRB_SCHEDULEWEEKLY     PIRB_SCHEDULEWEEKLY
#define CCRB_SCHEDULEMONTHLY    PIRB_SCHEDULEMONTHLY
#define CCRB_SCHEDULEONCE       PIRB_SCHEDULEONCE
#define CCLB_SCHEDULEMINUTE     PILB_SCHEDULEMINUTE
#define CCLB_SCHEDULEHOUR       PILB_SCHEDULEHOUR
#define CCLB_SCHEDULEWEEKDAY    PILB_SCHEDULEWEEKDAY
#define CCLB_SCHEDULEDAY        PILB_SCHEDULEDAY
#define CCLB_SCHEDULEMONTH      PILB_SCHEDULEMONTH
#define CCCB_SCHEDULESAVEANYWAY PICB_SCHEDULESAVEANYWAY

#define DDID_DESKTOPDIALOG      1750    /* Desktop configuration dialog */
#define DDNB_NOTEBOOK           1751

#define DDID_PAGE1A             1800    /* First notebook page (1/2) */
#define DDGRP_VIRTUALDESKTOP    1801
#define DDCB_VIRTUALDESKTOP     1802
#define DDCB_FOLLOWFOCUS        1803
#define DDCB_MOVEDESKTOP        1804
#define DDCB_EXPANDWPS          1805
#define DDCB_CLICK2MOVE         1806
#define DDCB_AUTOSCROLL         1807
#define DDHSB_DESKTOPDELAY      1808
#define DDEF_DESKTOPDELAY       1810
#define DDCB_OVERVIEW           1811
#define DDSB_HORIZONTALDESKTOPS 1812
#define DDSB_VERTICALDESKTOPS   1813
#define DDEF_SCROLLPERCENTAGE   1814
#define DDHSB_SCROLLPERCENTAGE  1815
#define DDGRP_POPUP             1816
#define DDRB_BUTTON1            1817
#define DDRB_BUTTON3            1818
#define DDCB_SYSTEMWIDE         1819
#define DDRB_BUTTONCLICK        1820
#define DDRB_BUTTONDBLCLK       1821
#define DDRB_SELECTDEFAULT      1822
#define DDRB_SELECTFIRSTITEM    1823
#define DDRB_SELECTCENTER       1824
#define DDCB_POPUPICON          1825
#define DDRB_FULLSIZE           1826
#define DDRB_HALFSIZE           1827
#define DDCB_LEFTRIGHTSHIFT     1828
#define DDCB_LEFTCTL            1829
#define DDCB_RIGHTCTL           1830
#define DDCB_LEFTWIN95          1831
#define DDCB_RIGHTWIN95         1832
#define DDCB_WIN95MENU          1833
#define DDCB_CAPSLOCK           1834
#define DDCB_SCROLLLOCK         1835

#define DDID_PAGE1B             1850    /* First notebook page (2/2) */
#define DDGRP_SLIDINGFOCUS      1851
#define DDCB_SLIDINGFOCUS       1852
#define DDCB_PRESERVEZORDER     1853
#define DDCB_ACTIVATEWINDOW     1854
#define DDHSB_SLIDINGDELAY      1855
#define DDEF_SLIDINGDELAY       1856
#define DDGRP_HWVIRTUALDESKTOP  1857
#define DDCB_HWVIRTUALDESKTOP   1858
#define DDRB_640X480            1859
#define DDRB_800X600            1860
#define DDRB_1024X768           1861
#define DDRB_1280X1024          1862
#define DDMLE_PANNINGEXCEPTION  1863

#define DDID_PAGE2              1900    /* Second notebook page */
#define DDGRP_OVERVIEWWINDOW    1901
#define DDCB_DYNAMICOVERVIEW    1902
#define DDRB_OVERVIEWUL         1903
#define DDRB_OVERVIEWUR         1904
#define DDRB_OVERVIEWLL         1905
#define DDRB_OVERVIEWLR         1906
#define DDEF_OVERVIEWDELAY      1907
#define DDHSB_OVERVIEWDELAY     1908
#define DDCB_HIDETITLEBAR       1909
#define DDCB_HIDESMARTICONBAR   1910
#define DDCB_TARGETCURSOR       1911
#define DDCB_KEEPONTOPOV        1912
#define DDCB_ONECLICKSWITCH     1913
#define DDGRP_OVERVIEWLOOK      1914
#define DDCB_3DBORDER           1915
#define DDCB_OVERVIEWTITLES     1916
#define DDCB_TITLEAUTOCOLOR     1917
#define DDRB_OVERVIEWICONS      1918
#define DDRB_OVERVIEWGRADIENT   1919
#define DDRB_OVERVIEWRANDOM     1920
#define DDRB_OVERVIEWMONO       1921
#define DDST_COLOR1             1922
#define DDST_RGBCOLOR1          1923
#define DDST_COLOR2             1924
#define DDST_RGBCOLOR2          1925
#define DDST_COLOR3             1926
#define DDST_RGBCOLOR3          1927
#define DDST_COLOR4             1928
#define DDST_RGBCOLOR4          1929
#define DDGRP_STICKYWINDOWS     1930
#define DDMLE_STICKYWINDOWS     1931
#define DDLB_STICKYWINDOWS      1932
#define DDPB_ADDSTICKY          1933
#define DDPB_REMOVESTICKY       1934

#define DDID_PAGE3              1950    /* Third notebook page */
#define DDGRP_SESSIONBAR        1951
#define DDCB_SESSIONBAR         1952
#define DDCB_KEEPONTOPSB        1953
#define DDCB_NOSESSIONNAME      1954
#define DDCB_DEFAULTDATETIME    1955
#define DDCB_NODATETIME         1956
#define DDEF_SESSIONBARDATETIME 1957
#define DDRB_SESSIONBARLARGE    1958
#define DDRB_SESSIONBARSMALL    1959
#define DDCB_SESSIONBARHELP     1960
#define DDCB_DYNAMICSESSIONBAR  1961
#define DDHSB_SESSIONBARDELAY   1962
#define DDEF_SESSIONBARDELAY    1963
#define DDSB_SESSIONBARHOTSPOT  1964
#define DDCB_CPUPERFORMANCE     1965
#define DDCB_PERFORMANCEFILL    1966
#define DDCB_NOGRID             1967
#define DDMLE_SESSIONBAREXCEPTION   1968
#define DDGRP_QUICKSWITCH       1969
#define DDCB_ENABLEQUICKSWITCH  1970

#define DDID_PAGE4              2000    /* Fourth notebook page */
#define DDGRP_PHYSICALDESKTOP   2001
#define DDCB_BUTTON2ZORDER      2002
#define DDCB_BUTTON2MOVE        2003
#define DDCB_USESPOOLER         2004
#define DDEF_DESKTOPNAME        2005
#define DDEF_WINDOWLISTNAME     2006
#define DDGRP_BACKGROUNDBITMAP  2007
#define DDCB_BACKGROUNDBITMAP   2008
#define DDEF_BACKGROUNDBITMAP   2009
#define DDPB_WINFILEDLG         2010
#define DDRB_BITMAPNORMAL       2011
#define DDRB_BITMAPSCALED       2012
#define DDRB_BITMAPTILED        2013
#define DDSB_BITMAPTILEDCOUNT   2014

#define DDID_PAGE5              2050    /* Fifth notebook page */
#define DDGRP_GENERAL           2051
#define DDCB_IGNOREAUTOSTART    2052
#define DDCB_IGNORESCHEDULER    2053
#define DDCB_HOTKEY4PC2ONLY     2054
#define DDCB_ENABLEDIAGNOSTICS  2055
#define DDGRP_SHUTDOWN          2056
#define DDCB_NORMALSHUTDOWN     2057
#define DDCB_NOVERIFYSHUTDOWN   2058
#define DDCB_SHUTDOWNREBOOT     2059
#define DDGRP_POWERDOWN         2060
#define DDCB_SUPPORTAPM         2061
#define DDCB_ENABLEAPM          2062
#define DDCB_NOVERIFYPOWERDOWN  2063
#define DDRB_POWERDOWNSUSPEND   2064
#define DDRB_POWERDOWNPOWEROFF  2065
#define DDCB_ENABLEPOWERDOWN    2066
#define DDSB_POWERDOWNTIMER     2067
#define DDGRP_ADVANCEDMARKING   2068
#define DDCB_ADVANCEDMARKING    2069
#define DDRB_ADVMARKINGBUTTON2  2070
#define DDRB_ADVMARKINGBUTTON3  2071
#define DDGRP_DYNAMICMENU       2072
#define DDCB_DYNAMICMENU        2073
#define DDCB_CASCADEMENU        2074
#define DDCB_DYNAMICMENUKBD     2075
#define DDHSB_DYNAMICDELAY      2076
#define DDEF_DYNAMICDELAY       2077

#define DDID_PAGE6              2100    /* Sixth notebook page */
#define DDGRP_TITLEBARICONS     2101
#define DDCB_TITLEBARICONS      2102
#define DDCB_CLOSEWINDOW        2103
#define DDCB_ROLLWINDOW         2104
#define DDCB_HELP               2105
#define DDCB_MOVEVD             2106
#define DDCB_MOVEEDGE           2107
#define DDMLE_TITLEBAREXCEPTION 2108

#define DDID_PAGE7              2150    /* Seventh notebook page */
#define DDGRP_LOCKUP            2151
#define DDEF_ACCESSPASSWORD     2152
#define DDEF_VERIFYACCESSPWD    2153
#define DDCB_PC2SECURITY        2154
#define DDEF_LOCKUPPASSWORD     2155
#define DDEF_VERIFYLOCKUPPWD    2156
#define DDCB_ENABLELOCKUP       2157
#define DDCB_LOCKUPONBOOT       2158
#define DDSB_LOCKUPTIMER        2159

#define DDID_PAGE8A             2200    /* Eighth notebook page (1/2) */
#define DDGRP_ENVIRONMENT       2201
#define DDLB_ENVIRONMENT        2202
#define DDPB_ADDENVIRONMENT     2203
#define DDPB_REMOVEENVIRONMENT  2204
#define DDPB_CHANGEENVIRONMENT  2205
#define DDPB_2BATCHENVIRONMENT  2206
#define DDPB_EXPORTENVIRONMENT  2207
#define DDPB_IMPORTENVIRONMENT  2208

#define DDID_PAGE8B             2250    /* Eighth notebook page (2/2) */
#define DDEF_ENVIRONMENT        2251
#define DDMLE_ENVIRONMENT       2252

#define DDID_PAGE9              2300    /* Nineth notebook page */
#define DDCB_AUTOUPDATEENABLE   2301
#define DDCB_SENDREGISTRATION   2302
#define DDEF_FIRSTNAME          2303
#define DDEF_LASTNAME           2304
#define DDEF_EMAILADDRESS       2305
#define DDCB_USESOCKS           2306
#define DDEF_PROXYURL           2307
#define DDEF_HTTPUSERID         2308
#define DDEF_HTTPPASSWORD       2309
#define DDEF_UPDATEURL          2310
#define DDPB_ACCESSNET          2311
#define DDCB_NETWORKSTATUS      2312
#define DDST_NETWORKSTATUS      2313

#define DDID_PAGE10             2350    /* Tenth notebook page */
#define DDCB_SPYFRAMEWINDOW     2351
#define DDEF_FRAMEWINDOWTEXT    2352
#define DDEF_FRAMEWINDOWCLASS   2353
#define DDEF_FRAMEWINDOWLIST    2354
#define DDEF_EXECUTABLE         2355
#define DDEF_FRAMEPID           2356
#define DDEF_FRAMETID           2357
#define DDEF_FRAMEX             2358
#define DDEF_FRAMECX            2359
#define DDEF_FRAMEY             2360
#define DDEF_FRAMECY            2361
#define DDEF_FRAMEHWND          2362

#define CDID_CONFIGDIALOG       2500    /* Configuration menu dialog */
#define CDLB_MENUPROGRAM        2501
#define CDPB_LEVELUP            2502
#define CDPB_LEVELDOWN          2503
#define CDPB_CUT                2504
#define CDPB_PASTE              2505
#define CDPB_ADDMENU            2506
#define CDPB_ADDPROGRAM         2507
#define CDPB_CLONEPROGRAM       2508
#define CDPB_CHANGEENTRY        2509
#define CDPB_REMOVEENTRY        2510
#define CDPB_RESORT             2511
#define CDPB_ADDCONTROL         2512
#define CDPB_EXPORTPROGRAM      2513
#define CDPB_IMPORTPROGRAM      2514
#define CDLB_ENVIRONMENT        2515
#define CDPB_ADDENVIRONMENT     2516
#define CDPB_REMOVEENVIRONMENT  2517
#define CDST_PATH               2518

#define RDID_RESORTDIALOG       2600    /* Resort menuentries dialog */
#define RDPB_MOVE               2601    /* Move pushbutton */
#define RDGRP_SOURCEMENU        2602    /* Move from menu */
#define RDLB_SOURCEMENU         2603
#define RDGRP_DESTINATIONMENU   2604    /* Move into menu */
#define RDLB_DESTINATIONMENU    2605

#define MIID_MENUDIALOG         2700    /* Menu installation notebook dialog */
#define MINB_NOTEBOOK           2701

#define MIID_PAGE1              2800    /* First notebook page */
#define MIGRP_MENU              2801
#define MIEF_MENUTITLE          2802
#define MIEF_ICONFILE           2803

#define MDID_CONTROLDIALOG      2900    /* PC/2 Menu-Control dialog */
#define MDGRP_CTRL              2901
#define MDRB_CTRL_CONFIGMENU    2902
#define MDRB_CTRL_CONFIGDESKTOP 2903
#define MDRB_CTRL_ABOUT         2904
#define MDRB_CTRL_SHUTDOWN      2905
#define MDRB_CTRL_POWERDOWN     2906
#define MDRB_CTRL_HELP          2907
#define MDRB_CTRL_EXIT          2908
#define MDRB_CTRL_SPOOLER       2909
#define MDRB_CTRL_LOCKUP        2910
#define MDRB_CTRL_BRKSEPARATOR  2911
#define MDRB_CTRL_SEPARATOR     2912

#define SDID_SHUTDOWNDIALOG     3000    /* OS/2 ShutDown dialog */
#define SDID_SHUTDOWNMESSAGE    3001    /* Message diaplayed a ShutDown */

#define SUID_STARTUPDIALOG      3100    /* Request startup parameters from user before launching
                                           an application */
#define SUTF_STARTUPPROGRAM     3101    /* Application to be started */
#define SUTF_STARTUPINFO        3102    /* Info to the user what to do */
#define SUTF_STARTUPPARAMETERS  3103    /* Info to the user of current parameters */
#define SUEF_STARTUPPARAMETERS  3104    /* Entryfield for user data */
#define SUPB_FILE               3105    /* Get parameters via WinFileDlg() */

#define SEID_STARTUPDIALOG      3200    /* Request Environment Spaces to be used from user before
                                           launching an application */
#define SETF_STARTUPPROGRAM     3201    /* Application we change Environment */
#define SELB_ENVIRONMENTS       3202    /* Environment Spaces to be started */

#define WPID_WPS2PC2DIALOG      3300    /* WPS 2 PC2 dialog */
#define WPTF_EXTRACTEDOBJECTS   3301    /* Text for listbox */
#define WPLB_EXTRACTEDOBJECTS   3302    /* Listbox of WPS objects extracted */
#define WPTF_STATUSBITMAP       3303    /* Text for status bitmap */
#define WPBM_STATUSBITMAP       3304    /* Status bitmap */
#define WPBM_EMPTY              3305    /* First status bitmap (empty listbox) */
#define WPBM_FULL               3306    /* Second status bitmap (filled listbox) */
#define WPPB_CLEAR              3307    /* Clear all WPS objects extracted up to now */
#define WPPB_HELP               3308    /* Help */

#define DDID_WINDOWLIST         3400    /* PC/2 Window List dialog */
#define DDLB_WINDOWLIST         3401    /* Window List listbox control */
#define DDPB_WINDOWSELECT       3402    /* 'Select' button */
#define DDPB_WINDOWREFRESH      3403    /* 'Refresh' button */

#define ID_PC2MAINWINDOW        256
#define ID_VGA_PC2MAINWINDOW    ID_PC2MAINWINDOW
                                        /* Unfortunately the resource compile can't do simple
                                           arithmetic, so we can't use the following macros,
                                           but have to use the hardcoded method
#define ID_BGA_PC2MAINWINDOW    ID_PC2MAINWINDOW + VGA2BGA
                                           this is the hardcoded way */
#define ID_BGA_PC2MAINWINDOW    356

#define ID_PC2THREADWINDOW      257
#define ID_PC2SPOOLERWINDOW     258
#define ID_PC2POPUPMENU         259
#define ID_PC2SPOOLERCONTAINER  260
#define ID_WPS2PC2MAINWINDOW    261
#define ID_PC2SWITCHWINDOW      262
#define ID_PC2SESSIONWINDOW     263
#define ID_PC2STATUSWINDOW      264
#define ID_PC2STATUSTEXT        265
#define ID_PC2BUBBLEWINDOW      266

#define ID_SPOOLERPOPUPMENU     290     /* Popup Menu for PC/2 spooler window container */
#define ID_SPOOLERREFRESH       291     /* Refresh spooler window */
#define ID_SPOOLERHOLDJOB       292     /* Pause a print job */
#define ID_SPOOLERRELEASEJOB    293     /* Release a print job */
#define ID_SPOOLERDELETEJOB     294     /* Delete a print job */
#define ID_SPOOLERHOLDQUEUE     295     /* Pause a printer queue */
#define ID_SPOOLERRELEASEQUEUE  296     /* Release a printer queue */
#define ID_SPOOLERHELP          297     /* Help for PC/2 spooler window */

#define ID_PC2SETUP             300     /* Defines for PopUp Menu, that is popped up after
                                           the user clicks mouse button 1 on DESKTOP */
#define ID_CONFIGDIALOG         301     /* User pressed Configure Menu */
#define ID_DESKTOPDIALOG        302     /* User pressed Configure Desktop */
#define ID_ABOUTDIALOG          303     /* User pressed About PC/2 */
#define ID_SHUTDOWN             304     /* User pressed ShutDown OS/2 */
#define ID_POWERDOWN            305     /* User pressed PowerDown OS/2 */
#define ID_HELP                 306     /* ID used for HELP panels */
#define ID_EXIT                 307     /* User pressed F3 to exit PC/2 */
#define ID_SPOOLER              308     /* User pressed PC/2 Spooler */
#define ID_LOCKUP               309     /* User pressed Lockup */
#define ID_DEBUG                310     /* User pressed Debug toggle key */
#define ID_UNHANG               311     /* User pressed unhang busy thread */
#define ID_BREAKPOINT           312     /* User pressed breakpoint */

                                        /* User selected to unhide hidden SessionBar icons */
#define ID_ICONSUNHIDE          315

#define ID_BITMAPEXIT           320     /* Exit PC/2 smarticon */
#define ID_BITMAPMOVE           321     /* Move smarticon */
#define ID_BITMAPHIDE           322     /* Hide window smarticon */
#define ID_BITMAPZORDERTOP      323     /* Set window behind HWND_TOP smarticon */
#define ID_BITMAPZORDERBOTTOM   324     /* Set window behind HWND_BOTTOM smarticon */
#define ID_BITMAPCLOSE          325     /* Close window smarticon */
#define ID_BITMAPMAXIMIZE       326     /* Maximize window smarticon */
#define ID_BITMAPMINIMIZE       327     /* Minimize window smarticon */
#define ID_BITMAPRESTORE        328     /* Restore window smarticon */
#define ID_BITMAPSHUTDOWN       329     /* ShutDown OS/2 smarticon */
#define ID_BITMAPPOWERDOWN      330     /* PowerDown OS/2 smarticon */
#define ID_BITMAPLOCKUP         331     /* Lockup smarticon */
#define ID_BITMAPHELP           332     /* Help PC/2 window smarticon */
#define ID_BITMAPLAUNCHBUTTON   335     /* SessionBar PC/2's Launch icon */

#define ID_ICONEXIT             ID_BITMAPEXIT
#define ID_ICONMOVE             ID_BITMAPMOVE
#define ID_ICONHIDE             ID_BITMAPHIDE
#define ID_ICONZORDERTOP        ID_BITMAPZORDERTOP
#define ID_ICONZORDERBOTTOM     ID_BITMAPZORDERBOTTOM
#define ID_ICONCLOSE            ID_BITMAPCLOSE
#define ID_ICONMAXIMIZE         ID_BITMAPMAXIMIZE
#define ID_ICONMINIMIZE         ID_BITMAPMINIMIZE
#define ID_ICONRESTORE          ID_BITMAPRESTORE
#define ID_ICONSHUTDOWN         ID_BITMAPSHUTDOWN
#define ID_ICONPOWERDOWN        ID_BITMAPPOWERDOWN
#define ID_ICONLOCKUP           ID_BITMAPLOCKUP
#define ID_ICONHELP             ID_BITMAPHELP

#define ID_OVCLIENTMENU         400     /* Popup Menu for CTRL+button 1 on Overview client area */
#define ID_OVSUBMENUICONS       401     /* Submenu to enable Overview window's SmartIcons */
#define ID_OVICONEXIT           402
#define ID_OVICONMOVE           403
#define ID_OVICONHIDE           404
#define ID_OVICONZORDERTOP      405
#define ID_OVICONZORDERBOTTOM   406
#define ID_OVICONCLOSE          407
#define ID_OVICONMAXIMIZE       408
#define ID_OVICONMINIMIZE       409
#define ID_OVICONRESTORE        410
#define ID_OVICONSHUTDOWN       411
#define ID_OVICONPOWERDOWN      412
#define ID_OVICONLOCKUP         413
#define ID_OVICONHELP           414
#define ID_OVSUBMENUDEBUG       415     /* Submenu for PC/2 diagnostics */
#define ID_OVUNHANG             416
#define ID_OVDUMPPROCESS        417
#define ID_OVDUMPWTRD           418
#define ID_OVDUMPITRD           419
#define ID_OVDUMPNTRD           420

#define ID_SBPERFORMANCEMENU    450     /* Submenu for SessionBar processor status */
#define ID_SBSETTINGSMENU       451
#define ID_SBSETTINGS60S        452
#define ID_SBSETTINGS5M         453
#define ID_SBSETTINGS30M        454
#define ID_SBSETTINGS1H         455
#define ID_SBPROCESSORMENU      460
#define ID_SBPROCESSOR_CPU1     461
#define ID_SBPROCESSOR_CPULAST  ID_SBPROCESSORMENU+64
                                        /* Don't use up to 525 */

#define ID_SBSTATUSMENU         550     /* Submenu for SessionBar Statusarea */
#define ID_SBLEFTJUSTIFIED      551
#define ID_SBRIGHTJUSTIFIED     552

                                        /* Titlebar extenstion bitmaps in up state */
#define ID_BITMAP0TITLEBARBEGIN 0x8040
#define ID_BITMAP0CLOSEWINDOW   0x8040
#define ID_BITMAP0ROLLWINDOW    0x8041
#define ID_BITMAP0HELP          0x8042
#define ID_BITMAP0MOVEVD        0x8043
#define ID_BITMAP0MOVEEDGE      0x8044

                                        /* Offset to add to the titlebar bitmaps to get the bitmaps
                                           of the toggled state of the current state. This is
                                           required where two states (e.g. an "ON" and an "OFF" state)
                                           are available, like the maximize button of PM windows which
                                           is different for restored and maximized windows */
#define TOGGLESTATE             0x0010
                                        /* Titlebar extenstion bitmaps in toggled state */
                                        /* We're at a resource compiler limitation, because
                                           it can't understand the following
#define ID_BITMAP1CLOSEWINDOW   ID_BITMAP0CLOSEWINDOW   + TOGGLESTATE
#define ID_BITMAP1ROLLWINDOW    ID_BITMAP0ROLLWINDOW    + TOGGLESTATE
#define ID_BITMAP1HELP          ID_BITMAP0HELP          + TOGGLESTATE
#define ID_BITMAP1MOVEVD        ID_BITMAP0MOVEVD        + TOGGLESTATE
#define ID_BITMAP1MOVEEDGE      ID_BITMAP0MOVEEDGE      + TOGGLESTATE
                                         */
#define ID_BITMAP1CLOSEWINDOW   0x8050
#define ID_BITMAP1ROLLWINDOW    0x8051
#define ID_BITMAP1HELP          0x8052
#define ID_BITMAP1MOVEVD        0x8053
#define ID_BITMAP1MOVEEDGE      0x8054

                                        /* Offset to add to the titelbar bitmaps to get the bitmaps
                                           of the WARP 4 (aka Merlin) down state (which is the up
                                           state with dark grey and white exchanged) */
#define DOWNSTATE               0x0020
                                        /* Titlebar extenstion bitmaps in down state (WARP 4) */
                                        /* We're at a resource compiler limitation, because
                                           it can't understand the following
#define ID_BITMAP2CLOSEWINDOW   ID_BITMAP0CLOSEWINDOW   + DOWNSTATE
#define ID_BITMAP2ROLLWINDOW    ID_BITMAP0ROLLWINDOW    + DOWNSTATE
#define ID_BITMAP2HELP          ID_BITMAP0HELP          + DOWNSTATE
#define ID_BITMAP2MOVEVD        ID_BITMAP0MOVEVD        + DOWNSTATE
#define ID_BITMAP2MOVEEDGE      ID_BITMAP0MOVEEDGE      + DOWNSTATE
                                         */
#define ID_BITMAP2CLOSEWINDOW   0x8060
#define ID_BITMAP2ROLLWINDOW    0x8061
#define ID_BITMAP2HELP          0x8062
#define ID_BITMAP2MOVEVD        0x8063
#define ID_BITMAP2MOVEEDGE      0x8064

                                        /* Titlebar extenstion bitmaps in down and toggled
                                           state (WARP 4) */
                                        /* We're at a resource compiler limitation, because
                                           it can't understand the following
#define ID_BITMAP3CLOSEWINDOW   ID_BITMAP0CLOSEWINDOW   + DOWNSTATE + TOGGLESTATE
#define ID_BITMAP3ROLLWINDOW    ID_BITMAP0ROLLWINDOW    + DOWNSTATE + TOGGLESTATE
#define ID_BITMAP3HELP          ID_BITMAP0HELP          + DOWNSTATE + TOGGLESTATE
#define ID_BITMAP3MOVEVD        ID_BITMAP0MOVEVD        + DOWNSTATE + TOGGLESTATE
#define ID_BITMAP3MOVEEDGE      ID_BITMAP0MOVEEDGE      + DOWNSTATE + TOGGLESTATE
                                         */
#define ID_BITMAP3CLOSEWINDOW   0x8070
#define ID_BITMAP3ROLLWINDOW    0x8071
#define ID_BITMAP3HELP          0x8072
#define ID_BITMAP3MOVEVD        0x8073
#define ID_BITMAP3MOVEEDGE      0x8074

#define ID_MOVEPOINTER          330     /* Pointer active while moving windows on
                                           Overview window is enabled */
#define ID_ACTIONPOINTER        331     /* Pointer active while performing actions (except
                                           moving) on windows */

                                        /* Popup Menu menuentry's icon for executable where
                                           WinLoadFileIcon() fails (e.g. bad path,...) */
#define ID_ICONUNKNOWN          340
                                        /* Popup Menu menuentry's icon for WPS objects */
#define ID_ICONWPSOBJECT        341
                                        /* SessionBar and QuickSwitch icon for icons that could not
                                           be loaded from the session's systemmenu */
#define ID_ICONSESSIONUNKNOWN   342

                                        /* SessionBar scroll left pushbutton */
#define ID_BITMAPSCROLLLEFT     345
                                        /* SessionBar scroll right pushbutton */
#define ID_BITMAPSCROLLRIGHT    346

#define ID_EMERGENCYEXIT        350     /* User pressed ALT+CTRL+F3 to exit PC/2 in
                                           an undocumented way. This is for testing
                                           or debugging only, useful when running as WPS */

#define ID_POPUPMENU            5000    /* ID used by first user entered entry in the
                                           popup menu. It is incremented by 1 by tracing
                                           through the linked list for each entry (submenu
                                           or menuitem). */
#define USERITEMFIRST           ID_POPUPMENU
#define USERITEMLAST            ID_POPUPMENU+1499
