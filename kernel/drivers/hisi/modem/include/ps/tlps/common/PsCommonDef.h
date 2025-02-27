/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


#ifndef __PSCOMMONDEF_H__
#define __PSCOMMONDEF_H__



/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "vos.h"
#include "v_timer.h"    /*lint !e537*/


#ifdef _lint
/* PCLINT特殊编译开关，只在PCLINT时打开 */
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#endif

#ifdef __RECUR_TEST__                                                           /* _H2ASN_Skip */
#include "RecurTest.h"                                                          /* _H2ASN_Skip */
#endif                                                                          /* _H2ASN_Skip */



/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  #pragma pack(*)    设置字节对齐方式
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/
#define PS_SUBMOD_NULL 0

#ifndef L2_LIBFUZZ_TEST
#define PS_PRINTF                                           (VOS_VOID)vos_printf
#else
#define PS_PRINTF
#endif

/* 协议栈日志打印新接口 */
#ifndef L2_LIBFUZZ_TEST
#define PS_PRINTF_FATAL(fmt, ...)               (mdrv_fatal(fmt, ##__VA_ARGS__))
#define PS_PRINTF_ERR(fmt, ...)                 (mdrv_err(fmt, ##__VA_ARGS__))
#define PS_PRINTF_WARNING(fmt, ...)             (mdrv_wrn(fmt, ##__VA_ARGS__))
#define PS_PRINTF_INFO(fmt, ...)                (mdrv_info(fmt, ##__VA_ARGS__))
#define PS_PRINTF_DEBUG(fmt, ...)               (mdrv_debug(fmt, ##__VA_ARGS__))
#else
#define PS_PRINTF_FATAL(fmt, ...)     0
#define PS_PRINTF_ERR(fmt, ...)       0
#define PS_PRINTF_WARNING(fmt, ...)   0
#define PS_PRINTF_INFO(fmt, ...)      0
#define PS_PRINTF_DEBUG(fmt, ...)     0
#endif



/* 内存操作封装 */
#ifdef _lint

#define PS_MEM_CMP(pDestBuffer,pSrcBuffer,Count)            memcmp(pDestBuffer,pSrcBuffer, Count )


/*申请消息包,申请的长度包括消息报头长度*/
/*lint -emacro({586}, PS_ALLOC_MSG_WITH_HEADER_LEN)*/
#define PS_ALLOC_MSG_WITH_HEADER_LEN(ulPid , ulLen)         malloc(ulLen)

/*申请消息包,申请的长度不包括消息报头长度*/
/*lint -emacro({586}, PS_ALLOC_MSG)*/
#define PS_ALLOC_MSG(ulPid , ulLen)                         malloc((ulLen) + VOS_MSG_HEAD_LENGTH)

/*消息发送*/
#ifdef LINUX_PC_LINT
#define PS_SEND_MSG(ulPid, pMsg)                            (free((VOS_VOID*)pMsg), ulPid)
/* Added for CHR 降功耗, 2017-11-20, begin */
#define PS_CHR_RPT_SEND_MSG(ulPid, pMsg)                    (free((VOS_VOID*)pMsg), ulPid)
/* Added for CHR 降功耗, 2017-11-20, end */

#else
/*lint -emacro({58}, PS_SEND_MSG)*/
/*lint -emacro({64}, PS_SEND_MSG)*/
/*lint -emacro({144}, PS_SEND_MSG)*/
/*lint -emacro({586}, PS_SEND_MSG)*/
/*lint -emacro({516}, PS_SEND_MSG)*/
#define PS_SEND_MSG(ulPid, pMsg)                            free(pMsg)

/*lint -emacro({64}, PS_CHR_RPT_SEND_MSG)*/
/*lint -emacro({144}, PS_CHR_RPT_SEND_MSG)*/
/* Added for CHR 降功耗, 2017-11-20, begin */
#define PS_CHR_RPT_SEND_MSG(ulPid, pMsg)                    free(pMsg)
/* Added for CHR 降功耗, 2017-11-20, end */
#endif

/*消息发送,由于PS_POST_MSG之后,需要使用者显示的释放消息包,因此这里不将其转定义为free*/
#define PS_POST_MSG(ulPid, pMsg)                            VOS_PostMsg( ulPid, pMsg )

/*消息发送*/
#ifdef LINUX_PC_LINT
#define PS_SEND_URGENT_MSG(ulPid, pMsg)                     (free(pMsg), ulPid)

#else
/*lint -emacro({58}, PS_SEND_URGENT_MSG)*/
/*lint -emacro({64}, PS_SEND_URGENT_MSG)*/
/*lint -emacro({144}, PS_SEND_URGENT_MSG)*/
/*lint -emacro({586}, PS_SEND_URGENT_MSG)*/
#define PS_SEND_URGENT_MSG(ulPid, pMsg)                     free(pMsg)

#endif

/*消息释放宏定义*/
#ifdef LINUX_PC_LINT
#define PS_FREE_MSG(ulPid, pMsg)                            (free(pMsg), ulPid)

#else
/*lint -emacro({58}, PS_FREE_MSG)*/
/*lint -emacro({64}, PS_FREE_MSG)*/
/*lint -emacro({144}, PS_FREE_MSG)*/
/*lint -emacro({586}, PS_FREE_MSG)*/
#define PS_FREE_MSG(ulPid, pMsg)                            free(pMsg)

#endif

/*内存申请宏定义*/
/*lint -emacro({586}, PS_MEM_ALLOC)*/
#define PS_MEM_ALLOC(ulPid , ulSize)                         malloc(ulSize)

/*内存释放宏定义*/
#ifdef LINUX_PC_LINT
#define PS_MEM_FREE(ulPid, pAddr )                          (free(pAddr), ulPid)

#else
/*lint -emacro({58}, PS_MEM_FREE)*/
/*lint -emacro({64}, PS_MEM_FREE)*/
/*lint -emacro({144}, PS_MEM_FREE)*/
/*lint -emacro({586}, PS_MEM_FREE)*/
#define PS_MEM_FREE(ulPid, pAddr )                          free(pAddr)

#endif

/*申请静态内存宏定义*/
/*lint -emacro({586}, PS_ALLOC_STATIC_MEM)*/
#define PS_ALLOC_STATIC_MEM(ulPid , ulSize)                 malloc(ulSize)

/*释放静态内存宏定义*/
#ifdef LINUX_PC_LINT
#define PS_FREE_STATIC_MEM(ulPid, pAddr )                   (free(pAddr), ulPid)

#else
/*lint -emacro({58}, PS_FREE_STATIC_MEM)*/
/*lint -emacro({64}, PS_FREE_STATIC_MEM)*/
/*lint -emacro({144}, PS_FREE_STATIC_MEM)*/
/*lint -emacro({586}, PS_FREE_STATIC_MEM)*/
#define PS_FREE_STATIC_MEM(ulPid, pAddr )                   free(pAddr)

#endif


/*内存申请*/
#define PS_ALLOC_MSG_ALL_CHECK(ulPid , ulLen)  \
            VOS_AllocMsg( ulPid, (ulLen)-(VOS_MSG_HEAD_LENGTH) )



/*Modified for UT,2010-2-1,begin*/
#elif defined(PS_UT_SWITCH)|| defined(_GAS_UT_SWITCH_)
#include "stdlib.h"


/*lint -emacro({586}, PS_ALLOC_MSG)*/
#define PS_ALLOC_MSG(ulPid , ulLen)                         malloc((ulLen) + VOS_MSG_HEAD_LENGTH)
/* add for clear define start*/
/*lint -emacro({586}, PS_ALLOC_MSG_WITH_HEADER_LEN)*/
#define PS_ALLOC_MSG_WITH_HEADER_LEN(ulPid , ulLen)         malloc(ulLen)
#define PS_POST_MSG(ulPid, pMsg)                            VOS_PostMsg(ulPid,pMsg)
/* add for clear define end*/
/*lint -emacro({586}, PS_SEND_MSG)*/
/*lint -emacro({516}, PS_SEND_MSG)*/
#define PS_SEND_MSG(ulPid, pMsg)                            free(pMsg)
/* Added for CHR 降功耗, 2017-11-20, begin */
#define PS_CHR_RPT_SEND_MSG(ulPid, pMsg)                    free(pMsg)
/* Added for CHR 降功耗, 2017-11-20, end */
/*lint -emacro({586}, PS_MEM_FREE)*/
#define PS_MEM_FREE(ulPid, pAddr )                          free(pAddr)
/*lint -emacro({586}, PS_MEM_ALLOC)*/
#define PS_MEM_ALLOC(ulPid , ulSize)                        malloc(ulSize)
/*lint -emacro({586}, PS_FREE_MSG)*/
#define PS_FREE_MSG(ulPid, pMsg)                            free(pMsg)
/*lint -emacro({586}, PS_FREE_STATIC_MEM)*/
#define PS_FREE_STATIC_MEM(ulPid, pAddr )                   free(pAddr)
#define PS_MEM_CMP(pDestBuffer,pSrcBuffer,Count)            memcmp(pDestBuffer, pSrcBuffer, Count )
#define PS_MEM_MOVE(pDestBuffer,pSrcBuffer,ulBuffLen)       memmove_s(pDestBuffer, ulBuffLen, pSrcBuffer, ulBuffLen)

#else
/*Modified by for UT,2010-2-1,end*/

#define PS_MEM_CPY(pucDestBuffer, pucSrcBuffer, ulBufferLen) \
            (VOS_VOID)VOS_MemCpy_s( pucDestBuffer, ulBufferLen, pucSrcBuffer, ulBufferLen )

/*内存内容填充宏定义*/
#define PS_MEM_SET(pucBuffer, ucData, ulBufferLen) \
            (VOS_VOID)VOS_MemSet_s( pucBuffer, ulBufferLen, ucData, ulBufferLen )

/*安全内存拷贝宏定义*/
#define PS_MEM_CPY_S(pucDestBuffer, ulDestBufferLen, pucSrcBuffer, ulSrcBufferLen) \
            (VOS_VOID)VOS_MemCpy_s( pucDestBuffer, ulDestBufferLen, pucSrcBuffer, ulSrcBufferLen )

/*安全内存内容填充宏定义*/
#define PS_MEM_SET_S(pucDestBuffer, ulDestBufferLen, ucData, ulCount) \
            (VOS_VOID)VOS_MemSet_s( pucDestBuffer, ulDestBufferLen, ucData, ulCount )

/*内存移动宏定义*/
#define PS_MEM_MOVE(pucDestBuffer, pucSrcBuffer, ulBufferLen) \
            VOS_MemMove_s( pucDestBuffer, ulBufferLen, pucSrcBuffer, ulBufferLen )

#define PS_MEM_CMP( pucDestBuffer, pucSrcBuffer, ulBufferLen ) \
            VOS_MemCmp( pucDestBuffer, pucSrcBuffer, ulBufferLen )

/*申请消息包,申请的长度包括消息报头长度*/
/*lint -emacro({586}, PS_ALLOC_MSG_WITH_HEADER_LEN)*/
#define PS_ALLOC_MSG_WITH_HEADER_LEN(ulPid , ulLen)  \
            VOS_AllocMsg( ulPid, (ulLen)-(VOS_MSG_HEAD_LENGTH) )

/*申请消息包,申请的长度不包括消息报头长度*/
/*lint -emacro({586}, PS_ALLOC_MSG)*/
#define PS_ALLOC_MSG(ulPid , ulLen)  \
            VOS_AllocMsg( ulPid, ulLen)

/*lint -emacro({586}, PS_ALLOC_MSG_ALL_CHECK)*/
#define PS_ALLOC_MSG_ALL_CHECK(ulPid , ulLen)  \
            VOS_AllocMsg( ulPid, (ulLen)-(VOS_MSG_HEAD_LENGTH) )

/*目前,协议栈OSA还不支持WIN32操作系统,在PC环境上测试时仍需要使用miniDOPRA,因此
  我们在这里将PS_SEND_MSG分别定义,区分WIN32和VXWORKS版本,对于WIN32版本,仍使用原
  Ps_SendMsg函数,以便于向PC STUB桩转发消息.后续OSA支持WIN32版本后,这里可统一处理*/
    #if(VOS_OS_VER == VOS_WIN32 )

        /*消息发送*/
        #ifdef __RECUR_TEST__

        #define PS_SEND_MSG(ulPid, pMsg) \
            RECRUTEST_PsSendMsg(ulPid, pMsg)

        #elif defined(PS_ITT_PC_TEST) 

        #define PS_SEND_MSG(ulPid, pMsg) \
            Ps_Itt_Stub_SendMsg(ulPid, pMsg) 

        #elif defined(PS_ITT_PC_TEST_L2)
        #define PS_SEND_MSG(ulPid, pMsg) \
            VOS_SendMsg(ulPid, pMsg)
        #else

        VOS_UINT32 Ps_SendMsg(VOS_INT8 *cFileName, VOS_UINT32 ulLine, VOS_PID ulPid, VOS_VOID * pMsg);

        #define PS_SEND_MSG(ulPid, pMsg) \
            Ps_SendMsg((VOS_INT8 *)__FILE__, __LINE__, ulPid, pMsg)

        /* Added for CHR 降功耗, 2017-11-20, begin */
        #define PS_CHR_RPT_SEND_MSG(ulPid, pMsg)  Ps_SendMsg((VOS_INT8 *)__FILE__, __LINE__, ulPid, pMsg)
        /* Added for CHR 降功耗, 2017-11-20, end */
        #endif
    #else
    /*消息发送*/
    /*lint --e(683)*/
    #define PS_SEND_MSG(ulPid, pMsg) \
            VOS_SendMsg( ulPid, pMsg)

    /* Added for CHR 降功耗, 2017-11-20, begin */
    #define PS_CHR_RPT_SEND_MSG(ulPid, pMsg)  Chr_MsgReport(pMsg)
    /* Added for CHR 降功耗, 2017-11-20, end */
    #endif

/*消息发送*/
#define PS_POST_MSG(ulPid, pMsg) \
            VOS_PostMsg( ulPid, pMsg)

/*消息发送*/
#define PS_SEND_URGENT_MSG(ulPid, pMsg) \
            VOS_SendUrgentMsg( ulPid, pMsg)


/*消息释放宏定义*/
#define PS_FREE_MSG(ulPid, pMsg) \
            VOS_FreeMsg( ulPid, pMsg)

/*内存申请宏定义*/
#if (VOS_OS_VER != VOS_WIN32)

/*lint -emacro({586}, PS_MEM_ALLOC)*/
#define PS_MEM_ALLOC(ulPid , ulSize) \
            VOS_MemAlloc( ulPid, (DYNAMIC_MEM_PT), ulSize)

#else   /*WIN32 DOPRA 1.6.1版本DYNAMIC_MEM_PT内存分配算法存在BUG，因此换成BLOCK_MEM_PT*/
/*lint -emacro({586}, PS_MEM_ALLOC)*/
#define PS_MEM_ALLOC(ulPid , ulSize) \
            VOS_MemAlloc( ulPid, (BLOCK_MEM_PT), ulSize)
#endif


/*内存释放宏定义*/
/*lint -emacro({586}, PS_MEM_FREE)*/
#define PS_MEM_FREE(ulPid, pAddr ) \
            VOS_MemFree( ulPid, pAddr)

/*申请静态内存宏定义*/
/*lint -emacro({586}, PS_ALLOC_STATIC_MEM)*/
#define PS_ALLOC_STATIC_MEM(ulPid , ulSize)\
            VOS_MemAlloc(ulPid, (STATIC_MEM_PT), ulSize)


/*释放静态内存宏定义*/
/*lint -emacro({586}, PS_FREE_STATIC_MEM)*/
#define PS_FREE_STATIC_MEM(ulPid, pAddr )\
            VOS_MemFree(ulPid, (pAddr))
#endif



/* 定时器封装 */

/* 启动定时器
Input Parameters
Pid: The function module ID of application.

ulLength:The duration of relative timer, in milliseconds.

ulName:Timer name. When the timer times out, this parameter will be passed to the application.

ulParam:Additional parameter. When the timer times out,
this parameter will be passed to the application.

ucMode: Work mode of relative timer:
VOS_RELTIMER_LOOP (loop);
VOS_RELTIMER_NOLOOP (no loop).

Output Parameters

phTm: The relative timer provided to the application by the system.
When phTm is VOS_NULL_PTR, ucMode is not allowed to be VOS_RELTIMER_LOOP.
*/

#if (VOS_OS_VER != VOS_WIN32)

    #define PS_START_REL_TIMER(phTm, ulPid, ulLength, ulName, ulParam, ucMode)\
                            VOS_StartRelTimer( phTm, ulPid, ulLength, ulName, ulParam, ucMode, VOS_TIMER_PRECISION_0 )

    #define PS_START_NO_PRECISION_REL_TIMER(phTm, ulPid, ulLength, ulName, ulParam, ucMode)\
                            VOS_StartRelTimer( phTm, ulPid, ulLength, ulName, ulParam, ucMode, VOS_TIMER_NO_PRECISION )

#else
/* dmt start*/
#ifndef DMT
#ifdef __PS_WIN32_RECUR__
    #define PS_START_REL_TIMER(phTm, ulPid, ulLength, ulName, ulParam, ucMode)\
        VOS_StartRelTimer( phTm, ulPid, ulLength, ulName, ulParam, ucMode ,VOS_TIMER_PRECISION_0)
    #define PS_START_NO_PRECISION_REL_TIMER(phTm, ulPid, ulLength, ulName, ulParam, ucMode)\
        VOS_StartRelTimer( phTm, ulPid, ulLength, ulName, ulParam, ucMode ,VOS_TIMER_NO_PRECISION)
#else
    #define PS_START_REL_TIMER(phTm, ulPid, ulLength, ulName, ulParam, ucMode)\
                            VOS_StartRelTimer( phTm, ulPid, ulLength, ulName, ulParam, ucMode )
    #define PS_START_NO_PRECISION_REL_TIMER(phTm, ulPid, ulLength, ulName, ulParam, ucMode)\
                            VOS_StartRelTimer( phTm, ulPid, ulLength, ulName, ulParam, ucMode )
#endif
#else
	#define PS_START_REL_TIMER(phTm, ulPid, ulLength, ulName, ulParam, ucMode)\
                            VOS_StartRelTimer( phTm, ulPid, ulLength, ulName, ulParam, ucMode ,VOS_TIMER_PRECISION_0)
    #define PS_START_NO_PRECISION_REL_TIMER(phTm, ulPid, ulLength, ulName, ulParam, ucMode)\
                            VOS_StartRelTimer( phTm, ulPid, ulLength, ulName, ulParam, ucMode ,VOS_TIMER_NO_PRECISION)
#endif
/* dmt end*/
#endif

/* 2013-7-14 VoLTE begin*/
#define PS_START_CALLBACK_REL_TIMER(phTm, ulPid, ulLength, ulName, ulParam, ucMode, TimeOutRoutine, ulPrecision)\
                            VOS_StartCallBackRelTimer(phTm, ulPid, ulLength, ulName, ulParam, ucMode, TimeOutRoutine, ulPrecision)
/* 2013-7-14 VoLTE end*/

#define PS_STOP_REL_TIMER(phTm)               VOS_StopRelTimer( phTm )

#define PS_GET_REL_TIMER_NAME(pMsg)          (((REL_TIMER_MSG *)pMsg)->ulName)

#define PS_GET_REL_TIMER_PARA(pMsg)          (((REL_TIMER_MSG *)pMsg)->ulPara)

#define PS_RESTART_REL_TIMER(phTm) \
        VOS_RestartRelTimer(phTm)

/*封装钩子函数注册函数*/
#define PS_REGISTER_MSG_GET_HOOK(pfnMsgHook) \
        VOS_RegisterMsgGetHook(pfnMsgHook)
/*reboot errorlog record 2015-4-1 begin*/
#if (VOS_WIN32 != VOS_OS_VER)
#ifdef _lint
#define PS_SND_MSG_ALL_CHECK(ulPid , pMsg)              PS_SEND_MSG(ulPid, pMsg)
#else

#define PS_SND_MSG_ALL_CHECK(ulPid , pMsg)              PS_OM_SendMsg(ulPid, pMsg)//PS_SEND_MSG(ulPid, pMsg)
#endif
#else
#define PS_SND_MSG_ALL_CHECK(ulPid , pMsg)              PS_SEND_MSG(ulPid, pMsg)
#endif
/*reboot errorlog record 2015-4-1 end*/

/* 发送紧急消息 */
#define PS_SND_URGENT_MSG(ulPid , pMsg)                 VOS_SendUrgentMsg(ulPid, pMsg)

#define PS_MEM_ALLOC_All_CHECK(ulPid , ulSize) \
            PS_MEM_ALLOC(ulPid , ulSize)

/*内存释放宏定义*/
#define PS_MEM_FREE_ALL_CHECK(ulPid, pAddr )            PS_MEM_FREE(ulPid, pAddr)

#define PS_FREE_MSG_ALL_CHECK(Pid, pMsg)                PS_FREE_MSG(Pid, pMsg)

/*add 2014-03-03 for migrating to new os start*/
/*flush和clear底层实现一样*/
#if (VOS_WIN32 != VOS_OS_VER)
#if (VOS_VXWORKS == VOS_OS_VER)
#define    LPS_CacheClear(pDataAddr, ulDataLen)         (VOS_VOID)cacheClear(DATA_CACHE, (VOS_VOID *)(pDataAddr), (ulDataLen))
#define    LPS_CacheFlush(pDataAddr, ulDataLen)         (VOS_VOID)cacheFlush(DATA_CACHE,(VOS_VOID *)(pDataAddr), (ulDataLen))
#define    LPS_CacheInvalidate(pDataAddr, ulDataLen)    (VOS_VOID)cacheInvalidate(DATA_CACHE,(VOS_VOID *)(pDataAddr), (ulDataLen))
#else
#define    LPS_CacheClear(pDataAddr, ulDataLen)         (VOS_VOID)OSAL_CacheFlush(OSAL_DATA_CACHE, (VOS_VOID *)(pDataAddr), (ulDataLen))
#define    LPS_CacheFlush(pDataAddr, ulDataLen)         (VOS_VOID)OSAL_CacheFlush(OSAL_DATA_CACHE, (VOS_VOID *)(pDataAddr), (ulDataLen))
#define    LPS_CacheInvalidate(pDataAddr, ulDataLen)    (VOS_VOID)OSAL_CacheInvalid(OSAL_DATA_CACHE, (VOS_VOID *)(pDataAddr), (ulDataLen))
#endif
/* mod begin, 2018-12-27 */
/* 增加函数入参: 字节对其方式enAlignPow */
/* begin, 2017-11-30, modify for replace_interface */
/* #else: VOS_UnCacheMemAlloc --> VOS_UnCacheMemAllocDebug */
/* #if: OSAL_CacheDmaMalloc --> VOS_UnCacheMemAllocDebug */
/* #if: OSAL_CacheDmaFree --> VOS_UnCacheMemFree */
#if (VOS_OS_VER == VOS_RTOSCK)
#define    LPS_CacheDmaMalloc(ulSize,enAlignPow,pDataAddr,uwCookie)        VOS_UnCacheMemAllocDebug(ulSize,enAlignPow,pDataAddr,uwCookie)
#define    LPS_CacheDmaFree(pBuf,ulSize)                        VOS_UnCacheMemFree(pBuf,pBuf,ulSize)
#else
#define    LPS_CacheDmaMalloc(ulSize,enAlignPow,pDataAddr,uwCookie)        VOS_UnCacheMemAllocDebug(ulSize,enAlignPow,pDataAddr,uwCookie)
#define    LPS_CacheDmaFree(pBuf,ulSize)                        VOS_UnCacheMemFree(pBuf,pBuf,ulSize)
#endif
/* end, 2017-11-30, modify for replace_interface */
/* mod end, 2018-12-27 */
#endif
/*add 2014-03-03 for migrating to new os end*/


#ifndef CONST
  #define CONST const
#endif

#ifndef STATIC
  #if (VOS_OS_VER == VOS_WIN32)
    #define STATIC
  #else
    #define STATIC static
  #endif
#endif



/*****************************************************************************
  3 Massage Declare
*****************************************************************************/



/*****************************************************************************
  4 Enum
*****************************************************************************/
/*add 2014-03-03 for migrating to new os start*/
typedef enum   /*CACHE TYPE, copy from BSP*/
{
    OSAL_INSTRUCTION_CACHE,
    OSAL_DATA_CACHE
}OSAL_CACHE_TYPE;
/*add 2014-03-03 for migrating to new os end*/

/*****************************************************************************
  5 STRUCT
*****************************************************************************/


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
/*add 2014-03-03 for migrating to new os start*/
/* cache相关操作，接口来自底软*/
extern int OSAL_CacheInvalid(OSAL_CACHE_TYPE type, void * address, unsigned int bytes);
extern int OSAL_CacheFlush(OSAL_CACHE_TYPE type, void * address, unsigned int bytes);
extern VOS_VOID* OSAL_CacheDmaMalloc(unsigned int  bytes);
extern int OSAL_CacheDmaFree(VOS_VOID*  pBuf);
/*add 2014-03-03 for migrating to new os end*/



/*****************************************************************************
  9 OTHERS
*****************************************************************************/




#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* PsOsAdapter.h */


