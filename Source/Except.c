/***********************************************************************\
 *                                PC2.c                                *
 *              Copyright (C) by Stangl Roman, 1997, 2000              *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
 * Except.c     Thread exception handler                               *
 *                                                                     *
\***********************************************************************/

#pragma strings(readonly)

#include    "PC2.h"

extern  HMTX    hmtxExceptionHandler;   /* DosKillThread()/ExceptionHandler PC/2 semaphore */

typedef struct  _EXCEPTIONDATA  EXCEPTIONDATA;
typedef APIRET  (APIENTRY PFNDOSQUERYTHREADCONTEXT) (TID tid, ULONG ulLevel, PCONTEXTRECORD pcontectrec);

ULONG           DumpException(PCONTEXTRECORD pcontextrec, EXCEPTIONDATA *pexcpdata);

                                        /* Datastructure dynamically allocated while processing
                                           an exception in the exception handler */
struct  _EXCEPTIONDATA
{
APIRET      apiretRc;
HMODULE     hmodModule;
HMODULE     hmodAlias;
PIB        *ppibThread;
TIB        *ptibThread;
UCHAR       aucModule[CCHMAXPATH];
UCHAR       aucError[CCHMAXPATH];
UCHAR       aucCurrentTime[26]; 
ULONG       ulStatus;
ULONG       ulIndex;
ULONG       ulObjNum;
ULONG       ulBuffLen;
ULONG       ulOffset;
ULONG       ulCountPages;
ULONG       ulFlagsPage;
ULONG      *pulStackWord;
ULONG      *pulStackBegin;
ULONG       ulOrdinalCurrent;    
ULONG       ulOrdinalNearest;    
PFN         pfnEntrypointCurrent;
PFN         pfnEntrypointNearest;
time_t      time_tCurrent;
struct tm  *ptmCurrent;
};

UCHAR  *pucLogFile;
FILE   *pfileLogFile;
        
#define LOGFILE pfileLogFile 

ULONG           DumpThread(PIB *ppibThread, TIB *ptibThread, ULONG ulThreadIndex)
{
EXCEPTIONDATA               excpdata;
CONTEXTRECORD               contextrec;
HEV                         hev;
HMODULE                     hmodDOSCALLS;
PFNDOSQUERYTHREADCONTEXT   *pfnDosQueryThreadContext;

/*                                                                                      *\
 * Initialize to dump a thread that is currently running after having it suspended.     *
\*                                                                                      */
                                    /* Serialize access by obtaining a sempahore */
if(DosCreateEventSem("\\SEM32\\PC2EXCEPTIONHANDLER", &hev, 0, (BOOL32)FALSE)!=NO_ERROR)
    {
    fprintf(LOGFILE, "\n-------------------------------------------------------------------------------\n");
    fprintf(LOGFILE, "Nested Thread Dump detected and ignored in exception handler\n");
    fprintf(LOGFILE, "-------------------------------------------------------------------------------\n");
    return(XCPT_CONTINUE_SEARCH);
    }
pfileLogFile=fopen(pucLogFile, "a");
if(pfileLogFile==NULL)
    pfileLogFile=fopen("C:\\PC2.TRP", "a");
memset(&excpdata, 0, sizeof(EXCEPTIONDATA));
memset(&contextrec, 0, sizeof(CONTEXTRECORD));
pfnDosQueryThreadContext=0;
contextrec.ContextFlags=CONTEXT_FULL;
excpdata.ptibThread=ptibThread;
fprintf(LOGFILE, "-------------------------------------------------------------------------------\n");
fprintf(LOGFILE, "Thread Dump Facility (you may regularily delete this file to free some space)\n\n"); 
fprintf(LOGFILE, "Version    : %s %s\n", BLDLEVEL_INFO, BLDLEVEL_VERSION); 
time(&excpdata.time_tCurrent);
excpdata.ptmCurrent=localtime(&excpdata.time_tCurrent);
strncpy(excpdata.aucCurrentTime, asctime(excpdata.ptmCurrent), 24);
fprintf(LOGFILE, "Time       : %s\n", excpdata.aucCurrentTime);
/*                                                                                      *\
 * Load Dos32QueryThreadContext() as this API is only supported in Warp4+               *
\*                                                                                      */
excpdata.ulStatus=TRUE;
if(DosLoadModule(excpdata.aucModule, sizeof(excpdata.aucModule)-1, "DOSCALLS", &hmodDOSCALLS)==NO_ERROR)
    DosQueryProcAddr(hmodDOSCALLS, 877, NULL, (PFN *)(&pfnDosQueryThreadContext));
if(ptibThread==0)
    {
    fprintf(LOGFILE, "Thread     : Unknown\n");
    excpdata.ulStatus=FALSE;
    }
if(pfnDosQueryThreadContext==NULL)
    {
    fprintf(LOGFILE, "\nDosQueryThreadContext() req. Warp 4+\n");
    excpdata.ulStatus=FALSE;
    }
if(excpdata.ulStatus==TRUE)
    {
/*                                                                                      *\
 * Dump the thread context to file.                                                     *
\*                                                                                      */
    if(DosSuspendThread(ptibThread->tib_ptib2->tib2_ultid)!=NO_ERROR)
        fprintf(LOGFILE, "\nDosSuspendThread() failed\n");
    else
        {
        DosBeep(1000, 500);             /* Allow OS/2 time to suspend thread */
        if(pfnDosQueryThreadContext(ptibThread->tib_ptib2->tib2_ultid, CONTEXT_FULL, &contextrec)!=NO_ERROR)
            fprintf(LOGFILE, "\nDosQueryThreadContext() failed\n");
        else
            {
            if(DosGetInfoBlocks(&excpdata.ptibThread, &excpdata.ppibThread)==NO_ERROR)
                {
                excpdata.ptibThread=ptibThread;
                if(DosQueryModuleName(excpdata.ppibThread->pib_hmte, sizeof(excpdata.aucModule), 
                    excpdata.aucModule)==NO_ERROR)
                    fprintf(LOGFILE, "Executable : %s\n", excpdata.aucModule); 
                if(contextrec.ContextFlags & CONTEXT_CONTROL)
                    {
                    if(DosQueryModFromEIP(&excpdata.hmodModule, &excpdata.ulObjNum, 
                        sizeof(excpdata.aucModule), excpdata.aucModule, 
                        &excpdata.ulOffset, contextrec.ctx_RegEip)==NO_ERROR)
                        {
                        if(DosQueryModuleName(excpdata.hmodModule, sizeof(excpdata.aucModule), excpdata.aucModule)==NO_ERROR)
                            fprintf(LOGFILE, "Module     : %s\n", excpdata.aucModule); 
                        fprintf(LOGFILE, "           : Handle: %08lX  Object: %ld Offset: %08lX\n", 
                            (ULONG)excpdata.hmodModule, excpdata.ulObjNum, excpdata.ulOffset);
                                        /* If we found we trapped in a DLL, and that DLL is
                                           not one of PC/2's DLLs, then try to locate the nearest
                                           DLL entrypoint, as for some traps in OS/2 code the
                                           caller can't be located via stackframes as the EBP
                                           is not used/is modified */
                        strupr(excpdata.aucModule);
                        if((excpdata.aucModule[0]!='\0') && (!strstr(excpdata.aucModule, "PC2")))
                            {
                            if(DosLoadModule(excpdata.aucError, sizeof(excpdata.aucError), excpdata.aucModule,
                                &excpdata.hmodAlias)==NO_ERROR)
                                for(excpdata.ulOrdinalCurrent=0; 
                                    excpdata.ulOrdinalCurrent<=65533; 
                                    excpdata.ulOrdinalCurrent++)
                                    {
                                    if(DosQueryProcAddr(excpdata.hmodModule, excpdata.ulOrdinalCurrent, 
                                        NULL, &excpdata.pfnEntrypointCurrent)==NO_ERROR)
                                        {
                                        if((ULONG)excpdata.pfnEntrypointCurrent<=(ULONG)contextrec.ctx_RegEip)
                                            {
                                            if(((ULONG)contextrec.ctx_RegEip-(ULONG)excpdata.pfnEntrypointCurrent)<
                                                ((ULONG)contextrec.ctx_RegEip-(ULONG)excpdata.pfnEntrypointNearest))
                                                {
                                                excpdata.pfnEntrypointNearest=excpdata.pfnEntrypointCurrent;
                                                excpdata.ulOrdinalNearest=excpdata.ulOrdinalCurrent;
                                                }
                                            }
                                        }
                                    }
                            fprintf(LOGFILE, "Ordinals   : ");
                            for(excpdata.ulOrdinalCurrent=0, excpdata.ulIndex=0; 
                                excpdata.ulOrdinalCurrent<=65533; 
                                excpdata.ulOrdinalCurrent++)
                                {
                                if(DosQueryProcAddr(excpdata.hmodModule, excpdata.ulOrdinalCurrent, 
                                    NULL, &excpdata.pfnEntrypointCurrent)==NO_ERROR)
                                    {
                                    fprintf(LOGFILE, "%5ld: %08lX ", excpdata.ulOrdinalCurrent, (ULONG)excpdata.pfnEntrypointCurrent);
                                    excpdata.ulIndex++;
                                    if((excpdata.ulIndex%4)==0)
                                        fprintf(LOGFILE, "\n             ");
                                    }
                                }
                            fprintf(LOGFILE, "\n");
                            if(excpdata.pfnEntrypointNearest!=0)
                                fprintf(LOGFILE, "Nearest    : Ordinal %ld\n", excpdata.ulOrdinalNearest);
                            if(excpdata.hmodAlias!=NULLHANDLE)
                                {
                                DosFreeModule(excpdata.hmodAlias);
                                excpdata.hmodAlias=NULLHANDLE;
                                }
                            }
                        }
                    }
                }
            fprintf(LOGFILE, "PID        : %04lX\n", ppibThread->pib_ulpid);
            fprintf(LOGFILE, "TID (PC/2) : %04lX ", ulThreadIndex);
            if(ulThreadIndex==ID_MAINTHREAD)
                fprintf(LOGFILE, "(%s)  Checkpoint: %08lX\n", "MTrd", 0L);
            else if(ulThreadIndex==ID_IOTHREAD)
                fprintf(LOGFILE, "(%s)  Checkpoint: %08lX\n", "ITrd", pHP->ulAliveMsgITrd);
            else if(ulThreadIndex==ID_NETTHREAD)
                fprintf(LOGFILE, "(%s)  Checkpoint: %08lX\n", "NTrd", pHP->ulAliveMsgNTrd);
            else if(ulThreadIndex==ID_WORKINGTHREAD)
                fprintf(LOGFILE, "(%s)  Checkpoint: %08lX\n", "WTrd", pHP->ulAliveMsgWTrd);
            else
                fprintf(LOGFILE, "\n");
            fprintf(LOGFILE, "TID (OS/2) : %04lX         PRTY: %08lX\n", 
                ptibThread->tib_ptib2->tib2_ultid,
                ptibThread->tib_ptib2->tib2_ulpri);

            DumpException(&contextrec, &excpdata);
            }
        if(DosResumeThread(ptibThread->tib_ptib2->tib2_ultid)!=NO_ERROR)
            fprintf(LOGFILE, "\nDosResumeThread() failed\n");
        }
    }
/*                                                                                      *\
 * Cleanup.                                                                             *
\*                                                                                      */
fprintf(LOGFILE, "-------------------------------------------------------------------------------\n");
fprintf(LOGFILE, "\n");
fclose(pfileLogFile);
DosFreeModule(hmodDOSCALLS);
DosCloseEventSem(hev);
return(NO_ERROR);
}

/*--------------------------------------------------------------------------------------*\
 * This procedure is an exception handler that can be installed on a per thread basis   *
 * (that is, it must be installed for each thread the exception should be traced).      *
 * Req:                                                                                 *
 *      pexcpreprec ... Pointer to the PEXCEPTIONREPORTRECORD passed by OS/2            *
 *      pexcpregrec ... Pointer to the PEXCEPTIONREGISTRATIONRECORD passed by OS/2      *
 *      pcontextrec ... Pointer to the PCONTEXTRECORD passed by OS/2                    *
 *      pvoid ......... Reserved                                                        *
 * Ret:                                                                                 *
 *      ulRc .......... Return code: XCPT_CONTINUE_SEARCH to pass exception on to next  *
 *                      exception handler in chain (the chain is pointed to by FS:0)    *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG _System   ThreadExceptionHandler(PEXCEPTIONREPORTRECORD pexcpreprec,
                                       PEXCEPTIONREGISTRATIONRECORD pexcpregrec,
                                       PCONTEXTRECORD pcontextrec,
                                       PVOID pvoid)
{
                                        /* We are mainly interested in access violations, as
                                           these are the main bugs caused by the programmer,
                                           and this is a programmer not end-user tool */
switch(pexcpreprec->ExceptionNum)
{
#ifndef DEBUG
case XCPT_BREAKPOINT:
#endif  /* DEBUG */
case XCPT_ACCESS_VIOLATION:
case XCPT_ILLEGAL_INSTRUCTION:
case XCPT_FLOAT_DENORMAL_OPERAND:
case XCPT_FLOAT_DIVIDE_BY_ZERO:
case XCPT_FLOAT_INEXACT_RESULT:
case XCPT_FLOAT_INVALID_OPERATION:
case XCPT_FLOAT_OVERFLOW:
case XCPT_FLOAT_STACK_CHECK:
case XCPT_FLOAT_UNDERFLOW:
case XCPT_INTEGER_DIVIDE_BY_ZERO:
case XCPT_INTEGER_OVERFLOW:
case XCPT_PRIVILEGED_INSTRUCTION:
    {
    HEV             hev;
    EXCEPTIONDATA  *pexcpdata;

                                        /* Serialize access by obtaining a sempahore */
    if(DosCreateEventSem("\\SEM32\\PC2EXCEPTIONHANDLER", &hev, 0, (BOOL32)FALSE)!=NO_ERROR)
        {
        fprintf(LOGFILE, "-------------------------------------------------------------------------------\n");
        fprintf(LOGFILE, "Nested Exception detected and ignored in exception handler\n");
        fprintf(LOGFILE, "-------------------------------------------------------------------------------\n");
        return(XCPT_CONTINUE_SEARCH);
        }
    pfileLogFile=fopen(pucLogFile, "a");
    if(pfileLogFile==NULL)
        pfileLogFile=fopen("C:\\PC2.TRP", "a");
    if((DosAllocMem((VOID **)&pexcpdata, sizeof(EXCEPTIONDATA), PAG_COMMIT|PAG_READ|PAG_WRITE)!=NO_ERROR) ||
        (pfileLogFile==NULL))
        return(XCPT_CONTINUE_SEARCH);
    memset(pexcpdata, 0, sizeof(EXCEPTIONDATA));
    fprintf(LOGFILE, "\n-------------------------------------------------------------------------------\n");
    fprintf(LOGFILE, "Exception Dump Facility (you may regularily delete this file to free some space)\n\n"); 
    fprintf(LOGFILE, "Version    : %s %s\n", BLDLEVEL_INFO, BLDLEVEL_VERSION); 
    time(&pexcpdata->time_tCurrent);
    pexcpdata->ptmCurrent=localtime(&pexcpdata->time_tCurrent);
    strncpy(pexcpdata->aucCurrentTime, asctime(pexcpdata->ptmCurrent), 24);
    fprintf(LOGFILE, "Time       : %s\n", pexcpdata->aucCurrentTime);
/*                                                                                      *\
 * Print the executable the exception occurred while running and the module (this may   *
 * be the executable or a dynamic link library) where the exception occurred.           *
\*                                                                                      */
    if(DosGetInfoBlocks(&pexcpdata->ptibThread, &pexcpdata->ppibThread)==NO_ERROR)
        {
        if(DosQueryModuleName(pexcpdata->ppibThread->pib_hmte, sizeof(pexcpdata->aucModule), 
            pexcpdata->aucModule)==NO_ERROR)
            fprintf(LOGFILE, "Executable : %s\n", pexcpdata->aucModule); 
        if(pcontextrec->ContextFlags & CONTEXT_CONTROL)
            {
            if(DosQueryModFromEIP(&pexcpdata->hmodModule, &pexcpdata->ulObjNum, 
                sizeof(pexcpdata->aucModule), pexcpdata->aucModule, 
                &pexcpdata->ulOffset, pcontextrec->ctx_RegEip)==NO_ERROR)
                {
                if(DosQueryModuleName(pexcpdata->hmodModule, sizeof(pexcpdata->aucModule), pexcpdata->aucModule)==NO_ERROR)
                    fprintf(LOGFILE, "Module     : %s\n", pexcpdata->aucModule); 
                fprintf(LOGFILE, "           : Handle: %08lX  Object: %ld Offset: %08lX\n", 
                    (ULONG)pexcpdata->hmodModule, pexcpdata->ulObjNum, pexcpdata->ulOffset);
                                        /* If we found we trapped in a DLL, and that DLL is
                                           not one of PC/2's DLLs, then try to locate the nearest
                                           DLL entrypoint, as for some traps in OS/2 code the
                                           caller can't be located via stackframes as the EBP
                                           is not used/is modified */
                strupr(pexcpdata->aucModule);
                if(!strstr(pexcpdata->aucModule, "PC2"))
                    {
                    if(DosLoadModule(pexcpdata->aucError, sizeof(pexcpdata->aucError), pexcpdata->aucModule,
                        &pexcpdata->hmodAlias)==NO_ERROR)
                        for(pexcpdata->ulOrdinalCurrent=0; 
                            pexcpdata->ulOrdinalCurrent<=65533; 
                            pexcpdata->ulOrdinalCurrent++)
                            {
                            if(DosQueryProcAddr(pexcpdata->hmodModule, pexcpdata->ulOrdinalCurrent, 
                                NULL, &pexcpdata->pfnEntrypointCurrent)==NO_ERROR)
                                {
                                if((ULONG)pexcpdata->pfnEntrypointCurrent<=(ULONG)pcontextrec->ctx_RegEip)
                                    {
                                    if(((ULONG)pcontextrec->ctx_RegEip-(ULONG)pexcpdata->pfnEntrypointCurrent)<
                                        ((ULONG)pcontextrec->ctx_RegEip-(ULONG)pexcpdata->pfnEntrypointNearest))
                                        {
                                        pexcpdata->pfnEntrypointNearest=pexcpdata->pfnEntrypointCurrent;
                                        pexcpdata->ulOrdinalNearest=pexcpdata->ulOrdinalCurrent;
                                        }
                                    }
                                }
                            }
                    fprintf(LOGFILE, "Ordinals   : ");
                    for(pexcpdata->ulOrdinalCurrent=0, pexcpdata->ulIndex=0; 
                        pexcpdata->ulOrdinalCurrent<=65533; 
                        pexcpdata->ulOrdinalCurrent++)
                        {
                        if(DosQueryProcAddr(pexcpdata->hmodModule, pexcpdata->ulOrdinalCurrent, 
                            NULL, &pexcpdata->pfnEntrypointCurrent)==NO_ERROR)
                            {
                            fprintf(LOGFILE, "%5ld: %08lX ", pexcpdata->ulOrdinalCurrent, (ULONG)pexcpdata->pfnEntrypointCurrent);
                            pexcpdata->ulIndex++;
                            if((pexcpdata->ulIndex%4)==0)
                                fprintf(LOGFILE, "\n             ");
                            }
                        }
                    fprintf(LOGFILE, "\n");
                    if(pexcpdata->pfnEntrypointNearest!=0)
                        fprintf(LOGFILE, "Nearest    : Ordinal %ld\n", pexcpdata->ulOrdinalNearest);
                    if(pexcpdata->hmodAlias!=NULLHANDLE)
                        {
                        DosFreeModule(pexcpdata->hmodAlias);
                        pexcpdata->hmodAlias=NULLHANDLE;
                        }
                    }
                }
            }
        fprintf(LOGFILE, "PID        : %04lX\n", pexcpdata->ppibThread->pib_ulpid);
        fprintf(LOGFILE, "TID (PC/2) : ");
        if(pexcpdata->ptibThread->tib_ptib2->tib2_ultid==pHP->ulThreadIDITrd)
            fprintf(LOGFILE, "%04lX (%s) Checkpoint: %08lX\n", ID_IOTHREAD, "ITrd", pHP->ulAliveMsgITrd);
        else if(pexcpdata->ptibThread->tib_ptib2->tib2_ultid==pHP->ulThreadIDNTrd)
            fprintf(LOGFILE, "%04lX (%s) Checkpoint: %08lX\n", ID_NETTHREAD, "NTrd", pHP->ulAliveMsgNTrd);
        else if(pexcpdata->ptibThread->tib_ptib2->tib2_ultid==pHP->ulThreadIDWTrd)
            fprintf(LOGFILE, "%04lX (%s) Checkpoint: %08lX\n", ID_WORKINGTHREAD, "WTrd", pHP->ulAliveMsgWTrd);
        else if(pexcpdata->ptibThread->tib_ptib2->tib2_ultid==1)
            fprintf(LOGFILE, "%04lX (%s) Checkpoint: %08lX\n", 1L, "MTrd", 0L);
        else
            fprintf(LOGFILE, "?\n");
        fprintf(LOGFILE, "TID (OS/2) : %04lX        PRTY: %08lX\n", 
            pexcpdata->ptibThread->tib_ptib2->tib2_ultid,
            pexcpdata->ptibThread->tib_ptib2->tib2_ulpri);
        fprintf(LOGFILE, "Ordinal    : %04lX\n", pexcpdata->ptibThread->tib_ordinal);
        }
/*                                                                                      *\
 * Print the type, address and diagnostic parameters of the exception. The header file  *
 * BSEXCPT.H contains all information required to interpret that data.                  *
\*                                                                                      */
    fprintf(LOGFILE, "Exception  : Type: %08lX  Address: %08lX\n             ", 
        pexcpreprec->ExceptionNum, (ULONG)pexcpreprec->ExceptionAddress);
    for(pexcpdata->ulIndex=0; pexcpdata->ulIndex<pexcpreprec->cParameters; pexcpdata->ulIndex++)
        fprintf(LOGFILE, "P%01ld=%08lX ", pexcpdata->ulIndex, pexcpreprec->ExceptionInfo[pexcpdata->ulIndex]); 
    fprintf(LOGFILE, "\n");
/*                                                                                      *\
 * Print the general exception information.                                             *
\*                                                                                      */
    DumpException(pcontextrec, pexcpdata);
    fprintf(LOGFILE, "-------------------------------------------------------------------------------\n");
    fprintf(LOGFILE, "\n");
    DosFreeMem(pexcpdata);
    fclose(pfileLogFile);
    DosCloseEventSem(hev);
    }
    break;

case XCPT_ASYNC_PROCESS_TERMINATE:
    {
    EXCEPTIONRECORD *pexcprec=(EXCEPTIONRECORD *)pexcpregrec;
    PID             pidOwner=0;
    TID             tidOwner=0;
    ULONG           ulSemCount=0;
    PIB             *ppibThread;
    TIB             *ptibThread;

#ifdef  DEBUG
    DosBeep(2000, 500);
    DosBeep(1000, 500);
    DosBeep(2000, 500);
    if(DosGetInfoBlocks(&ptibThread, &ppibThread)==NO_ERROR)
    printf("Except: XCPT_ASYNC_PROCESS_TERMINATE for thread %d detected\n", (int)ptibThread->tib_ptib2->tib2_ultid);
#endif  /* DEBUG */
                                        /* First, check if DosKillThread() was raised from within
                                           PC/2 or from an external application like e.g. WatchCat */
    DosQueryMutexSem(hmtxExceptionHandler, &pidOwner, &tidOwner, &ulSemCount);
#ifdef  DEBUG
    printf("Except: %s usage count is %d\n", PC2_EXCPHANDLER_SEM, ulSemCount);
#endif  /* DEBUG */
    if(ulSemCount==0)
        {
#ifdef  DEBUG
        printf("Except: External Exception, calling default handler\n");
#endif  /* DEBUG */
        return(XCPT_CONTINUE_SEARCH);
        }
                                        /* If we are really sure it's our exception handler,
                                           then see if we should unwind the C stack to return
                                           back to a known entrypoint */
    if((pexcpregrec->ExceptionHandler==ThreadExceptionHandler) &&
        (pexcprec->ulSignature==BLDLEVEL_SIGNATURE))
        {
#ifdef  DEBUG
        printf("Except: XCPT_ASYNC_PROCESS_TERMINATE found signature\n");
#endif  /* DEBUG */
        if(pexcprec->ulStatus & EXCEPTIONRECORD_JMPBUF)
            {
#ifdef  DEBUG
            printf("Except: XCPT_ASYNC_PROCESS_TERMINATE returning via longjmp\n");
#endif  /* DEBUG */
            longjmp(pexcprec->jmpbufReturn, -1);
#ifdef  DEBUG
            printf("Except: XCPT_ASYNC_PROCESS_TERMINATE returning failed\n");
#endif  /* DEBUG */
            }
        }
    }
    break;
}
return(XCPT_CONTINUE_SEARCH);
}

ULONG           DumpException(PCONTEXTRECORD pcontextrec, EXCEPTIONDATA *pexcpdata)
{
/*                                                                                      *\
 * Print the general purpose registers including the result of verifying access to the  *
 * memory pointed to by the register (as Intel architecture allows virtualyl all        *
 * registers to perform addressing.                                                     *
\*                                                                                      */
if(pcontextrec->ContextFlags & CONTEXT_INTEGER)
    {
    fprintf(LOGFILE, "Registers  : ");
                                        /* EAX */
    fprintf(LOGFILE, "EAX=%08lX  ", pcontextrec->ctx_RegEax); 
    pexcpdata->ulCountPages=1;
    pexcpdata->ulFlagsPage=0;
    pexcpdata->apiretRc=DosQueryMem((void *)pcontextrec->ctx_RegEax, &pexcpdata->ulCountPages, &pexcpdata->ulFlagsPage);
    if(pexcpdata->apiretRc==NO_ERROR)
        fprintf(LOGFILE, "valid   (%08lX)", pexcpdata->ulFlagsPage);
    else if(pexcpdata->apiretRc==ERROR_INVALID_ADDRESS)
        fprintf(LOGFILE, "invalid");
                                        /* EBX */
    fprintf(LOGFILE, "\n             EBX=%08lX  ", pcontextrec->ctx_RegEbx); 
    pexcpdata->ulCountPages=1;
    pexcpdata->ulFlagsPage=0;
    pexcpdata->apiretRc=DosQueryMem((void *)pcontextrec->ctx_RegEbx, &pexcpdata->ulCountPages, &pexcpdata->ulFlagsPage);
    if(pexcpdata->apiretRc==NO_ERROR)
        fprintf(LOGFILE, "valid   (%08lX)", pexcpdata->ulFlagsPage);
    else if(pexcpdata->apiretRc==ERROR_INVALID_ADDRESS)
        fprintf(LOGFILE, "invalid");
                                        /* ECX */
    fprintf(LOGFILE, "\n             ECX=%08lX  ", pcontextrec->ctx_RegEcx); 
    pexcpdata->ulCountPages=1;
    pexcpdata->ulFlagsPage=0;
    pexcpdata->apiretRc=DosQueryMem((void *)pcontextrec->ctx_RegEcx, &pexcpdata->ulCountPages, &pexcpdata->ulFlagsPage);
    if(pexcpdata->apiretRc==NO_ERROR)
        fprintf(LOGFILE, "valid   (%08lX)", pexcpdata->ulFlagsPage);
    else if(pexcpdata->apiretRc==ERROR_INVALID_ADDRESS)
        fprintf(LOGFILE, "invalid");
                                        /* EDX */
    fprintf(LOGFILE, "\n             EDX=%08lX  ", pcontextrec->ctx_RegEdx); 
    pexcpdata->ulCountPages=1;
    pexcpdata->ulFlagsPage=0;
    pexcpdata->apiretRc=DosQueryMem((void *)pcontextrec->ctx_RegEdx, &pexcpdata->ulCountPages, &pexcpdata->ulFlagsPage);
    if(pexcpdata->apiretRc==NO_ERROR)
        fprintf(LOGFILE, "valid   (%08lX)", pexcpdata->ulFlagsPage);
    else if(pexcpdata->apiretRc==ERROR_INVALID_ADDRESS)
        fprintf(LOGFILE, "invalid");
                                        /* ESI */
    fprintf(LOGFILE, "\n             ESI=%08lX  ", pcontextrec->ctx_RegEsi); 
    pexcpdata->ulCountPages=1;
    pexcpdata->ulFlagsPage=0;
    pexcpdata->apiretRc=DosQueryMem((void *)pcontextrec->ctx_RegEsi, &pexcpdata->ulCountPages, &pexcpdata->ulFlagsPage);
    if(pexcpdata->apiretRc==NO_ERROR)
        fprintf(LOGFILE, "valid   (%08lX)", pexcpdata->ulFlagsPage);
    else if(pexcpdata->apiretRc==ERROR_INVALID_ADDRESS)
        fprintf(LOGFILE, "invalid");
                                        /* EDI */
    fprintf(LOGFILE, "\n             EDI=%08lX  ", pcontextrec->ctx_RegEdi); 
    pexcpdata->ulCountPages=1;
    pexcpdata->ulFlagsPage=0;
    pexcpdata->apiretRc=DosQueryMem((void *)pcontextrec->ctx_RegEdi, &pexcpdata->ulCountPages, &pexcpdata->ulFlagsPage);
    if(pexcpdata->apiretRc==NO_ERROR)
        fprintf(LOGFILE, "valid   (%08lX)", pexcpdata->ulFlagsPage);
    else if(pexcpdata->apiretRc==ERROR_INVALID_ADDRESS)
        fprintf(LOGFILE, "invalid");
    fprintf(LOGFILE, "\n");
    }
/*                                                                                      *\
 * Print the selector registers. I don't know how to get the access rights and limit    *
 * data without going into the LDT, but under flat (32 bit) memory model it doesn't     *
 * matter a lot.                                                                        *
\*                                                                                      */
if(pcontextrec->ContextFlags & CONTEXT_SEGMENTS)
    {
    fprintf(LOGFILE, "             DS=%04lX  ES=%04lX  FS=%04lX  GS=%04lX\n", 
        pcontextrec->ctx_SegDs, pcontextrec->ctx_SegEs, pcontextrec->ctx_SegFs, pcontextrec->ctx_SegGs);
    }
/*                                                                                      *\
 * Print the instruction and stack data that really allows us to trace the cause of the *
 * access violation. Introductionary some words about function calling:                 *
 *                                                                                      *
 * Basically, to find the cause of the exception we are interested in the sequence of   *
 * functions called. We can find the (virtual) address of all public symbols in the     *
 * MAP file generated by the linker. We then have to find the actual invocations of the *
 * functions from walking the chain of stack frames.                                    *
 *                                                                                      *
 * A stack frame is a widely used datastructure to implement function calling. It is    *
 * implemented in a way that the pointer to a stack frame (EPB for Intel architecture)  *
 * is used to allow walking up the stack frames. Stack frames work like:                *
 *                                                                                      *
 * 1.) Stack immediately before implementation of a function call                       *
 *                                                                                      *
 *     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                                                             *
 *     ³ some data ...    ³ <-- ESP                                                     *
 *     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                                                             *
 *                                                                                      *
 * 2.) Stack after pushing the arguments of function call onto stack (Note: some        *
 *     arguments may be passed via registers)                                           *
 *                                                                                      *
 *     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                                                             *
 *     ³ some data ...    ³                                                             *
 *     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 *     ³ argument 1       ³                                                             *
 *     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 *     ³ argument 2       ³                                                             *
 *              ...                                                                     *
 *     ³ argument n       ³ <-- ESP                                                     *
 *     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                                                             *
 *                                                                                      *
 * 3.) Stack at the first staement of the function call                                 *
 *                                                                                      *
 *     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                                                             *
 *     ³ argument n       ³                                                             *
 *     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 *     ³ return address   ³ <-- ESP                                                     *
 *     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                                                             *
 *                                                                                      *
 * 4.) When using stack frames, the first statements of a function are usually          *
 *     PUSH EBP             ... Save old EBP (frame pointer) register                   *
 *     MOV  EBP, ESP        ... Copy top of stack pointer ESP to EBP (this allows the   *
 *                              parameters passed to the function and the automatic     *
 *                              data of that function to by accessed by [EBP + x] and   *
 *                              [EBP - y ]                                              *
 *     SUB  ESP, m          ... Subtract automatic (local) data of function, so that    *
 *                              the top of stack pointer ESP points to unused stack     *
 *                                                                                      *
 *     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                                                             *
 *     ³ return address   ³                                                             *
 *     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 *     ³ saved old EBP    ³ <-- EBP (EBP points now to the stack frame of the current   *
 *     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´     function. You can see, automatic data is addressed by   *
 *     ³ automatic data 1 ³     [EBP - y] and the function arguments are addressed by   *
 *     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´     [EBP + x]                                               *
 *     ³ automatic data 2 ³                                                             *
 *              ...                                                                     *
 *     ³ automatic data n ³ <-- ESP                                                     *
 *     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                                                             *
 *                                                                                      *
 * 5.) When exiting a function, step 4.) is done in reverse order                       *
 *                                                                                      *
 * 6.) Neglecting function arguments and automatic function data, multiple functions    *
 *     called on the stack now look like:                                               *
 *                                                                                      *
 * ³   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                                                             *
 * ³   ³ func 1 params    ³                                                             *
 * ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 * ³   ³ return address   ³ <-- Next statement when function 1 returns (e.g. in main()) *
 * ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 * ÀÄÄ ³ saved old EBP    ³ <-- EBP pointed here when function 1 was called             *
 * ÚÄ> ³                  ³                                                             *
 * ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 * ³   ³ func 1 auto data ³                                                             *
 * ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 * ³   ³ func 2 params    ³                                                             *
 * ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 * ³   ³ return address   ³ <-- Next statement of function 1 when function 2 returns    *
 * ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 * ÀÄÄ ³ saved old EBP    ³ <-- EBP pointed here when function 2 was called (Note: the  *
 * ÚÄ> ³                  ³     "old" value of EBP is exactly the pointer to the stack  *
 * ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´     frame of the previously called function 1)              *
 * ³   ³ func 2 auto data ³                                                             *
 * ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 * ³   ³ func 3 params    ³                                                             *
 * ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 * ³   ³ return address   ³ <-- Next statement of function 2 when function 3 returns    *
 * ³   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 * ÀÄÄ ³ saved old EBP    ³ <-- EBP (EBP when an exception occurs in function 3)        *
 *     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´                                                             *
 *     ³ func 3 auto data ³ <-- ESP (ESP when an exception occurs in function 3)        *
 *     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                                                             * 
 *                                                                                      *
 * 7.) Now, during the exception we can get the value of EBP (and ESP) out of the data  *
 *     structures passed by OS/2, which allows us first to walk up the chain of stack   *
 *     frames and second as we know that the return address is 1 word up the stack, we  *
 *     can also display the address a function was called from. And comparing that      *
 *     address with the addresses of all functions in the map file should allow us to   *
 *     find out the function calling sequence.                                          *
 *                                                                                      *
 * Note: It works this way only if running pure 32-bit code, as 16-bit code has only a  *
 *       16-bit stack where SP and BP would have to be used instead.                    *
\*                                                                                      */

/*                                                                                      *\
 * Also dump the module:memory object from each return address. If this address is in   *
 * your user application, then by taking the map file it is relatively easy to spot out *
 * the location in the source code, even when not having debugging information included *
 * in the executables. If this address is within OS/2 code, it is a little bit more     *
 * difficult to tell (mainly because the offsets change from Fixpak to Fixpak). It      *
 * works like this:                                                                     *
 *                                                                                      *
 * 1.) The format in the trap file for example contains:                                *
 *     Address  Module:Object                                                           *
 *     000428EF PC2:0                                                                   *
 *     1C0873C7 PMMERGE:3                                                               *
 *     0003E883 PC2:0                                                                   *
 *                                                                                      *
 * 2.) To find out where 00042BEF is, just take your mapfile. The first thing to check  *
 *     is if the address is in an executable or DLL. If it is in an executable, then    *
 *     memory object 0 starts at 0x00010000 (64kB) so you have to subtract it giving    *
 *     00032BEF as the address to search within the mapfile.                            *
 *                                                                                      *
 *     With 0032BEF you got into the list of "Publics by Value" which is already sorted *
 *     ascending and check for that address which is the nearest one before for that    *
 *     memory object. This for example may result in:                                   *
 *     0001:0002FC10 PC2_WorkThreadWindowProc                                           *
 *                                                                                      *
 *     Note: The map files generated by the IBM linker seem to count memory objects     *
 *           incremented by 1 compared to what DosQueryModFromEIP() returns, that's why *
 *           0001:0002FC10 matches 00032BEF best.                                       *
 *                                                                                      *
 *     Now that you know the function the next thing of interest is which source state- *
 *     ment actually failed (and if the function is rather large this is not trivial to *
 *     tell).                                                                           *
 *                                                                                      *
 *     If the executable contains debug code, just load it into the debugger and set a  *
 *     breakpoint at the address (00042BEF in this case) and then look at that break-   *
 *     point. The debugger will then display the source line, and by turning into the   *
 *     assembly view and the register contents you may even spot what went wrong        *
 *     exactly.                                                                         *
 *                                                                                      *
 *     If the executable contains no debug code, also load it into the debugger and set *
 *     a breakpoint at the address. Then write down the statements around the failing   *
 *     one. Hopefully during the build process you have also requested the compiler to  *
 *     also generate an assembler source file. Then either use an assembler to assemble *
 *     that assembler source file and generate a listing containing relative addresses  *
 *     and do some math to get the offset to the beginning of the function found in the *
 *     map file, or use the statements written down from the debugger to find similar   *
 *     ones (with some experience and some estimation where the statement is located    *
 *     about, you should find the failing statement).                                   *
 *                                                                                      *
 * 3.) To find out where 1C0873C7 is located, is much more difficult to tell if not     *
 *     impossible (without using the kernel debugger). All one knows is that is in      *
 *     memory object 3, thus you have to subtract 3 times 0x0001000 (64kB) giving       *
 *     1C0573C7. The high part depends on where the DLL is loaded into memory, thus may *
 *     be different between OS/2 versions, PCs or even reboots. Additionally and object *
 *     may be larger than 64kB so that it you have to calculate 1C040000 + 173C7        *
 *     instead of 1C050000 + 73C7.                                                      *
 *                                                                                      *
 *     How can you find out? Well, You can use EXEHDR -v to show the memory objects and *
 *     the ordinals. You need to determine the address of a known entry point in a DLL, *
 *     e.g. from WinGetMsg(), by a small program using DosQueryProcAddr(). As EXEHDR    *
 *     gives you the memory object of the known Api, you can calculate the DLL's base   *
 *     address, e.g. 1C040000 (in this case). Thus you know the address is 1C040000 +   *
 *     173C7. Then you have to look at all memory objects 2 with an address about 173C7 *
 *     again by looking into the EXEHDR output.                                         *
 *                                                                                      *
 *     In this case this gives nothing useful, however in this case 0003E883 PC2:0      *
 *     tells that WinGetMsg() was called before, so it is something internal to PM that *
 *     is called after WinGetMsg(). (As PM code likely contains jumps this explains     *
 *     why the address 1C0573C7 does not make sense, though it seems to be near to      *
 *     entrypoint 2183 in PMMERGE.DLL, but that entrypoint is not documented).          *
 *                                                                                      *
 *     One additional problem is that PMMERGE.DLL is a collection introduced with Warp3 *
 *     of initially individual DLLs, e.g. WinGetMsg() is entrypoint 65 in PMWIN.DLL,    *
 *     but gets forwarded to entrypoint 3030 in PMMERGE.DLL.                            *
 *                                                                                      *
 * In short, you may find some hints when you trap in OS/2, but you mostly will not,    *
 * however for your own DLLs this will work much better (as you have all the stuff      *
 * required for the analyis.                                                            *
\*                                                                                      */
if(pcontextrec->ContextFlags & CONTEXT_CONTROL)
    {
                                        /* Instruction data CS:EIP, CPU Flags */
    fprintf(LOGFILE, "Instruction: CS:EIP=%04lX:%08lX  ",
        pcontextrec->ctx_SegCs, pcontextrec->ctx_RegEip);
    pexcpdata->ulCountPages=1;
    pexcpdata->ulFlagsPage=0;
    pexcpdata->apiretRc=DosQueryMem((void *)pcontextrec->ctx_RegEip, &pexcpdata->ulCountPages, &pexcpdata->ulFlagsPage);
    if(pexcpdata->apiretRc==NO_ERROR)
        fprintf(LOGFILE, "valid   (%08lX)", pexcpdata->ulFlagsPage);
    else if(pexcpdata->apiretRc==ERROR_INVALID_ADDRESS)
        fprintf(LOGFILE, "invalid");
    fprintf(LOGFILE, "\n             FLG=%08lX", pcontextrec->ctx_EFlags);
                                        /* Stack data SS:ESP, stack frame EBP */
    fprintf(LOGFILE, "\nStack      : Base: %08lX  Limit: %08lX\n",
        (ULONG)(pexcpdata->ptibThread ? pexcpdata->ptibThread->tib_pstack : 0), 
        (ULONG)(pexcpdata->ptibThread ? pexcpdata->ptibThread->tib_pstacklimit :0));
    fprintf(LOGFILE, "             SS:ESP=%04lX:%08lX  ",
        pcontextrec->ctx_SegSs, pcontextrec->ctx_RegEsp);
    pexcpdata->ulCountPages=1;
    pexcpdata->ulFlagsPage=0;
    pexcpdata->apiretRc=DosQueryMem((void *)pcontextrec->ctx_RegEsp, &pexcpdata->ulCountPages, &pexcpdata->ulFlagsPage);
    if(pexcpdata->apiretRc==NO_ERROR)
        fprintf(LOGFILE, "valid   (%08lX)", pexcpdata->ulFlagsPage);
    else if(pexcpdata->apiretRc==ERROR_INVALID_ADDRESS)
        fprintf(LOGFILE, "invalid");
    fprintf(LOGFILE, "\n             EBP=%08lX  ", pcontextrec->ctx_RegEbp);
    pexcpdata->ulCountPages=1;
    pexcpdata->ulFlagsPage=0;
    pexcpdata->apiretRc=DosQueryMem((void *)pcontextrec->ctx_RegEbp, &pexcpdata->ulCountPages, &pexcpdata->ulFlagsPage);
    if(pexcpdata->apiretRc==NO_ERROR)
        fprintf(LOGFILE, "valid   (%08lX)", pexcpdata->ulFlagsPage);
    else if(pexcpdata->apiretRc==ERROR_INVALID_ADDRESS)
        fprintf(LOGFILE, "invalid");
    fprintf(LOGFILE, "\n\n");
    if(pexcpdata->ptibThread!=0)
        {
                                        /* Stack dump. If EBP within stack and smaller than ESP
                                           use EBP, otherwise use ESP (which points int the stack
                                           anyway, so no check needed) */
        fprintf(LOGFILE, "Stack dump :               +0         +4         +8         +C");
        if(pcontextrec->ctx_RegEbp<pcontextrec->ctx_RegEsp)
                                        /* I would have expected that EBP also should point into
                                           the stack, however when requesting a thread dump, EBP
                                           sometimes is outside the stack, but tracing the stack
                                           frames still works, so I assume some kind of stack switch
                                           that doesn't get reflected in the thread info block.
        if((pcontextrec->ctx_RegEbp<pcontextrec->ctx_RegEsp) &&
            (pcontextrec->ctx_RegEbp>=(ULONG)pexcpdata->ptibThread->tib_pstack) &&
            (pcontextrec->ctx_RegEbp<=(ULONG)pexcpdata->ptibThread->tib_pstacklimit))
                                           So I'll try to skip that verification, but to be on the
                                           safe side we verify access for each page */
            pexcpdata->pulStackWord=(ULONG *)(pcontextrec->ctx_RegEbp & 0xFFFFFFF0);
        else
            pexcpdata->pulStackWord=(ULONG *)(pcontextrec->ctx_RegEsp & 0xFFFFFFF0);
        for(pexcpdata->pulStackBegin=pexcpdata->pulStackWord; 
            pexcpdata->pulStackWord<=(ULONG *)pexcpdata->ptibThread->tib_pstacklimit;
            /* NOP */)
            {
                                        /* If we're on a page boundary, check access */
            if(((ULONG)pexcpdata->pulStackWord & 0x00000FFF)==0x00000000)
                {
                pexcpdata->ulCountPages=0x1000;
                pexcpdata->ulFlagsPage=0;
                pexcpdata->apiretRc=DosQueryMem((void *)pexcpdata->pulStackWord, &pexcpdata->ulCountPages, &pexcpdata->ulFlagsPage);
                if((pexcpdata->apiretRc!=NO_ERROR) ||
                    ((pexcpdata->apiretRc==NO_ERROR) && 
                        !(((pexcpdata->ulFlagsPage & (PAG_COMMIT|PAG_READ))==(PAG_COMMIT|PAG_READ)))))
                    {
                    fprintf(LOGFILE, "\n             %08lX: ", (ULONG)pexcpdata->pulStackWord);
                    fprintf(LOGFILE, "Page inaccessible");
                    pexcpdata->pulStackWord+=0x1000;
                    continue;
                    }
                }
            if(((ULONG)pexcpdata->pulStackWord & 0x0000000F)==0)
                fprintf(LOGFILE, "\n             %08lX: ", (ULONG)pexcpdata->pulStackWord);
            if((*pexcpdata->pulStackWord>=(ULONG)pexcpdata->pulStackBegin) &&
                (*pexcpdata->pulStackWord<=(ULONG)pexcpdata->ptibThread->tib_pstacklimit))
                fprintf(LOGFILE, "<%08lX> ", (ULONG)*pexcpdata->pulStackWord);
            else
                fprintf(LOGFILE, " %08lX  ", (ULONG)*pexcpdata->pulStackWord);
            pexcpdata->pulStackWord++;
            }
        fprintf(LOGFILE, "\n\n");
                                    /* Follow stack frames */
        fprintf(LOGFILE, "Stack frame: Address  Module:Object\n");
        pexcpdata->pulStackWord=(ULONG *)pcontextrec->ctx_RegEbp;
        while((pexcpdata->pulStackWord!=0) && 
            (pexcpdata->pulStackWord<(ULONG *)pexcpdata->ptibThread->tib_pstacklimit))
            {
            if(DosQueryModFromEIP(&pexcpdata->hmodModule, &pexcpdata->ulObjNum, 
                sizeof(pexcpdata->aucModule), pexcpdata->aucModule, 
                &pexcpdata->ulOffset, *(pexcpdata->pulStackWord+1))==NO_ERROR)
                {
                fprintf(LOGFILE, "             %08lX %s:%d", 
                    (ULONG)*(pexcpdata->pulStackWord+1), pexcpdata->aucModule, pexcpdata->ulObjNum);
                }
            else
                fprintf(LOGFILE, "             %08lX ", (ULONG)*(pexcpdata->pulStackWord+1));
            fprintf(LOGFILE, "\n");
            pexcpdata->pulStackWord=(ULONG *)*(pexcpdata->pulStackWord);
            }
        }
    }
return(NO_ERROR);
}
                                
/*--------------------------------------------------------------------------------------*\
 * This procedure is used to register and deregister the exception handler for a thread *
 * (that is, it must be installed for each thread the exception should be traced).      *
 * Req:                                                                                 *
 *      ulRegisterFlag  TRUE to register, FALSE to deregister                           *
 *      pexcpregrecThread                                                               *
 *                      Pointer to the EXCEPTIONREGISTRATIONRECORD that must be on the  *
 *                      stack of the thread we want to register the exception handler   *
 *                      for                                                             *
 *      pucExcpetionFile                                                                *
 *                      Pointer to the file information will be appended to during      *
 *                      processing an exception                                         *
 * Ret:                                                                                 *
 *      ulRc .......... NO_ERROR for success, error code otherwise                      *
 * Ref:                                                                                 *
\*--------------------------------------------------------------------------------------*/
ULONG   RegisterExceptionHandler(ULONG ulRegisterFlag, EXCEPTIONREGISTRATIONRECORD *pexcpregrecThread, UCHAR *pucExceptionFile)
{
APIRET  apiretRc;

if(ulRegisterFlag==TRUE)
    {
    pucLogFile=pucExceptionFile;
    pexcpregrecThread->ExceptionHandler=(ERR)ThreadExceptionHandler;
    apiretRc=DosSetExceptionHandler(pexcpregrecThread);
    }
else
    {
    apiretRc=DosUnsetExceptionHandler(pexcpregrecThread);
    }
return(apiretRc);
}
