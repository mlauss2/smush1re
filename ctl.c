/*
 * SMUSHv1 RE
 *
 * ctl - input devices
 */

#include "smush.h"

void ctl_joy_update_axes(uint16_t *a0, uint16_t *a1, uint16_t *a2, uint16_t *a3,
			 uint16_t *a4, uint16_t *a5, uint16_t *a6, uint16_t *a7)
{
	ctl_joy_plat_update_axes(a0, a1, a2, a3, a4, a5, a6, a7);
}

void ctl_joy_enable(void)
{
	AG(ctl_mou_enabled) = 0;
}

void ctl_mou_enable(void)
{
	AG(ctl_joy_enabled) = 0;
}

void ctl_init(void)
{
	ctl_init_platform(AG(ctl_mou_enabled), AG(ctl_joy_enabled));

}

void ctl_terminate(void)
{
	ctl_terminate_platform();
}

void ctl_joy_center(void)
{
	if (AG(ctl_joy_enabled) == 0) {
		AG(ctl_j_centy) = 128;
		AG(ctl_j_centx) = 128;
	} else {
		AG(ctl_j_centx) = AG(ctl_j_axis0);
		AG(ctl_j_centy) = AG(ctl_j_axis1);
	}
	AG(ctl_j_scalex1) =  1500;
	AG(ctl_j_scalex2) =  1500;
	AG(ctl_j_scaley1) =  1500;
	AG(ctl_j_scaley2) =  1500;
}

void ctl_joy_read(void)
{
	/* TODO */
}

void ctl_mouse_reset(void)
{
	AG(ctl_mouse_xpos) = 0;
	AG(ctl_mouse_ypos) = 0;
}

uint8_t ctl_joy_calibrate_start(void)
{
	AG(ctl_j_minx) = 30000;
	AG(ctl_j_miny) = 30000;
	AG(ctl_j_maxx) = 0;
	AG(ctl_j_maxy) = 0;
	AG(ctl_j_scalex1) = 0x5dc;
	AG(ctl_j_scalex2) = 0x5dc;
	AG(ctl_j_scaley1) = 0x5dc;
	AG(ctl_j_scaley2) = 0x5dc;
	AG(ctl_j_calibrating) = 0xff;
	return AG(ctl_joy_enabled);
}

void ctl_joy_calibrate_end(void)
{
	AG(ctl_j_scalex1) = AG(ctl_j_centx) - AG(ctl_j_minx);
	if (AG(ctl_j_centx) - AG(ctl_j_minx) < 0x32) {
		AG(ctl_j_scalex1) = 0x32;
	}
	AG(ctl_j_scalex2) = AG(ctl_j_maxx) - AG(ctl_j_centx);
	if (AG(ctl_j_maxx) - AG(ctl_j_centx) < 0x32) {
		AG(ctl_j_scalex2) = 0x32;
	}
	AG(ctl_j_scaley1) = AG(ctl_j_centy) - AG(ctl_j_miny);
	if (AG(ctl_j_centy) - AG(ctl_j_miny) < 0x32) {
		AG(ctl_j_scaley1) = 0x32;
	}
	AG(ctl_j_scaley2) = AG(ctl_j_maxy) - AG(ctl_j_centy);
	if (AG(ctl_j_maxy) - AG(ctl_j_centy) < 0x32) {
		AG(ctl_j_scaley2) = 0x32;
	}
	AG(ctl_j_calibrating) = 0;
}

void ctl_pointer_query(int16_t *xpos_out, int16_t *ypos_out, int16_t *button1_out, int16_t *button2_out)
{
	*xpos_out = 320/2;
	*ypos_out = 200/2;
	*button1_out = 0;
	*button2_out = 0;
	return;
	if (AG(ctl_input_updates_locked) != 0) {
		return;
	}

	if (AG(ctl_mou_enabled) == 0) {
		AG(ctl_input_mouse_xpos) = 0;
		AG(ctl_input_mouse_ypos) = 0;
	} else {
		int16_t my = (AG(ctl_mousedrv_ypos) + AG(ctl_input_mouse_ypos) - 320) >> 2;
		AG(ctl_input_mouse_ypos) = my;
		int16_t mx = (AG(ctl_mousedrv_xpos) + AG(ctl_input_mouse_xpos) - 100) >> 1;
		AG(ctl_input_mouse_xpos) = mx;

		if (mx <= 100) {
			int16_t last_x = AG(ctl_input_mouse_xpos_last);
			int16_t curr_y = AG(ctl_input_mouse_ypos);
			int16_t last_y = AG(ctl_input_mouse_ypos_last);
			int16_t curr_x = AG(ctl_input_mouse_xpos);

			/* mouse movement deadzone check */
			if (curr_y > last_x + 0x14 || curr_x > last_y + 0x14 ||
				curr_y < last_x - 0x14 || curr_x < last_y - 0x14) {

				AG(ctl_input_mouse_ypos) = AG(ctl_input_mouse_xpos_last);
			AG(ctl_input_mouse_xpos) = AG(ctl_input_mouse_ypos_last);
			AG(ctl_mou_pos_changed) = 1;
				} else {
					AG(ctl_mou_pos_changed) = 0;
				}
		} else {
			AG(ctl_input_mouse_ypos) = AG(ctl_input_mouse_xpos_last);
			AG(ctl_input_mouse_xpos) = AG(ctl_input_mouse_ypos_last);
			AG(ctl_mou_pos_changed) = 1;
		}

		/* boundary clamping */
		int16_t val_y = AG(ctl_mousedrv_ypos) + AG(ctl_input_mouse_ypos);
		if (val_y > 448) {
			AG(ctl_input_mouse_ypos) = 448 - AG(ctl_mousedrv_ypos);
		} else if (val_y < 0xC0) {
			AG(ctl_input_mouse_ypos) = 0xC0 - AG(ctl_mousedrv_ypos);
		}

		int16_t val_x = AG(ctl_mousedrv_xpos) + AG(ctl_input_mouse_xpos);
		if (val_x > 0xE4) {
			AG(ctl_input_mouse_xpos) = 0xE4 - AG(ctl_mousedrv_xpos);
		} else if (val_x < -0x1C) {
			AG(ctl_input_mouse_xpos) = -0x1C - AG(ctl_mousedrv_xpos);
		}

		/* drift application */
		if (AG(ctl_mousedrv_in_handler) != 0) {
			int16_t cur_y = AG(ctl_mousedrv_ypos) + AG(ctl_input_mouse_ypos);
			if (cur_y > 324)
				AG(ctl_input_mouse_ypos) -= 4;
			else if (cur_y > 321)
				AG(ctl_input_mouse_ypos) -= 1;
			else if (cur_y < 316)
				AG(ctl_input_mouse_ypos) += 4;
			else if (cur_y < 319)
				AG(ctl_input_mouse_ypos) += 1;

			int16_t cur_x = AG(ctl_mousedrv_xpos) + AG(ctl_input_mouse_xpos);
			if (cur_x > 104)
				AG(ctl_input_mouse_xpos) -= 4;
			else if (cur_x > 101)
				AG(ctl_input_mouse_xpos) -= 1;
			else if (cur_x < 100)
				AG(ctl_input_mouse_xpos) += 4;
			else if (cur_x < 99)
				AG(ctl_input_mouse_xpos) += 1;
		} else {
			AG(ctl_mousedrv_in_handler) = 0;
		}

		AG(ctl_input_mouse_xpos_last) = AG(ctl_input_mouse_ypos);
		AG(ctl_input_mouse_ypos_last) = AG(ctl_input_mouse_xpos);
	}

	int16_t jx = AG(ctl_j_axis0);
	if (jx < AG(ctl_j_minx))
		AG(ctl_j_minx) = jx;
	if (jx > AG(ctl_j_maxx))
		AG(ctl_j_maxx) = jx;

	int16_t jy = AG(ctl_j_axis1);
	if (jy < AG(ctl_j_miny))
		AG(ctl_j_miny) = jy;
	if (jy > AG(ctl_j_maxy))
		AG(ctl_j_maxy) = jy;

	*xpos_out = AG(ctl_j_axis0) - AG(ctl_j_centx);
	*ypos_out = AG(ctl_j_axis1) - AG(ctl_j_centy);

	if (AG(ctl_j_axis0) <= AG(ctl_j_centx)) {
		*xpos_out = (int16_t)(((int32_t)(*xpos_out) << 7) / AG(ctl_j_scalex1));
	} else {
		*xpos_out = (int16_t)(((int32_t)(*xpos_out) << 7) / AG(ctl_j_scalex2));
	}
	*xpos_out += AG(ctl_input_mouse_ypos);

	if (AG(ctl_j_axis1) <= AG(ctl_j_centy)) {
		*ypos_out = (int16_t)(((int32_t)(*ypos_out) << 7) / AG(ctl_j_scaley1));
	} else {
		*ypos_out = (int16_t)(((int32_t)(*ypos_out) << 7) / AG(ctl_j_scaley2));
	}
	*ypos_out += AG(ctl_input_mouse_xpos);

	*button1_out = AG(ctl_j_axis2) | AG(ctl_mousedrv_button1);
	*button2_out = AG(ctl_j_axis3) | AG(ctl_mousedrv_button2);
}

