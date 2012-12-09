/***********************************************************************\
 *                                PC2.c                                *
 *             Copyright (C) by Stangl Roman, 1996, 2000               *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * Resource.c   Dummy procedure to build a DLL containing the          *
 *              resources                                              *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#include        "PC2.h"                 /* User include files */

#define         _FILE_  "Resource.c "##BLDLEVEL_VERSION
static char     _VERSION_[]=_FILE_;
const ULONG     _PC2RESSIGNATURE=BLDLEVEL_SIGNATURE;

/*--------------------------------------------------------------------------------------*\
 * This procedure is a dummy function, just to get a function entry point in the DLL    *
 * we're attaching the resource into by the Resource Compiler.                          *
 * Req:                                                                                 *
 * Ret:                                                                                 *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
void    dummy(void)
{
}

