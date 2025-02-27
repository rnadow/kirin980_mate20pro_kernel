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
#include "ATCmdProc.h"
#include "siapppb.h"
#include "siappstk.h"
#include "AtMsgPrint.h"
#include "mdrv.h"
#include "AtPhyInterface.h"
#include "PppInterface.h"
#include "AtDataProc.h"
#include "AtCmdMsgProc.h"
#include "TafDrvAgent.h"
#include "AtCmdMsgProc.h"
#include "AtEventReport.h"
#include "AtRabmInterface.h"
#include "AtRnicInterface.h"
#include "AtDeviceCmd.h"
#include "AtInit.h"
#include "at_common.h"
#include "AcpuReset.h"

#include "AtInternalMsg.h"

#if (FEATURE_ON==FEATURE_LTE)
#include "msp_nvim.h"
#include "at_lte_common.h"
#endif

#include "nv_stru_sys.h"
#include "nv_stru_was.h"
#include "nv_stru_gas.h"

#include "OmApi.h"
#include "AppVcApi.h"

#include "AtCmdImsProc.h"

#include "product_config.h"

#include "AtCmdCallProc.h"
#include "AtCmdFtmProc.h"
#include "AtCmdMiscProc.h"
#include "AtCmdCagpsProc.h"
#include "AtCmdCssProc.h"

#include "mdrv.h"
#include "TafAppCall.h"
#include "AtEventReport.h"
#include "AtCmdSupsProc.h"
#include "AtCtx.h"
#include "AtCmdAgpsProc.h"
#include "TafCcmApi.h"

#include "AtMtMsgProc.h"



/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_AT_CMDMSGPROC_C


/*****************************************************************************
  2 类型定义
*****************************************************************************/

/*****************************************************************************
   3 全局变量定义
*****************************************************************************/
#if ((FEATURE_ON == FEATURE_UE_MODE_CDMA)&&(FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE)&&(FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT))

EXTERN VOS_UINT8    gucCurrEncVoiceDataWriteFileNum;
EXTERN VOS_UINT32   gulAtCurrEncVoiceDataCount;
EXTERN TAF_CHAR    *g_pacCurrEncVoiceDataWriteFilePath[];
EXTERN VOS_UINT8    gucAtCurrEncVoiceTestFileNum;
EXTERN TAF_CHAR     gacAtCurrDocName[AT_TEST_ECC_FILE_NAME_MAX_LEN];
#endif


/*AT 模块处理来自AT AGENT消息函数对应表*/
const AT_PROC_MSG_FROM_DRV_AGENT_STRU g_astAtProcMsgFromDrvAgentTab[]=
{
    /* 消息ID */                            /* 消息处理函数 */
    {DRV_AGENT_MSID_QRY_CNF,                AT_RcvDrvAgentMsidQryCnf},

    {DRV_AGENT_HARDWARE_QRY_RSP,            AT_RcvDrvAgentHardwareQryRsp},
    {DRV_AGENT_FULL_HARDWARE_QRY_RSP,       AT_RcvDrvAgentFullHardwareQryRsp},
    {DRV_AGENT_RXDIV_QRY_CNF,               AT_RcvDrvAgentQryRxdivCnf},
    {DRV_AGENT_SIMLOCK_SET_CNF,             AT_RcvDrvAgentSetSimlockCnf},


    {DRV_AGENT_VERTIME_QRY_CNF,             AT_RcvDrvAgentVertimeQryRsp},
    {DRV_AGENT_YJCX_SET_CNF,                AT_RcvDrvAgentYjcxSetCnf},
    {DRV_AGENT_YJCX_QRY_CNF,                AT_RcvDrvAgentYjcxQryCnf},
    {DRV_AGENT_GPIOPL_SET_CNF,              AT_RcvDrvAgentSetGpioplRsp},
    {DRV_AGENT_GPIOPL_QRY_CNF,              AT_RcvDrvAgentQryGpioplRsp},
    {DRV_AGENT_DATALOCK_SET_CNF,            AT_RcvDrvAgentSetDatalockRsp},
    {DRV_AGENT_TBATVOLT_QRY_CNF,            AT_RcvDrvAgentQryTbatvoltRsp},
    {DRV_AGENT_VERSION_QRY_CNF,             AT_RcvDrvAgentQryVersionRsp},
#if(FEATURE_OFF == FEATURE_UE_MODE_NR)
    {DRV_AGENT_FCHAN_SET_CNF,               AT_RcvDrvAgentSetFchanRsp},
#endif    
    {DRV_AGENT_SFEATURE_QRY_CNF,            AT_RcvDrvAgentQrySfeatureRsp},
    {DRV_AGENT_PRODTYPE_QRY_CNF,            AT_RcvDrvAgentQryProdtypeRsp},

    {DRV_AGENT_CPULOAD_QRY_CNF,             AT_RcvDrvAgentCpuloadQryRsp},
    {DRV_AGENT_MFREELOCKSIZE_QRY_CNF,       AT_RcvDrvAgentMfreelocksizeQryRsp},
    {DRV_AGENT_MEMINFO_QRY_CNF,             AT_RcvDrvAgentMemInfoQryRsp},
    {DRV_AGENT_DLOADINFO_QRY_CNF,           AT_RcvDrvAgentDloadInfoQryRsp},
    {DRV_AGENT_FLASHINFO_QRY_CNF,           AT_RcvDrvAgentFlashInfoQryRsp},
    {DRV_AGENT_AUTHORITYVER_QRY_CNF,        AT_RcvDrvAgentAuthorityVerQryRsp},
    {DRV_AGENT_AUTHORITYID_QRY_CNF,         AT_RcvDrvAgentAuthorityIdQryRsp},
    {DRV_AGENT_AUTHVER_QRY_CNF,             AT_RcvDrvAgentAuthVerQryRsp},
#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)
    {DRV_AGENT_GODLOAD_SET_CNF,             AT_RcvDrvAgentGodloadSetRsp},
#endif
    {DRV_AGENT_PFVER_QRY_CNF,               AT_RcvDrvAgentPfverQryRsp},
    {DRV_AGENT_HWNATQRY_QRY_CNF,            AT_RcvDrvAgentHwnatQryRsp},
    {DRV_AGENT_SDLOAD_SET_CNF,              AT_RcvDrvAgentSdloadSetRsp},
    {DRV_AGENT_APPDMVER_QRY_CNF,            AT_RcvDrvAgentAppdmverQryRsp},
    {DRV_AGENT_DLOADVER_QRY_CNF,            AT_RcvDrvAgentDloadverQryRsp},

    {DRV_AGENT_IMSICHG_QRY_CNF,             AT_RcvDrvAgentImsiChgQryRsp},
    {DRV_AGENT_INFORBU_SET_CNF,             AT_RcvDrvAgentInfoRbuSetRsp},
#if ( FEATURE_ON == FEATURE_LTE )
    {DRV_AGENT_INFORRS_SET_CNF,             AT_RcvDrvAgentInfoRrsSetRsp},
#endif
    {DRV_AGENT_CPNN_QRY_CNF,                AT_RcvDrvAgentCpnnQryRsp},
    {DRV_AGENT_CPNN_TEST_CNF,               AT_RcvDrvAgentCpnnTestRsp},
    {DRV_AGENT_NVBACKUP_SET_CNF,            AT_RcvDrvAgentNvBackupSetRsp},

    {DRV_AGENT_NVRESTORE_SET_CNF,           AT_RcvDrvAgentSetNvRestoreCnf},
    {DRV_AGENT_NVRSTSTTS_QRY_CNF,           AT_RcvDrvAgentQryNvRestoreRstCnf},
    {DRV_AGENT_NVRESTORE_MANU_DEFAULT_CNF,  AT_RcvDrvAgentNvRestoreManuDefaultRsp},

    {DRV_AGENT_ADC_SET_CNF,                 AT_RcvDrvAgentSetAdcRsp},
#if(FEATURE_OFF == FEATURE_UE_MODE_NR)
    {DRV_AGENT_TSELRF_SET_CNF,              AT_RcvDrvAgentTseLrfSetRsp},
#endif
    {DRV_AGENT_HKADC_GET_CNF,               AT_RcvDrvAgentHkAdcGetRsp},

    {DRV_AGENT_TBAT_QRY_CNF,                AT_RcvDrvAgentQryTbatRsp},
#if (FEATURE_ON == FEATURE_SECURITY_SHELL)
    {DRV_AGENT_SPWORD_SET_CNF,              AT_RcvDrvAgentSetSpwordRsp},
#endif

    {DRV_AGENT_NVBACKUPSTAT_QRY_CNF,        AT_RcvDrvAgentNvBackupStatQryRsp},
    {DRV_AGENT_NANDBBC_QRY_CNF,             AT_RcvDrvAgentNandBadBlockQryRsp},
    {DRV_AGENT_NANDVER_QRY_CNF,             AT_RcvDrvAgentNandDevInfoQryRsp},
    {DRV_AGENT_CHIPTEMP_QRY_CNF,            AT_RcvDrvAgentChipTempQryRsp},


    {DRV_AGENT_MAX_LOCK_TIMES_SET_CNF,      AT_RcvDrvAgentSetMaxLockTmsRsp},

    {DRV_AGENT_AP_SIMST_SET_CNF,            AT_RcvDrvAgentSetApSimstRsp},

    {DRV_AGENT_HUK_SET_CNF,                 AT_RcvDrvAgentHukSetCnf},
    {DRV_AGENT_FACAUTHPUBKEY_SET_CNF,       AT_RcvDrvAgentFacAuthPubkeySetCnf},
    {DRV_AGENT_IDENTIFYSTART_SET_CNF,       AT_RcvDrvAgentIdentifyStartSetCnf},
    {DRV_AGENT_IDENTIFYEND_SET_CNF,         AT_RcvDrvAgentIdentifyEndSetCnf},
    {DRV_AGENT_SIMLOCKDATAWRITE_SET_CNF,    AT_RcvDrvAgentSimlockDataWriteSetCnf},
    {DRV_AGENT_PHONESIMLOCKINFO_QRY_CNF,    AT_RcvDrvAgentPhoneSimlockInfoQryCnf},
    {DRV_AGENT_PHONESIMLOCKINFO_QRY_NEW_CNF, AT_RcvDrvAgentPhoneSimlockInfoQryCnf},
    {DRV_AGENT_SIMLOCKDATAREAD_QRY_CNF,     AT_RcvDrvAgentSimlockDataReadQryCnf},
    {DRV_AGENT_PHONEPHYNUM_SET_CNF,         AT_RcvDrvAgentPhonePhynumSetCnf},
    {DRV_AGENT_PHONEPHYNUM_QRY_CNF,         AT_RcvDrvAgentPhonePhynumQryCnf},
    {DRV_AGENT_OPWORD_SET_CNF,              AT_RcvDrvAgentOpwordSetCnf},

    {DRV_AGENT_SWVER_SET_CNF,               AT_RcvDrvAgentSwverSetCnf},



    {DRV_AGENT_NVMANUFACTUREEXT_SET_CNF,    AT_RcvNvManufactureExtSetCnf},


    {DRV_AGENT_QRY_CCPU_MEM_INFO_CNF,       AT_RcvDrvAgentQryCcpuMemInfoCnf},

    {DRV_AGENT_SIMLOCKWRITEEX_SET_CNF,       AT_RcvDrvAgentSimlockWriteExSetCnf},
    {DRV_AGENT_SIMLOCKDATAREADEX_SET_CNF,    AT_RcvDrvAgentSimlockDataReadExReadCnf},
};


/* AT模块处理来自MTA消息函数对应表*/
const AT_PROC_MSG_FROM_MTA_STRU g_astAtProcMsgFromMtaTab[]=
{
    /* 消息ID */                            /* 消息处理函数 */
    {ID_MTA_AT_CPOS_SET_CNF,                AT_RcvMtaCposSetCnf},
    {ID_MTA_AT_CPOSR_IND,                   AT_RcvMtaCposrInd},
    {ID_MTA_AT_XCPOSRRPT_IND,               AT_RcvMtaXcposrRptInd},
    {ID_MTA_AT_CGPSCLOCK_SET_CNF,           AT_RcvMtaCgpsClockSetCnf},

    {ID_MTA_AT_SIMLOCKUNLOCK_SET_CNF,       AT_RcvMtaSimlockUnlockSetCnf},

    {ID_MTA_AT_QRY_NMR_CNF,                 AT_RcvMtaQryNmrCnf},

    {ID_MTA_AT_WRR_AUTOTEST_QRY_CNF,        AT_RcvMtaWrrAutotestQryCnf},
    {ID_MTA_AT_WRR_CELLINFO_QRY_CNF,        AT_RcvMtaWrrCellinfoQryCnf},
    {ID_MTA_AT_WRR_MEANRPT_QRY_CNF,         AT_RcvMtaWrrMeanrptQryCnf},
    {ID_MTA_AT_WRR_FREQLOCK_SET_CNF,        AT_RcvMtaWrrFreqLockSetCnf},
    {ID_MTA_AT_WRR_RRC_VERSION_SET_CNF,     AT_RcvMtaWrrRrcVersionSetCnf},
    {ID_MTA_AT_WRR_CELLSRH_SET_CNF,         AT_RcvMtaWrrCellSrhSetCnf},
    {ID_MTA_AT_WRR_FREQLOCK_QRY_CNF,        AT_RcvMtaWrrFreqLockQryCnf},
    {ID_MTA_AT_WRR_RRC_VERSION_QRY_CNF,     AT_RcvMtaWrrRrcVersionQryCnf},
    {ID_MTA_AT_WRR_CELLSRH_QRY_CNF,         AT_RcvMtaWrrCellSrhQryCnf},

    {ID_MTA_AT_GAS_AUTOTEST_QRY_CNF,        AT_RcvMtaGrrAutotestQryCnf},

    {ID_MTA_AT_BODY_SAR_SET_CNF,           AT_RcvMtaBodySarSetCnf},

    {ID_MTA_AT_CURC_QRY_CNF,                AT_RcvMtaQryCurcCnf},
    {ID_MTA_AT_UNSOLICITED_RPT_SET_CNF,     AT_RcvMtaSetUnsolicitedRptCnf},
    {ID_MTA_AT_UNSOLICITED_RPT_QRY_CNF,     AT_RcvMtaQryUnsolicitedRptCnf},

    {ID_MTA_AT_IMEI_VERIFY_QRY_CNF,         AT_RcvMtaImeiVerifyQryCnf},
    {ID_MTA_AT_CGSN_QRY_CNF,                AT_RcvMtaCgsnQryCnf},
    {ID_MTA_AT_NCELL_MONITOR_SET_CNF,       AT_RcvMtaSetNCellMonitorCnf},
    {ID_MTA_AT_NCELL_MONITOR_QRY_CNF,       AT_RcvMtaQryNCellMonitorCnf},
    {ID_MTA_AT_NCELL_MONITOR_IND,           AT_RcvMtaNCellMonitorInd},

    {ID_MTA_AT_REFCLKFREQ_SET_CNF,          AT_RcvMtaRefclkfreqSetCnf},
    {ID_MTA_AT_REFCLKFREQ_QRY_CNF,          AT_RcvMtaRefclkfreqQryCnf},
    {ID_MTA_AT_REFCLKFREQ_IND,              AT_RcvMtaRefclkfreqInd},

#if(FEATURE_OFF == FEATURE_UE_MODE_NR)
    {ID_MTA_AT_RFICSSIRD_QRY_CNF,           AT_RcvMtaRficSsiRdQryCnf},
#endif    

    {ID_MTA_AT_HANDLEDECT_SET_CNF,          AT_RcvMtaHandleDectSetCnf},
    {ID_MTA_AT_HANDLEDECT_QRY_CNF,          AT_RcvMtaHandleDectQryCnf},

    {ID_MTA_AT_PS_TRANSFER_IND,             AT_RcvMtaPsTransferInd},

#if (FEATURE_ON == FEATURE_DSDS)
    {ID_MTA_AT_RRC_PROTECT_PS_CNF,          AT_RcvMtaPsProtectSetCnf},
#endif
    { ID_MTA_AT_PHY_INIT_CNF,               AT_RcvMtaPhyInitCnf},

    {ID_MTA_AT_ECID_SET_CNF,                AT_RcvMtaEcidSetCnf},

    {ID_MTA_AT_MIPICLK_QRY_CNF,             AT_RcvMtaMipiInfoCnf},
    {ID_MTA_AT_MIPICLK_INFO_IND,            AT_RcvMtaMipiInfoInd},
    {ID_MTA_AT_SET_DPDTTEST_FLAG_CNF,       AT_RcvMtaSetDpdtTestFlagCnf},
    {ID_MTA_AT_SET_DPDT_VALUE_CNF,          AT_RcvMtaSetDpdtValueCnf},
    {ID_MTA_AT_QRY_DPDT_VALUE_CNF,          AT_RcvMtaQryDpdtValueCnf},

    {ID_MTA_AT_SET_JAM_DETECT_CNF,          AT_RcvMtaSetJamDetectCnf},
    {ID_MTA_AT_QRY_JAM_DETECT_CNF,          AT_RcvMtaQryJamDetectCnf},
    {ID_MTA_AT_JAM_DETECT_IND,              AT_RcvMtaJamDetectInd},

    {ID_MTA_AT_SET_FREQ_LOCK_CNF,           AT_RcvMtaSetRatFreqLockCnf},

    {ID_MTA_AT_SET_GSM_FREQLOCK_CNF,        AT_RcvMtaSetGFreqLockCnf},

    {ID_MTA_AT_QRY_GSM_FREQLOCK_CNF,        AT_RcvMtaGFreqLockQryCnf},

    #if (FEATURE_ON == FEATURE_PROBE_FREQLOCK)
    {ID_MTA_AT_SET_M2M_FREQLOCK_CNF,        AT_RcvMtaSetM2MFreqLockCnf},
    {ID_MTA_AT_QRY_M2M_FREQLOCK_CNF,        AT_RcvMtaQryM2MFreqLockCnf},
    #endif
    {ID_MTA_AT_XPASS_INFO_IND,              AT_RcvMtaXpassInfoInd},

    {ID_AT_MTA_SET_FEMCTRL_CNF,             AT_RcvMtaSetFemctrlCnf},


#if(FEATURE_ON == FEATURE_LTE)
#if(FEATURE_ON == FEATURE_LTE_MBMS)
    {ID_MTA_AT_MBMS_SERVICE_OPTION_SET_CNF,     AT_RcvMtaMBMSServiceOptSetCnf},
    {ID_MTA_AT_MBMS_SERVICE_STATE_SET_CNF,      AT_RcvMtaMBMSServiceStateSetCnf},
    {ID_MTA_AT_MBMS_PREFERENCE_SET_CNF,         AT_RcvMtaMBMSPreferenceSetCnf},
    {ID_MTA_AT_SIB16_NETWORK_TIME_QRY_CNF,      AT_RcvMtaMBMSSib16NetworkTimeQryCnf},
    {ID_MTA_AT_BSSI_SIGNAL_LEVEL_QRY_CNF,       AT_RcvMtaMBMSBssiSignalLevelQryCnf},
    {ID_MTA_AT_NETWORK_INFO_QRY_CNF,            AT_RcvMtaMBMSNetworkInfoQryCnf},
    {ID_MTA_AT_EMBMS_STATUS_QRY_CNF,            AT_RcvMtaMBMSModemStatusQryCnf},
    {ID_MTA_AT_MBMS_UNSOLICITED_CFG_SET_CNF,    AT_RcvMtaMBMSEVSetCnf},
    {ID_MTA_AT_MBMS_SERVICE_EVENT_IND,          AT_RcvMtaMBMSServiceEventInd},
    {ID_MTA_AT_INTEREST_LIST_SET_CNF,           AT_RcvMtaMBMSInterestListSetCnf},
    {ID_MTA_AT_MBMS_AVL_SERVICE_LIST_QRY_CNF,   AT_RcvMtaMBMSCmdQryCnf},
#endif
    {ID_MTA_AT_LTE_LOW_POWER_SET_CNF,           AT_RcvMtaLteLowPowerSetCnf},
    {ID_MTA_AT_LTE_WIFI_COEX_SET_CNF,           AT_RcvMtaIsmCoexSetCnf},
    {ID_MTA_AT_LTE_WIFI_COEX_QRY_CNF,           AT_RcvMtaIsmCoexQryCnf},
    {ID_MTA_AT_LTE_CA_CFG_SET_CNF,              AT_RcvMtaLteCaCfgSetCnf},
    {ID_MTA_AT_LTE_CA_CELLEX_QRY_CNF,           AT_RcvMtaLteCaCellExQryCnf},
    {ID_MTA_AT_LTE_CA_CELLEX_INFO_NTF,          AT_RcvMtaLteCaCellExInfoNtf},
    {ID_MTA_AT_LTE_CA_CELL_RPT_CFG_SET_CNF,     AT_RcvMtaCACellSetCnf},
    {ID_MTA_AT_LTE_CA_CELL_RPT_CFG_QRY_CNF,     AT_RcvMtaCACellQryCnf},
    {ID_MTA_AT_LTE_FINE_TIME_SET_CNF,           AT_RcvMtaFineTimeSetCnf},
    {ID_MTA_AT_SIB_FINE_TIME_NTF,               AT_RcvMtaSibFineTimeNtf},
    {ID_MTA_AT_LPP_FINE_TIME_NTF,               AT_RcvMtaLppFineTimeNtf},
    {ID_MTA_AT_EPS_NETWORK_INFO_IND,            AT_RcvMtaEpsNetworkInfoInd},
    {ID_MTA_AT_EPS_NETWORK_QRY_CNF,             AT_RcvMtaEpsNetworkQryCnf},
    {ID_MTA_AT_LL2_COM_CFG_SET_CNF,             AT_RcvMtaLL2ComCfgSetCnf},
    {ID_MTA_AT_LL2_COM_CFG_QRY_CNF,             AT_RcvMtaLL2ComCfgQryCnf},
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    {ID_MTA_AT_NL2_COM_CFG_SET_CNF,             AT_RcvMtaNL2ComCfgSetCnf},
    {ID_MTA_AT_NL2_COM_CFG_QRY_CNF,             AT_RcvMtaNL2ComCfgQryCnf},
#endif

    {ID_MTA_AT_SET_FR_CNF,                  AT_RcvMtaSetFrCnf},

    {ID_MTA_AT_SIB16_TIME_UPDATE_IND,       AT_RcvMtaSib16TimeUpdateInd},

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    {ID_MTA_AT_LENDC_QRY_CNF,               AT_RcvMtaLendcQryCnf},
    {ID_MTA_AT_LENDC_INFO_IND,              AT_RcvMtaLendcInfoInd},
    {ID_MTA_AT_NRRCCAP_CFG_SET_CNF,         AT_RcvMtaNrrcCapCfgCnf},
    {ID_MTA_AT_NRRCCAP_QRY_CNF,             AT_RcvMtaNrrcCapQryCnf},
    {ID_MTA_AT_NRPWRCTRL_SET_CNF,           AT_RcvMtaNrPwrCtrlSetCnf},
#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)
    {ID_MTA_AT_NR_FREQLOCK_SET_CNF,         AT_RcvMtaNrFreqLockSetCnf},
    {ID_MTA_AT_NR_FREQLOCK_QRY_CNF,         AT_RcvMtaNrFreqLockQryCnf},
#endif

#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    {ID_MTA_AT_MEID_SET_CNF,                AT_RcvMtaMeidSetCnf},
    {ID_MTA_AT_MEID_QRY_CNF,                AT_RcvMtaMeidQryCnf},

    {ID_MTA_AT_EVDO_SYS_EVENT_CNF,          AT_RcvMtaEvdoSysEventSetCnf},
    {ID_MTA_AT_EVDO_SIG_MASK_CNF,           AT_RcvMtaEvdoSigMaskSetCnf},
    {ID_MTA_AT_EVDO_REVA_RLINK_INFO_IND,    AT_RcvMtaEvdoRevARLinkInfoInd},
    {ID_MTA_AT_EVDO_SIG_EXEVENT_IND,        AT_RcvMtaEvdoSigExEventInd},
    {ID_MTA_AT_NO_CARD_MODE_SET_CNF,        AT_RcvMtaNoCardModeSetCnf},
    {ID_MTA_AT_NO_CARD_MODE_QRY_CNF,        AT_RcvMtaNoCardModeQryCnf},

#endif
    {ID_MTA_AT_TRANSMODE_QRY_CNF,           AT_RcvMtaTransModeQryCnf},

    {ID_MTA_AT_UE_CENTER_SET_CNF,           AT_RcvMtaUECenterSetCnf},
    {ID_MTA_AT_UE_CENTER_QRY_CNF,           AT_RcvMtaUECenterQryCnf},
    {ID_MTA_AT_SET_NETMON_SCELL_CNF,        AT_RcvMtaSetNetMonSCellCnf},
    {ID_MTA_AT_SET_NETMON_NCELL_CNF,        AT_RcvMtaSetNetMonNCellCnf},

    {ID_MTA_AT_QRY_AFC_CLK_FREQ_XOCOEF_CNF, AT_RcvMtaAfcClkInfoCnf},
    {ID_MTA_AT_ANQUERY_QRY_CNF,             AT_RcvMtaAnqueryQryCnf},
    {ID_MTA_AT_CSNR_QRY_CNF,                AT_RcvMtaCsnrQryCnf},
    {ID_MTA_AT_CSQLVL_QRY_CNF,              AT_RcvMtaCsqlvlQryCnf},
    {ID_MTA_AT_XCPOSR_SET_CNF,              AT_RcvMtaSetXCposrCnf},
    {ID_MTA_AT_XCPOSR_QRY_CNF,              AT_RcvMtaQryXcposrCnf},
    {ID_MTA_AT_XCPOSRRPT_SET_CNF,           AT_RcvMtaSetXcposrRptCnf},
    {ID_MTA_AT_XCPOSRRPT_QRY_CNF,           AT_RcvMtaQryXcposrRptCnf},
    {ID_MTA_AT_CLEAR_HISTORY_FREQ_CNF,      AT_RcvMtaClearHistoryFreqCnf},
    {ID_MTA_AT_SET_SENSOR_CNF,              AT_RcvMtaSetSensorCnf},
    {ID_MTA_AT_SET_SCREEN_CNF,              AT_RcvMtaSetScreenCnf},
    {ID_MTA_AT_FRAT_IGNITION_QRY_CNF,       AT_RcvFratIgnitionQryCnf},
    {ID_MTA_AT_FRAT_IGNITION_SET_CNF,       AT_RcvFratIgnitionSetCnf},

    {ID_MTA_AT_SET_MODEM_TIME_CNF,          AT_RcvMtaSetModemTimeCnf},

    {ID_MTA_AT_RX_TEST_MODE_SET_CNF,        AT_RcvMtaSetRxTestModeCnf},

    {ID_MTA_AT_AFC_CLK_UNLOCK_CAUSE_IND,    AT_RcvMtaAfcClkUnlockCauseInd},

#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)
    {ID_MTA_AT_SLAVE_SET_CNF,               AT_RcvMtaSetSlaveCnf},
#endif

    {ID_MTA_AT_BESTFREQ_SET_CNF,            AT_RcvMtaSetBestFreqCnf},
    {ID_MTA_AT_BEST_FREQ_INFO_IND,          AT_RcvMtaBestFreqInfoInd},
    {ID_MTA_AT_BESTFREQ_QRY_CNF,            AT_RcvMtaQryBestFreqCnf},

    {ID_MTA_AT_RFIC_DIE_ID_QRY_CNF,         AT_RcvMtaRficDieIDQryCnf},

    {ID_MTA_AT_MIPI_WREX_CNF,               AT_RcvMtaMipiWrEXCnf},
    {ID_MTA_AT_MIPI_RDEX_CNF,               AT_RcvMtaMipiRdEXCnf},

    {ID_MTA_AT_PHY_COM_CFG_SET_CNF,         AT_RcvMtaPhyComCfgSetCnf},

    {ID_MTA_AT_CRRCONN_SET_CNF,             AT_RcvMtaSetCrrconnCnf},
    {ID_MTA_AT_CRRCONN_QRY_CNF,             AT_RcvMtaQryCrrconnCnf},
    {ID_MTA_AT_CRRCONN_STATUS_IND,          AT_RcvMtaCrrconnStatusInd},
    {ID_MTA_AT_VTRLQUALRPT_SET_CNF,         AT_RcvMtaSetVtrlqualrptCnf},
    {ID_MTA_AT_RL_QUALITY_INFO_IND,         AT_RcvMtaRlQualityInfoInd},
    {ID_MTA_AT_VIDEO_DIAG_INFO_RPT,         AT_RcvMtaVideoDiagInfoRpt},

    {ID_MTA_AT_PMU_DIE_SN_QRY_CNF,          AT_RcvMtaPmuDieSNQryCnf},

    {ID_MTA_AT_MODEM_CAP_UPDATE_CNF,        AT_RcvMtaModemCapUpdateCnf},

    {ID_MTA_AT_TAS_TEST_CFG_CNF,            AT_RcvMtaTasTestCfgCnf},
    {ID_MTA_AT_TAS_TEST_QRY_CNF,            AT_RcvMtaTasTestQryCnf},

    {ID_MTA_AT_LTE_CATEGORY_INFO_IND,       AT_RcvMtaLteCategoryInfoInd},

    {ID_MTA_AT_ACCESS_STRATUM_REL_IND,      AT_RcvMtaAccessStratumRelInd},

    {ID_MTA_AT_RS_INFO_QRY_CNF,             AT_RcvMtaRsInfoQryCnf},

    {ID_MTA_AT_ERRCCAP_CFG_SET_CNF,         AT_RcvMtaErrcCapCfgCnf},
    {ID_MTA_AT_ERRCCAP_QRY_SET_CNF,         AT_RcvMtaErrcCapQryCnf},

    {ID_MTA_AT_DEL_CELLENTITY_CNF,          AT_RcvMtaDelCellEntityCnf},

    {ID_MTA_AT_PSEUCELL_INFO_SET_CNF,       AT_RcvMtaPseucellInfoSetCnf},
#if(FEATURE_OFF == FEATURE_UE_MODE_NR)
    {ID_MTA_AT_POWER_DET_QRY_CNF,           AT_RcvMtaPowerDetQryCnf},
#endif

    {ID_MTA_AT_MIPIREAD_SET_CNF,            AT_RcvMtaSetMipiReadCnf},

    {ID_MTA_AT_PHYMIPIWRITE_SET_CNF,        AT_RcvMtaSetPhyMipiWriteCnf},

    {ID_MTA_AT_CHRALARMRLAT_CFG_SET_CNF,    AT_RcvMtaChrAlarmRlatCfgSetCnf},

    {ID_MTA_AT_ECC_CFG_SET_CNF,             AT_RcvMtaEccCfgCnf},
    {ID_MTA_AT_ECC_STATUS_IND,              AT_RcvMtaEccStatusInd},

    {ID_MTA_AT_EPDUR_DATA_IND,              AT_RcvMtaEpduDataInd},
    {ID_MTA_AT_EPDU_SET_CNF,                AT_RcvMtaEpduSetCnf},

#if ( FEATURE_ON == FEATURE_DCXO_HI1102_SAMPLE_SHARE )
    {ID_MTA_AT_DCXO_SAMPLE_QRY_CNF,         AT_RcvMtaRcmDcxoSampleQryCnf},
#endif

    {ID_MTA_AT_PHY_COMM_ACK_IND,            AT_RcvMtaPhyCommAckInd},

    {ID_MTA_AT_COMM_BOOSTER_SET_CNF,        AT_RcvMtaCommBoosterSetCnf},
    {ID_MTA_AT_COMM_BOOSTER_IND,            AT_RcvMtaCommBoosterInd},
    {ID_MTA_AT_COMM_BOOSTER_QUERY_CNF,      AT_RcvMtaCommBoosterQueryCnf},

    {ID_MTA_AT_NVLOAD_SET_CNF,              AT_RcvMtaNvLoadSetCnf},

    {ID_MTA_AT_SMS_DOMAIN_SET_CNF,          AT_RcvMtaSmsDomainSetCnf},
    {ID_MTA_AT_SMS_DOMAIN_QRY_CNF,          AT_RcvMtaSmsDomainQryCnf},

    {ID_MTA_AT_SET_SAMPLE_CNF,              AT_RcvMtaSetSampleCnf},

    {ID_MTA_AT_GPS_LOCSET_SET_CNF,          AT_RcvMtaGpsLocSetCnf},

    {ID_MTA_AT_CCLK_QRY_CNF,                AT_RcvMtaCclkQryCnf},

    {ID_MTA_AT_TEMP_PROTECT_IND,            AT_RcvMtaTempProtectInd},

    {ID_AT_MTA_GAME_MODE_SET_CNF,           AT_RcvGameModeSetCnf},
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    {ID_MTA_AT_CDMA_MODEM_CAP_SET_CNF,      AT_RcvCdmaModemSetCnf},
    {ID_MTA_AT_CDMA_CAP_RESUME_SET_CNF,     AT_RcvCdmaCapResumeSetCnf},
#endif
    {ID_MTA_AT_NV_REFRESH_SET_CNF,          AT_RcvMtaNvRefreshSetCnf},

    {ID_MTA_AT_PSEUDBTS_SET_CNF,            AT_RcvMtaPseudBtsSetCnf},

    {ID_MTA_AT_SUBCLFSPARAM_SET_CNF,        AT_RcvMtaSubClfSparamSetCnf},
    {ID_MTA_AT_SUBCLFSPARAM_QRY_CNF,        AT_RcvMtaSubClfSparamQryCnf},


    {ID_MTA_AT_PSEUD_BTS_IDENT_IND,         AT_RcvMtaPseudBtsIdentInd},

    {ID_MTA_AT_FORCESYNC_SET_CNF,         AT_RcvMtaForceSyncSetCnf},
    {ID_AT_MTA_SET_LTEPWRDISS_CNF,          AT_RcvMtaAtLtePwrDissSetCnf},
};

#if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_AGPS) && (FEATURE_ON == FEATURE_XPDS))
const AT_PROC_MSG_FROM_XPDS_STRU g_astAtProcMsgFromXpdsTab[]=
{
    {ID_XPDS_AT_GPS_START_CNF,                    AT_RcvXpdsCagpsRlstCnf},
    {ID_XPDS_AT_GPS_CFG_MPC_ADDR_CNF,             AT_RcvXpdsCagpsRlstCnf},
    {ID_XPDS_AT_GPS_CFG_PDE_ADDR_CNF,             AT_RcvXpdsCagpsRlstCnf},
    {ID_XPDS_AT_GPS_CFG_MODE_CNF,                 AT_RcvXpdsCagpsRlstCnf},
    {ID_XPDS_AT_GPS_STOP_CNF,                     AT_RcvXpdsCagpsRlstCnf},

    {ID_XPDS_AT_GPS_TIME_INFO_IND,                AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_REFLOC_INFO_CNF,              AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_PDE_POSI_INFO_IND,            AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_NI_SESSION_IND,               AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_START_REQ,                    AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_CANCEL_IND,                   AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_ACQ_ASSIST_DATA_IND,          AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_ABORT_IND,                    AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_DEL_ASSIST_DATA_IND,          AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_ION_INFO_IND,                 AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_EPH_INFO_IND,                 AT_RcvXpdsEphInfoInd},
    {ID_XPDS_AT_GPS_ALM_INFO_IND,                 AT_RcvXpdsAlmInfoInd},
    {ID_XPDS_AT_GPS_NI_CP_STOP,                   AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_NI_CP_START,                  AT_RcvXpdsCagpsCnf},

    {ID_XPDS_AT_AP_DATA_CALL_REQ,                 AT_RcvXpdsAgpsDataCallReq},
    {ID_XPDS_AT_AP_SERVER_BIND_REQ,               AT_RcvXpdsAgpsServerBindReq},
    {ID_XPDS_AT_AP_REVERSE_DATA_IND,              AT_RcvXpdsAgpsReverseDataInd},

    {ID_XPDS_AT_GPS_UTS_TEST_START_REQ,           AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_UTS_TEST_STOP_REQ,            AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_UTS_GPS_POS_INFO_IND,             AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_OM_TEST_START_REQ,            AT_RcvXpdsCagpsCnf},
    {ID_XPDS_AT_GPS_OM_TEST_STOP_REQ,             AT_RcvXpdsCagpsCnf},
};
#endif

const AT_PROC_MSG_FROM_CALL_STRU g_astAtProcMsgFromCallTab[]=
{
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT)
    {ID_TAF_CALL_APP_ENCRYPT_VOICE_CNF,                     AT_RcvXcallEncryptCallCnf},
    {ID_TAF_CALL_APP_ENCRYPT_VOICE_IND,                     AT_RcvXcallEncryptCallInd},
    {ID_TAF_CALL_APP_EC_REMOTE_CTRL_IND,                    AT_RcvXcallEccRemoteCtrlInd},
    {ID_TAF_CALL_APP_REMOTE_CTRL_ANSWER_CNF,                AT_RcvXcallRemoteCtrlAnsCnf},
    {ID_TAF_CALL_APP_ECC_SRV_CAP_CFG_CNF,                   AT_RcvXcallEccCapSetCnf},
    {ID_TAF_CALL_APP_ECC_SRV_CAP_QRY_CNF,                   AT_RcvXcallEccCapQryCnf},
#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE)
    {ID_TAF_CALL_APP_SET_EC_TEST_MODE_CNF,                  AT_RcvXcallSetEccTestModeCnf},
    {ID_TAF_CALL_APP_GET_EC_TEST_MODE_CNF,                  AT_RcvXcallQryEccTestModeCnf},
    {ID_TAF_CALL_APP_GET_EC_RANDOM_CNF,                     AT_RcvXcallQryEccRandomCnf},
    {ID_TAF_CALL_APP_GET_EC_KMC_CNF,                        AT_RcvXcallQryEccKmcCnf},
    {ID_TAF_CALL_APP_SET_EC_KMC_CNF,                        AT_RcvXcallSetEccKmcCnf},
    {ID_TAF_CALL_APP_ENCRYPTED_VOICE_DATA_IND,              AT_RcvXcallEncryptedVoiceDataInd},
#endif
#endif
    {ID_TAF_CALL_APP_PRIVACY_MODE_SET_CNF,                  AT_RcvXcallPrivacyModeSetCnf},
    {ID_TAF_CALL_APP_PRIVACY_MODE_QRY_CNF,                  AT_RcvXcallPrivacyModeQryCnf},
    {ID_TAF_CALL_APP_PRIVACY_MODE_IND,                      AT_RcvXcallPrivacyModeInd},
#endif
    {ID_TAF_CALL_APP_CNAP_INFO_IND,                         AT_RcvTafCallCnapInfoInd},
    {ID_TAF_CALL_APP_CNAP_QRY_CNF,                          AT_RcvTafCallCnapQryCnf},
};

const AT_PROC_MSG_FROM_CCM_STRU g_astAtProcMsgFromCcmTab[]=
{
    {ID_TAF_CCM_QRY_CHANNEL_INFO_CNF,                       AT_RcvTafCcmCSChannelInfoQryCnf},
    {ID_TAF_CCM_CALL_CHANNEL_INFO_IND,                      AT_RcvTafCcmChannelInfoInd},
#if (FEATURE_ON == FEATURE_IMS)
    {ID_TAF_CCM_CALL_MODIFY_CNF,                            At_RcvTafCcmCallModifyCnf},
    {ID_TAF_CCM_CALL_ANSWER_REMOTE_MODIFY_CNF,              At_RcvTafCcmCallAnswerRemoteModifyCnf},
    {ID_TAF_CCM_QRY_ECONF_CALLED_INFO_CNF,                  AT_RcvTafCcmQryEconfCalledInfoCnf},
    {ID_TAF_CCM_CALL_MODIFY_STATUS_IND,                     AT_RcvTafCcmCallModifyStatusInd},
#endif
};

const AT_PROC_MSG_FROM_MMA_STRU g_astAtProcMsgFromMmaTab[]=
{
    /* 消息ID */                            /* 消息处理函数 */
    {ID_TAF_MMA_OM_MAINTAIN_INFO_IND,   AT_RcvMmaOmMaintainInfoInd},

    {ID_TAF_MMA_USIM_STATUS_IND,            AT_RcvAtMmaUsimStatusInd},
    {ID_TAF_MMA_CMM_SET_CNF,            AT_RcvMmaCmmSetCmdRsp},
    {ID_TAF_MMA_SIMLOCK_STAUS_QUERY_CNF, AT_RcvSimLockQryRsp},
    {ID_TAF_MMA_AC_INFO_QRY_CNF,        AT_RcvMmaAcInfoQueryCnf},
    {ID_TAF_MMA_AC_INFO_CHANGE_IND,     AT_RcvMmaAcInfoChangeInd},
    {ID_TAF_MMA_EOPLMN_SET_CNF,         AT_RcvMmaEOPlmnSetCnf},
    {ID_TAF_MMA_NET_SCAN_CNF,           AT_RcvMmaNetScanCnf},
    {ID_TAF_MMA_USER_SRV_STATE_QRY_CNF,   AT_RcvMmaUserSrvStateQryCnf},
    {ID_TAF_MMA_POWER_ON_AND_REG_TIME_QRY_CNF,     AT_RcvMmaPwrOnAndRegTimeQryCnf},
    {ID_TAF_MMA_SPN_QRY_CNF,            AT_RcvMmaSpnQryCnf},
    {ID_TAF_MMA_MMPLMNINFO_QRY_CNF,     AT_RcvMmaMMPlmnInfoQryCnf},
    {ID_TAF_MMA_LAST_CAMP_PLMN_QRY_CNF,           AT_RcvMmaPlmnQryCnf},
    {ID_TAF_MMA_EOPLMN_QRY_CNF,         AT_RcvMmaEOPlmnQryCnf},
    {ID_TAF_MMA_COPN_INFO_QRY_CNF,      AT_RcvMmaCopnInfoQueryCnf},
#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)
    {ID_TAF_MMA_SIM_INSERT_CNF,          AT_RcvMmaSimInsertRsp},
#endif
    {ID_TAF_MMA_SYS_CFG_SET_CNF,        AT_RcvMmaSysCfgSetCnf},
    {ID_TAF_MMA_PHONE_MODE_SET_CNF,     AT_RcvMmaPhoneModeSetCnf},
    {ID_TAF_MMA_DETACH_CNF,             AT_RcvMmaDetachCnf},
    {ID_TAF_MMA_ATTACH_CNF,             AT_RcvMmaAttachCnf},
    {ID_TAF_MMA_ATTACH_STATUS_QRY_CNF,  AT_RcvMmaAttachStatusQryCnf},

    {ID_TAF_MMA_EMC_CALL_BACK_NTF,      AT_RcvMmaEmcCallBackNtf},

    {ID_TAF_MMA_EMC_CALL_BACK_QRY_CNF,  AT_RcvMmaQryEmcCallBackCnf},
    {ID_TAF_MMA_QUIT_CALLBACK_SET_CNF,  AT_RcvMmaQuitCallBackCnf},

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    {ID_TAF_MMA_CL_DBDOMAIN_STATUSE_SET_CNF,   AT_RcvMmaClDbDomainStatusSetCnf},
    {ID_TAF_MMA_CL_DBDOMAIN_STATUSE_INFO_IND,   AT_RcvMmaClDbdomainStatusReportInd},
    {ID_TAF_MMA_CDMA_LOCINFO_QRY_CNF,   AT_RcvMmaCLocInfoQueryCnf},
    {ID_TAF_MMA_CSID_IND,               AT_RcvMmaCSidInd},

    /* 移出CDMA编译开关 */
#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)
    {ID_TAF_MMA_CSIDLIST_SET_CNF,       AT_RcvMmaSetCSidListCnf},
#endif
    /* 移出CDMA编译开关 */
    {ID_TAF_MMA_CURR_SID_NID_QRY_CNF,   AT_RcvMmaQryCurrSidNidCnf},

    {ID_TAF_MMA_CLOCINFO_IND,           AT_RcvMmaClocinfoInd},
    {ID_TAF_MMA_ROAMING_MODE_SWITCH_IND,       AT_RcvMmaRoamingModeSwitchInd},
    {ID_TAF_MMA_CTCC_ROAMING_NW_INFO_QRY_CNF,    AT_RcvMmaCtRoamInfoCnf},
    {ID_TAF_MMA_CTCC_ROAMING_NW_INFO_REPORT_IND, AT_RcvMmaCtRoamingInfoChgInd},
    {ID_TAF_MMA_CTCC_OOS_COUNT_SET_CNF, AT_RcvMmaCtOosCountCnf},
    {ID_TAF_MMA_CTCC_ROAMING_NW_INFO_RTP_CFG_SET_CNF, AT_RcvMmaCtRoamRptCfgSetCnf},
    {ID_TAF_MMA_PRLID_QRY_CNF,           AT_RcvMmaPrlIdQryCnf},

    {ID_TAF_MMA_COMBINED_MODE_SWITCH_IND,       AT_RcvMmaCombinedModeSwitchInd},

    {ID_TAF_MMA_ICC_APP_TYPE_SWITCH_IND,        AT_RcvMmaIccAppTypeSwitchInd},
    {ID_TAF_MMA_RAT_COMBINED_MODE_QRY_CNF,      AT_RcvMmaRatCombinedModeQryCnf},
    {ID_TAF_MMA_CTIME_IND,              AT_RcvMmaCTimeInd},

    {ID_TAF_MMA_CDMA_FREQ_LOCK_SET_CNF, AT_RcvMmaCFreqLockSetCnf},
    {ID_TAF_MMA_CDMA_FREQ_LOCK_QRY_CNF, AT_RcvMmaCFreqLockQueryCnf},

    {ID_TAF_MMA_CDMACSQ_SET_CNF,        AT_RcvMmaCdmaCsqSetCnf},
    {ID_TAF_MMA_CDMACSQ_QRY_CNF,        AT_RcvMmaCdmaCsqQryCnf},
    {ID_TAF_MMA_CDMACSQ_IND,            AT_RcvMmaCdmaCsqInd},

    {ID_MMA_TAF_HDR_CSQ_VALUE_IND,      AT_RcvMmaHdrCsqInd},
    {ID_MMA_TAF_HDR_CSQ_SET_CNF,        AT_RcvMmaHdrCsqSetCnf},
    {ID_MMA_TAF_HDR_CSQ_QRY_SETTING_CNF,AT_RcvMmaHdrCsqQryCnf},
#endif

    {ID_TAF_MMA_CFPLMN_SET_CNF,         AT_RcvMmaCFPlmnSetCnf},
    {ID_TAF_MMA_CFPLMN_QUERY_CNF,       AT_RcvMmaCFPlmnQueryCnf},

    {ID_TAF_MMA_PREF_PLMN_SET_CNF,     AT_RcvMmaPrefPlmnSetCnf},
    {ID_TAF_MMA_PREF_PLMN_QUERY_CNF,   AT_RcvMmaPrefPlmnQueryCnf},
    {ID_TAF_MMA_PREF_PLMN_TEST_CNF,    AT_RcvMmaPrefPlmnTestCnf},

    {ID_TAF_MMA_PLMN_SEARCH_CNF,   AT_RcvMmaPlmnSearchCnf},
    {ID_TAF_MMA_PLMN_LIST_ABORT_CNF,   AT_RcvMmaPlmnListAbortCnf},
    {ID_TAF_MMA_LOCATION_INFO_QRY_CNF,       AT_RcvMmaLocInfoQueryCnf},
    {ID_TAF_MMA_CIPHER_QRY_CNF,        AT_RcvMmaCipherInfoQueryCnf},
    {ID_TAF_MMA_PREF_PLMN_TYPE_SET_CNF,  AT_RcvMmaPrefPlmnTypeSetCnf},
#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)
    {ID_TAF_MMA_MT_POWER_DOWN_CNF,     AT_RcvMmaPowerDownCnf},
#endif


    {ID_TAF_MMA_PHONE_MODE_QRY_CNF,   AT_RcvMmaPhoneModeQryCnf},
    {ID_TAF_MMA_TIME_CHANGE_IND,      AT_RcvMmaTimeChangeInd},
    {ID_TAF_MMA_MODE_CHANGE_IND,      AT_RcvMmaModeChangeInd},
    {ID_TAF_MMA_PLMN_CHANGE_IND,      AT_RcvMmaPlmnChangeInd},

    {ID_TAF_MMA_CERSSI_SET_CNF,        AT_RcvMmaCerssiSetCnf},
    {ID_TAF_MMA_CERSSI_QRY_CNF,        AT_RcvMmaCerssiInfoQueryCnf},
    {ID_TAF_MMA_ACCESS_MODE_QRY_CNF,   AT_RcvMmaAccessModeQryCnf},
    {ID_TAF_MMA_COPS_QRY_CNF,          AT_RcvMmaCopsQryCnf},
    {ID_TAF_MMA_REG_STATE_QRY_CNF,     AT_RcvMmaRegStateQryCnf},
    {ID_TAF_MMA_AUTO_ATTACH_QRY_CNF,   AT_RcvMmaAutoAttachQryCnf},
    {ID_TAF_MMA_SYSINFO_QRY_CNF,       AT_RcvMmaSysInfoQryCnf},
    /* ANTENNA_INFO_QRY_CNF 移到MTA模块处理 */
    {ID_TAF_MMA_HOME_PLMN_QRY_CNF,     AT_RcvMmaEHplmnInfoQryCnf},

    {ID_TAF_MMA_SRV_STATUS_IND,        AT_RcvMmaSrvStatusInd},
    {ID_TAF_MMA_RSSI_INFO_IND,         AT_RcvMmaRssiInfoInd},
    {ID_TAF_MMA_REG_STATUS_IND,        AT_RcvMmaRegStatusInd},
    {ID_TAF_MMA_REG_REJ_INFO_IND,      AT_RcvMmaRegRejInfoInd},
    {ID_TAF_MMA_PLMN_SELECTION_INFO_IND,  AT_RcvMmaPlmnSelectInfoInd},


    {ID_TAF_MMA_QUICKSTART_SET_CNF,     AT_RcvTafMmaQuickStartSetCnf},
    {ID_TAF_MMA_AUTO_ATTACH_SET_CNF,    AT_RcvTafMmaAutoAttachSetCnf},
    {ID_TAF_MMA_SYSCFG_QRY_CNF,         AT_RcvTafMmaSyscfgQryCnf},
    {ID_TAF_MMA_SYSCFG_TEST_CNF,        AT_RcvTafMmaSyscfgTestCnf},
    {ID_TAF_MMA_CRPN_QRY_CNF,           AT_RcvTafMmaCrpnQryCnf},
    {ID_TAF_MMA_QUICKSTART_QRY_CNF,     AT_RcvTafMmaQuickStartQryCnf},
    /* CSNR_QRY_CNF移到MTA模块处理 */
    {ID_TAF_MMA_CSQ_QRY_CNF,            AT_RcvTafMmaCsqQryCnf},
    /* CSQLVL_QRY_CNF移到MTA模块处理 */

    {ID_TAF_MMA_BATTERY_CAPACITY_QRY_CNF, AT_RcvMmaCbcQryCnf},
    {ID_TAF_MMA_HAND_SHAKE_QRY_CNF,      AT_RcvMmaHsQryCnf},
    {ID_TAF_MMA_PLMN_LIST_CNF,          At_QryParaPlmnListProc},
    {ID_TAF_MSG_MMA_EONS_UCS2_CNF,      AT_RcvMmaEonsUcs2Cnf},

#if (FEATURE_ON == FEATURE_CSG)
     {ID_TAF_MMA_CSG_LIST_SEARCH_CNF,       AT_RcvMmaCsgListSearchCnfProc},
     {ID_TAF_MMA_CSG_LIST_ABORT_CNF,        AT_RcvMmaCsgListAbortCnf},
	 {ID_TAF_MMA_CSG_SPEC_SEARCH_CNF,       AT_RcvMmaCsgSpecSearchCnfProc},
     {ID_TAF_MMA_QRY_CAMP_CSG_ID_INFO_CNF,  AT_RcvMmaQryCampCsgIdInfoCnfProc},
#endif
#if (FEATURE_ON == FEATURE_IMS)
    {ID_TAF_MMA_IMS_SWITCH_SET_CNF,     AT_RcvMmaImsSwitchSetCnf},
    {ID_TAF_MMA_IMS_SWITCH_QRY_CNF,     AT_RcvMmaImsSwitchQryCnf},
    {ID_TAF_MMA_VOICE_DOMAIN_SET_CNF,   AT_RcvMmaVoiceDomainSetCnf},
    {ID_TAF_MMA_VOICE_DOMAIN_QRY_CNF,   AT_RcvMmaVoiceDomainQryCnf},

    {ID_TAF_MMA_IMS_DOMAIN_CFG_SET_CNF, AT_RcvMmaImsDomainCfgSetCnf},
    {ID_TAF_MMA_IMS_DOMAIN_CFG_QRY_CNF, AT_RcvMmaImsDomainCfgQryCnf},

    {ID_TAF_MMA_ROAM_IMS_SUPPORT_SET_CNF, AT_RcvMmaRoamImsSupportSetCnf},

    {ID_TAF_MMA_IMS_VIDEO_CALL_CAP_SET_CNF, AT_RcvMmaImsVideoCallCapSetCnf},

    {ID_TAF_MMA_IMS_SMS_CFG_QRY_CNF, AT_RcvMmaImsSmsCfgQryCnf},
    {ID_TAF_MMA_IMS_SMS_CFG_SET_CNF, AT_RcvMmaImsSmsCfgSetCnf},

    {ID_TAF_MMA_RCS_SWITCH_QRY_CNF,  AT_RcvMmaRcsSwitchQryCnf},
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    {ID_TAF_MMA_CLIMS_CFG_INFO_IND,  AT_RcvMmaCLImsCfgInfoInd},
#endif
#endif
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    {ID_TAF_MMA_1XCHAN_SET_CNF,     AT_RcvMma1xChanSetCnf},
    {ID_TAF_MMA_1XCHAN_QUERY_CNF,   AT_RcvMma1xChanQueryCnf},
    {ID_TAF_MMA_CVER_QUERY_CNF,     AT_RcvMmaCVerQueryCnf},
    {ID_TAF_MMA_GETSTA_QUERY_CNF,   AT_RcvMmaStateQueryCnf},
    {ID_TAF_MMA_CHIGHVER_QUERY_CNF, AT_RcvMmaCHverQueryCnf},
#endif
    {ID_TAF_MMA_SRCHED_PLMN_INFO_IND, AT_RcvMmaSrchedPlmnInfoInd},

    {ID_TAF_MMA_DPLMN_SET_CNF,       AT_RcvMmaDplmnSetCnf},
    {ID_TAF_MMA_DPLMN_QRY_CNF,       AT_RcvMmaDplmnQryCnf},
    {ID_TAF_MMA_BORDER_INFO_SET_CNF,       AT_RcvMmaBorderInfoSetCnf},
    {ID_TAF_MMA_BORDER_INFO_QRY_CNF,       AT_RcvMmaBorderInfoQryCnf},

    {ID_TAF_MMA_EXCHANGE_MODEM_INFO_CNF,         AT_RcvMmaExchangeModemInfoCnf},

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    {ID_TAF_MMA_CLMODE_IND,           AT_RcvMmaCLModInd},

#endif
    {ID_TAF_MMA_INIT_LOC_INFO_IND,    AT_RcvMmaInitLocInfoInd},

    {ID_TAF_MMA_EFLOCIINFO_SET_CNF,     AT_RcvMmaEflociInfoSetCnf},
    {ID_TAF_MMA_EFLOCIINFO_QRY_CNF,     AT_RcvMmaEflociInfoQryCnf},
    {ID_TAF_MMA_EFPSLOCIINFO_SET_CNF,   AT_RcvMmaPsEflociInfoSetCnf},
    {ID_TAF_MMA_EFPSLOCIINFO_QRY_CNF,   AT_RcvMmaPsEflociInfoQryCnf},

    {ID_TAF_MMA_IMSI_REFRESH_IND,       AT_RcvMmaImsiRefreshInd},

    {ID_TAF_MMA_PACSP_QRY_CNF,       AT_RcvMmaPacspQryCnf},

    {ID_TAF_MMA_MT_REATTACH_IND,         AT_RcvMmaMtReattachInd},

    {ID_TAF_MMA_CIND_SET_CNF,        AT_RcvMmaCindSetCnf},
#if (FEATURE_ON == FEATURE_DSDS)
    {ID_TAF_MMA_DSDS_STATE_SET_CNF,  AT_RcvMmaDsdsStateSetCnf},
    {ID_TAF_MMA_DSDS_STATE_NOTIFY,   AT_RcvMmaDsdsStateNotify},
#endif

    {ID_TAF_MMA_CEMODE_SET_CNF,         AT_RcvMmaCemodeSetCnf},
    {ID_TAF_MMA_CEMODE_QRY_CNF,         AT_RcvMmaCemodeQryCnf},

    {ID_TAF_MMA_REJINFO_QRY_CNF,        AT_RcvMmaRejinfoQryCnf},

    {ID_TAF_MMA_EMRSSICFG_SET_CNF,   AT_RcvMmaEmRssiCfgSetCnf},
    {ID_TAF_MMA_EMRSSICFG_QRY_CNF,   AT_RcvMmaEmRssiCfgQryCnf},
    {ID_TAF_MMA_EMRSSIRPT_SET_CNF,   AT_RcvMmaEmRssiRptSetCnf},
    {ID_TAF_MMA_EMRSSIRPT_QRY_CNF,   AT_RcvMmaEmRssiRptQryCnf},
    {ID_TAF_MMA_EMRSSIRPT_IND,       AT_RcvMmaEmRssiRptInd},

    {ID_TAF_MMA_ELEVATOR_STATE_IND,     AT_RcvMmaElevatorStateInd},

    {ID_TAF_MMA_ULFREQRPT_SET_CNF,      AT_RcvMmaUlFreqRptSetCnf},
    {ID_TAF_MMA_ULFREQ_CHANGE_IND,      AT_RcvMmaUlFreqChangeInd},
    {ID_TAF_MMA_ULFREQRPT_QRY_CNF,      AT_RcvMmaUlFreqRptQryCnf},

    {ID_TAF_MMA_PS_SCENE_SET_CNF,       AT_RcvMmaPsSceneSetCnf},
    {ID_TAF_MMA_PS_SCENE_QRY_CNF,       AT_RcvMmaPsSceneQryCnf},
};

AT_MSG_PROC_STRU        g_astAtMsgProc[] = 
{
    {WUEPS_PID_AT           , At_CmdMsgDistr        },
    {I0_WUEPS_PID_TAF       , At_TafAndDmsMsgProc   },
    {I1_WUEPS_PID_TAF       , At_TafAndDmsMsgProc   },
    {I2_WUEPS_PID_TAF       , At_TafAndDmsMsgProc   },
    {I0_UEPS_PID_DSM        , At_TafAndDmsMsgProc   },
    {I1_UEPS_PID_DSM        , At_TafAndDmsMsgProc   },
    {I2_UEPS_PID_DSM        , At_TafAndDmsMsgProc   },
    {PS_PID_APP_PPP         , At_PppMsgProc         },
    {PS_PID_APP_NDIS        , AT_RcvNdisMsg         },
    {I0_DSP_PID_WPHY        , At_HPAMsgProc         },
    {I1_DSP_PID_WPHY        , At_HPAMsgProc         },
    {I0_DSP_PID_GPHY        , At_GHPAMsgProc        },
    {I1_DSP_PID_GPHY        , At_GHPAMsgProc        },
    {I2_DSP_PID_GPHY        , At_GHPAMsgProc        },
    {I0_WUEPS_PID_MMA       , At_MmaMsgProc         },
    {I1_WUEPS_PID_MMA       , At_MmaMsgProc         },
    {I2_WUEPS_PID_MMA       , At_MmaMsgProc         },
    {I0_WUEPS_PID_DRV_AGENT , At_ProcMsgFromDrvAgent},
    {I1_WUEPS_PID_DRV_AGENT , At_ProcMsgFromDrvAgent},
    {I2_WUEPS_PID_DRV_AGENT , At_ProcMsgFromDrvAgent},
    {I0_WUEPS_PID_VC        , At_ProcMsgFromVc      },
    {I1_WUEPS_PID_VC        , At_ProcMsgFromVc      },
    {I1_WUEPS_PID_VC        , At_ProcMsgFromVc      },
    {ACPU_PID_RNIC          , At_RcvRnicMsg         },
    {I0_WUEPS_PID_CC        , At_ProcMsgFromCc      },
    {I1_WUEPS_PID_CC        , At_ProcMsgFromCc      },
    {I1_WUEPS_PID_CC        , At_ProcMsgFromCc      },
#if (FEATURE_ON == FEATURE_LTE)
    {I0_MSP_L4_L4A_PID      , at_L4aCnfProc         },
    {I1_MSP_L4_L4A_PID      , at_L4aCnfProc         },
    {I0_MSP_SYS_FTM_PID     , At_FtmEventMsgProc    },
    {I1_MSP_SYS_FTM_PID     , At_FtmEventMsgProc    },
#endif
    {I0_MAPS_STK_PID        , At_STKMsgProc         },
    {I1_MAPS_STK_PID        , At_STKMsgProc         },
    {I2_MAPS_STK_PID        , At_STKMsgProc         },
    {I0_MAPS_PB_PID         , At_PbMsgProc          },
    {I1_MAPS_PB_PID         , At_PbMsgProc          },
    {I2_MAPS_PB_PID         , At_PbMsgProc          },
    {I0_MAPS_PIH_PID        , At_PIHMsgProc         },
    {I1_MAPS_PIH_PID        , At_PIHMsgProc         },
    {I2_MAPS_PIH_PID        , At_PIHMsgProc         },
    {I0_WUEPS_PID_RABM      , AT_RabmMsgProc        },
    {I1_WUEPS_PID_RABM      , AT_RabmMsgProc        },
    {I2_WUEPS_PID_RABM      , AT_RabmMsgProc        },
    {WUEPS_PID_SPY          , AT_SpyMsgProc         },
    {I0_UEPS_PID_MTA        , At_ProcMtaMsg         },
    {I1_UEPS_PID_MTA        , At_ProcMtaMsg         },
    {I2_UEPS_PID_MTA        , At_ProcMtaMsg         },
#if (FEATURE_ON == FEATURE_IMS)
    {I0_PS_PID_IMSA         , AT_ProcImsaMsg        },
    {I1_PS_PID_IMSA         , AT_ProcImsaMsg        },
#endif
    {PS_PID_CSS             , AT_ProcCssMsg         },
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    {I0_UEPS_PID_XSMS       , AT_ProcXsmsMsg        },
    {I1_UEPS_PID_XSMS       , AT_ProcXsmsMsg        },
    {I2_UEPS_PID_XSMS       , AT_ProcXsmsMsg        },
    {DSP_PID_PROCSTUB       , At_CHPAMsgProc        },
#endif
#if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_AGPS) && (FEATURE_ON == FEATURE_XPDS))
    {I0_UEPS_PID_XPDS       , AT_ProcXpdsMsg        },
    {I1_UEPS_PID_XPDS       , AT_ProcXpdsMsg        },
    {I2_UEPS_PID_XPDS       , AT_ProcXpdsMsg        },
#endif
#if(FEATURE_ON == FEATURE_UE_MODE_NR)
    {CCPU_PID_CBT           , AT_ProcCbtMsg         },
    {DSP_PID_BBA_CAL        , AT_ProcBbicMsg        },
#endif
    {I0_UEPS_PID_CCM        , At_CcmMsgProc         },
    {I1_UEPS_PID_CCM        , At_CcmMsgProc         },
    {I2_UEPS_PID_CCM        , At_CcmMsgProc         }
};


/* AT码流缓存数组(包括AT命令或其它数据)。
   设置该缓存数组的原因:底软任务在调用At_CmdStreamPreProc接口时，某些场景下(如直接使用超级终端发送AT命令),则会
   出现AT码流以一个字符为单位发送到AT的消息队列中，导致AT的消息队列慢，触发单板复位。*/
AT_DATA_STREAM_BUFFER_STRU              g_aucAtDataBuff[AT_MAX_CLIENT_NUM];

/*CREG/CGREG的<CI>域以4字节上报是否使能(VDF需求)*/

AT_CLIENT_MANAGE_STRU                   gastAtClientTab[AT_MAX_CLIENT_NUM];

TAF_UINT32                              gulPBPrintTag = VOS_FALSE;

extern VOS_BOOL                         g_bSetFlg;

#if (FEATURE_ON == FEATURE_PROBE_FREQLOCK)
AT_FREQLOCK_MODE_TBL_STRU    g_astFreqLockModeTbl[] =
{
    {AT_FREQLOCK_MODE_TYPE_GSM          ,0, "01"},
    {AT_FREQLOCK_MODE_TYPE_WCDMA        ,0, "02"},
    {AT_FREQLOCK_MODE_TYPE_TDSCDMA      ,0, "03"},
    {AT_FREQLOCK_MODE_TYPE_LTE          ,0, "04"},
};

AT_FREQLOCK_GSM_BAND_TBL_STRU    g_astFreqLockGsmBandTbl[] =
{
    {AT_FREQLOCK_BAND_TYPE_GSM_850     ,0, "00"},
    {AT_FREQLOCK_BAND_TYPE_GSM_900     ,0, "01"},
    {AT_FREQLOCK_BAND_TYPE_GSM_1800    ,0, "02"},
    {AT_FREQLOCK_BAND_TYPE_GSM_1900    ,0, "03"},
};
#endif/*FEATURE_ON == FEATURE_PROBE_FREQLOCK*/


typedef struct
{
    VOS_UINT16                  usMcc;
    VOS_INT8                    cZone;
    VOS_UINT8                   ucReserved;
}MCC_ZONE_INFO_STRU;

MCC_ZONE_INFO_STRU g_astMccZoneInfoTbl[] =
{
    {0x202, +30  * 4 / 10, 0},  /* Greece */
    {0x204, +10  * 4 / 10, 0},  /* Greece */
    {0x206, +10  * 4 / 10, 0},  /* Holland */
    {0x208, +10  * 4 / 10, 0},  /* France */
    {0x212, +10  * 4 / 10, 0},  /* Monaco */
    {0x213, +10  * 4 / 10, 0},  /* Andorra */
    {0x214, +10  * 4 / 10, 0},  /* Spain */
    {0x216, +10  * 4 / 10, 0},  /* Hungary */
    {0x218, +10  * 4 / 10, 0},  /* Bosnia and Herzegovina, the Republic of */
    {0x219, +10  * 4 / 10, 0},  /* Croatia */
    {0x220, +10  * 4 / 10, 0},  /* Montenegro / Monte Nige Lu / Serbia*/
    {0x222, +10  * 4 / 10, 0},  /* Italy */
    {0x226, +20  * 4 / 10, 0},  /* Romania */
    {0x228, +10  * 4 / 10, 0},  /* Switzerland */
    {0x230, +10  * 4 / 10, 0},  /* Czech */
    {0x231, +10  * 4 / 10, 0},  /* Slovakia */
    {0x232, +10  * 4 / 10, 0},  /* Austria */
    {0x234, +10  * 4 / 10, 0},  /* Guernsey */
    {0x238, +10  * 4 / 10, 0},  /* Denmark */
    {0x240, +10  * 4 / 10, 0},  /* Sweden */
    {0x242, +10  * 4 / 10, 0},  /* Norway */
    {0x244, +20  * 4 / 10, 0},  /* Finland */
    {0x246, +20  * 4 / 10, 0},  /* Lithuania */
    {0x247, +20  * 4 / 10, 0},  /* Latvia */
    {0x248, +20  * 4 / 10, 0},  /* Estonia */
    {0x250, +80  * 4 / 10, 0},  /* Russia GMT+3 ~ GMT+12 */
    {0x255, +20  * 4 / 10, 0},  /* Ukraine */
    {0x257, +20  * 4 / 10, 0},  /* Belarus */
    {0x259, +20  * 4 / 10, 0},  /* Moldova */
    {0x260, +10  * 4 / 10, 0},  /* Poland */
    {0x262, +10  * 4 / 10, 0},  /* Germany */
    {0x266, +10  * 4 / 10, 0},  /* Gibraltar */
    {0x268, +0   * 4 / 10, 0},  /* Portugal */
    {0x270, +10  * 4 / 10, 0},  /* Luxembourg */
    {0x272, +0   * 4 / 10, 0},  /* Ireland  GMT+0 ~ GMT+1*/
    {0x274, +0   * 4 / 10, 0},  /*Iceland  */
    {0x276, +10  * 4 / 10, 0},  /* Albania */
    {0x278, +10  * 4 / 10, 0},  /* Malta */
    {0x280, +20  * 4 / 10, 0},  /* Cyprus */
    {0x282, +40  * 4 / 10, 0},  /* Georgia */
    {0x283, +40  * 4 / 10, 0},  /* Armenia */
    {0x284, +20  * 4 / 10, 0},  /* Bulgaria */
    {0x286, +20  * 4 / 10, 0},  /* Turkey */
    {0x288, +0   * 4 / 10, 0},  /* Faroe Islands */
    {0x290, -20  * 4 / 10, 0},  /* Greenland GMT+0 ~ GMT-4 */
    {0x292, +10  * 4 / 10, 0},  /* San Marino */
    {0x293, +10  * 4 / 10, 0},  /* Slovenia */
    {0x294, +10  * 4 / 10, 0},  /* Macedonia */
    {0x295, +10  * 4 / 10, 0},  /* Liechtenstein */
    {0x302, -40  * 4 / 10, 0},  /* Canada */
    {0x308, -30  * 4 / 10, 0},  /* Saint-Pierre and Miquelon */
    {0x310, -80  * 4 / 10, 0},  /* America */
    {0x311, -80  * 4 / 10, 0},  /* America */
    {0x330, -40  * 4 / 10, 0},  /* Puerto Rico */
    {0x334, -70  * 4 / 10, 0},  /* Mexico */
    {0x338, -50  * 4 / 10, 0},  /* Jamaica */
    {0x340, -40  * 4 / 10, 0},  /* Martinique */
    {0x342, -40  * 4 / 10, 0},  /* Barbados */
    {0x344, -40  * 4 / 10, 0},  /* Antigua and Barbuda */
    {0x346, -50  * 4 / 10, 0},  /* Cayman Islands */
    {0x348, -40  * 4 / 10, 0},  /* The British Virgin Islands */
    {0x350, -30  * 4 / 10, 0},  /* Bermuda */
    {0x352, -40  * 4 / 10, 0},  /* Grenada */
    {0x354, -40  * 4 / 10, 0},  /* Montserrat */
    {0x356, -40  * 4 / 10, 0},  /* Saint Kitts and Nevis */
    {0x358, -40  * 4 / 10, 0},  /* St. Lucia */
    {0x360, -40  * 4 / 10, 0},  /* Saint Vincent and the Grenadines */
    {0x362, -40  * 4 / 10, 0},  /* Netherlands Antilles */
    {0x363, -40  * 4 / 10, 0},  /* Aruba */
    {0x364, -50  * 4 / 10, 0},  /* Bahamas */
    {0x365, -40  * 4 / 10, 0},  /* Anguilla */
    {0x366, -50  * 4 / 10, 0},  /* Dominican */
    {0x368, -50  * 4 / 10, 0},  /* Cuba */
    {0x370, -50  * 4 / 10, 0},  /* Dominican Republic */
    {0x372, -50  * 4 / 10, 0},  /* Haiti */
    {0x374, -40  * 4 / 10, 0},  /* The Republic of Trinidad and Tobago */
    {0x376, -50  * 4 / 10, 0},  /* Turks and Caicos Islands */
    {0x400, +40  * 4 / 10, 0},  /* Republic of Azerbaijan */
    {0x401, +40  * 4 / 10, 0},  /* Kazakhstan */
    {0x402, +60  * 4 / 10, 0},  /* Bhutan */
    {0x404, +55  * 4 / 10, 0},  /* India */
    {0x405, +55  * 4 / 10, 0},  /*India*/
    {0x410, +50  * 4 / 10, 0},  /*Pakistan*/
    {0x412, +45  * 4 / 10, 0},  /*Afghanistan*/
    {0x413, +55  * 4 / 10, 0},  /*Sri Lanka*/
    {0x414, +65  * 4 / 10, 0},  /* Myanmar */
    {0x415, +30  * 4 / 10, 0},  /* Lebanon */
    {0x416, +20  * 4 / 10, 0},  /* Jordan */
    {0x417, +20  * 4 / 10, 0},  /* Syria */
    {0x418, +30  * 4 / 10, 0},  /* Iraq */
    {0x419, +30  * 4 / 10, 0},  /* Kuwait */
    {0x420, +30  * 4 / 10, 0},  /* Saudi Arabia */
    {0x421, +30  * 4 / 10, 0},  /* Yemen */
    {0x422, +40  * 4 / 10, 0},  /* Oman */
    {0x424, +40  * 4 / 10, 0},  /* United Arab Emirates */
    {0x425, +20  * 4 / 10, 0},  /* Israel */
    {0x426, +30  * 4 / 10, 0},  /* Bahrain  */
    {0x427, +30  * 4 / 10, 0},  /* Qatar */
    {0x428, +80  * 4 / 10, 0},  /* Mongolia */
    {0x429, +58  * 4 / 10, 0},  /* Nepal */
    {0x432, +35  * 4 / 10, 0},  /* Iran */
    {0x434, +50  * 4 / 10, 0},  /* Uzbekistan */
    {0x436, +50  * 4 / 10, 0},  /* Tajikistan */
    {0x437, +60  * 4 / 10, 0},  /* Kyrgyzstan */
    {0x438, +50  * 4 / 10, 0},  /* Turkmenistan */
    {0x440, +90  * 4 / 10, 0},  /* Japan */
    {0x450, +90  * 4 / 10, 0},  /* South Korea */
    {0x452, +70  * 4 / 10, 0},  /* Vietnam */
    {0x454, +80  * 4 / 10, 0},  /* Hong Kong */
    {0x455, +80  * 4 / 10, 0},  /* Macau */
    {0x456, +70  * 4 / 10, 0},  /* Cambodia */
    {0x457, +70  * 4 / 10, 0},  /* Laos */
    {0x460, +80  * 4 / 10, 0},  /*China*/
    {0x466, +80  * 4 / 10, 0},  /* Taiwan */
    {0x467, +90  * 4 / 10, 0},  /* North Korea */
    {0x470, +60  * 4 / 10, 0},  /* Bangladesh */
    {0x472, +50  * 4 / 10, 0},  /* Maldives */
    {0x502, +80  * 4 / 10, 0},  /*  */
    {0x505, +90  * 4 / 10, 0},  /*  */
    {0x510, +80  * 4 / 10, 0},  /*  */
    {0x514, +90  * 4 / 10, 0},  /*  */
    {0x515, +80  * 4 / 10, 0},  /*  */
    {0x520, +70  * 4 / 10, 0},  /*  */
    {0x525, +80  * 4 / 10, 0},  /*  */
    {0x528, +80  * 4 / 10, 0},  /*  */
    {0x530, +120 * 4 / 10, 0},  /*  */
    {0x537, +100 * 4 / 10, 0},  /*  */
    {0x539, +130 * 4 / 10, 0},  /*  */
    {0x540, +110 * 4 / 10, 0},  /*  */
    {0x541, +110 * 4 / 10, 0},  /*  */
    {0x542, +120 * 4 / 10, 0},  /*  */
    {0x544, -110 * 4 / 10, 0},  /* American Samoa*/
    {0x545, +130 * 4 / 10, 0},  /*GMT +13 - +15*/
    {0x546, +110 * 4 / 10, 0},  /*  */
    {0x547, -100 * 4 / 10, 0},  /*  */
    {0x548, -100 * 4 / 10, 0},  /*  */
    {0x549, -110 * 4 / 10, 0},  /*  */
    {0x550, +100 * 4 / 10, 0},  /*  */
    {0x552, +90  * 4 / 10, 0},  /*  */
    {0x602, +20  * 4 / 10, 0},  /*  */
    {0x603, +10  * 4 / 10, 0},  /*  */
    {0x604, +0   * 4 / 10, 0},  /*  */
    {0x605, +10  * 4 / 10, 0},  /*  */
    {0x606, +20  * 4 / 10, 0},  /*  */
    {0x607, +0   * 4 / 10, 0},  /*  */
    {0x608, +0   * 4 / 10, 0},  /*  */
    {0x609, +0   * 4 / 10, 0},  /*  */
    {0x610, +0   * 4 / 10, 0},  /*  */
    {0x611, +0   * 4 / 10, 0},  /*  */
    {0x612, +0   * 4 / 10, 0},  /*  */
    {0x613, +0   * 4 / 10, 0},  /*  */
    {0x614, +10  * 4 / 10, 0},  /*  */
    {0x615, +0   * 4 / 10, 0},  /*  */
    {0x616, +10  * 4 / 10, 0},  /*  */
    {0x617, +40  * 4 / 10, 0},  /*  */
    {0x618, +0   * 4 / 10, 0},  /*  */
    {0x619, +0   * 4 / 10, 0},  /*  */
    {0x620, +0   * 4 / 10, 0},  /*  */
    {0x621, +10  * 4 / 10, 0},  /*  */
    {0x622, +10  * 4 / 10, 0},  /*  */
    {0x623, +10  * 4 / 10, 0},  /*  */
    {0x624, +10  * 4 / 10, 0},  /*  */
    {0x625, +10  * 4 / 10, 0},  /*  */
    {0x626, +0   * 4 / 10, 0},  /*  */
    {0x627, +10  * 4 / 10, 0},  /*  */
    {0x628, +10  * 4 / 10, 0},  /*  */
    {0x629, +10  * 4 / 10, 0},  /*  */
    {0x630, +20  * 4 / 10, 0},  /*  */
    {0x631, +10  * 4 / 10, 0},  /*  */
    {0x632, +0   * 4 / 10, 0},  /*  */
    {0x633, +40  * 4 / 10, 0},  /*  */
    {0x634, +30  * 4 / 10, 0},  /*  */
    {0x635, +20  * 4 / 10, 0},  /*  */
    {0x636, +30  * 4 / 10, 0},  /* Ethiopia */
    {0x637, +30  * 4 / 10, 0},  /* Somalia */
    {0x638, +30  * 4 / 10, 0},  /* Djibouti */
    {0x639, +30  * 4 / 10, 0},  /* Kenya */
    {0x640, +30  * 4 / 10, 0},  /* Tanzania */
    {0x641, +30  * 4 / 10, 0},  /* Uganda */
    {0x642, +20  * 4 / 10, 0},  /* Burundi */
    {0x643, +20  * 4 / 10, 0},  /* Mozambique */
    {0x645, +20  * 4 / 10, 0},  /* Zambia */
    {0x646, +30  * 4 / 10, 0},  /* Madagascar */
    {0x647, +0   * 4 / 10, 0},  /* not sure */
    {0x648, +20  * 4 / 10, 0},  /* Zimbabwe */
    {0x649, +10  * 4 / 10, 0},  /* Namibia */
    {0x650, +20  * 4 / 10, 0},  /* Malawi */
    {0x651, +20  * 4 / 10, 0},  /* Lesotho */
    {0x652, +20  * 4 / 10, 0},  /* Botswana */
    {0x653, +20  * 4 / 10, 0},  /* Swaziland */
    {0x654, +20  * 4 / 10, 0},  /* Comoros */
    {0x655, +20  * 4 / 10, 0},  /* South Africa */
    {0x659, +30  * 4 / 10, 0},  /* sudan */
    {0x702, -60  * 4 / 10, 0},  /* Belize */
    {0x704, -60  * 4 / 10, 0},  /* Guatemala */
    {0x706, -60  * 4 / 10, 0},  /* Salvador */
    {0x708, -60  * 4 / 10, 0},  /* Honduras */
    {0x710, -60  * 4 / 10, 0},  /* Nicaragua */
    {0x712, -60  * 4 / 10, 0},  /* Costa Rica */
    {0x714, -50  * 4 / 10, 0},  /* Panama */
    {0x716, -50  * 4 / 10, 0},  /* Peru */
    {0x722, -30  * 4 / 10, 0},  /* Argentina */
    {0x724, -30  * 4 / 10, 0},  /* Brazil */
    {0x730, -50  * 4 / 10, 0},  /* Chile */
    {0x732, -50  * 4 / 10, 0},  /* Colombia */
    {0x734, -45  * 4 / 10, 0},  /* Venezuela */
    {0x736, -40  * 4 / 10, 0},  /* Bolivia */
    {0x738, -30  * 4 / 10, 0},  /* Guyana */
    {0x740, -50  * 4 / 10, 0},  /* Ecuador */
    {0x744, -40  * 4 / 10, 0},  /* Paraguay */
    {0x746, -30  * 4 / 10, 0},  /* Suriname */
    {0x748, -30  * 4 / 10, 0},  /* Uruguay */
};


/*****************************************************************************
   3 函数、变量声明
*****************************************************************************/

extern VOS_UINT32    AT_ProcTempprtEventInd(
    TEMP_PROTECT_EVENT_AT_IND_STRU     *pstMsg
);

VOS_UINT32 AT_PlmnTimeZoneProc(
    TAF_UINT32                          ulMcc,
    NAS_MM_INFO_IND_STRU               *stMmTimeInfo
);

LOCAL VOS_VOID AT_ProcCerssiInfoQueryCampOnGsm(
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf,
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSystemAppConfig
);

LOCAL VOS_VOID AT_ProcCerssiInfoQueryCampOnWcdma(
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf,
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSystemAppConfig
);

#if (FEATURE_ON == FEATURE_LTE)
LOCAL VOS_VOID AT_ProcCerssiInfoQueryCampOnLte(
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf,
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSystemAppConfig
);
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
LOCAL VOS_VOID AT_ProcCerssiInfoQueryCampOnNr(
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf,
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSystemAppConfig
);
#endif

LOCAL VOS_VOID AT_ProcCerssiInfoQueryNotCampOn(
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf,
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSystemAppConfig
);


/*****************************************************************************
   4 函数实现
*****************************************************************************/



TAF_VOID At_MmaMsgProc(VOS_VOID *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          i;
    VOS_UINT32                          ulMsgCnt;
    VOS_UINT32                          ulRst;

    /*从g_astAtProcMsgFromMmaTab中获取消息个数*/
    ulMsgCnt = sizeof(g_astAtProcMsgFromMmaTab)/sizeof(AT_PROC_MSG_FROM_MMA_STRU);
    pstMsg   = (MSG_HEADER_STRU*)pMsg;

    /*g_astAtProcMsgFromMtaTab查表，进行消息分发*/
    for (i = 0; i < ulMsgCnt; i++)
    {
        if (g_astAtProcMsgFromMmaTab[i].ulMsgName == pstMsg->ulMsgName)
        {
            ulRst = g_astAtProcMsgFromMmaTab[i].pProcMsgFunc(pstMsg);

            if (VOS_ERR == ulRst)
            {
                AT_ERR_LOG("At_MmaMsgProc: Msg Proc Err!");
            }

            return;
        }
    }

    /*没有找到匹配的消息*/
    if (ulMsgCnt == i)
    {
        AT_ERR_LOG("At_MmaMsgProc: Msg Id is invalid!");
    }

    return;
}


TAF_UINT32 At_CallMsgProc(
    MSG_HEADER_STRU                     *pstMsg
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulMsgCnt;
    VOS_UINT32                          ulRst;

    /*从g_astAtProcMsgFromXcallTab中获取消息个数*/
    ulMsgCnt = sizeof(g_astAtProcMsgFromCallTab)/sizeof(AT_PROC_MSG_FROM_CALL_STRU);

    /*g_astAtProcMsgFromXcallTab查表，进行消息分发*/
    for (i = 0; i < ulMsgCnt; i++)
    {
        if ((g_astAtProcMsgFromCallTab[i].ulMsgName == pstMsg->ulMsgName)
         && (ID_TAF_CALL_APP_TYPE_BUTT != pstMsg->ulMsgName))
        {
            ulRst = g_astAtProcMsgFromCallTab[i].pProcMsgFunc(pstMsg);

            if (VOS_ERR == ulRst)
            {
                AT_ERR_LOG("At_CallMsgProc: Msg Proc Err!");
            }

            return AT_OK;
        }
    }

    /*没有找到匹配的消息*/
    if (ulMsgCnt == i)
    {
        AT_ERR_LOG("At_CallMsgProc: Msg Id is invalid!");
    }

    return AT_ERROR;
}



VOS_VOID At_CcmMsgProc(VOS_VOID *pMsg)
{
    MSG_HEADER_STRU                     *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          i;
    VOS_UINT32                          ulMsgCnt;
    VOS_UINT32                          ulRst;

    pstMsg = (MSG_HEADER_STRU*)pMsg;

    /*从g_astAtProcMsgFromCcmTab中获取消息个数*/
    ulMsgCnt = sizeof(g_astAtProcMsgFromCcmTab)/sizeof(AT_PROC_MSG_FROM_CCM_STRU);

    /*g_astAtProcMsgFromCcmTab查表，进行消息分发*/
    for (i = 0; i < ulMsgCnt; i++)
    {
        if ((g_astAtProcMsgFromCcmTab[i].ulMsgName == pstMsg->ulMsgName)
         && (ID_TAF_CALL_APP_TYPE_BUTT != pstMsg->ulMsgName))
        {
            ulRst = g_astAtProcMsgFromCcmTab[i].pProcMsgFunc(pstMsg);

            if (VOS_ERR == ulRst)
            {
                AT_ERR_LOG("At_CcmMsgProc: Msg Proc Err!");
            }

            return;
        }
    }

    /*没有找到匹配的消息*/
    if (ulMsgCnt == i)
    {
        AT_ERR_LOG("At_CcmMsgProc: Msg Id is invalid!");
    }

    return;
}


TAF_VOID At_EventMsgProc(MN_AT_IND_EVT_STRU *pMsg)
{

    switch(pMsg->usMsgName)
    {
        case MN_CALLBACK_MSG:
            At_SmsMsgProc(pMsg, pMsg->usLen);
            return;

        case MN_CALLBACK_SET:
            At_SetMsgProc(pMsg->aucContent, pMsg->usLen);
            return;

        case MN_CALLBACK_QRY:
            At_QryMsgProc(pMsg->aucContent, pMsg->usLen);
            return;

        case MN_CALLBACK_PS_CALL:
            /* PS域事件处理 */
            AT_RcvTafPsEvt((TAF_PS_EVT_STRU*)pMsg);
            return;

        case MN_CALLBACK_DATA_STATUS:
            At_DataStatusMsgProc(pMsg->aucContent, pMsg->usLen);
            return;

        case MN_CALLBACK_CS_CALL:
            At_CsMsgProc(pMsg,pMsg->usLen);
            return;

        case MN_CALLBACK_SS:
            AT_RcvTafSsaEvt((TAF_SSA_EVT_STRU*)pMsg);
            return;

        case MN_CALLBACK_PHONE:
            At_PhMsgProc(pMsg->aucContent, pMsg->usLen);
            break;

        case MN_CALLBACK_PHONE_BOOK:
            At_TAFPbMsgProc(pMsg->aucContent, pMsg->usLen);
            break;

        case MN_CALLBACK_CMD_CNF:
            At_CmdCnfMsgProc(pMsg->aucContent, pMsg->usLen);
            break;

        case MN_CALLBACK_VOICE_CONTROL:
            At_VcMsgProc(pMsg, pMsg->usLen);
            break;

        case MN_CALLBACK_LOG_PRINT:
            AT_LogPrintMsgProc((TAF_MNTN_LOG_PRINT_STRU *)pMsg);
            break;

        default:
            AT_LOG1("At_MsgProc: invalid message name.", pMsg->usMsgName);
            return;
    }
}



VOS_BOOL AT_E5CheckRight(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                          *pucData,
    VOS_UINT16                          usLen
)
{
    static VOS_BOOL                     bIsRight = VOS_FALSE;
    VOS_UINT8                           aucCmdTmp[20]; /* 须大于aucQuickCmd[]的长度 */
    VOS_UINT8                           aucQuickCmd[] = "AT^OPENPORT=";
    VOS_UINT16                          usQuickCmdLen;
    VOS_UINT16                          usLeftLen;
    VOS_UINT8                           *pucPwdPos;
    VOS_CHAR                            acPassword[AT_RIGHT_PWD_LEN+1];
    VOS_UINT8                           *pucAtStart ;

    /* 当前权限未开启,直接返回  */
    if ( AT_RIGHT_OPEN_FLAG_CLOSE == g_stAtRightOpenFlg.enRightOpenFlg )
    {
        return VOS_TRUE;
    }

    /* 如果已经获取到权限,则直接返回 */
    if ( VOS_TRUE == bIsRight )
    {
        return VOS_TRUE;
    }

    /* SSCOM 输入的字符串前有 0x0A */
    pucAtStart = pucData;
    while( (*pucAtStart != 'a') && (*pucAtStart != 'A')  )
    {
        pucAtStart++;
        if ( pucAtStart >= (pucData+usLen) )
        {
            /* 未找到 a 或 A break */
            break;
        }
    }
    usLen -= (VOS_UINT16)(pucAtStart - pucData);
    pucData = pucAtStart;

    /* 长度非法,直接返回无权限 */
    usQuickCmdLen = (VOS_UINT16)VOS_StrLen((VOS_CHAR *)aucQuickCmd);
    if (usLen <= usQuickCmdLen)
    {
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_FALSE;
    }

    TAF_MEM_SET_S(aucCmdTmp, sizeof(aucCmdTmp), 0x00, (VOS_SIZE_T)sizeof(aucCmdTmp));
    TAF_MEM_CPY_S(aucCmdTmp, sizeof(aucCmdTmp), pucData, (VOS_SIZE_T)usQuickCmdLen);

    /* 不是AT^OPENPORT命令直接返回无权限 */
    if (0 != VOS_StrNiCmp((VOS_CHAR *)aucCmdTmp,
                          (VOS_CHAR *)aucQuickCmd,
                          (VOS_SIZE_T)usQuickCmdLen))
    {
        At_FormatResultData(ucIndex,AT_ERROR);
        return VOS_FALSE;
    }

    usLeftLen = usLen - usQuickCmdLen;
    pucPwdPos = pucData + usQuickCmdLen;

    if ( usLeftLen >= sizeof(aucCmdTmp) )
    {
        At_FormatResultData(ucIndex,AT_ERROR);
        return VOS_FALSE;
    }

    TAF_MEM_SET_S(aucCmdTmp, sizeof(aucCmdTmp), 0x00, sizeof(aucCmdTmp));
    TAF_MEM_CPY_S(aucCmdTmp, sizeof(aucCmdTmp), pucPwdPos, usLeftLen);

    TAF_MEM_SET_S(acPassword, sizeof(acPassword), 0x00, sizeof(acPassword));
    TAF_MEM_CPY_S(acPassword, sizeof(acPassword), g_stAtRightOpenFlg.acPassword, sizeof(g_stAtRightOpenFlg.acPassword));

    /* 比较密码是否相同*/
    if ( 0 == VOS_StrCmp(acPassword, (VOS_CHAR *)aucCmdTmp))
    {
        /* 上报OK并记录有权限 */
        At_FormatResultData(ucIndex, AT_OK);
        bIsRight = VOS_TRUE;
        return VOS_FALSE;
    }

    /* 上报ERROR */
    At_FormatResultData(ucIndex, AT_ERROR);
    return VOS_FALSE;
}


TAF_VOID At_PppProtocolRelIndProc(AT_PPP_PROTOCOL_REL_IND_MSG_STRU *pMsg)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usPppId;

    usPppId = pMsg->usPppId;
    ucIndex = gastAtPppIndexTab[usPppId];

    if ( (AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
      || (AT_HSUART_USER == gastAtClientTab[ucIndex].UserType) )
    {
        if ( (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode)
          && (AT_PPP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
          && (AT_CMD_WAIT_PPP_PROTOCOL_REL_SET == gastAtClientTab[ucIndex].CmdCurrentOpt) )
        {
            /* 将AT通道切换为命令模式 */
            At_SetMode(ucIndex, AT_CMD_MODE, AT_NORMAL_MODE);

            /* 停止定时器 */
            AT_STOP_TIMER_CMD_READY(ucIndex);

            /* 回复NO CARRIER */
            gstAtSendData.usBufLen = 0;
            At_FormatResultData(ucIndex, AT_NO_CARRIER);
        }
    }

    return;
}


VOS_VOID AT_ModemMscIndProc(AT_PPP_MODEM_MSC_IND_MSG_STRU *pMsg)
{
    VOS_UINT8                           ucIndex;

    ucIndex = pMsg->ucIndex;

    AT_MNTN_TraceInputMsc(ucIndex, (AT_DCE_MSC_STRU *)pMsg->aucMscInd);

    if ( (AT_MODEM_USER == gastAtClientTab[ucIndex].UserType)
      || (AT_HSUART_USER == gastAtClientTab[ucIndex].UserType) )
    {
        AT_ModemStatusPreProc(ucIndex, (AT_DCE_MSC_STRU *)pMsg->aucMscInd);
    }
    else
    {
        AT_WARN_LOG1("AT_ModemMscIndProc: index %d is unused", ucIndex);
    }

    return;
}


TAF_VOID At_PppMsgProc(VOS_VOID *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg = VOS_NULL_PTR;

    pstMsg = (MSG_HEADER_STRU*)pMsg;
    
    switch  (pstMsg->ulMsgName)
    {
        case AT_PPP_RELEASE_IND_MSG:
            At_PppReleaseIndProc(((AT_PPP_RELEASE_IND_MSG_STRU *)pstMsg)->ucIndex);
            break;

        case AT_PPP_MODEM_MSC_IND_MSG:
            AT_ModemMscIndProc((AT_PPP_MODEM_MSC_IND_MSG_STRU *)pstMsg);
            break;

        case AT_PPP_PROTOCOL_REL_IND_MSG:
            At_PppProtocolRelIndProc((AT_PPP_PROTOCOL_REL_IND_MSG_STRU *)pstMsg);
            break;

        default:
            AT_LOG1("At_PppMsgProc: ulMsgName is abnormal!",pstMsg->ulMsgName);
            break;
    }
}



VOS_VOID  At_HPAMsgProc(VOS_VOID *pstMsg)
{
#if(FEATURE_OFF == FEATURE_UE_MODE_NR)  
    HPA_AT_HEADER_STRU                  *pHeader;

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_HpaMsgProc: pstMsg is NULL");
        return;
    }

    pHeader = (HPA_AT_HEADER_STRU *)pstMsg;

    switch (pHeader->usMsgID)
    {
        case ID_HPA_AT_RF_CFG_CNF:
            At_HpaRfCfgCnfProc((HPA_AT_RF_CFG_CNF_STRU *)pstMsg);
            break;

        case ID_HPA_AT_RF_RX_RSSI_IND:
            At_RfRssiIndProc((HPA_AT_RF_RX_RSSI_IND_STRU *)pstMsg);
            break;

        case ID_WPHY_AT_TX_CLT_IND:
            At_WTxCltIndProc((WPHY_AT_TX_CLT_IND_STRU *)pstMsg);
            break;

        case ID_AT_WPHY_RF_PLL_STATUS_CNF:
            At_RfPllStatusCnfProc((PHY_AT_RF_PLL_STATUS_CNF_STRU *)pstMsg);
            break;

        case ID_AT_PHY_POWER_DET_CNF:
            At_RfFpowdetTCnfProc((PHY_AT_POWER_DET_CNF_STRU *)pstMsg);
            break;
        case ID_HPA_AT_MIPI_WR_CNF:
            At_MipiWrCnfProc((HPA_AT_MIPI_WR_CNF_STRU *)pstMsg);
            break;
        case ID_HPA_AT_MIPI_RD_CNF:
            At_MipiRdCnfProc((HPA_AT_MIPI_RD_CNF_STRU *)pstMsg);
            break;
        case ID_HPA_AT_SSI_WR_CNF:
            At_SsiWrCnfProc((HPA_AT_SSI_WR_CNF_STRU *)pstMsg);
            break;

        case ID_HPA_AT_SSI_RD_CNF:
            At_SsiRdCnfProc((HPA_AT_SSI_RD_CNF_STRU *)pstMsg);
            break;


        case ID_HPA_AT_PDM_CTRL_CNF:
            At_PdmCtrlCnfProc((HPA_AT_PDM_CTRL_CNF_STRU *)pstMsg);
            break;

        default:
            AT_WARN_LOG("At_HpaMsgProc: ulMsgName is Abnormal!");
            break;
    }
#endif

    return;
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_VOID  At_CHPAMsgProc(VOS_VOID *pstMsg)
{
#if(FEATURE_OFF == FEATURE_UE_MODE_NR)   
    HPA_AT_HEADER_STRU                  *pHeader = VOS_NULL_PTR;

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_HpaMsgProc: pstMsg is NULL");
        return;
    }

    pHeader = (HPA_AT_HEADER_STRU *)pstMsg;

    switch (pHeader->usMsgID)
    {
        case ID_CHPA_AT_RF_CFG_CNF:
            At_CHpaRfCfgCnfProc((CHPA_AT_RF_CFG_CNF_STRU *)pstMsg);
            break;

        case ID_AT_PHY_POWER_DET_CNF:
            At_RfFpowdetTCnfProc((VOS_VOID *)pstMsg);
            break;

        default:
            AT_WARN_LOG("At_CHPAMsgProc: ulMsgName is Abnormal!");
            break;
    }
#endif

    return;
}
#endif



VOS_VOID  At_GHPAMsgProc(VOS_VOID *pstMsg)
{
#if(FEATURE_OFF == FEATURE_UE_MODE_NR)   
    HPA_AT_HEADER_STRU         *pHeader;

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_HpaMsgProc: pstMsg is NULL");
        return;
    }

    pHeader = (HPA_AT_HEADER_STRU *)pstMsg;

    switch (pHeader->usMsgID)
    {
        case ID_GHPA_AT_RF_MSG_CNF:
            At_HpaRfCfgCnfProc((HPA_AT_RF_CFG_CNF_STRU *)pstMsg);
            break;

        case ID_GHPA_AT_RF_RX_RSSI_IND:
            At_RfRssiIndProc((HPA_AT_RF_RX_RSSI_IND_STRU *)pstMsg);
            break;

        case ID_AT_GPHY_RF_PLL_STATUS_CNF:
            At_RfPllStatusCnfProc((PHY_AT_RF_PLL_STATUS_CNF_STRU *)pstMsg);
            break;

        case ID_HPA_AT_MIPI_WR_CNF:
            At_MipiWrCnfProc((HPA_AT_MIPI_WR_CNF_STRU *)pstMsg);
            break;

        case ID_HPA_AT_MIPI_RD_CNF:
            At_MipiRdCnfProc((HPA_AT_MIPI_RD_CNF_STRU *)pstMsg);
            break;

        case ID_HPA_AT_SSI_WR_CNF:
            At_SsiWrCnfProc((HPA_AT_SSI_WR_CNF_STRU *)pstMsg);
            break;

        case ID_HPA_AT_SSI_RD_CNF:
            At_SsiRdCnfProc((HPA_AT_SSI_RD_CNF_STRU *)pstMsg);
            break;


        case ID_AT_PHY_POWER_DET_CNF:
            At_RfFpowdetTCnfProc((VOS_VOID *)pstMsg);
            break;

        default:
            AT_WARN_LOG("At_HpaMsgProc: ulMsgName is Abnormal!");
            break;
    }
#endif
    return;
}


VOS_UINT32 AT_FormatAtiCmdQryString(
    MODEM_ID_ENUM_UINT16                enModemId,
    DRV_AGENT_MSID_QRY_CNF_STRU        *pstDrvAgentMsidQryCnf
)
{
    TAF_PH_MEINFO_STRU                  stMeInfo;
    VOS_UINT16                          usDataLen;
    VOS_UINT8                           aucTmp[AT_MAX_VERSION_LEN];
    VOS_UINT32                          ulI;
    VOS_UINT8                           ucCheckData;

    /* 初始化 */
    usDataLen     = 0;
    ulI           = 0;
    ucCheckData   = 0;
    TAF_MEM_SET_S(&stMeInfo, sizeof(stMeInfo), 0x00, sizeof(TAF_PH_MEINFO_STRU));


    TAF_MEM_SET_S(aucTmp, sizeof(aucTmp), 0x00, sizeof(aucTmp));

    /* 读取Model信息 */
    if ( (VOS_NULL_PTR == pstDrvAgentMsidQryCnf)
      || (DRV_AGENT_MSID_QRY_NO_ERROR != pstDrvAgentMsidQryCnf->ulResult))
    {
        return AT_ERROR;
    }

    /* 读取Manufacturer信息 */
    usDataLen = TAF_MAX_MFR_ID_LEN + 1;
    if (NV_OK != TAF_ACORE_NV_READ(MODEM_ID_0, en_NV_Item_FMRID, aucTmp, usDataLen))
    {
        AT_WARN_LOG("AT_FormatAtiCmdQryString:WARNING:NVIM Read en_NV_Item_FMRID falied!");
        return AT_ERROR;
    }
    else
    {
        TAF_MEM_CPY_S(stMeInfo.FmrId.aucMfrId, sizeof(stMeInfo.FmrId.aucMfrId), aucTmp, usDataLen);
    }

    /* 读取IMEI信息 */
    for (ulI = 0; ulI < TAF_PH_IMEI_LEN; ulI++)
    {
        stMeInfo.ImeisV.aucImei[ulI] = pstDrvAgentMsidQryCnf->aucImei[ulI] + 0x30;
    }

    for (ulI = 0; ulI < (TAF_PH_IMEI_LEN - 2); ulI += 2)
    {
        ucCheckData += (TAF_UINT8)(((pstDrvAgentMsidQryCnf->aucImei[ulI])
                       +((pstDrvAgentMsidQryCnf->aucImei[ulI + 1UL] * 2) / 10))
                       +((pstDrvAgentMsidQryCnf->aucImei[ulI + 1UL] * 2) % 10));
    }
    ucCheckData = (10 - (ucCheckData%10)) % 10;

    stMeInfo.ImeisV.aucImei[TAF_PH_IMEI_LEN - 2] = ucCheckData + 0x30;
    stMeInfo.ImeisV.aucImei[TAF_PH_IMEI_LEN - 1] = 0;

    /* 输出ATI命令返回结果 */
    usDataLen = 0;
    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,"%s: %s\r\n","Manufacturer",stMeInfo.FmrId.aucMfrId);
    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,"%s: %s\r\n","Model",  pstDrvAgentMsidQryCnf->acModelId);
    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,"%s: %s\r\n","Revision",pstDrvAgentMsidQryCnf->acSoftwareVerId);
    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,"%s: %s\r\n","IMEI",stMeInfo.ImeisV.aucImei);
    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,"%s","+GCAP: +CGSM,+DS,+ES");

    gstAtSendData.usBufLen = usDataLen;

    return AT_OK;

}


VOS_UINT32 AT_RcvDrvAgentMsidQryCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_MSID_QRY_CNF_STRU        *pstDrvAgentMsidQryCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRet;

    enModemId = MODEM_ID_0;

    /* 初始化 */
    pRcvMsg               = (DRV_AGENT_MSG_STRU *)pMsg;
    pstDrvAgentMsidQryCnf = (DRV_AGENT_MSID_QRY_CNF_STRU *)(pRcvMsg->aucContent);

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstDrvAgentMsidQryCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentMsidQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentMsidQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }
    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_MSID_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        g_enLogPrivacyAtCmd = TAF_LOG_PRIVACY_AT_CMD_MSID;
        ulResult = AT_FormatAtiCmdQryString(enModemId, pstDrvAgentMsidQryCnf);
    }
    else if (AT_CMD_CGMM_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                                       "%s",
                                                       pstDrvAgentMsidQryCnf->acModelId);
        ulResult = AT_OK;
    }
    else if (AT_CMD_CGMR_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                                       "%s",
                                                       pstDrvAgentMsidQryCnf->acSoftwareVerId);
        ulResult = AT_OK;
    }
    else
    {
        return VOS_ERR;
    }

    ulRet = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRet)
    {
        g_enLogPrivacyAtCmd = TAF_LOG_PRIVACY_AT_CMD_BUTT;

        AT_ERR_LOG("At_SetImeiPara: Get modem id fail.");
        return VOS_ERR;
    }

    if (AT_ERROR == ulResult)
    {
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_ERROR);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_OK);
    }

    g_enLogPrivacyAtCmd = TAF_LOG_PRIVACY_AT_CMD_BUTT;

    return VOS_OK;
}




VOS_UINT32 AT_RcvDrvAgentHardwareQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                                  ulRet;
    TAF_UINT16                                  usLength;
    VOS_UINT8                                   ucIndex;
    DRV_AGENT_MSG_STRU                         *pRcvMsg;
    DRV_AGENT_HARDWARE_QRY_CNF_STRU            *pHardWareCnf;

    /* 初始化 */
    pRcvMsg      = (DRV_AGENT_MSG_STRU*)pMsg;
    pHardWareCnf = (DRV_AGENT_HARDWARE_QRY_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pHardWareCnf->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentHardwareQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentHardwareQryRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if (AT_CMD_DRV_AGENT_HARDWARE_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    usLength               = 0;
    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if ( DRV_AGENT_NO_ERROR == pHardWareCnf->ulResult )
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s:",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "\"%s\"",
                                           pHardWareCnf->aucHwVer);

        gstAtSendData.usBufLen = usLength;
        ulRet                  = AT_OK;
    }
    else
    {
        ulRet                  = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulRet);

    return VOS_OK;

}


VOS_UINT32 AT_RcvDrvAgentVertimeQryRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_VERSION_TIME_STRU        *pstDrvAgentVersionTime;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg                     = (DRV_AGENT_MSG_STRU *)pMsg;
    pstDrvAgentVersionTime      = (DRV_AGENT_VERSION_TIME_STRU *)(pRcvMsg->aucContent);
    ulResult                    = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstDrvAgentVersionTime->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentVertimeQryRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentVertimeQryRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_VERSIONTIME_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^VERTIME命令返回 */
    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    (TAF_CHAR*)pgucAtSndCodeAddr,
                                                    "%s:%s",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    pstDrvAgentVersionTime->aucData);

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentYjcxSetCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                     *pRcvMsg;
    DRV_AGENT_YJCX_SET_CNF_STRU            *pstYjcxSetCnf;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;
    VOS_UINT16                              usDataLen;

    /* 初始化 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstYjcxSetCnf   = (DRV_AGENT_YJCX_SET_CNF_STRU *)(pRcvMsg->aucContent);
    ulResult        = AT_OK;
    usDataLen       = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstYjcxSetCnf->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentYjcxSetCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentYjcxSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_YJCX_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^YJCX设置命令返回 */
    gstAtSendData.usBufLen = 0;
    if (VOS_OK != pstYjcxSetCnf->ulResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        usDataLen =  (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR*)pgucAtSndCodeAddr,
                                            "%s: ",
                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                            "%s",
                                            pstYjcxSetCnf->aucflashInfo);
        gstAtSendData.usBufLen = usDataLen;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentYjcxQryCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                     *pRcvMsg;
    DRV_AGENT_YJCX_QRY_CNF_STRU            *pstYjcxQryCnf;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;
    VOS_UINT16                              usDataLen;

    /* 初始化 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstYjcxQryCnf   = (DRV_AGENT_YJCX_QRY_CNF_STRU *)(pRcvMsg->aucContent);
    ulResult        = AT_OK;
    usDataLen       = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstYjcxQryCnf->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentYjcxQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentYjcxQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_YJCX_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^YJCX查询命令返回 */
    gstAtSendData.usBufLen = 0;
    if (VOS_OK != pstYjcxQryCnf->ulResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        usDataLen =  (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR*)pgucAtSndCodeAddr,
                                            "%s: ",
                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                            "%s",
                                            pstYjcxQryCnf->aucgpioInfo);

        gstAtSendData.usBufLen = usDataLen;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_VOID At_QryEonsUcs2RspProc(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucOpId,
    VOS_UINT32                          ulResult,
    TAF_MMA_EONS_UCS2_PLMN_NAME_STRU   *stEonsUcs2PlmnName,
    TAF_MMA_EONS_UCS2_HNB_NAME_STRU    *pstEonsUcs2HNBName
)
{
    TAF_MMA_EONS_UCS2_PLMN_NAME_STRU   *pstPlmnName = VOS_NULL_PTR;
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulLoop;

#if (FEATURE_ON == FEATURE_CSG)
    TAF_MMA_EONS_UCS2_HNB_NAME_STRU   *pstHnbName = VOS_NULL_PTR;
    VOS_UINT8                          ucHomeNodeBLen;

    pstHnbName = (TAF_MMA_EONS_UCS2_HNB_NAME_STRU *)pstEonsUcs2HNBName;

    usLength = 0;

    if (VOS_NULL_PTR != pstHnbName)
    {
        if (0 != pstHnbName->ucHomeNodeBNameLen)
        {
            usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              "%s:",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

            /* 打印home NodeB Name, ucs2编码，最大长度48字节 */
            ucHomeNodeBLen = AT_MIN(pstHnbName->ucHomeNodeBNameLen, TAF_MMA_MAX_HOME_NODEB_NAME_LEN);

            for (ulLoop = 0; ulLoop < ucHomeNodeBLen; ulLoop++)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%02X",
                                                   pstHnbName->aucHomeNodeBName[ulLoop]);
            }
            gstAtSendData.usBufLen = usLength;

            At_FormatResultData(ucIndex,ulResult);

            return;
        }
    }

#endif

    /* 变量初始化 */
    pstPlmnName = (TAF_MMA_EONS_UCS2_PLMN_NAME_STRU *)stEonsUcs2PlmnName;

    if (VOS_NULL_PTR == pstPlmnName)
    {
        return;
    }

    /* 转换LongName及ShortName */
    if ( pstPlmnName->ucLongNameLen <= TAF_PH_OPER_NAME_LONG
      && pstPlmnName->ucShortNameLen <= TAF_PH_OPER_NAME_SHORT )
    {

        /* ^EONSUCS2:<long name>,<short name> */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          "%s:",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        for (ulLoop = 0; ulLoop < pstPlmnName->ucLongNameLen; ulLoop++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%02X",
                                               pstPlmnName->aucLongName[ulLoop]);
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr + usLength, ",");

        for (ulLoop = 0; ulLoop < pstPlmnName->ucShortNameLen; ulLoop++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%02X",
                                               pstPlmnName->aucShortName[ulLoop]);
        }

        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
    }

    At_FormatResultData(ucIndex,ulResult);

    return;
}


VOS_UINT32 AT_RcvMmaEonsUcs2Cnf(VOS_VOID *pMsg)
{
    TAF_MMA_EONS_UCS2_CNF_STRU         *pstEonsUcs2QryCnfMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstEonsUcs2QryCnfMsg    = (TAF_MMA_EONS_UCS2_CNF_STRU *)pMsg;
    ulResult                = AT_OK;

    if (VOS_NULL_PTR == pstEonsUcs2QryCnfMsg)
    {
        AT_WARN_LOG("AT_RcvMmaCrpnQueryRsp:MSG IS NULL!");
        return VOS_ERR;
    }

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEonsUcs2QryCnfMsg->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCrpnQueryRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaEonsUcs2Cnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_EONSUCS2_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^EONSUCS2查询命令返回 */
    if (TAF_ERR_NO_ERROR != pstEonsUcs2QryCnfMsg->enRslt)
    {
        ulResult = At_ChgTafErrorCode(ucIndex, (VOS_UINT16)pstEonsUcs2QryCnfMsg->enErrorCause);

        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex,ulResult);
        return VOS_OK;
    }
    At_QryEonsUcs2RspProc(ucIndex,
                    pstEonsUcs2QryCnfMsg->ucOpId,
                    ulResult,
                    &pstEonsUcs2QryCnfMsg->stEonsUcs2PlmnName,
                    &pstEonsUcs2QryCnfMsg->stEonsUcs2HNBName);

    return VOS_OK;
}


VOS_UINT32 AT_RcvAtMmaUsimStatusInd(VOS_VOID *pMsg)
{
    AT_MMA_USIM_STATUS_IND_STRU        *pstAtMmaUsimStatusIndMsg;
    MODEM_ID_ENUM_UINT16                enModemId;
    AT_USIM_INFO_CTX_STRU              *pstUsimInfoCtx = VOS_NULL_PTR;

    /* 初始化 */
    pstAtMmaUsimStatusIndMsg   = (AT_MMA_USIM_STATUS_IND_STRU *)pMsg;

    enModemId = AT_GetModemIDFromPid(pstAtMmaUsimStatusIndMsg->ulSenderPid);

    if (enModemId >= MODEM_ID_BUTT)
    {
        AT_PR_LOGE("enModemId :%d , ulSenderPid :%d", enModemId, pstAtMmaUsimStatusIndMsg->ulSenderPid);

        return VOS_ERR;
    }

    pstUsimInfoCtx = AT_GetUsimInfoCtxFromModemId(enModemId);

    /* 刷新卡状态全局变量 */
    pstUsimInfoCtx->enCardType       = pstAtMmaUsimStatusIndMsg->enCardType;
    pstUsimInfoCtx->enCardMediumType = pstAtMmaUsimStatusIndMsg->enCardMediumType;
    pstUsimInfoCtx->enCardStatus     = pstAtMmaUsimStatusIndMsg->enCardStatus;
    pstUsimInfoCtx->ucIMSILen        = pstAtMmaUsimStatusIndMsg->ucIMSILen;
    TAF_MEM_CPY_S(pstUsimInfoCtx->aucIMSI, sizeof(pstUsimInfoCtx->aucIMSI), pstAtMmaUsimStatusIndMsg->aucIMSI, NAS_MAX_IMSI_LENGTH);

    AT_PR_LOGI("CardType: %d , CardStatus: %d , ulSenderPid: %d",
           pstAtMmaUsimStatusIndMsg->enCardType, pstAtMmaUsimStatusIndMsg->enCardStatus, pstAtMmaUsimStatusIndMsg->ulSenderPid);

    return VOS_OK;

}


VOS_UINT32 At_RcvAtCcMsgStateQryCnfProc(VOS_VOID *pMsg)
{
    AT_CC_STATE_QRY_CNF_MSG_STRU           *pstAtCcStateQryCnfMsg;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              i;
    VOS_UINT16                              usLength;

    /* 初始化 */
    pstAtCcStateQryCnfMsg = (AT_CC_STATE_QRY_CNF_MSG_STRU *)pMsg;
    ulResult              = AT_OK;
    usLength              = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstAtCcStateQryCnfMsg->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("At_RcvAtCcMsgStateQryCnfProc:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvAtCcMsgStateQryCnfProc : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前未等待该命令回复 */
    if (AT_CMD_CC_STATE_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CCC查询命令返回 */
    gstAtSendData.usBufLen = 0;
    if (0 == pstAtCcStateQryCnfMsg->ucCallNum)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
        for ( i = 0 ; i < pstAtCcStateQryCnfMsg->ucCallNum ; i++ )
        {
            AT_ShowCccRst(&pstAtCcStateQryCnfMsg->astCcStateInfoList[i],&usLength);
        }

        if (0 == usLength)
        {
            ulResult = AT_ERROR;
        }
    }

    /* 输出结果 */
    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCmmSetCmdRsp(VOS_VOID *pMsg)
{
    TAF_MMA_CMM_SET_CNF_STRU           *pstMnMmTestCmdRspMsg;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          i;
    VOS_UINT16                          usLength;

    /* 初始化 */
    pstMnMmTestCmdRspMsg = (TAF_MMA_CMM_SET_CNF_STRU *)pMsg;
    ulResult              = AT_OK;
    usLength              = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMnMmTestCmdRspMsg->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCmmSetCmdRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCmmSetCmdRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CMM查询命令返回 */
    gstAtSendData.usBufLen = 0;
    if (VOS_OK != pstMnMmTestCmdRspMsg->ulResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        if (pstMnMmTestCmdRspMsg->stAtCmdRslt.ulRsltNum > 0)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR*)pgucAtSndCodeAddr+usLength,
                                               "%d",
                                               pstMnMmTestCmdRspMsg->stAtCmdRslt.aulRslt[0]);

            for (i = 1; i< pstMnMmTestCmdRspMsg->stAtCmdRslt.ulRsltNum ;i++)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR*)pgucAtSndCodeAddr+usLength,
                                                   ",%d",
                                                   pstMnMmTestCmdRspMsg->stAtCmdRslt.aulRslt[i]);
            }
            gstAtSendData.usBufLen = usLength;
        }
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentFullHardwareQryRsp(VOS_VOID *pMsg)
{
    TAF_UINT16                                       usLength;
    VOS_UINT8                                        ucIndex;
    DRV_AGENT_MSG_STRU                              *pRcvMsg;
    DRV_AGENT_FULL_HARDWARE_QRY_CNF_STRU            *pstEvent;

    /* 初始化 */
    pRcvMsg  = (DRV_AGENT_MSG_STRU*)pMsg;
    pstEvent = (DRV_AGENT_FULL_HARDWARE_QRY_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentFullHardwareQryRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentFullHardwareQryRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if (AT_CMD_DRV_AGENT_FULL_HARDWARE_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    usLength               = 0;
    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if ( DRV_AGENT_NO_ERROR == pstEvent->ulResult )
    {

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s:",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "\"%s ",
                                           pstEvent->aucModelId);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s,",
                                           pstEvent->aucRevisionId);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s\"",
                                           pstEvent->aucHwVer);

        gstAtSendData.usBufLen = usLength;

        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentSetSimlockCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                              *pRcvMsg;
    DRV_AGENT_SET_SIMLOCK_CNF_STRU                  *pstEvent;
    VOS_UINT32                                       ulRet;
    VOS_UINT8                                        ucIndex;

    /* 初始化 */
    pRcvMsg  = (DRV_AGENT_MSG_STRU*)pMsg;
    pstEvent = (DRV_AGENT_SET_SIMLOCK_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetSimlockCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetSimlockCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if (AT_CMD_DRV_AGENT_SIMLOCK_SET_REQ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if ( DRV_AGENT_NO_ERROR == pstEvent->ulResult )
    {
        ulRet = AT_OK;
    }
    else
    {
        ulRet = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulRet);

    return VOS_OK;

}


VOS_UINT32 AT_RcvDrvAgentQryRxdivCnf(VOS_VOID *pMsg)
{
    VOS_UINT32                                       ulRet;
    VOS_UINT8                                        ucIndex;
    DRV_AGENT_MSG_STRU                              *pRcvMsg;
    DRV_AGENT_QRY_RXDIV_CNF_STRU                    *pstEvent;
    VOS_UINT32                                       ulUserDivBandsLow;
    VOS_UINT32                                       ulUserDivBandsHigh;
    VOS_UINT32                                       ulDrvDivBandsLow;
    VOS_UINT32                                       ulDrvDivBandsHigh;
    VOS_UINT16                                       usLen;

    /* 初始化 */
    pRcvMsg  = (DRV_AGENT_MSG_STRU*)pMsg;
    pstEvent = (DRV_AGENT_QRY_RXDIV_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryRxdivCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryRxdivCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if (AT_CMD_DRV_AGENT_RXDIV_QRY_REQ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    usLen                  = 0;
    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if ( DRV_AGENT_NO_ERROR == pstEvent->ulResult )
    {
        At_CovertMsInternalRxDivParaToUserSet(pstEvent->usDrvDivBands,
                                             &ulDrvDivBandsLow,
                                             &ulDrvDivBandsHigh);
        At_CovertMsInternalRxDivParaToUserSet(pstEvent->usCurBandSwitch,
                                             &ulUserDivBandsLow,
                                             &ulUserDivBandsHigh);

        usLen +=  (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (TAF_CHAR *)pgucAtSndCodeAddr,
                                         (TAF_CHAR*)pgucAtSndCodeAddr,
                                         "%s:",
                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        if (0 != ulDrvDivBandsHigh)
        {
            usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)(pgucAtSndCodeAddr + usLen),
                                            "%d,%X%08X,",
                                            0,
                                            ulDrvDivBandsHigh,ulDrvDivBandsLow);
        }
        else
        {
            usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)(pgucAtSndCodeAddr + usLen),
                                            "%d,%X,",
                                            0,
                                            ulDrvDivBandsLow);
        }

        if (0 != ulUserDivBandsHigh)
        {
            usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)(pgucAtSndCodeAddr + usLen),
                                            "%X%08X",
                                            ulUserDivBandsHigh,
                                            ulUserDivBandsLow);
        }
        else
        {
            usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)(pgucAtSndCodeAddr + usLen),
                                            "%X",
                                            ulUserDivBandsLow);
        }
        usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                        (TAF_CHAR *)(pgucAtSndCodeAddr + usLen),
                                        "\r\n");
        usLen +=  (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (TAF_CHAR *)pgucAtSndCodeAddr,
                                         (TAF_CHAR *)(pgucAtSndCodeAddr + usLen),
                                         "%s:",
                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                        (TAF_CHAR *)(pgucAtSndCodeAddr + usLen),
                                        "%d,%X,%X",
                                        1,
                                        0,
                                        0);
        gstAtSendData.usBufLen = usLen;

        ulRet = AT_OK;
    }
    else
    {
        ulRet = AT_CME_UNKNOWN;
    }

    At_FormatResultData(ucIndex, ulRet);

    return VOS_OK;
}



VOS_UINT32 AT_RcvDrvAgentSetNvRestoreCnf(VOS_VOID *pMsg)
{
    VOS_UINT8                                        ucIndex;
    DRV_AGENT_MSG_STRU                              *pRcvMsg;
    DRV_AGENT_NVRESTORE_RST_STRU                *pstEvent;

    /* 初始化 */
    pRcvMsg                 = (DRV_AGENT_MSG_STRU*)pMsg;
    pstEvent                = (DRV_AGENT_NVRESTORE_RST_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetNvRestoreCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetNvRestoreCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if (AT_CMD_NVRESTORE_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%d",
                                                    pstEvent->ulResult);

    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;

}


VOS_UINT32 AT_RcvDrvAgentQryNvRestoreRstCnf(VOS_VOID *pMsg)
{
    VOS_UINT8                                        ucIndex;
    VOS_UINT32                                       ulRestoreStatus;
    DRV_AGENT_MSG_STRU                              *pRcvMsg;
    DRV_AGENT_NVRESTORE_RST_STRU                    *pstEvent;

    /* 初始化 */
    pRcvMsg                 = (DRV_AGENT_MSG_STRU*)pMsg;
    pstEvent                = (DRV_AGENT_NVRESTORE_RST_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryNvRestoreRstCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryNvRestoreRstCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if (AT_CMD_NVRSTSTTS_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if ((AT_NV_RESTORE_RESULT_INIT == pstEvent->ulResult)
     || (AT_NV_RESTORE_RUNNING == pstEvent->ulResult))
    {
        ulRestoreStatus = AT_NV_RESTORE_FAIL;
    }
    else if (VOS_OK == pstEvent->ulResult)
    {
        ulRestoreStatus = AT_NV_RESTORE_SUCCESS;
    }
    else
    {
        ulRestoreStatus = AT_NV_RESTORE_FAIL;
    }
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%d",
                                                     ulRestoreStatus);

    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;

}


VOS_UINT32 AT_RcvDrvAgentNvRestoreManuDefaultRsp(VOS_VOID *pMsg)
{
    VOS_UINT8                                        ucIndex;
    VOS_UINT32                                       ulRst;
    VOS_UINT32                                       ulResult;
    DRV_AGENT_MSG_STRU                              *pRcvMsg;
    DRV_AGENT_NVRESTORE_RST_STRU                    *pstEvent;

    /* 初始化 */
    pRcvMsg                 = (DRV_AGENT_MSG_STRU*)pMsg;
    pstEvent                = (DRV_AGENT_NVRESTORE_RST_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentNvRestoreManuDefaultRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentNvRestoreManuDefaultRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if (AT_CMD_F_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    ulResult = pstEvent->ulResult;
    if ( NV_OK != ulResult )
    {
        ulRst = AT_ERROR;
    }
    else
    {
        g_bSetFlg = VOS_TRUE;

        /* E5通知APP恢复用户设置  */
        AT_PhSendRestoreFactParm();

        ulRst = AT_OK;
    }
    At_FormatResultData(ucIndex, ulRst);

    return VOS_OK;

}



VOS_UINT32  AT_GetImeiValue(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           aucImei[TAF_PH_IMEI_LEN + 1]
)
{
    NV_SC_PERS_CTRL_STRU                    stScPersCtrl;
    VOS_UINT8                               aucBuf[TAF_PH_IMEI_LEN + 1];
    VOS_UINT8                               ucCheckData;
    VOS_UINT32                              ulDataLen;
    VOS_UINT32                              i;

    ucCheckData = 0;
    ulDataLen   = TAF_PH_IMEI_LEN;


    TAF_MEM_SET_S(aucBuf, sizeof(aucBuf), 0x00, sizeof(aucBuf));

    TAF_MEM_SET_S(&stScPersCtrl, sizeof(stScPersCtrl), 0x00, sizeof(NV_SC_PERS_CTRL_STRU));

    if (NV_OK != TAF_ACORE_NV_READ(enModemId, en_NV_Item_IMEI, aucBuf, ulDataLen))
    {
        AT_WARN_LOG("TAF_GetImeiValue:Read IMEI Failed!");
        return VOS_ERR;
    }

#if (3 == MULTI_MODEM_NUMBER)
#if (VOS_WIN32 != VOS_OS_VER)
    if (NV_OK != TAF_ACORE_NV_READ(enModemId, en_NV_Item_NV_SC_PERS_CTRL_CFG, &stScPersCtrl, sizeof(stScPersCtrl)))
    {
        AT_WARN_LOG("TAF_GetImeiValue:Read en_NV_Item_NV_SC_PERS_CTRL_CFG Failed!");
        return VOS_ERR;
    }

    /* NV 4008读取值为0x5a5a表示生效，其他值表示不生效 */
    if ((MODEM_ID_2 == enModemId)
     && (0x5A5A == stScPersCtrl.usImei0ReplaseImei2))
    {
        /* IMEI all zero */
        for (i = 0; i < ulDataLen; i++)
        {
            if (0 != aucBuf[i])
            {
                break;
            }
        }

        if (i >= ulDataLen)
        {
            TAF_MEM_SET_S(aucBuf, sizeof(aucBuf), 0x00, sizeof(aucBuf));

            if (NV_OK != TAF_ACORE_NV_READ(MODEM_ID_0, en_NV_Item_IMEI, aucBuf, ulDataLen))
            {
                AT_WARN_LOG("TAF_GetImeiValue:Read IMEI Failed!");
                return VOS_ERR;
            }
        }
    }
#endif
#endif

    ucCheckData = 0;
    for (i = 0; i < (TAF_PH_IMEI_LEN-2); i += 2)
    {
        ucCheckData += aucBuf[i]
                      +((aucBuf[i+1UL] + aucBuf[i+1UL])/10)
                      +((aucBuf[i+1UL] + aucBuf[i+1UL])%10);
    }
    ucCheckData = (10 - (ucCheckData%10))%10;
    AT_DeciDigit2Ascii(aucBuf, (VOS_UINT8)ulDataLen, aucImei);
    aucImei[TAF_PH_IMEI_LEN-2] = ucCheckData+0x30;
    aucImei[TAF_PH_IMEI_LEN-1] = 0;

    return VOS_OK;
}




VOS_UINT32 AT_RcvDrvAgentSetGpioplRsp(VOS_VOID *pMsg)
{

    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_GPIOPL_SET_CNF_STRU      *pstDrvAgentGpioSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pRcvMsg               = (DRV_AGENT_MSG_STRU *)pMsg;
    pstDrvAgentGpioSetCnf = (DRV_AGENT_GPIOPL_SET_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstDrvAgentGpioSetCnf->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentGpioplRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentGpioplRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if (AT_CMD_GPIOPL_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_TRUE == pstDrvAgentGpioSetCnf->bFail)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
    }

    /* 调用AT_FormATResultDATa发送命令结果 */
    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}




VOS_UINT32 AT_RcvDrvAgentQryGpioplRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                   *pRcvMsg;
    DRV_AGENT_GPIOPL_QRY_CNF_STRU        *pstGpioQryCnf;
    VOS_UINT16                            usLength;
    VOS_UINT32                            i;
    VOS_UINT8                             ucIndex;
    VOS_UINT32                            ulResult;

    /* 初始化消息，获取ucContent */
    pRcvMsg               = (DRV_AGENT_MSG_STRU *)pMsg;
    pstGpioQryCnf         = (DRV_AGENT_GPIOPL_QRY_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstGpioQryCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryGpioplRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryGpioplRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_GPIOPL_QRY */
    if (AT_CMD_GPIOPL_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /*复位AT状态*/
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_TRUE != pstGpioQryCnf->bFail)
    {
        /* 打印结果 */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                          "%s:", g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        for (i = 0; i < DRVAGENT_GPIOPL_MAX_LEN; i++)
        {
           usLength += (VOS_UINT16)At_sprintf (AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               "%02X", pstGpioQryCnf->aucGpiopl[i]);
        }

        gstAtSendData.usBufLen = usLength;
        ulResult               = AT_OK;
    }
    else
    {
        /* 调用AT_FormATResultDATa发送命令结果 */
        gstAtSendData.usBufLen = 0;
        ulResult               = AT_ERROR;
    }
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}



VOS_UINT32 AT_RcvDrvAgentSetDatalockRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_DATALOCK_SET_CNF_STRU    *pstDatalockSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pRcvMsg                   = (DRV_AGENT_MSG_STRU *)pMsg;
    pstDatalockSetCnf         = (DRV_AGENT_DATALOCK_SET_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstDatalockSetCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetDatalockRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetDatalockRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_DATALOCK_SET */
    if (AT_CMD_DATALOCK_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_TRUE != pstDatalockSetCnf->bFail)
    {
        /* 设置dATalock已解锁 */
        g_bAtDataLocked    =  VOS_FALSE;
        ulResult           =  AT_OK;
    }
    else
    {
        ulResult           =  AT_ERROR;
    }

    /* 调用AT_FormAtResultData发送命令结果  */
    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
 }


VOS_UINT32 AT_GetSimLockStatus(VOS_UINT8 ucIndex)
{

    /* 发消息到C核获取SIMLOCK 状态信息 */
    if(TAF_SUCCESS != Taf_ParaQuery(gastAtClientTab[ucIndex].usClientId, 0,
                                    TAF_PH_SIMLOCK_VALUE_PARA, VOS_NULL_PTR))
    {
        AT_WARN_LOG("AT_GetSimLockStatus: Taf_ParaQuery fail.");
        return VOS_ERR;
    }

    /* ^SIMLOCK=2查询UE的锁卡状态不在AT命令处理的主流程，需要本地启动保护定时器并更新端口状态 */
    if (AT_SUCCESS != At_StartTimer(AT_SET_PARA_TIME, ucIndex))
    {
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_ERR;
    }

    /* 设置AT模块实体的状态为等待异步返回 */
    gastAtClientTab[ucIndex].CmdCurrentOpt   = AT_CMD_SIMLOCKSTATUS_READ;

    g_stParseContext[ucIndex].ucClientStatus = AT_FW_CLIENT_STATUS_PEND;

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentQryTbatvoltRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_TBATVOLT_QRY_CNF_STRU    *pstTbatvoltQryCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pRcvMsg                   = (DRV_AGENT_MSG_STRU *)pMsg;
    pstTbatvoltQryCnf         = (DRV_AGENT_TBATVOLT_QRY_CNF_STRU *)pRcvMsg->aucContent;


    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstTbatvoltQryCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryTbatvoltRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryTbatvoltRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_TBATVOLT_QRY*/
    if (AT_CMD_TBATVOLT_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 电池电压查询结果判断 */
    if (VOS_TRUE != pstTbatvoltQryCnf->bFail)
    {

        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        "%s:%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstTbatvoltQryCnf->lBatVol);
        ulResult = AT_OK;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
        ulResult = AT_ERROR;
    }

    /* 调用AT_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_VOID At_RcvVcMsgQryModeCnfProc(MN_AT_IND_EVT_STRU *pstData)
{

    MN_AT_IND_EVT_STRU                  *pRcvMsg;
    APP_VC_EVENT_INFO_STRU              *pstEvent;
    VOS_UINT8                            ucIndex;
    VOS_UINT16                           usVoiceMode;
    VOS_UINT16                           usDevMode;
    VOS_UINT32                           ulRet;


    /* 初始化 */
    pRcvMsg         = pstData;
    pstEvent        = (APP_VC_EVENT_INFO_STRU *)pstData->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgQryModeCnfProc:WARNING:AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgQryModeCnfProc : AT_BROADCAST_INDEX.");
        return;
    }

    /* 格式化VMSET命令返回 */
    if (AT_CMD_VMSET_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        usDevMode = pstEvent->enDevMode;

        /* 无效的模式，直接返回ERROR */
        if (usDevMode >= VC_PHY_DEVICE_MODE_BUTT)
        {
            gstAtSendData.usBufLen = 0;
            ulRet = AT_ERROR;
        }
        else
        {
            gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                                            "%s: %d",
                                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                            usDevMode);
            ulRet = AT_OK;
        }
    }
    /* 格式化CVOICE命令返回 */
    else if ( AT_CMD_CVOICE_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* 转换为应用于VC语音模式 */
        usVoiceMode     = APP_VC_VcPhyVoiceMode2AppVcVoiceMode(pstEvent->enDevMode);

        /* 无效的模式，直接返回ERROR */
        if (usVoiceMode >= APP_VC_VOICE_MODE_BUTT)
        {
            gstAtSendData.usBufLen = 0;
            ulRet = AT_ERROR;
        }
        else
        {
            gstAtSendData.usBufLen =(VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                                           "%s: %d, %d, %d, %d",
                                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                           usVoiceMode,
                                                           AT_PCVOICE_SAMPLE_RATE,
                                                           AT_PCVOICE_DATA_BIT,
                                                           AT_PCVOICE_FRAME_PERIOD);
            ulRet = AT_OK;
        }
    }
    else
    {
        return;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return;
}


VOS_VOID At_RcvVcMsgSetPortCnfProc(MN_AT_IND_EVT_STRU *pstData)
{
    MN_AT_IND_EVT_STRU                 *pRcvMsg;
    APP_VC_EVENT_INFO_STRU             *pstEvent;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;

    /* 初始化 */
    pRcvMsg         = pstData;
    pstEvent        = (APP_VC_EVENT_INFO_STRU *)pstData->aucContent;


    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgSetPortCnfProc:WARNING:AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgSetPortCnfProc : AT_BROADCAST_INDEX.");
        return;
    }

    if (VOS_TRUE == pstEvent->bSuccess)
    {
        ulRet = AT_OK;
    }
    else
    {
        ulRet = AT_ERROR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return;

}


VOS_VOID At_RcvVcMsgQryPortCnfProc(MN_AT_IND_EVT_STRU *pstData)
{
    MN_AT_IND_EVT_STRU                  *pRcvMsg;
    APP_VC_EVENT_INFO_STRU              *pstEvent;
    VOS_UINT8                            ucIndex;
    APP_VC_VOICE_PORT_ENUM_U8            ucVoicePort;                           /* 语音设备端口号 */
    VOS_UINT32                           ulRet;

    /* 初始化 */
    pRcvMsg         = pstData;
    pstEvent        = (APP_VC_EVENT_INFO_STRU *)pstData->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgQryPortCnfProc:WARNING:AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgQryPortCnfProc : AT_BROADCAST_INDEX.");
        return;
    }

    /* 查询的端口号 */
    ucVoicePort = pstEvent->enVoicePort;

    if (APP_VC_VOICE_PORT_BUTT > ucVoicePort)
    {
        gstAtSendData.usBufLen =
            (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   "%s: %d",
                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                   ucVoicePort);
        ulRet = AT_OK;
    }
    else
    {
        ulRet = AT_ERROR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return;

}


VOS_UINT32 AT_RcvRnicDialModeCnf(MsgBlock *pstMsg)
{
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    RNIC_AT_DIAL_MODE_CNF_STRU         *pstRcvMsg;

    usLength                            = 0;
    pstRcvMsg                           = (RNIC_AT_DIAL_MODE_CNF_STRU *)pstMsg;

    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvRnicDialModeCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvRnicDialModeCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    usLength =  (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR*)pgucAtSndCodeAddr,
                                        "%s:",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    usLength += (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%d",
                                        pstRcvMsg->ulDialMode);

    usLength += (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                        ",%d",
                                        pstRcvMsg->ulEventReportFlag);

    usLength += (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                         ",%d",
                                         pstRcvMsg->ulIdleTime);

    gstAtSendData.usBufLen = usLength;

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_VOID At_ZeroReplaceBlankInString( VOS_UINT8 *pData, VOS_UINT32 ulLen)
{
    TAF_UINT32 ulChkLen  = 0;
    TAF_UINT8  *pWrite   = pData;
    TAF_UINT8  *pRead    = pData;

    /* 输入参数检查 */
    while ( ulChkLen < ulLen )
    {
        /* 时间格式 May  5 2011 17:08:00
           转换成   May 05 2011 17:08:00 */
        if (' ' == *pRead++)
        {
            if (' ' == *pRead)
            {
                pWrite++;
                *pWrite = '0';
                pRead++;
                ulChkLen++;
            }
        }
        pWrite++;
        ulChkLen++;
    }

    return;
}



VOS_UINT32 AT_RcvDrvAgentQryVersionRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_VERSION_QRY_CNF_STRU     *pstVersionQryCnfInfo;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulLen;
    VOS_CHAR                            acTmpTime[AT_AGENT_DRV_VERSION_TIME_LEN] = {0};
    VOS_UINT16                          usLength;
    VOS_BOOL                            bPhyNumIsNull;

    VOS_UINT32                          ulOpt;
    VOS_UINT32                          ulRet;
    TAF_NVIM_CS_VER_STRU                stCsver;
    MODEM_ID_ENUM_UINT16                enModemId;

    enModemId = MODEM_ID_0;

    stCsver.usCsver = 0;

    ulRet = TAF_ACORE_NV_READ(MODEM_ID_0, en_NV_Item_Csver, &(stCsver.usCsver), sizeof(stCsver.usCsver));
    if (NV_OK != ulRet)
    {
        return AT_ERROR;
    }

    /* 初始化消息，获取ucContent */
    ulLen                        = 0;
    pRcvMsg                      = (DRV_AGENT_MSG_STRU *)pMsg;
    pstVersionQryCnfInfo         = (DRV_AGENT_VERSION_QRY_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstVersionQryCnfInfo->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryVersionRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryVersionRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_VERSION_QRY/AT_CMD_RSFR_VERSION_QRY, ^RSFR命令也借用此接口 */
    if ((AT_CMD_VERSION_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
        && (AT_CMD_RSFR_VERSION_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        return VOS_ERR;
    }

    ulRet = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRet)
    {
        AT_ERR_LOG("AT_RcvDrvAgentQryVersionRsp: Get modem id fail.");
        return VOS_ERR;
    }

    ulOpt = gastAtClientTab[ucIndex].CmdCurrentOpt;

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /*查询出错 */
    if(DRV_AGENT_VERSION_QRY_NO_ERROR != pstVersionQryCnfInfo->enResult)
    {
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_OK;
    }

    /*版本编译时间格式转换，将时间戳中连续两个空格的后一个空格用0替换 */
    ulLen = VOS_StrLen(pstVersionQryCnfInfo->acVerTime);
    TAF_MEM_CPY_S(acTmpTime, sizeof(acTmpTime), pstVersionQryCnfInfo->acVerTime, ulLen + 1);
    At_ZeroReplaceBlankInString((VOS_UINT8 *)acTmpTime, ulLen);

    /* 字符串预解析 */
    usLength = TAF_CDROM_VERSION_LEN;
    At_DelCtlAndBlankCharWithEndPadding(pstVersionQryCnfInfo->stIsoVer.aucIsoInfo, &usLength);

    /* 获取物理号是否为空的标识 */
    if (AT_OK != AT_PhyNumIsNull(enModemId, AT_PHYNUM_TYPE_IMEI, &bPhyNumIsNull))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryVersionRsp(): AT_PhyNumIsNull Error!");
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_OK;
    }

    usLength = 0;

    /* 内部版本号在单板未写入物理号前，查询需如实显示；写入物理号后，如查询版本号前未
       输入解锁指令，内部版本号显示为空，如果已经输入解锁指令，内部版本号如实显示 */
    if (VOS_FALSE == g_bAtDataLocked || (VOS_TRUE == bPhyNumIsNull))
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:BDT:%s%s",
                                           "^VERSION", acTmpTime, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:EXTS:%s%s",
                                           "^VERSION", pstVersionQryCnfInfo->stSoftVersion.aucRevisionId, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:INTS:%s%s",
                                           "^VERSION", pstVersionQryCnfInfo->stSoftVersion.aucRevisionId, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:EXTD:%s%s",
                                           "^VERSION", pstVersionQryCnfInfo->stIsoVer.aucIsoInfo, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:INTD:%s%s",
                                           "^VERSION", pstVersionQryCnfInfo->stIsoVer.aucIsoInfo,gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:EXTH:%s%s",
                                           "^VERSION", pstVersionQryCnfInfo->stInterHwVer.aucHwVer, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:INTH:%s%s",
                                           "^VERSION", pstVersionQryCnfInfo->stFullHwVer.aucHwVer, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:EXTU:%s%s",
                                           "^VERSION",pstVersionQryCnfInfo->stModelId.aucModelId, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:INTU:%s%s",
                                           "^VERSION",pstVersionQryCnfInfo->stInterModelId.aucModelId, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:CFG:%d%s",
                                           "^VERSION",stCsver.usCsver, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:PRL:",
                                           "^VERSION");

        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:BDT:%s%s",
                                           "^VERSION", acTmpTime, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:EXTS:%s%s",
                                           "^VERSION", pstVersionQryCnfInfo->stSoftVersion.aucRevisionId, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:INTS:%s",
                                           "^VERSION", gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:EXTD:%s%s",
                                           "^VERSION", pstVersionQryCnfInfo->stIsoVer.aucIsoInfo, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:INTD:%s",
                                           "^VERSION", gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:EXTH:%s%s",
                                           "^VERSION", pstVersionQryCnfInfo->stInterHwVer.aucHwVer, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:INTH:%s",
                                           "^VERSION", gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:EXTU:%s%s",
                                           "^VERSION",pstVersionQryCnfInfo->stModelId.aucModelId, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:INTU:%s",
                                           "^VERSION", gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:CFG:%d%s",
                                           "^VERSION",stCsver.usCsver, gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "%s:PRL:",
                                           "^VERSION");

        gstAtSendData.usBufLen = usLength;
    }

    /* 如果是^RSFR命令发起的查询VERSION信息的请求,则由^RSFR命令去处理 */
    if(AT_CMD_RSFR_VERSION_QRY == ulOpt)
    {
        AT_SetRsfrVersionCnf(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

        /* AT返回的字符串在AT_SetRsfrVersionCnf中处理 */
        return VOS_OK;
    }

    At_FormatResultData(ucIndex, AT_OK);
    return VOS_OK;

}


VOS_UINT32 AT_GetRxpriErr(DRV_AGENT_ERROR_ENUM_UINT32 enResult)
{
    if(DRV_AGENT_CME_RX_DIV_OTHER_ERR == enResult)
    {
        return AT_CME_RX_DIV_OTHER_ERR;
    }

    if(DRV_AGENT_CME_RX_DIV_NOT_SUPPORTED == enResult)
    {
        return AT_CME_RX_DIV_NOT_SUPPORTED;
    }

    return AT_OK;
}


VOS_VOID AT_SupportHsdpa(
    AT_NVIM_UE_CAPA_STRU               *pstUECapa,
    VOS_BOOL                           *pbSupportHsdpa
    )
{

    /* 初始化为不支持DPA */
    *pbSupportHsdpa = VOS_FALSE;

    if (pstUECapa->enAsRelIndicator >= 2)
    {
        if (1 == pstUECapa->ulHspaStatus)
        {
            if (PS_TRUE == pstUECapa->enHSDSCHSupport)
            {
                *pbSupportHsdpa = VOS_TRUE;
            }
        }
        else
        {
            *pbSupportHsdpa = VOS_TRUE;
        }
    }
}



VOS_VOID AT_SupportHsupa(
    AT_NVIM_UE_CAPA_STRU               *pstUECapa,
    VOS_BOOL                           *pbSupportHsupa
)
{

    /* 初始化为不支持UPA */
    *pbSupportHsupa = VOS_FALSE;

    if (pstUECapa->enAsRelIndicator >= 3)
    {
       if (1 == pstUECapa->ulHspaStatus)
       {
           if (PS_TRUE == pstUECapa->enEDCHSupport)
           {
               *pbSupportHsupa = VOS_TRUE;
           }
       }
       else
       {
            *pbSupportHsupa = VOS_TRUE;
       }
    }
}


VOS_UINT32 AT_GetWFeatureInfo(
    AT_FEATURE_SUPPORT_ST              *pstFeATure,
    DRV_AGENT_SFEATURE_QRY_CNF_STRU    *pstAtAgentSfeatureQryCnf
)
{
    AT_NVIM_UE_CAPA_STRU                stUECapa;
    VOS_UINT32                          ulLen;
    VOS_BOOL                            bSupportHsdpa;
    VOS_BOOL                            bSupportHsupa;
    VOS_UINT8                           aucStrTmp[50] = {0};
    VOS_UINT8                           aucStrDiv[AT_SFEATURE_DIV_STRING_LEN];
    VOS_UINT32                          ulDivLen;


    bSupportHsdpa = VOS_FALSE;
    bSupportHsupa = VOS_FALSE;

    TAF_MEM_SET_S(&stUECapa, sizeof(stUECapa), 0x00, sizeof(stUECapa));


    /* 读取HSPA,HSPA+支持能力 */
    if (NV_OK != TAF_ACORE_NV_READ(MODEM_ID_0,
                                   en_NV_Item_WAS_RadioAccess_Capa_New,
                                   &stUECapa,
                                   sizeof(AT_NVIM_UE_CAPA_STRU)))
    {
        AT_WARN_LOG("AT_GetWFeatureInfo: en_NV_Item_WAS_RadioAccess_Capa NV Read  Fail!");
        return VOS_ERR;
    }

    /* 获取Wcdma频段信息 */
    /* !!!如果增加频段，aucStrTmp 长度需要增加!!! */
    ulLen = (VOS_UINT32)AT_GetWcdmaBandStr(aucStrTmp,&(pstAtAgentSfeatureQryCnf->stBandFeature));

    /* HSPA+是否支持 */
    if ((stUECapa.enAsRelIndicator >= 4)
     && (PS_TRUE == stUECapa.enMacEhsSupport)
     && (stUECapa.ucHSDSCHPhyCategory >= 10)
     && (0 != stUECapa.ucHSDSCHPhyCategoryExt)
     && (PS_TRUE == stUECapa.enHSDSCHSupport))
     {
        pstFeATure[AT_FEATURE_HSPAPLUS].ucFeatureFlag = AT_FEATURE_EXIST;
        TAF_MEM_CPY_S(pstFeATure[AT_FEATURE_HSPAPLUS].aucContent, sizeof(pstFeATure[AT_FEATURE_HSPAPLUS].aucContent), aucStrTmp, ulLen);
     }

    /* DPA是否支持 */
    AT_SupportHsdpa(&stUECapa, &bSupportHsdpa);
    if (VOS_TRUE == bSupportHsdpa)
    {
        pstFeATure[AT_FEATURE_HSDPA].ucFeatureFlag = AT_FEATURE_EXIST;
        TAF_MEM_CPY_S(pstFeATure[AT_FEATURE_HSDPA].aucContent, sizeof(pstFeATure[AT_FEATURE_HSDPA].aucContent), aucStrTmp, ulLen);
    }

    /*UPA是否支持*/
    AT_SupportHsupa(&stUECapa, &bSupportHsupa);
    if(VOS_TRUE == bSupportHsupa)
    {
        pstFeATure[AT_FEATURE_HSUPA].ucFeatureFlag = AT_FEATURE_EXIST;
        TAF_MEM_CPY_S(pstFeATure[AT_FEATURE_HSUPA].aucContent, sizeof(pstFeATure[AT_FEATURE_HSUPA].aucContent), aucStrTmp, ulLen);
    }


    /* 分集信息 */
    pstFeATure[AT_FEATURE_DIVERSITY].ucFeatureFlag = AT_FEATURE_EXIST;
    /* UMTS的分集信息输出*/
    TAF_MEM_SET_S(aucStrDiv, sizeof(aucStrDiv), 0x00, sizeof(aucStrDiv));

    /* !!!如果增加频段，aucStrDiv 长度需要增加!!! */
    ulDivLen = (VOS_UINT32)AT_GetWcdmaDivBandStr(aucStrDiv);

    TAF_MEM_CPY_S(pstFeATure[AT_FEATURE_DIVERSITY].aucContent, sizeof(pstFeATure[AT_FEATURE_DIVERSITY].aucContent), aucStrDiv, ulDivLen);

    /* UMTS */
    pstFeATure[AT_FEATURE_UMTS].ucFeatureFlag = AT_FEATURE_EXIST;
    TAF_MEM_CPY_S(pstFeATure[AT_FEATURE_UMTS].aucContent, sizeof(pstFeATure[AT_FEATURE_UMTS].aucContent), aucStrTmp, ulLen);

    return VOS_OK;

}



VOS_VOID AT_GetGFeatureInfo(
    AT_FEATURE_SUPPORT_ST              *pstFeATure,
    DRV_AGENT_SFEATURE_QRY_CNF_STRU    *pstATAgentSfeatureQryCnf
)
{
    VOS_UINT32                          ulLen;

    VOS_UINT16                          usEgprsFlag;
    VOS_UINT8                           aucStrTmp[50] = {0};


    usEgprsFlag = 0;


    /* 获取GSM频段信息 */
    /* !!!如果增加频段，aucStrDiv 长度需要增加!!! */
    ulLen = (VOS_UINT32)AT_GetGsmBandStr(aucStrTmp,&(pstATAgentSfeatureQryCnf->stBandFeature));

    if ( 0 == ulLen )
    {
        PS_PRINTF_WARNING("<AT_GetGFeatureInfo> Not Support G.\n");
        return;
    }

    /* 读取EDGE支持能力 */
    if (NV_OK != TAF_ACORE_NV_READ(MODEM_ID_0,
                                   en_NV_Item_Egprs_Flag,
                                   &usEgprsFlag,
                                   sizeof(VOS_UINT16)))
    {
        AT_WARN_LOG("AT_GetGFeatureInfo: en_NV_Item_Egprs_Flag NV Read  Fail!");
        pstFeATure[AT_FEATURE_EDGE].ucFeatureFlag = AT_FEATURE_EXIST;
        TAF_MEM_CPY_S(pstFeATure[AT_FEATURE_EDGE].aucContent, sizeof(pstFeATure[AT_FEATURE_EDGE].aucContent), aucStrTmp, ulLen);
    }
    else
    {
        if (usEgprsFlag != 0)
        {
            pstFeATure[AT_FEATURE_EDGE].ucFeatureFlag = AT_FEATURE_EXIST;
            TAF_MEM_CPY_S(pstFeATure[AT_FEATURE_EDGE].aucContent, sizeof(pstFeATure[AT_FEATURE_EDGE].aucContent), aucStrTmp, ulLen);
        }
    }

    /* GPRS */
    pstFeATure[AT_FEATURE_GPRS].ucFeatureFlag = AT_FEATURE_EXIST;
    TAF_MEM_CPY_S(pstFeATure[AT_FEATURE_GPRS].aucContent, sizeof(pstFeATure[AT_FEATURE_GPRS].aucContent), aucStrTmp, ulLen);

    /* GSM */
    pstFeATure[AT_FEATURE_GSM].ucFeatureFlag = AT_FEATURE_EXIST;
    TAF_MEM_CPY_S(pstFeATure[AT_FEATURE_GSM].aucContent, sizeof(pstFeATure[AT_FEATURE_GSM].aucContent), aucStrTmp, ulLen);
}


#if(FEATURE_ON == FEATURE_UE_MODE_TDS)
VOS_UINT32 AT_GetTdsFeatureInfo(AT_FEATURE_SUPPORT_ST * pstFeATure)
{
    VOS_UINT32 ulRst  = 0;
    NVIM_UE_TDS_SUPPORT_FREQ_BAND_LIST_STRU stTdsFeature;
    VOS_UINT32 i = 0;
    VOS_UINT32 ulLen;


    TAF_MEM_SET_S(&stTdsFeature, sizeof(stTdsFeature), 0x00, sizeof(stTdsFeature));

    ulRst = TAF_ACORE_NV_READ(MODEM_ID_0, en_NV_Item_UTRAN_TDD_FREQ_BAND, &stTdsFeature, sizeof(NVIM_UE_TDS_SUPPORT_FREQ_BAND_LIST_STRU));
    if( ulRst != ERR_MSP_SUCCESS )
    {
        PS_PRINTF_WARNING("Read EN_NV_ID_TDS_SUPPORT_FREQ_BAND failed:0x%x.\n", en_NV_Item_UTRAN_TDD_FREQ_BAND);
        return ERR_MSP_FAILURE;
    }
    if(stTdsFeature.ucBandCnt > NVIM_TDS_MAX_SUPPORT_BANDS_NUM)
    {
        PS_PRINTF_WARNING("Read en_NV_Item_UTRAN_TDD_FREQ_BAND stTdsFeature.ucBandCnt:%d.\n", stTdsFeature.ucBandCnt);
        return ERR_MSP_FAILURE;
    }

    /* 读取NV成功，支持TDS */
    pstFeATure[AT_FEATURE_TDSCDMA].ucFeatureFlag = AT_FEATURE_EXIST;

    ulLen = 0;

    /* BandA:2000Hz, BanE:2300Hz, BandF:1900Hz */
    for(i = 0; i < stTdsFeature.ucBandCnt; i++)
    {
        if(ID_NVIM_TDS_FREQ_BAND_A == stTdsFeature.aucBandNo[i])
        {
            VOS_sprintf_s((VOS_CHAR*)(pstFeATure[AT_FEATURE_TDSCDMA].aucContent + ulLen), AT_FEATURE_CONTENT_LEN_MAX - ulLen, "2000,");
            ulLen = VOS_StrLen((VOS_CHAR *)pstFeATure[AT_FEATURE_TDSCDMA].aucContent);
            continue;
        }

        if(ID_NVIM_TDS_FREQ_BAND_E == stTdsFeature.aucBandNo[i])
        {
            VOS_sprintf_s((VOS_CHAR*)(pstFeATure[AT_FEATURE_TDSCDMA].aucContent + ulLen), AT_FEATURE_CONTENT_LEN_MAX - ulLen, "2300,");
            ulLen = VOS_StrLen((VOS_CHAR *)pstFeATure[AT_FEATURE_TDSCDMA].aucContent);
            continue;
        }

        if(ID_NVIM_TDS_FREQ_BAND_F == stTdsFeature.aucBandNo[i])
        {
            VOS_sprintf_s((VOS_CHAR*)(pstFeATure[AT_FEATURE_TDSCDMA].aucContent + ulLen), AT_FEATURE_CONTENT_LEN_MAX - ulLen, "1900,");
            ulLen = VOS_StrLen((VOS_CHAR *)pstFeATure[AT_FEATURE_TDSCDMA].aucContent);
            continue;
        }
    }

    if(ulLen > 0)
    {
        /* 屏蔽掉最后一个逗号 */
        pstFeATure[AT_FEATURE_TDSCDMA].aucContent[ulLen - 1] = '\0';
    }

    return ERR_MSP_SUCCESS;
}
#endif


VOS_UINT32 AT_RcvDrvAgentQrySfeatureRsp(VOS_VOID *pMsg)
{
    /* 初始化 */
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_SFEATURE_QRY_CNF_STRU    *pstAtAgentSfeatureQryCnf;
    VOS_UINT8                           ucIndex;
    AT_FEATURE_SUPPORT_ST              *pstFeATure;
    VOS_UINT32                          ulLen;
    VOS_UINT32                          ulReult;
    VOS_UINT8                           ucFeATrueNum;
    VOS_UINT8                           ucIndexTmp;
    VOS_UINT8                           aucFeATureName[][AT_FEATURE_NAME_LEN_MAX] =
                                        {
                                            "LTE",
                                            "HSPA+",
                                            "HSDPA",
                                            "HSUPA",
                                            "DIVERSITY",
                                            "UMTS",
                                            "EVDO",
                                            "EDGE",
                                            "GPRS",
                                            "GSM",
                                            "CDMA1X",
                                            "WIMAX",
                                            "WIFI",
                                            "GPS",
                                            "TD",

                                            "NR"
                                        };

    pRcvMsg                      = (DRV_AGENT_MSG_STRU*)pMsg;
    pstAtAgentSfeatureQryCnf     = (DRV_AGENT_SFEATURE_QRY_CNF_STRU*)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstAtAgentSfeatureQryCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQrySfeatureRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQrySfeatureRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_SFEATURE_QRY */
    if (AT_CMD_SFEATURE_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    /*lint -save -e516 */
    pstFeATure = (AT_FEATURE_SUPPORT_ST*)PS_MEM_ALLOC(WUEPS_PID_AT, AT_FEATURE_MAX*sizeof(AT_FEATURE_SUPPORT_ST));
    /*lint -restore */
    if (VOS_NULL == pstFeATure)
    {
        AT_WARN_LOG("AT_RcvDrvAgentQrySfeatureRsp: GET MEM Fail!");
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_OK;
    }
    TAF_MEM_SET_S(pstFeATure, AT_FEATURE_MAX*sizeof(AT_FEATURE_SUPPORT_ST), 0x00, AT_FEATURE_MAX*sizeof(AT_FEATURE_SUPPORT_ST));

    ulReult = AT_GetWFeatureInfo(pstFeATure,pstAtAgentSfeatureQryCnf);
    if (VOS_OK != ulReult)
    {
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_ERROR);
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, pstFeATure);
        /*lint -restore */
        return VOS_OK;
    }

#if(FEATURE_ON == FEATURE_LTE)
    ulReult = AT_GetLteFeatureInfo(pstFeATure);
    if (VOS_OK != ulReult)
    {
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_ERROR);
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, pstFeATure);
        /*lint -restore */
        return VOS_OK;
    }
#endif

#if(FEATURE_UE_MODE_TDS == FEATURE_ON)
    ulReult = AT_GetTdsFeatureInfo(pstFeATure);
    if (VOS_OK != ulReult)
    {
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_ERROR);
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, pstFeATure);
        /*lint -restore */
        return VOS_OK;
    }
#endif

#if(FEATURE_ON == FEATURE_UE_MODE_NR)
    pstFeATure[AT_FEATURE_NR].ucFeatureFlag = AT_FEATURE_EXIST;

    /* 暂时不输出任何信息 */
    pstFeATure[AT_FEATURE_NR].aucContent[0]    = '\0';
#endif

    AT_GetGFeatureInfo(pstFeATure,pstAtAgentSfeatureQryCnf);

    if (BSP_MODULE_SUPPORT == mdrv_misc_support_check(BSP_MODULE_TYPE_WIFI) )
    {
        /* WIFI */
        pstFeATure[AT_FEATURE_WIFI].ucFeatureFlag = AT_FEATURE_EXIST;
        VOS_sprintf_s((VOS_CHAR*)pstFeATure[AT_FEATURE_WIFI].aucContent, sizeof(pstFeATure[AT_FEATURE_WIFI].aucContent), "B,G,N");
    }

    /* 计算支持特性的个数 */
    ucFeATrueNum = 0;
    for (ucIndexTmp = 0; ucIndexTmp < AT_FEATURE_MAX; ucIndexTmp++)
    {
        TAF_MEM_CPY_S(pstFeATure[ucIndexTmp].aucFeatureName, sizeof(pstFeATure[ucIndexTmp].aucFeatureName), aucFeATureName[ucIndexTmp],sizeof(aucFeATureName[ucIndexTmp]));
        if (AT_FEATURE_EXIST == pstFeATure[ucIndexTmp].ucFeatureFlag)
        {
            ucFeATrueNum++;
        }
    }

    ulLen = 0;
    /* 打印输出支持的特性数 */
    ulLen = (TAF_UINT32)At_sprintf(AT_CMD_MAX_LEN,
                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                   (TAF_CHAR *)pgucAtSndCodeAddr + ulLen,
                                   "%s:%d%s",
                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                   ucFeATrueNum,
                                   gaucAtCrLf);

    /* 分行打印输出支持的特性 */
    for (ucIndexTmp = 0; ucIndexTmp < AT_FEATURE_MAX; ucIndexTmp++)
    {
#if(FEATURE_ON == FEATURE_UE_MODE_NR)
        if ((AT_FEATURE_EXIST == pstFeATure[AT_FEATURE_NR].ucFeatureFlag)
         && (AT_FEATURE_NR == ucIndexTmp))
        {
            ulLen += (TAF_UINT32)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + ulLen,
                                           "%s:%s%s",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           pstFeATure[ucIndexTmp].aucFeatureName,
                                           gaucAtCrLf);

            continue;
        }
#endif        
        if (AT_FEATURE_EXIST == pstFeATure[ucIndexTmp].ucFeatureFlag)
        {
            ulLen += (TAF_UINT32)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + ulLen,
                                           "%s:%s,%s%s",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           pstFeATure[ucIndexTmp].aucFeatureName,
                                           pstFeATure[ucIndexTmp].aucContent,
                                           gaucAtCrLf);
        }
    }

    gstAtSendData.usBufLen = (VOS_UINT16)(ulLen - (VOS_UINT32)VOS_StrLen((VOS_CHAR*)gaucAtCrLf));

    At_FormatResultData(ucIndex, AT_OK);
    /*lint -save -e516 */
    PS_MEM_FREE(WUEPS_PID_AT, pstFeATure);
    /*lint -restore */
    return VOS_OK;

}



VOS_UINT32 AT_RcvDrvAgentQryProdtypeRsp(VOS_VOID * pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_PRODTYPE_QRY_CNF_STRU    *stProdTypeCnf;
    VOS_UINT8                           ucIndex;

    /* 初始化消息 */
    pRcvMsg                  = (DRV_AGENT_MSG_STRU *)pMsg;
    stProdTypeCnf            = (DRV_AGENT_PRODTYPE_QRY_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(stProdTypeCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryProdtypeRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryProdtypeRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_PRODTYPE_QRY */
    if (AT_CMD_PRODTYPE_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: %d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    stProdTypeCnf->ulProdType);

    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, AT_OK);
    return VOS_OK;
}


VOS_VOID At_ProcMsgFromDrvAgent(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          i;
    VOS_UINT32                          ulMsgCnt;
    VOS_UINT32                          ulMsgId;
    VOS_UINT32                          ulRst;

    /*从g_astDrvAgentMsgProcTab中获取消息个数*/
    ulMsgCnt = sizeof(g_astAtProcMsgFromDrvAgentTab)/sizeof(AT_PROC_MSG_FROM_DRV_AGENT_STRU);
    pstMsg   = (DRV_AGENT_MSG_STRU *)pMsg;

    /*从消息包中获取MSG ID*/
    ulMsgId  = ((DRV_AGENT_MSG_STRU *)pstMsg)->ulMsgId;

    /*g_astDrvAgentMsgProcTab查表，进行消息分发*/
    for (i = 0; i < ulMsgCnt; i++)
    {
        if (g_astAtProcMsgFromDrvAgentTab[i].ulMsgType == ulMsgId)
        {
            ulRst = g_astAtProcMsgFromDrvAgentTab[i].pProcMsgFunc(pstMsg);

            if (VOS_ERR == ulRst)
            {
                AT_ERR_LOG("At_ProcMsgFromDrvAgent: Msg Proc Err!");
            }

            return;
        }
    }

    /*没有找到匹配的消息*/
    if (ulMsgCnt == i)
    {
        AT_ERR_LOG("At_ProcMsgFromDrvAgent: Msg Id is invalid!");
    }

    return;
}


TAF_UINT32 At_PB_Unicode2UnicodePrint(TAF_UINT32 MaxLength,TAF_INT8 *headaddr,TAF_UINT8 *pucDst, TAF_UINT8 *pucSrc, TAF_UINT16 usSrcLen)
{
    TAF_UINT16 usLen    = 0;
    TAF_UINT16 usChkLen = 0;
    TAF_UINT8  ucHigh1  = 0;
    TAF_UINT8  ucHigh2  = 0;
    TAF_UINT8 *pWrite   = pucDst;
    TAF_UINT8 *pRead    = pucSrc;

    if(((TAF_UINT32)(pucDst - (TAF_UINT8 *)headaddr) + (2 * usSrcLen)) >= MaxLength)
    {
        AT_ERR_LOG("At_Unicode2UnicodePrint too long");
        return 0;
    }

    /* 扫完整个字串 */
    while( usChkLen < usSrcLen )
    {
        /* 第一个字节 */
        ucHigh1 = 0x0F & (*pRead >> 4);
        ucHigh2 = 0x0F & *pRead;

        if(0x09 >= ucHigh1)   /* 0-9 */
        {
            *pWrite++ = ucHigh1 + 0x30;
        }
        else if(0x0A <= ucHigh1)    /* A-F */
        {
            *pWrite++ = ucHigh1 + 0x37;
        }
        else
        {

        }

        if(0x09 >= ucHigh2)   /* 0-9 */
        {
            *pWrite++ = ucHigh2 + 0x30;
        }
        else if(0x0A <= ucHigh2)    /* A-F */
        {
            *pWrite++ = ucHigh2 + 0x37;
        }
        else
        {

        }

        /* 下一个字符 */
        usChkLen++;
        pRead++;


        usLen += 2;    /* 记录长度 */
    }

    return usLen;
}


TAF_UINT16 At_PbGsmFormatPrint(TAF_UINT16 usMaxLength,
                               TAF_UINT8  *pucDst,
                               TAF_UINT8  *pucSrc,
                               TAF_UINT16 usSrcLen)
{
    TAF_UINT32  i;

    if (usSrcLen > usMaxLength)
    {
        AT_ERR_LOG("At_PbGsmFormatPrint error: too long");

        return 0;
    }

    for (i = 0; i < usSrcLen; i++)
    {
        /* 将最高bit清零 */
        pucDst[i] = pucSrc[i] & AT_PB_GSM7_CODE_MAX_VALUE;
    }

    return usSrcLen;
}


TAF_UINT32 At_PbOneUnicodeToIra(TAF_UINT16 usUnicodeChar, TAF_UINT8 *pucDst)
{
    TAF_UINT16 usIndex;

    for(usIndex = 0; usIndex < AT_PB_IRA_MAX_NUM; usIndex++)
    {
        if(usUnicodeChar == g_astIraToUnicode[usIndex].usUnicode)
        {
            *pucDst = g_astIraToUnicode[usIndex].ucOctet;

            return AT_SUCCESS;
        }
    }

    /* 未找到UCS2对应的IRA编码 */
    AT_LOG1("At_PbOneUnicodeToIra error: no found [%x]",usUnicodeChar);

    return AT_FAILURE;
}


TAF_UINT16 At_PbUnicodeToIraFormatPrint(TAF_UINT16 usMaxLength,
                                        TAF_UINT8  *pucDst,
                                        TAF_UINT8  *pucSrc,
                                        TAF_UINT16 usSrcLen)
{
    TAF_UINT16      usIndex;
    TAF_UINT16      usRetLen = 0;
    TAF_UINT8       *pucData = pucSrc;
    TAF_UINT16      usUnicodeChar;

    /* IRA码流的长度是UCS2码流长度的一半 */
    usRetLen = usSrcLen >> 1;

    /* 检查存储空间是否足够 */
    if (usRetLen > usMaxLength)
    {
       AT_ERR_LOG("At_PbUnicodeToIraFormatPrint error: too long");

       return 0;
    }

    /* 逐个将UCS2字符通过查表转换为IRA编码 */
    for (usIndex = 0; usIndex < usRetLen; usIndex++)
    {
        /* 先将UINT8数据转为UINT16 */
        usUnicodeChar = (*pucData << 8) | (*(pucData + 1));
        pucData += 2;

        if (AT_SUCCESS != At_PbOneUnicodeToIra(usUnicodeChar, &pucDst[usIndex]))
        {
            AT_ERR_LOG("At_PbUnicodeToIraFormatPrint error: no found");

            return 0;
        }
    }

    return usRetLen;
}


TAF_VOID At_PbUnicode82FormatPrint(TAF_UINT8  *puc82Code,
                                   TAF_UINT8  *pucDst,
                                   TAF_UINT16 *pusDstLen)
{

    TAF_UINT16      usIndex;
    TAF_UINT16      usBaseCode;
    TAF_UINT16      usUnicode;
    TAF_UINT16      usSrcLen;
    TAF_UINT16      usDstIndex  = 0;
    TAF_UINT16      usGsmCodeLen;
    TAF_UINT16      usGsm2UCS2CodeLen;
    TAF_UINT16      *pusUnicode = (TAF_UINT16 *)pucDst;
    TAF_UINT8       *pucSrc;

    /* 取得82编码的长度 */
    usSrcLen = puc82Code[0];

    /* 取得82编码的Basecode */
    usBaseCode = (puc82Code[1] << 8) | puc82Code[2];

    if((SI_PB_ALPHATAG_MAX_LEN < usSrcLen)||(0xFFFF == usBaseCode)) /*数据长度错误*/
    {
        *pusDstLen = 0;

        return;
    }

    /* 取得82编码的码流, 第四个及以后的Byte表示82压缩后的码流 */
    pucSrc = puc82Code + 3;

    /* 从第四个字节开始是82编码的码流 */
    for(usIndex = 0; usIndex < usSrcLen; usIndex++)
    {
        usGsmCodeLen = 0;
        while((AT_PB_GSM7_CODE_MAX_VALUE >= pucSrc[usIndex + usGsmCodeLen])
           && ((usIndex + usGsmCodeLen) < usSrcLen))
        {
            usGsmCodeLen++;
        }

        if(0 != usGsmCodeLen)
        {
            usGsm2UCS2CodeLen = 0;
            At_PbGsmToUnicode((pucSrc + usIndex), usGsmCodeLen,
                             (TAF_UINT8 *)(pusUnicode + usDstIndex),
                             &usGsm2UCS2CodeLen);

            usIndex    += (usGsmCodeLen - 1);
            usDstIndex += (usGsm2UCS2CodeLen >> 1);

            continue;
        }

        /* 82 编码先将第8bit置零再与基本码相加 */
        usUnicode = usBaseCode + (pucSrc[usIndex] & AT_PB_GSM7_CODE_MAX_VALUE);

        AT_UNICODE2VALUE(usUnicode);

        pusUnicode[usDstIndex++] = usUnicode;
    }

    *pusDstLen = (TAF_UINT16)(usDstIndex << 1);

    return;
}


TAF_VOID At_PbUnicode81FormatPrint(TAF_UINT8  *puc81Code,
                                   TAF_UINT8  *pucDst,
                                   TAF_UINT16 *pusDstLen)
{
    TAF_UINT16      usIndex;
    TAF_UINT16      usUnicode;
    TAF_UINT16      usBaseCode;
    TAF_UINT16      usSrcLen;
    TAF_UINT16      usDstIndex  = 0;
    TAF_UINT16      usGsmCodeLen;
    TAF_UINT16      usGsm2UCS2CodeLen;
    TAF_UINT16      *pusUnicode = (TAF_UINT16 *)pucDst;
    TAF_UINT8       *pucSrc;

    if(SI_PB_ALPHATAG_MAX_LEN < puc81Code[0])    /*字符个数不能大于姓名的最大值*/
    {
        *pusDstLen = 0;

        return;
    }

    /* 取得81编码的长度 */
    usSrcLen = puc81Code[0];

    /* 取得81编码的基指针 */
    usBaseCode = puc81Code[1] << 7;

    /* 取得81编码的码流, 第三个及以后的Byte表示81压缩后的码流 */
    pucSrc = puc81Code + 2;

    /* 从第三个字节开始是81编码的码流 */
    for (usIndex = 0; usIndex < usSrcLen; usIndex++)
    {
        usGsmCodeLen = 0;
        while ((AT_PB_GSM7_CODE_MAX_VALUE >= pucSrc[usIndex + usGsmCodeLen])
            && ((usIndex+usGsmCodeLen) < usSrcLen))
        {
            usGsmCodeLen++;
        }

        if (0 != usGsmCodeLen)
        {
            usGsm2UCS2CodeLen = 0;
            At_PbGsmToUnicode((pucSrc + usIndex), usGsmCodeLen,
                             (TAF_UINT8 *)(pusUnicode + usDstIndex),
                             &usGsm2UCS2CodeLen);

            usIndex    += (usGsmCodeLen - 1);
            usDstIndex += (usGsm2UCS2CodeLen >> 1);

            continue;
        }

        /* 81编码先将第8bit置零再与基本码相加 */
        usUnicode = usBaseCode + (pucSrc[usIndex] & AT_PB_GSM7_CODE_MAX_VALUE);

        AT_UNICODE2VALUE(usUnicode);

        pusUnicode[usDstIndex++] = usUnicode;
    }

    *pusDstLen = (TAF_UINT16)(usDstIndex << 1);

    return;
}


TAF_VOID At_PbUnicode80FormatPrint(TAF_UINT8  *pucSrc,
                                   TAF_UINT16 usSrcLen,
                                   TAF_UINT8  *pucDst,
                                   TAF_UINT16 *pusDstLen)
{
    if((0xFF == pucSrc[0])&&(0xFF == pucSrc[1]))   /*当首字符为FFFF，认为当前姓名为空*/
    {
        *pusDstLen = 0;
    }
    else
    {
        *pusDstLen = 0;

        /* 跳过80标志位 */
        if (500 >= usSrcLen)
        {
            TAF_MEM_CPY_S(pucDst, 500, pucSrc, usSrcLen);

            *pusDstLen = usSrcLen;
        }
    }

    return;
}


TAF_UINT32 At_PbGsmExtToUnicode(TAF_UINT8 ucGsmExtChar, TAF_UINT16 *pusUnicodeChar)
{
    TAF_UINT16      usIndex;

    /* 查找GSM到UNICODE扩展表，找到则返回成功，否则返回失败 */
    for (usIndex = 0; usIndex < AT_PB_GSM7EXT_MAX_NUM; usIndex++)
    {
        if (ucGsmExtChar == g_astGsm7extToUnicode[usIndex].ucOctet)
        {
            *pusUnicodeChar = g_astGsm7extToUnicode[usIndex].usUnicode;

            return AT_SUCCESS;
        }
    }

    AT_LOG1("At_PbGsmExtToUnicode warning: no found [0x%x]", ucGsmExtChar);

    return AT_FAILURE;
}


TAF_VOID At_PbGsmToUnicode(TAF_UINT8  *pucSrc,
                           TAF_UINT16 usSrcLen,
                           TAF_UINT8  *pucDst,
                           TAF_UINT16 *pusDstLen)
{
    TAF_UINT16      usIndex;
    TAF_UINT16      usUnicodeLen = 0;
    TAF_UINT16      *pusUnicode  = (TAF_UINT16 *)pucDst;
    TAF_UINT16      usUnicodeChar;

    for (usIndex = 0; usIndex < usSrcLen; usIndex++)
    {
        /* 为兼容有些平台再GSM模式下写入记录时未对最高BIT置零 */
        pucSrc[usIndex] = pucSrc[usIndex] & AT_PB_GSM7_CODE_MAX_VALUE;

        /* 非0x1B字符，直接查GSM到UNICODE基本表 */
        if (AT_PB_GSM7EXT_SYMBOL != pucSrc[usIndex])
        {
            usUnicodeChar = g_astGsmToUnicode[pucSrc[usIndex]].usUnicode;
            AT_UNICODE2VALUE(usUnicodeChar);
            *pusUnicode = usUnicodeChar;
            pusUnicode++;
            usUnicodeLen++;

            continue;
        }

        /* 当前GSM编码为0x1b时,可能为扩展标志 */
        if ((usSrcLen - usIndex) < 2)
        {
            /* 最后一个字节为0x1B，查GSM到UNICODE基本表, 将0x1B译为SPACE */
            usUnicodeChar = g_astGsmToUnicode[pucSrc[usIndex]].usUnicode;
            AT_UNICODE2VALUE(usUnicodeChar);
            *pusUnicode = usUnicodeChar;
            pusUnicode++;
            usUnicodeLen++;

            continue;
        }

        /* 对于0x1B 0x1B的情况，因为我们不支持第三张扩展表，因此直接将0x1B
            0x1B译为SPACE SPACE */
        if (AT_PB_GSM7EXT_SYMBOL == pucSrc[usIndex + 1])
        {
            usUnicodeChar = g_astGsmToUnicode[pucSrc[usIndex]].usUnicode;
            AT_UNICODE2VALUE(usUnicodeChar);
            *pusUnicode = usUnicodeChar;
            pusUnicode++;

            usUnicodeChar = g_astGsmToUnicode[pucSrc[usIndex + 1]].usUnicode;
            AT_UNICODE2VALUE(usUnicodeChar);
            *pusUnicode = usUnicodeChar;
            pusUnicode++;

            usIndex++;
            usUnicodeLen += 2;

            continue;
        }

        /* 0x1B可能为标志，查找GSM到UNICODE扩展表 */
        if (AT_SUCCESS == At_PbGsmExtToUnicode(pucSrc[usIndex + 1], &usUnicodeChar))
        {
            AT_UNICODE2VALUE(usUnicodeChar);
            *pusUnicode = usUnicodeChar;
            pusUnicode++;
            usIndex++;
            usUnicodeLen++;

            continue;
        }

       /* 对于1BXX，未在扩展表中，且XX不为0x1B的情况，译为SPACE+XX对应的字符 */
       usUnicodeChar = g_astGsmToUnicode[pucSrc[usIndex]].usUnicode;
       AT_UNICODE2VALUE(usUnicodeChar);
       *pusUnicode = usUnicodeChar;
       pusUnicode++;
       usUnicodeLen++;
    }

    /* GSM7BIT 码流中可能有扩展表中字符，GSM7bit到UNICODE转换长度不一定是原码流长度的2倍 */
    *pusDstLen = (TAF_UINT16)(usUnicodeLen << 1);

    return;
}


TAF_VOID At_PbRecordToUnicode(SI_PB_EVENT_INFO_STRU *pstEvent,
                              TAF_UINT8             *pucDecode,
                              TAF_UINT16            *pusDecodeLen)
{
    TAF_UINT16      usDecodeLen = 0;

    switch(pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType)
    {
        case SI_PB_ALPHATAG_TYPE_GSM:
            At_PbGsmToUnicode(pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTag,
                              pstEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength,
                              pucDecode,
                              &usDecodeLen);
            break;
        case SI_PB_ALPHATAG_TYPE_UCS2_80:
            At_PbUnicode80FormatPrint(pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTag,
                                      pstEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength,
                                      pucDecode,
                                      &usDecodeLen);
            break;
        case SI_PB_ALPHATAG_TYPE_UCS2_81:
            At_PbUnicode81FormatPrint(pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTag,
                                      pucDecode,
                                      &usDecodeLen);
            break;
        case SI_PB_ALPHATAG_TYPE_UCS2_82:
            At_PbUnicode82FormatPrint(pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTag,
                                      pucDecode,
                                      &usDecodeLen);
            break;
        default:
            AT_ERR_LOG("At_PbRecordToUnicode error: not support code type");

            break;
    }

    *pusDecodeLen = usDecodeLen;

    return;
}


TAF_UINT32 At_Pb_AlaphPrint(TAF_UINT16            *pusDataLen,
                            SI_PB_EVENT_INFO_STRU *pstEvent,
                            TAF_UINT8             *pucData)
{
    TAF_UINT16              usLength = *pusDataLen;
    TAF_UINT16              usDecodeLen = 0;
    TAF_UINT16              usReturnLen = 0;
    TAF_UINT8               aucDecode[500] = {0};

    if (AT_CSCS_GSM_7Bit_CODE == gucAtCscsType)
    {
        /* 在GSM字符集下，不支持UCS2编码 */
        if ((SI_PB_ALPHATAG_TYPE_UCS2_80 == pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType)
         || (SI_PB_ALPHATAG_TYPE_UCS2_81 == pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType)
         || (SI_PB_ALPHATAG_TYPE_UCS2_82 == pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType))
        {
            /* usReturnLen = 0; */

            AT_LOG1("At_Pb_AlaphPrint error: GSM7 doesn't read UCS code[%d]",
                    pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType);
            return AT_FAILURE;
        }

        usReturnLen = At_PbGsmFormatPrint((AT_CMD_MAX_LEN - usLength),
                                          (pucData + usLength),
                                          pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTag,
                                          pstEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength);
    }
    else  /* 当前字符集为UCS2或IRA时 */
    {
        /* 当前字符集为UCS2或IRA时，先根据存储格式转换为UCS2编码 */
        At_PbRecordToUnicode(pstEvent, aucDecode, &usDecodeLen);

        /* 当前字符集为IRA时，将UCS2转换为IRA码流输出*/
        if (AT_CSCS_IRA_CODE == gucAtCscsType)
        {
            usReturnLen = At_PbUnicodeToIraFormatPrint((AT_CMD_MAX_LEN - usLength),
                                                       (pucData + usLength),
                                                       aucDecode,
                                                       usDecodeLen);
        }

        /* 当前字符集为UCS2时，将UCS2转换为打印格式输出*/
        if (AT_CSCS_UCS2_CODE == gucAtCscsType)
        {
            usReturnLen = (TAF_UINT16)At_Unicode2UnicodePrint(AT_CMD_MAX_LEN,
                                                              (TAF_INT8 *)pucData,
                                                              (pucData + usLength),
                                                              aucDecode,
                                                              usDecodeLen);
        }
    }

    if (0 == usReturnLen)
    {
        gstAtSendData.usBufLen = 0;

        AT_LOG1("At_Pb_AlaphPrint error: usReturnLen = 0,gucAtCscsType = [%d]", gucAtCscsType);

        return AT_FAILURE;
    }

    usLength    += usReturnLen;
    *pusDataLen  = usLength;

    return AT_SUCCESS;
}


TAF_UINT32 At_Pb_CnumAlaphPrint(TAF_UINT16            *pusDataLen,
                            SI_PB_EVENT_INFO_STRU *pstEvent,
                            TAF_UINT8             *pucData)
{
    TAF_UINT16              usLength = *pusDataLen;
    TAF_UINT16              usDecodeLen = 0;
    TAF_UINT16              usReturnLen = 0;
    TAF_UINT8               aucDecode[500] = {0};

    if (AT_CSCS_GSM_7Bit_CODE == gucAtCscsType)
    {
        /* 在GSM字符集下，不支持UCS2编码 */
        if ((SI_PB_ALPHATAG_TYPE_UCS2_80 == pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType)
         || (SI_PB_ALPHATAG_TYPE_UCS2_81 == pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType)
         || (SI_PB_ALPHATAG_TYPE_UCS2_82 == pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType))
        {

            AT_LOG1("At_Pb_AlaphPrint error: GSM7 doesn't read UCS code[%d]",
                    pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType);
            return AT_SUCCESS;
        }

        usReturnLen = At_PbGsmFormatPrint((AT_CMD_MAX_LEN - usLength),
                                          (pucData + usLength),
                                          pstEvent->PBEvent.PBReadCnf.PBRecord.AlphaTag,
                                          pstEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength);
    }
    else  /* 当前字符集为UCS2或IRA时 */
    {
        /* 当前字符集为UCS2或IRA时，先根据存储格式转换为UCS2编码 */
        At_PbRecordToUnicode(pstEvent, aucDecode, &usDecodeLen);

        /* 当前字符集为IRA时，将UCS2转换为IRA码流输出*/
        if (AT_CSCS_IRA_CODE == gucAtCscsType)
        {
            usReturnLen = At_PbUnicodeToIraFormatPrint((AT_CMD_MAX_LEN - usLength),
                                                       (pucData + usLength),
                                                       aucDecode,
                                                       usDecodeLen);
        }

        /* 当前字符集为UCS2时，将UCS2转换为打印格式输出*/
        if (AT_CSCS_UCS2_CODE == gucAtCscsType)
        {
            usReturnLen = (TAF_UINT16)At_Unicode2UnicodePrint(AT_CMD_MAX_LEN,
                                                              (TAF_INT8 *)pucData,
                                                              (pucData + usLength),
                                                              aucDecode,
                                                              usDecodeLen);
        }
    }

    if (0 == usReturnLen)
    {

        AT_LOG1("At_Pb_AlaphPrint error: usReturnLen = 0,gucAtCscsType = [%d]", gucAtCscsType);

        return AT_SUCCESS;
    }

    usLength    += usReturnLen;
    *pusDataLen  = usLength;

    return AT_SUCCESS;
}


TAF_VOID AT_Pb_NumberPrint(TAF_UINT16 *pusDataLen, SI_PB_EVENT_INFO_STRU *pstEvent, TAF_UINT8 *pucData)
{
    TAF_UINT16      usLength = *pusDataLen;

    if(0 == pstEvent->PBEvent.PBReadCnf.PBRecord.NumberLength)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pucData,
                                           (TAF_CHAR *)pucData+usLength,
                                           ",\"\",129");
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pucData,
                                           (TAF_CHAR *)pucData + usLength,
                                           ",\"");

        if (PB_NUMBER_TYPE_INTERNATIONAL == pstEvent->PBEvent.PBReadCnf.PBRecord.NumberType )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pucData,
                                               (TAF_CHAR *)pucData+usLength,
                                               "+");
        }

        if ((AT_CMD_MAX_LEN + 20 - 1 - usLength) >= pstEvent->PBEvent.PBReadCnf.PBRecord.NumberLength)
        {
            TAF_MEM_CPY_S(pucData + usLength,
                          AT_CMD_MAX_LEN + 20 - 1 - usLength,
                          pstEvent->PBEvent.PBReadCnf.PBRecord.Number,
                          pstEvent->PBEvent.PBReadCnf.PBRecord.NumberLength);

            usLength += (TAF_UINT16)pstEvent->PBEvent.PBReadCnf.PBRecord.NumberLength;
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pucData,
                                           (TAF_CHAR *)pucData + usLength,
                                           "\"");

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pucData,
                                           (TAF_CHAR *)pucData + usLength,
                                           ",%d",
                                           pstEvent->PBEvent.PBReadCnf.PBRecord.NumberType);
    }

    *pusDataLen  = usLength;

    return;
}


TAF_UINT32 At_PbCNUMCmdPrint(VOS_UINT8 ucIndex,TAF_UINT16 *pusDataLen,TAF_UINT8 *pucData,SI_PB_EVENT_INFO_STRU *pEvent)
{
    TAF_UINT16              usLength = *pusDataLen;
    TAF_UINT32              ulResult;

    if(SI_PB_CONTENT_INVALID == pEvent->PBEvent.PBReadCnf.PBRecord.ValidFlag)
    {
        return AT_SUCCESS;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,(TAF_CHAR *)pucData + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    /* 注意:内容需要根据编码类型进行输出，如果是ASCII码直接打印，如果UNICODE需要转换 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,(TAF_CHAR *)pucData + usLength,"\"");

    if ( (pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength != 0)
      && (pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength <= SI_PB_ALPHATAG_MAX_LEN) )
    {
         ulResult = At_Pb_CnumAlaphPrint(&usLength, pEvent, pucData);

        if(AT_SUCCESS != ulResult)
        {
            At_FormatResultData(ucIndex, AT_CME_INVALID_CHARACTERS_IN_TEXT_STRING);

            return AT_FAILURE;
        }
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,(TAF_CHAR *)pucData + usLength,"\"");


    /* 将电话号码及号码类型处理封装为函数 */
    AT_Pb_NumberPrint(&usLength, pEvent, pucData);

    *pusDataLen = usLength;

    return AT_SUCCESS;
}


TAF_UINT32 At_PbCPBR2CmdPrint(VOS_UINT8 ucIndex,TAF_UINT16 *pusDataLen,TAF_UINT8 *pucData,SI_PB_EVENT_INFO_STRU *pEvent)
{
    TAF_UINT16              usLength = *pusDataLen;
    TAF_UINT32              ulResult;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,
                                    (TAF_CHAR *)pucData+usLength, "%s: ",
                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,
                                    (TAF_CHAR *)pucData+usLength, "%d",
                                    pEvent->PBEvent.PBReadCnf.PBRecord.Index);


    /* 将电话号码及号码类型处理封装为函数 */
    AT_Pb_NumberPrint(&usLength, pEvent, pucData);

    /* 注意:内容需要根据编码类型进行输出，如果是ASCII码直接打印，如果UNICODE需要转换 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,(TAF_CHAR *)pucData + usLength,",\"");

    if( (pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength != 0)
     && (pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength <= SI_PB_ALPHATAG_MAX_LEN) )
    {
        ulResult = At_Pb_AlaphPrint(&usLength, pEvent, pucData);

        if(AT_SUCCESS != ulResult)
        {
            At_FormatResultData(ucIndex, AT_CME_INVALID_CHARACTERS_IN_TEXT_STRING);

            return AT_FAILURE;
        }
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,
                                (TAF_CHAR *)pucData+usLength, "\"");

    *pusDataLen = usLength;

    return AT_SUCCESS;
}



TAF_UINT32 At_PbCPBRCmdPrint(VOS_UINT8 ucIndex,TAF_UINT16 *pusDataLen,TAF_UINT8 *pucData,SI_PB_EVENT_INFO_STRU *pEvent)
{
    TAF_UINT16              usLength = *pusDataLen;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,
                                    (TAF_CHAR *)pucData+usLength, "%s: ",
                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,
                                    (TAF_CHAR *)pucData+usLength, "%d",
                                    pEvent->PBEvent.PBReadCnf.PBRecord.Index);


    /* 将电话号码及号码类型处理封装为函数 */
    AT_Pb_NumberPrint(&usLength, pEvent, pucData);

    /* 注意:内容需要根据编码类型进行输出，如果是ASCII码直接打印，如果UNICODE需要转换 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,(TAF_CHAR *)pucData + usLength,",\"");

    if ( (pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength != 0)
      && (pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength <= SI_PB_ALPHATAG_MAX_LEN) )
    {

        if(SI_PB_ALPHATAG_TYPE_UCS2 == (pEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType&0x80))  /* DATA:IRA */
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,
                                                (TAF_CHAR *)pucData+usLength, "%X",
                                                pEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType);

            usLength += (TAF_UINT16)At_PB_Unicode2UnicodePrint(AT_CMD_MAX_LEN,(TAF_INT8 *)pucData,
                                        pucData+usLength, pEvent->PBEvent.PBReadCnf.PBRecord.AlphaTag,
                                        pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength);

            if((pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength%2) == 0)/*对于奇数个中文需要后面补充FF*/
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,(TAF_CHAR *)pucData + usLength,"FF");
            }
        }
        else                                    /* DATA:UCS2 */
        {
            if ((AT_CMD_MAX_LEN + 20 - 1 - usLength) >= pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength)
            {
                TAF_MEM_CPY_S((TAF_CHAR *)pucData + usLength,
                              AT_CMD_MAX_LEN + 20 - 1 - usLength,
                              pEvent->PBEvent.PBReadCnf.PBRecord.AlphaTag,
                              pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength);

                usLength += pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength;
            }
        }
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,
                                (TAF_CHAR *)pucData+usLength, "\"");

    if(SI_PB_ALPHATAG_TYPE_UCS2 == (pEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType&0x80))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,
                                        (TAF_CHAR *)pucData+usLength, ",1");
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pucData,
                                        (TAF_CHAR *)pucData+usLength, ",0");
    }

    *pusDataLen = usLength;

    return AT_SUCCESS;
}


TAF_VOID At_PbEmailPrint(TAF_UINT8  *pucSrc,
                         TAF_UINT16 usSrcLen,
                         TAF_UINT8  *pucDst,
                         TAF_UINT16 *pusDstLen)
{
    TAF_UINT8   aucUCS2Code[2 * SI_PB_EMAIL_MAX_LEN];
    TAF_UINT16  usUCS2CodeLen;
    TAF_UINT16  usEMailLen = usSrcLen;

    TAF_UINT16  usReturnLen;

    /* 对EMAIL长度大于64Byte，进行截断处理 */
    if(SI_PB_EMAIL_MAX_LEN < usEMailLen)
    {
        usEMailLen = SI_PB_EMAIL_MAX_LEN;
    }

    /* 先将GSM模式转成UCS2模式 */
    At_PbGsmToUnicode(pucSrc, usEMailLen, aucUCS2Code, &usUCS2CodeLen);

    /* 再将UCS2模式转成IRA模式 */
    usReturnLen = At_PbUnicodeToIraFormatPrint(usSrcLen, pucDst, aucUCS2Code, usUCS2CodeLen);
    if (0 == usReturnLen)
    {
        AT_INFO_LOG("At_PbEmailPrint error: usReturnLen = 0");
        return;
    }

    *pusDstLen = (TAF_UINT16)(usUCS2CodeLen >> 1);

    return;
}


TAF_UINT32 At_PbSCPBRCmdPrint(VOS_UINT8 ucIndex,TAF_UINT16 *pusDataLen,SI_PB_EVENT_INFO_STRU *pEvent)
{
    TAF_UINT16              usLength = *pusDataLen;
    TAF_UINT16              i;
    TAF_UINT16              usEmailLen = 0;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                    (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, "%s: ",
                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                    (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, "%d",
                                    pEvent->PBEvent.PBReadCnf.PBRecord.Index);


    /* 将电话号码及号码类型处理封装为函数 */
    AT_Pb_NumberPrint(&usLength, pEvent, pgucAtSndCrLfAddr);

    for(i = 0; i < 3; i++)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                        (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, ",\"");

        if( (0 == pEvent->PBEvent.PBReadCnf.PBRecord.AdditionNumber[i].NumberLength)
         || ((SI_PB_PHONENUM_MAX_LEN + 2) < pEvent->PBEvent.PBReadCnf.PBRecord.AdditionNumber[i].NumberLength) )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                            (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, "\",129");

            continue;
        }

        if (PB_NUMBER_TYPE_INTERNATIONAL == pEvent->PBEvent.PBReadCnf.PBRecord.AdditionNumber[i].NumberType )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                        (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, "+");
        }

        if ((AT_CMD_MAX_LEN + 20 - 1 - usLength) >= pEvent->PBEvent.PBReadCnf.PBRecord.AdditionNumber[i].NumberLength)
        {
            TAF_MEM_CPY_S(pgucAtSndCrLfAddr + usLength,
                          AT_CMD_MAX_LEN + 20 - 1 - usLength,
                          pEvent->PBEvent.PBReadCnf.PBRecord.AdditionNumber[i].Number,
                          pEvent->PBEvent.PBReadCnf.PBRecord.AdditionNumber[i].NumberLength);

            usLength += (TAF_UINT16)pEvent->PBEvent.PBReadCnf.PBRecord.AdditionNumber[i].NumberLength;
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                        (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, "\"");

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                        (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, ",%d",
                                        pEvent->PBEvent.PBReadCnf.PBRecord.AdditionNumber[i].NumberType);
    }

    /* 注意:内容需要根据编码类型进行输出，如果是ASCII码直接打印，如果UNICODE需要转换 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,(TAF_CHAR *)pgucAtSndCrLfAddr + usLength,",\"");

    if( (pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength != 0)
     && (pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength <= SI_PB_ALPHATAG_MAX_LEN) )
    {
        if(SI_PB_ALPHATAG_TYPE_UCS2 == (pEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType&0x80))  /* DATA:IRA */
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                                (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, "%X",
                                                pEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType);

            usLength += (TAF_UINT16)At_PB_Unicode2UnicodePrint(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCrLfAddr,
                                        pgucAtSndCrLfAddr+usLength, pEvent->PBEvent.PBReadCnf.PBRecord.AlphaTag,
                                        pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength);

            if((pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength%2) == 0)/*对于奇数个中文需要后面补充FF*/
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,(TAF_CHAR *)pgucAtSndCrLfAddr + usLength,"FF");
            }
        }
        else                                    /* DATA:UCS2 */
        {
            if ((AT_CMD_MAX_LEN + 20 - 1 - usLength) >= pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength)
            {
                TAF_MEM_CPY_S((TAF_CHAR *)pgucAtSndCrLfAddr + usLength,
                               AT_CMD_MAX_LEN + 20 - 1 - usLength,
                               pEvent->PBEvent.PBReadCnf.PBRecord.AlphaTag,
                               pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength);

                usLength += pEvent->PBEvent.PBReadCnf.PBRecord.ucAlphaTagLength;
            }
        }
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, "\"");

    if (AT_CMD_SCPBR_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {

        if(SI_PB_ALPHATAG_TYPE_UCS2 == (pEvent->PBEvent.PBReadCnf.PBRecord.AlphaTagType&0x80))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                            (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, ",1");
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                            (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, ",0");
        }
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                    (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, ",\"");

    if ( 0 != pEvent->PBEvent.PBReadCnf.PBRecord.Email.EmailLen )
    {
        At_PbEmailPrint(pEvent->PBEvent.PBReadCnf.PBRecord.Email.Email,
                        (TAF_UINT16)pEvent->PBEvent.PBReadCnf.PBRecord.Email.EmailLen,
                        (TAF_UINT8 *)(pgucAtSndCrLfAddr + usLength),
                        &usEmailLen);

        usLength += usEmailLen;

    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                    (TAF_CHAR *)pgucAtSndCrLfAddr+usLength, "\"");

    *pusDataLen = usLength;

    return AT_SUCCESS;
}

/*****************************************************************************
 Prototype      : At_PbSearchCnfProc
 Description    : 电话簿查找数据打印函数
 Input          : pEvent --- 事件内容
 Output         :
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        :
  1.Date        : 2009-03-14
    Author      : ---
    Modification: Created function
*****************************************************************************/
TAF_VOID At_PbSearchCnfProc(VOS_UINT8 ucIndex,SI_PB_EVENT_INFO_STRU  *pEvent)
{
    TAF_UINT16              usLength = 0;

    gulPBPrintTag = TAF_TRUE;

/*
    if(0 != usLength)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr, (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
    }
*/
    if(pEvent->PBEvent.PBSearchCnf.PBRecord.ValidFlag == SI_PB_CONTENT_INVALID)/*当前的内容无效*/
    {
        return;              /*返回不处理*/
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",pEvent->PBEvent.PBSearchCnf.PBRecord.Index);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",\"");

    if (PB_NUMBER_TYPE_INTERNATIONAL == pEvent->PBEvent.PBSearchCnf.PBRecord.NumberType)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"+");
    }

    if ((AT_CMD_MAX_LEN + 20 - 3 - usLength) >= pEvent->PBEvent.PBSearchCnf.PBRecord.NumberLength)
    {
        TAF_MEM_CPY_S(pgucAtSndCodeAddr + usLength,
                      AT_CMD_MAX_LEN + 20 - 3 - usLength,
                      pEvent->PBEvent.PBSearchCnf.PBRecord.Number,
                      pEvent->PBEvent.PBSearchCnf.PBRecord.NumberLength);

        usLength += (TAF_UINT16)pEvent->PBEvent.PBSearchCnf.PBRecord.NumberLength;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d,",pEvent->PBEvent.PBSearchCnf.PBRecord.NumberType);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");
    /* PS_MEM_CPY((TAF_CHAR *)pgucAtSndCodeAddr + usLength,pEvent->PBEvent.PBSearchCnf.PBRecord.AlphaTag, pEvent->PBEvent.PBSearchCnf.PBRecord.ucAlphaTagLength); */
    /* usLength += pEvent->PBEvent.PBSearchCnf.PBRecord.ucAlphaTagLength; */

    if(SI_PB_ALPHATAG_TYPE_UCS2 == (pEvent->PBEvent.PBSearchCnf.PBRecord.AlphaTagType & 0x80))  /* DATA:IRA */
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr+usLength,"%X",pEvent->PBEvent.PBSearchCnf.PBRecord.AlphaTagType);

        usLength += (TAF_UINT16)At_PB_Unicode2UnicodePrint(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,pgucAtSndCodeAddr+usLength,pEvent->PBEvent.PBSearchCnf.PBRecord.AlphaTag,pEvent->PBEvent.PBSearchCnf.PBRecord.ucAlphaTagLength);

        if((pEvent->PBEvent.PBSearchCnf.PBRecord.ucAlphaTagLength%2) == 0)/*对于奇数个中文需要后面补充FF*/
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"FF");
        }
    }
    else                                    /* DATA:UCS2 */
    {
        if(AT_CSCS_UCS2_CODE == gucAtCscsType)       /* +CSCS:UCS2 */
        {
            usLength += (TAF_UINT16)At_Ascii2UnicodePrint(AT_CMD_MAX_LEN,(TAF_INT8 *)pgucAtSndCodeAddr,pgucAtSndCodeAddr+usLength,pEvent->PBEvent.PBSearchCnf.PBRecord.AlphaTag,pEvent->PBEvent.PBSearchCnf.PBRecord.ucAlphaTagLength);
        }
        else
        {
            if ((AT_CMD_MAX_LEN + 20 - 3 - usLength) >= pEvent->PBEvent.PBSearchCnf.PBRecord.ucAlphaTagLength)
            {
                TAF_MEM_CPY_S((TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                               AT_CMD_MAX_LEN + 20 - 3 - usLength,
                               pEvent->PBEvent.PBSearchCnf.PBRecord.AlphaTag,
                               pEvent->PBEvent.PBSearchCnf.PBRecord.ucAlphaTagLength);

                usLength += pEvent->PBEvent.PBSearchCnf.PBRecord.ucAlphaTagLength;
            }
        }
    }
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"\"");

    if(AT_V_ENTIRE_TYPE == gucAtVType)
    {
        TAF_MEM_CPY_S((TAF_CHAR *)pgucAtSndCrLfAddr, AT_CMD_MAX_LEN + 20 - 1, (TAF_CHAR *)gaucAtCrLf, 2);/*Code前面加\r\n*/
        At_SendResultData(ucIndex,pgucAtSndCrLfAddr,usLength+2);
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);

        At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);
    }
}



TAF_UINT32 At_PbReadCnfProc(VOS_UINT8 ucIndex,SI_PB_EVENT_INFO_STRU  *pEvent)
{
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usPBReadPrintLength;
    ulResult                            = AT_FAILURE;
    usPBReadPrintLength                 = 0;

    if (SI_PB_CONTENT_INVALID == pEvent->PBEvent.PBReadCnf.PBRecord.ValidFlag)/*当前的内容无效*/
    {
        return AT_SUCCESS;              /*返回不处理*/
    }
    else
    {
        if (TAF_FALSE == gulPBPrintTag)
        {
             usPBReadPrintLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,(TAF_CHAR *)pgucAtSndCrLfAddr,"%s","\r\n");
        }

        gulPBPrintTag = TAF_TRUE;

        if(AT_CMD_CPBR_SET == gastAtClientTab[ucIndex].CmdCurrentOpt) /*按照 ^CPBR 的方式进行打印*/
        {
            ulResult = At_PbCPBRCmdPrint(ucIndex,&usPBReadPrintLength,pgucAtSndCrLfAddr,pEvent);
        }
        else if(AT_CMD_CPBR2_SET == gastAtClientTab[ucIndex].CmdCurrentOpt) /*按照 +CPBR 的方式进行打印*/
        {
            ulResult = At_PbCPBR2CmdPrint(ucIndex,&usPBReadPrintLength,pgucAtSndCrLfAddr,pEvent);
        }
        else if(AT_CMD_SCPBR_SET == gastAtClientTab[ucIndex].CmdCurrentOpt) /*按照 ^SCPBR 的方式进行打印*/
        {
            ulResult = At_PbSCPBRCmdPrint(ucIndex,&usPBReadPrintLength,pEvent);
        }
        else if(AT_CMD_CNUM_READ == gastAtClientTab[ucIndex].CmdCurrentOpt) /*按照 CNUM 的方式进行打印*/
        {
            ulResult = At_PbCNUMCmdPrint(ucIndex,&usPBReadPrintLength,pgucAtSndCrLfAddr,pEvent);
        }
        else
        {
            AT_ERR_LOG1("At_PbReadCnfProc: the Cmd Current Opt %d is Unknow", gastAtClientTab[ucIndex].CmdCurrentOpt);

            return AT_FAILURE;
        }
    }

    if(AT_SUCCESS == ulResult)
    {
        usPBReadPrintLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCrLfAddr,
                                                (TAF_CHAR *)pgucAtSndCrLfAddr + usPBReadPrintLength,
                                                "%s","\r\n");
    }

    At_SendResultData(ucIndex,pgucAtSndCrLfAddr,usPBReadPrintLength);

    TAF_MEM_CPY_S((TAF_CHAR *)pgucAtSndCrLfAddr, AT_CMD_MAX_LEN + 20 - 1, (TAF_CHAR *)gaucAtCrLf,2);/*AT输出Buffer的前两个字节恢复为\r\n*/

    return ulResult;
}



TAF_VOID At_PbCallBackFunc(SI_PB_EVENT_INFO_STRU  *pEvent)
{
    TAF_UINT32 ulSendMsg = 0;
    TAF_UINT8  ucIndex;

    if(TAF_NULL_PTR == pEvent)      /*参数错误*/
    {
        AT_WARN_LOG("At_PbCallBackFunc pEvent NULL");

        return;
    }

    if(AT_FAILURE == At_ClientIdToUserId(pEvent->ClientId, &ucIndex))/*回复的客户端内容错误*/
    {
        //At_SendReportMsg(MN_CALLBACK_PHONE_BOOK, (TAF_UINT8*)pEvent, sizeof(SI_PB_EVENT_INFO_STRU));

        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_PbMsgProc : AT_BROADCAST_INDEX.");
        return;
    }

    if(AT_CMD_CNUM == gastAtClientTab[ucIndex].CmdIndex)
    {
        pEvent->PBError = ((TAF_ERR_PB_NOT_FOUND == pEvent->PBError)?TAF_ERR_NO_ERROR:pEvent->PBError);
    }

    if(TAF_ERR_NO_ERROR != pEvent->PBError)/*出错，发送消息处理*/
    {
        ulSendMsg = SI_TRUE;
    }
    else if(((pEvent->PBEventType == SI_PB_EVENT_READ_CNF)||(pEvent->PBEventType == SI_PB_EVENT_SREAD_CNF))
        && (pEvent->PBLastTag != SI_PB_LAST_TAG_TRUE))/*由于读取的特殊要求，因此不能通过一次性的消息发送数据进行处理*/
    {
        if ( AT_SUCCESS != At_PbReadCnfProc(ucIndex,pEvent) )
        {
            AT_WARN_LOG("At_PbCallBackFunc:At_PbReadCnfProc Error");
        }

        ulSendMsg = SI_FALSE;
    }
    else if(pEvent->PBEventType == SI_PB_EVENT_SEARCH_CNF)
    {
        if( SI_PB_LAST_TAG_TRUE == pEvent->PBLastTag )
        {
            ulSendMsg = SI_TRUE;
        }
        else
        {
            At_PbSearchCnfProc(ucIndex,pEvent);
            return;
        }
    }
    else        /*其它的回复可以通过消息进行一次性发送处理*/
    {
        ulSendMsg = SI_TRUE;
    }

    if (SI_TRUE == ulSendMsg)
    {
        At_SendReportMsg(MN_CALLBACK_PHONE_BOOK,(TAF_UINT8*)pEvent,sizeof(SI_PB_EVENT_INFO_STRU));
    }

    return;
}



VOS_VOID At_RcvVcMsgSetGroundCnfProc(
    MN_AT_IND_EVT_STRU                 *pstData
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgSetGroundCnfProc:WARNING:AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgSetGroundCnfProc : AT_BROADCAST_INDEX.");
        return;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CBG_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("At_RcvVcMsgSetGroundCnfProc:WARNING:AT ARE WAITING ANOTHER CMD!");
        return;
    }

    if (VOS_OK == pstData->aucContent[0])
    {
        ulRet = AT_OK;
    }
    else
    {
        ulRet = AT_ERROR;
    }

    /* 输出设置结果 */
    gstAtSendData.usBufLen = 0;

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return;
}


VOS_VOID At_RcvVcMsgQryGroundRspProc(
    MN_AT_IND_EVT_STRU                 *pstData
)
{
    APP_VC_QRY_GROUNG_RSP_STRU         *pstQryRslt;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgQryGroundRspProc:WARNING:AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgQryGroundRspProc : AT_BROADCAST_INDEX.");
        return;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CBG_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("At_RcvVcMsgQryGroundRspProc:WARNING:AT ARE WAITING ANOTHER CMD!");
        return;
    }

    /* 初始化 */
    pstQryRslt = (APP_VC_QRY_GROUNG_RSP_STRU *)pstData->aucContent;

    if (VOS_OK == pstQryRslt->ucQryRslt)
    {
        /* 输出查询结果 */
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        "^CBG:%d",
                                                        pstQryRslt->enGround);
        ulRet = AT_OK;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
        ulRet = AT_ERROR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return;
}



VOS_VOID At_RcvVcMsgQryTTYModeCnfProc(
    MN_AT_IND_EVT_STRU                 *pstData
)
{
    APP_VC_QRY_TTYMODE_CNF_STRU        *pstTTYMode;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstTTYMode      = (APP_VC_QRY_TTYMODE_CNF_STRU *)pstData->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgQryTTYModeCnfProc: WARNING: AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgQryTTYModeCnfProc: AT_BROADCAST_INDEX.");
        return;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_TTYMODE_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("At_RcvVcMsgQryTTYModeCnfProc: WARNING:AT ARE WAITING ANOTHER CMD!");
        return;
    }

    /* 查询的TTY MODE */
    if (VOS_OK == pstTTYMode->ucQryRslt)
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   "%s: %u",
                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                   pstTTYMode->enTTYMode);
        ulRet = AT_OK;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
        ulRet = AT_ERROR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return;
}


VOS_VOID At_RcvVcMsgSetTTYModeCnfProc(
    MN_AT_IND_EVT_STRU                 *pstData
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                          *pucResult;
    VOS_UINT8                           ucIndex;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgSetTTYModeCnfProc: WARNING: AT INDEX NOT FOUND!");
        return;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvVcMsgSetTTYModeCnfProc: AT_BROADCAST_INDEX.");
        return;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_TTYMODE_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("At_RcvVcMsgSetTTYModeCnfProc: WARNING:AT ARE WAITING ANOTHER CMD!");
        return;
    }

    pucResult = pstData->aucContent;
    if (VOS_OK == *pucResult)
    {
        ulRet = AT_OK;
    }
    else
    {
        ulRet = AT_ERROR;
    }

    /* 输出设置结果 */
    gstAtSendData.usBufLen = 0;

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return;
}



VOS_VOID At_ProcMsgFromVc(VOS_VOID *pMsg)
{
    MN_AT_IND_EVT_STRU                 *pstMsg = VOS_NULL_PTR;

    pstMsg = (MN_AT_IND_EVT_STRU*)pMsg;
    
    switch(pstMsg->usMsgName)
    {
        case APP_VC_MSG_CNF_QRY_MODE:
            At_RcvVcMsgQryModeCnfProc(pstMsg);
            break;

        case APP_VC_MSG_CNF_SET_PORT:
            At_RcvVcMsgSetPortCnfProc(pstMsg);
            break;

        case APP_VC_MSG_CNF_QRY_PORT:
            At_RcvVcMsgQryPortCnfProc(pstMsg);
            break;

        case APP_VC_MSG_SET_FOREGROUND_CNF:
        case APP_VC_MSG_SET_BACKGROUND_CNF:
            /*AT 处理 VC 上报的前台模式、后台模式CNF MSG*/
            At_RcvVcMsgSetGroundCnfProc(pstMsg);
            break;

        case APP_VC_MSG_FOREGROUND_RSP:
            /*AT 处理VC 上报的查询前台模式回复消息*/
            At_RcvVcMsgQryGroundRspProc(pstMsg);
            break;
        case APP_VC_MSG_QRY_TTYMODE_CNF:
            At_RcvVcMsgQryTTYModeCnfProc(pstMsg);
            break;
        case APP_VC_MSG_SET_TTYMODE_CNF:
            At_RcvVcMsgSetTTYModeCnfProc(pstMsg);
            break;

#if (FEATURE_ON == FEATURE_ECALL)
        case APP_VC_MSG_SET_MSD_CNF:
            AT_RcvVcMsgSetMsdCnfProc(pstMsg);
            break;

        case APP_VC_MSG_QRY_MSD_CNF:
            AT_RcvVcMsgQryMsdCnfProc(pstMsg);
            break;

        case APP_VC_MSG_QRY_ECALL_CFG_CNF:
            AT_RcvVcMsgQryEcallCfgCnfProc(pstMsg);
            break;
#endif

        case APP_VC_MSG_DTMF_DECODER_IND:
            At_RcvVcMsgDtmfDecoderIndProc(pstMsg);
            break;

        default:
            return;
    }
    return;
}


VOS_VOID At_RcvRnicMsg(VOS_VOID *pstMsg)
{
    MSG_HEADER_STRU                    *pstMsgHeader;

    pstMsgHeader                        = (MSG_HEADER_STRU *)pstMsg;

    switch(pstMsgHeader->ulMsgName)
    {
        /* 处理RNIC发来的拨号模式查询回复 */
        case ID_RNIC_AT_DIAL_MODE_CNF:

            AT_RcvRnicDialModeCnf((MsgBlock *)pstMsg);
            break;

        default:
            AT_ERR_LOG("At_RcvRnicMsg: ulMsgName err.");
            break;
    }
    return;
}


VOS_VOID At_ProcMsgFromCc(VOS_VOID *pMsg)
{
    MSG_HEADER_STRU                    *pstMsgHeader;

    pstMsgHeader = (MSG_HEADER_STRU *)pMsg;

    switch(pstMsgHeader->ulMsgName)
    {
        case AT_CC_MSG_STATE_QRY_CNF:
            At_RcvAtCcMsgStateQryCnfProc(pMsg);
            break;

        default:
            break;
    }
    return;
}


VOS_VOID AT_RcvNdisPdnInfoCfgCnf(
    AT_NDIS_PDNINFO_CFG_CNF_STRU       *pstNdisPdnInfoCfgCnf
)
{
    if (AT_NDIS_PDNCFG_CNF_FAIL == pstNdisPdnInfoCfgCnf->enResult)
    {
        AT_ERR_LOG("AT_RcvNdisPdnInfoRelCnf: Failed.");
    }

    return;
}


VOS_VOID AT_RcvNdisPdnInfoRelCnf(
    AT_NDIS_PDNINFO_REL_CNF_STRU       *pstNdisPdnInfoRelCnf
)
{
    if (AT_NDIS_SUCC != pstNdisPdnInfoRelCnf->enResult)
    {
        AT_ERR_LOG("AT_RcvNdisPdnInfoRelCnf: Failed.");
    }

    return;
}


VOS_VOID AT_RcvNdisMsg(VOS_VOID *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg = VOS_NULL_PTR;

    pstMsg  = (MSG_HEADER_STRU*)pMsg;
    switch (pstMsg->ulMsgName)
    {
        case ID_AT_NDIS_PDNINFO_CFG_CNF:
            AT_RcvNdisPdnInfoCfgCnf((AT_NDIS_PDNINFO_CFG_CNF_STRU *)pstMsg);
            break;

        case ID_AT_NDIS_PDNINFO_REL_CNF:
            AT_RcvNdisPdnInfoRelCnf((AT_NDIS_PDNINFO_REL_CNF_STRU *)pstMsg);
            break;

        default:
            AT_WARN_LOG("AT_RcvNdisMsg: Unexpected MSG is received.");
            break;
    }

    return;
}


VOS_UINT32    AT_ProcRabmSetFastDormParaCnf(
    RABM_AT_SET_FASTDORM_PARA_CNF_STRU     *pstMsg
)
{
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulRslt;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMsg->usClientId,&ucIndex))
    {
        AT_WARN_LOG("At_ProcRabmSetFastDormParaCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出设置结果 */
    gstAtSendData.usBufLen = 0;
    ulRslt = AT_ERROR;
    if (AT_RABM_PARA_SET_RSLT_SUCC == pstMsg->enRslt)
    {
        ulRslt = AT_OK;
    }

    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_UINT32    AT_ProcRabmQryFastDormParaCnf(
    RABM_AT_QRY_FASTDORM_PARA_CNF_STRU     *pstMsg
)
{
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulResult;
    VOS_UINT16                              usLength;

    ulResult = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMsg->usClientId,&ucIndex))
    {
        AT_WARN_LOG("At_RcvAtCcMsgStateQryCnfProc:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_TRUE == pstMsg->ucRslt)
    {
        /* 输出查询结果 */
        usLength = 0;
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"^FASTDORM:%d",pstMsg->stFastDormPara.enFastDormOperationType);

        if (AT_RABM_FASTDORM_STOP_FD_ASCR != pstMsg->stFastDormPara.enFastDormOperationType)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",pstMsg->stFastDormPara.ulTimeLen);
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);

        ulResult = AT_OK;
        gstAtSendData.usBufLen = usLength;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}


VOS_UINT32 AT_ProcRabmReleaseRrcCnf(RABM_AT_RELEASE_RRC_CNF_STRU *pstMsg)
{
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulRslt;

    ulRslt = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMsg->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_ProcRabmReleaseRrcCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 输出设置结果 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK == pstMsg->ulRslt)
    {
        ulRslt = AT_OK;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_HUAWEI_VP)

VOS_UINT32    AT_ProcRabmSetVoicePreferParaCnf(
    RABM_AT_SET_VOICEPREFER_PARA_CNF_STRU     *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRslt;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMsg->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_ProcRabmSetVoicePreferParaCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出设置结果 */
    gstAtSendData.usBufLen = 0;
    ulRslt = AT_ERROR;
    if (AT_RABM_PARA_SET_RSLT_SUCC == pstMsg->enRslt)
    {
        ulRslt = AT_OK;
    }

    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}

VOS_UINT32 AT_ProcRabmQryVoicePreferEnableParaCnf(RABM_AT_QRY_VOICEPREFER_PARA_CNF_STRU *pstMsg)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;

    ulResult = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMsg->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_ProcRabmQryVoicePreferEnableParaCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    usLength = 0;
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s:%d%s",
                                       gaucAtCrLf,
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       pstMsg->ulRslt,
                                       gaucAtCrLf);
    ulResult = AT_OK;
    gstAtSendData.usBufLen = usLength;

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}

VOS_UINT32 AT_ProcRabmVoicePreferStatusReport(RABM_AT_VOICEPREFER_STATUS_REPORT_STRU *pstMsg)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    ucIndex = 0;
    usLength = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMsg->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_ProcRabmVoicePreferStatusReport:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 输出上报结果 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "%s^VOICEPREFERSTATUS:%d%s",
                                      gaucAtCrLf,
                                      pstMsg->ulVpStatus,
                                      gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}
#endif


TAF_VOID AT_RabmMsgProc(VOS_VOID *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg = VOS_NULL_PTR;

    pstMsg = (MSG_HEADER_STRU *)pMsg;
    switch(pstMsg->ulMsgName)
    {
        case ID_RABM_AT_SET_FASTDORM_PARA_CNF:
            AT_ProcRabmSetFastDormParaCnf((RABM_AT_SET_FASTDORM_PARA_CNF_STRU*)pstMsg);
            break;

        case ID_RABM_AT_QRY_FASTDORM_PARA_CNF:
            AT_ProcRabmQryFastDormParaCnf((RABM_AT_QRY_FASTDORM_PARA_CNF_STRU*)pstMsg);
            break;

        case ID_RABM_AT_SET_RELEASE_RRC_CNF:
            AT_ProcRabmReleaseRrcCnf((RABM_AT_RELEASE_RRC_CNF_STRU*)pstMsg);
            break;

#if (FEATURE_ON == FEATURE_HUAWEI_VP)
        case ID_RABM_AT_SET_VOICEPREFER_PARA_CNF:
            AT_ProcRabmSetVoicePreferParaCnf((RABM_AT_SET_VOICEPREFER_PARA_CNF_STRU*)pstMsg);
            break;

        case ID_RABM_AT_QRY_VOICEPREFER_PARA_CNF:
            AT_ProcRabmQryVoicePreferEnableParaCnf((RABM_AT_QRY_VOICEPREFER_PARA_CNF_STRU*)pstMsg);
            break;

        case ID_RABM_AT_VOICEPREFER_STATUS_REPORT:
            AT_ProcRabmVoicePreferStatusReport((RABM_AT_VOICEPREFER_STATUS_REPORT_STRU *)pstMsg);
            break;
#endif
        default:
            AT_WARN_LOG("At_RabmMsgProc:WARNING:Wrong Msg!");
            break;
    }
}



VOS_VOID At_TafAndDmsMsgProc(VOS_VOID *pMsg)
{
    if (AT_OK == At_CallMsgProc((MSG_HEADER_STRU *)pMsg))
    {
        return;
    }

    At_EventMsgProc((MN_AT_IND_EVT_STRU *)pMsg);

    return;
}


AT_MSG_PROC_FUNC At_GetMsgProcFunBySndPid(VOS_UINT32 ulSndPid)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulSndPidNo;

    ulSndPidNo = sizeof(g_astAtMsgProc) / sizeof(g_astAtMsgProc[0]);

    for (ulIndex = 0; ulIndex < ulSndPidNo; ulIndex++)
    {
        if (ulSndPid == g_astAtMsgProc[ulIndex].ulSndPid)
        {
            return g_astAtMsgProc[ulIndex].pProcMsgFunc; 
        }
    }

    return VOS_NULL_PTR;
}



TAF_VOID At_MsgProc(MsgBlock* pMsg)
{
    AT_MSG_PROC_FUNC                    pstProcFun = VOS_NULL_PTR;
    VOS_UINT32                          ulSendPid;
    VOS_UINT32                          ulMsgName;
    VOS_UINT32                          ulSliceStart;
    VOS_UINT32                          ulSliceEnd;
    VOS_UINT32                          ulType;

    ulSliceStart = VOS_GetSlice();
    ulSendPid    = pMsg->ulSenderPid;
    ulMsgName    = ((MSG_HEADER_STRU *)pMsg)->ulMsgName;

    /* 超时处理 */
    if ( VOS_PID_TIMER == pMsg->ulSenderPid )
    {
        At_TimeOutProc((REL_TIMER_MSG*)pMsg);
        ulSliceEnd = VOS_GetSlice();
        AT_RecordAtMsgInfo(ulSendPid, ulMsgName, ulSliceStart, ulSliceEnd);

        return;
    }

    if (WUEPS_PID_AT == pMsg->ulSenderPid)
    {
        ulType     = ((AT_MSG_STRU*)pMsg)->ucType;
        ulMsgName  = (ulType<<16) | ulMsgName;
    }

    if ((I0_WUEPS_PID_TAF == pMsg->ulSenderPid)
     || (I1_WUEPS_PID_TAF == pMsg->ulSenderPid)
     || (I2_WUEPS_PID_TAF == pMsg->ulSenderPid)
     || (I0_UEPS_PID_DSM  == pMsg->ulSenderPid)
     || (I1_UEPS_PID_DSM  == pMsg->ulSenderPid)
     || (I2_UEPS_PID_DSM  == pMsg->ulSenderPid))
    {
        ulMsgName = ((MN_AT_IND_EVT_STRU *)pMsg)->usMsgName;
    }

    pstProcFun = At_GetMsgProcFunBySndPid(pMsg->ulSenderPid);

    if (VOS_NULL_PTR == pstProcFun)
    {
        AT_LOG1("At_MsgProc other PID msg",pMsg->ulSenderPid);
    }
    else
    {
        pstProcFun(pMsg);
    }
    
    ulSliceEnd = VOS_GetSlice();
    AT_RecordAtMsgInfo(ulSendPid, ulMsgName, ulSliceStart, ulSliceEnd);

    return;
}


VOS_VOID AT_EventReport(
    VOS_UINT32                          ulPid,
    NAS_OM_EVENT_ID_ENUM_UINT16         enEventId,
    VOS_VOID                           *pPara,
    VOS_UINT32                          ulLen
)
{
    DIAG_EVENT_IND_STRU                 stDiagEvent;
    NAS_OM_EVENT_IND_STRUCT            *pstAtEvent = VOS_NULL_PTR;
    VOS_VOID                           *pData = pPara;
    VOS_UINT32                          ulAtEventMsgLen;
    VOS_UINT32                          ulRslt;

    /* 入参检查 */
    if ( (VOS_NULL_PTR == pData) && (ulLen != 0) )
    {
        /* 错误打印 */
        TAF_LOG(ulPid, VOS_NULL, PS_LOG_LEVEL_WARNING, "NAS_EventReport:pPara is NULL.");
        return;
    }

    /* 申请内存 */
    if (4 >= ulLen)
    {
        ulAtEventMsgLen = sizeof(NAS_OM_EVENT_IND_STRUCT);
        pstAtEvent = (NAS_OM_EVENT_IND_STRUCT*)PS_MEM_ALLOC(ulPid, ulAtEventMsgLen);
    }
    else
    {
        ulAtEventMsgLen = sizeof(NAS_OM_EVENT_IND_STRUCT) + ulLen -4;
        pstAtEvent = (NAS_OM_EVENT_IND_STRUCT*)PS_MEM_ALLOC(ulPid, ulAtEventMsgLen);
    }

    if (VOS_NULL_PTR == pstAtEvent)
    {
        TAF_LOG(ulPid, VOS_NULL, PS_LOG_LEVEL_WARNING, "NAS_EventReport:Alloc mem fail.");
        return;
    }

    /* 初始化 */
    TAF_MEM_SET_S(pstAtEvent, ulAtEventMsgLen, 0x00, ulAtEventMsgLen);

    /* 填写事件消息内容 */
    pstAtEvent->usEventId    = enEventId;
    pstAtEvent->usReserve    = 0;
    if ( (VOS_NULL_PTR != pData) && (ulLen > 0) )
    {
        TAF_MEM_CPY_S(pstAtEvent->aucData, ulLen, pData, ulLen);
    }

    /* 填写发给DIAG的结构体 */
    stDiagEvent.ulLength        = sizeof(NAS_OM_EVENT_IND_STRUCT) - 4 + ulLen;
    stDiagEvent.ulPid           = ulPid;
    stDiagEvent.ulEventId       = (VOS_UINT32)enEventId;
    stDiagEvent.ulModule        = DIAG_GEN_MODULE(MODEM_ID_0, DIAG_MODE_UMTS);
    stDiagEvent.pData           = pstAtEvent;

    ulRslt = DIAG_EventReport(&stDiagEvent);
    if (VOS_OK != ulRslt)
    {
        AT_WARN_LOG("AT_EventReport:OM_AcpuEvent Fail.");
    }

    /*lint -save -e516 */
    PS_MEM_FREE(ulPid, pstAtEvent);
    /*lint -restore */

    return;
}



VOS_UINT32 AT_RcvDrvAgentSetAdcRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                        *pRcvMsg;
    DRV_AGENT_ADC_SET_CNF_STRU                *pstAdcCnf;
    VOS_UINT8                                  ucIndex;
    VOS_UINT32                                 ulResult;

    /* 初始化消息 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstAdcCnf       = (DRV_AGENT_ADC_SET_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstAdcCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetAdcRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetAdcRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_ADC_SET */
    if (AT_CMD_ADC_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /*设置门限值是否成功 */
    if (VOS_TRUE != pstAdcCnf->bFail)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    /* 调用AT_FormATResultDATa发送命令结果 */
    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}



VOS_UINT32 AT_RcvDrvAgentQryTbatRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_TBAT_QRY_CNF_STRU        *stTbatTypeCnf;
    VOS_UINT8                           ucIndex;

    /* 初始化消息 */
    pRcvMsg                = (DRV_AGENT_MSG_STRU *)pMsg;
    stTbatTypeCnf          = (DRV_AGENT_TBAT_QRY_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(stTbatTypeCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryTbatRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /*自动应答开启情况下被叫死机问题*/
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryTbatRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_TBAT_QRY */
    if (AT_CMD_TBAT_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s:%d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    stTbatTypeCnf->ulTbatType);

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, AT_OK);
    return VOS_OK;

}



VOS_UINT32 AT_RcvSimLockQryRsp(VOS_VOID *pMsg)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;
    AT_MMA_SIMLOCK_STATUS_STRU         *pstEvent;

    pstEvent = (AT_MMA_SIMLOCK_STATUS_STRU *)pMsg;

    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvSimLockQryRsp: AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_SIMLOCKSTATUS_READ或AT_CMD_CSDFLT_READ */
    if ((AT_CMD_SIMLOCKSTATUS_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_CSDFLT_READ != gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    if (AT_CMD_SIMLOCKSTATUS_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s:%d",
                                                        "^SIMLOCK",
                                                        pstEvent->bSimlockEnableFlg);
        ulRet                  = AT_OK;

    }
    else
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;
        ulRet                  = AT_OutputCsdfltDefault(ucIndex, pstEvent->bSimlockEnableFlg);
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulRet);

    return VOS_OK;
}


#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#if (VOS_WIN32 == VOS_OS_VER)

VOS_UINT32 WuepsATFidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ulRslt = VOS_OK;

    switch( ip )
    {
        case   VOS_IP_LOAD_CONFIG:
        #if (VOS_WIN32 == VOS_OS_VER)
            ulRslt = VOS_RegisterPIDInfo(WUEPS_PID_AT,
                                           (Init_Fun_Type)At_PidInit,
                                           (Msg_Fun_Type)At_MsgProc);

            if (VOS_OK != ulRslt)
            {
                return VOS_ERR;
            }
        #endif

            ulRslt = VOS_RegisterMsgTaskPrio(WUEPS_FID_AT, VOS_PRIORITY_P6);

            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            break;
        default:
            break;
    }

    return VOS_OK;
}
#endif
#endif

#if (FEATURE_ON == FEATURE_SECURITY_SHELL)

VOS_UINT32 AT_RcvDrvAgentSetSpwordRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                        *pRcvMsg;
    DRV_AGENT_SPWORD_SET_CNF_STRU             *pstSpwordSetCnf;
    VOS_UINT8                                  ucIndex;

    /* 初始化消息 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstSpwordSetCnf = (DRV_AGENT_SPWORD_SET_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstSpwordSetCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("pstSpwordSetCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("pstSpwordSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_SPWORD_SET */
    if (AT_CMD_SPWORD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 密码校验通过，错误次数需要清零 */
    if (VOS_OK == pstSpwordSetCnf->ulResult)
    {
        g_stSpWordCtx.ucShellPwdCheckFlag = VOS_TRUE;
        g_stSpWordCtx.ucErrTimes          = 0;

        /* 调用At_FormatResultData发送命令结果 */
        At_FormatResultData(ucIndex, AT_OK);
    }
    /* 密码校验失败，错误次数加1 */
    else
    {
        g_stSpWordCtx.ucErrTimes++;

        /* 调用At_FormatResultData发送命令结果 */
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}

#endif

VOS_UINT32 AT_RcvMmaCipherInfoQueryCnf(VOS_VOID *pMsg)
{
    TAF_MMA_CIPHER_QRY_CNF_STRU        *pstCipherQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstCipherQryCnf     = (TAF_MMA_CIPHER_QRY_CNF_STRU *)pMsg;
    ulResult            = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCipherQryCnf->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCipherInfoQueryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCipherInfoQueryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CIPERQRY_READ */
    if (AT_CMD_CIPERQRY_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CIPERQRY查询命令返回 */
    gstAtSendData.usBufLen = 0;
    if (VOS_OK != pstCipherQryCnf->ulResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR*)pgucAtSndCodeAddr,
                                                   "%s:%d",
                                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                   pstCipherQryCnf->ucCipherInfo);
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}


VOS_UINT32 AT_RcvMmaLocInfoQueryCnf(VOS_VOID *pMsg)
{
    TAF_MMA_LOCATION_INFO_QRY_CNF_STRU *pstLocInfoCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstLocInfoCnf       = (TAF_MMA_LOCATION_INFO_QRY_CNF_STRU *)pMsg;
    usLength            = 0;
    ulResult            = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstLocInfoCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaLocInfoQueryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaLocInfoQueryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_LOCINFO_READ */
    if (AT_CMD_LOCINFO_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CIPERQRY查询命令返回 */
    if (VOS_OK != pstLocInfoCnf->ulResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          "%s:",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        /* 上报MCC和MNC */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%X%X%X",
                                          (VOS_INT32)(pstLocInfoCnf->ulMcc & 0x0f) ,
                                          (VOS_INT32)(pstLocInfoCnf->ulMcc & 0x0f00) >> 8,
                                          (VOS_INT32)(pstLocInfoCnf->ulMcc & 0x0f0000) >> 16);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%X%X",
                                          (VOS_INT32)(pstLocInfoCnf->ulMnc & 0x0f) ,
                                          (VOS_INT32)(pstLocInfoCnf->ulMnc & 0x0f00) >> 8);

        /* MNC最后一位不等于F，则MNC为三位，否则为两位 */
        if (0x0f != ((pstLocInfoCnf->ulMnc & 0x0f0000) >> 16))
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%X",
                                          (VOS_INT32)(pstLocInfoCnf->ulMnc & 0x0f0000) >> 16);
        }

        /* Lac */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                          ",0x%X",
                                          pstLocInfoCnf->ulLac);

        /* Rac */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                          ",0x%X",
                                          pstLocInfoCnf->ucRac);

        /* Cell ID */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                          ",0x%X",
                                          pstLocInfoCnf->ulCellid);

        gstAtSendData.usBufLen = usLength;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentNvBackupStatQryRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                     *pRcvMsg;
    DRV_AGENT_NVBACKUPSTAT_QRY_CNF_STRU    *pstEvent;
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulResult;
    VOS_UINT16                              usLength;

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_NVBACKUPSTAT_QRY_CNF_STRU *)pRcvMsg->aucContent;
    usLength        = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentNvBackupStatQryRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentNvBackupStatQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_NVBACKUPSTAT_READ */
    if ( AT_CMD_NVBACKUPSTAT_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_NO_ERROR == pstEvent->ulResult )
    {
        if ( NV_OK == pstEvent->ulNvBackupStat )
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s: %d",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                               VOS_TRUE);
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s: %d",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                               VOS_FALSE);
        }

        ulResult    = AT_OK;
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = usLength;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentNandBadBlockQryRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                     *pRcvMsg;
    DRV_AGENT_NANDBBC_QRY_CNF_STRU         *pstEvent;
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulResult;
    VOS_UINT16                              usLength;
    VOS_UINT32                              uli;
    VOS_UINT32                              ulMaxBBNum;

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_NANDBBC_QRY_CNF_STRU *)pRcvMsg->aucContent;
    usLength        = 0;
    uli             = 0;
    ulMaxBBNum      = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentNandBadBlockQryRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentNandBadBlockQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_NANDBBC_READ */
    if ( AT_CMD_NANDBBC_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_NO_ERROR == pstEvent->ulResult )
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: %d",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           pstEvent->ulBadBlockNum);

        ulMaxBBNum = (pstEvent->ulBadBlockNum > DRV_AGENT_NAND_BADBLOCK_MAX_NUM) ? DRV_AGENT_NAND_BADBLOCK_MAX_NUM : pstEvent->ulBadBlockNum;
        for (uli = 0; uli < ulMaxBBNum; uli++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s%s: %d",
                                               gaucAtCrLf,
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                               pstEvent->aulBadBlockIndex[uli]);
        }

        ulResult    = AT_OK;
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = usLength;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentNandDevInfoQryRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                     *pRcvMsg;
    DRV_AGENT_NANDVER_QRY_CNF_STRU         *pstEvent;
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulResult;
    VOS_UINT16                              usLength;

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_NANDVER_QRY_CNF_STRU *)pRcvMsg->aucContent;
    usLength        = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentNandDevInfoQryRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentNandDevInfoQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_NANDVER_READ */
    if ( AT_CMD_NANDVER_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_NO_ERROR == pstEvent->ulResult )
    {
        /* 输出查询结果 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: %d,\"%s\",%d,\"%s\"",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           pstEvent->stNandDevInfo.ulMufId,
                                           pstEvent->stNandDevInfo.aucMufName,
                                           pstEvent->stNandDevInfo.ulDevId,
                                           pstEvent->stNandDevInfo.aucDevSpec);

        ulResult    = AT_OK;
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = usLength;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentChipTempQryRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_CHIPTEMP_QRY_CNF_STRU    *pstEvent;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_CHIPTEMP_QRY_CNF_STRU *)pRcvMsg->aucContent;
    usLength        = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentChipTempQryRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentChipTempQryRsp: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_CHIPTEMP_READ */
    if ( AT_CMD_CHIPTEMP_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_NO_ERROR == pstEvent->ulResult )
    {
        /* 输出查询结果 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: %d,%d,%d,%d,%d",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           pstEvent->lGpaTemp,
                                           pstEvent->lWpaTemp,
                                           pstEvent->lLpaTemp,
                                           pstEvent->lSimTemp,
                                           pstEvent->lBatTemp);

        ulResult    = AT_OK;
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = usLength;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}






VOS_UINT32 AT_RcvMmaOmMaintainInfoInd(
    VOS_VOID                           *pstMsg
)
{
    ATMMA_OM_MAINTAIN_INFO_IND_STRU    *pstOmMaintainInfo;

    pstOmMaintainInfo = (ATMMA_OM_MAINTAIN_INFO_IND_STRU*)pstMsg;

    if ( (VOS_TRUE == pstOmMaintainInfo->ucOmConnectFlg)
      && (VOS_TRUE == pstOmMaintainInfo->ucOmPcRecurEnableFlg) )
    {
        AT_MNTN_TraceContextData();
        AT_MNTN_TraceClientData();
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentSetMaxLockTmsRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                        *pRcvMsg;
    DRV_AGENT_MAX_LOCK_TMS_SET_CNF_STRU       *pstMaxlockTmsSetCnf;
    VOS_UINT8                                  ucIndex;

    /* 初始化消息 */
    pRcvMsg             = (DRV_AGENT_MSG_STRU *)pMsg;
    pstMaxlockTmsSetCnf = (DRV_AGENT_MAX_LOCK_TMS_SET_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMaxlockTmsSetCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetMaxLockTmsRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetMaxLockTmsRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_MAXLCKTMS_SET */
    if (AT_CMD_MAXLCKTMS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^MAXLCKTMS设置命令返回 */
    if (VOS_OK != pstMaxlockTmsSetCnf->ulResult)
    {
        /* 调用At_FormatResultData发送命令结果 */
        At_FormatResultData(ucIndex, AT_DEVICE_OTHER_ERROR);
    }
    else
    {
        /* 调用At_FormatResultData发送命令结果 */
        At_FormatResultData(ucIndex, AT_OK);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentSetApSimstRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                        *pRcvMsg;
    DRV_AGENT_AP_SIMST_SET_CNF_STRU           *pstApSimStSetCnf;
    VOS_UINT8                                  ucIndex;

    /* 初始化消息 */
    pRcvMsg             = (DRV_AGENT_MSG_STRU *)pMsg;
    pstApSimStSetCnf    = (DRV_AGENT_AP_SIMST_SET_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstApSimStSetCnf->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetApSimstRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetApSimstRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_APSIMST_SET */
    if (AT_CMD_APSIMST_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^APSIMST设置命令返回 */
    if (VOS_OK != pstApSimStSetCnf->ulResult)
    {
        /* 调用At_FormatResultData发送命令结果 */
        At_FormatResultData(ucIndex, AT_ERROR);
    }
    else
    {
        /* 调用At_FormatResultData发送命令结果 */
        At_FormatResultData(ucIndex, AT_OK);
    }

    return VOS_OK;
}



VOS_UINT32 AT_RcvDrvAgentHukSetCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                     *pRcvMsg;
    DRV_AGENT_HUK_SET_CNF_STRU             *pstEvent;
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulResult;

    AT_PR_LOGI("enter");

    /* 初始化 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_HUK_SET_CNF_STRU *)pRcvMsg->aucContent;
    ucIndex         = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    ulResult        = VOS_NULL;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentHukSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentHukSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_HUK_SET */
    if (AT_CMD_HUK_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if (DRV_AGENT_PERSONALIZATION_NO_ERROR == pstEvent->enResult)
    {
        /* 输出设置结果 */
        ulResult    = AT_OK;
    }
    else
    {
        /* 异常情况, 转换错误码 */
        ulResult    = AT_PERSONALIZATION_ERR_BEGIN + pstEvent->enResult;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentFacAuthPubkeySetCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                     *pRcvMsg;
    DRV_AGENT_FACAUTHPUBKEY_SET_CNF_STRU   *pstEvent;
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulResult;

    AT_PR_LOGI("enter");

    /* 初始化 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_FACAUTHPUBKEY_SET_CNF_STRU *)pRcvMsg->aucContent;
    ucIndex         = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    ulResult        = VOS_NULL;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentFacAuthPubkeySetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentFacAuthPubkeySetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_FACAUTHPUBKEY_SET */
    if (AT_CMD_FACAUTHPUBKEYEX_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_PERSONALIZATION_NO_ERROR == pstEvent->enResult )
    {
        /* 输出设置结果 */
        ulResult    = AT_OK;
    }
    else
    {
        /* 异常情况, 转换错误码 */
        ulResult    = AT_PERSONALIZATION_ERR_BEGIN + pstEvent->enResult;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentIdentifyStartSetCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                     *pRcvMsg;
    DRV_AGENT_IDENTIFYSTART_SET_CNF_STRU   *pstEvent;
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulResult;
    VOS_UINT16                              usLength;
    VOS_UINT32                              i;

    AT_PR_LOGI("enter");

    /* 初始化 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_IDENTIFYSTART_SET_CNF_STRU *)pRcvMsg->aucContent;
    ucIndex         = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    usLength        = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentIdentifyStartSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentIdentifyStartSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_IDENTIFYSTART_SET */
    if ( AT_CMD_IDENTIFYSTART_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_PERSONALIZATION_NO_ERROR == pstEvent->enResult )
    {
        /* 输出设置结果 */
        ulResult    = AT_OK;
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          "%s: ",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        for (i = 0; i < DRV_AGENT_RSA_CIPHERTEXT_LEN; i++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%02X",
                                          pstEvent->aucRsaText[i]);
        }
    }
    else
    {
        /* 异常情况, 转换错误码 */
        ulResult    = AT_PERSONALIZATION_ERR_BEGIN + pstEvent->enResult;
    }

    gstAtSendData.usBufLen = usLength;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentIdentifyEndSetCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                     *pRcvMsg;
    DRV_AGENT_IDENTIFYEND_SET_CNF_STRU     *pstEvent;
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulResult;

    AT_PR_LOGI("enter");

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_IDENTIFYEND_SET_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentIdentifyEndSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentIdentifyEndSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_IDENTIFYEND_SET */
    if ( AT_CMD_IDENTIFYEND_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_PERSONALIZATION_NO_ERROR == pstEvent->enResult )
    {
        /* 输出设置结果 */
        ulResult    = AT_OK;
    }
    else
    {
        /* 异常情况, 转换错误码 */
        ulResult    = AT_PERSONALIZATION_ERR_BEGIN + pstEvent->enResult;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentSimlockDataWriteSetCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                         *pRcvMsg;
    DRV_AGENT_SIMLOCKDATAWRITE_SET_CNF_STRU    *pstEvent;
    VOS_UINT8                                   ucIndex;
    VOS_UINT32                                  ulResult;

    AT_PR_LOGI("enter");

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_SIMLOCKDATAWRITE_SET_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentSimlockDataWriteSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSimlockDataWriteSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_SIMLOCKDATAWRITE_SET */
    if ( AT_CMD_SIMLOCKDATAWRITE_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_PERSONALIZATION_NO_ERROR == pstEvent->enResult )
    {
        /* 输出设置结果 */
        ulResult    = AT_OK;
    }
    else
    {
        /* 异常情况, 转换错误码 */
        ulResult    = AT_PERSONALIZATION_ERR_BEGIN + pstEvent->enResult;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_CHAR AT_Num2AsciiNum(VOS_UINT8 ucNum)
{
    if (9 >= ucNum)
    {
        return (VOS_CHAR)('0' + ucNum);
    }
    else if (0x0F >= ucNum)
    {
        return (VOS_CHAR)('A' + (ucNum - 0x0A));
    }
    else
    {
        return '*';
    }
}


VOS_UINT32 AT_CheckSimlockCodeLast2Char(
    DRV_AGENT_PERSONALIZATION_CATEGORY_ENUM_UINT8   enCategory,
    VOS_CHAR                                       *pcStrLast2Char
)
{
    VOS_UINT8                           i           = 0;

    /* 检测最后两位的有效性, NET无需检测 */
    if (DRV_AGENT_PERSONALIZATION_CATEGORY_NETWORK < enCategory)
    {
        for (i = 0; i < 2; i++)
        {
            /* NETSUB支持范围: 0x00~0x99 */
            if ( ('0' <= pcStrLast2Char[i]) && ('9' >= pcStrLast2Char[i]) )
            {
                continue;
            }
            /* SP支持范围: 0x00~0xFF */
            else if ( (DRV_AGENT_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER == enCategory)
                   && ('A' <= pcStrLast2Char[i]) && ('F' >= pcStrLast2Char[i]) )
            {
                continue;
            }
            else
            {
                return VOS_ERR;
            }
        }
    }

    return VOS_OK;

}



VOS_UINT32 AT_CheckSimlockCodeStr(
    DRV_AGENT_PERSONALIZATION_CATEGORY_ENUM_UINT8   enCategory,
    VOS_CHAR                                       *pcStrCode
)
{
    VOS_UINT8                           i           = 0;
    VOS_UINT8                           j           = 0;
    VOS_UINT8                           ucLen;              /* 锁网号码长度 */
    VOS_CHAR                           *pcTmpStr;

    pcTmpStr    = pcStrCode;

    switch(enCategory)
    {
        case DRV_AGENT_PERSONALIZATION_CATEGORY_NETWORK:
            ucLen = AT_PERSONALIZATION_NET_CODE_LEN;
            break;
        case DRV_AGENT_PERSONALIZATION_CATEGORY_CORPORATE:
            ucLen = AT_PERSONALIZATION_CP_CODE_LEN;
            break;
        default:
            ucLen = AT_PERSONALIZATION_CODE_LEN;
            break;
    }

    /* 检测PLMN合法性及其位数 */
    for (i = 0; i < AT_PERSONALIZATION_NET_CODE_LEN; i++)
    {
        if ( ('0' <= pcTmpStr[i]) && ('9' >= pcTmpStr[i]) )
        {
            continue;
        }
        else if ( ((AT_PERSONALIZATION_NET_CODE_LEN - 1) == i)
               && ('F' == pcTmpStr[i]) )
        {
            for (j = i + 1; j < ucLen; j++)
            {
                pcTmpStr[j - 1] = pcTmpStr[j];
            }
            ucLen = ucLen - 1;
            break;
        }
        else
        {
            return VOS_ERR;
        }
    }

    pcTmpStr[ucLen] = 0;

    /* 检测锁网锁卡号码最后两位的合法性 */
    if (VOS_OK != AT_CheckSimlockCodeLast2Char(enCategory, &pcTmpStr[ucLen - 2]))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_SimlockCodeBcd2Str(
    DRV_AGENT_PERSONALIZATION_CATEGORY_ENUM_UINT8   enCategory,
    VOS_UINT8                                      *pucBcdNum,
    VOS_CHAR                                       *pcStrNum
)
{
    VOS_UINT8                           i               = 0;
    VOS_UINT8                           ucDecodeLen;
    VOS_UINT8                           ucFirstNum;
    VOS_UINT8                           ucSecondNum;
    VOS_CHAR                           *pcStrTmp        = pcStrNum;
    VOS_CHAR                            cTmpChar;

    switch(enCategory)
    {
        case DRV_AGENT_PERSONALIZATION_CATEGORY_NETWORK:
            ucDecodeLen = AT_PERSONALIZATION_NET_CODE_BCD_LEN;
            break;
        case DRV_AGENT_PERSONALIZATION_CATEGORY_NETWORK_SUBSET:
            ucDecodeLen = AT_PERSONALIZATION_SUBNET_CODE_BCD_LEN;
            break;
        case DRV_AGENT_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER:
            ucDecodeLen = AT_PERSONALIZATION_SP_CODE_BCD_LEN;
            break;
        case DRV_AGENT_PERSONALIZATION_CATEGORY_CORPORATE:
            ucDecodeLen = AT_PERSONALIZATION_CP_CODE_BCD_LEN;
            break;
        default:
            return VOS_ERR;
    }

    /* 对锁网锁卡号码进行高低字节转换 */
    for (i = 0; i < ucDecodeLen; i++)
    {
        /* 分别取出高低字节 */
        ucFirstNum  = (VOS_UINT8)((*(pucBcdNum + i)) & 0x0F);
        ucSecondNum = (VOS_UINT8)(((*(pucBcdNum + i)) >> 4) & 0x0F);

        *pcStrTmp++ = AT_Num2AsciiNum(ucFirstNum);
        *pcStrTmp++ = AT_Num2AsciiNum(ucSecondNum);
    }

    /* 将第四位号码后移到第六位(与产线对接) */
    pcStrTmp        = &pcStrNum[AT_PERSONALIZATION_CODE_FOURTH_CHAR_INDEX];
    cTmpChar        = pcStrTmp[0];
    pcStrTmp[0]     = pcStrTmp[1];
    pcStrTmp[1]     = pcStrTmp[2];
    pcStrTmp[2]     = cTmpChar;

    /* 对高低字节转换后的锁网锁卡号码进行合法性检查 */
    if (VOS_OK != AT_CheckSimlockCodeStr(enCategory, pcStrNum))
    {
        return VOS_ERR;
    }

    return VOS_OK;

}

VOS_UINT32 AT_PhoneSimlockInfoPrint(
    DRV_AGENT_PHONESIMLOCKINFO_QRY_CNF_NEW_STRU            *pstPhoneSimlockInfo,
    VOS_UINT8                                               ucIndex,
    VOS_UINT16                                             *pusLength
)
{
    VOS_UINT16                                  usLength;
    VOS_CHAR                                    acCodeBegin[AT_PERSONALIZATION_CODE_LEN_EX + 1];
    VOS_CHAR                                    acCodeEnd[AT_PERSONALIZATION_CODE_LEN_EX + 1];
    VOS_UINT32                                  ulCodeBeginRslt;
    VOS_UINT32                                  ulCodeEndRslt;
    VOS_UINT32                                  ulPrintGroupNum;
    VOS_UINT8                                   ucCatIndex;
    VOS_UINT8                                   ucGroupIndex;
    VOS_UINT8                                   ucCatNum;

    /* 局部变量初始化 */
    usLength        = *pusLength;
    ucCatIndex      = 0;
    ucGroupIndex    = 0;

    TAF_MEM_SET_S(acCodeBegin, sizeof(acCodeBegin), 0x00,   (AT_PERSONALIZATION_CODE_LEN_EX + 1));
    TAF_MEM_SET_S(acCodeEnd, sizeof(acCodeEnd), 0x00,   (AT_PERSONALIZATION_CODE_LEN_EX + 1));
    ulCodeBeginRslt = VOS_NULL;
    ulCodeEndRslt   = VOS_NULL;
    ucCatNum        = AT_MIN(pstPhoneSimlockInfo->ucSupportCategoryNum, DRV_AGENT_SUPPORT_CATEGORY_NUM_EXTERED);

    for (ucCatIndex = 0; ucCatIndex < ucCatNum; ucCatIndex++)
    {
        if (0 != ucCatIndex)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s",
                                               gaucAtCrLf);
        }

        /* ^PHONESIMLOCKINFO: <cat>,<indicator> [,<total_group_num>,<flag>,(<code_begin>,<code_end>)...] */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: ",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        switch (pstPhoneSimlockInfo->astCategoryInfo[ucCatIndex].enCategory)
        {
            case DRV_AGENT_PERSONALIZATION_CATEGORY_NETWORK:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "NET");
                break;
            case DRV_AGENT_PERSONALIZATION_CATEGORY_NETWORK_SUBSET:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "NETSUB");
                break;
            case DRV_AGENT_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "SP");
                break;
            case DRV_AGENT_PERSONALIZATION_CATEGORY_CORPORATE:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "CP");
                break;

            case DRV_AGENT_PERSONALIZATION_CATEGORY_SIM_USIM:
            default:
                return VOS_ERR;
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",%d",
                                           pstPhoneSimlockInfo->astCategoryInfo[ucCatIndex].enIndicator);

        if (DRV_AGENT_PERSONALIZATION_INDICATOR_ACTIVE == pstPhoneSimlockInfo->astCategoryInfo[ucCatIndex].enIndicator)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",%d",
                                           pstPhoneSimlockInfo->astCategoryInfo[ucCatIndex].ulGroupNum);

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",%d",
                                           pstPhoneSimlockInfo->astCategoryInfo[ucCatIndex].ucFlag);

            /* 最多打印上报10组号段 */
            ulPrintGroupNum = AT_MIN(pstPhoneSimlockInfo->astCategoryInfo[ucCatIndex].ulGroupNum, DRV_AGENT_PH_LOCK_CODE_GROUP_NUM_EXTERED);

            for (ucGroupIndex = 0; ucGroupIndex < ulPrintGroupNum; ucGroupIndex++)
            {
                ulCodeBeginRslt = AT_SimlockCodeBcd2Str(pstPhoneSimlockInfo->astCategoryInfo[ucCatIndex].enCategory,
                                                        pstPhoneSimlockInfo->astCategoryInfo[ucCatIndex].astLockCode[ucGroupIndex].aucPhLockCodeBegin,
                                                        acCodeBegin);
                ulCodeEndRslt   = AT_SimlockCodeBcd2Str(pstPhoneSimlockInfo->astCategoryInfo[ucCatIndex].enCategory,
                                                        pstPhoneSimlockInfo->astCategoryInfo[ucCatIndex].astLockCode[ucGroupIndex].aucPhLockCodeEnd,
                                                        acCodeEnd);
                if ( (VOS_OK == ulCodeBeginRslt)
                  && (VOS_OK == ulCodeEndRslt) )
                {
                    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                       ",(%s,%s)",
                                                       acCodeBegin,
                                                       acCodeEnd);
                }
                else
                {
                    return VOS_ERR;
                }
            }
        }

    }
    *pusLength = usLength;
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentPhoneSimlockInfoQryCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                                     *pRcvMsg;
    DRV_AGENT_PHONESIMLOCKINFO_QRY_CNF_NEW_STRU            *pstEvent;
    VOS_UINT8                                               ucIndex;
    VOS_UINT32                                              ulResult;
    VOS_UINT16                                              usLength;

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;

    pstEvent        = (DRV_AGENT_PHONESIMLOCKINFO_QRY_CNF_NEW_STRU *)pRcvMsg->aucContent;

    ucIndex         = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    usLength        = 0;

    AT_PR_LOGI("enter");

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentPhoneSimlockInfoQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentPhoneSimlockInfoQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_PHONESIMLOCKINFO_READ */
    if ( AT_CMD_PHONESIMLOCKINFO_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_PERSONALIZATION_NO_ERROR == pstEvent->enResult )
    {
        /* 输出设置结果 */
        ulResult    = AT_OK;
        /* 打印结果 */
        if (VOS_OK != AT_PhoneSimlockInfoPrint(pstEvent, ucIndex, &usLength))
        {
            ulResult = AT_PERSONALIZATION_OTHER_ERROR;
        }
    }
    else
    {
        /* 异常情况, 转换错误码 */
        ulResult    = AT_PERSONALIZATION_ERR_BEGIN + pstEvent->enResult;
    }

    gstAtSendData.usBufLen = usLength;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_SimlockDataReadPrint(
    DRV_AGENT_SIMLOCKDATAREAD_QRY_CNF_STRU      stSimlockDataRead,
    VOS_UINT8                                   ucIndex,
    VOS_UINT16                                 *pusLength
)
{
    VOS_UINT16                                  usLength        = *pusLength;
    VOS_UINT8                                   ucCatIndex      = 0;
    VOS_UINT8                                   ucCatNum;

    ucCatNum = AT_MIN(stSimlockDataRead.ucSupportCategoryNum, DRV_AGENT_SUPPORT_CATEGORY_NUM);

    for (ucCatIndex = 0; ucCatIndex < ucCatNum; ucCatIndex++)
    {
        if (0 != ucCatIndex)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s",
                                               gaucAtCrLf);
        }

        /* ^SIMLOCKDATAREAD: <cat>,<indicator>,<lock_status>,<max_times>,<remain_times> */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: ",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        switch(stSimlockDataRead.astCategoryData[ucCatIndex].enCategory)
        {
            case DRV_AGENT_PERSONALIZATION_CATEGORY_NETWORK:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "NET");
                break;
            case DRV_AGENT_PERSONALIZATION_CATEGORY_NETWORK_SUBSET:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "NETSUB");
                break;
            case DRV_AGENT_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "SP");
                break;
            case DRV_AGENT_PERSONALIZATION_CATEGORY_CORPORATE:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "CP");
                break;
            case DRV_AGENT_PERSONALIZATION_CATEGORY_SIM_USIM:
            default:
                return VOS_ERR;
        }
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",%d",
                                           stSimlockDataRead.astCategoryData[ucCatIndex].enIndicator);
        if (DRV_AGENT_PERSONALIZATION_INDICATOR_ACTIVE == stSimlockDataRead.astCategoryData[ucCatIndex].enIndicator)
        {
            switch (stSimlockDataRead.astCategoryData[ucCatIndex].enStatus)
            {
            case DRV_AGENT_PERSONALIZATION_STATUS_READY:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   ",READY,,");
                break;
            case DRV_AGENT_PERSONALIZATION_STATUS_PIN:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   ",PIN,%d,%d",
                                                   stSimlockDataRead.astCategoryData[ucCatIndex].ucMaxUnlockTimes,
                                                   stSimlockDataRead.astCategoryData[ucCatIndex].ucRemainUnlockTimes);
                break;
            case DRV_AGENT_PERSONALIZATION_STATUS_PUK:
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   ",PUK,,");
                break;
            default:
                return VOS_ERR;
            }
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               ",,,");
        }

    }
    *pusLength = usLength;
    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentSimlockDataReadQryCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                         *pRcvMsg;
    DRV_AGENT_SIMLOCKDATAREAD_QRY_CNF_STRU     *pstEvent;
    VOS_UINT8                                   ucIndex;
    VOS_UINT8                                   ucSimlockType;
    VOS_UINT32                                  ulResult;
    VOS_UINT16                                  usLength;

    AT_PR_LOGI("enter");

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_SIMLOCKDATAREAD_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ucIndex         = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    ucSimlockType   = (VOS_UINT8)AT_GetSimlockUnlockCategoryFromClck();
    usLength        = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentSimlockDataReadQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSimlockDataReadQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if ( AT_CMD_SIMLOCKDATAREAD_READ != gastAtClientTab[ucIndex].CmdCurrentOpt
      && AT_CMD_CLCK_SIMLOCKDATAREAD != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_PERSONALIZATION_NO_ERROR == pstEvent->enResult )
    {
        /* 输出设置结果 */
        ulResult    = AT_OK;
        /* 打印结果 */
        if ( AT_CMD_CLCK_SIMLOCKDATAREAD != gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            if ( VOS_OK != AT_SimlockDataReadPrint(*pstEvent, ucIndex, &usLength))
            {
                ulResult = AT_PERSONALIZATION_OTHER_ERROR;
            }
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s: %d",
                                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                   pstEvent->astCategoryData[ucSimlockType].enIndicator);
        }
    }
    else
    {
        /* 异常情况, 转换错误码 */
        ulResult    = AT_PERSONALIZATION_ERR_BEGIN + pstEvent->enResult;
    }

    gstAtSendData.usBufLen = usLength;

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentPhonePhynumSetCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_PHONEPHYNUM_SET_CNF_STRU *pstEvent;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_PHONEPHYNUM_SET_CNF_STRU *)pRcvMsg->aucContent;

    AT_PR_LOGI("enter");

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentPhonePhynumSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentPhonePhynumSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_PHONEPHYNUM_SET */
    if ( AT_CMD_PHONEPHYNUM_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_PERSONALIZATION_NO_ERROR == pstEvent->enResult )
    {
        /* 输出设置结果 */
        ulResult    = AT_OK;
    }
    else
    {
        /* 异常情况, 转换错误码 */
        ulResult    = AT_PERSONALIZATION_ERR_BEGIN + pstEvent->enResult;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentPhonePhynumQryCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_PHONEPHYNUM_QRY_CNF_STRU *pstEvent;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_UINT32                          i;

    AT_PR_LOGI("enter");

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_PHONEPHYNUM_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ucIndex         = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    usLength        = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentPhonePhynumQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentPhonePhynumQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_PHONEPHYNUM_READ */
    if ( AT_CMD_PHONEPHYNUM_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_PERSONALIZATION_NO_ERROR == pstEvent->enResult )
    {
        ulResult    = AT_OK;
        /* 格式化输出查询结果 */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          "%s: IMEI,",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        for (i = 0; i < DRV_AGENT_RSA_CIPHERTEXT_LEN; i++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%02X",
                                          pstEvent->aucImeiRsa[i]);
        }
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s%s: SN,",
                                           gaucAtCrLf,
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        for (i = 0; i < DRV_AGENT_RSA_CIPHERTEXT_LEN; i++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%02X",
                                          pstEvent->aucSnRsa[i]);
        }
    }
    else
    {
        /* 异常情况, 转换错误码 */
        ulResult    = AT_PERSONALIZATION_ERR_BEGIN + pstEvent->enResult;
    }

    gstAtSendData.usBufLen = usLength;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvDrvAgentOpwordSetCnf(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                      *pRcvMsg;
    DRV_AGENT_OPWORD_SET_CNF_STRU           *pstEvent;
    VOS_UINT8                                ucIndex;
    VOS_UINT32                               ulResult;

    /* 初始化消息变量 */
    pRcvMsg         = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent        = (DRV_AGENT_OPWORD_SET_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvDrvAgentOpwordSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentOpwordSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_OPWORD_SET */
    if ( AT_CMD_OPWORD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( DRV_AGENT_PERSONALIZATION_NO_ERROR == pstEvent->enResult )
    {
        /* 密码比对成功，获取权限 */
        g_enATE5RightFlag   = AT_E5_RIGHT_FLAG_YES;
        ulResult            = AT_OK;
    }
    else
    {
        /* 输出设置结果, 转换错误码 */
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}




VOS_UINT32 AT_RcvMtaCposSetCnf(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                  *pstCposMsg;
    MTA_AT_CPOS_CNF_STRU             *pstEvent;
    VOS_UINT8                         ucIndex;
    VOS_UINT32                        ulResult;

    /* 初始化消息变量 */
    pstCposMsg  = (AT_MTA_MSG_STRU*)pMsg;
    pstEvent    = (MTA_AT_CPOS_CNF_STRU*)pstCposMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstCposMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaCposSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCposSetCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_CPOS_SET */
    if ( AT_CMD_CPOS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_RcvMtaCposSetCnf: WARNING:Not AT_CMD_CPOS_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstEvent->enResult )
    {
        ulResult            = AT_OK;
    }
    else
    {
        ulResult            = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormatResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_VOID AT_ReportCposrInd(
    VOS_UINT8                           ucIndex,
    VOS_CHAR                           *pcXmlText
)
{
    VOS_UINT16                          usLen;
    VOS_UINT8                           ucFinalUsbIndex;
#if (FEATURE_ON == FEATURE_VCOM_EXT)
    VOS_UINT8                           ucFinalVcomIndex;
    MODEM_ID_ENUM_UINT16                enModemId;
#endif

    ucFinalUsbIndex     = AT_CLIENT_TAB_PCUI_INDEX;
#if (FEATURE_ON == FEATURE_VCOM_EXT)
    ucFinalVcomIndex    = AT_CLIENT_TAB_APP9_INDEX;
    enModemId           = MODEM_ID_0;

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_ReportCposrInd: Get modem id fail.");
        return;
    }

    switch (enModemId)
    {
        case MODEM_ID_0:
            ucFinalUsbIndex     = AT_CLIENT_TAB_PCUI_INDEX;
            ucFinalVcomIndex    = AT_CLIENT_TAB_APP9_INDEX;
            break;
        case MODEM_ID_1:
            ucFinalUsbIndex     = AT_CLIENT_TAB_CTRL_INDEX;
            ucFinalVcomIndex    = AT_CLIENT_TAB_APP12_INDEX;
            break;
        case MODEM_ID_2:
            ucFinalUsbIndex     = AT_CLIENT_TAB_PCUI2_INDEX;
            ucFinalVcomIndex    = AT_CLIENT_TAB_APP24_INDEX;
            break;
        default:
            AT_ERR_LOG("AT_ReportCposrInd: Invalid modem id!");
            return;
    }
#endif

    usLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   "%s+CPOSR: %s%s",
                                   gaucAtCrLf, pcXmlText, gaucAtCrLf);

    At_SendResultData(ucFinalUsbIndex, pgucAtSndCodeAddr, usLen);
#if (FEATURE_ON == FEATURE_VCOM_EXT)
    At_SendResultData(ucFinalVcomIndex, pgucAtSndCodeAddr, usLen);
#endif

    return;
}


VOS_UINT32 AT_RcvMtaCposrInd(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstCposrMsg;
    MTA_AT_CPOSR_IND_STRU              *pstEvent;
    AT_MODEM_AGPS_CTX_STRU             *pstAgpsCtx = VOS_NULL_PTR;

    /* 初始化消息变量 */
    pstCposrMsg = (AT_MTA_MSG_STRU*)pMsg;
    pstEvent    = (MTA_AT_CPOSR_IND_STRU*)pstCposrMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstCposrMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCposrInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    pstAgpsCtx = AT_GetModemAgpsCtxAddrFromClientId(ucIndex);

    /* 根据当前的AT_GetModemAgpsCtxAddrFromModemId(MODEM_ID_0)->enCposrReport的值判断是否允许主动上报辅助数据和指示 */
    if (AT_CPOSR_ENABLE == pstAgpsCtx->enCposrReport)
    {
        AT_ReportCposrInd(ucIndex, pstEvent->acXmlText);
    }
    else
    {
        AT_WARN_LOG1("AT_RcvMtaCposrInd: +CPOSR: ", pstAgpsCtx->enCposrReport);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaXcposrRptInd(VOS_VOID *pMsg)
{
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstXcposrRptMsg;
    MTA_AT_XCPOSRRPT_IND_STRU          *pstEvent;

    /* 初始化消息变量 */
    pstXcposrRptMsg = (AT_MTA_MSG_STRU*)pMsg;
    pstEvent        = (MTA_AT_XCPOSRRPT_IND_STRU*)pstXcposrRptMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstXcposrRptMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaXcposrRptInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 根据当前的AT_GetModemAgpsCtxAddrFromModemId(MODEM_ID_0)->enXcposrReport的值判断是否允许上报主动清除申请 */
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s^XCPOSRRPT: %d%s",
                                                        gaucAtCrLf,
                                                        pstEvent->ulClearFlg,
                                                        gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaCgpsClockSetCnf(VOS_VOID *pMsg)
{
#if (FEATURE_ON == FEATURE_AGPS)
    /* 定义局部变量 */
    VOS_BOOL                            bEnable;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstMtaMsg    = VOS_NULL_PTR;
    MTA_AT_CGPSCLOCK_CNF_STRU          *pstSetCnf    = VOS_NULL_PTR;

    pstMtaMsg = (AT_MTA_MSG_STRU*)pMsg;

    pstSetCnf = (MTA_AT_CGPSCLOCK_CNF_STRU*)pstMtaMsg->aucContent;

    /* 初始化消息变量 */
    ucIndex             = 0;
    bEnable             = 0;
    enModemId           = MODEM_ID_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCgpsClockSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCgpsClockSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CGPSCLOCK_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaCgpsClockSetCnf : Current Option is not AT_CMD_CGPSCLOCK_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if (MTA_AT_RESULT_NO_ERROR != pstSetCnf->enResult)
    {
        gstAtSendData.usBufLen = 0;
        /* 调用AT_FormATResultDATa发送命令结果 */
        At_FormatResultData(ucIndex, AT_ERROR);

        return VOS_OK;
    }

    if (VOS_OK != AT_GetModemIdFromClient(gastAtClientTab[ucIndex].usClientId, &enModemId))
    {
        enModemId = MODEM_ID_0;
    }

    bEnable = AT_GetCgpsCLockEnableFlgByModemId(enModemId);

    if (VOS_OK != AT_SetCgpsClock( bEnable,
                                   pstSetCnf->enModemId,
                                   pstSetCnf->enRatMode) )
    {
        gstAtSendData.usBufLen = 0;
        /* 调用AT_FormATResultDATa发送命令结果 */
        At_FormatResultData(ucIndex, AT_ERROR);

        AT_ERR_LOG("AT_RcvMtaCgpsClockSetCnf: AT_CfgGpsRefClk() failed!");
        return VOS_OK;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
#else
    AT_ERR_LOG("AT_RcvMtaCgpsClockSetCnf: AGPS is not open!");
    return VOS_OK;
#endif
}

VOS_VOID At_ProcMtaMsg(VOS_VOID *pstMsg)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulMsgCnt;
    VOS_UINT32                          ulMsgId;
    VOS_UINT32                          ulRst;

    /*从g_astAtProcMsgFromMtaTab中获取消息个数*/
    ulMsgCnt = sizeof(g_astAtProcMsgFromMtaTab)/sizeof(AT_PROC_MSG_FROM_MTA_STRU);

    /*从消息包中获取MSG ID*/
    ulMsgId  = ((AT_MTA_MSG_STRU *)pstMsg)->ulMsgId;

    /*g_astAtProcMsgFromMtaTab查表，进行消息分发*/
    for (i = 0; i < ulMsgCnt; i++)
    {
        if (g_astAtProcMsgFromMtaTab[i].ulMsgType == ulMsgId)
        {
            ulRst = g_astAtProcMsgFromMtaTab[i].pProcMsgFunc(pstMsg);

            if (VOS_ERR == ulRst)
            {
                AT_ERR_LOG("At_ProcMtaMsg: Msg Proc Err!");
            }

            return;
        }
    }

    /*没有找到匹配的消息*/
    if (ulMsgCnt == i)
    {
        AT_ERR_LOG("At_ProcMtaMsg: Msg Id is invalid!");
    }

    return;
}

#if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_AGPS) && (FEATURE_ON == FEATURE_XPDS))

VOS_VOID AT_ProcXpdsMsg(VOS_VOID *pstMsg)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulMsgCnt;
    VOS_UINT32                          ulMsgId;
    VOS_UINT32                          ulRst;

    /*从g_astAtProcMsgFromXpdsTab中获取消息个数*/
    ulMsgCnt = sizeof(g_astAtProcMsgFromXpdsTab)/sizeof(AT_PROC_MSG_FROM_XPDS_STRU);

    /*从消息包中获取MSG ID*/
    ulMsgId  = ((AT_XPDS_MSG_STRU *)pstMsg)->enMsgId;

    /*g_astAtProcMsgFromXpdsTab查表，进行消息分发*/
    for (i = 0; i < ulMsgCnt; i++)
    {
        if (g_astAtProcMsgFromXpdsTab[i].ulMsgType == ulMsgId)
        {
            ulRst = g_astAtProcMsgFromXpdsTab[i].pProcMsgFunc(pstMsg);

            if (VOS_ERR == ulRst)
            {
                AT_ERR_LOG("AT_ProcXpdsMsg: Msg Proc Err!");
            }

            return;
        }
    }

    /*没有找到匹配的消息*/
    if (ulMsgCnt == i)
    {
        AT_ERR_LOG("At_ProcXpdsMsg: Msg Id is invalid!");
    }

    return;
}

#endif


VOS_UINT32 AT_RcvMtaSimlockUnlockSetCnf( VOS_VOID *pMsg )
{
    AT_MTA_MSG_STRU                    *pstRcvMsg;
    MTA_AT_SIMLOCKUNLOCK_CNF_STRU      *pstEvent;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg       = (AT_MTA_MSG_STRU *)pMsg;
    pstEvent        = (MTA_AT_SIMLOCKUNLOCK_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex         = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSimlockUnlockSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSimlockUnlockSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if (AT_CMD_SIMLOCKUNLOCK_SET    != gastAtClientTab[ucIndex].CmdCurrentOpt
     && AT_CMD_CLCK_SIMLOCKUNLOCK   != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSimlockUnlockSetCnf: NOT CURRENT CMD OPTION!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断回复消息中的错误码 */
    if (MTA_AT_RESULT_NO_ERROR == pstEvent->enResult)
    {
        /* 解锁成功，输出OK */
        ulResult    = AT_OK;
    }
    else
    {
        /* 解锁失败，输出ERROR */
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaQryNmrCnf( VOS_VOID *pMsg )
{
    AT_MTA_MSG_STRU                    *pRcvMsg      = VOS_NULL_PTR;
    MTA_AT_QRY_NMR_CNF_STRU            *pstQryNmrCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulNmrDataIndex;
    VOS_UINT16                          usLength;

    /* 初始化 */
    pRcvMsg      = (AT_MTA_MSG_STRU *)pMsg;
    pstQryNmrCnf = (MTA_AT_QRY_NMR_CNF_STRU *)(pRcvMsg->aucContent);

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryNmrCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryNmrCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CNMR_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    if (MTA_AT_RESULT_ERROR == pstQryNmrCnf->enResult)
    {
        /* 查询失败直接上报error */
        gstAtSendData.usBufLen = 0;

        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        /* 输出结果 */
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_OK;
    }

    usLength = 0;

    /* 按AT^CNMR查询命令返回将接入层返回的数据码流显示上报 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    if (0 == pstQryNmrCnf->ucTotalIndex)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%d,%d",
                                           0,
                                           0);

        gstAtSendData.usBufLen = usLength;

        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        /* 输出结果 */
        At_FormatResultData(ucIndex, AT_OK);
        return VOS_OK;
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d,%d,",
                                       pstQryNmrCnf->ucTotalIndex,
                                       pstQryNmrCnf->ucCurrIndex);

    for (ulNmrDataIndex = 0; ulNmrDataIndex < pstQryNmrCnf->usNMRLen; ulNmrDataIndex++)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR*)pgucAtSndCodeAddr + usLength,
                                           "%02x",
                                           pstQryNmrCnf->aucNMRData[ulNmrDataIndex]);
    }

    if (pstQryNmrCnf->ucTotalIndex != pstQryNmrCnf->ucCurrIndex)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s",
                                          gaucAtCrLf);

         At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    }


    if (pstQryNmrCnf->ucTotalIndex == pstQryNmrCnf->ucCurrIndex)
    {
        At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        gstAtSendData.usBufLen = 0;

        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        /* 输出结果 */
        At_FormatResultData(ucIndex, AT_OK);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaWrrAutotestQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg             = VOS_NULL_PTR;
    MTA_AT_WRR_AUTOTEST_QRY_CNF_STRU   *pstWrrAutotestCnf   = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          i;
    VOS_UINT32                          ulRsltNum;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstWrrAutotestCnf   = (MTA_AT_WRR_AUTOTEST_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;
    ulRsltNum           = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrAutotestQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrAutotestQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CWAS_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaWrrAutotestQryCnf : Current Option is not AT_CMD_CWAS_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CWAS命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK != pstWrrAutotestCnf->ulResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;

        if (pstWrrAutotestCnf->stWrrAutoTestRslt.ulRsltNum > 0)
        {
            gstAtSendData.usBufLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                             (TAF_CHAR *)pgucAtSndCodeAddr,
                                                             (TAF_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                                             "%d",
                                                             pstWrrAutotestCnf->stWrrAutoTestRslt.aulRslt[0]);

            ulRsltNum =  AT_MIN(pstWrrAutotestCnf->stWrrAutoTestRslt.ulRsltNum, (MTA_AT_WRR_AUTOTEST_MAX_RSULT_NUM - 1));

            for (i = 1; i < ulRsltNum; i++)
            {
                gstAtSendData.usBufLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                                 (TAF_CHAR *)pgucAtSndCodeAddr,
                                                                 (TAF_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                                                 ",%d",
                                                                 pstWrrAutotestCnf->stWrrAutoTestRslt.aulRslt[i]);
            }
        }
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_VOID AT_RcvMtaGrrAutotestQryProc(
    MTA_AT_GAS_AUTOTEST_QRY_CNF_STRU                       *pstMtaAutotestCnf,
    VOS_UINT32                                             *pUlRslt
)
{
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          i;

    usLength = 0;
    ulRslt   = AT_OK;
    i        = 0;

    /* 邻区状态 */
    if (GAS_AT_CMD_NCELL == gastAtParaList[0].ulParaValue)
    {
        if ((pstMtaAutotestCnf->stGrrAutoTestRslt.ulRsltNum > 0)
         && ((pstMtaAutotestCnf->stGrrAutoTestRslt.ulRsltNum %3 ) == 0)
         && (pstMtaAutotestCnf->stGrrAutoTestRslt.ulRsltNum <= 20))
        {
            AT_FormatGasAtCmdRslt(&(pstMtaAutotestCnf->stGrrAutoTestRslt));
        }
        else if (0 == pstMtaAutotestCnf->stGrrAutoTestRslt.ulRsltNum)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                               "0");
            gstAtSendData.usBufLen = usLength;
        }
        else
        {
            ulRslt = AT_ERROR;
        }
    }
    else if (GAS_AT_CMD_SCELL == gastAtParaList[0].ulParaValue)
    {
        /* 服务小区状态 */
        if (3 != pstMtaAutotestCnf->stGrrAutoTestRslt.ulRsltNum)
        {
            ulRslt = AT_ERROR;
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                               "%d,",
                                               pstMtaAutotestCnf->stGrrAutoTestRslt.aulRslt[0]);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                               "0x%X,",
                                               pstMtaAutotestCnf->stGrrAutoTestRslt.aulRslt[1]&0xff);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                               "%d",
                                               pstMtaAutotestCnf->stGrrAutoTestRslt.aulRslt[2]);
            gstAtSendData.usBufLen = usLength;
            ulRslt = AT_OK;
        }
    }
    else if (pstMtaAutotestCnf->stGrrAutoTestRslt.ulRsltNum > 0)
    {
        /* 其它命令 */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                           "%d",
                                           pstMtaAutotestCnf->stGrrAutoTestRslt.aulRslt[0]);

        for (i = 1; i< pstMtaAutotestCnf->stGrrAutoTestRslt.ulRsltNum ;i++)
        {
            usLength +=(TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (TAF_CHAR *)pgucAtSndCodeAddr,
                                              (TAF_CHAR *)pgucAtSndCodeAddr+usLength,
                                              ",%d",
                                              pstMtaAutotestCnf->stGrrAutoTestRslt.aulRslt[i]);
        }
        gstAtSendData.usBufLen = usLength;
    }
    else
    {

    }

    *pUlRslt = ulRslt;
}


VOS_UINT32 AT_RcvMtaGrrAutotestQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg             = VOS_NULL_PTR;
    MTA_AT_GAS_AUTOTEST_QRY_CNF_STRU   *pstGrrAutotestCnf   = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstGrrAutotestCnf   = (MTA_AT_GAS_AUTOTEST_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaGrrAutotestQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaGrrAutotestQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CGAS_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaGrrAutotestQryCnf : Current Option is not AT_CMD_CGAS_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CGAS命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK != pstGrrAutotestCnf->ulResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;

        AT_RcvMtaGrrAutotestQryProc(pstGrrAutotestCnf, &ulResult);
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaWrrCellinfoQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg              = VOS_NULL_PTR;
    MTA_AT_WRR_CELLINFO_QRY_CNF_STRU   *pstWrrCellinfoQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          i;
    VOS_UINT32                          ulCellNum;

    /* 初始化 */
    pRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstWrrCellinfoQryCnf    = (MTA_AT_WRR_CELLINFO_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ulResult                = AT_OK;
    ucIndex                 = 0;
    ulCellNum               = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrCellinfoQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrCellinfoQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CELLINFO_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaWrrCellinfoQryCnf : Current Option is not AT_CMD_CELLINFO_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CELLINFO命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK != pstWrrCellinfoQryCnf->ulResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;

        /*没有获取的小区信息，打印0*/
        if ( 0 == pstWrrCellinfoQryCnf->stWrrCellInfo.ulCellNum )
        {
            gstAtSendData.usBufLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR*)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                               "no cellinfo rslt");
        }

        ulCellNum = AT_MIN(pstWrrCellinfoQryCnf->stWrrCellInfo.ulCellNum, MTA_AT_WRR_MAX_NCELL_NUM);

        for (i = 0; i < ulCellNum; i++)
        {
            gstAtSendData.usBufLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                             (TAF_CHAR *)pgucAtSndCodeAddr,
                                                             (TAF_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                                             "%d,%d,%d,%d\r\n",
                                                             pstWrrCellinfoQryCnf->stWrrCellInfo.astWCellInfo[i].usCellFreq,
                                                             pstWrrCellinfoQryCnf->stWrrCellInfo.astWCellInfo[i].usPrimaryScramCode,
                                                             pstWrrCellinfoQryCnf->stWrrCellInfo.astWCellInfo[i].sCpichRscp,
                                                             pstWrrCellinfoQryCnf->stWrrCellInfo.astWCellInfo[i].sCpichEcN0);
        }
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaDelCellEntityCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg           = VOS_NULL_PTR;
    MTA_AT_DEL_CELLENTITY_CNF_STRU     *pstDelCellEntityCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstDelCellEntityCnf = (MTA_AT_DEL_CELLENTITY_CNF_STRU *)pRcvMsg->aucContent;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaDelCellentityCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaDelCellentityCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_DEL_CELLENTITY_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaDelCellentityCnf : Current Option is not AT_CMD_DEL_CELLENTITY_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出结果 */
   if (VOS_OK == pstDelCellEntityCnf->ulResult)
   {
        At_FormatResultData(ucIndex, AT_OK);
   }
   else
   {
        At_FormatResultData(ucIndex, AT_ERROR);
   }

   return VOS_OK;
}


VOS_UINT32 AT_RcvMtaWrrMeanrptQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg             = VOS_NULL_PTR;
    MTA_AT_WRR_MEANRPT_QRY_CNF_STRU    *pstWrrMeanrptQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;
    VOS_UINT32                          i;
    VOS_UINT32                          ulCellNumLoop;
    VOS_UINT32                          ulRptNum;
    VOS_UINT16                          usCellNum;

    /* 初始化 */
    pRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstWrrMeanrptQryCnf     = (MTA_AT_WRR_MEANRPT_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ulResult                = AT_OK;
    usLength                = 0;
    ucIndex                 = 0;
    ulRptNum                = 0;
    usCellNum               = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrMeanrptQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrMeanrptQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_MEANRPT_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaWrrMeanrptQryCnf : Current Option is not AT_CMD_MEANRPT_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^MEANRPT命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK != pstWrrMeanrptQryCnf->ulResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR*)pgucAtSndCodeAddr+usLength,
                                           "%d",
                                           pstWrrMeanrptQryCnf->stMeanRptRslt.ulRptNum);

        ulRptNum = AT_MIN(pstWrrMeanrptQryCnf->stMeanRptRslt.ulRptNum, MTA_AT_WRR_MAX_MEANRPT_NUM);

        for ( i = 0 ; i < ulRptNum; i++ )
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR*)pgucAtSndCodeAddr+usLength,
                                               "\r\n0x%X,%d",
                                                pstWrrMeanrptQryCnf->stMeanRptRslt.astMeanRptInfo[i].usEventId,
                                                pstWrrMeanrptQryCnf->stMeanRptRslt.astMeanRptInfo[i].usCellNum );

            usCellNum = AT_MIN(pstWrrMeanrptQryCnf->stMeanRptRslt.astMeanRptInfo[i].usCellNum, MTA_AT_WRR_ONE_MEANRPT_MAX_CELL_NUM);

            for ( ulCellNumLoop = 0; ulCellNumLoop < usCellNum; ulCellNumLoop++ )
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR*)pgucAtSndCodeAddr+usLength,
                                                   ",%d",
                                                   pstWrrMeanrptQryCnf->stMeanRptRslt.astMeanRptInfo[i].ausPrimaryScramCode[ulCellNumLoop]);
            }

        }
        gstAtSendData.usBufLen = usLength;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaWrrCellSrhSetCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg             = VOS_NULL_PTR;
    MTA_AT_WRR_CELLSRH_SET_CNF_STRU    *pstWrrCellSrhSetCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstWrrCellSrhSetCnf = (MTA_AT_WRR_CELLSRH_SET_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrCellSrhSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrCellSrhSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_WAS_MNTN_SET_CELLSRH != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaWrrCellSrhSetCnf : Current Option is not AT_CMD_WAS_MNTN_SET_CELLSRH.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CELLSRH命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK != pstWrrCellSrhSetCnf->ulResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaWrrCellSrhQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg             = VOS_NULL_PTR;
    MTA_AT_WRR_CELLSRH_QRY_CNF_STRU    *pstWrrCellSrhQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstWrrCellSrhQryCnf = (MTA_AT_WRR_CELLSRH_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrCellSrhQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrCellSrhQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_WAS_MNTN_QRY_CELLSRH != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaWrrCellSrhQryCnf : Current Option is not AT_CMD_WAS_MNTN_QRY_CELLSRH.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^FREQLOCK查询命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK != pstWrrCellSrhQryCnf->ulResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;

        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR*)pgucAtSndCodeAddr,
                                                        "%s: %d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        (TAF_INT32)pstWrrCellSrhQryCnf->ucCellSearchType);

    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaWrrFreqLockSetCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg           = VOS_NULL_PTR;
    MTA_AT_WRR_FREQLOCK_SET_CNF_STRU   *pstWrrFreqlockCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstWrrFreqlockCnf   = (MTA_AT_WRR_FREQLOCK_SET_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrFreqLockSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrFreqLockSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_FREQLOCK_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaWrrFreqLockSetCnf : Current Option is not AT_CMD_FREQLOCK_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^FREQLOCK命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK != pstWrrFreqlockCnf->ulResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaWrrFreqLockQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg           = VOS_NULL_PTR;
    MTA_AT_WRR_FREQLOCK_QRY_CNF_STRU   *pstWrrFreqlockCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstWrrFreqlockCnf   = (MTA_AT_WRR_FREQLOCK_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrFreqLockQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrFreqLockQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_FREQLOCK_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaWrrFreqLockQryCnf : Current Option is not AT_CMD_FREQLOCK_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^FREQLOCK查询命令返回 */
    gstAtSendData.usBufLen = 0;
    if (VOS_OK != pstWrrFreqlockCnf->ulResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;

        if (VOS_FALSE == pstWrrFreqlockCnf->stFreqLockInfo.ucFreqLockEnable)
        {
            gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                                            (TAF_CHAR*)pgucAtSndCodeAddr,
                                                            "%s: %d",
                                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                            (TAF_INT32)pstWrrFreqlockCnf->stFreqLockInfo.ucFreqLockEnable);
        }
        else
        {
            gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                                            (TAF_CHAR*)pgucAtSndCodeAddr,
                                                            "%s: %d,%d",
                                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                            (TAF_INT32)pstWrrFreqlockCnf->stFreqLockInfo.ucFreqLockEnable,
                                                            (TAF_INT32)pstWrrFreqlockCnf->stFreqLockInfo.usLockedFreq);
        }

    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaWrrRrcVersionSetCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                        *pRcvMsg             = VOS_NULL_PTR;
    MTA_AT_WRR_RRC_VERSION_SET_CNF_STRU    *pstWrrVersionSetCnf = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstWrrVersionSetCnf     = (MTA_AT_WRR_RRC_VERSION_SET_CNF_STRU *)pRcvMsg->aucContent;
    ulResult                = AT_OK;
    ucIndex                 = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrRrcVersionSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrRrcVersionSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_RRC_VERSION_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaWrrRrcVersionSetCnf : Current Option is not AT_CMD_RRC_VERSION_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^FREQLOCK命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK != pstWrrVersionSetCnf->ulResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        /* 设置成功后重新读写一下NV */
        AT_ReadWasCapabilityNV();

        ulResult = AT_OK;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}




VOS_UINT32 AT_RcvMtaWrrRrcVersionQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                        *pRcvMsg             = VOS_NULL_PTR;
    MTA_AT_WRR_RRC_VERSION_QRY_CNF_STRU    *pstWrrVersionQryCnf = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstWrrVersionQryCnf = (MTA_AT_WRR_RRC_VERSION_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrRrcVersionQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaWrrRrcVersionQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_RRC_VERSION_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaWrrRrcVersionQryCnf : Current Option is not AT_CMD_RRC_VERSION_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^FREQLOCK查询命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK != pstWrrVersionQryCnf->ulResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;

        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        (TAF_INT32)pstWrrVersionQryCnf->ucRrcVersion);

    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaAcInfoQueryCnf(VOS_VOID *pstMsg)
{
    TAF_MMA_AC_INFO_QRY_CNF_STRU       *pstAcInfoQueryCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstAcInfoQueryCnf       = (TAF_MMA_AC_INFO_QRY_CNF_STRU*)pstMsg;
    usLength                = 0;
    ulResult                = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstAcInfoQueryCnf->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAcInfoQueryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAcInfoQueryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_LOCINFO_READ */
    if (AT_CMD_ACINFO_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询结果是否失败,如果失败则返回ERROR */
    if (VOS_OK != pstAcInfoQueryCnf->ulRslt)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        usLength  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          (VOS_UINT8)(pstAcInfoQueryCnf->stCellCsAcInfo.enSrvDomain),/* 上报服务域 */
                                          (VOS_UINT8)(pstAcInfoQueryCnf->stCellCsAcInfo.enCellAcType),/* 上报小区禁止接入类型 */
                                          (VOS_UINT8)(pstAcInfoQueryCnf->stCellCsAcInfo.ucRestrictRegister),/* 上报是否注册受限 */
                                          (VOS_UINT8)(pstAcInfoQueryCnf->stCellCsAcInfo.ucRestrictPagingRsp));/* 上报是否寻呼受限 */

        /* 换行 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (TAF_CHAR *)pgucAtSndCodeAddr,
                                          (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s",
                                          gaucAtCrLf);

        usLength  += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          (VOS_UINT8)(pstAcInfoQueryCnf->stCellPsAcInfo.enSrvDomain),/* 上报服务域 */
                                          (VOS_UINT8)(pstAcInfoQueryCnf->stCellPsAcInfo.enCellAcType),/* 上报小区禁止接入类型 */
                                          (VOS_UINT8)(pstAcInfoQueryCnf->stCellPsAcInfo.ucRestrictRegister),/* 上报是否注册受限 */
                                          (VOS_UINT8)(pstAcInfoQueryCnf->stCellPsAcInfo.ucRestrictPagingRsp));/* 上报是否寻呼受限 */

        gstAtSendData.usBufLen = usLength;

    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}



VOS_UINT32 AT_RcvMtaQryCurcCnf(VOS_VOID *pstMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg          = VOS_NULL_PTR;
    MTA_AT_CURC_QRY_CNF_STRU               *pstMtaAtQryCurcCnf = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;
    VOS_UINT16                              usLength;
    VOS_UINT8                               ucLoop;

    /* 初始化 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pstMsg;
    pstMtaAtQryCurcCnf  = (MTA_AT_CURC_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;
    usLength            = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryCurcCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryCurcCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CURC_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaQryCurcCnf : Current Option is not AT_CMD_RRC_VERSION_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CURC查询命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR != pstMtaAtQryCurcCnf->enResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;

        usLength        += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                  "%s: %d",
                                                  g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                  pstMtaAtQryCurcCnf->enCurcRptType);

        /* 如果<mode>参数为2，需要打印参数<report_cfg> */
        if (AT_MTA_RPT_GENERAL_CONTROL_CUSTOM == pstMtaAtQryCurcCnf->enCurcRptType)
        {
            usLength    += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                  ",0x");

            for (ucLoop = 0; ucLoop < AT_CURC_RPT_CFG_MAX_SIZE; ucLoop++)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%02X",
                                                   pstMtaAtQryCurcCnf->aucRptCfg[ucLoop]);
            }
        }
    }

    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaSetUnsolicitedRptCnf(VOS_VOID *pstMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg            = VOS_NULL_PTR;
    MTA_AT_UNSOLICITED_RPT_SET_CNF_STRU    *pstSetUnsolicitedCnf = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg               = (AT_MTA_MSG_STRU *)pstMsg;
    pstSetUnsolicitedCnf    = (MTA_AT_UNSOLICITED_RPT_SET_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult                = AT_OK;
    ucIndex                 = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetUnsolicitedRptCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetUnsolicitedRptCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_UNSOLICITED_RPT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetUnsolicitedRptCnf : Current Option is not AT_CMD_RRC_VERSION_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR != pstSetUnsolicitedCnf->enResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}


VOS_UINT32 AT_RcvMtaQryUnsolicitedRptCnf(VOS_VOID *pstMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg            = VOS_NULL_PTR;
    MTA_AT_UNSOLICITED_RPT_QRY_CNF_STRU    *pstQryUnsolicitedCnf = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg               = (AT_MTA_MSG_STRU *)pstMsg;
    pstQryUnsolicitedCnf    = (MTA_AT_UNSOLICITED_RPT_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult                = AT_OK;
    ucIndex                 = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryUnsolicitedRptCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryUnsolicitedRptCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_UNSOLICITED_RPT_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaQryUnsolicitedRptCnf : Current Option is not AT_CMD_RRC_VERSION_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化查询命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR != pstQryUnsolicitedCnf->enResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        /* ucIndex参数带下来 */
        ulResult = AT_ProcMtaUnsolicitedRptQryCnf(ucIndex, pstMsg);
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_ProcMtaUnsolicitedRptQryCnf(
    VOS_UINT8                               ucIndex,
    VOS_VOID                               *pstMsg
)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg            = VOS_NULL_PTR;
    MTA_AT_UNSOLICITED_RPT_QRY_CNF_STRU    *pstQryUnsolicitedCnf = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    /* ucIndex通过上层函数传下来 */

    pstRcvMsg               = (AT_MTA_MSG_STRU *)pstMsg;
    pstQryUnsolicitedCnf    = (MTA_AT_UNSOLICITED_RPT_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult                = AT_OK;

    switch (pstQryUnsolicitedCnf->enReqType)
    {
        /* TIME查询的处理 */
        case AT_MTA_QRY_TIME_RPT_TYPE:
            gstAtSendData.usBufLen =
                (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: %d",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       pstQryUnsolicitedCnf->u.ucTimeRptFlg);
            break;

        /* pstQryUnsolicitedCnf数据结构体修改 */
        /* CTZR查询的处理 */
        case AT_MTA_QRY_CTZR_RPT_TYPE:
            gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: %d",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       pstQryUnsolicitedCnf->u.ucCtzrRptFlg);
            break;

        /* CSSN查询的处理 */
        case AT_MTA_QRY_CSSN_RPT_TYPE:
            gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                                            "%s: %d,%d",
                                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                            pstQryUnsolicitedCnf->u.stCssnRptFlg.ucCssiRptFlg,
                                                            pstQryUnsolicitedCnf->u.stCssnRptFlg.ucCssuRptFlg);
            break;

        /* CUSD查询的处理 */
        case AT_MTA_QRY_CUSD_RPT_TYPE:
            gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                                            (VOS_CHAR *)pgucAtSndCodeAddr,"%s: %d",
                                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                            pstQryUnsolicitedCnf->u.ucCusdRptFlg);
            break;

        default:
            ulResult = AT_CME_UNKNOWN;

    }

    return ulResult;
}


LOCAL VOS_VOID AT_ProcCerssiInfoQueryCampOnGsm(
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf,
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSystemAppConfig
)
{
    AT_MODEM_NET_CTX_STRU              *pstNetCtx;
    VOS_UINT16                          usLength;

    usLength    = 0;
    pstNetCtx   = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    if (SYSTEM_APP_ANDROID != ucSystemAppConfig)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stGCellSignInfo.sRssiValue,
                                          0,
                                          255,
                                          0,
                                          0,
                                          0,
                                          0,0x7f7f,0x7f7f,
                                          0,99,99,99,99,99,99,99,99,
                                          0,
                                          0,
                                          0);
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stGCellSignInfo.sRssiValue,
                                          0,
                                          255,
                                          0,
                                          0,
                                          0,
                                          0,0x7f7f,0x7f7f,
                                          0,
                                          0,
                                          0);
    }

    gstAtSendData.usBufLen = usLength;

    return;
}


LOCAL VOS_VOID AT_ProcCerssiInfoQueryCampOnWcdma(
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf,
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSystemAppConfig
)
{
    AT_MODEM_NET_CTX_STRU              *pstNetCtx;
    VOS_UINT16                          usLength;

    usLength    = 0;
    pstNetCtx   = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    if (TAF_UTRANCTRL_UTRAN_MODE_FDD == pstCerssiInfoQueryCnf->stCerssi.ucCurrentUtranMode)
    {
        if (SYSTEM_APP_ANDROID != ucSystemAppConfig)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          0,      /* rssi */
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.sRscpValue,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.sEcioValue,
                                          0,
                                          0,
                                          0,
                                          0,0x7f7f,0x7f7f,
                                          0,99,99,99,99,99,99,99,99,
                                          0,
                                          0,
                                          0);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          0,      /* rssi */
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.sRscpValue,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.sEcioValue,
                                          0,
                                          0,
                                          0,
                                          0,0x7f7f,0x7f7f,
                                          0,
                                          0,
                                          0);
        }

        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        /* 非fdd 3g 小区，ecio值为无效值255 */
        if (SYSTEM_APP_ANDROID != ucSystemAppConfig)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          0,      /* rssi */
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.sRscpValue,
                                          255,
                                          0,
                                          0,
                                          0,
                                          0,0x7f7f,0x7f7f,
                                          0,99,99,99,99,99,99,99,99,
                                          0,
                                          0,
                                          0);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          0,      /* rssi */
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.sRscpValue,
                                          255,
                                          0,
                                          0,
                                          0,
                                          0,0x7f7f,0x7f7f,
                                          0,
                                          0,
                                          0);
        }

        gstAtSendData.usBufLen = usLength;
    }

    return;
}

#if ( FEATURE_ON == FEATURE_LTE )

LOCAL VOS_VOID AT_ProcCerssiInfoQueryCampOnLte(
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf,
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSystemAppConfig
)
{
    VOS_UINT16                          usLength;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx;

    usLength = 0;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    /* LTE下的cerssi查询处理由L4A移到NAS，和atCerssiInfoCnfProc的处理相同 */
    if (SYSTEM_APP_ANDROID != ucSystemAppConfig)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          0,
                                          0,
                                          255,
                                          (pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrp ==  AT_SIGNAL_INVALID_VALUE ? \
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrp : \
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrp / AT_SIGNAL_DIVISOR_EIGHT),
                                          (pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrq ==  AT_SIGNAL_INVALID_VALUE ? \
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrq : \
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrq / AT_SIGNAL_DIVISOR_EIGHT),
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.lSINR,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.usRI,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[0],
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[1],
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.ucRxANTNum,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.asRsrpRx[0],
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.asRsrpRx[1],
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.asRsrpRx[2],
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.asRsrpRx[3],
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.alSINRRx[0],
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.alSINRRx[1],
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.alSINRRx[2],
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.alSINRRx[3],
                                          0,
                                          0,
                                          0);

    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          0,
                                          0,
                                          255,
                                          (pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrp ==  AT_SIGNAL_INVALID_VALUE ? \
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrp : \
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrp / AT_SIGNAL_DIVISOR_EIGHT),
                                          (pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrq ==  AT_SIGNAL_INVALID_VALUE ? \
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrq : \
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrq / AT_SIGNAL_DIVISOR_EIGHT),
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.lSINR,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.usRI,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[0],
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[1],
                                          0,
                                          0,
                                          0);
    }
    gstAtSendData.usBufLen = usLength;

    return;
}
#endif

#if ( FEATURE_ON == FEATURE_UE_MODE_NR )

LOCAL VOS_VOID AT_ProcCerssiInfoQueryCampOnNr(
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf,
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSystemAppConfig
)
{
    VOS_UINT16                          usLength;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx;

    usLength = 0;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    if (SYSTEM_APP_ANDROID != ucSystemAppConfig)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          0,
                                          0,
                                          255,
                                          0,
                                          0,
                                          0,
                                          0,0x7f7f,0x7f7f,
                                          0,99,99,99,99,99,99,99,99,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.s5GRsrp,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.s5GRsrq,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.l5GSinr);

        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          0,
                                          0,
                                          255,
                                          0,
                                          0,
                                          0,
                                          0,0x7f7f,0x7f7f,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.s5GRsrp,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.s5GRsrq,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.l5GSinr);

        gstAtSendData.usBufLen = usLength;
    }



    return;

}
#endif


LOCAL VOS_VOID AT_ProcCerssiInfoQueryNotCampOn(
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf,
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSystemAppConfig
)
{
    VOS_UINT16                          usLength;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx;

    usLength = 0;

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    if (SYSTEM_APP_ANDROID != ucSystemAppConfig)
    {
        /* 刚开机没有接入模式，参数都返回无效值 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stGCellSignInfo.sRssiValue,      /* rssi */
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.sRscpValue,
                                          255,
                                          0,
                                          0,
                                          0,
                                          0,0x7f7f,0x7f7f,
                                          0,99,99,99,99,99,99,99,99,
                                          0,
                                          0,
                                          0);
    }
    else
    {
        /* 刚开机没有接入模式，参数都返回无效值 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstNetCtx->ucCerssiReportType,
                                          pstNetCtx->ucCerssiMinTimerInterval,
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stGCellSignInfo.sRssiValue,      /* rssi */
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.sRscpValue,
                                          255,
                                          0,
                                          0,
                                          0,
                                          0,0x7f7f,0x7f7f,
                                          0,
                                          0,
                                          0);
    }

    gstAtSendData.usBufLen = usLength;

    return;
}



VOS_UINT32 AT_ProcCerssiInfoQuery(VOS_VOID *pstMsg)
{
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU                       *pstCerssiInfoQueryCnf = VOS_NULL_PTR;
    VOS_UINT8                                               ucIndex;
    VOS_UINT8                                              *pucSystemAppConfig;

    /* 初始化 */
    pstCerssiInfoQueryCnf  = (TAF_MMA_CERSSI_INFO_QRY_CNF_STRU*)pstMsg;
    pucSystemAppConfig     = AT_GetSystemAppConfigAddr();
    ucIndex                = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCerssiInfoQueryCnf->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCerssiInfoQueryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCerssiInfoQueryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if (TAF_MMA_RAT_GSM == pstCerssiInfoQueryCnf->stCerssi.enRatType)
    {
        AT_ProcCerssiInfoQueryCampOnGsm(pstCerssiInfoQueryCnf, ucIndex, *pucSystemAppConfig);
    }

    else if (TAF_MMA_RAT_WCDMA == pstCerssiInfoQueryCnf->stCerssi.enRatType)
    {
        AT_ProcCerssiInfoQueryCampOnWcdma(pstCerssiInfoQueryCnf, ucIndex, *pucSystemAppConfig);
    }


#if ( FEATURE_ON == FEATURE_LTE )
    else if (TAF_MMA_RAT_LTE == pstCerssiInfoQueryCnf->stCerssi.enRatType)
    {
        AT_ProcCerssiInfoQueryCampOnLte(pstCerssiInfoQueryCnf, ucIndex, *pucSystemAppConfig);
    }
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    else if (TAF_MMA_RAT_NR == pstCerssiInfoQueryCnf->stCerssi.enRatType)
    {
        AT_ProcCerssiInfoQueryCampOnNr(pstCerssiInfoQueryCnf, ucIndex, *pucSystemAppConfig);
    }
#endif

    /* 还没有收到小区信号 */
    else
    {
        AT_ProcCerssiInfoQueryNotCampOn(pstCerssiInfoQueryCnf, ucIndex, *pucSystemAppConfig);
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


#if (FEATURE_ON == FEATURE_UE_MODE_NR)

VOS_UINT32 AT_ProcCserssiInfoQuery(VOS_VOID *pstMsg)
{
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU                       *pstCserssiInfoQueryCnf = VOS_NULL_PTR;
    VOS_UINT16                                              usLength;
    VOS_INT16                                               sRsrp;
    VOS_INT16                                               sRsrq;
    VOS_UINT8                                               ucIndex;
    AT_MODEM_NET_CTX_STRU                                  *pstNetCtx               = VOS_NULL_PTR;
    VOS_UINT8                                              *pucSystemAppConfig;

    /* 初始化 */
    pstCserssiInfoQueryCnf  = (TAF_MMA_CERSSI_INFO_QRY_CNF_STRU*)pstMsg;
    usLength               = 0;
    ucIndex                = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    pucSystemAppConfig     = AT_GetSystemAppConfigAddr();

    sRsrp = \
            pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrp == AT_SIGNAL_INVALID_VALUE ? \
            pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrp : \
            pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrp / AT_SIGNAL_DIVISOR_EIGHT;
    sRsrq = \
            pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrq == AT_SIGNAL_INVALID_VALUE ? \
            pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrq : \
            pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrq / AT_SIGNAL_DIVISOR_EIGHT;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCserssiInfoQueryCnf->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_ProcCserssiInfoQuery: WARNING: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_ProcCserssiInfoQuery: WARNING: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    if (TAF_MMA_RAT_NR == pstCserssiInfoQueryCnf->stCerssi.enRatType)
    {
        if (SYSTEM_APP_ANDROID != *pucSystemAppConfig)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                              "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              pstNetCtx->ucCerssiReportType,
                                              pstNetCtx->ucCerssiMinTimerInterval,
                                              0,
                                              0,
                                              255,
                                              0,
                                              0,
                                              0,
                                              0,0x7f7f,0x7f7f,
                                              0,99,99,99,99,99,99,99,99,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.s5GRsrp,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.s5GRsrq,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.l5GSinr);

            gstAtSendData.usBufLen = usLength;

        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                              "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              pstNetCtx->ucCerssiReportType,
                                              pstNetCtx->ucCerssiMinTimerInterval,
                                              0,
                                              0,
                                              255,
                                              0,
                                              0,
                                              0,
                                              0,0x7f7f,0x7f7f,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.s5GRsrp,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.s5GRsrq,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stNrCellSignInfo.l5GSinr);

            gstAtSendData.usBufLen = usLength;
        }
    }

    else if (TAF_MMA_RAT_LTE == pstCserssiInfoQueryCnf->stCerssi.enRatType)
    {
        if (SYSTEM_APP_ANDROID != *pucSystemAppConfig)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                              "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              pstNetCtx->ucCerssiReportType,
                                              pstNetCtx->ucCerssiMinTimerInterval,
                                              0,
                                              0,
                                              255,
                                              sRsrp,
                                              sRsrq,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.lSINR,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.usRI,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[0],
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[1],
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.ucRxANTNum,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.asRsrpRx[0],
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.asRsrpRx[1],
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.asRsrpRx[2],
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.asRsrpRx[3],
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.alSINRRx[0],
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.alSINRRx[1],
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.alSINRRx[2],
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stRxAntInfo.alSINRRx[3],
                                              0,
                                              0,
                                              0);

        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                              "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              pstNetCtx->ucCerssiReportType,
                                              pstNetCtx->ucCerssiMinTimerInterval,
                                              0,
                                              0,
                                              255,
                                              sRsrp,
                                              sRsrq,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.lSINR,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.usRI,
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[0],
                                              pstCserssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.stCQI.ausCQI[1],
                                              0,
                                              0,
                                              0);
        }
        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        if (SYSTEM_APP_ANDROID != *pucSystemAppConfig)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                              "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              pstNetCtx->ucCerssiReportType,
                                              pstNetCtx->ucCerssiMinTimerInterval,
                                              0,
                                              0,
                                              255,
                                              0,
                                              0,
                                              0,
                                              0,0x7f7f,0x7f7f,
                                              0,99,99,99,99,99,99,99,99,
                                              0,
                                              0,
                                              0);

            gstAtSendData.usBufLen = usLength;

        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                              "%s:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              pstNetCtx->ucCerssiReportType,
                                              pstNetCtx->ucCerssiMinTimerInterval,
                                              0,
                                              0,
                                              255,
                                              0,
                                              0,
                                              0,
                                              0,0x7f7f,0x7f7f,
                                              0,
                                              0,
                                              0);

            gstAtSendData.usBufLen = usLength;
        }
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}
#endif



VOS_INT16 AT_ConvertCerssiRssiToCesqRxlev(VOS_INT16 sCerssiRssi)
{
    VOS_INT16                           sCesqRxlev;

    /*
        转换原则如下:
              0                        sCerssiRssi < -110dBm
              1             -110dBm <= sCerssiRssi < -109dBm
              2             -109dBm <= sCerssiRssi < -108dBm
              3             -108dBm <= sCerssiRssi < -107dBm
              ...           ...
              62            -49dBm  <= sCerssiRssi < -48 dBm
              63            -48dBm  <= sCerssiRssi
              99            not known or not detectable
    */

    if (sCerssiRssi < AT_CMD_CESQ_RXLEV_LOWER_BOUNDARY_VALUE)
    {
        sCesqRxlev = AT_CMD_CESQ_RXLEV_MIN_VALUE;
    }
    else if (sCerssiRssi < AT_CMD_CESQ_RXLEV_UPPER_BOUNDARY_VALUE)
    {
        sCesqRxlev = sCerssiRssi + AT_CMD_CESQ_RXLEV_CONVERT_BASE_VALUE;
    }
    else
    {
        sCesqRxlev = AT_CMD_CESQ_RXLEV_MAX_VALUE;
    }

    return sCesqRxlev;
}


VOS_INT16 AT_ConvertCerssiRscpToCesqRscp(VOS_INT16 sCerssiRscp)
{
    VOS_INT16                           sCesqRscp;

    /*
        转换原则如下:
              0                        sCerssiRscp < -120dBm
              1             -120dBm <= sCerssiRscp < -119dBm
              2             -119dBm <= sCerssiRscp < -118dBm
              3             -118dBm <= sCerssiRscp < -117dBm
              ...           ...
              95            -26dBm  <= sCerssiRscp < -25 dBm
              96            -25dBm  <= sCerssiRscp
              255            not known or not detectable
    */

    if (sCerssiRscp < AT_CMD_CESQ_RSCP_LOWER_BOUNDARY_VALUE)
    {
        sCesqRscp = AT_CMD_CESQ_RSCP_MIN_VALUE;
    }
    else if (sCerssiRscp < AT_CMD_CESQ_RSCP_UPPER_BOUNDARY_VALUE)
    {
        sCesqRscp = sCerssiRscp + AT_CMD_CESQ_RSCP_CONVERT_BASE_VALUE;
    }
    else
    {
        sCesqRscp = AT_CMD_CESQ_RSCP_MAX_VALUE;
    }

    return sCesqRscp;
}


VOS_INT8 AT_ConvertCerssiEcnoToCesqEcno(VOS_INT8 cEcno)
{
    VOS_INT8                            cCesqEcno;

    /*
        转换原则如下:
       【原有sEcioValue转换方法】
              0                        sEcioValue < -24dB
              1             -24dB   <= sEcioValue < -23.5dB
              2             -23.5dB <= sEcioValue < -23dB
              ...           ...
              47            -1dB    <= sEcioValue < -0.5dB
              48            -0.5dB  <= sEcioValue < 0dB
              49             0dB    <= sEcioValue
              255            not known or not detectable
       【新增cEcno接口, 比原有的sEcioValue增加0.5精度，目前仅为AT命令CESQ使用。范围为: -49~0】
              0                        cEcno < -48dB
              1             -48dB   <= cEcno < -47dB
              2             -47dB   <= cEcno < -46dB
              3             -46dB   <= cEcno < -45dB
              4             -45dB   <= cEcno < -44dB
              ...           ...
              47            -2dB    <= cEcno < -1dB
              48            -1dB    <= cEcno < 0dB
              49            0dB     <= cEcno
              255           not known or not detectable
    */

    if (cEcno < AT_CMD_CESQ_ECNO_LOWER_BOUNDARY_VALUE)
    {
        cCesqEcno = AT_CMD_CESQ_ECNO_MIN_VALUE;
    }
    else if (cEcno < AT_CMD_CESQ_ECNO_UPPER_BOUNDARY_VALUE)
    {
        cCesqEcno = cEcno + AT_CMD_CESQ_ECNO_CONVERT_BASE_VALUE;
    }
    else
    {
        cCesqEcno = AT_CMD_CESQ_ECNO_MAX_VALUE;
    }

    return cCesqEcno;
}


VOS_INT16 AT_ConvertCerssiRsrqToCesqRsrq(VOS_INT16 sCerssiRsrq)
{
    VOS_INT16                           sCesqRsrq;
    VOS_INT16                           sResult;

    /*
        转换原则如下:
       【Rsrq底层上报原值，这里除以4, 所以没有小数, 目前sCerssiRsrq按照-40~-6上报的】
              0                       sCerssiRsrq < -39dB
              1             -39dB  <= sCerssiRsrq < -38dB
              2             -38dB  <= sCerssiRsrq < -37dB
              3             -37dB  <= sCerssiRsrq < -36dB
              ...           ...
              33            -7dB   <= sCerssiRsrq < -6dB
              34            -6 dB  <= sCerssiRsrq
              255           not known or not detectable
    */

    sResult = sCerssiRsrq >> AT_SIGNAL_DIVISOR_TWO; /*lint !e702*/

    if (sResult < AT_CMD_CESQ_RSRQ_LOWER_BOUNDARY_VALUE)
    {
        sCesqRsrq = AT_CMD_CESQ_RSRQ_MIN_VALUE;
    }
    else if (sResult < AT_CMD_CESQ_RSRQ_UPPER_BOUNDARY_VALUE)
    {
        sCesqRsrq = sResult + AT_CMD_CESQ_RSRQ_CONVERT_BASE_VALUE;
    }
    else
    {
        sCesqRsrq = AT_CMD_CESQ_RSRQ_MAX_VALUE;
    }

    return sCesqRsrq;
}


VOS_INT16 AT_ConvertCerssiRsrpToCesqRsrp(VOS_INT16 sCerssiRsrp)
{
    VOS_INT16                           sCesqRsrp;

    /*
        转换原则如下:
              0                        sCerssiRsrp < -140dBm
              1             -140dBm <= sCerssiRsrp < -139dBm
              2             -139dBm <= sCerssiRsrp < -138dBm
              3             -138dBm <= sCerssiRsrp < -137dBm
              ...           ...
              96            -45dBm  <= sCerssiRsrp < -44 dBm
              97            -44dBm  <= sCerssiRsrp
              255            not known or not detectable
    */
    /* LRRC上报给空口的测量报告按照移位处理，此处需要保持一致 */
    sCerssiRsrp = sCerssiRsrp >> AT_SIGNAL_DIVISOR_THREE; /*lint !e702*/

    if (sCerssiRsrp < AT_CMD_CESQ_RSRP_LOWER_BOUNDARY_VALUE)
    {
        sCesqRsrp = AT_CMD_CESQ_RSRP_MIN_VALUE;
    }
    else if (sCerssiRsrp < AT_CMD_CESQ_RSRP_UPPER_BOUNDARY_VALUE)
    {
        sCesqRsrp = sCerssiRsrp + AT_CMD_CESQ_RSRP_CONVERT_BASE_VALUE;
    }
    else
    {
        sCesqRsrp = AT_CMD_CESQ_RSRP_MAX_VALUE;
    }

    return sCesqRsrp;
}


VOS_UINT32 AT_ProcCesqInfoSet(VOS_VOID *pstMsg)
{
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU                       *pstCerssiInfoQueryCnf = VOS_NULL_PTR;
    VOS_UINT16                                              usLength;
    VOS_UINT8                                               ucIndex;

    /* 初始化 */
    pstCerssiInfoQueryCnf  = (TAF_MMA_CERSSI_INFO_QRY_CNF_STRU*)pstMsg;
    usLength               = 0;
    ucIndex                = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index*/
    if (AT_FAILURE == At_ClientIdToUserId(pstCerssiInfoQueryCnf->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_ProcCesqInfoSet:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_ProcCesqInfoSet : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* GSM*/
    if (TAF_MMA_RAT_GSM == pstCerssiInfoQueryCnf->stCerssi.enRatType)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s: %d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          AT_ConvertCerssiRssiToCesqRxlev(pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stGCellSignInfo.sRssiValue),
                                          pstCerssiInfoQueryCnf->stCerssi.aRssi[0].ucChannalQual,
                                          AT_CMD_CESQ_RSCP_INVALID_VALUE,
                                          AT_CMD_CESQ_ECNO_INVALID_VALUE,
                                          AT_CMD_CESQ_RSRQ_INVALID_VALUE,
                                          AT_CMD_CESQ_RSRP_INVALID_VALUE
                                          );

        gstAtSendData.usBufLen = usLength;

        /* 输出结果 */
        At_FormatResultData(ucIndex, AT_OK);
        return VOS_OK;
    }


    if (TAF_MMA_RAT_WCDMA == pstCerssiInfoQueryCnf->stCerssi.enRatType)
    {
        /* FDD 3G小区*/
        if (TAF_UTRANCTRL_UTRAN_MODE_FDD == pstCerssiInfoQueryCnf->stCerssi.ucCurrentUtranMode)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                              "%s: %d,%d,%d,%d,%d,%d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              AT_CMD_CESQ_RXLEV_INVALID_VALUE,
                                              AT_CMD_CESQ_BER_INVALID_VALUE,
                                              AT_ConvertCerssiRscpToCesqRscp(pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.sRscpValue),
                                              AT_ConvertCerssiEcnoToCesqEcno(pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.cEcno),
                                              AT_CMD_CESQ_RSRQ_INVALID_VALUE,
                                              AT_CMD_CESQ_RSRP_INVALID_VALUE
                                              );
            gstAtSendData.usBufLen = usLength;

            /* 输出结果 */
            At_FormatResultData(ucIndex, AT_OK);
            return VOS_OK;
        }
        else
        {
            /* 非FDD 3G小区，ecio值为无效值255 */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                              "%s: %d,%d,%d,%d,%d,%d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              AT_CMD_CESQ_RXLEV_INVALID_VALUE,
                                              AT_CMD_CESQ_BER_INVALID_VALUE,
                                              AT_ConvertCerssiRscpToCesqRscp(pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stWCellSignInfo.sRscpValue),
                                              AT_CMD_CESQ_ECNO_INVALID_VALUE,
                                              AT_CMD_CESQ_RSRQ_INVALID_VALUE,
                                              AT_CMD_CESQ_RSRP_INVALID_VALUE
                                              );

            gstAtSendData.usBufLen = usLength;

            /* 输出结果 */
            At_FormatResultData(ucIndex, AT_OK);
            return VOS_OK;
        }
    }

#if ( FEATURE_ON == FEATURE_LTE )
    /* LTE*/
    if (TAF_MMA_RAT_LTE == pstCerssiInfoQueryCnf->stCerssi.enRatType)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s: %d,%d,%d,%d,%d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          AT_CMD_CESQ_RXLEV_INVALID_VALUE,
                                          AT_CMD_CESQ_BER_INVALID_VALUE,
                                          AT_CMD_CESQ_RSCP_INVALID_VALUE,
                                          AT_CMD_CESQ_ECNO_INVALID_VALUE,
                                          AT_ConvertCerssiRsrqToCesqRsrq(pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrq),
                                          AT_ConvertCerssiRsrpToCesqRsrp(pstCerssiInfoQueryCnf->stCerssi.aRssi[0].u.stLCellSignInfo.sRsrp)
                                          );

        gstAtSendData.usBufLen = usLength;

        /* 输出结果 */
        At_FormatResultData(ucIndex, AT_OK);
        return VOS_OK;
    }
#endif

    /* 刚开机没有接入模式，参数都返回无效值 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR *)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "%s: %d,%d,%d,%d,%d,%d",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                      AT_CMD_CESQ_RXLEV_INVALID_VALUE,
                                      AT_CMD_CESQ_BER_INVALID_VALUE,
                                      AT_CMD_CESQ_RSCP_INVALID_VALUE,
                                      AT_CMD_CESQ_ECNO_INVALID_VALUE,
                                      AT_CMD_CESQ_RSRQ_INVALID_VALUE,
                                      AT_CMD_CESQ_RSRP_INVALID_VALUE
                                      );

    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCerssiInfoQueryCnf(VOS_VOID *pstMsg)
{
    TAF_MMA_CERSSI_INFO_QRY_CNF_STRU   *pstCerssiInfoQueryCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstCerssiInfoQueryCnf   = (TAF_MMA_CERSSI_INFO_QRY_CNF_STRU*)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCerssiInfoQueryCnf->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCerssiInfoQueryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCerssiInfoQueryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断查询结果是否失败,如果失败则返回ERROR */
    if (TAF_ERR_NO_ERROR != pstCerssiInfoQueryCnf->enErrorCause)
    {
        At_FormatResultData(ucIndex, AT_ERROR);

        /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        return VOS_ERR;
    }
    else
    {

        switch (gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            case AT_CMD_CERSSI_READ:
                AT_ProcCerssiInfoQuery(pstCerssiInfoQueryCnf);
                break;

            case AT_CMD_CESQ_SET:
                AT_ProcCesqInfoSet(pstCerssiInfoQueryCnf);
                break;

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
            case AT_CMD_CSERSSI_READ:
                AT_ProcCserssiInfoQuery(pstCerssiInfoQueryCnf);
                break;
#endif

            default:
                return VOS_ERR;
        }
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaBodySarSetCnf(VOS_VOID *pstMsg)
{
    AT_MTA_MSG_STRU                    *pRcvMsg             = VOS_NULL_PTR;
    MTA_AT_RESULT_CNF_STRU             *pstBodySarSetCnf    = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg          = (AT_MTA_MSG_STRU*)pstMsg;
    pstBodySarSetCnf = (MTA_AT_RESULT_CNF_STRU*)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaBodySarSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaBodySarSetCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_BODYSARON_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断回复消息中的错误码 */
    if (MTA_AT_RESULT_NO_ERROR == pstBodySarSetCnf->enResult)
    {
        ulResult    = AT_OK;

        /* 更新BODYSAR 状态 */
        if (AT_MTA_BODY_SAR_OFF == g_enAtBodySarState)
        {
            g_enAtBodySarState = AT_MTA_BODY_SAR_ON;
        }
        else
        {
            g_enAtBodySarState = AT_MTA_BODY_SAR_OFF;
        }
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_VOID AT_ReportResetCmd(AT_RESET_REPORT_CAUSE_ENUM_UINT32 enCause)
{
    VOS_UINT16                          usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s%s%d%s",
                                        gaucAtCrLf,
                                        gastAtStringTab[AT_STRING_RESET].pucText,
                                        enCause,
                                        gaucAtCrLf);

    /* 需要上报给上层的双RIL */
    At_SendResultData(AT_CLIENT_ID_APP,  pgucAtSndCodeAddr, usLength);
    At_SendResultData(AT_CLIENT_ID_PCUI, pgucAtSndCodeAddr, usLength);

#if (MULTI_MODEM_NUMBER > 1)
#if (FEATURE_VCOM_EXT == FEATURE_ON)
    At_SendResultData(AT_CLIENT_ID_APP5, pgucAtSndCodeAddr, usLength);
#endif
    At_SendResultData(AT_CLIENT_ID_CTRL, pgucAtSndCodeAddr, usLength);
#if (MULTI_MODEM_NUMBER > 2)
#if (FEATURE_VCOM_EXT == FEATURE_ON)
    At_SendResultData(AT_CLIENT_ID_APP20, pgucAtSndCodeAddr, usLength);
#endif
    At_SendResultData(AT_CLIENT_ID_PCUI2, pgucAtSndCodeAddr, usLength);
#endif
#endif

    return;
}


VOS_VOID AT_StopAllTimer(VOS_VOID)
{
    VOS_UINT8                           ucModemIndex;
    VOS_UINT8                           ucClientIndex;
    VOS_UINT32                          ulTimerName;
    AT_MODEM_CC_CTX_STRU               *pstCcCtx         = VOS_NULL_PTR;
    AT_PARSE_CONTEXT_STRU              *pstParseContext = VOS_NULL_PTR;
    AT_CLIENT_MANAGE_STRU              *pstClientContext = VOS_NULL_PTR;

    for (ucModemIndex = 0; ucModemIndex < MODEM_ID_BUTT; ucModemIndex++)
    {
        pstCcCtx = AT_GetModemCcCtxAddrFromClientId(ucModemIndex);

        /* 停止S0定时器 */
        if (VOS_TRUE == pstCcCtx->stS0TimeInfo.bTimerStart)
        {
            ulTimerName = pstCcCtx->stS0TimeInfo.ulTimerName;

            AT_StopRelTimer(ulTimerName, &(pstCcCtx->stS0TimeInfo.s0Timer));
            pstCcCtx->stS0TimeInfo.bTimerStart = VOS_FALSE;
            pstCcCtx->stS0TimeInfo.ulTimerName = 0;
        }

    }

    for (ucClientIndex = 0; ucClientIndex < AT_MAX_CLIENT_NUM; ucClientIndex++)
    {
        pstParseContext = &(g_stParseContext[ucClientIndex]);
        AT_StopRelTimer(ucClientIndex, &pstParseContext->hTimer);

        pstClientContext = &(gastAtClientTab[ucClientIndex]);
        AT_StopRelTimer(ucClientIndex, &pstClientContext->hTimer);
    }

    return;
}


VOS_VOID AT_ResetParseCtx(VOS_VOID)
{
    VOS_UINT8                           ucClientIndex;
    AT_PARSE_CONTEXT_STRU              *pstParseContext = VOS_NULL_PTR;

    for (ucClientIndex = 0; ucClientIndex < AT_MAX_CLIENT_NUM; ucClientIndex++)
    {
        pstParseContext = &(g_stParseContext[ucClientIndex]);

        pstParseContext->ucClientStatus = AT_FW_CLIENT_STATUS_READY;

        /* 清空所有的缓存的AT命令 */
        AT_ClearBlockCmdInfo(ucClientIndex);

        /* 重置AT组合命令解析的信息 */
        At_ResetCombinParseInfo(ucClientIndex);

        pstParseContext->ucMode    = AT_NORMAL_MODE;
        pstParseContext->usDataLen = 0;
        pstParseContext->usCmdLineLen = 0;

        TAF_MEM_SET_S(pstParseContext->aucDataBuff, sizeof(pstParseContext->aucDataBuff), 0x00, AT_CMD_MAX_LEN);

        if (NULL != pstParseContext->pucCmdLine)
        {
            AT_FREE(pstParseContext->pucCmdLine);
            pstParseContext->pucCmdLine = NULL;
        }
    }

    return;
}


VOS_VOID AT_ResetClientTab(VOS_VOID)
{
    VOS_UINT8                           ucLoop;
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           ucClientIndex;

    for (ucClientIndex = 0; ucClientIndex < AT_MAX_CLIENT_NUM; ucClientIndex++)
    {
        /* 清空对应表项 */
        TAF_MEM_SET_S(&gastAtClientTab[ucClientIndex], sizeof(AT_CLIENT_MANAGE_STRU), 0x00, sizeof(AT_CLIENT_MANAGE_STRU));
    }

    /* USB PCUI */
    ucIndex = AT_CLIENT_TAB_PCUI_INDEX;
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_PCUI;
    gastAtClientTab[ucIndex].ucPortNo        = AT_USB_COM_PORT_NO;
    gastAtClientTab[ucIndex].UserType        = AT_USBCOM_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    /* USB Control */
    ucIndex = AT_CLIENT_TAB_CTRL_INDEX;
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_CTRL;
    gastAtClientTab[ucIndex].ucPortNo        = AT_CTR_PORT_NO;
    gastAtClientTab[ucIndex].UserType        = AT_CTR_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    ucIndex = AT_CLIENT_TAB_PCUI2_INDEX;
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_PCUI2;
    gastAtClientTab[ucIndex].ucPortNo        = AT_PCUI2_PORT_NO;
    gastAtClientTab[ucIndex].UserType        = AT_PCUI2_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    /* USB MODEM */
    ucIndex = AT_CLIENT_TAB_MODEM_INDEX;
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_MODEM;
    gastAtClientTab[ucIndex].ucPortType      = AT_USB_MODEM_PORT_NO;
    gastAtClientTab[ucIndex].ucDlci          = AT_MODEM_USER_DLCI;
    gastAtClientTab[ucIndex].ucPortNo        = AT_USB_MODEM_PORT_NO;
    gastAtClientTab[ucIndex].UserType        = AT_MODEM_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    /* NDIS MODEM */
    ucIndex = AT_CLIENT_TAB_NDIS_INDEX;
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_NDIS;
    gastAtClientTab[ucIndex].ucPortNo        = AT_NDIS_PORT_NO;
    gastAtClientTab[ucIndex].UserType        = AT_NDIS_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    /* USB UART */
    ucIndex = AT_CLIENT_TAB_UART_INDEX;
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_UART;
    gastAtClientTab[ucIndex].ucPortNo        = AT_UART_PORT_NO;
    gastAtClientTab[ucIndex].UserType        = AT_UART_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

#if (FEATURE_ON == FEATURE_AT_HSUART)
    /* HSUART */
    ucIndex = AT_CLIENT_TAB_HSUART_INDEX;
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_HSUART;
    gastAtClientTab[ucIndex].ucPortNo        = AT_HSUART_PORT_NO;
    gastAtClientTab[ucIndex].UserType        = AT_HSUART_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;
#endif

    /* SOCK */
    ucIndex = AT_CLIENT_TAB_SOCK_INDEX;
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_SOCK;
    gastAtClientTab[ucIndex].ucPortNo        = AT_SOCK_PORT_NO;
    gastAtClientTab[ucIndex].UserType        = AT_SOCK_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    /* APP SOCK */
    ucIndex = AT_CLIENT_TAB_APPSOCK_INDEX;
    gastAtClientTab[ucIndex].usClientId      = AT_CLIENT_ID_APPSOCK;
    gastAtClientTab[ucIndex].ucPortNo        = AT_APP_SOCK_PORT_NO;
    gastAtClientTab[ucIndex].UserType        = AT_APP_SOCK_USER;
    gastAtClientTab[ucIndex].ucUsed          = AT_CLIENT_USED;

    /* APP */
    for (ucLoop = 0; ucLoop < AT_VCOM_AT_CHANNEL_MAX; ucLoop++)
    {
        ucIndex = AT_CLIENT_TAB_APP_INDEX + ucLoop;
        gastAtClientTab[ucIndex].usClientId  = AT_CLIENT_ID_APP + ucLoop;
        gastAtClientTab[ucIndex].ucPortNo    = APP_VCOM_DEV_INDEX_0 + ucLoop;
        gastAtClientTab[ucIndex].UserType    = AT_APP_USER;
        gastAtClientTab[ucIndex].ucUsed      = AT_CLIENT_USED;
    }
}


VOS_VOID AT_ResetOtherCtx(VOS_VOID)
{
    /* <CR> */
    ucAtS3          = 13;

    gaucAtCrLf[0]   = ucAtS3;

    /* <LF> */
    ucAtS4          = 10;

    gaucAtCrLf[1]   = ucAtS4;

    /* <DEL> */
    ucAtS5          = 8;

    /* 指示命令返回码类型 */
    gucAtVType      = AT_V_ENTIRE_TYPE;

    /* 指示TE编码类型 */
    gucAtCscsType   = AT_CSCS_IRA_CODE;

    gucAtEType      = AT_E_ECHO_CMD;

    /* CONNECT <text> result code is given upon entering online data state.
       Dial tone and busy detection are disabled. */
    gucAtXType      = 0;

    /* 默认LTE上下行category都为6 */
    g_stAtDlRateCategory.ucLteUeDlCategory = 6;
    g_stAtDlRateCategory.ucLteUeUlCategory = 6;
}


VOS_UINT32 AT_RcvCcpuResetStartInd(
    VOS_VOID                           *pstMsg
)
{
    AT_PR_LOGI("enter %u", VOS_GetSlice());
    /* 上报^RESET:0命令 */
    AT_ReportResetCmd(AT_RESET_REPORT_CAUSE_CCPU_START);

    /* 停止所有启动的内部定时器 */
    AT_StopAllTimer();

    /* 初始化上下文信息 */
    AT_InitCtx();

    AT_PR_LOGI("nv write begin %u", VOS_GetSlice());


    AT_PR_LOGI("nv write end %u", VOS_GetSlice());

    /* 读取NV项 */
    AT_ReadNV();

    /* 装备初始化 */
    AT_InitDeviceCmd();

    /* STK初始化 */
    AT_InitStk();

    AT_PR_LOGI("nv read end %u", VOS_GetSlice());

    /* AT模块参数的初始化 */
    AT_InitPara();

    /* 重置客户端解析信息 */
    AT_ResetParseCtx();

    /* 重置用户信息 */
    AT_ResetClientTab();

    /* 重置其他散落的全局变量 */
    AT_ResetOtherCtx();

    AT_PR_LOGI("set modem state %u", VOS_GetSlice());

    /* C核单独复位后设置设备节点，当前未启动 */
    mdrv_set_modem_state(PS_FALSE);

    AT_PR_LOGI("leave %u", VOS_GetSlice());

    /* 释放信号量，使得调用API任务继续运行 */
    VOS_SmV(AT_GetResetSem());

    return VOS_OK;
}

VOS_UINT32 AT_RcvCcpuResetEndInd(
    VOS_VOID                           *pstMsg
)
{
    AT_PR_LOGI("enter %u", VOS_GetSlice());

    /* 设置复位完成的标志 */
    AT_SetResetFlag(VOS_FALSE);

    /* 上报^RESET:1命令 */
    AT_ReportResetCmd(AT_RESET_REPORT_CAUSE_CCPU_END);


    AT_PR_LOGI("leave %u", VOS_GetSlice());

    return VOS_OK;
}

VOS_UINT32 AT_RcvHifiResetStartInd(
    VOS_VOID                           *pstMsg
)
{
    AT_PR_LOGI("%u", VOS_GetSlice());

    /* 上报^RESET:2命令 */
    AT_ReportResetCmd(AT_RESET_REPORT_CAUSE_HIFI_START);

    AT_DBG_SAVE_HIFI_RESET_NUM(1);

    APP_VC_SendVcHifiResetNtf(I0_WUEPS_PID_VC);

#if (2 == MULTI_MODEM_NUMBER)
    APP_VC_SendVcHifiResetNtf(I1_WUEPS_PID_VC);
#endif

#if (3 == MULTI_MODEM_NUMBER)
    APP_VC_SendVcHifiResetNtf(I1_WUEPS_PID_VC);
    APP_VC_SendVcHifiResetNtf(I2_WUEPS_PID_VC);
#endif

    return VOS_OK;
}


VOS_UINT32 AT_RcvHifiResetEndInd(
    VOS_VOID                           *pstMsg
)
{
    AT_PR_LOGI("%u", VOS_GetSlice());

    /* 上报^RESET:3命令 */
    AT_ReportResetCmd(AT_RESET_REPORT_CAUSE_HIFI_END);

    return VOS_OK;
}


VOS_VOID AT_SpyMsgProc(VOS_VOID* pstMsg)
{
    TEMP_PROTECT_EVENT_AT_IND_STRU* pDataMsg = (TEMP_PROTECT_EVENT_AT_IND_STRU*)pstMsg;

    switch(pDataMsg->ulMsgId)
    {
#if ( FEATURE_ON == FEATURE_LTE )
        case ID_TEMPPRT_AT_EVENT_IND:
            AT_ProcTempprtEventInd(pDataMsg);
            break;
#endif

        case ID_TEMPPRT_STATUS_AT_EVENT_IND:
            AT_RcvTempprtStatusInd(pDataMsg);
            break;

        default:
            AT_WARN_LOG("AT_SpyMsgProc:WARNING:Wrong Msg!\n");
            break;
    }

    return;
}


VOS_UINT32 AT_RcvTempprtStatusInd(VOS_VOID *pMsg)
{
    TEMP_PROTECT_EVENT_AT_IND_STRU     *pstTempPrt = VOS_NULL_PTR;
    TAF_TEMP_PROTECT_CONFIG_STRU        stTempProtectPara;
    VOS_UINT32                          ulResult;

    TAF_MEM_SET_S(&stTempProtectPara, sizeof(stTempProtectPara), 0x00, sizeof(stTempProtectPara));

    /* 读取温度保护状态主动上报NV项 */
    ulResult = TAF_ACORE_NV_READ(MODEM_ID_0,
                                 en_NV_Item_TEMP_PROTECT_CONFIG,
                                 &stTempProtectPara,
                                 sizeof(TAF_TEMP_PROTECT_CONFIG_STRU));

    if (NV_OK != ulResult)
    {
        AT_ERR_LOG("AT_RcvTempprtStatusInd: Read NV fail");
        return VOS_ERR;
    }

    if (AT_TEMPPRT_STATUS_IND_ENABLE == stTempProtectPara.ucSpyStatusIndSupport)
    {
        pstTempPrt = (TEMP_PROTECT_EVENT_AT_IND_STRU*)pMsg;

        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s^THERM: %d%s",
                                                         gaucAtCrLf, pstTempPrt->ulTempProtectEvent, gaucAtCrLf);

        At_SendResultData(AT_BROADCAST_CLIENT_INDEX_MODEM_0, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

        return VOS_OK;
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaImeiVerifyQryCnf(VOS_VOID *pstMsg)
{
    AT_MTA_MSG_STRU                    *pRcvMsg       = VOS_NULL_PTR;
    VOS_UINT32                         *pulImeiVerify = VOS_NULL_PTR;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg       = (AT_MTA_MSG_STRU*)pstMsg;
    pulImeiVerify = (VOS_UINT32*)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaImeiVerifyQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaImeiVerifyQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_IMEI_VERIFY_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 打印^IMEIVERIFY */
    usLength  = 0;
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "^IMEIVERIFY: ");

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d",
                                       *pulImeiVerify);

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaCgsnQryCnf(VOS_VOID *pstMsg)
{
    VOS_UINT32                          ulI;
    VOS_UINT8                           ucCheckData;
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstRcvMsg;
    MTA_AT_CGSN_QRY_CNF_STRU           *pstCgsn;
    VOS_UINT8                           aucImeiAscii[TAF_PH_IMEI_LEN];

    /* 初始化 */
    ucCheckData   = 0;
    TAF_MEM_SET_S(aucImeiAscii, sizeof(aucImeiAscii), 0x00, TAF_PH_IMEI_LEN);

    pstRcvMsg = (AT_MTA_MSG_STRU*)pstMsg;
    pstCgsn   = (MTA_AT_CGSN_QRY_CNF_STRU *)pstRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCgsnQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCgsnQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CGSN_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 读取IMEI信息 */
    for (ulI = 0; ulI < (TAF_PH_IMEI_LEN - 2); ulI += 2)
    {
        aucImeiAscii[ulI]     = pstCgsn->aucImei[ulI] + 0x30;
        aucImeiAscii[ulI + 1UL] = pstCgsn->aucImei[ulI + 1UL] + 0x30;

        ucCheckData += (TAF_UINT8)(pstCgsn->aucImei[ulI]
                       +((pstCgsn->aucImei[ulI + 1UL] * 2) / 10)
                       +((pstCgsn->aucImei[ulI + 1UL] * 2) % 10));
    }
    ucCheckData = (10 - (ucCheckData%10)) % 10;

    aucImeiAscii[TAF_PH_IMEI_LEN - 2] = ucCheckData + 0x30;
    aucImeiAscii[TAF_PH_IMEI_LEN - 1] = 0;

    gstAtSendData.usBufLen  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      "%s",
                                      aucImeiAscii);

    g_enLogPrivacyAtCmd = TAF_LOG_PRIVACY_AT_CMD_CGSN;
    At_FormatResultData(ucIndex, AT_OK);
    g_enLogPrivacyAtCmd = TAF_LOG_PRIVACY_AT_CMD_BUTT;

    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaCopnInfoQueryCnf(VOS_VOID *pMsg)
{
    TAF_MMA_COPN_INFO_QRY_CNF_STRU     *pstCopnInfo = VOS_NULL_PTR;
    TAF_PH_OPERATOR_NAME_STRU          *pstPlmnName = VOS_NULL_PTR;
    VOS_UINT16                          usFromIndex;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulRet;

    /* 初始化 */
    pstCopnInfo   = (TAF_MMA_COPN_INFO_QRY_CNF_STRU *)pMsg;
    usLength      = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCopnInfo->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCopnInfoQueryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCopnInfoQueryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_COPN_QRY */
    if (AT_CMD_COPN_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 参照+COPS或^MMPLMNINFO命令处理 */
    pstPlmnName = (TAF_PH_OPERATOR_NAME_STRU *)pstCopnInfo->aucContent;

    for (ulLoop = 0; ulLoop < pstCopnInfo->usPlmnNum; ulLoop++)
    {
        /*  +COPN: <operator in numeric format><operator in long alphanumeric format>     */
        /* 输出命令名 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s: ",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        /* 输出数字格式运营商名称 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                           "\"%X%X%X",
                                           (TAF_MMA_PLMN_MCC_DIGIT3_MASK & pstPlmnName->PlmnId.Mcc) >> TAF_MMA_PLMN_MCC_DIGIT3_OFFSET,
                                           (TAF_MMA_PLMN_MCC_DIGIT2_MASK & pstPlmnName->PlmnId.Mcc) >> TAF_MMA_PLMN_MCC_DIGIT2_OFFSET,
                                           (TAF_MMA_PLMN_MCC_DIGIT1_MASK & pstPlmnName->PlmnId.Mcc) >> TAF_MMA_PLMN_MCC_DIGIT1_OFFSET);

        if (TAF_MMA_PLMN_MNC_DIGIT3_MASK != (TAF_MMA_PLMN_MNC_DIGIT3_MASK & pstPlmnName->PlmnId.Mnc))
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)(pgucAtSndCodeAddr + usLength),
                                               "%X",
                                               (TAF_MMA_PLMN_MNC_DIGIT3_MASK & pstPlmnName->PlmnId.Mnc) >> TAF_MMA_PLMN_MNC_DIGIT3_OFFSET);

        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%X%X\"",
                                           (TAF_MMA_PLMN_MNC_DIGIT2_MASK & pstPlmnName->PlmnId.Mnc) >> TAF_MMA_PLMN_MNC_DIGIT2_OFFSET,
                                           (TAF_MMA_PLMN_MNC_DIGIT1_MASK & pstPlmnName->PlmnId.Mnc) >> TAF_MMA_PLMN_MNC_DIGIT1_OFFSET);

        /* 添加逗号分隔符 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",\"");

        /* 输出运营商长名 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s",
                                           pstPlmnName->aucOperatorNameLong);

        /* 添加引号 */
        /* 添加逗号分隔符 */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "\"%s",
                                           gaucAtCrLf);

        At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        usLength = 0;

        pstPlmnName++;

    }

    /*
    AT模块根据MMA上报的运营商个数来确认运营商信息是否收集完成: 小于请求的运营商
    个数则认为运营商信息已经收集完成，原因如下所述:
    AT模块每次请求50条运营商信息，起始位置为已经输出的最后一个运营商的下一条运营商信息索引
    如果待输出的运营商信息总数不足50条，则按实际的输出，
    如果运营商数为50的倍数，则AT会再发一次运营商信息请求，MMA回复的运营商信息总数为0
    */
    if (pstCopnInfo->usPlmnNum < TAF_MMA_COPN_PLMN_MAX_NUM)
    {
        /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        usFromIndex = pstCopnInfo->usFromIndex + pstCopnInfo->usPlmnNum;

        ulRet = TAF_MMA_QryCopnInfoReq(WUEPS_PID_AT,
                                       gastAtClientTab[ucIndex].usClientId,
                                       usFromIndex,
                                       0);

        if (VOS_TRUE != ulRet)
        {
            /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
            AT_STOP_TIMER_CMD_READY(ucIndex);
            gstAtSendData.usBufLen = 0;
            At_FormatResultData(ucIndex, AT_ERROR);
        }
    }
    return VOS_OK;
}



VOS_UINT32 AT_RcvMtaSetNCellMonitorCnf(VOS_VOID *pstMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg = VOS_NULL_PTR;
    MTA_AT_RESULT_CNF_STRU                 *pstSetCnf = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pstMsg;
    pstSetCnf    = (MTA_AT_RESULT_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult     = AT_OK;
    ucIndex      = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetNCellMonitorCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetNCellMonitorCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_NCELL_MONITOR_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetNCellMonitorCnf : Current Option is not AT_CMD_NCELL_MONITOR_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR != pstSetCnf->enResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}


VOS_UINT32 AT_RcvMtaQryNCellMonitorCnf(VOS_VOID *pstMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg      = VOS_NULL_PTR;
    MTA_AT_NCELL_MONITOR_QRY_CNF_STRU      *pstMtaAtQryCnf = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;
    VOS_UINT16                              usLength;

    /* 初始化 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pstMsg;
    pstMtaAtQryCnf      = (MTA_AT_NCELL_MONITOR_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;
    usLength            = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryNCellMonitorCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryNCellMonitorCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_NCELL_MONITOR_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaQryNCellMonitorCnf : Current Option is not AT_CMD_NCELL_MONITOR_READ.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^NCELLMONITOR查询命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR != pstMtaAtQryCnf->enResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;

        /* 查询时，还需要上报邻区状态 */
        usLength        += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr,
                                                  (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                  "%s: %d,%d",
                                                  g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                  pstMtaAtQryCnf->ucSwitch,
                                                  pstMtaAtQryCnf->ucNcellState);

    }

    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaNCellMonitorInd(VOS_VOID *pstMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg      = VOS_NULL_PTR;
    MTA_AT_NCELL_MONITOR_IND_STRU          *pstMtaAtInd    = VOS_NULL_PTR;
    VOS_UINT8                               ucIndex;
    VOS_UINT16                              usLength;

    /* 初始化 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pstMsg;
    pstMtaAtInd         = (MTA_AT_NCELL_MONITOR_IND_STRU *)pstRcvMsg->aucContent;
    ucIndex             = 0;
    usLength            = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaNCellMonitorInd : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s^NCELLMONITOR: %d%s",
                                       gaucAtCrLf,
                                       pstMtaAtInd->ucNcellState,
                                       gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}


#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)

VOS_UINT32 AT_RcvMmaSimInsertRsp(VOS_VOID *pMsg)
{
    TAF_MMA_SIM_INSERT_CNF_STRU        *pstSimInsertRsp = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstSimInsertRsp = (TAF_MMA_SIM_INSERT_CNF_STRU *)pMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstSimInsertRsp->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSimInsertRsp : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSimInsertRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_SIMINSERT_SET */
    if (AT_CMD_SIMINSERT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_OK == pstSimInsertRsp->ulResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
#endif


VOS_UINT32 AT_RcvMtaRefclkfreqSetCnf(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_RESULT_CNF_STRU             *pstRltCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex     = 0;
    pstMtaMsg   = (AT_MTA_MSG_STRU *)pMsg;
    pstRltCnf   = (MTA_AT_RESULT_CNF_STRU *)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaRefclkfreqSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaRefclkfreqSetCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_REFCLKFREQ_SET */
    if ( AT_CMD_REFCLKFREQ_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_RcvMtaRefclkfreqSetCnf: WARNING:Not AT_CMD_REFCLKFREQ_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstRltCnf->enResult )
    {
        ulResult    = AT_OK;
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaRefclkfreqQryCnf(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_REFCLKFREQ_QRY_CNF_STRU     *pstRefclkfreqCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstMtaMsg           = (AT_MTA_MSG_STRU *)pMsg;
    pstRefclkfreqCnf    = (MTA_AT_REFCLKFREQ_QRY_CNF_STRU *)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaRefclkfreqQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaRefclkfreqQryCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_REFCLKFREQ_READ */
    if ( AT_CMD_REFCLKFREQ_READ != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_RcvMtaRefclkfreqQryCnf: WARNING:Not AT_CMD_REFCLKFREQ_READ!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstRefclkfreqCnf->enResult )
    {
        /* 输出GPS参考时钟信息，命令版本号默认为0 */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: 0,%d,%d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstRefclkfreqCnf->ulFreq,
                                                        pstRefclkfreqCnf->ulPrecision,
                                                        pstRefclkfreqCnf->enStatus);
        ulResult    = AT_OK;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
        ulResult    = AT_ERROR;
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaRefclkfreqInd(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_REFCLKFREQ_IND_STRU         *pstRefclkfreqInd;

    /* 初始化消息变量 */
    pstMtaMsg           = (AT_MTA_MSG_STRU*)pMsg;
    pstRefclkfreqInd    = (MTA_AT_REFCLKFREQ_IND_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaRefclkfreqInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 输出GPS参考时钟信息，命令版本号默认为0 */
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s%s: 0,%d,%d,%d%s",
                                                    gaucAtCrLf,
                                                    gastAtStringTab[AT_STRING_REFCLKFREQ].pucText,
                                                    pstRefclkfreqInd->ulFreq,
                                                    pstRefclkfreqInd->ulPrecision,
                                                    pstRefclkfreqInd->enStatus,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaHandleDectSetCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg           = VOS_NULL_PTR;
    MTA_AT_HANDLEDECT_SET_CNF_STRU     *pstSetCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf           = (MTA_AT_HANDLEDECT_SET_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaHandleDectSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaHandleDectSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_HANDLEDECT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaHandleDectSetCnf : Current Option is not AT_CMD_HANDLEDECT_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;
    if (MTA_AT_RESULT_NO_ERROR != pstSetCnf->enResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaHandleDectQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg           = VOS_NULL_PTR;
    MTA_AT_HANDLEDECT_QRY_CNF_STRU     *pstQryCnf         = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstQryCnf           = (MTA_AT_HANDLEDECT_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaHandleDectQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaHandleDectQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_HANDLEDECT_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaHandleDectQryCnf : Current Option is not AT_CMD_HANDLEDECT_QRY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^HANDLEDECT?查询命令返回 */
    gstAtSendData.usBufLen = 0;
    if (MTA_AT_RESULT_NO_ERROR != pstQryCnf->enResult)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;

        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        (VOS_UINT16)pstQryCnf->usHandle);

    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaPsTransferInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_PS_TRANSFER_IND_STRU        *pstPsTransferInd;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstMtaMsg           = (AT_MTA_MSG_STRU*)pMsg;
    pstPsTransferInd    = (MTA_AT_PS_TRANSFER_IND_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaPsTransferInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^PSTRANSFER: %d%s",
                                                    gaucAtCrLf,
                                                    pstPsTransferInd->ucCause,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaMipiInfoInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_RF_LCD_MIPICLK_IND_STRU     *pstMipiClkCnf;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstMtaMsg           = (AT_MTA_MSG_STRU*)pMsg;
    pstMipiClkCnf       = (MTA_AT_RF_LCD_MIPICLK_IND_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaMipiInfoInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^MIPICLK: %d%s",
                                                    gaucAtCrLf,
                                                    pstMipiClkCnf->usMipiClk,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaMipiInfoCnf(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_RF_LCD_MIPICLK_CNF_STRU     *pstMipiClkCnf;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstMtaMsg           = (AT_MTA_MSG_STRU*)pMsg;
    pstMipiClkCnf       = (MTA_AT_RF_LCD_MIPICLK_CNF_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaMipiInfoCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaMipiInfoCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_MIPI_CLK_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;
    if (VOS_OK != pstMipiClkCnf->usResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstMipiClkCnf->usMipiClk);
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_VOID AT_RcvSwitchCmdModeMsg(VOS_UINT8 ucIndex)
{
    AT_UART_LINE_CTRL_STRU             *pstLineCtrl = VOS_NULL_PTR;

    pstLineCtrl = AT_GetUartLineCtrlInfo();

    /* 处理原则(enPortIndex对应的端口):
     * (1) ONLINE-DATA模式 - 上报OK
     * (2) 其他模式        - 丢弃
     */

    /* 端口索引检查 */
    if (ucIndex >= AT_CLIENT_BUTT)
    {
        return;
    }

    /* 只处理UART端口检测到的切换命令 */
    if (VOS_TRUE != AT_CheckHsUartUser(ucIndex))
    {
        return;
    }

    /* 目前只支持PPP和IP模式下切换为ONLINE-COMMAND模式 */
    if ( (AT_DATA_MODE == gastAtClientTab[ucIndex].Mode)
      && ( (AT_PPP_DATA_MODE == gastAtClientTab[ucIndex].DataMode)
        || (AT_IP_DATA_MODE == gastAtClientTab[ucIndex].DataMode) ) )
    {
        if (AT_UART_DSR_MODE_CONNECT_ON == pstLineCtrl->enDsrMode)
        {
            AT_CtrlDSR(ucIndex, AT_IO_LEVEL_LOW);
        }

        At_SetMode(ucIndex, AT_ONLINE_CMD_MODE, AT_NORMAL_MODE);
        At_FormatResultData(ucIndex, AT_OK);
    }

    return;
}


VOS_VOID AT_RcvWaterLowMsg(VOS_UINT8 ucIndex)
{
    AT_UART_CTX_STRU                   *pstUartCtx = VOS_NULL_PTR;

    pstUartCtx = AT_GetUartCtxAddr();

    /* 端口索引检查 */
    if (ucIndex >= AT_CLIENT_BUTT)
    {
        return;
    }

    /* 只处理UART端口的低水线消息 */
    if (VOS_TRUE != AT_CheckHsUartUser(ucIndex))
    {
        return;
    }

    /* TX低水线处理 */
    if (VOS_NULL_PTR != pstUartCtx->pWmLowFunc)
    {
        pstUartCtx->pWmLowFunc(ucIndex);
    }

    return;
}

#if (FEATURE_ON == FEATURE_DSDS)

VOS_UINT32 AT_RcvMtaPsProtectSetCnf(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                    *pRcvMsg;
    MTA_AT_RRC_PROTECT_PS_CNF_STRU     *pstEvent;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;
    /* 初始化消息变量 */
    pRcvMsg = (AT_MTA_MSG_STRU*)pMsg;
    pstEvent = (MTA_AT_RRC_PROTECT_PS_CNF_STRU*)pRcvMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaPsProtectSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaPsProtectSetCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_PSPROTECTMODE_SET */
    if ( AT_CMD_PSPROTECTMODE_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_RcvMtaCgpsClockSetCnf: WARNING:Not AT_CMD_CGPSCLOCK_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstEvent->enResult )
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
#endif


VOS_UINT32 AT_RcvMtaPhyInitCnf(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                    *pRcvMsg;
    MTA_AT_PHY_INIT_CNF_STRU           *pstEvent;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    pRcvMsg = (AT_MTA_MSG_STRU*)pMsg;
    pstEvent = (MTA_AT_PHY_INIT_CNF_STRU*)pRcvMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId,&ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaPhyInitCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaPhyInitCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_PHYINIT_SET */
    if ( AT_CMD_PHYINIT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_RcvMtaPhyInitCnf: WARNING:Not AT_CMD_PHYINIT!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstEvent->enResult )
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormATResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}




VOS_UINT32 AT_RcvMtaEcidSetCnf(VOS_VOID *pMsg)
{
    /* 定义局部变量 */
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_ECID_SET_CNF_STRU           *pstEcidSetCnf;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex         = 0;
    pstMtaMsg       = (AT_MTA_MSG_STRU*)pMsg;
    pstEcidSetCnf   = (MTA_AT_ECID_SET_CNF_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaEcidSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaEcidSetCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_REFCLKFREQ_READ */
    if ( AT_CMD_ECID_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_RcvMtaEcidSetCnf: WARNING:Not AT_CMD_REFCLKFREQ_READ!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstEcidSetCnf->enResult )
    {
        /* 输出查询的增强型小区信息 */
        /* +ECID=<version>,<rat>,[<cell_description>] */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s=%s",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstEcidSetCnf->aucCellInfoStr);
        ulResult    = AT_OK;
    }
    else
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s=%s",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        "0,NONE:,");

        ulResult    = AT_OK;
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaSysCfgSetCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_SYS_CFG_CNF_STRU           *pstCnfMsg;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstCnfMsg = (TAF_MMA_SYS_CFG_CNF_STRU *)pMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCnfMsg->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSysCfgSetCnf : WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSysCfgSetCnf : AT_BROADCAST_INDEX.");

        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstCnfMsg->enRslt)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstCnfMsg->enErrorCause);
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaPhoneModeSetCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_PHONE_MODE_SET_CNF_STRU    *pstCnfMsg;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;
    MODEM_ID_ENUM_UINT16                enModemId;
#if(FEATURE_ON == FEATURE_LTE)
    VOS_UINT8                           ucSptLteFlag;
    VOS_UINT8                           ucSptUtralTDDFlag;
#endif

    pstCnfMsg           = (TAF_MMA_PHONE_MODE_SET_CNF_STRU *)pMsg;
    ucIndex             = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    enModemId           = MODEM_ID_0;
    ulResult            = AT_FAILURE;

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG1("AT_RcvMmaPhoneModeSetCnf:Get ModemID From ClientID fail,ClientID:", ucIndex);

        return VOS_ERR;
    }

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCnfMsg->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSysCfgSetCnf : WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPhoneModeSetCnf : AT_BROADCAST_INDEX.");

        return VOS_ERR;
    }

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstCnfMsg->enRslt)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstCnfMsg->enErrorCause);       /* 发生错误 */
    }




    /* V7R2 ^PSTANDBY命令复用关机处理流程 */
#ifdef FEATURE_UPGRADE_TL
    if(AT_CMD_PSTANDBY_SET == (AT_LTE_CMD_CURRENT_OPT_ENUM)gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        return VOS_OK;
    }
#endif

#if((FEATURE_ON == FEATURE_LTE) || (FEATURE_ON == FEATURE_UE_MODE_TDS))
    /* 如果GU处理结果正确，则发送到TL测并等待结果 */
    if (ulResult == AT_OK)
    {
        ucSptLteFlag        = AT_IsModemSupportRat(enModemId, TAF_MMA_RAT_LTE);
        ucSptUtralTDDFlag   = AT_IsModemSupportUtralTDDRat(enModemId);

        if ((VOS_TRUE       == ucSptLteFlag)
         || (VOS_TRUE       == ucSptUtralTDDFlag))
        {
            if ((AT_CMD_TMODE_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
             || (AT_CMD_SET_TMODE == gastAtClientTab[ucIndex].CmdCurrentOpt))
            {
                AT_ProcOperModeWhenLteOn(ucIndex);
                return VOS_OK;
            }
        }
    }
#endif

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaDetachCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_DETACH_CNF_STRU            *pstDetachCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstDetachCnf = (TAF_MMA_DETACH_CNF_STRU *)pMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstDetachCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaDetachCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaDetachCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstDetachCnf->enRslt)
    {
        ulResult = AT_OK;
    }
    else
    {
        /* gastAtClientTab[ucIndex].ulCause没有使用点，赋值点删除 */
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaAttachCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_ATTACH_CNF_STRU            *pstAttachCnf;
    VOS_UINT8                           ucIndex;

    pstAttachCnf = (TAF_MMA_ATTACH_CNF_STRU *)pstMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstAttachCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAttachCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 去除广播CLIENT类型的情况 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAttachCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 停止保护定时器 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 只输出结果(OK/ERROR)，不输出其他信息 */
    gstAtSendData.usBufLen = 0;

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstAttachCnf->enRslt)
    {
        /* 调用At_FormatResultData发送命令结果 */
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        /* gastAtClientTab[ucIndex].ulCause没有使用点，赋值点删除 */

        /* 调用At_FormatResultData发送命令结果 */
        At_FormatResultData(ucIndex, AT_ERROR);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaAttachStatusQryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_ATTACH_STATUS_QRY_CNF_STRU *pstAttachStatusQryCnf;
    VOS_UINT8                           ucIndex;

    pstAttachStatusQryCnf = (TAF_MMA_ATTACH_STATUS_QRY_CNF_STRU *)pstMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstAttachStatusQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAttachStatusQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 去除广播CLIENT类型的情况 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAttachStatusQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 检查当前操作类型 */
    if (AT_CMD_MMA_GET_CURRENT_ATTACH_STATUS != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaAttachStatusQryCnf : CmdCurrentOpt Not Match.");
        return VOS_ERR;
    }

    /* 停止保护定时器 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 检查查询结果 */
    if (TAF_MMA_APP_OPER_RESULT_SUCCESS != pstAttachStatusQryCnf->enRslt)
    {
        At_FormatResultData(ucIndex,AT_ERROR);
        return AT_ERROR;
    }

    /* 根据Domain Type输出返回结果 */
    if (TAF_MMA_SERVICE_DOMAIN_CS_PS == pstAttachStatusQryCnf->enDomainType)
    {
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR*)pgucAtSndCodeAddr,
                                                        "%s: %d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        (VOS_UINT32)pstAttachStatusQryCnf->enPsStatus,
                                                        (VOS_UINT32)pstAttachStatusQryCnf->enCsStatus);

        At_FormatResultData(ucIndex,AT_OK);
    }
    else if (TAF_MMA_SERVICE_DOMAIN_PS == pstAttachStatusQryCnf->enDomainType)
    {
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR*)pgucAtSndCodeAddr,
                                                        "%s: %d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        (VOS_UINT32)pstAttachStatusQryCnf->enPsStatus);

        At_FormatResultData(ucIndex,AT_OK);
    }
    else
    {
        At_FormatResultData(ucIndex,AT_ERROR);
        return AT_ERROR;
    }

    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaQryEmcCallBackCnf(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_MMA_EMC_CALL_BACK_QRY_CNF_STRU                     *pstQryEmcCallBackCnf;
    VOS_UINT16                                              usLength;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex            = 0;
    usLength           = 0;
    pstQryEmcCallBackCnf = (TAF_MMA_EMC_CALL_BACK_QRY_CNF_STRU*)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstQryEmcCallBackCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaQryEmcCallBackCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaQryEmcCallBackCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_EMCCBM_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaQuitCallBackCnf : Current Option is not AT_CMD_EMCCBM_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^QCCB查询命令返回 */
    gstAtSendData.usBufLen = 0;

    if (TAF_ERR_NO_ERROR == pstQryEmcCallBackCnf->enErrorCode)
    {
        ulResult = AT_OK;
        usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (TAF_CHAR *)pgucAtSndCodeAddr,
                                         (TAF_CHAR *)pgucAtSndCodeAddr,
                                          "%s: %d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstQryEmcCallBackCnf->ucIsInCallBackMode);
        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        ulResult = AT_CME_UNKNOWN;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaEmcCallBackNtf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_EMC_CALL_BACK_NTF_STRU     *pstEmcCallBack;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstEmcCallBack      = (TAF_MMA_EMC_CALL_BACK_NTF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstEmcCallBack->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaEmcCallBackNtf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^EMCCBM: %d%s",
                                                    gaucAtCrLf,
                                                    pstEmcCallBack->ulIsInCallBack,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_ECALL)

VOS_UINT32 AT_RcvVcMsgSetMsdCnfProc(
    MN_AT_IND_EVT_STRU                 *pstData
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;
    APP_VC_SET_MSD_CNF_STRU            *pstRslt  = VOS_NULL_PTR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvVcMsgSetMsdCnfProc:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvVcMsgSetMsdCnfProc : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECLMSD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvVcMsgSetMsdCnfProc:WARNING:AT ARE WAITING ANOTHER CMD!");
        return VOS_ERR;
    }

    pstRslt = (APP_VC_SET_MSD_CNF_STRU *)pstData->aucContent;

    if (VOS_OK == pstRslt->ucRslt)
    {
        ulRet = AT_OK;
    }
    else
    {
        ulRet = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return VOS_OK;
}


VOS_UINT32 AT_RcvVcMsgQryMsdCnfProc(
    MN_AT_IND_EVT_STRU                 *pstData
)
{
    APP_VC_MSG_QRY_MSD_CNF_STRU        *pstQryMsd  = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;
    VOS_UINT16                          i;
    VOS_UINT16                          usLength;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvVcMsgQryMsdCnfProc:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvVcMsgQryMsdCnfProc : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECLMSD_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvVcMsgQryMsdCnfProc:WARNING:AT ARE WAITING ANOTHER CMD!");
        return VOS_ERR;
    }

    /* 初始化 */
    pstQryMsd = (APP_VC_MSG_QRY_MSD_CNF_STRU *)pstData->aucContent;

    if (VOS_OK == pstQryMsd->ucQryRslt)
    {
        /* 输出查询结果 */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: \"",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        for (i = 0; i < APP_VC_MSD_DATA_LEN; i++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                        "%02x",
                                                        pstQryMsd->aucMsdData[i]);
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                        "\"");

        gstAtSendData.usBufLen = usLength;
        ulRet = AT_OK;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
        ulRet = AT_ERROR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return VOS_OK;
}


VOS_UINT32 AT_RcvVcMsgQryEcallCfgCnfProc(
    MN_AT_IND_EVT_STRU                 *pstData
)
{
    APP_VC_MSG_QRY_ECALL_CFG_CNF_STRU  *pstQryCfg  = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRet;



    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstData->clientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvVcMsgQryEcallCfgCnfProc:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvVcMsgQryEcallCfgCnfProc : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECLCFG_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvVcMsgQryEcallCfgCnfProc:WARNING:AT ARE WAITING ANOTHER CMD!");
        return VOS_ERR;
    }

    /* 初始化 */
    pstQryCfg  = (APP_VC_MSG_QRY_ECALL_CFG_CNF_STRU *)pstData->aucContent;

    if (VOS_OK == pstQryCfg->ucQryRslt)
    {
        /* 输出查询结果 */
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstQryCfg->enMode,
                                                        pstQryCfg->enVocConfig);
        ulRet = AT_OK;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
        ulRet = AT_ERROR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return VOS_OK;
}
#endif

VOS_UINT32 AT_RcvMmaQuitCallBackCnf(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_QUIT_CALLBACK_SET_CNF_STRU *pstQccbCnf;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex            = 0;
    pstQccbCnf = (TAF_MMA_QUIT_CALLBACK_SET_CNF_STRU*)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstQccbCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaQuitCallBackCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaQuitCallBackCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_QCCB_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaQuitCallBackCnf : Current Option is not AT_CMD_QCCB_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^QCCB查询命令返回 */
    gstAtSendData.usBufLen = 0;

    if (TAF_ERR_NO_ERROR == pstQccbCnf->enErrorCode)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)


VOS_UINT32 AT_RcvMmaCLocInfoQueryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_CDMA_LOCINFO_QRY_CNF_STRU  *pstCLocInfoQueryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;


    /* 初始化 */
    pstCLocInfoQueryCnf = (TAF_MMA_CDMA_LOCINFO_QRY_CNF_STRU*)pstMsg;
    usLength            = 0;
    ulResult            = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCLocInfoQueryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCLocInfoQueryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCLocInfoQueryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CLOCINFO_QRY */
    if (AT_CMD_CLOCINFO_QRY == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        if (VOS_OK == pstCLocInfoQueryCnf->ulRslt)
        {


            usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                             "%s: %d,%d,%d,%d,%d,%d,%d,%d,%d",
                                             g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                             pstCLocInfoQueryCnf->stClocinfoPara.ucRatMode,
                                             pstCLocInfoQueryCnf->stClocinfoPara.usPrevInUse,
                                             pstCLocInfoQueryCnf->stClocinfoPara.ulMcc,
                                             pstCLocInfoQueryCnf->stClocinfoPara.ulMnc,
                                             pstCLocInfoQueryCnf->stClocinfoPara.lSid,
                                             pstCLocInfoQueryCnf->stClocinfoPara.lNid,
                                             pstCLocInfoQueryCnf->stClocinfoPara.ulBaseId,
                                             pstCLocInfoQueryCnf->stClocinfoPara.lBaseLatitude,
                                             pstCLocInfoQueryCnf->stClocinfoPara.lBaseLongitude);

            gstAtSendData.usBufLen = usLength;
        }
        else
        {
            /* IsClocInfoValid = VOS_FALSE; */

            /* 查询失败，上报^CLOCINFO: 0*/
            usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                             (VOS_CHAR *)pgucAtSndCodeAddr,
                                             (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                             "%s: %d",
                                             g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                             VOS_FALSE);

            gstAtSendData.usBufLen = usLength;
        }
    }
    else
    {
        return VOS_ERR;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCSidInd(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_CSID_IND_STRU              *pstCSidInd;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstCSidInd       = (TAF_MMA_CSID_IND_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstCSidInd->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaCSidInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^CSID: %d%s",
                                                    gaucAtCrLf,
                                                    pstCSidInd->lSid,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}



#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)

VOS_UINT32 AT_RcvMmaSetCSidListCnf(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_CSIDLIST_SET_CNF_STRU      *pstSetCSidListCnf;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex            = 0;
    pstSetCSidListCnf = (TAF_MMA_CSIDLIST_SET_CNF_STRU*)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstSetCSidListCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSetCSidListCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSetCSidListCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CSIDLIST_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaSetCSidListCnf : Current Option is not AT_CMD_CSIDLIST_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CSIDLIST设置命令 */
    gstAtSendData.usBufLen = 0;

    if (TAF_ERR_NO_ERROR == pstSetCSidListCnf->enErrorCode)
    {
     ulResult = AT_OK;
    }
    else
    {
     ulResult = AT_ERROR;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}
#endif


VOS_UINT32 AT_RcvMmaQryCurrSidNidCnf(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_MMA_CURR_SID_NID_QRY_CNF_STRU                      *pstQryCurrSidNidCnf = VOS_NULL_PTR;
    VOS_UINT16                                              usLength;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex            = 0;
    usLength           = 0;
    pstQryCurrSidNidCnf = (TAF_MMA_CURR_SID_NID_QRY_CNF_STRU*)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstQryCurrSidNidCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaQryCurrSidNidCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaQryCurrSidNidCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CSNID_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaQryCurrSidNidCnf : Current Option is not AT_CMD_EMCCBM_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CSNID查询命令返回 */
    gstAtSendData.usBufLen = 0;

    if (TAF_ERR_NO_ERROR == pstQryCurrSidNidCnf->enErrorCode)
    {
        ulResult = AT_OK;
        usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (TAF_CHAR *)pgucAtSndCodeAddr,
                                         (TAF_CHAR *)pgucAtSndCodeAddr,
                                          "%s: %d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstQryCurrSidNidCnf->lSid,
                                          pstQryCurrSidNidCnf->lNid);

        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaRoamingModeSwitchInd(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_ROAMING_MODE_SWITCH_IND_STRU                   *pstRoamingModeSwitchInd = VOS_NULL_PTR;
    VOS_UINT8                                               ucIndex;

    /* 初始化消息变量 */
    ucIndex                 = 0;
    pstRoamingModeSwitchInd = (TAF_MMA_ROAMING_MODE_SWITCH_IND_STRU *)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRoamingModeSwitchInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaRoamingModeSwitchInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^ROAMINGMODESWITCH%s",
                                                    gaucAtCrLf,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}

VOS_UINT32 AT_RcvMmaCombinedModeSwitchInd(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_COMBINED_MODE_SWITCH_IND_STRU                  *pstCombinedModeSwitchInd = VOS_NULL_PTR;
    VOS_UINT8                                               ucIndex;

    /* 初始化消息变量 */
    ucIndex = 0;
    pstCombinedModeSwitchInd = (TAF_MMA_COMBINED_MODE_SWITCH_IND_STRU *)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstCombinedModeSwitchInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaRoamingModeSwitchInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^RATCOMBINEDMODE: %d%s",
                                                    gaucAtCrLf,
                                                    pstCombinedModeSwitchInd->enCombinedMode,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaRatCombinedModeQryCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_MMA_RAT_COMBINED_MODE_QRY_CNF_STRU                 *pstRcvMsg;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_MMA_RAT_COMBINED_MODE_QRY_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaRatCombinedModeQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaRatCombinedModeQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_RATCOMBINEDMODE_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaRatCombinedModeQryCnf : Current Option is not AT_CMD_PRLID_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s^RATCOMBINEDMODE: %d%s",
                                       gaucAtCrLf,
                                       pstRcvMsg->enCombinedMode,
                                       gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaIccAppTypeSwitchInd(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_ICC_APP_TYPE_SWITCH_IND_STRU                   *pstIccAppTypeSwitchInd;
    VOS_UINT8                                               ucIndex;

    /* 初始化消息变量 */
    ucIndex = 0;
    pstIccAppTypeSwitchInd = (TAF_MMA_ICC_APP_TYPE_SWITCH_IND_STRU *)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstIccAppTypeSwitchInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaIccAppTypeSwitchInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^ICCAPPTYPESWITCH: %d%s",
                                                    gaucAtCrLf,
                                                    pstIccAppTypeSwitchInd->enSwitchState,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCtRoamInfoCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_MMA_CTCC_ROAMING_NW_INFO_QRY_CNF_STRU              *pstQryMultiModeSidMcc = VOS_NULL_PTR;
    VOS_UINT16                                              usLength;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex            = 0;
    usLength           = 0;
    pstQryMultiModeSidMcc = (TAF_MMA_CTCC_ROAMING_NW_INFO_QRY_CNF_STRU *)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstQryMultiModeSidMcc->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCtRoamInfoCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCtRoamInfoCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CTROAMINFO_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCtRoamInfoCnf : Current Option is not AT_CMD_MULTIMODESIDMCC_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CSNID查询命令返回 */
    gstAtSendData.usBufLen = 0;

    ulResult = AT_OK;
    usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (TAF_CHAR *)pgucAtSndCodeAddr,
                                     (TAF_CHAR *)pgucAtSndCodeAddr,
                                      "%s: %d,%x%x%x,%x%x%x,%d",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                      pstQryMultiModeSidMcc->lSid,
                                      (pstQryMultiModeSidMcc->ul3Gpp2Mcc & 0x0f00)>>8,
                                      (pstQryMultiModeSidMcc->ul3Gpp2Mcc & 0xf0)>>4,
                                      (pstQryMultiModeSidMcc->ul3Gpp2Mcc & 0x0f),
                                      (pstQryMultiModeSidMcc->ul3GppMcc & 0x0f00)>>8,
                                      (pstQryMultiModeSidMcc->ul3GppMcc & 0xf0)>>4,
                                      (pstQryMultiModeSidMcc->ul3GppMcc & 0x0f),
                                      pstQryMultiModeSidMcc->ulModeType);

    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCtRoamingInfoChgInd(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_MMA_CTCC_ROAMING_NW_INFO_REPORT_IND_STRU           *pstMultiModeSidMccChgInd = VOS_NULL_PTR;
    VOS_UINT16                                              usLength;

    /* 初始化消息变量 */
    ucIndex            = 0;
    usLength           = 0;
    pstMultiModeSidMccChgInd = (TAF_MMA_CTCC_ROAMING_NW_INFO_REPORT_IND_STRU *)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMultiModeSidMccChgInd->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCtRoamingInfoChgInd : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 格式化AT^CSNID查询命令返回 */
    gstAtSendData.usBufLen = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     "%s^CTROAMINFO: %d,%x%x%x,%x%x%x,%d%s",
                                     gaucAtCrLf,
                                     pstMultiModeSidMccChgInd->lSid,
                                     (pstMultiModeSidMccChgInd->ul3Gpp2Mcc & 0x0f00)>>8,
                                     (pstMultiModeSidMccChgInd->ul3Gpp2Mcc & 0xf0)>>4,
                                     (pstMultiModeSidMccChgInd->ul3Gpp2Mcc & 0x0f),
                                     (pstMultiModeSidMccChgInd->ul3GppMcc & 0x0f00)>>8,
                                     (pstMultiModeSidMccChgInd->ul3GppMcc & 0xf0)>>4,
                                     (pstMultiModeSidMccChgInd->ul3GppMcc & 0x0f),
                                     pstMultiModeSidMccChgInd->ulModeType,
                                     gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCtOosCountCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_CTCC_OOS_COUNT_SET_CNF_STRU                    *pstOosCountCnf = VOS_NULL_PTR;
    VOS_UINT8                                               ucIndex;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex            = 0;
    pstOosCountCnf = (TAF_MMA_CTCC_OOS_COUNT_SET_CNF_STRU*)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstOosCountCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCtOosCountCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCtOosCountCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CTOOSCOUNT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCtOosCountCnf : Current Option is not AT_CMD_CTOOSCOUNT_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^QCCB查询命令返回 */
    gstAtSendData.usBufLen = 0;

    if (TAF_ERR_NO_ERROR == pstOosCountCnf->enErrorCode)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT)


VOS_UINT32 AT_RcvXcallEncryptCallCnf(
    VOS_VOID                                               *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_ENCRYPT_VOICE_CNF_STRU                    *pstRcvMsg;
    VOS_UINT32                                              ulResult;
    AT_ENCRYPT_VOICE_ERROR_ENUM_UINT32                      enEcnErr;
#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE)
    TAF_CHAR                                                acNewDocName[AT_TEST_ECC_FILE_NAME_MAX_LEN];
    VOS_INT32                                               lLength;

    TAF_MEM_SET_S(acNewDocName, (VOS_SIZE_T)sizeof(acNewDocName), 0x00, (VOS_SIZE_T)sizeof(acNewDocName));
    lLength = 0;
#endif

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_ENCRYPT_VOICE_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallEncryptCallCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallEncryptCallCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECCALL_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallEncryptCallCnf : Current Option is not AT_CMD_ECCALL_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;
    /* 格式化AT^ECCALL  SET命令返回 */
    if (TAF_CALL_APP_ENCRYPT_VOICE_SUCC == pstRcvMsg->enEccVoiceStatus)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
        enEcnErr = AT_MapEncVoiceErr(pstRcvMsg->enEccVoiceStatus);

        /* 密话主叫结果其他原因值上报 */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s^ECCALL: %d%s",
                                                        gaucAtCrLf,
                                                        enEcnErr,
                                                        gaucAtCrLf);

        At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);
#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE)
        lLength = VOS_sprintf_s(acNewDocName, (VOS_SIZE_T)sizeof(acNewDocName), "/data/hisi_logs/modem_log/ECC_TEST/Encrypted_call_data%d", gucAtCurrEncVoiceTestFileNum);
        if (lLength <= 0)
        {
            AT_WARN_LOG("AT_RcvXcallEncryptCallCnf(): VOS_sprintf_s len <= 0");

            return VOS_ERR;
        }

        lLength = VOS_sprintf_s((VOS_CHAR *)(acNewDocName + lLength), ((VOS_SIZE_T)sizeof(acNewDocName) - lLength), "_[%d]records.txt", gulAtCurrEncVoiceDataCount);

        if (lLength <= 0)
        {
            AT_WARN_LOG("AT_RcvXcallEncryptCallCnf(): VOS_sprintf_s len <= 0 (2)");

            return VOS_ERR;
        }

        (VOS_VOID)mdrv_file_rename(g_pacCurrEncVoiceDataWriteFilePath[gucCurrEncVoiceDataWriteFileNum], acNewDocName);
#endif
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvXcallEncryptCallInd(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_ENCRYPT_VOICE_IND_STRU                    *pstRcvMsg;
    VOS_UINT8                                               aucAsciiNum[MN_CALL_MAX_CALLED_BCD_NUM_LEN + 1];
    AT_ENCRYPT_VOICE_ERROR_ENUM_UINT32                      enEcnErr;
#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE)
    TAF_CHAR                                                acNewDocName[AT_TEST_ECC_FILE_NAME_MAX_LEN];
    VOS_INT32                                               lLength;

    TAF_MEM_SET_S(acNewDocName, (VOS_SIZE_T)sizeof(acNewDocName), 0x00, (VOS_SIZE_T)sizeof(acNewDocName));
    lLength = 0;
#endif

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_ENCRYPT_VOICE_IND_STRU*)pstMsg;
    TAF_MEM_SET_S(aucAsciiNum, sizeof(aucAsciiNum), 0x00, MN_CALL_MAX_CALLED_BCD_NUM_LEN + 1);

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallEncryptCallInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    enEcnErr = AT_MapEncVoiceErr(pstRcvMsg->enEccVoiceStatus);

    /* AT^ECCALL ind命令返回 */
    if (AT_ENCRYPT_VOICE_MT_CALL_NOTIFICATION == enEcnErr)
    {
        /* 密话被叫主动上报 */
        AT_BcdNumberToAscii(pstRcvMsg->stCallingNumber.aucBcdNum,
                            pstRcvMsg->stCallingNumber.ucNumLen,
                            (VOS_CHAR *)aucAsciiNum);
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s^ECCALL: %d,\"%s\"%s",
                                                        gaucAtCrLf,
                                                        enEcnErr,
                                                        aucAsciiNum,
                                                        gaucAtCrLf);
    }
    else if (AT_ENCRYPT_VOICE_ERROR_ENUM_BUTT == enEcnErr)
    {
        /* 异常场景 */
        return VOS_ERR;
    }
    else
    {
        /* 密话主叫结果其他原因值上报 */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s^ECCALL: %d%s",
                                                        gaucAtCrLf,
                                                        enEcnErr,
                                                        gaucAtCrLf);

#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE)
        lLength = VOS_sprintf_s(acNewDocName, (VOS_SIZE_T)sizeof(acNewDocName), "/data/hisi_logs/modem_log/ECC_TEST/Encrypted_call_data%d", gucAtCurrEncVoiceTestFileNum);
        if (lLength <= 0)
        {
            AT_WARN_LOG("AT_RcvXcallEncryptCallInd(): VOS_sprintf_s len <= 0");

            return VOS_ERR;
        }

        lLength = VOS_sprintf_s((VOS_CHAR *)(acNewDocName + lLength), ((VOS_SIZE_T)sizeof(acNewDocName) - lLength), "_[%d]records.txt", gulAtCurrEncVoiceDataCount);

        if (lLength <= 0)
        {
            AT_WARN_LOG("AT_RcvXcallEncryptCallInd(): VOS_sprintf_s len <= 0 (2)");

            return VOS_ERR;
        }

        if (AT_ENCRYPT_VOICE_SUCC != enEcnErr)
        {
            (VOS_VOID)mdrv_file_rename(g_pacCurrEncVoiceDataWriteFilePath[gucCurrEncVoiceDataWriteFileNum], acNewDocName);

            TAF_MEM_CPY_S(gacAtCurrDocName, (VOS_SIZE_T)sizeof(gacAtCurrDocName), acNewDocName, (VOS_SIZE_T)sizeof(acNewDocName));
        }
#endif

    }

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;

}

VOS_UINT32 AT_RcvXcallEccRemoteCtrlInd(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_EC_REMOTE_CTRL_IND_STRU                   *pstRcvMsg;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_EC_REMOTE_CTRL_IND_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallEcRemoteCtrlInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^ECCTRL: %d%s",
                                                    gaucAtCrLf,
                                                    pstRcvMsg->enRemoteCtrlType,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;

}


VOS_UINT32 AT_RcvXcallRemoteCtrlAnsCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_REMOTE_CTRL_ANSWER_CNF_STRU               *pstRcvMsg;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_REMOTE_CTRL_ANSWER_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallRemoteCtrlAnsCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallRemoteCtrlAnsCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECCTRL_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallRemoteCtrlAnsCnf : Current Option is not AT_CMD_ECCTRL_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;
    /* 格式化AT^ECCTRL  SET命令返回 */
    if (TAF_CALL_SEND_RESULT_SUCC == pstRcvMsg->enResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}

VOS_UINT32 AT_RcvXcallEccCapSetCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_ECC_SRV_CAP_CFG_CNF_STRU                  *pstRcvMsg;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_ECC_SRV_CAP_CFG_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallEccCapSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallEccCapSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECCAP_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallEccCapSetCnf : Current Option is not AT_CMD_ECCAP_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* 格式化AT^ECCAP  SET命令返回 */
    if (TAF_CALL_APP_ECC_SRV_CAP_CFG_RESULT_SUCC == pstRcvMsg->enResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}

VOS_UINT32 AT_RcvXcallEccCapQryCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_ECC_SRV_CAP_QRY_CNF_STRU                  *pstRcvMsg;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_ECC_SRV_CAP_QRY_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallEccCapQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallEccCapQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECCAP_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallEccCapQryCnf : Current Option is not AT_CMD_ECCAP_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* 格式化AT^ECCAP  SET命令返回 */
    if (TAF_CALL_APP_ECC_SRV_CAP_DISABLE == pstRcvMsg->enEccSrvCap)
    {
        ulResult               = AT_OK;
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s^ECCAP: %d,%d%s",
                                           gaucAtCrLf,
                                           pstRcvMsg->enEccSrvCap,
                                           TAF_CALL_APP_ECC_SRV_STATUS_CLOSE,
                                           gaucAtCrLf);
    }
    else if (TAF_CALL_APP_ECC_SRV_CAP_ENABLE == pstRcvMsg->enEccSrvCap)
    {
        ulResult               = AT_OK;
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s^ECCAP: %d,%d%s",
                                           gaucAtCrLf,
                                           pstRcvMsg->enEccSrvCap,
                                           pstRcvMsg->enEccSrvStatus,
                                           gaucAtCrLf);

    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}
#if (FEATURE_ON == FEATURE_CHINA_TELECOM_VOICE_ENCRYPT_TEST_MODE)

VOS_UINT32 AT_RcvXcallSetEccTestModeCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_SET_EC_TEST_MODE_CNF_STRU                 *pstRcvMsg;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_SET_EC_TEST_MODE_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallSetEccTestModeCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallSetEccTestModeCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECCTEST_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallSetEccTestModeCnf : Current Option is not AT_CMD_ECCTEST_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* 格式化AT^ECCTEST  SET命令返回 */
    if (VOS_TRUE == pstRcvMsg->ulResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}

VOS_UINT32 AT_RcvXcallQryEccTestModeCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_GET_EC_TEST_MODE_CNF_STRU                 *pstRcvMsg;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_GET_EC_TEST_MODE_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallQryEccTestModeCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallQryEccTestModeCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECCTEST_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallQryEccTestModeCnf : Current Option is not AT_CMD_ECCTEST_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* 格式化AT^ECCTEST  SET命令返回 */
    if (VOS_TRUE == pstRcvMsg->ulResult)
    {
        ulResult               = AT_OK;
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s^ECCTEST: %d%s",
                                           gaucAtCrLf,
                                           pstRcvMsg->enEccTestModeStatus,
                                           gaucAtCrLf);
    }
    else
    {
        ulResult               = AT_ERROR;
    }
    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}


VOS_UINT32 AT_RcvXcallQryEccRandomCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_GET_EC_RANDOM_CNF_STRU                    *pstRcvMsg;
    VOS_UINT32                                              ulResult;
    VOS_UINT32                                              ulIndex;
    VOS_UINT16                                              usLength;
    VOS_UINT8                                               ucRandomIndex;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_GET_EC_RANDOM_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallQryEccRandomCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallQryEccRandomCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECRANDOM_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallQryEccRandomCnf : Current Option is not AT_CMD_ECRANDOM_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* 格式化AT^ECRANDOM  QRY命令返回 */
    if (VOS_TRUE == pstRcvMsg->ulResult)
    {
        ulResult      = AT_OK;
        ucRandomIndex = 0;

        for (ulIndex  = 1;ulIndex < TAF_CALL_APP_EC_RANDOM_MAX_GROUP + 1;ulIndex++)
        {

            usLength  = 0;
            usLength += (TAF_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                                "%s^ECRANDOM: %d,%s",
                                                gaucAtCrLf,
                                                ulIndex,
                                                pstRcvMsg->stEccRandom[ucRandomIndex++].aucEccData);

            usLength += (TAF_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%s",
                                                pstRcvMsg->stEccRandom[ucRandomIndex++].aucEccData);

            usLength += (TAF_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%s",
                                                pstRcvMsg->stEccRandom[ucRandomIndex++].aucEccData);

            usLength += (TAF_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                "%s%s",
                                                pstRcvMsg->stEccRandom[ucRandomIndex++].aucEccData,
                                                gaucAtCrLf);
            gstAtSendData.usBufLen = usLength;
            At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);
        }


    }
    else
    {
        ulResult               = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}

VOS_UINT32 AT_RcvXcallQryEccKmcCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_GET_EC_KMC_CNF_STRU                       *pstRcvMsg;
    VOS_UINT32                                              ulResult;
    VOS_UINT8                                               aucKmcAsciiData[TAF_CALL_ECC_KMC_PUBLIC_KEY_MAX_ASCII_LENGTH + 1];

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_GET_EC_KMC_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallQryEccKmcCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallQryEccKmcCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECKMC_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallQryEccKmcCnf : Current Option is not AT_CMD_ECKMC_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;


    /* 格式化AT^ECKMC  QRY命令返回 */
    if (VOS_TRUE == pstRcvMsg->ulResult)
    {
        ulResult               = AT_OK;

        /* 把kmc data转换为ascii码 */
        if (AT_OK != AT_HexToAsciiString(pstRcvMsg->stKmcData.aucEccKmcData,
                                         aucKmcAsciiData,
                                         pstRcvMsg->stKmcData.usEccKmcLength))
        {
            return VOS_ERR;
        }

        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s^ECKMC: %d,\"%s\"%s",
                                           gaucAtCrLf,
                                           pstRcvMsg->stKmcData.ucEccKmcVer,
                                           aucKmcAsciiData,
                                           gaucAtCrLf);
        At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    }
    else
    {
        ulResult               = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}

VOS_UINT32 AT_RcvXcallSetEccKmcCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_SET_EC_KMC_CNF_STRU                       *pstRcvMsg;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_SET_EC_KMC_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallSetEccKmcCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallSetEccKmcCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECKMC_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallSetEccKmcCnf : Current Option is not AT_CMD_ECKMC_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* 格式化AT^ECKMC  SET命令返回 */
    if (VOS_TRUE == pstRcvMsg->ulResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}


VOS_UINT32 AT_RcvXcallEncryptedVoiceDataInd(
    VOS_VOID                                               *pstMsg
)
{
    VOS_VOID                                               *pFile;
    VOS_INT32                                               ret;
    TAF_CHAR                                               *DirPath = "/data/hisi_logs/modem_log/ECC_TEST";
    TAF_CALL_APP_ENCRYPTED_VOICE_DATA_IND_STRU             *pstRcvMsg;
    TAF_CHAR                                                acNewDocName[AT_TEST_ECC_FILE_NAME_MAX_LEN];
    TAF_CHAR                                                acTimeDocName[AT_TEST_ECC_FILE_NAME_MAX_LEN];
    VOS_INT32                                               lLength;

    pstRcvMsg = (TAF_CALL_APP_ENCRYPTED_VOICE_DATA_IND_STRU *)pstMsg;
    TAF_MEM_SET_S(acNewDocName, (VOS_SIZE_T)sizeof(acNewDocName), 0x00, (VOS_SIZE_T)sizeof(acNewDocName));
    lLength   = 0;

    /* 如果目录不存在则先创建目录 */
    if (VOS_OK != mdrv_file_access(DirPath, 0))
    {
        if (VOS_OK != mdrv_file_mkdir(DirPath))
        {
            AT_WARN_LOG("AT_RcvXcallEncryptedVoiceDataInd: mdrv_file_mkdir /data/hisi_logs/modem_log/ECC_TEST failed.\n");
            return VOS_ERR;
        }
    }

    if (VOS_TRUE == pstRcvMsg->ucIsTimeInfo)
    {
        TAF_MEM_SET_S(acTimeDocName, (VOS_SIZE_T)sizeof(acTimeDocName), 0x00, (VOS_SIZE_T)sizeof(acTimeDocName));

        lLength = VOS_sprintf_s(acTimeDocName, (VOS_SIZE_T)sizeof(acTimeDocName), "/data/hisi_logs/modem_log/ECC_TEST/Encrypted_call_execute_time%d.txt", gucAtCurrEncVoiceTestFileNum);
        if (lLength <= 0)
        {
            AT_WARN_LOG("AT_RcvXcallEncryptedVoiceDataInd(): VOS_sprintf_s len <= 0");

            return VOS_ERR;
        }

        pFile = mdrv_file_open(acTimeDocName, "a+");
    }
    else
    {
        if (0 != PS_MEM_CMP(acNewDocName, gacAtCurrDocName, sizeof(acNewDocName)))
        {
            pFile = mdrv_file_open(gacAtCurrDocName, "a+");
        }
        else
        {
            pFile = mdrv_file_open(g_pacCurrEncVoiceDataWriteFilePath[gucCurrEncVoiceDataWriteFileNum], "a+");
        }
    }

    if(0 == pFile)
    {
        AT_WARN_LOG("AT_RcvXcallEncryptedVoiceDataInd: mdrv_file_open failed.\n");

        return VOS_ERR;
    }

    ret = mdrv_file_write(pstRcvMsg->aucData, 1, pstRcvMsg->usLength, pFile);
    if(ret != pstRcvMsg->usLength)
    {
        AT_WARN_LOG("AT_RcvXcallEncryptedVoiceDataInd: mdrv_file_write write data failed.\n");

        mdrv_file_close(pFile);

        return VOS_ERR;
    }

    if (VOS_FALSE == pstRcvMsg->ucIsTimeInfo)
    {
        gulAtCurrEncVoiceDataCount++;
    }

    mdrv_file_close(pFile);


    return VOS_OK;
}

#endif
#endif

VOS_UINT32 AT_RcvMmaClocinfoInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_CLOCINFO_IND_STRU          *pstClocinfoInd;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstClocinfoInd      = (TAF_MMA_CLOCINFO_IND_STRU*)pMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstClocinfoInd->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaClocinfoInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^CLOCINFO: %d,%d,%d,%d,%d,%d,%d,%d,%d%s",
                                                    gaucAtCrLf,
                                                    pstClocinfoInd->stClocinfoPara.ucRatMode,
                                                    pstClocinfoInd->stClocinfoPara.usPrevInUse,
                                                    pstClocinfoInd->stClocinfoPara.ulMcc,
                                                    pstClocinfoInd->stClocinfoPara.ulMnc,
                                                    pstClocinfoInd->stClocinfoPara.lSid,
                                                    pstClocinfoInd->stClocinfoPara.lNid,
                                                    pstClocinfoInd->stClocinfoPara.ulBaseId,
                                                    pstClocinfoInd->stClocinfoPara.lBaseLatitude,
                                                    pstClocinfoInd->stClocinfoPara.lBaseLongitude,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvXcallPrivacyModeSetCnf(
    VOS_VOID                                               *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_PRIVACY_MODE_SET_CNF_STRU                 *pstRcvMsg = VOS_NULL_PTR;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_PRIVACY_MODE_SET_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallPrivacyModeSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallPrivacyModeSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CPMP_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallPrivacyModeSetCnf : Current Option is not AT_CMD_CPMP_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* 格式化AT^CPMP  SET命令返回 */
    if (TAF_CALL_APP_RESULT_TYPE_SUCCESS == pstRcvMsg->enResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvXcallPrivacyModeQryCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_PRIVACY_MODE_QRY_CNF_STRU                 *pstRcvMsg = VOS_NULL_PTR;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_PRIVACY_MODE_QRY_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallPrivacyModeQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvXcallPrivacyModeQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CPMP_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvXcallPrivacyModeQryCnf : Current Option is not AT_CMD_ECCAP_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* X模下，只有一路激活的呼叫 */
    if (0 != pstRcvMsg->ucCallNums)
    {
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s^CPMP: %d,%d,%d%s",
                                           gaucAtCrLf,
                                           pstRcvMsg->enPrivacyMode,
                                           pstRcvMsg->astCallVoicePrivacyInfo[0].ucCallId,
                                           pstRcvMsg->astCallVoicePrivacyInfo[0].enPrivacyMode,
                                           gaucAtCrLf);
    }
    else
    {
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s^CPMP: %d%s",
                                           gaucAtCrLf,
                                           pstRcvMsg->enPrivacyMode,
                                           gaucAtCrLf);
    }

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvXcallPrivacyModeInd(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_CALL_APP_PRIVACY_MODE_IND_STRU                     *pstRcvMsg = VOS_NULL_PTR;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_CALL_APP_PRIVACY_MODE_IND_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvXcallPrivacyModeInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^CPMP: %d,%d,%d%s",
                                                    gaucAtCrLf,
                                                    pstRcvMsg->enPrivacyMode,
                                                    pstRcvMsg->stCallVoicePrivacyInfo.ucCallId,
                                                    pstRcvMsg->stCallVoicePrivacyInfo.enPrivacyMode,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCtRoamRptCfgSetCnf(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                                               ucIndex;
    TAF_MMA_CTCC_ROAMING_NW_INFO_RTP_CFG_SET_CNF_STRU      *pstCtRoamRptSetCnf;
    VOS_UINT32                                              ulResult;

    /* 初始化消息变量 */
    ucIndex    = 0;
    pstCtRoamRptSetCnf = (TAF_MMA_CTCC_ROAMING_NW_INFO_RTP_CFG_SET_CNF_STRU*)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCtRoamRptSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCtRoamRptCfgSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCtRoamRptCfgSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CTROAMINFO_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCtRoamRptCfgSetCnf : Current Option is not AT_CMD_CTROAMINFO_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CTROAM设置命令返回 */
    gstAtSendData.usBufLen = 0;

    if (TAF_ERR_NO_ERROR == pstCtRoamRptSetCnf->enErrorCode)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaPrlIdQryCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_PRLID_QRY_CNF_STRU         *pstRcvMsg = VOS_NULL_PTR;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstRcvMsg         = (TAF_MMA_PRLID_QRY_CNF_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaPrlIdQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPrlIdQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_PRLID_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaPrlIdQryCnf : Current Option is not AT_CMD_PRLID_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s^PRLID: %d,%d,%d,%d,%d%s",
                                       gaucAtCrLf,
                                       pstRcvMsg->lPrlSrcType,
                                       pstRcvMsg->lPrlId,
                                       pstRcvMsg->lMlplMsplSrcType,
                                       pstRcvMsg->lMlplId,
                                       pstRcvMsg->lMsplId,
                                       gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCTimeInd(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_CTIME_IND_STRU             *pstCTimeInd;
    VOS_INT8                            cTimeZone;

    /* 初始化消息变量 */
    ucIndex           = 0;
    pstCTimeInd       = (TAF_MMA_CTIME_IND_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstCTimeInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaCTimeInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 底层上报的cTimeZone是以30分钟为单位的，AP要求以15分钟为单位 */
    if(pstCTimeInd->stTimezoneCTime.cTimeZone < 0)
    {
        cTimeZone = (VOS_INT8)(-1 * 2 * pstCTimeInd->stTimezoneCTime.cTimeZone);
    }
    else
    {
        cTimeZone = (VOS_INT8)(2 * pstCTimeInd->stTimezoneCTime.cTimeZone);
    }

    gstAtSendData.usBufLen = 0;
    /*时间显示格式: ^CTIME: "yy/mm/dd,hh:mm:ss(+/-)tz,dst" */
    if (pstCTimeInd->stTimezoneCTime.cTimeZone >= 0)
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s^CTIME: \"%02d/%02d/%02d,%02d:%02d:%02d+%d,%02d\"%s",
                                                        gaucAtCrLf,
                                                        pstCTimeInd->stTimezoneCTime.ucYear%100,
                                                        pstCTimeInd->stTimezoneCTime.ucMonth,
                                                        pstCTimeInd->stTimezoneCTime.ucDay,
                                                        pstCTimeInd->stTimezoneCTime.ucHour,
                                                        pstCTimeInd->stTimezoneCTime.ucMinute,
                                                        pstCTimeInd->stTimezoneCTime.ucSecond,
                                                        cTimeZone,
                                                        pstCTimeInd->stTimezoneCTime.ucDayltSavings,
                                                        gaucAtCrLf);
    }
    else
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s^CTIME: \"%02d/%02d/%02d,%02d:%02d:%02d-%d,%02d\"%s",
                                                        gaucAtCrLf,
                                                        pstCTimeInd->stTimezoneCTime.ucYear%100,
                                                        pstCTimeInd->stTimezoneCTime.ucMonth,
                                                        pstCTimeInd->stTimezoneCTime.ucDay,
                                                        pstCTimeInd->stTimezoneCTime.ucHour,
                                                        pstCTimeInd->stTimezoneCTime.ucMinute,
                                                        pstCTimeInd->stTimezoneCTime.ucSecond,
                                                        cTimeZone,
                                                        pstCTimeInd->stTimezoneCTime.ucDayltSavings,
                                                        gaucAtCrLf);
    }

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaCFreqLockSetCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_CFREQ_LOCK_SET_CNF_STRU    *pstCFreqLockSetCnf;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstCFreqLockSetCnf  = (TAF_MMA_CFREQ_LOCK_SET_CNF_STRU*)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCFreqLockSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCFreqLockSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCFreqLockSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CFREQLOCK_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCFreqLockSetCnf : Current Option is not AT_CMD_CFREQLOCK_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CFREQLOCK命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_TRUE != pstCFreqLockSetCnf->ulRslt)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCFreqLockQueryCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_CFREQ_LOCK_QUERY_CNF_STRU  *pstCFreqLockQryCnf;
    VOS_UINT16                          usLength;


    /* 初始化消息变量 */
    ucIndex            = 0;
    usLength           = 0;
    pstCFreqLockQryCnf = (TAF_MMA_CFREQ_LOCK_QUERY_CNF_STRU*)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCFreqLockQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCFreqLockQueryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCFreqLockQueryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CFREQLOCK_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCFreqLockQueryCnf : Current Option is not AT_CMD_CFREQLOCK_QUERY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^CFREQLOCK查询命令返回 */
    gstAtSendData.usBufLen = 0;
    if (TAF_MMA_CFREQ_LOCK_MODE_OFF == pstCFreqLockQryCnf->stCFreqLockPara.enFreqLockMode)
    {
       usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstCFreqLockQryCnf->stCFreqLockPara.enFreqLockMode);
    }
    else
    {
       usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d,%d,%d,%d,%d,%d,%d,%d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstCFreqLockQryCnf->stCFreqLockPara.enFreqLockMode,
                                                        pstCFreqLockQryCnf->stCFreqLockPara.usSid,
                                                        pstCFreqLockQryCnf->stCFreqLockPara.usNid,
                                                        pstCFreqLockQryCnf->stCFreqLockPara.usCdmaBandClass,
                                                        pstCFreqLockQryCnf->stCFreqLockPara.usCdmaFreq,
                                                        pstCFreqLockQryCnf->stCFreqLockPara.usCdmaPn,
                                                        pstCFreqLockQryCnf->stCFreqLockPara.usEvdoBandClass,
                                                        pstCFreqLockQryCnf->stCFreqLockPara.usEvdoFreq,
                                                        pstCFreqLockQryCnf->stCFreqLockPara.usEvdoPn);
    }

    gstAtSendData.usBufLen = usLength;
    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaCdmaCsqSetCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_CDMACSQ_SET_CNF_STRU            *pstCdmaCsqCnf;
    VOS_UINT8                                ucIndex;
    VOS_UINT32                               ulResult;

    pstCdmaCsqCnf = (TAF_MMA_CDMACSQ_SET_CNF_STRU *)pMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCdmaCsqCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCdmaCsqSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCdmaCsqSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstCdmaCsqCnf->enRslt)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCdmaCsqQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_CDMACSQ_QUERY_CNF_STRU     *pstCdmaCsqQueryCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstCdmaCsqQueryCnf  = (TAF_MMA_CDMACSQ_QUERY_CNF_STRU*)pMsg;
    usLength            = 0;
    ulResult            = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCdmaCsqQueryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCdmaCsqQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCdmaCsqQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CDMACSQ_QRY */
    if (AT_CMD_CDMACSQ_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s: %d,%d,%d,%d,%d,%d",
                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                     pstCdmaCsqQueryCnf->enMode,
                                     pstCdmaCsqQueryCnf->ucTimeInterval,
                                     pstCdmaCsqQueryCnf->ucRssiRptThreshold,
                                     pstCdmaCsqQueryCnf->ucEcIoRptThreshold,
                                     pstCdmaCsqQueryCnf->stSigQualityInfo.sCdmaRssi,
                                     pstCdmaCsqQueryCnf->stSigQualityInfo.sCdmaEcIo);


    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCdmaCsqInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_CDMACSQ_IND_STRU           *pstCdmaCsqInd;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstCdmaCsqInd       = (TAF_MMA_CDMACSQ_IND_STRU*)pMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstCdmaCsqInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaCdmaCsqInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^CDMACSQ: %d,%d%s",
                                                    gaucAtCrLf,
                                                    pstCdmaCsqInd->sCdmaRssi,
                                                    pstCdmaCsqInd->sCdmaEcIo,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;

}



VOS_UINT32 AT_RcvMmaCLModInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_CLMODE_IND_STRU            *pstCLModeInd;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                          *pucIsCLMode;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstCLModeInd        = (TAF_MMA_CLMODE_IND_STRU*)pMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstCLModeInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaCLModInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    enModemId = MODEM_ID_0;

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_RcvMmaCLModInd: Get modem id fail.");
        return VOS_ERR;
    }

    pucIsCLMode = AT_GetModemCLModeCtxAddrFromModemId(enModemId);

    *pucIsCLMode = pstCLModeInd->ucIsCLMode;
    return VOS_OK;

}


VOS_UINT32 AT_RcvMmaClDbDomainStatusSetCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_CL_DBDOMAIN_STATUS_SET_CNF_STRU        *pstClDbdomainStatusCnf = VOS_NULL_PTR;
    VOS_UINT8                                       ucIndex;
    VOS_UINT32                                      ulResult;

    pstClDbdomainStatusCnf = (TAF_MMA_CL_DBDOMAIN_STATUS_SET_CNF_STRU *)pMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstClDbdomainStatusCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaClDbDomainStatusSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaClDbDomainStatusSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CLDBDOMAINSTATUS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaClDbDomainStatusSetCnf : Current Option is not AT_CMD_CFPLMN_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstClDbdomainStatusCnf->enResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaClDbdomainStatusReportInd(
    VOS_VOID                                               *pstMsg
)
{
    TAF_MMA_CL_DBDOMAIN_STATUS_INFO_IND_STRU               *pstClDbdomainStaRInd = VOS_NULL_PTR;
    VOS_UINT8                                               ucIndex;

    /* 初始化消息变量 */
    ucIndex                 = 0;
    pstClDbdomainStaRInd    = (TAF_MMA_CL_DBDOMAIN_STATUS_INFO_IND_STRU *)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstClDbdomainStaRInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaRoamingModeSwitchInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^CLDBDOMAINSTATUS: %d,%d,%d%s",
                                                    gaucAtCrLf,
                                                    pstClDbdomainStaRInd->ucEnableReportFlag,
                                                    pstClDbdomainStaRInd->uc1xSrvStatus,
                                                    pstClDbdomainStaRInd->ucImsVoiceCap,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}
#endif


VOS_UINT32 AT_RcvMtaXpassInfoInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstMtaMsg       = VOS_NULL_PTR;
    MTA_AT_XPASS_INFO_IND_STRU         *pstXpassInfoInd = VOS_NULL_PTR;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstMtaMsg           = (AT_MTA_MSG_STRU*)pMsg;
    pstXpassInfoInd     = (MTA_AT_XPASS_INFO_IND_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaXpassInfoInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^XPASSINFO: %d,%d%s",
                                                    gaucAtCrLf,
                                                    pstXpassInfoInd->enGphyXpassMode,
                                                    pstXpassInfoInd->enWphyXpassMode,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCFPlmnSetCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_CFPLMN_SET_CNF_STRU        *pstCFPLmnCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstCFPLmnCnf = (TAF_MMA_CFPLMN_SET_CNF_STRU *)pMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCFPLmnCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCFPlmnSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCFPlmnSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CFPLMN_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCFPlmnSetCnf : Current Option is not AT_CMD_CFPLMN_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstCFPLmnCnf->enRslt)
    {
        ulResult = AT_OK;
    }
    else
    {
        /* 这次代码的修改涉及接口的重构，为了保持和原有GU代码的一致性，返回结果出错时，结果设置为AT_CME_UNKNOWN  */
        ulResult = AT_CME_UNKNOWN;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}

VOS_UINT32 AT_RcvMmaCFPlmnQueryCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_CFPLMN_QUERY_CNF_STRU      *pstCFPlmnQrynf = VOS_NULL_PTR;
    TAF_USER_PLMN_LIST_STRU            *pstCFPlmnList  = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulLoop;

    /* 初始化消息变量 */
    ucIndex = 0;
    pstCFPlmnQrynf = (TAF_MMA_CFPLMN_QUERY_CNF_STRU *)pstMsg;
    /*lint -save -e516 */
    pstCFPlmnList = (TAF_USER_PLMN_LIST_STRU *)PS_MEM_ALLOC(WUEPS_PID_AT, sizeof(TAF_USER_PLMN_LIST_STRU));
    /*lint -restore */
    if (VOS_NULL_PTR == pstCFPlmnList)
    {
        AT_WARN_LOG("AT_RcvMmaCFPlmnQueryCnf : alloc fail");
        return VOS_ERR;
    }

    TAF_MEM_SET_S(pstCFPlmnList, sizeof(TAF_USER_PLMN_LIST_STRU), 0x00, sizeof(TAF_USER_PLMN_LIST_STRU));

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCFPlmnQrynf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCFPlmnQueryCnf : WARNING:AT INDEX NOT FOUND!");
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, pstCFPlmnList);
        /*lint -restore */
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCFPlmnQueryCnf : AT_BROADCAST_INDEX.");
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, pstCFPlmnList);
        /*lint -restore */
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CFPLMN_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCFPlmnQueryCnf : Current Option is not AT_CMD_CFREQLOCK_QUERY.");
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, pstCFPlmnList);
        /*lint -restore */
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS != pstCFPlmnQrynf->enRslt)
    {
        gstAtSendData.usBufLen = 0;
        /* 这次代码的修改涉及接口的重构，为了保持和原有GU代码的一致性，返回结果出错时，结果设置为AT_CME_UNKNOWN  */
        ulResult = AT_CME_UNKNOWN;
        At_FormatResultData(ucIndex, ulResult);
    }
    else
    {
        if (pstCFPlmnQrynf->usPlmnNum > TAF_USER_MAX_PLMN_NUM)
        {
            pstCFPlmnQrynf->usPlmnNum = TAF_USER_MAX_PLMN_NUM;
        }

        pstCFPlmnList->usPlmnNum = pstCFPlmnQrynf->usPlmnNum;

        for (ulLoop = 0; (ulLoop < pstCFPlmnQrynf->usPlmnNum); ulLoop++ )
        {
            pstCFPlmnList->Plmn[ulLoop].Mcc = pstCFPlmnQrynf->astPlmn[ulLoop].Mcc;
            pstCFPlmnList->Plmn[ulLoop].Mnc = pstCFPlmnQrynf->astPlmn[ulLoop].Mnc;
        }

        At_QryParaRspCfplmnProc(ucIndex, pstCFPlmnQrynf->stCtrl.ucOpId, (TAF_VOID *)pstCFPlmnList);
    }
    /*lint -save -e516 */
    PS_MEM_FREE(WUEPS_PID_AT, pstCFPlmnList);
    /*lint -restore */
    return VOS_OK;
}


VOS_VOID AT_ReportQryPrefPlmnCmdPara(
    TAF_MMA_PREF_PLMN_QUERY_CNF_STRU   *pstCpolQryCnf,
    AT_MODEM_NET_CTX_STRU              *pstNetCtx,
    VOS_UINT16                         *pusLength,
    VOS_UINT32                          ucIndex,
    VOS_UINT32                          ulLoop
)
{
    TAF_PLMN_NAME_LIST_STRU            *pstAvailPlmnInfo;
    AT_COPS_FORMAT_TYPE                 CurrFormat;

    pstAvailPlmnInfo = (TAF_PLMN_NAME_LIST_STRU*)&pstCpolQryCnf->stPlmnName;

    /* +CPOL:   */
    *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    /* <index> */
    *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),
                                       "%d",
                                       ((ulLoop + pstCpolQryCnf->ulFromIndex) + 1));

    CurrFormat = pstNetCtx->ucCpolFormatType;

    if (('\0' == pstAvailPlmnInfo->astPlmnName[ulLoop].aucOperatorNameLong[0])
     && (AT_COPS_LONG_ALPH_TYPE == CurrFormat))
    {
        CurrFormat = AT_COPS_NUMERIC_TYPE;
    }

    if (('\0' == pstAvailPlmnInfo->astPlmnName[ulLoop].aucOperatorNameShort[0])
     && (AT_COPS_SHORT_ALPH_TYPE == CurrFormat))
    {
        CurrFormat = AT_COPS_NUMERIC_TYPE;
    }

    /* <format> */
    *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),
                                       ",%d",
                                       CurrFormat);

    /* <oper1> */
    if (AT_COPS_LONG_ALPH_TYPE == CurrFormat)
    {
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),
                                           ",\"%s\"",
                                           pstAvailPlmnInfo->astPlmnName[ulLoop].aucOperatorNameLong);
    }
    else if (AT_COPS_SHORT_ALPH_TYPE == CurrFormat)
    {
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),
                                           ",\"%s\"",
                                           pstAvailPlmnInfo->astPlmnName[ulLoop].aucOperatorNameShort);
    }
    else
    {
        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),
                                           ",\"%X%X%X",
                                           (0x0f00 & pstAvailPlmnInfo->astPlmnName[ulLoop].PlmnId.Mcc) >> 8,
                                           (0x00f0 & pstAvailPlmnInfo->astPlmnName[ulLoop].PlmnId.Mcc) >> 4,
                                           (0x000f & pstAvailPlmnInfo->astPlmnName[ulLoop].PlmnId.Mcc));

        if (0x0F != ((0x0f00 & pstAvailPlmnInfo->astPlmnName[ulLoop].PlmnId.Mnc) >> 8))
        {
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),
                                               "%X",
                                               (0x0f00 & pstAvailPlmnInfo->astPlmnName[ulLoop].PlmnId.Mnc) >> 8);

        }

        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),
                                           "%X%X\"",
                                           (0x00f0 & pstAvailPlmnInfo->astPlmnName[ulLoop].PlmnId.Mnc) >> 4,
                                           (0x000f & pstAvailPlmnInfo->astPlmnName[ulLoop].PlmnId.Mnc));
    }

    return;
}


VOS_VOID AT_ReportQryPrefPlmnCmd(
    TAF_MMA_PREF_PLMN_QUERY_CNF_STRU   *pstCpolQryCnf,
    VOS_UINT32                         *pulValidPlmnNum,
    AT_MODEM_NET_CTX_STRU              *pstNetCtx,
    VOS_UINT16                         *pusLength,
    VOS_UINT32                          ucIndex
)
{
    VOS_UINT32                          i;
    TAF_PLMN_NAME_LIST_STRU            *pstAvailPlmnInfo;
    TAF_MMC_USIM_RAT                    usPlmnRat;

    pstAvailPlmnInfo = (TAF_PLMN_NAME_LIST_STRU*)&pstCpolQryCnf->stPlmnName;

    for(i = 0; i < pstAvailPlmnInfo->ulPlmnNum; i++)
    {
        if (VOS_FALSE == AT_PH_IsPlmnValid(&(pstAvailPlmnInfo->astPlmnName[i].PlmnId)))
        {
            continue;
        }

        (*pulValidPlmnNum)++;

        if ((0 == pstCpolQryCnf->ulValidPlmnNum)
         && (1 == *pulValidPlmnNum))
        {
            /* 参考V.250协议5.7.2: 根据V命令的设置给信息字段的头尾增加回车换行符 */
            if (AT_V_ENTIRE_TYPE == gucAtVType)
            {
                *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),
                                                   "%s",
                                                   gaucAtCrLf);
            }
        }
        else
        {
            *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),
                                               "%s",
                                               gaucAtCrLf);
        }

        AT_ReportQryPrefPlmnCmdPara(pstCpolQryCnf, pstNetCtx, pusLength, ucIndex, i);

        if ((MN_PH_PREF_PLMN_UPLMN == pstNetCtx->enPrefPlmnType)
         && (VOS_TRUE == pstAvailPlmnInfo->bPlmnSelFlg))
        {
            continue;
        }


        /* <GSM_AcT1> */
        /* <GSM_Compact_AcT1> */
        /* <UTRAN_AcT1> */
        usPlmnRat = pstAvailPlmnInfo->ausPlmnRat[i];

        *pusLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + (*pusLength),",%d,%d,%d,%d",
                                           (usPlmnRat & TAF_MMC_USIM_GSM_RAT) >> 7,
                                           (usPlmnRat & TAF_MMC_USIM_GSM_COMPACT_RAT) >> 6,
                                           (usPlmnRat & TAF_MMC_USIM_UTRN_RAT) >> 15,
                                           (usPlmnRat & TAF_MMC_USIM_E_UTRN_RAT) >> 14);

    }

    return;

}

VOS_UINT32 AT_RcvMmaPrefPlmnSetCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_PREF_PLMN_SET_CNF_STRU     *pstPrefPlmnCnf;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex            = 0;
    ulResult           = AT_ERROR;
    pstPrefPlmnCnf     = (TAF_MMA_PREF_PLMN_SET_CNF_STRU*)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPrefPlmnCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPrefPlmnSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPrefPlmnSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CPOL_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaPrefPlmnSetCnf : Current Option is not AT_CMD_CPOL_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_APP_PREF_PLMN_OPER_RESULT_SUCCESS != pstPrefPlmnCnf->enRslt)
    {
        ulResult = AT_CME_UNKNOWN;
    }
    else
    {
        ulResult = AT_OK;
    }

    /* 格式化AT+CPOL测试命令返回 */
    gstAtSendData.usBufLen = 0;

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaPrefPlmnQueryCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT16                          usLength = 0;
    TAF_PLMN_NAME_LIST_STRU            *pstAvailPlmnInfo;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulValidPlmnNum;
    TAF_MMA_CPOL_INFO_QUERY_REQ_STRU    stCpolInfo;
    VOS_UINT8                           ucIndex;
    TAF_MMA_PREF_PLMN_QUERY_CNF_STRU   *pstCpolQryCnf = VOS_NULL_PTR;

    pstCpolQryCnf    = (TAF_MMA_PREF_PLMN_QUERY_CNF_STRU *)pstMsg;

    pstAvailPlmnInfo = (TAF_PLMN_NAME_LIST_STRU*)&pstCpolQryCnf->stPlmnName;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCpolQryCnf->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_QryParaRspPrefPlmnProc:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_QryParaRspPrefPlmnProc : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }
    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    /*判断当前操作类型是否为AT_CMD_CPOL_READ */
    if (AT_CMD_CPOL_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    if (TAF_MMA_APP_PREF_PLMN_OPER_RESULT_SUCCESS != pstCpolQryCnf->enRslt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_CME_UNKNOWN);

        return VOS_OK;
    }

    ulValidPlmnNum = 0;
    AT_ReportQryPrefPlmnCmd(pstCpolQryCnf, &ulValidPlmnNum, pstNetCtx, &usLength, ucIndex);

    /*
    AT模块根据MMA上报的运营商个数来确认运营商信息是否收集完成: 小于请求的运营商
    个数则认为运营商信息已经收集完成，原因如下所述:
    AT模块每次请求37条运营商信息，起始位置为已经输出的最后一个运营商的下一条运营商信息索引
    如果待输出的运营商信息总数不足37条，则按实际的输出，
    如果运营商数为37的倍数，则AT会再发一次运营商信息请求，MMA回复的运营商信息总数为0
    */
    if (pstAvailPlmnInfo->ulPlmnNum < TAF_MMA_MAX_PLMN_NAME_LIST_NUM)
    {
        /* 参考V.250协议5.7.2: 根据V命令的设置给信息字段的头尾增加回车换行符 */
        if ((AT_V_ENTIRE_TYPE == gucAtVType)
         && (0 != (pstCpolQryCnf->ulValidPlmnNum + ulValidPlmnNum)))
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s",
                                               gaucAtCrLf);

        }

        At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

        stCpolInfo.enPrefPLMNType   = pstNetCtx->enPrefPlmnType;
        stCpolInfo.ulFromIndex      = (pstCpolQryCnf->ulFromIndex + pstAvailPlmnInfo->ulPlmnNum);
        stCpolInfo.ulPlmnNum        = TAF_MMA_MAX_PLMN_NAME_LIST_NUM;
        stCpolInfo.ulValidPlmnNum   = (pstCpolQryCnf->ulValidPlmnNum + ulValidPlmnNum);
        ulResult                    = TAF_MMA_QueryCpolReq(WUEPS_PID_AT,
                                      gastAtClientTab[ucIndex].usClientId,
                                      0,
                                      &stCpolInfo);
        if (VOS_TRUE != ulResult)
        {
            /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
            AT_STOP_TIMER_CMD_READY(ucIndex);
            gstAtSendData.usBufLen = 0;
            At_FormatResultData(ucIndex, AT_CME_UNKNOWN);
        }
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaPrefPlmnTestCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_PREF_PLMN_TEST_CNF_STRU    *pstPrefPlmnCnf;
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex            = 0;
    usLength           = 0;
    ulResult           = AT_ERROR;
    pstPrefPlmnCnf     = (TAF_MMA_PREF_PLMN_TEST_CNF_STRU*)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPrefPlmnCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPrefPlmnTestCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPrefPlmnTestCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CPOL_TEST != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaPrefPlmnTestCnf : Current Option is not AT_CMD_CPOL_TEST.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_APP_PREF_PLMN_OPER_RESULT_SUCCESS != pstPrefPlmnCnf->enRslt)
    {
        gstAtSendData.usBufLen = usLength;
        At_FormatResultData(ucIndex, AT_CME_UNKNOWN);

        return VOS_OK;
    }

    /* 格式化AT+CPOL测试命令返回 */
    gstAtSendData.usBufLen = 0;
    if ( 0 == pstPrefPlmnCnf->usPlmnNum )
    {
        ulResult = AT_CME_OPERATION_NOT_ALLOWED;
    }
    else
    {
        usLength = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                         (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                         "%s: (1-%d),(0-2)",
                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                         pstPrefPlmnCnf->usPlmnNum);


        ulResult = AT_OK;
    }

    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaAcInfoChangeInd(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT16                          usLength;
    TAF_MMA_CELL_AC_INFO_STRU          *pstCellAcInfo = VOS_NULL_PTR;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;
    TAF_MMA_AC_INFO_CHANGE_IND_STRU    *psAcInfoChangeInd = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    psAcInfoChangeInd = (TAF_MMA_AC_INFO_CHANGE_IND_STRU*)pstMsg;

    if (AT_FAILURE == At_ClientIdToUserId(psAcInfoChangeInd->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAcInfoChangeInd : AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    enModemId = MODEM_ID_0;

    usLength = 0;
    pstCellAcInfo = (TAF_MMA_CELL_AC_INFO_STRU*)(&psAcInfoChangeInd->stCellAcInfo);

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_RcvMmaAcInfoChangeInd: Get modem id fail.");
        return VOS_ERR;
    }

    /* 通过AT^CURC控制紧急呼叫号码不上报 */

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s%d,%d,%d,%d%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_ACINFO].pucText,
                                       pstCellAcInfo->enSrvDomain,/* 上报服务域 */
                                       pstCellAcInfo->enCellAcType,/* 上报小区禁止接入类型 */
                                       pstCellAcInfo->ucRestrictRegister,/* 上报是否注册受限 */
                                       pstCellAcInfo->ucRestrictPagingRsp,/* 上报是否寻呼受限 */
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaEOPlmnSetCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulRslt;
    TAF_MMA_EOPLMN_SET_CNF_STRU        *pstEOPlmnSetCnf = VOS_NULL_PTR;

    pstEOPlmnSetCnf  = (TAF_MMA_EOPLMN_SET_CNF_STRU *)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEOPlmnSetCnf->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaEOPlmnSetCnf At_ClientIdToUserId FAILURE");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaEOPlmnSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_EOPLMN_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_RcvMmaEOPlmnSetCnf: WARNING:Not AT_CMD_EOPLMN_SET!");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if( TAF_ERR_NO_ERROR == pstEOPlmnSetCnf->ulResult)
    {
        ulRslt = AT_OK;
    }
    else
    {
        ulRslt = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;


    At_FormatResultData(ucIndex,ulRslt);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaEOPlmnQryCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT16                                              usLen;
    VOS_UINT8                                               ucIndex;
    VOS_UINT32                                              ulRslt;
    TAF_MMA_EOPLMN_QRY_CNF_STRU                            *pstEOPlmnQryCnf = VOS_NULL_PTR;

    usLen            = 0;
    pstEOPlmnQryCnf  = (TAF_MMA_EOPLMN_QRY_CNF_STRU *)pstMsg;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstEOPlmnQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaEOPlmnQryCnf At_ClientIdToUserId FAILURE");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_EOPLMN_QRY */
    if (AT_CMD_EOPLMN_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_RcvMmaEOPlmnQryCnf: WARNING:Not AT_CMD_EOPLMN_QRY!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 输出查询结果 */
    if (TAF_ERR_NO_ERROR == pstEOPlmnQryCnf->ulResult)
    {
        usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                        "%s: \"%s\",%d,",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                        pstEOPlmnQryCnf->aucVersion,
                                        pstEOPlmnQryCnf->usOPlmnNum * TAF_AT_PLMN_WITH_RAT_LEN);


        /* 将16进制数转换为ASCII码后输入主动命令内容 */
        usLen += (TAF_UINT16)At_HexText2AsciiStringSimple(AT_CMD_MAX_LEN,
                                                            (TAF_INT8 *)pgucAtSndCodeAddr,
                                                            (TAF_UINT8 *)pgucAtSndCodeAddr + usLen,
                                                            pstEOPlmnQryCnf->usOPlmnNum * TAF_SIM_PLMN_WITH_RAT_LEN,
                                                            pstEOPlmnQryCnf->aucOPlmnList);

        gstAtSendData.usBufLen = usLen;

        ulRslt = AT_OK;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
        ulRslt = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaNetScanCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_NET_SCAN_CNF_STRU          *pstNetScanCnf       = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    pstNetScanCnf  = (TAF_MMA_NET_SCAN_CNF_STRU *)pstMsg;

    if (AT_FAILURE == At_ClientIdToUserId(pstNetScanCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaNetScanCnf At_ClientIdToUserId FAILURE");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaNetScanCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if ( TAF_MMA_NET_SCAN_RESULT_SUCCESS == pstNetScanCnf->enResult )
    {
        AT_PhNetScanReportSuccess(ucIndex, pstNetScanCnf);
    }
    else
    {
        AT_PhNetScanReportFailure(ucIndex, pstNetScanCnf->enCause);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaUserSrvStateQryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_USER_SRV_STATE_QRY_CNF_STRU                    *pstUserSrvStateCnf = VOS_NULL_PTR;
    VOS_UINT16                                              usLength;
    VOS_UINT8                                               ucIndex;

    pstUserSrvStateCnf = (TAF_MMA_USER_SRV_STATE_QRY_CNF_STRU *)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstUserSrvStateCnf->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaUserSrvStateQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaUserSrvStateQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_USER_SRV_STATE_READ */
    if (AT_CMD_USER_SRV_STATE_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    usLength  = 0;

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "^USERSRVSTATE: ");

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d,%d",
                                       pstUserSrvStateCnf->ulCsSrvExistFlg,
                                       pstUserSrvStateCnf->ulPsSrvExistFlg);

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}



VOS_UINT32  AT_RcvMmaPwrOnAndRegTimeQryCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT32                                              ulCostTime;
    VOS_UINT16                                              usLength;
    VOS_UINT32                                              ulResult;
    TAF_MMA_POWER_ON_AND_REG_TIME_QRY_CNF_STRU             *pstAppwronregCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    pstAppwronregCnf = (TAF_MMA_POWER_ON_AND_REG_TIME_QRY_CNF_STRU *)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstAppwronregCnf->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPwrOnAndRegTimeQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPwrOnAndRegTimeQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_REGISTER_TIME_READ */
    if (AT_CMD_REGISTER_TIME_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    ulCostTime = pstAppwronregCnf->ulCostTime;

    /* 初始化 */
    ulResult   = AT_OK;
    usLength   = 0;

    /* MMA报给AT时，按slice上报，(32 * 1024)个slice是1S
       如果slice为0，表示没有注册成功；如果slice小于1S,AT按1S上报 */

    /* 将查询时间上报给APP*/
    if ( 0 ==  ulCostTime)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulCostTime = ulCostTime/(32 * 1024);

        if (0 == ulCostTime)
        {
            ulCostTime = 1;
        }

        ulResult   = AT_OK;

        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%s",
                                          gaucAtCrLf);

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s:%d",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           ulCostTime);
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}


VOS_UINT32  AT_RcvMmaSpnQryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_SPN_QRY_CNF_STRU           *pstSpnCnf;
    VOS_UINT32                          ulResult;
    TAF_PH_USIM_SPN_STRU                stAtSPNRslt;
    VOS_UINT16                          usLength;
    VOS_UINT16                          usDatalen;
    VOS_UINT8                          *pucSystemAppConfig;
    VOS_UINT8                           ucIndex;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pstSpnCnf = (TAF_MMA_SPN_QRY_CNF_STRU *)pstMsg ;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstSpnCnf->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSpnQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSpnQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_SPN_QUERY */
    if (AT_CMD_SPN_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    /* 初始化 */
    ulResult  = AT_OK;
    usLength  = 0;
    usDatalen = 0;

    TAF_MEM_SET_S(&stAtSPNRslt, (VOS_SIZE_T)sizeof(stAtSPNRslt), 0x00, (VOS_SIZE_T)sizeof(TAF_PH_USIM_SPN_STRU));
    /* 0 表示SIM */
    if (1 == pstNetCtx->ucSpnType)
    {
        TAF_MEM_CPY_S(&stAtSPNRslt, sizeof(stAtSPNRslt), &(pstSpnCnf->stMnMmaSpnInfo.stUsimSpnInfo), sizeof(TAF_PH_USIM_SPN_STRU));
    }
    else
    {
        TAF_MEM_CPY_S(&stAtSPNRslt, sizeof(stAtSPNRslt), &(pstSpnCnf->stMnMmaSpnInfo.stSimSpnInfo), sizeof(TAF_PH_USIM_SPN_STRU));
    }

    if (0 == stAtSPNRslt.ucLength)
    {
        /*  SPN file not exist */
        ulResult = AT_CME_SPN_FILE_NOT_EXISTS;
    }
    else
    {
        /* display format: ^SPN:disp_rplmn,coding,spn_name */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr+usLength,
                                            "%s:",
                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName);


        if ( SYSTEM_APP_WEBUI == *pucSystemAppConfig)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr+usLength,
                                               "%d,%d,",
                                               stAtSPNRslt.ucDispRplmnMode & 0x03,
                                               stAtSPNRslt.ucCoding);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr+usLength,
                                               "%d,%d,",
                                               stAtSPNRslt.ucDispRplmnMode & 0x01,
                                               stAtSPNRslt.ucCoding);
        }

        if ( stAtSPNRslt.ucCoding == TAF_PH_GSM_7BIT_DEFAULT )
        {
            usDatalen = stAtSPNRslt.ucLength;
            TAF_MEM_CPY_S(pgucAtSndCodeAddr + usLength,
                AT_CMD_MAX_LEN + 20 - 3 - usLength,
                stAtSPNRslt.aucSpnName,
                usDatalen);
            usLength = usLength + usDatalen;
        }
        else
        {
            usDatalen = stAtSPNRslt.ucLength;

            usLength += (VOS_UINT16)At_HexAlpha2AsciiString(AT_CMD_MAX_LEN,
                                                            (VOS_INT8 *)pgucAtSndCodeAddr,
                                                            (VOS_UINT8 *)pgucAtSndCodeAddr + usLength,
                                                            stAtSPNRslt.aucSpnName,
                                                            (VOS_UINT16)usDatalen);
        }
    }

    gstAtSendData.usBufLen = usLength;

    /* 回复用户命令结果 */
    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}



VOS_UINT32  AT_RcvMmaMMPlmnInfoQryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_MMPLMNINFO_QRY_CNF_STRU    *pstMMPlmnInfoCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_UINT8                           i;
    VOS_UINT8                           ucIndex;

    /* 变量初始化 */
    pstMMPlmnInfoCnf = (TAF_MMA_MMPLMNINFO_QRY_CNF_STRU *)pstMsg;
    ulResult    = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMMPlmnInfoCnf->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaMMPlmnInfoQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaMMPlmnInfoQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_MMPLMNINFO_QRY */
    if (AT_CMD_MMPLMNINFO_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 转换LongName及ShortName */
    if ( pstMMPlmnInfoCnf->stMmPlmnInfo.ucLongNameLen <= TAF_PH_OPER_NAME_LONG
      && pstMMPlmnInfoCnf->stMmPlmnInfo.ucShortNameLen <= TAF_PH_OPER_NAME_SHORT )
    {

        /* ^MMPLMNINFO:<long name>,<short name> */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          "%s:",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        for (i = 0; i < pstMMPlmnInfoCnf->stMmPlmnInfo.ucLongNameLen; i++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%02X",
                                               pstMMPlmnInfoCnf->stMmPlmnInfo.aucLongName[i]);
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr + usLength, ",");

        for (i = 0; i < pstMMPlmnInfoCnf->stMmPlmnInfo.ucShortNameLen; i++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%02X",
                                               pstMMPlmnInfoCnf->stMmPlmnInfo.aucShortName[i]);
        }

        ulResult = AT_OK;
        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
    }

    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}


VOS_UINT32  AT_RcvMmaPlmnQryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_LAST_CAMP_PLMN_QRY_CNF_STRU                    *pstPlmnCnf = VOS_NULL_PTR;
    NAS_MM_INFO_IND_STRU                                    stMmTimeInfo;
    VOS_UINT16                                              usLength;
    VOS_UINT8                                               ucIndex;

    pstPlmnCnf = (TAF_MMA_LAST_CAMP_PLMN_QRY_CNF_STRU *)pstMsg;
    usLength  = 0;
    TAF_MEM_SET_S(&stMmTimeInfo, sizeof(NAS_MM_INFO_IND_STRU), 0x00, sizeof(NAS_MM_INFO_IND_STRU));

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPlmnCnf->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPlmnQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPlmnQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_PLMN_QRY 或 AT_CMD_TIMEQRY_QRY */
    if ((AT_CMD_PLMN_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_TIMEQRY_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        return VOS_ERR;
    }

    /* 无效的PLMN */
    if ((0 == pstPlmnCnf->stPlmnId.Mcc)
     && (0 == pstPlmnCnf->stPlmnId.Mnc))
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, AT_OK);
        return VOS_ERR;
    }

    if (AT_CMD_PLMN_QRY == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s^PLMN: ",
                                           gaucAtCrLf);

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%x%x%x,",
                                           (pstPlmnCnf->stPlmnId.Mcc & 0x0f00)>>8,
                                           (pstPlmnCnf->stPlmnId.Mcc & 0xf0)>>4,
                                           (pstPlmnCnf->stPlmnId.Mcc & 0x0f));

        if (0x0f00 == (pstPlmnCnf->stPlmnId.Mnc & 0x0f00))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%x%x",
                                               (pstPlmnCnf->stPlmnId.Mnc & 0xf0)>>4,
                                               (pstPlmnCnf->stPlmnId.Mnc & 0x0f));
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%x%x%x",
                                               (pstPlmnCnf->stPlmnId.Mnc & 0x0f00)>>8,
                                               (pstPlmnCnf->stPlmnId.Mnc & 0xf0)>>4,
                                               (pstPlmnCnf->stPlmnId.Mnc & 0x0f));
        }

        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s",
                                           gaucAtCrLf);

    }
    else
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        if(VOS_OK == AT_PlmnTimeZoneProc(pstPlmnCnf->stPlmnId.Mcc,&stMmTimeInfo))
        {
            usLength = AT_PrintNwTimeInfo(&stMmTimeInfo, (VOS_CHAR *)g_stParseContext[ucIndex].pstCmdElement->pszCmdName, 0);
        }
        else
        {
            At_FormatResultData(ucIndex, AT_ERROR);
            return VOS_ERR;
        }
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, AT_OK);
    return VOS_OK;

}



VOS_UINT32 AT_GetTimeZoneByMcc(VOS_UINT16 usPlmnMcc, VOS_INT8 *pcMccZoneInfo)
{
    VOS_UINT32                          ulTotalMccNum = 0;
    VOS_UINT32                          i             = 0;
    VOS_UINT32                          ulRet         = VOS_FALSE;

    ulTotalMccNum = AT_ARRAY_SIZE(g_astMccZoneInfoTbl);

    for (i = 0; i < ulTotalMccNum; i++)
    {
        if (usPlmnMcc == g_astMccZoneInfoTbl[i].usMcc)
        {
            *pcMccZoneInfo = g_astMccZoneInfoTbl[i].cZone;

            ulRet = VOS_TRUE;

            break;
        }
    }

    return ulRet;
}


VOS_UINT32 AT_PlmnTimeZoneProc(
    TAF_UINT32                          ulMcc,
    NAS_MM_INFO_IND_STRU               *pstMmTimeInfo
)
{
    VOS_UINT32           ulResult     = VOS_ERR;
    VOS_UINT16           usMcc        = 0;
    VOS_INT8             cTimeZone    = 0;


    /*Get MCC*/
    usMcc = (VOS_UINT16)ulMcc;

    /* ^TIMEQRY在没收到EMM INFO的时候根据MCC计算时区 */
    if (VOS_TRUE == AT_GetTimeZoneByMcc(usMcc, &cTimeZone))
    {
        /*能走到这个分支说明之前A核没有拿到过MM INFO时间，因此填充一个默认值*/
        pstMmTimeInfo->ucIeFlg = NAS_MM_INFO_IE_UTLTZ;
        pstMmTimeInfo->stUniversalTimeandLocalTimeZone.ucYear    = AT_CMD_TIMEQRY_YEAR_DEFAULT_VALUE;
        pstMmTimeInfo->stUniversalTimeandLocalTimeZone.ucMonth   = AT_CMD_TIMEQRY_MONTH_DEFAULT_VALUE;
        pstMmTimeInfo->stUniversalTimeandLocalTimeZone.ucDay     = AT_CMD_TIMEQRY_DAY_DEFAULT_VALUE;
        pstMmTimeInfo->stUniversalTimeandLocalTimeZone.ucHour    = AT_CMD_TIMEQRY_HOUR_DEFAULT_VALUE;
        pstMmTimeInfo->stUniversalTimeandLocalTimeZone.cTimeZone = cTimeZone;
        pstMmTimeInfo->cLocalTimeZone                            = cTimeZone;

        ulResult = VOS_OK;
    }
    else
    {
        ulResult = VOS_ERR;
    }

    return ulResult;
}




VOS_UINT32 AT_RcvMmaCerssiSetCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_CERSSI_SET_CNF_STRU        *pstCerssiSetCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstCerssiSetCnf = (TAF_MMA_CERSSI_SET_CNF_STRU *)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCerssiSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCerssiSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCerssiSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CERSSI_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCerssiSetCnf : Current Option is not AT_CMD_CERSSI_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_ERR_NO_ERROR == pstCerssiSetCnf->enErrorCause)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstCerssiSetCnf->enErrorCause);
    }

    gstAtSendData.usBufLen = 0;

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaCindSetCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_CIND_SET_CNF_STRU          *pstCindSetCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstCindSetCnf = (TAF_MMA_CIND_SET_CNF_STRU *)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCindSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCindSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCindSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CIND_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCindSetCnf : Current Option is not AT_CMD_CIND_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_ERR_NO_ERROR == pstCindSetCnf->enErrorCause)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstCindSetCnf->enErrorCause);
    }

    gstAtSendData.usBufLen = 0;

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaPlmnSearchCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_PLMN_SEARCH_CNF_STRU       *pstPlmnSearchCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstPlmnSearchCnf = (TAF_MMA_PLMN_SEARCH_CNF_STRU *)pstMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPlmnSearchCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPlmnSearchCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPlmnSearchCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if ( (AT_CMD_COPS_SET_AUTOMATIC != gastAtClientTab[ucIndex].CmdCurrentOpt)
      && (AT_CMD_COPS_SET_MANUAL != gastAtClientTab[ucIndex].CmdCurrentOpt)
      && (AT_CMD_COPS_SET_MANUAL_AUTOMATIC_MANUAL != gastAtClientTab[ucIndex].CmdCurrentOpt)
      && (AT_CMD_PLMNSRCH_SET != gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        AT_WARN_LOG("AT_RcvMmaPlmnSearchCnf : Current Option is invalid.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_ERR_NO_ERROR == pstPlmnSearchCnf->enErrorCause)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstPlmnSearchCnf->enErrorCause);
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaPlmnListAbortCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_PLMN_LIST_ABORT_CNF_STRU   *pstPlmnListAbortCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstPlmnListAbortCnf = (TAF_MMA_PLMN_LIST_ABORT_CNF_STRU *)pstMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPlmnListAbortCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPlmnSpecialSelSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPlmnSpecialSelSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    /* 容错处理, 当前不在列表搜ABORT过程中则不上报ABORT.
       如AT的ABORT保护定时器已超时, 之后再收到MMA的ABORT_CNF则不上报ABORT */
    if (AT_CMD_COPS_ABORT_PLMN_LIST != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaPlmnSpecialSelSetCnf : Current Option is not AT_CMD_COPS_SET_MANUAL.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    ulResult = AT_ABORT;

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)

VOS_UINT32 AT_RcvMmaPowerDownCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_MT_POWER_DOWN_CNF_STRU     *pstPrefPlmnTypeSetCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstPrefPlmnTypeSetCnf = (TAF_MMA_MT_POWER_DOWN_CNF_STRU *)pstMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPrefPlmnTypeSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPowerDownCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPowerDownCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_MMA_MT_POWER_DOWN != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaPowerDownCnf : Current Option is not AT_CMD_MMA_MT_POWER_DOWN.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_ERR_NO_ERROR == pstPrefPlmnTypeSetCnf->enErrorCause)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
#endif

VOS_UINT32 AT_RcvMmaPrefPlmnTypeSetCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_PREF_PLMN_TYPE_SET_CNF_STRU    *pstPrefPlmnTypeSetCnf = VOS_NULL_PTR;
    VOS_UINT8                               ucIndex;
    VOS_UINT32                              ulResult;
    AT_MODEM_NET_CTX_STRU                  *pstNetCtx = VOS_NULL_PTR;

    pstPrefPlmnTypeSetCnf = (TAF_MMA_PREF_PLMN_TYPE_SET_CNF_STRU *)pstMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPrefPlmnTypeSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPrefPlmnTypeSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPrefPlmnTypeSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_MMA_SET_PREF_PLMN_TYPE != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaPrefPlmnTypeSetCnf : Current Option is not AT_CMD_MMA_SET_PREF_PLMN_TYPE.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    pstNetCtx->enPrefPlmnType = pstPrefPlmnTypeSetCnf->enPrefPlmnType;

    ulResult = AT_OK;

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}

VOS_UINT32 AT_RcvMmaPhoneModeQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_PHONE_MODE_QRY_CNF_STRU    *pstPhoneModeQryCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstPhoneModeQryCnf = (TAF_MMA_PHONE_MODE_QRY_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPhoneModeQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPhoneModeQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPhoneModeQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前操作是否为查询操作AT_CMD_CFUN_READ */
    if (AT_CMD_CFUN_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaPhoneModeQryCnf : Current Option is not AT_CMD_CFUN_READ.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = 0;

    if (TAF_ERR_NO_ERROR == pstPhoneModeQryCnf->enErrorCause)
    {
        gstAtSendData.usBufLen  = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                         "%s: %d",
                                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                         pstPhoneModeQryCnf->ucPhMode);
        ulResult = AT_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstPhoneModeQryCnf->enErrorCause);
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafMmaQuickStartSetCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_QUICKSTART_SET_CNF_STRU    *pstQuickStartSetCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstQuickStartSetCnf = (TAF_MMA_QUICKSTART_SET_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstQuickStartSetCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaQuickStartSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaQuickStartSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前操作是否为查询操作AT_CMD_CQST_SET */
    if (AT_CMD_CQST_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvTafMmaQuickStartSetCnf : Current Option is not AT_CMD_CQST_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_PARA_OK == pstQuickStartSetCnf->ucResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafMmaAutoAttachSetCnf(
    VOS_VOID                           *pMsg
)
{
     TAF_MMA_AUTO_ATTACH_SET_CNF_STRU   *pstAutoAttachSetCnf = VOS_NULL_PTR;
     VOS_UINT8                           ucIndex;
     AT_RRETURN_CODE_ENUM_UINT32         ulResult = AT_FAILURE;

     pstAutoAttachSetCnf = (TAF_MMA_AUTO_ATTACH_SET_CNF_STRU *)pMsg;

     ucIndex             = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

     /* 通过clientid获取index */
     if (AT_FAILURE == At_ClientIdToUserId(pstAutoAttachSetCnf->stCtrl.usClientId, &ucIndex))
     {
         AT_WARN_LOG("AT_RcvTafMmaAutoAttachSetCnf : WARNING:AT INDEX NOT FOUND!");
         return VOS_ERR;
     }

     if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
     {
         AT_WARN_LOG("AT_RcvTafMmaAutoAttachSetCnf : AT_BROADCAST_INDEX.");
         return VOS_ERR;
     }

     /* 当前操作是否为查询操作AT_CMD_CAATT_SET */
     if (AT_CMD_CAATT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
     {
         AT_WARN_LOG("AT_RcvTafMmaAutoAttachSetCnf : Current Option is not AT_CMD_CAATT_SET.");
         return VOS_ERR;
     }

     AT_STOP_TIMER_CMD_READY(ucIndex);

     if (TAF_PARA_OK == pstAutoAttachSetCnf->ucResult)
     {
         ulResult = AT_OK;
     }
     else
     {
         ulResult = AT_ERROR;
     }

     /* 调用At_FormatResultData发送命令结果 */
     At_FormatResultData(ucIndex, ulResult);

     return VOS_OK;

}


VOS_UINT32 AT_RcvTafMmaSyscfgQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_SYSCFG_QRY_CNF_STRU        *pstSyscfgQryCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    AT_RRETURN_CODE_ENUM_UINT32         ulResult = AT_FAILURE;

    pstSyscfgQryCnf = (TAF_MMA_SYSCFG_QRY_CNF_STRU *)pMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstSyscfgQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaSyscfgQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaSyscfgQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    if(TAF_ERR_NO_ERROR != pstSyscfgQryCnf->enErrorCause)  /* MT本地错误 */
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstSyscfgQryCnf->enErrorCause);       /* 发生错误 */
    }
    else if(AT_CMD_SYSCFG_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        AT_ReportSysCfgQryCmdResult(&(pstSyscfgQryCnf->stSysCfg), ucIndex);

        ulResult = AT_OK;
    }
    else if (AT_CMD_SYSCFGEX_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        AT_ReportSysCfgExQryCmdResult(&(pstSyscfgQryCnf->stSysCfg), ucIndex);

        ulResult = AT_OK;
    }
    else if (AT_CMD_WS46_QRY == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);

        ulResult = AT_ReportWs46QryCmdResult(&(pstSyscfgQryCnf->stSysCfg), ucIndex);
    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}


VOS_UINT32 AT_RcvMmaAccessModeQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_ACCESS_MODE_QRY_CNF_STRU   *pstAccessModeCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstAccessModeCnf    = (TAF_MMA_ACCESS_MODE_QRY_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstAccessModeCnf->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAccessModeQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAccessModeQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CPAM_READ */
    if (AT_CMD_CPAM_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* 如果查询的接入模式为无效值，则返回ERROR，例如当前LTE only不支持GU */
    if (pstAccessModeCnf->enAccessMode >= MN_MMA_CPAM_RAT_TYPE_BUTT)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         "%s: %d,%d",
                                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                         pstAccessModeCnf->enAccessMode,
                                                         pstAccessModeCnf->ucPlmnPrio);
    }

    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCopsQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_COPS_QRY_CNF_STRU          *pstCopsQryCnf;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstCopsQryCnf    = (TAF_MMA_COPS_QRY_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCopsQryCnf->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCopsQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCopsQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_COPS_READ */
    if (AT_CMD_COPS_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    At_QryParaRspCopsProc(ucIndex, pstCopsQryCnf->ucOpId, (VOS_VOID *)&(pstCopsQryCnf->stCopsInfo));

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaRegStateQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_REG_STATE_QRY_CNF_STRU     *pstRegStateCnf;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstRegStateCnf    = (TAF_MMA_REG_STATE_QRY_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRegStateCnf->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaRegStateQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaRegStateQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CREG_READ */
    if (AT_CMD_CREG_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        At_QryParaRspCregProc(ucIndex, pstRegStateCnf->stCtrl.ucOpId, (VOS_VOID *)&(pstRegStateCnf->stRegInfo));
    }
    else if (AT_CMD_CGREG_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        At_QryParaRspCgregProc(ucIndex, pstRegStateCnf->stCtrl.ucOpId, (VOS_VOID *)&(pstRegStateCnf->stRegInfo));
    }
#if(FEATURE_ON == FEATURE_LTE)
    else if (AT_CMD_CEREG_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        AT_QryParaRspCeregProc(ucIndex, pstRegStateCnf->stCtrl.ucOpId, (VOS_VOID *)&(pstRegStateCnf->stRegInfo));
    }
#endif

#if(FEATURE_ON == FEATURE_UE_MODE_NR)
    else if (AT_CMD_C5GREG_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        AT_QryParaRspC5gregProc(ucIndex, pstRegStateCnf->stCtrl.ucOpId, (VOS_VOID *)&(pstRegStateCnf->stRegInfo));
    }
#endif
    else
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaAutoAttachQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_AUTO_ATTACH_QRY_CNF_STRU   *pstAutoAttachCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstAutoAttachCnf    = (TAF_MMA_AUTO_ATTACH_QRY_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstAutoAttachCnf->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAutoAttachQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaAutoAttachQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CAATT_READ */
    if (AT_CMD_CAATT_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    if (TAF_ERR_NO_ERROR == pstAutoAttachCnf->enErrorCause)
    {
        ulResult = AT_OK;

        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         "%s: %d",
                                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                         pstAutoAttachCnf->ucAutoAttachFlag);
    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex,ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaSysInfoQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_SYSINFO_QRY_CNF_STRU       *pstSysInfoCnf;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstSysInfoCnf  = (TAF_MMA_SYSINFO_QRY_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstSysInfoCnf->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSysInfoQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (VOS_FALSE == pstSysInfoCnf->ucIsSupport)
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        At_FormatResultData(ucIndex,AT_CMD_NOT_SUPPORT);

        return VOS_OK;

    }


    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSysInfoQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_SYSINFO_READ */
    if (AT_CMD_SYSINFO_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        At_QryParaRspSysinfoProc(ucIndex, pstSysInfoCnf->ucOpId, (VOS_VOID *)&(pstSysInfoCnf->stSysInfo));

        return VOS_OK;
    }
    else if (AT_CMD_SYSINFOEX_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        AT_QryParaRspSysinfoExProc(ucIndex, pstSysInfoCnf->ucOpId, (VOS_VOID *)&(pstSysInfoCnf->stSysInfo));

        return VOS_OK;
    }
    else
    {
        return VOS_ERR;
    }
}


VOS_UINT32 AT_RcvMtaAnqueryQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pRcvMsg              = VOS_NULL_PTR;
    MTA_AT_ANTENNA_INFO_QRY_CNF_STRU   *pstAntennaInfoQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstAntennaInfoQryCnf    = (MTA_AT_ANTENNA_INFO_QRY_CNF_STRU *)pRcvMsg->aucContent;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaAnqueryQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaAnqueryQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_ANQUERY_READ */
    if (AT_CMD_ANQUERY_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MTA_AT_RESULT_NO_ERROR == pstAntennaInfoQryCnf->enResult)
    {
        AT_QryParaAnQueryProc(ucIndex,
                              pRcvMsg->stAppCtrl.ucOpId,
                              (VOS_VOID *)&(pstAntennaInfoQryCnf->stAntennaInfo));
    }
    else
    {
        ulResult = AT_CME_UNKNOWN;

        At_FormatResultData(ucIndex, ulResult);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaEHplmnInfoQryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_HOME_PLMN_QRY_CNF_STRU     *pstHplmnCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    pstHplmnCnf  = (TAF_MMA_HOME_PLMN_QRY_CNF_STRU *)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstHplmnCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaEHplmnInfoQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaEHplmnInfoQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 根据当前操作类型进入^HPLMN/^APHPLMN的处理 */
    if (AT_CMD_HOMEPLMN_READ == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* ^APHPLMN */
        return AT_RcvMmaApHplmnQryCnf(ucIndex, pstMsg);
    }
    else if (AT_CMD_EHPLMN_LIST_QRY == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* ^HPLMN */
        return AT_RcvMmaHplmnQryCnf(ucIndex, pstMsg);
    }
    else
    {
        return VOS_ERR;
    }

}


VOS_UINT32 AT_RcvMmaApHplmnQryCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_HOME_PLMN_QRY_CNF_STRU     *pstHplmnCnf;
    VOS_UINT32                          ulResult;
    TAF_MMA_HPLMN_WITH_MNC_LEN_STRU     stHplmn;

    /* 初始化 */
    pstHplmnCnf  = (TAF_MMA_HOME_PLMN_QRY_CNF_STRU *)pstMsg;

    /* index的异常判断移到AT_RcvMmaEHplmnInfoQryCnf中 */

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_ERR_NO_ERROR == pstHplmnCnf->enErrorCause)
    {
        TAF_MEM_SET_S(&stHplmn, sizeof(stHplmn), 0x00, sizeof(TAF_MMA_HPLMN_WITH_MNC_LEN_STRU));
        stHplmn.ucHplmnMncLen   = pstHplmnCnf->stEHplmnInfo.ucHplmnMncLen;

        stHplmn.stHplmn.Mcc     = (pstHplmnCnf->stEHplmnInfo.aucImsi[1] & 0xF0) >> 4;
        stHplmn.stHplmn.Mcc     |= (pstHplmnCnf->stEHplmnInfo.aucImsi[2] & 0x0F) << 8;
        stHplmn.stHplmn.Mcc     |= (pstHplmnCnf->stEHplmnInfo.aucImsi[2] & 0xF0) << 12;

        stHplmn.stHplmn.Mnc     = (pstHplmnCnf->stEHplmnInfo.aucImsi[3] & 0x0F);
        stHplmn.stHplmn.Mnc     |= (pstHplmnCnf->stEHplmnInfo.aucImsi[3] & 0xF0) << 4;
        stHplmn.stHplmn.Mnc     |= (pstHplmnCnf->stEHplmnInfo.aucImsi[4] & 0x0F) << 16;

        AT_QryParaHomePlmnProc(ucIndex,
                               pstHplmnCnf->stCtrl.ucOpId,
                               (VOS_VOID *)&stHplmn);

        return VOS_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstHplmnCnf->enErrorCause);

        gstAtSendData.usBufLen = 0;

        At_FormatResultData(ucIndex,ulResult);

        return VOS_ERR;
    }
}


VOS_UINT32 AT_RcvMmaSrvStatusInd(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_SRV_STATUS_IND_STRU        *pstSrvStatusInd;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    /* 初始化消息变量 */
    ucIndex             = 0;
    usLength            = 0;
    pstSrvStatusInd     = (TAF_MMA_SRV_STATUS_IND_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstSrvStatusInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaSrvStatusInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaSrvStatusInd: Get modem id fail.");
        return VOS_ERR;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s%d%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_SRVST].pucText,
                                       pstSrvStatusInd->ucSrvStatus,
                                       gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaImsiRefreshInd(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_IMSI_REFRESH_IND_STRU      *pstImsiRefreshInd;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    /* 初始化消息变量 */
    ucIndex             = 0;
    usLength            = 0;
    pstImsiRefreshInd   = (TAF_MMA_IMSI_REFRESH_IND_STRU *)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstImsiRefreshInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaImsiRefreshInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaImsiRefreshInd: Get modem id fail.");
        return VOS_ERR;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_IMSI_REFRESH].pucText,
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaRssiInfoInd(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_RSSI_INFO_IND_STRU         *pstRssiInfoInd;
    VOS_UINT8                           ucIndex;

    /* 初始化消息变量 */
    ucIndex            = 0;
    pstRssiInfoInd     = (TAF_MMA_RSSI_INFO_IND_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstRssiInfoInd->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaRssiInfoInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    AT_RcvMmaRssiChangeInd(ucIndex, pstRssiInfoInd);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaRegStatusInd(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_REG_STATUS_IND_STRU        *pstRegStatusInd;
    VOS_UINT8                           ucIndex;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstRegStatusInd     = (TAF_MMA_REG_STATUS_IND_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstRegStatusInd->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaRegStatusInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    AT_ProcRegStatusInfoInd(ucIndex, pstRegStatusInd);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaRegRejInfoInd(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_REG_REJ_INFO_IND_STRU      *pstRegRejInd;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    /* 初始化消息变量 */
    ucIndex          = 0;
    usLength         = 0;
    pstRegRejInd     = (TAF_MMA_REG_REJ_INFO_IND_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstRegRejInd->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaRegRejInfoInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaRegRejInfoInd: Get modem id fail.");
        return VOS_ERR;
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_REJINFO].pucText);

    /*打印rejinfo AT命令参数*/
    usLength = AT_PrintRejinfo(usLength, &(pstRegRejInd->stRegRejInfo));

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaPlmnSelectInfoInd(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_PLMN_SElECTION_INFO_IND_STRU   *pstPlmnSelecInd;
    MODEM_ID_ENUM_UINT16                    enModemId;
    VOS_UINT16                              usLength;
    VOS_UINT8                               ucIndex;

    /* 初始化消息变量 */
    ucIndex             = 0;
    usLength            = 0;
    pstPlmnSelecInd     = (TAF_MMA_PLMN_SElECTION_INFO_IND_STRU*)pstMsg;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstPlmnSelecInd->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPlmnSelectInfoInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaPlmnSelectInfoInd: Get modem id fail.");
        return VOS_ERR;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_PLMNSELEINFO].pucText);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d,%d%s",
                                       pstPlmnSelecInd->stPlmnSelectInfo.ulPlmnSelectFlag,
                                       pstPlmnSelecInd->stPlmnSelectInfo.ulPlmnSelectRlst,
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafMmaSyscfgTestCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_SYSCFG_TEST_CNF_STRU       *pstSysCfgTestCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;
    VOS_CHAR                           *pcRoamPara = VOS_NULL_PTR;
    VOS_CHAR                           *pcRatPara = VOS_NULL_PTR;

    pstSysCfgTestCnf = (TAF_MMA_SYSCFG_TEST_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstSysCfgTestCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaSyscfgTestCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaSyscfgTestCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }


    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    if (AT_ROAM_FEATURE_OFF == pstNetCtx->ucRoamFeature)
    {
        pcRoamPara = "(0-2)";
    }
    else
    {
        pcRoamPara = "(0-3)";
    }

#if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) && (FEATURE_ON == FEATURE_UE_MODE_NR))
    pcRatPara = "(\"01\",\"02\",\"03\",\"04\",\"07\",\"08\")";
#elif (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    pcRatPara = "(\"01\",\"02\",\"03\",\"04\",\"07\")";
#elif (FEATURE_ON == FEATURE_UE_MODE_NR)
    pcRatPara = "(\"01\",\"02\",\"03\",\"08\")";
#else
    pcRatPara = "(\"01\",\"02\",\"03\")";
#endif


    /* Modified by l60609 for DSDA Phase III, 2013-2-22, Begin */

    /* SYSCFG */
    if (AT_CMD_SYSCFG_TEST == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR*)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: (2,13,14,16),(0-3),%s,%s,(0-4)",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstSysCfgTestCnf->stBandInfo.strSysCfgBandGroup,
                                                        pcRoamPara);
    }
    /* SYSCFGEX */
    else
    {
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR*)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %s,%s,%s,(0-4),((7fffffffffffffff,\"All bands\"))",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pcRatPara,
                                                        pstSysCfgTestCnf->stBandInfo.strSysCfgBandGroup,
                                                        pcRoamPara);
    }
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}



VOS_UINT32 AT_RcvTafMmaQuickStartQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_QUICKSTART_QRY_CNF_STRU    *pstQuickStartQryCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstQuickStartQryCnf = (TAF_MMA_QUICKSTART_QRY_CNF_STRU *)pMsg;
    ulResult = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstQuickStartQryCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaQuickStartQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaQuickStartQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CQST_READ */
    if (AT_CMD_CQST_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    /* 如果查询的接入模式为无效值，则返回ERROR，例如当前LTE only不支持GU */
    if (TAF_ERR_NO_ERROR != pstQuickStartQryCnf->enErrorCause)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         "%s: %d",
                                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                         pstQuickStartQryCnf->ulQuickStartMode);
    }

    At_FormatResultData(ucIndex,ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaCsnrQryCnf(
    VOS_VOID                           *pMsg
)
{
    MTA_AT_CSNR_QRY_CNF_STRU           *pstrCsnrQryCnf    = VOS_NULL_PTR;
    AT_MTA_MSG_STRU                    *pstRcvMsg         = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstRcvMsg      = (AT_MTA_MSG_STRU *)pMsg;
    pstrCsnrQryCnf = (MTA_AT_CSNR_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult       = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCsnrQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCsnrQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CSNR_READ */
    if (AT_CMD_CSNR_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR != pstrCsnrQryCnf->enResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    (TAF_CHAR*)pgucAtSndCodeAddr,
                                                    "%s: %d,%d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    (TAF_INT32)pstrCsnrQryCnf->stCsnrPara.sCpichRscp,
                                                    (TAF_INT32)pstrCsnrQryCnf->stCsnrPara.sCpichEcNo);
    }

    /* 回复用户命令结果 */
    At_FormatResultData(ucIndex,ulResult);
    return  VOS_OK;
}


VOS_UINT32 AT_RcvTafMmaCsqQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_CSQ_QRY_CNF_STRU           *pstrCsqQryCnf;
    VOS_UINT32                          ulResult = AT_FAILURE;
    VOS_UINT8                           ucIndex;
    TAF_UINT16                          usLength = 0;
    TAF_PH_RSSI_STRU                    stCsq;

    /* 初始化 */
    pstrCsqQryCnf = (TAF_MMA_CSQ_QRY_CNF_STRU *)pMsg;
    ulResult = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstrCsqQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaCsqQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaCsqQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CSQ_SET */
    if (AT_CMD_CSQ_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    TAF_MEM_CPY_S(&stCsq, sizeof(stCsq), &pstrCsqQryCnf->stCsq, sizeof(TAF_PH_RSSI_STRU));
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    if (TAF_ERR_NO_ERROR  != pstrCsqQryCnf->enErrorCause)
    {
        ulResult = AT_ERROR;
    }
    else{
        ulResult = AT_OK;
        if(stCsq.ucRssiNum > 0)
        {
            /* 增加范围限制 */
            if((stCsq.aRssi[0].ucRssiValue >= 31) && (stCsq.aRssi[0].ucRssiValue < 99))
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",31);
            }
            else
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d",stCsq.aRssi[0].ucRssiValue);
            }
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR *)pgucAtSndCodeAddr,(TAF_CHAR *)pgucAtSndCodeAddr + usLength,",%d",stCsq.aRssi[0].ucChannalQual);
        }
    }

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);
    return VOS_OK;

}


VOS_UINT32 AT_RcvMtaCsqlvlQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg         = VOS_NULL_PTR;
    MTA_AT_CSQLVL_QRY_CNF_STRU         *pstrCsqlvlQryCnf  = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    AT_MTA_CSQLVL_PARA_STRU             stCsqlvl;
    AT_MTA_CSQLVLEXT_PARA_STRU          stCsqlvlext;

    /* 初始化 */
    pstRcvMsg        = (AT_MTA_MSG_STRU *)pMsg;
    pstrCsqlvlQryCnf = (MTA_AT_CSQLVL_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult         = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCsqlvlQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCsqlvlQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    TAF_MEM_CPY_S(&stCsqlvlext, sizeof(stCsqlvlext), &pstrCsqlvlQryCnf->stCsqLvlExtPara, sizeof(AT_MTA_CSQLVLEXT_PARA_STRU));
    stCsqlvl = pstrCsqlvlQryCnf->stCsqLvlPara;

    if (MTA_AT_RESULT_NO_ERROR  == pstrCsqlvlQryCnf->enResult)
    {
        if (AT_CMD_CSQLVL_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            /* 复位AT状态 */
            AT_STOP_TIMER_CMD_READY(ucIndex);

            ulResult = AT_OK;
            /* 根据MMA上报的查询结果显示Csqlvl到串口；*/
            gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                                            "%s: %d,%d",
                                                             g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                             stCsqlvl.usLevel,
                                                             stCsqlvl.usRscp);
        }
        else if (AT_CMD_CSQLVLEXT_SET == gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            /* 复位AT状态 */
            AT_STOP_TIMER_CMD_READY(ucIndex);

            ulResult = AT_OK;
            /* 根据MMA上报的查询结果显示Csqlvl到串口；*/
            /* 根据MMA上报的查询结果显示CsqlvlExt到串口；*/
            gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                                            (VOS_CHAR*)pgucAtSndCodeAddr,
                                                            "%s: %d,%d",
                                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                            stCsqlvlext.enRssilv,
                                                            stCsqlvlext.enBer);
        }
        else
        {
            AT_WARN_LOG("AT_RcvMtaCsqlvlQryCnf : CmdCurrentOpt is invalid.");
            ulResult = AT_ERROR;
        }

    }
    else
    {
        ulResult = AT_ERROR;
    }


    At_FormatResultData(ucIndex,ulResult);
    return VOS_TRUE;
}


VOS_UINT32 AT_RcvMmaTimeChangeInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_TIME_CHANGE_IND_STRU        *pstTimeChangeInd;
    TAF_AT_COMM_TIME_STRU               stMmTimeInfo;
    TAF_UINT16                          usLength;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstTimeChangeInd    = (TAF_MMA_TIME_CHANGE_IND_STRU*)pMsg;
    usLength            = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstTimeChangeInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaTimeChangeInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    TAF_MEM_SET_S(&stMmTimeInfo, sizeof(stMmTimeInfo), 0x00, sizeof(stMmTimeInfo));

    stMmTimeInfo.ucDST          = pstTimeChangeInd->ucDST;
    stMmTimeInfo.ucIeFlg        = pstTimeChangeInd->ucIeFlg;
    stMmTimeInfo.cLocalTimeZone = pstTimeChangeInd->cLocalTimeZone;

    TAF_MEM_CPY_S(stMmTimeInfo.aucCurcRptCfg,
            sizeof(stMmTimeInfo.aucCurcRptCfg),
            pstTimeChangeInd->aucCurcRptCfg,
            sizeof(pstTimeChangeInd->aucCurcRptCfg));

    TAF_MEM_CPY_S(stMmTimeInfo.aucUnsolicitedRptCfg,
            sizeof(stMmTimeInfo.aucUnsolicitedRptCfg),
            pstTimeChangeInd->aucUnsolicitedRptCfg,
            sizeof(pstTimeChangeInd->aucUnsolicitedRptCfg));

    stMmTimeInfo.stUniversalTimeandLocalTimeZone.usYear     = pstTimeChangeInd->stUniversalTimeandLocalTimeZone.ucYear;
    stMmTimeInfo.stUniversalTimeandLocalTimeZone.ucMonth    = pstTimeChangeInd->stUniversalTimeandLocalTimeZone.ucMonth;
    stMmTimeInfo.stUniversalTimeandLocalTimeZone.ucDay      = pstTimeChangeInd->stUniversalTimeandLocalTimeZone.ucDay;
    stMmTimeInfo.stUniversalTimeandLocalTimeZone.ucHour     = pstTimeChangeInd->stUniversalTimeandLocalTimeZone.ucHour;
    stMmTimeInfo.stUniversalTimeandLocalTimeZone.ucMinute   = pstTimeChangeInd->stUniversalTimeandLocalTimeZone.ucMinute;
    stMmTimeInfo.stUniversalTimeandLocalTimeZone.ucSecond   = pstTimeChangeInd->stUniversalTimeandLocalTimeZone.ucSecond;
    stMmTimeInfo.stUniversalTimeandLocalTimeZone.cTimeZone  = pstTimeChangeInd->stUniversalTimeandLocalTimeZone.cTimeZone;

    if ((NAS_MM_INFO_IE_UTLTZ == (pstTimeChangeInd->ucIeFlg & NAS_MM_INFO_IE_UTLTZ))
     || (NAS_MM_INFO_IE_LTZ == (pstTimeChangeInd->ucIeFlg & NAS_MM_INFO_IE_LTZ)))
    {
        /* 存在时间信息 */

        usLength += (TAF_UINT16)At_PrintMmTimeInfo(ucIndex,
                                                   &stMmTimeInfo,
                                                   (pgucAtSndCodeAddr + usLength));
    }

    At_SendResultData(ucIndex,pgucAtSndCodeAddr,usLength);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaModeChangeInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;
    TAF_MMA_MODE_CHANGE_IND_STRU       *pstRcvMsg;
    pstRcvMsg = (TAF_MMA_MODE_CHANGE_IND_STRU *)pMsg;
    usLength  = 0;
    enModemId = MODEM_ID_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaCsqlvlextQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_RcvMmaModeChangeInd: Get modem id fail.");
        return VOS_ERR;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s%d,%d%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_MODE].pucText,
                                       pstRcvMsg->RatType,
                                       pstRcvMsg->ucSysSubMode,
                                       gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    /* 此处全局变量维护不再准确，需要删除，用时从C核获取 */

    return VOS_TRUE;

}


VOS_UINT32 AT_RcvMmaPlmnChangeInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;
    TAF_MMA_PLMN_CHANGE_IND_STRU       *pstRcvMsg;

    usLength  = 0;
    pstRcvMsg = (TAF_MMA_PLMN_CHANGE_IND_STRU *)pMsg;
    enModemId = MODEM_ID_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaCsqlvlextQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }
    ulRslt    = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("At_RcvMmaPlmnChangeIndProc: Get modem id fail.");
        return VOS_ERR;
    }


    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s^PLMN:",gaucAtCrLf);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr +
                                       usLength,
                                       "%x%x%x,",
                                       (pstRcvMsg->stCurPlmn.Mcc & 0x0f00)>>8,
                                       (pstRcvMsg->stCurPlmn.Mcc & 0xf0)>>4,
                                       (pstRcvMsg->stCurPlmn.Mcc & 0x0f));

    if (0x0f00 == (pstRcvMsg->stCurPlmn.Mnc & 0x0f00))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           "%x%x",
                                           (pstRcvMsg->stCurPlmn.Mnc & 0xf0)>>4,
                                           (pstRcvMsg->stCurPlmn.Mnc & 0x0f));
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr +
                                           usLength,
                                           "%x%x%x",
                                           (pstRcvMsg->stCurPlmn.Mnc & 0x0f00)>>8,
                                           (pstRcvMsg->stCurPlmn.Mnc & 0xf0)>>4,
                                           (pstRcvMsg->stCurPlmn.Mnc & 0x0f));
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s",gaucAtCrLf);

    At_SendResultData((VOS_UINT8)ucIndex, pgucAtSndCodeAddr, usLength);
    return VOS_TRUE;
}


VOS_UINT32 AT_RcvTafMmaCrpnQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_CRPN_QRY_CNF_STRU          *pstCrpnQryCnf;
    TAF_MMA_CRPN_QRY_INFO_STRU         *pstMnMmaCrpnQryCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;
    VOS_UINT32                          ulLoop;
    VOS_UINT8                           aucPlmn[TAF_MAX_AUTHDATA_PLMN_LEN + 1];
    TAF_MMA_CRPN_QRY_PARA_STRU          stMnMmaCrpnQry;

    /* 初始化 */
    pstCrpnQryCnf           = (TAF_MMA_CRPN_QRY_CNF_STRU *)pMsg;
    pstMnMmaCrpnQryCnf      = &pstCrpnQryCnf->stCrpnQryInfo;
    ulResult                = AT_OK;
    usLength                = 0;
    ulLoop                  = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCrpnQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaCrpnQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafMmaCrpnQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CRPN_QUERY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }


    /* 格式化AT^CRPN查询命令返回 */
    gstAtSendData.usBufLen = 0;
    if (TAF_ERR_NO_ERROR != pstCrpnQryCnf->enErrorCause)
    {
        ulResult = At_ChgTafErrorCode(ucIndex, (VOS_UINT16)pstCrpnQryCnf->enErrorCause);;

        /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        /* 输出结果 */
        At_FormatResultData(ucIndex, ulResult);

        return VOS_OK;
    }

    ulResult = AT_OK;

    for (ulLoop = 0; ulLoop < pstMnMmaCrpnQryCnf->ucTotalNum; ulLoop++)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s",
                                           gaucAtCrLf);

        /* ^CRPN:  */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: ",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        /* <PLMN> */
        /* 参照标杆，如果命令中输入的是PLMN ID，则输出时直接采用输入的PLMN ID */
        if (2 == pstMnMmaCrpnQryCnf->stMnMmaCrpnQry.ucPlmnType)
        {
            TAF_MEM_SET_S(aucPlmn, sizeof(aucPlmn), 0x00, sizeof(aucPlmn));
            if (pstMnMmaCrpnQryCnf->stMnMmaCrpnQry.usPlmnNameLen > TAF_MAX_AUTHDATA_PLMN_LEN)
            {
                TAF_MEM_CPY_S(aucPlmn,
                       sizeof(aucPlmn),
                       pstMnMmaCrpnQryCnf->stMnMmaCrpnQry.aucPlmnName,
                       TAF_MAX_AUTHDATA_PLMN_LEN);
            }
            else
            {
                TAF_MEM_CPY_S(aucPlmn,
                       sizeof(aucPlmn),
                       pstMnMmaCrpnQryCnf->stMnMmaCrpnQry.aucPlmnName,
                       pstMnMmaCrpnQryCnf->stMnMmaCrpnQry.usPlmnNameLen);
            }
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "\"%s\"",
                                               aucPlmn);
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "\"%X%X%X",
                                               (0x0f00 & pstMnMmaCrpnQryCnf->stOperNameList[ulLoop].PlmnId.Mcc) >> 8,
                                               (0x00f0 & pstMnMmaCrpnQryCnf->stOperNameList[ulLoop].PlmnId.Mcc) >> 4,
                                               (0x000f & pstMnMmaCrpnQryCnf->stOperNameList[ulLoop].PlmnId.Mcc));


            if (6 == pstMnMmaCrpnQryCnf->ucMaxMncLen)
            {
                usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%X",
                                                   (0x0f00 & pstMnMmaCrpnQryCnf->stOperNameList[ulLoop].PlmnId.Mnc) >> 8);

            }
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%X%X\"",
                                               (0x00f0 & pstMnMmaCrpnQryCnf->stOperNameList[ulLoop].PlmnId.Mnc) >> 4,
                                               (0x000f & pstMnMmaCrpnQryCnf->stOperNameList[ulLoop].PlmnId.Mnc));
        }

        /* <short name> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",\"%s\"",
                                           pstMnMmaCrpnQryCnf->stOperNameList[ulLoop].aucOperatorNameShort);

        /* <long name> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",\"%s\"",
                                           pstMnMmaCrpnQryCnf->stOperNameList[ulLoop].aucOperatorNameLong);
    }

    gstAtSendData.usBufLen = usLength;

    At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    usLength = 0;

    /* 如果本次上报的CRPN数目与要求的相同，则认为C核仍有Plmn list没有上报，要继续发送请求进行查询 */
    if (TAF_PH_CRPN_PLMN_MAX_NUM == pstMnMmaCrpnQryCnf->ucTotalNum)
    {
        TAF_MEM_CPY_S(&stMnMmaCrpnQry, sizeof(stMnMmaCrpnQry), &pstMnMmaCrpnQryCnf->stMnMmaCrpnQry, sizeof(TAF_MMA_CRPN_QRY_PARA_STRU));
        stMnMmaCrpnQry.ucCurrIndex = pstMnMmaCrpnQryCnf->ucCurrIndex + pstMnMmaCrpnQryCnf->ucTotalNum;
        stMnMmaCrpnQry.ucQryNum    = TAF_PH_CRPN_PLMN_MAX_NUM;

        /* 发送ID_TAF_MMA_CRPN_QRY_REQ消息给MMA处理 */
        if (VOS_TRUE == TAF_MMA_QryCrpnReq( WUEPS_PID_AT,
                                            gastAtClientTab[ucIndex].usClientId,
                                            0,
                                            &stMnMmaCrpnQry))
        {
            gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CRPN_QUERY;
        }
        else
        {
            /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
            AT_STOP_TIMER_CMD_READY(ucIndex);
            gstAtSendData.usBufLen = 0;
            At_FormatResultData(ucIndex, AT_ERROR);
        }
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s",
                                               gaucAtCrLf);
        At_BufferorSendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
        usLength = 0;

        /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = usLength;
        At_FormatResultData(ucIndex, ulResult);
    }

    return VOS_OK;

}


VOS_UINT32 AT_RcvMmaCbcQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_BATTERY_CAPACITY_QRY_CNF_STRU                  *pstCbcCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    TAF_UINT16                          usLength = 0;

    /* 初始化 */
    pstCbcCnf           = (TAF_MMA_BATTERY_CAPACITY_QRY_CNF_STRU *)pMsg;
    ulResult            = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCbcCnf->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCbcQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCbcQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CBC_SET */
    if (AT_CMD_CBC_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "%s: %d",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                      pstCbcCnf->stBatteryStatus.BatteryPowerStatus);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                      ",%d",
                                      pstCbcCnf->stBatteryStatus.BatteryRemains);


    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);
    return VOS_OK;

}


VOS_UINT32 AT_RcvMmaHsQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_HAND_SHAKE_QRY_CNF_STRU    *pstHsCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    TAF_UINT16                          usLength = 0;

    /* 初始化 */
    pstHsCnf            = (TAF_MMA_HAND_SHAKE_QRY_CNF_STRU *)pMsg;
    ulResult            = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstHsCnf->stCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaHsQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaHsQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_HS_READ */
    if (AT_CMD_HS_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }


    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: %d",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                      *(TAF_UINT32*)pstHsCnf->aucBuf);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                      ",%d",pstHsCnf->aucBuf[4]);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                      ",%d",pstHsCnf->aucBuf[5]);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                      ",%d",pstHsCnf->aucBuf[6]);
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (TAF_CHAR *)pgucAtSndCodeAddr,
                                      (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                      ",%d",pstHsCnf->aucBuf[7]);

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex,ulResult);
    return VOS_OK;

}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 AT_RcvMmaHdrCsqInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    MMA_TAF_HDR_CSQ_VALUE_IND_STRU     *pstHdrcsqInd;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstHdrcsqInd        = (MMA_TAF_HDR_CSQ_VALUE_IND_STRU*)pMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstHdrcsqInd->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaCdmaCsqInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^HDRCSQ: %d,%d,%d%s",
                                                    gaucAtCrLf,
                                                    pstHdrcsqInd->sHdrRssi,
                                                    pstHdrcsqInd->sHdrSnr,
                                                    pstHdrcsqInd->sHdrEcio,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;

}


VOS_UINT32 AT_RcvMmaHdrCsqSetCnf(
    VOS_VOID                           *pMsg
)
{
    MMA_TAF_HDR_CSQ_SET_CNF_STRU            *pstHdrCsqCnf;
    VOS_UINT8                                ucIndex;
    VOS_UINT32                               ulResult;

    pstHdrCsqCnf = (MMA_TAF_HDR_CSQ_SET_CNF_STRU *)pMsg;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstHdrCsqCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaHdrCsqSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaHdrCsqSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstHdrCsqCnf->enRslt)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaHdrCsqQryCnf(
    VOS_VOID                           *pMsg
)
{
    MMA_TAF_HDR_CSQ_QRY_SETTING_CNF_STRU *pstHdrCsqQueryCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstHdrCsqQueryCnf  = (MMA_TAF_HDR_CSQ_QRY_SETTING_CNF_STRU*)pMsg;
    usLength            = 0;
    ulResult            = AT_OK;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstHdrCsqQueryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaHdrCsqQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaHdrCsqQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_HDR_CSQ_QRY */
    if (AT_CMD_HDR_CSQ_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s: %d,%d,%d,%d,%d,%d,%d,%d",
                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                     pstHdrCsqQueryCnf->stHdrCsq.enMode,
                                     pstHdrCsqQueryCnf->stHdrCsq.ucTimeInterval,
                                     pstHdrCsqQueryCnf->stHdrCsq.ucRssiThreshold,
                                     pstHdrCsqQueryCnf->stHdrCsq.ucSnrThreshold,
                                     pstHdrCsqQueryCnf->stHdrCsq.ucEcioThreshold,
                                     pstHdrCsqQueryCnf->sHdrRssi,
                                     pstHdrCsqQueryCnf->sHdrSnr,
                                     pstHdrCsqQueryCnf->sHdrEcio);

    gstAtSendData.usBufLen = usLength;

    At_FormatResultData(ucIndex, ulResult);

    /* 输出结果 */
    return VOS_OK;
}
#endif


#if (FEATURE_ON == FEATURE_IMS)

VOS_UINT32 AT_RcvMmaImsSwitchSetCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_IMS_SWITCH_SET_CNF_STRU   *pstCnfMsg = VOS_NULL_PTR;
    VOS_UINT8                          ucIndex;
    VOS_UINT32                         ulResult;

    ucIndex = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    pstCnfMsg = (TAF_MMA_IMS_SWITCH_SET_CNF_STRU*)pMsg;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstCnfMsg->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaImsSwitchSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 广播消息不处理 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaImsSwitchSetCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型 */
    if (VOS_TRUE == pstCnfMsg->bitOpImsSwitch)
    {
        if (AT_CMD_IMS_SWITCH_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            AT_WARN_LOG("AT_RcvMmaImsSwitchSetCnf: WARNING:Not AT_CMD_IMS_SWITCH_SET!");
            return VOS_ERR;
        }
    }
    else if (VOS_TRUE == pstCnfMsg->bitOpRcsSwitch)
    {
        if (AT_CMD_RCSSWITCH_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            AT_WARN_LOG("AT_RcvMmaImsSwitchSetCnf: WARNING:Not AT_CMD_RCSSWITCH_SET!");
            return VOS_ERR;
        }
    }
    else
    {
        AT_WARN_LOG("AT_RcvMmaImsSwitchSetCnf: WARNING:option id error!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_ERR_NO_ERROR == pstCnfMsg->enResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstCnfMsg->enResult);
    }

    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaImsSwitchQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_IMS_SWITCH_QRY_CNF_STRU    *pstSwitchQryCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstSwitchQryCnf     = (TAF_MMA_IMS_SWITCH_QRY_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstSwitchQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaImsSwitchQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaImsSwitchQryCnf : AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_IMS_SWITCH_QRY */
    if (AT_CMD_IMS_SWITCH_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaImsSwitchQryCnf: WARNING:Not AT_CMD_IMS_SWITCH_QRY!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_ERR_NO_ERROR == pstSwitchQryCnf->enResult)
    {
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %d,%d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstSwitchQryCnf->stImsSwitchRat.enLteEnable,
                                                        pstSwitchQryCnf->stImsSwitchRat.enUtranEnable,
                                                        pstSwitchQryCnf->stImsSwitchRat.enGsmEnable);
        ulResult = AT_OK;
    }
    else
    {
        gstAtSendData.usBufLen = 0;
        ulResult = At_ChgTafErrorCode(ucIndex, pstSwitchQryCnf->enResult);
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}


VOS_UINT32 AT_RcvMmaVoiceDomainSetCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_VOICE_DOMAIN_SET_CNF_STRU  *pstCnfMsg = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    ucIndex   = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    pstCnfMsg = (TAF_MMA_VOICE_DOMAIN_SET_CNF_STRU *)pMsg;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstCnfMsg->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaVoiceDomainSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 广播消息不处理 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaVoiceDomainSetCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型 */
    if (AT_CMD_VOICE_DOMAIN_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaVoiceDomainSetCnf: WARNING:Not AT_CMD_VOICE_DOMAIN_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstCnfMsg->enResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstCnfMsg->enErrorCause);
    }

    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_VoiceDomainTransToOutputValue(
    TAF_MMA_VOICE_DOMAIN_ENUM_UINT32    enVoiceDoman,
    VOS_UINT32                         *pulValue
)
{
    VOS_UINT32                          ulRst;

    ulRst = VOS_TRUE;

    switch (enVoiceDoman)
    {
        case TAF_MMA_VOICE_DOMAIN_CS_ONLY:
            *pulValue = AT_VOICE_DOMAIN_TYPE_CS_ONLY;
            break;

        case TAF_MMA_VOICE_DOMAIN_IMS_PS_ONLY:
            *pulValue = AT_VOICE_DOMAIN_TYPE_IMS_PS_ONLY;
            break;

        case TAF_MMA_VOICE_DOMAIN_CS_PREFERRED:
            *pulValue = AT_VOICE_DOMAIN_TYPE_CS_PREFERRED;
            break;

        case TAF_MMA_VOICE_DOMAIN_IMS_PS_PREFERRED:
            *pulValue = AT_VOICE_DOMAIN_TYPE_IMS_PS_PREFERRED;
            break;

        default:
            *pulValue = AT_VOICE_DOMAIN_TYPE_BUTT;
            ulRst = VOS_FALSE;
            break;
    }

    return ulRst;

}


VOS_UINT32 AT_RcvMmaVoiceDomainQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_VOICE_DOMAIN_QRY_CNF_STRU  *pstDomainQryCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulValue;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    ulValue             = 0;
    pstDomainQryCnf     = (TAF_MMA_VOICE_DOMAIN_QRY_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstDomainQryCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaVoiceDomainQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaVoiceDomainQryCnf : AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_VOICE_DOMAIN_QRY */
    if (AT_CMD_VOICE_DOMAIN_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaVoiceDomainQryCnf: WARNING:Not AT_CMD_VOICE_DOMAIN_QRY!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_TRUE != AT_VoiceDomainTransToOutputValue(pstDomainQryCnf->enVoiceDomain, &ulValue))
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
        gstAtSendData.usBufLen = 0;
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       "%s: %d",
                                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                       ulValue);
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaImsDomainCfgSetCnf(VOS_VOID * pMsg)
{
    TAF_MMA_IMS_DOMAIN_CFG_SET_CNF_STRU     *pstDomainCfgSetCnf = VOS_NULL_PTR;
    VOS_UINT32                               ulResult;
    VOS_UINT8                                ucIndex;

    pstDomainCfgSetCnf = (TAF_MMA_IMS_DOMAIN_CFG_SET_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstDomainCfgSetCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaImsDomainCfgSetCnf :WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 判断是否为广播 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaImsDomainCfgSetCnf : AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_IMSDOMAIN_SET */
    if (AT_CMD_IMSDOMAINCFG_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaImsDomainCfgSetCnf : WARNING:Not AT_CMD_IMSDOMAINCFG_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_IMS_DOMAIN_CFG_RESULT_SUCCESS == pstDomainCfgSetCnf->enImsDomainCfgResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaImsDomainCfgQryCnf(VOS_VOID *pMsg)
{
    TAF_MMA_IMS_DOMAIN_CFG_QRY_CNF_STRU     *pstDomainCfgQryCnf = VOS_NULL_PTR;
    VOS_UINT8                                ucIndex;

    pstDomainCfgQryCnf = (TAF_MMA_IMS_DOMAIN_CFG_QRY_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstDomainCfgQryCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaImsDomainCfgQryCnf :WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaImsDomainCfgQryCnf : AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_IMSDOMAINCFG_QRY */
    if (AT_CMD_IMSDOMAINCFG_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaImsDomainCfgQryCnf : WARNING:Not AT_CMD_IMSDOMAIN_QRY!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: %d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    pstDomainCfgQryCnf->enImsDomainCfgType);

    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaRoamImsSupportSetCnf(VOS_VOID * pMsg)
{
    TAF_MMA_ROAM_IMS_SUPPORT_SET_CNF_STRU   *pstRoamImsSupportSetCnf = VOS_NULL_PTR;
    VOS_UINT32                               ulResult;
    VOS_UINT8                                ucIndex;

    pstRoamImsSupportSetCnf = (TAF_MMA_ROAM_IMS_SUPPORT_SET_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRoamImsSupportSetCnf->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaRoamImsSupportSetCnf :WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 判断是否为广播 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaRoamImsSupportSetCnf : AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_ROAM_IMS_SET */
    if (AT_CMD_ROAM_IMS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaRoamImsSupportSetCnf : WARNING:Not AT_CMD_ROAM_IMS_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_MMA_ROAM_IMS_SUPPORT_RESULT_SUCCESS == pstRoamImsSupportSetCnf->enRoamImsSupportResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 AT_RcvMmaCLImsCfgInfoInd(VOS_VOID * pMsg)
{
    TAF_MMA_CLIMS_CFG_INFO_IND_STRU    *pstClImsCfgInfo = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    ucIndex         = 0;
    pstClImsCfgInfo = (TAF_MMA_CLIMS_CFG_INFO_IND_STRU *)pMsg;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstClImsCfgInfo->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCLImsCfgInfoInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen= (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   "%s^CLIMSCFG: %d,%d,%d%s",
                                                   gaucAtCrLf,
                                                   pstClImsCfgInfo->ucClImsSupportFlag,
                                                   pstClImsCfgInfo->ucClVolteMode,
                                                   pstClImsCfgInfo->ucLteImsSupportFlag,
                                                   gaucAtCrLf);


    /* 调用At_SendResultData发送命令结果 */
    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}
#endif

#endif


VOS_UINT32 AT_RcvMtaSetFemctrlCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg;
    MTA_AT_SET_FEMCTRL_CNF_STRU        *pstSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf    = (MTA_AT_SET_FEMCTRL_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex      = 0;
    ulResult     = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetFemctrlCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetFemctrlCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_FEMCTRL_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetFemctrlCnf : Current Option is not AT_CMD_FEMCTRL_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetCnf->enResult)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 AT_RcvMma1xChanSetCnf(
    VOS_VOID                           *pstMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_CFREQ_LOCK_SET_CNF_STRU    *pstCFreqLockSetCnf;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstCFreqLockSetCnf  = (TAF_MMA_CFREQ_LOCK_SET_CNF_STRU*)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCFreqLockSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMma1xChanSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMma1xChanSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_1XCHAN_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMma1xChanSetCnf : Current Option is not AT_CMD_CFREQLOCK_SET.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^1XCHAN命令返回 */
    gstAtSendData.usBufLen = 0;

    if (VOS_OK != pstCFreqLockSetCnf->ulRslt)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMma1xChanQueryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_1XCHAN_QUERY_CNF_STRU      *pst1xChanQueryCnf;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    VOS_INT16                           sInvalidChan;

    /* 初始化 */
    pst1xChanQueryCnf   = (TAF_MMA_1XCHAN_QUERY_CNF_STRU*)pstMsg;
    usLength            = 0;
    sInvalidChan        = -1;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pst1xChanQueryCnf->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMma1xChanQueryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMma1xChanQueryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CVER_QRY */
    if (AT_CMD_1XCHAN_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMma1xChanQueryCnf : Current Option is not AT_CMD_1XCHAN_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_OK == pst1xChanQueryCnf->ulRslt)
    {
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s: %d",
                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                     pst1xChanQueryCnf->usChannel);

        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        /* 查询失败，上报^1XCHAN: BUTT */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s: %d",
                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                     sInvalidChan);

        gstAtSendData.usBufLen = usLength;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCVerQueryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_CVER_QUERY_CNF_STRU        *pstCVerQueryCnf;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    TAF_MMA_1X_CAS_P_REV_ENUM_UINT8     enInvalidVersion;

    /* 初始化 */
    pstCVerQueryCnf     = (TAF_MMA_CVER_QUERY_CNF_STRU*)pstMsg;
    usLength            = 0;
    enInvalidVersion    = TAF_MMA_1X_CAS_P_REV_ENUM_BUTT;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCVerQueryCnf->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCVerQueryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCVerQueryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CVER_QRY */
    if (AT_CMD_CVER_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCVerQueryCnf : Current Option is not AT_CMD_CVER_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_OK == pstCVerQueryCnf->ulRslt)
    {
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s: %d",
                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                     pstCVerQueryCnf->enPrevInUse);

        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        /* 查询失败，上报^CVER: BUTT */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s: %d",
                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                     enInvalidVersion);

        gstAtSendData.usBufLen = usLength;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaStateQueryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_STATE_QUERY_CNF_STRU       *pstStQryCnf;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    VOS_INT8                            cInvalidSta;

    /* 初始化 */
    pstStQryCnf     = (TAF_MMA_STATE_QUERY_CNF_STRU*)pstMsg;
    usLength        = 0;
    cInvalidSta     = -1;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstStQryCnf->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaStateQueryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaStateQueryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CVER_QRY */
    if (AT_CMD_GETSTA_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaStateQueryCnf : Current Option is not AT_CMD_GETST_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_OK == pstStQryCnf->ulRslt)
    {
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s: %d,%d",
                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                     pstStQryCnf->stHandsetSta.ucCasState,
                                     pstStQryCnf->stHandsetSta.ucCasSubSta);

        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        /* 查询失败，上报^GETSTA: BUTT */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s: %d,%d",
                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                     cInvalidSta,
                                     cInvalidSta);

        gstAtSendData.usBufLen = usLength;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCHverQueryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_CHIGHVER_QUERY_CNF_STRU    *pstStaQryCnf;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    TAF_MMA_1X_CAS_P_REV_ENUM_UINT8     enInvalidVer;

    /* 初始化 */
    pstStaQryCnf     = (TAF_MMA_CHIGHVER_QUERY_CNF_STRU*)pstMsg;
    usLength         = 0;
    enInvalidVer     = TAF_MMA_1X_CAS_P_REV_ENUM_BUTT;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstStaQryCnf->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCHverQueryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCHverQueryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /*判断当前操作类型是否为AT_CMD_CHIGHVER_QRY */
    if (AT_CMD_CHIGHVER_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCHverQueryCnf : Current Option is not AT_CMD_CHIGHVER_QRY.");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_OK == pstStaQryCnf->ulRslt)
    {
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s: %d",
                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                     pstStaQryCnf->enHighRev);

        gstAtSendData.usBufLen = usLength;
    }
    else
    {
        /* 查询失败，上报^CHIGHVER: BUTT */
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "%s: %d",
                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                     enInvalidVer);

        gstAtSendData.usBufLen = usLength;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}

#endif

VOS_UINT32 AT_RcvMmaHplmnQryCnf(
    VOS_UINT8                           ucIndex,
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_HOME_PLMN_QRY_CNF_STRU     *pstHplmnQryCnf = VOS_NULL_PTR;
    VOS_UINT16                          usLength;
    VOS_UINT32                          i;
    VOS_UINT8                           aucImsiString[NAS_IMSI_STR_LEN + 1];

    /* 局部变量初始化 */
    pstHplmnQryCnf = (TAF_MMA_HOME_PLMN_QRY_CNF_STRU *)pstMsg;
    usLength       = 0;

    /* index的异常判断移到AT_RcvMmaEHplmnInfoQryCnf中 */

    /* 复位AT状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if ((0 == pstHplmnQryCnf->stEHplmnInfo.ulEHplmnNum)
     || (TAF_ERR_NO_ERROR != pstHplmnQryCnf->enErrorCause))
    {
        AT_WARN_LOG("AT_RcvMmaHplmnQryCnf : EHPLMN num error.");

        /* 查询命令失败，上报错误情况 */
        gstAtSendData.usBufLen = 0;

        At_FormatResultData(ucIndex, At_ChgTafErrorCode(ucIndex, pstHplmnQryCnf->enErrorCause));

        return VOS_ERR;
    }

    /* 将IMSI数字转换为字符串 */
    AT_ConvertImsiDigit2String(pstHplmnQryCnf->stEHplmnInfo.aucImsi, aucImsiString);

    /* 查询命令成功，上报AT^HPLMN */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 "%s: %s,%d,%d,",
                                 g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                 aucImsiString,
                                 pstHplmnQryCnf->stEHplmnInfo.ucHplmnMncLen,
                                 pstHplmnQryCnf->stEHplmnInfo.ulEHplmnNum);

    for ( i = 0; i < (VOS_UINT8)(pstHplmnQryCnf->stEHplmnInfo.ulEHplmnNum - 1); i++ )
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                   "%x%x%x",
                                   (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mcc & 0x0f00)>>8,
                                   (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mcc & 0xf0)>>4,
                                   (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mcc & 0x0f));

        if (0x0f00 == (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0x0f00))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%x%x,",
                                               (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0xf0)>>4,
                                               (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0x0f));
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%x%x%x,",
                                               (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0x0f00)>>8,
                                               (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0xf0)>>4,
                                               (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0x0f));
        }
     }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (TAF_CHAR *)pgucAtSndCodeAddr,
                                   (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                   "%x%x%x",
                                   (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mcc & 0x0f00)>>8,
                                   (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mcc & 0xf0)>>4,
                                   (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mcc & 0x0f));

    if (0x0f00 == (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0x0f00))
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%x%x",
                                           (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0xf0)>>4,
                                           (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0x0f));
    }
    else
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%x%x%x",
                                           (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0x0f00)>>8,
                                           (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0xf0)>>4,
                                           (pstHplmnQryCnf->stEHplmnInfo.astEHplmnList[i].Mnc & 0x0f));
    }

    gstAtSendData.usBufLen = usLength;

    g_enLogPrivacyAtCmd = TAF_LOG_PRIVACY_AT_CMD_HPLMN;
    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);
    g_enLogPrivacyAtCmd = TAF_LOG_PRIVACY_AT_CMD_BUTT;

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaDplmnQryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_DPLMN_QRY_CNF_STRU         *pstDplmnQryCnf = VOS_NULL_PTR;
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           aucTempVersion[NAS_VERSION_LEN + 1];

    /* 局部变量初始化*/
    pstDplmnQryCnf  = (TAF_MMA_DPLMN_QRY_CNF_STRU *)pstMsg;
    usLength        = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstDplmnQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaDplmnQryCnf At_ClientIdToUserId FAILURE");
        return VOS_ERR;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaDplmnQryCnf: Get modem id fail.");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaDplmnQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_DPLMNLIST_QRY */
    if (AT_CMD_DPLMNLIST_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaDplmnQryCnf : Current Option is not AT_CMD_DPLMNLIST_READ.");
        return VOS_ERR;
    }

    /* 复位AT状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 将version数字转换为字符串 */
    TAF_MEM_CPY_S(aucTempVersion, sizeof(aucTempVersion), pstDplmnQryCnf->aucVersionId, NAS_VERSION_LEN);

    /* 加字符串结束符 */
    aucTempVersion[NAS_VERSION_LEN]    = '\0';

    /* 查询命令成功，上报^DPLMN版本号 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 "%s: %s",
                                 g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                 aucTempVersion);

    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaDplmnSetCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_DPLMN_SET_CNF_STRU         *pstDplmnSetCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulRetVal;

    /* 局部变量初始化 */
    pstDplmnSetCnf = (TAF_MMA_DPLMN_SET_CNF_STRU *)pstMsg;
    ulRetVal       = VOS_ERR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstDplmnSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaDplmnSetCnf At_ClientIdToUserId FAILURE");
        return ulRetVal;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaDplmnSetCnf: Get modem id fail.");
        return ulRetVal;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaDplmnSetCnf: AT_BROADCAST_INDEX.");
        return ulRetVal;
    }

    /* 判断当前操作类型是否为AT_CMD_DPLMNLIST_SET */
    if (AT_CMD_DPLMNLIST_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaDplmnSetCnf : Current Option is not AT_CMD_DPLMNLIST_SET.");
        return ulRetVal;
    }

    /* 复位AT状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_OK == pstDplmnSetCnf->ulRslt)
    {
        ulResult = AT_OK;
        ulRetVal = VOS_OK;
    }
    else
    {
        ulResult = AT_CME_UNKNOWN;
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return ulRetVal;
}



VOS_UINT32 AT_RcvMmaExchangeModemInfoCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_EXCHANGE_MODEM_INFO_CNF_STRU                   *pstExchangeNvimCnf = VOS_NULL_PTR;
    VOS_UINT8                                               ucIndex;
    VOS_UINT32                                              ulResult;
    VOS_UINT32                                              ulRetVal;

    /* 局部变量初始化 */
    pstExchangeNvimCnf  = (TAF_MMA_EXCHANGE_MODEM_INFO_CNF_STRU *)pstMsg;
    ulRetVal            = VOS_ERR;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstExchangeNvimCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaExchangeModemInfoCnf: At_ClientIdToUserId FAILURE");
        return ulRetVal;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaExchangeModemInfoCnf: AT_BROADCAST_INDEX.");
        return ulRetVal;
    }

    /* 判断当前操作类型是否为AT_CMD_EXCHANGE_NVIM_SET */
    if (AT_CMD_EXCHANGE_MODEM_INFO_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaExchangeModemInfoCnf: Current Option is not AT_CMD_EXCHANGE_MODEM_INFO_SET.");
        return ulRetVal;
    }

    /* 复位AT状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_OK == pstExchangeNvimCnf->ulRslt)
    {
        ulResult = AT_OK;
        ulRetVal = VOS_OK;
    }
    else
    {
        ulResult = AT_CME_UNKNOWN;
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return ulRetVal;
}


VOS_UINT32 AT_RcvMmaBorderInfoSetCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_BORDER_INFO_SET_CNF_STRU   *pstSetCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulRetVal;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT8                           ucIndex;

    /* 局部变量初始化 */
    pstSetCnf = (TAF_MMA_BORDER_INFO_SET_CNF_STRU *)pstMsg;
    ulRetVal  = VOS_ERR;
    enModemId = MODEM_ID_BUTT;
    ucIndex   = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaBorderInfoSetCnf At_ClientIdToUserId FAILURE");
        return ulRetVal;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaBorderInfoSetCnf: Get modem id fail.");
        return ulRetVal;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaBorderInfoSetCnf: AT_BROADCAST_INDEX.");
        return ulRetVal;
    }

    /* 判断当前操作类型是否为AT_CMD_BORDERINFO_SET */
    if (AT_CMD_BORDERINFO_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaBorderInfoSetCnf : Current Option is not AT_CMD_BORDERINFO_SET.");
        return ulRetVal;
    }

    /* 复位AT状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (VOS_OK == pstSetCnf->ulRslt)
    {
        ulResult = AT_OK;
        ulRetVal = VOS_OK;
    }
    else
    {
        ulResult = AT_CME_UNKNOWN;
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return ulRetVal;
}


VOS_UINT32 AT_RcvMmaBorderInfoQryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_BORDER_INFO_QRY_CNF_STRU   *pstQryCnf = VOS_NULL_PTR;
    VOS_UINT8                           aucTempVersion[NAS_VERSION_LEN + 1];
    VOS_UINT16                          usLength;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT8                           ucIndex;

    /* 局部变量初始化*/
    pstQryCnf  = (TAF_MMA_BORDER_INFO_QRY_CNF_STRU *)pstMsg;
    usLength   = 0;
    enModemId  = MODEM_ID_BUTT;
    ucIndex    = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaBorderInfoQryCnf At_ClientIdToUserId FAILURE");
        return VOS_ERR;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaBorderInfoQryCnf: Get modem id fail.");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaBorderInfoQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_BORDERINFO_QRY */
    if (AT_CMD_BORDERINFO_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaBorderInfoQryCnf : Current Option is not AT_CMD_BORDERINFO_QRY.");
        return VOS_ERR;
    }

    /* 复位AT状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 将version数字转换为字符串 */
    TAF_MEM_CPY_S(aucTempVersion, sizeof(aucTempVersion), pstQryCnf->aucVersionId, NAS_VERSION_LEN);

    /* 加字符串结束符 */
    aucTempVersion[NAS_VERSION_LEN]    = '\0';

    /* 查询命令成功，上报^BORDERINFO版本号 */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 "%s: %s",
                                 g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                 aucTempVersion);

    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 AT_RcvMtaSetFrCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg   = VOS_NULL_PTR;
    MTA_AT_SET_FR_CNF_STRU             *pstSetFrCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetFrCnf  = (MTA_AT_SET_FR_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex      = 0;
    ulResult     = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetFrCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetFrCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_FRSTATUS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetFrCnf : Current Option is not AT_CMD_FRSTATUS_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetFrCnf->enRslt)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaSib16TimeUpdateInd(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg          = VOS_NULL_PTR;
    MTA_AT_SIB16_TIME_UPDATE_STRU      *pstSib16TimeUpdate = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    /* 初始化消息变量 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pstMsg;
    pstSib16TimeUpdate  = (MTA_AT_SIB16_TIME_UPDATE_STRU *)pstRcvMsg->aucContent;
    ucIndex             = 0;
    usLength            = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaSib16TimeUpdateInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    switch(pstSib16TimeUpdate->enRptOptType)
    {
        case MTA_AT_TIME_INFO_RPT_OPT_MMINFO:
            usLength += (VOS_UINT16)At_PrintMmTimeInfo(ucIndex,
                                               &(pstSib16TimeUpdate->stCommTimeInfo),
                                               (pgucAtSndCodeAddr + usLength));
            break;

        case MTA_AT_TIME_INFO_RPT_OPT_SIB16TIME:
            /* "^SIB16TIME: */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s%s",
                                               gaucAtCrLf,
                                               gastAtStringTab[AT_STRING_SIB16TIME].pucText);

            /* YY */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "\"%d/",
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.usYear);
            /* MM */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d%d/",
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.ucMonth / 10,
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.ucMonth % 10);
            /* dd */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d%d,",
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.ucDay / 10,
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.ucDay % 10);

            /* hh */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d%d:",
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.ucHour / 10,
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.ucHour % 10);

            /* mm */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d%d:",
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.ucMinute / 10,
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.ucMinute % 10);

            /* ss */
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d%d",
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.ucSecond / 10,
                                               pstSib16TimeUpdate->stCommTimeInfo.stUniversalTimeandLocalTimeZone.ucSecond % 10);

            /* GMT±tz, Summer(Winter) Time" */
            usLength += (VOS_UINT16)AT_PrintTimeZoneInfoNoAdjustment(&(pstSib16TimeUpdate->stCommTimeInfo),
                                                                     pgucAtSndCodeAddr + usLength);
            break;

        default:
            break;
    }

    /* 输出结果 */
    gstAtSendData.usBufLen = usLength;
    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;

}


/*lint -save -e838 -specific(-e838)*/
/*lint -save -e826 -specific(-e826)*/
VOS_UINT32 AT_RcvMtaAccessStratumRelInd(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg              = VOS_NULL_PTR;
    MTA_AT_ACCESS_STRATUM_REL_IND_STRU *pstAccessStratumRelInd = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    /* 初始化消息变量 */
    pstRcvMsg              = (AT_MTA_MSG_STRU *)pstMsg;
    pstAccessStratumRelInd = (MTA_AT_ACCESS_STRATUM_REL_IND_STRU *)pstRcvMsg->aucContent;
    ucIndex                = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaAccessStratumRelInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    switch(pstAccessStratumRelInd->enAccessStratumRel)
    {
        case MTA_AT_ACCESS_STRATUM_REL8:
            g_stReleaseInfo.enAccessStratumRel = AT_ACCESS_STRATUM_REL8;
            break;

        case MTA_AT_ACCESS_STRATUM_REL9:
            g_stReleaseInfo.enAccessStratumRel = AT_ACCESS_STRATUM_REL9;
            break;

        case MTA_AT_ACCESS_STRATUM_REL10:
            g_stReleaseInfo.enAccessStratumRel = AT_ACCESS_STRATUM_REL10;
            break;

        case MTA_AT_ACCESS_STRATUM_REL11:
            g_stReleaseInfo.enAccessStratumRel = AT_ACCESS_STRATUM_REL11;
            break;

        case MTA_AT_ACCESS_STRATUM_REL12:
            g_stReleaseInfo.enAccessStratumRel = AT_ACCESS_STRATUM_REL12;
            break;

        case MTA_AT_ACCESS_STRATUM_REL13:
            g_stReleaseInfo.enAccessStratumRel = AT_ACCESS_STRATUM_REL13;
            break;
        case MTA_AT_ACCESS_STRATUM_REL14:
            g_stReleaseInfo.enAccessStratumRel = AT_ACCESS_STRATUM_REL14;
            break;
        case MTA_AT_ACCESS_STRATUM_REL_SPARE1:
            g_stReleaseInfo.enAccessStratumRel = AT_ACCESS_STRATUM_REL_SPARE1;
            break;

        default:
            g_stReleaseInfo.enAccessStratumRel = AT_ACCESS_STRATUM_REL8;
            AT_NORM_LOG("AT_RcvMtaAccessStratumRelInd: Wrong Release Number!");
            break;
    }

    return VOS_OK;

}
/*lint -restore*/
/*lint -restore*/

/*lint -save -e838 -specific(-e838)*/
/*lint -save -e826 -specific(-e826)*/

VOS_UINT32 AT_RcvMtaRsInfoQryCnf(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg       = VOS_NULL_PTR;
    MTA_AT_RS_INFO_QRY_CNF_STRU        *pstRsInfoQryCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pstMsg;
    pstRsInfoQryCnf     = (MTA_AT_RS_INFO_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaRsInfoQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaRsInfoQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待+RSRP/+RSRQ命令返回 */
    if ( (AT_CMD_RSRP_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
      && (AT_CMD_RSRQ_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt) )
    {
        AT_WARN_LOG("AT_RcvMtaRsInfoQryCnf : Current Option is not AT_CMD_RSRP_QRY or AT_CMD_RSRQ_QRY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (AT_MTA_RSRP_TYPE == pstRsInfoQryCnf->stRsInfoRslt.enRsInfoType)
    {
        AT_FormatRsrp(ucIndex, pstRsInfoQryCnf);
    }
    else
    {
        AT_FormatRsrq(ucIndex, pstRsInfoQryCnf);
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}
/*lint -restore*/
/*lint -restore*/



/*lint -save -e838 -specific(-e838)*/
/*lint -save -e826 -specific(-e826)*/
VOS_UINT32 AT_RcvMtaEccStatusInd(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg    = VOS_NULL_PTR;
    MTA_AT_ECC_STATUS_IND_STRU         *pstEccStatus = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    /* 初始化消息变量 */
    pstRcvMsg     = (AT_MTA_MSG_STRU *)pstMsg;
    pstEccStatus  = (MTA_AT_ECC_STATUS_IND_STRU *)pstRcvMsg->aucContent;
    ucIndex       = 0;
    usLength      = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaEccStatusInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* "^ECCSTATUS: */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s%u,%u,%u,%u,%u,%u,%u,%u,%u,%d,%d,%d,%d,%d,%d,%d,%d%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_ECC_STATUS].pucText,
                                       pstEccStatus->ucValidflag,
                                       pstEccStatus->ucRxAntNum,
                                       pstEccStatus->ucTmMode,
                                       pstEccStatus->ulCorrQR1Data0011,
                                       pstEccStatus->ulCorrQR1Data01IQ,
                                       pstEccStatus->ulCorrQR2Data0011,
                                       pstEccStatus->ulCorrQR2Data01IQ,
                                       pstEccStatus->ulCorrQR3Data0011,
                                       pstEccStatus->ulCorrQR3Data01IQ,
                                       pstEccStatus->sRsrpR0,
                                       pstEccStatus->sRsrpR1,
                                       pstEccStatus->sRsrpR2,
                                       pstEccStatus->sRsrpR3,
                                       pstEccStatus->sRssiR0,
                                       pstEccStatus->sRssiR1,
                                       pstEccStatus->sRssiR2,
                                       pstEccStatus->sRssiR3,
                                       gaucAtCrLf);

    /* 输出结果 */
    gstAtSendData.usBufLen = usLength;
    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;

}


VOS_UINT32 AT_RcvMtaEccCfgCnf(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg       = VOS_NULL_PTR;
    MTA_AT_SET_ECC_CFG_CNF_STRU        *pstEccCfgSetCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pstMsg;
    pstEccCfgSetCnf     = (MTA_AT_SET_ECC_CFG_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex             = 0;
    ulResult            = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaEccCfgCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaEccCfgCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_ECCCFG_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaEccCfgCnf : Current Option is not AT_CMD_ECCCFG_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstEccCfgSetCnf->enResult)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
/*lint -restore*/
/*lint -restore*/

/*lint -save -e838 -specific(-e838)*/
/*lint -save -e826 -specific(-e826)*/

VOS_UINT32 AT_RcvMtaErrcCapCfgCnf(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg        = VOS_NULL_PTR;
    MTA_AT_ERRCCAP_CFG_CNF_STRU        *pstErrcCapCfgCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pstMsg;
    pstErrcCapCfgCnf    = (MTA_AT_ERRCCAP_CFG_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex             = 0;
    ulResult            = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaErrcCapCfgCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaErrcCapCfgCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待AT^ERRCCAPCFG命令返回 */
    if (AT_CMD_ERRCCAPCFG_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaErrcCapCfgCnf : Current Option is not AT_CMD_ERRCCAPCFG_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MTA_AT_RESULT_NO_ERROR == pstErrcCapCfgCnf->enResult)
    {
        ulResult = AT_OK;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaErrcCapQryCnf(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg        = VOS_NULL_PTR;
    MTA_AT_ERRCCAP_QRY_CNF_STRU        *pstErrcCapQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pstMsg;
    pstErrcCapQryCnf    = (MTA_AT_ERRCCAP_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex             = 0;
    ulResult            = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaErrcCapQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaErrcCapCfgCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待^ERRCCAPQRY命令返回 */
    if (AT_CMD_ERRCCAPQRY_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaErrcCapCfgCnf : Current Option is not AT_CMD_ERRCCAPQRY_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstErrcCapQryCnf->enResult)
    {
        /* ^ERRCCAPQRY:  */
        gstAtSendData.usBufLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr,
                                                         (VOS_CHAR *)pgucAtSndCodeAddr + gstAtSendData.usBufLen,
                                                         "^ERRCCAPQRY: %u,%u,%u,%u",
                                                         pstErrcCapQryCnf->ulErrcCapType,
                                                         pstErrcCapQryCnf->ulPara1,
                                                         pstErrcCapQryCnf->ulPara2,
                                                         pstErrcCapQryCnf->ulPara3);
        ulResult = AT_OK;
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
/*lint -restore*/
/*lint -restore*/
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_NR)


VOS_UINT32 AT_RcvMtaLendcQryCnf(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg        = VOS_NULL_PTR;
    MTA_AT_LENDC_QRY_CNF_STRU          *pstLendcQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pstMsg;
    pstLendcQryCnf      = (MTA_AT_LENDC_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex             = 0;
    ulResult            = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaLendcQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaLendcQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待^LENDC命令返回 */
    if (AT_CMD_LENDC_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaLendcQryCnf : Current Option is not AT_CMD_LENDC_QRY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstLendcQryCnf->enRslt)
    {
        /* ^LENDC:  */
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                                       (TAF_CHAR*)pgucAtSndCodeAddr,
                                                        "%s: %d,%d,%d",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstLendcQryCnf->ulReportFlag,
                                                        (TAF_UINT8)pstLendcQryCnf->stEndcInfo.ucEutranNrFlag,
                                                        (TAF_UINT8)pstLendcQryCnf->stEndcInfo.ucNrPscellFlag);

        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ConvertMtaResult(pstLendcQryCnf->enRslt);
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaLendcInfoInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_LENDC_INFO_IND_STRU         *pstLendcInfoInd;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstMtaMsg           = (AT_MTA_MSG_STRU*)pMsg;
    pstLendcInfoInd     = (MTA_AT_LENDC_INFO_IND_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaLendcInfoInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   "%s^LENDC: %d,%d%s",
                                                    gaucAtCrLf,
                                                    pstLendcInfoInd->ucEutranNrFlag,
                                                    pstLendcInfoInd->ucNrPscellFlag,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)
VOS_UINT32 AT_RcvMtaNrFreqLockSetCnf(VOS_VOID * pMsg)
{
    AT_MTA_MSG_STRU                    *pRcvMsg            = VOS_NULL_PTR;
    MTA_AT_NRRC_FREQLOCK_SET_CNF_STRU  *pstNrrcFreqlockCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstNrrcFreqlockCnf  = (MTA_AT_NRRC_FREQLOCK_SET_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaNrFreqLockSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaNrFreqLockSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_NRFREQLOCK_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaNrFreqLockSetCnf : Current Option is not AT_CMD_NRFREQLOCK_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^NRFREQLOCK命令返回 */
    gstAtSendData.usBufLen = 0;

    ulResult = AT_ConvertMtaResult(pstNrrcFreqlockCnf->enResult);

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaNrFreqLockQryCnf(VOS_VOID * pMsg)
{
    AT_MTA_MSG_STRU                    *pRcvMsg            = VOS_NULL_PTR;
    MTA_AT_NRRC_FREQLOCK_QRY_CNF_STRU  *pstNrrcFreqlockCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstNrrcFreqlockCnf  = (MTA_AT_NRRC_FREQLOCK_QRY_CNF_STRU *)pRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaNrFreqLockQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaNrFreqLockQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_NRFREQLOCK_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaNrFreqLockQryCnf : Current Option is not AT_CMD_NRFREQLOCK_QRY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^NRFREQLOCK查询命令返回 */
    gstAtSendData.usBufLen = 0;
    if (VOS_OK != pstNrrcFreqlockCnf->enResult)
    {
        ulResult = AT_ConvertMtaResult(pstNrrcFreqlockCnf->enResult);
    }
    else
    {
        ulResult = AT_OK;

        switch (pstNrrcFreqlockCnf->enFreqType)
        {
            case MTA_AT_FREQLOCK_TYPE_LOCK_NONE:
                gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                                               "%s: %d",
                                                                g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                               pstNrrcFreqlockCnf->enFreqType);
                break;

            case MTA_AT_FREQLOCK_TYPE_LOCK_FREQ:
                gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                                               "%s: %d,%d,%d,%u",
                                                                g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                               pstNrrcFreqlockCnf->enFreqType,
                                                               pstNrrcFreqlockCnf->enScsType,
                                                               pstNrrcFreqlockCnf->usBand,
                                                               pstNrrcFreqlockCnf->ulNrArfcn);
                break;

            case MTA_AT_FREQLOCK_TYPE_LOCK_CELL:
                gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                                               "%s: %d,%d,%d,%u,%d",
                                                                g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                               pstNrrcFreqlockCnf->enFreqType,
                                                               pstNrrcFreqlockCnf->enScsType,
                                                               pstNrrcFreqlockCnf->usBand,
                                                               pstNrrcFreqlockCnf->ulNrArfcn,
                                                               pstNrrcFreqlockCnf->usPhyCellId);
                break;

            case MTA_AT_FREQLOCK_TYPE_LOCK_BAND:
                gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                                               "%s: %d,%d,%d",
                                                                g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                               pstNrrcFreqlockCnf->enFreqType,
                                                               pstNrrcFreqlockCnf->enScsType,
                                                               pstNrrcFreqlockCnf->usBand);
                break;

            default:
                AT_WARN_LOG("AT_RcvMtaNrFreqLockQryCnf : Current req type is out of bounds.");
                return VOS_ERR;
        }
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
#endif

#endif


VOS_UINT32 AT_RcvMtaCommBoosterInd(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg           = VOS_NULL_PTR;
    MTA_AT_COMM_BOOSTER_IND_STRU       *pstMtaAtCommBooster = VOS_NULL_PTR;
    VOS_UINT32                          i = 0;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* 初始化消息变量 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pstMsg;
    pstMtaAtCommBooster  = (MTA_AT_COMM_BOOSTER_IND_STRU *)pstRcvMsg->aucContent;
    ucIndex             = 0;
    usLength            = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaCommBoosterInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (pstMtaAtCommBooster->ulLen > MTA_AT_BOOSTER_IND_MAX_NUM)
    {
        pstMtaAtCommBooster->ulLen = MTA_AT_BOOSTER_IND_MAX_NUM;
    }

    /* \r\n^BOOSTERNTF */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_BOOSTERNTF].pucText);

    /* module index, len */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d, %d,\"",
                                       pstMtaAtCommBooster->ulPidIndex,
                                       pstMtaAtCommBooster->ulLen);

    for (i = 0; ((i < pstMtaAtCommBooster->ulLen) && (i < MTA_AT_BOOSTER_IND_MAX_NUM)); i++)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%02X",
                                       pstMtaAtCommBooster->aucBoosterIndData[i]);
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "\"%s", gaucAtCrLf);

    /* 输出结果 */
    gstAtSendData.usBufLen = usLength;
    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;

}


VOS_UINT32 AT_RcvMtaCommBoosterSetCnf(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg               = VOS_NULL_PTR;
    MTA_AT_COMM_BOOSTER_SET_CNF_STRU       *pstMtaAtSetCommBoosterCnf  = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstMtaAtSetCommBoosterCnf = (MTA_AT_COMM_BOOSTER_SET_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult                  = AT_OK;
    ucIndex                   = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCommBoosterSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCommBoosterSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_COMM_BOOSTER_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaCommBoosterSetCnf : Current Option is not AT_CMD_COMM_BOOSTER_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = 0;

    if ((VOS_FALSE == pstMtaAtSetCommBoosterCnf->ucInternalResultFlag)
     && (VOS_FALSE == pstMtaAtSetCommBoosterCnf->ucExternalResultFlag))
    {
        ulResult = AT_OK;
        gstAtSendData.usBufLen =
            (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   "%s: %d",
                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                   0);
    }
    else if (VOS_TRUE == pstMtaAtSetCommBoosterCnf->ucInternalResultFlag)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_ERROR;
        gstAtSendData.usBufLen =
            (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   "%s: %d",
                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                   pstMtaAtSetCommBoosterCnf->ulResult);
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaCommBoosterQueryCnf(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg                    = VOS_NULL_PTR;
    MTA_AT_COMM_BOOSTER_QUERY_CNF_STRU *pstMtaAtQueryCommBoosterCnf  = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;
    VOS_UINT32                          i;

    /* 初始化 */
    pstRcvMsg                   = (AT_MTA_MSG_STRU *)pMsg;
    pstMtaAtQueryCommBoosterCnf = (MTA_AT_COMM_BOOSTER_QUERY_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult                    = AT_OK;
    ucIndex                     = 0;
    usLength                    = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCommBoosterQueryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaCommBoosterQueryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_COMM_BOOSTER_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaCommBoosterQueryCnf : Current Option is not AT_CMD_COMM_BOOSTER_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = 0;

    if (VOS_FALSE == pstMtaAtQueryCommBoosterCnf->ucResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        /* \r\n^COMMBOOSTER */
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s: \"",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

        if (pstMtaAtQueryCommBoosterCnf->ulLen > MTA_AT_BOOSTER_QRY_CNF_MAX_NUM)
        {
            pstMtaAtQueryCommBoosterCnf->ulLen = MTA_AT_BOOSTER_QRY_CNF_MAX_NUM;
        }

        for (i = 0; (i < pstMtaAtQueryCommBoosterCnf->ulLen); i++)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%02X",
                                           pstMtaAtQueryCommBoosterCnf->aucDate[i]);
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "\"");
        gstAtSendData.usBufLen = usLength;

    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}



VOS_UINT32 AT_RcvGameModeSetCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg               = VOS_NULL_PTR;
    MTA_AT_GAME_MODE_SET_CFN_STRU          *pstSetCnf               = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf                 = (MTA_AT_GAME_MODE_SET_CFN_STRU *)pstRcvMsg->aucContent;
    ulResult                  = AT_OK;
    ucIndex                   = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvGameModeSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 判断是否为广播 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvGameModeSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_GAME_MODE_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvGameModeSetCnf : Current Option is not AT_CMD_SAMPLE_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetCnf->ucResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 AT_RcvCdmaModemSetCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg               = VOS_NULL_PTR;
    MTA_AT_CDMA_MODEM_CAP_SET_CNF_STRU     *pstSetCnf               = VOS_NULL_PTR;
    AT_MTA_MODEM_CAP_UPDATE_REQ_STRU        stAtMtaModemCapUpdate;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;
    VOS_UINT8                               ucIsCdmaModemSwitchNotResetFlg;


    /* 初始化 */
    pstRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf                 = (MTA_AT_CDMA_MODEM_CAP_SET_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex                   = 0;
    ulResult                  = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvCdmaModemSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 判断是否为广播 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvCdmaModemSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CDMAMODEMSWITCH_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvCdmaModemSetCnf : Current Option is not AT_CMD_CDMAMODEMSWITCH_SET.");
        return VOS_ERR;
    }


    AT_NORM_LOG1("AT_RcvCdmaModemSetCnf : return Result ", pstSetCnf->ucResult);

    if (MTA_AT_RESULT_ERROR == pstSetCnf->ucResult)
    {
        AT_WARN_LOG1("AT_RcvCdmaModemSetCnf : MTA Return Error. ucResult", pstSetCnf->ucResult);
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;

        At_FormatResultData(ucIndex, AT_CME_WRITE_NV_TimeOut);
        return VOS_OK;
    }
    else if (MTA_AT_RESULT_NO_ERROR != pstSetCnf->ucResult)
    {
        AT_WARN_LOG1("AT_RcvCdmaModemSetCnf : MTA Return Error. ucResult", pstSetCnf->ucResult);
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;

        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_OK;
    }
    else
    {
        ulResult = AT_OK;
    }
    AT_ReadPlatformNV();

    /*  at 命令入口处已经检查，到了此处，必定时modem0 */
    ucIsCdmaModemSwitchNotResetFlg = AT_GetModemCdmaModemSwitchCtxAddrFromModemId(MODEM_ID_0)->ucEnableFlg;

    if (VOS_FALSE == ucIsCdmaModemSwitchNotResetFlg)
    {
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;

        At_FormatResultData(ucIndex, ulResult);

        return VOS_OK;
    }

    TAF_MEM_SET_S(&stAtMtaModemCapUpdate, (VOS_UINT32)sizeof(AT_MTA_MODEM_CAP_UPDATE_REQ_STRU),
                  0x00, (VOS_UINT32)sizeof(AT_MTA_MODEM_CAP_UPDATE_REQ_STRU));

    /* 更新了平台能力，通知AT->MTA->RRM，进行底层平台能力更新 */
    /* AT发送至MTA的消息结构赋值 */
    stAtMtaModemCapUpdate.enModemCapUpdateType = AT_MTA_MODEM_CAP_UPDATE_TYPE_CDMA_MODEM_SWITCH;

    /* 发送消息给C核处理 */
    if (AT_SUCCESS != AT_FillAndSndAppReqMsg(gastAtClientTab[ucIndex].usClientId,
                                                 0,
                                                 ID_AT_MTA_MODEM_CAP_UPDATE_REQ,
                                                 &stAtMtaModemCapUpdate,
                                                 (VOS_UINT32)sizeof(AT_MTA_MODEM_CAP_UPDATE_REQ_STRU),
                                                 I0_UEPS_PID_MTA))
    {
        AT_WARN_LOG("AT_RcvCdmaModemSetCnf: AT_FillAndSndAppReqMsg Fail!");
        /* 复位AT状态 */
        AT_STOP_TIMER_CMD_READY(ucIndex);
        gstAtSendData.usBufLen = 0;

        At_FormatResultData(ucIndex, ulResult);
        return VOS_OK;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_MODEM_CAP_UPDATE_SET;

    return AT_WAIT_ASYNC_RETURN;
}



VOS_UINT32 AT_RcvCdmaCapResumeSetCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg               = VOS_NULL_PTR;
    MTA_AT_CDMA_CAP_RESUME_SET_CNF_STRU    *pstSetCnf               = VOS_NULL_PTR;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf                 = (MTA_AT_CDMA_CAP_RESUME_SET_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex                   = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvCdmaCapResumeSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 判断是否为广播 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvCdmaCapResumeSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CDMACAPRESUME_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvCdmaCapResumeSetCnf : Current Option is not AT_CMD_CDMACAPRESUME_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = 0;

    AT_NORM_LOG1("AT_RcvCdmaCapResumeSetCnf : return Result ",pstSetCnf->ucResult);

    /*  平台能力恢复失败，后续流行不再处理，at命令返回失败 */
    if (MTA_AT_RESULT_NO_ERROR != pstSetCnf->ucResult)
    {
        AT_WARN_LOG1("AT_RcvCdmaCapResumeSetCnf : MTA Return Error. ucResult", pstSetCnf->ucResult);

        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_OK;
    }

    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


#endif


VOS_UINT32 AT_RcvMtaNvLoadSetCnf(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg               = VOS_NULL_PTR;
    MTA_AT_SET_NVLOAD_CNF_STRU             *pstSetCnf               = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf                 = (MTA_AT_SET_NVLOAD_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult                  = AT_OK;
    ucIndex                   = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaNvLoadSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 判断是否为广播 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaNvLoadSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_NVLOAD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaNvLoadSetCnf : Current Option is not AT_CMD_NVLOAD_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = 0;

    if (TAF_ERR_NO_ERROR == pstSetCnf->enResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstSetCnf->enResult);
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}




VOS_UINT32 AT_RcvMtaPseucellInfoSetCnf(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg;
    MTA_AT_PSEUCELL_INFO_SET_CNF_STRU  *pstPseucellInfoSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg             = (AT_MTA_MSG_STRU *)pstMsg;
    pstPseucellInfoSetCnf = (MTA_AT_PSEUCELL_INFO_SET_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex               = 0;
    ulResult              = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaPseucellInfoSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaPseucellInfoSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_PSEUCELL_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaPseucellInfoSetCnf : Current Option is not AT_CMD_PSEUCELL_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstPseucellInfoSetCnf->enResult)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaChrAlarmRlatCfgSetCnf(
    VOS_VOID                           *pstMsg
)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg;
    MTA_AT_CHR_ALARM_RLAT_CFG_SET_CNF_STRU *pstChrAlarmSetCnf;
    VOS_UINT32                              ulResult;
    VOS_UINT16                              usLength;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg             = (AT_MTA_MSG_STRU *)pstMsg;
    pstChrAlarmSetCnf     = (MTA_AT_CHR_ALARM_RLAT_CFG_SET_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex               = 0;
    ulResult              = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaChrAlarmRlatCfgSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaChrAlarmRlatCfgSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CHRALARMRLATCFG_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaChrAlarmRlatCfgSetCnf : Current Option is not AT_CMD_CHRALARMRLATCFG_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstChrAlarmSetCnf->enResult)
    {
        if (AT_MTA_CHR_ALARM_RLAT_OP_READ == pstChrAlarmSetCnf->enAlarmOp)
        {
            usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               "%s: %d",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                               pstChrAlarmSetCnf->ulAlarmDetail);

            gstAtSendData.usBufLen = usLength;
        }

        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaClearHistoryFreqCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg   = VOS_NULL_PTR;
    MTA_AT_RESULT_CNF_STRU             *pstSetCnf   = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf    = (MTA_AT_RESULT_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex      = 0;
    ulResult     = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaClearHistoryFreqCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaClearHistoryFreqCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CLEAR_HISTORY_FREQ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaClearHistoryFreqCnf : Current Option is not AT_CMD_CLEAR_HISTORY_FREQ.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetCnf->enResult)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaSrchedPlmnInfoInd(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_SRCHED_PLMN_INFO_IND_STRU  *pstSrchedPlmnInfo = VOS_NULL_PTR;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          i;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRslt;

    usLength    = 0;
    enModemId   = MODEM_ID_0;
    pstSrchedPlmnInfo  = (TAF_MMA_SRCHED_PLMN_INFO_IND_STRU*)pMsg;

    if (AT_FAILURE == At_ClientIdToUserId(pstSrchedPlmnInfo->usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaSrchedPlmnInfoInd At_ClientIdToUserId FAILURE");
        return VOS_ERR;
    }

    ulRslt = AT_GetModemIdFromClient(ucIndex, &enModemId);

    if (VOS_OK != ulRslt)
    {
        AT_ERR_LOG("AT_RcvMmaSrchedPlmnInfoInd: Get modem id fail.");
        return VOS_ERR;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_SRCHEDPLMNINFO].pucText);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%d,%d",
                                       pstSrchedPlmnInfo->enRat,
                                       pstSrchedPlmnInfo->ulLaiNum);

    for (i = 0; i < pstSrchedPlmnInfo->ulLaiNum; i++)
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (TAF_CHAR *)pgucAtSndCodeAddr,
                                           (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",%x%x%x",
                                           (pstSrchedPlmnInfo->astLai[i].stPlmnId.Mcc & 0x0f00) >> 8,
                                           (pstSrchedPlmnInfo->astLai[i].stPlmnId.Mcc & 0xf0) >> 4,
                                           (pstSrchedPlmnInfo->astLai[i].stPlmnId.Mcc & 0x0f));

        if (0x0f00 == (pstSrchedPlmnInfo->astLai[i].stPlmnId.Mnc & 0x0f00))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%x%x,",
                                               (pstSrchedPlmnInfo->astLai[i].stPlmnId.Mnc & 0xf0) >> 4,
                                               (pstSrchedPlmnInfo->astLai[i].stPlmnId.Mnc & 0x0f));
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (TAF_CHAR *)pgucAtSndCodeAddr,
                                               (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%x%x%x,",
                                               (pstSrchedPlmnInfo->astLai[i].stPlmnId.Mnc & 0x0f00) >> 8,
                                               (pstSrchedPlmnInfo->astLai[i].stPlmnId.Mnc & 0xf0) >> 4,
                                               (pstSrchedPlmnInfo->astLai[i].stPlmnId.Mnc & 0x0f));
        }
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
        if (TAF_MMA_RAT_NR == pstSrchedPlmnInfo->enRat)
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%d",
                                           pstSrchedPlmnInfo->astLai[i].ulLac);
        }
        else
#endif
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%d",
                                               (VOS_UINT16)pstSrchedPlmnInfo->astLai[i].ulLac);
        }
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",%d",
                                           pstSrchedPlmnInfo->aucPlmnstatus[i]);
    }


    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s",
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaTransModeQryCnf(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg               = VOS_NULL_PTR;
    MTA_AT_TRANSMODE_QRY_CNF_STRU          *pstMtaAtQryTransModeCnf = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg                = (AT_MTA_MSG_STRU *)pMsg;
    pstMtaAtQryTransModeCnf  = (MTA_AT_TRANSMODE_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult                 = AT_OK;
    ucIndex                  = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaTransModeQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaTransModeQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_TRANSMODE_READ!= gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaTransModeQryCnf : Current Option is not AT_CMD_RRC_VERSION_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = 0;
    if (MTA_AT_RESULT_NO_ERROR != pstMtaAtQryTransModeCnf->enResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       "%s: %d",
                                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                       pstMtaAtQryTransModeCnf->ucTransMode);
    }
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaUECenterQryCnf(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg               = VOS_NULL_PTR;
    MTA_AT_QRY_UE_CENTER_CNF_STRU          *pstMtaAtQryUECenterCnf  = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg                = (AT_MTA_MSG_STRU *)pMsg;
    pstMtaAtQryUECenterCnf   = (MTA_AT_QRY_UE_CENTER_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult                 = AT_OK;
    ucIndex                  = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaUECenterQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaUECenterQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_UE_CENTER_QRY!= gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaUECenterQryCnf : Current Option is not AT_CMD_RRC_VERSION_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = 0;
    if (MTA_AT_RESULT_NO_ERROR != pstMtaAtQryUECenterCnf->enResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                                       "%s: %d",
                                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                       pstMtaAtQryUECenterCnf->enUeCenter);
    }
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaUECenterSetCnf(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg               = VOS_NULL_PTR;
    MTA_AT_SET_UE_CENTER_CNF_STRU          *pstMtaAtSetUECenterCnf  = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;

    /* 初始化 */
    pstRcvMsg                = (AT_MTA_MSG_STRU *)pMsg;
    pstMtaAtSetUECenterCnf   = (MTA_AT_SET_UE_CENTER_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult                 = AT_OK;
    ucIndex                  = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaUECenterQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaUECenterQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_UE_CENTER_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaUECenterQryCnf : Current Option is not AT_CMD_RRC_VERSION_QUERY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    gstAtSendData.usBufLen = 0;
    if (MTA_AT_RESULT_NO_ERROR != pstMtaAtSetUECenterCnf->enResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;
    }
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}



VOS_UINT32 AT_RcvMmaInitLocInfoInd(
    VOS_VOID                           *pMsg
)
{
    VOS_UINT8                           ucIndex;
    TAF_MMA_INIT_LOC_INFO_IND_STRU     *pstInitLocInfoInd;

    /* 初始化消息变量 */
    ucIndex             = 0;
    pstInitLocInfoInd   = (TAF_MMA_INIT_LOC_INFO_IND_STRU *)pMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstInitLocInfoInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaInitLocInfoInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    gstAtSendData.usBufLen = 0;

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s^INITLOCINFO: %x%x%x,%d%s",
                                                    gaucAtCrLf,
                                                    (pstInitLocInfoInd->ulMcc & 0x0f00)>>8,
                                                    (pstInitLocInfoInd->ulMcc & 0x00f0)>>4,
                                                    (pstInitLocInfoInd->ulMcc & 0x000f),
                                                    pstInitLocInfoInd->lSid,
                                                    gaucAtCrLf);

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaEflociInfoSetCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_EFLOCIINFO_SET_CNF_STRU    *pstEflociInfoSetCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstEflociInfoSetCnf = (TAF_MMA_EFLOCIINFO_SET_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEflociInfoSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaEflociInfoSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaEflociInfoSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_EFLOCIINFO_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaEflociInfoSetCnf : Current Option is not AT_CMD_EFLOCIINFO_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstEflociInfoSetCnf->enRslt)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaEflociInfoQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_EFLOCIINFO_QRY_CNF_STRU    *pstEfloiinfoQryCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;

    pstEfloiinfoQryCnf = (TAF_MMA_EFLOCIINFO_QRY_CNF_STRU *)pMsg;
    usLength = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEfloiinfoQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaEflociInfoQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaEflociInfoQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_EFLOCIINFO_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaEflociInfoQryCnf : Current Option is not AT_CMD_EFLOCIINFO_QRY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 查询失败直接返回ERROR */
    if (TAF_MMA_APP_OPER_RESULT_SUCCESS != pstEfloiinfoQryCnf->enRslt)
    {
        ulResult = AT_ERROR;

        At_FormatResultData(ucIndex, ulResult);
        return VOS_OK;
    }

    /* <tmsi> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 "%s: 0x%08x",
                                 g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                 (VOS_UINT32)pstEfloiinfoQryCnf->stEflociInfo.ulTmsi);

    /* <plmn> */
    if (0xfff == pstEfloiinfoQryCnf->stEflociInfo.stPlmnId.Mcc
     && 0xfff == pstEfloiinfoQryCnf->stEflociInfo.stPlmnId.Mnc)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",\"ffffff\"");
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",\"%x%x%x",
                                           (0x0f00 & pstEfloiinfoQryCnf->stEflociInfo.stPlmnId.Mcc) >> 8,
                                           (0x00f0 & pstEfloiinfoQryCnf->stEflociInfo.stPlmnId.Mcc) >> 4,
                                           (0x000f & pstEfloiinfoQryCnf->stEflociInfo.stPlmnId.Mcc));

        if (0x0F != ((0x0f00 & pstEfloiinfoQryCnf->stEflociInfo.stPlmnId.Mnc) >> 8))
        {
            usLength +=(VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%x",
                                          (0x0f00 & pstEfloiinfoQryCnf->stEflociInfo.stPlmnId.Mnc) >> 8);
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%x%x\"",
                                           (0x00f0 & pstEfloiinfoQryCnf->stEflociInfo.stPlmnId.Mnc) >> 4,
                                           (0x000f & pstEfloiinfoQryCnf->stEflociInfo.stPlmnId.Mnc));
    }
    /* <lac> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                   ",0x%04x",
                                   pstEfloiinfoQryCnf->stEflociInfo.usLac);

    /* <location_update_status> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                   ",%d",
                                   pstEfloiinfoQryCnf->stEflociInfo.ucLocationUpdateStatus);

    /* <rfu> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                   ",0x%02x",
                                   pstEfloiinfoQryCnf->stEflociInfo.ucRfu);

    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaPsEflociInfoSetCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_EFPSLOCIINFO_SET_CNF_STRU  *pstPsEflociInfoSetCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    pstPsEflociInfoSetCnf = (TAF_MMA_EFPSLOCIINFO_SET_CNF_STRU *)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPsEflociInfoSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPsEflociInfoSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPsEflociInfoSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_EFPSLOCIINFO_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaPsEflociInfoSetCnf : Current Option is not AT_CMD_EFPSLOCIINFO_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (TAF_MMA_APP_OPER_RESULT_SUCCESS == pstPsEflociInfoSetCnf->enRslt)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaPsEflociInfoQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_EFPSLOCIINFO_QRY_CNF_STRU  *pstPsefloiinfoQryCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;

    pstPsefloiinfoQryCnf = (TAF_MMA_EFPSLOCIINFO_QRY_CNF_STRU *)pMsg;
    usLength = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPsefloiinfoQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPsEflociInfoQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPsEflociInfoQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_EFPSLOCIINFO_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaPsEflociInfoQryCnf : Current Option is not AT_CMD_EFPSLOCIINFO_QRY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 查询失败直接返回ERROR */
    if (TAF_MMA_APP_OPER_RESULT_SUCCESS != pstPsefloiinfoQryCnf->enRslt)
    {
        ulResult = AT_ERROR;

        At_FormatResultData(ucIndex, ulResult);
        return VOS_OK;
    }

    /* <ptmsi> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 "%s: 0x%08x",
                                 g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                 pstPsefloiinfoQryCnf->stPsEflociInfo.ulPTmsi);

    /* <ptmsi_signature> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                 (VOS_CHAR *)pgucAtSndCodeAddr,
                                 (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                 ",0x%6x",
                                 pstPsefloiinfoQryCnf->stPsEflociInfo.ulPTmsiSignature);

    /* <plmn> */
    if (0xfff == pstPsefloiinfoQryCnf->stPsEflociInfo.stPlmnId.Mcc
     && 0xfff == pstPsefloiinfoQryCnf->stPsEflociInfo.stPlmnId.Mnc)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       ",\"ffffff\"");
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           ",\"%x%x%x",
                                           (0x0f00 & pstPsefloiinfoQryCnf->stPsEflociInfo.stPlmnId.Mcc) >> 8,
                                           (0x00f0 & pstPsefloiinfoQryCnf->stPsEflociInfo.stPlmnId.Mcc) >> 4,
                                           (0x000f & pstPsefloiinfoQryCnf->stPsEflociInfo.stPlmnId.Mcc));

        if(0x0F != ((0x0f00 & pstPsefloiinfoQryCnf->stPsEflociInfo.stPlmnId.Mnc) >> 8))
        {
            usLength +=(VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                          "%x",
                                          (0x0f00 & pstPsefloiinfoQryCnf->stPsEflociInfo.stPlmnId.Mnc) >> 8);
        }

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%x%x\"",
                                           (0x00f0 & pstPsefloiinfoQryCnf->stPsEflociInfo.stPlmnId.Mnc) >> 4,
                                           (0x000f & pstPsefloiinfoQryCnf->stPsEflociInfo.stPlmnId.Mnc));
    }
    /* <lac> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                   ",0x%04x",
                                   pstPsefloiinfoQryCnf->stPsEflociInfo.usLac);

    /* <rac> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                   ",0x%02x",
                                   pstPsefloiinfoQryCnf->stPsEflociInfo.ucRac);

    /* <location_update_status> */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                   ",%d",
                                   pstPsefloiinfoQryCnf->stPsEflociInfo.ucPsLocationUpdateStatus);


    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_PROBE_FREQLOCK)

VOS_UINT32 AT_RcvMtaSetM2MFreqLockCnf(
    VOS_VOID                            *pmsg
)
{
    AT_MTA_MSG_STRU                     *pstRcvMsg;
    MTA_AT_SET_M2M_FREQLOCK_CNF_STRU    *pstSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg       = (AT_MTA_MSG_STRU *)pmsg;
    pstSetCnf       = (MTA_AT_SET_M2M_FREQLOCK_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex         = 0;
    ulResult        = AT_ERROR;

    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId,&ucIndex))
    {
        return AT_ERROR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        return AT_ERROR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_M2M_FREQLOCK_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return AT_ERROR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if(MTA_AT_RESULT_NO_ERROR == pstSetCnf->ulResult)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaQryM2MFreqLockCnf(
    VOS_VOID                            *pmsg
)
{
    AT_MTA_MSG_STRU                        *pstRcvMsg           = VOS_NULL_PTR;
    MTA_AT_QRY_M2M_FREQLOCK_CNF_STRU       *pstQryFreqLockCnf   = VOS_NULL_PTR;
    VOS_UINT32                              ulResult;
    VOS_UINT8                               ucIndex;
    VOS_UINT16                              usLength;

    /* 初始化 */
    pstRcvMsg           = (AT_MTA_MSG_STRU *)pmsg;
    pstQryFreqLockCnf   = (MTA_AT_QRY_M2M_FREQLOCK_CNF_STRU *)pstRcvMsg->aucContent;
    ulResult            = AT_OK;
    ucIndex             = 0;
    usLength            = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryM2MFreqLockCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryM2MFreqLockCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_M2M_FREQLOCK_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaQryFreqLockCnf : Current Option is not AT_CMD_M2M_FREQLOCK_QRY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化AT^FREQLOCK查询命令返回 */
    gstAtSendData.usBufLen = 0;

    /* 锁频查询结果按G-W-T-L顺序依次输出  */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                        (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    /* 输出GSM制式锁频状态信息 */
    if (AT_MTA_M2M_FREQLOCK_FLAG_TYPE_OFF == pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_GSM - 1])
    {
        /* <enable>,<mode> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\"",
                                            pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_GSM - 1],
                                            g_astFreqLockModeTbl[AT_FREQLOCK_MODE_TYPE_GSM - 1].acStrMode);
    }
    else
    {
        /* <enable>,<freq>,<mode>,<band>,, */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\",%d,\"%s\",,",
                                            pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_GSM - 1],
                                            g_astFreqLockModeTbl[AT_FREQLOCK_MODE_TYPE_GSM - 1].acStrMode,
                                            pstQryFreqLockCnf->stGFreqLockInfo.usFreq,
                                            g_astFreqLockGsmBandTbl[pstQryFreqLockCnf->stGFreqLockInfo.enBand].acStrGsmBand);
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);

    /* 输出WCDMA制式锁频状态信息 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                        (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    if (AT_MTA_M2M_FREQLOCK_FLAG_TYPE_OFF == pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_WCDMA - 1])
    {
        /* <enable>,<mode> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\"",
                                            pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_WCDMA - 1],
                                            g_astFreqLockModeTbl[AT_FREQLOCK_MODE_TYPE_WCDMA - 1].acStrMode);
    }
    else
    {
        /* 判断WCDMA锁频的类型:区分锁频、锁频+扰码组合 */
        if (AT_MTA_M2M_FREQLOCK_TYPE_ENUM_FREQ_ONLY == pstQryFreqLockCnf->stWFreqLockInfo.ucFreqType)
        {
            /* <enable>,<mode>,<freq>,,, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\",%d,,,",
                                                pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_WCDMA - 1],
                                                g_astFreqLockModeTbl[AT_FREQLOCK_MODE_TYPE_WCDMA - 1].acStrMode,
                                                pstQryFreqLockCnf->stWFreqLockInfo.usFreq);
        }
        else
        {
            /* <enable>,<mode>,<freq>,,<psc>, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\",%d,,%d,",
                                                pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_WCDMA - 1],
                                                g_astFreqLockModeTbl[AT_FREQLOCK_MODE_TYPE_WCDMA - 1].acStrMode,
                                                pstQryFreqLockCnf->stWFreqLockInfo.usFreq,
                                                pstQryFreqLockCnf->stWFreqLockInfo.usPsc);
        }
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);

    /* 输出TDS-CDMA制式锁频状态信息 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                        (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    if (AT_MTA_M2M_FREQLOCK_FLAG_TYPE_OFF == pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_TDSCDMA - 1])
    {
        /* <enable>,<mode> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\"",
                                            pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_TDSCDMA - 1],
                                            g_astFreqLockModeTbl[AT_FREQLOCK_MODE_TYPE_TDSCDMA - 1].acStrMode);
    }
    else
    {
        /* 判断WCDMA锁频的类型:区分锁频、锁频+扰码组合 */
        if (AT_MTA_M2M_FREQLOCK_TYPE_ENUM_FREQ_ONLY == pstQryFreqLockCnf->stTFreqLockInfo.ucFreqType)
        {
            /* <enable>,<mode>,<freq>,,, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\",%d,,,",
                                                pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_TDSCDMA - 1],
                                                g_astFreqLockModeTbl[AT_FREQLOCK_MODE_TYPE_TDSCDMA - 1].acStrMode,
                                                pstQryFreqLockCnf->stTFreqLockInfo.usFreq);
        }
        else
        {
            /* <enable>,<mode>,<freq>,,<psc>, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\",%d,,%d,",
                                                pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_TDSCDMA - 1],
                                                g_astFreqLockModeTbl[AT_FREQLOCK_MODE_TYPE_TDSCDMA - 1].acStrMode,
                                                pstQryFreqLockCnf->stTFreqLockInfo.usFreq,
                                                pstQryFreqLockCnf->stTFreqLockInfo.usSc);
        }
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,(VOS_CHAR*)pgucAtSndCodeAddr,(VOS_CHAR*)pgucAtSndCodeAddr + usLength,"%s",gaucAtCrLf);

    /* 输出LTE制式锁频状态信息 */
    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                        (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%s: ",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    if (AT_MTA_M2M_FREQLOCK_FLAG_TYPE_OFF == pstQryFreqLockCnf->ucFreqState[AT_MTA_M2M_FREQLOCK_MODE_LTE - 1])
    {
        /* <enable>,<mode> */
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\"",
                                            pstQryFreqLockCnf->ucFreqState[AT_MTA_M2M_FREQLOCK_MODE_LTE - 1],
                                            g_astFreqLockModeTbl[AT_MTA_M2M_FREQLOCK_MODE_LTE - 1].acStrMode);
    }
    else
    {
        /* 判断LTE锁频的类型:区分锁频、锁频+扰码组合 */
        if (AT_MTA_M2M_FREQLOCK_TYPE_ENUM_FREQ_ONLY == pstQryFreqLockCnf->stLFreqLockInfo.ucFreqType)
        {
            /* <enable>,<mode>,<freq>,,, */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\",%d,,,",
                                                pstQryFreqLockCnf->ucFreqState[AT_MTA_M2M_FREQLOCK_MODE_LTE - 1],
                                                g_astFreqLockModeTbl[AT_MTA_M2M_FREQLOCK_MODE_LTE - 1].acStrMode,
                                                pstQryFreqLockCnf->stLFreqLockInfo.ulFreq);
        }
        else
        {
            /* <enable>,<mode>,<freq>,,,<pci> */
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                (TAF_CHAR *)pgucAtSndCodeAddr,
                                                (TAF_CHAR *)pgucAtSndCodeAddr + usLength,"%d,\"%s\",%d,,,%d",
                                                pstQryFreqLockCnf->ucFreqState[AT_FREQLOCK_MODE_TYPE_LTE - 1],
                                                g_astFreqLockModeTbl[AT_FREQLOCK_MODE_TYPE_LTE - 1].acStrMode,
                                                pstQryFreqLockCnf->stLFreqLockInfo.ulFreq,
                                                pstQryFreqLockCnf->stLFreqLockInfo.usPci);
        }
    }

    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
#endif


VOS_UINT32 AT_RcvMtaSetXCposrCnf(
    VOS_VOID                                *pMsg
)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                  *pstMtaMsg;
    MTA_AT_RESULT_CNF_STRU           *pstSetXcposrCnf;
    VOS_UINT8                         ucIndex;
    VOS_UINT32                        ulResult;

    /* 初始化消息变量 */
    pstMtaMsg       = (AT_MTA_MSG_STRU*)pMsg;
    pstSetXcposrCnf = (MTA_AT_RESULT_CNF_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaSetXCposrCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetXCposrCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_XCPOSR_SET */
    if (AT_CMD_XCPOSR_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetXCposrCnf: WARNING:Not AT_CMD_XCPOSR_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstSetXcposrCnf->enResult )
    {
        ulResult    = AT_OK;
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;
    /* 调用AT_FormatResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaQryXcposrCnf(
    VOS_VOID                        *pMsg
)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                  *pstMtaMsg;
    MTA_AT_QRY_XCPOSR_CNF_STRU       *pstQryXcposrCnf;
    VOS_UINT8                         ucIndex;
    VOS_UINT32                        ulResult;

    /* 初始化消息变量 */
    pstMtaMsg       = (AT_MTA_MSG_STRU*)pMsg;
    pstQryXcposrCnf = (MTA_AT_QRY_XCPOSR_CNF_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaQryXcposrCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryXcposrCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_XCPOSR_QRY */
    if (AT_CMD_XCPOSR_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaQryXcposrCnf: WARNING:Not AT_CMD_XCPOSR_QRY!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstQryXcposrCnf->enResult )
    {
        ulResult    = AT_OK;
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR*)pgucAtSndCodeAddr,
                                                    (VOS_CHAR*)pgucAtSndCodeAddr,
                                                    "%s: %d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    pstQryXcposrCnf->enXcposrEnableCfg);
    }
    else
    {
        ulResult    = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }

    /* 调用AT_FormatResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaSetXcposrRptCnf(
    VOS_VOID                        *pMsg
)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                  *pstMtaMsg             = VOS_NULL_PTR;
    MTA_AT_RESULT_CNF_STRU           *pstSetXcposrRptCnf    = VOS_NULL_PTR;
    VOS_UINT8                         ucIndex;
    VOS_UINT32                        ulResult;

    /* 初始化消息变量 */
    pstMtaMsg           = (AT_MTA_MSG_STRU*)pMsg;
    pstSetXcposrRptCnf  = (MTA_AT_RESULT_CNF_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaSetXcposrRptCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetXcposrRptCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_XCPOSR_SET */
    if (AT_CMD_XCPOSRRPT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetXcposrRptCnf: WARNING:Not AT_CMD_XCPOSRRPT_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstSetXcposrRptCnf->enResult )
    {
        ulResult    = AT_OK;
    }
    else
    {
        ulResult    = AT_ERROR;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用AT_FormatResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;

}


VOS_UINT32 AT_RcvMtaQryXcposrRptCnf(
    VOS_VOID                        *pMsg
)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                  *pstMtaMsg;
    MTA_AT_QRY_XCPOSRRPT_CNF_STRU    *pstQryXcposrRptCnf;
    VOS_UINT8                         ucIndex;
    VOS_UINT32                        ulResult;

    /* 初始化消息变量 */
    pstMtaMsg           = (AT_MTA_MSG_STRU*)pMsg;
    pstQryXcposrRptCnf  = (MTA_AT_QRY_XCPOSRRPT_CNF_STRU*)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaQryXcposrCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaQryXcposrCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_XCPOSR_QRY */
    if (AT_CMD_XCPOSRRPT_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaQryXcposrCnf: WARNING:Not AT_CMD_XCPOSR_QRY!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstQryXcposrRptCnf->enResult )
    {
        ulResult                = AT_OK;
        gstAtSendData.usBufLen  = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                         (VOS_CHAR*)pgucAtSndCodeAddr,
                                                         (VOS_CHAR*)pgucAtSndCodeAddr,
                                                         "%s: %d",
                                                         g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                         pstQryXcposrRptCnf->ucXcposrRptFlg);
    }
    else
    {
        ulResult                = AT_ERROR;
        gstAtSendData.usBufLen  = 0;
    }

    /* 调用AT_FormatResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvFratIgnitionQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstMtaMsg           = VOS_NULL_PTR;
    MTA_AT_FRAT_IGNITION_QRY_CNF_STRU  *pstIgnitionCnf      = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;

    /* 初始化消息变量 */
    pstMtaMsg       = (AT_MTA_MSG_STRU *)pMsg;
    pstIgnitionCnf  = (MTA_AT_FRAT_IGNITION_QRY_CNF_STRU *)pstMtaMsg->aucContent;
    ucIndex         = 0;
    ulRet           = AT_OK;

     /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvFratIgnitionQryCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvFratIgnitionQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_FRATIGNITION_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvFratIgnitionQryCnf : Current Option is not AT_CMD_FRATIGNITION_QRY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR != pstIgnitionCnf->enResult)
    {
        ulRet = AT_ERROR;
    }
    else
    {
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                                        "%s: %u",
                                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                        pstIgnitionCnf->enFratIgnitionState);
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulRet);

    return VOS_OK;
}


VOS_UINT32 AT_RcvFratIgnitionSetCnf(
    VOS_VOID                           *pMsg
)
{
    /* 定义局部变量 */
    AT_MTA_MSG_STRU                    *pstMtaMsg           = VOS_NULL_PTR;
    MTA_AT_FRAT_IGNITION_SET_CNF_STRU  *pstCnfContent       = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化消息变量 */
    pstMtaMsg       = (AT_MTA_MSG_STRU *)pMsg;
    pstCnfContent   = (MTA_AT_FRAT_IGNITION_SET_CNF_STRU *)pstMtaMsg->aucContent;
    ulResult        = AT_ERROR;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvFratIgnitionSetCnf: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvFratIgnitionSetCnf: AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_FRATIGNITION_SET */
    if (AT_CMD_FRATIGNITION_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvFratIgnitionSetCnf: Not AT_CMD_FRATIGNITION_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断查询操作是否成功 */
    if ( MTA_AT_RESULT_NO_ERROR == pstCnfContent->enResult )
    {
        ulResult    = AT_OK;
    }

    gstAtSendData.usBufLen = 0;

    /* 调用AT_FormatResultDATa发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}




VOS_UINT32 AT_RcvMtaSetSensorCnf(
    VOS_VOID                            *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg;
    MTA_AT_SET_SENSOR_CNF_STRU         *pstSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf    = (MTA_AT_SET_SENSOR_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex      = 0;
    ulResult     = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetSensorCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetSensorCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_SENSOR_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetSensorCnf : Current Option is not AT_CMD_SENSOR_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetCnf->enResult)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaSetScreenCnf(
    VOS_VOID                            *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg;
    MTA_AT_SET_SCREEN_CNF_STRU         *pstSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf    = (MTA_AT_SET_SCREEN_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex      = 0;
    ulResult     = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetScreenCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetScreenCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_SCREEN_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetScreenCnf : Current Option is not AT_CMD_SCREEN_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetCnf->enResult)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}



VOS_UINT32 AT_RcvMtaSetModemTimeCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg;
    MTA_AT_SET_TIME_CNF_STRU           *pstSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf    = (MTA_AT_SET_TIME_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex      = 0;
    ulResult     = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetModemTimeCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetModemTimeCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_MODEM_TIME_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetModemTimeCnf : Current Option is not AT_CMD_MODEM_TIME_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetCnf->enResult)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaAfcClkUnlockCauseInd(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg               = VOS_NULL_PTR;
    MTA_AT_AFC_CLK_UNLOCK_IND_STRU     *pstMtaAtAfcClkUnlockInd = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    /* 初始化 */
    pstRcvMsg               = (AT_MTA_MSG_STRU *)pMsg;
    pstMtaAtAfcClkUnlockInd = (MTA_AT_AFC_CLK_UNLOCK_IND_STRU *)pstRcvMsg->aucContent;
    ucIndex                 = 0;
    usLength                = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaAfcClkUnlockCauseInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 打印^AFCCLKUNLOCK: */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s%d%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_AFCCLKUNLOCK].pucText,
                                       pstMtaAtAfcClkUnlockInd->usCause,
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}


/*lint -save -e838 -specific(-e838)*/
/*lint -save -e826 -specific(-e826)*/
VOS_UINT32 AT_RcvDrvAgentQryCcpuMemInfoCnf(VOS_VOID *pstData)
{
    VOS_UINT8                                               ucIndex              = 0;
    DRV_AGENT_QRY_CCPU_MEM_INFO_CNF_STRU                   *pstQryCcpuMemInfoCnf = VOS_NULL_PTR;
    DRV_AGENT_MSG_STRU                                     *pRcvMsg              = VOS_NULL_PTR;

    pRcvMsg              = (DRV_AGENT_MSG_STRU *)pstData;
    pstQryCcpuMemInfoCnf = (DRV_AGENT_QRY_CCPU_MEM_INFO_CNF_STRU *)pRcvMsg->aucContent;

    /* 根据ClientID获取通道索引 */
    if(AT_FAILURE == At_ClientIdToUserId(pstQryCcpuMemInfoCnf->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryCcpuMemInfoCnf: Get Index Fail!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryCcpuMemInfoCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* AT模块在等待^CCPUMEMINFO命令的操作结果事件上报 */
    if (AT_CMD_CCPUMEMINFO_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvDrvAgentQryCcpuMemInfoCnf: Error Option!");
        return VOS_ERR;
    }

    /* 使用AT_STOP_TIMER_CMD_READY恢复AT命令实体状态为READY状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;
    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: %d,%d,%d",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    pstQryCcpuMemInfoCnf->ulChkPoolLeakRslt,
                                                    pstQryCcpuMemInfoCnf->ulChkNodeLeakRslt,
                                                    pstQryCcpuMemInfoCnf->ulChkRrmNodeLeakRslt);

    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}
/*lint -restore*/
/*lint -restore*/


VOS_UINT32 AT_RcvMtaSetBestFreqCnf(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg = VOS_NULL_PTR;
    MTA_AT_RESULT_CNF_STRU             *pstSetCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;

    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf    = (MTA_AT_RESULT_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex      = 0;
    ulRslt       = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetBestFreqCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSetBestFreqCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_BESTFREQ_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaSetBestFreqCnf : Current Option is not AT_CMD_RXTESTMODE_SET.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetCnf->enResult)
    {
        ulRslt = AT_OK;
    }

    At_FormatResultData(ucIndex, ulRslt);
    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaBestFreqInfoInd(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstMtaMsg          = VOS_NULL_PTR;
    MTA_AT_BEST_FREQ_CASE_IND_STRU     *pstBestFreqCaseInd = VOS_NULL_PTR;
    VOS_UINT32                          ulLoop;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    ucIndex            = 0;
    usLength           = 0;
    pstMtaMsg          = (AT_MTA_MSG_STRU *)pMsg;
    pstBestFreqCaseInd = (MTA_AT_BEST_FREQ_CASE_IND_STRU *)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaBestFreqInfoInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    for (ulLoop = 0; ulLoop < pstBestFreqCaseInd->ucRptDeviceNum; ulLoop++)
    {
        usLength = 0;

        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                           "%s^BESTFREQ: %d,%d,%d%s",
                                           gaucAtCrLf,
                                           pstBestFreqCaseInd->astDeviceFreqList[ulLoop].ucGroupID,
                                           pstBestFreqCaseInd->astDeviceFreqList[ulLoop].ucDeviceID,
                                           pstBestFreqCaseInd->astDeviceFreqList[ulLoop].ucCaseID,
                                           gaucAtCrLf);

       gstAtSendData.usBufLen = usLength;

       At_SendResultData(ucIndex, pgucAtSndCodeAddr, gstAtSendData.usBufLen);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaQryBestFreqCnf(
    VOS_VOID                           *pMsg
)

{
    AT_MTA_MSG_STRU                    *pstMtaMsg      = VOS_NULL_PTR;
    MTA_AT_BESTFREQ_QRY_CNF_STRU       *pstBestFreqCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulLoop;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* 初始化消息变量 */
    ucIndex        = 0;
    usLength       = 0;
    pstMtaMsg      = (AT_MTA_MSG_STRU *)pMsg;
    pstBestFreqCnf = (MTA_AT_BESTFREQ_QRY_CNF_STRU *)pstMtaMsg->aucContent;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstMtaMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaBestFreqInfoCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaBestFreqInfoCnf: WARNING:AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_BESTFREQ_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MTA_AT_RESULT_NO_ERROR != pstBestFreqCnf->enResult)
    {
        ulResult = AT_ERROR;
    }
    else
    {
        ulResult = AT_OK;

        for (ulLoop = 0; ulLoop < pstBestFreqCnf->ucActiveDeviceNum; ulLoop++)
        {
            if (0 != ulLoop)
            {
                usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s",
                                                   gaucAtCrLf);
            }

            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                               "%s: %d,%d,%d,%d",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                               pstBestFreqCnf->astDeviceInfoList[ulLoop].ucGroupID,
                                               pstBestFreqCnf->astDeviceInfoList[ulLoop].ucDeviceID,
                                               pstBestFreqCnf->astDeviceInfoList[ulLoop].ucCaseID,
                                               pstBestFreqCnf->astDeviceInfoList[ulLoop].ucMode);
        }
    }

    gstAtSendData.usBufLen = usLength;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}

/*lint -e838 */

VOS_UINT32 AT_RcvMtaModemCapUpdateCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg = VOS_NULL_PTR;
    MTA_AT_MODEM_CAP_UPDATE_CNF_STRU   *pstSetCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* 初始化 */
    pstRcvMsg    = (AT_MTA_MSG_STRU *)pMsg;
    pstSetCnf    = (MTA_AT_MODEM_CAP_UPDATE_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex      = 0;
    ulResult     = AT_ERROR;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaModemCapUpdateCnf : WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaModemCapUpdateCnf : AT_BROADCAST_INDEX.");

        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_MODEM_CAP_UPDATE_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaModemCapUpdateCnf : Current Option is not AT_CMD_MODEM_CAP_UPDATE_SET.");

        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 格式化命令返回 */
    gstAtSendData.usBufLen = 0;

    if (MTA_AT_RESULT_NO_ERROR == pstSetCnf->enRslt)
    {
        ulResult = AT_OK;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
/*lint +e838 */

/*lint -e838 */

VOS_UINT32 AT_RcvMmaPacspQryCnf(
    VOS_VOID                           *pMsg
)
{
    TAF_MMA_PACSP_QRY_CNF_STRU         *pstPacspQryCnf = VOS_NULL_PTR;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    pstPacspQryCnf = (TAF_MMA_PACSP_QRY_CNF_STRU *)pMsg;
    usLength = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstPacspQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPacspQryCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaDplmnQryCnf: Get modem id fail.");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaPacspQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_PACSP_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaPacspQryCnf : Current Option is not AT_CMD_PACSP_QRY.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                     "+PACSP%d",pstPacspQryCnf->ucPlmnMode);

    gstAtSendData.usBufLen = usLength;

    /* 输出结果 */
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}
/*lint +e838 */

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 AT_RcvMtaLteCategoryInfoInd(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_LTE_CATEGORY_INFO_IND_STRU  *pstLteCategoryInfoInd;

    pstMtaMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstLteCategoryInfoInd = (MTA_AT_LTE_CATEGORY_INFO_IND_STRU *)pstMtaMsg->aucContent;

    if ((pstLteCategoryInfoInd->ucDlCategery >= AT_UE_LTE_CATEGORY_NUM_MAX)
     || (pstLteCategoryInfoInd->ucUlCategery >= AT_UE_LTE_CATEGORY_NUM_MAX))
    {
        AT_WARN_LOG2("AT_RcvMtaLteCategoryInfoInd: WARNING: illegal Category value <DlCategery, UlCategery>!",pstLteCategoryInfoInd->ucDlCategery,pstLteCategoryInfoInd->ucUlCategery);
        return VOS_ERR;
    }
    else
    {
        g_stAtDlRateCategory.ucLteUeDlCategory = pstLteCategoryInfoInd->ucDlCategery;
        g_stAtDlRateCategory.ucLteUeUlCategory = pstLteCategoryInfoInd->ucUlCategery;
    }

    return VOS_OK;
}
#endif


VOS_UINT32 AT_RcvMtaPhyCommAckInd(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg               = VOS_NULL_PTR;
    MTA_AT_PHY_COMM_ACK_IND_STRU       *pstMtaAtCommAckInd      = VOS_NULL_PTR;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* 初始化 */
    pstRcvMsg               = (AT_MTA_MSG_STRU *)pMsg; /*lint !e838 */
    pstMtaAtCommAckInd      = (MTA_AT_PHY_COMM_ACK_IND_STRU *)pstRcvMsg->aucContent; /*lint !e838 */
    ucIndex                 = 0;
    usLength                = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaPhyCommAckInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 打印^PHYCOMACK: */
    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s%d,%d,%d,%d%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_PHYCOMACK].pucText,
                                       pstMtaAtCommAckInd->ulAckType,
                                       pstMtaAtCommAckInd->ulAckValue1,
                                       pstMtaAtCommAckInd->ulAckValue2,
                                       pstMtaAtCommAckInd->ulAckValue3,
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}

#if ( FEATURE_ON == FEATURE_DCXO_HI1102_SAMPLE_SHARE )

VOS_UINT32 AT_RcvMtaRcmDcxoSampleQryCnf(
    VOS_VOID                           *pMsg
)
{
    AT_MTA_MSG_STRU                    *pstRcvMsg;
    MTA_AT_DCXO_SAMPE_QRY_CNF_STRU     *pstMtaAtDcxoSampleQryCnf;
    VOS_UINT16                          usDataLen;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          i;

    /* 初始化 */
    pstRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstMtaAtDcxoSampleQryCnf  = (MTA_AT_DCXO_SAMPE_QRY_CNF_STRU *)pstRcvMsg->aucContent;
    ucIndex                   = 0;
    usDataLen                 = 0;
    i                         = 0;
    ulResult                  = AT_OK;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstRcvMsg->stAppCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMtaRcnDcxoSampleQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaRcnDcxoSampleQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_DCXOQRY_QRY */
    if (AT_CMD_DCXOQRY_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMtaRcnDcxoSampleQryCnf: NOT CURRENT CMD OPTION!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 打印^PHYCOMACK: */
    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                        (TAF_CHAR*)pgucAtSndCodeAddr,
                                        "%s: ",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                        (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                        "%d,",
                                        pstMtaAtDcxoSampleQryCnf->enDcxoSampleType);

    if ( MTA_AT_DCXO_SAMPLE_RESULT_NO_ERROR == pstMtaAtDcxoSampleQryCnf->enResult )
    {
        switch ( pstMtaAtDcxoSampleQryCnf->enDcxoSampleType )
        {
            case AT_MTA_DCXO_SAMPLE_H:
            case AT_MTA_DCXO_SAMPLE_M:
            case AT_MTA_DCXO_SAMPLE_L:
                usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                                    "%d,",
                                                    pstMtaAtDcxoSampleQryCnf->enResult);
                for ( i = 0;i < UCOM_NV_DCXO_SAMPLE_H_MAX_NUM;i++ )
                {
                    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                                        "%d,",
                                                        pstMtaAtDcxoSampleQryCnf->stSctionData.astSample[i].swTime);

                    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                                        "%d,",
                                                        pstMtaAtDcxoSampleQryCnf->stSctionData.astSample[i].swMode);

                    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                                        "%d,",
                                                        pstMtaAtDcxoSampleQryCnf->stSctionData.astSample[i].swTemp);
                    if ((UCOM_NV_DCXO_SAMPLE_H_MAX_NUM -1) == i)
                    {
                        usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                                            (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                                            "%d",
                                                            pstMtaAtDcxoSampleQryCnf->stSctionData.astSample[i].swPpm);
                    }
                    else
                    {
                        usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                                            (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                                            "%d,",
                                                            pstMtaAtDcxoSampleQryCnf->stSctionData.astSample[i].swPpm);
                    }

                }
                break;
            case AT_MTA_DCXO_SAMPLE_INITFREQ:
                    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                                        "%d,",
                                                        pstMtaAtDcxoSampleQryCnf->enResult);
                    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                                        "%d,",
                                                        pstMtaAtDcxoSampleQryCnf->stSctionData.stDcxoInitPpm.swInitFrequency);

                    usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr,
                                                        (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                                        "%d",
                                                        pstMtaAtDcxoSampleQryCnf->stSctionData.stDcxoInitPpm.swInitTemperature);
                break;
            default:
                pstMtaAtDcxoSampleQryCnf->enResult = MTA_AT_DCXO_SAMPLE_RESULT_ERROR;
                usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                                    "%d",
                                                    pstMtaAtDcxoSampleQryCnf->enResult);
                break;
        }
    }
    else
    {
        usDataLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                            (TAF_CHAR *)pgucAtSndCodeAddr,
                                            (TAF_CHAR *)pgucAtSndCodeAddr + usDataLen,
                                            "%d",
                                            pstMtaAtDcxoSampleQryCnf->enResult);
    }

    gstAtSendData.usBufLen = usDataLen;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}
#endif


VOS_UINT32 AT_RcvMmaMtReattachInd(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_MTREATTACH_IND_STRU        *pstMtReattachInd;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    /* 初始化消息变量 */
    ucIndex             = 0;
    usLength            = 0;
    pstMtReattachInd    = (TAF_MMA_MTREATTACH_IND_STRU *)pstMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstMtReattachInd->usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaMtReattachInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaMtReattachInd: Get modem id fail.");
        return VOS_ERR;
    }

    usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (TAF_CHAR *)pgucAtSndCodeAddr,
                                       (TAF_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_MTREATTACH].pucText,
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCemodeSetCnf(VOS_VOID *pstMsg)
{
    TAF_MMA_CEMODE_SET_CNF_STRU        *pstCemodeSetCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化消息变量 */
    ucIndex                             = 0;
    pstCemodeSetCnf                     = (TAF_MMA_CEMODE_SET_CNF_STRU*)pstMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstCemodeSetCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCemodeSetCnf : WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCemodeSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }
    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CEMODE_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCemodeSetCnf : Current Option is not AT_CMD_CEMODE_SET.");
        return VOS_ERR;
    }
    AT_STOP_TIMER_CMD_READY(ucIndex);

    gstAtSendData.usBufLen = 0;

    if (TAF_ERR_NO_ERROR == pstCemodeSetCnf->enResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = At_ChgTafErrorCode(ucIndex, pstCemodeSetCnf->enResult);
    }

    /* 输出结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaCemodeQryCnf(VOS_VOID  *pMsg)
{
    TAF_MMA_CEMODE_QRY_CNF_STRU        *pstCemodeQryCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    /* 初始化消息变量 */
    ucIndex                             = 0;
    ulResult                            = 0;
    pstCemodeQryCnf                     = (TAF_MMA_CEMODE_QRY_CNF_STRU *)pMsg;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstCemodeQryCnf->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("AT_RcvMmaCemodeQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaCemodeQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CEMODE_READ != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvMmaCemodeQryCnf : Current Option is not AT_CMD_CEMODE_READ.");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (TAF_ERR_NO_ERROR == pstCemodeQryCnf->enResult)
    {
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s%s: %d%s",
                                                    gaucAtCrLf,
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                    pstCemodeQryCnf->enCeMode,
                                                    gaucAtCrLf);
        ulResult               = AT_OK;
    }
    else
    {
        ulResult               = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }

    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}




VOS_UINT32  AT_RcvMtaPseudBtsSetCnf (VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                    *pRcvMsg;
    MTA_AT_PSEUD_BTS_SET_CNF_STRU      *pstSetPseudBtsCnf;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    usLength            = 0;
    pRcvMsg             = (AT_MTA_MSG_STRU *)pMsg;
    pstSetPseudBtsCnf   = (MTA_AT_PSEUD_BTS_SET_CNF_STRU *)(pRcvMsg->aucContent);

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaPseudBtsSetCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 判断是否为广播 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaPseudBtsSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_PSEUDBTS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MTA_AT_RESULT_NO_ERROR == pstSetPseudBtsCnf->enResult)
    {
        if (AT_PSEUD_BTS_PARAM_TIMES == pstSetPseudBtsCnf->ucPseudBtsIdentType)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               "%s: %d",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                               pstSetPseudBtsCnf->u.ulPseudBtsIdentTimes);
        }
        else
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               "%s: %d",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                               pstSetPseudBtsCnf->u.ucPseudBtsIdentCap);
        }

        gstAtSendData.usBufLen  = usLength;
        ulResult                = AT_OK;
    }
    else if (MTA_AT_RESULT_INCORRECT_PARAMETERS == pstSetPseudBtsCnf->enResult)
    {
        gstAtSendData.usBufLen  = 0;
        ulResult                = AT_CME_INCORRECT_PARAMETERS;
    }

    else if (MTA_AT_RESULT_FUNC_DISABLE == pstSetPseudBtsCnf->enResult)
    {
        gstAtSendData.usBufLen  = 0;
        ulResult                = AT_CME_FUNC_DISABLE;
    }
    else
    {
        gstAtSendData.usBufLen  = 0;
        ulResult                = AT_ERROR;
    }

    /* 发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}



VOS_UINT32 AT_RcvMtaSubClfSparamSetCnf (VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                         *pRcvMsg;
    MTA_AT_SUBCLFSPARAM_SET_CNF_STRU        *pstSetSubClfSparamCnf;
    VOS_UINT8                                ucIndex;
    VOS_UINT32                               ulResult;

    pRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstSetSubClfSparamCnf   = (MTA_AT_SUBCLFSPARAM_SET_CNF_STRU *)(pRcvMsg->aucContent);

     /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSubClfSparamSetCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 判断是否为广播 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSubClfSparamSetCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_SUBCLFSPARAM_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MTA_AT_RESULT_NO_ERROR == pstSetSubClfSparamCnf->enResult)
    {
        ulResult = AT_OK;
    }
    else
    {
        ulResult = AT_ERROR;
    }
    gstAtSendData.usBufLen = 0;

    /* 发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMtaSubClfSparamQryCnf (VOS_VOID *pMsg)
{
    MTA_AT_SUBCLFSPARAM_QRY_CNF_STRU   *pstSubClfSparamQryCnf;
    AT_MTA_MSG_STRU                    *pRcvMsg;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    usLength                = 0;
    pRcvMsg                 = (AT_MTA_MSG_STRU *)pMsg;
    pstSubClfSparamQryCnf   = (MTA_AT_SUBCLFSPARAM_QRY_CNF_STRU *)(pRcvMsg->aucContent);

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pRcvMsg->stAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSubClfSparamQryCnf:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 判断是否为广播 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaSubClfSparamQryCnf : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_SUBCLFSPARAM_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MTA_AT_RESULT_NO_ERROR == pstSubClfSparamQryCnf->enResult)
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s: %s",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           pstSubClfSparamQryCnf->aucVersionId);

        gstAtSendData.usBufLen  = usLength;
        ulResult                = AT_OK;
    }
    else if (MTA_AT_RESULT_INCORRECT_PARAMETERS == pstSubClfSparamQryCnf->enResult)
    {
        gstAtSendData.usBufLen  = 0;
        ulResult                = AT_CME_INCORRECT_PARAMETERS;
    }
    else if (MTA_AT_RESULT_FUNC_DISABLE == pstSubClfSparamQryCnf->enResult)
    {
        gstAtSendData.usBufLen  = 0;
        ulResult                = AT_CME_FUNC_DISABLE;
    }
    else
    {
        gstAtSendData.usBufLen  = 0;
        ulResult                = AT_ERROR;
    }

    /* 发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvMmaRejinfoQryCnf(
    VOS_VOID                           *pstMsg
)
{
    TAF_MMA_REJINFO_QRY_CNF_STRU       *pstRejinfoQryCnf;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* 初始化消息变量 */
    ucIndex             = 0;
    usLength            = 0;
    pstRejinfoQryCnf    = (TAF_MMA_REJINFO_QRY_CNF_STRU *)pstMsg;

    /* 通过ClientId获取ucIndex */
    if (AT_FAILURE == At_ClientIdToUserId(pstRejinfoQryCnf->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaRejinfoQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMmaRejinfoQryCnf: AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_REJINFO_QRY */
    if (AT_CMD_REJINFO_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 停止保护定时器 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: ",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    /*打印rejinfo AT命令参数*/
    usLength = AT_PrintRejinfo(usLength, &(pstRejinfoQryCnf->stPhoneRejInfo));

    gstAtSendData.usBufLen = usLength;

    At_FormatResultData(ucIndex, AT_OK);
    return VOS_OK;
}


VOS_UINT16 AT_PrintRejinfo(VOS_UINT16 usLength,TAF_PH_REG_REJ_INFO_STRU* pstRejinfo)
{

    VOS_UINT16                          usLen;

    usLen = usLength;

    usLen += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                    (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                    "%x%x%x",
                                    (pstRejinfo->stPlmnId.Mcc & 0x0f00) >> 8,
                                    (pstRejinfo->stPlmnId.Mcc & 0xf0) >> 4,
                                    (pstRejinfo->stPlmnId.Mcc & 0x0f));

    if (0x0f00 == (pstRejinfo->stPlmnId.Mnc & 0x0f00))
    {
        usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                        "%x%x,",
                                        (pstRejinfo->stPlmnId.Mnc & 0xf0) >> 4,
                                        (pstRejinfo->stPlmnId.Mnc & 0x0f));
    }
    else
    {
        usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                        "%x%x%x,",
                                        (pstRejinfo->stPlmnId.Mnc & 0x0f00) >> 8,
                                        (pstRejinfo->stPlmnId.Mnc & 0xf0) >> 4,
                                        (pstRejinfo->stPlmnId.Mnc & 0x0f));
    }

    usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                    (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                    "%d,%d,%d,%d,%d,",
                                    pstRejinfo->enSrvDomain,
                                    pstRejinfo->ulRejCause,
                                    pstRejinfo->enRat,
                                    pstRejinfo->ucRejType,
                                    pstRejinfo->ucOriginalRejCause);

    /* Lac */
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    if (TAF_MMA_REJ_RAT_NR == pstRejinfo->enRat)
    {
        usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                        "\"%X%X%X%X%X%X\",",
                                        0x000f & (pstRejinfo->ulLac >> 20),
                                        0x000f & (pstRejinfo->ulLac >> 16),
                                        0x000f & (pstRejinfo->ulLac >> 12),
                                        0x000f & (pstRejinfo->ulLac >> 8),
                                        0x000f & (pstRejinfo->ulLac >> 4),
                                        0x000f & (pstRejinfo->ulLac >> 0));
    }
    else
#endif
    {
    usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                    (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                    "\"%X%X%X%X\",",
                                    0x000f & (pstRejinfo->ulLac >> 12),
                                    0x000f & (pstRejinfo->ulLac >> 8),
                                    0x000f & (pstRejinfo->ulLac >> 4),
                                    0x000f & (pstRejinfo->ulLac >> 0));
    }

    /* Rac */
    usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                    (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                    "\"%X%X\",",
                                    0x000f & (pstRejinfo->ucRac >> 4),
                                    0x000f & (pstRejinfo->ucRac >> 0));

    /* <CI>域以4字节方式上报 */
    usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                    (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                    "\"%X%X%X%X%X%X%X%X\"",
                                    0x000f & (pstRejinfo->ulCellId >> 28),
                                    0x000f & (pstRejinfo->ulCellId >> 24),
                                    0x000f & (pstRejinfo->ulCellId >> 20),
                                    0x000f & (pstRejinfo->ulCellId >> 16),
                                    0x000f & (pstRejinfo->ulCellId >> 12),
                                    0x000f & (pstRejinfo->ulCellId >> 8),
                                    0x000f & (pstRejinfo->ulCellId >> 4),
                                    0x000f & (pstRejinfo->ulCellId >> 0));

    /* ESM cause */
    if (0 != pstRejinfo->ucPdnRejCause)
    {
        usLen += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,
                                        (VOS_CHAR *)pgucAtSndCodeAddr + usLen,
                                        ",%d",
                                        pstRejinfo->ucPdnRejCause);
    }

    return usLen;
}



VOS_UINT32 AT_RcvMmaElevatorStateInd(VOS_VOID  *pMsg)
{
    TAF_MMA_ELEVATOR_STATE_IND_STRU    *pstElevatorState;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    /* 初始化消息变量 */
    ucIndex              = 0;
    usLength             = 0;
    pstElevatorState     = (TAF_MMA_ELEVATOR_STATE_IND_STRU*)pMsg;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstElevatorState->usClientId, &ucIndex))
    {
        AT_ERR_LOG("AT_RcvMmaElevatorStateInd: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (VOS_OK != AT_GetModemIdFromClient(ucIndex, &enModemId))
    {
        AT_ERR_LOG("AT_RcvMmaElevatorStateInd: Get modem id fail.");
        return VOS_ERR;
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                   "%s%s: %d,%d,%d%s",
                                   gaucAtCrLf,
                                   gastAtStringTab[AT_STRING_ELEVATOR].pucText,
                                   pstElevatorState->stSensorPara.ulSensorState,
                                   pstElevatorState->stSensorPara.ucServiceState,
                                   pstElevatorState->stSensorPara.enSensorScene,
                                   gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}

VOS_UINT32 AT_RcvTafCcmCSChannelInfoQryCnf(VOS_VOID *pMsg)
{
    TAF_CCM_QRY_CHANNEL_INFO_CNF_STRU  *pstChannelInfo;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucIndex;

    pstChannelInfo  = (TAF_CCM_QRY_CHANNEL_INFO_CNF_STRU *)pMsg;
    ucIndex         = 0;

    /* 根据clientId获取通道索引 */
    if (AT_FAILURE == At_ClientIdToUserId(pstChannelInfo->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafCcmCSChannelInfoQryCnf: Get Index Fail!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafCcmCSChannelInfoQryCnf: WARNING:AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

     /* 当前AT是否在等待该命令返回 */
    if (AT_CMD_CSCHANNELINFO_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);


    if (VOS_OK != pstChannelInfo->stPara.ulResult)
    {
        ulResult = AT_ERROR;
        gstAtSendData.usBufLen = 0;
    }
    else
    {
        ulResult = AT_OK;

        /* 输出查询结果 */
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          (VOS_CHAR *)pgucAtSndCodeAddr,
                                          "%s: %d,%d",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                          pstChannelInfo->stPara.enChannelType,
                                          pstChannelInfo->stPara.enVoiceDomain);
    }

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafCcmChannelInfoInd(VOS_VOID *pMsg)
{
    TAF_CCM_CALL_CHANNEL_INFO_IND_STRU *pstChannelInfoInd;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;
    TAF_CALL_CHANNEL_TYPE_ENUM_UINT8    enChannelType;

    usLength                            = 0;
    ucIndex                             = 0;
    pstChannelInfoInd                   = (TAF_CCM_CALL_CHANNEL_INFO_IND_STRU *)pMsg;

    if ((pstChannelInfoInd->enCodecType == MN_CALL_CODEC_TYPE_BUTT)
     && (VOS_FALSE == pstChannelInfoInd->ucIsLocalAlertingFlag))
    {
        AT_WARN_LOG("AT_RcvTafCcmChannelInfoInd: WARNING: CodecType BUTT!");
        return VOS_ERR;
    }

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstChannelInfoInd->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafCcmChannelInfoInd:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    enChannelType = AT_ConvertCodecTypeToChannelType(pstChannelInfoInd->ucIsLocalAlertingFlag, pstChannelInfoInd->enCodecType);

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s%s %d,%d%s",
                                       gaucAtCrLf,
                                       gastAtStringTab[AT_STRING_CS_CHANNEL_INFO].pucText,
                                       enChannelType,
                                       pstChannelInfoInd->enVoiceDomain,
                                       gaucAtCrLf);

    gstAtSendData.usBufLen = usLength;

    At_SendResultData(ucIndex, pgucAtSndCodeAddr, usLength);

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_IMS)

VOS_UINT32 At_RcvTafCcmCallModifyCnf(VOS_VOID *pMsg)
{
    TAF_CCM_CALL_ANSWER_REMOTE_MODIFY_CNF_STRU             *pstModifyCnf;
    VOS_UINT8                                               ucIndex;

    pstModifyCnf    = (TAF_CCM_CALL_ANSWER_REMOTE_MODIFY_CNF_STRU *)pMsg;
    ucIndex         = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstModifyCnf->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("At_RcvTafCallModifyCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 广播消息不处理 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvTafCallModifyCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_CALL_MODIFY_INIT_SET */
    if (AT_CMD_CALL_MODIFY_INIT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("At_RcvTafCallModifyCnf: WARNING:Not AT_CMD_CALL_MODIFY_INIT_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断操作是否成功 */
    if (TAF_CS_CAUSE_SUCCESS != pstModifyCnf->enCause)
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_OK);
    }

    return VOS_OK;
}


VOS_UINT32 At_RcvTafCcmCallAnswerRemoteModifyCnf(VOS_VOID *pMsg)
{
    TAF_CCM_CALL_ANSWER_REMOTE_MODIFY_CNF_STRU             *pstModifyCnf;
    VOS_UINT8                                               ucIndex;

    pstModifyCnf    = (TAF_CCM_CALL_ANSWER_REMOTE_MODIFY_CNF_STRU *)pMsg;
    ucIndex         = 0;

    /* 通过ClientId获取ucIndex */
    if ( AT_FAILURE == At_ClientIdToUserId(pstModifyCnf->stCtrl.usClientId, &ucIndex) )
    {
        AT_WARN_LOG("At_RcvTafCcmCallAnswerRemoteModifyCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* 广播消息不处理 */
    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("At_RcvTafCcmCallAnswerRemoteModifyCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_CALL_MODIFY_ANS_SET */
    if (AT_CMD_CALL_MODIFY_ANS_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("At_RcvTafCcmCallAnswerRemoteModifyCnf: WARNING:Not AT_CMD_CALL_MODIFY_CNF_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* 判断操作是否成功 */
    if (TAF_CS_CAUSE_SUCCESS != pstModifyCnf->enCause)
    {
        At_FormatResultData(ucIndex, AT_ERROR);
    }
    else
    {
        At_FormatResultData(ucIndex, AT_OK);
    }

    return VOS_OK;
}


VOS_UINT32 AT_RcvTafCcmQryEconfCalledInfoCnf(VOS_VOID *pMsg)
{
    TAF_CCM_QRY_ECONF_CALLED_INFO_CNF_STRU                 *pstEconfInfo;
    VOS_UINT32                                              ulRet;
    VOS_UINT8                                               ucIndex;

    /* 初始化 */
    pstEconfInfo    = (TAF_CCM_QRY_ECONF_CALLED_INFO_CNF_STRU *)pMsg;
    ucIndex         = 0;

    /* 通过clientid获取index */
    if (AT_FAILURE == At_ClientIdToUserId(pstEconfInfo->stCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvTafCcmQryEconfCalledInfoCnf: WARNING: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }


    if (AT_CMD_CLCCECONF_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("AT_RcvTafCcmQryEconfCalledInfoCnf: WARNING: CmdCurrentOpt != AT_CMD_CLCCECONF_QRY!");
        return VOS_ERR;
    }

    ulRet = At_ProcQryClccEconfResult(pstEconfInfo, ucIndex);

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRet);

    return VOS_OK;
}
#endif


