/*
 * SMUSHv1 RE
 *
 * ctl - input devices
 */

#ifndef _SMUSH1_CTL_H_
#define _SMUSH1_CTL_H_

/* called by the ctl_init() function to give platform chance to initialize mouese
 * and joystick input.
 * DOS: call int 0x33, function 12 to set a callback at 0029b413
 * */
void ctl_init_platform(int mouse_en, int joy_en);
void ctl_terminate_platform(void);


/* Read the absolute values of 8 joystick axes: x1/y1/but11/but12 x2/y2/but12/but22
 * into ctl_joy_translated_axis_values[8]
 */
void ctl_joy_plat_update_axes(uint16_t *a0, uint16_t *a1, uint16_t *a2, uint16_t *a3,
			      uint16_t *a4, uint16_t *a5, uint16_t *a6, uint16_t *a7);


void ctl_joy_update_axes(uint16_t *a0, uint16_t *a1, uint16_t *a2, uint16_t *a3,
			 uint16_t *a4, uint16_t *a5, uint16_t *a6, uint16_t *a7);	/* 00299ee */

void ctl_joy_enable(void);			/* 0029b403 */
void ctl_mou_enable(void);			/* 0029b40b */
void ctl_init(void);				/* 0029b46b */
void ctl_terminate(void);			/* 0029b51f */
void ctl_joy_center(void);			/* 0029b54a */
void ctl_mouse_reset(void);			/* 0029b5a4 */
uint8_t ctl_joy_calibrate_start(void);		/* 0029b5b4 */
void ctl_joy_calibrate_end(void);		/* 0029b606 */
void ctl_pointer_query(int16_t *xpos, int16_t *ypos, int16_t *but1, int16_t *but2);	/* 0029b7ef */

void ctl_joy_read(void);

void ctl_kbd_query(uint16_t *key_out);

void ctl_mou_center(void);



/* DOS */
int16_t ctl_joy_x86_pc_poll_port(void);				/* 00291a6d */



#endif

