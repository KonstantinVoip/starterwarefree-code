/**
 * \file   gpioLEDBlink.c
 *
 *  \brief  This application uses a GPIO pin to blink the LED.
 *
*/


/*
 *
 * Описание PIN где смотерть на осцилографе наш код   +FEMALE P9 HEADER+
 * SPI_CLK         A17                                 -> 22 (pin)               SPI0_SCLK  //оставляем как есть   Pin mux 0
 * SPD_DATA        B16                                 -> 18 (pin)               SPI0_D1    //оставляем как есть   pin mux 0
 * SPI_SEL1        C12                                 -> 28 (pin)               GPIO3_17   //					   pin mux 7 режим
 * SPI_SEL2        D12                                 -> 30 (pin)               GPIO3_16   //					pin mux 7 режим
 */





#include "soc_AM335x.h"
#include "hw_control_AM335x.h" //skd_add
#include "hw_types.h"          //skd_add
#include "beaglebone.h"
#include "gpio_v2.h"
#include "interrupt.h"
#include "mcspi.h"


#define FLASH_WRITE_IN_PROGRESS          (0x01u)
#define FLASH_SECTOR_ADD_HIGH            (0x3Fu)
#define FLASH_SECTOR_ADD_LOW             (0x00u)
#define FLASH_SECTOR_ADD_MID             (0x00u)
#define FLASH_READ_STAT_REG1             (0x05u)
#define FLASH_WRITE_ENABLE               (0x06u)
#define FLASH_SECTOR_ERASE               (0x20u)
#define FLASH_PAGE_PROGRAM               (0x02u)
#define FLASH_DUMMY_BYTE                 (0xFFu)
#define FLASH_DATA_READ                  (0x03u)
#define MCSPI_OUT_FREQ                   (24000000u) //24 Mhz
#define MCSPI_IN_CLK                     (48000000u) //48 Mhz


#define MCSPI6Mhz_OUT_FREQ               (6000000u)  //6 Mhz
#define MCSPI5Mhz_OUT_FREQ               (5000000u)  //5 Mhz



/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS DTIMER2
*****************************************************************************/
#define TIMER_INITIAL_COUNT             (0xFF000000u)
#define TIMER_RLD_COUNT                 (0xFF000000u)


/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS GPIO1
*****************************************************************************/
#define GPIO_INSTANCE_ADDRESS           (SOC_GPIO_1_REGS)
#define GPIO_INSTANCE_PIN_NUMBER        (23)


/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS GPIO3
*****************************************************************************/
#define GPIO3_INSTANCE_ADDRESS           (SOC_GPIO_3_REGS)

#define GPIO3_INSTANCE_PIN_NUMBER_14        (14)
#define GPIO3_INSTANCE_PIN_NUMBER_15        (15)
#define GPIO3_INSTANCE_PIN_NUMBER_16        (16)
#define GPIO3_INSTANCE_PIN_NUMBER_17        (17)



#define REG_IDX_SHIFT                      (0x05)
#define REG_BIT_MASK                       (0x1F)
#define NUM_INTERRUPTS                     (128u)

/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
static void Delay(unsigned int count);


static unsigned int flag = 1;

//unsigned char rxBuffer[260]; //260 байт
//unsigned char vrfyData[256]; //256 байт
static  unsigned int length = 0;
static unsigned int chNum = 0;
//unsigned char *p_rx;
//unsigned char txBuffer[260];  //260 байт
//unsigned char *p_tx;


//static unsigned char transmit_to_spi[1]={0x0A,0xFF};
//static unsigned int IsWriteSuccess(void);
static void McSPI0AintcConfigure(void);
//static void FlashStatusGet(void);
static void super_McSPITransfer(void);
//static void WriteEnable(void);
//static void IsFlashBusy(void);
static void McSPISetUp(void);
static void super_McSPITransfer(void);
//static void syl_write_to_spi_bus();


/*****************************************************************************
**                INTERNAL FUNCTION DEFINITIONS
*****************************************************************************/
/*
** The main function. Application starts here.
*/
int main()
{
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

    //+++++++++++++++++++++++++++++++++++++GPIO3 MODULE ADD++++++++++++++++++++++++++++++++++++++++++++
    GPIO3ModuleClkConfig();

    //pin_mux
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_ACLKX)  = CONTROL_CONF_MUXMODE(7);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_FSX)    = CONTROL_CONF_MUXMODE(7);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AXR0)   = CONTROL_CONF_MUXMODE(7);
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AHCLKR) = CONTROL_CONF_MUXMODE(7);

    GPIOModuleEnable(GPIO3_INSTANCE_ADDRESS);
    GPIOModuleReset(GPIO3_INSTANCE_ADDRESS);

    GPIODirModeSet(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_14,GPIO_DIR_OUTPUT);
    GPIODirModeSet(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_15,GPIO_DIR_OUTPUT);
    GPIODirModeSet(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_16,GPIO_DIR_OUTPUT);
    GPIODirModeSet(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_17,GPIO_DIR_OUTPUT);



    #if 0
        /* Enable the clocks for McSPI0 module.*/
    	McSPI0ModuleClkConfig();

    	/* Perform Pin-Muxing for SPI0 Instance  set SPI_CLK and SPI_D1(data_output) */
    	McSPIPinMuxSetup(0);
    	//ConsoleUtilsPrintf("___McSPIPinMuxSetup(0)__;\n\r");
        /* Enable IRQ in CPSR.*/
        IntMasterIRQEnable();
        //ConsoleUtilsPrintf("___IntMasterIRQEnable();__\n\r");
        /* Map McSPI Interrupts to AINTC */
        McSPI0AintcConfigure();
        //ConsoleUtilsPrintf("___McSPI0AintcConfigure();__\n\r");
        /* Do the necessary set up configurations for McSPI.*/
        McSPISetUp();
       // ConsoleUtilsPrintf("___McSPISetUp();__\n\r");
       #endif


       IntRegister(SYS_INT_SPI0INT, super_McSPITransfer);    //65 Interrup IRQ
       McSPIIntEnable(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum) | MCSPI_INT_RX_FULL(chNum));

      //Для Диагностики Зажигаем Светодиод
     // GPIOPinWrite(GPIO_INSTANCE_ADDRESS,GPIO_INSTANCE_PIN_NUMBER,GPIO_PIN_HIGH);





    while(1)
    {
        // Driving a logic HIGH on the GPIO pin.

    	GPIOPinWrite(GPIO_INSTANCE_ADDRESS,GPIO_INSTANCE_PIN_NUMBER,GPIO_PIN_HIGH);
        GPIOPinWrite(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_14,GPIO_PIN_HIGH);
        GPIOPinWrite(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_15,GPIO_PIN_HIGH);
        GPIOPinWrite(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_16,GPIO_PIN_HIGH);
        GPIOPinWrite(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_17,GPIO_PIN_HIGH);

        //Delay(0x3FFF);
        // Driving a logic LOW on the GPIO pin
        GPIOPinWrite(GPIO_INSTANCE_ADDRESS,GPIO_INSTANCE_PIN_NUMBER,GPIO_PIN_LOW);
        GPIOPinWrite(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_14,GPIO_PIN_LOW);
        GPIOPinWrite(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_15,GPIO_PIN_LOW);
        GPIOPinWrite(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_16,GPIO_PIN_LOW);
       // GPIOPinWrite(GPIO3_INSTANCE_ADDRESS,GPIO3_INSTANCE_PIN_NUMBER_17,GPIO_PIN_LOW);
        //Delay(0x3FF0);
        super_McSPITransfer();
   }




} 

/*
** A function which is used to generate a delay  создаём задердку для времени.
*/


#if 0
static void Delay(volatile unsigned int count)
{
    while(count--);
}







/* Interrupt mapping to AINTC and registering McSPI ISR */
static void McSPI0AintcConfigure(void)
{
    /* Initialze ARM interrupt controller */
    IntAINTCInit();

    /* Register McSPIIsr interrupt handler */
    //IntRegister(SYS_INT_SPI0INT, McSPIIsr);    //65 Interrup IRQ

    /* Set Interrupt Priority */
    IntPrioritySet(SYS_INT_SPI0INT, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enable system interrupt in AINTC */
    IntSystemEnable(SYS_INT_SPI0INT);
}



static void McSPISetUp(void)
{

    /* Reset the McSPI instance.*/
    McSPIReset(SOC_SPI_0_REGS);

    /* Enable chip select pin.*/
    McSPICSEnable(SOC_SPI_0_REGS);

    /* Enable master mode of operation.*/
    McSPIMasterModeEnable(SOC_SPI_0_REGS);

    /* Perform the necessary configuration for master mode.*/
    McSPIMasterModeConfig(SOC_SPI_0_REGS, MCSPI_SINGLE_CH,MCSPI_TX_ONLY_MODE, MCSPI_DATA_LINE_COMM_MODE_1,chNum);

    /* Configure the McSPI bus clock depending on clock mode. */
    //McSPIClkConfig(SOC_SPI_0_REGS, MCSPI_IN_CLK, MCSPI6Mhz_OUT_FREQ, chNum,MCSPI_CLK_MODE_1);
    McSPIClkConfig(SOC_SPI_0_REGS, MCSPI_IN_CLK, MCSPI5Mhz_OUT_FREQ, chNum,MCSPI_CLK_MODE_1);



    /* Configure the word length.*/
   //McSPIWordLengthSet(SOC_SPI_0_REGS, MCSPI_WORD_LENGTH(8), chNum);

    McSPIWordLengthSet(SOC_SPI_0_REGS, MCSPI_WORD_LENGTH(26), chNum);

    /* Set polarity of SPIEN to low.*/
   // McSPICSPolarityConfig(SOC_SPI_0_REGS, MCSPI_CS_POL_LOW, chNum);

    /* Enable the transmitter FIFO of McSPI peripheral.*/
    McSPITxFIFOConfig(SOC_SPI_0_REGS, MCSPI_TX_FIFO_ENABLE, chNum);

    /* Enable the receiver FIFO of McSPI peripheral.*/
   // McSPIRxFIFOConfig(SOC_SPI_0_REGS, MCSPI_RX_FIFO_ENABLE, chNum);
   // McSPIRxFIFOConfig(SOC_SPI_0_REGS, MCSPI_RX_FIFO_DISABLE, chNum);
}
#endif


#if 0
static void syl_write_to_spi_bus()
{
   // unsigned int index = 0;

  //  txBuffer[0] = 0xAA;
  //  txBuffer[1] = 0xBB;
  //  txBuffer[2] = 0xCC;

    //length = 1;  //24 bits send data to SPI
    //ConsoleUtilsPrintf("___syl_write_to_spi_bus();__\n\r");
    /*
    for(index = 0; index < 256; index++)
    {
        txBuffer[index + 4] = (unsigned char) index;
        vrfyData[index] = (unsigned char) index;
    }

    length = 260;
    ConsoleUtilsPrintf("++mcspi.c/WriteToFlash++\r\n");
    */
    //ConsoleUtilsPrintf("syl_write_to_spi_bus()\n\r");
    McSPITransfer();
}
#endif





/*
** This function will activate/deactivate CS line and also enable Tx and Rx
** interrupts of McSPI peripheral.
*/
static void super_McSPITransfer(void)
{
   // p_tx = &transmit_to_spi[0];//txBuffer;
   // p_rx = rxBuffer;
	length=1;
    /* SPIEN line is forced to low state.*/


	//McSPICSAssert(SOC_SPI_0_REGS, chNum);
	//McSPIIntEnable(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum) | MCSPI_INT_RX_FULL(chNum));
/*
    // Enable the Tx/Rx interrupts of McSPI.
    //McSPIIntEnable(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum) | MCSPI_INT_RX_FULL(chNum));
    McSPIIntEnable(SOC_SPI_0_REGS, MCSPI_INT_TX_EMPTY(chNum));

    // Enable the McSPI channel for communication.
    McSPIChannelEnable(SOC_SPI_0_REGS, chNum);

    // Wait until control returns back from McSPI ISR.
    while(flag);

    flag = 1;

   // Force SPIEN line to the inactive state.
    McSPICSDeAssert(SOC_SPI_0_REGS, chNum);

    // Disable the McSPI channel.
   McSPIChannelDisable(SOC_SPI_0_REGS, chNum);*/
}









/*
** McSPI Interrupt Service Routine. This function will clear the status of the
** Tx/Rx interrupts when generated. Will write the Tx data on transmit data
** register and also will put the received data from receive data register to
** a location in memory.
*/
/*
static void super_McSPIIsr(void)
{
    unsigned int intCode = 0;



}
*/











/******************************* End of file *********************************/
