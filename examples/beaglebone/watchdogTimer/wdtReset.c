/**********************************************************************************************************************
*                                        (c) COPYRIGHT by ZAO NPK PELENGATOR                                                *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : armskd_syl_first_emmc_flash.c
*
* Description : Function for first flashing eMMC -> MLO, APLLICATION and TFA/TYR table
*
* Author      : Konstantin Shiluaev
*
******************************************************************************/
#include "stdio.h"
#include "stdlib.h"

#include "beaglebone.h"

//#include "edma_event.h"
#include "soc_AM335x.h"
#include "hw_control_AM335x.h"
#include "hw_cm_per.h"
//#include "interrupt.h"
#include "consoleUtils.h"

#include "string.h"
#include "delay.h"
#include "cache.h"
//#include "edma.h"
//#include "mmu.h"
#include "consoleUtils.h"

#include "syl_mmcsd_proto.h"


//#include  "armskd_syl_first_emmc_flash.h"
#include  "emmc_hex_MLO_bootloader.h"      //massive gde hranitsa first MLO  bootloader
//#include  "emmc_hex_application.h"         //massive gde hranitsa application in HEX ARRAY

/*FIND MAIN START FUNCTION  ->main address   0x08, 0x40, 0x2D, 0xE9, 0xFA, 0xFF, 0xFF, 0xEB, 0xA1, 0x0F, 0x8F, 0xE2,
 *THIS CORRECT find ENTRY start function !!!!!! not MAIN and bin image not ti BIN IMAGE
 * */
//#include "MLO_array.h"


/*
 * ��������� �� �������� ������ ��� ��� 1,2,3,4 ������.
 * ��� ��������� ������� ������� � ������� ����� ���������.
 * ��� ������� ������
 * unsigned char array[4][4] =
 * {
 * 10,20,30,40,
 * 50,60,70,80,
 * 22,33,44,55,
 * 66,77,88,99,
 * }
 *
 * ������������ � DDR ��������� �������:
 *
 * ����� ������:0x80000000
 * 10,20,30,40,50,60,70,80,22,33,44,55,66,77,88,99
 *
 * �������� ����(tfa.bin):
 * (2<FP!,7BMXc
 *
 * ��� ��������� ��� �������� ������� ������ ������ 8�/����
 *
 * Memory Browser -> Load Memory ->
*
*  ����������� File Type: Ti Raw Data  (���� ������� ���-�� ������ �� ������� ����� � �������)
*
*  Start Address ���������� ��� � ��� ����� �������.
*
*
*  					������ DDR     |  ������ eMMC
*
*   MLO                                                 -���������� ���������
*
*   SL_INFO                                             -��������� ���������� (512 ����) ��������� �� ������� ���������� ������ �� ������
*
*   APPLICATION                                         -��������������� ����������� ����������
*  ------------------------------------------------------------------------------------------------------
*                              mem        (32768)       1 ���� = 512 ���� *32768 = 16 772 16 (16 �/����)
*   tab_1   				0x01000000  blk=0x8000 ->    tab1_������ �������            16 �/����
*                                         (65536)
*                                       blk=0x10000      ����� �������_1
*   -----------------------------------------------------------------------------------------------
*   tab_2 								  				-tab2_������ �������            16 �/����
*	                                      (65536)
*									    blk=0x10000  	 tab2_������ �������            16 �/����
*                                         (98304)
*                                       blk=0x18000
*   ----------------------------------------------------------------------------------------------
*   tab_3												-tab3_������ �������            16 �/����
*                                         (98304)
*                                       blk=0x18000
*
                                         (131072)
*                                       blk=0x20000
*   ----------------------------------------------------------------------------------------------
*   tab_4												-tab4_�������� �������         16 �/����
*                                         (131072)
*                                       blk=0x20000
*
*                                         (163840)
*                                       blk=0x28000
*
*/


//Include for Table TFA and TYR ���� �� ����������.
/*
#include  "tfa_tab_1.h"          //    test tfa table
#include  "tfa_tab_2.h"          //    test tyr table
#include  "tfa_tab_3.h"          //
#include  "tfa_tab_4.h"          //
#include  "tyr_tab_1.h"         //
#include  "tyr_tab_2.h"         //
#include  "tyr_tab_3.h"          //
#include  "tyr_tab_4.h"         //
*/


#pragma DATA_SECTION(bufferB, "application")
unsigned char bufferB[131072];


//������� ���
#pragma DATA_SECTION(buf_ppmtab_1, "ppm_tab1")
unsigned char  buf_ppmtab_1[16777216];               //16 �/����

#pragma DATA_SECTION(buf_ppmtab_2, "ppm_tab2")       //16 �/����
unsigned char  buf_ppmtab_2[16777216];

#pragma DATA_SECTION(buf_ppmtab_3, "ppm_tab3")       //16 �/����
unsigned char buf_ppmtab_3[16777216];

#pragma DATA_SECTION(buf_ppmtab_4, "ppm_tab4")       //16 �/����
unsigned char  buf_ppmtab_4[16777216];



/*�������� ��� �������� */
#define EMMC_FLASH       1     //��� eMMC ������
//#define MMCSD_CARD     1     //��� SD �����


/*�������� ��� ���������� �� ������ */
#define 	 MLO_FIRST_LOADER     1
//#define  	 APPLICATION          1
//#define    SLINFO_ENTRY_POINT   1

//#define ERASE_MLO_FIRST_LOADER     1
//#define ERASE_APPLICATION		   1


//������ ������ ��� ���
//#define 	 PPM_TAB_1_WRITE     	1
//#define 	 PPM_TAB_2_WRITE      	1
//#define 	 PPM_TAB_3_WRITE      	1
//#define 	 PPM_TAB_4_WRITE     	1



//������� ������ ������ �� eMMC ������ [first_MLO_bootloader][Application][TYR/TFA TABLE]
int main(void)
{
	   unsigned int n=0;



	   //eMMC flash
		#ifdef  EMMC_FLASH
		struct mmc *mmc_emmc=0;
	    unsigned int dev_mmc=1;   //dev -1 eMMC card
		#endif

	    //MMC SD Card
	    #ifdef 	MMCSD_CARD
		struct mmc *mmc_sd=0;
	 	unsigned int dev_sd =0;   //dev -1 eMMC card
	    #endif


	 	/////////////////////////////////////////////////////////
		#ifdef  PPM_TAB_1_WRITE
	 	  unsigned int blk=0x8000;       //start block address
		  unsigned int cnt=0x8000;       //32768 blocks  num of block size 512 bait  * 0x8000(32768)  = 262144 ()  = 16777216
		#endif
        //////////////////////////////////////////////////////////
		#ifdef  PPM_TAB_2_WRITE
	 	  unsigned int blk=0x10000;       //start block address
		  unsigned int cnt=0x8000;     //512 blocks  num of block size 512 bait  * 0x100(512)  = 262144 ()  = 256 �/����
		#endif
        ///////////////////////////////////////////////////////////
		#ifdef  PPM_TAB_3_WRITE
	 	  unsigned int blk=0x18000;       //start block address
		  unsigned int cnt=0x8000;     //512 blocks  num of block size 512 bait  * 0x100(512)  = 262144 ()  = 256 �/����
		#endif
        ///////////////////////////////////////////////////////////
		#ifdef  PPM_TAB_4_WRITE
	 	  unsigned int blk=0x20000;       //start block address
		  unsigned int cnt=0x8000;     //512 blocks  num of block size 512 bait  * 0x100(512)  = 262144 ()  = 256 �/����
		#endif





	     #ifdef  MLO_FIRST_LOADER
		  /*������ ���������� ���������� MLO � ������� ������� ����� eMMC */
	      unsigned int   memaddr_emmc_mlo_bootloader_array=0;  //Memory address of const array mlo first bootloader
	 	  unsigned int blk=0x0;       //start block address
		  unsigned int cnt=0x200;     //512 blocks  num of block size 512 bait  * 0x100(512)  = 262144 ()  = 256 �/����
		  memaddr_emmc_mlo_bootloader_array =&emmc_mlo_bootloader[0];  //get address of MLO arrray in memory
		  //memaddr_emmc_mlo_bootloader_array =&rawData[0];
		// ConsoleUtilsPrintf("memaddr_emmc_mlo_bootloader_array=0x%x\n\r+", memaddr_emmc_mlo_bootloader_array);
		 #endif


	     #ifdef SLINFO_ENTRY_POINT
		 unsigned int   entry_point_application=0x80000000;
		 unsigned int blk=0x300;       //start block address ��������� ���� 0x250
		 unsigned int cnt=0x001;       //1 block = 512 ���� ��������� ����������
		 ConsoleUtilsPrintf("memaddr_Entry_point_applicaion=0x%x\n\r+", &entry_point_application);
		 #endif


	     #ifdef  APPLICATION
		  FILE *fp;
		  unsigned int   memaddr_emmc_application_array=0;  //Memory address of const array mlo first bootloader
		  /*������ ������ ���������� APP(app) */  /*������ � ������� ������� 64 �/���� ������ */
		  unsigned int blk=0x400;               //����� ���������� � 0x400 (1024) �����  524288[����] = 512 [�/����]
		  unsigned int cnt=0x200;               //������ ���������� 256 �/���� ������� �������.

		  /*���� �� ������ �������� �� HEX ������� �������������� */
		  //memaddr_emmc_application_array=&emmc_application[0];


		  fp=fopen("C:/Share/sota_skd/arm_skd_syl/Debug/arm_skd_syl.bin" ,"rwb+");

		  //fp=fopen("C:/Share/sota_skd/arm_skd_syl/Debug/arm_skd_syl_ti_ok.bin" ,"rwb+");
		  //fp=fopen("C:/Share/sota_skd/arm_skd_syl/Debug/arm_skd_syl_ti.bin" ,"rwb+");
		  if(fp==0)
		  {
		 	 printf("ERROR OPEN FILE\n\r");
		 	 ConsoleUtilsPrintf("memaddr_emmc_application_arrayr=0x%x\n\r+",memaddr_emmc_application_array);
		 	 return 0;
		  }
		  fread(&bufferB,65536,1,fp);
		  memaddr_emmc_application_array=&bufferB[0];
		  fclose(fp);
		  ConsoleUtilsPrintf("memaddr_emmc_application_arrayr=0x%x\n\r+",memaddr_emmc_application_array);
		  #endif



	     #ifdef  TFA_TABLE_1
		 	 /*������ ��� ������� */  /*������ � ������� ������� 64 �/���� ������ */
	     #endif

	     ///////////////////////////////////////////////////////////////////////////////////////////////

	     #ifdef  TYR_TABLE_1
		 	 /*������ ���� ������� */  /*������ � ������� ������� 64 �/���� ������ */
	     #endif

	    ////////////////////////////////////////////////////////////////////////////////////////////////
		  //������ ���� ����� ��������������_������_2 ����������
		syl_dtimer2_init();


		#ifdef  EMMC_FLASH
	     mmc_initialize(dev_mmc);
	     mmc_emmc=find_mmc_device(dev_mmc);
	     mmc_init( mmc_emmc);
	   #endif


		#ifdef ERASE_MLO_FIRST_LOADER
	 	unsigned int blk=0x0;       //start block address
		unsigned int cnt=0x200;     //512 blocks  num of block size 512 bait  * 0x100(512)  = 262144 ()  = 256 �/����
		n = mmc_emmc->block_dev.block_erase(dev_mmc, blk, cnt);
		printf("\nMMC erase_MLO_FIRST_LOADER : dev # %d, block # %d, count %d ...\n\r ",dev_mmc, blk, cnt);
	    return 1;
		#endif


		#ifdef ERASE_APPLICATION
		unsigned int blk=0x400;               //����� ���������� � 0x400 (1024) �����  524288[����] = 512 [�/����]
		unsigned int cnt=0x200;               //������ ���������� 256 �/���� ������� �������.
	    printf("\nMMC erase_APPLICATION : dev # %d, block # %d, count %d ...\n\r ",dev_mmc, blk, cnt);
	    n = mmc_emmc->block_dev.block_erase(dev_mmc, blk, cnt);
		return 1;
	    #endif


	    ///////////////////////////��� EMMC �����
	    #ifdef  EMMC_FLASH
	     ///////////////////////////////////////////Erase to FLASH////////////////////////////////////////////////////////////////
	   #ifdef APPLICATION
	    printf("\nMMC erase: dev # %d, block # %d, count %d ...\n\r ",dev_mmc, blk, cnt);
	    n = mmc_emmc->block_dev.block_erase(dev_mmc, blk, cnt);
	   #endif


	   #ifdef  MLO_FIRST_LOADER
	    //printf("\nMMC erase: dev # %d, block # %d, count %d ...\n\r ",dev_mmc, blk, cnt);
	    n = mmc_emmc->block_dev.block_erase(dev_mmc, blk, cnt);
	   #endif



	    #ifdef  MLO_FIRST_LOADER
	     n = mmc_emmc->block_dev.block_write(dev_mmc, blk, cnt, memaddr_emmc_mlo_bootloader_array);
	    #endif
	    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	    #ifdef APPLICATION
	     n = mmc_emmc->block_dev.block_write(dev_mmc, blk, cnt, memaddr_emmc_application_array);
	    #endif
	    ////////////////////////////////////////////////////////////////////////////////////////////////////////

	    #ifdef SLINFO_ENTRY_POINT
	     n = mmc_emmc->block_dev.block_write(dev_mmc, blk, cnt, &entry_point_application);
	    #endif

	    ///////////////////////////////////////////////////////////////////////////////////////////////////////

	    //printf("%d blocks written: %s\n", n, (n == cnt) ? "OK" : "ERROR");

	     //////////////////////////////////////////////ERASE FLASH
	     /*
	     printf("\nMMC erase: dev # %d, block # %d, count %d ...\n\r ",dev_mmc, blk, cnt);
	     n = mmc_emmc->block_dev.block_erase(dev_mmc, blk, cnt);
	     printf("%d blocks erased: %s\n", n, (n == cnt) ? "OK" : "ERROR");
	     */

	     //////////////////////////////////////////////READ from eMMC flash
	     /*
	     printf("\eMMC read: dev # %d, block # %d, count %d ... ",dev_mmc, blk, cnt);
	     n = mmc_emmc->block_dev.block_read(dev_mmc, blk, cnt, 0x90000000);
	     printf("%d blocks read: %s\n", n, (n == cnt) ? "OK" : "ERROR");
	     */

	    #endif

	    while(1)
	    {

	    }




}
