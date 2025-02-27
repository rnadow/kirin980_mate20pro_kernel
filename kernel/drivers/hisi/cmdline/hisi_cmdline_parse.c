#include <linux/of_platform.h>
#include <linux/memblock.h>
#include <linux/irqchip.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include "securec.h"

/* Format of hex string: 0x12345678 */
#define RUNMODE_FLAG_NORMAL         0
#define RUNMODE_FLAG_FACTORY        1
#define HEX_STRING_MAX              10
#define TRANSFER_BASE               16
#define ASW_PROTECT_FLAG_INVALID    (-1)
#define ASW_PROTECT_VOLT_INVALID    (-1)
#define ASW_PROTECT_BACKUP_INVALID  (-1)

static unsigned int hisi_platform_product_id = 0xFFFFFFFF;
static unsigned int runmode_factory = RUNMODE_FLAG_NORMAL;

#ifdef CONFIG_HISI_ASW
static int g_asw_protect_status;
static int g_asw_protect_voltage;
static int g_asw_protect_backup_flag;
#endif /* CONFIG_HISI_ASW */

static int __init early_parse_runmode_cmdline(char *p)
{
	if (p) {
		if (!strncmp(p, "factory", strlen("factory")))
			runmode_factory = RUNMODE_FLAG_FACTORY;

		pr_info("runmode is %s, runmode_factory = %d\n", p,
		       runmode_factory);
	}

	return 0;
}

early_param("androidboot.swtype", early_parse_runmode_cmdline);

/* the function interface to check factory/normal mode in kernel */
unsigned int runmode_is_factory(void)
{
	return runmode_factory;
}
EXPORT_SYMBOL(runmode_is_factory);

static int logctl_flag;	/* default 0, switch logger off */
int get_logctl_value(void)
{
	return logctl_flag;
}
EXPORT_SYMBOL(get_logctl_value);

unsigned int enter_recovery_flag;
/**
* parse boot_into_recovery cmdline which is passed from boot_recovery() of boot.c *
* Format : boot_into_recovery_flag=0 or 1             *
*/
static int __init early_parse_enterrecovery_cmdline(char *p)
{
	int ret = 0;
	char enter_recovery[HEX_STRING_MAX + 1];

	if (EOK != memset_s(enter_recovery, HEX_STRING_MAX + 1, 0, HEX_STRING_MAX + 1)) {
		pr_err("%s:%d Err\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (EOK != memcpy_s(enter_recovery, HEX_STRING_MAX + 1, p, HEX_STRING_MAX)) {
		pr_err("%s:%d Err\n", __func__, __LINE__);
		return -EINVAL;
	}
	enter_recovery[HEX_STRING_MAX] = '\0';

	ret = kstrtouint(enter_recovery, TRANSFER_BASE, &enter_recovery_flag);
	if (ret)
		return ret;

	pr_info("enter recovery p:%s, enter_recovery_flag :%d\n", p,
	       enter_recovery_flag);

	return 0;
}

early_param("enter_recovery", early_parse_enterrecovery_cmdline);

unsigned int get_boot_into_recovery_flag(void)
{
	return enter_recovery_flag;
}
EXPORT_SYMBOL(get_boot_into_recovery_flag);

unsigned int recovery_update_flag;
/**
* Format : recovery_update=0 or 1 *
*/
static int __init early_parse_recovery_update_cmdline(char *p)
{
	int ret = 0;
	char recovery_update[HEX_STRING_MAX + 1];

	if (EOK != memset_s(recovery_update, HEX_STRING_MAX + 1, 0, HEX_STRING_MAX + 1)) {
		pr_err("%s:%d Err\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (EOK != memcpy_s(recovery_update, HEX_STRING_MAX + 1, p, HEX_STRING_MAX)) {
		pr_err("%s:%d Err\n", __func__, __LINE__);
		return -EINVAL;
	}
	recovery_update[HEX_STRING_MAX] = '\0';

	ret = kstrtouint(recovery_update, TRANSFER_BASE, &recovery_update_flag);
	if (ret)
		return ret;

	pr_info("recovery_update p:%s, recovery_update_flag :%d\n", p,
	       recovery_update_flag);

	return 0;
}

early_param("recovery_update", early_parse_recovery_update_cmdline);

unsigned int get_recovery_update_flag(void)
{
	return recovery_update_flag;
}
EXPORT_SYMBOL(get_recovery_update_flag);

static int __init early_parse_logctl_cmdline(char *p)
{
	int ret = 0;
	char logctl[HEX_STRING_MAX + 1] = {'\0'};

	if (p == NULL) {
		logctl_flag = 0;	/* error parameter, switch logger off */
		pr_err("%s: setup_logctl parameter error!\n", __func__);
		return 0;
	}

	if (EOK != memset_s(logctl, HEX_STRING_MAX + 1, 0, HEX_STRING_MAX + 1)) {
		pr_err("%s:%d Err\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (EOK != memcpy_s(logctl, HEX_STRING_MAX + 1, p, HEX_STRING_MAX)) {
		pr_err("%s:%d Err\n", __func__, __LINE__);
		return -EINVAL;
	}

	ret = kstrtoint(logctl, TRANSFER_BASE, &logctl_flag);
	if (ret)
		return ret;

	pr_info("%s: p: %s, logctl: %d\n", __func__, p, logctl_flag);

	return 0;
}

early_param("setup_logctl", early_parse_logctl_cmdline);

/*******************
*0: battery is normal;
*1: battery is low;
*********************/
static int g_lowbattery;

int get_lowbatteryflag(void)
{
	return g_lowbattery;
}
EXPORT_SYMBOL(get_lowbatteryflag);

void set_lowBatteryflag(int flag)
{
	g_lowbattery = flag;
}
EXPORT_SYMBOL(set_lowBatteryflag);

static int __init early_parse_lowpower_cmdline(char *p)
{
	int ret = 0;
	char clow[HEX_STRING_MAX + 1] = {'\0'};

	if (p == NULL) {
		g_lowbattery = 0;	/* error parameter, switch logger off */
		pr_err("%s input is NULL\n", __func__);
		return 0;
	}

	if (EOK != memset_s(clow, HEX_STRING_MAX + 1, 0, HEX_STRING_MAX + 1)) {
		pr_err("%s:%d Err\n", __func__, __LINE__);
		return -EINVAL;
	}
	if (EOK != memcpy_s(clow, HEX_STRING_MAX + 1, p, HEX_STRING_MAX)) {
		pr_err("%s:%d Err\n", __func__, __LINE__);
		return -EINVAL;
	}

	ret = kstrtoint(clow, TRANSFER_BASE, &g_lowbattery);
	if (ret)
		return ret;

	pr_info("%s %d\n", __func__, g_lowbattery);
	return 0;
}

early_param("low_volt_flag", early_parse_lowpower_cmdline);

static int need_dsm_notify = 1;
int get_dsm_notify_flag(void)
{
	return need_dsm_notify;
}
EXPORT_SYMBOL(get_dsm_notify_flag);

static int __init early_parse_dsm_notify_cmdline(char *p)
{
	int ret;
	char clow[HEX_STRING_MAX + 1];

	if (p == NULL) {
		need_dsm_notify = 1;       /* error parameter, dsm notify */
		pr_err("%s input is NULL\n", __func__);
		return 0;
	}

	ret = memset_s(clow, HEX_STRING_MAX + 1, 0, HEX_STRING_MAX + 1);
	if (ret != EOK) {
		pr_err("%s clow set 0 fail:%d\n", __func__, ret);
		return ret;
	}

	ret = memcpy_s(clow, HEX_STRING_MAX + 1, p, HEX_STRING_MAX);
	if (ret != EOK) {
		pr_err("%s clow copy fail:%d\n", __func__, ret);
		return ret;
	}

	ret = kstrtoint(clow, TRANSFER_BASE, &need_dsm_notify);
	if (ret)
		return ret;

	pr_info("%s %d\n", __func__, need_dsm_notify);
	return 0;
}

early_param("dsm_notify", early_parse_dsm_notify_cmdline);

/**
 * parse powerdown charge cmdline which is passed from fastoot. *
 * Format : pd_charge=0 or 1             *
 */
static unsigned int pd_charge_flag;
static int __init early_parse_pdcharge_cmdline(char *p)
{
	if (p) {
		if (!strncmp(p, "charger", strlen("charger")))
			pd_charge_flag = 1;
		else
			pd_charge_flag = 0;

		pr_info("power down charge p:%s, pd_charge_flag :%u\n", p,
			   pd_charge_flag);
	}

	return 0;
}

early_param("androidboot.mode", early_parse_pdcharge_cmdline);

#ifdef CONFIG_HISI_ASW
static int __init early_parse_asw_protect_flag_cmdline(char *p)
{
	int ret;
	char status_info[HEX_STRING_MAX + 1] = { 0 };

	if (!p) {
		g_asw_protect_status = ASW_PROTECT_FLAG_INVALID;
		pr_err("asw_protect_status cmdline input is NULL\n");
		return -EINVAL;
	}

	ret = memcpy_s(status_info, HEX_STRING_MAX + 1, p, HEX_STRING_MAX);
	if (ret) {
		g_asw_protect_status = ASW_PROTECT_FLAG_INVALID;
		pr_err("asw_protect_status memcopy_s fail\n");
		return -EINVAL;
	}

	ret = kstrtoint(status_info, HEX_STRING_MAX, &g_asw_protect_status);
	if (ret) {
		g_asw_protect_status = ASW_PROTECT_FLAG_INVALID;
		pr_err("asw_protect_status kstrtoint fail\n");
		return -EINVAL;
	}

	pr_info("asw_protect_status = %d\n", g_asw_protect_status);
	return 0;
}
early_param("asw_protect_status", early_parse_asw_protect_flag_cmdline);

static int __init early_parse_asw_protect_voltage_cmdline(char *p)
{
	int ret;
	char volt_info[HEX_STRING_MAX + 1] = { 0 };

	if (!p) {
		g_asw_protect_voltage = ASW_PROTECT_VOLT_INVALID;
		pr_err("asw_protect_voltage cmdline input is NULL\n");
		return -EINVAL;
	}

	ret = memcpy_s(volt_info, HEX_STRING_MAX + 1, p, HEX_STRING_MAX);
	if (ret) {
		g_asw_protect_voltage = ASW_PROTECT_VOLT_INVALID;
		pr_err("asw_protect_voltage memcopy_s fail\n");
		return -EINVAL;
	}

	ret = kstrtoint(volt_info, HEX_STRING_MAX, &g_asw_protect_voltage);
	if (ret) {
		g_asw_protect_voltage = ASW_PROTECT_VOLT_INVALID;
		pr_err("asw_protect_voltage kstrtoint fail\n");
		return -EINVAL;
	}

	pr_info("asw_protect_voltage = %d\n", g_asw_protect_voltage);
	return 0;
}
early_param("asw_protect_voltage", early_parse_asw_protect_voltage_cmdline);

static int __init early_parse_asw_protect_backup_flag_cmdline(char *p)
{
	int ret;
	char backup_info[HEX_STRING_MAX + 1] = { 0 };

	if (!p) {
		g_asw_protect_backup_flag = ASW_PROTECT_BACKUP_INVALID;
		pr_err("asw_protect_backup_flag cmdline input is NULL\n");
		return -EINVAL;
	}

	ret = memcpy_s(backup_info, HEX_STRING_MAX + 1, p, HEX_STRING_MAX);
	if (ret) {
		g_asw_protect_backup_flag = ASW_PROTECT_BACKUP_INVALID;
		pr_err("asw_protect_backup_flag memcopy_s fail\n");
		return -EINVAL;
	}

	ret = kstrtoint(backup_info, HEX_STRING_MAX,
		&g_asw_protect_backup_flag);
	if (ret) {
		g_asw_protect_backup_flag = ASW_PROTECT_BACKUP_INVALID;
		pr_err("asw_protect_backup_flag kstrtoint fail\n");
		return -EINVAL;
	}

	pr_info("asw_protect_backup_flag = %d\n", g_asw_protect_backup_flag);
	return 0;
}
early_param("asw_protect_backup_flag",
	early_parse_asw_protect_backup_flag_cmdline);

int asw_get_protect_status(void)
{
	return g_asw_protect_status;
}
EXPORT_SYMBOL(asw_get_protect_status);

int asw_get_protect_voltage(void)
{
	return g_asw_protect_voltage;
}
EXPORT_SYMBOL(asw_get_protect_voltage);

int asw_get_protect_backup_flag(void)
{
	return g_asw_protect_backup_flag;
}
EXPORT_SYMBOL(asw_get_protect_backup_flag);
#endif /* CONFIG_HISI_ASW */

unsigned int get_pd_charge_flag(void)
{
	return pd_charge_flag;
}
EXPORT_SYMBOL(get_pd_charge_flag);

#if CONFIG_HISI_NVE_WHITELIST
static unsigned int userlock = 0;
static int __init early_parse_userlock_cmdline(char *p)
{
	if (p) {
		if (!strncmp(p, "locked", strlen("locked")))
			userlock = 1;
		else
			userlock = 0;

		pr_info("userlock p:%s, userlock :%u\n", p,
			   userlock);
	}

	return 0;
}

early_param("userlock", early_parse_userlock_cmdline);

unsigned int get_userlock(void)
{
	return userlock;
}
EXPORT_SYMBOL(get_userlock);
#endif /* CONFIG_HISI_NVE_WHITELIST */

unsigned int bsp_need_loadmodem(void)
{
	unsigned int rs = 0;

	rs |= get_recovery_update_flag();
	return !rs;
}
EXPORT_SYMBOL(bsp_need_loadmodem);

unsigned int is_load_modem(void)
{
	unsigned int rs = 0;

	rs |= get_recovery_update_flag();
	rs |= get_pd_charge_flag();
	return !rs;
}
EXPORT_SYMBOL(is_load_modem);


/******************************************************************************
* Function: static int __init early_parse_product_id(char *p)
* Description: Save hisilicon platform product id to kernel.
*              The product id passed by cmd line from bootloader.
* Input:
*        char *p -- cmd line node
* Return:
*        always 0
******************************************************************************/
static int __init early_parse_product_id(char *p)
{
    int ret = 0;
    char input_id[HEX_STRING_MAX + 1] = {0};

    if (p) {
        if (EOK != memcpy_s(input_id, HEX_STRING_MAX + 1, p, HEX_STRING_MAX)) {
		pr_err("%s:%d Err\n", __func__, __LINE__);
		return -EINVAL;
	}

        ret = kstrtouint(input_id, TRANSFER_BASE, &hisi_platform_product_id);
        if (ret){
            hisi_platform_product_id = 0xFFFFFFFF;
        }

        pr_info("input product id:%s, parsed product id:0X%X\n", p, hisi_platform_product_id);
    }

    return 0;
}

early_param("productid", early_parse_product_id);

/******************************************************************************
* Function: unsigned int get_product_id(void)
* Description: Get hisilicon platform product id.
*              The product id passed by cmd line from bootloader.
* Input:
*        void
* Return:
*        unsigned in product id
*        error   -- return 0xFFFFFFFF
*        success -- return not 0xFFFFFFFF
******************************************************************************/
unsigned int get_cmd_product_id(void)
{
    return hisi_platform_product_id;
}

EXPORT_SYMBOL(get_cmd_product_id);
