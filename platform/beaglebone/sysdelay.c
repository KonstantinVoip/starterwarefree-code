/**
 * \file   sysdelay.c
 *
 * \brief  This file contains functions that configures a DMTimer instance
 *         for operation and to generate a requested amount of delay.
 *
*/
#include "soc_AM335x.h"
#include "hw_control_AM335x.h" //skd_add_for_pin
#include "hw_types.h"          //skd_add_for_pin
#include "beaglebone.h"
#include "clock.h"
#include "interrupt.h"
#include "dmtimer.h"
#include "consoleUtils.h"
#include "delay.h"


#define timer_1ms_delay     0x5DC0u  //24000 тиков   = 1[мс]
#define timer_100mks_delay  0x0960u  //2400 тиков   = 100[мкс]
#define timer_10mks_delay   0x00F0


#define TIME_GRANULARITY                (1000000u)
#define TIMER_PERF_BASE                 (SOC_DMTIMER_7_REGS)
#define TIMER_INITIAL_COUNT             (0xFFFFFFFFu-12)
#define TIMER_RLD_COUNT                 (0xFFFFFFFFu-12)

static void DMTimerSetUp(void);


/***********************************************************************************************/
//DTIMER 3 GLOBAL function setup
/***********************************************************************************************/
void syl_dtimer3_init()
{

    DMTimer3ModuleClkConfig();
    DMTimerSetUp();


}



static void DMTimerSetUp(void)
{
	DMTimerPreScalerClkDisable(SOC_DMTIMER_3_REGS);
    DMTimerReset(SOC_DMTIMER_3_REGS);
    DMTimerCounterSet(SOC_DMTIMER_3_REGS, TIMER_INITIAL_COUNT);
    DMTimerReloadSet(SOC_DMTIMER_3_REGS, TIMER_RLD_COUNT);
    DMTimerModeConfigure(SOC_DMTIMER_3_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);
    DMTimerPostedModeConfig(SOC_DMTIMER_3_REGS,DMTIMER_POSTED);
}



//Пример для разбора функций.
void dmtimer3_2_microsecond (unsigned int microsec)
{
	   HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR)=0;
		//DMTimerEnable(SOC_DMTIMER_3_REGS);
       HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) |= DMTIMER_TCLR_ST;
       while(HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCRR) < 0x0008); // 2 мкс
       HWREG(SOC_DMTIMER_3_REGS + DMTIMER_TCLR) &= ~DMTIMER_TCLR_ST;

}








/***********************************************************************************************/
//DTIMER 2 GLOBAL function setup
/***********************************************************************************************/
void syl_dtimer2_init()
{
	/* This function will enable clocks for the DMTimer2 instance */
    DMTimer2ModuleClkConfig();

}


void syl_Sysdelay(unsigned int milliSec)
{

    while(milliSec != 0)
    {
        DMTimerCounterSet(SOC_DMTIMER_2_REGS, 0);
        DMTimerEnable(SOC_DMTIMER_2_REGS);
        //while(DMTimerCounterGet(SOC_DMTIMER_2_REGS) < timer_1ms_delay);

        while(DMTimerCounterGet(SOC_DMTIMER_2_REGS) < timer_1ms_delay);

        DMTimerDisable(SOC_DMTIMER_2_REGS);
        milliSec--;
    }


}

/***********************************************************************************************/
//PERFOMANCE DRIMER 7 FUNCTION SETUP
/***********************************************************************************************/
//Меряем Производительность
void  inline  dmtimer7_SysPerfTimerSetup(void)
{
    DMTimer7ModuleClkConfig();
    DMTimerCounterSet(TIMER_PERF_BASE, 0);
}

//запускаем  таймер   //flag =1
void inline  dmtimer7_PerfTimerStart(unsigned short flag)
{

     // DMTimerCounterSet(TIMER_PERF_BASE, 0);
	  HWREG(TIMER_PERF_BASE + DMTIMER_TCRR) = 0;

	  //DMTimerEnable(TIMER_PERF_BASE);
	   /* Start the timer */
	  HWREG(TIMER_PERF_BASE + DMTIMER_TCLR) |= DMTIMER_TCLR_ST;

}

//останавливаем  таймер   //flag =0 выводим на печать сколько получили
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

 //возвращаем количество тиков
 return timeInTicks;
}
















/***********************************************************************************************/
//HARDWARE FUNCTION SETUP
/***********************************************************************************************/
/*****************************************************************************
Syntax:      	    int writel(unsigned int val ,void * addr)
Return Value:	    write to reg value
*******************************************************************************/
void  writel_to_reg(unsigned int val ,void * addr)
{

	HWREG(addr)  = val;

}

/*****************************************************************************
Syntax:      	    int readl(void * addr)
Return Value:	    Returns 1 reg value.
*******************************************************************************/
unsigned int  readl_from_reg(void * addr)
{

	return HWREG(addr);
}




























