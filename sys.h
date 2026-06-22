/*
 * SMUSHv1 RE
 *
 * sys - system
 */

#ifndef _SMUSH_SYS_H_
#define _SMUSH_SYS_H_

#include "smush.h"

/* platform stuff */
void sys_timer_continue(void);
void sys_timer_init(int cfg_timerrate);			/* synthetic */
void sys_timer_terminate(void);				/* synthetic */

void sys_timer_handler_generic(void);			/* 0029c84d */
void sys_timer_animator_handler(void);			/* 0029c95e */
void sys_timer_continue_generic(void);			/* 0029ca44 */

void sys_timer_set_usagecap(uint8_t enabled);		/* 0029ca73 */


void sys_timer_more_cpuusage(void);
void sys_timer_less_cpuusage(void);


#endif
