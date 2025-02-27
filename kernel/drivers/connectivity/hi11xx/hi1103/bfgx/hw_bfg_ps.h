

#ifndef __HW_BFG_PS_H__
#define __HW_BFG_PS_H__

/* 其他头文件包含 */
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/debugfs.h>
#include <linux/workqueue.h>
#include "plat_type.h"

#define DTS_COMP_HI110X_PS_NAME "hisilicon,hisi_bfgx"

#define BT_TX_MAX_FRAME   1500  /* tx bt data max lenth */
#define GNSS_TX_MAX_FRAME 30000 /* gnss send buf max lenth */
#define FM_TX_MAX_FRAME   512   /* fm cmd data max lenth */
#define NFC_TX_MAX_FRAME  512   /* nfc cmd data max lenth */
#define IR_TX_MAX_FRAME   5120  /* ir cmd data max lenth */
#define DBG_TX_MAX_FRAME  10000 /* dbg cmd data max lenth */

#define BT_RX_MAX_FRAME                1500  /* RX bt data max lenth */
#define FM_RX_MAX_FRAME                512   /* fm recv buf max lenth */
#define NFC_RX_MAX_FRAME               512   /* nfc recv buf max lenth */
#define IR_RX_MAX_FRAME                5120  /* ir recv buf max lenth */
#define DBG_RX_MAX_FRAME               10000 /* dbg recv buf max lenth */
#define GNSS_RX_MAX_FRAME              30000 /* gnss recv buf max lenth */

/* skb has skb_shared_info, and SKB_DATA_ALIGN */
#define GNSS_RX_SEPERATE_FRAME_MAX_LEN (4 * 1024 - SKB_DATA_ALIGN(sizeof(struct skb_shared_info)) - SKB_DATA_ALIGN(1))

#define MEM_DUMP_RX_MAX_FRAME 4096 /* mem dump recv buf max lenth */
#define WIFI_MEM_RX_MAX_FRAME 256  /* uart wifi mem dump recv buf max lenth */

#define UART_LOOP_MAX_PKT_LEN 2048 /* device buf len is 2400 */

#define NOBT_FRAME_EXTRA_SPACE  6                              /* 预留6字节余量 */
#define MAX_NOBT_FRAME_LEN      (67 + NOBT_FRAME_EXTRA_SPACE)  /* FGNI max frame len */
#define MAX_LAST_WORD_FRAME_LEN (114 + NOBT_FRAME_EXTRA_SPACE) /* FGNI max frame len */

#define GNSS_TX_PACKET_LIMIT 200
#define FM_TX_PACKET_LIMIT   200
#define NFC_TX_PACKET_LIMIT  200
#define IR_TX_PACKET_LIMIT   200

#define SEPRETED_RX_PKT_SEQ_ERROR   0
#define SEPRETED_RX_PKT_SEQ_CORRECT 1

#define DEBUG_SKB_BUFF_LEN 2048
#define PUBLIC_BUF_MAX     (8 * 1024) /* public buf total lenth */

#define PACKET_START_SIGNAL 0x7e
#define PACKET_END_SIGNAL   0x7e

#define BT_TX_TIMES 5 /* times continue tx bt data */

/* last words type in 8 byte packet head */
#define PACKET_RX_FUNC_LAST_WORDS    0x10 /* rx last words from device */
#define SYS_TOTAL_PACKET_LENTH       0x06 /* sys packet total lenth */
#define PACKET_RX_RPT_IND_LAST_WORDS 0x03 /* log pri id */
#define LAST_WORD_LEN                109
#define DEV_SEND_CHR_ERRNO_LEN       0x04 /* device send CHR errno lenth */

/* last word info index */
#define START_SIGNAL_LAST_WORDS         0
#define PACKET_RX_FUNC_INDEX_LAST_WORDS 1
#define RPT_IND_INDEX_LAST_WORDS        2
#define FAULT_TYPE_INDEX_LAST_WORDS     12
#define FAULT_REASON_INDEX_LAST_WORDS   16
#define LR_INDEX_LAST_WORDS             80
#define PC_INDEX_LAST_WORDS             84
#define PACKET_FRAMELEN_INDEX           4
#define PACKET_HEADER_LEN               8
#define PACKET_OFFSET_HEAD_INDEX        4
#define PACKET_OFFSET_HEAD_NEXT_INDEX   5

#define BAUD_STR_LEN      20
#define SSI_PATH_NAME_LEN 100

/*
 * time in msec to wait for
 * line discipline to be installed
 */
#define HISI_LDISC_TIME 2000

#define RX_PACKET_CORRECT  0
#define RX_PACKET_CONTINUE 1
#define RX_PACKET_ERR      (-1)

#define OPEN_TTY_RETRY_COUNT 5

#define HISI_UART_DEV_NAME_LEN 32

/* BFGX正常通信使用3M波特率，host唤醒device时使用115200波特率，单红外时使用921600波特率 */
#ifdef BFGX_UART_DOWNLOAD_SUPPORT
#define DEFAULT_BAUD_RATE 2000000
#else
#define DEFAULT_BAUD_RATE 4000000
#endif

#define LOW_FREQ_BAUD_RATE  4000000
#define HIGH_FREQ_BAUD_RATE 6000000

#define UART_BAUD_RATE_1M  1000000
#define UART_BAUD_RATE_2M  2000000
#define UART_BAUD_RATE_4M  4000000
#define UART_BAUD_RATE_6M  6000000
#define UART_BAUD_RATE_10M 10000000

#define WKUP_DEV_BAUD_RATE 115200
#define IR_ONLY_BAUD_RATE  921600

#define FLOW_CTRL_ENABLE  1
#define FLOW_CTRL_DISABLE 0

#define UART_PCLK_FROM_SENSORHUB 0
#define UART_PCLK_NORMAL         1

/* timeout for gnss read */
#define GNSS_SET_READ_TIME     1
#define GNSS_READ_DEFAULT_TIME 1000
#define GNSS_MAX_READ_TIME     10000

/* gnss update para */
#define PLAT_GNSS_MAGIC             'w'
#define PLAT_GNSS_REFCLK_PARA_CMD   _IOW(PLAT_GNSS_MAGIC, 3, int)

/* timeout for fm read */
#define FM_SET_READ_TIME     1
#define FM_READ_DEFAULT_TIME 1000
#define FM_MAX_READ_TIME     10000

/* timeout for dbg read */
#define DBG_READ_DEFAULT_TIME 500

#define LEN_LOW_BYTE(len)  ((uint8)((len)&0xff))
#define LEN_HIGH_BYTE(len) ((uint8)(((len)&0xff00) >> 8))

/* define for tx and rx packet queue */
#define TX_URGENT_QUEUE 0
#define TX_HIGH_QUEUE   1
#define TX_LOW_QUEUE    2
#define RX_GNSS_QUEUE   3
#define RX_FM_QUEUE     4
#define RX_BT_QUEUE     5
#define RX_DBG_QUEUE    6
#define RX_NFC_QUEUE    7
#define RX_IR_QUEUE     8

/* max number for tx and rx packet queue */
#define TX_HIGH_QUE_MAX_NUM 50
#define TX_LOW_QUE_MAX_NUM  250
#define RX_GNSS_QUE_MAX_NUM 50
#define RX_FM_QUE_MAX_NUM   50
#define RX_BT_QUE_MAX_NUM   50
#define RX_DBG_QUE_MAX_NUM  50
#define RX_NFC_QUE_MAX_NUM  50
#define RX_IR_QUE_MAX_NUM   50

/* timeout when after download patch */
#define WAIT_BFGX_BOOTOK_TIME 1000
#define WAIT_BT_OPEN_TIME     3000
#define WAIT_BT_CLOSE_TIME    3000
#define WAIT_FM_OPEN_TIME     3000
#define WAIT_FM_CLOSE_TIME    3000
#define WAIT_GNSS_OPEN_TIME   3000
#define WAIT_GNSS_CLOSE_TIME  3000
#define WAIT_IR_OPEN_TIME     3000
#define WAIT_IR_CLOSE_TIME    3000
#define WAIT_NFC_OPEN_TIME    1000
#define WAIT_NFC_CLOSE_TIME   2000

#define WAIT_WIFI_OPEN_TIME  3000
#define WAIT_WIFI_CLOSE_TIME 1000

#define RELEASE_DELAT_TIMES 50

#define BT_SLEEP_TIME       2000
#define PLATFORM_SLEEP_TIME 50

/* 检查是否bfg timer工作正常,5s内无有mod timer, 异常处理,防止始终不睡眠 */
#define PL_CHECK_TIMER_WORK 5000

/* 检查是否gnss异常不投票睡眠的次数:5s */
#define PL_CHECK_GNSS_VOTE_CNT (5000 / PLATFORM_SLEEP_TIME)

#define BFG_POWER_GPIO_DOWN 0
#define BFG_POWER_GPIO_UP   1

#define NONEED_SET_FLAG 0
#define NEED_SET_FLAG   1

#define DBG_FILE_TIME 600

#define UART_LOOP_RESUME_DEVICE_DATA_HANDLER 0
#define UART_LOOP_SET_DEVICE_DATA_HANDLER    1

#define SYS_INF_HEARTBEAT_CMD_BASE 128
#define BT_TYPE_DATA_LEN           1

#define DEV_THREAD_EXIT 0
#define DEV_THREAD_INIT 1

enum TTY_COMPLETE_TYPE_ENUM {
    TTY_LDISC_UNINSTALL = 0, /* 通知octty关闭uart */
    TTY_LDISC_INSTALL = 1,   /* 通知octty打开uart */
    TTY_LDISC_RECONFIG = 2,  /* 通知octty修改uart的配置参数，波特率和流控 */

    TTY_LDISC_TYPE_BUTT,
};

enum BFGN_DATA_MSG_TYPE_ENUM {
    SYS_MSG = 0x00,         /* 系统串口消息 */
    BT_MSG = 0x01,          /* BT串口消息 */
    GNSS_First_MSG = 0x02,  /* GNSS串口消息，第一个分段消息 */
    GNSS_Common_MSG = 0x03, /* GNSS串口消息，中间分段消息 */
    GNSS_Last_MSG = 0x04,   /* GNSS串口消息，最后一个分段消息 */
    FM_FIRST_MSG = 0x05,    /* FM串口消息，第一个分段消息 */
    FM_COMMON_MSG = 0x06,   /* FM串口消息，中间分段消息 */
    FM_LAST_MSG = 0x07,     /* FM串口消息，最后一个分段消息 */
    IR_FIRST_MSG = 0x08,    /* 红外串口消息，第一个分段消息 */
    IR_COMMON_MSG = 0x09,   /* 红外串口消息，中间分段消息 */
    IR_LAST_MSG = 0x0A,     /* 红外串口消息，最后一个分段消息 */
    NFC_First_MSG = 0x0B,   /* NFC串口消息，第一个分段消息 */
    NFC_Common_MSG = 0x0C,  /* NFC串口消息，中间分段消息 */
    NFC_Last_MSG = 0x0D,    /* NFC串口消息，最后一个分段消息 */
    OML_MSG = 0x0E,         /* 可维可测消息 */
    MEM_DUMP_SIZE = 0x0f,   /* bfgx异常时，要dump的mem长度消息 */
    MEM_DUMP = 0x10,        /* bfgx异常时，内存dump消息 */
    WIFI_MEM_DUMP = 0x11,   /* UART READ WIFI MEM，内存dump消息 */
    BFGX_CALI_MSG = 0x12,   /* BFGX 校准数据上报 */

    MSG_BUTT,
};

/* below is tx sys content define */
enum platform_event_enum {
    SYS_CFG_OPEN_BT = 0,
    SYS_CFG_CLOSE_BT = 1,
    SYS_CFG_OPEN_GNSS = 2,
    SYS_CFG_CLOSE_GNSS = 3,
    SYS_CFG_OPEN_FM = 4,
    SYS_CFG_CLOSE_FM = 5,
    SYS_CFG_OPEN_NFC = 6,
    SYS_CFG_CLOSE_NFC = 7,
    SYS_CFG_OPEN_IR = 8,
    SYS_CFG_CLOSE_IR = 9,
    SYS_CFG_OPEN_WIFI = 0xa,        /* host通过uart打开WCPU */
    SYS_CFG_CLOSE_WIFI = 0xb,       /* host通过uart关闭WCPU */
    SYS_CFG_READ_STACK = 0xc,       /* host通过uart读栈 */
    SYS_CFG_QUERY_RF_TEMP = 0xd,    /* host通过uart查询rf温度 */
    SYS_CFG_ALLOWDEV_SLP = 0xe,     /* host allow device sleep */
    SYS_CFG_DISALLOW_SLP = 0xf,     /* host disallow device sleep */
    SYS_CFG_SHUTDOWN_SLP = 0x10,    /* shutdown bcpu */
    SYS_CFG_PL_ENABLE_PM = 0x11,    /* enable plat dev lowpower feature */
    SYS_CFG_PL_DISABLE_PM = 0x12,   /* disable plat dev lowpower feature */
    SYS_CFG_BT_ENABLE_PM = 0x13,    /* enable BT dev lowpower feature */
    SYS_CFG_BT_DISABLE_PM = 0x14,   /* disable BT dev lowpower feature */
    SYS_CFG_GNSS_ENABLE_PM = 0x15,  /* enable GNSS dev lowpower feature */
    SYS_CFG_GNSS_DISABLE_PM = 0x16, /* disable GNSS dev lowpower feature */
    SYS_CFG_NFC_ENABLE_PM = 0x17,   /* enable NFC dev lowpower feature */
    SYS_CFG_NFC_DISABLE_PM = 0x18,  /* disable NFC dev lowpower feature */

    SYS_CFG_DEV_PANIC = 0x19,       /* cause device into exception */
    SYS_CFG_DUMP_RESET_WCPU = 0x1a, /* host通过uart不掉电复位WCPU */

    SYS_CFG_HALT_WCPU = 0x1b,          /* halt WCPU */
    SYS_CFG_READ_WLAN_PUB_REG = 0x1c,  /* 读取wcpu的公共寄存器 */
    SYS_CFG_READ_WLAN_PRIV_REG = 0x1d, /* 读取wcpu的私有寄存器 */
    SYS_CFG_READ_WLAN_MEM = 0x1e,      /* 读取wcpu的mem */
    SYS_CFG_EXCEP_RESET_WCPU = 0x1f,   /* wifi DFR WCPU复位 */

    SYS_CFG_SET_UART_LOOP_HANDLER = 0x20, /* 设置device侧uart环回处理函数 */
    SYS_CFG_SET_UART_LOOP_FINISH = 0x21,  /* UART环回test ok，恢复gnss消息处理函数 */

    PL_PM_DEBUG = 0x22,          /* 低功耗维测开关 */
    PL_BAUT_CHG_REQ_ACK = 0x23,  /* 波特率切换请求ACK */
    PL_BAUT_CHG_COMPLETE = 0x24, /* 波特率切换完成 */

    SYS_CFG_NOTIFY_WIFI_OPEN = 0x25,  /* wifi open  通知 */
    SYS_CFG_NOTIFY_WIFI_CLOSE = 0x26, /* wifi close 通知 */

    PL_EVT_BUTT
};

/* below is rx sys content define from device */
enum SYS_INF_MSG_VALUE_ENUM {
    SYS_INF_PF_INIT = 0x00,                /* 平台软件初始化完成 */
    SYS_INF_BT_INIT = 0x01,                /* BT软件初始化完成 */
    SYS_INF_GNSS_INIT = 0x02,              /* GNSS软件初始化完成 */
    SYS_INF_FM_INIT = 0x03,                /* FM软件初始化完成 */
    SYS_INF_BT_DISABLE = 0x04,             /* BT禁能 */
    SYS_INF_GNSS_DISABLE = 0x05,           /* GNSS禁能 */
    SYS_INF_FM_DISABLE = 0x06,             /* FM禁能 */
    SYS_INF_BT_EXIT = 0x07,                /* BT退出 */
    SYS_INF_GNSS_EXIT = 0x08,              /* GNSS退出 */
    SYS_INF_FM_EXIT = 0x09,                /* FM退出 */
    SYS_INF_GNSS_WAIT_DOWNLOAD = 0x0A,     /* 等待GNSS代码下载 */
    SYS_INF_GNSS_DOWNLOAD_COMPLETE = 0x0B, /* GNSS代码下载完毕 */
    SYS_INF_BFG_HEART_BEAT = 0x0C,         /* 心跳信号 */
    SYS_INF_DEV_AGREE_HOST_SLP = 0x0D,     /* device回复host可睡 */
    SYS_INF_DEV_NOAGREE_HOST_SLP = 0x0E,   /* device回复host不可睡 */
    SYS_INF_WIFI_OPEN = 0x0F,              /* WCPU上电完成 */
    SYS_INF_IR_INIT = 0x10,                /* IR软件初始化完成 */
    SYS_INF_IR_EXIT = 0x11,                /* IR退出 */
    SYS_INF_NFC_INIT = 0x12,               /* NFC软件初始化完成 */
    SYS_INF_NFC_EXIT = 0x13,               /* NFC退出 */
    SYS_INF_WIFI_CLOSE = 0x14,             /* WCPU下电完成 */
    SYS_INF_RF_TEMP_NORMAL = 0x15,         /* RF温度正常 */
    SYS_INF_RF_TEMP_OVERHEAT = 0x16,       /* RF温度过热 */
    SYS_INF_MEM_DUMP_COMPLETE = 0x17,      /* bfgx异常时，MEM DUMP已完成 */
    SYS_INF_WIFI_MEM_DUMP_COMPLETE = 0X18, /* bfgx异常时，MEM DUMP已完成 */
    SYS_INF_UART_HALT_WCPU = 0x19,         /* uart halt wcpu ok */
    SYS_INF_UART_LOOP_SET_DONE = 0x1a,     /* device 设置uart环回ok */
    SYS_INF_CHR_ERRNO_REPORT = 0x1b,       /* device向host上报CHR异常码 */

    SYS_INF_BAUD_CHG_2M_REQ = 0x1c,   /* device向host请求切换2M波特率 */
    SYS_INF_BAUD_CHG_6M_REQ = 0x1D,   /* device向host请求切换6M波特率 */
    SYS_INF_BAUD_CHG_COMP_ACK = 0x1E, /* device对host的波特率切换完成通知消息的ACK */

    SYS_INF_GNSS_LPPE_INIT = 0x1F,     /* 1103 GNSS 新增的线程初始化完成 */
    SYS_INF_GNSS_LPPE_EXIT = 0x20,     /* 1103 GNSS 新增的线程退出 */
    SYS_INF_GNSS_TRICKLE_SlEEP = 0x21, /* 1103 GNSS TRICKLE Sleep */

    SYS_INF_BUTT, /* 枚举定义最大不能超过127，因为128~255被用来当做心跳的时间戳 */
};

typedef enum {
    BFGX_BT = 0x00,
    BFGX_FM = 0x01,
    BFGX_GNSS = 0x02,
    BFGX_IR = 0x03,
    BFGX_NFC = 0x04,
    BFGX_BUTT = 0x05,
} bfgx_subsys_type_enum;

typedef enum {
    THREAD_FLAG_INIT = 0x00,
    THREAD_FLAG_EXIT = 0x01,
    THREAD_FLAG_BUTT = 0x02,
} bfgx_thread_flag_enum;

typedef enum {
    POWER_STATE_SHUTDOWN = 0,
    POWER_STATE_OPEN = 1,
    POWER_STATE_BUTT = 2,
} power_state_enum;

typedef enum {
    RX_SEQ_NULL = 0,
    RX_SEQ_START = 1,
    RX_SEQ_INT = 2,
    RX_SEQ_LAST = 3,

    RX_SEQ_BUTT,
} bfgx_rx_sepreted_pkt_type;

typedef enum {
    KZALLOC = 0,
    VMALLOC = 1,

    ALLOC_BUFF,
} alloc_type;

typedef enum {
    GNSS_SEPER_TAG_INIT,
    GNSS_SEPER_TAG_LAST,
} GNSS_SEPERATE_TAG;

typedef enum {
    SSI_MEM_TEST,
    SSI_FILE_TEST,
    SSI_TEXT_BUTT,
} SSI_TEST_TYPE;

/* STRUCT 定义 */
struct ps_pm_s {
    struct pm_drv_data *pm_priv_data;
    struct ps_core_s *ps_core_data;
    int32 (*change_baud_rate)(int64 baud_rate, uint8 enable_flowctl);
    int32 (*bfg_power_set)(uint8, uint8);
    void (*bfg_wake_lock_etc)(void);
    void (*bfg_wake_unlock_etc)(void);
    int32 (*bfgx_dev_state_get)(void);
    void (*bfgx_dev_state_set)(uint8);
    int32 (*operate_beat_timer)(uint8);
    void (*bfgx_uart_state_set)(uint8);
    int8 (*bfgx_uart_state_get)(void);

#ifdef BFGX_UART_DOWNLOAD_SUPPORT
    int32 (*download_patch)(void);
    int32 (*recv_patch)(const uint8 *, int32 count);
    int32 (*write_patch)(uint8 *, int32 count);
#endif
};

struct ps_packet_head {
    uint8 packet_start;
    uint8 func_type;
    uint16 packet_len;
};

struct ps_packet_end {
    uint8 packet_end;
};

struct bfgx_sepreted_rx_st {
    spinlock_t sepreted_rx_lock;
    uint8 *rx_buf_ptr;
    uint8 *rx_buf_org_ptr;
    uint16 rx_pkt_len;
    uint16 rx_buf_all_len;
    uint8 rx_prev_seq;
};

struct st_bfgx_data {
    struct sk_buff_head rx_queue;
    struct completion wait_opened;
    struct completion wait_closed;
    struct bfgx_sepreted_rx_st sepreted_rx; /* bfgx分包接收数据结构 */
    wait_queue_head_t rx_wait;              /* wait queue for rx packet */
    atomic_t subsys_state;
    char *name;
};

/*
 * struct ps_core_s - PS core internal structure
 * @tty: tty provided by the TTY core for line disciplines.
 * @txq: the list of skbs which needs to be sent onto the TTY.
 * @pm_data: reference to the parent encapsulating structure.
 *
 */
struct ps_core_s {
    struct ps_pm_s *ps_pm;
    struct tty_struct *tty;
    struct st_bfgx_data bfgx_info[BFGX_BUTT];
    /* queue head define for all sk_buff */
    struct sk_buff_head tx_urgent_seq;
    struct sk_buff_head tx_high_seq;
    struct sk_buff_head tx_low_seq;

    struct sk_buff_head rx_dbg_seq;
    /* wait queue for rx packet */
    wait_queue_head_t rx_dbg_wait;
    atomic_t dbg_func_has_open;
    uint16 dbg_read_delay;

    /* define for new workqueue */
    struct workqueue_struct *ps_tx_workqueue;
    /* define new work struct */
    struct work_struct tx_skb_work;
    /* wait for wifi opened */
    struct completion wait_wifi_opened;
    /* wait for wifi closed */
    struct completion wait_wifi_closed;

#ifdef PLATFORM_DEBUG_ENABLE
    struct file *rx_data_fp;
    uint64 pre_time;
    uint64 curr_time;
#endif

    spinlock_t rx_lock;
    /* define gnss read data lock */
    spinlock_t gnss_rx_lock;

    /* mark user doing something on node */
    atomic_t node_visit_flag;

    void *pm_data;

    /* define variable for decode rx data from uart */
    uint8 rx_pkt_type;
    uint16 rx_pkt_total_len;
    uint16 rx_have_recv_pkt_len;
    uint16 rx_have_del_public_len;
    uint8 *rx_decode_tty_ptr;
    uint8 *rx_public_buf_org_ptr;
    uint8 *rx_decode_public_buf_ptr;

    uint8 tty_have_open;
    uint16 gnss_read_delay;
    uint16 fm_read_delay;

    uint64 rx_pkt_num[BFGX_BUTT];
    uint64 tx_pkt_num[BFGX_BUTT];
    uint64 rx_pkt_sys;
    uint64 rx_pkt_oml;
};

/*
 * struct ps_plat_s - the PM internal data, embedded as the
 *  platform's drv data. One for each PS device in the system.
 * @pm_pdev: the platform device added in one of the board-XX.c file
 *  in arch/XX/ directory, 1 for each PS device.
 * @ldisc_installed: completion handler to notify that the OCTTY accepted
 *  the request to install ldisc, notify from tty_open which suggests
 *  the ldisc was properly installed.
 * @core_data: PS core's data, which mainly is the tty's disc_data
 *
 */
struct ps_plat_s {
    struct platform_device *pm_pdev;
    struct completion ldisc_uninstalled;
    struct completion ldisc_installed;
    struct completion ldisc_reconfiged;
    struct ps_core_s *core_data;
    uint8 dev_name[HISI_UART_DEV_NAME_LEN];
    uint64 baud_rate;
    uint8 ldisc_install;
    uint8 flow_cntrl;
    uint16 fill_byte1;
};

/*
 * struct hw_ps_plat_data - platform data shared between PS driver and
 *  platform specific board file which adds the PS device.
 * @dev_name: The UART/TTY name to which chip is interfaced. (eg: /dev/ttyS1)
 * @flow_cntrl: Should always be 1, since UART's CTS/RTS is used for PM
 *  purposes.
 * @baud_rate: The baud rate supported by the Host UART controller, this will
 *  be shared across with the chip via a HCI VS command from User-Space Init
 *  Mgr application.
 * @suspend:
 * @resume: legacy PM routines hooked to platform specific board file, so as
 *  to take chip-host interface specific action.
 * @set_bt_power: set gpio BT_EN to high.
 */
struct hw_ps_plat_data {
    int32 (*suspend)(struct platform_device *, pm_message_t);
    int32 (*resume)(struct platform_device *);
    void (*set_bt_power)(void);
    void (*set_fm_power)(void);
    void (*set_gnss_power)(void);
    void (*clear_bt_power)(void);
    void (*clear_fm_power)(void);
    void (*clear_gnss_power)(void);
    uint8 dev_name[HISI_UART_DEV_NAME_LEN]; /* uart name */
    uint64 baud_rate;
    uint8 flow_cntrl; /* flow control flag */
    uint16 fill_byte1;
    uint8 fill_byte2;
};

struct bt_data_combination {
    uint8 len;
    uint8 type;
    uint16 resverd;
};

typedef struct uart_loop_user_cfg {
    uint32 loop_count;
    uint16 pkt_len;
    uint16 cpu_fre;
    uint32 uart_loop_enable;
    uint32 uart_loop_tx_random_enable;
} uart_loop_cfg;

typedef struct uart_loop_test_etc {
    uart_loop_cfg *test_cfg;
    uint8 *tx_buf;
    uint8 *rx_buf;
    uint16 rx_pkt_len;
    uint16 result;
    struct completion set_done;
    struct completion loop_test_done;
} uart_loop_test_struct;

#ifdef BFGX_UART_DOWNLOAD_SUPPORT
typedef struct uart_download_test {
    uint8 baud[BAUD_STR_LEN];
    uint32 xmodern_len;
    uint32 file_len;

    uint32 used_time;
    int32 send_status;
} uart_download_test_st;
#endif /* BFGX_UART_DOWNLOAD_SUPPORT */

typedef struct ssi_trans_test {
    uint32 test_type;

    uint32 trans_len;
    uint32 used_time;
    int32 send_status;
} ssi_trans_test_st;

typedef struct _ssi_file_st {
    uint8 file_name[SSI_PATH_NAME_LEN];
    uint32 write_addr;
} ssi_file_st;
extern uart_loop_cfg uart_loop_test_cfg_etc;
extern uart_loop_test_struct *uart_loop_test_info_etc;
extern int32 uart_loop_test_recv_pkt_etc(struct ps_core_s *ps_core_d, const uint8 *buf_ptr, uint16 pkt_len);
extern int32 (*tty_recv_etc)(void *, const uint8 *, int32);
extern uint32 bfgx_rx_queue_etc[BFGX_BUTT];
extern const uint8 *bfgx_subsys_name_etc[BFGX_BUTT];
extern uint32 bfgx_rx_max_frame_etc[BFGX_BUTT];

extern uint32 gnss_me_thread_status;
extern uint32 gnss_lppe_thread_status;
extern volatile bool ir_only_mode;
extern uint8 uc_wakeup_src_debug;

/* 函数声明 */
extern struct platform_device *hw_ps_device_etc;

extern int32 ps_pm_register_etc(struct ps_pm_s *new_pm);

extern int32 ps_pm_unregister_etc(struct ps_pm_s *del_pm);

int32 ps_get_plat_reference_etc(struct ps_plat_s **plat_data);
extern int32 ps_exe_sys_func_etc(struct ps_core_s *ps_core_d, uint8 *buf_ptr);
extern int32 uart_wifi_open_etc(void);
extern int32 uart_wifi_close_etc(void);
extern int32 uart_bfgx_close_cmd_etc(void);
extern void dump_uart_rx_buf(void);
extern int hw_ps_init_etc(void);
extern void hw_ps_exit_etc(void);
extern int32 hw_ir_only_open_other_subsys(void);
extern int32 uart_loop_test_etc(void);

bool ps_is_device_log_enable_etc(void);

/*
 * ps_write_tty_etc -
 * point this to tty->driver->write or tty->ops->write
 * depending upon the kernel version
 */
int32 ps_write_tty_etc(struct ps_core_s *, const uint8 *, int32);
int32 ps_core_tx_attemper_etc(struct ps_core_s *ps_core_d);

/* init, exit entry funcs called from PM */
int32 ps_core_init_etc(struct ps_core_s **);
int32 ps_core_exit_etc(struct ps_core_s *);

/*
 * functions called when 1 of the protocol drivers gets
 * registered, these need to communicate with OCTTY to request
 * ldisc installed
 */
int32 ps_get_core_reference_etc(struct ps_core_s **core_data);
int32 ps_core_recv_etc(void *disc_data, const uint8 *data, int32 count);
int32 ps_core_recv_uart_test(void *disc_data, const uint8 *data, int32 count);
int32 ps_tx_sys_cmd_etc(struct ps_core_s *ps_core_d, uint8 type, uint8 content);
int32 ps_tx_urgent_cmd(struct ps_core_s *ps_core_d, uint8 type, uint8 content);
int32 ps_tx_gnssbuf_etc(struct ps_core_s *ps_core_d, const int8 __user *buf, size_t count);
int32 ps_tx_nfcbuf_etc(struct ps_core_s *ps_core_d, const int8 __user *buf, size_t count);
int32 ps_tx_irbuf_etc(struct ps_core_s *ps_core_d, const int8 __user *buf, size_t count);
int32 ps_tx_fmbuf_etc(struct ps_core_s *ps_core_d, const int8 __user *buf, size_t count);
int32 ps_skb_enqueue_etc(struct ps_core_s *ps_core_d, struct sk_buff *skb, uint8 type);
int32 ps_add_packet_head_etc(uint8 *buf, uint8 type, uint16 lenth);
void ps_kfree_skb_etc(struct ps_core_s *ps_core_d, uint8 type);
int32 hw_bfgx_open(uint32 subsys);
int32 hw_bfgx_close(uint32 subsys);
int32 bfgx_cali_data_init(void);
bool ps_chk_tx_queue_empty(struct ps_core_s *ps_core_d);
struct sk_buff *ps_alloc_skb_etc(uint16 len);
struct sk_buff *ps_skb_dequeue_etc(struct ps_core_s *ps_core_d, uint8 type);
int32 ps_patch_write(uint8 *data, int32 count);
int32 ps_change_uart_baud_rate_etc(int64 baud_rate, uint8 enable_flowctl);
uint8 check_bfg_not_booting(struct ps_core_s *ps_core_d);
void host_allow_bfg_sleep_etc(struct work_struct *work);
uint8 bfgx_wakeup_device(struct ps_core_s *ps_core_d, uint8 type);
int32 ps_check_packet_head_etc(struct ps_core_s *ps_core_d, uint8 *buf_ptr, int32 count);
bool ps_chk_bfg_active_etc(struct ps_core_s *ps_core_d);
int32 bfgx_open_cmd_send_etc(uint32 subsys);
int32 prepare_to_visit_node_etc(struct ps_core_s *ps_core_d);
int32 post_to_visit_node_etc(struct ps_core_s *ps_core_d);
void reset_uart_rx_buf_etc(void);
int32 ps_push_skb_queue_etc(struct ps_core_s *ps_core_d, uint8 *buf_ptr, uint16 pkt_len, uint8 type);
bool ps_chk_only_gnss_and_cldslp_etc(struct ps_core_s *ps_core_d);
extern void test_case_01(uint32 pkt_gen, uint32 pkt_len, uint8 test_type);
extern struct kset *devices_kset;

#endif /* HW_BFG_PS_H */
