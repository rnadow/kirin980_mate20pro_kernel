


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "oal_profiling.h"
#include "oal_net.h"
#include "frw_ext_if.h"
#include "hmac_tx_data.h"
#include "hmac_tx_amsdu.h"
#include "mac_frame.h"
#include "mac_data.h"
#include "hmac_frag.h"
#include "hmac_11i.h"
#ifdef _PRE_WLAN_FEATURE_MCAST /* 组播转单播 */
#include "hmac_m2u.h"
#endif

#ifdef _PRE_WLAN_FEATURE_PROXY_ARP
#include "hmac_proxy_arp.h"
#endif

#ifdef _PRE_WLAN_FEATURE_WAPI
#include "hmac_wapi.h"
#endif

#include "hmac_traffic_classify.h"

#include "hmac_crypto_tkip.h"
#include "hmac_device.h"
#include "hmac_resource.h"

#include "hmac_tcp_opt.h"

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#include "hisi_customize_wifi.h"
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
#include "hmac_proxysta.h"
#endif
#include "hmac_statistic_data_flow.h"
#include "plat_pm_wlan.h"

#ifdef _PRE_WLAN_FEATURE_SNIFFER
#include <hwnet/ipv4/sysctl_sniffer.h>
#endif
#include "hmac_config.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_TX_DATA_C

/*
 * definitions of king of games feature
 */
#ifdef CONFIG_NF_CONNTRACK_MARK
#define VIP_APP_VIQUE_TID              WLAN_TIDNO_VIDEO
#define VIP_APP_MARK                   0x5a
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
#define PKTMARK(p)                     (((struct sk_buff *)(p))->mark)
#define PKTSETMARK(p, m)               ((struct sk_buff *)(p))->mark = (m)
#else /* !2.6.0 */
#define PKTMARK(p)                     (((struct sk_buff *)(p))->nfmark)
#define PKTSETMARK(p, m)               ((struct sk_buff *)(p))->nfmark = (m)
#endif /* 2.6.0 */
#else /* CONFIG_NF_CONNTRACK_MARK */
#define PKTMARK(p)                     0
#define PKTSETMARK(p, m)
#endif /* CONFIG_NF_CONNTRACK_MARK */

#define PSM_PPS_VALUE_LOW    (50)

#define PPS_LEVEL_IDLE      (0)
#define PPS_LEVEL_BUSY      (1)
#define PSM_PPS_LEVEL_SUCCESSIVE_DOWN_CNT    (3)
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
OAL_STATIC oal_uint8 g_uc_ac_new = 0;
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
static oal_uint16 us_noqos_frag_seqnum= 0; /*保存非qos分片帧seqnum*/
#endif

mac_rx_dyn_bypass_extlna_stru g_st_rx_dyn_bypass_extlna_switch = {0};

mac_small_amsdu_switch_stru g_st_small_amsdu_switch = {0};

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/* 用于测试命令配置 */
/* STA模式下值为OAL_TRUE时建立BA的条件是从发送第一个单播数据帧(关键数据帧不算)开始收到5个帧后建立聚合
   值为OAL_FALSE时在100ms内连续发送5个单播数据帧(关键数据帧不算)后才开始建立聚合*/
oal_uint32  g_ul_tx_ba_policy_select = OAL_TRUE;
#endif

#ifdef WIN32
oal_uint8 wlan_fast_check_cnt;
#endif

/* 用来标记性能统计传来的pps是否和上次传来的属于同一个等级 */
OAL_STATIC oal_uint8 g_uc_pps_level = 0;
/* 用来记录是否连续3个周期会降低到低pps,才启动快睡模式 */
OAL_STATIC oal_uint8 g_uc_pps_level_adjust_cnt = 0;

/* device每20ms检查一次如果检查g_wlan_fast_check_cnt依旧无数据收发则进入低功耗模式 */
extern oal_uint8 wlan_fast_check_cnt;

oal_bool_enum_uint8 g_en_force_pass_filter = OAL_FALSE;
#ifdef _PRE_WLAN_FEATURE_APF
oal_uint16    g_us_apf_program_len = 0;
#endif
/*****************************************************************************
  3 函数实现
*****************************************************************************/
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
oal_uint32 hmac_tx_data(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf);
#endif



OAL_STATIC OAL_INLINE oal_bool_enum_uint8 hmac_tx_is_dhcp(mac_ether_header_stru  *pst_ether_hdr)
{
    mac_ip_header_stru     *puc_ip_hdr;

    puc_ip_hdr = (mac_ip_header_stru *)(pst_ether_hdr + 1);

    return mac_is_dhcp_port(puc_ip_hdr);
}



OAL_STATIC oal_void hmac_tx_report_dhcp_and_arp(
                                            mac_vap_stru  *pst_mac_vap,
                                            mac_ether_header_stru  *pst_ether_hdr,
                                            oal_uint16   us_ether_len)
{
    oal_bool_enum_uint8     en_flg = OAL_FALSE;

    switch (OAL_HOST2NET_SHORT(pst_ether_hdr->us_ether_type))
    {
        case ETHER_TYPE_ARP:
            en_flg = OAL_TRUE;
        break;

        case ETHER_TYPE_IP:
            en_flg = hmac_tx_is_dhcp(pst_ether_hdr);
        break;

        default:
            en_flg = OAL_FALSE;
        break;
    }

    if (en_flg && oam_report_dhcp_arp_get_switch())
    {
        if (WLAN_VAP_MODE_BSS_AP == pst_mac_vap->en_vap_mode)
        {
            oam_report_eth_frame(pst_ether_hdr->auc_ether_dhost, (oal_uint8 *)pst_ether_hdr, us_ether_len, OAM_OTA_FRAME_DIRECTION_TYPE_TX);
        }
        else if (WLAN_VAP_MODE_BSS_STA == pst_mac_vap->en_vap_mode)
        {
            oam_report_eth_frame(pst_mac_vap->auc_bssid, (oal_uint8 *)pst_ether_hdr, us_ether_len, OAM_OTA_FRAME_DIRECTION_TYPE_TX);
        }
        else
        {
        }
    }

}


oal_uint32  hmac_tx_report_eth_frame(mac_vap_stru   *pst_mac_vap,
                                           oal_netbuf_stru *pst_netbuf)
{
    oal_uint16              us_user_idx = 0;
    mac_ether_header_stru  *pst_ether_hdr = OAL_PTR_NULL;
    oal_uint32              ul_ret;
    oal_uint8               auc_user_macaddr[WLAN_MAC_ADDR_LEN] = {0};
    oal_switch_enum_uint8   en_eth_switch = 0;
#ifdef _PRE_WLAN_DFT_STAT
    hmac_vap_stru           *pst_hmac_vap;
#endif

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap) || OAL_UNLIKELY(OAL_PTR_NULL == pst_netbuf))
    {
        OAM_ERROR_LOG2(0, OAM_SF_TX, "{hmac_tx_report_eth_frame::input null %x %x}", (uintptr_t)pst_mac_vap, (uintptr_t)pst_netbuf);
        return OAL_ERR_CODE_PTR_NULL;
    }
#ifdef _PRE_WLAN_DFT_STAT
    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_hmac_vap))
    {
        OAM_ERROR_LOG1(0, OAM_SF_TX, "{hmac_tx_report_eth_frame::mac_res_get_hmac_vap fail. vap_id = %u}", pst_mac_vap->uc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

#endif

    /* 统计以太网下来的数据包统计 */





    /* 获取目的用户资源池id和用户MAC地址，用于过滤 */
    if (WLAN_VAP_MODE_BSS_AP == pst_mac_vap->en_vap_mode)
    {
        pst_ether_hdr = (mac_ether_header_stru *)oal_netbuf_data(pst_netbuf);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_ether_hdr))
        {
            OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_tx_report_eth_frame::ether_hdr is null!\r\n");
            return OAL_ERR_CODE_PTR_NULL;
        }

        ul_ret = mac_vap_find_user_by_macaddr(pst_mac_vap, pst_ether_hdr->auc_ether_dhost, &us_user_idx);
        if (OAL_ERR_CODE_PTR_NULL == ul_ret)
        {
            OAM_ERROR_LOG1(0, OAM_SF_TX, "{hmac_tx_report_eth_frame::find user return ptr null!!\r\n", ul_ret);
            return ul_ret;
        }

        if (OAL_FAIL == ul_ret)
        {
            /* 如果找不到用户，该帧可能是dhcp或者arp request，需要上报 */
            hmac_tx_report_dhcp_and_arp(pst_mac_vap, pst_ether_hdr, (oal_uint16)OAL_NETBUF_LEN(pst_netbuf));
            return OAL_SUCC;
        }

        oal_set_mac_addr(auc_user_macaddr, pst_ether_hdr->auc_ether_dhost);
    }

    if (WLAN_VAP_MODE_BSS_STA == pst_mac_vap->en_vap_mode)
    {
        if (0 == pst_mac_vap->us_user_nums)
        {
            return OAL_SUCC;
        }
#if 1
        pst_ether_hdr = (mac_ether_header_stru *)oal_netbuf_data(pst_netbuf);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_ether_hdr))
        {
            OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_tx_report_eth_frame::ether_hdr is null!\r\n");
            return OAL_ERR_CODE_PTR_NULL;
        }
        /* 如果找不到用户，该帧可能是dhcp或者arp request，需要上报 */
        hmac_tx_report_dhcp_and_arp(pst_mac_vap, pst_ether_hdr, (oal_uint16)OAL_NETBUF_LEN(pst_netbuf));

#endif
        us_user_idx = pst_mac_vap->uc_assoc_vap_id;
        oal_set_mac_addr(auc_user_macaddr, pst_mac_vap->auc_bssid);
    }

    /* 检查打印以太网帧的开关 */
    ul_ret = oam_report_eth_frame_get_switch(us_user_idx, OAM_OTA_FRAME_DIRECTION_TYPE_TX, &en_eth_switch);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG0(0, OAM_SF_TX, "{hmac_tx_report_eth_frame::get tx eth frame switch fail!\r\n");
        return ul_ret;
    }

    if (OAL_SWITCH_ON == en_eth_switch)
    {
        /* 将以太网下来的帧上报 */
        ul_ret = oam_report_eth_frame(auc_user_macaddr,
                             oal_netbuf_data(pst_netbuf),
                             (oal_uint16)OAL_NETBUF_LEN(pst_netbuf),
                             OAM_OTA_FRAME_DIRECTION_TYPE_TX);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_TX, "{hmac_tx_report_eth_frame::oam_report_eth_frame return err: 0x%x.}\r\n", ul_ret);
        }
    }

    return OAL_SUCC;
}


oal_uint16 hmac_free_netbuf_list(oal_netbuf_stru  *pst_buf)
{
    oal_netbuf_stru     *pst_buf_tmp;
    mac_tx_ctl_stru     *pst_tx_cb;
    oal_uint16           us_buf_num = 0;

    if(OAL_PTR_NULL != pst_buf)
    {
        pst_tx_cb = (mac_tx_ctl_stru *)OAL_NETBUF_CB(pst_buf);

        while (OAL_PTR_NULL != pst_buf)
        {
            pst_buf_tmp = oal_netbuf_list_next(pst_buf);
            us_buf_num++;

            pst_tx_cb = (mac_tx_ctl_stru *)OAL_NETBUF_CB(pst_buf);
            
            if ((oal_netbuf_headroom(pst_buf) <  MAC_80211_QOS_HTC_4ADDR_FRAME_LEN) && (OAL_PTR_NULL != pst_tx_cb->pst_frame_header))
            {
                OAL_MEM_FREE(pst_tx_cb->pst_frame_header, OAL_TRUE);
                pst_tx_cb->pst_frame_header = OAL_PTR_NULL;
            }

            oal_netbuf_free(pst_buf);

            pst_buf = pst_buf_tmp;
        }
    }
    else
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_free_netbuf_list::pst_buf is null}");
    }

    return us_buf_num;
}




















































#ifdef _PRE_WLAN_FEATURE_HS20

oal_void hmac_tx_set_qos_map(oal_netbuf_stru *pst_buf, oal_uint8 *puc_tid)
{
    mac_ether_header_stru  *pst_ether_header;
    mac_ip_header_stru     *pst_ip;
    oal_uint8               uc_dscp = 0;
    mac_tx_ctl_stru        *pst_tx_ctl;
    hmac_vap_stru          *pst_hmac_vap     = OAL_PTR_NULL;
    oal_uint8               uc_idx;

    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_buf);
    pst_hmac_vap     = (hmac_vap_stru *)mac_res_get_hmac_vap(MAC_GET_CB_TX_VAP_INDEX(pst_tx_ctl));

    /* 获取以太网头 */
    pst_ether_header = (mac_ether_header_stru *)oal_netbuf_data(pst_buf);

    /* 参数合法性检查 */
    if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_ether_header))
    {
        OAM_ERROR_LOG0(0, OAM_SF_HS20,
                       "{hmac_tx_set_qos_map::The input parameter of QoS_Map_Configure_frame_with_QoSMap_Set_element is OAL_PTR_NULL.}");
        return;
    }

    /* 从IP TOS字段寻找DSCP优先级 */
    /*---------------------------------
      tos位定义
      ---------------------------------
    |    bit7~bit2      | bit1~bit0 |
    |    DSCP优先级     | 保留      |
    ---------------------------------*/

    /* 偏移一个以太网头，取ip头 */
    pst_ip = (mac_ip_header_stru *)(pst_ether_header + 1);
    uc_dscp = pst_ip->uc_tos >> WLAN_DSCP_PRI_SHIFT;
    OAM_INFO_LOG2(0, OAM_SF_HS20, "{hmac_tx_set_qos_map::tos = %d, uc_dscp=%d.}", pst_ip->uc_tos, uc_dscp);

    if ((pst_hmac_vap->st_cfg_qos_map_param.uc_num_dscp_except > 0)
        &&(pst_hmac_vap->st_cfg_qos_map_param.uc_num_dscp_except <= MAX_DSCP_EXCEPT)
        && (pst_hmac_vap->st_cfg_qos_map_param.uc_valid))
    {
        for (uc_idx = 0; uc_idx < pst_hmac_vap->st_cfg_qos_map_param.uc_num_dscp_except; uc_idx++)
        {
            if (uc_dscp == pst_hmac_vap->st_cfg_qos_map_param.auc_dscp_exception[uc_idx])
            {
               *puc_tid  = pst_hmac_vap->st_cfg_qos_map_param.auc_dscp_exception_up[uc_idx];
                pst_tx_ctl->bit_is_vipframe              = OAL_TRUE;
                pst_tx_ctl->bit_is_needretry             = OAL_TRUE;
                pst_hmac_vap->st_cfg_qos_map_param.uc_valid = 0;
                return;
            }
        }
    }

    for (uc_idx = 0; uc_idx < MAX_QOS_UP_RANGE; uc_idx++)
    {
        if ((uc_dscp < pst_hmac_vap->st_cfg_qos_map_param.auc_up_high[uc_idx])
            && (uc_dscp > pst_hmac_vap->st_cfg_qos_map_param.auc_up_low[uc_idx]))
        {
           *puc_tid  = uc_idx;
            pst_tx_ctl->bit_is_vipframe              = OAL_TRUE;
            pst_tx_ctl->bit_is_needretry             = OAL_TRUE;
            pst_hmac_vap->st_cfg_qos_map_param.uc_valid = 0;
            return;
        }
        else
        {
            *puc_tid = 0;
        }
    }
    pst_hmac_vap->st_cfg_qos_map_param.uc_valid = 0;
    return;
}
#endif //_PRE_WLAN_FEATURE_HS20

#ifdef _PRE_WLAN_FEATURE_CLASSIFY

OAL_STATIC oal_void  hmac_tx_classify_tcp(hmac_vap_stru         *pst_hmac_vap,
                                            mac_ip_header_stru  *pst_ip,
                                            mac_tx_ctl_stru     *pst_tx_ctl,
                                            oal_uint8           *puc_tid)
{
    mac_tcp_header_stru *pst_tcp_hdr;

    pst_tcp_hdr = (mac_tcp_header_stru *)(pst_ip + 1);

#ifdef _PRE_WLAN_FEATURE_SCHEDULE
    if ((OAL_NTOH_16(pst_tcp_hdr->us_dport) == MAC_CHARIOT_NETIF_PORT)
            || (OAL_NTOH_16(pst_tcp_hdr->us_sport) == MAC_CHARIOT_NETIF_PORT))
    {
        /* 对于chariot信令报文进行特殊处理，防止断流 */
        OAM_INFO_LOG0(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::chariot netif tcp pkt.}");
        *puc_tid = WLAN_DATA_VIP_TID;
        pst_tx_ctl->bit_is_vipframe  = OAL_TRUE;
        pst_tx_ctl->bit_is_needretry = OAL_TRUE;
    }
    else
#endif /* _PRE_WLAN_FEATURE_SCHEDULE */
    if(OAL_TRUE == oal_netbuf_is_tcp_ack((oal_ip_header_stru *)pst_ip))
    {
        /*option3:SYN FIN RST URG有为1的时候不缓存*/
        if ((pst_tcp_hdr->uc_flags) & (FIN_FLAG_BIT | RESET_FLAG_BIT | URGENT_FLAG_BIT))
        {
            pst_tx_ctl->bit_data_frame_type = MAC_DATA_URGENT_TCP_ACK;
        }
        else if((pst_tcp_hdr->uc_flags) & SYN_FLAG_BIT )
        {
            MAC_GET_CB_IS_NEEDRETRY(pst_tx_ctl)  = OAL_TRUE;
            pst_tx_ctl->bit_data_frame_type = MAC_DATA_TCP_SYN;
        }
        else
        {
            pst_tx_ctl->bit_data_frame_type = MAC_DATA_NORMAL_TCP_ACK;
        }
    }

    else if ((!IS_LEGACY_VAP(&(pst_hmac_vap->st_vap_base_info)))
        && OAL_NTOH_16(pst_tcp_hdr->us_sport) == MAC_WFD_RTSP_PORT)
    {
        OAM_INFO_LOG0(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::tx rtsp.}");
        MAC_GET_CB_FRAME_TYPE(pst_tx_ctl)    = WLAN_DATA_BASICTYPE;
        MAC_GET_CB_IS_NEEDRETRY(pst_tx_ctl)  = OAL_TRUE;
        pst_tx_ctl->bit_data_frame_type      = MAC_DATA_RTSP;
        *puc_tid                             = WLAN_TIDNO_VOICE;
    }

    else
    {
        pst_tx_ctl->bit_data_frame_type = MAC_DATA_UNSPEC;
    }
}


OAL_STATIC OAL_INLINE oal_void  hmac_tx_classify_lan_to_wlan(oal_netbuf_stru *pst_buf, oal_uint8 *puc_tid)
{
    mac_ether_header_stru  *pst_ether_header;
    mac_ip_header_stru     *pst_ip;
    oal_vlan_ethhdr_stru   *pst_vlan_ethhdr;
    oal_uint32              ul_ipv6_hdr;
    oal_uint32              ul_pri;
    oal_uint16              us_vlan_tci;
    oal_uint8               uc_tid = 0;
    mac_tx_ctl_stru        *pst_tx_ctl;
    mac_data_type_enum_uint8  uc_arp_type;
    hmac_vap_stru          *pst_hmac_vap     = OAL_PTR_NULL;
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    hmac_user_stru         *pst_hmac_user    = OAL_PTR_NULL;
#endif

    pst_tx_ctl     = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_buf);

    pst_hmac_vap   = (hmac_vap_stru *)mac_res_get_hmac_vap(MAC_GET_CB_TX_VAP_INDEX(pst_tx_ctl));
    if  (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_WARNING_LOG1(0,OAM_SF_TX,"{hmac_tx_classify_lan_to_wlan::mac_res_get_hmac_vap fail.vap_index[%u]}",MAC_GET_CB_TX_VAP_INDEX(pst_tx_ctl));
        return;
    }

#ifdef CONFIG_NF_CONNTRACK_MARK
/*the king of game feature will mark packets
 *and we will use VI queue to send these packets.
 */
    if(PKTMARK(pst_buf) == VIP_APP_MARK) {
        *puc_tid = VIP_APP_VIQUE_TID;
        pst_tx_ctl->bit_is_needretry = OAL_TRUE;
        return;
    }
#endif

    /* 获取以太网头 */
    pst_ether_header = (mac_ether_header_stru *)oal_netbuf_data(pst_buf);

    switch (pst_ether_header->us_ether_type)
    {
        /*lint -e778*//* 屏蔽Info -- Constant expression evaluates to 0 in operation '&' */
        case OAL_HOST2NET_SHORT(ETHER_TYPE_IP):

        #ifdef _PRE_WLAN_FEATURE_HS20
            if (pst_hmac_vap->st_cfg_qos_map_param.uc_valid)
            {
                hmac_tx_set_qos_map(pst_buf, &uc_tid);
               *puc_tid = uc_tid;
                return;
            }
        #endif //_PRE_WLAN_FEATURE_HS20

            /* 从IP TOS字段寻找优先级 */
            /*----------------------------------------------------------------------
                tos位定义
             ----------------------------------------------------------------------
            | bit7~bit5 | bit4 |  bit3  |  bit2  |   bit1   | bit0 |
            | 包优先级  | 时延 | 吞吐量 | 可靠性 | 传输成本 | 保留 |
             ----------------------------------------------------------------------*/
            pst_ip = (mac_ip_header_stru *)(pst_ether_header + 1);      /* 偏移一个以太网头，取ip头 */

            uc_tid = pst_ip->uc_tos >> WLAN_IP_PRI_SHIFT;

            if (0 != uc_tid)
            {
                break;
            }
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
            /* RTP RTSP 限制只在 P2P上才开启识别功能 */
            if (!IS_LEGACY_VAP(&(pst_hmac_vap->st_vap_base_info)))
#endif
            {
                hmac_tx_traffic_classify(pst_tx_ctl, pst_ip, &uc_tid);
            }

            /* 如果是DHCP帧，则进入VO队列发送 */
            if (OAL_TRUE == mac_is_dhcp_port(pst_ip))
            {
                uc_tid = WLAN_DATA_VIP_TID;
                pst_tx_ctl->bit_is_vipframe  = OAL_TRUE;
                pst_tx_ctl->bit_is_needretry = OAL_TRUE;
                pst_tx_ctl->bit_data_frame_type = MAC_DATA_DHCP;
            }
            else if (OAL_TRUE == mac_is_dns_frame(pst_ip))
            {
                pst_tx_ctl->bit_is_needretry = OAL_TRUE;
                pst_tx_ctl->bit_data_frame_type = MAC_DATA_DNS;
            }
            else if (MAC_TCP_PROTOCAL == pst_ip->uc_protocol)
            {
                hmac_tx_classify_tcp(pst_hmac_vap, pst_ip, pst_tx_ctl, &uc_tid);
            }
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
            pst_hmac_user    = (hmac_user_stru *)mac_res_get_hmac_user(MAC_GET_CB_TX_USER_IDX(pst_tx_ctl));
            if (OAL_UNLIKELY(OAL_PTR_NULL == pst_hmac_user))
            {
                OAM_WARNING_LOG1(0, OAM_SF_CFG, "{hmac_edca_opt_rx_pkts_stat::null param,pst_hmac_user[%d].}",MAC_GET_CB_TX_USER_IDX(pst_tx_ctl));
                break;
            }

            if ((OAL_TRUE == pst_hmac_vap->uc_edca_opt_flag_ap) && (WLAN_VAP_MODE_BSS_AP == pst_hmac_vap->st_vap_base_info.en_vap_mode))
            {
                /* mips优化:解决开启业务统计性能差10M问题 */
                if (((MAC_UDP_PROTOCAL == pst_ip->uc_protocol) && (pst_hmac_user->aaul_txrx_data_stat[WLAN_WME_TID_TO_AC(uc_tid)][WLAN_TX_UDP_DATA] < (HMAC_EDCA_OPT_PKT_NUM + 10)))
                    || ((MAC_TCP_PROTOCAL == pst_ip->uc_protocol) && (pst_hmac_user->aaul_txrx_data_stat[WLAN_WME_TID_TO_AC(uc_tid)][WLAN_TX_TCP_DATA] < (HMAC_EDCA_OPT_PKT_NUM + 10))))
                {
                    hmac_edca_opt_tx_pkts_stat(pst_tx_ctl, uc_tid, pst_ip);
                }
            }
#endif

            break;

        case OAL_HOST2NET_SHORT(ETHER_TYPE_IPV6):
            OAM_INFO_LOG0(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::ETHER_TYPE_IPV6.}");
            /* 从IPv6 traffic class字段获取优先级 */
            /*----------------------------------------------------------------------
                IPv6包头 前32为定义
             -----------------------------------------------------------------------
            | 版本号 | traffic class   | 流量标识 |
            | 4bit   | 8bit(同ipv4 tos)|  20bit   |
            -----------------------------------------------------------------------*/
            ul_ipv6_hdr = *((oal_uint32 *)(pst_ether_header + 1));  /* 偏移一个以太网头，取ip头 */

            ul_pri = (OAL_NET2HOST_LONG(ul_ipv6_hdr) & WLAN_IPV6_PRIORITY_MASK) >> WLAN_IPV6_PRIORITY_SHIFT;

            uc_tid = (oal_uint8)(ul_pri >> WLAN_IP_PRI_SHIFT);
            OAM_INFO_LOG1(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::uc_tid=%d.}", uc_tid);

            /* 如果是ND帧，则进入VO队列发送 */
            if (OAL_TRUE == mac_is_nd((oal_ipv6hdr_stru *)(pst_ether_header + 1)))
            {
                uc_tid = WLAN_DATA_VIP_TID;
                pst_tx_ctl->bit_is_vipframe  = OAL_TRUE;
                pst_tx_ctl->bit_is_needretry = OAL_TRUE;
            }
            /* 如果是DHCPV6帧，则进入VO队列发送 */
            else if (OAL_TRUE == mac_is_dhcp6((oal_ipv6hdr_stru *)(pst_ether_header + 1)))
            {
                OAM_INFO_LOG0(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::ETHER_TYPE_DHCP6.}");
                uc_tid = WLAN_DATA_VIP_TID;
                pst_tx_ctl->bit_is_vipframe  = OAL_TRUE;
                pst_tx_ctl->bit_is_needretry = OAL_TRUE;
                pst_tx_ctl->bit_data_frame_type = MAC_DATA_DHCPV6;
            }

            break;

        case OAL_HOST2NET_SHORT(ETHER_TYPE_PAE):
            /* 如果是EAPOL帧，则进入VO队列发送 */
            OAM_INFO_LOG0(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::ETHER_TYPE_PAE.}");
            uc_tid = WLAN_DATA_VIP_TID;
            pst_tx_ctl->bit_is_vipframe  = OAL_TRUE;
            pst_tx_ctl->bit_is_needretry = OAL_TRUE;

            pst_tx_ctl->bit_data_frame_type = MAC_DATA_EAPOL;
            /* 如果是4 次握手设置单播密钥，则设置tx cb 中bit_is_eapol_key_ptk 置一，dmac 发送不加密 */
            if (OAL_TRUE == mac_is_eapol_key_ptk((mac_eapol_header_stru *)(pst_ether_header + 1)))
            {
                pst_tx_ctl->bit_is_eapol_key_ptk = OAL_TRUE;
            }

            OAM_INFO_LOG1(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::uc_tid=%d.}", uc_tid);
            break;

        /* TDLS帧处理，建链保护，入高优先级TID队列 */
        case OAL_HOST2NET_SHORT(ETHER_TYPE_TDLS):
            OAM_INFO_LOG0(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::ETHER_TYPE_TDLS.}");
            uc_tid = WLAN_DATA_VIP_TID;
            OAM_INFO_LOG1(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::uc_tid=%d.}", uc_tid);
            break;

        /* PPPOE帧处理，建链保护(发现阶段, 会话阶段)，入高优先级TID队列 */
        case OAL_HOST2NET_SHORT(ETHER_TYPE_PPP_DISC):
        case OAL_HOST2NET_SHORT(ETHER_TYPE_PPP_SES):
            OAM_INFO_LOG0(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::ETHER_TYPE_PPP_DISC, ETHER_TYPE_PPP_SES.}");
            uc_tid = WLAN_DATA_VIP_TID;
            pst_tx_ctl->bit_is_vipframe  = OAL_TRUE;
            pst_tx_ctl->bit_is_needretry = OAL_TRUE;

            OAM_INFO_LOG1(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::uc_tid=%d.}", uc_tid);
            break;

#ifdef _PRE_WLAN_FEATURE_WAPI
        case OAL_HOST2NET_SHORT(ETHER_TYPE_WAI):
            OAM_INFO_LOG0(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::ETHER_TYPE_WAI.}");
            uc_tid = WLAN_DATA_VIP_TID;
            pst_tx_ctl->bit_is_vipframe  = OAL_TRUE;
            pst_tx_ctl->bit_is_needretry = OAL_TRUE;
            break;
#endif

        case OAL_HOST2NET_SHORT(ETHER_TYPE_VLAN):
            OAM_INFO_LOG0(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::ETHER_TYPE_VLAN.}");
            /* 获取vlan tag的优先级 */
            pst_vlan_ethhdr = (oal_vlan_ethhdr_stru *)oal_netbuf_data(pst_buf);

            /*------------------------------------------------------------------
                802.1Q(VLAN) TCI(tag control information)位定义
             -------------------------------------------------------------------
            |Priority | DEI  | Vlan Identifier |
            | 3bit    | 1bit |      12bit      |
             ------------------------------------------------------------------*/
            us_vlan_tci = OAL_NET2HOST_SHORT(pst_vlan_ethhdr->h_vlan_TCI);

            uc_tid = us_vlan_tci >> OAL_VLAN_PRIO_SHIFT;    /* 右移13位，提取高3位优先级 */
            OAM_INFO_LOG1(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::uc_tid=%d.}", uc_tid);

            break;

        case OAL_HOST2NET_SHORT(ETHER_TYPE_ARP):
            /* 如果是ARP帧，则进入VO队列发送 */
            uc_tid = WLAN_DATA_VIP_TID;
            pst_tx_ctl->bit_is_vipframe  = OAL_TRUE;
            uc_arp_type = mac_get_arp_type_by_arphdr((oal_eth_arphdr_stru *)(pst_ether_header + 1));
            pst_tx_ctl->bit_data_frame_type = uc_arp_type;
            OAM_INFO_LOG1(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::ETHER_TYPE_ARP,uc_tid=%d.}", uc_tid);
            break;

        /*lint +e778*/
        default:
            OAM_INFO_LOG1(0, OAM_SF_TX, "{hmac_tx_classify_lan_to_wlan::default us_ether_type[%d].}", pst_ether_header->us_ether_type);
            break;
    }

    /* 出参赋值 */
    *puc_tid = uc_tid;
}


OAL_STATIC OAL_INLINE oal_void hmac_tx_update_tid(oal_bool_enum_uint8  en_wmm, oal_uint8 *puc_tid)
{

    if (OAL_LIKELY(OAL_TRUE == en_wmm)) /*wmm使能*/
    {
        *puc_tid = (*puc_tid < WLAN_TIDNO_BUTT) ? WLAN_TOS_TO_TID(*puc_tid): WLAN_TIDNO_BCAST;
    }
    else   /*wmm不使能*/
    {
        *puc_tid = MAC_WMM_SWITCH_TID;
    }
}

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)

oal_uint8 hmac_tx_wmm_acm(oal_bool_enum_uint8  en_wmm, hmac_vap_stru *pst_hmac_vap, oal_uint8 *puc_tid)
{
    oal_uint8                   uc_ac;

    if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == puc_tid))
    {
        return OAL_FALSE;
    }

    if (OAL_FALSE == en_wmm)
    {
        return OAL_FALSE;
    }

    uc_ac = WLAN_WME_TID_TO_AC(*puc_tid);
    g_uc_ac_new = uc_ac;
    while ((g_uc_ac_new != WLAN_WME_AC_BK) && (OAL_TRUE == pst_hmac_vap->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[g_uc_ac_new].en_dot11QAPEDCATableMandatory))
    {
        switch (g_uc_ac_new)
        {
            case WLAN_WME_AC_VO:
                g_uc_ac_new = WLAN_WME_AC_VI;
                break;

            case WLAN_WME_AC_VI:
                g_uc_ac_new = WLAN_WME_AC_BE;
                break;

            default:
                g_uc_ac_new = WLAN_WME_AC_BK;
                break;
        }
    }

    if (g_uc_ac_new != uc_ac)
    {
        *puc_tid = WLAN_WME_AC_TO_TID(g_uc_ac_new);
    }

    return OAL_TRUE;
}
#endif /* defined(_PRE_PRODUCT_ID_HI110X_HOST) */


OAL_STATIC OAL_INLINE oal_void  hmac_tx_classify(
                hmac_vap_stru   *pst_hmac_vap,
                mac_user_stru   *pst_user,
                oal_netbuf_stru *pst_buf)
{
    oal_uint8               uc_tid = 0;
    mac_tx_ctl_stru        *pst_tx_ctl;
    mac_device_stru        *pst_mac_dev = OAL_PTR_NULL;

    hmac_tx_classify_lan_to_wlan(pst_buf, &uc_tid);

    /* 非QoS站点，直接返回 */
    if (OAL_UNLIKELY(OAL_TRUE != pst_user->st_cap_info.bit_qos))
    {
        OAM_INFO_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX, "{hmac_tx_classify::user is a none QoS station.}");
        return;
    }

    pst_mac_dev = mac_res_get_dev(pst_user->uc_device_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_dev))
    {
        OAM_WARNING_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX, "{hmac_tx_classify::pst_mac_dev null.}");
        return;
    }
    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_buf);

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
#ifdef _PRE_WLAN_FEATURE_WMMAC
    if (g_en_wmmac_switch)
    {
        oal_uint8               uc_ac_num;
        uc_ac_num = WLAN_WME_TID_TO_AC(uc_tid);
        /* 如果ACM位为1，且对应AC的TS没有建立成功，则将该AC的数据全部放到BE队列发送 */
        if((OAL_TRUE == pst_hmac_vap->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[uc_ac_num].en_dot11QAPEDCATableMandatory)
           && (pst_user->st_ts_info[uc_ac_num].en_ts_status != MAC_TS_SUCCESS))
        {
            uc_tid = WLAN_WME_AC_BE;
            if(OAL_TRUE == pst_tx_ctl->bit_is_vipframe)
            {
                uc_tid = WLAN_TIDNO_BEST_EFFORT;
                pst_tx_ctl->bit_is_vipframe  = OAL_FALSE;
                pst_tx_ctl->bit_is_needretry = OAL_FALSE;
            }
        }
    }
    else
#endif  // _PRE_WLAN_FEATURE_WMMAC
    {
        hmac_tx_wmm_acm(pst_mac_dev->en_wmm, pst_hmac_vap, &uc_tid);
    }
#endif  // _PRE_PRODUCT_ID_HI110X_HOST
#ifdef _PRE_WLAN_FEATURE_WMMAC
    if (!g_en_wmmac_switch)
#endif  // _PRE_WLAN_FEATURE_WMMAC
    {
        
        if ((OAL_TRUE != pst_tx_ctl->bit_is_vipframe) || (OAL_FALSE == pst_mac_dev->en_wmm))
        {
            hmac_tx_update_tid(pst_mac_dev->en_wmm, &uc_tid);
        }
    }

    /* 如果使能了vap流等级，则采用设置的vap流等级 */
    if (OAL_TRUE == pst_mac_dev->en_vap_classify)
    {
        uc_tid = pst_hmac_vap->uc_classify_tid;
    }

    /* 设置ac和tid到cb字段 */
    pst_tx_ctl->uc_tid  = uc_tid;
    pst_tx_ctl->uc_ac   = WLAN_WME_TID_TO_AC(uc_tid);

    OAM_INFO_LOG2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX, "{hmac_tx_classify::uc_ac=%d uc_tid=%d.}", pst_tx_ctl->uc_ac, pst_tx_ctl->uc_tid);

    return;
}
#endif


OAL_STATIC OAL_INLINE oal_uint32 hmac_tx_filter_security(hmac_vap_stru     *pst_hmac_vap,
                                        oal_netbuf_stru  *pst_buf,
                                        hmac_user_stru   *pst_hmac_user)
{
    mac_ether_header_stru   *pst_ether_header = OAL_PTR_NULL;
    mac_user_stru           *pst_mac_user     = OAL_PTR_NULL;
    mac_vap_stru            *pst_mac_vap      = OAL_PTR_NULL;
    oal_uint32               ul_ret           = OAL_SUCC;
    oal_uint16               us_ether_type;

    pst_mac_vap  = &(pst_hmac_vap->st_vap_base_info);
    pst_mac_user = &(pst_hmac_user->st_user_base_info);

    if (OAL_TRUE == mac_mib_get_rsnaactivated(pst_mac_vap))/* 判断是否使能WPA/WPA2 */
    {
        if (pst_mac_user->en_port_valid != OAL_TRUE)/* 判断端口是否打开 */
        {
            /* 获取以太网头 */
            pst_ether_header = (mac_ether_header_stru *)oal_netbuf_data(pst_buf);
            /* 发送数据时，针对非EAPOL 的数据帧做过滤 */

            if (oal_byteorder_host_to_net_uint16(ETHER_TYPE_PAE) != pst_ether_header->us_ether_type)
            {
                us_ether_type = oal_byteorder_host_to_net_uint16(pst_ether_header->us_ether_type);
                OAM_WARNING_LOG2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX, "{hmac_tx_filter_security::TYPE 0x%04x, 0x%04x.}",
                               us_ether_type, ETHER_TYPE_PAE);
                ul_ret = OAL_FAIL;
            }
        }
    }


    return ul_ret;

}


oal_void  hmac_tx_ba_setup(
                hmac_vap_stru    *pst_hmac_vap,
                hmac_user_stru   *pst_user,
                oal_uint8         uc_tidno)
{
    mac_action_mgmt_args_stru   st_action_args;   /* 用于填写ACTION帧的参数 */
    oal_bool_enum_uint8         en_ampdu_support = OAL_FALSE;

    /* 建立BA会话，是否需要判断VAP的AMPDU的支持情况，因为需要实现建立BA会话时，一定发AMPDU */
    en_ampdu_support = hmac_user_xht_support(pst_user);

    if (en_ampdu_support)
    {
        /*
        建立BA会话时，st_action_args结构各个成员意义如下
        (1)uc_category:action的类别
        (2)uc_action:BA action下的类别
        (3)ul_arg1:BA会话对应的TID
        (4)ul_arg2:BUFFER SIZE大小
        (5)ul_arg3:BA会话的确认策略
        (6)ul_arg4:TIMEOUT时间
        */
        st_action_args.uc_category = MAC_ACTION_CATEGORY_BA;
        st_action_args.uc_action   = MAC_BA_ACTION_ADDBA_REQ;
        st_action_args.ul_arg1     = uc_tidno;                     /* 该数据帧对应的TID号 */
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
        st_action_args.ul_arg2     = (oal_uint32)wlan_customize.ul_ampdu_tx_max_num;       /* ADDBA_REQ中，buffer_size的默认大小 */
        OAM_WARNING_LOG1(0, OAM_SF_TX, "hisi_customize_wifi::[ba buffer size:%d]", st_action_args.ul_arg2);
#else
        st_action_args.ul_arg2     = WLAN_AMPDU_TX_MAX_BUF_SIZE;       /* ADDBA_REQ中，buffer_size的默认大小 */
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

        st_action_args.ul_arg3     = MAC_BA_POLICY_IMMEDIATE;     /* BA会话的确认策略 */
        st_action_args.ul_arg4     = 0;                            /* BA会话的超时时间设置为0 */

        /* 建立BA会话 */
        hmac_mgmt_tx_action(pst_hmac_vap, pst_user, &st_action_args);
    }
    else
    {
        if (DMAC_BA_INIT != pst_user->ast_tid_info[uc_tidno].st_ba_tx_info.en_ba_status)
        {
            st_action_args.uc_category = MAC_ACTION_CATEGORY_BA;
            st_action_args.uc_action   = MAC_BA_ACTION_DELBA;
            st_action_args.ul_arg1     = uc_tidno;                                         /* 该数据帧对应的TID号 */
            st_action_args.ul_arg2     = MAC_ORIGINATOR_DELBA;                             /* 发送端删除ba */
            st_action_args.ul_arg3     = MAC_UNSPEC_REASON;                                /* BA会话删除原因 */
            st_action_args.puc_arg5     = pst_user->st_user_base_info.auc_user_mac_addr;   /* 用户mac地址 */

            /* 删除BA会话 */
            hmac_mgmt_tx_action(pst_hmac_vap, pst_user, &st_action_args);
        }
    }

}


oal_uint32 hmac_tx_ucast_process(
                hmac_vap_stru     *pst_hmac_vap,
                oal_netbuf_stru   *pst_buf,
                hmac_user_stru    *pst_user,
                mac_tx_ctl_stru   *pst_tx_ctl)
{
    oal_uint32         ul_ret = HMAC_TX_PASS;

   /*安全过滤*/
#if defined(_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
    if (OAL_UNLIKELY(OAL_SUCC != hmac_tx_filter_security(pst_hmac_vap, pst_buf, pst_user)))
    {
        OAM_STAT_VAP_INCR(pst_hmac_vap->st_vap_base_info.uc_vap_id, tx_security_check_faild, 1);
        return HMAC_TX_DROP_SECURITY_FILTER;
    }
#endif

    /*以太网业务识别*/
#ifdef _PRE_WLAN_FEATURE_CLASSIFY
    hmac_tx_classify(pst_hmac_vap, &(pst_user->st_user_base_info), pst_buf);
#endif

#ifdef _PRE_WLAN_FEATURE_TRAFFIC_CTL
    /* TBD */
#endif

    OAL_MIPS_TX_STATISTIC(HMAC_PROFILING_FUNC_TRAFFIC_CLASSIFY);
    /* 如果是EAPOL、DHCP帧，则不允许主动建立BA会话 */
    if (OAL_FALSE == pst_tx_ctl->bit_is_vipframe)
    {
#ifdef _PRE_WLAN_FEATURE_AMPDU
        if (OAL_TRUE == hmac_tid_need_ba_session(pst_hmac_vap, pst_user, pst_tx_ctl->uc_tid, pst_buf))
        {
            /* 自动触发建立BA会话，设置AMPDU聚合参数信息在DMAC模块的处理addba rsp帧的时刻后面 */
            hmac_tx_ba_setup(pst_hmac_vap, pst_user, pst_tx_ctl->uc_tid);
        }
#endif

        OAL_MIPS_TX_STATISTIC(HMAC_PROFILING_FUNC_SETUP_BA);
#ifdef _PRE_WLAN_FEATURE_AMSDU
        ul_ret = hmac_amsdu_notify(pst_hmac_vap, pst_user, pst_buf);
        if (OAL_UNLIKELY(HMAC_TX_PASS != ul_ret))
        {
            return ul_ret;
        }
#endif
        OAL_MIPS_TX_STATISTIC(HMAC_PROFILING_FUNC_AMSDU);
    }

    return ul_ret;
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)

OAL_STATIC oal_uint32 hmac_nonqos_frame_set_sn(hmac_vap_stru *pst_hmac_vap, mac_tx_ctl_stru *pst_tx_ctl)
{
    pst_tx_ctl->pst_frame_header->bit_seq_num = (us_noqos_frag_seqnum++) & 0x0fff;
    pst_tx_ctl->en_seq_ctrl_bypass = OAL_TRUE;
    return OAL_SUCC;
}

OAL_STATIC oal_bool_enum_uint8  hmac_tx_is_need_frag(hmac_vap_stru *pst_hmac_vap, hmac_user_stru *pst_hmac_user, oal_netbuf_stru *pst_netbuf, mac_tx_ctl_stru *pst_tx_ctl)
{
    oal_uint32        ul_threshold;
	oal_uint32        uc_last_frag;
    /* 判断报文是否需要进行分片 */
    /* 1、长度大于门限          */
    /* 2、是legac协议模式       */
    /* 3、不是广播帧            */
    /* 4、不是聚合帧            */
    /* 6、DHCP帧不进行分片      */

    if (OAL_TRUE == pst_tx_ctl->bit_is_vipframe)
    {
        return OAL_FALSE;
    }

    ul_threshold = pst_hmac_vap->st_vap_base_info.pst_mib_info->st_wlan_mib_operation.ul_dot11FragmentationThreshold;
    ul_threshold = (ul_threshold & (~(BIT0|BIT1))) + 2;
	/* 规避1151硬件bug,调整分片门限：TKIP加密时，当最后一个分片的payload长度小于等于8时，无法进行加密 */
	if (WLAN_80211_CIPHER_SUITE_TKIP == pst_hmac_user->st_user_base_info.st_key_info.en_cipher_type)
	{
		uc_last_frag = (OAL_NETBUF_LEN(pst_netbuf) + 8) % (ul_threshold - pst_tx_ctl->uc_frame_header_length - (WEP_IV_FIELD_SIZE + EXT_IV_FIELD_SIZE));
		if ((uc_last_frag > 0) && (uc_last_frag <= 8))
		{
			ul_threshold = ul_threshold + 8;
		}
	}

    return (((OAL_NETBUF_LEN(pst_netbuf) + pst_tx_ctl->uc_frame_header_length) > ul_threshold)
                     && (!pst_tx_ctl->en_ismcast)
                     && (!pst_tx_ctl->en_is_amsdu)
                #ifdef _PRE_WLAN_FEATURE_MULTI_NETBUF_AMSDU
                     && (OAL_FALSE == MAC_GET_CB_HAS_EHTER_HEAD(pst_tx_ctl))
                #endif
                     && (pst_hmac_user->st_user_base_info.en_cur_protocol_mode < WLAN_HT_MODE || pst_hmac_vap->st_vap_base_info.en_protocol < WLAN_HT_MODE)
                     &&(OAL_FALSE == hmac_vap_ba_is_setup(pst_hmac_user, pst_tx_ctl->uc_tid)));/* [false alarm]:返回值是oal_bool_enum_uint8 和函数类型一致*/

}


#else


OAL_STATIC oal_bool_enum_uint8  hmac_tx_is_need_frag(hmac_vap_stru *pst_hmac_vap, hmac_user_stru *pst_hmac_user, oal_netbuf_stru *pst_netbuf, mac_tx_ctl_stru *pst_tx_ctl)
{
    oal_uint32        ul_threshold;
    /* 判断报文是否需要进行分片 */
    /* 1、长度大于门限          */
    /* 2、是legac协议模式       */
    /* 3、不是广播帧            */
    /* 4、不是聚合帧            */
    /* 6、DHCP帧不进行分片      */

    if (OAL_TRUE == pst_tx_ctl->bit_is_vipframe)
    {
        return OAL_FALSE;
    }
    ul_threshold = pst_hmac_vap->st_vap_base_info.pst_mib_info->st_wlan_mib_operation.ul_dot11FragmentationThreshold;
    ul_threshold = (ul_threshold & (~(BIT0|BIT1))) + 2;

    return ((OAL_NETBUF_LEN(pst_netbuf) > ul_threshold)
                     && (!pst_tx_ctl->en_ismcast)
                     && (!pst_tx_ctl->en_is_amsdu)
                     && (pst_hmac_user->st_user_base_info.en_avail_protocol_mode < WLAN_HT_MODE || pst_hmac_vap->st_vap_base_info.en_protocol < WLAN_HT_MODE)
                     &&(OAL_FALSE == hmac_vap_ba_is_setup(pst_hmac_user, pst_tx_ctl->uc_tid)));/* [false alarm]:返回值是oal_bool_enum_uint8 和函数类型一致*/

}

#endif

#ifdef _PRE_WLAN_FEATURE_APF

oal_void hmac_auto_set_apf_switch_in_suspend(oal_uint32 ul_total_pps)
{
    mac_device_stru                 *pst_mac_device;
    oal_bool_enum_uint8              en_apf_switch = OAL_FALSE;
    mac_vap_stru                    *pst_mac_vap;
    mac_cfg_suspend_stru             st_suspend;
    oal_uint32                       ul_ret;

    pst_mac_device = mac_res_get_dev(0);
    /* 如果非单VAP,则不处理 */
    if (1 != mac_device_calc_up_vap_num(pst_mac_device))
    {
        return;
    }

    if (g_en_force_pass_filter == OAL_FALSE || pst_mac_device->uc_in_suspend == OAL_FALSE
#ifdef _PRE_WLAN_FEATURE_WAPI
        || (pst_mac_device->uc_wapi == OAL_TRUE)
#endif
       ) {
        return;
    }

    /* 暗屏模式下只有低流量时才开启apf过滤，否则关闭此功能 */
    if (ul_total_pps < g_st_thread_bindcpu.us_throughput_pps_irq_low && (g_us_apf_program_len > OAL_SIZEOF(oal_uint8)))
    {
        en_apf_switch = OAL_TRUE;
    }

    if (en_apf_switch == pst_mac_device->en_apf_switch)
    {
        return;
    }
    /* 需要切换时，满足条件后通知device操作 */

    pst_mac_device->en_apf_switch = en_apf_switch;

    ul_ret = mac_device_find_up_vap(pst_mac_device, &pst_mac_vap);
    if ((OAL_SUCC != ul_ret) || (OAL_PTR_NULL == pst_mac_vap))
    {
        return;
    }
    OAM_WARNING_LOG3(0,OAM_SF_ANY,"{hmac_auto_set_apf_switch_in_suspend: 1.en_apf_switch = [%d],2.cur_txrx_pps = [%d],3.pps_low_th = [%d]! send to device!}",
                    en_apf_switch,ul_total_pps,g_st_thread_bindcpu.us_throughput_pps_irq_low);

    st_suspend.uc_apf_switch = en_apf_switch;
    st_suspend.uc_in_suspend = pst_mac_device->uc_in_suspend;
    st_suspend.uc_arpoffload_switch = pst_mac_device->uc_arpoffload_switch;
    /***************************************************************************
        抛事件到DMAC层, 同步屏幕最新状态到DMAC
    ***************************************************************************/
    ul_ret = hmac_config_send_event(pst_mac_vap, WLAN_CFGID_APF_SWITCH_SYN, OAL_SIZEOF(mac_cfg_suspend_stru), (oal_uint8 *)&st_suspend);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{hmac_auto_set_apf_switch_in_suspend::hmac_config_send_event failed[%d]}",ul_ret);
    }


}
#endif


oal_void hmac_set_psm_activity_timer(oal_uint32 ul_total_pps)
{
    oal_uint8                       uc_current_pps_level = PPS_LEVEL_IDLE;
    oal_uint32                      ul_pm_timer_restart_cnt = HMAC_PSM_TIMER_MIDIUM_CNT;
    oal_uint32                      ul_ret;
    mac_device_stru                 *pst_mac_device;
    mac_vap_stru                    *pst_mac_vap;
    mac_cfg_ps_params_stru          st_ps_params;

    /* 只分两档忙或闲，避免档位太多，来回抖动 */
    if(ul_total_pps < PSM_PPS_VALUE_LOW)
    {
        uc_current_pps_level = PPS_LEVEL_IDLE;
#ifdef _PRE_WLAN_DOWNLOAD_PM
        ul_pm_timer_restart_cnt = download_rate_limit_pps ? 1 : wlan_fast_check_cnt;
#else
        ul_pm_timer_restart_cnt = wlan_fast_check_cnt;
#endif
    }
    else
    {
        uc_current_pps_level = PPS_LEVEL_BUSY;
        ul_pm_timer_restart_cnt = HMAC_PSM_TIMER_BUSY_CNT;
    }

    /* 如果目前的吞吐率和上一次统计的吞吐率属于同一个等级，则不需要将结果下发 */
    if(uc_current_pps_level == g_uc_pps_level)
    {
        g_uc_pps_level_adjust_cnt = 0;
        return;
    }

    /* 为保证性能，pps从高到低时，只有连续3个周期都低于门限，才启动快睡模式 */
    if(uc_current_pps_level < g_uc_pps_level)
    {
        g_uc_pps_level_adjust_cnt++;
        if(g_uc_pps_level_adjust_cnt < PSM_PPS_LEVEL_SUCCESSIVE_DOWN_CNT)
        {
            return;
        }
    }
    /* 为保证性能，如果pps升高超过门限，立即增大pm定时器重启次数 */
    g_uc_pps_level_adjust_cnt = 0;/* 此清零操作是pps从高到低门限超过3次，以及从低到高立即下发时的计数清零 */
    g_uc_pps_level = uc_current_pps_level;

    /* 需要切换时，满足条件后通知device操作 */
    pst_mac_device = mac_res_get_dev(0);
    /* 如果非单VAP,则不处理 */
    if (1 != mac_device_calc_up_vap_num(pst_mac_device))
    {
        return;
    }
    ul_ret = mac_device_find_up_vap(pst_mac_device, &pst_mac_vap);
    if ((OAL_SUCC != ul_ret) || (OAL_PTR_NULL == pst_mac_vap))
    {
        return;
    }

    st_ps_params.en_cmd           = MAC_PS_PARAMS_RESTART_COUNT;
    st_ps_params.uc_restart_count = ul_pm_timer_restart_cnt;
    ul_ret = hmac_config_set_ps_params(pst_mac_vap, OAL_SIZEOF(mac_cfg_ps_params_stru),(oal_uint8 *)(&st_ps_params));
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "{hmac_set_psm_activity_timer:: send failed!}");
        return;
    }

}


oal_void  hmac_tx_small_amsdu_switch(oal_uint32 ul_rx_throughput_mbps,oal_uint32 ul_tx_pps)
{
    mac_device_stru                 *pst_mac_device;
    hmac_vap_stru                   *pst_hmac_vap = OAL_PTR_NULL;
    mac_vap_stru                    *pst_mac_vap  = OAL_PTR_NULL;
    oal_uint32                       ul_limit_throughput_high;
    oal_uint32                       ul_limit_throughput_low;
    oal_uint32                       ul_limit_pps_high;
    oal_uint32                       ul_limit_pps_low;
    oal_uint32                       ul_ret;
    oal_bool_enum_uint8              en_small_amsdu;

    /* 如果定制化不支持小包聚合 */
    if (OAL_FALSE == g_st_small_amsdu_switch.uc_ini_small_amsdu_en)
    {
        return;
    }

    /* 每秒吞吐量门限 */
    ul_limit_throughput_high = g_st_small_amsdu_switch.us_small_amsdu_throughput_high;
    ul_limit_throughput_low  = g_st_small_amsdu_switch.us_small_amsdu_throughput_low;

    /* 每秒PPS门限 */
    ul_limit_pps_high = g_st_small_amsdu_switch.us_small_amsdu_pps_high;
    ul_limit_pps_low  = g_st_small_amsdu_switch.us_small_amsdu_pps_low;


    if ((ul_rx_throughput_mbps > ul_limit_throughput_high) || (ul_tx_pps > ul_limit_pps_high))
    {
        /* rx吞吐量高于150M或者tx pps大于12500,打开小包amsdu聚合 */
        en_small_amsdu = OAL_TRUE;
    }
    else if ((ul_rx_throughput_mbps < ul_limit_throughput_low) && (ul_tx_pps < ul_limit_pps_low))
    {
        /* rx吞吐量低于100M且tx pps小于7500,关闭小包amsdu聚合，避免来回切换 */
        en_small_amsdu = OAL_FALSE;
    }
    else
    {
        /* 介于100M-150M之间,不作切换 */
        return;
    }


     /* 当前聚合方式相同,不处理 */
    if (g_st_small_amsdu_switch.uc_cur_small_amsdu_en == en_small_amsdu)
    {
        return;
    }

    pst_mac_device = mac_res_get_dev(0);
    /* 如果非单VAP,则不切换 */
    if (1 != mac_device_calc_up_vap_num(pst_mac_device))
    {
        return;
    }

    ul_ret = mac_device_find_up_vap(pst_mac_device, &pst_mac_vap);
    if ((OAL_SUCC != ul_ret) || (OAL_PTR_NULL == pst_mac_vap))
    {
        return;
    }

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id,OAM_SF_ANY,"hmac_tx_small_amsdu_switch:pst_hmac_vap is null");
        return;
    }
    g_st_small_amsdu_switch.uc_cur_small_amsdu_en = en_small_amsdu;
    pst_hmac_vap->en_small_amsdu_switch = en_small_amsdu;
    OAM_WARNING_LOG3(0,OAM_SF_ANY,"{hmac_tx_small_amsdu_switch: 1.ul_rx_throughput_mbps = [%d],2.ul_tx_pps = [%d],3.en_small_amsdu= [%d]!}",
          ul_rx_throughput_mbps,ul_tx_pps, en_small_amsdu);
}

#ifdef _PRE_WLAN_FEATURE_TCP_ACK_BUFFER

oal_void  hmac_tx_tcp_ack_buf_switch(oal_uint32 ul_rx_throughput_mbps)
{
    mac_device_stru  *pst_mac_device;
    mac_vap_stru     *pst_mac_vap = OAL_PTR_NULL;
    mac_cfg_tcp_ack_buf_stru       st_tcp_ack_param = {0};
    oal_uint32      ul_limit_throughput_high;
    oal_uint32      ul_limit_throughput_low;
    oal_uint32      ul_ret;
    oal_bool_enum_uint8   en_tcp_ack_buf;

    /* 如果定制化不支持tcp_ack_buf */
    if (OAL_FALSE == tcp_ack_buf_switch.uc_ini_tcp_ack_buf_en)
    {
        return;
    }

    pst_mac_device = mac_res_get_dev(0);
    /* 如果非单VAP,则不切换 */
    if (mac_device_calc_up_vap_num(pst_mac_device) != 1)
    {
        return;
    }

    ul_ret = mac_device_find_up_vap(pst_mac_device, &pst_mac_vap);
    if ((OAL_SUCC != ul_ret) || (OAL_PTR_NULL == pst_mac_vap))
    {
        return;
    }

    /* 非STA模式不切换 */
    if ( WLAN_VAP_MODE_BSS_STA != pst_mac_vap->en_vap_mode)
    {
        return;
    }

    if (pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_20M)
    {
        /* 每秒吞吐量门限 */
        ul_limit_throughput_high = tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high;
        ul_limit_throughput_low  = tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low;
    }
    else if ((pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40PLUS)
            || (pst_mac_vap->st_channel.en_bandwidth == WLAN_BAND_WIDTH_40MINUS))
    {
        /* 每秒吞吐量门限 */
        ul_limit_throughput_high = tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high_40M;
        ul_limit_throughput_low  = tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low_40M;
    }
    else if ((pst_mac_vap->st_channel.en_bandwidth >= WLAN_BAND_WIDTH_80PLUSPLUS)
            && (pst_mac_vap->st_channel.en_bandwidth < WLAN_BAND_WIDTH_40M))
    {
        ul_limit_throughput_high = tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high_80M;
        ul_limit_throughput_low  = tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low_80M;
    }
    else
    {
        return;
    }

    if (ul_rx_throughput_mbps > ul_limit_throughput_high)
    {
        en_tcp_ack_buf = OAL_TRUE;
    }
    else if (ul_rx_throughput_mbps < ul_limit_throughput_low)
    {
        en_tcp_ack_buf = OAL_FALSE;
    }
    else
    {
        /* 介于low-high之间,不作切换 */
        return;
    }

     /* 当前聚合方式相同,不处理 */
    if (tcp_ack_buf_switch.uc_cur_tcp_ack_buf_en == en_tcp_ack_buf)
    {
        return;
    }

    OAM_WARNING_LOG4(0,OAM_SF_ANY,"{hmac_tx_tcp_ack_buf_switch: limit_high = [%d],limit_low = [%d],rx_throught= [%d]! en_tcp_ack_buf=%d}",
          ul_limit_throughput_high,ul_limit_throughput_low, ul_rx_throughput_mbps, en_tcp_ack_buf);

    tcp_ack_buf_switch.uc_cur_tcp_ack_buf_en = en_tcp_ack_buf;

    st_tcp_ack_param.en_cmd = MAC_TCP_ACK_BUF_ENABLE;
    st_tcp_ack_param.en_enable = en_tcp_ack_buf;

    hmac_config_tcp_ack_buf(pst_mac_vap, OAL_SIZEOF(mac_cfg_tcp_ack_buf_stru), (oal_uint8*)&st_tcp_ack_param);


}
#endif

#ifdef _PRE_WLAN_FEATURE_DYN_BYPASS_EXTLNA
oal_void hmac_rx_dyn_bypass_extlna_switch(oal_uint32 ul_tx_throughput_mbps, oal_uint32 ul_rx_throughput_mbps)
{
    mac_device_stru  *pst_mac_device;
    mac_vap_stru     *pst_mac_vap;
    oal_uint32      ul_limit_throughput_high;
    oal_uint32      ul_limit_throughput_low;
    oal_uint32      ul_throughput_mbps = OAL_MAX(ul_tx_throughput_mbps, ul_rx_throughput_mbps);
    oal_uint32      ul_ret;
    oal_bool_enum_uint8   en_is_pm_test;

    /* 如果定制化不支持根据吞吐bypass外置LNA */
    if (OAL_FALSE == g_st_rx_dyn_bypass_extlna_switch.uc_ini_en)
    {
        return;
    }

    /* 每秒吞吐量门限 */
    ul_limit_throughput_high = g_st_rx_dyn_bypass_extlna_switch.us_throughput_high;
    ul_limit_throughput_low  = g_st_rx_dyn_bypass_extlna_switch.us_throughput_low;

    if (ul_throughput_mbps > ul_limit_throughput_high)
    {
        /* 高于100M,非低功耗测试场景 */
        en_is_pm_test = OAL_FALSE;
    }
    else if (ul_throughput_mbps < ul_limit_throughput_low)
    {
        /* 低于50M,低功耗测试场景 */
        en_is_pm_test = OAL_TRUE;
    }
    else
    {
        /* 介于50M-100M之间,不作切换 */
        return;
    }

    /* 需要切换时，满足条件后通知device操作 */
    pst_mac_device = mac_res_get_dev(0);

    /* 如果非单VAP,则不处理 */
    if (1 != mac_device_calc_up_vap_num(pst_mac_device))
    {
        return;
    }

    ul_ret = mac_device_find_up_vap(pst_mac_device, &pst_mac_vap);
    if ((OAL_SUCC != ul_ret) || (OAL_PTR_NULL == pst_mac_vap))
    {
        return;
    }

    /* 当前方式相同,不处理 */
    if (g_st_rx_dyn_bypass_extlna_switch.uc_cur_status == en_is_pm_test)
    {
        return;
    }

    ul_ret = hmac_config_send_event(pst_mac_vap, WLAN_CFGID_DYN_EXTLNA_BYPASS_SWITCH, OAL_SIZEOF(oal_uint8), (oal_uint8 *)(&en_is_pm_test));
    if (OAL_SUCC == ul_ret)
    {
        g_st_rx_dyn_bypass_extlna_switch.uc_cur_status = en_is_pm_test;
    }

    OAM_WARNING_LOG4(0,OAM_SF_ANY,"{hmac_rx_dyn_bypass_extlna_switch: limit_high[%d],limit_low[%d],throughput[%d], uc_cur_status[%d](0:not pm, 1:pm))!}",
        ul_limit_throughput_high,ul_limit_throughput_low, ul_throughput_mbps, en_is_pm_test);
}
#endif


OAL_INLINE oal_uint32  hmac_tx_encap(
                hmac_vap_stru    *pst_vap,
                hmac_user_stru   *pst_user,
                oal_netbuf_stru  *pst_buf)
{
    mac_ieee80211_qos_htc_frame_addr4_stru   *pst_hdr;           /* 802.11头 */
    mac_ether_header_stru                    *pst_ether_hdr;
    oal_uint32                                ul_qos = HMAC_TX_BSS_NOQOS;
    mac_tx_ctl_stru                          *pst_tx_ctl;
    oal_uint16                                us_ether_type = 0;
    oal_uint8                                 auc_saddr[ETHER_ADDR_LEN];   /* 原地址指针 */
    oal_uint8                                 auc_daddr[ETHER_ADDR_LEN];   /* 目的地址指针 */
    oal_uint32                                ul_ret;
    mac_llc_snap_stru                        *pst_snap_hdr;

#ifdef _PRE_DEBUG_MODE
    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_vap) || (OAL_PTR_NULL == pst_buf)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_TX, "{hmac_tx_encap::param null,%x %x.}", (uintptr_t)pst_vap, (uintptr_t)pst_buf);
        return OAL_ERR_CODE_PTR_NULL;
    }
#endif

    /* 获取CB */
    pst_tx_ctl = (mac_tx_ctl_stru *)(oal_netbuf_cb(pst_buf));
    pst_tx_ctl->bit_align_padding_offset = 0;

#ifdef _PRE_WLAN_NARROW_BAND
    if(hitalk_status & NARROW_BAND_ON_MASK)
    {
        if (!MAC_GET_CB_IS_VIPFRAME(pst_tx_ctl) &&(OAL_NETBUF_LEN(pst_buf) > 200))
        {
            OAM_WARNING_LOG0(0,0, "drop more than 200 byte non vip frame packet");
            return OAL_ERR_CODE_PTR_NULL;
        }
    }
#endif

    /*如果skb中data指针前预留的空间大于802.11 mac head len，则不需要格外申请内存存放802.11头,3为pading最大值 */
    if (oal_netbuf_headroom(pst_buf) >=  MAC_80211_QOS_HTC_4ADDR_FRAME_LEN + SNAP_LLC_FRAME_LEN + MAC_MAX_MSDU_ADDR_ALIGN)
    {
        pst_hdr = (mac_ieee80211_qos_htc_frame_addr4_stru *)(OAL_NETBUF_HEADER(pst_buf)- MAC_80211_QOS_HTC_4ADDR_FRAME_LEN - SNAP_LLC_FRAME_LEN -2);
        pst_tx_ctl->bit_80211_mac_head_type = 1;  /*指示mac头部在skb中*/
    }
    else
    {
        /* 申请最大的80211头 */
        pst_hdr = (mac_ieee80211_qos_htc_frame_addr4_stru *)OAL_MEM_ALLOC(OAL_MEM_POOL_ID_SHARED_DATA_PKT,
                                                                          MAC_80211_QOS_HTC_4ADDR_FRAME_LEN,
                                                                          OAL_FALSE);

        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_hdr))
        {
            OAM_ERROR_LOG0(pst_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX, "{hmac_tx_encap::pst_hdr null.}");
            return OAL_ERR_CODE_PTR_NULL;
        }

        pst_tx_ctl->bit_80211_mac_head_type = 0;  /*指示mac头部不在skb中，申请了额外内存存放的*/
    }

    /* 获取以太网头, 原地址，目的地址, 以太网类型 */
    pst_ether_hdr = (mac_ether_header_stru *)oal_netbuf_data(pst_buf);
    oal_set_mac_addr((oal_uint8 *)auc_daddr, pst_ether_hdr->auc_ether_dhost);
    oal_set_mac_addr((oal_uint8 *)auc_saddr, pst_ether_hdr->auc_ether_shost);

    /*非amsdu帧*/
    if (OAL_FALSE == pst_tx_ctl->en_is_amsdu)
    {
        us_ether_type = pst_ether_hdr->us_ether_type;
    }
    else
    {
       /* 如果是AMSDU的第一个子帧，需要从snap头中获取以太网类型，如果是以太网帧，可以
          直接从以太网头中获取*/
        pst_snap_hdr  = (mac_llc_snap_stru *)((oal_uint8 *)pst_ether_hdr + ETHER_HDR_LEN);
        us_ether_type = pst_snap_hdr->us_ether_type;
    }

    /* 非组播帧，获取用户的QOS能力位信息 */
    if (OAL_FALSE == pst_tx_ctl->en_ismcast)
    {
        /* 根据用户结构体的cap_info，判断是否是QOS站点 */
        ul_qos                      = pst_user->st_user_base_info.st_cap_info.bit_qos;
        pst_tx_ctl->en_is_qosdata   = pst_user->st_user_base_info.st_cap_info.bit_qos;
    }

    /* 设置帧控制 */
    hmac_tx_set_frame_ctrl(ul_qos, pst_tx_ctl, pst_hdr);

    /* 设置地址 */
    ul_ret = hmac_tx_set_addresses(pst_vap, pst_user, pst_tx_ctl, auc_saddr, auc_daddr, pst_hdr, us_ether_type);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        if (0 == pst_tx_ctl->bit_80211_mac_head_type)
        {
            OAL_MEM_FREE(pst_hdr, OAL_TRUE);
        }
        OAM_ERROR_LOG1(pst_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX, "{hmac_tx_encap::hmac_tx_set_addresses failed[%d].}", ul_ret);
        return ul_ret;
    }

    /* 对于LAN to WLAN的非AMSDU聚合帧，填充LLC SNAP头 */
    if (OAL_FALSE == pst_tx_ctl->en_is_amsdu)
    {

#ifdef _PRE_WLAN_CHIP_TEST
        /* al tx */
        if(OAL_SWITCH_ON != pst_vap->st_vap_base_info.bit_al_tx_flag)
        {
#endif
#ifdef _PRE_WLAN_FEATURE_MULTI_NETBUF_AMSDU
           hmac_tx_encap_large_skb_amsdu(pst_vap, pst_user, pst_buf, pst_tx_ctl);

           if (MAC_GET_CB_HAS_EHTER_HEAD(pst_tx_ctl))
           {
               /* 恢复data指针到ETHER HEAD - LLC HEAD */
               oal_netbuf_pull(pst_buf, SNAP_LLC_FRAME_LEN);
           }
#endif

           mac_set_snap(pst_buf, us_ether_type, (ETHER_HDR_LEN - SNAP_LLC_FRAME_LEN));
#ifdef _PRE_WLAN_FEATURE_MULTI_NETBUF_AMSDU
        if (MAC_GET_CB_HAS_EHTER_HEAD(pst_tx_ctl))
        {
            /* 恢复data指针到ETHER HEAD */
            oal_netbuf_push(pst_buf, ETHER_HDR_LEN);
            /* 保证4bytes对齐 */
            if((oal_uint)(uintptr_t)oal_netbuf_data(pst_buf) != OAL_ROUND_DOWN((oal_uint)(uintptr_t)oal_netbuf_data(pst_buf), 4))
            {
                pst_tx_ctl->bit_align_padding_offset = (oal_uint)(uintptr_t)oal_netbuf_data(pst_buf)- OAL_ROUND_DOWN((oal_uint)(uintptr_t)oal_netbuf_data(pst_buf), 4);
                oal_netbuf_push(pst_buf, pst_tx_ctl->bit_align_padding_offset);
            }
        }
#endif
#ifdef _PRE_WLAN_CHIP_TEST
        }
#endif
        /* 更新frame长度 */
        pst_tx_ctl->us_mpdu_len = (oal_uint16) oal_netbuf_get_len(pst_buf);

        /* 非amsdu聚合帧，记录mpdu字节数，不包括snap */
        pst_tx_ctl->us_mpdu_bytes = pst_tx_ctl->us_mpdu_len - SNAP_LLC_FRAME_LEN;
#ifdef _PRE_WLAN_FEATURE_SNIFFER
		proc_sniffer_write_file((const oal_uint8*)pst_hdr, MAC_80211_QOS_FRAME_LEN,
				(const oal_uint8 *)oal_netbuf_data(pst_buf), pst_tx_ctl->us_mpdu_len, 1);
#endif
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* mac头部在skb中时，netbuf的data指针指向mac头。但是mac_set_snap函数中已经将data指针指向了llc头。因此这里要重新push到mac头。  */
    if (pst_tx_ctl->bit_80211_mac_head_type == 1)
    {
        oal_netbuf_push(pst_buf, MAC_80211_QOS_HTC_4ADDR_FRAME_LEN);
    }
#endif

    /* 挂接802.11头 */
    pst_tx_ctl->pst_frame_header = (mac_ieee80211_frame_stru *)pst_hdr;

    /* 分片处理 */
    if (OAL_TRUE == hmac_tx_is_need_frag(pst_vap, pst_user, pst_buf, pst_tx_ctl))
    {
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
        hmac_nonqos_frame_set_sn(pst_vap, pst_tx_ctl);
#endif
        ul_ret = hmac_frag_process_proc(pst_vap, pst_user, pst_buf, pst_tx_ctl);
    }

    return ul_ret;
}


OAL_STATIC oal_uint32  hmac_tx_lan_mpdu_process_sta(
                hmac_vap_stru      *pst_vap,
                oal_netbuf_stru    *pst_buf,
                mac_tx_ctl_stru    *pst_tx_ctl)
{
    hmac_user_stru          *pst_user;      /* 目标STA结构体 */
    mac_ether_header_stru   *pst_ether_hdr; /* 以太网头 */
    oal_uint32              ul_ret;
    oal_uint16              us_user_idx;
    oal_uint8               *puc_ether_payload;

    pst_ether_hdr  = (mac_ether_header_stru *)oal_netbuf_data(pst_buf);
    pst_tx_ctl->uc_tx_vap_index = pst_vap->st_vap_base_info.uc_vap_id;

    us_user_idx = pst_vap->st_vap_base_info.uc_assoc_vap_id;

    pst_user = (hmac_user_stru *)mac_res_get_hmac_user(us_user_idx);
    if (OAL_PTR_NULL == pst_user)
    {
        OAM_STAT_VAP_INCR(pst_vap->st_vap_base_info.uc_vap_id, tx_abnormal_msdu_dropped, 1);
        return HMAC_TX_DROP_USER_NULL;
    }

    if (oal_byteorder_host_to_net_uint16(ETHER_TYPE_ARP) == pst_ether_hdr->us_ether_type)
    {
        pst_ether_hdr++;
        puc_ether_payload = (oal_uint8 *)pst_ether_hdr;
        /* The source MAC address is modified only if the packet is an   */
        /* ARP Request or a Response. The appropriate bytes are checked. */
        /* Type field (2 bytes): ARP Request (1) or an ARP Response (2)  */
        if((puc_ether_payload[6] == 0x00) &&
          (puc_ether_payload[7] == 0x02 || puc_ether_payload[7] == 0x01))
        {
            /* Set Address2 field in the WLAN Header with source address */
            oal_set_mac_addr(puc_ether_payload + 8, pst_vap->st_vap_base_info.pst_mib_info->st_wlan_mib_sta_config.auc_dot11StationID);
        }
    }

    pst_tx_ctl->us_tx_user_idx = us_user_idx;

    ul_ret = hmac_tx_ucast_process(pst_vap, pst_buf, pst_user, pst_tx_ctl);
    if (OAL_UNLIKELY(HMAC_TX_PASS != ul_ret))
    {
        return ul_ret;
    }

    /* 封装802.11头 */
    ul_ret = hmac_tx_encap(pst_vap, pst_user, pst_buf);
    if (OAL_UNLIKELY((OAL_SUCC != ul_ret) ))
    {
        OAM_WARNING_LOG1(pst_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX, "{hmac_tx_lan_mpdu_process_sta::hmac_tx_encap failed[%d].}", ul_ret);
        OAM_STAT_VAP_INCR(pst_vap->st_vap_base_info.uc_vap_id, tx_abnormal_msdu_dropped, 1);
        return HMAC_TX_DROP_80211_ENCAP_FAIL;
    }

    OAL_MIPS_TX_STATISTIC(HMAC_PROFILING_FUNC_ENCAP_HEAD);
    return HMAC_TX_PASS;
}

#ifdef _PRE_WLAN_FEATURE_PROXY_ARP

OAL_STATIC OAL_INLINE oal_bool_enum_uint8  hmac_tx_proxyarp_is_en(hmac_vap_stru *pst_vap)
{

   return (OAL_PTR_NULL != pst_vap->st_vap_base_info.pst_vap_proxyarp)
           && (OAL_TRUE == pst_vap->st_vap_base_info.pst_vap_proxyarp->en_is_proxyarp);

}
#endif


OAL_STATIC OAL_INLINE oal_uint32  hmac_tx_lan_mpdu_process_ap(
                hmac_vap_stru      *pst_vap,
                oal_netbuf_stru    *pst_buf,
                mac_tx_ctl_stru    *pst_tx_ctl)
{
    hmac_user_stru          *pst_user;      /* 目标STA结构体 */
    mac_ether_header_stru   *pst_ether_hdr; /* 以太网头 */
    oal_uint8               *puc_addr;      /* 目的地址 */
    oal_uint32               ul_ret;
    oal_uint16               us_user_idx;

    /* 判断是组播或单播,对于lan to wlan的单播帧，返回以太网地址 */
    pst_ether_hdr   = (mac_ether_header_stru *)oal_netbuf_data(pst_buf);
    puc_addr        = pst_ether_hdr->auc_ether_dhost;

#ifdef _PRE_WLAN_FEATURE_PROXY_ARP
    /*确认proxy arp 是否使能*/
    if (OAL_TRUE == hmac_tx_proxyarp_is_en(pst_vap))
    {
        if(OAL_TRUE == hmac_proxy_arp_proc(pst_vap, pst_buf))
        {
            return HMAC_TX_DROP_PROXY_ARP;
        }
    }
#endif

    /*单播数据帧*/
#ifdef _PRE_WLAN_CHIP_TEST
    if (OAL_LIKELY(!ETHER_IS_MULTICAST(puc_addr)) && pst_vap->st_vap_base_info.bit_al_tx_flag != OAL_SWITCH_ON)
#else
    if (OAL_LIKELY(!ETHER_IS_MULTICAST(puc_addr)))
#endif
    {
        ul_ret = mac_vap_find_user_by_macaddr(&(pst_vap->st_vap_base_info), puc_addr, &us_user_idx);
        if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
        {
            OAM_WARNING_LOG4(pst_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX,
                             "{hmac_tx_lan_mpdu_process_ap::hmac_tx_find_user failed %2x:%2x:%2x:%2x}", puc_addr[2], puc_addr[3], puc_addr[4], puc_addr[5]);
            OAM_STAT_VAP_INCR(pst_vap->st_vap_base_info.uc_vap_id, tx_abnormal_msdu_dropped, 1);
            return HMAC_TX_DROP_USER_UNKNOWN;
        }

        /* 转成HMAC的USER结构体 */
        pst_user = (hmac_user_stru *)mac_res_get_hmac_user(us_user_idx);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_user))
        {
            OAM_STAT_VAP_INCR(pst_vap->st_vap_base_info.uc_vap_id, tx_abnormal_msdu_dropped, 1);
            return HMAC_TX_DROP_USER_NULL;
        }

        /* 用户状态判断 */
        if (OAL_UNLIKELY(MAC_USER_STATE_ASSOC != pst_user->st_user_base_info.en_user_asoc_state))
        {
             OAM_STAT_VAP_INCR(pst_vap->st_vap_base_info.uc_vap_id, tx_abnormal_msdu_dropped, 1);
             return HMAC_TX_DROP_USER_INACTIVE;
        }

        /* 目标user指针 */
        pst_tx_ctl->us_tx_user_idx = us_user_idx;

        ul_ret = hmac_tx_ucast_process(pst_vap, pst_buf, pst_user, pst_tx_ctl);
        if (OAL_UNLIKELY(HMAC_TX_PASS != ul_ret))
        {
            return ul_ret;
        }
    }
    else /* 组播 or 广播 */
    {
        /* 设置组播标识位 */
        pst_tx_ctl->en_ismcast = OAL_TRUE;

        /* 更新ACK策略 */
        pst_tx_ctl->en_ack_policy = WLAN_TX_NO_ACK;

        /* 获取组播用户 */
        pst_user = mac_res_get_hmac_user(pst_vap->st_vap_base_info.us_multi_user_idx);

        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_user))
        {
            OAM_WARNING_LOG1(pst_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX,
                                 "{hmac_tx_lan_mpdu_process_ap::get multi user failed[%d].}", pst_vap->st_vap_base_info.us_multi_user_idx);
            OAM_STAT_VAP_INCR(pst_vap->st_vap_base_info.uc_vap_id, tx_abnormal_msdu_dropped, 1);
            return HMAC_TX_DROP_MUSER_NULL;
        }

        pst_tx_ctl->us_tx_user_idx = pst_vap->st_vap_base_info.us_multi_user_idx;
        pst_tx_ctl->uc_tid  = WLAN_TIDNO_BCAST;

        pst_tx_ctl->uc_ac   = WLAN_WME_TID_TO_AC(pst_tx_ctl->uc_tid);

        #ifdef _PRE_WLAN_FEATURE_MCAST /* 组播转单播 */
            if ((ETHER_IS_IPV4_MULTICAST(puc_addr)) && (!ETHER_IS_BROADCAST(puc_addr)) && (OAL_PTR_NULL != pst_vap->pst_m2u))
            {
                ul_ret = hmac_m2u_snoop_convert(pst_vap, pst_buf);
                if (ul_ret != HMAC_TX_PASS)
                {
                    return ul_ret;
                }
            }
        #endif
    }

    /* 封装802.11头 */
    ul_ret = hmac_tx_encap(pst_vap, pst_user, pst_buf);
    if (OAL_UNLIKELY((OAL_SUCC != ul_ret) ))
    {
        OAM_WARNING_LOG1(pst_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX,
                             "{hmac_tx_lan_mpdu_process_ap::hmac_tx_encap failed[%d].}", ul_ret);
        OAM_STAT_VAP_INCR(pst_vap->st_vap_base_info.uc_vap_id, tx_abnormal_msdu_dropped, 1);
        return HMAC_TX_DROP_80211_ENCAP_FAIL;
    }

    return HMAC_TX_PASS;
}

oal_void hmac_tx_vip_mpdu_info(mac_vap_stru *pst_vap, oal_uint8 uc_data_type, oal_netbuf_stru *pst_buf)
{
#ifndef WIN32
    mac_ether_header_stru *pst_ether_head = (mac_ether_header_stru *)oal_netbuf_data(pst_buf);
    oal_eth_arphdr_stru   *pst_arp_head   = OAL_PTR_NULL;

    if (uc_data_type == MAC_DATA_ARP_REQ || uc_data_type == MAC_DATA_ARP_RSP) {
        pst_arp_head = (oal_eth_arphdr_stru*)(pst_ether_head + 1);
        OAM_WARNING_LOG4(pst_vap->uc_vap_id, OAM_SF_ANY, "{hmac_tx_vip_mpdu_info:: ARP sip: %d.%d, dip: %d.%d",
            pst_arp_head->auc_ar_sip[2],pst_arp_head->auc_ar_sip[3],
            pst_arp_head->auc_ar_tip[2],pst_arp_head->auc_ar_tip[3]);
    }
#endif
}

OAL_INLINE oal_uint32  hmac_tx_lan_to_wlan_no_tcp_opt(mac_vap_stru *pst_vap, oal_netbuf_stru *pst_buf)
{
    frw_event_stru        *pst_event;        /* 事件结构体 */
    frw_event_mem_stru    *pst_event_mem;
    hmac_vap_stru         *pst_hmac_vap;     /* VAP结构体 */
    mac_tx_ctl_stru       *pst_tx_ctl;       /* SKB CB */
    oal_uint32             ul_ret = HMAC_TX_PASS;
    dmac_tx_event_stru    *pst_dtx_stru;
    oal_uint8              uc_data_type;
#ifdef _PRE_WLAN_FEATURE_WAPI
    hmac_user_stru              *pst_user;
    hmac_wapi_stru              *pst_wapi;
    mac_ieee80211_frame_stru    *pst_mac_hdr;
    oal_bool_enum_uint8          en_is_mcast = OAL_FALSE;
#endif

    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_vap->uc_vap_id);

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_hmac_vap))
    {
        OAM_ERROR_LOG0(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_tx_lan_to_wlan::pst_hmac_vap null.}");
        OAM_STAT_VAP_INCR(pst_vap->uc_vap_id, tx_abnormal_msdu_dropped, 1);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* VAP模式判断 */
    if (!IS_LEGACY_AP_OR_STA(pst_vap))
    {
        OAM_WARNING_LOG1(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_tx_lan_to_wlan::en_vap_mode=%d.}", pst_vap->en_vap_mode);
        OAM_STAT_VAP_INCR(pst_vap->uc_vap_id, tx_abnormal_msdu_dropped, 1);
        return OAL_ERR_CODE_CONFIG_UNSUPPORT;
    }

    /* 如果关联用户数量为0，则丢弃报文 */
    if (OAL_UNLIKELY(0 == pst_hmac_vap->st_vap_base_info.us_user_nums))
    {
        OAM_STAT_VAP_INCR(pst_vap->uc_vap_id, tx_abnormal_msdu_dropped, 1);
        return OAL_FAIL;
    }
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    /* 发数据，只发一次，避免反复申请tx描述符地址 */
    if (OAL_SWITCH_ON == pst_vap->bit_al_tx_flag)
    {
        if (OAL_FALSE != pst_vap->bit_first_run)
        {
            return OAL_SUCC;
        }
        mac_vap_set_al_tx_first_run(pst_vap, OAL_TRUE);
    }
#endif

    /* 初始化CB tx rx字段 , CB字段在前面已经被清零， 在这里不需要重复对某些字段赋零值*/
    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_buf);
    pst_tx_ctl->uc_mpdu_num               = 1;
    pst_tx_ctl->uc_netbuf_num             = 1;
    pst_tx_ctl->en_frame_type             = WLAN_DATA_BASICTYPE;
    pst_tx_ctl->en_is_probe_data          = DMAC_USER_ALG_NON_PROBE;
    pst_tx_ctl->en_ack_policy             = WLAN_TX_NORMAL_ACK;
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    pst_tx_ctl->en_ack_policy             = pst_hmac_vap->bit_ack_policy;
#endif
    pst_tx_ctl->uc_tx_vap_index           = pst_vap->uc_vap_id;
    pst_tx_ctl->us_tx_user_idx            = 0xffff;
    pst_tx_ctl->uc_ac                     = WLAN_WME_AC_BE;                  /* 初始化入BE队列 */

    /* 由于LAN TO WLAN和WLAN TO WLAN的netbuf都走这个函数，为了区分，需要先判断
       到底是哪里来的netbuf然后再对CB的事件类型字段赋值
    */
    if (FRW_EVENT_TYPE_WLAN_DTX != pst_tx_ctl->en_event_type)
    {
        pst_tx_ctl->en_event_type             = FRW_EVENT_TYPE_HOST_DRX;
        pst_tx_ctl->uc_event_sub_type         = DMAC_TX_HOST_DRX;
    }

    /* 此处数据可能从内核而来，也有可能由dev报上来再通过空口转出去，注意一下 */
    uc_data_type =  mac_get_data_type_from_8023((oal_uint8 *)oal_netbuf_data(pst_buf), MAC_NETBUFF_PAYLOAD_ETH);
    /* 维测，输出一个关键帧打印 */
    if (uc_data_type != MAC_DATA_UNSPEC && uc_data_type < MAC_DATA_VIP)
    {
        pst_tx_ctl->bit_is_vipframe  = OAL_TRUE;
        OAM_WARNING_LOG2(pst_vap->uc_vap_id, OAM_SF_TX,
                            "{hmac_tx_lan_to_wlan_no_tcp_opt::send datatype==%u, len==%u}[1:dhcp 2:arp_req 3:arp_rsp 4:eapol]",
                            uc_data_type, OAL_NETBUF_LEN(pst_buf));
        hmac_tx_vip_mpdu_info(pst_vap, uc_data_type, pst_buf);
    }

    oal_spin_lock_bh(&pst_hmac_vap->st_lock_state);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /*发数据包计数统计*/
    hmac_wifi_statistic_tx_packets(OAL_NETBUF_LEN(pst_buf));
#endif

    if (WLAN_VAP_MODE_BSS_AP == pst_hmac_vap->st_vap_base_info.en_vap_mode)
    {
        /*  处理当前 MPDU */
        if( OAL_FALSE == pst_vap->pst_mib_info->st_wlan_mib_sta_config.en_dot11QosOptionImplemented )
        {
            pst_tx_ctl->uc_ac                     = WLAN_WME_AC_VO;            /*AP模式 关WMM 入VO队列 */
            pst_tx_ctl->uc_tid =  WLAN_WME_AC_TO_TID(pst_tx_ctl->uc_ac);
        }

        ul_ret = hmac_tx_lan_mpdu_process_ap(pst_hmac_vap, pst_buf, pst_tx_ctl);
    }
    else if (WLAN_VAP_MODE_BSS_STA == pst_hmac_vap->st_vap_base_info.en_vap_mode)
    {
        /* 处理当前MPDU */
        pst_tx_ctl->uc_ac                     = WLAN_WME_AC_VO;                  /* STA模式 非qos帧入VO队列 */
        pst_tx_ctl->uc_tid =  WLAN_WME_AC_TO_TID(pst_tx_ctl->uc_ac);

        OAL_MIPS_TX_STATISTIC(HMAC_PROFILING_FUNC_CB_INIT);
        ul_ret = hmac_tx_lan_mpdu_process_sta(pst_hmac_vap, pst_buf, pst_tx_ctl);

#ifdef _PRE_WLAN_FEATURE_WAPI
        if(HMAC_TX_PASS == ul_ret)
            //&& OAL_UNLIKELY(WAPI_IS_WORK(pst_hmac_vap)))
        {
            pst_user = (hmac_user_stru *)mac_res_get_hmac_user(pst_vap->uc_assoc_vap_id);
            if (OAL_PTR_NULL == pst_user)
            {
                OAM_STAT_VAP_INCR(pst_vap->uc_vap_id, tx_abnormal_msdu_dropped, 1);
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "hmac_tx_lan_to_wlan_no_tcp_opt::usrid==%u no usr!}", pst_vap->uc_assoc_vap_id);
                oal_spin_unlock_bh(&pst_hmac_vap->st_lock_state);
                return HMAC_TX_DROP_USER_NULL;
            }

            /* 获取wapi对象 组播/单播 */
            pst_mac_hdr = ((mac_tx_ctl_stru *)OAL_NETBUF_CB(pst_buf))->pst_frame_header;
            en_is_mcast = ETHER_IS_MULTICAST(pst_mac_hdr->auc_address1);
/*lint -e730*/
            pst_wapi = hmac_user_get_wapi_ptr(pst_vap,
                                              !en_is_mcast,
                                              pst_user->st_user_base_info.us_assoc_id);
/*lint +e730*/

            if (OAL_PTR_NULL == pst_wapi)
            {
                OAM_ERROR_LOG1(pst_vap->uc_vap_id, OAM_SF_ANY, "{hmac_tx_lan_to_wlan_no_tcp_opt_etc::pst_wapi null, %x}", (uintptr_t)pst_wapi);
                oal_spin_unlock_bh(&pst_hmac_vap->st_lock_state);
                return OAL_ERR_CODE_PTR_NULL;
            }
            if ( (OAL_TRUE == WAPI_IS_PORT_VALID(pst_wapi))
                    && (OAL_PTR_NULL != pst_wapi->wapi_netbuff_txhandle))
            {
                pst_buf = pst_wapi->wapi_netbuff_txhandle(pst_wapi, pst_buf);
                if (pst_buf == OAL_PTR_NULL)
                {
                    OAM_STAT_VAP_INCR(pst_vap->uc_vap_id, tx_abnormal_msdu_dropped, 1);
                    OAM_WARNING_LOG0(pst_vap->uc_vap_id, OAM_SF_ANY, "{hmac_tx_lan_to_wlan_no_tcp_opt_etc:: wapi_netbuff_txhandle fail!}");
                    oal_spin_unlock_bh(&pst_hmac_vap->st_lock_state);
                    return OAL_ERR_CODE_PTR_NULL;
                }
                /*  由于wapi可能修改netbuff，此处需要重新获取一下cb */
                pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_buf);
            }
        }

#endif /* #ifdef _PRE_WLAN_FEATURE_WAPI */

    }

    oal_spin_unlock_bh(&pst_hmac_vap->st_lock_state);

    if (OAL_LIKELY(HMAC_TX_PASS == ul_ret))
    {
        /* 抛事件，传给DMAC */
        pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_tx_event_stru));
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
        {
            OAM_ERROR_LOG0(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_tx_lan_to_wlan::FRW_EVENT_ALLOC failed.}");
            return OAL_ERR_CODE_ALLOC_MEM_FAIL;
        }

        pst_event = (frw_event_stru *)pst_event_mem->puc_data;

        /* 填写事件头 */
        FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                            FRW_EVENT_TYPE_HOST_DRX,
                            DMAC_TX_HOST_DRX,
                            OAL_SIZEOF(dmac_tx_event_stru),
                            FRW_EVENT_PIPELINE_STAGE_1,
                            pst_vap->uc_chip_id,
                            pst_vap->uc_device_id,
                            pst_vap->uc_vap_id);

        pst_dtx_stru             = (dmac_tx_event_stru *)pst_event->auc_event_data;
        pst_dtx_stru->pst_netbuf = pst_buf;

        pst_dtx_stru->us_frame_len = pst_tx_ctl->us_mpdu_len;

        /* 调度事件 */
        ul_ret = frw_event_dispatch_event(pst_event_mem);
        if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
        {
            OAM_WARNING_LOG1(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_tx_lan_to_wlan::frw_event_dispatch_event failed[%d].}", ul_ret);
            OAM_STAT_VAP_INCR(pst_vap->uc_vap_id, tx_abnormal_msdu_dropped, 1);
        }

         /* 释放事件 */
        FRW_EVENT_FREE(pst_event_mem);

        OAL_MIPS_TX_STATISTIC(HMAC_PROFILING_FUNC_TX_EVENT_TO_DMAC);
#ifdef _PRE_WLAN_PROFLING_MIPS
        oal_profiling_stop_tx_save();
#endif
    }
    else if (OAL_UNLIKELY(HMAC_TX_BUFF == ul_ret))
    {
        ul_ret = OAL_SUCC;
    }
    else if ((HMAC_TX_DONE == ul_ret))
    {
        ul_ret = OAL_SUCC;
    }
    else if ((HMAC_TX_DROP_MTOU_FAIL == ul_ret))
    {
        /* 组播报文没有对应的STA可以转成单播，所以丢弃，属正常行为 */
        OAM_INFO_LOG1(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_tx_lan_to_wlan_no_tcp_opt::HMAC_TX_DROP.reason[%d]!}", ul_ret);
    }
    else
    {
        OAM_WARNING_LOG1(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_tx_lan_to_wlan_no_tcp_opt::HMAC_TX_DROP.reason[%d]!}", ul_ret);
    }

    return ul_ret;
}

#ifdef _PRE_WLAN_TCP_OPT
OAL_STATIC  oal_uint32 hmac_transfer_tx_handler(hmac_device_stru * hmac_device,hmac_vap_stru * hmac_vap,oal_netbuf_stru* netbuf)
{
    oal_uint32              ul_ret      = OAL_SUCC;
#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
    if(WLAN_TCP_ACK_QUEUE == oal_netbuf_select_queue(netbuf))
    {
#ifdef _PRE_WLAN_TCP_OPT_DEBUG
        OAM_WARNING_LOG0(0, OAM_SF_TX,
                             "{hmac_transfer_tx_handler::netbuf is tcp ack.}\r\n");
#endif
        oal_spin_lock_bh(&hmac_vap->ast_hmac_tcp_ack[HCC_TX].data_queue_lock[HMAC_TCP_ACK_QUEUE]);
        oal_netbuf_list_tail(&hmac_vap->ast_hmac_tcp_ack[HCC_TX].data_queue[HMAC_TCP_ACK_QUEUE], netbuf);

        /* 单纯TCP ACK等待调度, 特殊报文马上发送 */
        if(hmac_judge_tx_netbuf_is_tcp_ack((oal_ether_header_stru *)oal_netbuf_data(netbuf)))
        {
            oal_spin_unlock_bh(&hmac_vap->ast_hmac_tcp_ack[HCC_TX].data_queue_lock[HMAC_TCP_ACK_QUEUE]);
            hmac_sched_transfer();
        }
        else
        {
            oal_spin_unlock_bh(&hmac_vap->ast_hmac_tcp_ack[HCC_TX].data_queue_lock[HMAC_TCP_ACK_QUEUE]);
            hmac_tcp_ack_process();
        }
    }
    else
    {
        ul_ret = hmac_tx_lan_to_wlan_no_tcp_opt(&(hmac_vap->st_vap_base_info),netbuf);
    }
#endif
    return ul_ret;
}
#endif


oal_uint32  hmac_tx_wlan_to_wlan_ap(oal_mem_stru *pst_event_mem)
{
    frw_event_stru         *pst_event;        /* 事件结构体 */
    mac_vap_stru           *pst_mac_vap;
    oal_netbuf_stru        *pst_buf;          /* 从netbuf链上取下来的指向netbuf的指针 */
    oal_netbuf_stru        *pst_buf_tmp;      /* 暂存netbuf指针，用于while循环 */
    mac_tx_ctl_stru        *pst_tx_ctl;
    oal_uint32              ul_ret;

    /* 入参判断 */
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_tx_wlan_to_wlan_ap::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* 获取事件 */
    pst_event = (frw_event_stru *)pst_event_mem->puc_data;
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event))
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_tx_wlan_to_wlan_ap::pst_event null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 获取PAYLOAD中的netbuf链 */
    pst_buf = (oal_netbuf_stru *)(uintptr_t)(*((oal_uint *)(pst_event->auc_event_data)));

    ul_ret = hmac_tx_get_mac_vap(pst_event->st_event_hdr.uc_vap_id, &pst_mac_vap);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_ERROR_LOG1(pst_event->st_event_hdr.uc_vap_id, OAM_SF_TX, "{hmac_tx_wlan_to_wlan_ap::hmac_tx_get_mac_vap failed[%d].}", ul_ret);
    #ifdef _PRE_DEBUG_MODE
        hmac_free_netbuf_list(pst_buf);
    #else
        hmac_free_netbuf_list(pst_buf);
    #endif
        return ul_ret;
    }

    /* 循环处理每一个netbuf，按照以太网帧的方式处理 */
    while (OAL_PTR_NULL != pst_buf)
    {
        pst_buf_tmp = OAL_NETBUF_NEXT(pst_buf);

        OAL_NETBUF_NEXT(pst_buf) = OAL_PTR_NULL;
        OAL_NETBUF_PREV(pst_buf) = OAL_PTR_NULL;

        
        pst_tx_ctl = (mac_tx_ctl_stru *)OAL_NETBUF_CB(pst_buf);
        OAL_MEMZERO(pst_tx_ctl, sizeof(mac_tx_ctl_stru));

        pst_tx_ctl->en_event_type = FRW_EVENT_TYPE_WLAN_DTX;
        pst_tx_ctl->uc_event_sub_type = DMAC_TX_WLAN_DTX;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        /*set the queue map id when wlan to wlan*/
        oal_skb_set_queue_mapping(pst_buf, WLAN_NORMAL_QUEUE);
#endif

        ul_ret = hmac_tx_lan_to_wlan(pst_mac_vap, pst_buf);

        /* 调用失败，自己调用自己释放netbuff内存 */
        if(OAL_SUCC != ul_ret)
        {
            hmac_free_netbuf_list(pst_buf);
        }

        pst_buf = pst_buf_tmp;
    }

    return OAL_SUCC;
}


OAL_INLINE oal_uint32  hmac_tx_lan_to_wlan(mac_vap_stru *pst_vap, oal_netbuf_stru *pst_buf)
{
    oal_uint32             ul_ret = HMAC_TX_PASS;
#ifdef _PRE_WLAN_TCP_OPT
    hmac_device_stru    *pst_hmac_device;
    hmac_vap_stru         *pst_hmac_vap;     /* VAP结构体 */
#endif


#ifdef _PRE_WLAN_TCP_OPT
    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_vap->uc_vap_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_hmac_vap))
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_tx_lan_to_wlan_tcp_opt::pst_dmac_vap null.}\r\n");
        return OAL_FAIL;
    }
    pst_hmac_device = hmac_res_get_mac_dev(pst_vap->uc_device_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_hmac_device))
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX, "{hmac_tx_lan_to_wlan_tcp_opt::pst_hmac_device null.}\r\n");
        return OAL_FAIL;
    }
    if(OAL_TRUE == pst_hmac_device->sys_tcp_tx_ack_opt_enable)
    {
        ul_ret = hmac_transfer_tx_handler(pst_hmac_device,pst_hmac_vap,pst_buf);
    }
    else
 #endif
    {
        ul_ret = hmac_tx_lan_to_wlan_no_tcp_opt(pst_vap, pst_buf);
    }
    return ul_ret;
}


oal_bool_enum_uint8 hmac_bridge_vap_should_drop(oal_netbuf_stru *pst_buf, mac_vap_stru *pst_vap)
{
    oal_bool_enum_uint8 en_drop_frame = OAL_FALSE;
    oal_uint8           uc_data_type;
    /* 入参在外面已经判空，这里不用判空 */
    /* 入网过程中触发p2p扫描不丢dhcp、eapol帧，防止入网失败 */
    if (MAC_VAP_STATE_STA_LISTEN != pst_vap->en_vap_state)
    {
        en_drop_frame = OAL_TRUE;
    }
    else
    {
        uc_data_type =  mac_get_data_type_from_8023((oal_uint8 *)oal_netbuf_payload(pst_buf), MAC_NETBUFF_PAYLOAD_ETH);
        if ((MAC_DATA_EAPOL != uc_data_type) && (MAC_DATA_DHCP != uc_data_type))
        {
            en_drop_frame = OAL_TRUE;
        }
        else
        {
            OAM_WARNING_LOG2(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_bridge_vap_should_drop::donot drop [%d]frame[EAPOL:1,DHCP:0]. vap state[%d].}",uc_data_type, pst_vap->en_vap_state);
        }
    }
    return en_drop_frame;
}

#define MAC_VAP_IS_NOT_WORK  (OAL_UNLIKELY(!((MAC_VAP_STATE_UP == pst_vap->en_vap_state) || (MAC_VAP_STATE_PAUSE == pst_vap->en_vap_state))))

oal_net_dev_tx_enum  hmac_bridge_vap_xmit(oal_netbuf_stru *pst_buf, oal_net_device_stru *pst_dev)
{
    mac_vap_stru                *pst_vap;
    hmac_vap_stru               *pst_hmac_vap;
    oal_uint32                   ul_ret;
#ifdef _PRE_WLAN_FEATURE_ROAM
    oal_uint8                    uc_data_type;
#endif

#if defined(_PRE_WLAN_FEATURE_PROXYSTA) ||  defined(_PRE_WLAN_FEATURE_ALWAYS_TX)
    mac_device_stru  *pst_mac_device;
#endif
    oal_bool_enum_uint8          en_drop_frame = OAL_FALSE;


    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_buf))
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_bridge_vap_xmit::pst_buf = OAL_PTR_NULL!}\r\n");
        return OAL_NETDEV_TX_OK;
    }

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_bridge_vap_xmit::pst_dev = OAL_PTR_NULL!}\r\n");
        oal_netbuf_free(pst_buf);
        OAM_STAT_VAP_INCR(0, tx_abnormal_msdu_dropped, 1);
        return OAL_NETDEV_TX_OK;
    }

    /* 获取VAP结构体 */
    pst_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_dev);

    /* 如果VAP结构体不存在，则丢弃报文 */
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_vap))
    {
        /* will find vap fail when receive a pkt from
         * kernel while vap is deleted, return OAL_NETDEV_TX_OK is so. */
        OAM_WARNING_LOG0(0, OAM_SF_TX, "{hmac_bridge_vap_xmit::pst_vap = OAL_PTR_NULL!}\r\n");
        oal_netbuf_free(pst_buf);
        OAM_STAT_VAP_INCR(0, tx_abnormal_msdu_dropped, 1);
        return OAL_NETDEV_TX_OK;
    }

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    // pst_vap maybe changed by proxysta
    if (mac_vap_is_msta(pst_vap) || mac_vap_is_vsta(pst_vap))
    {
        if (OAL_SUCC != hmac_psta_tx_process(pst_buf, &pst_vap))
        {
            oal_netbuf_free(pst_buf);
            return OAL_NETDEV_TX_OK;
        }
    }
#endif

    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_vap->uc_vap_id, OAM_SF_CFG, "{hmac_bridge_vap_xmit::pst_hmac_vap null.}");
        oal_netbuf_free(pst_buf);
        return OAL_NETDEV_TX_OK;
    }

#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX

    pst_mac_device = mac_res_get_dev(pst_vap->uc_device_id);
    if ( OAL_PTR_NULL == pst_mac_device)
    {
        OAM_ERROR_LOG0(pst_vap->uc_vap_id, OAM_SF_PROXYSTA, "{hmac_bridge_vap_xmit::mac_res_get_dev is null!}");
        oal_netbuf_free(pst_buf);

        return OAL_NETDEV_TX_OK;
    }

    /*代码待整改，pst_device_stru指针切换未状态, 长发长收切换未本地状态*/
    if ((OAL_SWITCH_ON == pst_vap->bit_al_tx_flag)
    || ((pst_mac_device->pst_device_stru != OAL_PTR_NULL) && (HAL_ALWAYS_TX_AMPDU_ENABLE == pst_mac_device->pst_device_stru->bit_al_tx_flag)))
    //|| (HAL_ALWAYS_RX_DISABLE != pst_mac_device->pst_device_stru->bit_al_rx_flag))
    {
        OAM_INFO_LOG0(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_bridge_vap_xmit::the vap alway tx/rx!}\r\n");
        oal_netbuf_free(pst_buf);
        return OAL_NETDEV_TX_OK;
    }
#endif

    pst_buf = oal_netbuf_unshare(pst_buf, GFP_ATOMIC);
    if (OAL_UNLIKELY(pst_buf == OAL_PTR_NULL))
    {
        OAM_INFO_LOG0(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_bridge_vap_xmit::the unshare netbuf = OAL_PTR_NULL!}\r\n");
        return OAL_NETDEV_TX_OK;
    }

    /* 统计以太网下来的数据包统计 */
    HMAC_VAP_DFT_STATS_PKT_INCR(pst_hmac_vap->st_query_stats.ul_tx_pkt_num_from_lan, 1);
    HMAC_VAP_DFT_STATS_PKT_INCR(pst_hmac_vap->st_query_stats.ul_tx_bytes_from_lan, OAL_NETBUF_LEN(pst_buf));
    OAM_STAT_VAP_INCR(pst_vap->uc_vap_id, tx_pkt_num_from_lan, 1);
    OAM_STAT_VAP_INCR(pst_vap->uc_vap_id, tx_bytes_from_lan, OAL_NETBUF_LEN(pst_buf));








    if(OAL_GET_THRUPUT_BYPASS_ENABLE(OAL_TX_LINUX_BYPASS))
    {
        oal_netbuf_free(pst_buf);
        return OAL_NETDEV_TX_OK;
    }

    /* 考虑VAP状态与控制面互斥，需要加锁保护 */
    oal_spin_lock_bh(&pst_hmac_vap->st_lock_state);

    /* 判断VAP的状态，如果ROAM，则丢弃报文 MAC_DATA_DHCP/MAC_DATA_ARP */
#ifdef _PRE_WLAN_FEATURE_ROAM
    if(MAC_VAP_STATE_ROAMING == pst_vap->en_vap_state)
    {
        uc_data_type =  mac_get_data_type_from_8023((oal_uint8 *)oal_netbuf_payload(pst_buf), MAC_NETBUFF_PAYLOAD_ETH);
        if(MAC_DATA_EAPOL != uc_data_type)
        {
            oal_netbuf_free(pst_buf);
            oal_spin_unlock_bh(&pst_hmac_vap->st_lock_state);
            return OAL_NETDEV_TX_OK;
        }
    }
    else
    {
#endif  //_PRE_WLAN_FEATURE_ROAM
    /* 判断VAP的状态，如果没有UP/PAUSE，则丢弃报文 */
    if (MAC_VAP_IS_NOT_WORK)
    {

        en_drop_frame = hmac_bridge_vap_should_drop(pst_buf, pst_vap);
        if (OAL_TRUE == en_drop_frame)
        {
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
            /*filter the tx xmit pkts print*/
            if(MAC_VAP_STATE_INIT == pst_vap->en_vap_state || MAC_VAP_STATE_BUTT == pst_vap->en_vap_state)
            {
                OAM_WARNING_LOG1(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_bridge_vap_xmit::vap state[%d] != MAC_VAP_STATE_{UP|PAUSE}!}\r\n", pst_vap->en_vap_state);
            }
            else
            {
                OAM_INFO_LOG1(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_bridge_vap_xmit::vap state[%d] != MAC_VAP_STATE_{UP|PAUSE}!}\r\n", pst_vap->en_vap_state);
            }
#else
            OAM_WARNING_LOG1(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_bridge_vap_xmit::vap state[%d] != MAC_VAP_STATE_{UP|PAUSE}!}\r\n", pst_vap->en_vap_state);
#endif
            oal_netbuf_free(pst_buf);
            OAM_STAT_VAP_INCR(pst_vap->uc_vap_id, tx_abnormal_msdu_dropped, 1);

            oal_spin_unlock_bh(&pst_hmac_vap->st_lock_state);
            return OAL_NETDEV_TX_OK;
        }
    }
#ifdef _PRE_WLAN_FEATURE_ROAM
    }
#endif

    OAL_NETBUF_NEXT(pst_buf) = OAL_PTR_NULL;
    OAL_NETBUF_PREV(pst_buf) = OAL_PTR_NULL;

    OAL_MEMZERO(OAL_NETBUF_CB(pst_buf), OAL_NETBUF_CB_SIZE());

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    /* ARP、DHCP、ICMPv6等数据包的地址转换 (只有proxy STA的发送的包才需要地址转换)*/
    if (mac_vap_is_vsta(pst_vap))
    {
        ul_ret = hmac_psta_tx_mat(pst_buf, pst_hmac_vap);
        if (OAL_SUCC != ul_ret)
        {
            oal_netbuf_free(pst_buf);
            oal_spin_unlock_bh(&pst_hmac_vap->st_lock_state);
            return OAL_NETDEV_TX_OK;
        }
    }
#endif

#ifdef _PRE_WLAN_FEATURE_BTCOEX
    /* 发送方向的arp_req 统计和删ba的处理 */
    hmac_btcoex_arp_fail_delba_process(pst_buf, &(pst_hmac_vap->st_vap_base_info));
#endif

    oal_spin_unlock_bh(&pst_hmac_vap->st_lock_state);
    OAL_MIPS_TX_STATISTIC(HMAC_PROFILING_FUNC_BRIDGE_VAP_XMIT);

    ul_ret = hmac_tx_lan_to_wlan(pst_vap, pst_buf);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        /* 调用失败，要释放内核申请的netbuff内存池 */
        oal_netbuf_free(pst_buf);
    }

    return OAL_NETDEV_TX_OK;
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
oal_uint8 g_tx_debug = 0;
oal_uint32 hmac_tx_data(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf)
{
    mac_vap_stru        *pst_vap = &(pst_hmac_vap->st_vap_base_info);
    oal_uint32           ul_ret;

    if(g_tx_debug)
    {
       OAL_IO_PRINT("hmac_tx_data start\n");
    }

#ifdef _PRE_WLAN_CHIP_TEST
    if (OAL_SWITCH_ON == pst_vap->bit_al_tx_flag)
    {
        OAM_INFO_LOG0(0, OAM_SF_TX, "{hmac_tx_data::the vap alway tx!}\r\n");
        oal_netbuf_free(pst_netbuf);
        return OAL_NETDEV_TX_OK;
    }
#endif

    /* 将以太网过来的帧上报SDT */
    hmac_tx_report_eth_frame(pst_vap, pst_netbuf);










    /* 考虑VAP状态与控制面互斥，需要加锁保护 */


     /* 判断VAP的状态，如果没有UP，则丢弃报文 */
    if (OAL_UNLIKELY(!((MAC_VAP_STATE_UP == pst_vap->en_vap_state) || (MAC_VAP_STATE_PAUSE == pst_vap->en_vap_state))))
    {
        OAM_WARNING_LOG1(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_tx_data::vap state[%d] != MAC_VAP_STATE_{UP|PAUSE}!}\r\n", pst_vap->en_vap_state);

        oal_netbuf_free(pst_netbuf);


        OAM_STAT_VAP_INCR(pst_vap->uc_vap_id, tx_abnormal_msdu_dropped, 1);

        return OAL_NETDEV_TX_OK;
    }

    OAL_NETBUF_NEXT(pst_netbuf) = OAL_PTR_NULL;
    OAL_NETBUF_PREV(pst_netbuf) = OAL_PTR_NULL;

    OAL_MEMZERO(OAL_NETBUF_CB(pst_netbuf), OAL_NETBUF_CB_SIZE());

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    /* ARP、DHCP、ICMPv6等数据包的地址转换 (只有proxy STA的发送的包才需要地址转换)*/
    if (mac_vap_is_vsta(pst_vap))
    {
        ul_ret = hmac_psta_tx_mat(pst_netbuf, pst_hmac_vap);

        if (OAL_SUCC != ul_ret)
        {
            OAM_ERROR_LOG0(0, OAM_SF_PROXYSTA, "{hmac_tx_data::hmac_tx_proxysta_mat fail.}");
            oal_netbuf_free(pst_netbuf);


            return OAL_NETDEV_TX_OK;
        }
    }
#endif

    ul_ret = hmac_tx_lan_to_wlan(pst_vap, pst_netbuf);

    /* 调用失败，要释放内核申请的netbuff内存池 */
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        hmac_free_netbuf_list(pst_netbuf);
    }



    return OAL_NETDEV_TX_OK;
}


OAL_STATIC oal_void  hmac_lock_resource(oal_spin_lock_stru *pst_lock, oal_uint *pui_flags)
{
#ifdef _PRE_WLAN_SPE_SUPPORT
    oal_spin_lock_irq_save(pst_lock, pui_flags);        //主要在E5产品上使用
#else
    oal_spin_lock_bh(pst_lock);
#endif
}

OAL_STATIC oal_void  hmac_unlock_resource(oal_spin_lock_stru *pst_lock, oal_uint *pui_flags)
{
#ifdef _PRE_WLAN_SPE_SUPPORT
    oal_spin_unlock_irq_restore(pst_lock, pui_flags);   //主要在E5产品上使用
#else
    oal_spin_unlock_bh(pst_lock);
#endif
}


oal_net_dev_tx_enum  hmac_vap_start_xmit(oal_netbuf_stru *pst_buf, oal_net_device_stru *pst_dev)
{
    mac_vap_stru   *pst_vap;
    hmac_vap_stru  *pst_hmac_vap;
    oal_ulong       ul_lock_flag;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_vap_start_xmit::pst_dev = OAL_PTR_NULL!}\r\n");
        oal_netbuf_free(pst_buf);
        OAM_STAT_VAP_INCR(0, tx_abnormal_msdu_dropped, 1);
        return OAL_NETDEV_TX_OK;
    }

    /* 获取VAP结构体 */
    pst_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_dev);

    /* 如果VAP结构体不存在，则丢弃报文 */
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_vap))
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_vap_start_xmit::pst_vap = OAL_PTR_NULL!}\r\n");
        oal_netbuf_free(pst_buf);
        OAM_STAT_VAP_INCR(0, tx_abnormal_msdu_dropped, 1);
        return OAL_NETDEV_TX_OK;
    }

    pst_buf = oal_netbuf_unshare(pst_buf, GFP_ATOMIC);
    if (pst_buf == OAL_PTR_NULL)
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_vap_start_xmit::the unshare netbuf = OAL_PTR_NULL!}\r\n");
        return OAL_NETDEV_TX_OK;
    }

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    // warning:pst_vap maybe changed in this function!!!
    if (mac_vap_is_msta(pst_vap) || mac_vap_is_vsta(pst_vap))
    {
        if (OAL_SUCC != hmac_psta_tx_process(pst_buf, &pst_vap))
        {
            oal_netbuf_free(pst_buf);
            return OAL_NETDEV_TX_OK;
        }
    }
#endif

    pst_hmac_vap = mac_res_get_hmac_vap(pst_vap->uc_vap_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_hmac_vap))
    {
        OAM_ERROR_LOG1(0, OAM_SF_TX, "{hmac_vap_start_xmit::pst_hmac_vap[%d] = OAL_PTR_NULL!}", pst_vap->uc_vap_id);
        oal_netbuf_free(pst_buf);
        OAM_STAT_VAP_INCR(0, tx_abnormal_msdu_dropped, 1);
        return OAL_NETDEV_TX_OK;
    }

    /* 防止下行来包太多，造成软件处理来不及，造成软件积累包太多，skb内存不能及时释放，入队限制修改为300，MIPS降低后，这个值可以抬高 */
    if(OAL_NETBUF_LIST_NUM(&pst_hmac_vap->st_tx_queue_head[pst_hmac_vap->uc_in_queue_id])>=300)
    {
        /* 关键帧做100个缓存，保证关键帧的正常发送 */
        if(OAL_NETBUF_LIST_NUM(&pst_hmac_vap->st_tx_queue_head[pst_hmac_vap->uc_in_queue_id]) < 400)
        {
            oal_uint8   uc_data_type;

            uc_data_type = mac_get_data_type_from_8023((oal_uint8 *)oal_netbuf_payload(pst_buf), MAC_NETBUFF_PAYLOAD_ETH);
            if((MAC_DATA_EAPOL == uc_data_type) || (MAC_DATA_DHCP == uc_data_type) || (MAC_DATA_ARP_REQ == uc_data_type)|| (MAC_DATA_ARP_RSP == uc_data_type))
            {
                hmac_lock_resource(&pst_hmac_vap->st_smp_lock, &ul_lock_flag);
                oal_netbuf_add_to_list_tail(pst_buf, &(pst_hmac_vap->st_tx_queue_head[pst_hmac_vap->uc_in_queue_id]));
                hmac_unlock_resource(&pst_hmac_vap->st_smp_lock, &ul_lock_flag);
            }
            else
            {
                oal_netbuf_free(pst_buf);
            }
        }
        else
        {
            oal_netbuf_free(pst_buf);
        }

        if(g_tx_debug)
        {
            /* 增加维测信息，把tx_event_num的值打印出来，用户关连不上，或者一直ping不通，打开g_tx_debug开关，如果此时的值不为1，就属于异常 */
            OAM_ERROR_LOG1(pst_vap->uc_vap_id, OAM_SF_TX, "{hmac_vap_start_xmit::tx_event_num value is [%d].}", (oal_int32)oal_atomic_read(&(pst_hmac_vap->ul_tx_event_num)));
            OAL_IO_PRINT("hmac_vap_start_xmit too fast\n");
        }
    }
    else
    {
        if(g_tx_debug)
            OAL_IO_PRINT("hmac_vap_start_xmit enqueue and post event\n");

        hmac_lock_resource(&pst_hmac_vap->st_smp_lock, &ul_lock_flag);
        oal_netbuf_add_to_list_tail(pst_buf, &(pst_hmac_vap->st_tx_queue_head[pst_hmac_vap->uc_in_queue_id]));
        hmac_unlock_resource(&pst_hmac_vap->st_smp_lock, &ul_lock_flag);
    }

    hmac_tx_post_event(pst_vap);

    return OAL_NETDEV_TX_OK;
}

oal_uint32 hmac_tx_post_event(mac_vap_stru *pst_mac_vap)
{
    oal_uint32              ul_ret;
    frw_event_stru          *pst_event;
    frw_event_mem_stru      *pst_event_mem;
    hmac_vap_stru           *pst_hmac_vap;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);

    /* tx_event_num减1后等于0表示eth to wlan数据事件为空，可以申请事件。同时这个方案能确保每次只有一个事件入队并处理，防止存在两个以上事件申请入队 */
    if(oal_atomic_dec_and_test(&(pst_hmac_vap->ul_tx_event_num)))
    {
        /* 申请事件内存 */
        pst_event_mem = FRW_EVENT_ALLOC(0);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
        {
            oal_atomic_inc(&(pst_hmac_vap->ul_tx_event_num)); /* 事件申请失败，tx_event_num要加回去，恢复成默认值1 */
            OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_TX, "{hmac_tx_post_event::pst_event_mem null.}");
            OAL_IO_PRINT("Hmac_tx_post_event fail to alloc event mem\n");
            return OAL_ERR_CODE_PTR_NULL;
        }

        pst_event = (frw_event_stru *)pst_event_mem->puc_data;

        /* 填写事件 */
        FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                           FRW_EVENT_TYPE_HOST_DRX,
                           HMAC_TX_HOST_DRX,
                           0,
                           FRW_EVENT_PIPELINE_STAGE_0,
                           pst_mac_vap->uc_chip_id,
                           pst_mac_vap->uc_device_id,
                           pst_mac_vap->uc_vap_id);


        /* 抛事入队列 */
        ul_ret = frw_event_post_event(pst_event_mem,pst_mac_vap->ul_core_id);
        if (OAL_SUCC != ul_ret)
        {
            oal_atomic_inc(&(pst_hmac_vap->ul_tx_event_num)); /* 事件入队失败，tx_event_num要加回去，恢复成默认值1 */
            OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_TX, "{hmac_tx_post_event::frw_event_dispatch_event failed[%d].}", ul_ret);
            OAL_IO_PRINT("{hmac_tx_post_event::frw_event_dispatch_event failed}\n");
        }

        /* 释放事件内存 */
        FRW_EVENT_FREE(pst_event_mem);
    }
    else
    {
        /* 事件队列中已经有事件要处理(tx_event_num值为0)，这个时候预先减去的值要加回去，tx_event_num要恢复成0 */
        oal_atomic_inc(&(pst_hmac_vap->ul_tx_event_num));

        if(g_tx_debug)
        {
            /* 增加维测信息，把tx_event_num的值打印出来，如果此时的值不为0，就属于异常(不申请事件，但是值又不为0) */
            OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_TX, "{hmac_tx_post_event::tx_event_num value is [%d].}", (oal_int32)oal_atomic_read(&(pst_hmac_vap->ul_tx_event_num)));
            OAL_IO_PRINT("do not post tx event, data in queue len %d,out queue len %d\n",
                            OAL_NETBUF_LIST_NUM(&pst_hmac_vap->st_tx_queue_head[pst_hmac_vap->uc_in_queue_id]),
                            OAL_NETBUF_LIST_NUM(&pst_hmac_vap->st_tx_queue_head[pst_hmac_vap->uc_out_queue_id]));
        }
    }

    return OAL_SUCC;
}

OAL_STATIC oal_uint8 hmac_vap_user_is_bw_limit(mac_vap_stru *pst_vap, oal_netbuf_stru *pst_netbuf)
{
    mac_ether_header_stru   *pst_ether_hdr; /* 以太网头 */
    oal_uint16               us_user_idx;
    hmac_user_stru          *pst_hmac_user;
    oal_uint32               ul_ret;

    pst_ether_hdr   = (mac_ether_header_stru *)oal_netbuf_data(pst_netbuf);
    ul_ret = mac_vap_find_user_by_macaddr(pst_vap, pst_ether_hdr->auc_ether_dhost, &us_user_idx);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        return OAL_FALSE;
    }
    pst_hmac_user    = (hmac_user_stru *)mac_res_get_hmac_user(us_user_idx);
    if (OAL_PTR_NULL == pst_hmac_user)
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    return (pst_vap->bit_vap_bw_limit || pst_hmac_user->en_user_bw_limit);
}

oal_uint32  hmac_tx_event_process(oal_mem_stru *pst_event_mem)
{
    frw_event_stru          *pst_event;
    hmac_vap_stru           *pst_hmac_vap;
    oal_uint32               ul_work = 0;
    oal_uint32               ul_reschedule = OAL_TRUE;
	oal_netbuf_stru         *pst_netbuf;

    /* 入参判断 */
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_tx_event_process::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 获取事件 */
    pst_event = (frw_event_stru *)pst_event_mem->puc_data;
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event))
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_tx_event_process::pst_event null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_vap = mac_res_get_hmac_vap(pst_event->st_event_hdr.uc_vap_id);
    if(OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_TX, "{hmac_tx_event_process::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if(g_tx_debug)
        OAL_IO_PRINT("hmac_tx_event_process start\n");

    /* 事件申请时在hmac_tx_post_event处理时已经减为了0，这里要加回去，恢复成1 */
    oal_atomic_inc(&(pst_hmac_vap->ul_tx_event_num));

    if(g_tx_debug)
        OAL_IO_PRINT("oal_atomic_dec OK\n");

    oal_spin_lock_bh(&pst_hmac_vap->st_smp_lock);
    if(0 == OAL_NETBUF_LIST_NUM(&pst_hmac_vap->st_tx_queue_head[pst_hmac_vap->uc_out_queue_id]))
    {
        /*如果当前out queue空了, 才切换out_queue_id*/
        pst_hmac_vap->uc_in_queue_id = pst_hmac_vap->uc_out_queue_id;
        pst_hmac_vap->uc_out_queue_id = (pst_hmac_vap->uc_out_queue_id + 1) & 1;

    }
    oal_spin_unlock_bh(&pst_hmac_vap->st_smp_lock);

    do
    {
        oal_spin_lock_bh(&pst_hmac_vap->st_smp_lock);
        pst_netbuf = oal_netbuf_delist(&pst_hmac_vap->st_tx_queue_head[pst_hmac_vap->uc_out_queue_id]);
        oal_spin_unlock_bh(&pst_hmac_vap->st_smp_lock);

        if (!pst_netbuf)
        {
            if(g_tx_debug)
                OAL_IO_PRINT("oal netbuf delist OK\n");
            ul_reschedule = OAL_FALSE;
            break;
        }

        if(OAL_TRUE == hmac_vap_user_is_bw_limit(&(pst_hmac_vap->st_vap_base_info), pst_netbuf))
        {
            /* 增加维测信息，出现用户和vap限速之后，也会造成用户关连不上，ping不通问题 */
            OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_TX, "{hmac_tx_event_process::vap id[%d] hmac_vap_user_is_bw_limit.}", pst_hmac_vap->st_vap_base_info.uc_vap_id);
            oal_spin_lock_bh(&pst_hmac_vap->st_smp_lock);
            oal_netbuf_add_to_list_tail(pst_netbuf, &pst_hmac_vap->st_tx_queue_head[pst_hmac_vap->uc_out_queue_id]);
            oal_spin_unlock_bh(&pst_hmac_vap->st_smp_lock);
            continue;
        }

        hmac_tx_data(pst_hmac_vap,pst_netbuf);
    } while(++ul_work < pst_hmac_vap->ul_tx_quata); // && jiffies == ul_start_time

    if(OAL_TRUE == ul_reschedule)
    {
        hmac_tx_post_event(&(pst_hmac_vap->st_vap_base_info));
    }

    return OAL_SUCC;
}
#endif


oal_void hmac_tx_ba_cnt_vary(
                       hmac_vap_stru   *pst_hmac_vap,
                       hmac_user_stru  *pst_hmac_user,
                       oal_uint8        uc_tidno,
                       oal_netbuf_stru *pst_buf)
{
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    oal_uint32             ul_current_timestamp;
    if ((WLAN_VAP_MODE_BSS_STA == pst_hmac_vap->st_vap_base_info.en_vap_mode)
        && (OAL_TRUE == g_ul_tx_ba_policy_select))
    {
        
        pst_hmac_user->auc_ba_flag[uc_tidno]++;
    }
    else
    {

        ul_current_timestamp = (oal_uint32)OAL_TIME_GET_STAMP_MS();

        /* 第一个包直接计数；
           短时间连续发包时，开始建立BA;
           TCP ACK回复慢，不考虑时间限制。 */
        if((0 == pst_hmac_user->auc_ba_flag[uc_tidno])
           || (oal_netbuf_is_tcp_ack((oal_ip_header_stru *)(oal_netbuf_data(pst_buf) + ETHER_HDR_LEN)))
           || ((oal_uint32)OAL_TIME_GET_RUNTIME(ul_current_timestamp, pst_hmac_user->aul_last_timestamp[uc_tidno]) < WLAN_BA_CNT_INTERVAL))
        {
            pst_hmac_user->auc_ba_flag[uc_tidno]++;
        }
        else
        {
            pst_hmac_user->auc_ba_flag[uc_tidno] = 0;
        }

        pst_hmac_user->aul_last_timestamp[uc_tidno] = ul_current_timestamp;
    }
#else
    pst_hmac_user->auc_ba_flag[uc_tidno]++;
#endif
}

#ifdef _PRE_WLAN_FEATURE_MULTI_NETBUF_AMSDU

oal_void hmac_tx_amsdu_ampdu_switch(oal_uint32 ul_tx_throughput_mbps)
{
    mac_device_stru  *pst_mac_device;
    mac_vap_stru     *pst_mac_vap;
    oal_uint32      ul_limit_throughput_high;
    oal_uint32      ul_limit_throughput_low;
    oal_uint32      ul_ret;
    oal_bool_enum_uint8   en_large_amsdu_ampdu;

    /* 如果定制化不支持amsdu_ampdu联合聚合 */
    if (OAL_FALSE == tx_large_amsdu.uc_tx_amsdu_ampdu_en)
    {
        return;
    }

    /* 如果有兼容性问题，关掉amsdu_ampdu联合聚合 */
    if (OAL_TRUE == tx_large_amsdu.uc_compability_en)
    {
        tx_large_amsdu.uc_cur_amsdu_ampdu_enable = OAL_FALSE;
        return;
    }

    /* 每秒吞吐量门限 */
    ul_limit_throughput_high = tx_large_amsdu.us_amsdu_ampdu_throughput_high;
    ul_limit_throughput_low  = tx_large_amsdu.us_amsdu_ampdu_throughput_low;

    if (ul_tx_throughput_mbps > ul_limit_throughput_high)
    {
        /* 高于150M,切换amsdu大包聚合 */
        en_large_amsdu_ampdu = OAL_TRUE;
    }
    else if (ul_tx_throughput_mbps < ul_limit_throughput_low)
    {
        /* 低于100M,关闭amsdu大包聚合 */
        en_large_amsdu_ampdu = OAL_FALSE;
    }
    else
    {
        /* 介于100M-150M之间,不作切换 */
        return;
    }
    /* 当前聚合方式相同,不处理 */
    if (tx_large_amsdu.uc_cur_amsdu_ampdu_enable == en_large_amsdu_ampdu)
    {
        return;
    }

    /* 切换至amsdu大包聚合时,需要判断是否符合切换条件 */
    if (OAL_TRUE == en_large_amsdu_ampdu)
    {
        pst_mac_device = mac_res_get_dev(0);
        /* 如果非单VAP,则不开启tx amsdu大包聚合 */
        if (1 != mac_device_calc_up_vap_num(pst_mac_device))
        {
            return;
        }

        ul_ret = mac_device_find_up_vap(pst_mac_device, &pst_mac_vap);
        if ((OAL_SUCC != ul_ret) || (OAL_PTR_NULL == pst_mac_vap))
        {
            return;
        }
    }
    tx_large_amsdu.uc_cur_amsdu_ampdu_enable = en_large_amsdu_ampdu;

    OAM_WARNING_LOG3(0,OAM_SF_ANY,"{hmac_tx_amsdu_ampdu_switch: limit_high = [%d],limit_low = [%d],tx_throught= [%d]!}",
          ul_limit_throughput_high,ul_limit_throughput_low, ul_tx_throughput_mbps);
}
#endif


/*lint -e19*/
oal_module_symbol(hmac_tx_wlan_to_wlan_ap);
oal_module_symbol(hmac_tx_lan_to_wlan);
oal_module_symbol(hmac_free_netbuf_list);

oal_module_symbol(hmac_tx_report_eth_frame);
oal_module_symbol(hmac_bridge_vap_xmit);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
oal_module_symbol(g_tx_debug);
oal_module_symbol(hmac_vap_start_xmit);
oal_module_symbol(hmac_tx_post_event);
#endif
/*lint +e19*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

