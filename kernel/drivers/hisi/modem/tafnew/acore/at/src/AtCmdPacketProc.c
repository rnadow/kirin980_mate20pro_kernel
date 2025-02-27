/*
* Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
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

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "AtCmdPacketProc.h"

#include "product_config.h"


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID                 PS_FILE_ID_AT_CMD_PACKET_PROC_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/


AT_APP_CONN_STATE_ENUM_U32 AT_AppConvertPdpStateToConnStatus(
    AT_PDP_STATE_ENUM_U8                enPdpState
)
{
    AT_APP_CONN_STATE_ENUM_U32          enConnStatus;

    switch (enPdpState)
    {
        case AT_PDP_STATE_ACTING:
            enConnStatus = AT_APP_DIALING;
            break;

        case AT_PDP_STATE_ACTED:
        case AT_PDP_STATE_DEACTING:
            enConnStatus = AT_APP_DIALED;
            break;

        case AT_PDP_STATE_IDLE:
        default:
            enConnStatus = AT_APP_UNDIALED;
            break;
    }

    return enConnStatus;
}


AT_APP_CONN_STATE_ENUM_U32 AT_GetAppConnStatus(
    AT_PS_CALL_ENTITY_STRU             *pstPsCallEntity,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
)
{
    AT_APP_CONN_STATE_ENUM_U32          enConnStatus;

    enConnStatus = AT_APP_UNDIALED;

    if (TAF_PDP_IPV4 == enPdpType)
    {
        if ( (VOS_TRUE == pstPsCallEntity->stApnDataSysInfo.ucDataSysInfoFlg)
          && (TAF_PS_APN_DATA_SYS_WLAN == pstPsCallEntity->stApnDataSysInfo.enCurrentDataSys)
          && (VOS_FALSE == pstPsCallEntity->stApnDataSysInfo.ucIsHandOver))
        {
            enConnStatus = AT_AppConvertPdpStateToConnStatus(pstPsCallEntity->stIpv4Info.enWlanIpv4State);
        }
        else
        {
            enConnStatus = AT_AppConvertPdpStateToConnStatus(pstPsCallEntity->stIpv4Info.enIpv4State);
        }
    }
    else if (TAF_PDP_IPV6 == enPdpType)
    {
        if ( (VOS_TRUE == pstPsCallEntity->stApnDataSysInfo.ucDataSysInfoFlg)
          && (TAF_PS_APN_DATA_SYS_WLAN == pstPsCallEntity->stApnDataSysInfo.enCurrentDataSys)
          && (VOS_FALSE == pstPsCallEntity->stApnDataSysInfo.ucIsHandOver))
        {
            enConnStatus = AT_AppConvertPdpStateToConnStatus(pstPsCallEntity->stIpv6Info.enWlanIpv6State);
        }
        else
        {
            enConnStatus = AT_AppConvertPdpStateToConnStatus(pstPsCallEntity->stIpv6Info.enIpv6State);
        }
    }
    else
    {
        AT_WARN_LOG("AT_GetAppConnStatus: enPdpType is error!");
    }

    return enConnStatus;
}


VOS_UINT32 AT_SetChdataPara_AppUser(VOS_UINT8 ucIndex)
{

    AT_MODEM_PS_CTX_STRU               *pstPsModemCtx   = VOS_NULL_PTR;
    CONST AT_CHDATA_RNIC_RMNET_ID_STRU *pstChDataCfg    = VOS_NULL_PTR;
    AT_CH_DATA_CHANNEL_ENUM_UINT32      enDataChannelId;
    VOS_UINT8                           ucLoop;
    VOS_UINT8                           ucCid;

    ucCid = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    pstPsModemCtx = AT_GetModemPsCtxAddrFromClientId(ucIndex);

    /* 指定CID的PDP若已经激活，则不允许删除或修改该CID的通道映射关系，直接返回ERROR */
    if ((VOS_TRUE == pstPsModemCtx->astChannelCfg[ucCid].ulUsed)
      && (VOS_TRUE == pstPsModemCtx->astChannelCfg[ucCid].ulRmNetActFlg))
    {
        return AT_ERROR;
    }

    /* 第二个参数为空，则表示删除配置关系 */
    if (0 == gastAtParaList[1].usParaLen)
    {
        pstPsModemCtx->astChannelCfg[ucCid].ulUsed     = VOS_FALSE;
        pstPsModemCtx->astChannelCfg[ucCid].ulRmNetId  = AT_PS_INVALID_RMNET_ID;
        return AT_OK;
    }

    enDataChannelId = gastAtParaList[1].ulParaValue;

    /* 获取网卡ID */
    pstChDataCfg = AT_PS_GetChDataCfgByChannelId(ucIndex,enDataChannelId);
    if (VOS_NULL_PTR == pstChDataCfg)
    {
        return AT_ERROR;
    }

    /* 查找是否有不同的<CID>配置了相同的<enRnicRmNetId> */
    for (ucLoop = 1; ucLoop <= TAF_MAX_CID; ucLoop++)
    {
        if ( (ucLoop != ucCid)
          && (VOS_TRUE == pstPsModemCtx->astChannelCfg[ucLoop].ulUsed)
          && (pstChDataCfg->enRnicRmNetId == (RNIC_DEV_ID_ENUM_UINT8)pstPsModemCtx->astChannelCfg[ucLoop].ulRmNetId))
        {
            /* 不同的<CID>配置了相同的<enRnicRmNetId>，直接返回ERROR */
            return AT_ERROR;
        }
    }

    /* 配置数传通道映射表 */
    pstPsModemCtx->astChannelCfg[ucCid].ulUsed    = VOS_TRUE;
    pstPsModemCtx->astChannelCfg[ucCid].ulRmNetId = pstChDataCfg->enRnicRmNetId;
    pstPsModemCtx->astChannelCfg[ucCid].ulIfaceId = pstChDataCfg->enIfaceId;

    return AT_OK;
}


VOS_UINT32 AT_SetChdataPara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucUserIndex;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 第一个参数为空 */
    if ((0 == gastAtParaList[0].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    ucUserIndex = ucIndex;

    /* PCUI口且已设置PCUI口模拟NDISDUP拨号 */
    if (AT_USBCOM_USER == gastAtClientTab[ucIndex].UserType)
    {
        if (VOS_TRUE == AT_GetPcuiPsCallFlag())
        {
            ucUserIndex = AT_GetPcuiUsertId();
        }
    }

    /* CTRL口且已设置CTRL口模拟NDISDUP拨号 */
    if (AT_CTR_USER == gastAtClientTab[ucIndex].UserType)
    {
        if (VOS_TRUE == AT_GetCtrlPsCallFlag())
        {
            ucUserIndex = AT_GetCtrlUserId();
        }
    }

    /* PCUI2口且已设置PCUI2口模拟NDISDUP拨号 */
    if (AT_PCUI2_USER == gastAtClientTab[ucIndex].UserType)
    {
        if (VOS_TRUE == AT_GetPcui2PsCallFlag())
        {
            ucUserIndex = AT_GetPcui2UserId();
        }
    }

    /* APP通道的处理 */
    if (VOS_TRUE == AT_CheckAppUser(ucUserIndex))
    {
        return AT_SetChdataPara_AppUser(ucUserIndex);
    }

    return AT_ERROR;
}


VOS_UINT32 AT_QryChdataPara_AppUser(TAF_UINT8 ucIndex)
{
    VOS_UINT8                           ucLoop;
    VOS_UINT8                           ucNum;
    VOS_UINT16                          usLength;
    AT_CH_DATA_CHANNEL_ENUM_UINT32      enDataChannelId;
    AT_MODEM_PS_CTX_STRU               *pstPsModemCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;

    ucNum    = 0;
    usLength = 0;

    pstPsModemCtx = AT_GetModemPsCtxAddrFromClientId(ucIndex);

    /* 输出结果 */
    for (ucLoop = 1; ucLoop <= TAF_MAX_CID; ucLoop++)
    {
        if ( (VOS_TRUE == pstPsModemCtx->astChannelCfg[ucLoop].ulUsed)
          && (AT_PS_INVALID_RMNET_ID != pstPsModemCtx->astChannelCfg[ucLoop].ulRmNetId) )
        {
            if (0 != ucNum)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
            }

            /* ^CHDATA:  */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"^CHDATA: ");

            /* <cid> */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%d", ucLoop);

            /* <datachannel> */
            ulRslt = AT_PS_GetChDataValueFromRnicRmNetId((RNIC_DEV_ID_ENUM_UINT8)(pstPsModemCtx->astChannelCfg[ucLoop].ulRmNetId), &enDataChannelId);

            if (VOS_OK != ulRslt)
            {
                return AT_ERROR;
            }

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,",%d", enDataChannelId);

            ucNum++;
        }
    }

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 AT_QryChdataPara(TAF_UINT8 ucIndex)
{
    VOS_UINT8                           ucUserId;

    /*命令状态类型检查*/
    if (AT_CMD_OPT_READ_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    ucUserId = ucIndex;

    /* PCUI口且已设置PCUI口模拟NDISDUP拨号 */
    if (AT_USBCOM_USER == gastAtClientTab[ucIndex].UserType)
    {
        if (VOS_TRUE == AT_GetPcuiPsCallFlag())
        {
            ucUserId = AT_GetPcuiUsertId();
        }
    }

    /* CTRL口且已设置CTRL口模拟NDISDUP拨号 */
    if (AT_CTR_USER == gastAtClientTab[ucIndex].UserType)
    {
        if (VOS_TRUE == AT_GetCtrlPsCallFlag())
        {
            ucUserId = AT_GetCtrlUserId();
        }
    }

    /* PCUI2口且已设置PCUI2口模拟NDISDUP拨号 */
    if (AT_PCUI2_USER == gastAtClientTab[ucIndex].UserType)
    {
        if (VOS_TRUE == AT_GetPcui2PsCallFlag())
        {
            ucUserId = AT_GetPcui2UserId();
        }
    }

    /* APP通道的处理 */
    if (VOS_TRUE == AT_CheckAppUser(ucUserId))
    {
        return AT_QryChdataPara_AppUser(ucUserId);
    }

    return AT_ERROR;
}


VOS_UINT32 AT_TestChdataPara(VOS_UINT8 ucIndex)
{
    /* 通道检查 */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: %s",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       CHDATA_TEST_CMD_PARA_STRING);

    return AT_OK;
}


VOS_UINT32 AT_PS_ReportDhcp(VOS_UINT8 ucIndex)
{
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity = VOS_NULL_PTR;
    AT_DHCP_PARA_STRU                   stDhcpConfig;
    AT_DISPLAY_RATE_STRU                stSpeed;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucCallId;

    usLength  = 0;
    TAF_MEM_SET_S(&stSpeed, sizeof(stSpeed), 0x00, (VOS_SIZE_T)(sizeof(AT_DISPLAY_RATE_STRU)));

    ucCallId = AT_PS_TransCidToCallId(ucIndex, (VOS_UINT8)gastAtParaList[0].ulParaValue);
    if (VOS_TRUE != AT_PS_IsCallIdValid(ucIndex, ucCallId))
    {
        AT_ERR_LOG("AT_PS_ReportDhcp:ERROR: CallId is invalid!");
        return AT_ERROR;
    }

    pstCallEntity = AT_PS_GetCallEntity(ucIndex, ucCallId);

    if ( (AT_PDP_STATE_ACTED != pstCallEntity->stIpv4Info.enIpv4State)
      && (AT_PDP_STATE_ACTED != pstCallEntity->stIpv4Info.enWlanIpv4State))
    {
        AT_WARN_LOG("AT_PS_ReportDhcp: PDP is not actived.");
        return AT_ERROR;
    }

    /* 获取接入理论带宽*/
    if (VOS_ERR == AT_GetDisplayRate(ucIndex, &stSpeed))
    {
        AT_WARN_LOG("AT_PS_ReportDhcp: ERROR : AT_GetDisplayRate Error!");
    }

    /* 获取DHCP参数(网络序) */
    AT_GetDhcpPara(&stDhcpConfig, &pstCallEntity->stIpv4Info.stIpv4DhcpInfo);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulIPAddr);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulSubNetMask);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulGateWay);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulGateWay);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulPrimDNS);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulSndDNS);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%s,",stSpeed.ucDlSpeed);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%s",stSpeed.ucUlSpeed);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 At_SetDhcpPara(VOS_UINT8 ucIndex)
{
    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 查询指定CID的实体PDP上下文 */
    return AT_PS_ReportDhcp(ucIndex);
}


VOS_UINT32 At_QryDhcpPara(VOS_UINT8 ucIndex)
{
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity = VOS_NULL_PTR;
    AT_DHCP_PARA_STRU                   stDhcpConfig;
    AT_DISPLAY_RATE_STRU                stSpeed;
    VOS_UINT32                          i;
    VOS_UINT32                          ulRst;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucCallId;

    usLength = 0;
    ulRst    = AT_ERROR;
    TAF_MEM_SET_S(&stSpeed, sizeof(stSpeed), 0x00, (VOS_SIZE_T)(sizeof(AT_DISPLAY_RATE_STRU)));

    /* 目前DHCP的查询只支持1-11的cid */
    for ( i = 1; i <= TAF_MAX_CID_NV; i++ )
    {
        ucCallId = AT_PS_TransCidToCallId(ucIndex, (VOS_UINT8)i);
        if (VOS_TRUE != AT_PS_IsCallIdValid(ucIndex, ucCallId))
        {
            continue;
        }

        pstCallEntity = AT_PS_GetCallEntity(ucIndex, ucCallId);

        if ( (AT_PDP_STATE_ACTED != pstCallEntity->stIpv4Info.enIpv4State)
          && (AT_PDP_STATE_ACTED != pstCallEntity->stIpv4Info.enWlanIpv4State))
        {
            continue;
        }

        /* 获取接入理论带宽*/
        if (VOS_ERR == AT_GetDisplayRate(ucIndex, &stSpeed))
        {
            AT_WARN_LOG("At_QryDhcpPara_UsbComUser: ERROR : AT_GetDisplayRate Error!");
        }

        /* 获取DHCP参数(网络序) */
        AT_GetDhcpPara(&stDhcpConfig, &pstCallEntity->stIpv4Info.stIpv4DhcpInfo);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulIPAddr);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulSubNetMask);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulGateWay);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulGateWay);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulPrimDNS);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%08X,",stDhcpConfig.stDhcpCfg.ulSndDNS);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%s,",stSpeed.ucDlSpeed);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,"%s",stSpeed.ucUlSpeed);

        ulRst    = AT_OK;
    }

    gstAtSendData.usBufLen = usLength;

    return ulRst;
}


VOS_UINT32 AT_TestDhcpPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-11)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}

#if (FEATURE_ON == FEATURE_IPV6)

VOS_UINT32 AT_PS_ReportDhcpv6(TAF_UINT8 ucIndex)
{
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity = VOS_NULL_PTR;
    AT_DISPLAY_RATE_STRU                stSpeed;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucCallId;
    VOS_UINT8                           aucIpv6AddrStr[TAF_MAX_IPV6_ADDR_COLON_STR_LEN];
    VOS_UINT8                           aucInvalidIpv6Addr[TAF_IPV6_ADDR_LEN];

    usLength  = 0;
    TAF_MEM_SET_S(&stSpeed, sizeof(stSpeed), 0x00, (VOS_SIZE_T)(sizeof(AT_DISPLAY_RATE_STRU)));
    TAF_MEM_SET_S(aucIpv6AddrStr, sizeof(aucIpv6AddrStr), 0x00, (TAF_MAX_IPV6_ADDR_COLON_STR_LEN));
    TAF_MEM_SET_S(aucInvalidIpv6Addr, sizeof(aucInvalidIpv6Addr), 0x00, TAF_IPV6_ADDR_LEN);

    ucCallId = AT_PS_TransCidToCallId(ucIndex, (VOS_UINT8)gastAtParaList[0].ulParaValue);
    if (VOS_TRUE != AT_PS_IsCallIdValid(ucIndex, ucCallId))
    {
        AT_ERR_LOG("AT_PS_ReportDhcpv6:ERROR: CallId is invalid!");
        return AT_ERROR;
    }

    pstCallEntity = AT_PS_GetCallEntity(ucIndex, ucCallId);

    if ( (AT_PDP_STATE_ACTED != pstCallEntity->stIpv6Info.enIpv6State)
      && (AT_PDP_STATE_ACTED != pstCallEntity->stIpv6Info.enWlanIpv6State))
    {
        AT_WARN_LOG("AT_PS_ReportDhcpv6: PDP is not actived.");
        return AT_ERROR;
    }

    if (VOS_TRUE != pstCallEntity->stIpv6Info.stIpv6RaInfo.bitOpPrefixAddr)
    {
        AT_WARN_LOG("AT_PS_ReportDhcpv6: Prefix address is not received.");
        return AT_ERROR;
    }

    /* 获取接入理论带宽*/
    if (VOS_ERR == AT_GetDisplayRate(ucIndex, &stSpeed))
    {
        AT_WARN_LOG("AT_PS_ReportDhcpv6: ERROR : AT_GetDisplayRate Error!");
    }

    usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr, "%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    /* 填写IPV6地址 */
    AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                      pstCallEntity->stIpv6Info.stIpv6DhcpInfo.aucIpv6Addr,
                                      TAF_IPV6_STR_RFC2373_TOKENS);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, "%s", aucIpv6AddrStr);

    /* 填写IPV6掩码, 该字段填全0 */
    AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                      aucInvalidIpv6Addr,
                                      TAF_IPV6_STR_RFC2373_TOKENS);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", aucIpv6AddrStr);

    /* 填写IPV6网关, 该字段填全0 */
    AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                      aucInvalidIpv6Addr,
                                      TAF_IPV6_STR_RFC2373_TOKENS);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", aucIpv6AddrStr);

    /* 填写DHCP IPV6, 该字段填全0 */
    AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                      aucInvalidIpv6Addr,
                                      TAF_IPV6_STR_RFC2373_TOKENS);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", aucIpv6AddrStr);

    /* 填写IPV6 Primary DNS */
    AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                      pstCallEntity->stIpv6Info.stIpv6DhcpInfo.aucIpv6PrimDNS,
                                      TAF_IPV6_STR_RFC2373_TOKENS);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", aucIpv6AddrStr);

    /* 填写IPV6 Secondary DNS */
    AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                      pstCallEntity->stIpv6Info.stIpv6DhcpInfo.aucIpv6SecDNS,
                                      TAF_IPV6_STR_RFC2373_TOKENS);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", aucIpv6AddrStr);

    /* 填写MAX RX/TX Rate */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", stSpeed.ucDlSpeed);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", stSpeed.ucUlSpeed);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 AT_SetDhcpv6Para(VOS_UINT8 ucIndex)
{
    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数错误 */
    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 查询指定CID的实体PDP上下文 */
    return AT_PS_ReportDhcpv6(ucIndex);
}


VOS_UINT32 AT_QryDhcpv6Para(VOS_UINT8 ucIndex)
{
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity = VOS_NULL_PTR;
    VOS_UINT8                           aucIpv6AddrStr[TAF_MAX_IPV6_ADDR_COLON_STR_LEN];
    VOS_UINT8                           aucInvalidIpv6Addr[TAF_IPV6_ADDR_LEN];
    AT_DISPLAY_RATE_STRU                stSpeed;
    VOS_UINT32                          i;
    VOS_UINT32                          ulRst;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucCallId;

    usLength  = 0;
    ulRst     = AT_ERROR;
    TAF_MEM_SET_S(&stSpeed, sizeof(stSpeed), 0x00, (VOS_SIZE_T)(sizeof(AT_DISPLAY_RATE_STRU)));

    TAF_MEM_SET_S(aucIpv6AddrStr, TAF_MAX_IPV6_ADDR_COLON_STR_LEN, 0x00, TAF_MAX_IPV6_ADDR_COLON_STR_LEN);
    TAF_MEM_SET_S(aucInvalidIpv6Addr, TAF_IPV6_ADDR_LEN, 0x00, TAF_IPV6_ADDR_LEN);

    /* 目前DHCP的查询只支持1-11的cid */
    for ( i = 1; i <= TAF_MAX_CID_NV; i++ )
    {

        ucCallId = AT_PS_TransCidToCallId(ucIndex, (VOS_UINT8)i);
        if (VOS_TRUE != AT_PS_IsCallIdValid(ucIndex, ucCallId))
        {
            continue;
        }

        pstCallEntity = AT_PS_GetCallEntity(ucIndex, ucCallId);

        if ( (AT_PDP_STATE_ACTED != pstCallEntity->stIpv6Info.enIpv6State)
          && (AT_PDP_STATE_ACTED != pstCallEntity->stIpv6Info.enWlanIpv6State))
        {
            continue;
        }

        if (VOS_TRUE != pstCallEntity->stIpv6Info.stIpv6RaInfo.bitOpPrefixAddr)
        {
            continue;
        }

        /* 获取接入理论带宽*/
        if (VOS_ERR == AT_GetDisplayRate(ucIndex, &stSpeed))
        {
            AT_WARN_LOG("AT_QryDhcpv6Para: ERROR : AT_GetDisplayRate Error!");
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, "%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        /* 填写IPV6地址 */
        AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                          pstCallEntity->stIpv6Info.stIpv6DhcpInfo.aucIpv6Addr,
                                          TAF_IPV6_STR_RFC2373_TOKENS);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, "%s", aucIpv6AddrStr);

        /* 填写IPV6掩码, 该字段填全0 */
        AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                          aucInvalidIpv6Addr,
                                          TAF_IPV6_STR_RFC2373_TOKENS);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", aucIpv6AddrStr);

        /* 填写IPV6网关, 该字段填全0 */
        AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                          aucInvalidIpv6Addr,
                                          TAF_IPV6_STR_RFC2373_TOKENS);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", aucIpv6AddrStr);

        /* 填写DHCP IPV6, 该字段填全0 */
        AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                          aucInvalidIpv6Addr,
                                          TAF_IPV6_STR_RFC2373_TOKENS);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", aucIpv6AddrStr);

        /* 填写IPV6 Primary DNS */
        AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                          pstCallEntity->stIpv6Info.stIpv6DhcpInfo.aucIpv6PrimDNS,
                                          TAF_IPV6_STR_RFC2373_TOKENS);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", aucIpv6AddrStr);

        /* 填写IPV6 Secondary DNS */
        AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                          pstCallEntity->stIpv6Info.stIpv6DhcpInfo.aucIpv6SecDNS,
                                          TAF_IPV6_STR_RFC2373_TOKENS);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", aucIpv6AddrStr);

        /* 填写MAX RX/TX Rate */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", stSpeed.ucDlSpeed);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength, ",%s", stSpeed.ucUlSpeed);

        ulRst    = AT_OK;
    }

    gstAtSendData.usBufLen = usLength;

    return ulRst;
}


VOS_UINT32 AT_TestDhcpv6Para(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-11)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}

VOS_UINT32 AT_PS_ReportApraInfo(TAF_UINT8 ucIndex)
{
    VOS_UINT16                          usLength;
    VOS_UINT8                           aucIpv6AddrStr[TAF_MAX_IPV6_ADDR_COLON_STR_LEN];
    VOS_UINT8                           aucInvalidIpv6Addr[TAF_IPV6_ADDR_LEN];
    VOS_UINT32                          ulMtuSize;
    VOS_UINT32                          ulPrefixBitLen;
    VOS_UINT32                          ulPreferredLifetime;
    VOS_UINT32                          ulValidLifetime;
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity = VOS_NULL_PTR;
    VOS_UINT8                           ucCallId;

    usLength                            = 0;
    TAF_MEM_SET_S(aucInvalidIpv6Addr, sizeof(aucInvalidIpv6Addr), 0x00, sizeof(aucInvalidIpv6Addr));
    TAF_MEM_SET_S(aucIpv6AddrStr, sizeof(aucIpv6AddrStr), 0x00, (TAF_MAX_IPV6_ADDR_COLON_STR_LEN));

    ucCallId = AT_PS_TransCidToCallId(ucIndex, (VOS_UINT8)gastAtParaList[0].ulParaValue);
    if (VOS_TRUE != AT_PS_IsCallIdValid(ucIndex, ucCallId))
    {
        AT_ERR_LOG("AT_PS_ReportApraInfo:ERROR: CallId is invalid!");
        return AT_ERROR;
    }

    pstCallEntity = AT_PS_GetCallEntity(ucIndex, ucCallId);

    if (AT_PDP_STATE_ACTED != pstCallEntity->stIpv6Info.enIpv6State)
    {
        AT_WARN_LOG("AT_PS_ReportApraInfo: PDP is not actived in cellular.");
        return AT_ERROR;
    }

    usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr,
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    /* 填写MTU */
    if (VOS_FALSE == pstCallEntity->stIpv6Info.stIpv6RaInfo.bitOpMtuSize)
    {
        ulMtuSize = 0;
    }
    else
    {
        ulMtuSize = pstCallEntity->stIpv6Info.stIpv6RaInfo.ulMtuSize;
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                       "%d",
                                       ulMtuSize);
    /* 填写Prefix */
    if (VOS_FALSE == pstCallEntity->stIpv6Info.stIpv6RaInfo.bitOpPrefixAddr)
    {
        ulPrefixBitLen = 0;
        AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                          aucInvalidIpv6Addr,
                                          TAF_IPV6_STR_RFC2373_TOKENS);
    }
    else
    {
        ulPrefixBitLen = pstCallEntity->stIpv6Info.stIpv6RaInfo.ulPrefixBitLen;
        AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                          pstCallEntity->stIpv6Info.stIpv6RaInfo.aucPrefixAddr,
                                          TAF_IPV6_STR_RFC2373_TOKENS);
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                       ",\"%s/%d\"",
                                       aucIpv6AddrStr,
                                       ulPrefixBitLen);

    /* 填写Preferred Lifetime */
    if (VOS_FALSE == pstCallEntity->stIpv6Info.stIpv6RaInfo.bitOpPreferredLifetime)
    {
        ulPreferredLifetime = 0;
    }
    else
    {
        ulPreferredLifetime = pstCallEntity->stIpv6Info.stIpv6RaInfo.ulPreferredLifetime;
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                       ",%u",
                                       ulPreferredLifetime);

    if (VOS_FALSE == pstCallEntity->stIpv6Info.stIpv6RaInfo.bitOpValidLifetime)
    {
        ulValidLifetime = 0;
    }
    else
    {
        ulValidLifetime = pstCallEntity->stIpv6Info.stIpv6RaInfo.ulValidLifetime;
    }

    /* 填写Valid Lifetime */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                       ",%u",
                                       ulValidLifetime);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 AT_SetApRaInfoPara(VOS_UINT8 ucIndex)
{
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数错误 */
    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 查询指定CID的实体PDP上下文 */
    return AT_PS_ReportApraInfo(ucIndex);
}


VOS_UINT32 AT_QryApRaInfoPara(VOS_UINT8 ucIndex)
{
    AT_PS_CALL_ENTITY_STRU             *pstAppPdpEntity;
    AT_IPV6_RA_INFO_STRU               *pstAppRaInfoAddr;
    VOS_UINT32                          ulMtuSize;
    VOS_UINT32                          ulPrefixBitLen;
    VOS_UINT32                          ulPreferredLifetime;
    VOS_UINT32                          ulValidLifetime;
    VOS_UINT16                          usLength;
    VOS_UINT8                           aucIpv6AddrStr[TAF_MAX_IPV6_ADDR_COLON_STR_LEN];
    VOS_UINT8                           aucInvalidIpv6Addr[TAF_IPV6_ADDR_LEN];
    VOS_UINT32                          ulCallIndex;

    usLength                            = 0;

    TAF_MEM_SET_S(aucInvalidIpv6Addr, sizeof(aucInvalidIpv6Addr), 0x00, sizeof(aucInvalidIpv6Addr));

    for (ulCallIndex = AT_PS_APP_CALL_ID_BEGIN;  ulCallIndex <= AT_PS_APP_CALL_ID_END; ulCallIndex++)
    {
        pstAppPdpEntity = AT_PS_GetCallEntity(ucIndex, (VOS_UINT8)ulCallIndex);
        pstAppRaInfoAddr = &pstAppPdpEntity->stIpv6Info.stIpv6RaInfo;

        if (AT_PDP_STATE_ACTED != pstAppPdpEntity->stIpv6Info.enIpv6State)
        {
            continue;
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           "%s: %d,",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           pstAppPdpEntity->stUserInfo.ucUsrCid);
        /* 填写MTU */
        if (VOS_FALSE == pstAppRaInfoAddr->bitOpMtuSize)
        {
            ulMtuSize = 0;
        }
        else
        {
            ulMtuSize = pstAppRaInfoAddr->ulMtuSize;
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           "%d",
                                           ulMtuSize);

        /* 填写Prefix */
        if (VOS_FALSE == pstAppRaInfoAddr->bitOpPrefixAddr)
        {
            ulPrefixBitLen = 0;
            AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                              aucInvalidIpv6Addr,
                                              TAF_IPV6_STR_RFC2373_TOKENS);
        }
        else
        {
            ulPrefixBitLen = pstAppRaInfoAddr->ulPrefixBitLen;
            AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                              pstAppRaInfoAddr->aucPrefixAddr,
                                              (VOS_UINT8)ulPrefixBitLen/16);
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           ",\"%s/%d\"",
                                           aucIpv6AddrStr,
                                           ulPrefixBitLen);

        /* 填写Preferred Lifetime */
        if (VOS_FALSE == pstAppRaInfoAddr->bitOpPreferredLifetime)
        {
            ulPreferredLifetime = 0;
        }
        else
        {
            ulPreferredLifetime = pstAppRaInfoAddr->ulPreferredLifetime;
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           ",%u",
                                           ulPreferredLifetime);

        if (VOS_FALSE == pstAppRaInfoAddr->bitOpValidLifetime)
        {
            ulValidLifetime = 0;
        }
        else
        {
            ulValidLifetime = pstAppRaInfoAddr->ulValidLifetime;
        }

        /* 填写Valid Lifetime */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           ",%u%s",
                                           ulValidLifetime,
                                           gaucAtCrLf);
    }

    if (usLength < (VOS_UINT16)VOS_StrLen((VOS_CHAR *)gaucAtCrLf))
    {
        AT_ERR_LOG("AT_QryApRaInfoPara:ERROR: PDP is not active for APP.");
        return AT_ERROR;
    }
    else
    {
        usLength -= (VOS_UINT16)VOS_StrLen((VOS_CHAR *)gaucAtCrLf);
    }
    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 AT_TestApRaInfoPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-11)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}



VOS_UINT32 AT_PS_ReportLanAddr(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucCallId;
    AT_PS_CALL_ENTITY_STRU             *pstPsCallEntity = VOS_NULL_PTR;
    AT_IPV6_RA_INFO_STRU               *pstIpv6RaInfo;
    VOS_UINT16                          usLength;
    VOS_UINT8                           aucIpv6AddrStr[TAF_MAX_IPV6_ADDR_COLON_STR_LEN];
    VOS_UINT8                           aucInvalidIpv6Addr[TAF_IPV6_ADDR_LEN];

    usLength = 0;
    TAF_MEM_SET_S(aucInvalidIpv6Addr, sizeof(aucInvalidIpv6Addr), 0x00, sizeof(aucInvalidIpv6Addr));

    /* 根据cid获取对应的callid */
    ucCallId = AT_PS_TransCidToCallId(ucIndex, (VOS_UINT8)gastAtParaList[0].ulParaValue);

    if (VOS_TRUE != AT_PS_IsCallIdValid(ucIndex, ucCallId))
    {
        AT_ERR_LOG("AT_PS_ReportLanAddr:ERROR: CallId is invalid!");
        return AT_ERROR;
    }

    pstPsCallEntity = AT_PS_GetCallEntity(ucIndex, ucCallId);

    /* 当前未激活，直接返回error */
    if (AT_PDP_STATE_ACTED != pstPsCallEntity->stIpv6Info.enIpv6State)
    {
        AT_WARN_LOG("AT_PS_ReportLanAddr: PDP is not actived in cellular.");
        return AT_ERROR;
    }

    pstIpv6RaInfo = &(pstPsCallEntity->stIpv6Info.stIpv6RaInfo);

    usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr,
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    if (VOS_FALSE == pstIpv6RaInfo->bitOpLanAddr)
    {
        AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                          aucInvalidIpv6Addr,
                                          TAF_IPV6_STR_RFC2373_TOKENS);
    }
    else
    {
        AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                          pstIpv6RaInfo->aucLanAddr,
                                          TAF_IPV6_STR_RFC2373_TOKENS);
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                       "\"%s\"",
                                       aucIpv6AddrStr);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                       ",%d",
                                       pstIpv6RaInfo->ulPrefixBitLen);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 AT_SetApLanAddrPara(VOS_UINT8 ucIndex)
{
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数错误 */
    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 查询指定CID的实体PDP上下文 */
    return AT_PS_ReportLanAddr(ucIndex);
}


VOS_UINT32 AT_QryApLanAddrPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16                          usLength;
    VOS_UINT8                           aucIpv6AddrStr[TAF_MAX_IPV6_ADDR_COLON_STR_LEN];
    VOS_UINT8                           aucInvalidIpv6Addr[TAF_IPV6_ADDR_LEN];
    AT_PS_CALL_ENTITY_STRU             *pstAppPdpEntity;
    AT_IPV6_RA_INFO_STRU               *pstAppRaInfoAddr;
    VOS_UINT32                          ulCallIndex;

    usLength                            = 0;
    TAF_MEM_SET_S(aucInvalidIpv6Addr, sizeof(aucInvalidIpv6Addr), 0x00, sizeof(aucInvalidIpv6Addr));

    if (AT_APP_USER == gastAtClientTab[ucIndex].UserType)
    {
        for (ulCallIndex = AT_PS_APP_CALL_ID_BEGIN; ulCallIndex <= AT_PS_APP_CALL_ID_END; ulCallIndex++)
        {
            pstAppPdpEntity = AT_PS_GetCallEntity(ucIndex, (VOS_UINT8)ulCallIndex);
            pstAppRaInfoAddr = &pstAppPdpEntity->stIpv6Info.stIpv6RaInfo;

            if (AT_PDP_STATE_ACTED != pstAppPdpEntity->stIpv6Info.enIpv6State)
            {
                continue;
            }

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%s: %d,",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                               pstAppPdpEntity->stUserInfo.ucUsrCid);

            if (VOS_FALSE == pstAppRaInfoAddr->bitOpLanAddr)
            {
                AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                                  aucInvalidIpv6Addr,
                                                  TAF_IPV6_STR_RFC2373_TOKENS);
            }
            else
            {
                AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                                  pstAppRaInfoAddr->aucLanAddr,
                                                  TAF_IPV6_STR_RFC2373_TOKENS);
            }

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "\"%s\"",
                                               aucIpv6AddrStr);

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR*)pgucAtSndCodeAddr, (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               ",%d%s",
                                               pstAppRaInfoAddr->ulPrefixBitLen,
                                               gaucAtCrLf);
        }
    }

    if (usLength < (VOS_UINT16)VOS_StrLen((VOS_CHAR *)gaucAtCrLf))
    {
        AT_ERR_LOG("AT_QryApLanAddrPara:ERROR: PDP is not active for APP.");
        return AT_ERROR;
    }
    else
    {
        usLength -= (VOS_UINT16)VOS_StrLen((VOS_CHAR *)gaucAtCrLf);
    }

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 AT_TestApLanAddrPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-11)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}


VOS_UINT32 AT_PS_ReportIPv6TempAddr(VOS_UINT8 ucIndex)
{
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity = VOS_NULL_PTR;
    VOS_UINT8                           aucIpv6AddrStr[TAF_MAX_IPV6_ADDR_COLON_STR_LEN];
    VOS_UINT16                          usLength = 0;
    VOS_UINT8                           ucCallId;

    TAF_MEM_SET_S(aucIpv6AddrStr, sizeof(aucIpv6AddrStr),
                  0x00, sizeof(aucIpv6AddrStr));

    if (IPV6_ADDRESS_TEST_MODE_ENABLE != AT_GetPsIPv6IIDTestModeConfig())
    {
        AT_ERR_LOG("AT_PS_ReportIPv6TempAddr: Test mode is disabled!");
        return AT_CME_OPERATION_NOT_ALLOWED;
    }

    ucCallId = AT_PS_TransCidToCallId(ucIndex, (VOS_UINT8)gastAtParaList[0].ulParaValue);
    if (VOS_TRUE != AT_PS_IsCallIdValid(ucIndex, ucCallId))
    {
        AT_ERR_LOG("AT_PS_ReportIPv6TempAddr: CallId is invalid!");
        return AT_ERROR;
    }

    pstCallEntity = AT_PS_GetCallEntity(ucIndex, ucCallId);
    if (AT_PDP_STATE_ACTED != pstCallEntity->stIpv6Info.enIpv6State)
    {
        AT_WARN_LOG("AT_PS_ReportIPv6TempAddr: PDP is not actived in cellular.");
        return AT_ERROR;
    }

    if (VOS_TRUE != pstCallEntity->stIpv6Info.stIpv6RaInfo.bitOpPrefixAddr)
    {
        AT_WARN_LOG("AT_PS_ReportIPv6TempAddr: Prefix address is not received.");
        return AT_ERROR;
    }

    if (!AT_PS_IS_IPV6_ADDR_IID_VALID(pstCallEntity->stIpv6Info.stIpv6DhcpInfo.aucIpv6TmpAddr))
    {
        AT_WARN_LOG("AT_PS_ReportIPv6TempAddr: IID is invalid.");
        return AT_ERROR;
    }

    usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    AT_ConvertIpv6AddrToCompressedStr(aucIpv6AddrStr,
                                      pstCallEntity->stIpv6Info.stIpv6DhcpInfo.aucIpv6TmpAddr,
                                      TAF_IPV6_STR_RFC2373_TOKENS);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "\"%s\"",
                                       aucIpv6AddrStr);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 AT_SetIPv6TempAddrPara(VOS_UINT8 ucIndex)
{
    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数错误 */
    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    return AT_PS_ReportIPv6TempAddr(ucIndex);
}


VOS_UINT32 AT_TestIPv6TempAddrPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: (1-11)",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}


VOS_UINT32 AT_SetCgpiafPara(VOS_UINT8 ucIndex)
{
    AT_COMM_PS_CTX_STRU                *pstCommPsCtx        = VOS_NULL_PTR;

    pstCommPsCtx    = AT_GetCommPsCtxAddr();

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (4 < gucAtParaIndex)
    {
        return AT_TOO_MANY_PARA;
    }

    /* 参数<IPv6_AddressFormat> */
    if (0 != gastAtParaList[0].usParaLen)
    {
        pstCommPsCtx->bitOpIpv6AddrFormat = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    }

    /* 参数<IPv6_SubnetNotation> */
    if (0 != gastAtParaList[1].usParaLen)
    {
        pstCommPsCtx->bitOpIpv6SubnetNotation = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    }

    /* 参数<IPv6_LeadingZeros> */
    if (0 != gastAtParaList[2].usParaLen)
    {
        pstCommPsCtx->bitOpIpv6LeadingZeros = (VOS_UINT8)gastAtParaList[2].ulParaValue;
    }

    /* 参数<IPv6_CompressZeros> */
    if (0 != gastAtParaList[2].usParaLen)
    {
        pstCommPsCtx->bitOpIpv6CompressZeros = (VOS_UINT8)gastAtParaList[3].ulParaValue;
    }

    return AT_OK;
}


VOS_UINT32 AT_QryCgpiafPara(VOS_UINT8 ucIndex)
{
    AT_COMM_PS_CTX_STRU                *pstCommPsCtx        = VOS_NULL_PTR;

    pstCommPsCtx    = AT_GetCommPsCtxAddr();

    /* 命令操作类型检查 */
    if (AT_CMD_OPT_READ_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 输出查询结果 */
    /* +CGPIAF: <IPv6_AddressFormat>,<IPv6_SubnetNotation>,<IPv6_LeadingZeros>,<IPv6_CompressZeros> */
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      "%s: %d,%d,%d,%d",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                      pstCommPsCtx->bitOpIpv6AddrFormat,
                                      pstCommPsCtx->bitOpIpv6SubnetNotation,
                                      pstCommPsCtx->bitOpIpv6LeadingZeros,
                                      pstCommPsCtx->bitOpIpv6CompressZeros);

    return AT_OK;
}


#endif


VOS_UINT32 AT_PS_ReportConnSt(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucCallId;
    AT_PS_CALL_ENTITY_STRU             *pstPsCallEntity = VOS_NULL_PTR;
    AT_APP_CONN_STATE_ENUM_U32          enIpv4ConnStatus;
#if (FEATURE_ON == FEATURE_IPV6)
    AT_APP_CONN_STATE_ENUM_U32          enIpv6ConnStatus;
#endif
    VOS_UINT16                          usLength;

    usLength = 0;

    /* 根据cid获取对应的callid */
    ucCallId = AT_PS_TransCidToCallId(ucIndex, (VOS_UINT8)gastAtParaList[0].ulParaValue);

    if (VOS_TRUE != AT_PS_IsCallIdValid(ucIndex, ucCallId))
    {
        AT_ERR_LOG("AT_PS_ReportConnSt:ERROR: CallId is invalid!");
        return AT_ERROR;
    }

    pstPsCallEntity = AT_PS_GetCallEntity(ucIndex, ucCallId);

#if (FEATURE_ON == FEATURE_IPV6)
    switch (AT_GetIpv6Capability())
    {
        case AT_IPV6_CAPABILITY_IPV4_ONLY:
            enIpv4ConnStatus = AT_GetAppConnStatus(pstPsCallEntity, TAF_PDP_IPV4);

            usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               "%s: ",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d,\"IPV4\"",
                                               enIpv4ConnStatus);
            break;

        case AT_IPV6_CAPABILITY_IPV6_ONLY:
            enIpv6ConnStatus = AT_GetAppConnStatus(pstPsCallEntity, TAF_PDP_IPV6);

            usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               "%s: ",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d,\"IPV6\"",
                                               enIpv6ConnStatus);
            break;

        case AT_IPV6_CAPABILITY_IPV4V6_OVER_ONE_PDP:
        case AT_IPV6_CAPABILITY_IPV4V6_OVER_TWO_PDP:
            enIpv4ConnStatus = AT_GetAppConnStatus(pstPsCallEntity, TAF_PDP_IPV4);
            enIpv6ConnStatus = AT_GetAppConnStatus(pstPsCallEntity, TAF_PDP_IPV6);

            usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               "%s: ",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d,\"IPV4\"",
                                               enIpv4ConnStatus);
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",%d,\"IPV6\"",
                                               enIpv6ConnStatus);
            break;

        default:
            break;
    }
#else
    enIpv4ConnStatus = AT_GetAppConnStatus(pstPsCallEntity, TAF_PDP_IPV4);

    usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d,\"IPV4\"",
                                       enIpv4ConnStatus);
#endif

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32  At_SetApConnStPara(
    VOS_UINT8                           ucIndex
)
{
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数错误 */
    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 查询指定CID的实体PDP上下文 */
    return AT_PS_ReportConnSt(ucIndex);
}


VOS_UINT32  At_QryApConnStPara(
    VOS_UINT8                           ucIndex
)
{
    AT_PS_CALL_ENTITY_STRU             *pstAppPdpEntity;
    AT_APP_CONN_STATE_ENUM_U32          enIpv4ConnStatus;
#if (FEATURE_ON == FEATURE_IPV6)
    AT_APP_CONN_STATE_ENUM_U32          enIpv6ConnStatus;
#endif
    VOS_UINT16                          usLength = 0;
    VOS_UINT32                          ulCallIndex;

    for (ulCallIndex = AT_PS_APP_CALL_ID_BEGIN; ulCallIndex <= AT_PS_APP_CALL_ID_END; ulCallIndex++)
    {
        pstAppPdpEntity = AT_PS_GetCallEntity(ucIndex, (VOS_UINT8)ulCallIndex);

        if ( (0 == pstAppPdpEntity->stUserInfo.ucUsrCid)
          || (pstAppPdpEntity->stUserInfo.ucUsrCid > TAF_MAX_CID_NV))
        {
            continue;
        }

#if (FEATURE_ON == FEATURE_IPV6)
        switch (AT_GetIpv6Capability())
        {
            case AT_IPV6_CAPABILITY_IPV4_ONLY:
                enIpv4ConnStatus = AT_AppConvertPdpStateToConnStatus(AT_PS_GetCallStateByType(pstAppPdpEntity, TAF_PDP_IPV4));

                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s: ",
                                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%d,%d,\"IPV4\"%s",
                                                   pstAppPdpEntity->stUserInfo.ucUsrCid,
                                                   enIpv4ConnStatus,
                                                   gaucAtCrLf);
                break;

            case AT_IPV6_CAPABILITY_IPV6_ONLY:
                enIpv6ConnStatus = AT_AppConvertPdpStateToConnStatus(AT_PS_GetCallStateByType(pstAppPdpEntity, TAF_PDP_IPV6));

                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s: ",
                                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%d,%d,\"IPV6\"%s",
                                                   pstAppPdpEntity->stUserInfo.ucUsrCid,
                                                   enIpv6ConnStatus,
                                                   gaucAtCrLf);
                break;

            case AT_IPV6_CAPABILITY_IPV4V6_OVER_ONE_PDP:
            case AT_IPV6_CAPABILITY_IPV4V6_OVER_TWO_PDP:
                enIpv4ConnStatus = AT_AppConvertPdpStateToConnStatus(AT_PS_GetCallStateByType(pstAppPdpEntity, TAF_PDP_IPV4));
                enIpv6ConnStatus = AT_AppConvertPdpStateToConnStatus(AT_PS_GetCallStateByType(pstAppPdpEntity, TAF_PDP_IPV6));

                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s: ",
                                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%d,%d,\"IPV4\"",
                                                   pstAppPdpEntity->stUserInfo.ucUsrCid,
                                                   enIpv4ConnStatus);
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   ",%d,\"IPV6\"%s",
                                                   enIpv6ConnStatus,
                                                   gaucAtCrLf);
                break;

            default:
                break;
        }
#else
        enIpv4ConnStatus = AT_AppConvertPdpStateToConnStatus(AT_PS_GetCallStateByType(pstAppPdpEntity, TAF_PDP_IPV4));

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: ",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%d,%d,\"IPV4\"%s",
                                           pstAppPdpEntity->stUserInfo.ucUsrCid,
                                           enIpv4ConnStatus,
                                           gaucAtCrLf);
#endif
    }

    if (usLength >= (VOS_UINT16)VOS_StrLen((VOS_CHAR *)gaucAtCrLf))
    {
        usLength -= (VOS_UINT16)VOS_StrLen((VOS_CHAR *)gaucAtCrLf);
    }

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


VOS_UINT32 AT_TestApConnStPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-11)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}


VOS_UINT32 AT_SetApDsFlowRptCfgPara(VOS_UINT8 ucIndex)
{
    TAF_APDSFLOW_RPT_CFG_STRU           stRptCfg;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 命令参数个数检查 */
    if ((gucAtParaIndex < 1) || (gucAtParaIndex > 4))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT^APDSFLOWRPTCFG=, */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置<enable> */
    stRptCfg.ulRptEnabled = gastAtParaList[0].ulParaValue;

    if (VOS_TRUE == stRptCfg.ulRptEnabled)
    {
        /* AT^APDSFLOWRPTCFG=1 */
        if (1 == gucAtParaIndex)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* AT^APDSFLOWRPTCFG=1, */
        if (0 == gastAtParaList[1].usParaLen)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        /* AT^APDSFLOWRPTCFG=1,<threshold> */
        stRptCfg.ulFluxThreshold = gastAtParaList[1].ulParaValue;

        /* AT^APDSFLOWRPTCFG=1,0 */
        if (0 == stRptCfg.ulFluxThreshold)
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    else
    {
        /* AT^APDSFLOWRPTCFG=0 */
        stRptCfg.ulFluxThreshold = 0;
    }

    /* 执行命令操作 */
    if (VOS_OK != TAF_PS_SetApDsFlowRptCfg(WUEPS_PID_AT,
                                           AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                           0,
                                           &stRptCfg))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_APDSFLOWRPTCFG_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_QryApDsFlowRptCfgPara(VOS_UINT8 ucIndex)
{
    /* 执行命令操作 */
    if (VOS_OK != TAF_PS_GetApDsFlowRptCfg(WUEPS_PID_AT,
                                           AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                           0))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_APDSFLOWRPTCFG_QRY;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_SetDsFlowNvWriteCfgPara(VOS_UINT8 ucIndex)
{
    TAF_DSFLOW_NV_WRITE_CFG_STRU        stWriteNvCfg;

    TAF_MEM_SET_S(&stWriteNvCfg, sizeof(stWriteNvCfg), 0x00, sizeof(TAF_DSFLOW_NV_WRITE_CFG_STRU));

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 命令参数个数检查 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT^DSFLOWNVWRCFG= */
    if (0 == gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* AT^DSFLOWNVWRCFG=,<interval> */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 获取<enable> */
    stWriteNvCfg.ucEnabled          = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 获取<interval> */
    if (gucAtParaIndex > 1)
    {
        if (0 != gastAtParaList[1].usParaLen)
        {
            /* AT^DSFLOWNVWRCFG=<enable>,<interval> */
            stWriteNvCfg.ucInterval = (VOS_UINT8)gastAtParaList[1].ulParaValue;
        }
        else
        {
            /* AT^DSFLOWNVWRCFG=<enable>, */
            return AT_CME_INCORRECT_PARAMETERS;
        }
    }
    else
    {
        /* AT^DSFLOWNVWRCFG=<enable> */
        stWriteNvCfg.ucInterval     = TAF_DEFAULT_DSFLOW_NV_WR_INTERVAL;
    }

    /* AT^DSFLOWNVWRCFG=1,0 */
    if ((VOS_TRUE == stWriteNvCfg.ucEnabled) && (0 == stWriteNvCfg.ucInterval))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 执行命令操作 */
    if (VOS_OK != TAF_PS_SetDsFlowNvWriteCfg(WUEPS_PID_AT,
                                             AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                             0,
                                             &stWriteNvCfg))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DSFLOWNVWRCFG_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_QryDsFlowNvWriteCfgPara(VOS_UINT8 ucIndex)
{
    /* 执行命令操作 */
    if (VOS_OK != TAF_PS_GetDsFlowNvWriteCfg(WUEPS_PID_AT,
                                             AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                             0))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DSFLOWNVWRCFG_QRY;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}

#if (FEATURE_ON == FEATURE_IMS)

VOS_UINT32 AT_SetImsPdpCfg(VOS_UINT8 ucIndex)
{
    TAF_IMS_PDP_CFG_STRU                stImsPdpCfg;

    /* 参数过多 */
    if (gucAtParaIndex != 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数检查 */
    if ((0 == gastAtParaList[0].usParaLen)
     || (0 == gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    TAF_MEM_SET_S(&stImsPdpCfg, sizeof(stImsPdpCfg), 0x00, sizeof(stImsPdpCfg));

    /* 参数赋值 */
    stImsPdpCfg.ucCid           = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    stImsPdpCfg.ucImsFlag       = (VOS_UINT8)gastAtParaList[1].ulParaValue;

    /* 发送跨核消息 */
    if ( VOS_OK != TAF_PS_SetImsPdpCfg(WUEPS_PID_AT,
                                       AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                       0,
                                       &stImsPdpCfg) )
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_IMSPDPCFG_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}
#endif
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 AT_SetMipPara(VOS_UINT8 ucIndex)
{

    /* 参数错误 */
    if (gucAtParaIndex != 1)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数检查 */
    if (0 == gastAtParaList[0].usParaLen)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 发送跨核消息 */
    if ( VOS_OK != TAF_PS_SetMipMode(WUEPS_PID_AT,
                                     AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                     0,
                                     (VOS_UINT8)gastAtParaList[0].ulParaValue) )
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMIP_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_QryMipPara(VOS_UINT8 ucIndex)
{

    /* 发送跨核消息 */
    if ( VOS_OK != TAF_PS_GetMipMode(WUEPS_PID_AT,
                                     AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                     0) )
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CMIP_QRY;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}

#endif

VOS_UINT32 AT_SetVzwApnePara(VOS_UINT8 ucIndex)
{
    TAF_PS_VZWAPNE_INFO_STRU            stVzwapneInfo;
    VOS_UINT8                           ucIsCustomCmd;

    TAF_MEM_SET_S(&stVzwapneInfo, sizeof(stVzwapneInfo), 0, sizeof(TAF_PS_VZWAPNE_INFO_STRU));
    ucIsCustomCmd = VOS_FALSE;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* +VZWAPNE命令标记 */
    if (AT_CMD_VZWAPNE == g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex)
    {
        ucIsCustomCmd = VOS_TRUE;
    }

    /* 设置<wapn>/<cid> */
    stVzwapneInfo.ucCid     = (VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 设置<apncl> */
    stVzwapneInfo.ucClass   = (VOS_UINT8)gastAtParaList[1].ulParaValue;

    /* 设置<apnni> */
    if (gastAtParaList[2].usParaLen > TAF_MAX_APN_LEN)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (0 != gastAtParaList[2].usParaLen)
    {
        /* 检查 */
        if ( VOS_OK != AT_CheckApnFormat(gastAtParaList[2].aucPara,
                                         gastAtParaList[2].usParaLen) )
        {
            return AT_CME_INCORRECT_PARAMETERS;
        }

        stVzwapneInfo.stApn.ucLength = (VOS_UINT8)gastAtParaList[2].usParaLen;
        TAF_MEM_CPY_S(stVzwapneInfo.stApn.aucValue,
                      sizeof(stVzwapneInfo.stApn.aucValue),
                      gastAtParaList[2].aucPara,
                      gastAtParaList[2].usParaLen);
    }

    /* 设置<apntype> */
    stVzwapneInfo.enIpType  = (VOS_UINT8)gastAtParaList[3].ulParaValue + 1;

    /* 设置<apned> */
    stVzwapneInfo.ucEnable  = (VOS_UINT8)gastAtParaList[5].ulParaValue;

    /* 设置<apntime> */
    stVzwapneInfo.usInactiveTimer  = (VOS_UINT8)gastAtParaList[6].ulParaValue;

    /* 发送跨核消息 */
    if (VOS_OK != TAF_PS_SetVzwApneInfo(WUEPS_PID_AT,
                                        AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                        gastAtClientTab[ucIndex].opId,
                                        ucIsCustomCmd,
                                        &stVzwapneInfo))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_VZWAPNE_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_QryVzwApnePara(VOS_UINT8 ucIndex)
{
    VOS_UINT8                           ucIsCustomCmd;

    ucIsCustomCmd = VOS_FALSE;

    /* 参数检查 */
    if (AT_CMD_OPT_READ_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* +VZWAPNE命令标记 */
    if (AT_CMD_VZWAPNE == g_stParseContext[ucIndex].pstCmdElement->ulCmdIndex)
    {
        ucIsCustomCmd = VOS_TRUE;
    }

    /* 发送跨核消息 */
    if (VOS_OK != TAF_PS_GetVzwApneInfo(WUEPS_PID_AT,
                                        AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                        gastAtClientTab[ucIndex].opId,
                                        ucIsCustomCmd))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_VZWAPNE_QRY;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_TestVzwApnePara(VOS_UINT8 ucIndex)
{
    /* 参数检查 */
    if (AT_CMD_OPT_TEST_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
        (VOS_CHAR *)pgucAtSndCodeAddr,
        (VOS_CHAR *)pgucAtSndCodeAddr,
        "%s: (1-8),(1-8),(\"VZWIMS\",\"VZWADMIN\",\"VZWINTERNET\",\"VZWAPP\"),(\"IPv4\",\"IPv6\",\"IPv4v6\"),(\"LTE\"),(\"Enabled\",\"Disabled\"),(0-255)",
        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}


VOS_UINT32 AT_SetCgerepPara(VOS_UINT8 ucIndex)
{
    AT_MODEM_NET_CTX_STRU                  *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (gucAtParaIndex > 2)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* +CGEREP=[<mode>[,<bfr>]] */
    if (0 == gucAtParaIndex)
    {
        pstNetCtx->stCgerepCfg.ucMode   = 0;
        pstNetCtx->stCgerepCfg.ucBfr    = 0;
    }
    else if (1 == gucAtParaIndex)
    {
        if (0 == gastAtParaList[0].usParaLen)
        {
             return AT_CME_INCORRECT_PARAMETERS;
        }

        pstNetCtx->stCgerepCfg.ucMode   = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        pstNetCtx->stCgerepCfg.ucBfr    = 0;
    }
    else
    {
        if ( (0 == gastAtParaList[0].usParaLen)
          || (0 == gastAtParaList[1].usParaLen))
        {
             return AT_CME_INCORRECT_PARAMETERS;
        }

        pstNetCtx->stCgerepCfg.ucMode   = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        pstNetCtx->stCgerepCfg.ucBfr    = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    }

    return AT_OK;
}


VOS_UINT32 AT_QryCgerepPara(VOS_UINT8 ucIndex)
{
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if (AT_CMD_OPT_READ_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: %d,%d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    pstNetCtx->stCgerepCfg.ucMode,
                                                    pstNetCtx->stCgerepCfg.ucBfr);


    return AT_OK;
}


VOS_UINT32 AT_SetCindPara(VOS_UINT8 ucIndex)
{
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;
    TAF_START_INFO_IND_STRU             stStartInfoInd;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);
    TAF_MEM_SET_S(&stStartInfoInd, sizeof(stStartInfoInd), 0x00, sizeof(stStartInfoInd));

    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数过多 */
    if (1 < gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (0 == gucAtParaIndex)
    {
        return AT_OK;
    }

    /* +CIND=[<ind>] 目前只支持signal的设置 */
    if (0 != gastAtParaList[0].usParaLen)
    {
        pstNetCtx->ucCerssiReportType = (AT_CERSSI_REPORT_TYPE_ENUM_UINT8)gastAtParaList[0].ulParaValue;

        stStartInfoInd.ucActionType             = TAF_START_EVENT_INFO_FOREVER;
        stStartInfoInd.ucSignThreshold          = (VOS_UINT8)gastAtParaList[0].ulParaValue;
        stStartInfoInd.ucRrcMsgType             = TAF_EVENT_INFO_CELL_SIGN;
        stStartInfoInd.ucMinRptTimerInterval    = pstNetCtx->ucCerssiMinTimerInterval;

        if (VOS_TRUE == TAF_MMA_SetCindReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stStartInfoInd))
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CIND_SET;
            return AT_WAIT_ASYNC_RETURN;
        }
        else
        {
            return AT_ERROR;
        }
    }

    return AT_OK;
}


VOS_UINT32 AT_QryCindPara(VOS_UINT8 ucIndex)
{
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    /* 参数检查 */
    if (AT_CMD_OPT_READ_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    /* +CIND: <ind> 目前只支持signal的查询 */
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: %d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    pstNetCtx->ucCerssiReportType);


    return AT_OK;
}


VOS_UINT32 AT_TestCindPara(VOS_UINT8 ucIndex)
{
    /* 参数检查 */
    if (AT_CMD_OPT_TEST_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
        (VOS_CHAR *)pgucAtSndCodeAddr,
        (VOS_CHAR *)pgucAtSndCodeAddr,
        "%s: \"signal\",(0-5)",
        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}


VOS_UINT32 AT_SetDataSwitchStatus( VOS_UINT8 ucIndex )
{
    VOS_UINT8                           ucDataSwitchAT;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        AT_WARN_LOG("AT_SetDataSwitchStatus: NOT AT_CMD_OPT_SET_PARA_CMD!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数不正确 */
    if (1 != gucAtParaIndex)
    {
        AT_WARN_LOG("AT_SetDataSwitchStatus: para num is not equal 1!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (1 != gastAtParaList[0].usParaLen)
    {
        AT_WARN_LOG("AT_SetDataSwitchStatus: usParaLen  is not equal 1!");
        return AT_CME_INCORRECT_PARAMETERS;

    }

    ucDataSwitchAT=(VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 给TAF APS发送通知消息 */
    if (VOS_OK != TAF_PS_Set_DataSwitchStatus(WUEPS_PID_AT,
                                              AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                              0,
                                              ucDataSwitchAT))
    {
        AT_WARN_LOG("AT_SetDataSwitchStatus():TAF_PS_Set_DataSwitchStatus fail");
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DATASWITCH_SET;
    return AT_WAIT_ASYNC_RETURN;
}

VOS_UINT32 AT_SetDataRoamSwitchStatus( VOS_UINT8 ucIndex )
{
    VOS_UINT8                           ucDataRoamSwitchAT;

    /* 参数检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        AT_WARN_LOG("AT_SetDataRoamSwitchStatus: NOT AT_CMD_OPT_SET_PARA_CMD!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数个数不正确 */
    if (1 != gucAtParaIndex)
    {
        AT_WARN_LOG("AT_SetDataRoamSwitchStatus: para num is not equal 1!");
        return AT_CME_INCORRECT_PARAMETERS;
    }

    if (1 != gastAtParaList[0].usParaLen)
    {
        AT_WARN_LOG("AT_SetDataRoamSwitchStatus: usParaLen  is not equal 1!");
        return AT_CME_INCORRECT_PARAMETERS;

    }

    ucDataRoamSwitchAT=(VOS_UINT8)gastAtParaList[0].ulParaValue;

    /* 给TAF APS发送通知消息 */
    if (VOS_OK != TAF_PS_Set_DataRoamSwitchStatus(WUEPS_PID_AT,
                                                  AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                                  0,
                                                  ucDataRoamSwitchAT) )
    {
        AT_WARN_LOG("AT_SetDataRoamSwitchStatus():TAF_PS_Set_DataRoamSwitchStatus fail");
        return AT_ERROR;
    }


    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DATAROAMSWITCH_SET;

    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_QryDataSwitchStatus(  VOS_UINT8 ucIndex )
{
    /* 发送跨核消息 */
    if ( VOS_OK != TAF_PS_Get_DataSwitchStatus(WUEPS_PID_AT,
                                               AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                               0))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DATASWITCH_QRY;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}



VOS_UINT32 AT_QryDataRoamSwitchStatus(  VOS_UINT8 ucIndex )
{
    /* 发送跨核消息 */
    if ( VOS_OK != TAF_PS_Get_DataRoamSwitchStatus(WUEPS_PID_AT,
                                                   AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                                   0))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_DATAROAMSWITCH_QRY;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 AT_SetApnThrotInfoPara(VOS_UINT8 ucIndex)
{
    TAF_PS_APN_THROT_INFO_STRU                stApnThrotInfo;

    TAF_MEM_SET_S(&stApnThrotInfo, sizeof(TAF_PS_APN_THROT_INFO_STRU), 0, sizeof(TAF_PS_APN_THROT_INFO_STRU));

    /* 指令类型检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数数目错误 */
    if (5 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if ( (0 == gastAtParaList[0].usParaLen)
      || (0 == gastAtParaList[1].usParaLen)
      || (0 == gastAtParaList[2].usParaLen)
      || (0 == gastAtParaList[3].usParaLen)
      || (0 == gastAtParaList[4].usParaLen) )
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* CID与ApnClassType 必须一一对应 */
    if (gastAtParaList[0].ulParaValue != gastAtParaList[1].ulParaValue)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 设置参数 */
    stApnThrotInfo.ucCid               = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    stApnThrotInfo.ucApnClassType      = (VOS_UINT8)gastAtParaList[1].ulParaValue;
    stApnThrotInfo.ulWaitTime          = gastAtParaList[2].ulParaValue;
    stApnThrotInfo.ulPdnMaxConnTime    = gastAtParaList[3].ulParaValue;
    stApnThrotInfo.ulPdnMaxConnCount   = gastAtParaList[4].ulParaValue;

    /* 发送跨核消息 */
    if (VOS_OK != TAF_PS_SetApnThrotInfo(WUEPS_PID_AT,
                                         AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                         gastAtClientTab[ucIndex].opId,
                                         &stApnThrotInfo))
    {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_APN_THROT_INFO_SET;

    /* 返回命令处理挂起状态 */
    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 At_SetUsbTetherInfo(VOS_UINT8 ucIndex)
{
    AT_RNIC_USB_TETHER_INFO_IND_STRU   *pstMsg;

    /* 指令类型检查 */
    if (AT_CMD_OPT_SET_PARA_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数数目错误 */
    if (2 != gucAtParaIndex)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    /* 参数为空 */
    if ((0 == gastAtParaList[0].usParaLen)
     || (0 == gastAtParaList[1].usParaLen)
     || (RNIC_RMNET_NAME_MAX_LEN < gastAtParaList[1].usParaLen))
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    AT_WARN_LOG1("At_SetUsbTetherInfo: para2 len is ",gastAtParaList[1].usParaLen);

    pstMsg = (AT_RNIC_USB_TETHER_INFO_IND_STRU *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_RNIC_USB_TETHER_INFO_IND_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SetUsbTetherInfo: alloc msg fail!");
        return AT_ERROR;
    }

    /* 初始化消息体 */
    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                  (VOS_SIZE_T)sizeof(AT_RNIC_USB_TETHER_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  (VOS_SIZE_T)sizeof(AT_RNIC_USB_TETHER_INFO_IND_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 填写消息头和消息ID */
    AT_CFG_MSG_HDR( pstMsg, ACPU_PID_RNIC, ID_AT_RNIC_USB_TETHER_INFO_IND );

    /* 填写消息体 */
    pstMsg->enTetherConnStat = (VOS_UINT8)gastAtParaList[0].ulParaValue;
    TAF_MEM_CPY_S(pstMsg->aucRmnetName, RNIC_RMNET_NAME_MAX_LEN, gastAtParaList[1].aucPara, gastAtParaList[1].usParaLen);

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SetUsbTetherInfo: Send msg fail!");
        return AT_ERROR;
    }

    return AT_OK;
}


VOS_UINT32 At_TestUsbTetherInfo(VOS_UINT8 ucIndex)
{
    /* 参数检查 */
    if (AT_CMD_OPT_TEST_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
        (VOS_CHAR *)pgucAtSndCodeAddr,
        (VOS_CHAR *)pgucAtSndCodeAddr,
        "%s: (\"0:disconn\",\"1:connect\"),(\"rmnet0 - rmnet6\")",
        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}


VOS_UINT32 At_QryDconnStatPara(VOS_UINT8 ucIndex)
{
    AT_PS_CALL_ENTITY_STRU             *pstPsCallEntity;
    AT_PDP_STATUS_ENUM_UINT32           enIpv4Status;
    AT_PDP_STATUS_ENUM_UINT32           enIpv6Status;
    AT_DIALTYPE_ENUM_UINT32             enDialType;
    VOS_UINT32                          ulCid;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucCallId;
    VOS_UINT8                           ucCrLf;
    VOS_UINT8                           aucStr[TAF_MAX_APN_LEN + 1];

    usLength = 0;
    ucCrLf   = 0;

    for (ulCid = 1; ulCid <= TAF_MAX_CID_NV; ulCid++)
    {
        if (0 != ucCrLf)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR*)pgucAtSndCodeAddr,
                                               (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%s", gaucAtCrLf);
        }
        /* ^DCONNSTAT:  */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: ",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        /* CID */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR*)pgucAtSndCodeAddr,
                                           (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           "%d",
                                           ulCid);

        ucCrLf++;

        ucCallId = AT_PS_TransCidToCallId(ucIndex,(VOS_UINT8)ulCid);

        if (VOS_FALSE == AT_PS_IsCallIdValid(ucIndex, ucCallId))
        {
            continue;
        }

        pstPsCallEntity = AT_PS_GetCallEntity(ucIndex, ucCallId);

        enIpv4Status    = AT_NdisGetConnStatus(pstPsCallEntity->stIpv4Info.enIpv4State);
        enIpv6Status    = AT_NdisGetConnStatus(pstPsCallEntity->stIpv6Info.enIpv6State);

        if ((AT_PDP_STATUS_DEACT == enIpv4Status) && (AT_PDP_STATUS_DEACT == enIpv6Status))
        {
            continue;
        }

        if (AT_PS_WAN_TYPE_NDIS == pstPsCallEntity->enPsCallType)
        {
            enDialType = AT_DIALTYPE_NDIS;
        }
        else if (AT_PS_WAN_TYPE_APP == pstPsCallEntity->enPsCallType)
        {
            enDialType = AT_DIALTYPE_APP;
        }
        else
        {
            enDialType = AT_DIALTYPE_BUTT;
        }

        /* APN */
        TAF_MEM_SET_S(aucStr, sizeof(aucStr), 0x00, sizeof(aucStr));
        TAF_MEM_CPY_S(aucStr, sizeof(aucStr),
                      pstPsCallEntity->stUsrDialParam.aucAPN,
                      TAF_MIN(pstPsCallEntity->stUsrDialParam.ucAPNLen, TAF_MAX_APN_LEN));

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",\"%s\"",
                                           aucStr);
        /* IPV4 IPV6 STATUS*/
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR*)pgucAtSndCodeAddr,
                                           (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           ",%d,%d,%d",
                                           enIpv4Status, enIpv6Status, enDialType);

    }

    gstAtSendData.usBufLen  = usLength;

    return AT_OK;
}


VOS_UINT32 AT_TestDconnStatPara(VOS_UINT8 ucIndex)
{
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity;
    AT_PDP_STATUS_ENUM_UINT32           enIpv4Status;
    AT_PDP_STATUS_ENUM_UINT32           enIpv6Status;
    VOS_UINT32                          ulCid;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucCallId;
    VOS_UINT8                           ucHasDialedFlg;

    usLength        = 0;
    ucHasDialedFlg  = VOS_FALSE;

    /* 参数检查 */
    if (AT_CMD_OPT_TEST_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_CME_INCORRECT_PARAMETERS;
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: %s",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName, "(");

    for (ulCid = 1; ulCid <= TAF_MAX_CID_NV; ulCid++)
    {
        ucCallId = AT_PS_TransCidToCallId(ucIndex, (VOS_UINT8)ulCid);

        if (VOS_FALSE == AT_PS_IsCallIdValid(ucIndex, ucCallId))
        {
            continue;
        }

        pstCallEntity = AT_PS_GetCallEntity(ucIndex, ucCallId);

        enIpv4Status = AT_NdisGetConnStatus(pstCallEntity->stIpv4Info.enIpv4State);
        enIpv6Status = AT_NdisGetConnStatus(pstCallEntity->stIpv6Info.enIpv6State);

        if ( (AT_PDP_STATUS_ACT == enIpv4Status)
          || (AT_PDP_STATUS_ACT == enIpv6Status))
        {
            ucHasDialedFlg = VOS_TRUE;

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d,",ulCid);
        }
    }

    if (VOS_TRUE == ucHasDialedFlg)
    {
        /* 删除最后一个逗号 */
        usLength--;

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s", ")");
    }
    else
    {
        /* 若无连接态CID,直接返回OK */
        usLength = 0;
    }

    gstAtSendData.usBufLen  = usLength;

    return AT_OK;

}


