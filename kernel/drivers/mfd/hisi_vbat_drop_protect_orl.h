/*
 * Header file for hisi vbat drop protect device driver
 */

#ifndef	__HISI_VBAT_DROP_PROTECT_ORL_H
#define	__HISI_VBAT_DROP_PROTECT_ORL_H

#include <soc_corecrg_b_m_interface.h>
#include <soc_corecrg_l_l3_interface.h>
#include <soc_crgperiph_interface.h>
#define VBAT_DROP_TEST /* for get core auto div status*/

/*core enable reg in pmctrl*/
#define LITTLE_VOL_DROP_EN_ADDR(base)       SOC_CORECRG_L_L3_VS_CTRL_LITTLE_ADDR(base)
#define BIG_VOL_DROP_EN_ADDR(base)          SOC_CORECRG_B_M_VS_CTRL_BIG0_ADDR(base)
#define L3_VOL_DROP_EN_ADDR(base)           SOC_CORECRG_L_L3_VS_CTRL_L3_ADDR(base)
#define GPU_VOL_DROP_EN_ADDR(base)          SOC_PMCTRL_VS_CTRL_GPU_ADDR(base)
#define NPU_VOL_DROP_EN_ADDR(base)          SOC_CRGPERIPH_PERI_COMMON_CTRL0_ADDR(base)

#define LITTLE_VOL_DROP_EN_BIT              BIT(SOC_CORECRG_L_L3_VS_CTRL_LITTLE_vol_drop_en_little_START)
#define BIG_VOL_DROP_EN_BIT                 BIT(SOC_CORECRG_B_M_VS_CTRL_BIG0_vol_drop_en_big0_START)
#define L3_VOL_DROP_EN_BIT                  BIT(SOC_CORECRG_L_L3_VS_CTRL_L3_vol_drop_en_l3_START)
#define GPU_VOL_DROP_EN_BIT                 BIT(SOC_PMCTRL_VS_CTRL_GPU_vol_drop_en_gpu_START)
#define NPU_VOL_DROP_EN_BIT                 BIT(SOC_CRGPERIPH_PERI_COMMON_CTRL0_aicore_dbat_drop_freq_down_bp_START)
#define NPU_VOL_DROP_MASK                   (NPU_VOL_DROP_EN_BIT << 16)
#ifdef VBAT_DROP_TEST
/*core enable status register in pmctrl*/
#define LITTLE_VOL_DROP_EN_STAT_ADDR(base)  SOC_CORECRG_L_L3_VS_TEST_STAT_LITTLE_ADDR(base)
#define BIG_VOL_DROP_EN_STAT_ADDR(base)     SOC_CORECRG_B_M_VS_TEST_STAT_BIG0_ADDR(base)
#define L3_VOL_DROP_EN_STAT_ADDR(base)      SOC_CORECRG_L_L3_VS_TEST_STAT_L3_ADDR(base)
#define GPU_VOL_DROP_EN_STAT_ADDR(base)     SOC_PMCTRL_VS_TEST_STAT_GPU_ADDR(base)
#define NPU_VOL_DROP_EN_STAT_ADDR(base)     SOC_CRGPERIPH_PERI_AUTODIV_INUSE_STAT_ADDR(base)

#define LITTLE_VOL_DROP_EN_STAT_BIT         BIT(SOC_CORECRG_L_L3_VS_TEST_STAT_LITTLE_vbat_drop_protect_ind_little_START)
#define BIG_VOL_DROP_EN_STAT_BIT            BIT(SOC_CORECRG_B_M_VS_TEST_STAT_BIG0_vbat_drop_protect_ind_big0_START)
#define L3_VOL_DROP_EN_STAT_BIT             BIT(SOC_CORECRG_L_L3_VS_TEST_STAT_L3_vbat_drop_protect_ind_l3_START)
#define GPU_VOL_DROP_EN_STAT_BIT            BIT(SOC_PMCTRL_VS_TEST_STAT_GPU_vbat_drop_protect_ind_gpu_START)
#define NPU_VOL_DROP_EN_STAT_BIT            BIT(SOC_CRGPERIPH_PERI_AUTODIV_INUSE_STAT_aicore_dbat_drop_freq_down_stat_START)
#endif

#endif