#include "beaglebone.h"

#include "edma_event.h"
#include "soc_AM335x.h"
#include "hw_control_AM335x.h"
#include "hw_cm_per.h"
#include "interrupt.h"
#include "consoleUtils.h"

#include "string.h"
#include "delay.h"
#include "cache.h"
#include "edma.h"
#include "mmu.h"
#include "consoleUtils.h"

#include "syl_mmcsd_proto.h"



#define   CONFIG_AM33XX  1


//stavim 1 if only  eMMC flash
//stavim 0 if only  SD card
  static int cur_dev_num = 1;
//static int cur_dev_num = 0;



#define LBAF "%llx"
#define LBAFU "%llu"


// ALLOC_CACHE_ALIGN_BUFFER(type, name, size);
 #define IF_TYPE_MMC		6
 #define IF_TYPE_SD		    7
 #define IF_TYPE_SATA		8
 #define IF_TYPE_SDHC		9


 #define OMAP_HSMMC1_BASE		0x48060100  //MMC0
 #define OMAP_HSMMC2_BASE		0x481D8100  //MMC1



//#define DEBUG 1
//Ошибка консоли UART
//#define DEBUG_ERROR_UART_CONSOLE  1
//#define  DEBUG_TIMING_PATCH      1


#define  ARRAY_SIZE 			0x5




 //ENABLE LIST DEVICE SUPPORT
static struct list_head mmc_devices;


unsigned int  mmc_berase(int dev_num, /*lbaint_t*/unsigned int start,unsigned int cnt /*lbaint_t blkcnt*/);
unsigned int  mmc_bwrite(int dev_num, /*lbaint_t*/unsigned int start, /*lbaint_t*/unsigned int blkcnt, const void *src);
static unsigned  int  mmc_bread(int dev_num, unsigned int start, unsigned int blkcnt, void *dst);

static int    mmc_read_data(struct hsmmc *mmc_base, char *buf, unsigned int size);
static int    mmc_write_data(struct hsmmc *mmc_base, const char *buf,unsigned int siz);







/* simplify defines to OMAP_HSMMC_USE_GPIO */
#if (defined(CONFIG_OMAP_GPIO) && !defined(CONFIG_SPL_BUILD)) || \
	(defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_GPIO_SUPPORT))
#define OMAP_HSMMC_USE_GPIO
#else
#undef OMAP_HSMMC_USE_GPIO
#endif

/* common definitions for all OMAPs */
#define SYSCTL_SRC	(1 << 25)
#define SYSCTL_SRD	(1 << 26)

struct omap_hsmmc_data {
	struct hsmmc *base_addr;
	struct mmc_config cfg;
#ifdef OMAP_HSMMC_USE_GPIO
	int cd_gpio;
	int wp_gpio;
#endif
};

/* If we fail after 1 second wait, something is really bad */
#define MAX_RETRY_MS	1000





/*****************************************************************************
Syntax:      	    int writel(unsigned int val ,void * addr)
Return Value:	    write to reg value
*******************************************************************************/
static void inline writel(unsigned int val ,void * addr)
{

	HWREG(addr)  = val;

}

/*****************************************************************************
Syntax:      	    int readl(void * addr)
Return Value:	    Returns 1 reg value.
*******************************************************************************/
static unsigned int inline readl(void * addr)
{

	return HWREG(addr);
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int board_mmc_getwp(struct mmc *mmc)
{
	return -1;
}

int mmc_getwp(struct mmc *mmc)
{
	int wp;

	wp = board_mmc_getwp(mmc);

	if (wp < 0) {
		if (mmc->cfg->ops->getwp)
			wp = mmc->cfg->ops->getwp(mmc);
		else
			wp = 0;
	}

	return wp;
}


/*
int __board_mmc_getcd(struct mmc *mmc)
{
	return -1;
}
int board_mmc_getcd(struct mmc *mmc)__attribute__((weak,alias("__board_mmc_getcd")));
*/



int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	int ret;

#ifdef CONFIG_MMC_TRACE
	int i;
	u8 *ptr;

	printf("CMD_SEND:%d\n", cmd->cmdidx);
	printf("\t\tARG\t\t\t 0x%08X\n", cmd->cmdarg);
	ret = mmc->cfg->ops->send_cmd(mmc, cmd, data);

	switch (cmd->resp_type)
	{
		case MMC_RSP_NONE:
			printf("\t\tMMC_RSP_NONE\n");
			break;
		case MMC_RSP_R1:
			printf("\t\tMMC_RSP_R1,5,6,7 \t 0x%08X \n",cmd->response[0]);
			break;
		case MMC_RSP_R1b:
			printf("\t\tMMC_RSP_R1b\t\t 0x%08X \n",
				cmd->response[0]);
			break;
		case MMC_RSP_R2:
			printf("\t\tMMC_RSP_R2\t\t 0x%08X \n",
				cmd->response[0]);
			printf("\t\t          \t\t 0x%08X \n",
				cmd->response[1]);
			printf("\t\t          \t\t 0x%08X \n",
				cmd->response[2]);
			printf("\t\t          \t\t 0x%08X \n",
				cmd->response[3]);
			printf("\n");
			printf("\t\t\t\t\tDUMPING DATA\n");
			for (i = 0; i < 4; i++) {
				int j;
				printf("\t\t\t\t\t%03d - ", i*4);
				ptr = (u8 *)&cmd->response[i];
				ptr += 3;
				for (j = 0; j < 4; j++)
					printf("%02X ", *ptr--);
				printf("\n");
			}
			break;
		case MMC_RSP_R3:
			printf("\t\tMMC_RSP_R3,4\t\t 0x%08X \n",
				cmd->response[0]);
			break;
		default:
			printf("\t\tERROR MMC rsp not supported\n");
			break;
	}
#else
	ret = mmc->cfg->ops->send_cmd(mmc, cmd, data);
#endif
	return ret;
}



static int mmc_send_status(struct mmc *mmc, int timeout)
{
	struct mmc_cmd cmd;
	int err, retries = 5;
//#ifdef CONFIG_MMC_TRACE
	int status;
//#endif

	cmd.cmdidx = MMC_CMD_SEND_STATUS;
	cmd.resp_type = MMC_RSP_R1;



	//if (!mmc_host_is_spi(mmc))
		cmd.cmdarg = mmc->rca << 16;

	do {
		err = mmc_send_cmd(mmc, &cmd, NULL);
		if (!err) {
			if ((cmd.response[0] & MMC_STATUS_RDY_FOR_DATA) &&
			    (cmd.response[0] & MMC_STATUS_CURR_STATE) !=
			     MMC_STATE_PRG)
				break;
			else if (cmd.response[0] & MMC_STATUS_MASK) {
#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)
				printf("Status Error: 0x%08X\n",
					cmd.response[0]);
#endif
				return COMM_ERR;
			}
		} else if (--retries < 0)
			return err;


		 syl_Sysdelay(5); //5ms
		//printf("delay_1000\n\r");
		//udelay(1000);  //1_ms
		//Sysdelay(1);

	} while (timeout--);

//#ifdef CONFIG_MMC_TRACE
	status = (cmd.response[0] & MMC_STATUS_CURR_STATE) >> 9;
	printf("CURR STATE:%d\n", status);
//#endif
	if (timeout <= 0) {
#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)
		printf("Timeout waiting card ready\n");
#endif
		return TIMEOUT;
	}
	if (cmd.response[0] & MMC_STATUS_SWITCH_ERROR)
		return SWITCH_ERR;

	return 0;
}

int mmc_set_blocklen(struct mmc *mmc, int len)
{
	struct mmc_cmd cmd;
    printf("+mmc.c/mmc_set_blocklen=%d\n+",len);
	if (mmc->card_caps & MMC_MODE_DDR_52MHz)
		return 0;

	cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = len;

	return mmc_send_cmd(mmc, &cmd, NULL);
}

struct mmc *find_mmc_device(int dev_num)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices)
	{
		m = list_entry(entry, struct mmc, link);

		if (m->block_dev.dev == dev_num)
			return m;
	}

#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)
	printf("MMC Device %d not found\n", dev_num);
#endif

	return NULL;
}

static int mmc_read_blocks(struct mmc *mmc, void *dst, unsigned long long start,unsigned long long blkcnt)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	printf("+mmc.c/mmc_read_blocks+\n");
	if (blkcnt > 1)
		cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * mmc->read_bl_len;

	cmd.resp_type = MMC_RSP_R1;

	data.dest = dst;
	data.blocks = blkcnt;
	data.blocksize = mmc->read_bl_len;
	data.flags = MMC_DATA_READ;

	if (mmc_send_cmd(mmc, &cmd, &data))
	{
		return 0;
	}

	if (blkcnt > 1)
	{
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1b;
		if (mmc_send_cmd(mmc, &cmd, NULL))
		{
			#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)
			printf("mmc fail to send stop cmd\n");
			#endif
			return 0;
		}
	}

	return blkcnt;
}


static unsigned int mmc_bread(int dev_num, unsigned int start, unsigned int blkcnt, void *dst)
{
	unsigned int cur, blocks_todo = blkcnt;

	if (blkcnt == 0)
	{
		return 0;
	}

	struct mmc *mmc = find_mmc_device(dev_num);
	if (!mmc)
	{
		return 0;
	}

	//TODO popravlu
/*
	if ((start + blkcnt) > mmc->block_dev.lba)
	{
#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)
		//printf("MMC: block number 0x" LBAF " exceeds max(0x" LBAF ")\n",start + blkcnt, mmc->block_dev.lba);
#endif
		return 0;
	}
*/

	if (mmc_set_blocklen(mmc, mmc->read_bl_len))
	{
		return 0;
	}

	do {
		cur = (blocks_todo > mmc->cfg->b_max) ?
			mmc->cfg->b_max : blocks_todo;
		if(mmc_read_blocks(mmc, dst, start, cur) != cur)
		{
			return 0;
		}
		blocks_todo -= cur;
		start += cur;
		dst += cur * mmc->read_bl_len;
	} while (blocks_todo > 0);

	return blkcnt;
}




static int mmc_go_idle(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int err;

#ifdef DEBUG
	printf("+mmc.c/mmc_go_idle+\n");
	ConsoleUtilsPrintf("+mmc.c/mmc_go_idle+\n");
#endif





	syl_Sysdelay(5);
    //udelay(1000);
    //Sysdelay(1);

	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_NONE;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
	{
		return err;
	}

	syl_Sysdelay(5);

	//printf("udelay_2000\n\r");

	//udelay(2000);
	//Sysdelay(2);
	return 0;
}

static int sd_send_op_cond(struct mmc *mmc)
{
	int timeout = 1000;
	int err;
	struct mmc_cmd cmd;


#ifdef DEBUG
    printf("+mmc.c/sd_send_op_cond+\n");
    ConsoleUtilsPrintf("+mmc.c/sd_send_op_cond+\n");
#endif


	do {
		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;

		/*
		 * Most cards do not answer if some reserved bits
		 * in the ocr are set. However, Some controller
		 * can set bit 7 (reserved for low voltages), but
		 * how to manage low voltages SD card is not yet
		 * specified.
		 */


		//cmd.cmdarg = mmc_host_is_spi(mmc) ? 0 :(mmc->cfg->voltages & 0xff8000);
		 cmd.cmdarg   = 0 ? 0 :(mmc->cfg->voltages & 0xff8000);

		if (mmc->version == SD_VERSION_2)
			cmd.cmdarg |= OCR_HCS;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;


		syl_Sysdelay(5);
		//printf("udelay_1000\n\r");

		//udelay(1000);
		//  Sysdelay(1);
	} while ((!(cmd.response[0] & OCR_BUSY)) && timeout--);

	if (timeout <= 0)
		return UNUSABLE_ERR;

	if (mmc->version != SD_VERSION_2)
		mmc->version = SD_VERSION_1_0;


	//mmc_host_is_spi(mmc)=0;

	/*
	if (mmc_host_is_spi(mmc))
	{ // read OCR for spi
		cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;
	}*/

	mmc->ocr = cmd.response[0];

	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 0;

	return 0;
}

/* We pass in the cmd since otherwise the init seems to fail */
static int mmc_send_op_cond_iter(struct mmc *mmc, struct mmc_cmd *cmd,int use_arg)
{
	int err;

	cmd->cmdidx = MMC_CMD_SEND_OP_COND;
	cmd->resp_type = MMC_RSP_R3;
	cmd->cmdarg = 0;


#ifdef DEBUG
	printf("+mmc.c/<<<MMC_send_op_cond_iter=mmc_host_is_spi(mmc)_0>>\n");
	ConsoleUtilsPrintf("+mmc.c/<<<MMC_send_op_cond_iter=mmc_host_is_spi(mmc)_0>>\n");
#endif



	//if (use_arg && !mmc_host_is_spi(mmc))  //mmc_host_is_spi(mmc)=0

	if (use_arg && !0)
	{

		printf("+mmc.c/<<<MMC_send_op_cond_iter|if =1>>>\n");

#ifdef DEBUG_ERROR_UART_CONSOLE
		ConsoleUtilsPrintf("+mmc.c/<<<MMC_send_op_cond_iter|if =1>>>\n");
#endif

		cmd->cmdarg =
			(mmc->cfg->voltages &
			(mmc->op_cond_response & OCR_VOLTAGE_MASK)) |
			(mmc->op_cond_response & OCR_ACCESS_MODE);

		if (mmc->cfg->host_caps & MMC_MODE_HC)
			cmd->cmdarg |= OCR_HCS;
	}

	err = mmc_send_cmd(mmc, cmd, NULL);
	if (err)
		return err;
	mmc->op_cond_response = cmd->response[0];
	return 0;
}

int mmc_send_op_cond(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int err, i;


#ifdef DEBUG
	printf("+mmc.c/<<<MMC_send_op_cond>>>\n");
	ConsoleUtilsPrintf("+mmc.c/<<<MMC_send_op_cond>>>\n");
#endif


	/* Some cards seem to need this */
	mmc_go_idle(mmc);

 	/* Asking to the card its capabilities */
	mmc->op_cond_pending = 1;
	for (i = 0; i < 2; i++)
	{
		err = mmc_send_op_cond_iter(mmc, &cmd, i != 0);
		if (err)
			return err;

		/* exit if not busy (flag seems to be inverted) */
		if (mmc->op_cond_response & OCR_BUSY)
			return 0;
	}
	return IN_PROGRESS;
}

int mmc_complete_op_cond(struct mmc *mmc)
{
	struct mmc_cmd cmd;
//	int timeout = 1000;
//	unsigned int start;
	int err;




#ifdef DEBUG
	printf("+mmc.c/<<<MMC_complete_op_cond>>\n");
	ConsoleUtilsPrintf("+mmc.c/<<<MMC_complete_op_cond>>\n");
#endif


   //skd_patch_timings_618

	mmc->op_cond_pending = 0;



#ifdef  DEBUG_TIMING_PATCH
	start = get_timer(0);
#endif

	do {
		err = mmc_send_op_cond_iter(mmc, &cmd, 1);
		if (err)
		{
			return err;
		}

#ifdef  DEBUG_TIMING_PATCH
		if (get_timer(start) > timeout)
			return UNUSABLE_ERR;
#endif

		syl_Sysdelay(5);
		//printf("udelay_100\n\r");
		//udelay(100);
	} while (!(mmc->op_cond_response & OCR_BUSY));


	/*
	if (mmc_host_is_spi(mmc))   //mmc_host_is_spi(mmc)=0
	{ // read OCR for spi
		cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;
	}*/

	mmc->version = MMC_VERSION_UNKNOWN;
	mmc->ocr = cmd.response[0];

	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 1;

	return 0;
}


static int mmc_send_ext_csd(struct mmc *mmc, unsigned char *ext_csd)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;

#ifdef DEBUG
    printf("+mmc.c/mmc_send_ext_csd+\n");
    ConsoleUtilsPrintf("+mmc.c/mmc_send_ext_csd+\n");
#endif




    /* Get the Card Status Register */
	cmd.cmdidx = MMC_CMD_SEND_EXT_CSD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;

	data.dest = (char *)ext_csd;
	data.blocks = 1;
	data.blocksize = MMC_MAX_BLOCK_LEN;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	return err;
}


static int mmc_switch(struct mmc *mmc, unsigned char set, unsigned char index, unsigned char value)
{
	struct mmc_cmd cmd;
	int timeout = 1000;
	int ret;

#ifdef DEBUG
    printf("+mmc.c/mmc_switch+\n");
    ConsoleUtilsPrintf("+mmc.c/mmc_switch+\n");
#endif


    cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
				 (index << 16) |
				 (value << 8);

	ret = mmc_send_cmd(mmc, &cmd, NULL);

	/* Waiting for the ready status */
	if (!ret)
		ret = mmc_send_status(mmc, timeout);

	return ret;

}

static int mmc_change_freq(struct mmc *mmc)
{
	//ALLOC_CACHE_ALIGN_BUFFER(u8, ext_csd, MMC_MAX_BLOCK_LEN);
    unsigned char *ext_csd;
	ext_csd=(unsigned char*) malloc (MMC_MAX_BLOCK_LEN);

	char cardtype;
	int err;

	mmc->card_caps = 0;
    printf("+mmc.c/mmc_change_freq+\n");

    /*
    if (mmc_host_is_spi(mmc))  // mmc_host_is_spi(mmc)=0
		return 0;
    */

	/* Only version 4 supports high-speed */
	if (mmc->version < MMC_VERSION_4)
		return 0;

	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;

	cardtype = ext_csd[EXT_CSD_CARD_TYPE] & 0xf;

	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 1);

	if (err)
		return err == SWITCH_ERR ? 0 : err;

	/* Now check to see that it worked */
	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;

	/* No high-speed support */
	if (!ext_csd[EXT_CSD_HS_TIMING])
		return 0;

	/* High Speed is set, there are two types: 52MHz and 26MHz */
	if (cardtype & EXT_CSD_CARD_TYPE_52)
	{
		if (cardtype & EXT_CSD_CARD_TYPE_DDR_52)
			mmc->card_caps |= MMC_MODE_DDR_52MHz;
		mmc->card_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;
	}
	else
	{
		mmc->card_caps |= MMC_MODE_HS;
	}

	return 0;
}

static int mmc_set_capacity(struct mmc *mmc, int part_num)
{

	//lldiv_t exit;

	switch (part_num)
	{
	case 0:
		mmc->capacity = mmc->capacity_user;
		break;
	case 1:
	case 2:
		mmc->capacity = mmc->capacity_boot;
		break;
	case 3:
		mmc->capacity = mmc->capacity_rpmb;
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		mmc->capacity = mmc->capacity_gp[part_num - 4];
		break;
	default:
		return -1;
	}

	//mmc->block_dev.lba_c


	mmc->block_dev.lba_c=lldiv(mmc->capacity, mmc->read_bl_len);;
	//mmc->block_dev.lba

	//mmc->block_dev.lba =(lbaint_t)exit;

	//mmc->block_dev.lba = lldiv(mmc->capacity, mmc->read_bl_len);


	return 0;
}

int mmc_select_hwpart(int dev_num, int hwpart)
{
	struct mmc *mmc = find_mmc_device(dev_num);
	int ret;

	if (!mmc)
		return -ENODEV;

	if (mmc->part_num == hwpart)
		return 0;

	if (mmc->part_config == MMCPART_NOAVAILABLE) {
		printf("Card doesn't support part_switch\n");
		return -EMEDIUMTYPE;
	}

	ret = mmc_switch_part(dev_num, hwpart);
	if (ret)
		return ret;

	mmc->part_num = hwpart;

	return 0;
}


int mmc_switch_part(int dev_num, unsigned int part_num)
{
	struct mmc *mmc = find_mmc_device(dev_num);
	int ret;

	if (!mmc)
		return -1;

	ret = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_PART_CONF,
			 (mmc->part_config & ~PART_ACCESS_MASK)
			 | (part_num & PART_ACCESS_MASK));

	/*
	 * Set the capacity if the switch succeeded or was intended
	 * to return to representing the raw device.
	 */


	if ((ret == 0) || ((ret == -ENODEV) && (part_num == 0)))
		ret = mmc_set_capacity(mmc, part_num);

	return ret;
}

/*
int mmc_getcd(struct mmc *mmc)
{
	int cd;

	cd = board_mmc_getcd(mmc);
    printf("+++++!!!mmc.c/mmc_getcd_cd=%d+++++++++++!!\r\n",cd);
	if (cd < 0) 
	{
		if (mmc->cfg->ops->getcd)
			cd = mmc->cfg->ops->getcd(mmc);
		else
			cd = 1;
	}

	return cd;
}
*/

static int sd_switch(struct mmc *mmc, int mode, int group, unsigned char value, unsigned char *resp)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Switch the frequency */
	cmd.cmdidx = SD_CMD_SWITCH_FUNC;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = (mode << 31) | 0xffffff;
	cmd.cmdarg &= ~(0xf << (group * 4));
	cmd.cmdarg |= value << (group * 4);

	data.dest = (char *)resp;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	return mmc_send_cmd(mmc, &cmd, &data);
}


static int sd_change_freq(struct mmc *mmc)
{
	int err;
	struct mmc_cmd cmd;
	//ALLOC_CACHE_ALIGN_BUFFER(uint, scr, 2);
	//ALLOC_CACHE_ALIGN_BUFFER(uint, switch_status, 16);

	unsigned int scr[2];
	unsigned int switch_status[16];



	//scr=(uint) malloc (2);
	//switch_status=(uint) malloc (16);



	struct mmc_data data;
	int timeout;

	mmc->card_caps = 0;

	//mmc_host_is_spi(mmc))=0

	/*
	if (mmc_host_is_spi(mmc))
		return 0;
    */

	/* Read the SCR to find out if this card supports higher speeds */
	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	cmd.cmdidx = SD_CMD_APP_SEND_SCR;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;

	timeout = 3;

retry_scr:
	data.dest = (char *)scr;
	data.blocksize = 8;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	if (err)
	{
		if (timeout--)
			goto retry_scr;

		return err;
	}

	  mmc->scr[0]  = 0x22500000;
      mmc->scr[1]  = 0x00000000;


    //TODO DODELAT this fignu
	//mmc->scr[0] = __be32_to_cpu(scr[0]);
	//mmc->scr[1] = __be32_to_cpu(scr[1]);

	switch ((mmc->scr[0] >> 24) & 0xf) {
		case 0:
			mmc->version = SD_VERSION_1_0;
			break;
		case 1:
			mmc->version = SD_VERSION_1_10;
			break;
		case 2:
			mmc->version = SD_VERSION_2;
			if ((mmc->scr[0] >> 15) & 0x1)
				mmc->version = SD_VERSION_3;
			break;
		default:
			mmc->version = SD_VERSION_1_0;
			break;
	}

	if (mmc->scr[0] & SD_DATA_4BIT)
		mmc->card_caps |= MMC_MODE_4BIT;

	/* Version 1.0 doesn't support switching */
	if (mmc->version == SD_VERSION_1_0)
		return 0;

	timeout = 4;


	while (timeout--)
	{
		err = sd_switch(mmc, SD_SWITCH_CHECK, 0, 1,(unsigned char *)switch_status);

		if (err)
			return err;


		switch_status[7]= 0x00000000;
		if (!switch_status[7] & SD_HIGHSPEED_BUSY)
					break;

		//TODO
		// The high-speed function is busy.  Try again
		//if (!(__be32_to_cpu(switch_status[7]) & SD_HIGHSPEED_BUSY))
		//	break;
	}

	switch_status[3]=0x80030000;
	if (!switch_status[3] & SD_HIGHSPEED_SUPPORTED)
	return 0;
	// If high-speed isn't supported, we return
	//if (!(__be32_to_cpu(switch_status[3]) & SD_HIGHSPEED_SUPPORTED))

	//return 0;



	/*
	 * If the host doesn't support SD_HIGHSPEED, do not switch card to
	 * HIGHSPEED mode even if the card support SD_HIGHSPPED.
	 * This can avoid furthur problem when the card runs in different
	 * mode between the host.
	 */
	if (!((mmc->cfg->host_caps & MMC_MODE_HS_52MHz) &&
		(mmc->cfg->host_caps & MMC_MODE_HS)))
		return 0;

	err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (unsigned char *)switch_status);

	if (err)
		return err;



	switch_status[4]=0x10000000;
	if (switch_status[4] & 0x0f000000 == 0x01000000)
			mmc->card_caps |= MMC_MODE_HS;

	//TODO

	/*
	if ((__be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000)
		mmc->card_caps |= MMC_MODE_HS;*/

	return 0;
}

/* frequency bases */
/* divided by 10 to be nice to platforms without floating point */
static const int fbase[] = {
	10000,
	100000,
	1000000,
	10000000,
};

/* Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
 * to platforms without floating point.
 */
static const int multipliers[] = {
	0,	/* reserved */
	10,
	12,
	13,
	15,
	20,
	25,
	30,
	35,
	40,
	45,
	50,
	55,
	60,
	70,
	80,
};

static void mmc_set_ios(struct mmc *mmc)
{
	if (mmc->cfg->ops->set_ios)
		mmc->cfg->ops->set_ios(mmc);
}

void mmc_set_clock(struct mmc *mmc, unsigned int clock)
{
	if (clock > mmc->cfg->f_max)
		clock = mmc->cfg->f_max;

	if (clock < mmc->cfg->f_min)
		clock = mmc->cfg->f_min;

	mmc->clock = clock;

	mmc_set_ios(mmc);
}

static void mmc_set_bus_width(struct mmc *mmc, unsigned int width)
{
	mmc->bus_width = width;

	mmc_set_ios(mmc);
}




static int mmc_startup(struct mmc *mmc)
{
	int err, i;
	unsigned int mult, freq;
	unsigned long long cmult, csize, capacity;
	struct mmc_cmd cmd;

	unsigned char *ext_csd;
	unsigned char *test_csd;
	ext_csd=(unsigned char*) malloc (MMC_MAX_BLOCK_LEN);
	test_csd=(unsigned char*) malloc (MMC_MAX_BLOCK_LEN);

	//ALLOC_CACHE_ALIGN_BUFFER(u8, ext_csd, MMC_MAX_BLOCK_LEN);
	//ALLOC_CACHE_ALIGN_BUFFER(u8, test_csd, MMC_MAX_BLOCK_LEN);



	int timeout = 1000;




#ifdef DEBUG
    printf("!_mmc.c/mmc_startup_822!\n");
    ConsoleUtilsPrintf("!_mmc.c/mmc_startup_822!\n");
#endif




#ifdef CONFIG_MMC_SPI_CRC_ON
	if (mmc_host_is_spi(mmc)) { /* enable CRC check for spi */
		cmd.cmdidx = MMC_CMD_SPI_CRC_ON_OFF;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 1;
		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;
	}
#endif

	/* Put the Card in Identify Mode */


	//cmd.cmdidx = mmc_host_is_spi(mmc) ? MMC_CMD_SEND_CID :MMC_CMD_ALL_SEND_CID; /* cmd not supported in spi */
	cmd.cmdidx =0  ? MMC_CMD_SEND_CID :MMC_CMD_ALL_SEND_CID; /* cmd not supported in spi */
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	memcpy(mmc->cid, cmd.response, 16);

	/*
	 * For MMC cards, set the Relative Address.
	 * For SD cards, get the Relatvie Address.
	 * This also puts the cards into Standby State
	 */


	//if (!mmc_host_is_spi(mmc))                  //mmc_host_is_spi(mmc) =0
	if (!0)
	{ // cmd not supported in spi
		cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
		cmd.cmdarg = mmc->rca << 16;
		cmd.resp_type = MMC_RSP_R6;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		if (IS_SD(mmc))
			mmc->rca = (cmd.response[0] >> 16) & 0xffff;
	}

	/* Get the Card-Specific Data */
	cmd.cmdidx = MMC_CMD_SEND_CSD;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = mmc->rca << 16;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	/* Waiting for the ready status */
	mmc_send_status(mmc, timeout);

	if (err)
		return err;

	mmc->csd[0] = cmd.response[0];
	mmc->csd[1] = cmd.response[1];
	mmc->csd[2] = cmd.response[2];
	mmc->csd[3] = cmd.response[3];

	if (mmc->version == MMC_VERSION_UNKNOWN) {
		int version = (cmd.response[0] >> 26) & 0xf;

		switch (version) {
			case 0:
				mmc->version = MMC_VERSION_1_2;
				break;
			case 1:
				mmc->version = MMC_VERSION_1_4;
				break;
			case 2:
				mmc->version = MMC_VERSION_2_2;
				break;
			case 3:
				mmc->version = MMC_VERSION_3;
				break;
			case 4:
				mmc->version = MMC_VERSION_4;
				break;
			default:
				mmc->version = MMC_VERSION_1_2;
				break;
		}
	}

	/* divide frequency by 10, since the mults are 10x bigger */
	freq = fbase[(cmd.response[0] & 0x7)];
	mult = multipliers[((cmd.response[0] >> 3) & 0xf)];

	mmc->tran_speed = freq * mult;

	mmc->dsr_imp = ((cmd.response[1] >> 12) & 0x1);
	mmc->read_bl_len = 1 << ((cmd.response[1] >> 16) & 0xf);

	if (IS_SD(mmc))
		mmc->write_bl_len = mmc->read_bl_len;
	else
		mmc->write_bl_len = 1 << ((cmd.response[3] >> 22) & 0xf);

	if (mmc->high_capacity) {
		csize = (mmc->csd[1] & 0x3f) << 16
			| (mmc->csd[2] & 0xffff0000) >> 16;
		cmult = 8;
	} else {
		csize = (mmc->csd[1] & 0x3ff) << 2
			| (mmc->csd[2] & 0xc0000000) >> 30;
		cmult = (mmc->csd[2] & 0x00038000) >> 15;
	}

	mmc->capacity_user = (csize + 1) << (cmult + 2);
	mmc->capacity_user *= mmc->read_bl_len;
	mmc->capacity_boot = 0;
	mmc->capacity_rpmb = 0;
	for (i = 0; i < 4; i++)
		mmc->capacity_gp[i] = 0;

	if (mmc->read_bl_len > MMC_MAX_BLOCK_LEN)
		mmc->read_bl_len = MMC_MAX_BLOCK_LEN;

	if (mmc->write_bl_len > MMC_MAX_BLOCK_LEN)
		mmc->write_bl_len = MMC_MAX_BLOCK_LEN;

	if ((mmc->dsr_imp) && (0xffffffff != mmc->dsr)) {
		cmd.cmdidx = MMC_CMD_SET_DSR;
		cmd.cmdarg = (mmc->dsr & 0xffff) << 16;
		cmd.resp_type = MMC_RSP_NONE;
		if (mmc_send_cmd(mmc, &cmd, NULL))
			printf("MMC: SET_DSR failed\n");
	}

	/* Select the card, and put it into Transfer Mode */
	//if (!mmc_host_is_spi(mmc))
	if (!0)
	{ // cmd not supported in spi
		cmd.cmdidx = MMC_CMD_SELECT_CARD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = mmc->rca << 16;
		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;
	}

	/*
	 * For SD, its erase group is always one sector
	 */
	mmc->erase_grp_size = 1;
	mmc->part_config = MMCPART_NOAVAILABLE;
	if (!IS_SD(mmc) && (mmc->version >= MMC_VERSION_4))
	{
		/* check  ext_csd version and capacity */


		err = mmc_send_ext_csd(mmc, ext_csd);
		if (!err && (ext_csd[EXT_CSD_REV] >= 2))
		{

			// According to the JEDEC Standard, the value of
			// ext_csd's capacity is valid if the value is more
			// than 2GB

			capacity = ext_csd[EXT_CSD_SEC_CNT] << 0
					| ext_csd[EXT_CSD_SEC_CNT + 1] << 8
					| ext_csd[EXT_CSD_SEC_CNT + 2] << 16
					| ext_csd[EXT_CSD_SEC_CNT + 3] << 24;
			capacity *= MMC_MAX_BLOCK_LEN;
			if ((capacity >> 20) > 2 * 1024)
				mmc->capacity_user = capacity;
		}


		switch (ext_csd[EXT_CSD_REV])
		{
		case 1:
			mmc->version = MMC_VERSION_4_1;
			break;
		case 2:
			mmc->version = MMC_VERSION_4_2;
			break;
		case 3:
			mmc->version = MMC_VERSION_4_3;
			break;
		case 5:
			mmc->version = MMC_VERSION_4_41;
			break;
		case 6:
			mmc->version = MMC_VERSION_4_5;
			break;
		}

		/*
		 * Host needs to enable ERASE_GRP_DEF bit if device is
		 * partitioned. This bit will be lost every time after a reset
		 * or power off. This will affect erase size.
		 */


		if ((ext_csd[EXT_CSD_PARTITIONING_SUPPORT] & PART_SUPPORT) && (ext_csd[EXT_CSD_PARTITIONS_ATTRIBUTE] & PART_ENH_ATTRIB))
		{
			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,EXT_CSD_ERASE_GROUP_DEF, 1);

			if (err)
				return err;

			// Read out group size from ext_csd
			mmc->erase_grp_size =ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] *MMC_MAX_BLOCK_LEN * 1024;
		} else
		{
			// Calculate the group size from the csd value.
			int erase_gsz, erase_gmul;
			erase_gsz = (mmc->csd[2] & 0x00007c00) >> 10;
			erase_gmul = (mmc->csd[2] & 0x000003e0) >> 5;
			mmc->erase_grp_size = (erase_gsz + 1)
				* (erase_gmul + 1);
		}

		/* store the partition info of emmc */
		if ((ext_csd[EXT_CSD_PARTITIONING_SUPPORT] & PART_SUPPORT) ||  ext_csd[EXT_CSD_BOOT_MULT])
			mmc->part_config = ext_csd[EXT_CSD_PART_CONF];

		mmc->capacity_boot = ext_csd[EXT_CSD_BOOT_MULT] << 17;

		mmc->capacity_rpmb = ext_csd[EXT_CSD_RPMB_MULT] << 17;

		for (i = 0; i < 4; i++)
		{
			int idx = EXT_CSD_GP_SIZE_MULT + i * 3;
			mmc->capacity_gp[i] = (ext_csd[idx + 2] << 16) +
				(ext_csd[idx + 1] << 8) + ext_csd[idx];
			mmc->capacity_gp[i] *=
				ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE];
			mmc->capacity_gp[i] *= ext_csd[EXT_CSD_HC_WP_GRP_SIZE];
		}
	}

	err = mmc_set_capacity(mmc, mmc->part_num);
	if (err)
		return err;

	if (IS_SD(mmc))
		err = sd_change_freq(mmc);
	else
		err = mmc_change_freq(mmc);

	if (err)
		return err;

	/* Restrict card's capabilities by what the host can do */
	mmc->card_caps &= mmc->cfg->host_caps;

	if (IS_SD(mmc))
	{
		if (mmc->card_caps & MMC_MODE_4BIT)
		{
			cmd.cmdidx = MMC_CMD_APP_CMD;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = mmc->rca << 16;

			err = mmc_send_cmd(mmc, &cmd, NULL);
			if (err)
			{
				return err;
			}

			cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = 2;
			err = mmc_send_cmd(mmc, &cmd, NULL);
			if (err)
				return err;

			mmc_set_bus_width(mmc, 4);
		}

		if (mmc->card_caps & MMC_MODE_HS)
			mmc->tran_speed = 50000000;
		else
			mmc->tran_speed = 25000000;
	} else {
		int idx;

		/* An array of possible bus widths in order of preference */
		static unsigned ext_csd_bits[] = {
			EXT_CSD_DDR_BUS_WIDTH_8,
			EXT_CSD_DDR_BUS_WIDTH_4,
			EXT_CSD_BUS_WIDTH_8,
			EXT_CSD_BUS_WIDTH_4,
			EXT_CSD_BUS_WIDTH_1,
		};

		/* An array to map CSD bus widths to host cap bits */
		static unsigned ext_to_hostcaps[] = {
			[EXT_CSD_DDR_BUS_WIDTH_4] = MMC_MODE_DDR_52MHz,
			[EXT_CSD_DDR_BUS_WIDTH_8] = MMC_MODE_DDR_52MHz,
			[EXT_CSD_BUS_WIDTH_4] = MMC_MODE_4BIT,
			[EXT_CSD_BUS_WIDTH_8] = MMC_MODE_8BIT,
		};

		/* An array to map chosen bus width to an integer */
		static unsigned widths[] = {
			8, 4, 8, 4, 1,
		};

		for (idx=0; idx < 0x5/*ARRAY_SIZE(ext_csd_bits)*/; idx++)
		{
			unsigned int extw = ext_csd_bits[idx];

			/*
			 * Check to make sure the controller supports
			 * this bus width, if it's more than 1
			 */
			if (extw != EXT_CSD_BUS_WIDTH_1 &&
					!(mmc->cfg->host_caps & ext_to_hostcaps[extw]))
				continue;

			err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH, extw);

			if (err)
				continue;

			mmc_set_bus_width(mmc, widths[idx]);


			err = mmc_send_ext_csd(mmc, test_csd);
			if (!err && ext_csd[EXT_CSD_PARTITIONING_SUPPORT] \
				    == test_csd[EXT_CSD_PARTITIONING_SUPPORT]
				 && ext_csd[EXT_CSD_ERASE_GROUP_DEF] \
				    == test_csd[EXT_CSD_ERASE_GROUP_DEF] \
				 && ext_csd[EXT_CSD_REV] \
				    == test_csd[EXT_CSD_REV]
				 && ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] \
				    == test_csd[EXT_CSD_HC_ERASE_GRP_SIZE]
				 && memcmp(&ext_csd[EXT_CSD_SEC_CNT], \
					&test_csd[EXT_CSD_SEC_CNT], 4) == 0)
			{

				mmc->card_caps |= ext_to_hostcaps[extw];
				break;
			}

		}

		if (mmc->card_caps & MMC_MODE_HS) {
			if (mmc->card_caps & MMC_MODE_HS_52MHz)
				mmc->tran_speed = 52000000;
			else
				mmc->tran_speed = 26000000;
		}
	}

	mmc_set_clock(mmc, mmc->tran_speed);
	mmc->block_dev.lun = 0;
	mmc->block_dev.type = 0;
	mmc->block_dev.blksz = mmc->read_bl_len;
	mmc->block_dev.log2blksz =LOG2(mmc->block_dev.blksz);


	//
	//mmc->block_dev.lba = lldiv(mmc->capacity, mmc->read_bl_len);



	#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)
	sprintf(mmc->block_dev.vendor, "Man %06x Snr %04x%04x",
		mmc->cid[0] >> 24, (mmc->cid[2] & 0xffff),
		(mmc->cid[3] >> 16) & 0xffff);
	sprintf(mmc->block_dev.product, "%c%c%c%c%c%c", mmc->cid[0] & 0xff,
		(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
		(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff,
		(mmc->cid[2] >> 24) & 0xff);
	sprintf(mmc->block_dev.revision, "%d.%d", (mmc->cid[2] >> 20) & 0xf,
		(mmc->cid[2] >> 16) & 0xf);

#else
	mmc->block_dev.vendor[0] = 0;
	mmc->block_dev.product[0] = 0;
	mmc->block_dev.revision[0] = 0;
#endif
#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBDISK_SUPPORT)
	//init_part(&mmc->block_dev);
#endif

	return 0;
}

static int mmc_send_if_cond(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int err;
    printf("+_mmc.c/mmc_send_if_cond+\n");


#ifdef DEBUG_ERROR_UART_CONSOLE
    ConsoleUtilsPrintf("+_mmc.c/mmc_send_if_cond+\n");
#endif

    cmd.cmdidx = SD_CMD_SEND_IF_COND;
	/* We set the bit if the host supports voltages between 2.7 and 3.6 V */
	cmd.cmdarg = ((mmc->cfg->voltages & 0xff8000) != 0) << 8 | 0xaa;
	cmd.resp_type = MMC_RSP_R7;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	if ((cmd.response[0] & 0xff) != 0xaa)
		return UNUSABLE_ERR;
	else
		mmc->version = SD_VERSION_2;

	return 0;
}

/* not used any more */
int  mmc_register(struct mmc *mmc)
{
#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)
	printf("%s is deprecated! use mmc_create() instead.\n", __func__);
#endif
	return -1;
}

struct mmc *mmc_create(const struct mmc_config *cfg, void *priv)
{
	struct mmc *mmc;


	/* quick validation */
	if (cfg == NULL || cfg->ops == NULL || cfg->ops->send_cmd == NULL ||cfg->f_min == 0 || cfg->f_max == 0 || cfg->b_max == 0)
		return NULL;




	mmc = calloc(1, sizeof(*mmc));
	if (mmc == NULL)
		return NULL;

	mmc->cfg = cfg;
	mmc->priv = priv;

	/* the following chunk was mmc_register() */

	/* Setup dsr related values */
	mmc->dsr_imp = 0;
	mmc->dsr = 0xffffffff;
	/* Setup the universal parts of the block interface just once */


	mmc->block_dev.if_type = IF_TYPE_MMC;
	mmc->block_dev.dev =     cur_dev_num;
	mmc->block_dev.removable = 1;
	mmc->block_dev.block_read =  mmc_bread;
	mmc->block_dev.block_write = mmc_bwrite;    //This function ispolzuem go fail mmc_write.c
	mmc->block_dev.block_erase = mmc_berase;    //This function ispolzuem go fail mmc_write.c


	printf("+_mmc.c/mmc_create_cur_dev_num=%d\n",cur_dev_num);
#ifdef DEBUG_ERROR_UART_CONSOLE
	ConsoleUtilsPrintf("+_mmc.c/mmc_create_cur_dev_num=%d\n",cur_dev_num);
#endif

	// setup initial part type
	mmc->block_dev.part_type = mmc->cfg->part_type;
	cur_dev_num++;

	INIT_LIST_HEAD(&mmc->link);
	list_add_tail(&mmc->link, &mmc_devices);

	return mmc;

}

void mmc_destroy(struct mmc *mmc)
{
	/* only freeing memory for now */
	free(mmc);
}

#ifdef CONFIG_PARTITIONS
block_dev_desc_t *mmc_get_dev(int dev)
{
	struct mmc *mmc = find_mmc_device(dev);
	if (!mmc || mmc_init(mmc))
		return NULL;

	return &mmc->block_dev;
}
#endif

int mmc_start_init(struct mmc *mmc)
{
	int err;

	// we pretend there's no card when init is NULL

#if 0  //kosta_skd_patch
	if (mmc_getcd(mmc) == 0 || mmc->cfg->ops->init == NULL)
	{
		mmc->has_init = 0;
#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)
		printf("MMC: no card present\n");
#endif
		return NO_CARD_ERR;
	}
#endif
     
	 if (mmc->cfg->ops->init == NULL)
     {
	 
	    printf("MMC: no card present\n");
	    return NO_CARD_ERR;
	 }









#ifdef DEBUG
	printf("!+_mmc.c/mmc_start_init+!_1290\n");
    ConsoleUtilsPrintf("!+_mmc.c/mmc_start_init+!_1290\n");
#endif



     if (mmc->has_init)
	{
		return 0;
	}
	/* made sure it's not NULL earlier */
	err = mmc->cfg->ops->init(mmc);

	if (err)
		return err;

	mmc_set_bus_width(mmc, 1);
	mmc_set_clock(mmc, 1);

	/* Reset the Card */
	err = mmc_go_idle(mmc);

	if (err)
		return err;

	/* The internal partition reset to user partition(0) at every CMD0*/
	mmc->part_num = 0;

	/* Test for SD version 2 */
	err = mmc_send_if_cond(mmc);

	/* Now try to get the SD card's operating condition */
	err = sd_send_op_cond(mmc);

	/* If the command timed out, we check for an MMC card */
	if (err == TIMEOUT) {
		err = mmc_send_op_cond(mmc);

		if (err && err != IN_PROGRESS) {
#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)
			printf("Card did not respond to voltage select!\n");


#ifdef DEBUG_ERROR_UART_CONSOLE
			ConsoleUtilsPrintf("Card did not respond to voltage select!\n");
#endif

#endif
			return UNUSABLE_ERR;
		}
	}

	if (err == IN_PROGRESS)
		mmc->init_in_progress = 1;

	return err;
}

static int mmc_complete_init(struct mmc *mmc)
{
	int err = 0;
     printf("!+_mmc.c/mmc_complete_init\n");

#ifdef DEBUG_ERROR_UART_CONSOLE
     ConsoleUtilsPrintf("!+_mmc.c/mmc_complete_init\n");
#endif

     if (mmc->op_cond_pending)
		err = mmc_complete_op_cond(mmc);

	if (!err)
		err = mmc_startup(mmc);
	if (err)
		mmc->has_init = 0;
	else
		mmc->has_init = 1;
	mmc->init_in_progress = 0;
	return err;
}

int mmc_init(struct mmc *mmc)
{
	int err = IN_PROGRESS;
	//unsigned start;
#ifdef DEBUG
    printf("+mmc.c_1357/Mmc_init+\n");
    ConsoleUtilsPrintf("+mmc.c_1357/Mmc_init+\n");
#endif

    if (mmc->has_init)
    {
    	return 0;
    }


	//start = get_timer(0);

	if (!mmc->init_in_progress)
		err = mmc_start_init(mmc);

	if (!err || err == IN_PROGRESS)
		err = mmc_complete_init(mmc);
	//debug("%s: %d, time %lu\n", __func__, err, get_timer(start));
	//printf("%s: %d, time %lu\n", __func__, err, get_timer(start));
	return err;
}

int mmc_set_dsr(struct mmc *mmc, unsigned short val)
{
	mmc->dsr = val;
	return 0;
}



#if !defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_LIBCOMMON_SUPPORT)

void print_mmc_devices(char separator)
{
	struct mmc *m;
	struct list_head *entry;


	list_for_each(entry, &mmc_devices)
	{

		m = list_entry(entry, struct mmc, link);

		printf("%s: %d", m->cfg->name, m->block_dev.dev);

#ifdef DEBUG_ERROR_UART_CONSOLE
		ConsoleUtilsPrintf("%s: %d", m->cfg->name, m->block_dev.dev);
#endif

		if (entry->next != &mmc_devices)
			printf("%c ", separator);
#ifdef DEBUG_ERROR_UART_CONSOLE
		ConsoleUtilsPrintf("%c ", separator);
#endif
	}


#ifdef DEBUG_ERROR_UART_CONSOLE
	ConsoleUtilsPrintf("\n");
#endif
	printf("\n");
}

#else
void print_mmc_devices(char separator) { }
#endif

int get_mmc_num(void)
{
	return cur_dev_num;
}

void mmc_set_preinit(struct mmc *mmc, int preinit)
{
	mmc->preinit = preinit;
}

static void do_preinit(void)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices)
	{
		m = list_entry(entry, struct mmc, link);

		if (m->preinit)
			mmc_start_init(m);
	}


}


int mmc_initialize(unsigned int dev)

//int mmc_initialize(struct bd_t *bis)
{
	INIT_LIST_HEAD (&mmc_devices);


	//cur_dev_num = 0;
    printf("_______mmc.c/mmc_initialize______1443_dev=%d\n",dev);


#ifdef DEBUG_ERROR_UART_CONSOLE
    ConsoleUtilsPrintf("_______mmc.c/mmc_initialize______1443_dev=%d\n",dev);
#endif

    omap_mmc_init(dev, 0, 0, -1, -1);

   //#ifndef CONFIG_SPL_BUILD
	print_mmc_devices(',');
   //#endif

	do_preinit();
	return 0;
}




//#ifdef CONFIG_SUPPORT_EMMC_BOOT
/*
 * This function changes the size of boot partition and the size of rpmb
 * partition present on EMMC devices.
 *
 * Input Parameters:
 * struct *mmc: pointer for the mmc device strcuture
 * bootsize: size of boot partition
 * rpmbsize: size of rpmb partition
 *
 * Returns 0 on success.
 */

int mmc_boot_partition_size_change(struct mmc *mmc, unsigned long bootsize,unsigned long rpmbsize)
{
	int err;
	struct mmc_cmd cmd;
    printf("!+_mmc.c/mmc_boot_partition_size_change!!\n");
	/* Only use this command for raw EMMC moviNAND. Enter backdoor mode */
	cmd.cmdidx = MMC_CMD_RES_MAN;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = MMC_CMD62_ARG1;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err) {
		//debug("mmc_boot_partition_size_change: Error1 = %d\n", err);
		return err;
	}

	/* Boot partition changing mode */
	cmd.cmdidx = MMC_CMD_RES_MAN;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = MMC_CMD62_ARG2;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err) {
		//debug("mmc_boot_partition_size_change: Error2 = %d\n", err);
		return err;
	}
	/* boot partition size is multiple of 128KB */
	bootsize = (bootsize * 1024) / 128;

	/* Arg: boot partition size */
	cmd.cmdidx = MMC_CMD_RES_MAN;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = bootsize;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err) {
		//debug("mmc_boot_partition_size_change: Error3 = %d\n", err);
		return err;
	}
	/* RPMB partition size is multiple of 128KB */
	rpmbsize = (rpmbsize * 1024) / 128;
	/* Arg: RPMB partition size */
	cmd.cmdidx = MMC_CMD_RES_MAN;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = rpmbsize;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err) {
		//debug("mmc_boot_partition_size_change: Error4 = %d\n", err);
		return err;
	}
	return 0;
}

/*
 * Modify EXT_CSD[177] which is BOOT_BUS_WIDTH
 * based on the passed in values for BOOT_BUS_WIDTH, RESET_BOOT_BUS_WIDTH
 * and BOOT_MODE.
 *
 * Returns 0 on success.
 */
int mmc_set_boot_bus_width(struct mmc *mmc, unsigned char width, unsigned char reset, unsigned char mode)
{
	int err;
    printf("!+_mmc.c/mmc_set_boot_bus_width!!\n");
	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_BOOT_BUS_WIDTH,
			 EXT_CSD_BOOT_BUS_WIDTH_MODE(mode) |
			 EXT_CSD_BOOT_BUS_WIDTH_RESET(reset) |
			 EXT_CSD_BOOT_BUS_WIDTH_WIDTH(width));

	if (err)
	{
		return err;
	}
return 0;
}

/*
 * Modify EXT_CSD[179] which is PARTITION_CONFIG (formerly BOOT_CONFIG)
 * based on the passed in values for BOOT_ACK, BOOT_PARTITION_ENABLE and
 * PARTITION_ACCESS.
 *
 * Returns 0 on success.
 */
int mmc_set_part_conf(struct mmc *mmc, unsigned char ack, unsigned char part_num, unsigned char access)
{
	int err;
    printf("!+_mmc.c/ mmc_set_part_conf!!\n");
	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_PART_CONF,
			 EXT_CSD_BOOT_ACK(ack) |
			 EXT_CSD_BOOT_PART_NUM(part_num) |
			 EXT_CSD_PARTITION_ACCESS(access));

	if (err)
		return err;
	return 0;
}

/*
 * Modify EXT_CSD[162] which is RST_n_FUNCTION based on the given value
 * for enable.  Note that this is a write-once field for non-zero values.
 *
 * Returns 0 on success.
 */
int mmc_set_rst_n_function(struct mmc *mmc, unsigned char enable)
{
	return mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_RST_N_FUNCTION,enable);
}
//#endif



////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD OMAP functions//////////////////////////////////////////////////////////////
unsigned char mmc_board_init(struct mmc *mmc)
{
#if defined(CONFIG_OMAP34XX)
	t2_t *t2_base = (t2_t *)T2_BASE;
	struct prcm *prcm_base = (struct prcm *)PRCM_BASE;
	u32 pbias_lite;
	pbias_lite = readl(&t2_base->pbias_lite);
	pbias_lite &= ~(PBIASLITEPWRDNZ1 | PBIASLITEPWRDNZ0);
	writel(pbias_lite, &t2_base->pbias_lite);
#endif
#if defined(CONFIG_TWL4030_POWER)
	twl4030_power_mmc_init();
	mdelay(100);	/* ramp-up delay from Linux code */
#endif
#if defined(CONFIG_OMAP34XX)
	writel(pbias_lite | PBIASLITEPWRDNZ1 |
		PBIASSPEEDCTRL0 | PBIASLITEPWRDNZ0,
		&t2_base->pbias_lite);

	writel(readl(&t2_base->devconf0) | MMCSDIO1ADPCLKISEL,
		&t2_base->devconf0);

	writel(readl(&t2_base->devconf1) | MMCSDIO2ADPCLKISEL,
		&t2_base->devconf1);

	/* Change from default of 52MHz to 26MHz if necessary */
	if (!(mmc->cfg->host_caps & MMC_MODE_HS_52MHz))
		writel(readl(&t2_base->ctl_prog_io1) & ~CTLPROGIO1SPEEDCTRL,
			&t2_base->ctl_prog_io1);

	writel(readl(&prcm_base->fclken1_core) |
		EN_MMC1 | EN_MMC2 | EN_MMC3,
		&prcm_base->fclken1_core);

	writel(readl(&prcm_base->iclken1_core) |
		EN_MMC1 | EN_MMC2 | EN_MMC3,
		&prcm_base->iclken1_core);
#endif

#if defined(CONFIG_OMAP44XX) && defined(CONFIG_TWL6030_POWER)
	/* PBIAS config needed for MMC1 only */
    printf("!+omap_hsmmc.c/mmc_board_init\n");
	if (mmc->block_dev.dev == 0)
		omap4_vmmc_pbias_config(mmc);
#endif
#if defined(CONFIG_OMAP54XX) && defined(CONFIG_PALMAS_POWER)
	if (mmc->block_dev.dev == 0)
		omap5_pbias_config(mmc);
#endif

	return 0;
}



void mmc_init_stream(struct hsmmc *mmc_base)
{
//	unsigned long long start;

	writel(readl(&mmc_base->con) | INIT_INITSTREAM, &mmc_base->con);

	writel(MMC_CMD0, &mmc_base->cmd);


#ifdef DEBUG
	printf("!+omap_hsmmc.c/mmc_init_stream\n");
	ConsoleUtilsPrintf("!+omap_hsmmc.c/mmc_init_stream\n");
#endif



#ifdef  DEBUG_TIMING_PATCH
   start = get_timer(0);
#endif

	while (!(readl(&mmc_base->stat) & CC_MASK))
	{

		printf("????omap_hsmmc.c/mmc_init_stream_2115_error????\n");
		#ifdef  DEBUG_TIMING_PATCH
		if (get_timer(0) - start > MAX_RETRY_MS)
		{
			printf("%s: timedout waiting for cc!\n", __func__);
			return;
		}
      	#endif

	}
	writel(CC_MASK, &mmc_base->stat)
		;
	writel(MMC_CMD0, &mmc_base->cmd)
		;

#ifdef  DEBUG_TIMING_PATCH
	start = get_timer(0);
#endif
	while (!(readl(&mmc_base->stat) & CC_MASK))
	{
#ifdef  DEBUG_TIMING_PATCH
		if (get_timer(0) - start > MAX_RETRY_MS)
		{
			printf("%s: timedout waiting for cc2!\n", __func__);
			return;
		}
#endif
	}

	writel(readl(&mmc_base->con) & ~INIT_INITSTREAM, &mmc_base->con);
}

static int omap_hsmmc_init_setup(struct mmc *mmc)
{
	struct hsmmc *mmc_base;
	unsigned int reg_val;
	unsigned int dsor;
//	unsigned long long start;


#ifdef DEBUG
	printf("!+omap_hsmmc.c/omap_hsmmc_init_setup\n");
    ConsoleUtilsPrintf("!+omap_hsmmc.c/omap_hsmmc_init_setup\n");
#endif





    mmc_base = ((struct omap_hsmmc_data *)mmc->priv)->base_addr;
	mmc_board_init(mmc);

	writel(readl(&mmc_base->sysconfig) | MMC_SOFTRESET,&mmc_base->sysconfig);
//////////////////////////////////////////////////////////////////////////////////////
#ifdef  DEBUG_TIMING_PATCH
	start = get_timer(0);
#endif
	while ((readl(&mmc_base->sysstatus) & RESETDONE) == 0)
	{

#ifdef  DEBUG_TIMING_PATCH
		if (get_timer(0) - start > MAX_RETRY_MS)
		{
			printf("%s: timedout waiting for cc2!\n", __func__);
			return TIMEOUT;
		}
#endif
	}
	writel(readl(&mmc_base->sysctl) | SOFTRESETALL, &mmc_base->sysctl);
	//////////////////////////////////////////////////////////////////////////////////////

#ifdef  DEBUG_TIMING_PATCH
	start = get_timer(0);
#endif
	while ((readl(&mmc_base->sysctl) & SOFTRESETALL) != 0x0)
	{
#ifdef  DEBUG_TIMING_PATCH
		if (get_timer(0) - start > MAX_RETRY_MS)
		{
			printf("%s: timedout waiting for softresetall!\n",__func__);
			return TIMEOUT;
		}
#endif

	}

	writel(DTW_1_BITMODE | SDBP_PWROFF | SDVS_3V0, &mmc_base->hctl);
	writel(readl(&mmc_base->capa) | VS30_3V0SUP | VS18_1V8SUP,&mmc_base->capa);

	reg_val = readl(&mmc_base->con) & RESERVED_MASK;

	writel(CTPL_MMC_SD | reg_val | WPP_ACTIVEHIGH | CDP_ACTIVEHIGH |
		MIT_CTO | DW8_1_4BITMODE | MODE_FUNC | STR_BLOCK |
		HR_NOHOSTRESP | INIT_NOINIT | NOOPENDRAIN, &mmc_base->con);

	dsor = 240;
	mmc_reg_out(&mmc_base->sysctl, (ICE_MASK | DTO_MASK | CEN_MASK),
		(ICE_STOP | DTO_15THDTO | CEN_DISABLE));
	mmc_reg_out(&mmc_base->sysctl, ICE_MASK | CLKD_MASK,
		(dsor << CLKD_OFFSET) | ICE_OSCILLATE);
//////////////////////////////////////////////////////////////////////////////////////

#ifdef  DEBUG_TIMING_PATCH
	start = get_timer(0);
#endif
	while ((readl(&mmc_base->sysctl) & ICS_MASK) == ICS_NOTREADY)
	{
#ifdef  DEBUG_TIMING_PATCH
		if (get_timer(0) - start > MAX_RETRY_MS)
		{
			printf("%s: timedout waiting for ics!\n", __func__);
			return TIMEOUT;
		}
#endif
	}


	writel(readl(&mmc_base->sysctl) | CEN_ENABLE, &mmc_base->sysctl);
	writel(readl(&mmc_base->hctl) | SDBP_PWRON, &mmc_base->hctl);
	writel(IE_BADA | IE_CERR | IE_DEB | IE_DCRC | IE_DTO | IE_CIE |IE_CEB | IE_CCRC | IE_CTO | IE_BRR | IE_BWR | IE_TC | IE_CC,&mmc_base->ie);
	mmc_init_stream(mmc_base);

	return 0;
}

/*
 * MMC controller internal finite state machine reset
 *
 * Used to reset command or data internal state machines, using respectively
 * SRC or SRD bit of SYSCTL register
 */
static void mmc_reset_controller_fsm(struct hsmmc *mmc_base, unsigned int bit)
{
//	unsigned long long start;
	unsigned int i=0;
	//printf("!+omap_hsmmc.c/mmc_reset_controller_fsm_\n");
	mmc_reg_out(&mmc_base->sysctl, bit, bit);

	/*
	 * CMD(DAT) lines reset procedures are slightly different
	 * for OMAP3 and OMAP4(AM335x,OMAP5,DRA7xx).
	 * According to OMAP3 TRM:
	 * Set SRC(SRD) bit in MMCHS_SYSCTL register to 0x1 and wait until it
	 * returns to 0x0.
	 * According to OMAP4(AM335x,OMAP5,DRA7xx) TRMs, CMD(DATA) lines reset
	 * procedure steps must be as follows:
	 * 1. Initiate CMD(DAT) line reset by writing 0x1 to SRC(SRD) bit in
	 *    MMCHS_SYSCTL register (SD_SYSCTL for AM335x).
	 * 2. Poll the SRC(SRD) bit until it is set to 0x1.
	 * 3. Wait until the SRC (SRD) bit returns to 0x0
	 *    (reset procedure is completed).
	 */
#if defined(CONFIG_OMAP44XX) || defined(CONFIG_OMAP54XX) || \
	defined(CONFIG_AM33XX)

	  //printf("!+omap_hsmmc.c/mmc_reset_controller_fsm_\n");
	//printf("!+omap_hsmmc.c/CONFIG_AM33XX|CONFIG_OMAP44XX\n");
	//printf("!+omap_hsmmc.c/CONFIG_AM33XX|CONFIG_OMAP44XX_mmc_reset_controller_fsm__bit=0x%x\n",bit);

	if (!(readl(&mmc_base->sysctl) & bit))
	{
		//start = get_timer(0);
		while (!(readl(&mmc_base->sysctl) & bit))
		{
			//printf("!+omap_hsmmc.c/CONFIG_AM33XX|CONFIG_OMAP44XX=0x%x|%d\n",readl(&mmc_base->sysctl),i);
			i++;

			if(i==600000)
			//if(i==2274597)
			{

			//if (get_timer(0) - start > MAX_RETRY_MS)
				printf("++++++++++++CONFIG_OMAP44XX_RETURN =0x%x|i=%d|ms=0x%x+++++++++++++++\n\r",readl(&mmc_base->sysctl),i,MAX_RETRY_MS);



#ifdef DEBUG_ERROR_UART_CONSOLE
				ConsoleUtilsPrintf("++++++++++++CONFIG_OMAP44XX_RETURN =0x%x|i=%d|ms=0x%x+++++++++++++++\n\r",readl(&mmc_base->sysctl),i,MAX_RETRY_MS);
#endif

				return;
			}
		}
	}
#endif

#ifdef  DEBUG_TIMING_PATCH
	start = get_timer(0);
#endif
	while ((readl(&mmc_base->sysctl) & bit) != 0)
	{
#ifdef  DEBUG_TIMING_PATCH
		if (get_timer(0) - start > MAX_RETRY_MS)
		{
			printf("%s: timedout waiting for sysctl %x to clear\n",__func__, bit);
			return;
		}
#endif

	}
}


static int omap_hsmmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,struct mmc_data *data)
{
	struct hsmmc *mmc_base;
	unsigned int flags, mmc_stat;
//	unsigned long long start;


#ifdef DEBUG
	printf("!+omap_hsmmc.c/omap_hsmmc_send_cmd\n");
    ConsoleUtilsPrintf("!+omap_hsmmc.c/omap_hsmmc_send_cmd\n");
#endif



    mmc_base = ((struct omap_hsmmc_data *)mmc->priv)->base_addr;

#ifdef  DEBUG_TIMING_PATCH
    start = get_timer(0);
#endif
    while ((readl(&mmc_base->pstate) & (DATI_MASK | CMDI_MASK)) != 0)
	{
#ifdef  DEBUG_TIMING_PATCH
    	if (get_timer(0) - start > MAX_RETRY_MS)
		{
			printf("%s: timedout waiting on cmd inhibit to clear\n",__func__);
			return TIMEOUT;
		}
#endif
	}

	writel(0xFFFFFFFF, &mmc_base->stat);


#ifdef  DEBUG_TIMING_PATCH
	start = get_timer(0);
#endif

	while (readl(&mmc_base->stat))
	{


		//printf("timedout waiting for STAT %x\n\r", readl(&mmc_base->stat);

#ifdef  DEBUG_TIMING_PATCH
		if (get_timer(0) - start > MAX_RETRY_MS)
		{
			printf("%s: timedout waiting for STAT (%x) to clear\n",__func__, readl(&mmc_base->stat));
			return TIMEOUT;
		}
#endif
	}
	//printf("omap_hsmmc_send_cmd/ok_mmc_base->stat=0x%x\n\r",readl(&mmc_base->stat));


	/*
	 * CMDREG
	 * CMDIDX[13:8]	: Command index
	 * DATAPRNT[5]	: Data Present Select
	 * ENCMDIDX[4]	: Command Index Check Enable
	 * ENCMDCRC[3]	: Command CRC Check Enable
	 * RSPTYP[1:0]
	 *	00 = No Response
	 *	01 = Length 136
	 *	10 = Length 48
	 *	11 = Length 48 Check busy after response
	 */
	/* Delay added before checking the status of frq change
	 * retry not supported by mmc.c(core file)
	 */
	if (cmd->cmdidx == SD_CMD_APP_SEND_SCR)
		//udelay(50000); /* wait 50 ms */
        //  Sysdelay(50);
		//printf("udelay(50000).\n\r");
		syl_Sysdelay(50); //50 ms


	if (!(cmd->resp_type & MMC_RSP_PRESENT))
		flags = 0;
	else if (cmd->resp_type & MMC_RSP_136)
		flags = RSP_TYPE_LGHT136 | CICE_NOCHECK;
	else if (cmd->resp_type & MMC_RSP_BUSY)
		flags = RSP_TYPE_LGHT48B;
	else
		flags = RSP_TYPE_LGHT48;

	/* enable default flags */
	flags =	flags | (CMD_TYPE_NORMAL | CICE_NOCHECK | CCCE_NOCHECK | MSBS_SGLEBLK | ACEN_DISABLE | BCE_DISABLE | DE_DISABLE);

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= CCCE_CHECK;
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= CICE_CHECK;

	if (data)
	{
		//printf("if_data =0x%x\n\r",data);

		if ((cmd->cmdidx == MMC_CMD_READ_MULTIPLE_BLOCK) ||
			 (cmd->cmdidx == MMC_CMD_WRITE_MULTIPLE_BLOCK)) {
			flags |= (MSBS_MULTIBLK | BCE_ENABLE);
			data->blocksize = 512;
			writel(data->blocksize | (data->blocks << 16),&mmc_base->blk);
		} else
			writel(data->blocksize | NBLK_STPCNT, &mmc_base->blk);

		if (data->flags & MMC_DATA_READ)
			flags |= (DP_DATA | DDIR_READ);
		else
			flags |= (DP_DATA | DDIR_WRITE);
	}

	writel(cmd->cmdarg, &mmc_base->arg);


	//printf("udelay(20.\n\r");
	syl_Sysdelay(50);

	//udelay(20);		/* To fix "No status update" error on eMMC */
	writel((cmd->cmdidx << 24) | flags, &mmc_base->cmd);

	//start = get_timer(0);
	do {
		mmc_stat = readl(&mmc_base->stat);



#ifdef DEBUG
		printf("do {_mmc_stat=0%x|%d\n", mmc_stat,mmc_stat);
		ConsoleUtilsPrintf("do {_mmc_stat=0%x|%d\n", mmc_stat,mmc_stat);
#endif


		/*
		if (get_timer(0) - start > MAX_RETRY_MS)
		{
			printf("%s : timeout: No status update\n", __func__);
			return TIMEOUT;
		}*/

	} while (!mmc_stat);

	if ((mmc_stat & IE_CTO) != 0)
	{

#ifdef DEBUG
		printf("(mmc_stat & IE_CTO) != 0)\n\r");
	    ConsoleUtilsPrintf("(mmc_stat & IE_CTO) != 0)\n\r");
#endif

		mmc_reset_controller_fsm(mmc_base, SYSCTL_SRC);
		{
		  printf("(mmc_stat & IE_CTO) != 0)_return TIMEOUT=0x%x;\n\r",TIMEOUT);

#ifdef DEBUG_ERROR_UART_CONSOLE
		  ConsoleUtilsPrintf("(mmc_stat & IE_CTO) != 0)_return TIMEOUT=0x%x;\n\r",TIMEOUT);
#endif

		  return TIMEOUT;

	   }

	}
	else if ((mmc_stat & ERRI_MASK) != 0)
	{
	printf("return -1\n\r");

#ifdef DEBUG_ERROR_UART_CONSOLE
	ConsoleUtilsPrintf("return -1\n\r");
#endif

	return -1;
	}


	if (mmc_stat & CC_MASK) {


#ifdef DEBUG
		printf("(mmc_stat & CC_MASK)\n\r");
		ConsoleUtilsPrintf("(mmc_stat & CC_MASK)\n\r");
#endif




		writel(CC_MASK, &mmc_base->stat);
		if (cmd->resp_type & MMC_RSP_PRESENT) {
			if (cmd->resp_type & MMC_RSP_136) {
				/* response type 2 */
				cmd->response[3] = readl(&mmc_base->rsp10);
				cmd->response[2] = readl(&mmc_base->rsp32);
				cmd->response[1] = readl(&mmc_base->rsp54);
				cmd->response[0] = readl(&mmc_base->rsp76);
			} else
				/* response types 1, 1b, 3, 4, 5, 6 */
				cmd->response[0] = readl(&mmc_base->rsp10);
		}
	}

	if (data && (data->flags & MMC_DATA_READ)) {
		mmc_read_data(mmc_base,	data->dest,
				data->blocksize * data->blocks);
	} else if (data && (data->flags & MMC_DATA_WRITE)) {
		mmc_write_data(mmc_base, data->src,
				data->blocksize * data->blocks);
	}
	return 0;
}

static int mmc_read_data(struct hsmmc *mmc_base, char *buf, unsigned int size)
{
	unsigned int *output_buf = (unsigned int *)buf;
	unsigned int mmc_stat;
	unsigned int count;

    printf("!+omap_hsmmc.c/mmc_read_data\n\r");



#ifdef DEBUG_ERROR_UART_CONSOLE
    ConsoleUtilsPrintf("!+omap_hsmmc.c/mmc_read_data\n\r");
#endif
	/*
	 * Start Polled Read
	 */
	count = (size > MMCSD_SECTOR_SIZE) ? MMCSD_SECTOR_SIZE : size;
	count /= 4;

	while (size)
	{
#ifdef  DEBUG_TIMING_PATCH
		unsigned long long start = get_timer(0);
#endif

		do {
			mmc_stat = readl(&mmc_base->stat);

#ifdef  DEBUG_TIMING_PATCH
			if (get_timer(0) - start > MAX_RETRY_MS)
			{
				printf("%s: timedout waiting for status!\n",__func__);return TIMEOUT;
			}
#endif

		} while (mmc_stat == 0);

		if ((mmc_stat & (IE_DTO | IE_DCRC | IE_DEB)) != 0)
			mmc_reset_controller_fsm(mmc_base, SYSCTL_SRD);

		if ((mmc_stat & ERRI_MASK) != 0)
			return 1;

		if (mmc_stat & BRR_MASK)
		{
			unsigned int k;

			writel(readl(&mmc_base->stat) | BRR_MASK,&mmc_base->stat);
			for (k = 0; k < count; k++)
			{
				*output_buf = readl(&mmc_base->data);
				output_buf++;
			}
			size -= (count*4);
		}

		if (mmc_stat & BWR_MASK)
			writel(readl(&mmc_base->stat) | BWR_MASK,
				&mmc_base->stat);

		if (mmc_stat & TC_MASK) {
			writel(readl(&mmc_base->stat) | TC_MASK,
				&mmc_base->stat);
			break;
		}
	}
	return 0;
}

static int mmc_write_data(struct hsmmc *mmc_base, const char *buf,unsigned int size)
{
	unsigned int *input_buf = (unsigned int *)buf;
	unsigned int mmc_stat;
	unsigned int count;

	/*
	 * Start Polled Write
	 */
	count = (size > MMCSD_SECTOR_SIZE) ? MMCSD_SECTOR_SIZE : size;
	count /= 4;

	printf("!+omap_hsmmc.c/mmc_write_data_size=%d\n\r",size);


	while (size)
	{


#ifdef  DEBUG_TIMING_PATCH
		unsigned long long start = get_timer(0);
#endif


		do
		{
			mmc_stat = readl(&mmc_base->stat);

#ifdef  DEBUG_TIMING_PATCH
			if (get_timer(0) - start > MAX_RETRY_MS)
			{
				printf("%s: timedout waiting for status!\n",__func__);
				return TIMEOUT;
			}
#endif

		} while (mmc_stat == 0);

		if ((mmc_stat & (IE_DTO | IE_DCRC | IE_DEB)) != 0)
			mmc_reset_controller_fsm(mmc_base, SYSCTL_SRD);

		if ((mmc_stat & ERRI_MASK) != 0)
			return 1;

		if (mmc_stat & BWR_MASK) {
			unsigned int k;

			writel(readl(&mmc_base->stat) | BWR_MASK,&mmc_base->stat);
			for (k = 0; k < count; k++) {
				writel(*input_buf, &mmc_base->data);
				input_buf++;
			}
			size -= (count*4);
		}

		if (mmc_stat & BRR_MASK)
			writel(readl(&mmc_base->stat) | BRR_MASK,&mmc_base->stat);

		if (mmc_stat & TC_MASK) {
			writel(readl(&mmc_base->stat) | TC_MASK,&mmc_base->stat);
			break;
		}
	}
	return 0;
}

static void omap_hsmmc_set_ios(struct mmc *mmc)
{
	struct hsmmc *mmc_base;
	unsigned int dsor = 0;
//	unsigned long long start;

#ifdef DEBUG

	printf("!+omap_hsmmc.c/omap_hsmmc_set_ios\n\r");
    ConsoleUtilsPrintf("!+omap_hsmmc.c/omap_hsmmc_set_ios\n\r");
#endif




    mmc_base = ((struct omap_hsmmc_data *)mmc->priv)->base_addr;
	/* configue bus width */
	switch (mmc->bus_width)
	{
	case 8:
		writel(readl(&mmc_base->con) | DTW_8_BITMODE,&mmc_base->con);
		break;

	case 4:
		writel(readl(&mmc_base->con) & ~DTW_8_BITMODE,&mmc_base->con);
		writel(readl(&mmc_base->hctl) | DTW_4_BITMODE,&mmc_base->hctl);
		break;

	case 1:
	default:
		writel(readl(&mmc_base->con) & ~DTW_8_BITMODE,&mmc_base->con);
		writel(readl(&mmc_base->hctl) & ~DTW_4_BITMODE,&mmc_base->hctl);
		break;
	}

	/* configure clock with 96Mhz system clock.
	 */
	if (mmc->clock != 0) {
		dsor = (MMC_CLOCK_REFERENCE * 1000000 / mmc->clock);
		if ((MMC_CLOCK_REFERENCE * 1000000) / dsor > mmc->clock)
			dsor++;
	}

	mmc_reg_out(&mmc_base->sysctl, (ICE_MASK | DTO_MASK | CEN_MASK),(ICE_STOP | DTO_15THDTO | CEN_DISABLE));

	mmc_reg_out(&mmc_base->sysctl, ICE_MASK | CLKD_MASK,(dsor << CLKD_OFFSET) | ICE_OSCILLATE);

#ifdef  DEBUG_TIMING_PATCH
	start = get_timer(0);
#endif

	while ((readl(&mmc_base->sysctl) & ICS_MASK) == ICS_NOTREADY)
	{

#ifdef  DEBUG_TIMING_PATCH
		if (get_timer(0) - start > MAX_RETRY_MS)
		{
			printf("%s: timedout waiting for ics!\n", __func__);
			return;
		}
#endif
	}

	writel(readl(&mmc_base->sysctl) | CEN_ENABLE, &mmc_base->sysctl);
}


static const struct mmc_ops omap_hsmmc_ops = {
	.send_cmd	= omap_hsmmc_send_cmd,
	.set_ios	= omap_hsmmc_set_ios,
	.init		= omap_hsmmc_init_setup,
#ifdef OMAP_HSMMC_USE_GPIO
	.getcd		= omap_hsmmc_getcd,
	.getwp		= omap_hsmmc_getwp,
#endif
};

int omap_mmc_init(int dev_index, unsigned int host_caps_mask, unsigned int f_max, int cd_gpio,int wp_gpio)
{
	struct mmc *mmc;
	struct omap_hsmmc_data *priv_data;
	struct mmc_config *cfg;
	unsigned int host_caps_val;

	//printf("+dev_index=%x,f_max=%x+\n\r",dev_index,f_max);


	priv_data = malloc(sizeof(*priv_data));
	if (priv_data == NULL)
	{
		return -1;
	}


	host_caps_val = MMC_MODE_4BIT| MMC_MODE_HS_52MHz | MMC_MODE_HS |MMC_MODE_HC;

	switch (dev_index)
	{
	case 0:
		 priv_data->base_addr = (struct hsmmc *)OMAP_HSMMC1_BASE;
		 printf("+omap_hsmmc.c/omap_mmc_init_SD_CARD\n\r");


#ifdef DEBUG_ERROR_UART_CONSOLE
		 ConsoleUtilsPrintf("+omap_hsmmc.c/omap_mmc_init_SD_CARD\n\r");
#endif


		 break;
#ifdef OMAP_HSMMC2_BASE
	case 1:
		priv_data->base_addr = (struct hsmmc *)OMAP_HSMMC2_BASE;
		/* Enable 8-bit interface for eMMC on OMAP4/5 or DRA7XX */
	    //host_caps_val |= MMC_MODE_8BIT;
	    printf("+omap_hsmmc.c/omap_mmc_init_eMMC_CARD_8bit\n\r");

		#ifdef DEBUG_ERROR_UART_CONSOLE
	    ConsoleUtilsPrintf("+omap_hsmmc.c/omap_mmc_init_eMMC_CARD_8bit\n\r");
		#endif

	    break;
#endif
#ifdef OMAP_HSMMC3_BASE
	case 2:
		priv_data->base_addr = (struct hsmmc *)OMAP_HSMMC3_BASE;
#if (defined(CONFIG_DRA7XX) || defined(CONFIG_AM57XX)) && defined(CONFIG_HSMMC3_8BIT)
		/* Enable 8-bit interface for eMMC on DRA7XX */
		host_caps_val |= MMC_MODE_8BIT;
#endif
		break;
#endif
	default:
		priv_data->base_addr = (struct hsmmc *)OMAP_HSMMC1_BASE;
		return 1;
	}
#ifdef OMAP_HSMMC_USE_GPIO
	/* on error gpio values are set to -1, which is what we want */
	priv_data->cd_gpio = omap_mmc_setup_gpio_in(cd_gpio, "mmc_cd");
	priv_data->wp_gpio = omap_mmc_setup_gpio_in(wp_gpio, "mmc_wp");
#endif

	cfg = &priv_data->cfg;

	cfg->name = "OMAP SD/MMC";
	cfg->ops = &omap_hsmmc_ops;

	cfg->voltages = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;
	cfg->host_caps = host_caps_val & ~host_caps_mask;

	cfg->f_min = 400000;

	if (f_max != 0)
		cfg->f_max = f_max;
	else {
		if (cfg->host_caps & MMC_MODE_HS) {
			if (cfg->host_caps & MMC_MODE_HS_52MHz)
				cfg->f_max = 52000000;
			else
				cfg->f_max = 26000000;
		} else
			cfg->f_max = 20000000;
	}

	cfg->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

#if defined(CONFIG_OMAP34XX)
	/*
	 * Silicon revs 2.1 and older do not support multiblock transfers.
	 */
	if ((get_cpu_family() == CPU_OMAP34XX) && (get_cpu_rev() <= CPU_3XX_ES21))
		cfg->b_max = 1;
#endif
	mmc = mmc_create(cfg, priv_data);
	if (mmc == NULL)
	{
		return -1;
	}

	return 0;
}

////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////ADD WRITE functions//////////////////////////////////////////////////////////////
static unsigned int mmc_erase_t(struct mmc *mmc, unsigned int start, unsigned int blkcnt)
{
	struct mmc_cmd cmd;
	unsigned int end;
	int err, start_cmd, end_cmd;

	printf("mmc_write.c/mmc_erase_t=blkcnt_0x%x\n\r",blkcnt);

	if (mmc->high_capacity) {
		end = start + blkcnt - 1;
	} else {
		end = (start + blkcnt - 1) * mmc->write_bl_len;
		start *= mmc->write_bl_len;
	}

	if (IS_SD(mmc)) {
		start_cmd = SD_CMD_ERASE_WR_BLK_START;
		end_cmd = SD_CMD_ERASE_WR_BLK_END;
	} else {
		start_cmd = MMC_CMD_ERASE_GROUP_START;
		end_cmd = MMC_CMD_ERASE_GROUP_END;
	}

	cmd.cmdidx = start_cmd;
	cmd.cmdarg = start;
	cmd.resp_type = MMC_RSP_R1;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		goto err_out;

	cmd.cmdidx = end_cmd;
	cmd.cmdarg = end;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		goto err_out;

	cmd.cmdidx = MMC_CMD_ERASE;
	cmd.cmdarg = SECURE_ERASE;
	cmd.resp_type = MMC_RSP_R1b;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		goto err_out;

	return 0;

err_out:
	puts("mmc erase failed\n");
	return err;
}

unsigned int mmc_berase(int dev_num, unsigned int  start,unsigned int cnt /*lbaint_t blkcnt*/)
//unsigned long mmc_berase(int dev_num, lbaint_t start,unsigned int cnt /*lbaint_t blkcnt*/)
{
	int err = 0;
	struct mmc *mmc = find_mmc_device(dev_num);
	unsigned int blk = 0, blk_r = 0;
	int timeout = 1000;
    unsigned int blkcnt =512;



	if (!mmc)
	{
		return 0;
	}

	printf("mmc_write.c/mmc_berase=blkcnt_0x%x\n\r",blkcnt);


	if ((start % mmc->erase_grp_size) || (blkcnt % mmc->erase_grp_size))
		printf("\n\nCaution! Your devices Erase group is 0x%x\n"
		       "The erase range would be change to "
		       "0x" LBAF "~0x" LBAF "\n\n",mmc->erase_grp_size, start & ~(mmc->erase_grp_size - 1),((start + blkcnt + mmc->erase_grp_size) & ~(mmc->erase_grp_size - 1)) - 1);

	while (blk < blkcnt) {
		blk_r = ((blkcnt - blk) > mmc->erase_grp_size) ?
			mmc->erase_grp_size : (blkcnt - blk);
		err = mmc_erase_t(mmc, start + blk, blk_r);
		if (err)
			break;

		blk += blk_r;

		/* Waiting for the ready status */
		if (mmc_send_status(mmc, timeout))
			return 0;
	}

	return blk;
}



static unsigned int mmc_write_blocks(struct mmc *mmc,unsigned int start,unsigned int blkcnt_ext, const void *src)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int timeout = 1000;
    int mmchost_is_spi=0;
    unsigned int blkcnt=256;

	printf("mmc_write.c/mmc_write_blocks=blkcnt_0x%x\n\r",blkcnt);

	//mmc->block_dev.lba_c

	if ((start + blkcnt) > mmc->block_dev.lba_c.quot)
		{
			printf("MMC: block number 0x" LBAF " exceeds max(0x" LBAF ")\n",start + blkcnt, mmc->block_dev.lba_c.quot);
			return 0;
		}

	/*  //TODO rename
	if ((start + blkcnt) > mmc->block_dev.lba)
	{
		printf("MMC: block number 0x" LBAF " exceeds max(0x" LBAF ")\n",start + blkcnt, mmc->block_dev.lba);
		return 0;
	}*/

	if (blkcnt == 0)
		return 0;
	else if (blkcnt == 1)
		cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * mmc->write_bl_len;

	cmd.resp_type = MMC_RSP_R1;

	data.src = src;
	data.blocks = blkcnt;
	data.blocksize = mmc->write_bl_len;
	data.flags = MMC_DATA_WRITE;

	if (mmc_send_cmd(mmc, &cmd, &data)) {
		printf("mmc write failed\n");
		return 0;
	}


	/* SPI multiblock writes terminate using a special
	 * token, not a STOP_TRANSMISSION request.
	 */
	//if (!mmc_host_is_spi(mmc) && blkcnt > 1)
	if (!mmchost_is_spi && blkcnt > 1)
	{
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1b;
		if (mmc_send_cmd(mmc, &cmd, NULL))
		{
			printf("mmc fail to send stop cmd\n");
			return 0;
		}
	}

	/* Waiting for the ready status */
	if (mmc_send_status(mmc, timeout))
	{
		return 0;
	}

	return blkcnt;
}

unsigned int mmc_bwrite(int dev_num, unsigned int start, unsigned int blkcnt, const void *src)
//unsigned long mmc_bwrite(int dev_num, lbaint_t start, lbaint_t blkcnt_external, const void *src)
{

	unsigned int cur, blocks_todo = blkcnt;

	struct mmc *mmc = find_mmc_device(dev_num);

	printf("mmc_write.c/mmc_bwrite=blkcnt_0x%x\n\r",blkcnt);


	if (!mmc)
	{
		return 0;
	}


	if (mmc_set_blocklen(mmc, mmc->write_bl_len))
	{
		return 0;
	}

	do {
		cur = (blocks_todo > mmc->cfg->b_max) ?
			mmc->cfg->b_max : blocks_todo;
		if (mmc_write_blocks(mmc, start, cur, src) != cur)
			return 0;
		blocks_todo -= cur;
		start += cur;
		src += cur * mmc->write_bl_len;
	} while (blocks_todo > 0);

return blkcnt;
}






#if 0

static void test_mmc0_sd_pin_mux_setup()
{
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_DAT3) =
                   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_DAT2) =
                   (0 << CONTROL_CONF_MMC0_DAT2_CONF_MMC0_DAT2_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_DAT2_CONF_MMC0_DAT2_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_DAT2_CONF_MMC0_DAT2_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_DAT2_CONF_MMC0_DAT2_RXACTIVE_SHIFT);

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_DAT1) =
                   (0 << CONTROL_CONF_MMC0_DAT1_CONF_MMC0_DAT1_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_DAT1_CONF_MMC0_DAT1_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_DAT1_CONF_MMC0_DAT1_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_DAT1_CONF_MMC0_DAT1_RXACTIVE_SHIFT);

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_DAT0) =
                   (0 << CONTROL_CONF_MMC0_DAT0_CONF_MMC0_DAT0_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_DAT0_CONF_MMC0_DAT0_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_DAT0_CONF_MMC0_DAT0_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_DAT0_CONF_MMC0_DAT0_RXACTIVE_SHIFT);

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_CLK) =
                   (0 << CONTROL_CONF_MMC0_CLK_CONF_MMC0_CLK_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_CLK_CONF_MMC0_CLK_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_CLK_CONF_MMC0_CLK_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_CLK_CONF_MMC0_CLK_RXACTIVE_SHIFT);

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_CMD) =
                   (0 << CONTROL_CONF_MMC0_CMD_CONF_MMC0_CMD_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_CMD_CONF_MMC0_CMD_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_CMD_CONF_MMC0_CMD_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_CMD_CONF_MMC0_CMD_RXACTIVE_SHIFT);

     HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_CS1) =
                   (5 << CONTROL_CONF_SPI0_CS1_CONF_SPI0_CS1_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_SPI0_CS1_CONF_SPI0_CS1_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_SPI0_CS1_CONF_SPI0_CS1_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_SPI0_CS1_CONF_SPI0_CS1_RXACTIVE_SHIFT);

}




static void test_mmc1_emmc_pin_mux_setup()
{
    HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(0))=CONTROL_CONF_MUXMODE(1); //25
    HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(1))=CONTROL_CONF_MUXMODE(1); //24
    HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(2))=CONTROL_CONF_MUXMODE(1); // 5
    HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(3))=CONTROL_CONF_MUXMODE(1); // 6
    HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(4))=CONTROL_CONF_MUXMODE(1); //23
    HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(5))=CONTROL_CONF_MUXMODE(1); //22
    HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(6))=CONTROL_CONF_MUXMODE(1); // 3
    HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(7))=CONTROL_CONF_MUXMODE(1); // 4
    HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_CSN(2))=CONTROL_CONF_MUXMODE(2); //20
    HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_CSN(1))=CONTROL_CONF_MUXMODE(2); //21

}



/*****************************************************************************
Syntax:      	    Init_FIFO_voice_rtp_buf ();
Remarks:			This Function Init FIFO buffers
*******************************************************************************/
static void test_mmc0_sd_clk_enable()
{
    HWREG(SOC_PRCM_REGS + CM_PER_MMC0_CLKCTRL)|=CM_PER_MMC0_CLKCTRL_MODULEMODE_ENABLE;
    while((HWREG(SOC_PRCM_REGS + CM_PER_MMC0_CLKCTRL) & CM_PER_MMC0_CLKCTRL_MODULEMODE) != CM_PER_MMC0_CLKCTRL_MODULEMODE_ENABLE);

}


/*****************************************************************************
Syntax:      	    Init_FIFO_voice_rtp_buf ();
Remarks:			This Function Init FIFO buffers
*******************************************************************************/
static void test_mmc1_emmc_clk_enable()
{
    HWREG(SOC_PRCM_REGS+CM_PER_MMC1_CLKCTRL)|=CM_PER_MMC1_CLKCTRL_MODULEMODE_ENABLE;
    while ((HWREG(SOC_PRCM_REGS+CM_PER_MMC1_CLKCTRL) & CM_PER_MMC1_CLKCTRL_MODULEMODE)!=CM_PER_MMC1_CLKCTRL_MODULEMODE_ENABLE);

}


#endif













