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
        //Память Программ
        DDR_MEM_0        : org = 0x80000000  len = 0x10000000                    // 256 Mb RAM
        //Здесь будут лежать наши таблицы для ТФА и СУЛ
        DDR_MEM_1        : org = 0x90000000  len = 0x10000000                    // 256 Mb RAM
        //Память и КЭШ
        SRAM		   : org = 0x402F0400    len = 0x0000FC00           // 64kB internal SRAM very fast
        L3OCMC0		   : org = 0x40300000    len = 0x00010000           // 64kB L3 OCMC SRAM  L3 interconnect
}
*/


/*Этот файл разбиения Памяти Предназначен для работы без таблиц СУЛ и ТУЭР  */
MEMORY
{
        //Память Программ
        DDR_MEM_0        : org = 0x80000000  len = 0x00040000                    // 128 K/bait
        //Здесь будут лежать наш образ приложения.CONST
        DDR_MEM_1        : org = 0x80040000  len = 0x00040000                    // 128 K/bait
        //Здесь будут лежать таблицы в будущем 64 М/байт
       // DDR_MEM_3      : org = 0x80020000  len = 0x00020000                  // 128 K/bait
        //Память и КЭШ буду использовать в будущем
       // SRAM		     : org = 0x402F0400    len = 0x0000FC00                   // 64kB internal SRAM very fast
       // L3OCMC0		 : org = 0x40300000    len = 0x00010000               // 64kB L3 OCMC SRAM  L3 interconnect
        //Таблицы ППМ для прошифки блоков
        PPM_TAB1_DDR     : org = 0x81000000  len = 0x01000000                 //таблица 1  таблица длинной 16 М/байт
        PPM_TAB2_DDR     : org = 0x82000000  len = 0x01000000                 //таблица 2  таблица длинной 16 М/байт
        PPM_TAB3_DDR     : org = 0x83000000  len = 0x01000000                 //таблица 3  таблица длинной 16 М/байт
        PPM_TAB4_DDR     : org = 0x84000000  len = 0x01000000                 //таблица 4  таблица длинной 16 М/байт
}




SECTIONS
{
    .text    : load >  DDR_MEM_0 /*Исполняемый программный код*/
  //  .text	  :Entry : load > 0x80000000
    .cinit   : load >  DDR_MEM_0  /*Код инициализации Глобальных и Статических Переменных*/
    .switch  : load >  DDR_MEM_0  /* switch table info         */
    .sysmem  : load >  DDR_MEM_0  /*Динамическая память (куча)*/
    .stack   : load >  DDR_MEM_0  /*Область стека*/
    /*************************************************/
    .data    : load >  DDR_MEM_0  /* initialized data  */
    .bss     : load >  DDR_MEM_0  /*Глобальные и статические Переменные проинициализированные*/
     RUN_START(bss_start)
     RUN_END(bss_end)
    .const   : load >  DDR_MEM_1  /*Глобальные и Статические Константы Проинициализированные*/
    //.const   : load >  DDR_MEM_0
    .cio     : load >  DDR_MEM_0  /* C I/O             */

   //Отдельная Секция здесь расположена таблица для прошифки application
    application  : load > DDR_MEM_1  //
   //Отдельные секции для прошифки таблиц ППМ
   //------------------------------------------------------------------------------------------
       //размещаем таблицы для каждого канала.
    //Отдельная Секция здесь расположена таблица для прошифки application
    ppm_tab1  : load > PPM_TAB1_DDR  //1 -е 16 М/байт
    ppm_tab2  : load > PPM_TAB2_DDR  //2 -е 16 М/байт
    ppm_tab3  : load > PPM_TAB3_DDR  //3 -е 16 М/байт
    ppm_tab4  : load > PPM_TAB4_DDR  //4 -е 16 М/байт

   //------------------------------------------------------------------------------------------



   //------------------------------------------------------------------------------------------
















   //my_sect  : load > DDR_MEM_1  //add my_sections for table

}
