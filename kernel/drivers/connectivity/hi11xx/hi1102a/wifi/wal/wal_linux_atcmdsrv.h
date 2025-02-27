

#ifndef __WAL_LINUX_ATCMDSRV_H__
#define __WAL_LINUX_ATCMDSRV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "oal_ext_if.h"
#include "wlan_types.h"
#include "wlan_spec.h"
#include "mac_vap.h"
#include "hmac_ext_if.h"
#include "wal_ext_if.h"
#include "wal_config.h"
//#include "dmac_alg_if.h"
#include "wal_linux_ioctl.h"
#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_LINUX_ATCMDSRV_H
/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define WAL_ATCMDSRB_DBB_NUM_TIME                (5 * OAL_TIME_HZ)
#define WAL_ATCMDSRB_GET_RX_PCKT                 (5 * OAL_TIME_HZ)

#if (defined(_PRE_PRODUCT_ID_HI110X_DEV) || defined(_PRE_PRODUCT_ID_HI110X_HOST))
/*atcmdsrv私有命令宏定义*/
#define WAL_ATCMDSRV_IOCTL_DBB_LEN               12
#define WAL_ATCMDSRV_IOCTL_MAC_LEN               6
#define WAL_ATCMDSRV_IOCTL_COUNTRY_LEN           3
#define WAL_ATCMDSRV_IOCTL_VERIFY_CODE           0x1102A
#define WAL_ATCMDSRV_IOCTL_MODE_NUM              13
#define WAL_ATCMDSRV_IOCTL_DATARATE_NUM          15
#define WAL_ATCMDSRB_IOCTL_AL_TX_LEN             1000
#define WAL_ATCMDSRV_NV_WINVRAM_LENGTH           104                    /* WINVRAM内存空间 */
#define WAL_ATCMDSRV_DIE_ID_LENGTH               16
#define WAL_ATCMDSRB_CHECK_FEM_PA                (5 * OAL_TIME_HZ)
#define WAL_ATCMDSRV_GET_HEX_CHAR(tmp) (((tmp) > 9)? ((tmp-10) + 'A') : ((tmp) + '0'))
#define WLAN_HT_ONLY_MODE_2G                     WLAN_HT_ONLY_MODE + 3
#define WLAN_VHT_ONLY_MODE_2G                    WLAN_VHT_ONLY_MODE +3
/*lte共存对应的管脚号*/
#define WAL_ATCMDSRV_LTE_ISM_PRIORITY_NAME       "lte_ism_priority"
#define WAL_ATCMDSRV_LTE_RX_ACT_NAME             "lte_rx_act"
#define WAL_ATCMDSRV_LTE_TX_ACT_NAME             "lte_tx_act"
#define WAL_ATCMDSRV_EFUSE_CHIP_ID                0x02
#define WAL_ATCMDSRV_EFUSE_ADC_ERR_FLAG           0x0C
#define WAL_ATCMDSRV_EFUSE_BUFF_LEN               16
#define WAL_ATCMDSRV_EFUSE_REG_WIDTH              0x10
#define WAL_ATCMDSRV_CHANNEL_NUM                  6
#define WAL_ATCMSRV_MIN_BSS_EXPIRATION_AGE 25           /* 单位:  秒 */
#define WAL_ATCMSRV_MAX_BSS_EXPIRATION_AGE 500          /*单位:  秒*/

#define WAL_ATCMDSRV_WIFI_MIN_TXPOWER             5     /*单位:dBm */
#define WAL_ATCMDSRV_WIFI_MAX_TXPOWER             14    /*单位:dBm */

#define WAL_ATCMDSRV_IOCTL_DYN_INTV_LEN         3

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*atcmdsrv枚举类型*/
enum WAL_ATCMDSRV_IOCTL_CMD
{
    WAL_ATCMDSRV_IOCTL_CMD_WI_FREQ_SET=0,              /*  */
    WAL_ATCMDSRV_IOCTL_CMD_WI_POWER_SET,               /*  */
    WAL_ATCMDSRV_IOCTL_CMD_MODE_SET,                   /*  */
    WAL_ATCMDSRV_IOCTL_CMD_DATARATE_SET,
    WAL_ATCMDSRV_IOCTL_CMD_BAND_SET,
    WAL_ATCMDSRV_IOCTL_CMD_ALWAYS_TX_SET,
    WAL_ATCMDSRV_IOCTL_CMD_DBB_GET,
    WAL_ATCMDSRV_IOCTL_CMD_HW_STATUS_GET,
    WAL_ATCMDSRV_IOCTL_CMD_ALWAYS_RX_SET,
    WAL_ATCMDSRV_IOCTL_CMD_HW_ADDR_SET,
    WAL_ATCMDSRV_IOCTL_CMD_RX_PCKG_GET,
    WAL_ATCMDSRV_IOCTL_CMD_PM_SWITCH,
    WAL_ATCMDSRV_IOCTL_CMD_RX_RSSI,
    WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_SET,
    WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_RESULT,
    WAL_ATCMDSRV_IOCTL_CMD_CHIPCHECK_TIME,
    WAL_ATCMDSRV_IOCTL_CMD_UART_LOOP_SET,
    WAL_ATCMDSRV_IOCTL_CMD_SDIO_LOOP_SET,
    WAL_ATCMDSRV_IOCTL_CMD_RD_CALDATA,                  /* read caldata from dts */
    WAL_ATCMDSRV_IOCTL_CMD_SET_CALDATA,
    WAL_ATCMDSRV_IOCTL_CMD_EFUSE_CHECK,
    WAL_ATCMDSRV_IOCTL_CMD_SET_ANT,
    WAL_ATCMDSRV_IOCTL_CMD_DIEID_INFORM,
    WAL_ATCMDSRV_IOCTL_CMD_SET_COUNTRY,
    WAL_ATCMDSRV_IOCTL_CMD_GET_UPCCODE,
    WAL_ATCMDSRV_IOCTL_CMD_WIPIN_TEST,
    WAL_ATCMDSRV_IOCTL_CMD_GET_PDET_PARAM,
    WAL_ATCMDSRV_IOCTL_CMD_IO_TEST,
    WAL_ATCMDSRV_IOCTL_CMD_DYN_INTERVAL,
    WAL_ATCMDSRV_IOCTL_CMD_PMU_CHECK,
    WAL_ATCMDSRV_IOCTL_CMD_PT_STE,
    WAL_ATCMDSRV_IOCTL_CMD_SELFCALI_INTERVAL,
    WAL_ATCMDSRV_IOCTL_CMD_SET_BSS_EXPIRE_AGE,      /* 设置扫描结果老化时间*/
    WAL_ATCMDSRV_IOCTL_CMD_GET_CONN_INFO,           /* 获取连接信息 */

	WAL_ATCMDSRV_IOCTL_CMD_TEST_BUTT
};
typedef enum
{
    ATCMDSRV_WIFI_DISCONNECT,           /*  */
    ATCMDSRV_WIFI_CONNECTED,            /*  */
}atcmdsrv_wifi_conn_info_enum;
typedef oal_uint8 atcmdsrv_wifi_conn_info_enum_uint8;

#ifdef _PRE_WLAN_FEATURE_SMARTANT
typedef struct
{
    oal_uint8   uc_ant_type;
    oal_uint8   auc_resv[3];
    oal_uint32  ul_last_ant_change_time_ms;
    oal_uint32  ul_ant_change_number;
    oal_uint32  ul_main_ant_time_s;
    oal_uint32  ul_aux_ant_time_s;
    oal_uint32  ul_total_time_s;
}wal_atcmdsrv_ant_info_stru;
#endif
/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_SMARTANT
extern wal_atcmdsrv_ant_info_stru g_st_atcmdsrv_ant_info;
#endif

extern oal_uint32  g_pd_bss_expire_time;

/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
struct wal_atcmdsrv_wifi_connect_info
{
	atcmdsrv_wifi_conn_info_enum_uint8 en_status;
	oal_uint8 auc_ssid[WLAN_SSID_MAX_LEN];
	oal_uint8 auc_bssid[WLAN_MAC_ADDR_LEN];
	oal_int8  c_rssi;
};

struct wal_atcmdsrv_wifi_tx_power_range
{
	oal_uint8  uc_min;
	oal_uint8  uc_max;
};


/* 1102 使用atcmdsrv 下发命令 */
typedef struct wal_atcmdsrv_wifi_priv_cmd {
    /* 校验位,取值1102,与其他平台区别开来 */
    oal_int32 l_verify;
    oal_int32   ul_cmd;                                          /* 命令号 */
    union
    {
    oal_int32 l_freq;
    oal_int32 l_pow;
    oal_int32 l_mode;
    oal_int32 l_datarate;
    oal_int32 l_bandwidth;
    oal_int32 l_awalys_tx;
    oal_int32 l_fem_pa_status;
    oal_int32 l_awalys_rx;
    oal_int32 l_rx_pkcg;
    oal_int32 l_pm_switch;
    oal_int32 l_rx_rssi;
    oal_int32 l_chipcheck_result;
    oal_uint64 ull_chipcheck_time;
    oal_int32 l_uart_loop_set;
    oal_int32 l_sdio_loop_set;
    oal_int32 l_efuse_check_result;
    oal_int32 l_set_ant;
    oal_int32 upc_code;
    oal_int32 l_pin_status;
    oal_int32 l_pmu_status;
    oal_uint8 auc_mac_addr[WAL_ATCMDSRV_IOCTL_MAC_LEN];         /* 6 */
    oal_int8  auc_dbb[WAL_ATCMDSRV_IOCTL_DBB_LEN];              /* 12 */
    oal_uint8 auc_caldata[WAL_ATCMDSRV_NV_WINVRAM_LENGTH];      /* 104 */
    oal_uint16 die_id[WAL_ATCMDSRV_DIE_ID_LENGTH];      /*  16 */
    oal_int8  auc_country_code[WAL_ATCMDSRV_IOCTL_COUNTRY_LEN]; /* 3 */
    oal_int8  ac_pd_tran_param[WAL_ATCMDSRV_NV_WINVRAM_LENGTH];
    oal_int16 s_wkup_io_status;
    oal_int8  c_dyn_interval[WAL_ATCMDSRV_IOCTL_DYN_INTV_LEN];
    oal_int32 l_pt_set;
    oal_int32 selfcali_interval_set;
    oal_uint32 ul_bss_expire_age;                               /* 产线扫描结果老化时间 */
    struct wal_atcmdsrv_wifi_connect_info st_connect_info;      /* WiFi 连接信息 */
    }pri_data;

}wal_atcmdsrv_wifi_priv_cmd_stru;
/********************************************************************************************
*****************************EFUSE异常检查位域说明*******************************************

         位域名称               位域起始位  位域与以下位段映射关系
    (1): DIE_ID                 [154:  0]   die_id_0,die_id1,die_id2,die_id3,die_id4
    (2): Reserve0               [159:155]   reserve0
    (3): CHIP ID                [167:160]   chip_id
    (4): Reserve1               [170:168]   reserve1
    (5): CHIP FUNCTION Value    [202:171]   chip_function_value_low,chip_function_value_high
    (6): ADC                    [206:203]   adc
    (7): Reserve2               [207:207]   reserve2
    (8): BCPU                   [208:208]   bcpu
    (9): Reserve3               [227:209]   reserve3_low,reserve3_high
    (10):PMU TRIM Value         [247:228]   pmu_trim_value
    (11):NFC PUN TRIM Value     [253:248]   nfc_pmu_trim_value
    (12):Reserve4               [255:254]   reserve4
*********************************************************************************************/
typedef struct efuse_bits
{
    oal_uint32 die_id_0;   //DIE_ID[31:0]
    oal_uint32 die_id_1;   //DIE_ID[63:32]
    oal_uint32 die_id_2;   //DIE_ID[95:64]
    oal_uint32 die_id_3;   //DIE_ID[127:96]
    oal_uint32 die_id_4:27;//DIE_ID[154:128]
    oal_uint32 reserve0:5; //Reserve0[159:155]
    oal_uint32 chip_id :8; //chip_id[167:160]
    oal_uint32 trim:1; //Reserve1[168]
    oal_uint32 reserve1:2; //Reserve1[170:169]
    oal_uint32 chip_function_value_low :21;//CHIP_FUNCTION_Value[191:171]
    oal_uint32 chip_function_value_high:11;//CHIP_FUNCTION_Value[202:192]
    oal_uint32 adc     :4;//ADC[206:203]
    oal_uint32 reserve2:1;//Reserve2[207:207]
    oal_uint32 bcpu    :1;//BCPU[208:208]
    oal_uint32 reserve3_low      :15;//Reserve3[223:209]
    oal_uint32 reserve3_high     :4; //Reserve3[227:224]
    oal_uint32 pmu_trim_value    :20;//PMU_TRIM_Value[247:228]
    oal_uint32 nfc_pmu_trim_value:6; //NFC_PUM_TRIM_Value[253:248]
    oal_uint32 reserve4          :2; //Reserve4[255:254]
}wal_efuse_bits;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern oal_int32 wal_atcmdsrv_wifi_priv_cmd(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr, oal_int32 ul_cmd);
extern oal_int32 wal_atcmsrv_ioctl_get_hw_status(oal_net_device_stru *pst_net_dev, oal_int32 *pl_fem_pa_status);
extern oal_void  wal_atcmsrv_ioctl_get_fem_pa_status(oal_net_device_stru *pst_net_dev, oal_int32 *pl_fem_pa_status);
extern oal_int32 wlan_device_mem_check(void);
extern oal_int32 wlan_device_mem_check_result(unsigned long long *time);
extern oal_int32 conn_test_uart_loop(char *param);
extern oal_int32 conn_test_sdio_loop(char *param);
extern oal_int32 conn_test_wifi_chan_loop(const char *param);
extern oal_int32 hwifi_fetch_ori_caldata(oal_uint8* auc_caldata, oal_int32 l_nvm_len);
extern oal_int32 hwifi_config_init(oal_int32);
extern oal_int32 hi1102a_dev_io_test(void);

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of wal_linux_ioctl.h */

