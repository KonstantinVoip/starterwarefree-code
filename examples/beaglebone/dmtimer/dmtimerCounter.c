#include "consoleUtils.h"
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "interrupt.h"
#include "dmtimer.h"
#include "error.h"

/******************************************************************************
**                      INTERNAL MACRO DEFINITIONS
*******************************************************************************/
#define TIMER_INITIAL_COUNT             (0xFF000000u)
#define TIMER_RLD_COUNT                 (0xFF000000u)

/******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/
static void DMTimerAintcConfigure(void);
static void DMTimerSetUp(void);
static void DMTimerIsr(void);

/******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
static volatile unsigned int cntValue = 10;
static volatile unsigned int flagIsr = 0;

/******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/
int main(void)
{
    /* This function will enable clocks for the DMTimer2 instance */
    DMTimer2ModuleClkConfig();

    /* Initialize the UART console */
    ConsoleUtilsInit();

    /* Select the console type based on compile time check */
    ConsoleUtilsSetType(CONSOLE_UART);

    /* Enable IRQ in CPSR */
    IntMasterIRQEnable();

    /* Register DMTimer2 interrupts on to AINTC */
    DMTimerAintcConfigure();

    /* Perform the necessary configurations for DMTimer */
    DMTimerSetUp();

    /* Enable the DMTimer interrupts */
    DMTimerIntEnable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

    ConsoleUtilsPrintf("Tencounter: ");

    /* Start the DMTimer */
    DMTimerEnable(SOC_DMTIMER_2_REGS);

    while(cntValue)
    {
        if(flagIsr == 1)
        {
            ConsoleUtilsPrintf("\b%d",(cntValue - 1));
            cntValue--;
            flagIsr = 0;
        }
    }

    /* Stop the DMTimer */
    DMTimerDisable(SOC_DMTIMER_2_REGS);

    PRINT_STATUS(S_PASS);

    /* Halt the program */
    while(1);
}

/*
** Do the necessary DMTimer configurations on to AINTC.
*/
static void DMTimerAintcConfigure(void)
{
    /* Initialize the ARM interrupt control */
    IntAINTCInit();

    /* Registering DMTimerIsr */
    IntRegister(SYS_INT_TINT2, DMTimerIsr);

    /* Set the priority */
    IntPrioritySet(SYS_INT_TINT2, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enable the system interrupt */
    IntSystemEnable(SYS_INT_TINT2);
}

/*
** Setup the timer for one-shot and compare mode.
*/
static void DMTimerSetUp(void)
{
    /* Load the counter with the initial count value */
    DMTimerCounterSet(SOC_DMTIMER_2_REGS, TIMER_INITIAL_COUNT);

    /* Load the load register with the reload count value */
    DMTimerReloadSet(SOC_DMTIMER_2_REGS, TIMER_RLD_COUNT);

    /* Configure the DMTimer for Auto-reload and compare mode */
    DMTimerModeConfigure(SOC_DMTIMER_2_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);
}

/*
** DMTimer interrupt service routine. This will send a character to serial 
** console.
*/    
static void DMTimerIsr(void)
{
    /* Disable the DMTimer interrupts */
    DMTimerIntDisable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

    /* Clear the status of the interrupt flags */
    DMTimerIntStatusClear(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_IT_FLAG);

    flagIsr = 1;

    /* Enable the DMTimer interrupts */
    DMTimerIntEnable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);
}












