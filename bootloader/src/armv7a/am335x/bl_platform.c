/**
 * \file  bl_platform.c
 *
 * \brief Initializes AM335x Device Peripherals.
 *
 */

/*
* Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "hw_types.h"
#include "hw_cm_cefuse.h"
#include "hw_cm_device.h"
#include "hw_cm_dpll.h"
#include "hw_cm_gfx.h"
#include "hw_cm_mpu.h"
#include "hw_cm_per.h"
#include "hw_cm_rtc.h"
#include "hw_cm_wkup.h"
#include "hw_control_AM335x.h"
#include "hw_emif4d.h"
#include "bl.h"
#include "gpmc.h"
#include "bl_copy.h"
#include "bl_platform.h"
#include "uartStdio.h"
#include "watchdog.h"
#include "hsi2c.h"
#include "gpio_v2.h"
#include "board.h"
#include "device.h"
#include "string.h"
#ifdef evmAM335x
    #include "hw_tps65910.h"
#elif  (defined beaglebone)
    #include "hw_tps65217.h"
    #include "pin_mux.h"  	/*This is required by the DDRVTT function,which is a necessary setting for DDR3 */
#elif  (defined evmskAM335x)
    #include "hw_tps65910.h"
    #include "pin_mux.h"
#endif

#if defined(SPI)
    #include "bl_spi.h"
#elif defined(MMCSD)
    #include "bl_mmcsd.h"
#elif defined(NAND)
    #include "bl_nand.h"
    #include "nandlib.h"
    #include "nand_gpmc.h"
#endif


/*GPIO START*/
#define GPIO_INSTANCE_ADDRESS           (SOC_GPIO_1_REGS)
#define GPIO_INSTANCE_PIN_NUMBER        (23)
static void DDRVTTEnable(void);
/******************************************************************************
**                     Internal Macro Definitions
*******************************************************************************/
#define INTVECMAX                          (9)
#define BIT(n)                             (1 << (n))
#define PAD_CTRL_PULLUDDISABLE             (BIT(3))
#define PAD_CTRL_PULLUPSEL                 (BIT(4))
#define PAD_CTRL_RXACTIVE                  (BIT(5))
#define PAD_CTRL_SLOWSLEW                  (BIT(6))
#define PAD_CTRL_MUXMODE(n)                ((n))

/*	I2C instance	*/
#define  I2C_0				   (0x0u)
/*	System clock fed to I2C module - 48Mhz	*/
#define  I2C_SYSTEM_CLOCK		   (48000000u)
/*	Internal clock used by I2C module - 12Mhz	*/
#define  I2C_INTERNAL_CLOCK		   (12000000u)
/*	I2C bus speed or frequency - 100Khz	*/
#define	 I2C_OUTPUT_CLOCK		   (100000u)
/*	I2C interrupt flags to clear	*/
#define  I2C_INTERRUPT_FLAG_TO_CLR         (0x7FF)
#define  PMIC_SR_I2C_SLAVE_ADDR            (0x12)
#define SMPS_DRIVE_SCLSR_EN1	           (0x0u)
#define SMPS_DRIVE_SDASR_EN2	           (0x1u)




/* TODO : These are not there in the control module header file */
#define DDR_PHY_CTRL_REGS                  (SOC_CONTROL_REGS + 0x2000)
#define CMD0_SLAVE_RATIO_0                 (DDR_PHY_CTRL_REGS + 0x1C)
#define CMD0_SLAVE_FORCE_0                 (DDR_PHY_CTRL_REGS + 0x20)
#define CMD0_SLAVE_DELAY_0                 (DDR_PHY_CTRL_REGS + 0x24)
#define CMD0_LOCK_DIFF_0                   (DDR_PHY_CTRL_REGS + 0x28)
#define CMD0_INVERT_CLKOUT_0               (DDR_PHY_CTRL_REGS + 0x2C)
#define CMD1_SLAVE_RATIO_0                 (DDR_PHY_CTRL_REGS + 0x50)
#define CMD1_SLAVE_FORCE_0                 (DDR_PHY_CTRL_REGS + 0x54)
#define CMD1_SLAVE_DELAY_0                 (DDR_PHY_CTRL_REGS + 0x58)
#define CMD1_LOCK_DIFF_0                   (DDR_PHY_CTRL_REGS + 0x5C)
#define CMD1_INVERT_CLKOUT_0               (DDR_PHY_CTRL_REGS + 0x60)
#define CMD2_SLAVE_RATIO_0                 (DDR_PHY_CTRL_REGS + 0x84)
#define CMD2_SLAVE_FORCE_0                 (DDR_PHY_CTRL_REGS + 0x88)
#define CMD2_SLAVE_DELAY_0                 (DDR_PHY_CTRL_REGS + 0x8C)
#define CMD2_LOCK_DIFF_0                   (DDR_PHY_CTRL_REGS + 0x90)
#define CMD2_INVERT_CLKOUT_0               (DDR_PHY_CTRL_REGS + 0x94)
#define DATA0_RD_DQS_SLAVE_RATIO_0         (DDR_PHY_CTRL_REGS + 0xC8)
#define DATA0_RD_DQS_SLAVE_RATIO_1         (DDR_PHY_CTRL_REGS + 0xCC)
#define DATA0_WR_DQS_SLAVE_RATIO_0         (DDR_PHY_CTRL_REGS + 0xDC)
#define DATA0_WR_DQS_SLAVE_RATIO_1         (DDR_PHY_CTRL_REGS + 0xE0)
#define DATA0_WRLVL_INIT_RATIO_0           (DDR_PHY_CTRL_REGS + 0xF0)
#define DATA0_WRLVL_INIT_RATIO_1           (DDR_PHY_CTRL_REGS + 0xF4)
#define DATA0_GATELVL_INIT_RATIO_0         (DDR_PHY_CTRL_REGS + 0xFC)
#define DATA0_GATELVL_INIT_RATIO_1         (DDR_PHY_CTRL_REGS + 0x100)
#define DATA0_FIFO_WE_SLAVE_RATIO_0        (DDR_PHY_CTRL_REGS + 0x108)
#define DATA0_FIFO_WE_SLAVE_RATIO_1        (DDR_PHY_CTRL_REGS + 0x10C)
#define DATA0_WR_DATA_SLAVE_RATIO_0        (DDR_PHY_CTRL_REGS + 0x120)
#define DATA0_WR_DATA_SLAVE_RATIO_1        (DDR_PHY_CTRL_REGS + 0x124)
#define DATA0_USE_RANK0_DELAYS_0           (DDR_PHY_CTRL_REGS + 0x134)
#define DATA0_LOCK_DIFF_0                  (DDR_PHY_CTRL_REGS + 0x138)
#define DATA1_RD_DQS_SLAVE_RATIO_0         (DDR_PHY_CTRL_REGS + 0x16c)
#define DATA1_RD_DQS_SLAVE_RATIO_1         (DDR_PHY_CTRL_REGS + 0x170)
#define DATA1_WR_DQS_SLAVE_RATIO_0         (DDR_PHY_CTRL_REGS + 0x180)
#define DATA1_WR_DQS_SLAVE_RATIO_1         (DDR_PHY_CTRL_REGS + 0x184)
#define DATA1_WRLVL_INIT_RATIO_0           (DDR_PHY_CTRL_REGS + 0x194)
#define DATA1_WRLVL_INIT_RATIO_1           (DDR_PHY_CTRL_REGS + 0x198)
#define DATA1_GATELVL_INIT_RATIO_0         (DDR_PHY_CTRL_REGS + 0x1a0)
#define DATA1_GATELVL_INIT_RATIO_1         (DDR_PHY_CTRL_REGS + 0x1a4)
#define DATA1_FIFO_WE_SLAVE_RATIO_0        (DDR_PHY_CTRL_REGS + 0x1ac)
#define DATA1_FIFO_WE_SLAVE_RATIO_1        (DDR_PHY_CTRL_REGS + 0x1b0)
#define DATA1_WR_DATA_SLAVE_RATIO_0        (DDR_PHY_CTRL_REGS + 0x1c4)
#define DATA1_WR_DATA_SLAVE_RATIO_1        (DDR_PHY_CTRL_REGS + 0x1c8)
#define DATA1_USE_RANK0_DELAYS_0           (DDR_PHY_CTRL_REGS + 0x1d8)
#define DATA1_LOCK_DIFF_0                  (DDR_PHY_CTRL_REGS + 0x1dc)


/* DDR3 init values */

#if  defined (evmAM335x)
#define DDR3_CMD0_SLAVE_RATIO_0            (0x80)
#define DDR3_CMD0_INVERT_CLKOUT_0          (0x0)
#define DDR3_CMD1_SLAVE_RATIO_0            (0x80)
#define DDR3_CMD1_INVERT_CLKOUT_0          (0x0)
#define DDR3_CMD2_SLAVE_RATIO_0            (0x80)
#define DDR3_CMD2_INVERT_CLKOUT_0          (0x0)

#define DDR3_DATA0_RD_DQS_SLAVE_RATIO_0    (0x3B)
#define DDR3_DATA0_WR_DQS_SLAVE_RATIO_0    (0x3C)
#define DDR3_DATA0_FIFO_WE_SLAVE_RATIO_0   (0xA5)
#define DDR3_DATA0_WR_DATA_SLAVE_RATIO_0   (0x74)

#define DDR3_DATA0_RD_DQS_SLAVE_RATIO_1    (0x3B)
#define DDR3_DATA0_WR_DQS_SLAVE_RATIO_1    (0x3C)
#define DDR3_DATA0_FIFO_WE_SLAVE_RATIO_1   (0xA5)
#define DDR3_DATA0_WR_DATA_SLAVE_RATIO_1   (0x74)

#define DDR3_CONTROL_DDR_CMD_IOCTRL_0      (0x18B)
#define DDR3_CONTROL_DDR_CMD_IOCTRL_1      (0x18B)
#define DDR3_CONTROL_DDR_CMD_IOCTRL_2      (0x18B)

#define DDR3_CONTROL_DDR_DATA_IOCTRL_0      (0x18B)
#define DDR3_CONTROL_DDR_DATA_IOCTRL_1      (0x18B)

#define DDR3_CONTROL_DDR_IO_CTRL           (0xefffffff)

#define DDR3_EMIF_DDR_PHY_CTRL_1           (0x06)
#define DDR3_EMIF_DDR_PHY_CTRL_1_DY_PWRDN         (0x00100000)
#define DDR3_EMIF_DDR_PHY_CTRL_1_SHDW      (0x06)
#define DDR3_EMIF_DDR_PHY_CTRL_1_SHDW_DY_PWRDN    (0x00100000)
#define DDR3_EMIF_DDR_PHY_CTRL_2           (0x06)

#define DDR3_EMIF_SDRAM_TIM_1              (0x0888A39B)
#define DDR3_EMIF_SDRAM_TIM_1_SHDW         (0x0888A39B)

#define DDR3_EMIF_SDRAM_TIM_2              (0x26517FDA)
#define DDR3_EMIF_SDRAM_TIM_2_SHDW         (0x26517FDA)

#define DDR3_EMIF_SDRAM_TIM_3              (0x501F84EF)
#define DDR3_EMIF_SDRAM_TIM_3_SHDM         (0x501F84EF)

#define DDR3_EMIF_SDRAM_REF_CTRL_VAL1      (0x0000093B)
#define DDR3_EMIF_SDRAM_REF_CTRL_SHDW_VAL1 (0x0000093B)

#define DDR3_EMIF_ZQ_CONFIG_VAL            (0x50074BE4)

/*
** termination = 1 (RZQ/4)
** dynamic ODT = 2 (RZQ/2)
** SDRAM drive = 0 (RZQ/6)
** CWL = 0 (CAS write latency = 5)
** CL = 2 (CAS latency = 5)
** ROWSIZE = 7 (16 row bits)
** PAGESIZE = 2 (10 column bits)
*/
#define DDR3_EMIF_SDRAM_CONFIG             (0x61C04BB2)

#elif defined (beaglebone)

#define DDR3_CMD0_SLAVE_RATIO_0            (0x80)
#define DDR3_CMD0_INVERT_CLKOUT_0          (0x0)
#define DDR3_CMD1_SLAVE_RATIO_0            (0x80)
#define DDR3_CMD1_INVERT_CLKOUT_0          (0x0)
#define DDR3_CMD2_SLAVE_RATIO_0            (0x80)
#define DDR3_CMD2_INVERT_CLKOUT_0          (0x0)

#define DDR3_DATA0_RD_DQS_SLAVE_RATIO_0    (0x38)
#define DDR3_DATA0_WR_DQS_SLAVE_RATIO_0    (0x44)
#define DDR3_DATA0_FIFO_WE_SLAVE_RATIO_0   (0x94)
#define DDR3_DATA0_WR_DATA_SLAVE_RATIO_0   (0x7d)

#define DDR3_DATA0_RD_DQS_SLAVE_RATIO_1    (0x38)
#define DDR3_DATA0_WR_DQS_SLAVE_RATIO_1    (0x44)
#define DDR3_DATA0_FIFO_WE_SLAVE_RATIO_1   (0x94)
#define DDR3_DATA0_WR_DATA_SLAVE_RATIO_1   (0x7d)

#define DDR3_CONTROL_DDR_CMD_IOCTRL_0      (0x18B)
#define DDR3_CONTROL_DDR_CMD_IOCTRL_1      (0x18B)
#define DDR3_CONTROL_DDR_CMD_IOCTRL_2      (0x18B)

#define DDR3_CONTROL_DDR_DATA_IOCTRL_0      (0x18B)
#define DDR3_CONTROL_DDR_DATA_IOCTRL_1      (0x18B)

#define DDR3_CONTROL_DDR_IO_CTRL           (0xefffffff)

#define DDR3_EMIF_DDR_PHY_CTRL_1           (0x07)
#define DDR3_EMIF_DDR_PHY_CTRL_1_DY_PWRDN         (0x00100000)
#define DDR3_EMIF_DDR_PHY_CTRL_1_SHDW      (0x07)
#define DDR3_EMIF_DDR_PHY_CTRL_1_SHDW_DY_PWRDN    (0x00100000)
#define DDR3_EMIF_DDR_PHY_CTRL_2           (0x07)

#define DDR3_EMIF_SDRAM_TIM_1              (0x0AAAD4DB)
#define DDR3_EMIF_SDRAM_TIM_1_SHDW         (0x0AAAD4DB)

#define DDR3_EMIF_SDRAM_TIM_2              (0x266B7FDA)
#define DDR3_EMIF_SDRAM_TIM_2_SHDW         (0x266B7FDA)

#define DDR3_EMIF_SDRAM_TIM_3              (0x501F867F)
#define DDR3_EMIF_SDRAM_TIM_3_SHDM         (0x501F867F)

#define DDR3_EMIF_SDRAM_REF_CTRL_VAL1      (0x00000C30)
#define DDR3_EMIF_SDRAM_REF_CTRL_SHDW_VAL1 (0x00000C30)

#define DDR3_EMIF_ZQ_CONFIG_VAL            (0x50074BE4)

/*
** termination = 1 (RZQ/4)
** dynamic ODT = 2 (RZQ/2)
** SDRAM drive = 0 (RZQ/6)
** CWL = 0 (CAS write latency = 5)
** CL = 2 (CAS latency = 5)
** ROWSIZE = 7 (16 row bits)
** PAGESIZE = 2 (10 column bits)
*/
#define DDR3_EMIF_SDRAM_CONFIG             (0x61C05332)

#else

#define DDR3_CMD0_SLAVE_RATIO_0            (0x40)
#define DDR3_CMD0_INVERT_CLKOUT_0          (0x1)
#define DDR3_CMD1_SLAVE_RATIO_0            (0x40)
#define DDR3_CMD1_INVERT_CLKOUT_0          (0x1)
#define DDR3_CMD2_SLAVE_RATIO_0            (0x40)
#define DDR3_CMD2_INVERT_CLKOUT_0          (0x1)

#define DDR3_DATA0_RD_DQS_SLAVE_RATIO_0    (0x3B)
#define DDR3_DATA0_WR_DQS_SLAVE_RATIO_0    (0x85)
#define DDR3_DATA0_FIFO_WE_SLAVE_RATIO_0   (0x100)
#define DDR3_DATA0_WR_DATA_SLAVE_RATIO_0   (0xC1)

#define DDR3_DATA0_RD_DQS_SLAVE_RATIO_1    (0x3B)
#define DDR3_DATA0_WR_DQS_SLAVE_RATIO_1    (0x85)
#define DDR3_DATA0_FIFO_WE_SLAVE_RATIO_1   (0x100)
#define DDR3_DATA0_WR_DATA_SLAVE_RATIO_1   (0xC1)

#define DDR3_CONTROL_DDR_CMD_IOCTRL_0      (0x18B)
#define DDR3_CONTROL_DDR_CMD_IOCTRL_1      (0x18B)
#define DDR3_CONTROL_DDR_CMD_IOCTRL_2      (0x18B)

#define DDR3_CONTROL_DDR_DATA_IOCTRL_0      (0x18B)
#define DDR3_CONTROL_DDR_DATA_IOCTRL_1      (0x18B)

//#define DDR3_CONTROL_DDR_IO_CTRL           (0x0fffffff)
#define DDR3_CONTROL_DDR_IO_CTRL           (0xefffffff)

#define DDR3_EMIF_DDR_PHY_CTRL_1           (0x06)
#define DDR3_EMIF_DDR_PHY_CTRL_1_DY_PWRDN         (0x00100000)
#define DDR3_EMIF_DDR_PHY_CTRL_1_SHDW      (0x06)
#define DDR3_EMIF_DDR_PHY_CTRL_1_SHDW_DY_PWRDN    (0x00100000)
#define DDR3_EMIF_DDR_PHY_CTRL_2           (0x06)

#define DDR3_EMIF_SDRAM_TIM_1              (0x0888A39B)
#define DDR3_EMIF_SDRAM_TIM_1_SHDW         (0x0888A39B)

#define DDR3_EMIF_SDRAM_TIM_2              (0x26337FDA)
#define DDR3_EMIF_SDRAM_TIM_2_SHDW         (0x26337FDA)

#define DDR3_EMIF_SDRAM_TIM_3              (0x501F830F)
#define DDR3_EMIF_SDRAM_TIM_3_SHDM         (0x501F830F)

#define DDR3_EMIF_SDRAM_REF_CTRL_VAL1      (0x0000093B)
#define DDR3_EMIF_SDRAM_REF_CTRL_SHDW_VAL1 (0x0000093B)

#define DDR3_EMIF_ZQ_CONFIG_VAL            (0x50074BE4)
#define DDR3_EMIF_SDRAM_CONFIG             (0x61C04AB2)//termination = 1 (RZQ/4)
                                                       //dynamic ODT = 2 (RZQ/2)
                                                       //SDRAM drive = 0 (RZQ/6)
                                                       //CWL = 0 (CAS write latency = 5)
                                                       //CL = 2 (CAS latency = 5)
                                                       //ROWSIZE = 5 (14 row bits)
                                                       //PAGESIZE = 2 (10 column bits)
													   
#endif


/******************************************************************************
**                     STATIC function Declarations
*******************************************************************************/
//void CorePLLInit(void)
//void DisplayPLLInit(void);
//void PerPLLInit(void)
//void DDRPLLInit(unsigned int freqMult);
//void MPUPLLInit(unsigned int freqMult)
//void InterfaceClkInit(void)











/******************************************************************************
**                     Local function Declarations
*******************************************************************************/

//static void DDRVTTEnable(void);
//extern void SPIConfigure(void);
extern void I2C1ModuleClkConfig(void);


/******************************************************************************
**                     Global variable Definitions
*******************************************************************************/
char *deviceType = "AM335x";
volatile unsigned char dataFromSlave[2];
volatile unsigned char dataToSlave[3];
volatile unsigned int tCount;
//volatile unsigned int rCount;    //skd_patch_hlam v reistre u nas poluchaetsa popravil
static unsigned int rCount=0;

//static unsigned int oppMaxIdx;
static unsigned int deviceVersion;
//static unsigned int freqMultDDR;


unsigned char isBBB = 0;

/*
** OPP table for mpu multiplier and pmic voltage select.
** MPUPLL_N and MPUPLL_M2 are divider and post divider values.
*/
tOPPConfig oppTable[] =
{
    {MPUPLL_M_275_MHZ, PMIC_VOLT_SEL_1100MV},  /* OPP100 275Mhz - 1.1v */
    {MPUPLL_M_500_MHZ, PMIC_VOLT_SEL_1100MV},  /* OPP100 500Mhz - 1.1v */
    {MPUPLL_M_600_MHZ, PMIC_VOLT_SEL_1200MV},  /* OPP120 600Mhz - 1.2v */
    {MPUPLL_M_720_MHZ, PMIC_VOLT_SEL_1260MV},  /* OPP TURBO 720Mhz - 1.26v */
    {MPUPLL_M_300_MHZ, PMIC_VOLT_SEL_0950MV},  /* OPP50 300Mhz - 950mv */
    {MPUPLL_M_300_MHZ, PMIC_VOLT_SEL_1100MV},  /* OPP100 300Mhz - 1.1v */
    {MPUPLL_M_600_MHZ, PMIC_VOLT_SEL_1100MV},  /* OPP100 600Mhz - 1.1v */
    {MPUPLL_M_720_MHZ, PMIC_VOLT_SEL_1200MV},  /* OPP120 720Mhz - 1.2v */
    {MPUPLL_M_800_MHZ, PMIC_VOLT_SEL_1260MV},  /* OPP TURBO 800Mhz - 1.26v */
    {MPUPLL_M_1000_MHZ, PMIC_VOLT_SEL_1325MV}  /* OPP NITRO 1000Mhz - 1.325v */
};

#if defined(NAND)
  static GPMCNANDTimingInfo_t nandTimingInfo;
#endif


/******************************************************************************
**                     Function Definitions
*******************************************************************************/

/*
** Determine maximum OPP configuration of SoC.
*/
unsigned int BootMaxOppGet(void)
{
    unsigned int oppIdx;
    unsigned int oppSupport = SysConfigOppDataGet();
	


    if(DEVICE_VERSION_1_0 == deviceVersion) 		/*frequency setting related to device version 1.0*/
    {

    	oppIdx = EFUSE_OPPTB_720;

    }
    else if((DEVICE_VERSION_2_0 == deviceVersion) || (DEVICE_VERSION_2_1 == deviceVersion)) /*frequency setting related to device version 2.x*/
    {
        if(oppSupport == 0)							/*possible for experimental devices whose part number starting with X*/
        {
            oppIdx = EFUSE_OPPTB_800;

        }
        else if(!(oppSupport & EFUSE_OPPNT_1000_MASK))
        {
            oppIdx = EFUSE_OPPNT_1000;
          ;
        }
        else if(!(oppSupport & EFUSE_OPPTB_800_MASK))
        {
            oppIdx = EFUSE_OPPTB_800;

        }
        else if(!(oppSupport & EFUSE_OPP120_720_MASK))
        {
            oppIdx = EFUSE_OPP120_720;

        }
        else if(!(oppSupport & EFUSE_OPP100_600_MASK))
        {
            oppIdx = EFUSE_OPP100_600;

        }
        else if(!(oppSupport & EFUSE_OPP100_300_MASK))
        {
            oppIdx = EFUSE_OPP100_300;

        }
        else										/*possible if fuses are not blown for experimental devices whose part number starting with X*/
        {
            oppIdx = EFUSE_OPP50_300;

        }
    }
    else
    {
        return OPP_NONE;

    }

    return oppIdx;
}

/**************************************************************************************************
Syntax:      	    void CorePLLInit(void)
Remarks:            ��� �����������:
					CORE_CLKOUTM4
					L3F_CLK, L4F_CLK
					PRU-ICSS IEP CLK,
					L4_PER, L4_WKUP
					SGX CORECLK
					CORE_CLKOUTM5
					CORE_CLKOUTM6
***************************************************************************************************/
void CorePLLInit(void)
{
    //volatile unsigned int regVal = 0;
	 static unsigned int regVal = 0;
    /* Enable the Core PLL */

    /* Put the PLL in bypass mode */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_CORE) &
                ~CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN_DPLL_MN_BYP_MODE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_CORE) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_CORE) & CM_WKUP_CM_IDLEST_DPLL_CORE_ST_MN_BYPASS));

    /* Set the multipler and divider values for the PLL */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_CORE) = ((COREPLL_M << CM_WKUP_CM_CLKSEL_DPLL_CORE_DPLL_MULT_SHIFT) |(COREPLL_N << CM_WKUP_CM_CLKSEL_DPLL_CORE_DPLL_DIV_SHIFT));

    /* Configure the High speed dividers */
    /* Set M4 divider */    
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M4_DPLL_CORE);
    regVal = regVal & ~CM_WKUP_CM_DIV_M4_DPLL_CORE_HSDIVIDER_CLKOUT1_DIV;
    regVal = regVal | (COREPLL_HSD_M4 << CM_WKUP_CM_DIV_M4_DPLL_CORE_HSDIVIDER_CLKOUT1_DIV_SHIFT);
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M4_DPLL_CORE) = regVal;
    
    /* Set M5 divider */    
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M5_DPLL_CORE);
    regVal = regVal & ~CM_WKUP_CM_DIV_M5_DPLL_CORE_HSDIVIDER_CLKOUT2_DIV;
    regVal = regVal | (COREPLL_HSD_M5 << CM_WKUP_CM_DIV_M5_DPLL_CORE_HSDIVIDER_CLKOUT2_DIV_SHIFT);
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M5_DPLL_CORE) = regVal;        
        
    /* Set M6 divider */    
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M6_DPLL_CORE);
    regVal = regVal & ~CM_WKUP_CM_DIV_M6_DPLL_CORE_HSDIVIDER_CLKOUT3_DIV;
    regVal = regVal | (COREPLL_HSD_M6 << CM_WKUP_CM_DIV_M6_DPLL_CORE_HSDIVIDER_CLKOUT3_DIV_SHIFT);
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M6_DPLL_CORE) = regVal;         

    /* Now LOCK the PLL by enabling it */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_CORE) &
                ~CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_CORE_DPLL_EN;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_CORE) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_CORE) & CM_WKUP_CM_IDLEST_DPLL_CORE_ST_DPLL_CLK));
}

/* \brief This function initializes the DISPLAY PLL
 * 
 * \param none
 *
 * \return none
 *
 */
void DisplayPLLInit(void)
{
    //volatile unsigned int regVal = 0;
	static unsigned int regVal = 0;
    /* Put the PLL in bypass mode */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DISP) &
                ~CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN_DPLL_MN_BYP_MODE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DISP) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DISP) &
                        CM_WKUP_CM_IDLEST_DPLL_DISP_ST_MN_BYPASS));

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DISP) &=
                           ~(CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_DIV |
                             CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_MULT);

    /* Set the multipler and divider values for the PLL */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DISP) |=
        ((DISPLL_M << CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_MULT_SHIFT) |
         (DISPLL_N << CM_WKUP_CM_CLKSEL_DPLL_DISP_DPLL_DIV_SHIFT));

    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DISP);
    regVal = regVal & ~CM_WKUP_CM_DIV_M2_DPLL_DISP_DPLL_CLKOUT_DIV;
    regVal = regVal | DISPLL_M2;

    /* Set the CLKOUT2 divider */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DISP) = regVal;

    /* Now LOCK the PLL by enabling it */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DISP) &
                ~CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_DISP_DPLL_EN;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DISP) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DISP) &
                         CM_WKUP_CM_IDLEST_DPLL_DISP_ST_DPLL_CLK));
}


/**************************************************************************************************
Syntax:      	    void PerPLLInit(void)
Remarks:            ��� �����������:
					USB_PHY_CLK
					PER_CLKOUTM2
					MMC_CLK
					SPI_CLK,
					UART_CLK,
					I2C_CLK
					CLK_24
					CLK_32KHZ
***************************************************************************************************/
void PerPLLInit(void)
{
    //volatile unsigned int regVal = 0;
	static unsigned int regVal = 0;
    /* Put the PLL in bypass mode */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_PER) &
                ~CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN_DPLL_MN_BYP_MODE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_PER) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_PER) &
                      CM_WKUP_CM_IDLEST_DPLL_PER_ST_MN_BYPASS));

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_PERIPH) &=
                             ~(CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_MULT |
                                    CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_DIV);

    /* Set the multipler and divider values for the PLL */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_PERIPH) |=
        ((PERPLL_M << CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_MULT_SHIFT) |
         (PERPLL_N << CM_WKUP_CM_CLKSEL_DPLL_PERIPH_DPLL_DIV_SHIFT));

    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_PER);
    regVal = regVal & ~CM_WKUP_CM_DIV_M2_DPLL_PER_DPLL_CLKOUT_DIV;
    regVal = regVal | PERPLL_M2;

    /* Set the CLKOUT2 divider */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_PER) = regVal;
    
    /* Now LOCK the PLL by enabling it */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_PER) &
                ~CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_PER_DPLL_EN;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_PER) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_PER) &
                           CM_WKUP_CM_IDLEST_DPLL_PER_ST_DPLL_CLK));

}

/**************************************************************************************************
Syntax:      	    void DDRPLLInit(unsigned int freqMult)
Remarks:            ��� �����������:

***************************************************************************************************/
void DDRPLLInit(unsigned int freqMult)
{
    //volatile unsigned int regVal = 0;
	static unsigned int regVal = 0;

    /* Put the PLL in bypass mode */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) & ~CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN_DPLL_MN_BYP_MODE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DDR) & CM_WKUP_CM_IDLEST_DPLL_DDR_ST_MN_BYPASS));

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DDR) &= ~(CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_MULT | CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_DIV);

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_DDR) |= ((freqMult << CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_MULT_SHIFT) | (DDRPLL_N << CM_WKUP_CM_CLKSEL_DPLL_DDR_DPLL_DIV_SHIFT));

    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DDR);
    regVal = regVal & ~CM_WKUP_CM_DIV_M2_DPLL_DDR_DPLL_CLKOUT_DIV;
    regVal = regVal | DDRPLL_M2;

    /* Set the CLKOUT2 divider */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_DDR) = regVal;

    /* Now LOCK the PLL by enabling it */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) & ~CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_DDR_DPLL_EN;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_DDR) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_DDR) & CM_WKUP_CM_IDLEST_DPLL_DDR_ST_DPLL_CLK));
}


/**************************************************************************************************
Syntax:      	    void MPUPLLInit(unsigned int freqMult)
Remarks:            ��� �����������:
                    ARM (ARM_FCLK)-Cortex A8 Core Functional Clock:MPU_CLK
                    AXI2OCP Clock (AXI_FCLK):
                    Interrupt Controller Functional Clock (MPU_INTC_FCLK):
                    ICE-Crusher Functional Clock (ICECRUSHER_FCLK):
                    I2Async Clock (I2ASYNC_FCLK):
                    It matches the OCP interface of the AXI2OCP bridge.

***************************************************************************************************/
void MPUPLLInit(unsigned int freqMult)
{
    //volatile unsigned int regVal = 0;
	static unsigned int regVal = 0;
    /* Put the PLL in bypass mode */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_MPU) & ~CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN_DPLL_MN_BYP_MODE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_MPU) = regVal;

    /* Wait for DPLL to go in to bypass mode */
    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_MPU) & CM_WKUP_CM_IDLEST_DPLL_MPU_ST_MN_BYPASS));

    /* Clear the MULT and DIV field of DPLL_MPU register */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_MPU) &=~(CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_MULT | CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_DIV);

    /* Set the multiplier and divider values for the PLL */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKSEL_DPLL_MPU) |= ((freqMult << CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_MULT_SHIFT) | (MPUPLL_N << CM_WKUP_CM_CLKSEL_DPLL_MPU_DPLL_DIV_SHIFT));

    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_MPU);

    regVal = regVal & ~CM_WKUP_CM_DIV_M2_DPLL_MPU_DPLL_CLKOUT_DIV;

    regVal = regVal | MPUPLL_M2;

    /* Set the CLKOUT2 divider */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_DIV_M2_DPLL_MPU) = regVal;

    /* Now LOCK the PLL by enabling it */
    regVal = HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_MPU) &  ~CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN;

    regVal |= CM_WKUP_CM_CLKMODE_DPLL_MPU_DPLL_EN;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_CLKMODE_DPLL_MPU) = regVal;

    while(!(HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CM_IDLEST_DPLL_MPU) & CM_WKUP_CM_IDLEST_DPLL_MPU_ST_DPLL_CLK));
}



/* \brief This function initializes the interface clock 
 * 
 * \param none
 *
 * \return none
 *
 */
void InterfaceClkInit(void)
{
    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) |=
                                   CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) &
        CM_PER_L3_CLKCTRL_MODULEMODE) != CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) |=
                                       CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) &
      CM_PER_L4LS_CLKCTRL_MODULEMODE) != CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4FW_CLKCTRL) |=
                                 CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4FW_CLKCTRL) &
      CM_PER_L4FW_CLKCTRL_MODULEMODE) != CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_L4WKUP_CLKCTRL) |=
                                          CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE;
    while((HWREG(SOC_CM_WKUP_REGS + CM_WKUP_L4WKUP_CLKCTRL) &
                        CM_WKUP_L4WKUP_CLKCTRL_MODULEMODE) !=
                                         CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) |=
                                      CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) &
                        CM_PER_L3_INSTR_CLKCTRL_MODULEMODE) !=
                        CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4HS_CLKCTRL) |=
                              CM_PER_L4HS_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4HS_CLKCTRL) &
                        CM_PER_L4HS_CLKCTRL_MODULEMODE) !=
                  CM_PER_L4HS_CLKCTRL_MODULEMODE_ENABLE);
}

/* \brief This function initializes the power domain transition.
 * 
 * \8.1.12 Clock Module Registers  p.1161 clock module forced
 *
 * \return none
 *
 */
void PowerDomainTransitionInit(void)
{
    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) |= CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

     HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) |=CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CLKSTCTRL) |= CM_WKUP_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    HWREG(SOC_CM_PER_REGS + CM_PER_L4FW_CLKSTCTRL) |= CM_PER_L4FW_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) |= CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

}

/*
Table 5-8. Valid Combinations of VDD_CORE and
VDD_MPU OPPs for ZCZ Package With Device        am3358 -> p.84  ������� �����������.
Revision Code "A" or Newer
 */
void PLLInit(void)
{
    //������������� ������� MPU  Microprocessor Unit (MPU) Subsystem  ��� ���������� 1.100[V]  = 600 ���
	MPUPLLInit(MPUPLL_M_600_MHZ);
	//MPUPLLInit(oppTable[oppMaxIdx].pllMult);

    //������������� ������� ���� ������� ��������� � ����������
    CorePLLInit();

    //������������� ������� ����� ������������ ������
    PerPLLInit();
    //������������� ������� DDR3 ��� ���������� , 1.500[V]  = 400 ���
	DDRPLLInit(DDRPLL_M_DDR3);
	//DDRPLLInit(freqMultDDR);


	InterfaceClkInit();
    PowerDomainTransitionInit();
    //DisplayPLLInit();                //skd_patch_no_display_pll

    arm3358_beagleboard.arm_freq_mhz=MPUPLL_M_600_MHZ;
	arm3358_beagleboard.ddr_freg_mhz=DDRPLL_M_DDR3;



}

/*
 * \brief Configure I2C0 on which the PMIC is interfaced
 *
 * \param  none
 *
 * \return none
 */
void SetupI2C(void)
{
    I2C0ModuleClkConfig();

    I2CPinMuxSetup(I2C_0);

    /* Put i2c in reset/disabled state */
    I2CMasterDisable(SOC_I2C_0_REGS);

    I2CSoftReset(SOC_I2C_0_REGS);

    /* Disable auto Idle functionality */
    I2CAutoIdleDisable(SOC_I2C_0_REGS);

    /* Configure i2c bus speed to 100khz */
    I2CMasterInitExpClk(SOC_I2C_0_REGS, I2C_SYSTEM_CLOCK, I2C_INTERNAL_CLOCK,I2C_OUTPUT_CLOCK);
    I2CMasterEnable(SOC_I2C_0_REGS);

    while(!I2CSystemStatusGet(SOC_I2C_0_REGS));
}

/*
 * \brief Clear the status of all interrupts 
 *
 * \param  none.
 *
 * \return none
 */

void CleanupInterrupts(void)
{
    I2CMasterIntClearEx(SOC_I2C_0_REGS,  I2C_INTERRUPT_FLAG_TO_CLR);
}

/*
 * \brief Transmits data over I2C0 bus 
 *
 * \param  none
 *
 * \return none
 */
void SetupI2CTransmit(unsigned int dcount)
{
    I2CSetDataCount(SOC_I2C_0_REGS, dcount);

    CleanupInterrupts();

    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_TX);

    I2CMasterStart(SOC_I2C_0_REGS);

    while(I2CMasterBusBusy(SOC_I2C_0_REGS) == 0);

    while((I2C_INT_TRANSMIT_READY == (I2CMasterIntRawStatus(SOC_I2C_0_REGS)
                                     & I2C_INT_TRANSMIT_READY)) && dcount--)
    {
        I2CMasterDataPut(SOC_I2C_0_REGS, dataToSlave[tCount++]);

        I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY);
    }

    I2CMasterStop(SOC_I2C_0_REGS);

    while(0 == (I2CMasterIntRawStatus(SOC_I2C_0_REGS) & I2C_INT_STOP_CONDITION));

    I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_STOP_CONDITION);

}

/*
 * \brief Receives data over I2C0 bus 
 *
 * \param  dcount - Number of bytes to receive.
 *
 * \return none
 */

void SetupReception(unsigned int dcount)
{
    I2CSetDataCount(SOC_I2C_0_REGS, 1);

    CleanupInterrupts();

    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_TX);

    I2CMasterStart(SOC_I2C_0_REGS);

    while(I2CMasterBusBusy(SOC_I2C_0_REGS) == 0);

    I2CMasterDataPut(SOC_I2C_0_REGS, dataToSlave[tCount]);

    I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY);

    while(0 == (I2CMasterIntRawStatus(SOC_I2C_0_REGS) & I2C_INT_ADRR_READY_ACESS));

    I2CSetDataCount(SOC_I2C_0_REGS, dcount);

    CleanupInterrupts();

    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_RX);

    I2CMasterStart(SOC_I2C_0_REGS);

    /* Wait till the bus if free */
    while(I2CMasterBusBusy(SOC_I2C_0_REGS) == 0);

    /* Read the data from slave of dcount */
    while((dcount--))
    {
        while(0 == (I2CMasterIntRawStatus(SOC_I2C_0_REGS) & I2C_INT_RECV_READY));

        dataFromSlave[rCount++] = I2CMasterDataGet(SOC_I2C_0_REGS);

        I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_RECV_READY);
    }

    I2CMasterStop(SOC_I2C_0_REGS);

    while(0 == (I2CMasterIntRawStatus(SOC_I2C_0_REGS) & I2C_INT_STOP_CONDITION));

    I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_STOP_CONDITION);
}

/*
 * \brief Generic function that can read a TPS65217 register
 *
 * \param regOffset -  Source register address
 *
 * \return dest     -  Place holder for read bytes.
 */
//#ifdef beaglebone

void TPS65217RegRead(unsigned char regOffset, unsigned char* dest)
{
    dataToSlave[0] = regOffset;
    tCount = 0;

    SetupReception(1);

    *dest = dataFromSlave[0];
}

/**
 *  \brief            - Generic function that can write a TPS65217 PMIC
 *                      register or bit field regardless of protection
 *                      level.
 *
 * \param prot_level  - Register password protection.
 *                      use PROT_LEVEL_NONE, PROT_LEVEL_1, or PROT_LEVEL_2
 * \param regOffset:  - Register address to write.
 *
 * \param dest_val    - Value to write.
 *
 * \param mask        - Bit mask (8 bits) to be applied.  Function will only
 *                      change bits that are set in the bit mask.
 *
 * \return:            None.
 */
void TPS65217RegWrite(unsigned char port_level, unsigned char regOffset,unsigned char dest_val, unsigned char mask)
{
    unsigned char read_val;
    unsigned xor_reg;

    dataToSlave[0] = regOffset;
    tCount = 0;
    rCount = 0;

    if(mask != MASK_ALL_BITS)
    {
         SetupReception(1);

         read_val = dataFromSlave[0];
         read_val &= (~mask);
         read_val |= (dest_val & mask);
         dest_val = read_val;
    }

    if(port_level > 0)
    {
         xor_reg = regOffset ^ PASSWORD_UNLOCK;

         dataToSlave[0] = PASSWORD;
         dataToSlave[1] = xor_reg;
         tCount = 0;

         SetupI2CTransmit(2);
    }

    dataToSlave[0] = regOffset;
    dataToSlave[1] = dest_val;
    tCount = 0;

    SetupI2CTransmit(2);

    if(port_level == PROT_LEVEL_2)
    {
         dataToSlave[0] = PASSWORD;
         dataToSlave[1] = xor_reg;
         tCount = 0;

         SetupI2CTransmit(2);

         dataToSlave[0] = regOffset;
         dataToSlave[1] = dest_val;
         tCount = 0;

         SetupI2CTransmit(2);
    }
}

/**
 *  \brief              - Controls output voltage setting for the DCDC1,
 *                        DCDC2, or DCDC3 control registers in the PMIC.
 *
 * \param  dc_cntrl_reg   DCDC Control Register address.
 *                        Must be DEFDCDC1, DEFDCDC2, or DEFDCDC3.
 *
 * \param  volt_sel       Register value to set.  See PMIC TRM for value set.
 *
 * \return:               None.
 */
void TPS65217VoltageUpdate(unsigned char dc_cntrl_reg, unsigned char volt_sel)
{
    /* set voltage level */
    TPS65217RegWrite(PROT_LEVEL_2, dc_cntrl_reg, volt_sel, MASK_ALL_BITS);

    /* set GO bit to initiate voltage transition */
    TPS65217RegWrite(PROT_LEVEL_2, DEFSLEW, DCDC_GO, DCDC_GO);
}



/**
 *  \brief set VDD1_OP voltage value.
 *
 * \param  opVolSelector  - VDD2_OP voltage value.
 *
 * \return None.
 */
void SetVdd1OpVoltage(unsigned int opVolSelector)
{
    /* Set DCDC2 (MPU) voltage */
    TPS65217VoltageUpdate(DEFDCDC2, opVolSelector);

}

/*
 * \brief Configures the VDD OP voltage. 
 *
 * \param  none.
 *
 * \return none
 */

void ConfigVddOpVoltage(void)
{
    SetupI2C();

#ifdef beaglebone

    unsigned char pmic_status = 0;

    /* Configure PMIC slave address */
    I2CMasterSlaveAddrSet(SOC_I2C_0_REGS, PMIC_TPS65217_I2C_SLAVE_ADDR);

    TPS65217RegRead(STATUS, &pmic_status);
    /* Increase USB current limit to 1300mA */
    TPS65217RegWrite(PROT_LEVEL_NONE, POWER_PATH, USB_INPUT_CUR_LIMIT_1300MA,USB_INPUT_CUR_LIMIT_MASK);

	/* Set DCDC1 (DDR3) voltage to 1.5V */
    if(isBBB)
    {
        TPS65217VoltageUpdate(DEFDCDC1, DCDC_VOLT_SEL_1500MV);
    }


    // Set DCDC2 (MPU) voltage to 1.275V
    //skd patch _set default voltage 1.100v
    //TPS65217VoltageUpdate(DEFDCDC2, DCDC_VOLT_SEL_1275MV);
      TPS65217VoltageUpdate(DEFDCDC2, DCDC_VOLT_SEL_1100MV);


      arm3358_beagleboard.vdd_core_volt = DCDC_VOLT_SEL_1100MV;
	  arm3358_beagleboard.vdd_mpu_volt  = DCDC_VOLT_SEL_1100MV;
	  arm3358_beagleboard.vdd_ddr_volt  = DCDC_VOLT_SEL_1500MV;



    /* Set LDO3, LDO4 output voltages according to the board */
	if(isBBB)
	{
		TPS65217RegWrite(PROT_LEVEL_2, DEFLS1, LDO_VOLTAGE_OUT_1_8, LDO_MASK);
	}
	else
	{
		TPS65217RegWrite(PROT_LEVEL_2, DEFLS1, LDO_VOLTAGE_OUT_3_3, LDO_MASK);
	}

    TPS65217RegWrite(PROT_LEVEL_2, DEFLS2, LDO_VOLTAGE_OUT_3_3, LDO_MASK);

#elif  defined (evmAM335x) || defined (evmskAM335x)

    /* Configure PMIC slave address */
    I2CMasterSlaveAddrSet(SOC_I2C_0_REGS, PMIC_CNTL_I2C_SLAVE_ADDR);

	/* Select SR I2C(0) */
    SelectI2CInstance(PMIC_DEVCTRL_REG_SR_CTL_I2C_SEL_CTL_I2C);

    /* Configure vdd1- need to validate these parameters */
    ConfigureVdd1(PMIC_VDD1_REG_VGAIN_SEL_X1, PMIC_VDD1_REG_ILMAX_1_5_A,
	              PMIC_VDD1_REG_TSTEP_12_5, PMIC_VDD1_REG_ST_ON_HI_POW);

    /* Select the source for VDD1 control */
    SelectVdd1Source(PMIC_VDD1_OP_REG_CMD_OP);

#else

    #error Unsupported EVM !!

#endif
}



/*
 * \brief This function sets up the DDR PHY
 *
 * \param  none
 *
 * \return none
 */
static void DDR3PhyInit(void)
{
    /* Enable VTP */
    HWREG(SOC_CONTROL_REGS + CONTROL_VTP_CTRL) |= CONTROL_VTP_CTRL_ENABLE;
    HWREG(SOC_CONTROL_REGS + CONTROL_VTP_CTRL) &= ~CONTROL_VTP_CTRL_CLRZ;
    HWREG(SOC_CONTROL_REGS + CONTROL_VTP_CTRL) |= CONTROL_VTP_CTRL_CLRZ;
    while((HWREG(SOC_CONTROL_REGS + CONTROL_VTP_CTRL) & CONTROL_VTP_CTRL_READY) !=
                CONTROL_VTP_CTRL_READY);

    /* DDR PHY CMD0 Register configuration */
    HWREG(CMD0_SLAVE_RATIO_0)   = DDR3_CMD0_SLAVE_RATIO_0;
    HWREG(CMD0_INVERT_CLKOUT_0) = DDR3_CMD0_INVERT_CLKOUT_0;

    /* DDR PHY CMD1 Register configuration */
    HWREG(CMD1_SLAVE_RATIO_0)   = DDR3_CMD1_SLAVE_RATIO_0;
    HWREG(CMD1_INVERT_CLKOUT_0) = DDR3_CMD1_INVERT_CLKOUT_0;

    /* DDR PHY CMD2 Register configuration */
    HWREG(CMD2_SLAVE_RATIO_0)   = DDR3_CMD2_SLAVE_RATIO_0;
    HWREG(CMD2_INVERT_CLKOUT_0) = DDR3_CMD2_INVERT_CLKOUT_0;

    /* DATA macro configuration */
    HWREG(DATA0_RD_DQS_SLAVE_RATIO_0)  = DDR3_DATA0_RD_DQS_SLAVE_RATIO_0;
    HWREG(DATA0_WR_DQS_SLAVE_RATIO_0)  = DDR3_DATA0_WR_DQS_SLAVE_RATIO_0;
    HWREG(DATA0_FIFO_WE_SLAVE_RATIO_0) = DDR3_DATA0_FIFO_WE_SLAVE_RATIO_0;
    HWREG(DATA0_WR_DATA_SLAVE_RATIO_0) = DDR3_DATA0_WR_DATA_SLAVE_RATIO_0;
    HWREG(DATA1_RD_DQS_SLAVE_RATIO_0)  = DDR3_DATA0_RD_DQS_SLAVE_RATIO_1;
    HWREG(DATA1_WR_DQS_SLAVE_RATIO_0)  = DDR3_DATA0_WR_DQS_SLAVE_RATIO_1;
    HWREG(DATA1_FIFO_WE_SLAVE_RATIO_0) = DDR3_DATA0_FIFO_WE_SLAVE_RATIO_1;
    HWREG(DATA1_WR_DATA_SLAVE_RATIO_0) = DDR3_DATA0_WR_DATA_SLAVE_RATIO_1;

}


/* \brief This function initializes the DDR2
 * 
 * \param none
 *
 * \return none
 *
 */
void DDR3Init(void)
{
    /* DDR3 Phy Initialization */
    DDR3PhyInit();

    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_CMD_IOCTRL(0)) =
                                                 DDR3_CONTROL_DDR_CMD_IOCTRL_0;
    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_CMD_IOCTRL(1)) =
                                                 DDR3_CONTROL_DDR_CMD_IOCTRL_1;
    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_CMD_IOCTRL(2)) =
                                                 DDR3_CONTROL_DDR_CMD_IOCTRL_2;
    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_DATA_IOCTRL(0)) =
                                                 DDR3_CONTROL_DDR_DATA_IOCTRL_0;
    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_DATA_IOCTRL(1)) =
                                                 DDR3_CONTROL_DDR_DATA_IOCTRL_1;

    /* IO to work for DDR3 */
    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_IO_CTRL) &= DDR3_CONTROL_DDR_IO_CTRL;

    HWREG(SOC_CONTROL_REGS + CONTROL_DDR_CKE_CTRL) |= CONTROL_DDR_CKE_CTRL_DDR_CKE_CTRL;

    HWREG(SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1) = DDR3_EMIF_DDR_PHY_CTRL_1;

    /* Dynamic Power Down */
    if((DEVICE_VERSION_2_0 == deviceVersion) || (DEVICE_VERSION_2_1 == deviceVersion))
    {
        HWREG(SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1) |= DDR3_EMIF_DDR_PHY_CTRL_1_DY_PWRDN;
    }

    HWREG(SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1_SHDW) = DDR3_EMIF_DDR_PHY_CTRL_1_SHDW;

    /* Dynamic Power Down */
    if((DEVICE_VERSION_2_0 == deviceVersion) || (DEVICE_VERSION_2_1 == deviceVersion))
    {
        HWREG(SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_1_SHDW) |= DDR3_EMIF_DDR_PHY_CTRL_1_SHDW_DY_PWRDN;
    }

    HWREG(SOC_EMIF_0_REGS + EMIF_DDR_PHY_CTRL_2) = DDR3_EMIF_DDR_PHY_CTRL_2;

    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_1)      = DDR3_EMIF_SDRAM_TIM_1;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_1_SHDW) = DDR3_EMIF_SDRAM_TIM_1_SHDW;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_2)      = DDR3_EMIF_SDRAM_TIM_2;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_2_SHDW) = DDR3_EMIF_SDRAM_TIM_2_SHDW;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_3)      = DDR3_EMIF_SDRAM_TIM_3;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_TIM_3_SHDW) = DDR3_EMIF_SDRAM_TIM_3_SHDM;

    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_REF_CTRL)   = DDR3_EMIF_SDRAM_REF_CTRL_VAL1;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_REF_CTRL_SHDW) =
                                                 DDR3_EMIF_SDRAM_REF_CTRL_SHDW_VAL1;

    HWREG(SOC_EMIF_0_REGS + EMIF_ZQ_CONFIG)     = DDR3_EMIF_ZQ_CONFIG_VAL;
    HWREG(SOC_EMIF_0_REGS + EMIF_SDRAM_CONFIG)     = DDR3_EMIF_SDRAM_CONFIG;
	
    /* The CONTROL_SECURE_EMIF_SDRAM_CONFIG register exports SDRAM configuration 
       information to the EMIF */
    HWREG(SOC_CONTROL_REGS + CONTROL_SECURE_EMIF_SDRAM_CONFIG) = DDR3_EMIF_SDRAM_CONFIG;

}





/* \brief This function initializes the EMIF
 * 
 * \param none
 *
 * \return none
 *
 */
void EMIFInit(void)
{
    //volatile unsigned int regVal;
	static unsigned int regVal;
    /* Enable the clocks for EMIF */
    regVal = HWREG(SOC_CM_PER_REGS + CM_PER_EMIF_FW_CLKCTRL) & ~(CM_PER_EMIF_FW_CLKCTRL_MODULEMODE);

    regVal |= CM_PER_EMIF_FW_CLKCTRL_MODULEMODE_ENABLE;

    HWREG(SOC_CM_PER_REGS + CM_PER_EMIF_FW_CLKCTRL) = regVal;

    regVal = HWREG(SOC_CM_PER_REGS + CM_PER_EMIF_CLKCTRL) & ~(CM_PER_EMIF_CLKCTRL_MODULEMODE);

    regVal |= CM_PER_EMIF_CLKCTRL_MODULEMODE_ENABLE;

    HWREG(SOC_CM_PER_REGS + CM_PER_EMIF_CLKCTRL) = regVal;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
          (CM_PER_L3_CLKSTCTRL_CLKACTIVITY_EMIF_GCLK |
           CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK)) !=
          (CM_PER_L3_CLKSTCTRL_CLKACTIVITY_EMIF_GCLK |
           CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK));
}






#if defined(SPI)
/*
 * \brief This function is used to initialize and configure SPI Module.
 *
 * \param  none.
 *
 * \return none
*/
void BlPlatformSPISetup(void)
{
    unsigned int regVal;

    /* Enable clock for SPI */
    regVal = (HWREG(SOC_CM_PER_REGS + CM_PER_SPI0_CLKCTRL) &
                    ~(CM_PER_SPI0_CLKCTRL_MODULEMODE));

    regVal |= CM_PER_SPI0_CLKCTRL_MODULEMODE_ENABLE;

    HWREG(SOC_CM_PER_REGS + CM_PER_SPI0_CLKCTRL) = regVal;

    /* Setup SPI PINMUX */
    McSPIPinMuxSetup(0);
    McSPI0CSPinMuxSetup(0);

    SPIConfigure();
}
#endif

/*
 * \brief This function is used to initialize and configure UART Module.
 *
 * \param  none.
 *
 * \return none
*/

void UARTSetup(void)
{
    //volatile unsigned int regVal;
    static unsigned int regVal;

    /* Enable clock for UART0 */
    regVal = (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_UART0_CLKCTRL) & ~(CM_WKUP_UART0_CLKCTRL_MODULEMODE));

    regVal |= CM_WKUP_UART0_CLKCTRL_MODULEMODE_ENABLE;

    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_UART0_CLKCTRL) = regVal;

    UARTStdioInit();
}

/*
 * \brief This function Initializes Pll, DDR and Uart
 *
 * \param  none
 *
 * \return none
*/
struct parametr_of_sysytem BlPlatformConfig(void)
{

	//���� ��������� ����� �� EEPROM 4KB EEPROM
	BoardInfoInit();
	isBBB = TRUE;


    deviceVersion = DeviceVersionGet();



    //���������� ������� �������� ��� VDD_MPU �� 1.275 V ����� I2C ����  � TPS TPS65217C
    //VDDS_DDR  -> 1.500 [v]
    //VDD_CORE  -> 1.100 [v]   (OPP100)
    //LDO3,LDO4 ->
    //Increase USB to low 1300_ma
    //skd_patch set VDD_MPU �� 1.100 V
     ConfigVddOpVoltage();


     //���� ������������ ���� �������� � ������� ��������� � ����
#if 0
    //���� ������������ ������������ �������� �� �������� EFUSE_SMA  ������ ����� ���� ��� ������� � ��� 2.1 �� 1000 [���]
    oppMaxIdx = BootMaxOppGet();
    //� ������������ ��� �������� �� �������� ���������� ������������ ������� 1.325 ����� 1000 [���] �� �������
    SetVdd1OpVoltage(oppTable[oppMaxIdx].pmicVolt);
#endif


    //Disable watchdog options
    HWREG(SOC_WDT_1_REGS + WDT_WSPR) = 0xAAAAu;
    while(HWREG(SOC_WDT_1_REGS + WDT_WWPS) != 0x00);

    HWREG(SOC_WDT_1_REGS + WDT_WSPR) = 0x5555u;
    while(HWREG(SOC_WDT_1_REGS + WDT_WWPS) != 0x00);

    /* Configure DDR frequency 400 ��� DDR3*/

    /*
    if(isBBB)
    {
        freqMultDDR = DDRPLL_M_DDR3;   //400 [Mhz]
    }
    */

    //������ ����������� ����������� ����� ������ ARM ����������.
    /* Set the PLL Initialization */
    PLLInit();

    /* Enable the control module */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CONTROL_CLKCTRL) = CM_WKUP_CONTROL_CLKCTRL_MODULEMODE_ENABLE;

    /* EMIF Initialization */
    EMIFInit();

    /* DDR Initialization */
    //DDRVTTEnable();  //��� ������� �� ����� �������� ����� ���������.
    DDR3Init();


    DDRVTTEnable();


#if 0
    UARTSetup();

    ////////////////////������� ���������� � ���� �������//////////////////////////////
    switch(deviceVersion)  //������� CPU
    {case 2:
    UARTPuts("AM335x Revision 2.1 \n\r", -1);break;
    default: break;}
#endif    					  //������� CPU

   return arm3358_beagleboard;

}

/*
** Enable DDR_VTT.
**
*/
static void DDRVTTEnable(void)
{
//�������� ��������� ��� �� ���������� ��������� � �������
/* Enabling functional clocks for GPIO1 instance. */
GPIO1ModuleClkConfig();
/* Selecting GPIO1[23] pin for use. */
GPIO1Pin23PinMuxSetup();
/* Enabling the GPIO module. */
GPIOModuleEnable(GPIO_INSTANCE_ADDRESS);
/* Resetting the GPIO module. */
GPIOModuleReset(GPIO_INSTANCE_ADDRESS);
/* Setting the GPIO pin as an output pin. */
GPIODirModeSet(GPIO_INSTANCE_ADDRESS,GPIO_INSTANCE_PIN_NUMBER,GPIO_DIR_OUTPUT);
GPIOPinWrite(GPIO_INSTANCE_ADDRESS,GPIO_INSTANCE_PIN_NUMBER,GPIO_PIN_HIGH);
}













#if 0
static void DDRVTTEnable(void)
{
    GPIO0ModuleClkConfig();

    GPIO_PMUX_OFFADDR_VALUE(0, 7, PAD_FS_RXE_PD_PUPDE(7));

    /* Resetting the GPIO module. */
    GPIOModuleReset(SOC_GPIO_0_REGS);

    /* Enabling the GPIO module. */
    GPIOModuleEnable(SOC_GPIO_0_REGS);

    /* Setting the GPIO pin as an input pin. */    
    GPIODirModeSet(SOC_GPIO_0_REGS,GPIO_INSTANCE_PIN_NUMBER,GPIO_DIR_OUTPUT);

    GPIOPinWrite(SOC_GPIO_0_REGS, GPIO_INSTANCE_PIN_NUMBER,GPIO_PIN_HIGH);

}
#endif

/*
 * \brief This function does any post boot setup/init
 *
 * \param  none
 *
 * \return none
*/
void BlPlatformConfigPostBoot( void )
{
}

/*
 * \brief This function copies the image form SPI to RAM
 *
 * \param  none
 *
 * \return Status of the copy operation.
*/
#if defined(SPI)

unsigned int BlPlatformSPIImageCopy()
{
    ti_header header;

    /* Spi read function to read the header */
    BlSPIReadFlash(IMAGE_OFFSET, sizeof(header), (unsigned char *)&header);

    /* Copies application from SPI flash to DDR RAM */
    BlSPIReadFlash( IMAGE_OFFSET + 8,
                    header.image_size,
                    (unsigned char *)(header.load_addr) );
                     
    entryPoint = (unsigned int)header.load_addr;
    
    return (TRUE);
}
#endif

/*
 * \brief This function copies the image form MMCSD to RAM
 *
 * \param  none
 *
 * \return Status of the copy operation.
*/

#if defined(MMCSD)
/*
unsigned int BlPlatformMMCSDImageCopy()
{
    //Init function for copy LOW level MICROD register for copy image
	//HSMMCSDInit();
    //samo copurovanie obraza
    //HSMMCSDImageCopy();
    return (TRUE);
}
*/
#endif


/******************************************************************************
**                              END OF FILE
*******************************************************************************/
