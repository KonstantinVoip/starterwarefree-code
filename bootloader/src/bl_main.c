/**
 * \file  bl_main.c
 *
 * \brief Implements main function for StarterWare bootloader
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

#include "uartStdio.h"
#include "bl_platform.h"
#include "bl.h"
#include "delay.h"
#include "stdio.h"
#include "consoleUtils.h"
#include "string.h"
#include "syl_mmcsd_proto.h"
#include "hw_types.h"          //skd_add_for_pin
#include "beaglebone.h"
#include "interrupt.h"
//#include "dmtimer.h"
//#include "cp15.h"
//�������� ������� ������������������
#include "clock.h"




#define EMMC_FLASH     1   //��� eMMC ������
//#define MMCSD_CARD   1   // ��� SD �����


/*������� ����� �������� MLO ������� ���������� */
//#define REVISION_MLO 200715
//#define REVISION_MLO 270715
//#define REVISION_MLO 150815
  #define REVISION_MLO 280815    //NO NEON and floating point

/*FIND MAIN START FUNCTION  ->main address   0x08, 0x40, 0x2D, 0xE9,0xFA, 0xFF, 0xFF, 0xEB, 0xA1, 0x0F, 0x8F, 0xE2, */
/*����� ��������� +8 ���� � ������ ����� ������� */


/******************************************************************************
**                    External Variable Declararions 
*******************************************************************************/
extern char *deviceType;

/******************************************************************************
**                     Global Variable Definitions
*******************************************************************************/

unsigned int entryPoint = 0;
unsigned int DspEntryPoint = 0;
static void (*appEntry)();


#if 0
#define TIMER_PERF_BASE                 (SOC_DMTIMER_7_REGS)
#define TIME_GRANULARITY                (1000000u)




//������ ������������������
void  inline  dmtimer7_SysPerfTimerSetup(void)
{
    DMTimer7ModuleClkConfig();
    DMTimerCounterSet(TIMER_PERF_BASE, 0);
}

//���������  ������   //flag =1
void inline  dmtimer7_PerfTimerStart(unsigned short flag)
{

     // DMTimerCounterSet(TIMER_PERF_BASE, 0);
	  HWREG(TIMER_PERF_BASE + DMTIMER_TCRR) = 0;

	  //DMTimerEnable(TIMER_PERF_BASE);
	   /* Start the timer */
	  HWREG(TIMER_PERF_BASE + DMTIMER_TCLR) |= DMTIMER_TCLR_ST;

}

//�������������  ������   //flag =0 ������� �� ������ ������� ��������
unsigned int inline dmtimer7_PerfTimerStop(unsigned short flag)
{
    unsigned int timeInTicks = 0;
    unsigned int time=0;
    //DMTimerDisable(TIMER_PERF_BASE);
    HWREG(TIMER_PERF_BASE + DMTIMER_TCLR) &= ~DMTIMER_TCLR_ST;

    timeInTicks = DMTimerCounterGet(TIMER_PERF_BASE);

    /* Get the time taken in from the ticks read from the Timer */
    time = (timeInTicks/ (CLK_EXT_CRYSTAL_SPEED/TIME_GRANULARITY));
    ConsoleUtilsPrintf("-takes %u [microsec] %u[nano]  %u [ticks]-\r\n",time,timeInTicks*41,timeInTicks);

 //���������� ���������� �����
 return timeInTicks;
}
#endif





/******************************************************************************
**                     Global Function Definitions
*******************************************************************************/
/*
 * \brief This function initializes the system and copies the image. 
 *
 * \param  none
 *
 * \return none 
*/
int main(void)
{

    volatile unsigned int i=0;

//��������� �� ��������� � ��� ��� ������������
	//int val;
	/* Configures PLL and DDR controller*/
   // ti_header *imageHdr=0;
	struct parametr_of_sysytem  out;
    //unsigned int n=0;
	unsigned int *sl_info=(unsigned int*)0x90000000;
	//unsigned int  val_of_entry_point=0;

///////////////////////////��� SD �����
#ifdef 	MMCSD_CARD
	    struct mmc *mmc_sd=0;
	 	unsigned int dev_sd =0;   //dev =0 SD card
#endif

///////////////////////////��� EMMC �����
#ifdef  EMMC_FLASH
	    struct mmc *mmc_emmc=0;
	    unsigned int dev_mmc=1;   //dev =1 eMMC card
#endif


	/*������ ������ ���������� APP(app) � ����� */  /*������ � ������� ������� 64 �/���� ������ */
 	unsigned int blk=0x400;                         //����� ���������� � 0x400 (1024) �����  524288[����] = 512 [�/����]
 	unsigned int cnt=0x200;                         //������ ���������� 256 �/���� ������� ������ ������ ������ ����������.

 	/*������ ��������� ���������� �� 512 ���� � ������ ������ ����� ����� ���������� Entry:*/
    unsigned int blk_sl_info_array=0x0300;   //� ������� ������� ������
    unsigned int cnt_sl_info_array=0x0001;   //������ 1 ���� �������� 512 ����;

  	/*����� ������ ����� ������ ������� ��� ������ ���������� ���� �������� ���
  	 *��������� ������ ����� � application  */
  	//Popravil kosak s I2C bus na plate


    out=BlPlatformConfig();
    //UARTStdioInit();
    //UARTConsoleInit();
    //UARTStdioInit();

    /* Configuring the system clocks for UART0 instance. */


    //UARTModuleReset(unsigned int baseAdd);
    //UART0ModuleClkConfig();

    /* Performing the Pin Multiplexing for UART0 instance. */
    //UARTPinMuxSetup(0);

    //UARTStdioInitExpClk(BAUD_RATE_115200, 1, 1);


    //UART MODULE RESET
#if 0
    // Performing Software Reset of the module.
    HWREG(baseAdd + UART_SYSC) |= (UART_SYSC_SOFTRESET);
    // Wait until the process of Module Reset is complete.
    while(!(HWREG(baseAdd + UART_SYSS) & UART_SYSS_RESETDONE));
#endif



    UARTStdioInit();

    //ConsoleUtilsInit();
    ConsoleUtilsPrintf("--------------------------START_FIRST_BOOT----------------------------\n\r");

    /*������ ���������� � ���������� ����� ����� ����������������*/
    memset((void *)0x90000000,0x00,65536);   //DDR  256 �/����    0-256  �/����
    memset((void *)0x80000000,0x00,65536);   //DDR  256 �/����   256-512 �/����

    //ConsoleUtilsInit();
 	//ConsoleUtilsSetType(CONSOLE_UART);

 	//UARTPuts("_NPO_Pelengator_first_bootloader_rev=%d);
 	ConsoleUtilsPrintf("boot:Pelengator_first_bootloader_rev=%d_\n\r",REVISION_MLO);
 	ConsoleUtilsPrintf("boot:arm=%d[mhz],ddr=%d[mhz]\n\r",out.arm_freq_mhz,out.ddr_freg_mhz);

 	switch(out.vdd_mpu_volt)
 	{
 	case(0x08) :
    ConsoleUtilsPrintf("boot:MPU_VDD=1.100[mv]\n\r");
    break;
 	default:
 	ConsoleUtilsPrintf("boot:MPU_VDD=error[mv]\n\r");
 	break;
 	}



 	//�������� ��� ����� �������������
     //ImageCopy();
    syl_dtimer2_init();



     #ifdef  EMMC_FLASH
     mmc_initialize(dev_mmc);
     mmc_emmc=find_mmc_device(dev_mmc);
     mmc_init( mmc_emmc);


     //������ ��������� ���������� 512 ���� ���� ����� EntryPoint ��� application
     /*������ � ������ 0x0000  1 ���� ����������  ���� �������� � DDR ���� sl_info */

     ConsoleUtilsPrintf("boot:COPY from EMMC blk_0x%x|cnt_0x%x to DDR ->0x80000000 \n\r",blk,cnt);

     /*n =*/ mmc_emmc->block_dev.block_read(dev_mmc, blk_sl_info_array, cnt_sl_info_array,(void*) 0x90000000);
     /*�������� ���� ���������� � ����� 0x80000000*/
     /*n =*/ mmc_emmc->block_dev.block_read(dev_mmc, blk, cnt,(void*)  0x80000000);

     //printf("\eMMC read: dev # %d, block # %d, count %d ... \n\r",dev_mmc, blk, cnt);
     //n = mmc_emmc->block_dev.block_read(dev_mmc, blk, cnt, 0x80000000);
     //printf("%d blocks read: %s\n", n, (n == cnt) ? "OK" : "ERROR");
     #endif

     //���������� ����� ����� ������ �� eMMC �� ������ 0x9000|0000
       entryPoint=*sl_info;
    // entryPoint=0x80000000;//0x80006e3c;
     //entryPoint=0x80007498;
     if(entryPoint==0)
     {
        	ConsoleUtilsPrintf("boot:NO_ENTRY_POINT_FOUND_in_sl_info\n\r");
            return 0;
     }


    //�������� ���� ����� UART ������� ����� ��� ���� ����. ��������� ���������.
    //����� ������� � ������� ����� ���.


     //GO TO APPLICATIONS

     ConsoleUtilsPrintf("boot:BOOT_GO_TO_ENTRY -->0x%x\n\r",entryPoint);
     /* Giving control to the application */
     appEntry = (void (*)(void)) entryPoint;


     if((*appEntry)==NULL)
     {

    	 ConsoleUtilsPrintf("boot:(*appEntry)==NULL ERROR\n\r");
    	 return 0;
     }
     //���� � ����� ����� ���������� ��� ����� ������ ������ eMMC �� �����������.
     //RETURN 0
     (*appEntry)( );  //ENTER to Entry in APPLICATION

#if 0
    //Read app image from eMMC card
    //Read app image from SD cart
    /*
    printf("\nMMC read: dev # %d, block # %d, count %d ... \n\r",dev_sd, blk, cnt);
    n = mmc_sd->block_dev.block_read(dev_sd, blk, cnt, 0x90000000);
    printf("%d blocks read: %s\n", n, (n == cnt) ? "OK" : "ERROR");
    */
 /*
    ConsoleUtilsPrintf("Device: %s\n", mmc_emmc->cfg->name);
    ConsoleUtilsPrintf("Manufacturer ID: %x\n", mmc_emmc->cid[0] >> 24);
    ConsoleUtilsPrintf("OEM: %x\n", (mmc_emmc->cid[0] >> 8) & 0xffff);
    ConsoleUtilsPrintf("Name: %c%c%c%c%c \n", mmc_emmc->cid[0] & 0xff,(mmc_emmc->cid[1] >> 24), (mmc_emmc->cid[1] >> 16) & 0xff,(mmc_emmc->cid[1] >> 8) & 0xff, mmc_emmc->cid[1] & 0xff);
    ConsoleUtilsPrintf("Tran Speed: %d\n", mmc_emmc->tran_speed);
    ConsoleUtilsPrintf("Rd Block Len: %d\n", mmc_emmc->read_bl_len);
    ConsoleUtilsPrintf("%s version %d.%d\n", IS_SD(mmc_emmc) ? "SD" : "MMC",(mmc_emmc->version >> 8) & 0xf, mmc_emmc->version & 0xff);
    ConsoleUtilsPrintf("High Capacity: %s\n", mmc_emmc->high_capacity ? "Yes" : "No");
	puts("Capacity: ");
	//print_size(mmc->capacity, "\n");
	ConsoleUtilsPrintf("Bus Width: %d-bit\n", mmc_emmc->bus_width);

	printf("-----------------------------------------------------------------------------------------/n/r");
	ConsoleUtilsPrintf("-----------------------------------------------------------------------------------------/n/r");

    ConsoleUtilsPrintf("Device: %s\n", mmc_sd->cfg->name);
    ConsoleUtilsPrintf("Manufacturer ID: %x\n", mmc_sd->cid[0] >> 24);
    ConsoleUtilsPrintf("OEM: %x\n", (mmc_sd->cid[0] >> 8) & 0xffff);
    ConsoleUtilsPrintf("Name: %c%c%c%c%c \n", mmc_sd->cid[0] & 0xff,(mmc_sd->cid[1] >> 24), (mmc_sd->cid[1] >> 16) & 0xff,(mmc_sd->cid[1] >> 8) & 0xff, mmc_sd->cid[1] & 0xff);
    ConsoleUtilsPrintf("Tran Speed: %d\n", mmc_sd->tran_speed);
    ConsoleUtilsPrintf("Rd Block Len: %d\n", mmc_sd->read_bl_len);
    ConsoleUtilsPrintf("%s version %d.%d\n", IS_SD(mmc_sd) ? "SD" : "MMC",(mmc_sd->version >> 8) & 0xf, mmc_sd->version & 0xff);
    ConsoleUtilsPrintf("High Capacity: %s\n", mmc_sd->high_capacity ? "Yes" : "No");
	puts("Capacity: ");
	//print_size(mmc->capacity, "\n");
	ConsoleUtilsPrintf("Bus Width: %d-bit\n", mmc_sd->bus_width);
   */

#endif

}

/*
void BootAbort(void)
{
    while(1);
}
*/
/******************************************************************************
**                              END OF FILE
*******************************************************************************/
