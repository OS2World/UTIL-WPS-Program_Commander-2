/***********************************************************************\
 *                             WPS2PC2.csc                             *
 *              Copyright (C) by Stangl Roman, 1994, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * WPS2PC2.C    PC/2's interface to the WPS. This WPS DLL allows you   *
 *              to drag WPS objects onto, which are then converted     *
 *              into a format understandable by PC/2. This newly       *
 *              structure can then be dropped onto PC/2's Setup dialog *
 *              to add Menuentries corresponding to the dropped WPS    *
 *              objects to PC/2's Popup Menu. E.g. this allows you to  *
 *              create a Menuentry that is equivalent to a WPS DOS     *
 *              program object, inclusive any DOS settings.            *
 *                                                                     *
 * SOMObject                                                           *
 *    WPObject                                                         *
 *       WPAbstract                                                    *
 *                                                                     *
\***********************************************************************/

include         <wpabs.sc>

class:          Wps2Pc2,
                external stem   = Wps2Pc2,
                local,
                external prefix = Wps2Pc2X_,
                classprefix     = Wps2Pc2C_,
                major version   = 1,
                minor version   = 1,
                file stem       = Wps2Pc2;

parent:         WPAbstract;

passthru:       C.ph;
endpassthru;

#
# Pass to .ih file
#
passthru:       C.ih;
#pragma strings(readonly)

#include        "PC2.h"                 /* User include files */

#define         _FILE_  "WPS2PC2.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;

#define         INCL_WPCLASS
#define         INCL_WPFOLDER
#include        <pmwp.h>

                                        /* Menu ID for "Open->WPS 2 PC/2" */
#define ID_WPS2PC2OPENMENU      (WPMENUID_USER+1)
                                        /* Menuitem ID for "Open->WPS 2 PC/2" */
#define ID_WPS2PC2OPEN          (WPMENUID_USER+2)
#define DebugBox(title, text) \
                              \
           WinMessageBox(     \
              HWND_DESKTOP,   \
              HWND_DESKTOP,   \
              (PSZ) text,     \
              (PSZ) title,    \
              20,             \
              MB_OK | MB_INFORMATION | MB_MOVEABLE)
endpassthru;

#
# Pass to .h file
#
passthru:       C.h, after;

void            FillList2Listbox(WPSOBJECTLIST *pListRoot,
                                 ULONG ulRecursionLevel);
/*--------------------------------------------------------------------------------------*\
 * METHOD: wpOpen                                                           ( ) Private *
 *                                                                          (x) Public  *
 * DESCRIPTION:                                                                         *
 *      Called when the WPS2PC2 WPS icon gets opened via the context menu.              *
\*--------------------------------------------------------------------------------------*/

WPSOBJECTLIST  *ConvertObject2MenuData(void *pObject,
                                       ULONG ulRecursionLevel);
/*--------------------------------------------------------------------------------------*\
 * METHOD: ConvertObject2MenuData                                           ( ) Private *
 *                                                                          (x) Public  *
 * DESCRIPTION:                                                                         *
 *      Called when a WPS instances have been dropped over WPS2PC2 to query the         *
 *      instances data, allocate MENUDATA structures in unnamed shared memory and       *
 *      convert WPS object data into a WPSOBJECTLIST LIST understandable by PC/2.       *
 *      The pointer pObject is a WPObject *, but under SOM 2 it is not defined when     *
 *      processing the header file, to keep a common code between SOM 1 and SOM 2 we    *
 *      have to add a cast.                                                             *
\*--------------------------------------------------------------------------------------*/

PBYTE           AllocateSharedMem(void);
/*--------------------------------------------------------------------------------------*\
 * METHOD: AllocateSharedMem                                                ( ) Private *
 *                                                                          (x) Public  *
 * DESCRIPTION:                                                                         *
 *      This function allocates a memory block of an unnamed shared memory, whose       *
 *      starting address is passed to PC2.EXE via Drag'n Drop which also frees this     *
 *      memory (otherwise WPS2PC2.DLL provides a menuitem to free it) before a new      *
 *      shared memory can be allocated. The allocated memory is not commited, so any    *
 *      access leads to an access violation, which invokes the exception handler to     *
 *      commit it.                                                                      *
\*--------------------------------------------------------------------------------------*/

PBYTE           GetFreeSharedMem(ULONG ulSharedMemSize);
/*--------------------------------------------------------------------------------------*\
 * METHOD: GetFreeSharedMem                                                 ( ) Private *
 *                                                                          (x) Public  *
 * DESCRIPTION:                                                                         *
 *      This function allocated the next free ulSharedMemSize sized block within the    *
 *      shared memory allocated by AllocateSharedMem().                                 *
 *      Pointer DLLPc2Wps.pbSharedMemNextFree is updated accordingly.                   *
\*--------------------------------------------------------------------------------------*/

void            DeAllocateSharedMem(void);
/*--------------------------------------------------------------------------------------*\
 * METHOD: DeAllocateSharedMem                                              ( ) Private *
 *                                                                          (x) Public  *
 * DESCRIPTION:                                                                         *
 *      This function deallocates the unnamed shared memory allocated with              *
 *      AllocateSharedMem().                                                            *
 *      The control structure entries about shared memory are updated accordingly.      *
\*--------------------------------------------------------------------------------------*/

#pragma linkage(_Exception, System)
ULONG           _Exception(PEXCEPTIONREPORTRECORD pExcRepRecord,
                           PEXCEPTIONREGISTRATIONRECORD pExcRegRecord,
                           PCONTEXTRECORD pCtxRecord,
                           PVOID  pVoid);
/*--------------------------------------------------------------------------------------*\
 * METHODE: _Exception                                                      ( ) Private *
 *                                                                          (x) Public  *
 * DESCRIPTION:                                                                         *
 *      This function is the exception handler for WPS2PC2.DLL. It is only used to      *
 *      commit unnamed shared memory, which was allocated uncommited and accessed,      *
 *      leading to an access violation.                                                 *
\*--------------------------------------------------------------------------------------*/

ULONG           ProcessDragOver(PDRAGINFO pdrgInfo);
/*--------------------------------------------------------------------------------------*\
 * METHOD: ProcessDragOver                                                  ( ) Private *
 *                                                                          (x) Public  *
 * DESCRIPTION:                                                                         *
 *      This function processes the dragover events to allow or deny dropping WPS       *
 *      Objects.                                                                        *
\*--------------------------------------------------------------------------------------*/

ULONG           ProcessDrop(PDRAGINFO pdrgInfo, PDRAGITEM pdrgItem);
/*--------------------------------------------------------------------------------------*\
 * METHOD: ProcessDrop                                                      ( ) Private *
 *                                                                          (x) Public  *
 * DESCRIPTION:                                                                         *
 *      This function processes the drop event, which does analyse the WPS Objects      *
 *      having been dropped.                                                            *
\*--------------------------------------------------------------------------------------*/

MRESULT EXPENTRY WPS2PC2_MainWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
/*--------------------------------------------------------------------------------------*\
 * METHOD: WPS2PC2_MainWindowProc                                           ( ) Private *
 *                                                                          (x) Public  *
 * DESCRIPTION:                                                                         *
 *      This function is the main window procedure of the window created when selecting *
 *      "Open -> WPS 2 PC/2" from the "Open ->" submenu at the context menu.            *
\*--------------------------------------------------------------------------------------*/

endpassthru;

#
# Define instance data
#
data:
VOID           *pVoid;                  /* Define some data to force a (dummy) GetData */

#
# Define new methods
#
methods:
/*--------------------------------------------------------------------------------------*\
 * Instance override methods.                                                           *
\*--------------------------------------------------------------------------------------*/
override        wpDragOver;
--
-- OVERRIDE: wpDragOver                                                     ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Called when a WPS Object is dragged over the WPS 2 PC2 icon. Accept object if
--      rendering method DRM_OBJECT and format DRF_OBJECT.
--
override        wpDrop;
--
-- OVERRIDE: wpDrop                                                         ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Called when a WPS Object was dropped over the WPS 2 PC2 icon. Query all objects
--      dropped, allocate a unnamed shared memory, copy object data into and allow this
--      shared memory to be dragged to PC/2, which will add Menuentries according to
--      the objects dropped.
--

override        wpModifyPopupMenu;
--
-- OVERRIDE: wpModifyPopupMenu                                              ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Overwrite to add the "Open->WPS 2 PC/2" menuitem to the "Open->" context menu.
--
override        wpMenuItemHelpSelected;
--
-- OVERRIDE: wpMenuItemHelpSelected                                         ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Called when receiving an help event from our menuitem we added to the context
--      menu.
--
override        wpMenuItemSelected;
--
-- OVERRIDE: wpMenuItemSelected                                             ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Called when receiving an event from our menuitem we added to the context menu.
--
override        wpOpen;
--
-- OVERRIDE: wpOpen                                                         ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Called when the WPS2PC2 WPS icon gets opened via the context menu.
--
/*--------------------------------------------------------------------------------------*\
 * Class override methods.                                                              *
\*--------------------------------------------------------------------------------------*/
override        wpclsQueryIconData, class;
--
-- OVERRIDE: wpclsQueryIconData                                             ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Called when querying the class's icon.
--
override        wpclsInitData, class;
--
-- OVERRIDE: wpclsInitData                                                  ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Called during class initalization.
--
override        wpclsUnInitData, class;
--
-- OVERRIDE: wpclsUnInitData                                                ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Called during class termination.
--
override        wpclsQueryDefaultHelp, class;
--
-- OVERRIDE: wpclsQueryDefaultHelp                                          ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Add default halp panels for WPS2PC2.
--
override        wpclsQueryDefaultView, class;
--
-- OVERRIDE: wpclsQueryDefaultView                                          ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Returns the default view for a new instance of this object, but WPS2PC2 should
--      be used only once, we just tell what the default open view is.
--
override        wpclsQueryStyle, class;
--
-- OVERRIDE: wpclsQueryStyle                                                ( ) Private
--                                                                          (x) Public
-- DESCRIPTION:
--      Returns the default style, used to inhibit creation of shadows, copies or
--      templates because WPS 2 PC/2 supports only one instance systemwide.
--

