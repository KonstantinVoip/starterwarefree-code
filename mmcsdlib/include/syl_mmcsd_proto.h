/*
 * \file	mmcsd_proto.h
 *
 * \brief	MMC/SD definitions
 *
 */

/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                               *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_fifodrv.h
*
* Description : definitions and declarations for ARM FIFO DRV driver kernel module
*
* Author      : Konstantin Shiluaev
*
******************************************************************************
******************************************************************************
*
* Module's Description Record:
* ============================
*
* $ProjectRevision: 0.0.1 $
* $Source:
* $State: Debug$
* $Revision: 0.0.1 $ $Name: $
* $Date: 2014/09/21 10:40:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcdrv.h $
* --------- Initial revision
******************************************************************************/
#include "stdlib.h"
#include "list.h"


/*Extern functions and macros*/



/*Typedef Testing for compilator */

//typedef signed char        s8;
//typedef unsigned char      u8;
//typedef unsigned char      uchar;
//typedef signed short       s16;
//typedef unsigned short     u16;
//typedef unsigned short     ushort;
//typedef signed int         s32;
//typedef unsigned int       u32;
//typedef unsigned int       uint;
//typedef signed long long   s64;
//typedef unsigned long long u64;
//typedef unsigned long long lbaint_t;

//typedef __signed__ char __s8;
//typedef unsigned char __u8;

//typedef __signed__ short __s16;
//typedef unsigned short __u16;

//typedef __signed__ int __s32;
//typedef unsigned int __u32;


//typedef __signed__ long long __s64;
//typedef unsigned long long __u64;



#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define ESRCH            3      /* No such process */
#define EINTR            4      /* Interrupted system call */
#define EIO              5      /* I/O error */
#define ENXIO            6      /* No such device or address */
#define E2BIG            7      /* Argument list too long */
#define ENOEXEC          8      /* Exec format error */
#define EBADF            9      /* Bad file number */
#define ECHILD          10      /* No child processes */
#define EAGAIN          11      /* Try again */
#define ENOMEM          12      /* Out of memory */
#define EACCES          13      /* Permission denied */
#define EFAULT          14      /* Bad address */
#define ENOTBLK         15      /* Block device required */
#define EBUSY           16      /* Device or resource busy */
#define EEXIST          17      /* File exists */
#define EXDEV           18      /* Cross-device link */
#define ENODEV          19      /* No such device */
#define ENOTDIR         20      /* Not a directory */
#define EISDIR          21      /* Is a directory */
#define EINVAL          22      /* Invalid argument */
#define ENFILE          23      /* File table overflow */
#define EMFILE          24      /* Too many open files */
#define ENOTTY          25      /* Not a typewriter */
#define ETXTBSY         26      /* Text file busy */
#define EFBIG           27      /* File too large */
#define ENOSPC          28      /* No space left on device */
#define ESPIPE          29      /* Illegal seek */
#define EROFS           30      /* Read-only file system */
#define EMLINK          31      /* Too many links */
#define EPIPE           32      /* Broken pipe */
#define EDOM            33      /* Math argument out of domain of func */
#define ERANGE          34      /* Math result not representable */
#define EMEDIUMTYPE     160
















#ifndef SYLMMCSD_LIB
#define SYLMMCSD_LIB

#define SD_VERSION_SD	0x20000
#define SD_VERSION_3	(SD_VERSION_SD | 0x300)
#define SD_VERSION_2	(SD_VERSION_SD | 0x200)
#define SD_VERSION_1_0	(SD_VERSION_SD | 0x100)
#define SD_VERSION_1_10	(SD_VERSION_SD | 0x10a)
#define MMC_VERSION_MMC		0x10000
#define MMC_VERSION_UNKNOWN	(MMC_VERSION_MMC)
#define MMC_VERSION_1_2		(MMC_VERSION_MMC | 0x102)
#define MMC_VERSION_1_4		(MMC_VERSION_MMC | 0x104)
#define MMC_VERSION_2_2		(MMC_VERSION_MMC | 0x202)
#define MMC_VERSION_3		(MMC_VERSION_MMC | 0x300)
#define MMC_VERSION_4		(MMC_VERSION_MMC | 0x400)
#define MMC_VERSION_4_1		(MMC_VERSION_MMC | 0x401)
#define MMC_VERSION_4_2		(MMC_VERSION_MMC | 0x402)
#define MMC_VERSION_4_3		(MMC_VERSION_MMC | 0x403)
#define MMC_VERSION_4_41	(MMC_VERSION_MMC | 0x429)
#define MMC_VERSION_4_5		(MMC_VERSION_MMC | 0x405)

#define MMC_MODE_HS		(1 << 0)
#define MMC_MODE_HS_52MHz	(1 << 1)
#define MMC_MODE_4BIT		(1 << 2)
#define MMC_MODE_8BIT		(1 << 3)
#define MMC_MODE_SPI		(1 << 4)
#define MMC_MODE_HC		(1 << 5)
#define MMC_MODE_DDR_52MHz	(1 << 6)

#define SD_DATA_4BIT	0x00040000

#define IS_SD(x) (x->version & SD_VERSION_SD)

#define MMC_DATA_READ		1
#define MMC_DATA_WRITE		2

#define NO_CARD_ERR		-16 /* No SD/MMC card inserted */
#define UNUSABLE_ERR		-17 /* Unusable Card */
#define COMM_ERR		-18 /* Communications Error */
#define TIMEOUT			-19
#define IN_PROGRESS		-20 /* operation is in progress */
#define SWITCH_ERR		-21 /* Card reports failure to switch mode */

#define MMC_CMD_GO_IDLE_STATE		0
#define MMC_CMD_SEND_OP_COND		1
#define MMC_CMD_ALL_SEND_CID		2
#define MMC_CMD_SET_RELATIVE_ADDR	3
#define MMC_CMD_SET_DSR			4
#define MMC_CMD_SWITCH			6
#define MMC_CMD_SELECT_CARD		7
#define MMC_CMD_SEND_EXT_CSD		8
#define MMC_CMD_SEND_CSD		9
#define MMC_CMD_SEND_CID		10
#define MMC_CMD_STOP_TRANSMISSION	12
#define MMC_CMD_SEND_STATUS		13
#define MMC_CMD_SET_BLOCKLEN		16
#define MMC_CMD_READ_SINGLE_BLOCK	17
#define MMC_CMD_READ_MULTIPLE_BLOCK	18
#define MMC_CMD_SET_BLOCK_COUNT         23
#define MMC_CMD_WRITE_SINGLE_BLOCK	24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK	25
#define MMC_CMD_ERASE_GROUP_START	35
#define MMC_CMD_ERASE_GROUP_END		36
#define MMC_CMD_ERASE			38
#define MMC_CMD_APP_CMD			55
#define MMC_CMD_SPI_READ_OCR		58
#define MMC_CMD_SPI_CRC_ON_OFF		59
#define MMC_CMD_RES_MAN			62

#define MMC_CMD62_ARG1			0xefac62ec
#define MMC_CMD62_ARG2			0xcbaea7


#define SD_CMD_SEND_RELATIVE_ADDR	3
#define SD_CMD_SWITCH_FUNC		6
#define SD_CMD_SEND_IF_COND		8

#define SD_CMD_APP_SET_BUS_WIDTH	6
#define SD_CMD_ERASE_WR_BLK_START	32
#define SD_CMD_ERASE_WR_BLK_END		33
#define SD_CMD_APP_SEND_OP_COND		41
#define SD_CMD_APP_SEND_SCR		51

/* SCR definitions in different words */
#define SD_HIGHSPEED_BUSY	0x00020000
#define SD_HIGHSPEED_SUPPORTED	0x00020000

#define OCR_BUSY		0x80000000
#define OCR_HCS			0x40000000
#define OCR_VOLTAGE_MASK	0x007FFF80
#define OCR_ACCESS_MODE		0x60000000

#define SECURE_ERASE		0x80000000

#define MMC_STATUS_MASK		(~0x0206BF7F)
#define MMC_STATUS_SWITCH_ERROR	(1 << 7)
#define MMC_STATUS_RDY_FOR_DATA (1 << 8)
#define MMC_STATUS_CURR_STATE	(0xf << 9)
#define MMC_STATUS_ERROR	(1 << 19)

#define MMC_STATE_PRG		(7 << 9)

#define MMC_VDD_165_195		0x00000080	/* VDD voltage 1.65 - 1.95 */
#define MMC_VDD_20_21		0x00000100	/* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22		0x00000200	/* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23		0x00000400	/* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24		0x00000800	/* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25		0x00001000	/* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26		0x00002000	/* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27		0x00004000	/* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28		0x00008000	/* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29		0x00010000	/* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30		0x00020000	/* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31		0x00040000	/* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32		0x00080000	/* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33		0x00100000	/* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34		0x00200000	/* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35		0x00400000	/* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36		0x00800000	/* VDD voltage 3.5 ~ 3.6 */

#define MMC_SWITCH_MODE_CMD_SET		0x00 /* Change the command set */
#define MMC_SWITCH_MODE_SET_BITS	0x01 /* Set bits in EXT_CSD byte
						addressed by index which are
						1 in value field */
#define MMC_SWITCH_MODE_CLEAR_BITS	0x02 /* Clear bits in EXT_CSD byte
						addressed by index, which are
						1 in value field */
#define MMC_SWITCH_MODE_WRITE_BYTE	0x03 /* Set target byte to value */

#define SD_SWITCH_CHECK		0
#define SD_SWITCH_SWITCH	1

/*
 * EXT_CSD fields
 */
#define EXT_CSD_GP_SIZE_MULT		143	/* R/W */
#define EXT_CSD_PARTITIONS_ATTRIBUTE	156	/* R/W */
#define EXT_CSD_PARTITIONING_SUPPORT	160	/* RO */
#define EXT_CSD_RST_N_FUNCTION		162	/* R/W */
#define EXT_CSD_RPMB_MULT		168	/* RO */
#define EXT_CSD_ERASE_GROUP_DEF		175	/* R/W */
#define EXT_CSD_BOOT_BUS_WIDTH		177
#define EXT_CSD_PART_CONF		179	/* R/W */
#define EXT_CSD_BUS_WIDTH		183	/* R/W */
#define EXT_CSD_HS_TIMING		185	/* R/W */
#define EXT_CSD_REV			192	/* RO */
#define EXT_CSD_CARD_TYPE		196	/* RO */
#define EXT_CSD_SEC_CNT			212	/* RO, 4 bytes */
#define EXT_CSD_HC_WP_GRP_SIZE		221	/* RO */
#define EXT_CSD_HC_ERASE_GRP_SIZE	224	/* RO */
#define EXT_CSD_BOOT_MULT		226	/* RO */

/*
 * EXT_CSD field definitions
 */

#define EXT_CSD_CMD_SET_NORMAL		(1 << 0)
#define EXT_CSD_CMD_SET_SECURE		(1 << 1)
#define EXT_CSD_CMD_SET_CPSECURE	(1 << 2)

#define EXT_CSD_CARD_TYPE_26	(1 << 0)	/* Card can run at 26MHz */
#define EXT_CSD_CARD_TYPE_52	(1 << 1)	/* Card can run at 52MHz */
#define EXT_CSD_CARD_TYPE_DDR_1_8V	(1 << 2)
#define EXT_CSD_CARD_TYPE_DDR_1_2V	(1 << 3)
#define EXT_CSD_CARD_TYPE_DDR_52	(EXT_CSD_CARD_TYPE_DDR_1_8V \
					| EXT_CSD_CARD_TYPE_DDR_1_2V)

#define EXT_CSD_BUS_WIDTH_1	0	/* Card is in 1 bit mode */
#define EXT_CSD_BUS_WIDTH_4	1	/* Card is in 4 bit mode */
#define EXT_CSD_BUS_WIDTH_8	2	/* Card is in 8 bit mode */
#define EXT_CSD_DDR_BUS_WIDTH_4	5	/* Card is in 4 bit DDR mode */
#define EXT_CSD_DDR_BUS_WIDTH_8	6	/* Card is in 8 bit DDR mode */

#define EXT_CSD_BOOT_ACK_ENABLE			(1 << 6)
#define EXT_CSD_BOOT_PARTITION_ENABLE		(1 << 3)
#define EXT_CSD_PARTITION_ACCESS_ENABLE		(1 << 0)
#define EXT_CSD_PARTITION_ACCESS_DISABLE	(0 << 0)

#define EXT_CSD_BOOT_ACK(x)		(x << 6)
#define EXT_CSD_BOOT_PART_NUM(x)	(x << 3)
#define EXT_CSD_PARTITION_ACCESS(x)	(x << 0)

#define EXT_CSD_BOOT_BUS_WIDTH_MODE(x)	(x << 3)
#define EXT_CSD_BOOT_BUS_WIDTH_RESET(x)	(x << 2)
#define EXT_CSD_BOOT_BUS_WIDTH_WIDTH(x)	(x)

#define R1_ILLEGAL_COMMAND		(1 << 22)
#define R1_APP_CMD			(1 << 5)

#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136	(1 << 1)		/* 136 bit response */
#define MMC_RSP_CRC	(1 << 2)		/* expect valid crc */
#define MMC_RSP_BUSY	(1 << 3)		/* card may send busy */
#define MMC_RSP_OPCODE	(1 << 4)		/* response contains opcode */

#define MMC_RSP_NONE	(0)
#define MMC_RSP_R1	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1b	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE| \
			MMC_RSP_BUSY)
#define MMC_RSP_R2	(MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3	(MMC_RSP_PRESENT)
#define MMC_RSP_R4	(MMC_RSP_PRESENT)
#define MMC_RSP_R5	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R6	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

#define MMCPART_NOAVAILABLE	(0xff)
#define PART_ACCESS_MASK	(0x7)
#define PART_SUPPORT		(0x1)
#define PART_ENH_ATTRIB		(0x1f)

/* Maximum block size for MMC */
#define MMC_MAX_BLOCK_LEN	512

/* The number of MMC physical partitions.  These consist of:
 * boot partitions (2), general purpose partitions (4) in MMC v4.4.
 */
#define MMC_NUM_BOOT_PARTITION	2
#define MMC_PART_RPMB           3       /* RPMB partition number */

/*
#define BLOCK_CNT(size, block_dev_desc) (PAD_COUNT(size, block_dev_desc->blksz))
#define PAD_TO_BLOCKSIZE(size, block_dev_desc) \
	(PAD_SIZE(size, block_dev_desc->blksz))
*/






#define LOG2(x) (((x & 0xaaaaaaaa) ? 1 : 0) + ((x & 0xcccccccc) ? 2 : 0) + \
		 ((x & 0xf0f0f0f0) ? 4 : 0) + ((x & 0xff00ff00) ? 8 : 0) + \
		 ((x & 0xffff0000) ? 16 : 0))



typedef struct block_dev_desc {
	int		if_type;	/* type of the interface */
	int		dev;		/* device number */
	unsigned char	part_type;	/* partition type */
	unsigned char	target;		/* target SCSI ID */
	unsigned char	lun;		/* target LUN */
	unsigned char	type;		/* device type */
	unsigned char	removable;	/* removable device */
#ifdef CONFIG_LBA48
	unsigned char	lba48;		/* device can use 48bit addr (ATA/ATAPI v7) */
#endif
	unsigned long	lba;		/* number of blocks */
	lldiv_t         lba_c;
	unsigned long	blksz;		/* block size */
	int		    log2blksz;	/* for convenience: log2(blksz) */
	char		vendor [40+1];	/* IDE model, SCSI Vendor */
	char		product[20+1];	/* IDE Serial no, SCSI product */
	char		revision[8+1];	/* firmware revision */
	unsigned int	(*block_read)(int dev ,unsigned int start,unsigned  int blkcnt,void *buffer);
	unsigned int	(*block_write)(int dev,unsigned int start,unsigned   int blkcnt,const void *buffer);
	unsigned int    (*block_erase)(int dev,unsigned int start,unsigned  int cnt);
	void		*priv;		/* driver private struct pointer */
}block_dev_desc_t;




struct mmc_cid {
	unsigned long psn;
	unsigned short oid;
	unsigned char mid;
	unsigned char prv;
	unsigned char mdt;
	char pnm[7];
};

struct mmc_cmd {
	unsigned short cmdidx;
	unsigned int resp_type;
	unsigned int cmdarg;
	unsigned int response[4];
};

struct mmc_data {
	union {
		char *dest;
		const char *src; /* src buffers don't get written to */
	};
	unsigned int flags;
	unsigned int blocks;
	unsigned int blocksize;
};

/* forward decl. */
struct mmc;

struct mmc_ops {
	int (*send_cmd)(struct mmc *mmc,
			struct mmc_cmd *cmd, struct mmc_data *data);
	void (*set_ios)(struct mmc *mmc);
	int (*init)(struct mmc *mmc);
	int (*getcd)(struct mmc *mmc);
	int (*getwp)(struct mmc *mmc);
};

struct mmc_config {
	const char *name;
	const struct mmc_ops *ops;
	unsigned int host_caps;
	unsigned int voltages;
	unsigned int f_min;
	unsigned int f_max;
	unsigned int b_max;
	unsigned char part_type;
};

/* TODO struct mmc should be in mmc_private but it's hard to fix right now */
struct mmc {
	struct list_head link;
	const struct mmc_config *cfg;	/* provided configuration */
	unsigned int version;
	void *priv;
	unsigned int has_init;
	int high_capacity;
	unsigned int bus_width;
	unsigned int clock;
	unsigned int card_caps;
	unsigned int ocr;
	unsigned int dsr;
	unsigned int dsr_imp;
	unsigned int scr[2];
	unsigned int csd[4];
	unsigned int cid[4];
	unsigned short rca;
	char part_config;
	char part_num;
	unsigned int tran_speed;
	unsigned int read_bl_len;
	unsigned int write_bl_len;
	unsigned int erase_grp_size;
	unsigned long long capacity;
	unsigned long long capacity_user;
	unsigned long long capacity_boot;
	unsigned long long capacity_rpmb;
	unsigned long long capacity_gp[4];
	block_dev_desc_t block_dev;
	char op_cond_pending;	/* 1 if we are waiting on an op_cond command */
	char init_in_progress;	/* 1 if we have done mmc_start_init() */
	char preinit;		/* start init as early as possible */
	unsigned int op_cond_response;	/* the response byte from the last op_cond */
};




/*Read and write from registers*/
extern void writel(unsigned int val ,void * addr);
extern unsigned int readl(void * addr);

/*Extern Declaration functions */
extern int mmc_initialize(unsigned int dev) ;
extern int mmc_init(struct mmc *mmc);
extern struct mmc *find_mmc_device(int dev_num);


/*Extern read and write functions*/
extern int mmc_set_boot_bus_width(struct mmc *mmc, unsigned char width, unsigned char reset,unsigned char mode);



int mmc_register(struct mmc *mmc);
struct mmc *mmc_create(const struct mmc_config *cfg, void *priv);
void mmc_destroy(struct mmc *mmc);
//int mmc_initialize(bd_t *bis);
//int mmc_init(struct mmc *mmc);
int mmc_read(struct mmc *mmc, unsigned long long src, unsigned char *dst, int size);
void mmc_set_clock(struct mmc *mmc, unsigned int clock);
//struct mmc *find_mmc_device(int dev_num);
int mmc_set_dev(int dev_num);
void print_mmc_devices(char separator);
int get_mmc_num(void);
int board_mmc_getcd(struct mmc *mmc);
int mmc_switch_part(int dev_num, unsigned int part_num);
int mmc_getcd(struct mmc *mmc);
int mmc_getwp(struct mmc *mmc);
int mmc_set_dsr(struct mmc *mmc, unsigned short val);
/* Function to change the size of boot partition and rpmb partitions */
int mmc_boot_partition_size_change(struct mmc *mmc, unsigned long bootsize,unsigned long rpmbsize);
/* Function to modify the PARTITION_CONFIG field of EXT_CSD */
int mmc_set_part_conf(struct mmc *mmc, unsigned char ack, unsigned char part_num, unsigned char access);
/* Function to modify the BOOT_BUS_WIDTH field of EXT_CSD */
int mmc_set_boot_bus_width(struct mmc *mmc, unsigned char width, unsigned char reset, unsigned char mode);
/* Function to modify the RST_n_FUNCTION field of EXT_CSD */
int mmc_set_rst_n_function(struct mmc *mmc, unsigned char enable);
/* Functions to read / write the RPMB partition */
int mmc_rpmb_set_key(struct mmc *mmc, void *key);
int mmc_rpmb_get_counter(struct mmc *mmc, unsigned long *counter);
int mmc_rpmb_read(struct mmc *mmc, void *addr, unsigned short blk,unsigned short cnt, unsigned char *key);
int mmc_rpmb_write(struct mmc *mmc, void *addr, unsigned short blk,unsigned short cnt, unsigned char *key);
/**
 * Start device initialization and return immediately; it does not block on
 * polling OCR (operation condition register) status.  Then you should call
 * mmc_init, which would block on polling OCR status and complete the device
 * initializatin.
 *
 * @param mmc	Pointer to a MMC device struct
 * @return 0 on success, IN_PROGRESS on waiting for OCR status, <0 on error.
 */
int mmc_start_init(struct mmc *mmc);

/**
 * Set preinit flag of mmc device.
 *
 * This will cause the device to be pre-inited during mmc_initialize(),
 * which may save boot time if the device is not accessed until later.
 * Some eMMC devices take 200-300ms to init, but unfortunately they
 * must be sent a series of commands to even get them to start preparing
 * for operation.
 *
 * @param mmc		Pointer to a MMC device struct
 * @param preinit	preinit flag value
 */
void mmc_set_preinit(struct mmc *mmc, int preinit);

#ifdef CONFIG_GENERIC_MMC
#ifdef CONFIG_MMC_SPI
#define mmc_host_is_spi(mmc)	((mmc)->cfg->host_caps & MMC_MODE_SPI)
#else
#define mmc_host_is_spi(mmc)	0
#endif
struct mmc *mmc_spi_init(uint bus, uint cs, uint speed, uint mode);
#else
int mmc_legacy_init(int verbose);
#endif



/* Set block count limit because of 16 bit register limit on some hardware*/
#ifndef CONFIG_SYS_MMC_MAX_BLK_COUNT
#define CONFIG_SYS_MMC_MAX_BLK_COUNT 65535




///////////////////////////////////////////////ADD OMAP functions/////////////////////////////////////
///////////////////////////////////////////////ADD OMAP functions/////////////////////////////////////
///////////////////////////////////////////////ADD OMAP functions/////////////////////////////////////
///////////////////////////////////////////////ADD OMAP functions/////////////////////////////////////
///////////////////////////////////////////////ADD OMAP functions/////////////////////////////////////
///////////////////////////////////////////////ADD OMAP functions/////////////////////////////////////
///////////////////////////////////////////////ADD OMAP functions/////////////////////////////////////
///////////////////////////////////////////////ADD OMAP functions/////////////////////////////////////
///////////////////////////////////////////////ADD OMAP functions/////////////////////////////////////
/* common definitions for all OMAPs */
struct hsmmc {
	unsigned char res1[0x10];
	unsigned int sysconfig;		/* 0x10 */
	unsigned int sysstatus;		/* 0x14 */
	unsigned char res2[0x14];
	unsigned int con;		    /* 0x2C */
	unsigned char res3[0xD4];
	unsigned int blk;		    /* 0x104 */
	unsigned int arg;		    /* 0x108 */
	unsigned int cmd;		    /* 0x10C */
	unsigned int rsp10;		    /* 0x110 */
	unsigned int rsp32;		    /* 0x114 */
	unsigned int rsp54;		    /* 0x118 */
	unsigned int rsp76;		    /* 0x11C */
	unsigned int data;		    /* 0x120 */
	unsigned int pstate;		/* 0x124 */
	unsigned int hctl;		    /* 0x128 */
	unsigned int sysctl;		/* 0x12C */
	unsigned int stat;		    /* 0x130 */
	unsigned int ie;		    /* 0x134 */
	unsigned char res4[0x8];
	unsigned int capa;		    /* 0x140 */
};


/*
 * OMAP HS MMC Bit definitions
 */
#define MMC_SOFTRESET			(0x1 << 1)
#define RESETDONE			(0x1 << 0)
#define NOOPENDRAIN			(0x0 << 0)
#define OPENDRAIN			(0x1 << 0)
#define OD				(0x1 << 0)
#define INIT_NOINIT			(0x0 << 1)
#define INIT_INITSTREAM			(0x1 << 1)
#define HR_NOHOSTRESP			(0x0 << 2)
#define STR_BLOCK			(0x0 << 3)
#define MODE_FUNC			(0x0 << 4)
#define DW8_1_4BITMODE			(0x0 << 5)
#define MIT_CTO				(0x0 << 6)
#define CDP_ACTIVEHIGH			(0x0 << 7)
#define WPP_ACTIVEHIGH			(0x0 << 8)
#define RESERVED_MASK			(0x3 << 9)
#define CTPL_MMC_SD			(0x0 << 11)
#define BLEN_512BYTESLEN		(0x200 << 0)
#define NBLK_STPCNT			(0x0 << 16)
#define DE_DISABLE			(0x0 << 0)
#define BCE_DISABLE			(0x0 << 1)
#define BCE_ENABLE			(0x1 << 1)
#define ACEN_DISABLE			(0x0 << 2)
#define DDIR_OFFSET			(4)
#define DDIR_MASK			(0x1 << 4)
#define DDIR_WRITE			(0x0 << 4)
#define DDIR_READ			(0x1 << 4)
#define MSBS_SGLEBLK			(0x0 << 5)
#define MSBS_MULTIBLK			(0x1 << 5)
#define RSP_TYPE_OFFSET			(16)
#define RSP_TYPE_MASK			(0x3 << 16)
#define RSP_TYPE_NORSP			(0x0 << 16)
#define RSP_TYPE_LGHT136		(0x1 << 16)
#define RSP_TYPE_LGHT48			(0x2 << 16)
#define RSP_TYPE_LGHT48B		(0x3 << 16)
#define CCCE_NOCHECK			(0x0 << 19)
#define CCCE_CHECK			(0x1 << 19)
#define CICE_NOCHECK			(0x0 << 20)
#define CICE_CHECK			(0x1 << 20)
#define DP_OFFSET			(21)
#define DP_MASK				(0x1 << 21)
#define DP_NO_DATA			(0x0 << 21)
#define DP_DATA				(0x1 << 21)
#define CMD_TYPE_NORMAL			(0x0 << 22)
#define INDEX_OFFSET			(24)
#define INDEX_MASK			(0x3f << 24)
#define INDEX(i)			(i << 24)
#define DATI_MASK			(0x1 << 1)
#define CMDI_MASK			(0x1 << 0)
#define DTW_1_BITMODE			(0x0 << 1)
#define DTW_4_BITMODE			(0x1 << 1)
#define DTW_8_BITMODE                   (0x1 << 5) /* CON[DW8]*/
#define SDBP_PWROFF			(0x0 << 8)
#define SDBP_PWRON			(0x1 << 8)
#define SDVS_1V8			(0x5 << 9)
#define SDVS_3V0			(0x6 << 9)
#define ICE_MASK			(0x1 << 0)
#define ICE_STOP			(0x0 << 0)
#define ICS_MASK			(0x1 << 1)
#define ICS_NOTREADY			(0x0 << 1)
#define ICE_OSCILLATE			(0x1 << 0)
#define CEN_MASK			(0x1 << 2)
#define CEN_DISABLE			(0x0 << 2)
#define CEN_ENABLE			(0x1 << 2)
#define CLKD_OFFSET			(6)
#define CLKD_MASK			(0x3FF << 6)
#define DTO_MASK			(0xF << 16)
#define DTO_15THDTO			(0xE << 16)
#define SOFTRESETALL			(0x1 << 24)
#define CC_MASK				(0x1 << 0)
#define TC_MASK				(0x1 << 1)
#define BWR_MASK			(0x1 << 4)
#define BRR_MASK			(0x1 << 5)
#define ERRI_MASK			(0x1 << 15)
#define IE_CC				(0x01 << 0)
#define IE_TC				(0x01 << 1)
#define IE_BWR				(0x01 << 4)
#define IE_BRR				(0x01 << 5)
#define IE_CTO				(0x01 << 16)
#define IE_CCRC				(0x01 << 17)
#define IE_CEB				(0x01 << 18)
#define IE_CIE				(0x01 << 19)
#define IE_DTO				(0x01 << 20)
#define IE_DCRC				(0x01 << 21)
#define IE_DEB				(0x01 << 22)
#define IE_CERR				(0x01 << 28)
#define IE_BADA				(0x01 << 29)

#define VS30_3V0SUP			(1 << 25)
#define VS18_1V8SUP			(1 << 26)

/* Driver definitions */
#define MMCSD_SECTOR_SIZE		512
#define MMC_CARD			0
#define SD_CARD				1
#define BYTE_MODE			0
#define SECTOR_MODE			1
#define CLK_INITSEQ			0
#define CLK_400KHZ			1
#define CLK_MISC			2

#define RSP_TYPE_NONE	(RSP_TYPE_NORSP   | CCCE_NOCHECK | CICE_NOCHECK)
#define MMC_CMD0	(INDEX(0)  | RSP_TYPE_NONE | DP_NO_DATA | DDIR_WRITE)

/* Clock Configurations and Macros */
#define MMC_CLOCK_REFERENCE	96 /* MHz */

#define mmc_reg_out(addr, mask, val)\
	writel((readl(addr) & (~(mask))) | ((val) & (mask)), (addr))

int omap_mmc_init(int dev_index, unsigned int host_caps_mask, unsigned int f_max, int cd_gpio,int wp_gpio);



#endif /* ABARMFIFODRV_H */






#endif /* ABARMFIFODRV_H */
