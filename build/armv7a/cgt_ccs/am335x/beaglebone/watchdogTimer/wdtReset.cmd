/*
 * arm_skd_syl.cmd
 *
 * Linker command file.
 *
 * Copyright (C) ZAO NPK_PELENGATOR Konstantin Shiluaev 2015
 *
*/

/****************************************************************************/
-stack  0x1000                             /* SOFTWARE STACK SIZE           */
-heap   0x2000                             /* HEAP AREA SIZE                */
-e Entry

/* Since we used 'Entry' as the entry-point symbol the compiler issues a    */
/* warning (#10063-D: entry-point symbol other than "_c_int00" specified:   */
/* "Entry"). The CCS Version (5.1.0.08000) stops building from command      */
/* line when there is a warning. So this warning is suppressed with the     */
/* below flag. */

--diag_suppress=10063
/*--retain=PRU_LED0_image.obj(*)*/
/*--retain=PRU_LED1_image.obj(*)*/
/*--retain=PRU_Switch_image.obj(*)*/
/*--retain=PRU_Audio_image.obj(*)*/
/*--retain=PRU_Hardware_UART_image.obj(*)*/
/*--retain=PRU_HDQ_TempSensor0_image.obj(*)*/
/*--retain=PRU_HDQ_TempSensor1_image.obj(*)*/


/* SPECIFY THE SYSTEM MEMORY MAP */





/*
MEMORY
{
        //������ ��������
        DDR_MEM_0        : org = 0x80000000  len = 0x10000000                    // 256 Mb RAM
        //����� ����� ������ ���� ������� ��� ��� � ���
        DDR_MEM_1        : org = 0x90000000  len = 0x10000000                    // 256 Mb RAM
        //������ � ���
        SRAM		   : org = 0x402F0400    len = 0x0000FC00           // 64kB internal SRAM very fast
        L3OCMC0		   : org = 0x40300000    len = 0x00010000           // 64kB L3 OCMC SRAM  L3 interconnect
}
*/


/*���� ���� ��������� ������ ������������ ��� ������ ��� ������ ��� � ����  */
MEMORY
{
        //������ ��������
        DDR_MEM_0        : org = 0x80000000  len = 0x00040000                    // 128 K/bait
        //����� ����� ������ ��� ����� ����������.CONST
        DDR_MEM_1        : org = 0x80040000  len = 0x00040000                    // 128 K/bait
        //����� ����� ������ ������� � ������� 64 �/����
       // DDR_MEM_3      : org = 0x80020000  len = 0x00020000                  // 128 K/bait
        //������ � ��� ���� ������������ � �������
       // SRAM		     : org = 0x402F0400    len = 0x0000FC00                   // 64kB internal SRAM very fast
       // L3OCMC0		 : org = 0x40300000    len = 0x00010000               // 64kB L3 OCMC SRAM  L3 interconnect
        //������� ��� ��� �������� ������
        PPM_TAB1_DDR     : org = 0x81000000  len = 0x01000000                 //������� 1  ������� ������� 16 �/����
        PPM_TAB2_DDR     : org = 0x82000000  len = 0x01000000                 //������� 2  ������� ������� 16 �/����
        PPM_TAB3_DDR     : org = 0x83000000  len = 0x01000000                 //������� 3  ������� ������� 16 �/����
        PPM_TAB4_DDR     : org = 0x84000000  len = 0x01000000                 //������� 4  ������� ������� 16 �/����
}




SECTIONS
{
    .text    : load >  DDR_MEM_0 /*����������� ����������� ���*/
  //  .text	  :Entry : load > 0x80000000
    .cinit   : load >  DDR_MEM_0  /*��� ������������� ���������� � ����������� ����������*/
    .switch  : load >  DDR_MEM_0  /* switch table info         */
    .sysmem  : load >  DDR_MEM_0  /*������������ ������ (����)*/
    .stack   : load >  DDR_MEM_0  /*������� �����*/
    /*************************************************/
    .data    : load >  DDR_MEM_0  /* initialized data  */
    .bss     : load >  DDR_MEM_0  /*���������� � ����������� ���������� ���������������������*/
     RUN_START(bss_start)
     RUN_END(bss_end)
    .const   : load >  DDR_MEM_1  /*���������� � ����������� ��������� ���������������������*/
    //.const   : load >  DDR_MEM_0
    .cio     : load >  DDR_MEM_0  /* C I/O             */

   //��������� ������ ����� ����������� ������� ��� �������� application
    application  : load > DDR_MEM_1  //
   //��������� ������ ��� �������� ������ ���
   //------------------------------------------------------------------------------------------
       //��������� ������� ��� ������� ������.
    //��������� ������ ����� ����������� ������� ��� �������� application
    ppm_tab1  : load > PPM_TAB1_DDR  //1 -� 16 �/����
    ppm_tab2  : load > PPM_TAB2_DDR  //2 -� 16 �/����
    ppm_tab3  : load > PPM_TAB3_DDR  //3 -� 16 �/����
    ppm_tab4  : load > PPM_TAB4_DDR  //4 -� 16 �/����

   //------------------------------------------------------------------------------------------



   //------------------------------------------------------------------------------------------
















   //my_sect  : load > DDR_MEM_1  //add my_sections for table

}
