/*
 * SMUSHv1 RE
 *
 * ctl - input devices
 */

#ifndef _SMUSH1_CTL_H_
#define _SMUSH1_CTL_H_


void ctl_init(void);
void ctl_terminate(void);

void ctl_joy_enable(void);
void ctl_joy_read(void);

void ctl_kbd_query(uint16_t *key_out);

void ctl_mou_center(void);
void ctl_mou_enable(void);

void ctl_pointer_query(int16_t *xpos, int16_t *ypos, int16_t *but1, int16_t *but2);	/* 0029b7ef */

#endif

