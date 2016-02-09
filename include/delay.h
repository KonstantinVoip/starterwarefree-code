/**
 * \file   delay.h
 *
 * \brief  This file contains the prototypes of the functions present in
 *         utils/src/delay.c
 */
#ifndef _DELAY_H_
#define _DELAY_H_

//Работа с Таймерами ARM3358 всего у нас 8 таймеров.
//таймер миллисекунд
extern  void syl_Sysdelay(unsigned int milliSec);
extern  void syl_dtimer2_init(void);


//7 Таймер это таймер производительности
extern void  inline  dmtimer7_SysPerfTimerSetup(void);
extern void inline  dmtimer7_PerfTimerStart(unsigned short flag);
extern unsigned int inline dmtimer7_PerfTimerStop(unsigned short flag);

/*Чтение и запись в конкретные регистры */
extern void  writel_to_reg(unsigned int val ,void * addr);
extern unsigned int  readl_from_reg(void * addr);


//таймер 2 микросекунды.
extern void syl_dtimer3_init();
extern void dmtimer3_2_microsecond (unsigned int microsec);


#endif


