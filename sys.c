/*
 * SMUSHv1 RE
 *
 * sys - system
 */

#include "smush.h"


/* 002b239e */
static const uint16_t sys_timer_abort_thresholds[9] = { 4, 3, 2, 3, 2, 2, 2, 2, 3 };

/* 002b23b0 */
static const uint16_t sys_timer_abort_slacks[9] = { 1, 1, 1, 2, 2, 3, 4, 5, 6 };


/* The central timer handler.
 * in the original DOS version, this handler is run in x86 irq context at
 * regular intervals, at 16x rate of what DOS sets.
 */
void sys_timer_handler_generic(void)
{
	if (AG(sys_timer_handler_disable_cntr) != 0)
		return;

	if ((AG(sys_animation_running_flag) == 0) || (AG(sys_abort_flag) == 0)) {
		AG(sys_timer_idle_cntr) += 1;
	}
	AG(sys_animation_tick_cntr) += 1;
	if (AG(sys_timer_idle_cntr) == 1) {
		if (AG(sys_timer_yielding) == 0) {
			AG(sys_timer_yielding) = 1;
#if 0
			push ds ; pop es
#endif
			AG(sys_abort_flag) = 0;
			sys_timer_animator_handler();
			AG(sys_timer_yielding) = 1;
		}
	} else {
		int v = (AG(sys_timer_abort_toggle) + AG(sys_cpu_usagecap_val)) >> 1;
		if (AG(sys_timer_idle_cntr) == sys_timer_abort_thresholds[v]) {
			AG(sys_abort_flag) += 1;
		} else if (sys_timer_abort_slacks[v] <= AG(sys_timer_idle_cntr)) {
			AG(sys_timer_idle_cntr) = 0;
			AG(sys_timer_abort_toggle) ^= 1;
		}
	}
}

void sys_timer_animator_handler(void)
{
	int res = setjmp(AG(sys_jmpbuf_2));
	if (res != 0) {
		if (AG(sys_saved_flag) != 0) {
			AG(sys_saved_flag) = 0;
			AG(sys_animation_running_flag) = 1;
			longjmp(AG(sys_jmpbuf_1), 1);
		}
		if (AG(sys_timer_fps_tickthres) <= AG(sys_animation_tick_cntr)) {
			/* enough ticks for 1 frame at fps have elapsed */
			AG(sys_animation_tick_cntr) = 0;
#if 0
			/* DOS: switch from IRQ stack to App stack. DS has already
			 * been set in the sys_timer_handler().
			 */
			mov dx,ds; mov eax,0x002b9f6e; mov ss,dx; mov esp,eax
#endif
			AG(sys_saved_flag) = 0;
			AG(sys_animation_running_flag) = 1;
			ctl_joy_read();
			AG(anm_animator_result) = anm_animator_render(0, 0, AG(game_anm_flags_initial));
			if ((AG(anm_animator_result) == 0) && (AG(anm_animator_result_prev) == 0) &&
			    (AG(sys_timer_cpu_usagecap_is_on) != 0) && (AG(sys_cfg_cddrvusagecap_set) == 0) &&
			    (AG(sys_cpu_usagecap_val) < 16)) {
				AG(sys_cpu_usagecap_val) += 1;
			}
			AG(sys_animation_running_flag) = 0;
			AG(anm_animator_result_prev) = AG(anm_animator_result);
			longjmp(AG(sys_jmpbuf_2), 2);
		}
	}
}

void sys_timer_continue_generic(void)
{
	if (AG(sys_abort_flag) != 0) {
		int ret = setjmp(AG(sys_jmpbuf_1));
		if (ret == 0) {
			AG(sys_animation_running_flag) = 0;
			AG(sys_saved_flag) = 1;
			longjmp(AG(sys_jmpbuf_2), 1);
		}
	}
}

void sys_timer_set_usagecap(uint8_t enabled)
{
	if (enabled)
		AG(sys_cpu_usagecap_val) = AG(sys_cfg_cddrvusage);
	AG(sys_timer_cpu_usagecap_is_on) = 1;
}

/* FIXME: better name? */
void sys_timer_more_cpuusage(void)
{
	AG(sys_timer_handler_disable_cntr) += 1;
}

/* FIXME: better name? */
void sys_timer_less_cpuusage(void)
{
	AG(sys_timer_handler_disable_cntr) -= 1;
}

